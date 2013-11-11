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
* Description:    Media item list observer interface 
*
*/




#ifndef M_GLXMEDIALISTOBSERVER_H
#define M_GLXMEDIALISTOBSERVER_H

#include <mpxcommand.h>
#include <mpxmessage2.h>
#include <mpxattribute.h>
#include <glxlistdefs.h>

#include "glxthumbnailinfo.h"

// Forward declaration
class MGlxMediaList;

/*! 
  @class MGlxMediaListObserver
  
  @discussion Observes for changes in media list
              MPX-specific
  */
class MGlxMediaListObserver
    {
public:

    /**
     * Notification that media items were added to the list
     *
     * @param aStartIndex First item that was added (inserted)
     * @param aEndIndex Last item that was added (inserted)
     * @param aList List that this callback relates to 
     */
    virtual void HandleItemAddedL(TInt aStartIndex, TInt aEndIndex,
                                  MGlxMediaList* aList) = 0;

    /**
     * Notification that media object is now available for an item 
     *
     * @param Index of the item 
     * @param aList List that this callback relates to 
     */
    virtual void HandleMediaL(TInt aListIndex, MGlxMediaList* aList) = 0;

    /**
     * Notification that media items were removed from the list
     *
     * @param aStartIndex First item that was removed 
     * @param aEndIndex Last item that was removed
     * @param aList List that this callback relates to 
     */
    virtual void HandleItemRemovedL(TInt aStartIndex, TInt aEndIndex,
                                    MGlxMediaList* aList) = 0;

    /**
     * Notification that media items were changed
     *
     * @param aItemIndexes Indexes of items that were changed
     * @param aList List that this callback relates to 
     */
    virtual void HandleItemModifiedL(const RArray<TInt>& aItemIndexes, 
                                     MGlxMediaList* aList) = 0;

    /**
     * Notification that attributes are available
     *
     * @param aItemIndex Index of the item for which attributes are available
     * @param aAttributes Array of attributes that have become available
     * @param aList List that this callback relates to 
     */
    virtual void HandleAttributesAvailableL(TInt aItemIndex, 
                                            const RArray<TMPXAttribute>& aAttributes, 
                                            MGlxMediaList* aList) = 0;

    /**
     * Notification that focus has moved
     *
     * @param aType the direction of the focus change
     * @param aNewIndex the new index after the focus change
     * @param aOldIndex the old index before the focus change
     * @param aList List that this callback relates to
     */
    virtual void HandleFocusChangedL(NGlxListDefs::TFocusChangeType aType, 
                                     TInt aNewIndex, TInt aOldIndex, 
                                     MGlxMediaList* aList) = 0;

    /**
     * Notification that an item has been selected/deselected
     *
     * @param aIndex Index of the item that has been selected/deselected
     * @param aSelected Boolean to indicate selection/deselection
     * @param aList List that the selection relates to
     */
    virtual void HandleItemSelectedL(TInt aIndex, TBool aSelected, 
                                     MGlxMediaList* aList) = 0;

    /**
     * Notification from the collection
     * E.g. Items added/modified/deleted and progress notifications
     *
     * @param aMessage Message notification from the collection
     * @param aList List that the notification relates to
     */
    virtual void HandleMessageL(const CMPXMessage& aMessage, 
                                MGlxMediaList* aList) = 0;

    /**
     * Notification of an error from the collection
     *
     * @param aError Error code
     */
    virtual void HandleError(TInt aError) { (void)aError; };

    /**
     * Handle completion of a asynchronous command
     * @todo: All clients should implement this callback
     *
     * @param aSessionId session id of the client that issued the command
     * @param aCommandResult result of the command, NULL if error
     * @param aError error code
     * @param aList List that the notification relates to 
     */
    virtual void HandleCommandCompleteL(TAny* /*aSessionId*/, 
                                        CMPXCommand* /*aCommandResult*/, 
                                        TInt /*aError*/, 
                                        MGlxMediaList* /*aList*/) {};

    /**
     * Handle media list population.
     *
     * @param aList List that the notification relates to 
     */                                   
    virtual void HandlePopulatedL(MGlxMediaList* /*aList*/) {};
    };

#endif // M_GLXMEDIALISTOBSERVER_H

