/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for states
*
*/


// INCLUDE FILES
#include    "CVtEngStateConnected.h"
#include    <cvtlogger.h>
#include    "CVtEngOperation.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateConnected::CVtEngStateConnected
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateConnected::CVtEngStateConnected( 
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : CVtEngStateBase( aStateManager, aEventManager )
    {
    }

// Destructor
CVtEngStateConnected::~CVtEngStateConnected()
    {
    __VTPRINT( DEBUG_DESTRUCT, "~StateConnected")
    }


// -----------------------------------------------------------------------------
// CVtEngStateConnected::UpdateL
// Updates state.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateConnected::UpdateL()
    {
    __VTPRINTENTER( "StateConnected.Update")
    const MVtCtlCallControl::TVtCtlState bearerstate = 
        SessionHandler().RealState();
    MVtProtocolCommand::TVtProtocolState state( MediaHandler().ProtoState() );
    
        
    CVtEngStateBase* newState = NULL;
    if ( bearerstate != MVtCtlCallControl::EConnected )
        {
        // we've lost bearer connection, go to idle
        newState = NewStateL( MVtEngSessionInfo::EIdle );
        }
    else if ( state == MVtProtocolCommand::EConnected )
        {
        // protocol is open (should always go through neg.state but just in case)
        // change to open state
        newState = NewStateL( MVtEngSessionInfo::EOpen );
        }
    else if ( state == MVtProtocolCommand::EDisconnecting ||
              state == MVtProtocolCommand::EResetting )
        {
        // change to clearing state
        newState = NewStateL( MVtEngSessionInfo::EClearing );
        }
    else
        {
        // protocol is set up so connect now if ready
        newState = NegotiatingOrConnectedStateL( ETrue );
        }
    if ( newState )
        {
        newState->OpenL( this );
        }
    __VTPRINTEXIT( "StateConnected.Update")
    }

// -----------------------------------------------------------------------------
// CVtEngStateConnected::DoOpenL
// State transition to connected state.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateConnected::DoOpenL()
    {
    __VTPRINTENTER( "StateConnected.Open")
    NotifyStateChange( MVtEngSessionInfo::EConnected );
    //start monitor incoming call
    __VTPRINTEXIT( "StateConnected.Open")
    }

// -----------------------------------------------------------------------------
// CVtEngStateConnected::ValidateCommand
// Returns if a command is valid in idle state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateConnected::ValidateCommand(
            const TVtEngCommandId aCommandId )
    {
    __VTPRINT( DEBUG_DETAIL |DEBUG_GEN, "StateConnected.Validate")
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
        case KVtEngSetSource:
        case KVtEngPrepareViewFinder:
        case KVtEngStartViewFinder:
        case KVtEngPauseViewFinder:
        case KVtEngStopViewFinder:
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRenderNGA:
            okToPerform = ETrue;
            break;
        default:
            okToPerform = CVtEngStateBase::ValidateCommand( aCommandId );
            break;
        }
    return okToPerform;
    }

// -----------------------------------------------------------------------------
// CVtEngStateConnected::State
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateConnected::State() const
    {
    return MVtEngSessionInfo::EConnected;
    }

// -----------------------------------------------------------------------------
// CVtEngStateConnected::HandleL
// Handles an operation when behavior is state dependent.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateConnected::HandleL( CVtEngOperation& aOp )
    {
    TBool handled( CVtEngStateBase::HandleL( aOp ));
    return handled;
    }

// End of File
