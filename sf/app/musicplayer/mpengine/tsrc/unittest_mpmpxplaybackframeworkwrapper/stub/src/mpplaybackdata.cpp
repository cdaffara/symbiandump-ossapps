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
* Description: Playback Data stub for testing mpmpxframeworkwrapper
*
*/


#include <QString>

#include "stub/inc/mpplaybackdata.h"
#include "mptrace.h"


/*!
 Stub function.
*/
MpPlaybackData::MpPlaybackData()
    : mDuration( 0 ),
      mPosition( 0 ),
      mTitle(""),
      mAlbum(""),
      mArtist("Unknown"),
      mPlaybackState(MpPlaybackData::Stopped),
      mMediaReady(EFalse),
      mRealAudio( false ),
      mCorrupted( false ),
      mAlbumId( 0 ),
      mId( 0 )
{
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
int MpPlaybackData::duration() const
{
    TX_LOG
    return mDuration;
}

/*!
 Stub function.
*/
int MpPlaybackData::position() const
{
    TX_LOG
    return mPosition;
}

/*!
 Stub function.
*/
const QString& MpPlaybackData::title() const
{
    TX_LOG
    return mTitle;
}

/*!
 Stub function.
*/
const QString& MpPlaybackData::album() const
{
    TX_LOG
    return mAlbum;
}

/*!
 Stub function.
*/
const QString& MpPlaybackData::artist() const
{
    TX_LOG
    return mArtist;
}

/*!
 Stub function.
*/
MpPlaybackData::SimplifiedPlaybackState MpPlaybackData::playbackState()
{
    TX_LOG_ARGS("mPlaybackState=" << mPlaybackState)
    return mPlaybackState;
}

/*!
 Stub function.
*/
void MpPlaybackData::setDuration( int duration )
{
    TX_ENTRY_ARGS( "duration =" << duration )
    mDuration = duration;
    TX_EXIT
}

/*!
 Stub function.
*/
void MpPlaybackData::setPosition( int position )
{
    TX_ENTRY_ARGS( "position =" << position )
    mPosition = position;
    TX_EXIT
}

/*!
 Stub function.
*/
bool MpPlaybackData::setTitle( const QString& title )
{
    TX_ENTRY_ARGS( "title =" << title )
    bool change = false;
    if ( title != mTitle ) {
        change = true;
        mTitle = title;
    }
    TX_EXIT
    return change;
}

/*!
 Stub function.
*/
bool MpPlaybackData::setAlbum( const QString& album )
{
    TX_ENTRY_ARGS( "album =" << album )
    bool change = false;
    if ( album != mAlbum ) {
        change = true;
        mAlbum = album;
    }
    TX_EXIT
    return change;
}

/*!
 Stub function.
*/
bool MpPlaybackData::setArtist( const QString& artist )
{
    TX_ENTRY_ARGS( "artist =" << artist )
    bool change = false;
    if ( artist != mArtist ) {
        change = true;
        mArtist = artist;
    }
    TX_EXIT
    return change;
}


/*!
 Stub function.
*/
void MpPlaybackData::setAlbumArtUri( const QString& albumArtUri)
{
    TX_ENTRY_ARGS( "albumArtUri =" << albumArtUri )
    if ( !albumArtUri.isEmpty() ) {
        if ( albumArtUri != currentAAUri ) {
            currentAAUri = albumArtUri;
        }
    }
    TX_EXIT
}

/*!
 Stub function.
*/
void MpPlaybackData::setUri( const QString& aUri)
{
    TX_ENTRY_ARGS( "aUri =" << aUri )
    if ( !aUri.isEmpty() ) {
        if ( aUri != currentUri ) {
            currentUri = aUri;
        }
    }
    TX_EXIT
}

/*!
 Stub function.
*/
void MpPlaybackData::setPlaybackState(const SimplifiedPlaybackState state )
{
    TX_ENTRY_ARGS( "state =" << state )
    mPlaybackState = state;
    TX_EXIT
}

/*!
 Stub function.
*/
void MpPlaybackData::commitPlaybackInfo( bool changed )
{
    TX_ENTRY
    mMediaReady = changed;
    TX_EXIT
}

/*!
 Stub function.
*/
void MpPlaybackData::handleMediaReady()
{
    mMediaReady = ETrue;
}

/*!
Stub function.
*/
bool MpPlaybackData::setRealAudio( bool mode )
{
    if( mRealAudio != mode ) {
        mRealAudio = mode;
        return true;
    }
    else {
       return false;
    }
}

/*!
Stub function.
*/
bool MpPlaybackData::setAlbumId( int id )
{
    if ( mAlbumId != id ) {
        mAlbumId = id;
        return true;
    }
    else {
        return false;
    }
}

/*!
Stub function.
*/
bool MpPlaybackData::setId( int id )
{
    if( mId != id ) {
        mId = id;
        return true;
    }
    else {
       return false;
    }
}

/*!
Stub function.
*/
void MpPlaybackData::resetData()
{
    mDuration = 0;
    mPosition = 0;
    mTitle = QString();
    mAlbum = QString();
    mArtist = QString();
    currentUri = QString();
    mAlbumId = 0;
    mId = 0;
    mRealAudio = false;
    mCorrupted = false;
}

/*!
 Stub function.
*/
void MpPlaybackData::setCorrupted( int id )
{
    Q_UNUSED(id);
    mCorrupted = true;
}

//end of file
