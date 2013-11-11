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




#include "glxmedialist.h"

#include <glxcollectionplugintype.hrh>

#include <glxassert.h>
#include <glxcollectiongeneraldefs.h>
#include <glxerrorposter.h>
#include <mpxcollectionmessage.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionutility.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <glxtracer.h>
#include <glxlog.h>
#include "glxcachemanager.h"
#include "glxmedialistarray.h"
#include "glxnavigablelist.h"
#include "mglxfetchcontext.h"
#include "mglxmedialistobserver.h"
#include "glxmediastaticitemdefs.h"

/**
 * Min & Max wait interval for a modify event, in microseconds
 * This is to allow thumbnail manager to procees the event first.
 */
const TInt KModifyEventMinWaitInterval = 2000000;
const TInt KModifyEventMaxWaitInterval = 3000000;
/**
 * Maximum items count for minimum wait interval.
 */
const TInt KMaxItemsCount = 500;
namespace NGlxMediaList
    {
    /**
     * Interface to notify observers. Allows different notifications to use
     * the same (complicated) iteration loop
     */
    class TNotificationStrategy 
        {
        public:
            TNotificationStrategy( MGlxMediaList& aList )
                    : iList( aList )
                {
                }
                
            /** 
             * Notify observer
             * @param aObserver Observer to notify 
             */
            virtual void NotifyL( MGlxMediaListObserver& aObserver ) = 0;
        
        protected:
            MGlxMediaList& iList;
        };

    /**
     * Notification strategy for items being added
     */
    class TItemsAddedNotificationStrategy : public TNotificationStrategy 
        {
        public:
            /**
             * Constructor 
             * @param aFirstAddedIndex Index of the first item added
             * @param aCount number of items added
             * @param aList Media list that sends the notification
             */
            TItemsAddedNotificationStrategy( TInt aFirstAddedIndex, 
                TInt aCount, MGlxMediaList& aList )
                    : TNotificationStrategy( aList )
                {
                iFirstAddedIndex = aFirstAddedIndex;
                iLastAddedIndex = aFirstAddedIndex + aCount - 1;
                }
                
            // From MGlxNotificationStrategy
            void NotifyL( MGlxMediaListObserver& aObserver )
                {
                aObserver.HandleItemAddedL( iFirstAddedIndex, iLastAddedIndex, 
                    &iList );
                }        
                
        private:
            /// Index of the first item added
            TInt iFirstAddedIndex;
            /// Index of the last item added
            TInt iLastAddedIndex;
        };

    /**
     * Notification strategy for items being removed
     */
    class TItemsRemovedNotificationStrategy : public TNotificationStrategy 
        {
        public:
            /**
             * Constructor 
             * @param aFirstRemovedIndex Index of the first item removed
             * @param aCount number of items removed
             * @param aList Media list that sends the notification
             */
            TItemsRemovedNotificationStrategy( TInt aFirstRemovedIndex, 
                TInt aCount, MGlxMediaList& aList )
                    : TNotificationStrategy( aList )
                {
                iFirstRemovedIndex = aFirstRemovedIndex;
                iLastRemovedIndex = aFirstRemovedIndex + aCount - 1;
                }
                
            // From MGlxNotificationStrategy
            void NotifyL( MGlxMediaListObserver& aObserver )
                {
                aObserver.HandleItemRemovedL( iFirstRemovedIndex, 
                    iLastRemovedIndex, &iList );
                }        
                
        private:
            /// Index of the first item removed
            TInt iFirstRemovedIndex;
            /// Index of the last item removed
            TInt iLastRemovedIndex;
        };

    /**
     * Notification strategy for focus being changed
     */
    class TFocusChangedNotificationStrategy : public TNotificationStrategy 
        {
        public:
            /**
             * Constructor 
             * @param aFirstRemovedIndex Index of the first item removed
             * @param aCount number of items removed
             * @param aList Media list that sends the notification
             */
            TFocusChangedNotificationStrategy( NGlxListDefs::TFocusChangeType aType, 
                TInt aNewIndex, TInt aOldIndex, MGlxMediaList& aList )
                    : TNotificationStrategy( aList )
                {
                iType = aType;
                iNewIndex = aNewIndex;
                iOldIndex = aOldIndex;
                }
                
            // From MGlxNotificationStrategy
            void NotifyL( MGlxMediaListObserver& aObserver )
                {
                aObserver.HandleFocusChangedL( iType, iNewIndex, iOldIndex, &iList );
                }        
                
        private:
            /// Focus change type
            NGlxListDefs::TFocusChangeType iType;
            /// New focus index
            TInt iNewIndex;
            /// Old focus index
            TInt iOldIndex;
        };

    /**
     * Notification strategy for selection being changed
     */
    class TItemSelectedNotificationStrategy : public TNotificationStrategy 
        {
        public:
            /**
             * Constructor 
             * @param aIndex Index of item being selected/deselected
             * @param aSelected Selection/deselection info
             * @param aList Media list that sends the notification
             */
            TItemSelectedNotificationStrategy( TInt aIndex, TBool aSelected,
                MGlxMediaList& aList )
                    : TNotificationStrategy( aList )
                {
                iIndex = aIndex;
                iSelected = aSelected;
                }
                
            // From MGlxNotificationStrategy
            void NotifyL( MGlxMediaListObserver& aObserver )
                {
                aObserver.HandleItemSelectedL( iIndex, iSelected, &iList );
                }        
                
        private:
            /// Index of item being selected/deselected
            TInt iIndex;
            /// Selection/deselection info
            TBool iSelected;
        };    

    /**
     * Notification strategy for command being completed
     */
    class TCommandCompletedNotificationStrategy : public TNotificationStrategy 
        {
        public:
            /**
             * Constructor 
             * @param aSessionId Session id of client that issued the command
             * @param aCommand Command which was completed
             * @param aError Error code from execution of the command
             * @param aList Media list that sends the notification
             */
            TCommandCompletedNotificationStrategy( TAny* aSessionId, 
                CMPXCommand* aCommand, TInt aError, MGlxMediaList& aList )
                    : TNotificationStrategy( aList )
                {
                iSessionId = aSessionId;
                iCommand = aCommand;
                iError = aError;
                }
                
            // From MGlxNotificationStrategy
            void NotifyL( MGlxMediaListObserver& aObserver )
                {
                /// @todo: make this call leaving
                aObserver.HandleCommandCompleteL( iSessionId, iCommand, iError, &iList );
                }        

        private:
            /// Session id of client that issued the command
            TAny* iSessionId;
            /// Command which was completed
            CMPXCommand* iCommand;
            /// Error code 
            TBool iError;
        };            
        
        

    /**
     * Notification strategy for list population
     */
    class TListPopulatedNotificationStrategy : public TNotificationStrategy 
        {
        public:
            /**
             * Constructor 
             * @param aList Media list that sends the notification
             */
            TListPopulatedNotificationStrategy ( MGlxMediaList& aList )
                    : TNotificationStrategy( aList )
                {
                }
                
            // From MGlxNotificationStrategy
            void NotifyL( MGlxMediaListObserver& aObserver )
                {
                aObserver.HandlePopulatedL( &iList );
                }        
        }; 

    /**
     * Notification strategy for messages
     */
    class TMessageNotificationStrategy : public TNotificationStrategy
        {
        public:
            /**
             * Constructor
             * @param aList Media list that sends the notification
             */
            TMessageNotificationStrategy ( const CMPXMessage& aMessage, MGlxMediaList& aList ) :
                    TNotificationStrategy( aList ),
                    iMessage( aMessage )
                {
                }

            // From MGlxNotificationStrategy
            void NotifyL( MGlxMediaListObserver& aObserver )
                {
                aObserver.HandleMessageL( iMessage, &iList );
                }

        private:
            /// Message which was received
            const CMPXMessage& iMessage;
        };
    } // namespace NGlxMediaList
    
using namespace NGlxMediaList;

// -----------------------------------------------------------------------------
// Returns a new/existing media list interface
// -----------------------------------------------------------------------------
CGlxMediaList* CGlxMediaList::InstanceL(const CMPXCollectionPath& aPath, 
        const TGlxHierarchyId& aHierarchyId, CMPXFilter* aFilter)
    {
    TRACER("CGlxMediaList::InstanceL" );
    
    CGlxMediaListArray* mediaListArray = CGlxMediaListArray::InstanceL();
    CleanupClosePushL(*mediaListArray);
    
    RPointerArray<CGlxMediaList>& mediaLists =  mediaListArray->Array();

    TInt matchIndex = KErrNotFound;
    CGlxMediaList* mediaListInstance = NULL; // List to return

    // Try to find an existing media list to return (by matching the path)
    TInt listCount = mediaLists.Count();
    for (TInt count = 0; count < listCount; ++count)
        {
        CGlxMediaList* mediaList = mediaLists[count];

        // See if path's and hierarchy id's match
        // Filter is ignored
        if (mediaList->Equals(aPath) && (mediaList->iHierarchyId == aHierarchyId))
            {
            // Found a match
            matchIndex = count;
            mediaListInstance = mediaList;
            break;
            }
        }

    // Create a new media list if could not use an existing one
    if (matchIndex == KErrNotFound)
        {
        __ASSERT_DEBUG(!mediaListInstance, Panic(EGlxPanicAlreadyInitialised));

        mediaListInstance = CGlxMediaList::NewLC(aHierarchyId);

        // Set specified filter (if any) before opening the collection to path
        mediaListInstance->SetFilterL(aFilter);
        mediaListInstance->OpenL(aPath);

        // Insert media list as highest priority
        mediaLists.InsertL(mediaListInstance, 0);
        CleanupStack::Pop(mediaListInstance);
        }
    else
        {
        __ASSERT_DEBUG(mediaListInstance, Panic(EGlxPanicNullPointer));

        // Match found, set as highest priority
        mediaLists.InsertL(mediaListInstance, 0);
        mediaLists.Remove(matchIndex + 1);
        }

    mediaListInstance->AddReference(); // Add user

    CleanupStack::PopAndDestroy(mediaListArray);
    
    return mediaListInstance;
    }

// -----------------------------------------------------------------------------
// Gives an array of all media lists in current use
// -----------------------------------------------------------------------------
RPointerArray<CGlxMediaList>& CGlxMediaList::MediaListsL()
    {
    TRACER("CGlxMediaList::MediaListsL" );
    
    CGlxMediaListArray* mediaListArray = CGlxMediaListArray::InstanceL();
    RPointerArray<CGlxMediaList>& array = mediaListArray->Array();
    mediaListArray->Close();
    return array;
    }

// -----------------------------------------------------------------------------
// OfferMedia
// -----------------------------------------------------------------------------
//
void CGlxMediaList::OfferMedia(const TGlxIdSpaceId& aIdSpaceId, CGlxMedia* aMedia) 
    {
    TRACER("CGlxMediaList::OfferMedia" );

    // try to find the matching item in this list
    TInt index = Index( aIdSpaceId, aMedia->Id() );
    // if matching item found, link media object
    if ( KErrNotFound != index )
        {
        TGlxMedia& item = iItemList->Item( index );

        // static items should not be offered
        if ( !item.IsStatic() )
            {
            item.SetMedia( aMedia, *this, index );
            }
        }
    }

// -----------------------------------------------------------------------------
// NotifyThumbnailAvailableL
// -----------------------------------------------------------------------------
//
void CGlxMediaList::HandleAttributesAvailableL(TInt aIndex, 
        const RArray<TMPXAttribute>& aAttributes)
    {
    TRACER("CGlxMediaList::HandleAttributesAvailableL" );

    __ASSERT_DEBUG(aIndex >= 0, Panic(EGlxPanicLogicError)); // Must exist
    // Need to get exact count of iItemListObservers at
    // every iteration because it is possible that 
    // RemoveMediaListObserver might call during this iteration
    for (TInt i = 0; i < iItemListObservers.Count(); i++)
        {
        iItemListObservers[i]->HandleAttributesAvailableL(aIndex, aAttributes, this);
        }
    }

// -----------------------------------------------------------------------------
// Ask if the list has any requests to place
// -----------------------------------------------------------------------------
//
void CGlxMediaList::AttributeRequestL(RArray<TInt>& aItemIndexes,
		RArray<TGlxMediaId>& aItemIds, 
        RArray<TMPXAttribute>& aAttributes, 
        CMPXAttributeSpecs*& aDetailedSpecs) const
    {
    TRACER("CGlxMediaList::AttributeRequestL" );
    
    // Find the highest priority context that has something to request
    TInt count = iContexts.Count();
    for (TInt i = 0; i < count; i++)
        {
        // Determine items (referenced by index) to request attributes for
        iContexts[i].iContext->AttributeRequestL(this, aItemIndexes, aAttributes, aDetailedSpecs);

        // Map item indices to item Ids
        TInt itemIndicesCount = aItemIndexes.Count();
        for (TInt itemIndicesCounter = 0; itemIndicesCounter < itemIndicesCount; ++itemIndicesCounter)
            {
            const TGlxMedia& item = iItemList->Item( aItemIndexes[itemIndicesCounter] );
            if (!item.IsStatic())
                {
                aItemIds.AppendL( item.Id() );
                }
            }

        // break if context has something to request
        if (itemIndicesCount > 0)
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// AllAttributesL
// ---------------------------------------------------------------------------
//
void CGlxMediaList::GetRequiredAttributesL(TInt aIndex, RArray<TMPXAttribute>& aAttributes)
    {
    TRACER("CGlxMediaList::GetRequiredAttributesL" );
    
    TLinearOrder<TMPXAttribute> orderer (&AttributeOrder);

    // Determine all attributes to request for an item, for all contexts
    TInt contextCount = iContexts.Count();
    for ( TInt contextIndex = 0; contextIndex < contextCount; contextIndex++ )
        {
        const MGlxFetchContext* context = iContexts[contextIndex].iContext;
        if ( context )
            {
            // Determine all attributes to request for an item, for this context
            context->AllAttributesL(this, aIndex, aAttributes);

            }
        }
    }

// ---------------------------------------------------------------------------
// AttributeOrder
// ---------------------------------------------------------------------------
//
TInt CGlxMediaList::AttributeOrder(const TMPXAttribute& aItem1, 
        const TMPXAttribute& aItem2) 
    {
    TRACER("CGlxMediaList::AttributeOrder");
            
    TInt contentId1 = aItem1.ContentId();
    TInt contentId2 = aItem2.ContentId();

    if (contentId1 < contentId2) 
        {
        return -1;
        }

    if (contentId1 > contentId2) 
        {
        return 1;
        }

    TUint attributeId1 = aItem1.AttributeId();
    TUint attributeId2 = aItem2.AttributeId();

    if (attributeId1 < attributeId2) 
        {
        return -1;
        }

    if (attributeId1 > attributeId2) 
        {
        return 1;
        }

    return 0;
    }

// ---------------------------------------------------------------------------
// AttributeOrderReversed
// ---------------------------------------------------------------------------
//
TInt CGlxMediaList::AttributeOrderReversed(const TMPXAttribute& aItem1, 
        const TMPXAttribute& aItem2) 
    {
    TRACER("CGlxMediaList::AttributeOrderReversed");
    
    return -AttributeOrder(aItem1, aItem2);
    }

// ---------------------------------------------------------------------------
// Remore references to the media object
// ---------------------------------------------------------------------------
//
void CGlxMediaList::RemoveReference( TInt aIndex )
    {
    TRACER( "CGlxMediaList::RemoveReference" );
    GLX_ASSERT_DEBUG( iItemList, Panic( EGlxPanicNotInitialised ), 
        "not ready to remove references" );
    
    GLX_LOG_INFO1( "CGlxMediaList::RemoveReference %d", aIndex );
    iItemList->RemoveReference( aIndex );
    }

// ---------------------------------------------------------------------------
// HandleError
// ---------------------------------------------------------------------------
//
void CGlxMediaList::HandleError(TInt aError)
    {
    TRACER( "CGlxMediaList::HandleError");    
    GLX_LOG_INFO1( "CGlxMediaList::HandleError %d", aError );
    TInt obsCount = iItemListObservers.Count();
    for (TInt obsIdx = 0; obsIdx < obsCount; ++obsIdx)
        {
        iItemListObservers[obsIdx]->HandleError(aError);
        }
    }

// -----------------------------------------------------------------------------
// Releases a media list interface
// -----------------------------------------------------------------------------
void CGlxMediaList::Close()
    {
    TRACER( "CGlxMediaList::Close" );
    
    RPointerArray<CGlxMediaList>& mediaLists = iMediaListArray->Array();

    TInt listCount = mediaLists.Count();
    for (TInt count = 0; count < listCount; ++count)
        {
        CGlxMediaList* mediaList = mediaLists[count];

        if (mediaList == this)
            {
            TInt referenceCount = mediaList->RemoveReference();
            if (referenceCount == 0)
                {
                mediaLists.Remove(count);
                mediaLists.Compress();

                delete mediaList;
                break;
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// Id
// From MGlxMediaList
// -----------------------------------------------------------------------------
//
TGlxMediaListId CGlxMediaList::Id() const
    {
    TRACER( "CGlxMediaList::Id" );
    
    return TGlxMediaListId((unsigned int)(void*)this);	
    }

// -----------------------------------------------------------------------------
// Count
// From MGlxMediaList
// -----------------------------------------------------------------------------
//
TInt CGlxMediaList::Count(NGlxListDefs::TCountType aType) const
    {
    TRACER("CGlxMediaList::Count");
    
    return iItemList->Count( aType );    
    }

// -----------------------------------------------------------------------------
// FocusIndex
// From MGlxMediaList
// -----------------------------------------------------------------------------
//
TInt CGlxMediaList::FocusIndex() const
    {
    TRACER("CGlxMediaList::FocusIndex");
    
    return iItemList->FocusIndex();    
    }

// -----------------------------------------------------------------------------
// SetFocusL
// -----------------------------------------------------------------------------
//
void CGlxMediaList::SetFocusL(NGlxListDefs::TFocusSetType aType, TInt aValue)
    {
    TRACER("CGlxMediaList::SetFocusL");
    
    GLX_LOG_INFO1( "CGlxMediaList::SetFocusL %d", aValue );
    iItemList->SetFocus( aType, aValue );
    }

// -----------------------------------------------------------------------------
// Item
// From MGlxMediaList
// -----------------------------------------------------------------------------
//
const TGlxMedia& CGlxMediaList::Item(TInt aIndex) const
    {
    TRACER("CGlxMediaList::Item");
    
    return iItemList->Item( aIndex );    
    }

// -----------------------------------------------------------------------------
// Index
// From MGlxMediaList
// -----------------------------------------------------------------------------
//
TInt CGlxMediaList::Index(const TGlxIdSpaceId& aIdSpaceId, const TGlxMediaId& aId) const
    {
    TRACER("CGlxMediaList::Index");
    
    return iItemList->Index( aIdSpaceId, aId );    
    }

// -----------------------------------------------------------------------------
// AddMediaListObserverL
// From MGlxMediaList
// -----------------------------------------------------------------------------
//
void CGlxMediaList::AddMediaListObserverL(MGlxMediaListObserver* aObserver)
    {
    TRACER("CGlxMediaList::AddMediaListObserverL");    
    GLX_LOG_INFO1( "CGlxMediaList::AddMediaListObserverL(0x%x)", aObserver );

    // Make sure the observer is not already in the array
    __ASSERT_DEBUG(iItemListObservers.Find(aObserver) == -1, Panic(EGlxPanicIllegalArgument)); // Observer already added

    iItemListObservers.AppendL(aObserver);
    }

// -----------------------------------------------------------------------------
// RemoveMediaListObserver
// From MGlxMediaList
// -----------------------------------------------------------------------------
//	
void CGlxMediaList::RemoveMediaListObserver(MGlxMediaListObserver* aObserver)
    {
    TRACER("CGlxMediaList::RemoveMediaListObserver");    
    GLX_LOG_INFO1( "CGlxMediaList::RemoveMediaListObserver(0x%x)", aObserver );

    TInt index = iItemListObservers.Find(aObserver);

    // Make sure the observer is in the array
    // LOG THIS! __ASSERT_DEBUG(index != -1, Panic(EGlxPanicIllegalArgument)); // No such observer

    if (index != KErrNotFound) 
        {
        iItemListObservers.Remove(index);	
        }
    }

// -----------------------------------------------------------------------------
// AddContextL
// From MGlxMediaList
// -----------------------------------------------------------------------------
//	
void CGlxMediaList::AddContextL(const MGlxFetchContext* aContext, 
        TInt aPriority)
    {
    TRACER("CGlxMediaList::AddContextL");    
    GLX_LOG_INFO1( "CGlxMediaList::AddContextL(0x%x)", aContext );

    if (!aContext)
        {
        return;
        }

    // Add the new context in priority order
    TContext context;
    context.iContext = aContext;
    context.iPriority = aPriority;

#ifdef _DEBUG

    // Make sure no duplicate entries
    TIdentityRelation<CGlxMediaList::TContext> match (&CGlxMediaList::TContext::Match);
    __ASSERT_DEBUG(iContexts.Find(context, match) == KErrNotFound, Panic(EGlxPanicAlreadyAdded));

#endif // _DEBUG

    TLinearOrder<CGlxMediaList::TContext> orderer (&CGlxMediaList::TContext::Compare);
    TInt ret = iContexts.InsertInOrderAllowRepeats(context, orderer); 
    User::LeaveIfError(ret);

    iManager->HandleWindowChangedL(this);
    }

// -----------------------------------------------------------------------------
// RemoveContext
// From MGlxMediaList
// -----------------------------------------------------------------------------
//	
void CGlxMediaList::RemoveContext(const MGlxFetchContext* aContext)
    {
    TRACER("CGlxMediaList::RemoveContext");    
    GLX_LOG_INFO1( "CGlxMediaList::RemoveContext(0x%x)", aContext );

    if (!aContext)
        {
        return;
        }

    TIdentityRelation<CGlxMediaList::TContext> match (&CGlxMediaList::TContext::Match);
    TContext context;
    context.iContext = aContext;
    context.iPriority = 0;
    TInt index = iContexts.Find(context, match);

    // remove the context if it was found, just ignore if its not found
    if (index != KErrNotFound) 
        {
        iContexts.Remove(index);
        // notify cache manager: garbage collection needs to be run to free the 
        // items that the removed context required
        TRAP_IGNORE( iManager->HandleWindowChangedL( this ) );
        }
    }

// -----------------------------------------------------------------------------
// Returns the isolated collection, that is used by this media list
// -----------------------------------------------------------------------------
//
MMPXCollection& CGlxMediaList::Collection() const
    {
    TRACER("CGlxMediaList::Collection");
    __ASSERT_DEBUG(iCollectionUtility != NULL, Panic(EGlxPanicNullPointer));
    return iCollectionUtility->Collection();
    }

// -----------------------------------------------------------------------------
// Returns the current path
// -----------------------------------------------------------------------------
CMPXCollectionPath* CGlxMediaList::PathLC(NGlxListDefs::TPathType aType) const
    {
    TRACER("CGlxMediaList::PathLC");  

    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL(path);

    // Add navigation to parent
    PathPopulateParentL( *path );

    if ( aType == NGlxListDefs::EPathAllOrSelection ||
         aType == NGlxListDefs:: EPathFocusOrSelection )
        {
        // If selection exists, add only selection
        // Otherwise add all items or focused item
        if ( iItemList->SelectedItemIndices().Count() > 0 )
            {
            PathPopulateSelectionL( *path );
            }
        else
            {
            if ( aType == NGlxListDefs::EPathAllOrSelection )
                {
                PathPopulateAllL( *path );
                }
            else if ( aType == NGlxListDefs:: EPathFocusOrSelection )
                {
                PathPopulateFocusL( *path );
                }
            }
        }

    return path;
    }

// -----------------------------------------------------------------------------
// Determines if an item has been selected
// -----------------------------------------------------------------------------
TBool CGlxMediaList::IsSelected(TInt aIndex) const
    {
    TRACER("CGlxMediaList::IsSelected");    
    GLX_LOG_INFO1( "CGlxMediaList::IsSelected %d", aIndex );
    return iItemList->IsSelected( aIndex );
    }

// -----------------------------------------------------------------------------
// Handles item selection/deselection
// -----------------------------------------------------------------------------
void CGlxMediaList::SetSelectedL(TInt aIndex, TBool aSelected)
    {
    TRACER("CGlxMediaList::SetSelectedL");    
    GLX_LOG_INFO2( "CGlxMediaList::SetSelectedL %d:%d", aIndex, aSelected );
    iItemList->SetSelectedL( aIndex, aSelected );
    }

// -----------------------------------------------------------------------------
// Returns selection count
// -----------------------------------------------------------------------------
TInt CGlxMediaList::SelectionCount() const
    {
    TRACER("CGlxMediaList::SelectionCount");
    
    return iItemList->SelectedItemIndices().Count();
    }

// -----------------------------------------------------------------------------
// Returns selected item index from selection
// -----------------------------------------------------------------------------
TInt CGlxMediaList::SelectedItemIndex(TInt aSelectionIndex) const
    {
    TRACER("CGlxMediaList::SelectedItemIndex");    
    GLX_LOG_INFO1( "CGlxMediaList::SelectedItemIndex %d", aSelectionIndex );
    return iItemList->SelectedItemIndices()[ aSelectionIndex ];
    }

// -----------------------------------------------------------------------------
// Sends a command to the collection
// -----------------------------------------------------------------------------
void CGlxMediaList::CommandL(CMPXCommand& aCommand)
    {
    TRACER("CGlxMediaList::CommandL");

    // Multiple commands should not happen
    __ASSERT_DEBUG( !iCommandPending, Panic( EGlxPanicLogicError ) );

    // Send the command
    Collection().CommandL( aCommand );

    // Use the sessionId to indicate that a command is pending
    __ASSERT_DEBUG( aCommand.IsSupported( KMPXCommandGeneralSessionId ), 
                    Panic( EGlxPanicLogicError ) );

    iCommandPending = aCommand.ValueTObjectL<TAny*>( KMPXCommandGeneralSessionId );
    }

// -----------------------------------------------------------------------------
// Checks if a command is active or not
// -----------------------------------------------------------------------------
TBool CGlxMediaList::IsCommandActive()
    {
    TRACER("CGlxMediaList::IsCommandActive");

    if(iCommandPending)
        {
         return ETrue;  
        }
    else
        {
         return EFalse;
        }
    }


// -----------------------------------------------------------------------------
// Cancels a command on the collection
// -----------------------------------------------------------------------------
void CGlxMediaList::CancelCommand()
    {
    TRACER("CGlxMediaList::CancelCommand");

    // Cancelling a non issued command should not happen
    __ASSERT_DEBUG( iCommandPending, Panic( EGlxPanicLogicError ) );

    // Cancel command
    Collection().CancelRequest();
    iCommandPending = NULL;
    }

// -----------------------------------------------------------------------------
// Specify filter on the media list
// -----------------------------------------------------------------------------
void CGlxMediaList::SetFilterL(CMPXFilter* aFilter)
    {
    TRACER("CGlxMediaList::SetFilterL");
    
    // This method now takes a copy of the filter. It does not take ownership of aFilter
    
    CMPXFilter* tempFilter = NULL;
    
    if (aFilter)
        {
        // Create copy of filter
        tempFilter = CMPXFilter::NewL(*aFilter);
        CleanupStack::PushL(tempFilter);
        }
    
    // Set filter on the collection
    Collection().SetFilterL(aFilter);

    // Re-open collection if filter has been applied and list is already populated
    if ( iIsPopulated )
        {
        ReOpenL();

        iReorderPending = ETrue;
        }

    delete iFilter;
    iFilter = NULL;

    if (aFilter)
        {
        // SetFilterL did not leave. So take ownership of copy
        CleanupStack::Pop(tempFilter);
        iFilter = tempFilter;
        }
    }

// -----------------------------------------------------------------------------
// Returns filter on the media list
// -----------------------------------------------------------------------------
CMPXFilter* CGlxMediaList::Filter() const
    {
    TRACER("CGlxMediaList::Filter");
    
    return iFilter;
    }

// ---------------------------------------------------------------------------
// IdSpaceId
// ---------------------------------------------------------------------------
//
TGlxIdSpaceId CGlxMediaList::IdSpaceId(TInt aIndex) const
    {
    TRACER("CGlxMediaList::IdSpaceId");
    
    ///@todo Delegate id space to item list.
    if (Count() && iItemList->Item(aIndex).IsStatic())
    	{
    	return KGlxStaticItemIdSpaceId;
    	}
    else
    	{
    	return iIdSpaceId;
    	}
    }

// ---------------------------------------------------------------------------
// CGlxMediaList::IsPopulated()
// ---------------------------------------------------------------------------
//
TBool CGlxMediaList::IsPopulated() const
    {
    TRACER("CGlxMediaList::IsPopulated");
    
    return iIsPopulated;
    }


// -----------------------------------------------------------------------------
// Add a static item
// -----------------------------------------------------------------------------
//
void CGlxMediaList::AddStaticItemL( CGlxMedia* aStaticItem, 
        NGlxListDefs::TInsertionPosition aTargetPosition )
    {
    TRACER("CGlxMediaList::AddStaticItemL");    
    
    iItemList->AddStaticItemL( aStaticItem, aTargetPosition );
    }

// -----------------------------------------------------------------------------
// Remove a static item
// -----------------------------------------------------------------------------
//
void CGlxMediaList::RemoveStaticItem(const TGlxMediaId& aItemId)
	{
	TRACER("CGlxMediaList::RemoveStaticItem");
    
    iItemList->Remove(TGlxIdSpaceId(KGlxStaticItemIdSpaceId), aItemId);
	}

// -----------------------------------------------------------------------------
// Enable/disable static items
// -----------------------------------------------------------------------------
//
void CGlxMediaList::SetStaticItemsEnabled( TBool aEnabled )
    {
    TRACER("CGlxMediaList::SetStaticItemsEnabled");
    
    iItemList->SetStaticItemsEnabled( aEnabled );
    }
    
// -----------------------------------------------------------------------------
// return ETrue if static items are enabled
// -----------------------------------------------------------------------------
//
TBool CGlxMediaList::IsStaticItemsEnabled() const
    {
    TRACER("CGlxMediaList::IsStaticItemsEnabled");
        
    return iItemList->IsStaticItemsEnabled();
    }

// -----------------------------------------------------------------------------
// Sets the initial focus position
// -----------------------------------------------------------------------------
//
void CGlxMediaList::SetFocusInitialPosition(NGlxListDefs::TFocusInitialPosition aFocusInitialPosition)
    {
    TRACER("CGlxMediaList::SetFocusInitialPosition");    
    
    iItemList->SetFocusInitialPosition( aFocusInitialPosition );
    }

// -----------------------------------------------------------------------------
// Resets the focus to the initial position
// -----------------------------------------------------------------------------
//
void CGlxMediaList::ResetFocus()
    {
    TRACER("CGlxMediaList::ResetFocus");
        
    iItemList->ResetFocus();
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle collection message
// ---------------------------------------------------------------------------
void CGlxMediaList::HandleCollectionMessageL(const CMPXMessage& aMessage)
    {
    TRACER("CGlxMediaList::HandleCollectionMessageL");
    
    
    if (aMessage.IsSupported(KMPXMessageGeneralId))
        {
        TInt messageId = aMessage.ValueTObjectL<TInt>(KMPXMessageGeneralId);
        if ( messageId == KMPXMessageGeneral )
            {
            if (!aMessage.IsSupported(KMPXMessageGeneralEvent) ||
                !aMessage.IsSupported(KMPXMessageGeneralType))
                {
                return;
                }

            TInt messageEvent = aMessage.ValueTObjectL<TInt>(KMPXMessageGeneralEvent);
            if (messageEvent == TMPXCollectionMessage::EPathChanged)
                {
                GLX_LOG_INFO("CGlxMediaList::HandleCollectionMessageL() EPathChanged");

                TInt messageType = aMessage.ValueTObjectL<TInt>(KMPXMessageGeneralType);
                switch (messageType)
                    {
                    case EMcPathChangedByOpen:
                        {
                        HandleOpenL();
                        break;
                        }
                    case EMcPathChangedByCollectionChange:
                        {
                        break;
                        }
                    default:
                        __ASSERT_DEBUG(EFalse, Panic(EGlxPanicLogicError));
                        break;
                    }
                }
            }
        else if (messageId == KMPXMessageIdItemChanged)
            {
            if (!aMessage.IsSupported(KMPXMessageMediaGeneralCategory) ||
                !aMessage.IsSupported(KMPXMessageChangeEventType) ||
                !aMessage.IsSupported(KMPXMessageMediaGeneralId))
                {
                return;
                }

            TMPXChangeEventType messageType = aMessage.ValueTObjectL<TMPXChangeEventType>(KMPXMessageChangeEventType);
            switch (messageType)
                {
                case EMPXItemModified:
                    {
                    RArray<TMPXAttribute> attributes;
                    CleanupClosePushL(attributes);
                    TMPXItemId itemId = aMessage.ValueTObjectL<TMPXItemId>(KMPXMessageMediaGeneralId);
                    HandleItemModifiedL(itemId, attributes);
                    CleanupStack::PopAndDestroy(&attributes);
                    iManager->HandleWindowChangedL(this);
                    //Fix for Bug 'ESLM-827JU8 Vasco w03':Dont 'ReOpenL()' in any case
	      			//Side Effect: Does not reorder album list in case of renaming album (To be handled later).
	      			break;
                    // Drop through to perform sync, in case the order has changed
                    }
                case EMPXItemInserted:
                case EMPXItemDeleted:
                default:
                    // Items have changed, determine whether to sync now
                    // or resync later if a sync is already pending after opening
                    if ( iSyncStatus == KNonePending )
                        {
                        ReOpenL(); // force re-opens

                        iSyncStatus = KSyncPending;
                        }
                    else
                        {
                        iSyncStatus = KResyncPending;
                        }
                    break;
                }
            }
        else // received message isn't handled by media list
            {
            // Inform observers of message
            TMessageNotificationStrategy strategy( aMessage, *this );
            NotifyObservers( strategy );
            }
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d a folder
// ---------------------------------------------------------------------------
//
void CGlxMediaList::HandleOpenL(const CMPXMedia& /*aEntries*/, TInt /*aIndex*/,
        TBool /*aComplete*/, TInt aError)
    {
    TRACER("CGlxMediaList::HandleOpenL");
    
    /// @todo Need to handle errors
    __ASSERT_DEBUG(aError == KErrNone, Panic(EGlxPanicLogicError));
    HandleOpenL();
    }

// -----------------------------------------------------------------------------
// HandleOpenL
// -----------------------------------------------------------------------------
//
void CGlxMediaList::HandleOpenL()
    {
    TRACER("CGlxMediaList::HandleOpenL");

    // if we dont have the item list constructed
    // dont do anything as it would lead to a crash
    if( iItemList )
        {
        PopulateL();
        // Sync (via population) has now occured,
        // determine whether to resync if one is pending
        if ( iSyncStatus == KResyncPending )
            {
            ReOpenL();

            iSyncStatus = KSyncPending;
            }

        iManager->HandleWindowChangedL(this);
        }
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handles the collection entries being opened. Typically called
// when client has Open()'d an item. Client typically responds by
// 'playing' the item
// ---------------------------------------------------------------------------
//
void CGlxMediaList::HandleOpenL(const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/)
    {
    TRACER("CGlxMediaList::HandleOpenL" );
    __ASSERT_DEBUG(EFalse, Panic(EGlxPanicLogicError));
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle extended media properties
// ---------------------------------------------------------------------------
//
void CGlxMediaList::HandleCollectionMediaL(const CMPXMedia& aMedia, TInt aError)
    {
    TRACER("CGlxMediaList::HandleCollectionMediaL" );

    iManager->HandleCollectionMediaL(iIdSpaceId, aMedia, aError);
    }

// ---------------------------------------------------------------------------
// From MMPXCollectionObserver
// Handle command completion
// ---------------------------------------------------------------------------
//
void CGlxMediaList::HandleCommandComplete(CMPXCommand* aCommandResult, TInt aError)
    {
    TRACER("CGlxMediaList::HandleCommandComplete" );

    // SessionId is stored in iCommandPending
    TAny* sessionId = iCommandPending;

    // Clear command pending flag here, in case an observer issues another command
    iCommandPending = NULL;

    TCommandCompletedNotificationStrategy strategy( sessionId, aCommandResult, aError, *this );
    NotifyObservers( strategy );
    }

// -----------------------------------------------------------------------------
// NewLC
// -----------------------------------------------------------------------------
//
CGlxMediaList* CGlxMediaList::NewLC(const TGlxHierarchyId& aHierarchyId)
    {
    TRACER("CGlxMediaList::NewLC" );
    
    CGlxMediaList* obj = new (ELeave) CGlxMediaList(aHierarchyId);
    CleanupStack::PushL(obj);
    obj->ConstructL();
    return obj;
    }

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
//
CGlxMediaList::CGlxMediaList(const TGlxHierarchyId& aHierarchyId) :
    iIdSpaceId(KGlxIdNone),
    iHierarchyId(aHierarchyId), iVisibleWindowIndex( 0 )
    {
    TRACER("CGlxMediaList::CGlxMediaList");
    
    __ASSERT_DEBUG(KErrNotFound == -1, Panic(EGlxPanicDebugUnexpectedError));
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CGlxMediaList::~CGlxMediaList()
    {
    TRACER("CGlxMediaList::~CGlxMediaList" );
    
    if (iCollectionUtility)
        {
        iCollectionUtility->Close();
        }

    iErrorPoster->Close();

    iItemListObservers.Close();
    __ASSERT_DEBUG(iContexts.Count() == 0, Panic(EGlxPanicLogicError)); // Release all contexts
    iContexts.Close();
    iPath.Close();

    delete iItemList;
    delete iFilter;
    
    if (iMediaListArray)
    	{
    	iMediaListArray->Close();
    	}
    
    iCountAttributes.Close();

    // close the manager last as it may delete the cache and thus the CGlxMedia 
    // objects.. item list destructor manipulates those objects so need to do it in this order
	if(iManager)
		{
		iManager->HandleListDeleted( this );
		iManager->Close();
		}    
    }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CGlxMediaList::ConstructL()
    {
    TRACER("CGlxMediaList::ConstructL" );
    
    iCollectionUtility = MMPXCollectionUtility::NewL(this, KMcModeIsolated);
    iManager = CGlxCacheManager::InstanceL();
    iErrorPoster = CGlxErrorPoster::InstanceL();
    iMediaListArray = CGlxMediaListArray::InstanceL();
    iCountAttributes.AppendL(KGlxMediaCollectionPluginSpecificSubTitle);
    iCountAttributes.AppendL(KGlxMediaGeneralSlideshowableContent);
    }

// -----------------------------------------------------------------------------
// Initialize the media list
// -----------------------------------------------------------------------------
//
void CGlxMediaList::OpenL(const CMPXCollectionPath& aPath)
    {
    TRACER("CGlxMediaList::OpenL" );
    
    __ASSERT_DEBUG(iPath.Count() == 0, Panic(EGlxPanicAlreadyInitialised));

    // Copy the path's depth dimension
    TInt levels = aPath.Levels();

    iPath.ReserveL(levels);
    for (TInt level = 0; level < levels; level++) 
        {
        TGlxMediaId id(aPath.Id(level));
        iPath.AppendL(id); 
        }

    
	// Id space ids will no longer be retrieved from the collection framework
	// See ID:  ESLU-7C8CVN Inc9 MP: Error "Program closed: Music player" occurs if 
    // user presses Rocker Select key continually in Album art view.
    
    // Consider the following scenario:
    // 1. User launches fetcher dialog
    // 2. Fetcher opens a medialist
    // 3. MGlxMediaList::InstanceL starts an async wait loop allowing other active objects to run
    // 4. User cancels fetcher
    // 5. Fetcher does not have a handle to the MGlxMediaList so it cannot cancel or close it.
    // --> We leak a medialist and leave an async wait loop in client processes active scheduler.

    if (levels)
    	{
    	iIdSpaceId = aPath.Id(0);
    	}
    else
    	{
    	iIdSpaceId = KGlxIdSpaceIdRoot;
    	}
    
    iItemList = CGlxNavigableList::NewL( iIdSpaceId, *this, *this );

    OpenCollectionL( aPath );
    }

// -----------------------------------------------------------------------------
// AddReference
// -----------------------------------------------------------------------------
//
TInt CGlxMediaList::AddReference()
    {
    TRACER("CGlxMediaList::AddReference");
    
    iReferenceCount++;
    return iReferenceCount;
    }
    
// -----------------------------------------------------------------------------
// RemoveReference
// -----------------------------------------------------------------------------
//
TInt CGlxMediaList::RemoveReference()
    {
    TRACER("CGlxMediaList::RemoveReference");
    
    __ASSERT_ALWAYS(iReferenceCount > 0, Panic(EGlxPanicLogicError));
    iReferenceCount--;
    return iReferenceCount;
    }
    
// -----------------------------------------------------------------------------
// ReferenceCount
// -----------------------------------------------------------------------------
//
TInt CGlxMediaList::ReferenceCount() const
    {
    TRACER("CGlxMediaList::ReferenceCount");
    
    return iReferenceCount;
    }

// -----------------------------------------------------------------------------
// Returns ETrue if this media list refers to the path
// -----------------------------------------------------------------------------
//
TBool CGlxMediaList::Equals(const CMPXCollectionPath& aPath) const 
    {
    TRACER("CGlxMediaList::Equals" );
    
    TInt myLevels = iPath.Count();
    TInt pathLevels = aPath.Levels();
    if (myLevels != pathLevels)
        {
        return EFalse;
        }

    // Check if path's match
    for (TInt i = 0; i < myLevels; i++) 
        {
        if (iPath[i] != TGlxMediaId(aPath.Id(i)))
            {
            return EFalse;
            }
        }

    return ETrue; // Match		
    }

// -----------------------------------------------------------------------------
// Determines if a filter has been set
// -----------------------------------------------------------------------------
TBool CGlxMediaList::IsFiltered() const
    {
    TRACER("CGlxMediaList::IsFiltered");
    
    return iFilter != NULL;
    }

// -----------------------------------------------------------------------------
// Synchronise the media list
// -----------------------------------------------------------------------------
void CGlxMediaList::ReOpenL()
    {
    TRACER("CGlxMediaList::ReOpenL" );
    
    // We must not re-open the list before it has been opened the first time
    // note - this can happen if update messages are received whilst retreiving
    // the id space id
    if ( !iItemList )
        {
        return;
        }
    
    CMPXCollectionPath* path = PathLC( NGlxListDefs::EPathParent );

    OpenCollectionL( *path );

    CleanupStack::PopAndDestroy(path);
    }

// -----------------------------------------------------------------------------
// Populates the list, i.e., opens the collection with the path
// -----------------------------------------------------------------------------
//
void CGlxMediaList::PopulateL()
    {
    TRACER("CGlxMediaList::PopulateL" );

    // Reserve space for all items in cache, that this media list is a potential user
    // This allows SetContentsL to build user links without having to reserve space
    iManager->ReserveUsersL( iIdSpaceId, CGlxMediaList::MediaListsL().Count() );

    CMPXCollectionPath* path = Collection().PathL();
    CleanupStack::PushL(path);

    if ( iReorderPending )
        {
        iItemList->ReorderContentsL( *path, *iManager );
        iReorderPending = EFalse;
        }
    else
        {
        iItemList->SetContentsL( *path, *iManager );

        // Sync has now occured,
        // if only a sync was pending, clear sync status
        if ( iSyncStatus == KSyncPending )
            {
            iSyncStatus = KNonePending;
            }
        }

    CleanupStack::PopAndDestroy(path);

    // The list contents may have changed, so update each used media with the
    // current index into the list
    UpdateMedia();

    // Inform observers of first time population
    if (!iIsPopulated)
        {
        TListPopulatedNotificationStrategy strategy( *this );
        iIsPopulated = ETrue; // Do this only once.
        NotifyObservers( strategy );
        }
    }

// -----------------------------------------------------------------------------
// Handles item modifications
// -----------------------------------------------------------------------------
void CGlxMediaList::HandleItemModifiedL(TInt aId, const RArray<TMPXAttribute>& aAttributes)
    {
    TRACER("CGlxMediaList::HandleItemModifiedL");
    GLX_LOG_INFO1( "CGlxMediaList::HandleItemModifiedL %d", aId );
    TGlxMediaId id(aId);

    /// @todo: Check that the correct IdSpaceId is being used here
    
    TInt index = Index(iIdSpaceId, id);

    if (index != KErrNotFound)
        {
        iManager->HandleItemModified(iIdSpaceId, id, aAttributes);

        RArray<TInt> itemIndices;
        CleanupClosePushL(itemIndices);
        itemIndices.AppendL(index);

        // Inform observers of modified items
        TInt obsCount = iItemListObservers.Count();
        GLX_DEBUG2("ML:HandleItemModifiedL obsCount=%d", obsCount);
        for (TInt obsIdx = 0; obsIdx < obsCount; obsIdx++)
            {
            MGlxMediaListObserver* obs = iItemListObservers[obsIdx];
            obs->HandleItemModifiedL(itemIndices, this);
            }

        CleanupStack::PopAndDestroy(&itemIndices);
        RPointerArray<CGlxMediaList>& mediaLists = iMediaListArray->Array();
        TInt listCount = mediaLists.Count();
        GLX_DEBUG2("ML:HandleItemModifiedL listCount=%d", listCount);
        if (listCount > 0)
            {
            CGlxMediaList* mediaList = mediaLists[listCount-1];
            if (mediaList == this)
                {
                GLX_DEBUG3("ML:HandleItemModifiedL(wait) listCount=%d, Id=%d",
                                                  listCount, id.Value());
                TTimeIntervalMicroSeconds32 timeout;
                timeout = (mediaList->Count() > KMaxItemsCount ?
                  KModifyEventMaxWaitInterval : KModifyEventMinWaitInterval );
                RTimer timer;
                CleanupClosePushL(timer);
                TRequestStatus status;
                timer.CreateLocal();
                timer.After(status, timeout);
                User::WaitForRequest(status);
                CleanupStack::PopAndDestroy(&timer);
                }
            }
        }
    }

// -----------------------------------------------------------------------------
// NotifyObserversOfMediaL
// -----------------------------------------------------------------------------
//
void CGlxMediaList::NotifyObserversOfMediaL(TInt aListIndex)
    {
    TRACER("CGlxMediaList::NotifyObserversOfMediaL");
    GLX_LOG_INFO1( "CGlxMediaList::NotifyObserversOfMediaL %d", aListIndex );
    TInt count = iItemListObservers.Count();
    for (TInt i = 0; i < count; i++)
        {
        iItemListObservers[i]->HandleMediaL(aListIndex, this);
        }
    }
        
// -----------------------------------------------------------------------------
// Notify observers of items added
// -----------------------------------------------------------------------------
//
void CGlxMediaList::HandleItemsAdded( TInt aAddedAtIndex, TInt aCount )
    {
    TRACER("CGlxMediaList::HandleItemsAdded");
    
    TItemsAddedNotificationStrategy strategy( aAddedAtIndex, aCount, *this );
    NotifyObservers( strategy );
    }

// -----------------------------------------------------------------------------
// Notify observers of items removed
// -----------------------------------------------------------------------------
//
void CGlxMediaList::HandleItemsRemoved( TInt aRemovedFromIndex, TInt aCount )
    {
    TRACER("CGlxMediaList::HandleItemsRemoved");
    
    TItemsRemovedNotificationStrategy strategy( aRemovedFromIndex, aCount,
        *this );
    NotifyObservers( strategy );
    }

// -----------------------------------------------------------------------------
// Notify observers of focus change
// -----------------------------------------------------------------------------
//
void CGlxMediaList::HandleFocusChanged( NGlxListDefs::TFocusChangeType aType, 
        TInt aNewIndex, TInt aOldIndex )
    {
    TRACER("CGlxMediaList::HandleFocusChanged");
    
    TFocusChangedNotificationStrategy strategy( aType, aNewIndex, aOldIndex,
        *this );
    NotifyObservers( strategy );
    }

// -----------------------------------------------------------------------------
// Notify observers of selection having changed
// -----------------------------------------------------------------------------
//
void CGlxMediaList::HandleItemSelected( TInt aIndex, TBool aSelected )
    {
    TRACER("CGlxMediaList::HandleItemSelected");
    
    TItemSelectedNotificationStrategy strategy( aIndex, aSelected, *this );
    NotifyObservers( strategy );
    }

// -----------------------------------------------------------------------------
// Call strategy object for each observer
// -----------------------------------------------------------------------------
//
void CGlxMediaList::NotifyObservers( TNotificationStrategy& aStrategy )
    {
    TRACER("CGlxMediaList::NotifyObservers");
    
    // Notify all observers, even if some of them leave leave.
    // "i" needs to be volatile, so that it does not get optimised into a 
    // register. If i were in a register and observer left, i's new value 
    // would be lost. 
    volatile TInt i = iItemListObservers.Count() - 1;
    
    // Loop count + 1 times, so that iManager also gets notified if last 
    // observer leaves. Iterate backwards in case observes remove themselves
    // from the observer array during the callbacks.
    while ( i >= -1 ) // usually tested only twice
        {
        TRAPD( err, 
            {
            while ( i >= 0 )
                {
                aStrategy.NotifyL( *iItemListObservers[ i ] );
                i--;
                }

            iManager->HandleWindowChangedL( this );
            }
        );
        
        if ( err != KErrNone )
            {
            iErrorPoster->PostError(err);
            }

        i--;
        }
    }

// -----------------------------------------------------------------------------
// Populates the path with hierarchy to parent
// -----------------------------------------------------------------------------
//
inline void CGlxMediaList::PathPopulateParentL(CMPXCollectionPath& aPath) const
    {
    TRACER("CGlxMediaList::PathPopulateParentL");
    
    // Add navigational hierarchy to path
    TInt pathCount = iPath.Count();
    for (TInt i = 0; i < pathCount; ++i)
        {
        aPath.AppendL( iPath[ i ].Value() );
        }
    }

// -----------------------------------------------------------------------------
// Populates the path with all items and sets the focus
// -----------------------------------------------------------------------------
//
inline void CGlxMediaList::PathPopulateAllL(CMPXCollectionPath& aPath) const
    {
    TRACER("CGlxMediaList::PathPopulateAllL");
    
    RArray<TMPXItemId> mpxIds;
    CleanupClosePushL( mpxIds );

    // Reserve space for all items
    TInt itemCount = iItemList->Count();
    mpxIds.ReserveL( itemCount );

    // Obtain TMPXItemId for each item
    for (TInt i = 0; i < itemCount; ++i)
        {
        if ( !iItemList->Item( i ).IsStatic() )
            {
            mpxIds.AppendL( iItemList->Item( i ).Id().Value() );
            }
        }

    // Add Ids to current level in path
    aPath.AppendL( mpxIds.Array() );

    // Set focused item
    TInt focusIndex = FocusIndex();
    if ( focusIndex >= 0 )
        {
        if ( !iItemList->Item( focusIndex ).IsStatic() )
            {
            aPath.Set( focusIndex - iItemList->Count( NGlxListDefs::ECountPreStatic ) );
            }
        }

    CleanupStack::PopAndDestroy( &mpxIds );
    }

// -----------------------------------------------------------------------------
// Populates the path with focused item and sets the focus
// -----------------------------------------------------------------------------
//
inline void CGlxMediaList::PathPopulateFocusL(CMPXCollectionPath& aPath) const
    {
    TRACER("CGlxMediaList::PathPopulateFocusL");
    
    // Obtain focused item
    TInt focusIndex = FocusIndex();
    if ( focusIndex >= 0 )
        {
        const TGlxMedia& item = iItemList->Item( focusIndex );
        if ( !item.IsStatic() )
            {
            // Add focused item to path
            aPath.AppendL( item.Id().Value() );
            aPath.Set( 0 );
            }
        }
    }

// -----------------------------------------------------------------------------
// Populates the path with selected items and selects all
// -----------------------------------------------------------------------------
//
inline void CGlxMediaList::PathPopulateSelectionL(CMPXCollectionPath& aPath) const
    {
    TRACER("CGlxMediaList::PathPopulateSelectionL");
    
    RArray<TMPXItemId> mpxIds;
    CleanupClosePushL( mpxIds );

    // Reserve space for all items
    TInt selectionCount = iItemList->SelectedItemIndices().Count();
    mpxIds.ReserveL( selectionCount );

    // Obtain TMPXItemId for each item
    for (TInt i = 0; i < selectionCount; ++i)
        {
        TInt selectedItemIndex = iItemList->SelectedItemIndices()[ i ];
        mpxIds.AppendL( iItemList->Item( selectedItemIndex ).Id().Value() );
        }

    // Add Ids to current level in path
    aPath.AppendL( mpxIds.Array() );

    // Set selection
    aPath.SelectAllL();

    CleanupStack::PopAndDestroy( &mpxIds );
    }

// -----------------------------------------------------------------------------
// Updates each media used by this media list with the current index
// -----------------------------------------------------------------------------
//
inline void CGlxMediaList::UpdateMedia()
    {
    TRACER("CGlxMediaList::UpdateMedia");
    
    TInt count = iItemList->Count();
    GLX_DEBUG2("CGlxMediaList::UpdateMedia() count=%d", count);    
    for (TInt i = 0; i < count; ++i)
        {
        TGlxMedia& item = iItemList->Item( i );

        // static items should not be updated
        if ( !item.IsStatic() )
            {
            item.UpdateMedia( *this, i );
            UpdateMediaInvalidateAttributesChangedByCounts(item);
            }
        }
    }

// -----------------------------------------------------------------------------
// Updates each media used by this media list with the current index
// -----------------------------------------------------------------------------
//
inline void CGlxMediaList::UpdateMediaInvalidateAttributesChangedByCounts(TGlxMedia& aItem)
    {
    TRACER("CGlxMediaList::UpdateMediaInvalidateAttributesChangedByCounts");
    
    iManager->HandleItemModified(iIdSpaceId, aItem.Id(), iCountAttributes);
    }
    
// -----------------------------------------------------------------------------
// Opens a collection at the appropriate level
// -----------------------------------------------------------------------------
//
void CGlxMediaList::OpenCollectionL(const CMPXCollectionPath& aPath)
    {
    TRACER("CGlxMediaList::OpenCollectionL");
    
    // Open the level
    if ( aPath.Levels() == 0 )
        {
        Collection().OpenL( TUid::Uid( EGlxCollectionPluginShowInMainListView ) );
        }
    else
        {
        Collection().OpenL( aPath );
        }
    }


// -----------------------------------------------------------------------------
// Compare contexts by pointer
// -----------------------------------------------------------------------------
//
TBool CGlxMediaList::TContext::Match(const TContext& a1, const TContext& a2)
    {
    TRACER("CGlxMediaList::TContext::Match");
    
    return a1.iContext == a2.iContext;
    }

// -----------------------------------------------------------------------------
// Compare contexts by priority
// -----------------------------------------------------------------------------
//
TBool CGlxMediaList::TContext::Compare(const TContext& a1, const TContext& a2)
    {
    TRACER("CGlxMediaList::TContext::Compare");
    
    return a2.iPriority - a1.iPriority;
    }

// -----------------------------------------------------------------------------
// Set the visible dataWindow index
// -----------------------------------------------------------------------------
//
void CGlxMediaList::SetVisibleWindowIndexL( TInt aIndex )
    {
    TRACER("CGlxMediaList::SetVisibleWindowIndexL");
    __ASSERT_ALWAYS( aIndex <= Count(), Panic( EGlxPanicIllegalArgument ) );
    if( aIndex != iVisibleWindowIndex )
        {
        iVisibleWindowIndex = aIndex;
        GLX_DEBUG2("SetVisibleWindowIndexL() iVisibleWindowIndex=%d",
                                                        iVisibleWindowIndex);
        iManager->HandleWindowChangedL(this);
        iManager->RefreshL();
        }
    }
    
// -----------------------------------------------------------------------------
// Returns visible dataWindow index
// -----------------------------------------------------------------------------
//
TInt CGlxMediaList::VisibleWindowIndex() const
    {
    TRACER("CGlxMediaList::VisibleWindowIndex");
    return iVisibleWindowIndex;
    }

// -----------------------------------------------------------------------------
// Cancels the pending attribute/thumbnail requests 
// -----------------------------------------------------------------------------

void CGlxMediaList::CancelPreviousRequests()
	{
	TRACER("CGlxMediaList::CancelPreviousRequests");	
	
	
			// If media is NULL, cancel the previous pending request.
			// Place a new request for the item in focus, to fetch the media attributes
			iManager->CancelPreviousRequest();		
			}
