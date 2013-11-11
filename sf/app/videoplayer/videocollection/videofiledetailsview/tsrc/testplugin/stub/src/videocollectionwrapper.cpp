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
* Description:   VideoCollectionWrapper dummy class implementation
* 
*/

// INCLUDE FILES
#include <qabstractitemmodel.h>
#include <QDebug>
#include "videoproxymodelgeneric.h"
#include "videocollectionwrapper.h"

int VideoCollectionWrapper::mReferenceCount = 0;
VideoCollectionWrapper *VideoCollectionWrapper::mInstance = 0;

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::VideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper &VideoCollectionWrapper::instance()
{
    static VideoCollectionWrapper _staticWrapper;

    return _staticWrapper;
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper::cleanup()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::decreaseReferenceCount()
{
    if(mInstance)
    {
        qDebug() << "VideoCollectionWrapper::instance()-- = " << (mInstance->mReferenceCount-1);
        if(--mInstance->mReferenceCount == 0)
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

// -----------------------------------------------------------------------------
// VideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::VideoCollectionWrapper() :
mModel(0)
{
}

// -----------------------------------------------------------------------------
// ~VideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::~VideoCollectionWrapper()
{
}
// -----------------------------------------------------------------------------
// getModel()
// -----------------------------------------------------------------------------
//
VideoProxyModelGeneric* VideoCollectionWrapper::getAllVideosModel()
{
    return mModel;
}

// -----------------------------------------------------------------------------
// setModel()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::setModel(VideoProxyModelGeneric* model)
{
    mModel = model;
}

// End of file
