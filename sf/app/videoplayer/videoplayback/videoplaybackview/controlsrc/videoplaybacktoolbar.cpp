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
* Description:  Implementation of VideoPlaybackToolBar
*
*/

// Version : %version:  14 %



#include <QTimer>
#include <QGraphicsLayout>

#include <hbaction.h>
#include <hbtoolbar.h>
#include <hbinstance.h>
#include <hbtoolbutton.h>
#include <w32std.h>

#include "mpxvideo_debug.h"
#include "videobaseplaybackview.h"
#include "videoplaybacktoolbar.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"

const int KSeekStartTimeOut = 700;
const int KRetrieveButtonTimeOut = 1000;

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::VideoPlaybackToolBar()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackToolBar::VideoPlaybackToolBar(
        VideoPlaybackControlsController* controller )
    : mController( controller )
    , mSeekStartTimer( NULL )
    , mRetrieveButtonTimer( NULL )
    , mSeekingState( ENotSeeking )
    , mInitialized( false )
    , mPosition( 0 )
    , mDuration( 0 )
    , mAspectRatio( EMPXPbvCmdNaturalAspectRatio )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::VideoPlaybackToolBar"));

    mToolBar = mController->view()->toolBar();
    mToolBar->setOrientation( Qt::Horizontal );

    mController->view()->hideItems( Hb::ToolBarItem );

    initialize();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::~VideoPlaybackToolBar()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackToolBar::~VideoPlaybackToolBar()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::~VideoPlaybackToolBar()"));

    mToolBar->clearActions();

    for ( int i = 0 ; i < mButtonIcons.count() ; i++ )
    {
        if ( mButtonIcons[i] )
        {
            delete mButtonIcons[i];
        }
    }
    mButtonIcons.clear();

    if ( mSeekStartTimer )
    {
        if ( mSeekStartTimer->isActive() )
        {
            mSeekStartTimer->stop();
        }

        delete mSeekStartTimer;
        mSeekStartTimer = NULL;
    }

    if ( mRetrieveButtonTimer )
    {
        if ( mRetrieveButtonTimer->isActive() )
        {
            mRetrieveButtonTimer->stop();
        }

        delete mRetrieveButtonTimer;
        mRetrieveButtonTimer = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::initialize()"));

    //
    // Don't need to initialize buttons once it gets initialized
    //
    if ( ! mInitialized )
    {
        mInitialized = true;

        QActionGroup *actionsGroup = new QActionGroup( mToolBar );

        for ( int i = 0 ; i < EButtonCount ; i++ )
        {
            mButtonActions.append( new HbAction( actionsGroup ) );
        }

        for ( int i = 0 ; i < EIconCount ; i++ )
        {
            mButtonIcons.append( new HbIcon() );
        }

        //
        // RW button
        //
        mButtonIcons[ERWIcon]->setIconName( "qtg_mono_previous" );
        mButtonActions[E2ndButton]->setIcon( *mButtonIcons[ERWIcon] );

        //
        // Play/Pause button
        //
        mButtonIcons[EPlayIcon]->setIconName( "qtg_mono_play" );
        mButtonIcons[EPauseIcon]->setIconName( "qtg_mono_pause" );
        mButtonActions[E3rdButton]->setIcon( *mButtonIcons[EPlayIcon] );

        //
        // FF button
        //
        mButtonIcons[EFFIcon]->setIconName( "qtg_mono_next" );
        mButtonActions[E4thButton]->setIcon( *mButtonIcons[EFFIcon] );

        //
        // Details button
        //
        mButtonIcons[EDetailsIcon]->setIconName( "qtg_mono_info" );
        mButtonActions[E5thButton]->setIcon( *mButtonIcons[EDetailsIcon] );

        //
        // Aspect ratio buttons
        //
        mButtonIcons[ENaturalIcon]->setIconName( "qtg_mono_aspect_ratio_natural" );
        mButtonIcons[EStretchIcon]->setIconName( "qtg_mono_aspect_ratio_stretched" );
        mButtonIcons[EZoomIcon]->setIconName( "qtg_mono_aspect_ratio_zoom" );
        mButtonActions[E1stButton]->setIcon( *mButtonIcons[ENaturalIcon] );

        //
        // Attach/Share button
        //
        mButtonIcons[EAttachIcon]->setIconName( "qtg_mono_attach" );
        mButtonIcons[EShareIcon]->setIconName( "qtg_mono_share" );

        for ( int i = 0 ; i < EButtonCount ; i++ )
        {
            mButtonActions[i]->setCheckable( false );
            mToolBar->addAction( mButtonActions[i] );
        }

        mDuration = (qreal)mController->fileDetails()->mDuration / (qreal)KPbMilliMultiplier;

        //
        // Create a timer for seeking.
        // We will issue SetPosition every KSeekingTimeOut msec to show the current frame to user
        //
        mSeekStartTimer = new QTimer();
        mSeekStartTimer->setSingleShot( true );
        mSeekStartTimer->setInterval( KSeekStartTimeOut );

        //
        // get window size
        //
        RWindow *window = mController->view()->getWindow();
        TRect displayRect = TRect( TPoint( window->Position() ), TSize( window->Size() ) );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::playPause()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::playPause()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::playPause()"));

    mController->resetDisappearingTimers( ETimerReset );
    mController->handleCommand( EMPXPbvCmdPlayPause );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::ffPressing()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::ffPressing()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::ffPressing()"),
                   _L("mSeekingState = %d"), mSeekingState );

    if ( mSeekingState == ENotSeeking )
    {
        mSeekingState = EFastForwarding;

        mController->handleCommand( EMPXPbvCmdSeekForward );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::rwPressing()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::rwPressing()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::rwPressing()"),
                   _L("mSeekingState = %d"), mSeekingState );

    if ( mSeekingState == ENotSeeking )
    {
        mSeekingState = ERewinding;

        mController->handleCommand( EMPXPbvCmdSeekBackward );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::ffReleased()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::ffReleased()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::ffReleased()"),
                   _L("mSeekingState = %d"), mSeekingState );

    disconnect( mSeekStartTimer, SIGNAL( timeout() ), this, SLOT( ffPressing() ) );

    if ( mSeekStartTimer->isActive() )
    {
        mSeekStartTimer->stop();
    }

    if ( mSeekingState == EFastForwarding )
    {
        mController->handleCommand( EMPXPbvCmdEndSeek );
    }
    else
    {
        int temp = mPosition + KMPXFastForward;
        MPX_DEBUG(_L("VideoPlaybackToolBar::ffReleased() temp position = %d"), temp);

        //
        // If it has the playing time which is less than KMPXFastForward, ignore
        //
        if ( temp < mDuration )
        {
            mController->handleCommand( EMPXPbvCmdSetPosition, temp );
        }
    }

    mSeekingState = ENotSeeking;
    mController->resetDisappearingTimers( ETimerReset );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::rwReleased()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::rwReleased()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::rwReleased()"),
                   _L("mSeekingState = %d"), mSeekingState );

    disconnect( mSeekStartTimer, SIGNAL( timeout() ), this, SLOT( rwPressing() ) );

    if ( mSeekStartTimer->isActive() )
    {
        mSeekStartTimer->stop();
    }

    if ( mSeekingState == ERewinding )
    {
        mController->handleCommand( EMPXPbvCmdEndSeek );
    }
    else
    {
        int temp = mPosition + KMPXRewind;
        MPX_DEBUG(_L("VideoPlaybackToolBar::rwReleased() temp position = %d"), temp);

        //
        // If it played less than KMPXRewind, jump to 0
        //
        if ( temp < 0 )
        {
            temp = 0;
        }

        mController->handleCommand( EMPXPbvCmdSetPosition, temp );
    }

    mSeekingState = ENotSeeking;
    mController->resetDisappearingTimers( ETimerReset );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::changeAspectRatio()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::changeAspectRatio()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::changeAspectRatio()"));

    mController->resetDisappearingTimers( ETimerReset );

    TMPXVideoPlaybackViewCommandIds cmd = EMPXPbvCmdStretchAspectRatio;

    switch( mAspectRatio )
    {
        case EMMFZoom:
        {
            cmd = EMPXPbvCmdNaturalAspectRatio;
            break;
        }
        case EMMFStretch:
        {
            cmd = EMPXPbvCmdZoomAspectRatio;
            break;
        }
    }

    mController->handleCommand( cmd );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::updateState() state = %d"), state );

    switch ( state )
    {
        case EPbStatePlaying:
        {
            mToolBar->setEnabled( true );

            mButtonActions[E3rdButton]->setIcon( *mButtonIcons[EPauseIcon] );

            if ( ! mController->fileDetails()->mPausableStream  )
            {
                //
                // non-pausable stream, disable 'E3rdButton' action (for 'Pause' button)
                //
                mButtonActions[E3rdButton]->setEnabled( false );
            }

            break;
        }
        case EPbStatePaused:
        {
            mToolBar->setEnabled( true );

            mButtonActions[E3rdButton]->setIcon( *mButtonIcons[EPlayIcon] );

            if ( ! mButtonActions[E3rdButton]->isEnabled() )
            {
                //
                // enable 'E3rdButton' action (for 'Play' button) in 'Paused' state
                //
                mButtonActions[E3rdButton]->setEnabled( true );
            }

            break;
        }
        case EPbStatePluginSeeking:
        {
            break;
        }
        default:
        {
            mToolBar->setEnabled( false );

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    mAspectRatio = aspectRatio;

    setAspectRatioButton();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::handleButtonPressed()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::handleButtonPressed()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::handleButtonPressed()"));

    mController->resetDisappearingTimers( ETimerCancel );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::handleRWButtonPressed()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::handleRWButtonPressed()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::handleRWButtonPressed()"));

    mController->resetDisappearingTimers( ETimerCancel );

    connect( mSeekStartTimer, SIGNAL( timeout() ), this, SLOT( rwPressing() ) );

    if ( mSeekStartTimer->isActive() )
    {
        mSeekStartTimer->stop();
    }

    mSeekStartTimer->start();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::handleFFButtonPressed()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::handleFFButtonPressed()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::handleButtonPressed()"));

    mController->resetDisappearingTimers( ETimerCancel );

    connect( mSeekStartTimer, SIGNAL( timeout() ), this, SLOT( ffPressing() ) );

    if ( mSeekStartTimer->isActive() )
    {
        mSeekStartTimer->stop();
    }

    mSeekStartTimer->start();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::updateWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::updateWithFileDetails()"));

    if ( mController->isAttachOperation() )
    {
        //
        // Show attach button
        //
        mButtonActions[E1stButton]->setIcon( *mButtonIcons[EAttachIcon] );

        if ( mButtons.count() )
        {
            disconnect( mButtons[E1stButton], SIGNAL( released() ), 0, 0 );
            connect( mButtons[E1stButton], SIGNAL( released() ),
                     mController, SLOT( attachVideo() ) );
        }
    }
    else
    {
        if ( mController->viewMode() == EFullScreenView )
        {
            setAspectRatioButton();

            if ( mButtons.count() )
            {
                disconnect( mButtons[E1stButton], SIGNAL( released() ), 0, 0 );
                connect( mButtons[E1stButton], SIGNAL( released() ),
                         this, SLOT( changeAspectRatio() ) );
            }
        }
        else if ( mController->viewMode() == EAudioOnlyView )
        {
            //
            // Show share button
            //
            mButtonActions[E1stButton]->setIcon( *mButtonIcons[EShareIcon] );

            if ( mController->fileDetails()->mPlaybackMode == EMPXVideoStreaming ||
                 mController->fileDetails()->mPlaybackMode == EMPXVideoLiveStreaming )
            {
                //
                // dim 'share' button for streaming
                //
                mButtonActions[E1stButton]->setEnabled( false );
            }
            else
            {
                //
                // enable 'share' button
                //
                mButtonActions[E1stButton]->setEnabled( true );

                if ( mButtons.count() )
                {
                    disconnect( mButtons[E1stButton], SIGNAL( released() ), 0, 0 );
                    connect( mButtons[E1stButton], SIGNAL( released() ),
                             mController, SLOT( sendVideo() ) );
                }
            }
        }
    }

    //
    // for audio-only clips and tv-out, default view is audionOnlyView
    // therefore, dim details button
    //
    if ( ! details->mVideoEnabled || details->mTvOutConnected )
    {
        mButtonActions[E5thButton]->setEnabled( false );
    }
    else
    {
        mButtonActions[E5thButton]->setEnabled( true );
    }

    if ( details->mSeekable && details->mPausableStream )
    {
        mButtonActions[E2ndButton]->setEnabled( true );
        mButtonActions[E4thButton]->setEnabled( true );
    }
    else
    {
        mButtonActions[E2ndButton]->setEnabled( false );
        mButtonActions[E4thButton]->setEnabled( false );
    }

    //
    // The buttons 'Play' and 'Pause' are shared by the same 'E3rdButton' action.
    // The logic to enable or disable 'E3rdButton' will depend on the current playback state.
    //
    updateState( mController->state() );

    //
    // toolbar creates button once it gets visible, so we don't know exact timing when toolbar
    // creates button, so start timer to get layout information once the toolbar gets visible.
    // This is needed since we don't use toolbar in proper way.
    //
    if ( ! mRetrieveButtonTimer && ! mButtons.count() )
    {
        mRetrieveButtonTimer = new QTimer();
        mRetrieveButtonTimer->setSingleShot( false );
        mRetrieveButtonTimer->setInterval( KRetrieveButtonTimeOut );
        connect( mRetrieveButtonTimer, SIGNAL( timeout() ), this, SLOT( retrieveButtons() ) );

        mRetrieveButtonTimer->start();
    }

    durationChanged( (qreal)mController->fileDetails()->mDuration / (qreal)KPbMilliMultiplier );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::openDetailsView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::openDetailsView()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::openDetailsView()"));

    TPlaybackViewMode viewMode = mController->viewMode();

    if ( viewMode == EFullScreenView )
    {
        mController->evaluateAndChangeViewMode( EDetailsView, true );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::positionChanged( int position )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::positionChanged position = %d"), position );

    mPosition = position;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::durationChanged
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::durationChanged duration = %d"), duration );

    mDuration = duration;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::setVisible
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::setVisible( bool visible )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::setVisible visible = %d"), visible );

    if ( visible )
    {
        mController->view()->showItems( Hb::ToolBarItem );
    }
    else
    {
        mController->view()->hideItems( Hb::ToolBarItem );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::retrieveButtons()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::retrieveButtons()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::retrieveButtons()"));

    if ( mButtons.count() == 0 )
    {
        QGraphicsLayout *layout = mToolBar->layout();

        if ( layout )
        {
            if ( mRetrieveButtonTimer )
            {
                disconnect( mRetrieveButtonTimer, SIGNAL( timeout() ), this, SLOT( retrieveButtons() ) );
                if ( mRetrieveButtonTimer->isActive() )
                {
                    mRetrieveButtonTimer->stop();
                }
            }

            for ( int i = 0 ; i < layout->count() ; i++ )
            {
                mButtons.append( dynamic_cast<HbToolButton*>( layout->itemAt( i ) ) );
            }

            //
            // Connect signal and slots
            //
            connect( mButtons[E1stButton], SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );

            connect( mButtons[E2ndButton], SIGNAL( pressed() ), this, SLOT( handleRWButtonPressed() ) );
            connect( mButtons[E2ndButton], SIGNAL( released() ), this, SLOT( rwReleased() ) );

            connect( mButtons[E3rdButton], SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
            connect( mButtons[E3rdButton], SIGNAL( released() ), this, SLOT( playPause() ) );

            connect( mButtons[E4thButton], SIGNAL( pressed() ), this, SLOT( handleFFButtonPressed() ) );
            connect( mButtons[E4thButton], SIGNAL( released() ), this, SLOT( ffReleased() ) );

            connect( mButtons[E5thButton], SIGNAL( pressed() ), this, SLOT( handleButtonPressed() ) );
            connect( mButtons[E5thButton], SIGNAL( released() ), this, SLOT( openDetailsView() ) );

            if ( mController->isAttachOperation() )
            {
                connect( mButtons[E1stButton], SIGNAL( released() ),
                         mController, SLOT( attachVideo() ) );
            }
            else
            {
                if ( mController->viewMode() == EFullScreenView )
                {
                    connect( mButtons[E1stButton], SIGNAL( released() ),
                             this, SLOT( changeAspectRatio() ) );
                }
                else if ( mController->viewMode() == EAudioOnlyView )
                {
                    connect( mButtons[E1stButton], SIGNAL( released() ),
                             mController, SLOT( sendVideo() ) );
                }
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::resetControl
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::resetControl()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::resetControl"));

    switch( mSeekingState )
    {
        case EFastForwarding:
        {
            ffReleased();
            break;
        }
        case ERewinding:
        {
            rwReleased();
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::setAspectRatioButton
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::setAspectRatioButton()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::setAspectRatioButton"));

    //
    // If we are in attach service or audio only view, then don't update the icon.
    // Aspect ratio icon slots are shared with attach and share icon.
    // Just update the mAspectRatio
    // and once we go back to full screen, we will show the correct aspect ratio icon
    //
    if ( ! mController->isAttachOperation() && mController->viewMode() == EFullScreenView )
    {
        switch( mAspectRatio )
        {
            case EMMFNatural:
            {
                mButtonActions[E1stButton]->setIcon( *mButtonIcons[EStretchIcon] );
                break;
            }
            case EMMFStretch:
            {
                mButtonActions[E1stButton]->setIcon( *mButtonIcons[EZoomIcon] );
                break;
            }
            default:
            {
                mButtonActions[E1stButton]->setIcon( *mButtonIcons[ENaturalIcon] );
                break;
            }
        }

        if ( mController->fileDetails()->mAspectRatioChangeable )
        {
            if ( ! mButtonActions[E1stButton]->isEnabled() )
            {
                mButtonActions[E1stButton]->setEnabled( true );
            }
        }
        else
        {
            if ( mButtonActions[E1stButton]->isEnabled() )
            {
                mButtonActions[E1stButton]->setEnabled( false );
            }
        }
    }
}

//End of file
