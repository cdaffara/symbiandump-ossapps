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
* Description:  Implementation of VideoServicePlay
*
*/

// Version : %version: da1mmcf#6 %

#include "videoservices.h"
#include "videoserviceplay.h"
#include "videoplayerengine.h"
#include "mpxvideo_debug.h"


// -------------------------------------------------------------------------------------------------
// VideoServicePlay()
// -------------------------------------------------------------------------------------------------
// 
VideoServicePlay::VideoServicePlay( VideoServices* parent, 
                                    VideoPlayerEngine* engine, 
                                    QLatin1String service )
    : XQServiceProvider( service, parent )
    , mEngine( engine )
    , mServiceApp( parent )
{
    MPX_ENTER_EXIT(_L("VideoServicePlay::VideoServicePlay()"));
    publishAll();
}

// -------------------------------------------------------------------------------------------------
// ~VideoServicePlay()
// -------------------------------------------------------------------------------------------------
// 
VideoServicePlay::~VideoServicePlay()
{
    MPX_DEBUG(_L("VideoServicePlay::~VideoServicePlay()"));
}

// -------------------------------------------------------------------------------------------------
// setEngine()
// -------------------------------------------------------------------------------------------------
// 
void VideoServicePlay::setEngine( VideoPlayerEngine* engine )
{
    MPX_DEBUG(_L("VideoServicePlay::setEngine()"));
    mEngine = engine;
}

// -------------------------------------------------------------------------------------------------
// playMedia()
// -------------------------------------------------------------------------------------------------
// 
void VideoServicePlay::playMedia( QString filePath )
{
    MPX_ENTER_EXIT(_L("VideoServicePlay::playMedia()"),
                   _L("filePath = %s"), filePath.data() );  
                     
    if ( mEngine )
    {
    	mServiceApp->setCurrentService( VideoServices::EPlayback );
    	mEngine->playMedia( filePath );
    }
}

// -------------------------------------------------------------------------------------------------
// playPDLMedia()
// -------------------------------------------------------------------------------------------------
// 
void VideoServicePlay::playPDLMedia( QString filePath, int downloadID  )
{
    MPX_DEBUG(_L("VideoServicePlay::playPDLMedia()"));
    Q_UNUSED( filePath );
    Q_UNUSED( downloadID );
}

// -------------------------------------------------------------------------------------------------
// closePlayer()
// -------------------------------------------------------------------------------------------------
// 
void VideoServicePlay::closePlayer()
{
    MPX_DEBUG(_L("VideoServicePlay::closePlayer()"));
}

