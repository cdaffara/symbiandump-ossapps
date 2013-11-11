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
* Description: Implementation of CPhoneConference class.
*
*/


// INCLUDES
#include <eikmenub.h> 
#include <eikenv.h>
#include <StringLoader.h>
#include <pevirtualengine.h>
#include <mpeengineinfo.h>
#include <cpephonemodelif.h>
#include <featmgr.h>
#include "cphoneconference.h"
#include "mphonestatemachine.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamkeyevent.h"
#include "phonestatedefinitions.h"
#include "tphonecmdparamglobalnote.h"
#include "phonestatedefinitionsgsm.h"
#include "tphonecmdparamcallstatedata.h"
#include "cphonekeys.h"
#include "phonelogger.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneConference::CPhoneConference( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneGsmInCall( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneConference::~CPhoneConference()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneConference::~CPhoneConference()
    {
    }

// -----------------------------------------------------------
// CPhoneConference::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneConference::ConstructL()
    {
    CPhoneGsmInCall::ConstructL();
    
    // Re-enable global notes
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
    
    }

// -----------------------------------------------------------
// CPhoneConference::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneConference* CPhoneConference::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneConference* self = new( ELeave ) CPhoneConference( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneConference::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneConference::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;
        
        case MEngineMonitor::EPEMessageConferenceIdle:
            HandleConferenceIdleL();
            break;
            
        case MEngineMonitor::EPEMessageHeldConference:
        case MEngineMonitor::EPEMessageConnectedConference:
            UpdateCallHeaderAndUiCommandsL( aCallId );
            break;

        case MEngineMonitor::EPEMessageAddedConferenceMember:
            UpdateConferenceSecurityStatusL();
            break;
            
        case MEngineMonitor::EPEMessageWentOneToOne:
            HandleWentOneToOneL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageHeld:
        case MEngineMonitor::EPEMessageConnected:
            {
            UpdateCallHeader( aCallId );
            }
            break;
            
        case MEngineMonitor::EPEMessageDialing:
            HandleDialingL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageIncoming:
            HandleIncomingL( aCallId );
            break;

        default:
            CPhoneGsmInCall::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneConference:HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneConference::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleCommandL()");
    TBool commandStatus = ETrue;
    switch( aCommand )
        {   
        case EAknSoftkeyCancel:
            BeginUiUpdateLC();
            UpdateUiCommands();
            EndUiUpdate();
            break;
    
        case EPhoneInCallCmdHold:
        case EPhoneInCallCmdConferenceHold:
            iStateMachine->SendPhoneEngineMessage( 
                CPEPhoneModelIF::EPEMessageHoldConference );
            break;
            
        case EPhoneInCallCmdUnhold:
        case EPhoneInCallCmdConferenceUnhold:
            iStateMachine->SendPhoneEngineMessage( 
                CPEPhoneModelIF::EPEMessageResumeConference );
            break;
            
        case EPhoneInCallCmdDrop:
            DropSelectedParticipantL();
            break;
            
        case EPhoneInCallCmdPrivate:
            PrivateSelectedParticipantL();
            break;
            
        case EPhoneInCallCmdPrivateSelection:
        case EPhoneInCallCmdDropSelection:
        case EPhoneInCallCmdNewCall:
        case EPhoneInCallCmdParticipants:
            break;

        default:
            commandStatus = CPhoneGsmInCall::HandleCommandL( aCommand );
            break;
        }
    return commandStatus;
    }

// -----------------------------------------------------------
// CPhoneConference::HandleKeyEventL
// -----------------------------------------------------------
//
void CPhoneConference::HandleKeyEventL( 
    const TKeyEvent& aKeyEvent, 
    TEventCode aEventCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleKeyEventL()");
    if ( !CPhoneKeys::IsNumericKey( aKeyEvent, aEventCode ) )
        {
        // Handle other key events than numeric key events.
        CPhoneState::HandleKeyEventL( aKeyEvent, aEventCode );
        }
    }

// --------------------------------------------------------------
// CPhoneConference::HandleKeyMessageL
// --------------------------------------------------------------
//
void CPhoneConference::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleKeyMessageL()");
    switch ( aCode )
        {
        case EKeyYes: // send-key
            if( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                iNumberEntryManager->CallFromNumberEntryL(); 
                }
            else
                {
                ToggleHoldL();    
                }
            break;
            
        default:
            CPhoneGsmInCall::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneConference::HandleNumberEntryClearedL
// -----------------------------------------------------------
//
void CPhoneConference::HandleNumberEntryClearedL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleNumberEntryClearedL()");
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneConference:HandleIdleL
// -----------------------------------------------------------
//
void CPhoneConference::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleIdleL()");
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
        
    // If conference bubble still exists then we have conference and
    // single call
    TPhoneCmdParamBoolean conferenceBubbleExists;
    iViewCommandHandle->ExecuteCommandL( EPhoneViewGetIsConference, 
        &conferenceBubbleExists ); 
        
    if( conferenceBubbleExists.Boolean() )
        {
        iViewCommandHandle->ExecuteCommandL(
            EPhoneViewRemoveFromConference, aCallId ); 
        }
    else
        {
        iViewCommandHandle->ExecuteCommandL( 
            EPhoneViewRemoveCallHeader, aCallId );
        }
    }
    
// -----------------------------------------------------------
// CPhoneConference:HandleConferenceIdleL
// -----------------------------------------------------------
//
void CPhoneConference::HandleConferenceIdleL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleConferenceIdleL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveConferenceBubble );

    TPhoneCmdParamInteger activeCallCount;
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewGetCountOfActiveCalls, &activeCallCount );

    switch( activeCallCount.Integer() )
        {
        case ENoActiveCalls:
            MakeStateTransitionToIdleL();
            break;
            
        case EOneActiveCall:
            {
            TPhoneCmdParamCallStateData callStateData;
            callStateData.SetCallState( EPEStateRinging );
            iViewCommandHandle->HandleCommandL(
                EPhoneViewGetCallIdByState, &callStateData );
                
            if( callStateData.CallId() > KErrNotFound )
                {
                UpdateUiCommands();
                iStateMachine->ChangeState( EPhoneStateWaitingInSingle );    
                }
            else
                {
                MakeStateTransitionToSingleL();
                }
            }
            break;
            
        case ETwoActiveCalls:
            MakeStateTransitionToTwoSinglesL();
            break;
            
        default:
            MakeStateTransitionToTwoSinglesL();
            break;
        }
    EndUiUpdate(); 
    }

// -----------------------------------------------------------
// CPhoneConference::DropSelectedParticipantL
// -----------------------------------------------------------
//
void CPhoneConference::DropSelectedParticipantL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::DropSelectedParticipantL()");
    TPhoneViewResponseId response;
    TPhoneCmdParamInteger callId;
    response = iViewCommandHandle->HandleCommandL(
        EPhoneViewSelectedConfMember, &callId );    
        
    if( response == EPhoneViewResponseSuccess )
        {
        iStateMachine->SetCallId( callId.Integer() );    
        iStateMachine->SendPhoneEngineMessage( 
            CPEPhoneModelIF::EPEMessageDropConferenceMember );
        }
    }

// -----------------------------------------------------------
// CPhoneConference::PrivateSelectedParticipantL
// -----------------------------------------------------------
//
void CPhoneConference::PrivateSelectedParticipantL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::PrivateSelectedParticipantL()");
    TPhoneViewResponseId response;
    TPhoneCmdParamInteger callId;
    response = iViewCommandHandle->HandleCommandL(
        EPhoneViewSelectedConfMember, &callId );
    
    if( response == EPhoneViewResponseSuccess )
        {
        iStateMachine->SetCallId( callId.Integer() );    
        iStateMachine->SendPhoneEngineMessage( 
            CPEPhoneModelIF::EPEMessageGoOneToOne );
        BeginUiUpdateLC();
        UpdateUiCommands();
        EndUiUpdate();
        }
    }

// -----------------------------------------------------------
// CPhoneConference::MakeStateTransitionToIdleL
// -----------------------------------------------------------
//
void CPhoneConference::MakeStateTransitionToIdleL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::MakeStateTransitionToIdleL()");
    SetDefaultFlagsL();

    if ( iNumberEntryManager->IsNumberEntryUsedL() )
        {
        iNumberEntryManager->SetNumberEntryVisibilityL(ETrue);
        }
    else
        {
        RemoveDialogsAndSendPhoneToBackgroundL();
        }
    DisplayCallTerminationNoteL();
    UpdateUiCommands();
    iStateMachine->ChangeState( EPhoneStateIdle );
    }

// -----------------------------------------------------------
// CPhoneConference::MakeStateTransitionToSingleL
// -----------------------------------------------------------
//
void CPhoneConference::MakeStateTransitionToSingleL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::MakeStateTransitionToSingleL()");
    iNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue);
    UpdateUiCommands();
    iStateMachine->ChangeState( EPhoneStateSingle );
    }

// -----------------------------------------------------------
// CPhoneConference::MakeTransitionToTwoSinglesL
// -----------------------------------------------------------
//
void CPhoneConference::MakeStateTransitionToTwoSinglesL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::MakeStateTransitionToTwoSinglesL()");
    iNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue);
    UpdateUiCommands();
    iStateMachine->ChangeState( EPhoneStateTwoSingles );
    }

// -----------------------------------------------------------
// CPhoneConference::ToggleHold
// -----------------------------------------------------------
//
void CPhoneConference::ToggleHoldL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::ToggleHoldL()");
    iStateMachine->SetCallId( KConferenceCallId );
    iStateMachine->SendPhoneEngineMessage( 
        CPEPhoneModelIF::EPEMessageSwapConference );
    }

// -----------------------------------------------------------
// CPhoneConference::HandleDialingL
// -----------------------------------------------------------
//
void CPhoneConference::HandleDialingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleDialingL()");
    DisplayCallHeaderL( aCallId, ESetNEVisibilityFalse);
    iStateMachine->ChangeState( EPhoneStateConferenceAndCallSetup );
    }

// -----------------------------------------------------------
// CPhoneConference::HandleIncomingL
// -----------------------------------------------------------
//
void CPhoneConference::HandleIncomingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleIncomingL()");
    DisplayCallHeaderL( aCallId, ECheckIfNEUsedBeforeSettingVisibilityFalse );
    iStateMachine->ChangeState( EPhoneStateConferenceAndWaiting );
    }

// -----------------------------------------------------------
// CPhoneConference::HandleWentOneToOneL
// -----------------------------------------------------------
//
void CPhoneConference::HandleWentOneToOneL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConference::HandleWentOneToOneL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewPrivateFromConference, aCallId );
        
    // If conference bubble still exists then we have conference and
    // single call
    TPhoneCmdParamBoolean conferenceBubbleExists;
    iViewCommandHandle->ExecuteCommandL( EPhoneViewGetIsConference, 
        &conferenceBubbleExists ); 
    if( conferenceBubbleExists.Boolean() )
        {
        iStateMachine->ChangeState( EPhoneStateConferenceAndSingle );
        }
    
    UpdateUiCommands();
    EndUiUpdate();
    }
    
// -----------------------------------------------------------
// CPhoneConference::UpdateConferenceSecurityStatusL
// -----------------------------------------------------------
//
void CPhoneConference::UpdateConferenceSecurityStatusL( )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
            "CPhoneConference::UpdateConferenceSecurityStatusL()");
    iViewCommandHandle->ExecuteCommandL( 
        EPhoneViewCipheringInfoChange,
        KPEConferenceCallID);
    }

// End of File
