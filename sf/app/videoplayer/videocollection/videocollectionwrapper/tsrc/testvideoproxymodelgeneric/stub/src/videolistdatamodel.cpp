/**
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
* Description: Stub model to be used when unit testing proxy model. 
* 
*/


// INCLUDES
#include "videocollectionclient.h"
#include "videolistdatamodel.h"

bool VideoListDataModel::mInitFails = false;
bool VideoListDataModel::mRemoveRowsFails = false;
bool VideoListDataModel::mGetMediaIdAtIndexFails = false;
bool VideoListDataModel::mBelongsToAlbum = false;
bool VideoListDataModel::mReturnInvalid = false;
int VideoListDataModel::mRemoveFrAlbumReturn = -1;
int VideoListDataModel::mLastDeletedIndexRow = -1;
QString VideoListDataModel::mMediaFilePathReturnValue = "";
bool VideoListDataModel::mGetCollectionClientFails = false;

TMPXItemId gInvalidId  = TMPXItemId::InvalidId();

// ---------------------------------------------------------------------------
// VideoListDataModel
// ---------------------------------------------------------------------------
//
VideoListDataModel::VideoListDataModel(QObject *parent) :
QAbstractItemModel(parent),
 mCollectionClient(0)
{
    
}

// ---------------------------------------------------------------------------
// ~VideoListDataModel
// ---------------------------------------------------------------------------
//
VideoListDataModel::~VideoListDataModel()
{
    removeAll();
   
    delete mCollectionClient;
}

// -----------------------------------------------------------------------------
// initialize
// -----------------------------------------------------------------------------
//
int VideoListDataModel::initialize()
{
    if(mInitFails)
    {
        return -1;
    }
    return 0;
}

// ---------------------------------------------------------------------------
// mediaIdAtIndex
// ---------------------------------------------------------------------------
//
TMPXItemId &VideoListDataModel::mediaIdAtIndex(int index) const
{
	if( mGetMediaIdAtIndexFails || index < 0 || index >= mData.count())
	{
		return gInvalidId;
	}
	
	return mData.at(index)->mId;
}

// ---------------------------------------------------------------------------
// mediaFilePathForId
// ---------------------------------------------------------------------------
//
QString VideoListDataModel::mediaFilePathForId(int mediaId) const
{
    Q_UNUSED(mediaId);
    return mMediaFilePathReturnValue;
}

// ---------------------------------------------------------------------------
// removeRows
// ---------------------------------------------------------------------------
//
bool VideoListDataModel::removeRows(const QModelIndexList &indexList)
{
    mLastDeletedIndexRow = -1;
    if( mRemoveRowsFails )
    	return false;
    else
    {
        if(indexList.count() > 0)
        {
            mLastDeletedIndexRow = indexList.at(indexList.count() - 1).row();
        }
        QModelIndexList sortable(indexList);
        qSort(sortable);
        QModelIndexList::const_iterator iter = sortable.constEnd();
        QModelIndex index;
        while(iter != sortable.constBegin())
        {
            iter--;
            index = (*iter); 
            beginRemoveRows(QModelIndex(), index.row(), index.row());

            delete mData.at(index.row());
            mData.removeAt(index.row());
            
            endRemoveRows();
        }
        
    	return true;
    }
}

// ---------------------------------------------------------------------------
// getCollectionClient
// ---------------------------------------------------------------------------
//
VideoCollectionClient* VideoListDataModel::getCollectionClient()
{
    if(mGetCollectionClientFails)
    {
        return 0;
    }
    
    if(!mCollectionClient)
    {
        mCollectionClient = new VideoCollectionClient();
    }
    return mCollectionClient;
}

// ---------------------------------------------------------------------------
// rowCount
// ---------------------------------------------------------------------------
//
int VideoListDataModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return mData.count();
}

// ---------------------------------------------------------------------------
// itemData
// ---------------------------------------------------------------------------
//
QMap<int, QVariant> VideoListDataModel::itemData(const QModelIndex &index) const
{
    Q_UNUSED(index);
    QMap<int, QVariant> itemData;
        
    return itemData;
}
  
// ---------------------------------------------------------------------------
// data
// ---------------------------------------------------------------------------
//
QVariant VideoListDataModel::data(const QModelIndex & index, int role) const
{
    QVariant returnValue = QVariant();
    if(!index.isValid())
    {
        return returnValue;
    }
    if(mReturnInvalid)
    {
        return returnValue;
    }
    int row = index.row();
    if(role == VideoCollectionCommon::KeyTitle)
    {       
        if(row >= 0 && row < mData.count())
        {
            returnValue = mData.at(row)->mName;
        }
    }
    else if(role == VideoCollectionCommon::KeySizeValue || 
            role == VideoCollectionCommon::KeyNumberOfItems)
    {
        if(row >= 0 && row < mData.count())
        {
            returnValue = mData.at(row)->mSize;
        }
    }
    else if(role == VideoCollectionCommon::KeyDateTime)
    {
        if(row >= 0 && row < mData.count())
        {
            returnValue = mData.at(row)->mDate;
        }
    }
    else if(role == VideoCollectionCommon::KeyStatus)
    {
        if(row >= 0 && row < mData.count())
        {
            returnValue = mData.at(row)->mStatus;
        }
    }
    else if(role == INVALID_ROLE_FOR_SORTING)
    {
        if(row >= 0 && row < mData.count())
        {
            returnValue = mData.at(row)->mName;
        }
    }    
    return returnValue;    
}
 
// ---------------------------------------------------------------------------
// columnCount
// ---------------------------------------------------------------------------
//
int VideoListDataModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid())
   {
       return 0;
   }
   else
   {
       return 1;
   }
}

// ---------------------------------------------------------------------------
// index
// ---------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::index(int row, int column, const QModelIndex & parent) const
{
    Q_UNUSED(parent);
    if(row >= 0 && row < mData.count())
    {
        return createIndex(row, column);
    }
    return QModelIndex();
}
  
// ---------------------------------------------------------------------------
// parent
// ---------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::parent(const QModelIndex & index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

void VideoListDataModel::removeAll()
{
    beginRemoveRows(QModelIndex(), 0,0);
    QList<DummyData*>::iterator iter = mData.begin();
    while(iter != mData.end())
    {
        delete *iter;
        ++iter;
    }
    mData.clear();
    endRemoveRows();
}


// ---------------------------------------------------------------------------
// emitAlbumChanged
// ---------------------------------------------------------------------------
//
void VideoListDataModel::emitAlbumChanged()
{
    emit albumChanged();
}

// ---------------------------------------------------------------------------
// appendData
// ---------------------------------------------------------------------------
//
void VideoListDataModel::appendData(TMPXItemId data)
{
    DummyData* obj;
    for(int i = 0; i < mData.count(); ++i)
    {
        obj = mData.at(i);
        if(obj->mId == TMPXItemId::InvalidId() || obj->mId.iId1 == data.iId1)
        {
            obj->mId = data;
            QModelIndex change = index(i,0);
            emit dataChanged(change, change);
            return;
        }
    }
    beginInsertRows(QModelIndex(), mData.count(), mData.count());
    obj = new DummyData;
    obj->mId = data;
    mData.append(obj);
    endInsertRows();  
}


// ---------------------------------------------------------------------------
// appendData
// ---------------------------------------------------------------------------
//
void VideoListDataModel::appendData(QString data)
{
    DummyData* obj;
    for(int i = 0; i < mData.count(); ++i)
    {
        obj = mData.at(i);
        if(!obj->mName.length())
        {
            obj->mName = data;
            QModelIndex change = index(i,0);
            emit dataChanged(change, change);
            return;
        }
    }
    beginInsertRows(QModelIndex(), mData.count(), mData.count());
    obj = new DummyData;
    obj->mId = TMPXItemId( mData.count(), 0);
    obj->mName = data;
    mData.append(obj);
    endInsertRows();
}
 
// ---------------------------------------------------------------------------
// appendData
// ---------------------------------------------------------------------------
//
void VideoListDataModel::appendData(uint data)
{
    DummyData* obj;
    for(int i = 0; i < mData.count(); ++i)
    {
        obj = mData.at(i);
        if(obj->mSize == 666)
        {
            obj->mSize = data;
            QModelIndex change = index(i,0);
            emit dataChanged(change, change);
            return;
        }
    }
    beginInsertRows(QModelIndex(), mData.count(), mData.count());
    obj = new DummyData;
    obj->mId = TMPXItemId( mData.count(), 0);
    obj->mSize = data;
    mData.append(obj);
    endInsertRows();
}

// ---------------------------------------------------------------------------
// appendData
// ---------------------------------------------------------------------------
//
void VideoListDataModel::appendData(QDateTime data)
{
    DummyData* obj;
    for(int i = 0; i < mData.count(); ++i)
    {
        obj = mData.at(i);
        if(!obj->mDate.isValid())
        {
            obj->mDate = data;
            QModelIndex change = index(i,0);
            emit dataChanged(change, change);
            return;
        }
    }
    beginInsertRows(QModelIndex(), mData.count(), mData.count());
    obj = new DummyData;
    obj->mId = TMPXItemId( mData.count(), 0);
    obj->mDate = data;
    mData.append(obj);
    endInsertRows();
}

// ---------------------------------------------------------------------------
// appendStatus
// ---------------------------------------------------------------------------
//
void VideoListDataModel::appendStatus(int status)
{
    DummyData* obj;
    for(int i = 0; i < mData.count(); ++i)
    {
        obj = mData.at(i);
        if(obj->mStatus == -1)
        {
            obj->mStatus = status;
            QModelIndex change = index(i,0);
            emit dataChanged(change, change);
            return;
        }
    }
    beginInsertRows(QModelIndex(), mData.count(), mData.count());
    obj = new DummyData;
    obj->mId = TMPXItemId( mData.count(), 0);
    obj->mStatus = status;
    mData.append(obj);
    endInsertRows();
}

// ---------------------------------------------------------------------------
// setAlbumInUse
// ---------------------------------------------------------------------------
//
void VideoListDataModel::setAlbumInUse(TMPXItemId itemId)
{
    mAlbumInUse = itemId;
}

// ---------------------------------------------------------------------------
// albumInUse
// ---------------------------------------------------------------------------
//
TMPXItemId VideoListDataModel::albumInUse()
{
    return mAlbumInUse;
}

// ---------------------------------------------------------------------------
// belongsToAlbum
// ---------------------------------------------------------------------------
//
bool VideoListDataModel::belongsToAlbum(TMPXItemId itemId, TMPXItemId albumId )
{
    Q_UNUSED(itemId);
    Q_UNUSED(albumId);
    return mBelongsToAlbum;
}

// ---------------------------------------------------------------------------
// removeItemsFromAlbum
// ---------------------------------------------------------------------------
//
int VideoListDataModel::removeItemsFromAlbum(TMPXItemId &albumId, const QList<TMPXItemId> &items)
{
    Q_UNUSED(albumId);
    Q_UNUSED(items);
    return mRemoveFrAlbumReturn;
}

// ---------------------------------------------------------------------------
// indexOfId
// ---------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::indexOfId(TMPXItemId id)
{
    QModelIndex itemIndex;
    DummyData* obj;
    for(int i = 0; i < mData.count(); ++i)
    {
       obj = mData.at(i);
       if(obj->mId == id)
       {
           itemIndex = index(i,0,QModelIndex());
           break;
       }
    }
    return itemIndex;
}

// End of file
    


