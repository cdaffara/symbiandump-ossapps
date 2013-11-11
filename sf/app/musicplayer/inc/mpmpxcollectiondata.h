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
* Description: Music Player collection data.
*
*/

#ifndef MPMPXCOLLECTIONDATA_H
#define MPMPXCOLLECTIONDATA_H

#include <QObject>
#include <QAbstractListModel>

#include "mpmpxcollectionviewdefs.h"

class MpMpxCollectionDataPrivate;
class CMPXMedia;

#if defined(BUILD_MPDATA_LIB)
#define MPDATA_EXPORT Q_DECL_EXPORT
#else
#define MPDATA_EXPORT Q_DECL_IMPORT
#endif

class MPDATA_EXPORT MpMpxCollectionData : public QObject
{
    Q_OBJECT

    friend class MpMpxCollectionDataPrivate;

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

    explicit MpMpxCollectionData( QObject *parent=0 );
    virtual ~MpMpxCollectionData();

    TCollectionContext context() const;

    int count() const;
    QString collectionTitle() const;
    QString itemData( int index, MpMpxCollectionData::DataType type ) const;

    bool isAutoPlaylist();
    bool isAutoPlaylist( int index );
    int itemCount( int index );
    int containerId();
    int itemId(int index);
    int albumSongId( int index );
    void removeItem(int index);
    bool testCachedItem( int itemId );
    void insertCachedItem(int index);

    bool setCurrentAlbum( int index );
    int currentAlbumIndex() const;
    int albumSongsCount() const;
    QString albumSongData( int index, MpMpxCollectionData::DataType type ) const;
    bool hasItemProperty( int index, MpMpxCollectionData:: DataProperty type ) const;
    bool hasAlbumSongProperty( int index, MpMpxCollectionData:: DataProperty type ) const;

    void setMpxMedia( const CMPXMedia& entries, bool reopen=false );
    void incrementalOpenUpdate();
    const CMPXMedia& containerMedia();
    void setContext( TCollectionContext context );
    void setAlbumContent( const CMPXMedia& albumContent );
    int itemIndex( int itemUniqueId );
    int albumSongIndex( int songUniqueId );
    QList<int> songIndex( int songUniqueId );
    void setCorruptValue(QModelIndex index, bool tBone );
    void setReloadAlbumContent( bool reloadAlbum );


signals:

    void contextChanged( TCollectionContext context );
    void dataChanged();
    void dataChanged( int fromIndex, int toIndex );
    void albumDataChanged();
    void refreshAlbumSongs();

private:

    Q_DISABLE_COPY(MpMpxCollectionData)
    MpMpxCollectionDataPrivate *d_ptr;

};

#endif // MPMPXCOLLECTIONDATA_H

