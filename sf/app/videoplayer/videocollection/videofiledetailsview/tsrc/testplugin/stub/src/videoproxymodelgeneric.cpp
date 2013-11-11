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
* Description: Dummy VideoProxyModelGeneric class implementation
* 
*/
// INCLUDE FILES
#include "videoproxymodelgeneric.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric::VideoProxyModelGeneric(QObject *parent) :
QAbstractItemModel(parent),
mStartPlaybackIndex(TMPXItemId::InvalidId()),
mDeleteFileIndex(-1)
{
    reset();
}

// -----------------------------------------------------------------------------
// ~VideoProxyModelGeneric()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric::~VideoProxyModelGeneric()
{
    reset();
}

// -----------------------------------------------------------------------------
// lastIndex()
// -----------------------------------------------------------------------------
//
QModelIndex VideoProxyModelGeneric::lastIndex()
{
    return mLastIndex;
}

// -----------------------------------------------------------------------------
// lastIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelGeneric::lastId()
{
    return mLastId;
}

// -----------------------------------------------------------------------------
// dataAccessCount()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::dataAccessCount()
{
    return mDataAccessCount;
}

// -----------------------------------------------------------------------------
// reset()
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::reset()
{
    mLastIndex = QModelIndex();
    mLastId    = TMPXItemId::InvalidId();
    mDataAccessCount = 0;
    mRowCount = 0;
    mData.clear();
    mStartPlaybackIndex = TMPXItemId::InvalidId();
    mDeleteFileIndex = -1;
    mDatareturnsInvalid = false;
}

// -----------------------------------------------------------------------------
// setDataReturnInvalid()
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::setDataReturnInvalid(bool setInvalid)
{
    mDatareturnsInvalid = setInvalid;
}

// -----------------------------------------------------------------------------
// setData()
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::setData(int role, QVariant data)
{
    mData.insert(role, data);
}

// -----------------------------------------------------------------------------
// setRowCount()
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::setRowCount(int count)
{
    if ( count == mRowCount ) return;
    
    if ( count > mRowCount ) {
        beginInsertRows(QModelIndex(), mRowCount, count);
        mRowCount = count;
        endInsertRows();
    } else {
        beginRemoveRows(QModelIndex(), count, mRowCount);
        mRowCount = count;
        endRemoveRows();
    }
}

// -----------------------------------------------------------------------------
// rowCount()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::rowCount(const QModelIndex &parent ) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    
    return mRowCount;
}

// -----------------------------------------------------------------------------
// itemData()
// -----------------------------------------------------------------------------
//
QMap<int, QVariant> VideoProxyModelGeneric::itemData(const QModelIndex &index) const
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
// -----------------------------------------------------------------------------
// data()
// -----------------------------------------------------------------------------
//
QVariant VideoProxyModelGeneric::data(const QModelIndex & index, int role) const
{
    QVariant returnValue = QVariant();
    mLastIndex = index;
    mDataAccessCount++;
    if (index.isValid() && !mDatareturnsInvalid) 
    {
        returnValue = mData.value(role);
    }
    
    return returnValue;
}

// -----------------------------------------------------------------------------
// columnCount()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::columnCount(const QModelIndex & parent) const
{
    // according to Qt documentation if parent is valid this should return 0 if
    // implementing a table based implementation like this.
    if (parent.isValid())
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// -----------------------------------------------------------------------------
// index()
// -----------------------------------------------------------------------------
//
QModelIndex VideoProxyModelGeneric::index(int row, int column, const QModelIndex & /*parent*/) const
{
    return createIndex(row, column);
}

// -----------------------------------------------------------------------------
// parent()
// -----------------------------------------------------------------------------
//
QModelIndex VideoProxyModelGeneric::parent(const QModelIndex & /*index*/) const
{
    return QModelIndex();
}

// -----------------------------------------------------------------------------
// openItem()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::openItem(const TMPXItemId &index)
{
    mStartPlaybackIndex = index;
    return 0;
}
    
// -----------------------------------------------------------------------------
// deleteItems()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::deleteItems(const QModelIndexList &indexList)
{
    if(indexList.count() > 0)
    {
        mDeleteFileIndex = indexList.at(0).row();
        return 0;
    }
    else
    {
        mDeleteFileIndex = -1;
        return -1;
    }
    
}

// -----------------------------------------------------------------------------
// startPlaybackIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelGeneric::startPlaybackIndex()
{
    return mStartPlaybackIndex;
}     

// -----------------------------------------------------------------------------
// deleteFileIndex()
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::deleteFileIndex()
{
    return mDeleteFileIndex;
}

// -----------------------------------------------------------------------------
// sourceModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoProxyModelGeneric::sourceModel()
{
    return this;
}

// -----------------------------------------------------------------------------
// getMediaIdAtIndex()
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelGeneric::getMediaIdAtIndex(const QModelIndex &/*index*/)
{
	TMPXItemId id = TMPXItemId::InvalidId();
	id.iId1 = mLastIndex.row();
	id.iId2 = 0;
	return id;
}

// -----------------------------------------------------------------------------
//  VideoProxyModelGeneric::indexOfId()
// -----------------------------------------------------------------------------
//
QModelIndex VideoProxyModelGeneric::indexOfId(TMPXItemId id)
{    
	mLastId = id;
	mLastIndex = createIndex(id.iId1, 0);
	return mLastIndex; //rikki! createIndex(row, column)
}


// End of file
