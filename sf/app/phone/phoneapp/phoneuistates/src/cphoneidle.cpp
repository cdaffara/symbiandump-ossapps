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
* Description: Implementation of CPhoneIdle class.
*
*/


// INCLUDES
#include "cphoneidle.h"
#include "phonerssbase.h"
#include "phonelogger.h"
#include "cphonegeneralgsmmessageshandler.h"
#include "tphonecmdparamboolean.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneIdle::CPhoneIdle( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneStateIdle( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneIdle::~CPhoneIdle()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------

EXPORT_C CPhoneIdle::~CPhoneIdle()
    {
    }

// -----------------------------------------------------------
// CPhoneIdle::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneIdle::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneIdle::ConstructL()");
    CPhoneStateIdle::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneIdle::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneIdle* CPhoneIdle::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneIdle* self = new (ELeave) 
        CPhoneIdle( aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneIdle::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneIdle::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneUIStates, 
        "CPhoneIdle::HandlePhoneEngineMessageL() ");
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
            
        case MEngineMonitor::EPEMessageAudioOutputChanged:
            HandleAudioOutputChangedL();
            break;

        case MEngineMonitor::EPEMessageIssuedSSRequest: // fall through
        case MEngineMonitor::EPEMessageIssuingSSRequest: // fall through
        case MEngineMonitor::EPEMessageCallBarred: // fall through
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
            CPhoneStateIdle::HandlePhoneEngineMessageL( 
                aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneIdle::HandleErrorL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneIdle::HandleErrorL( const TPEErrorInfo& aErrorInfo )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneIdle::HandleErrorL() ");
    CPhoneState::HandleErrorL( aErrorInfo );
    }

// End of File
