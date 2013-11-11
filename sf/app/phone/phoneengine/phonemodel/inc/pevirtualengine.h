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
* Description:  This file defines the protocol independent interface of the Phone Engine.
*
*/

#ifndef PEVIRTUALENGINE_H
#define PEVIRTUALENGINE_H

// INCLUDES
#include <badesca.h>
#include <bldvariant.hrh>   //protocol definition, component configuration definitions
#include <cphcntcontactid.h>
#include <ctsydomainpskeys.h>
#include <etelmm.h>
#include <exterror.h>
#include <fbs.h>
#include <mccecall.h>
#include <mmtsy_names.h>
#include <networkhandlingdomainpskeys.h>
#include <phclttypes.h>
#include <Profile.hrh>
#include <rmmcustomapi.h>
#include <rsssettings.h>
#include <sysutil.h>


// CONSTANTS

//Call origin types
enum TPECallOrigin
    {
    EPECallOriginPhone,
    EPECallOriginClient,
    EPECallOriginSAT
    };

//Output types
enum TPEAudioOutput
    {
    EPENotActive,
    EPEHandset,
    EPELoudspeaker,
    EPEWiredAudioAccessory,
    EPEBTAudioAccessory,
    EPETTY
    };

//Error types
enum TPEErrorType
    {
    EPECcp = 0,
    EPECch
    };

// PhoneEngine Errors
const TInt KErrPhoneEngineErrorBase = -30000;
const TInt KErrPhoneEngineNoWcdmaNetwork = KErrPhoneEngineErrorBase - 1;
const TInt KErrPhoneEngineVideoCallNotAllowedDuringRestore = KErrPhoneEngineErrorBase - 2;
const TInt KErrPhoneEngineVideoCallSetupFailed = KErrPhoneEngineErrorBase - 3;

// Phone Engine dll uid
const TUid KUidPhoneEngine = { 0x1000A86C };

// Maximum length of phone number 
const TInt KPEPhoneNumberMaxLength = 100;
//Phone number type defined using the custom maximum length
typedef TBuf<KPEPhoneNumberMaxLength> TPEPhoneNumber;
//Maximum length of dtmf string        
const TInt KPEDtmfMaxLength = KPEPhoneNumberMaxLength;

//Maximum number of the SS service groups
const TInt KPEMaxServiceGroup = 53; // Maximum value taken from etelmm.h(TMobileService)    

const TInt KPEAutomaticAnswerTimeout = 5000000; // 5 seconds;

// Enumerations for variant messages
// For enumeration TPEMessagesToPhoneEngine
const TInt KPEMessagesToPhoneEngineGsmFirst = 11000;
const TInt KPEMessagesToPhoneEngineGsmLast = 12999;
const TInt KPEMessagesToPhoneEngineVariantFirst = KPEMessagesToPhoneEngineGsmFirst;

// For enumeration TPEMessagesFromPhoneEngine
const TInt KPEMessagesFromPhoneEngineGsmFirst = 11000;
const TInt KPEMessagesFromPhoneEngineGsmLast = 12999;
const TInt KPEMessagesFromPhoneEngineVariantFirst = KPEMessagesFromPhoneEngineGsmFirst;

// Used when initializing number variables with non negative value range.
const TInt KPENumberInitValue = -1; 
//CallId not used in SendErrorMessageL
const TInt KPECallIdNotUsed = -1;            
//ErrorCode not used in SendErrorMessageL
const TInt KPEErrorCodeNotUsed = -1;        
// Informs to phone engine that dont send message to the phone application
const TInt KPEDontSendMessage = 1;
 
// Maximum lenght of timer
const TInt KPESsTimerMaxLength = 2;

// Audio player repeats tone forever
const TInt KPEAudioPlayerRepeatForever = -2;
// Default Audio volume value
const TInt KPEDefaultAudioVolume = 7;
// Default volume value for accessory
const TInt KPEDefaultAccessoryVolume = 5;
// Default keypad volume
const TProfileKeypadVolume KPEDefaultKeypadVolume = EProfileKeypadVolumeLevel2;

// Dtmf stop character, PE handles internally DTMF characters as lowercase.
const TUint KPEDtmfStopCharLowercase = 'w';
const TUint KPEDtmfStopCharUppercase = 'W';
const TUint KPEDtmfPauseCharLowercase = 'p';
const TUint KPEDtmfPauseCharUppercase = 'P';
const TUint KPEDtmfPlusChar = '+';

enum TPELoggingIndicator
    {
    EPENormalLogging = 0,
    EPEContinuousLogging 
    };

// Maximum Length of profile name
const TInt KPEProfileNameLength = 64;
// Profile is alerting all incoming calls.
_LIT( KPEAlertForAllCalls, "1");
// Informs how many profiles there are
const TInt KPEProfileGroups = 3;
// Informs how much time it take's to change ringing tone volume from minimum to maximum(Ascending)
const TInt KPERingingToneRampDuration = 5000000;
// Informs how many ringing tone volume levels there are in the profile db
const TInt KPEProfileRingingToneLevels = 10;
// Includes default tone volume
const TInt KPEDefaultToneVolume = 30000;
// Contact group id string length
const TInt KPEContactGroupString = 100;

//The length of a pause created during dialling by the comma character (',') measured in seconds.  
const TUint KPEDefaultInterval = 2;

// CommDB constants for Data calls
// Internet Access Point service type
_LIT( KPEIAPServiceTypeName, "IAP\\IAPServiceType" );
// Dial Out Internet Serivce Provider Table name
_LIT( KPEIAPDialOutISPTableName, "DialOutISP" );
// Dial In Internet Service Provider Table name
_LIT( KPEIAPDialInISPTableName, "DialInISP" );
// Access Point Name column on IAP table
_LIT( KPEIAPAccessPointName, "IAP\\NAME" );

const TInt KPEMaxAudioOutputs = EPETTY + 1; // enum count

// DATA TYPES

// Contact name definition
typedef TBuf<KCntMaxTextFieldLength> TPEContactName;
// Contact company definition
typedef TBuf<KCntMaxTextFieldLength> TPEContactCompany;
// Contact group definition
typedef TBuf<KCntMaxTextFieldLength> TPEContactGroup;
// Contact filename definition
typedef TBuf<KMaxFileName>         TPEContactFileName;
// Contact group ids sting
typedef TBuf<KPEContactGroupString> TPEContactGroupId;
//Dtmf-string definition
typedef TBuf<KPEDtmfMaxLength> TPEDtmfString;
//Profile name definition
typedef TBuf<KPEProfileNameLength> TPEProfileName;
//Call control caps
typedef TUint32 TPECallControlCaps;
//User to user info
typedef TBuf<KPhCltUUILength> TPEUserToUserInfo;

// Phone identity parameters type.
typedef TBuf<RMobilePhone::KPhoneManufacturerIdSize> TPEManufacturer;
typedef TBuf<RMobilePhone::KPhoneModelIdSize> TPEModel;
typedef TBuf<KSysUtilVersionTextLength> TPERevision;
typedef TBuf<RMobilePhone::KPhoneSerialNumberSize> TPESerialNumber;

//Call States
typedef TUint TPEState;

enum
    {
    // Call states                        0 - 499
    EPEStateUnknown                     = 0,
    EPEStateIdle,
    EPEStateDialing,
    EPEStateEmergencyDialing,
    EPEStateRinging,
    EPEStateConnecting,
    EPEStateConnected,
    EPEStateHangingUp,
    EPEStateHeld,
    EPEStateAnswering,
    EPEStateRejecting,
    EPEStateDisconnecting,
    // conference call states
    EPEStateConferenceIdle,
    EPEStateConnectedConference,
    EPEStateHeldConference,
    EPEStateCreatingConference,
    EPEStateHangingUpConference,
    EPEStateSwappingConference,
    EPEStateResumeHoldConference,        
    EPEStateGoingOneToOne,
    EPEStateAddingConferenceMember,
    EPEStateAddedConferenceMember,
    EPEStateDroppingConferenceMember,
    EPEStateDroppedConferenceMember,
    // maximum value for boundary checks
    EPEStateMaximumState                = 1000
    };

NONSHARABLE_STRUCT( TPEErrorInfo )
    {
    //Symbian OS error code
    TInt iErrorCode;
    //Call Id number, equals KPECallIdNotUsed if call id is not available or is
    //invalid for this error condition
    TInt iCallId;
    // Error type( Cch or Ccp )
    TPEErrorType iErrorType;
    };
    
enum TPECallType
    { // maintain compatibility with P&S type enum
    // Used to initialize call type information
    EPECallTypeUninitialized    = EPSCTsyCallTypeUninitialized,
    // Circuit switched voice call
    EPECallTypeCSVoice          = EPSCTsyCallTypeCSVoice,
    // Circuit switched multimedia call
    EPECallTypeVideo            = EPSCTsyCallTypeH324Multimedia,
    // Packet switched voice call
    EPECallTypeVoIP             = EPSCTsyCallTypeVoIP
    };

enum TPEPinCode 
    {
    EPEPinCode,
    EPEPin2Code
    };

enum TPEPhoneCommandType
    {
    EPESendingFlash
    };

// Enumeration which defines the search keys with which
// contact information is searched
enum TPEFindContactKey
    {
    //Search keys
    EPEFindWithPhoneNumber = 0,
    EPEFindWithContactId,
    EPEFindKeyNotDefined
    };

enum TPESecurityCommand 
    {
    EPESecurityCommandUnknown,
    EPESecurityCommandChangePinCode,
    EPESecurityCommandUnblockPinCode,
    EPESecurityCommandChangeBarringPassword,
    EPESecurityCommandChangeSimLock
    };

enum TPEPhoneNumberIdType
    {
    EPEUnknownNumber,
    EPEMobileNumber,
    EPEPager,
    EPETelephoneNumber,
    EPEVmbxNumberLine1,
    EPEVmbxNumberLine2,
    EPEPrivateNumber,
    EPEEmergencyNumber,
    EPEFaxNumber,
    EPEVoipNumber,
    EPEVideoNumber,
    EPEAssistantNumber,
    EPECarNumber,
    EPEHomeNumber,
    EPEWorkNumber
    };

enum TPERxTimeSlots
    {
    EPERxSlots1 = 1,
    EPERxSlots2 = 2,
    EPERxSlots3 = 3
    };

//VoiceMailBoxNumbers
NONSHARABLE_STRUCT( TPEVoiceMailBoxNumberInfo )
    {
    TPEPhoneNumber iVMBXL1; // Line 1 VoiceMailBoxNumber
    TPEPhoneNumber iVMBXL2; // Line 2 VoiceMailBoxNumber
    TPEPhoneNumber iDTMFVMBXL1; // Line 1 DTMFVoiceMailBoxNumber
    TPEPhoneNumber iDTMFVMBXL2; // Line 2 DTMFVoiceMailBoxNumber
    };

// Contains information of call audio route parameters
NONSHARABLE_STRUCT( TPEPhoneAudioRouteParameters )
    {
    TPEAudioOutput iAudioOutput;
    TPEAudioOutput iPreviousOutput;
    TBool iShowNote;
    };

// Enumerate definitions for MMI Service Codes from MMETel API:

// FUNCTION PROTOTYPES
// None.

// FORWARD DECLARATIONS

class MPEEngineInfo;

// CLASS DECLARATION

/**
*  Offers message interface from phone engine to phone application.
*  
*  @lib phonenegine.lib
*  @since S60 v.4.0
*/
NONSHARABLE_CLASS( MEngineMonitor )
    {

    public:

        enum TPEMessagesFromPhoneEngine
        {
        // Audio messages                     1000 - 1999
        EPEMessageAudioDtmfEvent            = 1000,
        EPEMessageAudioDTMFPlayComplete,
        EPEMessageAudioMuteChanged,
        EPEMessageAudioOutputChanged,
        EPEMessageAudioOutputPreferenceChanged,
        EPEMessageAudioPlayStopped,
        EPEMessageAudioVolumeChanged,
        EPEMessageAvailableAudioOutputsChanged,

        // Call messages                      2000 - 2999
        EPEMessageAnswer                    = 2000,
        EPEMessageAnswering,
        EPEMessageCallAdded,
        EPEMessageCallControlCapsChanged,
        EPEMessageCallWaiting,
        EPEMessageCallWaitingAnswered,
        EPEMessageChangedCallDuration,
        EPEMessageConferenceCapsChange,        
        EPEMessageConferenceIdle,
        EPEMessageConnected,
        EPEMessageConnectedConference,                
        EPEMessageConnecting,
        EPEMessageContinuingDTMFSending,
        EPEMessageDialing,
        EPEMessageDialingError,
        EPEMessageDisconnecting,
        EPEMessageDisconnectingConference,        
        EPEMessageDisconnectingWithInband,  //UI doesn't get this message, it is mapped as EPEMessageDisconnecting
        EPEMessageDTMFSendingAborted,
        EPEMessageHeld,
        EPEMessageHeldConference,        
        EPEMessageIdle,
        EPEMessageIncoming,
        EPEMessageInitiatedEmergencyCall,
        EPEMessageInitiatedEmergencyWhileActiveVideo,
        EPEMessageIssuingUSSDRequest,
        EPEMessageIssuedUSSDRequest,
        EPEMessagePacketNetworkDetached,   
        EPEMessageRemoteBusy,
        EPEMessageRemoteTerminated,               
        EPEMessageSendingDTMF,
        EPEMessageSentDTMF,
        EPEMessageSentSSString,
        EPEMessageStoppedDTMF,
        EPEMessageSwapped,
        EPEMessageTransferDone,
        EPEMessageTransferring,
        EPEMessageUnknown,
        EPEMessageWentOneToOne,        
        EPEMessageRemotePartyInfoChanged,
        EPEMessageConnectingWithInband, //UI doesn't get this message, it is mapped as EPEMessageConnecting

        EPEMessageUnattendedTransferRequest,

        EPEMessageRemoteForwarding,
        EPEMessageMovedPermanently,        
        EPEMessageMultipleChoices,        
        EPEMessageColpNumberAvailable,

        // Contact messages                   3000 - 3999
        EPEMessageThumbnailLoadingCompleted  = 3000,

        // Error messages                     4000 - 4999
        EPEMessageError                     = 4000,
        EPEMessageAudioHandlingError,
        EPEMessageCallHandlingError,
        EPEMessageConferenceErrorAddCall,
        EPEMessageConferenceErrorRemoveCall,
        EPEMessageConferenceErrorSwap,
        EPEMessageConferenceErrorHold,
        EPEMessageConferenceErrorResume,
        EPEMessageConferenceErrorGoOneToOne,
        EPEMessageConferenceErrorCurrentCallsToConference,
        EPEMessageConferenceErrorHangUp,
        EPEMessageContactHandlingError,
        EPEMessageLogHandlingError,
        EPEMessageServiceHandlingError,

        // Network messages                   5000 - 5999
        EPEMessageCallSecureStatusChanged= 5000,
        EPEMessageNetworkRegistrationStatusChange,

        // Parser messages                    6000 - 6999
        EPEMessageActivateRfsDeep           = 6000,
        EPEMessageActivateRfsNormal,
        EPEMessageShowBTDeviceAddress,
        EPEMessageShowBTLoopback,
        EPEMessageShowWlanMacAddress,
        EPEMessageSSRequestFailed,

        // Security messages                  7000 - 7999
        EPEMessageBadBarringPasswordChange  = 7000,
        EPEMessageBadPin1Change,
        EPEMessageBadPin1Unblock,
        EPEMessageBadPin2Change,
        EPEMessageBadPin2Unblock,
        EPEMessageBarringPasswordChanged,
        EPEMessageChangedSimLock,
        EPEMessageFailedChangeSimLock,
        EPEMessageFailedNewPin1AndPin2NotEqual,         // Error message, NewPin1 and NewPin2 are not equal
        EPEMessageFailedSecurityCommandAlreadyActive,   // Error message, SecurityCommand is already active
        EPEMessagePin1Changed,
        EPEMessagePin1Unblocked,
        EPEMessagePin2Changed,
        EPEMessagePin2Unblocked,
        EPEMessageSIMChanged,
        EPEMessageSIMStateChanged,

        // Settings and Shared Data messages  8000 - 8999
        EPEMessageProfileChanged            = 8000,

        // Startup messages                   9000 - 9999
        EPEMessagePEConstructionReady       = 9000,     // Phone Engine is initialized and fully operational
        EPEMessagePEConstructionFailed,
        EPEMessagePEConstructionTwoPhaseReady,
        // Miscellaneous messages             10000 - 10999
        EPEMessageDoNotSendMe               = 10000,
        EPEMessageInValidEmergencyNumber,
        EPEMessageLogEventSaved, 
        EPEMessageStartATDialing,
        EPEMessageValidEmergencyNumber,
        EPEMessageMuteRingingTone,

        // Audio messages                         11000 - 11199

        // Call messages                          11200 - 11399
        EPEMessageActiveBarring                 = KPEMessagesFromPhoneEngineGsmFirst + 200,
        EPEMessageActiveCallForwarding,
        EPEMessageAddedConferenceMember,
        EPEMessageALSLineChanged,
        EPEMessageCallBarred,
        EPEMessageIncCallIsForw,
        EPEMessageIncCallForwToC,
        EPEMessageOutCallForwToC,
        EPEMessageForwardUnconditionalModeActive,
        EPEMessageForwardConditionallyModeActive,
        EPEMessageDroppedConferenceMember,
        EPEMessageGoingOneToOne,
        EPEMessageRemoteCreatedConference,
        EPEMessageRemoteHeld,
        EPEMessageRemoteResumed,
        EPEMessageTempClirSuppressUnsuccessful,
        EPEMessageTempClirActivationUnsuccessful,
        EPEMessageShowIncCallGroupIndex,
        EPEMessageDataPortLoaned,
        EPEMessageOutgoingCallBarred,
        // Contact messages                       11400 - 11599

        // Error messages                         11600 - 11799

        // VoIP service messages                       11800 - 12999
        EPEMessageServiceEnabled               = KPEMessagesFromPhoneEngineGsmFirst + 800,
        EPEMessageServiceEnabling,
        EPEMessageNoService,
        EPEMessageServiceDisabled,

        // Parser messages                        12000 - 12199
        EPEMessageIssuedSSRequest               = KPEMessagesFromPhoneEngineGsmFirst + 1000,
        EPEMessageIssuingSSRequest,

        // Security messages                      12200 - 12399
        EPEMessageSecureNotSpecified,

        // Settings and Shared Data messages      12400 - 12599
        EPEMessageAlsStatusChanged              = KPEMessagesFromPhoneEngineGsmFirst + 1400,
        EPEMessageBarringNotification,
        EPEMessageClipStatusNotification,
        EPEMessageClirStatusNotification,
        EPEMessageCnapStatusNotification,
        EPEMessageColpStatusNotification,
        EPEMessageColrStatusNotification,
        EPEMessageForwardingNotification,
        EPEMessageVmbxNumberLine1Changed,
        EPEMessageVmbxNumberLine2Changed,
        EPEMessageWaitingNotification,


        // Miscellaneous messages                 12600 - 12799

        // Last                                   12999
        // Exposing this allows the compiler to detect the overlapping enumerations.
        EPEMessageLastVariantItem               = KPEMessagesFromPhoneEngineGsmLast
        };
        
        /**
        * HandleError
        * Phone Application implements this callback method to provide a framework
        * for proceeding in an error condition.
        * @param aErrorInfo is the call id number.
        */
        virtual void HandleError( const TPEErrorInfo& aErrorInfo ) = 0;

        /**
        * HandleMessage
        * Phone Application implements this callback method to provide a response
        * for an event message sent by the Phone Engine.
        * @param aMessage specifies the event that has taken place
        * @param aCallId is the call id number.
        */
        virtual void HandleMessage( const TInt aMessage, const TInt aCallId ) = 0;
        
    }; // MEngineMonitor


// Maximum number of the conference members.
const TInt KPEMaximumNumberOfConferenceMembers = 5;
// Maximum number of voice calls
const TInt KPEMaximumNumberOfVoiceCalls = 8; // Phone engine user can change this value if needed.
// Maximum number of data calls
const TInt KPEMaximumNumberOfDataCalls = 2; 
// Maximum number of conference calls
const TInt KPEMaximumNumberOfConferenceCalls = 1;    
// Maximum number of all calls
const TInt KPEMaximumNumberOfCalls = KPEMaximumNumberOfVoiceCalls + 
    KPEMaximumNumberOfDataCalls + KPEMaximumNumberOfConferenceCalls;

// Emergency CallId
const TInt KPEEmergencyCallId = KPEMaximumNumberOfVoiceCalls - 1;
// Conference Call ID
const TInt KPEConferenceCallID = KPEMaximumNumberOfVoiceCalls;

// Invalid call index, note not the same as call id
const TInt KPEInvalidCallIndex = -1;
// These two are needed in TPECallInfo's conference field
// Indicates, that this is Conference Master
const TInt KPEConferenceMaster = -1;
// Indicates, that this is Normal voice call
const TInt KPENormalVoiceCall = -2;

// All MMI Service Codes are defined in standard 3GPP TS 02.30 V7.1.1, Annex C.
// MMI Service Codes: No service code was specified
_LIT( KPENoServiceCode, "" );
// MMI Service Codes: All teleservices
_LIT( KPEAllTeleservices, "10" );
// MMI Service Codes: Voice teleservices
_LIT( KPETelephony, "11" );
// MMI Service Codes: All Data Services
_LIT( KPEAllDataServices, "12" );
// MMI Service Codes: Fax Services
_LIT( KPEFacsimileServices, "13" );
// MMI Service Codes: SMS Services
_LIT( KPEShortMessageServices, "16" );
// MMI Service Codes: Voice Group Call Service
_LIT( KPEVoiceGroupCallService, "17" );
// MMI Service Codes: Voice Broadcast Service
_LIT( KPEVoiceBroadcastService, "18" );
// MMI Service Codes: All teleservices except SMS
_LIT( KPEAllTeleservicesExceptSMS, "19" );
// MMI Service Codes: All Bearer Service
_LIT( KPEAllBearerServices, "20" );
// MMI Service Codes: All Async Services
_LIT( KPEAllAsyncServices, "21" );
// MMI Service Codes: All Sync Services
_LIT( KPEAllSyncServices, "22" );
// MMI Service Codes: All Data Circuit Sync
_LIT( KPEAllDataCircuitSync, "24" );
// MMI Service Codes: All Data Circuit Async
_LIT( KPEAllDataCircuitAsync, "25" );
// MMI Service Codes: All Dedicated Packet Access
_LIT( KPEAllDedicatedPacketAccess, "26" );
// MMI Service Codes: All Dedicated Pad Access
_LIT( KPEAllDedicatedPadAccess, "27" );
// MMI Service Codes: All Gprs Beares Services
_LIT( KPEAllGprsBearerServices, "99" );
// MMI Service Codess: PLMN specific teleservices
_LIT( KPEAllPlmnSpecificTeleservices, "50" );
_LIT( KPEPlmnSpecificTeleservice1, "51" );
_LIT( KPEPlmnSpecificTeleservice2, "52" );
_LIT( KPEPlmnSpecificTeleservice3, "53" );
_LIT( KPEPlmnSpecificTeleservice4, "54" );
_LIT( KPEPlmnSpecificTeleservice5, "55" );
_LIT( KPEPlmnSpecificTeleservice6, "56" );
_LIT( KPEPlmnSpecificTeleservice7, "57" );
_LIT( KPEPlmnSpecificTeleservice8, "58" );
_LIT( KPEPlmnSpecificTeleservice9, "59" );
_LIT( KPEPlmnSpecificTeleservice10, "60" );
_LIT( KPEPlmnSpecificTeleservice11, "61" );
_LIT( KPEPlmnSpecificTeleservice12, "62" );
_LIT( KPEPlmnSpecificTeleservice13, "63" );
_LIT( KPEPlmnSpecificTeleservice14, "64" );
_LIT( KPEPlmnSpecificTeleservice15, "65" );
// MMI Service Codess: PLMN specific bearer services
_LIT( KPEAllPlmnSpecificBearerServices, "70" );
_LIT( KPEPlmnSpecificBearerService1, "71" );
_LIT( KPEPlmnSpecificBearerService2, "72" );
_LIT( KPEPlmnSpecificBearerService3, "73" );
_LIT( KPEPlmnSpecificBearerService4, "74" );
_LIT( KPEPlmnSpecificBearerService5, "75" );
_LIT( KPEPlmnSpecificBearerService6, "76" );
_LIT( KPEPlmnSpecificBearerService7, "77" );
_LIT( KPEPlmnSpecificBearerService8, "78" );
_LIT( KPEPlmnSpecificBearerService9, "79" );
_LIT( KPEPlmnSpecificBearerService10, "80" );
_LIT( KPEPlmnSpecificBearerService11, "81" );
_LIT( KPEPlmnSpecificBearerService12, "82" );
_LIT( KPEPlmnSpecificBearerService13, "83" );
_LIT( KPEPlmnSpecificBearerService14, "84" );
_LIT( KPEPlmnSpecificBearerService15, "85" );
// ALS
_LIT( KPEAuxVoiceService, "89" );

//Fax Modem UI application UID
const TInt KPEFaxModemUIUid = 0x1000594E;
// TPESimState fisrt value
const TInt KPEStartupEnumerationFirstValue = 100;

// Indicates that dtmf sending is possible with this call. 
const TUint KPECapsDtmf = 0x00080000;

// SIM states
enum TPESimState
    {
    EPESimStatusUninitialized = KPEStartupEnumerationFirstValue,
    EPESimUsable,       // The Sim card is fully usable.    
    EPESimReadable,     // The SIM card is not fully usable, but the emergency number can be read.
    EPESimNotReady,     // The Sim card is present but not ready or usable.
    EPESimNotPresent,   // The Sim card is not present.
    EPESimNotSupported  // SIM/RUIM is not supported. Some CDMA phones do not support a SIM/RUIM at all.
    };
    
// Supplementary Services action type
enum TPESSAction
    {
    EPESSActionUnspecified = 0,
    EPESSActivation,
    EPESSDeactivation,
    EPESSInterrogation,
    EPESSRegistration,
    EPESSErasure,
    EPEPasswordOperation
    };

// Supplementary Services type
enum TPESSType
    {
    EPESSTypeUnspecified = 0,
    EPECallForwardingNotReachable,
    EPECallForwardingNoReply,
    EPECallForwardingBusy,
    EPECallForwardingUnconditional,
    EPECallForwardingAll,
    EPECallForwardingAllConditional,
    EPECallBarringAllOutgoing,
    EPECallBarringAllOutgoingInternational,
    EPECallBarringAllOutgoingInternationalExcHome,
    EPECallBarringAllIncoming,
    EPECallBarringAllIncomingWhenRoaming,
    EPECallBarringAllServices,
    EPECallBarringAllOutgoingServices,
    EPECallBarringAllIncomingServices,
    EPECallBarringPassword,
    EPECallWaiting,
    EPEClip,
    EPEClir,
    EPECnap,
    EPEColp,
    EPEColr,
    EPEEmlpp,
    EPEMsp,
    EPESpnp,
    EPEPin,
    EPEUnblockPin,
    EPESimLock
    };

//Forwarded information definition
NONSHARABLE_STRUCT( TPEForwardedInfo )
    {
    RMobilePhone::TMobilePhoneCFCondition iCondition;
    RMobilePhone::TMobileService iServiceGroup;
    RMobilePhone::TMobilePhoneCFStatus iStatus; 
    TPEPhoneNumber iPhoneNumber;
    TInt iTimeout;
    };

// Forwarding information definition
NONSHARABLE_STRUCT( TPEForwardingInfo )
    {
    TInt iGroupCount;
    TFixedArray<TPEForwardedInfo, KPEMaxServiceGroup> iForwardedInfo;
    };

NONSHARABLE_STRUCT( TPEActiveForwardInfo )
    {
    RMobilePhone::TMobilePhoneCFActive iActiveType;
    RMobilePhone::TMobileService  iServiceGroup;
    };

//Wait information definition
NONSHARABLE_STRUCT( TPEWaitInfo )
    {
    RMobilePhone::TMobileService iServiceGroup;
    RMobilePhone::TMobilePhoneCWStatus iStatus; 
    };

// Waiting information definition
NONSHARABLE_STRUCT( TPEWaitingInfo )
    {
    TInt iGroupCount;
    TFixedArray<TPEWaitInfo, KPEMaxServiceGroup> iWaitInfo;
    };

//Barred information definition
NONSHARABLE_STRUCT( TPEBarredInfo )
    {
    RMobilePhone::TMobilePhoneCBCondition iCondition;
    RMobilePhone::TMobileService iServiceGroup;
    RMobilePhone::TMobilePhoneCBStatus iStatus; 
    };

// Barring information definition
NONSHARABLE_STRUCT( TPEBarringInfo )
    {
    TInt iGroupCount;
    TFixedArray<TPEBarredInfo, KPEMaxServiceGroup> iBarredInfo;
    };

// Contains information of the Suplementary Services command.
NONSHARABLE_STRUCT( TPESSCommandInfo )                   // For SS Commands.
    {
    TPESSAction action;     // SS Action
    /*RMobilePhone::TMobileService*/ TInt group;       // MMI Service Code
    TPESSType type;         // Supplementary Services type
    };

NONSHARABLE_STRUCT( TPEConferenceMemberInfo )
    {
    TName iConferenceMemberName;
    TInt iConferenceMemberId;
    };

/**
 *  Offers message interface from phone application to phone engine.
 *
 *  ?more_complete_description
 *
 *  @lib phoneengine
 *  @since S60 v4.0 
 */
NONSHARABLE_CLASS( MPEPhoneModel ) 
    {
public:

    typedef TInt TPEMessagesToPhoneEngine;

    enum
        {
        // Audio messages                     1000 - 1999
        EPEMessageSetAudioMute              = 1000,
        EPEMessageSetAudioVolume,
        EPEMessageSetAudioOutput,

        // Call messages                      2000 - 2999
        EPEMessageAnswer                    = 2000,
        EPEMessageClientDial,
        EPEMessageContinueDTMFSending,
        EPEMessageDial,
        EPEMessagePlayDTMF,
        EPEMessageEndDTMF,
        EPEMessagePhoneNumberEdited,
        EPEMessageRelease,
        EPEMessageReleaseAll,
        EPEMessageReject,
        EPEMessageSendDTMF,
        EPEMessageStopDTMFSending,
        EPEMessageStopTonePlay,
        EPEMessageAutomaticAnswerOff,       //UI sends when soft reject is needed
        EPEMessageTerminateAllConnections,                
        EPEMessageSatCallRequestCompleted,
        EPEMessageClientDialEmergency,
        EPEMessageReplaceActive,
        EPEMessageContinueEmergencyCallInitialization,
        EPEMessageAcceptUnattendedTransfer,
        EPEMessageRejectUnattendedTransfer,

        EPEMessageDoUnattendedTransfer,
        EPEMessageForwardCallToAddress,
        EPEMessageDialServiceCall,
        // Contact messages                   3000 - 3999

        // Error messages                     4000 - 4999

        // Network messages                   5000 - 5999

        // Parser messages                    6000 - 6999

        // Security messages                  7000 - 7999

        // Service messages                   8000 - 8999
        EPEMessageDisableService            = 8000,

        // Startup messages                   9000 - 9999
        EPEMessagePEConstructionReady       = 9000,

        // Miscellaneous messages             10000 - 10999
        EPEMessageCheckEmergencyNumber      = 10000 // Request engine to check is entered number emergency.

        // Variant messages                   11000 - 12999
        // From KPEMessagesToPhoneEngineGsmFirst
        // to KPEMessagesToPhoneEngineGsmLast
        };

     virtual ~MPEPhoneModel() {};

    /**
     * Returns pointer to information object (CPEEngineInfo) 
     *
     * @since S60 v4.0
     * @return Return pointer to CPEEngineInfo object
     */
     virtual MPEEngineInfo* EngineInfo() = 0;
     
    /**
     * Reroutes messages from the phone application to the message handler
     *
     * @since S60 v4.0
     * @param aMessage is the message code
     */
     virtual void HandleMessage( const TInt aMessage ) = 0;
     
    /**
     * Returns the name of the specified message to PhoneEngine
     *
     * @since S60 v4.0
     * @param aMessage is the message code
     * @return String containing the name of the message
     */
     virtual TPtrC NameByMessageToPhoneEngine( const TInt aMessage ) const = 0;

    /**
     * Returns the name of the specified message from PhoneEngine
     *
     * @param aMessage is the message code
     * @return String containing the name of the message
     */
     virtual TPtrC NameByMessageFromPhoneEngine( const TInt aMessage ) const = 0;
     
    }; // MPEPhoneModel 

/**
* Utility class for call id.
*
*  @lib PhoneEngine.lib
*/
NONSHARABLE_CLASS( CallIdCheck )
    {
    public:
        static inline TBool IsValid( TInt aCallId ) 
            {
            return aCallId >= 0 && aCallId < KPEMaximumNumberOfCalls;
            }
        static inline TBool IsVoice( TInt aCallId )
            {
            return aCallId >= 0 && aCallId < KPEMaximumNumberOfVoiceCalls;
            }
        static inline TBool IsConference( TInt aCallId )
            {
            return aCallId >= KPEMaximumNumberOfVoiceCalls && 
                   aCallId < ( KPEMaximumNumberOfVoiceCalls + KPEMaximumNumberOfConferenceCalls );
            }
        static inline TBool IsVideo( TInt aCallId )
            {
            return aCallId >= ( KPEMaximumNumberOfVoiceCalls + KPEMaximumNumberOfConferenceCalls ) 
                && aCallId < ( KPEMaximumNumberOfVoiceCalls + KPEMaximumNumberOfConferenceCalls + 
                KPEMaximumNumberOfDataCalls );
            }            
    };

#endif // PEVIRTUALENGINE_H

//End of file
