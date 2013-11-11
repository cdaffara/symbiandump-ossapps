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
* Description: Playback Data stub for testing mpmpxpbframeworkwrapper
*
*/

#ifndef MPPLAYBACKDATA_H
#define MPPLAYBACKDATA_H

#include <QObject>
#include <QString>


//class declaration
class MpPlaybackData : public QObject
{
    Q_OBJECT

public:
    enum SimplifiedPlaybackState {
        NotPlaying,
        Playing,
        Paused,
        Stopped
    };
    
    explicit MpPlaybackData();
    virtual ~MpPlaybackData();

    void setDuration( int duration);
    void setPosition( int position);
    bool setTitle( const QString& title);
    bool setAlbum( const QString& album);
    bool setArtist( const QString& artist);
    void setAlbumArtUri( const QString& albumArtUri);
    void setUri( const QString& aUri);
    void setPlaybackState(const SimplifiedPlaybackState state );
    int duration() const;
    int position() const;
    const QString& title() const;
    const QString& album() const;
    const QString& artist() const;
    MpPlaybackData::SimplifiedPlaybackState playbackState();
    void commitPlaybackInfo( bool changed );
    void handleMediaReady();
    bool setRealAudio( bool mode );
    bool setAlbumId( int id );
    bool setId( int id );
    void resetData();
    void setCorrupted( int id );


public:
    int                      mDuration;
    int                      mPosition;
    QString                  mTitle;
    QString                  mAlbum;
    QString                  mArtist;
    SimplifiedPlaybackState  mPlaybackState;
    QString                  currentAAUri; 
    QString                  currentUri;
    TBool                    mMediaReady;
    bool                     mRealAudio;
    bool                     mCorrupted;
    int                      mAlbumId;
    int                      mId;

    Q_DISABLE_COPY(MpPlaybackData)
};

#endif // MPPLAYBACKDATA_H
