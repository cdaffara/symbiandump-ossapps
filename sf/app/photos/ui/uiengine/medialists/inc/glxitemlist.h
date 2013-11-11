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
* Description:    List of media items
*
*/




#ifndef __C_GLXITEMLIST_H__
#define __C_GLXITEMLIST_H__

#include <e32std.h>
#include "glxmedia.h"

// Forward declarations
class CMPXCollectionPath;
class MGlxItemListObserver;
class MGlxMediaUser;
class MGlxMediaPool;

namespace NGlxItemList
    {
    class MListModificationStrategy;
    class TDifferenceInfo;
    }

/**
 * CGlxItemList
 *
 * List that has media items
 *
 * @author Aki Vanhatalo
 *
 * @internal reviewed 17/07/2007 by Kimmo Hoikka
 *
 * @ingroup mlm_media_list_manager_design
 */
NONSHARABLE_CLASS( CGlxItemList ) : public CBase
    {
public:
     /**
      * Constructor
      * @param aIdSpaceId id of id space in which list item ids are
      * @param aObserver Observer to be notified of item addition/removal
      * @param aMediaUser User class of media objects - media objects that 
      *                   this list links to will be linked to the user, so that 
      *                   cache manager is able to quickly access the media lists 
      *                   that use particular media objects
      */
    static CGlxItemList* NewL( const TGlxIdSpaceId& aIdSpaceId, 
        MGlxItemListObserver& aObserver, MGlxMediaUser& aMediaUser );    
        
    /** 
     * Destructor
     */
    ~CGlxItemList();
    
    /**
     * @return id space id
     */
    inline const TGlxIdSpaceId& IdSpaceId() const;
    
    /**
     * Synchronises the list with new path 
     * Sends added/removed notifications as necessary
     * Note: Does not send "perfect" notification if the order of the items 
     * has been changed. Hence, this function cannot be reliably used when 
     * sort order has changed
     *
     * @param aSource path with item ids. 
     * @param aMediaPool interface from which to ask for media items
     */
    void SetContentsL( const CMPXCollectionPath& aSource, const MGlxMediaPool& aMediaPool );
    
    /**
     * Remove an item form the list
     * Sends removed notification
     *
     * @param aItemId Id of item to remove
     * @param aIdSpaceId Id space of the item
     */
    void Remove( const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aItemId );

    /**
     * Remove any pointers to the media object at the specified index
     * @param aIndex index of media object from which to remove a reference
     */
    void RemoveReference( TInt aIndex );

    /** 
     * @return count of items in the list
     */
    inline TInt Count() const;

    /** 
     * @return item at index
     */
    inline TGlxMedia& Item( TInt aIndex );

    /** 
     * @param aId Id of item for which index is needed
     * @return index of item with id aId or KErrNotFound
     */
    TInt Index( const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aId ) const;

private:
    /**
     * Constructor
     * @param aIdSpaceId id of id space in which list item ids are
     * @param aObserver Observer to be notified
     * @param aMediaUser User class of media objects - media objects that 
     *                   this list links to will be linked to the user, so that 
     *                   cache manager is able to quickly access the media lists 
     *                   that use particular media objects
     */
    CGlxItemList( const TGlxIdSpaceId& aIdSpaceId,
        MGlxItemListObserver& aObserver, MGlxMediaUser& aMediaUser );
           
    /**
     * Process differences between source and current list
     * @param aSource source list 
     * @param aStrategy strategy object that contains the logic of how to
     *                  process the differences that are found
     */
    void ProcessDifferences( const CMPXCollectionPath& aSource, 
            NGlxItemList::MListModificationStrategy& aStrategy );

    /**
     * Find the first matches in the source and current lists.
     * @param aInfo object that contains start-search-from indexes in source 
     *              and current list
     * @param aSource source list
     * @param aSourceMatchIndex On return, contains the index of matching 
     *                          source item, or if not found, the count of
     *                          source list. (Returning count simplifies 
     *                          client functions algorithm)
     * @param aTargetMatchIndex On return, contains the index of matching 
     *                          target item, or if not found, the count of
     *                          target (current) list. (Returning count 
     *                          simplifies client functions algorithm)
     */
    void FindMatchingItems( const NGlxItemList::TDifferenceInfo& aInfo, 
        const CMPXCollectionPath& aSource, TInt& aSourceMatchIndex, 
        TInt& aTargetMatchIndex );
        
    /**
     * Process difference that would require removing items to eliminate.
     * Forwards the actual "remove" request to the modification strategy
     * @param aTargetMatchIndex Index until which items need to be removed
     * @param aInfo Info of indexes needed to remove the items.
     *              Strategy can modify these indexes
     * @param aStrategy strategy object that contains the logic of how to
     *                  process the differences that are found
     */
    void ProcessRemove( TInt aTargetMatchIndex, NGlxItemList::TDifferenceInfo& aInfo, 
            NGlxItemList::MListModificationStrategy& aStrategy );
            
    /**
     * Process difference that would require inserting items to eliminate.
     * Forwards the actual "insert" request to the modification strategy
     * @param aSourceMatchIndex Index to which items need to be copies
     * @param aInfo Info of indexes needed to insert the items.
     *              Strategy can modify these indexes
     * @param aStrategy strategy object that contains the logic of how to
     *                  process the differences that are found
     */
    void ProcessInsert( TInt aSourceMatchIndex, NGlxItemList::TDifferenceInfo& aInfo, 
            NGlxItemList::MListModificationStrategy& aStrategy );

    /** 
     * @return item at index, non-const
     */
    inline TGlxMedia& operator[] ( TInt aIndex );
    
private:
    /// Media items in this list
    RArray< TGlxMedia > iItems;
    
    /// The id of the "space" of unique item ids 
    TGlxIdSpaceId iIdSpaceId;
    
    /// User of media objects. Used for building a link from media object 
    /// (CGlxMedia) to its used when building a link from item (TGlxMedia) 
    /// to media object
    MGlxMediaUser& iMediaUser;
    
    /// Observer for changes
    MGlxItemListObserver& iObserver; 

    __DECLARE_TEST;
    };

#include "glxitemlist.inl" 
    
#endif // __C_GLXITEMLIST_H__
