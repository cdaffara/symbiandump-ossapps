/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implements class CSPCall which provides call functionality
*
*/


#include <etelmm.h>
#include <etel.h>
#include <centralrepository.h>
#include <settingsinternalcrkeys.h>
#include <ccpdefs.h>
#include <gsmerror.h>
#include <etelsat.h>
#include <cccpcallparameters.h>

#include "cspcall.h"
#include "cspetelcallrequester.h"
#include "cspetelcalleventmonitor.h"
#include "cspcallinfomonitor.h"
#include "cspetelcallstatusmonitor.h"
#include "cspetelcallcapsmonitor.h"
#include "csptransferprovider.h"
#include "cspforwardprovider.h"
#include "csplogger.h"
#include "csppanic.pan"
#include "cspconsts.h"
#include "cspaudiohandler.h"
#include "mcspcommoninfo.h"
#include "mcspsecuritysettingobserver.h"
#include "tcspskypeidparser.h"
#include "cspuuimonitor.h"
#include "cspuuimessagesender.h"

const TInt KTimesToSplitValue = 16;
const TInt KStreamsStartWaitTime = 2000000; //2 sec

// ---------------------------------------------------------------------------
// CSPCall::~CSPCall
// ---------------------------------------------------------------------------
//
CSPCall::~CSPCall()
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCall::~CSPCall <");

    delete iParams;
    iCommonInfo.IndicateHangupComplete( *this );

    if ( iAudioHandler
         && iAudioStatus == ECSPCallAudioStatusActive )
        {
        iAudioStatus = ECSPCallAudioStatusInactive;
        iAudioHandler->Stop();
        }

    delete iUserToUserInformation;
    delete iRequester;
    delete iCallEventMonitor;
    delete iCallStatusMonitor;
    delete iCallCapsMonitor;
    delete iCallInfoMonitor;
    delete iForwardProvider;
    delete iTransferProvider;
    delete iUUIMonitor;
    delete iUUIMessageSender;
    delete iSkypeId;

    iObservers.Close();
    if ( iCall.SubSessionHandle() )
        {
        iCall.Close();
        }

    CSPLOGSTRING(CSPOBJECT, "CSPCall::~CSPCall >");
    }

// ---------------------------------------------------------------------------
// CSPCall::SetAudioHandler
// ---------------------------------------------------------------------------
//
void CSPCall::SetAudioHandler( CSPAudioHandlerBase* aHandler )
    {
    CSPLOGSTRING2(CSPINT, "CSPCall::SetAudioHandler handler: %x", aHandler);
    iAudioHandler = aHandler;

    if (iAudioHandler)
        {
        iAudioHandler->SetObserver(*this);
        }
    }

// ---------------------------------------------------------------------------
// CSPCall::SecuritySettingChanged
// ---------------------------------------------------------------------------
//
void CSPCall::SecuritySettingChanged( TInt aValue )
    {
    switch ( aValue )
        {
        case MCSPSecuritySettingObserver::ESecureCall:
            {
            CSPLOGSTRING(CSPINT,
                    "CSPCall::SecuritySettingChanged Sending 'secure call' event");
            NotifyCallEventOccurred( MCCPCallObserver::ECCPSecureCall );
            break;
            }
        case MCSPSecuritySettingObserver::ENotSecureCall:
            {
            CSPLOGSTRING(CSPINT,
                    "CSPCall::SecuritySettingChanged Sending 'not secure call' event");
            NotifyCallEventOccurred( MCCPCallObserver::ECCPNotSecureCall );
            break;
            }
        case MCSPSecuritySettingObserver::ESecureNotSpecified:
            {
            CSPLOGSTRING(CSPINT,
                    "CSPCall::SecuritySettingChanged Sending SecureNotSpecified");
            NotifyCallEventOccurred( MCCPCallObserver::ECCPSecureNotSpecified );
            break;
            }

        default:
            {
            CSPLOGSTRING(CSPERROR, "CSPCall::SecuritySettingChanged, \
                unknown event");
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPCall::RemoteAlertingToneStatusChanged
// Sends EarlyMediaStarted event to observer if network has started to
// play remote alerting tone. There is only one known use case: Network
// starts playing alerting tone during connecting state.
//
// This RemoteAlertingToneStatusChanged is called for every call,
// so it is calls responsibility to determine if the
// observer should be notified.
// ---------------------------------------------------------------------------
//
void CSPCall::RemoteAlertingToneStatusChanged(
    RMmCustomAPI::TRemoteAlertingToneStatus aNewStatus )
    {
    CSPLOGSTRING2(CSPINT,
        "CSPCall::RemoteAlertingToneStatusChanged new status: %d", aNewStatus );

    if ( aNewStatus == RMmCustomAPI::EUiStopTone ||
         aNewStatus == RMmCustomAPI::EUiNoTone ) // NW tells us to stop playing
        {
        // Connecting is only state where network starts playing the tone.
        if ( iCallState == MCCPCallObserver::ECCPStateConnecting )
            {
            NotifyCallEventOccurred(
                MCCPCallObserver::ECCCSPEarlyMediaStarted );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyCallStateChangedETel
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyCallStateChangedETel( RMobileCall::TMobileCallStatus aState )
    {
    CSPLOGSTRING3(CSPINT,
                  "CSPCall::NotifyCallStateChangedETel < state: %d this: %x",
                  aState, this );
    switch ( aState )
        {
        /*
        Cannot receive any mapping call statuses from ETel to following
        optional states:

        ECCPStateForwarding   MO call is being forwarded at receiver end
        ECCPStateQueued       Call is queued locally.

        The commented observer calls are for CS specific call states.
        */

        // Indicates that the call is idle or unknown.
        case RMobileCall::EStatusIdle:
        case RMobileCall::EStatusUnknown:
            {
            CSPLOGSTRING(CSPINT, "CSPCall::NotifyCallStateChangedETel Idle");

            // If audio still active
            if ( iAudioStatus == ECSPCallAudioStatusActive
                 && iAudioHandler
                 && iParams->CallType() == CCPCall::ECallTypeCSVoice )
                {
                iAudioStatus = ECSPCallAudioStatusInactive;
                iAudioHandler->Stop();
                }

            // Notify error in case not going through disconnecting
            if ( iCallState != MCCPCallObserver::ECCPStateDisconnecting
                     && !iTerminationErrorNotified )
                {
                CheckAndNotifyTerminationError();
                }

            NotifyCallStateChanged( MCCPCallObserver::ECCPStateIdle );
            iCommonInfo.IndicateHangupComplete( *this );
            break;
            }
        // The call is dialling.
        case RMobileCall::EStatusDialling:
            CSPLOGSTRING(CSPINT, "CSPCall callstate Dialling");
            {
            if ( iAudioHandler && iParams->CallType() == CCPCall:: ECallTypeCSVoice )
                {
                iAudioStatus = ECSPCallAudioStatusActive;
                iAudioHandler->Start();
                }

            iDontReportTerm = EFalse;
            NotifyCallStateChanged( MCCPCallObserver::ECCPStateDialling );
            break;
            }
        // Indicates that the MT call is ringing but not answered yet by
        // the local user
        case RMobileCall::EStatusRinging:
            {
            CSPLOGSTRING(CSPINT, "CSPCall::NotifyCallStateChangedETel Ringing");
            NotifyCallStateChanged( MCCPCallObserver::ECCPStateRinging );
            break;
            }
        // Indicates that the local user has answered the MT call but
        // the network has not acknowledged the call connection yet.
        case RMobileCall::EStatusAnswering:
            {
            CSPLOGSTRING(CSPINT, "CSPCall::NotifyCallStateChangedETel Answering");

            if ( !iMobileOriginated
                && iAudioHandler
                && iAudioStatus == ECSPCallAudioStatusInactive
                && iParams->CallType() == CCPCall::ECallTypeCSVoice )
                {
                iAudioStatus = ECSPCallAudioStatusActive;
                iAudioHandler->Start();
                }

            NotifyCallStateChanged( MCCPCallObserver::ECCPStateAnswering );
            break;
            }
        // MO Call: the network notifies to the MS that the remote party
        // is now ringing.
        case RMobileCall::EStatusConnecting:
            {
            CSPLOGSTRING(CSPINT, "CSPCall::NotifyCallStateChangedETelConnecting");
            RMmCustomAPI::TRemoteAlertingToneStatus tone =
                iCommonInfo.GetRemoteAlertingToneStatus();
            if ( tone == RMmCustomAPI::EUiNoTone ||
                 tone == RMmCustomAPI::EUiStopTone )
                {
                NotifyCallStateChangedWithInband( MCCPCallObserver::ECCPStateConnecting );
                }
            else
                {
                NotifyCallStateChanged( MCCPCallObserver::ECCPStateConnecting );
                }
            break;
            }
        // Indicates that call is connected and active.
        case RMobileCall::EStatusConnected:
            {
            CSPLOGSTRING(CSPINT, "CSPCall::NotifyCallStateChangedETel Connected");

            // Expect MCSPAudioHandlerObserver::AudioStartingFailed callback
            // when call audio control streams fail to start within specified
            // timeout.
            if (iAudioHandler && (iParams->CallType() == CCPCall::ECallTypeCSVoice))
                {
                iAudioHandler->ReportAudioFailureAfterTimeout(
                        KStreamsStartWaitTime);
                }

            iDontReportTerm = ETrue;
            NotifyCallStateChanged( MCCPCallObserver::ECCPStateConnected );

            // Agreement with TSY is that the
            // COLP number is available in connected state.
            NotifyRemotePartyNumberChanged();
            break;
            }
        // Indicates that call is disconnecting. (Same as RCall::HangingUp)
        case RMobileCall::EStatusDisconnecting:
            {
            CSPLOGSTRING(CSPINT, "CSPCall::NotifyCallStateChangedETel Disconnecting");

            if ( !iTerminationErrorNotified )
                {
                CheckAndNotifyTerminationError();
                }

            NotifyCallStateChanged( MCCPCallObserver::ECCPStateDisconnecting );
            break;
            }
        // Indicates that the call is disconnecting with inband data
        // (to enable the network to send an audio tone), signifying
        // that the call is not to be released until user terminates call
        case RMobileCall::EStatusDisconnectingWithInband:
            {
            CSPLOGSTRING(CSPINT,
                "CSPCall::NotifyCallStateChangedETel DisconnectingWithInband");

            if ( !iTerminationErrorNotified )
                {
                CheckAndNotifyTerminationError();
                }

            NotifyCallStateChangedWithInband(
                    MCCPCallObserver::ECCPStateDisconnecting );
            break;
            }
        // Indicates that the call is connected but on hold.
        case RMobileCall::EStatusHold:
            {
            CSPLOGSTRING(CSPINT, "CSPCall::NotifyCallStateChangedETel Hold");
            NotifyCallStateChanged( MCCPCallObserver::ECCPStateHold );
            break;
            }
        case RMobileCall::EStatusTransferring:
            {
            CSPLOGSTRING(CSPINT, "CSPCall::NotifyCallStateChangedETel Transferring");
            NotifyCallStateChanged( MCCPCallObserver::ECCPStateTransferring );
            break;
            }

        // Indicates that call is undergoing temporary channel loss
        // and it may or may not be reconnected.
        case RMobileCall::EStatusReconnectPending: // fall through
        //Indicates that the call is the non-active half of an alternating
        // call. This call is waiting for its active half or the remote
        // end to switch alternating call mode.
        case RMobileCall::EStatusWaitingAlternatingCallSwitch: // fall through
        case RMobileCall::EStatusTransferAlerting:
            {
            CSPLOGSTRING2(CSPINT,
                    "CSPCall::NotifyCallStateChangedETel no special handling for state %d",
                    aState);
            break;
            }

        default:
            {
            CSPLOGSTRING(CSPERROR,
                    "CSPCall::NotifyCallStateChangedETel callstate UNKNOWN");
            break;
            }
        }
    CSPLOGSTRING(CSPREQIN, "CSPCall::NotifyCallStateChangedETel >");
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyCallEventOccurred
// Notifies observers about call events
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyCallEventOccurred(
    MCCPCallObserver::TCCPCallEvent aEvent )
    {
    CSPLOGSTRING2(CSPINT,
            "CSPCall::NotifyCallEventOccurred < event: %d", aEvent);

    TInt obsCount = iObservers.Count();
    for ( TInt i = 0; i < obsCount; i++ )
        {
        CSPLOGSTRING2(CSPINT, "CSPCall::NotifyCallEventOccurred obs=%d",i);
        MCCPCallObserver *obs = iObservers[i];
        if ( obs )
            {
            obs->CallEventOccurred( aEvent, this );
            }

        CSPLOGSTRING2(CSPINT, "CSPCall::NotifyCallEventOccurred ok obs=%d",i);
        }

    CSPLOGSTRING2(CSPINT, "CSPCall::NotifyCallEventOccurred > event: %d", aEvent);
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyTransferCallEventOccurred
// Forward notification of transfer event to it's provider
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyTransferCallEventOccurred(
    MCCPTransferObserver::TCCPTransferEvent aEvent )
    {
    CSPLOGSTRING2(CSPINT, "CSPCall::NotifyTransferCallEventOccurred %d", aEvent);
    // forward the method call to CSPTransferProvider object
    iTransferProvider->TransferEventOccurred( aEvent );
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyForwardEventOccurred
// Notifies observers about call events
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyForwardEventOccurred(
    MCCPForwardObserver::TCCPForwardEvent aEvent )
    {
    CSPLOGSTRING2(CSPINT, "CSPCall::NotifyForwardEventOccurred %d", aEvent);
    iForwardProvider->NotifyForwardEventOccurred( aEvent );
    }

// ---------------------------------------------------------------------------
// CSPCall::CallName
// Get call name
// ---------------------------------------------------------------------------
//
void CSPCall::CallName( TName& aCallName )
    {
    CSPLOGSTRING(CSPINT, "CSPCall::CallName");
    aCallName.Zero();
    aCallName.Append( iCallName );
    }

// ---------------------------------------------------------------------------
// CSPCall::DialRequestFailed
// Handles situations when dial fails
// ---------------------------------------------------------------------------
//
void CSPCall::DialRequestFailed( TInt aErrorCode )
    {
    CSPLOGSTRING(CSPERROR, "CSPCall::DialRequestFailed");
    iDialCompletionCode = aErrorCode;

    // If state has not changed
    // ( e.g. in case of dial could not be initiated by network problem)
    if ( iCallStatusMonitor->State() ==
                   RMobileCall::RMobileCall::EStatusUnknown )
        {
        NotifyErrorOccurred( iRequester->MapError( aErrorCode ) );

        // Force release since otherwise call remains unreleased
        CSPLOGSTRING(CSPERROR, "CSPCall::DialRequestFailed() Force Idle");
        NotifyCallStateChanged( MCCPCallObserver::ECCPStateIdle );
        }

    // If dial request has completed after notifying disconnecting state
    else if ( iCallState == MCCPCallObserver::ECCPStateDisconnecting
             && !iTerminationErrorNotified )
        {
        CheckAndNotifyTerminationError();
        }
    }

// ---------------------------------------------------------------------------
// CSPCall::EmergencyDialRequestFailed
// Handles situations when emergency dial fails
// ---------------------------------------------------------------------------
//
void CSPCall::EmergencyDialRequestFailed( TInt /*aErrorCode*/ )
    {
    CSPLOGSTRING(CSPERROR, "CSPCall::EmergencyDialRequestFailed");

    // Always same error code for CCE
    NotifyErrorOccurred( ECCPEmergencyFailed );

    // Mark that exit code will not be used
    iTerminationErrorNotified = ETrue;
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyErrorOccurred
// Notifies observers about errors
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyErrorOccurred( TCCPError aError )
    {
    CSPLOGSTRING2(CSPERROR, "CSPCall::NotifyErrorOccurred err %d", aError );

    TInt obsCount = iObservers.Count();
    for ( TInt i = 0; i < obsCount; i++ )
        {
        MCCPCallObserver *obs = iObservers[i];
        if ( obs )
            {
            obs->ErrorOccurred( aError, this );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPCall::CallCapsChanged
// Notifies observers about new capabilities.
// ---------------------------------------------------------------------------
//
void CSPCall::CallCapsChanged( const TUint32 aCapsFlags )
    {
    CSPLOGSTRING2(CSPERROR, "CSPCall::CallCapsChanged %b", aCapsFlags );
    iCapsFlags = aCapsFlags;
    TInt obsCount = iObservers.Count();
    for ( TInt i = 0; i < obsCount; i++ )
        {
        MCCPCallObserver *obs = iObservers[i];
        if ( obs )
            {
            obs->CallCapsChanged( aCapsFlags, this );
            }
        }
    }

// ---------------------------------------------------------------------------
// From class MCCPCSCall
// CSPCall::GetMobileCallInfo
// ---------------------------------------------------------------------------
//
TInt CSPCall::GetMobileCallInfo( TDes8& aCallInfo ) const
    {
    CSPLOGSTRING(CSPINT, "CSPCall::GetMobileCallInfo");
    return iCall.GetMobileCallInfo( aCallInfo );
    }

// ---------------------------------------------------------------------------
// From class MCCPCSCall
// CSPCall::GetMobileDataCallCaps
// ---------------------------------------------------------------------------
//
TInt CSPCall::GetMobileDataCallCaps( TDes8& /*aCaps*/ ) const
    {
    CSPLOGSTRING(CSPERROR,
            "CSPCall::GetMobileDataCallCaps ERROR, Not supported");
    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Dial
// ---------------------------------------------------------------------------
//
TInt CSPCall::Dial()
    {
    CSPLOGSTRING( CSPREQIN, "CSPCall::Dial" );
    // Parameters not given, must create empty descriptor.
    TBuf8<1> params;
    return Dial(params);
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::RemoteParty
// ---------------------------------------------------------------------------
//
const TDesC& CSPCall::RemoteParty() const
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::RemoteParty");
    return iRemotePartyNumber;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::RemotePartyName
// ---------------------------------------------------------------------------
//
const TDesC& CSPCall::RemotePartyName()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::RemotePartyName");
    return iRemotePartyName;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::DialledParty
// ---------------------------------------------------------------------------
//
const TDesC& CSPCall::DialledParty() const
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::DialledParty %S", &iRemotePartyNumber);
    return iRemotePartyNumber;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Answer
// ---------------------------------------------------------------------------
//
TInt CSPCall::Answer()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::Answer <");
    TInt ret( KErrNone );

    if ( iCallState == MCCPCallObserver::ECCPStateRinging
         || iCallState == MCCPCallObserver::ECCPStateQueued )
        {
        if( iMobileOriginated )
            {
            ret = KErrGeneral;
            }
        else if( iCallState != MCCPCallObserver::ECCPStateRinging &&
            iCallState != MCCPCallObserver::ECCPStateQueued )
            {
            ret = KErrAccessDenied;
            }
        else
            {
            ret = iCommonInfo.IndicateAnswerRequest( *this );
            }
        }
    else
        {
        // Not correct state for answer
        ret = KErrNotReady;
        }

    CSPLOGSTRING2(CSPREQIN, "CSPCall::Answer > ret %d", ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// From class MCCPCallCommandHandling
// CSPCall::PerformAnswerRequest
// ---------------------------------------------------------------------------
//
TInt CSPCall::PerformAnswerRequest()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::PerformAnswerRequest");

    TInt ret = iRequester->MakeRequest( CSPEtelCallRequester::ERequestTypeAnswer );
    CSPLOGSTRING3(CSPINT,
            "CSPCall::Answer request performed, call state %d ret: %d", iCallState, ret);

    return ret;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Reject
// ---------------------------------------------------------------------------
//
TInt CSPCall::Reject()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::Reject <");
    TInt ret;
    if ( iCallState == MCCPCallObserver::ECCPStateRinging
         || iCallState == MCCPCallObserver::ECCPStateQueued
         || iCallState == MCCPCallObserver::ECCPStateAnswering )
        {
        CSPLOGSTRING( CSPREQIN, "CSPCall::Reject 2" );
        ret = HangUp();
        }
    else
        {
        ret = KErrNotReady;
        }
    CSPLOGSTRING2(CSPREQIN, "CSPCall::Reject > ret: %d", ret );
    return ret;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Queue
// ---------------------------------------------------------------------------
//
TInt CSPCall::Queue()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::Queue");
    TBool callWaitingState;
    iDontReportTerm = ETrue;

    TRAPD( res, iCommonInfo.GetCallWaitingL( *iParams, callWaitingState ) );
    if( res == KErrNone )
        {
        if ( callWaitingState )
            {
            CSPLOGSTRING(CSPREQIN, "CSPCall::Queue Call Waiting On");
            iCallState = MCCPCallObserver::ECCPStateQueued;

            // Notify Queued state
            TInt obsCount = iObservers.Count ( );
            for (TInt i = 0; i < obsCount; i++ )
                {
                MCCPCallObserver *obs = iObservers[i];
                if ( obs )
                    {
                    obs->CallStateChanged ( iCallState, this );
                    }
                }
            CSPLOGSTRING(CSPOBJECT, "CSPCall::Queue ok");

            return KErrNone;
            }
        else
            {
            CSPLOGSTRING(CSPREQIN, "CSPCall::Queue Call Waiting Off");
            }
        }
    else
        {
        CSPLOGSTRING2(CSPERROR,
                      "CSPCall::Queue Error %d with CR. Call Waiting Off",
                      res);
        }

    return KErrNotSupported;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Ringing
// ---------------------------------------------------------------------------
//
TInt CSPCall::Ringing()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::Ringing");
    iDontReportTerm = ETrue;

    // Ringing state is accepted because MT-call goes to ringing
    // state in the beginning based on ETel state maching.
    if ( iCallState == MCCPCallObserver::ECCPStateIdle ||
         iCallState == MCCPCallObserver::ECCPStateRinging )
        {
        NotifyRingingState();
        return KErrNone;
        }
    else if ( iCallState == MCCPCallObserver::ECCPStateAnswering )
        {
        // Do nothing if already in Answering state (autoanswer).
        return KErrNone;
        }

    return KErrNotReady;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::HangUp
// ---------------------------------------------------------------------------
//
TInt CSPCall::HangUp()
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::HangUp this: %x", this);
    TInt ret( KErrNone );

    CSPEtelCallRequester::TRequestType req = iRequester->Request();
    if ( req == CSPEtelCallRequester::ERequestTypeDial ||
         ( req == CSPEtelCallRequester::ERequestTypeDialEmergency ) )
        {
        CSPLOGSTRING(CSPREQIN, "CSPCall::HangUp 1: Cancel ongoing dial");
        ret = iRequester->DialCancel();
        CSPLOGSTRING(CSPREQIN, "CSPCall::HangUp Canceled");
        }
    else if ( req == CSPEtelCallRequester::ERequestTypeHangup )
        {
        CSPLOGSTRING(CSPREQIN, "CSPCall::HangUp 2");
        ret = KErrAlreadyExists;
        }
    else if ( req == CSPEtelCallRequester::ERequestTypeNone
              && iCallState != MCCPCallObserver::ECCPStateIdle )
        {
        CSPLOGSTRING(CSPREQIN, "CSPCall::HangUp 3");
        ret = iRequester->MakeRequest( CSPEtelCallRequester::ERequestTypeHangup );
        iCommonInfo.IndicateActiveHangup( *this );
        }
    else if ( req == CSPEtelCallRequester::ERequestTypeNone )
        {
        CSPLOGSTRING(CSPREQIN, "CSPCall::HangUp 4");
        ret = KErrAlreadyExists;
        }
    else
        {
        CSPLOGSTRING(CSPREQIN, "CSPCall::HangUp 5");
        // Request pending, must be canceled
        iRequester->Cancel();
        ret = iRequester->MakeRequest( CSPEtelCallRequester::ERequestTypeHangup );
        iCommonInfo.IndicateActiveHangup( *this );
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Cancel
// ---------------------------------------------------------------------------
//
TInt CSPCall::Cancel()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::Cancel");
    return HangUp();
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Hold
// ---------------------------------------------------------------------------
//
TInt CSPCall::Hold()
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::Hold this: %x", this);
    TInt ret;

    if ( iCallState == MCCPCallObserver::ECCPStateConnected )
        {
        CSPLOGSTRING(CSPREQOUT, "CSPCall::Hold make request");

        // Set call on hold
        ret = iRequester->MakeRequest(
                    CSPEtelCallRequester::ERequestTypeHold );
        if ( ret != KErrNone )
            {
            CSPLOGSTRING2(CSPERROR,
                "CSPCall::Hold ERROR: %d", ret);
            }
        }
    else if ( iCallState == MCCPCallObserver::ECCPStateHold )
        {
        CSPLOGSTRING(CSPERROR, "CSPCall::Hold : already held");
        ret = KErrAlreadyExists;
        }
    else
        {
        CSPLOGSTRING(CSPERROR, "CSPCall::Hold not allowed");
        ret = KErrNotReady;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Resume
// ---------------------------------------------------------------------------
//
TInt CSPCall::Resume()
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::Resume this: %x", this);
    TInt ret;

    if ( iCallState == MCCPCallObserver::ECCPStateHold )
        {
        CSPLOGSTRING(CSPREQOUT, "CSPCall::Resume request");

        ret = iRequester->MakeRequest(
            CSPEtelCallRequester::ERequestTypeResume );
        if ( KErrNone != ret )
            {
            CSPLOGSTRING2(CSPERROR, "CSPCall::Resume ERROR %d", ret);
            }
        }
    else if ( iCallState == MCCPCallObserver::ECCPStateConnected )
        {
        CSPLOGSTRING(CSPERROR, "CSPCall::Resume already connected" );
        ret = KErrAlreadyExists;
        }
    else
        {
        CSPLOGSTRING(CSPERROR, "CSPCall::Resume not held state" );
        ret = KErrNotReady;
        }

    return ret;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Swap
// ---------------------------------------------------------------------------
//
TInt CSPCall::Swap()
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::Swap this: %x", this);
    TInt ret;
    if ( iCallState == MCCPCallObserver::ECCPStateConnected
         || iCallState == MCCPCallObserver::ECCPStateHold )
        {
        ret = iRequester->MakeRequest( CSPEtelCallRequester::ERequestTypeSwap );
        if ( ret )
            {
            CSPLOGSTRING2( CSPERROR, "CSPCall::Swap Error %d", ret );
            }
        }
    else
        {
        ret = KErrNotReady;
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::IsMobileOriginated
// ---------------------------------------------------------------------------
//
TBool CSPCall::IsMobileOriginated() const
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::IsMobileOriginated");
    return iMobileOriginated;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::State
// ---------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallState CSPCall::State() const
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::State %d", iCallState);
    return iCallState;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Uid
// ---------------------------------------------------------------------------
//
TUid CSPCall::Uid() const
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::Uid %d", KCSPImplementationUid);
    return KCSPImplementationUid;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Caps
// ---------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallControlCaps CSPCall::Caps( ) const
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::Caps %b", iCapsFlags );
    return (MCCPCallObserver::TCCPCallControlCaps) iCapsFlags;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::SetParameters
// ---------------------------------------------------------------------------
//
void CSPCall::SetParameters( const CCCPCallParameters& /*aNewParams*/ )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCall::SetParameters ERROR Can't set params");
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Parameters
// ---------------------------------------------------------------------------
//
const CCCPCallParameters& CSPCall::Parameters() const
    {
    CSPLOGSTRING(CSPINT, "CSPCall::Parameters");
    return *iParams;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::TransferProvider
// ---------------------------------------------------------------------------
//
MCCPTransferProvider* CSPCall::TransferProviderL(const MCCPTransferObserver&
                                                            aObserver )
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::TransferProvider");
    iTransferProvider->AddObserverL( aObserver );
    return iTransferProvider;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::ForwardProvider
// ---------------------------------------------------------------------------
//
MCCPForwardProvider* CSPCall::ForwardProviderL(
        const MCCPForwardObserver& aObserver )
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::ForwardProvider");
    iForwardProvider->AddObserverL( aObserver );
    return iForwardProvider;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::AddObserverL
// ---------------------------------------------------------------------------
//
void CSPCall::AddObserverL( const MCCPCallObserver& aObserver )
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::AddObserverL %x", &aObserver);
    if ( iObservers.Find( &aObserver ) == KErrNotFound )
        {
        iObservers.AppendL( &aObserver );
        }
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// Removes observer.
// ---------------------------------------------------------------------------
//
TInt CSPCall::RemoveObserver( const MCCPCallObserver& aObserver )
    {
    CSPLOGSTRING2(CSPREQIN, "CSPCall::RemoveObserver %x", &aObserver);

    TInt found = iObservers.Find( &aObserver );
    if ( found != KErrNotFound )
        {
        iObservers.Remove( found );
        return KErrNone;
        }

    return found;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::IsCallForwarded
// ---------------------------------------------------------------------------
//
TBool CSPCall::IsCallForwarded( ) const
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::IsCallForwarded <");
    TBool ret( EFalse );
    TInt err( KErrNone );
    RMobileCall::TMobileCallInfoV7Pckg pck( iEtelCallInfo );
    err = GetMobileCallInfo( pck );
    if (err == KErrNone )
        {
        ret = iEtelCallInfo.iForwarded;
        }
    
    CSPLOGSTRING2(CSPREQIN, 
            "CSPCall::IsCallForwarded > forwarder: %d", ret );
    
    return ret;
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::IsSecured
// ---------------------------------------------------------------------------
//
TBool CSPCall::IsSecured( ) const
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::IsSecured");
    return iCommonInfo.NetworkSecurityStatus();
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::SecureSpecified
// ---------------------------------------------------------------------------
//
TBool CSPCall::SecureSpecified( ) const
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::SecureSpecified");
    return iCommonInfo.SecureSpecified();
    }

// ---------------------------------------------------------------------------
// From class MCCPCall
// CSPCall::Tone
// ---------------------------------------------------------------------------
//
TCCPTone CSPCall::Tone() const
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::Tone <");
    TCCPTone tone( ECCPNoSoundSequence );

    if ( iCallState == MCCPCallObserver::ECCPStateConnecting )
        {
        RMmCustomAPI::TRemoteAlertingToneStatus ts = iCommonInfo.GetRemoteAlertingToneStatus();

        if (ts == RMmCustomAPI::EUiRbtTone )
            {
            tone = ECCPRemoteAlerting;
            }
        else if ( ts == RMmCustomAPI::EUiStopTone )
            {
            tone = ECCPNoSoundSequence;
            }
        else
            {
            // No tone
            tone = ECCPNoSoundSequence;
            }
        }
    else
        {
        // Handle disconnecting tones
        TInt callDisconnectingError = ExitCodeError();
        CSPLOGSTRING2(CSPINT, "CSPCall::Tone exit code err: %d", callDisconnectingError);

        switch( callDisconnectingError )
            {
            case KErrNone:
                // GSM: DIAL TONE (optional) - not used in Nokia phones
                CSPLOGSTRING(CSPERROR, "CSPCall::Tone: No sound");
                break;
            case KErrGsmCCUserBusy:
                // GSM: SUBSCRIBER BUSY
                tone = ECCPToneUserBusy;
                CSPLOGSTRING(CSPINT, "CSPCall::Tone: Subscriber Busy");
                break;
                // Fall through
            case KErrGsmCCNumberChanged:
            case KErrGsmCCResponseToStatusEnquiry:
            case KErrGsmCCNormalUnspecified:
                // GSM: NONE, Nokia phones: radio path not available
                tone = ECCPToneRadioPathNotAvailable;
                CSPLOGSTRING(CSPINT, "CSPCall::Tone: RadioPathNotAvailable");
                break;
            case KErrGsmCCNoChannelAvailable:
            case KErrGsmCCTemporaryFailure:
            case KErrGsmCCSwitchingEquipmentCongestion:
            case KErrGsmCCRequestedChannelNotAvailable:
            case KErrGsmCCQualityOfServiceNotAvailable:
            case KErrGsmCCBearerCapabilityNotCurrentlyAvailable:
                // GSM: CONGESTION
                tone = ECCPToneCongestion;
                CSPLOGSTRING(CSPINT, "CSPCall::Tone: Congestion");
                break;
            case KErrGsmCCUnassignedNumber:
            case KErrGsmCCNoRouteToDestination:
            case KErrGsmCCChannelUnacceptable:
            case KErrGsmCCOperatorDeterminedBarring:
            case KErrGsmCCUserNotResponding:
            case KErrGsmCCUserAlertingNoAnswer:
            case KErrGsmCCCallRejected:
            case KErrGsmCCNonSelectedUserClearing:
            case KErrGsmCCDestinationOutOfOrder:
            case KErrGsmCCInvalidNumberFormat:
            case KErrGsmCCFacilityRejected:
            case KErrGsmCCNetworkOutOfOrder:
            case KErrGsmCCAccessInformationDiscarded:
            case KErrGsmCCResourceNotAvailable:
            case KErrGsmCCRequestedFacilityNotSubscribed:
            case KErrGsmCCIncomingCallsBarredInCug:
            case KErrGsmCCBearerCapabilityNotAuthorised:
            case KErrGsmCCServiceNotAvailable:
            case KErrGsmCCBearerServiceNotImplemented:
            case KErrGsmCCChannelTypeNotImplemented:
            case KErrGsmCCAcmGreaterThanAcmMax:
            case KErrGsmCCRequestedFacilityNotImplemented:
            case KErrGsmCCOnlyRestrictedDigitalInformationBCAvailable:
            case KErrGsmCCServiceNotImplemented:
            case KErrGsmCCInvalidCallReferenceValue:
            case KErrGsmCCChannelDoesNotExist:
            case KErrGsmCCSuspendedCallExistsButCallIdentityDoesNotWork:
            case KErrGsmCCCallIdentityInUse:
            case KErrGsmCCNoCallSuspended:
            case KErrGsmCCRequestedCallIdentityAlreadyCleared:
            case KErrGsmCCUserNotInCug:
            case KErrGsmCCIncompatibleDestination:
            case KErrGsmCCInvalidTransitNetworkSelection:
            case KErrGsmCCIncompatibleSegmentedMessage:
            case KErrGsmCCSemanticallyIncorrectMessage:
            case KErrGsmCCInvalidMandatoryInformation:
            case KErrGsmCCNonExistentMessageType:
            case KErrGsmCCIncompatibleMessageInProtocolState:
            case KErrGsmCCNonExistentInformationElement:
            case KErrGsmCCConditionalIEError:
            case KErrGsmCCIncompatibleMessageInCallState:
            case KErrGsmCCRecoveryOnTimerExpiry:
            case KErrGsmCCUnspecifiedProtocolError:
            case KErrGsmCCUnspecifiedInterworkingError:
                // GSM: SPECIAL INFORMATION
                tone = ECCPToneSpecialInformation;
                CSPLOGSTRING(CSPINT, "CSPCall::Tone: Special Information");
                break;
            default:
                CSPLOGSTRING(CSPERROR, "CSPCall::Tone: ERROR not found");
                break;
            }
         }

    CSPLOGSTRING2(CSPREQIN, "CSPCall::Tone > tone: %d", tone);
    return tone;
    }

// ---------------------------------------------------------------------------
// From class MCCPCSCall
// CSPCall::NoFdnCheck
// ---------------------------------------------------------------------------
//
void CSPCall::NoFDNCheck( )
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::NoFDNCheck");
    iFdnCheck = EFalse;
    }

// ---------------------------------------------------------------------------
// CSPCall::LogDialedNumber
// ---------------------------------------------------------------------------
//
TBool CSPCall::LogDialedNumber() const
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::LogDialedNumber true");
    return ETrue;
    }

// ---------------------------------------------------------------------------
// From class MCCPCSCall
// CSPCall::Dial
// ---------------------------------------------------------------------------
//
TInt CSPCall::Dial( const TDesC8& /*aCallParams*/ )
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::Dial");
    return iCommonInfo.IndicateDialRequest( *this );
    }

// ---------------------------------------------------------------------------
// From class MCCPCallCommandHandling
// CSPCall::PerformDialRequest
// ---------------------------------------------------------------------------
//
TInt CSPCall::PerformDialRequest()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::PerformDialRequest");
    iDialCompletionCode = KErrNone;
    iTerminationErrorNotified = EFalse;

    // Send the User to User Information.
    if( iUserToUserInformation->Length() )
        {
        iUUIMessageSender->SendUUIMessage( *iUserToUserInformation );
        }

    return DialFdnCond( iFdnCheck );
    }

// ---------------------------------------------------------------------------
// From class MCSPAudioHandlerObserver
// CSPCall::AudioStartingFailed
// ---------------------------------------------------------------------------
//
void CSPCall::AudioStartingFailed()
    {
    HangUp();
    }

// ---------------------------------------------------------------------------
// CSPCall::CSPCall
// ---------------------------------------------------------------------------
//
CSPCall::CSPCall( RMobileLine& aLine,
                  TBool aMobileOriginated,
                  const TDesC& aName,
                  MCSPCommonInfo& aCommonInfo,
                  TBool aIsEmergencyCall ) :
                    iLine( aLine ),
                    iMobileOriginated( aMobileOriginated ),
                    iName( aName ), 
                    iParams( NULL ),
                    iCallState( MCCPCallObserver::ECCPStateIdle ),
                    iCapsFlags( 0 ),
                    iCallStatusMonitor( NULL ),
                    iCallEventMonitor( NULL ),
                    iCallInfoMonitor( NULL ),
                    iRequester( NULL ),
                    iDialCompletionCode( KErrNone ),
                    iCommonInfo( aCommonInfo ),
                    iTerminationErrorNotified( EFalse ),
                    iIsEmergencyCall( aIsEmergencyCall),
                    iTransferProvider( NULL ),
                    iForwardProvider( NULL ),
                    iAudioHandler( NULL ),
                    iFdnCheck( ETrue ),
                    iCallCapsMonitor( NULL ),
                    iUUIMonitor( NULL ),
                    iUUIMessageSender( NULL ),
                    iSkypeId( NULL ),
                    iAudioStatus( ECSPCallAudioStatusInactive ),
                    iDontReportTerm( EFalse ),
                    iUserToUserInformation( NULL )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCall::CSPCall");
    }

// ---------------------------------------------------------------------------
// CSPCall::ConstructL
// Constructing CSPCall in 2nd phase
// ---------------------------------------------------------------------------
//
void CSPCall::ConstructL( const CCCECallParameters& aParams )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPCall::ConstructL <");

    // Create cloned copy of call parameters
    iParams = static_cast<CCCECallParameters*>( aParams.CloneL() );

    // Open call handle
    OpenCallHandleL();

    // Update call info
    UpdateCallInfo();

    // Update call state from ETel
    UpdateCallState();

    // Create call handlers for call related requests
    // and for monitoring call related events
    CreateCallHandlersL();

    CSPLOGSTRING(CSPOBJECT, "CSPCall::ConstructL >");
    }

// ---------------------------------------------------------------------------
// CSPCall::OpenCallHandleL
// Open call handle, calls either OpenNewCallL or OpenExistingCallL depending
// of the call direction and if the call is a client call
// ---------------------------------------------------------------------------
//
void CSPCall::OpenCallHandleL()
    {
    CSPLOGSTRING(CSPINT, "CSPCall::OpenCallHandleL <");

    if ( iMobileOriginated )
        {
        // There is no existing call on line, open new call
        OpenNewCall();
        }
    else
        {
        // Mobile terminated call on line, open existing call
        OpenExistingCallL( iName );
        }

    CSPLOGSTRING(CSPINT, "CSPCall::OpenCallHandleL >");
    }

// ---------------------------------------------------------------------------
// CSPCall::UpdateCallInfoL
// Update call info including remote party name and number data
// ---------------------------------------------------------------------------
//
void CSPCall::UpdateCallInfo()
    {
    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallInfo <");
    RMobileCall::TMobileCallInfoV7Pckg pck( iEtelCallInfo );
    TInt err = GetMobileCallInfo( pck );
    UpdateCallInfoImpl( iEtelCallInfo );

    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallInfo >");
    }

// ---------------------------------------------------------------------------
// CSPCall::UpdateCallInfoImpl
// Implementation for UpdateCallInfo().
// ---------------------------------------------------------------------------
//
void CSPCall::UpdateCallInfoImpl( const RMobileCall::TMobileCallInfoV7& aCallInfo )
    {
    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallInfoImpl <");

    if ( iMobileOriginated )
        {
        // Call wasn't added by ETel monitor, update info accordingly
        UpdateCallNameNumberInfo( aCallInfo, EFalse );
        }
    else
        {
        // Call was added by ETel monitor, update info accordingly
        UpdateCallNameNumberInfo( aCallInfo, ETrue );
        }


    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallInfoImpl >");
    }

// ---------------------------------------------------------------------------
// CSPCall::UpdateCallOrigin
// Set call origin to CCCECallParameters.
// ---------------------------------------------------------------------------
//
void CSPCall::UpdateCallOrigin( const RMobileCall::TMobileCallInfoV7& aCallInfo )
    {
    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallOrigin <");

    if ( RMobileCall::EOriginatorSIM == aCallInfo.iCallParamOrigin )
        {
        iParams->SetOrigin(CCCECallParameters::ECCECallOriginSAT);
        iParams->SetAlphaId(aCallInfo.iAlphaId);
        }

    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallOrigin >");
    }



// ---------------------------------------------------------------------------
// CSPCall::UpdateCallStateL
//
// ---------------------------------------------------------------------------
//
int CSPCall::UpdateCallState()
    {
    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallState <");
    TInt err( KErrNone );

    // Update call state from ETel
    RMobileCall::TMobileCallStatus etelState;
    err = iCall.GetMobileCallStatus( etelState );
    if ( KErrNone == err )
        {
        iCallState = CCPStateFromETelState( etelState );
        }
    CSPLOGSTRING2(CSPINT, "CSPCall::UpdateCallState > res %d", err);

    return err;
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyRemotePartyNumberChanged
//
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyRemotePartyNumberChanged()
    {
    CSPLOGSTRING(CSPINT, "CSPCall::NotifyRemotePartyNumberChanged <");
    // If COLP number is different from original dialled number
    // it is available in connected state of a MO call.
    // TSY does not send notification so number must be fetched.
    if ( IsMobileOriginated() )
        {
        RMobileCall::TMobileCallInfoV7Pckg callInfoPckg( iEtelCallInfo );
        GetMobileCallInfo( callInfoPckg );
        if ( iEtelCallInfo.iRemoteParty.iRemoteNumber.iTelNumber.Length() )
            {
            NotifyRemotePartyInfoChanged( KNullDesC(),
                iEtelCallInfo.iRemoteParty.iRemoteNumber.iTelNumber);        
            }
        }

    }


// ---------------------------------------------------------------------------
// CSPCall::CreateCallHandlersL
// Create call handlers for call related requests and call monitoring
// ---------------------------------------------------------------------------
//
void CSPCall::CreateCallHandlersL()
    {
    CSPLOGSTRING(CSPINT, "CSPCall::CreateCallHandlersL <");

    iRequester = CSPEtelCallRequester::NewL( *this, iCall );

    iCallEventMonitor = CSPEtelCallEventMonitor::NewL( *this, iCall );
    iCallEventMonitor->StartMonitoring();
    iCallStatusMonitor = CSPEtelCallStatusMonitor::NewL( *this, iCall );
    iCallStatusMonitor->StartMonitoring();

    iCallCapsMonitor = CSPEtelCallCapsMonitor::NewL( *this, iCall );
    iCallCapsMonitor->StartMonitoring();
    iCapsFlags = iCallCapsMonitor->FetchCallControlCapsL();

    iCallInfoMonitor = CSPCallInfoMonitor::NewL( *this, iCall );
    iCallInfoMonitor->StartMonitoring();

    // Start UUI monitor and create message sender
    iSkypeId = TCSPSkypeIdParser::CreateSkypeIdBufferL();
    iUUIMonitor = CSPUUIMonitor::NewL( iCall,*this );
    iUUIMonitor->StartMonitor();
    iUUIMessageSender = CSPUUIMessageSender::NewL( iCall );
    iUserToUserInformation = iParams->UUSId().AllocL();

    // Transfer provider
    iTransferProvider = CSPTransferProvider::NewL( iCall, *this, iCommonInfo );

    // Forward provider
    iForwardProvider = CSPForwardProvider::NewL();

    CSPLOGSTRING(CSPINT, "CSPCall::CreateCallHandlersL >");
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyRingingState
// Gets to ringing state
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyRingingState( )
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::NotifyRingingState");
    iCallState = MCCPCallObserver::ECCPStateRinging;

    TInt obsCount = iObservers.Count ( );
    for (TInt i = 0; i < obsCount; i++ )
        {
        MCCPCallObserver *obs = iObservers[i];
        if ( obs )
            {
            obs->CallStateChanged ( iCallState, this );
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPCall::CCPStateFromETelState
// Simple mapping between state enumerations.
// ---------------------------------------------------------------------------
//
MCCPCallObserver::TCCPCallState CSPCall::CCPStateFromETelState(
    RMobileCall::TMobileCallStatus aEtelState )
    {
    MCCPCallObserver::TCCPCallState ret = MCCPCallObserver::ECCPStateIdle;
    switch ( aEtelState )
        {
        case RMobileCall::EStatusUnknown:
            {
            CSPLOGSTRING( CSPREQIN,
                "CSPCall::State > RMobileCall::Unknown" );
            ret = MCCPCallObserver::ECCPStateIdle;
            break;
            }
        case RMobileCall::EStatusIdle:
            {
            CSPLOGSTRING( CSPREQIN,
                "CSPCall::State > RMobileCall::EStatusIdle" );
            ret = MCCPCallObserver::ECCPStateIdle;
            break;
            }
        case RMobileCall::EStatusRinging:
            {
            CSPLOGSTRING( CSPREQIN,
                "CSPCall::State > RMobileCall::EStatusRinging" );
            ret = MCCPCallObserver::ECCPStateRinging;
            break;
            }
        // Map the following to connecting status
        case RMobileCall::EStatusDialling:
            {
            CSPLOGSTRING( CSPREQIN, "CSPCall::State > \
                RMobileCall::EStatusDialling" );
            ret = MCCPCallObserver::ECCPStateDialling;
            break;
            }
        case RMobileCall::EStatusConnecting:
            {
            CSPLOGSTRING( CSPREQIN, "CSPCall::State > \
                RMobileCall::Connecting" );
            ret = MCCPCallObserver::ECCPStateConnecting;
            break;
            }
        case RMobileCall::EStatusAnswering:
            {
            CSPLOGSTRING( CSPREQIN, "CSPCall::State > \
                RMobileCall::Answering" );
            ret = MCCPCallObserver::ECCPStateAnswering;
            break;
            }
        // Call is connected and active.
        case RMobileCall::EStatusConnected:
            {
            CSPLOGSTRING( CSPREQIN,
            "CSPCall::State > RMobileCall::EStatusConnected" );
            ret = MCCPCallObserver::ECCPStateConnected;
            break;
            }
        case RMobileCall::EStatusDisconnectingWithInband:
        case RMobileCall::EStatusDisconnecting:
            {
            CSPLOGSTRING( CSPREQIN,
                 "CSPCall::State > RMobileCall::EStatusDisconnecting/Inband?");
            ret = MCCPCallObserver::ECCPStateDisconnecting;
            break;
            }
        case RMobileCall::EStatusReconnectPending:
            {
            CSPLOGSTRING( CSPREQIN,
            "CSPCall::State > RMobileCall::EStatusHangingUp" );
            ret = MCCPCallObserver::ECCPStateDisconnecting;
            break;
            }
        case RMobileCall::EStatusHold:
            {
            CSPLOGSTRING( CSPREQIN,
            "CSPCall::State > RMobileCall::EStatusHold" );
            ret = MCCPCallObserver::ECCPStateHold;
            break;
            }
        case RMobileCall::EStatusWaitingAlternatingCallSwitch:
            {
            // This state is not used in real life.
            CSPLOGSTRING( CSPREQIN,
            "CSPCall::State > RMobileCall::EStatusWaitingAlternatingCallSwitch" );
            ret = MCCPCallObserver::ECCPStateConnected;
            break;
            }
        case RMobileCall::EStatusTransferring:
            {
            CSPLOGSTRING( CSPREQIN,
            "CSPCall::State > RMobileCall::EStatusHold" );
            ret = MCCPCallObserver::ECCPStateTransferring;
            break;
            }
        case RMobileCall::EStatusTransferAlerting:
            {
            CSPLOGSTRING( CSPREQIN,
            "CSPCall::State > RMobileCall::EStatusHold" );
            ret = MCCPCallObserver::ECCPStateTransferring;
            break;
            }
        default:
            {
            CSPLOGSTRING( CSPREQIN,
                "CSPCall::CCPStateFromETelState unhandled state ");
            }
        }

    CSPLOGSTRING3(CSPINT,
        "CSPCall::CCPStateFromETelState ETel: %d CCP: %d", aEtelState, ret);
    return ret;
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyCallStateChanged
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyCallStateChanged( MCCPCallObserver::TCCPCallState aState )
    {
    CSPLOGSTRING3(CSPINT,
            "CSPCall::NotifyCallStateChanged < state: %d this: %x",
            aState, this );

    iCallState = aState;
    TBool notifyObservers( ETrue );

    if ( !iMobileOriginated && MCCPCallObserver::ECCPStateRinging == aState )
        {
        CSPLOGSTRING2(CSPINT, "CSPCall::No notify on ringing MT", aState);
        notifyObservers = EFalse;
        }

    if ( notifyObservers )
        {
        TInt obsCount = iObservers.Count();
        for ( TInt i = 0; i < obsCount; i++ )
            {
            MCCPCallObserver *obs = iObservers[i];
            if ( obs )
                {
                obs->CallStateChanged( aState, this );
                }
            }
        }

    CSPLOGSTRING3(CSPINT,
            "CSPCall::NotifyCallStateChanged > state: %d this: %x",
            aState, this );
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyCallStateChangedWithInband
// Notifies observers about state changes
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyCallStateChangedWithInband(
        MCCPCallObserver::TCCPCallState aState )
    {
    CSPLOGSTRING3(CSPINT,
        "CSPCall::NotifyCallStateChangedWithInBand < state: %d this: %x",
        aState, this );

    iCallState = aState;

    if ( !iMobileOriginated && MCCPCallObserver::ECCPStateRinging == aState )
        {
        CSPLOGSTRING2(CSPINT, "CSPCall::No notify on ringing MT", aState);
        }
    else
        {
        TInt obsCount = iObservers.Count();
        for ( TInt i = 0; i < obsCount; i++ )
            {
            MCCPCallObserver *obs = iObservers[i];
            if ( obs )
                {
                obs->CallStateChangedWithInband( aState, this );
                }
            }
        }
    CSPLOGSTRING3(CSPINT,
            "CSPCall::NotifyCallStateChangedWithInband > state: %d this: %x",
            aState, this);
    }

// ---------------------------------------------------------------------------
// CSPCall::ReadRepositoryL
// Reads the given information from Central Repository.
// ---------------------------------------------------------------------------
//
void CSPCall::ReadRepositoryL( TUid aUid, TUint aKey, TInt& aVal)
    {
    CSPLOGSTRING(CSPINT, "CSPCall::ReadRepositoryL");

    CRepository* cr = CRepository::NewL( aUid );
    TInt err = cr->Get( aKey, aVal );
    delete cr;
    User::LeaveIfError( err );
    }

// ---------------------------------------------------------------------------
// CSPCall::CheckAndNotifyTerminationError
// Reads error code from ETel.
// ---------------------------------------------------------------------------
//
void CSPCall::CheckAndNotifyTerminationError()
    {
    CSPLOGSTRING2(CSPINT,
        "CSPCall::CheckAndNotifyTerminationError dial completion code: %d",
        iDialCompletionCode);

    // Emergency error handling is separated into CSPEtelCallRequester
    if ( !iIsEmergencyCall )
        {
        TInt termErr = ExitCodeError();
        CSPLOGSTRING2(CSPINT,
                "CSPCall::CheckAndNotifyTerminationError exit code error: %d",
                termErr);

        if ( termErr == KErrNone )
            {
            // Handle KErrGsmCCUnassignedNumber correctly
            // because the value is not stored in exit code.
            termErr = iDialCompletionCode;
            CSPLOGSTRING(CSPINT,
                    "CSPCall::CheckAndNotifyTerminationError use dial completion code");
            }

        if ( termErr == KErrGsmRRPreEmptiveRelease )
            {
            // Error KErrGsmRRPreEmptiveRelease occurs when there is active call
            // and user make emergency call, can be ignored.
            CSPLOGSTRING3(
                CSPERROR,
                "CSPCall::CheckAndNotifyTerminationError preemptive release, ignore: %d",
                termErr,
                this );
            }
        else if ( iDontReportTerm
                && (  termErr == KErrGsmCCNormalUnspecified
                   || termErr == KErrGsmCCCallRejected ) )
            {
            // Not an error, since this happens on normal
            // call termination situation after connected call.
            CSPLOGSTRING3(CSPERROR,
                    "CSPCall::CheckAndNotifyTerminationError DISCARD this: %x err: %d",
                    this,
                    termErr);
            }
        else if ( termErr )
            {
            TCCPError ccpErr(ECCPErrorNone);

            // Only a couple of error codes can have diagnostic information.
            // aDiagnostic ought to be undefined in other situatios,
            // but at least in this way we can really guarantee it.
            if ( termErr == KErrGsmCCFacilityRejected ||
                 termErr == KErrGsmCCRequestedFacilityNotSubscribed ||
                 termErr == KErrGsmCCIncomingCallsBarredInCug ||
                 termErr == KErrGsmCCRequestedFacilityNotImplemented ||
                 termErr == KErrGsmCCUserNotInCug )
                {
                TName name;
                CallName( name );
                TInt diagErr = iCommonInfo.GetDiagnosticError( name );
                ccpErr = iRequester->MapError( diagErr );

                // Finally use exit code if diagnostic did not contain
                // any useful information.
                if ( ccpErr == ECCPRequestFailure )
                    {
                    ccpErr = iRequester->MapError( termErr );
                    }
                }
            else
                {
                ccpErr = iRequester->MapError( termErr );
                }

            NotifyErrorOccurred( ccpErr );
            iTerminationErrorNotified = ETrue;
            }
        }
    }

// ---------------------------------------------------------------------------
// CSPCall::ExitCodeError
// Reads error code from ETel call info's iExitCode.
// ---------------------------------------------------------------------------
//
TInt CSPCall::ExitCodeError() const
    {
    CSPLOGSTRING2(CSPINT, "CSPCall::ExitCodeError < this: %x",
                    this );
    TInt callError;
    RMobileCall::TMobileCallInfoV7Pckg pck( iEtelCallInfo );
    TInt getErr = GetMobileCallInfo( pck );
    // Is there value in higher 16 bits
    if ( KErrNone == getErr && (iEtelCallInfo.iExitCode & 0xFFFF0000) ) 
        {
        CSPLOGSTRING2(CSPINT, "CSPCall::ExitCodeError callInfo.iExitCode %d", 
            iEtelCallInfo.iExitCode );
        callError = ( iEtelCallInfo.iExitCode >> KTimesToSplitValue ); 
        
        if ( callError > KErrEtelGsmBase ) 
            // Not in valid exteded range
            {
            // Ignore invalid extented code
            if ( ( iEtelCallInfo.iExitCode & 0x0000FFFF ) == KErrNone ) 
                // core error is zero
                {
                callError = KErrNone;
                }
            else
                {
                callError = ( ( iEtelCallInfo.iExitCode & 0x0000FFFF ) 
                    | 0xFFFF0000 );
                }
            }
        }
    // Higher and lower 16 bits are zero
    else if ( !( iEtelCallInfo.iExitCode & 0xFFFFFFFF ) ) 
        {
        callError = KErrNone;
        }
    else
        {
        // No extended error, expand value to full range
        callError = ( iEtelCallInfo.iExitCode | 0xFFFF0000 );
        }

    CSPLOGSTRING2(CSPINT, "CSPCall::ExitCodeError > err: %d", callError);

    return callError;
    }

// ---------------------------------------------------------------------------
// CSPCall::UUSMessageReceived
// ---------------------------------------------------------------------------
//
 void CSPCall::UUSMessageReceived( const TDesC& aMessage )
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::UUSMessageReceived");
    TCSPSkypeIdParser parser;

    TPtr ptr = iSkypeId->Des();
    ptr.Zero();
    const TInt error( parser.Parse( aMessage, ptr ) );

    if ( !error )
        {
        iRemotePartyName = ptr;
        NotifyCallEventOccurred( MCCPCallObserver::ECCPNotifyRemotePartyInfoChange );
        }
    CSPLOGSTRING2(CSPINT, "CSPCall::UUSMessageReceived  err: %d",  error );
    }

// ---------------------------------------------------------------------------
// CSPCall::NotifyRemotePartyInfoChanged
// ---------------------------------------------------------------------------
//
void CSPCall::NotifyRemotePartyInfoChanged( const TDesC& aRemotePartyName,
                                            const TDesC& aRemotePartyNumber )
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::NotifyRemotePartyInfoChanged");
    if ( aRemotePartyName.Length() )
        {
        iRemotePartyName = aRemotePartyName;
        }

    if ( aRemotePartyNumber.Length() )
        {
        iRemotePartyNumber = aRemotePartyNumber;
        }

    NotifyCallEventOccurred( MCCPCallObserver::ECCPNotifyRemotePartyInfoChange );
    }

// ---------------------------------------------------------------------------
// CSPCall::DontReportTerminationError
// ---------------------------------------------------------------------------
//
void CSPCall::DontReportTerminationError()
    {
    CSPLOGSTRING(CSPREQIN, "CSPCall::DontReportTerminationError");
    iDontReportTerm = ETrue;
    }

// ---------------------------------------------------------------------------
// CSPCall::UpdateCallNameNumberInfo
// Update call info including remote party name and number data  
// ---------------------------------------------------------------------------
//    
void CSPCall::UpdateCallNameNumberInfo(
        const RMobileCall::TMobileCallInfoV3& aCallInfo, 
        TBool aCallAddedByMonitor )
    {
    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallNameNumberInfo <");

    iCallName.Zero();
    iRemotePartyNumber.Zero();
    iRemotePartyName.Zero();

    if ( aCallAddedByMonitor )
        {
        CSPLOGSTRING(CSPINT,
                "CSPCall::UpdateCallNameNumberInfo call added by monitor");

        // If call was created by ETel line status or incoming call monitor
        // the call name is already known, only remote party info is fetched from
        // call info
        iCallName.Append( iName );
        iRemotePartyName.Append( aCallInfo.iRemoteParty.iCallingName );
        iRemotePartyNumber.Append( aCallInfo.iRemoteParty.iRemoteNumber.iTelNumber );
        }
    else
        {
        CSPLOGSTRING(CSPINT,
                "CSPCall::UpdateCallNameNumberInfo call added by plugin owner");

        // The call was not created by monitor and the remote number is already known,
        // fetch only call name from call info
        iCallName.Append( aCallInfo.iCallName );
        iRemotePartyNumber.Append( iName );
        }

    CSPLOGSTRING2(CSPINT,
            "CSPCall::UpdateCallNameNumberInfo iCallName: %S", &iCallName );
    CSPLOGSTRING2(CSPINT,
            "CSPCall::UpdateCallNameNumberInfo iRemotePartyNumber: %S", &iRemotePartyNumber );
    CSPLOGSTRING2(CSPINT,
            "CSPCall::UpdateCallNameNumberInfo iRemotePartyName: %S", &iRemotePartyName );

    CSPLOGSTRING(CSPINT, "CSPCall::UpdateCallNameNumberInfo >");
    }

// ---------------------------------------------------------------------------
// CSPCall::OpenNewCall
// Open new call
// ---------------------------------------------------------------------------
//
void CSPCall::OpenNewCall()
    {
    CSPLOGSTRING(CSPINT, "CSPCall::OpenNewCall <");

    // Open new call
    __ASSERT_ALWAYS( iCall.OpenNewCall( iLine ) == KErrNone,
            Panic( ECSPPanicNoEtel ) );

    CSPLOGSTRING(CSPINT, "CSPCall::OpenNewCall >");
    }

// ---------------------------------------------------------------------------
// CSPCall::OpenExistingCallL
// Open existing call
// ---------------------------------------------------------------------------
//
void CSPCall::OpenExistingCallL( const TDesC& aName )
    {
    CSPLOGSTRING2(CSPINT,
            "CSPCall::OpenExistingCallL < aName: %S", &aName);

    // Open existing call with given name from current line
    TInt err = iCall.OpenExistingCall( iLine, aName );

    if ( KErrNone != err  )
        {
        CSPLOGSTRING2(CSPERROR,
            "CSPCall::OpenCallHandleL OpenExistingCall error: %d, leaving", err);
        User::Leave( err );
        }

    CSPLOGSTRING(CSPINT, "CSPCall::OpenExistingCallL >");
    }

// End of File

