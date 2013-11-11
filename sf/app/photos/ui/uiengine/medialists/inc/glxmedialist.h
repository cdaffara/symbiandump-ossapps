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




#ifndef C_GLXMEDIALIST_H
#define C_GLXMEDIALIST_H

#include <e32std.h>
#include <mpxattributespecs.h>
#include <mpxcollectionobserver.h>

#include "mglxmedialist.h"
#include "mglxnavigablelistobserver.h"
#include "mglxmediauser.h"

// Forward declarations
class CGlxCacheManager;
class CGlxErrorPoster;
class CGlxNavigableList;
class CMPXCollectionPath;
class MGlxFetchContext;
class MMPXCollection;
class MMPXCollectionUtility;
class TGlxMedia;
class CGlxMediaListArray;

namespace NGlxMediaList
    {
    class TNotificationStrategy;
    }

/**
 * CGlxMediaList
 *
 * Interface for reading lists of media items
 *
 * @ingroup mlm_media_list_manager_design
 * @lib glxmedialists.lib
 */
class CGlxMediaList :
    public CBase, public MGlxMediaList, public MMPXCollectionObserver, 
        public MGlxNavigableListObserver, public MGlxMediaUser    
    {
public:
    /**
     * Return a reference-counted instance of CGlxMediaList. User MUST Close() this when finished with it.
     *
     * @param aPath Path to the collection
     * @param aHierarchyId Id of the client hierarchy
     * @param aFilter Filter on the collection
     * @return instance of media list object
     */
    static CGlxMediaList* InstanceL(const CMPXCollectionPath& aPath, 
        const TGlxHierarchyId& aHierarchyId = KGlxIdNone, CMPXFilter* aFilter = NULL);

    
    /**
     * Gives an array of all media lists in current use
     *
     * @return The array of media lists
     */
    static RPointerArray<CGlxMediaList>& MediaListsL();

public:
    /**
     * Media object is available. The list picks up the pointer to the media object 
     * if it has a matching item, and adds itself as a user of the media object
     * The index into the list is also stored, to allow clients that
     * are processing users of a media, to index directly into the list
     * Note: The function assumes that a reservation for at least one list has been
     * made in each CGlxMedia::iUsedByLists array, and this object can use that 
     * reservation
     *
     * @param aItems media items to offer to list
     */
    void OfferMedia(const TGlxIdSpaceId& aIdSpaceId, CGlxMedia* aItems);

    /**
     * Ask if the list has any requests to place
     *
     * @param aItemIndexes Indexes of the items to request attributes for
     * @param aItemIds Ids of the items to request attributes for
     * @param aAttributes attributes to request for the items
     * @param aDetailedSpecs attribute specifications
     */
    void AttributeRequestL(RArray<TInt>& aItemIndexes,
            RArray<TGlxMediaId>& aItemIds,
            RArray<TMPXAttribute>& aAttributes,
            CMPXAttributeSpecs*& aDetailedSpecs) const;

    /**
     * Ordering function for attributes
     *
     * @see TLinearOrder
     *
     * @param aItem1 first item to order
     * @param aItem2 second item to order
     * @return TInt indicating ordering
     */
    static TInt AttributeOrder(const TMPXAttribute& aItem1, const TMPXAttribute& aItem2);

    /**
     * Ordering function for attributes
     *
     * @see TLinearOrder
     *
     * @param aItem1 first item to order
     * @param aItem2 second item to order
     * @return TInt indicating ordering
     */
    static TInt AttributeOrderReversed(const TMPXAttribute& aItem1, const TMPXAttribute& aItem2);

public: // From MGlxMediaList
    /// See @ref MGlxMediaList::Close
    void Close();

    /// See @ref MGlxMediaList::Id
    TGlxMediaListId Id() const;

    /// See @ref MGlxMediaList::Count
    TInt Count(NGlxListDefs::TCountType aType = NGlxListDefs::ECountAll) const;

    /// See @ref MGlxMediaList::FocusIndex
    TInt FocusIndex() const;

    /// See @ref MGlxMediaList::SetFocusL
    void SetFocusL(NGlxListDefs::TFocusSetType aType, TInt aValue);

    /// See @ref MGlxMediaList::Item
    const TGlxMedia& Item(TInt aIndex) const;

    /// See @ref MGlxMediaList::Index
    TInt Index(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aId) const;

    /// See @ref MGlxMediaList::AddMediaListObserverL
    void AddMediaListObserverL(MGlxMediaListObserver* aObserver);

    /// See @ref MGlxMediaList::RemoveMediaListObserver
    void RemoveMediaListObserver(MGlxMediaListObserver* aObserver);

    /// See @ref MGlxMediaList::AddContext
    void AddContextL(const MGlxFetchContext* aContext, TInt aPriority);

    /// See @ref MGlxMediaList::RemoveContext
    void RemoveContext(const MGlxFetchContext* aContext);

    /// See @ref MGlxMediaList::Collection
    MMPXCollection& Collection() const;

    /// See @ref MGlxMediaList::PathLC
    CMPXCollectionPath* PathLC(NGlxListDefs::TPathType aType = NGlxListDefs::EPathAllOrSelection) const;

    /// See @ref MGlxMediaList::IsSelected
    TBool IsSelected(TInt aIndex) const;

    /// See @ref MGlxMediaList::SetSelectedL
    void SetSelectedL(TInt aIndex, TBool aSelected);

    /// See @ref MGlxMediaList::SelectionCount
    TInt SelectionCount() const;

    /// See @ref MGlxMediaList::SelectedItemIndex
    TInt SelectedItemIndex(TInt aSelectionIndex) const;

    /// See @ref MGlxMediaList::CommandL
    void CommandL(CMPXCommand& aCommand);

    /// See @ref MGlxMediaList::CancelCommand
    void CancelCommand();
    
    /// See @ref MGlxMediaList::IsCommandActive
    TBool IsCommandActive();

    /// See @ref MGlxMediaList::SetFilterL
    void SetFilterL(CMPXFilter* aFilter);

    /// See @ref MGlxMediaList::Filter
    CMPXFilter* Filter() const;

    /// See @ref MGlxMediaList::IdSpaceId
    TGlxIdSpaceId IdSpaceId(TInt aIndex) const;

    /// See @ref MGlxMediaList::IsPopulated
    TBool IsPopulated() const;
    
    /// See @ref MGlxMediaList::AddStaticItemL
    void AddStaticItemL( CGlxMedia* aStaticItem,
        NGlxListDefs::TInsertionPosition aTargetPosition );

    /// See @ref MGlxMediaList::RemoveStaticItem
      void RemoveStaticItem(const TGlxMediaId& aItemId);

    /// See @ref MGlxMediaList::SetStaticItemsEnabled
    void SetStaticItemsEnabled( TBool aEnabled );

    /// See @ref MGlxMediaList::IsStaticItemsEnabled
    TBool IsStaticItemsEnabled() const;

    /// See @ref MGlxMediaList::SetFocusInitialPosition
    void SetFocusInitialPosition(NGlxListDefs::TFocusInitialPosition aFocusInitialPosition);

    /// See @ref MGlxMediaList::ResetFocus
    void ResetFocus();
    
    /// See @ref MGlxMediaList::SetVisibleWindowIndex  
    void SetVisibleWindowIndexL( TInt aIndex );

    /// See @ref MGlxMediaList::VisibleWindowIndex
    TInt VisibleWindowIndex() const;
    
	/// See @ref MGlxMediaList::CancelPreviousRequests
    void  CancelPreviousRequests();
    
public: // From MMPXCollectionObserver
    /// See @ref MMPXCollectionObserver::HandleCollectionMessageL
    void HandleCollectionMessageL(const CMPXMessage& aMessage);

    /// See @ref MMPXCollectionObserver::HandleOpenL
    void HandleOpenL(const CMPXMedia& aEntries, TInt aIndex, TBool aComplete, TInt aError);

    /// See @ref MMPXCollectionObserver::HandleOpenL
    void HandleOpenL(const CMPXCollectionPlaylist& aPlaylist, TInt aError);  

    /// See @ref MMPXCollectionObserver::HandleCollectionMediaL
    void HandleCollectionMediaL(const CMPXMedia& aMedia, TInt aError);
    
    /// See @ref MMPXCollectionObserver::HandleCommandComplete
    void HandleCommandComplete(CMPXCommand* aCommandResult, TInt aError);
    
    // from MGlxNavigableListObserver
    void HandleItemsAdded( TInt aAddedAtIndex, TInt aCount );
    void HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount );
    void HandleFocusChanged( NGlxListDefs::TFocusChangeType aType, 
        TInt aNewIndex, TInt aOldIndex );
    void HandleItemSelected( TInt aIndex, TBool aSelected );
    
    // From MGlxMediaUser
    void HandleAttributesAvailableL( TInt aIndex, const RArray< TMPXAttribute >& aAttributes );
    void GetRequiredAttributesL( TInt aIndex, RArray<TMPXAttribute>& aAttributes );
    void RemoveReference( TInt aIndex );
    void HandleError(TInt aError);
    
private:
    /**
     * Two phase constructor
     */
    static CGlxMediaList* NewLC(const TGlxHierarchyId& aHierarchyId);

    /**
     * Constructor
     */
    CGlxMediaList(const TGlxHierarchyId& aHierarchyId);

    /**
     * Destructor
     */
    virtual ~CGlxMediaList();

    /**
     * 2nd phase constructor
     */
    void ConstructL();

    /**
     * Initialize the media list
     * Copies the levels from the path, and retrieves the 
     * contents from MPX Collection Server
     *
     * @param aPath path that this list refers to
     */
    void OpenL(const CMPXCollectionPath& aPath);

    /**
     * Add a reference
     *
     * @return new reference count
     */
    TInt AddReference();

    /**
     * Remove a reference
     *
     * @return new reference count
     */
    TInt RemoveReference();

    /**
     * Current number of references
     *
     * @return new reference count
     */
    TInt ReferenceCount() const;

    /**
     * Determines if path is the same
     *
     * @param aPath collection path
     * @return ETrue if this media list refers to the path
     */
    TBool Equals(const CMPXCollectionPath& aPath) const;

    /**
     * Determines if this list has a filter applied
     *
     * @return Boolean indication if filter has been applied
     */
    TBool IsFiltered() const;

    /**
     * Synchronise the media list
     * Re-retrieves the contents from MPX Collection Server
     */
    void ReOpenL();

    /**
     * Populates the list with the items in the current path
     */
    void PopulateL();

    /**
     * Handles item modifications
     *
     * @param aId MPX item id
     * @param aAttributes Attributes that have been modified
     */
    void HandleItemModifiedL(TInt aId, const RArray<TMPXAttribute>& aAttributes);

    /**
     * Notify observers of media object being now available
     *
     * @param aListIndex index of item for which properties are available
     */
    void NotifyObserversOfMediaL(TInt aListIndex);

    /**
     * Notify observers, and reports errors to error manager
     */
    void NotifyObservers( NGlxMediaList::TNotificationStrategy& aStrategy );
    
    /**
     * Populates the path with hierarchy to parent
     *
     * @param aPath path to populate
     */
    void PathPopulateParentL(CMPXCollectionPath& aPath) const;

    /**
     * Populates the path with all items
     * and sets the focus
     *
     * @param aPath path to populate
     */
    void PathPopulateAllL(CMPXCollectionPath& aPath) const;

    /**
     * Populates the path with focused item
     * and sets the focus
     *
     * @param aPath path to populate
     */
    void PathPopulateFocusL(CMPXCollectionPath& aPath) const;

    /**
     * Populates the path with selected items
     * and selects all
     *
     * @param aPath path to populate
     */
    void PathPopulateSelectionL(CMPXCollectionPath& aPath) const;

    /*
     * Updates each media used by this media list with the current index
     */
    void UpdateMedia();

    /*
     * Updates each media used by this media list with the current index
     */
    void UpdateMediaInvalidateAttributesChangedByCounts(TGlxMedia& aItem);

    /**
     * Opens a collection at the appropriate level
     *
     * @param aPath used to determine which collection and level to open
     */
    void OpenCollectionL(const CMPXCollectionPath& aPath);
    
    /**
     * Handles the tasks after open completes
     */
    void HandleOpenL();

private:
    /** 
     * Enumeration to indicate pending sync status
     */ 
    enum TSyncStatus
        {
        KNonePending,
        KSyncPending,
        KResyncPending
        };

    /** 
     * TContext
     */ 
    struct TContext
        {
        /**
         * Find match by context pointer
         */
        static TBool Match(const TContext& a1, const TContext& a2);

        /**
         * Compare by comparing priority
         */
        static TInt Compare(const TContext& a1, const TContext& a2);

        const MGlxFetchContext* iContext; // Not owned
        TInt iPriority; // Priority, higher is more important
        };

private:
    /**
     * Number of users of this object
     */
    TInt iReferenceCount;

    /**
     * Manager of this list
     * Not owned
     */
    CGlxCacheManager* iManager;

    /**
     * Observers that will be notified if items get added or removed or if they change
     * Not owned
     */
    RPointerArray<MGlxMediaListObserver> iItemListObservers; 

    /**
     * Contexts that contain intelligence of attribute (e.g.,
     * thumbnail of URI) retrieving, incl. priorities.  Not owned
     */
    RArray<TContext> iContexts;

    /** List that contains items, focus, and selection */
    CGlxNavigableList* iItemList;

    /**
     * Collection utility that is bound to this list. 
     */
    MMPXCollectionUtility* iCollectionUtility; // own

    RArray<TGlxMediaId> iPath;

    /**
     * Indicates if a command has already been placed on the collection, which has not complete yet.
     * Currently, only one command can be made at a time
     * Stores the sessionId (client that issued the command)
     */
    TAny* iCommandPending;

    /**
     * Filter currently set on the collection.  Owned
     */
    CMPXFilter* iFilter;

    /// IdSpaceId for items in the list.
    TGlxIdSpaceId iIdSpaceId;

    TGlxHierarchyId iHierarchyId;

    /**
     * Is populated, set to ETrue
     * when the media list is first populated.
     */
    TBool iIsPopulated;

    /**
     * Singleton to post errors to
     */
    CGlxErrorPoster* iErrorPoster;

    /// Indicates that filter was set, so need to re-order contents
    TBool iReorderPending;

    /// Indicates that item(s) updated, so need to sync now or resync later
    TSyncStatus iSyncStatus;
    
    /**
     * Array of media lists
     * (singleton: should be closed, not deleted)
     */
    CGlxMediaListArray* iMediaListArray;
    
    /// Array of attributes to be invalidated when items are added or removed
    RArray<TMPXAttribute> iCountAttributes;
    
    /// VisibleWindow Index 
    TInt iVisibleWindowIndex;
    };

#endif // C_GLXMEDIALIST_H
