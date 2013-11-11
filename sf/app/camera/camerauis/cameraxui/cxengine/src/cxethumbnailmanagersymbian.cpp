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
#include <thumbnailmanager_qt.h>

#include "cxutils.h"
#include "cxethumbnailmanagersymbian.h"




/*!
* CxeThumbnailManagerSymbian::CxeThumbnailManagerSymbian
*/
CxeThumbnailManagerSymbian::CxeThumbnailManagerSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    mThumbnailManager = new ThumbnailManager();

    // connect thumbnail ready signal from thumbnailmanager
    connect(mThumbnailManager, SIGNAL(thumbnailReady(QPixmap, void *, int, int)),
            this, SLOT(thumbnailReady(QPixmap, void *, int, int)));

    CX_DEBUG_EXIT_FUNCTION();
}


/*!
* CxeThumbnailManagerSymbian::~CxeThumbnailManagerSymbian
*/
CxeThumbnailManagerSymbian::~CxeThumbnailManagerSymbian()
{
    CX_DEBUG_ENTER_FUNCTION();

    mThumbnailRequests.clear();
    delete mThumbnailManager;

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* Creates a thumbnail based on the snapshot data.
@param filename - name of the image/video filename
@param snapshot - snapshot data from image/video
*/
void CxeThumbnailManagerSymbian::createThumbnail(const QString &filename,
                                                 const QImage &snapshot)
{
    CX_DEBUG_ENTER_FUNCTION();

    TPtrC16 fName(reinterpret_cast<const TUint16*>(filename.utf16()));
    CX_DEBUG(("Create thumbnail! filename = %s", filename.toAscii().constData()));

    if (mThumbnailManager) {
        CX_DEBUG(("creating thumbnails"));
        int thumbnailId = mThumbnailManager->setThumbnail(snapshot, filename);
        if (thumbnailId != -1) {
            CX_DEBUG(("Thumbnail ID = %d", thumbnailId));
            mThumbnailRequests.insert(filename, thumbnailId);
        } else {
            CX_DEBUG(("error initializing data to thumbnail manager"));
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}



/*!
* start canceling creating thumbnail operation
*/
void CxeThumbnailManagerSymbian::cancelThumbnail(const QString& filename)
{
    CX_DEBUG_ENTER_FUNCTION();

    if (mThumbnailRequests.contains(filename)) {
        int thumbnailId = mThumbnailRequests.value(filename);
        if (mThumbnailManager &&
            mThumbnailManager->cancelRequest(thumbnailId)) {
            // thumbnail creation cancelled sucessfully
            mThumbnailRequests.remove(filename);
        }
    }

    CX_DEBUG_EXIT_FUNCTION();
}


/**!
* Slot that notifies when final thumbnail bitmap generation or loading is complete.
* @param pixmap     An object representing the resulting thumbnail.
* @param clientData Client data
* @param id         Request ID for the operation
* @param errorCode  error code
*/
void CxeThumbnailManagerSymbian::thumbnailReady(QPixmap thumbnail, void * data, int id, int error)
{

    CX_DEBUG_ENTER_FUNCTION();

    Q_UNUSED(thumbnail);
    Q_UNUSED(data);
    Q_UNUSED(error);

    CX_DEBUG(("CxeThumbnailManagerSymbian::thumbnailReady error = %d", error));

    QString key;
    QHash<QString, int>::const_iterator i = mThumbnailRequests.constBegin();

    while (i != mThumbnailRequests.constEnd()) {
        if (i.value() == id) {
            key = i.key();
            break;
        }
        ++i;
    }

    if (mThumbnailRequests.contains(key)) {
        CX_DEBUG(("Thumbnail created for filename = %s", key.toAscii().constData()));
        mThumbnailRequests.remove(key);
    }

    CX_DEBUG_EXIT_FUNCTION();
}
