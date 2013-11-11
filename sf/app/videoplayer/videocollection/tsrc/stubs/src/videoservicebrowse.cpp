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
* Description:  Implementation of VideoServiceBrowse Stub
*
*/

// Version : %version: 6 %

#include <hbapplication.h>

#include "xqserviceproviderstub.h"
#include "videoservices.h"
#include "videoservicebrowse.h"
#include "mpxhbvideocommondefs.h"
#include "videoservicebrowsedata.h"

int VideoServiceBrowseData::mBrowseCategory = 0;

/*!
    Constructor.
    \param parent, Pointer to the video services.
*/
VideoServiceBrowse::VideoServiceBrowse(VideoServices* parent, QLatin1String service)
    : XQServiceProvider( service, parent  )
    , mRequestIndex( 0 )
    , mServiceApp( parent )
{
    // not stubbed
}

/*!
    Destructor.
*/
VideoServiceBrowse::~VideoServiceBrowse()
{
    // not stubbed
}

/*!
    Complete pending service request.
    \param None.
    \return None.
*/
void VideoServiceBrowse::complete()
{
    // not stubbed
}

/*!
    Get current browsing category.
    \param None.
    \return VideoServices::TVideoCategory.
*/
int VideoServiceBrowse::getBrowseCategory() const
{
    return VideoServiceBrowseData::mBrowseCategory;
}

/*!
    Return context title of videos application.
    \param None.
    \return QString.
*/
QString VideoServiceBrowse::contextTitle() const
{
    // not stubbed
    return QString();
}

/*!
    Browse videos.
    \param title, Title of the embedded Videos application.
    \param category, Category which type of videos are browsed.
    \return None.
*/
void VideoServiceBrowse::browseVideos(const QString &title,
    int category,
    int sortRole)
{
    Q_UNUSED(title);
    Q_UNUSED(category);
    Q_UNUSED(sortRole);
    
    // not stubbed
}

/*!
    Browse videos.
    \param category, Category which type of videos are browsed.
    \return None.
*/
void VideoServiceBrowse::browseVideos(int category,
    int sortRole)
{
    Q_UNUSED(category);
    Q_UNUSED(sortRole);
    
    // not stubbed
}

// End of file
