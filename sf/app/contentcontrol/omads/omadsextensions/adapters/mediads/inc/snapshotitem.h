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
* Description:  Private snapshot item header
*
*/


#ifndef __SNAPSHOTITEM_H__
#define __SNAPSHOTITEM_H__

// INCLUDES
#include <SmlDataSyncDefs.h>
#include <hash.h>
#include "mediadsproviderdefs.h"
#include "songitem.h"
#include "playlistitem.h"

typedef TBuf<KMaxFolderNameLength> TFolderName;

// CLASS DECLARATION

/**
 * @brief TSnapshotItem
 * This class represents a single item in the ChangeFinder snapshot.
 * 
 */

// Defines the maximum length of a hash.
const TInt KMaxHashLength = 16;
typedef TBuf8<KMaxHashLength> THashValue;

class TSnapshotItem
	{ 
public:
	
	/**
    * Constructor
    */
	TSnapshotItem();

	/**
    * Constructor
    * 
    * @param aItemId   IN: id of the item.
    * @param aParent   IN: parent item, 0 as default.
    * @param aUnread   IN: read/unread information, the default is read.
    */
	TSnapshotItem( const TSmlDbItemUid& aItemId,
	    const TSmlDbItemUid& aParent = 0 );
	
	/**
    * Writes the item to a stream.
    * 
    * @param aStream   IN: a reference to a stream, where this item is to be written.
    */
	void ExternalizeL( RWriteStream& aStream ) const;

	/**
    * Reads item from a stream.
    * 
    * @param aStream   IN: a reference to a stream, where this item is read from.
    */
	void InternalizeL( RReadStream& aStream );	
	
	/**
    * Getter for item UID value.
    * @return const TSmlDbItemUid& item UID.
    */
    const TSmlDbItemUid& ItemId() const;

    /**
    * Setter for item UID value.
    * @param aItemId item UID.
    */
    void SetItemId( const TSmlDbItemUid& aItemId );

    /**
    * Getter for parent UID value.
    * @return const TSmlDbItemUid& parent UID.
    */
    const TSmlDbItemUid& ParentId() const;

    /**
    * Setter for parent UID value.
    * @param aParentId parent UID.
    */
    void SetParentId( const TSmlDbItemUid& aParentId );
	
    /**
     * Generate hash value from foldername
     * @param aFolderName foldername
     * @param CMD5& aHasher MD5 hasher
     */
	void SetItemHash( const TDesC& aFolderName, CMD5& aHasher );
	
	/**
     * Generate hash value from aSongItem content
     * @param aSongItem song item
     * @param CMD5& aHasher MD5 hasher
     */
	void SetItemHash( const CSongItem& aSongItem, CMD5& aHasher );
	
	/**
     * Generate hash value from aPlaylist content
     * @param aPlaylist aPlaylist
     * @param CMD5& aHasher MD5 hasher
     */
	void SetItemHash( const CPlaylistItem& aPlaylist, CMD5& aHasher );
	
	/**
	 * Getter for generated hash value.
	 * @return const TDesC8& hash value
	 */
	const TDesC8& Hash() const;
	
private:
	TSmlDbItemUid iItemId;
	TSmlDbItemUid iParentId;
    THashValue    iHash;
    };

#endif
