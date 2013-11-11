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
* Description: VideoCollectionWrapperPrivate class implementation
* 
*/

// Version : %version: 30 %

// INCLUDE FILES
#include <qapplication.h>
#include <qabstractitemmodel.h>
#include <vcxmyvideosdefs.h>

#include "videocollectionwrapper.h"
#include "videocollectionwrapper_p.h"
#include "videolistdatamodel.h"
#include "videoproxymodelgeneric.h"
#include "videoproxymodelallvideos.h"
#include "videoproxymodelcollections.h"
#include "videoproxymodelcontent.h"
#include "videocollectionclient.h"
#include "videocollectioncommon.h"
#include "videocollectiontrace.h"

// ================= MEMBER FUNCTIONS =======================
//

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::VideoCollectionWrapperPrivate()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapperPrivate::VideoCollectionWrapperPrivate() :
    mGenericModel( 0 ),
    mAllVideosModel( 0 ),
    mCollectionsModel( 0 ),
    mCollectionContentModel( 0 ),
    mSourceModel( 0 ),
    mAboutToClose( false )
{
	FUNC_LOG;
    // NOP 
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::~VideoCollectionWrapperPrivate()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapperPrivate::~VideoCollectionWrapperPrivate()
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::getGenericModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapperPrivate::getGenericModel()
{
    FUNC_LOG;
    if(!mGenericModel)
    {
        mGenericModel = initProxyModelModel<VideoProxyModelGeneric>();
    }
    return mGenericModel;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::getAllVideosModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapperPrivate::getAllVideosModel()
{
    FUNC_LOG;
    if(!mAllVideosModel)
    {
        mAllVideosModel = initProxyModelModel<VideoProxyModelAllVideos>();
    }
    return mAllVideosModel;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::getCollectionsModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapperPrivate::getCollectionsModel()
{
    FUNC_LOG;
    if(!mCollectionsModel)
    {
        mCollectionsModel = initProxyModelModel<VideoProxyModelCollections>();
    }
    return mCollectionsModel;    
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::getCollectionContentModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapperPrivate::getCollectionContentModel()
{
    FUNC_LOG;
    if(!mCollectionContentModel)
    {
        mCollectionContentModel = initProxyModelModel<VideoProxyModelContent>();
    }
    return mCollectionContentModel;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::initSourceModel()
// -----------------------------------------------------------------------------
//
bool VideoCollectionWrapperPrivate::initSourceModel()
{
    FUNC_LOG;
    
    if(!mSourceModel)
    {
        mSourceModel = new VideoListDataModel();
        if(mSourceModel->initialize() < 0 || 
            !connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(aboutToQuitSlot())) )
        {
            delete mSourceModel;
            mSourceModel = 0;
            ERROR(-1, "VideoCollectionWrapperPrivate::initSourceModel() sourceModel setup failed.");
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapperPrivate::aboutToQuitSlot()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapperPrivate::aboutToQuitSlot()
{    
	FUNC_LOG;
    if(!mSourceModel.isNull())
    {
        delete mSourceModel;
        delete mAllVideosModel;
        delete mCollectionsModel;
        delete mCollectionContentModel;
        delete mGenericModel;
        mAboutToClose = true;
    }
}

// End of file
