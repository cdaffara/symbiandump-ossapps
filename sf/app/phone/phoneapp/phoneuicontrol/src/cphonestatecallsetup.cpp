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
* Description: Implementation of CPhoneStateCallSetup class.
*
*/


// INCLUDES
#include <featmgr.h>
#include <StringLoader.h>
#include <mpeengineinfo.h>
#include <mpeclientinformation.h>

#include "cphonepubsubproxy.h"
#include "cphonestatecallsetup.h"
#include "cphonestateincall.h"
#include "mphonestatemachine.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamkeyevent.h"
#include "tphonecmdparamnote.h"
#include "tphonecmdparamquery.h"
#include "phonestatedefinitions.h"
#include "phoneui.hrh"
#include "phonerssbase.h"
#include "cphonemainresourceresolver.h"
#include "phonelogger.h"
#include "phoneui.pan"
#include "cphonedtmfwaitchartimer.h"
#include "tphonecmdparamaudiooutput.h"
#include "cphonekeys.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPhoneStateCallSetup::CPhoneStateCallSetup( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aCustomization ) : 
    CPhoneState( aStateMachine, aViewCommandHandle, aCustomization )
    {
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::~CPhoneStateCallSetup()
// Destructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C CPhoneStateCallSetup::~CPhoneStateCallSetup()
    {
    delete iDtmfWaitCharTimer;    
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::ConstructL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::ConstructL()
    {
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    
    CPhoneState::BaseConstructL();

    HandleAudioOutputChangedL();
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::NewL()
// Constructor
// (other items were commented in a header).
// -----------------------------------------------------------
//
CPhoneStateCallSetup* CPhoneStateCallSetup::NewL( 
    MPhoneStateMachine* aStateMachine, 
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aPhoneCustomization )
    {
    CPhoneStateCallSetup* self = new (ELeave) CPhoneStateCallSetup( 
        aStateMachine, aViewCommandHandle, aPhoneCustomization );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    
    return self;
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleCommandL()
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneStateCallSetup::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND( EPhoneControl,  
        "CPhoneStateCallSetup::HandleCommandL()" );
    TBool commandStatus = ETrue;
    
    switch( aCommand )
        {
        case EPhoneInCallCmdDialer:
            iViewCommandHandle->ExecuteCommandL( EPhoneViewOpenDialer );
            break;
            
        case EPhoneNumberAcqCmdAddToContacts:
            {
            // If call setup is ongoing then add to contacts query should
            // not be opened -> do nothing here.
            }
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

        
        case EPhoneCmdOptions:
            break;
        
        case EPhoneInCallCmdDtmfManualQuery:
              break;    
            
        case EPhoneDtmfDialerExit:      // fall through
        case EPhoneDtmfDialerCancel:
            break;

        case EPhoneInCallCmdMute: // fall through
        case EPhoneInCallCmdUnmute:
            iStateMachine->PhoneEngineInfo()->SetAudioMuteCommand( 
                ( aCommand == EPhoneInCallCmdMute ) );
            iStateMachine->SendPhoneEngineMessage( 
                MPEPhoneModel::EPEMessageSetAudioMute );
            break;
                                   
        default:
            commandStatus = CPhoneState::HandleCommandL( aCommand );
            break;
        }

    return commandStatus;
    }


// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleNumberEntryClearedL()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::HandleNumberEntryClearedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::HandleNumberEntryClearedL()" );
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleKeyEventL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::HandleKeyEventL( 
    const TKeyEvent& aKeyEvent, TEventCode aEventCode )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::HandleKeyEventL( ) ");
    if ( !iNumberEntryManager->IsNumberEntryVisibleL() )
        {
        // Send the key events to the phone engine
        switch( aEventCode )
            {
            case EEventKey:
                {
                PlayKeySpecificDTMF( aKeyEvent.iCode );
                if ( aKeyEvent.iScanCode == EStdKeyUpArrow && 
                        !OnlySideVolumeKeySupported() )
                    {
                    CPhoneState::IncreaseAudioVolumeL();
                    }
                else if ( aKeyEvent.iScanCode == EStdKeyDownArrow && 
                        !OnlySideVolumeKeySupported() )
                    {
                    CPhoneState::DecreaseAudioVolumeL();
                    }
                }
                break;
                
            case EEventKeyUp:
                // Send a key up event for the last key code sent to
                // the phone engine
                iStateMachine->SendPhoneEngineMessage( 
                    MPEPhoneModel::EPEMessageEndDTMF );
                break;
                
            default:
                break;
            }
        }
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::PlayKeySpecificDTMF
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::PlayKeySpecificDTMF( const TUint aCode )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::PlayKeySpecificDTMF( ) ");
    TBuf<1> buffer; // one character
    buffer.Append( aCode );
    __PHONELOG1( EBasic, EPhoneControl, "HandleKeyEventL(%S)", &buffer );
    // Convert key code to western.
    TLex code( buffer ); 
    // Send the key press to the phone engine, if applicable
    iStateMachine->PhoneEngineInfo()->SetKeyCode( code.Peek() );
    iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessagePlayDTMF );
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleKeyMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::HandleKeyMessageL( 
    TPhoneKeyEventMessages aMessage,
    TKeyCode aCode )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::HandleKeyMessageL()");
    switch ( aCode )
        {
        // end-key
        case EKeyNo:
            if ( aMessage == EPhoneKeyLongPress )
                {
                // Close all connections
                iStateMachine->SendPhoneEngineMessage(
                    MPEPhoneModel::EPEMessageTerminateAllConnections );
                
                // Remove number entry if long press
                if ( iNumberEntryManager->IsNumberEntryUsedL() )
                    {
                    iViewCommandHandle->ExecuteCommandL( 
                        EPhoneViewRemoveNumberEntry );
                    }
                }
            else
                {
                CPhoneState::DisconnectCallL();
                }
            break;
            
        // Voice key
        case EKeyDevice6:
            if ( aMessage == EPhoneKeyShortPress )
                {
                const TBool handsfreeMode = 
                    iStateMachine->PhoneEngineInfo()->AudioOutput() ==
                        EPELoudspeaker;
                CPhoneState::SetHandsfreeModeL( !handsfreeMode );
                }
            else // aMessage == EPhoneKeyLongPress
                {
                CPhoneState::SendGlobalInfoNoteL( EPhoneCallInProgress, ETrue );
                }
            break;

        default:
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandlePhoneEngineMessageL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneStateCallSetup::HandlePhoneEngineMessageL( 
    const TInt aMessage, 
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::HandlePhoneEngineMessageL()");
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageConnected:
            HandleConnectedL( aCallId );
            break;

        case MEngineMonitor::EPEMessageRemoteTerminated:
            break;

        case MEngineMonitor::EPEMessageIdle:
            HandleIdleL( aCallId );
            break;

        case MEngineMonitor::EPEMessageAudioOutputChanged:
            HandleAudioOutputChangedL();
            break;
            
        case MEngineMonitor::EPEMessageSendingDTMF:   // fall through
        case MEngineMonitor::EPEMessageContinuingDTMFSending:
            HandleSendingDTMFL();
            break;
         
        case MEngineMonitor::EPEMessageStoppedDTMF:
            HandleStoppedDTMFL();
            break;

        case MEngineMonitor::EPEMessageSentDTMF:    // fall through
        case MEngineMonitor::EPEMessageDTMFSendingAborted:
            CancelDTMFSendingL();
            break;
            
         default:
            CPhoneState::HandlePhoneEngineMessageL( aMessage, aCallId );
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleSendingDTMFL
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::HandleSendingDTMFL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateCallSetup::HandleSendingDTMFL()" );
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
// CPhoneStateCallSetup::HandleStoppedDTMFL
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::HandleStoppedDTMFL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, 
            "CPhoneStateCallSetup::HandleStoppedDTMFL()" );
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
        iViewCommandHandle->ExecuteCommandL( 
                EPhoneViewShowQuery, 
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
        CPhoneState::ShowTextQueryL( 
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
// CPhoneStateCallSetup::CancelDTMFSendingL
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::CancelDTMFSendingL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::CancelDTMFSendingL()" );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveDtmfNote );
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleConnectedL
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::HandleConnectedL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::HandleConnectedL()");
    BeginUiUpdateLC();
    CPhoneState::UpdateSingleActiveCallL( aCallId );
    UpdateUiCommands();
    EndUiUpdate();
    iStateMachine->ChangeState( EPhoneStateSingle );
    }

// -----------------------------------------------------------
// CPhoneStateCallSetup::HandleIdleL
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::HandleIdleL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::HandleIdleL()");
    HBufC *phoneNumber = HBufC::NewLC( KPhoneNumberEntryBufferSize );
    TPtr ptr( phoneNumber->Des() );
    TPhoneCmdParamString stringParam;
    stringParam.SetString( &ptr );
    
    BeginUiUpdateLC ();
    iViewCommandHandle->ExecuteCommandL( EPhoneViewHideNaviPaneAudioVolume );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveCallHeader, aCallId );
    SetDefaultFlagsL();
    
    if ( iNumberEntryManager->IsNumberEntryUsedL() )
        {
        iViewCommandHandle->ExecuteCommand(
                EPhoneViewGetNumberFromEntry,
                &stringParam );
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNumberEntry );
        iViewCommandHandle->ExecuteCommandL( EPhoneViewLaunchLogs, &stringParam );
        }    
    else
        {
        RemoveDialogsAndSendPhoneToBackgroundL();
        }
    EndUiUpdate();
    CleanupStack::PopAndDestroy( phoneNumber );
    iStateMachine->ChangeState( EPhoneStateIdle );
    }
    
// -----------------------------------------------------------------------------
// CPhoneStateCallSetup::HandleAudioOutputChangedL
// -----------------------------------------------------------------------------
//    
void CPhoneStateCallSetup::HandleAudioOutputChangedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateCallSetup::HandleAudioOutputChangedL( ) ");
    CPhoneState::HandleAudioOutputChangedL();
    // Update the call setup CBA
    UpdateUiCommands();
    }
 
// -----------------------------------------------------------
// CPhoneStateCallSetup::SendDtmfL
// -----------------------------------------------------------
//
void CPhoneStateCallSetup::SendDtmfL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,  
        "CPhoneStateCallSetup::SendDtmfL()" );
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

// End of File
