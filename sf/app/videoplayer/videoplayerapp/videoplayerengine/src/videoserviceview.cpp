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
* Description:  Implementation of VideoServiceView
*
*/

// Version : %version:  5 %

#include "videoservices.h"
#include "videoserviceview.h"
#include "videoplayerengine.h"
#include "mpxvideo_debug.h"

// -------------------------------------------------------------------------------------------------
// VideoServiceView()
// -------------------------------------------------------------------------------------------------
// 
VideoServiceView::VideoServiceView( VideoServices* parent, 
                                    VideoPlayerEngine* engine, 
                                    QLatin1String service )
    : XQServiceProvider( service, parent )
    , mEngine( engine )
    , mServiceApp( parent )
{
    MPX_ENTER_EXIT(_L("VideoServiceView::VideoServiceView()"));    
    publishAll();
}

// -------------------------------------------------------------------------------------------------
// ~VideoServiceView()
// -------------------------------------------------------------------------------------------------
// 
VideoServiceView::~VideoServiceView()
{
    MPX_ENTER_EXIT(_L("VideoServiceView::~VideoServiceView()"));
}

// -------------------------------------------------------------------------------------------------
// setEngine()
// -------------------------------------------------------------------------------------------------
// 
void VideoServiceView::setEngine( VideoPlayerEngine* engine )
{
    MPX_ENTER_EXIT(_L("VideoServiceView::setEngine()"));
    mEngine = engine;
}

// -------------------------------------------------------------------------------------------------
// view( QString )
// -------------------------------------------------------------------------------------------------
// 
bool VideoServiceView::view( QString file )
{
    MPX_ENTER_EXIT(_L("VideoServiceView::view( QString ) "),
                   _L("file = %s"), file.data() );
    
    if ( mEngine )
    {
    	mServiceApp->setCurrentService( VideoServices::EView );
    	mEngine->playMedia( file );
    }
    
    return true;
}

// -------------------------------------------------------------------------------------------------
// view( XQSharableFile )
// -------------------------------------------------------------------------------------------------
// 
bool VideoServiceView::view( XQSharableFile file  )
{
    MPX_ENTER_EXIT(_L("VideoServiceView::view( XQSharableFile )"));
    
    RFile rFile;
    bool handleOK = file.getHandle( rFile );

    mServiceApp->setCurrentService( VideoServices::EView );
    
    if ( mEngine)
    {
        if ( handleOK )
        {            
            mEngine->playMedia( rFile );        
        }
        else
        {
            mEngine->playMedia( file.fileName() );    
        }
    }
       
    return true;
}




