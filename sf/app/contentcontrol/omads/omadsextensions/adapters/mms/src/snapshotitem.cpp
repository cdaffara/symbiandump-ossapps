/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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


#include <msvuids.h>
#include "snapshotitem.h"


// -----------------------------------------------------------------------------
// TSnapshotItem::TSnapshotItem
// C++ default constructor can NOT contain any code, that might leave
// -----------------------------------------------------------------------------
TSnapshotItem::TSnapshotItem() : TNSmlSnapshotItem()
    {
    iUnread = EFalse;
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::TSnapshotItem
// Constructor, takes item id as a parameter
// -----------------------------------------------------------------------------
TSnapshotItem::TSnapshotItem( const TSmlDbItemUid& aItemId,
    const TSmlDbItemUid& aParent, TBool aUnread )
: TNSmlSnapshotItem( aItemId )
    {
    SetParentId( aParent );
    iUnread = aUnread;
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::ExternalizeL
// Writes the contents of this class and it's base to stream
// -----------------------------------------------------------------------------
void TSnapshotItem::ExternalizeL( RWriteStream& aStream ) const
    {
    TNSmlSnapshotItem::ExternalizeL( aStream );
    aStream.WriteInt32L( iUnread );
    TPckgBuf<TFolderName> nameBuf(iFolderName);
	aStream << nameBuf;
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::InternalizeL
// Reads the contents of this class and it's base from stream
// -----------------------------------------------------------------------------
void TSnapshotItem::InternalizeL( RReadStream& aStream )
    {
    TNSmlSnapshotItem::InternalizeL( aStream );
    iUnread = aStream.ReadInt32L();
    TPckgBuf<TFolderName> nameBuf;
	aStream >> nameBuf;
	iFolderName = nameBuf();
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::SetUnread
// Sets unread flag
// -----------------------------------------------------------------------------
void TSnapshotItem::SetUnread( TBool aState )
    {
    iUnread = aState;
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::Unread
// Gets unread flag
// -----------------------------------------------------------------------------
TBool TSnapshotItem::Unread() const
    {
    return iUnread;
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::SetFolderNameL
// Sets folder name
// ----------------------------------------------------------------------------- 
void TSnapshotItem::SetFolderNameL( const TDesC& aFolderName )
    {
    if ( aFolderName.Length() > KMaxFolderNameLength )
        {
        User::Leave( KErrTooBig );
        }
    iFolderName.Copy( aFolderName );
    }

// -----------------------------------------------------------------------------
// TSnapshotItem::FolderName
// Gets folder name
// ----------------------------------------------------------------------------- 
const TDesC& TSnapshotItem::FolderName() const
    {
    return iFolderName;
    }
