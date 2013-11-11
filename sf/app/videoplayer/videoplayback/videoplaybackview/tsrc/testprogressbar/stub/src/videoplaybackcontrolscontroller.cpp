/*
* Copyright (c) 20010 Nokia Corporation and/or its subsidiary(-ies).
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

// Version : %version:4 %



// INCLUDE FILES


#include "videoplaybackdocumentloader.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolscontroller.h"
#include "videoplaybackcontrolconfiguration.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::VideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::VideoPlaybackControlsController()   
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::VideoPlaybackControlsController") );

    initializeController();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::initializeController()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::initializeController()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::initializeController") );

    mLoader = new VideoPlaybackDocumentLoader();		
    mFileDetails = new VideoPlaybackViewFileDetails();
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

    if ( mFileDetails )
    {
        delete mFileDetails;
        mFileDetails = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::resetDisappearingTimers( TTimerAction timerAction )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::resetDisappearingTimers") );

    mTimerAction = timerAction;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::handleCommand()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleCommand( 
        TMPXVideoPlaybackViewCommandIds command, int value )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::handleCommand") );

    mCommand = command;
    mValue = value;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::layoutLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader* VideoPlaybackControlsController::layoutLoader()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::layoutLoader") );

    return mLoader;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::viewMode
// -------------------------------------------------------------------------------------------------
//
TPlaybackViewMode VideoPlaybackControlsController::viewMode()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::viewMode") );

    return mViewMode;
}

// End of File
