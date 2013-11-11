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
* Description:
*
*/

#include "irqmediaplayer.h"
#if defined(MMFADAPTER)
#include "irqmmfadapter.h"
#elif defined(PHONONAdapter)
#include "irqphononadapter.h"
#endif

#include "irqlogger.h"


// ---------------------------------------------------------------------------
// IRQMediaPlayer::IRQMediaPlayer
// Constructor
// Creates player adpater and connect the signals
// ---------------------------------------------------------------------------
//
IRQMediaPlayer::IRQMediaPlayer()
{
    LOG_METHOD;
#if defined(MMFADAPTER)
    iPlayer = new IRQMMFAdapter();
#elif defined(PHONONAdapter)
    iPlayer = new IRQPHONONAdapter();
#endif

    connect(iPlayer, SIGNAL(connectionEstablished(int)),
            this, SIGNAL(connectionEstablished(int)));
    connect(iPlayer, SIGNAL(errorOccured(IRQError)),
            this, SIGNAL(errorOccured(IRQError)));
    connect(iPlayer, SIGNAL(percentageBuffered(int)),
            this, SIGNAL(percentageBuffered(int)));
    connect(iPlayer, SIGNAL(metaDataReceived(IRQMetaData&)),
            this, SIGNAL(metaDataReceived(IRQMetaData&)));
    connect(iPlayer, SIGNAL(volumeExpected(int&)),
            this, SIGNAL(volumeExpected(int&)));
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::~IRQMediaPlayer
// Destructor
// Delete player adapter
// ---------------------------------------------------------------------------
//
IRQMediaPlayer::~IRQMediaPlayer()
{
    LOG_METHOD;
    delete iPlayer;
    iPlayer = NULL;
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::playStation
// Plays a specific radio station via certain access point id
// ---------------------------------------------------------------------------
//
void IRQMediaPlayer::playStation(const QString &aUrl, int aApId)
{
    LOG_METHOD;
    LOG_FORMAT("aUrl=%s, aApId=%d", STRING2CHAR(aUrl), aApId);
    if (iPlayer)
    {
        iPlayer->playStation(aUrl, aApId);
    }
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::stop
// Stops the playback
// ---------------------------------------------------------------------------
//
void IRQMediaPlayer::stop()
{
    LOG_METHOD;
    if (iPlayer)
    {
        iPlayer->stop();
    }
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::setVolume
// Sets the volume
// ---------------------------------------------------------------------------
//
void IRQMediaPlayer::setVolume(int aVolume)
{
    LOG_METHOD;
    LOG_FORMAT("aVolume=%d", aVolume);
    if (iPlayer)
    {
        iPlayer->setVolume(aVolume);
    }
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::getVolume
// Gets the current volume of playback
// ---------------------------------------------------------------------------
//
int IRQMediaPlayer::getVolume()
{
    LOG_METHOD;
    int val = 0;

    if (iPlayer)
    {
        val = iPlayer->getVolume();
    }

    return val;
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::enableStereoEffect
// Turns on stereo effect
// ---------------------------------------------------------------------------
//
void IRQMediaPlayer::enableStereoEffect()
{
    LOG_METHOD;
    iPlayer->enableStereoEffect();
}

// ---------------------------------------------------------------------------
// IRQMediaPlayer::disableStereoEffect
// Turns off stereo effect
// ---------------------------------------------------------------------------
//
void IRQMediaPlayer::disableStereoEffect()
{
    LOG_METHOD;
    iPlayer->disableStereoEffect();
}

