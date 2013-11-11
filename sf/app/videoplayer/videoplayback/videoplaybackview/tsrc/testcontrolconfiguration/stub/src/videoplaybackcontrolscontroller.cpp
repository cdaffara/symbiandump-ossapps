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
* Description:  Implementation of MPXVideoPlaybackControlsController
*
*/

// Version : %version: 6 %



// INCLUDE FILES

#include "../inc/videoplaybackdocumentloader.h"
#include "../inc/videoplaybackviewfiledetails.h"
#include "../inc/videoplaybackcontrolscontroller.h"

#include "videoplaybackcontrolconfiguration.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::VideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::VideoPlaybackControlsController(
        VideoPlaybackViewFileDetails *details )
    : mFileDetails( details )
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
//   VideoPlaybackControlsController::layoutLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader* VideoPlaybackControlsController::layoutLoader()
{
    return mLoader;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::shouldShowRNLogo
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::shouldShowRNLogo()
{
     return mFileDetails->mRNFormat ;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::controlsListUpdated
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::controlsListUpdated()
{
}

// End of File
