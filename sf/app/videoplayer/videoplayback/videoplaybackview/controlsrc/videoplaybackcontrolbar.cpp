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

// Version : %version:  10 %



#include "mpxvideo_debug.h"
#include "videoplaybacktoolbar.h"
#include "videoplaybackcontrolbar.h"
#include "videoplaybackprogressbar.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::VideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlBar::VideoPlaybackControlBar(
        VideoPlaybackControlsController* controller )
    : mController( controller )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlBar::VideoPlaybackControlBar"));

    //
    // create toolbar handler
    //
    mToolBar = new VideoPlaybackToolBar( mController );

    //
    // create progressbar handler
    //
    mProgressBar = new VideoPlaybackProgressBar( mController );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::~VideoPlaybackControlBar()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlBar::~VideoPlaybackControlBar()
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::~VideoPlaybackControlBar()"));

    if ( mProgressBar )
    {
        delete mProgressBar;
        mProgressBar = NULL;
    }

    if ( mToolBar )
    {
        delete mToolBar;
        mToolBar = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlBar::initialize()"));

    if ( mProgressBar )
    {
        mProgressBar->initialize();
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::updateState() state = %d"), state );

    if ( mToolBar )
    {
        mToolBar->updateState( state );
    }

    if ( mProgressBar )
    {
        mProgressBar->updateState( state );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::aspectRatioChanged() aspectRatio = %d"), aspectRatio );

    if ( mToolBar )
    {
        mToolBar->aspectRatioChanged( aspectRatio );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::updateWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::updateWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::updateWithFileDetails()"));

    if ( mToolBar )
    {
        mToolBar->updateWithFileDetails( details );
    }

    if ( mProgressBar )
    {
        mProgressBar->updateWithFileDetails( details );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::setVisibleToControlBar()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::setVisibleToControlBar( bool visible )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlBar::setVisibleToControlBar()"),
                   _L("visible = %d"), visible );

    setVisible( visible );
    mToolBar->setVisible( visible );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::durationChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::durationChanged( int duration )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::durationChanged()"));

    if ( mProgressBar )
    {
        mProgressBar->durationChanged( duration );
    }

    if ( mToolBar )
    {
        mToolBar->durationChanged( duration );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::positionChanged()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::positionChanged( int position )
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::positionChanged()"));

    if ( mProgressBar )
    {
        mProgressBar->positionChanged( position );
    }

    if ( mToolBar )
    {
        mToolBar->positionChanged( position );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlBar::resetControl()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::resetControl()
{
    MPX_DEBUG(_L("VideoPlaybackControlBar::positionChanged()"));

    if ( mProgressBar )
    {
        mProgressBar->resetControl();
    }

    if ( mToolBar )
    {
        mToolBar->resetControl();
    }
}

//End of file
