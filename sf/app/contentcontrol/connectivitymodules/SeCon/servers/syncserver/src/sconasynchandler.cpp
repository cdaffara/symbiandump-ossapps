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
* Description:  CSconAsyncHandler implementation
*
*/


#include "sconasynchandler.h"

#include <ecom/ecom.h>
#include <SmlDataProvider.h>
#include <s32mem.h>
#include <nsmlchangefinder.h>
#include <e32cmn.h>
#include <ecom/ImplementationInformation.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

#include "sconsyncclientserver.h"
#include "sconsyncrelationship.h"
#include "scondataproviderinfo.h"
#include "debug.h"
#include "logdatastoreformat.h"

// Data Store interface implementation Uid.
// Load all plugins with this uid.
const TUid KDSEcomIFUid = {0x101F4D3A};

const TInt KDefaultExpandSize = 1024;

// one store per computer, max store count.
const TInt KMaxStoresCount = 10;

const TInt KDefaultTimeOutInMicroSeconds = 30 * 1000000; // 30 seconds
const TInt KDeleteAllTimeOutInMicroSeconds = 300 * 1000000; // 5 minutes
const TInt KOpenStoreTimeOutInMicroSeconds = 180 * 1000000; // 180 seconds

CSconAsyncHandler::~CSconAsyncHandler()
    {
    TRACE_FUNC_ENTRY;
    Cancel();
    DoCloseStore();
    delete iItems;
    delete iWriteData;
    delete iDataStore;
    delete iDataProvider;
    delete iContext;
    delete iStoreFormat;
    iStringPool.Close();
    iFs.Close();
    REComSession::FinalClose();
    iChunk.Close();
    delete iTimeOut;
    TRACE_FUNC_EXIT;
    }

CSconAsyncHandler::CSconAsyncHandler(): CActive( EPriorityStandard )
    {
    TRACE_FUNC;
    CActiveScheduler::Add( this );
    }

CSconAsyncHandler* CSconAsyncHandler::NewL()
    {
    TRACE_FUNC;
    CSconAsyncHandler* self = new(ELeave) CSconAsyncHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

void CSconAsyncHandler::ConstructL()
    {
    TRACE_FUNC;
    User::LeaveIfError( iFs.Connect() );
    TInt err = iFs.CreatePrivatePath( EDriveC );
    LOGGER_WRITE_1("CreatePrivatePath err: %d", err);
    iStringPool.OpenL();
    iTimeOut = CCSconTimeOut::NewL( *this );
    }

// -----------------------------------------------------------------------------
// CSconAsyncHandler::HandleServiceL()
// Handles the client request
// -----------------------------------------------------------------------------
//
void CSconAsyncHandler::HandleServiceL( const RMessage2& aMessage )
    {
    TRACE_FUNC_ENTRY;
    if ( aMessage.Function() == ECancelRequest )
        {
        LOGGER_WRITE("ECancelRequest");
        if ( IsActive() )
            {
            Cancel();
            LOGGER_WRITE("iMessage.Complete( KErrCancel )");
            CompleteRequest( KErrCancel );
            }
        
        LOGGER_WRITE("aMessage.Complete( KErrNone )");
        aMessage.Complete( KErrNone );
        TRACE_FUNC_EXIT;
        return;
        }
    
    iTimeOut->Start( KDefaultTimeOutInMicroSeconds );
    
    iMessage = aMessage;
    switch ( aMessage.Function() )
        {
        case ESendChunkHandle:
            LOGGER_WRITE( "CSconSyncSession::ServiceL() : ESendChunkHandle" );
            HandleChunkMessage();
            break;
            
        case EListImplementations:
            LOGGER_WRITE( "CSconSyncSession::ServiceL() : EListImplementations" );
            ListAllImplementationsL();
            break;
            
        case EOpenStore:
            LOGGER_WRITE( "CSconSyncSession::ServiceL() : EOpenStore" );
            iTimeOut->Start( KOpenStoreTimeOutInMicroSeconds );
            OpenStoreL();
            break;
            
        case EOpenItem:
            OpenItemL();
            break;
        case ECreateItem:
            CreateItemL();
            break;
        case EReplaceItem:
            ReplaceItemL();
            break;
        case EMoveItem:
            MoveItemL();
            break;
        case EDeleteItem:
            DeleteItemL();
            break;
        case ESoftDeleteItem:
            SoftDeleteItemL();
            break;
        case EDeleteAllItems:
            iTimeOut->Start( KDeleteAllTimeOutInMicroSeconds );
            DeleteAllItemsL();
            break;
        case EReadParent:
            ReadParentL();
            break;
            
        case EHasHistory:
            HasHistoryL();
            break;
        case EAddedItems:
            AddedItemsL();
            break;
        case EDeletedItems:
            DeletedItemsL();
            break;
        case ESoftDeletedItems:
            SoftDeletedItemsL();
            break;
        case EModifiedItems:
            ModifiedItemsL();
            break;
        case EMovedItems:
            MovedItemsL();
            break;
        case EResetChangeInfo:
            ResetChangeInfoL();
            break;
        case ECommitChangeInfo:
            CommitChangeInfoL();
            break;
        case ECloseStore:
            CloseStore();
            break;
        case ESetSyncTimeStamp:
            SetSyncTimeStampL();
            break;
        case EGetSyncTimeStamp:
            GetSyncTimeStampL();
            break;
        case EExportStoreFormat:
            ExportStoreFormatL();
            break;
        case ESetRemoteStoreFormat:
            SetRemoteStoreFormatL();
            break;
            
        default:
            aMessage.Complete(KErrNotSupported);
            break;
        }
    TRACE_FUNC_EXIT;
    }


void CSconAsyncHandler::RunL()
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("iStatus.Int(): %d", iStatus.Int());
    switch ( iMessage.Function() )
        {
        case EOpenStore:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : EOpenStore" );
            CompleteRequest( iStatus.Int() );
            break;
            
        case EOpenItem:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : EOpenItem" );
            OpenItemCompletedL( iStatus.Int() );
            break;
            
        case ECreateItem:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : ECreateItem" );
            if (!iCommitInProgress)
                {
                // Write data to item
                CreateItemCompletedL( iStatus.Int() );
                }
            else
                {
                // commit is done, all ready
                LOGGER_WRITE( "CSconAsyncHandler::RunL() : ECreateItem, commit done" );
                TPckg<TSmlDbItemUid> pckg(iNewItem);
                iMessage.WriteL( 0, pckg, 0);
                CompleteRequest( iStatus.Int() );
                }
            break;
            
        case EReplaceItem:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : EReplaceItem" );
            if (!iCommitInProgress)
                {
                // Write data to item
                ReplaceItemCompletedL( iStatus.Int() );
                }
            else
                {
                // commit is done, all ready
                LOGGER_WRITE( "CSconAsyncHandler::RunL() : EReplaceItem, commit done" );
                CompleteRequest( iStatus.Int() );
                }
            break;
            
        case EMoveItem:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : EMoveItem" );
            CompleteRequest( iStatus.Int() );
            break;
            
        case EDeleteItem:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : EDeleteItem" );
            CompleteRequest( iStatus.Int() );
            break;
            
        case ESoftDeleteItem:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : ESoftDeleteItem" );
            CompleteRequest( iStatus.Int() );
            break;
            
        case EDeleteAllItems:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : EDeleteAllItems" );
            CompleteRequest( iStatus.Int() );
            break;
            
        case EReadParent:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : EReadParent" );
            ReadParentCompletedL( iStatus.Int() );
            break;
            
        case EResetChangeInfo:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : EResetChangeInfo" );
            CompleteRequest( iStatus.Int() );
            break;
            
        case ECommitChangeInfo:
            LOGGER_WRITE( "CSconAsyncHandler::RunL() : ECommitChangeInfo" );
            delete iItems;
            iItems = NULL;
            CompleteRequest( iStatus.Int() );
            break;
            
        default:
            LOGGER_WRITE("Not asynchronous function")
            CompleteRequest( KErrNotSupported );
            break;
        }
    TRACE_FUNC_EXIT;
    }

TInt CSconAsyncHandler::RunError(TInt aError)
    {
    LOGGER_WRITE_1("CSconAsyncHandler::RunError() aError: %d", aError);
    delete iItems;
    iItems = NULL;
    delete iWriteData;
    iWriteData = NULL;
    
    if ( IsActive() )
        {
        LOGGER_WRITE("IsActive");
        //iDataStore->CancelRequest();
        Cancel();
        }
    
    if ( iDataStore && iMessage.Function() == EOpenItem )
        {
        LOGGER_WRITE( "CSconAsyncHandler::RunError() : EOpenItem" );
        iDataStore->CloseItem();
        }
    
    CompleteRequest( aError );
    aError = KErrNone;
    LOGGER_WRITE_1("CSconAsyncHandler::RunError() : Return %d", aError);
    return aError;
    //return KErrNone;//aError;
    }

void CSconAsyncHandler::DoCancel()
    {
    TRACE_FUNC_ENTRY;
    iDataStore->CancelRequest();
    iTimeOut->Cancel();
    
    if ( iStatus == KRequestPending )
        {
        LOGGER_WRITE( "iStatus == KRequestPending" );
        }
    
    if ( iDataStore && iMessage.Function() == EOpenItem )
        {
        LOGGER_WRITE( "CSconAsyncHandler::RunError() : EOpenItem" );
        iDataStore->CloseItem();
        }
    
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::TimeOut()
    {
    TRACE_FUNC_ENTRY;
    if ( IsActive() )
        {
        Cancel();
        CompleteRequest( KErrCancel );
        }
    TRACE_FUNC_EXIT;
    }


void CSconAsyncHandler::HandleChunkMessage()
    {
    TRACE_FUNC_ENTRY;
    TInt ret ( KErrNone );
    
    ret = iChunk.Open( iMessage, 0, EFalse );
        
    LOGGER_WRITE_1( "CSconAsyncHandler::HandleChunkMessageL() : ret %d", ret );
    CompleteRequest( ret );
    }



void CSconAsyncHandler::ListAllImplementationsL()
    {
    TRACE_FUNC_ENTRY;
    
    RImplInfoPtrArray implInfoArray;
    CleanupResetAndDestroyPushL( implInfoArray );
    REComSession::ListImplementationsL( KDSEcomIFUid, implInfoArray );
    
    const TInt KDataproviderInfoSize = 250; // 250 bytes should be enought
    TInt requiredSize = implInfoArray.Count() * KDataproviderInfoSize;
    if ( iChunk.Size() < requiredSize )
        {
        User::LeaveIfError( iChunk.Adjust( requiredSize ) );
        }
    
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    
    RSconDataProviderInfoArray dpInfoArray;
    CleanupResetAndDestroyPushL( dpInfoArray );
    
    // Collect information from dataproviders
    for ( TInt i=0; i < implInfoArray.Count(); i++ )
        {
        LOGGER_WRITE_1("implInfoArray: %d",i);
        CImplementationInformation& implInfo = *implInfoArray[i];
        
        CSmlDataProvider* dp(NULL);
        TRAPD( err, dp = CSmlDataProvider::NewL( implInfo.ImplementationUid().iUid ) );
        LOGGER_WRITE_2("CSmlDataProvider::NewL, uid(0x%08x) err: %d",implInfo.ImplementationUid().iUid, err );
        if ( !err )
            {
            CleanupStack::PushL( dp );
            
            CSconDataproviderInfo *dpInfo = CSconDataproviderInfo::NewL();
            CleanupStack::PushL( dpInfo );
            
            dpInfo->SetImplementationUid( implInfo.ImplementationUid() );
            LOGGER_WRITE_1("ImplementationUid: 0x%08x", implInfo.ImplementationUid().iUid );
            dpInfo->SetDisplayNameL( implInfo.DisplayName() );
            LOGGER_WRITE_1("DisplayName: %S", &implInfo.DisplayName() );
            
            dpInfo->SetDefaultStoreL( dp->DefaultStoreL() );
            LOGGER_WRITE_1("DefaultStoreL: %S", &dp->DefaultStoreL() );
            CDesCArray* stores = dp->ListStoresLC();
            dpInfo->SetStoresL( *stores );
            CleanupStack::PopAndDestroy( stores );
            
            dpInfoArray.AppendL( dpInfo );
            CleanupStack::Pop( dpInfo );
            
            CleanupStack::PopAndDestroy( dp );
            }
        }
    
    // Wrtie to stream
    stream.WriteUint16L( dpInfoArray.Count() );
    for ( TInt i = 0; i < dpInfoArray.Count(); i++ )
        {
        CSconDataproviderInfo& dpInfo = *dpInfoArray[i];
        dpInfo.ExternalizeL( stream );
        }
    
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &dpInfoArray );
    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::PopAndDestroy( &implInfoArray );
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::OpenStoreL()
    {
    TRACE_FUNC_ENTRY;
    if ( iDataStore )
        {
        LOGGER_WRITE("Warning: Previous DataStore was not closed properly.");
        DoCloseStore();
        }
    
    // copy data from the chunk
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    TSmlDataProviderId providerId = readStream.ReadUint32L();
    LOGGER_WRITE_1("providerId: 0x%08x", providerId);
    
    TInt len = readStream.ReadUint32L();
    HBufC* storeName = HBufC::NewLC(len);
    TPtr storeNamePtr = storeName->Des();
    readStream.ReadL(storeNamePtr, len);
    LOGGER_WRITE_1("storeName: %S", &storeNamePtr);
    
    TInt contextUid = readStream.ReadUint32L();
    
    LOGGER_WRITE_1("contextIdentifier: 0x%08x", contextUid);
    
    LOGGER_WRITE("Open provider");
    delete iDataProvider;
    iDataProvider = NULL;
    iDataProvider = CSmlDataProvider::NewL( providerId );
    
    delete iContext;
    iContext = NULL;
    
    delete iStoreFormat;
    iStoreFormat = NULL;

    LOGGER_WRITE("Create context");
    iContext = CSconSyncRelationship::NewL( iFs, TUid::Uid(contextUid), providerId );
    
    
    LOGGER_WRITE("Create NewStoreInstanceLC");
    iDataStore = iDataProvider->NewStoreInstanceLC();
    CleanupStack::Pop( iDataStore );
    
    SetActive();
    iStatus = KRequestPending;
    LOGGER_WRITE("OpenL");
    TRAPD(err, iDataStore->OpenL(storeNamePtr,*iContext, iStatus));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->OpenL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    
    CleanupStack::PopAndDestroy( storeName );
    CleanupStack::PopAndDestroy( &readStream );
    
    TRACE_FUNC_EXIT;
    }


void CSconAsyncHandler::OpenItemL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    if ( IsActive() )
        {
        LOGGER_WRITE("Was still in Active");
        Cancel();
        }
    
    TSmlDbItemUid itemUid = iMessage.Int0();
    LOGGER_WRITE_1("Open item: %d", itemUid);
    
    // set default values
    iFieldChange = EFalse;
    iSize = 0;
    iParent = 0;
    iMimeType.Copy(KNullDesC);
    iMimeVer.Copy(KNullDesC);
    
    SetActive();
    LOGGER_WRITE("iDataStore->OpenItemL");
    iStatus = KRequestPending;
    TRAPD( err, iDataStore->OpenItemL(itemUid,iFieldChange,iSize,iParent,iMimeType,iMimeVer,iStatus));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->OpenItemL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::OpenItemCompletedL( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    // item opened, now read it
    if ( aError )
        {
        CompleteRequest( aError );
        return;
        }
    LeaveIfNoInstanceL();
    
    LOGGER_WRITE_1("item size: %d", iSize);
    
    TInt totalObjectSize( 0 );
    totalObjectSize+= sizeof(TInt32);   // data length
    totalObjectSize+= iSize;            // data
    totalObjectSize+= sizeof(TInt8);    // iFieldChange
    totalObjectSize+= sizeof(TInt32);   // iParent
    totalObjectSize+= sizeof(TInt32);   // iMimeType
    totalObjectSize+= iMimeType.Length();  // iMimeType
    totalObjectSize+= sizeof(TInt32);   // iMimeVer
    totalObjectSize+= iMimeVer.Length();  // iMimeType
    
    LOGGER_WRITE_1("iChunk.Size(): %d", iChunk.Size());
    LOGGER_WRITE_1("iChunk.MaxSize(): %d", iChunk.MaxSize());
    LOGGER_WRITE_1("totalObjectSize: %d", totalObjectSize);
    
    if ( iChunk.Size() < totalObjectSize )
        {
        LOGGER_WRITE("adjust chunk");
        TInt err = iChunk.Adjust( totalObjectSize );
        LOGGER_WRITE_1("Chunk.Adjust err: %d", err);
        User::LeaveIfError( err );
        }
    LOGGER_WRITE_1("new Chunk size: %d", iChunk.Size());
    LOGGER_WRITE_1("new ChunkMaxSize(): %d", iChunk.MaxSize());
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    
    LOGGER_WRITE("ReadItemL");
    HBufC8* tempBuf = HBufC8::NewLC(iSize);
    TPtr8 tempPtr = tempBuf->Des();
    
    TUint8* ptr = (TUint8*) tempPtr.Ptr();
    
    TPtr8 tempPtr2( ptr, 0, iSize );
    LOGGER_WRITE_1( "tempPtr2.Size(): %d", tempPtr2.Size());
    iDataStore->ReadItemL( tempPtr2 );
    LOGGER_WRITE("ReadItemL -ok");
    
    LOGGER_WRITE_1("readed Length: %d", tempPtr2.Length() );
    LOGGER_WRITE_1("readed Size: %d", tempPtr2.Size() );
    
    LOGGER_WRITE8_1("iMimeType: %S", &iMimeType );
    LOGGER_WRITE_1("iMimeType.Length(): %d", iMimeType.Length() );
    LOGGER_WRITE_1("iMimeType Size: %d", iMimeType.Size() );
    
    LOGGER_WRITE8_1("iMimeVer: %S", &iMimeVer );
    LOGGER_WRITE_1("iMimeVer.Length(): %d", iMimeVer.Length() );
    LOGGER_WRITE_1("iMimeVer Size: %d", iMimeVer.Size() );
    
    iDataStore->CloseItem();
    LOGGER_WRITE("Write to chunk");
    stream.WriteInt32L( tempPtr2.Length() );
    stream.WriteL( tempPtr2 );
    CleanupStack::PopAndDestroy( tempBuf );
    
    stream.WriteInt8L( (TInt)iFieldChange );
    stream.WriteInt32L( iParent );
    stream.WriteInt32L( iMimeType.Length() );
    stream.WriteL( iMimeType );
    stream.WriteInt32L( iMimeVer.Length() );
    stream.WriteL( iMimeVer );
    
    stream.CommitL();
    LOGGER_WRITE("Writed ok");
    CleanupStack::PopAndDestroy( &stream );
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::CreateItemL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    iCommitInProgress = EFalse;
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    TSmlDbItemUid parent = readStream.ReadUint32L();
    TInt len = readStream.ReadUint32L();
    readStream.ReadL(iMimeType, len);
    len = readStream.ReadUint32L();
    readStream.ReadL(iMimeVer, len);
    len = readStream.ReadUint32L();
    delete iWriteData;
    iWriteData = NULL;
    iWriteData = HBufC8::New(len);
    TPtr8 dataPtr = iWriteData->Des();
    readStream.ReadL(dataPtr, len);
    CleanupStack::PopAndDestroy( &readStream );
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->CreateItemL( iNewItem, dataPtr.Size(), parent, iMimeType, iMimeVer, iStatus ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->CreateItemL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::CreateItemCompletedL( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( aError );
    LeaveIfNoInstanceL();
    // CreateItem completed, now we must write the data to the created item and commit it.
    
    iDataStore->WriteItemL( iWriteData->Des() );
    delete iWriteData;
    iWriteData = NULL;
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->CommitItemL( iStatus ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->CommitItemL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    iCommitInProgress = ETrue;
    
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::ReplaceItemL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    
    iCommitInProgress = EFalse;
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    TSmlDbItemUid uid = readStream.ReadUint32L();
    TSmlDbItemUid parent = readStream.ReadUint32L();
    TBool fieldChange = readStream.ReadUint8L();
    TInt len = readStream.ReadUint32L();
    delete iWriteData;
    iWriteData = NULL;
    iWriteData = HBufC8::New(len);
    TPtr8 dataPtr = iWriteData->Des();
    readStream.ReadL(dataPtr, len);
    
    CleanupStack::PopAndDestroy( &readStream );
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err,iDataStore->ReplaceItemL( uid,dataPtr.Size(),parent,fieldChange, iStatus ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->CommitItemL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::ReplaceItemCompletedL( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( aError );
    LeaveIfNoInstanceL();
    // CreateItem completed, now we must write the data to the created item and commit it.
    
    iDataStore->WriteItemL( iWriteData->Des() );
    delete iWriteData;
    iWriteData = NULL;
    
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->CommitItemL( iStatus ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->CommitItemL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    iCommitInProgress = ETrue;
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::MoveItemL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    TSmlDbItemUid uid = iMessage.Int0();
    TSmlDbItemUid newParent = iMessage.Int1();
    LOGGER_WRITE_1( "uid: %d", uid );
    LOGGER_WRITE_1( "newParent: %d", newParent );
    
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->MoveItemL( uid, newParent, iStatus )); 
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->MoveItemL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::DeleteItemL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    
    TSmlDbItemUid uid = iMessage.Int0();
    LOGGER_WRITE_1( "uid: %d", uid );
    
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->DeleteItemL( uid, iStatus ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->DeleteItemL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::SoftDeleteItemL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    
    TSmlDbItemUid uid = iMessage.Int0();
    LOGGER_WRITE_1( "uid: %d", uid );
    
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->SoftDeleteItemL( uid, iStatus ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->SoftDeleteItemL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::DeleteAllItemsL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->DeleteAllItemsL( iStatus ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->DeleteAllItemsL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::ReadParentL()
    {
    TRACE_FUNC_ENTRY;
    OpenItemL();
    
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::ReadParentCompletedL( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    // item opened, now read it
    if ( aError )
        {
        CompleteRequest( aError );
        return;
        }
    LeaveIfNoInstanceL();
    
    iDataStore->CloseItem();
    
    TPckg<TSmlDbItemUid> pckg(iParent);
    iMessage.WriteL( 1, pckg, 0);
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::HasHistoryL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    TBool hasHistory = iDataStore->HasSyncHistory();
    
    TPckgC<TBool> pckg(hasHistory);
    iMessage.WriteL( 0, pckg, 0);
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::AddedItemsL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    const MSmlDataItemUidSet& items = iDataStore->AddedItems();
    
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    items.ExternalizeL( stream );
    CleanupStack::PopAndDestroy( &stream );
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::DeletedItemsL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    const MSmlDataItemUidSet& items = iDataStore->DeletedItems();
    
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    items.ExternalizeL( stream );
    CleanupStack::PopAndDestroy( &stream );
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::SoftDeletedItemsL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    const MSmlDataItemUidSet& items = iDataStore->SoftDeletedItems();
    
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    items.ExternalizeL( stream );
    CleanupStack::PopAndDestroy( &stream );
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::ModifiedItemsL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    const MSmlDataItemUidSet& items = iDataStore->ModifiedItems();
    
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    items.ExternalizeL( stream );
    CleanupStack::PopAndDestroy( &stream );
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::MovedItemsL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    const MSmlDataItemUidSet& items = iDataStore->MovedItems();
    
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    items.ExternalizeL( stream );
    CleanupStack::PopAndDestroy( &stream );
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::ResetChangeInfoL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->ResetChangeInfoL( iStatus ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->ResetChangeInfoL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::CommitChangeInfoL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    if ( iItems )
        {
        delete iItems;
        iItems = NULL;
        }
    iItems = new (ELeave) CNSmlDataItemUidSet();
    iItems->InternalizeL( readStream );
    
    CleanupStack::PopAndDestroy( &readStream );
    
    SetActive();
    iStatus = KRequestPending;
    TRAPD(err, iDataStore->CommitChangeInfoL( iStatus, *iItems ));
    if ( err )
        {
        // we are on active state, call request completed (RunL)
        LOGGER_WRITE_1("iDataStore->CommitChangeInfoL leaved with err: %d", err);
        TRequestStatus* status = &iStatus;
        User::RequestComplete( status, err );
        }
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::CloseStore()
    {
    TRACE_FUNC_ENTRY;
    DoCloseStore();
    REComSession::FinalClose();
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::SetSyncTimeStampL()
    {
    TRACE_FUNC_ENTRY;
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    TInt relationId = readStream.ReadInt32L();
    TInt providerId = readStream.ReadInt32L();
    CleanupStack::PopAndDestroy( &readStream );
    
    CSconSyncRelationship::SetTimeStampL( iFs, TUid::Uid(relationId), providerId);
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::GetSyncTimeStampL()
    {
    TRACE_FUNC_ENTRY;
    TInt providerId = iMessage.Int0();
    TInt relationId = iMessage.Int1();
        
    TDateTime time;
    CSconSyncRelationship::GetTimeStampL( iFs, TUid::Uid(relationId), providerId, time);
    
    // write timestamp
    TPckgC<TDateTime> timeBuf(time);
    iMessage.WriteL( 2, timeBuf, 0);
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::ExportStoreFormatL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    if ( !iDataProvider )
        {
        User::Leave( KErrNotReady );
        }
    CBufFlat* buffer = CBufFlat::NewL( KDefaultExpandSize );
    CleanupStack::PushL( buffer );
    RBufWriteStream stream( *buffer );
    CleanupClosePushL( stream );
    
    const CSmlDataStoreFormat& storeFormat = iDataProvider->StoreFormatL();
    TRAP_IGNORE( TLogDataStoreFormat::LogDataStoreFormatL( _L("iDataProvider->StoreFormatL()"), storeFormat ));
    
    storeFormat.ExternalizeL( stream );
    
    stream.CommitL();
    CleanupStack::PopAndDestroy( &stream );
    buffer->Compress();
    
    RMemWriteStream chunkStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( chunkStream );
    
    chunkStream.WriteInt32L( buffer->Size() );
    chunkStream.WriteL( buffer->Ptr(0), buffer->Size());
    chunkStream.CommitL();
    CleanupStack::PopAndDestroy( &chunkStream );
    
    CleanupStack::PopAndDestroy( buffer );
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::SetRemoteStoreFormatL()
    {
    TRACE_FUNC_ENTRY;
    LeaveIfNoInstanceL();
    
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    if ( iStoreFormat )
        {
        delete iStoreFormat;
        iStoreFormat = NULL;
        }
    iStoreFormat = CSmlDataStoreFormat::NewLC( iStringPool, readStream);
    CleanupStack::Pop( iStoreFormat );
    TRAP_IGNORE( TLogDataStoreFormat::LogDataStoreFormatL( _L("iDataProvider->SetRemoteStoreFormatL()"), *iStoreFormat ));
    iDataStore->SetRemoteStoreFormatL( *iStoreFormat );
    
    CleanupStack::PopAndDestroy( &readStream );
    
    CompleteRequest( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::LeaveIfNoInstanceL()
    {
    if ( !iDataStore )
        {
        LOGGER_WRITE("DataStore instance was not ready, leaving KErrNotReady");
        User::Leave( KErrNotReady );
        }
    }

void CSconAsyncHandler::DoCloseStore()
    {
    TRACE_FUNC_ENTRY;
    delete iItems;
    iItems = NULL;
    delete iWriteData;
    iWriteData = NULL;
    if ( iDataProvider && iContext )
        {
        TInt providerId = iDataProvider->Identifier();
        TInt contextId = iContext->SyncTaskKey();
        TRAP_IGNORE( CSconSyncRelationship::SetTimeStampL( iFs, TUid::Uid(contextId), providerId) );
        }
    delete iDataStore;
    iDataStore = NULL;
    delete iDataProvider;
    iDataProvider = NULL;
    delete iContext;
    iContext = NULL;
    delete iStoreFormat;
    iStoreFormat = NULL;
    
    CleanOldStoresL();
    
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::CleanOldStoresL()
    {
    TRACE_FUNC_ENTRY;
    TFileName path;
    User::LeaveIfError( iFs.PrivatePath( path ) );
    CDir* dir;
    iFs.GetDir( path, KEntryAttNormal, ESortByDate, dir );
    CleanupStack::PushL( dir );
    TInt storesCount(0);
    LOGGER_WRITE_1("count: %d", dir->Count() );
    for ( TInt i=dir->Count()-1; i >= 0; i-- )
        {
        LOGGER_WRITE_1("dir[%d]", i);
        TEntry entry = (*dir)[i];
        LOGGER_WRITE_1("file: %S", &entry.iName);
        _LIT(KContextStoreName, "contextstore");
        _LIT(KTimeStoreName, "timestore");
        if ( entry.iName.Find(KContextStoreName) == 0 )
            {
            storesCount++;
            LOGGER_WRITE_1("storeNro: %d", storesCount);
#ifdef _DEBUG
            TTime time = entry.iModified;
            TDateTime dt = time.DateTime();
            _LIT(KFormat, "%d.%d %02d:%02d");
            TFileName mod;
            mod.Format(KFormat, dt.Day()+1, dt.Month()+1, dt.Hour()+1,dt.Minute() );
            LOGGER_WRITE_1("time: %S", &mod);
#endif
            if ( storesCount > KMaxStoresCount )
                {
                LOGGER_WRITE_1("delete contextstore: '%S'", &entry.iName );
                iFs.Delete( entry.iName );
                TFileName timeStoreFile;
                timeStoreFile = entry.iName.Right(15);
                timeStoreFile.Insert(0, KTimeStoreName);
                LOGGER_WRITE_1("delete timeStoreFile: %S", &timeStoreFile);
                iFs.Delete( timeStoreFile );
                }
            }
        
        }
    CleanupStack::PopAndDestroy( dir );
    TRACE_FUNC_EXIT;
    }

void CSconAsyncHandler::CompleteRequest( TInt aError )
    {
    TRACE_FUNC;
    iTimeOut->Cancel();
    LOGGER_WRITE_1("iMessage.Complete( %d )", aError);
    iMessage.Complete( aError );
    }
