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

class MpSongDataPrivate;
class CMPXMedia;
class HbIcon;
class ThumbnailManager;

#if defined(BUILD_MPDATA_LIB)
#define MPDATA_EXPORT Q_DECL_EXPORT
#else
#define MPDATA_EXPORT Q_DECL_IMPORT
#endif

//class declaration
class MPDATA_EXPORT MpSongData : public QObject
{
    Q_OBJECT

    friend class MpSongDataPrivate;

public:
    explicit MpSongData( QObject *parent=0 );
    virtual ~MpSongData();

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

signals:

    void albumArtReady();
    void playbackInfoChanged();
    void songDetailInfoChanged();

public slots:

    int reservedLength() const;
    void thumbnailReady( QPixmap pixmap, void *data, int id, int error );

private:

    Q_DISABLE_COPY(MpSongData)
    MpSongDataPrivate   *d_ptr;
    ThumbnailManager    *mThumbnailManager; // Owned

};

#endif // MPSONGDATA_H

