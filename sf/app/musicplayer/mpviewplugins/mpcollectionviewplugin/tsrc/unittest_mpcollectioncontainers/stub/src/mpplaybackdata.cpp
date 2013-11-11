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
    : mPlaybackState(NotPlaying)
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
int MpPlaybackData::id()
{
    return 0;
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
MpPlaybackData::SimplifiedState MpPlaybackData::playbackState() const
{
    return mPlaybackState;
}
