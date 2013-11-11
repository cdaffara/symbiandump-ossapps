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
* Description:  Implementation of VideoPlaybackFullScreenControl
*
*/

// Version : %version:  6 %



#include <w32std.h>

#include "mpxvideo_debug.h"
#include "videoplaybackfullscreencontrol.h"
#include "videoplaybackfiledetailswidget.h"
#include "videoplaybackcontrolscontroller.h"
#include "videoplaybackdetailsplaybackwindow.h"


// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::VideoPlaybackFullScreenControl()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackFullScreenControl::VideoPlaybackFullScreenControl(
        VideoPlaybackControlsController* controller,
        TVideoPlaybackControls index,
        HbWidget* widget,
        TUint controlproperties )
    : mController( controller )
    , mControl( widget )
    , mControlIndex( index )
    , mProperties( controlproperties )
    , mControlReset( false )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackFullScreenControl::VideoPlaybackFullScreenControl()"));

    setParent( mController );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::~VideoPlaybackFullScreenControl()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackFullScreenControl::~VideoPlaybackFullScreenControl()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::~VideoPlaybackFullScreenControl"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::setVisible()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::setVisible( bool visible )
{
    mVisible = visible;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::isVisible()
{
    return mVisible;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::setVisibility()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::setVisibility( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::setVisibility(%d)"), state);

    mVisibilityState = state;
    setVisible( true );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::controlIndex()
// -------------------------------------------------------------------------------------------------
//
TVideoPlaybackControls VideoPlaybackFullScreenControl::controlIndex()
{
    return mControlIndex;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::volumeChanged()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::volumeChanged( int volume )
{
    mVolume = volume;
    return true;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::durationChanged()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::durationChanged( int duration )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::durationChanged() [%d]"), duration );
    mDuration = duration;
    return true;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::positionChanged()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::positionChanged( int position )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::positionChanged() [%d]"), position );
    mPosition = position;
    return true;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::aspectRatioChanged( int aspectRatio )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::aspectRatioChanged() [%d]"), aspectRatio );
    mAspectRatio = aspectRatio;
    return true;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackFullScreenControl::setDownloadSize()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::setDownloadSize( int size )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::setDownloadSize() [%d]"), size );
    mDownloadSize = size;
    return true;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackFullScreenControl::updateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::updateDownloadPosition( int size )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::updateDownloadPosition() [%d]"), size );
    mDownloadPosition = size;
    return true;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateState( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::updateState() [%d]"), state );
    mState = state;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateControlsWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::updateControlsWithFileDetails()") );
    Q_UNUSED( details );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::updateControlProperties()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateControlProperties( TUint properties )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::updateControlProperties()") );
    mProperties = properties;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::resetControl()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::resetControl()
{
    mControlReset = true;
}

// End of file
