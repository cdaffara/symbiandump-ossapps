/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     Messaging progress watching classes
*
*
*/



// INCLUDE FILES

#include "emailtrace.h"
#include "ipsplgheaders.h"

// CONSTANTS
const TInt KSingleOpWatcherPriority = KMaxTInt;
#ifdef _DEBUG
_LIT( KPanicIpsPlgSingleOpWatcher, "CIpsPlgSingleOpWatcher");
#endif

// ============================ MEMBER FUNCTIONS ===============================

// ---------------------------------------------------------
// CIpsPlgSingleOpWatcher::NewLC
//
// ---------------------------------------------------------
//
EXPORT_C CIpsPlgSingleOpWatcher* CIpsPlgSingleOpWatcher::NewLC( 
    MIpsPlgSingleOpWatcher& aObserver )
    {
    FUNC_LOG;
    CIpsPlgSingleOpWatcher* self = new( ELeave ) CIpsPlgSingleOpWatcher( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------
// CIpsPlgSingleOpWatcher::NewL
//
// ---------------------------------------------------------
//
EXPORT_C CIpsPlgSingleOpWatcher* CIpsPlgSingleOpWatcher::NewL( 
    MIpsPlgSingleOpWatcher& aObserver )
    {
    FUNC_LOG;
    CIpsPlgSingleOpWatcher* self = CIpsPlgSingleOpWatcher::NewLC( aObserver );
    CleanupStack::Pop( self ); // self
    return self;
    }


// ---------------------------------------------------------
// CIpsPlgSingleOpWatcher::CIpsPlgSingleOpWatcher
//
// ---------------------------------------------------------
//
CIpsPlgSingleOpWatcher::CIpsPlgSingleOpWatcher( 
    MIpsPlgSingleOpWatcher& aObserver )
    : CActive( KSingleOpWatcherPriority ), iObserver( aObserver )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------
// CIpsPlgSingleOpWatcher::ConstructL
//
// ---------------------------------------------------------
//
void CIpsPlgSingleOpWatcher::ConstructL()
    {
    FUNC_LOG;
    // Nothing to do, reserved for future use...
    }


// ---------------------------------------------------------
// CIpsPlgSingleOpWatcher::~CIpsPlgSingleOpWatcher
//
// ---------------------------------------------------------
//
EXPORT_C CIpsPlgSingleOpWatcher::~CIpsPlgSingleOpWatcher()
    {
    FUNC_LOG;
    Cancel();
    delete iOperation;
    delete iBaseOperation;
    delete iRequestObserver;
    }


// ---------------------------------------------------------
// CIpsPlgSingleOpWatcher::SetOperation
// --- Setter ---
// Must only be called once during the lifetime 
//of a CIpsPlgSingleOpWatcher object.
// ---------------------------------------------------------
//
EXPORT_C void CIpsPlgSingleOpWatcher::SetOperation(CMsvOperation* aOperation)
    {
    FUNC_LOG;
    __ASSERT_DEBUG( !IsActive(), 
        User::Panic( KPanicIpsPlgSingleOpWatcher, KErrInUse ) );
    __ASSERT_DEBUG( !iOperation, 
        User::Panic( KPanicIpsPlgSingleOpWatcher, KErrAlreadyExists ) );
    __ASSERT_DEBUG( aOperation, 
        User::Panic( KPanicIpsPlgSingleOpWatcher, KErrNotFound ) );
    __ASSERT_DEBUG( !iBaseOperation, 
        User::Panic( KPanicIpsPlgSingleOpWatcher, KErrAlreadyExists ) );
    //
    // Take ownership of operation and set our active status so we're handled 
    // by the active scheduler.
    iOperation = aOperation;
    iStatus = KRequestPending;
    SetActive();
    }

void CIpsPlgSingleOpWatcher::SetOperation( CIpsPlgBaseOperation* aBaseOperation )
    {
    FUNC_LOG;
    __ASSERT_DEBUG( !IsActive(), 
        User::Panic( KPanicIpsPlgSingleOpWatcher, KErrInUse ) );
    __ASSERT_DEBUG( !iOperation, 
        User::Panic( KPanicIpsPlgSingleOpWatcher, KErrAlreadyExists ) );
    __ASSERT_DEBUG( aBaseOperation, 
        User::Panic( KPanicIpsPlgSingleOpWatcher, KErrNotFound ) );
    __ASSERT_DEBUG( !iBaseOperation, 
        User::Panic( KPanicIpsPlgSingleOpWatcher, KErrAlreadyExists ) );
    
    iBaseOperation = aBaseOperation;
    iStatus = KRequestPending;
    SetActive();
    }

const CIpsPlgBaseOperation* CIpsPlgSingleOpWatcher::BaseOperation( ) const
    {
    FUNC_LOG;
    CIpsPlgBaseOperation* op = NULL;
    
    if ( !iOperation )
        {
        __ASSERT_DEBUG( iBaseOperation, 
            User::Panic( KPanicIpsPlgSingleOpWatcher, KErrNotFound  )  );
        op =  iBaseOperation;
        }
    
    return op;
    }

// ---------------------------------------------------------
// CIpsPlgSingleOpWatcher::Operation
// --- Accessor ---
// ---------------------------------------------------------
//
EXPORT_C CMsvOperation& CIpsPlgSingleOpWatcher::Operation() const
    {
    FUNC_LOG;
    CMsvOperation* op = NULL;
    if ( !iOperation )
        {
        __ASSERT_DEBUG( iBaseOperation, 
            User::Panic( KPanicIpsPlgSingleOpWatcher, KErrNotFound  )  );
        op = static_cast<CMsvOperation*>( iBaseOperation );
        }
    else if ( !iBaseOperation )
        {
        __ASSERT_DEBUG( iOperation, 
            User::Panic( KPanicIpsPlgSingleOpWatcher, KErrNotFound  )  );
        op = iOperation;
        }
        
    return *op;
    }




// ---------------------------------------------------------
// CIpsPlgSingleOpWatcher::DoCancel
// --- From CActive ---
// ---------------------------------------------------------
//
void CIpsPlgSingleOpWatcher::DoCancel()        
    {
    FUNC_LOG;

    // Pass on cancel
    if ( iOperation && iOperation->IsActive() )
        {
        iOperation->Cancel();
        }
    else if ( iBaseOperation && iBaseOperation->IsActive() )
        {
        iBaseOperation->Cancel();
        }
    else
        {
        TRequestStatus* status = &iStatus;
        User::RequestComplete(status, KErrNone);
        }
    }


//------------------------------------
// CIpsPlgSingleOpWatcher::RunL
// --- From CActive ---
// ---------------------------------------------------------
//
void CIpsPlgSingleOpWatcher::RunL()
    {
    FUNC_LOG;
    //
    // Inform client of watcher that the operation has completed
    iObserver.OpCompleted( *this, iStatus.Int() );
    }


void CIpsPlgSingleOpWatcher::SetRequestObserver(
    CIpsPlgImap4MoveRemoteOpObserver* aObserver )
    {
    delete iRequestObserver;
    iRequestObserver = aObserver;
    }

// End of file

