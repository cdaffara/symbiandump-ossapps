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
* Description: CVideoThumbnailData class implementation
* 
*/

// INCLUDE FILES
#include <qdebug.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <mpxmediageneraldefs.h>
#include <thumbnailmanager_qt.h>

#include "videothumbnaildata.h"

int VideoThumbnailData::mEnableBackgroundThumbnailFetchingCallCount = 0;
int VideoThumbnailData::mStartFetchingThumbnailsCallCount = 0;
int VideoThumbnailData::mStartFetchingThumbnailsThumbnailCount = 0;
int VideoThumbnailData::mInstanceCallCount = 0;
int VideoThumbnailData::mBackgroundThumbnailFetchingEnabled = true;
int VideoThumbnailData::mFreeThumbnailDataCallCount = 0;
int VideoThumbnailData::mStartBackgroundFetchingCallCount = 0;

// -----------------------------------------------------------------------------
// VideoThumbnailData::instance()
// -----------------------------------------------------------------------------
//
VideoThumbnailData &VideoThumbnailData::instance()
{
    mInstanceCallCount++;
    static VideoThumbnailData _thumbnailData;
    return _thumbnailData;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::VideoThumbnailData() 
{
    
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::~VideoThumbnailData()
// -----------------------------------------------------------------------------
//
VideoThumbnailData::~VideoThumbnailData()
{
    
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::startBackgroundFetching(VideoProxyModelGeneric *model, int fetchIndex)
{
    Q_UNUSED(model);
    Q_UNUSED(fetchIndex);
    mStartBackgroundFetchingCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::enableBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::enableBackgroundFetching(bool enable)
{
    mBackgroundThumbnailFetchingEnabled = enable;
    mEnableBackgroundThumbnailFetchingCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::freeThumbnailData()
// -----------------------------------------------------------------------------
//
void VideoThumbnailData::freeThumbnailData()
{
    mFreeThumbnailDataCallCount++;
}

// End of file
