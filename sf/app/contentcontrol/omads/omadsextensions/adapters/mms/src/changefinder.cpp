/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
    TInt error;
    TRAP( error, CloseL() );
    if ( error != KErrNone )
        {
        LOGGER_WRITE( "CChangeFinder::~CChangeFinder, CloseL failed." );
        }
    
    delete iCurrentSnapshot;
    delete iOldSnapshot;
    
    LOGGER_LEAVEFN( "CChangeFinder::~CChangeFinder" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ConstructL
// 2nd phase constructor for the class, reads snapshot from stream
// -----------------------------------------------------------------------------
void CChangeFinder::ConstructL( TBool& aHasHistory )
    {
    LOGGER_ENTERFN( "CChangeFinder::ConstructL" );
        
    aHasHistory = EFalse;
    RReadStream readStream;
    
    TUid streamId;
    streamId.iUid = iStreamUid;
    
    aHasHistory = iSyncRelationship.IsStreamPresentL(streamId);
    
    if (aHasHistory)
        {
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
            CleanupStack::PopAndDestroy(); // readStream
            LOGGER_LEAVEFN("CChangeFinder::ConstructL");
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

        readStream.Close();
        readStream.Pop();
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
    writeStream.Close();
    writeStream.Pop();
    
    LOGGER_LEAVEFN( "CChangeFinder::CloseL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ResetL
// Resets synchronization history, all contetn is considered new after this call
// -----------------------------------------------------------------------------
void CChangeFinder::ResetL()
    {
    LOGGER_ENTERFN( "CChangeFinder::ResetL" );
    // Delete old change information
    if ( iOldSnapshot )
        {
        LOGGER_WRITE("iOldSnapshot->Reset()");
        iOldSnapshot->Reset();
        }
    
    // Write 'null' data to file, 
    // this removes change history from the file
    CloseL();
    
    LOGGER_LEAVEFN( "CChangeFinder::ResetL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindChangedItemsL
// Compares snapshots, finds changed items
// -----------------------------------------------------------------------------
void CChangeFinder::FindChangedItemsL( CNSmlDataItemUidSet& aChangedUids )
    {   
    LOGGER_ENTERFN( "CChangeFinder::FindChangedItemsL" );
    
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
    for ( TInt i=0; i < count; i++ )
        {
        const TSnapshotItem& currentItem = iCurrentSnapshot->At( i );
        // Find this entry from the old snapshot
        if ( iOldSnapshot->FindIsq( currentItem, iKey, index ) == KErrNone)
            {
            // This is the old item
            TSnapshotItem& oldItem = iOldSnapshot->At( index );
            // Compare times to see whether this was changed or item was read
            if ( ( oldItem.LastChangedDate() != currentItem.LastChangedDate() )
                || ( oldItem.Unread() && !currentItem.Unread() )
                || ( !oldItem.Unread() && currentItem.Unread() )
                || ( oldItem.FolderName().Compare( currentItem.FolderName() ) != 0 ) )
                {
                aChangedUids.AddItem( currentItem.ItemId() );
                LOGGER_WRITE_1( "Item %d was changed.", currentItem.ItemId() );  
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
            aDeletedUids.AddItem( currentItem.ItemId() );
            LOGGER_WRITE_1( "Item %d was deleted.", currentItem.ItemId() );
            }
        // It is also new if it doesn't exist int the current snapshot.
        else if ( iCurrentSnapshot->FindIsq( currentItem, iKey, index ) != KErrNone )
            {
            aDeletedUids.AddItem( currentItem.ItemId() );
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
            LOGGER_WRITE_1( "Item %d was new.", currentItem.ItemId() );
            }
        // It is also new if it doesn't exist int the old snapshot.
        else if ( iOldSnapshot->FindIsq( currentItem, iKey, index ) != KErrNone )
            {
            aNewUids.AddItem( currentItem.ItemId() );
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
                && oldItem.LastChangedDate() == currentItem.LastChangedDate()
                && !(oldItem.Unread() && !currentItem.Unread() )
                && !(!oldItem.Unread() && currentItem.Unread() ) )
                {
                aMovedUids.AddItem( currentItem.ItemId() );
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
        
    LOGGER_LEAVEFN( "CChangeFinder::ItemAddedL" );
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemDeletedL
// Removes item to snapshot, this item is no longer considered deleted
// -----------------------------------------------------------------------------
void CChangeFinder::ItemDeletedL( const TSnapshotItem& aItem )
    {
    LOGGER_ENTERFN( "CChangeFinder::ItemDeleted" );
    
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
    
    LOGGER_LEAVEFN( "CChangeFinder::ItemDeleted" );
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
            TSnapshotItem& oldItem = iOldSnapshot->At( index );
            oldItem = aItem;
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
            TSnapshotItem& oldItem = iOldSnapshot->At( index );
            oldItem.SetParentId( aItem.ParentId() );
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
    for ( TInt i=0; i < count; i++ )
        {   
        const TSnapshotItem& newItem = iCurrentSnapshot->At( i );

        // Commit it to the old array.
        TRAPD( error, iOldSnapshot->InsertIsqL( newItem, iKey ) );
        if ( error == KErrAlreadyExists )
            {
            // It was already committed, this is an internal error of change finder
            LOGGER_WRITE( "iOldSnapshot->InsertIsqL leaved with KErrAlreadyExists." );
            User::Leave( error );
            }
        else if ( error != KErrNone )
            {
            LOGGER_WRITE_1( "iOldSnapshot->InsertIsqL leaved with %d.", error );
            User::Leave( error );
            }
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
        
    // Use only these uid values
    TInt index;
    TInt count = aUids.ItemCount();
    
    for ( TInt i=0; i < count; i++ )
        {
        TSmlDbItemUid uid = aUids.ItemAt( i );
        TSnapshotItem item( uid );
        // Let's see if this uid exists in the current snapshot
        if ( iCurrentSnapshot->FindIsq( item, iKey, index ) == KErrNone )
            {
            // This is the new item
            const TSnapshotItem& newItem = iCurrentSnapshot->At( index );
            
            // Let's see if we can find the old one
            if ( iOldSnapshot->FindIsq( item, iKey, index ) == KErrNone )
                {
                // This is the old item
                TSnapshotItem& oldItem = iOldSnapshot->At( index );
                
                // Copy new item over the old one, this'll commit the change
                oldItem = newItem;
                }
            else // This entry was not found. It means that it is new one
                {
                // Commit it to the old array.
                ItemAddedL( newItem );
                }
            }
        else 
            {
            // This item was deleted from the current snapshot.
            TSnapshotItem toDelete( item );
            ItemDeletedL( toDelete );
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

// -----------------------------------------------------------------------------
// CChangeFinder::UpdatePartialL
// Returns ETrue if item can be updated partially (only status bits are changed)
// -----------------------------------------------------------------------------
TBool CChangeFinder::UpdatePartialL( TSmlDbItemUid& aUid )
    {
    LOGGER_ENTERFN( "CChangeFinder::UpdatePartialL" );
    
    if ( !iCurrentSnapshot )
        {
        LOGGER_WRITE( "CChangeFinder::UpdatePartialL leaved, current snapshot missing." );
        User::Leave( KErrNotFound );
        }

    if ( !iOldSnapshot )
        {
        LOGGER_LEAVEFN( "CChangeFinder::UpdatePartialL" );
        return EFalse;
        }
        
    TInt index;
    TSnapshotItem item( aUid );
    // Find from current snapshot, if not found -> deleted
    if ( iCurrentSnapshot->FindIsq( item, iKey, index ) != KErrNone )
        {
        LOGGER_LEAVEFN( "CChangeFinder::UpdatePartialL" );
        return EFalse;
        }   
        
    // Current item 
    const TSnapshotItem& currentItem = iCurrentSnapshot->At( index );
    
    // Old item, if not found -> added
    if ( iOldSnapshot->FindIsq( item, iKey, index ) != KErrNone )
        {
        LOGGER_LEAVEFN( "CChangeFinder::UpdatePartialL" );
        return EFalse;
        }
    else
        {
        // This is the old item
        TSnapshotItem& oldItem = iOldSnapshot->At( index );
        
        // Status bits must have been changed to allow partial update
        if ( oldItem.Unread() == currentItem.Unread() )
            {
            LOGGER_LEAVEFN( "CChangeFinder::UpdatePartialL" );
            return EFalse;
            }
            
        // Date should be same, otherwise 'just' changed item
        if ( oldItem.LastChangedDate() == currentItem.LastChangedDate() )
            {
            LOGGER_LEAVEFN( "CChangeFinder::UpdatePartialL" );
            return ETrue;
            }
        
        LOGGER_LEAVEFN( "CChangeFinder::UpdatePartialL" );
        return EFalse;
        }
    }
