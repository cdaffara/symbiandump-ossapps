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
* Description:  CSconSyncRelationship implementation
*
*/


#include "sconsyncrelationship.h"
#include <s32file.h>
#include <nsmlchangefinder.h>

#include "debug.h"

_LIT(KContextStore, "contextstore_0x%08x.dat");
_LIT(KTimeStore, "timestore_0x%08x.dat");

CSconSyncRelationship::~CSconSyncRelationship()
    {
    TRACE_FUNC_ENTRY;
    if ( iDictionaryStore )
        {
        iDictionaryStore->Commit();
        delete iDictionaryStore;
        }
    TRACE_FUNC_EXIT;
    }

CSconSyncRelationship::CSconSyncRelationship( RFs& aFs, TUid aRelationUid, TSmlDataProviderId aProviderId )
    : iFs(aFs), iID(aRelationUid), iProviderId(aProviderId)
    {
    TRACE_FUNC;
    
    }

CSconSyncRelationship* CSconSyncRelationship::NewL( RFs& aFs, TUid aRelationUid, TSmlDataProviderId aProviderId )
    {
    TRACE_FUNC_ENTRY;
    CSconSyncRelationship* self = new (ELeave) CSconSyncRelationship( aFs, aRelationUid, aProviderId );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

void CSconSyncRelationship::SetTimeStampL( RFs& aFs, TUid aRelationUid, TSmlDataProviderId aId )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aRelationUid: 0x%08x", aRelationUid.iUid );
    LOGGER_WRITE_1("aId: 0x%08x", aId );
    aFs.SetSessionToPrivate( EDriveC );
    TFileName file;
    file.Format(KTimeStore, aRelationUid.iUid);
    CDictionaryFileStore* dictionaryStore = CDictionaryFileStore::OpenLC(aFs, file, TUid::Uid(0x0001));
    
    RDictionaryWriteStream stream;
    stream.AssignLC( *dictionaryStore, TUid::Uid(aId) );
    
    TTime time;
    time.UniversalTime();
    
    TDateTime dateTime = time.DateTime();
    TPckgBuf<TDateTime> timeBuf(dateTime);
    
    stream.WriteL( timeBuf );
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream ); //AssingLC
    dictionaryStore->Commit();
    CleanupStack::PopAndDestroy( dictionaryStore );
    TRACE_FUNC_EXIT;
    }

void CSconSyncRelationship::GetTimeStampL( RFs& aFs, TUid aRelationUid, TSmlDataProviderId aId, TDateTime& aTimeStamp )
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aRelationUid: 0x%08x", aRelationUid.iUid );
    LOGGER_WRITE_1("aId: 0x%08x", aId );
    User::LeaveIfError( aFs.SetSessionToPrivate( EDriveC ) );
    TFileName file;
    file.Format(KTimeStore, aRelationUid.iUid);
    CDictionaryFileStore* dictionaryStore = CDictionaryFileStore::OpenLC(aFs, file, TUid::Uid(0x0001));
    TBool present = dictionaryStore->IsPresentL( TUid::Uid(aId) );
    if ( !present )
        {
        LOGGER_WRITE("Stream was not present");
        User::Leave(KErrNotFound);
        }
    RDictionaryReadStream stream;
    stream.OpenLC( *dictionaryStore, TUid::Uid(aId) );
    TPckgBuf<TDateTime> timeBuf;
    stream.ReadL( timeBuf );
    aTimeStamp = timeBuf();
    CleanupStack::PopAndDestroy(); //OpenLC
    CleanupStack::PopAndDestroy( dictionaryStore );
    
    TRACE_FUNC_EXIT;
    }

void CSconSyncRelationship::ConstructL()
    {
    TRACE_FUNC_ENTRY;
    iFs.SetSessionToPrivate( EDriveC );
    TFileName file;
    file.Format(KContextStore, iID.iUid);
    iDictionaryStore = CDictionaryFileStore::OpenL(
            iFs, file, TUid::Uid(0x0001));
    
    TRACE_FUNC_EXIT;
    }


TSmlSyncTaskKey CSconSyncRelationship::SyncTaskKey() const
    {
    TRACE_FUNC;
    return static_cast<TSmlSyncTaskKey>(iID.iUid);
    }

void CSconSyncRelationship::OpenReadStreamLC(RReadStream& aReadStream, TUid aStreamUid)
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aStreamUid: 0x%08x", aStreamUid);
    TUid streamUid(aStreamUid);
    if ( aStreamUid.iUid == KNSmlDefaultSnapshotStreamUID )
        {
        LOGGER_WRITE_1(" Default stream uid was used, use provider uid (0x%08x) as stream uid", iProviderId);
        streamUid.iUid = iProviderId;
        }
    if ( !IsStreamPresentL( streamUid ) )
        {
        LOGGER_WRITE("Stream was not present");
        User::Leave(KErrNotFound);
        }
    
        
    static_cast<RDictionaryReadStream&>(aReadStream).OpenLC(
        *iDictionaryStore, streamUid );
    TRACE_FUNC_EXIT;
    }

void CSconSyncRelationship::OpenWriteStreamLC(RWriteStream& aWriteStream, TUid aStreamUid)
    {
    TRACE_FUNC_ENTRY;
    
    LOGGER_WRITE_1("aStreamUid: 0x%08x", aStreamUid);
    TUid streamUid(aStreamUid);
    if ( aStreamUid.iUid == KNSmlDefaultSnapshotStreamUID )
        {
        LOGGER_WRITE_1(" Default stream uid was used, use provider uid (0x%08x) as stream uid", iProviderId);
        streamUid.iUid = iProviderId;
        }
    static_cast<RDictionaryWriteStream&>(aWriteStream).AssignLC(
                 *iDictionaryStore, streamUid );
    TRACE_FUNC_EXIT;
    }

TBool CSconSyncRelationship::IsStreamPresentL(TUid aStreamUid) const
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("aStreamUid: 0x%08x", aStreamUid.iUid );
    TUid streamUid(aStreamUid);
    if ( aStreamUid.iUid == KNSmlDefaultSnapshotStreamUID )
        {
        LOGGER_WRITE_1(" Default stream uid was used, use provider uid (0x%08x) as stream uid", iProviderId);
        streamUid.iUid = iProviderId;
        }
    TBool present = iDictionaryStore->IsPresentL( streamUid );
    LOGGER_WRITE_1("CSconSyncRelationship::IsStreamPresentL() return: %d",(TInt)present);
    return present;
    }
