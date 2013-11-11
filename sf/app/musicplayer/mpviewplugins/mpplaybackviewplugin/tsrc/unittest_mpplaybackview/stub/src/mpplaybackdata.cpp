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

#include "../inc/mpplaybackdata.h"


int gUpdateCounter = 0;


/*!
 Returns gUpdateCounter.
 gUpdateCounter counts the number of MpNowPlayingBackEnd::update calls.
 */
int MpPlaybackData::getUpdateCounter()
{
    return gUpdateCounter;
}

/*!
 Resets gUpdateCounter to zero.
 */
void MpPlaybackData::resetUpdateCounter()
{
    gUpdateCounter = 0;
}

/*!
 Causes signal stateUpdate to be emitted.
 */
void MpPlaybackData::triggerStateUpdate(SimplifiedState state)
{
    mPlaybackState = state;
    emit playbackStateChanged();
    
}

/*!
 Causes signal titleChanged to be emitted.
 */
void MpPlaybackData::triggerLabelsChanged(const QString &title, const QString &artist )
{
    mArtist = artist;
    mTitle = title;
    emit playbackInfoChanged();;
}

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

const QString& MpPlaybackData::uri() const
{
    return mUri;
}

void MpPlaybackData::setPlaybackState(MpPlaybackData::SimplifiedState state)
{
    mPlaybackState=state;
}

void MpPlaybackData::setUri(QString uri)
{
    mUri = uri;
}
