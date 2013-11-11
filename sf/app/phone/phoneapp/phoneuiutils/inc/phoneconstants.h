/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Common constant values needed through PhoneUI module.

*
*/

#ifndef PHONECONSTANTS_H
#define PHONECONSTANTS_H

//  INCLUDES
#include <e32base.h>
#ifdef RD_UIKON_UNBRANCH
#include    <AknDef.h>
#include    <aknconsts.h>
#endif // RD_UIKON_UNBRANCH
#include "telinternalsingletonuids.h"


// CONSTANTS

// SECURITY POLICIES

    // Read capability: AlwaysPass.
    _LIT_SECURITY_POLICY_PASS( KPhoneReadPolicyAlwaysPass );
    
    // Read capability: ReadDeviceData.
    _LIT_SECURITY_POLICY_C1( KPhoneReadPolicy, ECapabilityReadDeviceData );
    
    // Write capability: WriteDeviceData.
    _LIT_SECURITY_POLICY_C1( KPhoneWritePolicy, ECapabilityWriteDeviceData );
    
// Application UIDs
const TUid KUidPhoneApplication = { 0x100058B3 };   // Phone application
const TUid KPhoneUidAppPhonebook = { 0x101F4CCE };  // PhoneBook
const TUid KPhoneUidAppPhoneInfo = { 0x101fc0ef };  // Phone Info
const TUid KPhoneUidAppFTD = { 0x100058F2 };        // Field Test Display
const TUid KUidStartupApplication = { 0x100058F4 }; // Startup application
const TUid KVtUiAppUidValue = { 0x101F8681 };       // Video Telephony application

// Contants below define which view will be activated. 
const TInt KTouchDiallerViewCommand = 1;
const TInt KTouchCallHandlingVievCommand = 2;

// Defination for launching PhoneInfo application
const TUid KPhoneUidViewPhoneInfo = { 1 };  

// Defination for launching Log application
const TUid KPhoneUidViewLogLastDial = { 1 };        // Last dialled view ID of Log application
const TUid KPhoneUidLogStm = { 0x01002982 };        // Id for stm for Log application
_LIT8( KPhoneStmLoadMagicString, "outside" );       // Message for activating view of an application

// Used in CPhoneKeyEventHandler
// Seconds in microseconds
const TInt KPhoneSecondInMicroseconds = 1000000;
// For long press of end key while external
const TInt KPhoneLongPressKeyEventDuration = 
    ( KPhoneSecondInMicroseconds * 6 ) / 10; // 0.6 sec

// Used in CPhoneUIController
//
// Delay between Phone Engine start tries
const TInt KPeRetryDelay = 500000;
// PE start retry times
const TInt KPeRetryCount = 10;

// ServiceId value: service not determined
const TInt  KUnknownService = 0;

// Used in CPhoneViewController
// Voice Mail Box key "1"
const TInt KPhoneVoiceMailBoxLocation = 1;  
// Video Mail Box key "2"
const TInt KPhoneVideoMbxLocation = 2;
// Path to soft reject resource
_LIT( KPhoneSoftRejectResource, "z:softreject.rsc" );

// Used in CPhoneMenuController
// valid phone number length
const TInt KPhoneValidPhoneNumberLength = 3;

// Used in CPhoneState
// Maximum length of the Call header label
const TInt KPhoneCallHeaderLabelMaxLength = 50;
// Line feed
_LIT( KPhoneLineFeed, "\n");
// Length of the text to display call duration - 
// at least double the length of (HH:MM:SS)
const TInt KPhoneMaxTimeDisplayTextLength = 16;
// Length of the profile display date string
const TInt KPhoneMaxDateDisplayTextLength = 100;

// Used in CPhoneStateIncoming
// Array granularity for soft reject array.
const TInt KPhoneSoftRejectArrayGranularity = 2;
// Length of SMS.
const TInt KPhoneSMSLength = 160;

// Used in CPhoneStateInCall
// Default CBA resource ID for single item fetch dialog
const TInt KPhoneDefaultFetchCBA = 0; 

// Used in CPhoneBubbleWrapper
const TInt KPhoneNumberEntryBufferSize = 100;
const TInt KMaxNumberOfCallerBubbles = 20;
const TInt KNumberOfActiveCalls = 2;

// Constant to indicate 'no resource'
const TInt KPhoneBubbleNoResource = KErrNotFound;
const TInt KPhoneContactNameMaxLength = 101;    // 50+50 first & last names + 1 for space between
// Conference case
const TInt KPhoneConferenceInitialSize = 2;
const TInt KConferenceMaxParticipants = 5;

// Used in CPhoneStatusPane
const TInt KPhoneTitlePaneTextSize = 128;
const TInt KPhoneTitlePaneForOperatorName = 1;
const TInt KPhoneTitlePaneForPbkSigleFetch = 2;
const TInt KPhoneTitlePaneListGranularity = 4;
const TInt KPhoneNaviPaneListGranularity = 4;

// Resource enumeration uid's
const TUid KUidRssPhoneBase                       = { 0x01 };
const TUid KUidRssPhoneGSM                        = { 0x02 };

// For CPhoneView use
const TUid KUidViewId = { 0x10282D81 };
const TUid KUidAppId = { 0x100058B3 };
 
// Amount of regions to be reserved.
const TInt KPhoneRegionAmount = 16;

const TInt KDefaultCallHeaderLeft = 12;
const TInt KDefaultCallHeaderTop = 6;
const TInt KDefaultCallHeaderRight = 19;
const TInt KDefaultCallHeaderBottom = 12;
const TInt KCallHeaderLeft = 156;
const TInt KCallHeaderTop = 6;
const TInt KCallHeaderRight = 164;
const TInt KCallHeaderBottom = 12;

const TInt KClockWindowActive = 3;
const TInt KClockWindowIdle = 2;


// Used in CPhoneKeys
// The character to be used as the 'wait' character, by DTMF code.
const TUint KPhoneDtmfWaitCharacter  = 'w';

// The character to be used as the 'soft pause' character, by DTMF code.
const TUint KPhoneDtmfPauseCharacter = 'p';

// Following, are the standard DTMF characters...
const TInt KPhoneDtmfHashCharacter    = '#';
const TInt KPhoneDtmfStarCharacter    = '*';
const TInt KPhoneDtmf0Character       = '0';
const TInt KPhoneDtmf1Character       = '1';
const TInt KPhoneDtmf2Character       = '2';
const TInt KPhoneDtmf3Character       = '3';
const TInt KPhoneDtmf4Character       = '4';
const TInt KPhoneDtmf5Character       = '5';
const TInt KPhoneDtmf6Character       = '6';
const TInt KPhoneDtmf7Character       = '7';
const TInt KPhoneDtmf8Character       = '8';
const TInt KPhoneDtmf9Character       = '9';

// Check second #
const TInt KSecondKeyEvent = 2;

// Used in CPhoneStringContainer
const TInt KPhoneStringContainerReserved = 50;
const TInt KPhoneStringContainerGranularity = 10;

// Used in test shutdown
_LIT( KExit, "*#1234#" );
/// ROM drive.
_LIT( KDriveZ, "z:" );

_LIT( KPhoneResourceFile, "phoneui.rsc" );
_LIT( KPhoneResourceFileVoIP, "phoneuivoip.rsc" );
_LIT( KPhoneResourceFileCHUI, "callhandlingui.rsc" );
_LIT( KPhoneResourceFilePHTC, "phoneuitouch.rsc" );

// Used in CPhoneAppUI
// Priority of application window groups.
const TInt KPhoneApplicationWindowGroupPriority = 0;

// Ordinal position of phone application when sending to background.
// NOTE: 0 - foreground application
//       1 - server side status pane
//       2 - phone application
const TInt KPhoneWindowGroupPositionBackground = 2;

// Here is priority for capturing keys, used in cpaappuibase.cpp :
const TInt KPhoneCapturePriority = 10;

// Name of the EIKON server window group.
_LIT( KPhoneEikonServer, "EikonServer" );

// For CPhoneLogger use
const TInt KMaxLogLineLength = 256;
const TInt KMaxAppendLength = 64;

// From CPhoneNoteUtilities
const TInt KPhoneNoteSoftkeysUseDefault = 0;
const TInt KPhoneNoteGraphicNone = -1;
const TInt KPhoneNoteNoTimeout = -1;

// State machine ordinal number
static const TInt KPhoneUiStateMachineOrdinal = 1;

static const TUid KUidAppGSMStates = { 0x101F7CAC };
static const TUid KUidAppVoIPExtensionStates = { 0x101F7CA9 };

// Log folder and log file name
_LIT( KPhoneLogFolder, "PhoneUI");
_LIT( KPhoneLogFileName, "PhoneUI.log" );

// Paths to CentRep
_LIT( KGSMProtocol, "phoneuistates.dll" );
_LIT( KVoIPExtension, "phoneuivoipextension.dll" );

// Literals for formatting log messages
_LIT( KPEDefaultMsgToPhoneUI,   "PE ----> PHONEUI <%S> message from PE. CallId: %d.");
_LIT( KPEDefaultMsgToPE,        "PE <---- PHONEUI        <%S> to PE.");
_LIT( KPEDefaultMsgToPhoneUIEnd,"PE ----> PHONEUI_COMPLETED    <%S> message from PE  executed succesfully");
_LIT( KPEDefaultMsgToPEEnd,     "PE <---- PHONEUI_COMPLETED    <%S> message to PE  sended succesfully");
_LIT( KPEDefaultStateChange,    "PHONEUI_STATE_CHANGE     <%S> -> <%S> ");
_LIT( KPEDefaultMsgToView,      "PHONEUI ----> VIEW <%S> message from Control. CallId: %d.");

// Component literals for CPhoneMethodLogger
_LIT( KPhoneUI, "PhoneUI: " );
_LIT( KPhoneUIControl, "PhoneUIControl: " );
_LIT( KPhoneUIView, "PhoneUIView: " );
_LIT( KPhoneUIStates, "PhoneUIStates: " );
_LIT( KPhoneUIUtils, "PhoneUIUtils: " );
_LIT( KPhoneMediatorCenter, "PhoneMediatorCenter: " );
_LIT( KLessThan, "<" );
_LIT( KGreaterThan, ">" );

// Used in PhoneStateDefinitions*
const TInt KPhoneStateCommonFirst = 0;
const TInt KPhoneStateProtocolFirst = 100;

// Used in PhoneViewCommandDefinitions*
const TInt KPhoneViewCommandCommonFirst = 1; // can't be 0 due to mediator
const TInt KPhoneViewCommandCarrierFirst = 1000;
const TInt KPhoneViewResponseCommonFirst = 0;
const TInt KPhoneViewResponseCarrierFirst = 1000;

// Used in PhoneRss*
const TInt KPhoneRssCommonFirst = 0;
const TInt KPhoneRssProtocolFirst = 1000;

// Used in CPhoneNoteController
const TInt KPhoneMaxCharsInNote = 256;

// Used in CPhoneCenRepProxy
// Variant read only default value
const TInt KPhoneVariantReadOnlyDefaultValue = 0x00000000;
const TInt KPhoneCenRepObserversGranularity = 16;

// Used in CPhonePubSubProxy
const TInt KPhonePubSubProxyObserverArrayGranularity = 16;
const TInt KPhonePubSubArrayGranularity = 16;

// Used in CPhoneInCallIndicator
const TInt KPhoneIncallIndicatorDefaultValue = 0;
const TInt KPhoneIncallIndicatorArrayGranularity = 8;

// Used in CPhoneApplicationExit
// Default buffer size. Do not rely on this. Use MaxLength instead.
const TInt KPhoneTemporaryBufferMaxLength = 1024;
// UID range separator.
const TInt KPhoneApplicationRangeSeparator = '-';
// Drive identifier.
const TInt KPhoneApplicationDriveIdentifier = ':';
// Retry five times.
const TInt KPhoneApplicationRetryCount = 5;
// Interval between retry attempts.
const TInt KPhoneApplicationRetryInterval = 100000;
// Any application 
_LIT( KPhoneApplicationAnyApp, "*" );
// OOM idenfier.
_LIT( KPhoneApplicationOom, "OOM" );

// Emergency call id
const TInt KEmergencyCallId = 7;

// Conference call id
const TInt KConferenceCallId = 8;

//AudioVolume const
// CONSTANTS
const TInt KPhoneDefaultVolume = 4;

const TInt KPhoneVolumeMinValue = 0;
const TInt KPhoneVolumeMaxValue = 10;

const TInt KPhoneVolumeWinsVolumeDivider = 10;

// Used in CPhoneRemoteControlHandler
#ifdef RD_UIKON_UNBRANCH
// Volume repeat delay.
const TInt KPhoneButtonRepeatDelay = KAknStandardKeyboardRepeatRate;
#else // RD_UIKON_UNBRANCH
// Volume repeat delay.
const TInt KPhoneButtonRepeatDelay = (1000000/6);
#endif // RD_UIKON_UNBRANCH

// First voice call id
const TInt KVoiceCallIdFirst = 0;

// Video telephony call ids
const TInt KVideoTelephonyCallId1 = 9;
const TInt KVideoTelephonyCallId2 = 10;
  
// Used in MPhoneAudioPlayerObserver
const TInt KPhoneAudioPlayerNotSpecified = -1;

// Used in CPhoneAudioPlayer
// One second pause between ringing repeats.
const TInt KPhoneRingingRepeatsTrailPause = 1000000;
// For ascending and descending, each level is played for 3 seconds
const TInt KPhoneAudioAscendingRampDuration = 3000000;
// For repeating the audio tone forever
const TInt KPhoneMdaAudioToneRepeatForever = -2;
// Value how much to shift variable
const TInt KTimesToMultiply = 2; 

// Used in CPhoneRingingTone
_LIT( KPhoneRingingToneDriveZ, "z:" );
#ifdef RD_VIDEO_AS_RINGING_TONE
    _LIT( KPhoneRingingToneVideoMime, "video/*" );
    _LIT( KPhoneRingingToneRealVideoMime, "*realmedia" );
#endif    

// Used in CPhoneRingingTonePlayerAO
// Tts volumes
const TInt KTtsVolumeBoost = 9;
const TInt KTtsVolumeMin = 2;
const TInt KTtsVolumeAscendingRepeat = 4;
const TInt KPlayerVolumeAscendingRepeat = 3;
const TInt KTtsVolumeAscendingDecrease = 2;

// Header that is added to the text-to-say, that makes this text recognizable 
// as Text-To-Speech data
_LIT( KPhoneTtsHeader, "(tts)");
// If the ringing tone for line1, line2 or personal ringing tone is not played
// in 3 seconds, then try next. 
const TInt KPhoneMaxRingingWaiting = 3000000;
const TInt KPhoneTtsAscendingStep = 3000000; 
// Delays between periods of Text-To-Speech playing
// e.g. { 3 000 000, 4 000 000} means the following order of playiong:
// 3 secs tone, then TTS, then 4 secs of tone, then TTS, then tone forever.
const TInt KPhoneTtsDelays[] = { 3000000, 4000000 }; //2000000, 3000000
// Amount of elements in KPhoneTtsDelays array.
const TInt KPhoneTtsDelaysCount = sizeof( KPhoneTtsDelays ) / sizeof ( TInt );
// Audio preference for Text to Speech
const TUint KAudioPrefTextToSpeechCallHardCoded = 0x01370001;
// Sound files
#ifdef __WINS__
    _LIT( KPhoneDefaultSoundFile, "z:test.wav" );
    _LIT( KPhoneSilentSoundFile, "z:test.wav" );
    _LIT( KPhoneBeepOnceSoundFile, "z:test.wav" );
#else
    _LIT( KPhoneDefaultSoundFile, "\\System\\Sounds\\Simple\\Nokia tune.wav" );
    // Sequence for silent call ringing tone. Don't touch these sequence.
    _LIT8( KPhoneNoSoundSequence, "\x00\x11\x0A\x05\xFC\x0A\x08\x40\x32\x0A\
\xF7\x40\x64\x06\x0B" );
    // Sequence for beep call ringing tone. Don't touch these sequence.
    _LIT8( KPhoneBeepSequence, "\x00\x11\x0A\x0A\x08\x73\x0A\x40\x28\x0A\xF7\
\x05\xFC\x40\x64\x0A\x08\x40\x32\x0A\xF7\x06\x0B" );
    // Sequence for unsecure VoIP call warning tone. Don´t touch these sequence.
    _LIT8( KPhoneUnsecureVoIPCall, "\x00\x11\x0B\x67\x04\x40\x04\x67\x04\x40\
\x04\x6C\x04\x40\x04\x6D\x04\x0B" );
#endif // __WINS__

// Used in CPhoneVideoPlayer
const TInt KPhoneVideoMinVolumeLevel = 0;
const TInt KPhoneVideoMaxVolumeLevel = 10;
const TInt KPhoneVideoVolumeRampInterval = 3000000;
const TInt KPhoneVideoVolumeRampStep = 1;
const TInt KPhoneVideoSubQCCIFWidth = 128;
const TInt KPhoneVideoSubQCCIFHeight = 96;

const TInt KBaseScaleFactor = 100;
const TInt KHalfBaseScale = 50;
const TInt KFullScaleFactor = 1.0;
const TInt KHalfScaleFactor = 0.5;
const TInt KCentering = 2;


// Used in mediator for receiving and sending events and commands
const TUid KPhoneUiCategory = { 0x100058B3 };

// Our mediator interface versions
const TInt KCommandIfVersionMajor = 1;
const TInt KCommandIfVersionMinor = 0;
const TInt KCommandIfVersionBuild = 0;
const TInt KEventIfVersionMajor = 1;
const TInt KEventIfVersionMinor = 0;
const TInt KEventIfVersionBuild = 0;

// Mediator interface command's time out
const TInt KPhoneUiMediatorIfTimeout = 10000;

// Name of Call UI dll which is loaded polymorficly in CPhoneQueryController
_LIT( KPhoneCaUiLoadedLib, "cauiengine.dll" );

// Used in CPhoneNoteTimer
// Time in milliseconds that the timer runs.
const TInt KDtmfWaitNoteTimeoutValue = 100000;

// Als line change timer 2,4 seconds.
// Together with long keypress (0.6) seconds
// it is 3 seconds like in the specification.
const TInt KAlsLineChangeTimerValue = 2400000;

// Wlan MAC address length, separator and formatter
const TInt  KPhoneWlanMacAddressLength = 50;
const TUint KPhoneWlanSeparator (':');
_LIT( KWLanMACDataFormat, "%02x");

// BTA Disconnect handler timeout
// 1 minute in microseconds.
const TInt KPhoneBtaaConnectionLostQueryTimeout = 60000000; 

// Time cap between information note and reconnect query
const TInt KInformationNoteDisplayTime = 3000000;

// Maximum length of file path for caller image theme
const TInt KMaxFilePathLength = 256;

// Used in Ringingtone handling
const TInt Kkilo = 1024;

// Allow name in call header in setup state after 1.5s
const TInt KTimeoutAllowName = 1500000;

// It enumerates different screen saver states.
enum TPhoneScreensaverState
    {
    EPhoneScreensaverNotAllowed = 1,
    EPhoneScreensaverAllowed = 0
    };

// VoIP variation
const TInt KDynamicVoIPOff = 0;
const TInt KDynamicVoIPOn = 1;

// Max param lenght.
const TInt KPhoneMaxParamLength = 10;

// Video call setup failed query timeout
const TInt KPhoneVideoCallSetupFailedQueryTimeout = 5; 

// Key event value for long press
const TInt EEventLongPress = 1100;

#endif  // PHONECONSTANTS_H

// End of File
