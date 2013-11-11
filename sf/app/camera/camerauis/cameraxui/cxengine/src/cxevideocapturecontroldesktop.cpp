/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
#include <QTime>
#include <QPixmap>

#include "cxevideocapturecontroldesktop.h"
#include "cxeviewfindercontrol.h"
#include "cxutils.h"
#include "cxestate.h"
#include "cxenamespace.h"
#include "cxefilenamegeneratordesktop.h"
#include "cxecameradevicedesktop.h"
#include "cxeexception.h"

namespace
{
    // Unit is milliseconds
    static const int CXENGINE_ELAPSED_TIME_TIMEOUT = 1000; // 1 second
    static const int VIEWFINDER_START_TIMEOUT      =  500; // 0.5 second

    // Unit is seconds
    static const int CXENGINE_MAXIMUM_VIDEO_TIME = 90*60; // 90 minutes
}


/*!
* Constructor
*/
CxeVideoCaptureControlDesktop::CxeVideoCaptureControlDesktop(CxeCameraDeviceDesktop &cameraDevice,
                                                             CxeViewfinderControl &viewfinderControl,
                                                             CxeCameraDeviceControl &cameraDeviceControl,
                                                             CxeFilenameGenerator &nameGenerator,
                                                             CxeQualityPresets &qualityPresets) :
    mCameraDevice(cameraDevice),
    mCameraDeviceControl(cameraDeviceControl),
    mViewfinderControl(viewfinderControl),
    mFilenameGenerator(nameGenerator),
    mQualityPresets(qualityPresets),
    mSnapshot(),
    mNewFileName(""),
    mCurrentFilename(""),
    mState(Idle),
    mElapsedTime(0)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeVideoCaptureControl::State> ();
    initializeStates();
    setupElapseTimer();

    mViewFinderStartTimer.setSingleShot(true);
    mViewFinderStartTimer.setInterval(VIEWFINDER_START_TIMEOUT);
    connect(&mViewFinderStartTimer, SIGNAL(timeout()), this, SLOT(startViewfinder()));

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Destructor
*/
CxeVideoCaptureControlDesktop::~CxeVideoCaptureControlDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();

    releaseResources();

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Initializes resources for video recording.
*/
void CxeVideoCaptureControlDesktop::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Idle) {
        // start initializing resources for video capture
        initVideoRecorder();
    } else if (state() == Initialized) {
        // video recorder already initalized. Continue to prepare video reocording.
        open();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Releases all resources
*/
void CxeVideoCaptureControlDesktop::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Idle) {
        // nothing to do
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    mViewFinderStartTimer.stop();
    mViewfinderControl.stop();

    // stop video-recording in-case if its ongoing.
    stop();

    setState(Idle);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Intializes VideoRecorder for recording.
*/
void CxeVideoCaptureControlDesktop::initVideoRecorder()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Idle) {
        // not valid state to start "open" operation
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    // update current video quality details from icm
    CxeError::Id err = CxeError::None;

    setState(Initialized);
    open();

    if (err) {
        // In case of error
        emit videoPrepareComplete(err);
        deinit();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Setup the timer for elapsed time
*/
void CxeVideoCaptureControlDesktop::setupElapseTimer()
{
    mRecordElapseTimer.setSingleShot(false);
    mRecordElapseTimer.setInterval(CXENGINE_ELAPSED_TIME_TIMEOUT);
    connect(&mRecordElapseTimer, SIGNAL(timeout()), this, SLOT(handleElapseTimeout()));
}


/*!
* Opens file for video recording.
*/
void CxeVideoCaptureControlDesktop::open()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Initialized) {
        // not valid state to start "open" operation
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    CxeError::Id err = CxeError::None;

    // generate video file name, if necessary
    if (mNewFileName.isEmpty()) {
        QString fileExt = ".mp4";

        // Generate new filename and open the file for writing video data
        err = mFilenameGenerator.generateFilename(mNewFileName, fileExt);
        if (err == CxeError::None) {
            mCurrentFilename = mNewFileName;
        } else {
            // file name is not valid, re-initialize the value of current string
            // back to empty string
            mCurrentFilename = QString("");
        }
    }

    if (err) {
        // error occured.
        deinit();
        emit videoPrepareComplete(err);
    } else {
        setState(Preparing);
        prepare();
    }
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Prepare Video Recorder with necessary settings for video capture.
*/
void CxeVideoCaptureControlDesktop::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Preparing) {
        // not valid state to continue prepare.
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    int err = CxeError::None;

    if (!err) {
        // prepare snapshot
        err = prepareVideoSnapshot();
    }

    if (!err) {
        // prepare zoom only when there are no errors during prepare.
        emit prepareZoomForVideo();
    }

    // Start viewfinder with delay.
    mViewFinderStartTimer.start();
    setState(Ready);

    // emit video prepare status
    emit videoPrepareComplete(CxeError::None);


    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 Prepare snapshot
@Return symbian error code.
 */
int CxeVideoCaptureControlDesktop::prepareVideoSnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();


    int err = 0;
    // Whether or not we have postcapture on, we need the snapshot for Thumbnail Manager.

    mSnapshot = mCameraDevice.currentSnaphot();
    handleSnapshotEvent(CxeError::None);

    CX_DEBUG_EXIT_FUNCTION();

    return err;
}

/*!
* Camera events coming from ecam.
*/
void CxeVideoCaptureControlDesktop::handleCameraEvent(int eventUid, int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(eventUid);
    Q_UNUSED(error);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Handle Snapshot event from ecam
*/
void CxeVideoCaptureControlDesktop::handleSnapshotEvent(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Idle) {
        // we ignore this event, when we are not in active state(s)
        CX_DEBUG(( "wrong state, ignoring snapshot" ));
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }
    emit snapshotReady(error, QImage(), filename());
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Resets the video snapshot and current video filename
*/
void CxeVideoCaptureControlDesktop::reset()
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
*@Return current video filename
*/
QString CxeVideoCaptureControlDesktop::filename() const
{
    // Simply return the current contents of mCurrentFilename.
    // If video recording was started then it returns proper filename
    // otherwise an empty string is returned.
    return mCurrentFilename;
}


/*!
* @Return current video snapshot
*/
QPixmap CxeVideoCaptureControlDesktop::snapshot() const
{
    return mSnapshot;
}

/*!
* @Return QList of all supported video quality details based on the camera index
*  (primary/secondary).
*/
QList<CxeVideoDetails> CxeVideoCaptureControlDesktop::supportedVideoQualities()
{
    return QList<CxeVideoDetails>();
}

/*!
* Starts video recording if we are in appropriate state.
*/
void CxeVideoCaptureControlDesktop::record()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Ready || state() == Paused) {
        //we skip the playing of the sound in the desktop state for now
        setState(Recording);
        mRecordElapseTimer.start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Pauses video recording.
*/
void CxeVideoCaptureControlDesktop::pause()
{
    CX_DEBUG_ENTER_FUNCTION();

    mRecordElapseTimer.stop();

    setState(Paused);

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Stops video recording.
*/
void CxeVideoCaptureControlDesktop::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Recording || state() == Paused) {
        mFilenameGenerator.raiseCounterValue();

        mRecordElapseTimer.stop();
        mElapsedTime = 0;

        setState(Stopping);
        setState(Initialized);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* camera reference changing, release resources
*/
void CxeVideoCaptureControlDesktop::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();
    releaseResources();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* prepare for camera release.
*/
void CxeVideoCaptureControlDesktop::prepareForRelease()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* new camera available,
*/
void CxeVideoCaptureControlDesktop::handleCameraAllocated(CxeError::Id error)
{
    Q_UNUSED(error);
    CX_DEBUG_IN_FUNCTION();
}


/*!
* Initializes video recorder.
*/
void CxeVideoCaptureControlDesktop::createVideoRecorder()
{
    CX_DEBUG_IN_FUNCTION();
}


/*!
* releases resources used by videocapture
*/
void CxeVideoCaptureControlDesktop::releaseResources()
{
    CX_DEBUG_ENTER_FUNCTION();

    // first de-init videocapture control
    deinit();
    reset();

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
@Returns current state of videocapture
*/
CxeVideoCaptureControl::State CxeVideoCaptureControlDesktop::state() const
{
    return mState;
}

/*!
* Initialize states for videocapturecontrol
*/
void CxeVideoCaptureControlDesktop::initializeStates()
{
    CX_DEBUG_IN_FUNCTION();
}

/*!
* calculates remaining video recording time.
*/
void CxeVideoCaptureControlDesktop::remainingTime(int &time)
{
    CX_DEBUG_ENTER_FUNCTION();

    time = CXENGINE_MAXIMUM_VIDEO_TIME - mElapsedTime;
    if (time < 0) {
        time = 0;
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Calculates elapsed recording time during video recording
* @return Did fetching elapsed time succeed.
*/
bool CxeVideoCaptureControlDesktop::elapsedTime(int &time)
{
    CX_DEBUG_ENTER_FUNCTION();

    time = mElapsedTime;

    CX_DEBUG_EXIT_FUNCTION();

    return true;
}

/*!
* slot called when playing a sound has finished.
*/
void CxeVideoCaptureControlDesktop::handleSoundPlayed()
{
    // in case of video capture stop sound playing, nothing needs to be done
    // meaning the state set elsewhere, and the video capture has been stopped already

    CX_DEBUG_IN_FUNCTION();
}


/*!
* setting has changed, check if we are interested.
*/
void CxeVideoCaptureControlDesktop::handleSettingValueChanged(const QString& settingId,
                                                              QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(newValue);

    if (settingId == CxeSettingIds::VIDEO_QUALITY) {
        // re-prepare for video
       deinit();
       init();
    } else if (settingId == CxeSettingIds::VIDEO_MUTE_SETTING) {
        // mute setting changed, apply the new setting and re-prepare.
        prepare();
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Video Scene mode changed, needs updated
*/
void CxeVideoCaptureControlDesktop::handleSceneChanged(CxeScene &scene)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(scene);
    CX_DEBUG_EXIT_FUNCTION();
}

void CxeVideoCaptureControlDesktop::handleElapseTimeout()
{
    mElapsedTime++;

    CX_DEBUG( ("CxeVideoCaptureControlDesktop::handleElapseTimeout() <> mElapsedTime: %d", mElapsedTime ) );
}

/*!
* Slot for starting viewfinder after a delay.
* Delay helps viewfinder widget to find the right, visible transparent window to attach to.
*/
void CxeVideoCaptureControlDesktop::startViewfinder()
{
    CX_DEBUG_ENTER_FUNCTION();
    mViewfinderControl.start();
    CX_DEBUG_EXIT_FUNCTION();
}


void CxeVideoCaptureControlDesktop::setState(CxeVideoCaptureControl::State stateId, CxeError::Id error)
{
    mState = stateId;
    CX_DEBUG( ("CxeVideoCaptureControlDesktop::setState <> mState: %d", mState ) );
    emit stateChanged(mState, error);
}
// End of file
