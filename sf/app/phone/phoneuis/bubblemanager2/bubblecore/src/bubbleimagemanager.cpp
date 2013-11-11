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
#include "bubbleimagemanager.h"
#include "bubbleimagemanager_p.h"

BubbleImageManager::BubbleImageManager(QObject* parent)
    : QObject(parent),
      d_ptr(new BubbleImageManagerPrivate(this))
{
}

BubbleImageManager::~BubbleImageManager()
{
    delete d_ptr;
}

const QPixmap* BubbleImageManager::pixmap(
    const QString& imageFileName)
{
    return d_ptr->pixmap(imageFileName);
}

void BubbleImageManager::loadImage(const QString& imageFileName)
{
    d_ptr->loadImage(imageFileName);
}

void BubbleImageManager::unloadImage(const QString& imageFileName)
{
    d_ptr->unloadImage(imageFileName);
}

void BubbleImageManager::releasePixmaps()
{
    d_ptr->releasePixmaps();
}

