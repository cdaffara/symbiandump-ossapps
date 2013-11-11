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
* Description:  Implementation of VideoPlaybackControlConfiguration
*
*/

// Version : %version: da1mmcf#25 %



// INCLUDE FILES

#include <QGraphicsWidget>

#include "mpxvideo_debug.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackviewfiledetails.h"
#include "videoplaybackcontrolconfiguration.h"
#include "videoplaybackcontrolscontroller.h"


// ================= MEMBER FUNCTIONS ==============================================================

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::VideoPlaybackControlConfiguration()
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackControlConfiguration::VideoPlaybackControlConfiguration(
        VideoPlaybackControlsController* controller)
    : mControlsController( controller )
    , mSurfaceAttached( false )
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

    VideoPlaybackViewFileDetails* fileDetails = mControlsController->fileDetails();

    addControlToList( EStatusPane );

    if ( fileDetails->mPlaybackMode == EMPXVideoStreaming ||
         fileDetails->mPlaybackMode == EMPXVideoLiveStreaming )
    {
        //
        // streaming, add branding animation control to show while initializing
        //
        addControlToList( EBufferingAnimation );

        emit controlListUpdated();
    }
    else if ( mControlsController->shouldShowRNLogo() )
    {
        addControlToList( ERealLogoBitmap );

        emit controlListUpdated();
    }
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

    QGraphicsWidget *widget =
            mControlsController->layoutLoader()->findWidget( QString( "transparentWindow" ) );

    switch ( event )
    {
        case EControlCmdFullScreenViewOpened:
        {
            MPX_DEBUG(_L("VideoPlaybackControlConfiguration::updateControlList() full screen view"));

            if ( mSurfaceAttached )
            {
                widget->setVisible( true );
            }

            deleteControlFromList( EDetailsViewPlaybackWindow );
            deleteControlFromList( EFileDetailsWidget );
            deleteControlFromList( EIndicatorBitmap );

            emit controlListUpdated();

            break;
        }
        case EControlCmdDetailsViewOpened:
        {
            MPX_DEBUG(_L("VideoPlaybackControlConfiguration::updateControlList() details view"));

            widget->setVisible( false );

            deleteControlFromList( EIndicatorBitmap );

            addControlToList( EDetailsViewPlaybackWindow );
            addControlToList( EFileDetailsWidget );

            emit controlListUpdated();

            break;
        }
        case EControlCmdTvOutConnected:
        case EControlCmdAudionOnlyViewOpened:
        {
            MPX_DEBUG(_L("VideoPlaybackControlConfiguration::updateControlList() audio only view"));

            widget->setVisible( false );

            deleteControlFromList( EDetailsViewPlaybackWindow );

            addControlToList( EIndicatorBitmap );
            addControlToList( EFileDetailsWidget );

            emit controlListUpdated();

            break;
        }
        case EControlCmdRemoveRNLogo:
        {
            MPX_DEBUG(_L("VideoPlaybackControlConfiguration::updateControlList() RN Logo removed"));

            deleteControlFromList( ERealLogoBitmap );

            emit controlListUpdated();

            break;
        }
        case EControlCmdSurfaceAttached:
        {
            MPX_DEBUG(_L("VideoPlaybackControlConfiguration::updateControlList() surface added"));

            mSurfaceAttached = true;

            widget->setVisible( true );

            break;
        }
        case EControlCmdSurfaceDetached:
        {
            MPX_DEBUG(_L("VideoPlaybackControlConfiguration::updateControlList() surface removed"));

            mSurfaceAttached = false;

            widget->setVisible( false );

            break;
        }
        default:
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackControlConfiguration::deleteControlFromList
// -------------------------------------------------------------------------------------------------
//
void VideoPlaybackControlConfiguration::deleteControlFromList( TVideoPlaybackControls control )
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

    addControlToList( EControlBar );

    emit controlListUpdated();
}

// End of File
