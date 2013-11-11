/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/


#include <sysutil.h> 
#include <cntfldst.h>

#include "contactsgrpdatastore.h"
#include "contactsgrpconverter.h"
#include "changefinder.h"
#include "contactsgrpdataproviderdefs.h"
#include "logger.h"

const TInt KDefaultBufferSize = 1024;
const TInt KDataBufferNotReady = -1;
// -----------------------------------------------------------------------------
// CContactsGrpDataStore::CContactsGrpDataStore
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CContactsGrpDataStore::CContactsGrpDataStore() :
    iKey( TKeyArrayFix( _FOFF( TNSmlSnapshotItem, ItemId() ), ECmpTInt ) )
    {
    TRACE_FUNC;
    }
    
// -----------------------------------------------------------------------------
// CContactsGrpDataStore::ConstructL
// Symbian 2nd phase constructor, can leave.
// -----------------------------------------------------------------------------    
void CContactsGrpDataStore::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    
    User::LeaveIfError( iFs.Connect() );
    
    iNewItems       = new ( ELeave ) CNSmlDataItemUidSet;
    iDeletedItems   = new ( ELeave ) CNSmlDataItemUidSet;
    iUpdatedItems   = new ( ELeave ) CNSmlDataItemUidSet;
    iEmptyList      = new ( ELeave ) CNSmlDataItemUidSet;
    
    iConverter = CContactsGrpConverter::NewL();
    
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CContactsGrpDataStore::~CContactsGrpDataStore
// Destructor
// -----------------------------------------------------------------------------    
CContactsGrpDataStore::~CContactsGrpDataStore()
    {
    TRACE_FUNC_ENTRY;

    delete iDataBuffer;
    delete iNewItems;
    delete iDeletedItems;
    delete iUpdatedItems;
    delete iEmptyList;
    
    if ( iChangeFinder )
        {
        TRAPD( error, iChangeFinder->CloseL() );
        if ( error != KErrNone )
            {
            LOGGER_WRITE_1( "iChangeFinder->CloseL() leaved with %d", error );
            }
        }
    delete iChangeFinder;
    
    delete iConverter;
    
    delete iContactsDb;
    
    iFs.Close();
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CContactsGrpDataStore* CContactsGrpDataStore::NewL()
    {
    TRACE_FUNC_ENTRY;
    CContactsGrpDataStore* self = CContactsGrpDataStore::NewLC();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CContactsGrpDataStore* CContactsGrpDataStore::NewLC()
    {
    TRACE_FUNC_ENTRY;
    CContactsGrpDataStore* self = new ( ELeave ) CContactsGrpDataStore();
    CleanupStack::PushL( self );
    self->ConstructL();
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoOpenL
// Opens database.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoOpenL( const TDesC& /*aStoreName*/,
    MSmlSyncRelationship& aContext, TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    
    iCallerStatus = &aStatus;           
    *iCallerStatus = KRequestPending;
    
    if ( iContactsDb )
        {
        // already opened
        User::RequestComplete( iCallerStatus, KErrInUse );
        LOGGER_WRITE( "CContactsGrpDataStore::DoOpenL failed with KErrInUse." );
        return;
        }
    
    // Create ChangeFinder
    if ( iChangeFinder )
        {
        delete iChangeFinder;
        iChangeFinder = NULL;
        }
    iChangeFinder = CChangeFinder::NewL( aContext, iKey, iHasHistory );
    
    iContactsDb = CContactDatabase::OpenL();
    
    RegisterSnapshotL();
    
    User::RequestComplete( iCallerStatus, KErrNone );
    TRACE_FUNC_EXIT;
    }


// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCancelRequest
// Not supported, does nothing.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCancelRequest()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoStoreName
// Returns the name of the DataStore
// -----------------------------------------------------------------------------
const TDesC& CContactsGrpDataStore::DoStoreName() const
    {
    TRACE_FUNC;
    
    if ( iContactsDb )
        {
        return KContactsGrpStoreName;
        }
    else
        {
        return KNullDesC;
        }
    }
    
// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoBeginTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------    
void CContactsGrpDataStore::DoBeginTransactionL()
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCommitTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCommitTransactionL( TRequestStatus& aStatus )
    {
    TRACE_FUNC;
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoRevertTransaction
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoRevertTransaction( TRequestStatus& aStatus )
    {
    TRACE_FUNC;
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoBeginBatchL
// Batching is not supported.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoBeginBatchL()
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCommitBatchL
// Batching is not supported
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCommitBatchL( RArray<TInt>& /*aResultArray*/, TRequestStatus& aStatus )
    {
    TRACE_FUNC;
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCancelBatch
// Batching is not supported
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCancelBatch()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoSetRemoteStoreFormatL
// Not supported
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoSetRemoteStoreFormatL( const CSmlDataStoreFormat& /*aServerDataStoreFormat*/ )
    {
    TRACE_FUNC;
    }
    
// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoSetRemoteMaxObjectSize
// Not supported
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoSetRemoteMaxObjectSize( TInt /*aServerMaxObjectSize*/ )
    {
    TRACE_FUNC;
    }


// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoMaxObjectSize
// Reads the maximum object size from the central repository
// -----------------------------------------------------------------------------
TInt CContactsGrpDataStore::DoMaxObjectSize() const
    {
    TRACE_FUNC;
    return 0; // no limit
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoOpenItemL
// Opens item in the DataStore, reads it (either completely or partially) 
// to the temporary buffer where it can be later read to the remote database.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoOpenItemL(
    TSmlDbItemUid aUid, 
    TBool& aFieldChange, 
    TInt& aSize, 
    TSmlDbItemUid& aParent, 
    TDes8& aMimeType, 
    TDes8& aMimeVer, 
    TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aUid: %d", aUid );
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    aFieldChange = EFalse;
    aParent = KErrNotFound;
    
    delete iDataBuffer;
    iDataBuffer = NULL;
    
    if ( !iContactsDb )
        {
        User::Leave( KErrNotReady );
        }
    
    iDataBuffer = CBufFlat::NewL( KDefaultBufferSize ); 
    iReaderPosition = 0;
    
    CContactItem* item = iContactsDb->ReadContactLC( aUid );
    if ( item->Type() != KUidContactGroup )
        {
        CleanupStack::PopAndDestroy( item );
        User::Leave( KErrNotFound );
        }

    iConverter->ImportDbItemL( *static_cast<CContactGroup*>(item) );
    
    CleanupStack::PopAndDestroy( item );
    
    iConverter->ExportVCardL( *iDataBuffer );
    iConverter->ResetL();
    aSize = iDataBuffer->Size();
    
    // Set mime type to correct one
    AssignString( aMimeType, KContactsGrpItemMimeType );
    AssignString( aMimeVer, KContactsGrpItemMimeVersion );
    
    User::RequestComplete( iCallerStatus, KErrNone );   
    iCurrentState = EContactGrpOpening;
    
    TRACE_FUNC_EXIT;   
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCreateItemL
// Create new item to the message store.
// Return the id number of the newly created item
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCreateItemL(
    TSmlDbItemUid& aUid, 
    TInt aSize, 
    TSmlDbItemUid /*aParent*/, 
    const TDesC8& /*aMimeType*/, 
    const TDesC8& /*aMimeVer*/, 
    TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "Warning: Unexpected current state: %d", iCurrentState );
        }
        
    if ( !iContactsDb )
        {
        LOGGER_WRITE( "iContactsDb not ready" );
        User::Leave( KErrNotReady );
        }
    
    // Ensure that we've got enough disk space for the item
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aSize, EDriveC ) )
        {
        User::RequestComplete( iCallerStatus, KErrDiskFull );
        LOGGER_WRITE( "SysUtil::DiskSpaceBelowCriticalLevelL failed with KErrDiskFull." );
        return;
        }
    
    // item uid is updated when groups is saved to db.
    iCurrentItem = &aUid; 
    // Save current operation-state so we know what operation
    // is needed to do on DoCommitItemL
    iCurrentState = EContactGrpCreating;
                
    delete iDataBuffer;
    iDataBuffer = NULL;
    iDataBuffer = CBufFlat::NewL( KDefaultBufferSize );
    iWriterPosition = 0;
    
    User::RequestComplete( iCallerStatus, KErrNone );  
    TRACE_FUNC_EXIT;    
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoReplaceItemL
// Begin the replace operation, ensure that the item really exists
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoReplaceItemL(
    TSmlDbItemUid aUid, 
    TInt aSize, 
    TSmlDbItemUid /*aParent*/,
    TBool /*aFieldChange*/, 
    TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aUid: %d", aUid);
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    if ( !iContactsDb )
        {
        LOGGER_WRITE( "iContactsDb not ready" );
        User::Leave( KErrNotReady );
        }
        
    // Ensure that we've got enough disk space for the item
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, aSize, EDriveC ) )
        {
        User::RequestComplete( iCallerStatus, KErrDiskFull );
        LOGGER_WRITE( "SysUtil::DiskSpaceBelowCriticalLevelL failed with KErrDiskFull." );
        return;
        }
    
    // check that item exist and is Group-type
    CContactItem* item = iContactsDb->ReadContactLC( aUid );
    if ( item->Type() != KUidContactGroup )
        {
        CleanupStack::PopAndDestroy( item );
        User::Leave( KErrNotFound );
        }
    CleanupStack::PopAndDestroy( item );
    
    // init databuffer
    delete iDataBuffer;
    iDataBuffer = NULL;
    iDataBuffer = CBufFlat::NewL( KDefaultBufferSize );
    iWriterPosition = 0;
    
    // Save current item, so we know what item needs to be replaced
    iItemToBeReplaced = aUid;
    // Save current operation-state so we know what operation
    // is needed to do on DoCommitItemL
    iCurrentState = EContactGrpUpdating;
    
    User::RequestComplete( iCallerStatus, KErrNone );   
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoReadItemL
// Read specified amount of data from the temporary buffer
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoReadItemL( TDes8& aBuffer )
    {
    TRACE_FUNC_ENTRY;
    
    if ( iCurrentState != EContactGrpOpening || !iDataBuffer )
        {
        LOGGER_WRITE_1( "Unexpected state %d", iCurrentState );
        User::Leave( KErrNotReady );
        }
        
    if ( iReaderPosition == KDataBufferNotReady )
        {
        LOGGER_WRITE( "No data to read" );
        User::Leave( KErrEof );
        }
    
    // Thiw is how much data there is left in the buffer    
    TInt left = iDataBuffer->Size() - iReaderPosition;
    
    if ( left > 0 )
        {
        // This is how much there's space in the destination buffer
        TInt destSize = aBuffer.MaxSize();
        
        // This is how much we can read
        TInt toRead = destSize < left ? destSize : left;
        
        // Read the data from the buffer, then update the position      
        iDataBuffer->Read( iReaderPosition, aBuffer, toRead );
        iReaderPosition += toRead;
        }
    else
        {
        iReaderPosition = KDataBufferNotReady;
        LOGGER_WRITE( "No data to read" );
        User::Leave( KErrEof );
        }   
        
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoWriteItemL
// Write specified amount of data to the temporary buffer
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoWriteItemL( const TDesC8& aData )
    {
    TRACE_FUNC_ENTRY;
    
    if ( iCurrentState != EContactGrpCreating && iCurrentState != EContactGrpUpdating )
        {
        LOGGER_WRITE_1( "Unexpected current state: %d", iCurrentState );
        User::Leave( KErrNotReady );
        }
    
    // Calculate total size
    TInt totalSize = aData.Size() + iDataBuffer->Size();

    // Ensure that we've got enough disk space for the item
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iFs, totalSize, EDriveC ) )
        {
        User::RequestComplete( iCallerStatus, KErrDiskFull );
        LOGGER_WRITE("SysUtil::DiskSpaceBelowCriticalLevelL failed with KErrDiskFull.");
        return;
        }
        
    // Add data to buffer       
    iDataBuffer->InsertL( iWriterPosition, aData );
    iWriterPosition += aData.Size();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCommitItemL
// Commits item from temporary buffer to the message store
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCommitItemL( TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
    if ( iCurrentState != EContactGrpCreating && iCurrentState != EContactGrpUpdating )
        {
        User::RequestComplete( iCallerStatus, KErrNotReady );
        LOGGER_WRITE_1( "Unexpected current state: %d", iCurrentState );
        return;
        }
        
    if ( iDataBuffer->Size() <= 0 )
        {
        User::RequestComplete( iCallerStatus, KErrNotReady );
        LOGGER_WRITE_1( "Data buffer has no data (%d)", iDataBuffer->Size() );
        return; 
        }
    
    if ( !iContactsDb )
        {
        LOGGER_WRITE( "iContactsDb not ready" );
        User::Leave( KErrNotReady );
        }

    iDataBuffer->Compress();
    iConverter->ImportVCardL( iDataBuffer->Ptr(0) );
    
    const CContactIdArray& contactArr = iConverter->ItemsContained();
    
    if ( iCurrentState == EContactGrpCreating )
        {
        *iCurrentItem = CreateNewGroupL( iConverter->GroupLabel(), contactArr );
        }
    else
        {
        ReplaceGroupL( iItemToBeReplaced, iConverter->GroupLabel(), contactArr);
        }
    iConverter->ResetL();
    
    // Destroy buffer
    delete iDataBuffer;
    iDataBuffer = NULL;
    iWriterPosition = 0;
    
    // Restore state and signal we're done
    iCurrentState = EOpenAndWaiting;
    User::RequestComplete( iCallerStatus, KErrNone );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::CreateNewGroupL
// Creates new groupd to db.
// -----------------------------------------------------------------------------
TContactItemId CContactsGrpDataStore::CreateNewGroupL( const TDesC& aLabel,
        const CContactIdArray& aContactArray )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aLabel: %S", &aLabel );
    
    // Check for duplicates
    if ( GroupNameExistsL( aLabel ) )
        {
        LOGGER_WRITE("Duplicate group name found, leave KErrAlreadyExists");
        User::Leave( KErrAlreadyExists );
        }
    
    CContactGroup* group =
        static_cast<CContactGroup*>
        (iContactsDb->CreateContactGroupLC( aLabel ));
    
    // Create new contact group
    
    TContactItemId groupId = group->Id();
    LOGGER_WRITE_1("new group id: %d", groupId);
    
    TBool partiallyUpdated(EFalse);
    TTime lastMod;
    lastMod.UniversalTime();
    TInt err(KErrNone);
    for ( TInt i=0; i<aContactArray.Count(); i++ )
        {
        LOGGER_WRITE_1("Add contact: %d", aContactArray[i] );
        TRAP( err, iContactsDb->AddContactToGroupL( aContactArray[i], groupId ));
        if ( err )
            {
            LOGGER_WRITE_2("Warning: AddContactToGroupL, contact: %d, err: %d", aContactArray[i], err);
            partiallyUpdated = ETrue;
            }
        else
            {
            // If succesfully added, update contact timestamp so contact sync sees those
            // contacts as changed ones.
            UpdateContactLastMod( aContactArray[i], lastMod );
            }
        }
    
    CleanupStack::PopAndDestroy( group );
    group = NULL;
    
    // if item was only partially updated, do not register to snapshot
    //   --> on next sync item is marked as updated
    if ( !partiallyUpdated )
        {
        // reload group
        group = static_cast<CContactGroup*>
            (iContactsDb->ReadContactLC( groupId ));
        // Inform ChangeFinder of new item
        TSnapshotItem snapshotItem( groupId );
        snapshotItem.CreateHashL( *group );
        iChangeFinder->ItemAddedL( snapshotItem );
        
        CleanupStack::PopAndDestroy( group );
        }
    
    
    
    TRACE_FUNC_EXIT;
    return groupId;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::ReplaceGroupL
// Replaces existing group on db
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::ReplaceGroupL( TContactItemId aGroupId, const TDesC& aLabel,
            const CContactIdArray& aContactArray )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aGroupId: %d", aGroupId);
    LOGGER_WRITE_1("aLabel: %S", &aLabel);
    TBool partiallyUpdated(EFalse);
    // check that item exist and is Group-type
    // The LX suffix means that the lock record of the contact item is left on the cleanup stack.
    CContactItem* item = iContactsDb->OpenContactLX( aGroupId );
    CleanupStack::PushL( item );
    if ( item->Type() != KUidContactGroup )
        {
        LOGGER_WRITE("Type was not KUidContactGroup, leaving KErrNotFound");
        User::Leave( KErrNotFound );
        }
    
    // cast item to CContactGroup type
    CContactGroup* groupItem = static_cast<CContactGroup*>(item);
    TContactItemId groupId = groupItem->Id();
    
    TBool labelUpdated(EFalse);
    // Check is group label changed
    if ( groupItem->HasItemLabelField() )
        {
        TPtrC dbLabel = groupItem->GetGroupLabelL();
        if ( aLabel.Compare( dbLabel ) != 0 )
            {
            // didn't match, update label.
            // Check that new name does not already exist
            if ( GroupNameExistsL( aLabel ) )
                {
                LOGGER_WRITE("Name already exists, leave KErrAlreadyExists");
                User::Leave( KErrAlreadyExists );
                }
            LOGGER_WRITE("Update label" );
            groupItem->SetGroupLabelL( aLabel );
            iContactsDb->CommitContactL( *item );
            labelUpdated = ETrue;
            }
        }
    TTime lastMod;
    lastMod.UniversalTime();
    // Update joined contacts
    TInt err(KErrNone);
    // add new items to current group
    for ( TInt i=0; i<aContactArray.Count(); i++ )
        {
        TContactItemId newItem = aContactArray[i];
        TBool founded = groupItem->ContainsItem( newItem );
        if ( !founded )
            {
            // item does not exist on current group, add it.
            LOGGER_WRITE_1("Add contact: %d", newItem)
            TRAP( err, iContactsDb->AddContactToGroupL( newItem, groupId ));
            if ( err )
                {
                LOGGER_WRITE_2("Warning: Could not add contact: %d, err: %d", newItem, err);
                partiallyUpdated = ETrue;
                }
            else
                {
                // Update contact timestamp
                UpdateContactLastMod( newItem, lastMod );
                }
            }
        else
            {
            LOGGER_WRITE_1("contact( %d ) already exist on group, no need to update", newItem)
            }
        }
    
    // remove all missing items from group
    CContactIdArray* oldIdArr = groupItem->ItemsContainedLC();
    if ( oldIdArr )
        {
        for ( TInt i=0; i<oldIdArr->Count(); i++ )
            {
            TContactItemId oldItem = (*oldIdArr)[i];
            TInt err = aContactArray.Find( oldItem );
            if ( err == KErrNotFound )
                {
                LOGGER_WRITE_1("Remove contact: %d", oldItem );
                // old item was not found from new item list, remove it.
                TRAP( err, iContactsDb->RemoveContactFromGroupL(oldItem, groupId));
                if ( err )
                    {
                    LOGGER_WRITE_2("Warning: Could not remove contact: %d, err: ",oldItem, err );
                    partiallyUpdated = ETrue;
                    }
                else
                    {
                    UpdateContactLastMod( oldItem, lastMod );
                    }
                }
            // Update all contacts in group if label is changed
            else if ( labelUpdated )
                {
                UpdateContactLastMod( oldItem, lastMod );
                }
            }
        }
    CleanupStack::PopAndDestroy( oldIdArr );
    CleanupStack::PopAndDestroy( 2 ); // item, lock object 
    
    // if item was only partially updated, do not register to snapshot
    //   --> on next sync item is marked as updated
    if ( !partiallyUpdated )
        {
        // reload group
        groupItem = NULL;
        groupItem = static_cast<CContactGroup*>
            (iContactsDb->ReadContactLC( groupId ));
        // Inform ChangeFinder of updated item
        TSnapshotItem snapshotItem( groupId );
        snapshotItem.CreateHashL( *groupItem );
        iChangeFinder->ItemUpdatedL( snapshotItem );
        
        CleanupStack::PopAndDestroy( groupItem );
        }
    
    TRACE_FUNC_EXIT;
    }
// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCloseItem
// Closes open item in the data store
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCloseItem()
    {
    TRACE_FUNC_ENTRY;
    // Reset read buffer 
    iReaderPosition = KDataBufferNotReady;
    delete iDataBuffer;
    iDataBuffer = NULL;
    
    // Make sure that we're opened an item
    if ( iCurrentState != EContactGrpOpening )
        {
        LOGGER_WRITE_1( "WARNING: Invalid state %d.", iCurrentState );
        }
    // Start to wait for the next operation     
   iCurrentState = EOpenAndWaiting;
    
    TRACE_FUNC_EXIT;   
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoMoveItemL
// Not supported.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoMoveItemL( TSmlDbItemUid /*aUid*/,
    TSmlDbItemUid /*aNewParent*/, TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Restore state and signal we're done
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoDeleteItemL
// Removes item from the message store
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "aUid: %d", aUid );
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CContactsGrpDataStore::DoDeleteItemL, Incorrect state: %d", iCurrentState);
        }
    
    if ( !iContactsDb )
        {
        LOGGER_WRITE( "iContactsDb not ready" );
        User::Leave( KErrNotReady );
        }
    
    // check that item type is ContactGroup
    CContactItem* item = iContactsDb->ReadContactLC( aUid );
    if ( item->Type() != KUidContactGroup )
        {
        LOGGER_WRITE("Item was not ContactGroup");
        CleanupStack::PopAndDestroy( item );
        User::Leave( KErrNotFound );
        }
    
    // Update all contacts in group
    CContactGroup* groupItem = static_cast<CContactGroup*>(item);
    CContactIdArray* contactArr = groupItem->ItemsContainedLC();
    if ( contactArr )
        {
        TTime lastMod;
        lastMod.UniversalTime();
        for ( TInt i=0; i<contactArr->Count(); i++ )
            {
            TContactItemId contactId = (*contactArr)[i];
            UpdateContactLastMod( contactId, lastMod );
            }
        }
    CleanupStack::PopAndDestroy( contactArr );
    CleanupStack::PopAndDestroy( item );
    
    // delete group
    iContactsDb->DeleteContactL( aUid );
    
    TSnapshotItem snapshotItem( aUid );     
    iChangeFinder->ItemDeleted( snapshotItem );
    
    User::RequestComplete( iCallerStatus, KErrNone );
        
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoSoftDeleteItemL
// Soft delete isn't supported.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoSoftDeleteItemL(TSmlDbItemUid /*aUid*/, TRequestStatus& aStatus)
    {
    TRACE_FUNC_ENTRY;
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    User::RequestComplete(iCallerStatus, KErrNotSupported); 
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoDeleteAllItemsL
// Deletes all items in the standard folders of bookmark store
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoDeleteAllItemsL( TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;

    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    if ( !iContactsDb )
        {
        LOGGER_WRITE( "iContactsDb is not opened, Leaving KErrNotReady");
        User::Leave( KErrNotReady );
        }
    
    CContactIdArray* groups = iContactsDb->GetGroupIdListL();
    if ( groups )
        {
        CleanupStack::PushL( groups );
        TTime lastMod;
        lastMod.UniversalTime();
        
        // update all contacts in all groups
        for ( TInt i=0; i<groups->Count(); i++ )
            {
            // check that item type is ContactGroup
            CContactItem* item = iContactsDb->ReadContactLC( (*groups)[i] );
            
            // Update all contacts in group
            CContactGroup* groupItem = static_cast<CContactGroup*>(item);
            CContactIdArray* contactArr = groupItem->ItemsContainedLC();
            if ( contactArr )
                {
                for ( TInt i=0; i<contactArr->Count(); i++ )
                    {
                    TContactItemId contactId = (*contactArr)[i];
                    UpdateContactLastMod( contactId, lastMod );
                    }
                }
            CleanupStack::PopAndDestroy( contactArr );
            CleanupStack::PopAndDestroy( item );
            }
        
        // delete all groups
        iContactsDb->DeleteContactsL( *groups );
        CleanupStack::PopAndDestroy( groups );
        }
    
    // Reset the whole change finder
    iChangeFinder->ResetL();
    
    User::RequestComplete( iCallerStatus, KErrNone );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoHasSyncHistory
// This method returns ETrue if Data Store has history information. 
// Slow-sync will be used if Data Store does not have history information.
// -----------------------------------------------------------------------------
TBool CContactsGrpDataStore::DoHasSyncHistory() const
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "iHasHistory: %d", iHasHistory );
    TRACE_FUNC_EXIT;  
    return iHasHistory;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoAddedItems
// This method returns UIDs of added items. Those items are added after previous
// synchronization with current synchronization relationship. 
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CContactsGrpDataStore::DoAddedItems() const
    {
    TRACE_FUNC_ENTRY;
    
    // Clear new-items array
    iNewItems->Reset();
    if ( !iChangeFinder )
        {
        LOGGER_WRITE( "WARNING: Not ready, returns empty item list" );
        return *iNewItems;      
        }
    
    // Search for new items
    TRAPD( error, iChangeFinder->FindNewItemsL( *iNewItems ) )
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1( "CContactsGrpDataStore::DoAddedItems, iChangeFinder->FindNewItemsL leaved with %d.", error );
        } 
    
    LOGGER_WRITE_1( "New item count: %d.", iNewItems->ItemCount() );    
    TRACE_FUNC_EXIT;
    
    return *iNewItems;  
    }
    
// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoDeletedItems
// Returns ids of items, which are deleted after previous synchronization
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CContactsGrpDataStore::DoDeletedItems() const
    {
    TRACE_FUNC_ENTRY;    
    
    // Clear deleted-items array
    iDeletedItems->Reset();
    if ( !iChangeFinder )
        {
        LOGGER_WRITE( "WARNING: Not ready, returns empty item list" );
        return *iDeletedItems;      
        }
    
    // Search for deleted items
    TRAPD( error, iChangeFinder->FindDeletedItemsL( *iDeletedItems ) );
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1( "CContactsGrpDataStore::DoDeletedItems, iChangeFinder->FindDeletedItemsL leaved with %d.", error );
        }
    
    LOGGER_WRITE_1( "Deleted item count: %d.", iDeletedItems->ItemCount() );
    TRACE_FUNC_EXIT;
    return *iDeletedItems;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoSoftDeletedItems
// Not directly supported, empty list returned
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CContactsGrpDataStore::DoSoftDeletedItems() const
    {
    TRACE_FUNC;
    // Return empty array as a result
    return *iEmptyList;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoModifiedItems
// Finds all modified items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CContactsGrpDataStore::DoModifiedItems() const
    {
    TRACE_FUNC_ENTRY;
    
    // Clear updated-items array
    iUpdatedItems->Reset();
    if ( !iChangeFinder )
        {
        LOGGER_WRITE( "WARNING: Not ready, returns empty item list" );
        return *iUpdatedItems;      
        }
    
    // Search for updated items
    TRAPD( error, iChangeFinder->FindChangedItemsL( *iUpdatedItems ) )
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1( "CContactsGrpDataStore::DoModifiedItems, iChangeFinder->FindChangedItemsL leaved with %d.", error );
        }
    
    LOGGER_WRITE_1( "Modified item count: %d.", iUpdatedItems->ItemCount() );
    TRACE_FUNC_EXIT;     
    return *iUpdatedItems;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoMovedItems
// Finds all moved items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CContactsGrpDataStore::DoMovedItems() const
    {
    TRACE_FUNC;
    // Moved items are not supported
    // Return empty array as a result
    return *iEmptyList;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoResetChangeInfoL
// Resets change history in the data store. All content is considered
// new in the data store point of view.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoResetChangeInfoL( TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1( "CContactsGrpDataStore::DoResetChangeInfoL, invalid state %d.", iCurrentState );
        }   
            
    // Reset change info in ChangeFinder
    iChangeFinder->ResetL();
    iHasHistory = EFalse;
    
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );   
        
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCommitChangeInfoL
// Commits change info. These items are no longer reported, when change
// information is being queried.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus,
    const MSmlDataItemUidSet& aItems )
    {
    TRACE_FUNC_ENTRY;
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1( "CContactsGrpDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState );
        }

    // Notify ChangeFinder
    LOGGER_WRITE_1( "CContactsGrpDataStore::DoCommitChangeInfoL, item count %d.", aItems.ItemCount() );
    iChangeFinder->CommitChangesL( aItems );
    iHasHistory = ETrue;
        
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::DoCommitChangeInfoL
// Commits change info. There is no more nothing to report when change
// information is being queried. 
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;

    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1( "CContactsGrpDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState );
        }
    
    // Notify ChangeFinder
    iChangeFinder->CommitChangesL();
    iHasHistory = ETrue;
        
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );
        
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::RegisterSnapshotL
// Sets Changefinder to compare against current message store content
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::RegisterSnapshotL()
    {
    TRACE_FUNC_ENTRY;
    
    CArrayFixSeg<TSnapshotItem>* snapshot = 
        new ( ELeave ) CArrayFixSeg<TSnapshotItem>( KSnapshotGranularity );
    CleanupStack::PushL( snapshot );
    
    // Add everything to snapshot
    
    // GetGroupIdListL returns NULL if there are no groups in the database.
    CContactIdArray* groups = iContactsDb->GetGroupIdListL();
    if ( groups )
        {
        TKeyArrayFix key( iKey );
        CleanupStack::PushL( groups );
        for ( TInt i=0; i<groups->Count(); i++ )
            {
            CContactItem* item = iContactsDb->ReadContactLC((*groups)[i]);
            CContactGroup* groupItem = static_cast<CContactGroup*>(item);
            
            LOGGER_WRITE_1( "Add group to snatshot, group id: %d",groupItem->Id() );
            
            TSnapshotItem snapshotItem( groupItem->Id() );
            snapshotItem.CreateHashL( *groupItem );
            
            snapshot->InsertIsqL( snapshotItem, key );
            
            CleanupStack::PopAndDestroy( item );
            }
        CleanupStack::PopAndDestroy( groups );
        }
    
    // Set new snapshot to compare against
    iChangeFinder->SetNewSnapshot( snapshot );
    
    // Changefinder takes ownership of the snapshot
    CleanupStack::Pop( snapshot );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::AssignString
// Assigns data from one descriptor into another, truncates if too long 
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::AssignString( TDes8& aDestination, const TDesC8& aSource )
    {
    TInt targetLength = aSource.Length();
    if ( aDestination.MaxLength() < targetLength )
        {
        targetLength = aDestination.MaxLength();
        }
        
    aDestination.Copy( aSource.Ptr(), targetLength );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::UpdateContactLastMod
// Updates contact item last modified time. Contact sync will see item as changed one.
// -----------------------------------------------------------------------------
void CContactsGrpDataStore::UpdateContactLastMod( TContactItemId aContactId,
        const TTime& aLastModified )
    {
    // Ignore errors.
    // Cannot update timestamp if contact is already open. However modified time
    // is most likely still updated by UI.
    TRAP_IGNORE(
        // OpenContactLX() places the edit lock on the cleanup stack
        CContactItem* contact = iContactsDb->OpenContactLX( aContactId );
        CleanupStack::PushL( contact );
        contact->SetLastModified( aLastModified );
        iContactsDb->CommitContactL( *contact );
        CleanupStack::PopAndDestroy( contact );
        CleanupStack::PopAndDestroy( 1 ); // lock object
        );
    }

// -----------------------------------------------------------------------------
// CContactsGrpDataStore::GroupNameExistsL
// Checks does group name already exists.
// -----------------------------------------------------------------------------
TBool CContactsGrpDataStore::GroupNameExistsL( const TDesC& aLabel )
    {
    TBool duplicateFound( EFalse );
    // GetGroupIdListL returns NULL if there are no groups in the database.
    CContactIdArray* groups = iContactsDb->GetGroupIdListL();
    if ( groups )
        {
        CleanupStack::PushL( groups );
        for ( TInt i=0; i<groups->Count() && !duplicateFound; i++ )
            {
            CContactItem* item = iContactsDb->ReadContactLC((*groups)[i]);
            CContactGroup* groupItem = static_cast<CContactGroup*>(item);
            
            if ( groupItem->HasItemLabelField() )
                {
                TPtrC label = groupItem->GetGroupLabelL();
                if ( aLabel.Compare( label ) == 0 )
                    {
                    duplicateFound = ETrue;
                    }
                }
            CleanupStack::PopAndDestroy( item );
            }
        CleanupStack::PopAndDestroy( groups );
        }
    
    return duplicateFound;
    }
