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
* Description: MpMpxCollectionData stub for testing mpmediawallview
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
    
    enum DataProperty {
           Corrupted,
           DrmExpired
    };

    // Stub functions
    explicit MpMpxCollectionData( QObject *parent=0 );
    virtual ~MpMpxCollectionData();
    void setCount(int count);

    TCollectionContext context();

    int count() const;
    QString collectionTitle() const;
    QString itemData( int index, MpMpxCollectionData::DataType type ) const;
    void setContext( TCollectionContext context );
    int itemIndex( int itemUniqueId );    
    bool setCurrentAlbum( int index );
    int albumSongId(int index);
    bool hasAlbumSongProperty( int index, MpMpxCollectionData:: DataProperty type ) const;
    const CMPXMedia& containerMedia();

public:

    CMPXMedia               *mContainerMedia;
    bool                    mMediaSet;
    TCollectionContext      mContext;
    int                     mCount;
    int                     mCurrentAlbumIndex;
    bool                    mCurrentAlbumAvailable;
    bool                    mReturnCollectionTitle;
    int                     mArbitraryAlbumSongId;
    bool                    mAlbumSongProperty;

};

#endif // MPMPXCOLLECTIONDATA_H

