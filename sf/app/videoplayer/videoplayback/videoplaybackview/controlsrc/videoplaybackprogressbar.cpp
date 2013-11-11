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
* Description:  Implementation of VideoPlaybackProgressBar
*
*/

// Version : %version: da1mmcf#32 %




#include <QTimer>

#include <hbframeitem.h>
#include <hbframedrawer.h>
#include <hbprogressslider.h>

#include "mpxvideo_debug.h"
#include "videoplaybackprogressbar.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackcontrolscontroller.h"

const int KSeekingTimeOut = 250;

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::VideoPlaybackProgressBar
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackProgressBar::VideoPlaybackProgressBar(
        VideoPlaybackControlsController* controller )
    : mController( controller )
    , mDraggingPosition( 0 )
    , mSetPosition( -1 )
    , mNeedToResumeAfterSetPosition( false )
    , mInitialized( false )
    , mSliderDragging( false )
    , mLongTimeFormat( false )
    , mLiveStreaming( false )
    , mSeekingTimer( NULL )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackProgressBar::VideoPlaybackProgressBar()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::~VideoPlaybackProgressBar
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackProgressBar::~VideoPlaybackProgressBar()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackProgressBar::~VideoPlaybackProgressBar()"));

    if ( mSeekingTimer )
    {
        disconnect( mSeekingTimer, SIGNAL( timeout() ), this, SLOT( handleSeekingTimeout() ) );

        if ( mSeekingTimer->isActive() )
        {
            mSeekingTimer->stop();
        }

        delete mSeekingTimer;
        mSeekingTimer = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::initialize
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackProgressBar::initialize()"));

    HbExtendedLocale mLocale = HbExtendedLocale::system();
    VideoPlaybackDocumentLoader *loader = mController->layoutLoader();

    //
    // Don't need to initialize buttons once it gets initialized
    //
    if ( loader && ! mInitialized )
    {
        mInitialized = true;
        mLiveStreaming =
                ( mController->fileDetails()->mPlaybackMode == EMPXVideoLiveStreaming )? true:false;

        //
        // Create a timer for seeking.
        // We will issue SetPosition every KSeekingTimeOut msec to show the current frame to user
        //
        mSeekingTimer = new QTimer();
        mSeekingTimer->setSingleShot( false );
        mSeekingTimer->setInterval( KSeekingTimeOut );
        connect( mSeekingTimer, SIGNAL( timeout() ), this, SLOT( handleSeekingTimeout() ) );

        //
        // progress slider
        //
        QGraphicsWidget *widget = loader->findWidget( QString( "progressSlider" ) );
        mProgressSlider = qobject_cast<HbProgressSlider*>( widget );

        connect( mProgressSlider, SIGNAL( sliderPressed() ), this, SLOT( handleSliderPressed() ) );
        connect( mProgressSlider, SIGNAL( sliderReleased() ), this, SLOT( handleSliderReleased() ) );
        connect( mProgressSlider, SIGNAL( sliderMoved( int ) ), this, SLOT( handleSliderMoved( int ) ) );
        connect( mProgressSlider, SIGNAL( trackPressed() ), this ,SLOT( handleSliderPressed() ) );
        connect( mProgressSlider, SIGNAL( trackReleased() ), this ,SLOT( handleSliderReleased() ) );

        //
        // If we init the progress bar after pp sends the duration informatin
        // we need to set the duration manually
        //
        durationChanged( (qreal)mController->fileDetails()->mDuration / (qreal)KPbMilliMultiplier );

        //
        // Set the position to 0 until we get position information
        //
        positionChanged( 0 );

        //
        // Set framedrawer for semi transparent background
        //
        HbFrameItem *frameItem = new HbFrameItem();
        frameItem->frameDrawer().setFrameGraphicsName( "qtg_fr_multimedia_trans" );
        frameItem->frameDrawer().setFrameType( HbFrameDrawer::NinePieces );
        frameItem->frameDrawer().setFillWholeRect( true );
        mProgressSlider->setBackgroundItem( frameItem );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::durationChanged
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::durationChanged duration = %d"), duration );

    if ( mLiveStreaming )
    {
        mDuration = 0;
        mProgressSlider->setMaxText( hbTrId( "txt_videos_slidervalue_live" ) );
    }
    else
    {
        mDuration = duration;

        if ( ( mDuration / 3600 ) > 0 )
        {
            mLongTimeFormat = true;
        }
        else
        {
            mLongTimeFormat = false;
        }

        mProgressSlider->setMaxText( valueToReadableFormat( mDuration ) );
    }

    mProgressSlider->setRange( 0, mDuration );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::positionChanged( int position )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::positionChanged position = %d"), position );

    //
    // While dragging event, don't update old position information from mpx framework
    //
    if ( ! mSliderDragging )
    {
        updatePostion( position );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::updatePostion
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::updatePostion( int position )
{
    if ( position < 0 )
    {
        position = 0;
    }
    else if ( position > mDuration && ! mLiveStreaming )
    {
        position = mDuration;
    }

    MPX_DEBUG(_L("VideoPlaybackProgressBar::updatePostion position = %d"), position );

    mProgressSlider->setMinText( valueToReadableFormat( position ) );

    //
    // Don't need to set the slider for live streaming
    //
    if ( ! mLiveStreaming )
    {
        mProgressSlider->setSliderValue( position );
        mProgressSlider->setProgressValue( position );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::valueToReadableFormat
// -------------------------------------------------------------------------------------------------
//
QString VideoPlaybackProgressBar::valueToReadableFormat( int value )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::valueToReadableFormat value = %d"), value);

    QString str, hourString, minString, secString;

    int hours = value / 3600;
    value = value % 3600;
    int mins = value / 60;
    value = value % 60;
    int secs = value;

    //
    // show like this
    // 0:03, 10:03, 0:05:03, 12:12:12
    //
    if ( mins < 10 && mLongTimeFormat )
    {
        minString = mLocale.toString( 0 );
    }
    minString.append( mLocale.toString( mins ) );

    if ( secs < 10 )
    {
        secString = mLocale.toString( 0 );
    }
    secString.append( mLocale.toString( secs ) );

    //
    // Formatting
    //
    if ( mLongTimeFormat )
    {
        hourString = mLocale.toString( hours );

        str = hbTrId( "txt_videos_slidervalue_l1l2l3_2" )
                .arg( hourString ).arg( minString ).arg( secString );
    }
    else
    {
        str = hbTrId( "txt_videos_slidervalue_l1l2" ).arg( minString ).arg( secString );
    }

    return str;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::handleSliderPressed
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::handleSliderPressed()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackProgressBar::handleSliderPressed()"));

    mSliderDragging = true;

    mController->resetDisappearingTimers( ETimerCancel );

    //
    // Pause the playback if it's playing
    //
    if( mController->state() == EPbStatePlaying )
    {
        mNeedToResumeAfterSetPosition = true;
        mController->handleCommand( EMPXPbvCmdCustomPause );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::handleSliderMoved
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::handleSliderMoved( int value )
{
    MPX_DEBUG(_L("VideoPlaybackProgressBar::handleSliderMoved() position = %d"), value);

    updatePostion( value );

    //
    // If the slider is dragging, start the timer and seek every KSeekingTimeOut msec
    //
    if ( mSliderDragging )
    {
        mDraggingPosition = value;
        MPX_DEBUG(_L("VideoPlaybackProgressBar::handleSliderMoved() mDraggingPosition = %d"), mDraggingPosition);

        if ( mSeekingTimer && ! mSeekingTimer->isActive() )
        {
            mSeekingTimer->start();
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::handleSliderReleased
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::handleSliderReleased()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackProgressBar::handleSliderReleased()"));

    mSliderDragging = false;

    if ( mSeekingTimer && mSeekingTimer->isActive() )
    {
        mSeekingTimer->stop();
    }

    mController->resetDisappearingTimers( ETimerReset );
    int value = mProgressSlider->sliderValue();

    if ( value >= mDuration )
    {
        MPX_DEBUG(_L("VideoPlaybackProgressBar::setPosition() reached end of the clip"));
        mController->handleCommand( EMPXPbvCmdEndOfClip );
    }
    else
    {
        if ( value < 0 )
        {
            value = 0;
        }

        MPX_DEBUG(_L("VideoPlaybackProgressBar::setPosition() position = %d"), value);
        mController->handleCommand( EMPXPbvCmdSetPosition, value );

        //
        // Resume if it was playing
        //
        if( mNeedToResumeAfterSetPosition )
        {
            mNeedToResumeAfterSetPosition = false;
            mController->handleCommand( EMPXPbvCmdCustomPlay );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::updateWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    Q_UNUSED( details );

    MPX_DEBUG(_L("VideoPlaybackProgressBar::updateControlsWithFileDetails()"));

    setEnableProgressSlider( true );

    mProgressSlider->backgroundItem()->setVisible(
            ( mController->viewMode() == EFullScreenView )? ETrue:EFalse );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackProgressBar::updateState() state = %d"), state );

    switch ( state )
    {
        case EPbStatePlaying:
        case EPbStatePaused:
        {
            setEnableProgressSlider( true );

            break;
        }
        case EPbStatePluginSeeking:
        {
            break;
        }
        default:
        {
            setEnableProgressSlider( false );

            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::handleSeekingTimeout()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::handleSeekingTimeout()
{
    if ( mDraggingPosition >= mDuration )
    {
        mDraggingPosition = mDuration;
    }
    else if( mDraggingPosition < 0 )
    {
        mDraggingPosition = 0;
    }

    MPX_DEBUG(_L("VideoPlaybackProgressBar::handleSeekingTimeout() Dragging pos = %d, Set pos = %d")
            , mDraggingPosition, mSetPosition );

    if ( mSetPosition != mDraggingPosition )
    {
        mSetPosition = mDraggingPosition;
        mController->handleCommand( EMPXPbvCmdSetPosition, mSetPosition );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::setEnableProgressSlider()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::setEnableProgressSlider( bool enable )
{
    MPX_DEBUG(_L("VideoPlaybackProgressBar::setEnableProgressSlider() enable = %d"), enable);

    if ( ! enable ||
         ! mController->fileDetails()->mPausableStream ||
         ! mController->fileDetails()->mSeekable )
    {
        if ( mProgressSlider->isEnabled() )
        {
            mProgressSlider->setEnabled( false );
        }
    }
    else if ( ! mProgressSlider->isEnabled() )
    {
        mProgressSlider->setEnabled( true );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::resetControl
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::resetControl()
{
    MPX_DEBUG(_L("VideoPlaybackProgressBar::resetControl"));

    if ( mSliderDragging )
    {
        handleSliderReleased();
    }
}

//End of file
