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
* Description: stub for MpMpxPlaybackFrameworkWrapper
*
*/

#ifndef MPMPXPLAYBACKFRAMEWORKWRAPPER_H
#define MPMPXPLAYBACKFRAMEWORKWRAPPER_H

#include <QObject>
#include "mpmpxcommondefs.h"
#include "mpcommondefs.h"
#include <xqsharablefile.h>

class MpPlaybackData;
class MpSongData;
class QStringList;


class MpMpxPlaybackFrameworkWrapper : public QObject
{
    Q_OBJECT

    friend class MpMpxPlaybackFrameworkWrapperPrivate;

public:

    explicit MpMpxPlaybackFrameworkWrapper( quint32 clientSecureId=MpCommon::KMusicPlayerUid,
        MpSongData *songData=0, QObject *parent=0 );
    virtual ~MpMpxPlaybackFrameworkWrapper();
    
    MpPlaybackData *playbackData();
    void setBalance( int balance );
    void applyAudioEffects();
    void applyEqualizer();

    void retrieveSongDetails();

public slots:
    
    void play( QString aFilename );
    void play( const XQSharableFile& file );
    void play();
    void playPause();
    void stop();
    void skipForward();
    void startSeekForward();
    void stopSeeking();
    void skipBackward();
    void startSeekBackward();
    void setPosition( int position );
    void setShuffle( bool mode );
    void setRepeat( bool mode );
    void getMaxVolume();
    void pause();
    void getVolume();
    void increaseVolume();
    void decreaseVolume();
    void setVolume( int value );
    void getMuteState();
    void mute();
    void unmute();
    
public:
    MpPlaybackData* mPlaybackData;
};

#endif  // MPMPXPLAYBACKFRAMEWORKWRAPPER_H
