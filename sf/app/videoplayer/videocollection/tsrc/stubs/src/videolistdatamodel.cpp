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
* Description: VideoListDataModel class implementation
* 
*/

#include <qstringlist.h>

#include "videolistdatamodel.h"
#include "videolistdatamodeldata.h"

QMap<int, QVariant> VideoListDataModelData::mData;
QModelIndex VideoListDataModelData::mLastIndex;
int VideoListDataModelData::mDataAccessCount = 0;
int VideoListDataModelData::mRowCount = 0;
int VideoListDataModelData::mRowCountDecrement = 0;
TMPXItemId VideoListDataModelData::mItemId = TMPXItemId::InvalidId();



VideoListDataModel::VideoListDataModel(QObject *parent) :
    QAbstractItemModel(parent),
    d_ptr(0), 
    mCollectionClient(0),
    mDeleteWorker(0),
    mInitialized(false)
{
    // nop
}

VideoListDataModel::~VideoListDataModel()
{
    VideoListDataModelData::reset();
}

int VideoListDataModel::initialize()
{
    if (VideoListDataModelData::mRowCount)
    {
        int newCount = VideoListDataModelData::mRowCount;
        int oldCount = VideoListDataModelData::mData.count();
        if(newCount == oldCount)
        {
            return 0;
        }
        if (newCount < oldCount)
        {
            beginRemoveRows(QModelIndex(), newCount, oldCount);
            endRemoveRows();
        }
        else
        {
            beginInsertRows(QModelIndex(), oldCount, newCount-1);
            endInsertRows();
        }
    }
    else
    {
        reset();
    }
    return 0;
}

VideoCollectionClient* VideoListDataModel::getCollectionClient()
{
    // not stubbed
    return 0;
}
 
void VideoListDataModel::connectSignals()
{
    // not stubbed
}

void VideoListDataModel::disconnectSignals()
{
    // not stubbed
}

const TMPXItemId& VideoListDataModel::mediaIdAtIndex(const int &index) const
{
    Q_UNUSED(index);
    // not stubbed
    return VideoListDataModelData::mItemId;
}

QString VideoListDataModel::mediaFilePathForId(const TMPXItemId &mediaId) const
{
    Q_UNUSED(mediaId);
    // not stubbed
    return QString();
}

int VideoListDataModel::rowCount(const QModelIndex &parent ) const
{
    Q_UNUSED(parent);

    uint count = VideoListDataModelData::mRowCount - VideoListDataModelData::mRowCountDecrement;
    if(VideoListDataModelData::mRowCountDecrement > 0)
    {
        VideoListDataModelData::mRowCountDecrement--;
    }
    return count;
}

QMap<int, QVariant> VideoListDataModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> itemData;
    if (index.isValid()) 
    {
        // returns only basic data of the item
        itemData.insert(Qt::DisplayRole, data(index, Qt::DisplayRole));
        itemData.insert(Qt::DecorationRole, data(index, Qt::DecorationRole)); 
        itemData.insert(Qt::BackgroundRole, data(index, Qt::BackgroundRole)); 
    }
    return itemData;
}

QString VideoListDataModel::prepareDetailRow(int &index) const
{
    Q_UNUSED(index);
    
    // not stubbed
    return QString();
}

QString VideoListDataModel::prepareSizeString(int &index) const
{
    Q_UNUSED(index);
    
    // not stubbed
    return QString();
}

QVariant VideoListDataModel::data(const QModelIndex &index, int role) const
{
    QVariant returnValue = QVariant();
    VideoListDataModelData::mLastIndex = index;
    VideoListDataModelData::mDataAccessCount++;
    if (index.isValid()) 
    {
        returnValue = VideoListDataModelData::mData.value(role);
    }
    
    return returnValue;
}

int VideoListDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    
    return 1;
}

QModelIndex VideoListDataModel::index(int row,
    int column,
    const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    
    return createIndex(row, column);
}

QModelIndex VideoListDataModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    
    // not stubbed
    return QModelIndex();
}

bool VideoListDataModel::removeRows(const QModelIndexList &indexList)
{
    Q_UNUSED(indexList);
    
    // not stubbed
    return false;
}

bool VideoListDataModel::belongsToAlbum(const TMPXItemId &itemId, TMPXItemId albumId) const
{
    Q_UNUSED(albumId);
    Q_UNUSED(itemId);
    
    // not stubbed
    return false;
}

void VideoListDataModel::setAlbumInUse(const TMPXItemId &albumId)
{
    Q_UNUSED(albumId);
    
    // not stubbed
}

void VideoListDataModel::deleteStartingFailsSlot(QList<TMPXItemId>& ids)
{
    Q_UNUSED(ids);
    
    // not stubbed
}

void VideoListDataModel::reportAsyncStatus(int statusCode, QVariant &additional)
{
    Q_UNUSED(statusCode);
    Q_UNUSED(additional);
    
    // not stubbed
}

// end of file
