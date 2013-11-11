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
* Description:  Implementation of VideoPlaybackVolumeControl
*
*/

// Version : %version:  1 %


#include <QTimer>
#include <hbvolumesliderpopup.h>

#include "mpxvideo_debug.h"
#include "videoplaybackvolumecontrol.h"
#include "videoplaybackcontrolscontroller.h"

const int KVolumeDragEventTimeOut = 100000;   //set volume maximum 10 times a sec during dragging


// -------------------------------------------------------------------------------------------------
// VideoPlaybackVolumeControl::VideoPlaybackVolumeControl()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackVolumeControl::VideoPlaybackVolumeControl( VideoPlaybackControlsController* controller )
    : mController( controller )
    , mVolumePopup( NULL )
    , mDraggingHandlerTimer( NULL )
    , mVolume( KPbPlaybackVolumeLevelMin )
    , mVolumeNormalizer( 0 )
    , mMutedByMuteIcon( false )
    , mDragging( false )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackVolumeControl::VideoPlaybackVolumeControl"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackVolumeControl::~VideoPlaybackVolumeControl()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackVolumeControl::~VideoPlaybackVolumeControl()
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::~VideoPlaybackVolumeControl()"));

    if ( mDraggingHandlerTimer )
    {
        disconnect( mDraggingHandlerTimer, SIGNAL( timeout() ), this, SLOT( handleDraggingTimerTimeOut() ) );

        if ( mDraggingHandlerTimer->isActive() )
        {
            mDraggingHandlerTimer->stop();
        }

        delete mDraggingHandlerTimer;
        mDraggingHandlerTimer = NULL;
    }

    if ( mVolumePopup )
    {
        disconnect( mVolumePopup, SIGNAL( valueChanged( int ) ), this, SLOT( handleSliderValueChanged( int ) ) );
        disconnect( mVolumePopup, SIGNAL( iconClicked() ), this, SLOT( handleMuteIconClicked() ) );
        disconnect( mVolumePopup, SIGNAL( sliderPressed() ), this, SLOT( handleSliderPressed() ) );
        disconnect( mVolumePopup, SIGNAL( sliderReleased() ), this, SLOT( handleSliderReleased() ) );

        delete mVolumePopup;
        mVolumePopup = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackVolumeControl::initialize()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackVolumeControl::initialize()"));

    if ( ! mVolumePopup )
    {
        //
        // Create volume popup control
        //
        mVolumePopup = new HbVolumeSliderPopup();
        mVolumePopup->setVisible( false );
        mVolumePopup->setTimeout( KControlsTimeOut );
        mVolumePopup->setTickPosition( Hb::NoSliderTicks );

        connect( mVolumePopup, SIGNAL( valueChanged( int ) ), this, SLOT( handleSliderValueChanged( int ) ), Qt::QueuedConnection );
        connect( mVolumePopup, SIGNAL( iconClicked() ), this, SLOT( handleMuteIconClicked() ) );
        connect( mVolumePopup, SIGNAL( sliderPressed() ), this, SLOT( handleSliderPressed() ) );
        connect( mVolumePopup, SIGNAL( sliderReleased() ), this, SLOT( handleSliderReleased() ) );

        if ( mController->fileDetails()->mAudioEnabled )
        {
            int volumeSteps = mController->volumeSteps();

            mVolumePopup->setRange( 0, volumeSteps );
            mVolumePopup->setSingleStep( 1 );
            mVolumeNormalizer = ( KPbPlaybackVolumeLevelMax - KPbPlaybackVolumeLevelMin ) / volumeSteps;
            mVolumePopup->setValue( mVolume / mVolumeNormalizer );

            mDraggingHandlerTimer = new QTimer();
            mDraggingHandlerTimer->setSingleShot( false );
            mDraggingHandlerTimer->setInterval( KVolumeDragEventTimeOut );
            connect( mDraggingHandlerTimer, SIGNAL( timeout() ), this, SLOT( handleDraggingTimerTimeOut() ) );
        }
        else
        {
            //
            // Dimmed the volume control if it is video only
            //
            mVolumePopup->setValue( 0 );
            mVolumePopup->setEnabled( false );
        }

    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackVolumeControl::volumeChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::volumeChanged( int volume )
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::volumeChanged() volume = %d"), volume );

    //
    // If we get expected volume from video helix, we don't need to set the volume to volume slider.
    // Or we can stay in infinite loop like this..1->2->3->4->1->2->3->4...
    // Unlike 9.2, mVolumePopup updates UI first and then emit volumechanged signal,
    // so there should be workaround to avoid infinite loop
    // 1. mVolumePopup->setValue( 5 )
    // 2. mVolumePopup emits valueChanged( 5 )
    // 3. set the volume 5 to video helix
    // 4. video helix sends voluemchanged with volume 5
    //
    if ( mExpectedVolList.length() && mExpectedVolList.first() == volume )
    {
        MPX_DEBUG(_L("VideoPlaybackVolumeControl::volumeChanged() mExpectedVolList.first() = %d"),
                mExpectedVolList.first() );

        mExpectedVolList.removeFirst();
    }
    else if ( ! mDragging )
    {
        mVolume = volume;

        if ( mVolumePopup )
        {
            mVolumePopup->setValue( mVolume / mVolumeNormalizer );
            setVisible( true );
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackVolumeControl::setVisible()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::setVisible( bool visible )
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::setVisible() visibility = %d"), visible );

    //
	// Late init. When we have to show the volume control, we create.
	//
    if ( ! mVolumePopup )
    {
        initialize();
    }

    mVolumePopup->setVisible( visible );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackVolumeControl::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackVolumeControl::isVisible()
{
    bool visible = false;

    if ( mVolumePopup )
    {
        visible = mVolumePopup->isVisible();
    }

    MPX_DEBUG(_L("VideoPlaybackVolumeControl::isVisible()"), visible );

    return visible;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackVolumeControl::handleSliderValueChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::handleSliderValueChanged( int volume )
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::handleSliderValueChanged() vol = %d"), volume );

    volume *= mVolumeNormalizer;

    //
    // If user is dragging, we won't send all the volumechanged value to video helix
    // since it causes huge traffic and issues with asynchnous calls
    // So we send commands 10 times a sec as we do in 9.2
    //
    if ( mDragging )
    {
        mDraggingVolume = volume;
    }
    else
    {
        setVolume( volume );
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackVolumeControl::handleMuteIconClicked()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::handleMuteIconClicked()
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::handleMuteIconClicked()"));

    if ( mVolumePopup->value() )
    {
        mController->handleCommand( EMPXPbvCmdUnMute );
    }
    else
    {
        mMutedByMuteIcon = true;
        mController->handleCommand( EMPXPbvCmdMute );
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackVolumeControl::handleSliderPressed()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::handleSliderPressed()
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::handleSliderPressed()"));

    mDragging = true;

    if ( mDraggingHandlerTimer && mDraggingHandlerTimer->isActive() )
    {
        mDraggingHandlerTimer->stop();
    }

    mDraggingHandlerTimer->start( 0 );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackVolumeControl::handleSliderReleased()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::handleSliderReleased()
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::handleSliderReleased()"));

    mDragging = false;

    if ( mDraggingHandlerTimer )
    {
        mDraggingHandlerTimer->stop();
    }

    setVolume( mDraggingVolume );
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackVolumeControl::handleDraggingTimerTimeOut()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::handleDraggingTimerTimeOut()
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::handleDraggingTimerTimeOut() mDraggingVolume = %d")
            ,mDraggingVolume );

    if ( mDragging )
    {
        setVolume( mDraggingVolume );
    }
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackVolumeControl::setVolume()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackVolumeControl::setVolume( int volume )
{
    MPX_DEBUG(_L("VideoPlaybackVolumeControl::setVolume() mVolume = %d, volume = %d")
            , mVolume, volume );

    if ( mVolume != volume )
    {
        mVolume = volume;

        if ( mMutedByMuteIcon && volume == 0 )
        {
            MPX_DEBUG(_L("VideoPlaybackVolumeControl::setVolume() don't need to update volume right after muted"));

            //
            // If this function gets called sincen user tap on mute icon to mute,
            // Then add min value to the expectedlist. playback plugin will send min value to ui.
            //
            mExpectedVolList.append( KPbPlaybackVolumeLevelMin );

            //
            // Don't need to update volume '0' right after muted.
            // If we set '0' to playbackplugin after muted,
            // it won't go back to previous volume with unmuted through media key or remote
            //
            mMutedByMuteIcon = false;
        }
        else
        {
            MPX_DEBUG(_L("VideoPlaybackVolumeControl::setVolume() setvolume = %d"), mVolume );

            mExpectedVolList.append( mVolume );
            mController->handleCommand( EMPXPbvCmdSetVolume, mVolume );
        }
    }
}

//End of file
