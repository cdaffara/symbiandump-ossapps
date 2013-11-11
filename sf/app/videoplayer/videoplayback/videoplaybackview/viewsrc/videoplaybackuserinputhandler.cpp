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

// Version : %version: 11 %


// INCLUDE FILES
#include <e32std.h>
#include <w32std.h> // RWindowBase
#include <e32base.h>
#include <eikclbd.h>
#include <aknconsts.h>
#include <remconcoreapi.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h> // Side volume key
#include <mpxplaybackframeworkdefs.h>
#include <centralrepository.h>  // for peripheral display timeout setting
#include <settingsinternalcrkeys.h> // display timeout setting keys
#include <hwrmlightdomaincrkeys.h>
#include <mpxvideoplaybackdefs.h>
#include <avkondomainpskeys.h>

#include "mpxvideo_debug.h"
#include "mpxvideoviewwrapper.h"
#include "mpxcommonvideoplaybackview.hrh"
#include "videoplaybackuserinputhandler.h"



// ======== MEMBER FUNCTIONS =======================================================================

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::CVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler::CVideoPlaybackUserInputHandler( CMPXVideoViewWrapper* aWrapper )
    : iProcessingInputType( EVideoNone )
    , iForeground( ETrue )
    , iViewWrapper( aWrapper )
{
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::NewL()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler* CVideoPlaybackUserInputHandler::NewL( CMPXVideoViewWrapper* aWrapper )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::NewL()"));

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
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::ConstructL()"));

    iVolumeRepeatTimer = CPeriodic::NewL( CActive::EPriorityStandard );

    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );

    // not detrimental if Media Keys dont work - so ignore any errors here
    TRAP_IGNORE( iInterfaceSelector->OpenTargetL() );
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::~CVideoPlaybackUserInputHandler()
// -------------------------------------------------------------------------------------------------
//
CVideoPlaybackUserInputHandler::~CVideoPlaybackUserInputHandler()
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::~CVideoPlaybackUserInputHandler()"));

    if ( iVolumeRepeatTimer )
    {
        iVolumeRepeatTimer->Cancel();
        delete iVolumeRepeatTimer;
    }

    if ( iInterfaceSelector )
    {
        delete iInterfaceSelector;
        iCoreTarget = NULL;
        iInterfaceSelector = NULL;
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::MrccatoPlay()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::MrccatoPlay( TRemConCoreApiPlaybackSpeed /*aSpeed*/,
                                                  TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(
        _L("CVideoPlaybackUserInputHandler::MrccatoPlay"),
        _L("aButtonAct = %d"), aButtonAct );

    ProcessMediaKey(ERemConCoreApiPlay, aButtonAct);
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::MrccatoCommand()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::MrccatoCommand( TRemConCoreApiOperationId aOperationId,
                                                     TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(
        _L("CVideoPlaybackUserInputHandler::MrccatoCommand"),
        _L("aButtonAct = %d"), aButtonAct );

    ProcessMediaKey(aOperationId, aButtonAct);
}


// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::DoHandleMediaKey()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::DoHandleMediaKey( TRemConCoreApiOperationId aOperationId,
                                                       TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::DoHandleMediaKey()"),
                   _L("aOperationId = %d"), aOperationId );

    switch ( aOperationId )
    {
        case ERemConCoreApiStop:
        {
            if ( aButtonAct == ERemConCoreApiButtonClick )
            {
                TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdStop ) );
            }
            break;
        }
        case ERemConCoreApiRewind:
        {
            HandleRewind( aButtonAct );
            break;
        }
        case ERemConCoreApiFastForward:
        {
            HandleFastForward( aButtonAct );
            break;
        }
        case ERemConCoreApiVolumeUp:
        {
            HandleVolumeUp( aButtonAct );
            break;
        }
        case ERemConCoreApiVolumeDown:
        {
            HandleVolumeDown( aButtonAct );
            break;
        }
        case ERemConCoreApiPausePlayFunction:
        {
            if ( aButtonAct == ERemConCoreApiButtonClick )
            {
                TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdPlayPause ) );
            }
            break;
        }
        case ERemConCoreApiPause:
        {
            TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdPause ) );
            break;
        }
        case ERemConCoreApiPlay:
        {
            if ( aButtonAct == ERemConCoreApiButtonClick )
            {
                TRAP_IGNORE(iViewWrapper->HandleCommandL(EMPXPbvCmdPlay));
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::HandleFastForward()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleFastForward( TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::HandleFastForward()"));

    if ( aButtonAct == ERemConCoreApiButtonPress )
    {
        TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdSeekForward ) );
    }
    else if ( aButtonAct == ERemConCoreApiButtonRelease )
    {
        TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdEndSeek ) );
    }
}


// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::HandleRewind()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleRewind( TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::HandleFastForward()"));

    if ( aButtonAct == ERemConCoreApiButtonPress )
    {
        TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdSeekBackward ) );
    }
    else if ( aButtonAct == ERemConCoreApiButtonRelease )
    {
        TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdEndSeek ) );
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::HandleVolumeUp()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleVolumeUp( TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::HandleVolumeUp()"),
                   _L("aButtonAct = %d"), aButtonAct );

    switch ( aButtonAct )
    {
        case ERemConCoreApiButtonPress:
        {
            // Volume Up - Pressed
            if ( iVolumeRepeatTimer->IsActive() )
            {
                iVolumeRepeatTimer->Cancel();
            }

            iVolumeRepeatUp = ETrue;

            iVolumeRepeatTimer->Start(
                KAknStandardKeyboardRepeatRate,
                KAknStandardKeyboardRepeatRate,
                TCallBack(
                    CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL,
                    this ) );

            break;
        }
        case ERemConCoreApiButtonRelease:
        {
            // Volume Up - Released
            iVolumeRepeatTimer->Cancel();
            break;
        }
        case ERemConCoreApiButtonClick:
        {
            // Volume Up - Clicked
            TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdIncreaseVolume ) );
            break;
        }
    }
}

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::HandleVolumeDown()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleVolumeDown( TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::HandleVolumeDown()"));

    switch ( aButtonAct )
    {
        case ERemConCoreApiButtonPress:
        {
            // Volume Up - Pressed
            if ( iVolumeRepeatTimer->IsActive() )
            {
                iVolumeRepeatTimer->Cancel();
            }

            iVolumeRepeatUp = EFalse;

            iVolumeRepeatTimer->Start(
                KAknStandardKeyboardRepeatRate,
                KAknStandardKeyboardRepeatRate,
                TCallBack(
                    CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL,
                    this ) );

            break;
        }
        case ERemConCoreApiButtonRelease:
        {
            // Volume Up - Released
            iVolumeRepeatTimer->Cancel();
            break;
        }
        case ERemConCoreApiButtonClick:
        {
            // Volume Down - Clicked
            TRAP_IGNORE( iViewWrapper->HandleCommandL( EMPXPbvCmdDecreaseVolume ) );
            break;
        }
    }
}

/*
// -------------------------------------------------------------------------------------------------
// CMPXVideoPlaybackUserInputHandler::ProcessPointerEvent()
// -------------------------------------------------------------------------------------------------
//
void
CMPXVideoPlaybackUserInputHandler::ProcessPointerEventL( CCoeControl* aControl,
                                                         const TPointerEvent& aPointerEvent,
                                                         TMPXVideoControlType aControl )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::ProcessPointerEvent()"),
                   _L("iProcessingInputType = %d, aPointerEvent.iType = %d"),
                   iProcessingInputType, aPointerEvent.iType );

    switch ( iProcessingInputType )
    {
        case EMpxVideoNone:
        {
            if ( aPointerEvent.iType == TPointerEvent::EButton1Down && IsUserInputAllowed() )
            {
                iProcessingInputType = EMpxVideoTouch;

                //
                //  Save the active controls pointer to reroute invalid pointer events
                //
                iActiveControlPtr = aControl;
                iActiveControlType = aMPXControl;

                ReRoutePointerEventL( aControl, aPointerEvent, aMPXControl );
            }

            break;
        }
        case EMpxVideoTouch:
        {
            if ( aControl == iActiveControlPtr )
            {
                //
                //  Event is from the active control, process pointer event normally
                //
                if ( aPointerEvent.iType != TPointerEvent::EButton1Down )
                {
                    ReRoutePointerEventL( aControl, aPointerEvent, aMPXControl );

                    //
                    //  reset the value only on pointer up event - but not on drag
                    //
                    if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                    {
                        iProcessingInputType = EMpxVideoNone;
                        iActiveControlPtr = NULL;
                    }
                }
            }
            else
            {
                //
                //  Event is from non active control
                //  This should not happen, but if event is a button up event,
                //  end the current active control pointer processing
                //
                if ( aPointerEvent.iType == TPointerEvent::EButton1Up )
                {
                    //
                    //  Reroute button up event to active control and end current
                    //  control processing
                    //
                    ReRoutePointerEventL( iActiveControlPtr, aPointerEvent, iActiveControlType );
                    iProcessingInputType = EMpxVideoNone;
                    iActiveControlPtr = NULL;
                }
            }

            break;
        }
    } // switch
}

// -------------------------------------------------------------------------------------------------
//   CMPXVideoPlaybackUserInputHandler::ReRoutePointerEventL()
// -------------------------------------------------------------------------------------------------
//
void CMPXVideoPlaybackUserInputHandler::ReRoutePointerEventL( CCoeControl* aControl,
                                                              const TPointerEvent& aPointerEvent,
                                                              TVideoControlType aMPXControl )
{
    MPX_ENTER_EXIT(_L("CMPXVideoPlaybackUserInputHandler::ReRoutePointerEventL()"),
                   _L("aMPXControl = %d"), aMPXControl );

    if ( aMPXControl == EMpxVideoPlaybackContainer )
    {
        iContainer->DoHandlePointerEventL( aPointerEvent );
    }
    else if ( aMPXControl == EMpxVideoPlaybackControl )
    {
        static_cast<CMPXVideoPlaybackControl*>(aControl)->DoHandlePointerEventL( aPointerEvent );
    }
}*/

// -------------------------------------------------------------------------------------------------
// CVideoPlaybackUserInputHandler::ProcessMediaKey()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::ProcessMediaKey( TRemConCoreApiOperationId aOperationId,
                                                      TRemConCoreApiButtonAction aButtonAct )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::ProcessMediaKey()"),
                   _L("iProcessingInputType = %d, aButtonAct = %d"),
                       iProcessingInputType, aButtonAct );

    switch ( iProcessingInputType )
    {
        case EVideoNone:
        {
            if ( iForeground && !IsKeyLocked() )
            {
                if ( aButtonAct == ERemConCoreApiButtonPress )
                {
                    iProcessingInputType = EVideoMediaKeys;
                    iLastMediaKeyPressed = aOperationId;
                    DoHandleMediaKey( aOperationId, aButtonAct );
                }
                else if ( aButtonAct == ERemConCoreApiButtonClick )
                {
                    DoHandleMediaKey( aOperationId, aButtonAct );

                    // reset on click AND/OR release
                    iProcessingInputType = EVideoNone;
                }
            }

            break;
        }
        case EVideoMediaKeys:
        {
            if ( aButtonAct == ERemConCoreApiButtonRelease )
            {
                // handle only if this release is for media-key being currently handled
                // ignore spurious media key presses
                if ( iLastMediaKeyPressed == aOperationId )
                {
                    DoHandleMediaKey( aOperationId, aButtonAct );
                    // reset on click AND/OR release
                    iProcessingInputType = EVideoNone;
                }
            }
            break;
        }
        default:
        {
            // user input is disallowed
            break;
        }
    } // switch
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL()
// -------------------------------------------------------------------------------------------------
//
TInt CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL( TAny* aPtr )
{
    MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::HandleVolumeRepeatTimeoutL()"));

    static_cast<CVideoPlaybackUserInputHandler*>(aPtr)->HandleVolumeRepeatL();

    return KErrNone;
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::HandleVolumeRepeatL()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::HandleVolumeRepeatL()
{
    MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::HandleVolumeRepeatL()"));

    TMPXVideoPlaybackViewCommandIds command = EMPXPbvCmdDecreaseVolume;

    if ( iVolumeRepeatUp )
    {
        command = EMPXPbvCmdIncreaseVolume;
    }

    iViewWrapper->HandleCommandL( command );
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::SetForeground()
// -------------------------------------------------------------------------------------------------
//
void CVideoPlaybackUserInputHandler::SetForeground( TBool aForeground )
{
    MPX_ENTER_EXIT(_L("CVideoPlaybackUserInputHandler::SetForeground()"),
                   _L("aForeground = %d"), aForeground );

    iForeground = aForeground;

    if ( ! iForeground )
    {
        //
        //  Keyboard focus has been lost
        //  Reset input type and clear volume timer if necessary
        //
        iProcessingInputType = EVideoNone;

        if ( iVolumeRepeatTimer->IsActive() )
        {
            iVolumeRepeatTimer->Cancel();
        }
    }
}

// -------------------------------------------------------------------------------------------------
//   CVideoPlaybackUserInputHandler::IsKeyLocked
// -------------------------------------------------------------------------------------------------
//
TBool CVideoPlaybackUserInputHandler::IsKeyLocked()
{
    TBool keylock( EFalse );
    RProperty::Get( KPSUidAvkonDomain, KAknKeyguardStatus, keylock );

    MPX_DEBUG(_L("CVideoPlaybackUserInputHandler::IsKeyLocked(%d)"), keylock);

    return keylock;
}

// EOF
