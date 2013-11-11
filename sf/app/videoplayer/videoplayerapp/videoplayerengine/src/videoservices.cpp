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
* Description:  Implementation of VideoServices
*
*/

// Version : %version: 11 %

#include "videoplayerengine.h"
#include "videoservices.h"
#include "videoserviceurifetch.h"
#include "videoserviceplay.h"
#include "videoserviceview.h"
#include "videoservicebrowse.h"
#include "videoserviceuri.h"
#include "mpxvideo_debug.h"
#include <xqaiwdecl.h>
#include <xqserviceutil.h>

VideoServices *VideoServices::mInstance = 0;

// -------------------------------------------------------------------------------------------------
// VideoServices::instance()
// -------------------------------------------------------------------------------------------------
//
VideoServices* VideoServices::instance( VideoPlayerEngine* engine )
{
    MPX_ENTER_EXIT(_L("VideoServices::instance()"));
    
    if ( ! mInstance )
    {
        mInstance = new VideoServices(engine);
    }
    else if ( engine && ! mInstance->engine() )
    {
    	mInstance->setEngine(engine);
    }

    mInstance->mReferenceCount++;
    return mInstance;
}

// -------------------------------------------------------------------------------------------------
// VideoServices::decreaseReferenceCount()
// -------------------------------------------------------------------------------------------------
//
void VideoServices::decreaseReferenceCount()
{
    MPX_ENTER_EXIT(_L("VideoServices::decreaseReferenceCount()"));
    
    if ( mInstance )
    {
        if ( --mInstance->mReferenceCount == 0 )
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// setEngine()
// -------------------------------------------------------------------------------------------------
//
void VideoServices::setEngine( VideoPlayerEngine* engine )
{
    MPX_ENTER_EXIT(_L("VideoServices::setEngine()"));
    
    if ( mServicePlay )
    {
    	mEngine = engine;
    	mServicePlay->setEngine(engine);
    }
    
    if ( mServiceView )
    {
    	mEngine = engine;
    	mServiceView->setEngine(engine);    	
    }
    
    if ( mServiceUriView )
    {
        mEngine = engine;
        mServiceUriView->setEngine(engine);  
    }
    		
}

// -------------------------------------------------------------------------------------------------
// engine()
// -------------------------------------------------------------------------------------------------
//
VideoPlayerEngine* VideoServices::engine()
{
    MPX_DEBUG(_L("VideoServices::engine"));
	
    return mEngine;
}

// -------------------------------------------------------------------------------------------------
// VideoServices()
// -------------------------------------------------------------------------------------------------
//
VideoServices::VideoServices( VideoPlayerEngine* engine ) 
    : mReferenceCount( 0 )
    , mEngine( engine )
    , mCurrentService( VideoServices::ENoService )
    , mFetchSelected( false )
{
    MPX_ENTER_EXIT(_L("VideoServices::VideoServices()"));
   
    // New service, new interface
    mServicePlay = new VideoServicePlay( this, engine, 
                                 QLatin1String("videoplayer.com.nokia.symbian.IVideoView") );  

    // New service, new interface
    mServiceView = new VideoServiceView( this, engine, 
                                 QLatin1String("videoplayer.com.nokia.symbian.IFileView") ); 
          
    //URI fetch service
    mServiceUriFetch = new VideoServiceUriFetch( this,
                                 QLatin1String("videoplayer.com.nokia.symbian.IVideoFetch") );

    //Browse service
    mServiceBrowse = new VideoServiceBrowse( this,
                                 QLatin1String("videoplayer.com.nokia.symbian.IVideoBrowse") );

    // New service, old interface
    mServicePlayDeprecatedNewService = new VideoServicePlay( this, engine, 
                                                      QLatin1String("videoplayer.IVideoView") ); 

    // Old service, old interface
    mServicePlayDeprecatedOldService = new VideoServicePlay( this, engine, 
                                                      QLatin1String("com.nokia.Videos.IVideoView") );    
    
    // New service, old interface
    mServiceViewDeprecatedNewService = new VideoServiceView( this, engine, 
                                                      QLatin1String("videoplayer.IFileView") ); 

    // Old service, old interface 
    mServiceViewDeprecatedOldService = new VideoServiceView( this, engine, 
                                                      QLatin1String("com.nokia.Videos.IFileView") );     
    // new service, new interface
    mServiceUriView = new VideoServiceUri(  this, engine, QLatin1String("videoplayer.com.nokia.symbian.IUriView"));
}

// -------------------------------------------------------------------------------------------------
// ~VideoServices()
// -------------------------------------------------------------------------------------------------
//
VideoServices::~VideoServices()
{
    MPX_ENTER_EXIT(_L("VideoServices::~VideoServices()"));
    
    delete mServiceUriFetch;
    delete mServicePlay;
    delete mServiceView;
    delete mServiceBrowse;
    delete mServicePlayDeprecatedNewService;
    delete mServicePlayDeprecatedOldService;
    delete mServiceViewDeprecatedNewService;
    delete mServiceViewDeprecatedOldService;
    delete mServiceUriView;
}

// -------------------------------------------------------------------------------------------------
// currentService()
// -------------------------------------------------------------------------------------------------
//
VideoServices::TVideoService VideoServices::currentService()
{
    MPX_DEBUG(_L("VideoServices::currentService() ret %d"), mCurrentService );
	
    return mCurrentService;
}

// -------------------------------------------------------------------------------------------------
// getBrowseCategory()
// -------------------------------------------------------------------------------------------------
//
int VideoServices::getBrowseCategory() const
{
    MPX_DEBUG(_L("VideoServices::getBrowseCategory()"));
	
    int category = 0;

    if ( mServiceBrowse && ( XQServiceUtil::interfaceName().contains("symbian") ) )
    {
        category = mServiceBrowse->getBrowseCategory();
    }
    return category;
}

// -------------------------------------------------------------------------------------------------
// setCurrentService()
// -------------------------------------------------------------------------------------------------
//
void VideoServices::setCurrentService(VideoServices::TVideoService service)
{
    MPX_DEBUG(_L("VideoServices::setCurrentService(%d)"), service );
	
    mCurrentService = service;	
}

// -------------------------------------------------------------------------------------------------
// contextTitle()
// -------------------------------------------------------------------------------------------------
//
QString VideoServices::contextTitle() const
{
    MPX_DEBUG(_L("VideoServices::contextTitle()") );
	
    QString title;

    if ( mCurrentService == VideoServices::EUriFetcher )
    {
    	if ( mServiceUriFetch && ( XQServiceUtil::interfaceName().contains("symbian") ) )
    	{
            title = mServiceUriFetch->contextTitle();
    	}
    }
    else if ( mCurrentService == VideoServices::EBrowse )
    {
    	if ( mServiceBrowse && ( XQServiceUtil::interfaceName().contains("symbian") ) )
        {
            title = mServiceBrowse->contextTitle();
        }
    }

    return title;
}

// -------------------------------------------------------------------------------------------------
// sortRole()
// -------------------------------------------------------------------------------------------------
//
int VideoServices::sortRole() const
{
    MPX_ENTER_EXIT(_L("VideoServices::sortType()"));
    
    int sortRole = 0;

    if ( mCurrentService == EBrowse )
    {
    	if ( mServiceBrowse && ( XQServiceUtil::interfaceName().contains("symbian") ) )
        {
            sortRole = mServiceBrowse->sortRole();
        }
    }

    return sortRole;
}

// -------------------------------------------------------------------------------------------------
// itemSelected()
// -------------------------------------------------------------------------------------------------
//
void VideoServices::itemSelected(const QString& item)
{
    MPX_ENTER_EXIT(_L("VideoServices::itemSelected()"),
                   _L("item = %s"), item.data() );
    
    QStringList list;
    list.append( item );

    if( mServiceUriFetch && ( XQServiceUtil::interfaceName().contains("symbian") ) )
    {
    	mServiceUriFetch->complete( list );
    }
    mFetchSelected = true;
}

// -------------------------------------------------------------------------------------------------
// browsingEnded()
// -------------------------------------------------------------------------------------------------
//
void VideoServices::browsingEnded()
{
    MPX_ENTER_EXIT(_L("VideoServices::browsingEnded()"));

    if ( mServiceBrowse && ( XQServiceUtil::interfaceName().contains("symbian") ) )
    {
        mServiceBrowse->complete();
    }
}

// End of file
