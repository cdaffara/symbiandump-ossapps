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
* Description: MpPlaybackData stub for testing mpnowplayingwidget
*
*/

#include "stub/inc/mpplaybackdata.h"


int gUpdateCounter = 0;


/*!
 Stub function.
 */
MpPlaybackData::MpPlaybackData()
    : mPlaybackState(NotPlaying),
      mSongId(0)
{
    gUpdateCounter++;
}

/*!
 Stub function.
 */
MpPlaybackData::~MpPlaybackData()
{
}

/*!
 Stub function.
 */
const QString& MpPlaybackData::title() const
{
    return mTitle;
}

/*!
 Stub function.
 */
const QString& MpPlaybackData::artist() const
{
    return mArtist;
}

/*!
 Stub function.
 */
MpPlaybackData::SimplifiedState MpPlaybackData::playbackState() const
{
    return mPlaybackState;
}

/*!
 Stub function.
 */
int MpPlaybackData::albumId()
{
    return 0;
}

/*!
 Stub function.
 */
void MpPlaybackData::emitPlaybackStateChanged()
{
   emit playbackStateChanged();
}

/*!
 Stub function.
 */
void MpPlaybackData::setPlaybackState(MpPlaybackData::SimplifiedState state)
{
     mPlaybackState=state;
}

/*!
 Stub function.
 */
int MpPlaybackData::id()
{
    return mSongId;
}
 
