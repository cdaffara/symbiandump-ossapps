/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Part of ContactsGroup Plug In Adapter
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
CChangeFinder::CChangeFinder( MSmlSyncRelationship& aSyncRelationship,
        TKeyArrayFix aKey, TInt aStreamUid ) :
iSyncRelationship( aSyncRelationship ),
iKey( aKey ),
iStreamUid( aStreamUid )
    {
    }

// -----------------------------------------------------------------------------
// CChangeFinder::~CChangeFinder
// Destructor for the class, closes the ChangeFinder and writes snapshot to stream
// -----------------------------------------------------------------------------
CChangeFinder::~CChangeFinder()
    {
    TRACE_FUNC_ENTRY;
    delete iOldSnapshot;
	delete iCurrentSnapshot;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ConstructL
// 2nd phase constructor for the class, reads snapshot from stream
// -----------------------------------------------------------------------------
void CChangeFinder::ConstructL( TBool& aHasHistory )
    {
    TRACE_FUNC_ENTRY;
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
            TRACE_FUNC_EXIT;
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
        
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::CloseL
// Closes ChangeFinder object and writes snapshot to stream
// -----------------------------------------------------------------------------
void CChangeFinder::CloseL()
    {
    TRACE_FUNC_ENTRY;
     
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
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ResetL
// Resets synchronization history, all contetn is considered new after this call
// -----------------------------------------------------------------------------
void CChangeFinder::ResetL()
    {
    TRACE_FUNC_ENTRY;
    iOldSnapshot->Reset();
    
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
    aChangedUids.Reset();
    
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
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindDeletedItemsL
// Compares snapshots, finds deleted items
// -----------------------------------------------------------------------------
void CChangeFinder::FindDeletedItemsL( CNSmlDataItemUidSet& aDeletedUids )
    {
    TRACE_FUNC_ENTRY;
    aDeletedUids.Reset();
        
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
        
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::FindNewItemsL
// Compares snapshots, finds new items
// -----------------------------------------------------------------------------
void CChangeFinder::FindNewItemsL( CNSmlDataItemUidSet& aNewUids )
    {
    TRACE_FUNC_ENTRY;
    aNewUids.Reset();
    
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
        
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemAddedL
// Adds item to snapshot, this item is no longer considered new
// -----------------------------------------------------------------------------
void CChangeFinder::ItemAddedL( const TSnapshotItem& aItem )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "Adding item %d.", aItem.ItemId() );
    
    iOldSnapshot->InsertIsqL( aItem, iKey );
    iOldSnapshot->Compress();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::ItemDeleted
// Removes item from snapshot, this item is no longer considered deleted
// -----------------------------------------------------------------------------
void CChangeFinder::ItemDeleted( const TSnapshotItem& aItem )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "deleting item %d.", aItem.ItemId() );
    
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
    
    // Update item in the old snapshot
    TInt index;
    if ( iOldSnapshot->FindIsq( aItem, iKey, index ) == KErrNone )
        {
        iOldSnapshot->At( index ) = aItem;
        }
    else 
        {
        // There was no such item. Let's add it
        ItemAddedL( aItem );
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::CommitChangesL
// Commits current changes to snapshot
// -----------------------------------------------------------------------------
void CChangeFinder::CommitChangesL()
    {
    TRACE_FUNC_ENTRY;
     
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
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CChangeFinder::SetNewSnapshot
// Sets new snapshot (to be compared against), ChangeFinder takes ownership
// -----------------------------------------------------------------------------
void CChangeFinder::SetNewSnapshot( CSnapshotArray* aNewSnapshot )
    {
    TRACE_FUNC;
    
    // Delete existing snapshot
    delete iCurrentSnapshot;
    
    // Set submitted snapshot as active
    iCurrentSnapshot = aNewSnapshot;
    TRACE_FUNC_EXIT;
    }
