/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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

#include <s32strm.h>
#include <nsmlsnapshotitem.h>


// CLASS DECLARATION
class CContactGroup;

// Defines the maximum length of a hash.
const TInt KMaxHashLength = 20;
typedef TBuf8<KMaxHashLength> THashValue;

/**
* @brief TSnapshotItem
* This class represents a single item in the ChangeFinder snapshot.
* It's derived from the generic snapshot item and extended
* to contain the hash (name and url) information.
*/
class TSnapshotItem : public TNSmlSnapshotItem
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
    * Creates hash from name and url
    *
    * @param aName, name to used
    * @param aUrl, url to used
    * @return none.
    */
	void CreateHashL( const TDesC& aName, const TDesC& aUrl );
	void CreateHashL( CContactGroup& aGroup );
	
	/**
    * Reads hash
    * 
    * @return TDesC8& hash.
    */
	const TDesC8& Hash() const;
	
private:
    
    THashValue       iHash;
    };

#endif
