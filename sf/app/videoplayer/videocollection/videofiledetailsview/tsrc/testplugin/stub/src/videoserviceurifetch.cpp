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
* Description:   Implementation of VideoServiceUriFetch Stub
*
*/

// Version : %version: 3 %

#include "videoservices.h"
#include "videoserviceurifetch.h"


// ----------------------------------------------------------------------------
// VideoServiceUriFetch()
// ----------------------------------------------------------------------------
//
VideoServiceUriFetch::VideoServiceUriFetch(VideoServices* parent)
{
	mServiceApp = parent;
}

// ----------------------------------------------------------------------------
// ~VideoServiceUriFetch()
// ----------------------------------------------------------------------------
//
VideoServiceUriFetch::~VideoServiceUriFetch()
{

}

// ----------------------------------------------------------------------------
// fetch()
// ----------------------------------------------------------------------------
//
void VideoServiceUriFetch::fetch(const QString& /*title*/)
{
    mServiceApp->setCurrentService(VideoServices::EUriFetcher);
}
