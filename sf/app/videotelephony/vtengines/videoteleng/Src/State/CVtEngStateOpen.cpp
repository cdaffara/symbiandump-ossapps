/*
* Copyright (c) 2006 - 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Open state handle interfaces implementation
*
*/


// INCLUDE FILES
#include    "CVtEngStateOpen.h"
#include    "CVtEngOperation.h"
#include    "CVtEngAudioHandler.h"
#include    <cvtlogger.h>

#include    "CVtEngExtensions.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVtEngStateOpen::CVtEngStateOpen
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CVtEngStateOpen::CVtEngStateOpen( 
    CVtEngStateManager& aStateManager,
    CVtEngEventManager& aEventManager )
    : CVtEngStateBase( aStateManager, aEventManager )
    {
    }

// Destructor
CVtEngStateOpen::~CVtEngStateOpen()
    {
    __VTPRINT( DEBUG_DESTRUCT, "~StateOpen")
    }


// -----------------------------------------------------------------------------
// CVtEngStateOpen::UpdateL
// Updates state
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CVtEngStateOpen::UpdateL()
    {
    __VTPRINTENTER( "StateOpen.Update" )
    const MVtCtlCallControl::TVtCtlState bearerstate = 
        SessionHandler().RealState();

    const MVtProtocolCommand::TVtProtocolState protoState( MediaHandler().ProtoState() );
    const TBool bearerIdle = ( bearerstate == MVtCtlCallControl::EIdle ||
        bearerstate == MVtCtlCallControl::EUnknown );
    const TBool bearerDisconnecting = 
        ( bearerstate == MVtCtlCallControl::EDisconnecting );

    CVtEngStateBase* newState = NULL;

    if ( protoState == MVtProtocolCommand::ESetup || 
         protoState == MVtProtocolCommand::EIdle  || 
         bearerIdle )
        {
        newState = NewStateL( MVtEngSessionInfo::EIdle );
        }
    else if ( bearerDisconnecting || protoState == MVtProtocolCommand::EDisconnecting ) 
        {
        newState = NewStateL( MVtEngSessionInfo::EClearing );
        }
    if ( newState )
        {
        newState->OpenL( this );
        }
    __VTPRINTEXIT( "StateOpen.Update" )
    }

// -----------------------------------------------------------------------------
// CVtEngStateOpen::OpenL
// State transition to open.
// -----------------------------------------------------------------------------
//
void CVtEngStateOpen::DoOpenL()
    {
    __VTPRINTENTER( "StateOpen.Open" )
    
    // set audio routing (and ouput volume)
    RouteAudioL( NULL );

    NotifyStateChange( MVtEngSessionInfo::EOpen );
    __VTPRINTEXIT( "StateOpen.Open" )
    }

// -----------------------------------------------------------------------------
// CVtEngStateOpen::ValidateCommand
// Returns if a command is valid in open state.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateOpen::ValidateCommand(
            const TVtEngCommandId aCommandId )
    {
    __VTPRINT( DEBUG_DETAIL | DEBUG_GEN, "StateOpen.Validate")
    TBool okToPerform( EFalse );
    switch ( aCommandId )
        {
        case KVtEngInitializeEngine: // temporary     
        case KVtEngPrepareCamera:
        case KVtEngSetZoomStep:
        case KVtEngStartViewFinder:
        case KVtEngPauseViewFinder:
        case KVtEngStopViewFinder:
        case KVtEngStartRenderRemote: 
        case KVtEngStopRenderRemote: 
        case KVtEngPauseRenderRemote:
        case KVtEngTerminateSession:
        case KVtEngStartDtmfTone:
        case KVtEngStopDtmfTone:
        // Handling of different UIIs, support is there
        case KVtEngSetUIIDtmfSupport:
        case KVtEngSetUIIBasicStringSupport:
        case KVtEngSetUIIIA5StringSupport:
        case KVtEngSetUIIGeneralStringSupport:
        case KVtEngSwitchToVoice:
            okToPerform = ETrue;
            break;

        // Extension commands
        case KVtEngSetContrast:
        case KVtEngSetBrightness:
        case KVtEngSetWhiteBalance:
        case KVtEngSetColorTone:
#if defined ( RD_VT_RTF )
        case KVtEngStartRecord:
        case KVtEngStopRecord:
#endif // RD_VT_RTF
        	{
        	// Get extension handle from Utils
        	__VTPRINT(DEBUG_GEN, "StateOpen extension->ValidateCommand")
            okToPerform = CVtEngUtility::Extensions()->ValidateCommand(
            	aCommandId );
            break;
        	}

        default:
            okToPerform = CVtEngStateBase::ValidateCommand( aCommandId );
            break;
        }
    return okToPerform;
    }

// -----------------------------------------------------------------------------
// CVtEngStateOpen::State
// Returns session state.
// 
// -----------------------------------------------------------------------------
//
MVtEngSessionInfo::TSessionState CVtEngStateOpen::State() const
    {
    return MVtEngSessionInfo::EOpen;
    }

// -----------------------------------------------------------------------------
// CVtEngStateOpen::HandleL
// Handles an operation when behavior is state dependent.
// 
// -----------------------------------------------------------------------------
//
TBool CVtEngStateOpen::HandleL( CVtEngOperation& aOp )
    {
    __VTPRINTENTER( "StateOpen.HandleL" )
    TBool handled( CVtEngStateBase::HandleL( aOp ) );
    switch ( aOp.Command() )
        {
        case KVtEngTerminateSession:
            TerminateSessionL( aOp );
            handled = ETrue;
            break;
      
        case KVtEngSwitchToVoice:
            {
            CVtEngStateBase* newState = 
                NewStateL( MVtEngSessionInfo::EClearing );
            CleanupStack::PushL( newState );
            newState->OpenL( this );        
            CleanupStack::Pop();
            // aOp is not passed to media handler because mediator command
            // sender signals completion, see
            // CVtEngMdtrCommandSender::HandleVtEventL
            }
            break;
            
        // Extension commands
        case KVtEngSetContrast:
        case KVtEngSetBrightness:
        case KVtEngSetWhiteBalance:
        case KVtEngSetColorTone:
#if defined ( RD_VT_RTF )
        case KVtEngStartRecord:
        case KVtEngStopRecord:
#endif // RD_VT_RTF
            CallExtensionL( &aOp);
            break;
        default:
            break;
        }
    __VTPRINTEXITR( "StateOpen.HandleL %d", handled )
    return handled;
    }

// -----------------------------------------------------------------------------
// CVtEngStateOpen::RouteAudioL
// Handles audio routing operation.
// 
// -----------------------------------------------------------------------------
//
void CVtEngStateOpen::RouteAudioL( 
    CVtEngOperation* aOp )
    {
    __VTPRINTENTER( "StateOpen.RouteAudioL" )
    CVtEngSettings& settings = CVtEngUtility::Settings();
    const CVtEngSettings::TVtEngVideoCallConfig& config = settings.Config();

    CVtEngAudioHandler& audioHandler = AudioHandler();
    MVtEngAudio::TAudioRoutingState routingState( MVtEngAudio::EAudioWiredHeadset );
    audioHandler.GetRoutingState( routingState );

    TBool routeAudio( ETrue );
    TInt volume( config.iAudio.iVolume.iHandsfreeVolume );
    switch ( routingState )
        {
        case MVtEngAudio::EAudioWiredHeadset:
             routeAudio = EFalse;
             // fall thru
        case MVtEngAudio::EAudioHandset:
        case MVtEngAudio::EAudioBT:
            volume = config.iAudio.iVolume.iHandsetVolume;
            break;
        // use default values
        case MVtEngAudio::EAudioLoudspeaker:
        default:
            break;
        }
    __VTPRINT3( DEBUG_GEN | DEBUG_DETAIL, 
        "StateOpen.RouteAudioL route=%d,vol=%d", routeAudio,volume )
    if ( aOp == NULL || ( aOp && routeAudio ) )
        {
        // set volume if state was changed (called from DoOpenL) or
        // audio routing will be changed
        __VTPRINT( DEBUG_GEN | DEBUG_DETAIL, "StateOpen.RouteAudioL SetVol" )
        MediaHandler().SetVolume( volume );
        }

    // *** AUDIO && ( aOp != NULL ) ***
    if ( routeAudio && ( aOp != NULL ) )
        {
        if ( aOp )
            {
            audioHandler.HandleL( *aOp );
            }
        }
    __VTPRINTEXIT( "StateOpen.RouteAudioL" )
    }

    
// -----------------------------------------------------------------------------
// CVtEngStateOpen::CallExtensionL
// Handles extension operations.
//
// -----------------------------------------------------------------------------
//
void CVtEngStateOpen::CallExtensionL(
    CVtEngOperation* aOp)
    {
    __VTPRINTENTER( "StateOpen.CallExtensionL" )
    CVtEngUtility::Extensions()->OfferOperationL( aOp );
    __VTPRINTEXIT( "StateOpen.CallExtensionL" )
    }

// End of File
