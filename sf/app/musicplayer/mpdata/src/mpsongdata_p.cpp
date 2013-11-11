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
* Description: Metadata of song for details view - private implementation.
*
*/


#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QBuffer>
#include <QPixmap>
#include <QTime>
#include <QIcon>
#include <QFile>
#include <QRegExp>
#include <QFileInfo>
#include <QDateTime>
#include <QDate>
#include <HbExtendedLocale>
#include <hbi18ndef.h>
#include <HbStringUtil>

#include <hbicon.h>

#include <mpxmedia.h>
#include <mpxmediacontainerdefs.h>
#include <mpxmediaarray.h>
#include <mpxmediageneraldefs.h>
#include <mpxmediamusicdefs.h>
#include <mpxmediaaudiodefs.h>
#include <mpxmediadrmdefs.h>
#include <thumbnailmanager_qt.h>

#include "mpsongdata_p.h"
#include "mptrace.h"

const int KUndefined = -1;

/*!
    \class MpSongDataPrivate
    \brief Music Player song metadata - private implementation.

    Song data provide access to song metadata.
*/

/*!
    Constructs a new MpSongDataPrivate.
 */
MpSongDataPrivate::MpSongDataPrivate( MpSongData *wrapper, ThumbnailManager *thumbnailManager )
    : q_ptr( wrapper ),
      mThumbnailManager(thumbnailManager),
      mReqId( KUndefined ),
      mAlbumArt()
{
    TX_ENTRY
    mDefaultAlbumArt = new HbIcon( "qtg_large_album_art" );
    TX_EXIT
}

/*!
 Constructs a new MpSongDataPrivate.
 */
MpSongDataPrivate::~MpSongDataPrivate()
{
    TX_ENTRY
    removeAlbumArtFile(); // TODO Remove when base64 is working
    TX_EXIT
}

/*!
 Returns the song title.
 */
QString MpSongDataPrivate::title() const
{
    TX_LOG
    return mTitle;
}

/*!
 Returns the song album.
 */
QString MpSongDataPrivate::album() const
{
    TX_LOG
    return mAlbum;
}

/*!
 Returns the song artist.
 */
QString MpSongDataPrivate::artist() const
{
    TX_LOG
    return mArtist;
}

/*!
 Returns the comment.
 */
QString MpSongDataPrivate::comment() const
{
    TX_LOG
    return mComment;
}

/*!
 Returns the song album art on \a icon.
 */
void MpSongDataPrivate::albumArt( HbIcon& icon ) const
{
    TX_ENTRY
     if (  !mAlbumArt || mAlbumArt->isNull() ) {
         TX_LOG_ARGS( "Album art is NULL." );
         icon = HbIcon();
     } else {
         TX_LOG_ARGS( "Album art is not NULL." );
         icon = *mAlbumArt ;
     }
    TX_EXIT
}

/*!
 Returns the release date.
 */
QString MpSongDataPrivate::year() const
{
    TX_LOG
    return mYear;
}

/*!
 Returns the song genre.
 */
QString MpSongDataPrivate::genre() const
{
    TX_LOG
    return mGenre;
}

/*!
 Returns the song composer.
 */
QString MpSongDataPrivate::composer() const
{
    TX_LOG
    return mComposer;
}

/*!
 Returns the album track.
 */
QString MpSongDataPrivate::albumTrack() const
{
    TX_LOG
    return mAlbumTrack;
}

/*!
 Returns link
 */
QString MpSongDataPrivate::link() const
{
    TX_LOG
    return mLink;
}

/*!
 Returns the file name
 */
QString MpSongDataPrivate::fileName() const
{
    TX_LOG
    return mFileName;
}

/*!
 Returns the MIME type
 */
QString MpSongDataPrivate::mimeType() const
{
    TX_LOG
    return mMimeType;
}

/*!
 Returns the duration
*/
QString MpSongDataPrivate::duration() const
{
    TX_LOG
    return mDuration;
}

/*!
 Returns the bit rate
 */
QString MpSongDataPrivate::bitRate() const
{
    TX_LOG
    return mBitRate;
}

/*!
 Returns the sampling rate
 */
QString MpSongDataPrivate::sampleRate() const
{
    TX_LOG
    return mSampleRate;
}

/*!
 Returns the size
 */
QString MpSongDataPrivate::size() const
{
    TX_LOG
    return mSize;
}

/*!
 Returns the modified time
 */
QString MpSongDataPrivate::modified() const
{
    TX_LOG
    return mModified;
}

/*!
 Returns the copyright
 */
QString MpSongDataPrivate::copyright() const
{
    TX_LOG
    return mCopyright;
}

/*!
 Returns the music URL
 */
QString MpSongDataPrivate::musicURL() const
{
    TX_LOG
    return mMusicURL;
}

/*!
 Returns whether the song is protected
 */
bool MpSongDataPrivate::isDrmProtected() const
{
    TX_LOG
    return mDrmProtected;
}

/*!
 Retrieve the album art in base64 encoding suitable for inline HTML display for sharing player.
 */
QString MpSongDataPrivate::albumArtBase64() const
{
    /*
    // Converts the current album art icon to a base64 string, and return the string.
    TX_LOG
    if ( mAlbumArt->isNull() ) {
        TX_ENTRY_ARGS( "MpSongDataPrivate: album art isNull" )
        return "nullimgcraptoberemoved";
    }
    TX_ENTRY_ARGS("MpSongDataPrivate: album art exists");
    QByteArray array;
    QBuffer buffer( &array );
    buffer.open( QIODevice::WriteOnly );
    mAlbumArt->pixmap().save( &buffer, "PNG" ); // writes pixmap into bytes in PNG format
    buffer.close();
    QString result = array.toBase64().constData();
    TX_ENTRY_ARGS("MpSongDataPrivate: album art base64 length: " << result.length());
    return result;
    */
    // TODO: this is temporary solution until base64 defect in QT is fixed.
    TX_LOG
    QByteArray array;

    // Remove old album art in case new one cannot be written.
    removeAlbumArtFile();

    QString sTimeStamp = QTime::currentTime().toString( "hhmmsszzz" );
    QString sTempFileLocation = QString( "e:\\album_art_%1.png" ).arg( sTimeStamp );

    ( ( MpSongDataPrivate* ) this )->mTempAlbumArt = sTempFileLocation;
    TX_LOG_ARGS( "Create album art file " << mTempAlbumArt );

    QFile file( mTempAlbumArt );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        return QString("");
    }
    if ( mAlbumArt && !mAlbumArt->isNull() && !mAlbumArt->qicon().isNull() )
    {
        QPixmap p = mAlbumArt->qicon().pixmap( QSize( 74, 74 ), QIcon::Normal, QIcon::Off );
        p.save( &file, "PNG" );
        //mAlbumArt->pixmap().save( &file, "PNG" ); // writes pixmap into bytes in PNG format
    }
    file.close();
    return mTempAlbumArt;
}

/*!
 Delete temporary album art file.
 */
void MpSongDataPrivate::removeAlbumArtFile() const
{
    TX_ENTRY
    if ( !mTempAlbumArt.isEmpty() )
    {
        TX_LOG_ARGS( "Remove album art file " << mTempAlbumArt );
        QFile::remove( mTempAlbumArt );
        ( ( MpSongDataPrivate* ) this )->mTempAlbumArt = "";
    }
    else
    {
        TX_LOG_ARGS( "Album art filename is empty" );
    }
    TX_EXIT
}

/*!
 Sets the \a link
 */
void MpSongDataPrivate::setLink( const QString &link )
{
    TX_ENTRY_ARGS( "Link =" << link )
    mLink = link;
    TX_EXIT
}

/*!
 \internal
 New data from MPX collection.
 */
void MpSongDataPrivate::setMpxMedia( const CMPXMedia& aMedia )
{
    TX_ENTRY
    TRAPD(err, DoSetMpxMediaL(aMedia));
    if ( err != KErrNone ) {
        TX_LOG_ARGS("Error: " << err << "; should never get here.");
    }
    TX_EXIT
}

/*!
 Returns the reserved length.
 */
int MpSongDataPrivate::reservedLength() const
{
    return mLink.length() + mTitle.length() + mArtist.length();
}

/*!
 Handles the album art thumbnail.
 */
void MpSongDataPrivate::thumbnailReady( QPixmap pixmap, void *data, int id, int error )
{
    TX_ENTRY
    Q_UNUSED( data );
    if ( error == 0 && mReqId == id ) {
        QIcon qicon;
        QPixmap mCompositePixmap;
        mReqId = KUndefined;

        mCompositePixmap = QPixmap( 360, 360 );
        mCompositePixmap.fill( Qt::transparent );
        QPainter painter(&mCompositePixmap);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.fillRect(mCompositePixmap.rect(), Qt::transparent);
        painter.drawPixmap(QRect(0, 0,360,360), pixmap);

        if ( !mCompositePixmap.isNull() ) {
            qicon = QIcon( mCompositePixmap );
        }
        else {
            qicon = QIcon( pixmap );
        }

        if ( mAlbumArt == mDefaultAlbumArt ) {
            TX_LOG_ARGS( "Album art is default album art." )
            delete mAlbumArt;
            mAlbumArt = new HbIcon(qicon);
            mDefaultAlbumArt = new HbIcon( "qtg_large_album_art" );
        } else {
            TX_LOG_ARGS( "Album art is NOT default album art." )
            delete mAlbumArt;
            mAlbumArt = new HbIcon(qicon);
        }

        emit q_ptr->albumArtReady();
    }
    else {
        mReqId = KUndefined;
        mAlbumArt = mDefaultAlbumArt;
        emit q_ptr->albumArtReady();
    }

    TX_EXIT
}

/*!
 \internal
 */
void MpSongDataPrivate::DoSetMpxMediaL( const CMPXMedia& aMedia )
{
    TX_ENTRY
    bool changed = false;
    if ( aMedia.IsSupported( KMPXMediaGeneralTitle ) ) {
        changed |= setTitle(
            QString::fromUtf16(
                aMedia.ValueText( KMPXMediaGeneralTitle ).Ptr(),
                aMedia.ValueText( KMPXMediaGeneralTitle ).Length() ) );
    } else {
        changed |= setTitle( QString() );
    }

    if ( aMedia.IsSupported( KMPXMediaMusicArtist ) ) {
        changed |= setArtist(
            QString::fromUtf16(
                aMedia.ValueText( KMPXMediaMusicArtist ).Ptr(),
                aMedia.ValueText( KMPXMediaMusicArtist ).Length() ) );
    } else {
        changed |= setArtist( QString() );
    }

    if ( aMedia.IsSupported( KMPXMediaMusicAlbum ) ) {
        changed |= setAlbum(
            QString::fromUtf16(
                aMedia.ValueText( KMPXMediaMusicAlbum ).Ptr(),
                aMedia.ValueText( KMPXMediaMusicAlbum ).Length() ) );
    } else {
        changed |= setAlbum( QString() );
    }
    if ( changed ) {
        emit q_ptr->playbackInfoChanged();
    }

    // call back will be called when Album art is retrieved
    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaMusicAlbumArtFileName ) ) ) {
        setAlbumArtUri(
                QString::fromUtf16(
                        aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Ptr(),
                        aMedia.ValueText(KMPXMediaMusicAlbumArtFileName).Length() ) );
    } else {
        setAlbumArtUri( QString() );
    }

    // all following will be for song details
    changed = false;
    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaGeneralUri ) ) ) {
        QString fullName = QString::fromUtf16(
                aMedia.ValueText( KMPXMediaGeneralUri ).Ptr(),
                aMedia.ValueText( KMPXMediaGeneralUri ).Length() );

        TX_LOG_ARGS( "File name with path: " << fullName );
        // get size & last modifed information from file system
        QFileInfo info( fullName );
        changed |= setSize( info.size() );

        // Localization of timestamp is somewhat complex operation:
        // 1. Localize the date and time parts separately
        // 2. Concatenate the resulting localized strings
        // 3. Finally, convert all of the digits in resulting single string
        //    to their localized versions (not all scripts have same numerals
        //    as latin/arabic)
        QDateTime lastModified = info.lastModified();
        QDate date( lastModified.date() );
        QTime time( lastModified.time() );

        HbExtendedLocale locale = HbExtendedLocale::system();

        QString dateStr = locale.format( date, r_qtn_date_usual_with_zero );
        QString timeStr = locale.format( time, r_qtn_time_long_with_zero );
        QString timestampStr( dateStr + " " + timeStr );
        TX_LOG_ARGS( "Timestamp before corrections: " << timestampStr );
        QString convertedTimestampStr( HbStringUtil::convertDigits(timestampStr) );

        changed |= setModified( convertedTimestampStr );


        // get file name without suffix
        QString file;
        QRegExp rx("(.+)\\..+");
        QString str = info.fileName();
        TX_LOG_ARGS( "File name with suffix = " << str );

        int pos = rx.indexIn( str );
        if( pos > -1 ) {
            file = rx.cap( 1 );
            TX_LOG_ARGS( "File = " << file );
        }

        changed |= setFileName( file );
    } else {
        changed |= setFileName( QString() );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaMusicComposer ) ) ) {
        TX_LOG_ARGS( "Composer is supported " );
        changed |= setComposer(
            QString::fromUtf16(
                aMedia.ValueText( KMPXMediaMusicComposer ).Ptr(),
                aMedia.ValueText( KMPXMediaMusicComposer ).Length() ) );
    } else {
        changed |= setComposer( QString() );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaMusicYear ) ) ) {
        TInt64 yearInMicroSeconds = aMedia.ValueTObjectL<TInt64>( KMPXMediaMusicYear );
        TX_LOG_ARGS( "year = " << yearInMicroSeconds );
        TTime yearTime( yearInMicroSeconds );
        changed |= setYear( yearTime.DateTime().Year() );
    } else {
        // to clear previous result
        changed |= setYear( -1 );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaMusicAlbumTrack ) ) ) {
        changed |= setAlbumTrack(
            QString::fromUtf16(
                aMedia.ValueText( KMPXMediaMusicAlbumTrack ).Ptr(),
                aMedia.ValueText( KMPXMediaMusicAlbumTrack ).Length() ) );
    } else {
        changed |= setAlbumTrack( QString() );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaMusicGenre ) ) ) {
        changed |= setGenre(
            QString::fromUtf16(
                aMedia.ValueText( KMPXMediaMusicGenre ).Ptr(),
                aMedia.ValueText( KMPXMediaMusicGenre ).Length() ) );
    } else {
        changed |= setGenre( QString() );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaGeneralMimeType ) ) ) {
        QString type = QString::fromUtf16(
                            aMedia.ValueText( KMPXMediaGeneralMimeType ).Ptr(),
                            aMedia.ValueText( KMPXMediaGeneralMimeType ).Length() );
        QString regularExpression(".+/(.+)");
        QRegExp rx(regularExpression);
        QString mimeType;

        int pos = rx.indexIn( type );
        if( pos > -1 ) {
            mimeType = rx.cap( 1 );
            mimeType = mimeType.toUpper();
            TX_LOG_ARGS( "MIME type =" << mimeType );
        }

        changed |= setMimeType( mimeType );
    } else {
        changed |= setMimeType( QString() );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaGeneralDuration ) ) ) {
        TInt duration( aMedia.ValueTObjectL<TInt>( KMPXMediaGeneralDuration ) );
        changed |= setDuration( duration / 1000 );
    } else {
        changed |= setDuration( -1 );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaAudioBitrate  ) ) ) {
        TInt bitRate( aMedia.ValueTObjectL<TInt>( KMPXMediaAudioBitrate ) );
        changed |= setBitRate( bitRate );
    } else {
        changed |= setBitRate( -1 );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaAudioSamplerate  ) ) ) {
        TInt sampleRate( aMedia.ValueTObjectL<TInt>( KMPXMediaAudioSamplerate ) );
        changed |= setSampleRate( sampleRate );
    } else {
        changed |= setSampleRate( -1 );
    }


    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaGeneralCopyright ) ) ) {
        changed |= setCopyright(
            QString::fromUtf16(
                aMedia.ValueText( KMPXMediaGeneralCopyright ).Ptr(),
                aMedia.ValueText( KMPXMediaGeneralCopyright ).Length() ) );
    } else {
        changed |= setCopyright( QString() );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaMusicURL ) ) ) {
        changed |= setMusicURL(
            QString::fromUtf16(
                aMedia.ValueText( KMPXMediaMusicURL ).Ptr(),
                aMedia.ValueText( KMPXMediaMusicURL ).Length() ) );
    } else {
        changed |= setMusicURL( QString() );
    }

    if ( aMedia.IsSupported( TMPXAttribute( KMPXMediaDrmProtected ) ) ) {
    TX_LOG_ARGS( "DRM is supported." );
        changed |= setDrmProtected( aMedia.ValueTObjectL<TBool>( KMPXMediaDrmProtected ) );
    } else {
        changed |= setDrmProtected( false );
    }

    if ( changed ) {
        emit q_ptr->songDetailInfoChanged();
    }
    TX_EXIT
}

/*!
 Sets the song \a title, returns true if the value is new.
 */
bool MpSongDataPrivate::setTitle( const QString &title )
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
 Sets the song \a album, returns true if the value is new.
 */
bool MpSongDataPrivate::setAlbum( const QString &album )
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
 Sets the song \a artist, returns true if the value is new.
 */
bool MpSongDataPrivate::setArtist( const QString &artist )
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
 Sets the song \a comment, returns true if the value is new.
 */
bool MpSongDataPrivate::setComment( const QString &comment)
{
    TX_ENTRY_ARGS( "comment =" << comment )
    bool change = false;
    if ( comment != mComment ) {
        change = true;
        mComment = comment;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the song \a composer, returns true if the value is new.
 */
bool MpSongDataPrivate::setComposer( const QString &composer )
{
    TX_ENTRY_ARGS( "composer =" << composer )
    bool change = false;
    if ( composer != mComposer ) {
        change = true;
        mComposer = composer;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the song \a genre, returns true if the value is new.
 */
bool MpSongDataPrivate::setGenre( const QString &genre )
{
    TX_ENTRY_ARGS( "genre =" << genre )
    bool change = false;
    if ( genre != mGenre ) {
        change = true;
        mGenre = genre;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the song \a date, returns true if the value is new.
 */
bool MpSongDataPrivate::setYear( int year )
{
    TX_ENTRY_ARGS( "year =" << year )
    bool change = false;
    if ( QString::number(year) != mYear ) {
        change = true;
        if ( year >= 0 && year < 9999 ) {
            mYear = QString::number(year);
        } else {
            mYear = QString();
        }
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a album track, returns true if the value is new.
 */
bool MpSongDataPrivate::setAlbumTrack( const QString &track )
{
    TX_ENTRY_ARGS( "track =" << track )
    bool change = false;
    if ( track != mAlbumTrack ) {
        change = true;
        mAlbumTrack = track;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the song \a albumArtUri.
 */
void MpSongDataPrivate::setAlbumArtUri( const QString &albumArtUri)
{
    TX_ENTRY_ARGS( "albumArtUri = " << albumArtUri )
    if ( !albumArtUri.isEmpty() ) {
        TX_LOG_ARGS( "There is album art" );
        bool ok = true;
        if ( mReqId != KUndefined ) {
            // There is already an outstanding request. Cancel it first.
            bool ok = mThumbnailManager->cancelRequest( mReqId );
        }
        if ( ok ) {
            mReqId = mThumbnailManager->getThumbnail( albumArtUri );
            if ( mReqId == KUndefined ) {
                // Request failed. Set default album art.
                mAlbumArt = mDefaultAlbumArt;
                emit q_ptr->albumArtReady();
            }
        }
    }
    else {
        // No album art uri. Set default album art.
        TX_LOG_ARGS( "There is No album art" );
        mAlbumArt = mDefaultAlbumArt;
        emit q_ptr->albumArtReady();
    }
    TX_EXIT
}

/*!
 Sets the \a file name
 */
bool MpSongDataPrivate::setFileName( const QString &fileName )
{
    TX_ENTRY_ARGS( "File name =" << fileName )
    bool change = false;
    if ( fileName != mFileName ) {
        change = true;
        mFileName = fileName;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a MIME type
 */
bool MpSongDataPrivate::setMimeType( const QString &mimeType )
{
    TX_ENTRY_ARGS( "Mime =" << mimeType )
    bool change = false;
    if ( mimeType != mMimeType ) {
        change = true;
        mMimeType = mimeType;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a duration
 */
bool MpSongDataPrivate::setDuration( int duration )
{
    TX_ENTRY_ARGS( "Duration =" << duration )
    bool change = false;
    QString timeFormatOne("%1:%2:%3");
    QString timeFormatTwo("%1:%2");
    if ( QString::number( duration ) != mDuration ) {
        change = true;
        if ( duration >= 3600 ) {
            // more than one hours
            QString hourStr, minStr, secStr;
            int hour = duration / 3600;
            int min = duration % 3600 / 60;
            int sec = duration % 3600 % 60;

            hourStr = hour >= 10 ? QString::number( hour ) : QString::number( hour ).prepend( "0" );
            minStr = min >= 10 ? QString::number( min ) : QString::number( min ).prepend( "0" );
            secStr = sec >= 10 ? QString::number( sec ) : QString::number( sec ).prepend( "0" );
            mDuration = timeFormatOne.arg( hourStr ).arg( minStr ).arg( secStr );
        } else if ( duration >= 60 && duration < 3600 ) {
            // more than one min && less than one hour
            QString minStr, secStr;
            int min = duration / 60;
            int sec = duration % 60;

            minStr = min >= 10 ? QString::number( min ) : QString::number( min ).prepend( "0" );
            secStr = sec >= 10 ? QString::number( sec ) : QString::number( sec ).prepend( "0" );
            mDuration = timeFormatTwo.arg( minStr ).arg( secStr );
        } else if ( duration > 0 && duration < 60 ) {
            QString secStr;
            secStr = duration >= 10 ? QString::number( duration ) : QString::number( duration ).prepend( "0" );
            mDuration = secStr;
        } else {
            mDuration = QString();
        }
    }
    TX_EXIT
    return change;
}

/*!
 Sets bit rate
 */
bool MpSongDataPrivate::setBitRate( int bitRate)
{
    TX_ENTRY_ARGS( "Bit rate =" << bitRate )
    bool change = false;
    if ( QString::number( bitRate ) != mBitRate ) {
        change = true;
        if ( bitRate > 0 ) {
            mBitRate = QString::number( bitRate / 1000 );
        } else {
            mBitRate = QString();
        }
    }
    TX_EXIT
    return change;
}

/*!
 Sets sample rate
 */
bool MpSongDataPrivate::setSampleRate( int sampleRate )
{
    TX_ENTRY_ARGS( "Sample rate =" << sampleRate )
    bool change = false;
    if ( QString::number( sampleRate ) != mSampleRate ) {
        change = true;
        if ( sampleRate > 0 ) {
            mSampleRate = QString::number( sampleRate );
        } else {
            mSampleRate = QString();
        }
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a size
 */
bool MpSongDataPrivate::setSize( int size )
{
    TX_ENTRY_ARGS( "Size =" << size )
    bool change = false;
    if ( QString::number( size ) != mSize ) {
        change = true;
        mSize = QString::number( size ); // in bytes
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a modification information
 */
bool MpSongDataPrivate::setModified( const QString &modified )
{
    TX_ENTRY_ARGS( "Modified =" << modified )
    bool change = false;
    if ( modified != mModified ) {
        change = true;
        mModified = modified;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a copyright information
 */
bool MpSongDataPrivate::setCopyright( const QString &copyright )
{
    TX_ENTRY_ARGS( "Copyright =" << copyright )
    bool change = false;
    if ( copyright != mCopyright ) {
        change = true;
        mCopyright = copyright;
    }
    TX_EXIT
    return change;
}

/*!
 Sets the \a music URL
 */
bool MpSongDataPrivate::setMusicURL( const QString &musicURL )
{
    TX_ENTRY_ARGS( "Music URL =" << musicURL )
    bool change = false;
    if ( musicURL != mMusicURL ) {
        change = true;

        // Make sure URL contains the correct protocol definition (HTTP).
        if ( (musicURL.length() > 0) &&
             (musicURL.indexOf( "://", 0, Qt::CaseInsensitive ) == -1) ) {
            mMusicURL = "http://" + musicURL;
            TX_LOG_ARGS("Changed music url to " << mMusicURL);
        }
		else {
            mMusicURL = musicURL;
            TX_LOG_ARGS("Music url unchanged: " << mMusicURL);
        }
    }
    TX_EXIT
    return change;
}

/*!
 Set whether the song is DRM protected
 */
bool MpSongDataPrivate::setDrmProtected( bool drmProtected )
{
    TX_ENTRY_ARGS( "DRM protected =" << drmProtected )
    bool change = false;
    if ( drmProtected != mDrmProtected ) {
        change = true;
        mDrmProtected = drmProtected;
    }
    TX_EXIT
    return change;
}

