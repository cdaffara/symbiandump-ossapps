/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Stub collection client to be used when unit testing videocollectionsortfilterproxy 
*/



// INCLUDE FILES>
#include <mpxmediaarray.h>
#include "stub/inc/videocollectionclient.h"

VideoCollectionClient* VideoCollectionClient::testInstance = 0;

bool VideoCollectionClient::mFailInit = false;
bool VideoCollectionClient::mFailStartOpen = false;
bool VideoCollectionClient::mFailMediaPlayback = false;
bool VideoCollectionClient::mFailMediaDetails = false;
bool VideoCollectionClient::mFailSetSort = false;
bool VideoCollectionClient::mFailAddNewCollection = false;
TMPXItemId VideoCollectionClient::mNewAlbumId = TMPXItemId::InvalidId();
TVcxMyVideosSortingOrder VideoCollectionClient::mSettedSortOrder = EVcxMyVideosSortingNone;
int VideoCollectionClient::mSortOrderSetCount = 0;
QString VideoCollectionClient::mAddNewCollectionName = QString();
QString VideoCollectionClient::mAddNewCollectionThumb = QString();
QList<TMPXItemId> VideoCollectionClient::mAddNewCollectionIds = QList<TMPXItemId>();
int VideoCollectionClient::mRenameItemReturnValue = 0;
int VideoCollectionClient::mBackReturnValue = 0;

// -----------------------------------------------------------------------------
// VideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::VideoCollectionClient()
{
    testInstance = this;
}

// -----------------------------------------------------------------------------
// ~CVideoCollectionClient
// -----------------------------------------------------------------------------
//
VideoCollectionClient::~VideoCollectionClient()
{
}

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::initialize()
{
    if(mFailInit)
    {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// startOpenCollection
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::startOpenCollection(int level)
{
    Q_UNUSED(level);
    if(mFailStartOpen)
    {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// openItem
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::openItem(int /*mpxId1*/)
{
    if(mFailMediaPlayback)
    {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// addNewAlbum
// -----------------------------------------------------------------------------
//
TMPXItemId VideoCollectionClient::addNewAlbum(QString title)
{
    mAddNewCollectionName = title;
    return mNewAlbumId;
}

// -----------------------------------------------------------------------------
// addItemsInAlbum
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::addItemsInAlbum(TMPXItemId albumId, QList<TMPXItemId> items)
{
    Q_UNUSED(albumId);
    Q_UNUSED(items);
    return 0;
}

// -----------------------------------------------------------------------------
// revoveAlbums
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::removeAlbums(const QList<TMPXItemId> &items)
{
    Q_UNUSED(items);
    return 0;
}

// -----------------------------------------------------------------------------
// deleteFile
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::getVideoDetails(int /*mpxId1*/)
{
    if(mFailMediaDetails)
    {
        return -1;
    }
    return 0;
}

// -----------------------------------------------------------------------------
// getCategoryId
// -----------------------------------------------------------------------------
//
void VideoCollectionClient::getCategoryId(TMPXItemId &id)
{
    id.iId1 = 1;
    id.iId2 = 2;
}

// -----------------------------------------------------------------------------
// back
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::back()
{
    return mBackReturnValue;
}

// -----------------------------------------------------------------------------
// renameItem
// -----------------------------------------------------------------------------
//
int VideoCollectionClient::renameItem(const TMPXItemId &itemId, const QString &newTitle)
{
    Q_UNUSED(itemId);
    Q_UNUSED(newTitle);
    return mRenameItemReturnValue;
}
