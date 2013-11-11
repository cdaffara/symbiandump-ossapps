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
* Description:  Event manager implementation.
*
*/



// INCLUDE FILES
#include    "CVtEngEventManager.h"
#include    "MVtEngEventObserver.h"
#include    "VtEngUtils.h"
#include    "VtEngEvents.h"
#include    "VtEngPanic.h"
#include    <cvtlogger.h>

// CONSTANTS
const TInt KMaxEventsInQueue = 8;

// max amount of simultanous event observers
const TInt KMaxVtEventObservers = 2;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngEventManager::CVtEngEventManager
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngEventManager::CVtEngEventManager() : CActive( CActive::EPriorityHigh ),
    iObservers( KMaxVtEventObservers ) 
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CVtEngEventManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CVtEngEventManager* CVtEngEventManager::NewL(
    MVtEngEventObserver& aEventObserver )
    {
    CVtEngEventManager* self = 
        new ( ELeave ) CVtEngEventManager();        
    CleanupStack::PushL( self );
    self->ConstructL( aEventObserver );
    CleanupStack::Pop(); // self 
    return self;
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::ConstructL
// 2nd phase constructor.
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::ConstructL( MVtEngEventObserver& aEventObserver )
	{
	iEventQueue = new ( ELeave ) CArrayFixFlat< TInt >( KMaxEventsInQueue );
    iEventQueue->SetReserveL( KMaxEventsInQueue );
    User::LeaveIfError( iObservers.Append( &aEventObserver ) );
	}
	
// Destructor
CVtEngEventManager::~CVtEngEventManager()
    {
    Cancel();
    delete iEventQueue;
    iObservers.Close();
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::SessionStateChangedL
// Notifies session state change to UI.
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::SessionStateChanged()
    {
    __VTPRINTENTER( "EventManager.SessionStateChanged" )
    NotifyEvent( KVtEngSessionStateChanged );
    __VTPRINTEXIT( "EventManager.SessionStateChanged" )
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::NotifyEvent
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::NotifyEvent( const TInt aEvent )
    {
    __VTPRINTENTER( "EventManager.NotifyEvent" )
    __VTPRINT2( DEBUG_GEN, "EventManager.NotifyEvent event=%d", aEvent );
    CVtEngEventManager& eventManager = 
            CVtEngUtility::EventManager();
    if ( !eventManager.QueueAsyncEvent( aEvent ) )
        {        
        eventManager.DoNotifyEvent( aEvent );
        }
    __VTPRINTEXIT( "EventManager.NotifyEvent" )
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::AddObserverL
//
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::AddObserverL( MVtEngEventObserver* aObserver )
    {
    TInt count( iObservers.Count() );
    TBool duplicate = EFalse;
    while ( count-- )
        {
        if ( aObserver == iObservers[ count ] )
            {
            count = 0; // break loop
            duplicate = ETrue;
            }
        }
    if ( !duplicate )
        {
        User::LeaveIfError( iObservers.Append( aObserver ) );
        }
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::RemoveObserver
//
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::RemoveObserver( const MVtEngEventObserver* aObserver )
    {
    TInt count( iObservers.Count() );
    while ( count-- )
        {
        if ( aObserver == iObservers[ count ] )
            {
            iObservers.Remove( count );
            count = 0; // break loop
            }
        }
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::RunL
// Notify event
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::RunL()
    {
    __VTPRINTENTER( "EventManager.RunL" )
    __VTPRINT2( DEBUG_GEN, "EventManager.RunL count=%d", iEventQueue->Count() );
    // get oldest event in queue (event at index 0)
    const TInt event = iEventQueue->At( 0 ) ;
    
    // delete event before calling DoNotifyEvent(), because DoNotifyEvent() may
    // hang if it causes ASynchronous call in handler
    iEventQueue->Delete( 0 );

    // if more events pending -> signal again
    if( iEventQueue->Count() > 0  )
        {
        Signal();    
        }

    // and finally notify event
    DoNotifyEvent( event );      
    __VTPRINTEXIT( "EventManager.RunL" )
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::DoCancel
// No op
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::DoNotifyEvent
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::DoNotifyEvent( const TInt aEvent )
    {
    __VTPRINTENTER( "EventManager.DoNotifyEvent" )
    __VTPRINT2( DEBUG_GEN, "EventManager.DoNotifyEvent event=%d", aEvent );
    TRAP_IGNORE( {
        TInt count( iObservers.Count() );
        while ( count-- )
            {
            iObservers[count]->HandleVtEventL( aEvent ) ;
            }
        } )
    __VTPRINTEXIT( "EventManager.DoNotifyEvent" )
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::QueueAsyncEvent
// -----------------------------------------------------------------------------
//
TBool CVtEngEventManager::QueueAsyncEvent( const TInt aEvent )
    {
    __VTPRINTENTER( "EventManager.QueueAsyncEvent" )
    __VTPRINT2( DEBUG_GEN, "EventManager.QueueAsyncEvent event=%d", aEvent );
    TBool async( EFalse );
    switch ( aEvent )
        {
        case KVtEngSessionStateChanged:
        case KVtEngNegotiationProblem:
        case KVtEngShareImageInitializeBegin:
        case KVtEngShareImageInitializeEnd:
            async = ETrue;
            QueueAndSignal( aEvent );
            break;
        default:
            break;
        }
    __VTPRINTEXIT( "EventManager.QueueAsyncEvent" )
    return async;
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::QueueAndSignal
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::QueueAndSignal( const TInt aEvent )
    {
    __VTPRINTENTER( "EventManager.QueueAndSignal" )
    __VTPRINT2( DEBUG_GEN, "EventManager.QueueAndSignal event=%d", aEvent );
    if( iEventQueue->Count() < KMaxEventsInQueue )
        {
        TRAP_IGNORE( iEventQueue->AppendL( aEvent ) ); // can't leave
        Signal();
        }
    else
        {
        Panic( EVtEngPanicInvalidTooManyPendingEvents );        
        }
    __VTPRINTEXIT( "EventManager.QueueAndSignal" )
    }

// -----------------------------------------------------------------------------
// CVtEngEventManager::Signal
// -----------------------------------------------------------------------------
//
void CVtEngEventManager::Signal()
    {
    __VTPRINTENTER( "EventManager.Signal" )
    if( !IsActive() )
        {
        SetActive();
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, KErrNone );
        __VTPRINT2( DEBUG_GEN, "EventManager.QueueAndSignal count=%d", iEventQueue->Count() );
        }                
    __VTPRINTEXIT( "EventManager.Signal" )
    }

//  End of File  
