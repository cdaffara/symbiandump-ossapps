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
* Description: Implementation of CPhoneTwoSingles class.
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
#include "cphonetwosingles.h"
#include "mphonestatemachine.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "phonestatedefinitions.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamappinfo.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamcallstatedata.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneTwoSingles::CPhoneTwoSingles(
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneGsmInCall( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneTwoSingles::~CPhoneTwoSingles()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneTwoSingles::~CPhoneTwoSingles()
    {
    }

// -----------------------------------------------------------
// CPhoneTwoSingles::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneTwoSingles::ConstructL()
    {
    CPhoneGsmInCall::ConstructL();
    
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
    }

// -----------------------------------------------------------
// CPhoneTwoSingles::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneTwoSingles* CPhoneTwoSingles::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneTwoSingles* self = new( ELeave ) CPhoneTwoSingles( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneTwoSingles::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneTwoSingles::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSingles::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageConnected:
        case MEngineMonitor::EPEMessageHeld:
            UpdateCallHeaderAndUiCommandsL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageConnectedConference:
            HandleConnectedConferenceL( aCallId );
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
// CPhoneTwoSingles:HandleIdleL
// -----------------------------------------------------------
//    
void CPhoneTwoSingles::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSingles::HandleIdleL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );

    TPhoneCmdParamInteger activeCallCount;
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewGetCountOfActiveCalls, &activeCallCount );

    switch( activeCallCount.Integer() )
        {
        case EOneActiveCall:
            {   
            iNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue);
            UpdateUiCommands();
            iStateMachine->ChangeState( EPhoneStateSingle ); 
            }
            
        default:
            break;
        }
    EndUiUpdate(); 
    }

// -----------------------------------------------------------
// CPhoneTwoSingles::HandleKeyMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneTwoSingles::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSingles::HandleKeyMessageL()");
    switch ( aCode )
        {
        case EKeyYes: // send-key
            {
            if ( !iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                iStateMachine->SendPhoneEngineMessage(
                    CPEPhoneModelIF::EPEMessageSwap );
                }
            else
                {
                iNumberEntryManager->CallFromNumberEntryL();
                }
            break;    
            }

        default:
            CPhoneGsmInCall::HandleKeyMessageL( aMessage, aCode );   
            break;
        }
    }
    
// -----------------------------------------------------------
// CPhoneTwoSingles::HandleNumberEntryClearedL()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneTwoSingles::HandleNumberEntryClearedL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSingles::HandleNumberEntryClearedL()");
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneTwoSingles::HandleConnectedConferenceL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneTwoSingles::HandleConnectedConferenceL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneTwoSingles::HandleConnectedConferenceL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewCreateConference, aCallId );
    UpdateUiCommands();
    EndUiUpdate();
    iStateMachine->ChangeState( EPhoneStateConference );
    }

// -----------------------------------------------------------
// CPhoneTwoSingles::HandleIncomingL
// -----------------------------------------------------------
//
void CPhoneTwoSingles::HandleIncomingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
            "CPhoneTwoSingles::HandleIncomingL()");
    DisplayCallHeaderL( aCallId, ECheckIfNEUsedBeforeSettingVisibilityFalse );
    iStateMachine->ChangeState( EPhoneStateTwoSinglesAndWaiting );
    }

// End of File
