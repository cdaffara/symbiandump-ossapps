/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:    Categories related functionality*
*/




#ifndef VCXMYVIDEOSCATEGORIES_H
#define VCXMYVIDEOSCATEGORIES_H

// INCLUDES

#include <vcxmyvideosdefs.h>

// FORWARD DECLARATIONS
class CMPXMedia;
class CVcxMyVideosCollectionPlugin;

// CONSTANTS

// CLASS DECLARATION

/**
* MPX My Videos collection categories implementation.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosCategories) : CBase
    {    
    public: // Constructors and destructor

        /**
        * Two-phased constructor
        * @return object constructed
        */
        static CVcxMyVideosCategories* NewL ( CVcxMyVideosCollectionPlugin& aCollection );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosCategories();

        /**
        * Creates Media container with categories.
        */
        CMPXMedia* CreateCategoryListL();

    public: // new methods
    
        /**
        * When CVcxMyVideosVideoCache::iCache->iVideoList is grown (appended),
        * this is called to update category attributes.
        *
        * @param aVideoList          Current video list cache.
        * @param aNewItemsStartIndex Start position of new items.
        */
        void UpdateCategoriesL( CMPXMedia& aVideoList, TInt aNewItemsStartIndex );
 
        /**
        * Called after video is added to iCollection.iCache.
        * Updates category variables accordingly.
        * Modify events to iCollection.iMessageList are added, but the list
        * is not sent.
        *
        * @param aMdsId Item which was added.
        */
        void VideoAddedL( TUint32 aMdsId );


        /**
        * Called after video is added to iCollection.iCache.
        * Updates category variables accordingly.
        * Modify events to iCollection.iMessageList are added, but the list
        * is not sent.
        *
        * @param aVideo Item which was added.
        */
        void VideoAddedL( CMPXMedia& aVideo );

        /**
        * Called after videos are added to iCollection.iCache.
        * Updates category variables accordingly.
        * Modify events to iCollection.iMessageList are added, but the list
        * is not sent.
        *
        * @param aIds Items which were added.
        */
        void VideosAddedL( RArray<TUint32>& aIds );

        /**
        * Called just _before_ video is removed from iCollection.iCache.
        * Updates category variables accordingly.
        * Modify events to iCollection.iMessageList are added, but the list
        * is not sent.
        *
        * @param aMdsId Item which is being removed.
        */
        void VideoRemovedL( TUint32 aMdsId );

        /**
        * Called just _before_ video is removed from iCollection.iCache.
        * Updates category variables accordingly.
        * Modify events to iCollection.iMessageList are added, but the list
        * is not sent.
        *
        * @param aVideo Item which is being removed.
        */
        void VideoRemovedL( CMPXMedia& aVideo );

        /**
        * Called just _before_ videos are removed from iCollection.iCache.
        * Updates category variables accordingly.
        * Modify events to iCollection.iMessageList are added, but the list
        * is not sent.
        *
        * @param aIds Items which are being removed.
        */
        void VideosRemovedL( RArray<TUint32>& aIds );

        /**
        * Resets video counters to 0 from categories. Does not
        * send events.
        */
        void ResetVideoCountersL();

        /**
        * Updates categories' New Video Name fields.
        * Sends iCollection.iMessageList if any of the categories is modified.
        */
        void UpdateCategoriesNewVideoNamesL();
        
        
        /**
        * Called when video is updated in a way it affects New Video Name (and date) in category.
        * This updates the category accordinly. This function adds modify events to
        * collections message list if category is edited. Does not flush the message list.
        *
        * @param aVideo     Video which was changed.
        * @param aModified  Set to ETrue if categories were modified.
        */
        void UpdateCategoryNewVideoNameAndDateL( CMPXMedia& aVideo, TBool& aModified );
        
        /**
        * Called when items New Video Flag is updated in iCollection.iCache.
        * This function updates video counters only, no New Video Name.
        *
        * @param aOldFlags  Items flags before update.
        * @param aNewFlags  Items flags after update.
        * @param aOrigin    Which origin is affected.
        * @param aModified  Set to ETrue if category counters were edited and events
        *                   were added to message list. This does not flush the message list.
        */
        void NewVideoFlagChangedL( TUint32 aOldFlags, TUint32 aNewFlags,
                TUint8 aOrigin, TBool& aModified );        

        /**
        * Checks in which categories the video belongs to and sends modified events for them.
        *
        * @param aVideo             Video which was modified.
        * @param aFlushMessageList  If ETrue, then the collections message list is sent to clients.
        * @param aExtraInfo         Extra information about the event, this value is written to
        *                           KVcxMediaMyVideosInt32Value attribute. If aExtraInfo
        *                           is 0, nothing is written.
        */
        void GenerateCategoryModifiedEventL( CMPXMedia& aVideo, TBool aFlushMessageList,
                TInt32 aExtraInfo = 0 );

        /**
        * Called when videos origin is changed in iCollection.iCache.
        * Updates category variables and sends necessarry events.
        *
        * @param aVideo       Video which is changed, flags variable is read from here.
        * @param aPrevOrigin  Previous origin.
        * @param aNewOrigin   New origin.
        */
        void OriginChangedL( CMPXMedia& aVideo, TUint8 aPrevOrigin, TUint8 aNewOrigin );
                    
    private:
        /**
        * Constructor
        */
        CVcxMyVideosCategories( CVcxMyVideosCollectionPlugin& aCollectionPlugin );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL ();
        
        /**
        * Creates CMPXMedia object with category values and appends it to
        * aCategoryArray.
        *
        * @param aTitle         Category title.
        * @param aId            Collection item ID
        * @param aCategoryArray New CMPXMedia category object is appended here. 
        */
        void AppendCategoryToArrayL( const TDesC& aTitle, TMPXItemId aId,
                CMPXMediaArray& aCategoryArray );
                
        /**
        * Updates video counts in categories.
        *
        * @param aCategoryArray       Category array, provided as function parameter since the
        *                             calling function already has it available.
        * @param aVideosIncrements    How many new items were added to each category.
        * @param aNewVideosIncrements How many videos with new flag were added to each category.
        */
        void UpdateVideosCountL( CMPXMediaArray& aCategoryArray,
                RArray<TInt>& aVideosIncrements, RArray<TInt>& aNewVideosIncrements );

        /**
        * Increases/decreases aCountAttribute in aCategory. Adds event to
        * iCollection.iMessageList if category is modified. aEventInfo is written
        * to event's extra info. Does not flush iCollection.iMessageList.
        *
        * @param aCategory       Category being modified.
        * @param aIncrement      This is added to category's current attribute value.
        * @param aCountAttribute Attribute ID.
        * @param aEventInfo      This is written to generated event's extra info attribute.
        * 
        */
        void UpdateVideosCountL( CMPXMedia& aCategory, TInt aIncrement,
                TMPXAttributeData aCountAttribute, TInt aEventInfo );

        /**
        * aVideo is added(or removed) to aCategory. Updates category variables accordingly. 
        *
        * @param aCategory      Category
        * @param aCategoryIndex Category index in iList
        * @param aAdded         ETrue if aVideo was added to iList, EFalse if being removedo.
        */
        void UpdateCategoryL( CMPXMedia& aCategory, TInt aCategoryIndex,
                CMPXMedia& aVideo, TBool aAdded );

        /**
        * Adds aAddition to aAttribute in aCategory.
        *
        * @param aCategory  Category to modify.
        * @param aAddition  How much increase/decrease the attribute.
        * @param aAttribute Which attribute to modify.
        */
        void AddToTUint32AttributeL( CMPXMedia& aCategory, TInt aAddition,
                TMPXAttributeData aAttribute );

        /**
        * Returns the corresponding category index in category list (=iList) for aOrigin.
        * Notice that category index is the same as categorys MPXIDs iId1 value.
        * They should be kept the same.
        *
        * @param aOrigin  Origin, see values from TVcxMyVideosOrigin, if -1 given, then
        *                 index for All category is returned.
        * @return         Index in category list (=iList)
        */
        TInt CategoryIndex( TInt aOrigin );
                
        /**
        * Updates categorys New Video Name and Creation date fields. Sets aModified to
        * ETrue if value is changed, also adds modify event to event list,
        * but does not send it.
        *
        * @param aOrigin     Identifies the category, -1 is used for All category.
        * @param aModified   Set to ETrue if category item is modified. Otherwise
        *                    the value is not touched.
        * @param aIgnoredIds Items with these MDS ids are ignored on the update.
        *                    Usefull if items are being deleted but are still on the
        *                    video list.
        */
        void UpdateCategoryNewVideoNameAndDateL( TInt aOrigin, TBool& aModified,
                RArray<TUint32>& aIgnoredIds );
        
        /**
        * Goes through aVideoList and checks all videos which have new video
        * flag set and returns the latest ones name. Pointer to latest video
        * object is written to aNewVideo.
        *
        * @param aVideoList  Video list
        * @param aOrigin     Can be used to filter search. Only items which have
        *                    matching origin are included to the operation.
        *                    If -1, then all items are included.
        * @param aIgnoredIds Items with these MDS ids are ignored on the update.
        *                    Usefull if items are being deleted but are still on the
        *                    video list.
        * @param aNewVideo   Pointer to media object containing the new video name is
        *                    written here. NULL is written if not found. Ownership
        *                    does not move (stays in aVideoList).
        *                   
        */
        const TDesC& CalculateNewVideoNameL( CMPXMedia& aVideoList, TInt aOrigin,
                RArray<TUint32>& aIgnoredIds, CMPXMedia*& aNewVideo );

        /**
        * Video was added or is being removed in iCollection.iCache. Update
        * Categories accordingly.
        *
        * @param aMdsId Item which was added or removed.
        * @param aAdded ETrue if add operation, EFalse if remove.
        */
        void VideoAddedOrRemovedL( TUint32 aMdsId, TBool aAdded );

        /**
        * Video was added or is being removed in iCollection.iCache. Update
        * Categories accordingly.
        *
        * @param aVideo Item which was added or removed.
        * @param aAdded ETrue if add operation, EFalse if remove.
        */
        void VideoAddedOrRemovedL( CMPXMedia& aVideo, TBool aAdded );

        /**
        * Videos were added or are being removed in iCollection.iCache. Update
        * Categories accordingly.
        *
        * @param aIds Items which were added or are being removed.
        * @param aAdded ETrue if add operation, EFalse if remove.
        */
        void VideosAddedOrRemovedL( RArray<TUint32>& aIds, TBool aAdded );

        /**
        * Resets iModifiedCategories
        */
        void ResetModifiedCategoriesArray();

        /**
        * Creates events from iModifiedCategories and adds them
        * to iCollection.iMessageList. Does not send.
        */
        void AddEventsFromModifiedCategoriesArrayL();
        
    public:
    
        /**
        * Contains category list, this is kept in memory to speed things up.
        * Own.
        */
        CMPXMedia* iList;
        
        /**
        * Media array of iList, owned by iList.
        */
        CMPXMediaArray* iListArray;
    
    private:
        
        /**
        * Owner of this object.
        */
        CVcxMyVideosCollectionPlugin& iCollection;
        
        /**
        * Array containing boolean for each category, if set ETrue, then modified event
        * has been added for the category (to iCollection.iMessageList).
        */
        RArray<TBool> iModifiedCategories;
                                
    };

#endif   // VCXMYVIDEOSCATEGORIES_H


