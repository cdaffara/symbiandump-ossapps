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
#include <QPixmap>
#include <coemain.h>
#include <ECamOrientationCustomInterface2.h>
#include <ecamfacetrackingcustomapi.h>
#include <ecamusecasehintcustomapi.h>

#include "cxestillcapturecontrolsymbian.h"
#include "cxeimagedataqueuesymbian.h"
#include "cxefilenamegenerator.h"
#include "cxefilesavethread.h"
#include "cxutils.h"
#include "cxecameradevicecontrol.h"
#include "cxecameradevice.h"
#include "cxesoundplayersymbian.h"
#include "cxestillimagesymbian.h"
#include "cxeviewfindercontrolsymbian.h"
#include "cxesnapshotcontrol.h"
#include "cxesettingsmappersymbian.h"
#include "cxestate.h"
#include "cxesettings.h"
#include "cxeerrormappingsymbian.h"
#include "cxeautofocuscontrol.h"
#include "cxesensoreventhandler.h"
#include "cxesensoreventhandler.h"
#include "cxequalitypresetssymbian.h"
#include "cxediskmonitor.h"
#include "cxeexception.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cxestillcapturecontrolsymbianTraces.h"
#endif


// constants
namespace
{
    const TInt64 KMinRequiredSpaceImage = 2000000;
}

/*!
 * Constructor.
 */
CxeStillCaptureControlSymbian::CxeStillCaptureControlSymbian(
    CxeCameraDevice &cameraDevice,
    CxeViewfinderControl &viewfinderControl,
    CxeSnapshotControl &snapshotControl,
    CxeCameraDeviceControl &cameraDeviceControl,
    CxeFilenameGenerator &nameGenerator,
    CxeSensorEventHandler &sensorEventHandler,
    CxeAutoFocusControl &autoFocusControl,
    CxeSettings &settings,
    CxeQualityPresets &qualityPresets,
    CxeFileSaveThread &fileSaveThread,
    CxeDiskMonitor &diskMonitor)
    : CxeStateMachine("CxeStillCaptureControlSymbian"),
      mCameraDevice(cameraDevice),
      mViewfinderControl(viewfinderControl),
      mSnapshotControl(snapshotControl),
      mCameraDeviceControl(cameraDeviceControl),
      mFilenameGenerator(nameGenerator),
      mSensorEventHandler(sensorEventHandler),
      mAutoFocusControl(autoFocusControl),
      mSettings(settings),
      mQualityPresets(qualityPresets),
      mFileSaveThread(fileSaveThread),
      mDiskMonitor(diskMonitor),
      mMode(SingleImageCapture),
      mAfState(CxeAutoFocusControl::Unknown)
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_CREATE_IN, "msg: e_CX_STILLCAPTURECONTROL_NEW 1");

    qRegisterMetaType<CxeStillCaptureControl::State>();
    initializeStates();
    reset();

    // If camera is already allocated, call the slot ourselves.
    if (mCameraDevice.camera()) {
        handleCameraAllocated(CxeError::None);
    }

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_CREATE_MID1, "msg: e_CX_ENGINE_CONNECT_SIGNALS 1");
    // connect signals from cameraDevice to recieve events when camera reference changes
    connect(&cameraDevice, SIGNAL(prepareForCameraDelete()),
            this, SLOT(prepareForCameraDelete()));
    connect(&cameraDevice, SIGNAL(prepareForRelease()),
            this, SLOT(prepareForRelease()));
    connect(&cameraDevice, SIGNAL(cameraAllocated(CxeError::Id)),
            this, SLOT(handleCameraAllocated(CxeError::Id)));
    connect(&mSensorEventHandler,
            SIGNAL(sensorEvent(CxeSensorEventHandler::SensorType,QVariant)),
            this, SLOT(handleSensorEvent(CxeSensorEventHandler::SensorType,QVariant)));

    // enabling setting change callbacks to stillcapturecontrol
    connect(&mSettings, SIGNAL(settingValueChanged(const QString&,QVariant)),
            this, SLOT(handleSettingValueChanged(const QString&,QVariant)));

    // Connect ECam image buffer ready event
    connect(&mCameraDeviceControl, SIGNAL(imageBufferReady(MCameraBuffer*,int)),
            this, SLOT(handleImageData(MCameraBuffer*,int)));
    // connect snapshot ready signal
    connect(&mSnapshotControl, SIGNAL(snapshotReady(CxeError::Id, const QImage&)),
            this, SLOT(handleSnapshotReady(CxeError::Id, const QImage&)));

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_CREATE_MID2, "msg: e_CX_ENGINE_CONNECT_SIGNALS 0");

    mImageDataQueue = new CxeImageDataQueueSymbian();
    mAutoFocusSoundPlayer = new CxeSoundPlayerSymbian(CxeSoundPlayerSymbian::AutoFocus, mSettings);
    mCaptureSoundPlayer = new CxeSoundPlayerSymbian(CxeSoundPlayerSymbian::StillCapture, mSettings);

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_CREATE_OUT, "msg: e_CX_STILLCAPTURECONTROL_NEW 0");
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Destructor.
 */
CxeStillCaptureControlSymbian::~CxeStillCaptureControlSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    deinit();
    reset();
    mIcmSupportedImageResolutions.clear();
    mECamSupportedImageResolutions.clear();
    delete mImageDataQueue;
    delete mCaptureSoundPlayer;
    delete mAutoFocusSoundPlayer;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Return the current state.
 */
CxeStillCaptureControl::State CxeStillCaptureControlSymbian::state() const
{
    return static_cast<CxeStillCaptureControl::State>( stateId() );
}

/*!
 * Handle state changed event. Normally just emits the signal
 * for observers to react appropriately.
 */
void CxeStillCaptureControlSymbian::handleStateChanged( int newStateId, CxeError::Id error )
{
    emit stateChanged( static_cast<State>( newStateId ), error );
}

/*!
 * Initialize the control states.
 */
void CxeStillCaptureControlSymbian::initializeStates()
{
    // addState( id, name, allowed next states )
    addState(new CxeState( Uninitialized , "Uninitialized", Ready));
    addState(new CxeState( Ready , "Ready", Uninitialized | Capturing));
    addState(new CxeState( Capturing , "Capturing", Uninitialized | Ready));

    setInitialState(Uninitialized);
}

/*!
 * Initialize the still image capture control.
 */
void CxeStillCaptureControlSymbian::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Uninitialized) {
        OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_INIT_IN, "msg: e_CX_STILL_CAPCONT_INIT 1");

        // prepare for still capture.
        prepare();

        // Initialize orientation sensor and other sensors
        mSensorEventHandler.init();

        // inform zoom control to prepare zoom
        emit prepareZoomForStill(mSizeIndex);

        OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_INIT_OUT, "msg: e_CX_STILL_CAPCONT_INIT 0");
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Un-initialize the image mode.
 */
void CxeStillCaptureControlSymbian::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Uninitialized) {
        OstTrace0( camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_DEINIT_IN, "msg: e_CX_STILLCAPCONT_DEINIT 1" );

        // Stop monitoring disk space.
        mDiskMonitor.stop();
        disconnect(&mDiskMonitor, SIGNAL(diskSpaceChanged()), this, SLOT(handleDiskSpaceChanged()));

        //stop viewfinder
        mViewfinderControl.stop();

        if (state() == Capturing) {
            mCameraDevice.camera()->CancelCaptureImage();
        }

        // disable sensor event handler.
        mSensorEventHandler.deinit();

        mSnapshotControl.stop();

        setState(Uninitialized);

        OstTrace0( camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_DEINIT_OUT, "msg: e_CX_STILLCAPCONT_DEINIT 0" );
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
 * Prepare still capture mode.
 */
void CxeStillCaptureControlSymbian::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROL_PREPARE_IN, "msg: e_CX_STILLCAPCONT_PREPARE 1");

    CxeError::Id status(CxeError::None);

    try {
        // Update capture parameters
        updateStillCaptureParameters();

        // Prepare Image capture
        CX_DEBUG(("Calling PrepareImageCaptureL, resolution index = %d", mSizeIndex));
        OstTrace0(camerax_performance, CXESTILLCAPTURECONTROL_PREPARE_MID1, "msg: e_CX_PREPARE_IMAGE_CAPTURE 1");
        QT_TRAP_THROWING(mCameraDevice.camera()->PrepareImageCaptureL(mCaptureFormat, mSizeIndex));
        OstTrace0(camerax_performance, CXESTILLCAPTURECONTROL_PREPARE_MID2, "msg: e_CX_PREPARE_IMAGE_CAPTURE 0");

        // Start viewfinder before claiming to be ready,
        // as e.g. pending capture might be started by state change,
        // and viewfinder start might have problems with just started capturing.
        // If viewfinder is already running, this call does nothing.
        mViewfinderControl.start();

        // Prepare snapshot
        prepareSnapshot();

        // Start monitoring disk space.
        mDiskMonitor.start();
        connect(&mDiskMonitor, SIGNAL(diskSpaceChanged()), this, SLOT(handleDiskSpaceChanged()), Qt::UniqueConnection);

        // Enable AF reticule drawing by adaptation
        MCameraFaceTracking *faceTracking = mCameraDevice.faceTracking();
        if (faceTracking) {
            TRAP_IGNORE(faceTracking->EnableFaceIndicatorsL(ETrue));
        }

        // Still capture and still snapshot are OK.
        // We can safely set state to READY.
        setState(Ready);

    } catch (const std::exception &e) {
        // Exception encountered, free resources.
        CX_DEBUG(("Image Prepare FAILED! symbian error = %d", qt_symbian_exception2Error(e)));
        status = CxeErrorHandlingSymbian::map(qt_symbian_exception2Error(e));
        deinit();
    }

    // Inform interested parties that image mode has been prepared for capture
    emit imagePrepareComplete(status);

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROL_GOTOSTILL, "msg: e_CX_GO_TO_STILL_MODE 0");
    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROL_PREPARE_OUT, "msg: e_CX_STILLCAPCONT_PREPARE 0");
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
 Prepare still snapshot
 Throws exception on error.
 */
void CxeStillCaptureControlSymbian::prepareSnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROL_PREPARESNAP_1, "msg: e_CX_PREPARE_SNAPSHOT 1" );

    QSize snapshotSize = mSnapshotControl.calculateSnapshotSize(
                            mViewfinderControl.deviceDisplayResolution(),
                            mCurrentImageDetails.mAspectRatio);
    mSnapshotControl.start(snapshotSize);

    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROL_PREPARESNAP_2, "msg: e_CX_PREPARE_SNAPSHOT 0" );
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
    Update image capture parameters. mCurrentImageDetails, mCaptureFormat and
    mSizeIndex are updated based on the current settings.
*/
void CxeStillCaptureControlSymbian::updateStillCaptureParameters()
{
    CX_DEBUG_ENTER_FUNCTION();
    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROL_UPDATESTILLCAPTUREPARAMETERS_1, "msg: e_CX_UPDATE_STILL_CAPTURE_PARAMETERS 1" );

    int imageQuality = 0;
    CxeError::Id err = CxeError::None;
    if (mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex) {
        try {
            imageQuality = mSettings.get<int>(CxeSettingIds::IMAGE_QUALITY);

            bool validQuality = (imageQuality >= 0 &&
                                 imageQuality < mIcmSupportedImageResolutions.count());

            if (validQuality ) {
                // get image quality details
                mCurrentImageDetails = mIcmSupportedImageResolutions.at(imageQuality);
            } else {
                err = CxeError::NotFound;
                CX_DEBUG(("Invalid ImageQuality = %d", imageQuality));
            }
        } catch (CxeException &e) {
            err = (CxeError::Id) e.error();
        }
    } else {
        // we are in secondary camera
        // get secondary camera image quality details
       mCurrentImageDetails = mIcmSupportedImageResolutions.at(imageQuality);
    }

    mSizeIndex = 0;

    if (err == CxeError::None) {
        int imageWidth = mCurrentImageDetails.mWidth;
        int imageHeight = mCurrentImageDetails.mHeight;
        CX_DEBUG(("CxeStillCaptureControlSymbian::updateStillCaptureParameters <> resolution = (%d, %d)", imageWidth, imageHeight));

        TSize imageSize;
        imageSize.SetSize(imageWidth, imageHeight);

        if (mECamSupportedImageResolutions.count() > 0) {
            mSizeIndex = mECamSupportedImageResolutions.indexOf(imageSize);
        }

        if (mSizeIndex < 0) {
            CX_DEBUG(("CxeStillCaptureControlSymbian::updateStillCaptureParameters - WARNING! resolution not supported, falling back to index 0"));
            mSizeIndex = 0;
        }

        mCaptureFormat = supportedStillFormat(mCameraDeviceControl.cameraIndex());
    }

    OstTrace0( camerax_performance, CXESTILLCAPTURECONTROL_UPDATESTILLCAPTUREPARAMETERS_2, "msg: e_CX_UPDATE_STILL_CAPTURE_PARAMETERS 0" );
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Command to start image capture now.
 * @sa handleCameraEvent
 */
void CxeStillCaptureControlSymbian::capture()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_ASSERT_ALWAYS(mCameraDevice.camera());

    // Start the image capture as fast as possible to minimize lag.
    // Check e.g. space available *after* this.
    // Capture sound will be played when we receive "image capture event" from ECAM.
    mCameraDevice.camera()->CaptureImage();

    if (imagesLeft() > 0) {
        setState(Capturing);

        //! @todo: NOTE: This call may not stay here. It can move depending on the implementation for burst capture.
        if (mMode == BurstCapture) {
            // Start a new filename sequence
            mFilenameGenerator.startNewImageFilenameSequence();
        }
    } else {
        // There's no space for the image.
        // Cancel started capturing.
        mCameraDevice.camera()->CancelCaptureImage();

        // Report error.
        // Ui notification has anyway some delays, hence handling VF after this.
        emit imageCaptured(CxeError::DiskFull, CxeStillImage::INVALID_ID);

        // Capturing stops viewfinder, so restart it now.
        mViewfinderControl.stop();
        mViewfinderControl.start();
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
@Param cameraIndex indicates which camera we are in use, primary/secondary
Returns the format we use for specific camera index.
*/
CCamera::TFormat CxeStillCaptureControlSymbian::supportedStillFormat(Cxe::CameraIndex cameraIndex)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(cameraIndex);

    // The same image format used for both primary and secodary cameras
    CCamera::TFormat imgFormat = CCamera::EFormatExif;

    CX_DEBUG_EXIT_FUNCTION();

    return imgFormat;
}

/*!
 * Snapshot ready notification. Ask the snapshot from snapshot interface.
 * NB: Typically snapshot arrives before image data but can be in reverse
 * order as well.
 *
 * @param status Status of snapshot creation. CxeError::None if no error, otherwise contains the error code.
 * @param snapshot Snapshot as QImage
 */
void CxeStillCaptureControlSymbian::handleSnapshotReady(CxeError::Id status, const QImage &snapshot)
{
    CX_DEBUG_ENTER_FUNCTION();
    if (mCameraDeviceControl.mode() == Cxe::ImageMode) {

        OstTrace0(camerax_performance, CXESTILLCAPTURECONTROL_HANDLESNAPSHOT_1, "msg: e_CX_HANDLE_SNAPSHOT 1");

        QPixmap ss = QPixmap::fromImage(snapshot);
        // Get image container for current snapshot index.
        // Remember to increment counter.
        CxeStillImageSymbian* stillImage = getImageForIndex(mNextSnapshotIndex++);
        if (status == CxeError::None) {
            stillImage->setSnapshot(ss);
        }

        // Emit snapshotReady signal in all cases (error or not)
        emit snapshotReady(status, snapshot, stillImage->id());

        OstTrace0(camerax_performance, CXESTILLCAPTURECONTROL_HANDLESNAPSHOT_2, "msg: e_CX_HANDLE_SNAPSHOT 0");
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Handle ECAM events.
* Needed only for capture sound synchronization.
* @param eventUid ECAM event id.
* @param error Error code. KErrNone if operation has been successful.
*/
void CxeStillCaptureControlSymbian::handleCameraEvent(int eventUid, int error)
{
    if (eventUid == KUidECamEventImageCaptureEventUidValue && error == KErrNone) {
        CX_DEBUG(("CxeStillCaptureControlSymbian::handleCameraEvent - image capture event"));
        if (state() == CxeStillCaptureControl::Capturing) {
            mCaptureSoundPlayer->play();
        }
    }
}


/*!
 * handleImageData: Image data received from ECam
 */
void CxeStillCaptureControlSymbian::handleImageData(MCameraBuffer* cameraBuffer, int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == CxeStillCaptureControl::Uninitialized) {
        // we ignore this event, when we are not active
        cameraBuffer->Release();
        return;
    }

    // Get image container for current image data index.
    // Remember to increment counter.
    CxeStillImageSymbian* stillImage = getImageForIndex(mNextImageDataIndex++);

    if (error) {
        // Indicate error in image capture to the UI.
        cameraBuffer->Release();
        emit imageCaptured(CxeErrorHandlingSymbian::map(error), stillImage->id());
        return;
    }

    // If filename is not available at this stage, then generate one now
    if (stillImage->filename().isEmpty()) {
        CxeError::Id cxErr = prepareFilename(stillImage);
        if (cxErr) {
            cameraBuffer->Release();
            emit imageCaptured(cxErr, stillImage->id());
            return;
        }
    }

    // Get the image data from the buffer
    TDesC8* data = NULL;
    TRAPD( symbErr, data = cameraBuffer->DataL(0) );
    CX_DEBUG(("dataError: %d, data: 0x%08x", symbErr, data));

    if (!data && !symbErr) {
        symbErr = KErrNoMemory;
    }

    // If data is available, initiate saving of image
    if (!symbErr) {
        //! @todo: this does a deep copy... we want to avoid this for performance/memory consumption reasons
        QByteArray byteArray( reinterpret_cast<const char*>( data->Ptr() ), data->Size() );
        data = NULL;

        // get geotagging setting value and check if we have to add location trail to image data.
        Cxe::GeoTagging value = mSettings.get<Cxe::GeoTagging>(CxeSettingIds::GEOTAGGING, Cxe::GeoTaggingOff);


        // Save the image data
        CxeImageDataItemSymbian* dataItem = mImageDataQueue->startSave(byteArray,
                                                                       stillImage->filename(),
                                                                       stillImage->id(),
                                                                       value == Cxe::GeoTaggingOn);
        stillImage->setDataItem(dataItem);
        mFileSaveThread.save(dataItem); // Saving thread takes ownership of dataItem.
    }

    // ECam Camera buffer should always be released
    cameraBuffer->Release();

    // Inform interested parties about image capture
    emit imageCaptured(CxeErrorHandlingSymbian::map(symbErr), stillImage->id());

    // set state to ready, since capturing is complete
    setState(Ready);

    // image capture ready, before that we check if the orientation of the device changed during capture and if so, we set the new orientation
    setOrientation(mSensorEventHandler.sensorData(CxeSensorEventHandler::OrientationSensor));

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
 * Settings changed, needs updated.
 */
void CxeStillCaptureControlSymbian::handleSettingValueChanged(const QString& settingId, QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(newValue);

    if (settingId == CxeSettingIds::IMAGE_QUALITY) {
        // Re-prepare for still, if already prepared.
        if (state() == Ready) {
            // Avoid problems with AF status overlays.
            mAutoFocusControl.cancel();

            // Viewfinder and snapshot need to be re-prepared if active.
            // Stop them now to get proper re-prepare done.
            mSnapshotControl.stop();
            mViewfinderControl.stop();

            // Re-prepare image capture for the new quality.
            prepare();

            // Inform zoom control to prepare zoom for new quality.
            emit prepareZoomForStill(mSizeIndex);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Disk space changed.
* Emit remaining images changed signal, if space change affects it.
*/
void CxeStillCaptureControlSymbian::handleDiskSpaceChanged()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Ignore updates on other states.
    if (state() == CxeStillCaptureControl::Ready) {

        int images = calculateRemainingImages(mCurrentImageDetails.mEstimatedSize);

        if (images != mCurrentImageDetails.mPossibleImages) {
            CX_DEBUG(("CxeStillCaptureControlSymbian - available images changed %d -> %d",
                      mCurrentImageDetails.mPossibleImages, images));

            mCurrentImageDetails.mPossibleImages = images;
            emit availableImagesChanged();
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * ECam reference changing, release resources
 */
void CxeStillCaptureControlSymbian::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Camera being released. Cancel ongoing capture, if any.
 */
void CxeStillCaptureControlSymbian::prepareForRelease()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 *  New camera available,
 */
void CxeStillCaptureControlSymbian::handleCameraAllocated(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (error == CxeError::None) {
        // load all still resoultions supported by ecam
        mECamSupportedImageResolutions.clear();
        TCameraInfo cameraInfo;
        Cxe::CameraIndex cameraIndex = mCameraDeviceControl.cameraIndex();
        CCamera::TFormat imgFormat = supportedStillFormat(cameraIndex);
        mCameraDevice.camera()->CameraInfo(cameraInfo);

        for(int i = 0; i < cameraInfo.iNumImageSizesSupported; i++) {
            TSize size;
            mCameraDevice.camera()->EnumerateCaptureSizes(size, i, imgFormat);
            CX_DEBUG(("ECam supported resolution <> Size (%d): (%d,%d)", i, size.iWidth, size.iHeight));
            mECamSupportedImageResolutions.insert(i, size);
        }

        // load all still qualities supported by icm
        mIcmSupportedImageResolutions.clear();
        // get list of supported image qualities based on camera index
        mIcmSupportedImageResolutions = mQualityPresets.imageQualityPresets(cameraIndex);

        CX_DEBUG(("ECAM Supported Qualities count = %d", mECamSupportedImageResolutions.count()));
        CX_DEBUG(("ICM Supported Qualities count = %d", mIcmSupportedImageResolutions.count()));
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Return number of images captured (during current capture operation only).
 */
int CxeStillCaptureControlSymbian::imageCount() const
{
    return mImages.count();
}

/*!
 * Reset the image array.
 */
void CxeStillCaptureControlSymbian::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    qDeleteAll( mImages );
    mImages.clear();

    mNextSnapshotIndex = 0;
    mNextImageDataIndex = 0;

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * This should cancel any ongoing image captures.
 */
void CxeStillCaptureControlSymbian::cancelAll()
{
    mImageDataQueue->clear();
    reset();
}

/*!
 * Sets the current capture mode: SingleImageCapture / BurstCapture.
 */
void CxeStillCaptureControlSymbian::setMode( CaptureMode mode )
{
    mMode = mode;
}

/*!
 * Returns the current capture mode.
 */
CxeStillCaptureControl::CaptureMode CxeStillCaptureControlSymbian::mode() const
{
    return mMode;
}

/*!
 * Operator [] - returns the indexed image from capture array.
 */
CxeStillImage &CxeStillCaptureControlSymbian::operator[]( int index )
{
    return *mImages[ index ];
}

/*!
 * Getter for image data queue.
 */
CxeImageDataQueue &CxeStillCaptureControlSymbian::imageDataQueue()
{
    return *mImageDataQueue;
}

/*!
 * Generates a filename and sets it in the still image object.
 * Skips the process if filename already copied exists in the object. This
 * behaviour is required in rare cases where image data arrives before snapshot.
 */
CxeError::Id
CxeStillCaptureControlSymbian::prepareFilename(CxeStillImageSymbian *stillImage)
{
    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_PREPAREFILENAME_1, "msg: e_CX_PREPARE_FILENAME 1" );

    CxeError::Id err = CxeError::None;
    if (stillImage->filename().isEmpty()) {
        QString path;

        QString fileExt = mCurrentImageDetails.mImageFileExtension;

        if (mMode == SingleImageCapture) {
            err = mFilenameGenerator.generateFilename(path, fileExt);
        }
        else {
            err = mFilenameGenerator.nextImageFilenameInSequence(path, fileExt);
        }

        if (!err) {
            CX_DEBUG(( "Next image file path: %s", path.toAscii().constData() ));
            stillImage->setFilename(path);
        }
        else {
            //! @todo: Error ID can be returned by this function.
            // Also error can be detected from empty filename string.
            CX_DEBUG(("ERROR in filename generation. err:%d", err));
        }
    }

    OstTrace0(camerax_performance, CXESTILLCAPTURECONTROLSYMBIAN_PREPAREFILENAME_2, "msg: e_CX_PREPARE_FILENAME 0" );
    return err;
}

/*!
* Helper method to set orientation data from the orientation sensor
*/
void CxeStillCaptureControlSymbian::setOrientation(QVariant sensorData)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCameraDevice.cameraOrientation() && state() == Ready) {
        Cxe::DeviceOrientation uiOrientation = qVariantValue<Cxe::DeviceOrientation >(sensorData);
        MCameraOrientation::TOrientation currentCameraOrientation = mCameraDevice.cameraOrientation()->Orientation();
        MCameraOrientation::TOrientation newCameraOrientation = CxeSettingsMapperSymbian::Map2CameraOrientation(uiOrientation);

        CX_DEBUG((("cameraindex: %d mMode: %d state(): %d mAfState: %d uiOrientation: %d "),mCameraDeviceControl.cameraIndex(),
                mMode, state(), mAfState, uiOrientation ));
        if (mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex &&
            mMode                              == SingleImageCapture &&
            uiOrientation                      != Cxe::OrientationNone &&
            currentCameraOrientation           != newCameraOrientation &&
           (mAfState                           == CxeAutoFocusControl::Unknown ||
            mAfState                           == CxeAutoFocusControl::Canceling)
        ) {
            CX_DEBUG(("Setting Orientation to adaptation"));
            TRAP_IGNORE(mCameraDevice.cameraOrientation()->SetOrientationL(newCameraOrientation));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
 * Get the image container at given index or create a new one if needed.
 */
CxeStillImageSymbian* CxeStillCaptureControlSymbian::getImageForIndex(int index)
{
    CxeStillImageSymbian* image(NULL);

    if (mImages.count() <= index) {
        image = new CxeStillImageSymbian();
        mImages.append(image);
    } else {
        CX_DEBUG_ASSERT( mNextImageDataIndex >= 0 && index < mImages.count() );
        image = mImages[index];
    }
    return image;
}


/*!
* Slot to handle Autofocus events.
* \param newState Indicates current state of the auto focus
* \param error Contains possible error code
*/
void CxeStillCaptureControlSymbian::handleAutofocusStateChanged(
                                         CxeAutoFocusControl::State newState,
                                         CxeError::Id error )
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(error);
    mAfState = newState;
    CxeAutoFocusControl::Mode mode = mAutoFocusControl.mode();

    // if focused and in correct mode, play sound
    if  (newState == CxeAutoFocusControl::Ready &&
         !mAutoFocusControl.isFixedFocusMode(mode) &&
         mAutoFocusControl.isSoundEnabled()) {
        mAutoFocusSoundPlayer->play();
    }
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Slot that sets orientation data emited from orientation sensor
*/
void CxeStillCaptureControlSymbian::handleSensorEvent(
                                CxeSensorEventHandler::SensorType type,
                                QVariant data)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (type == CxeSensorEventHandler::OrientationSensor) {
        setOrientation(data);
    } else {
        // nothing to do
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
    Use ECam Use Case Hint Custom API to inform ECam of our intended use case
    before calling Reserve().
*/
void CxeStillCaptureControlSymbian::hintUseCase()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Make sure ECam knows we're doing still image capture so it can prepare
    // for the correct use case.
    if (mCameraDeviceControl.mode() == Cxe::ImageMode) {
        MCameraUseCaseHint *useCaseHintApi = mCameraDevice.useCaseHintApi();
        if (useCaseHintApi) {
            updateStillCaptureParameters();
            TRAP_IGNORE(useCaseHintApi->HintStillCaptureL(mCaptureFormat,
                                                          mSizeIndex));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Returns supported image qualities based on the camera index
* (primary/secondary).
*/
QList<CxeImageDetails> CxeStillCaptureControlSymbian::supportedImageQualities()
{
    // update remaining images counter
    updateRemainingImagesCounter();

    return mIcmSupportedImageResolutions;
}


/*!
* Updates remaining images counter to all the image qualities supported by ICM
* this should be done whenever storage location setting changes and when values are
* read from ICM for the first time
*/
void CxeStillCaptureControlSymbian::updateRemainingImagesCounter()
{
    CX_DEBUG_ENTER_FUNCTION();

    for( int index = 0; index < mIcmSupportedImageResolutions.count(); index++) {
        CxeImageDetails &qualityDetails = mIcmSupportedImageResolutions[index];
        qualityDetails.mPossibleImages = calculateRemainingImages(qualityDetails.mEstimatedSize);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Returns the number of images left for the current image quality setting
*/
int CxeStillCaptureControlSymbian::imagesLeft()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCurrentImageDetails.mPossibleImages == CxeImageDetails::UNKNOWN) {
        mCurrentImageDetails.mPossibleImages = calculateRemainingImages(mCurrentImageDetails.mEstimatedSize);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return mCurrentImageDetails.mPossibleImages;
}



/*!
* CxeStillCaptureControlSymbian::calculateRemainingImages
@ param estimatedImagesize, the estimated size for image resolution
@ returns number of images remaining
*/
int
CxeStillCaptureControlSymbian::calculateRemainingImages(int estimatedImagesize)
{
    CX_DEBUG_ENTER_FUNCTION();

    qint64 space = mDiskMonitor.free() - KMinRequiredSpaceImage;
    int images = std::max(qint64(0), space / estimatedImagesize);

    CX_DEBUG_EXIT_FUNCTION();

    return images;
}

// end of file
