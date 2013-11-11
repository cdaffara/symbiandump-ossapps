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
* Description:   Implementation of VideoServices Stub
*
*/

// Version : %version: da1mmcf#5 %

#include "videoplayerengine.h"
#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "videoserviceplay.h"
#include "videoserviceview.h"
#include "mpxvideo_debug.h"


VideoServices *VideoServices::mInstance = 0;
int  VideoServices::mReferenceCount = 0;

// -----------------------------------------------------------------------------
// VideoServices::instance()
// -----------------------------------------------------------------------------
//
VideoServices* VideoServices::instance( VideoPlayerEngine* engine )
{
    MPX_ENTER_EXIT(_L("VideoServices::instance()"));
    
    if ( ! mInstance )
    {
        mInstance = new VideoServices( engine );
    }
    mInstance->mReferenceCount++;
    return mInstance;
}

// -----------------------------------------------------------------------------
// VideoServices::decreaseReferenceCount()
// -----------------------------------------------------------------------------
//
void VideoServices::decreaseReferenceCount()
{
    MPX_ENTER_EXIT(_L("VideoServices::decreaseReferenceCount()"));
    
    if ( mInstance )
    {
        if( --mInstance->mReferenceCount == 0 )
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
}


// ----------------------------------------------------------------------------
// VideoServices()
// ----------------------------------------------------------------------------
//
VideoServices::VideoServices( VideoPlayerEngine* engine ) 
    : mCurrentService(VideoServices::ENoService)
    , mFetchSelected( false )
{
    MPX_ENTER_EXIT(_L("VideoServices::VideoServices()"));
    
    mServiceUriFetch = new VideoServiceUriFetch( this );
    mServicePlay     = new VideoServicePlay( this, engine );
    mServiceView     = new VideoServiceView( this, engine );
}

// ----------------------------------------------------------------------------
// ~VideoServices()
// ----------------------------------------------------------------------------
//
VideoServices::~VideoServices()
{
    MPX_ENTER_EXIT(_L("VideoServices::~VideoServices()"));
    
    delete mServiceUriFetch;
    delete mServicePlay;
    delete mServiceView;
}


// ----------------------------------------------------------------------------
// currentService()
// ----------------------------------------------------------------------------
//
VideoServices::TVideoService VideoServices::currentService()
{
    MPX_DEBUG(_L("VideoServices::currentService() ret %d"), mCurrentService);
    return mCurrentService;
}


// ----------------------------------------------------------------------------
// setCurrentService()
// ----------------------------------------------------------------------------
//
void VideoServices::setCurrentService( VideoServices::TVideoService service )
{
    MPX_DEBUG(_L("VideoServices::setCurrentService(%d)"), service);
    mCurrentService = service;
}

