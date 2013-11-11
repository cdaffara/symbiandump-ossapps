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
* Description:  Handles collection Open operation related functionality.
*
*/


// INCLUDE FILES
#include <e32base.h>
#include <mpxlog.h>
#include <mpxcollectionplugin.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>

#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideosopenhandler.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideosmdsdb.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosalbums.h"
#include "vcxmyvideosmdsalbums.h"

// ============================ MEMBER FUNCTIONS =============================

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler::CVcxMyVideosOpenHandler( CVcxMyVideosCollectionPlugin& aCollection,
        CVcxMyVideosVideoCache& aCache,
        CVcxMyVideosMdsDb& aMds )
: iCollection( aCollection ), iCache( aCache ), iMds( aMds )
    {
    }

// ---------------------------------------------------------------------------
// 2nd-phase constructor
// ---------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::ConstructL()
    {
    iCategoryIdsBeingOpened.Reset();
    iVideoListsBeingOpened.Reset();
    iAlbumIdsBeingOpened.Reset();
    iAlbumVideoListsBeingOpened.Reset();
    }

// ---------------------------------------------------------------------------
// Two-Phase Constructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler* CVcxMyVideosOpenHandler::NewL( CVcxMyVideosCollectionPlugin& aCollection,
        CVcxMyVideosVideoCache& aCache,
        CVcxMyVideosMdsDb& aMds )
    {
    CVcxMyVideosOpenHandler* self = new(ELeave) CVcxMyVideosOpenHandler( aCollection,
            aCache, aMds );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CVcxMyVideosOpenHandler::~CVcxMyVideosOpenHandler()
    {
    iCategoryIdsBeingOpened.Close();
    
    TInt count = iVideoListsBeingOpened.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        delete iVideoListsBeingOpened[i];
        iVideoListsBeingOpened[i] = NULL; 
        }
    iVideoListsBeingOpened.Close();

    iAlbumIdsBeingOpened.Close();
    
    count = iAlbumVideoListsBeingOpened.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        delete iAlbumVideoListsBeingOpened[i];
        iAlbumVideoListsBeingOpened[i] = NULL; 
        }
    iAlbumVideoListsBeingOpened.Close();    
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::OpenL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::OpenL(
    const CMPXCollectionPath& aPath )
    {
    MPX_FUNC("CVcxMyVideosOpenHandler::OpenL");

    MPX_DEBUG1("CVcxMyVideosOpenHandler:: path before open:");
    MPX_DEBUG_PATH( aPath );

    iPath = &aPath;
    
    const TInt categoryLevel = 1;
    const TInt videosLevel   = 2;
    const TInt playlistLevel = 3;
    
    switch ( aPath.Levels() )
        {
        case categoryLevel:
            {
            //we are at root level, return main level items (categories + albums)

#ifdef VCX_ALBUMS             
            iCollection.AlbumsL().CreateAlbumListL(); //async
#endif

            // This is needed to update category counters.
            // Goes to queue if CreateAlbumListL command goes to execution
            TRAPD( err, iCache.CreateVideoListL() );
            
            if ( err != KErrNone )
                {
                MPX_DEBUG2("iCache->CreateVideoListL() left: %d. Returning categories and albums anyway.", err);
                }
            
            CMPXMedia* itemList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
            CleanupStack::PushL( itemList );
            
            itemList->SetTObjectValueL<TBool>( KVcxMediaMyVideosVideoListIsPartial,
                !iCache.IsComplete() );
                
            TVcxMyVideosCollectionUtil::AppendToListL( *itemList, *iCollection.CategoriesL().iList );
#ifdef VCX_ALBUMS
            TVcxMyVideosCollectionUtil::AppendToListL( *itemList, *iCollection.AlbumsL().iAlbumList );
#endif
            itemList->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
            iCollection.iObs->HandleOpen( itemList, KErrNone );
            CleanupStack::PopAndDestroy( itemList );
            MPX_DEBUG1("CVcxMyVideosOpenHandler:: called HandleOpen for categories + albums");
            }
            break;

        case videosLevel:
            {
            //we are at second level, return video list from some category or album

#ifdef VCX_ALBUMS
            iCollection.AlbumsL().CreateAlbumListL(); //async
#endif

            TMPXItemId categoryId( aPath.Id() );
            
            if ( ( categoryId.iId2 == KVcxMvcMediaTypeCategory ) ||
                    ( categoryId.iId2 == KVcxMvcMediaTypeVideo ) && 
                 (categoryId.iId1 == KVcxMvcCategoryIdAll ||
                 categoryId.iId1 == KVcxMvcCategoryIdDownloads ||
                 categoryId.iId1 == KVcxMvcCategoryIdTvRecordings ||
                 categoryId.iId1 == KVcxMvcCategoryIdCaptured ||
                 categoryId.iId1 == KVcxMvcCategoryIdOther) )
                {
                MPX_DEBUG2("CVcxMyVideosOpenHandler:: opening category %d", categoryId.iId1 );
                OpenCategoryL( categoryId.iId1 );
                return;
                }
            else
                {
                MPX_DEBUG2("CVcxMyVideosOpenHandler:: opening album %d", categoryId.iId1 );
                
                iPendingAlbumOpenId = categoryId.iId1;
                if ( iCollection.AlbumsL().iAlbumListIsComplete ) 
                    {
                    HandleAlbumOpenL(); // iPendingAlbumOpenId goes to 0
                    return;
                    }
                else
                    {
                    // We will continue at HandleAlbumOpenL when album list arrives, iPendingAlbumOpenId is left != 0
                    return;
                    }
                }            
            }

        case playlistLevel:
			{
			iCollection.iObs->HandleOpen(const_cast<CMPXCollectionPath*> (&aPath), KErrNone);
			}
			break;

        default:
            break;

        } //switch
    }

// ---------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::OpenCategoryL
// ---------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::OpenCategoryL( TUint32 aCategoryId )
    {
    TUint8 origin = static_cast<TUint8>( TVcxMyVideosCollectionUtil::Origin( aCategoryId ) );

    iCache.CreateVideoListL(); // Causes async call to MDS, callbacks to DoHandleCreateVideoListRespL will follow.
                               // If iCache.iVideoList is complete and can be used (correct sorting order),
                               // then nothing is done.

    if ( iCache.IsComplete() )
        {
        MPX_DEBUG1("CVcxMyVideosOpenHandler:: videolist complete");

        // iCache.iVideoList is complete
        if ( aCategoryId == KVcxMvcCategoryIdAll )
            {
            MPX_DEBUG1("CVcxMyVideosOpenHandler:: KVcxMvcCategoryIdAll: calling HandleOpen(iCache.iVideoList)");
            iCache.iVideoList->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
            iCache.iVideoList->SetTObjectValueL<TInt>( KVcxMediaMyVideosInt32Value,
                    EVcxMyVideosVideoListComplete );
            iCollection.iObs->HandleOpen( iCache.iVideoList, KErrNone );                    
            }
        else
            {
            MPX_DEBUG1("CVcxMyVideosOpenHandler:: other than KVcxMvcCategoryIdAll: creating new category video list");
            CMPXMedia* videoList = iCache.CreateVideoListByOriginL( origin );
            MPX_DEBUG1("CVcxMyVideosOpenHandler:: calling HandleOpen(new list)");
            videoList->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
            videoList->SetTObjectValueL<TInt>( KVcxMediaMyVideosInt32Value,
                    EVcxMyVideosVideoListComplete );
            iCollection.iObs->HandleOpen( videoList, KErrNone );
            delete videoList;
            }
        iCollection.iMessageList->AddEventL( KVcxMessageMyVideosListComplete );
        iCollection.iMessageList->SendL();

        // No append events will arrive anymore -> we create our own version of the
        // video list.
        iCache.ReCreateVideoListL();
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosOpenHandler:: video list incomplete");
        // iCache.iVideoList is incomplete                

        if ( aCategoryId == KVcxMvcCategoryIdAll )
            {
            MPX_DEBUG1("CVcxMyVideosOpenHandler:: KVcxMvcCategoryIdAll, calling HandleOpen");
            iCache.iVideoList->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
            iCollection.iObs->HandleOpen( iCache.iVideoList, KErrNone  );
            }
        else
            {   
            MPX_DEBUG1("CVcxMyVideosOpenHandler:: other than KVcxMvcCategoryIdAll");

            TInt pos = iCategoryIdsBeingOpened.Find( aCategoryId );
            if ( pos == KErrNotFound )
                {
                MPX_DEBUG1("CVcxMyVideosOpenHandler:: category was not opened yet, creating list for it");
                iCategoryIdsBeingOpened.AppendL( aCategoryId ); 
                CMPXMedia* videoList = iCache.CreateVideoListByOriginL( origin );
                CleanupStack::PushL( videoList );
                iVideoListsBeingOpened.AppendL( videoList );
                CleanupStack::Pop( videoList );
                MPX_DEBUG1("CVcxMyVideosOpenHandler:: calling HandleOpen");
                videoList->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
                iCollection.iObs->HandleOpen( videoList, KErrNone  );
                }
            else
                {
                MPX_DEBUG1("CVcxMyVideosOpenHandler:: category was already being opened, calling HandleOpen with that");                        
                iVideoListsBeingOpened[pos]->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
                iCollection.iObs->HandleOpen( iVideoListsBeingOpened[pos], KErrNone  );
                }
            }
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::DoHandleCreateVideoListRespL
// New items fetched from MDS. iCollection.iCache->iVideoList = &aVideoList.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::DoHandleCreateVideoListRespL(
        CMPXMedia* aVideoList, TInt aNewItemsStartIndex, TBool aComplete )
    {    
    MPX_FUNC("CVcxMyVideosOpenHandler::DoHandleCreateVideoListRespL()");

    MPX_DEBUG2("CVcxMyVideosOpenHandler:: aComplete = %d", aComplete);
    
    if ( aNewItemsStartIndex != KErrNotFound )
        {
        //iCache.iVideoList now contains new items, tell iCache to check if partial list contains the new items
        iCache.CheckForPartialVideoListItemsL( aNewItemsStartIndex );
        }

    // Append new items to category video lists. All category is using iCache.iVideoList, thus no need to append to it.
    TInt categoryCount = iCategoryIdsBeingOpened.Count();
    for ( TInt i = 0; i < categoryCount; i++ )
        {
        iCache.AppendToListL( *iVideoListsBeingOpened[i], *aVideoList,
                TVcxMyVideosCollectionUtil::Origin( iCategoryIdsBeingOpened[i] ),
                aNewItemsStartIndex );
        }

    // Append new items to album video lists.
    TInt albumCount = iAlbumVideoListsBeingOpened.Count();
    for ( TInt i = 0; i < albumCount; i++ )
        {
        iCollection.iAlbums->Album( iAlbumIdsBeingOpened[i] )->AppendToVideoListL(
                *aVideoList, *(iAlbumVideoListsBeingOpened[i]), aNewItemsStartIndex );
        }

    if ( aComplete == 0 )
        {
        iCollection.iMessageList->AddEventL( KVcxMessageMyVideosItemsAppended );

        // Still fetching items
        iCollection.CategoriesL().UpdateCategoriesL( *aVideoList, aNewItemsStartIndex );
        }                           
    else
        {
        // End event arrived
        
        iCache.SetComplete( ETrue );
        iCache.iIsFetchingVideoList = EFalse;
        

        // Create modify event for All category.
        // This will cause client to make a new OpenL() call.
        // Also this causes collection framework to purge its possibly outdated
        // (KVcxMessageMyVideosItemsAppended events don't update collection framework cache) cache.
        MPX_DEBUG3("CVcxMyVideosOpenHandler:: adding modify event for category[%d], extra info = %d",
                KVcxMvcCategoryIdAll, EVcxMyVideosVideoListOrderChanged );
        iCollection.iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdAll, 1 ), EMPXItemModified,
                EVcxMyVideosVideoListOrderChanged );
        // We dont send here, the send is at the end of this function.

        for ( TInt i = 0; i < categoryCount; i++ )
            {
            // Create modify event for other than All categories.
            // This will cause client to make a new OpenL() call.
            // Also this causes collection framework to purge its possibly outdated
            // (KVcxMessageMyVideosItemsAppended events don't update collection frameworks cache) cache.
            MPX_DEBUG3("CVcxMyVideosOpenHandler:: adding modify event for category[%d], extra info = %d",
                    iCategoryIdsBeingOpened[i], EVcxMyVideosVideoListOrderChanged );
            iCollection.iMessageList->AddEventL( TMPXItemId( iCategoryIdsBeingOpened[i], 1 ), EMPXItemModified,
                    EVcxMyVideosVideoListOrderChanged );
            // We dont send here, the send is at the end of this function.
            delete iVideoListsBeingOpened[i]; // we can delete our copy, client has its own copy
            iVideoListsBeingOpened[i] = NULL;
            }
        iVideoListsBeingOpened.Reset();
        iCategoryIdsBeingOpened.Reset();

        for ( TInt i = 0; i < albumCount; i++ )
            {
            // Create modify event for albums.
            // This will cause client to make a new OpenL() call.
            // Also this causes collection framework to purge its possibly outdated
            // (KVcxMessageMyVideosItemsAppended events don't update collection frameworks cache) cache.
            MPX_DEBUG3("CVcxMyVideosOpenHandler:: adding modify event for album %d, extra info = %d",
                    iAlbumIdsBeingOpened[i], EVcxMyVideosVideoListOrderChanged );
            iCollection.iMessageList->AddEventL(
                    TMPXItemId( iAlbumIdsBeingOpened[i], KVcxMvcMediaTypeAlbum ),
                    EMPXItemModified, EVcxMyVideosVideoListOrderChanged );
            // We dont send here, the send is at the end of this function.
            delete iAlbumVideoListsBeingOpened[i]; // we can delete our copy, client has its own copy
            iAlbumVideoListsBeingOpened[i] = NULL;
            }
        iAlbumVideoListsBeingOpened.Reset();
        iAlbumIdsBeingOpened.Reset();
        
        // All category, lets delete our copy and start using new one, this gives the full
        // ownership of the list to client. Video objects will continue to point to the same shared data.
        iCache.ReCreateVideoListL();

        iCollection.CategoriesL().UpdateCategoriesNewVideoNamesL();

        iCollection.AlbumsL().CalculateAttributesL(); // adds events if attributes modified, does not send
        
        iCollection.iMessageList->AddEventL( KVcxMessageMyVideosListComplete );
        }
    iCollection.iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::HandleAlbumOpenL
// Album list has been fetched already.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::HandleAlbumOpenL()
    {
    MPX_DEBUG1("CVcxMyVideosOpenHandler::HandleAlbumOpenL() start");
    
    CVcxMyVideosAlbum* album = iCollection.AlbumsL().Album( iPendingAlbumOpenId );
    if ( album )
        {
        if ( iCollection.iCache->IsComplete() )
            {
            //videolist complete
            CMPXMedia* videoList = album->CreateVideoListL();
            CleanupStack::PushL( videoList ); // 1->
            videoList->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
            videoList->SetTObjectValueL<TInt>( KVcxMediaMyVideosInt32Value,
                    EVcxMyVideosVideoListComplete ); 
            iCollection.iObs->HandleOpen( videoList, KErrNone );
            CleanupStack::PopAndDestroy( videoList ); // <-1
            iCollection.iMessageList->AddEventL( KVcxMessageMyVideosListComplete );
            iCollection.iMessageList->SendL();
            }
        else
            {
            if ( iCollection.iCache->iIsFetchingVideoList )
                {
                MPX_DEBUG1("CVcxMyVideosOpenHandler:: videolist fetching already in progress -> use that to populate album");

                // videolist incomplete and fetching already going on
                TInt pos = iAlbumIdsBeingOpened.Find( iPendingAlbumOpenId );
                if ( pos == KErrNotFound )
                    {
                    //store video list for append events
                    CMPXMedia* videoList = album->CreateVideoListL();
                    CleanupStack::PushL( videoList );
                    iAlbumVideoListsBeingOpened.AppendL( videoList );
                    CleanupStack::Pop( videoList );
                    iAlbumIdsBeingOpened.AppendL( iPendingAlbumOpenId );
                    videoList->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
                    iCollection.iObs->HandleOpen( videoList, KErrNone );
                    }
                else
                    {
                    //video list already stored
                    iAlbumVideoListsBeingOpened[pos]->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
                    iCollection.iObs->HandleOpen( iAlbumVideoListsBeingOpened[pos],
                            KErrNone );
                    }
                }
            else
                {
                MPX_DEBUG1("CVcxMyVideosOpenHandler:: videolist partial and no fetching going on -> fetch the album videos separately");

                delete iAlbumVideoList;
                iAlbumVideoList = NULL;
                iAlbumVideoList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
                iCollection.iMyVideosMdsDb->iAlbums->GetAlbumContentVideosL( iPendingAlbumOpenId,
                        *iAlbumVideoList, *this );
                return;
                }
            }
        }
    else
        {
        MPX_DEBUG2("CVcxMyVideosOpenHandler:: album ID not valid (%d) -> calling HandleOpen with KErrNotFound",
                iPendingAlbumOpenId);
        iCollection.iObs->HandleOpen( static_cast<CMPXMedia*>(NULL), KErrNotFound );
        }
    iPendingAlbumOpenId = 0;

    MPX_DEBUG1("CVcxMyVideosOpenHandler::HandleAlbumOpenL() exit");
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosResp(
        TUint32 aAlbumId, CMPXMedia& aVideoList, TInt aError,
        TInt aFirstNewItemIndex, TInt aNewItemCount, TBool aComplete )
    {
    TRAPD( err, HandleGetAlbumContentVideosRespL( aAlbumId, aVideoList, aError,
            aFirstNewItemIndex, aNewItemCount, aComplete ));
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosOpenHandler:: HandleGetAlbumContentVideosRespL leaved: %d", err);
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosRespL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosRespL(
        TUint32 aAlbumId, CMPXMedia& /*aVideoList*/, TInt aError,
        TInt /*aFirstNewItemIndex*/, TInt /*aNewItemCount*/, TBool aComplete )
    {
    MPX_DEBUG1("CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosRespL() start");
    
    if ( aError != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosOpenHandler:: error %d occured when fetching album videos", aError );        
        }
    
    MPX_DEBUG3("CVcxMyVideosOpenHandler:: iPendingAlbumOpenId = %d, aAlbumId = %d", iPendingAlbumOpenId, aAlbumId );
    
    if ( iPendingAlbumOpenId == aAlbumId )
        {
        MPX_DEBUG2("CVcxMyVideosOpenHandler:: First videos for album %d arrived, calling HandleOpen()", aAlbumId);
        
        iAlbumVideoList->SetCObjectValueL( KMPXMediaGeneralContainerPath, iPath );
        iCollection.iObs->HandleOpen( iAlbumVideoList, aError );
        iPendingAlbumOpenId = 0;
        }

    if ( !aComplete )
        {
        iCollection.iMessageList->AddEventL( KVcxMessageMyVideosItemsAppended );

        MPX_DEBUG3("CVcxMyVideosOpenHandler:: adding modify event for album %d, extra info = %d",
                aAlbumId, EVcxMyVideosVideoListOrderChanged );
        iCollection.iMessageList->AddEventL( TMPXItemId( aAlbumId, KVcxMvcMediaTypeAlbum ), EMPXItemModified,
                EVcxMyVideosVideoListOrderChanged );
        }
    else
        {
        //TODO: should add album id
        iCollection.AlbumsL().CalculateAttributesL(); // adds events if attributes modified, does not send

        iAlbumVideoList->SetTObjectValueL<TInt>( KVcxMediaMyVideosInt32Value,
                    EVcxMyVideosVideoListComplete );
        iCollection.iMessageList->AddEventL( KVcxMessageMyVideosListComplete );

        
        delete iAlbumVideoList;
        iAlbumVideoList = NULL;
        }
    
    iCollection.iMessageList->SendL();
    
    MPX_DEBUG1("CVcxMyVideosOpenHandler::HandleGetAlbumContentVideosRespL() exit");
    }

// END OF FILE
