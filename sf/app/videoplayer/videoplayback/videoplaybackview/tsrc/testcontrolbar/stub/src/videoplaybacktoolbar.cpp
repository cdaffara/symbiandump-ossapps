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

// Version : %version: 3 %



#include <hbpushbutton.h>

#include "mpxvideo_debug.h"
#include "videoplaybacktoolbar.h"
#include "videoplaybackviewfiledetails.h"
#include "mpxcommonvideoplaybackview.hrh"


// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::VideoPlaybackToolBar()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackToolBar::VideoPlaybackToolBar(
        VideoPlaybackControlsController* controller )
    : mController( controller )
    , mReset( false )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::VideoPlaybackToolBar"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::~VideoPlaybackToolBar()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackToolBar::~VideoPlaybackToolBar()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::~VideoPlaybackToolBar()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackToolBar::initialize()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::updateState() state = %d"), state );

    mState = state;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    mAspectRatio = aspectRatio;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::updateWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::updateWithFileDetails()"));

    mFileDetails = details;
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
// VideoPlaybackToolBar::positionChanged
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::positionChanged( int position )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::positionChanged position = %d"), position );

    mPosition = position;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::setVisible
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::setVisible( bool visible )
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::setVisible position = %d"), visible );

    mVisible = visible;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackToolBar::resetControl()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackToolBar::resetControl()
{
    MPX_DEBUG(_L("VideoPlaybackToolBar::resetControl()"));

    mReset = true;
}

//End of file
