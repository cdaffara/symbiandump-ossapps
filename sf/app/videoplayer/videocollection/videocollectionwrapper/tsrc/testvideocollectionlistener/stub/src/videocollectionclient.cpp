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
* Description:    stub collection client for testing VideoCollectionListener
* 
*/

#include "stub/inc/videocollectionclient.h"

bool VideoCollectionClient::mSetOpenStatusStartOpening = false;
int VideoCollectionClient::mStartOpenCurrentStateCallCount = 0;

// -----------------------------------------------------------------------------
// VideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::VideoCollectionClient():
mLevel(0),
mOpenStatus(-1),
mLatestItemId(0),
mOpenItemCallCount(0)
{
    
}
 
// -----------------------------------------------------------------------------
// ~VideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::~VideoCollectionClient()
{
    
}

// -----------------------------------------------------------------------------
// setCollectionLevel
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::setCollectionLevel(int level)
{
    mLevel = level;
}
  
// -----------------------------------------------------------------------------
// getCategoryIds
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::getCategoryId(TMPXItemId &id)
{    
	id = mLatestItemId;
}

// -----------------------------------------------------------------------------
// getCollectionLevel
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::getCollectionLevel()
{
    return mLevel;
}
   
// -----------------------------------------------------------------------------
// getOpenStatus
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::getOpenStatus()
{
    return mOpenStatus;
}
 
// -----------------------------------------------------------------------------
// setOpenStatus
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::setOpenStatus(int status, bool startOpening)
{
    mSetOpenStatusStartOpening = startOpening;
    mOpenStatus = status;
}

// -----------------------------------------------------------------------------
// fetchMpxMediaByMpxId
// -----------------------------------------------------------------------------
//   
void VideoCollectionClient::fetchMpxMediaByMpxId(TMPXItemId &eventData)
{
    mLatestItemId = eventData;
}

// -----------------------------------------------------------------------------
// getLatestMPXId
// -----------------------------------------------------------------------------
// 
TMPXItemId VideoCollectionClient::getLatestMPXId()
{
    return mLatestItemId;
}

// -----------------------------------------------------------------------------
// startOpenCurrentState
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::startOpenCurrentState()
{
	mStartOpenCurrentStateCallCount++;
    return 0;
}

// -----------------------------------------------------------------------------
// openItem
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::openItem(TMPXItemId id)
{
    mOpenItemCallCount++;
    mLatestItemId = id;
}

