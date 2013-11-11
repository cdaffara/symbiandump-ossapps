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
* Description: Implementation of CPhoneState class.
*
*/


// INCLUDE FILES
#include <avkon.rsg>
#include <bautils.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <pevirtualengine.h>
#include <mpeclientinformation.h>
#include <mpeengineinfo.h>
#include <cpephonemodelif.h>
#include <featmgr.h>
#include <rconnmon.h>
#include <Profile.hrh>
#include <PUAcodes.hrh>
#include <telephonyvariant.hrh>
#include <aknsoundsystem.h>
#include <wlaninternalpskeys.h>
#include <btengdomainpskeys.h>
#include <btengdomaincrkeys.h>
#include <settingsinternalcrkeys.h>
#include <starterclient.h>
#include <rsssettings.h>
#include <telephonydomainpstypes.h>
#include <telinformationpskeys.h>
#include <coreapplicationuisdomainpskeys.h>
#include <mccecall.h>
#include <ccpdefs.h>
#include <LogsDomainCRKeys.h>
#include <spsettings.h>
#include <startupdomainpskeys.h>
#include <MediatorDomainUIDs.h>
#include <videotelcontrolmediatorapi.h>
#include <textresolver.h>
#include <phoneappvoipcommands.hrh>
#include <hwrmdomainpskeys.h>
#include <hal.h>

#include "phoneui.pan"
#include "cphonestate.h"
#include "mphonestatemachine.h"
#include "mphoneviewcommandhandle.h"
#include "tphonecmdparamboolean.h"
#include "tphonecmdparaminteger.h"
#include "tphonecmdparamkeyevent.h"
#include "tphonecmdparamdynmenu.h"
#include "tphonecmdparamcallheaderdata.h"
#include "tphonecmdparamincallindicatordata.h"
#include "tphonecmdparamnote.h"
#include "tphonecmdparamkeycapture.h"
#include "tphonecmdparamglobalnote.h"
#include "tphonecmdparamquery.h"
#include "tphonecmdparamstring.h"
#include "tphonecmdparamcallstatedata.h"
#include "tphonecmdparambitmap.h"
#include "tphonecmdparamappinfo.h"
#include "tphonecmdparamtranseffect.h"
#include "tphonecmdparamringtone.h"
#include "tphonecmdparamcustomdialer.h"
#include "cphonekeys.h"
#include "phoneui.hrh"
#include "phonerssbase.h"
#include "cphonebtaadisconnecthandler.h"
#include "cphonemainresourceresolver.h"
#include "cphonecenrepproxy.h"
#include "cphonepubsubproxy.h"
#include "phoneviewcommanddefinitions.h"
#include "phonelogger.h"
#include "phonestatedefinitions.h"
#include "cphonetimer.h"
#include "mphoneerrormessageshandler.h"
#include "cphoneclearblacklist.h"
#include "mphonecustomization.h"
#include "cphonemainerrormessageshandler.h"
#include "cphoneaccessorybthandler.h"
#include "cphonemediatorfactory.h"
#include "mphonemediatormessage.h"
#include "cphonemediatorsender.h"
#include "cphonereleasecommand.h"
#include "cphonecontinueemergencycallcommand.h"
#include "tphonecmdparamsfidata.h" 
#include "mphonestorage.h"
#include "phoneconstants.h"
#include "cphoneclearblacklist.h"
#include "phonenoteutil.h"
#include "phoneaudioutil.h"

// ================= MEMBER FUNCTIONS =======================

EXPORT_C CPhoneState::CPhoneState(
    MPhoneStateMachine* aStateMachine,
    MPhoneViewCommandHandle* aViewCommandHandle,
    MPhoneCustomization* aCustomization) :
    iStateMachine( aStateMachine ),
    iViewCommandHandle( aViewCommandHandle ),
    iCustomization( aCustomization )
    {
    // Need to get current SimState for inherited classis
    iPreviousSimState = SimState();
    __ASSERT_ALWAYS(
        aStateMachine && aViewCommandHandle,
        Panic( EPhoneCtrlParameterNotInitialized ) );
    }

EXPORT_C void CPhoneState::BaseConstructL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::BaseConstructL() ");
    if ( !iUiCommandManager )
        {
        iUiCommandManager = CPhoneUiCommandManager::NewL(
                        this,
                        *iViewCommandHandle,
                        *iStateMachine );
        }
    
    if ( !iNumberEntryManager )
        {
        iNumberEntryManager = CPhoneNumberEntryManager::NewL(
                this,
                *iViewCommandHandle,
                *iStateMachine,
                iCustomization,
                *iUiCommandManager );
        }
    
    }

EXPORT_C CPhoneState::~CPhoneState()
    {
    delete iNumberEntryManager;
    delete iUiCommandManager;
    }

// -----------------------------------------------------------
// CPhoneState::EikonEnv
// -----------------------------------------------------------
//
EXPORT_C CEikonEnv* CPhoneState::EikonEnv() const
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::EikonEnv( ) ");
    return iEnv;
    }

// -----------------------------------------------------------
// CPhoneState::SetEikonEnv
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::SetEikonEnv( CEikonEnv* aEnv )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SetEikonEnv( ) ");
    iEnv = aEnv;
    }

// <-------------------------- PHONE ENGINE EVENTS --------------------------->
// -----------------------------------------------------------
// CPhoneState::HandlePhoneEngineMessageL
// Default handling for Phone Engine messages
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandlePhoneEngineMessageL(
    const TInt aMessage,
    TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandlePhoneEngineMessageL() ");
    CPhoneAccessoryBTHandler* accessoryBtHandler;
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageNetworkRegistrationStatusChange:
            {
            __PHONELOG1( EBasic, EPhoneControl,
               "PHONEUI_ERROR: EPEMessageNetworkRegistrationStatusChange - RegistrationStatus =%d ",
               iStateMachine->PhoneEngineInfo()->NetworkRegistrationStatus());
            }
            break;

        case MEngineMonitor::EPEMessageAudioMuteChanged:
            HandleAudioMuteChangedL();
            break;

        case MEngineMonitor::EPEMessageAudioOutputChanged:
            HandleAudioOutputChangedL();
            UpdateUiCommands();
            break;

        case MEngineMonitor::EPEMessageAvailableAudioOutputsChanged:
            HandleAudioAvailableOutputChangedL();
            if ( iStateMachine->PhoneEngineInfo()->AudioOutput() != EPENotActive )
                {
                UpdateUiCommands();
                }
            break;

        case MEngineMonitor::EPEMessageChangedCallDuration:
            HandleChangedCallDurationL( aCallId );
            break;

        case MEngineMonitor::EPEMessageRemoteBusy:
            PhoneNoteUtil::HandleRemoteBusyNoteL( aCallId );
            break;

        case MEngineMonitor::EPEMessageCallWaiting:
        case MEngineMonitor::EPEMessageProfileChanged:
            break;

        case MEngineMonitor::EPEMessageRemoteTerminated:
            /* Flow through */
        case MEngineMonitor::EPEMessageDisconnecting:
            HandleDisconnectingL( aCallId );
            break;

        case MEngineMonitor::EPEMessageTransferDone:
            SendGlobalInfoNoteL( EPhoneInCallTransferred, ETrue );
            break;

       case MEngineMonitor::EPEMessageInitiatedEmergencyCall:
            HandleInitiatedEmergencyCallL( aCallId );
            break;

       case MEngineMonitor::EPEMessageInitiatedEmergencyWhileActiveVideo:
           HandleInitiatedEmergencyWhileActiveVideoL();
           break;

       case MEngineMonitor::EPEMessageCallSecureStatusChanged:
           HandleCallSecureStatusChangeL( aCallId );
           if ( iCustomization )
               {
               iCustomization->HandlePhoneEngineMessageL( aMessage,
                   aCallId );
               }
           break;

        case MEngineMonitor::EPEMessageIssuingUSSDRequest:
            {
            TPhoneCmdParamBoolean globalNotifierParam;
            globalNotifierParam.SetBoolean( EFalse );
            iViewCommandHandle->ExecuteCommandL(
                EPhoneViewSetGlobalNotifiersDisabled,
                &globalNotifierParam );
            iNumberEntryManager->NumberEntryClearL();
            }
            break;

        case MEngineMonitor::EPEMessageIssuedUSSDRequest:
            UpdateUiCommands();
            break;

        // *#9990#
        case MEngineMonitor::EPEMessageShowBTLoopback:
            iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageEndDTMF );
            accessoryBtHandler = CPhoneAccessoryBTHandler::NewLC(
                iViewCommandHandle, iStateMachine );
            accessoryBtHandler->ShowBTLoopbackL();
            CleanupStack::PopAndDestroy( accessoryBtHandler );
            break;

        // *#2820#
        case MEngineMonitor::EPEMessageShowBTDeviceAddress:
            iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageEndDTMF );
            accessoryBtHandler = CPhoneAccessoryBTHandler::NewLC(
                iViewCommandHandle, iStateMachine );
            accessoryBtHandler->ShowBTAddressL();
            CleanupStack::PopAndDestroy( accessoryBtHandler );
            break;

        // *#7370#
        case MEngineMonitor::EPEMessageActivateRfsDeep:
            iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageEndDTMF );
            iViewCommandHandle->ExecuteCommandL( EPhoneViewLaunchRfsDeep );
            break;

        // *#7780#
        case MEngineMonitor::EPEMessageActivateRfsNormal:
            iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageEndDTMF );
            iViewCommandHandle->ExecuteCommandL( EPhoneViewLaunchRfsNormal );
            break;
            
        // *#62209526#
        case MEngineMonitor::EPEMessageShowWlanMacAddress:
            iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageEndDTMF );
            ShowWlanMacAddressL();
            break;

        case MEngineMonitor::EPEMessageThumbnailLoadingCompleted:
        case MEngineMonitor::EPEMessageRemotePartyInfoChanged:
            UpdateCallHeader( aCallId );
            break;

        case MEngineMonitor::EPEMessageSIMStateChanged:
            __PHONELOG1( EBasic, EPhoneControl, "CPhoneState::HandlePhoneEngineMessageL - simStateChanged =%d", SimState()  );
            HandleSimStateChangedL();
            break;

        case MEngineMonitor::EPEMessageShowIncCallGroupIndex:
            HandleCugInUseNoteL();
            break;

        default:
            TBool handled( EFalse );
            if ( iCustomization )
                {
                handled = iCustomization->HandlePhoneEngineMessageL(
                                aMessage, aCallId );
                }
            if ( EFalse == handled )
                {
                MPhoneMediatorMessage* mediatorMessage = CPhoneMediatorFactory::Instance()->MediatorMessage( aMessage, aCallId );
                if( mediatorMessage )
                    {
                    CleanupDeletePushL( mediatorMessage );
                    mediatorMessage->ExecuteL();
                    CleanupStack::PopAndDestroy( mediatorMessage );
                    mediatorMessage = NULL;
                    }
                }
            break;
        }
    }

// -----------------------------------------------------------
// CPhoneState::HandleAudioMuteChangedL
// Default handling for EPEMessageAudioMuteChanged message
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandleAudioMuteChangedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleAudioMuteChangedL() ");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    PhoneAudioUtil::HandleAudioMuteChangedL();
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneState::HandleAudioOutputChangedL
// Default handling for EPEMessageAudioOutputChanged message
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandleAudioOutputChangedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleAudioOutputChangedL() ");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(), Panic( EPhoneCtrlInvariant ) );
    PhoneAudioUtil::HandleAudioOutputChangedL();
    }

// -----------------------------------------------------------
// CPhoneState::HandleSimStateChangedL
// Handles changes in the SIM card state.
// -----------------------------------------------------------
//
void CPhoneState::HandleSimStateChangedL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneState::HandleSimStateChangedL()" );
    CPhoneMainResourceResolver& resolver = *CPhoneMainResourceResolver::Instance();
    TPESimState simState = SimState();
    __PHONELOG2( EBasic, EPhoneControl, "SIM state was changed from %d to %d", iPreviousSimState, simState );
    switch ( simState )
        {
        case EPESimUsable: // Falls through.
        case EPESimReadable: // Falls through.
        case EPESimNotReady:
            {
            if ( iPreviousSimState == EPESimNotPresent )
                {
                __PHONELOG( EBasic, EPhoneControl, "SIM card was inserted, rebooting the phone" );
                RStarterSession starterSession;
                User::LeaveIfError( starterSession.Connect() );
                CleanupClosePushL( starterSession );
                
                HBufC* queryText = StringLoader::LoadLC( resolver.ResolveResourceID( EPhoneRebootRequired ) );
                TPhoneCmdParamQuery queryParams;
                queryParams.SetCommandParamId( TPhoneCommandParam::EPhoneParamRebootQuery );
                queryParams.SetQueryPrompt( *queryText );
                queryParams.SetDefaultCba( R_AVKON_SOFTKEYS_OK_EMPTY );
                iViewCommandHandle->ExecuteCommandL( EPhoneViewShowQuery, &queryParams );
                starterSession.Reset( RStarterSession::ESIMStatusChangeReset );
                CleanupStack::PopAndDestroy( 2, &starterSession ); // queryText
                }
            break;
            }
        case EPESimNotPresent:
            {
            if ( iPreviousSimState == EPESimUsable ||
                 iPreviousSimState == EPESimReadable ||
                 iPreviousSimState == EPESimNotReady )
                {
                __PHONELOG( EBasic, EPhoneControl, "SIM card was removed" );
                TPhoneCmdParamGlobalNote globalNoteParam;
                globalNoteParam.SetType( EPhoneMessageBoxInformation );
                globalNoteParam.SetTextResourceId(
                    CPhoneMainResourceResolver::Instance()->
                    ResolveResourceID( EPhoneSimRemoved ) );
                iViewCommandHandle->ExecuteCommandL(
                    EPhoneViewShowGlobalNote, &globalNoteParam );
                }
            // Show security note, if SIM not present and KFeatureIdFfSimlessOfflineSupport is disabled.
            else if ( !FeatureManager::FeatureSupported( KFeatureIdFfSimlessOfflineSupport ) &&
                    ( iPreviousSimState != EPESimNotSupported ) )
                {
                StartShowSecurityNoteL();
                }
            break;
            }
        default:
            break;
        }
    iPreviousSimState = simState;
    }

// -----------------------------------------------------------------------------
// CPhoneState::SendDtmfKeyEventL
// send dtmf event when,
// -touch dtmf dialer is NOT visible
// -query is not active
// -menubar is not open
// -----------------------------------------------------------------------------
//
void CPhoneState::SendDtmfKeyEventL( const TKeyEvent& aKeyEvent,
               TEventCode aEventCode  )
    {
    // Send the key event to the phone engine.
    SendKeyEventL( aKeyEvent, aEventCode );
    }

// -----------------------------------------------------------
// CPhoneState::HandleErrorL
// Default handling for HandleError message
// (other items were commented in a header).
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandleErrorL( const TPEErrorInfo& aErrorInfo )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleErrorL()");
    __PHONELOG1( EBasic, EPhoneControl,
            "PhoneUIControl: CPhoneState::HandleErrorL - aErrorInfo.iErrorCode =%d ",
            aErrorInfo.iErrorCode );
    __PHONELOG1( EBasic, EPhoneControl,
        "PhoneUIControl: CPhoneState::HandleErrorL - aErrorInfo.iCallId =%d ",
        aErrorInfo.iCallId );
    // Do the common error handling (display proper notes etc)
    PhoneNoteUtil::HandleErrorL( aErrorInfo );
    }

// -----------------------------------------------------------
// CPhoneState::HandleChangedCallDurationL
// Default handling for EPEMessageChangedCallDuration message
// (other items were commented in a header).
// -----------------------------------------------------------
void CPhoneState::HandleChangedCallDurationL( TInt aCallId )
    {
     __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleChangedCallDurationL() ");
    TInt ret = KErrNone;
    TInt isDisplayOn;
    ret = HAL::Get( HALData::EDisplayState, isDisplayOn );
    // Update only if the display is on or if HAL::Get returns an error, 
    // in which case display value cannot be trusted.
    if ( ret || isDisplayOn )
        {
        // Get the call duration
        TTimeIntervalSeconds seconds =
            iStateMachine->PhoneEngineInfo()->CallDuration( aCallId );
        TPhoneCmdParamInteger time;
        time.SetInteger(seconds.Int());
        iViewCommandHandle->ExecuteCommandL(EPhoneViewUpdateCallHeaderCallDuration, aCallId, &time);
        }
    }	

// -----------------------------------------------------------
// CPhoneState::HandleDisconnectingL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandleDisconnectingL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleDisconnectingL( ) ");

    iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );
    UpdateCallHeader( aCallId );

    CPhoneClearBlacklist::Instance()->ClearBlackListOnNextKey();
    }

// <------------------------------- KEY EVENTS ------------------------------->
// -----------------------------------------------------------
// CPhoneState::HandleKeyMessageL( aMessage, aKeyCode )
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandleKeyMessageL(
    TPhoneKeyEventMessages /*aMessage*/,
    TKeyCode /*aKeyCode*/ )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleKeyMessageL( ) ");
    }

// -----------------------------------------------------------
// CPhoneState::HandleKeyEventL( aKeyEvent, aEventCode )
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandleKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleKeyEventL( ) ");
    if ( aKeyEvent.iScanCode == EStdKeyUpArrow &&
         aEventCode == EEventKey )
        {
        // Increace audio volume
        IncreaseAudioVolumeL();
        }
    else if ( aKeyEvent.iScanCode == EStdKeyDownArrow &&
            aEventCode == EEventKey )
        {
        // Decreace audio volume
        DecreaseAudioVolumeL();
        }
    }

// -----------------------------------------------------------
// CPhoneState::OnlySideVolumeKeySupported
// -----------------------------------------------------------
//
TBool CPhoneState::OnlySideVolumeKeySupported()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::OnlySideVolumeKeySupported( ) ");
    TBool onlySideVolumeKeySupported(EFalse);
    if ( !CPhoneCenRepProxy::Instance()->
            IsTelephonyFeatureSupported( KTelephonyLVFlagScrollVolumeKeys ) &&
            FeatureManager::FeatureSupported( KFeatureIdSideVolumeKeys ) )
        {
        onlySideVolumeKeySupported = ETrue;
        }
    __PHONELOG1( EBasic, EPhoneControl,
            "OnlySideVolumeKeySupported: %d",
            onlySideVolumeKeySupported );
    return onlySideVolumeKeySupported;
    }

// -----------------------------------------------------------
// CPhoneState::SendKeyEventL
// -----------------------------------------------------------
//
void CPhoneState::SendKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SendKeyEventL( ) ");
    switch( aEventCode )
        {
        case EEventKey:
            // Send the key press to the phone engine, if applicable
            SendKeyPressL( aKeyEvent, aEventCode );
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

// -----------------------------------------------------------
// CPhoneState::SendKeyPressL
// -----------------------------------------------------------
//
void CPhoneState::SendKeyPressL(
    const TKeyEvent& aKeyEvent,
    TEventCode aEventCode )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SendKeyPressL( ) ");
    const TBool dtmfToneKeyEntered =
        CPhoneKeys::IsDtmfTone( aKeyEvent, aEventCode );

    if ( dtmfToneKeyEntered ||
        aKeyEvent.iCode == EKeyBackspace )
        {
        // Get the number entry contents and store, if it exists
        if ( iNumberEntryManager->IsNumberEntryUsedL() )
            {
            HBufC* phoneNumber = iNumberEntryManager->PhoneNumberFromEntryLC();
            __PHONELOG1( EBasic, EPhoneControl, "SetPhoneNumber: %S ", phoneNumber );
            iStateMachine->PhoneEngineInfo()->SetPhoneNumber( *phoneNumber );

            // Convert key code to western.
            TBuf<1> buffer; // one character
            buffer.Append( aKeyEvent.iCode );
            __PHONELOG1( EBasic, EPhoneControl,
                "CPhoneState::SendKeyPressL(%S)",
                &buffer );
            
            // Save the key code
            TLex code( buffer );
            iStateMachine->PhoneEngineInfo()->SetKeyCode( code.Peek() );

            // Plays a DTMF tone if active call
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessagePlayDTMF );
            CleanupStack::PopAndDestroy( phoneNumber );
            }
        }
    }

// <------------------------------ SYSTEM EVENTS ----------------------------->

// -----------------------------------------------------------
// CPhoneState::HandlePhoneStartupL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandlePhoneStartupL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandlePhoneStartupL( ) ");
    // Empty implementation
    }

// -----------------------------------------------------------
// CPhoneState::HandlePropertyChangedL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandlePropertyChangedL(
    const TUid& aCategory,
    const TUint aKey,
    const TInt aValue )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandlePropertyChangedL( ) ");
    if ( ( aCategory == KPSUidStartup  ) && 
         ( aKey == KStartupSimSecurityStatus ) && 
         ( aValue == ESimRejected || aValue == ESimUnaccepted ) )
        {
       
        __PHONELOG3( EBasic, EPhoneControl,
                "CPhoneState::HandlePropertyChangedL -aCategory= %d, aKey= %d, aValue= %d",
                aCategory, aKey, aValue );
        // Show security note, SIM is not valid.
        StartShowSecurityNoteL();
        }
    }

// <---------------------------- MENU AND CBA EVENTS ------------------------->
EXPORT_C TBool CPhoneState::HandleCommandL( TInt aCommand )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleCommandL( ) ");
    TBool commandStatus = ETrue;
    switch( aCommand )
        {
        case EPhoneEmergencyCmdExit:
        case EPhoneDialerCallHandling:
        case EPhoneCmdBack:
            iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNumberEntry );
            // Do state-specific behaviour if number entry is cleared
            HandleNumberEntryClearedL();
            break;

        case EPhoneDialerCmdTouchInput:
        case EPhoneNumberAcqCmdSave:
        case EPhoneNumberAcqCmdAddToName:
            break;

        case EPhoneNumberAcqCmdAddToContacts:
            {
            TPhoneCmdParamQuery queryDialogParam;
                queryDialogParam.SetQueryType( EPhoneContactsListQuery );
                queryDialogParam.SetQueryResourceId(
                CPhoneMainResourceResolver::Instance()->
                ResolveResourceID( EPhoneAddtoContactsListQuery ) );
            iViewCommandHandle->ExecuteCommandL( EPhoneViewShowQuery,
                &queryDialogParam );
            }
            break;

        case EPhoneCmdWaitNoteOk:
            iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNote );
            break;

        case EPhoneInCallCmdEndThisOutgoingCall:
            DisconnectCallL();
            break;

        case EPhoneCmdYesBtDisconnectQuery:
        case EPhoneCmdNoBtDisconnectQuery:
            CPhoneBtaaDisconnectHandler::InstanceL()->HandleQueryDismissL( aCommand );
            // Make sure that query is removed
            // if there has been also another ( by Cover UI ).
            iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveQuery );
            break;

        case EPhoneInCallCmdDeactivateIhf: // fall through
        case EPhoneInCallCmdActivateIhf:
             SetHandsfreeModeL( aCommand == EPhoneInCallCmdActivateIhf );
             break;

        case EPhoneInCallCmdHandset:    // fall through
        case EPhoneInCallCmdBtHandsfree:
            CPhoneState::SetBTHandsfreeModeL(
                 aCommand == EPhoneInCallCmdBtHandsfree );
            break;

        case EPhoneInCallCmdActivatEPhonebook:
        case EPhoneNumberAcqSecurityDialer:
        case EPhoneDialerCmdContacts:
        case EPhoneNumberAcqCmdToggleNeAlphaMode:
        case EPhoneNumberAcqCmdToggleNeNumericMode:
            break;
        
        case EPhoneInCallCmdSetVolumeLevel:
            {
            // This command comes from ui control, no need to update
            // value to control (second parameter set false).
            PhoneAudioUtil::ChangeAudioVolumeL( 
                    PhoneAudioUtil::GetVolumeLevel(), EFalse );
            }
            break;

        case EPhoneCmdVideoCallOutOfMemory:
            ShowVideoCallOutOfMemoryNoteL();
            DisconnectCallL();
            break;
            
        case EPhoneCallComingCmdSoftReject:
            OpenSoftRejectMessageEditorL();
            break;
            
        case EPhoneInCallCmdContacts:
            iViewCommandHandle->ExecuteCommand( EPhoneViewOpenContacts );
            break;
            
        default:
            {
            commandStatus = EFalse;
            }
            break;
        }
    
    if( !commandStatus && iCustomization )
        {
        commandStatus = iCustomization->HandleCommandL( aCommand );
        }
    return commandStatus;
    }

// -----------------------------------------------------------------------------
// CPhoneState::ProcessCommandL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPhoneState::ProcessCommandL( TInt /*aCommand*/ )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneStateIdle::ProcessCommandL() ");
    // no implementation.
    return EFalse;
    }

// <-------------------------- REMOTE CONTROL EVENTS ------------------------->

// -----------------------------------------------------------------------------
// CPhoneState::HandleRemConCommandL
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CPhoneState::HandleRemConCommandL(
    TRemConCoreApiOperationId aOperationId,
    TRemConCoreApiButtonAction /*aButtonAct*/ )
    {
    TBool handled = EFalse;
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::MrccatoCommand() ");
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp:
            __PHONELOG( EBasic, EPhoneControl, "CPhoneState::MrccatoCommand ERemConCoreApiVolumeUp" );
            IncreaseAudioVolumeL();
            handled = ETrue;
            break;

        case ERemConCoreApiVolumeDown:
            __PHONELOG( EBasic, EPhoneControl, "CPhoneState::MrccatoCommand ERemConCoreApiVolumeDown" );
            DecreaseAudioVolumeL();
            handled = ETrue;
            break;
        case ERemConCoreApiMute:
            {
            __PHONELOG( EBasic, EPhoneControl, "CPhoneState::MrccatoCommand ERemConCoreApiMute" );
            PhoneAudioUtil::ToggleMute();
            handled = ETrue;
            break;
            }

         default:
             __PHONELOG1( EBasic, EPhoneControl, "CPhoneState::MrccatoCommand MrccatoCmd.Other(%d)", aOperationId );
            // Other commands ignored.
            break;
        }
    return handled;
    }

// -----------------------------------------------------------
// CPhoneState::DecreaseAudioVolume()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::DecreaseAudioVolumeL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::DecreaceAudioVolumeL( ) ");
    PhoneAudioUtil::DecreaseAudioVolumeL();
    }

// -----------------------------------------------------------
// CPhoneState::IncreaceAudioVolume()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::IncreaseAudioVolumeL()
    {
     __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::IncreaceAudioVolumeL( ) ");
    PhoneAudioUtil::IncreaseAudioVolumeL();
    }

// <-------------------------- COMMON STATE FUNCTIONS ------------------------>
// -----------------------------------------------------------
// CPhoneState::DialMultimediaCallL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::DialMultimediaCallL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::DialMultimediaCall() ");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    iStateMachine->PhoneEngineInfo()->SetCallTypeCommand( EPECallTypeVideo );
    iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageDial );
    }

// -----------------------------------------------------------
// CPhoneState::DialVoiceCallL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::DialVoiceCallL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::DialVoiceCallL() ");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(), Panic( EPhoneCtrlInvariant ) );
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( ETrue );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
            &globalNotifierParam );
    iStateMachine->PhoneEngineInfo()->SetCallTypeCommand( EPECallTypeCSVoice );
    iStateMachine->SendPhoneEngineMessage( MPEPhoneModel::EPEMessageDial );
    }

// -----------------------------------------------------------
// CPhoneState::DisconnectCallL
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneState::DisconnectCallL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::DisconnectCallL( ) ");
    TPhoneCmdParamInteger callIdParam;
    iViewCommandHandle->ExecuteCommand( 
            EPhoneViewGetExpandedBubbleCallId, &callIdParam );
    TBool ret = EFalse;
    if( callIdParam.Integer() > KErrNotFound )
        {
        iStateMachine->SetCallId( callIdParam.Integer() );
        if( IsVideoCall( callIdParam.Integer() ) )
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
                   CPhoneReleaseCommand::NewL( *iStateMachine ) );
            }
        else
            {
            iStateMachine->SendPhoneEngineMessage(
                MPEPhoneModel::EPEMessageRelease );
            }
        ret = ETrue;
        }
    else
        {
        __PHONELOG( EOnlyFatal, EPhoneControl,
            "CPhoneState::DisconnectCallL has negative call id!" );
        }
    return ret;
    }

// -----------------------------------------------------------
// CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::RemoveDialogsAndSendPhoneToBackgroundL( ) ");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemovePhoneDialogs );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSendToBackground );
    UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneState::UpdateSingleActiveCallL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::UpdateSingleActiveCallL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::UpdateSingleActiveCallL( ) ");
    BeginUiUpdateLC();

    UpdateCallHeaderAndUiCommandsL( aCallId );

    // Create call duration label if enabled
    // This key will be moved to some other area, but since key
    // is supported we can still use it.
    TBool callDuration( EFalse );
    CPhoneCenRepProxy::Instance()->GetInt(
            KCRUidLogs, KLogsShowCallDuration, callDuration );

    if ( callDuration )
        {
        HandleChangedCallDurationL( aCallId );
        }

    EndUiUpdate();
    
    // If there is no need to send back ground and number entry is used then
    // we must show number entry.
    if ( iNumberEntryManager->IsNumberEntryUsedL() && !IsAutoLockOn() )
        {
        iNumberEntryManager->SetNumberEntryVisibilityL(ETrue);
        }
    }

// -----------------------------------------------------------
// CPhoneState::IsWaitingCallL
// -----------------------------------------------------------
//
TBool CPhoneState::IsWaitingCallL( const TInt aCallId  )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::IsWaitingCallL( ) ");
    TBool waiting( EFalse );
    if ( aCallId >= 0 && 
         iStateMachine->PhoneEngineInfo()->CallState( aCallId ) == EPEStateRinging )
        {
        TPhoneCmdParamInteger activeCallCount;
        iViewCommandHandle->ExecuteCommandL(
            EPhoneViewGetCountOfActiveCalls, &activeCallCount );
        
        if( activeCallCount.Integer() > 0 )
            {
            waiting = ETrue;
            }
        }
    
    return waiting;
    }

// -----------------------------------------------------------
// CPhoneState::UpdateCallHeader
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::UpdateCallHeader( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneState::UpdateCallHeader() ");
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewUpdateBubble,
        aCallId );
    
    }

// -----------------------------------------------------------
// CPhoneState::UpdateCallHeaderAndUiCommandsL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::UpdateCallHeaderAndUiCommandsL( TInt aCallId )
    {
    __LOGMETHODSTARTEND( EPhoneControl, 
            "CPhoneState::UpdateCallHeaderAndUiCommandsL() ");
    BeginUiUpdateLC();
    UpdateCallHeader( aCallId );
    UpdateUiCommands();
    EndUiUpdate();
    }

// <-------------------------- CONTEXT MENU ------------------------->

// -----------------------------------------------------------
// CPhoneState::ShowNoteL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::ShowNoteL( TInt aResourceId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::ShowNoteL( ) ");
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
    PhoneNoteUtil::ShowNoteL( aResourceId );
    }

// -----------------------------------------------------------
// CPhoneState::ShowQueryL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::ShowQueryL( TInt aResourceId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::ShowQueryL( ) ");
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
    PhoneNoteUtil::ShowQueryL( aResourceId );
    }

// -----------------------------------------------------------
// CPhoneState::ShowTextQueryL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::ShowTextQueryL(
    TInt aDialogResourceId,
    TInt aDefaultCbaResourceId,
    TInt aContentCbaResourceId,
    TDes* aDataText,
    TBool aSendKeyEnabled )
    {
    __ASSERT_DEBUG( aDialogResourceId &&
        aDefaultCbaResourceId &&
        aContentCbaResourceId &&
        aDataText,
        Panic( EPhoneCtrlParameterNotInitialized ) );
    
    PhoneNoteUtil::ShowTextQueryL(
            aDialogResourceId,
            aDefaultCbaResourceId,
            aContentCbaResourceId,
            aDataText,
            aSendKeyEnabled );

    }

// -----------------------------------------------------------
// CPhoneState::DisplayCallTerminationNoteL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::DisplayCallTerminationNoteL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::DisplayCallTerminationNoteL( ) ");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    if ( CheckIfShowCallTerminationNote() )
        {
        TBuf<KPhoneMaxCharsInNote> noteText;
        TBuf<KPhoneMaxCharsInNote> postCallLineText;

        // Post call Line 1: Call Summary
        LoadResource( postCallLineText, EPhoneNoteTextCallSummary );
        noteText.Append( postCallLineText );
        noteText.Append( KPhoneLineFeed );

        // Post call Line 2: Duration
        LoadResource( postCallLineText, EPhoneNoteTextCallDuration );
        noteText.Append( postCallLineText );
        noteText.Append( KPhoneLineFeed );

        // Post call Line 3: Time, get the format for last call time.
        LoadResource( postCallLineText, EPhoneCallDurationFormatString );
        TPtrC durationFormat( postCallLineText );

        // Get the last call duration
        TTime time( 0 );
        TTimeIntervalSeconds timeInterval =
            iStateMachine->PhoneEngineInfo()->CallDuration();
        time += timeInterval;

        // Format the time
        TBuf<KPhoneMaxTimeDisplayTextLength> timeString( KNullDesC );
        time.FormatL( timeString, durationFormat );

        noteText.Append( timeString );

        PhoneNoteUtil::SendGlobalNoteL(
                EPhoneMessageBoxInformation, noteText);
        }
    }

// ---------------------------------------------------------
// CPhoneState::SendGlobalInfoNoteL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::SendGlobalInfoNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SendGlobalInfoNoteL( ) ");
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
    PhoneNoteUtil::SendGlobalInfoNoteL( aResourceId, aNotificationDialog );
    }

// ---------------------------------------------------------
//  CPhoneUIController::SendGlobalWarningNoteL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::SendGlobalWarningNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SendGlobalWarningNoteL( ) ");
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
    PhoneNoteUtil::SendGlobalWarningNoteL( aResourceId, aNotificationDialog );
    }

// ---------------------------------------------------------
//  CPhoneUIController::SendGlobalErrorNoteL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::SendGlobalErrorNoteL( 
        TInt aResourceId, TBool aNotificationDialog )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SendGlobalErrorNoteL( ) ");
    __ASSERT_DEBUG( aResourceId, Panic( EPhoneCtrlParameterNotInitialized ) );
    PhoneNoteUtil::SendGlobalErrorNoteL( aResourceId, aNotificationDialog );
    }

// ---------------------------------------------------------
// CPhoneState::SetHandsfreeModeL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::SetHandsfreeModeL( TBool aHandsfreeMode )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SetHandsfreeModeL( ) ");
    PhoneAudioUtil::SetHandsfreeModeL( aHandsfreeMode );
    }

// ---------------------------------------------------------
// CPhoneState::SetBTHandsfreeModeL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::SetBTHandsfreeModeL( TBool aHandsfreeMode )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SetBTHandsfreeModeL( ) ");
    PhoneAudioUtil::SetBtHandsfreeModeL( aHandsfreeMode );
    }

// <-------------------------- INTERNAL FUNCTIONS ------------------------>
// -----------------------------------------------------------
// CPhoneState::HandleInitiatedEmergencyCallL
// Default handling for EPEMessageInitiatedEmergencyCallL message
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneState::HandleInitiatedEmergencyCallL( TInt /*aCallId*/ )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleInitiatedEmergencyCallL( ) ");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemovePhoneDialogs );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewStopRingTone );
    
    iStateMachine->ChangeState( EPhoneStateEmergency );
    }

// -----------------------------------------------------------
// CPhoneState::HandleInitiatedEmergencyWhileActiveVideoL
// -----------------------------------------------------------
//
void CPhoneState::HandleInitiatedEmergencyWhileActiveVideoL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleInitiatedEmergencyWhileActiveVideoL( ) ");
    // We have existing video call so need to release dataport before continuings
    // emergency call. Otherwise we will face problems with dataport use later.
    CPhoneMediatorFactory::Instance()->Sender()->IssueCommand( 
            KMediatorVideoTelephonyDomain,
            KCatPhoneToVideotelCommands,
            EVtCmdReleaseDataport,
            TVersion( KPhoneToVideotelCmdVersionMajor,
                    KPhoneToVideotelCmdVersionMinor,
                    KPhoneToVideotelCmdVersionBuild ),
            KNullDesC8,
            CPhoneContinueEmergencyCallCommand::NewL( *iStateMachine ) );
    }

// -----------------------------------------------------------
// CPhoneState::HandleCallSecureStatusChangeL
// -----------------------------------------------------------
//
void CPhoneState::HandleCallSecureStatusChangeL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleCallSecureStatusChangeL( ) ");
    iViewCommandHandle->ExecuteCommandL(
        EPhoneViewCipheringInfoChange,
        aCallId);
    }

// ---------------------------------------------------------
// CPhoneState::IsVideoCall
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneState::IsVideoCall( const TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::IsVideoCall( ) ");
    if ( aCallId < 0 )
        {
        return EFalse;
        }

    return EPECallTypeVideo == iStateMachine->PhoneEngineInfo()->CallType( aCallId );
    }

// ---------------------------------------------------------
// CPhoneState::SimState()
// ---------------------------------------------------------
//
EXPORT_C TPESimState CPhoneState::SimState() const
    {
    /*
    SIM states:
    EPESimStatusUninitialized = KPEStartupEnumerationFirstValue =100,
    EPESimUsable,       // The Sim card is fully usable.
    EPESimReadable,     // The SIM card is not fully usable, but the emergency number can be read.
    EPESimNotReady,     // The Sim card is present but not ready or usable.
    EPESimNotPresent,   // The Sim card is not present.
    EPESimNotSupported  // SIM/RUIM is not supported. Some CDMA phones do not support a SIM/RUIM at all.
    */
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SimState()");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(),
        Panic( EPhoneCtrlInvariant ) );
    TPESimState simState = iStateMachine->PhoneEngineInfo()->SimState();
    __PHONELOG1( EBasic, EPhoneControl,
            "CPhoneState::SimState - value= %d", simState );
    return simState;
    }

// ---------------------------------------------------------
// CPhoneState::IsSimOk()
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneState::IsSimOk()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::IsSimOk()");
    // Phone is interested on Sim state and sim security statuses
    // Check first Sim state status:
    TBool retVal( ETrue );
    switch ( SimState() )
        {
        case EPESimNotSupported:
            retVal = EFalse;
            break;

        case EPESimNotPresent:
            {
            if ( !FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) ||
                 !FeatureManager::FeatureSupported( KFeatureIdFfSimlessOfflineSupport ) )
                {
                retVal = EFalse;
                }
            break;
            }

        default:
            break;
        }
    if ( retVal != EFalse )
        {
        // Sim state is valid, check also Sim security status
        switch ( SimSecurityStatus() )
            {
            case ESimRejected:
            case ESimUnaccepted:
                retVal = EFalse;
                break;

            default:
                break;
            }
        }
    __PHONELOG1( EBasic, EPhoneControl,
            "CPhoneState::IsSimOK - value= %d", retVal );
    return retVal;
    }

// -----------------------------------------------------------
// CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneState::IsSimStateNotPresentWithSecurityModeEnabled()
    {
    TPhoneCmdParamBoolean isSecurityMode;
    TBool retValue(EFalse);
    TRAP_IGNORE( iViewCommandHandle->ExecuteCommandL( EPhoneViewGetSecurityModeStatus, &isSecurityMode ) );
    if ( SimState() == EPESimNotPresent && isSecurityMode.Boolean() )
        {
        retValue = ETrue;
        }
    return retValue;
    }

// -----------------------------------------------------------------------------
// CPhoneState::StartShowSecurityNoteL
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneState::StartShowSecurityNoteL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneState::StartShowSecurityNoteL ");
    TPhoneCmdParamBoolean securityMode;
    securityMode.SetBoolean( ETrue );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetSecurityMode, &securityMode );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveNumberEntry );
    UpdateUiCommands();
    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewBringPhoneAppToForeground );
    TInt resourceId ( KErrNone );
    if ( SimSecurityStatus() == ESimRejected )
        {
        resourceId = CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( EPhoneSimRejected );
        }
    else if ( SimState() == EPESimNotPresent )
        {
        // insert sim card -note
        resourceId = CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( EPhoneSimRemoved );
        }
    else if ( SimSecurityStatus() == ESimUnaccepted )
        {
        resourceId = CPhoneMainResourceResolver::Instance()->
            ResolveResourceID( EPhoneSimUnaccepted );
        }

    if ( resourceId != KErrNone )
        {
        // Add it to the resource string
        HBufC* buf = StringLoader::LoadLC( resourceId );
        TPhoneCmdParamNote noteParam;
        noteParam.SetResourceId( CPhoneMainResourceResolver::Instance()->
           ResolveResourceID( EPhoneSecurityInformationNote ) );
        noteParam.SetText( *buf );
        noteParam.SetTone( CAknNoteDialog::EConfirmationTone );
        noteParam.SetType( EPhoneNoteSecurity );
        // Display note
        iViewCommandHandle->ExecuteCommandL( EPhoneViewShowNote, &noteParam );
        CleanupStack::PopAndDestroy( buf );
        // Capture the App key
        TPhoneCmdParamKeyCapture appKeyCaptureParam;
        appKeyCaptureParam.SetKey( EStdKeyApplication0 );
        appKeyCaptureParam.SetKeyCode( EKeyApplication0 );
        appKeyCaptureParam.SetCaptureType( EPhoneKeyAllEvents );
        iViewCommandHandle->ExecuteCommandL( 
               EPhoneViewStartCapturingKey, &appKeyCaptureParam );
        }
    }

// ---------------------------------------------------------
// CPhoneState::ShowWlanMacAddressL
// ---------------------------------------------------------
//
void CPhoneState::ShowWlanMacAddressL()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneState::ShowWlanMacAddressL()");
    iNumberEntryManager->NumberEntryClearL();
    // Fetch WLAN MAC address
    TBuf8<KPhoneWlanMacAddressLength> address;
    RProperty::Get(
        KPSUidWlan,
        KPSWlanMacAddress,
        address );
    // Format fetched address
    TBuf<KPhoneWlanMacAddressLength> wlanMACAddress;
    for ( TInt i( 0 ); i < address.Length(); i++ )
        {
        // Set separator
        if( i > 0 )
            {
            wlanMACAddress.Append( KPhoneWlanSeparator );
            }
        // Set data
        TBuf<10> tmp;
        tmp.Format( KWLanMACDataFormat, address[i] );
        wlanMACAddress.Append( tmp );
        }

    // Now we need the localised text
    HBufC* wlanMacAddress = StringLoader::LoadLC(
        CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( EPhoneWlanMacAddress ), wlanMACAddress );
    TPhoneCmdParamNote noteParam;
    noteParam.SetType( EPhoneNoteCustom );
    noteParam.SetResourceId( CPhoneMainResourceResolver::Instance()->
        ResolveResourceID( EPhoneInformationWaitNote ) );
    noteParam.SetText( *wlanMacAddress );
    noteParam.SetTone( CAknNoteDialog::EConfirmationTone );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewShowNote, &noteParam );
    CleanupStack::PopAndDestroy( wlanMacAddress );
    }

// -----------------------------------------------------------
// CPhoneState::HandleAudioAvailableOutputChangedL
// Default handling for EPEMessageAvailableAudioOutputsChanged message
// (other items were commented in a header).
// -----------------------------------------------------------
//
void CPhoneState::HandleAudioAvailableOutputChangedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleAudioAvailableOutputChangedL() ");
    __ASSERT_DEBUG( iStateMachine->PhoneEngineInfo(), Panic( EPhoneCtrlInvariant ) );
    PhoneAudioUtil::HandleAudioAvailableOutputChangedL();
    }

// ---------------------------------------------------------
// CPhoneState::ShowNumberBusyNoteL
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::ShowNumberBusyNoteL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::ShowNumberBusyNoteL( ) ");

    TInt resource( EPhoneNumberBusy );
    if( iCustomization )
        {
        // Get customized text resource for busy note
        resource = iCustomization->CustomizeBusyNoteText();
        }
    
    PhoneNoteUtil::SendGlobalNoteL( resource, ETrue );
    }

// ---------------------------------------------------------
// CPhoneState::IsAutoLockOn
// ---------------------------------------------------------
//
EXPORT_C TBool CPhoneState::IsAutoLockOn() const
    {
    const TBool phoneIsLocked =
                CPhonePubSubProxy::Instance()->Value(
                KPSUidCoreApplicationUIs,
                KCoreAppUIsAutolockStatus ) > EAutolockOff;
    __PHONELOG1( EBasic,
            EPhoneControl,
            "CPhoneState::IsAutoLockOn() Status: %d",
            phoneIsLocked );
    return phoneIsLocked;
    }

// ---------------------------------------------------------
// CPhoneState::DeleteTouchPaneButtons
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::DeleteTouchPaneButtons()
    {
    /*TPhoneCmdParamBoolean boolParam;
    TRAP_IGNORE( iViewCommandHandle->ExecuteCommand(
                    EPhoneViewSetTouchPaneButtons,
                    &boolParam ) );*/
    }

// -----------------------------------------------------------
// CPhoneState::BeginUiUpdateLC
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::BeginUiUpdateLC()
    {
    iViewCommandHandle->ExecuteCommand( EPhoneViewBeginUpdate );
    TCleanupItem operation( UiUpdateCleanup, this );
    CleanupStack::PushL( operation );
    }

// -----------------------------------------------------------
// CPhoneState::EndUiUpdate
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::EndUiUpdate()
    {
    CleanupStack::PopAndDestroy(); // Call UiUpdateCleanup
    }

// -----------------------------------------------------------
// CPhoneState::CheckIfShowTerminationNote
// This method is intended to be overridden in states
// that contain more info about decision.
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneState::CheckIfShowCallTerminationNote( )
    {
    TBool show = EFalse;
    TInt callSummaryActivated = 0;
    const TInt err = CPhoneCenRepProxy::Instance()->GetInt(
        KCRUidCommonTelephonySettings,
        KSettingsSummaryAfterCall,
        callSummaryActivated );
    if ( err == KErrNone && callSummaryActivated )
        {
          show = ETrue;
        }
    return show;
    }

// -----------------------------------------------------------------------------
// CPhoneState::HandleDtmfKeyTone
// Called from KeyEventForwarder
// Allow only special characters to pass from
// keyEventForwarder to phoneEngine
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneState::HandleDtmfKeyToneL( const TKeyEvent& aKeyEvent,
                TEventCode aEventCode )
    {
    SendDtmfKeyEventL( aKeyEvent, aEventCode );
    }

// -----------------------------------------------------------------------------
// CPhoneState::UiUpdateCleanup
// -----------------------------------------------------------------------------
//
void CPhoneState::UiUpdateCleanup(TAny* aThis )
    {
    static_cast<CPhoneState*>( aThis )->iViewCommandHandle->ExecuteCommand(
        EPhoneViewEndUpdate );
    }

// -----------------------------------------------------------
// CPhoneState::SetDefaultFlagsL()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::SetDefaultFlagsL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SetDefaultFlagsL()");
    iViewCommandHandle->ExecuteCommandL( EPhoneViewHideNaviPaneAudioVolume );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewUpdateFSW );
    
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
    
    // Restore keylock if phone has been locked before call.
    if ( iStateMachine->PhoneStorage()->NeedToEnableKeylock() )
        {
        iViewCommandHandle->ExecuteCommandL( EPhoneViewEnableKeyLock );
        iStateMachine->PhoneStorage()->SetNeedToEnableKeylock( EFalse );
        }
    }

// -----------------------------------------------------------
// CPhoneState::ShowVideoCallOutOfMemoryNoteL
// -----------------------------------------------------------
//
void CPhoneState::ShowVideoCallOutOfMemoryNoteL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneState::ShowVideoCallOutOfMemoryNoteL()" );
    TPhoneCmdParamBoolean globalNotifierParam;
    globalNotifierParam.SetBoolean( EFalse );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewSetGlobalNotifiersDisabled,
        &globalNotifierParam );
    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewBringPhoneAppToForeground );
    
    // Display error note
    /*SExtendedError ext;
    ext.iComponent = KUidPhoneApplication;
    ext.iErrorNumber = KErrNoMemory;
    ext.iInformation = EFalse;
    TBuf<1> ignore;
    iAvkonAppUi->HandleError( KErrNoMemory, ext, ignore, ignore );*/
    //TODO show note
    }

// -----------------------------------------------------------------------------
// CPhoneState::SimSecurityStatus
// -----------------------------------------------------------------------------
//
TInt CPhoneState::SimSecurityStatus() const
    {
    /*
    Sim security statuses:
    ESimSecurityStatusUninitialized = KStartupEnumerationFirstValue,
    ESimRejected,   // The PUK code has been entered incorrectly, so the card is rejected.
    ESimUnaccepted  // The SIM lock is on, so the card is unaccepted.
    */
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SimSecurityStatus()" );
    return CPhonePubSubProxy::Instance()->Value(
                    KPSUidStartup, KStartupSimSecurityStatus );
    }

// ----------------------------------------------------------------------------
// CPhoneState::SetRingingTonePlaybackL()
// ----------------------------------------------------------------------------
//
EXPORT_C void CPhoneState::SetRingingTonePlaybackL( TInt aCallId )
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::SetRingingTonePlaybackL()" );
    TPhoneCmdParamRingTone ringToneParam;
    ringToneParam.SetVolume(
        iStateMachine->PhoneEngineInfo()->RingingVolume() );

    TArray< TContactItemId > alertGroups =
        iStateMachine->PhoneEngineInfo()->AlertForGroup();
    
    TInt alertGroupCount = alertGroups.Count();
    TInt contactGroupCount =
        iStateMachine->PhoneEngineInfo()->ContactGroups( aCallId ).Count();
    TBool alertingGroupFound( EFalse );

    // Check contents of both the alerting groups array and call contact group
    // array, if any of the group ids match then allow playing.
    if ( alertGroupCount > 0 )
        {
        for ( TInt i = 0 ; i < alertGroupCount ; i++ )
            {
            for ( TInt j = 0 ; j < contactGroupCount ; j++ )
                {
                if ( alertGroups[ i ] ==
                     ( iStateMachine->PhoneEngineInfo()->ContactGroups(
                           aCallId ) )[ j ] )
                    {
                    alertingGroupFound = ETrue;
                    }
                }
            }
        if ( alertingGroupFound == EFalse )
            {
            ringToneParam.SetRingingType( EProfileRingingTypeSilent );
            }
        else
            {
            ringToneParam.SetRingingType(
                iStateMachine->PhoneEngineInfo()->RingingType() );
            }
        }
    else // no alerting groups set
        {
        ringToneParam.SetRingingType(
            iStateMachine->PhoneEngineInfo()->RingingType() );
        }

    // Set ring tone
    if ( iStateMachine->PhoneEngineInfo()->PersonalRingingTone(
             aCallId ).Length() )
        {
        // Set the personal ring tone
        ringToneParam.SetRingTone(
            iStateMachine->PhoneEngineInfo()->PersonalRingingTone( aCallId ) );
        ringToneParam.SetType( EPhoneRingTonePersonal );
        }
    else if ( iStateMachine->PhoneEngineInfo()->RingingTone(
                  aCallId ).Length() )
        {
        // Set the profile ring tone
        ringToneParam.SetRingTone(
            iStateMachine->PhoneEngineInfo()->RingingTone( aCallId ) );
        ringToneParam.SetType( EPhoneRingToneProfile );

        if ( ( iStateMachine->PhoneEngineInfo()->CallerImage( aCallId ).Length() > 0 &&
                BaflUtils::FileExists( CCoeEnv::Static()->FsSession(),
                        iStateMachine->PhoneEngineInfo()->CallerImage( aCallId ) ) ) ||
               iStateMachine->PhoneEngineInfo()->HasCallerThumbnail( aCallId ) )
            {
            // Caller image set
            ringToneParam.SetCallerImageStatus( ETrue );
            }
        }

    // Set text to say, if enabled
    if ( iStateMachine->PhoneEngineInfo()->TextToSpeech() )
        {
        ringToneParam.SetTextToSay(
            iStateMachine->PhoneEngineInfo()->RemoteTextToSpeechText(
                aCallId ) );
        }

    if ( iStateMachine->PhoneEngineInfo()->CallerText( aCallId ).Length() > 0 )
        {
        ringToneParam.SetCallerTextStatus( ETrue );
        }    
    iViewCommandHandle->ExecuteCommandL( EPhoneViewPlayRingTone, &ringToneParam );
    }

// -----------------------------------------------------------
// CPhoneState::HandleCugNoteL
// -----------------------------------------------------------
//
void CPhoneState::HandleCugInUseNoteL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleCugInUseNoteL() ");
    TInt cugIndex( 0 );
    RSSSettings ssSettings;
    TInt retValue = ssSettings.Open();
    if ( retValue == KErrNone )
        {
        ssSettings.Get( ESSSettingsCug, cugIndex  );
        }
    ssSettings.Close();
    if ( cugIndex )
        {
        // Add it to the resource string
        HBufC* buf = StringLoader::LoadLC(
            CPhoneMainResourceResolver::Instance()->
            ResolveResourceID(
            EPhoneInfoCugInUse ), cugIndex );
        
        PhoneNoteUtil::SendGlobalNoteL(
                EPhoneMessageBoxInformation, *buf );

        CleanupStack::PopAndDestroy( buf );
        }
    }

// ---------------------------------------------------------------------------
// CPhoneState::LoadResource
// ---------------------------------------------------------------------------
//
void CPhoneState::LoadResource( TDes& aData, const TInt aResource ) const
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneState::LoadResource() ");
    __PHONELOG1( EBasic, EPhoneControl, "CPhoneState::LoadResource - aResource: %d", aResource );
    StringLoader::Load(  
            aData,
            CPhoneMainResourceResolver::Instance()->ResolveResourceID( aResource ),
            EikonEnv() );
    }

// -----------------------------------------------------------
// CPhoneState::IsAnyConnectedCalls
// -----------------------------------------------------------
//
TBool CPhoneState::IsAnyConnectedCalls()
    {
    __LOGMETHODSTARTEND( EPhoneControl, "CPhoneState::IsAnyConnectedCalls() ");
    TBool connectedCall(EFalse);
    if ( iStateMachine->PhoneEngineInfo()->CheckIfCallStateExists( EPEStateConnected ) ||
        iStateMachine->PhoneEngineInfo()->CheckIfCallStateExists( EPEStateConnecting ) ||
        iStateMachine->PhoneEngineInfo()->CheckIfCallStateExists( EPEStateConnectedConference ) )
        {
        connectedCall = ETrue;
        }
    __PHONELOG1( EBasic, EPhoneControl, "CPhoneState::IsAnyConnectedCalls: (%d)", connectedCall );
    return connectedCall;
    }

// ---------------------------------------------------------
// CPhoneState::SetToolbarButtonLoudspeakerEnabled
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::SetToolbarButtonLoudspeakerEnabled()
    {
    TPhoneCmdParamInteger integerParam;
    integerParam.SetInteger( EPhoneInCallCmdActivateIhf );
    TRAP_IGNORE( iViewCommandHandle->ExecuteCommandL(
        EPhoneViewEnableToolbarButton, &integerParam ));
    }

// ---------------------------------------------------------
// CPhoneState::SetBackButtonActive
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::SetBackButtonActive( TBool aActive )
    {
    if( IsAutoLockOn() && aActive ) 
        {
        // keep back button dimmed device lock case
        return;
        }
    TPhoneCmdParamBoolean booleanParam;
    booleanParam.SetBoolean( aActive );
    iViewCommandHandle->ExecuteCommand(
        EPhoneViewBackButtonActive, &booleanParam );
    }

// -----------------------------------------------------------
// CPhoneState::OpenSoftRejectMessageEditorL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::OpenSoftRejectMessageEditorL()
    {
    __LOGMETHODSTARTEND(EPhoneControl,
        "CPhoneState::OpenSoftRejectMessageEditorL ()" );
    iUiCommandManager->SetSoftRejectStatus( EFalse );
    TPhoneCmdParamCallStateData callStateData;
    callStateData.SetCallState( EPEStateRinging );
    iViewCommandHandle->HandleCommandL(
        EPhoneViewGetCallIdByState, &callStateData );

    TInt ringingCallId( KErrNotFound );
    // Do nothing if negative incoming call id
    if( callStateData.CallId() > KErrNotFound  )
        {
        iViewCommandHandle->ExecuteCommandL( EPhoneViewRemoveQuery );
        ringingCallId = callStateData.CallId();
        }
    
    TPhoneCmdParamSfiData sfiDataParam;
    if (KErrNotFound != ringingCallId) 
        {
        if ( iStateMachine->PhoneEngineInfo()->
                RemoteName( ringingCallId ).Length() )
            {
            // store both the name and the number
            sfiDataParam.SetNumber( 
                    iStateMachine->PhoneEngineInfo()->RemotePhoneNumber( ringingCallId ) );
            sfiDataParam.SetName( 
                    iStateMachine->PhoneEngineInfo()->RemoteName( ringingCallId ) );
            }
        else
            {
            // store the number
            sfiDataParam.SetNumber( 
                    iStateMachine->PhoneEngineInfo()->RemotePhoneNumber( ringingCallId ) );
            }
        }
    UpdateUiCommands();
    iViewCommandHandle->ExecuteCommandL( 
            EPhoneViewOpenSoftRejectEditor, 
            &sfiDataParam );
    }

// -----------------------------------------------------------
// CPhoneState::DisplayCallHeaderL()
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::DisplayCallHeaderL( 
        TInt aCallId,
        TNumberEntrySetRule aRule )
    {
    __LOGMETHODSTARTEND(EPhoneControl, 
            "CPhoneState::BringIncomingCallHeaderToForegroundL( ) ");
    BeginUiUpdateLC();
    switch ( aRule )
       {
       case ESetNEVisibilityFalse:
           iNumberEntryManager->
               SetNumberEntryVisibilityL( EFalse );
           break;
       case ECheckIfNEUsedBeforeSettingVisibilityFalse:
           iNumberEntryManager->
               SetVisibilityIfNumberEntryUsedL(EFalse);
           break;
       default: // by default do nothing.
           break;
       }
    iViewCommandHandle->ExecuteCommandL( EPhoneViewRemovePhoneDialogs );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewBringPhoneAppToForeground );
    iViewCommandHandle->ExecuteCommandL( EPhoneViewCreateCallHeader, aCallId );
    UpdateUiCommands();
    EndUiUpdate();
    }

// ---------------------------------------------------------
// CPhoneState::SetTouchPaneButtons
// ---------------------------------------------------------
//
EXPORT_C void CPhoneState::UpdateUiCommands()
    {
    iUiCommandManager->UpdateUiCommands();
    }

// -----------------------------------------------------------
// CPhoneState::HandleNumberEntryClearedL
// -----------------------------------------------------------
//
EXPORT_C void CPhoneState::HandleNumberEntryClearedL()
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::HandleNumberEntryClearedL( ) ");
    }

// -----------------------------------------------------------
// CPhoneState::IsCustomizedDialerVisibleL()
// -----------------------------------------------------------
//
EXPORT_C TBool CPhoneState::IsCustomizedDialerVisibleL() const
    {
    __LOGMETHODSTARTEND(EPhoneControl, "CPhoneState::IsCustomizedDialerVisibleL( ) ");
    return iNumberEntryManager->IsCustomizedDialerVisibleL();
    }

//  End of File
