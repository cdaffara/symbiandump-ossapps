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
* Description:  Implementation of Video playback view
*
*/

// Version : %version: 4 %



//  Include Files
#include <aknappui.h>
#include <qgraphicssceneevent>
#include <hbinstance.h>
#include <mmf/common/mmferrors.h>

#include "../inc/videobaseplaybackview.h"
#include "mpxvideo_debug.h"


//  Member Functions

// -------------------------------------------------------------------------------------------------
// VideoBasePlaybackView::VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::VideoBasePlaybackView()
    : mViewClosed( false )
{
    MPX_DEBUG(_L("VideoBasePlaybackView::VideoBasePlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::~VideoBasePlaybackView()
// -------------------------------------------------------------------------------------------------
//
VideoBasePlaybackView::~VideoBasePlaybackView()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::~VideoBasePlaybackView()"));
}

// -------------------------------------------------------------------------------------------------
//   VideoBasePlaybackView::closePlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoBasePlaybackView::closePlaybackView()
{
    MPX_DEBUG(_L("VideoBasePlaybackView::closePlaybackView()"));
    mViewClosed = true;    
}

// EOF
