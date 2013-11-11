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
* Description: Playback Data provider for playback view.
*
*/

#include <QIcon>
#include <QString>
#include <QPixmap>
#include <QPainter>
#include <hbicon.h>

#include "mpplaybackdata.h"
#include "mptrace.h"

const int KUndefined = -1;


/*!
    Constructs a new MpPlaybackData.
 */
MpPlaybackData::MpPlaybackData( QObject *parent )
    : QObject( parent ),
      mReqId( KUndefined ),
      mDuration(20000),
      mPosition(10000),
      mTitle( QString( "title" ) ),      
      mAlbum( QString( "album" ) ),
      mArtist( QString( "artist" ) ),
      mAlbumId(0),
      mId(0),
      mAlbumArt( "qtg_large_music_album" ),
      mPlaybackState( NotPlaying ),
      mRealAudio( false )
{
    TX_ENTRY

    TX_EXIT
}

/*!
 Constructs a new MpPlaybackData.
 */
MpPlaybackData::~MpPlaybackData()
{
    TX_ENTRY
    TX_EXIT
}



/*!
 Returns the song duration.
*/
int MpPlaybackData::duration() const
{
    TX_LOG_ARGS( "mDuration = " << mDuration )
    return mDuration;
}


/*!
 Returns the song position.
*/
int MpPlaybackData::position() const
{
    TX_ENTRY_ARGS( "mPosition = " << mPosition )
    return mPosition;
}

/*!
 Returns the song title.
*/
const QString& MpPlaybackData::title() const
{
    TX_ENTRY_ARGS( "mTitle = " << mTitle )
    return mTitle;
}



/*!
 Returns the song artist.
*/
const QString& MpPlaybackData::artist() const
{
    TX_ENTRY_ARGS( "mArtist = " << mArtist )
    return mArtist;
}



/*!
 Returns the song album.
*/
const QString& MpPlaybackData::album() const
{
    TX_ENTRY_ARGS( "mAlbum = " << mAlbum )
    return mAlbum;
}


/*!
 Returns the song uri.
*/
const QString& MpPlaybackData::uri() const
{
    TX_ENTRY_ARGS( "mUri = " << mUri )
    return mUri;
}


/*!
 Returns the song album art on \a icon.
*/
void MpPlaybackData::albumArt( HbIcon& icon ) const
{
    TX_ENTRY
    icon = mAlbumArt ;
    TX_EXIT
}


/*!
 Returns the id of the album to which the song belongs.
*/
int MpPlaybackData::albumId()
{
    return mAlbumId;
}



/*!
 Returns the id the song.
*/
int MpPlaybackData::id()
{
    return mId;
}


/*!
 Returns the mode of Real Audio.
*/
bool MpPlaybackData::realAudio()
{
    return mRealAudio;
}



/*!
 Returns the playback state.
*/
MpPlaybackData::SimplifiedState MpPlaybackData::playbackState() const
{
    TX_LOG_ARGS( "mPlaybackState = " << mPlaybackState )
    return mPlaybackState;
}

