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
* Description:  Implementation of VideoPlaybackControlsController
*
*/

// Version : %version: 10 %



// INCLUDE FILES

#include "mpxvideoviewwrapper.h"
#include "videobaseplaybackview.h"
#include "videoplaybackviewfiledetails.h"
#include "../inc/videoplaybackcontrolscontroller.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::VideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::VideoPlaybackControlsController(
        VideoBasePlaybackView *view,
        CMPXVideoViewWrapper *viewWrapper,
        VideoPlaybackViewFileDetails *details )
    : mView( view )
    , mViewWrapper( viewWrapper )
    , mFileDetails( details ) 
    , mFileDetailsAdded( false )
{
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::addFileDetails()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::addFileDetails(
    VideoPlaybackViewFileDetails* details )
{
    Q_UNUSED( details );

    mFileDetailsAdded = true;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::~VideoPlaybackControlsController
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::~VideoPlaybackControlsController()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::~VideoPlaybackControlsController"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::handleEvent
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleEvent(
    TVideoPlaybackControlCommandIds event, int value )
{
    mReceivedEvent = event;
    mValue = value;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::updateVideoRectDone
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::updateVideoRectDone()
{
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::isRNLogoBitmapVisible()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::isRNLogoBitmapInControlList()
{
    return mRNLogoVisible;
}

// End of File
