/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Source code file for datastorewaiter helper class.
*
*/

//  CLASS HEADER
#include "cnsmldummydatastorewaiter.h"

// INTERNAL INCLUDES
#include "nsmldummydatastore.h"

//  EXTERNAL INCLUDES
#include <eunitmacros.h>

// CONSTANTS


// --- Constructors & destructors --------------------------------------------
CNSmlDummyDataStoreWaiter* CNSmlDummyDataStoreWaiter::NewL(
        CNSmlDummyDataStore& aStore )
    {
    CNSmlDummyDataStoreWaiter* self =
        CNSmlDummyDataStoreWaiter::NewLC( aStore );
    CleanupStack::Pop( self );

    return self;
    }

CNSmlDummyDataStoreWaiter* CNSmlDummyDataStoreWaiter::NewLC(
    CNSmlDummyDataStore& aStore )
    {
    CNSmlDummyDataStoreWaiter* self =
        new ( ELeave ) CNSmlDummyDataStoreWaiter( aStore );
    CleanupStack::PushL( self );
    self->ConstructL();

    return self;
    }

CNSmlDummyDataStoreWaiter::~CNSmlDummyDataStoreWaiter()
    {
    Cancel();
    delete iWait;
    iOpen = EFalse;
    }

CNSmlDummyDataStoreWaiter::CNSmlDummyDataStoreWaiter(
    CNSmlDummyDataStore& aStore ):
    CActive( EPriorityNormal ), iStore( aStore ), iOpen( EFalse )
    {

    }

void CNSmlDummyDataStoreWaiter::ConstructL()
    {
    CActiveScheduler::Add( this );
    iWait = new ( ELeave ) CActiveSchedulerWait();
    }

// --- From CActive ----------------------------------------------------------
void CNSmlDummyDataStoreWaiter::DoCancel()
    {
    iStore.CancelRequest();
    }

void CNSmlDummyDataStoreWaiter::RunL()
    {
    iWait->AsyncStop();
    }

// --- Test methods -----------------------------------------------------------

void CNSmlDummyDataStoreWaiter::OpenStoreL( MSmlSyncRelationship& aRelationship, 
    const TDesC& /*aStoreName*/ )
    {
    Cancel();
    iStore.OpenL( _L("C:Contacts.cdb"), aRelationship, iStatus );
    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }
    iOpen = ETrue;
    }

TInt CNSmlDummyDataStoreWaiter::DoCommitChangeInfoL(
    const MSmlDataItemUidSet& aItems )
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }

    Cancel();
    iStore.DoCommitChangeInfoL( iStatus, aItems );
    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

TInt CNSmlDummyDataStoreWaiter::DoResetChangeInfoL()
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }

    Cancel();
    iStore.DoResetChangeInfoL( iStatus );
    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

TInt CNSmlDummyDataStoreWaiter::DoCommitChangeInfoL()
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }

    Cancel();
    iStore.DoCommitChangeInfoL( iStatus );
    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

// Item addition in "normal" (non-batch) mode
TInt CNSmlDummyDataStoreWaiter::DoCreateItemL( TSmlDbItemUid& aUid,
    TInt aSize, const TDesC8& aMimeType, const TDesC8& aMimeVer )
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }
    Cancel();
    iStore.DoCreateItemL( aUid, aSize, 0, aMimeType, aMimeVer, iStatus );

    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

TInt CNSmlDummyDataStoreWaiter::DoCommitItemL()
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }
    Cancel();
    iStore.DoCommitItemL( iStatus );

    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

TInt CNSmlDummyDataStoreWaiter::DoCommitBatchL( RArray<TInt>& aResultArray )
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }
    Cancel();
    iStore.DoCommitBatchL( aResultArray, iStatus );

    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

void CNSmlDummyDataStoreWaiter::DoCancelBatch()
    {
    if( !iOpen )
        {
        return;
        }
    iStore.DoCancelBatch();
    }

TInt CNSmlDummyDataStoreWaiter::DoOpenItemL( TSmlDbItemUid aUid,
    TBool& aFieldChange, TInt& aSize, TSmlDbItemUid& aParent,
    TDes8& aMimeType, TDes8& aMimeVer )
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }
    Cancel();
    iStore.OpenItemL( aUid, aFieldChange, aSize, aParent, aMimeType,
        aMimeVer, iStatus );

    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

TInt CNSmlDummyDataStoreWaiter::DoReplaceItemL( TSmlDbItemUid aUid,
    TInt aSize, TSmlDbItemUid aParent, TBool aFieldChange )
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }
    Cancel();
    iStore.ReplaceItemL( aUid, aSize, aParent, aFieldChange, iStatus );
    SetActive();
    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }
    return iStatus.Int();
    }

TInt CNSmlDummyDataStoreWaiter::DoDeleteItemL( TSmlDbItemUid aUid )
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }
    Cancel();
    iStore.DeleteItemL( aUid, iStatus );

    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

TInt CNSmlDummyDataStoreWaiter::DoDeleteAllItemsL()
    {
    if( !iOpen )
        {
        User::Leave( KErrNotReady );
        }
    Cancel();
    iStore.DeleteAllItemsL( iStatus );

    SetActive();

    if( !iWait->IsStarted() )
        {
        iWait->Start();
        }

    return iStatus.Int();
    }

// End of file
