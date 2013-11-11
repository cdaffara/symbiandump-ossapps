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

#include <QMetaType>
#include <QImage>
//#include <fbs.h>
//#include <ecam.h> // CCamera
//#include <ecam/ecamadvsettingsintf.h> // CCamera

#include "cxesettings.h"
#include "cxuifakeautofocuscontrol.h"
#include "cxutils.h"
#include "cxecameradevicecontrolsymbian.h" // CxeCameraDevice
#include "cxesettingsmappersymbian.h"


/*
* CxeAutoFocusControlSymbian::CxeAutoFocusControlSymbian
*/
CxuiFakeAutoFocusControl::CxuiFakeAutoFocusControl(CxeCameraDevice &cameraDevice)
    : CxeAutoFocusControlSymbian(cameraDevice),
      mCameraDevice( cameraDevice ),
      mAdvancedSettings( NULL )
{
    CX_DEBUG_ENTER_FUNCTION();

    /*qRegisterMetaType<CxeAutoFocusControl::State>();

    initializeStates();

    // connect signals from cameraDevice, so we recieve events when camera reference changes
    QObject::connect( &cameraDevice,
                      SIGNAL(prepareForCameraDelete()),
                      this,SLOT(prepareForCameraDelete()) );

    QObject::connect( &cameraDevice,
                      SIGNAL(cameraAllocated()),
                      this,SLOT(cameraAllocated()) );

    QObject::connect( &cameraDevice,
                      SIGNAL(prepareForRelease()),
                      this,SLOT(prepareForRelease()) );

    initializeResources();*/

    CX_DEBUG_EXIT_FUNCTION();
}



/*
*CxuiFakeAutoFocusControl::initAdvancedSettings
*/
/*void CxuiFakeAutoFocusControl::initializeResources()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_ASSERT( mCameraDevice.camera() );

    mAdvancedSettings = mCameraDevice.advancedSettings();

    CX_DEBUG_EXIT_FUNCTION();
}*/



/*
* CxuiFakeAutoFocusControl::~CxuiFakeAutoFocusControl
*/
CxuiFakeAutoFocusControl::~CxuiFakeAutoFocusControl()
{
    CX_DEBUG_ENTER_FUNCTION();
    CX_DEBUG_EXIT_FUNCTION();
}


/*
* Start Autofocus
*/
int CxuiFakeAutoFocusControl::start()
{
    CX_DEBUG( ("CxuiFakeAutoFocusControl::start() <> state: %d", state() ) );

    int err = KErrNone;
    setState( InProgress );
    handleAfEvent( KUidECamEventCameraSettingsOptimalFocusUidValue, err);

    return(err);
}



/*
* Cancel Autofocus
*/
void CxuiFakeAutoFocusControl::cancel()
{
    CX_DEBUG( ("CxuiFakeAutoFocusControl::cancel <> state: %d", state() ) );
    setState( Canceling );
    handleAfEvent( KUidECamEventCameraSettingsOptimalFocusUidValue, KErrNone );
}



/*
* Set Autofocus mode
*/
void CxuiFakeAutoFocusControl::setMode(CxeAutoFocusControl::Mode newMode)
{
    CX_DEBUG_ENTER_FUNCTION();
    mAfMode = newMode;
    CX_DEBUG_EXIT_FUNCTION();
}


/*
* returns Autofocus mode
*/

CxeAutoFocusControl::Mode CxuiFakeAutoFocusControl::mode() const
{
    return mAfMode;
}



/*
* To check if Autofocus is supported
*/
bool CxuiFakeAutoFocusControl::supported() const
{
    return true;
}

CxeAutoFocusControl::State CxuiFakeAutoFocusControl::state() const
{
    return static_cast<State>( stateId() );
}


void CxuiFakeAutoFocusControl::handleStateChanged( int newStateId, CxeError::Id error )
{
    CX_DEBUG_ENTER_FUNCTION();
    emit stateChanged(static_cast<State>(newStateId), error);
}

/*
* CxuiFakeAutoFocusControl::handleAfEvent
*/
void CxuiFakeAutoFocusControl::handleAfEvent(int eventUid, int error)
{
    CX_DEBUG_ENTER_FUNCTION();

    switch ( state() ) {
    case CxeAutoFocusControl::InProgress: {
        if(eventUid == KUidECamEventCameraSettingsOptimalFocusUidValue ) {
            CX_DEBUG(("CxuiFakeAutoFocusControl::handleAfEvent <> KUidECamEventCameraSettingsOptimalFocus"));
            if (KErrNone == error) {
                setState(CxeAutoFocusControl::Ready);
            } else {
                setState(CxeAutoFocusControl::Failed, error);
            }
         }
         break;
        }
    case CxeAutoFocusControl::Canceling: {
        CX_DEBUG(("CxuiFakeAutoFocusControl::handleAfEvent <> Canceling"));
        // Focus position is not known after cancel
        setState(CxeAutoFocusControl::Unknown);
        break;
    }
    default:
        break;
    } // end switch

}

int CxuiFakeAutoFocusControl::setFakeState(CxeAutoFocusControl::State aState)
{
    int err = KErrNone;
    setState(aState);
    return err;
}

// end of file
