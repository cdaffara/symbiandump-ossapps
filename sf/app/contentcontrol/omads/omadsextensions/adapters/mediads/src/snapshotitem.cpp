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



#include <S32STRM.h>

#include "snapshotitem.h"


// -----------------------------------------------------------------------------
// TSnapshotItem::TSnapshotItem
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
TSnapshotItem::TSnapshotItem()
    {
    SetItemId( 0 );
    SetParentId( 0 );
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::TSnapshotItem
// Constructor, takes item id as a parameter
// -----------------------------------------------------------------------------
TSnapshotItem::TSnapshotItem( const TSmlDbItemUid& aItemId,
    const TSmlDbItemUid& aParent )
    {
    SetItemId( aItemId );
    SetParentId( aParent );
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::ExternalizeL
// Writes the contents of this class to stream
// -----------------------------------------------------------------------------
void TSnapshotItem::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( iItemId );
    aStream.WriteInt32L( iParentId );
    
    TPckgBuf<THashValue> nameBuf(iHash);
    
    aStream << nameBuf;
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::InternalizeL
// Reads the contents of this class from stream
// -----------------------------------------------------------------------------
void TSnapshotItem::InternalizeL( RReadStream& aStream )
    {
    iItemId = aStream.ReadInt32L();
    iParentId = aStream.ReadInt32L();
    
    TPckgBuf<THashValue> nameBuf;
	aStream >> nameBuf;
	iHash = nameBuf();
    }

const TSmlDbItemUid& TSnapshotItem::ItemId() const
    {
    return iItemId;
    }

void TSnapshotItem::SetItemId( const TSmlDbItemUid& aItemId )
    {
    iItemId = aItemId;
    }


const TSmlDbItemUid& TSnapshotItem::ParentId() const
    {
    return iParentId;
    }

void TSnapshotItem::SetParentId( const TSmlDbItemUid& aParentId )
    {
    iParentId = aParentId;
    }


void TSnapshotItem::SetItemHash( const TDesC& aFolderName, CMD5& aHasher )
    {
    TPtrC8 narrowFolder;
    TInt narrowFolderLen = 0;

    narrowFolderLen = aFolderName.Length()*2;
    narrowFolder.Set((TUint8*)aFolderName.Ptr(), narrowFolderLen);
    
    aHasher.Reset();
    aHasher.Update( narrowFolder );
    
    iHash = aHasher.Final();
    
    }

void TSnapshotItem::SetItemHash( const CSongItem& aSongItem, CMD5& aHasher )
    {
    TPtrC8 narrowData;
    TInt narrowDataLen = 0;

    
    aHasher.Reset();
    
    narrowDataLen = aSongItem.Uri().Length()*2;
    narrowData.Set((TUint8*)aSongItem.Uri().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aSongItem.Title().Length()*2;
    narrowData.Set((TUint8*)aSongItem.Title().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aSongItem.MimeType().Length()*2;
    narrowData.Set((TUint8*)aSongItem.MimeType().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aSongItem.Artist().Length()*2;
    narrowData.Set((TUint8*)aSongItem.Artist().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aSongItem.Album().Length()*2;
    narrowData.Set((TUint8*)aSongItem.Album().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aSongItem.Genre().Length()*2;
    narrowData.Set((TUint8*)aSongItem.Genre().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aSongItem.Comment().Length()*2;
    narrowData.Set((TUint8*)aSongItem.Comment().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aSongItem.Composer().Length()*2;
    narrowData.Set((TUint8*)aSongItem.Composer().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aSongItem.MimeType().Length()*2;
    narrowData.Set((TUint8*)aSongItem.MimeType().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    const TInt KNumberMaxLength(11);
    TBuf8<KNumberMaxLength> temp;
    temp.Num( aSongItem.Year() );
    aHasher.Update( temp );
    
    temp.Num( aSongItem.AlbumTrack() );
    aHasher.Update( temp );
    
    temp.Num( aSongItem.Rating() );
    aHasher.Update( temp );   
    
    temp.Num( aSongItem.Duration() );
    aHasher.Update( temp );
    
    iHash.Copy( aHasher.Final() );
    
    }

void TSnapshotItem::SetItemHash( const CPlaylistItem& aPlaylist, CMD5& aHasher )
    {
    TPtrC8 narrowData;
    TInt narrowDataLen = 0;
    
    aHasher.Reset();
    narrowDataLen = aPlaylist.Title().Length()*2;
    narrowData.Set((TUint8*)aPlaylist.Title().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    narrowDataLen = aPlaylist.Url().Length()*2;
    narrowData.Set((TUint8*)aPlaylist.Url().Ptr(), narrowDataLen);
    aHasher.Update( narrowData );
    
    for (TInt i=0; i<aPlaylist.ItemCount(); i++ )
        {
        TPtrC16 song = aPlaylist.ItemAt(i);
        narrowDataLen = song.Length()*2;
        narrowData.Set((TUint8*)song.Ptr(), narrowDataLen);
        aHasher.Update( narrowData );
        }
    iHash.Copy( aHasher.Final() );
    }

const TDesC8& TSnapshotItem::Hash() const
    {
    return iHash;
    }
