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
#include "videooperatorservice_p.h"
#include "videocollectiontrace.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
VideoOperatorService::VideoOperatorService(QObject *parent) : 
    QObject(parent), d_ptr( new VideoOperatorServicePrivate() )
{
    FUNC_LOG;
}


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
VideoOperatorService::~VideoOperatorService()
{
    FUNC_LOG;
    delete d_ptr;
}

// ---------------------------------------------------------------------------
// load
// ---------------------------------------------------------------------------
//
bool VideoOperatorService::load(int titleKey, int iconKey, int serviceUriKey, int appUidKey)
{
    FUNC_LOG;
    return d_ptr->load(titleKey, iconKey, serviceUriKey, appUidKey);
}

// ---------------------------------------------------------------------------
// title
// ---------------------------------------------------------------------------
//
const QString VideoOperatorService::title() const
{
    return d_ptr->title();
}

// ---------------------------------------------------------------------------
// iconResource
// ---------------------------------------------------------------------------
//
const QString VideoOperatorService::iconResource() const
{
    return d_ptr->iconResource();
}

// ---------------------------------------------------------------------------
// launchService
// ---------------------------------------------------------------------------
//
void VideoOperatorService::launchService()
{
    FUNC_LOG;
    d_ptr->launchService();
}

// End of file.
