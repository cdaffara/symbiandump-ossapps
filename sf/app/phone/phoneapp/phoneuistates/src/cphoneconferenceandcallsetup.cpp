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
* Description: Implementation of CPhoneConferenceAndCallSetup class.
*
*/


// INCLUDES
#include "cphoneconferenceandcallsetup.h"
#include "mphonestatemachine.h"
#include "phoneviewcommanddefinitions.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamcallstatedata.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "tphonecmdparamglobalnote.h"
#include "phoneui.hrh"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneConferenceAndCallSetup::CPhoneConferenceAndCallSetup( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneConference( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup::~CPhoneConferenceAndCallSetup()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneConferenceAndCallSetup::~CPhoneConferenceAndCallSetup()
    {
    }

// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneConferenceAndCallSetup::ConstructL()
    {
    CPhoneConference::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneConferenceAndCallSetup* CPhoneConferenceAndCallSetup::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneConferenceAndCallSetup* self = new( ELeave ) CPhoneConferenceAndCallSetup( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneConferenceAndCallSetup::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneConferenceAndCallSetup::HandlePhoneEngineMessageL()");
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
            
        case MEngineMonitor::EPEMessageConferenceIdle:
            HandleConferenceIdleL();
            break;
            
        default:
            CPhoneConference::HandlePhoneEngineMessageL( aMessage, 
                aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup::HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneConferenceAndCallSetup::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConferenceAndCallSetup::HandleCommandL()");
    TBool commandStatus = ETrue;
    switch( aCommand )
        {   
        case EPhoneDtmfDialerCancel:
            {
            }
            break;    
    
        default:
            commandStatus = CPhoneConference::HandleCommandL( aCommand );
            break;
        }
    return commandStatus;
    }
    
// --------------------------------------------------------------
// CPhoneConferenceAndCallSetup::HandleKeyMessageL
// --------------------------------------------------------------
//
void CPhoneConferenceAndCallSetup::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConferenceAndCallSetup::HandleKeyMessageL()");
    switch ( aCode )
        {
        case EKeyNo: // End-key
            if ( aMessage == EPhoneKeyLongPress )
                {
                iStateMachine->SendPhoneEngineMessage(
                        MPEPhoneModel::EPEMessageTerminateAllConnections );   
                }
            else
                {
                DisconnectOutgoingCallL();
                }
            break;
            
        default:
            CPhoneConference::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup::HandleConnectingL
// -----------------------------------------------------------
//
void CPhoneConferenceAndCallSetup::HandleConnectingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneConferenceAndCallSetup::HandleConnectingL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveGlobalNote );
    
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
    
    iNumberEntryManager->RemoveNumberEntryIfVisibilityIsFalseL();
    
    UpdateCallHeaderAndUiCommandsL( aCallId );

    EndUiUpdate();        
    }
    
// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneConferenceAndCallSetup::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneConferenceAndCallSetup::HandleConnectedL()");
    UpdateCallHeaderAndUiCommandsL( aCallId );
    iNumberEntryManager->RemoveNumberEntryIfVisibilityIsFalseL();
    iStateMachine->ChangeState( EPhoneStateConferenceAndSingle );
    }

// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup:HandleIdleL
// -----------------------------------------------------------
//
void CPhoneConferenceAndCallSetup::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConferenceAndCallSetup::HandleIdleL()");
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );

    TPhoneCmdParamBoolean conferenceExistsForCallId;
    iViewCommandHandle->ExecuteCommandL( EPhoneViewGetCallExistsInConference,
        aCallId, &conferenceExistsForCallId );
    
    if( conferenceExistsForCallId.Boolean() )
        {
        // Remove 'Conference on hold' global note
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveGlobalNote );
        iViewCommandHandle->ExecuteCommandL( 
                EPhoneViewRemoveFromConference, 
                aCallId );
        
        //Check if removed call was last one to be able to be in conference mode
        //If there is no confrence call, go to single and call setup state
        TPhoneCmdParamBoolean conferenceExists;
        iViewCommandHandle->ExecuteCommandL( EPhoneViewGetIsConference, 
            &conferenceExists );
        __PHONELOG1( EBasic, EPhoneControl,
            "CPhoneConferenceAndCallSetup::HandleIdleL - conferenceExists =%d ",
            conferenceExists.Boolean() );
                
        if( !conferenceExists.Boolean() )
            {
            if( iAlerting )
                {
                iStateMachine->ChangeState( EPhoneStateAlertingInSingle ); 
                }
            else
                {
                iStateMachine->ChangeState( EPhoneStateCallSetupInSingle );
                }
            }
        }
    else
        {
        // Remove  outgoing call 
        BeginUiUpdateLC();
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
        iNumberEntryManager->SetVisibilityIfNumberEntryUsedL( ETrue );
        UpdateUiCommands();
        EndUiUpdate();
        iStateMachine->ChangeState( EPhoneStateConference );
        }
    }

// -----------------------------------------------------------
// CPhoneConferenceAndCallSetup:HandleConferenceIdleL
// -----------------------------------------------------------
//
void CPhoneConferenceAndCallSetup::HandleConferenceIdleL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneConferenceAndCallSetup::HandleConferenceIdleL()");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveConferenceBubble );
    
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateConnecting );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );

    if ( callStateData.CallId() > KErrNotFound )
        {
        // Go to alerting and single state
        UpdateUiCommands();
        iStateMachine->ChangeState( EPhoneStateAlertingInSingle );     
        }
    else
        {
        UpdateUiCommands();
        iStateMachine->ChangeState( EPhoneStateCallSetupInSingle );
        }
    }

// End of File
