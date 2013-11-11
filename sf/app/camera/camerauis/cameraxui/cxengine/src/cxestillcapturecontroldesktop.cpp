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

#include <QPixmap>

#include "cxestillcapturecontroldesktop.h"
#include "cxeimagedataitemdesktop.h"
#include "cxeimagedataqueuedesktop.h"
#include "cxefilenamegenerator.h"
#include "cxefilesavethreaddesktop.h"
#include "cxutils.h"
#include "cxecameradevicecontrol.h"
#include "cxestillimagedesktop.h"
#include "cxeviewfindercontrol.h"
#include "cxeviewfindercontroldesktop.h"
#include "cxeautofocuscontrol.h"
#include "cxestate.h"
#include "cxecameradevicedesktop.h"

// constants
namespace
{
    static const int KMaintainAspectRatio = false;
    static const int VIEWFINDER_START_TIMEOUT = 500; // 0.5 second
}


/**
 * Constructor.
 */
CxeStillCaptureControlDesktop::CxeStillCaptureControlDesktop(
        CxeCameraDeviceDesktop &cameraDevice,
        CxeViewfinderControl &viewfinderControl,
        CxeCameraDeviceControl &cameraDeviceControl,
        CxeFilenameGenerator &nameGenerator,
        CxeAutoFocusControl &autoFocusControl,
        CxeFileSaveThread &saveThread) :
  mCameraDevice(cameraDevice),
  mState(CxeStillCaptureControl::Uninitialized),
  mViewfinderControl(viewfinderControl),
  mCameraDeviceControl(cameraDeviceControl),
  mFilenameGenerator(nameGenerator),
  mAutoFocusControl(autoFocusControl),
  mMode(SingleImageCapture),
  mAfState(CxeAutoFocusControl::Unknown),
  mSaveThread(saveThread)
{
    CX_DEBUG_ENTER_FUNCTION();

    qRegisterMetaType<CxeStillCaptureControl::State>();
    initializeStates();
    reset();

    mViewFinderStartTimer.setSingleShot(true);
    mViewFinderStartTimer.setInterval(VIEWFINDER_START_TIMEOUT);
    connect(&mViewFinderStartTimer, SIGNAL(timeout()), this, SLOT(startViewfinder()));

    mImageDataQueue = new CxeImageDataQueueDesktop();

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Destructor.
 */
CxeStillCaptureControlDesktop::~CxeStillCaptureControlDesktop()
{
    CX_DEBUG_ENTER_FUNCTION();

    deinit();
    reset();
    mSupportedImageQualities.clear();
    delete mImageDataQueue;

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Return the current state.
 */
CxeStillCaptureControl::State CxeStillCaptureControlDesktop::state() const
{
    return mState;
}

/**
 * Initialize the control states.
 */
void CxeStillCaptureControlDesktop::initializeStates()
{

    // addState( id, name, allowed next states )
}

/**
 * Initialize the still image capture control.
 */
void CxeStillCaptureControlDesktop::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Uninitialized) {
        CxeImageDetails dummyDetails;
        dummyDetails.mAspectRatio = Cxe::AspectRatio4to3;
        dummyDetails.mEstimatedSize = 10000;
        dummyDetails.mHeight = 360;
        dummyDetails.mWidth = 640;
        mSupportedImageQualities.append(dummyDetails);
        prepare();
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Un-initialize the image mode.
 */
void CxeStillCaptureControlDesktop::deinit()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Uninitialized) {
        // nothing to do
        CX_DEBUG_EXIT_FUNCTION();
        return;
    }

    mViewFinderStartTimer.stop();
    mViewfinderControl.stop();

    mState = Uninitialized;
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
 * Prepare still capture mode.
 */
void CxeStillCaptureControlDesktop::prepare()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() != Uninitialized) {
        // wrong state and we return
        return;
    }

    int err = 0;
    CxeError::Id cxErr = getImageQualityDetails(mCurrentImageDetails);
    int ecamStillResolutionIndex = 0;

    if (cxErr == CxeError::None) {
        int imageWidth =  mCurrentImageDetails.mWidth;
        int imageHeight = mCurrentImageDetails.mHeight;
        CX_DEBUG(("CxeStillCaptureControlSymbian::prepare <> resolution = (%d, %d)", imageWidth, imageHeight));

        if (ecamStillResolutionIndex < 0) {
            CX_DEBUG(("CxeStillCaptureControlSymbian::prepare - WARNING! resolution not supported, falling back to index 0"));
            ecamStillResolutionIndex = 0;
        }

        CX_DEBUG(("PrepareImageCaptureL done, err=%d, resolution index = %d", err, ecamStillResolutionIndex));

        if (!err) {
            // still capture prepare went fine, try preparing snapshot
            err = prepareStillSnapshot();
        }
    } else {
        err = 0;
    }

    if (!err) {
        // Start viewfinder with delay.
        mViewFinderStartTimer.start();
        // inform zoom control to prepare zoom
        emit prepareZoomForStill(ecamStillResolutionIndex);
    } else {
        CX_DEBUG(("Image Prepare FAILED! symbian error = %d", err));
    }
    mState = Ready;
    emit imagePrepareComplete(CxeError::None);
    CX_DEBUG_EXIT_FUNCTION();
}



/*!
 Prepare still snapshot
 Returns symbian error code.
 */
int CxeStillCaptureControlDesktop::prepareStillSnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();

    handleSnapshotEvent(CxeError::None);

    CX_DEBUG_EXIT_FUNCTION();

    return 0;
}


/*!
 imageInfo contains image qualities details
 Returns CxeError error code.
 */
CxeError::Id CxeStillCaptureControlDesktop::getImageQualityDetails(CxeImageDetails &imageInfo)
{
    CX_DEBUG_ENTER_FUNCTION();

    int imageQuality = 0;
    CxeError::Id err = CxeError::None;
    if (mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex) {
        bool validQuality = (imageQuality >= 0 && imageQuality < mSupportedImageQualities.count());

        if (err == CxeError::None && validQuality ) {
            // get image quality details
            imageInfo = mSupportedImageQualities.at(imageQuality);
        } else {
            err = CxeError::NotFound;
            CX_DEBUG(("Invalid ImageQuality = %d", imageQuality));
        }
    } else {
        // we are in secondary camera
        // get secondary camera image quality details
       imageInfo = mSupportedImageQualities.at(imageQuality);
    }

    CX_DEBUG_EXIT_FUNCTION();
    return err;
}


/**
 * Command to start image capture now.
 */
void CxeStillCaptureControlDesktop::capture()
{
    CX_DEBUG_ENTER_FUNCTION();

    //! @todo: NOTE: This call may not stay here. It can move depending on the implementation for burst capture.
    if (mMode == BurstCapture) {
        // Start a new filename sequence
        mFilenameGenerator.startNewImageFilenameSequence();
    }
    emit imageCaptured(CxeError::None, 0);
    handleSnapshotEvent(CxeError::None);
    CX_DEBUG_EXIT_FUNCTION();
}


/**
 * Snapshot ready notification. Ask the snapshot from snapshot interface.
 * NB: Typically snapshot arrives before image data but can be in reverse
 * order as well.
 */
void CxeStillCaptureControlDesktop::handleSnapshotEvent(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == CxeStillCaptureControl::Uninitialized) {
        // we ignore this event, when we are not active
        return;
    }

    if (error) {
        emit snapshotReady(error, QImage(), 0);
    }

    // Get image container for current snapshot index.
    // Remember to increment counter.
    CxeStillImageDesktop* stillImage = getImageForIndex(mNextSnapshotIndex++);

    // When the snapshot ready event is handled, prepare new filename.
    if (stillImage->filename().isEmpty()) {
        // Error ignored at this point, try again when image data arrives.
        prepareFilename(stillImage);
    }

    CxeImageDataItem* dataItem = new CxeImageDataItemDesktop(mNextImageDataIndex++, stillImage->snapshot(), stillImage->filename(), stillImage->id());

    stillImage->setDataItem(dataItem);

    mSaveThread.save(dataItem);

    emit snapshotReady(CxeError::None, QImage(), stillImage->id());
    emit imageCaptured(CxeError::None, stillImage->id());

    mState = Ready;
    emit stateChanged(mState, CxeError::None);

    CX_DEBUG_EXIT_FUNCTION();
}



/**
 * Settings changed, needs updated
 */
void CxeStillCaptureControlDesktop::handleSettingValueChanged(const QString& settingId,QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (settingId == CxeSettingIds::FLASH_MODE) {
        updateFlashSetting(newValue);
    } else if (settingId == CxeSettingIds::LIGHT_SENSITIVITY) {
        updateISOSetting(newValue);
    } else if (settingId == CxeSettingIds::EV_COMPENSATION_VALUE) {
        updateEVCompensationSetting(newValue);
    } else if (settingId == CxeSettingIds::IMAGE_QUALITY) {
        // re-prepare for still
        deinit();
        init();
    } else {
        // do nothing
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Image Scene mode changed, needs updated
 */
void CxeStillCaptureControlDesktop::handleSceneChanged(CxeScene& scene)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mCameraDeviceControl.cameraIndex() == Cxe::PrimaryCameraIndex){
        CX_DEBUG_ASSERT(scene.count() != 0);
        // we are interested only in the following settings in this class scope

        if(scene.contains(CxeSettingIds::FLASH_MODE)) {
            CX_DEBUG(("CxeStillCaptureControlSymbian::handleSceneChanged scene->mFlashMode = %d", scene[CxeSettingIds::FLASH_MODE].toInt()));
            updateFlashSetting(scene[CxeSettingIds::FLASH_MODE]);
        }

        if(scene.contains(CxeSettingIds::LIGHT_SENSITIVITY)) {
            updateISOSetting(scene[CxeSettingIds::LIGHT_SENSITIVITY]);
        }

        if(scene.contains(CxeSettingIds::EV_COMPENSATION_VALUE)) {
            updateEVCompensationSetting(scene[CxeSettingIds::EV_COMPENSATION_VALUE]);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * UpdateFlashSetting
 */
void CxeStillCaptureControlDesktop::updateFlashSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(newValue);

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * UpdateISOSetting
 */
void CxeStillCaptureControlDesktop::updateISOSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(newValue);

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * UpdateEVCompensationSetting
 */
void CxeStillCaptureControlDesktop::updateEVCompensationSetting(QVariant newValue)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(newValue);


    CX_DEBUG_EXIT_FUNCTION();
}


/**
 * ECam reference changing, release resources
 */
void CxeStillCaptureControlDesktop::prepareForCameraDelete()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Camera being released. Cancel ongoing capture, if any.
 */
void CxeStillCaptureControlDesktop::prepareForRelease()
{
    CX_DEBUG_ENTER_FUNCTION();
    deinit();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 *  New camera available,
 */
void CxeStillCaptureControlDesktop::handleCameraAllocated(CxeError::Id error)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (error == CxeError::None) {
        // load all still resoultions supported by ecam
        // load all still qualities supported by icm
        mSupportedImageQualities.clear();
        // get list of supported image qualities based on camera index

    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Return number of images captured (during current capture operation only).
 */
int CxeStillCaptureControlDesktop::imageCount() const
{
    return mImages.count();
}

/**
 * Reset the image array.
 */
void CxeStillCaptureControlDesktop::reset()
{
    CX_DEBUG_ENTER_FUNCTION();

    qDeleteAll(mImages);
    mImages.clear();

    mNextSnapshotIndex = 0;
    mNextImageDataIndex = 0;

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * This should cancel any ongoing image captures.
 */
void CxeStillCaptureControlDesktop::cancelAll()
{
    mImageDataQueue->clear();
    reset();
}

/**
 * Sets the current capture mode: SingleImageCapture / BurstCapture.
 */
void CxeStillCaptureControlDesktop::setMode(CaptureMode mode)
{
    mMode = mode;
}

/**
 * Returns the current capture mode.
 */
CxeStillCaptureControl::CaptureMode CxeStillCaptureControlDesktop::mode() const
{
    return mMode;
}

/**
 * Operator [] - returns the indexed image from capture array.
 */
CxeStillImage &CxeStillCaptureControlDesktop::operator[](int index)
{
    return *mImages[ index ];
}

/**
 * Getter for image data queue.
 */
CxeImageDataQueue &CxeStillCaptureControlDesktop::imageDataQueue()
{
    return *mImageDataQueue;
}

/**
 * Generates a filename and sets it in the still image object.
 * Skips the process if filename already copied exists in the object. This
 * behaviour is required in rare cases where image data arrives before snapshot.
 */
CxeError::Id CxeStillCaptureControlDesktop::prepareFilename(CxeStillImageDesktop *stillImage)
{
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
    return err;
}

/*!
* Helper method to set orientation data from the orientation sensor
*/
void CxeStillCaptureControlDesktop::setOrientation(QVariant sensorData)
{
    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(sensorData);

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * Get the image container at given index or create a new one if needed.
 */
CxeStillImageDesktop* CxeStillCaptureControlDesktop::getImageForIndex(int index)
{
    CxeStillImageDesktop* image(NULL);

    if (mImages.count() <= index) {
        image = new CxeStillImageDesktop();
        image->setSnapshot(mCameraDevice.currentSnaphot());
        mImages.append(image);
    } else {
        CX_DEBUG_ASSERT( mNextImageDataIndex >= 0 && index < mImages.count() );
        image = mImages[index];
    }
    return image;
}


/*!
* Slot to handle Autofocus events.
*/
void CxeStillCaptureControlDesktop::handleAutofocusStateChanged(
                                         CxeAutoFocusControl::State newState,
                                         CxeError::Id /*error*/ )
{
    CX_DEBUG_ENTER_FUNCTION();

    mAfState = newState;
    CxeAutoFocusControl::Mode mode = mAutoFocusControl.mode();

    // if focused and in correct mode, play sound
    if  (newState == CxeAutoFocusControl::Ready &&
         mode != CxeAutoFocusControl::Hyperfocal &&
         mode != CxeAutoFocusControl::Infinity) {
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/*!
* Slot for starting viewfinder after a delay.
* Delay helps viewfinder widget to find the right, visible transparent window to attach to.
*/
void CxeStillCaptureControlDesktop::startViewfinder()
{
    CX_DEBUG_ENTER_FUNCTION();
    mViewfinderControl.start();
    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* Returns supported image qualities based on the camera index
* (primary/secondary).
*/
QList<CxeImageDetails> CxeStillCaptureControlDesktop::supportedImageQualities()
{
    return QList<CxeImageDetails>();
}

/*!
* Returns the number of images left for the current image quality setting
*/
int CxeStillCaptureControlDesktop::imagesLeft()
{
    return 100; // Stub: Dummy value
}

// end of file
