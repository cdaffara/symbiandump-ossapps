/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handler utilities implementation.
*
*/



// INCLUDE FILES
#include    "TVtEngOperationUtils.h"
#include    "VtEngPanic.h"
#include    "CVtEngInitializer.h"
#include    "VtEngUtils.h"
#include    "CVtEngOperation.h"
#include    "CVtEngHandlerContainer.h"
#include    "CVtEngMediaHandler.h"
#include    "CVtEngAudioHandler.h"
#include    "CVtEngSessionHandler.h"
#include    "CVtEngStateManager.h"
#include    "CVtEngSettings.h"
#include    "cvtengmdtrcommandsender.h"
#include    <cvtlogger.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TVtEngRenderUtil::HandleL
// unpacks and handles a rendering command
// 
// -----------------------------------------------------------------------------
//
void TVtEngRenderUtil::HandleL( 
    CVtEngOperation& aOperation,
    CVtEngHandlerContainer& aHandlers )
    {
    CVtEngMediaHandler& media =
        static_cast<CVtEngMediaHandler&>( aHandlers.Media() );

    media.HandleL( aOperation );
    }

// -----------------------------------------------------------------------------
// TVtEngRenderUtil::Cancel
// cancel the specify pending command
// 
// -----------------------------------------------------------------------------
//

void TVtEngRenderUtil::Cancel(
    CVtEngOperation& aOperation,
    CVtEngHandlerContainer& aHandlers )
    {    
    const TVtEngCommandId id = aOperation.Command();
    __VTPRINTENTER( "TVtEngRenderUtil.Cancel")
    __VTPRINT2( DEBUG_GEN, "Command id = %d", id)
    if ( id == KVtEngInitializeShareImage )        
        {
        CVtEngMediaHandler& media =
            static_cast<CVtEngMediaHandler&>( aHandlers.Media() );
            
        media.CancelInitializeShareImage();
        }
    __VTPRINTEXIT( "TVtEngRenderUtil.Cancel")
    }
// -----------------------------------------------------------------------------
// TVtEngAudioRoutingUtil::HandleL
// unpacks and handles audio routing command.
// 
// -----------------------------------------------------------------------------
//

void TVtEngAudioRoutingUtil::HandleL( 
    CVtEngOperation& aOperation )
    {
    CVtEngUtility::StateManager()->HandleOperationL( aOperation );
    }

// -----------------------------------------------------------------------------
// TVtEngAudioPlaybackUtil::HandleL
// unpacks and handles audio playback command.
// 
// -----------------------------------------------------------------------------
//
void TVtEngAudioPlaybackUtil::HandleL( 
    CVtEngOperation& aOperation,
    CVtEngHandlerContainer& aHandlers )
    {
    __VTPRINTENTER( "AudioPlaybackUtil.HandleL" )
    if ( aOperation.Command() == KVtEngSetAudioVolume ) 
        {
        CVtEngSettings& settings = CVtEngUtility::Settings();
        const TPckgC<MVtEngAudio::TVtEngOutputVolume>& pckg = 
            static_cast<const TPckgC<MVtEngAudio::TVtEngOutputVolume>&>
            ( *aOperation.Parameters() );

        settings.SetVolume( pckg().iHandsetVolume, pckg().iHandsfreeVolume,
            ETrue );
        }

    // All the commands are though passed to handler
    CVtEngMediaHandler& media =
        static_cast<CVtEngMediaHandler&>( aHandlers.Media() );
    __VTPRINT( DEBUG_DETAIL, "TVtEngAudioPlaybackUtil::HandleL:Handler fetched")

    media.HandleL( aOperation );
    __VTPRINTEXIT( "AudioPlaybackUtil.HandleL" )
    }

// -----------------------------------------------------------------------------
// TVtEngMediaTransportUtil::HandleL
// unpacks and handles media transport command.
// 
// -----------------------------------------------------------------------------
//
void TVtEngMediaTransportUtil::HandleL( 
    CVtEngOperation& aOperation,
    CVtEngHandlerContainer& aHandlers )
    {
    __VTPRINTENTER( "MediaTransportUtil.HandleL" )
    const TVtEngCommandId command( aOperation.Command() );
    if ( command == KVtEngStopDtmfTone ||
         command == KVtEngStartDtmfTone ||
		 // DTMF commands
		 command == KVtEngSetUIIDtmfSupport ||
         command == KVtEngSetUIIBasicStringSupport ||
         command == KVtEngSetUIIIA5StringSupport ||
         command == KVtEngSetUIIGeneralStringSupport )
        {
        aHandlers.Dtmf().HandleL( aOperation );
        }
        
    __VTPRINTEXIT( "MediaTransportUtil.HandleL" )
    }

// -----------------------------------------------------------------------------
// TVtEngViewFinderConfigureUtil::HandleL
// unpacks and handles 
// 
// -----------------------------------------------------------------------------
//
void TVtEngViewFinderConfigureUtil::HandleL( 
    CVtEngOperation& aOperation,
    CVtEngHandlerContainer& aHandlers )
    {
    CVtEngMediaHandler& media =
        static_cast<CVtEngMediaHandler&>( aHandlers.Media() );

    media.HandleL( aOperation );
    }

// -----------------------------------------------------------------------------
// TVtEngCameraConfigUtil::HandleL
// unpacks and handles 
// 
// -----------------------------------------------------------------------------
//
void TVtEngCameraConfigUtil::HandleL( 
    CVtEngOperation& /*aOperation*/,
    CVtEngHandlerContainer& /*aHandlers*/ )
    {
    }

// -----------------------------------------------------------------------------
// TVtEngEngineInitUtil::HandleL
// Handles engine initialization and uninitialization command.
// 
// -----------------------------------------------------------------------------
//
void TVtEngEngineInitUtil::HandleL( 
    CVtEngOperation& aOperation,
    CVtEngHandlerContainer& /* aHandlers */ )
    {
    __VTPRINTENTER( "EngineInitUtil.HandleL" )
    const TVtEngCommandId id = aOperation.Command();

    CVtEngInitializer& initializer = 
        CVtEngUtility::EngineUtils()->Initializer();

    switch ( id )
        {
        case KVtEngInitializeEngine:
        case KVtEngInitializeEngineDiag:
        case KVtEngInitializeEngineTest:
            initializer.InitializeL( aOperation );
            break;
        case KVtEngResetEngine:
            initializer.Uninitialize( aOperation );
            break;
        case KVtEngTerminateSession:
            CVtEngUtility::StateManager()->HandleOperationL( aOperation );
            break;
        case KVtEngSwitchToVoice:
            CVtEngUtility::MediatorCommands().SwitchToVoiceL( aOperation );
            break;
        default:
            Panic( EVtEngPanicIncorrectUtility );
            break;
        }
    __VTPRINTEXIT( "EngineInitUtil.HandleL" )
    }

void TVtEngEngineInitUtil::Cancel( CVtEngOperation& aOperation )
    {
    const TVtEngCommandId id = aOperation.Command();
    if ( id == KVtEngInitializeEngine )
        {
        CVtEngInitializer& initializer = 
            CVtEngUtility::EngineUtils()->Initializer();
        initializer.CancelInit();
        }
    }

// -----------------------------------------------------------------------------
// TVtEngExtensionUtil::HandleL
// handles extension operations.
//
// -----------------------------------------------------------------------------
//
void TVtEngExtensionUtil::HandleL(
    CVtEngOperation& aOperation )
    {
    CVtEngUtility::StateManager()->HandleOperationL( aOperation );
    }

//  End of File
