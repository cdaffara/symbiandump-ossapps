/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Acessory and Bluetooth handler.
*
*/


// INCLUDE FILES
#include    "phoneaudioutil.h"
#include    "phonenoteutil.h"
#include    "cphoneaccessorybthandler.h"
#include    "cphonecenrepproxy.h"
#include    "cphonemainresourceresolver.h"
#include    "mphoneneclearedhandler.h"
#include    "mphonestatemachine.h"
#include    "mphoneviewcommandhandle.h"
#include    "cphonestatehandle.h"
#include    "tphonecmdparamboolean.h"
#include    "tphonecmdparamaudiooutput.h"
#include    "cphonebtaadisconnecthandler.h"
#include    "phonerssbase.h"
#include    "tphonecmdparaminteger.h"
#include    "tphonecmdparamaudioavailability.h"

#include    <mpeengineinfo.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------
// PhoneAudioUtil::HandleAudioMuteChangedL
// Default handling for EPEMessageAudioMuteChanged message
// (other items were commented in a header).
// -----------------------------------------------------------
//
void PhoneAudioUtil::HandleAudioMuteChangedL()
    {
    TPhoneCmdParamBoolean booleanParam;
    const TBool audioMute = StateMachine()->PhoneEngineInfo()->AudioMute();
    booleanParam.SetBoolean( audioMute );
    ViewCommandHandle()->ExecuteCommandL( EPhoneViewActivateMuteUIChanges,
        &booleanParam );
    }

// ---------------------------------------------------------
// PhoneAudioUtil::HandleAudioOutputChangedL
// ---------------------------------------------------------
//
void PhoneAudioUtil::HandleAudioOutputChangedL()
    {    
    MPhoneViewCommandHandle* viewCommandHandle = 
            ViewCommandHandle();

    MPhoneStateMachine* stateMachine = StateMachine();
    
    TPhoneCmdParamAudioOutput outputParam;
    const TPEAudioOutput audioOutput =
            stateMachine->PhoneEngineInfo()->AudioOutput();
    outputParam.SetAudioOutput( audioOutput );

    viewCommandHandle->ExecuteCommandL( EPhoneViewActivateAudioPathUIChanges,
        &outputParam );

    // Previous Output
    TPEPhoneAudioRouteParameters RouteParameters = stateMachine->
        PhoneEngineInfo()->RouteParameters();
    const TPEAudioOutput previousOutput =
        RouteParameters.iPreviousOutput;

    TBool btAvailable = stateMachine->PhoneEngineInfo()->AudioOutputAvailable(
            EPEBTAudioAccessory );
    // Show note or BT disconnect handler
    if ( audioOutput != EPENotActive &&
         previousOutput == EPEBTAudioAccessory &&
         !btAvailable )
        {
        CPhoneBtaaDisconnectHandler::InstanceL()->HandleConnectionLostL();
        }
    else if ( audioOutput == EPEBTAudioAccessory && 
            previousOutput != EPEBTAudioAccessory &&
            btAvailable )
        {
        CPhoneAccessoryBTHandler* bt = CPhoneAccessoryBTHandler::NewLC(
                viewCommandHandle, stateMachine );
        bt->ShowBTActivatedL();
        CleanupStack::PopAndDestroy( bt );
        }
    else if ( audioOutput == EPENotActive &&
              CPhoneBtaaDisconnectHandler::InstanceL()->IsQuery() )
        {
        CPhoneBtaaDisconnectHandler::InstanceL()->Cancel();
        }
    else if ( RouteParameters.iShowNote && audioOutput == EPELoudspeaker )
        {
        // Avkon removal
        // TODO Play tone
        }
    }

// -----------------------------------------------------------
// PhoneAudioUtil::DecreaseAudioVolume()
// -----------------------------------------------------------
//
void PhoneAudioUtil::DecreaseAudioVolumeL()
    {
    MPhoneStateMachine* stateMachine = StateMachine();
    TPEAudioOutput output( stateMachine->PhoneEngineInfo()->AudioOutput() );

    if( output == EPETTY )
        {
        PhoneNoteUtil::SendGlobalInfoNoteL( 
                EPhoneNoteTTYNoAudioControl, ETrue );
        }
    else
        {
        TInt audioVolume( stateMachine->PhoneEngineInfo()->AudioVolume() );
        audioVolume--;
        ChangeAudioVolumeL( audioVolume, ETrue );
        }
    }

// -----------------------------------------------------------
// PhoneAudioUtil::IncreaceAudioVolume()
// -----------------------------------------------------------
//
void PhoneAudioUtil::IncreaseAudioVolumeL()
    {
    MPhoneStateMachine* stateMachine = StateMachine();
    TPEAudioOutput output( stateMachine->PhoneEngineInfo()->AudioOutput() );

    if( output == EPETTY )
        {
        PhoneNoteUtil::SendGlobalInfoNoteL( 
                EPhoneNoteTTYNoAudioControl, ETrue );
        }
    else
        {
        TInt audioVolume( stateMachine->PhoneEngineInfo()->AudioVolume() );
        audioVolume++;
        ChangeAudioVolumeL( audioVolume, ETrue );
        }
    }

// -----------------------------------------------------------
// PhoneAudioUtil::ChangeAudioVolumeL()
// -----------------------------------------------------------
//
void PhoneAudioUtil::ChangeAudioVolumeL( TInt aLevel, TBool aUpdateControl )
    {
    TInt valueToControl = aLevel;
    // sets value between 1 -10
    if ( aLevel>=KPhoneVolumeMinValue && aLevel<=KPhoneVolumeMaxValue )
        {
        MPhoneStateMachine* stateMachine = StateMachine();
        
        stateMachine->PhoneEngineInfo()->SetAudioVolumeCommand( aLevel );
        // Syncronizes audio volume level between engine and ui
        // causes EPEMessageAudioVolumeChanged message to phoneUI
        stateMachine->SendPhoneEngineMessage(
            MPEPhoneModel::EPEMessageSetAudioVolume );
        }
        
    if ( aUpdateControl )
        {
        // Update the volume display.
        // Upper max (11) and under min (-1)
        // values are also updated to volume popup.
        TPhoneCmdParamInteger volumeParam;
        volumeParam.SetInteger( valueToControl );
        ViewCommandHandle()->ExecuteCommandL(
            EPhoneViewSetNaviPaneAudioVolume,
            &volumeParam );
        }
    }

// -----------------------------------------------------------
// PhoneAudioUtil::GetVolumeLevel
// -----------------------------------------------------------
//
TInt PhoneAudioUtil::GetVolumeLevel()
    {
    TPhoneCmdParamInteger integerParam;
    ViewCommandHandle()->ExecuteCommand( EPhoneViewGetAudioVolumeLevel,
                                         &integerParam );
    return integerParam.Integer();
    }

// -----------------------------------------------------------
// PhoneAudioUtil::HandleAudioAvailableOutputChangedL
// Default handling for EPEMessageAvailableAudioOutputsChanged message
// (other items were commented in a header).
// -----------------------------------------------------------
//
void PhoneAudioUtil::HandleAudioAvailableOutputChangedL()
    {
    MPhoneStateMachine* stateMachine = StateMachine();
    TPhoneCmdParamAudioAvailability outputParam;
    // Output
    const TPEAudioOutput audioOutput =
            stateMachine->PhoneEngineInfo()->AudioOutput();
    // BT availability
    TBool btAvailable = stateMachine->PhoneEngineInfo()->
            AudioOutputAvailable( EPEBTAudioAccessory );
    outputParam.SetBTAccAvailable( btAvailable );
    // Wired availability
    TBool wiredAvailable = stateMachine->PhoneEngineInfo()->
            AudioOutputAvailable( EPEWiredAudioAccessory );
    outputParam.SetWiredAccAvailable( wiredAvailable );
    // BTA disconnect handler check
    if( btAvailable )
        {
        CPhoneBtaaDisconnectHandler::InstanceL()->Cancel();
        }
    
    ViewCommandHandle()->ExecuteCommandL( 
            EPhoneViewActivateAudioAvailabilityUIChanges,
            &outputParam );
    }

// -----------------------------------------------------------
// PhoneAudioUtil::ToggleMute
// -----------------------------------------------------------
//
void PhoneAudioUtil::ToggleMute()
    {
    MPhoneStateMachine* stateMachine = StateMachine();
    stateMachine->PhoneEngineInfo()->AudioMute() ?
        stateMachine->PhoneEngineInfo()->SetAudioMuteCommand( EFalse ):
        stateMachine->PhoneEngineInfo()->SetAudioMuteCommand( ETrue );
    
    stateMachine->SendPhoneEngineMessage(
        MPEPhoneModel::EPEMessageSetAudioMute );
    }

// ---------------------------------------------------------
// PhoneAudioUtil::SetHandsfreeModeL
// ---------------------------------------------------------
//
void PhoneAudioUtil::SetHandsfreeModeL( TBool aHandsfreeMode )
    {
    CPhoneAccessoryBTHandler* bt = CPhoneAccessoryBTHandler::NewLC(
            ViewCommandHandle(), StateMachine() );
    if ( !bt->SetHandsfreeModeL( aHandsfreeMode ))
        {
        PhoneNoteUtil::SendGlobalErrorNoteL( 
                EPhoneNoteTextNotAllowed, ETrue );
        }
    CleanupStack::PopAndDestroy( bt );
    }

// ---------------------------------------------------------
// PhoneAudioUtil::SetBTHandsfreeModeL
// ---------------------------------------------------------
//
void PhoneAudioUtil::SetBtHandsfreeModeL( TBool aHandsfreeMode )
    {
    CPhoneAccessoryBTHandler* bt = CPhoneAccessoryBTHandler::NewLC(
            ViewCommandHandle(), StateMachine() );
    if ( !bt->SetBTHandsfreeModeL( aHandsfreeMode ))
        {
        PhoneNoteUtil::SendGlobalErrorNoteL( 
                EPhoneNoteTextNotAllowed, ETrue );
        }
    CleanupStack::PopAndDestroy( bt );
    }

// ---------------------------------------------------------
//  PhoneAudioUtil::ViewCommandHandle
// ---------------------------------------------------------
//
MPhoneViewCommandHandle* PhoneAudioUtil::ViewCommandHandle()
    {   
    return CPhoneStateHandle::Instance()->ViewCommandHandle();
    }

// ---------------------------------------------------------
//  PhoneAudioUtil::StateMachine
// ---------------------------------------------------------
//
MPhoneStateMachine* PhoneAudioUtil::StateMachine()
    {   
    return CPhoneStateHandle::Instance()->StateMachine();
    }

//  End of File  
