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
* Description: MpEngineFactory stub for testing mpmediakeyhandler
*
*/

#ifndef MPENGINEFACTORY_H
#define MPENGINEFACTORY_H

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

class MpEngine
{
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
    void getVolumeLevelMax();
    void getVolumeLevel();
    void increaseVolume();
    void decreaseVolume();
    void setVolumeLevel( int value );
    void getVolumeMuteState();
    void muteVolume();
    void unmuteVolume();
    
public:
    MediaCommandReceived      iMediaCommandReceived;
    MediaPropertyRequested    iMediaPropertyRequested;
    
};

class MpEngineFactory
{
    
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
