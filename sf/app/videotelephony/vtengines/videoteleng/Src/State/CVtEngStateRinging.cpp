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
* Description:  Ringing state handle interfaces implementation
*
*/


// INCLUDE FILES
#include    "CVtEngStateRinging.h"
#include    "CVtEngInitializer.h"
#include    "VtEngUtils.h"
#include    <cvtlogger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateRinging::CVtEngStateRinging
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateRinging::CVtEngStateRinging( 
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : CVtEngStateBase( aStateManager, aEventManager )
    {
    }

// Destructor
CVtEngStateRinging::~CVtEngStateRinging()
    {
    __VTPRINT( DEBUG_DESTRUCT, "~StateRing")
    }


// -----------------------------------------------------------------------------
// CVtEngStateRinging::UpdateL
// Updates state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateRinging::UpdateL()
    {
    __VTPRINTENTER( "StateRing.Update" )
    const MVtCtlCallControl::TVtCtlState bearerstate = 
        SessionHandler().RealState();
    CVtEngStateBase* newState = NULL;
    switch ( bearerstate )
        {
        case MVtCtlCallControl::EConnected:
            newState = NegotiatingOrConnectedStateL();
            break;
        case MVtCtlCallControl::EDisconnecting:
        case MVtCtlCallControl::EIdle:
            newState = NewStateL( MVtEngSessionInfo::EIdle );
            // Initializer may be waiting for dataport. In that case MediaHandler 
            // has already signaled reqStatus of CVtEngInitializer which must
            // complete KVtEngInitializeEngine command in order uninitialisation to
            // finish properly. This scenario may happen in 1st call since phone
            // boot, i.e. when dataport name is not already published.
            CVtEngUtility::EngineUtils()->Initializer().CancelInit();
            break;
        default:
            break;
        }
    if ( newState )
        {
        newState->OpenL( this );
        }
    __VTPRINTEXIT( "StateRing.Update")
    }

// -----------------------------------------------------------------------------
// CVtEngStateRinging::OpenL
// State transition to Ring.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateRinging::DoOpenL()
    {
    __VTPRINTENTER( "StateRing.Open")
    NotifyStateChange( MVtEngSessionInfo::EReceiving );
    __VTPRINTEXIT( "StateRing.Open")
    }

// -----------------------------------------------------------------------------
// CVtEngStateRinging::ValidateCommand
// Returns if a command is valid in ringing state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateRinging::ValidateCommand(
    const TVtEngCommandId aCommandId )
    {
    __VTPRINT( DEBUG_DETAIL | DEBUG_GEN, "Statering.Validate")
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
        case KVtEngSetSource:
        case KVtEngPrepareViewFinder:                
        case KVtEngStartViewFinder:
        case KVtEngPauseViewFinder:
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRender:
        case KVtEngPrepareRemoteRenderNGA:
        case KVtEngStartRenderRemote: 
            okToPerform = ETrue;
            break;
        default:
            okToPerform = CVtEngStateBase::ValidateCommand( aCommandId );
            break;
        }
    return okToPerform;
    }

// -----------------------------------------------------------------------------
// CVtEngStateRinging::State
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateRinging::State() const
    {
    return MVtEngSessionInfo::EReceiving;
    }

// End of File
