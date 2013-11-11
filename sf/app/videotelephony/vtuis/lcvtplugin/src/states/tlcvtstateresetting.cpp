/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  waiting details application state
*
*/


#include "tlcvtstates.h"
#include "tlcvtpluginstates.h"
#include "mlcvtstatecontext.h"
#include "clcvtappshutter.h"
#include <cvtlogger.h>
#include <vtengevents.h>

TLcVtStateResetting* TLcVtStateResetting::iInstance = NULL;

// c++ constructor
TLcVtStateResetting::TLcVtStateResetting( 
    MLcVtStateContext& aCtx, 
    TLcVtStates& aUiStates, 
    const TVtEngCommandId aCommand )
    : TLcVtStateBase( aCtx, aUiStates ), 
      iCommand( aCommand )
    {
        __VTPRINT2( DEBUG_GEN, "TLcVtStateResetting.ctr iCommand=%d", iCommand )
        iCtx.StartShutdown();
    }

// Destructor
TLcVtStateResetting::~TLcVtStateResetting()
    {
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::
// -----------------------------------------------------------------------------
//
TLcVtStateResetting* TLcVtStateResetting::InstanceL( 
    MLcVtStateContext& aCtx, 
    TLcVtStates& aUiStates,
    const TVtEngCommandId aCommand )
    {
    __VTPRINTENTER( "TLcVtStateResetting.InstanceL" )
    if ( iInstance == NULL )
        {
        iInstance = new ( ELeave ) TLcVtStateResetting( 
            aCtx, aUiStates, aCommand );
        }
    else if ( aCommand != KVtEngCommandNone )
        { // if call ending is UI originated, command is given
        iInstance->iCommand  = aCommand;
        __VTPRINT2( DEBUG_GEN, "TLcVtStateResetting.InstanceL iCommand=%d", 
            iInstance->iCommand )
        }
    __VTPRINTEXIT( "TLcVtStateResetting.InstanceL" )
    return iInstance;
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::ShutdownDoneL
// AppUi has finished its own shutdown steps. Notify app shutter that it may 
// proceed.
// 
// -----------------------------------------------------------------------------
//
void TLcVtStateResetting::ShutdownDoneL()
    {
    __VTPRINTENTER( "TLcVtStateResetting.ShutdownDone" )
    CLcVtAppShutter* shutter = CLcVtAppShutter::InstanceL( *this );
    shutter->StartShutdown();
    __VTPRINTEXIT( "TLcVtStateResetting.ShutdownDone" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::AppUiStartupFailedL
// -----------------------------------------------------------------------------
//
void TLcVtStateResetting::LcVtStartupFailedL()
    {
    __VTPRINTENTER( "TLcVtStateResetting.LcVtStartupFailedL" )
    // no-op (overrides default behavior)
    __VTPRINTEXIT( "TLcVtStateResetting.LcVtStartupFailedL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::HandleVTCommandPerformedL
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateResetting::HandleVTCommandPerformedL
    ( 
    TVtEngCommandId aCommand,
    const TInt /*aError*/ )
    {
    __VTPRINTENTER( "TLcVtStateResetting.HandleVTCommandPerformedL" )
    // By default responses to commands are ignored while resetting
    TEventResponse handled = EEventHandled;
    switch ( aCommand )
        {
        case KVtEngResetEngine:
            // Allow UI to process reset command response        
            handled = EEventNotHandled;
            break;
        case KVtEngTerminateSession:
        case KVtEngSwitchToVoice:
            // 
            handled = EEventNotHandled;
            iCtx.ShutdownL();
            break;
        default:
            break;            
        }
    __VTPRINTEXIT( "TLcVtStateResetting.HandleVTCommandPerformedL" )
    return handled;
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::HandleVtEventL
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateResetting::HandleVtEventL(
    const TInt aEvent )
    {
    __VTPRINTENTER( "TLcVtStateResetting.HandleVtEventL" )
    TEventResponse handled = EEventHandled;
    switch ( aEvent )
        {
    // While resetting appUi should not handle these events.
    case KVtEngSourceChanged:
    case KVtEngCameraChanged:
    case KVtEngSourceCapsChanged:
    case KVtEngSessionStateChanged:
    case KVtEngDurationSettingChanged:
    case KVtEngAudioOutputVolumeChanged:
    case KVtEngAudioRoutingAvailabilityChanged:
    case KVtEngAudioRoutingChanged:
    case KVtEngAudioMuted:
    case KVtEngAudioUnmuted:
    case KVtEngRemoteVideoStopped:
    case KVtEngSessionWaitingCallActive:
    case KVtEngSessionWaitingCallInactive:
        break;
    default:
        // allow appUi to handle other events
        handled = EEventNotHandled;
        break;
        }
    __VTPRINTEXIT( "TLcVtStateResetting.HandleVtEventL" )
    return handled;
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::HandleCommandL
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateResetting::HandleCommandL(
    const TInt /*aCommand*/ )
    {
    __VTPRINTENTER( "TLcVtStateResetting.HandleCommandL" )
    /* All commands ignored while resetting. CAknAppUi::Exit() is called
       when shutdown is completed with asistance of CVtUiAppShutter */
    __VTPRINTEXIT( "TLcVtStateResetting.HandleCommandL" )
     return EEventHandled;
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::HandleForegroundChangedL
// -----------------------------------------------------------------------------
//
TBool TLcVtStateResetting::HandleForegroundChangedL(
    const TBool /*aIsForeground*/ )
    {
    __VTPRINTENTER( "TLcVtStateResetting.HandleForegroundChangedL" )
    __VTPRINTEXIT( "TLcVtStateResetting.HandleForegroundChangedL" )
    // not on "foreground" while resetting
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::PreHandleForegroundChangedL
// -----------------------------------------------------------------------------
//
TBool TLcVtStateResetting::PreHandleForegroundChangedL(
    const TBool /*aIsForegroung*/ )
    {
    // no default behavior
    return EFalse;
    }

#if 0
// -----------------------------------------------------------------------------
// TLcVtStateResetting::HandleLayoutChangedL
// -----------------------------------------------------------------------------
//
void TLcVtStateResetting::HandleLayoutChangedL()
    {
    __VTPRINTENTER( "TLcVtStateResetting.HandleLayoutChangedL" )
    // no-op (overrides default behavior)
    __VTPRINTEXIT( "TLcVtStateResetting.HandleLayoutChangedL" )
    }
#endif        
// -----------------------------------------------------------------------------
// TLcVtStateResetting::OpenL
// -----------------------------------------------------------------------------
//
void TLcVtStateResetting::OpenL()
    {
    __VTPRINTENTER( "TLcVtStateResetting.OpenL" )
    SetExecState( TLcVtStates::EExecStateResetting );
    //iCtx.StopDtmfTone();
    iCtx.ChangeApplicationFocus( EFalse );
    
    if ( iCommand != KVtEngCommandNone )
        {        
        // Command is either KVtEngTerminateSession or KVtEngSwitchToVoice
        // Need to wait for callback before ShutdownL can be called on context.
        iCtx.DoExecuteCmdL( iCommand, NULL );
        }
    else
        {
        // This branch is executed when resetting state is opened due
        // to emergency call or state change to one causing shutdown.
        // No need to wait for any callbacks (emergency call callback
        // handled by CVtUiAppShutter).
        iCtx.ShutdownL();
        }
    __VTPRINTEXIT( "TLcVtStateResetting.OpenL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateResetting::Close
// -----------------------------------------------------------------------------
//
void TLcVtStateResetting::Close()
    {
    __VTPRINTENTER( "TLcVtStateResetting.Close" )
    SetExecState( TLcVtStates::EExecStateFinalized );
    if ( TLcVtStateResetting::iInstance )
        {
        delete TLcVtStateResetting::iInstance;
        TLcVtStateResetting::iInstance = NULL;
        }
    __VTPRINTEXIT( "TLcVtStateResetting.Close" )
    }
