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
#ifndef CXESTILLCAPTURECONTROLDESKTOP_H
#define CXESTILLCAPTURECONTROLDESKTOP_H

#include <QList>
#include <QVariant>
#include <QTimer>

#include "cxestillcapturecontrol.h"
#include "cxeimagedataqueuedesktop.h"
#include "cxenamespace.h"
#include "cxeerror.h"
#include "cxeautofocuscontrol.h"

class CxeFilenameGenerator;
class CxeFileSaveThread;
class CxeStillImageDesktop;
class CxeViewfinderControl;
class CxeCameraDeviceControl;
class CxeCameraDeviceDesktop;



/**
 * Class CxeStillCaptureControlSymbian.
 * Inherits/implements CxeStillCaptureControl.
 */
class CxeStillCaptureControlDesktop : public CxeStillCaptureControl
{
    Q_OBJECT

public:  // constructors

    CxeStillCaptureControlDesktop(CxeCameraDeviceDesktop &cameraDevice,
                                  CxeViewfinderControl &viewfinderControl,
                                  CxeCameraDeviceControl &cameraDeviceControl,
                                  CxeFilenameGenerator &nameGenerator,
                                  CxeAutoFocusControl &autoFocusControl,
                                  CxeFileSaveThread &saveThread);

    virtual ~CxeStillCaptureControlDesktop();

public:  // from CxeStillCaptureControl

    void capture();
    State state() const;
    int imageCount() const;
    void reset();
    void cancelAll();
    void setMode(CaptureMode mode);
    CaptureMode mode() const;
    CxeStillImage &operator[](int index);
    CxeImageDataQueue &imageDataQueue();
    QList<CxeImageDetails> supportedImageQualities();
    int imagesLeft();

signals:
    void imagePrepareComplete(CxeError::Id error);
    void prepareZoomForStill(int ecamStillResolutionIndex);

public slots:
    void init();
    void deinit();

protected slots:
    // notifications when camera reference is changed
    void prepareForCameraDelete();
    void handleCameraAllocated(CxeError::Id error);

    // notification for when camera is released
    void prepareForRelease();

    // settings call back
    void handleSettingValueChanged(const QString &settingId, QVariant newValue);
    void handleSceneChanged(CxeScene &scene);

    // Autofocus events
    void handleAutofocusStateChanged(CxeAutoFocusControl::State newState, CxeError::Id error);

private slots:
    void startViewfinder();

private: // helper functions
    void updateFlashSetting(QVariant newValue);
    void updateISOSetting(QVariant newValue);
    void updateEVCompensationSetting(QVariant newValue);
    CxeError::Id prepareFilename(CxeStillImageDesktop *stillImage);
    void setOrientation(QVariant sensorData);
    CxeStillImageDesktop *getImageForIndex(int index);
    int prepareStillSnapshot();
    CxeError::Id getImageQualityDetails(CxeImageDetails &imageInfo);
    void handleSnapshotEvent(CxeError::Id error);
    void initializeStates();
    void prepare();

private: // private data

    State mState;
    CxeImageDataQueueDesktop *mImageDataQueue;  // own
    CxeCameraDeviceDesktop &mCameraDevice;
    CxeViewfinderControl &mViewfinderControl;
    CxeCameraDeviceControl &mCameraDeviceControl;
    CxeFilenameGenerator &mFilenameGenerator; //! @todo could be shared with video capture control?
    CxeAutoFocusControl &mAutoFocusControl;
    CaptureMode mMode;
    CxeAutoFocusControl::State mAfState;
    QList<CxeStillImageDesktop*> mImages;
    CxeFileSaveThread &mSaveThread;

    QTimer mViewFinderStartTimer;
    
    int mNextSnapshotIndex;
    int mNextImageDataIndex;
    //still resolutions supported by ICM
    QList<CxeImageDetails> mSupportedImageQualities;
    //still resolutions supported by ecam
    //current image quality details
    CxeImageDetails mCurrentImageDetails;
};

#endif // CXESTILLCAPTURECONTROLDESKTOP_H
