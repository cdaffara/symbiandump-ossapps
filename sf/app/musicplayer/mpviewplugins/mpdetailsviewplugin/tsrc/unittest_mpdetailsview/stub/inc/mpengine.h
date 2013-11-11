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
* Description: Music Player engine.
*
*/

#ifndef MPENGINE_H
#define MPENGINE_H

#include <QObject>
#include "mpmpxcollectionviewdefs.h"
#include "mpmpxcommondefs.h"
#include "mpcommondefs.h"

class MpMpxCollectionData;
class MpPlaybackData;
class MpSongData;
class XQSharableFile;
class MpEngine : public QObject
{
    Q_OBJECT

    friend class MpEngineFactory;

public:
    enum EngineMode{
        StandAlone,
        Fetch,
        Embedded,
        MediaBrowsing
    };
    
private:

    enum UsbBlockingState {
        USB_NotConnected,          // Not connected
        USB_Connected,             // Connected in MTP mode but not synchronizing
        USB_Synchronizing          // Connected in MassStorage mode or MTP mode and synchronizing
    };

private:
    explicit MpEngine();

public:

    virtual ~MpEngine();

    // Harvester related
    void refreshLibrary();
    bool verifyUsbBlocking( bool showMessage = false );
    void checkForSystemEvents();
    
    // Collection related
    void openCollection( TCollectionContext context );
    void openCollectionItem( int index );
    void back();
    void findPlaylists( QStringList &playlists );
    void createPlaylist( QString &playlistName, QList<int> &selection, MpMpxCollectionData* collectionData = 0  );
    void saveToPlaylist( int playlistIndex, QList<int> &selection );
    void saveToCurrentPlaylist( QList<int> &selection, MpMpxCollectionData *collectionData );    
    void renamePlaylist( QString &newName, int index );
    void renamePlaylist( QString &newName );
    void deleteSongs( QList<int> &selection );

    void previewItem( int index );
    void openIsolatedCollection( TCollectionContext context );
    void releaseIsolatedCollection();

    void findAlbumSongs( int index );
    void playAlbumSongs( int albumIndex, int songIndex );

    MpMpxCollectionData *collectionData();
    
    // Playback related
    
    MpPlaybackData *playbackData();
    
    // Details related
    MpSongData *songData();
    void retrieveSong();

signals:
    
    // Harvester related
    void libraryAboutToRefresh();
    void libraryRefreshed();
    void usbBlocked( bool blocked );
    
    // Collection related
    void collectionPlaylistOpened();
    void playlistSaved( bool success );
    void songsDeleted( bool success );
    void playlistsRenamed( bool success );
    
    void isolatedCollectionOpened( MpMpxCollectionData* collectionData );
    
    void containerContentsChanged();

public slots:

    // Harvester related
    void handleScanStarted();
    void handleScanEnded( int count, int error );
    void handleDiskEvent( MpxDiskEvents event );
    void handleUsbEvent( MpxUsbEvents event );
    void handleBlockingNoteClosing();
    
    // Collection related
    void reopenCollection();
    void reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal );

    // Playback related
    void playEmbedded( QString aFilename );
    void playEmbedded( const XQSharableFile&  file );
    void playPause();
    void stop();
    void skipForward();
    void skipBackward();
    void setPosition( int position );
    void setShuffle( bool mode );
    void setRepeat( bool mode );
	
private:

    Q_DISABLE_COPY( MpEngine )

};

#endif // MPENGINE_H
