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
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/


#include "changefinder.h"
#include "logger.h"

const TUint KSnapshotFormatVersion ( 0xf0000001 ); // format version

// -----------------------------------------------------------------------------
// CChangeFinder::NewL
// Static function to create CChangeFider object(s)
// -----------------------------------------------------------------------------
CChangeFinder* CChangeFinder::NewL( MSmlSyncRelationship& aSyncRelationship, TKeyArrayFix aKey,
        TBool& aHasHistory, TInt aStreamUid )
    {
    CChangeFinder* self = new (ELeave) CChangeFinder( aSyncRelationship, aKey, aStreamUid );
    CleanupStack::PushL( self );
    self->ConstructL( aHasHistory );
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::CChangeFinder
// Constructor for the class
// -----------------------------------------------------------------------------
CChangeFinder::CChangeFinder( MSmlSyncRelationship& aSyncRelationship, TKeyArrayFix aKey, TInt aStreamUid ) :
iSyncRelationship( aSyncRelationship ),
iKey(aKey),
iStreamUid( aStreamUid ),
iDataStoreUid( KErrNotFound )
    {
        
    }

// -----------------------------------------------------------------------------
// CChangeFinder::~CChangeFinder
// Destructor for the class, closes the ChangeFinder and writes snapshot to stream
// -----------------------------------------------------------------------------
CChangeFinder::~CChangeFinder()
    {
    TRACE_FUNC_ENTRY;
    TInt error;
    TRAP( error, CloseL() );
    if ( error != KErrNone )
        {
        LOGGER_WRITE( "CChangeFinder::~CChangeFinder, CloseL failed." );
        }
    
    delete iCurrentSnapshot;
    iCurrentSnapshot = NULL;
    delete iOldSnapshot;
    iOldSnapshot = NULL;
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ConstructL
// 2nd phase constructor for the class, reads snapshot from stream
// -----------------------------------------------------------------------------
void CChangeFinder::ConstructL( TBool& aHasHistory )
    {
    TRACE_FUNC_ENTRY;
    
    TUid streamId;
    streamId.iUid = iStreamUid;
    
    aHasHistory = iSyncRelationship.IsStreamPresentL(streamId);
    
    if ( aHasHistory )
        {
        LOGGER_WRITE("CChangeFinder::ConstructL, history exists.");
        RReadStream readStream;
        iSyncRelationship.OpenReadStreamLC(readStream, streamId);
        
        // Read the index, first create snapshot array
        iOldSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
        
        // Read used format version
        TUint formatVer = readStream.ReadUint32L();
        if ( formatVer != KSnapshotFormatVersion )
            {
            // Wrong version, do not try to import data
            LOGGER_WRITE("CChangeFinder::ConstructL, Wrong format version -> no history");
            aHasHistory = EFalse;
            CleanupStack::PopAndDestroy( &readStream );
            TRACE_FUNC_EXIT;
            return;
            }
        
        // Read item count
        TInt count = readStream.ReadUint32L();
        // Read items
        for ( TInt i=0; i<count; i++ )
            {
            TSnapshotItem item;
            item.InternalizeL( readStream );
            iOldSnapshot->InsertIsqL( item, iKey );
            }

        CleanupStack::PopAndDestroy( &readStream );
        }
    else
        {
        LOGGER_WRITE("CChangeFinder::ConstructL, no sync history.");
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::CloseL
// Closes ChangeFinder object and writes snapshot to stream
// -----------------------------------------------------------------------------
void CChangeFinder::CloseL()
    {
    TRACE_FUNC_ENTRY;
    
    // Write to stream
    RWriteStream writeStream;
    
    TUid streamId;
    streamId.iUid = iStreamUid; 

    // Open write stream
    iSyncRelationship.OpenWriteStreamLC( writeStream, streamId );
    
    // Write used format version
    writeStream.WriteUint32L( KSnapshotFormatVersion );

    // Write item count
    TInt count(0);
    if ( iOldSnapshot )
        {
        count = iOldSnapshot->Count();
        }
    writeStream.WriteUint32L(count);
    
    // Write items
    for (TInt i=0; i<count; i++)
        {
        const TSnapshotItem& item = iOldSnapshot->At( i );
        item.ExternalizeL( writeStream );
        }

    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ResetL
// Resets synchronization history, all contetn is considered new after this call
// -----------------------------------------------------------------------------
void CChangeFinder::ResetL()
    {
    TRACE_FUNC_ENTRY;
    
    // Delete old change information
    if ( iOldSnapshot )
        {
        LOGGER_WRITE("iOldSnapshot->Reset()");
        iOldSnapshot->Reset();
        }
    
    // Write 'null' data to file, 
    // this removes change history from the file
    CloseL();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindChangedItemsL
// Compares snapshots, finds changed items
// -----------------------------------------------------------------------------
void CChangeFinder::FindChangedItemsL( CNSmlDataItemUidSet& aChangedUids )
    {   
    TRACE_FUNC_ENTRY;
    
    if ( !iCurrentSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::FindChangedItemsL leaved, no current snapshot." );
        User::Leave( KErrNotFound );
        }

    if ( !iOldSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::FindChangedItemsL leaved, no old snapshot." );
        User::Leave( KErrNotFound );
        }
        
    TInt index;
    TInt count = iCurrentSnapshot->Count();
    LOGGER_WRITE_1( "CChangeFinder::FindChangedItemsL count: %d", count );
    for ( TInt i=0; i < count; i++ )
        {
        const TSnapshotItem& currentItem = iCurrentSnapshot->At( i );
        // Find this entry from the old snapshot
        if ( iOldSnapshot->FindIsq( currentItem, iKey, index ) == KErrNone)
            {
            // This is the old item
            TSnapshotItem& oldItem = iOldSnapshot->At( index );
            // Compare hash to see whether this was changed
            if ( oldItem.Hash().Compare( currentItem.Hash() ) != 0
                    || oldItem.ParentId() != currentItem.ParentId() )
                {
                aChangedUids.AddItem( currentItem.ItemId() );
                //LOGGER_WRITE_1( "Item %d was changed.", currentItem.ItemId() );  
                }
            }
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindDeletedItemsL
// Compares snapshots, finds deleted items
// -----------------------------------------------------------------------------
void CChangeFinder::FindDeletedItemsL( CNSmlDataItemUidSet& aDeletedUids )
    {
    TRACE_FUNC_ENTRY;
    
    if ( !iOldSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::FindDeletedItemsL leaved, no old snapshot." );
        User::Leave( KErrNotFound );
        }
    
    TInt index;
    TInt count = iOldSnapshot->Count();
    for ( TInt i=0; i < count; i++ )
        {
        const TSnapshotItem& currentItem = iOldSnapshot->At( i );
        
        // If there's no current snapshot, this definately is deleted item
        if ( !iCurrentSnapshot )
            {
            aDeletedUids.AddItem( currentItem.ItemId() );
            //LOGGER_WRITE_1( "Item %d was deleted.", currentItem.ItemId() );
            }
        // It is also new if it doesn't exist int the current snapshot.
        else if ( iCurrentSnapshot->FindIsq( currentItem, iKey, index ) != KErrNone )
            {
            aDeletedUids.AddItem( currentItem.ItemId() );
            //LOGGER_WRITE_1( "Item %d was deleted.", currentItem.ItemId() );
            }       
        }
        
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindNewItemsL
// Compares snapshots, finds new items
// -----------------------------------------------------------------------------
void CChangeFinder::FindNewItemsL( CNSmlDataItemUidSet& aNewUids )
    {
    TRACE_FUNC_ENTRY;
    
    if ( !iCurrentSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::FindNewItemsL leaved, no current snapshot." );
        User::Leave( KErrNotFound );
        }

    TInt index;
    TInt count = iCurrentSnapshot->Count();
    for ( TInt i=0; i < count; i++ )
        {
        const TSnapshotItem& currentItem = iCurrentSnapshot->At( i );
        
        // If there's no old snapshot, all items are new
        if ( !iOldSnapshot )
            {
            aNewUids.AddItem( currentItem.ItemId() );
            //LOGGER_WRITE_1( "Item %d was new.", currentItem.ItemId() );
            }
        // It is also new if it doesn't exist int the old snapshot.
        else if ( iOldSnapshot->FindIsq( currentItem, iKey, index ) != KErrNone )
            {
            aNewUids.AddItem( currentItem.ItemId() );
            //LOGGER_WRITE_1( "Item %d was new.", currentItem.ItemId() );
            }       
        }
        
    TRACE_FUNC_EXIT;
    }


// -----------------------------------------------------------------------------
// CChangeFinder::ItemAddedL
// Adds item to snapshot, this item is no longer considered new
// -----------------------------------------------------------------------------
void CChangeFinder::ItemAddedL( const TSnapshotItem& aItem )
    {
    TRACE_FUNC_ENTRY;
    
    // Add this to old snapshot, if there's no old snapshot it must be created
    if ( !iOldSnapshot )
        {
        iOldSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
        }
        
    LOGGER_WRITE_1( "Adding item %d.", aItem.ItemId() );
    
    TRAPD( error, iOldSnapshot->InsertIsqL( aItem, iKey ) );
    if ( error == KErrAlreadyExists )
        {
        // It was already committed, no actions required
        LOGGER_WRITE( "iOldSnapshot->InsertIsqL leaved with KErrAlreadyExists" );
        }
    else if ( error != KErrNone )
        {
        LOGGER_WRITE_1( "iOldSnapshot->InsertIsqL leaved with %d.", error );
        User::Leave( error );
        }
    iOldSnapshot->Compress();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemDeletedL
// Removes item to snapshot, this item is no longer considered deleted
// -----------------------------------------------------------------------------
void CChangeFinder::ItemDeletedL( const TSnapshotItem& aItem )
    {
    TRACE_FUNC_ENTRY;
    
    LOGGER_WRITE_1( "deleting item %d.", aItem.ItemId() );
    
    if ( !iOldSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::ItemDeleted leaved, no old snapshot." );
        User::Leave( KErrNotFound );
        }

    // Delete item from the old snapshot
    TInt index;
    if ( iOldSnapshot->FindIsq( aItem, iKey, index ) == KErrNone )
        {
        iOldSnapshot->Delete( index );
        }
    else // Skip, there wasn't such entry
        {
        LOGGER_WRITE( "iOldSnapshot->FindIsq, item was not found." );
        }
    iOldSnapshot->Compress();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemUpdatedL
// Updates item to snapshot, this item is no longer considered changed
// -----------------------------------------------------------------------------
void CChangeFinder::ItemUpdatedL( const TSnapshotItem& aItem )
    {
    TRACE_FUNC_ENTRY;

    LOGGER_WRITE_1( "Updating item %d.", aItem.ItemId() );
    
    // There must be such entry in the snapshot after this
    // If there isn't old snapshot, we'll create it and add the item
    if ( !iOldSnapshot )
        {
        iOldSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
        ItemAddedL( aItem );
        }
    else
        {
        // Update item in the old snapshot
        TInt index;
        if ( iOldSnapshot->FindIsq( aItem, iKey, index ) == KErrNone )
            {
            TSnapshotItem& oldItem = iOldSnapshot->At( index );
            oldItem = aItem;
            }
        else 
            {
            // There was old snapshot but no such item. Let's add it
            ItemAddedL( aItem );
            }
        
        }
    iOldSnapshot->Compress();
    TRACE_FUNC_EXIT;
    }


// -----------------------------------------------------------------------------
// CChangeFinder::CommitChangesL
// Commits current changes to snapshot
// -----------------------------------------------------------------------------
void CChangeFinder::CommitChangesL()
    {
    TRACE_FUNC_ENTRY;
    
    if ( !iCurrentSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::CommitChangesL leaved, current snapshot missing." );
        User::Leave( KErrNotFound );
        }
        
    if ( !iOldSnapshot )
        {
        iOldSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
        }
        
    // Delete everything from the old snapshot
    iOldSnapshot->Reset();
    
    // Loop through all the items in current snapshot
    TInt count = iCurrentSnapshot->Count();
    
    // Copy everything from current to old snapshot
    for ( TInt i = 0; i < count; i++ )
        {
        // Commit it to the old array.
        iOldSnapshot->InsertIsqL( iCurrentSnapshot->At( i ), iKey );
        }
    TRACE_FUNC_EXIT;
    }


// -----------------------------------------------------------------------------
// CChangeFinder::CommitChangesL
// Commits current changes to snapshot, affects only a specified group of items
// -----------------------------------------------------------------------------
void CChangeFinder::CommitChangesL( const MSmlDataItemUidSet& aUids )
    {
    TRACE_FUNC_ENTRY;
    
    
    // This function commits changes from current snapshot to old snapshot
    // But commits only the entries in the parameter array
    if ( !iCurrentSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::CommitChangesL leaved, current snapshot missing." );
        User::Leave( KErrNotFound );
        }
        
    if ( !iOldSnapshot )
        {
        iOldSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
        }
    
    for ( TInt i = 0; i < aUids.ItemCount(); i++ )
        {
        TSmlDbItemUid itemId = aUids.ItemAt( i );
        TSnapshotItem temp( itemId );
        TInt indexOld( -1 );
        TInt indexNew( -1 );
        TInt err = iOldSnapshot->FindIsq( temp, iKey, indexOld);
        if ( !err )
            {
            // founded from old snapshot
            if ( !iCurrentSnapshot->FindIsq(temp, iKey, indexNew) )
                {
                // Replace
                iOldSnapshot->At( indexOld ) = iCurrentSnapshot->At( indexNew );
                }
            else
                {
                // not found from current snapshot, delete from old also.
                iOldSnapshot->Delete( indexOld );
                }
            }
        else
            {
            // not found from old snapshot, add it.
            if ( !iCurrentSnapshot->FindIsq( temp, iKey, indexNew ) )
                {
                iOldSnapshot->InsertIsqL( iCurrentSnapshot->At( indexNew ), iKey );
                }
            }
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::SetNewSnapshot
// Sets new snapshot (to be compared against), ChangeFinder takes ownership
// -----------------------------------------------------------------------------
void CChangeFinder::SetNewSnapshot( CSnapshotArray* aNewSnapshot )
    {
    TRACE_FUNC_ENTRY;
    
    // Delete existing snapshot
    delete iCurrentSnapshot;
    
    // Set submitted snapshot as active
    iCurrentSnapshot = aNewSnapshot;
    iCurrentSnapshot->Compress();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::DataStoreUid
// returns stored data store id number
// -----------------------------------------------------------------------------
TInt64 CChangeFinder::DataStoreUid() const
    {
    TRACE_FUNC;
    return iDataStoreUid;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::SetDataStoreUid
// Sets data store id number
// -----------------------------------------------------------------------------
void CChangeFinder::SetDataStoreUid( TInt64 aUid )
    {
    TRACE_FUNC;
    iDataStoreUid = aUid;
    }

