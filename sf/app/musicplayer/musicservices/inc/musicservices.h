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
* Description: Music Services interface.
*
*/

#ifndef MUSICSERVICES_H
#define MUSICSERVICES_H

#include <xqserviceprovider.h>
#include <QStringList>


// Forward declarations
class GetMusicService;
class PlayMusicService;
class XQSharableFile;

#if defined(BUILD_MUSICSERVICES_LIB)
#define MUSICSERVICES_EXPORT Q_DECL_EXPORT
#else
#define MUSICSERVICES_EXPORT Q_DECL_IMPORT
#endif


class MUSICSERVICES_EXPORT MusicServices : public QObject
{
    Q_OBJECT

public:

    /*
     * Enum reflecting the services provided 
     */
    enum MusicService
    {
        ENoService,
        EUriFetcher,
        EPlayback
    };

    MusicServices();
    ~MusicServices();   
    QString contextTitle() const;
    MusicServices::MusicService currentService();
    void setCurrentService( MusicServices::MusicService service );

public slots:

    void itemSelected( QString songName );

signals:

    void playReady( const QString& uri );
    void playReady( const XQSharableFile& file );
    void serviceActive( quint32 clientSecureId );

private:
    
    GetMusicService                 *mFetchService;
    PlayMusicService                *mPlayService;
    MusicServices::MusicService     mCurrentService;
    
    friend class GetMusicService;
    friend class PlayMusicService;
};

#endif // MUSICSERVICES_H
