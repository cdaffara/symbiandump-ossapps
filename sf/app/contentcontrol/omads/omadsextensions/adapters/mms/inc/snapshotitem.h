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
* Description:  Private snapshot item header
*
*/


#ifndef __SNAPSHOTITEM_H__
#define __SNAPSHOTITEM_H__

// INCLUDES
#include <msvids.h>
#include <nsmlsnapshotitem.h>
#include "mmsdataproviderdefs.h"

typedef TBuf<KMaxFolderNameLength> TFolderName;

// CLASS DECLARATION

/**
* @brief TSnapshotItem
* This class represents a single item in the ChangeFinder snapshot.
* It's derived from the generic snapshot item and extended
* to contain the read/unread information, which is mandatory when 
* partial update of the messages is required.
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
    * @param aUnread   IN: read/unread information, the default is read.
    */
	TSnapshotItem( const TSmlDbItemUid& aItemId,
	    const TSmlDbItemUid& aParent = 0, TBool aUnread = EFalse );
	
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
    * Sets the read flag
    * 
    * @param aState   IN: a new state.
    */
	void SetUnread( TBool aState ); 
	
	/**
    * Sets the read flag
    * 
    * @return TBool, ETrue if the item is not read and EFalse if it has been read.
    */
	TBool Unread() const;	
	
	void SetFolderNameL( const TDesC& aFolderName );
	
	const TDesC& FolderName() const;
	
private:

    /**
    * Read / Unread information
    */
	TBool       iUnread;
	
	TFolderName iFolderName;
    };

#endif
