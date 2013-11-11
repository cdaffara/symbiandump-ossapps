/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QString>

#include "cxutils.h"
#include "thumbnailmanager_qt.h"




/*!
* ThumbnailManager::ThumbnailManager
*/
ThumbnailManager::ThumbnailManager(QObject* parentPtr)
    : mTimer(this)
{
    Q_UNUSED(parentPtr);

    mCurrentThumbnailId = 0;
    mThumbnailManagerIds.clear();
    mTimer.setSingleShot(true);
    connect(&mTimer, SIGNAL(timeout()),
            this, SLOT(emulateThumbnailReady()));
}


/*!
* ThumbnailManager::~ThumbnailManager
*/
ThumbnailManager::~ThumbnailManager()
{
}



/*!
* create thumbnail for the given image/video file name and data.
*/
int ThumbnailManager::getThumbnail(const QString& filename, void * clientData, int priority)
{
    CX_DEBUG_ENTER_FUNCTION();
    Q_UNUSED(clientData);
    Q_UNUSED(priority);

    int id = 0;

    if (filename.isNull() || filename.isEmpty()) {
        id = -1;
    } else {
        // generate thumbnail id
        id = mCurrentThumbnailId;
        mThumbnailManagerIds.append(mCurrentThumbnailId++);
    }

    // try emulating thumbnail ready
    mTimer.start(1000);

    CX_DEBUG_EXIT_FUNCTION();

    return id;
}

int ThumbnailManager::setThumbnail( const QImage& source, const QString& fileName,
      void *clientData, int priority)
{
    Q_UNUSED(clientData);
    Q_UNUSED(priority);
    Q_UNUSED(source);

    int status = KErrNone;

    if (fileName.isNull() || fileName.isEmpty()) {
        status = KErrNotFound;
    } else {
        // generate thumbnail id
        mThumbnailManagerIds.append(mCurrentThumbnailId);
    }

    emit thumbnailReady(QPixmap(), clientData, mCurrentThumbnailId, status);

    mCurrentThumbnailId++;

    CX_DEBUG_EXIT_FUNCTION();

    return mCurrentThumbnailId;
}

/*!
* start canceling creating thumbnail operation
*/
bool ThumbnailManager::cancelRequest(int id)
{
    return mThumbnailManagerIds.contains(id);
}


/*!
* slot that emulates thumbnail ready
*/
void ThumbnailManager::emulateThumbnailReady()
{
    // get the current thumbnail id
    int id = mCurrentThumbnailId - 1;
    int status = KErrNone;

    if (id == -1) {
        // if there are no valid thumbnails
        status = KErrNotFound;
    }
    emit thumbnailReady(QPixmap(), 0, id, status);
}

