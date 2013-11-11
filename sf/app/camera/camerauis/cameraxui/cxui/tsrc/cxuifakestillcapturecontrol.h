/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#ifndef CXUIFAKESTILLCAPTURECONTROL_H
#define CXUIFAKESTILLCAPTURECONTROL_H

#include <QList>
#include <ecam/camerasnapshot.h>

#include "cxestillcapturecontrol.h"
#include "cxestillcapturecontrolsymbian.h"
#include "cxenamespace.h"

class CxeSoundPlayerSymbian;
class CxeFilenameGenerator;
class CxeCameraDevice;
class CxeStillImageSymbian;
class CxeImageDataQueue;
class CxeImageDataQueueSymbian;
class CxeViewfinderControl;
class CxeCameraDeviceControl;

class CxuiFakeStillCaptureControl : public CxeStillCaptureControlSymbian

{
    Q_OBJECT

public:  // constructors
    CxuiFakeStillCaptureControl(
        CxeCameraDevice &cameraDevice,
        CxeViewfinderControl &viewfinderControl,
        CxeCameraDeviceControl &cameraDeviceControl,
        CxeSoundPlayerSymbian &soundPlayer,
        CxeFilenameGenerator &nameGenerator);

    virtual ~CxuiFakeStillCaptureControl();

public:  // from CxeStillCaptureControl
    int prepare();
    void capture();

public:  // public member functions, not in client API
    //void handleSnapshotEvent();
    void setFakeState(CxeStillCaptureControl::State aState);

signals:
    void imagePrepareComplete(int error);

signals:
    void capturedCalled();

public slots:
    //! Notification for when camera device is ready and we're entering still image mode
    void init();

    //! Notification for when we're exiting still image mode
    void deinit();

protected slots:
    // notifications when camera reference is changed
    void prepareForCameraDelete();
    void handleCameraAllocated(CxeError::Id error);

    // notification for when camera is released
    void prepareForRelease();

    // settings call back
    void handleSceneChanged(CxeScene* scene);

private:
    CCamera::CCameraSnapshot *iCameraSnapshot; // own
    CxeImageDataQueueSymbian *mImageDataQueue;  // own

    CxeCameraDevice &mCameraDevice;
    CxeViewfinderControl &mViewfinderControl;
    CxeCameraDeviceControlSymbian &mCameraDeviceControl;


    CxeSoundPlayerSymbian &mSoundPlayer;
    CxeFilenameGenerator &mFilenameGenerator;

    CaptureMode mMode;
    QList<CxeStillImageSymbian*> mImages;
    int mNextSnapshotIndex;
    int mNextImageDataIndex;
};

#endif // CXESTILLCAPTURECONTROLSYMBIAN_H
