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
* Description: Implementation of CPhoneCallSetup class.
*
*/


// INCLUDES
#include <MediatorDomainUIDs.h>
#include "cphonecallsetup.h"
#include "mphonestatemachine.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamcallheaderdata.h"
#include "phonestatedefinitionsgsm.h"
#include "cphonetimer.h"
#include "phonerssbase.h"
#include "cphonemainresourceresolver.h"
#include "phonelogger.h"
#include "cphonegeneralgsmmessageshandler.h"
#include "cphonemediatorfactory.h"
#include "cphonemediatorsender.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneCallSetup::CPhoneCallSetup( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneStateCallSetup( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneCallSetup::~CPhoneCallSetup()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneCallSetup::~CPhoneCallSetup()
    {
    }

// -----------------------------------------------------------
// CPhoneCallSetup::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneCallSetup::ConstructL()
    {
    CPhoneStateCallSetup::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneCallSetup::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneCallSetup* CPhoneCallSetup::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneCallSetup* self = new (ELeave) CPhoneCallSetup( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneCallSetup::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneCallSetup::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneCallSetup::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageConnecting:
            HandleConnectingL( aCallId );
            break;

        // fall through.
        case MEngineMonitor::EPEMessageIssuingSSRequest:
        case MEngineMonitor::EPEMessageCallBarred:
        case MEngineMonitor::EPEMessageIssuedSSRequest:
        case MEngineMonitor::EPEMessageTempClirActivationUnsuccessful:
        case MEngineMonitor::EPEMessageIncCallIsForw:
        case MEngineMonitor::EPEMessageIncCallForwToC:
        case MEngineMonitor::EPEMessageOutCallForwToC:
        case MEngineMonitor::EPEMessageForwardUnconditionalModeActive: 
        case MEngineMonitor::EPEMessageForwardConditionallyModeActive:
            {
            __PHONELOG1( 
                EBasic, 
                EPhoneUIStates, 
                "CPhoneCallSetup::HandlePhoneEngineMessageL Start create gsm message handler %d",
                aMessage ); 
            CPhoneGeneralGsmMessagesHandler* gsmMsgHandler =
                CPhoneGeneralGsmMessagesHandler::NewL( 
                        *iStateMachine,
                        *iViewCommandHandle,
                        *this );
            CleanupStack::PushL( gsmMsgHandler );
            gsmMsgHandler->HandlePhoneEngineMessageL( aMessage, aCallId );
            CleanupStack::PopAndDestroy( gsmMsgHandler );
            }
            break;
            
        default:
            CPhoneStateCallSetup::HandlePhoneEngineMessageL( aMessage, 
                aCallId );
            break;
        }
    }

// -----------------------------------------------------------------------------
// CPhoneCallSetup::HandleConnectingL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneCallSetup::HandleConnectingL( TInt aCallId )
    {
    // Only handle connecting case for GSM protocol. In CDMA, this message is 
    // set when the CDMA network receives the call, not (like in GSM) when
    // when the remote party receives the call. So, in CDMA, the user
    // should still be able to cancel the MO call before the call is connected.
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneCallSetup::HandleConnectingL()");
    BeginUiUpdateLC();
    
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );

    TPhoneCmdParamBoolean booleanParam;
    booleanParam.SetBoolean( ETrue );
    iViewCommandHandle->ExecuteCommand( EPhoneViewHsToForegroundAfterCall,
        &booleanParam );
    
    UpdateCallHeaderAndUiCommandsL( aCallId);

    SetToolbarButtonLoudspeakerEnabled();
    EndUiUpdate();
    iStateMachine->ChangeState( EPhoneStateAlerting );
    }

// End of File
