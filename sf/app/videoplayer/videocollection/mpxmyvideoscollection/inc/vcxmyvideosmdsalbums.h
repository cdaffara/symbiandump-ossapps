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
* Description:   Provides albums support from MDS*
*/



#ifndef VCXMYVIDEOSMDSALBUMS_H
#define VCXMYVIDEOSMDSALBUMS_H

// INCLUDES
#include <mdequery.h>
#include <mpxmedia.h>
#include "vcxmyvideosalbum.h"
#include "vcxmyvideosmdsdb.h"

// FORWARD DECLARATIONS
class CMPXMedia;

// CONSTANTS

// CLASS DECLARATION

/**
* Provides albums support, uses MDS.
*
* @lib mpxmyvideoscollectionplugin.lib
*/
NONSHARABLE_CLASS(CVcxMyVideosMdsAlbums) :
        public CActive,
        public MMdEQueryObserver,
        public MMdERelationItemObserver
#if 0
        ,
        public MMdERelationObserver
#endif
    {    
    public: // Constructors and destructor
        
        friend class CVcxMyVideosMdsCmdQueue;
        friend class CVcxMyVideosMdsDb;
        
        enum TVcxAsyncOperation
            {
            EVcxNone,
            EVcxAddVideosToAlbum,
            EVcxRemoveRelations,
            EVcxRemoveAlbums
            };
        
        /**
        * Constructor.
        * 
        * @param aMdsDb    Owner of this object.
        * @param aObserver Object which is listening album related events. If NULL, then
        *                  no observer is set.
        */
        static CVcxMyVideosMdsAlbums* NewL( CVcxMyVideosMdsDb& aMdsDb,
                MVcxMyVideosMdsAlbumsObserver* aObserver );

        /**
        * Destructor
        */
        virtual ~CVcxMyVideosMdsAlbums();

    public: // new methods

        /**
         * From CActive.
         * Cancels possible ongoing asynchronous request.
         * 
         * @param aType Defines what kind of request is cancelled.
         */
        void DoCancel( CVcxMyVideosMdsDb::TRequestType aType = CVcxMyVideosMdsDb::EAll );

        /**
         * Cancels queries.
         *
         * @param aType Defines what kind of request is cancelled.
         */
        void CancelQueries( CVcxMyVideosMdsDb::TRequestType aType = CVcxMyVideosMdsDb::EAll );
        
        /**
        * Gets My Videos albums from MDS asynchronously.
        * HandleGetAlbumsResp() callback function is called when ready.
        * Utilizes CVcxMyVideosMdsCmdQueue.
        *
        * @param aAlbumList  Album list is written here. Caller owns this, ownership
        *                    does not move.
        * @param aClient     Response call is done to this object.
        */
        void GetAlbumsL( CMPXMedia* aAlbumList, MVcxMyVideosMdsAlbumsObserver& aClient );

        /**
         * Gets album from MDS synchronously.
         * 
         * @param aId  MDS ID of the album to be fetched.
         * @returm     MPX media containing album data. Ownership
         *             moves to caller. NULL if not found.
         */
        CMPXMedia* GetAlbumL( TUint32 aId );

        /**
         * Gets album content IDs asynchronously.
         * HandleGetAlbumContentIdsResp() callback function is called when ready.
         * Utilizes CVcxMyVideosMdsCmdQueue.
         *
         * @param aAlbumId      Album ID.
         * @param aContentArray Item IDs which belog to aAlbum are written here. Caller must
         *                      keep aContentArray available until HandleGetAlbumContentIdsResp
         *                      has been called.
        * @param aClient        Response call is done to this object.
         */
        void GetAlbumContentIdsL( TUint32 aAlbumId, RArray<TVcxMyVideosAlbumVideo>& aContentArray,
                MVcxMyVideosMdsAlbumsObserver& aClient );
        
        /**
         * Gets album content videos asynchronously.
         * HandleGetAlbumContentVideosResp() callback function is called when ready.
         * Utilizes CVcxMyVideosMdsCmdQueue.
         * 
         * @param aAlbumId    Album ID.
         * @param aVideoList  Media containing media array. Video media objects are added to media array.
         *                    Caller must keep aVideoList available until callback has been called.
         * @param aClient     Response call is done to this object.
         */
        void GetAlbumContentVideosL( TUint32 aAlbumId, CMPXMedia& aVideoList,
                MVcxMyVideosMdsAlbumsObserver& aClient );
        
        /**
         * Adds videos to album asynchronously. HandleAddVideosToAlbumResp() callback function
         * is called when operation finishes.
         * Utilizes CVcxMyVideosMdsCmdQueue.
         *
         * @param aMpxCmd    Media containing command parameters: album ID and video ID list.
         *                   Results are also written to this object. See media structure and
         *                   used attributes from vcxmyvideosdef.c file,
         *                   KVcxCommandMyVideosAddToAlbum command. Ownership does not move.
         *                   
         * @param aClient    Response call is done to this object.
         */
        void AddVideosToAlbumL( CMPXMedia* aMpxCmd, MVcxMyVideosMdsAlbumsObserver& aClient );
        
        /**
         * Removes relations asynchronously.
         * Utilizes CVcxMyVideosMdsCmdQueue.
         * 
         * @param aRelationIds  Relations which are removed.
         * @param aResults      In sync with aRelationIds. Result codes, KErrNone if successful,
         *                      KErrGeneral if failed.
         * @param aClient       Response call is done to this object.
         */
        void RemoveRelationsL( RArray<TUint32>& aRelationIds,
                RArray<TUint32>& aResults, MVcxMyVideosMdsAlbumsObserver& aClient );
 
        /**
         * Adds album to MDS. aAlbum should have KMPXMediaGeneralTitle attribute
         * set. The MDS item ID of the created album is written to KMPXMediaGeneralId
         * attribute.
         * 
         * @param aAlbum  Album which is added to database. 
         */
        void AddAlbumL( CMPXMedia& aAlbum );
        
        /**
         * Removes albums asynchronously. HandleRemoveAlbumsResp() callback function
         * is called when operation finishes.
         * Utilizes CVcxMyVideosMdsCmdQueue.
         *
         * @param aMpxCmd    Media containing command parameters: media array containing album IDs.
         *                   Results are also written to this object. See media structure and
         *                   used attributes from vcxmyvideosdef.c file,
         *                   KVcxCommandMyVideosRemoveAlbums command. Ownership does not move.
         *                   
         * @param aClient    Response call is done to this object.
         */        
        void RemoveAlbumsL( CMPXMedia* aMpxCmd, MVcxMyVideosMdsAlbumsObserver& aClient );
        
        /**
         * Sets album attributes.
         * 
         * @param aVideo  Values from aVideo are written to MDS.
         */
        void SetAlbumL( CMPXMedia& aVideo );
        
    protected:
        
        /**
        * From CActive.
        * Called when operation completes.
        */
        void RunL();

        /**
        * From CActive.
        * Actual implementation for cancelling.
        */
        void DoCancel();
        
    private:

        /**
        * Constructor
        */
        CVcxMyVideosMdsAlbums( CVcxMyVideosMdsDb& aMdsDb,
                MVcxMyVideosMdsAlbumsObserver* aObserver );

        /**
        * Symbian 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Copies album data from aObject to aAlbum.
        *
        * @param aObject  MDS object to copy data from.
        * @param aAlbum   MPX media to copy data to.
        */
        void Object2MediaL( CMdEObject& aObject, CMPXMedia& aAlbum );
        
        /**
         * Copies album data from aAlbum to aObject.
         * 
         * @param aAlbum MPX media to copy data from.
         * @param aObject MDS object to copy data to.
         */
        void Media2ObjectL( CMPXMedia& aAlbum, CMdEObject& aObject);

        /**
        * Gets various defintions from MDS and stores them to member variables.
        */
        void GetSchemaDefinitionsL();

        /**
        * Gets My Videos albums from MDS asynchronously.
        * HandleGetAlbumsResp() callback function is called when ready.
        *
        * @param aAlbumList  Album list is written here. Caller owns this, ownership
        *                    does not move.
        * @param aClient     Callback is done to this object.
        */
        void DoGetAlbumsL( CMPXMedia* aAlbumList, MVcxMyVideosMdsAlbumsObserver& aClient );

        /**
         * Gets album content IDs from MDS asynchronously.
         * HandleGetAlbumContentIdsResp() callback function is called when ready.
         *
         * @param aAlbumId      Album ID.
         * @param aContentArray Array containing MDS IDs which belong to aAlbum.
         *                      Caller is responsible to keep array available
         *                      until HandleGetAlbumContentResp has been called. 
        * @param aClient        Callback is done to this object.
         */
        void DoGetAlbumContentIdsL( TUint32 aAlbumId,
                RArray<TVcxMyVideosAlbumVideo>& aContentArray,
                MVcxMyVideosMdsAlbumsObserver& aClient );

        /**
         * Gets album content videos from MDS asynchronously.
         * HandleGetAlbumContentVideosResp() callback function is called when ready.
         * 
         * @param aAlbumId   Album ID.
         * @param aVideoList Media containing empty media array. Videos are added to array. Caller
         *                   must keep aVideoList available until HandleGetAlbumContentVideosResp
         *                   is called.
         * @param aClient    Callback is done to this object.
         */
        void DoGetAlbumContentVideosL( TUint32 aAlbumId, CMPXMedia& aVideoList,
                MVcxMyVideosMdsAlbumsObserver& aClient );

        /**
         * Adds videos to album asynchronously. HandleAddVideosToAlbumResp() callback function
         * is called when operation finishes.
         * 
         * @param aMpxCmd    Media containing command parameters: album ID and video ID list.
         *                   Results are also written to this object. See media structure and
         *                   used attributes from vcxmyvideosdef.c file,
         *                   KVcxCommandMyVideosAddToAlbum command. Ownership does not move.
         * @param aClient    Response call is done to this object.
         */
        void DoAddVideosToAlbumL( CMPXMedia* aMpxCmd, MVcxMyVideosMdsAlbumsObserver& aClient );
        
        /**
         * Removes relations asynchronously.
         * 
         * @param aRelationIds  Relations which are removed.
         * @param aResults      In sync with aRelationIds. Result codes, KErrNone if successful,
         *                      KErrGeneral if failed.
         * @param aClient       Response call is done to this object.
         */
        void DoRemoveRelationsL( RArray<TUint32>& aRelationIds,
                RArray<TUint32>& aResults, MVcxMyVideosMdsAlbumsObserver& aClient );
        
        /**
         * Removes albums asynchronously.
         * 
         * @param aMpxCmd  Media containing command parameters: array with albums IDs.
         * @param aClient  Response call is done to this object.
         */
        void DoRemoveAlbumsL( CMPXMedia* aMpxCmd,
                MVcxMyVideosMdsAlbumsObserver& aClient );

        /**
        * Handles album query responses.
        * 
        * @param aQuery  Query instance.
        * @param aError  <code>KErrNone</code>, if the query was completed
        *                successfully. Otherwise one of the system-wide error 
        *                codes.
        */
        void HandleAlbumQueryCompletedL(CMdEQuery& aQuery, TInt aError);

        /**
        * Handles video query responses.
        * 
        * @param aQuery  Query instance.
        * @param aError  <code>KErrNone</code>, if the query was completed
        *                successfully. Otherwise one of the system-wide error 
        *                codes.
        * @param aFirstNewItemIndex Index of the first new item in the query.
        * @param aNewItemCount      How many new items were added.
        * @param aComplete          ETrue if query is complete, EFalse if new to come.
        */
        void HandleVideoQueryResultsL( CMdEQuery& aQuery, TInt aError,
                TInt aFirstNewItemIndex, TInt aNewItemCount, TBool aComplete );

        /**
         * Handles relation query responses.
         * 
         * @param aQuery Query instance.
         * @param aError  <code>KErrNone</code>, if the query was completed
         *                successfully. Otherwise one of the system-wide error 
         *                codes.
         */
        void HandleRelationQueryCompletedL( CMdEQuery& aQuery, TInt aError );

        /**
         * Called from RunL when video adding to album completes.
         */
        void HandleAddVideosToAlbumCompletedL();

        /**
         * Called from RunL when relations remove completes.
         */
        void HandleRemoveRelationsCompletedL();

        /**
         * Called from RunL when albums remove completes.
         */
        void HandleRemoveAlbumsCompletedL();

        /**
         * Registers observing to MDS session.
         */
        void SetObservingL();
        
    protected:

        /**
        * From MMdEQueryObserver.
        * Called to notify the observer that new results have been received 
        * in the query.
        *
        * @param aQuery              Query instance that received new results.
        * @param aFirstNewItemIndex  Index of the first new item that was added
        *                            to the result item array.
        * @param aNewItemCount       Number of items added to the result item 
        *                            array.
        */
        void HandleQueryNewResults(CMdEQuery& aQuery,
                TInt aFirstNewItemIndex,
                TInt aNewItemCount);

        /**
        * From MMdEQueryObserver.
        * Called to notify the observer that the query has been completed,
        * or that an error has occured.
        *
        * @param aQuery  Query instance.
        * @param aError  <code>KErrNone</code>, if the query was completed
        *                successfully. Otherwise one of the system-wide error 
        *                codes.
        */
        void HandleQueryCompleted(CMdEQuery& aQuery, TInt aError);

        /**
         * From MMdERelationItemObserver
         */
        void HandleRelationItemNotification(CMdESession& aSession, 
                    TObserverNotificationType aType,
                    const RArray<TMdERelation>& aRelationArray);

        /**
         * From MMdERelationObserver
         */
        void HandleRelationNotification(CMdESession& aSession, 
                TObserverNotificationType aType,
                const RArray<TItemId>& aRelationIdArray);
        
    private:
        
        /**
         * Main class for Mds operations. Owns session to MDS.
         */
        CVcxMyVideosMdsDb& iMdsDb;
        
        /**
         * Observer of albums related events. May be NULL.
         * not own.
         */
        MVcxMyVideosMdsAlbumsObserver* iObserver;
        
        /**
        * Asynchronous album object fetching query is stored here. Own.
        */
        CMdEObjectQuery* iAlbumQuery;

        /**
        * Asynchronous video object fetching query is stored here. Own.
        */
        CMdEObjectQuery* iVideoQuery;

        /**
        * Asynchronous relation fetching query is stored here. Own.
        */
        CMdERelationQuery* iRelationQuery;
                
        /**
        * The default namespace definition, not own.
        */
        CMdENamespaceDef* iNamespaceDef;
        
        /**
        * Album object definition, not own.
        */
        CMdEObjectDef* iAlbumObjectDef;
        
        /**
        * Album type property definition, not own.
        */
        CMdEPropertyDef* iTypePropertyDef;
        
        /**
         * "Contains" relation definition, not own.
         */
        CMdERelationDef* iContainsRelationDef;
        
        /**
        * Pointer to album list which is being fetched from MDS. Not own.
        */
        CMPXMedia* iAlbumList;

        /**
        * Pointer to video list which is being fetched from MDS. Not own.
        */
        CMPXMedia* iVideoList;

        /**
         * When doing some async operation to MDS, the album ID is stored here.
         */
        TUint32 iAlbumId;
        
        /**
         * When fetching album content from MDS, the pointer to content array is stored here.
         * Only MDS ID is filled to TVideo.
         * Not own.
         */
        RArray<TVcxMyVideosAlbumVideo>* iAlbumContent;

        /**
         * The pending async operation ID is stored here.
         */
        TVcxAsyncOperation iAsyncOperation;

        /**
         * Used for storing items during async mds operations.
         */
        RPointerArray<CMdEInstanceItem> iItemArray;
        
        /**
         * Used for storing result buffer during async mds operations.
         */
        RMdEDataBuffer iResultBuffer;

        /**
         * Used in async operations to store
         * pointer to result array (which is owned by the user). 
         */
        RArray<TInt>* iResultArray;

        /**
         * Used in async operations to store
         * pointer to result array (which is owned by the user). 
         */
        RArray<TUint32>* iResultArrayUint32;
        
        /**
         * Used in async oprations (EVcxAddVideosToAlbum and EVcxRemoveVideosFromAlbum) to store
         * pointer to item ID array (which is owned by the user). 
         */
        RArray<TUint32>* iIdArray;
                
        /**
         * Used to store command parameters during async MDS operations. Not owned.
         */
        CMPXMedia* iMpxCmd;
        
        /**
         * Used to store pointer to client who initiated the async operation.
         */
        MVcxMyVideosMdsAlbumsObserver* iClient;

    };

#endif   // VCXMYVIDEOSMDSALBUMS_H


