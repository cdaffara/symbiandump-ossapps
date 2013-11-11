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
* Description: Playback Data provider for playback view.
*
*/

#ifndef MPPLAYBACKDATA_H
#define MPPLAYBACKDATA_H

#include <QObject>
#include <QString>
#include <QPixmap>

#include <hbicon.h>

//forward declartions

class ThumbnailManager;

#if defined(BUILD_MPDATA_LIB)
#define MPDATA_EXPORT Q_DECL_EXPORT
#else
#define MPDATA_EXPORT Q_DECL_IMPORT
#endif

class MPDATA_EXPORT MpPlaybackData : public QObject
{
    Q_OBJECT

public:

    enum SimplifiedState {
        NotPlaying,
        Playing,
        Paused,
        Stopped
    };

    explicit MpPlaybackData( QObject *parent=0 );
    virtual ~MpPlaybackData();

    void setDuration( int duration );
    int duration() const;
    void setPosition( int position );
    int position() const;

    bool setTitle( const QString& title );
    const QString& title() const;
    bool setArtist( const QString& artist );
    const QString& artist() const;
    bool setAlbum( const QString& album );
    const QString& album() const;
    bool setUri( const QString& uri );
    const QString& uri() const;
    void setAlbumArtUri( const QString& albumArtUri );
    void albumArt( HbIcon& icon ) const;
    bool setAlbumId( int id );
    int albumId();
    bool setId( int id );
    int id();
    bool setRealAudio( bool mode );
    bool realAudio();
    void setPlaybackState( const SimplifiedState state );
    MpPlaybackData::SimplifiedState playbackState() const;

    void commitPlaybackInfo( bool changed );
    void resetData();
    void setCorrupted( int id );
    void handleCollectionPlaylistOpened();
    

public slots:

    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );

signals:

    void durationChanged();
    void positionChanged();
    void albumArtReady();
    void playbackStateChanged();
    void playbackInfoChanged();
    void fileCorrupted(int id );
    void collectionPlaylistOpened();
    
private:

    ThumbnailManager         *mThumbnailManager; //owned
    int                      mReqId;

    int                      mDuration;
    int                      mPosition;
    QString                  mTitle;
    QString                  mAlbum;
    QString                  mArtist;
    QString                  mUri;
    int                      mAlbumId;
    int                      mId;
    HbIcon                   mAlbumArt;
    SimplifiedState          mPlaybackState;
    bool                     mRealAudio;
    bool                     mCollectionPlaylistOpened;

    Q_DISABLE_COPY(MpPlaybackData)
};

#endif // MPPLAYBACKDATA_H
