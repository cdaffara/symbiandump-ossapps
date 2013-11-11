/*!
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

#include <QImage>
#include <QPixmap>
#include "bubbleimagemanager.h"

QPixmap* _pixmap;

BubbleImageManager::BubbleImageManager(QObject* parent)
    : QObject(parent), d_ptr(0)
{
    _pixmap = new QPixmap;
}

BubbleImageManager::~BubbleImageManager()
{
    delete _pixmap;
}

const QPixmap* BubbleImageManager::pixmap(
    const QString& imageFileName)
{
    return _pixmap;
}

void BubbleImageManager::loadImage(const QString& imageFileName)
{
    QImage image(imageFileName);
    _pixmap->fromImage(image);

    emit pixmapReady(imageFileName);
}

void BubbleImageManager::unloadImage(const QString& imageFileName)
{
}

void BubbleImageManager::releasePixmaps()
{
}
