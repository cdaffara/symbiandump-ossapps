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
* Description:  Videolist sort filter proxy stub implementation
*
*/

#include <qstringlist.h>
#include <qdatetime.h>

#include "videocollectioncommon.h"
#include "videoproxymodelgeneric.h"

int VideoProxyModelGeneric::mRowCountCallCount = 0;
bool VideoProxyModelGeneric::mReturnInvalidIndexes = false;

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric::VideoProxyModelGeneric(QObject *parent) :
 QSortFilterProxyModel(parent)
{
    reset();
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::~VideoProxyModelGeneric
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric::~VideoProxyModelGeneric()
{
    disconnect();
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::lessThan
// -----------------------------------------------------------------------------
//
bool VideoProxyModelGeneric::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    Q_UNUSED(left);
    Q_UNUSED(right);
    return false;
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::getMediaIdAtIndex
// -----------------------------------------------------------------------------
//
TMPXItemId VideoProxyModelGeneric::getMediaIdAtIndex(QModelIndex index)
{
	return TMPXItemId(index.row(), 0);
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::getMediaFilePathForId()
// -----------------------------------------------------------------------------
//
QString VideoProxyModelGeneric::getMediaFilePathForId(TMPXItemId mediaId)
{
    if(mediaId.iId1 >= 0 && mediaId.iId1 < mFileNames.count())
    {
        return mFileNames.at(mediaId.iId1);
    }
    return "";
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::back
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::back()
{
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::reset
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::reset()
{

}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::appendData
// -----------------------------------------------------------------------------
//
void VideoProxyModelGeneric::appendData(QString fileName)
{
    mFileNames.append(fileName);
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::rowCount
// -----------------------------------------------------------------------------
//
int VideoProxyModelGeneric::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    VideoProxyModelGeneric::mRowCountCallCount++;
    return mFileNames.count();
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::index
// -----------------------------------------------------------------------------
//
QModelIndex VideoProxyModelGeneric::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if(mReturnInvalidIndexes)
        return QModelIndex();
    else
        return QModelIndex(createIndex(row, column));
}

// -----------------------------------------------------------------------------
// VideoProxyModelGeneric::mapToSource
// -----------------------------------------------------------------------------
//
QModelIndex VideoProxyModelGeneric::mapToSource(const QModelIndex &proxyIndex) const
{
    return QModelIndex(proxyIndex);
}

VideoProxyModelGeneric* VideoProxyModelGeneric::sourceModel()
{
    return this;
}

// end of file

