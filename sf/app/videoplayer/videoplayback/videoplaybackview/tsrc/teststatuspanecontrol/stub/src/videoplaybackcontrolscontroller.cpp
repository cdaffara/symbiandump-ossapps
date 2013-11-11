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
* Description:  Implementation of VideoPlaybackControlsController
*
*/

// Version : %version:  6 %



// INCLUDE FILES
#include <coecntrl.h>
#include <bautils.h>
#include <barsread.h>
#include <f32file.h>

#include "../inc/videobaseplaybackview.h"
#include "../inc/videoplaybackviewfiledetails.h"
#include "../inc/videoplaybackcontrolscontroller.h"
#include "../inc/videoplaybackdocumentloader.h"



// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::VideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::VideoPlaybackControlsController(
        VideoBasePlaybackView *view,
        VideoPlaybackViewFileDetails *details )
    : mView( view )
    , mFileDetails( details )
{
    initializeController();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::initializeController()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::initializeController()
{
    mLoader = new VideoPlaybackDocumentLoader();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::~VideoPlaybackControlsController
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::~VideoPlaybackControlsController()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::~VideoPlaybackControlsController"));

    if ( mLoader )
    {
        delete mLoader;
        mLoader = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::resetDisappearingTimers( TTimerAction timerAction )
{
    mTimerAction = timerAction;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::evaluateAndChangeViewMode
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::evaluateAndChangeViewMode(
        TPlaybackViewMode viewMode, bool transitionEffect )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::evaluateAndChangeViewMode()"));

    Q_UNUSED( transitionEffect );
    mViewMode = viewMode;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::layoutLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader* VideoPlaybackControlsController::layoutLoader()
{
    return mLoader;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
TPlaybackViewMode VideoPlaybackControlsController::viewMode()
{
    return mViewMode;
}

// End of File
