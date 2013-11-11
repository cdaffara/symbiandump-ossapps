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
* Description:   stub CVideoCollectionWrapper class for unitesting CVideoListData 
* 
*/

// INCLUDE FILES

#include "videocollectionwrapper.h"

int VideoCollectionWrapper::mLatestStatusCode = -1;

QVariant VideoCollectionWrapper::mLatestAdditional = QVariant();

int VideoCollectionWrapper::mReferenceCount = 0;
VideoCollectionWrapper *VideoCollectionWrapper::mInstance = 0;

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper *VideoCollectionWrapper::instance()
{
    if(!mInstance)
    {
        mInstance = new VideoCollectionWrapper();
    }
    mInstance->mReferenceCount++;
    return mInstance;
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::cleanup()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::decreaseReferenceCount()
{
    if(mInstance)
    {
        if(--mInstance->mReferenceCount == 0)
        {
            delete mInstance;
            mInstance = NULL;
        }
    }
}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::VideoCollectionWrapper()
{

}

// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::~CVideoCollectionWrapper()
// -----------------------------------------------------------------------------
//
VideoCollectionWrapper::~VideoCollectionWrapper()
{

}


// -----------------------------------------------------------------------------
// CVideoCollectionWrapper::sendError()
// -----------------------------------------------------------------------------
//
void VideoCollectionWrapper::sendAsyncStatus(int statusCode, QVariant &additional)
{
    mLatestStatusCode = statusCode;
    mLatestAdditional = additional;
}

// End of file
