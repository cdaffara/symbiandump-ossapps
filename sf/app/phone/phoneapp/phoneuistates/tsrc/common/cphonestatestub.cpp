/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/


// INCLUDE FILES
#include <aknkeylock.h>
#include <avkon.rsg>
#include <bautils.h> 
#include <eikenv.h>
#include <StringLoader.h>
#include <FeatMgr.h>
#include <rconnmon.h>
#include <Profile.hrh>
#include <PUAcodes.hrh>
#include <aknutils.h>
#include <aknsoundsystem.h>
#include <WlanInternalPSKeys.h>
#include <btengdomainpskeys.h>
#include <btengdomaincrkeys.h>
#include <SettingsInternalCRKeys.h>
#include <starterclient.h>
#include <rsssettings.h>

#include <TelephonyDomainPSTypes.h>
#include <telinformationpskeys.h>
#include <coreapplicationuisdomainpskeys.h>
#include <mccecall.h>
#include <ccpdefs.h>
#include <activeidle2domainpskeys.h>
#include <LogsDomainCRKeys.h>
#include <StartupDomainPSKeys.h>

#include "PhoneUI.pan"
#include "CPhoneState.h"
#include "MPhoneStateMachine.h"
#include "mphonestorage.h"
#include "MPhoneViewCommandHandle.h"
#include "TPhoneCmdParamBoolean.h"
#include "TPhoneCmdParamInteger.h"
#include "TPhoneCmdParamKeyEvent.h"
#include "TPhoneCmdParamDynMenu.h"
#include "TPhoneCmdParamCallHeaderData.h"
#include "TPhoneCmdParamIncallIndicatorData.h"
#include "TPhoneCmdParamNote.h"
#include "TPhoneCmdParamKeyCapture.h"
#include "TPhoneCmdParamGlobalNote.h"
#include "TPhoneCmdParamQuery.h"
#include "TPhoneCmdParamString.h"
#include "TPhoneCmdParamCallStateData.h"
#include "TPhoneCmdParamBitmap.h"
#include "TPhoneCmdParamAudioOutput.h"
#include "TPhoneCmdParamAudioAvailability.h"
#include "TPhoneCmdParamAppInfo.h"
#include "TPhoneCmdParamTransEffect.h"
#include "TPhoneCmdParamRingTone.h"


const TInt KPhoneBtAddressTextLength = 64;


// ================= MEMBER FUNCTIONS =======================

CPhoneState::CPhoneState(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle, 
    MPhoneCustomization* aCustomization) :
    iStateMachine( aStateMachine ),
        iViewCommandHandle( aViewCommandHandle ),
        iCustomization( aCustomization )
    {
    }

EXPORT_C void CPhoneState::BaseConstructL()
    {
    }
    
CPhoneState::~CPhoneState()
    {
    iStateMachine = NULL;
    iViewCommandHandle = NULL;
    iCustomization = NULL;
    }
    
EXPORT_C CEikonEnv* CPhoneState::EikonEnv() const
    {    
    return iEnv;
    }

EXPORT_C void CPhoneState::SetEikonEnv( CEikonEnv* aEnv )
    {    
    iEnv = aEnv;
    }    


void CPhoneState::DisplayCallHeaderL( 
        TInt aCallId,
        TNumberEntrySetRule aRule )
    {
    }
    
EXPORT_C void CPhoneState::UpdateUiCommands()
    {
    
    }    
    
// <-------------------------- PHONE ENGINE EVENTS --------------------------->

// -----------------------------------------------------------
// CPhoneState::HandlePhoneEngineMessageL
// Default handling for Phone Engine messages
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneState::HandlePhoneEngineMessageL( 
    const TInt /*aMessage*/, 
    TInt /*aCallId*/ )
    {
    }

// -----------------------------------------------------------
// CPhoneState::HandleAudioMuteChangedL
// Default handling for EPEMessageAudioMuteChanged message 
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneState::HandleAudioMuteChangedL()
    {
    }

// -----------------------------------------------------------
// CPhoneState::HandleAudioOutputChangedL
// Default handling for EPEMessageAudioOutputChanged message 
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneState::HandleAudioOutputChangedL()
    {
    }

// -----------------------------------------------------------
// CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled
// -----------------------------------------------------------
//
TBool CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled()
    {
    TPhoneCmdParamBoolean isSecurityMode;      
    iViewCommandHandle->ExecuteCommandL( EPhoneViewGetSecurityModeStatus, &isSecurityMode ); 
    if ( SimState() == EPESimNotPresent && isSecurityMode.Boolean() )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// -----------------------------------------------------------
// CPhoneState::HandleSimStateChangedL
// Handles changes in the SIM card state.
// -----------------------------------------------------------
//
void CPhoneState::HandleSimStateChangedL()
    {

    }

// -----------------------------------------------------------
// CPhoneState::HandleErrorL
// Default handling for HandleError message 
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneState::HandleErrorL( const TPEErrorInfo& /*aErrorInfo*/ )
    {
       
    }
   
// -----------------------------------------------------------
// CPhoneState::HandleChangedCallDurationL
// Default handling for EPEMessageChangedCallDuration message 
// (other items were commented in a header).
// -----------------------------------------------------------
void CPhoneState::HandleChangedCallDurationL( TInt /*aCallId*/ )
    {
     
    }
    
// -----------------------------------------------------------
// CPhoneState::HandleDisconnectingL
// -----------------------------------------------------------
//
void CPhoneState::HandleDisconnectingL( TInt /*aCallId*/ )
    {
   
    }


// <------------------------------- KEY EVENTS ------------------------------->

// -----------------------------------------------------------
// CPhoneState::HandleKeyMessageL( aMessage, aKeyCode )
// -----------------------------------------------------------
//
void CPhoneState::HandleKeyMessageL(
    TPhoneKeyEventMessages /*aMessage*/,
    TKeyCode /*aScanCode*/ )
    {
    
    }

// -----------------------------------------------------------
// CPhoneState::HandleKeyEventL( aKeyEvent, aEventCode )
// -----------------------------------------------------------
//
void CPhoneState::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aEventCode*/ )
    {
    }

// -----------------------------------------------------------
// CPhoneState::HandleNumberEntryClearedL
// -----------------------------------------------------------
//
void CPhoneState::HandleNumberEntryClearedL()
    {
    }

// -----------------------------------------------------------
// CPhoneState::SendKeyEventL
// -----------------------------------------------------------
//
void CPhoneState::SendKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aEventCode*/ )
    {
    }

// -----------------------------------------------------------
// CPhoneState::SendKeyPressL
// -----------------------------------------------------------
//
void CPhoneState::SendKeyPressL(
    const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aEventCode*/ )
    {
    }

// <------------------------------ SYSTEM EVENTS ----------------------------->


// -----------------------------------------------------------
// CPhoneState::HandlePhoneStartupL
// -----------------------------------------------------------
//
void CPhoneState::HandlePhoneStartupL()
    {
   
    }

// -----------------------------------------------------------
// CPhoneState::HandlePropertyChangedL
// -----------------------------------------------------------
//
void CPhoneState::HandlePropertyChangedL( 
    const TUid& /*aCategory*/,
    const TUint /*aKey*/,
    const TInt /*aValue*/ )
    {
    
    }


// <---------------------------- MENU AND CBA EVENTS ------------------------->

TBool CPhoneState::HandleCommandL( TInt /*aCommand*/ )
    {
    
    }

// <-------------------------- REMOTE CONTROL EVENTS ------------------------->

// -----------------------------------------------------------------------------
// CPhoneState::HandleRemConCommandL
// -----------------------------------------------------------------------------
//
TBool CPhoneState::HandleRemConCommandL(
    TRemConCoreApiOperationId /*aOperationId*/, 
    TRemConCoreApiButtonAction /*aButtonAct*/ )
    {
   
    }

// -----------------------------------------------------------
// CPhoneState::DecreaseAudioVolume()
// -----------------------------------------------------------
//
void CPhoneState::DecreaseAudioVolumeL()
    {
   
    }

// -----------------------------------------------------------
// CPhoneState::IncreaceAudioVolume()
// -----------------------------------------------------------
//
void CPhoneState::IncreaseAudioVolumeL()
    {
    }        

// <-------------------------- COMMON STATE FUNCTIONS ------------------------>

// -----------------------------------------------------------
// CPhoneState::DialMultimediaCallL
// -----------------------------------------------------------
//
void CPhoneState::DialMultimediaCallL()
    {
    }

// -----------------------------------------------------------
// CPhoneState::DialVoiceCallL
// -----------------------------------------------------------
//
void CPhoneState::DialVoiceCallL()
    {
    }

// -----------------------------------------------------------
// CPhoneState::DisconnectCallL
// -----------------------------------------------------------
//
TBool CPhoneState::DisconnectCallL()
    {
    }


// -----------------------------------------------------------
// CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL
// -----------------------------------------------------------
//
void CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL()
    {
    }


// -----------------------------------------------------------
// CPhoneState::UpdateSingleActiveCallL
// -----------------------------------------------------------
//
void CPhoneState::UpdateSingleActiveCallL( TInt /*aCallId*/ )
    {
    
    }


// -----------------------------------------------------------
// CPhoneState::UpdateCallHeader
// -----------------------------------------------------------
//
void CPhoneState::UpdateCallHeader( TInt /*aCallId*/ )
    {
    }

// -----------------------------------------------------------
// CPhoneState::UpdateCallHeaderAndUiCommandsL
// -----------------------------------------------------------
//
void CPhoneState::UpdateCallHeaderAndUiCommandsL( TInt /*aCallId*/ )
    {
    }

// -----------------------------------------------------------
// CPhoneState::ShowNoteL
// -----------------------------------------------------------
//
void CPhoneState::ShowNoteL( TInt /*aResourceId*/ )
    {
   
    }

// -----------------------------------------------------------
// CPhoneState::ShowQueryL
// -----------------------------------------------------------
//
void CPhoneState::ShowQueryL( TInt /*aResourceId*/ )
    {
   
    }

// -----------------------------------------------------------
// CPhoneState::ShowTextQueryL
// -----------------------------------------------------------
//
void CPhoneState::ShowTextQueryL(
    TInt /*aDialogResourceId*/,
    TInt /*aDefaultCbaResourceId*/,
    TInt /*aContentCbaResourceId*/,
    TDes* /*aDataText*/,
    TBool /*aSendKeyEnabled*/ )
    {
  
    }
    
// -----------------------------------------------------------
// CPhoneState::DisplayCallTerminationNoteL
// -----------------------------------------------------------
//
void CPhoneState::DisplayCallTerminationNoteL()
    {
    }
    

// ---------------------------------------------------------
// CPhoneState::SetHandsfreeModeL
// ---------------------------------------------------------
//
void CPhoneState::SetHandsfreeModeL( TBool /*aHandsfreeMode*/ )
    {
  
    }

// ---------------------------------------------------------
// CPhoneState::SetBTHandsfreeModeL
// ---------------------------------------------------------
//
void CPhoneState::SetBTHandsfreeModeL( TBool /*aHandsfreeMode*/ )
    {
    
    }

// <-------------------------- INTERNAL FUNCTIONS ------------------------>

// -----------------------------------------------------------
// CPhoneState::HandleInitiatedEmergencyCallL
// Default handling for EPEMessageInitiatedEmergencyCallL message
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneState::HandleInitiatedEmergencyCallL( TInt /*aCallId*/ )
    {
    
    }

void CPhoneState::HandleCallSecureStatusChangeL( TInt /*aCallId*/ )
    {
   
    }

// ---------------------------------------------------------
// CPhoneState::IsVideoCall()
// ---------------------------------------------------------
//
TBool CPhoneState::IsVideoCall( const TInt /*aCallId*/ )
    {
    
    }

// ---------------------------------------------------------
// CPhoneState::SimState()
// ---------------------------------------------------------
//
TPESimState CPhoneState::SimState() const
    {
  
    }

// ---------------------------------------------------------
// CPhoneState::IsSimOk()
// ---------------------------------------------------------
//
TBool CPhoneState::IsSimOk()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneState::StartShowSecurityNoteL
// -----------------------------------------------------------------------------
//
void CPhoneState::StartShowSecurityNoteL()
    {
    }

// ---------------------------------------------------------
// CPhoneState::ShowWlanMacAddressL
// ---------------------------------------------------------
//
void CPhoneState::ShowWlanMacAddressL()
    {
   
    }

// -----------------------------------------------------------
// CPhoneState::HandleAudioAvailableOutputChangedL
// Default handling for EPEMessageAvailableAudioOutputsChanged message 
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneState::HandleAudioAvailableOutputChangedL()
    {
    
    }
  

// ---------------------------------------------------------                   
// CPhoneState::ShowNumberBusyNoteL
// ---------------------------------------------------------
//
void CPhoneState::ShowNumberBusyNoteL()
    {
   
    }

// ---------------------------------------------------------
// CPhoneState::IsAutoLockOn
// ---------------------------------------------------------
//
TBool CPhoneState::IsAutoLockOn() const
    { 
  
    }
    

// ---------------------------------------------------------
// CPhoneState::DeleteTouchPaneButtons
// ---------------------------------------------------------
//  
void CPhoneState::DeleteTouchPaneButtons()
    {
  
    }
    
// ---------------------------------------------------------
// CPhoneState::IsCustomizedDialerVisibleL
// ---------------------------------------------------------
//
TBool CPhoneState::IsCustomizedDialerVisibleL() const
    {
    
    }

        
// -----------------------------------------------------------
// CPhoneState::BeginUiUpdateLC
// -----------------------------------------------------------
//    
void CPhoneState::BeginUiUpdateLC()
    {
    }

// -----------------------------------------------------------
// CPhoneState::EndUiUpdate
// -----------------------------------------------------------
//
void CPhoneState::EndUiUpdate()
    {
    }

// -----------------------------------------------------------
// CPhoneState::CheckIfShowTerminationNote
// This method is intended to be overridden in states
// that contain more info about decision.
// -----------------------------------------------------------
//
TBool CPhoneState::CheckIfShowCallTerminationNote( )
    {
    }

// -----------------------------------------------------------
// CPhoneState::UiUpdateCleanup
// -----------------------------------------------------------
//
void CPhoneState::UiUpdateCleanup(TAny* /*aThis*/ )
    {
    
    }
    
// -----------------------------------------------------------------------------
// CPhoneState::SetCallerImage
// -----------------------------------------------------------------------------

// -----------------------------------------------------------
// CPhoneState::SetDefaultFlagsL()
// -----------------------------------------------------------
//
void CPhoneState::SetDefaultFlagsL()
    {    
    }

// -----------------------------------------------------------------------------
// CPhoneState::SimSecurityStatus
// -----------------------------------------------------------------------------
//
TInt CPhoneState::SimSecurityStatus() const  
    {
   
    }   

    
// ----------------------------------------------------------------------------
// CPhoneState::SetRingingTonePlaybackL()
// ----------------------------------------------------------------------------
//
void CPhoneState::SetRingingTonePlaybackL( TInt /*aCallId*/ )
    {
    
    }          

// -----------------------------------------------------------
// CPhoneState::HandleCugNoteL
// -----------------------------------------------------------
//
void CPhoneState::HandleCugInUseNoteL()
    {
    
    }

// -----------------------------------------------------------
// CPhoneState::IsWaitingCallL
// -----------------------------------------------------------
//
TBool CPhoneState::IsWaitingCallL( const TInt /*aCallId*/  )
    {
  
    }

// ---------------------------------------------------------
// CPhoneState::HandleDtmfKeyToneL
// ---------------------------------------------------------
//
void CPhoneState::HandleDtmfKeyToneL( const TKeyEvent&, TEventCode)
    {
    
    }

TBool CPhoneState::ProcessCommandL( TInt aCommand )
    {
    }


// ---------------------------------------------------------
// CPhoneState::SetToolbarButtonLoudspeakerEnabled
// ---------------------------------------------------------
//
void CPhoneState::SetToolbarButtonLoudspeakerEnabled()
    {
    }

// -----------------------------------------------------------------------------
// CPhoneState::OpenSoftRejectMessageEditorL
// -----------------------------------------------------------------------------
//
void CPhoneState::OpenSoftRejectMessageEditorL(  )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneState::SetBackButtonActive
// -----------------------------------------------------------------------------
//
void CPhoneState::SetBackButtonActive( 
        TBool aActive )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneState::SendGlobalInfoNoteL
// -----------------------------------------------------------------------------
//
void CPhoneState::SendGlobalInfoNoteL( 
        TInt aResourceId,
        TBool aNotificationDialog )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneState::SendGlobalErrorNoteL
// -----------------------------------------------------------------------------
//
void CPhoneState::SendGlobalErrorNoteL( 
        TInt aResourceId,
        TBool aNotificationDialog )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneState::SendGlobalWarningNoteL
// -----------------------------------------------------------------------------
//
void CPhoneState::SendGlobalWarningNoteL( 
        TInt aResourceId,
        TBool aNotificationDialog )
    {
    }


//  End of File  

