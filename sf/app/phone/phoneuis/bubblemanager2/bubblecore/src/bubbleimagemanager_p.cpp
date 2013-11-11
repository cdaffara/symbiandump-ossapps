/*!
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QtCore>
#include "bubbleimagemanager_p.h"
#include "bubbleimagemanager.h"
#include <thumbnailmanager_qt.h>

//#define BUBBLE_IMAGEMANAGER_DEBUG
#if defined(BUBBLE_IMAGEMANAGER_DEBUG)
   #define BUBBLE_QDEBUG( a )
   #define BUBBLE_QDEBUG_2( a, b)
#else
   #define BUBBLE_QDEBUG( a )\
       qDebug() << a;

   #define BUBBLE_QDEBUG_2( a, b)\
       qDebug() << a << b;
#endif

BubbleImageManagerPrivate::BubbleImageManagerPrivate(
    BubbleImageManager* parent)
    : q_ptr(parent),
      mThumbnailManager(0),
      mInitialized(false)
{
}

BubbleImageManagerPrivate::~BubbleImageManagerPrivate()
{
}

void BubbleImageManagerPrivate::initialize()
{
    mThumbnailManager = new ThumbnailManager(this);
    
    mThumbnailManager->setMode(ThumbnailManager::Default);
    
    mThumbnailManager->setQualityPreference(
        ThumbnailManager::OptimizeForPerformance);
    
    mThumbnailManager->setThumbnailSize(
        ThumbnailManager::ThumbnailLarge);
    
    connect(mThumbnailManager,
            SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
            this,
            SLOT(thumbnailReady(QPixmap, void *, int, int)));
    
    mInitialized = true;
}

const QPixmap* BubbleImageManagerPrivate::pixmap(
    const QString& imageFileName)
{
    if (mImageCache.contains(imageFileName)) {
        return mImageCache.value(imageFileName);
    } else {
        return 0;
    }
}

void BubbleImageManagerPrivate::loadImage(const QString& imageFileName)
{
    if (mImageCache.contains(imageFileName) ||
        isLoading(imageFileName)) {
        return;
    }

    if (!mInitialized) {
        // connect to thumbnail server
        initialize();
    }

    BUBBLE_QDEBUG_2("loading image: ", imageFileName);

    QString* data = new QString(imageFileName);
    int reqId = mThumbnailManager->getThumbnail(imageFileName, data);
    mRequestQueue.enqueue(qMakePair(imageFileName,reqId));
}

void BubbleImageManagerPrivate::unloadImage(const QString& imageFileName)
{
    BUBBLE_QDEBUG_2("unloading image: ", imageFileName);

    // returns null, if key doesn't exists
    QPixmap* pixmap = mImageCache.take(imageFileName);
    delete pixmap;

    // cancel possible outstanding request
    QMutableListIterator< QPair<QString,int> > requests(mRequestQueue);
    while(requests.hasNext()) {
        QPair<QString,int>& request = requests.next();  
        
        if (request.first==imageFileName) {
            BUBBLE_QDEBUG_2("cancelling request: ", imageFileName);
            mThumbnailManager->cancelRequest(request.second);            
            requests.remove();
            break;
        }
    }
}

void BubbleImageManagerPrivate::releasePixmaps()
{
    BUBBLE_QDEBUG_2("releasing all pixmaps: ", mImageCache.count());

    // release all cached pixmaps
    QList<QPixmap*> pixmaps = mImageCache.values();
    qDeleteAll(pixmaps);
    mImageCache.clear();
}

void BubbleImageManagerPrivate::thumbnailReady(
    const QPixmap& pixmap, void *data, int id, int error)
{
    QString* fileName = (QString*)data;            
    
    if (error==0 && fileName) {
        QPixmap *p = new QPixmap(pixmap);
        mImageCache.insert(*fileName,p);
        emit q_ptr->pixmapReady(*fileName);
        BUBBLE_QDEBUG_2("thumbnailReady: ", *fileName);
    }
    
    // request complete, remove it from queue
    QMutableListIterator< QPair<QString,int> > requests(mRequestQueue);
    while(requests.hasNext()) {
        QPair<QString,int>& request = requests.next();  
        if (request.second==id) {
            BUBBLE_QDEBUG_2("deleting completed request: ", *fileName);
            requests.remove();
            break;
        }
    }    
    
    delete fileName;    
}

void BubbleImageManagerPrivate::thumbnailReady()
{
}

bool BubbleImageManagerPrivate::isLoading(const QString& imageFileName) const
{
    bool loading = false;

    QListIterator< QPair<QString,int> > requests(mRequestQueue);
    while(requests.hasNext()) {
        if (requests.next().first==imageFileName) {
            loading = true;
            break;
        }
    }

    return loading;
}

