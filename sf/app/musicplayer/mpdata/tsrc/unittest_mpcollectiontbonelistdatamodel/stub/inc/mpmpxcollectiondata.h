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

#ifndef MPMPXCOLLECTIONDATA_H
#define MPMPXCOLLECTIONDATA_H

#include <QObject>
#include <QAbstractListModel>

#include "mpmpxcollectionviewdefs.h"


class MpMpxCollectionData : public QObject
{
    Q_OBJECT

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

    // Test utility functions
    static int getInitCounter();
    static void resetInitCounter();

    // Stub functions
    explicit MpMpxCollectionData( QObject *parent=0 );
    virtual ~MpMpxCollectionData();

    int albumSongsCount() const;
    int albumSongId( int index );
    QString albumSongData( int index, MpMpxCollectionData::DataType type ) const;
    bool hasAlbumSongProperty( int index, MpMpxCollectionData:: DataProperty type );
    int albumSongIndex( int songUniqueId );
    void setCorruptValue(QModelIndex index, bool tBone );
    void setReloadAlbumContent( bool );
signals:

    void contextChanged( TCollectionContext context );
    void refreshAlbumSongs();
	void dataChanged();

public:

    int                     mAlbumSongCount;
    bool                    mItemDataReturn;
    bool                    mReloadAlbumContent;
    bool                    mHasAlbumSongProperty;
    QList<int>              mCorruptedIndex;

};

#endif // MPMPXCOLLECTIONDATA_H

