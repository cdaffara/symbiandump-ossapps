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


#include <QString>
#include <QPixmap>
#include <QPainter>
#include <QBuffer>
#include <hbicon.h>

#include "stub/inc/thumbnailmanager_qt.h"
#include <thumbnaildata.h>
#include <thumbnailobjectsource.h>

#include "stub/inc/mpsongdata.h"
#include "mptrace.h"

const int KUndefined = -1;


MpSongData::MpSongData( QObject *parent )
    : QObject( parent ),
      mTitle( QString( "title" ) ),
      mAlbum( QString( "album" ) ),
      mArtist( QString( "artist" ) ),
      mComposer( NULL),
      mGenre( NULL ),
      mYear( NULL ),
      mAlbumTrack( NULL ),
      mAlbumArt(),
      mReqId( KUndefined )
{
    TX_ENTRY_ARGS("Stub")
    mThumbnailManager = new ThumbnailManager( this );
    mThumbnailManager->setQualityPreference( ThumbnailManager::OptimizeForQuality );
    mThumbnailManager->setThumbnailSize( ThumbnailManager::ThumbnailMedium );
    QObject::connect( mThumbnailManager, SIGNAL( thumbnailReady( QPixmap , void * , int , int ) ),
            this, SLOT( thumbnailReady( QPixmap , void * , int , int  ) ) );
    
    TX_EXIT_ARGS("Stub")
}

MpSongData::~MpSongData()
{
    TX_ENTRY_ARGS("Stub")
    if( mThumbnailManager ) {
        delete mThumbnailManager;
    }
    TX_EXIT_ARGS("Stub")
}

void MpSongData::albumArt( HbIcon& /*icon*/ ) const
{
    TX_ENTRY_ARGS("Stub")
    TX_EXIT_ARGS("Stub")
}


QString MpSongData::title() const
{
    TX_ENTRY_ARGS("Stub")
    return mTitle;
}

QString MpSongData::album() const
{
    TX_ENTRY_ARGS("Stub")
    return mAlbum;
}

QString MpSongData::artist() const
{
    TX_ENTRY_ARGS("Stub")
    return mArtist;
}

QString MpSongData::comment() const
{
    TX_ENTRY_ARGS("Stub")
    return mComment;
}

QString MpSongData::composer() const
{
    TX_ENTRY_ARGS("Stub")
    return mComposer;
}


QString MpSongData::genre() const
{
    TX_ENTRY_ARGS("Stub")
    return mGenre;
}


QString MpSongData::albumTrack() const
{
    TX_ENTRY_ARGS("Stub")
    return mAlbumTrack;
}

QString MpSongData::link() const
{
    TX_ENTRY_ARGS("Stub")
    return mLink;
}

QString MpSongData::year() const
{
    TX_ENTRY_ARGS("Stub")    
    return mYear;
}

QString MpSongData::fileName() const
{
    TX_LOG    
    return mFileName;
}

QString MpSongData::mimeType() const
{
    TX_LOG    
    return mMimeType;   
}

QString MpSongData::duration() const
{
    TX_LOG    
    return mDuration;  
}

QString MpSongData::bitRate() const
{
    TX_LOG    
    return mBitRate;  
}

QString MpSongData::sampleRate() const
{
    TX_LOG    
    return mSampleRate;  
}

QString MpSongData::size() const
{
    TX_LOG    
    return mSize;  
}

QString MpSongData::modified() const
{
    TX_LOG    
    return mModified;  
}

QString MpSongData::copyright() const
{
    TX_LOG    
    return mCopyright;  
}

QString MpSongData::musicURL() const
{
    TX_LOG    
    return mMusicURL;  
}

bool MpSongData::isDrmProtected() const
{
    TX_LOG    
    return mDrmProtected;
}
bool MpSongData::setTitle( const QString &title )
{
    TX_ENTRY_ARGS("Stub title =" << title )
    bool change = false;
    if ( title != mTitle ) {
        change = true;
        mTitle = title;
    }
    TX_EXIT_ARGS("Stub")
    return change;
}

bool MpSongData::setAlbum( const QString &/*album*/ )
{

    TX_EXIT_ARGS("Stub")
    return false;
}

bool MpSongData::setArtist( const QString &artist )
{
    TX_ENTRY_ARGS("Stub artist =" << artist )
    bool change = false;
    if ( artist != mArtist ) {
        change = true;
        mArtist = artist;
    }
    TX_EXIT_ARGS("Stub")
    return change;
}

bool MpSongData::setComment( const QString &comment)
{
    TX_ENTRY_ARGS("Stub comment =" << comment )
    bool change = false;
    if ( comment != mComment ) {
        change = true;
        mComment = comment;
    }
    TX_EXIT_ARGS("Stub")
    return change;
}

bool MpSongData::setComposer( const QString &composer )
{
    TX_ENTRY_ARGS("Stub composer =" << composer )
    bool change = false;
    if ( composer != mComposer ) {
        change = true;
        mComposer = composer;
    }
    TX_EXIT_ARGS("Stub")
    return change;
}

bool MpSongData::setGenre( const QString &genre )
{
    TX_ENTRY_ARGS("Stub genre =" << genre )
    bool change = false;
    if ( genre != mGenre ) {
        change = true;
        mGenre = genre;
    }
    TX_EXIT_ARGS("Stub")
    return change;
}


bool MpSongData::setYear( int year )
{
    TX_ENTRY_ARGS("Stub year =" << year )
    bool change = false;
    if ( QString::number(year) != mYear ) {
        change = true;
        if ( year >= 0 && year < 9999 ) {
            mYear = QString::number(year);
        }
    }
    TX_EXIT_ARGS("Stub")
    return change;
}

bool MpSongData::setAlbumTrack( const QString &track )
{
    TX_ENTRY_ARGS("Stub track =" << track )
    bool change = false;
    if ( track != mAlbumTrack ) {
        change = true;
        mAlbumTrack = track;
    }
    TX_EXIT_ARGS("Stub")
    return change;
}

void MpSongData::setLink( const QString &link )
{
    TX_ENTRY_ARGS("Stub Link =" << link )
    mLink = link;
    TX_EXIT_ARGS("Stub")
}

void MpSongData::setAlbumArtUri( const QString &albumArtUri)
{
    TX_ENTRY_ARGS("Stub albumArtUri = " << albumArtUri )
    if ( !albumArtUri.isEmpty() ) {
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
                emit albumArtReady();
            }
        }
    }
    else {
        // No album art uri. Set default album art.
        mAlbumArt = mDefaultAlbumArt;
        emit albumArtReady();
    }
    TX_EXIT
}

bool MpSongData::setFileName( const QString &fileName )
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

bool MpSongData::setMimeType( const QString &mimeType )
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

bool MpSongData::setDuration( int duration )
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

bool MpSongData::setBitRate( int bitRate)
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

bool MpSongData::setSampleRate( int sampleRate )
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

bool MpSongData::setSize( int size )
{   
    TX_ENTRY_ARGS( "Size =" << size )
    bool change = false;
    if ( QString::number( size ) != mSize ) {
        change = true;
        mSize = QString::number( size / 1000 ).append( " MB" );
    }
    TX_EXIT
    return change;
}

bool MpSongData::setModified( const QString &modified )
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

bool MpSongData::setCopyright( const QString &copyright )
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

bool MpSongData::setMusicURL( const QString &musicURL )
{
    TX_ENTRY_ARGS( "Music URL =" << musicURL )
    bool change = false;
    if ( musicURL != mMusicURL ) {
        change = true;
        mMusicURL = musicURL;
    }
    TX_EXIT
    return change;
}

bool MpSongData::setDrmProtected( bool drmProtected )
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

void MpSongData::thumbnailReady(
        const QPixmap& pixmap,
        void *data,
        int id,
        int error  )
{
    Q_UNUSED( pixmap );
    Q_UNUSED( data );
    Q_UNUSED( id );
    Q_UNUSED( error );
    TX_ENTRY_ARGS("Stub")
    TX_EXIT_ARGS("Stub")
}

void MpSongData::commitPlaybackInfo()
{
    TX_ENTRY_ARGS("Stub")
    emit playbackInfoChanged();
    TX_EXIT_ARGS("Stub")
}

void MpSongData::commitSongDetailInfo()
{
    TX_ENTRY_ARGS("Stub")
    emit songDetailInfoChanged();
    TX_EXIT_ARGS("Stub")
}

QString MpSongData::albumArtBase64() const
{
    return QString();
}
