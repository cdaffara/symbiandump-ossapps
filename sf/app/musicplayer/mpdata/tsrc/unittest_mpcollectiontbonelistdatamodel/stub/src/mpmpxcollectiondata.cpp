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
* Description: MpMpxCollectionData stub for testing MpCollectionDataModel
*
*/

#include "mptrace.h"
#include "stub/inc/mpmpxcollectiondata.h"


int gInitCounter = 0;

/*!
 Returns gInitCounter.
 gInitCounter counts the number of constructor/destructor calls.
 */
int MpMpxCollectionData::getInitCounter()
{
    return gInitCounter;
}

/*!
 Resets gInitCounter to zero.
 */
void MpMpxCollectionData::resetInitCounter()
{
    gInitCounter = 0;
}


/*!
 Stub function.
 */
MpMpxCollectionData::MpMpxCollectionData( QObject *parent )
    : QObject(parent),
      mAlbumSongCount(0),
      mItemDataReturn(true)
{
    gInitCounter++;
}

/*!
 Stub function.
 */
MpMpxCollectionData::~MpMpxCollectionData()
{
    gInitCounter--;
}

/*!
 Stub function.
 */
int MpMpxCollectionData::albumSongsCount() const
{
    return mAlbumSongCount;
}

/*!
 Stub function.
 */
int MpMpxCollectionData::albumSongId( int index )
{
    Q_UNUSED(index);
    return 0;
}

/*!
 Stub function.
 */
QString MpMpxCollectionData::albumSongData( int index, MpMpxCollectionData::DataType type ) const
{
    QString data;
    if ( !mItemDataReturn ) {
        return data;
    }
    switch ( type ) {
        case MpMpxCollectionData::Title:
            data = QString("Title");
            data.append( QString().setNum(index) );
            break;
        default:
            qFatal("MpMpxCollectionData::itemData called with unknown type");
            break;
    }
    return data;
}

/*!
 Stub function.
 */
bool MpMpxCollectionData::hasAlbumSongProperty( int index, MpMpxCollectionData:: DataProperty type )
{
    TX_ENTRY
    Q_UNUSED(type);
    if (index == 2){
    mHasAlbumSongProperty = true;
    return true;
    }
    else {
    mHasAlbumSongProperty = false;
    return false;
    }
}

/*!
 Stub function.
 */
int MpMpxCollectionData::albumSongIndex( int songUniqueId )
{
    if (songUniqueId == 0){
        return -1;
    }
    else {
        return songUniqueId;
    }
      
}

/*!
 Stub function.
 */
void MpMpxCollectionData::setCorruptValue(QModelIndex index, bool tBone )
{
    Q_UNUSED(tBone);
    mCorruptedIndex.append(index.row());
}

/*!
 Stub function.
 */
void MpMpxCollectionData::setReloadAlbumContent( bool reload )
{
    mReloadAlbumContent = reload;
}

