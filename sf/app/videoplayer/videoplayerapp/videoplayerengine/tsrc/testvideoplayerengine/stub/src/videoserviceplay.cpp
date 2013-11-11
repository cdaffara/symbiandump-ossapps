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

// Version : %version: da1mmcf#3 %


#include "videoservices.h"
#include "videoserviceplay.h"
#include "videoplayerengine.h"
#include "mpxvideo_debug.h"


VideoServicePlay::VideoServicePlay( VideoServices* parent, VideoPlayerEngine* engine )
{
    MPX_DEBUG(_L("VideoServicePlay::VideoServicePlay()")); 
    
    Q_UNUSED( engine );    
	mServiceApp = parent;
}

VideoServicePlay::~VideoServicePlay()
{
    MPX_DEBUG(_L("VideoServicePlay::~VideoServicePlay()")); 
}

void VideoServicePlay::playMedia( QString filePath )
{
    MPX_DEBUG(_L("VideoServicePlay::playMedia()")); 
    
    Q_UNUSED( filePath );    
    mServiceApp->setCurrentService( VideoServices::EPlayback );
}
