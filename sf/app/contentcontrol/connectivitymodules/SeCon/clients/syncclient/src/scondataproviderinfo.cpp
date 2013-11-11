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
* Description:  CSconDataproviderInfo implementation
*
*/


#include "scondataproviderinfo.h"
#include <s32strm.h>
#include <badesca.h>
#include <utf.h>  // for CnvUtfConverter
#include "debug.h"

CSconDataproviderInfo::CSconDataproviderInfo()
    {
    }

EXPORT_C CSconDataproviderInfo::~CSconDataproviderInfo()
    {
    delete iName;
    delete iDefaultStore;
    delete iStores;
    }

EXPORT_C CSconDataproviderInfo* CSconDataproviderInfo::NewL()
    {
    return new (ELeave) CSconDataproviderInfo();
    }

EXPORT_C void CSconDataproviderInfo::ExternalizeL( RWriteStream& aStream ) const
    {
    TRACE_FUNC_ENTRY;
    LOGGER_WRITE_1("iUid: 0x%x", iUid);
    aStream.WriteInt32L( iUid );
    if ( iName )
        {
        HBufC8* temp = CnvUtfConverter::ConvertFromUnicodeToUtf8L( iName->Des() );
        CleanupStack::PushL( temp );
        TPtrC8 tempPtr = temp->Des();
        aStream.WriteUint16L( tempPtr.Length() );
        aStream.WriteL( tempPtr );
        CleanupStack::PopAndDestroy( temp );
        }
    else
        {
        aStream.WriteUint16L( 0 );
        }
    
    if ( iDefaultStore )
        {
        HBufC8* temp = CnvUtfConverter::ConvertFromUnicodeToUtf8L( iDefaultStore->Des() );
        CleanupStack::PushL( temp );
        TPtrC8 tempPtr = temp->Des();
        aStream.WriteUint16L( tempPtr.Length() );
        aStream.WriteL( tempPtr );
        CleanupStack::PopAndDestroy( temp );
        }
    else
        {
        aStream.WriteUint16L( 0 );
        }
    if ( iStores )
        {
        aStream.WriteUint16L( iStores->Count() );
        for ( TInt i=0; i<iStores->Count(); i++ )
            {
            TPtrC storePtr = (*iStores)[i];
            HBufC8* temp = CnvUtfConverter::ConvertFromUnicodeToUtf8L( storePtr );
            CleanupStack::PushL( temp );
            TPtrC8 tempPtr = temp->Des();
            aStream.WriteUint16L( tempPtr.Length() );
            if ( tempPtr.Length() > 0 )
                {
                aStream.WriteL( tempPtr );
                }
            CleanupStack::PopAndDestroy( temp );
            }
        }
    else
        {
        aStream.WriteUint16L( 0 );
        }
    aStream.CommitL();
    TRACE_FUNC_EXIT;
    }

EXPORT_C void CSconDataproviderInfo::InternalizeL( RReadStream& aStream )
    {
    TRACE_FUNC_ENTRY;
    delete iName;
    iName = NULL;
    delete iDefaultStore;
    iDefaultStore = NULL;
    delete iStores;
    iStores = NULL;
    iStores = new ( ELeave ) CDesCArrayFlat( 1 );
    iUid = aStream.ReadInt32L();
    TInt len = aStream.ReadUint16L();
    if ( len > 0 )
        {
        HBufC8* tempBuf = HBufC8::NewLC( len );
        TPtr8 tempPtr = tempBuf->Des();
        aStream.ReadL( tempPtr, len );
        
        iName = CnvUtfConverter::ConvertToUnicodeFromUtf8L( tempPtr );
        CleanupStack::PopAndDestroy( tempBuf );
        }
    
    len = aStream.ReadUint16L();
    if ( len > 0 )
        {
        HBufC8* tempBuf = HBufC8::NewLC( len );
        TPtr8 tempPtr = tempBuf->Des();
        aStream.ReadL( tempPtr, len );
        
        iDefaultStore = CnvUtfConverter::ConvertToUnicodeFromUtf8L( tempPtr );
        CleanupStack::PopAndDestroy( tempBuf );
        }
    
    TInt storeCount = aStream.ReadUint16L();
    for (TInt i=0; i<storeCount; i++ )
        {
        len = aStream.ReadUint16L();
        if ( len > 0 )
            {
            HBufC8* tempBuf8 = HBufC8::NewLC( len );
            TPtr8 tempPtr8 = tempBuf8->Des();
            aStream.ReadL( tempPtr8, len );
            
            HBufC* temp = CnvUtfConverter::ConvertToUnicodeFromUtf8L( tempPtr8 );
            CleanupStack::PushL( temp );
            TPtr tempPtr = temp->Des();
            iStores->AppendL( tempPtr );
            CleanupStack::PopAndDestroy( temp );
            CleanupStack::PopAndDestroy( tempBuf8 );
            }
        }
    TRACE_FUNC_EXIT;
    }

EXPORT_C TUid CSconDataproviderInfo::ImplementationUid() const
    {
    return TUid::Uid( iUid );
    }

EXPORT_C void CSconDataproviderInfo::SetImplementationUid( const TUid aUid )
    {
    iUid = aUid.iUid;
    }

EXPORT_C TDesC& CSconDataproviderInfo::DisplayNameL()
    {
    if ( iName )
        {
        return *iName;
        }
    else
        {
        iName = KNullDesC().AllocL();
        return *iName;
        }
    }

EXPORT_C void CSconDataproviderInfo::SetDisplayNameL( const TDesC& aName )
    {
    delete iName;
    iName = NULL;
    iName = aName.AllocL();
    }

EXPORT_C TDesC& CSconDataproviderInfo::DefaultStoreL()
    {
    if ( iDefaultStore )
        {
        return *iDefaultStore;
        }
    else
        {
        iDefaultStore = KNullDesC().AllocL();
        return *iDefaultStore;
        }
    }

EXPORT_C void CSconDataproviderInfo::SetDefaultStoreL( const TDesC& aStore )
    {
    delete iDefaultStore;
    iDefaultStore = NULL;
    iDefaultStore = aStore.AllocL();
    }

EXPORT_C CDesCArray* CSconDataproviderInfo::ListStoresL() const
    {
    if ( !iStores )
        {
        User::Leave( KErrNotFound );
        }
    return iStores;
    }

EXPORT_C void CSconDataproviderInfo::SetStoresL( const CDesCArray& aStores )
    {
    delete iStores;
    iStores = NULL;
    iStores = new ( ELeave ) CDesCArrayFlat( 1 );
    for ( TInt i=0; i<aStores.Count(); i++ )
        {
        iStores->AppendL( aStores[i] );
        }
    }
