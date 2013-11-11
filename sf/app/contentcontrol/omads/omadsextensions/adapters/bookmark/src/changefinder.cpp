/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
iKey( aKey ),
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
    LOGGER_ENTERFN( "CChangeFinder::~CChangeFinder" );
    delete iOldSnapshot;
	delete iCurrentSnapshot;
    LOGGER_LEAVEFN( "CChangeFinder::~CChangeFinder" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ConstructL
// 2nd phase constructor for the class, reads snapshot from stream
// -----------------------------------------------------------------------------
void CChangeFinder::ConstructL( TBool& aHasHistory )
    {
    LOGGER_ENTERFN( "CChangeFinder::ConstructL" );
    iOldSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
	iCurrentSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
    LOGGER_WRITE("CChangeFinder::ConstructL, CSnapshotArray:s created");
    TUid uid = {iStreamUid};
    
    aHasHistory = iSyncRelationship.IsStreamPresentL( uid );
    
    if ( aHasHistory )
        {
        LOGGER_WRITE("CChangeFinder::ConstructL, HasHistory");
        // Open stream for reading
        RReadStream readStream;
        iSyncRelationship.OpenReadStreamLC( readStream, uid );
        
        // Read used format version
        TUint formatVer = readStream.ReadUint32L();
        if ( formatVer != KSnapshotFormatVersion )
            {
            // Wrong version, do not try to import data
            LOGGER_WRITE("CChangeFinder::ConstructL, Wrong format version -> no history");
            aHasHistory = EFalse;
            CleanupStack::PopAndDestroy( &readStream );
            LOGGER_LEAVEFN("CChangeFinder::ConstructL");
            return;
            }
        else
            {
            LOGGER_WRITE("CChangeFinder::ConstructL, format is OK");
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
        iOldSnapshot->Compress();
        
        LOGGER_WRITE_1("iOldSnapshot done, iOldSnapshot->Count: %d", iOldSnapshot->Count() );
        CleanupStack::PopAndDestroy( &readStream ); // readStream
        }
    else
        {
        LOGGER_WRITE("CChangeFinder::ConstructL, no sync history.");
        }
        
    LOGGER_LEAVEFN("CChangeFinder::ConstructL");
    }

// -----------------------------------------------------------------------------
// CChangeFinder::CloseL
// Closes ChangeFinder object and writes snapshot to stream
// -----------------------------------------------------------------------------
void CChangeFinder::CloseL()
    {
    LOGGER_ENTERFN( "CChangeFinder::CloseL" );
     
    RWriteStream writeStream;
    
    TUid uid = {iStreamUid};
    
    // Open write stream
    iSyncRelationship.OpenWriteStreamLC( writeStream, uid );
    
    // Write used format version
    writeStream.WriteUint32L( KSnapshotFormatVersion );
    
    // Write item count
    TInt count = iOldSnapshot->Count();
    writeStream.WriteUint32L( count );
    
    // Write items
    for ( TInt i = 0; i < count; i++ )
        {
        const TSnapshotItem& item = iOldSnapshot->At( i );
        item.ExternalizeL( writeStream );
        }
    
    writeStream.CommitL();
    CleanupStack::PopAndDestroy( &writeStream ); // writeStream
    
    LOGGER_LEAVEFN( "CChangeFinder::CloseL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ResetL
// Resets synchronization history, all contetn is considered new after this call
// -----------------------------------------------------------------------------
void CChangeFinder::ResetL()
    {
    LOGGER_ENTERFN( "CChangeFinder::ResetL" );
    if ( iOldSnapshot )
        {
        LOGGER_WRITE("iOldSnapshot->Reset()");
        iOldSnapshot->Reset();
        // Write 'null' data to file, 
        // this removes change history from the file
        CloseL();
        }
    LOGGER_LEAVEFN( "CChangeFinder::ResetL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindChangedItemsL
// Compares snapshots, finds changed items
// -----------------------------------------------------------------------------
void CChangeFinder::FindChangedItemsL( CNSmlDataItemUidSet& aChangedUids )
    {   
    LOGGER_ENTERFN( "CChangeFinder::FindChangedItemsL" );
    aChangedUids.Reset();
    
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
    LOGGER_WRITE_1( "CChangeFinder::FindChangedItemsL items on iCurrentSnapshot: %d", count );
    for ( TInt i=0; i < count; i++ )
        {
        const TSnapshotItem& currentItem = iCurrentSnapshot->At( i );
        
        // Find this entry from the old snapshot
        if ( iOldSnapshot->FindIsq( currentItem, iKey, index ) == KErrNone)
            {
            // This is the old item
            TSnapshotItem& oldItem = iOldSnapshot->At( index );
            if ( currentItem.Hash().Compare( oldItem.Hash() ) != 0 )
                {
                // add to list
				User::LeaveIfError( aChangedUids.AddItem( currentItem.ItemId() ) );
                }
            }
        }
    
    LOGGER_LEAVEFN( "CChangeFinder::FindChangedItemsL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindDeletedItemsL
// Compares snapshots, finds deleted items
// -----------------------------------------------------------------------------
void CChangeFinder::FindDeletedItemsL( CNSmlDataItemUidSet& aDeletedUids )
    {
    LOGGER_ENTERFN( "CChangeFinder::FindDeletedItemsL" );
    aDeletedUids.Reset();
    if ( !iOldSnapshot )
        {
        LOGGER_LEAVEFN( "CChangeFinder::FindDeletedItemsL leaved, no old snapshot." );
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
            User::LeaveIfError( aDeletedUids.AddItem( currentItem.ItemId() ) );
            LOGGER_WRITE_1( "Item %d was deleted.", currentItem.ItemId() );
            }
        // It is also new if it doesn't exist int the current snapshot.
        else if ( iCurrentSnapshot->FindIsq( currentItem, iKey, index ) != KErrNone )
            {
            User::LeaveIfError( aDeletedUids.AddItem( currentItem.ItemId() ) );
            LOGGER_WRITE_1( "Item %d was deleted.", currentItem.ItemId() );
            }       
        }
        
    LOGGER_LEAVEFN( "CChangeFinder::FindDeletedItemsL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindNewItemsL
// Compares snapshots, finds new items
// -----------------------------------------------------------------------------
void CChangeFinder::FindNewItemsL( CNSmlDataItemUidSet& aNewUids )
    {
    LOGGER_ENTERFN( "CChangeFinder::FindNewItemsL" );
    aNewUids.Reset();
    /*if ( !iCurrentSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::FindNewItemsL leaved, no current snapshot." );
        User::Leave( KErrNotFound );
        }*/

    TInt index;
    TInt count = iCurrentSnapshot->Count();
    LOGGER_WRITE_1( "iCurrentSnapshot->Count(): %d", count );
    LOGGER_WRITE_1( "iOldSnapshot->Count(): %d", iOldSnapshot->Count() );
    for ( TInt i=0; i < count; i++ )
        {
        const TSnapshotItem& currentItem = iCurrentSnapshot->At( i );
        
        // If there's no old snapshot, all items are new
        if ( !iOldSnapshot )
            {
            User::LeaveIfError( aNewUids.AddItem( currentItem.ItemId() ) );
            LOGGER_WRITE_1( "!iOldSnapshot, Item %d was new.", currentItem.ItemId() );
            }
        // It is also new if it doesn't exist int the old snapshot.
        else if ( iOldSnapshot->FindIsq( currentItem, iKey, index ) != KErrNone )
            {
            User::LeaveIfError( aNewUids.AddItem( currentItem.ItemId() ) );
            LOGGER_WRITE_1( "Item %d was new.", currentItem.ItemId() );
            }       
        }
        
    LOGGER_LEAVEFN( "CChangeFinder::FindNewItemsL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindMovedItemsL
// Compares snapshots, finds moved items
// -----------------------------------------------------------------------------
void CChangeFinder::FindMovedItemsL( CNSmlDataItemUidSet& aMovedUids )
    {
    LOGGER_ENTERFN( "CChangeFinder::FindMovedItemsL" );
    aMovedUids.Reset();
    if ( !iCurrentSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::FindMovedItemsL leaved, no current snapshot." );
        User::Leave( KErrNotFound );
        }
        
    if ( !iOldSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::FindMovedItemsL leaved, no old snapshot." );
        User::Leave( KErrNotFound );
        }
        
    TInt index;
    TInt count = iCurrentSnapshot->Count();
    for ( TInt i=0; i < count; i++ )
        {
        const TSnapshotItem& currentItem = iCurrentSnapshot->At( i );
        
        // Find this entry from the old snapshot
        if(iOldSnapshot->FindIsq( currentItem, iKey, index ) == KErrNone)
            {
            // This is the old item
            TSnapshotItem& oldItem = iOldSnapshot->At( index );
            
            // Report only moved items in which only parent id has been changed
            if ( oldItem.ParentId() != currentItem.ParentId()
                && currentItem.Hash().Compare( oldItem.Hash() ) == 0 )
                {
                User::LeaveIfError( aMovedUids.AddItem( currentItem.ItemId() ) );
                LOGGER_WRITE_1( "Item %d was moved.", currentItem.ItemId() );
                }
            }       
        }
    
    LOGGER_LEAVEFN( "CChangeFinder::FindMovedItemsL" );     
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemAddedL
// Adds item to snapshot, this item is no longer considered new
// -----------------------------------------------------------------------------
void CChangeFinder::ItemAddedL( const TSnapshotItem& aItem )
    {
    LOGGER_ENTERFN( "CChangeFinder::ItemAddedL" );
    
    // Add this to old snapshot, if there's no old snapshot it must be created
    if ( !iOldSnapshot )
        {
        iOldSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
        }
        
    LOGGER_WRITE_1( "Adding item %d.", aItem.ItemId() );
    
    iOldSnapshot->InsertIsqL( aItem, iKey );
    iOldSnapshot->Compress();
    
    LOGGER_LEAVEFN( "CChangeFinder::ItemAddedL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemDeletedL
// Removes item to snapshot, this item is no longer considered deleted
// -----------------------------------------------------------------------------
void CChangeFinder::ItemDeletedL( const TSnapshotItem& aItem )
    {
    LOGGER_ENTERFN( "CChangeFinder::ItemDeletedL" );
    
    LOGGER_WRITE_1( "deleting item %d.", aItem.ItemId() );
    
    if ( !iOldSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::ItemDeletedL leaved, no old snapshot." );
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
    
    LOGGER_LEAVEFN( "CChangeFinder::ItemDeletedL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemUpdatedL
// Updates item to snapshot, this item is no longer considered changed
// -----------------------------------------------------------------------------
void CChangeFinder::ItemUpdatedL( const TSnapshotItem& aItem )
    {
    LOGGER_ENTERFN( "CChangeFinder::ItemUpdatedL" );

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
            iOldSnapshot->At( index ) = aItem;
            }
        else 
            {
            // There was old snapshot but no such item. Let's add it
            ItemAddedL( aItem );
            }
        
        }
        
    LOGGER_LEAVEFN( "CChangeFinder::ItemUpdatedL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemMovedL
// Moves item within snapshot, this item is no longer considered moved
// -----------------------------------------------------------------------------
void CChangeFinder::ItemMovedL( const TSnapshotItem& aItem )
    {
    LOGGER_ENTERFN( "CChangeFinder::ItemMovedL" );
    
    LOGGER_WRITE_1( "Moving item %d.", aItem.ItemId() );
            
    // There must be such entry in the snapshot after this
    // If there isn't old snapshot, we'll create it and add the item
    if ( !iOldSnapshot )
        {
        iOldSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
        ItemAddedL( aItem );
        }
    else
        {
        // Update item's parent in the old snapshot
        TInt index;
        if ( iOldSnapshot->FindIsq( aItem, iKey, index ) == KErrNone )
            {
            iOldSnapshot->At(index) = aItem;
            }
        else 
            {
            // There was old snapshot but no such item. Let's add it
            ItemAddedL( aItem );
            }   
        }
    
    LOGGER_LEAVEFN("CChangeFinder::ItemMovedL");    
    }

// -----------------------------------------------------------------------------
// CChangeFinder::CommitChangesL
// Commits current changes to snapshot
// -----------------------------------------------------------------------------
void CChangeFinder::CommitChangesL()
    {
    LOGGER_ENTERFN( "CChangeFinder::CommitChangesL" );
    
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
    LOGGER_LEAVEFN( "CChangeFinder::CommitChangesL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::CommitChangesL
// Commits current changes to snapshot, affects only a specified group of items
// -----------------------------------------------------------------------------
void CChangeFinder::CommitChangesL( const MSmlDataItemUidSet& aUids )
    {
    LOGGER_ENTERFN( "CChangeFinder::CommitChangesL" );
    
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
		if ( !iOldSnapshot->FindIsq( temp, iKey, indexOld) )
			{
			if ( !iCurrentSnapshot->FindIsq(temp, iKey, indexNew) )
				{
				// Replace, moved or softdeleted
				iOldSnapshot->At( indexOld ) = iCurrentSnapshot->At( indexNew );
				}
			else
				{
				// Delete
				iOldSnapshot->Delete( indexOld );
				}
			}
		else
			{
			// Add
			if ( !iCurrentSnapshot->FindIsq( temp, iKey, indexNew ) )
			    {
			    iOldSnapshot->InsertIsqL( iCurrentSnapshot->At( indexNew ), iKey );
			    }
			}
		}
    LOGGER_LEAVEFN( "CChangeFinder::CommitChangesL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::SetNewSnapshot
// Sets new snapshot (to be compared against), ChangeFinder takes ownership
// -----------------------------------------------------------------------------
void CChangeFinder::SetNewSnapshot( CSnapshotArray* aNewSnapshot )
    {
    LOGGER_ENTERFN( "CChangeFinder::SetNewSnapshot" );
    
    // Delete existing snapshot
    delete iCurrentSnapshot;
    
    // Set submitted snapshot as active
    iCurrentSnapshot = aNewSnapshot;
    LOGGER_LEAVEFN( "CChangeFinder::SetNewSnapshot" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::DataStoreUid
// returns stored data store id number
// -----------------------------------------------------------------------------
TInt64 CChangeFinder::DataStoreUid() const
    {
    LOGGER_ENTERFN( "CChangeFinder::DataStoreUid" );
    LOGGER_LEAVEFN( "CChangeFinder::DataStoreUid" );
    return iDataStoreUid;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::SetDataStoreUid
// Sets data store id number
// -----------------------------------------------------------------------------
void CChangeFinder::SetDataStoreUid( TInt64 aUid )
    {
    LOGGER_ENTERFN( "CChangeFinder::SetDataStoreUid" );
    iDataStoreUid = aUid;
    LOGGER_LEAVEFN( "CChangeFinder::SetDataStoreUid" );
    }
