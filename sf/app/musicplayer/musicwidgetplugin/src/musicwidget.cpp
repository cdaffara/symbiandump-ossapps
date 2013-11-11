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

// System includes
#include <qgraphicslinearlayout.h>
#include <hbpushbutton.h>
#include <hblabel.h>
#include <hbicon.h>
#include <hbmarqueeitem.h>
#include <hbanchorlayout.h>
#include <hbwidget.h>
#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbtranslator.h>

// User includes
#include "musicwidget.h"
#include "musicwidgetdocumentloader.h"
#include "mptrace.h"
#include "mpenginefactory.h"
#include "mpalbumcoverwidget.h"
#include "mpplaybackdata.h"
#include "mpapplicationmonitor.h"

// Constants
/** Docml */
const QString MUSIC_WIDGET_DOCML = ":/docml/music_widget.docml";

/**  Play and Pause icons */
const QLatin1String ICON_PLAY ( "qtg_mono_play" );
const QLatin1String ICON_PAUSE ( "qtg_mono_pause" );

/** Album art icon */
const QLatin1String ICON_DEFAULT_ART ( "qtg_large_music_empty" );

/**  HsWidget normal background */
const QLatin1String WIDGET_BG_NORMAL ( "qtg_fr_hswidget_normal" );
/**  Text background */
const QLatin1String TEXT_BG_NORMAL ( "qtg_fr_multimedia_trans" );
/**  Separator : character and space between artist and song name */
const QLatin1String SEPARATOR_TEXT ( ": " );

/**  Infinite loop value for marquee */
const int MARQUEE_LOOP_FOREVER = -1;

/**  Now playing view with random play */
const QLatin1String SERVICEREQ_NOW_PLAYING_VIEW_SHUFFLE ( "appto://10207C62?activityname=MusicNowPlayingView&launchtype=standalone&shuffle=yes" );
/**  Main view */
const QLatin1String SERVICEREQ_MAIN_VIEW ( "appto://10207C62?activityname=MusicMainView&launchtype=standalone" );
/**  Now playing view */
const QLatin1String SERVICEREQ_NOW_PLAYING_VIEW ( "appto://10207C62?activityname=MusicNowPlayingView&launchtype=standalone" );

/**  DOCML object names */
const QLatin1String DOCML_CONTAINER_WIDGET ( "containerWidget" );
const QLatin1String DOCML_BG_ICON ( "widget_background_icon" );
const QLatin1String DOCML_ALBUM_ART ( "album_art_image" );
const QLatin1String DOCML_TEXT_BG ( "text_bg" );
const QLatin1String DOCML_SCROLLING_TEXT ( "scrolling_text" );
const QLatin1String DOCML_BUTTON_GROUP_CONTAINER ( "button_group_container" );
const QLatin1String DOCML_PREV_BUTTON ( "btn_previous" );
const QLatin1String DOCML_PLAY_BUTTON ( "btn_play" );
const QLatin1String DOCML_NEXT_BUTTON ( "btn_next" );

/*!
    \class MusicWidget
    \brief Provides simple Music Player playback controls from the Home Screen.
 */

/*!
 Constructs the Music Player home screen widget.
 */
MusicWidget::MusicWidget( QGraphicsItem* parent, Qt::WindowFlags flags ):
    HbWidget( parent, flags ),
    mSongDataBackground( 0 ),
    mMarqueeText( 0 ),    
    mPreviousPushButton( 0 ),
    mPlayPushButton( 0 ),    
    mNextPushButton( 0 ),
    mMusicPlayerUpdating( false ),
    mMusicPlayerBlocked( false ),
    mMusicPlayerRunning( false ),
    mAlbumArt( 0 ),
    mMpEngine( 0 ),
    mMpPlaybackData( 0 ),
    mMpApplicationMonitor( 0 )
{
    TX_LOG
    HbTranslator translator( QString( "musicwidgetplugin" ) );
}

/*!
 Destructor
 */
MusicWidget::~MusicWidget()
{
    TX_ENTRY
    if ( mMpEngine ) {
        MpEngineFactory::close();
    }

    delete mMpApplicationMonitor;
    TX_EXIT
}

/*!
 Creates UI.
 */
void MusicWidget::setupUi()
{
    TX_ENTRY
    
    // Use document loader to load the contents
    MusicWidgetDocumentLoader loader;
    bool ok = false;
    loader.load( MUSIC_WIDGET_DOCML, &ok );
    Q_ASSERT_X( ok, "music_widget", "invalid title docml file" );

    TX_LOG_ARGS( "loaded docml" )
    
    QGraphicsLinearLayout* mainLayout = new QGraphicsLinearLayout( Qt::Vertical, this );
    QGraphicsWidget* container = loader.findWidget( DOCML_CONTAINER_WIDGET );
    mainLayout->addItem( container );
    
    //Sets parent
    setLayout( mainLayout );

    // Load and set widget background
    HbLabel *bgWidget = qobject_cast<HbLabel*> ( loader.findWidget( DOCML_BG_ICON ) );
    HbFrameItem *frameItemWidgetBg = new HbFrameItem();
    frameItemWidgetBg->frameDrawer().setFrameGraphicsName( WIDGET_BG_NORMAL );
    frameItemWidgetBg->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
    frameItemWidgetBg->frameDrawer().setFillWholeRect( true );
    bgWidget->setBackgroundItem( frameItemWidgetBg );

    // Setup album art area
    QGraphicsWidget *tmpWidgetPtr;
    tmpWidgetPtr = loader.findWidget( DOCML_ALBUM_ART );
    mAlbumArt = qobject_cast<MpAlbumCoverWidget*>( tmpWidgetPtr );
    mAlbumArt->setEnabled( false );
    mAlbumArt->setDefaultIcon( HbIcon( ICON_DEFAULT_ART ) );
        
    // Set song data background
    mSongDataBackground = qobject_cast<HbLabel*> ( loader.findWidget( DOCML_TEXT_BG ) );
    HbFrameItem *frameItemSongBg = new HbFrameItem();
    frameItemSongBg->frameDrawer().setFrameGraphicsName( TEXT_BG_NORMAL );
    frameItemSongBg->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
    frameItemSongBg->frameDrawer().setFillWholeRect( true );
    mSongDataBackground->setBackgroundItem( frameItemSongBg );
    mSongDataBackground->hide();

    // Set scrolling song data
    mMarqueeText = qobject_cast<HbMarqueeItem*> ( loader.findWidget( DOCML_SCROLLING_TEXT ) );
    mMarqueeText->setLoopCount( MARQUEE_LOOP_FOREVER );

    // Get buttons
    HbWidget *buttonGroupContainer = qobject_cast<HbWidget*> ( loader.findWidget(
            DOCML_BUTTON_GROUP_CONTAINER ) );
    if ( buttonGroupContainer ) {
        // Define push buttons
        mPreviousPushButton = qobject_cast<HbPushButton*> ( loader.findWidget( DOCML_PREV_BUTTON ) );
        mPlayPushButton = qobject_cast<HbPushButton*> ( loader.findWidget( DOCML_PLAY_BUTTON ) );
        mNextPushButton = qobject_cast<HbPushButton*> ( loader.findWidget( DOCML_NEXT_BUTTON ) );
    }

    TX_EXIT
}

/*!
 Manually update music widget state.
 */
void MusicWidget::refreshData()
{ 
    TX_ENTRY
    
    // Get current player state
    playbackStateChanged();
    
    // Get current song data, updates button states
    playbackInfoChanged();

    // Get current album art
    albumArtReady();
    
    TX_EXIT
}

/*!
 Launch music player.
 */
void MusicWidget::launchMusicPlayer( int launchMode )
{  
    TX_ENTRY
    
    QUrl url;
    
    switch ( launchMode ) {
        case ELaunchToMainView:
            TX_LOG_ARGS( "to main view" )
            // Launch to main view
            url.setUrl( SERVICEREQ_MAIN_VIEW );    
            break;
        case ELaunchToNowPlayingView:
            TX_LOG_ARGS( "to now playing view" )
            // Launch to now playing view
            url.setUrl( SERVICEREQ_NOW_PLAYING_VIEW );
            break;
        case ELaunchToNowPlayingViewWithShuffle:
            TX_LOG_ARGS( "to now playing view with shuffle" )
            // Launch to now playing view with shuffle
            url.setUrl( SERVICEREQ_NOW_PLAYING_VIEW_SHUFFLE );        
            break;
        default:
            //should never get here
            TX_LOG_ARGS( "Error: invalid launchMode." )
            return;
    }
    
    XQAiwRequest* req;
    req = mApplicationManager.create( url );    
    if ( req ) {
        req->setBackground( false );
        req->setSynchronous( false );
    
        // Set function parameters
        QList<QVariant> args;
        args << url.toString();
        req->setArguments( args );

        // Send the request
        bool res = req->send();
        if ( !res ) {
            // Request failed. 
            int error = req->lastError();
            // Handle error
            TX_LOG_ARGS( "req send error: " << error )
        }
        delete req;
    }
   
    TX_EXIT
}

/*!
 Initializes the widget.
 */
void MusicWidget::onInitialize()
{
    TX_ENTRY

    // Setup UI
    setupUi();
    
    // Start Monitoring Music Player running status.
    mMpApplicationMonitor = new MpApplicationMonitor();
    
    // Check if Music Player UI is up and running
    mMusicPlayerRunning = mMpApplicationMonitor->isApplicationRunning();
    
    // Connect to MP engine and playback data
    connectMusicPlayerEngine( mMusicPlayerRunning );

    // Connect albumart click signal
    QObject::connect( mAlbumArt, SIGNAL( clicked() ), this, SLOT( handleAlbumArtClicked() ) );    
    
    // Connect button clicked signals
    QObject::connect( mPreviousPushButton, SIGNAL( clicked() ), this, SLOT( handlePrevButtonClicked() ) );
    QObject::connect( mPlayPushButton, SIGNAL( clicked() ), this, SLOT( handlePlayButtonClicked() ) );   
    QObject::connect( mNextPushButton, SIGNAL( clicked() ), this, SLOT( handleNextButtonClicked() ) );

    // Connect Music Player observer signal
    QObject::connect( mMpApplicationMonitor, SIGNAL( applicationStatusChanged( bool ) ), 
                     this, SLOT ( applicationStatusChanged( bool ) ) );   
    
    TX_EXIT
}

/*!
 Shows the widget.
 */
void MusicWidget::onShow()
{
    TX_ENTRY
    refreshData();
    TX_EXIT
}

/*!
 Hides the widget.
 */
void MusicWidget::onHide()
{
    TX_LOG
    if ( mMarqueeText->isAnimating() ) {
        mMarqueeText->stopAnimation();
    }
}

/*!
 Uninitializes the widget.
 */
void MusicWidget::onUninitialize()
{
    TX_LOG
}

/*!
 Slot to handle when the album art area is clicked.
 */
void MusicWidget::handleAlbumArtClicked()
{
    TX_ENTRY
    if ( getPlaybackState() == MpPlaybackData::NotPlaying ) {
        launchMusicPlayer( ELaunchToMainView );
    }
    else {
        launchMusicPlayer( ELaunchToNowPlayingView );
    }
    TX_EXIT
}

/*!
 Slot for previous button clicked.
 */
void MusicWidget::handlePrevButtonClicked()
{ 
    TX_ENTRY
    if ( mMpEngine ) {
        mMpEngine->skipBackward();
    }
    TX_EXIT
}

/*!
 Slot for play button clicked. Handles both Play and Pause!
 */
void MusicWidget::handlePlayButtonClicked()
{
    TX_ENTRY
    // If nothing to play, start music player in playback view.
    if ( getPlaybackState() == MpPlaybackData::NotPlaying ) {
        //Signal music player to start playing all songs with random
        launchMusicPlayer( ELaunchToNowPlayingViewWithShuffle );
    }
    else {
        // send play/pause command.
        if ( mMpEngine ) {
            mMpEngine->playPause();
        }
    }
    TX_EXIT
}

/*!
 Slot for next button clicked.
 */
void MusicWidget::handleNextButtonClicked()
{
    TX_ENTRY
    if ( mMpEngine ) {
        mMpEngine->skipForward();
    }
    TX_EXIT
}

/*!
 Slot to handle when the music player running status changed.
 */
void MusicWidget::applicationStatusChanged( bool isRunning )
{
    TX_ENTRY_ARGS( "running " << isRunning );

    // Connect/disconnect engine base on music player running state.
    if ( mMusicPlayerRunning != isRunning ) {
        TX_LOG_ARGS("running state changed");
        mMusicPlayerRunning = isRunning;
        connectMusicPlayerEngine( isRunning );
        // refresh data if musicplayer is on, otherwise clear data
        if ( isRunning ) {
            refreshData();
        }
        else {
            clearData();
        }
    }
    
    TX_EXIT
}

/*!
 Library started refreshing.
 */
void MusicWidget::libraryUpdateStarted()
{
    TX_ENTRY
    //Refresh Library start
    mMusicPlayerUpdating = true;
    
    // Update button enabled/disabled state
    updateButtons();
    TX_EXIT
}

/*!
 Library is finished refreshing.
 */
void MusicWidget::libraryUpdated()
{
    TX_ENTRY
    //Refresh Library done
    mMusicPlayerUpdating = false;
    
    // Update button enabled/disabled state
    updateButtons();
    TX_EXIT
}

/*!
 USB blocking state has changed.
 */
void MusicWidget::usbBlocked( bool blocked )
{
    TX_ENTRY_ARGS( "blocked: " << blocked )
    mMusicPlayerBlocked = blocked;
    
    // Update button enabled/disabled state
    updateButtons();
    TX_EXIT
}

/*!
 The album art has changed.
 */
void MusicWidget::albumArtReady()
{   
    TX_ENTRY
    HbIcon icon;
    
    if ( getPlaybackState() == MpPlaybackData::NotPlaying ) {
        TX_LOG_ARGS( "default album art" )
        icon = HbIcon( ICON_DEFAULT_ART );    
    }
    else {
        TX_LOG_ARGS( "playback data album art" )
        if ( mMpPlaybackData ) {
            mMpPlaybackData->albumArt( icon );
        }
    }
    
    mAlbumArt->setIcon( icon );
    mAlbumArt->setEnabled( true );
    TX_EXIT
}

/*!
 Playback state has changed.
 */
void MusicWidget::playbackStateChanged()
{
    TX_ENTRY
    
    if ( getPlaybackState() == MpPlaybackData::Playing ) {
        TX_LOG_ARGS( "pause icon" )
        mPlayPushButton->setIcon( HbIcon( ICON_PAUSE ) );
    }
    else {
        TX_LOG_ARGS( "play icon" )
        mPlayPushButton->setIcon( HbIcon( ICON_PLAY ) );
    }

    // Update button enabled/disabled state
    updateButtons();
    
    TX_EXIT
}

/*!
 Song data has changed.
 */
void MusicWidget::playbackInfoChanged()
{ 
    TX_ENTRY
    
    QString songData;
    QString artist;
    QString title;
    
    if ( mMpPlaybackData ) {
        artist = mMpPlaybackData->artist();
        title = mMpPlaybackData->title();
    }
    
    if ( !title.length() ) {
        artist.clear();
        title.clear();
        // Initialize marquee with something (space), to ensure text updates to display as well.
        songData = " ";
        TX_LOG_ARGS( "no song data" )
    }
    else {
        if ( !artist.length() ) {
            //Show localized "Unknown" if there is no artist name
            songData.append( hbTrId( "txt_muwidget_other_unknown_1" ).arg( title ) );
        }
        else {
            songData.append( artist );
            songData.append( SEPARATOR_TEXT );
            songData.append( title );
        }
        TX_LOG_ARGS( "song data received" )
    }    
    
    mMarqueeText->setText( songData );
    TX_LOG_ARGS( "marquee text: " << songData )

    // Display song data if it exist.
    if ( title.length() ) {
        if ( !mSongDataBackground->isVisible() ) {
            mSongDataBackground->show();
        }
        if ( !mMarqueeText->isAnimating() ) {
            mMarqueeText->startAnimation();
        }
    }
    else {
        if ( mSongDataBackground->isVisible() ) {
            mSongDataBackground->hide();
        }
        if ( mMarqueeText->isAnimating() ) {
            mMarqueeText->stopAnimation();
        }
    }
    
    TX_EXIT
}

/*!
 Update buttons to disabled/enabled as required.
 */
void MusicWidget::updateButtons()
{ 
    TX_ENTRY
    
    // All buttons disabled if updating or usb blocked
    if ( mMusicPlayerUpdating || mMusicPlayerBlocked ) {
        TX_LOG_ARGS( "refreshing" )
        mPreviousPushButton->setEnabled( false );
        mPlayPushButton->setEnabled( false );
        mNextPushButton->setEnabled( false );
    }
    // Only play button enabled if there is no song data
    else if ( getPlaybackState() == MpPlaybackData::NotPlaying ) {
        TX_LOG_ARGS( "no song data" )
        mPreviousPushButton->setEnabled( false );
        mPlayPushButton->setEnabled( true );
        mNextPushButton->setEnabled( false );
    }
    // Enable all buttons if there is song data
    else {
        TX_LOG_ARGS( "enable all buttons" )
        mPreviousPushButton->setEnabled( true );
        mPlayPushButton->setEnabled( true );
        mNextPushButton->setEnabled( true );
    }
    
    TX_EXIT
}

/*!
 Connect/disconnect Music Player engine and playback data
 */
void MusicWidget::connectMusicPlayerEngine( bool connect )
{
    TX_ENTRY

    if ( mMpEngine ) {
        MpEngineFactory::close();
        mMpEngine = 0;
        mMpPlaybackData = 0;
        mMusicPlayerUpdating = false;
        mMusicPlayerBlocked = false;
    }
    
    if ( connect ) {
        // Connect to MP engine and playback data
        mMpEngine = MpEngineFactory::createSharedEngine( MpCommon::KMusicPlayerUid, MpEngine::HomeScreen );
        Q_ASSERT_X( mMpEngine, "music_widget", "no music player engine" );
        TX_LOG_ARGS( "got mp engine" )
    
        mMpPlaybackData = mMpEngine->playbackData();
        Q_ASSERT_X( mMpPlaybackData, "music_widget", "no playback data" );
        TX_LOG_ARGS( "got playback data" )
    
        // MpEngine
        QObject::connect( mMpEngine, SIGNAL( libraryUpdateStarted() ), this, SLOT( libraryUpdateStarted() ) );
        QObject::connect( mMpEngine, SIGNAL( libraryUpdated() ), this, SLOT( libraryUpdated() ) );
        QObject::connect( mMpEngine, SIGNAL( usbBlocked( bool ) ), this, SLOT( usbBlocked( bool ) ) );

        //MpPlaybackData
        QObject::connect( mMpPlaybackData, SIGNAL( albumArtReady() ), this, SLOT( albumArtReady() ) );
        QObject::connect( mMpPlaybackData, SIGNAL( playbackStateChanged() ), this, SLOT( playbackStateChanged() ) );
        QObject::connect( mMpPlaybackData, SIGNAL( playbackInfoChanged() ), this, SLOT( playbackInfoChanged() ) );
    }
    
    TX_EXIT
}

/*!
 Get the playback state
 */
int MusicWidget::getPlaybackState()
{
    TX_ENTRY
    int state;
    if ( mMpPlaybackData ) {
        state = mMpPlaybackData->playbackState();
    }
    else {
        state = MpPlaybackData::NotPlaying;
    }
    TX_EXIT_ARGS( "playback state = " << state );
    return state;
}

/*!
 Clear the song data
 */
void MusicWidget::clearData()
{
    TX_ENTRY
    // set play button to PLAY icon.
    mPlayPushButton->setIcon( HbIcon( ICON_PLAY ) );
    updateButtons();
    
    // remove song data.
    mMarqueeText->setText( QString( " " ) );
    mMarqueeText->stopAnimation();
    mSongDataBackground->hide();
    
    // set default album art
    mAlbumArt->setIcon( HbIcon( ICON_DEFAULT_ART ) );
    mAlbumArt->setEnabled( true );
    TX_EXIT
}
