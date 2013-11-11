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
* Description: Stub for MpEngine
*
*/

#include "mpengine.h"
#include "stub\inc\mpplaybackdata.h"
#include "mptrace.h"


/*!
  Stub
  */
MpEngine::MpEngine() :
    mActivePreset(-1),
    mApplyPresetCount(0),
    mDisableEquqlizerCount(0),
    mRetrieveSongDetailsCount(0),
    mStopCount(0),
    mStartSeekBackwardCount(0),
    mStartSeekForwardCount(0),
    mStopSeekingCount(0),
    mSkipBackwardCount(0),
    mSkipForwardCount(0)
{
    TX_LOG_ARGS("Stub")
}

/*!
  Stub
  */
MpEngine::~MpEngine()
{
    TX_ENTRY_ARGS("Stub")
}


/*!
  Stub
  */
void MpEngine::applyPreset( int preset )
{
    mApplyPresetCount++;
    mActivePreset = preset;
}

/*!
  Stub
  */
void MpEngine::disableEqualizer()
{
    mDisableEquqlizerCount++;
    mActivePreset = -1;
}

/*!
  Stub
  */
int MpEngine::activePreset()
{
    return mActivePreset;
}

/*!
  Stub
  */
QStringList MpEngine::presetNames()
{
    QStringList presets;
    presets << "Bass Booster" << "Classic" << "Jazz" << "Pop" << "Rock" << "New Preset";
    return presets;
}


/*!
  Stub
  */
MpPlaybackData *MpEngine::playbackData()
{
    return new MpPlaybackData();
}

/*!
  Stub
  */
void MpEngine::stop()
{
    mStopCount++;
}

/*!
  Stub
  */
void MpEngine::retrieveSongDetails( int index )
{
    if ( index == -1 ) {
        // Retrieve song details of currently playing song
        mRetrieveSongDetailsCount++;
    }
}

/*!
  Stub
  */
void MpEngine::setShuffle( bool mode )
{
    Q_UNUSED(mode)
}
/*!
  Stub
  */
void MpEngine::setRepeat( bool mode )
{
    Q_UNUSED(mode)
}

/*!
  Stub
  */
void MpEngine::startSeekBackward()
{
    mStartSeekBackwardCount++;
}

/*!
  Stub
  */
void MpEngine::startSeekForward()
{
    mStartSeekForwardCount++;
}

/*!
  Stub
  */
void MpEngine::stopSeeking()
{
    mStopSeekingCount++;
}

/*!
  Stub
  */
void MpEngine::skipBackward()
{
    mSkipBackwardCount++;
}

/*!
  Stub
  */
void MpEngine::skipForward()
{
    mSkipForwardCount++;
}

/*!
  Stub
  */
void MpEngine::playPause()
{

}

/*!
  Stub
  */
void MpEngine::setPosition(int position)
{
    Q_UNUSED(position)
}
