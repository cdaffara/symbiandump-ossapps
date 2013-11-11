/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
// Create hash value from name and url
// -----------------------------------------------------------------------------

void TSnapshotItem::CreateHashL( const TDesC& aName , const TDesC& aUrl )
    {
    CMessageDigest* hash = CMessageDigestFactory::NewDigestLC( CMessageDigest::EMD5 );
    HBufC8* tempBuf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aName );
    CleanupStack::PushL( tempBuf );
    hash->Update( tempBuf->Des() );
    CleanupStack::PopAndDestroy( tempBuf );
    tempBuf = NULL;
    tempBuf = CnvUtfConverter::ConvertFromUnicodeToUtf8L( aUrl );
    hash->Update( tempBuf->Des() );
    iHash.Copy( hash->Final() );
    delete tempBuf;
    tempBuf = NULL;
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
