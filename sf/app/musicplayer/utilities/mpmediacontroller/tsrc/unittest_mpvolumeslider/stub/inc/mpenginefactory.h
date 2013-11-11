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
* Description: MpEngineFactory stub for testing mpvolumeslider
*
*/

#ifndef MPENGINEFACTORY_H
#define MPENGINEFACTORY_H

#include <QObject>

#include "mpcommondefs.h"

enum MediaCommandReceived{
    CmdNone,
    CmdPlay,
    CmdPause,
    CmdPlayPause,
    CmdStop,
    CmdSkipForward,
    CmdStartSeekForward,
    CmdStopSeeking,
    CmdSkipBackward,
    CmdStartSeekBackward,
    CmdIncreaseVolume,
    CmdDecreaseVolume,
    CmdSetVolumeLevel,
    CmdMuteVolume,
    CmdUnmuteVolume
    };

enum MediaPropertyRequested{
    PropNone,
    PropVolumeLevelMax,
    PropVolumeLevel,
    PropVolumeMuteState
    };

class MpEngine : public QObject
{
    Q_OBJECT

public:
    // Stub functions
    MpEngine();

public:
    // Playback related
    void play();
    void pause();
    void playPause();
    void stop();
    void skipForward();
    void startSeekForward();
    void stopSeeking();
    void skipBackward();
    void startSeekBackward();
    void getMaxVolume();
    void getVolume();
    void increaseVolume();
    void decreaseVolume();
    void setVolume( int value );
    void getMuteState();
    void mute();
    void unmute();
    
signals:
    void volumePropertyChanged( MpCommon::MpVolumeProperty property, int value );
    
public:
    MediaCommandReceived                    iMediaCommandReceived;
    QList<MediaPropertyRequested>           iMediaPropertyRequested;
    int                                     iCommandValue;
    
};

class MpEngineFactory : public QObject
{
    Q_OBJECT

public:
    // Stub functions
private:
    explicit MpEngineFactory();

public:
    virtual ~MpEngineFactory();
    static MpEngineFactory * instance();
    static MpEngine *sharedEngine();
    static void close();
    
private:
    MpEngine             *mSharedEngine;

};


#endif // MPENGINEFACTORY_H
