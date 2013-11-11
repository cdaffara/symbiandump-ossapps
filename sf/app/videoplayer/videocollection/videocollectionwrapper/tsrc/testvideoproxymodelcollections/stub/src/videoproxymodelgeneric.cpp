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
* Description: VideoProxyModelGeneric implementation
*
*/

// Version : 

// INCLUDE FILES
#include <qstringlist.h>
#include <qset.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <vcxmyvideosdefs.h>

#include "videolistdatamodel.h"
#include "videocollectioncommon.h"
#include "videoproxymodelgeneric.h"

const TMPXItemId INVALID_ID = TMPXItemId::InvalidId();

int VideoProxyModelGeneric::mOpenItemCallCount = 0;
int VideoProxyModelGeneric::mGetOpenItemCallCount = 0;
int VideoProxyModelGeneric::mLessThanCallCount = 0;
int VideoProxyModelGeneric::mFilterAcceptsRowCallCount = 0;
int VideoProxyModelGeneric::mConnectSignalsCallCount = 0;
int VideoProxyModelGeneric::mDisconnectSignalsCallCount = 0;
int VideoProxyModelGeneric::mProcessSortingCallCount = 0;

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric::VideoProxyModelGeneric(QObject *parent) : 
    mModel(0), 
    mCollectionClient(0),
    mLevel(VideoCollectionCommon::ELevelInvalid),
    mWantedSortOrder(Qt::AscendingOrder),
    mDefaultSortRole(0)        
{
    Q_UNUSED(parent);
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::~VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric::~VideoProxyModelGeneric()
{
    delete mModel;
    mModel = 0;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::initialize
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::initialize(VideoListDataModel *sourceModel)
{
    return 0;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::openItem
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::openItem(TMPXItemId mediaId)
{
    Q_UNUSED(mediaId);
    mOpenItemCallCount++;
    return -1;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::getOpenItem()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelGeneric::getOpenItem() const
{
	mGetOpenItemCallCount++;
    TMPXItemId itemId = TMPXItemId::InvalidId();
    return itemId;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::lessThan
// -----------------------------------------------------------------------------
//
bool VideoProxyModelGeneric::lessThan(const QModelIndex &left,
    const QModelIndex &right) const
{
    Q_UNUSED(left);
    Q_UNUSED(right);
    mLessThanCallCount++;
    return true;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::filterAcceptsRow
// -----------------------------------------------------------------------------
//
bool VideoProxyModelGeneric::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_row);
    Q_UNUSED(source_parent);
    mFilterAcceptsRowCallCount++;
    return false;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::connectSignals
// -----------------------------------------------------------------------------
//
bool VideoProxyModelGeneric::connectSignals()
{
    mConnectSignalsCallCount++;
    return true;
}
   
// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::disconnectSignals
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::disconnectSignals()
{
    mDisconnectSignalsCallCount++;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::processSorting
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::processSorting()
{
    mProcessSortingCallCount++;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::sourceModel
// -----------------------------------------------------------------------------
//
const VideoListDataModel *VideoProxyModelGeneric::sourceModel() const
{
    return mModel;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::setGenericIdFilter
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::setGenericIdFilter(const TMPXItemId &itemId, bool filterValue)
{
    Q_UNUSED(itemId);    
    Q_UNUSED(filterValue);
}
    
// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::setAlbumInUse
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::setAlbumInUse(const TMPXItemId &albumId)
{
    Q_UNUSED(albumId);
}

// End of file
