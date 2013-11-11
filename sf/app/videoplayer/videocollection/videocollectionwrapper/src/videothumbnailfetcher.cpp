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

// Version : %version: 6.1.1 %

// INCLUDE FILES
#include <qpixmap.h>
#include <thumbnailmanager_qt.h>

#include "videothumbnailfetcher.h"
#include "videocollectiontrace.h"

/**
 * global qHash function required fo creating hash values for TMPXItemId -keys
 */
inline uint qHash(TMPXItemId key) 
{ 
    QPair<uint, uint> keyPair(key.iId1, key.iId2);

    return qHash(keyPair);
}
// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::VideoThumbnailFetcher()
// -----------------------------------------------------------------------------
//
VideoThumbnailFetcher::VideoThumbnailFetcher() 
    : mThumbnailManager( 0 )
    , mPaused( false )
    , mTbnCreationEnabled( true )
{
	FUNC_LOG;
    mThumbnailManager = new ThumbnailManager();
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForPerformance);
    mThumbnailManager->setMode(ThumbnailManager::CropToAspectRatio);
    
    connect(mThumbnailManager, SIGNAL(thumbnailReady( QPixmap , void * , int , int )),
                this, SLOT(thumbnailReadySlot( QPixmap , void * , int , int )));
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::~VideoThumbnailFetcher()
// -----------------------------------------------------------------------------
//
VideoThumbnailFetcher::~VideoThumbnailFetcher()
{
	FUNC_LOG;
    cancelFetches();

    disconnect(mThumbnailManager, SIGNAL(thumbnailReady( QPixmap , void * , int , int )),
                this, SLOT(thumbnailReadySlot( QPixmap , void * , int , int )));
    
    delete mThumbnailManager;
    mThumbnailManager = 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::addFetch()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::addFetch(const QString fileName, const TMPXItemId &mediaId, int priority)
{
    ThumbnailFetchData *fetch = new ThumbnailFetchData;
    fetch->mFileName = fileName;
    fetch->mMediaId = mediaId;
    fetch->mPriority = priority;
    fetch->mRequestId = -1;
    mFetchList.insert(mediaId, fetch);
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::continueFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::continueFetching(bool cancelOngoingFetches)
{
	FUNC_LOG;
    mPaused = false;
    
    // First fetch all thumbnails that have been created already, next
    // start thumbnail creation for one thumbnail at a time. 
    if(!mFetchList.isEmpty())
    {
        startThumbnailFetches(cancelOngoingFetches);
    }
    else if(!mCreationList.isEmpty())
    {
        startThumbnailCreation();
    }

    // All thumbnails have been fetched, report it.
    if(mFetchList.isEmpty() && mCreationList.isEmpty() && mStartedFetchList.isEmpty())
    {
        emit allThumbnailsFetched();
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::startThumbnailFetches()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::startThumbnailFetches(bool cancelOngoingFetches)
{
	FUNC_LOG;
	
    if(!mThumbnailManager)
    {
        return;
    }
    
    // Only fetch those thumbnails that are already been created.
    mThumbnailManager->setMode(ThumbnailManager::DoNotCreate);
    
    // Cancel ongoing fetches, but only those that are not in the list of new fetches.
    if(cancelOngoingFetches)
    {
        QHash<TMPXItemId, ThumbnailFetchData*>::iterator iter = mStartedFetchList.begin();
        while(iter != mStartedFetchList.end())
        {
            if(!mFetchList.contains(iter.key()))
            {
                ThumbnailFetchData *fetch = iter.value();
                INFO_3("VideoThumbnailFetcher::startThumbnailFetches() canceling - mpx id: (%d, %d), requestId: %d", fetch->mMediaId.iId1, fetch->mMediaId.iId2, fetch->mRequestId);
                mThumbnailManager->cancelRequest(fetch->mRequestId);
                iter = mStartedFetchList.erase(iter);
                delete fetch;
            }
            else
            {
                iter++;    
            }
        }
    }
    
    // Start fetches.
    QHash<TMPXItemId, ThumbnailFetchData*>::iterator iter = mFetchList.begin();
    while(iter != mFetchList.end())
    {
        ThumbnailFetchData *fetch = iter.value(); 
        iter = mFetchList.erase(iter);

        if(!mStartedFetchList.contains(fetch->mMediaId))
        {
            TMPXItemId *internal = new TMPXItemId(fetch->mMediaId.iId1, fetch->mMediaId.iId2);
            
            fetch->mRequestId = mThumbnailManager->getThumbnail(fetch->mFileName,
                   internal, fetch->mPriority);
            INFO_3("VideoThumbnailFetcher::startThumbnailFetches() started - mpx id: (%d, %d), requestId: %d", fetch->mMediaId.iId1, fetch->mMediaId.iId2, fetch->mRequestId);
            if(fetch->mRequestId != -1)
            {
                // Request succeed, add to list of started fetches.
                mStartedFetchList.insert(fetch->mMediaId, fetch);
            }
            else
            {
                // Request failed, free data.
                delete internal;
                delete fetch;
            }
        }
        else
        {
            INFO_3("VideoThumbnailFetcher::startThumbnailFetches() already fetching - mpx id: (%d, %d), requestId: %d", fetch->mMediaId.iId1, fetch->mMediaId.iId2, fetch->mRequestId);
            // Already fetching this one, fetch data not needed anymore.
            delete fetch;
        }
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::startThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::startThumbnailCreation()
{
	FUNC_LOG;
    if(!mThumbnailManager || !mTbnCreationEnabled)
    {
        return;
    }
    
    mThumbnailManager->setMode(ThumbnailManager::CropToAspectRatio);
    
    // Do nothing if list is empty. 
    if(mCreationList.isEmpty())
    {
        return;
    }
    
    // Find fetch with highest priority. 
    int highestPriority = 0;
    int indexWithHighestPriority = 0;
    
    for(int i = 0; i < mCreationList.count(); i++)
    {
        if(mCreationList.at(i)->mPriority > highestPriority)
        {
            indexWithHighestPriority = i;
            highestPriority = mCreationList.at(i)->mPriority;
        }
    }
    
    ThumbnailFetchData *fetch = mCreationList.takeAt(indexWithHighestPriority);
    TMPXItemId *internal = new TMPXItemId(fetch->mMediaId.iId1, fetch->mMediaId.iId2);
    
    // Do request to thumbnail manager.
    int requestId = mThumbnailManager->getThumbnail(fetch->mFileName,
            internal, fetch->mPriority);
    INFO_3("VideoThumbnailFetcher::startThumbnailCreation() started - mpx id: (%d, %d), requestId: %d", fetch->mMediaId.iId1, fetch->mMediaId.iId2, requestId);
    
    // Request failed, free data.
    if(requestId == -1)
    {
        delete internal;
    }
    // No need for the fetch data anymore when creating thumbnails.
    delete fetch;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::pauseFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::pauseFetching()
{
	FUNC_LOG;
    mPaused = true;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::cancelFetches()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::cancelFetches()
{
	FUNC_LOG;
	
    QHash<TMPXItemId, ThumbnailFetchData*>::const_iterator iter = mStartedFetchList.constBegin();
    while(iter != mStartedFetchList.constEnd())
    {
        mThumbnailManager->cancelRequest(iter.value()->mRequestId);
        delete *iter;
        iter++;
    }
    mStartedFetchList.clear();
    
    iter = mFetchList.constBegin();
    while(iter != mFetchList.constEnd())
    {
        delete *iter;
        iter++;
    }
    mFetchList.clear();
	
    while(!mCreationList.isEmpty())
    {
        ThumbnailFetchData *fetch = mCreationList.takeFirst();
        delete fetch;
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::fetchCount()
// -----------------------------------------------------------------------------
//
int VideoThumbnailFetcher::fetchCount()
{
    return mFetchList.count() + mCreationList.count() + mStartedFetchList.count();
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::enableThumbnailCreation(bool enable)
{
	FUNC_LOG;
    mTbnCreationEnabled = enable;
}

// -----------------------------------------------------------------------------
// VideoThumbnailFetcher::thumbnailReadySlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailFetcher::thumbnailReadySlot(QPixmap tnData, void *internal, int requestId, int error)
{
    INFO_2("VideoThumbnailFetcher::thumbnailReadySlot() requestId: %d, error: %d", requestId, error);
    Q_UNUSED(requestId);
    
    TMPXItemId mediaId = TMPXItemId::InvalidId();
    if(internal)
    {
        TMPXItemId *idPointer = static_cast<TMPXItemId*>(internal); 
        mediaId = *idPointer;
        delete idPointer;
    }

    // Thumbnail has not been generated yet, put it into creation list.
    if(error == -1)
    {   
        if(mStartedFetchList.contains(mediaId))
        {
            ThumbnailFetchData *fetch = mStartedFetchList.take(mediaId);
            mCreationList.append(fetch);
        }
        // If it's not found from started list then cancelFetches has been called and 
        // there's nothing to do with the failed fetch.
    }
    else
    {
        // Report that thumbnail was fetched, internal data pointer ownership moves.
        emit thumbnailReady(tnData, mediaId, error);
        if(mStartedFetchList.contains(mediaId))
        {
            delete mStartedFetchList.take(mediaId);
        }
    }
    
    // Continue the fetching process.
    if(!mPaused && mStartedFetchList.isEmpty())
    {
        continueFetching(false); // No need to cancel fetches because there's none.
    }
}

// End of file.
