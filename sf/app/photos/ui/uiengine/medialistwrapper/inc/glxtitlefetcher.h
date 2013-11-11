/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:    Title fetcher 
*
*/




#ifndef GLXTITLEFETCHER_H
#define GLXTITLEFETCHER_H

#include <e32base.h>
#include <coedef.h>
#include <w32std.h>
#include <mglxmedialistobserver.h>
#include "mglxtitlefetcherobserver.h"


class CMPXCollectionPath;
class MGlxMediaList;                   
class CGlxDefaultAttributeContext;


class CGlxTitleFetcher : public CBase, public MGlxMediaListObserver
    {
public:
    /**
     * Constructs an instance of CGlxTitleFetcher and initiates obtaining
     * a title 
     *
     * @param aObserver Observer of this list 
     * @param aPath The current path by which to obtain the title 
     */
    static CGlxTitleFetcher*  NewL(MGlxTitleFetcherObserver& aObserver,
                                    CMPXCollectionPath* aPath);

	IMPORT_C virtual ~CGlxTitleFetcher();
                        
    // From MGlxMediaListObserver                    
	virtual void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
		MGlxMediaList* aList);

    /**
     * Notification that media object is now available for an item 
     *
     * @param Index of the item 
     */
	virtual void HandleMediaL(TInt /*aListIndex*/, MGlxMediaList* /*aList*/) {};

    /**
     * Notification that media item was removed from the list
     *
     * @param aStartIndex First item that was removed 
     * @param aEndIndex Last item that was removed
     */
	virtual void HandleItemRemovedL(TInt /*aStartIndex*/, TInt /*aEndIndex*/,
		MGlxMediaList* /*aList*/) {};

    /**
     * Notification that media item was changed
     *
     * @param aItemIndexes Indexes of items that were changed
     */
	virtual void HandleItemModifiedL(const RArray<TInt>& /*aItemIndexes*/, 
		MGlxMediaList* /*aList*/) {};


    /**
     * Notification that an attribute is available
     *
     * @param aItemIndex Index of the for which the thumbnail is available
     * @param aAttributes Array of attributes that have become available
     * @param aList List that this callback relates to 
     */
	virtual void HandleAttributesAvailableL(TInt aItemIndex, 	
		const RArray<TMPXAttribute>& aAttributes, MGlxMediaList* aList);	
			
    /**
     * Notification that focus has moved
     *
     * @param aType the direction of the focus change
     * @param aNewIndex the new index after the focus change
     * @param aOldIndex the old index before the focus change
     * @param aList List that this callback relates to
     */
	virtual void HandleFocusChangedL(NGlxListDefs::TFocusChangeType /*aType*/, 
	    TInt /*aNewIndex*/, TInt /*aOldIndex*/, MGlxMediaList* /*aList*/) {};

	/**
	 * Notification that an item has been selected/deselected
	 *
	 * @param aIndex Index of the item that has been selected/deselected
	 * @param aSelected Boolean to indicate selection/deselection
	 * @param aList List that the selection relates to
	 */
	virtual void HandleItemSelectedL(TInt /*aIndex*/, TBool /*aSelected*/, 
	              MGlxMediaList* /*aList*/) {};

	/**
	 * Notification from the collection.  E.g. Items added/modified/deleted and progress notifications
	 *
	 * @param aMessage Message notification from the collection
	 * @param aList List that the notification relates to
	 */
	virtual void HandleMessageL(const CMPXMessage& /*aMessage*/, 
	                            MGlxMediaList* /*aList*/) {};
	
private:
                        
    CGlxTitleFetcher(MGlxTitleFetcherObserver& aObserver,
                     CMPXCollectionPath* aPath);
    
    void ConstructL();
    
private:

    MGlxTitleFetcherObserver& iObserver;
    CMPXCollectionPath* iPath;
    TMPXItemId iPathId;
    MGlxMediaList*  iBackMediaList; 
    CGlxDefaultAttributeContext* iContext;                  
    };


#endif // GLXTITLEFETCHER_H
