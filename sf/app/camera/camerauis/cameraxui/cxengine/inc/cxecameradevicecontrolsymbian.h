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
#ifndef CXECAMERADEVICECONTROLSYMBIAN_H
#define CXECAMERADEVICECONTROLSYMBIAN_H

//  Include Files
#include <QTimer>

#include "cxecameradevicecontrol.h"
#include "cxecameradevice.h"
#include "cxestatemachine.h"
#include "cxeerror.h"

class CxeCameraDeviceControlSymbian : public CxeCameraDeviceControl,
                                      public CxeStateMachine,
                                      public MCameraObserver2
{
    Q_OBJECT

public:

    CxeCameraDeviceControlSymbian();
    virtual ~CxeCameraDeviceControlSymbian();

public: // from CxeCameraDeviceControl

    /**
    * Calls newCamera and emits initModeComplete() on error.
    * This is in a separate method and not in the constructor so that
    * newCamera() can be properly overridden in derived classes.
    */
    void init();
    Cxe::CameraMode mode() const;
    void setMode(Cxe::CameraMode mode);
    void reserve();
    void release();
    Cxe::CameraIndex cameraIndex() const;
    CxeError::Id switchCamera(Cxe::CameraIndex index);
    State state() const;
    CxeCameraDevice *cameraDevice();

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);

protected:
    void initializeStates();
    void powerOn();
    void prepare(int mode);
    void releaseCurrentCamera();
    void doHandleEvent(const TECAMEvent &aEvent);
    void handleReserveComplete(int error);
    void handlePowerOnComplete(int error);
    void handleNoLongerReserved(int error);

protected: // from MCameraObserver2
    void HandleEvent(const TECAMEvent &aEvent);
    void ViewFinderReady(MCameraBuffer &aCameraBuffer, TInt aError);
    void ImageBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);
    void VideoBufferReady(MCameraBuffer &aCameraBuffer, TInt aError);

signals:
    void cameraEvent(int eventUid, int error); // ECam events
    void imageBufferReady(MCameraBuffer* buffer, int error);
    void deviceReady(); // device is ready to prepared in either still or video mode
    void vfFrameReady(MCameraBuffer* buffer, int error);

private slots:
    void doReserve();

protected: // Protected data so that unit test cases can replace
           // mCameraDevice with a fake implementation.
    CxeCameraDevice *mCameraDevice; // own

protected: // data
    Cxe::CameraIndex mCameraIndex;
    Cxe::CameraMode  mCameraMode;
    QTimer mReserveTimer;
};

#endif  // CXECAMERADEVICECONTROLSYMBIAN_H

