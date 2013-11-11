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
* Description:  Negotiating state
*
*/


// INCLUDE FILES
#include    "CVtEngStateNegotiating.h"
#include    <cvtlogger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateNegotiating::CVtEngStateNegotiating
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateNegotiating::CVtEngStateNegotiating( 
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : CVtEngStateBase( aStateManager, aEventManager )
    {
    }

// Destructor
CVtEngStateNegotiating::~CVtEngStateNegotiating()
    {
    __VTPRINT( DEBUG_DESTRUCT, "~Stateneg")
    }


// -----------------------------------------------------------------------------
// CVtEngStateNegotiating::UpdateL
// Updates state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateNegotiating::UpdateL()
    {
    __VTPRINTENTER( "StateNegotiate.Update" )
    const MVtCtlCallControl::TVtCtlState bearerstate = 
        SessionHandler().RealState();
    const MVtProtocolCommand::TVtProtocolState state( MediaHandler().ProtoState() );
    CVtEngStateBase* newState = NULL;

    if ( bearerstate != MVtCtlCallControl::EConnected &&
         state == MVtProtocolCommand::ESetup ) // <- connect failed
        {
        newState = NewStateL( MVtEngSessionInfo::EClearing );
        }
    else if ( state == MVtProtocolCommand::EConnected )
        {
        // change to open state
        newState = NewStateL( MVtEngSessionInfo::EOpen );
        }
    else if ( state == MVtProtocolCommand::ESetup )
        {
        // should not usually happen unless connect fails
        newState = NewStateL( MVtEngSessionInfo::EConnected );
        }
    if ( newState )
        {
        newState->OpenL( this );
        }
    __VTPRINTEXIT( "StateNegotiate.Update" )
    }

// -----------------------------------------------------------------------------
// CVtEngStateNegotiating::OpenL
// State transition to negotiating.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateNegotiating::DoOpenL( )
    {
    __VTPRINTENTER( "StateNegotiate.Open" )
    NotifyStateChange( MVtEngSessionInfo::ENegotiating);
    __VTPRINTEXIT( "StateNegotiate.Open" )
    }

// -----------------------------------------------------------------------------
// CVtEngStateNegotiating::ValidateCommand
// Returns if a command is valid in negotiating state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateNegotiating::ValidateCommand(
    const TVtEngCommandId aCommandId )
    {
    __VTPRINT( DEBUG_DETAIL | DEBUG_GEN, "StateNegotiate.Validate")
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
        case KVtEngPrepareViewFinder:        
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRender:
        case KVtEngPrepareRemoteRenderNGA:
        case KVtEngStartRenderRemote: // cached to settings
        case KVtEngSetSource:
        case KVtEngPrepareCamera:
        case KVtEngSetZoomStep:
        case KVtEngStartViewFinder:
        case KVtEngStopViewFinder:
        case KVtEngTerminateSession:
            okToPerform = ETrue;
            break;
        default:
            okToPerform = CVtEngStateBase::ValidateCommand( aCommandId );
            break;
        }
    return okToPerform;
    }

// -----------------------------------------------------------------------------
// CVtEngStateNegotiating::State
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateNegotiating::State() const
    {
    return MVtEngSessionInfo::ENegotiating;
    }

// -----------------------------------------------------------------------------
// CVtEngStateNegotiating::HandleL
// Handles an operation when behavior is state dependent.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateNegotiating::HandleL( CVtEngOperation& aOp )
    {
    __VTPRINTENTER( "StateNegotiate.HandleL" )
    TBool handled( CVtEngStateBase::HandleL( aOp ) );
    if ( !handled )
        {
        switch ( aOp.Command() )
            {
            case KVtEngStartRenderRemote:
                SetVideoEnabled( ETrue );
                handled = ETrue;
                break;
            case KVtEngTerminateSession:
                TerminateSessionL( aOp );
                break;
            }
        }
    __VTPRINTEXITR( "StateNegotiate.HandleL %d", handled )
    return handled;
    }

// End of File
