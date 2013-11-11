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
* Description:   Contains cached MDS media items*
*/



#ifndef VCXMYVIDEOSVIDEOCACHE_H
#define VCXMYVIDEOSVIDEOCACHE_H

// INCLUDES
#include "vcxmyvideosmdsdb.h"

// FORWARD DECLARATIONS
class CVcxMyVideosCollectionPlugin;
class CMPXMedia;
class CRepository;

// CONSTANTS

// CLASS DECLARATION

/**
 * Contains information about video.
 * These are stored to CVcxMyVideosVideoListIndex::iVideoArray.
 */
NONSHARABLE_CLASS(TVcxMyVideosVideo)
    {
    public:

        /**
         * Constructor.
         */        
        TVcxMyVideosVideo();
        
        /**
        * = operator.
        */        
        TVcxMyVideosVideo& operator=( const TVcxMyVideosVideo& aVideo );
    public:
        
        /**
        * Set values.
        * 
        * @param aMdsId  MDS ID
        * @param aPos    Position in CVcxMyVideosVideoCache::iVideoList
        * @param aVideo  Pointer to CVcxMyVideosVideoCache::iVideoList item,
        *                ownership does not move.
        */
        void Set( TUint32 aMdsId, TInt aPos, CMPXMedia* aVideo );

    public:
        TUint32    iMdsId; // Video object ID in MDS.
        CMPXMedia* iVideo; // Pointer to CVcxMyVideosVideoCache::iVideoList item
        TInt       iPos;   // Items position in CVcxMyVideosVideoCache::iVideoList
    };

/**
* Used for keeping videos in MDS ID order for fast access.
* (Indexes CVcxMyVideosVideoCache::iVideoList).
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosVideoListIndex) : public CBase
    {    
public: // Constructors and destructor

    /**
    * Two-phased constructor
    * @return Object constructed
    */
    static CVcxMyVideosVideoListIndex* NewL();
    
    /**
    * Destructor
    */
    virtual ~CVcxMyVideosVideoListIndex();

public:
    
    /**
     * Sets up iVideoArray from aVideoList. Sorting is also done.
     * 
     * @param aVideoList  Video list to use for constructing iVideoArray. 
     */
    void SetL( const CMPXMedia& aVideoList );
    
    /**
     * Finds video by MDS ID from the index. Uses bisection method.
     * 
     * @param aVideo  The found video data is written here.
     * @return        KErrNotFound if not found, index of the item in iVideoArray
     *                otherwise.
     */
    TInt Find( TUint32 aMdsId, TVcxMyVideosVideo& aVideo );
    
    /**
     * Removes video from index.
     * 
     * @param aMdsId     ID if the item to be removed.
     * @param aCompress  If ETrue, compresses the video array.
     * @return           KErrNotFound if not found, index of the removed item otherwise.
     */
    TInt Remove( TUint32 aMdsId, TBool aCompress = ETrue );
    
    /**
     * Adds video to index. Keeps sorting order.
     * 
     * @param aVideo  Video to be added, ownership does not move.
     * @param aPos    aVideo's position in CVcxMyVideosVideoCache::iVideoList.
     */
    void AddL( CMPXMedia* aVideo, TInt aPos );

#ifdef _DEBUG
    /**
     * Returns reference to video array.
     */
    const RArray<TVcxMyVideosVideo>& VideoArray();
#endif
    
private:
    /**
    * Constructor
    */
    CVcxMyVideosVideoListIndex();
    
    /**
    * Symbian 2nd phase constructor.
    */
    void ConstructL ();

    /**
     * Sorts iVideoArray by MDS ID.
     */
    void Sort();
    
    /**
     * Used for keeping RArray<TVcxMyVideosVideo> in integer order by
     * TVcxMyVideosVideo::iMdsId.
     * 
     * @param aVideo1 Video to compare
     * @param aVideo2 Video to compare
     * @return -1 if aVideo1 is smaller than aVideo2, 1 if aVideo1 is larger than
     *         aVideo2.
     */
    static TInt CompareVideosByMdsId( const TVcxMyVideosVideo& aVideo1,
            const TVcxMyVideosVideo& aVideo2 );

private:
    
    /**
     * Video array which is kept in order by MDS ID.
     */
    RArray<TVcxMyVideosVideo> iVideoArray;
    };

/**
* Used for storing MDS items to RAM for fast access.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosVideoCache) : public CBase
    {    
    public: // Constructors and destructor

        /**
        * Two-phased constructor
        * @param aMyVideosCollectionPlugin  Owner of this object.
        * @return                           Object constructed
        */
        static CVcxMyVideosVideoCache* NewL (
                CVcxMyVideosCollectionPlugin& aMyVideosCollectionPlugin );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosVideoCache();
 
    public:
        
        /**
         * @return  ETrue if iVideoList is complete. (All items have
         *          been fetched from MDS.)
         */
        TBool IsComplete();
        
        /**
         * Set to ETrue when all items have been fetched from MDS.
         */
        void SetComplete( TBool aComplete );

    private:
        /**
        * Constructor
        */
        CVcxMyVideosVideoCache( CVcxMyVideosCollectionPlugin& aMyVideosCollectionPlugin );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL ();
        
        /**
        * Finds correct position in iVideoList for the new video.
        *
        * @param aMedia        Video to be inserted to the list.
        * @param aSortingOrder Sorting order.
        * @return              Position in the cache.
        */
        TInt FindCorrectPositionL( CMPXMedia& aMedia, TVcxMyVideosSortingOrder aSortingOrder );

        /**
        * Compares aNewVideo and aVideoInList in sizes, creation dates
        * or titles.
        *
        * @param aNewVideo     Video to be added to the list.
        * @param aVideoInList  Video in cache video list which is currently compared against
        *                      aNewVideo.
        * @param aSortingOrder Defines which attributes are compared. Size, Creation Date and Title
        *                      are possible.
        * @return              -1 if aVideoInList has smaller value, 1 if greater and 0 if equal.
        */
        TInt CompareL( CMPXMedia& aNewVideo, CMPXMedia& aVideoInList,
                TVcxMyVideosSortingOrder aSortingOrder );

        /**
        * Adds video to iVideoList to the correct position, according to sorting order.
        * MDS ID is used to identify the video.
        *
        * @param aVideo            Video to add.
        * @param aSortingOrder     Sorting order used.
        * @param aUpdateCategories If ETrue, then categories are updated.
        * @return                  KErrNone if added, KErrAlreadyExists if already exists and
        *                          was not added.
        */
        TInt AddToCorrectPlaceL( CMPXMedia& aVideo, TVcxMyVideosSortingOrder aSortingOrder,
                TBool aUpdateCategories = ETrue );

        /**
        * Moves a video to correct place on iVideoList, according to sorting order.
        * aVideo should be a refrence to iVideoList item. After the call the referenced
        * item is gone and the reference shouldn't be used anymore.
        *
        * @param aVideo        Video to move.
        * @param aSortingOrder Sorting order used.
        */                
        void MoveToCorrectPlaceL( CMPXMedia& aVideo,
                TVcxMyVideosSortingOrder aSortingOrder );

        /**
        * Removes video from iVideoList or iPartialVideoList.
        * MDS ID is used to identify the video.
        *
        * @param aVideo            Video to remove, MDS ID is read from here.
        * @param aUpdateCategories If ETrue, then categories are updated.
        * @return                  KErrNotFound if not found, KErrNone otherwise.
        */
        TInt RemoveL( CMPXMedia& aVideo, TBool aUpdateCategories = ETrue );
                
        /**
        * Fetches sorting order from Cenrep.
        * @return Sorting order.
        */
        TVcxMyVideosSortingOrder SortingOrderL();

        /**
        * Called when Title in video cache has been modified. 
        * Updates sorting order and category attributes if necessarry.
        * NOTICE that aVideoInCache is not allowed to own the media object, since this
        * function may change the pointer value to point to a new object.
        *
        * @param aVideoInCache  Video in cache which Title has been modified. aVideoInCache
        *                       is not allowed to own the data.
        */
        void HandleVideoTitleModifiedL( CMPXMedia*& aVideoInCache );
       
        /**
        * Deletes iPartialList.
        */ 
        void DeletePartialList();

        /**
        * Adds video to cache (iVideoList or iPartialVideoList). Ownership moves.
        *
        * @param aVideo             Video to add.
        * @param aSortingOrder      Sorting order.
        * @param aUpdateCategores   If ETrue, then category and album attributes are updated.
        * @return                   KErrNone if added, KErrAlreadyExists if already exists and
        *                           was not added.
        */
        TInt AddL( CMPXMedia* aVideo, TVcxMyVideosSortingOrder aSortingOrder,
                TBool aUpdateCategories = ETrue );
        
        /**
        * Adds video to iPartialVideoList. Ownership moves.
        *
        * @param aVideo  Video to add.
        * @return        KErrNone if item was added, KErrAlreadyExists if video
        *                was already on the list and was not added.
        */       
        TInt AddToPartialListL( CMPXMedia* aVideo );

        /**
        * Finds the array position of aVideo in iPartialVideoList .
        *
        * @param aVideo  Video which position is searched.
        * @return        Position, KErrNotFound if item is not in iPartialVideoList.
        */
        TInt PosOnPartialVideoListL( CMPXMedia& aVideo );

        /**
        * Finds the array position of aVideo in iVideoList.
        *
        * @param aVideo  Video which position is searched.
        * @return        Position, KErrNotFound if item is not in iVideoList.
        */
        TInt PosOnVideoListL( CMPXMedia& aVideo );

#ifdef _DEBUG
        /**
         * Checks that iVideoListIndex is correctly formed.
         */
        void CheckVideoListIndexL();
#endif
        
    public:            
        /**
        * Creates filtered video list from iVideoList. This is used for
        * showing video categories by origin.
        *
        * @param aOrigin Only videos with this origin are added to list,
        *                see values from TVcxMyVideosOrigin (in vcxmyvideosdefs.h).                
        * @return        New video list, containing videos from aOrigin.
        */
        CMPXMedia* CreateVideoListByOriginL( TUint8 aOrigin );

        /**
        * Appends items from aFromList to aToList if items origin is equal with
        * aOrigin. This is used when fetching other than All category from MDS.
        *
        * @param aToList    List to append
        * @param aFromList  List to append from.
        * @param aOrigin    Only items with this origin are copied.
        * @param aNewItemsStartIndex Start index in aFromList where from the copying is started.
        */
        void AppendToListL( CMPXMedia& aToList, CMPXMedia& aFromList,
                TUint8 aOrigin, TInt aNewItemsStartIndex );
                
        /**
        * Tries to find media with matching URI from the cached video list.
        *
        * @param aUri URI to compare
        * @return     Contains pointer to media if match found, NULL otherwise.
        *             Pointer ownership is not moved to the caller.
        */
        CMPXMedia* FindVideoByUriL( const TDesC& aUri );

        /**
        * Tries to find media with matching MdsId from iVideoList and iPartialVideoList.
        *
        * @param aMdsId ID to compare
        * @param aPos   If found from iVideoList, the position index is written here,
        *               KErrNotFound otherwise. If found from iPartialVideoList, then
        *               the value is still set to KErrNotFound.
        * @return       Contains pointer to media if match found, NULL otherwise.
        *               Pointer ownership is not moved to the caller.
        */
        CMPXMedia* FindVideoByMdsIdL( TUint32 aMdsId, TInt& aPos );
        
        /**
        * Gets medias from iVideoList and iPartialVideoList.
        *
        * @param aMdsIds  IDs of the items to fetch.
        * @return         Pointer to fetched items, KMPXMediaArrayContents
        *                 attribute contains the media items. Ownership
        *                 moves to caller.
        */
        CMPXMedia* GetVideosL( RArray<TUint32>& aMdsIds );

        /**
        * Removes video from iVideoList (or from iPartialVideoList).
        *
        * @param aMdsId             MDS ID of the video to be removed.
        * @param aUpdateCategories  If ETrue, then categories are updated.
        * @return                   KErrNotFound if not found, KErrNone otherwise.
        */
        TInt RemoveL( TUint32 aMdsId, TBool aUpdateCategories = ETrue );

        /**
        * Removes videos from iVideoList.
        *
        * @param aUpdateCategories  If ETrue, then categories are updated.
        * @param aMdsIds            Array containing MDS IDs of the videos to be deleted.
        */
        void RemoveL( RArray<TUint32>& aMdsIds, TBool aUpdateCategories = ETrue );

        /**
        * Adds videos to cache (iVideoList or iPartialVideoList). After the function call aMdsIds
        * will contain only those items which were actually added to cache.
        *
        * @param aMdsIds                   Array containing MDS IDs of the videos to be added.
        * @param aListFetchingWasCanceled  This is set to EFalse if video list fetching from mds
        *                                  had to be cancelled. EFalse otherwise. 
        * @param aNonVideoIds              If argument given then Ids which were detected to not be
        *                                  videos are written here. Caller owns the array, ownership
        *                                  does not move.
        * @param aUpdateCategories         If ETrue, then category/album attributes are updated.
        */
        void AddVideosFromMdsL( RArray<TUint32>& aMdsIds, TBool& aListFetchingWasCanceled,
                RArray<TUint32>* aNonVideoIds = NULL, TBool aUpdateCategories = ETrue );

        /**
        * Deletes old and creates new iVideoList. After the function call iVideoList exists,
        * but it might not contain any media items yet, only empty array.
        * Media items are added to the list asynchronoysly in
        * VcxMyVideosColletionPlugin::HandleCreateVideoListResp(), called by
        * VcxMyVideosMdsDb. Once iVideoList is complete, iVideoListIsPartial
        * is set to EFalse. During the video list fetching
        * VcxMyVideosMdsDb::iVideoListFetchingIsOngoing is ETrue.
        * If there is video list creation already ongoing
        * and the aSortingOrder is the same, nothing is done.
        * During the video list fetching iPartialVideoList items are moved to
        * iVideoList. When fetch is complete, iPartialVideoList is empty.
        *
        * @param aForce  If ETrue, then the list is always created from scratch, possible
        *                ongoing fetch is cancelled.
        */
        void CreateVideoListL( TBool aForce = EFalse );
    
        /**
        * Resets iVideoList (removes items, empty list is left) and cancels possible ongoing
        * asynchronous videolist fetching. iVideoListIsPartial is set to ETrue.
        * CVcxMyVideosCollection plugin variables are not touched. Caller is responsible to
        * either cancel the fetch from plugin, or restart fetch from mds in a way that
        * client gets its list. iVideoList media object remains pointing to same shared data.
        */
        void ResetVideoListL();
        
        /**
        * Updates video attributes in cache from aVideo. NOTE: iVideoList might be changed
        * if Size or Title values are modified and matching sorting order is currently on.
        * Pointers to iVideoList items should be refreshed after calling this with size and title
        * parameters.
        *
        * @param aVideo  Attributes are read from here and updated to cache if difference exists. 
        * @return        ETrue if any parameter was changed. 
        */
        TBool UpdateVideoL( CMPXMedia& aVideo );

        /**
        * iVideoList and its media array are deleted and recreated. New instances point to
        * different global heap position. Media objects stay the same and point to
        * same global heap.
        */
        void ReCreateVideoListL();

        /**
         * Replaces items in iVideoList with items from iPartialVideoList.
         * Called by iCollection when fetching videolist.
         */
        void CheckForPartialVideoListItemsL( TInt aNewItemsStartIndex );
        
    public:
    
        /**
        * Contains list of all videos in MDS. Used for caching MDS to RAM.
        */
        CMPXMedia* iVideoList;

        /**
        * Items fetched by MDS ID which don't (yet) have instance in iVideoList
        * are added here. When iVideoList instance is received from MDS, it is
        * replaced by item from iPartialVideoList and instance from iPartialVideoList
        * is removed.
        */
        RArray<CMPXMedia*> iPartialVideoList;
                                
        /**
        * Sorting order which was used last time when list was queryed from MDS.
        */
        TVcxMyVideosSortingOrder iLastSortingOrder;
        
        /**
         * Set to ETrue when doing videolist fetching.
         */
        TBool iIsFetchingVideoList;
    private:

        /**
        * If ETrue then iVideoList contains all items from MDS.
        */
        TBool iVideoListIsComplete;
        
        /**
        * My Videos collection plugin, owner of this object.
        */
        CVcxMyVideosCollectionPlugin& iCollection;
        
        /**
         * Index which keeps TVcxMyVidesVideo items indexed in MDS ID
         * order for fast access.
         */
        CVcxMyVideosVideoListIndex* iVideoListIndex;
        
        /**
        * Provides access to the sorting order key in cenrep. Own.
        */
        CRepository* iCenRep;

    };

#endif   // VCXMYVIDEOSVIDEOCACHE_H


