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

// Version : %version:  4 %



#include <w32std.h>
#include <hbwidget.h>

#include "mpxvideo_debug.h"
#include "videoplaybackfullscreencontrol.h"
#include "videoplaybackcontrolscontroller.h"


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
    mControl->setVisible( visible );
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
void VideoPlaybackFullScreenControl::setVisibility( TMPXPlaybackState /*state*/ )
{
    MPX_DEBUG(_L("VideoPlaybackFullScreenControl::setVisibility()"));
   
    mVisible = true;
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
// VideoPlaybackFullScreenControl::updateState()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateState( TMPXPlaybackState state )
{
    Q_UNUSED( state );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::updateControlsWithFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateControlsWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    Q_UNUSED( details );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFullScreenControl::updateControlProperties()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFullScreenControl::updateControlProperties( TUint properties )
{
    mProperties = properties;
}

// End of file
