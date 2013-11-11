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
* Description:   stub VideoProxyModelGeneric implementation
*
*/

#include "videoproxymodelgeneric.h"
#include "videoproxymodeldata.h"
#include "videocollectioncommon.h"
#include "videolistdatamodel.h"

using namespace VideoCollectionCommon;

int VideoProxyModelData::mSortRole = -1;
Qt::SortOrder VideoProxyModelData::mSortOrder = Qt::AscendingOrder;
bool VideoProxyModelData::mSortAsync = false;
bool VideoProxyModelData::mDeleteItemsFails = false;
bool VideoProxyModelData::mRemoveAlbumsFails = false;
int VideoProxyModelData::mDoSortingCallCount = 0;
bool VideoProxyModelData::mOpenFails = false;
QList<TMPXItemId> VideoProxyModelData::mItemIds = QList<TMPXItemId>();
QModelIndex VideoProxyModelData::mLastIndex = QModelIndex();
TMPXItemId VideoProxyModelData::mLastItemId = TMPXItemId::InvalidId();
TMPXItemId VideoProxyModelData::mOpenedItemId = TMPXItemId::InvalidId();
int VideoProxyModelData::mDetailsReturnValue = 0;
int VideoProxyModelData::mAddItemsInAlbumReturnValue = 0;
int VideoProxyModelData::mRemoveItemsFromAlbumReturnValue = 0;
TMPXItemId VideoProxyModelData::mGenericFilterId  = TMPXItemId::InvalidId();
bool VideoProxyModelData::mGenericFilterValue = false;
TMPXItemId VideoProxyModelData::mNewAlbumId = TMPXItemId::InvalidId();
QString VideoProxyModelData::mLastItemNameInRename = "";
int VideoProxyModelData::mRenameItemReturnValue = 0;
QString VideoProxyModelData::mLastAddedAlbumName = "";

const TMPXItemId INVALID_ID = TMPXItemId::InvalidId();

VideoProxyModelGeneric::VideoProxyModelGeneric(QObject *parent):
    QSortFilterProxyModel(parent),
    mModel(0),
    mCollectionClient(0),
    mLevel(VideoCollectionCommon::ELevelInvalid),
    mIdleSortTimer(0),
    mWantedSortRole(VideoCollectionCommon::KeyDateTime),
    mDefaultSortRole(VideoCollectionCommon::KeyTitle)
{

}

VideoProxyModelGeneric::~VideoProxyModelGeneric()
{
    VideoProxyModelData::reset();
}

int VideoProxyModelGeneric::initialize(VideoListDataModel *sourceModel)
{
    mModel = sourceModel;
    setSourceModel(mModel);
    return 0;
}

bool VideoProxyModelGeneric::connectSignals()
{
    // not stubbed
    return true;
}
   
void VideoProxyModelGeneric::disconnectSignals()
{
    // not stubbed
}

int VideoProxyModelGeneric::open(VideoCollectionCommon::TCollectionLevels level)
{
    int err = 0;
    
    mLevel = level;
    if (VideoProxyModelData::mOpenFails)
    {
        err = -1;
    }
    
    return err;
}

void VideoProxyModelGeneric::doSorting(int sortingRole,
    Qt::SortOrder order, 
    bool async)
{
    VideoProxyModelData::mDoSortingCallCount++;
    VideoProxyModelData::mSortRole = sortingRole;
    VideoProxyModelData::mSortOrder = order;
    VideoProxyModelData::mSortAsync = async;
    setSortRole(sortingRole);
    sort(0, order);
}

void VideoProxyModelGeneric::getSorting(int &sortingRole,
    Qt::SortOrder &order)
{
    sortingRole = VideoProxyModelData::mSortRole;
    order = VideoProxyModelData::mSortOrder;
}

int VideoProxyModelGeneric::deleteItems(const QModelIndexList &indexList)
{
    
    int err = 0;
    if (VideoProxyModelData::mDeleteItemsFails)
    {
        err = -1;
    }
    else
    {
        if (indexList.count() > 0)
        {
            VideoProxyModelData::mLastIndex = indexList.at(0);
        }
    }
    
    return err;
}

int VideoProxyModelGeneric::openItem(TMPXItemId mediaId)
{
    VideoProxyModelData::mLastItemId = mediaId;
    
    return 0;
}

int VideoProxyModelGeneric::back()
{
    // not stubbed
    return 0;
}

int VideoProxyModelGeneric::fetchItemDetails(const QModelIndex &index)
{
    VideoProxyModelData::mLastIndex = index;
    
    return VideoProxyModelData::mDetailsReturnValue;  
}

void VideoProxyModelGeneric::processSorting()
{
    // not stubbed
}

void VideoProxyModelGeneric::processSortingSlot()
{
    // not stubbed
}

bool VideoProxyModelGeneric::lessThan(const QModelIndex &left,
    const QModelIndex &right) const
{
    Q_UNUSED(left);
    Q_UNUSED(right);
    // not stubbed
    return true;
}

bool VideoProxyModelGeneric::filterAcceptsRow (int source_row,
    const QModelIndex &source_parent) const
{
    Q_UNUSED(source_row);
    Q_UNUSED(source_parent);
    // not stubbed
    return true;
}

 const TMPXItemId& VideoProxyModelGeneric::getMediaIdAtIndex(
    const QModelIndex &index) const
{
    if (index.row() >= 0 &&
        index.row() < VideoProxyModelData::mItemIds.count())
    {
        return VideoProxyModelData::mItemIds[index.row()];
    }
    return INVALID_ID;
}

QModelIndex VideoProxyModelGeneric::indexOfId(const TMPXItemId &id)
{
    //VideoListDataModel *sourceModel = qobject_cast<VideoListDataModel*>(sourceModel());

    for(int i = 0; i < VideoProxyModelData::mItemIds.count(); i++)
    {
        if(VideoProxyModelData::mItemIds.at(i) == id)
        {
            //return sourceModel->index(i, 0);
            return index(i, 0);
        }
    }
    return QModelIndex();
}

QString VideoProxyModelGeneric::getMediaFilePathForId(const TMPXItemId &mediaId)
{
    Q_UNUSED(mediaId);
    // not stubbed
    return QString();
}

TMPXItemId VideoProxyModelGeneric::addNewAlbum(const QString &title)
{
    VideoProxyModelData::mLastAddedAlbumName = title;
    return VideoProxyModelData::mNewAlbumId;
}

QString VideoProxyModelGeneric::resolveAlbumName(
    const QString& albumName) const
{
    QString name = albumName;
    return name;
}

int VideoProxyModelGeneric::addItemsInAlbum(TMPXItemId &albumId, const QList<TMPXItemId> &mediaIds)
{
    if(VideoProxyModelData::mAddItemsInAlbumReturnValue >= 0)
    {
        VideoProxyModelData::mLastItemId = albumId;
        VideoProxyModelData::mItemIds.clear();
        VideoProxyModelData::mItemIds = mediaIds;
    }
    // not stubbed
    return VideoProxyModelData::mAddItemsInAlbumReturnValue;
}

int VideoProxyModelGeneric::removeAlbums(const QModelIndexList &indexList)
{
    
    int err = 0;
    if (VideoProxyModelData::mRemoveAlbumsFails)
    {
        err = -1;
    }
    else
    {
        if (indexList.count() > 0)
        {
            VideoProxyModelData::mLastIndex = indexList.at(0);
        }
    }
    
    return err;
}

int VideoProxyModelGeneric::removeItemsFromAlbum(class TMPXItemId &album, QList<class TMPXItemId> const &mediaIds)
{
    if(VideoProxyModelData::mRemoveItemsFromAlbumReturnValue >= 0)
    {
        VideoProxyModelData::mLastItemId = album;
        VideoProxyModelData::mItemIds.clear();
        VideoProxyModelData::mItemIds = mediaIds;
    }
    return VideoProxyModelData::mRemoveItemsFromAlbumReturnValue;
}

void VideoProxyModelGeneric::setAlbumInUse(const TMPXItemId &albumId)
{
    VideoProxyModelData::mLastItemId = albumId;
}

int VideoProxyModelGeneric::renameItem(const TMPXItemId &itemId, const QString &newTitle)
{
    if(VideoProxyModelData::mRemoveItemsFromAlbumReturnValue >= 0)
    {
        VideoProxyModelData::mLastItemId = itemId;
        VideoProxyModelData::mLastItemNameInRename = newTitle;

    }
    return VideoProxyModelData::mRenameItemReturnValue;
}

TMPXItemId VideoProxyModelGeneric::getOpenItem() const
{
    return VideoProxyModelData::mOpenedItemId;
}

void VideoProxyModelGeneric::setGenericIdFilter(const TMPXItemId &itemId, bool filterValue)
{
    VideoProxyModelData::mGenericFilterId  = itemId;
    VideoProxyModelData::mGenericFilterValue = filterValue;
    return;
}

// end of file
