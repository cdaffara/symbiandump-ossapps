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
* Description:
*
*/

#include "logsthumbnailmanager.h"
#include "logslogger.h"
#include "logsengdefs.h"
#include <thumbnailmanager_qt.h>
#include <hbicon.h>

const int KMaxQueueSize = 25;
const int KContactFetchTimer1 = 400;
const int KContactFetchTimer2 = 20;
const int KLogsMaxCacheSize   = 50;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsThumbIconManager::LogsThumbIconManager(QObject *parent)
    : QObject(parent),
      mQueueCount(0),
      mDefaultIcon(0)
{
    mThumbnailManager = new ThumbnailManager(this);
    mThumbnailManager->setMode(ThumbnailManager::Default);
    mThumbnailManager->setQualityPreference(ThumbnailManager::OptimizeForPerformance);
    mThumbnailManager->setThumbnailSize(ThumbnailManager::ThumbnailSmall);

    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
             this, SLOT(thumbnailReady(QPixmap, void *, int, int)));
             
    mTimer = new QTimer();
    connect( mTimer, SIGNAL(timeout()), this, SLOT(timerTimeout()) );
    mDefaultIcon = new HbIcon(logsThumbUnknownId);
    mImageCache.setMaxCost(KLogsMaxCacheSize);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
LogsThumbIconManager::~LogsThumbIconManager()
{
    cancel();
    mImageCache.clear();
    delete mTimer;
    delete mDefaultIcon;
}

// -----------------------------------------------------------------------------
// Get the icon for the requested avatarPath or send a new request
// to the thumbnailmanager if it doesn't exist yet, default icon is returned
// if cached icon does not exist
// -----------------------------------------------------------------------------
//
QIcon& LogsThumbIconManager::contactIcon(const QString &avatarPath, int index)
{
    LOGS_QDEBUG( "logs [ENG] -> LogsThumbIconManager::contactIcon()" )
    QIcon* icon = 0;
    if ( avatarPath.isEmpty() ){
        icon = &defaultIcon();
    } else if ( mImageCache.contains(avatarPath) ) {
        icon = mImageCache.object(avatarPath);
        LOGS_QDEBUG( "logs [ENG] -> LogsThumbIconManager::contactIcon() image was cached" )
    }
    else
    {
        icon = &defaultIcon();
        mTimer->stop();
        mTimer->start(KContactFetchTimer1); 
        mQueueCount++;
        mRequestQueue.enqueue(qMakePair(avatarPath, index));
        if( mQueueCount > KMaxQueueSize ){
            LOGS_QDEBUG( "logs [ENG] -> LogsThumbIconManager::contactIcon() image was not cached" )
            mRequestQueue.dequeue();
            mQueueCount--;
        }        
    }
    return *icon;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
QIcon& LogsThumbIconManager::defaultIcon()
{
    return mDefaultIcon->qicon();
}

// -----------------------------------------------------------------------------
// Cancel all requests
// -----------------------------------------------------------------------------
//
void LogsThumbIconManager::cancel()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsThumbIconManager::cancel()" )
    if (!mTnmReqMap.empty()){
        QMapIterator<int, QString> iter(mTnmReqMap);
        while (iter.hasNext()){
            iter.next();
            bool result = mThumbnailManager->cancelRequest(iter.key());
        }
    }
    mTnmReqMap.clear();
    mRequestQueue.clear();
    mQueueCount = 0;
    mImageCache.clear();
}


// -----------------------------------------------------------------------------
// Called when thumbnailmanager finishes creating a thumbnail,
// emits a signal for LogsMatchesModelModel to update icon for a contact
// -----------------------------------------------------------------------------
//
void LogsThumbIconManager::thumbnailReady(const QPixmap& pixmap, void *data, int id, int error)
{
	LOGS_QDEBUG_2("LogsThumbIconManager::thumbnailReady:: error:", error );
    // Find the index
    if (mTnmReqMap.contains(id)){
        QString avatarPath = mTnmReqMap[id];
        LOGS_QDEBUG_3( "LogsThumbIconManager::thumbnailReady (avatarPath, error):", avatarPath, error );
        mTnmReqMap.remove(id);
        if ( error == 0 ){
            int *clientData = (int *)data;
            int index = *clientData;
            QIcon* icon = new QIcon(pixmap);
            LOGS_QDEBUG_3( "LogsThumbIconManager::thumbnailReady (avatarPath, mImageCache.count()):", avatarPath, mImageCache.count() );
            mImageCache.insert(avatarPath, icon);
            LOGS_QDEBUG_3( "LogsThumbIconManager::thumbnailReady (avatarPath, mImageCache.count()):", avatarPath, mImageCache.count() );
            emit contactIconReady(index);
            LOGS_QDEBUG("LogsThumbIconManager::thumbnailReady - signal emitted");
            if (!mRequestQueue.isEmpty()){
                mTimer->start(KContactFetchTimer2);                
            }
            delete clientData;
        } else {
        	  thumbnailLoad();
        }
    }
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsThumbIconManager::thumbnailLoad()
{
    LOGS_QDEBUG( "logs [ENG] -> LogsThumbIconManager::thumbnailLoad()->" )
	mTimer->stop();
    if (!mRequestQueue.isEmpty()){
		mQueueCount--;
        QPair<QString, int> req = mRequestQueue.dequeue();
        QString avatarPath = req.first;
        int index = req.second;
        int *clientData = new int(index);
        LOGS_QDEBUG_2("LogsThumbIconManager::thumbnailLoad clientData is ", clientData );
        int reqId = mThumbnailManager->getThumbnail(avatarPath, clientData, 0);
        LOGS_QDEBUG_2("LogsThumbIconManager::thumbnailLoad reqId is ", reqId );
        mTnmReqMap.insert(reqId, avatarPath);
    }
    LOGS_QDEBUG( "logs [ENG] -> LogsThumbIconManager::thumbnailLoad()<-" )
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
void LogsThumbIconManager::timerTimeout()
{
    thumbnailLoad();
 	
}


