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
* Description: Implementation of CPhoneIncoming class.
*
*/


// INCLUDES
#include <featmgr.h>
#include <telephonyvariant.hrh>
#include "cphoneincoming.h"
#include "phonerssbase.h"
#include "phonelogger.h"
#include "cphonegeneralgsmmessageshandler.h"
#include "phonestatedefinitionsgsm.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamboolean.h"
#include "phoneui.hrh"
#include "mphonestatemachine.h"
#include "mphonestorage.h"
#include "cphonecenrepproxy.h"
#include "tphonecmdparamcallstatedata.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneIncoming::CPhoneIncoming( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneStateIncoming( aStateMachine, aViewCommandHandle, aPhoneCustomization ),
    iWaitingCallId( KErrNotFound )
    {
    }

// -----------------------------------------------------------
// CPhoneIncoming::~CPhoneIncoming()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneIncoming::~CPhoneIncoming()
    {
    }

// -----------------------------------------------------------
// CPhoneIncoming::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneIncoming::ConstructL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneIncoming::ConstructL()");
    CPhoneStateIncoming::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneIncoming::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneIncoming* CPhoneIncoming::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneIncoming* self = new (ELeave) 
        CPhoneIncoming( aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneIncoming::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneIncoming::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneUIStates, 
        "CPhoneIncoming::HandlePhoneEngineMessageL() ");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIncoming:
            iWaitingCallId = aCallId;
            break;

        case MEngineMonitor::EPEMessageConnected:
            if( iWaitingCallId > KErrNotFound )
                {
                HandleIncomingL( iWaitingCallId );
                }
            else // There is no another incoming call
                {
                CPhoneStateIncoming::HandlePhoneEngineMessageL( 
                    aMessage, aCallId );
                }
            break;
            
        case MEngineMonitor::EPEMessageRemoteHeld:
                SendGlobalInfoNoteL( EPhoneInformationRemotePutOnHoldNote, ETrue );
            break;
        
        case MEngineMonitor::EPEMessageRemoteResumed:
                SendGlobalInfoNoteL( EPhoneInformationConnectedNote, ETrue );
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
            CPhoneGeneralGsmMessagesHandler* gsmMsgHandler =
                CPhoneGeneralGsmMessagesHandler::NewL( *iStateMachine,
                                                       *iViewCommandHandle,
                                                       *this );
            CleanupStack::PushL( gsmMsgHandler );
            gsmMsgHandler->HandlePhoneEngineMessageL( aMessage, aCallId );
            CleanupStack::PopAndDestroy( gsmMsgHandler );
            }
            break;
            
        case MEngineMonitor::EPEMessageIdle:
           {
           // Check if there is another incoming call while the first one is
           // still disconnecting
           if ( iWaitingCallId > KErrNotFound && aCallId != iWaitingCallId )
               {
               HandleIncomingL( iWaitingCallId );
               
               // Prevent execution of CPhoneStateIncoming::HandleIdleL, only remove
               // the call bubble
               BeginUiUpdateLC();
               iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
               EndUiUpdate();
               iWaitingCallId = KErrNotFound;
               }
           else
               {
               CPhoneStateIncoming::HandlePhoneEngineMessageL( 
                   aMessage, aCallId );
               }
           }
           break;

        default:
            CPhoneStateIncoming::HandlePhoneEngineMessageL( 
                aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
//  CPhoneIncoming::HandleIncomingL
// -----------------------------------------------------------
void CPhoneIncoming::HandleIncomingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneIncoming::HandleIncomingL()");
    TPhoneCmdParamCallStateData callState;
    callState.SetCallState( EPEStateConnected );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewGetCallIdByState, &callState );
    TInt connectedCall = callState.CallId(); 
    
    DisplayCallHeaderL( aCallId, ECheckIfNEUsedBeforeSettingVisibilityFalse );

    if ( connectedCall > KErrNotFound )
        {
        UpdateUiCommands();
        iStateMachine->ChangeState( EPhoneStateWaitingInSingle );   
        }
    }

// -----------------------------------------------------------
// CPhoneIncoming::HandleErrorL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneIncoming::HandleErrorL( 
         const TPEErrorInfo& aErrorInfo )
     {
     __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneIncoming::HandleErrorL()");
     if( aErrorInfo.iErrorCode == ECCPErrorCCUserAlertingNoAnswer )
         {
         // Should not show "No Answer" note
         __PHONELOG1( EBasic, EPhoneUIStates,
        "PhoneUIControl: CPhoneIncoming::HandleErrorL - aErrorInfo.iErrorCode =%d ",
             aErrorInfo.iErrorCode);
         }
     else
         {
         CPhoneState::HandleErrorL( aErrorInfo );
         }
     }
 
// End of File
