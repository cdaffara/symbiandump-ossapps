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
* Description: Implementation of CPhoneStateIncoming class.
*
*/


// INCLUDES
#include <cntdef.h>
#include <e32base.h>
#include <eikenv.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <dundomainpskeys.h>
#include <telephonyvariant.hrh>
#include <mpeengineinfo.h>
#include <MediatorDomainUIDs.h>
#include <videotelcontrolmediatorapi.h>

#include "cphonepubsubproxy.h"
#include "phoneui.pan"
#include "cphonestateincoming.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamringtone.h"
#include "tphonecmdparamsfidata.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamdynmenu.h"
#include "mphonestatemachine.h"
#include "phonestatedefinitions.h"
#include "phoneui.hrh"
#include "phonerssbase.h"
#include "cphonemainresourceresolver.h"
#include "phonelogger.h"
#include "cphonecenrepproxy.h"
#include "cphonepubsubproxy.h"
#include "tphonecmdparamquery.h"
#include "cphonemediatorfactory.h"
#include "cphonemediatorsender.h"
#include "cphonereleasecommand.h"
#include "mphonecustomization.h"
#include "mphonestorage.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneStateIncoming::CPhoneStateIncoming(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization ) :
    CPhoneState( aStateMachine, aViewCommandHandle, aPhoneCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::~CPhoneStateIncoming()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneStateIncoming::~CPhoneStateIncoming()
    {
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateIncoming::ConstructL()
    {
    CPhoneState::BaseConstructL();
    
    // Fetch incoming call's id from view
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    if( callStateData.CallId() > KErrNotFound  )
        {
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveQuery );
        iRingingCallId = callStateData.CallId();
        }
    else
        {
        __PHONELOG( EOnlyFatal, EPhoneControl,
            "CPhoneStateIncoming::ConstructL - incoming call already in disconnecting state!");
        }
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateIncoming* CPhoneStateIncoming::NewL(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneStateIncoming* self = new (ELeave) CPhoneStateIncoming(
        aStateMachine, aViewCommandHandle, aPhoneCustomization );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandleKeyEventL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateIncoming::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::HandleKeyEventL ()" );
    CPhoneState::HandleKeyEventL( aKeyEvent, aEventCode );
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandleNumberEntryClearedL()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateIncoming::HandleNumberEntryClearedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::HandleNumberEntryClearedL ()" );
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandleKeyMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateIncoming::HandleKeyMessageL(
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND( EPhoneControl,
        "CPhoneStateIncoming::HandleKeyMessageL ()" );
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    switch ( aCode )
        {
        case EKeyNo: // end-key
            {
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
            else
                {
                // handle short end key
                DisconnectWaitingCallL();
                }
            }
            break;
            
        case EKeyYes: // send-key
            if( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                HandleSendL();
                }
            else
                {
                AnswerCallL();
                }
            break;

        case EKeyDevice3: // OK key
            if ( aMessage == EPhoneKeyLongPress )
                {
                AnswerCallL();
                }
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandleSendL
// -----------------------------------------------------------
//
void CPhoneStateIncoming::HandleSendL()
    {
    HBufC* phoneNumber = iNumberEntryManager->PhoneNumberFromEntryLC();
    TPtr ptr( phoneNumber->Des() );

    iStateMachine->PhoneEngineInfo()->SetPhoneNumber( ptr );
    iStateMachine->SendPhoneEngineMessage(
            MPEPhoneModel::EPEMessagePhoneNumberEdited );
    
    if ( phoneNumber->Des().Length() < KPhoneValidPhoneNumberLength 
            && iStateMachine->PhoneEngineInfo()->PhoneNumberIsServiceCode() ) 
        {
        // Send a manual control sequence by providing number
        // information with dial command
        iNumberEntryManager->CallFromNumberEntryL();
        }
    else
        {
        iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageCheckEmergencyNumber );
        }
    CleanupStack::PopAndDestroy( phoneNumber );
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateIncoming::HandlePhoneEngineMessageL(
    const TInt aMessage,
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::HandlePhoneEngineMessageL ()" );
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;

        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;
            
        case MEngineMonitor::EPEMessageDisconnecting:
            CPhoneState::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;

        case MEngineMonitor::EPEMessageInValidEmergencyNumber:
            AnswerCallL();
            break;

        case MEngineMonitor::EPEMessageValidEmergencyNumber:
            DialVoiceCallL();
            break;

        case MEngineMonitor::EPEMessageAnswering:
            iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );
            break;

        case MEngineMonitor::EPEMessageAudioOutputChanged:
            HandleAudioOutputChangedL();
            break;

        case MEngineMonitor::EPEMessageMuteRingingTone:
            // Silence the ringer. And stop vibrating, if it is active.
            iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );
            HandleAudioPlayStoppedL();
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageStopTonePlay );
            break;
         
        case MEngineMonitor::EPEMessageAvailableAudioOutputsChanged:
            HandleAudioAvailableOutputChangedL();
            // Do not update CBA's.
            break;
        
        case MEngineMonitor::EPEMessageRemotePartyInfoChanged:
            UpdateCallHeader( aCallId );
            break;
            
        default:
            CPhoneState::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneStateIncoming::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::HandleConnectedL ()" );
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );

    iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );

    BeginUiUpdateLC();
    UpdateSingleActiveCallL( aCallId );

    UpdateUiCommands();
    SetBackButtonActive(ETrue);
    EndUiUpdate();

    iStateMachine->ChangeState( EPhoneStateSingle );
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandleAudioPlayStoppedL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateIncoming::HandleAudioPlayStoppedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::HandleAudioPlayStoppedL ()" );
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneStateIncoming::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::HandleIdleL ()" );
    BeginUiUpdateLC();
    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );
    
    SetDefaultFlagsL();
    
     if ( !iNumberEntryManager->SetVisibilityIfNumberEntryUsedL( ETrue ) )
        {
        RemoveDialogsAndSendPhoneToBackgroundL();
        }
 
    DeleteTouchPaneButtons();
    SetBackButtonActive(ETrue);
    
    EndUiUpdate();
    iStateMachine->ChangeState( EPhoneStateIdle );
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::HandleCommandL
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneStateIncoming::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::HandleCommandL ()" );
    TBool commandStatus = ETrue;
    switch( aCommand )
        {
        case EPhoneCmdOptions:
            // Stop tone playing, if necessary.
            // And stop vibrating, if it is active.
            iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );
            break;

        case EPhoneCallComingCmdAnswer:
            AnswerCallL();
            break;

        case EPhoneCallComingCmdAnswerHandPortableVideo:
        case EPhoneCallComingCmdAnswerHandPortable:
            iStateMachine->PhoneEngineInfo()->SetAudioOutputCommand(
                EPEHandset, ETrue );
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageSetAudioOutput );
            AnswerCallL();
            break;

        case EPhoneCallComingCmdReject:
            DisconnectWaitingCallL();
            break;

        case EPhoneCallComingCmdSilent:
            // Silence the ringer. And stop vibrating, if it is active.
            iViewCommandHandle->ExecuteCommandL( EPhoneViewMuteRingTone );
            HandleAudioPlayStoppedL();
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageStopTonePlay );
            break;

        case EPhoneCallComingCmdSoftReject:
            OpenSoftRejectMessageL();
            break;

        case EPhoneNumberAcqCmdSendCommand:
            HandleSendL();
            break;

        default:
            commandStatus = CPhoneState::HandleCommandL( aCommand );
            break;
        }
    return commandStatus;
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::AnswerCallL
// -----------------------------------------------------------
//
void CPhoneStateIncoming::AnswerCallL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::AnswerCallL ()" );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewMuteRingToneOnAnswer );
    iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageAnswer );
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::DisconnectWaitingCallL
// -----------------------------------------------------------
//
void CPhoneStateIncoming::DisconnectWaitingCallL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::DisconnectWaitingCallL ()" );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );

    // The ringing call might have changed
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    
    if ( callStateData.CallId() > KErrNotFound )
        {
        iRingingCallId = callStateData.CallId();
        }
    
    iStateMachine->SetCallId( iRingingCallId );

    if( IsVideoCall( iRingingCallId ) )
        {
        // Video call can be released only after we get response 
        // to VT Shutdown Command.
        CPhoneMediatorFactory::Instance()->Sender()->IssueCommand( 
                KMediatorVideoTelephonyDomain,
                         KCatPhoneToVideotelCommands, 
                         EVtCmdReleaseDataport,
               TVersion( KPhoneToVideotelCmdVersionMajor,
                         KPhoneToVideotelCmdVersionMinor, 
                         KPhoneToVideotelCmdVersionBuild ),
               KNullDesC8,
               CPhoneReleaseCommand::NewL( *iStateMachine ) );
        }
    else
        {
        iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageRelease );
        }
    ShowDisconnectingL( iRingingCallId );
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::OpenSoftRejectMessageEditorL
// -----------------------------------------------------------
//
void CPhoneStateIncoming::OpenSoftRejectMessageL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneStateIncoming::OpenSoftRejectMessageEditorL ()" );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );
    iStateMachine->SendPhoneEngineMessage(
        MPEPhoneModel::EPEMessageStopTonePlay );
    
    iUiCommandManager->SetSoftRejectStatus( EFalse );
    
    UpdateUiCommands();

    TPhoneCmdParamSfiData sfiDataParam;
    if ( iStateMachine->PhoneEngineInfo()->RemoteName( iRingingCallId ).Length() )
        {
        // store both the name and the number
        sfiDataParam.SetNumber( iStateMachine->PhoneEngineInfo()->RemotePhoneNumber( iRingingCallId ) );
        sfiDataParam.SetName( iStateMachine->PhoneEngineInfo()->RemoteName( iRingingCallId ) );
        }
    else
        {
        // store the number
        sfiDataParam.SetNumber( iStateMachine->PhoneEngineInfo()->RemotePhoneNumber( iRingingCallId ) );
        }
    iViewCommandHandle->ExecuteCommandL( EPhoneViewOpenSoftRejectEditor, &sfiDataParam );
    }


// -----------------------------------------------------------
// CPhoneStateIncoming::GetNumberEntryVisibleMenuBar
// -----------------------------------------------------------
//
TInt CPhoneStateIncoming::GetNumberEntryVisibleMenuBar()
    {
    TInt resource(EPhoneIncomingCallMenubarWithNumberEntry);
    if( IsVideoCall ( iRingingCallId ) )
        {
        resource = EPhoneIncomingVideoCallMenubarWithNumberEntry;
        }
    return resource;
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::GetNumberEntryNotVisibleMenuBar
// -----------------------------------------------------------
//
TInt CPhoneStateIncoming::GetNumberEntryNotVisibleMenuBar()
    {
    TInt resource(EPhoneIncomingCallMenubar);
    if( IsVideoCall ( iRingingCallId ) )
        {
        resource =  EPhoneIncomingVideoCallMenubar;
        }
    return resource;
    }

// -----------------------------------------------------------
// CPhoneStateIncoming::ShowDisconnectingL
// -----------------------------------------------------------
//
void CPhoneStateIncoming::ShowDisconnectingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateIncoming::ShowDisconnectingL( ) ");

    iViewCommandHandle->ExecuteCommandL( EPhoneViewUpdateBubble, aCallId);
    }


// End of File
