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
* Description:  Implementation of VideoPlaybackFullScreenControl
*
*/

// Version : %version: da1mmcf#12 %



#include <w32std.h>

#include "mpxvideo_debug.h"
#include "videoplaybackcontrolbar.h"
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
    , mVisible( false )
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
    if( mVisible != visible )
    {
        mVisible = visible;

        switch ( mControlIndex )
        {
            case EControlBar:
            {
                static_cast<VideoPlaybackControlBar*>(mControl)->setVisibleToControlBar( visible );
                break;
            }
            default:
            {
                mControl->setVisible( visible );
                break;
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::isVisible()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::isVisible()
{
    return mControl->isVisible();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::setVisibility()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::setVisibility( TMPXPlaybackState state )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::setVisibility()"));

    bool visible = false;

    switch( state )
    {
        case EPbStatePlaying:
        {
            if ( mProperties & EShownWhenPlaying )
            {
                visible = true;
            }
            break;
        }
        case EPbStatePaused:
        {
            if ( mProperties & EShownWhenPaused )
            {
                visible = true;
            }
            break;
        }
        case EPbStatePluginSeeking:
        {
            if ( mProperties & EShownWhenSeeking )
            {
                visible = true;
            }
            break;
        }
        case EPbStateBuffering:
        {
            if ( mProperties & EShownWhenBuffering )
            {
                visible = true;
            }
            break;
        }
        case EPbStateNotInitialised:
        case EPbStateInitialising:
        {
            if ( mProperties & EShownWhenInitializing )
            {
                visible = true;
            }
            break;
        }
    }

    setVisible( visible );
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
// VideoPlaybackFullScreenControl::durationChanged()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::durationChanged( int duration )
{
    bool changed = EFalse;

    if ( mControlIndex == EControlBar )
    {
        MPX_DEBUG(_L("VideoPlaybackFullScreenControl::DurationChanged() [%d]"), duration);

        static_cast<VideoPlaybackControlBar*>(mControl)->durationChanged( duration );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::positionChanged()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::positionChanged( int position )
{
    bool changed = EFalse;

    if ( mControlIndex == EControlBar )
    {
        MPX_DEBUG(_L("VideoPlaybackFullScreenControl::positionChanged() [%d]"), position);

        static_cast<VideoPlaybackControlBar*>(mControl)->positionChanged( position );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::aspectRatioChanged()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::aspectRatioChanged( int aspectRatio )
{
    bool changed = EFalse;

    if ( mControlIndex == EControlBar )
    {
        MPX_DEBUG(
            _L("VideoPlaybackFullScreenControl::aspectRatioChanged() [%d]"), aspectRatio);

        static_cast<VideoPlaybackControlBar*>(mControl)->aspectRatioChanged( aspectRatio );

        changed = ETrue;
    }

    return changed;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackFullScreenControl::setDownloadSize()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::setDownloadSize( int /*size*/ )
{
    bool changed = EFalse;

    /*
    if ( mControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("VideoPlaybackFullScreenControl::SetDownloadSize() [%d]"), size);

        //static_cast<VideoPlaybackProgressBar*>(mControl)->SetDownloadSize( size );

        changed = ETrue;
    }
*/
    return changed;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackFullScreenControl::updateDownloadPosition()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackFullScreenControl::updateDownloadPosition( int /*size*/ )
{
    bool changed = EFalse;

    /*
    if ( mControlIndex == EMPXProgressBar )
    {
        MPX_DEBUG(_L("VideoPlaybackFullScreenControl::UpdateDownloadPosition() [%d]"), size);

        //static_cast<VideoPlaybackProgressBar*>(mControl)->updateDownloadPosition( size );

        changed = ETrue;
    }*/

    return changed;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateState( TMPXPlaybackState state )
{
    switch ( mControlIndex )
    {
        case EControlBar:
        {
            static_cast<VideoPlaybackControlBar*>(mControl)->updateState( state );
            break;
        }
        case EDetailsViewPlaybackWindow:
        {
            static_cast<VideoPlaybackDetailsPlaybackWindow*>(mControl)->updateState( state );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateControlsWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    switch ( mControlIndex )
    {
        case EControlBar:
        {
            static_cast<VideoPlaybackControlBar*>(mControl)->updateWithFileDetails( details );
            break;
        }
        case EFileDetailsWidget:
        {
            static_cast<VideoPlaybackFileDetailsWidget*>(mControl)->updateWithFileDetails( details );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::updateControlProperties()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateControlProperties( TUint properties )
{
    mProperties = properties;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::resetControl()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::resetControl()
{
    if ( mControlIndex == EControlBar )
    {
        static_cast<VideoPlaybackControlBar*>(mControl)->resetControl();
    }
}

// End of file
