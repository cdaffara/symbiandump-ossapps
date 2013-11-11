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
* Description:  running (normal operation) application state
*
*/


#include "tlcvtstates.h"
#include "tlcvtpluginstates.h"
#include "mlcvtstatecontext.h"
#include "clcvtappshutter.h"
#include <cvtlogger.h>
#include <vtengevents.h>
#include <StringLoader.h>
#include "lcvtplugincommand.h"

TLcVtStateRunning* TLcVtStateRunning::iInstance = NULL;

// c++ constructor
TLcVtStateRunning::TLcVtStateRunning(
    MLcVtStateContext& aCtx, TLcVtStates& aUiStates )
    : TLcVtStateBase( aCtx, aUiStates )
    {
    }

// Destructor
TLcVtStateRunning::~TLcVtStateRunning()
    {
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::InstanceL
// -----------------------------------------------------------------------------
//
TLcVtStateRunning * TLcVtStateRunning::InstanceL(
    MLcVtStateContext& aCtx,
    TLcVtStates& aUiStates )
    {
    __VTPRINTENTER( "TLcVtStateRunning.InstanceL" )
    if ( iInstance == NULL )
        iInstance = new ( ELeave ) TLcVtStateRunning( aCtx, aUiStates );
    __VTPRINTEXIT( "TLcVtStateRunning.InstanceL" )
    return iInstance;
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::HandleVtEventL
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateRunning::HandleVtEventL(
    const TInt aEvent )
    {
    __VTPRINTENTER( "TLcVtStateRunning.HandleVtEventL" )
    // let TVtUiAppStateBase base class do common tasks
    TEventResponse handled = TLcVtStateBase::HandleVtEventL( aEvent );
    switch ( aEvent )
        {
    case KVtEngDeviceLockOn:
    case KVtEngDeviceLockOff:
        TLcVtStateBase::HandleVtEventL( aEvent );
        if ( aEvent == KVtEngDeviceLockOff )
            {
            iCtx.ChangeApplicationFocus( ETrue );
            }
        break;
    case KVtEngSessionStateChanged:
        if ( CheckEndActiveCallL() )
            {
            // At this point shutdown is in progress. Don't allow
            // appUi to do actions due to this event.
            handled = EEventHandled;
            }
        break;
    default:
        break;
        }
    __VTPRINTEXIT( "TLcVtStateRunning.HandleVtEventL" )
    return handled;
    }


// -----------------------------------------------------------------------------
// TLcVtStateRunning::HandleCommandL
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateRunning::HandleCommandL(
    const TInt aCommand )
    {
    __VTPRINTENTER( "TLcVtStateRunning.HandleCommandL" )
    /** By default all commands are enabled by running state.
      * Other details command validations are done elsewhere
      */  
    TEventResponse handled = EEventHandled;
    
    switch ( aCommand )
        {
    case EPluginCmdEndActiveCall:
        ShutdownWithEngineCommandL( KVtEngTerminateSession );
        break;
    case EPluginCmdCreateVoice:
        CreateVoiceCallL();
        break;
    case EPluginCmdDialEmergency:
        // Attempt emergency call. Note that emergency number check is done
        // after calling DialEmergencyCallL and if supplied number is not
        // emergency leave occurs.
        TRAP_IGNORE( DialEmergencyCallL() );
        break;
    default:
        handled = TLcVtStateBase::HandleCommandL( aCommand );
        break;
        }
        
    __VTPRINTEXITR( "TLcVtStateRunning.HandleCommandL %d", handled )
    return handled;
    }


// -----------------------------------------------------------------------------
// TLcVtStateRunning::HandleForegroundChangedL
// -----------------------------------------------------------------------------
//
TBool TLcVtStateRunning::HandleForegroundChangedL( const TBool aIsForeground )
    {
    __VTPRINTENTER( "TLcVtStateRunning.HandleForegroundChangedL" )

    iCtx.RefreshL( MLcVtStateContext::ENaviPane );

    TBool handleAsForeground( aIsForeground );
    TPtrC8 params( reinterpret_cast< TUint8* >( &handleAsForeground ),
        sizeof( TBool ) );
    iCtx.DoExecuteCmdL( KVtEngSetUIForeground, &params );
    __VTPRINTEXIT( "TLcVtStateRunning.HandleForegroundChangedL" )
    return handleAsForeground;
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::PreHandleForegroundChangedL
// -----------------------------------------------------------------------------
//
TBool TLcVtStateRunning::PreHandleForegroundChangedL( const TBool aIsForeground )
    {
    __VTPRINTENTER( "TLcVtStateRunning.PreHandleForegroundChangedL" )
    TBool handleAsForeground( aIsForeground );
    TPtrC8 params( reinterpret_cast< TUint8* >( &handleAsForeground ), sizeof( TBool ) );
    iCtx.DoExecuteCmdL( KVtEngSetUIForeground, &params );
    __VTPRINTEXIT( "TLcVtStateRunning.PreHandleForegroundChangedL" )
    return handleAsForeground;
    }
#if 0
// -----------------------------------------------------------------------------
// TLcVtStateRunning::HandleLayoutChangedL
// -----------------------------------------------------------------------------
//
void TLcVtStateRunning::HandleLayoutChangedL( )
    {
    __VTPRINTENTER( "TLcVtStateRunning.HandleLayoutChangedL" )
    iUiStates.SetLayoutChangeNeeded( EFalse );
    iCtx.DoHandleLayoutChangedL();
    __VTPRINTEXIT( "TLcVtStateRunning.HandleLayoutChangedL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::StartDtmfTone
// -----------------------------------------------------------------------------
//
void TLcVtStateRunning::StartDtmfTone( const TChar& aTone )
    {
    iCtx.StartDtmfTone( aTone );
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::StopDtmfTone
// -----------------------------------------------------------------------------
//
void TLcVtStateRunning::StopDtmfTone()
    {
    iCtx.StopDtmfTone();
    }
#endif
// -----------------------------------------------------------------------------
// TLcVtStateRunning::CreateVoiceCallL
// -----------------------------------------------------------------------------
//
void TLcVtStateRunning::CreateVoiceCallL()
    {
    __VTPRINTENTER( "TLcVtStateRunning.CreateVoiceCallL" )
    
    //TInt result( KErrNone );
    /*
    HBufC* prompt =
        StringLoader::LoadLC( R_VIDEOTELUI_QTN_QUERY_CREATE_VOICE_CALL );

    // Create and initialize query dialog
    CAknQueryDialog* createVoiceCallDlg = CAknQueryDialog::NewL();
    CleanupStack::PushL( createVoiceCallDlg );
    createVoiceCallDlg->SetPromptL( *prompt );
    CleanupStack::Pop(); // createVoiceCallDlg

    iCreateVoiceCallDlg = createVoiceCallDlg;
    TInt result( KErrNone );
    TRAPD( error, result = iCreateVoiceCallDlg->ExecuteLD(
        R_VIDEOTELUI_CREATE_VOICE_QUERY ) );
    // Just set to NULL because ExecuteLD() deletes instance
    iCreateVoiceCallDlg = NULL;
    // Leave if ExecuteLD() left
    User::LeaveIfError( error );
    CleanupStack::PopAndDestroy( ); // prompt
    if ( result )
        {
        //user answered yes
        iCtx.DoExecuteCmdL( KVtEngSwitchToVoice, NULL );
        }
        */
    __VTPRINTEXITR( "TLcVtStateRunning.CreateVoiceCallL %d", KErrNone )
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::ShutdownWithEngineCommandL
// Starts shutdown with command to engine.
// -----------------------------------------------------------------------------
//
void TLcVtStateRunning::ShutdownWithEngineCommandL( const TInt aCommand )
    {
    __VTPRINTENTER( "TLcVtStateRunning.ShutdownWithEngineCommandL" )

    TLcVtStateBase* resetState =
        TLcVtStateResetting::InstanceL( iCtx, iUiStates, aCommand ); 
    CLcVtAppShutter* shutter = CLcVtAppShutter::InstanceL( *resetState );

    if ( shutter )
    {
        // open resetting state, handles engine command request/reply
        ChangeStateL( *resetState );
#ifdef _DEBUG
        __VTPRINTEXIT( "TLcVtStateRunning.ShutdownWithEngineCommandL 1" )
        return;
#endif
    }    
   
    __VTPRINTEXIT( "TLcVtStateRunning.ShutdownWithEngineCommandL 0" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::DialEmergencyCallL
// -----------------------------------------------------------------------------
//
void TLcVtStateRunning::DialEmergencyCallL()
    {
    __VTPRINTENTER(
        "TLcVtStateRunning.DialEmergencyCallL" )
    TLcVtStateBase* resetState =
        TLcVtStateResetting::InstanceL( iCtx, iUiStates );
    CLcVtAppShutter* shutter = CLcVtAppShutter::InstanceL( *resetState );
    if ( shutter )
        {
        // Make emergency call and proceed when 1) state has given permission 
        // and 2) callback from emergency call api is received
        shutter->ShutdownWithEmergencyCallL( iCtx );
        ChangeStateL( *resetState );
        }
    __VTPRINTEXIT( "TLcVtStateRunning.DialEmergencyCallL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::OpenL
// -----------------------------------------------------------------------------
//
void TLcVtStateRunning::OpenL()
    {
    __VTPRINTENTER( "TLcVtStateRunning.OpenL" )
    SetExecState( TLcVtStates::EExecStateRunning );
    iCtx.RefreshStatesL();
    __VTPRINTEXIT( "TLcVtStateRunning.OpenL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateRunning::Close
// -----------------------------------------------------------------------------
//
void TLcVtStateRunning::Close()
    {
    __VTPRINTENTER( "TLcVtStateRunning.Close" )
    delete TLcVtStateRunning::iInstance;
    TLcVtStateRunning::iInstance = NULL;
    __VTPRINTEXIT( "TLcVtStateRunning.Close" )
    }
