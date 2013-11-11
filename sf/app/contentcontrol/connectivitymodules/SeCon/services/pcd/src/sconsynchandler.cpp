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
* Description:  CSconSyncHandler implementation
*
*/


#include "sconsynchandler.h"
#include <f32file.h>
#include <S32MEM.H>
#include <UTF.H>
#include <mmf/common/mmfcontrollerpluginresolver.h>
#include <bautils.h>
#include <nsmlconstants.h>
#include <centralrepository.h>

#include "debug.h"

// versions up to 5 must be backward compatible
const TUint16 KFormatVersionNumber ( 1 );

_LIT8(KCommandListStores, "SYNC: ListStores");
_LIT8(KCommandOpenStore, "SYNC: OpenStore: ");
_LIT8(KCommandCloseStore, "SYNC: CloseStore");
_LIT8(KCommandListChanges, "SYNC: ListChanges");
_LIT8(KCommandResetChangeInfo, "SYNC: ResetChangeInfo");
_LIT8(KCommandCommitChanges, "SYNC: CommitChanges: ");
_LIT8(KCommandReadItems, "SYNC: ReadItems: ");
_LIT8(KCommandGetParents, "SYNC: GetParents: ");
_LIT8(KCommandCreateItems, "SYNC: CreateItems");
_LIT8(KCommandGetCreateItemsResponse, "SYNC: GetCreateItemsResponse");
_LIT8(KCommandGetCreateItems, "SYNC: GetCreateItems: ");
_LIT8(KCommandReplaceItems, "SYNC: ReplaceItems");
_LIT8(KCommandGetReplaceItemsResponse, "SYNC: GetReplaceItemsResponse");
_LIT8(KCommandGetReplaceItems, "SYNC: GetReplaceItems: ");
_LIT8(KCommandMoveItems, "SYNC: MoveItems");
_LIT8(KCommandDeleteItems, "SYNC: DeleteItems: ");
_LIT8(KCommandSoftDeleteItems, "SYNC: SoftDeleteItems: ");
_LIT8(KCommandDeleteAllItems, "SYNC: DeleteAllItems");
_LIT8(KCommandDeleteAllItemsStatus, "SYNC: GetDeleteAllItemsStatus");
_LIT8(KCommandGetStoreFormat, "SYNC: GetStoreFormat");
_LIT8(KCommandSetRemoteStoreFormat, "SYNC: SetRemoteStoreFormat");
_LIT8(KCommandGetRestOfData, "SYNC: GetRestOfData");
_LIT8(KCommandCancel, "SYNC: Cancel");


_LIT(KRemoteFormatStore, "remoteformatstore_0x%08x.dat");
_LIT(KDataFileExtension, ".tmp");

const TInt KTimeStampLength = 16;
_LIT8(KTimeStampFormat, "%02d%02d%04dT%02d%02d%02dZ");

const TInt KDefaultExpandSize = 1024;

// Calendar sync specific constants
const TInt KCalendarDsUid = 0x101F6DDE;

CSconSyncHandler::~CSconSyncHandler()
    {
    TRACE_FUNC_ENTRY;
    Cancel();
    iSyncSession.Close();
    delete iResponseData;
    iFileStream.Close();
    iCreatedItems.Close();
    iReplacedItems.Close();
    iItemsToRead.Close();
    iFs.Delete( iFileInProgress );
    ClearCalendarCenrepL();
    TRACE_FUNC_EXIT;
    }

CSconSyncHandler::CSconSyncHandler( RFs& aFs ) 
    : CActive( EPriorityStandard ), iFs(aFs), iCreatedItemUidPckg(iCreatedItemUid)
    {
    CActiveScheduler::Add( this );
    }

CSconSyncHandler* CSconSyncHandler::NewL( RFs& aFs)
    {
    TRACE_FUNC_ENTRY;
    CSconSyncHandler* self = new (ELeave) CSconSyncHandler(aFs);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

void CSconSyncHandler::ConstructL()
    {
    TInt err = iFs.CreatePrivatePath( EDriveC );
    LOGGER_WRITE_1("CreatePrivatePath err: %d", err);
    iFs.SetSessionToPrivate( EDriveC );
    }

void CSconSyncHandler::HandleGetSyncRequestL( const TDesC8& aRequest, RWriteStream& aResult, TInt aMaxObjectSize )
    {
    TRACE_FUNC_ENTRY;
    iMaxObjectSize = aMaxObjectSize;
    iBytesWrited = 0;
    LOGGER_WRITE_1("iMaxObjectSize: %d", iMaxObjectSize);
    if ( !iConnected )
        {
        LOGGER_WRITE("Try to connect")
        User::LeaveIfError( iSyncSession.Connect() );
        iConnected = ETrue;
        }
    
    if ( aRequest.Compare( KCommandListStores ) == KErrNone )
        {
        ListStoresL( aResult );
        }
    else if ( aRequest.Find( KCommandOpenStore ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandOpenStore().Length() );
        OpenStoreL( params, aResult );
        }
    else if ( aRequest.Find( KCommandCloseStore ) == 0 )
        {
        CloseStoreL( aResult );
        }
    else if ( aRequest.Find( KCommandListChanges ) == 0 )
        {
        ListChangesL( aResult );
        }
    else if ( aRequest.Find( KCommandResetChangeInfo ) == 0 )
        {
        ResetChangeInfoL( aResult );
        }
    else if ( aRequest.Find( KCommandCommitChanges ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandCommitChanges().Length() );
        CommitChangesL( params, aResult );
        }
    else if ( aRequest.Find( KCommandReadItems ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandReadItems().Length() );
        ReadItemsL( params, aResult );
        }
    else if ( aRequest.Find( KCommandGetParents ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandGetParents().Length() );
        GetParentsL( params, aResult );
        }
    else if ( aRequest.Find( KCommandGetRestOfData ) == 0 )
        {
        // Get rest of data
        ReadNextDataBlockL( aResult );
        }
    else if ( aRequest.Find( KCommandGetCreateItemsResponse ) == 0 )
        {
        GetCreateItemsResponseL( KNullDesC8(), aResult );
        }
    else if ( aRequest.Find( KCommandGetCreateItems ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandGetCreateItems().Length() );
        GetCreateItemsResponseL( params, aResult );
        }
    else if ( aRequest.Find( KCommandGetReplaceItemsResponse ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandGetReplaceItemsResponse().Length() );
        GetReplaceItemsResponseL( KNullDesC8(), aResult );
        }
    else if ( aRequest.Find( KCommandGetReplaceItems ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandGetReplaceItems().Length() );
        GetReplaceItemsResponseL( params, aResult );
        }
    else if ( aRequest.Find( KCommandMoveItems ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandMoveItems().Length() );
        MoveItemsL( params, aResult );
        }
    else if ( aRequest.Find( KCommandDeleteItems ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandDeleteItems().Length() );
        DeleteItemsL( params, aResult );
        }
    else if ( aRequest.Find( KCommandSoftDeleteItems ) == 0 )
        {
        TPtrC8 params = aRequest.Mid( KCommandSoftDeleteItems().Length() );
        SoftDeleteItemsL( params, aResult );
        }
    else if ( aRequest.Find( KCommandDeleteAllItems ) == 0 )
        {
        DeleteAllItemsL( aResult );
        }
    else if ( aRequest.Find( KCommandDeleteAllItemsStatus ) == 0 )
        {
        GetDeleteAllItemsStatusL( aResult );
        }
    else if ( aRequest.Find( KCommandGetStoreFormat ) == 0 )
        {
        GetStoreFormatL( aResult );
        }
    else if ( aRequest.Find( KCommandCancel ) == 0 )
        {
        CancelOperationsL( aResult );
        }
    else    
        {
        LOGGER_WRITE("Unknown command");
        User::Leave( KErrNotSupported );
        }
    TRACE_FUNC_EXIT;
    }


void CSconSyncHandler::HandlePutSyncRequestL( const TDesC8& aRequest, RReadStream& aData )
    {
    TRACE_FUNC_ENTRY;
    if ( !iConnected )
        {
        LOGGER_WRITE("Try to connect")
        User::LeaveIfError( iSyncSession.Connect() );
        iConnected = ETrue;
        }
    if ( aRequest.Find( KCommandCreateItems ) == KErrNone )
        {
        CreateItemsL( aData );
        }
    else if ( aRequest.Find( KCommandReplaceItems ) == 0 )
        {
        ReplaceItemsL( aData );
        }
    else if ( aRequest.Find( KCommandSetRemoteStoreFormat ) == 0 )
        {
        SetRemoteStoreFormatL( aData );
        }
    else
        {
        LOGGER_WRITE("Unknown command");
        User::Leave( KErrNotSupported );
        }
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::ListStoresL( RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    RSconDataProviderInfoArray infoArray(5);
    CleanupResetAndDestroyPushL( infoArray );
    iSyncSession.ListAllImplementationsL( infoArray );
    LOGGER_WRITE_1("infoArray.Count(): %d", infoArray.Count());
    aResult.WriteInt32L( infoArray.Count() );
    for (TInt i=0; i<infoArray.Count(); i++ )
        {
        CSconDataproviderInfo& info = (*infoArray[i]);
        info.ExternalizeL( aResult );
        }
    CleanupStack::PopAndDestroy( &infoArray );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::OpenStoreL( const TDesC8& aParams, RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE8_1("Params:'%S'", &aParams);
    
    const TUint8* ptr = aParams.Ptr();
    LOGGER_WRITE_1("Params size: %d", aParams.Size())
    RMemReadStream stream( ptr , aParams.Size() );
    CleanupClosePushL( stream );

    TInt providerId = stream.ReadInt32L();
    LOGGER_WRITE_1("Provider Id: 0x%08x", providerId);
    TInt contextId = stream.ReadInt32L();
    LOGGER_WRITE_1("Context Id: 0x%08x", contextId);
    
    // read utf8 formatted text
    TInt len = stream.ReadUint16L();
    
    LOGGER_WRITE_1("storeName length: %d", len);
    HBufC8* buf8 = HBufC8::NewLC( len );
    TPtr8 bufPtr8 = buf8->Des();
    stream.ReadL( bufPtr8, len );
    
    LOGGER_WRITE8_1("storeName8: %S", &bufPtr8);
    
    // and convert it to unicode
    HBufC* storeName = CnvUtfConverter::ConvertToUnicodeFromUtf8L( bufPtr8 ); 
    CleanupStack::PopAndDestroy( buf8 );
    CleanupStack::PushL( storeName );
    
    TPtr storeNamePtr = storeName->Des();
    LOGGER_WRITE_1("storeName: %S", &storeNamePtr);
    
    if ( providerId == KCalendarDsUid && aParams.Size() >
            sizeof(TInt32) // providerId
            + sizeof(TInt32) // contextId
            + sizeof(TUint16)// storeName len
            + len )          // storeName
        {
        TInt profileId = stream.ReadInt32L();
        len = stream.ReadUint16L();
        HBufC8* buffer = HBufC8::NewLC( len );
        TPtr8 bufferPtr8 = buffer->Des();
        stream.ReadL( bufferPtr8, len );
        HBufC* serverId = CnvUtfConverter::ConvertToUnicodeFromUtf8L( bufferPtr8 ); 
        CleanupStack::PopAndDestroy( buffer );
        CleanupStack::PushL( serverId );
        SetCalendarCenrepL( profileId, serverId->Des() );
        CleanupStack::PopAndDestroy(serverId);
        }
    
    aResult.WriteUint16L( KFormatVersionNumber );
    TRAPD( err, iSyncSession.OpenDataStoreL( providerId, storeName->Des(), contextId));
    if ( err == KErrServerTerminated )
        {
        // server terminated, try to reconnect
        iConnected = EFalse;
        LOGGER_WRITE("Try to re-connect");
        err = iSyncSession.Connect();
        LOGGER_WRITE_1("iSyncSession.Connect() err: %d", err);
        if ( !err )
            {
            iConnected = ETrue;
            TRAP( err, iSyncSession.OpenDataStoreL( providerId, storeName->Des(), contextId));
            }
        }
    
    CleanupStack::PopAndDestroy( storeName );
    CleanupStack::PopAndDestroy( &stream );
    
    aResult.WriteInt32L( err );
    
    if ( err )
        {
        LOGGER_WRITE_1("Error: %d", err);
        LOGGER_WRITE("iSyncSession.CloseDataStore()");
        iSyncSession.CloseDataStore();
        ClearCalendarCenrepL();
        return;
        }
    iCurrentDataProviderUid = providerId;
    iCurrentContextUid = contextId;
    
    
    // Now the store is opened
    TRAP(err, LoadRemoteStoreFormatL( contextId, providerId ));
    if ( err )
        {
        aResult.WriteUint8L( 1 ); //  Remote store format needed
        }
    else
        {
        aResult.WriteUint8L( 0 ); //  Remote store format not needed
        }
    
    // return last sync timestamp, if has synced before.
    TBool hasHistory = iSyncSession.HasSyncHistoryL();
    TUint8 historyFlag(0);
    if ( hasHistory )
        {
        historyFlag = 1;
        }
    aResult.WriteUint8L( historyFlag );
    
    if ( hasHistory )
        {
        TDateTime timeStamp;
        TInt err(KErrNone);
        TRAP(err, iSyncSession.GetSyncTimeStampL( providerId, contextId, timeStamp));
        LOGGER_WRITE_1("GetSyncTimeStampL err: %d", err);
        
        TBuf8<KTimeStampLength> timeStampBuf;
        
        timeStampBuf.Format( KTimeStampFormat, 
                timeStamp.Day()+1,timeStamp.Month()+1, timeStamp.Year(),
                timeStamp.Hour(), timeStamp.Minute(), timeStamp.Second() );
        LOGGER_WRITE8_1("timeStamp: %S", &timeStampBuf);
        aResult.WriteL( timeStampBuf );
        }
    
    
    //  for testin purposes only
    /*
    if (!remoteStoreSetted)
        {
        RStringPool pool;
        pool.OpenL();
        CleanupClosePushL( pool );
        CSmlDataStoreFormat* storeFormat(NULL);
        TRAPD( err, storeFormat = iSyncSession.StoreFormatL( pool ) );
        CleanupStack::PushL( storeFormat );
        
        iSyncSession.SetRemoteStoreFormatL( *storeFormat );
        SaveRemoteStoreFormatL( *storeFormat, contextId, providerId );
        
        CleanupStack::PopAndDestroy( storeFormat );
        CleanupStack::PopAndDestroy( &pool );
        }
    */
    
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::LoadRemoteStoreFormatL( TInt aContextId, TInt aProviderId )
    {
    TRACE_FUNC_ENTRY;
    TFileName remoteFormatStore;
    iFs.SetSessionToPrivate( EDriveC );
    remoteFormatStore.Format( KRemoteFormatStore, aContextId );
       
    CDictionaryFileStore* dictionaryStore =
        CDictionaryFileStore::OpenLC(iFs, remoteFormatStore, TUid::Uid(0x0001));
    TBool present = dictionaryStore->IsPresentL( TUid::Uid(aProviderId) );
    if ( !present )
        {
        LOGGER_WRITE("Remote store was not saved");
        User::Leave( KErrNotFound );
        }
    else
        {
        RDictionaryReadStream stream;
        stream.OpenLC( *dictionaryStore, TUid::Uid(aProviderId) );
        RStringPool stringPool;
        stringPool.OpenL();
        CleanupClosePushL( stringPool );
        CSmlDataStoreFormat* storeFormat = CSmlDataStoreFormat::NewLC(stringPool, stream);
        iSyncSession.SetRemoteStoreFormatL( *storeFormat );
        CleanupStack::PopAndDestroy( storeFormat );
        CleanupStack::PopAndDestroy( &stringPool );
        CleanupStack::PopAndDestroy(); //OpenLC
        }
    CleanupStack::PopAndDestroy( dictionaryStore );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::SaveRemoteStoreFormatL( CSmlDataStoreFormat& aStoreFormat, TInt aContextId, TInt aProviderId)
    {
    TRACE_FUNC_ENTRY;
    iFs.SetSessionToPrivate( EDriveC );
    TFileName remoteFormatStore;
    remoteFormatStore.Format( KRemoteFormatStore, aContextId );
    CDictionaryFileStore* dictionaryStore =
        CDictionaryFileStore::OpenLC(iFs, remoteFormatStore, TUid::Uid(0x0001));
    
    RDictionaryWriteStream stream;
    stream.AssignLC( *dictionaryStore, TUid::Uid(aProviderId) );
    
    aStoreFormat.ExternalizeL( stream );
    stream.CommitL();
    CleanupStack::PopAndDestroy(); //AssignLC
    dictionaryStore->CommitL();
        
    CleanupStack::PopAndDestroy( dictionaryStore );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::CloseStoreL( RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    iSyncSession.CloseDataStore();
    ClearCalendarCenrepL();
    
    // Get last used timestamp if provider was open
    if ( iCurrentDataProviderUid != 0 )
        {
        TDateTime timeStamp;
        iSyncSession.GetSyncTimeStampL(iCurrentDataProviderUid, iCurrentContextUid, timeStamp );
        
        iCurrentDataProviderUid = 0;
        iCurrentContextUid = 0;
        
        const TInt KTimeStampLength = 16;
        TBuf8<KTimeStampLength> timeStampBuf;
        
        
        timeStampBuf.Format( KTimeStampFormat, 
                timeStamp.Day()+1,timeStamp.Month()+1, timeStamp.Year(),
                timeStamp.Hour(), timeStamp.Minute(), timeStamp.Second() );
        aResult.WriteL( timeStampBuf );
        }
    
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::ListChangesL( RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    RArray<TSmlDbItemUid> items;
    CleanupClosePushL( items );
    
    iSyncSession.AddedItemsL( items );
    LOGGER_WRITE_1("Added items count: %d", items.Count());
    aResult.WriteInt32L( items.Count() );
    for (TInt i=0; i<items.Count(); i++ )
        {
        aResult.WriteInt32L( items[i] );
        }
    
    items.Reset();
    iSyncSession.ModifiedItemsL( items );
    LOGGER_WRITE_1("Modified items count: %d", items.Count());
    aResult.WriteInt32L( items.Count() );
    for (TInt i=0; i<items.Count(); i++ )
        {
        aResult.WriteInt32L( items[i] );
        }
    
    items.Reset();
    iSyncSession.MovedItemsL( items );
    LOGGER_WRITE_1("Moved items count: %d", items.Count());
    aResult.WriteInt32L( items.Count() );
    for (TInt i=0; i<items.Count(); i++ )
        {
        aResult.WriteInt32L( items[i] );
        }
    
    items.Reset();
    iSyncSession.DeletedItemsL( items );
    LOGGER_WRITE_1("Deleted items count: %d", items.Count());
    aResult.WriteInt32L( items.Count() );
    for (TInt i=0; i<items.Count(); i++ )
        {
        aResult.WriteInt32L( items[i] );
        }
    
    items.Reset();
    iSyncSession.SoftDeletedItemsL( items );
    LOGGER_WRITE_1("SoftDeleted items count: %d", items.Count());
    aResult.WriteInt32L( items.Count() );
    for (TInt i=0; i<items.Count(); i++ )
        {
        aResult.WriteInt32L( items[i] );
        }
    CleanupStack::PopAndDestroy( &items );
    
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::ResetChangeInfoL( RWriteStream& /*aResult*/ )
    {
    TRACE_FUNC_ENTRY;
    iSyncSession.ResetChangeInfoL();
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::CommitChangesL( const TDesC8& aParams, RWriteStream& /*aResult*/ )
    {
    TRACE_FUNC_ENTRY;
    const TUint8* ptr = aParams.Ptr();
    RMemReadStream stream( ptr , aParams.Size() );
    CleanupClosePushL( stream );
    
    TInt itemCount = stream.ReadInt32L();
    RArray<TSmlDbItemUid> items;
    CleanupClosePushL( items );
    for ( TInt i=0; i<itemCount; i++ )
        {
        items.AppendL( stream.ReadInt32L() );
        }
    
    if ( items.Count() > 0 )
        {
        iSyncSession.CommitChangeInfoL( items );
        }
    
    CleanupStack::PopAndDestroy( &items );
    CleanupStack::PopAndDestroy( &stream );
    
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::ReadItemsL( const TDesC8& aParams, RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    iItemsToRead.Reset();
    LOGGER_WRITE_1("aParams length: %d", aParams.Length());
    
    const TUint8* ptr = aParams.Ptr();
    RMemReadStream stream( ptr , aParams.Size() );
    CleanupClosePushL( stream );
    
    // Read item uids from parameter
    TInt itemCount = stream.ReadInt32L();
    LOGGER_WRITE_1("Item count: %d", itemCount);
    for ( TInt i=0; i<itemCount; i++ )
        {
        iItemsToRead.AppendL( stream.ReadInt32L() );
        LOGGER_WRITE_2("Item[%d] = %d", i, iItemsToRead[i] );
        }
    CleanupStack::PopAndDestroy( &stream );
    LOGGER_WRITE( "Items readed ok" );
    
    aResult.WriteInt32L( iItemsToRead.Count() );
    iBytesWrited += sizeof(TInt32);
    
    ReadNextDataBlockL( aResult );
    
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::GetParentsL( const TDesC8& aParams, RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    const TUint8* ptr = aParams.Ptr();
    RMemReadStream stream( ptr , aParams.Size() );
    CleanupClosePushL( stream );
    
    // Read item uids from parameter
    TInt itemCount = stream.ReadInt32L();
    aResult.WriteInt32L( itemCount );
    LOGGER_WRITE_1("Item count: %d", itemCount);
    for ( TInt i=0; i<itemCount; i++ )
        {
        TSmlDbItemUid itemUid = stream.ReadInt32L();
        TSmlDbItemUid parent(KErrNotFound);
        aResult.WriteInt32L( itemUid );
        TInt err = iSyncSession.GetParent( itemUid, parent);
        LOGGER_WRITE_1("itemUid: %d", itemUid);
        LOGGER_WRITE_1("err: %d", err);
        aResult.WriteInt32L( err );
        if ( err == KErrNone )
            {
            LOGGER_WRITE_1("parent: %d", parent);
            aResult.WriteInt32L( parent );
            }
        }
    CleanupStack::PopAndDestroy( &stream );
    aResult.CommitL();
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::ReadNextDataBlockL( RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( (!iResponseData || iResponseData->Size() == 0 ) && iItemsToRead.Count() == 0 )
        {
        // no data
        LOGGER_WRITE("no more data or items");
        User::Leave(KErrEof);
        }
    
    if ( iResponseData )
        {
        // write data from tempbuffer to response buffer
        TInt writeLength = iResponseData->Size();
        if ( writeLength > iMaxObjectSize-iBytesWrited )
            {
            writeLength = iMaxObjectSize-iBytesWrited;
            }
        aResult.WriteL( iResponseData->Ptr(0), writeLength );
        iBytesWrited += writeLength;
        
        if ( iResponseData->Size()  > writeLength )
            {
            iResponseData->Delete(0, writeLength);
            }
        else
            {
            iResponseData->Reset();
            }
        }
    if ( iBytesWrited == iMaxObjectSize )
        {
        // responce buffer is full, return it.
        LOGGER_WRITE("Stream is full, return it");
        return;
        }
    
    TBool streamIsFull( EFalse );
    // Read items from server
    CBufFlat* dataBuffer = CBufFlat::NewL(KDefaultExpandSize);
    CleanupStack::PushL( dataBuffer );
    while ( iItemsToRead.Count() > 0 && !streamIsFull )
        {
        if ( iMaxObjectSize-iBytesWrited < sizeof(TUint32) )
            {
            streamIsFull = ETrue;
            LOGGER_WRITE("Stream is full, don't read next item");
            continue;
            }
        ReadItemL( iItemsToRead[0], *dataBuffer );
        aResult.WriteUint32L( dataBuffer->Size() );
        iBytesWrited += sizeof( TUint32 );
        
        TInt writeLength = dataBuffer->Size();
        if ( writeLength > iMaxObjectSize-iBytesWrited )
            {
            writeLength = iMaxObjectSize-iBytesWrited;
            LOGGER_WRITE_1("Write only %d bytes", writeLength);
            }
        aResult.WriteL( dataBuffer->Ptr(0), writeLength );
        iBytesWrited += writeLength;
        
        if ( dataBuffer->Size()  > writeLength )
            {
            // write rest to next data block
            if ( !iResponseData )
                {
                iResponseData = CBufFlat::NewL(KDefaultExpandSize);
                }
            iResponseData->Reset();
            LOGGER_WRITE_1("Save %d bytes for next request", dataBuffer->Size() - writeLength);
            iResponseData->ResizeL( dataBuffer->Size() - writeLength );
            iResponseData->Write(0, dataBuffer->Ptr( writeLength ));
            streamIsFull = ETrue;
            }
        
        iItemsToRead.Remove(0);
        
        if ( iBytesWrited == iMaxObjectSize )
            {
            // writestream is full
            LOGGER_WRITE("Stream is full");
            streamIsFull = ETrue;
            }
        
        }
    aResult.CommitL();
    
    CleanupStack::PopAndDestroy( dataBuffer );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::ReadItemL( TSmlDbItemUid aUid, CBufFlat& aItemData )
    {
    TRACE_FUNC_ENTRY;
    aItemData.Reset();
    CBufFlat* dataBuffer = CBufFlat::NewL(KDefaultExpandSize);
    CleanupStack::PushL( dataBuffer );
    TSmlDbItemUid parent(-1);
    TBool fieldChange(EFalse);
    
    TBuf8<256> mimeType;
    TBuf8<100> mimeVer;
    TRAPD( err, iSyncSession.OpenItemL( aUid, fieldChange,
                    parent, mimeType, mimeVer, *dataBuffer ));
    if ( err )
        {
        LOGGER_WRITE_1("Could not read item %d", aUid);
        // only item uid and errorcode will be writed
        aItemData.ResizeL( 
            sizeof(TInt32) +
            sizeof(TInt32));
        
        }
    else
        {
        // reserve memory for all fields
        aItemData.ResizeL( 
            sizeof(TInt32) +
            sizeof(TInt32) +
            sizeof(TUint8) +
            sizeof(TInt32) +
            sizeof(TUint16) +
            mimeType.Length() +
            sizeof(TUint16) +
            mimeVer.Length() +
            sizeof(TInt32) +
            dataBuffer->Size()
            );
        }
    
    RBufWriteStream tempStream( aItemData );
    CleanupClosePushL( tempStream );
    tempStream.WriteInt32L( aUid );
    tempStream.WriteInt32L( err );
    if ( !err )
        {
        tempStream.WriteUint8L( fieldChange );
        tempStream.WriteInt32L( parent );
        tempStream.WriteUint16L( mimeType.Length() );
        tempStream.WriteL( mimeType );
        tempStream.WriteUint16L( mimeVer.Length() );
        tempStream.WriteL( mimeVer );
        tempStream.WriteInt32L( dataBuffer->Size() );
        tempStream.WriteL( dataBuffer->Ptr(0), dataBuffer->Size() );
        }
    tempStream.CommitL();
    CleanupStack::PopAndDestroy( &tempStream );
    CleanupStack::PopAndDestroy( dataBuffer );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::CreateItemsL( RReadStream& aData )
    {
    TRACE_FUNC_ENTRY;
    delete iResponseData;
    iResponseData = NULL;
    iResponseData = CBufFlat::NewL(KDefaultExpandSize);
    RBufWriteStream responseStream( *iResponseData );
    CleanupClosePushL( responseStream );
    // Read item uids from parameter
    TInt itemCount = aData.ReadInt32L();
    LOGGER_WRITE_1("itemCount: %d", itemCount);
    responseStream.WriteInt32L( itemCount );
    for ( TInt i=0; i<itemCount; i++ )
        {
        LOGGER_WRITE("read from aData stream");
        TInt tempUid = aData.ReadInt32L();
        
        TSmlDbItemUid parent = aData.ReadInt32L();
        
        TInt len = aData.ReadUint16L();
        HBufC8* mimeTypeBuf = HBufC8::NewLC( len );
        TPtr8 mimeTypePtr = mimeTypeBuf->Des();
        aData.ReadL(mimeTypePtr, len);
        
        len = aData.ReadUint16L();
        HBufC8* mimeVerBuf = HBufC8::NewLC( len );
        TPtr8 mimeVerPtr = mimeVerBuf->Des();
        aData.ReadL(mimeVerPtr, len);
        
        len = aData.ReadInt32L();
        HBufC8* dataBuf = HBufC8::NewLC( len );
        TPtr8 dataPtr = dataBuf->Des();
        aData.ReadL( dataPtr, len );
        
        TSmlDbItemUid newUid(-1);
        LOGGER_WRITE("read from aData stream -readed ok");
        TRAPD( err, iSyncSession.CreateItemL(newUid,
                parent, mimeTypePtr,
                mimeVerPtr, dataPtr));
        
        CleanupStack::PopAndDestroy( dataBuf );
        CleanupStack::PopAndDestroy( mimeVerBuf );
        CleanupStack::PopAndDestroy( mimeTypeBuf );
        LOGGER_WRITE("Write to responseStream");
        responseStream.WriteInt32L( tempUid );
        responseStream.WriteInt32L( err );
        if ( !err )
            {
            responseStream.WriteInt32L( newUid );
            }
        LOGGER_WRITE("Write to responseStream -writed ok");
        }
    
    responseStream.CommitL();
    CleanupStack::PopAndDestroy( &responseStream );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::GetCreateItemsResponseL( const TDesC8& aParams, RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aParams.Length() > 0 )
        {
        LOGGER_WRITE_1("params length: %d", aParams.Length());
        
        //LOGGER_WRITE8_1("aParams: %S", &aParams);
        if ( iSyncStatus == EReady )
            {
            LOGGER_WRITE("Start creating items from file");
            iCreatedItems.Reset();
            const TUint8* ptr = aParams.Ptr();
            RMemReadStream stream( ptr , aParams.Size() );
            CleanupClosePushL( stream );
            TInt filenameLength = stream.ReadInt32L();
            LOGGER_WRITE_1("filename length: %d", filenameLength );
            HBufC8* filenameBuf8 = HBufC8::NewLC( /*stream,*/ filenameLength ); // filename
            TPtr8 filenamePtr = filenameBuf8->Des();
            stream.ReadL( filenamePtr, filenameLength);
            LOGGER_WRITE("filename ok");
            TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(iFileInProgress, filenamePtr );
            LOGGER_WRITE_1("ConvertToUnicodeFromUtf8 err: %d", err);
            User::LeaveIfError( err );
            LOGGER_WRITE_1("iFileInProgress: %S", &iFileInProgress);
            
            if ( iFileInProgress.RightTPtr(KDataFileExtension().Length()).CompareF(KDataFileExtension) != 0 )
                {
                iFileInProgress = KNullDesC();
                LOGGER_WRITE("File extendion was not correct");
                User::Leave( KErrAccessDenied );
                }
            
            CleanupStack::PopAndDestroy( filenameBuf8 );
			
            LOGGER_WRITE("open file");
            err = iFileStream.Open(iFs,iFileInProgress, EFileShareReadersOnly);
            LOGGER_WRITE_1("iFileStream.Open err: %d", err);
            User::LeaveIfError( err );
            
            CleanupStack::PopAndDestroy( &stream );
            
            iSyncStatus = ECreatingItemsFromFile;
            iItemsLeftInStream = iFileStream.ReadInt32L();
            LOGGER_WRITE_1("iItemsLeftInStream: %d", iItemsLeftInStream);
            aResult.WriteInt32L( iItemsLeftInStream );
            aResult.WriteInt32L( 0 ); // completed items since last sync
            CreateNextItemOnStreamL();
            iItemsLeftInStream--;
            }
        else if ( iSyncStatus == ECreatingItemsFromFile || iSyncStatus == EItemsCreated )
            {
            LOGGER_WRITE("Read status");
            TInt temp = 0;
            if ( iSyncStatus == ECreatingItemsFromFile )
                {
                // one item is on progress, add it to to "left" items.
                temp = 1;
                }
            LOGGER_WRITE_1("left items: %d", iItemsLeftInStream + temp);
            LOGGER_WRITE_1("completed items: %d", iCreatedItems.Count());
            aResult.WriteInt32L( iItemsLeftInStream + temp ); // items in progress
            aResult.WriteInt32L( iCreatedItems.Count() ); // completed items since last get
            
            while ( iCreatedItems.Count() > 0 )
                {
                aResult.WriteInt32L( iCreatedItems[0].iTemporaryId );
                aResult.WriteInt32L( iCreatedItems[0].iErrorCode );
                if ( !iCreatedItems[0].iErrorCode )
                    {
                    aResult.WriteInt32L( iCreatedItems[0].iNewUid );
                    }
                iCreatedItems.Remove(0);
                }
            if ( iSyncStatus == EItemsCreated )
                {
                // all done
                iSyncStatus = EReady;
                LOGGER_WRITE("iSyncStatus = EReady");
                }
            }
        else
            {
            LOGGER_WRITE_1("Wrong sync status: %d", iSyncStatus);
            User::Leave( KErrInUse );
            }
        }
    else
        {
        LOGGER_WRITE("Read response");
        if ( !iResponseData )
            {
            User::Leave( KErrNotReady );
            }
        aResult.WriteL( iResponseData->Ptr(0), iResponseData->Size() );
        delete iResponseData;
        iResponseData = NULL;
        }
    aResult.CommitL();
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::ReplaceItemsL( RReadStream& aData )
    {
    TRACE_FUNC_ENTRY;
    delete iResponseData;
    iResponseData = NULL;
    iResponseData = CBufFlat::NewL(KDefaultExpandSize);
    RBufWriteStream responseStream( *iResponseData );
    CleanupClosePushL( responseStream );
    // Read item uids from parameter
    TInt itemCount = aData.ReadInt32L();
    
    responseStream.WriteInt32L( itemCount );
    for ( TInt i=0; i<itemCount; i++ )
        {
        TSmlDbItemUid uid = aData.ReadInt32L();
        TSmlDbItemUid parent = aData.ReadInt32L();
        TBool fieldChange = aData.ReadUint8L();
        TInt len = aData.ReadInt32L();
        HBufC8* dataBuf = HBufC8::NewLC( len );
        TPtr8 dataPtr = dataBuf->Des();
        aData.ReadL( dataPtr, len );
        
        TRAPD( err, iSyncSession.ReplaceItemL(uid, 
                parent, 
                fieldChange, dataPtr));
        CleanupStack::PopAndDestroy( dataBuf );
        
        responseStream.WriteInt32L( uid );
        responseStream.WriteInt32L( err );
        }
    responseStream.CommitL();
    CleanupStack::PopAndDestroy( &responseStream );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::GetReplaceItemsResponseL( const TDesC8& aParams, RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( aParams.Length() > 0 )
        {
        LOGGER_WRITE8_1("aParams: %S", &aParams);
        if ( iSyncStatus == EReady )
            {
            LOGGER_WRITE("Start replacing items from file");
            iReplacedItems.Reset();
            
            // create parameter stream reader
            const TUint8* ptr = aParams.Ptr();
            RMemReadStream stream( ptr , aParams.Size() );
            CleanupClosePushL( stream );
            
            // read filename
            TInt filenameLength = stream.ReadInt32L();
            LOGGER_WRITE_1("filename length: %d", filenameLength );
            HBufC8* filenameBuf8 = HBufC8::NewLC( filenameLength ); // filename
            TPtr8 filenamePtr = filenameBuf8->Des();
            stream.ReadL( filenamePtr, filenameLength);
            LOGGER_WRITE("filename ok");
            
            TInt err = CnvUtfConverter::ConvertToUnicodeFromUtf8(iFileInProgress, filenameBuf8->Des() );
            LOGGER_WRITE_1("ConvertToUnicodeFromUtf8 err: %d", err);
            User::LeaveIfError( err );
            if ( iFileInProgress.RightTPtr(KDataFileExtension().Length()).CompareF(KDataFileExtension) != 0 )
                {
                iFileInProgress = KNullDesC();
                LOGGER_WRITE("File extendion was not correct");
                User::Leave( KErrAccessDenied );
                }
            
            // open file
            err = iFileStream.Open(iFs, iFileInProgress, EFileShareReadersOnly);
            LOGGER_WRITE_1("iFileStream.Open err: %d", err);
            User::LeaveIfError( err );
            
            CleanupStack::PopAndDestroy( filenameBuf8 );
            CleanupStack::PopAndDestroy( &stream );
            
            iSyncStatus = EReplacingItemsFromFile;
            iItemsLeftInStream = iFileStream.ReadInt32L();
            LOGGER_WRITE_1("iItemsLeftInStream: %d", iItemsLeftInStream);
            // write results to return stream
            aResult.WriteInt32L( iItemsLeftInStream );
            aResult.WriteInt32L( 0 ); // completed items since last sync
            ReplaceNextItemOnStreamL();
            iItemsLeftInStream--;
            }
        else if ( iSyncStatus == EReplacingItemsFromFile || iSyncStatus == EItemsReplaced )
            {
            LOGGER_WRITE("Read status");
            TInt temp = 0;
            if ( iSyncStatus == EReplacingItemsFromFile )
                {
                // one item is on progress, add it to to "left" items.
                temp = 1;
                }
            LOGGER_WRITE_1("left items: %d", iItemsLeftInStream + temp);
            LOGGER_WRITE_1("completed items: %d", iReplacedItems.Count());
            aResult.WriteInt32L( iItemsLeftInStream + temp ); // items in progress
            aResult.WriteInt32L( iReplacedItems.Count() ); // completed items since last get
            //for (TInt i=0; i<iCreatedItems.Count(); i++)
            while ( iReplacedItems.Count() > 0 )
                {
                aResult.WriteInt32L( iReplacedItems[0].iItemUid );
                aResult.WriteInt32L( iReplacedItems[0].iErrorCode );
                iReplacedItems.Remove(0);
                }
            if ( iSyncStatus == EItemsReplaced )
                {
                // all done
                iSyncStatus = EReady;
                LOGGER_WRITE("iSyncStatus = EReady");
                }
            }
        }
    else
        {
        if ( !iResponseData )
            {
            LOGGER_WRITE("No response data, leave KErrNotReady");
            User::Leave( KErrNotReady );
            }
        aResult.WriteL( iResponseData->Ptr(0), iResponseData->Size() );
        aResult.CommitL();
        delete iResponseData;
        iResponseData = NULL;
        }
    aResult.CommitL();
    
    
    
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::MoveItemsL( const TDesC8& aParams, RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;  
    const TUint8* ptr = aParams.Ptr();
    RMemReadStream stream( ptr , aParams.Size() );
    CleanupClosePushL( stream );
    
    // Read item uids from parameter
    TInt itemCount = stream.ReadInt32L();
    aResult.WriteInt32L( itemCount );
    for ( TInt i=0; i<itemCount; i++ )
        {
        TSmlDbItemUid uid = stream.ReadInt32L();
        TSmlDbItemUid newParent = stream.ReadInt32L();
        TRAPD( err, iSyncSession.MoveItemL( uid,newParent ));
        
        aResult.WriteInt32L( uid );
        aResult.WriteInt32L( err );
        }
    aResult.CommitL();
    CleanupStack::PopAndDestroy( &stream );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::DeleteItemsL( const TDesC8& aParams, RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    const TUint8* ptr = aParams.Ptr();
    RMemReadStream stream( ptr , aParams.Size() );
    CleanupClosePushL( stream );
    
    // Read item uids from parameter
    TInt itemCount = stream.ReadInt32L();
    aResult.WriteInt32L( itemCount );
    for ( TInt i=0; i<itemCount; i++ )
        {
        TSmlDbItemUid uid = stream.ReadInt32L();
        
        TRAPD( err, iSyncSession.DeleteItemL( uid ));
        
        aResult.WriteInt32L( uid );
        aResult.WriteInt32L( err );
        }
    aResult.CommitL();
    CleanupStack::PopAndDestroy( &stream );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::SoftDeleteItemsL( const TDesC8& aParams, RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    const TUint8* ptr = aParams.Ptr();
    RMemReadStream stream( ptr , aParams.Size() );
    CleanupClosePushL( stream );
    
    // Read item uids from parameter
    TInt itemCount = stream.ReadInt32L();
    aResult.WriteInt32L( itemCount );
    for ( TInt i=0; i<itemCount; i++ )
        {
        TSmlDbItemUid uid = stream.ReadInt32L();
        
        TRAPD( err, iSyncSession.SoftDeleteItemL( uid ));
        
        aResult.WriteInt32L( uid );
        aResult.WriteInt32L( err );
        }
    aResult.CommitL();
    CleanupStack::PopAndDestroy( &stream );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::DeleteAllItemsL( RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    if ( IsActive() )
        {
        LOGGER_WRITE("Warning: Was on active state!");
        Cancel();
        }
    SetActive();
    iStatus = KRequestPending;
    iSyncStatus = EDeletingAllItems;
    iSyncSession.DeleteAllItems( iStatus );
    aResult.WriteInt32L( KErrNone );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::GetDeleteAllItemsStatusL( RWriteStream& aResult )
    {
    if ( iSyncStatus == EDeletingAllItems )
        {
        LOGGER_WRITE("CSconSyncHandler::GetDeleteAllItemsStatusL - In progress");
        aResult.WriteInt8L( 1 ); // 1 = in progress
        }
    else
        {
        LOGGER_WRITE("CSconSyncHandler::GetDeleteAllItemsStatusL - Ready");
        aResult.WriteInt8L( 0 ); // 0 = ready
        aResult.WriteInt32L( iStatus.Int() );
        }
    }

void CSconSyncHandler::GetStoreFormatL( RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    RStringPool pool;
    pool.OpenL();
    CleanupClosePushL( pool );
    CSmlDataStoreFormat* storeFormat(NULL);
    TRAPD( err, storeFormat = iSyncSession.StoreFormatL( pool ) );
    CleanupStack::PushL( storeFormat );
    aResult.WriteInt32L( err );
    if ( !err )
        {
        storeFormat->ExternalizeL( aResult );
        }
    CleanupStack::PopAndDestroy( storeFormat );
    CleanupStack::PopAndDestroy( &pool );
    TRACE_FUNC_EXIT;
    }


void CSconSyncHandler::SetRemoteStoreFormatL( RReadStream& aData )
    {
    TRACE_FUNC_ENTRY;
    RStringPool pool;
    pool.OpenL();
    CleanupClosePushL( pool );
    CSmlDataStoreFormat* serverDataStoreFormat = CSmlDataStoreFormat::NewLC( pool, aData );
    iSyncSession.SetRemoteStoreFormatL( *serverDataStoreFormat );
    // Save format for later use
    TRAPD(err, SaveRemoteStoreFormatL( *serverDataStoreFormat, iCurrentContextUid, iCurrentDataProviderUid ));
    if ( err )
        {
        LOGGER_WRITE_1("SaveRemoteStoreFormatL err: %d", err);
        }
    CleanupStack::PopAndDestroy( serverDataStoreFormat );
    CleanupStack::PopAndDestroy( &pool );
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::CreateNextItemOnStreamL()
    {
    TRACE_FUNC_ENTRY;
    if ( IsActive() )
        {
        LOGGER_WRITE("Warning: Was on active state!");
        Cancel();
        }
    iTemporaryItemUid = iFileStream.ReadInt32L();
    TSmlDbItemUid parent = iFileStream.ReadInt32L();
    
    TInt len = iFileStream.ReadUint16L();
    HBufC8* mimeTypeBuf = HBufC8::NewLC( len );
    TPtr8 mimeTypePtr = mimeTypeBuf->Des();
    iFileStream.ReadL(mimeTypePtr, len);
    
    len = iFileStream.ReadUint16L();
    HBufC8* mimeVerBuf = HBufC8::NewLC( len );
    TPtr8 mimeVerPtr = mimeVerBuf->Des();
    iFileStream.ReadL(mimeVerPtr, len);
    
    len = iFileStream.ReadInt32L();
    HBufC8* dataBuf = HBufC8::NewLC( len );
    TPtr8 dataPtr = dataBuf->Des();
    iFileStream.ReadL( dataPtr, len );    
    
    SetActive();
    iStatus = KRequestPending;
    iSyncSession.CreateItemL(iCreatedItemUidPckg,
            parent, mimeTypePtr,
            mimeVerPtr, dataPtr, iStatus);
    
    CleanupStack::PopAndDestroy( dataBuf );
    CleanupStack::PopAndDestroy( mimeVerBuf );
    CleanupStack::PopAndDestroy( mimeTypeBuf );
    
   
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::ReplaceNextItemOnStreamL()
    {
    TRACE_FUNC_ENTRY;
    if ( IsActive() )
        {
        LOGGER_WRITE("Warning: Was on active state!");
        Cancel();
        }
    TSmlDbItemUid uid = iFileStream.ReadInt32L();
    TSmlDbItemUid parent = iFileStream.ReadInt32L();
    TBool fieldChange = iFileStream.ReadUint8L();
    TInt len = iFileStream.ReadInt32L();
    HBufC8* dataBuf = HBufC8::NewLC( len );
    TPtr8 dataPtr = dataBuf->Des();
    iFileStream.ReadL( dataPtr, len );
    
    iReplacedItem.iItemUid = uid;
    
    SetActive();
    iStatus = KRequestPending;
    
    iSyncSession.ReplaceItemL(uid, 
                parent, 
                fieldChange, dataPtr, iStatus);
    
    CleanupStack::PopAndDestroy( dataBuf );
   
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::CancelOperationsL( RWriteStream& aResult )
    {
    TRACE_FUNC_ENTRY;
    Cancel();
    // all done
    iFileStream.Close();
    iTemporaryItemUid = -1;
    iCreatedItemUid = -1;
    iSyncStatus = EItemsCreated;
    iFs.Delete( iFileInProgress );
    iFileInProgress = KNullDesC();
    iItemsLeftInStream = 0;
    _LIT8(KDummyParam, "Cancel");
    if ( iSyncStatus == EItemsCreated )
        {
        GetCreateItemsResponseL( KDummyParam, aResult );
        }
    else if ( iSyncStatus == EItemsReplaced )
        {
        GetReplaceItemsResponseL( KDummyParam, aResult );
        }
    
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::DoCancel()
    {
    TRACE_FUNC_ENTRY;
    iSyncSession.CancelRequest();
    Reset();
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::RunL()
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("iSyncStatus: %d", iSyncStatus);
    LOGGER_WRITE_1("iStatus: %d", iStatus.Int());
    if ( iSyncStatus == ECreatingItemsFromFile )
        {
        LOGGER_WRITE_1("iTemporaryItemUid: %d", iTemporaryItemUid);
        LOGGER_WRITE_1("iCreatedItemUid: %d", iCreatedItemUid);
        TCreatedItem item;
        item.iTemporaryId = iTemporaryItemUid;
        item.iErrorCode = iStatus.Int();
        item.iNewUid = iCreatedItemUid;
        iCreatedItems.Append( item );
        if ( iItemsLeftInStream > 0 )
            {
            CreateNextItemOnStreamL();
            iItemsLeftInStream--;
            LOGGER_WRITE_1("iItemsLeftInStream: %d", iItemsLeftInStream);
            }
        else
            {
            // all done
            LOGGER_WRITE("All items added.");
            iFileStream.Close();
            iTemporaryItemUid = -1;
            iCreatedItemUid = -1;
            iSyncStatus = EItemsCreated;
            TInt err = iFs.Delete( iFileInProgress );
            LOGGER_WRITE_2("iFs.Delete( '%S' ) ret: %d", &iFileInProgress, err);
            iFileInProgress = KNullDesC();
            }
        }
    else if ( iSyncStatus == EReplacingItemsFromFile )
        {
        iReplacedItem.iErrorCode = iStatus.Int();
        iReplacedItems.Append( iReplacedItem );
        if ( iItemsLeftInStream > 0 )
            {
            ReplaceNextItemOnStreamL();
            iItemsLeftInStream--;
            LOGGER_WRITE_1("iItemsLeftInStream: %d", iItemsLeftInStream);
            }
        else
            {
            // all done
            LOGGER_WRITE("All items replaced.");
            iFileStream.Close();
            iSyncStatus = EItemsReplaced;
            TInt err = iFs.Delete( iFileInProgress );
            LOGGER_WRITE_2("iFs.Delete( '%S' ) ret: %d", &iFileInProgress, err);
            iFileInProgress = KNullDesC();
            }
        }
    else if ( iSyncStatus == EDeletingAllItems )
        {
        iSyncStatus = EReady;
        }
    TRACE_FUNC_EXIT;
    }

TInt CSconSyncHandler::RunError( TInt aError )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aError: %d", aError);
    LOGGER_WRITE_1("iSyncStatus: %d", iSyncStatus);
    TInt err( aError );
    if ( iSyncStatus == ECreatingItemsFromFile )
        {
        iSyncStatus = EItemsCreated;
        iItemsLeftInStream = 0;
        iFileStream.Close();
        err = iFs.Delete( iFileInProgress );
        LOGGER_WRITE_2("iFs.Delete( '%S' ) ret: %d", &iFileInProgress, err);
        iFileInProgress = KNullDesC();
        err = KErrNone;
        }
    else if ( iSyncStatus == EReplacingItemsFromFile )
        {
        iSyncStatus = EItemsReplaced;
        iItemsLeftInStream = 0;
        iFileStream.Close();
        err = iFs.Delete( iFileInProgress );
        LOGGER_WRITE_2("iFs.Delete( '%S' ) ret: %d", &iFileInProgress, err);
        iFileInProgress = KNullDesC();
        err = KErrNone;
        }
    else if ( iSyncStatus == EDeletingAllItems )
        {
        iSyncStatus = EReady;
        }
    
    TRACE_FUNC_EXIT;
    return err;
    }

void CSconSyncHandler::Reset()
    {
    TRACE_FUNC_ENTRY;
    iFileStream.Close();
    TInt err = iFs.Delete( iFileInProgress );
    LOGGER_WRITE_2("iFs.Delete( '%S' ) ret: %d", &iFileInProgress, err);
    iFileInProgress = KNullDesC();
    iSyncStatus = EReady;
    TRACE_FUNC_EXIT;
    }

void CSconSyncHandler::SetCalendarCenrepL( TInt aProfileId, const TDesC& aServerId )
    {
    LOGGER_WRITE(" Write cenrep values");
    CRepository* rep = CRepository::NewLC(KNsmlDsSessionInfoKey);
    User::LeaveIfError( rep->Set(EDSSessionProfileId, aProfileId) );
    User::LeaveIfError( rep->Set(EDSSessionProfileName, KNullDesC) );
    User::LeaveIfError( rep->Set(EDSSessionServerId, aServerId) );
    CleanupStack::PopAndDestroy(rep);
    iCalendarCenrepUsed = ETrue;
    }

void CSconSyncHandler::ClearCalendarCenrepL()
    {
    if ( iCalendarCenrepUsed )
        {
        LOGGER_WRITE("Calendar sync, clear cenrep values");
                
        CRepository* rep = CRepository::NewLC(KNsmlDsSessionInfoKey);
        rep->Set(EDSSessionProfileId, KErrNotFound);
        rep->Set(EDSSessionProfileName, KNullDesC);
        rep->Set(EDSSessionServerId, KNullDesC);
        CleanupStack::PopAndDestroy(rep);
        iCalendarCenrepUsed = EFalse;
        }
    }

