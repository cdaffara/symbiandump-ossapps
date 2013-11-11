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
* Description:  DS Dummy datastore
*
*/

//  CLASS HEADER
#include "nsmldummydatastore.h"

//  EXTERNAL INCLUDES
#include <DataSyncInternalPSKeys.h>
#include <e32property.h>
#include <nsmldebug.h>

// -----------------------------------------------------------------------------
// CNSmlDummyDataStore::NewL
// -----------------------------------------------------------------------------
//
CNSmlDummyDataStore* CNSmlDummyDataStore::NewL()
    {
    CNSmlDummyDataStore* self = CNSmlDummyDataStore::NewLC();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDummyDataStore::NewLC
// -----------------------------------------------------------------------------
//
CNSmlDummyDataStore* CNSmlDummyDataStore::NewLC()
    {
    CNSmlDummyDataStore* self = new( ELeave ) CNSmlDummyDataStore;
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CNSmlDummyDataStore::CNSmlDummyDataStore
// -----------------------------------------------------------------------------
//
CNSmlDummyDataStore::CNSmlDummyDataStore()
    {
    _DBG_FILE("CNSmlDummyDataStore::CNSmlDummyDataStore()");
    }

// -----------------------------------------------------------------------------
// CNSmlDummyDataStore::ConstructL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::ConstructL()
    {
    _DBG_FILE("CNSmlDummyDataStore::ConstructL(): begin");
    CNSmlContactsDataStore::ConstructL();
    _DBG_FILE("CNSmlDummyDataStore::ConstructL(): end");
    }

// -----------------------------------------------------------------------------
// CNSmlDummyDataStore::~CNSmlDummyDataStore
// -----------------------------------------------------------------------------
//
CNSmlDummyDataStore::~CNSmlDummyDataStore()
    {
    _DBG_FILE("CNSmlDummyDataStore::~CNSmlDummyDataStore()");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoOpenL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoOpenL( const TDesC& aStoreName,
    MSmlSyncRelationship& aContext, TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoOpenL(): begin");
    CNSmlContactsDataStore::DoOpenL( aStoreName, aContext, aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoOpenL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCancelRequest
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoCancelRequest()
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCancelRequest(): begin");
    CNSmlContactsDataStore::DoCancelRequest();
    _DBG_FILE("CNSmlDummyDataStore::DoCancelRequest(): end");
    }

// -----------------------------------------------------------------------------
// const TDesC& CNSmlDummyDataStore::DoStoreName
// -----------------------------------------------------------------------------
//
const TDesC& CNSmlDummyDataStore::DoStoreName() const
    {
    _DBG_FILE("CNSmlDummyDataStore::DoStoreName(): begin");
    return CNSmlContactsDataStore::DoStoreName();
    _DBG_FILE("CNSmlDummyDataStore::DoStoreName(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoBeginTransactionL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoBeginTransactionL()
    {
    _DBG_FILE("CNSmlDummyDataStore::DoBeginTransactionL(): begin");
    CNSmlContactsDataStore::DoBeginTransactionL();
    _DBG_FILE("CNSmlDummyDataStore::DoBeginTransactionL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCommitTransactionL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoCommitTransactionL( TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCommitTransactionL(): begin");
    CNSmlContactsDataStore::DoCommitTransactionL( aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoCommitTransactionL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoRevertTransaction
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoRevertTransaction( TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoRevertTransaction(): begin");
    CNSmlContactsDataStore::DoRevertTransaction( aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoRevertTransaction(): end");
    }

// ----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoBeginBatchL
// ----------------------------------------------------------------------------
void CNSmlDummyDataStore::DoBeginBatchL()
    {
    _DBG_FILE("CNSmlDummyDataStore::DoBeginBatchL(): begin");
    CNSmlContactsDataStore::DoBeginBatchL();
    _DBG_FILE("CNSmlDummyDataStore::DoBeginBatchL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCommitBatchL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoCommitBatchL( RArray< TInt >& aResultArray,
    TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCommitBatchL(): begin");
    CNSmlContactsDataStore::DoCommitBatchL( aResultArray, aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoCommitBatchL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCancelBatch
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoCancelBatch()
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCancelBatch(): begin");
    CNSmlContactsDataStore::DoCancelBatch();
    _DBG_FILE("CNSmlDummyDataStore::DoCancelBatch(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoSetRemoteStoreFormatL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoSetRemoteStoreFormatL(
    const CSmlDataStoreFormat& aServerDataStoreFormat )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoSetRemoteStoreFormatL(): begin");
    CNSmlContactsDataStore::DoSetRemoteStoreFormatL( aServerDataStoreFormat );
    _DBG_FILE("CNSmlDummyDataStore::DoSetRemoteStoreFormatL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoSetRemoteMaxObjectSize
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoSetRemoteMaxObjectSize( TInt aServerMaxObjectSize )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoSetRemoteMaxObjectSize(): begin");
    CNSmlContactsDataStore::DoSetRemoteMaxObjectSize( aServerMaxObjectSize );
    _DBG_FILE("CNSmlDummyDataStore::DoSetRemoteMaxObjectSize(): end");
    }

// -----------------------------------------------------------------------------
// TInt CNSmlDummyDataStore::DoMaxObjectSize
// -----------------------------------------------------------------------------
//
TInt CNSmlDummyDataStore::DoMaxObjectSize() const
    {
    _DBG_FILE("CNSmlDummyDataStore::DoMaxObjectSize()");
    return CNSmlContactsDataStore::DoMaxObjectSize();
    }

// ----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoOpenItemL
// ----------------------------------------------------------------------------
void CNSmlDummyDataStore::DoOpenItemL( TSmlDbItemUid aUid,
    TBool& aFieldChange, TInt& aSize, TSmlDbItemUid& aParent,
    TDes8& aMimeType, TDes8& aMimeVer, TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoOpenItemL(): begin");
	CNSmlContactsDataStore::DoOpenItemL( aUid, aFieldChange,
			aSize, aParent, aMimeType, aMimeVer, aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoOpenItemL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCreateItemL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoCreateItemL( TSmlDbItemUid& aUid, TInt aSize,
    TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& aMimeVer, 
    TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCreateItemL(): begin");
    CNSmlContactsDataStore::DoCreateItemL( aUid, aSize, aParent, aMimeType, 
        aMimeVer, aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoCreateItemL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoReplaceItemL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoReplaceItemL( TSmlDbItemUid aUid, TInt aSize, 
    TSmlDbItemUid aParent, TBool aFieldChange, TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoReplaceItemL(): begin");
    CNSmlContactsDataStore::DoReplaceItemL( aUid, aSize, aParent, aFieldChange, 
        aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoReplaceItemL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoReadItemL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoReadItemL( TDes8& aBuffer )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoReadItemL(): begin");
    CNSmlContactsDataStore::DoReadItemL( aBuffer );
    _DBG_FILE("CNSmlDummyDataStore::DoReadItemL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoWriteItemL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoWriteItemL( const TDesC8& aData )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoWriteItemL(): begin");
    CNSmlContactsDataStore::DoWriteItemL( aData );
    _DBG_FILE("CNSmlDummyDataStore::DoWriteItemL(): end");
    }

// ----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCommitItemL()
// ----------------------------------------------------------------------------
void CNSmlDummyDataStore::DoCommitItemL( TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCommitItemL(): begin");
    CNSmlContactsDataStore::DoCommitItemL( aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoCommitItemL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCloseItem
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoCloseItem()
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCloseItem(): begin");
    CNSmlContactsDataStore::DoCloseItem();
    _DBG_FILE("CNSmlDummyDataStore::DoCloseItem(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoMoveItemL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoMoveItemL( TSmlDbItemUid aUid, 
    TSmlDbItemUid aNewParent, TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoMoveItemL(): begin");
    CNSmlContactsDataStore::DoMoveItemL( aUid, aNewParent, aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoMoveItemL(): end");
    }

// ----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoDeleteItemL
// ----------------------------------------------------------------------------
void CNSmlDummyDataStore::DoDeleteItemL(
    TSmlDbItemUid aUid, TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoDeleteItemL(): begin");
    CNSmlContactsDataStore::DoDeleteItemL( aUid, aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoDeleteItemL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoSoftDeleteItemL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoSoftDeleteItemL( TSmlDbItemUid aUid, 
    TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoSoftDeleteItemL(): begin");
    CNSmlContactsDataStore::DoSoftDeleteItemL( aUid, aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoSoftDeleteItemL(): end");
    }

// -----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoDeleteAllItemsL
// -----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoDeleteAllItemsL( TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::: begin");
    CNSmlContactsDataStore::DoDeleteAllItemsL( aStatus );
    _DBG_FILE("CNSmlDummyDataStore::: end");
    }

// ----------------------------------------------------------------------------
// TBool CNSmlDummyDataStore::DoHasSyncHistory
// ----------------------------------------------------------------------------
TBool CNSmlDummyDataStore::DoHasSyncHistory() const
    {
    _DBG_FILE("CNSmlDummyDataStore::DoHasSyncHistory()");
    return CNSmlContactsDataStore::DoHasSyncHistory();
    }

// ----------------------------------------------------------------------------
// const MSmlDataItemUidSet& CNSmlDummyDataStore::DoAddedItems
// ----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlDummyDataStore::DoAddedItems() const
    {
    _DBG_FILE("CNSmlDummyDataStore::DoAddedItems()");
    return CNSmlContactsDataStore::DoAddedItems();
    }

// ----------------------------------------------------------------------------
// const MSmlDataItemUidSet& CNSmlDummyDataStore::DoDeletedItems
// ----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlDummyDataStore::DoDeletedItems() const
    {
    _DBG_FILE("CNSmlDummyDataStore::DoDeletedItems()");
    return CNSmlContactsDataStore::DoDeletedItems();
    }

// ----------------------------------------------------------------------------
// const MSmlDataItemUidSet& CNSmlDummyDataStore::DoSoftDeletedItems
// ----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlDummyDataStore::DoSoftDeletedItems() const
    {
    _DBG_FILE("CNSmlDummyDataStore::DoSoftDeletedItems()");
    return CNSmlContactsDataStore::DoSoftDeletedItems();
    }

// ----------------------------------------------------------------------------
// const MSmlDataItemUidSet& CNSmlDummyDataStore::DoModifiedItems
// ----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlDummyDataStore::DoModifiedItems() const
    {
    _DBG_FILE("CNSmlDummyDataStore::DoModifiedItems()");
    return CNSmlContactsDataStore::DoModifiedItems();
    }

// ----------------------------------------------------------------------------
// const MSmlDataItemUidSet& CNSmlDummyDataStore::DoMovedItems
// ----------------------------------------------------------------------------
//
const MSmlDataItemUidSet& CNSmlDummyDataStore::DoMovedItems() const
    {
    _DBG_FILE("CNSmlDummyDataStore::DoMovedItems()");
    return CNSmlContactsDataStore::DoMovedItems();
    }

// ----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoResetChangeInfoL
// ----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoResetChangeInfoL( TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoResetChangeInfoL(): begin");
    CNSmlContactsDataStore::DoResetChangeInfoL( aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoResetChangeInfoL(): end");
    }

// ----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCommitChangeInfoL
// ----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus,
    const MSmlDataItemUidSet& aItems )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCommitChangeInfoL(aItems): begin");
    CNSmlContactsDataStore::DoCommitChangeInfoL( aStatus, aItems );
    _DBG_FILE("CNSmlDummyDataStore::DoCommitChangeInfoL(aItems): end");
    }

// ----------------------------------------------------------------------------
// void CNSmlDummyDataStore::DoCommitChangeInfoL
// ----------------------------------------------------------------------------
//
void CNSmlDummyDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus )
    {
    _DBG_FILE("CNSmlDummyDataStore::DoCommitChangeInfoL(): begin");
    CNSmlContactsDataStore::DoCommitChangeInfoL( aStatus );
    _DBG_FILE("CNSmlDummyDataStore::DoCommitChangeInfoL(): end");
    }

// -----------------------------------------------------------------------------
// const TDesC& CNSmlDummyDataStore::GetStoreFormatResourceFileL
// -----------------------------------------------------------------------------
const TDesC& CNSmlDummyDataStore::GetStoreFormatResourceFileL() const
    {
    _DBG_FILE("CNSmlDummyDataStore::GetStoreFormatResourceFileL()");
    // Check correct Data Sync protocol
    TInt value( EDataSyncNotRunning );
    TInt error = RProperty::Get( KPSUidDataSynchronizationInternalKeys,
                                 KDataSyncStatus,
                                 value );
    if ( error == KErrNone && value == EDataSyncRunning )
        {
        return KNSmlDummyContactsStoreFormatRsc_1_1_2;
        }
    else // error or protocol version 1.2
        {
        return KNSmlDummyContactsStoreFormatRsc_1_2;
        }
    }

// -----------------------------------------------------------------------------
// TInt CNSmlDummyDataStore::DoExecuteBufferL
// -----------------------------------------------------------------------------
//
TInt CNSmlDummyDataStore::DoExecuteBufferL( RArray< TInt >& aResultArray)
    {
    _DBG_FILE("CNSmlDummyDataStore::DoExecuteBufferL()");
    return CNSmlContactsDataStore::DoExecuteBufferL( aResultArray );
    }

// -----------------------------------------------------------------------------
// CNSmlDataModBase& CNSmlDummyDataStore::GetDataMod
// -----------------------------------------------------------------------------
//
CNSmlDataModBase& CNSmlDummyDataStore::GetDataMod()
    {
    _DBG_FILE("CNSmlDummyDataStore::GetDataMod()");
    return CNSmlContactsDataStore::GetDataMod();
    }

// -----------------------------------------------------------------------------
// TInt CNSmlDummyDataStore::ExportContactsL
// -----------------------------------------------------------------------------
//
TInt CNSmlDummyDataStore::ExportContactsL( const TUid& aUid, 
    CBufBase& aContactBufBase )
    {
    _DBG_FILE("CNSmlDummyDataStore::ExportContactsL()");
    return CNSmlContactsDataStore::ExportContactsL( aUid, aContactBufBase );
    }

// -----------------------------------------------------------------------------
// CArrayFixFlat<TUid>* CNSmlDummyDataStore::ImportContactsL
// -----------------------------------------------------------------------------
//
CArrayFixFlat<TUid>* CNSmlDummyDataStore::ImportContactsL( 
    const TDesC8& aContactBufBase )
    {
    _DBG_FILE("CNSmlDummyDataStore::ImportContactsL()");
    return CNSmlContactsDataStore::ImportContactsL( aContactBufBase );
    }

// END OF FILE
