/**
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
* Description:   stub signal receiver for testing CVideoCollectionListener
* 
*/

#include "stubsignalreceiver.h"

// -----------------------------------------------------------------------------
// StubSignalReceiver
// -----------------------------------------------------------------------------
//
StubSignalReceiver::StubSignalReceiver() :
mLatestPtr(0),
mLatestItemId(TMPXItemId::InvalidId()),
mLatestModifiedItemId(TMPXItemId::InvalidId()),
mLatestInteger(-1),
mVideoListComplete(false),
mAlbumListComplete(false)
{
    
}

// -----------------------------------------------------------------------------
// ~StubSignalReceiver
// -----------------------------------------------------------------------------
//
StubSignalReceiver::~StubSignalReceiver()
{
    
}
 
// -----------------------------------------------------------------------------
// resetLatestItems
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::resetLatestItems()
{
    mLatestPtr = 0;
    mLatestItemId = TMPXItemId::InvalidId();
    mLatestModifiedItemId = TMPXItemId::InvalidId();
    mLatesListData.clear();
    mLatestInteger = -1;
    mVideoListComplete = false;
    mAlbumListComplete = false;
}

// -----------------------------------------------------------------------------
// getLatestPointerAddr
// -----------------------------------------------------------------------------
//
void* StubSignalReceiver::getLatestPointerAddr()
{
    return mLatestPtr;
}
 
// -----------------------------------------------------------------------------
// getLatestItemId
// -----------------------------------------------------------------------------
//
TMPXItemId StubSignalReceiver::getLatestItemId()
{
    return mLatestItemId;
}


// -----------------------------------------------------------------------------
// getLatestModifiedItemId
// -----------------------------------------------------------------------------
//
TMPXItemId StubSignalReceiver::getLatestModifiedItemId()
{
    return mLatestModifiedItemId;
}

// -----------------------------------------------------------------------------
// getLatestListData
// -----------------------------------------------------------------------------
//
QList<TMPXItemId>& StubSignalReceiver::getLatestListData()
{
    return mLatesListData;
}

// -----------------------------------------------------------------------------
// getLatestListData
// -----------------------------------------------------------------------------
//
int StubSignalReceiver::getLatestIntegerData()
{
    return mLatestInteger;
}

// -----------------------------------------------------------------------------
// getVideoListComplete
// -----------------------------------------------------------------------------
//
bool StubSignalReceiver::getVideoListComplete()
{
    return mVideoListComplete;
}

// -----------------------------------------------------------------------------
// getAlbumListComplete
// -----------------------------------------------------------------------------
//
bool StubSignalReceiver::getAlbumListComplete()
{
    return mAlbumListComplete;
}

// -----------------------------------------------------------------------------
// newVideoListSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::newVideoListSlot(CMPXMediaArray* aVideoList)
{
    mLatestPtr = aVideoList;
}

// -----------------------------------------------------------------------------
// appendVideoListSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::appendVideoListSlot( CMPXMediaArray* videoList )
{
    mLatestPtr = videoList;
}

// -----------------------------------------------------------------------------
// videoListAppendedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoListAppendedSlot(CMPXMediaArray* aVideoList)
{
    mLatestPtr = aVideoList;
}
     
  
// -----------------------------------------------------------------------------
// newVideoAvailableSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::newVideoAvailableSlot(CMPXMedia* aVideo)
{
    mLatestPtr = aVideo;
}
 
// -----------------------------------------------------------------------------
// videoDeletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDeletedSlot(TMPXItemId videoId)
{
    mLatestItemId = videoId;
}
  
// -----------------------------------------------------------------------------
// videoDeleteCompletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDeleteCompletedSlot(int count, QList<TMPXItemId> *failedMediaIds)
{
    mLatestInteger = count;
    mLatestPtr = failedMediaIds;
    
    if(!failedMediaIds)
    {
        return;
    }
    mLatesListData.clear();
    TMPXItemId data = 0;
    foreach(data, (*failedMediaIds))
    {
        mLatesListData.append(data);
    }
}

// -----------------------------------------------------------------------------
// videoDetailsCompletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoDetailsCompletedSlot(CMPXMedia* media)
{
    mLatestPtr = media;
}

// -----------------------------------------------------------------------------
// albumListAvailableSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::albumListAvailableSlot(TMPXItemId &albumId, 
                                                CMPXMediaArray *albumItems)
{
    mLatestItemId = albumId;
    mLatestPtr = albumItems;
}

// -----------------------------------------------------------------------------
// itemDeletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::itemDeletedSlot(TMPXItemId &id)
{
    mLatestItemId = id;
}

// -----------------------------------------------------------------------------
// itemDeletedSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::itemModifiedSlot(const TMPXItemId &itemId)
{
    mLatestModifiedItemId = itemId;
}

// -----------------------------------------------------------------------------
// videoListCompleteSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::videoListCompleteSlot()
{
    mVideoListComplete = true;
}

// -----------------------------------------------------------------------------
// albumListCompleteSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::albumListCompleteSlot()
{
    mAlbumListComplete = true;
}

// -----------------------------------------------------------------------------
// albumRemoveFailureSlot
// -----------------------------------------------------------------------------
//
void StubSignalReceiver::albumRemoveFailureSlot(QList<TMPXItemId> *items)
{
    mLatestPtr = items;
      
    if(!items)
    {
        return;
    }
    mLatesListData.clear();
    TMPXItemId data = 0;
    foreach(data, (*items))
    {
        mLatesListData.append(data);
    }
}

