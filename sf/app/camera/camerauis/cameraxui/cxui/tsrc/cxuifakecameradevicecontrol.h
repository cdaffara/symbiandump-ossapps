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
#ifndef CXUIFAKECAMERADEVICECONTROL_H
#define CXUIFAKECAMERADEVICECONTROL_H

//  Include Files

#include "cxecameradevicecontrolsymbian.h"
#include <QObject>
//#include "cxecameradevice.h"
//#include "cxestatemachine.h"¨

class CxeCameraDeviceControlSymbian;


class CxuiFakeCameraDeviceControl : public CxeCameraDeviceControlSymbian 
{                                    
    Q_OBJECT

public:

    CxuiFakeCameraDeviceControl();
    virtual ~CxuiFakeCameraDeviceControl();
    
public: // from CxeCameraDeviceControl

    /*Cxe::CameraMode mode() const;
    void setMode( Cxe::CameraMode mode);

    void release();

    Cxe::CameraIndex cameraIndex() const;
    void switchCamera( Cxe::CameraIndex index );

    State state() const;

    CxeCameraDevice *cameraDevice();*/

    void reserve();
    void powerOn();


protected: // from CxeStateMachine
    
    //void handleStateChanged( int newStateId, int error );

private:
    
    //void doHandleEvent( const TECAMEvent &aEvent );
    /*void initializeStates();
    void powerOn();
    void prepare(int mode);
    void newCamera( Cxe::CameraIndex cameraIndex );
    void releaseCurrentCamera();
    void doHandleEvent( const TECAMEvent &aEvent );
    void handleReserveComplete( int error );
    void handlePowerOnComplete( int error );
    void handleNoLongerReserved( int error );*/

/*protected: // from MCameraObserver2

    void HandleEvent( const TECAMEvent &aEvent );
    void ViewFinderReady( MCameraBuffer &aCameraBuffer, TInt aError );
    void ImageBufferReady( MCameraBuffer &aCameraBuffer, TInt aError );
    void VideoBufferReady( MCameraBuffer &aCameraBuffer, TInt aError );*/
    
/*signals:
    void cameraEvent(int eventUid, int error); // ECam events
    void imageBufferReady(MCameraBuffer* buffer, int error);
    void deviceReady(); // device is ready to prepared in either still or video mode 
    void vfFrameReady(MCameraBuffer* buffer, int error);*/

private:
    CxeCameraDevice *mCameraDevice; // own

    Cxe::CameraIndex mCameraIndex;
    Cxe::CameraMode  mCameraMode;
};

#endif  // CXUIFAKECAMERADEVICECONTROL_H

