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
* Description: VideoThumbnailFetcher class implementation
*
*/

// INCLUDE FILES

#include <qpixmap.h>
#include <qlist.h>

#include "videothumbnailfetcher.h"

int VideoThumbnailFetcher::mConstructorCallCount = 0;
int VideoThumbnailFetcher::mDestructorCallCount = 0;
int VideoThumbnailFetcher::mAddFetchCallCount = 0;
int VideoThumbnailFetcher::mCancelFetchesCallCount = 0;
int VideoThumbnailFetcher::mFetchCountCallCount = 0;
int VideoThumbnailFetcher::mPauseFetchingCallCount = 0;
int VideoThumbnailFetcher::mContinueFetchingCallCount = 0;
int VideoThumbnailFetcher::mEnableThumbnailCreationCallCount = 0;

bool VideoThumbnailFetcher::mAddFetchFails = false;
int VideoThumbnailFetcher::mThumbnailReadyError = 0;

QMap<int, VideoThumbnailFetcher::TnRequest> VideoThumbnailFetcher::mRequests = 
    QMap<int, VideoThumbnailFetcher::TnRequest>();

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::VideoThumbnailFetcher()
// -----------------------------------------------------------------------------
//
VideoThumbnailFetcher::VideoThumbnailFetcher() 
{
    mConstructorCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::~VideoThumbnailFetcher()
// -----------------------------------------------------------------------------
//
VideoThumbnailFetcher::~VideoThumbnailFetcher()
{
    mConstructorCallCount++;
    cancelFetches();
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::addFetch()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::addFetch(const QString fileName, const TMPXItemId &mediaId, int priority)
{
    mAddFetchCallCount++;
    if(mAddFetchFails) 
        return;

    int id = mRequests.count();
    mRequests[id] = TnRequest(fileName, mediaId, priority, false);
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::continueFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::continueFetching(bool /*cancelOngoingFetches*/)
{
    mContinueFetchingCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::pauseFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::pauseFetching()
{
    mPauseFetchingCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::cancelFetches()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::cancelFetches()
{
    mCancelFetchesCallCount++;
    mRequests.clear();
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::fetchCount()
// -----------------------------------------------------------------------------
//
int VideoThumbnailFetcher::fetchCount()
{
    mFetchCountCallCount++;
    return mRequests.count();
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::enableThumbnailCreation(bool /*enable*/)
{
    mEnableThumbnailCreationCallCount++;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::emitThumbnailReady()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::emitThumbnailReady(QPixmap pixmap, const TMPXItemId &mediaId, int error)
{
    emit thumbnailReady(pixmap, mediaId, error);
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::emitAllThumbnailsFetched()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::emitAllThumbnailsFetched()
{
    emit allThumbnailsFetched();
}

// End of file.
