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
#ifndef CXESTILLCAPTURECONTROLSYMBIAN_H
#define CXESTILLCAPTURECONTROLSYMBIAN_H

#include <QList>
#include <ecam.h>
#include <QPixmap>
#include <QVariant>

#include "cxestillcapturecontrol.h"
#include "cxeautofocuscontrol.h"
#include "cxesensoreventhandler.h"
#include "cxestatemachine.h"
#include "cxenamespace.h"
#include "cxeerror.h"

class CxeSoundPlayerSymbian;
class CxeFilenameGenerator;
class CxeFileSaveThread;
class CxeCameraDevice;
class MCameraBuffer;
class CxeStillImageSymbian;
class CxeImageDataQueue;
class CxeImageDataQueueSymbian;
class CxeViewfinderControl;
class CxeSnapshotControl;
class CxeSensorEventHandler;
class CxeCameraDeviceControl;
class CxeAutoFocusControl;
class CxeSettings;
class CxeQualityPresets;
class CxeDiskMonitor;


/**
 * Class CxeStillCaptureControlSymbian.
 * Inherits/implements CxeStillCaptureControl.
 */
class CxeStillCaptureControlSymbian : public CxeStillCaptureControl,
                                      public CxeStateMachine
{
    Q_OBJECT

public:  // constructors

    CxeStillCaptureControlSymbian(CxeCameraDevice &cameraDevice,
                                  CxeViewfinderControl &viewfinderControl,
                                  CxeSnapshotControl &snapshotControl,
                                  CxeCameraDeviceControl &cameraDeviceControl,
                                  CxeFilenameGenerator &nameGenerator,
                                  CxeSensorEventHandler &sensorEventHandler,
                                  CxeAutoFocusControl &autoFocusControl,
                                  CxeSettings &settings,
                                  CxeQualityPresets &qualityPresets,
                                  CxeFileSaveThread &fileSaveThread,
                                  CxeDiskMonitor &diskMonitor);

    virtual ~CxeStillCaptureControlSymbian();

public:  // from CxeStillCaptureControl

    void capture();
    State state() const;
    int imageCount() const;
    void reset();
    void cancelAll();
    void setMode( CaptureMode mode );
    CaptureMode mode() const;
    CxeStillImage &operator[]( int index );
    CxeImageDataQueue &imageDataQueue();
    QList<CxeImageDetails> supportedImageQualities();
    int imagesLeft();

public slots:
    void init();
    void deinit();

signals:
    void imagePrepareComplete(CxeError::Id error);
    void prepareZoomForStill(int ecamStillResolutionIndex);

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);


protected slots:
    // notifications when camera reference is changed
    void prepareForCameraDelete();
    void handleCameraAllocated(CxeError::Id error);

    // notification for when camera is released
    void prepareForRelease();

    // ECAM events
    void handleCameraEvent(int eventUid, int error);

    // Image data event
    void handleImageData(MCameraBuffer *buffer, int error);

    // Snapshot event
    void handleSnapshotReady(CxeError::Id status, const QImage &snapshot);

    // settings call back
    void handleSettingValueChanged(const QString& settingId,QVariant newValue);

    // Disk space change
    void handleDiskSpaceChanged();

    // Autofocus events
    void handleAutofocusStateChanged(CxeAutoFocusControl::State newState, CxeError::Id error);

    // sensor events
    void handleSensorEvent(CxeSensorEventHandler::SensorType type,QVariant data);

    // Use ECam Use Case Hint Custom API to inform ECam of our intended use case
    // before calling Reserve()
    void hintUseCase();

private: // helper functions

    CxeError::Id prepareFilename(CxeStillImageSymbian *stillImage);
    void setOrientation(QVariant sensorData);
    CxeStillImageSymbian* getImageForIndex(int index);
    CCamera::TFormat supportedStillFormat(Cxe::CameraIndex cameraIndex);
    void prepareSnapshot();
    void updateStillCaptureParameters();
    void handleSnapshotEvent(CxeError::Id error);
    void initializeStates();
    void prepare();
    void updateRemainingImagesCounter();
    int calculateRemainingImages(int estimatedImagesize);

private: // private data

    CxeImageDataQueueSymbian *mImageDataQueue;  // own
    CxeCameraDevice &mCameraDevice;
    CxeViewfinderControl &mViewfinderControl;
    CxeSnapshotControl &mSnapshotControl;
    CxeCameraDeviceControl &mCameraDeviceControl;
    CxeFilenameGenerator &mFilenameGenerator; //! @todo could be shared with video capture control?
    CxeSensorEventHandler &mSensorEventHandler;
    CxeAutoFocusControl &mAutoFocusControl;
    CxeSettings &mSettings;
    CxeQualityPresets &mQualityPresets;
    CxeFileSaveThread &mFileSaveThread;
    CxeDiskMonitor &mDiskMonitor;
    CxeSoundPlayerSymbian *mCaptureSoundPlayer;
    CxeSoundPlayerSymbian *mAutoFocusSoundPlayer;
    CaptureMode mMode;
    CxeAutoFocusControl::State mAfState;
    QList<CxeStillImageSymbian*> mImages;
    int mNextSnapshotIndex;
    int mNextImageDataIndex;
    //still resolutions supported by ICM
    QList<CxeImageDetails> mIcmSupportedImageResolutions;
    //still resolutions supported by ecam
    QList<TSize> mECamSupportedImageResolutions;
    //current image quality details in use
    CxeImageDetails mCurrentImageDetails;
    CCamera::TFormat mCaptureFormat;
    int mSizeIndex; //!< ECam still capture size index
};

#endif // CXESTILLCAPTURECONTROLSYMBIAN_H
