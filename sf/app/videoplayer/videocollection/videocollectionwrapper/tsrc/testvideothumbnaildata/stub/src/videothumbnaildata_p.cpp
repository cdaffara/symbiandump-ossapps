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
* Description:  VideoThumbnailDataPrivate class implementation
*
*/

// INCLUDE FILES
#include <QApplication>
#include <QPixmap>
#include <QTimer>
#include <mpxmediageneraldefs.h>

#include "videothumbnaildata_p.h"

int VideoThumbnailDataPrivate::mConstructCallCount = 0;
int VideoThumbnailDataPrivate::mDestructCallCount = 0;
int VideoThumbnailDataPrivate::mStartFetchingThumbnailsCallCount = 0;
int VideoThumbnailDataPrivate::mStartFetchingThumbnailCallCount = 0;
int VideoThumbnailDataPrivate::mGetThumbnailCallCount = 0;
int VideoThumbnailDataPrivate::mRemoveThumbnailCallCount = 0;
bool VideoThumbnailDataPrivate::mBackgroundThumbnailFetchingEnabled = true;
int VideoThumbnailDataPrivate::mFreeThumbnailDataCallCount = 0;
int VideoThumbnailDataPrivate::mStartBackgroundFetchingCallCount = 0;
int VideoThumbnailDataPrivate::mEnableThumbnailCreationCallCount = 0;

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::VideoThumbnailDataPrivate()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataPrivate::VideoThumbnailDataPrivate() 
{
    mConstructCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::~VideoThumbnailDataPrivate()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataPrivate::~VideoThumbnailDataPrivate()
{
    mDestructCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::getThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailDataPrivate::getThumbnail( TMPXItemId /*mediaId*/ )
{
    mGetThumbnailCallCount++;
    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startFetchingThumbnails()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::startFetchingThumbnails(const QList<int> &/*indexes*/, int /*priority*/)
{
    mStartFetchingThumbnailsCallCount++;
    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startFetchingThumbnail()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::startFetchingThumbnail(TMPXItemId /*mediaId*/, int /*priority*/)
{
    VideoThumbnailDataPrivate::mStartFetchingThumbnailCallCount++;
    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::removeThumbnail()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailDataPrivate::removeThumbnail(TMPXItemId /*mediaId*/)
{
    VideoThumbnailDataPrivate::mRemoveThumbnailCallCount++;
    return true;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::enableBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::enableBackgroundFetching(bool enable)
{
    mBackgroundThumbnailFetchingEnabled = enable;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::freeThumbnailData()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::freeThumbnailData()
{
    mFreeThumbnailDataCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::startBackgroundFetching(VideoProxyModelGeneric */*model*/, int /*fetchIndex*/)
{
    mStartBackgroundFetchingCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::enableThumbnailCreation(bool /*enable*/)
{
    mEnableThumbnailCreationCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailData::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailDataPrivate::backgroundFetchingEnabled()
{
    return mBackgroundThumbnailFetchingEnabled;
}

// End of file
