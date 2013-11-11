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
* Description: Implementation of CPhoneConferenceAndSingleAndWaiting class.
*
*/


// INCLUDES
#include <StringLoader.h>
#include <cpephonemodelif.h>
#include "cphoneconferenceandsingleandwaiting.h"
#include "mphonestatemachine.h"
#include "phoneviewcommanddefinitions.h"
#include "tphonecmdparamcallheaderdata.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamcallstatedata.h"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "tphonecmdparamglobalnote.h"
#include "phoneui.hrh"
#include "mphonestorage.h"
#include "cphonecenrepproxy.h"
#include <telephonyvariant.hrh>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneConferenceAndSingleAndWaiting::CPhoneConferenceAndSingleAndWaiting(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) :
    CPhoneConferenceAndSingle( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::~CPhoneConferenceAndSingleAndWaiting()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneConferenceAndSingleAndWaiting::~CPhoneConferenceAndSingleAndWaiting()
    {
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::ConstructL()
    {
    CPhoneConferenceAndSingle::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneConferenceAndSingleAndWaiting* CPhoneConferenceAndSingleAndWaiting::NewL(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneConferenceAndSingleAndWaiting* self = new( ELeave ) CPhoneConferenceAndSingleAndWaiting(
        aStateMachine, aViewCommandHandle, aPhoneCustomization );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::HandlePhoneEngineMessageL(
    const TInt aMessage,
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneConferenceAndSingleAndWaiting::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageAddedConferenceMember:
            HandleAddedConferenceMemberL( aCallId );
            break;

        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;

        default:
            CPhoneConferenceAndSingle::HandlePhoneEngineMessageL( aMessage,
                aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneConferenceAndSingleAndWaiting::HandleIdleL()");
    BeginUiUpdateLC();
    UpdateUiCommands();

    TPhoneCmdParamBoolean conferenceExistsForCallId;
    iViewCommandHandle->ExecuteCommandL( EPhoneViewGetCallExistsInConference,
        aCallId, &conferenceExistsForCallId );

    if( conferenceExistsForCallId.Boolean() )
        {
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveFromConference,
            aCallId );
        }
    else
        {
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );

        TPhoneCmdParamBoolean conferenceBubbleExists;
        iViewCommandHandle->ExecuteCommandL( EPhoneViewGetIsConference,
            &conferenceBubbleExists );
        if( !conferenceBubbleExists.Boolean() )
            {
            // Idle message came for conference member.
            MakeTransitionAccordingToActiveCallsL();
            }
        else
            {
            TPhoneCmdParamCallStateData callStateData;
            callStateData.SetCallState( EPEStateRinging );
            iViewCommandHandle->HandleCommandL( EPhoneViewGetCallIdByState,
                &callStateData );
            if( callStateData.CallId() >= 0 )
                {
                // We have Conference and Waiting calls left
                MakeStateTransitionToConferenceAndWaitingL( aCallId );
                }
            else
                {
                // We have Conference and Single calls left
                MakeStateTransitionToConferenceAndSingleL( aCallId );
                }
            }
        }
    EndUiUpdate();
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::HandleAddedConferenceMemberL
// -----------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::HandleAddedConferenceMemberL( TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneConferenceAndSingleAndWaiting::HandleAddedConferenceMemberL");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewAddToConference );
    UpdateConferenceSecurityStatusL();
    MakeStateTransitionToConferenceAndWaitingL( KConferenceCallId );
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::MakeStateTransitionToConferenceAndWaitingL
// -----------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::MakeStateTransitionToConferenceAndWaitingL( TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneConferenceAndSingleAndWaiting::MakeStateTransitionToConferenceAndWaitingL()");
    UpdateUiCommands();
    iStateMachine->ChangeState( EPhoneStateConferenceAndWaiting );
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::MakeStateTransitionToConferenceAndSingleL
// -----------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::MakeStateTransitionToConferenceAndSingleL( 
        TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneConferenceAndSingleAndWaiting::MakeStateTransitionToConferenceAndSingleL()");
    iNumberEntryManager->SetVisibilityIfNumberEntryUsedL( ETrue );
    UpdateUiCommands();
    iStateMachine->ChangeState( EPhoneStateConferenceAndSingle );
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::HandleConferenceIdleL
// -----------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::HandleConferenceIdleL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneConferenceAndSingleAndWaiting::HandleConferenceIdleL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveConferenceBubble );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemovePhoneDialogs );
    MakeTransitionAccordingToActiveCallsL();
    EndUiUpdate();
    }

// --------------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::HandleKeyMessageL
// --------------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::HandleKeyMessageL(
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneConferenceAndSingleAndWaiting::HandleKeyMessageL()");
    switch ( aCode )
        {
        case EKeyYes: // send-key
            if( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                iNumberEntryManager->CallFromNumberEntryL();
                }
            else
                {
                SendGlobalWarningNoteL(
                    EPhoneNoteTextCallNotAllowed, ETrue );
                }
            break;

        default:
            CPhoneConferenceAndSingle::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneConferenceAndSingleAndWaiting::MakeTransitionAccordingToActiveCallsL
// -----------------------------------------------------------
//
void CPhoneConferenceAndSingleAndWaiting::MakeTransitionAccordingToActiveCallsL()
    {
    TPhoneCmdParamInteger activeCallCount;
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewGetCountOfActiveCalls, &activeCallCount );
    switch( activeCallCount.Integer() )
         {
         case ENoActiveCalls: // Go to incoming state
            {
            TPhoneCmdParamCallStateData callStateData;
            callStateData.SetCallState( EPEStateRinging );
            iViewCommandHandle->HandleCommandL( EPhoneViewGetCallIdByState,
                &callStateData );
            if ( callStateData.CallId() != KErrNotFound )
                {
                UpdateCallHeaderAndUiCommandsL( callStateData.CallId() );
                SetRingingTonePlaybackL( callStateData.CallId() );
                SetBackButtonActive(EFalse);
                iStateMachine->ChangeState( EPhoneStateIncoming );
                }
            else
                {
                SetDefaultFlagsL();
                iStateMachine->ChangeState( EPhoneStateIdle );
                }
            }
            break;
         case EOneActiveCall: // Go to Single And Waiting state
            {
            UpdateUiCommands();
            iStateMachine->ChangeState( EPhoneStateWaitingInSingle );
            }
            break;
         case ETwoActiveCalls: // Go to Two Singles And Waiting state
            {
            UpdateUiCommands();
            iStateMachine->ChangeState( EPhoneStateTwoSinglesAndWaiting );
            }
            break;
         default:
           // Too many active calls to handle reliable state change now.
           // Waiting for all conference member idles
           // This case may happen when 'End all calls' because conferenceIdle may
           // come earlier than idle for conference member
            break;
         }

    }

// End of File
