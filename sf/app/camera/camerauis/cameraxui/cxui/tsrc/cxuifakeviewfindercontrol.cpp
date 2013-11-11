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
#include <w32std.h> // RWindow, RWsSession.
#include <coemain.h> // CCoeEnv
#include <coecntrl.h>
#include "cxutils.h"
#include "cxecameradevicecontrolsymbian.h" // CxeCameraDevice
#include "cxuifakeviewfindercontrol.h"
#include "cxesettings.h"
#include "cxesettingsmappersymbian.h"


/*
* CxuiFakeViewfinderControl::CxuiFakeViewfinderControl
*/
CxuiFakeViewfinderControl::CxuiFakeViewfinderControl(
    CxeCameraDevice &cameraDevice,
    CxeCameraDeviceControlSymbian &cameraDeviceControl )
    : CxeViewfinderControlSymbian(cameraDevice, cameraDeviceControl),
      mCameraDevice(cameraDevice),
      mCameraDeviceControl(cameraDeviceControl),
      mWindow(NULL),
      mDirectViewfinder(NULL),
      mDirectViewfinderInUse(true)
{
    CX_DEBUG_IN_FUNCTION();
}



/*
* CxuiFakeViewfinderControl::~CxuiFakeViewfinderControl()
*/
CxuiFakeViewfinderControl::~CxuiFakeViewfinderControl()
{
    CX_DEBUG_ENTER_FUNCTION();

    stop();

    CX_DEBUG_EXIT_FUNCTION();
}


/*
* Stop viewfinder
*/
void CxuiFakeViewfinderControl::stop()
{
    CX_DEBUG_ENTER_FUNCTION();

    if (state() == Running) {
        setState(Ready);
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/**
 * Start the viewfinder
 */

int CxuiFakeViewfinderControl::start()
{
    CX_DEBUG_ENTER_FUNCTION();
    TInt err = KErrNone;
    
    if (state() == Running) {
        CX_DEBUG( ( "Viewfinder already running - ignored start()" ) );
        CX_DEBUG_EXIT_FUNCTION();
        return 0;
    }

    if (state() == Uninitialized) {
        err = initViewfinder();
    }

    // apply any settings here

    if (!err  && state() == Ready) {
        setState( Running );
    }

    CX_DEBUG(("CxuiFakeViewfinderControl::start symbian error code : %d", err));
    CX_DEBUG_EXIT_FUNCTION();
    return err;
}


/*
* Intialize the viewfinder based on the VF mode
*/ 

int CxuiFakeViewfinderControl::initViewfinder()
{
    CX_DEBUG_ENTER_FUNCTION();

    // For now only direct vf. If there is any need for supporting other VF modes, condition checks
    // and handling of new states are needed here. 
    setState(Ready);

    CX_DEBUG_EXIT_FUNCTION();
    return KErrNone;
}

/*
* Release Vf, when we change mode or during shutdown or ?
*/ 

void CxuiFakeViewfinderControl::releaseCurrentViewfinder()
{
    CX_DEBUG_ENTER_FUNCTION(); 

    setState(Uninitialized);

    CX_DEBUG_EXIT_FUNCTION();
}

CxeViewfinderControl::State CxuiFakeViewfinderControl::state() const
{
    return static_cast<State>(stateId());
}

void CxuiFakeViewfinderControl::handleStateChanged(int newStateId, CxeError::Id error)
{
    emit stateChanged(static_cast<State>(newStateId), error);
}

// end of file
