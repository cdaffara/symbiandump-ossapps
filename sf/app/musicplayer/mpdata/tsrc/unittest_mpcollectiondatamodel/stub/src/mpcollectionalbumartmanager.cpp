


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
* Description: Music Player collection album art manager.
*
*/

#include "stub/inc/mpcollectionalbumartmanager.h"

int gAlbumInitCounter = 0;

/*!
 Returns gAlbumInitCounter.
 gAlbumInitCounter counts the number of constructor/destructor calls.
 */
int MpCollectionAlbumArtManager::getInitCounter()
{
    return gAlbumInitCounter;
}

/*!
 Resets gAlbumInitCounter to zero.
 */
void MpCollectionAlbumArtManager::resetInitCounter()
{
    gAlbumInitCounter = 0;
}

/*!
 Stub function.
 */
MpCollectionAlbumArtManager::MpCollectionAlbumArtManager( MpMpxCollectionData *data, QObject *parent )
    : QObject(parent),
      mIcon(0),
      mAlbumArtCount(0),
      mCacheAlbumArtCount(0)
{
    Q_UNUSED(data);
    gAlbumInitCounter++;
    mIcon = new QIcon(":/testicons/default_album.png");
}

/*!
 Stub function.
 */
MpCollectionAlbumArtManager::~MpCollectionAlbumArtManager()
{
    gAlbumInitCounter--;
    delete mIcon;
}

/*!
 Stub function.
 */
const QIcon* MpCollectionAlbumArtManager::albumArt( int index )
{
    Q_UNUSED(index);
    mAlbumArtCount++;
    return mIcon;
}

/*!
 Stub function.
 */
void MpCollectionAlbumArtManager::cacheFirstScreen()
{
    mCacheAlbumArtCount++;
}

/*!
 Stub function.
 */
void MpCollectionAlbumArtManager::cancel()
{
}

/*!
 Stub function.
 */
void MpCollectionAlbumArtManager::setThumbnailSize( MpCommon::MpThumbType type )
{
    mThumbType = type;
}

/*!
 Stub function.
 */
void MpCollectionAlbumArtManager::enableDefaultArt( bool enabled )
{
    Q_UNUSED(enabled);
}
