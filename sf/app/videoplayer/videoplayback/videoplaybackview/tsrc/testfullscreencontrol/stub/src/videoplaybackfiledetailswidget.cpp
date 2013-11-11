/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of VideoPlaybackFileDetailsWidget
*
*/

// Version : %version:  3 %



#include <qfileinfo>
#include <qgraphicslinearlayout>
#include <qdatetime>
#include <qdir>

#include <hblistwidgetitem.h>
#include <hbratingslider.h>
#include <hblistwidget.h>

#include "mpxvideo_debug.h"
#include "videoplaybackfiledetailswidget.h"
#include "videoplaybackcontrolscontroller.h"

const float KILOBYTE = 1024 ;
const QString KDATETIMEFORMAT = "d/M/yyyy hh:mm:ss ap";

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFileDetailsWidget::VideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackFileDetailsWidget::VideoPlaybackFileDetailsWidget( 
        VideoPlaybackControlsController* controller )
    : mController( controller )
    , mListWidget( 0 )
    , mFileDetailsUpdated( false )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackFileDetailsWidget::VideoPlaybackFileDetailsWidget()"));
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFileDetailsWidget::~VideoPlaybackFileDetailsWidget
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackFileDetailsWidget::~VideoPlaybackFileDetailsWidget()
{
    MPX_ENTER_EXIT(_L("VideoPlaybackFileDetailsWidget::~VideoPlaybackFileDetailsWidget()"));
   
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackFileDetailsWidget::updateControlsWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackFileDetailsWidget::updateWithFileDetails(
        VideoPlaybackViewFileDetails* details )
{
    Q_UNUSED( details );
}

//End of file
