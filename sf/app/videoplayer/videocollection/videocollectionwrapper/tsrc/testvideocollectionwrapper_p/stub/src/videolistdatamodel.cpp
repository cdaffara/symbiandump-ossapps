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
* Description: Stub VideoListDataModel class for VideoCollectionWrapperPrivate unit tests   
*/


#include "videolistdatamodel.h"

bool VideoListDataModel::mInitFails = false;

// -----------------------------------------------------------------------------
// VideoListDataModel
// -----------------------------------------------------------------------------
//
VideoListDataModel::VideoListDataModel(QObject *parent)
 : QAbstractItemModel(parent)
{
}

// -----------------------------------------------------------------------------
//  ~VideoListDataModel
// -----------------------------------------------------------------------------
//
VideoListDataModel::~VideoListDataModel()
{
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

// -----------------------------------------------------------------------------
// mediaIdAtIndex
// -----------------------------------------------------------------------------
//
int VideoListDataModel::mediaIdAtIndex(int /*index*/) const
{
    return 1;
}

// -----------------------------------------------------------------------------
// rowCount
// -----------------------------------------------------------------------------
//           
int VideoListDataModel::rowCount(const QModelIndex &/*parent*/) const
{
    return 0;
}

// -----------------------------------------------------------------------------
// itemData
// -----------------------------------------------------------------------------
//
QMap<int, QVariant> VideoListDataModel::itemData(const QModelIndex &/*index*/) const
{
    QMap<int, QVariant> itemData;
    
    return itemData;
}

// -----------------------------------------------------------------------------
// data
// -----------------------------------------------------------------------------
//   
QVariant VideoListDataModel::data(const QModelIndex & /*index*/, int /*role*/) const
{
    QVariant returnValue = QVariant();
    return returnValue;
}

// -----------------------------------------------------------------------------
// columnCount
// -----------------------------------------------------------------------------
//   
int VideoListDataModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 0;
}

// -----------------------------------------------------------------------------
// index
// -----------------------------------------------------------------------------
//
QModelIndex VideoListDataModel::index(int /*row*/, int /*column*/, const QModelIndex & /*parent*/) const
{
    return QModelIndex();
}

// -----------------------------------------------------------------------------
// parent
// -----------------------------------------------------------------------------
//   
QModelIndex VideoListDataModel::parent(const QModelIndex & /*index*/) const
{
    return QModelIndex();
}

// -----------------------------------------------------------------------------
// removeRows
// -----------------------------------------------------------------------------
// 
bool VideoListDataModel::removeRows( QModelIndexList /*list*/)
{
    return true;
}
//end of file


