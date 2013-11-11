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
* Description: Implementation of CPhoneSingleAndAlerting class.
*
*/


// INCLUDES
#include <pevirtualengine.h>
#include <cpephonemodelif.h>
#include "cphonesingleandalerting.h"
#include "tphonecmdparamboolean.h"
#include "mphonestatemachine.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamcallstatedata.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneSingleAndAlerting::CPhoneSingleAndAlerting( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneAlerting( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneSingleAndAlerting::~CPhoneSingleAndAlerting()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneSingleAndAlerting::~CPhoneSingleAndAlerting()
    {
    }

// -----------------------------------------------------------
// CPhoneSingleAndAlerting::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneSingleAndAlerting::ConstructL()
    {
    CPhoneAlerting::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneSingleAndAlerting::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneSingleAndAlerting* CPhoneSingleAndAlerting::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneSingleAndAlerting* self = new( ELeave ) CPhoneSingleAndAlerting( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneSingleAndAlerting::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneSingleAndAlerting::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndAlerting::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;
            
        default:
            CPhoneAlerting::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// --------------------------------------------------------------
// CPhoneSingleAndAlerting::HandleKeyMessageL
// --------------------------------------------------------------
//
void CPhoneSingleAndAlerting::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndAlerting::HandleKeyMessageL()");
    switch ( aCode )
        {
        case EKeyYes: // send-key
            if ( iNumberEntryManager->IsNumberEntryUsedL() )
                {
                iNumberEntryManager->CallFromNumberEntryL();
                }
            else
                {
                CPhoneState::SendGlobalErrorNoteL( 
                    EPhoneNoteTextNotAllowed, ETrue );
                }
            break;
            
        case EKeyNo: // end-key
            if ( aMessage == EPhoneKeyLongPress )
                {
                iStateMachine->SendPhoneEngineMessage(
                    CPEPhoneModelIF::EPEMessageTerminateAllConnections );        
                }
            else
                {
                DisconnectOutgoingCallL();
                }
                
            if ( iNumberEntryManager->IsNumberEntryUsedL() )
                {
                iViewCommandHandle->ExecuteCommandL( 
                      EPhoneViewRemoveNumberEntry );
                // Do state-specific operation when number entry is cleared
                HandleNumberEntryClearedL();
                }
            break;
            
        default:
            CPhoneAlerting::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneSingleAndAlerting::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneSingleAndAlerting::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndAlerting::HandleConnectedL()");
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateConnecting );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
        
    if( callStateData.CallId() == aCallId )
        {
        UpdateCallHeaderAndUiCommandsL( aCallId );
        iStateMachine->ChangeState( EPhoneStateTwoSingles );
        }
    else
        {
        UpdateCallHeader( aCallId );  
        }    
    }
// -----------------------------------------------------------
// CPhoneSingleAndAlerting::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneSingleAndAlerting::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndAlerting::HandleIdleL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
        
    TPhoneCmdParamInteger countParam;
    iViewCommandHandle->ExecuteCommandL( EPhoneViewGetCountOfActiveCalls, &countParam);
    
    if ( countParam.Integer() )
        {
        UpdateUiCommands();
        iStateMachine->ChangeState( EPhoneStateSingle );
        }
    else
        {
        DisplayCallTerminationNoteL();
        UpdateUiCommands();
        iStateMachine->ChangeState( EPhoneStateAlerting );
        }
    EndUiUpdate();
    }
        
// End of File
