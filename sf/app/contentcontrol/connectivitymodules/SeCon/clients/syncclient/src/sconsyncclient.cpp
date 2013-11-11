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
* Description:  RSconSyncSession implementation
*
*/


#include "sconsyncclient.h"

#include <SmlDataProvider.h>
#include <s32mem.h>

#include "sconsyncclientserver.h"
#include "debug.h"

const TInt KSConSyncChunkSize = 0xFFFF; // 65535 bytes;
const TInt KSConSyncChunkMaxSize = 0x400000; // 4194304 bytes

EXPORT_C RSconSyncSession::RSconSyncSession()
    {
    TRACE_FUNC;
    }

EXPORT_C RSconSyncSession::~RSconSyncSession()
    {
    TRACE_FUNC;
    }

EXPORT_C TInt RSconSyncSession::Connect()
    {
    TRACE_FUNC_ENTRY;
    TInt retryCount = 2;
    TInt error = KErrNone;
    while(retryCount)
        {
        error = CreateSession(KSconSyncServerName, TVersion(1,0,0));
        if ( error != KErrNotFound && error != KErrServerTerminated )
            {
            break;
            }
        error = StartServer();
        if( error != KErrNone && error != KErrAlreadyExists )
            {
            break;
            }
        --retryCount;
        }
    
    if ( error == KErrNone )
        {
        error = CreateAndSendChunkHandle();
        if ( error != KErrNone )
            {
            LOGGER_WRITE("CreateAndSendChunkHandle failed, close session");
            Close();
            }
        }
    TRACE_FUNC_EXIT;
    return error;
    }

// -----------------------------------------------------------------------------
// RSconSyncSession::Close()
// Closes the server connection
// -----------------------------------------------------------------------------
//
EXPORT_C void RSconSyncSession::Close()
    {
    TRACE_FUNC_ENTRY;
    iChunk.Close();
    RSessionBase::Close();
    TRACE_FUNC_EXIT
    }

EXPORT_C void RSconSyncSession::ListAllImplementationsL( RSconDataProviderInfoArray& aInfoArray )
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    TInt ret = SendReceive ( EListImplementations, args );
    
    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSconSyncSession::ListAllImplementationsL() :\
         Send Receive failed with code %d", ret );
        User::Leave( ret );
        };
    
    // copy data from the chunk
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    TInt count( readStream.ReadUint16L() );
    LOGGER_WRITE_1("count: %d", count);
    for ( TInt i=0; i<count; i++ )
        {
        LOGGER_WRITE_1("Import[%d]", i);
        CSconDataproviderInfo *dpInfo = CSconDataproviderInfo::NewL();
        CleanupStack::PushL( dpInfo );
        
        dpInfo->InternalizeL( readStream );
        aInfoArray.AppendL( dpInfo );
        CleanupStack::Pop( dpInfo );
        }
    
    CleanupStack::PopAndDestroy( &readStream );

    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::OpenDataStoreL( const TSmlDataProviderId aProviderId, const TDesC& aStoreName, TInt aContextId )
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( iChunk.Adjust( 
            sizeof(TInt32)
            + sizeof(TInt32)
            + aStoreName.Size()
            + sizeof(TInt) ));
    
    RMemWriteStream writeBuf( iChunk.Base(), iChunk.Size() );
    writeBuf.WriteInt32L( aProviderId );
    
    writeBuf.WriteInt32L( aStoreName.Length() );
    writeBuf.WriteL( aStoreName );
    
    writeBuf.WriteInt32L( aContextId );

    writeBuf.CommitL();
    writeBuf.Close();
    
    TIpcArgs args;
    TInt ret = SendReceive ( EOpenStore, args );
    
    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSconSyncSession::OpenDataStoreL() :\
         Send Receive failed with code %d", ret );
        User::Leave( ret );
        };
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::OpenItemL( TSmlDbItemUid aUid, TBool& aFieldChange,
        TSmlDbItemUid& aParent, TDes8& aMimeType, TDes8& aMimeVer, CBufFlat& aItemData )
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args( aUid );
    TInt ret = SendReceive ( EOpenItem, args );
    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSconSyncSession::OpenItemL() :\
         Send Receive failed with code %d", ret );
        User::Leave( ret );
        };
    
    // copy data from the chunk
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    TInt32 length ( 0 );

    aItemData.Reset();
    length = readStream.ReadInt32L();
    
    HBufC8* data = HBufC8::NewLC( length );
    TPtr8 dataPtr = data->Des();
    
    readStream.ReadL( dataPtr, length );
    aItemData.ExpandL( 0, length );
    aItemData.Write ( 0, dataPtr );
    
    CleanupStack::PopAndDestroy( data );

    aFieldChange = readStream.ReadInt8L();
    
    aParent = readStream.ReadInt32L();
    length = readStream.ReadInt32L();
    readStream.ReadL( aMimeType, length );
    
    length = readStream.ReadInt32L();
    readStream.ReadL( aMimeVer, length );
    
    CleanupStack::PopAndDestroy( &readStream );
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::CreateItemL(TSmlDbItemUid& aUid/*, TInt aSize*/,
        TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& aMimeVer,
        const TDesC8& aData)
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( iChunk.Adjust( 
        sizeof(TInt32)
        + sizeof(TInt32)
        + aMimeType.Length()
        + sizeof(TInt32)
        + aMimeVer.Length()
        + sizeof(TInt32)
        + aData.Length() ));
    RMemWriteStream writeBuf( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( writeBuf );
    writeBuf.WriteInt32L( aParent );
    writeBuf.WriteInt32L( aMimeType.Length() );
    writeBuf.WriteL( aMimeType );
    writeBuf.WriteInt32L( aMimeVer.Length() );
    writeBuf.WriteL( aMimeVer );
    writeBuf.WriteInt32L( aData.Length() );
    writeBuf.WriteL( aData );
        
    writeBuf.CommitL();
    CleanupStack::PopAndDestroy( &writeBuf );
    

    
    TPckg<TSmlDbItemUid> pckg(aUid);
    iArgs = TIpcArgs( &pckg );
    
    TInt ret = SendReceive ( ECreateItem, iArgs );
    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSconSyncSession::CreateItemL() :\
         Send Receive failed with code %d", ret );
        User::Leave( ret );
        };
    LOGGER_WRITE_1("aUid: %d", aUid);
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::CreateItemL(TPckg<TSmlDbItemUid>& aUidPckg,
        TSmlDbItemUid aParent, const TDesC8& aMimeType, const TDesC8& aMimeVer,
        const TDesC8& aData, TRequestStatus& aStatus)
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( iChunk.Adjust( 
        sizeof(TInt32)
        + sizeof(TInt32)
        + aMimeType.Length()
        + sizeof(TInt32)
        + aMimeVer.Length()
        + sizeof(TInt32)
        + aData.Length() ));
    RMemWriteStream writeBuf( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( writeBuf );
    writeBuf.WriteInt32L( aParent );
    writeBuf.WriteInt32L( aMimeType.Length() );
    writeBuf.WriteL( aMimeType );
    writeBuf.WriteInt32L( aMimeVer.Length() );
    writeBuf.WriteL( aMimeVer );
    writeBuf.WriteInt32L( aData.Length() );
    writeBuf.WriteL( aData );
        
    writeBuf.CommitL();
    CleanupStack::PopAndDestroy( &writeBuf );
    
    iArgs = TIpcArgs( &aUidPckg );
    SendReceive( ECreateItem, iArgs, aStatus );
    
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::ReplaceItemL(TSmlDbItemUid aUid,
        TSmlDbItemUid aParent, TBool aFieldChange, const TDesC8& aData)
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( iChunk.Adjust( 
        sizeof(TInt32)
        + sizeof(TInt32)
        + sizeof(TInt8)
        + sizeof(TInt32)
        + aData.Length() ));
    RMemWriteStream writeBuf( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( writeBuf );
    writeBuf.WriteInt32L( aUid );
    writeBuf.WriteInt32L( aParent );
    writeBuf.WriteInt8L( aFieldChange );
    writeBuf.WriteInt32L( aData.Length() );
    writeBuf.WriteL( aData );
    writeBuf.CommitL();
    CleanupStack::PopAndDestroy( &writeBuf );
    
    TIpcArgs args;
    TInt ret = SendReceive( EReplaceItem, args );
    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSconSyncSession::ReplaceItemL() :\
         Send Receive failed with code %d", ret );
        User::Leave( ret );
        };
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::ReplaceItemL(TSmlDbItemUid aUid,
        TSmlDbItemUid aParent, TBool aFieldChange, const TDesC8& aData,
        TRequestStatus& aStatus)
    {
    TRACE_FUNC_ENTRY;
    User::LeaveIfError( iChunk.Adjust( 
        sizeof(TInt32)
        + sizeof(TInt32)
        + sizeof(TInt8)
        + sizeof(TInt32)
        + aData.Length() ));
    RMemWriteStream writeBuf( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( writeBuf );
    writeBuf.WriteInt32L( aUid );
    writeBuf.WriteInt32L( aParent );
    writeBuf.WriteInt8L( aFieldChange );
    writeBuf.WriteInt32L( aData.Length() );
    writeBuf.WriteL( aData );
    writeBuf.CommitL();
    CleanupStack::PopAndDestroy( &writeBuf );
    
    TIpcArgs args;
    SendReceive( EReplaceItem, args, aStatus );
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::MoveItemL( TSmlDbItemUid aUid, TSmlDbItemUid aNewParent )
    {
    TRACE_FUNC_ENTRY;
    
    TIpcArgs args(aUid, aNewParent);
    TInt ret = SendReceive ( EMoveItem, args );
    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSconSyncSession::MoveItemL() :\
         Send Receive failed with code %d", ret );
        User::Leave( ret );
        };
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::DeleteItemL( TSmlDbItemUid aUid )
    {
    TRACE_FUNC_ENTRY;
    
    TIpcArgs args( aUid );
    TInt ret = SendReceive ( EDeleteItem, args );
    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSconSyncSession::DeleteItemL() :\
         Send Receive failed with code %d", ret );
        User::Leave( ret );
        };
    TRACE_FUNC_EXIT;
    }


EXPORT_C void RSconSyncSession::SoftDeleteItemL( TSmlDbItemUid aUid )
    {
    TRACE_FUNC_ENTRY;
    
    TIpcArgs args( aUid );
    TInt ret = SendReceive ( ESoftDeleteItem, args );
    if ( ret != KErrNone) 
        {
        LOGGER_WRITE_1( "RSconSyncSession::SoftDeleteItemL() :\
         Send Receive failed with code %d", ret );
        User::Leave( ret );
        };
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::DeleteAllItems( TRequestStatus& aStatus )
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    SendReceive( EDeleteAllItems, args, aStatus );
    TRACE_FUNC_EXIT;
    }

    
EXPORT_C TBool RSconSyncSession::HasSyncHistoryL() const
    {
    TRACE_FUNC;
    TBool history(EFalse);
    TPckg<TBool> pck(history);
    TIpcArgs args(&pck);
    TInt ret = SendReceive ( EHasHistory, args );
    User::LeaveIfError( ret );
    TRACE_FUNC_EXIT;
    return history;
    }

EXPORT_C void RSconSyncSession::AddedItemsL( RArray<TSmlDbItemUid>& aItems) const
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    TInt ret = SendReceive ( EAddedItems, args );
    User::LeaveIfError( ret );
    
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    aItems.Reset();
    TInt itemCount = readStream.ReadInt32L();
    for (TInt i=0; i<itemCount; i++)
        {
        aItems.AppendL( readStream.ReadInt32L() );
        }
    
    CleanupStack::PopAndDestroy( &readStream );
    
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::DeletedItemsL( RArray<TSmlDbItemUid>& aItems) const
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    TInt ret = SendReceive ( EDeletedItems, args );
    User::LeaveIfError( ret );
    
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    aItems.Reset();
    TInt itemCount = readStream.ReadInt32L();
    for (TInt i=0; i<itemCount; i++)
        {
        aItems.AppendL( readStream.ReadInt32L() );
        }
    
    CleanupStack::PopAndDestroy( &readStream );
    
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::SoftDeletedItemsL( RArray<TSmlDbItemUid>& aItems) const
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    TInt ret = SendReceive ( ESoftDeletedItems, args );
    User::LeaveIfError( ret );
    
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    aItems.Reset();
    TInt itemCount = readStream.ReadInt32L();
    for (TInt i=0; i<itemCount; i++)
        {
        aItems.AppendL( readStream.ReadInt32L() );
        }
    
    CleanupStack::PopAndDestroy( &readStream );
    
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::ModifiedItemsL( RArray<TSmlDbItemUid>& aItems) const
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    TInt ret = SendReceive ( EModifiedItems, args );
    User::LeaveIfError( ret );
    
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    aItems.Reset();
    TInt itemCount = readStream.ReadInt32L();
    for (TInt i=0; i<itemCount; i++)
        {
        aItems.AppendL( readStream.ReadInt32L() );
        }
    
    CleanupStack::PopAndDestroy( &readStream );
    
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::MovedItemsL( RArray<TSmlDbItemUid>& aItems) const
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    TInt ret = SendReceive ( EMovedItems, args );
    User::LeaveIfError( ret );
    
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    
    aItems.Reset();
    TInt itemCount = readStream.ReadInt32L();
    for (TInt i=0; i<itemCount; i++)
        {
        aItems.AppendL( readStream.ReadInt32L() );
        }
    
    CleanupStack::PopAndDestroy( &readStream );
    
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::CloseDataStore() const
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    SendReceive ( ECloseStore, args );
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::ResetChangeInfoL()
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    TInt ret = SendReceive ( EResetChangeInfo, args );
    User::LeaveIfError( ret );
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::CommitChangeInfoL(const RArray<TSmlDbItemUid>& aItems )
    {
    TRACE_FUNC_ENTRY;
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    LOGGER_WRITE_1("items count: %d", aItems.Count());
    stream.WriteInt32L( aItems.Count() );
    for (TInt i=0; i < aItems.Count(); i++ )
        {
        LOGGER_WRITE("Write item");
        stream.WriteInt32L( aItems[i] );
        }
    stream.CommitL();
    CleanupStack::PopAndDestroy( &stream );
        
    TIpcArgs args;
    TInt ret = SendReceive ( ECommitChangeInfo, args );
    User::LeaveIfError( ret );
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::GetSyncTimeStampL( const TSmlDataProviderId aProviderId, const TInt aContextId, TDateTime& aTimeStamp ) const
    {
    TRACE_FUNC_ENTRY;
    
    TPckg<TDateTime> timeBuf( aTimeStamp );
    TIpcArgs args( aProviderId, aContextId, &timeBuf );
    TInt ret = SendReceive ( EGetSyncTimeStamp, args );
    User::LeaveIfError( ret );
    
    TRACE_FUNC_EXIT;
    }

EXPORT_C CSmlDataStoreFormat* RSconSyncSession::StoreFormatL( const RStringPool& aStringPool )
    {
    TRACE_FUNC_ENTRY;
    TIpcArgs args;
    TInt ret = SendReceive ( EExportStoreFormat, args );
    User::LeaveIfError( ret );
    
    RMemReadStream readStream( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( readStream );
    TInt len = readStream.ReadInt32L();
    LOGGER_WRITE_1("data size from server: %d", len);
    LOGGER_WRITE("CSmlDataStoreFormat::NewLC");
    CSmlDataStoreFormat* tempStoreFormat = CSmlDataStoreFormat::NewLC( aStringPool, readStream );
    LOGGER_WRITE("CSmlDataStoreFormat::NewLC -ok");
    CleanupStack::Pop( tempStoreFormat );
    
    LOGGER_WRITE("PopAndDestroy( &readStream )");
    CleanupStack::PopAndDestroy( &readStream );
    LOGGER_WRITE("return");
    TRACE_FUNC_EXIT;
    return tempStoreFormat;
    }

EXPORT_C void RSconSyncSession::SetRemoteStoreFormatL( const CSmlDataStoreFormat& aServerDataStoreFormat )
    {
    TRACE_FUNC_ENTRY;
    const TInt KMaximumDataStoreFormatSize = 100000; // 100kb should be enought
    User::LeaveIfError( iChunk.Adjust( KMaximumDataStoreFormatSize ));
    
    RMemWriteStream stream ( iChunk.Base(), iChunk.Size() );
    CleanupClosePushL( stream );
    
    aServerDataStoreFormat.ExternalizeL( stream );
    
    stream.CommitL();
    CleanupStack::PopAndDestroy( &stream );
    
    TIpcArgs args;
    TInt ret = SendReceive ( ESetRemoteStoreFormat, args );
    User::LeaveIfError( ret );
    TRACE_FUNC_EXIT;
    }

EXPORT_C void RSconSyncSession::CancelRequest()
    {
    TRACE_FUNC_ENTRY;
    SendReceive( ECancelRequest, TIpcArgs() );
    TRACE_FUNC_EXIT;
    }

EXPORT_C TInt RSconSyncSession::GetParent( TSmlDbItemUid aUid, TSmlDbItemUid& aParent )
    {
    TRACE_FUNC_ENTRY;
    TPckg<TSmlDbItemUid> pckg(aParent);
    TIpcArgs args( aUid, &pckg );
    TInt ret = SendReceive( EReadParent, args );
    TRACE_FUNC_EXIT;
    return ret;
    }

TInt RSconSyncSession::StartServer()
    {
    TRACE_FUNC_ENTRY;
    
    RProcess server;
    TInt error = server.Create(KSconSyncServerExe, KNullDesC);
    if ( error != KErrNone )
        {
        return error;
        }
    // start server and wait for signal before proceeding    
    TRequestStatus status;
    server.Rendezvous(status);
    if ( status.Int() != KRequestPending )
        {
        server.Kill(0);
        }
    else
        {
        server.Resume();
        }
    
    User::WaitForRequest( status );
    error = server.ExitType() == EExitPanic ? KErrGeneral : status.Int();
    server.Close();
    TRACE_FUNC_EXIT;
    return error;
    }

// -----------------------------------------------------------------------------
// RSconSyncSession::CreateAndSendChunkHandle()
// Creates a chunk and sends a handle to server
// -----------------------------------------------------------------------------
//  
TInt RSconSyncSession::CreateAndSendChunkHandle()
    {
    TRACE_FUNC_ENTRY;
    
    TInt err = iChunk.CreateGlobal( KNullDesC, 
                                    KSConSyncChunkSize, 
                                    KSConSyncChunkMaxSize );
    if ( err != KErrNone )
        {
        LOGGER_WRITE_1("iChunk.CreateGlobal failed, err: %d", err);
        return err;
        }                           
    TIpcArgs args;
    args.Set( 0, iChunk );
    err = SendReceive( ESendChunkHandle, args );
    
    LOGGER_WRITE_1("RSconSyncSession::CreateAndSendChunkHandle : returned: %d", err);
    return err;
    }

// End of File
