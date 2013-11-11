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
* Description:  Idle state
*
*/


// INCLUDE FILES
#include    "CVtEngStateIdle.h"
#include    <cvtlogger.h>


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateIdle::CVtEngStateIdle
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateIdle::CVtEngStateIdle( 
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : CVtEngStateBase( aStateManager, aEventManager )
    {
    }

// Destructor
CVtEngStateIdle::~CVtEngStateIdle()
    {
    __VTPRINT( DEBUG_DESTRUCT, "~Stateidl")
    }


// -----------------------------------------------------------------------------
// CVtEngStateIdle::UpdateL
// Updates state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateIdle::UpdateL()
    {
    __VTPRINTENTER( "StateIdle.Update")
   
    const MVtCtlCallControl::TVtCtlState state = SessionHandler().RealState();
    CVtEngStateBase* newState = NULL;
    
    if( iStateManager.PreviousSessionState() == MVtEngSessionInfo::EUnknown )
        {
        switch ( state )
            {
            case MVtCtlCallControl::EInitializing:
                newState = NewStateL( MVtEngSessionInfo::EInitializing );
                break;
            case MVtCtlCallControl::EAlerting:
                newState = NewStateL( MVtEngSessionInfo::ERemoteAlerting );
                break;
            case MVtCtlCallControl::ERinging:
                newState = NewStateL( MVtEngSessionInfo::EReceiving );
                break;
            case MVtCtlCallControl::EConnected:
                newState = NegotiatingOrConnectedStateL();
                break;
            default:
                break;
            }
        if ( newState )
            {
            CVtEngUtility* utils = CVtEngUtility::EngineUtils();
            
            // Notify UI about initial autolock state. UI must have this
            // information early enough to deduce if it is allowed to 
            // go to foreground.
            utils->NotifyLockState();            
            utils->StartSATListenL();
            newState->OpenL( this );
            }
        }
    else
        {
        __VTPRINT( DEBUG_GEN, "StateIdle.Update wrong previous state, ignoring")        
        }
    __VTPRINTEXIT( "StateIdle.Update")
    }

// -----------------------------------------------------------------------------
// CVtEngStateIdle::OpenL
// State transition to idle.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateIdle::DoOpenL()
    {
    __VTPRINTENTER( "StateIdle.Open")
    // Reset settings
    CVtEngSettings& settings = CVtEngUtility::Settings();
    settings.ResetAll();
    
    NotifyStateChange( MVtEngSessionInfo::EIdle );
    __VTPRINTEXIT( "StateIdle.Open")
    }

// -----------------------------------------------------------------------------
// CVtEngStateIdle::ValidateCommand
// Returns if a command is valid in idle state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateIdle::ValidateCommand(
            const TVtEngCommandId aCommandId )
    {
    __VTPRINT( DEBUG_DETAIL | DEBUG_GEN, "StateIdle.Validate")
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
        case KVtEngPrepareViewFinder:        
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRender:
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
// CVtEngStateIdle::State
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateIdle::State() const
    {
    return MVtEngSessionInfo::EIdle;
    }


// End of File
