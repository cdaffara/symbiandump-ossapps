/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Implementation of CPhoneStateInCall class.
*
*/


// INCLUDES
#include <featmgr.h>
#include <StringLoader.h>
#include <PSVariables.h>
#include <hwrmdomainpskeys.h>
#include <mpeengineinfo.h>
#include <videotelcontrolmediatorapi.h>
#include <MediatorDomainUIDs.h>
#include <bldvariant.hrh>

#include "cphonestateincall.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamnote.h"
#include "tphonecmdparamquery.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamkeycapture.h"
#include "tphonecmdparamsfidata.h"
#include "mphonestatemachine.h"
#include "phonestatedefinitions.h"
#include "phoneviewcommanddefinitions.h"
#include "phoneui.hrh"
#include "phonerssbase.h"
#include "cphonemainresourceresolver.h"
#include "phonelogger.h"
#include "phoneui.pan"
#include "cphonepubsubproxy.h"
#include "cphonedtmfwaitchartimer.h"
#include "tphonecmdparamcallheaderdata.h"
#include "cphonemediatorfactory.h"
#include "cphonemediatorsender.h"
#include "cphoneterminateallconnectionscommand.h"
#include "mphonecustomization.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneStateInCall::CPhoneStateInCall( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aCustomization ) : 
    CPhoneState( aStateMachine, aViewCommandHandle, aCustomization ),
    iDtmfWaitCharTimer( NULL )
    {
    }

// -----------------------------------------------------------
// CPhoneStateInCall::~CPhoneStateInCall()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneStateInCall::~CPhoneStateInCall()
    {
    if( iDtmfWaitCharTimer )
        {
        delete iDtmfWaitCharTimer;
        }
    }

// -----------------------------------------------------------
// CPhoneStateInCall::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateInCall::ConstructL()
    {
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    CPhoneState::BaseConstructL();
    }

// -----------------------------------------------------------
// CPhoneStateInCall::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateInCall* CPhoneStateInCall::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneStateInCall* self = new (ELeave) CPhoneStateInCall( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStateInCall::HandleKeyMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateInCall::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::HandleKeyMessageL()" ); 
    switch ( aCode )
        {
        case EKeyNo: // end-key
            HandleEndKeyPressL( aMessage );
            break;

        case EKeyDevice6: // Voice key
            HandleVoiceKeyPressL( aMessage ); 
            break;
            
#ifdef RD_INTELLIGENT_TEXT_INPUT
        case EKeyEnter:
            if ( iNumberEntryManager->IsNumberEntryVisibleL() )
                {
                HandleCommandL( EPhoneCmdOptions );
                }
            break;
#endif            
        default:
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateInCall::HandleNumberEntryClearedL()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateInCall::HandleNumberEntryClearedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::HandleNumberEntryClearedL()" );
    // Update single call CBA when number entry is cleared
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneStateInCall::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateInCall::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::HandlePhoneEngineMessageL()" );
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;

        case MEngineMonitor::EPEMessageAudioMuteChanged:
            HandleAudioMuteChangedL();
            break;
            
        case MEngineMonitor::EPEMessageAudioOutputChanged:
            HandleAudioOutputChangedL();
            break;
        
        case MEngineMonitor::EPEMessageSendingDTMF:
        case MEngineMonitor::EPEMessageContinuingDTMFSending:
            HandleSendingDTMFL();
            break;

        case MEngineMonitor::EPEMessageStoppedDTMF:
            HandleStoppedDTMFL();
            break;

        case MEngineMonitor::EPEMessageSentDTMF:
        case MEngineMonitor::EPEMessageDTMFSendingAborted:
            CancelDTMFSendingL();
            break;
            
        case MEngineMonitor::EPEMessageAudioVolumeChanged:
            HandleAudioVolumeChangedL();
            break;
            
        case MEngineMonitor::EPEMessageDisconnecting:
            CancelDTMFSendingL();
            CPhoneState::HandlePhoneEngineMessageL( aMessage, aCallId );
            break; 
            
        case MEngineMonitor::EPEMessageInValidEmergencyNumber:
            SendGlobalErrorNoteL( EPhoneNoteTextNotAllowed, ETrue );
            break;
            
        case MEngineMonitor::EPEMessageValidEmergencyNumber:
            DialVoiceCallL();
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
// CPhoneStateInCall::HandleIdleL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateInCall::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateInCall::HandleIdleL()" );
    __ASSERT_DEBUG( aCallId >= 0, Panic( EPhoneCtrlParameterNotInitialized ) );
    TBool showDialer( EFalse );
    HBufC *phoneNumber = HBufC::NewLC( KPhoneNumberEntryBufferSize );
    TPtr ptr( phoneNumber->Des() );
    TPhoneCmdParamString stringParam;
    stringParam.SetString( &ptr );

    
    BeginUiUpdateLC();
    SetDefaultFlagsL();
	iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewHideToolbar );
    if ( iNumberEntryManager->IsNumberEntryUsedL() )
        {
        iViewCommandHandle->ExecuteCommand(
                EPhoneViewGetNumberFromEntry,
                &stringParam );
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNumberEntry );
        showDialer = ETrue;
        }
    
    if ( IsAutoLockOn() || !showDialer )
        {
        RemoveDialogsAndSendPhoneToBackgroundL();
        }
    else if ( showDialer )
        {
        iViewCommandHandle->ExecuteCommandL( EPhoneViewLaunchLogs, &stringParam );
        }
        
    DeleteTouchPaneButtons();
    EndUiUpdate();
    DisplayCallTerminationNoteL();

    TPhoneCmdParamKeyCapture captureParam;
    captureParam.SetKeyCode( EKeyNo );
    iViewCommandHandle->ExecuteCommand( EPhoneViewStopCapturingKey, &captureParam );
    CleanupStack::PopAndDestroy( phoneNumber );
    iStateMachine->ChangeState( EPhoneStateIdle );
    }

// -----------------------------------------------------------
// CPhoneStateInCall::HandleAudioMuteChangedL
// -----------------------------------------------------------
//
void CPhoneStateInCall::HandleAudioMuteChangedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::HandleAudioMuteChangedL()" );
    CPhoneState::HandleAudioMuteChangedL();
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneStateInCall::HandleAudioOutputChangedL
// -----------------------------------------------------------
//
void CPhoneStateInCall::HandleAudioOutputChangedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::HandleAudioOutputChangedL()" );
    CPhoneState::HandleAudioOutputChangedL();    
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneStateInCall::HandleAudioVolumeChangedL
// -----------------------------------------------------------
//
void CPhoneStateInCall::HandleAudioVolumeChangedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::HandleAudioVolumeChangedL()" );
    TInt audioVolume = iStateMachine->PhoneEngineInfo()->AudioVolume();
    TPhoneCmdParamInteger volumeParam;
    volumeParam.SetInteger( audioVolume );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetNaviPaneAudioVolume,
        &volumeParam );
    }
    
// -----------------------------------------------------------
// CPhoneStateInCall::HandleSendingDTMFL
// -----------------------------------------------------------
//
void CPhoneStateInCall::HandleSendingDTMFL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::HandleSendingDTMFL()" );
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNoteDtmfSending );
    noteParam.SetResourceId( CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( EPhoneSendingDtmfWaitNote ) );
    
    // Show the "Sending..." Wait Note, and pre-populate it 
    // with the PhoneEngine's current DTMF String.
    // Set the text for the wait-note.
    TPEDtmfString dtmfString = iStateMachine->PhoneEngineInfo()->DtmfString();
    HBufC* noteText = StringLoader::LoadLC( 
        CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( EPhoneSendingDtmfWaitNoteText ), 
        dtmfString );
    noteParam.SetText( *noteText );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewShowNote, &noteParam );
    CleanupStack::PopAndDestroy( noteText );
    }

// -----------------------------------------------------------
// CPhoneStateInCall::HandleStoppedDTMFL
// -----------------------------------------------------------
//
void CPhoneStateInCall::HandleStoppedDTMFL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::HandleStoppedDTMFL()" );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNote );
    // Fetch the remaining (unparsed) portion of the DTMF String 
    // from PhoneEngine
    TPEDtmfString dtmfString = iStateMachine->PhoneEngineInfo()->DtmfString();

    if ( dtmfString.Length() )
        {
        // Show the DTMF Wait Character confirmation query
        TPhoneCmdParamQuery queryParam;
        queryParam.SetQueryType( EPhoneQueryDialog );
        queryParam.SetQueryResourceId( 
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( 
                EPhoneDtmfWaitCharacterConfirmationQuery ) );

        // Append the PhoneEngine's current DTMF String to the query's prompt
        HBufC* queryPrompt = StringLoader::LoadLC( 
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( 
                EPhoneDtmfWaitCharacterConfirmationQueryText ), 
            dtmfString );
        queryParam.SetQueryPrompt( *queryPrompt );
        iViewCommandHandle->ExecuteCommandL( EPhoneViewShowQuery, 
            &queryParam );
        CleanupStack::PopAndDestroy( queryPrompt );
        }
    else
        {
        // Stop the asynchronous sending operation the 
        // PhoneEngine may be performing.
        iStateMachine->SendPhoneEngineMessage( 
            MPEPhoneModel::EPEMessageStopDTMFSending );

        // Prompt for more characters by showing DTMF query with empty string
        HBufC* emptyString = HBufC::NewLC( KPhoneNumberEntryBufferSize );
        TPtr ptr( emptyString->Des() );
        ShowDtmfTextQueryL( 
            CPhoneMainResourceResolver::Instance()->
                ResolveResourceID( EPhoneDtmfNumberQuery ),
            CPhoneMainResourceResolver::Instance()->
                ResolveResourceID( EPhoneSendDtmfEmptyEditBoxCBA ),
            CPhoneMainResourceResolver::Instance()->
                ResolveResourceID( EPhoneSendDtmfNormalEditBoxCBA ),
            &ptr );
        CleanupStack::PopAndDestroy( emptyString );
        }
    }

// -----------------------------------------------------------
// CPhoneStateInCall::CancelDTMFSendingL
// -----------------------------------------------------------
//
void CPhoneStateInCall::CancelDTMFSendingL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::CancelDTMFSendingL()" );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveDtmfNote );
    }

// -----------------------------------------------------------
// CPhoneStateInCall::HandleCommandL
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneStateInCall::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneStateInCall::HandleCommandL()" );
    __PHONELOG1 ( EBasic, EPhoneControl,
        "CPhoneStateInCall::HandleCommandL() - aCommand  = %d ", aCommand );
    TBool commandStatus = ETrue;
    switch( aCommand )
        {
        case EPhoneInCallCmdDialer:
            iViewCommandHandle->ExecuteCommandL( EPhoneViewOpenDialer );
            break;
            
        case EPhoneDtmfDialerCancel:
            {
            // Activate DTMF list query when cancel is pressed.
            // Forwards command to the framework 
            HandleCommandL( EPhoneInCallCmdDtmfListQuery );
            }
            break;
            
        case EPhoneInCallCmdEndThisActiveCall:
            CPhoneState::DisconnectCallL();
            break;
            
        case EPhoneCallComingCmdReject: // fall through
        case EPhoneInCallCmdReject:
            iStateMachine->SendPhoneEngineMessage( 
                MPEPhoneModel::EPEMessageReject );
            break;
                     
        case EPhoneInCallCmdMute: // fall through
        case EPhoneInCallCmdUnmute:
            iStateMachine->PhoneEngineInfo()->SetAudioMuteCommand( 
                ( aCommand == EPhoneInCallCmdMute ) );
            iStateMachine->SendPhoneEngineMessage( 
                MPEPhoneModel::EPEMessageSetAudioMute );
            break;
            
        //DTMF list query - dialog   
        case EPhoneInCallCmdDtmfListQuery:
            LaunchDtmfListQueryL();
            break;
            
        // DTMF entry - Ok          
        case EPhoneCmdDtmfOk:
            SendDtmfL();
            break;

        // DTMF sending - Cancel
        case EPhoneInCallCmdCancelSendingDtmfString:
            // Stop the asynchronous sending operation the 
            // PhoneEngine may be performing.
            iStateMachine->SendPhoneEngineMessage( 
                MPEPhoneModel::EPEMessageStopDTMFSending );
            // Remove DTMF sending from screen
            iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNote );
            break;    
            
        // DTMF wait - Ok
        case EPhoneInCallCmdContinueSendingDtmfString:
            // Continue sending the DTMF string
            if( !iDtmfWaitCharTimer )
                {
                iDtmfWaitCharTimer = CPhoneDtmfWaitCharTimer::NewL( 
                    iStateMachine );                    
                }
            iDtmfWaitCharTimer->ButtonPressedL();
            break;
            
        case EPhoneNumberAcqCmdCall:
        case EPhoneNumberAcqCmdSendCommand:
            if ( IsVideoCallActiveL() )
                {
                // Get the number entry contents
                HBufC *phoneNumber = HBufC::NewLC( KPhoneNumberEntryBufferSize );
                TPtr ptr( phoneNumber->Des() );
                TPhoneCmdParamString stringParam;
                stringParam.SetString( &ptr );
                iViewCommandHandle->ExecuteCommand(
                    EPhoneViewGetNumberFromEntry,
                    &stringParam );
                iStateMachine->PhoneEngineInfo()->SetPhoneNumber( ptr );
                                        
                if ( iStateMachine->PhoneEngineInfo()->PhoneNumberIsServiceCode() ||
                     phoneNumber->Des().Length() < KPhoneValidPhoneNumberLength )
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
            else
                {
                iNumberEntryManager->CallFromNumberEntryL();
                }
            break;

        case EPhoneCmdEnd:
            CPhoneState::DisconnectCallL();
            break;
            
        // 'End all calls' from menu
        case EPhoneInCallCmdEndAllCalls:
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageReleaseAll );
            break;
            
        case EPhoneInCallCmdEndThisOutgoingCall:
            DisconnectOutgoingCallL();
            break;

        case EPhoneInCallCmdGoToIdle:
            iViewCommandHandle->ExecuteCommandL( EPhoneViewBringIdleToForeground );
            break;
            
        case EPhoneNumberAcqCmdVideoCall:
            {// Create normal voice call, if number is emergency number.
            // Get the number entry contents
            HBufC* phoneNumber = iNumberEntryManager->PhoneNumberFromEntryLC();
            iStateMachine->PhoneEngineInfo()->SetPhoneNumber( *phoneNumber );
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageCheckEmergencyNumber );
            CleanupStack::PopAndDestroy( phoneNumber );}
            break;

       case EPhoneViewOpenCallHandling:
            if ( iNumberEntryManager->IsNumberEntryUsedL() )
                {
                iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNumberEntry );
                HandleNumberEntryClearedL();
                }
            commandStatus = CPhoneState::HandleCommandL( aCommand );
            break;
            
       case EPhoneInCallCmdNewCall:
       case EPhoneInCallCmdDtmfManualQuery:
       case EPhoneDtmfDialerExit:
       case EPhoneCmdOptions:
           break;
 
       default:
            commandStatus = CPhoneState::HandleCommandL( aCommand );
            break;
        }
    return commandStatus;
    }

// -----------------------------------------------------------
// CPhoneStateInCall::CallFetchedNumberL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateInCall::CallFetchedNumberL( 
    const TDesC& aFetchedNumber )
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::CallFetchedNumberL()" );
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    // Store the phone number
    iStateMachine->PhoneEngineInfo()->SetPhoneNumber( aFetchedNumber );
    DialVoiceCallL();
    }

// -----------------------------------------------------------
// CPhoneStateInCall::SendDtmfL
// -----------------------------------------------------------
//
void CPhoneStateInCall::SendDtmfL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::SendDtmfL()" );
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );    
    // First get the DTMF sequence from dialog
    TPhoneCmdParamString dtmfSequence;
    HBufC *content = HBufC::NewLC( KPEDtmfMaxLength );
    TPtr ptr( content->Des() );
    dtmfSequence.SetString( &ptr );
    iViewCommandHandle->ExecuteCommand(
        EPhoneViewGetNumberFromEntry,
        &dtmfSequence );

    // Send the DTMF
    if ( ptr.Length() )
        {
        iStateMachine->PhoneEngineInfo()->SetDtmfStringCommand( ptr );
        iStateMachine->SendPhoneEngineMessage(
            MPEPhoneModel::EPEMessageSendDTMF );
        }
    CleanupStack::PopAndDestroy( content );
    }

// -----------------------------------------------------------
// CPhoneStateInCall::LaunchDtmfListQueryL
// -----------------------------------------------------------
//
void CPhoneStateInCall::LaunchDtmfListQueryL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::LaunchDtmfListQueryL()" );
    
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateConnected );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
   
    if ( callStateData.CallId() == KErrNotFound )
        {
        callStateData.SetCallState( EPEStateConnecting );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );    
        }
        
    if ( callStateData.CallId() > KErrNotFound )
        {
        //Get Array of DTMF strings from PhoneEngine
        const CDesCArray& dtmfArray = 
        iStateMachine->PhoneEngineInfo()->
        RemotePredefinedDtmfStrings( callStateData.CallId() );
        TInt itemCount = dtmfArray.Count();  
        if ( itemCount )
            {
            TPhoneCmdParamString stringParam;
            //Go through the array and send each dtmf string
            //to view.
            for ( TInt i=0; i<itemCount; i++ )
                {
                TPtrC16 ptrC16 = dtmfArray.MdcaPoint( i );
                HBufC* string = ptrC16.AllocL();
                CleanupStack::PushL( string );
                TPtr ptr( string->Des() );
                
                stringParam.SetString( &ptr ); 
                iViewCommandHandle->ExecuteCommandL(
                    EPhoneViewSetListQueryString,
                    &stringParam );
                CleanupStack::PopAndDestroy( string );
                string = NULL;
                }
            }
        }
    else
        {
        __PHONELOG( EOnlyFatal, EPhoneControl, 
            "CPhoneStateInCall::LaunchDtmfListQueryL() No found valid call id" );
        }
    // if there is a connected call only then open DtmfListQuery.
    if ( IsAnyConnectedCalls() )
       {
       TPhoneCmdParamQuery queryDialogParam;
       queryDialogParam.SetQueryType( EPhoneDtmfListQueryDialog );
       queryDialogParam.SetQueryResourceId(
               CPhoneMainResourceResolver::Instance()->
               ResolveResourceID( EPhoneDtmfNumberListQuery ) );
       iViewCommandHandle->ExecuteCommandL( EPhoneViewShowQuery,
           &queryDialogParam );
       }
    }   
    
// ---------------------------------------------------------
// CPhoneStateInCall::IsVideoCallActiveL
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneStateInCall::IsVideoCallActiveL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::IsVideoCallActive()" );
    TBool retVal = EFalse;
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateConnected );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    if ( callStateData.CallId() > KErrNotFound )
        {
        retVal = IsVideoCall( callStateData.CallId() );
        }
    return retVal;
    }
    
// ---------------------------------------------------------
// CPhoneStateInCall::IsVideoCallRingingL
// ---------------------------------------------------------
//

EXPORT_C TBool CPhoneStateInCall::IsVideoCallRingingL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::IsVideoCallRingingL()" );
    TBool retVal = EFalse;
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    if ( callStateData.CallId() > KErrNotFound )
        {
        retVal = IsVideoCall( callStateData.CallId() );
        }
    return retVal;
    }

// ---------------------------------------------------------
// CPhoneStateInCall::GetRingingCallL
// ---------------------------------------------------------
//

EXPORT_C TInt CPhoneStateInCall::GetRingingCallL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateInCall::GetRingingCallL()" );
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    return callStateData.CallId();
    }

// -----------------------------------------------------------
// CPhoneStateInCall::DisconnectOutgoingCallL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateInCall::DisconnectOutgoingCallL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, 
        "CPhoneStateInCall::DisconnectOutgoingCallL()");
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateConnecting );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );
    if( callStateData.CallId() == KErrNotFound )
        {
        // No connecting call, find the dialing call
        callStateData.SetCallState( EPEStateDialing );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );
        
        if( callStateData.CallId() == KErrNotFound )
            {
            // No dialing call, find the disconnectinging call
            callStateData.SetCallState( EPEStateDisconnecting );
            iViewCommandHandle->HandleCommandL(
                EPhoneViewGetCallIdByState, &callStateData );
            }
        else
            {
            iNumberEntryManager->SetVisibilityIfNumberEntryUsedL(ETrue);
            }
        }

    if( callStateData.CallId() > KErrNotFound )
        {
        iStateMachine->SetCallId( callStateData.CallId() );
        iStateMachine->SendPhoneEngineMessage( 
            MPEPhoneModel::EPEMessageRelease );
        }
     else
        {
        __PHONELOG( EOnlyFatal, EPhoneControl, 
            "CPhoneStateInCall::DisconnectOutgoingCallL() has negative call id!" );   
        __ASSERT_DEBUG( EFalse, Panic( EPhoneCtrlIndexOutOfBounds ) );
        }       
    }

// -----------------------------------------------------------------------------
// CPhoneStateInCall::HandleEndKeyPressL
// -----------------------------------------------------------------------------
//    
void CPhoneStateInCall::HandleEndKeyPressL( TPhoneKeyEventMessages aMessage )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateInCall::HandleEndKeyPressL()");
    if ( aMessage == EPhoneKeyLongPress )
        {
        TPhoneCmdParamCallStateData callStateData;
        callStateData.SetCallState( EPEStateConnected );
        iViewCommandHandle->HandleCommandL(
            EPhoneViewGetCallIdByState, &callStateData );

        if( IsVideoCall( callStateData.CallId() ) )
            {
            // Video call can be released only after we get response to VT Shutdown Command 
            CPhoneMediatorFactory::Instance()->Sender()->IssueCommand( 
                KMediatorVideoTelephonyDomain,
                KCatPhoneToVideotelCommands, 
                EVtCmdReleaseDataport,
                TVersion( KPhoneToVideotelCmdVersionMajor,
                    KPhoneToVideotelCmdVersionMinor, 
                    KPhoneToVideotelCmdVersionBuild ),
                KNullDesC8,
                CPhoneTerminateAllConnectionsCommand::NewL( *iStateMachine ) );
            }
        else
            {
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageTerminateAllConnections );
            iNumberEntryManager->ClearNumberEntryContentCacheIfContentStored();
            }

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
        CPhoneState::DisconnectCallL();
        }
    }

// -----------------------------------------------------------------------------
// CPhoneStateInCall::HandleVoiceKeyPressL
// -----------------------------------------------------------------------------
//    
void CPhoneStateInCall::HandleVoiceKeyPressL( TPhoneKeyEventMessages aMessage )
    {
    __LOGMETHODSTARTEND(EPhoneControl, 
        "CPhoneStateInCall::HandleVoiceKeyPressL()");
    if ( aMessage == EPhoneKeyShortPress )
        {
        const TBool handsfreeMode = iStateMachine->
                PhoneEngineInfo()->AudioOutput() == EPELoudspeaker;
        SetHandsfreeModeL( !handsfreeMode );
        }
    else // aMessage == EPhoneKeyLongPress
        {
        SendGlobalInfoNoteL( EPhoneCallInProgress, ETrue );
        }
    }

// -----------------------------------------------------------
// CPhoneStateInCall::ShowDtmfTextQueryL
// -----------------------------------------------------------
//
void CPhoneStateInCall::ShowDtmfTextQueryL(
    TInt aDialogResourceId,
    TInt aDefaultCbaResourceId,
    TInt aContentCbaResourceId,
    TDes* aDataText,
    TBool aSendKeyEnabled )
    {
    __LOGMETHODSTARTEND(EPhoneControl, 
        "CPhoneStateInCall::ShowDtmfTextQueryL()");
    __ASSERT_DEBUG( aDialogResourceId &&
        aDefaultCbaResourceId &&
        aContentCbaResourceId &&
        aDataText, 
        Panic( EPhoneCtrlParameterNotInitialized ) );
    TPhoneCmdParamQuery queryDialogParam;
    queryDialogParam.SetQueryType( EPhoneDtmfTextQuery );
    queryDialogParam.SetQueryResourceId( aDialogResourceId );
    queryDialogParam.SetDefaultCba( aDefaultCbaResourceId );
    queryDialogParam.SetContentCba( aContentCbaResourceId );
    queryDialogParam.SetDataText( aDataText );
    queryDialogParam.SetSendKeyEnabled( aSendKeyEnabled );
    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewShowQuery,
        &queryDialogParam );
    }
  
// End of File
