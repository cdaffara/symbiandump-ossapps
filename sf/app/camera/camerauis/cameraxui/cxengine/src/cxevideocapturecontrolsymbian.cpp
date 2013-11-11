/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

#include <algorithm>
#include <exception>
#include <QTime>
#include <QPixmap>
#include <coemain.h>
#include <QStringList>

#include "cxeexception.h"
#include "cxevideocapturecontrolsymbian.h"
#include "cxevideorecorderutilitysymbian.h"
#include "cxecameradevicecontrolsymbian.h"
#include "cxefilenamegeneratorsymbian.h"
#include "cxeerrormappingsymbian.h"
#include "cxeviewfindercontrol.h"
#include "cxesnapshotcontrol.h"
#include "cxestillimagesymbian.h"
#include "cxecameradevice.h"
#include "cxutils.h"
#include "cxestate.h"
#include "cxesettings.h"
#include "cxenamespace.h"
#include "cxesoundplayersymbian.h"
#include "cxequalitypresetssymbian.h"
#include "cxeviewfindercontrolsymbian.h"
#include "cxediskmonitor.h"
#include "cxesettingsmappersymbian.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxevideocapturecontrolsymbianTraces.h"
#endif


// constants
namespace
{
    // TMMFEvent UIDs for Async stop
    const TUid KCamCControllerCCVideoRecordStopped = {0x2000E546};
    const TUid KCamCControllerCCVideoFileComposed = {0x2000E547};

    const TInt64  KMinRequiredSpaceVideo      = 4000000;
}


/*!
* CxeVideoCaptureControlSymbian::CxeVideoCaptureControlSymbian
*/
CxeVideoCaptureControlSymbian::CxeVideoCaptureControlSymbian(
    CxeCameraDevice &cameraDevice,
    CxeViewfinderControl &viewfinderControl,
    CxeSnapshotControl &snapshotControl,
    CxeCameraDeviceControl &cameraDeviceControl,
    CxeFilenameGenerator &nameGenerator,
    CxeSettings &settings,
    CxeQualityPresets &qualityPresets,
    CxeDiskMonitor &diskMonitor)
    : CxeStateMachine("CxeVideoCaptureControlSymbian"),
      mVideoRecorder(NULL),
      mCameraDevice(cameraDevice),
      mCameraDeviceControl(cameraDeviceControl),
      mViewfinderControl(viewfinderControl),
      mSnapshotControl(snapshotControl),
      mFilenameGenerator(nameGenerator),
      mSettings(settings),
      mQualityPresets(qualityPresets),
      mDiskMonitor(diskMonitor),
      mSnapshot(),
      mVideoStartSoundPlayer(NULL),
      mVideoStopSoundPlayer(NULL),
      mCurrentFilename("")
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_CREATE_IN, "msg: e_CX_VIDEOCAPTURECONTROL_NEW 1");

    qRegisterMetaType<CxeVideoCaptureControl::State> ();
    initializeStates();

    mVideoStopSoundPlayer = new
             CxeSoundPlayerSymbian(CxeSoundPlayerSymbian::VideoCaptureStop,
                                   mSettings);
    mVideoStartSoundPlayer = new
             CxeSoundPlayerSymbian(CxeSoundPlayerSymbian::VideoCaptureStart,
                                   mSettings);

    // If camera is already allocated, call the slot ourselves.
    if (mCameraDevice.camera()) {
        handleCameraAllocated(CxeError::None);
    }

    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_CREATE_M1, "msg: e_CX_ENGINE_CONNECT_SIGNALS 1");
    // connect signals from cameraDevice, so we recieve events when camera reference changes
    connect(&cameraDevice, SIGNAL(prepareForCameraDelete()),
            this, SLOT(prepareForCameraDelete()));
    connect(&cameraDevice, SIGNAL(prepareForRelease()),
            this, SLOT(prepareForRelease()) );
    connect(&cameraDevice, SIGNAL(cameraAllocated(CxeError::Id)),
            this, SLOT(handleCameraAllocated(CxeError::Id)));

    // connect playing sound signals
    connect(mVideoStartSoundPlayer, SIGNAL(playComplete(int)),
            this, SLOT(handleSoundPlayed()));

    // connect snapshot ready signal
    connect(&mSnapshotControl, SIGNAL(snapshotReady(CxeError::Id, const QImage&)),
            this, SLOT(handleSnapshotReady(CxeError::Id, const QImage&)));

    // enabling setting change callbacks to videocapturecontrol
    connect(&mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            this, SLOT(handleSettingValueChanged(const QString&,QVariant)));

    mSettings.listenForSetting(CxeSettingIds::VIDEO_SCENE, this, SLOT(handleSceneChanged(const QVariant&)));

    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_CREATE_M2, "msg: e_CX_ENGINE_CONNECT_SIGNALS 0");

    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_CREATE_OUT, "msg: e_CX_VIDEOCAPTURECONTROL_NEW 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* CxeVideoCaptureControlSymbian::~CxeVideoCaptureControlSymbian()
*/
CxeVideoCaptureControlSymbian::~CxeVideoCaptureControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    releaseResources();
    mIcmSupportedVideoResolutions.clear();
    delete mVideoStartSoundPlayer;
    delete mVideoStopSoundPlayer;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Initializes resources for video recording.
*/
void CxeVideoCaptureControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_INIT, "msg: e_CX_VIDEO_CAPCONT_INIT 1" );

    if (state() == Idle) {
        // start initializing resources for video capture
        initVideoRecorder();
        // inform zoom control to prepare zoom.
        emit prepareZoomForVideo();
    } else if (state() == Initialized) {
        // video recorder already initalized. Continue to prepare video reocording.
        open();
    }

    OstTrace0( camerax_performance, DUP1_CXEVIDEOCAPTURECONTROLSYMBIAN_INIT, "msg: e_CX_VIDEO_CAPCONT_INIT 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Releases all resources
*/
void CxeVideoCaptureControlSymbian::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Nothing to do if already idle.
    if(state() != Idle) {
        OstTrace0( camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_DEINIT, "msg: e_CX_VIDEO_CAPCONT_DEINIT 1" );

        // first stop viewfinder
        mViewfinderControl.stop();

        // stop video-recording in-case if its ongoing.
        stop();

        // Check if state is stopping, in which case we have to inform the
        // file harvester that a file is to be completed. We would not
        // call harvestFile otherwise in this case.
        // Otherwise the video will not be found from videos app.
        if (state() == Stopping) {
            emit videoComposed(CxeError::None, mCurrentFilename);
        }

        mSnapshotControl.stop();

        if (mVideoRecorder) {
            mVideoRecorder->close();
        }

        setState(Idle);

        OstTrace0( camerax_performance, DUP1_CXEVIDEOCAPTURECONTROLSYMBIAN_DEINIT, "msg: e_CX_VIDEO_CAPCONT_DEINIT 0" );
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Intializes VideoRecorder for recording.
*/
void CxeVideoCaptureControlSymbian::initVideoRecorder()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Init needed only in Idle state
    if (state() == Idle) {
        try {
            // If video recorder is not yet created, do it now.
            createVideoRecorder();

            // Update current video quality details from ICM.
            // Throws an error if unable to get the quality.
            updateVideoCaptureParameters();

            // Video recorder is ready to open video file for recording.
            setState(Initialized);
            open();
        } catch (const std::exception &e) {
            // Handle error
            handlePrepareFailed();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Opens file for video recording.
*/
void CxeVideoCaptureControlSymbian::open()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Check valid state to start "open" operation
    if (state() == Initialized) {
        try {
            // generate video file name, if necessary
            generateFilename();
            CX_DEBUG(( "Next video file path: %s", mCurrentFilename.toAscii().constData() ));

            // Start preparing..
            setState(CxeVideoCaptureControl::Preparing);

            // Exception thrown if open fails.
            mVideoRecorder->open(mCameraDevice.camera()->Handle(),
                                 mCurrentFilename,
                                 mCurrentVideoDetails.mVideoFileMimeType,
                                 mCurrentVideoDetails.mPreferredSupplier,
                                 mCurrentVideoDetails.mVideoCodecMimeType,
                                 mCurrentVideoDetails.mAudioType);
        } catch (const std::exception &e) {
            handlePrepareFailed();
        }
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper method for generating filename.
* Throws exception, if file type mime is formatted wrong or
* filename generation fails.
*/
void CxeVideoCaptureControlSymbian::generateFilename()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCurrentFilename = QString("");

    QStringList list = mCurrentVideoDetails.mVideoFileMimeType.split("/");
    // Throw exception if mime string is formatted wrong.
    if (list.count() != 2) {
        throw new CxeException(CxeError::General);
    }
    QString fileExt = "." + list[1];

    // Generate new filename and open the file for writing video data
    CxeException::throwIfError(mFilenameGenerator.generateFilename(mCurrentFilename, fileExt));

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Prepare Video Recorder with necessary settings for video capture.
*/
void CxeVideoCaptureControlSymbian::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Preparing) {
        // not valid state to continue prepare.
        return;
    }

    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROL_PREPARE_1, "msg: e_CX_VIDCAPCONT_PREPARE 1");
    QSize frameSize(mCurrentVideoDetails.mWidth, mCurrentVideoDetails.mHeight);

    bool muteSetting = mSettings.get<bool>(CxeSettingIds::VIDEO_MUTE_SETTING, false);

    // Check if scene defines frame rate.
    // Use generic frame rate defined in video details, if no value is set in scene.
    int frameRate = mSettings.get<int>(CxeSettingIds::FRAME_RATE, 0);
    if (frameRate <= 0) {
        frameRate = mCurrentVideoDetails.mVideoFrameRate;
    }

    CX_DEBUG(("Video resolution (%d,%d)", mCurrentVideoDetails.mWidth,
                                           mCurrentVideoDetails.mHeight));
    CX_DEBUG(("Video bitrate = %d)", mCurrentVideoDetails.mVideoBitRate));
    CX_DEBUG(("Video frame rate = %d)", frameRate));

    try {
        mVideoRecorder->setVideoFrameSize(frameSize);
        mVideoRecorder->setVideoFrameRate(frameRate);
        mVideoRecorder->setVideoBitRate(mCurrentVideoDetails.mVideoBitRate);
        mVideoRecorder->setAudioEnabled(!muteSetting);
        // "No limit" value is handled in video recorder wrapper.
        mVideoRecorder->setVideoMaxSize(mCurrentVideoDetails.mMaximumSizeInBytes);

        // Settings have been applied successfully, start to prepare.
        mVideoRecorder->prepare();

        // Inform client
        emit videoPrepareComplete(CxeError::None);
    } catch (const std::exception &e) {
        // Handle error.
        handlePrepareFailed();
    }

    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROL_PREPARE_2, "msg: e_CX_VIDCAPCONT_PREPARE 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Prepare video snapshot.
* Throws exception on error.
*/
void CxeVideoCaptureControlSymbian::prepareSnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXEVIDEOCAPTURECONTROL_PREPARESNAP_1, "msg: e_CX_PREPARE_SNAPSHOT 1" );

    // Prepare snapshot. Snapshot control throws error if problems.
    QSize snapshotSize = mSnapshotControl.calculateSnapshotSize(
                            mViewfinderControl.deviceDisplayResolution(),
                            mCurrentVideoDetails.mAspectRatio);
    mSnapshotControl.start(snapshotSize);

    OstTrace0( camerax_performance, CXEVIDEOCAPTURECONTROL_PREPARESNAP_2, "msg: e_CX_PREPARE_SNAPSHOT 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
  Fetch video quality details based on current video quality setting.
*/
void CxeVideoCaptureControlSymbian::updateVideoCaptureParameters()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROL_UPDATEVIDEOCAPTUREPARAMETERS_1, "msg: e_CX_UPDATE_VIDEO_CAPTURE_PARAMETERS 1");

    int quality = 0;

    // Get quality index for primary camera. Only one quality for secondary camera.
    if (mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex) {
        quality = mSettings.get<int>(CxeSettingIds::VIDEO_QUALITY);
    }

    if (quality < 0 || quality >= mIcmSupportedVideoResolutions.count()) {
       throw new CxeException(CxeError::NotFound);
    }

    // Get video quality details
    mCurrentVideoDetails = mIcmSupportedVideoResolutions.at(quality);

    OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROL_UPDATEVIDEOCAPTUREPARAMETERS_2, "msg: e_CX_UPDATE_VIDEO_CAPTURE_PARAMETERS 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Resets the video snapshot and current video filename
*/
void CxeVideoCaptureControlSymbian::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Snapshot will consume considerably memory.
    // Replace it with null pixmap to have it freed.
    mSnapshot = QPixmap();
    // reset the current file name.
    mCurrentFilename = QString("");

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Returns current video filename
*/
QString CxeVideoCaptureControlSymbian::filename() const
{
    // Simply return the current contents of mCurrentFilename.
    // If video recording was started then it returns proper filename
    // otherwise an empty string is returned.
    return mCurrentFilename;
}

/*!
* Returns current video snapshot
*/
QPixmap CxeVideoCaptureControlSymbian::snapshot() const
{
    return mSnapshot;
}

/*!
* Starts video recording if we are in appropriate state.
*/
void CxeVideoCaptureControlSymbian::record()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Ready || state() == Paused) {
        // ask the player to play the sound
        // recording will start once start sound is played
        setState(CxeVideoCaptureControl::PlayingStartSound);
        mVideoStartSoundPlayer->play();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Pauses video recording.
*/
void CxeVideoCaptureControlSymbian::pause()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Recording) {
        try {
            mVideoRecorder->pause();
            setState(CxeVideoCaptureControl::Paused);
            // play the sound, but not changing the state
            mVideoStopSoundPlayer->play();
        } catch (const std::exception &e) {
            handleComposeFailed(qt_symbian_exception2Error(e));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Stops video recording.
*/
void CxeVideoCaptureControlSymbian::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Recording || state() == Paused) {
        // first stop viewfinder
        mViewfinderControl.stop();

        try {
            // Try asynchronous stopping first.
            mVideoRecorder->stop(true);
            // No error from asynchronous stop -> wait for stop event
            if (state() != Ready) {
                setState(Stopping);
            }
        } catch (const std::exception &e) {
            CX_DEBUG(("CxeVideoCaptureControlSymbian - async stop failed, try sync.."));
            try {
                mVideoRecorder->stop(false);
                // stopping went ok
                emulateNormalStopping();
            } catch (const std::exception &e) {
                // Even synchronous stopping failed -> release resources.
                CX_DEBUG(("CxeVideoCaptureControlSymbian - sync stop failed, too!"));
                handleComposeFailed(qt_symbian_exception2Error(e));
            }
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Callback when "Open" operation is complete.
*/
void CxeVideoCaptureControlSymbian::MvruoOpenComplete(TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeVideoCaptureControlSymbian::MvruoOpenComplete, err=%d", aError));

    if (state() == Preparing) {
        if (!aError) {
            prepare();
        } else {
            handlePrepareFailed();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Callback when "Prepare" request is complete.
*/
void CxeVideoCaptureControlSymbian::MvruoPrepareComplete(TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeVideoCaptureControlSymbian::MvruoPrepareComplete, err=%d", aError));

    if (state() == Preparing) {
        try {
            // Check that no error coming in.
            CxeException::throwIfError(aError);
            // Start viewfinder
            mViewfinderControl.start();
            // Prepare snapshot (throws exception if fails).
            prepareSnapshot();
            // Ready for recording now.
            setState(CxeVideoCaptureControl::Ready);
            OstTrace0( camerax_performance, CXEVIDEOCAPTURECONTROLSYMBIAN_GOTOVIDEO, "msg: e_CX_GO_TO_VIDEO_MODE 0" );
        } catch (const std::exception &e) {
            handlePrepareFailed();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Callback when "Record" operation is complete.
*/
void CxeVideoCaptureControlSymbian::MvruoRecordComplete(TInt aError)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("CxeVideoCaptureControlSymbian::MvruoRecordComplete, err=%d", aError));

    //! async stop customcommand stuff
    if (aError == KErrNone) {
        setState(CxeVideoCaptureControl::Ready);
    } else if (aError == KErrCompletion) {
        // KErrCompletion is received when video recording stops
        // because of maximum clip size is reached. Emulate
        // normal stopping.
        emulateNormalStopping();
    }
    else {
        // error during recording, report to client
        handleComposeFailed(aError);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Callback from MVideoRecorderUtilityObserver
*/
void CxeVideoCaptureControlSymbian::MvruoEvent(const TMMFEvent& aEvent)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (aEvent.iEventType.iUid == KCamCControllerCCVideoRecordStopped.iUid) {
        CX_DEBUG(("KCamCControllerCCVideoRecordStopped"));
        // play the sound, but not changing the state
        mVideoStopSoundPlayer->play();
    } else if (aEvent.iEventType.iUid == KCamCControllerCCVideoFileComposed.iUid) {
        CX_DEBUG(("KCamCControllerCCVideoFileComposed"));
        if (state() == Stopping) {
            // stop operation went fine, set back the state to intialized.
            setState(Initialized);
        }
        mFilenameGenerator.raiseCounterValue();
        // video file has composed, everything went well, inform the client
        emit videoComposed(CxeError::None, filename());
    } else {
        CX_DEBUG(("ignoring unknown MvruoEvent 0x%08x", aEvent.iEventType.iUid ));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* camera reference changing, release resources
*/
void CxeVideoCaptureControlSymbian::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();
    releaseResources();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* prepare for camera release.
*/
void CxeVideoCaptureControlSymbian::prepareForRelease()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* new camera available
*/
void CxeVideoCaptureControlSymbian::handleCameraAllocated(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (!error) {
        try {
            // Create the video recorder utility
            createVideoRecorder();
        } catch (...) {
            // We are just trying to create the recorder early.
            // Retry later when preparing, and fail then if
            // error still persists.
        }
        // new camera available, read supported video qualities from icm
        // load all video qualities supported by icm
        mIcmSupportedVideoResolutions.clear();
        Cxe::CameraIndex cameraIndex = mCameraDeviceControl.cameraIndex();
        // get list of supported image qualities based on camera index
        mIcmSupportedVideoResolutions =
                mQualityPresets.videoQualityPresets(cameraIndex);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Initializes video recorder.
* May throw exception.
*/
void CxeVideoCaptureControlSymbian::createVideoRecorder()
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mVideoRecorder == NULL) {
        mVideoRecorder = new CxeVideoRecorderUtilitySymbian(*this);
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* releases resources used by video capture control
*/
void CxeVideoCaptureControlSymbian::releaseResources()
{
    CX_DEBUG_ENTER_FUNCTION();

    // first de-init videocapture control
    deinit();
    reset();

    delete mVideoRecorder;
    mVideoRecorder = NULL;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Returns current state of video capture control
*/
CxeVideoCaptureControl::State CxeVideoCaptureControlSymbian::state() const
{
    return static_cast<CxeVideoCaptureControl::State> (stateId());
}

/*!
* Called when state is changed.
*/
void CxeVideoCaptureControlSymbian::handleStateChanged(int newStateId, CxeError::Id error)
{
    switch (newStateId) {
    case Ready:
        if (error == CxeError::None && !mDiskMonitor.isMonitoring()) {
            mDiskMonitor.start();
            connect(&mDiskMonitor, SIGNAL(diskSpaceChanged()), this, SLOT(handleDiskSpaceChanged()));
        }
        break;
    default:
        // Stop monitoring when video mode is released.
        // Same goes during recording, as video times come from recorder.
        if (mDiskMonitor.isMonitoring()) {
            mDiskMonitor.stop();
            disconnect(&mDiskMonitor, SIGNAL(diskSpaceChanged()), this, SLOT(handleDiskSpaceChanged()));
        }
        break;
    }
    emit stateChanged(static_cast<State> (newStateId), error);
}

/*!
* Initialize states for videocapturecontrol
*/
void CxeVideoCaptureControlSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState(Idle, "Idle", Initialized));
    addState(new CxeState(Initialized, "Initialized", Preparing | Idle));
    addState(new CxeState(Preparing, "Preparing", Ready | Idle));
    addState(new CxeState(Ready, "Ready", Recording | PlayingStartSound | Preparing | Idle));
    addState(new CxeState(Recording, "Recording", Recording | Paused | Stopping | Idle | Ready));
    addState(new CxeState(Paused, "Paused", Recording | Stopping | PlayingStartSound | Idle));
    addState(new CxeState(Stopping, "Stopping", Initialized | Idle | Ready));
    addState(new CxeState(PlayingStartSound, "PlayingStartSound", Recording | Idle));

    setInitialState(Idle);
}

/*!
* Updates remaining video recordng time counter to all the video qualities supported by ICM
* this should be done whenever storage location setting changes and when values are
* read from ICM for the first time
*/
void CxeVideoCaptureControlSymbian::updateRemainingRecordingTimeCounter()
{
    CX_DEBUG_ENTER_FUNCTION();

    for( int index = 0; index < mIcmSupportedVideoResolutions.count(); index++) {
        CxeVideoDetails &qualityDetails = mIcmSupportedVideoResolutions[index];
        qualityDetails.mRemainingTime = calculateRemainingTime(qualityDetails);
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* calculates remaining video recording time.
*/
void CxeVideoCaptureControlSymbian::remainingTime(int &time)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == CxeVideoCaptureControl::Recording ||
        state() == CxeVideoCaptureControl::Paused) {
        time = mVideoRecorder->availableRecordingTime();
        CX_DEBUG(("CxeVideoCaptureControlSymbian - time remaining: %d", time));
    } else {
        // Check if we need to recalculate the remaining time.
        if (mCurrentVideoDetails.mRemainingTime == CxeVideoDetails::UNKNOWN) {
            mCurrentVideoDetails.mRemainingTime = calculateRemainingTime(mCurrentVideoDetails);
        }
        time = mCurrentVideoDetails.mRemainingTime;
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Get the remaining recording time
* @param videoDetails Contains the current video resolution that is in use.
* @return The remaining recording time
*/
int CxeVideoCaptureControlSymbian::calculateRemainingTime(const CxeVideoDetails& videoDetails)
{
    CX_DEBUG_ENTER_FUNCTION();
    qint64 availableSpace = mDiskMonitor.free() - KMinRequiredSpaceVideo;
    int time = mQualityPresets.recordingTimeAvailable(videoDetails, availableSpace);
    CX_DEBUG_EXIT_FUNCTION();
    return time;
}

/*!
* Calculates elapsed recording time during video recording
* @return Did fetching elapsed time succeed.
*/
bool CxeVideoCaptureControlSymbian::elapsedTime(int &time)
{
    CX_DEBUG_ENTER_FUNCTION();

    bool ok = false;
    if (state() == CxeVideoCaptureControl::Recording ||
        state() == CxeVideoCaptureControl::Paused) {
        try {
            time = mVideoRecorder->duration();
            CX_DEBUG(("CxeVideoCaptureControlSymbian - elapsed: %d", time));
            ok = true;
        } catch (const std::exception &e) {
            // Returning false.
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
    return ok;
}

/*!
* slot called when playing a sound has finished.
*/
void CxeVideoCaptureControlSymbian::handleSoundPlayed()
{
    CX_DEBUG_ENTER_FUNCTION();

    // start recording, if we were playing capture sound
    if (state() == CxeVideoCaptureControl::PlayingStartSound) {
        setState(CxeVideoCaptureControl::Recording);
        mVideoRecorder->record();
    }

    // in case of video capture stop sound playing, nothing needs to be done
    // meaning the state set elsewhere, and the video capture has been stopped already

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle new snapshot.
* @param status Status code for getting the snapshot.
* @param snapshot Snapshot pixmap. Empty if error code reported.
*/
void CxeVideoCaptureControlSymbian::handleSnapshotReady(CxeError::Id status, const QImage &snapshot)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCameraDeviceControl.mode() == Cxe::VideoMode) {
        OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROL_HANDLESNAPSHOT_1, "msg: e_CX_HANDLE_SNAPSHOT 1");

        // Need to store snapshot for ui to be able to get it also later.
        mSnapshot = QPixmap::fromImage(snapshot);
        emit snapshotReady(status, snapshot, filename());

        OstTrace0(camerax_performance, CXEVIDEOCAPTURECONTROL_HANDLESNAPSHOT_2, "msg: e_CX_HANDLE_SNAPSHOT 0");
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* setting has changed, check if we are interested.
*/
void CxeVideoCaptureControlSymbian::handleSettingValueChanged(const QString& settingId,
                                                              QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(newValue);

    if (mCameraDeviceControl.mode() == Cxe::VideoMode) {
        if (settingId == CxeSettingIds::VIDEO_QUALITY) {
            // re-prepare for video
            if (state() == Ready) {
                // release resources
                deinit();
                // initialize video recording again
                init();
            }
        } else if (settingId == CxeSettingIds::VIDEO_MUTE_SETTING) {
            // mute setting changed, apply the new setting and re-prepare.
            setState(Preparing);
            prepare();
        } else if (settingId == CxeSettingIds::FRAME_RATE) {
            // Frame rate setting changed. Need to re-prepare if we are prepared already.
            // Otherwise can wait for next init call.
            if (state() == Ready) {
                setState(Preparing);
                prepare();
            }
        } else {
            // Setting not relevant to video mode
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Scene mode changed. We need to know about it because frame rate
 * might have changed.
 */
void CxeVideoCaptureControlSymbian::handleSceneChanged(const QVariant& scene)
{
    Q_UNUSED(scene)
    CX_DEBUG_ENTER_FUNCTION();

    // make sure we are in video mode
    if (mCameraDeviceControl.mode() == Cxe::VideoMode) {
        // Frame rate setting might have changed so re-prepare.
        if (state() == Ready) {
            setState(Preparing);
            prepare();
        }

    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Disk space changed.
* Emit remaining time changed signal, if space change affects it.
*/
void CxeVideoCaptureControlSymbian::handleDiskSpaceChanged()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Ignore updates on preparing phase.
    if (state() == CxeVideoCaptureControl::Ready) {

        int time(calculateRemainingTime(mCurrentVideoDetails));

        if (time !=  mCurrentVideoDetails.mRemainingTime) {
            mCurrentVideoDetails.mRemainingTime = time;
            emit remainingTimeChanged();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
    Use ECam Use Case Hint Custom API to inform ECam of our intended use case
    before calling Reserve().
*/
void CxeVideoCaptureControlSymbian::hintUseCase()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Make sure ECam knows we're doing video recording so it can prepare
    // for the correct use case.
    if (mCameraDeviceControl.mode() == Cxe::VideoMode) {
        MCameraUseCaseHint *useCaseHintApi = mCameraDevice.useCaseHintApi();
        if (useCaseHintApi) {
            MCameraUseCaseHint::TVideoCodec codec =
                    MCameraUseCaseHint::ECodecUnknown;
            MCameraUseCaseHint::TVideoProfile profile =
                    MCameraUseCaseHint::EProfileUnknown;

            updateVideoCaptureParameters();
            CxeSettingsMapperSymbian::Map2UseCaseHintVideoParameters(
                    mCurrentVideoDetails, codec, profile);

            TSize resolution(mCurrentVideoDetails.mWidth,
                             mCurrentVideoDetails.mHeight);
            TRAP_IGNORE(useCaseHintApi->HintDirectVideoCaptureL(codec, profile,
                                                                resolution));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Returns QList of all supported video quality details based on the camera index
* (primary/secondary).
*/
QList<CxeVideoDetails> CxeVideoCaptureControlSymbian::supportedVideoQualities()
{
    // update the remaining time counters for each quality setting
    updateRemainingRecordingTimeCounter();
    return mIcmSupportedVideoResolutions;
}

/*!
* Helper method to handle error during preparing phase.
*/
void CxeVideoCaptureControlSymbian::handlePrepareFailed()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("[ERROR] Preparing video failed!"));
    // Cleanup
    deinit();
    // Inform client
    emit videoPrepareComplete(CxeError::InitializationFailed);
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Helper method to handle error from video composing.
* @param error Symbian error code.
*/
void CxeVideoCaptureControlSymbian::handleComposeFailed(int error)
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG(("[ERROR] Composing video failed!"));
    // Inform client
    emit videoComposed(CxeErrorHandlingSymbian::map(error), filename());
    // Cleanup
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Helper method to emulate video stopping events.
 */
void CxeVideoCaptureControlSymbian::emulateNormalStopping()
{
    CX_DEBUG_ENTER_FUNCTION();

    setState(Stopping);
    MvruoEvent(TMMFEvent(KCamCControllerCCVideoRecordStopped, KErrNone));
    MvruoEvent(TMMFEvent(KCamCControllerCCVideoFileComposed, KErrNone));

    CX_DEBUG_EXIT_FUNCTION();
}
// End of file
