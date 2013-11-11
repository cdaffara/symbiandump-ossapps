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
* Description: VideoThumbnailData class implementation
*
*/

// INCLUDE FILES
#include "videothumbnaildata.h"
#include "videothumbnailtestdata.h"

int VideoThumbnailTestData::mInstanceCallCount = 0;
int VideoThumbnailTestData::mFreeThumbnailDataCallCount = 0;
bool VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = false;
int VideoThumbnailTestData::mStartBackgroundFetchingCallCount = 0;
int VideoThumbnailTestData::mFetchIndex = -1;

VideoThumbnailData &VideoThumbnailData::instance()
{
    VideoThumbnailTestData::mInstanceCallCount++;
    static VideoThumbnailData _thumbnailData;
    return _thumbnailData;
}

VideoThumbnailData::VideoThumbnailData()
{
    // not stubbed
}

VideoThumbnailData::~VideoThumbnailData()
{
    // not stubbed
}

bool VideoThumbnailData::removeThumbnail(const TMPXItemId &mediaId)
{
    Q_UNUSED(mediaId);
    
    // not stubbed
    
    return true;
}

const QIcon* VideoThumbnailData::getThumbnail(const TMPXItemId &mediaId)
{
    Q_UNUSED(mediaId);
    
    // not stubbed
    
    return 0;
}

void VideoThumbnailData::startBackgroundFetching(VideoProxyModelGeneric *model, int fetchIndex)
{
    Q_UNUSED(model);
    VideoThumbnailTestData::mStartBackgroundFetchingCallCount++;
    VideoThumbnailTestData::mFetchIndex = fetchIndex;
}

void VideoThumbnailData::enableBackgroundFetching(bool enable)
{
    VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled = enable;
}

bool VideoThumbnailData::backgroundFetchingEnabled()
{
    return VideoThumbnailTestData::mBackgroundThumbnailFetchingEnabled;
}

void VideoThumbnailData::enableThumbnailCreation(bool enable)
{
    Q_UNUSED(enable);
    // not stubbed
}

void VideoThumbnailData::freeThumbnailData()
{
    VideoThumbnailTestData::mFreeThumbnailDataCallCount++;
}
