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
* Description:  stub impl for VideoServiceView
*
*/

// Version : %version:  3 %


#include "videoservices.h"
#include "videoserviceview.h"
#include "videoplayerengine.h"
#include "mpxvideo_debug.h"


VideoServiceView::VideoServiceView( VideoServices* parent, VideoPlayerEngine* engine )
{
    MPX_DEBUG(_L("VideoServiceView::VideoServiceView()"));
    
    Q_UNUSED( engine );    
    mServiceApp = parent;
}

VideoServiceView::~VideoServiceView()
{
    MPX_DEBUG(_L("VideoServiceView::~VideoServiceView()"));
}

void VideoServiceView::view( QString filePath )
{
    MPX_DEBUG(_L("VideoServiceView::view()"));
    
    Q_UNUSED( filePath );    
    mServiceApp->setCurrentService( VideoServices::EView );
}
