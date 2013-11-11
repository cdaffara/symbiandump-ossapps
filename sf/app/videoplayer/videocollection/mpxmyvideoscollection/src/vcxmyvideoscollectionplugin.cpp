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
* Description:   Implementation of My Videos collection Plugin interface*
*/



// INCLUDE FILES
#include <e32cmn.h>
#include <s32mem.h>
#include <mpxlog.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediageneralextdefs.h>
#include <mpxcollectionpluginobserver.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <mpxmessagecontainerdefs.h>
#include <vcxmyvideosuids.h>
#include <drmutility.h>
#include <bautils.h>
#include "vcxmyvideoscollectionplugin.h"
#include "vcxmyvideoscollection.hrh"
#include "vcxmyvideoscollectionutil.h"
#include "vcxmyvideosvideocache.h"
#include "vcxmyvideoscategories.h"
#include "vcxmyvideosmessagelist.h"
#include "vcxmyvideosasyncfileoperations.h"
#include "vcxmyvideosopenhandler.h"
#include "vcxmyvideosmdsalbums.h"
#include "vcxmyvideosalbums.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCollectionPlugin* CVcxMyVideosCollectionPlugin::NewL(
    TAny* /* aInitParams */)
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::NewL");

    CVcxMyVideosCollectionPlugin* self = new (ELeave) CVcxMyVideosCollectionPlugin();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// Destructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCollectionPlugin::~CVcxMyVideosCollectionPlugin()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::~CVcxMyVideosCollectionPlugin");
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: this = %x", this);
        
    delete iMyVideosMdsDb;
    delete iCache;
    delete iMessageList;
    delete iCategories;
    delete iAsyncFileOperations;
    delete iActiveTask;
    delete iOpenHandler;
    delete iAlbums;
    iFs.Close();
    }

// ----------------------------------------------------------------------------
// Constructor.
// ----------------------------------------------------------------------------
//
CVcxMyVideosCollectionPlugin::CVcxMyVideosCollectionPlugin()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::CVcxMyVideosCollectionPlugin");
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: this = %x", this);
    }

// ----------------------------------------------------------------------------
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::ConstructL ()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::ConstructL");
    
    User::LeaveIfError( iFs.Connect() );
#ifndef VCX_ALBUMS        
    iMyVideosMdsDb = CVcxMyVideosMdsDb::NewL( this, iFs );    
#else
    iMyVideosMdsDb = CVcxMyVideosMdsDb::NewL( this, iFs, &AlbumsL() );
#endif
    iActiveTask    = CVcxMyVideosActiveTask::NewL( *this );
    iCache         = CVcxMyVideosVideoCache::NewL( *this );
    iMessageList   = CVcxMyVideosMessageList::NewL( *this );
    iOpenHandler   = CVcxMyVideosOpenHandler::NewL( *this, *iCache, *iMyVideosMdsDb );
    }

// ----------------------------------------------------------------------------
// Navigates to the given path
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::OpenL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& /* aAttrs */,
    CMPXFilter* /*aFilter*/)
    {
    iOpenHandler->OpenL( aPath );
    }

// ----------------------------------------------------------------------------
// Get the extended properties of the current file (async)
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::MediaL(
    const CMPXCollectionPath& aPath,
    const TArray<TMPXAttribute>& aAttrs,
    const TArray<TCapability>& /*aCaps*/,
    CMPXAttributeSpecs* /*aSpecs*/)
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::MediaL");
    MPX_DEBUG_PATH(aPath);
    
    RArray<TInt> supportedIds;
    CleanupClosePushL( supportedIds ); // 1->

    if ( aPath.Selection().Count() > 1 )
        {
        // it's a container if there are multiple selections, else it's not a container
        //supportedIds.AppendL(KMPXMediaIdContainer);
        //multiple selections not supported yet
        CleanupStack::PopAndDestroy( &supportedIds ); // <-1
        iObs->HandleMedia( NULL, KErrNotSupported );
        return;
        }

    RArray<TMPXItemId> ids;
    CleanupClosePushL(ids); // 2->

    aPath.SelectionL( ids );
    
    // MPX playback server asks it by path without selecting the media.
    if ( ids.Count() == 0 && aPath.Id() != KMPXInvalidItemId )
        {
        CMPXCollectionPath* path = CMPXCollectionPath::NewL( aPath );
        CleanupStack::PushL( path );
        path->SelectL( aPath.Id() );
        path->SelectionL( ids );
        CleanupStack::PopAndDestroy( path );
        }

    if ( ids.Count() == 0 )
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: request didn't contain any items ids, aborting");
        
        iObs->HandleMedia( NULL, KErrArgument );
        CleanupStack::PopAndDestroy( &ids );          // <-2
        CleanupStack::PopAndDestroy( &supportedIds ); // <-1
        return;
        }
        
    TBool useCachedVideo( EFalse );

    TInt pos;
    CMPXMedia* videoInCache = iCache->FindVideoByMdsIdL( ids[0].iId1, pos );
    
    if ( videoInCache )
        {
        // 0 attributes means "get all" -> can't use cache
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: client is requesting %d attributes", aAttrs.Count());
        if ( aAttrs.Count() > 0 )
            {
            TBool nonSupportedAttrCanBeFoundFromMds;
            if ( TVcxMyVideosCollectionUtil::AreSupported( *videoInCache, aAttrs,
                    nonSupportedAttrCanBeFoundFromMds ) )
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: all attributes found from cache");
                useCachedVideo = ETrue;
                }
            else
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: all attributes NOT found from cache");
                if ( !nonSupportedAttrCanBeFoundFromMds )
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: none of the non cached attrs can be found from MDS -> use cached version");
                    useCachedVideo = ETrue;
                    }
                else
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: at least one of the non cached attributes can be found from MDS");
                    }
                }
            }
        }

    CMPXMedia* video;

    if ( useCachedVideo )
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: using cached video");
        video = CMPXMedia::CopyL( *videoInCache );
        }
    else
        {
        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: fetching from MDS");
        video = iMyVideosMdsDb->CreateVideoL( ids[0].iId1, ETrue /* full details */ );    
        }
    
    if ( video )
        {
        iObs->HandleMedia( video, KErrNone );
        }
    else
        {
        iObs->HandleMedia( NULL, KErrNotFound );    
        }
    
    CleanupStack::PopAndDestroy( &ids );          // <-2
    CleanupStack::PopAndDestroy( &supportedIds ); // <-1
    }

// ----------------------------------------------------------------------------
// Cancel the pending request, this is called by mpx framework when client calls
// Cancel.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::CancelRequest()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::CancelRequest");

    iActiveTask->Cancel();
    iMyVideosMdsDb->Cancel();
    }
    
// ----------------------------------------------------------------------------
// Executes the given command on the collection
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::CommandL(
    CMPXCommand& aCmd)
    {
    MPX_DEBUG1("CVcxMyVideosCollectionPlugin::CommandL() start");

    if ( !aCmd.IsSupported( KMPXCommandGeneralId ) )
        {
        User::Leave( KErrArgument );
        }

    TMPXCommandId commandId = *aCmd.Value<TMPXCommandId>(KMPXCommandGeneralId);
     
    TBool syncOp( EFalse );
    if( aCmd.IsSupported( KMPXCommandGeneralDoSync ) )
        {
        syncOp = *aCmd.Value<TBool>( KMPXCommandGeneralDoSync );
        }

    if ( !syncOp )
        {
        // async
        iActiveTask->StartL( commandId, aCmd );        
        }
    else
        {
        // sync, operations to a single media object only
        TMPXCommandId commandId = *aCmd.Value<TMPXCommandId>(KMPXCommandGeneralId);

        switch ( commandId )
            {
            case KMPXCommandIdCollectionAdd:
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KMPXCommandIdCollectionAdd arrived");
                
#ifdef _DEBUG
                CMPXMedia* video = aCmd.Value<CMPXMedia>( KMPXCommandColAddMedia );                
                TUint32 mdsId( 0 );
                iMyVideosMdsDb->AddVideoL( *video, mdsId );
#else
                User::Leave( KErrNotSupported );
#endif
                }
                break;
                
            case KMPXCommandIdCollectionSet:
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KMPXCommandIdCollectionSet arrived");
                
                CMPXMedia* video = aCmd.Value<CMPXMedia>( KMPXCommandColSetMedia );
                
                TMPXItemId mpxId = TVcxMyVideosCollectionUtil::IdL( *video );
                if ( mpxId.iId2 == KVcxMvcMediaTypeVideo )
                    {
                    SetVideoL( *video );
                    }
                else if ( mpxId.iId2 == KVcxMvcMediaTypeAlbum )
                    {
                    iMyVideosMdsDb->iAlbums->SetAlbumL( *video );
                    }
                else
                    {
                    User::Leave( KErrNotFound );
                    }
                }
                break;
            
            case KVcxCommandIdMyVideos:
                {
                switch ( aCmd.ValueTObjectL<TUint32>( KVcxMediaMyVideosCommandId ) )
                    {
                    case KVcxCommandMyVideosCancelMoveOrCopy:
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KVcxCommandMyVideosCancelMoveOrCopy arrived");
                        iActiveTask->Cancel();
                        }
                        break;

                    case KVcxCommandMyVideosCancelDelete:
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KVcxCommandMyVideosCancelDelete arrived");
                        iActiveTask->Cancel();
                        }
                        break;
                    
                    case KVcxCommandMyVideosAddAlbum:
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: sync KVcxCommandMyVideosAddAlbum arrived");
                        AlbumsL().AddAlbumL( aCmd );
                        }
                        break;
                    }
                }
                break;
                   
            default:
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: UNKNOWN SYNC COMMAND ARRIVED");
                User::Leave( KErrNotSupported );
                }
            }
        }
    MPX_DEBUG1("CVcxMyVideosCollectionPlugin::CommandL() exit");            
    }

// ----------------------------------------------------------------------------
// Find the items matching the media specifications
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::FindAllL(
    const CMPXMedia& /* aCriteria */,
    const TArray<TMPXAttribute>& /* aAttrs */)
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::FindAllL");
    }

// ----------------------------------------------------------------------------
// Find the items matching the media specifications
// ----------------------------------------------------------------------------
//
CMPXMedia* CVcxMyVideosCollectionPlugin::FindAllSyncL(
    const CMPXMedia& /* aCriteria */,
    const TArray<TMPXAttribute>& /* aAttrs */)
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::FindAllSyncL");
    return NULL;
    }

// ----------------------------------------------------------------------------
// Get the list of supported capabilities
// ----------------------------------------------------------------------------
//
TCollectionCapability CVcxMyVideosCollectionPlugin::GetCapabilities()
    {
    // This one supports simple search
    return EMcSearch;
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SendMessages
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SendMessages( CMPXMessage& aMessages )
    {
    iObs->HandleMessage( aMessages );
    }

// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleMyVideosDbEvent
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleMyVideosDbEvent(
        TMPXChangeEventType aEvent,
        RArray<TUint32>& aId,
        TInt aEventsLeft )
    {
    TRAPD( err, DoHandleMyVideosDbEventL( aEvent, aId, aEventsLeft ));
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() leaved with error code: %d", err);
        }
    }
    
// ----------------------------------------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL
// ----------------------------------------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL(
        TMPXChangeEventType aEvent,
        RArray<TUint32>& aId,
        TInt /*aEventsLeft*/ )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL");
    
    RArray<TUint32> nonVideoIds;
    nonVideoIds.Reset();
    CleanupClosePushL( nonVideoIds );
    
    switch ( aEvent )
        {
        case EMPXItemDeleted:
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() --------------------------------------------.");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() Items from MDS deleted, deleting from cache |" );
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() --------------------------------------------'");

            iCache->RemoveL( aId, EFalse );
            
            CategoriesL().ResetVideoCountersL();
            CategoriesL().UpdateCategoriesL( *iCache->iVideoList, 0 );
            CategoriesL().UpdateCategoriesNewVideoNamesL();
            
#ifdef VCX_ALBUMS
            // Adds changed album indexes to Albums().iChangedAlbums.
            // AlbumsL().UpdateChangedAlbumsL() will update the changed ones and add events.
            AlbumsL().VideosAddedOrRemovedFromCacheL( aId );
            
            AlbumsL().RemoveAlbumsL( aId );
#endif
            }
            break;
        
        case EMPXItemInserted:
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() ------------------------------------.");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() Items added to MDS, adding to cache |");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() ------------------------------------'");
            
            if ( iMyVideosMdsDb->iVideoListFetchingIsOngoing )
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: video list fetching is ongoing, ignoring add event");
                CleanupStack::PopAndDestroy( &nonVideoIds );
                return;
                }
                
            TBool videoListFetchingWasCancelled = EFalse;

            // After the call, aId will contain only items which were actually inserted to cache.
            // We receive add events for all object types. When fetching the item from MDS we use
            // video condition and only video objects are added to cache. Items which were detected
            // to not be videos are added to nonVideoIds.
            iCache->AddVideosFromMdsL( aId, videoListFetchingWasCancelled,
                    &nonVideoIds, EFalse /* dont update categories*/ );

            if ( aId.Count() )
                {
                CategoriesL().ResetVideoCountersL();
                CategoriesL().UpdateCategoriesL( *iCache->iVideoList, 0 );
                CategoriesL().UpdateCategoriesNewVideoNamesL();

#ifdef VCX_ALBUMS          
                // Adds changed album indexes to Albums().iChangedAlbums.
                // AlbumsL().UpdateChangedAlbumsL() will update the changed ones and add events.
                AlbumsL().VideosAddedOrRemovedFromCacheL( aId );
#endif
                }
            
#ifdef VCX_ALBUMS
#if 0 //TODO: do this if we want to support albums which are being added by someone else than My Videos Collection
            
            //After the call nonVideoIds will contain only items which were actually added
            //to albums.
            AlbumsL().AddAlbumsFromMdsL( nonVideoIds );
#else
            nonVideoIds.Reset();
#endif
#endif
            
            if ( videoListFetchingWasCancelled )
                {
                RestartVideoListFetchingL();
                }
            }
            
            break;
        
        case EMPXItemModified:
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() --------------------------------------.");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() Items modified in MDS, updating cache |");
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin::DoHandleMyVideosDbEventL() --------------------------------------'");
            CMPXMedia* video;
#ifdef VCX_ALBUMS
            CMPXMedia* album;
#endif
            TInt count = aId.Count();
            for ( TInt i = count - 1; i >= 0; i-- )
                {
                video = iMyVideosMdsDb->CreateVideoL( aId[i], ETrue /* full details */ );
                
                if ( video )
                    {
                    CleanupStack::PushL( video ); // 1->
                    iCache->UpdateVideoL( *video );
                    CleanupStack::PopAndDestroy( video ); // <-1
                    }
                else
                    {
#ifndef VCX_ALBUMS
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: couldn't find the modified item from MDS");
                    aId.Remove( i );
#else
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: item was not found from videos, checking albums");
                    album = iMyVideosMdsDb->iAlbums->GetAlbumL( aId[i] );
                    
                    if ( album )
                        {
                        CleanupStack::PushL( album ); // 1->
                        iAlbums->UpdateAlbumL( *album ); // this will add event to iMessageList if necessarry
                        CleanupStack::PopAndDestroy( album ); // <-1
                        }
                    else
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: couldn't find the modified item from MDS");
                        }
                    aId.Remove( i );
#endif
                    }
                }
            
            }
            break;
        }

        TInt pos;
        TInt count = aId.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            if ( aEvent == EMPXItemInserted )
                {
                // add item from cache to the message if we have it.
                CMPXMedia* video = iCache->FindVideoByMdsIdL( aId[i], pos );
                TRAP_IGNORE( iMessageList->AddEventL( TMPXItemId( aId[i], KVcxMvcMediaTypeVideo),
                        aEvent, 0, video ) );
                }
            else
                {
                TRAP_IGNORE( iMessageList->AddEventL( TMPXItemId( aId[i], KVcxMvcMediaTypeVideo),
                        aEvent ) );
                }
            }

#ifdef VCX_ALBUMS
    //nonVideoIds are albums
    count = nonVideoIds.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TRAP_IGNORE( iMessageList->AddEventL(
                TMPXItemId( nonVideoIds[i], KVcxMvcMediaTypeAlbum ), aEvent ) );
        }

    AlbumsL().UpdateChangedAlbumsL();
#endif

    CleanupStack::PopAndDestroy( &nonVideoIds );
	iMessageList->SendL();
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleStepL
// ----------------------------------------------------------------------------
//
MVcxMyVideosActiveTaskObserver::TStepResult CVcxMyVideosCollectionPlugin::HandleStepL()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::HandleStepL");

    MVcxMyVideosActiveTaskObserver::TStepResult stepResult(MVcxMyVideosActiveTaskObserver::EDone);

    switch ( iActiveTask->GetTask() )
        {
        case KMPXCommandIdCollectionSet:
            {
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: async KMPXCommandIdCollectionSet in");
            
            CMPXMedia* video = CMPXMedia::NewL( *(iActiveTask->GetCommand().Value<CMPXMedia>(
                            KMPXCommandColSetMedia)) );
            
            CleanupStack::PushL( video );
            
            SetVideoL( *video );

            CleanupStack::PopAndDestroy( video );
            
            MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: async KMPXCommandIdCollectionSet out");
            
            stepResult = MVcxMyVideosActiveTaskObserver::EDone;            
            break;
            }
        case KVcxCommandIdMyVideos:
            {
            CMPXCommand& cmd = iActiveTask->GetCommand();
            if ( !cmd.IsSupported( KVcxMediaMyVideosCommandId ) )
                {
                MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxMediaMyVideosCommandId attribute not supported in cmd, aborting");
                User::Leave( KErrArgument );
                }

            TInt myVideosCmd( cmd.ValueTObjectL<TUint>( KVcxMediaMyVideosCommandId ) );

            switch ( myVideosCmd )
                {                
                case KVcxCommandMyVideosGetMediaFullDetailsByMpxId:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosGetMediaFullDetailsByMpxId received");
                    CMPXMedia& cmd = iActiveTask->GetCommand();
                    TMPXItemId mpxId( TVcxMyVideosCollectionUtil::IdL( cmd ) );
                    if ( !mpxId.iId1 && !mpxId.iId2 )
                        {
                        User::Leave( KErrArgument );
                        }
                    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosGetMediaFullDetailsByMpxId cmd: MDS ID %d requested", mpxId.iId1 );

                    CMPXMedia* video = iMyVideosMdsDb->CreateVideoL(
                            mpxId.iId1, ETrue /* full details */ );
                    
                    if ( !video )
                        {
                        User::Leave( KErrNotFound );
                        }
                    
                    CleanupStack::PushL( video ); // 1->
                                        
                    cmd.SetCObjectValueL<CMPXMedia>( KMPXCommandColAddMedia, video );
                    CleanupStack::PopAndDestroy( video ); // <-1

                    cmd.SetTObjectValueL<TUid>(KMPXMessageCollectionId, TUid::Uid(
                            KVcxUidMyVideosMpxCollection));
                    
                    stepResult = MVcxMyVideosActiveTaskObserver::EDone;            
                    }
                    break;
                    
                case KVcxCommandMyVideosGetMediasByMpxId:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosGetMediasByMpxId received");
                    
                    // Get ids from the request
                    CMPXMedia& cmd = iActiveTask->GetCommand();
                    if ( !cmd.IsSupported( KMPXMediaArrayContents ) )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: error, no array defined");
                        User::Leave( KErrArgument );
                        }
                    
                    CMPXMediaArray* idMediaArray = cmd.Value<CMPXMediaArray>(
                            KMPXMediaArrayContents );

                    if ( idMediaArray->Count() == 0 )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: error, no items in array ");
                        User::Leave( KErrArgument );
                        }

                    TMPXItemId mpxId;
                    RArray<TUint32> mdsIds;
                    RArray<TUint32> mdsIds2;
                    mdsIds.Reset();
                    CleanupClosePushL( mdsIds );  // 1->
                    mdsIds2.Reset();
                    CleanupClosePushL( mdsIds2 ); // 2->

                    for ( TInt i = 0; i < idMediaArray->Count(); i++ )
                        {
                        mpxId = (*idMediaArray)[i]->ValueTObjectL<TMPXItemId>(
                                KMPXMessageMediaGeneralId );
                        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosGetMediasByMpxId cmd: MDS ID %d requested",
                               mpxId.iId1 );
                        mdsIds.AppendL( mpxId.iId1 );
                        mdsIds2.AppendL( mpxId.iId1 );
                        }

                    if ( !iCache->iVideoList )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: iVideoListCache = NULL -> creating new empty iCache->iVideoList");
                        iCache->SetComplete( EFalse );
                        iCache->iVideoList = TVcxMyVideosCollectionUtil::CreateEmptyMediaListL();
                        }

                    TBool videoListFetchingWasCancelled = EFalse;
                        
                    if ( !iCache->IsComplete() )
                        {                            
                        // Load items to cache
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: loading requested items to iCache->iVideoList");
                        
                        iCache->AddVideosFromMdsL( mdsIds, videoListFetchingWasCancelled );
                        }
                    else
                        {
                        // iCache->iVideoList contains all 
                        }

                    CMPXMessage* message = iCache->GetVideosL( mdsIds2 );
                    CleanupStack::PushL( message ); // 3-> 

                    // Set message attributes
                    //
                    TMPXItemId itemId;
                    itemId.iId1 = KVcxCommandIdMyVideos;
                    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, itemId );
                    message->SetTObjectValueL<TInt>( KVcxMediaMyVideosCommandId,
                            KVcxMessageMyVideosGetMediasByMpxIdResp );
                    
                    SetTransactionIdL( cmd, *message );
                            
                    iMessageList->AddL( message );
                    CleanupStack::Pop( message );            //  <-3
                    iMessageList->SendL();

                    if ( videoListFetchingWasCancelled )
                        {
                        MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: video list fetching was cancelled, restarting");
                        RestartVideoListFetchingL();
                        }

                    CleanupStack::PopAndDestroy( &mdsIds2 ); //  <-2
                    CleanupStack::PopAndDestroy( &mdsIds );  //  <-1
                        
                    stepResult = MVcxMyVideosActiveTaskObserver::EDone;            
                    }
                    break;
                
                case KVcxCommandMyVideosCopy:
                case KVcxCommandMyVideosMove:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosMove or Copy step");
                    stepResult = AsyncFileOperationsL().HandleMoveOrCopyStepL();
                    }
                    break;
                    
                case KVcxCommandMyVideosDelete:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: KVcxCommandMyVideosDelete step");
                    stepResult = AsyncFileOperationsL().HandleDeleteStepL();
                    }
                    break;
                    
                case KVcxCommandMyVideosAddToAlbum:
                    iAlbums->AddVideosToAlbumL( &iActiveTask->GetCommand() );
                    stepResult = MVcxMyVideosActiveTaskObserver::EStopStepping;
                    break;

                case KVcxCommandMyVideosRemoveFromAlbum:
                    iAlbums->RemoveVideosFromAlbumL( &iActiveTask->GetCommand() );
                    stepResult = MVcxMyVideosActiveTaskObserver::EStopStepping;
                    break;
                    
                case KVcxCommandMyVideosAddAlbum:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: async KVcxCommandMyVideosAddAlbum arrived");
                    AlbumsL().AddAlbumL( iActiveTask->GetCommand() );
                    stepResult = MVcxMyVideosActiveTaskObserver::EDone;
                    break;
                    }

                case KVcxCommandMyVideosRemoveAlbums:
                    {
                    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: async KVcxCommandMyVideosRemoveAlbums arrived");
                    AlbumsL().RemoveAlbumsFromMdsOnlyL( iActiveTask->Command() );
                    stepResult = MVcxMyVideosActiveTaskObserver::EStopStepping;
                    break;
                    }

                    
                }
            }
            break;

            
        case KMPXCommandIdCollectionRemoveMedia:
            {
            TMPXItemId mpxId = iActiveTask->GetCommand().ValueTObjectL<TMPXItemId>(
                    KMPXMediaGeneralId );
            AsyncFileOperationsL().DeleteVideoL( mpxId.iId1 );
            stepResult = MVcxMyVideosActiveTaskObserver::EDone;            
            break;
            }
            
        default:
            {
            // Should never happen!
            ASSERT(0);
            break;
            }
        }
    return stepResult;
    }


// ----------------------------------------------------------------------------
// Handler for async operations completed
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleOperationCompleted(
    TInt aErr )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::HandleOperationCompleted");
    
    if ( aErr != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: Leave or cancel happened during the operation: %d", aErr);
        
        if ( iAsyncFileOperations )
            {
            TRAPD( err, AsyncFileOperationsL().CancelOperationL( aErr ) ); // generates resp message for move,copy or delete operations
            if ( err != KErrNone )
                {
                MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: failed to generate resp msg: %d", err);
                }
            }
        }

    

    CMPXCommand& cmd = iActiveTask->GetCommand();
    TRAP_IGNORE( cmd.SetTObjectValueL<TInt32>( KVcxMediaMyVideosInt32Value, aErr ) );

    iObs->HandleCommandComplete( &cmd, KErrNone );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::CategoriesL
// ----------------------------------------------------------------------------
//
CVcxMyVideosCategories& CVcxMyVideosCollectionPlugin::CategoriesL()
    {
    if ( !iCategories )
        {
        iCategories = CVcxMyVideosCategories::NewL( *this );
        }
    return *iCategories;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::AlbumsL
// TODO: Unecessarry func since we always load this
// ----------------------------------------------------------------------------
//
CVcxMyVideosAlbums& CVcxMyVideosCollectionPlugin::AlbumsL()
    {
    if ( !iAlbums )
        {
        iAlbums = CVcxMyVideosAlbums::NewL( *this );
        }
    return *iAlbums;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleCreateVideoListResp
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleCreateVideoListResp(
        CMPXMedia* aVideoList, TInt aNewItemsStartIndex, TBool aComplete )
    {
    TRAPD( err, iOpenHandler->DoHandleCreateVideoListRespL( aVideoList, aNewItemsStartIndex, aComplete ));
    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: DoHandleCreateVideoListRespL() left with error code: %d", err );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::RestartVideoListFetchingL
// Called when video list fetching is interrupted due to "get item by mpx id"
// request, or sorting order change. Or by some other reason that requires
// new video list fetching. iCache->iVideoList media array has been reset, Ie lists are the
// the same but items have been deleted.
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::RestartVideoListFetchingL()
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::RestartVideoListFetchingL()");
    
    // Client already had something on the list.
    // -> tell client to fetch everything from scratch again.
    MPX_DEBUG1("CVcxMyVideosCollectionPlugin:: open was not pending, client had something on the list");
    MPX_DEBUG1("                               -> telling client to fetch everything from the scratch again");
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: adding modify event for path root, extra info = %d",
            EVcxMyVideosVideoListOrderChanged );
    
    iCache->ResetVideoListL();
    
    iMessageList->AddEventL( TMPXItemId( KVcxUidMyVideosMpxCollection,
            KVcxUidMyVideosMpxCollection ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );
    iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SetVideoL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SetVideoL( CMPXMedia& aVideo )
    {
    TRAPD( leave, iCache->UpdateVideoL( aVideo ) );
	
    if ( leave == KErrNone )
        {
        TMPXItemId mpxId( TVcxMyVideosCollectionUtil::IdL( aVideo ) );

        TInt pos;
        CMPXMedia* videoInCache = iCache->FindVideoByMdsIdL( mpxId, pos );
		
        if ( videoInCache )
            {
            iMyVideosMdsDb->UpdateVideoL( *videoInCache );
            }
        }
    else if ( leave == KErrNotFound )
        {
        iMyVideosMdsDb->UpdateVideoL( aVideo );
        }
	else
	    {
		User::Leave( leave );
		}
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::NotifyNewVideosCountDecreasedL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::NotifyNewVideosCountDecreasedL( CMPXMedia& aVideo )
    {    
    const TInt KMediaExtSize( 8 );
    
    HBufC8* buffer = HBufC8::NewL( KMediaExtSize );
    CleanupStack::PushL( buffer );
    TPtr8 des = buffer->Des();
    
    RDesWriteStream stream;
    CleanupClosePushL( stream );
    stream.Open( des );
    
    aVideo.ExternalizeL( stream );
    
    stream.CommitL();
    
    CleanupStack::PopAndDestroy( &stream );    
        
    CleanupStack::PopAndDestroy( buffer );
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::AddVideoToMdsAndCacheL
// ----------------------------------------------------------------------------
//    
void CVcxMyVideosCollectionPlugin::AddVideoToMdsAndCacheL( CMPXMedia& aVideo )
    {
    MPX_FUNC("CVcxMyVideosCollectionPlugin::AddVideoToMdsAndCacheL");
    
    TMPXItemId mpxId;
    iMyVideosMdsDb->AddVideoL( aVideo, mpxId.iId1 );

    aVideo.SetTObjectValueL<TMPXItemId>( KMPXMediaGeneralId, mpxId );                        
    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: new MDS ID: %d", mpxId.iId1 );

    RArray<TUint32> ids;
    ids.Reset();
    CleanupClosePushL( ids ); // 1->
    ids.AppendL( mpxId.iId1 );
    HandleMyVideosDbEvent( EMPXItemInserted, ids, 0 ); //this will fetch from mds to cache
    CleanupStack::PopAndDestroy( &ids ); // <-1
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SetTransactionIdL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SetTransactionIdL( CMPXMedia& aRequest, CMPXMedia& aResp )
    {
    if ( aRequest.IsSupported( KVcxMediaMyVideosTransactionId ) )
        {
        TUint32 transactionId( aRequest.ValueTObjectL<TUint32>( KVcxMediaMyVideosTransactionId ));
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: transaction ID: %d", transactionId );
        aResp.SetTObjectValueL<TUint32>( KVcxMediaMyVideosTransactionId, transactionId );
        }
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::SendMyVideosMessageL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::SendMyVideosMessageL(
        TUint32 aMessageId, CMPXCommand* aCommand )
    {
    CMPXMessage* message = CMPXMessage::NewL();
    CleanupStack::PushL( message ); // 1->
    TMPXItemId itemId;
    itemId.iId1 = KVcxCommandIdMyVideos;
    message->SetTObjectValueL<TMPXMessageId>( KMPXMessageGeneralId, itemId );
    message->SetTObjectValueL<TInt>( KVcxMediaMyVideosCommandId,
            aMessageId );
    
    if ( aCommand )
        {
        SetTransactionIdL( *aCommand, *message );
        }
    else
        {
        message->SetTObjectValueL<TUint32>( KVcxMediaMyVideosTransactionId, 0 );
        }
            
    iMessageList->AddL( message );
    CleanupStack::Pop( message ); // <-1
    iMessageList->SendL();
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::AsyncFileOperationsL
// ----------------------------------------------------------------------------
//
CVcxMyVideosAsyncFileOperations& CVcxMyVideosCollectionPlugin::AsyncFileOperationsL()
    {
    if ( !iAsyncFileOperations )
        {
        iAsyncFileOperations = CVcxMyVideosAsyncFileOperations::NewL( *this );
        }
    return *iAsyncFileOperations;
    }

// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::HandleObjectPresentNotification
// From MVcxMyVideosMdsDbObserver. Called when media is inserted/removed. 
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::HandleObjectPresentNotification()
    {
    TRAPD( err, DoHandleObjectPresentNotificationL() );

    if ( err != KErrNone )
        {
        MPX_DEBUG2("CVcxMyVideosCollectionPlugin::DoHandleObjectPresentNotificationL() left with code %d", err);
        }
    }
    
// ----------------------------------------------------------------------------
// CVcxMyVideosCollectionPlugin::DoHandleObjectPresentNotificationL
// ----------------------------------------------------------------------------
//
void CVcxMyVideosCollectionPlugin::DoHandleObjectPresentNotificationL()
    {
    iCache->ResetVideoListL();

    MPX_DEBUG2("CVcxMyVideosCollectionPlugin:: adding modify event for path root, extra info = %d",
            EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxUidMyVideosMpxCollection,
            KVcxUidMyVideosMpxCollection ), EMPXItemModified, EVcxMyVideosVideoListOrderChanged );
            
    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdAll, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdAll, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdDownloads, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdDownloads, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdTvRecordings, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdTvRecordings, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdCaptured, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdCaptured, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    MPX_DEBUG3("CVcxMyVideosCollectionPlugin:: adding modify event for category[%d], extra info = %d",
            KVcxMvcCategoryIdOther, EVcxMyVideosVideoListOrderChanged );
    iMessageList->AddEventL( TMPXItemId( KVcxMvcCategoryIdOther, 1 ), EMPXItemModified,
            EVcxMyVideosVideoListOrderChanged );

    iMessageList->SendL();    
    }

