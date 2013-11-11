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
* Description: Implementation of CPhoneAlerting class.
*
*/


// INCLUDES
#include <pevirtualengine.h>
#include <mpeengineinfo.h>
#include <mpeclientinformation.h>
#include <MediatorDomainUIDs.h>
#include "cphonealerting.h"
#include "mphonestatemachine.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamcallstatedata.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"
#include "phoneconstants.h"
#include "cphonemediatorfactory.h"
#include "cphonemediatorsender.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneAlerting::CPhoneAlerting( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneGsmInCall( aStateMachine, aViewCommandHandle, aPhoneCustomization ),
    iIncomingCallId( KErrNotFound )
    {
    }

// -----------------------------------------------------------
// CPhoneAlerting::~CPhoneAlerting()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneAlerting::~CPhoneAlerting()
    {
    }

// -----------------------------------------------------------
// CPhoneAlerting::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneAlerting::ConstructL()
    {
    CPhoneGsmInCall::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneAlerting::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneAlerting* CPhoneAlerting::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneAlerting* self = new( ELeave ) CPhoneAlerting( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// --------------------------------------------------------------
// CPhoneAlerting::HandleKeyMessageL
// --------------------------------------------------------------
//
EXPORT_C void CPhoneAlerting::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneAlerting::HandleKeyMessageL()");
    switch ( aCode )
        {
        case EKeyYes: // send-key
            if ( iNumberEntryManager->IsNumberEntryUsedL() )
                {
                iNumberEntryManager->CallFromNumberEntryL();
                }
            else
                {
                SendGlobalErrorNoteL( 
                    EPhoneNoteTextNotAllowed, ETrue );
                }
            break;

        case EKeyNo: // end-key
            if ( aMessage == EPhoneKeyLongPress )
                {
                iStateMachine->SendPhoneEngineMessage(
                    MPEPhoneModel::EPEMessageTerminateAllConnections ); 
                if ( iNumberEntryManager->IsNumberEntryUsedL() )
                    {
                    iViewCommandHandle->ExecuteCommandL( 
                        EPhoneViewRemoveNumberEntry );
                    // Do state-specific operation when number entry is cleared
                    HandleNumberEntryClearedL();
                    }
                }
            else // handle short end key
                {
                DisconnectOutgoingCallL();
                }
            break;
            
        default:
            CPhoneGsmInCall::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneAlerting::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneAlerting::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneAlerting::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;
        
        case MEngineMonitor::EPEMessageDisconnecting:
            HandleDisconnectingL( aCallId );
            break;
        
        case MEngineMonitor::EPEMessageRemoteTerminated:
            iViewCommandHandle->ExecuteCommandL( 
                EPhoneViewHideNaviPaneAudioVolume );
            CPhoneGsmInCall::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        
        case MEngineMonitor::EPEMessageIncoming:
            {
            // If incoming message is received in alerting state, teardown of
            // alerting call must be ongoing. Handling of the incoming call is
            // buffered until first call teardown is completed.
            iIncomingCallId = aCallId;
            }
            break;
        
        case MEngineMonitor::EPEMessageIdle:
            {
            if ( KErrNotFound < iIncomingCallId )
                {
                TInt incomingCallId = iIncomingCallId;
                iIncomingCallId = KErrNotFound;
                
                if ( aCallId != incomingCallId )
                    {
                    CPhoneGsmInCall::HandlePhoneEngineMessageL( 
                        aMessage, aCallId );
                    // Simulate incoming event which was received while 
                    // teardown of first call was ongoing.
                    iStateMachine->State()->HandlePhoneEngineMessageL( 
                        MEngineMonitor::EPEMessageIncoming, incomingCallId );
                    }
                
                return;
                }
            }
            break;
        
        default:
            break;
        }
    CPhoneGsmInCall::HandlePhoneEngineMessageL( aMessage, aCallId );
    }

// -----------------------------------------------------------
// CPhoneAlerting::HandleConnectedL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneAlerting::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
        "CPhoneAlerting::HandleConnectedL()");
    BeginUiUpdateLC();
    UpdateSingleActiveCallL( aCallId );
    UpdateUiCommands();
    EndUiUpdate();
    iStateMachine->ChangeState( EPhoneStateSingle );
    }

// -----------------------------------------------------------
// CPhoneAlerting::HandleCommandL
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneAlerting::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,  
        "CPhoneAlerting::HandleCommandL()" );
    return CPhoneGsmInCall::HandleCommandL( aCommand );
    }

// -----------------------------------------------------------
// CPhoneAlerting::IsVideoCallAlertingL
// -----------------------------------------------------------
//
TBool CPhoneAlerting::IsVideoCallAlertingL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,  
        "CPhoneAlerting::IsVideoCallAlerting()" );
    TBool retVal = EFalse;
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateConnecting );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    if ( callStateData.CallId() > KErrNotFound )
        {
        retVal = IsVideoCall( callStateData.CallId() );
        }
    return retVal;
    }
    
// -----------------------------------------------------------
// CPhoneAlerting::CheckIfShowTerminationNote
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneAlerting::CheckIfShowCallTerminationNote( )
    {
    // Alerting state does not need call termination since
    // call has not begun.
    return EFalse;
    }

// -----------------------------------------------------------
// CPhoneAlerting::HandleDisconnectingL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneAlerting::HandleDisconnectingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,  
            "CPhoneAlerting::HandleDisconnectingL()" );
    CPhoneGsmInCall::HandleDisconnectingL( aCallId );
    }

// End of File
