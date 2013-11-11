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
#include <vtengcommands.h>
#include <cvtengmodel.h>
#include <mvtengsessioninfo.h>


TLcVtStateStarting* TLcVtStateStarting::iInstance = NULL;

// c++ constructor
TLcVtStateStarting::TLcVtStateStarting(
    MLcVtStateContext& aCtx, TLcVtStates& aUiStates ) 
    : TLcVtStateBase( aCtx, aUiStates )
    {
    iPreconditions.Set( EWaitingCLI );
    iPreconditions.Set( EWaitingPhase1ToComplete );
    iStartCallback = NULL;
    }

// Destructor
    TLcVtStateStarting::~TLcVtStateStarting()
    {
    delete iStartCallback;
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::InstanceL
//
// -----------------------------------------------------------------------------
//
TLcVtStateStarting * TLcVtStateStarting::InstanceL(
    MLcVtStateContext& aCtx, TLcVtStates& aUiStates )
    {
    __VTPRINTENTER( "TLcVtStateStarting.InstanceL" )
    if ( iInstance == NULL )
        {
        iInstance = new ( ELeave ) TLcVtStateStarting( aCtx, aUiStates );
        }
    __VTPRINTEXIT( "TLcVtStateStarting.InstanceL" )
    return iInstance;
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::LcVtStartupPhase1DoneL
//
// Phase 1 of application startup is completed, check if we can proceed to
// phase 2.
// -----------------------------------------------------------------------------
//
void TLcVtStateStarting::LcVtStartupPhase1DoneL()
    {
    __VTPRINTENTER( "TLcVtStateStarting.LcVtStartupPhase1DoneL" )
    // From startup phase point of view we're ready to proceed
    // to phase 2. Clear that precondition and proceed if otherwise ready.
    CheckPhase2StartupL( EWaitingPhase1ToComplete );
    __VTPRINTEXIT( "TLcVtStateStarting.LcVtStartupPhase1DoneL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::LcVtStartupPhase2DoneL
//
// -----------------------------------------------------------------------------
//
void TLcVtStateStarting::LcVtStartupPhase2DoneL()
    {
    __VTPRINTENTER( "TLcVtStateStarting.LcVtStartupPhase2DoneL" )
    // Phase 2 completed, proceed to next state
    ChangeStateL( *TLcVtStateRunning::InstanceL( iCtx, iUiStates ) );
    __VTPRINTEXIT( "TLcVtStateStarting.LcVtStartupPhase2DoneL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::HandleVtEventL
//
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateStarting::HandleVtEventL(
    const TInt aEvent )
    {
    __VTPRINTENTER( "TLcVtStateStarting.HandleVtEventL" )
    TEventResponse handled = EEventHandled;
    switch ( aEvent )
        {
    case KVtEngRemoteDisconnect:
        // base class handles this.
        handled = TLcVtStateBase::HandleVtEventL( aEvent );
        break;
    case KVtEngSessionStateChanged:
        {
        // check if session state is proper for proceeding to next phase
        // in startup.
        CheckSessionStateL();
        }
        break;
    case KVtEngRemoteVideoStarted:
        // allow to start remote video rendering
    case KVtEngSourceChanged:
        // allow handling of source changed event
        handled = EEventNotHandled; 
        break;
    case KVtEngCLIAvailable:
        HandleCLIReceivedL();
        break;
  
    case KVtEngDeviceLockOn:
    case KVtEngDeviceLockOff:
    default:
        handled = TLcVtStateBase::HandleVtEventL( aEvent );
        break;
        }
    __VTPRINTEXIT( "TLcVtStateStarting.HandleVtEventL" )
    return handled;
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::HandleCommandL
//
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateStarting::HandleCommandL(
    const TInt /*aCommand*/ )
    {
    __VTPRINTENTER( "TLcVtStateStarting.HandleCommandL" )
    __VTPRINTEXIT( "TLcVtStateStarting.HandleCommandL" )
    //return EEventNotHandled;
    return EEventHandled;
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::HandleForegroundChangedL
//
// -----------------------------------------------------------------------------
//
TBool TLcVtStateStarting::HandleForegroundChangedL( const TBool aIsForeground )
    {
    __VTPRINTENTER( "TLcVtStateStarting.HandleForegroundChangedL" )        
    
    iCtx.RefreshL( MLcVtStateContext::ENaviPane );
    
    // behave as in background if CLI (=details) has not yet received
    TBool handleAsForeground( aIsForeground && iUiStates.IsDetailsReceived() ); 
    TLcVtStateBase::ExecuteEngineCommand( KVtEngSetUIForeground, 
        handleAsForeground );
    
    __VTPRINTEXIT( "TLcVtStateStarting.HandleForegroundChangedL" )
    return handleAsForeground;
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::PreHandleForegroundChangedL
// -----------------------------------------------------------------------------
//
TBool TLcVtStateStarting::PreHandleForegroundChangedL(
    const TBool /*aIsForegroung*/ )
    {
    // no default behavior
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::OpenL
// -----------------------------------------------------------------------------
//
void TLcVtStateStarting::OpenL()
    {
    __VTPRINTENTER( "TLcVtStateStarting.OpenL" )
    iStartCallback = new ( ELeave ) CAsyncCallBack (
            TCallBack( &AsynchOpen, this ),
            CActive::EPriorityStandard );
    iStartCallback->CallBack();  
    __VTPRINTEXIT( "TLcVtStateStarting.OpenL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::AsynchOpen
// asynchronous entry to the state.
// -----------------------------------------------------------------------------
//
TInt TLcVtStateStarting::AsynchOpen( TAny* aAny )
    {
    __VTPRINTENTER( "TLcVtStateStarting.AsynchOpen" )
    TLcVtStateStarting* state = reinterpret_cast<TLcVtStateStarting*>( aAny );
    delete state->iStartCallback;
    state->iStartCallback = NULL;
    TRAPD( res, state->DoOpenL() );
    __VTPRINTEXITR( "TLcVtStateStarting.AsynchOpen err=%d", res )
    return res;
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::DoOpenL
// -----------------------------------------------------------------------------
//
void TLcVtStateStarting::DoOpenL()
    {
    __VTPRINTENTER( "TLcVtStateStarting.DoOpenL" )
    if ( !CheckEndActiveCallL() )
        {
        // not shutting down, proceed to phase 1
        iCtx.StartupPhase1L();
        }
    __VTPRINTEXIT( "TLcVtStateStarting.DoOpenL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::CheckSessionStateL
// Resolve action based on session state change in the engine.
// -----------------------------------------------------------------------------
//
void TLcVtStateStarting::CheckSessionStateL()
    {
    __VTPRINTENTER( "TLcVtStateStarting.CheckSessionStateL" )
    const MVtEngSessionInfo::TSessionState state = SessionState( EFalse );
    switch ( state )
        {
    case MVtEngSessionInfo::EConnected:
    case MVtEngSessionInfo::ENegotiating:
    case MVtEngSessionInfo::EOpen:
        // From session state point of view we're ready to proceed.
        // Clear precondition and proceed if otherwise ready.
        CheckPhase2StartupL( EWaitingSessionState );
        break;
    default:
        // Is session state such that we need to shutdown?
        CheckEndActiveCallL();
        break;
        }
    __VTPRINTEXIT( "TLcVtStateStarting.CheckSessionStateL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::CheckPhase2StartupL
//
// mark precondition passed and if no more preconditions exist proceed
// in startup.
// -----------------------------------------------------------------------------
//
void TLcVtStateStarting::CheckPhase2StartupL(
    const TPreconditions aCond )
    {
    __VTPRINTENTER( "TLcVtStateStarting.CheckPhase2StartupL" )
    // First check that at least one precondition holds
    if ( iPreconditions.Value() )
        {
        iPreconditions.Clear( aCond );
        if ( !iPreconditions.Value() )
            {
            CleanupResetPushL();  // start shutdown if StartupPhase2L leaves
            iCtx.StartupPhase2L();
            CleanupStack::Pop(); // CleanupResetPushL
            }
        }
#ifdef VTDEBUG
    else {
        __VTPRINT( DEBUG_GEN, "TLcVtStateStarting.CheckPhase2StartupL NoOp!" )
        }
#endif // VTDEBUG
    __VTPRINTEXITR( "TLcVtStateStarting.CheckPhase2StartupL cond=%d",
        (TInt) iPreconditions.Value() )
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::HandleCLIReceivedL
// CLI reception from engine is one of the two preconditions for proceeding
// to phase2 startup.
// -----------------------------------------------------------------------------
//
void TLcVtStateStarting::HandleCLIReceivedL()
    {
    __VTPRINTENTER( "TLcVtStateStarting.HandleCLIReceivedL" )
    const MVtEngSessionInfo& session = iCtx.Model().Session();
    MVtEngSessionInfo::TCLI cli;
    if ( session.GetCLI( cli ) == KErrNone )
        {
        iUiStates.SetDetailsReceived( ETrue );
        iUiStates.SetCLIReceived( cli.iVoiceCallPossible );
        iCtx.SetCallIdL( cli.iCallId );
        iCtx.SetCallNameL( cli.iName );
        iCtx.SetCallNumberL( cli.iCallNumber );
        }
    CheckPhase2StartupL( EWaitingCLI );
    __VTPRINTEXIT( "TLcVtStateStarting.HandleCLIReceivedL" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateStarting::Close
// -----------------------------------------------------------------------------
//
void TLcVtStateStarting::Close()
    {
    __VTPRINTENTER( "TLcVtStateStarting.Close" )
    delete TLcVtStateStarting::iInstance;
    TLcVtStateStarting::iInstance = NULL;
    __VTPRINTEXIT( "TLcVtStateStarting.Close" )
    }
