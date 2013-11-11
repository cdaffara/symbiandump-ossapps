/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Music player home screen widget
*
*/

#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

// includes
#include <hbwidget.h>
#include <xqappmgr.h>

// Forward declarations
class HbLabel;
class HbPushButton;
class HbIcon;
class HbMarqueeItem;
class MpEngine;
class MpAlbumCoverWidget;
class MpApplicationMonitor;
class MpPlaybackData;

/**
 * Media key identifier for push button events
 */
enum MediaKeyIdentifier {
    EPrevious = 1,
    EPlayPause,
    ENext
};

/**
 * Music Player launch source identifier
 */    
enum LaunchIdentifier {
    ELaunchToMainView,
    ELaunchToNowPlayingView,
    ELaunchToNowPlayingViewWithShuffle
};

class MusicWidget: public HbWidget
{
    Q_OBJECT
    
public:    
    MusicWidget(QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0);
    ~MusicWidget();

private:
    
    /**
     * Setup graphics
     */    
    void setupUi();
    
    /**
     * Get latest song data, player state and album art 
     */      
    void refreshData();
    
    /**
     * Launch Music Player to view
     */      
    void launchMusicPlayer( int launchMode );
    
    /**
     * Update widget buttons according to Music Player state
     */       
    void updateButtons();

    /**
     * Connect/disconnect Music Player engine and playback data
     */           
    void connectMusicPlayerEngine( bool connect );
    
    /**
     * Get the playback state
     */
    int getPlaybackState();

    /**
     * Clear the song data
     */
    void clearData();
    
public slots:

    /**
     * Widget related
     */    
    void onInitialize();
    void onShow();
    void onHide();
    void onUninitialize();
    
    /**
     * MpEngine related
     */    
    void libraryUpdateStarted();
    void libraryUpdated();
    void usbBlocked( bool blocked );    

    /**
     * MpPlaybackData related
     */        
    void albumArtReady();
    void playbackStateChanged();
    void playbackInfoChanged();    
    
private slots:

    /**
     * Slots to be called after button press
     */    
    void handlePrevButtonClicked();
    void handlePlayButtonClicked();
    void handleNextButtonClicked();
    void handleAlbumArtClicked();
    
    /**
     * MpApplicationMonitor related
     */
    void applicationStatusChanged( bool isrunning );
    
private:  
    //mSongDataLayout items
    HbLabel *mSongDataBackground;
    HbMarqueeItem *mMarqueeText;
    
    // mControlButtonsLayout items
    HbPushButton *mPreviousPushButton;
    HbPushButton *mPlayPushButton;
    HbPushButton *mNextPushButton;
    
    // MusicPlayer related state identifiers
    bool mMusicPlayerUpdating;
    bool mMusicPlayerBlocked;
    bool mMusicPlayerRunning;
    
    // Album art for background
    MpAlbumCoverWidget *mAlbumArt;
    
    // Used to launch MusicPlayer
    XQApplicationManager mApplicationManager;
    
    // MusicPlayer control and state updates
    MpEngine *mMpEngine;
    MpPlaybackData *mMpPlaybackData;
    
    // Music Player observer
    MpApplicationMonitor *mMpApplicationMonitor;
};

#endif // MUSICWIDGET_H
