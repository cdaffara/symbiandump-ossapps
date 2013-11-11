/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneStartup class.
*
*/


// INCLUDES
#include "cphonestartup.h"
#include "phonerssbase.h"
#include "phonelogger.h"
#include "cphonegeneralgsmmessageshandler.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneStartup::CPhoneStartup( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    TBool aPEReady ) : 
    CPhoneStateStartup( aStateMachine, aViewCommandHandle, NULL )
    {
    iPEReady = aPEReady;
    }

// -----------------------------------------------------------
// CPhoneStartup::~CPhoneStartup()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------

CPhoneStartup::~CPhoneStartup()
    {
    }

// -----------------------------------------------------------
// CPhoneStartup::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneStartup::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneStartup::ConstructL()");
    CPhoneStateStartup::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneStartup::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStartup* CPhoneStartup::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle, 
    TBool aPEReady )
    {
    CPhoneStartup* self = new (ELeave) 
        CPhoneStartup( aStateMachine, aViewCommandHandle, aPEReady );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStartup::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneStartup::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneUIStates, 
        "CPhoneStartup::HandlePhoneEngineMessageL() ");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageRemoteHeld:
            CPhoneState::SendGlobalInfoNoteL( 
                EPhoneInformationRemotePutOnHoldNote, ETrue );
            break;
        
        case MEngineMonitor::EPEMessageRemoteResumed:
            CPhoneState::SendGlobalInfoNoteL( 
                EPhoneInformationConnectedNote, ETrue );
            break;

        case MEngineMonitor::EPEMessageIssuingSSRequest: // fall through
        case MEngineMonitor::EPEMessageCallBarred: // fall through
        case MEngineMonitor::EPEMessageIssuedSSRequest: // fall through
        case MEngineMonitor::EPEMessageTempClirActivationUnsuccessful:
        case MEngineMonitor::EPEMessageIncCallIsForw: // fall through
        case MEngineMonitor::EPEMessageIncCallForwToC: // fall through
        case MEngineMonitor::EPEMessageOutCallForwToC: // fall through
        case MEngineMonitor::EPEMessageForwardUnconditionalModeActive: // fall through
        case MEngineMonitor::EPEMessageForwardConditionallyModeActive:
            {
            CPhoneGeneralGsmMessagesHandler* gsmMsgHandler =
                CPhoneGeneralGsmMessagesHandler::NewL( *iStateMachine,
                                                       *iViewCommandHandle,
                                                       *this );
            CleanupStack::PushL( gsmMsgHandler );
            gsmMsgHandler->HandlePhoneEngineMessageL( aMessage, aCallId );
            CleanupStack::PopAndDestroy( gsmMsgHandler );
            }
            break;

        default:
            CPhoneStateStartup::HandlePhoneEngineMessageL( 
                aMessage, aCallId );
            break;
        }
    }

// End of File
