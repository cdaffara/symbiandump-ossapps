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
* Description:  Handles collection Open operation related functionality
*
*/


#ifndef VCXMYVIDEOSOPENHANDLER_H
#define VCXMYVIDEOSOPENHANDLER_H

class CVcxMyVideosCollectionPlugin;
class MMPXCollectionPluginObserver;
class CVcxMyVideosMdsDb;

#include "vcxmyvideosmdsdb.h"
/**
 */
NONSHARABLE_CLASS( CVcxMyVideosOpenHandler ) : public CBase, public MVcxMyVideosMdsAlbumsObserver
    {
    friend class CVcxMyVideosVideoCache;
    
public:
    
    /**
    * Two-phased constructor
    */
    static CVcxMyVideosOpenHandler* NewL( CVcxMyVideosCollectionPlugin& aCollection,
            CVcxMyVideosVideoCache& aCache,
            CVcxMyVideosMdsDb& aMds );

    /**
    * Destructor
    */
    virtual ~CVcxMyVideosOpenHandler();

public:

    /**
    * Handle collection Open operation.
    *
    * @param aPath  Path to open.
    */
    void OpenL( const CMPXCollectionPath& aPath );

    /**
     * From MVcxMyVideosMdsAlbumsObserver.
     */
    void HandleAlbumOpenL();

    /**
     * From MVcxMyVideosMdsAlbumsObserver, no implementation.
     */
    void HandleGetAlbumsResp( CMPXMedia* /*aAlbumList*/ ) {}

    /**
     * From MVcxMyVideosMdsAlbumsObserver, no implementation.
     */
    void HandleGetAlbumContentIdsResp( TUint32 /*aAlbumId*/,
            RArray<TVcxMyVideosAlbumVideo>& /*aContentArray*/ ) {}

    /**
     * From MVcxMyVideosMdsAlbumsObserver, no implementation.
     */
    void HandleAddVideosToAlbumResp( CMPXMedia* /*aCmd*/,
            RPointerArray<CMdEInstanceItem>& /*aItemArray*/ ) {}

    /**
     * From MVcxMyVideosMdsAlbumsObserver, no implementation.
     */
    void HandleRemoveRelationsResp( RArray<TUint32>& /*aRelationIds*/,
            RArray<TUint32>& /*aResults*/ ) {}

    /**
     * From MVcxMyVideosMdsAlbumsObserver, no implementation.
     */
    void HandleRemoveAlbumsResp( CMPXMedia* /*aCmd*/,
            RArray<TUint32>& /*aResultIds*/ ) {}

    /**
     * From MVcxMyVideosMdsAlbumsObserver, no implementation.
     */
    void HandleRelationEvent( TObserverNotificationType /*aType*/,
            const RArray<TMdERelation>& /*aRelationArray*/ ) {}
    
    /**
     * Called as a response to GetAlbumContentVideosL.
     * 
     * @param aAlbumId           ID of the album containing the result videos.
     * @param aVideoList         Media containing array of videos.
     * @param aError             Error code in case of failure.
     * @param aFirstNewItemIndex Index of the first new item.
     * @param aNewItemCount      How many new items in the query since the last results.
     * @param aComplete          ETrue if query is complete, EFalse if there is more to come.
     */
    void HandleGetAlbumContentVideosResp( TUint32 aAlbumId, CMPXMedia& aVideoList,
            TInt aError, TInt aFirstNewItemIndex, TInt aNewItemCount, TBool aComplete );


    void DoHandleCreateVideoListRespL(
            CMPXMedia* aVideoList, TInt aNewItemsStartIndex, TBool aComplete );

private:
    /**
    * 2nd-phase Constructor
    */
    void ConstructL();

    /**
    * Default Constructor
    */
    CVcxMyVideosOpenHandler( CVcxMyVideosCollectionPlugin& aCollection,
            CVcxMyVideosVideoCache& aCache,
            CVcxMyVideosMdsDb& aMds );

    /**
     * Opens category.
     * 
     * @param aCategoryId  Category to open.
     */
    void OpenCategoryL( TUint32 aCategoryId );

    /**
     * Leaving version of HandleGetAlbumContentVideosResp.
     */
    void HandleGetAlbumContentVideosRespL(
            TUint32 aAlbumId, CMPXMedia& /*aVideoList*/, TInt aError,
            TInt /*aFirstNewItemIndex*/, TInt /*aNewItemCount*/, TBool aComplete );
public:

    /**
     * If album is opened when the album list is not yet complete,
     * then the album ID which is tried to open is stored here.
     * The open processing is continued when album list is received
     * from MDS. There can't be several album opens pending since
     * we do not call HandleOpen before we get the album list.
     */
    TUint32 iPendingAlbumOpenId;

    /**
     * for testing, temp
     */
    CMPXMedia* iAlbumVideoList;

private: // data

    /**
    * Collection plugin main class. Owner of this object.
    */
    CVcxMyVideosCollectionPlugin& iCollection;
        
    /**
    * Videos cache, owned by iCollection. Reference is stored here
    * just to avoid one pointer access.
    */
    CVcxMyVideosVideoCache& iCache;
    
    /**
    * Object for accessing MDS database, owned by iCollection. Reference is stored here
    * just to avoid one pointer access.
    */
    CVcxMyVideosMdsDb& iMds;

    /**
    * The category ids being opened are stored here. These values are used
    * to filter list when items arrive from MDS. KVcxMvcCategoryIdAll category is not listed here.
    * This is in sync with iVideoListsBeingOpened.
    */
    RArray<TInt> iCategoryIdsBeingOpened;
    
    /**
    * Video lists for categories being opened are stored here.
    * This is in sync with iCategoryIdsBeingOpened.
    */
    RArray<CMPXMedia*> iVideoListsBeingOpened;

    /**
    * The album IDs being opened are stored here. These values are used
    * to filter list when items arrive from MDS.
    * This is in sync with iAlbumVideoListsBeingOpened.
    */
    RArray<TInt> iAlbumIdsBeingOpened;

    /**
    * Video lists for albums being opened are stored here.
    * This is in sync with iAlbumIdsBeingOpened.
    */
    RArray<CMPXMedia*> iAlbumVideoListsBeingOpened;
    
    /**
     * Path being opened is stored here for the HandleOpenL call.
     * Not Own.
     */
    const CMPXCollectionPath* iPath;
    
    };

#endif // VCXMYVIDEOSACTIVETASK_H
