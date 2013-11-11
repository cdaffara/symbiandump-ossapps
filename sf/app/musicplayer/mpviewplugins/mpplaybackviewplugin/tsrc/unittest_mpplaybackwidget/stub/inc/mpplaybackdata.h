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

//forward declarations

class MpPlaybackData : public QObject
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

    int duration() const;
    int position() const;

    const QString& title() const;
    const QString& artist() const;
    const QString& album() const;
    const QString& uri() const;
    void albumArt( HbIcon& icon ) const;
    int albumId();
    bool setId( int id );
    int id();
    bool realAudio();
    MpPlaybackData::SimplifiedState playbackState() const;

    void commitPlaybackInfo();
    void resetData();
    
signals:

    void durationChanged();
    void positionChanged();
    void albumArtReady();
    void playbackStateChanged();
    void playbackInfoChanged();
    void collectionPlaylistOpened();

    
public:

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
};

#endif // MPPLAYBACKDATA_H
