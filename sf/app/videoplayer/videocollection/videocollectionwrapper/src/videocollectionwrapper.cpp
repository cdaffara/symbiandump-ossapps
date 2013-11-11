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
* Description:   VideoCollectionWrapper class implementation
* 
*/

// Version : %version: 32 %

// INCLUDE FILES
#include <qabstractitemmodel.h>

#include "videocollectionwrapper.h"
#include "videocollectionwrapper_p.h"
#include "videoproxymodelgeneric.h"
#include "videocollectiontrace.h"

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper &VideoCollectionWrapper::instance()
{
	FUNC_LOG;
    static VideoCollectionWrapper _staticWrapper;

    return _staticWrapper;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::VideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::VideoCollectionWrapper() : 
    d( new VideoCollectionWrapperPrivate ),
    mReferenceCount(0)
{
	FUNC_LOG;
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::~CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::~VideoCollectionWrapper()
{
	FUNC_LOG;
    delete d;    
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::getGenericModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapper::getGenericModel()
{
    FUNC_LOG;
    if(d)
    {
        return d->getGenericModel();
    }
    return 0;
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::getAllVideosModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapper::getAllVideosModel()
{
	FUNC_LOG;
    if(d)
    {
        return d->getAllVideosModel();
    }
    return 0;
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::getCollectionsModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapper::getCollectionsModel()
{
    FUNC_LOG;
    if(d)
    {
        return d->getCollectionsModel();
    }
    return 0;
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::getCollectionContentModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapper::getCollectionContentModel()
{
    FUNC_LOG;
    if(d)
    {
        return d->getCollectionContentModel();
    }
    return 0;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::sendAsyncStatus()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::sendAsyncStatus(int statusCode,  QVariant &additional)
{
	FUNC_LOG;
    emit asyncStatus(statusCode, additional);
}

// End of file
