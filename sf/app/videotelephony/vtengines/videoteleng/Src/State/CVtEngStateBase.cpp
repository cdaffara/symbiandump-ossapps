/*
* Copyright (c) 2004-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Base class for states
*
*/


// INCLUDE FILES
#include    "CVtEngStateBase.h"
#include    "CVtEngOperation.h"
#include    "CVtEngHandlerContainer.h"
#include    "TVtEngOperationUtils.h"
#include    "CVtEngStateInitializing.h"
#include    "CVtEngStateAlerting.h"
#include    "CVtEngStateRinging.h"
#include    "CVtEngStateConnected.h"
#include    "CVtEngStateClearing.h"
#include    "CVtEngStateOpen.h"
#include    "CVtEngStateNegotiating.h"
#include    "CVtEngStateIdle.h"
#include    "CVtEngSettings.h"
#include    "MVtEngSessionInfo.h"
#include    "VtEngUtils.h"
#include    "VtEngPanic.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateBase::CVtEngStateBase
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateBase::CVtEngStateBase(
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : iStateManager( aStateManager ),
      iEventManager( aEventManager )
    {
    }

// Destructor
CVtEngStateBase::~CVtEngStateBase()
    {
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::OpenL
// Changes state.
//
// -----------------------------------------------------------------------------
//
void CVtEngStateBase::OpenL( CVtEngStateBase* aOldState )
    {
    __VTPRINTENTER( "StateBase.Open" )
    __VTPRINT( DEBUG_GEN, "StateBase. **** State Change ****" )
    iStateManager.SetState( *this );
    if ( aOldState )
        {
        aOldState->Close();
        }
    // call derived class open.
    DoOpenL();
    __VTPRINTEXIT( "StateBase.Open" )
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::Close
// Closes state.
//
// -----------------------------------------------------------------------------
//
void CVtEngStateBase::Close()
    {
    delete this;
    }


// -----------------------------------------------------------------------------
// CVtEngStateBase::SaveToSettingsL
//
// Saves configuration to settings.
// -----------------------------------------------------------------------------
//
void CVtEngStateBase::SaveToSettingsL( CVtEngOperation& aOp )
    {
    const TVtEngCommandId command( aOp.Command() );

    if ( command == KVtEngSetAudioVolume )
        {
        MVtEngAudio::TVtEngOutputVolume volume;
        TVtEngOpParamUtil<MVtEngAudio::TVtEngOutputVolume>::Set( volume, aOp );
        CVtEngSettings& settings = CVtEngUtility::Settings();
        settings.SetVolume( volume.iHandsetVolume, volume.iHandsfreeVolume, ETrue );
        settings.SetVolume( volume.iHandsetVolume, volume.iHandsfreeVolume, EFalse );
        }    
    }
// -----------------------------------------------------------------------------
// CVtEngStateBase::NotifyStateChangeL
// Notifies state change to event manager.
//
// -----------------------------------------------------------------------------
//
void CVtEngStateBase::NotifyStateChange(
    MVtEngSessionInfo::TSessionState /*aNewState*/ )
    {
    iEventManager.SessionStateChanged( );
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::SetVideoEnabled
//
//
// -----------------------------------------------------------------------------
//
void CVtEngStateBase::SetVideoEnabled( TBool aEnabled )
    {
    CVtEngSettings& settings = CVtEngUtility::Settings();
        settings.SetVideoEnabled( aEnabled );
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::HandleL
// Handles an operation when behavior is state dependent.
//
// -----------------------------------------------------------------------------
//
TBool CVtEngStateBase::HandleL( CVtEngOperation& aOp )
    {
    __VTPRINTENTER( "StateBase.Handle" )
    __VTPRINT2( DEBUG_GEN, "StateBase.Handle cmd=%d", aOp.Command() )
    TBool handled( EFalse );
    switch ( aOp.Command() )
        {
        case KVtEngSetAudioRouting:
            // Change audio routing
            AudioHandler().HandleL( aOp );
            break;

        case KVtEngSetAudioVolume:
        case KVtEngIncreaseAudioVolume:
        case KVtEngDecreaseAudioVolume:
            // Change audio volume
            MediaHandler().HandleL( aOp );
            break;
        case KVtEngTerminateSession:
        default:
            break;
        }
    __VTPRINTEXITR( "StateBase.Handle handled=%d", handled )
    return handled;
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::SessionHandler
// Returns session handler.
//
// -----------------------------------------------------------------------------
//
CVtEngSessionHandler& CVtEngStateBase::SessionHandler()
    {
    MVtEngSessionInfo& msession = iStateManager.Handlers().Session();
    CVtEngSessionHandler& handler =
        static_cast<CVtEngSessionHandler&>( msession );
    return handler;
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::MediaHandler
// Returns media handler.
//
// -----------------------------------------------------------------------------
//
CVtEngMediaHandler& CVtEngStateBase::MediaHandler()
    {
    MVtEngMedia& msession = iStateManager.Handlers().Media();
    CVtEngMediaHandler& handler =
        static_cast<CVtEngMediaHandler&>( msession );
    return handler;
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::AudioHandler
// Returns audio handler.
//
// -----------------------------------------------------------------------------
//
CVtEngAudioHandler& CVtEngStateBase::AudioHandler()
    {
    MVtEngAudio& maudio = iStateManager.Handlers().Audio();
    CVtEngAudioHandler& handler =
        static_cast<CVtEngAudioHandler&>( maudio );
    return handler;
    }
// -----------------------------------------------------------------------------
// CVtEngStateBase::SessionHandler
// Returns session handler.
//
// -----------------------------------------------------------------------------
//
CVtEngStateBase* CVtEngStateBase::NewStateL(
    const MVtEngSessionInfo::TSessionState aState )
    {
    CVtEngStateBase* newState = NULL;
    switch ( aState )
        {
        case MVtEngSessionInfo::EIdle:
            newState = new ( ELeave )
                CVtEngStateIdle( iStateManager, iEventManager );
            break;
        case MVtEngSessionInfo::EInitializing:
            newState = new ( ELeave )
                CVtEngStateInitializing( iStateManager, iEventManager );
            break;
        case MVtEngSessionInfo::ERemoteAlerting:
            newState = new ( ELeave )
                CVtEngStateAlerting( iStateManager, iEventManager );
            break;
        case MVtEngSessionInfo::EReceiving:
            newState = new ( ELeave )
                CVtEngStateRinging( iStateManager, iEventManager );
            break;
        case MVtEngSessionInfo::EConnected:
            newState = new ( ELeave )
                CVtEngStateConnected( iStateManager, iEventManager );
            break;
        case MVtEngSessionInfo::ENegotiating:
            newState = new ( ELeave )
                CVtEngStateNegotiating( iStateManager, iEventManager );
            break;
        case MVtEngSessionInfo::EOpen:
            newState = new ( ELeave )
                CVtEngStateOpen( iStateManager, iEventManager );
            break;
        case MVtEngSessionInfo::EClearing:
            newState = new ( ELeave )
                CVtEngStateClearing( iStateManager, iEventManager );
            break;
        default:
            Panic( EVtEngPanicInvalidSessionState );
            break;
        }
    return newState;
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::NegotiatingOrConnectedStateL
//
// -----------------------------------------------------------------------------
//
CVtEngStateBase* CVtEngStateBase::NegotiatingOrConnectedStateL(
    TBool aAcceptOnlyNegotiating )
    {
    __VTPRINTENTER( "StateBase.NegOrCon" )
    __VTPRINT2( DEBUG_GEN, "StateBase.NegOrCon %d", aAcceptOnlyNegotiating)
    const MVtCtlCallControl::TVtCtlState bearerstate =
        SessionHandler().RealState();
    CVtEngStateBase* newState = NULL;
    if ( bearerstate == MVtCtlCallControl::EConnected )
        {
        const MVtProtocolCommand::TVtProtocolState state( MediaHandler().ProtoState() );
        const CVtEngSettings& settings = CVtEngUtility::Settings();
        const TBool readyForConnect = settings.Config().iReadyForConnect;

		__VTPRINT3( DEBUG_GEN, "StateBase.NegOrCon readyForConnect:%d, state:%d", readyForConnect, state)
		__VTPRINT2( DEBUG_GEN, "StateBase.NegOrCon iIsDataportLoaned:%d", settings.Config().iIsDataportLoaned)

        if ( readyForConnect && state == MVtProtocolCommand::ESetup && settings.Config().iIsDataportLoaned )
            {
            __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                "StateBase.NegOrCon ready & ESetup")
            const TInt res( MediaHandler().Connect() );
            if ( res == KErrNotReady )
                {
                // Cannot be done yet, need to wait for an operation
                // to complete.
                __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                    "StateBase.NegOrCon not ready" )
                }
            else if ( res != KErrNone )
                {
                __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                    "StateBase.NegOrCon fail")
                iEventManager.NotifyEvent( KVtEngNegotiationProblem );
                }
            else
                {
                // change to negotiating state
                newState = NewStateL( MVtEngSessionInfo::ENegotiating );
                __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                    "StateBase.NegOrCon-->negotiating")
                }
            }
        else if ( !aAcceptOnlyNegotiating )
            {
            newState = NewStateL( MVtEngSessionInfo::EConnected );
            __VTPRINT( DEBUG_GEN | DEBUG_DETAIL,
                "StateBase.NegOrCon-->connected")
            }
        }
    __VTPRINTEXIT( "StateBase.NegOrCon" )
    return newState;
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::TerminateSessionL
//
// -----------------------------------------------------------------------------
//
void CVtEngStateBase::TerminateSessionL( CVtEngOperation& aOp )
    {
    // State transition to "clearing state" initiates disconnect on protocol.
    CVtEngStateBase* newState = NewStateL( MVtEngSessionInfo::EClearing );
    // just store aOp for callback to UI
    CVtEngMediaHandler& mHandler = MediaHandler();
    CleanupStack::PushL( newState );
    newState->OpenL( this );
    CleanupStack::Pop(); // newState
    mHandler.HandleL( aOp );
    }

// -----------------------------------------------------------------------------
// CVtEngStateBase::ValidateCommand
//
// -----------------------------------------------------------------------------
//
TBool CVtEngStateBase::ValidateCommand(
            const TVtEngCommandId aCommandId )
    {
    __VTPRINT( DEBUG_DETAIL | DEBUG_GEN, "StateBase.Validate")
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
            // Flow through, ok to execute ==>  check the state possibilities
        case KVtEngInitializeEngine:
        case KVtEngInitializeEngineDiag:
        case KVtEngInitializeEngineTest:
        case KVtEngResetEngine:
        case KVtEngStartRenderRemote:
        case KVtEngStopRenderRemote:
        case KVtEngPrepareRemoteRender:
        case KVtEngPrepareRemoteRenderDSA:
        case KVtEngPrepareRemoteRenderDP:
        case KVtEngPrepareRemoteRenderNGA:        
        case KVtEngPrepareViewFinder:
        // Media
        case KVtEngSetUIForeground:
            okToPerform = ETrue;
            break;
        case KVtEngPrepareCamera:
        case KVtEngSetSource:
        case KVtEngSetZoomStep:
        case KVtEngStartViewFinder:
        case KVtEngFreeze:
        case KVtEngUnfreeze:
        case KVtEngSetVideoQuality:
        case KVtEngHandleLayoutChange:

        // Media object sharing
        case KVtEngInitializeShareImage:
        case KVtEngStartShareImage:
        case KVtEngStopShareImage:

        case KVtEngMuteOutgoingAudio:
        case KVtEngUnmuteOutgoingAudio:
        case KVtEngSetAudioRouting:
        case KVtEngSetAudioVolume:
        case KVtEngIncreaseAudioVolume:
        case KVtEngDecreaseAudioVolume:
        case KVtEngRequestLastRemoteFrame:

            okToPerform =
                ( MediaHandler().ValidateCommand( aCommandId ) == KErrNone );
            break;
        default:
            case KVtEngTerminateSession: // valid only in open state
            break;
        }
    return okToPerform;
    }

//  End of File

