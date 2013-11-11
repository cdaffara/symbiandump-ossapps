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
* Description:  base class for lc vt plugin states
*
*/


#include "tlcvtstatebase.h"
#include "tlcvtstates.h"
#include "tlcvtpluginstates.h"
#include "clcvtappshutter.h"
#include "mlcvtstatecontext.h"
#include <StringLoader.h>
#include <cvtlogger.h>
#include <cvtengmodel.h>
#include <vtengevents.h>
#include <mvtengcommandhandler.h>

// By default call ending command is allowed to send to call handling.

// Initially no shutdown request
TBool TLcVtStateBase::iShutdownRequested = EFalse;

// -----------------------------------------------------------------------------
// c++ constructor
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TLcVtStateBase(
    MLcVtStateContext& aCtx,
    TLcVtStates& aUiStates ) : iCtx(aCtx), iUiStates( aUiStates )
    {
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
TLcVtStateBase::~TLcVtStateBase()
    {
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::LcVtStartupPhase1DoneL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::LcVtStartupPhase1DoneL()
    {
    // no default behavior
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::LcVtStartupPhase2DoneL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::LcVtStartupPhase2DoneL()
    {
    // no default behavior
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::ShutdownDoneL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::ShutdownDoneL()
    {
    // no default behavior
    }
#if 0
// -----------------------------------------------------------------------------
// TLcVtStateBase::HandleForegroundChangedL
// -----------------------------------------------------------------------------
//
TBool TLcVtStateBase::HandleForegroundChangedL(
    const TBool /*aIsForegroung*/ )
    {
    // no default behavior
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::PreHandleForegroundChangedL
// -----------------------------------------------------------------------------
//
TBool TLcVtStateBase::PreHandleForegroundChangedL(
    const TBool /*aIsForegroung*/ )
    {
    // no default behavior
    return EFalse;
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::StartDtmfTone
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::StartDtmfTone( const TChar& /*aTone*/ )
    {
    // no default behavior
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::StopDtmfTone
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::StopDtmfTone()
    {
    // no default behavior
    }
#endif

// -----------------------------------------------------------------------------
// TLcVtStateBase::SetInitialStateL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::SetInitialStateL(
    MLcVtStateContext& aCtx,
    TLcVtStates& aUiStates )
    {
    __VTPRINTENTER( "TLcVtStateBase.SetInitialState" )
    TLcVtStateBase* initialState =
        TLcVtStateStarting::InstanceL( aCtx, aUiStates );
    aCtx.ChangeState( initialState );
    initialState->OpenL();
    __VTPRINTEXIT( "TLcVtStateBase.SetInitialState" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::LcVtStartupFailedL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::LcVtStartupFailedL()
    {
    __VTPRINTENTER( "TLcVtStateBase.AppUiStartupFailedL" )
    // Shutdown
    ChangeStateL( *TLcVtStateResetting::InstanceL( iCtx, iUiStates ) );
    __VTPRINTEXIT( "TLcVtStateBase.AppUiStartupFailedL" )
    }
#if 0
// -----------------------------------------------------------------------------
// TLcVtStateBase::OpenL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::OpenL()
    {
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::Close
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::Close()
    {
    }
#endif

// -----------------------------------------------------------------------------
// TLcVtStateBase::ChangeStateL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::ChangeStateL( TLcVtStateBase& aNewState )
    {
    __VTPRINTENTER( "TLcVtStateBase.ChangeStateL" )

    // Check for transition to already active state.
    TLcVtStateBase* state = &aNewState;
    if ( state == this )
        {
        __VTPRINTEXITR( "TLcVtStateBase.ChangeStateL %d", 0 )
        return;
        }
    const TVtEngCommandId pending =
        iCtx.Model().CommandHandler().PendingCommand();
    if ( pending !=  KVtEngCommandNone &&
         state == TLcVtStateResetting::InstanceL( iCtx, iUiStates ) )
        {
        // delay shutdown until engine command completes
        __VTPRINT2( DEBUG_GEN, "TLcVtStateBase.ChangeStateL, pengind command is: %d", pending );
        TLcVtStateBase::iShutdownRequested = ETrue;

        if ( pending == KVtEngInitializeShareImage )
            {
            iCtx.Model().CommandHandler().CancelCommand( pending );
            }

        __VTPRINTEXITR( "TLcVtStateBase.ChangeStateL %d", KErrNotReady )
        return;
        }
    __VTPRINT( DEBUG_GEN,
        " TLcVtStateBase.ChangeStateL !! STATE TRANSITION !!" )
    iCtx.ChangeState( &aNewState );
    Close();
    // perform entry action for new state
    aNewState.OpenL();
    __VTPRINTEXITR( "TLcVtStateBase.ChangeStateL %d", 1 )
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::CleanupResetPushL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::CleanupResetPushL()
    {
    // Shutdown if leave occurs in critical ops.
    CleanupStack::PushL(
        TCleanupItem( TransitionToReset, this ) );
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::TransitionToReset
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::TransitionToReset( TAny* aAny )
    {
    TLcVtStateBase* state = reinterpret_cast< TLcVtStateBase* >( aAny );
    TRAP_IGNORE( state->ChangeStateL( *TLcVtStateResetting::InstanceL(
        state->iCtx, state->iUiStates ) ) );
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::HandleVTCommandPerformedL
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateBase::HandleVTCommandPerformedL(
            TVtEngCommandId /*aCommand*/,
            const TInt /*aError*/ )
    {
    __VTPRINTENTER( "TLcVtStateBase.HandleVTCommandPerformedL" )
    // By default all UI is allowed to handle all command completions.
    TEventResponse handled = EEventNotHandled;

    // check if shutdown requested while command was pending
    const TVtEngCommandId pending = iCtx.Model().CommandHandler().PendingCommand();
    if ( pending == KVtEngCommandNone &&
            TLcVtStateBase::iShutdownRequested )
        {
        __VTPRINT( DEBUG_GEN, "Handling delayed shutdown" );
        ChangeStateL( *TLcVtStateResetting::InstanceL(
            iCtx, iUiStates ) );
        }
    __VTPRINTEXIT( "TLcVtStateBase.HandleVTCommandPerformedL" )
    return handled;
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::HandleVtEventL
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateBase::HandleVtEventL(
    const TInt aEvent )
    {
    __VTPRINTENTER( "TLcVtStateBase.HandleVtEventL" )
    TEventResponse handled = EEventHandled;
    switch ( aEvent )
        {
    case KVtEngDeviceLockOn:
    case KVtEngDeviceLockOff:
        HandleDeviceLockEventL( aEvent == KVtEngDeviceLockOn );
        break;
    case KVtEngRemoteDisconnect:
        /** Don't allow sending of end active call command to call handling
         *  because requesting disconnect from the network after NW originated
         *  termination violates standards.
         */
        break;
    default:
        handled = EEventNotHandled;
        break;
        }
    __VTPRINTEXIT( "TLcVtStateBase.HandleVtEventL" )
    return handled;
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::HandleCommandL
// -----------------------------------------------------------------------------
//
TLcVtStateBase::TEventResponse TLcVtStateBase::HandleCommandL(
    const TInt /*aCommand*/ )
    {
    __VTPRINTENTER( "TLcVtStateBase.HandleCommandL" )
    TEventResponse handled = EEventNotHandled;
    __VTPRINTEXITR( "TLcVtStateBase.HandleCommandL %d", handled )
    return handled;
    }
#if 0
// -----------------------------------------------------------------------------
// TLcVtStateBase::HandleLayoutChangedL
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::HandleLayoutChangedL( )
    {
    __VTPRINTENTER( "TLcVtStateBase.HandleLayoutChangedL" )
    // By default only indicate that layout change needs to be handled.
    iUiStates.SetLayoutChangeNeeded( ETrue );
    __VTPRINTEXIT( "TLcVtStateBase.HandleLayoutChangedL" )
    }
#endif
// -----------------------------------------------------------------------------
// TLcVtStateBase::HandleShutdownReady
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::HandleShutdownReady()
    {
     __VTPRINTENTER( "TLcVtStateBase.HandleShutdownReady" )
    // forwarded to AppUi which handles UI framework exit
    iCtx.HandleShutdownReady();
    iCtx.ChangeState( NULL );
    // deletes current state
    Close();
     __VTPRINTEXIT( "TLcVtStateBase.HandleShutdownReady" )
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::HandleDeviceLockEventL
//
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::HandleDeviceLockEventL( const TBool aDeviceIsLocked )
    {
    __VTPRINTENTER( "TLcVtStateBase.HandleDeviceLockEventL" )
    iUiStates.SetDeviceLockOn( aDeviceIsLocked );
    if ( aDeviceIsLocked )
        {
        iCtx.ChangeApplicationFocus( EFalse );
        }
    __VTPRINTEXITR( "TLcVtStateBase.HandleDeviceLockEventL lock state=%d",
        aDeviceIsLocked )
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::CheckEndActiveCallL
// -----------------------------------------------------------------------------
//
TBool TLcVtStateBase::CheckEndActiveCallL()
    {
    __VTPRINTENTER( "TLcVtStateBase.CheckEndActiveCallL" )
    const MVtEngSessionInfo::TSessionState state = SessionState( EFalse );
    TBool shutdownStarted( EFalse );
    if ( state == MVtEngSessionInfo::EIdle )
        {
        __VTPRINT( DEBUG_GEN, "VtUi.Release bearer" )
        // h.245 session is ended
        ChangeStateL( *TLcVtStateResetting::InstanceL( iCtx, iUiStates ) );        
        shutdownStarted = ETrue;
        }
    __VTPRINTEXITR( "TLcVtStateBase.CheckEndActiveCallL %d",
        shutdownStarted )
    return shutdownStarted;
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::SessionState
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState TLcVtStateBase::SessionState(
    const TBool aUpdate ) const
    {
    const CVtEngModel& model = iCtx.Model();
    const MVtEngSessionInfo& session = model.Session();
    return session.State( aUpdate );
    }

// -----------------------------------------------------------------------------
// TLcVtStateBase::SetExecState
// -----------------------------------------------------------------------------
//
void TLcVtStateBase::SetExecState( const TLcVtStates::TLcVtExecState aState )
    {
    __VTPRINTENTER( "TLcVtStateBase.SetExecState" )
    iUiStates.SetExecState( aState );
    __VTPRINTEXITR( "TLcVtStateBase.SetExecState %d", aState )
    }
