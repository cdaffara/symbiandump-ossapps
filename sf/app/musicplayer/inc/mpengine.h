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


class QStringList;
class MpMpxHarvesterFrameworkWrapper;
class MpSongScanner;
class MpMpxCollectionFrameworkWrapper;
class MpMpxPlaybackFrameworkWrapper;
class MpMpxCollectionData;
class MpPlaybackData;
class MpSongData;
class XQSharableFile;
class MpEngineFactory;
class MpAudioEffectsFrameworkWrapper;
class MpEqualizerFrameworkWrapper;
class MpApplicationMonitor;

#if defined(BUILD_MPENGINE_LIB)
#define MPENGINE_EXPORT Q_DECL_EXPORT
#else
#define MPENGINE_EXPORT Q_DECL_IMPORT
#endif


class MPENGINE_EXPORT MpEngine : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int activePreset READ activePreset)

    friend class MpEngineFactory;

public:
    enum EngineMode{
        StandAlone,
        Fetch,
        Embedded,
        MediaBrowsing,
        HomeScreen
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
    bool verifyUsbBlocking( bool notify = false );
    void checkForSystemEvents();
    MpSongScanner *songScanner();

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
    void playAlbumSongs( int albumIndex, int songIndex, MpMpxCollectionData* collectionData = 0 );

    MpMpxCollectionData *collectionData();

    // Playback related

    MpPlaybackData *playbackData();

    void shuffleAll();


    // Details related
    MpSongData *songData();
    void retrieveSongDetails( int index = -1 );

    // Audio Effects related
    int balance();
    bool loudness();

    // Equalizer related
    void applyPreset( int presetIndex );
    void disableEqualizer();
    int activePreset();
    QStringList presetNames();
    
    //Activity related
    void saveActivityData( QByteArray &data );
    void loadActivityData( const QByteArray &data );

signals:

    // Harvester related
    void libraryAboutToUpdate();
    void libraryUpdateStarted();
    void libraryUpdated();
    void usbBlocked( bool blocked );
    void unableToCotinueDueUSB();
    void usbSynchronizationStarted();
    void usbSynchronizationFinished();

    // Collection related
    void collectionPlaylistOpened();
    void aboutToAddSongs( int count );
    void playlistSaved( bool success );
    void deleteStarted(TCollectionContext context, int Count);
    void songsDeleted( bool success );
    void playlistsRenamed( bool success );
    void isolatedCollectionOpened( MpMpxCollectionData* collectionData );
    void containerContentsChanged();
    void restorePathFailed();

    // Playback related
    void volumePropertyChanged( MpCommon::MpVolumeProperty property, int value );
    void corruptedStop();

    // Equalizer related
    void equalizerReady();

public slots:

    // Harvester related
    void refreshLibrary( bool automaticRequest = false );
    void handleScanStarted();
    void handleScanEnded( int count, int error );
    void handleDiskEvent( MpxDiskEvents event );
    void handleUsbEvent( MpxUsbEvents event );

    // Collection related
    void reopenCollection();
    void reorderPlaylist( int playlistId, int songId, int originalOrdinal, int newOrdinal );
    void handleDeleteStarted( TCollectionContext context, int count );
    void handleDeleteEnded( bool success );
    void cancelCollectionRequest();
    void handleCollectionPlaylistOpened();

    // Playback related
    void playEmbedded( QString aFilename );
    void playEmbedded( const XQSharableFile&  file );
    void play();
    void pause();
    void playPause();
    void stop();
    void skipForward();
    void startSeekForward();
    void stopSeeking();
    void skipBackward();
    void startSeekBackward();
    void setPosition( int position );
    void setShuffle( bool mode );
    void setRepeat( bool mode );
    void getMaxVolume();
    void getVolume();
    void increaseVolume();
    void decreaseVolume();
    void setVolume( int value );
    void getMuteState();
    void mute();
    void unmute();
    void handleCorruptedStop( bool lastSong );

    // Audio Effects related
    void setBalance( int balance );
    void setLoudness( bool mode );

    // Equalizer related
    void handleEqualizerReady();

private:

    void initialize( quint32 clientSecureId, EngineMode mode);

    // Harvester related
    void handleUsbMassStorageStartEvent();
    void handleUsbMassStorageEndEvent();
    void handleUsbMtpStartEvent();
    void handleUsbMtpEndEvent();
    void handleUsbMtpNotActive();
    void changeUsbBlockingState( UsbBlockingState state );
    void autoRefreshLibrary( bool automaticRequest=false );

private:

    Q_DISABLE_COPY( MpEngine )

    EngineMode                          mEngineMode;

    // Harvesting related
    MpMpxHarvesterFrameworkWrapper      *mMpxHarvesterWrapper;  // Own
    MpSongScanner                       *mSongScanner;          // Own

    // Collection related
    MpMpxCollectionFrameworkWrapper     *mMpxCollectionWrapper; // Own

    // Playback related
    MpMpxPlaybackFrameworkWrapper       *mMpxPlaybackWrapper;   // Own

    // Audio Effects related
    MpAudioEffectsFrameworkWrapper      *mAudioEffectsWrapper;  // Own

    // Equalizer related
    MpEqualizerFrameworkWrapper         *mEqualizerWrapper;     // Own
    int                                 mCurrentPresetIndex;

    MpSongData                          *mSongData;             // Own
    MpApplicationMonitor                *mApplicationMonitor;   // Own

    UsbBlockingState                    mUsbBlockingState;
    UsbBlockingState                    mPreviousUsbState;
    bool                                mHandleMediaCommands;

};

#endif // MPENGINE_H
