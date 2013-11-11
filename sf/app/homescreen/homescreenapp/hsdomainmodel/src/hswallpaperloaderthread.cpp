/*
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

#include <QImageReader>
#include <QRect>

#include "hswallpaperloaderthread.h"


/*!
    \class HsWallpaperLoaderThread
    \ingroup group_hsdomainmodel
    \brief 
*/

/*!

*/
HsWallpaperLoaderThread::HsWallpaperLoaderThread(QObject *parent)
  : QThread(parent),
    mResult(false)
{
}

/*!

*/
HsWallpaperLoaderThread::~HsWallpaperLoaderThread()
{
}

/*!
    \internal
    Scales and crops source image using target rect.
    Centers target rect automatically.
    Saves scaled and cropped image as given target.
    Result can be read when finished by using result().
*/
void HsWallpaperLoaderThread::run() 
{
    mResult = true;
    QImageReader imageReader(mSourcePath);
    QRect clipRect(QPoint(), mTargetSize);
    
    if (imageReader.canRead()) {
        QSize sourceSize = imageReader.size();
        
        //Scale image's size to target's dimensions
        sourceSize.scale(clipRect.width(), clipRect.height(),
                         Qt::KeepAspectRatioByExpanding);
        imageReader.setScaledSize(sourceSize);

        //Crop center part of the scaled image as a target image
        clipRect.moveCenter(QPoint(sourceSize.width() / 2 - 1, sourceSize.height() / 2 - 1));
        imageReader.setScaledClipRect(clipRect);

        mResult = imageReader.read().save(mTargetPath);
    } else {
        mResult = false;
    }
}
