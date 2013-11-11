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
* Description:  Implementation of VideoPlaybackFileDetailsWidget
*
*/

// Version : %version:   2 %




#include "mpxvideo_debug.h"
#include "videoplaybackfiledetailswidget.h"
#include "videoplaybackcontrolscontroller.h"


// -------------------------------------------------------------------------------------------------
// VideoPlaybackFileDetailsWidget::VideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackFileDetailsWidget::VideoPlaybackFileDetailsWidget( 
        VideoPlaybackControlsController* controller )
    : mController( controller )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackFileDetailsWidget::VideoPlaybackFileDetailsWidget()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFileDetailsWidget::~VideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackFileDetailsWidget::~VideoPlaybackFileDetailsWidget()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackFileDetailsWidget::~VideoPlaybackFileDetailsWidget()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFileDetailsWidget::initialize
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFileDetailsWidget::initialize()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackFileDetailsWidget::initialize"));
}

//End of file
