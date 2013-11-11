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
* Description: Implementation of CPhoneCallSetupAndWaiting class.
*
*/


// INCLUDES
#include <pevirtualengine.h>
#include <StringLoader.h>
#include <MediatorDomainUIDs.h>

#include "cphonecallsetupandwaiting.h"
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
#include "cphonemediatorfactory.h"
#include "cphonemediatorsender.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneCallSetupAndWaiting::CPhoneCallSetupAndWaiting(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) :
    CPhoneGsmInCall( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneCallSetupAndWaiting::~CPhoneCallSetupAndWaiting()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneCallSetupAndWaiting::~CPhoneCallSetupAndWaiting()
    {
    }

// -----------------------------------------------------------
// CPhoneCallSetupAndWaiting::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneCallSetupAndWaiting::ConstructL()
    {
    CPhoneGsmInCall::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneCallSetupAndWaiting::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneCallSetupAndWaiting* CPhoneCallSetupAndWaiting::NewL(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneCallSetupAndWaiting* self = new( ELeave ) CPhoneCallSetupAndWaiting(
        aStateMachine, aViewCommandHandle, aPhoneCustomization );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------
// CPhoneCallSetupAndWaiting::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneCallSetupAndWaiting::HandlePhoneEngineMessageL(
    const TInt aMessage,
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneCallSetupAndWaiting::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;

        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;

        default:
            CPhoneGsmInCall::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneCallSetupAndWaiting::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneCallSetupAndWaiting::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneCallSetupAndWaiting::HandleIdleL()");
    BeginUiUpdateLC();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );

    // Find out do we have waiting or outgoing call left
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL( EPhoneViewGetCallIdByState,
        &callStateData );

    if( callStateData.CallId() > KErrNotFound )
        {
        BringIncomingToForegroundL();       
        SetRingingTonePlaybackL( callStateData.CallId() );
        UpdateCallHeaderAndUiCommandsL( callStateData.CallId() );
        SetBackButtonActive(EFalse);
        iStateMachine->ChangeState( EPhoneStateIncoming );
        }
    else // waiting call was terminated.
        {
        UpdateUiCommands();
        iStateMachine->ChangeState( EPhoneStateAlerting );
        }
    EndUiUpdate();
    }

// -----------------------------------------------------------
// CPhoneCallSetupAndWaiting::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneCallSetupAndWaiting::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneCallSetupAndWaiting::HandleConnectedL() ");
    UpdateCallHeaderAndUiCommandsL( aCallId );
    iNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue);
    iStateMachine->ChangeState( EPhoneStateWaitingInSingle );
    }

// End of File
