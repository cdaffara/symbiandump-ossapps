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

// Version : %version: da1mmcf#5 %


#include <qapplication>

#include <hbinstance.h>
#include <xqpluginloader.h>
#include <xqplugininfo.h>
#include <xqserviceutil.h>

#include "videoplayerengine.h"
#include "videoplaybackwrapper.h"
#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
// VideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
VideoPlayerEngine::VideoPlayerEngine( bool isService )
    : mIsService( isService )
    , mEmbedded( false )
{
    MPX_DEBUG(_L("VideoPlayerEngine::VideoPlayerEngine()"));
}

// -------------------------------------------------------------------------------------------------
// ~VideoPlayerEngine()
// -------------------------------------------------------------------------------------------------
//
VideoPlayerEngine::~VideoPlayerEngine()
{
    MPX_DEBUG(_L("VideoPlayerEngine::~VideoPlayerEngine()"));
}

// -------------------------------------------------------------------------------------------------
// initialize()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::initialize()
{
    MPX_DEBUG(_L("VideoPlayerEngine::initialize()"));
}

// -------------------------------------------------------------------------------------------------
// handleCommand()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::handleCommand( int commandCode )
{
    MPX_DEBUG(_L("VideoPlayerEngine::initialize()"));
    Q_UNUSED( commandCode );
}


// -------------------------------------------------------------------------------------------------
// handleQuit()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::handleQuit()
{
    MPX_DEBUG(_L("VideoPlayerEngine::handleQuit()"));
    delete this;
}


// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::playMedia( QString filePath )
{
    MPX_DEBUG(_L("VideoPlayerEngine::playMedia()"));
    Q_UNUSED( filePath );
}

// -------------------------------------------------------------------------------------------------
// setEmbedded()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::setEmbedded()
{
    MPX_DEBUG(_L("VideoPlayerEngine::setEmbedded()"));
    mEmbedded = true;
}

// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
//
void VideoPlayerEngine::playMedia( RFile file )
{    
    MPX_DEBUG(_L("VideoPlayerEngine::playMedia( file )"));
    Q_UNUSED( file );
}

void VideoPlayerEngine::playURI( QString uri )
{
    MPX_DEBUG(_L("VideoPlayerEngine::playURI( uri )"));
    Q_UNUSED( uri );    
}

// End of file
