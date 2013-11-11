/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneTwoSinglesAndWaiting class.
*
*/


// INCLUDES
#include <pevirtualengine.h>
#include <StringLoader.h>
#include <mpeengineinfo.h>
#include <featmgr.h>
#include "cphonetwosinglesandwaiting.h"
#include "tphonecmdparamboolean.h"
#include "mphonestatemachine.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamcallstatedata.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"
#include "mphonestorage.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneTwoSinglesAndWaiting::CPhoneTwoSinglesAndWaiting( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneTwoSingles( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::~CPhoneTwoSinglesAndWaiting()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneTwoSinglesAndWaiting::~CPhoneTwoSinglesAndWaiting()
    {
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneTwoSinglesAndWaiting::ConstructL()
    {
    CPhoneTwoSingles::ConstructL();
    
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    iRingingCallId = callStateData.CallId();
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneTwoSinglesAndWaiting* CPhoneTwoSinglesAndWaiting::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneTwoSinglesAndWaiting* self = new( ELeave ) CPhoneTwoSinglesAndWaiting( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::HandleKeyMessageL
// -----------------------------------------------------------
//
void CPhoneTwoSinglesAndWaiting::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSinglesAndWaiting::HandleKeyMessageL()");
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
                // Waiting call bubble is over number entry
                // so display not allowed note because we
                // can't answer the call
                SendGlobalWarningNoteL( EPhoneNoteTextCallNotAllowed, ETrue );                     
                }
            break;
            }

        default:
            CPhoneTwoSingles::HandleKeyMessageL( aMessage, aCode );   
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneTwoSinglesAndWaiting::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSinglesAndWaiting::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;
        case MEngineMonitor::EPEMessageConnected:
            UpdateUiCommands();
            CPhoneTwoSingles::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;           
        default:
            CPhoneTwoSingles::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::HandleErrorL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneTwoSinglesAndWaiting::HandleErrorL( 
        const TPEErrorInfo& aErrorInfo )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneTwoSinglesAndWaiting::HandleErrorL()");
    
    if( aErrorInfo.iErrorCode == ECCPErrorCCUserAlertingNoAnswer )
        {
        // Should not shown "No Answer" note
        __PHONELOG1( EBasic, EPhoneUIStates,
       "PhoneUIControl: CPhoneTwoSinglesAndWaiting::HandleErrorL - aErrorInfo.iErrorCode =%d ",
            aErrorInfo.iErrorCode);
        }
    else
        {
        CPhoneState::HandleErrorL( aErrorInfo );
        }
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneTwoSinglesAndWaiting::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSinglesAndWaiting::HandleIdleL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
    
    if ( iRingingCallId == aCallId )
        {
        StateTransitionToTwoSinglesL();    
        }
    else
        {
        StateTransitionToSingleAndWaitingL();
        }
    EndUiUpdate();
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::StateTransitionToTwoSinglesL
// -----------------------------------------------------------
//
void CPhoneTwoSinglesAndWaiting::StateTransitionToTwoSinglesL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSinglesAndWaiting::StateTransitionToTwoSinglesL()");
    iNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue);
    
    UpdateUiCommands();
    iStateMachine->ChangeState( EPhoneStateTwoSingles );
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::StateTransitionToSingleAndWaitingL
// -----------------------------------------------------------
//
void CPhoneTwoSinglesAndWaiting::StateTransitionToSingleAndWaitingL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSinglesAndWaiting::StateTransitionToSingleAndWaitingL()")
    UpdateUiCommands();
    iStateMachine->ChangeState( EPhoneStateWaitingInSingle );
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::HandleConnectedConferenceL
// -----------------------------------------------------------
//
void CPhoneTwoSinglesAndWaiting::HandleConnectedConferenceL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSinglesAndWaiting::HandleConnectedConferenceL()");
    BeginUiUpdateLC();

    iViewCommandHandle->ExecuteCommandL( EPhoneViewCreateConference, aCallId );
    UpdateUiCommands();
    EndUiUpdate();
    iStateMachine->ChangeState( EPhoneStateConferenceAndWaiting );
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::HandleNumberEntryClearedL()
// -----------------------------------------------------------
//
void CPhoneTwoSinglesAndWaiting::HandleNumberEntryClearedL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSinglesAndWaiting::HandleNumberEntryClearedL()");
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneTwoSinglesAndWaiting::HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneTwoSinglesAndWaiting::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneTwoSinglesAndWaiting::HandleCommandL() ");
    TBool commandStatus = ETrue;
    switch( aCommand )
        {
        case EPhoneCmdUpdateUiControls:
            UpdateUiCommands();
            break;

        default:
            commandStatus = CPhoneGsmInCall::HandleCommandL( aCommand );
            break;
        }

    return commandStatus;
    }


// End of File
