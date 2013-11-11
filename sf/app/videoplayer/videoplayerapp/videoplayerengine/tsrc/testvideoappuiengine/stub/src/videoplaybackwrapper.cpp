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
* Description:  Stub-implementation of VideoPlaybackWrapper
*
*/

// Version : %version:  2 %


#include "videoplaybackwrapper.h"
#include "mpxvideo_debug.h"

int mCount = 0;

// -------------------------------------------------------------------------------------------------
// VideoPlaybackWrapper::VideoPlaybackWrapper()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackWrapper::VideoPlaybackWrapper()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::VideoPlaybackWrapper()"));
    
    VideoPlaybackWrapper::Increment();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackWrapper::~VideoPlaybackWrapper()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackWrapper::~VideoPlaybackWrapper()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::~VideoPlaybackWrapper()"));
    
    VideoPlaybackWrapper::Decrement();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackWrapper::openPlaybackView()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::openPlaybackView()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::openPlaybackView()"));
    
    emit handlePlaybackView( MpxHbVideoCommon::ActivatePlaybackView );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackWrapper::Increment()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::Increment()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::Increment()"));
    
    ++mCount;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackWrapper::Decrement()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::Decrement()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::Decrement()"));
    
    if ( mCount > 0 )
    {   
        --mCount;
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackWrapper::GetInstanceCount()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::GetInstanceCount()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::GetInstanceCount()"));
    
    return mCount;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackWrapper::playMedia()
// -------------------------------------------------------------------------------------------------
//
int VideoPlaybackWrapper::playMedia( QString aFileName )
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::playMedia()"));
    
    Q_UNUSED( aFileName );
    return 0;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackWrapper::lateInit()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackWrapper::lateInit()
{
    MPX_DEBUG(_L("VideoPlaybackWrapper::lateInit()"));
}

// End of File
