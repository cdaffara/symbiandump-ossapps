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


#include "mediadsstore.h"
#include <mpxmessagegeneraldefs.h>
#include <mpxcollectionmessage.h>

#include "mediadsproviderdefs.h"
#include "mediamanager.h"
#include "cmdemanager.h"
#include "playlistitem.h"
#include "omadsfolderobject.h"
#include "snapshotitem.h"
#include "logger.h"

_LIT8( KFolderMimeType, "application/vnd.omads-folder+xml" );
_LIT8( KFolderMimeVersion, "1.2" );

_LIT8( KPlaylistMimeType, "audio/x-mpegurl");
_LIT8( KPlaylistMimeVersion, "1.0");

_LIT8( KSongMimeType, "application/x-song");
_LIT8( KSongMimeVersion, "1.0");


const TInt KDataBufferSize = 1024;

const TInt KAlbumsOffSet = 0x0000f000;

// -----------------------------------------------------------------------------
// CMediaDsDataStore::CMediaDsDataStore
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CMediaDsDataStore::CMediaDsDataStore( RFs& aFs):
    iKey(TKeyArrayFix(_FOFF(TSnapshotItem, ItemId()), ECmpTInt)),
    iFs( aFs )
    { 
    TRACE_FUNC;
    }
  
// -----------------------------------------------------------------------------
// CMediaDsDataStore::ConstructL
// Symbian 2nd phase constructor, can leave.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    
    // Item UID sets, used to transfer change info
    iNewItems = new (ELeave) CNSmlDataItemUidSet;
    iDeletedItems = new (ELeave) CNSmlDataItemUidSet;
    iUpdatedItems = new (ELeave) CNSmlDataItemUidSet;
    iMovedItems = new (ELeave) CNSmlDataItemUidSet;
    iSoftDeletedItems = new (ELeave) CNSmlDataItemUidSet;
    
    iHasher = CMD5::NewL();
    
    iMediaManager = CMediaManager::NewL( iFs, this, iKey, *iHasher );
    
    iMdEManager = CMdEManager::NewL( *this );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CMediaDsDataStore* CMediaDsDataStore::NewLC( RFs& aFs)
    {
    TRACE_FUNC_ENTRY;
    CMediaDsDataStore* self = new (ELeave) CMediaDsDataStore( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    TRACE_FUNC_EXIT;
    return self;    
    }

    
// -----------------------------------------------------------------------------
// CMediaDsDataStore::~CMediaDsDataStore
// Destructor
// -----------------------------------------------------------------------------
CMediaDsDataStore::~CMediaDsDataStore()
    {
    TRACE_FUNC_ENTRY;
    
    delete iDataBuffer;
    
    delete iChangeFinder;
    
    delete iNewItems;
    delete iDeletedItems;
    delete iUpdatedItems;
    delete iMovedItems;
    delete iSoftDeletedItems;
    
    delete iMediaManager;
    LOGGER_WRITE("iMediaManager deleted");
    
    delete iMdEManager;
    LOGGER_WRITE("iMdEManager deleted");
    
    delete iHasher;
    LOGGER_WRITE("iHasher deleted");
    // normally iSnapshot is NULL, but if error has occured we need to delete it.
    delete iSnapshot;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoOpenL
// Opens database. This operation is performed SYNCHRONOUSLY
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoOpenL( const TDesC& aStoreName,
    MSmlSyncRelationship& aContext, TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    if ( iCurrentState != EClosed )
        {
        User::RequestComplete( iCallerStatus, KErrInUse );
        LOGGER_WRITE("CMmsDataStore::DoOpenL failed with KErrInUse.");
        return;
        }
    
    // Create ChangeFinder object
    if ( iChangeFinder )
        {
        delete iChangeFinder;
        iChangeFinder = NULL;
        }
    iChangeFinder = CChangeFinder::NewL( aContext, iKey, iHasHistory, KMediaDataProviderImplUid );
    
    if ( aStoreName.CompareF( KMediaDsRefreshDbName ) == 0 )
        {
        LOGGER_WRITE("Refresh library and open database");
        // just kick off scanner, don't wait ready status
        // Async. func. HandleCollectionMessage is called when ready
        iMediaManager->ScanL();
        
        }
    else if ( aStoreName.CompareF( KMediaDsDbName ) == 0 )
        {
        LOGGER_WRITE("Open database");
        
        if ( !iHasHistory )
            {
            LOGGER_WRITE("No history, scan library");
            // just kick off scanner, don't wait ready status
            // Async. func. HandleCollectionMessage is called when ready
            iMediaManager->ScanL();
            }
        
        }
    else
        {
        LOGGER_WRITE("Unknown database");
        User::Leave( KErrNotSupported );
        }
    
    // Set current snapshot, this will be compared against the old one   
    RegisterSnapshotL();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCancelRequest
// Not supported, does nothing.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoCancelRequest()
    {
    TRACE_FUNC_ENTRY;
    if ( iCurrentState == EOpening )
        {
        LOGGER_WRITE("Cancel Open command");
        iMediaManager->Cancel();
        iMdEManager->Cancel();
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoStoreName
// Returns the name of the DataStore
// -----------------------------------------------------------------------------
const TDesC& CMediaDsDataStore::DoStoreName() const
    {
    TRACE_FUNC;
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoBeginTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoBeginTransactionL()
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCommitTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoCommitTransactionL( TRequestStatus& /*aStatus*/ )
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoRevertTransaction
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoRevertTransaction( TRequestStatus& aStatus )
    {
    TRACE_FUNC;
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoBeginBatchL
// Batching is not supported.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoBeginBatchL()
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCommitBatchL
// Batching is not supported
// -----------------------------------------------------------------------------
//
void CMediaDsDataStore::DoCommitBatchL( RArray<TInt>& /*aResultArray*/, TRequestStatus& /*aStatus*/ )
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCancelBatch
// Batching is not supported
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoCancelBatch()
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoSetRemoteStoreFormatL
// Not supported
// -----------------------------------------------------------------------------
//
void CMediaDsDataStore::DoSetRemoteStoreFormatL( const CSmlDataStoreFormat& /*aServerDataStoreFormat*/ )
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoSetRemoteMaxObjectSize
// Not supported
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoSetRemoteMaxObjectSize( TInt /*aServerMaxObjectSize*/ )
    {
    TRACE_FUNC;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoMaxObjectSize
// Reads the maximum MMS Message size from the central repository
// -----------------------------------------------------------------------------
TInt CMediaDsDataStore::DoMaxObjectSize() const
    {
    TRACE_FUNC;
    return 0;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoOpenItemL
// Opens item in the DataStore, reads it (either completely or partially) 
// to the temporary buffer where it can be later read to the remote database.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoOpenItemL( TSmlDbItemUid aUid, TBool& /*aFieldChange*/, 
    TInt& aSize, TSmlDbItemUid& aParent, TDes8& aMimeType, 
    TDes8& aMimeVer, TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;

    LOGGER_WRITE_1( "Opening item %d.", aUid );
    
    // Store these for later use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iReadPosition=0;
    
    // Check that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "CMediaDsDataStore::DoOpenItemL, invalid state %d.", iCurrentState );
        User::RequestComplete( iCallerStatus, KErrNotReady );
        return;
        }
    
    if ( !iDataBuffer )
        {
        iDataBuffer = CBufFlat::NewL( KDataBufferSize );
        }
    iDataBuffer->Reset();

    
    if ( aUid <= KLastFolderId )
        {
        COMADSFolderObject* folderObject = COMADSFolderObject::NewLC();
        
        switch ( aUid )
            {
            case KAllSongsFolderId:
                folderObject->SetName( KAllSongs );
                break;
            case KPlaylistsFolderId:
                folderObject->SetName( KPlaylists );
                break;
            case KAlbumsFolderId:
                folderObject->SetName( KAlbums );
                break;
            default:
                User::Leave( KErrNotFound );
                break;
            }
        folderObject->ExportFolderXmlL( *iDataBuffer );
        CleanupStack::PopAndDestroy( folderObject );
        aMimeType.Copy( KFolderMimeType );
        aMimeVer.Copy( KFolderMimeVersion );
        aParent = KErrNotFound;
        }
    else
        {
        CSongItem* song = CSongItem::NewLC();
        LOGGER_WRITE("Try to read song");
        TRAPD( err, iMediaManager->GetSongL( aUid, *song ));
        if ( !err )
            {
            RBufWriteStream stream( *iDataBuffer );
            CleanupClosePushL( stream );
            song->ExportL( stream );
            CleanupStack::PopAndDestroy( &stream );
            CleanupStack::PopAndDestroy( song );
            
            aMimeType.Copy( KSongMimeType );
            aMimeVer.Copy( KSongMimeVersion );
            aParent = KAllSongsFolderId;
            }
        else // could not read song, maybe it is playlist
            {
            CleanupStack::PopAndDestroy( song );
            CPlaylistItem* playList = CPlaylistItem::NewLC();
            LOGGER_WRITE("Try to read playlist");
            TRAP(err, iMediaManager->GetPlayListL( aUid, *playList ));
            if ( !err )
                {
                RBufWriteStream stream( *iDataBuffer );
                CleanupClosePushL( stream );
                
                playList->ExportL( stream );
                CleanupStack::PopAndDestroy( &stream );
                CleanupStack::PopAndDestroy( playList );
                
                aMimeType.Copy( KPlaylistMimeType );
                aMimeVer.Copy( KPlaylistMimeVersion );
                aParent = KPlaylistsFolderId;
                }
            else
                {
                CleanupStack::PopAndDestroy( playList );
                LOGGER_WRITE("Try to read album");
                // Could not read song, maybe it's a Album
                const CPlaylistItem& album = iMdEManager->AlbumL( MapSyncIdToAlbumId(aUid) );
                RBufWriteStream stream( *iDataBuffer );
                CleanupClosePushL( stream );
                
                album.ExportL( stream );
                CleanupStack::PopAndDestroy( &stream );
                
                aMimeType.Copy( KPlaylistMimeType );
                aMimeVer.Copy( KPlaylistMimeVersion );
                aParent = KAlbumsFolderId;
                }
            }
        
        }
    
    aSize = iDataBuffer->Size();
    
    LOGGER_WRITE_1("aSize: %d", aSize);
    
    // Signal we're complete
    User::RequestComplete( iCallerStatus, KErrNone ); 

    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCreateItemL
// Create new item to the message store.
// Return the id number of the newly created item
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoCreateItemL( TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent, 
    const TDesC8& aMimeType, const TDesC8& /*aMimeVer*/, TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1( "Parent folder: %d.", aParent );
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "Invalid state %d.", iCurrentState );
        }
    
    iWrittenDataLength = 0;
    iCreatedUid = &aUid;
    LOGGER_WRITE8_1( "aMimeType: %S", &aMimeType );
    
    if ( aMimeType.Compare( KPlaylistMimeType() ) == 0 )
        {
        LOGGER_WRITE("Add Playlist");
        iCurrentState = ECreatePlaylist;
        }
    else if ( aMimeType.Compare( KSongMimeType() ) == 0 )
        {
        LOGGER_WRITE("Add Song not supported");
        User::Leave( KErrNotSupported );
        }
    else
        {
        User::RequestComplete( iCallerStatus, KErrNotSupported );
        LOGGER_WRITE("Bad MIME type");
        return;
        }
    
    if ( iDataBuffer )
        {
        iDataBuffer->ResizeL( aSize );
        }
    else
        {
        iDataBuffer = CBufFlat::NewL( KDataBufferSize );
        iDataBuffer->ResizeL( aSize );
        }
    LOGGER_WRITE_1("iDataBuffer->Size: %d", iDataBuffer->Size());
    
    LOGGER_WRITE_1("aSize: %d", aSize);
    iParentId = aParent;
    
    // Signal we're complete
    User::RequestComplete( iCallerStatus, KErrNone );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoReplaceItemL
// Begin the replace operation, ensure that the item really exists
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoReplaceItemL( TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, 
    TBool /*aFieldChange*/, TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("Replacing item %d.", aUid);
    LOGGER_WRITE_1("Parent folder: %d.", aParent);
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1("Invalid state %d.", iCurrentState);
        }
    
    iCurrentState = EReplace;
    
    iParentId = aParent;
    iCurrentId = aUid;
    
    if ( iDataBuffer )
        {
        iDataBuffer->ResizeL( aSize );
        }
    else
        {
        iDataBuffer = CBufFlat::NewL( KDataBufferSize );
        iDataBuffer->ResizeL( aSize );
        }
    
    // Signal we're complete
    User::RequestComplete( iCallerStatus, KErrNone );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoReadItemL
// Read specified amount of data from the temporary buffer
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoReadItemL( TDes8& aBuffer )
    {
    TRACE_FUNC_ENTRY;
    // Thiw is how much data there is left in the buffer    
    TInt left = iDataBuffer->Size() - iReadPosition;
    
    if ( left > 0 )
        {
        // This is how much there's space in the destination buffer
        TInt destSize = aBuffer.MaxSize();
        
        // This is how much we can read
        TInt toRead = destSize < left ? destSize : left;
        
        // Read the data from the buffer, then update the position      
        iDataBuffer->Read( iReadPosition, aBuffer, toRead );
        iReadPosition += toRead;
        }
    else
        {
        LOGGER_WRITE( "No data to read" );
        User::Leave( KErrEof );
        }   
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoWriteItemL
// Write specified amount of data to the temporary buffer
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoWriteItemL( const TDesC8& aData )
    {
    TRACE_FUNC_ENTRY;
    
    iDataBuffer->Write( iWrittenDataLength, aData );
    iWrittenDataLength += aData.Size();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCommitItemL
// Commits item from temporary buffer to the message store
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoCommitItemL( TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    
    // Store some variables
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    iDataBuffer->Compress();
    
    // Read the playlist item
    CPlaylistItem* newItem = CPlaylistItem::NewLC();
    
    const TUint8* ptr8 = iDataBuffer->Ptr(0).Ptr();
    const TUint16* ptr16 = reinterpret_cast<const TUint16*>( ptr8 );
    TPtrC dataPtr;
    dataPtr.Set( ptr16, iDataBuffer->Size()/2 );
    newItem->ImportL( dataPtr );
    
    iDataBuffer->Reset();
    
#ifdef _DEBUG
    LOGGER_WRITE_1("Name: %S", &newItem->Title());
    LOGGER_WRITE_1("Items count: %d", newItem->ItemCount());
    for ( TInt i=0; i<newItem->ItemCount(); i++ )
        {
        TPtrC16 ptr = newItem->ItemAt(i);
        LOGGER_WRITE_1("item: %S", &ptr);
        }
#endif
    
    
    TInt error(KErrNone);
    if ( iCurrentState == ECreatePlaylist )
        {
        if ( iParentId == KPlaylistsFolderId )
            {
            iMediaManager->CreateNewPlaylistL( *iCreatedUid, *newItem );
            }
        else if ( iParentId == KAlbumsFolderId )
            {
            iMdEManager->CreateAlbumL( *newItem );
            *iCreatedUid = MapAlbumIdToSyncId( newItem->Id() );
            }
        else
            {
            LOGGER_WRITE("Not supported");
            User::Leave( KErrNotSupported);
            }
        
        // Inform ChangeFinder of the added item
        TSnapshotItem snapshotItem( *iCreatedUid );
        snapshotItem.SetParentId( iParentId );
        snapshotItem.SetItemHash( *newItem, *iHasher );
        iChangeFinder->ItemAddedL( snapshotItem );
        
        }
    else if ( iCurrentState == EReplace )
        {
        if ( iParentId == KPlaylistsFolderId )
            {
            iMediaManager->ReplacePlaylistL( iCurrentId, *newItem );
            }
        else if ( iParentId == KAlbumsFolderId )
            {
            iItemInReplacement.SetItemId( iCurrentId );
            iItemInReplacement.SetParentId( iParentId );
            iItemInReplacement.SetItemHash( *newItem, *iHasher );
            // Async function, calls AlbumReplaced when completed
            iMdEManager->ReplaceAlbumL( MapSyncIdToAlbumId(iCurrentId), *newItem );
            
            CleanupStack::PopAndDestroy( newItem );
            return;
            }
        else
            {
            LOGGER_WRITE("Not supported");
            User::Leave( KErrNotSupported);
            }
        
        // Inform ChangeFinder of the replaced item
        TSnapshotItem snapshotItem( iCurrentId );
        snapshotItem.SetParentId( iParentId );
        snapshotItem.SetItemHash( *newItem, *iHasher );
        iChangeFinder->ItemUpdatedL( snapshotItem );
        }
    else
        {
        LOGGER_WRITE_1("Wrong state: %d", iCurrentState);
        User::Leave( KErrNotSupported );
        }
    CleanupStack::PopAndDestroy( newItem );
    
    
    
    LOGGER_WRITE_1("error: %d", error);
    // We'll be waiting for next event, signal we're done
    iCurrentState = EOpenAndWaiting;
    User::RequestComplete( iCallerStatus, error );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCloseItem
// Closes open item in the data store
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoCloseItem()
    {
    TRACE_FUNC_ENTRY;
    if ( iDataBuffer )
        {
        iDataBuffer->Reset();
        }
    iCurrentState = EOpenAndWaiting;
    iReadPosition = 0;
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoMoveItemL
// Moves item from one folder to another in the message store
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoMoveItemL( TSmlDbItemUid /*aUid*/,
    TSmlDbItemUid /*aNewParent*/, TRequestStatus& /*aStatus*/ )
    {
    TRACE_FUNC;
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoDeleteItemL
// Removes item from the message store
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus  )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("Deleting item %d.", aUid);
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    TInt error(KErrNone);
    
    // Check that we're in proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoDeleteItemL, invalid state %d.", iCurrentState);        
        }
    
    TRAP( error,  iMediaManager->DeletePlaylistL( aUid ) );
    if ( error )
        {
        error = KErrNone;
        iMdEManager->DeleteAlbumL( MapSyncIdToAlbumId( aUid ) );
        }
    
    if ( !error )
        {
        // Inform ChangeFinder of the deleted item
        TSnapshotItem snapshotItem( aUid );
        iChangeFinder->ItemDeletedL( snapshotItem );
        }
    
    LOGGER_WRITE_1("complete error: %d", error);
    // Signal we're done
    User::RequestComplete( iCallerStatus, error );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoSoftDeleteItemL
// Soft delete isn't supported.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoSoftDeleteItemL( TSmlDbItemUid /*aUid*/, TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;

    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoDeleteAllItemsL
// Deletes all items in the standard folders of message store
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoDeleteAllItemsL( TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    
    // Store some variables for further use 
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoHasSyncHistory
// This method returns ETrue if Data Store has history information. 
// Slow-sync will be used if Data Store does not have history information.
// -----------------------------------------------------------------------------
TBool CMediaDsDataStore::DoHasSyncHistory() const
    {
    TRACE_FUNC_RET( (TInt)iHasHistory );      
    
    // iHasHistory is initialized in DoOpenL method
    return iHasHistory;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoAddedItems
// This method returns UIDs of added items. Those items are added after previous
// synchronization with current synchronization relationship. 
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMediaDsDataStore::DoAddedItems() const
    {
    TRACE_FUNC;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1("CMmsDataStore::DoAddedItems, invalid state %d.", iCurrentState);
        }
    
    TInt error(KErrNone);

    // Clear new-items array
    iNewItems->Reset();

    // Set current snapshot, this will be compared against the old one      
    // Search for new items
    TRAP( error, iChangeFinder->FindNewItemsL(*iNewItems) )
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1("CMmsDataStore::DoAddedItems, iChangeFinder->FindNewItemsL leaved with %d.", error);
        }
    
    LOGGER_WRITE_1("New item count: %d.", iNewItems->ItemCount());
    
    return *iNewItems;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoDeletedItems
//
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMediaDsDataStore::DoDeletedItems() const
    {
    TRACE_FUNC;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1("CMmsDataStore::DoDeletedItems, invalid state %d.", iCurrentState);
        }
    
    TInt error(KErrNone);
    
    // Clear deleted-items array
    iDeletedItems->Reset();
    
    // Search for deleted items
    TRAP( error, iChangeFinder->FindDeletedItemsL( *iDeletedItems ) );
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1("CMmsDataStore::DoDeletedItems, iChangeFinder->FindDeletedItemsL leaved with %d.", error);
        }           
    
    LOGGER_WRITE_1("Deleted item count: %d.", iDeletedItems->ItemCount());
    return *iDeletedItems;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoSoftDeletedItems
// Not directly supported, equals to "hard" delete
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMediaDsDataStore::DoSoftDeletedItems() const
    {
    TRACE_FUNC;

    iSoftDeletedItems->Reset();
    return *iSoftDeletedItems;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoModifiedItems
// Finds all modified items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMediaDsDataStore::DoModifiedItems() const
    {
    TRACE_FUNC;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1("CMmsDataStore::DoModifiedItems, invalid state %d.", iCurrentState);
        }
    
    TInt error(KErrNone);
    
    // Clear updated-items array
    iUpdatedItems->Reset();
    
    // Search for updated items
    TRAP( error, iChangeFinder->FindChangedItemsL( *iUpdatedItems ) )
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1("CMmsDataStore::DoModifiedItems, iChangeFinder->FindChangedItemsL leaved with %d.", error);
        }
    
    LOGGER_WRITE_1("Modified item count: %d.", iUpdatedItems->ItemCount()); 
    return *iUpdatedItems;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoMovedItems
// Finds all moved items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMediaDsDataStore::DoMovedItems() const
    {
    TRACE_FUNC;
    // return empty array
    return *iMovedItems;    
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoResetChangeInfoL
// Resets change history in the data store. All content is considered
// new in the data store point of view.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoResetChangeInfoL( TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoResetChangeInfoL, invalid state %d.", iCurrentState);
        }   
            
    // Reset change info in ChangeFinder
    iChangeFinder->ResetL();
    iHasHistory = EFalse;
    
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone ); 
    
    TRACE_FUNC_EXIT;
    }
        
// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCommitChangeInfoL
// Commits change info. These items are no longer reported, when change
// information is being queried.
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus, const MSmlDataItemUidSet& aItems )
    {
    TRACE_FUNC_ENTRY;
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState);
        }

    // Notify ChangeFinder
    iChangeFinder->CommitChangesL(aItems);
    iHasHistory = ETrue;
        
    // Signal we're done
    User::RequestComplete(iCallerStatus, KErrNone);
        
    TRACE_FUNC_EXIT;
    }
        
    
// -----------------------------------------------------------------------------
// CMediaDsDataStore::DoCommitChangeInfoL
// Commits change info. There is no more nothing to report when change
// information is being queried. 
// -----------------------------------------------------------------------------
void CMediaDsDataStore::DoCommitChangeInfoL(TRequestStatus& aStatus)
    {
    TRACE_FUNC_ENTRY;
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState);
        }
    
    // Notify ChangeFinder
    iChangeFinder->CommitChangesL();
    iHasHistory = ETrue;
        
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );
    
    TRACE_FUNC_EXIT;
    }


void CMediaDsDataStore::HandleCollectionMessage(CMPXMessage* /*aMsg*/, TInt /*aErr*/) 
    {
    TRACE_FUNC;
    }

void CMediaDsDataStore::HandleOpenL(const CMPXMedia& /*aEntries*/,
        TInt /*aIndex*/,TBool /*aComplete*/,TInt /*aError*/)
    {
    TRACE_FUNC;
    }

void CMediaDsDataStore::HandleOpenL(const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/)
    {
    TRACE_FUNC;
    }
    
void CMediaDsDataStore::HandleCommandComplete(CMPXCommand* /*aCommandResult*/, 
         TInt /*aError*/)
    {
    TRACE_FUNC;
    }

void CMediaDsDataStore::HandleCollectionMediaL(const CMPXMedia& /*aMedia*/, 
        TInt /*aError*/)
    {
    TRACE_FUNC;
    }

void CMediaDsDataStore::AlbumsReaded( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    iMdEManagerReady = ETrue;
    LOGGER_WRITE_1("aError: %d", aError );
    
    if ( aError )
        {
        iError = aError;
        }
    
    FinalizeOpenStore();
    
    TRACE_FUNC_EXIT;
    }

void CMediaDsDataStore::AlbumReplaced( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aError: %d", aError);
    if ( !aError )
        {
        // Inform ChangeFinder of the replaced item
        iChangeFinder->ItemUpdatedL( iItemInReplacement );
        }
    
    iCurrentState = EOpenAndWaiting;
    User::RequestComplete( iCallerStatus, aError );
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::RegisterSnapshotL
// Sets Changefinder to compare against current message store content
// -----------------------------------------------------------------------------
void CMediaDsDataStore::RegisterSnapshotL()
    {
    TRACE_FUNC_ENTRY;
    if ( iSnapshot )
        {
        delete iSnapshot;
        iSnapshot = NULL;
        }
    iSnapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
    RegisterFoldersL( *iSnapshot );
    iMediaManager->SetSnapshot( iSnapshot );
    
    iCurrentState = EOpening;
    TInt err = iMediaManager->RegisterAllPlayLists();
    if ( err )
        {
        LOGGER_WRITE_1("Could not start, err: %d", err);
        iCurrentState = EClosed;
        User::RequestComplete( iCallerStatus, err );
        }
    else
        {
        iMdEManager->GetAlbumsL();
        }

    
    TRACE_FUNC_EXIT;
    }

void CMediaDsDataStore::RegisterAllPlayListsCompleted( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    if ( !aError )
        {
        TInt err = iMediaManager->RegisterAllSongs();
        if ( err )
            {
            LOGGER_WRITE_1("Could not start, err: %d", err);
            iMediaManagerReady = ETrue;
            iError = err;
            }
        }
    else
        {
        // Error happened or cancelled.
        iMediaManagerReady = ETrue;
        iError = aError;
        }
    
    FinalizeOpenStore();
    TRACE_FUNC_EXIT;
    }

void CMediaDsDataStore::RegisterAllSongsCompleted( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    iMediaManagerReady = ETrue;
    if ( aError )
        {
        // Error happened or cancelled, save error code
        iError = aError;
        }
    
    FinalizeOpenStore();
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CMediaDsDataStore::RegisterFoldersL
// 
// -----------------------------------------------------------------------------
void CMediaDsDataStore::RegisterFoldersL( CSnapshotArray& aItemArray )
    {
    TRACE_FUNC_ENTRY;
    
    TKeyArrayFix key( iKey );
    TSnapshotItem folder;
    folder.SetItemId( KAllSongsFolderId );
    folder.SetItemHash( KAllSongs, *iHasher );
    aItemArray.InsertIsqL( folder, key );
    
    folder.SetItemId( KPlaylistsFolderId );
    folder.SetItemHash( KPlaylists, *iHasher );
    aItemArray.InsertIsqL( folder, key );
    
    folder.SetItemId( KAlbumsFolderId );
    folder.SetItemHash( KAlbums, *iHasher );
    aItemArray.InsertIsqL( folder, key );
    TRACE_FUNC_EXIT;
    }

void CMediaDsDataStore::FinalizeOpenStore()
    {
    TRACE_FUNC_ENTRY;
    // Signal client if all ready
    if ( iMediaManagerReady && iMdEManagerReady )
        {
        if ( !iError )
            {
            RPointerArray<CPlaylistItem> albums = iMdEManager->AlbumsArray();
            TKeyArrayFix key( iKey );
            LOGGER_WRITE("Album snapshots:")
            for (TInt i=0; i<albums.Count(); i++)
                {
                TSnapshotItem playlistItem( MapAlbumIdToSyncId( albums[i]->Id() ) );
                playlistItem.SetItemHash( *albums[i], *iHasher );
                playlistItem.SetParentId( KAlbumsFolderId );
                TRAP(iError, iSnapshot->InsertIsqL( playlistItem, key ));
                if ( iError )
                    {
                    LOGGER_WRITE_1("iSnapshot->InsertIsqL err: %d", iError);
                    iError = KErrGeneral;
                    }
                }
            
            if ( !iError )
                {
                // Set new snapshot to compare against
                iChangeFinder->SetNewSnapshot(iSnapshot);
                
                // Changefinder takes ownership of the snapshot
                iSnapshot = NULL;
                iCurrentState = EOpenAndWaiting;
                }
            }
        
        if ( iError )
            {
            iCurrentState = EClosed;
            }
        
        LOGGER_WRITE_1("Signal client with %d", iError);
        User::RequestComplete( iCallerStatus, iError );
        }
    TRACE_FUNC_EXIT;
    }

inline TInt CMediaDsDataStore::MapSyncIdToAlbumId( TSmlDbItemUid aSyncId )
    {
    return aSyncId - KAlbumsOffSet;
    }

inline TSmlDbItemUid CMediaDsDataStore::MapAlbumIdToSyncId( TInt aAlbumId )
    {
    return KAlbumsOffSet + aAlbumId;
    }
