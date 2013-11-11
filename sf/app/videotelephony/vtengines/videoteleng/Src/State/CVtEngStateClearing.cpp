/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
#include    "CVtEngStateClearing.h"
#include    <cvtlogger.h>
#include    "CVtEngOperation.h"
#include    "CVtEngSettings.h"
#include    "VtEngUtils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateClearing::CVtEngStateClearing
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateClearing::CVtEngStateClearing( 
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : CVtEngStateBase( aStateManager, aEventManager )
    {
    }

// Destructor
CVtEngStateClearing::~CVtEngStateClearing()
    {
    __VTPRINT( DEBUG_DESTRUCT, "~StateClr")
    }


// -----------------------------------------------------------------------------
// CVtEngStateClearing::UpdateL
// Updates state.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateClearing::UpdateL()
    {
    __VTPRINTENTER( "StateClr.Update" )
    const MVtCtlCallControl::TVtCtlState bearerstate = 
        SessionHandler().RealState();
    const MVtProtocolCommand::TVtProtocolState protostate( MediaHandler().ProtoState() );

    __VTPRINT2( DEBUG_GEN, "StateClr.bearerState=%d", bearerstate )
    __VTPRINT2( DEBUG_GEN, "StateClr.protoState=%d", protostate )

    if ( bearerstate == MVtCtlCallControl::EIdle ||
         bearerstate == MVtCtlCallControl::EDisconnecting ||
         bearerstate == MVtCtlCallControl::EUnknown ||
         protostate == MVtProtocolCommand::EIdle || 
         protostate == MVtProtocolCommand::ESetup )
        {
        CVtEngStateBase* idleState = NewStateL( MVtEngSessionInfo::EIdle );
        idleState->OpenL( this );
        }
    __VTPRINTEXIT( "StateClr.Update" )
    }

// -----------------------------------------------------------------------------
// CVtEngStateClearing::OpenL
// State transition to clearing state.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateClearing::DoOpenL()
    {
    __VTPRINTENTER( "StateClr.Open")
    
    // Do disconnect on Protocol, it is possible that disconnect is already
    // ongoing and we get error and that's fine.
    TInt err( MediaHandler().Disconnect() );
    if ( err != KErrNone && err != KErrAlreadyExists )
        {
        __VTPRINT2( DEBUG_GEN, "StateClr.OpenL disconnect fail=%d", err)
        }
    // Reset settings
    CVtEngSettings& settings = CVtEngUtility::Settings();
    settings.ResetAll();
    NotifyStateChange( MVtEngSessionInfo::EClearing );
    __VTPRINTEXIT( "StateClr.Open")
    }

// -----------------------------------------------------------------------------
// CVtEngStateClearing::ValidateCommand
// Returns if a command is valid in idle state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateClearing::ValidateCommand(
            const TVtEngCommandId aCommandId )
    {
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
        case KVtEngTerminateSession:
            break;
        default:
            okToPerform = CVtEngStateBase::ValidateCommand( aCommandId );
            break;
        }
    return okToPerform;
    }

// -----------------------------------------------------------------------------
// CVtEngStateClearing::State
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateClearing::State() const
    {
    return MVtEngSessionInfo::EClearing;
    }

// -----------------------------------------------------------------------------
// CVtEngStateClearing::HandleL
// Handles an operation when behavior is state dependent.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateClearing::HandleL( CVtEngOperation& aOp )
    {
    TBool handled( CVtEngStateBase::HandleL( aOp ));
    return handled;
    }

// End of File
