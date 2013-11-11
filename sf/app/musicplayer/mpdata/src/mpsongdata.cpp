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
* Description: Detailed song data provider for Details View.
*
*/

#include <thumbnailmanager_qt.h>

#include "mpsongdata.h"
#include "mpsongdata_p.h"

/*!
    \class MpSongData
    \brief Music Player song metadata.

    Song data provide access to song metadata.
*/

/*!
    \fn void albumArtReady()

    This signal is emitted album art is ready.
 */

/*!
    \fn void playbackInfoChanged()

    This signal is emitted when basic information is available.
 */

/*!
    \fn void songDetailInfoChanged()

    This signal is emitted when detail information is available.
 */


/*!
    Constructs a new song data.
 */
MpSongData::MpSongData( QObject *parent )
    : QObject( parent )
{
    mThumbnailManager = new ThumbnailManager( this );
    mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForQuality );
    mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailMedium );
    connect( mThumbnailManager, SIGNAL( thumbnailReady(QPixmap, void *, int, int) ),
             this, SLOT( thumbnailReady(QPixmap, void *, int, int) ) );
    d_ptr = new MpSongDataPrivate(this, mThumbnailManager);
}

/*!
 Destructs the song data.
 */
MpSongData::~MpSongData()
{
    delete mThumbnailManager;
    delete d_ptr;
}

/*!
 Returns the song title.
 */
QString MpSongData::title() const
{
    return d_ptr->title();
}

/*!
 Returns the song album.
 */
QString MpSongData::album() const
{
    return d_ptr->album();
}

/*!
 Returns the song artist.
 */
QString MpSongData::artist() const
{
    return d_ptr->artist();
}

/*!
 Returns comment
 */
QString MpSongData::comment() const
{
    return d_ptr->comment();
}

/*!
 Returns the song album art on \a icon.
 */
void MpSongData::albumArt( HbIcon& icon ) const
{
    d_ptr->albumArt(icon);
}

/*!
 Returns the release date.
 */
QString MpSongData::year() const
{
    return d_ptr->year();
}

/*!
 Returns the song genre.
 */
QString MpSongData::genre() const
{
    return d_ptr->genre();
}

/*!
 Returns the song composer.
 */
QString MpSongData::composer() const
{
    return d_ptr->composer();
}

/*!
 Returns the album track.
 */
QString MpSongData::albumTrack() const
{
    return d_ptr->albumTrack();
}

/*!
 Returns link
 */
QString MpSongData::link() const
{
    return d_ptr->link();
}


/*!
 Returns the file name
 */
QString MpSongData::fileName() const
{
    return d_ptr->fileName();
}

/*!
 Returns the MIME type
 */
QString MpSongData::mimeType() const
{
    return d_ptr->mimeType();
}

/*!
 Returns the duration
 */
QString MpSongData::duration() const
{
    return d_ptr->duration();
}

/*!
 Returns the bit rate
 */
QString MpSongData::bitRate() const
{
    return d_ptr->bitRate();
}

/*!
 Returns the sampling rate
 */
QString MpSongData::sampleRate() const
{
    return d_ptr->sampleRate();
}

/*!
 Returns the size
 */
QString MpSongData::size() const
{
    return d_ptr->size();
}

/*!
 Returns the modified time
 */
QString MpSongData::modified() const
{
    return d_ptr->modified();
}

/*!
 Returns the copyright
 */
QString MpSongData::copyright() const
{
    return d_ptr->copyright();
}

/*!
 Returns the music URL
 */
QString MpSongData::musicURL() const
{
    return d_ptr->musicURL();
}

/*!
 Returns whether the song is protected
 */
bool MpSongData::isDrmProtected() const
{
    return d_ptr->isDrmProtected();
}


/*!
 Retrieve the album art in base64 encoding suitable for inline HTML display for sharing player.
 */
QString MpSongData::albumArtBase64() const
{
    return d_ptr->albumArtBase64();
}

/*!
 Delete temporary album art file.
 */
void MpSongData::removeAlbumArtFile() const
{
    d_ptr->removeAlbumArtFile();
}

/*!
 Sets the \a link
*/
void MpSongData::setLink( const QString &link )
{
    d_ptr->setLink(link);
}

/*!
 Sets the media \a media from the MPX framework.
 Internal usage only from MpEngine.
 */
void MpSongData::setMpxMedia( const CMPXMedia& media )
{
    d_ptr->setMpxMedia(media);
}

/*!
 Returns the reserved length.
 */
int MpSongData::reservedLength() const
{
    return d_ptr->reservedLength();
}

/*!
 Slot to handle the album art thumb.
 */
void MpSongData::thumbnailReady( QPixmap pixmap, void *data, int id, int error )
{
    d_ptr->thumbnailReady(pixmap, data, id, error);
}

