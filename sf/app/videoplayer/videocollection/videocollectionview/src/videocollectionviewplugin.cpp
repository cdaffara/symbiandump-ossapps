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
* Description:   VideoCollectionViewPlugin class implementation
*
*/

// Version : %version: 25 %

// INCLUDE FILES
#include <xqplugin.h>
#include <hbaction.h>
#include <hbapplication.h>
#include <hbinstance.h>
#include <mpxitemid.h>

#include "videoservices.h"
#include "vcxmyvideosdefs.h"
#include "videocollectionviewplugin.h"
#include "videolistview.h"
#include "videocollectionuiloader.h"
#include "videocollectionviewutils.h"
#include "mpxhbvideocommondefs.h"
#include "videocollectiontrace.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoCollectionViewPlugin::VideoCollectionViewPlugin()
    : mUiLoader( 0 )
    , mView( 0 )
    , mUiUtils( VideoCollectionViewUtils::instance() )
    , mActivated( false )
    , mVideoServices( 0 )
{
	FUNC_LOG;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoCollectionViewPlugin::~VideoCollectionViewPlugin()
{
	FUNC_LOG;
    destroyView();
}

// ---------------------------------------------------------------------------
// Create view
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::createView()
{
	FUNC_LOG;
	mActivated = false;
    if ( !mView ) {

        if(!mUiLoader)
        {
            mUiLoader = new VideoCollectionUiLoader();
        }

        mUiLoader->reset();
        
        mUiUtils.setIsService();
        
        mUiLoader->setIsService(mUiUtils.isService());

		bool ok(false);

		mUiLoader->load(DOCML_VIDEOCOLLECTIONVIEW_FILE, &ok);
	    
		if (!ok)
	    {
	    	return;
	    }

	    mView = mUiLoader->findObject<VideoListView>( DOCML_NAME_VIEW );

        if(!mView)
        {
            ERROR(-1, "VideoCollectionViewPlugin::createView() failed to create view.");
            return;
        }

        if(!connect( mView, SIGNAL(command(int)), this, SIGNAL(command(int)) ) ||
           !connect( this, SIGNAL(doDelayeds()), mView, SLOT(doDelayedsSlot()) ) ||
           !connect( mView, SIGNAL(viewReady()), this, SLOT(viewReadySlot()) ) ) {
            ERROR(-1, "VideoCollectionViewPlugin::createView() failed to connect signals.");
            delete mView;
            mView = 0;
            return;
        }
        mView->initializeView();
    }
}


// ---------------------------------------------------------------------------
// timerEvent
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::timerEvent(QTimerEvent *event)
{
	FUNC_LOG;
    if (event)
    {
        if (event->timerId() == mTimerId)
        {
        	killTimer(mTimerId);
        	mTimerId = 0;

        	emit command(MpxHbVideoCommon::DoDelayedLoad);
        	emit doDelayeds();
        }
    }
}

// ---------------------------------------------------------------------------
// Destroy view
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::destroyView()
{
	FUNC_LOG;
    deactivateView();
    disconnect();

    delete mView; 
    mView = 0;
    delete mUiLoader;
    mUiLoader = 0;
    
    if(mVideoServices)
    {
        mVideoServices->decreaseReferenceCount();
        mVideoServices = 0;
    }
}

// ---------------------------------------------------------------------------
// Activate view
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::activateView()
{
	FUNC_LOG;
    if (!mActivated)
    {
        HbMainWindow *wnd = mView->mainWindow();
        if(wnd)
        {
            TMPXItemId itemId = TMPXItemId::InvalidId();
            bool isService = mUiUtils.isService();
            if (isService)
            {
                if(!mVideoServices)
                {
                    mVideoServices = VideoServices::instance();
                }

                VideoServices::TVideoService serviceType =
                        mVideoServices->currentService();
                if (serviceType == VideoServices::EBrowse)
                {
                    // activate browsing service
                    itemId.iId1 = mVideoServices->getBrowseCategory();
                    itemId.iId2 = KVcxMvcMediaTypeCategory;
                }
            }
            int err = mView->activateView(itemId);
            if (err != 0)
            {
                return;
            }
            mActivated = true;
        }
    }
}

// ---------------------------------------------------------------------------
// Deactivate view
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::deactivateView()
{
	FUNC_LOG;
    if ( mActivated ) {
        mView->deactivateView();
        mActivated = false;
    }
}

// ---------------------------------------------------------------------------
// Get view
// ---------------------------------------------------------------------------
//
QGraphicsWidget* VideoCollectionViewPlugin::getView()
{
	FUNC_LOG;
    return mView;
}

// ---------------------------------------------------------------------------
// Slot: Orientation change
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::orientationChange( Qt::Orientation  orientation)
{
	FUNC_LOG;
    // view handles orientation individually
    Q_UNUSED(orientation);
}

// ---------------------------------------------------------------------------
// Slot: back
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::back()
{
	FUNC_LOG;
    if ( mActivated ) {
        mView->back();
    }
}

// ---------------------------------------------------------------------------
// Slot: viewReadySlot
// ---------------------------------------------------------------------------
//
void VideoCollectionViewPlugin::viewReadySlot()
{
    FUNC_LOG;
    mTimerId = startTimer(DELAYED_LOAD_TIMEOUT);
}

XQ_EXPORT_PLUGIN2( videocollectionview, VideoCollectionViewPlugin );

// end of file
