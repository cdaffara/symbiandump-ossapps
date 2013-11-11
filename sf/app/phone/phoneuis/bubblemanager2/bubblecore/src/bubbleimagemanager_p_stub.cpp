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
* Description: Stub implementation for desktop environment.
*
*/

#include <QtCore>
#include <QImageReader>
#include <QTimer>
#include "bubbleimagemanager.h"
#include "bubbleimagemanager_p.h"

BubbleImageManagerPrivate::BubbleImageManagerPrivate(
    BubbleImageManager* parent)
    : q_ptr(parent), mInitialized(false)
{
}

BubbleImageManagerPrivate::~BubbleImageManagerPrivate()
{
}

void BubbleImageManagerPrivate::initialize()
{
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

    qDebug() << "loading image: " << imageFileName;

    mRequestQueue.enqueue(qMakePair(imageFileName,0));
    QTimer::singleShot(100,this,SLOT(thumbnailReady()));
}

void BubbleImageManagerPrivate::unloadImage(const QString& imageFileName)
{
    qDebug() << "unloading image: " << imageFileName;

    // returns null, if key doesn't exists
    QPixmap* pixmap = mImageCache.take(imageFileName);
    delete pixmap;

    // cancel possible outstanding request
    QMutableListIterator< QPair<QString,int> > requests(mRequestQueue);
    while(requests.hasNext()) {
        if (requests.next().first==imageFileName) {
            qDebug() << "cancelling request: " << imageFileName;
            //requests.remove();
            requests.value().second = 1; // mark as cancelled
            break;
        }
    }
}

void BubbleImageManagerPrivate::releasePixmaps()
{
    qDebug() << "releasing all pixmaps: " << mImageCache.count();
    // release all cached pixmaps
    QList<QPixmap*> pixmaps = mImageCache.values();
    qDeleteAll(pixmaps);
    mImageCache.clear();
}

void BubbleImageManagerPrivate::thumbnailReady(
    const QPixmap& pixmap, void *data, int id, int error)
{
    Q_UNUSED(pixmap);
    Q_UNUSED(data);
    Q_UNUSED(id);
    Q_UNUSED(error);
}

void BubbleImageManagerPrivate::thumbnailReady()
{
    QPair<QString,int> request = mRequestQueue.dequeue();

    if (request.second==1) {
        // canceled
        return;
    }

    QString readFile = request.first;
    qDebug() << "reading: " << readFile;

    QImageReader reader(readFile);
    QSize size(reader.size());
    QImage *image = new QImage();
    if (reader.read(image)) {
        QPixmap *pixmap = new QPixmap();
        *pixmap = QPixmap::fromImage(*image);
        mImageCache.insert(readFile,pixmap);
        emit  q_ptr->pixmapReady(readFile);
    }
    delete image;
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

