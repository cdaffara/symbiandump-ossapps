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
* Description:   Implementation of VideoServices STUB
*
*/

// Version : %version: 1 %

#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "mpxvideo_debug.h"


VideoServices *VideoServices::mInstance = 0;
int  VideoServices::mReferenceCount = 0;

// -----------------------------------------------------------------------------
// VideoServices::instance()
// -----------------------------------------------------------------------------
//
VideoServices* VideoServices::instance()
{
    MPX_DEBUG(_L("VideoServices::instance()"));

    if ( !mInstance )
    {
        mInstance = new VideoServices();
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
    MPX_DEBUG(_L("VideoServices::decreaseReferenceCount()"));

    if ( mInstance )
    {
        if ( --mInstance->mReferenceCount == 0 )
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
VideoServices::VideoServices()
    : mCurrentService( VideoServices::ENoService )
    , mItemSelected( false )
{
    MPX_DEBUG(_L("VideoServices::VideoServices()"));

    mServiceUriFetch = new VideoServiceUriFetch( this );
}

// ----------------------------------------------------------------------------
// ~VideoServices()
// ----------------------------------------------------------------------------
//
VideoServices::~VideoServices()
{
    MPX_DEBUG(_L("VideoServices::~VideoServices()"));

    delete mServiceUriFetch;
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

// ----------------------------------------------------------------------------
// itemSelected()
// ----------------------------------------------------------------------------
//
void VideoServices::itemSelected( const QString& item )
{
    MPX_DEBUG(_L("VideoServices::itemSelected()") );
    
    Q_UNUSED( item );

    mItemSelected = true;
}
