/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CPhoneResourceResolverBase class.
*
*/


// INCLUDE FILES
#include <bautils.h>
#include <eikenv.h>
#include <avkon.rsg>
#include <featmgr.h>
#include <telephonyvariant.hrh>

#include "phoneui.pan"
#include "cphoneresourceresolverbase.h"
#include "cphonemainresourceresolver.h"
#include "phoneconstants.h"
#include "phonerssbase.h"
#include "phonelogger.h"
#include "cphonecenrepproxy.h"

#include "phoneresourceids.h"
#include "tphonetouchbuttonconfig.h"
#include <data_caging_path_literals.hrh>

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPhoneResourceResolverBase::CPhoneResourceResolverBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CPhoneResourceResolverBase::CPhoneResourceResolverBase():
    iEnv( *CEikonEnv::Static() ) // codescanner::performance::eikonenvstatic
    {
    if ( FeatureManager::FeatureSupported( KFeatureIdVideocallMenuVisibility ) )
        {
        iVariationFlags |= EVideoCallMenu;
        }
    }

// Destructor
EXPORT_C CPhoneResourceResolverBase::~CPhoneResourceResolverBase()
    {
    // Remove resource file
    iEnv.DeleteResourceFile( iResourceOffset );
    iEnv.DeleteResourceFile( iCUIResourceOffset );
    iEnv.DeleteResourceFile( iPTCHResourceOffset );
    }

// -----------------------------------------------------------------------------
// CPhoneResourceResolverBase::BaseConstructL
// 
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CPhoneResourceResolverBase::BaseConstructL()
    { 
    // Add resource file
    // Create path to default resource file name
    TFileName path( KDriveZ );
    path.Append( KDC_APP_RESOURCE_DIR );
    path.Append( KPhoneResourceFile );
    
    RFs &fsSession= iEnv.FsSession();
    BaflUtils::NearestLanguageFile( fsSession, path ); 
    iResourceOffset = iEnv.AddResourceFileL( path );
    // callhandlingui.rss
    TFileName callHandlingPath( KDriveZ );
    callHandlingPath.Append( KDC_APP_RESOURCE_DIR );
    callHandlingPath.Append( KPhoneResourceFileCHUI );
    BaflUtils::NearestLanguageFile( fsSession, callHandlingPath ); 
    iCUIResourceOffset = iEnv.AddResourceFileL( callHandlingPath );
    // phoneuitouch.rss
    TFileName phoneUiTouchPath( KDriveZ );
    phoneUiTouchPath.Append( KDC_APP_RESOURCE_DIR );
    phoneUiTouchPath.Append( KPhoneResourceFilePHTC );
    BaflUtils::NearestLanguageFile( fsSession, phoneUiTouchPath ); 
    iPTCHResourceOffset = iEnv.AddResourceFileL( phoneUiTouchPath );

    iTouchButtonConfig.ReadConfiguration();
    }

EXPORT_C TBool CPhoneResourceResolverBase::IsTelephonyFeatureSupported( TInt aFeatureId ) const
    {
    return CPhoneCenRepProxy::Instance()->IsTelephonyFeatureSupported( aFeatureId );
    }
// -----------------------------------------------------------------------------
// CPhoneResourceResolverBase::ResolveResourceID
// Empty implementation, because there isn't common phoneapp.rss file.
// Panics, if called.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPhoneResourceResolverBase::ResolveResourceID(
    const TInt& aResource ) const
    {
    TInt retVal( KErrNotFound );
    __PHONELOG1( EBasic, EPhoneUIView, "CPhoneResourceResolverBase::ResolveResourceID - aResource(%d)",
        aResource );
    switch( aResource )
        {
        case EPhoneNumberBusy:
            retVal = R_PHONE_ERROR_NUMBER_BUSY;
            break;

        case EPhoneWaitingText:
            retVal = R_PHONE_SS_NOTIFICATION_MOCAL_WAITING_TEXT;
            break;

        case EPhoneIdleMenubar:
            retVal = R_PHONEUI_IDLE_MENUBAR;
            break;

        case EPhoneNumberAcqMenubar:
             if ( iVariationFlags & EVideoCallMenu )
                {
                retVal = R_PHONEUI_NUMBERACQ_VIDEOCALL_MENUBAR;
                }
            else
                {
                retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
                }
            break;

        case EPhoneNumberAcqOkMenubar:
            if ( iVariationFlags & EVideoCallMenu )
                {
                retVal = R_PHONEUI_NUMBERACQ_OK_VIDEOCALL_MENUBAR;
                }
            else
                {
                retVal = R_PHONEUI_NUMBERACQ_OK_MENUBAR;
                }
            break;

        case EPhoneCallHandlingMenubar:
            retVal = R_PHONEUI_CALLHANDLING_MENUBAR;
            break;

        case EPhoneCallHandlingEmergencyMenubar:
            retVal = R_PHONEUI_CALLHANDLING_EMERGENCY_MENUBAR;
            break;

        case EPhoneCallHandlingEmergencyMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_EMERGENCY_MENUBAR;
            break;

        case EPhoneIncomingCallMenubar:
            retVal = R_PHONEUI_INCOMINGCALL_MENUBAR;
            break;

       case EPhoneIncomingVideoCallMenubar:
            retVal = R_PHONEUI_INCOMINGVIDEOCALL_MENUBAR;
            break;

        case EPhoneIncomingCallMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;
            
        case EPhoneIncomingVideoCallMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case  EPhoneAlertingCallMenubar:
            retVal = R_PHONEUI_ALERTINGCALL_MENUBAR;
            break;

        case  EPhoneAlertingCallMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneCallActiveAndHeldMenubar:
            retVal = R_PHONEUI_ACTIVEANDHELD_MENUBAR;
            break;

        case EPhoneCallActiveAndHeldMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneCallWaitingMenubar:
            retVal = R_PHONEUI_CALLWAITING_MENUBAR;
            break;

        case EPhoneCallWaitingMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneCallActiveHeldAndWaitingMenubar:
            if ( !IsTelephonyFeatureSupported( KTelephonyLVFlagOptionsChange ))
                {
                retVal = R_PHONEUI_CALLACTIVEHELDANDWAITING_MENUBAR;
                }
            else
                {
                retVal = R_PHONEUI_CALLACTIVEHELDANDWAITING_MENUBAR_VARIED;
                }
            break;

        case EPhoneCallActiveHeldAndWaitingMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneAlertingAndHeldCallMenuBar:
            retVal = R_PHONEUI_ALERTINGANDHELDCALL_MENUBAR;
            break;

        case EPhoneAlertingAndHeldCallMenuBarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneAlertingAndConfHeldCallMenuBar:
            retVal = R_PHONEUI_ALERTINGANDCONFHELDCALL_MENUBAR;
            break;

        case EPhoneAlertingAndConfHeldCallMenuBarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneAlertingWaitingAndConfHeldCallMenuBar:
            retVal = R_PHONEUI_ALERTINGWAITINGANDHELDCONFCALL_MENUBAR;
            break;

        case EPhoneAlertingWaitingAndConfHeldCallMenuBarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneAlertingHeldAndWaitingCallMenuBar:
            retVal = R_PHONEUI_ALERTINGHELDANDWAITINGCALL_MENUBAR;
            break;

        case EPhoneAlertingHeldAndWaitingCallMenuBarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneAlertingAndWaitingCallMenuBar:
            retVal = R_PHONEUI_ALERTINGANDWAITINGCALL_MENUBAR;
            break;

        case EPhoneAlertingAndWaitingCallMenuBarWithNumberEntry:
            {
            retVal = R_PHONEUI_ALERTINGANDWAITINGCALL_MENUBAR_WITH_NUMBERENTRY;
            }
            break;

        case EPhoneConfCallMenubar:
           retVal = R_PHONEUI_TOUCH_CONFCALL_MENUBAR;
            break;

        case EPhoneConfCallMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneConfAndCallWaitingMenubar:
            retVal = R_PHONEUI_TOUCH_CONFANDCALLWAITING_MENUBAR;
            break;

        case EPhoneConfAndCallWaitingMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneConfAndHeldCallMenubar:
            retVal = R_PHONEUI_TOUCH_ACTIVECONFANDHELD_MENUBAR;
            break;

        case EPhoneConfAndHeldCallMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneActiveAndHeldConfMenubar:
            retVal = R_PHONEUI_TOUCH_ACTIVEANDHELDCONF_MENUBAR;
            break;

        case EPhoneActiveAndHeldConfMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneConfCallActiveHeldAndWaitingMenubar:
            retVal = R_PHONEUI_TOUCH_CONFCALLACTIVEHELDANDWAITING_MENUBAR;
            break;

        case EPhoneConfCallActiveHeldAndWaitingMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneCallActiveHeldConfAndWaitingMenubar:
            retVal = R_PHONEUI_TOUCH_CALLACTIVEHELDCONFANDWAITING_MENUBAR;
            break;

        case EPhoneCallActiveHeldConfAndWaitingMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

        case EPhoneConfCallParticipantsMenubar:
            retVal = R_PHONEUI_PARTICIPANT_LIST_MENUBAR;
            break;

        case EPhoneConfCallParticipantsDropMenubar:
            retVal = R_PHONEUI_PARTICIPANT_LIST_DROP_MENUBAR;
            break;

        case EPhoneCallHandlingMenubarWithNumberEntry:
            if ( iVariationFlags & EVideoCallMenu )
                {
                retVal = R_PHONE_INCALL_VIDEOCALL_MENUBAR_WITH_NUMBERENTRY;
                }
            else
                {
                retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
                }
            break;
            
        case EPhoneDtmfDialerMenubar:
            retVal = R_PHONEUIDIALER_DTMFVIEW_MENUBAR;
            break;

        case EPhoneEmptyCBA:
            retVal = R_PHONEUI_EMPTY_CBA;
            break;

        case EPhoneEmergencyIdleCBA:
            retVal = R_PHONEUI_EMERGENCY_IDLE_CBA;
            break;

        case EPhoneSendDtmfNoCBA:
            retVal = R_PHONEUI_SEND_DTMF_NO_CBA;
            break;

        case EPhoneSendDtmfEmptyEditBoxCBA:
            retVal = R_PHONEUI_SEND_DTMF_EMPTY_EDIT_BOX_CBA;
            break;

        case EPhoneSendDtmfNormalEditBoxCBA:
            retVal = R_PHONEUI_SEND_DTMF_NORMAL_EDIT_BOX_CBA;
            break;

        case EPhoneNumberAcqCBA:
            if ( iVariationFlags & EVideoCallMenu )
                  {
                  retVal = R_PHONEUI_NUMBERACQ_VIDEOCALL_CBA;
                  }
            else
                 {
                 retVal = R_PHONEUI_DIALER_CBA;
                 }
            break;

        case EPhoneInCallNumberAcqCBA:
            retVal = R_PHONEUI_INCALL_NUMBERACQ_CBA;
            break;

        case EPhoneSettingsUiCBA:
            retVal = R_PHONEUI_SETTINGSUI_CBA;
            break;

        case EPhoneCallHandlingCallSetupCBA:
            retVal = R_PHONEUI_CALLHANDLING_CALLSETUP_EMPTY_DTMFDIALER_CBA;
            break;

        case EPhoneCallHandlingCallSetupToIhfCBA:
            retVal = R_PHONEUI_CALLHANDLING_CALLSETUP_TOIHF_DTMFDIALER_CBA;
            break;

        case EPhoneCallHandlingCallSetupToHandsetCBA:
            retVal = R_PHONEUI_CALLHANDLING_CALLSETUP_TOHANDSET_DTMFDIALER_CBA;
            break;

        case EPhoneCallHandlingInCallCBA:
            retVal = R_PHONEUI_INCALL_DIALER_CBA;
            break;

        case EPhoneCallHandlingInCallNoIhfCBA:
            retVal = R_PHONEUI_INCALL_DIALER_CBA;
            break;

        case EPhoneCallHandlingInCallUnmuteCBA:
            retVal = R_PHONEUI_CALLHANDLING_INCALL_UNMUTE_CBA;
            break;

        case EPhoneCallHandlingInCallUnholdCBA:
            retVal = R_PHONEUI_INCALL_DIALER_CBA;
            break;

        case EPhoneCallHandlingInCallHandsetCBA:
            retVal = R_PHONEUI_CALLHANDLING_INCALL_HANDSET_CBA;
            break;

        case EPhoneCallHandlingInCallBtaaCBA:
            retVal = R_PHONEUI_CALLHANDLING_INCALL_BTAA_CBA;
            break;

        case EPhoneCallHandlingEmergencyCBA:
            retVal = R_PHONEUI_CALLHANDLING_EMERGENCY_CBA;
            break;

        case EPhoneCallHandlingEmergencyHandsetCBA:
            retVal = R_PHONEUI_INCALL_DIALER_CBA;
            break;

        case EPhoneCallHandlingEmergencyNoIhfCBA:
            retVal = R_PHONEUI_CALLHANDLING_EMERGENCY_NOIHF_CBA;
            break;

        case EPhoneCallHandlingEmergencyNoOptionsCBA:
            retVal = R_PHONEUI_CALLHANDLING_EMERGENCY_NO_OPTIONS_CBA;
            break;

        case EPhoneCallHandlingEmergencyNoOptionsHandsetCBA:
            retVal = R_PHONEUI_CALLHANDLING_EMERGENCY_NO_OPTIONS_HANDSET_CBA;
            break;

        case EPhoneCallHandlingEmergencyNoOptionsNoIhfCBA:
            retVal = R_PHONEUI_CALLHANDLING_EMERGENCY_NO_OPTIONS_NOIHF_CBA;
            break;

        case EPhoneCallHandlingIncomingCBA:
            retVal = R_PHONEUI_CALLHANDLING_INCOMINGCALL_CBA;
            break;

        case EPhoneCallHandlingIncomingRejectCBA:
            retVal = R_PHONEUI_CALLHANDLING_INCOMINGCALL_REJECT_CBA;
            break;

        case EPhoneCallHandlingIncomingSoftRejectCBA:
            retVal = R_PHONEUI_CALLHANDLING_INCOMINGCALL_SOFT_REJECT_CBA;
            break;

        case EPhoneCallHandlingCallWaitingCBA:
            retVal = R_PHONEUI_CALLHANDLING_CALLWAITING_CBA;
            break;

        case EPhoneCallHandlingNewCallFindCBA:
            retVal = R_PHONEUI_CALLHANDLING_NEW_CALL_FIND_CBA;
            break;

        case EPhoneCallHandlingNewCallCallCBA:
            retVal = R_PHONEUI_CALLHANDLING_NEW_CALL_CALL_CBA;
            break;

        case EPhoneCallHandlingNewCallSwapCBA:
            retVal = R_PHONEUI_INCALL_DIALER_CBA;
            break;

        case EPhoneNewCallFetchCBA:
            retVal = R_PHONEUI_NEW_CALL_FETCH_CBA;
            break;

        case EPhoneEmergencyExitCBA:
            retVal = R_PHONEUI_EXIT_EMERGENCY_MODE_CBA;
            break;

        case EPhoneEmergencyModeNoteCBA:
            retVal = R_PHONEUI_EMERGENCY_MODE_NOTE_CBA;
            break;

        case EPhoneDropParticipantCBA:
            retVal = R_CONFERENCE_DROPPARTICIPANT_SOFTKEYS;
            break;

        case EPhonePrivateParticipantCBA:
            retVal = R_CONFERENCE_PRIVATE_SOFTKEYS;
            break;

        case EPhoneCallHandlingInCallEndCallCBA:
            retVal = R_PHONEUI_CALLHANDLING_INCALL_ENDACTIVE_CBA;
            break;

        case EPhoneNumberNotInUse: // TPAUICmdParamMOTerminationIndicator related start here
            retVal = R_NOTETEXT_NUMBER_NOT_IN_USE;
            break;

        case EPhoneNumberBarred:
            retVal = R_NOTETEXT_NUMBER_BARRED;
            break;

        case EPhoneNumberNotInCUG:
            retVal = R_NOTETEXT_NUMBER_NOT_IN_CUG;
            break;

        case EPhoneNoAnswer:
            retVal = R_NOTETEXT_NO_ANSWER;
            break;

        case EPhoneNetworkBusy:
            retVal = R_NOTETEXT_NETWORK_BUSY;
            break;

        case EPhoneMOOutOfRange:
            retVal = R_NOTETEXT_OUT_OF_RANGE;
            break;

        case EPhoneMOCallFailed:
            retVal = R_NOTETEXT_CALL_FAILED;
            break;

        case EPhoneSignalFaded:
            retVal = R_NOTETEXT_SIGNAL_FADED;
            break;

        case EPhoneErrorInConnection:
            retVal = R_NOTETEXT_ERROR_IN_CONNECTION;
            break;

        case EPhoneInvalidPhoneNumber:
            retVal = R_NOTETEXT_INVALID_PHONENUMBER;
            break;

        case EPhoneEmptySimLocation:
            retVal = R_NOTETEXT_EMPTY_SIM_LOCATION;
            break;

        case EPhoneCLIConferenceCall:
            retVal = R_PHONEUI_CONFERENCE_CALL;
            break;

        case EPhoneEmergencyCallsOnly:
            retVal =R_PHONE_ERROR_EMERGENCY_CALLS_ONLY;
            break;

        case EPhoneNoteTextCallNotAllowed:
            retVal = R_PHONE_ERROR_CALL_NOT_ALLOWED;
            break;

        case EPhoneNoteTextCallNotAllowedFDN:
            retVal = R_PHONE_ERROR_CALL_NOT_ALLOWED_FDN;
            break;

        case EPhoneNoteTextCheckNetworkservices:
            retVal = R_PHONE_ERROR_CHECK_NETWORK_SERVICES;
            break;

        case EPhoneNoteTextRequestRejected:
            retVal = R_PHONE_ERROR_REQUEST_REJECTED;
            break;

       case EPhoneNoteTextNotAllowed:
            retVal = R_ERROR_NOTE_NOT_ALLOWED;
            break;

       case EPhoneNoteNoNetworkCallEmergency:
            retVal = R_PHONEUI_ERROR_EMERGENCY_ATTEMPT_FAILED;
            break;

        case EPhoneNoteNoVideoNetwork:
            retVal = R_PHONEUI_NO_VIDEO_NETWORK;
            break;

        case EPhoneVideoCallIncoming:
            retVal = R_PHONEUI_VIDEO_CALL_INCOMING;
            break;

        case EPhoneVideoCallIncomingShort:
            retVal = R_PHONEUI_VIDEO_CALL_INCOMING_SHORT;
            break;

       case EPhoneVideoCall2gMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

       case EPhoneVideoCallMenubar:
            retVal = R_PHONEUI_VIDEOCALL_MENUBAR;
            break;

       case EPhoneVideoCallMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

       case EPhoneVideoCallWaitingMenubar:
            retVal = R_PHONEUI_VIDEOCALL_WAITING_MENUBAR;
            break;

       case EPhoneCallWaitingLockMenubar:
            retVal = R_PHONEUI_WAITING_LOCK_MENUBAR;
            break;

       case EPhoneCallTwoSinglesWaitingLockMenubar:
            retVal = R_PHONEUI_TWOSINGLES_WAITING_LOCK_MENUBAR;
            break;

       case EPhoneVideoCallWaitingMenubarWithNumberEntry:
            retVal = R_PHONEUIDIALER_NUMBERACQ_MENUBAR;
            break;

       case EPhoneEmptyBackCBA:
            retVal = R_PHONEUI_SOFTKEYS_EMPTY_BACK;
            break;

        case EPhoneInfoHandsfreeDeactivated:
            retVal = R_AVKON_IHF_DEACTIVATED_CONFIRMATION_NOTE_TEXT;
            break;

        case EPhoneInfoHandsetActivated:
            retVal = R_NOTE_TEXT_CONF_AUDIOS_HANDSET;
            break;
            
        case EPhoneInfoBTAccActivated:
            retVal = R_NOTE_TEXT_CONF_AUDIOS_BT_ACCESSORY;
            break;

        case EPhoneStringList:
            retVal = R_PHONEUI_STRING_LIST;
            break;

        case EPhoneAllIncomingCallsDiverted:
            retVal = R_NOTETEXT_ALL_INCOMING_CALLS_DIVERTED;
            break;

        case EPhoneActiveDiverts:
            retVal = R_NOTETEXT_ACTIVE_DIVERTS;
            break;

        case EPhoneDefaultSkeyList:
            retVal = R_PHONEUI_DEFAULT_SKEY_LIST;
            break;

        case EPhoneEmergencyCallHeader:
            retVal = R_PHONEUI_EMERGENCY_CALL_HEADER;
            break;

        case EPhoneEmergencyConnectWaitNote:
            retVal = R_PHONE_EMERGENCY_NETWORK_CONNECT_WAIT_NOTE_EMPTY_DIALER;
            break;

        case EPhoneEmergencyModeInformationNote:
            retVal = R_PHONE_EMERGENCY_MODE_INFORMATION_NOTE;
            break;

        case EPhoneExitEmergencyModeQuery:
            retVal = R_PHONE_EXIT_EMERGENCY_MODE_QUERY;
            break;

        case EPhoneIncomingCallKeysLocked:
            retVal = R_NOTETEXT_USE_KEYGUARD_TO_UNLOCK;
            break;

        case EPhoneIncomingCallLabel:
            retVal = R_PHONEUI_MTCAL_INCOMING_CALL;
            break;

        case EPhoneIncomingCallLabelShort:
            retVal = R_PHONEUI_MTCAL_INCOMING_CALL_SHORT;
            break;

        case EPhoneOutgoingCallLabel:
            retVal = R_PHONEUI_OUT_GOING_CALL;
            break;

        case EPhoneOutgoingCallLabelShort:
            retVal = R_PHONEUI_OUT_GOING_CALL_SHORT;
            break;

        case EPhoneOutgoingVideoCallLabel:
            retVal = R_PHONEUI_OUT_GOING_VIDEO_CALL;
            break;

        case EPhoneOutgoingVideoCallLabelShort:
            retVal = R_PHONEUI_OUT_GOING_VIDEO_CALL_SHORT;
            break;

        case EPhoneCallDurationFormatString:
            retVal = R_PHONEUI_TIME_DURAT_LONG_WITH_ZERO;
            break;

        case EPhoneDtmfNumberQuery:
            retVal = R_PHONEUI_SEND_DTMF_QUERY;
            break;

        case EPhoneSendingDtmfWaitNote:
            retVal = R_PHONEUI_SENDING_DTMF_WAIT_NOTE;
            break;

        case EPhoneSendingDtmfWaitNoteText:
            retVal = R_PHONEUI_SENDING_DTMF_WAIT_NOTE_TEXT;
            break;

        case EPhoneDtmfWaitCharacterConfirmationQuery:
            retVal = R_PHONEUI_DTMF_WAIT_CHARACTER_CONFIRMATION_QUERY;
            break;

        case EPhoneDtmfWaitCharacterConfirmationQueryText:
            retVal = R_PHONEUI_DTMF_WAIT_CHARACTER_CONFIRMATION_QUERY_TEXT;
            break;

        case EPhoneDtmfFetchTitle:
            retVal = R_PHONEUI_TITLE_IN_DTMF_FETCH;
            break;

        case EPhoneAlsLineChangeConfirmationQuery:
            retVal = R_PHONEUI_ALS_CONFIRMATION_QUERY;
            break;

        case EPhoneAlsLineBlockingNoteText:
            retVal = R_PHONEUI_LINE_BLOCKING_NOTE_TEXT;
            break;

        case EPhoneIncomingLine2Text:
            retVal = R_PHONEUI_MTCAL_INCOMING_LINE2;
            break;

        case EPhoneIncomingLine2WaitingText:
            retVal = R_PHONEUI_MTCAL_WAITING_NUMBER_LINE2;
            break;

        case EPhoneIncomingLine2CallingText:
            retVal = R_PHONEUI_MTCAL_ARRIVING_NUMBER_LINE2;
            break;

        case EPhoneCallWaitingLabel:
            retVal = R_PHONEUI_MULTC_WAITING;
            break;

        case EPhoneCallWaitingWithoutLabel:
            retVal = R_PHONEUI_MULTC_INFO_WAIT_WITHOUT;
            break;

        case EPhoneCallWaitingWithLabel:
            retVal = R_PHONEUI_MULTC_INFO_WAIT_WITH;
            break;

        case EPhoneGprsConnInProgress:
            retVal = R_PHONEUI_GPRS_CONN_IN_PROGRESS;
            break;

        case EPhoneCallWaitingLabelShort:
            retVal = R_PHONEUI_MULTC_WAITING_SHORT;
            break;

        case EPhoneNewCallPhoneNumberEditor:
            retVal = R_PHONEUI_NEW_CALL_PHONE_NUMBER_EDITOR;
            break;

        case EPhoneNewCallFetchTitle:
            retVal = R_PHONEUI_TITLE_IN_NEW_CALL_FETCH;
            break;

        case EPhoneDisconnectingNetworkNote:
            retVal = R_PHONE_DISCONNECTING_NETWORK_WAIT_NOTE;
            break;

        case EPhoneInCallMutedText:
            retVal = R_PHONE_INCALL_MUTED_PANE;
            break;

        case EPhoneInCallDisconnected:
            retVal = R_PHONEUI_INCALL_CALL_DISCONNECTED;
            break;

        case EPhoneCallInProgress:
            retVal = R_PHONE_ERROR_CALL_IN_PROGRESS;
            break;

        case EPhoneConferenceDropParticipantDialog:
            retVal = R_CONFERENCE_DROPPARTICIPANT_DIALOG;
            break;

        case EPhoneConferencePrivateDialog:
            retVal = R_CONFERENCE_PRIVATE_DIALOG;
            break;

        case EPhoneInformationNote:
            retVal = R_PHONE_INFORMATION_NOTE;
            break;

        case EPhoneInformationWaitNote:
            retVal = R_PHONE_INFORMATION_NOTE_WITH_WAIT;
            break;

        case EPhoneInformationErrorNote:
            retVal = R_PHONE_ERROR_NOTE;
            break;

        case EPhoneInformationWarningNote:
            retVal = R_PHONE_WARNING_NOTE;
            break;

        case EPhoneInformationConfirmationNote:
            retVal = R_PHONE_CONFIRMATION_NOTE;
            break;

        case EPhoneSecurityInformationNote:
            retVal = R_PHONE_SECURITY_INFORMATION_NOTE;
            break;
        case EPhonePhoneBtDevAddress:
            retVal = R_PHONE_BT_DEV_ADDRESS;
            break;

        case EPhoneInformationRemotePutOnHoldNote:
            retVal = R_INCAL_INFO_HOLD_TEXT;
            break;

        case EPhoneInformationConnectedNote:
            retVal = R_INCAL_REMOTE_ACTIVE_TEXT;
            break;

        case EPhoneInformationRemoteCreateConferenceNote:
            retVal = R_INCAL_REMOTE_CREATE_CONFERENCE_TEXT;
            break;

        case EPhoneInformationCallOnHold:
            retVal = R_PHONE_SS_NOTIFICATION_INCAL_INFO_HOLD_TEXT;
            break;

        case EPhoneInCallTransferred:
            retVal = R_PHONE_SS_NOTIFICATION_INCAL_TRANSFERRED_TEXT;
            break;

        case EPhoneInformationCallOnHoldCli:
            retVal = R_INCAL_INFO_HOLD_CLI_TEXT;
            break;

        case EPhoneInformationConferenceOnHold:
            retVal = R_INCAL_INFO_CONF_HOLD_TEXT;
            break;

        case EPhoneInformationCallOnHoldNoCli:
            retVal = R_INCAL_INFO_HOLD_NOCLI_TEXT;
            break;

        case EPhoneInformationConferenceActiveted:
            retVal = R_MULTC_INFO_CONFERENCE_TEXT;
            break;

        case EPhoneCallOnHold:
            retVal = R_PHONEUI_INCALL_CALL_HELD;
            break;

        case EPhoneInformationCallActiveted:
            retVal = R_MULTC_UNHOLD_DONE_TEXT;
            break;

        case EPhoneRequestingNote:
            retVal = R_REQUESTING_WAIT_NOTE;
            break;

        case EPhoneInformationCallActivetedCli:
            retVal = R_MULTC_CLI_UNHOLD_DONE_TEXT;
            break;

        case EPhoneDiverting:
            retVal = R_NOTETEXT_DIVERTING;
            break;

        case EPhoneMtCallDiverting:
            retVal = R_NOTETEXT_DIVERTING_INCOMING_CALL;
            break;

        case EPhoneActiveBarrings:
            retVal = R_NOTETEXT_CALL_BARRINGS;
            break;

        case EPhoneCLIWithheld:
            retVal = R_PHONEUI_MTCAL_CLI_WITHHELD;
            break;

        case EPhoneCallCLIUnknown:
            retVal = R_PHONEUI_MTCAL_CLI_UNKNOWN;
            break;

        case EPhoneNoteTextCallSummary:
            retVal = R_PHONEUI_NOTE_TEXT_POST_CALL_SUMMARY;
            break;

        case EPhoneNoteTextCallDuration:
            retVal = R_PHONEUI_NOTE_TEXT_POST_CALL_DURATION;
            break;

        case EPhoneIdleTimeFormat:
            retVal = R_PHONEUI_IDLE_TIME_FORMAT;
            break;

        case EPhoneNoteTextPEFailedAtStartup:
            retVal = R_PHONEUI_NOTE_TEXT_PHONE_ENGINE_FAILED_AT_STARTUP;
            break;

        case EPhoneCLIPayphone:
            retVal = R_PHONE_MTCAL_CLI_PAYPHONE;
            break;

        case EPhoneOutgoingCallsBarredWithinCUG:
            retVal = R_PHONE_ERROR_CALL_BARRED_IN_CUG;
            break;

        case EPhoneNoCUGSelected:
            retVal = R_PHONE_ERROR_SELECT_CUG;
            break;

        case EPhoneUnknownCUGIndex:
            retVal = R_PHONE_ERROR_CUG_UNKNOWN;
            break;

        case EPhoneCUGIndexIncompatible:
            retVal = R_PHONE_ERROR_NOT_POSSIBLE_IN_CUG;
            break;

        case EPhoneCUGCallsFailure:
            retVal = R_PHONE_ERROR_CHECK_CUG;
            break;

        case EPhoneCLIRNotSubscribed:
            retVal = R_PHONE_ERROR_CHECK_CLIR;
            break;

        case EPhoneSSNotifCLIRSupprReject:
            retVal = R_PHONE_SS_NOTIFICATION_CLIR_SUPPR_REJECT_TEXT;
            break;

        case EPhoneWlanMacAddress:
            retVal = R_PHONE_WLAN_MAC_STRING;
            break;

        case EPhoneColpConnected:
            retVal = R_PHONE_TEXT_COLP_CONNECTED;
            break;

        case EPhoneBtLoopbackEnabled:
            retVal = R_PHONE_BT_LOOPBACK_ENABLED_STRING;
            break;

        case EPhoneBtLoopbackDisabled:
            retVal = R_PHONE_BT_LOOPBACK_DISABLED_STRING;
            break;

            
        case EPhoneInformationNoNetworkSupportForVideoCallNote:
            retVal =  R_PHONE_INCALL_INFO_NO_NETWORK_SUPPORT;
            break;

        case EPhoneInformationVideoCallNotAllowedDuringRestoreNote:
            retVal =  R_PHONE_INCALL_INFO_VIDEO_CALL_NOT_ALLOWED_DURING_RESTORE;
            break;

        case EPhoneSimRejected:
            retVal = R_PHONEUI_SIM_REJECTED;
            break;

        case EPhoneSimUnaccepted:
            retVal = R_PHONEUI_SIM_UNACCEPTED;
            break;

        case EPhoneSimRemoved:
            retVal = R_PHONEUI_SIM_REMOVED;
            break;

        case EPhoneRebootRequired:
            retVal = R_PHONEUI_REBOOT_REQUIRED;
            break;

        case EPhoneNoteVideoCallNotPossible:
            retVal = R_PHONEUI_VIDEO_CALL_NOT_POSSIBLE;
            break;

        case EPhoneContinueInHandsetConfirmationQuery:
            retVal = R_PHONEUI_CONTINUE_IN_HANDSET;
            break;

        case EPhoneContinueInLoudspeakerConfirmationQuery:
            retVal = R_PHONEUI_CONTINUE_IN_LOUDSPEAKER;
            break;

        case EPhoneDisconnectingQueryCBA:
            retVal = R_AVKON_SOFTKEYS_YES_NO;
            break;

        case EPhoneAlertingVideoCallMenubar:
            retVal = R_PHONEUI_ALERTINGVIDEOCALL_MENUBAR;
            break;

        case EPhoneCallHandlingIncomingSliderCBA:
            retVal = R_PHONEUI_CALLCOMING_SOFTKEYS_WHEN_SLIDER;
            break;

        case EPhoneCallHandlingIncomingSilentSliderCBA:
            retVal = R_PHONEUI_CALLCOMING_SOFTKEYS_WHEN_SILENT_SLIDER;
            break;

        case EPhoneCallHandlingIncomingSoftRejectSliderCBA:
            retVal = R_PHONEUI_SOFTKEYS_OPTIONS_SOFTREJECT_SLIDER;
            break;

        case EPhoneCallHandlingIncomingSwivelClosedCBA:
            retVal = R_PHONEUI_CALLCOMING_SWIVEL_CLOSED_CBA;
            break;

        case EPhoneCallHandlingIncomingSilentSwivelClosedCBA:
            retVal = R_PHONEUI_CALLCOMING_SILENT_SWIVEL_CLOSED_CBA;
            break;

        case EPhoneCallHandlingWaitingAnswerSwivelClosedCBA:
            retVal = R_PHONEUI_WAITING_ANSWER_SWIVEL_CLOSED_CBA;
            break;

        case EPhoneCallHandlingWaitingReplaceSwivelClosedCBA:
            retVal = R_PHONEUI_WAITING_REPLACE_SWIVEL_CLOSED_CBA;
            break;

        case EPhoneNoteVideoCallSetupFailed:
            retVal = R_PHONE_INCALL_INFO_VIDEO_CALL_SETUP_FAILED;
            break;

        case EPhoneNoteNoService:
            retVal = R_PHONE_ERROR_NO_SERVICE;
            break;

        case EPhoneIncallButtons:
            retVal = R_PHONEUI_INCALL_BUTTONS;
            break;

        case EPhoneTwoSinglesButtons:
            retVal = R_PHONEUI_TWO_SINGLES_BUTTONS;
            break;

        case EPhoneConferenceButtons:
            retVal = R_PHONEUI_CONFERENCE_BUTTONS;
            break;

        case EPhoneParticipantListButtons:
            retVal = R_PHONEUI_PARTICIPANT_LIST_BUTTONS;
            break;

        case EPhoneConferenceAndSingleButtons:
            retVal = R_PHONEUI_CONFERENCE_AND_SINGLE_BUTTONS;
            break;

        case EPhoneConferenceAndHeldSingleButtons:
            retVal = R_PHONEUI_CONFERENCE_AND_HELD_SINGLE_BUTTONS;
            break;

        case EPhoneEmergencyCallButtons:
            retVal = R_PHONEUI_EMERGENCY_CALL_BUTTONS;
            break;

        case EPhoneParticipantListCBA:
            retVal = R_CONFERENCE_PARTICIPANT_LIST_SOFTKEYS;
            break;

        case EPhoneAudioBTAButton:
            retVal = R_PHONEUI_BT_TOGGLE_BUTTON;
            break;

        case EPhoneAudioDefButton:
            retVal = R_PHONEUI_IHF_TOGGLE_BUTTON;
            break;

        case EPhoneIncomingCallButtons:
            retVal = iTouchButtonConfig.ResourceId( 
                    EPhoneConfigIncomingCallButtons,
                    R_PHONEUI_INCOMING_CALL_BUTTONS );
            break;

        case EPhoneCallSetupButtons:
            retVal = R_PHONEUI_CALL_SETUP_BUTTONS;
            break;

        case EPhoneWaitingCallButtons:
            retVal = R_PHONEUI_WAITING_CALL_BUTTONS;
            break;

        case EPhoneCallSetupAndSingleButtons:
            retVal = R_PHONEUI_CALL_SETUP_AND_SINGLE_BUTTONS;
            break;

        case EPhoneNotePhoneOutOf3GCoverage:
            retVal = R_NOTE_PHONE_OUT_OF_3G_COVERAGE;
            break;

        case EPhoneNoteCallInfoCauseValue16:
            retVal = R_CALL_INFO_CAUSE_VALUE16;
            break;

        case EPhoneNoteCallInfoCauseValue18:
            retVal = R_CALL_INFO_CAUSE_VALUE18;
            break;

        case EPhoneNoteCalledNumberHasBarredIncomingCalls:
            retVal = R_NOTE_CALLED_NUMBER_HAS_BARRED_INCOMING_CALLS;
            break;
            
        case EPhoneNoteCallInfoCauseValue21:
            retVal = R_CALL_INFO_CAUSE_VALUE21;
            break;

        case EPhoneNoteCallInfoCauseValue22:
            retVal = R_CALL_INFO_CAUSE_VALUE22;
            break;

        case EPhoneNoteCallInfoCauseValue34:
            retVal = R_CALL_INFO_CAUSE_VALUE34;
            break;

        case EPhoneNoteCallInfoCauseValue38:
            retVal = R_CALL_INFO_CAUSE_VALUE38;
            break;

        case EPhoneNoteVideoCallOnlyPossibleUnder3GCoverage:
            retVal = R_NOTE_VIDEO_CALL_ONLY_POSSIBLE_UNDER_3G_COVERAGE;
            break;

        case EPhoneNoteCallInfoCauseValue55:
            retVal = R_CALL_INFO_CAUSE_VALUE55;
            break;

        case EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone:
            retVal = R_NOTE_UNABLE_TO_MAKE_VIDEO_CALL_NOT_SUPPORTED_BY_OTHER_PHONE;
            break;

        case EPhoneNoteCallInfoCauseValue58:
            retVal = R_CALL_INFO_CAUSE_VALUE58;
            break;

        case EPhoneNoteCallInfoCauseValue127:
            retVal = R_CALL_INFO_CAUSE_VALUE127;
            break;

        case EPhoneNoteCallInfoNotSupported:
            retVal = R_CALL_INFO_NOT_SUPPORTED;
            break;

        case EPhoneNoteCallInfoServiceNotAvailable:
            retVal = R_CALL_INFO_SERVICE_NOT_AVAILABLE;
            break;

        case EPhoneDtmfNumberListQuery:
            retVal = R_PHONE_DTMF_LIST_QUERY;
            break;

        case EPhoneNoteTTYNoAudioControl:
            retVal = R_PHONE_TTY_INFO_NO_AUDIO_CONTROL;
            break;

        case EPhoneAddtoContactsListQuery:
            retVal = R_DIALER_ADD_TO_CONTACTS_SELECTION_LIST_QUERY;
            break;

        case EPhoneDtmfDialerCBA:
            retVal = R_PHONEUI_DTMFDIALER_SOFTKEYS_OPTIONS_CANCEL;
            break;

        case EPhoneDtmfDialerNumberEntryPromptText:
            retVal = R_PHONEUI_DIALER_NUMBERENTRY_PROMPT_TEXT;
            break;

        case EPhoneVideoCallSetupFailedCreateVoiceCallToTheSameContactText:
            retVal = R_NOTE_INCAL_QUERY_VCALL_FAILED;
            break;

        case EPhoneInfoClirChange:
            retVal = R_NOTE_TEXT_CLIR_CHANGE;
            break;

        case EPhoneInfoCugInUse:
            retVal = R_NOTE_TEXT_CUG_IN_USE;
            break;

        case EPhoneCallHandlingEmergencyInCallNoOptions:
            retVal = R_PHONEUI_CALLHANDLING_EMERGENCY_INCALL_NO_OPTIONS;
            break;

        case EPhoneIncallHeldButtons:
            retVal = R_PHONEUI_INCALL_HELD_BUTTONS;
            break;
            
        case EPhoneAttemptingEmergencyText:
            retVal = R_PHONEUI_ATTEMPTING_EMERGENCY_CALL_TEXT;
            break;

        default:
            Panic( EPhoneUtilsResourcePhoneappNotFound );
            break;
        }
    return retVal;
    }

//  End of File
