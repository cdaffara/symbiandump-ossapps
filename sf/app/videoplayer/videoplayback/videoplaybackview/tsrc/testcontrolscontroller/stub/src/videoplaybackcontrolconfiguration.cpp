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
* Description:  Implementation of VideoPlaybackControlConfiguration
*
*/

// Version : %version: 3 %



// INCLUDE FILES

#include <qgraphicswidget>

#include "mpxvideo_debug.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolconfiguration.h"
#include "videoplaybackcontrolscontroller.h"

const TInt KControlListUpdated = 12; // additional extension of TMPXPlaybackState

// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackControlConfiguration::CMPXVideoPlaybackControlConfiguration()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlConfiguration::VideoPlaybackControlConfiguration(
        VideoPlaybackControlsController* controller)
    : mControlsController( controller )
{
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::~VideoPlaybackControlConfiguration()
// Destructor.
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlConfiguration::~VideoPlaybackControlConfiguration()
{
    MPX_DEBUG(_L("VideoPlaybackControlConfiguration::~VideoPlaybackControlConfiguration"));

    mControlsList.clear();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::createControlList()
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlConfiguration::createControlList()
{
    MPX_DEBUG(_L("VideoPlaybackControlConfiguration::createControlList()"));

    addControlToList( EBufferingAnimation );
    addControlToList( EStatusPane );
    addControlToList( ERealLogoBitmap );

    emit controlListUpdated();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::controlList
// -------------------------------------------------------------------------------------------------
//
QList<TVideoPlaybackControls>& VideoPlaybackControlConfiguration::controlList()
{
    MPX_DEBUG(_L("VideoPlaybackControlConfiguration::controlList()"));

    return mControlsList;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::updateControlList
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlConfiguration::updateControlList(
    TVideoPlaybackControlCommandIds event )
{
    MPX_DEBUG(_L("VideoPlaybackControlConfiguration::updateControlList(%d)"), event);
    
    if ( event == EControlCmdFullScreenViewOpened ||
         event == EControlCmdDetailsViewOpened ||
         event == EControlCmdPluginInitialized )
    {
        mState = event;
    }
    else if ( event == EControlCmdTvOutConnected ||
              event == EControlCmdAudionOnlyViewOpened )
    {
        mState = EControlCmdTvOutConnected;
    }
    else
    {
        mState = KControlListUpdated;    
    }

    emit controlListUpdated();
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::deleteControlFromList
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlConfiguration::deleteControlFromList(
        TVideoPlaybackControls control )
{
    MPX_DEBUG(_L("VideoPlaybackControlConfiguration::deleteControlFromList(%d)"), control);

    mControlsList.removeAll( control );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::addControlToList
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlConfiguration::addControlToList(
        TVideoPlaybackControls control )
{
    MPX_DEBUG(_L("VideoPlaybackControlConfiguration::addControlToList(%d)"), control);

    if ( ! mControlsList.contains( control ) )
    {
        mControlsList.append( control );
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::updateControlsWithFileDetails
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlConfiguration::updateControlsWithFileDetails()
{
    MPX_DEBUG(_L("VideoPlaybackControlConfiguration::updateControlsWithFileDetails()"));

    mUpdateControlsWithFileDetails = true;
}

// End of File
