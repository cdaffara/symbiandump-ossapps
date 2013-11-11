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


#include <sysutil.h> 
#include <favouritesitem.h>
#include <favouritesitemlist.h> 

#include <feedsserverclient.h>
#include <feedsserverfeed.h>
#include <feedsserverfolderitem.h>

#include "bookmarkdatastore.h"
#include "vbookmarkconverter.h"
#include "omadsfolderobject.h"
#include "bookmarkdataproviderdefs.h"
#include "conversionutil.h"
#include "changefinder.h"
#include "logger.h"


_LIT8( KBookmarkFolderMimeType, "application/vnd.omads-folder+xml" );
_LIT8( KBookmarkFolderMimeVersion, "" );
_LIT8( KBookmarkItemMimeType, "text/x-vbookmark" );
_LIT8( KBookmarkItemMimeVersion, "" );
_LIT( KRSSRootFolderName, "Web Feeds" );

const TInt KDefaultBufferSize = 1024;
const TInt KDataBufferNotReady = -1;
const TInt KRssFeedsOffset = 50000;
// -----------------------------------------------------------------------------
// CBookmarkDataStore::CBookmarkDataStore
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CBookmarkDataStore::CBookmarkDataStore() :
    iDataBaseOpened( EFalse ), iFeedsServer(*this), iRootFolder(*this),
    iKey( TKeyArrayFix( _FOFF( TNSmlSnapshotItem, ItemId() ), ECmpTInt ) ),
    iHasHistory( EFalse )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore" );
    LOGGER_LEAVEFN( "CBookmarkDataStore" ); 
    }
    
// -----------------------------------------------------------------------------
// CBookmarkDataStore::ConstructL
// Symbian 2nd phase constructor, can leave.
// -----------------------------------------------------------------------------    
void CBookmarkDataStore::ConstructL()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::ConstructL" );
    
    User::LeaveIfError( iRfs.Connect() );
    User::LeaveIfError( iSession.Connect() );
    
    iNewItems = new ( ELeave ) CNSmlDataItemUidSet;
    iDeletedItems = new ( ELeave ) CNSmlDataItemUidSet;
    iSoftDeletedItems = new ( ELeave ) CNSmlDataItemUidSet;
    iUpdatedItems = new ( ELeave ) CNSmlDataItemUidSet;
    iMovedItems = new ( ELeave ) CNSmlDataItemUidSet;
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::ConstructL" );
    }
    
// -----------------------------------------------------------------------------
// CBookmarkDataStore::~CBookmarkDataStore
// Destructor
// -----------------------------------------------------------------------------    
CBookmarkDataStore::~CBookmarkDataStore()
    {
    LOGGER_ENTERFN( "~CBookmarkDataStore" );

    delete iDataBuffer;
    delete iNewItems;
    delete iDeletedItems;
    delete iSoftDeletedItems;
    delete iUpdatedItems;
    delete iMovedItems;
    delete iItemToBeReplaced;
    
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
            
    if ( iDataBaseOpened )
        {
        iDb.Close();
        iRootFolder.Close();
        iFeedsServer.Close();
        }
    
    iSession.Close();
    iRfs.Close();
    LOGGER_LEAVEFN( "~CBookmarkDataStore" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CBookmarkDataStore* CBookmarkDataStore::NewL()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::NewL" );
    CBookmarkDataStore* self = CBookmarkDataStore::NewLC();
    CleanupStack::Pop( self );
    LOGGER_LEAVEFN( "CBookmarkDataStore::NewL" );
    return self;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
CBookmarkDataStore* CBookmarkDataStore::NewLC()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::NewLC" );
    CBookmarkDataStore* self = new ( ELeave ) CBookmarkDataStore();
    CleanupStack::PushL( self );
    self->ConstructL();
    LOGGER_LEAVEFN( "CBookmarkDataStore::NewLC" );
    return self;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoOpenL
// Opens database.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoOpenL( const TDesC& /*aStoreName*/,
    MSmlSyncRelationship& aContext, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoOpenL" );
    
    iCallerStatus = &aStatus;           
    *iCallerStatus = KRequestPending;
    
    if ( iDataBaseOpened )
        {
        User::RequestComplete( iCallerStatus, KErrInUse );
        LOGGER_WRITE( "CBookmarkDataStore::DoOpenL failed with KErrInUse." );
        return;
        }
        
    // Create ChangeFinder
    if ( iChangeFinder )
        {
        delete iChangeFinder;
        iChangeFinder = NULL;
        }
    iChangeFinder = CChangeFinder::NewL( aContext, iKey, iHasHistory );
    
    // Create converter object
    if ( iConverter )
        {
        delete iConverter;
        iConverter = NULL;
        }
    iConverter = CBookmarkConversionUtil::NewL();
    

    TInt err( KErrNone );       
    err = iDb.Open( iSession, KBrowserBookmarks );
    if ( err )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoOpenL iDb.Open err: %d.", err );
        User::RequestComplete( iCallerStatus, err );
        return;
        }
    err = iFeedsServer.Connect();
    if ( err )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoOpenL iFeedsServer.Connect err: %d.", err );
        User::RequestComplete( iCallerStatus, err );
        return;
        }
    err = iRootFolder.Open( iFeedsServer );
    if ( err )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoOpenL iRootFolder.Open err: %d.", err );
        User::RequestComplete( iCallerStatus, err );
        return;
        }
        
    iRootFolder.FetchRootFolderItemL(); // calls FolderItemRequestCompleted when completed
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoOpenL" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::FolderItemRequestCompleted
// RSS item database request is completed
// -----------------------------------------------------------------------------
void CBookmarkDataStore::FolderItemRequestCompleted( TInt aStatus,
    CRequestHandler::TRequestHandlerType )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::FolderItemRequestCompleted" );
    
    TRAPD( err, DoFolderItemRequestCompletedL( aStatus ) );
    
    if ( err != KErrNone )
        {
        LOGGER_WRITE_1( "DoFolderItemRequestCompletedL leaved with err: %d", err );
        User::RequestComplete( iCallerStatus, err );
        }
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::FolderItemRequestCompleted" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoFolderItemRequestCompletedL
// RSS item database request is completed
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoFolderItemRequestCompletedL( TInt aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoFolderItemRequestCompletedL" );
    switch ( iCurrentState )
        {
        case EClosed : 
            {
            LOGGER_WRITE("Database ready");
            iDataBaseOpened = ETrue;
            iCurrentState = EOpenAndWaiting;
            RegisterSnapshotL();
            break;
            }
        
        // bookmark is added (Add command)
        case EBookmarkCreating :
           	{
           	// RSS Item has been created.
           	LOGGER_WRITE_1("RSS item created, err %d", aStatus);
           	// Restore state
           	iCurrentState = EOpenAndWaiting;
            if ( aStatus == KErrNone )
                {
                RssItemCreatedL();
                }
            break;
            }
        
        // bookmark is updated (Replace command)
        case EBookmarkUpdating :
            {
            LOGGER_WRITE("RSS item replaced");
            if ( aStatus == KErrNone )
                {
                TBool moveNeeded(EFalse);
                RssItemReplacedL( moveNeeded );
                
                if ( moveNeeded )
                    {
                    // Moving is still ongoing, don't complete request yet.
                    return;
                    }
                
                LOGGER_WRITE_1("Item ID: %d", *iCurrentItem);
                LOGGER_WRITE_1("Parent ID: %d", iParentFolder);
                }
            // Restore state
            iCurrentState = EOpenAndWaiting;
            break;
            }
        
        // bookmark is updated and moved (Replace command, 2nd phase)
        case EBookmarkUpdatedAndMoving:
            {
            LOGGER_WRITE_1( "RSS item moved (replaced), err: %d", aStatus );
            if ( aStatus == KErrNone )
                {
                // Inform ChangeFinder of updated item
                iChangeFinder->ItemUpdatedL( *iMovedItem ); // ownership transferred
                iMovedItem = NULL;
                LOGGER_WRITE( "ItemReplacedL" );
                }
            else
                {
                delete iMovedItem;
                iMovedItem = NULL;
                }
            // Restore state
            iCurrentState = EOpenAndWaiting;
            break;
            }
        
        // bookmark is moved (Move command)
        case EBookmarkMoving:
            {
            LOGGER_WRITE_1( "RSS item moved, err: %d", aStatus );
            if ( aStatus == KErrNone )
                {
                iChangeFinder->ItemMovedL( *iMovedItem ); // ownership transferred
                iMovedItem = NULL;
                }
            else
                {
                delete iMovedItem;
                iMovedItem = NULL;
                }
            // Restore state
            iCurrentState = EOpenAndWaiting;
            break;
            }
            
        case EBookmarkDeleting:
            {
            LOGGER_WRITE_1( "RSS item removed, err: %d", aStatus );
            if ( aStatus == KErrNone )
                {
                // Inform ChangeFinder of the removed item
                TSnapshotItem item( iReplaceItem );     
                iChangeFinder->ItemDeletedL( item );
                }
            // Restore state
            iCurrentState = EOpenAndWaiting;
            break;
            }
        case EBookmarkDeletingAll:
            {
            LOGGER_WRITE_1( "all RSS items removed, err: %d", aStatus );
            break;
            }
        
        default :
            LOGGER_WRITE_1( "Invalid state: %d", iCurrentState );
            return;
        }
    User::RequestComplete( iCallerStatus, aStatus );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoFolderItemRequestCompletedL" ); 
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::RssItemCreatedL
// Rss item has been created, add item to changefinder
// -----------------------------------------------------------------------------
void CBookmarkDataStore::RssItemCreatedL()
    {
    TRACE_FUNC_ENTRY;
    if ( !iRootFolder.HasRootFolderItem() )
        {
        LOGGER_WRITE( "rss root folder not found" );
        User::Leave( KErrNotFound );
        }
    const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
    const CFolderItem* parent = rootFolder.Search( iParentFolder - KRssFeedsOffset );
    if ( !parent )
        {
        LOGGER_WRITE( "parent folder not found" );
        User::Leave( KErrNotFound );
        }
    LOGGER_WRITE_1("parent->Id(): %d", parent->Id());
    LOGGER_WRITE_1( "Name: %S", &iNewItemName );
    const CFolderItem* addedItem = parent->Search( iNewItemName );
    iNewItemName.Zero();
    if ( addedItem )
        {
        LOGGER_WRITE_1("addedItem found, id: %d", addedItem->Id());
        
        // Output the uid value of new item
        *iCurrentItem = addedItem->Id() + KRssFeedsOffset;
        
        // Inform ChangeFinder of the added item
        TSnapshotItem snapshotItem( *iCurrentItem );
        snapshotItem.SetParentId( iParentFolder );
        snapshotItem.CreateHashL( addedItem->Name(), addedItem->SourceUrl() );
        iChangeFinder->ItemAddedL( snapshotItem );
        LOGGER_WRITE( "ItemAddedL" );
        }
    else
        {
        LOGGER_WRITE( "Added item not found" );
        User::Leave( KErrNotFound );
        }
    LOGGER_WRITE_1("Item ID: %d", *iCurrentItem);
    LOGGER_WRITE_1("Parent ID: %d", iParentFolder);
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::RssItemReplacedL
// Rss item has been replaced, update changefinder
// -----------------------------------------------------------------------------
void CBookmarkDataStore::RssItemReplacedL( TBool& aMoveNeeded )
    {
    TRACE_FUNC_ENTRY;
    aMoveNeeded = EFalse;
    if ( !iRootFolder.HasRootFolderItem() )
        {
        LOGGER_WRITE( "rss root folder not found" );
        User::Leave( KErrNotFound );
        }
    const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
    LOGGER_WRITE_1( "rootFolder.Search %d", iReplaceItem - KRssFeedsOffset );
    const CFolderItem* modItem = rootFolder.Search( iReplaceItem - KRssFeedsOffset );
    if ( modItem )
        {
        LOGGER_WRITE_1("Item found, id: %d", modItem->Id());
        
        const CFolderItem* parent = modItem->Parent();
        if ( parent )
            {
            TInt currentParent = parent->Id() + KRssFeedsOffset;
            if ( iParentFolder != parent->Id() + KRssFeedsOffset )
                {
                // move to another folder
                const CFolderItem* newParent = rootFolder.Search(
                    iParentFolder - KRssFeedsOffset );
                if ( !newParent )
                    {
                    // new parent not found
                    LOGGER_WRITE( "new parent not found" );
                    User::Leave( KErrPathNotFound );
                    }
                else if ( !newParent->IsFolder() )
                    {
                    // not a folder
                    LOGGER_WRITE( "parent is not a folder" );
                    User::Leave( KErrPathNotFound );
                    }
                else
                    {
                    // Output the uid value of new item
                    *iCurrentItem = modItem->Id() + KRssFeedsOffset;
        
                    iCurrentState = EBookmarkUpdatedAndMoving;
                    RPointerArray<const CFolderItem> movedItems;
                    CleanupClosePushL( movedItems );
                    movedItems.Append( modItem );
                    // FolderItemRequestCompleted is called when ready
                    iRootFolder.MoveFolderItemsToL( movedItems, *newParent);
                    CleanupStack::PopAndDestroy( &movedItems );
                    aMoveNeeded = ETrue;
                    
                    delete iMovedItem;
                    iMovedItem = NULL;
                    iMovedItem = new (ELeave) TSnapshotItem( *iCurrentItem ,
                        iParentFolder );
                    iMovedItem->CreateHashL( modItem->Name(), modItem->SourceUrl() );
                    LOGGER_WRITE( "ItemReplacedL,waiting to move item.." );
                    return;
                    }
                
                }
            }
        else
            {
            //parent not found
            LOGGER_WRITE( "rss parent not defined" );
            }
        
        // Output the uid value of new item
        *iCurrentItem = modItem->Id() + KRssFeedsOffset;
        
        // Inform ChangeFinder of updated item
        TSnapshotItem snapshotItem( *iCurrentItem, iParentFolder );
        snapshotItem.CreateHashL( modItem->Name(), modItem->SourceUrl() );
        iChangeFinder->ItemUpdatedL( snapshotItem );
        LOGGER_WRITE( "ItemReplacedL" );
        }
    else
        {
        LOGGER_WRITE_1( "Replaced item %d not found",iReplaceItem - KRssFeedsOffset );
        User::Leave( KErrNotFound );
        }
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCancelRequest
// Not supported, does nothing.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCancelRequest()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCancelRequest" );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoCancelRequest" );    
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoStoreName
// Returns the name of the DataStore
// -----------------------------------------------------------------------------
const TDesC& CBookmarkDataStore::DoStoreName() const
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoStoreName" );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoStoreName" );
    
    if ( iDataBaseOpened )
        {
        return KBrowserBookmarks;
        }
    else
        {
        return KNullDesC;
        }
    }
    
// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoBeginTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------    
void CBookmarkDataStore::DoBeginTransactionL()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoBeginTransactionL" );
    LOGGER_WRITE( "CBookmarkDataStore::DoBeginTransactionL leaved with KErrNotSupported." )
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCommitTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCommitTransactionL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCommitTransactionL" );
    LOGGER_WRITE( "CBookmarkDataStore::DoCommitTransactionL failed with KErrNotSupported." );
    
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoCommitTransactionL" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoRevertTransaction
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoRevertTransaction( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoRevertTransaction" );
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoRevertTransaction" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoBeginBatchL
// Batching is not supported.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoBeginBatchL()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoBeginBatchL" );
    LOGGER_WRITE( "CBookmarkDataStore::DoBeginBatchL leaved with KErrNotSupported." );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCommitBatchL
// Batching is not supported
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCommitBatchL( RArray<TInt>& /*aResultArray*/, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCommitBatchL" );
    LOGGER_WRITE( "CBookmarkDataStore::DoCommitBatchL failed with KErrNotSupported" );
        
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoCommitBatchL" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCancelBatch
// Batching is not supported
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCancelBatch()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCancelBatch" );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoCancelBatch" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoSetRemoteStoreFormatL
// Not supported
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoSetRemoteStoreFormatL( const CSmlDataStoreFormat& /*aServerDataStoreFormat*/ )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoSetRemoteStoreFormatL" );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoSetRemoteStoreFormatL" );
    }
    
// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoSetRemoteMaxObjectSize
// Not supported
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoSetRemoteMaxObjectSize( TInt /*aServerMaxObjectSize*/ )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoSetRemoteMaxObjectSize" );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoSetRemoteMaxObjectSize" );   
    }


// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoMaxObjectSize
// Reads the maximum object size from the central repository
// -----------------------------------------------------------------------------
TInt CBookmarkDataStore::DoMaxObjectSize() const
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoMaxObjectSize" );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoMaxObjectSize" );    
    return 0; // no limit
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoOpenItemL
// Opens item in the DataStore, reads it (either completely or partially) 
// to the temporary buffer where it can be later read to the remote database.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoOpenItemL(
    TSmlDbItemUid aUid, 
    TBool& aFieldChange, 
    TInt& aSize, 
    TSmlDbItemUid& aParent, 
    TDes8& aMimeType, 
    TDes8& aMimeVer, 
    TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoOpenItemL" );
    LOGGER_WRITE_1( "Item: %d", aUid );
    
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    aFieldChange = EFalse;
    
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "Warning: Unexpected current state: %d", iCurrentState );
        }
    
    SAFEDELETE( iDataBuffer );
    iDataBuffer = CBufFlat::NewL( KDefaultBufferSize ); 
    iReaderPosition = 0;
    if ( aUid < KRssFeedsOffset )
        {
        LOGGER_WRITE( "Handle id as bookmark item " );
        TInt err( KErrNone );   
        CFavouritesItem* item = CFavouritesItem::NewLC();
        err = iDb.Get( aUid, *item );
        if (err)
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE_1( "RFavouritesDb::Get failed with %d", err );
            CleanupStack::PopAndDestroy( item );
            return;
            }
            
        
        
        iConverter->FavouritesItemToBufferL( *item, *iDataBuffer );
        aSize = iDataBuffer->Size();
        aParent = item->ParentFolder();

        // Set mime type according to item type
        // The following code handles also the case, where the receiving
        // buffer doesn't contain enough space for the type (truncated)
        if ( item->Type() == CFavouritesItem::EItem )
            {
            LOGGER_WRITE("item type: EItem");
            AssignString( aMimeType, KBookmarkItemMimeType );
            AssignString( aMimeVer, KBookmarkItemMimeVersion );
            }
        else
            {
            LOGGER_WRITE("item type: EFolder");
            AssignString( aMimeType, KBookmarkFolderMimeType );
            AssignString( aMimeVer, KBookmarkFolderMimeVersion );
            }

        CleanupStack::PopAndDestroy( item );
        }
    else
        {
        LOGGER_WRITE( "Handle id as rssFeed item " );
        
        TInt rssId = aUid - KRssFeedsOffset;
        LOGGER_WRITE_1( "rssId: %d", rssId );
        
        if ( !iRootFolder.HasRootFolderItem() )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "rss root folder not found" );
            return;
            }
        
        const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
        const CFolderItem* founded = rootFolder.Search( rssId );
        if ( founded )
            {
            LOGGER_WRITE( "Item found" );
            const CFolderItem* parent = founded->Parent();
            if ( parent )
                {
                LOGGER_WRITE( "parent found" );
                aParent = parent->Id() + KRssFeedsOffset;
                }
            else
                {
                LOGGER_WRITE( "no parent (root)" );
                aParent = KErrNotFound;// rootFolder.Id() + KRssFeedsOffset;
                }
            
            if ( !founded->IsFolder() )
                {
                LOGGER_WRITE("item type: EItem");
                AssignString( aMimeType, KBookmarkItemMimeType );
                AssignString( aMimeVer, KBookmarkItemMimeVersion );
                }
            else
                {
                LOGGER_WRITE("item type: EFolder");
                AssignString( aMimeType, KBookmarkFolderMimeType );
                AssignString( aMimeVer, KBookmarkFolderMimeVersion );
                }
            iConverter->RssItemToBufferL( *founded, *iDataBuffer );
            aSize = iDataBuffer->Size();
            
            }
        else
            {
            LOGGER_WRITE( "Item not found" );
            User::RequestComplete( iCallerStatus, KErrNotFound );
            return;
            }
        
        }
    
    User::RequestComplete( iCallerStatus, KErrNone );   
    iCurrentState = EBookmarkOpen;
    
#ifdef _DEBUG
    LOGGER_WRITE_1( "aFieldChange: %d", (TInt)aFieldChange );
    LOGGER_WRITE_1( "aSize: %d", aSize );
    LOGGER_WRITE_1( "aParent: %d", aParent );
    TPtr8 bufPtr = iDataBuffer->Ptr(0);
    LOGGER_WRITE8_1( "iDataBuffer: %S", &bufPtr);
#endif

    LOGGER_LEAVEFN( "CBookmarkDataStore::DoOpenItemL" );        
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCreateItemL
// Create new item to the message store.
// Return the id number of the newly created item
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCreateItemL(
    TSmlDbItemUid& aUid, 
    TInt aSize, 
    TSmlDbItemUid aParent, 
    const TDesC8& aMimeType, 
    const TDesC8& /*aMimeVer*/, 
    TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCreateItemL" );
    LOGGER_WRITE_1( "aParent: %d", aParent );
    LOGGER_WRITE8_1( "aMimeType: %S", &aMimeType );
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    delete iItemToBeReplaced;
    iItemToBeReplaced = NULL;
    
    // Check MIME type
    if ( aMimeType.Compare( KBookmarkFolderMimeType() ) == 0 )  
        {
        LOGGER_WRITE("item type: EFolder");
        iMimeType = CFavouritesItem::EFolder;
        }
    else if ( aMimeType.Compare( KBookmarkItemMimeType() ) == 0 )
        {
        LOGGER_WRITE("item type: EItem");
        iMimeType = CFavouritesItem::EItem;
        }
    else
        {
        User::RequestComplete( iCallerStatus, KErrNotSupported );
        LOGGER_WRITE( "Unexpected mime type" );
        return;
        }   
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "Warning: Unexpected current state: %d", iCurrentState );
        }
        
    // Ensure that we've got enough disk space for the item
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, aSize, EDriveC ) )
        {
        User::RequestComplete( iCallerStatus, KErrDiskFull );
        LOGGER_WRITE( "SysUtil::DiskSpaceBelowCriticalLevelL failed with KErrDiskFull." );
        return;
        }
    
    if ( aParent < KRssFeedsOffset )
        {
        LOGGER_WRITE( "Handle id as bookmark item" );
        // Ensure that parent folder exists 
        TBool folderExists( EFalse );   
        TInt err = iDb.FolderExists( aParent, folderExists );
        if ( err || !folderExists )
            {
            User::RequestComplete( iCallerStatus, KErrPathNotFound );
            LOGGER_WRITE_1( "iDb.FolderExist returned %d, folder not found", err );
            return;
            }
        
        }
    else
        {
        LOGGER_WRITE( "Handle id as rssFeed item " );
        
        TInt rssId = aParent - KRssFeedsOffset;
        LOGGER_WRITE_1( "parentRssId: %d", rssId );
        
        if ( !iRootFolder.HasRootFolderItem() )
            {
            User::RequestComplete( iCallerStatus, KErrPathNotFound );
            LOGGER_WRITE( "rss root folder not found" );
            return;
            }
        
        const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
        const CFolderItem* founded = rootFolder.Search( rssId );
        TBool folderExists( EFalse );
        if ( founded )
            {
            if ( founded->IsFolder() )
                {
                folderExists = ETrue;
                }
            }
        if ( !folderExists )
            {
            User::RequestComplete( iCallerStatus, KErrPathNotFound );
            LOGGER_WRITE( "Folder does not exist" );
            return;
            }
        }
    
    iCurrentState = EBookmarkCreating;
    // Store parent for further use 
    iParentFolder = aParent;    
                
    SAFEDELETE( iDataBuffer );
    iDataBuffer = CBufFlat::NewL( KDefaultBufferSize );
    iWriterPosition = 0;
        
    iCurrentItem = &aUid;   
    
    
    User::RequestComplete( iCallerStatus, KErrNone );
        
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoCreateItemL" );      
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoReplaceItemL
// Begin the replace operation, ensure that the item really exists
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoReplaceItemL(
    TSmlDbItemUid aUid, 
    TInt aSize, 
    TSmlDbItemUid aParent,
    TBool /*aFieldChange*/, 
    TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoReplaceItemL" );
    LOGGER_WRITE_1("aUid: %d", aUid);
    LOGGER_WRITE_1("aParent: %d", aParent);
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    delete iItemToBeReplaced;
    iItemToBeReplaced = NULL;
    
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "Warning: Unexpected current state: %d", iCurrentState );
        }
        
    // Ensure that we've got enough disk space for the item
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, aSize, EDriveC ) )
        {
        User::RequestComplete( iCallerStatus, KErrDiskFull );
        LOGGER_WRITE( "SysUtil::DiskSpaceBelowCriticalLevelL failed with KErrDiskFull." );
        return;
        }       
    
    if ( aUid < KRssFeedsOffset )
        {
        LOGGER_WRITE( "Bookmark item" );
        iItemToBeReplaced = CFavouritesItem::NewL();
        // Check that item exists
        TInt err = iDb.Get( aUid, *iItemToBeReplaced );
        if ( err != KErrNone )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE_1( "iDb.Get failed with %d", err );
            SAFEDELETE(iItemToBeReplaced);
            return;
            }
        
        // is parent changed
        if ( aParent != iItemToBeReplaced->ParentFolder() )
            {
            if ( aParent >= KRssFeedsOffset )
                {
                User::RequestComplete( iCallerStatus, KErrNotSupported );
                LOGGER_WRITE( "cannot move to RSS items folder" );
                SAFEDELETE(iItemToBeReplaced);
                return;
                }
            // parent is changed, ensure that new parent folder exists
            TBool folderExists( EFalse ); 
            err = iDb.FolderExists( aParent, folderExists );
            if ( err || !folderExists )
                {
                User::RequestComplete( iCallerStatus, KErrPathNotFound );
                LOGGER_WRITE_1( "iDb.FolderExist returned %d, new parent folder not found", err );
                SAFEDELETE(iItemToBeReplaced);
                return;
                }
            }
        
        
        // Store some variables to be used later in commit
        iMimeType = iItemToBeReplaced->Type();
        iItemToBeReplaced->SetParentFolder( aParent );
        iParentFolder = aParent;
        iReplaceItem = aUid;
        }
    else
        {
        LOGGER_WRITE( "RSS item" );
        if ( aParent < KRssFeedsOffset )
            {
            User::RequestComplete( iCallerStatus, KErrNotSupported );
            LOGGER_WRITE( "cannot move RSS item to normal folder" );
            return;
            }
        if ( !iRootFolder.HasRootFolderItem() )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "rss root folder not found" );
            return;
            }
        const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
        const CFolderItem* rssItem = rootFolder.Search( aUid - KRssFeedsOffset );
        if ( !rssItem )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "rss item not found" );
            return;
            }
        if ( rssItem->IsFolder() )
            {
            iMimeType = CFavouritesItem::EFolder;
            }
        else
            {
            iMimeType = CFavouritesItem::EItem;
            }
        const CFolderItem* parent = rssItem->Parent();
        if ( parent )
            {
            TInt currentParent = parent->Id() + KRssFeedsOffset;
            
            const CFolderItem* newParent = rootFolder.Search( aParent - KRssFeedsOffset );
            if ( !newParent )
                {
                User::RequestComplete( iCallerStatus, KErrPathNotFound );
                LOGGER_WRITE( "new parent not found" );
                return;
                }
            else if ( !newParent->IsFolder() )
                {
                User::RequestComplete( iCallerStatus, KErrPathNotFound );
                LOGGER_WRITE( "new parent must be folder" );
                return;
                }
            else
                {
                iParentFolder = aParent;
                }
            
            }
        else
            {
            iParentFolder = KErrNotFound;
            }
        iReplaceItem = aUid;
        }
    
    
    
    SAFEDELETE(iDataBuffer);
    iDataBuffer = CBufFlat::NewL( KDefaultBufferSize );
    iWriterPosition = 0;
        
    iCurrentItem = &aUid;   
    iCurrentState = EBookmarkUpdating;
    
    User::RequestComplete( iCallerStatus, KErrNone );   
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoReplaceItemL" );     
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoReadItemL
// Read specified amount of data from the temporary buffer
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoReadItemL( TDes8& aBuffer )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoReadItemL" );
    
    if (iCurrentState != EBookmarkOpen || !iDataBuffer)
        {
        LOGGER_WRITE_1( "Unexpected state %d", iCurrentState );
        User::Leave( KErrNotReady );
        }
        
    if (iReaderPosition == KDataBufferNotReady)
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
        
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoReadItemL" );        
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoWriteItemL
// Write specified amount of data to the temporary buffer
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoWriteItemL( const TDesC8& aData )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoWriteItemL" );
    
    if ( iCurrentState != EBookmarkCreating && iCurrentState != EBookmarkUpdating )
        {
        LOGGER_WRITE_1( "Unexpected current state: %d", iCurrentState );
        User::Leave( KErrNotReady );
        }
    
    // Calculate total size
    TInt totalSize = aData.Size() + iDataBuffer->Size();

    // Ensure that we've got enough disk space for the item
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &iRfs, totalSize, EDriveC ) )
        {
        User::RequestComplete( iCallerStatus, KErrDiskFull );
        LOGGER_WRITE("SysUtil::DiskSpaceBelowCriticalLevelL failed with KErrDiskFull.");
        return;
        }
        
    // Add data to buffer       
    iDataBuffer->InsertL( iWriterPosition, aData );
    iWriterPosition += aData.Size();
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoWriteItemL" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCommitItemL
// Commits item from temporary buffer to the message store
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCommitItemL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCommitItemL" );
    LOGGER_WRITE_1("iParentFolder: %d", iParentFolder);
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
    if ( iCurrentState != EBookmarkCreating && iCurrentState != EBookmarkUpdating )
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
    
    
    
    // If iItemToBeReplaced exist, bookmark item is going to be replaced.
    // iItemToBeReplaced contains original bookmark and only some of it's data is replaced.
    CFavouritesItem* item(NULL);
    if ( iItemToBeReplaced )
        {
        // Change object ownership to "item"-object, and push to cleanupStack
        item = iItemToBeReplaced;
        CleanupStack::PushL( item );
        iItemToBeReplaced = NULL;
        }
    else
        {
        // Create new object.
        item = CFavouritesItem::NewLC();
        }
    
    iDataBuffer->Compress();
    TInt err = iConverter->BufferToFavouritesItemL( iMimeType, *iDataBuffer, *item );
    
    if ( iParentFolder == KErrNotFound )
        {
        if ( item->Name().Compare( KRSSRootFolderName ) == 0 )
            {
            LOGGER_WRITE( "Was RSS Rootfolder" );
            // Remove from cleanup stack
            CleanupStack::PopAndDestroy( item );
            
            if ( !iRootFolder.HasRootFolderItem() )
                {
                User::RequestComplete( iCallerStatus, KErrNotFound );
                LOGGER_WRITE( "rss root folder not found" );
                return;
                }
            const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
            
            *iCurrentItem = rootFolder.Id() + KRssFeedsOffset;
            
            // Inform ChangeFinder of updated item
            TSnapshotItem snapshotItem( rootFolder.Id() + KRssFeedsOffset );
            snapshotItem.SetParentId( iParentFolder );
            snapshotItem.CreateHashL( KRSSRootFolderName, KNullDesC );
            iChangeFinder->ItemUpdatedL( snapshotItem );
            
            // Destroy buffer
            SAFEDELETE( iDataBuffer );
            iWriterPosition = 0;
            
            // Restore state and signal we're done
            LOGGER_WRITE( "Signal KErrNone" );
            iCurrentState = EOpenAndWaiting;
            User::RequestComplete( iCallerStatus, KErrNone );
            
            LOGGER_WRITE_1("Item ID: %d", *iCurrentItem);
            LOGGER_WRITE_1("Parent ID: %d", iParentFolder);
            LOGGER_LEAVEFN( "CBookmarkDataStore::DoCommitItemL" );
            return;
            }
        }

    if ( iParentFolder < KRssFeedsOffset )
        {
        // Convert buffer to favourites item
        LOGGER_WRITE( "BookmarkItem" );
        
        if ( err == KErrNone )
            {
            LOGGER_WRITE_1 ( "ParentFolder: %d", iParentFolder )
            // Creating new item
            if ( iCurrentState == EBookmarkCreating )
                {
#ifdef _DEBUG                
                LOGGER_WRITE( "Create new bookmark item" );
                TPtrC namePtr = item->Name();
                LOGGER_WRITE_1( "name: %S", &namePtr );
#endif
                // Set parent folder
                item->SetParentFolder( iParentFolder );

                // Add to database
                err = iDb.Add( *item, EFalse );
                LOGGER_WRITE_1("iDb.Add return: %d", err);
                if ( err == KErrNone )
                    {
                    // Output the uid value of new item
                    *iCurrentItem = item->Uid();
                    LOGGER_WRITE_1("New item Uid: %d", *iCurrentItem);
                    // Inform ChangeFinder of the added item
                    TSnapshotItem snapshotItem( *iCurrentItem );
                    snapshotItem.SetParentId( iParentFolder );
                    snapshotItem.CreateHashL( item->Name(), item->Url() );
                    iChangeFinder->ItemAddedL( snapshotItem );
                    }
                else
                    {
                    LOGGER_WRITE_1( "RFavouritesDb::Add failed: %d", err );
                    }
                }
            // Updating existing item
            else
                {
                LOGGER_WRITE( "Update existing item" );
                item->SetParentFolder(iParentFolder);
                err = iDb.Update( *item, iReplaceItem, EFalse );
                if ( err == KErrNone )
                    {
                    // Inform ChangeFinder of updated item
                    TSnapshotItem snapshotItem( iReplaceItem );
                    snapshotItem.SetParentId( iParentFolder );
                    snapshotItem.CreateHashL( item->Name(), item->Url() );
                    iChangeFinder->ItemUpdatedL( snapshotItem );
                    }
                else
                    {
                    LOGGER_WRITE_1( "RFavouritesDb::Update failed: %d", err );
                    }
                }
            }
        else
            {
            LOGGER_WRITE_1( "Failed to create db item (%d)", err );
            }

        }
    else
        {
        LOGGER_WRITE( "RSS item" );
        err = iConverter->BufferToFavouritesItemL( iMimeType, *iDataBuffer, *item );
        // Destroy buffer
        SAFEDELETE( iDataBuffer );
        iWriterPosition = 0;
        if ( err == KErrNone )
            {
            if ( iCurrentState == EBookmarkCreating )
                {
                TBool isFolder = ( iMimeType == CFavouritesItem::EFolder );
                CreateRssItemL( iParentFolder - KRssFeedsOffset, item->Name(), item->Url(), isFolder );
                
                CleanupStack::PopAndDestroy( item );
                LOGGER_WRITE( "waiting FolderItemRequestCompleted..." );
                return;    
                }
            else
                {
                LOGGER_WRITE( "Replace RSS item" );
                ReplaceRssItemL( iReplaceItem - KRssFeedsOffset, item->Name(), item->Url());
                
                CleanupStack::PopAndDestroy( item );
                LOGGER_WRITE( "waiting FolderItemRequestCompleted..." );
                return;
                }
            }
        else
            {
            // Data was corrupted
            LOGGER_WRITE_1( " iConverter->BufferToFavouritesItemL err: %d", err );
            err = KErrCorrupt;
            }
        }
    

    // Remove from cleanup stack
    CleanupStack::PopAndDestroy( item );
    
    // Destroy buffer
    SAFEDELETE( iDataBuffer );
    iWriterPosition = 0;
    
    // Restore state and signal we're done
    iCurrentState = EOpenAndWaiting;
    User::RequestComplete( iCallerStatus, err );
    
    LOGGER_WRITE_1("Item ID: %d", iReplaceItem);
    LOGGER_WRITE_1("Parent ID: %d", iParentFolder);
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoCommitItemL" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::CreateRssItemL
// Creates new Rss item, FolderItemRequestCompleted is called when done
// -----------------------------------------------------------------------------
void CBookmarkDataStore::CreateRssItemL( TInt aRssParentUid, const TDesC& aName, const TDesC& aUrl, TBool aIsFolder )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aRssParentUid: %d", aRssParentUid);
    LOGGER_WRITE_1("aName: %S", &aName);
    
    if ( !iRootFolder.HasRootFolderItem() )
        {
        LOGGER_WRITE( "rss root folder not found" );
        User::Leave( KErrNotFound );
        }
    
    const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
    
    // check that parent folder exists
    const CFolderItem* parent = rootFolder.Search( aRssParentUid );
    if ( !parent )
        {
        LOGGER_WRITE( "parent folder not found" );
        User::Leave( KErrNotFound );
        }
    
    // check that folder does not already exists
    const CFolderItem* existingItem = parent->Search( aName );
    if ( existingItem )
        {
        LOGGER_WRITE("Same name already exist, return KErrAlreadyExists");
        User::Leave( KErrAlreadyExists );
        }
    
    TInt err(KErrNone);
    // async func. FolderItemRequestCompleted is called when completed
    TRAP( err, iRootFolder.AddFolderItemL(
        aName,
        aUrl,
        aIsFolder,
        *parent ) );
    LOGGER_WRITE_1( "iRootFolder.AddFolderItemL err: %d", err );
    
    iNewItemName.Copy( aName );
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::ReplaceRssItemL
// Replaces rss item, FolderItemRequestCompleted is called when done
// -----------------------------------------------------------------------------
void CBookmarkDataStore::ReplaceRssItemL( TInt aRssItemUid, const TDesC& aNewName, const TDesC& aNewUrl )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aRssItemUid: %d", aRssItemUid);
    LOGGER_WRITE_1("aNewName: %S", &aNewName);
    LOGGER_WRITE_1("aNewUrl: %S", &aNewUrl);
    
    if ( !iRootFolder.HasRootFolderItem() )
        {
        LOGGER_WRITE( "rss root folder not found" );
        User::Leave( KErrNotFound );
        }
    const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
    LOGGER_WRITE_1( "rootFolder.Search %d", aRssItemUid );
    const CFolderItem* rssItem = rootFolder.Search( aRssItemUid );
    if ( !rssItem )
        {
        LOGGER_WRITE( "rssItem not found" );
        User::Leave( KErrNotFound );
        }
    
    // async func. FolderItemRequestCompleted is called when completed
    if ( rssItem->IsFolder() )
        {
        iRootFolder.ChangeFolderItemL(
        *rssItem,
        aNewName,
        KNullDesC );
        }
    else
        {
        iRootFolder.ChangeFolderItemL(
        *rssItem,
        aNewName,
        aNewUrl );
        }
    
    TRACE_FUNC_EXIT;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCloseItem
// Closes open item in the data store
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCloseItem()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCloseItem" );
    
    // Make sure that we're opened an item
    if ( iCurrentState == EBookmarkOpen )
        {
        // Reset read buffer 
        iReaderPosition = KDataBufferNotReady;
        SAFEDELETE( iDataBuffer );
        
        // Start to wait for the next operation     
        iCurrentState = EOpenAndWaiting;
        }
    else 
        {
        LOGGER_WRITE_1( "Invalid state %d.", iCurrentState );
        }   
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoCloseItem" );    
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoMoveItemL
// Moves item from one bookmark folder to another 
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoMoveItemL( TSmlDbItemUid aUid,
    TSmlDbItemUid aNewParent, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoMoveItemL" );
    LOGGER_WRITE_1("aUid: %d", aUid);
    LOGGER_WRITE_1("new parent: %d", aNewParent);

    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;

    TInt err( KErrNone ); 
    
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "WARNING! Invalid state: %d", iCurrentState );
        }
    
    if ( aUid < KRssFeedsOffset )
        {
        // Bookmark item
        LOGGER_WRITE( "Bookmark item" );
        // Ensure that new parent folder exists 
        TBool folderExists( EFalse );   
        err = iDb.FolderExists( aNewParent, folderExists );
        if ( err || !folderExists )
            {
            User::RequestComplete( iCallerStatus, KErrPathNotFound );
            LOGGER_WRITE_1( "iDb.FolderExist returned %d, new parent folder not found", err );
            return;
            }   
        
        CFavouritesItem* item = CFavouritesItem::NewLC();

        err = iDb.Get( aUid, *item );
        if ( err == KErrNone )
            {
            if ( item->ParentFolder() != aNewParent )
                {
                TTime time = item->Modified();
                
                // Parent was changed, set new
                item->SetParentFolder( aNewParent );

                // Update, this'll move the item
                err = iDb.Update( *item, aUid, ETrue );
                
                if ( err == KErrNone )
                    {
                    // Do not change the timestamp, otherwise this will be
                    // considered to be 'updated' item instead of moved one
                    err = iDb.SetModified(aUid, time);
                    if ( err != KErrNone )
                        {
                        LOGGER_WRITE_1( "iDb.SetModified(aUid, time) failed (%d).", err );
                        }
                        
                    // Inform ChangeFinder of updated item
                    TSnapshotItem snapshotItem( aUid );
                    snapshotItem.SetParentId( iParentFolder );
                    snapshotItem.CreateHashL( item->Name(), item->Url() );
                    iChangeFinder->ItemMovedL( snapshotItem );
                    }
                else 
                    {
                    LOGGER_WRITE_1( "iDb.Update(*item, aUid, ETrue) failed (%d).", err );
                    }               
                }
            else 
                {
                LOGGER_WRITE( "Ignoring move (parent was already correct)." );
                }           
            }
        else
            {
            LOGGER_WRITE_1( "RFavouritesDb::Get failed with %d", err );
            err = KErrNotFound;
            }

        CleanupStack::PopAndDestroy( item );
        User::RequestComplete( iCallerStatus, err ); 
        }
    else
        {
        // RRS item
        LOGGER_WRITE( "RSS item" );
        if ( !iRootFolder.HasRootFolderItem() )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "rss root folder not found" );
            return;
            }
        const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
        const CFolderItem* rssItem = rootFolder.Search( aUid - KRssFeedsOffset );
        if ( !rssItem )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "rssItem not found" );
            return;
            }
        LOGGER_WRITE_1("rssItem->Id(): %d", rssItem->Id());
        if ( !rssItem->Parent() )
            {
            // this is "RSS Root" folder, can't move
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "RSS Root folder, can't move" );
            return;
            }
        const CFolderItem* parent = rootFolder.Search( aNewParent - KRssFeedsOffset );
        if ( !parent )
            {
            // new parent not found
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "new parent not found" );
            return;
            }
        if ( !parent->IsFolder() )
            {
            // not a folder
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "parent is not a folder" );
            return;
            }
        RPointerArray<const CFolderItem> movedItems;
        CleanupClosePushL( movedItems );
        movedItems.Append( rssItem );
        // FolderItemRequestCompleted is called when ready
        iRootFolder.MoveFolderItemsToL( movedItems, *parent);
        CleanupStack::PopAndDestroy( &movedItems );
              
        delete iMovedItem;
        iMovedItem = NULL;
        iMovedItem = new (ELeave) TSnapshotItem( aUid, aNewParent );
        
        iCurrentState = EBookmarkMoving;
        LOGGER_WRITE( "Wait for FolderItemRequestCompleted.." );
        }   
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoMoveItemL" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoDeleteItemL
// Removes item from the message store
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoDeleteItemL" );
    LOGGER_WRITE_1( "aUid: %d", aUid );
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CBookmarkDataStore::DoDeleteItemL, Incorrect state: %d", iCurrentState);
        }
    
    TInt error( KErrNone );
    if ( aUid < KRssFeedsOffset )
        {
        LOGGER_WRITE( "Delete Bookmark item" );
        // Delete item from db and return the error code
        error = iDb.Delete( aUid );
        
        if ( error == KErrNone )
            {
            // Inform ChangeFinder of the removed item
            TSnapshotItem item( aUid );     
            iChangeFinder->ItemDeletedL( item );
            }
        else
            {
            LOGGER_WRITE_1("CBookmarkDataStore::DoDeleteItemL, iDb.Delete failed: %d", error);
            }
        
        User::RequestComplete( iCallerStatus, error );
        }
    else
        {
        LOGGER_WRITE( "Delete RSS item" );
        if ( !iRootFolder.HasRootFolderItem() )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "rss root folder not found" );
            LOGGER_LEAVEFN( "CBookmarkDataStore::DoDeleteItemL" );
            return;
            }
        const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
        const CFolderItem* item = rootFolder.Search( aUid - KRssFeedsOffset );
        if ( !item )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound );
            LOGGER_WRITE( "item not found" );
            LOGGER_LEAVEFN( "CBookmarkDataStore::DoDeleteItemL" );
            return;
            }
        
        LOGGER_WRITE_1("founded. item->Id(): %d", item->Id());
        if ( !item->Parent() )
            {
            LOGGER_WRITE_1(
            "Item parent not found, this is root folder. Return error: %d", KErrAccessDenied);
            User::RequestComplete( iCallerStatus, KErrAccessDenied );
            LOGGER_LEAVEFN( "CBookmarkDataStore::DoDeleteItemL" );
            return;
            }
        else
            {
            LOGGER_WRITE( "parent found" );
            }
        
        RPointerArray<const CFolderItem> folderItems;
        CleanupClosePushL( folderItems );
        error = folderItems.Append( item );
        LOGGER_WRITE_1( "folderItems.Append error: %d", error );
        TRAP( error, iRootFolder.DeleteFolderItemsL( folderItems ) );
        CleanupStack::PopAndDestroy( &folderItems );
        LOGGER_WRITE_1( "Trap error: %d", error );
        
        iReplaceItem = aUid;
        iCurrentState = EBookmarkDeleting;
        LOGGER_WRITE( "Wait for FolderItemRequestCompleted.." );
        }
        
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoDeleteItemL" );
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoSoftDeleteItemL
// Soft delete isn't supported.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoSoftDeleteItemL(TSmlDbItemUid /*aUid*/, TRequestStatus& aStatus)
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoSoftDeleteItemL" );
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    User::RequestComplete(iCallerStatus, KErrNotSupported); 
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoSoftDeleteItemL" );  
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoDeleteAllItemsL
// Deletes all items in the standard folders of bookmark store
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoDeleteAllItemsL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoDeleteAllItemsL" );

    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;

    // Check that we're in proper state
    if (iCurrentState != EOpenAndWaiting) 
        {
        LOGGER_WRITE_1( "CSmsDataStore::DoDeleteAllItemsL, Incorrect state: %d", iCurrentState );
        }

    // Reset the whole change finder
    iChangeFinder->ResetL();
    
    // Delete all Bookmark items
    LOGGER_WRITE( "Delete Bookmark items" );
    // Loop through all items at root, and delete everything
    CFavouritesItemList* list = new ( ELeave ) CFavouritesItemList();
    TInt error = iDb.GetAll( *list, KFavouritesRootUid );
    if ( error == KErrNone )  
        {
        // Loop through the list and delete all items
        TInt count = list->Count();
        LOGGER_WRITE_1("list->Count(): %d", count);
        for ( TInt i = 0; i < count; ++i )
            {
            CFavouritesItem* item = (*list)[i];
        
            // Don't bother try to erase items, which are read only
            if ( !item->IsFactoryItem() )
                {
                TInt deleted = iDb.Delete( item->Uid() );
                if ( deleted != KErrNone )
                    {
                    // In the case of error, store the error value
                    error = deleted;
                    LOGGER_WRITE_1("CBookmarkDataStore::DoDeleteAllItemsL, iDb.Delete failed: %d", error);
                    }
                
                LOGGER_WRITE_1("item->Uid(): %d", item->Uid())
                LOGGER_WRITE_1("iDb.Delete ret: %d", error)
                
                }
            else
                {
                LOGGER_WRITE_1("item->IsFactoryItem(): id: %d", item->Uid());
                }
            }
        }
    else
        {
        LOGGER_WRITE_1("CBookmarkDataStore::DoDeleteAllItemsL, iDb.GetAll failed: %d", error);
        }
    delete list;
    
    // Delete all WebFeeds
    LOGGER_WRITE( "Delete RSS items" );
    if ( !iRootFolder.HasRootFolderItem() )
        {
        User::RequestComplete( iCallerStatus, error );
        LOGGER_WRITE( "no RSS items" );
        return;
        }
    const CFolderItem& rootFolder = iRootFolder.RootFolderItem();
    // add subfolders
    RPointerArray<const CFolderItem> folderItems;
    CleanupClosePushL( folderItems );
    for ( TInt i = 0; i < rootFolder.FolderItemCount(); i++)
        {
        error = folderItems.Append( rootFolder.FolderItemAt( i ) );
        LOGGER_WRITE_1( "folderItems.Append error: %d", error );
        }
    
    if ( folderItems.Count() > 0 )
        {
        TRAP( error, iRootFolder.DeleteFolderItemsL( folderItems ) );
        
        LOGGER_WRITE_1( "Trap error: %d", error );
        
        iCurrentState = EBookmarkDeletingAll;
        LOGGER_WRITE( "Wait for FolderItemRequestCompleted.." );
        }
    else
        {
        LOGGER_WRITE( "RSS items not found" );
        User::RequestComplete( iCallerStatus, KErrNone ); 
        }
    CleanupStack::PopAndDestroy( &folderItems );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoDeleteAllItemsL" );          
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoHasSyncHistory
// This method returns ETrue if Data Store has history information. 
// Slow-sync will be used if Data Store does not have history information.
// -----------------------------------------------------------------------------
TBool CBookmarkDataStore::DoHasSyncHistory() const
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoHasSyncHistory" );
    LOGGER_WRITE_1( "iHasHistory: %d", iHasHistory );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoHasSyncHistory" );   
    return iHasHistory;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoAddedItems
// This method returns UIDs of added items. Those items are added after previous
// synchronization with current synchronization relationship. 
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CBookmarkDataStore::DoAddedItems() const
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoAddedItems" );
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoAddedItems, invalid state %d.", iCurrentState );
        }
    
    TInt error(KErrNone);
    
    // Clear new-items array
    iNewItems->Reset();
    
    // Search for new items
    TRAP( error, iChangeFinder->FindNewItemsL( *iNewItems ) )
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoAddedItems, iChangeFinder->FindNewItemsL leaved with %d.", error );
        } 
    
    LOGGER_WRITE_1( "New item count: %d.", iNewItems->ItemCount() );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoAddedItems" );
    
    return *iNewItems;  
    }
    
// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoDeletedItems
// Returns ids of items, which are deleted after previous synchronization
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CBookmarkDataStore::DoDeletedItems() const
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoDeletedItemsL" );    
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoDeletedItems, invalid state %d.", iCurrentState );
        }
    
    TInt error( KErrNone );
    
    // Clear deleted-items array
    iDeletedItems->Reset();
    
    // Search for deleted items
    TRAP( error, iChangeFinder->FindDeletedItemsL( *iDeletedItems ) );
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoDeletedItems, iChangeFinder->FindDeletedItemsL leaved with %d.", error );
        }
    
    LOGGER_WRITE_1( "Deleted item count: %d.", iDeletedItems->ItemCount() );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoDeletedItemsL" );
    return *iDeletedItems;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoSoftDeletedItems
// Not directly supported, empty list returned
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CBookmarkDataStore::DoSoftDeletedItems() const
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoSoftDeletedItems" );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoSoftDeletedItems" );         

    // Return empty array as a result
    iSoftDeletedItems->Reset();
    return *iSoftDeletedItems;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoModifiedItems
// Finds all modified items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CBookmarkDataStore::DoModifiedItems() const
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoModifiedItems" );    
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoModifiedItems, invalid state %d.", iCurrentState );
        }

    TInt error( KErrNone );

    // Clear updated-items array
    iUpdatedItems->Reset();
    
    // Search for updated items
    TRAP( error, iChangeFinder->FindChangedItemsL( *iUpdatedItems ) )
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoModifiedItems, iChangeFinder->FindChangedItemsL leaved with %d.", error );
        }
    
    LOGGER_WRITE_1( "Modified item count: %d.", iUpdatedItems->ItemCount() );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoModifiedItems" );        
    return *iUpdatedItems;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoMovedItems
// Finds all moved items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CBookmarkDataStore::DoMovedItems() const
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoMovedItems" );   
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoMovedItems, invalid state %d.", iCurrentState );
        }       
    
    TInt error( KErrNone );
    
    // Clear moved-items array
    iMovedItems->Reset();
    
    TRAP( error, iChangeFinder->FindMovedItemsL( *iMovedItems ) );
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoMovedItems, iChangeFinder->FindMovedItemsL leaved with %d.", error );
        }
       
    LOGGER_WRITE_1( "Moved item count: %d.", iMovedItems->ItemCount() );
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoMovedItems" );
    return *iMovedItems;
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoResetChangeInfoL
// Resets change history in the data store. All content is considered
// new in the data store point of view.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoResetChangeInfoL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoResetChangeInfoL" );
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoResetChangeInfoL, invalid state %d.", iCurrentState );
        }   
            
    // Reset change info in ChangeFinder
    iChangeFinder->ResetL();
    iHasHistory = EFalse;
    
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );   
        
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoResetChangeInfoL" );     
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCommitChangeInfoL
// Commits change info. These items are no longer reported, when change
// information is being queried.
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus,
    const MSmlDataItemUidSet& aItems )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCommitChangeInfoL(1)" );
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState );
        }

    // Notify ChangeFinder
    LOGGER_WRITE_1( "CBookmarkDataStore::DoCommitChangeInfoL, item count %d.", aItems.ItemCount() );
    iChangeFinder->CommitChangesL( aItems );
    iHasHistory = ETrue;
        
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );
    
    LOGGER_LEAVEFN(" CBookmarkDataStore::DoCommitChangeInfoL(1)" ); 
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::DoCommitChangeInfoL
// Commits change info. There is no more nothing to report when change
// information is being queried. 
// -----------------------------------------------------------------------------
void CBookmarkDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::DoCommitChangeInfoL(2)" );

    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EOpenAndWaiting ) 
        {
        LOGGER_WRITE_1( "CBookmarkDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState );
        }
    
    // Notify ChangeFinder
    iChangeFinder->CommitChangesL();
    iHasHistory = ETrue;
        
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );
        
    LOGGER_LEAVEFN( "CBookmarkDataStore::DoCommitChangeInfoL(2)" );     
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::RegisterSnapshotL
// Sets Changefinder to compare against current bookmark store content
// -----------------------------------------------------------------------------
void CBookmarkDataStore::RegisterSnapshotL()
    {
    LOGGER_ENTERFN( "CBookmarkDataStore::RegisterSnapshotL()" );
    
    CArrayFixSeg<TSnapshotItem>* snapshot = 
        new ( ELeave ) CArrayFixSeg<TSnapshotItem>( KSnapshotGranularity );
    CleanupStack::PushL( snapshot );
    
    
    RegisterToSnapshotL( snapshot, KFavouritesRootUid );
    
    if ( iRootFolder.HasRootFolderItem() )
        {
        RegisterRssFeedsToSnapshotL( snapshot, iRootFolder.RootFolderItem() );
        }
    
    // Set new snapshot to compare against
    iChangeFinder->SetNewSnapshot( snapshot );
    
    // Changefinder takes ownership of the snapshot
    CleanupStack::Pop( snapshot );
    
    LOGGER_LEAVEFN( "CBookmarkDataStore::RegisterSnapshotL()" );
    }


// -----------------------------------------------------------------------------
// CBookmarkDataStore::RegisterToSnapshotL
// Adds recursively all the bookmark items to the snapshot
// -----------------------------------------------------------------------------
void CBookmarkDataStore::RegisterToSnapshotL( CArrayFixSeg<TSnapshotItem>* aSnapshot,
    TInt aFolderUid )
    {
    CFavouritesItemList* list = new ( ELeave ) CFavouritesItemList;
    CleanupStack::PushL( list );
    TInt error = iDb.GetAll( *list, aFolderUid );
    if ( error == KErrNone )
        {
        // Loop through the list. Add folders and items to the snapshot
        TInt count = list->Count();
        for ( TInt i = 0; i < count; ++i )
            {
            CFavouritesItem* item = (*list)[i];
            // No matter whether it's a folder or item, we'll add it to snapshot
            TSnapshotItem snapshotItem( item->Uid() );
            snapshotItem.SetParentId( item->ParentFolder() );
            snapshotItem.CreateHashL( item->Name(), item->Url() );
            TKeyArrayFix key( iKey );
            aSnapshot->InsertIsqL( snapshotItem, key );

            // Remember to add also childs in case of folder (recurse)
            if ( item->Type() == CFavouritesItem::EFolder )
                {
                RegisterToSnapshotL( aSnapshot, item->Uid() );
                }
            }
        }
    CleanupStack::PopAndDestroy( list ); //list
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::RegisterRssFeedsToSnapshotL
// Adds recursively all the rssFeed items to the snapshot
// -----------------------------------------------------------------------------
void CBookmarkDataStore::RegisterRssFeedsToSnapshotL( CArrayFixSeg<TSnapshotItem>* aSnapshot,
    const CFolderItem& folder )
    {
    TSnapshotItem snapshotItem( folder.Id() + KRssFeedsOffset );
    
    const CFolderItem* parent = folder.Parent();
    if ( parent )
        {
        snapshotItem.SetParentId( parent->Id() + KRssFeedsOffset );
        }
    else
        {
        snapshotItem.SetParentId( KErrNotFound );
        }
    snapshotItem.CreateHashL( folder.Name(), folder.SourceUrl() );
    
    TKeyArrayFix key( iKey );
    aSnapshot->InsertIsqL( snapshotItem, key );
    
    // add subfolders
    for ( TInt i = 0; i < folder.FolderItemCount(); i++)
        {
        RegisterRssFeedsToSnapshotL( aSnapshot, *(folder.FolderItemAt( i ) ));
        }
    }

// -----------------------------------------------------------------------------
// CBookmarkDataStore::AssignString
// Assigns data from one descriptor into another, truncates if too long 
// -----------------------------------------------------------------------------
void CBookmarkDataStore::AssignString( TDes8& aDestination, const TDesC8& aSource )
    {
    TInt targetLength = aSource.Length();
    if ( aDestination.MaxLength() < targetLength )
        {
        targetLength = aDestination.MaxLength();
        }
        
    aDestination.Copy( aSource.Ptr(), targetLength );
    }
