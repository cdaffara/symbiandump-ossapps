/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   VideoOperatorService class implementation.
*
*/

// Version : 

// INCLUDE FILES

#include "videooperatorservice.h"
#include "videooperatorservicedata.h"
#include "videocollectionviewutils.h"
#include "videocollectioncenrepdefs.h"
#define private public
#include "videooperatorservice_p.h"
#undef private

QList<QString> VideoOperatorServiceData::mTitles;
QList<QString> VideoOperatorServiceData::mIcons;
QList<QString> VideoOperatorServiceData::mUris;
QList<int> VideoOperatorServiceData::mUids;
        
int VideoOperatorServiceData::mLoadCallCount = 0;
int VideoOperatorServiceData::mTitleCallCount = 0;
int VideoOperatorServiceData::mIconResourceCallCount = 0;
int VideoOperatorServiceData::mLaunchServiceCallCount = 0;

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoOperatorService::VideoOperatorService(QObject *parent) : 
    QObject(parent), d_ptr(new VideoOperatorServicePrivate())
{
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoOperatorService::~VideoOperatorService()
{
    delete d_ptr;
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------
//
bool VideoOperatorService::load(int titleKey, int iconKey, int serviceUriKey, int appUidKey)
{
    Q_UNUSED(titleKey);
    Q_UNUSED(iconKey);
    Q_UNUSED(serviceUriKey);
    Q_UNUSED(appUidKey);
    if(d_ptr)
    {
        if(!VideoOperatorServiceData::mTitles.isEmpty())
        {
            d_ptr->mTitle = VideoOperatorServiceData::mTitles.takeFirst();
        }
         
        if(!VideoOperatorServiceData::mUris.isEmpty())
        {
            d_ptr->mServiceUri = VideoOperatorServiceData::mUris.takeFirst();
        }
        
        if(!VideoOperatorServiceData::mIcons.isEmpty())
        {
            d_ptr->mIconResource = VideoOperatorServiceData::mIcons.takeFirst();
        }        
    }
    VideoOperatorServiceData::mLoadCallCount++;

    // Icon is required, either service uri or application uid is required.
    if(d_ptr->mIconResource.isEmpty() || (d_ptr->mServiceUri.isEmpty() && d_ptr->mApplicationUid <= 0))
    {
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// title
// ---------------------------------------------------------------------------
//
const QString VideoOperatorService::title() const
{
    VideoOperatorServiceData::mTitleCallCount++;
    return d_ptr->mTitle;
}

// ---------------------------------------------------------------------------
// iconResource
// ---------------------------------------------------------------------------
//
const QString VideoOperatorService::iconResource() const
{
    VideoOperatorServiceData::mIconResourceCallCount++;
    return d_ptr->mIconResource;
}

// ---------------------------------------------------------------------------
// launchService
// ---------------------------------------------------------------------------
//
void VideoOperatorService::launchService()
{
    VideoOperatorServiceData::mLaunchServiceCallCount++;
}

// End of file.
