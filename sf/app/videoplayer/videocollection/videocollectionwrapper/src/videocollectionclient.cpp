/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   VideoCollectionClient class implementation
* 
*/

// Version : %version: 41 %

// INCLUDE FILES
#include <qobject.h>
#include <centralrepository.h>
#include <mpxcollectionutility.h>
#include <mpxcollectionpath.h>
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <vcxmyvideosuids.h>

#include "videodatasignalreceiver.h"
#include "videocollectionclient.h"
#include "videocollectionlistener.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoCollectionClient()
// -----------------------------------------------------------------------------
//
VideoCollectionClient::VideoCollectionClient() 
    : mCollectionUtility( 0 )
    , mCollectionOpenStatus( ECollectionNotOpen ) 
    , mCollectionListener( 0 )
    , mOpenCategoryAlbum( TMPXItemId::InvalidId() )
    , mCollectionPathLevel( VideoCollectionCommon::ELevelInvalid )
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// initialize()
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::initialize(VideoDataSignalReceiver *signalReceiver)
{
	FUNC_LOG;
    if(!signalReceiver)
    {
        return -1;
    }
    if(mCollectionUtility && mCollectionListener)
    {
        INFO("VideoCollectionClient::initialize() already initialized.");
        return 0;
    }
    if(!mCollectionListener)
    {
        mCollectionListener = new VideoCollectionListener(*this, *signalReceiver);
        if(!mCollectionListener)
        {
            return -1;
        }
    }
    if(!mCollectionUtility)
    {
        TRAPD( error, mCollectionUtility = MMPXCollectionUtility::NewL( mCollectionListener, KMcModeDefault ) );
        if(error)
        {
            ERROR(error, "VideoCollectionClient::initialize() construction of collection utility failed.");
            delete mCollectionListener;
            mCollectionListener = 0;
            return error;
        }
    }
    return 0;
}

// -----------------------------------------------------------------------------
// ~VideoCollectionClient()
// -----------------------------------------------------------------------------
//
VideoCollectionClient::~VideoCollectionClient()
{
	FUNC_LOG;
    if (mCollectionUtility)
    {
        // closing deallocates collection utility pointer
        mCollectionUtility->Close();
    }
    delete mCollectionListener;
}

// ---------------------------------------------------------------------------
// categoryIds
// ---------------------------------------------------------------------------
//

void VideoCollectionClient::getCategoryId(TMPXItemId &id)
{
	FUNC_LOG;
    id = mOpenCategoryAlbum;
}

// ---------------------------------------------------------------------------
// getCollectionLevel
// ---------------------------------------------------------------------------
//
int VideoCollectionClient::getCollectionLevel()
{
	FUNC_LOG;
    return mCollectionPathLevel;
}

// ---------------------------------------------------------------------------
// getOpenStatus
// ---------------------------------------------------------------------------
//
int VideoCollectionClient::getOpenStatus()
{
	FUNC_LOG;
	return mCollectionOpenStatus;
}

// ---------------------------------------------------------------------------
// setOpenStatus
// ---------------------------------------------------------------------------
//
void VideoCollectionClient::setOpenStatus(int status, bool startOpening)
{
	FUNC_LOG;
	INFO_2("VideoCollectionClient::setOpenStatus() status: %d start opening %d", status, startOpening);
    mCollectionOpenStatus = status;
    if(mCollectionOpenStatus == ECollectionOpened && startOpening)
    {
        startOpenCurrentState();
    }
}

// -----------------------------------------------------------------------------
// startOpenCollection
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::startOpenCollection(int level)
{
	FUNC_LOG;
	INFO_1("VideoCollectionClient::startOpenCollection() opening level: %d", level);
	
    if(!mCollectionUtility)
    {
        return -1;
    }
    
	if((mCollectionOpenStatus == ECollectionOpening) && (getCollectionLevel() == level))
    {
        // allready opening/opened
        return 0;
    }
    mCollectionOpenStatus = ECollectionNotOpen;
    TRAPD(error, startOpenCollectionL(level));
    return error;
}

// -----------------------------------------------------------------------------
// startOpenCurrentState
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::startOpenCurrentState()
{
	FUNC_LOG;
    int error = -1;
    if(mCollectionUtility && mCollectionOpenStatus == ECollectionOpened)
    {
        TRAP(error, mCollectionUtility->Collection().OpenL());
    }
    return error;
}

// -----------------------------------------------------------------------------
// deleteFile
// -----------------------------------------------------------------------------
int VideoCollectionClient::deleteVideos(QList<TMPXItemId> *mediaIds)
{  
	FUNC_LOG;
    if(!mCollectionUtility || !mediaIds)
    {
        return -1;
    }    
    TRAPD(error, deleteVideosL(*mediaIds));
    return error;    
}

// -----------------------------------------------------------------------------
// openItem
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::openItem(const TMPXItemId &mediaId)
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        return -1;
    } 
    
    TInt error;
    if (mediaId.iId2 == KVcxMvcMediaTypeVideo)
    {
    	TRAP(error, openVideoL(mediaId));
    }
    else
    {
    	TRAP(error, openCategoryL(mediaId));
    }
    return error;
}

// -----------------------------------------------------------------------------
// back
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::back()
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        return -1;
    } 
    TRAPD(error, backL());
    return error;
}

// -----------------------------------------------------------------------------
// fetchMpxMediaByMpxId
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::fetchMpxMediaByMpxId(const TMPXItemId &mpxId)
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        return -1;
    }
   
    TRAPD(error, fetchMpxMediaByMpxIdL(mpxId));
    return error;
}

// -----------------------------------------------------------------------------
// getVideoDetails
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::getVideoDetails(const TMPXItemId &mediaId)
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        return -1;
    }
    
    TRAPD(error, getVideoDetailsL(mediaId));
   
    return error;
}

// -----------------------------------------------------------------------------
// addNewAlbum
// -----------------------------------------------------------------------------
//
TMPXItemId VideoCollectionClient::addNewAlbum(const QString &title)
{
	FUNC_LOG;
	
    TMPXItemId id = TMPXItemId::InvalidId();
    
    if (mCollectionUtility && title.length())
    {
        TRAPD(err, id = createAlbumL(title));
        if(err)
        {
            id = TMPXItemId::InvalidId();
        }
    }

    return id;
}

// -----------------------------------------------------------------------------
// removeAlbums
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::removeAlbums(const QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG;
    int err(-1);
    
    if (mCollectionUtility && mediaIds.count())
    {
        TRAP(err, removeAlbumsL(mediaIds));
    }

    return err;
}

// -----------------------------------------------------------------------------
// addItemsInAlbum
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::addItemsInAlbum(const TMPXItemId &albumId,
        const QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG;
    int err(-1);
    
    if (mCollectionUtility && albumId != TMPXItemId::InvalidId() &&
        albumId.iId2 == KVcxMvcMediaTypeAlbum && mediaIds.count())
    {
        TRAP(err, addItemsInAlbumL(albumId, mediaIds));
    }
    
    return err;
}

// -----------------------------------------------------------------------------
// removeItemsFromAlbum
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::removeItemsFromAlbum(const TMPXItemId &albumId, 
        const QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG;
    int err(-1);
        
    if (mCollectionUtility && albumId != TMPXItemId::InvalidId() &&
        albumId.iId2 == KVcxMvcMediaTypeAlbum && mediaIds.count())
    {
        TRAP(err, removeItemsFromAlbumL(albumId, mediaIds));
    }
    
    return err;
}

// -----------------------------------------------------------------------------
// renameItem
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::renameItem(const TMPXItemId &itemId, 
        const QString &newTitle) 
{
	FUNC_LOG;
    int err(-1);

    if(mCollectionUtility && !newTitle.isEmpty() &&
       (itemId.iId2 == KVcxMvcMediaTypeAlbum ||
        itemId.iId2 == KVcxMvcMediaTypeVideo ) )
    {
        TRAP(err, renameL(itemId, newTitle));
    }

    return err;
}

// -----------------------------------------------------------------------------
// startOpenCollectionL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::startOpenCollectionL(int level)
{
	FUNC_LOG;
	INFO_1("VideoCollectionClient::startOpenCollectionL() level: %d", level);
	
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }
    CMPXCollectionPath* collectionPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL( collectionPath );
  
    collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
    if (level == VideoCollectionCommon::ELevelVideos)
    {
    	collectionPath->AppendL( KVcxMvcCategoryIdAll );
    	
    	mOpenCategoryAlbum = TMPXItemId(KVcxMvcCategoryIdAll, KVcxMvcMediaTypeCategory);
    	mCollectionPathLevel = VideoCollectionCommon::ELevelVideos;
    }
    else
    {
        mOpenCategoryAlbum = TMPXItemId::InvalidId();
    	mCollectionPathLevel = VideoCollectionCommon::ELevelCategory;
    }
    mCollectionUtility->Collection().OpenL( *collectionPath );
    CleanupStack::PopAndDestroy( collectionPath );  

    mCollectionOpenStatus = ECollectionOpening;
}

// -----------------------------------------------------------------------------
// deleteVideosL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::deleteVideosL(QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG;
    if(!mCollectionUtility || mediaIds.count() == 0)
    {
        User::Leave(KErrGeneral);
    }
    CMPXMediaArray* mediasToDelete = CMPXMediaArray::NewL();
    CleanupStack::PushL( mediasToDelete );
    
    CMPXMedia* media = NULL;

    TMPXItemId currentId;
    foreach(currentId, mediaIds)
    {
        media = CMPXMedia::NewL();
        CleanupStack::PushL( media );
        media->SetTObjectValueL( KMPXMediaGeneralId, currentId );
        mediasToDelete->AppendL( *media );
        CleanupStack::PopAndDestroy( media );
    }
    
    CMPXCommand* cmd = CMPXMedia::NewL();
    CleanupStack::PushL( cmd );

    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosDelete );    
    cmd->SetTObjectValueL( KMPXCommandGeneralDoSync, EFalse );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId, 
                           TUid::Uid( KVcxUidMyVideosMpxCollection ) );
    cmd->SetCObjectValueL( KMPXMediaArrayContents, mediasToDelete );

    mCollectionUtility->Collection().CommandL( *cmd );

    CleanupStack::PopAndDestroy( cmd );  
    CleanupStack::PopAndDestroy( mediasToDelete ); 
}

// -----------------------------------------------------------------------------
// openVideoL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::openVideoL(const TMPXItemId &videoId)
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    } 
    CMPXCollectionPath* path = CMPXCollectionPath::NewL();
    CleanupStack::PushL( path );
    path->AppendL( KVcxUidMyVideosMpxCollection );
    path->AppendL( KVcxMvcCategoryIdAll );
    path->AppendL( videoId );
    path->SelectL( videoId );
      
    mCollectionUtility->Collection().OpenL( *path );
    CleanupStack::PopAndDestroy( path );  
}

// -----------------------------------------------------------------------------
// openCategoryL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::openCategoryL(const TMPXItemId &id)
{    
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    } 
    CMPXCollectionPath* collectionPath = CMPXCollectionPath::NewL();
    CleanupStack::PushL( collectionPath );
  
    collectionPath->AppendL( KVcxUidMyVideosMpxCollection );
    collectionPath->AppendL( id );
    mCollectionUtility->Collection().OpenL( *collectionPath );
    CleanupStack::PopAndDestroy( collectionPath );  
    
	mOpenCategoryAlbum = id;
	mCollectionPathLevel = VideoCollectionCommon::ELevelAlbum;
    mCollectionOpenStatus = ECollectionOpening;
}

// -----------------------------------------------------------------------------
// backL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::backL()
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }
    
    if (getCollectionLevel() > 2 )
    {
    	mCollectionUtility->Collection().BackL();
    	mCollectionOpenStatus = ECollectionOpening;
    	
    	mOpenCategoryAlbum = TMPXItemId::InvalidId();

    	mCollectionPathLevel = VideoCollectionCommon::ELevelCategory;
    }
}

// -----------------------------------------------------------------------------
// getVideoDetailsL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::getVideoDetailsL(const TMPXItemId &videoId)
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosGetMediaFullDetailsByMpxId );
    cmd->SetTObjectValueL( KMPXMediaGeneralId, videoId );
    mCollectionUtility->Collection().CommandL( *cmd );
    
    CleanupStack::PopAndDestroy( cmd );  
}

// -----------------------------------------------------------------------------
// removeAlbumsL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::removeAlbumsL(const QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL(KVcxMediaMyVideosCommandId, KVcxCommandMyVideosRemoveAlbums);
    cmd->SetTObjectValueL(KMPXCommandGeneralDoSync, EFalse);
    cmd->SetTObjectValueL(KMPXCommandGeneralCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));
  
    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL( array );
    CMPXMedia* media = 0;
    int count = mediaIds.count();
    for (int i = 0; i < count; i++)
    {
        if(mediaIds.at(i).iId2 == KVcxMvcMediaTypeAlbum)
        {
            media = CMPXMedia::NewL();
            CleanupStack::PushL(media);
            media->SetTObjectValueL(KMPXMediaGeneralId, mediaIds.at(i));
            array->AppendL(*media);
            CleanupStack::PopAndDestroy(media);
        }
    }
    if(array->Count())
    {
        cmd->SetCObjectValueL(KMPXMediaArrayContents, array);
        mCollectionUtility->Collection().CommandL(*cmd);
    }
    else
    {
        // invalid data provided
        ERROR(KErrGeneral, "VideoCollectionClient::removeItemsFromAlbumL() no albums found");
        User::Leave(KErrGeneral);   
    }

    CleanupStack::PopAndDestroy( array );  
    CleanupStack::PopAndDestroy( cmd );  
}

// -----------------------------------------------------------------------------
// createAlbumL
// -----------------------------------------------------------------------------
//
TMPXItemId VideoCollectionClient::createAlbumL(const QString &title)
{
	FUNC_LOG;
    TMPXItemId albumId = TMPXItemId::InvalidId();
    
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );

    // 1. create album
    TPtrC titlePtrC(title.utf16());
    cmd->SetTObjectValueL(KMPXCommandGeneralId, KVcxCommandIdMyVideos);
    cmd->SetTObjectValueL(KVcxMediaMyVideosCommandId, KVcxCommandMyVideosAddAlbum);
    cmd->SetTextValueL(KMPXMediaGeneralTitle, titlePtrC);
    cmd->SetTObjectValueL(KMPXCommandGeneralDoSync, ETrue);
    cmd->SetTObjectValueL(KMPXCommandGeneralCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));
    
    mCollectionUtility->Collection().CommandL(*cmd);
    
    // get album id
    if (cmd->IsSupported(KMPXMediaGeneralId))
    {
        albumId = cmd->ValueTObjectL<TMPXItemId>(KMPXMediaGeneralId);        
    }

    // cleanup
    CleanupStack::PopAndDestroy(cmd);

    return albumId;
}

// -----------------------------------------------------------------------------
// addItemsInAlbumL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::addItemsInAlbumL(const TMPXItemId &albumId,
    const QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG;
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL(cmd);
    cmd->SetTObjectValueL(KMPXCommandGeneralId, KVcxCommandIdMyVideos);
    cmd->SetTObjectValueL(KVcxMediaMyVideosCommandId, KVcxCommandMyVideosAddToAlbum);
    cmd->SetTObjectValueL(KVcxMediaMyVideosUint32Value, albumId.iId1);
    cmd->SetTObjectValueL(KMPXCommandGeneralCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));

    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL( array );
    CMPXMedia* video = 0;
    int count = mediaIds.count();
    for (int i = 0; i < count; i++)
    {
        if(mediaIds.at(i).iId2 == KVcxMvcMediaTypeVideo)
        {
            video = CMPXMedia::NewL();
            CleanupStack::PushL(video);
            video->SetTObjectValueL(KMPXMediaGeneralId, mediaIds.at(i));
            array->AppendL(*video);
            CleanupStack::PopAndDestroy(video);
        }
    }
    if(array->Count())
    {
        cmd->SetCObjectValueL(KMPXMediaArrayContents, array);
        mCollectionUtility->Collection().CommandL(*cmd);
    }
    else
    {
        // invalid data provided
        ERROR(KErrGeneral, "VideoCollectionClient::removeItemsFromAlbumL() no videos found");
        User::Leave(KErrGeneral);   
    }

    CleanupStack::PopAndDestroy(array);
    CleanupStack::PopAndDestroy(cmd);
}

// -----------------------------------------------------------------------------
// removeItemsFromAlbumL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::removeItemsFromAlbumL(const TMPXItemId &albumId, 
                                                const QList<TMPXItemId> &mediaIds)
{
	FUNC_LOG;
    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL(cmd);
    cmd->SetTObjectValueL(KMPXCommandGeneralId, KVcxCommandIdMyVideos);
    cmd->SetTObjectValueL(KVcxMediaMyVideosCommandId, KVcxCommandMyVideosRemoveFromAlbum);
    cmd->SetTObjectValueL(KVcxMediaMyVideosUint32Value, albumId.iId1);
    cmd->SetTObjectValueL(KMPXCommandGeneralCollectionId, TUid::Uid(KVcxUidMyVideosMpxCollection));

    CMPXMediaArray* array = CMPXMediaArray::NewL();
    CleanupStack::PushL( array );
    CMPXMedia* video = 0;
    int count = mediaIds.count();
    for (int i = 0; i < count; i++)
    {
        if(mediaIds.at(i).iId2 == KVcxMvcMediaTypeVideo)
        {
            video = CMPXMedia::NewL();
            CleanupStack::PushL(video);
            video->SetTObjectValueL(KMPXMediaGeneralId, mediaIds.at(i));
            array->AppendL(*video);
            CleanupStack::PopAndDestroy(video);
        }
    }
    if(array->Count())
    {
        cmd->SetCObjectValueL(KMPXMediaArrayContents, array);
        mCollectionUtility->Collection().CommandL(*cmd);
    }
    else
    {
        // invalid data provided
        ERROR(KErrGeneral, "VideoCollectionClient::removeItemsFromAlbumL() no videos found");
        User::Leave(KErrGeneral);   
    }
    CleanupStack::PopAndDestroy(array);
    CleanupStack::PopAndDestroy(cmd);
}

// -----------------------------------------------------------------------------
// renameL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::renameL(const TMPXItemId &itemId, const QString newTitle)
{
	FUNC_LOG;
    CMPXMedia *media = CMPXMedia::NewL();
    CleanupStack::PushL(media);
    TPtrC titlePtrC(newTitle.utf16());
    media->SetTObjectValueL<TMPXItemId>(KMPXMediaGeneralId, itemId);
    media->SetTextValueL(KMPXMediaGeneralTitle, titlePtrC);

    CMPXCommand* cmd = CMPXMedia::NewL();
    CleanupStack::PushL( cmd );
    
    cmd->SetTObjectValueL(KMPXCommandGeneralId, KMPXCommandIdCollectionSet);
    cmd->SetCObjectValueL<CMPXMedia>(KMPXCommandColSetMedia, media);
    cmd->SetTObjectValueL(KMPXCommandGeneralDoSync, ETrue);
    cmd->SetTObjectValueL(KMPXCommandGeneralCollectionId,
                          TUid::Uid(KVcxUidMyVideosMpxCollection));

    mCollectionUtility->Collection().CommandL(*cmd);

    CleanupStack::PopAndDestroy(cmd);
    CleanupStack::PopAndDestroy(media);
}

// -----------------------------------------------------------------------------
// fetchMpxMediaByMpxIdL
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::fetchMpxMediaByMpxIdL(const TMPXItemId &aMpxId)
{
	FUNC_LOG;
    if(!mCollectionUtility)
    {
        User::Leave(KErrGeneral);
    }

    CMPXCommand* cmd = CMPXCommand::NewL();
    CleanupStack::PushL( cmd );
    cmd->SetTObjectValueL( KMPXCommandGeneralId, KVcxCommandIdMyVideos );
    cmd->SetTObjectValueL( KVcxMediaMyVideosCommandId, KVcxCommandMyVideosGetMediasByMpxId );
    cmd->SetTObjectValueL( KMPXCommandGeneralCollectionId,
                                   TUid::Uid( KVcxUidMyVideosMpxCollection ) );
   
    CMPXMediaArray* idMediaArray = CMPXMediaArray::NewL();
    CleanupStack::PushL( idMediaArray );
    CMPXMedia* media = CMPXMedia::NewL();
    CleanupStack::PushL( media );
    media->SetTObjectValueL( KMPXMessageMediaGeneralId, aMpxId );
    idMediaArray->AppendL( *media );
    cmd->SetCObjectValueL<CMPXMediaArray>( KMPXMediaArrayContents, idMediaArray );
    cmd->SetTObjectValueL( KMPXMediaArrayCount, idMediaArray->Count() );
    mCollectionUtility->Collection().CommandL( *cmd );
   
    CleanupStack::PopAndDestroy( media );  
    CleanupStack::PopAndDestroy( idMediaArray );  
    CleanupStack::PopAndDestroy( cmd );  
}

// End of file.
