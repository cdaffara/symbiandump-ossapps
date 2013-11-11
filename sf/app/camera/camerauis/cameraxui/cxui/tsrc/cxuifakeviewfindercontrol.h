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
#ifndef __CXUIFAKEVIEWFINDERCONTROL_H__
#define __CXUIFAKEVIEWFINDERCONTROL_H__

//  Include Files
#include "cxeviewfindercontrolsymbian.h"
#include "cxestatemachine.h"
#include "cxesettings.h"

//  Constants


//  Class Definitions

class CxeCameraDeviceControlSymbian;
class CxeCameraDevice;
class RWindowBase;

class CxuiFakeViewfinderControl : public CxeViewfinderControlSymbian

{
    Q_OBJECT

public:

    CxuiFakeViewfinderControl( CxeCameraDevice &cameraDevice,
            CxeCameraDeviceControlSymbian &mCameraDeviceControl);
    virtual ~CxuiFakeViewfinderControl();

public: // from base class

    /*
    * stops the view finder.
    */
    void stop();
    
    /*
    * Get current state of view finder control
    * returns one of Vf states defined above.
    */
    State state() const;
    
    /*
    * Start view finder
    * @return CxeEngine specific error code
    */
    CxeError::Id start();

    /*
    * Removes Viewfinder instance
    */
    void releaseCurrentViewfinder();

protected: // from CxeStateMachine
    void handleStateChanged( int newStateId, CxeError::Id error );

private:
    /*
    * Intializes viewfinder based on VF mode
    * returns CxeEngine Std error code
    */
   int initViewfinder();

private:

    CxeCameraDevice &mCameraDevice;
    CxeCameraDeviceControlSymbian &mCameraDeviceControl;
    
    RWindowBase *mWindow;
    TRect mWindowRect;
    CCamera::CCameraDirectViewFinder *mDirectViewfinder;
    
    bool mDirectViewfinderInUse;
};



#endif  // __CXUIFAKEVIEWFINDERCONTROL_H__

