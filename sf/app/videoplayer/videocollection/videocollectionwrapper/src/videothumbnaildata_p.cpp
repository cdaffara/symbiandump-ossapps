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

// Version : %version: 24.1.5 %

// INCLUDE FILES
#include <qapplication.h>
#include <qpixmap.h>
#include <qtimer.h>
#include <qpainter.h>
#include <mpxmediageneraldefs.h>
#include <hbicon.h>

#include <vcxmyvideosdefs.h>
#include <videocollectioncommon.h>

#include "videothumbnaildata_p.h"
#include "videocollectionwrapper.h"
#include "videoproxymodelgeneric.h"
#include "videothumbnailfetcher.h"
#include "videocollectiontrace.h"

// Maximum thumbnails kept in memory.
const int THUMBNAIL_CACHE_SIZE = 60;
// Maximum of thumbnail fetches done at one background fetch round.
const int THUMBNAIL_BACKGROUND_FETCH_AMOUNT = 20;
// Milliseconds for the background fetch timer.
const int THUMBNAIL_BACKGROUND_TIMEOUT = 100;
// Milliseconds while thumbnail ready events are gathered before they 
// are signaled.
const int THUMBNAIL_READY_SIGNAL_TIMEOUT = 50;
// Priority for background thumbnail fetches.
const int BACKGROUND_FETCH_PRIORITY = 3000;

// Size for default thumbnail, these match with large thumbnail in lists. 
const int DEFAULT_THUMBNAIL_WIDTH = 114;
const int DEFAULT_THUMBNAIL_HEIGHT = 64;

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
// VideoThumbnailDataPrivate::VideoThumbnailDataPrivate()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataPrivate::VideoThumbnailDataPrivate() 
    : mThumbnailFetcher( 0 )
    , mCurrentModel( 0 )
    , mCurrentFetchIndex( 0 )
    , mCurrentBackgroundFetchCount( 0 )
    , mBgFetchTimer( 0 )
    , mTbnReportTimer( 0 )
    , mSignalsConnected( false )
    , mBackgroundFetchingEnabled( true )
{
	FUNC_LOG;
	initialize();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::~VideoThumbnailDataPrivate()
// -----------------------------------------------------------------------------
//
VideoThumbnailDataPrivate::~VideoThumbnailDataPrivate()
{
	FUNC_LOG;
    cleanup();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::initialize()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::initialize()
{
	FUNC_LOG;
    mThumbnailData.setMaxCost(THUMBNAIL_CACHE_SIZE);
    
    if(!mThumbnailFetcher)
    {
        mThumbnailFetcher = new VideoThumbnailFetcher();        
    }

    if(!mBgFetchTimer)
    {
        mBgFetchTimer = new QTimer();
    }
    
    if(!mTbnReportTimer)
    {
        mTbnReportTimer = new QTimer();
    }

    if(connectSignals() < 0)
    {
        ERROR(-1, "VideoThumbnailDataPrivate::initialize() failed to connect signals.");
        cleanup();
        return -1;
    }

    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::cleanup()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::cleanup()
{
	FUNC_LOG;
    disconnectSignals();

    delete mThumbnailFetcher;
    mThumbnailFetcher = 0;
    
    freeThumbnailData();
    
    if(mTbnReportTimer)
    {
        mTbnReportTimer->stop();
        delete mTbnReportTimer;
        mTbnReportTimer = 0;
    }
    
    if(mBgFetchTimer)
    {
        mBgFetchTimer->stop();
        delete mBgFetchTimer;
        mBgFetchTimer = 0;
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::disconnectSignals()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::disconnectSignals()
{
	FUNC_LOG;
    if(mSignalsConnected)
    {
		disconnect(mThumbnailFetcher, SIGNAL(thumbnailReady(QPixmap , const TMPXItemId &, int )),
                    this, SLOT(thumbnailReadySlot(QPixmap , const TMPXItemId &, int )));
        disconnect(mThumbnailFetcher, SIGNAL(allThumbnailsFetched()),
                 this, SLOT(allThumbnailsFetchedSlot()));          
        disconnect(mBgFetchTimer, SIGNAL(timeout()), this, SLOT(doBackgroundFetching()));
        disconnect(mTbnReportTimer, SIGNAL(timeout()), this, SLOT(reportThumbnailsReadySlot()));
    }
    mSignalsConnected = false;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::connectSignals()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::connectSignals()
{
	FUNC_LOG;
    if(!mSignalsConnected)
    {
        if(!connect(mThumbnailFetcher, SIGNAL(thumbnailReady( QPixmap , const TMPXItemId &, int )),
                    this, SLOT(thumbnailReadySlot( QPixmap , const TMPXItemId &, int))) ||
            !connect(mThumbnailFetcher, SIGNAL(allThumbnailsFetched()),
                     this, SLOT(allThumbnailsFetchedSlot())) ||
           !connect(mBgFetchTimer, SIGNAL(timeout()), this, SLOT(doBackgroundFetching())) ||
           !connect(mTbnReportTimer, SIGNAL(timeout()), this, SLOT(reportThumbnailsReadySlot())))
        {
            return -1;
        }
        
        QApplication *app = qApp;
        if(!connect(app, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuitSlot())))
        {
            return -1;
        }
        
        mSignalsConnected = true;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::getThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailDataPrivate::getThumbnail( const TMPXItemId &mediaId)
{
    const QIcon *thumbnail = mThumbnailData[mediaId];
    if(!thumbnail)
    {
        return defaultThumbnail(mediaId);
    }
    return thumbnail;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startFetchingThumbnails()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::startFetchingThumbnails(const QList<QModelIndex> &indexes, int priority)
{
	FUNC_LOG;
	
    if(!mCurrentModel || !mThumbnailFetcher)
    {
        return -1;
    }
    if(indexes.count() == 0)
    {
        return 0;
    }
    
    mThumbnailFetcher->pauseFetching();

    int fetchCountBefore = mThumbnailFetcher->fetchCount();
    
    // Fetch the thumbnails
    for(int i = 0; i < indexes.count(); i++)
    {
        startFetchingThumbnail(mCurrentModel->getMediaIdAtIndex(indexes[i]), priority--);
    }
    
    int started = mThumbnailFetcher->fetchCount() - fetchCountBefore;
    
	// Start the fetches and cancel previous ones.
    mThumbnailFetcher->continueFetching(true);
    
    return started;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startFetchingThumbnail()
// -----------------------------------------------------------------------------
//
int VideoThumbnailDataPrivate::startFetchingThumbnail(const TMPXItemId &mediaId, int priority)
{
    if(!mCurrentModel || !mThumbnailFetcher)
    {
        return -1;
    }

    // Check that it's not fetched before.
    if(mThumbnailData.contains(mediaId))
    {
        return 0;
    }

    QString fileName = mCurrentModel->getMediaFilePathForId(mediaId);
    
    // Thumbnail fetcher signals into thumbnailReadySlot when thumbnail ready
    if(fileName.length() > 0)
    {
        mThumbnailFetcher->addFetch(fileName, mediaId, priority);
    }

    return 0;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::doBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::doBackgroundFetching()
{
	FUNC_LOG;
	INFO_1("VideoThumbnailDataPrivate::doBackgroundFetching() bg fetch count: %d", mCurrentBackgroundFetchCount);
	
    if(!mCurrentModel || !mThumbnailFetcher)
    {
        return;
    }
    
    if(mCurrentBackgroundFetchCount >= THUMBNAIL_CACHE_SIZE)
    {
        return;
    }
    
    int maxIndex = mCurrentModel->rowCount();
    if(maxIndex == 0)
    {
        return;
    }

    // Delta to UI index where fetch has been done already.  
    int currentDelta = mCurrentBackgroundFetchCount/2;
    
    // How many will be fetched.  
    const int fetchAmount = THUMBNAIL_BACKGROUND_FETCH_AMOUNT/2;

    QList<QModelIndex> indexes;

    // Items after the current fetch index.
    int startIndex = mCurrentFetchIndex+currentDelta;
    int endIndex = mCurrentFetchIndex+currentDelta+fetchAmount;
    getModelIndexes(indexes, startIndex, endIndex);

    // Items before the current fetch index.
    startIndex = mCurrentFetchIndex-currentDelta-fetchAmount;
    endIndex = mCurrentFetchIndex-currentDelta;
    getModelIndexes(indexes, startIndex, endIndex);
    
    mCurrentBackgroundFetchCount += THUMBNAIL_BACKGROUND_FETCH_AMOUNT;

    int fetchesStarted = startFetchingThumbnails(indexes, BACKGROUND_FETCH_PRIORITY);
    
    // No thumbnails to fetch, start again.
    if(fetchesStarted == 0)
    {
        continueBackgroundFetch();
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::getModelIndexes()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::getModelIndexes(QList<QModelIndex> &indexes, int &startIndex, int &endIndex)
{
	FUNC_LOG;
    INFO_2("VideoThumbnailDataPrivate::getModelIndexes() from %d to %d", startIndex, endIndex);
	
    QModelIndex index;
    for(int i = startIndex; i < endIndex; i++)
    {
        if(i >= 0)
        {
            index = mCurrentModel->index(i, 0);
            if(index.isValid())
            {
                indexes.append(index);
            }
        }
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::thumbnailReadySlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::thumbnailReadySlot(QPixmap tnData, const TMPXItemId &mediaId, int error)
{
    if(!error && !tnData.isNull())
    {
        mThumbnailData.insert(mediaId, new QIcon(tnData));
        
        // Gather list of media ids and emit thumbnailReady signals in larger set
        // when timer goes off.
        if(mTbnReportTimer && !mTbnReportTimer->isActive())
        {
            mTbnReportTimer->setSingleShot(true);
            mTbnReportTimer->start(THUMBNAIL_READY_SIGNAL_TIMEOUT);
        }

        // Save the media id for the signal.  
        mReadyThumbnailMediaIds.append(mediaId);
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::reportThumbnailsReadySlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::reportThumbnailsReadySlot()
{
	FUNC_LOG;
    emit thumbnailsFetched(mReadyThumbnailMediaIds);
    mReadyThumbnailMediaIds.clear();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::allThumbnailsFetchedSlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::allThumbnailsFetchedSlot()
{
	FUNC_LOG;
    continueBackgroundFetch();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::defaultThumbnail()
// -----------------------------------------------------------------------------
//
const QIcon* VideoThumbnailDataPrivate::defaultThumbnail(const TMPXItemId &mediaId)
{
    FUNC_LOG;
    
    if(!mBackgroundFetchingEnabled)
    {
        return 0;
    }
    
    const TMPXItemId defaultIdVideo(KMaxTUint32-1, KVcxMvcMediaTypeVideo);
    const TMPXItemId defaultIdAlbum(KMaxTUint32-1, KVcxMvcMediaTypeAlbum);
    const TMPXItemId defaultIdDownloads(KVcxMvcCategoryIdDownloads, KVcxMvcMediaTypeCategory);
    const TMPXItemId defaultIdCaptured(KVcxMvcCategoryIdCaptured, KVcxMvcMediaTypeCategory);

    // Default thumbnail for video
    if(mediaId.iId2 == KVcxMvcMediaTypeVideo)
    {
        if(!mDefaultThumbnails.contains(defaultIdVideo))
        {
            mDefaultThumbnails[defaultIdVideo] = loadIcon("qtg_large_video");
        }
        return mDefaultThumbnails[defaultIdVideo];
    }
    else
    {
        // Default thumbnail for user defined album.
        if(mediaId.iId2 == KVcxMvcMediaTypeAlbum)
        {
            if(!mDefaultThumbnails.contains(defaultIdAlbum))
            {
                mDefaultThumbnails[defaultIdAlbum] = loadIcon("qtg_large_video_collection");
            }
            return mDefaultThumbnails[defaultIdAlbum];
        }

        // Thumbnails for default collections.
        switch(mediaId.iId1)
        {
            case KVcxMvcCategoryIdDownloads:
            {
                if(!mDefaultThumbnails.contains(defaultIdDownloads))
                {
                    mDefaultThumbnails[defaultIdDownloads] = loadIcon("qtg_large_video_download");
                }
                return mDefaultThumbnails[defaultIdDownloads];
            }
            
            case KVcxMvcCategoryIdCaptured:
            {
                if(!mDefaultThumbnails.contains(defaultIdCaptured))
                {
                    mDefaultThumbnails[defaultIdCaptured] = loadIcon("qtg_large_video_capture");
                }
                return mDefaultThumbnails[defaultIdCaptured];
            }

            default:
            {
                return 0;
            }
        }
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::loadIcon()
// -----------------------------------------------------------------------------
//
QIcon *VideoThumbnailDataPrivate::loadIcon(QString iconName)
{   
    FUNC_LOG;
    
    HbIcon resource(iconName);
    QIcon *icon = 0;
    
    if(!resource.isNull())
    {
        // Set matching height, width is adjusted automaticly.
        resource.setHeight(DEFAULT_THUMBNAIL_HEIGHT);
        
        QPixmap dest = QPixmap(DEFAULT_THUMBNAIL_WIDTH, DEFAULT_THUMBNAIL_HEIGHT);
        
        QPainter painter(&dest);
        painter.setCompositionMode(QPainter::CompositionMode_Source);
        painter.fillRect(dest.rect(), Qt::transparent);        
        resource.paint(&painter, dest.rect());
        painter.end();
        
        icon = new QIcon(dest);
    }
    
    return icon;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::removeThumbnail()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailDataPrivate::removeThumbnail(const TMPXItemId &mediaId)
{
	FUNC_LOG;
    return mThumbnailData.remove(mediaId);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::enableBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::enableBackgroundFetching(bool enable)
{
	FUNC_LOG;
    INFO_1("VideoThumbnailDataPrivate::enableBackgroundFetching() enable: %d", enable);
    mBackgroundFetchingEnabled = enable;
    startBackgroundFetching(mCurrentModel, 0);
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::enableThumbnailCreation()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::enableThumbnailCreation(bool enable)
{
	FUNC_LOG;
	INFO_1("VideoThumbnailDataPrivate::enableThumbnailCreation() enable: %d", enable);
    if(mThumbnailFetcher)
    {
        mThumbnailFetcher->enableThumbnailCreation(enable);
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::backgroundFetchingEnabled()
// -----------------------------------------------------------------------------
//
bool VideoThumbnailDataPrivate::backgroundFetchingEnabled()
{
    return mBackgroundFetchingEnabled;
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::freeThumbnailData()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::freeThumbnailData()
{
	FUNC_LOG;
    // Stop timers.
	if(mBgFetchTimer)
    {
        mBgFetchTimer->stop();
    }
    
    if(mTbnReportTimer)
    {
        mTbnReportTimer->stop();
    }

    if(mThumbnailFetcher)
    {
        mThumbnailFetcher->cancelFetches();
    }
    
    // Clear data.
    mReadyThumbnailMediaIds.clear();
    mThumbnailData.clear();
    
    QHash<TMPXItemId, QIcon *>::const_iterator iter = mDefaultThumbnails.constBegin();
    while(iter != mDefaultThumbnails.constEnd())
    {
        delete *iter;
        iter++;
    }
    mDefaultThumbnails.clear();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::startBackgroundFetching()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::startBackgroundFetching(VideoProxyModelGeneric *model, int fetchIndex)
{
	FUNC_LOG;

    // If model is null, we continue using the current one. Model and index are kept up to date even
	// fetching is not enabled.
    if(model)
    {
        mCurrentModel = model;
    }
    
    mCurrentFetchIndex = fetchIndex;
    mCurrentBackgroundFetchCount = 0;
	
	if(!mBackgroundFetchingEnabled || !mThumbnailFetcher)
    {
        INFO("VideoThumbnailDataPrivate::startBackgroundFetching() fetching is disabled.");
        return;
    }

    if(mBgFetchTimer)
    {
        mBgFetchTimer->stop();
    }
	
    doBackgroundFetching();
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::continueBackgroundFetch()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::continueBackgroundFetch()
{
	FUNC_LOG;
    if(!mBackgroundFetchingEnabled)
    {
        INFO("VideoThumbnailDataPrivate::continueBackgroundFetch() fetching is disabled.")
        return;
    }

    if(mBgFetchTimer)
    {
        mBgFetchTimer->stop();
        mBgFetchTimer->setSingleShot(true);
        mBgFetchTimer->start(THUMBNAIL_BACKGROUND_TIMEOUT);
    }
}

// -----------------------------------------------------------------------------
// VideoThumbnailDataPrivate::aboutToQuitSlot()
// -----------------------------------------------------------------------------
//
void VideoThumbnailDataPrivate::aboutToQuitSlot()
{
	FUNC_LOG;
    cleanup();
}

// End of file
