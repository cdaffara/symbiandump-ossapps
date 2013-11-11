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
* Description:  Implementation of VideoPlayerEngine
*
*/

// Version : %version: 1 %


#include <hbinstance.h>
#include "videoplayerapp.h"
#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
// VideoPlayerApp()
// -------------------------------------------------------------------------------------------------
//
VideoPlayerApp::VideoPlayerApp(int &argc, char *argv[], Hb::ApplicationFlags flags)
    : HbApplication(argc, argv, flags)
{
    MPX_DEBUG(_L("VideoPlayerApp::VideoPlayerApp()"));
}

// -------------------------------------------------------------------------------------------------
// ~VideoPlayerApp()
// -------------------------------------------------------------------------------------------------
//
VideoPlayerApp::~VideoPlayerApp()
{
    MPX_DEBUG(_L("VideoPlayerApp::~VideoPlayerApp()"));
}

// -------------------------------------------------------------------------------------------------
// viewReadySlot()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerApp::viewReadySlot()
{
    MPX_ENTER_EXIT(_L("VideoPlayerApp::viewReadySlot()"));
    
    // applicationReady signal should be emitted only once, so disconnect the viewReady signal.
    disconnect(hbInstance->allMainWindows().value(0), SIGNAL(viewReady()), 
               this, SLOT(viewReadySlot()));
    
    emit applicationReady();
}

// End of file
