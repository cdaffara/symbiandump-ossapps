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
* Description:   VideoCollectionListener class implementation
* 
*/

// Version : %version: 35 %

// INCLUDE FILES
#include <mpxmediageneraldefs.h>
#include <mpxmessagegeneraldefs.h>
#include <mpxmessage2.h>
#include <mpxcollectionmessagedefs.h>
#include <mpxcollectionmessage.h>
#include <mpxmessagecontainerdefs.h>
#include <mpxmediacontainerdefs.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxcollectioncommanddefs.h>
#include <vcxmyvideosdefs.h>
#include <vcxmyvideosuids.h>

#include "videocollectionlistener.h"
#include "videocollectionclient.h"
#include "videodatasignalreceiver.h"
#include "videocollectionutils.h"
#include "videocollectioncommon.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoCollectionListener
// -----------------------------------------------------------------------------
//
VideoCollectionListener::VideoCollectionListener( VideoCollectionClient &collectionClient,
                                                  VideoDataSignalReceiver &signalReceiver)  
    : mCollectionClient( collectionClient )
    , mSignalReceiver( signalReceiver )
    , mVideoUtils( VideoCollectionUtils::instance() )
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// ~VideoCollectionListener
// -----------------------------------------------------------------------------
//
VideoCollectionListener::~VideoCollectionListener()
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// HandleCollectionMediaL
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::HandleCollectionMediaL( 
        const CMPXMedia& /*aMedia*/,
        TInt /*aError*/)
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// HandleOpenL
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::HandleOpenL( 
        const CMPXMedia& aEntries,
        TInt /*aIndex*/,
        TBool /*aComplete*/,
        TInt aError)
{
	FUNC_LOG;
    if(aError != KErrNone)
    {
        ERROR(aError, "VideoCollectionListener::HandleOpenL()");
        return;
    }

    // Check that current level is valid and entries has collection path. 
    if(mCollectionClient.getCollectionLevel() < VideoCollectionCommon::ELevelCategory )
    {
        ERROR(-1, "VideoCollectionListener::HandleOpenL() invalid level");
        return;
    }

    CMPXMediaArray *array =
                    mVideoUtils.mediaValuePtr<CMPXMessageArray>(&aEntries, KMPXMediaArrayContents);
    if(!array)
    {
        ERROR(-1, "VideoCollectionListener::HandleOpenL() array contents is NULL.");
        return;
    }

    CMPXCollectionPath* path = 
                    mVideoUtils.mediaValuePtr<CMPXCollectionPath>(&aEntries, KMPXMediaGeneralContainerPath);
    if(!path)
	{
        ERROR(-1, "VideoCollectionListener::HandleOpenL() no container path in message.");
        return;
	}

    TMPXItemId pathId = path->Id();
    
    TBool categoryOrAlbumVideoList = false;
    if(path->Levels() == VideoCollectionCommon::PathLevelVideos && pathId.iId2 != KVcxMvcMediaTypeVideo)
	{
        categoryOrAlbumVideoList = true;
	}

    if(categoryOrAlbumVideoList)
    {
        // checks if there are new videos in the list that are not yet in our data model.
        mSignalReceiver.newVideoListSlot(array);
        
        mSignalReceiver.albumListAvailableSlot(pathId, array);
    }
    else
    {
        mSignalReceiver.newVideoListSlot(array);
    }
    
    int listCompleted(-1);
    
    if(path->Levels() == VideoCollectionCommon::PathLevelCategories)
    {
        bool listIsPartial(false);
        if(mVideoUtils.mediaValue<bool>(&aEntries, KVcxMediaMyVideosVideoListIsPartial, listIsPartial))
        {
            if(!listIsPartial)
            {
                mSignalReceiver.albumListCompleteSlot();
            }
        }
    }
    else if(mVideoUtils.mediaValue<int>(&aEntries, KVcxMediaMyVideosInt32Value, listCompleted))
    {
        if(listCompleted == EVcxMyVideosVideoListComplete)
        {
            mSignalReceiver.videoListCompleteSlot();
        }
    }
}

// -----------------------------------------------------------------------------
// HandleOpenL
// -----------------------------------------------------------------------------
//                      
void VideoCollectionListener::HandleOpenL( 
        const CMPXCollectionPlaylist& /*aPlaylist*/,
        TInt /*aError*/)
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// HandleCommandComplete
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::HandleCommandComplete( 
        CMPXCommand* aCommandResult, 
        TInt aError)
{
	FUNC_LOG;
    if(aError != KErrNone || !aCommandResult)
    {
        ERROR_1(aError, "VideoCollectionListener::HandleCommandComplete() result: %d", aCommandResult);
        return;
    }
    int commandId = -1;
    
    if(mVideoUtils.mediaValue<int>(aCommandResult, KVcxMediaMyVideosCommandId, commandId))
    {
        if(commandId == KVcxCommandMyVideosGetMediaFullDetailsByMpxId)
        {
            handleGetVideoDetailsResp(aCommandResult);
        }
        else if(commandId == KVcxCommandMyVideosRemoveAlbums)
        {
			CMPXMediaArray *messageArray = 
				mVideoUtils.mediaValuePtr<CMPXMediaArray>(aCommandResult, KMPXMediaArrayContents);
			
			if(!messageArray || messageArray->Count() == 0)
			{
				return;
			}
			QList<TMPXItemId> failedIds;    
			TMPXItemId itemId;  
			int count = messageArray->Count();
			int failedFlag = 0;
			CMPXMedia* item = NULL;
			
			// go throught all removed albums and see if some has failed
			for (int i = 0; i < count; ++i)
			{
				item = (*messageArray)[i];
				if(!mVideoUtils.mediaValue<TMPXItemId>(item, KMPXMediaGeneralId, itemId))
				{
					// invalid message, return 
					return;
				}
				// if there's error while fetching value, it means that value does not exists,
				// so we can assume remove was succefull
				if(mVideoUtils.mediaValue<int>(item, KVcxMediaMyVideosInt32Value, failedFlag))
				{
					if (failedFlag)
					{
						failedIds.append(itemId);
					}
					failedFlag = 0;
				}       
			}
			if (failedIds.count())
			{
				mSignalReceiver.albumRemoveFailureSlot(&failedIds);
			}
        }
    }
}

// -----------------------------------------------------------------------------
// HandleCollectionMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::HandleCollectionMessage( 
        CMPXMessage* aMessage,
        TInt aError )
{
	FUNC_LOG;
    if(aError)
    {
        ERROR(aError, "VideoCollectionListener::HandleCollectionMessage()");
        return;
    }  

    TMPXMessageId mainMessageId;
    if(!mVideoUtils.mediaValue<TMPXMessageId>(aMessage, KMPXMessageGeneralId, mainMessageId))
    {
        return;
    }  
    
    if(mCollectionClient.getOpenStatus() == VideoCollectionClient::ECollectionOpened )
    {
        // after colletion has been opened we handle messages from our collection plugin only
        TUid collectionUid = {0};
        bool status = mVideoUtils.mediaValue<TUid>(aMessage, KMPXMessageCollectionId, collectionUid);
        if(!status || collectionUid.iUid != KVcxUidMyVideosMpxCollection)
        {
            return;
        }
    }
    else
    {
        // before opening we're just excepting general messages
        if(mainMessageId == KMPXMessageGeneral)
        {
            handleGeneralMPXMessage(aMessage);
        }
        return;
    }

    if( mainMessageId == KVcxCommandIdMyVideos)
    {        
        int myVideosMainMsgId = -1; 

        if(!mVideoUtils.mediaValue<int>(aMessage, KVcxMediaMyVideosCommandId, myVideosMainMsgId))
        {
            return;
        }

        if (myVideosMainMsgId == KVcxMessageMyVideosMessageArray)
        {
            handleMyVideosMessageArray(aMessage); 
        }
        else
        {
            handleMyVideosMPXMessage(myVideosMainMsgId, aMessage);
        }
    }
    else
    {
        handleMPXMessage(mainMessageId, aMessage);
    }
}

// -----------------------------------------------------------------------------
// handleMyVideosMessageArray
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMyVideosMessageArray(CMPXMessage *aMessage)
{   
	FUNC_LOG;
    CMPXMessageArray* messageArray = NULL;
    
    messageArray = mVideoUtils.mediaValuePtr<CMPXMessageArray>(aMessage, KMPXMessageArrayContents);
    if(!messageArray)
    {
        return;
    }
    
    int count = messageArray->Count();
    int myVideosMsgId;
    TMPXMessageId mpxMessageId;
    
    CMPXMessage *singleMessage;
    for ( int i = 0; i < count; ++i )
    {
        singleMessage = (*messageArray)[i];
        if(mVideoUtils.mediaValue<int>(singleMessage, KVcxMediaMyVideosCommandId, myVideosMsgId))
        {
            handleMyVideosMPXMessage(myVideosMsgId, singleMessage);
        }
        else if(mVideoUtils.mediaValue<TMPXMessageId>(
                                        singleMessage, KMPXMessageGeneralId, mpxMessageId))
        {
            handleMPXMessage(mpxMessageId, singleMessage);
        }
    }    
}

// -----------------------------------------------------------------------------
// handleMyVideosMPXMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMyVideosMPXMessage(int &myVideosMsgId, CMPXMessage *aMessage)
{
	FUNC_LOG;
    switch (myVideosMsgId)
    {
        case KVcxMessageMyVideosGetMediasByMpxIdResp:
            handleGetMediasByMpxIdResp(aMessage);
            break;
        case KVcxMessageMyVideosDeleteResp:
            handleMyVideosDeleteMessage(aMessage);
            break;
        case KVcxMessageMyVideosDeleteStarted:
            break;
        case KVcxMessageMyVideosListComplete:
            mCollectionClient.startOpenCurrentState();
            break;
        case KVcxMessageMyVideosItemsAppended:
            mCollectionClient.startOpenCurrentState();
            break;
        case KVcxMessageMyVideosMoveOrCopyStarted:
            break;
        case KVcxMessageMyVideosMoveResp:
            break;
        case KVcxMessageMyVideosCopyResp:
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// handleCommonMPXMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMPXMessage(TMPXMessageId &mpxMessageId, CMPXMessage *aMessage)
{
	FUNC_LOG;
    switch(mpxMessageId)
    {
        case KMPXMessageGeneral:
            handleGeneralMPXMessage(aMessage);
        break;
        case KMPXMessageIdItemChanged:
            handleMyVideosItemsChanged(aMessage);
        break;
        default:
        break;
    }
}

// -----------------------------------------------------------------------------
// handleGeneralMPXMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleGeneralMPXMessage(CMPXMessage* aMessage)
{
	FUNC_LOG;
    if(mCollectionClient.getOpenStatus() != VideoCollectionClient::ECollectionOpening)
    {
        return;
    }
    int event = 0;
    if(!mVideoUtils.mediaValue<int>(aMessage, KMPXMessageGeneralEvent, event))
    {
        return;
    }

    int data = 0;
    if(!mVideoUtils.mediaValue<int>(aMessage, KMPXMessageGeneralData, data))
    {
        return;
    }

    if( event == TMPXCollectionMessage::EPathChanged &&    
        data == EMcContainerOpened )
    {
        mCollectionClient.setOpenStatus(VideoCollectionClient::ECollectionOpened);
    }
}

// -----------------------------------------------------------------------------
// handleMyVideosItemsChanged
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMyVideosItemsChanged(CMPXMessage* aMessage)
{
	FUNC_LOG;
    TMPXChangeEventType eventType = EMPXItemModified; 
    if(!mVideoUtils.mediaValue<TMPXChangeEventType>(
        aMessage,KMPXMessageChangeEventType, eventType))
    {
        return;
    }
    TMPXItemId itemId = TMPXItemId::InvalidId(); 
    if(!mVideoUtils.mediaValue<TMPXItemId>(
        aMessage,KMPXMessageMediaGeneralId, itemId))
    {       
        return;
    }
    INFO_3("VideoCollectionListener::handleMyVideosItemsChanged event: %d - item id1: %d id2: %d", eventType, itemId.iId1, itemId.iId2);
    switch(eventType)
    {
        case EMPXItemDeleted:
        {
            mSignalReceiver.itemDeletedSlot(itemId);
            break;
        }
        case EMPXItemInserted:
        {
            CMPXMedia *media = mVideoUtils.mediaValuePtr<CMPXMedia>(
                aMessage, KMPXCommandColAddMedia);
            if(media)
            {
                mSignalReceiver.newVideoAvailableSlot(media); 
            }
            else
            {
                mCollectionClient.fetchMpxMediaByMpxId(itemId);
            }
            break;
        }
        case EMPXItemModified:
        {
            INFO("VideoCollectionListener::handleMyVideosItemsChanged EMPXItemModified");
            // Inform that item data has changed.
            mSignalReceiver.itemModifiedSlot(itemId);
            // Update contents for albums and captured and downloads categories.
            if( itemId.iId2 == KVcxMvcMediaTypeAlbum ||
               (itemId.iId2 == KVcxMvcMediaTypeCategory &&
               (itemId.iId1 == KVcxMvcCategoryIdCaptured ||
                itemId.iId1 == KVcxMvcCategoryIdDownloads)) )
            {
                INFO("VideoCollectionListener::handleMyVideosItemsChanged album or category modified, opening.");
                mCollectionClient.openItem(itemId);
                // collection is already opened at this stage, so we can safely set value here
                // to make sure we do not let any other messages unhandled 
                mCollectionClient.setOpenStatus(VideoCollectionClient::ECollectionOpened, false);
            }
            break;
        }
        default:
        {
            // invalid event type
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// handleMyVideosDeleteMessage
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleMyVideosDeleteMessage(CMPXMessage* aMessage)
    {
	FUNC_LOG;
    CMPXMediaArray *messageArray = 
        mVideoUtils.mediaValuePtr<CMPXMediaArray>(aMessage, KMPXMediaArrayContents);
    if(!messageArray || messageArray->Count() == 0)
    {
        return;
    }
    
    QList<TMPXItemId> failedIds;    
    TMPXItemId itemId;  
    int count = messageArray->Count();
    int failedFlag = 0;
    CMPXMedia* item = NULL;
    
    // go throught all removed videos and see if some has failed
    for (int i = 0; i < count; ++i)
    {
        item = (*messageArray)[i];
        if(!mVideoUtils.mediaValue<TMPXItemId>(item, KMPXMediaGeneralId, itemId))
        {
            // invalid message, return 
            return;
        }
        // if there's error while fetching value, it means that value does not exists,
        // so we can assume deletion was succefull
        if(mVideoUtils.mediaValue<int>(item, KVcxMediaMyVideosInt32Value, failedFlag))
        {
            if (failedFlag)
            {
                failedIds.append(itemId);
            }
            failedFlag = 0;
        }       
    }
    mSignalReceiver.videoDeleteCompletedSlot(count, &failedIds);
}

// -----------------------------------------------------------------------------
// handleGetMediasByMpxIdResp
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleGetMediasByMpxIdResp(CMPXMessage* aMessage)
{
	FUNC_LOG;
    CMPXMediaArray* array = 
        mVideoUtils.mediaValuePtr<CMPXMediaArray>(aMessage, KMPXMediaArrayContents);
    if(!array || array->Count() < 1)
    {
        return;
    }
    mSignalReceiver.newVideoAvailableSlot((*array)[0]);
}

// -----------------------------------------------------------------------------
// handleGetVideoDetailsResp
// -----------------------------------------------------------------------------
//
void VideoCollectionListener::handleGetVideoDetailsResp(CMPXMessage* aMessage)
{
	FUNC_LOG;
    CMPXMedia *item = mVideoUtils.mediaValuePtr<CMPXMedia>(aMessage,KMPXCommandColAddMedia); 
    if(!item)
    {
        return;
    }
    mSignalReceiver.videoDetailsCompletedSlot(item);
}

// End of file.
