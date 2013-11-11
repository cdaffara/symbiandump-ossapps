/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of CPEPhoneModel class 
                 member functions.
*
*/


//  INCLUDE FILES
#include "cpeactivestarter.h"
#include "cpeexternaldatahandler.h"
#include "cpemessagehandler.h"
#include "cpephonemodel.h"
#include "cpeservicehandling.h"
#include "cpesimstatemonitor.h"
#include "pepanic.pan"
#include "cperemotepartyinfomediator.h"

#include <ccce.h>
#include <cpeaudiodata.h>
#include <cpecallhandling.h>                   
#include <cpecontacthandlingproxy.h>
#include <cpeengineinfo.h>
#include <cpegsmaudiodata.h>
#include <cpeloghandlingproxy.h>
#include <featmgr.h>
#include <mccedtmfinterface.h>
#include <mpecontacthandling.h>
#include <mpeloghandling.h>
#include <xqservicerequest.h>
#include <xqappmgr.h>
#include "phoneservices.h"

// CONSTANTS
const TInt KDriveProfile ( 6 );
const TInt KPECallTimerOff = 0;
const TInt KTimesToSplitValue = 16;

// ==================== LOCAL FUNCTIONS ====================

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPEPhoneModel::CPEPhoneModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEPhoneModel::CPEPhoneModel(
        MEngineMonitor& aEngineMonitor      // The reference parameter for phone application communication
        ) : CPEPhoneModelIF(),
            iEngineMonitor( aEngineMonitor ),
            iStep( 0 )
    {
    }

// -----------------------------------------------------------------------------
// CPEPhoneModel::CPEPhoneModel
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEPhoneModel::CPEPhoneModel(
        MEngineMonitor& aEngineMonitor ,     // The reference parameter for phone application communication
        CPEAudioFactory& aAudioFactory
        ) : CPEPhoneModelIF(),
            iEngineMonitor( aEngineMonitor ),
            iStep( 0 ),
            iAudioFactory( &aAudioFactory )
    {
    }

// -----------------------------------------------------------------------------
// CPEPhoneModel::ConstructL
// EPOC default constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEPhoneModel::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::ConstructL: 1" );
    iEngineInfo = CPEEngineInfo::NewL();
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::ConstructL: 2" );
    User::LeaveIfError( iFsSession.Connect() );
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::ConstructL: 3" );
    FeatureManager::InitializeLibL();
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::ConstructL: 4" );
    iActiveStarter = CPEActiveStarter::NewL( this );
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::ConstructL: 5" );
    iActiveStarter->StartUp();
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::ConstructL: 6" );
    // Reserve needed callinfo and remoteinfo from heap.
    iCallInfo = new ( ELeave ) RMobileCall::TMobileCallInfoV3;
    }// ConstructL

// -----------------------------------------------------------------------------
// CPEPhoneModel::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEPhoneModel* CPEPhoneModel::NewL(
        MEngineMonitor& aEngineMonitor   //for phone application communication
        )
    {
    CPEPhoneModel* self = new ( ELeave ) CPEPhoneModel( aEngineMonitor );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }// NewL

// -----------------------------------------------------------------------------
// CPEPhoneModel::NewL
// Two-phased constructor for module testing.
// -----------------------------------------------------------------------------
//
CPEPhoneModel* CPEPhoneModel::NewL( 
    MEngineMonitor& aEngineMonitor,
    CPEAudioFactory& aAudioFactory )
    {
    CPEPhoneModel* self = 
        new ( ELeave ) CPEPhoneModel( aEngineMonitor, aAudioFactory );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CPEPhoneModel::~CPEPhoneModel()
    {   
    delete iMediatorCommunicationHandler;
    delete iSimChangedMonitor;
    delete iMessageHandler;
    delete iServiceHandling;
    delete iSimStatusMonitor;
    delete iContactHandling;
    delete iLogHandling;
    delete iAudioData;
    delete iCallHandling;
    delete iExternalDataHandler;
    delete iCallStackCutter;
    delete iCallInfo;

    if ( iActiveStarter )
        {
        delete iActiveStarter;
        }

    FeatureManager::UnInitializeLib();
    iFsSession.Close();
    delete iEngineInfo;
    delete iConvergedCallEngine;
    delete iPhoneServices;
    }// ~CPEPhoneModel

// -----------------------------------------------------------------------------
// CPEPhoneModel::EngineInfo
// Returns the pointer of the CPEEngineInfo object.
// -----------------------------------------------------------------------------
//
MPEEngineInfo* CPEPhoneModel::EngineInfo()
    {
    return iEngineInfo;
    }

// -----------------------------------------------------------------------------
// Returns the pointer of the CPEEngineInfo object.
// -----------------------------------------------------------------------------
//
MPEDataStore* CPEPhoneModel::DataStore()
    {
    return iEngineInfo;
    }    

// -----------------------------------------------------------------------------
// CPEPhoneModel::DataStoreExt
// Returns the pointer to CPEExternalDataHandler object
// -----------------------------------------------------------------------------
//
MPEExternalDataHandler* CPEPhoneModel::DataStoreExt()
    {
    return iExternalDataHandler;
    }

// -----------------------------------------------------------------------------
// CPEPhoneModel::MediatorCommandHandler
// Returns the pointer to CPERemotePartyInfoMediator object
// -----------------------------------------------------------------------------
//
CPERemotePartyInfoMediator* CPEPhoneModel::MediatorCommunicationHandler()
    {
    return iMediatorCommunicationHandler;
    }



// -----------------------------------------------------------------------------
// CPEPhoneModel::NameByMessageToPhoneEngine
// Return message name, given the id of message.
// -----------------------------------------------------------------------------
//
#ifdef TEF_LOGGING_ENABLED
TPtrC CPEPhoneModel::NameByMessageToPhoneEngine(
        const MPEPhoneModel::TPEMessagesToPhoneEngine aMessage
        ) const
    {
    switch ( aMessage )
        {
        // Audio messages                     1000 - 1999
        case MPEPhoneModel::EPEMessageSetAudioMute:
            return MESSAGE("EPEMessageSetAudioMute");
        case MPEPhoneModel::EPEMessageSetAudioVolume:
            return MESSAGE("EPEMessageSetAudioVolume");
        case MPEPhoneModel::EPEMessageSetAudioOutput:
            return MESSAGE("EPEMessageSetAudioOutput");

        // Call messages                      2000 - 2999
        case MPEPhoneModel::EPEMessageAnswer:
            return MESSAGE("EPEMessageAnswer");
        case MPEPhoneModel::EPEMessageClientDial:
            return MESSAGE("EPEMessageClientDial");
        case MPEPhoneModel::EPEMessageClientDialEmergency:
            return MESSAGE("EPEMessageClientDialEmergency");
        case MPEPhoneModel::EPEMessageDialServiceCall:
            return MESSAGE("EPEMessageDialServiceCall");
        case MPEPhoneModel::EPEMessageContinueEmergencyCallInitialization:
            return MESSAGE("EPEMessageContinueEmergencyCallInitialization");
        case MPEPhoneModel::EPEMessageContinueDTMFSending:
            return MESSAGE("EPEMessageContinueDTMFSending");
        case MPEPhoneModel::EPEMessageDial:
            return MESSAGE("EPEMessageDial");
        case MPEPhoneModel::EPEMessagePlayDTMF:
            return MESSAGE("EPEMessagePlayDTMF");
        case MPEPhoneModel::EPEMessageEndDTMF:
            return MESSAGE("EPEMessageEndDTMF");
        case MPEPhoneModel::EPEMessagePhoneNumberEdited:
            return MESSAGE("EPEMessagePhoneNumberEdited");
        case MPEPhoneModel::EPEMessageRelease:
            return MESSAGE("EPEMessageRelease");
        case MPEPhoneModel::EPEMessageReleaseAll:
            return MESSAGE("EPEMessageReleaseAll");
        case MPEPhoneModel::EPEMessageReject:
            return MESSAGE("EPEMessageReject");
        case MPEPhoneModel::EPEMessageSendDTMF:
            return MESSAGE("EPEMessageSendDTMF");
        case MPEPhoneModel::EPEMessageStopDTMFSending:
            return MESSAGE("EPEMessageStopDTMFSending");
        case MPEPhoneModel::EPEMessageStopTonePlay:
            return MESSAGE("EPEMessageStopTonePlay");
        case MPEPhoneModel::EPEMessageAutomaticAnswerOff:
            return MESSAGE("EPEMessageAutomaticAnswerOff");
        case MPEPhoneModel::EPEMessageTerminateAllConnections:
            return MESSAGE("EPEMessageTerminateAllConnections");
        case MPEPhoneModel::EPEMessageSatCallRequestCompleted:
            return MESSAGE("EPEMessageSatCallRequestCompleted");
            
        case MPEPhoneModel::EPEMessageAcceptUnattendedTransfer:
            return MESSAGE("EPEMessageAcceptUnattendedTransfer");
        case MPEPhoneModel::EPEMessageRejectUnattendedTransfer:
            return MESSAGE("EPEMessageRejectUnattendedTransfer");

        case MPEPhoneModel::EPEMessageDoUnattendedTransfer:
            return MESSAGE("EPEMessageDoUnattendedTransfer");
        case MPEPhoneModel::EPEMessageForwardCallToAddress:
            return MESSAGE("EPEMessageForwardCallToAddress");

        // Contact messages                   3000 - 3999

        // Error messages                     4000 - 4999

        // Network messages                   5000 - 5999

        // Parser messages                    6000 - 6999

        // Security messages                  7000 - 7999

        // Settings and Shared Data messages  8000 - 8999

        // Service messages                   8000 - 8999
        case MPEPhoneModel::EPEMessageDisableService:
            return MESSAGE("EPEMessageDisableService");
        // Startup messages                   9000 - 9999
        case MPEPhoneModel::EPEMessagePEConstructionReady:
            return MESSAGE("EPEMessagePEConstructionReady");

        // Miscellaneous messages             10000 - 10999
        case MPEPhoneModel::EPEMessageCheckEmergencyNumber:
            return MESSAGE("EPEMessageCheckEmergencyNumber");
            
        // Variant messages                   11000 - 12999
        
        // Audio messages                         11000 - 11199

        // Call messages                          11200 - 11399
        case CPEPhoneModelIF::EPEMessageAddConferenceMember:
            return MESSAGE("EPEMessageAddConferenceMember");
        case CPEPhoneModelIF::EPEMessageCreateConference:
            return MESSAGE("EPEMessageCreateConference");
        case CPEPhoneModelIF::EPEMessageDropConferenceMember:
            return MESSAGE("EPEMessageDropConferenceMember");
        case CPEPhoneModelIF::EPEMessageGoOneToOne:
            return MESSAGE("EPEMessageGoOneToOne");
        case CPEPhoneModelIF::EPEMessageHangUpConference:
            return MESSAGE("EPEMessageHangUpConference");
        case CPEPhoneModelIF::EPEMessageHold:
            return MESSAGE("EPEMessageHold");
        case CPEPhoneModelIF::EPEMessageHoldConference:
            return MESSAGE("EPEMessageHoldConference");
        case CPEPhoneModelIF::EPEMessageResume:
            return MESSAGE("EPEMessageResume");
        case CPEPhoneModelIF::EPEMessageResumeConference:
            return MESSAGE("EPEMessageResumeConference");
        case CPEPhoneModelIF::EPEMessageSwap:
            return MESSAGE("EPEMessageSwap");
        case CPEPhoneModelIF::EPEMessageSwapConference:
            return MESSAGE("EPEMessageSwapConference");
        case CPEPhoneModelIF::EPEMessageTransfer:
            return MESSAGE("EPEMessageTransfer");           

        // Contact messages                       11400 - 11599

        // Error messages                         11600 - 11799

        // Network messages                       11800 - 12999

        // Parser messages                        12000 - 12199
        case CPEPhoneModelIF::EPEMessageCancelSSstringCommand:
            return MESSAGE("EPEMessageCancelSSstringCommand");

        // Security messages                      12200 - 12399

        // Settings and Shared Data messages      12400 - 12599

        // Miscellaneous messages                 12600 - 12799
        
        default:
            TEFLOGSTRING2( KTAINT, "pe:cpephonemodel::namebymessagetophoneengine, unknown message = %d", static_cast<TInt>(aMessage));
            return MESSAGE("Unknown message");
        } 
    }
#else
TPtrC CPEPhoneModel::NameByMessageToPhoneEngine(
        const TInt /*aMessage*/ ) const
    {
    return MESSAGE("Not available");
    }
#endif // TEF_LOGGING_ENABLED

// -----------------------------------------------------------------------------
// CPEPhoneModel::NameByMessageFromPhoneEngine
// Return message name, given the id of message.
// -----------------------------------------------------------------------------
//
#ifdef TEF_LOGGING_ENABLED
TPtrC CPEPhoneModel::NameByMessageFromPhoneEngine(
        const TInt aMessage ) const
    {
    switch ( aMessage )
        {
        // Audio messages                     1000 - 1999
        case MEngineMonitor::EPEMessageAudioDtmfEvent:
            return MESSAGE("EPEMessageAudioDtmfEvent");
        case MEngineMonitor::EPEMessageAudioDTMFPlayComplete:
            return MESSAGE("EPEMessageAudioDTMFPlayComplete");
        case MEngineMonitor::EPEMessageAudioOutputPreferenceChanged:
            return MESSAGE("EPEMessageAudioOutputPreferenceChanged");
        case MEngineMonitor::EPEMessageAudioMuteChanged:
            return MESSAGE("EPEMessageAudioMuteChanged");
        case MEngineMonitor::EPEMessageAudioOutputChanged:
            return MESSAGE("EPEMessageAudioOutputChanged");
        case MEngineMonitor::EPEMessageAvailableAudioOutputsChanged:
            return MESSAGE("EPEMessageAvailableAudioOutputsChanged");
        case MEngineMonitor::EPEMessageAudioPlayStopped:
            return MESSAGE("EPEMessageAudioPlayStopped");
        case MEngineMonitor::EPEMessageAudioVolumeChanged:
            return MESSAGE("EPEMessageAudioVolumeChanged");

        // Call messages                      2000 - 2999
        case MEngineMonitor::EPEMessageAnswer:
            return MESSAGE("EPEMessageAnswer");
        case MEngineMonitor::EPEMessageAnswering:
            return MESSAGE("EPEMessageAnswering");
        case MEngineMonitor::EPEMessageCallControlCapsChanged:
            return MESSAGE("EPEMessageCallControlCapsChanged");
        case MEngineMonitor::EPEMessageCallWaiting:
            return MESSAGE("EPEMessageCallWaiting");
        case MEngineMonitor::EPEMessageCallWaitingAnswered:
            return MESSAGE("EPEMessageCallWaitingAnswered");
        case MEngineMonitor::EPEMessageChangedCallDuration:
            return MESSAGE("EPEMessageChangedCallDuration");
        case MEngineMonitor::EPEMessageConferenceIdle:
            return MESSAGE("EPEMessageConferenceIdle");
        case MEngineMonitor::EPEMessageConferenceCapsChange:
            return MESSAGE("EPEMessageConferenceCapsChange");
        case MEngineMonitor::EPEMessageConnected:
            return MESSAGE("EPEMessageConnected");
        case MEngineMonitor::EPEMessageConnectedConference:
            return MESSAGE("EPEMessageConnectedConference");
        case MEngineMonitor::EPEMessageConnecting:
            return MESSAGE("EPEMessageConnecting");
        case MEngineMonitor::EPEMessageDialing:
            return MESSAGE("EPEMessageDialing");
        case MEngineMonitor::EPEMessageDialingError:
            return MESSAGE("EPEMessageDialingError");
        case MEngineMonitor::EPEMessageDisconnecting:
            return MESSAGE("EPEMessageDisconnecting");
        case MEngineMonitor::EPEMessageDisconnectingConference:
            return MESSAGE("EPEMessageDisconnectingConference");
        case MEngineMonitor::EPEMessageDTMFSendingAborted:
            return MESSAGE("EPEMessageDTMFSendingAborted");
        case MEngineMonitor::EPEMessageHeld:
            return MESSAGE("EPEMessageHeld");
        case MEngineMonitor::EPEMessageHeldConference:
            return MESSAGE("EPEMessageHeldConference");
        case MEngineMonitor::EPEMessageIdle:
            return MESSAGE("EPEMessageIdle");
        case MEngineMonitor::EPEMessageIncoming:
            return MESSAGE("EPEMessageIncoming");
        case MEngineMonitor::EPEMessageInitiatedEmergencyCall:
            return MESSAGE("EPEMessageInitiatedEmergencyCall");
        case MEngineMonitor::EPEMessageInitiatedEmergencyWhileActiveVideo:
            return MESSAGE("EPEMessageInitiatedEmergencyWhileActiveVideo");
        case MEngineMonitor::EPEMessagePacketNetworkDetached:
            return MESSAGE("EPEMessagePacketNetworkDetached");
        case MEngineMonitor::EPEMessageRemoteBusy:
            return MESSAGE("EPEMessageRemoteBusy");            
        case MEngineMonitor::EPEMessageSendingDTMF:
            return MESSAGE("EPEMessageSendingDTMF");
        case MEngineMonitor::EPEMessageSentDTMF:
            return MESSAGE("EPEMessageSentDTMF");
        case MEngineMonitor::EPEMessageSentSSString:
            return MESSAGE("EPEMessageSentSSString");
        case MEngineMonitor::EPEMessageStoppedDTMF:
            return MESSAGE("EPEMessageStoppedDTMF");
        case MEngineMonitor::EPEMessageSwapped:
            return MESSAGE("EPEMessageSwapped");
        case MEngineMonitor::EPEMessageTransferDone:
            return MESSAGE("EPEMessageTransferDone");
        case MEngineMonitor::EPEMessageWentOneToOne:
            return MESSAGE("EPEMessageWentOneToOne");
        case MEngineMonitor::EPEMessageDisconnectingWithInband:
            return MESSAGE("EPEMessageDisconnectingWithInband");
        case MEngineMonitor::EPEMessageContinuingDTMFSending:
            return MESSAGE("EPEMessageContinuingDTMFSending");
        case MEngineMonitor::EPEMessageCallAdded:
            return MESSAGE("EPEMessageCallAdded");
        case MEngineMonitor::EPEMessageRemoteTerminated:
            return MESSAGE("EPEMessageRemoteTerminated");
        case MEngineMonitor::EPEMessageUnknown:
            return MESSAGE("EPEMessageUnknown");
        case MEngineMonitor::EPEMessageDataPortLoaned:
            return MESSAGE("EPEMessageDataPortLoaned");
            
        case MEngineMonitor::EPEMessageUnattendedTransferRequest:
            return MESSAGE("EPEMessageUnattendedTransferRequest");
        case MEngineMonitor::EPEMessageRemoteForwarding:
            return MESSAGE("EPEMessageRemoteForwarding");
        case MEngineMonitor::EPEMessageMovedPermanently:
            return MESSAGE("EPEMessageMovedPermanently");
        case MEngineMonitor::EPEMessageMultipleChoices:
            return MESSAGE("EPEMessageMultipleChoices");
        case MEngineMonitor::EPEMessageRemotePartyInfoChanged:
            return MESSAGE("EPEMessageRemotePartyInfoChanged");
        case MEngineMonitor::EPEMessageConnectingWithInband:
            return MESSAGE("EPEMessageConnectingWithInband");

        // Contact messages                   3000 - 3999
        case MEngineMonitor::EPEMessageThumbnailLoadingCompleted:
            return MESSAGE("EPEMessageThumbnailLoadingCompleted");

        // Error messages                     4000 - 4999
        case MEngineMonitor::EPEMessageError:
            return MESSAGE("EPEMessageError");
        case MEngineMonitor::EPEMessageAudioHandlingError:
            return MESSAGE("EPEMessageAudioHandlingError");
        case MEngineMonitor::EPEMessageCallHandlingError:
            return MESSAGE("EPEMessageCallHandlingError");
        case MEngineMonitor::EPEMessageContactHandlingError:
            return MESSAGE("EPEMessageContactHandlingError");
        case MEngineMonitor::EPEMessageLogHandlingError:
            return MESSAGE("EPEMessageLogHandlingError");

        case MEngineMonitor::EPEMessageServiceHandlingError:
            return MESSAGE("EPEMessageServiceHandlingError");
        // Network messages                   5000 - 5999
        case MEngineMonitor::EPEMessageCallSecureStatusChanged:
            return MESSAGE("EPEMessageCallSecureStatusChanged");
        case MEngineMonitor::EPEMessageNetworkRegistrationStatusChange:
            return MESSAGE("EPEMessageNetworkRegistrationStatusChange");

        // Parser messages                    6000 - 6999
        case MEngineMonitor::EPEMessageSSRequestFailed:
            return MESSAGE("EPEMessageSSRequestFailed");

        // Security messages                  7000 - 7999
        case MEngineMonitor::EPEMessageBadBarringPasswordChange:
            return MESSAGE("EPEMessageBadBarringPasswordChange");
        case MEngineMonitor::EPEMessageBadPin1Change:
            return MESSAGE("EPEMessageBadPin1Change");
        case MEngineMonitor::EPEMessageBadPin2Change:
            return MESSAGE("EPEMessageBadPin2Change");
        case MEngineMonitor::EPEMessageBadPin1Unblock:
            return MESSAGE("EPEMessageBadPin1Unblock");
        case MEngineMonitor::EPEMessageBadPin2Unblock:
            return MESSAGE("EPEMessageBadPin2Unblock");
        case MEngineMonitor::EPEMessageBarringPasswordChanged:
            return MESSAGE("EPEMessageBarringPasswordChanged");
        case MEngineMonitor::EPEMessageChangedSimLock:
            return MESSAGE("EPEMessageChangedSimLock");
        case MEngineMonitor::EPEMessageFailedChangeSimLock:
            return MESSAGE("EPEMessageFailedChangeSimLock");
        case MEngineMonitor::EPEMessageFailedNewPin1AndPin2NotEqual:
            return MESSAGE("EPEMessageFailedNewPin1AndPin2NotEqual");
        case MEngineMonitor::EPEMessageFailedSecurityCommandAlreadyActive:
            return MESSAGE("EPEMessageFailedSecurityCommandAlreadyActive");
        case MEngineMonitor::EPEMessagePin1Changed:
            return MESSAGE("EPEMessagePin1Changed");
        case MEngineMonitor::EPEMessagePin2Changed:
            return MESSAGE("EPEMessagePin2Changed");
        case MEngineMonitor::EPEMessagePin1Unblocked:
            return MESSAGE("EPEMessagePin1Unblocked");
        case MEngineMonitor::EPEMessagePin2Unblocked:
            return MESSAGE("EPEMessagePin2Unblocked");

        // Settings and Shared Data messages  8000 - 8999
        case MEngineMonitor::EPEMessageProfileChanged:
            return MESSAGE("EPEMessageProfileChanged");

        // Startup messages                   9000 - 9999
        case MEngineMonitor::EPEMessagePEConstructionReady:
            return MESSAGE("EPEMessagePEConstructionReady");
        case MEngineMonitor::EPEMessagePEConstructionFailed:
            return MESSAGE("EPEMessagePEConstructionFailed");
        case MEngineMonitor::EPEMessagePEConstructionTwoPhaseReady:
            return MESSAGE("EPEMessagePEConstructionTwoPhaseReady");     

        // Miscellaneous messages             10000 - 10999
        case MEngineMonitor::EPEMessageInValidEmergencyNumber:
            return MESSAGE("EPEMessageInValidEmergencyNumber");
        case MEngineMonitor::EPEMessageValidEmergencyNumber:
            return MESSAGE("EPEMessageValidEmergencyNumber");
        case MEngineMonitor::EPEMessageStartATDialing:
            return MESSAGE("EPEMessageStartATDialing");
        case MEngineMonitor::EPEMessageLogEventSaved:
            return MESSAGE("EPEMessageLogEventSaved");
        case MEngineMonitor::EPEMessageDoNotSendMe:
            return MESSAGE("EPEMessageDoNotSendMe");
            
        // Variant messages                   11000 - 12999
        
        // Audio messages                         11000 - 11199

        // Call messages                          11200 - 11399
        case MEngineMonitor::EPEMessageActiveBarring:
            return MESSAGE("EPEMessageActiveBarring");
        case MEngineMonitor::EPEMessageActiveCallForwarding:
            return MESSAGE("EPEMessageActiveCallForwarding");
        case MEngineMonitor::EPEMessageAddedConferenceMember:
            return MESSAGE("EPEMessageAddedConferenceMember");
        case MEngineMonitor::EPEMessageALSLineChanged:
            return MESSAGE("EPEMessageALSLineChanged");
        case MEngineMonitor::EPEMessageCallBarred:
            return MESSAGE("EPEMessageCallBarred");
        case MEngineMonitor::EPEMessageIncCallIsForw:
            return MESSAGE("EPEMessageIncCallIsForw");
        case MEngineMonitor::EPEMessageIncCallForwToC:
            return MESSAGE("EPEMessageIncCallForwToC");
        case MEngineMonitor::EPEMessageOutCallForwToC:
            return MESSAGE("EPEMessageOutCallForwToC");
        case MEngineMonitor::EPEMessageForwardUnconditionalModeActive:
            return MESSAGE( "EPEMessageForwardUnconditionalModeActive" );
        case MEngineMonitor::EPEMessageForwardConditionallyModeActive:
            return MESSAGE( "EPEMessageForwardConditionallyModeActive" );
        case MEngineMonitor::EPEMessageDroppedConferenceMember:
            return MESSAGE("EPEMessageDroppedConferenceMember");
        case MEngineMonitor::EPEMessageGoingOneToOne:
            return MESSAGE("EPEMessageGoingOneToOne");
        case MEngineMonitor::EPEMessageIssuedUSSDRequest:
            return MESSAGE("EPEMessageIssuedUSSDRequest");
        case MEngineMonitor::EPEMessageRemoteCreatedConference:
            return MESSAGE("EPEMessageRemoteCreatedConference");
        case MEngineMonitor::EPEMessageRemoteHeld:
            return MESSAGE("EPEMessageRemoteHeld");
        case MEngineMonitor::EPEMessageRemoteResumed:
            return MESSAGE("EPEMessageRemoteResumed");
        case MEngineMonitor::EPEMessageTempClirSuppressUnsuccessful:
            return MESSAGE("EPEMessageTempClirSuppressUnsuccessful");
        case MEngineMonitor::EPEMessageTempClirActivationUnsuccessful:
            return MESSAGE("EPEMessageTempClirActivationUnsuccessful");
        case MEngineMonitor::EPEMessageOutgoingCallBarred:
            return MESSAGE("EPEMessageOutgoingCallBarred");
            
        // Contact messages                       11400 - 11599

        // Error messages                         11600 - 11799

        // Network messages                       11800 - 12999
        case MEngineMonitor::EPEMessageServiceEnabled:
            return MESSAGE("EPEMessageServiceEnabled");
        case MEngineMonitor::EPEMessageServiceEnabling:
            return MESSAGE("EPEMessageServiceEnabling");    
        case MEngineMonitor::EPEMessageNoService:
            return MESSAGE("EPEMessageNoService");  
        case MEngineMonitor::EPEMessageServiceDisabled:
            return MESSAGE("EPEMessageServiceDisabled");  

        // Parser messages                        12000 - 12199
        case MEngineMonitor::EPEMessageActivateRfsDeep:
            return MESSAGE("EPEMessageActivateRfsDeep");
        case MEngineMonitor::EPEMessageActivateRfsNormal:
            return MESSAGE("EPEMessageActivateRfsNormal");
        case MEngineMonitor::EPEMessageIssuedSSRequest:
            return MESSAGE("EPEMessageIssuedSSRequest");
        case MEngineMonitor::EPEMessageIssuingSSRequest:
            return MESSAGE("EPEMessageIssuingSSRequest");

        // Security messages                      12200 - 12399

        // Settings messages                      12400 - 12599
        case MEngineMonitor::EPEMessageBarringNotification:
            return MESSAGE("EPEMessageBarringNotification");
        case MEngineMonitor::EPEMessageClipStatusNotification:
            return MESSAGE("EPEMessageClipStatusNotification");
        case MEngineMonitor::EPEMessageClirStatusNotification:
            return MESSAGE("EPEMessageClirStatusNotification");
        case MEngineMonitor::EPEMessageCnapStatusNotification:
            return MESSAGE("EPEMessageCnapStatusNotification");
        case MEngineMonitor::EPEMessageColpStatusNotification:
            return MESSAGE("EPEMessageColpStatusNotification");
        case MEngineMonitor::EPEMessageColrStatusNotification:
            return MESSAGE("EPEMessageColrStatusNotification");
        case MEngineMonitor::EPEMessageForwardingNotification:
            return MESSAGE("EPEMessageForwardingNotification");
        case MEngineMonitor::EPEMessageVmbxNumberLine1Changed:
            return MESSAGE("EPEMessageVmbxNumberLine1Changed");
        case MEngineMonitor::EPEMessageVmbxNumberLine2Changed:
            return MESSAGE("EPEMessageVmbxNumberLine2Changed");
        case MEngineMonitor::EPEMessageWaitingNotification:
            return MESSAGE("EPEMessageWaitingNotification");
        case MEngineMonitor::EPEMessageAlsStatusChanged:
            return MESSAGE("EPEMessageAlsStatusChanged");

        // Miscellaneous messages                 12600 - 12799
        case MEngineMonitor::EPEMessageSIMChanged:
            return MESSAGE("EPEMessageSIMChanged");
        case MEngineMonitor::EPEMessageSIMStateChanged:
            return MESSAGE("EPEMessageSIMStateChanged");
        default:
            TEFLOGSTRING2( KTAINT, "pe: cpephonemodel::namebymessagefromphoneengine, unknown message = %d", static_cast<TInt>(aMessage));
            return MESSAGE("Unknown message");
        } 
    }
#else
TPtrC CPEPhoneModel::NameByMessageFromPhoneEngine(
        const TInt /*aMessage*/ ) const
    { 
    return MESSAGE("Not available");
    }
#endif // TEF_LOGGING_ENABLED

// -----------------------------------------------------------------------------
// CPEPhoneModel::HandleMessage
// Handles messages from phone application.
// -----------------------------------------------------------------------------
//
void CPEPhoneModel::HandleMessage(
        const TInt aMessage ) 
    {
    TInt errorCode( ECCPErrorNone );
    #ifdef TEF_LOGGING_ENABLED
    TInt callId;
    callId = iEngineInfo->CallId(); // Get CallId
    TPtrC messageName = NameByMessageToPhoneEngine( aMessage );
    TEFLOGSTRING3( KTAINT, 
        "PE cpephonemodel::handlemessage, messageName=%s, callId=%d", 
        messageName.Ptr( ), 
        callId );
    #endif
    
    if ( !iMessageHandler )
        {
        TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::HandleMessage: Not ready" );
        return;
        }

    switch ( aMessage )
        {
        case MPEPhoneModel::EPEMessageTerminateAllConnections:
            errorCode = iMessageHandler->HandleTerminateAllConnections( );
            break;
        case MPEPhoneModel::EPEMessageDial:
            //Make a dial request
            errorCode = iMessageHandler->HandleDialCall( EFalse );
            break;
        case MPEPhoneModel::EPEMessageReleaseAll: //Make HangUp requests to all calls
            errorCode = iMessageHandler->HandleReleaseAll( );            
            break;
        case MPEPhoneModel::EPEMessageRelease:     //Make HangUp request
        case CPEPhoneModelIF::EPEMessageHangUpConference:
            errorCode = iMessageHandler->HandleReleaseCall( );            
            break;
        case MPEPhoneModel::EPEMessageAnswer:                        //Make Answer request
            errorCode = iMessageHandler->HandleAnswerCall( EFalse );
            break;
        case MPEPhoneModel::EPEMessageReject:                        //Make Reject request
            errorCode = iMessageHandler->HandleRejectCall( );
            break;
        case MPEPhoneModel::EPEMessageSetAudioMute:                  //Make Mute microphone request
            iMessageHandler->HandleSetAudioMute( );
            break;
        case MPEPhoneModel::EPEMessageSetAudioOutput:
            errorCode = iMessageHandler->HandleSetAudioOutput();
            break;
        case MPEPhoneModel::EPEMessageSetAudioVolume:              //Sets audio volume.
            iMessageHandler->HandleSetAudioVolume( );
            break;
        case CPEPhoneModelIF::EPEMessageHold:                        //Make Hold request
        case CPEPhoneModelIF::EPEMessageHoldConference:
            errorCode = iMessageHandler->HandleHoldCall( );
            break;
        case CPEPhoneModelIF::EPEMessageSwap:                        //Make Swap request
        case CPEPhoneModelIF::EPEMessageSwapConference:
            errorCode = iMessageHandler->HandleSwapCalls( );
            break;
        case CPEPhoneModelIF::EPEMessageResume:         //Make Resume request
        case CPEPhoneModelIF::EPEMessageResumeConference:
            errorCode = iMessageHandler->HandleResumeCall( );
            break;
        case CPEPhoneModelIF::EPEMessageDropConferenceMember:        // Drop conference member.
            errorCode = iMessageHandler->HandleDropConferenceMember( );
            break;
        case CPEPhoneModelIF::EPEMessageTransfer:                    //Make Transfer request
            errorCode = iMessageHandler->HandleTransferCalls( );
            break;
        case CPEPhoneModelIF::EPEMessageCreateConference:            // Build conference call.
            errorCode = iMessageHandler->HandleBuildConferenceCall( );
            break;
        case CPEPhoneModelIF::EPEMessageGoOneToOne:          // Go one to one. 
            errorCode = iMessageHandler->HandleGoOneToOne( );
            break;
        case CPEPhoneModelIF::EPEMessageAddConferenceMember:            //Add call to conference request
            errorCode = iMessageHandler->HandleAddConferenceMember();
            break;
        case MPEPhoneModel::EPEMessageSendDTMF:                    //Send dtmf string
            errorCode = iMessageHandler->HandleSendDtmf( );
            break;
        case MPEPhoneModel::EPEMessagePlayDTMF:                        //Play DTMF
            TRAP( errorCode, iMessageHandler->HandlePlayDTMFL() );
            break;
        case MPEPhoneModel::EPEMessageEndDTMF:                        //End DTMF playing
            errorCode = iMessageHandler->HandleEndDTMF();
            break;
        case MPEPhoneModel::EPEMessageStopTonePlay:    //Stop InBand tone playing.
            iMessageHandler->HandleStopInbandTonePlay( );
            break;
        case MPEPhoneModel::EPEMessageClientDial:  //Make Dial request(Client originated)
            errorCode = iMessageHandler->HandleDialCall( ETrue );
            break;
        case MPEPhoneModel::EPEMessageClientDialEmergency:  
            //Make emergency dial request(Client originated)
            errorCode = iMessageHandler->HandleDialEmergencyCall( ETrue );
            break;
        case MPEPhoneModel::EPEMessageContinueEmergencyCallInitialization:
            //Make emergency dial request
            iMessageHandler->ContinueDialEmergency();
            break;
        case MPEPhoneModel::EPEMessageContinueDTMFSending:     //Continue DTMF sending.
            iMessageHandler->HandleContinueDtmfSending( ETrue );
            break;
        case MPEPhoneModel::EPEMessageStopDTMFSending: //Stop DTMF sending.
            iMessageHandler->HandleContinueDtmfSending( EFalse );
            break;
        case CPEPhoneModelIF::EPEMessageCancelSSstringCommand:
            errorCode = iMessageHandler->HandleCancelSSstringCommand( );
            break;
        case MPEPhoneModel::EPEMessagePhoneNumberEdited:
            TRAP( errorCode, iMessageHandler->HandlePhoneNumberEditedL( ) );
            break;
        case MPEPhoneModel::EPEMessageCheckEmergencyNumber:
            errorCode = iMessageHandler->HandleEmergencyCheck( );
            break;
        case MPEPhoneModel::EPEMessageAutomaticAnswerOff:
            iMessageHandler->HandleAutomaticAnswerOff( );
            break;
        case CPEPhoneModelIF::EPEMessageSetALSLine:
            iCallHandling->SetActiveLine();
            break;
        case MPEPhoneModel::EPEMessageSatCallRequestCompleted: 
            iMessageHandler->HandleSatCallRequestCompleted();
            break;
        case MPEPhoneModel::EPEMessageReplaceActive:
            errorCode = iMessageHandler->HandleReplaceActive();
            break;
        case MPEPhoneModel::EPEMessageAcceptUnattendedTransfer:
            errorCode = iMessageHandler->
                HandleUnattendedTransferRequestResponse( ETrue );
            break;
        case MPEPhoneModel::EPEMessageRejectUnattendedTransfer:
            errorCode = iMessageHandler->
                HandleUnattendedTransferRequestResponse( EFalse );
            break;

        case MPEPhoneModel::EPEMessageDoUnattendedTransfer:
            errorCode = iMessageHandler->HandleUnattendedTransfer();
            break;
        case MPEPhoneModel::EPEMessageForwardCallToAddress:
            errorCode = iMessageHandler->ForwardCallToAddress();
            break;
        case MPEPhoneModel::EPEMessageDisableService:
            iMessageHandler->HandleDisableService();
            break;
        case MPEPhoneModel::EPEMessageDialServiceCall:  //Make Dial request
            errorCode = iMessageHandler->HandleDialCall( EFalse );
            break;
        default:
            errorCode = ECCPErrorNotFound;
            break;
        } 

    if( errorCode )
        {
        TPEErrorInfo errorInfo;
        errorInfo.iErrorCode = errorCode;
        errorInfo.iCallId = KPECallIdNotUsed;
        errorInfo.iErrorType = EPECcp;
        iEngineMonitor.HandleError( errorInfo );
        }
    }// HandleMessage

// -----------------------------------------------------------------------------
// CPEPhoneModel::SendMessage
// Reroutes messages to the over loaded SendMessage( TInt aMessage, TInt aCallId ) method.
// Method creates new callid and calls overloaded method.
// -----------------------------------------------------------------------------
//
void CPEPhoneModel::SendMessage(
       const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage )
    {    
    SendMessage( aMessage, KPECallIdNotUsed );
    }// SendMessage( 1 param )

// -----------------------------------------------------------------------------
// CPEPhoneModel::SendMessage
// Sends messages to the phone application
// Method sets call parameters to the CPEEngineInfo object and
// Method reroutes messages to the phone application.
// -----------------------------------------------------------------------------
//
void CPEPhoneModel::SendMessage( 
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
        const TInt aCallId )
    {
    TInt errorCode( ECCPErrorNone );
    TPEState callState;      
    //Save message to local variable so that it can be modified
    TInt message( aMessage );

    #ifdef TEF_LOGGING_ENABLED
    TPtrC messageName = NameByMessageFromPhoneEngine( 
        static_cast<MEngineMonitor::TPEMessagesFromPhoneEngine>( aMessage ) );
    TEFLOGSTRING3( KTAINT, 
        "PE cpephonemodel::sendmessage, messageName=%s, callId=%d"
        , messageName.Ptr( )
        , aCallId );
    #endif
    
    if ( iMessageHandler )
        {
        errorCode = ProcessMessage( aMessage, aCallId );          
        }    
        
    if ( DelayMessageSending( aMessage, aCallId ) )
        {
        message = KPEDontSendMessage;
        }    
    
    // Set Call State to the EngineInfo
    if ( CallIdCheck::IsConference( aCallId ))
        {
        callState = iCallHandling->GetCallState( aCallId );
        if ( callState > EPEStateUnknown && callState < EPEStateMaximumState )
            {   
            iEngineInfo->SetConferenceCallState( static_cast<TPEState>( callState ) );
            }
        else
            {
            iEngineInfo->SetConferenceCallState( EPEStateUnknown );
            }
        }
    if ( CallIdCheck::IsValid( aCallId ))
        {
        callState = iCallHandling->GetCallState( aCallId );
        if ( callState > EPEStateUnknown && callState < EPEStateMaximumState )
            {   
            iEngineInfo->SetCallState( static_cast<TPEState>( callState ), aCallId );
            }
        else
            {
            iEngineInfo->SetCallState( EPEStateUnknown, aCallId );
            }
        }

    if ( message == MEngineMonitor::EPEMessageOutgoingCallBarred )
        {
        iEngineInfo->SetOutgoingCallBarringActivated( ETrue );
        message = KPEDontSendMessage;
        }
    
    if ( ( message == MEngineMonitor::EPEMessageIncoming ) )
        {
        if ( FeatureManager::FeatureSupported( KFeatureIdDriveMode ) )
            {
            //Check for Drive mode
            if ( iEngineInfo->ProfileId() == KDriveProfile )
                {
                //Drive mode is enabled, do not forward message
                message = KPEDontSendMessage;
                }
            }
        }

    if ( message == MEngineMonitor::EPEMessageChangedCallDuration )    
        {   
        TInt callTimerSetting( EFalse );
        // safe to ignore error code here, duration display equals to zero == off if it fails
        iExternalDataHandler->Get( EPECallDurationDisplaySetting, callTimerSetting );
        // Message EPEMessageChangedCallDuration send supressed, if duration display is OFF
        if ( callTimerSetting == KPECallTimerOff )
            {
            message = KPEDontSendMessage;
            }
        }

    //MessageHandler may return error code
    if ( errorCode != ECCPErrorNone && errorCode != KPEDontSendMessage )
        {
        #ifdef TEF_LOGGING_ENABLED
        TPtrC messageName2 = 
            NameByMessageFromPhoneEngine( 
            static_cast<MEngineMonitor::TPEMessagesFromPhoneEngine>( message ) );
        TEFLOGSTRING3( 
            KTAERROR,
            "PE cpephonemodel::sendmessage > iEngineMonitor.HandleError: messageName=%s, CallId=%d",
        messageName2.Ptr( ), aCallId );
        #endif
        SetCallError( aCallId );
        TPEErrorInfo errorInfo;
        errorInfo.iErrorCode = errorCode;
        errorInfo.iCallId = aCallId;
        errorInfo.iErrorType = EPECcp;
        iEngineMonitor.HandleError( errorInfo );
        }
    else if ( message == MEngineMonitor::EPEMessageCallHandlingError )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "PE cpephonemodel::sendmessage > iEngineMonitor.HandleError: messageName=EPEMessageCallHandlingError, CallId=%d"
        , aCallId );
        SetCallError( aCallId );
	    TPEErrorInfo errorInfo = iEngineInfo->ErrorInfo();
    	errorInfo.iCallId = aCallId;
    	errorInfo.iErrorType = EPECcp;

        if ( ECCPErrorCCCallRejected == errorInfo.iErrorCode )
            {
            // Call direction is not set if the call was rejected before Dialing-state
            iEngineInfo->SetCallDirection( RMobileCall::EMobileOriginated, aCallId );
            iMessageHandler->AddSIMRejectedMoCsCallToLog( aCallId );
            }
    	iEngineMonitor.HandleError( errorInfo );
        }
    else if ( message == MEngineMonitor::EPEMessageServiceHandlingError )
        {
        TEFLOGSTRING( 
            KTAERROR, 
            "PE cpephonemodel::sendmessage > iEngineMonitor.HandleError: messageName=EPEMessageServiceHandlingError" );
        SetCallError( aCallId );
        TPEErrorInfo errorInfo = iEngineInfo->ErrorInfo();
        errorInfo.iCallId = aCallId;
        errorInfo.iErrorType = EPECch;
        iEngineMonitor.HandleError( errorInfo );
        }
    else if ( message != KPEDontSendMessage && errorCode != KPEDontSendMessage )
        {
        // Map message for PhoneApplication, since it doesn't use 
        // EPEMessageDisconnectingWithInband message
        if( message == MEngineMonitor::EPEMessageDisconnectingWithInband )
            {
            message = MEngineMonitor::EPEMessageDisconnecting;
            }
        else if ( message == MEngineMonitor::EPEMessageConnectingWithInband )
            {            
            message = MEngineMonitor::EPEMessageConnecting;
            }
        #ifdef TEF_LOGGING_ENABLED
        TPtrC messageName2 = NameByMessageFromPhoneEngine
            ( static_cast<MEngineMonitor::TPEMessagesFromPhoneEngine>( message ) );
        TEFLOGSTRING3( 
            KTAINT, 
            "PE cpephonemodel::sendmessage > iEngineMonitor HandleMessage: messageName=%s, CallId=%d"
        , messageName2.Ptr( ), aCallId );
        #endif
        
        __ASSERT_DEBUG( 
            ( KPEDontSendMessage <= message < KPEMessagesFromPhoneEngineGsmLast ),
            Panic( EPEPanicIllegalCommand ) );
        iEngineMonitor.HandleMessage( 
            static_cast<MEngineMonitor::TPEMessagesFromPhoneEngine>( message ), 
            aCallId );
        }
    }// SendMessage( 2 params )

// -----------------------------------------------------------------------------
// CPEPhoneModel::ProcessMessage
// Process messages from the subsystems by passing them to message handlder.
// -----------------------------------------------------------------------------
TInt CPEPhoneModel::ProcessMessage( 
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage, 
        const TInt aCallId )
    {
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::ProcessMessage" );
    TInt errorCode( ECCPErrorNone );
    
    //Get Message
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageDialing:
            TRAP( errorCode, iMessageHandler->HandleDialingStateL( aCallId ) );
            break;

        case MEngineMonitor::EPEMessageIncoming:
            TRAP( errorCode, iMessageHandler->HandleIncomingCallL( aCallId ) );            
            break;

        case MEngineMonitor::EPEMessageAnswering:
            if ( CallIdCheck::IsVideo( aCallId )  )
                {
                // Answering video calls
                iMessageHandler->HandleStopInbandTonePlay( );
                iMessageHandler->HandleAudioRouting( ETrue, aCallId  );
                }
            else if ( iEngineInfo->CallType( aCallId ) == EPECallTypeCSVoice || 
                    iEngineInfo->CallType( aCallId ) == EPECallTypeVoIP )
                {
                iMessageHandler->HandleAudioRouting( EFalse, aCallId);
                }
                
            break;

        case MEngineMonitor::EPEMessageConnected:            
            iEngineInfo->SetResumeHeldCall( ETrue, aCallId );
            if ( CallIdCheck::IsVoice( aCallId ))
                {
                errorCode = iMessageHandler->HandleConnectedState( aCallId );
                }
            else if ( CallIdCheck::IsVideo( aCallId )) 
                {
                // Video call : for video call logging and duration
                errorCode = iMessageHandler->HandleVideoCallConnected( aCallId );
                    
                }
            // VoIP call audio routing happens when 'connected' message is
            // received because of differences in VoIP and CS call audio
            // activation.
            if ( EPECallTypeVoIP == iEngineInfo->CallType( aCallId ) )
                {
                iMessageHandler->HandleAudioRouting( EFalse, aCallId );
                }
                
            break;                        

        case MEngineMonitor::EPEMessageIdle:
            if ( CallIdCheck::IsVoice( aCallId ))
                {
                errorCode = iMessageHandler->HandleVoiceCallIdleState( aCallId );
                }
            else if ( CallIdCheck::IsVideo( aCallId )) 
                {
                // Video call : for video call logging and duration
                errorCode = iMessageHandler->HandleVideoCallIdle( aCallId );
                }
                        
            iEngineInfo->SetOutgoingCallBarringActivated( EFalse );
            break;

        case MEngineMonitor::EPEMessageRemoteBusy:
            if ( CallIdCheck::IsVoice( aCallId ))
                {
                iMessageHandler->HandleCallHandlingError( aCallId, ETrue );                
                }
            break;

        case MEngineMonitor::EPEMessageCallHandlingError:            
            iMessageHandler->HandleCallHandlingError( aCallId, EFalse );            
            break;
                
        case MEngineMonitor::EPEMessageAudioOutputPreferenceChanged:
            errorCode = iMessageHandler->HandleRoutePreferenceChanged();
            break;
                    
        case MEngineMonitor::EPEMessageConferenceIdle:
            if ( CallIdCheck::IsConference( aCallId ) )
                {
                errorCode = iMessageHandler->HandleConferenceIdleState( aCallId );
                }
            break;
            
        case MEngineMonitor::EPEMessageAnswer: // Automatic answer complete
            errorCode = iMessageHandler->HandleAnswerCall( ETrue );
            break;

        case MEngineMonitor::EPEMessageAlsStatusChanged:
        case MEngineMonitor::EPEMessageSIMStateChanged:
            iMessageHandler->HandleSimStateChanged( );
            break;

        case MEngineMonitor::EPEMessagePEConstructionReady:
            iMessageHandler->HandleStartUp( );
            break;

        case MEngineMonitor::EPEMessageDisconnecting:
            errorCode = iMessageHandler->HandleDisconnecting( aCallId );
            break;

        case MEngineMonitor::EPEMessageDisconnectingWithInband:
            errorCode = iMessageHandler->HandleDisconnectingWithInband( aCallId );
            break;

        case MEngineMonitor::EPEMessageConnectedConference:
            iEngineInfo->SetResumeHeldCall( ETrue, aCallId );
            break;

        case MEngineMonitor::EPEMessageSentDTMF:
            errorCode = iMessageHandler->HandleDtmfSent();
            break;

        case MEngineMonitor::EPEMessageStartATDialing:
            errorCode = iMessageHandler->HandleDialCall( EFalse );
            iMessageHandler->HandleATDialingStarted( errorCode ? EFalse : ETrue );
            break;

        // MO call connecting.
        case MEngineMonitor::EPEMessageConnecting:            
            iEngineInfo->SetInbandTone( ECCPRemoteAlerting );
            iAudioData->PlayInbandTone();
            break;   
        /*        
        case MEngineMonitor::EPEMessageInitiatedEmergencyCall:            
            if ( iCallStackCutter )
                {
                delete iCallStackCutter;
                iCallStackCutter = NULL;
                }
            TRAPD( err, iCallStackCutter = CIdle::NewL( CActive::EPriorityHigh ) );
            if ( !err )
                {
                iCallBackMessage = aMessage; 
                iCallBackCallId  = aCallId;
                iCallStackCutter->Start( TCallBack( CallBackMessageSend , this ) );
                }
            message = KPEDontSendMessage;
            break;
        */        
        case MEngineMonitor::EPEMessageDroppedConferenceMember:
            iMessageHandler->HandleDroppedConferenceMember();
            break;
                
        case MEngineMonitor::EPEMessageDTMFSendingAborted:
            iEngineInfo->SetDtmfString( KNullDesC() );
            break;

        case MEngineMonitor::EPEMessageServiceEnabled:
            errorCode = iMessageHandler->HandleServiceEnabled();
            break;
            
        case MEngineMonitor::EPEMessageRemotePartyInfoChanged:
            iMessageHandler->HandleRemotePartyInfoChanged( aCallId );
            break;    

        default:
            break;
            }    
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEPhoneModel::DelayMessageSending
// Determines which messages needs to be delayed and delays sending them
// to Engine monitor.
// -----------------------------------------------------------------------------
TBool CPEPhoneModel::DelayMessageSending( 
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
        const TInt aCallId )
    {    
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::DelayMessageSending" );
        
    TBool sendingDelayed( EFalse );
        
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageInitiatedEmergencyCall:
            {              
            sendingDelayed = ETrue;            
            }
            break;
        case MEngineMonitor::EPEMessageColpNumberAvailable:
            {              
            sendingDelayed = ETrue;            
            }
            break;
        default:
            // Other messages cause no action.
            break;
        }
    
        if ( sendingDelayed )
            {        
            if ( iCallStackCutter )
                {
                delete iCallStackCutter;
                iCallStackCutter = NULL;
                }
            TRAPD( err, iCallStackCutter = CIdle::NewL( CActive::EPriorityHigh ) );
            if ( !err )
                {
                iCallBackMessage = aMessage; 
                iCallBackCallId  = aCallId;
                iCallStackCutter->Start( TCallBack( CallBackMessageSend , this ) );
                }
            }                
        
    return sendingDelayed;
    }

// -----------------------------------------------------------------------------
// CPEPhoneModel::RecallSteps
// Method informs about failures in starting modules.
// -----------------------------------------------------------------------------
//
void CPEPhoneModel::RecallSteps()
    {
    SendMessage( MEngineMonitor::EPEMessagePEConstructionFailed );
    iActiveStarter = NULL;
    }

// -----------------------------------------------------------------------------
// CPEPhoneModel::StepL
// Method starts and connects modules asynchronically.
// -----------------------------------------------------------------------------
//
TBool CPEPhoneModel::StepL()
    {
    TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 1" );
    TBool continueStepping = ETrue;

    switch ( iStep )
        {
        case CPEPhoneModel::EPEInitializeCceConnection:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 2.1" );
            iConvergedCallEngine = CCCE::NewL();
            MCCEDtmfObserver* tempPointer( NULL );
            MCCEDtmfInterface& tempReference = iConvergedCallEngine->DtmfInterfaceL( *tempPointer );
            iDtmfInterface = &tempReference;
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 2.2" );
            break;
            }
        case CPEPhoneModel::EPEExternalData:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 3.1" );
            iExternalDataHandler = CPEExternalDataHandler::NewL( *this );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 3.2" );
            break;
            }
        case CPEPhoneModel::EPECallHandling:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 4.1" );
            iCallHandling = CPECallHandling::NewL( *this, *iConvergedCallEngine, *iDtmfInterface  );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 4.2" );
            break;
            }
        case CPEPhoneModel::EPEAudioHandling:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 5.1" );
            if ( iAudioFactory )
                {
                iAudioData = CPEGsmAudioData::NewL( 
                        *this, iFsSession, *iAudioFactory );
                }
            else
                {
                iAudioData = CPEGsmAudioData::NewL( *this, iFsSession );
                }
            iDtmfInterface->AddObserverL( *iAudioData );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 5.2" );
            break;
            }
        case CPEPhoneModel::EPELogHandlingPhaseOne:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 6.1" );
            // Create Log Handling proxy along with dummy implementation
            iLogHandling = CPELogHandlingProxy::CreateFirstPhaseL();
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 6.2" );
            break;
            }
        case CPEPhoneModel::EPEContactHandlingPhaseOne:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 7.1" );
            // Create Contact Handling proxy along with dummy implementation
            iContactHandling = CPEContactHandlingProxy::CreateFirstPhaseL(); 
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 7.2" );
            break;
            }
        case CPEPhoneModel::EPESimStateMonitor:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 9.1" );
            //started after message handler construction
            iSimStatusMonitor = CPESimStateMonitor::NewL( 
                *this, 
                CPESimStateMonitor::EEventSimStatus );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 9.2" );
            break;
            }
        case CPEPhoneModel::EPEServiceHandling:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 10.1" );
            iServiceHandling = CPEServiceHandling::NewL( *this );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 10.2" );
            break;
            }
        case CPEPhoneModel::EPEMessageHandler:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 11.1" );
            iMessageHandler = CPEMessageHandler::NewL( *this, 
                                                          *iCallHandling, 
                                                          iEngineMonitor, 
                                                          *iAudioData, 
                                                          *iLogHandling, 
                                                          *iContactHandling, 
                                                          *iSimStatusMonitor,
                                                          *iExternalDataHandler,
                                                          *iServiceHandling,
                                                          iFsSession );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 11.2" );
            break;
            }
        case CPEPhoneModel::EPESimChangedMonitor:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 12.1" );
            iSimChangedMonitor = CPESimStateMonitor::NewL( 
                *this, 
                CPESimStateMonitor::EEventSimChanged );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 12.2" );
            iSimChangedMonitor->Start();
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 12.3" );
            break;
            }
        case CPEPhoneModel::EPESimStatusMonitorStart:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 13.1" );
            iSimStatusMonitor->Start();
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 13.2" );
            break;
            }
        case CPEPhoneModel::EPELogHandlingPhaseTwo:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 14.1" );
            // Finalize Log Handling contruction
            static_cast< CPELogHandlingProxy* > ( iLogHandling )->CreateSecondPhaseL( 
                *this, 
                iFsSession );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 14.2" );
            break;
            }
        case CPEPhoneModel::EPEContactHandlingPhaseTwo:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 15.1" );
            static_cast< CPEContactHandlingProxy* >( iContactHandling )->
                   CreateSecondPhaseL( *this, iFsSession );
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 15.2" );
            break; 
            }
        case CPEPhoneModel::EPEMediatorCommandHandler:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 16.1" );
            // Start Mediator command listener
            iMediatorCommunicationHandler = CPERemotePartyInfoMediator::NewL( *this );
            break; 
            }
        case CPEPhoneModel::EPEPhoneServices:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 16.1" );
            __ASSERT_DEBUG( iMessageHandler, Panic( EPEPanicNullPointer ) );
            __ASSERT_DEBUG( iEngineInfo, Panic( EPEPanicNullPointer ) );
            iPhoneServices = new PhoneServices(*iMessageHandler, *iEngineInfo);
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 16.2" );
            break;
            }

        case CPEPhoneModel::EPENetworkHandling:
            {
            TEFLOGSTRING( KTAOBJECT, "CPEPhoneModel::StepL: 18.1" );
            continueStepping = EFalse;
            QString service("networkhandlingstarter");
            QString interface("com.nokia.symbian.IStart");
            QString operation("start()");
            XQApplicationManager appManager;
            
            QScopedPointer<XQAiwRequest> request(
                    appManager.create(service, interface, operation, false));
            if (request == NULL) {
                TEFLOGSTRING(KTAOBJECT, 
                        "CPEPhoneModel::StepL 18.2 error, service not found");
                break;
            }
            XQRequestInfo info;
            info.setForeground(false);
            request->setInfo(info);
            request->setSynchronous(false);
            if (!request->send()) {
                int error = request->lastError();
                TEFLOGSTRING2(KTAOBJECT, 
                        "CPEPhoneModel::StepL 18.3 error %d", error);
                break;
            }
            TEFLOGSTRING(KTAOBJECT, "CPEPhoneModel::StepL: 18.4");
            break;
            }

        default:
            {
            Panic( EPEPanicIndexOutOfRange );
            break;
            }
        }

    if ( !continueStepping )
        {
        // Note, if you add cases, you have to set 
        // continueStepping = EFalse into the last case.
        SendMessage( MEngineMonitor::EPEMessagePEConstructionReady );
        iActiveStarter = NULL;
        }
    else
        {
        ++iStep;
        }
    return continueStepping;
    }// StepL

// -----------------------------------------------------------------------------
// Handle internal messages
// -----------------------------------------------------------------------------
//
void CPEPhoneModel::HandleInternalMessage( 
    const TInt aMessage ) 
    {
    HandleMessage( aMessage );
    }

// -----------------------------------------------------------------------------
// ConstructContactHandlingPhaseTwoL
// -----------------------------------------------------------------------------
//
void CPEPhoneModel::ConstructContactHandlingPhaseTwoL()
    {
    TEFLOGSTRING( KTAOBJECT, 
        "PE CPEPhoneModel::ConstructContactHandlingPhaseTwo, start" );
    // Finalize Contact Handling contruction
    static_cast< CPEContactHandlingProxy* >( iContactHandling )->
        CreateSecondPhaseL( *this, iFsSession );
    TEFLOGSTRING( KTAOBJECT, 
        "PE CPEPhoneModel::ConstructContactHandlingPhaseTwo, complete" );
    SendMessage( MEngineMonitor::EPEMessagePEConstructionTwoPhaseReady );   
    }

// -----------------------------------------------------------------------------
// CallBackSendMessage
// -----------------------------------------------------------------------------
//
TInt CPEPhoneModel::CallBackMessageSend( TAny* aSelf )
    {
    TEFLOGSTRING( KTAINT, 
        "PE CPEPhoneModel::CallBackMessageSend, start" );
      
    CPEPhoneModel* self = static_cast<CPEPhoneModel*>( aSelf );
  
    self->iEngineMonitor.HandleMessage( self->iCallBackMessage, self->iCallBackCallId );
   
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// SetCallError
// -----------------------------------------------------------------------------
//
void CPEPhoneModel::SetCallError( TInt aCallId )
    {
    TInt callError = KErrNone;
    if ( iCallHandling )
        {
        TInt err = iCallHandling->GetCallInfo( *iCallInfo, aCallId );
        
        if ( err == KErrNone && ( iCallInfo->iExitCode & 0xFFFF0000 ) ) 
            {
            callError = ( iCallInfo->iExitCode >> KTimesToSplitValue ); 
            //Set protocol spesific error code to TPEErrorInfo
            EngineInfo()->SetProtocolError( callError, aCallId );
            }
        }
    }
// End of File
