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

#ifndef MPMPXCOLLECTIONDATA_H
#define MPMPXCOLLECTIONDATA_H

#include <QObject>
#include "mpmpxcollectionviewdefs.h"

class CMPXMedia;

class MpMpxCollectionData : public QObject
{

public:

    enum DataType {
        Title,
        Uri,
        Duration,
        Count,
        Artist,
        Album,
        Genre,
        Rating,
        AlbumArtUri
    };

    // Stub functions
    explicit MpMpxCollectionData( QObject *parent=0 );
    virtual ~MpMpxCollectionData();

    TCollectionContext context();

    int count() const;
    QString collectionTitle() const;
    QString itemData( int index, MpMpxCollectionData::DataType type ) const;

    bool setCurrentAlbum( int index );
    int albumSongsCount() const;

    const CMPXMedia& containerMedia();

public:

    CMPXMedia               *mContainerMedia;
    bool                    mMediaSet;
    TCollectionContext      mContext;
    int                     mCount;
    int                     mAlbumSongsCount;
    int                     mCurrentAlbumIndex;
    bool                    mCurrentAlbumAvailable;
    bool                    mReturnCollectionTitle;

};

#endif // MPMPXCOLLECTIONDATA_H

