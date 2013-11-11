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
* Description:  Implementation of VideoPlaybackControlBar
*
*/

// Version : %version:  4 %



#include <hbframeitem.h>
#include <hbframedrawer.h>

#include "mpxvideo_debug.h"
#include "videoplaybackcontrolbar.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::VideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlBar::VideoPlaybackControlBar(
        VideoPlaybackControlsController* controller )
    : mController( controller )
    , mReset( false )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlBar::VideoPlaybackControlBar"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::~VideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlBar::~VideoPlaybackControlBar()
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::~VideoPlaybackControlBar()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::updateState() state = %d"), state );

    mState = state;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    mAspectRatio = aspectRatio;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::updateWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::updateWithFileDetails()"));

    mFileDetails = details;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::setVisibleToControlBar()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::setVisibleToControlBar( bool visible )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::setVisibleToControlBar()"));

    mVisibility = visible;
}


// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::durationChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::durationChanged()"));

    mDuration = duration;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::positionChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::positionChanged( int position )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::positionChanged()"));

    mPosition = position;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::resetControl()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::resetControl()
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::resetControl()"));

    mReset = true;
}

//End of file
