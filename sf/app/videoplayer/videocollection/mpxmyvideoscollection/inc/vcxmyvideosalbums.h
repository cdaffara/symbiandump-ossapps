/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Albums related collection functionality.
*/



#ifndef VCXMYVIDEOSALBUMS_H
#define VCXMYVIDEOSALBUMS_H

// INCLUDES
#include <mdequery.h>
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideosalbum.h"

// FORWARD DECLARATIONS
class CVcxMyVideosMdsDb;
class CMPXMedia;
class CVcxMyVideosCollectionPlugin;
class CVcxMyVideosAlbum;

// CONSTANTS

// CLASS DECLARATION

/**
* Albums related collection functionality.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosAlbums) : public CBase, public MVcxMyVideosMdsAlbumsObserver
    {    
    public: // Constructors and destructor
        
        /**
        * Two-phased constructor
        * @return object constructed
        */
        static CVcxMyVideosAlbums* NewL( CVcxMyVideosCollectionPlugin& aCollectionPlugin );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosAlbums();

    public: // new methods

        /**
         * Returns album from iAlbums array by MDS ID. Ownership does not move.
         * 
         * @param aMdsId  MDS ID of the album to get.
         * @param aPos    If given, then the position index in iAlbums is written here.
         *                Ownership does not move.
         * @return        Pointer to album or NULL if not found.
         */
        CVcxMyVideosAlbum* Album( TUint32 aMdsId, TInt* aPos = NULL );
        
        /**
        * Creates iAlbumList if it doesn't exist yet. The list is populated
        * from async callback, HandleGetAlbumsResp().
        */
        void CreateAlbumListL();
                
        /**
         * Fetches MDS IDs from MDS for all videos belonging to albums.
         * Ie fills iAlbums[*]->iVideoList[*].iMdsId (and iAlbums[*]->iVideoList[*].iRelationMdsId) fields.
         * Causes several async calls to MDS. 
         */
        void GetAlbumContentIdsL();

        /**
         * Adds videos to album.
         * 
         * @param aCmd  See attribute usage from vcxmyvideosdefs.h.
         */
        void AddVideosToAlbumL( CMPXMedia* aCmd );

        /**
         * Removes videos from album.
         * 
         * @param aCmd  See attribute usage from vcxmyvideosdefs.h.
         */
        void RemoveVideosFromAlbumL( CMPXMedia* aCmd );

        /**
         * Adds album to MDS and to this class. KMPXMediaGeneralTitle
         * should be set in aCmd. Upon completion, the KMPXMediaGeneralId
         * attribute contains the new MPX item ID. Leaves if could
         * not add to MDS.
         * 
         * @param aCmd  Command object received from collection client.
         */
        void AddAlbumL( CMPXMedia& aCmd );
        
        /**
         * Removes albums from MDS. MDS delete events will clean up albums
         * from this class.
         * 
         * @param aCmd  MPX command received from the client. Contains media array,
         *              which contains medias with album IDs set to KMPXMediaGeneralId.
         *              Ownership does not move. 
         */
        void RemoveAlbumsFromMdsOnlyL( CMPXMedia* aCmd );
        
        /**
         * Removes albums from this object (iAlbumList and iAlbums).
         * Called from MDS delete event. Album IDs which were removed are
         * also removed from aAlbumIds array. After the call aAlbumIds
         * will contain items which were not found and not deleted.
         * Sends appropriate events to collection client.
         * 
         * @param aAlbumIds  Album IDs to be removed.
         */
        void RemoveAlbumsL( RArray<TUint32>& aAlbumIds );

        /**
         * Removes album from this class (iAlbumList and iAlbums).
         * Adds event to message list but does not send it.
         * 
         * @param aMdsIds    Album MDS ID to be removed.
         * @param aCompress  If ETrue, then iAlbums is compressed,
         *                   otherwise not.
         * @return           ETrue if album was found and removed.
         */
        TBool RemoveAlbumL( TUint32 aMdsId, TBool aCompress );
        
        /**
         * Fetches albums from MDS. This is called from MDS insert event.
         * 
         * @param aAlbumIds  Album IDs to fetch.
         */
        void AddAlbumsFromMdsL( RArray<TUint32>& aAlbumIds );

        /**
         * Updates albums from MDS. This is called from MDS modify event.
         * 
         * @param aAlbumIds  Album IDs to update.
         */
        void UpdateAlbumsFromMdsL( RArray<TUint32>& aAlbumIds );

        /**
         * Updates album attributes, does not write to MDS. Adds
         * modify event to iCollection.iMessageList if necessarry.
         * 
         * @param aAlbum  New values are read from this.
         * @return        ETrue if album was modified, EFalse otherwise.
         */
        TBool UpdateAlbumL( const CMPXMedia& aAlbum );

        /**
         * Calculates and updates album media attributes.
         * Adds events to iCollection.iMessageList if albums
         * are modified. Does not send them.
         * 
         * @return  Returns ETrue if events were added to iCollection.iMessageList,
         *          EFalse otherwise.
         */
        TBool CalculateAttributesL();
        
        /**
         * Called when video's New Video flag changes. Calculates
         * album attributes for the affected albums. Adds events
         * to iCollection.iMessageList but does not send them.
         * 
         * @param aMdsId  MDS ID of the changed video.
         */
        void NewVideoFlagChangedL( TUint32 aMdsId );

        /**
         * Called when video's title changes. Calculates
         * album attributes for the affected albums. Adds events
         * to iCollection.iMessageList but does not send them.
         * 
         * @param aMdsId  MDS ID of the changed video.
         */
        void VideoTitleChangedL( TUint32 aMdsId );

        /**
         * Called when multiple videos are added or removed from cache. Adds changed
         * album indexes to iChangedAlbums.
         */
        void VideosAddedOrRemovedFromCacheL( RArray<TUint32> aMdsIds );
        
        /**
         * Called when video is added or removed from cache. Adds changed
         * album indexes to iChangedAlbums.
         */
        void VideoAddedOrRemovedFromCacheL( CMPXMedia& video );

        /**
         * Goes through iChangedAlbums and updates album attributes and adds
         * changed events.
         */
        void UpdateChangedAlbumsL();

protected:
        
        /**
         * From MVcxMyVideosMdsAlbumsObserver.
         * Process albums arriving from MDS. Response to VcxMyVideosMdsAlbums::GetAlbumsL.
         */
        void HandleGetAlbumsRespL( CMPXMedia* aAlbumList );

        /**
         * From MVcxMyVideosMdsAlbumsObserver.
         * Process content ids arriving from MDS. Response to VcxMyVideosMdsAlbums::GetAlbumContentIdsL.
         * 
         * @param aAlbumId      ID of the album.
         * @param aAlbumContent Array containing MDS object IDs. This array is the same which
         *                      was given in GetAlbumContentIdsL call.
         */
        void HandleGetAlbumContentIdsRespL( TUint32 aAlbumId,
                RArray<TVcxMyVideosAlbumVideo>& aAlbumContentIds );
        
        /**
        * From MVcxMyVideosMdsAlbumsObserver.
        * Response to VcxMyVideosMdsAlbums::GetAlbumsL.
        * Sends insert events to collection clients.
        * 
        * @param aAlbumList  Pointer to same array which was given in GetAlbums call.
        */
        void HandleGetAlbumsResp( CMPXMedia* aAlbumList );
 
        /**
         * From MVcxMyVideosMdsAlbumsObserver.
         * Process content ids arriving from MDS. Response to CVcxMyVideosMdsAlbums::GetAlbumContentIdsL.
         * 
         * @param aAlbumId      ID of the album.
         * @param aAlbumContent Array containing MDS object IDs. This array is the same which
         *                      was given in GetAlbumContentL call.
         */
        void HandleGetAlbumContentIdsResp( TUint32 aAlbumId,
                RArray<TVcxMyVideosAlbumVideo>& aAlbumContentIds );

        /**
         * From MVcxMyVideosMdsAlbumsObserver.
         * Process content videos arriving from MDS. Response to CVcxMyVideosMdsAlbums::GetAlbumContentVideosL.
         * 
         * @param aAlbumId    Album ID.
         * @param aVideoList  Media containing media array, array items are videos.
         * @param aError      Error code in case of failure.
         * @param aFirstNewItemIndex The index of the first new item.
         * @param aNewItemCount      How many new items since the last results.
         * @param aComplete          ETrue is query is complete, EFalse if there is new to come.
         */
        void HandleGetAlbumContentVideosResp( TUint32 /*aAlbumId*/, CMPXMedia& /*aVideoList*/,
                TInt /*aError*/, TInt /*aFirstNewItemIndex*/, TInt /*aNewItemCount*/, TBool /*aComplete*/ ) {}
        
        /**
         * From MVcxMyVideosMdsAlbumsObserver.
         * Response to CVcxMyVideosMdsAlbums::AddVideosToAlbumL.
         * 
         * @param aCmd  Pointer to object which was given in AddVideosToAlbumL
         *              function call. Ownership does not move.
         * @param aItemArray  Item array which was tried to add to MDS.
         */        
        void HandleAddVideosToAlbumResp( CMPXMedia* aCmd,
                RPointerArray<CMdEInstanceItem>& aItemArray );

        /**
         * From MVcxMyVideosMdsAlbumsObserver.
         * Response to CVcxMyVideosMdsAlbums::RemoveRelationsL.
         */
        void HandleRemoveRelationsResp( RArray<TUint32>& aRelationIds,
                RArray<TUint32>& aResults );

        /**
         * From MVcxMyVideosMdsAlbumsObserver.
         * Response to CVcxMyVideosMdsAlbums::RemoveAlbumsL.
         */
        void HandleRemoveAlbumsResp( CMPXMedia* aCmd,
                RArray<TUint32>& aResultIds );

        /**
         * From MVcxMyVideosMdsAlbumsObserver.
         * Relation events from MDS. This object is set as an observer at
         * iCollection.ConstructL, CVcxMyVideosMdsDb::NewL(..,aAlbumsObserver,..).
         */
        void HandleRelationEvent( TObserverNotificationType aType,
                const RArray<TMdERelation>& aRelationArray );
        
    private:

        /**
         * Fills in iAlbums array. This is called when iAlbumList is ready.
         * Pointers to CVcxMyVideosVideoCache::iVideoList are not filled
         * in yet.
         */
        void CreateAlbumsL();

        /**
        * Constructor
        */
        CVcxMyVideosAlbums( CVcxMyVideosCollectionPlugin& aCollectionPlugin );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
         * Leaving version of HandleAddVideosToAlbumResp.
         */
        void DoHandleAddVideosToAlbumRespL( CMPXMedia* aCmd,
                RPointerArray<CMdEInstanceItem>& aItemArray );

    public:

        /**
        * Album list. This is given to client when categorylevel items are requested. Own.
        * If this is destroyed/edited, then pointers have to be updated in iAlbums.
        */
        CMPXMedia* iAlbumList;
        
        /**
         * Array containing MDS IDs and pointers to album and video media items.
         * Album media items are in iAlbumList and video media items are in
         * CVcxMyVideosVideoCache::iVideoList.
         */
        RArray<CVcxMyVideosAlbum*> iAlbums;
    
        /**
         * ETrue when iAlbumList is ready and iAlbums contains video IDs.
         */
        TBool iAlbumListIsComplete;
        
    private:
        
        /**
         * Owner of this object.
         */
        CVcxMyVideosCollectionPlugin& iCollection;
        
        /**
         * Used in MDS operations to store MDS video (or relation) IDs.
         */
        RArray<TUint32> iMdsOpTargetIds;
        
        /**
         * Stores album video data during relation deletion. This is to
         * avoid second search when resp arrives.
         */
        RArray<TVcxMyVideosAlbumVideo> iRemoveFromAlbumVideos;
        
        /**
         * Used in MDS operations to store operation results. Is in sync with iMdsOpTargetIds. 
         */
        RArray<TInt> iMdsOpResults;

        /**
         * Used in MDS operations to store operation results. Is in sync with iMdsOpTargetIds. 
         */
        RArray<TUint32> iMdsOpResultsUint32;
        
        /**
         * When videos are removed or added from cache, the indexes of the
         * changed albums are stored here. Reason is to calculate new attributes
         * only once per changed album.
         */
        RArray<TInt> iChangedAlbums;
        
    };

#endif   // VCXMYVIDEOSALBUMS_H


