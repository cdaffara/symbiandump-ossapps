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
* Description: Music Player Playback view.
*
*/

#include <QPainter>
#include <QtGui>

#include <hbmenu.h>
#include <hbinstance.h>
#include <hbtoolbar.h>
#include <hbtoolbutton.h>
#include <hbaction.h>
#include <hbicon.h>
#include <hbmessagebox.h>

#include "mpplaybackview.h"
#include "mpplaybackwidget.h"
#include "mpenginefactory.h"
#include "mpplaybackdata.h"
#include "mpsettingsmanager.h"
#include "mpcommondefs.h"
#include "mptrace.h"
#include "mpequalizerwidget.h"


/*!
    \class MpPlaybackView
    \brief Music Player playback view.

    Playback view provides access to music playback in the device.
    This class owns the menu and the toolbar. In addition, it is
    responsible for creating UI widgets and the underlying engine
    components and connecting them to work together.
*/

/*!
    \fn void command( int command )

    This signal is emitted when the view issues a \a command to the
    application such as request to switch to a different view.
 */

/*!
 Constructs the playback view.
 */
MpPlaybackView::MpPlaybackView()
    : mMpEngine( 0 ),
      mPlaybackData( 0 ),
      mPlaybackWidget( 0 ),
      mEqualizerWidget( 0 ),
      mSoftKeyBack( 0 ),
      mActivated( false ),
      mPlayIcon( 0 ),
      mPauseIcon( 0 ),
      mTimer(0),
      mSeeking(false)

{
    TX_LOG
}

/*!
 Destructs the playback view.
 */
MpPlaybackView::~MpPlaybackView()
{
    TX_ENTRY
    delete mSoftKeyBack;
    delete mPlayIcon;
    delete mPauseIcon;
    delete mEqualizerWidget;
    TX_EXIT
}


/*!
 Initializes the playback view.
 */
void MpPlaybackView::initializeView()
{
    TX_ENTRY

    mWindow = mainWindow();

    mSoftKeyBack = new HbAction( Hb::BackNaviAction, this );
    connect( mSoftKeyBack, SIGNAL( triggered() ), this, SLOT( back() ) );

    mMpEngine = MpEngineFactory::sharedEngine();
    connect( mMpEngine, SIGNAL( corruptedStop() ), this, SLOT(showCorruptedNote() ));
    mPlaybackData = mMpEngine->playbackData();
    connect( mPlaybackData, SIGNAL( playbackStateChanged() ),
             this, SLOT( playbackStateChanged() ) );

    mPlaybackWidget = new MpPlaybackWidget( mPlaybackData );
    connect( mPlaybackWidget, SIGNAL( setPlaybackPosition( int ) ), mMpEngine, SLOT( setPosition( int ) ) );
    //repeat should not be displayed on fetcher or embedded.
    mRepeat = mViewMode == MpCommon::DefaultView ? MpSettingsManager::repeat() : false;
    mPlaybackWidget->repeatChanged( mRepeat );
    setWidget( mPlaybackWidget );
    setupMenu();
    setupToolbar();
    
    //Connect FF and RW buttons when view (toolbar) is ready
    connect ( mWindow, SIGNAL( viewReady() ), this, SLOT( connectButtons() ) );
    
    if ( mViewMode == MpCommon::DefaultView ) {
        // Observe changes in settings.
        connect( MpSettingsManager::instance(), SIGNAL( shuffleChanged( bool ) ),
                 this, SLOT( shuffleChanged( bool ) ) );
        connect( MpSettingsManager::instance(), SIGNAL( repeatChanged( bool ) ),
                 this, SLOT( repeatChanged( bool ) ) );
        connect( mMpEngine, SIGNAL( libraryUpdated() ), this, SLOT( closeEqualizerDialog() ) );
    }

    TX_EXIT
}

/*!
 Activates the playback view.
 */
void MpPlaybackView::activateView()
{
    TX_ENTRY
    mActivated = true;
    setNavigationAction( mSoftKeyBack );
    TX_EXIT
}

/*!
 Deactivates the playback view.
 */
void MpPlaybackView::deactivateView()
{
    TX_ENTRY
    menu()->close();
    setNavigationAction( 0 );
    mActivated = false;
    TX_EXIT
}

/*!
 Gets the current status of the view in a form of string
 */
QString MpPlaybackView::playbackStatus()
{
    TX_ENTRY
    QString currentStatus;
    
        switch ( mPlaybackData->playbackState() ) {
            case MpPlaybackData::NotPlaying:
                TX_LOG_ARGS( "playbackStatus: NotPlaying" )
                currentStatus = "NotPlaying";
                break;
			case MpPlaybackData::Playing:
                TX_LOG_ARGS( "playbackStatus: Playing" )
                currentStatus = "Playing";
                break;
            case MpPlaybackData::Paused:
                TX_LOG_ARGS( "playbackStatus: Paused" )
                currentStatus ="Paused";
                break;
            case MpPlaybackData::Stopped:
                TX_LOG_ARGS( "playbackStatus: Stopped" )
                currentStatus = "Stopped";
                break;
            default:
                break;
        }
    TX_EXIT
    return currentStatus;
    
}

/*!
 Gets the current Shuffle setting value
 */
bool MpPlaybackView::shuffleEnabled()
{
    TX_ENTRY
    return mShuffle;
}

/*!
 Gets the current Repeat setting value
 */
bool MpPlaybackView::repeatEnabled()
{
    TX_ENTRY
    return mRepeat;
}

/*!
 Slot to be called to activate settings view.
 */
void MpPlaybackView::startSettingsView()
{
    TX_LOG
    emit command( MpCommon::ActivateSettingsView );
}

/*!
 Slot to handle back command from softkey.
 */
void MpPlaybackView::back()
{
    TX_ENTRY
    // Stop the playback preview when returning to collection in fetch mode
    switch ( mViewMode ) {
        case MpCommon::EmbeddedView:
            TX_LOG_ARGS( "MpCommon::EmbeddedView" )
            mMpEngine->stop();
            emit songSelected( "" );
            break;
        case MpCommon::FetchView:
            TX_LOG_ARGS( "MpCommon::FetchView" )
            mMpEngine->stop(); //Intentional fallthrough        
        default: 
            emit command( MpCommon::ActivateCollectionView );
            break;
    }
    
    TX_EXIT
}

/*!
 Slot to be called to exit.
 */
void MpPlaybackView::exit()
{
    TX_ENTRY
    emit command( MpCommon::Exit );
    TX_EXIT
}

/*!
 Slot to handle playback state changed.
 */
void MpPlaybackView::playbackStateChanged()
{
    TX_ENTRY
    updatePlayPauseIcon();
    if ( MpPlaybackData::Stopped == mPlaybackData->playbackState() && mViewMode == MpCommon::FetchView ) {
          back();
    }
    TX_EXIT
}

/*!
 Slot to handle flip action.
 */
void MpPlaybackView::flip()
{
    TX_ENTRY
    mMpEngine->retrieveSongDetails();
    emit command( MpCommon::ActivateDetailsView );
    TX_EXIT
}

/*!
 Slot to handle shuffle toggle.
 */
void MpPlaybackView::toggleShuffle()
{
    mMpEngine->setShuffle( !mShuffle );
    MpSettingsManager::setShuffle( !mShuffle );
}

/*!
 Slot to handle /a shuffle setting change.
 */
void MpPlaybackView::shuffleChanged( bool shuffle )
{
    mShuffle = shuffle;
    mShuffleAction->setChecked( mShuffle );
}

/*!
 Slot to handle repeat toggle.
 */
void MpPlaybackView::toggleRepeat()
{
    mMpEngine->setRepeat( !mRepeat );
    MpSettingsManager::setRepeat( !mRepeat );
}

/*!
 Slot to handle /a repeat setting change.
 */
void MpPlaybackView::repeatChanged( bool repeat )
{
    mRepeat = repeat;
    mRepeatAction->setText( mRepeat ?  hbTrId( "txt_mus_opt_repeat_off" ) : hbTrId( "txt_mus_opt_repeat_on" ) );
    mPlaybackWidget->repeatChanged( repeat );
}

/*!
 Slot called when a song is selected in fetch mode.
 */
void MpPlaybackView::handleSongSelected()
{
    TX_ENTRY
    mMpEngine->stop();
    emit songSelected( mPlaybackData->uri() );
    TX_EXIT
}

/*!
 setup the menu.
 */
void MpPlaybackView::setupMenu()
{
    TX_ENTRY
    if ( mViewMode == MpCommon::DefaultView ) {
        mRepeat = MpSettingsManager::repeat();
        HbMenu* myMenu = new HbMenu();
        connect( myMenu->addAction( hbTrId( "txt_mus_opt_equalizer" ) ), SIGNAL( triggered() ), this, SLOT( showEqualizerDialog() ) );
        connect( myMenu->addAction( hbTrId( "txt_mus_opt_audio_effects" ) ), SIGNAL( triggered() ), this, SLOT( startSettingsView() ) );
        mRepeatAction = myMenu->addAction( mRepeat ?  hbTrId( "txt_mus_opt_repeat_off" ) : hbTrId( "txt_mus_opt_repeat_on" ) );
        connect( mRepeatAction , SIGNAL( triggered() ), this, SLOT( toggleRepeat() ) );
        connect( myMenu->addAction( hbTrId( "txt_common_opt_exit" ) ), SIGNAL( triggered() ), this, SLOT( exit() ) );
        setMenu( myMenu );
    }
    TX_EXIT
}

/*!
 setup the toolbar.
 */
void MpPlaybackView::setupToolbar()
{
    TX_ENTRY
    HbToolBar *myToolBar = toolBar();
    myToolBar->setObjectName("PlaybackToolbar");
    myToolBar->setOrientation( Qt::Horizontal );
    QActionGroup *actionsGroup = new QActionGroup( myToolBar );

    if ( mViewMode == MpCommon::DefaultView || mViewMode == MpCommon::EmbeddedView ) {
        mShuffleAction = new HbAction( actionsGroup );
        mShuffle = MpSettingsManager::shuffle();
        mShuffleAction->setIcon( HbIcon( "qtg_mono_shuffle" ) );
        mShuffleAction->setCheckable( true );
        mShuffleAction->setChecked( mShuffle );
        
        if ( mViewMode == MpCommon::DefaultView ) {
            connect( mShuffleAction, SIGNAL( triggered( bool ) ),
                         this, SLOT( toggleShuffle() ) );
        }
        else {
            mShuffleAction->setEnabled( false );
        }
        
        myToolBar->addAction( mShuffleAction );

        HbAction *action = new HbAction( actionsGroup );
        action->setIcon( HbIcon( "qtg_mono_seek_previous" ) );
        action->setCheckable( false );
        action->setObjectName("previous");
        myToolBar->addAction( action );
        
        mPlayPauseAction = new HbAction( actionsGroup );
        mPlayIcon = new HbIcon( "qtg_mono_play" );
        mPauseIcon = new HbIcon( "qtg_mono_pause" );
        updatePlayPauseIcon();
        mPlayPauseAction->setCheckable( false );
        connect( mPlayPauseAction, SIGNAL( triggered( bool ) ),
                 mMpEngine, SLOT( playPause() ) );
        myToolBar->addAction( mPlayPauseAction );

        action = new HbAction( actionsGroup );
        action->setIcon( HbIcon( "qtg_mono_seek_next" ) );
        action->setCheckable( false );
        action->setObjectName("next");
        myToolBar->addAction( action );

        HbIcon icon( "qtg_mono_info" );
        action = new HbAction( actionsGroup );
        action->setIcon( icon );
        action->setCheckable( false );

        connect( action, SIGNAL( triggered( bool ) ),
                 this, SLOT( flip() ) );

        myToolBar->addAction( action );
    }
    else {
        // Fetch mode
        HbAction *action = new HbAction( actionsGroup );
        action->setIcon( HbIcon( "qtg_mono_tick" ) );
        action->setCheckable( false );
        connect( action, SIGNAL( triggered( bool ) ),
                 this, SLOT( handleSongSelected() ) );
        myToolBar->addAction( action );

        mPlayPauseAction = new HbAction( actionsGroup );
        mPlayIcon = new HbIcon( "qtg_mono_play" );
        mPauseIcon = new HbIcon( "qtg_mono_pause" );
        updatePlayPauseIcon();
        mPlayPauseAction->setCheckable( false );
        connect( mPlayPauseAction, SIGNAL( triggered( bool ) ),
                 mMpEngine, SLOT( playPause() ) );
        myToolBar->addAction( mPlayPauseAction );
    }
    
    
    TX_EXIT
}

/*!
 Updates the play pause icon on the toolbar base on playback state.
 */
void MpPlaybackView::updatePlayPauseIcon()
{
    TX_ENTRY
    switch ( mPlaybackData->playbackState() ) {
        case MpPlaybackData::Playing:
        case MpPlaybackData::NotPlaying:
            TX_LOG_ARGS( "MpPlaybackData::Playing" )
            mPlayPauseAction->setIcon( *mPauseIcon );
            break;
        case MpPlaybackData::Paused:
        case MpPlaybackData::Stopped:
            TX_LOG_ARGS( "MpPlaybackData::Paused" )
            mPlayPauseAction->setIcon( *mPlayIcon );
            break;

        default:
            break;
    }
    TX_EXIT
}

/*!
 start rewind long press detection
 */
void MpPlaybackView::startRewindTimer()
{
    if(mTimer){
        delete mTimer;
        mTimer = 0;
    }
    mTimer = new QTimer(this);
    mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(startSeekRewind()));
    mTimer->start(600);    
}

/*!
 start fast forward long press detection
 */
void MpPlaybackView::startForwardTimer()
{
    if(mTimer){
        delete mTimer;
        mTimer = 0;
    }
    mTimer = new QTimer(this);
    mTimer->setSingleShot(true);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(startSeekForward()));
    mTimer->start(600);      
}

/*!
 start backward seeking
 */
void MpPlaybackView::startSeekRewind()
{
    mMpEngine->startSeekBackward();
    mSeeking = true;
}

/*!
 start forward seeking
 */
void MpPlaybackView::startSeekForward()
{
    mMpEngine->startSeekForward();
    mSeeking = true;
}

/*!
 end backward seeking or skip backward
 */
void MpPlaybackView::endRewind()
{
    if(mTimer){
        mTimer->stop();
        delete mTimer;
        mTimer = 0;
    }
    if( mSeeking ) {
        mMpEngine->stopSeeking();
    }
    else {
        mMpEngine->skipBackward();
    }
    mSeeking = false;
}

/*!
 end forward seeking or skip forward
 */
void MpPlaybackView::endForward()
{
    if(mTimer){
        mTimer->stop();
        delete mTimer;
        mTimer = 0;
    }
    if( mSeeking ) {
        mMpEngine->stopSeeking();
    }
    else {
        mMpEngine->skipForward();
    }
    mSeeking = false;
}

/*!
 Slot to connect seek forward and backward toolbar buttons
 */
void MpPlaybackView::connectButtons()
{
    
    HbToolBar *myToolBar = toolBar();
    QGraphicsLayout *layout = myToolBar->layout();
    if( layout ) {
        int count = layout->count();
        for ( int i = 0; i < count; i++ ) {
            HbToolButton *button = dynamic_cast<HbToolButton *>( layout->itemAt(i) );
            QString name = button->action()->objectName();
            
            if ( name == "previous" ) {
                connect( button, SIGNAL( pressed() ),
                         this, SLOT( startRewindTimer() ) );
                connect( button, SIGNAL( released() ),
                         this, SLOT( endRewind() ) );
            }
            if ( name == "next" ) {
                connect( button, SIGNAL( pressed() ),
                         this, SLOT( startForwardTimer() ) );
                connect( button, SIGNAL( released() ),
                         this, SLOT( endForward() ) );
                disconnect ( mWindow, SIGNAL( viewReady() ), this, SLOT( connectButtons() ) );
            }
        }
    }
}

/*!
 Slot to be called to show corrupted message box.
 */
void MpPlaybackView::showCorruptedNote()
{
    mMpEngine->stop();
    HbMessageBox *messageBox = new HbMessageBox( hbTrId( "txt_mus_info_unable_to_play_selection" ), HbMessageBox::MessageTypeWarning );
    messageBox->setAttribute( Qt::WA_DeleteOnClose );
    messageBox->setIcon( HbIcon( QString("qtg_small_fail") ) ); 
    connect (messageBox, SIGNAL( aboutToClose() ), this, SLOT( back() ) );
    messageBox->show();
}

/*!
 Slot to be called to activate equalizer dialog.
 */
void MpPlaybackView::showEqualizerDialog()
{
    TX_ENTRY
    if ( !mEqualizerWidget ){
        mEqualizerWidget = new MpEqualizerWidget();
        mEqualizerWidget->prepareDialog();
    }
    
    mEqualizerWidget->show();

    TX_EXIT
}

/*!
 Slot to be called when library has been updated
 */
void MpPlaybackView::closeEqualizerDialog()
{
    TX_ENTRY
    
    if ( mEqualizerWidget ) {
        mEqualizerWidget->close();
    }
    
    TX_EXIT
}
