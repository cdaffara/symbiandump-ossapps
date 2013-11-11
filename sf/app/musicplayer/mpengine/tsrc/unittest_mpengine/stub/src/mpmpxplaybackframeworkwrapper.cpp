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


#include "mpmpxplaybackframeworkwrapper.h"
#include "mpplaybackdata.h"

/*!
    \class MpMpxPlaybackFrameworkWrapper
    \brief Wrapper for mpx framework utilities.

    MPX framework wrapper provides Qt style interface to the MPX framework
    utilities. Its implementation is hidden using private class data pattern.
*/

/*!
 Constructs the utility wrapper.
 */
MpMpxPlaybackFrameworkWrapper::MpMpxPlaybackFrameworkWrapper( quint32 clientSecureId, MpSongData *songData, QObject *parent )
    : QObject(parent)
{
    Q_UNUSED(clientSecureId);
    Q_UNUSED(songData);
}

/*!
 Destructs the utility wrapper.
 */
MpMpxPlaybackFrameworkWrapper::~MpMpxPlaybackFrameworkWrapper()
{

}

/*!
 Returns pointer to MpPlaybackData, which is the playback data.
 */
MpPlaybackData *MpMpxPlaybackFrameworkWrapper::playbackData()
{
    return mPlaybackData;
}

/*!
 Slot to handle play from Uri aFilename
 */
void MpMpxPlaybackFrameworkWrapper::play( QString aFilename )
{
    Q_UNUSED(aFilename);
}

/*!
 Slot to handle play from file handle
 */
void MpMpxPlaybackFrameworkWrapper::play( const XQSharableFile& file)
{
    Q_UNUSED(file);
}

/*!
 Slot to handle play command
 */
void MpMpxPlaybackFrameworkWrapper::play()
{

}

/*!
 Slot to handle a play pause.
 */
void MpMpxPlaybackFrameworkWrapper::playPause()
{
}

/*!
 Slot to handle a stop.
 */
void MpMpxPlaybackFrameworkWrapper::stop()
{
}

/*!
 Slot to handle a skip forward.
 */
void MpMpxPlaybackFrameworkWrapper::skipForward()
{
}

/*!
 Slot to handle seek forward.
 */
void MpMpxPlaybackFrameworkWrapper::startSeekForward()
{
}

/*!
 Slot to handle stop seeking.
 */
void MpMpxPlaybackFrameworkWrapper::stopSeeking()
{
}

/*!
 Slot to handle a skip backwards.
 */
void MpMpxPlaybackFrameworkWrapper::skipBackward()
{
}

/*!
 Slot to handle seek backwards.
 */

void MpMpxPlaybackFrameworkWrapper::startSeekBackward()
{
}

/*!
 Slot to handle a request to change \a position.
 */
void MpMpxPlaybackFrameworkWrapper::setPosition( int position )
{
    Q_UNUSED(position);
}

/*!
 Slot to handle a request to change shuffle \a mode.
 */
void MpMpxPlaybackFrameworkWrapper::setShuffle( bool mode )
{
    Q_UNUSED(mode);
}

/*!
 Slot to handle a request to change repeat \a mode.
 */
void MpMpxPlaybackFrameworkWrapper::setRepeat( bool mode )
{
    Q_UNUSED(mode);
}

/*!
 Change \a balance.
 */
void MpMpxPlaybackFrameworkWrapper::setBalance( int balance )
{
    Q_UNUSED(balance);
}

/*!
 Send command to apply audio effects.
 */
void MpMpxPlaybackFrameworkWrapper::applyAudioEffects()
{

}

/*!
 Send command to apply equalizer.
 */
void MpMpxPlaybackFrameworkWrapper::applyEqualizer()
{

}

/*!
 Retrieves song details for the currently playing song.
 */
void MpMpxPlaybackFrameworkWrapper::retrieveSongDetails()
{

}

/*!
 Slot to handle a request to get the volume level Max.
 Response will be asynchronously sent through volumePropertyChanged() signal.
 */
void MpMpxPlaybackFrameworkWrapper::getMaxVolume()
{
}

/*!
 Slot to handle pause command
 */
void MpMpxPlaybackFrameworkWrapper::pause()
{
    
}

/*!
 Slot to handle a request to get the current volume level.
 Response will be asynchronously sent through volumePropertyChanged() signal.
 */
void MpMpxPlaybackFrameworkWrapper::getVolume()
{

}

/*!
 Slot to handle a request to increase volume.
 */
void MpMpxPlaybackFrameworkWrapper::increaseVolume()
{

}

/*!
 Slot to handle a request to decrease volume.
 */
void MpMpxPlaybackFrameworkWrapper::decreaseVolume()
{

}

/*!
 Slot to handle a request to set the volume level.
 */
void MpMpxPlaybackFrameworkWrapper::setVolume( int value )
{
    Q_UNUSED( value );
}

/*!
 Slot to handle a request to get the current mute state.
 Response will be asynchronously sent through volumePropertyChanged() signal.
 */
void MpMpxPlaybackFrameworkWrapper::getMuteState()
{

}

/*!
 Slot to handle a request to mute.
 */
void MpMpxPlaybackFrameworkWrapper::mute()
{

}

/*!
 Slot to handle a request to unmute.
 */
void MpMpxPlaybackFrameworkWrapper::unmute()
{

}

