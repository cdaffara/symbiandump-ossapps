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

// Version : %version:  2 %



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
// VideoPlaybackControlBar::initialize()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlBar::initialize()
{
}

//End of file
