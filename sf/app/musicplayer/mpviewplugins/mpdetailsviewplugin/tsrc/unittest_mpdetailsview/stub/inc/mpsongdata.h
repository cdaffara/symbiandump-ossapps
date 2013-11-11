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
* Description: Metadata of song for details view.
*
*/

#ifndef MPSONGDATA_H
#define MPSONGDATA_H

#include <QObject>
#include <QString>
#include <QPixmap>

//forward declartions
class ThumbnailManager;
class HbIcon;

//class declaration
class MpSongData : public QObject
{
    Q_OBJECT

public:
    explicit MpSongData( QObject *parent=0 );
    virtual ~MpSongData();
    
    bool setTitle( const QString &title );
    bool setAlbum( const QString &album );
    bool setArtist( const QString &artist );
    bool setComment( const QString &comment );
    void setAlbumArtUri( const QString &albumArtUri );
    bool setYear( int date );
    bool setGenre( const QString &genre );
    bool setComposer( const QString &compoer );
    bool setAlbumTrack( const QString &albumtrack );
    void setLink( const QString &link );
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
    
    // inform details view when basic information is ready to accelerate UI update
    void commitPlaybackInfo();
    // inform details view when details information is ready
    void commitSongDetailInfo();

public slots:
    void thumbnailReady( const QPixmap& pixmap, void *data, int id, int error );
    
    int reservedLength() const { return mLink.length() + mTitle.length() + mArtist.length(); }

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

signals:
    void albumArtReady();
    void playbackInfoChanged();
	void songDetailInfoChanged();	

public:
    QString                  mTitle;
    QString                  mAlbum;
    QString                  mArtist;
    QString                  mComment;
    QString                  mComposer;
    QString                  mGenre;
    QString                  mYear;
    QString                  mAlbumTrack;
    HbIcon                   *mAlbumArt;
    ThumbnailManager         *mThumbnailManager; //owned
    TInt                     mReqId;
    QString                  currentAAUri; 
    HbIcon                   *mDefaultAlbumArt;
    QString                  mLink;
    QString                  mFileName;
    QString                  mMimeType;
    QString                  mDuration;
    QString                  mBitRate;
    QString                  mSampleRate;
    QString                  mSize;
    QString                  mModified;
    QString                  mCopyright;
    QString                  mMusicURL;
    bool                     mDrmProtected;
    
    Q_DISABLE_COPY(MpSongData)
};

#endif // MPSONGDATA_H
