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
* Description: Implementation of CPhoneSingleAndCallSetupAndWaiting class.
*
*/


// INCLUDES
#include <eikmenub.h> 
#include <eikenv.h>
#include <StringLoader.h>
#include <pevirtualengine.h>
#include <cpephonemodelif.h>
#include "cphonesingleandcallsetupandwaiting.h"
#include "mphonestatemachine.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "cphonestateincall.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamcallstatedata.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"

enum TTerminatedCall
{
    ENull = 0,
    EHold,
    ERinging,
    EAlerting    
};

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneSingleAndCallSetupAndWaiting::CPhoneSingleAndCallSetupAndWaiting( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneGsmInCall( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting::~CPhoneSingleAndCallSetupAndWaiting()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneSingleAndCallSetupAndWaiting::~CPhoneSingleAndCallSetupAndWaiting()
    {
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::ConstructL()
    {
    CPhoneGsmInCall::ConstructL();

    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    iWaitingCallId = callStateData.CallId();
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneSingleAndCallSetupAndWaiting* CPhoneSingleAndCallSetupAndWaiting::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneSingleAndCallSetupAndWaiting* self = new( ELeave ) CPhoneSingleAndCallSetupAndWaiting( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// --------------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting::HandleKeyMessageL
// --------------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetupAndWaiting::HandleKeyMessageL() ");
    switch ( aCode )
        {
        case EKeyYes: // send-key
            if( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                iNumberEntryManager->CallFromNumberEntryL();
                }
            else
                {
                // Number entry is behind waiting call bubble
                // We can't answer to waiting call in this state
                // so display Not allowed -note.                        
                CPhoneState::SendGlobalErrorNoteL( 
                    EPhoneNoteTextNotAllowed, ETrue );
                }
            break;
            
        case EKeyNo: // end-key
            if ( aMessage == EPhoneKeyLongPress )
                {
                iStateMachine->SendPhoneEngineMessage(
                    MPEPhoneModel::EPEMessageTerminateAllConnections );
                iNumberEntryManager->ClearNumberEntryContentCacheIfContentStored();
                if ( iNumberEntryManager->IsNumberEntryUsedL() )
                    {
                    iViewCommandHandle->ExecuteCommandL( 
                        EPhoneViewRemoveNumberEntry );
                    // Do state-specific operation when number entry is cleared
                    HandleNumberEntryClearedL();
                    }
                }
            else // handle short end key
                {
                DisconnectOutgoingCallL();
                }
            break;
            
        default:
            CPhoneGsmInCall::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetupAndWaiting::HandlePhoneEngineMessageL() ");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageConnecting:
            iAlerting = ETrue;
            HandleConnectingL( aCallId );
            break;
        
        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;
        
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;
            
        default:
            CPhoneGsmInCall::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting:HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneSingleAndCallSetupAndWaiting::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetupAndWaiting::HandleCommandL() ");
    TBool commandStatus = ETrue;
    switch( aCommand )
        {
        case EPhoneInCallCmdEndThisActiveCall:
            DisconnectOutgoingCallL();
            break;
        
        case EPhoneInCallCmdEndAllCalls:
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageReleaseAll );
            break;
            
        case EPhoneDtmfDialerCancel:
            break;
            
        default:
            commandStatus = CPhoneGsmInCall::HandleCommandL( aCommand );
            break;
        }
    return commandStatus;
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting:HandleIdleL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetupAndWaiting::HandleIdleL() ");
    // Fetch call ids for hold, ringing and alerting bubbles
    TInt holdCallId;
    TInt ringingCallId;
    TInt alertingCallId;
    TPhoneViewResponseId heldResponse;
    TPhoneViewResponseId ringingResponse;
    TPhoneViewResponseId alertingResponse;
    TPhoneCmdParamCallStateData callStateData;
    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
    
    callStateData.SetCallState( EPEStateHeld );
    heldResponse = iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    holdCallId = callStateData.CallId();
    
    callStateData.SetCallState( EPEStateRinging );
    ringingResponse = iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    ringingCallId = callStateData.CallId();
                
    callStateData.SetCallState( EPEStateConnecting );
    alertingResponse = iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    alertingCallId = callStateData.CallId();
    
    if( heldResponse != EPhoneViewResponseSuccess ||
        ringingResponse != EPhoneViewResponseSuccess ||
        alertingResponse != EPhoneViewResponseSuccess )
        {
        // Some or all call id fetches were unsuccesfull.
        // Terminate all connections and return to idle state.
        // TBD: Kill all bubbles here
        iStateMachine->SendPhoneEngineMessage(
            MPEPhoneModel::EPEMessageTerminateAllConnections );
        CPhoneGsmInCall::HandleIdleL( aCallId );
        return;
        }

    // Find out which call was terminated
    TTerminatedCall terminatedCall = ENull;
    if( holdCallId < 0 )
        {
        terminatedCall = EHold;            
        }
    else if( ringingCallId < 0 )
        {
        terminatedCall = ERinging;            
        }
    else if( alertingCallId < 0 )
        {
        terminatedCall = EAlerting;            
        }
    
    switch( terminatedCall )
        {
        case EHold:
            // Idle message came for hold call
            StateChangeToCallSetupAndWaitingL( aCallId );            
            break;
            
        case ERinging:
            // Idle message came for ringing call
            StateChangeToSingleAndAlertingL( aCallId );                    
            break;        
            
        case EAlerting:
            // Idle message came for alerting call
            StateChangeToSingleAndWaitingL( aCallId );            
            break;
            
        default:
            break;
        }

    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting:HandleConnectingL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::HandleConnectingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetupAndWaiting::HandleConnectingL() ");
    iNumberEntryManager->RemoveNumberEntryIfVisibilityIsFalseL();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveGlobalNote );
    
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );

    UpdateCallHeaderAndUiCommandsL( aCallId );
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndCallSetupAndWaiting::HandleConnectedL()");
    
    UpdateCallHeader( aCallId );
    
    iNumberEntryManager->RemoveNumberEntryIfVisibilityIsFalseL();

    
    if ( aCallId != iWaitingCallId ) // Alerting call is connected
        {
        StateChangeToTwoSinglesAndWaitingL( aCallId );
        }
    else   
        {
        // Waiting call is connected while alerting call is disconnecting
        // (manual control sequence '1' + Send). 
        StateChangeToTwoSinglesL( aCallId );
        }
    }
    
// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting:StateChangeToCallSetupAndWaitingL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::StateChangeToCallSetupAndWaitingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetupAndWaiting::StateChangeToCallSetupAndWaitingL() ");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );  

    BeginUiUpdateLC();
    
    UpdateUiCommands();
    SetToolbarButtonLoudspeakerEnabled();
    
    EndUiUpdate();
    iStateMachine->ChangeState( EPhoneStateCallSetupAndWaiting );
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting:StateChangeToSingleAndWaitingL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::StateChangeToSingleAndWaitingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetupAndWaiting::StateChangeToSingleAndWaitingL() ");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
    if ( iNumberEntryManager->IsNumberEntryUsedL() )
        {
        TPhoneCmdParamBoolean booleanParam;
        booleanParam.SetBoolean( ETrue );
        iViewCommandHandle->ExecuteCommandL( EPhoneViewSetNumberEntryVisible, &booleanParam );
        }

    BeginUiUpdateLC();
    
    UpdateUiCommands();
    
    EndUiUpdate();
 
    iStateMachine->ChangeState( EPhoneStateWaitingInSingle );
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting:StateChangeToSingleAndAlertingL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::StateChangeToSingleAndAlertingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetupAndWaiting::StateChangeToSingleAndAlertingL() ");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
    iNumberEntryManager->RemoveNumberEntryIfVisibilityIsFalseL();
    BeginUiUpdateLC();
    UpdateUiCommands();
    EndUiUpdate();

    iStateMachine->ChangeState( EPhoneStateAlertingInSingle );        
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting::StateChangeToTwoSinglesL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::StateChangeToTwoSinglesL( TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndCallSetupAndWaiting::StateChangeToTwoSinglesL()");
    BeginUiUpdateLC();
    UpdateUiCommands();
    EndUiUpdate();

    iStateMachine->ChangeState( EPhoneStateTwoSingles );
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetupAndWaiting::StateChangeToTwoSinglesAndWaitingL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetupAndWaiting::StateChangeToTwoSinglesAndWaitingL( TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndCallSetupAndWaiting::StateChangeToTwoSinglesAndWaitingL()");
    BeginUiUpdateLC();
    UpdateUiCommands();
    EndUiUpdate();

    iStateMachine->ChangeState( EPhoneStateTwoSinglesAndWaiting );
    }


// End of File
