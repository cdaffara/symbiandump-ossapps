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

/*!
 Stub function.
 */
MpMpxCollectionData::MpMpxCollectionData( QObject *parent )
    : QObject(parent),
      mContext(ECollectionContextUnknown),
      mCount(0),
      mItemDataReturn(true)
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
TCollectionContext MpMpxCollectionData::context() const
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
QString MpMpxCollectionData::itemData( int index, MpMpxCollectionData::DataType type ) const
{
    if ( index < 0 || index > mCount ) {
        TX_LOG_ARGS("index=" << index);
        qFatal("MpMpxCollectionData::itemData called with index out of range");
    }
    QString data;
    if ( !mItemDataReturn && (type != MpMpxCollectionData::Count) ) {
        return data;
    }
    switch ( type ) {
        case MpMpxCollectionData::Title:
            data = QString("Title");
            break;
        case MpMpxCollectionData::Artist:
            data = QString("Artist");
            break;
        case MpMpxCollectionData::Count:
            data = QString("Count");
            break;
        case MpMpxCollectionData::AlbumArtUri:
            data = QString("AlbumArtUri");
            break;
        default:
            qFatal("MpMpxCollectionData::itemData called with unknown type");
            break;
    }
    return data;
}
