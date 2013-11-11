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
* Description:    List of media items, which has focus
*
*/




#ifndef __C_GLXNAVIGABLELIST_H__
#define __C_GLXNAVIGABLELIST_H__

#include <e32base.h>
#include <glxlistdefs.h>


#include "glxmediaid.h"
#include "mglxitemlistobserver.h"

/** @todo
Other required changes:
-Media list must not media for static items to be requested: Remove static item indexes from the list attribute context returned
-Thumbnail context needs to give a low score (very low) to static items, so that it does not stop requesting thumbnails
*/ 

// Forward declarations
class TGlxMedia;
class CGlxMedia;
class CGlxStaticItemList;
class CMPXCollectionPath;
class MGlxMediaPool;
class MGlxMediaUser;
class MGlxNavigableListObserver;

/** 
 * Namespace for CGlxNavigableList class' helpers
 */
namespace NGlxNavigableList
    {
    class MSelectionIndexStrategy;
    }

/**
 * CGlxNavigableList
 *
 * List that has 
 *  - non-static items
 *  - static items
 *  - focus
 *  - selection
 *
 * @author Aki Vanhatalo
 *
 * @internal reviewed 16/07/2007 by Kimmo Hoikka
 *
 * @ingroup mlm_media_list_manager_design
 */
NONSHARABLE_CLASS( CGlxNavigableList ) : public CBase, public MGlxItemListObserver
    {
public:
    /**
     * Two-phase constructor
     * @param aIdSpaceId id of the "space" in which all item ids are unique
     * @param aObserver Observer to be notified
     * @param aMediaUser User of media objects - media objects that 
     *                   this list links to will be linked to the user, so that 
     *                   cache manager is able to quickly access the media lists 
     *                   that use particular media objects
     */
    static CGlxNavigableList* NewL( const TGlxIdSpaceId& aIdSpaceId,
        MGlxNavigableListObserver& aObserver, MGlxMediaUser& aMediaUser );
    
    /** 
     * Destructor
     */
    ~CGlxNavigableList();
    
    /**
     * @return id space id
     */
    const TGlxIdSpaceId& IdSpaceId() const;
    
    /**
     * Synchronises the list with new path 
     * Sends added/removed notifications as necessary
     * Note: Does not copy the selected items or focus
     *
     * @param aSource path with item ids
     * @param aMediaPool interface from which to ask for media items
     */
    void SetContentsL( const CMPXCollectionPath& aSource, 
        const MGlxMediaPool& aMediaPool );

    /**
     * Re-orders the list with new path
     * Sends all items removed followed by all items added (from new path) notifications
     * Maintains the selected items and focus
     *
     * @param aSource path with item ids
     * @param aMediaPool interface from which to ask for media items
     */
    void ReorderContentsL( const CMPXCollectionPath& aSource, 
        const MGlxMediaPool& aMediaPool );
    
    /**
     * Remove an item form the list
     * Sends removed notification
     *
     * @param aIdSpaceId Id space of the item. Use KGlxStaticItemIdSpaceId
     *                   if you want to permanently remove a static item.
     * @param aItemId Id of item to remove
     */
    void Remove( const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aItemId );
        
    /**
     * Remove any pointers to the media object at the specified index
     * @param aIndex index of media object from which to remove a reference
     */
    void RemoveReference( TInt aIndex );

    /**
     * Adds a static item and takes ownership
     * Note: In case of a leave, aStaticItem may have been modified.
     *       It is assumed that since ownership of the static item is passed,
     *       the object is free to be modified.
     * @param aStaticItem Static item to be added
     * @param aTargetPosition Whether to be added at beginning or end
     */
    void AddStaticItemL( CGlxMedia* aStaticItem,
        NGlxListDefs::TInsertionPosition aTargetPosition );

    /**
     * Enables or disables static items
     * @param aEnabled true if static items should be enabled
     *                 false if static items should be disabled
     */
    void SetStaticItemsEnabled( TBool aEnabled );

    /**
     * @return ETrue if static items are enabled
     */
    TBool IsStaticItemsEnabled() const;

    /** 
     * @param aType type of count wanted
     * @return count of items, as specified in aType
     */
    TInt Count( NGlxListDefs::TCountType aType = NGlxListDefs::ECountAll ) const;

    /** 
     * @return item at index
     */
    TGlxMedia& Item( TInt aIndex );

    /** 
     * @param aId Id of item for which index is needed
     * @return item with id aId or KErrNotFound
     */
    TInt Index(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aId ) const;

    /** 
     * Sets the initial focus position, first or last item
     * @param aFocusInitialPosition initial focus on first or last item
     */
    void SetFocusInitialPosition(NGlxListDefs::TFocusInitialPosition aFocusInitialPosition);

    /** 
     * @return index of focused item
     */
    TInt FocusIndex() const;

    /** 
     * Sets focus to item at index 
     * Notifies observer via HandleFocusChangedL
     * @param aType EAbsolute or ERelative
     * @param aValue If aType is EAbsolute, the new index value
     *               If aType is ERelative, the amount of indexes to move focus
     */
    void SetFocus( NGlxListDefs::TFocusSetType aType, TInt aValue );

    /** 
     * Resets the focus to the initial position
     */
    void ResetFocus();

    /** 
     * @return ETrue if item is selected, EFalse if not
     */
    TBool IsSelected( TInt aIndex ) const;

    /** 
     * Selects or deselects an item.
     * If item is already at desired selection state, does nothing.
     * Otherwise, calls back via HandleItemSelected.
     *
     * @param aIndex index of item to be selected/deselected
     * @param aSelected true if item should be selected
     *                  false if item should be deselected
     */
    void SetSelectedL( TInt aIndex, TBool aSelected );

    /** 
     * @return Array of indexes of items that are selected
     */
    const TArray< TInt > SelectedItemIndices() const;

    // From MGlxItemListObserver
    void HandleItemsAdded( TInt aFirstInsertedIndex, TInt aCount );
    void HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount );

private:
    /**
     * Constructor
     * @param aObserver Observer to be notified
     */
    CGlxNavigableList( MGlxNavigableListObserver& aObserver );

    /** 
     * Second-phase constructor 
     * @param aIdSpaceId id of the "space" in which all item ids are unique
     * @param aMediaUser User of media objects 
     */
    void ConstructL( const TGlxIdSpaceId& aIdSpaceId, 
        MGlxMediaUser& aMediaUser );
    
    /** @return id of focused item or KGlxIdNone if nothing focused */
    TGlxMediaId FocusId() const;

    /** 
     * Populates provided array with currently selected item ids
     * @param aItemIds array to contain selected item ids
     */
    void SelectionL( RArray< TGlxMediaId >& aItemIds ) const;
    
    /** 
     * Sets list contents to nothing, but leaves static items
     * @param aMediaPool interface from which to ask for media items
     */
    void ClearContentsL( const MGlxMediaPool& aMediaPool );
    
    /** 
     * Selects items by id
     * Ok to call if item does not exist in list
     * @param aItemIds List of ids of items to select
     */
    void SelectL( const RArray< TGlxMediaId >& aItemIds );
   
    /** 
     * Select item at index
     * @param aIndex index to be selected
     */
    void SelectL( TInt aIndex );
    
    /** 
     * Select item at index
     * Must call ReserveFreeSpaceInSelectionL first
     * @param aIndex index to be selected
     */
    void Select( TInt aIndex );
    
    /** 
     * Reserve free space in selection, so that Select
     * can be safely called
     * @param aCount Number of times Select will be called (or larger)
     */
    void ReserveFreeSpaceInSelectionL( TInt aCount );

    /** 
     * Deselect item at index
     * @param aIndex index to be deselected
     */
    void Deselect( TInt aIndex );

    /** 
     * Sets the initial focus
     */
    void SetInitialFocus();

    /** 
     * Set focus by id. Notifies observer
     * Does nothing if id does not exist in list
     * @param aItemId Id of the item to be focused
     */
    void SetFocus( const TGlxMediaId& aItemId );

    /** 
     * Set focus index
     * @param aIndex New focus index
     * @return Focus change type
     */
    NGlxListDefs::TFocusChangeType SetFocus( TInt aIndex );
        
    /** 
     * Move focus index
     * @param aDelta amount of indexes to move focus by
     * @return Focus change type
     */
    NGlxListDefs::TFocusChangeType MoveFocus( TInt aDelta );

    /**
     * Optionally, notify observer of focus change
     * @param aType Focus change type
     * @param aOldFocusIndex Focus index before focus being moved
     * @param aNotify if ETrue, notifies observer
     *                if EFalse, does nothing
     */
    void NotifyFocusChange( NGlxListDefs::TFocusChangeType aType, 
        TInt aNewIndex, TBool aNotify );
        
    /** 
     * Update focus after items have been inserted to the list
     * @param aInsertionIndex Insertion point
     * @param aInsertionCount Number of items inserted
     * @return ETrue if observer should be notified of focus change
     *         EFalse if not
     */
    TInt UpdateFocusAfterInsertion( TInt aInsertionIndex, TInt aInsertionCount );

    /** 
     * Update selection item indexes after removal of items
     * @param aRemovedFromIndex First removed item index
     * @param aRemovedCount Number of items removed
     */
    void UpdateSelectionAfterRemove( TInt aRemovedFromIndex, TInt aRemovedCount );
    
    /** 
     * Update focus after items have been removed from the list
     * @param aRemovedFromIndex index of first removed item
     * @param aRemovedCount Number of items removed
     * @return ETrue if observer should be notified of focus change
     *         EFalse if not
     */
    TBool UpdateFocusAfterRemoval( TInt aRemovedFromIndex, TInt aRemovedCount ); 

    /** 
     * Move focus if index changed.
     * @param aChangedIndex Only move focus if index changed.
     * @param aMoveBy Amount to move by
     * @return ETrue if observer should be notified of focus change
     *         EFalse if not
     */
    TBool MoveFocusIfIndexChanged( TInt aChangedIndex, TInt aMoveBy );
    
    
    /** 
     * Run selection index strategy for all selection item indexes that point 
     * to items between aFromIndex and aToIndex (inclusive)
     * @param aFromIndex First item index to be processed (not selection item index, 
     *                   but item index)
     * @param aToIndex Last item index to be processed (not selection item index, 
     *                 but item index)
     * @param aStrategy Strategy object that contains the logic of what to do 
     *                  with the selection index
     */
    void ProcessSelectionItemIndicesBetweenIndexes( TInt aFromIndex, 
        TInt aToIndex, NGlxNavigableList::MSelectionIndexStrategy& aStrategy );
        
    /**
     * Find the first selected item index that points to an item index
     * smaller aMaxIndex
     * @aMaxIndex The item index pointed to by the returned selection index has 
     *            to be smaller than this parameter
     * @return selected item index (in iSelectedItemIndices)
     */
    TInt FindFirstSelectedItemIndexBefore( TInt aMaxIndex );
        
private:
    /// list of items
    CGlxStaticItemList* iItemList; // own
    
    
    /// Focus initial position, first or last item
    NGlxListDefs::TFocusInitialPosition iFocusInitialPosition;

    /// Focus index or KErrNotFound
    TInt iFocusIndex;
    
    /// Indexes of selected items
    RArray<TInt> iSelectedItemIndices;
    
    /// Observer for changes
    MGlxNavigableListObserver& iObserver;
    
    /// Free space in iSelectedItemIndices. Used to make sure Select(...) is not
    /// called without calling ReserveFreeSpaceInSelectionL first
    __DEBUG_ONLY( TInt _iSelectionReserveCount );
    
    __DECLARE_TEST;
    };
    
#endif // __C_GLXNAVIGABLELIST_H__

