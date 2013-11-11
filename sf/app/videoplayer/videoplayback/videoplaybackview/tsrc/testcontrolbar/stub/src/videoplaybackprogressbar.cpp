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
* Description:  Implementation of VideoPlaybackProgressBar
*
*/

// Version : %version:  3 %




#include "mpxvideo_debug.h"
#include "videoplaybackprogressbar.h"
#include "videoplaybackcontrolscontroller.h"

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::VideoPlaybackProgressBar
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackProgressBar::VideoPlaybackProgressBar(
        VideoPlaybackControlsController* controller )
    : mController( controller )
    , mReset( false )
    , mDuration( -1 )
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
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::initialize
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackProgressBar::initialize()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::durationChanged
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::durationChanged duration = %d"), duration );

    mDuration = duration;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::positionChanged( int position )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::positionChanged position = %d"), position );

    mPosition = position;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::updateWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("VideoPlaybackProgressBar::updateControlsWithFileDetails()"));

    mFileDetails = details;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackProgressBar::updateState() state = %d"), state );

    mState = state;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackProgressBar::resetControl()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackProgressBar::resetControl()
{
    MPX_DEBUG(_L("VideoPlaybackProgressBar::resetControl()"));

    mReset = true;
}

//End of file
