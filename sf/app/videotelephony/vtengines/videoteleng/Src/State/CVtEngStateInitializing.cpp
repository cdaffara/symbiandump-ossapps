/*
* Copyright (c) 2006 - 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Initializing state
*
*/


// INCLUDE FILES
#include    "CVtEngStateInitializing.h"
#include    "CVtEngOperation.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"
#include    "CVtEngAudioHandler.h"
#include    "CVtEngInitializer.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateInitializing::CVtEngStateInitializing
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateInitializing::CVtEngStateInitializing( 
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : CVtEngStateBase( aStateManager, aEventManager )
    {
    }

// Destructor
CVtEngStateInitializing::~CVtEngStateInitializing()
    {
    __VTPRINT( DEBUG_DESTRUCT, "~Stateini")
    }


// -----------------------------------------------------------------------------
// CVtEngStateInitializing::UpdateL
// Updates state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateInitializing::UpdateL()
    {
    __VTPRINTENTER( "StateInit.Update")
    const MVtCtlCallControl::TVtCtlState bearerstate = 
        SessionHandler().RealState();

    CVtEngStateBase* newState = NULL;
    if ( bearerstate == MVtCtlCallControl::EIdle ||
         bearerstate == MVtCtlCallControl::EDisconnecting )
        {
        newState = NewStateL( MVtEngSessionInfo::EClearing );
        // Initializer may be waiting for dataport. In that case MediaHandler 
        // has already signaled reqStatus of CVtEngInitializer which must
        // complete KVtEngInitializeEngine command in order uninitialisation to
        // finish properly. This scenario may happen in 1st call since phone
        // boot, i.e. when dataport name is not already published.
        CVtEngUtility::EngineUtils()->Initializer().CancelInit();
        }
    else if ( bearerstate == MVtCtlCallControl::EAlerting )
        {
        newState = NewStateL( MVtEngSessionInfo::ERemoteAlerting );
        }
    else
        {
        newState = NegotiatingOrConnectedStateL(); // or null
        }
    if ( newState )
        {
        newState->OpenL( this );
        }
    __VTPRINTEXIT( "StateInit.Update")
    }

// -----------------------------------------------------------------------------
// CVtEngStateInitializing::OpenL
// State transition to initializing.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateInitializing::DoOpenL()
    {
    __VTPRINTENTER( "StateInit.Open" )
    NotifyStateChange( MVtEngSessionInfo::EInitializing );
    
    // Instantiate temporary devsound for audio routing
    AudioHandler().SetRoutingEnablerL( ETrue );
    __VTPRINTEXIT( "StateInit.Open" )
    }

// -----------------------------------------------------------------------------
// CVtEngStateInitializing::ValidateCommand
// Returns if a command is valid in initializing state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateInitializing::ValidateCommand(
            const TVtEngCommandId aCommandId )
    {
    __VTPRINT( DEBUG_DETAIL | DEBUG_GEN, "StateInit.Validate")
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
        case KVtEngPrepareViewFinder:        
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRenderNGA:
        case KVtEngSetSource:
        case KVtEngStartViewFinder:
            okToPerform = ETrue;
            break;
        default:
            okToPerform = CVtEngStateBase::ValidateCommand( aCommandId );
            break;
        }
    return okToPerform;
    }

// -----------------------------------------------------------------------------
// CVtEngStateInitializing::State
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateInitializing::State() const
    {
    return MVtEngSessionInfo::EInitializing;
    }


// -----------------------------------------------------------------------------
// CVtEngStateInitializing::HandleL
// Handles an operation when behavior is state dependent.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateInitializing::HandleL( CVtEngOperation& aOp )
    {
    TBool handled( CVtEngStateBase::HandleL( aOp ));
    if ( !handled )
        {
        switch ( aOp.Command() )
            {
            case KVtEngStartRenderRemote:
                SetVideoEnabled( ETrue );
                handled = ETrue;
                break;
            }
        }
    return handled;
    }

// End of File
