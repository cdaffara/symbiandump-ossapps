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
#include    "CVtEngStateAlerting.h"
#include    <cvtlogger.h>
#include    "CVtEngOperation.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include    "CVtEngAudioHandler.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateAlerting::CVtEngStateAlerting
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateAlerting::CVtEngStateAlerting( 
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : CVtEngStateBase( aStateManager, aEventManager )
    {
    }

// Destructor
CVtEngStateAlerting::~CVtEngStateAlerting()
    {
    __VTPRINT( DEBUG_DESTRUCT, "~StateAlrt")
    }


// -----------------------------------------------------------------------------
// CVtEngStateAlerting::UpdateL
// Updates state.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateAlerting::UpdateL()
    {
    __VTPRINTENTER(  "StateAlert.Update" )
    const MVtCtlCallControl::TVtCtlState bearerstate = 
        SessionHandler().RealState();
    CVtEngStateBase* newState = NULL;
    if ( bearerstate == MVtCtlCallControl::EIdle ||
         bearerstate == MVtCtlCallControl::EDisconnecting )
        {
        newState = NewStateL( MVtEngSessionInfo::EIdle );
        }
    else
        {
        newState = NegotiatingOrConnectedStateL(); // or null
        }
    if ( newState )
        {
        newState->OpenL( this );
        }
    __VTPRINTEXIT( "StateAlert.Update")
    }

// -----------------------------------------------------------------------------
// CVtEngStateAlerting::OpenL
// State transition to alerting state.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateAlerting::DoOpenL()
    {
    __VTPRINTENTER( "StateAlert.Open" )
    NotifyStateChange( MVtEngSessionInfo::ERemoteAlerting );
    
    // Instantiate temporary devsound for audio routing
    AudioHandler().SetRoutingEnablerL( ETrue );
    __VTPRINTEXIT( "StateAlert.Open" )
    }

// -----------------------------------------------------------------------------
// CVtEngStateAlerting::ValidateCommand
// Returns if a command is valid in idle state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateAlerting::ValidateCommand(
    const TVtEngCommandId aCommandId )
    {
    __VTPRINT( DEBUG_DETAIL |DEBUG_GEN, "StateAlert.Validate" )
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
        case KVtEngInitializeEngine:
        case KVtEngInitializeEngineDiag:
        case KVtEngInitializeEngineTest:
        case KVtEngSetSource:
        case KVtEngPrepareViewFinder:
        case KVtEngPrepareViewFinderDSA:
        case KVtEngPrepareViewFinderDP:
        case KVtEngStartViewFinder:
        case KVtEngPauseViewFinder:
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
// CVtEngStateAlerting::State
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateAlerting::State() const
    {
    return MVtEngSessionInfo::ERemoteAlerting;
    }

// -----------------------------------------------------------------------------
// CVtEngStateAlerting::HandleL
// Handles an operation when behavior is state dependent.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateAlerting::HandleL( 
    CVtEngOperation& aOp )
    {
    TBool handled( CVtEngStateBase::HandleL( aOp ));
    return handled;
    }

// End of File
