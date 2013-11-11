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
* Description:    Reader interface of media item lists 
*
*/




#ifndef M_GLXMEDIALIST_H
#define M_GLXMEDIALIST_H

#include <mpxcollectionpath.h>
#include <mpxfilter.h>
#include <mpxcommand.h>

#include <glxhierarchyid.h>
#include <glxlistdefs.h>

#include "glxmedia.h" // For TGlxMediaId
#include "glxmedialistid.h" 

// Forward declarations
class MMPXCollection;
class MGlxMediaListObserver;
class MGlxFetchContext;

/**
 *  MGlxMediaList
 *
 *  Interface for reading lists of media items
 *
 *  @lib glxmedialists.lib
 */
class MGlxMediaList
    {
public:
    /**
     * Returns a new/existing media list interface.
     *
     * Returns an existing instance if and only if the hierarchy id and path
     * are the same.  Otherwise a new instance is returned
     *
     * Filter is only used when a new media list is created, otherwise it
     * is ignored.
     *
     * The new/existing media list is set as highest priority list
     *
     * @param aPath Path to the collection
     * @param aHierarchyId Id of the client hierarchy
     * @param aFilter Filter on the collection
     * @return Media list interface (does not pass ownership)
     */
    IMPORT_C static MGlxMediaList* InstanceL(const CMPXCollectionPath& aPath, 
        const TGlxHierarchyId& aHierarchyId = KGlxIdNone, CMPXFilter* aFilter = NULL);

    /**
     * Releases a media list interface
     */
    virtual void Close() = 0;

    /**
     * Id of this reader interface. The id of each _existing_ reader interface 
     * is guaranteed to be unique, but new readers can reuse the ids of deleted
     * ones.
     *
     * @return Unique id of this interface
     */
    virtual TGlxMediaListId Id() const = 0;

    /**
     * Count of media items in the list
     *
     * @return Count of media items
     */
    virtual TInt Count(NGlxListDefs::TCountType aType = NGlxListDefs::ECountAll) const = 0;

    /**
     * Current focus position 
     * 
     * @return Index of focused item
     */
    virtual TInt FocusIndex() const = 0;

    /**
     * Sets the focus. 
     * The implementation of this function is required to call 
     * MGlxMediaListObserver::HandleFocusChangedL for all added
     * observers. Calling this function will cause the item window
     * to be updated and potentially properties and thumbnails
     * to be loaded.
     *
     * @param aType whether the focus change is absolute or relative
     * @param aValue value of the focus change
     */
    virtual void SetFocusL(NGlxListDefs::TFocusSetType aType, TInt aValue) = 0;

    /**
     * Media item at index.
     * It is not legal to access items outside the preload 
     * (or visible) range.
     *
     * @param aIndex index of the item in the list
     * @return Media item pointer
     */
    virtual const TGlxMedia& Item(TInt aIndex) const = 0;

    /**
     * Item index by media item id
     * If the item is not within the preload (or visible) range 
     * returns KErrNotFound. The item may still exist in the full 
     * list. This behavior can be changed, if there is a use case.
     *
     * @param aId Unique id of the media item
     * @return Index of the item in the list or KErrNotFound
     */
    virtual TInt Index(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aId) const = 0;

    /**
     * Add an observer that observes changes to the list.
     * This observer will be informed whenever the items in the list 
     * change, more items get added, or items get deleted, or the 
     * focus changes.
     *
     * @param aObserver The observer
     */
    virtual void AddMediaListObserverL(MGlxMediaListObserver* aObserver) = 0;

    /**
     * Removes a media item list observer from the observer list
     *
     * @param aObserver The observer
     */
    virtual void RemoveMediaListObserver(MGlxMediaListObserver* aObserver) = 0;

    /**
     * Registers a context that affects the behavior of attribute and thumbnail loading.
     * @param aContext context to register
     * @param aPriority priority of the context. The higher the more important.
     */
    virtual void AddContextL(const MGlxFetchContext* aContext, TInt aPriority) = 0;

    /**
     * Removes an existing context
     *
     * @param aContext Pointer of the context to be removed
     */
    virtual void RemoveContext(const MGlxFetchContext* aContext) = 0;

    /**
     * Sets the specified contexts as highest priority contexts. Other 
     * contexts will retain their relative priority order, but will have 
     * less priority than the specified contexts.
     *
     * @param aContextIds Ids of the contexts to be set as highest priority.
     *		  Id at index 0 in the array is set to be the highest priority,
     *		  Id at index 1, the second highest, etc.
     */
//	virtual void SetContextAsHighestPriority(RArray<TGlxMediaListContextId> aContextIds) = 0;
	
    /**
     * @returns the MPX collection utility that is bound to this list
     */
    virtual MMPXCollection& Collection() const = 0;

    /**
     * Current path, including selection.  Ownership passed to caller.
     *
     * @return CMPXCollectionPath* representing current path
     */
    virtual CMPXCollectionPath* PathLC(NGlxListDefs::TPathType aType = NGlxListDefs::EPathAllOrSelection) const = 0;

    /**
     * Determines if an item has been selected
     *
     * @param aIndex Index of the item to be checked for selection
     * @return Boolean indication of the item selection
     */
    virtual TBool IsSelected(TInt aIndex) const = 0;

    /**
     * Item selection/deselection
     *
     * @param aIndex Index of the item to be selected/deselected
     * @param aSelected Boolean to indicate selection/deselection
     */
    virtual void SetSelectedL(TInt aIndex, TBool aSelected) = 0;

    /**
     * Selection count
     *
     * @return TInt count of selected items
     */
    virtual TInt SelectionCount() const = 0;

    /**
     * Selected item index from selection
     *
     * @param aSelectionIndex Index in selection array
     * @return TInt item index from selection
     */
    virtual TInt SelectedItemIndex(TInt aSelectionIndex) const = 0;

    /**
     * Sends a command to the collection
     *
     * @param aCommand MPX command from a command handler
     */
    virtual void CommandL(CMPXCommand& aCommand) = 0;

    /**
     * Cancels a command on the collection
     */
    virtual void CancelCommand() = 0;
	
	/**
     * Checks if  a command is active or not
     */
	virtual TBool IsCommandActive()= 0;

    /**
     * Sets a filter on the collection.  The filter will replace any existing
     * filter applied on the collection.  If a client needs an aggregate filter,
     * they will need to obtain the currently applied filter using Filter() and
     * merge the filters
     *
     * @param aFilter MPX filter on the collection.  Ownership passed to media list
     */
    virtual void SetFilterL(CMPXFilter* aFilter) = 0;

    /**
     * Filter on the collection
     *
     * @return CMPXFilter on the collection
     */
    virtual CMPXFilter* Filter() const = 0;

    /**
     * The Id Space Id of the media list.
     *
     * @param aIndex index of the item in the list
     * @return The Id Space of the media list
     */
    virtual TGlxIdSpaceId IdSpaceId(TInt aIndex) const = 0;
    
    /**
     * Returns ETrue if the media list has been
     * populated else EFalse
     */
    virtual TBool IsPopulated() const = 0;
    		
    /**
     * Adds a static item and takes ownership
     * Note: In case of a leave, aStaticItem may have been modified.
     *       It is assumed that since ownership of the static item is passed,
     *       the object is free to be modified.
     * @param aStaticItem Static item to be added
     * @param aTargetPosition Whether to be added at beginning or end
     */
    virtual void AddStaticItemL( CGlxMedia* aStaticItem,
        NGlxListDefs::TInsertionPosition aTargetPosition ) = 0;

	/**
	  * Remove static item from the list.
	  * @param aItemId Id of item to remove
	  */
	virtual void RemoveStaticItem(const TGlxMediaId& aItemId) = 0;
	
    /**
     * Enables or disables static items
     * @param aEnabled true if static items should be enabled
     *                 false if static items should be disabled
     */
    virtual void SetStaticItemsEnabled( TBool aEnabled ) = 0;

    /**
     * Tests if static items are enabled.
     * @return ETrue if static items are enabled
     */
    virtual TBool IsStaticItemsEnabled() const = 0;
    
    /**
     * Sets the initial focus position, first or last item
     * @param aFocusInitialPosition initial focus on first or last item
     */
    virtual void SetFocusInitialPosition(NGlxListDefs::TFocusInitialPosition aFocusInitialPosition) = 0;

    /**
     * Resets the focus to the initial position
     */
    virtual void ResetFocus() = 0;

    /**
     * Unmarks all items in the medialist.
     * @param aList Reference to Medialist items.
     */
    IMPORT_C static void UnmarkAllL( MGlxMediaList& aList );
    
    /**
     * Sets the dataWindow position. 
     * The implementation of this function is required for the
     * item window to be updated while blocky iterator is used 
     * and potentially properties and thumbnails to be loaded.
     *
     * @param aIndex value of the dataWindow change
     */
    virtual void SetVisibleWindowIndexL( TInt aIndex) = 0;
    
    /**
     * Current visible dataWindow position 
     */
    virtual TInt VisibleWindowIndex() const = 0;
    
    virtual ~MGlxMediaList() {};

    /**
     * Cancel the pending requests 
     */
    virtual void CancelPreviousRequests() = 0;
    };

#endif // M_GLXMEDIALIST_H

