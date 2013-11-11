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
* Description: Implementation of CPhoneSingleAndWaiting class.
*
*/


// INCLUDES
#include <eikmenub.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <pevirtualengine.h>
#include <mpeengineinfo.h>
#include <telephonyvariant.hrh>
#include <cpephonemodelif.h>
#include "cphonesingleandwaiting.h"
#include "mphonestatemachine.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "cphonegsmincall.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamcallstatedata.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"
#include "cphonecenrepproxy.h"
#include "cphonepubsubproxy.h"
#include "mphonestorage.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneSingleAndWaiting::CPhoneSingleAndWaiting(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) :
    CPhoneGsmInCall( aStateMachine, aViewCommandHandle, aPhoneCustomization ),
    iSingleCallId ( KErrNotFound )
    {
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::~CPhoneSingleAndWaiting()
// -----------------------------------------------------------
//
CPhoneSingleAndWaiting::~CPhoneSingleAndWaiting()
    {
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::ConstructL()
// -----------------------------------------------------------
//
void CPhoneSingleAndWaiting::ConstructL()
    {
    CPhoneGsmInCall::ConstructL();
    
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateConnected );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );

    if ( callStateData.CallId() == KErrNotFound )
        {
        callStateData.SetCallState( EPEStateHeld );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );
        }
    iSingleCallId = callStateData.CallId();
    __PHONELOG1( EBasic, EPhoneUIStates,
        "CPhoneSingleAndWaiting::ConstructL()  (iSingleCallId=%d)", iSingleCallId);
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::NewL()
// -----------------------------------------------------------
//
CPhoneSingleAndWaiting* CPhoneSingleAndWaiting::NewL(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneSingleAndWaiting* self = new( ELeave ) CPhoneSingleAndWaiting(
        aStateMachine, aViewCommandHandle, aPhoneCustomization );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneSingleAndWaiting::HandlePhoneEngineMessageL(
    const TInt aMessage,
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneSingleAndWaiting::HandlePhoneEngineMessageL() ");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;

        case MEngineMonitor::EPEMessageHeld:
            UpdateCallHeaderAndUiCommandsL( aCallId );
            break;

        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;

        case MEngineMonitor::EPEMessageDialing:
            HandleDialingL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageDisconnecting:
            HandleDisconnectingL( aCallId );
            /* Flow through */

        default:
            CPhoneGsmInCall::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting:HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneSingleAndWaiting::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneSingleAndWaiting::HandleCommandL() ");
    TBool commandStatus = ETrue;
    switch( aCommand )
        {
        case EPhoneInCallCmdHold:
            {
            iStateMachine->SetCallId( iSingleCallId );
            iStateMachine->SendPhoneEngineMessage(
                CPEPhoneModelIF::EPEMessageHold );
            break;
            }

        case EPhoneInCallCmdUnhold:
            {
            iStateMachine->SetCallId( iSingleCallId );
            iStateMachine->SendPhoneEngineMessage(
                CPEPhoneModelIF::EPEMessageResume );
            break;
            }

        case EPhoneInCallCmdNewCall:
            break;

        case EPhoneCallComingCmdReject: // fall through
            {
            // Get waiting callid
            TPhoneCmdParamCallStateData callStateData;
            callStateData.SetCallState( EPEStateRinging );
            iViewCommandHandle->HandleCommandL( EPhoneViewGetCallIdByState,
                &callStateData );
            iStateMachine->SetCallId( callStateData.CallId() );
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageReject );
            break;
            }

        case EPhoneInCallCmdReject:
            iStateMachine->SetCallId( iSingleCallId );
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageReject );
            break;

        case EPhoneCmdUpdateUiControls:
            UpdateUiCommands();
            break;

        default:
            commandStatus = CPhoneGsmInCall::HandleCommandL( aCommand );
            break;
        }
    return commandStatus;
    }

// --------------------------------------------------------------
// CPhoneSingleAndWaiting::HandleKeyMessageL
// --------------------------------------------------------------
//
void CPhoneSingleAndWaiting::HandleKeyMessageL(
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneSingleAndWaiting::HandleKeyMessageL() ");
    switch ( aCode )
        {
        case EKeyYes: // send-key
            {
            if( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                iNumberEntryManager->CallFromNumberEntryL();
                }
             else
                {
                // Close DTMF sending note if it exists
                iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNote );
                iStateMachine->SendPhoneEngineMessage(
                    CPEPhoneModelIF::EPEMessageAnswer );
                }
            break;
            }

        default:
            CPhoneGsmInCall::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneSingleAndWaiting::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneSingleAndWaiting::HandleIdleL() ");

    if ( iSingleCallId == KErrNotFound )
        {
        TPhoneCmdParamCallStateData callStateData;
        callStateData.SetCallState( EPEStateDisconnecting );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );

        if ( callStateData.CallId() == KErrNotFound )
            {
            callStateData.SetCallState( EPEStateConnected );
            iViewCommandHandle->HandleCommandL(
                EPhoneViewGetCallIdByState, &callStateData );

            if ( callStateData.CallId() == KErrNotFound )
                {
                callStateData.SetCallState( EPEStateHeld );
                iViewCommandHandle->HandleCommandL(
                    EPhoneViewGetCallIdByState, &callStateData );
                }
            }
        iSingleCallId = callStateData.CallId();
        }

    __PHONELOG1( EBasic, EPhoneControl, 
        "CPhoneSingleAndWaiting::HandleIdleL iSingleCallId(%d)",
        iSingleCallId );
    __PHONELOG1( EBasic, EPhoneControl, 
        "CPhoneSingleAndWaiting::HandleIdleL aCallId(%d)",
        aCallId );
    
    // Idle message came for active call
    if( iSingleCallId == aCallId )
        {
        BeginUiUpdateLC();
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );

        // Get waiting callId
        TPhoneCmdParamCallStateData callStateData;
        callStateData.SetCallState( EPEStateRinging );
        iViewCommandHandle->HandleCommandL( EPhoneViewGetCallIdByState,
            &callStateData );
        
        __PHONELOG1( EBasic, EPhoneControl, 
            "CPhoneSingleAndWaiting::HandleIdleL EPEStateRinging CallId(%d)",
            callStateData.CallId() );
        
        // Display ringing bubble
        if ( callStateData.CallId() > KErrNotFound )
            {           
            UpdateCallHeaderAndUiCommandsL( callStateData.CallId() );

            if ( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                iNumberEntryManager->SetNumberEntryVisibilityL(EFalse);
                }
            
            SetRingingTonePlaybackL( callStateData.CallId() );
            BringIncomingToForegroundL();
            }
        SetBackButtonActive(EFalse);
        EndUiUpdate();
        iStateMachine->ChangeState( EPhoneStateIncoming );
        }
    else
        {
        BeginUiUpdateLC();
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );

        // Idle message came for waiting call
        if ( iNumberEntryManager->IsNumberEntryUsedL() )
            {
            iNumberEntryManager->SetNumberEntryVisibilityL(ETrue);
            }
        
        UpdateUiCommands();
        EndUiUpdate();
        iStateMachine->ChangeState( EPhoneStateSingle );
        }

    }



// -----------------------------------------------------------
// CPhoneSingleAndWaiting::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneSingleAndWaiting::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneSingleAndWaiting::HandleConnectedL() ");
    __PHONELOG1( EBasic, EPhoneUIStates,
        "CPhoneSingleAndWaiting::HandleConnectedL  (CallId=%d)", aCallId);
    
    if ( iSingleCallId == aCallId )
        {
        // Connected message came for the hold call, we still
        // have the waiting call also
        UpdateCallHeaderAndUiCommandsL( aCallId );
        }
    else
        {
        // Connected message came for waiting call
        MakeStateTransitionToTwoSinglesL( aCallId );
        }
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::HandleUnholdL
// -----------------------------------------------------------
//
void CPhoneSingleAndWaiting::HandleUnholdL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneSingleAndWaiting::HandleUnholdL() ");
    UpdateCallHeaderAndUiCommandsL( aCallId );
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::MakeStateTransitionToTwoSinglesL
// -----------------------------------------------------------
//
void CPhoneSingleAndWaiting::MakeStateTransitionToTwoSinglesL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneSingleAndWaiting::MakeStateTransitionToTwoSinglesL() ");
    UpdateCallHeaderAndUiCommandsL( aCallId );
    
    if ( iNumberEntryManager->IsNumberEntryUsedL() )
        {
        iNumberEntryManager->SetNumberEntryVisibilityL(ETrue);
        }

    iStateMachine->ChangeState( EPhoneStateTwoSingles );
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::HandleDialingL
// -----------------------------------------------------------
//
void CPhoneSingleAndWaiting::HandleDialingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneSingleAndWaiting::HandleDialingL() ");
    DisplayCallHeaderL( aCallId, ESetNEVisibilityFalse);
    iStateMachine->ChangeState( EPhoneStateSingleAndCallSetupAndWaiting );
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::HandleErrorL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneSingleAndWaiting::HandleErrorL( const TPEErrorInfo& aErrorInfo )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndWaiting::HandleErrorL()");
    if( aErrorInfo.iErrorCode == ECCPErrorCCUserAlertingNoAnswer )
        {
        // Should not shown "Not Allowed" note, if active call.
        __PHONELOG1( EBasic, EPhoneUIStates,
            "PhoneUIControl: CPhoneSingleAndWaiting::HandleErrorL - aErrorInfo.iErrorCode =%d ",
            aErrorInfo.iErrorCode);
        }
    else
        {
        CPhoneState::HandleErrorL( aErrorInfo );
        }
    }

// -----------------------------------------------------------
// CPhoneSingleAndWaiting::HandleDisconnectingL
// -----------------------------------------------------------
//
void CPhoneSingleAndWaiting::HandleDisconnectingL( TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndWaiting::HandleDisconnectingL( ) ");
    }

// End of File
