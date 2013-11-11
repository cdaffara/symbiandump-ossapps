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
* Description: Implementation of CPhoneConferenceAndWaiting class.
*
*/


// INCLUDES
#include <StringLoader.h>
#include <cpephonemodelif.h>
#include <featmgr.h>
#include <telephonyvariant.hrh>
#include "cphoneconferenceandwaiting.h"
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
#include "cphonecenrepproxy.h"
#include "mphonestorage.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneConferenceAndWaiting::CPhoneConferenceAndWaiting( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneConference( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::~CPhoneConferenceAndWaiting()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneConferenceAndWaiting::~CPhoneConferenceAndWaiting()
    {
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::ConstructL()
    {
    CPhoneConference::ConstructL();
    
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    iRingingCallId = callStateData.CallId();
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneConferenceAndWaiting* CPhoneConferenceAndWaiting::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneConferenceAndWaiting* self = new( ELeave ) CPhoneConferenceAndWaiting( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneConferenceAndWaiting::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageDialing:
            HandleDialingL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageConferenceIdle:
            HandleConferenceIdleL();
            break;
            
        case MEngineMonitor::EPEMessageWentOneToOne:
            HandleWentOneToOneL( aCallId );
            break;

        case MEngineMonitor::EPEMessageHeldConference:            
        case MEngineMonitor::EPEMessageConnectedConference:
            {
            TPhoneCmdParamInteger callIdParam;
            iViewCommandHandle->ExecuteCommand( 
                        EPhoneViewGetExpandedBubbleCallId, &callIdParam );
            CPhoneConference::HandlePhoneEngineMessageL( aMessage, 
                            aCallId );
            if ( KConferenceCallId == callIdParam.Integer() )
                {
                iViewCommandHandle->ExecuteCommand(EPhoneSetConferenceExpanded);
                }
            }
            break;
            
        default:
            CPhoneConference::HandlePhoneEngineMessageL( aMessage, 
                aCallId );
            break;
        }
    }

// --------------------------------------------------------------
// CPhoneConferenceAndWaiting::HandleKeyMessageL
// --------------------------------------------------------------
//
void CPhoneConferenceAndWaiting::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND(EPhoneUIStates, "CPhoneConferenceAndWaiting::HandleKeyMessageL()");
    switch ( aCode )
        {
        case EKeyYes:  // send-key
            if( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                iNumberEntryManager->CallFromNumberEntryL();
                }
            else
                {
                iStateMachine->SendPhoneEngineMessage(
                    MPEPhoneModel::EPEMessageAnswer ); 
                }
            break;
            
        default:
            CPhoneConference::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }


// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneUIStates, "CPhoneConferenceAndWaiting::HandleConnectedL()");
    TPhoneCmdParamCallStateData callStateData;
    if ( iRingingCallId == KErrNotFound )
        {
        TPhoneCmdParamCallStateData callStateData;
        callStateData.SetCallState( EPEStateRinging );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );
        iRingingCallId = callStateData.CallId();   
        }
    
    if ( iRingingCallId == aCallId )
        {
        // not a member of conference call
        MakeStateTransitionToConferenceAndSingleL( aCallId );
        }   
    else
        {
        // member of conference call which is on hold
        UpdateCallHeader( aCallId );
        }   
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::MakeStateTransitionToConferenceAndSingleL
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::MakeStateTransitionToConferenceAndSingleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneUIStates, "CPhoneConferenceAndWaiting::MakeStateTransitionToConferenceAndSingleL()");
    UpdateCallHeaderAndUiCommandsL( aCallId ); 
    iNumberEntryManager->SetVisibilityIfNumberEntryUsedL( ETrue );

    iStateMachine->ChangeState( EPhoneStateConferenceAndSingle );
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneUIStates, "CPhoneConferenceAndWaiting::HandleIdleL()");
    if ( iRingingCallId == KErrNotFound )
        {
        TPhoneCmdParamCallStateData callStateData;
        callStateData.SetCallState( EPEStateRinging );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );
        iRingingCallId = callStateData.CallId();   
        }
    
    if( iRingingCallId == aCallId )
        {
        
        BeginUiUpdateLC();
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
        iNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue);
        
        UpdateUiCommands();
        EndUiUpdate();
        iStateMachine->ChangeState( EPhoneStateConference );
        }
    else
        {
        // Idle message came for conference member
        CPhoneConference::HandleIdleL( aCallId ); 
        
        TPhoneCmdParamBoolean conferenceBubbleExists;
        iViewCommandHandle->ExecuteCommandL( 
                EPhoneViewGetIsConference,
                &conferenceBubbleExists );
        if( !conferenceBubbleExists.Boolean() )
            {
            HandleConferenceIdleL();
            }
        }
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::HandleConferenceIdleL
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::HandleConferenceIdleL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneConferenceAndWaiting::HandleConferenceIdleL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveConferenceBubble );
    
    TPhoneCmdParamInteger activeCallCount;
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewGetCountOfActiveCalls, &activeCallCount );
    switch( activeCallCount.Integer() )
        {
        case ENoActiveCalls: // Go to incoming state
            {           
            BringIncomingToForegroundL(); 
            UpdateCallHeaderAndUiCommandsL( iRingingCallId );
            
            if ( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                iNumberEntryManager->SetNumberEntryVisibilityL(EFalse);   
                }
            
            SetRingingTonePlaybackL( iRingingCallId );
            SetBackButtonActive(EFalse);
            iStateMachine->ChangeState( EPhoneStateIncoming );
            }
            break;
            
        case EOneActiveCall:
            iNumberEntryManager->SetVisibilityIfNumberEntryUsedL( ETrue );
            UpdateUiCommands();
            iStateMachine->ChangeState( EPhoneStateWaitingInSingle );
            break;
            
        case ETwoActiveCalls:
            iNumberEntryManager->SetVisibilityIfNumberEntryUsedL( ETrue );

            UpdateUiCommands();
            iStateMachine->ChangeState( EPhoneStateTwoSinglesAndWaiting );
            break; 
            
        default:
          // Too many active calls to handle reliable state change now.
          // Waiting for all conference member idles
          // This case may happen when 'End all calls' because conferenceIdle may 
          // come earlier than idle for conference member
           break;
        }
    EndUiUpdate();
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::HandleDiallingL
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::HandleDialingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConferenceAndWaiting::HandleDialingL()");
    DisplayCallHeaderL( aCallId, ESetNEVisibilityFalse );
    iStateMachine->ChangeState( EPhoneStateConferenceAndWaitingAndCallSetup );
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::HandleWentOneToOneL
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::HandleWentOneToOneL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConferenceAndWaiting::HandleWentOneToOneL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewPrivateFromConference, aCallId );
    
    if ( iRingingCallId == KErrNotFound )
        {
        TPhoneCmdParamCallStateData callStateData;
        callStateData.SetCallState( EPEStateRinging );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );
        iRingingCallId = callStateData.CallId();
        }
        
    TPhoneCmdParamBoolean conferenceBubbleExists;
    iViewCommandHandle->ExecuteCommandL( EPhoneViewGetIsConference, 
        &conferenceBubbleExists );
    if( conferenceBubbleExists.Boolean() && 
        ( iRingingCallId != KErrNotFound ) )
        {
        UpdateUiCommands();
        iStateMachine->ChangeState( EPhoneStateConferenceAndSingleAndWaiting );  
        }
    EndUiUpdate();
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneConferenceAndWaiting::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneConferenceAndWaiting::HandleCommandL() ");
    TBool commandStatus = ETrue;
    switch( aCommand )
        {
        case EPhoneCmdUpdateUiControls:
            UpdateUiControlsL();
            break;

        default:
            commandStatus = CPhoneConference::HandleCommandL( aCommand );
            break;
        }
    return commandStatus;
    }

// -----------------------------------------------------------
// CPhoneConferenceAndWaiting::UpdateUiControlsL
// -----------------------------------------------------------
//
void CPhoneConferenceAndWaiting::UpdateUiControlsL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneConferenceAndWaiting::UpdateUiControlsL( ) ");
    iViewCommandHandle->ExecuteCommand(EPhoneSetConferenceExpanded);
    UpdateUiCommands();
    }

// End of File
