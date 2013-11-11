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
* Description:  Implementation of playback view's input handler
*
*/

// Version : %version: 8 %


// INCLUDE FILES
#include <qobject>
#include <e32std.h>
#include <w32std.h> // RWindowBase
#include <e32base.h>
#include <eikclbd.h>
#include <aknconsts.h>
#include <mpxplaybackframeworkdefs.h>
#include <hwrmlightdomaincrkeys.h>
#include <mpxvideoplaybackdefs.h>

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "../inc/videoplaybackuserinputhandler.h"



// ======== MEMBER FUNCTIONS =======================================================================

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::CVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler::CVideoPlaybackUserInputHandler( CMPXVideoViewWrapper* aWrapper )
   : iViewWrapper( aWrapper )
{
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::NewL()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler* CVideoPlaybackUserInputHandler::NewL(
        CMPXVideoViewWrapper* aWrapper)
{
    MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::NewL()"));

    CVideoPlaybackUserInputHandler* self =
        new (ELeave) CVideoPlaybackUserInputHandler( aWrapper );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::ConstructL()
{
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::~CVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler::~CVideoPlaybackUserInputHandler()
{

}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::ProcessKeyEvent()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                                       TEventCode aType )
{
    Q_UNUSED( aKeyEvent );
    Q_UNUSED( aType );
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::SetForeground()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::SetForeground(TBool aForeground)
{
    Q_UNUSED( aForeground );
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::HandleTVOutEvent()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleTVOutEventL( TBool aTVOutConnected )
{
    Q_UNUSED( aTVOutConnected );
}

// EOF
