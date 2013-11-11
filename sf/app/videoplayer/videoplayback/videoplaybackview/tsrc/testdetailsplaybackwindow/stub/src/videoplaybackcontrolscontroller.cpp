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

// Version : %version: 6 %



// INCLUDE FILES


#include "videoplaybackdocumentloader.h"
#include "videoplaybackcontrolscontroller.h"

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::VideoPlaybackControlsController()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlsController::VideoPlaybackControlsController( bool attachOperation )
    : mViewMode( EFullScreenView )
    , mIsAttachOperation( attachOperation )
    , mAttachVideoDone( false )
    , mSendVideoDone( false )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::VideoPlaybackControlsController()"));

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
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::handleCommand()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::handleCommand( 
        TMPXVideoPlaybackViewCommandIds command, int value )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::handleCommand(%d)"), command);

    mCommand = command;
    mCommandValue = value;
}

// -------------------------------------------------------------------------------------------------
//   VideoPlaybackControlsController::changeViewMode
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::changeViewMode( 
        TPlaybackViewMode viewMode, bool transitionEffect )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::changeViewMode()"));

    Q_UNUSED( transitionEffect );
    mViewMode = viewMode;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::resetDisappearingTimers()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::resetDisappearingTimers( TTimerAction timerAction )
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::resetDisappearingTimers()"));

    mTimerAction = timerAction;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::isAttachOperation()
// -------------------------------------------------------------------------------------------------
//
bool VideoPlaybackControlsController::isAttachOperation()
{
    MPX_DEBUG(_L("VideoPlaybackControlsController::isAttachOperation() ret %d"), 
            mIsAttachOperation);

    return mIsAttachOperation;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::attachVideo()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::attachVideo()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::attachVideo()"));
    
    mAttachVideoDone = true;
}    

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlsController::sendVideo()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlsController::sendVideo()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackControlsController::sendVideo()"));
    
    handleCommand( EMPXPbvCmdPause );    
    mSendVideoDone = true;
}    

// End of File
