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

#ifndef MPSONGDATA_P_H
#define MPSONGDATA_P_H

#include "mpsongdata.h"

class CMPXMedia;
class ThumbnailManager;

class MpSongDataPrivate
{
public:
    explicit MpSongDataPrivate( MpSongData *wrapper, ThumbnailManager *thumbnailManager );
    virtual ~MpSongDataPrivate();

    QString title() const;
    QString album() const;
    QString artist() const;
    QString comment() const;
    void albumArt( HbIcon& icon ) const;
    QString year() const;
    QString genre() const;
    QString composer() const;
    QString albumTrack() const;
    QString link() const;
    QString fileName() const;
    QString mimeType() const;
    QString duration() const;
    QString bitRate() const;
    QString sampleRate() const;
    QString size() const;
    QString modified() const;
    QString copyright() const;
    QString musicURL() const;
    bool isDrmProtected() const;

    QString albumArtBase64() const;
    void removeAlbumArtFile() const; //TODO: Remove when base64 starts to work

    void setLink( const QString &link );
    void setMpxMedia( const CMPXMedia& aMedia );

    int reservedLength() const;
    void thumbnailReady( QPixmap pixmap, void *data, int id, int error );

private:

    void DoSetMpxMediaL( const CMPXMedia& aMedia );

    bool setTitle( const QString &title );
    bool setAlbum( const QString &album );
    bool setArtist( const QString &artist );
    bool setComment( const QString &comment );
    void setAlbumArtUri( const QString &albumArtUri );
    bool setYear( int date );
    bool setGenre( const QString &genre );
    bool setComposer( const QString &compoer );
    bool setAlbumTrack( const QString &albumtrack );
    bool setFileName( const QString &fileName );
    bool setMimeType( const QString &mimeType );
    bool setDuration( int duration );
    bool setBitRate( int bitRate);
    bool setSampleRate( int sampleRate );
    bool setSize( int size );
    bool setModified( const QString &modified );
    bool setCopyright( const QString &copyright );
    bool setMusicURL( const QString &musicURL );
    bool setDrmProtected( bool drmProtected );

private:

    MpSongData          *q_ptr;

    ThumbnailManager    *mThumbnailManager; // Not owned
    TInt                mReqId;

    QString             mTitle;
    QString             mAlbum;
    QString             mArtist;
    QString             mComment;
    QString             mComposer;
    QString             mGenre;
    QString             mYear;
    QString             mAlbumTrack;
    HbIcon              *mAlbumArt;
    HbIcon              *mDefaultAlbumArt;
    QString             currentAAUri;
    QString             mLink;
    QString             mFileName;
    QString             mMimeType;
    QString             mDuration;
    QString             mBitRate;
    QString             mSampleRate;
    QString             mSize;
    QString             mModified;
    QString             mCopyright;
    QString             mMusicURL;
    bool                mDrmProtected;
    QString             mTempAlbumArt; //TODO: Remove when base64 starts to work

};

#endif // MPSONGDATA_P_H
