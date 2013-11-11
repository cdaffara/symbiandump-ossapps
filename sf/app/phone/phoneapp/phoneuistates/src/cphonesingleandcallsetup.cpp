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
* Description: Implementation of CPhoneSingleAndCallSetup class.
*
*/


// INCLUDES
#include <featmgr.h>
#include <cpephonemodelif.h>
#include "cphonesingleandcallsetup.h"
#include "mphonestatemachine.h"
#include "phoneviewcommanddefinitions.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "phonestatedefinitionsgsm.h"
#include "phonelogger.h"
#include "phonerssbase.h"
#include "phoneui.hrh"
#include "tphonecmdparamglobalnote.h"
#include "cphonemainresourceresolver.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPhoneSingleAndCallSetup::CPhoneSingleAndCallSetup( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneCallSetup( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetup::~CPhoneSingleAndCallSetup()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneSingleAndCallSetup::~CPhoneSingleAndCallSetup()
    {
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetup::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetup::ConstructL()
    {
    CPhoneCallSetup::ConstructL();
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetup::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneSingleAndCallSetup* CPhoneSingleAndCallSetup::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneSingleAndCallSetup* self = new( ELeave ) CPhoneSingleAndCallSetup( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetup::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetup::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates,
         "CPhoneSingleAndCallSetup::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageConnecting:
            HandleConnectingL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;
            
        default:
            CPhoneCallSetup::HandlePhoneEngineMessageL( aMessage, 
                aCallId );
            break;
        }
    }

// --------------------------------------------------------------
// CPhoneSingleAndCallSetup::HandleKeyMessageL
// --------------------------------------------------------------
//
void CPhoneSingleAndCallSetup::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndCallSetup::HandleKeyMessageL()");
    switch ( aCode )
        {
        // send-key
        case EKeyYes:
            if ( iNumberEntryManager->IsNumberEntryUsedL() )
                {
                // send a manual control sequence
                iNumberEntryManager->CallFromNumberEntryL();
                }
            else
                {
                // Show not allowed note
                CPhoneState::SendGlobalErrorNoteL( 
                    EPhoneNoteTextNotAllowed, ETrue );
                }
            break;

        // end-key
        case EKeyNo:
            if ( aMessage == EPhoneKeyLongPress )
                {
                iStateMachine->SendPhoneEngineMessage(
                    CPEPhoneModelIF::EPEMessageTerminateAllConnections );        
                }
            else
                {
                // Fetch active call's id from view
                TPhoneCmdParamCallStateData callStateData;
                // Find the dialing call
                callStateData.SetCallState( EPEStateDialing );
                iViewCommandHandle->HandleCommandL(
                    EPhoneViewGetCallIdByState, &callStateData ); 
            
                if( callStateData.CallId() > KErrNotFound )
                    {
                    // Release the call
                    iStateMachine->SetCallId( 
                        callStateData.CallId() );
                    iStateMachine->SendPhoneEngineMessage( 
                        MPEPhoneModel::EPEMessageRelease );
                    }
                else
                    {
                    callStateData.SetCallState( EPEStateDisconnecting );
                    iViewCommandHandle->HandleCommandL(
                        EPhoneViewGetCallIdByState, &callStateData );
                    
                     if ( callStateData.CallId() > KErrNotFound )
                        {    
                        // Release the call
                        iStateMachine->SetCallId( 
                            callStateData.CallId() );
                        iStateMachine->SendPhoneEngineMessage( 
                            MPEPhoneModel::EPEMessageRelease );   
                        }
                    }          
                }
            break;
            
        default:
            // do base operation
            CPhoneCallSetup::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }


// -----------------------------------------------------------
// CPhoneSingleAndCallSetup::HandleConnectingL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetup::HandleConnectingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetup::HandleConnectingL()");
    
    BeginUiUpdateLC();
    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveGlobalNote );
    
    // Re-enable global notes
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );

    iNumberEntryManager->RemoveNumberEntryIfVisibilityIsFalseL();

    UpdateCallHeaderAndUiCommandsL( aCallId );
    EndUiUpdate();
    
    // Go to alerting state
    iStateMachine->ChangeState( EPhoneStateAlertingInSingle );
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetup::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetup::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndCallSetup::HandleConnectedL()");    
    // Remove the number entry if it isn't DTMF dialer
    if ( !iNumberEntryManager->IsNumberEntryVisibleL() )
        {
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNumberEntry );
        }
    
    UpdateCallHeaderAndUiCommandsL( aCallId );
    
    iStateMachine->ChangeState( EPhoneStateTwoSingles );
    
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetup::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneSingleAndCallSetup::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleAndCallSetup::HandleIdleL()");
    
    BeginUiUpdateLC();
 
    // Remove call 
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );

    // Find out do we have single or outgoing call left
    TPhoneCmdParamInteger activeCallCount;
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewGetCountOfActiveCalls, &activeCallCount );
        
    if( activeCallCount.Integer() )
        {
        iNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue);
        UpdateUiCommands();
        iStateMachine->ChangeState( EPhoneStateSingle ); 
        }
    else
        {
        DisplayCallTerminationNoteL();

        // Single call was terminated
        UpdateUiCommands();
        SetToolbarButtonLoudspeakerEnabled();
        iStateMachine->ChangeState( EPhoneStateCallSetup );
        }
    EndUiUpdate();
    }

// -----------------------------------------------------------
// CPhoneSingleAndCallSetup::HandleCommandL
// -----------------------------------------------------------
//
TBool CPhoneSingleAndCallSetup::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleAndCallSetup::HandleCommandL()");
    TBool commandStatus = ETrue;

    switch( aCommand )
        {
        case EPhoneInCallCmdEndThisOutgoingCall:
            {
            // Fetch  call's id from view
            TPhoneCmdParamCallStateData callStateData;
            callStateData.SetCallState( EPEStateDialing );
            iViewCommandHandle->HandleCommandL(
                EPhoneViewGetCallIdByState, &callStateData );
                
            if ( callStateData.CallId() > KErrNotFound )
                {
                // Release the call
                iStateMachine->SetCallId( 
                    callStateData.CallId() );
                iStateMachine->SendPhoneEngineMessage( 
                    MPEPhoneModel::EPEMessageRelease );    
                }
            else
                {
                callStateData.SetCallState( EPEStateDisconnecting );
                iViewCommandHandle->HandleCommandL(
                    EPhoneViewGetCallIdByState, &callStateData );
                
                 if ( callStateData.CallId() > KErrNotFound )
                    {    
                    // Release the call
                    iStateMachine->SetCallId( 
                        callStateData.CallId() );
                    iStateMachine->SendPhoneEngineMessage( 
                        MPEPhoneModel::EPEMessageRelease );   
                    }
                __PHONELOG( EOnlyFatal, EPhoneUIStates, 
                    "CPhoneSingleAndCallSetup::HandleCommandL EPhoneInCallCmdEndThisOutgoingCall has negative call id!" );   
                }
            }
            break;
            
        default:
            commandStatus = CPhoneStateCallSetup::HandleCommandL( aCommand );
            break;
        }

    return commandStatus;
    }
 
// End of File

