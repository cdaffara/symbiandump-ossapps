/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*     File containing enumeration for PhoneUI resources used from
*     common code.
*
*/


#ifndef PHONERSSBASE_H
#define PHONERSSBASE_H

//  INCLUDES
#include <e32std.h>
#include "phoneconstants.h"

// DATA TYPES
enum TPhoneRssBase
    {
    EPhoneStringList = KPhoneRssCommonFirst,
    EPhoneDefaultSkeyList,
    EPhoneEmergencyCallLabel,
    EPhoneEmergencyCallHeader,
    EPhoneEmergencyConnectWaitNote,
    EPhoneEmergencyModeInformationNote,
    EPhoneExitEmergencyModeQuery,
    EPhoneIncomingCallLabel,
    EPhoneIncomingCallLabelShort,
    EPhoneIncomingCallKeysLocked,
    EPhoneOutgoingVideoCallLabel,
    EPhoneOutgoingVideoCallLabelShort,
    EPhoneOutgoingCallLabel,
    EPhoneOutgoingCallLabelShort,
    EPhoneCallDurationFormatString,
    EPhoneCallWaitingLabel,
    EPhoneCallWaitingLabelShort,
    EPhoneCallWaitingWithoutLabel,
    EPhoneCallWaitingWithLabel,
    EPhoneMulticallLabel,
    EPhoneGprsConnInProgress,
    EPhoneNewCallPhoneNumberEditor, //20
    EPhoneNewCallFetchTitle,
    EPhoneDisconnectingNetworkNote,
    EPhoneInformationNote,
    EPhoneInformationWaitNote,
    EPhoneInformationErrorNote,
    EPhoneInformationWarningNote,
    EPhoneInformationConfirmationNote,
    EPhoneSecurityInformationNote,
    EPhoneRequestingNote,
    EPhonePhoneBtDevAddress,
    EPhoneInformationRemoteCreateConferenceNote,
    EPhoneInformationRemotePutOnHoldNote,
    EPhoneInformationConnectedNote,
    EPhoneInformationCallOnHold,
    EPhoneInformationCallActiveted,
    EPhoneInformationCallOnHoldCli,
    EPhoneInformationConferenceOnHold,
    EPhoneInformationConferenceActiveted,
    EPhoneInformationCallActivetedCli, //40
    EPhoneInformationCallOnHoldNoCli,
    EPhoneInformationNoNetworkSupportForVideoCallNote,
    EPhoneInformationVideoCallNotAllowedDuringRestoreNote,
    EPhoneCallOnHold,

    EPhoneNoteTextCallNotPossibleWhenCallInCallSetupState ,
    EPhoneNoteTextCallNotPossibleWhenAlreadyMaxNumberOfCalls,
    EPhoneNoteTextCallNotPossibleWhenOnlyIncomingCall,
    EPhoneNoteTextJoinNotPossibleWhenOnlyOneCall,
    EPhoneNoteTextJoinNotPossibleWhenAlreadyMaxNumOfMembers,
    EPhoneNoteTextAnswerNotPossibleWhenCallInCallSetupState,
    EPhoneNoteTextAnswerNotPossibleWhenAlreadyMaxNumOfCalls,
    EPhoneNoteTextCallNotAllowed,
    EPhoneNoteTextCallNotAllowedFDN,
    EPhoneNoteTextHoldNotPossibleWhenCallInCallSetupState,
    EPhoneNoteTextActivateNotPossibleWhenCallInCallSetupState,
    EPhoneNoteSimNotReady,
    EPhoneNoteTextCallSummary,
    EPhoneNoteTextCallDuration,
    EPhoneNoteNoNetworkCallEmergency,
    EPhoneNoteNoVideoNetwork, //60
    EPhoneNoteTextRequestRejected,
    EPhoneNoteTextCheckNetworkservices,
    EPhoneInformationBannerCallEnded,
    EPhoneCLIConferenceCall,
    EPhoneCLIEmergencyCall,
    EPhoneCLIConferenceCallParticipants,
    EPhoneInfoHandsfreeDeactivated,
    EPhoneInfoHandsetActivated,
    EPhoneInfoBTAccActivated,
    EPhoneInfoMicrophoneMuted,
    EPhoneInfoMicrophoneUnmuted,

    EPhoneNumberNotInUse,
    EPhoneNumberBusy,
    EPhoneNumberBarred,
    EPhoneNumberNotInCUG,
    EPhoneNoAnswer,
    EPhoneMOOutOfRange,
    EPhoneNetworkBusy,
    EPhoneSignalFaded,
    EPhoneErrorInConnection,
    EPhoneInvalidPhoneNumber,
    EPhoneOutgoingCallsBarredWithinCUG,
    EPhoneNoCUGSelected,
    EPhoneUnknownCUGIndex,
    EPhoneCUGIndexIncompatible,
    EPhoneCUGCallsFailure,
    EPhoneCLIRNotSubscribed,
    EPhoneFDNCallBlock,
    EPhoneMOCallFailed,
    EPhoneNoNetworkCoverage,
    EPhoneCLIWithheld,
    EPhoneWaitingText,
    EPhoneIdleTimeFormat,
    EPhoneInCallDisconnected,
    EPhoneCallInProgress,
    EPhoneEmergencyCallsOnly,
    EPhoneNoteTextNotAllowed,
    EPhoneEmptySimLocation, //100
    EPhoneActiveDiverts,
    EPhoneAllIncomingCallsDiverted,
    EPhoneDiverting,
    EPhoneMtCallDiverting,
    EPhoneActiveBarrings,
    EPhoneVideoCallIncoming,
    EPhoneVideoCallIncomingShort,

    // "Muted"
    EPhoneInCallMutedText,
    // "Unknown call"
    EPhoneCallCLIUnknown,
    /** "Transferring" -information note. */
    EPhoneInCallWaitTransfer,
    EPhoneInCallTransferred,
    EPhoneInCallTransferredWithNro,
    // the "DTMF Number" Query Dialog for DTMF.
    EPhoneDtmfNumberQuery,

    // the "Sending..." Wait Note for DTMF.
    EPhoneSendingDtmfWaitNote,

    // the text, "Sending...\n%U", used to produce
    // the text for the wait-dialog.
    EPhoneSendingDtmfWaitNoteText,

    // the confirmation note, seen when processing
    // the 'wait' character in a DTMF string.
    EPhoneDtmfWaitCharacterConfirmationQuery,

    // the text, "Send DTMF:\n%U", used to produce
    // the text for the confirmation dialog.
    EPhoneDtmfWaitCharacterConfirmationQueryText,

    EPhoneDtmfFetchTitle,

    // ALS
    EPhoneAlsLineChangeConfirmationQuery,
    EPhoneAlsLineBlockingNoteText,
    EPhoneInCallLine2,
    EPhoneIncomingLine2Text,
    EPhoneIncomingLine2WaitingText,
    EPhoneIncomingLine2CallingText,

    EPhoneConferenceDropParticipantDialog,
    EPhoneConferencePrivateDialog,

    EPhoneIdleMenubar,
    EPhoneNumberAcqMenubar,           // include [Call] menu item
    EPhoneNumberAcqOkMenubar,         // OK menu with [Call] menu item
    EPhoneCallHandlingMenubar,
    EPhoneCallHandlingMenubarWithNumberEntry,
    EPhoneCallHandlingMenubarWithSendNumberEntry,
    EPhoneCallHandlingHeldMenubarWithNumberEntry,
    EPhoneCallHandlingHeldMenubarWithSendNumberEntry,
    EPhoneCallHandlingHeldMenubar,
    EPhoneCallHandlingHeldUnmuteMenubar,
    EPhoneCallHandlingUnmuteMenubar,
    EPhoneCallHandlingEmergencyMenubar,
    EPhoneCallHandlingEmergencyMenubarWithNumberEntry,
    EPhoneIncomingCallMenubar,
    EPhoneIncomingCallMenubarWithNumberEntry,
    EPhoneAlertingCallMenubar,
    EPhoneAlertingCallMenubarWithNumberEntry,
    EPhoneIncomingVideoCallMenubar,
    EPhoneIncomingVideoCallMenubarWithNumberEntry,

    // Multicall cases
    EPhoneCallActiveAndHeldMenubar, //150
    EPhoneCallActiveAndHeldMenubarWithNumberEntry,
    EPhoneCallWaitingMenubar,
    EPhoneCallWaitingMenubarWithNumberEntry,
    EPhoneCallHeldAndWaitingMenubar,
    EPhoneCallActiveHeldAndWaitingMenubar,
    EPhoneCallActiveHeldAndWaitingMenubarWithNumberEntry,
    EPhoneAlertingAndHeldCallMenuBar,
    EPhoneAlertingAndHeldCallMenuBarWithNumberEntry,
    EPhoneAlertingHeldAndWaitingCallMenuBar,
    EPhoneAlertingHeldAndWaitingCallMenuBarWithNumberEntry,
    EPhoneAlertingAndWaitingCallMenuBar,
    EPhoneAlertingAndWaitingCallMenuBarWithNumberEntry,
    EPhoneCallWaitingLockMenubar,
    EPhoneCallTwoSinglesWaitingLockMenubar,

    // Video call cases
    EPhoneVideoCall2gMenubarWithNumberEntry,
    EPhoneVideoCallWaitingMenubar,
    EPhoneVideoCallWaitingMenubarWithNumberEntry,
    EPhoneVideoCallMenubar,
    EPhoneVideoCallMenubarWithNumberEntry,
    EPhoneAlertingVideoCallMenubar,
    EPhoneVideoCallSetupFailedCreateVoiceCallToTheSameContactText,

    //Conference call cases
    EPhoneConfCallMenubar,
    EPhoneConfCallMenubarWithNumberEntry,
    EPhoneConfAndCallWaitingMenubar,
    EPhoneConfAndCallWaitingMenubarWithNumberEntry,
    EPhoneConfAndHeldCallMenubar,
    EPhoneConfAndHeldCallMenubarWithNumberEntry,
    EPhoneActiveAndHeldConfMenubar,
    EPhoneActiveAndHeldConfMenubarWithNumberEntry,
    EPhoneConfCallActiveHeldAndWaitingMenubar,
    EPhoneConfCallActiveHeldAndWaitingMenubarWithNumberEntry,
    EPhoneCallActiveHeldConfAndWaitingMenubar,
    EPhoneCallActiveHeldConfAndWaitingMenubarWithNumberEntry,
    EPhoneAlertingAndConfHeldCallMenuBar,
    EPhoneAlertingWaitingAndConfHeldCallMenuBar,
    EPhoneAlertingAndConfHeldCallMenuBarWithNumberEntry,
    EPhoneAlertingWaitingAndConfHeldCallMenuBarWithNumberEntry,
    EPhoneConfCallParticipantsMenubar,
    EPhoneConfCallParticipantsDropMenubar,
    //Dialer cases
    EPhoneDialerNumberAcqMenubar,
    EPhoneDtmfDialerCBA,
    EPhoneDtmfDialerMenubar,
    EPhoneDtmfDialerNumberEntryPromptText,

    EPhonePhoneAppMenuBarResourceOffset,
    EPhonePhoneAppMenuBarResourceLast,
    EPhonePhoneAppMenuPaneResourceOffset,
    EPhonePhoneAppMenuPaneResourceLast,

    EPhoneEmptyCBA,
    EPhoneEmergencyIdleCBA,
    EPhoneNumberAcqCBA, //200
    EPhoneInCallNumberAcqCBA,
    EPhoneSettingsUiCBA,
    EPhoneCallHandlingCallSetupCBA,
    EPhoneCallHandlingCallSetupToIhfCBA,
    EPhoneCallHandlingCallSetupToHandsetCBA,
    EPhoneCallHandlingInCallCBA,
    EPhoneCallHandlingInCallNoIhfCBA,
    EPhoneCallHandlingInCallUnmuteCBA,
    EPhoneCallHandlingInCallHandsetCBA,
    EPhoneCallHandlingInCallBtaaCBA,
    EPhoneCallHandlingEmergencyCBA,
    EPhoneCallHandlingEmergencyInCallNoOptions,
    EPhoneCallHandlingEmergencyHandsetCBA,
    EPhoneCallHandlingEmergencyNoIhfCBA,
    EPhoneCallHandlingEmergencyNoOptionsCBA,
    EPhoneCallHandlingEmergencyNoOptionsHandsetCBA,
    EPhoneCallHandlingEmergencyNoOptionsNoIhfCBA,
    EPhoneCallHandlingIncomingCBA,
    EPhoneCallHandlingIncomingRejectCBA,
    EPhoneCallHandlingIncomingSoftRejectCBA,
    EPhoneCallHandlingInCallUnholdCBA,

    // KTelephonyLVFlagCoverHideSendEndKey
    EPhoneCallHandlingIncomingSliderCBA,
    EPhoneCallHandlingIncomingSilentSliderCBA,
    EPhoneCallHandlingIncomingSoftRejectSliderCBA,

    // Swivel closed
    EPhoneCallHandlingIncomingSwivelClosedCBA,
    EPhoneCallHandlingIncomingSilentSwivelClosedCBA,
    EPhoneCallHandlingWaitingAnswerSwivelClosedCBA,
    EPhoneCallHandlingWaitingReplaceSwivelClosedCBA,

    // the view itself requires a default CBA, but it will never be visible,
    // so here's one that does nothing.
    EPhoneSendDtmfNoCBA,

    // the user has not typed anything into the edit-box (without content)
    EPhoneSendDtmfEmptyEditBoxCBA,

    // the user has typed something into the edit-box (with content)
    EPhoneSendDtmfNormalEditBoxCBA,

    EPhoneCallHandlingCallWaitingCBA,
    EPhoneCallHandlingNewCallFindCBA,
    EPhoneCallHandlingNewCallCallCBA,
    EPhoneCallHandlingNewCallSwapCBA,
    EPhoneEmptyBackCBA,
    EPhoneNewCallFetchCBA,
    EPhoneEmergencyExitCBA, //250
    EPhoneEmergencyModeNoteCBA,
    EPhoneDropParticipantCBA,
    EPhonePrivateParticipantCBA,
    EPhoneJoinParticipantCBA,
    EPhoneCallHandlingInCallEndCallCBA,
    EPhonePhoneAppToolbarResourceOffset,
    EPhonePhoneAppToolbarResourceLast,

    EPhoneCLIPayphone,
    EPhoneSSNotifCLIRSupprReject,


    EPhoneWlanMacAddress,

    EPhoneColpConnected,

    EPhoneBtLoopbackEnabled,
    EPhoneBtLoopbackDisabled,
    
    EPhoneSimRejected,
    EPhoneSimUnaccepted,
    EPhoneSimRemoved,
    EPhoneRebootRequired,
    EPhoneNoteVideoCallNotPossible,
    EPhoneContinueInHandsetConfirmationQuery,
    EPhoneContinueInLoudspeakerConfirmationQuery,
    EPhoneDisconnectingQueryCBA,
    EPhoneNoteVideoCallSetupFailed,
    EPhoneNoteNoService,
    EPhoneNoteCallInfoCauseValue16,
    EPhoneNoteCallInfoCauseValue18,
    EPhoneNoteCallInfoCauseValue21,
    EPhoneNoteCallInfoCauseValue22,
    EPhoneNoteCallInfoCauseValue34,
    EPhoneNoteCallInfoCauseValue38,
    EPhoneNoteCallInfoCauseValue55,
    EPhoneNoteCallInfoCauseValue58,
    EPhoneNoteCallInfoCauseValue127,
    EPhoneNoteCallInfoNotSupported,
    EPhoneNoteCallInfoServiceNotAvailable,
    EPhoneNoteTTYNoAudioControl, //300
    EPhoneInfoClirChange,
    EPhoneInfoCugInUse,

    //the "DTMF number" list query
    EPhoneDtmfNumberListQuery,
    // contact list query
    EPhoneAddtoContactsListQuery,
    // Debug only
    EPhoneNoteTextPEFailedAtStartup,

    // Touch buttons
    EPhoneIncallButtons,
    EPhoneTwoSinglesButtons,
    EPhoneConferenceButtons,
    EPhoneParticipantListButtons,
    EPhoneConferenceAndSingleButtons,
    EPhoneConferenceAndHeldSingleButtons,
    EPhoneParticipantListCBA,
    EPhoneEmergencyCallButtons,
    EPhoneAudioBTAButton,
    EPhoneAudioDefButton,
    EPhoneIncomingCallButtons,
    EPhoneCallSetupButtons,
    EPhoneWaitingCallButtons,
    EPhoneCallSetupAndSingleButtons,
    
    // Video call error notes
    EPhoneNotePhoneOutOf3GCoverage, // cause 3,31,69
    EPhoneNoteVideoCallOnlyPossibleUnder3GCoverage, // cause 50
    EPhoneNoteUnableToMakeVideoCallNotSupportedByOtherPhone, // cause 57,88 
    EPhoneNoteCalledNumberHasBarredIncomingCalls, // cause 21
    
    EPhoneIncallHeldButtons,
    EPhoneAttemptingEmergencyText,

    // Last
    EPhoneRssBaseLast
    };

#endif      // PHONERSSBASE_H

// End of File
