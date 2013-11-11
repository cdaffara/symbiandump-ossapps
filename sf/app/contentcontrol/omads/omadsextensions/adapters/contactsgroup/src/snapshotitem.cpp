/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <hash.h>
#include <utf.h>
#include <cntitem.h>

#include "snapshotitem.h"
#include "logger.h"


// -----------------------------------------------------------------------------
// TSnapshotItem::TSnapshotItem
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
TSnapshotItem::TSnapshotItem() : TNSmlSnapshotItem()
    {
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::TSnapshotItem
// Constructor, takes item id as a parameter
// -----------------------------------------------------------------------------
TSnapshotItem::TSnapshotItem( const TSmlDbItemUid& aItemId,
    const TSmlDbItemUid& aParent )
: TNSmlSnapshotItem( aItemId )
    {
    SetParentId( aParent );
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::ExternalizeL
// Writes the contents of this class and it's base to stream
// -----------------------------------------------------------------------------
void TSnapshotItem::ExternalizeL( RWriteStream& aStream ) const
    {
    TNSmlSnapshotItem::ExternalizeL( aStream );
    TPckgBuf<THashValue> nameBuf(iHash);
	aStream << nameBuf;
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::InternalizeL
// Reads the contents of this class and it's base from stream
// -----------------------------------------------------------------------------
void TSnapshotItem::InternalizeL( RReadStream& aStream )
    {
    TNSmlSnapshotItem::InternalizeL( aStream );
    TPckgBuf<THashValue> nameBuf;
	aStream >> nameBuf;
	iHash = nameBuf();
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::CreateHashL
// Create hash value from group content
// -----------------------------------------------------------------------------
void TSnapshotItem::CreateHashL( CContactGroup& aGroup )
    {
    CMessageDigest* hash = CMessageDigestFactory::NewDigestLC( CMessageDigest::EMD5 );
    
    if ( aGroup.HasItemLabelField() )
        {
        TPtrC label = aGroup.GetGroupLabelL();
        HBufC8* tempBuf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( label );
        CleanupStack::PushL( tempBuf );
        hash->Update( tempBuf->Des() );
        CleanupStack::PopAndDestroy( tempBuf );
        }
    
    // Create text field from items on group
    CContactIdArray* contactsIdArray = aGroup.ItemsContainedLC();
    const TInt idBufferMaxSize( 400 );
    HBufC8* tempIdBuf = HBufC8::NewLC( idBufferMaxSize );
    TPtr8 tempId = tempIdBuf->Des();
    const TInt KMaxNumLength(5);
    if ( contactsIdArray ) // this is NULL if there are no objects
        {
        tempId.AppendNum( contactsIdArray->Count(), EHex );
        for (TInt i=0; i< contactsIdArray->Count(); i++ )
            {
            if ( tempId.Length()+KMaxNumLength > tempId.MaxLength() )
                {
                // buffer is almost full, don't add any new items
                LOGGER_WRITE("buffer full");
                break;
                }
            // add item id
            tempId.AppendNum( (*contactsIdArray)[i] , EHex );
            }
        }
    else
        {
        tempId.AppendNum( 0 );
        }
    // update hash
    hash->Update( tempId );
    
    iHash.Copy( hash->Final() );
    
    CleanupStack::PopAndDestroy( tempIdBuf );
    CleanupStack::PopAndDestroy( contactsIdArray );
    CleanupStack::PopAndDestroy( hash );
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::Hash
// Gets Hash
// -----------------------------------------------------------------------------
const TDesC8& TSnapshotItem::Hash() const
    {
    return iHash;
    }
