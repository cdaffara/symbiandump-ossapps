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


#include <e32base.h>
#include <msvstd.h>
#include <msvapi.h>
#include <msvids.h>
#include <mtclreg.h> 
#include <mmsconst.h>
#include <centralrepository.h>
#include <mmscodecclient.h>
#include <sysutil.h>
#include <MmsEngineInternalCRKeys.h>

#include "mmsdatastore.h"
#include "omadsfolderobject.h"
#include "mmsadaptermsvapi.h"
#include "mmsdataproviderdefs.h"
#include "logger.h"

_LIT8( KMmsMimeType, "application/vnd.wap.mms-message" );
_LIT8( KMmsMimeVersion, "1.2" );
_LIT8( KFolderMimeType, "application/vnd.omads-folder+xml" );
_LIT8( KFolderMimeVersion, "1.2" );

const TInt KDataBufferSize = 1024;
const TUint KMMS_Flag_Read = 0x01;

// -----------------------------------------------------------------------------
// CMmsDataStore::CMmsDataStore
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
CMmsDataStore::CMmsDataStore():
    iHasHistory(EFalse),
    iDataBaseOpened(EFalse),
    iKey(TKeyArrayFix(_FOFF(TSnapshotItem, ItemId()), ECmpTInt))
    { 
    }
  
// -----------------------------------------------------------------------------
// CMmsDataStore::ConstructL
// Symbian 2nd phase constructor, can leave.
// -----------------------------------------------------------------------------
void CMmsDataStore::ConstructL(CMsvSession &aMsvSession)
    {
    LOGGER_ENTERFN("CMmsDataStore::ConstructL");
    
    iMsvSession = &aMsvSession;
    
    // Waiter object to be used with CodecClient
    iMsvWait = CMsvOperationActiveSchedulerWait::NewLC();
    CleanupStack::Pop( iMsvWait );
    iCodecClient = CMmsCodecClient::NewL( *iMsvSession );
    iMsvApi = CMmsAdapterMsvApi::NewL( *iMsvSession );
    
    // Item UID sets, used to transfer change info
    iNewItems = new (ELeave) CNSmlDataItemUidSet;
    iDeletedItems = new (ELeave) CNSmlDataItemUidSet;
    iUpdatedItems = new (ELeave) CNSmlDataItemUidSet;
    iMovedItems = new (ELeave) CNSmlDataItemUidSet;
    iSoftDeletedItems = new (ELeave) CNSmlDataItemUidSet;
    
    iFolderObjectParser = COMADSFolderObject::NewL();
    
    LOGGER_LEAVEFN("CMmsDataStore::ConstructL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
CMmsDataStore* CMmsDataStore::NewL( CMsvSession &aMsvSession)
    {
    CMmsDataStore* self = new (ELeave) CMmsDataStore;
    
    CleanupStack::PushL( self );
    self->ConstructL( aMsvSession );
    CleanupStack::Pop( self );

    return self;    
    }

    
// -----------------------------------------------------------------------------
// CMmsDataStore::~CMmsDataStore
// Destructor
// -----------------------------------------------------------------------------
CMmsDataStore::~CMmsDataStore()
    {
    LOGGER_ENTERFN("CMmsDataStore::~CMmsDataStore()");
    
    delete iDataBuffer;
    
    delete iChangeFinder;
    delete iFolderObjectParser;
    
    delete iNewItems;
    delete iDeletedItems;
    delete iUpdatedItems;
    delete iMovedItems;
    delete iSoftDeletedItems;
 
    delete iMsvApi;
    delete iCodecClient;
    delete iMsvWait;
            
    LOGGER_LEAVEFN("CMmsDataStore::~CMmsDataStore()");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoOpenL
// Opens database. This operation is performed SYNCHRONOUSLY
// -----------------------------------------------------------------------------
void CMmsDataStore::DoOpenL( const TDesC& /*aStoreName*/,
    MSmlSyncRelationship& aContext, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoOpenL");
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    if ( iDataBaseOpened )
        {
        User::RequestComplete( iCallerStatus, KErrInUse );
        LOGGER_WRITE("CMmsDataStore::DoOpenL failed with KErrInUse.");
        return;
        }
    
    *iContext = aContext;

    // Create ChangeFinder object
    if ( iChangeFinder )
        {
        delete iChangeFinder;
        iChangeFinder = NULL;
        }
    iChangeFinder = CChangeFinder::NewL( aContext, iKey, iHasHistory, KMmsDataProviderImplUid );
    
    // Set current snapshot, this will be compared against the old one   
    RegisterSnapshotL();
    
    iDataBaseOpened = ETrue;
    iCurrentState = EMmsOpenAndWaiting;
    User::RequestComplete( iCallerStatus, KErrNone );   
    
    LOGGER_LEAVEFN("CMmsDataStore::DoOpenL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoCancelRequest
// Not supported, does nothing.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoCancelRequest()
    {
    LOGGER_ENTERFN("CMmsDataStore::DoCancelRequestL");
    LOGGER_LEAVEFN("CMmsDataStore::DoCancelRequestL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoStoreName
// Returns the name of the DataStore
// -----------------------------------------------------------------------------
const TDesC& CMmsDataStore::DoStoreName() const
    {
    LOGGER_ENTERFN("CMmsDataStore::DoStoreName");
    
    if ( iDataBaseOpened )
        {
        LOGGER_LEAVEFN( "CMmsDataStore::DoStoreName" );
        LOGGER_WRITE_1( "Database name: %S", &KNSmlDefaultLocalDbName );
        return KNSmlDefaultLocalDbName;
        }

    LOGGER_LEAVEFN( "CMmsDataStore::DoStoreName" );
    return KNullDesC;
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoBeginTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoBeginTransactionL()
    {
    LOGGER_ENTERFN("CMmsDataStore::DoBeginTransactionL");
    LOGGER_WRITE( "CMmsDataStore::DoBeginTransactionL leaved with KErrNotSupported." );
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoCommitTransactionL
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoCommitTransactionL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CMmsDataStore::DoCommitTransactionL" ); 
    LOGGER_WRITE( "CMmsDataStore::DoCommitTransactionL failed with KErrNotSupported." );
    
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoRevertTransaction
// Transactions are not supported.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoRevertTransaction( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CMmsDataStore::DoRevertTransaction" ); 
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    LOGGER_LEAVEFN( "CMmsDataStore::DoRevertTransaction" );
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoBeginBatchL
// Batching is not supported.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoBeginBatchL()
    {
    LOGGER_ENTERFN( "CMmsDataStore::DoBeginBatchL" );
    LOGGER_WRITE( "CMmsDataStore::DoBeginBatchL leaved with KErrNotSupported." );
    User::Leave( KErrNotSupported );    
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoCommitBatchL
// Batching is not supported
// -----------------------------------------------------------------------------
//
void CMmsDataStore::DoCommitBatchL( RArray<TInt>& /*aResultArray*/, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CMmsDataStore::DoCommitBatchL" );  
    LOGGER_WRITE( "CMmsDataStore::DoCommitBatchL failed with KErrNotSupported" );
    iCallerStatus = &aStatus;
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoCancelBatch
// Batching is not supported
// -----------------------------------------------------------------------------
void CMmsDataStore::DoCancelBatch()
    {
    LOGGER_ENTERFN( "CMmsDataStore::DoCancelBatch" );
    LOGGER_LEAVEFN( "CMmsDataStore::DoCancelBatch" );
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoSetRemoteStoreFormatL
// Not supported
// -----------------------------------------------------------------------------
//
void CMmsDataStore::DoSetRemoteStoreFormatL( const CSmlDataStoreFormat& /*aServerDataStoreFormat*/ )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoSetRemoteStoreFormatL");
    LOGGER_LEAVEFN("CMmsDataStore::DoSetRemoteStoreFormatL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoSetRemoteMaxObjectSize
// Not supported
// -----------------------------------------------------------------------------
void CMmsDataStore::DoSetRemoteMaxObjectSize( TInt /*aServerMaxObjectSize*/ )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoSetRemoteMaxObjectSize");
    LOGGER_LEAVEFN("CMmsDataStore::DoSetRemoteMaxObjectSize");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoMaxObjectSize
// Reads the maximum MMS Message size from the central repository
// -----------------------------------------------------------------------------
TInt CMmsDataStore::DoMaxObjectSize() const
    {
    LOGGER_ENTERFN( "CMmsDataStore::DoMaxObjectSize" );
    
    CRepository* repository( NULL );
    TInt error( KErrNone );
    TInt maxSendSize( 0 );
    
    // Create central repository instance
    TRAP( error, repository = CRepository::NewL( KCRUidMmsEngine ) );  
    if ( error == KErrNone )
        {
        // Obtain the size from the central repository.
        // In the case of error we'll set the value to zero ("anything goes").
        error = repository->Get( KMmsEngineMaximumSendSize, maxSendSize );
        if ( error != KErrNone )
            {
            maxSendSize = 0;
            }
        
        delete repository;
        }
    else
        {
        LOGGER_WRITE_1( "CRepository::NewL leaved with %d", error );
        }       
        
    LOGGER_LEAVEFN( "CMmsDataStore::DoMaxObjectSize" );
    return maxSendSize;
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoOpenItemL
// Opens item in the DataStore, reads it (either completely or partially) 
// to the temporary buffer where it can be later read to the remote database.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoOpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange, 
    TInt& aSize, TSmlDbItemUid& aParent, TDes8& aMimeType, 
    TDes8& aMimeVer, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CMmsDataStore::DoOpenItemL" );

    LOGGER_WRITE_1( "Opening item %d.", aUid );
    
    // Store these for later use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in a proper state
    if ( iCurrentState != EMmsOpenAndWaiting )
        {
        LOGGER_WRITE_1( "CMmsDataStore::DoOpenItemL, invalid state %d.", iCurrentState );
        User::RequestComplete( iCallerStatus, KErrNotReady );
        return;
        }
        
    TBool userFolderFound( EFalse );
    TTime timeStamp;
    TPtrC folderName;
    userFolderFound = iMsvApi->FindUserFolderL( aUid, folderName, timeStamp );
    
    if ( userFolderFound )
        {
        // Allocate new buffer
        SAFEDELETE( iDataBuffer );
        iDataBuffer = CBufFlat::NewL( KDataBufferSize );
        
        iFolderObjectParser->SetName( folderName );
        iFolderObjectParser->SetCreatedDate( timeStamp.DateTime() );
        iFolderObjectParser->SetModifiedDate( timeStamp.DateTime() );
        iFolderObjectParser->ExportFolderXmlL( *iDataBuffer ); 
       
        iParentId = KMsvMyFoldersEntryIdValue;
        
        iCurrentState = EFolderOpen;
        iReadPosition = 0;
        
        aSize = iDataBuffer->Size();
        }
    else // Open MMS message
        {
        TInt error( KErrNone );
        
        CMsvEntry* entry( NULL );
        TRAP( error, entry = iMsvSession->GetEntryL(aUid) );
        if ( error != KErrNone )
            {
            User::RequestComplete( iCallerStatus, KErrNotFound ); 
            LOGGER_WRITE_1("iMsvSession->GetEntryL failed with %d.", error);
            return;
            }
        
        TMsvEntry messageEntry = entry->Entry();
        SAFEDELETE( entry );
    
        iCurrentId = aUid;
        iParentId = messageEntry.Parent();
        iReadCounter = 0;
    
        // Check whether we need to send the whole item
        if ( iChangeFinder->UpdatePartialL( aUid ) )
            {
            LOGGER_WRITE("EMmsItemOpenFieldUpdate");
            aSize = 1;
            iCurrentState = EMmsItemOpenFieldUpdate;
            }
        else    
            {
            // Read the whole item from the message store to the buffer
            TUint32 flags( 0 );
            TRAP( error, iCodecClient->InitializeChunkedRetrievingL(
                iCurrentId,
                iParentId,
                flags,
                iUnread,
                aSize,
                iMsvWait->iStatus) );
            
            if ( error != KErrNone ) 
                {
                User::RequestComplete( iCallerStatus, error );
                LOGGER_WRITE_1("iCodecClient->InitializeChunkedRetrievingL failed with %d.", error); 
                return;         
                }
            
            // Wait until the message has been processed
            iMsvWait->Start();
            
            if ( iMsvWait->iStatus != KErrNone )
                {
                User::RequestComplete( iCallerStatus, iMsvWait->iStatus.Int() );
                LOGGER_WRITE_1( "iCodecClient->InitializeChunkedRetrievingL failed with %d",
                    iMsvWait->iStatus.Int() ); 
                return;
                }
            LOGGER_WRITE_1("iUnread: %d", (TInt)iUnread);
            aSize++; // Status byte will be added also, reserve one additional byte for that.
            iCurrentState = EMmsItemOpen;
            }
        } // Open MMS message
    
    aParent = iParentId;
    
    aFieldChange = iCurrentState == EMmsItemOpenFieldUpdate ? ETrue : EFalse;
    
    if ( iCurrentState == EFolderOpen ) // Message folder
        {
        TInt targetLength = KFolderMimeType().Length();
        if ( aMimeType.MaxLength() < targetLength )
            {
            targetLength = aMimeType.MaxLength();
            }
        aMimeType.Copy( KFolderMimeType().Ptr(), targetLength );

        // Set mime version (do not exceed the allocated buffer)
        targetLength = KFolderMimeVersion().Length();
        if ( aMimeVer.MaxLength() < targetLength )
            {
            targetLength = aMimeVer.MaxLength();
            }
        aMimeVer.Copy( KFolderMimeVersion().Ptr(), targetLength );
        }
    else // EMmsMessage
        {   
        TInt targetLength = KMmsMimeType().Length();
        if ( aMimeType.MaxLength() < targetLength )
            {
            targetLength = aMimeType.MaxLength();
            }
        aMimeType.Copy( KMmsMimeType().Ptr(), targetLength );

        // Set mime version (do not exceed the allocated buffer)
        targetLength = KMmsMimeVersion().Length();
        if ( aMimeVer.MaxLength() < targetLength )
            {
            targetLength = aMimeVer.MaxLength();
            }
        aMimeVer.Copy( KMmsMimeVersion().Ptr(), targetLength );
        }
    
    LOGGER_WRITE_1("aSize: %d", aSize);
    
    // Signal we're complete
    User::RequestComplete( iCallerStatus, KErrNone ); 

    LOGGER_LEAVEFN("CMmsDataStore::DoOpenItemL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoCreateItemL
// Create new item to the message store.
// Return the id number of the newly created item
// -----------------------------------------------------------------------------
void CMmsDataStore::DoCreateItemL( TSmlDbItemUid& aUid, TInt aSize, TSmlDbItemUid aParent, 
    const TDesC8& aMimeType, const TDesC8& /*aMimeVer*/, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN( "CMmsDataStore::DoCreateItemL" );
    LOGGER_WRITE_1( "Parent folder: %d.", aParent );
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in proper state
    if ( iCurrentState != EMmsOpenAndWaiting )
        {
        LOGGER_WRITE_1( "Invalid state %d.", iCurrentState );
        }
        
    TBool createFolder( EFalse );
    LOG( aMimeType );
    if ( aMimeType.Compare( KFolderMimeType() ) == 0 )  
        {
        createFolder = ETrue;
        }
    else if ( aMimeType.Compare( KMmsMimeType() ) != 0 )
        {
        User::RequestComplete( iCallerStatus, KErrNotSupported );
        LOGGER_WRITE("Bad MIME type");
        return;
        }
    
    // Ensure that we've got enough disk space for the item
    if ( iCodecClient->DiskSpaceBelowCriticalLevelL( aSize ) )
        {
        User::RequestComplete( iCallerStatus, KErrDiskFull );
        LOGGER_WRITE( "Disk full" );
        return;
        }
        
    if( createFolder )
        {
        if ( aParent != KMsvMyFoldersEntryIdValue )
            {
            User::RequestComplete( iCallerStatus, KErrNotSupported );
            LOGGER_WRITE( "Bad parent folder" );
            return;
            }
        SAFEDELETE( iDataBuffer );
        iDataBuffer = CBufFlat::NewL( KDataBufferSize );
        iCurrentState = EFolderCreating;
        iCreatedUid = &aUid;
        iWrittenDataLength = 0;
        }
    else
        {
        // There is some problems on chunked data adding, so get all data to internal buffer
        iCreatedUid = &aUid;
        iCurrentState = EMmsItemCreating;
        iWriteCounter = 0;
        iWrittenDataLength = 0;
        if ( iDataBuffer )
            {
            iDataBuffer->ResizeL( aSize );
            }
        else
            {
            iDataBuffer = CBufFlat::NewL( KDataBufferSize );
            iDataBuffer->ResizeL( aSize );
            }
        }
        
    iParentId = aParent; 
    
    // Signal we're complete
    User::RequestComplete( iCallerStatus, KErrNone );
    
    LOGGER_LEAVEFN("CMmsDataStore::DoCreateItemL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoReplaceItemL
// Begin the replace operation, ensure that the item really exists
// -----------------------------------------------------------------------------
void CMmsDataStore::DoReplaceItemL( TSmlDbItemUid aUid, TInt aSize, TSmlDbItemUid aParent, 
    TBool /*aFieldChange*/, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoReplaceItemL");
    LOGGER_WRITE_1("Replacing item %d.", aUid);
    LOGGER_WRITE_1("Parent folder: %d.", aParent);
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure proper state
    if ( iCurrentState != EMmsOpenAndWaiting )
        {
        LOGGER_WRITE_1("Invalid state %d.", iCurrentState);
        }

    // Ensure that we've got enough disk space for the item
    if ( iCodecClient->DiskSpaceBelowCriticalLevelL( aSize ) )
        {
        User::RequestComplete( iCallerStatus, KErrDiskFull );
        LOGGER_WRITE("Disk full");
        return;
        }
             
    // Find entry
    CMsvEntry* entry(NULL);
    TRAPD( err, entry = iMsvSession->GetEntryL( aUid ) );
    if ( err != KErrNone )
        {
        User::RequestComplete( iCallerStatus, KErrNotFound );
        LOGGER_WRITE_1("CMsvSession::GetEntryL failed with %d.", err)
        return;
        }

    TMsvEntry tEntry = entry->Entry();
    delete entry;
    
    // Check entry type
    TBool updateFolder(EFalse);
    if ( tEntry.iType == KUidMsvFolderEntry )
        {
        updateFolder = ETrue;
        LOGGER_WRITE("Type: folder");
        }
        
   if ( ( updateFolder && aParent != KMsvMyFoldersEntryIdValue )
        || ( !updateFolder && !iMsvApi->ValidFolderL( aParent )
        || ( aParent != tEntry.Parent() ) ) )
        {
        User::RequestComplete( iCallerStatus, KErrNotSupported );
        LOGGER_WRITE_1("Bad parent folder, message entry parent is %d", tEntry.Parent());
        return;    
        }           
    
    // Store these for further use
    iParentId = aParent;
    iCurrentId = aUid;
    
    if ( updateFolder )
        {
        SAFEDELETE( iDataBuffer );
        iDataBuffer = CBufFlat::NewL( KDataBufferSize );
        iCurrentState = EFolderUpdating;
        iWrittenDataLength = 0;
        }
    else
        {
        iCurrentState = EMmsItemUpdating;
        iWriteCounter = 0;
        iWrittenDataLength = 0;
        if ( iDataBuffer )
            {
            iDataBuffer->ResizeL( aSize );
            }
        else
            {
            iDataBuffer = CBufFlat::NewL( KDataBufferSize );
            iDataBuffer->ResizeL( aSize );
            }
        }
    
    // Signal we're complete
    User::RequestComplete( iCallerStatus, KErrNone );

    LOGGER_LEAVEFN("CMmsDataStore::DoReplaceItemL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoReadItemL
// Read specified amount of data from the temporary buffer
// -----------------------------------------------------------------------------
void CMmsDataStore::DoReadItemL( TDes8& aBuffer )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoReadItemL");
    
    if ( iCurrentState == EFolderOpen )
        {   
        // This is how much we've got left in the buffer
        TInt left = iDataBuffer->Size() - iReadPosition;
    
        // Make sure that there's something to read
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
            LOGGER_WRITE("All data read");
            User::Leave( KErrEof );
            }
        }
    
    else if ( iCurrentState == EMmsItemOpenFieldUpdate )
        {
        if ( iReadCounter++ == 0 )
            {
            TUint8 status = ResolveStatusBits( iUnread );
            LOGGER_WRITE_1("WriteStatusBits: %d", status);
            aBuffer.Append( &status, 1 );
            }
        else
            {
            LOGGER_WRITE("Field update done");
            User::Leave( KErrEof );             
            }   
        }
        
    else if ( iCurrentState == EMmsItemOpen )
        {
        if ( iReadCounter++ == 0 )
            {
            TUint8 status = ResolveStatusBits( iUnread );
            LOGGER_WRITE_1("WriteStatusBits: %d", status);
            aBuffer.Append( &status, 1 );
            iReadPosition = 0;
            iLastDataChunk = EFalse;
            iReadAllData = EFalse;
            }
        else if ( iReadAllData )
            {
            User::Leave( KErrEof );
            }
            
        TInt error = ReadDataRecursively( aBuffer );
        if ( error != KErrNone )
            {
            User::Leave( error );
            }           
        }
    
    else
        {
        LOGGER_WRITE_1("CMmsDataStore::DoReadItemL: bad state %d", iCurrentState);
        User::Leave( KErrNotReady );
        }   

    LOGGER_LEAVEFN("CMmsDataStore::DoReadItemL");   
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoWriteItemL
// Write specified amount of data to the temporary buffer
// -----------------------------------------------------------------------------
void CMmsDataStore::DoWriteItemL( const TDesC8& aData )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoWriteItemL");
    LOGGER_WRITE_1("%d",iWriteCounter);
    
    TInt dataLength = aData.Length();
    LOGGER_WRITE_1("Data length: %d", dataLength);
    
    if ( !( dataLength > 0 ) ) // Should never happen...
        {
        LOGGER_WRITE("Error: no data");
        User::Leave( KErrArgument );
        }

    if ( iCodecClient->DiskSpaceBelowCriticalLevelL( dataLength ) )
        {
        LOGGER_WRITE("Error: disk full");
        User::Leave( KErrDiskFull );
        }
        
    TInt error( KErrNone );
    
    if ( iCurrentState == EFolderCreating || iCurrentState == EFolderUpdating )
        {
        // Add data to buffer
        iDataBuffer->InsertL( iWrittenDataLength, aData );
        iWrittenDataLength += aData.Size();
        }
      
    else if ( iCurrentState == EMmsItemCreating )
        {
        if ( iWriteCounter++ == 0 )
            {
            iUnread = aData[0] & KMMS_Flag_Read ? EFalse : ETrue;
            if ( dataLength > 1 )
                {
                TPtrC8 data = aData.Mid(1);
                iDataBuffer->Write( iWrittenDataLength, data );
                iWrittenDataLength += data.Length();
                }
            }
        else
            {
            TPtrC8 data = aData.Mid(0);
            iDataBuffer->Write( iWrittenDataLength, data );
            iWrittenDataLength += dataLength;
            }    
        }
        
    else if ( iCurrentState == EMmsItemUpdating )
        {
        if ( iWriteCounter++ == 0 )
            {
            iUnread = aData[0] & KMMS_Flag_Read ? EFalse : ETrue;
            if ( dataLength > 1 )
                {
                TPtrC8 data = aData.Mid(1);
                iDataBuffer->Write( iWrittenDataLength, data );
                iWrittenDataLength += data.Length();
                }
            else // just status update
                {
                UpdateMmsStatusL( iCurrentId, iUnread );
                LOGGER_WRITE_1("Message status updated, iUnread: %d", iUnread);
                }    
            }
        else
            {
            TPtrC8 data = aData.Mid(0);    
            iDataBuffer->Write( iWrittenDataLength, data );
            iWrittenDataLength += dataLength;
            }    
        }
        
    else
        {
        LOGGER_WRITE_1("Wrong state %d", iCurrentState);
        User::Leave( KErrNotReady );
        }
        
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1("iCodecClient->NextDataPart() failed with %d", error);
        User::Leave( error );
        }  

    LOGGER_LEAVEFN("CMmsDataStore::DoWriteItemL");  
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoCommitItemL
// Commits item from temporary buffer to the message store
// -----------------------------------------------------------------------------
void CMmsDataStore::DoCommitItemL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoCommitItemL");
    
    // Store some variables
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    TInt error(KErrNone);
    
    if ( iCurrentState == EFolderCreating || iCurrentState == EFolderUpdating )
        {
        error = iFolderObjectParser->ImportFolderXml( iDataBuffer->Ptr(0) );
        if ( error != KErrNone )
            {
            User::RequestComplete( iCallerStatus, error );
            LOGGER_WRITE_1("ImportFolderXml failed with %d", error);
            return;
            }
         
         const TDesC& name = iFolderObjectParser->GetName();
         if ( name.Length() <= 0 )
            {
            User::RequestComplete( iCallerStatus, KErrNotSupported );
            LOGGER_WRITE("Folder name is empty");
            return;
            }
            
         if ( iCurrentState == EFolderCreating )
            {
            TMsvId id;
            error = iMsvApi->AddUserFolderL( id, name );
            if ( error == KErrNone )
                {
                *iCreatedUid = id;
                iCurrentId = id;
                }
            else
                {
                LOGGER_WRITE_1("iMsvApi->AddFolderL failed with %d", error);
                }    
            }
         else
            {
            error = iMsvApi->UpdateUserFolderL( iCurrentId, name );
            if ( error != KErrNone )
                {
                LOGGER_WRITE_1("iMsvApi->UpdateFolderL failed with %d", error);
                }
            }
        }
    else if ( iCurrentState == EMmsItemCreating )
        {
        LOGGER_WRITE("Create MMS item");
        TMsvId newId(0);
        TUint32 flags(0);
        
        error = iCodecClient->CreateNewMessageEntryL( iParentId, newId );
        if ( !error )
            {
            iCodecClient->AddMML( *iDataBuffer, iParentId, flags, iUnread, newId, iMsvWait->iStatus );
            // Wait until the message has been processed
            iMsvWait->Start();
            error = iMsvWait->iStatus.Int();
            LOGGER_WRITE_1("error: %d", error);
            LOGGER_WRITE_1("AddMML newId: %d", newId);
            *iCreatedUid = newId;
            iCurrentId = newId;
            }
        }
    else if ( iCurrentState == EMmsItemUpdating )
        {
        if ( iWrittenDataLength > 0 ) // if no data then just field update
            {
            TUint32 flags(0);
            iCodecClient->ReplaceMML( iCurrentId, *iDataBuffer, flags, iUnread, iMsvWait->iStatus );
            iMsvWait->Start();
            error = iMsvWait->iStatus.Int();
            }
        else
            {
            UpdateMmsStatusL( iCurrentId, iUnread );   
            }
        }
    else
        {
        User::RequestComplete( iCallerStatus, KErrNotSupported );
        LOGGER_WRITE_1("Bad state: %d", iCurrentState);
        return;
        }
    
    delete iDataBuffer;
    iDataBuffer = NULL;
    
    if ( error == KErrNone ) // Update Change Finder
        {
        TMsvId service;
        TMsvEntry msgEntry;
        
        // Inform ChangeFinder of added item
        TSnapshotItem snapshotItem( iCurrentId, iParentId, iUnread );
        error = iMsvSession->GetEntry( iCurrentId, service, msgEntry );
        
        if ( error == KErrNone )
            {
            snapshotItem.SetLastChangedDate( msgEntry.iDate );
            if ( iCurrentState == EFolderCreating || iCurrentState == EFolderUpdating )
                {
                snapshotItem.SetFolderNameL( msgEntry.iDetails );
                }
            
            if ( iCurrentState == EFolderCreating || iCurrentState == EMmsItemCreating )
                {
                iChangeFinder->ItemAddedL( snapshotItem );
                }
            else
                {
                iChangeFinder->ItemUpdatedL( snapshotItem );
                }
            }
        else
            {
            LOGGER_WRITE_1( "CMsvSession::GetEntry failed with %d", error );
            }
        }
    
    // Send message if parent folder is Outbox
    if ( iParentId == KMsvGlobalOutBoxIndexEntryId &&
        iCurrentState == EMmsItemCreating &&
        error == KErrNone )
        {
        LOGGER_WRITE("Sending message...");
        iCodecClient->SendMML( iCurrentId, iMsvWait->iStatus );
        iMsvWait->Start();
        error = iMsvWait->iStatus.Int();
        }
    
    LOGGER_WRITE_1("error: %d", error);
    // We'll be waiting for next event, signal we're done
    iCurrentState = EMmsOpenAndWaiting;
    User::RequestComplete( iCallerStatus, error );
    
    LOGGER_LEAVEFN("CMmsDataStore::DoCommitItemL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoCloseItem
// Closes open item in the data store
// -----------------------------------------------------------------------------
void CMmsDataStore::DoCloseItem()
    {
    LOGGER_ENTERFN("CMmsDataStore::DoCloseItem");
    SAFEDELETE(iDataBuffer);
    if ( iCurrentState == EFolderOpen )
        {
        iCurrentState = EMmsOpenAndWaiting;
        }
    else if ( iCurrentState == EMmsItemOpen )
        {
        iCodecClient->ReleaseData();
        iCurrentState = EMmsOpenAndWaiting; 
        }
    else if ( iCurrentState == EMmsItemOpenFieldUpdate )
        {
        iCurrentState = EMmsOpenAndWaiting;
        }
    else 
        {
        LOGGER_WRITE_1("Invalid state %d.", iCurrentState);
        }
    
    LOGGER_LEAVEFN("CMmsDataStore::DoCloseItem");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoMoveItemL
// Moves item from one folder to another in the message store
// -----------------------------------------------------------------------------
void CMmsDataStore::DoMoveItemL( TSmlDbItemUid aUid,
    TSmlDbItemUid aNewParent, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoMoveItemL");
    
    LOGGER_WRITE_1("Moving item %d.", aUid);
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;

    // Check that we're in proper state
    if ( iCurrentState != EMmsOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoMoveItemL, invalid state %d.", iCurrentState);
        }

    // Ensure that we have this item in the message store   
    if ( !MmsItemExists( aUid ) )
        {
        User::RequestComplete( iCallerStatus, KErrNotSupported ); 
        LOGGER_WRITE("MMS item not found");
        return;
        }
    
    iCodecClient->MoveMML( aUid, aNewParent, iMsvWait->iStatus );
    iMsvWait->Start();  
    
    // Inform ChangeFinder of the moved item
    TMsvId service;
    TMsvEntry msgEntry;
    User::LeaveIfError( iMsvSession->GetEntry( aUid, service, msgEntry ) );
    TBool unread = msgEntry.Unread();
    TSnapshotItem snapshotItem( aUid, aNewParent, unread );
    iChangeFinder->ItemMovedL( snapshotItem );

    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );
    
    LOGGER_LEAVEFN("CMmsDataStore::DoMoveItemL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoDeleteItemL
// Removes item from the message store
// -----------------------------------------------------------------------------
void CMmsDataStore::DoDeleteItemL( TSmlDbItemUid aUid, TRequestStatus& aStatus  )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoDeleteItemL");
    LOGGER_WRITE_1("Deleting item %d.", aUid);
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    TInt error(KErrNone);
    
    // Check that we're in proper state
    if ( iCurrentState != EMmsOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoDeleteItemL, invalid state %d.", iCurrentState);        
        }
        
    // Check if this is a user folder
    if ( iMsvApi->FindUserFolderL( aUid ) )
        {
        LOGGER_WRITE("Folder");
        error = DeleteAllMessagesInFolderL( aUid );
        if ( error != KErrNone )
            {
            User::RequestComplete( iCallerStatus, error );    
            LOGGER_WRITE_1("Deleting MMS messages in folder failed with %d", error); 
            return;
            }
        error = iMsvApi->DeleteUserFolderL(aUid);  
        if ( error != KErrNone )
            {
            // Note: folder is not deleted if contains other message items (like MMS)
            // In this case DeleteUserFolderL returns KErrInUse.    
            LOGGER_WRITE_1("Deleting folder failed with %d", error); 
            }       
        }
    else if ( MmsItemExists( aUid ) )
        {
        // Tell CodecClient to delete this message
        error = iCodecClient->DeleteMM( aUid );
        if ( error != KErrNone )
            {
            User::RequestComplete( iCallerStatus, error );    
            LOGGER_WRITE_1("CMmsCodecClient::DeleteMM failed with %d", error);   
            return;
            }
        // Inform ChangeFinder of the removed item
        iChangeFinder->ItemDeletedL( aUid );
        }
    else
        {
        User::RequestComplete( iCallerStatus, KErrNotFound ); 
        LOGGER_WRITE_1("Item %d is not folder or MMS message", aUid);
        return;
        }
    
    LOGGER_WRITE_1("complete error: %d", error);
    // Signal we're done
    User::RequestComplete( iCallerStatus, error );
    LOGGER_LEAVEFN("CMmsDataStore::DoDeleteItemL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoSoftDeleteItemL
// Soft delete isn't supported.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoSoftDeleteItemL( TSmlDbItemUid /*aUid*/, TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoSoftDeleteItemL"); 
    
    // Store some variables for further use
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;

    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNotSupported );
    
    LOGGER_LEAVEFN("CMmsDataStore::DoSoftDeleteItemL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoDeleteAllItemsL
// Deletes all items in the standard folders of message store
// -----------------------------------------------------------------------------
void CMmsDataStore::DoDeleteAllItemsL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoDeleteAllItemsL");
    
    // Store some variables for further use 
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
   // Check that we're in proper state
    if ( iCurrentState != EMmsOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoDeleteAllItemsL, invalid state %d.", iCurrentState);
        }
        
    TInt error(KErrNone);
    TInt result(KErrNone);      
    
    // Delete all messages in the standard folders (except outbox)
    error = DeleteAllMessagesInFolderL( KMsvGlobalInBoxIndexEntryId );
    if ( error != KErrNone )
        {
        result = error;
        }
       
    error = DeleteAllMessagesInFolderL( KMsvDraftEntryId );
    if ( error != KErrNone )
        {
        result = error;
        }   
    
    error = DeleteAllMessagesInFolderL( KMsvSentEntryId );
    if ( error != KErrNone )
        {
        result = error;
        }
        
    error = CleanUserFoldersL();
    if ( error != KErrNone )
        {
        result = error;
        }
            
    iChangeFinder->ResetL();
    
    User::RequestComplete( iCallerStatus, result );
    
    LOGGER_LEAVEFN("CMmsDataStore::DoDeleteAllItemsL");
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DeleteAllMessagesInFolderL
// Deletes all items in the specified folder in message store
// -----------------------------------------------------------------------------
TInt CMmsDataStore::DeleteAllMessagesInFolderL( TMsvId aId )
    {
    LOGGER_ENTERFN("CMmsDataStore::DeleteAllMessagesInFolderL");
    LOGGER_WRITE_1("Folder: %d", aId);
    
    TInt error(KErrNone);
    
    // Get the root folder
    CMsvEntry* msvEntry = iMsvSession->GetEntryL(aId);
    CleanupStack::PushL(msvEntry);
    
    // Find all of it's childs
    CMsvEntrySelection* messages = msvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
    CleanupStack::PopAndDestroy(msvEntry);
    CleanupStack::PushL(messages);
        
    TMsvId service;
    TMsvEntry msg;
    TMsvId id;
    
    // We are only interested of the MM content
    for ( TInt index=0; index < messages->Count(); index++ )
        {
        id = messages->At( index );
        LOGGER_WRITE_1("Message item %d:", id);
        
        error = iMsvSession->GetEntry( id, service, msg );
        if ( error != KErrNone )
            {
            LOGGER_WRITE_1("GetEntry failed with %d", error);
            break;
            }
        
        if ( msg.iMtm == KUidMsgTypeMultimedia )
            {
            error = iCodecClient->DeleteMM( id );
            if ( error != KErrNone )
                {
                LOGGER_WRITE_1("DeleteMM failed with %d", error);
                break;
                }
            // Update Change Finder
            iChangeFinder->ItemDeletedL( id );
            LOGGER_WRITE("MMS message deleted");    
            }
        }
    CleanupStack::PopAndDestroy(messages); 
    
    LOGGER_LEAVEFN("CMmsDataStore::DeleteAllMessagesInFolderL");

    return error;
    }


// -----------------------------------------------------------------------------
// CMmsDataStore::DoHasSyncHistory
// This method returns ETrue if Data Store has history information. 
// Slow-sync will be used if Data Store does not have history information.
// -----------------------------------------------------------------------------
TBool CMmsDataStore::DoHasSyncHistory() const
    {
    LOGGER_ENTERFN("CMmsDataStore::DoHasSyncHistory");
    LOGGER_LEAVEFN("CMmsDataStore::DoHasSyncHistory");      
        
    // iHasHistory is initialized in DoOpenL method
    return iHasHistory;
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoAddedItems
// This method returns UIDs of added items. Those items are added after previous
// synchronization with current synchronization relationship. 
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMmsDataStore::DoAddedItems() const
    {
    LOGGER_ENTERFN("CMmsDataStore::DoAddedItems");  
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EMmsOpenAndWaiting )
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
    LOGGER_LEAVEFN("CMmsDataStore::DoAddedItems");      
    
    return *iNewItems;
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoDeletedItems
//
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMmsDataStore::DoDeletedItems() const
    {
    LOGGER_ENTERFN("CMmsDataStore::DoDeletedItemsL");   
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EMmsOpenAndWaiting )
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
    LOGGER_LEAVEFN("CMmsDataStore::DoDeletedItemsL");
    return *iDeletedItems;
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoSoftDeletedItems
// Not directly supported, equals to "hard" delete
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMmsDataStore::DoSoftDeletedItems() const
    {
    LOGGER_ENTERFN("CMmsDataStore::DoSoftDeletedItems");
    LOGGER_LEAVEFN("CMmsDataStore::DoSoftDeletedItems");

    iSoftDeletedItems->Reset();
    return *iSoftDeletedItems;
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoModifiedItems
// Finds all modified items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMmsDataStore::DoModifiedItems() const
    {
    LOGGER_ENTERFN("CMmsDataStore::DoModifiedItems");   
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EMmsOpenAndWaiting )
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
    LOGGER_LEAVEFN("CMmsDataStore::DoModifiedItems");       
    return *iUpdatedItems;
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoMovedItems
// Finds all moved items in the data store
// -----------------------------------------------------------------------------
const MSmlDataItemUidSet& CMmsDataStore::DoMovedItems() const
    {
    LOGGER_ENTERFN("CMmsDataStore::DoMovedItems");  
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EMmsOpenAndWaiting )
        {
        LOGGER_WRITE_1("CMmsDataStore::DoMovedItems, invalid state %d.", iCurrentState);
        }
    
    TInt error(KErrNone);
    
    // Clear moved-items array
    iMovedItems->Reset();
    
    // Search for moved items
    TRAP( error, iChangeFinder->FindMovedItemsL( *iMovedItems ) );
    if ( error != KErrNone )
        {
        LOGGER_WRITE_1("CMmsDataStore::DoMovedItems, iChangeFinder->FindMovedItemsL leaved with %d.", error);
        }
    
    LOGGER_WRITE_1("Moved item count: %d.", iMovedItems->ItemCount());
    LOGGER_LEAVEFN("CMmsDataStore::DoMovedItems");
    return *iMovedItems;    
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::DoResetChangeInfoL
// Resets change history in the data store. All content is considered
// new in the data store point of view.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoResetChangeInfoL( TRequestStatus& aStatus )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoResetChangeInfoL");    
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Check that we're in proper state
    if ( iCurrentState != EMmsOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoResetChangeInfoL, invalid state %d.", iCurrentState);
        }   
            
    // Reset change info in ChangeFinder
    iChangeFinder->ResetL();
    iHasHistory = EFalse;
    
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone ); 
    
    LOGGER_LEAVEFN("CMmsDataStore::DoResetChangeInfoL");
    }
        
// -----------------------------------------------------------------------------
// CMmsDataStore::DoCommitChangeInfoL
// Commits change info. These items are no longer reported, when change
// information is being queried.
// -----------------------------------------------------------------------------
void CMmsDataStore::DoCommitChangeInfoL( TRequestStatus& aStatus, const MSmlDataItemUidSet& aItems )
    {
    LOGGER_ENTERFN("CMmsDataStore::DoCommitChangeInfoL");
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EMmsOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState);
        }

    // Notify ChangeFinder
    iChangeFinder->CommitChangesL(aItems);
    iHasHistory = ETrue;
        
    // Signal we're done
    User::RequestComplete(iCallerStatus, KErrNone);
        
    LOGGER_LEAVEFN("CMmsDataStore::DoCommitChangeInfoL");
    }
        
    
// -----------------------------------------------------------------------------
// CMmsDataStore::DoCommitChangeInfoL
// Commits change info. There is no more nothing to report when change
// information is being queried. 
// -----------------------------------------------------------------------------
void CMmsDataStore::DoCommitChangeInfoL(TRequestStatus& aStatus)
    {
    LOGGER_ENTERFN("CMmsDataStore::DoCommitChangeInfoL");
    
    iCallerStatus = &aStatus;
    *iCallerStatus = KRequestPending;
    
    // Ensure that we're in a proper state
    if ( iCurrentState != EMmsOpenAndWaiting ) 
        {
        LOGGER_WRITE_1("CMmsDataStore::DoCommitChangeInfoL, invalid state %d.", iCurrentState);
        }
    
    // Notify ChangeFinder
    iChangeFinder->CommitChangesL();
    iHasHistory = ETrue;
        
    // Signal we're done
    User::RequestComplete( iCallerStatus, KErrNone );
    
    LOGGER_LEAVEFN("CMmsDataStore::DoCommitChangeInfoL");
    }
        

// -----------------------------------------------------------------------------
// CMmsDataStore::RegisterSnapshotL
// Sets Changefinder to compare against current message store content
// -----------------------------------------------------------------------------
void CMmsDataStore::RegisterSnapshotL() const
    {
    CSnapshotArray* snapshot = new (ELeave) CSnapshotArray( KSnapshotGranularity );
    CleanupStack::PushL(snapshot);
    
    // Use only standard folders (except outbox)
    RegisterFolderL(snapshot, KMsvGlobalInBoxIndexEntryId);
    RegisterFolderL(snapshot, KMsvDraftEntryId);
    RegisterFolderL(snapshot, KMsvSentEntryId);
    RegisterFolderL(snapshot, KMsvGlobalOutBoxIndexEntryId);    
    RegisterUserFoldersL(snapshot);
    
    // Set new snapshot to compare against
    iChangeFinder->SetNewSnapshot(snapshot);

    // Changefinder takes ownership of the snapshot
    CleanupStack::Pop(snapshot);
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::RegisterFolderL
// Adds a single folder into the snapshot array
// -----------------------------------------------------------------------------
TInt CMmsDataStore::RegisterFolderL(CSnapshotArray* aSnapshot, const TMsvId& aId) const
    {
    // Get the root folder
    CMsvEntry* msvEntry = iMsvSession->GetEntryL(aId);
    CleanupStack::PushL( msvEntry );
    
    // Find all of it's childs
    CMsvEntrySelection* messages = msvEntry->ChildrenWithTypeL( KUidMsvMessageEntry );
    CleanupStack::PopAndDestroy( msvEntry );
    CleanupStack::PushL( messages );
    
    TMsvId id;
    TMsvEntry msg;
    
    // We are only interested of the MM content
    for ( TInt index=0; index<messages->Count(); index++ )
        {
        TInt result = iMsvSession->GetEntry( messages->At( index ), id, msg );
        User::LeaveIfError( result );
        
        // We're only interested about the multimedia content
        if ( msg.iMtm == KUidMsgTypeMultimedia )
            {
            // Create snapshot item
            TKeyArrayFix key(iKey);
            TSnapshotItem item( (TUint) msg.Id() );
            
            item.SetLastChangedDate( msg.iDate );
            item.SetParentId( msg.Parent() );
            item.SetUnread( msg.Unread() ? ETrue : EFalse );
            
            // Add to snapshot
            aSnapshot->InsertIsqL( item, key );
            }
        }

    CleanupStack::PopAndDestroy( messages );
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CMmsDataStore::MmsItemExists
// Returns ETrue if MMS item exists in the message store, otherwise EFalse
// -----------------------------------------------------------------------------    
TBool CMmsDataStore::MmsItemExists( TMsvId aUid )
    {
    CMsvEntry* entry(NULL);
    
    // Try to open this item
    TRAPD( error, entry = iMsvSession->GetEntryL( aUid ) );
    if ( error != KErrNone )
        {
        return EFalse;
        }
        
    TMsvEntry tEntry = entry->Entry();
    TBool result(EFalse);
    
    if ( tEntry.iType == KUidMsvMessageEntry && tEntry.iMtm == KUidMsgTypeMultimedia )
        {
        result = ETrue;
        }
    
    delete entry;
    
    return result;
    }
    
// -----------------------------------------------------------------------------
// CMmsDataStore::ResolveStatusBits
// Creates status bit field according to TMsvEntry parameter
// -----------------------------------------------------------------------------
TUint8 CMmsDataStore::ResolveStatusBits(TBool aUnread)
    {
    // Reset the status byte, then find the correct flags
    TUint8 data(0);
    
    // Set status according to the Read/Unread information iCurrentEntry   
    if ( aUnread )
        {
        // Status unset
        data &= (~KMMS_Flag_Read);
        }
    else
        { 
        // Status set
        data |= KMMS_Flag_Read;
        }
    
    return data;
    }   
    
// -----------------------------------------------------------------------------
// CMmsDataStore::ReadDataRecursively
// Write specified amount of data to the temporary buffer
// -----------------------------------------------------------------------------    
TInt CMmsDataStore::ReadDataRecursively( TDes8& aBuffer )
    {
    LOGGER_ENTERFN("CMmsDataStore::ReadDataRecursively");
    
    TInt error(KErrNone);
    
    TInt freeBuffer = aBuffer.MaxLength() - aBuffer.Length();
    
    if ( freeBuffer == 0 )
        {
        LOGGER_WRITE("Destination buffer filled.");
        return KErrNone;
        }
    
    if ( iReadPosition == 0 )
        {
        if ( iLastDataChunk )
            {
            LOGGER_WRITE("All MMS data read");
            iReadAllData = ETrue;
            return KErrNone;
            }
        else
            {
            error = iCodecClient->GetNextDataPart( iReadDataChunk, iLastDataChunk );
            if ( error != KErrNone )
                {
                LOGGER_WRITE_1("iCodecClient->GetNextDataPart failed with %d", error);
                return error;
                }
            else
                {
                LOGGER_WRITE_1("iCodecClient->GetNextDataPart succeeded, length %d", iReadDataChunk.Length());
                }   
            }   
        }
        
    TInt left = iReadDataChunk.Length() - iReadPosition;    
    
    if ( freeBuffer < left )
        {
        TPtrC8 data = iReadDataChunk.Mid(iReadPosition, freeBuffer);
        aBuffer.Append(data);
        iReadPosition += freeBuffer;
        return KErrNone; 
        }
    else
        {
        if ( left > 0 )
            {
            TPtrC8 data = iReadDataChunk.Mid(iReadPosition, left);
            aBuffer.Append(data);
            }
        error = iCodecClient->ReleaseData();
        if ( error != KErrNone )
            {
            return error;
            }
        iReadPosition = 0;
        return ReadDataRecursively( aBuffer );
        }           
    }   
    
// -----------------------------------------------------------------------------
// CMmsDataStore::UpdateMmsStatusL
// Updates MMS message status
// -----------------------------------------------------------------------------
void CMmsDataStore::UpdateMmsStatusL( TMsvId aId, TBool aUnread )
    {
    CMsvEntry* msvEntry = iMsvSession->GetEntryL( aId );
    const TMsvEntry& oldEntry = msvEntry->Entry();
    
    TMsvEntry newEntry( oldEntry );
    newEntry.SetUnread( aUnread );
    
    CleanupStack::PushL( msvEntry );
    msvEntry->ChangeL( newEntry );
    CleanupStack::PopAndDestroy( msvEntry );   
    }
    
// -----------------------------------------------------------------------------
// CMmsDataStore::RegisterUserFoldersL
// Adds user folder messages into the snapshot array
// -----------------------------------------------------------------------------
TInt CMmsDataStore::RegisterUserFoldersL( CSnapshotArray* aSnapshot ) const
    {
    LOGGER_ENTERFN("CMmsDataStore::RegisterUserFoldersL");     
    
    // Get the folder   
    CMsvEntry* msvEntry = iMsvSession->GetEntryL( KMsvMyFoldersEntryIdValue );
    CleanupStack::PushL(msvEntry);
    
    // Find all of it's childs
    CMsvEntrySelection* folders = msvEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PopAndDestroy( msvEntry ); 
    CleanupStack::PushL( folders );

    for ( TInt index = 0; index < folders->Count(); index++ )
        {
        TMsvId folderId = folders->At(index);
        
        if ( folderId != KMsvMyFoldersTemplatesFolderId )
            {
            TMsvId service;
            TMsvEntry folderEntry;
            TInt result = iMsvSession->GetEntry( folderId, service, folderEntry );
            User::LeaveIfError( result );
            
            TKeyArrayFix key(iKey);
            TBool unread(EFalse);
            TSnapshotItem item( (TUint) folderId, folderEntry.Parent(), unread );
            item.SetLastChangedDate( folderEntry.iDate );
            item.SetFolderNameL( folderEntry.iDetails );
            
            aSnapshot->InsertIsqL( item, key );
            
            RegisterFolderL( aSnapshot, folderId );
            }
        }
    
    CleanupStack::PopAndDestroy( folders );
    
    // Register also MMS messages directly under My Folders
    RegisterFolderL( aSnapshot, KMsvMyFoldersEntryIdValue );
    
    LOGGER_LEAVEFN("CMmsDataStore::RegisterUserFoldersL");     
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMmsDataStore::CleanUserFoldersL
// Cleans all user folders from MMS messages
// -----------------------------------------------------------------------------
TInt CMmsDataStore::CleanUserFoldersL() 
    {
    LOGGER_ENTERFN("CMmsDataStore::CleanUserFoldersL");      
    
    // Get the folder   
    CMsvEntry* msvEntry = iMsvSession->GetEntryL( KMsvMyFoldersEntryIdValue );
    CleanupStack::PushL(msvEntry);
    
    // Find all of it's childs
    CMsvEntrySelection* folders = msvEntry->ChildrenWithTypeL( KUidMsvFolderEntry );
    CleanupStack::PopAndDestroy( msvEntry ); 
    CleanupStack::PushL( folders );
    
    TInt error(KErrNone);
    TInt result(KErrNone);

    for ( TInt index = 0; index < folders->Count(); index++ )
        {
        TMsvId folderId = folders->At(index);
        
        if ( folderId != KMsvMyFoldersTemplatesFolderId )
            {
            error = DeleteAllMessagesInFolderL(folderId);
            if ( error != KErrNone )
                {
                LOGGER_WRITE_1("Deleting messages in folder failed with %d", error); 
                result = error;
                }
            error = iMsvApi->DeleteUserFolderL( folderId );
            if ( error != KErrNone && error != KErrInUse )
                {
                // Note: folder is not deleted if contains other message items (like MMS)
                // In this case DeleteUserFolderL returns KErrInUse.
                LOGGER_WRITE_1("iMsvApi->DeleteUserFolderL failed with %d", error);
                result = error;
                }
            }
        }
    
    CleanupStack::PopAndDestroy( folders );
    
    // Delete all messages directly under My Folders
    DeleteAllMessagesInFolderL( KMsvMyFoldersEntryIdValue );
    
    LOGGER_LEAVEFN("CSmsDataStore::CleanUserFoldersL");
    
    return result;       
    }
    
