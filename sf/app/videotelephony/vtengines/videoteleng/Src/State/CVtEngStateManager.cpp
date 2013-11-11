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
* Description:  Implementation of state manager for session and audio routing states.
*
*/



// INCLUDE FILES
#include    "CVtEngStateManager.h"
#include    "CVtEngHandlerContainer.h"
#include    "CVtEngStateBase.h"
#include    "CVtEngStateIdle.h"
#include    "CVtEngStateOpen.h"
#include    "CVtEngEventManager.h"
#include    "CVtEngStateConnected.h"
#include    "CVtEngOperation.h"
#include    "cvtlogger.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateManager::CVtEngStateManager
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateManager::CVtEngStateManager( 
    CVtEngHandlerContainer& aHandlers,
    CVtEngEventManager& aEventManager ) : 
    iPreviousSessionState( MVtEngSessionInfo::EUnknown ),
    iHandlers( aHandlers ), 
    iEventManager( aEventManager )
    {
    }

// -----------------------------------------------------------------------------
// CVtEngStateManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CVtEngStateManager::ConstructL()
    {
    iSessionState = new ( ELeave ) 
        CVtEngStateIdle( *this, iEventManager );
    }

// -----------------------------------------------------------------------------
// CVtEngStateManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngStateManager* CVtEngStateManager::NewL(
    CVtEngHandlerContainer& aHandlers,
    CVtEngEventManager& aEventManager )
    {
    __VTPRINT( DEBUG_GEN , "CVtEngStateManager::NewL" )
    CVtEngStateManager* self = new ( ELeave ) 
        CVtEngStateManager( aHandlers, aEventManager );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

    
// Destructor
CVtEngStateManager::~CVtEngStateManager()
    {
    __VTPRINT( DEBUG_GEN , "CVtEngStateManager::~" )
    delete iSessionState;
    }

// -----------------------------------------------------------------------------
// CVtEngStateManager::UpdateL
// Updates current state.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateManager::Update( )
    {
    if ( iSessionState )
        {
        TRAPD(err,iSessionState->UpdateL());
        if ( err != KErrNone )
            {
            __VTPRINT2( DEBUG_GEN, "SM.update fail=%d", err )
            iEventManager.NotifyEvent( err );
            }
        }
    }

// -----------------------------------------------------------------------------
// CVtEngStateManager::IsCommandPossible
// Checks if a command is possible in current state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateManager::IsCommandPossible( 
            const TVtEngCommandId aCommandId ) const
    {
    if ( iSessionState )
        {
        return iSessionState->ValidateCommand( aCommandId );
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CVtEngStateManager::SetState
// Sets new session state.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateManager::SetState( CVtEngStateBase& aNewState )
    {
    if( iSessionState )
        {
        iPreviousSessionState = iSessionState->State();
        }
    iSessionState = &aNewState;
    }

// -----------------------------------------------------------------------------
// CVtEngStateManager::HandleOperationL
// 
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateManager::HandleOperationL( CVtEngOperation& aOp )
    {
    __VTPRINT2( DEBUG_GEN, "StateMgr.HandleOp=%d", aOp.Command() )
    TInt handled( EFalse );
    if ( iSessionState )
        {
        handled = iSessionState->HandleL( aOp );
        }
    __VTPRINT2( DEBUG_GEN | DEBUG_RETURN, "StateMgr.HandleOp handled=%d", 
        handled )
    return handled;
    }

// -----------------------------------------------------------------------------
// CVtEngStateManager::SessionState
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateManager::SessionState() const
    {
    if ( iSessionState )
        {
        return iSessionState->State();
        }
    return MVtEngSessionInfo::EUnknown;
    }

// -----------------------------------------------------------------------------
// CVtEngStateManager::PreviousSessionState
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateManager::PreviousSessionState() const
    {
    return iPreviousSessionState;
    }

//  End of File  
