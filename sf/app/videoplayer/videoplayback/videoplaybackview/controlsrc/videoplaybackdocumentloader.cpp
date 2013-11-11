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
* Description:  Implementation of VideoPlaybackDocumentLoader
*
*/

// Version : %version: 15 %



#include "mpxvideo_debug.h"
#include "videoplaybackcontrolbar.h"
#include "videoplaybackdocumentloader.h"
#include "videoplaybackfiledetailswidget.h"
#include "videoplaybackcontrolscontroller.h"
#include "videoplaybackcontrolconfiguration.h"
#include "videoplaybackdetailsplaybackwindow.h"

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader( 
        VideoPlaybackControlsController *controller )
{
    MPX_ENTER_EXIT(_L("VideoPlaybackDocumentLoader::VideoPlaybackDocumentLoader()"));

    mController = controller;
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::~VideoPlaybackDocumentLoader
// -------------------------------------------------------------------------------------------------
//
VideoPlaybackDocumentLoader::~VideoPlaybackDocumentLoader()
{
    MPX_DEBUG(_L("VideoPlaybackDocumentLoader::~VideoPlaybackDocumentLoader") );
}

// -------------------------------------------------------------------------------------------------
// VideoPlaybackDocumentLoader::createObject()
// -------------------------------------------------------------------------------------------------
//
QObject *VideoPlaybackDocumentLoader::createObject( const QString& type, const QString &name )
{
    MPX_ENTER_EXIT(
        _L("VideoPlaybackDocumentLoader::createObject()"),
        _L("type = %s, name = %s"), type.data(), name.data() );
    
    QObject *object = NULL;

    if ( name == "controlBarLayout" )
    {
        object = new VideoPlaybackControlBar( mController );
        object->setObjectName( name );
    }
    else if ( name == "fileDetailsLayout" )
    {
        object = new VideoPlaybackFileDetailsWidget( mController );
        object->setObjectName( name );
    }
    else if ( name == "detailsPlaybackWindow" )
    {
        object = new VideoPlaybackDetailsPlaybackWindow( mController );
        object->setObjectName( name );
    }
    else
    {
        object = HbDocumentLoader::createObject( type, name );
    }

    return object;
}

// End of file
