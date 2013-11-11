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
#ifndef __CXEVIEWFINDERCONTROLSYMBIAN_H__
#define __CXEVIEWFINDERCONTROLSYMBIAN_H__

//  Include Files
#include <ecam.h>
#include "cxeviewfindercontrol.h"
#include "cxestatemachine.h"
#include "cxesettings.h"

//  Constants


//  Class Definitions

class CxeCameraDeviceControl;
class CxeCameraDevice;
class RWindowBase;
class CxeVideoContainer;



class CxeViewfinderControlSymbian : public CxeViewfinderControl,
                                    public CxeStateMachine
{
    Q_OBJECT

public:

    CxeViewfinderControlSymbian(CxeCameraDevice &cameraDevice,
            CxeCameraDeviceControl &mCameraDeviceControl);
    virtual ~CxeViewfinderControlSymbian();

public: // from base class

    void setWindow(WId windowId);

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
    * @returns CxeError::Id error code
    */
    CxeError::Id start();

    /*!
    * Returns Device's Display resolution
    */
    QSize deviceDisplayResolution() const;

protected: // from CxeStateMachine
    void handleStateChanged(int newStateId, CxeError::Id error);

protected:
    void initializeStates();

protected slots:
    // notifications when camera reference is changed
    void prepareForCameraDelete();
    void handleCameraAllocated(CxeError::Id error);
    void handleVfFrame(MCameraBuffer* buffer, int error);
    void prepareForRelease();

private:
    /*
    * Intializes viewfinder based on VF mode
    * returns CxeEngine Std error code
    */
    int initViewfinder();
    void createViewfinderWindowL();
    void releaseCurrentViewfinder();

private:
    // data members
    CxeCameraDevice &mCameraDevice;
    CxeCameraDeviceControl &mCameraDeviceControl;

    RWindowBase *mUiWindow;
    RWindowBase *mVideoWindow;
    CxeVideoContainer *mVideoContainer;

    TRect mWindowRect;
    CCamera::CCameraDirectViewFinder *mDirectViewfinder;

    bool mDirectViewfinderInUse;
};



#endif  // __CXEVIEWFINDERCONTROLSYMBIAN_H__

