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
* Description:  Implementation of VideoServiceUri
*
*/

// Version : %version:  1 %

#include "videoservices.h"
#include "videoserviceuri.h"
#include "videoplayerengine.h"
#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
// VideoServiceUri()
// -------------------------------------------------------------------------------------------------
//
VideoServiceUri::VideoServiceUri( VideoServices* parent,
                                    VideoPlayerEngine* engine,
                                    QLatin1String service )
    : XQServiceProvider( service, parent )
    , mEngine( engine )
    , mServiceApp( parent )
{
    MPX_ENTER_EXIT(_L("VideoServiceUri::VideoServiceUri()"));
    publishAll();
}

// -------------------------------------------------------------------------------------------------
// ~VideoServiceUri()
// -------------------------------------------------------------------------------------------------
//
VideoServiceUri::~VideoServiceUri()
{
    MPX_ENTER_EXIT(_L("VideoServiceUri::~VideoServiceUri()"));
}

// -------------------------------------------------------------------------------------------------
// setEngine()
// -------------------------------------------------------------------------------------------------
//
void VideoServiceUri::setEngine( VideoPlayerEngine* engine )
{
    MPX_ENTER_EXIT(_L("VideoServiceUri::setEngine()"));
    mEngine = engine;
}

// -------------------------------------------------------------------------------------------------
// view( QString )
// -------------------------------------------------------------------------------------------------
//
bool VideoServiceUri::view( QString uri )
{
    MPX_ENTER_EXIT(_L("VideoServiceUri::view( QString ) "),
                   _L("uri = %s"), uri.data() );

    if(mEngine)
    {
       mServiceApp->setCurrentService(VideoServices::EUriView);
       mEngine->playURI( uri );
    }

    return true;
}
