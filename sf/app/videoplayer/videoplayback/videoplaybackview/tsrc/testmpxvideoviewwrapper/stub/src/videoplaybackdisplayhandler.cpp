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
* Description:   Implementation of video playback display handler
*
*/

// Version : %version:  11 %

#include <sysutil.h>
#include <s32file.h>
#include <mpxcommand.h>
#include <mpxcommandgeneraldefs.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideoviewwrapper.h"
#include "videoplaybackdisplayhandler.h"
#include "mpxvideoregion.h"
#include "videoplaybackviewfiledetails.h"


CVideoPlaybackDisplayHandler::CVideoPlaybackDisplayHandler( CMPXVideoViewWrapper* aViewWrapper )
    : iViewWrapper( aViewWrapper )
{
}

CVideoPlaybackDisplayHandler::~CVideoPlaybackDisplayHandler()
{
}

CVideoPlaybackDisplayHandler*
CVideoPlaybackDisplayHandler::NewL( CMPXVideoViewWrapper* aViewWrapper )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::NewL()"));

    CVideoPlaybackDisplayHandler* self =
        new(ELeave) CVideoPlaybackDisplayHandler( aViewWrapper );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
//  CVideoPlaybackDisplayHandler::ConstructL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::CreateDisplayWindowL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::CreateDisplayWindowL(
                                          RWsSession& aWs,
                                          CWsScreenDevice& aScreenDevice,
                                          RWindow& aWin,
                                          TRect aDisplayRect,
                                          VideoPlaybackViewFileDetails* fileDetails )
{
    Q_UNUSED( aWs );
    Q_UNUSED( aScreenDevice );
    Q_UNUSED( aWin );
    Q_UNUSED( aDisplayRect );
    Q_UNUSED( fileDetails );
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::RemoveDisplayWindow()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::RemoveDisplayWindow()
{
    if ( iVideoDisplay )
    {
        delete iVideoDisplay;
        iVideoDisplay = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::HandleVideoDisplayMessageL( CMPXMessage* /*aMessage*/ )
{
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::SetAspectRatioL()
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackDisplayHandler::SetAspectRatioL( TMPXVideoPlaybackCommand aCmd )
{
    iCommand = aCmd;

    switch ( aCmd )
    {
        case EPbCmdNaturalAspectRatio:
        {
            iAspectRatio = EMMFNatural;
            break;
        }
        case EPbCmdZoomAspectRatio:
        {
            iAspectRatio = EMMFZoom;
            break;
        }
        case EPbCmdStretchAspectRatio:
        {
            iAspectRatio = EMMFStretch;
            break;
        }
    }

    return iAspectRatio;
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackDisplayHandler::UpdateVideoRectL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackDisplayHandler::UpdateVideoRectL( TRect aClipRect, TBool transitionEffect  )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackDisplayHandler::UpdateVideoRectL()"));

    Q_UNUSED( transitionEffect );

    iRect = aClipRect;
}

// End of File
