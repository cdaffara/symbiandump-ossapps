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
* Description: MpMpxCollectionData stub for testing mpcollectioncontainers
*
*/

#include "mptrace.h"
#include "stub/inc/mpmpxcollectiondata.h"
#include <mpxmedia.h>


/*!
 Stub function.
 */
MpMpxCollectionData::MpMpxCollectionData( QObject *parent )
    : QObject(parent),
      mContainerMedia(0),
      mMediaSet(false),
      mContext(ECollectionContextUnknown),
      mCount(0),
      mCurrentAlbumIndex(-1),
      mCurrentAlbumAvailable(false),
      mReturnCollectionTitle(true),
      mArbitraryAlbumSongId(0),
      mAlbumSongProperty(false)
{
}

/*!
 Stub function.
*/
MpMpxCollectionData::~MpMpxCollectionData()
{
}

/*!
 Stub function.
*/
TCollectionContext MpMpxCollectionData::context()
{
    return mContext;
}

/*!
 Stub function.
 */
int MpMpxCollectionData::count() const
{
    return mCount;
}

/*!
 Stub function.
 */
QString MpMpxCollectionData::collectionTitle() const
{
    if ( mReturnCollectionTitle ) {
        return QString("Title");
    }
    else {
        return QString();
    }
}

/*!
 Stub function.
*/
QString MpMpxCollectionData::itemData( int index, MpMpxCollectionData::DataType type ) const
{
    Q_UNUSED(index);
    if ( type == MpMpxCollectionData::Uri ) {
        return QString("Uri");
    }
    else {
        return QString();
    }
}

/*!
 Stub function.
*/
bool MpMpxCollectionData::setCurrentAlbum( int index )
{
    mCurrentAlbumIndex = index;
    return mCurrentAlbumAvailable;
}

/*!
 Stub function.
*/
int MpMpxCollectionData::albumSongId(int index)
{
    Q_UNUSED(index);
    return mArbitraryAlbumSongId;
}
bool MpMpxCollectionData::hasAlbumSongProperty( int index, MpMpxCollectionData:: DataProperty type ) const
{
    Q_UNUSED(index);
    Q_UNUSED(type);
    return mAlbumSongProperty;
}

/*!
 Stub function.
*/
const CMPXMedia& MpMpxCollectionData::containerMedia()
{
    return *mContainerMedia;
}

void MpMpxCollectionData::setContext( TCollectionContext context )
{
    TX_LOG_ARGS("MpMpxCollectionData::setContext() Stub" << context)
    mContext=context;
}

int MpMpxCollectionData::itemIndex( int itemUniqueId )
{
    Q_UNUSED(itemUniqueId);
    return 0;
}

void MpMpxCollectionData::setCount(int count)
    {
    mCount=count;
    }
