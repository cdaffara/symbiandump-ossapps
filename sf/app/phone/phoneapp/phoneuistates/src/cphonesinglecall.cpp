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
* Description: Implementation of CPhoneSingleCall class.
*
*/


// INCLUDES
#include <eikmenub.h> 
#include <eikenv.h>
#include <StringLoader.h>
#include <featmgr.h>
#include <pevirtualengine.h>
#include <mpeengineinfo.h>
#include <cpephonemodelif.h>
#include <connect/sbdefs.h>
#include <videotelcontrolmediatorapi.h>
#include <MediatorDomainUIDs.h>
#include <telephonyvariant.hrh>

#include "cphonesinglecall.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamcallheaderdata.h"
#include "mphonestatemachine.h"
#include "tphonecmdparaminteger.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "cphonemainresourceresolver.h"
#include "phonerssbase.h"
#include "phonestatedefinitionsgsm.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamcallstatedata.h"
#include "phonelogger.h"
#include "cphonepubsubproxy.h"
#include "cphonemediatorfactory.h"
#include "cphonemediatorsender.h"
#include "mphonestorage.h"
#include "cphonecenrepproxy.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneSingleCall::CPhoneSingleCall( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) : 
    CPhoneGsmInCall( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneSingleCall::~CPhoneSingleCall()
// -----------------------------------------------------------
//
EXPORT_C CPhoneSingleCall::~CPhoneSingleCall()
    {
    }

// -----------------------------------------------------------
// CPhoneSingleCall::ConstructL()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneSingleCall::ConstructL()
    {
    CPhoneGsmInCall::ConstructL();
    
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
    
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateConnected );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    
    // No connected call, 
    if( callStateData.CallId() == KErrNotFound )
        {
        //find the held call.
        callStateData.SetCallState( EPEStateHeld );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );
        // No held call.
        if ( callStateData.CallId() == KErrNotFound ) 
            {
            // find the disconnected call.
            callStateData.SetCallState( EPEStateDisconnecting );
            iViewCommandHandle->HandleCommandL(
                    EPhoneViewGetCallIdByState, &callStateData );    
            }
        }
    iCallId = callStateData.CallId();      
    }

// -----------------------------------------------------------
// CPhoneSingleCall::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneSingleCall* CPhoneSingleCall::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneSingleCall* self = new( ELeave ) CPhoneSingleCall( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// --------------------------------------------------------------
// CPhoneSingleCall::HandleKeyMessageL
// --------------------------------------------------------------
//
EXPORT_C void CPhoneSingleCall::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleCall::HandleKeyMessageL()");
    switch ( aCode )
        {
        case EKeyYes: // send-key
            if( iNumberEntryManager->IsNumberEntryUsedL() )
                {
                if ( IsVideoCallActiveL() )
                    {
                    HBufC *phoneNumber = HBufC::NewLC( KPhoneNumberEntryBufferSize );
                    TPtr ptr( phoneNumber->Des() );
                    TPhoneCmdParamString stringParam;
                    stringParam.SetString( &ptr );
                    iViewCommandHandle->ExecuteCommand(
                        EPhoneViewGetNumberFromEntry,
                        &stringParam );
                    iStateMachine->PhoneEngineInfo()->SetPhoneNumber( ptr ) ;
                                            
                    if (  iStateMachine->PhoneEngineInfo()->PhoneNumberIsServiceCode() ||
                          phoneNumber->Des().Length() < KPhoneValidPhoneNumberLength )
                        {
                        iNumberEntryManager->CallFromNumberEntryL();
                        }
                    else
                        {
                        iStateMachine->SendPhoneEngineMessage( 
                            MPEPhoneModel::EPEMessageCheckEmergencyNumber );
                        }
                    CleanupStack::PopAndDestroy( phoneNumber ); 
                    }
                else
                    {
                    iNumberEntryManager->CallFromNumberEntryL();
                    }
                }
            else
                {
                ToggleHoldL();
                }
            break;
            
        default:
            CPhoneGsmInCall::HandleKeyMessageL( aMessage, aCode );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneSingleCall::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneSingleCall::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleCall::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageHeld:
        case MEngineMonitor::EPEMessageConnected:
            UpdateCallHeaderAndUiCommandsL( aCallId );
            break;

        case MEngineMonitor::EPEMessageIncoming:
            HandleIncomingL( aCallId );
            break;
                
        case MEngineMonitor::EPEMessageDialing:
            HandleDialingL( aCallId );
            break;
        
        /* Flow through */ 
        case MEngineMonitor::EPEMessageRemoteTerminated:  
        case MEngineMonitor::EPEMessageDisconnecting:
            iViewCommandHandle->ExecuteCommandL( EPhoneViewHideNaviPaneAudioVolume );
            CPhoneGsmInCall::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        
        case MEngineMonitor::EPEMessageInValidEmergencyNumber:
            SendGlobalErrorNoteL( EPhoneNoteTextNotAllowed, ETrue );
            break;
            
        case MEngineMonitor::EPEMessageValidEmergencyNumber:
            DialVoiceCallL();
            break;
            
        default:
            CPhoneGsmInCall::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneSingleCall::HandleCommandL
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneSingleCall::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, "CPhoneSingleCall::HandleCommandL()");
    TBool commandStatus = ETrue;
    switch( aCommand )
        {
        /* Flow through */           
        case EPhoneInCallCmdHold:
        case EPhoneInCallCmdUnhold:
            ToggleHoldL();
            break;
        
        /* Flow through */ 
        case EPhoneInCallCmdNewCall:
            break;        
                
        
        case EPhoneVideoSharing:
        case EPhoneVideoSharingOpt2:
        case EPhoneVideoSharingOpt3:
            {
            TPhoneCmdParamInteger command;
            command.SetInteger( aCommand );
            iViewCommandHandle->ExecuteCommandL( 
                    EPhoneViewSendAiwCommand, 
                    &command );
            }
            break;
            
        default:
            commandStatus = CPhoneGsmInCall::HandleCommandL( aCommand );
            break;
        }
    return commandStatus;
    }

// -----------------------------------------------------------
// CPhoneSingleCall::CallId
// -----------------------------------------------------------
//
EXPORT_C TInt CPhoneSingleCall::CallId() const
    {
    return iCallId;
    }
    
// -----------------------------------------------------------
// CPhoneSingleCall::ToggleHold
// -----------------------------------------------------------
//
void CPhoneSingleCall::ToggleHoldL()
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleCall::ToggleHoldL()");

    TPECallControlCaps callControlCaps =
        iStateMachine->PhoneEngineInfo()->CallControlCaps( iCallId );
    
    if ( callControlCaps & MCCECallObserver::ECCECapsHold  )
        {
        iStateMachine->SetCallId( iCallId );         
        iStateMachine->SendPhoneEngineMessage( 
            CPEPhoneModelIF::EPEMessageHold );    
        }
    else if ( callControlCaps & MCCECallObserver::ECCECapsResume )
        {
        iStateMachine->SetCallId( iCallId );         
        iStateMachine->SendPhoneEngineMessage( 
            CPEPhoneModelIF::EPEMessageResume );    
        }
    else
        {
        SendGlobalInfoNoteL( EPhoneNoteTextNotAllowed, ETrue );    
        }
    }
        
// -----------------------------------------------------------
// CPhoneSingleCall::HandleIncomingL
// -----------------------------------------------------------
//
void CPhoneSingleCall::HandleIncomingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleCall::HandleIncomingL()");
    DisplayCallHeaderL( aCallId, ECheckIfNEUsedBeforeSettingVisibilityFalse );
    iStateMachine->ChangeState( EPhoneStateWaitingInSingle );
    }

// -----------------------------------------------------------
// CPhoneSingleCall::HandleDialingL
// -----------------------------------------------------------
//
void CPhoneSingleCall::HandleDialingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneUIStates, 
        "CPhoneSingleCall::HandleDialingL()");
    DisplayCallHeaderL( aCallId, ESetNEVisibilityFalse);
    iStateMachine->ChangeState( EPhoneStateCallSetupInSingle );
    }

// End of File
