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




#ifndef __C_GLXSTATICITEMLIST_H__
#define __C_GLXSTATICITEMLIST_H__

#include <e32base.h>

#include "mglxitemlistobserver.h"
#include "glxlistdefs.h"
#include "glxmediaid.h"

class CGlxItemList;
class CGlxMedia;
class CMPXCollectionPath;
class MGlxMediaPool;
class MGlxMediaUser;
class TGlxMedia;

/**
 * CGlxItemList
 *
 * List that has 
 *  - non-static items
 *  - static items
 *
 * @author Aki Vanhatalo
 *
 * @internal reviewed 17/07/2007 by Kimmo Hoikka
 *
 * @ingroup mlm_media_list_manager_design
 */
NONSHARABLE_CLASS( CGlxStaticItemList ) : public CBase, public MGlxItemListObserver
    {
public: // Interface for external use
    /**
     * Two-phase constructor
     * @param aIdSpaceId id of the "space" in which all item ids are unique
     * @param aObserver Observer to be notified
     * @param aMediaUser User of media objects - media objects that 
     *                   this list links to will be linked to the user, so that 
     *                   cache manager is able to quickly access the media lists 
     *                   that use particular media objects
     */
    static CGlxStaticItemList* NewL( const TGlxIdSpaceId& aIdSpaceId,
        MGlxItemListObserver& aObserver, MGlxMediaUser& aMediaUser );
    
    /** 
     * Destructor
     */
    ~CGlxStaticItemList();
    
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
    void SetContentsL( const CMPXCollectionPath& aSource, const MGlxMediaPool& aMediaPool );
    
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
     * @param aType type of count (all, static items, etc)
     *              Default value of "all" to make it easier ro create test template
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
     * Adds a static item and takes ownership
     * If an item with the same id already exists (in the same position), 
     * deletes the old item.
     * Note: In case of a leave, aStaticItem may have been modified.
     *       It is assumed that since ownership of the static item is passed,
     *       the object is free to be modifies.
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

public: // From MGlxItemListObserver
    void HandleItemsAdded( TInt aFirstInsertedIndex, TInt aCount );
    void HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount );
    
private:
    /** Array that owns static media items */
    class RStaticMediaArray : public RArray< TGlxMedia >
        {
    public:
        /** Destructor */
        ~RStaticMediaArray();
        /** 
         * Deletes an item at aIndex 
         * Does not remove the position in the array at index
         * @param aIndex index at which to delete the item
         */
        void Delete( TInt aIndex );
        };

private:
    /**
     * Constructor
     * @param aObserver Observer to be notified
     */
    CGlxStaticItemList( MGlxItemListObserver& aObserver );
    
    /**
     * Second-phase constructor
     * @param aMediaUser User of media objects 
     * @param aIdSpaceId id of the "space" in which all item ids are unique
     */
    void ConstructL( const TGlxIdSpaceId& aIdSpaceId, 
        MGlxMediaUser& aMediaUser );

    /** @return Number of items in list */
    TInt CountAll() const;

    /** @return Number of static items in list */
    TInt CountStatic() const;
    
    /** @return Number of non static items */
    TInt CountNonStatic() const;
    
    /** @return Index of the first post static item */
    TInt FirstPostStaticIndex() const;

    /**
     * Make static item ready to be added to the list
     * @param aStaticItem Static item to be modified
     */
    void SetupStaticItemL( CGlxMedia& aStaticItem );

    /**
     * Delete a static item
     * @param aId Id of the static item to be deleted
     */
    void DeleteStaticItem( const TGlxMediaId& aId ); 
        
    /**
     * Try to replace the static item. Replaces the static item
     * only if it is found on the same position as to which the
     * replacement is intended for
     * @param aStaticItem Static item to replace with
     * @param aTargetPosition Position into which to add the item
     * @return ETrue if item was replaced, EFalse if not
     */
    TBool TryReplaceStaticItem( CGlxMedia& aStaticItem, 
        NGlxListDefs::TInsertionPosition aTargetPosition );
        
    /**
     * Try to find the static item in the provided array, and if found, replace 
     * the item with the supplied one
     * @param aStaticItem Static item to replace with
     * @param aInArray Array in which to try to find the item
     * @return ETrue if item was replaced, EFalse if not
     */
    TBool TryReplaceStaticItem( CGlxMedia& aStaticItem, 
        RStaticMediaArray& aInArray );
        
    /**
     * Deletes a static item and notifies observer
     * @param aId Id of the static item to delete
     * @param aFromArray Array from which to try to find the static item
     * @param aArrayOffset Index in the full list of the first 
     *                     item in aFromArray
     * @param aCountCurrentStatic Reference to the member variable
     *                            that stores the current count of the 
     *                            aFromArray
     */
    void DeleteStaticItem( const TGlxMediaId& aId, 
        RStaticMediaArray& aFromArray, TInt aArrayOffset, 
        TInt& aCountCurrentStatic ); 

    /**
     * Add a static item. Assumes reservation has been made on both front and 
     * rear static item array, and hence will never leave
     * @param aStaticItem Static item to be added
     * @param aTargetPosition Whether to be added at beginning or end
     */
    void AddStaticItem( CGlxMedia& aStaticItem, 
        NGlxListDefs::TInsertionPosition aTargetPosition );
        
    /**
     * Add static item to the beginning of the list. Assumes reservation has 
     * been made on the front static item array, and hence will never leave
     * @param aStaticItem Static item to add. 
     *                    Takes ownership
     */
    void AddFrontStaticItem( CGlxMedia& aStaticItem );
    
    /**
     * Add static item to the end of the list. Assumes reservation has been 
     * made on the rear static item array, and hence will never leave.
     * @param aStaticItem Static item to add. 
     *                    Takes ownership
     */
    void AddRearStaticItem( CGlxMedia& aStaticItem );

    /** Enable front static items and notify observer of additions */
    void EnableFrontStaticItems();
    /** Enable rear static items and notify observer of additions */
    void EnableRearStaticItems();

    /** Disable front static items and notify observer of removals */
    void DisableFrontStaticItems();
    /** Disable rear static items and notify observer of removals */
    void DisableRearStaticItems();
    
    /**
     * @param aId Id of an item to find
     * @param aInArray Array in which to find the item
     * @return index of item with aId in aInArray or KErrNotFound
     */
    TInt Index( const TGlxMediaId& aId, 
        const RStaticMediaArray& aInArray ) const;
        
private:
    /// Data model is organised as follows
    /// (Static items are items that client, not server, supplied)
    ///
    /// Example: In a list of 2 front static items, 5 non-static items, 3 rear 
    /// static items
    ///
    /// Legend:
    /// "S" static item
    /// "i" non-static item
    /// "|" Link (Pointer)
    /// "O" (Static) media object (CGlxMedia)
    /// "^" Position of index in relation to list
    /// 
    /// Case: Static items enabled:                  #              #
    /// iItems: id + pointer to media object         #  SSiiiiiSSS  #
    ///                                              #  ||     |||  # (linked via pointer)
    /// iFrontStaticItems: static media objects      #  OO     |||  #
    /// iRearStaticItems: static media objects       #         OOO  #
    ///                                              #              #
    /// iFirstNonStaticIndex, static items enabled   #    ^         # (2)
    /// iFirstRearStaticIndex, static items enabled  #         ^    # (7)
    ///                                              
    /// Case: Static items disabled:                 #         #
    /// iItems: id + pointer to media object         #  iiiii  #
    ///                                              #         #
    /// iFrontStaticItems: static media objects      #  OO     # (not linked to iItems)
    /// iRearStaticItems: static media objects       #  OOO    # (not linked to iItems)
    ///                                              #         #
    /// iFirstNonStaticIndex                         #  ^      #     (0)
    /// iFirstRearStaticIndex                        #       ^ #     (5)
    
    /// List of media items
    CGlxItemList* iItemList; // own
    
    /// Array of command items before the server-originated media items 
    RStaticMediaArray iFrontStaticItems; // own

    /// Array of command items after the server-originated media items 
    RStaticMediaArray iRearStaticItems; // own

    /// Current count of front static items. Zero if static items 
    /// are disabled (or no front static items).
    TInt iCountCurrentFrontStatic;
    
    /// Current count of front static items. Zero if static items 
    /// are disabled (or no front static items).
    TInt iCountCurrentRearStatic;
    
    /// Info of whether static items are visible or hidden (outside this object)
    TBool iStaticItemsEnabled;
    
    /// Observer for changes
    MGlxItemListObserver& iObserver; 

    __DECLARE_TEST;
    };
    
#endif // __C_GLXSTATICITEMLIST_H__
