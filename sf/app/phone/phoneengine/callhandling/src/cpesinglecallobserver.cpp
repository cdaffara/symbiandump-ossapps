/*
* Copyright (c) 2003-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPESingleCallObserver class 
*                member functions.
*
*/


// INCLUDE FILES
#include <talogger.h>
#include <pepanic.pan>

#include "cpesinglecallobserver.h"
#include "mpemessagesender.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPESingleCallObserver::CPESingleCallObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPESingleCallObserver::CPESingleCallObserver( MPEMessageSender& aOwner )
    : CPECall( aOwner ),
    iDialledNumber( KNullDesC )
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCallObserver::CPESingleCallObserver start." );
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCallObserver::CPESingleCallObserver complete." );
    }

// Destructor
CPESingleCallObserver::~CPESingleCallObserver()
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCallObserver::~CPESingleCallObserver: Start." );
    if( iCall )
        {
        TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCallObserver::~CPESingleCallObserver: Release" );
        iCall->Release();
        }
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCallObserver::~CPESingleCallObserver: Complete." );
    }

// -----------------------------------------------------------------------------
// CPESingleCallObserver::ErrorOccurred
// Asynchronous call errors are notified using this interface. 
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESingleCallObserver::ErrorOccurred( const TCCPError aError )
    {
    TInt error = static_cast < TInt > ( aError );
    TEFLOGSTRING2( KTAERROR, "CALL CPESINGLECALLOBSERVER::ERROROCCURRED, ! ERROR CODE: %d !", error );
    // Currently completes also with KErrNone
    // Need to prevent forwarding in no-error situation
    if( ECCPErrorBusy == aError )
        {
        iErrorCode = aError;
        CPECall::SendMessage( MEngineMonitor::EPEMessageRemoteBusy );
        }
    else if( ECCPErrorRejected == aError )
        {
        // Don't send error message.
        SetMissedCall( EFalse );
        }
    else if( aError != KErrNone )
        {
        CPECall::SendErrorMessage( error );
        }
    }

// -----------------------------------------------------------------------------
// CPESingleCallObserver::CallStateChanged
// Notifies observer the change in call state. The observer needs to verify
// event correctness.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESingleCallObserver::CallStateChanged( const CCPCall::TCallState aState )
    {
    TEFLOGSTRING2( KTAREQIN, "CALL CPESingleCallObserver::CallStateChanged, Call state: %d", aState );

    switch ( aState )
        {
        case CCPCall::EStateIdle:
            {
            SetCallState( EPEStateIdle );
            SendMessage( MEngineMonitor::EPEMessageIdle );
            break;
            }
        case CCPCall::EStateRinging:
            {
            SetCallState( EPEStateRinging );
            SendMessage( MEngineMonitor::EPEMessageIncoming );
            SetMissedCall( ETrue );
            break;
            }
        case CCPCall::EStateConnecting:
            {
            SetCallState( EPEStateConnecting );
            SendMessage( MEngineMonitor::EPEMessageConnecting );
            break;
            }
        case CCPCall::EStateConnected:
            {
            SetCallState( EPEStateConnected );
            SendMessage( MEngineMonitor::EPEMessageConnected );
            SetMissedCall( EFalse );
            break;
            }
        case CCPCall::EStateDisconnecting:
            {
            SetCallState( EPEStateDisconnecting );
            SendMessage( MEngineMonitor::EPEMessageDisconnecting );
            break;
            }
        case CCPCall::EStateHold:
            {
            SetCallState( EPEStateHeld );
            SendMessage( MEngineMonitor::EPEMessageHeld );
            break;  
            }
        case CCPCall::EStateAnswering:
            {
            SetCallState( EPEStateAnswering );
            SendMessage( MEngineMonitor::EPEMessageAnswering );
            break;
            }
        case CCPCall::EStateDialling:
            {
            SetCallState( EPEStateDialing );
            SendMessage( MEngineMonitor::EPEMessageDialing );
            break;
            }
        case CCPCall::EStateQueued:
            {
            SetCallState( EPEStateRinging );
            SendMessage( MEngineMonitor::EPEMessageIncoming );
            SetMissedCall( ETrue );
            break;
            }

        case CCPCall::EStateTransferring:

            {
            // Not supported -> Ignored
            TEFLOGSTRING( KTAERROR, "CALL CPESingleCallObserver::CALLSTATECHANGED ! STATE IGNORED !" );
            break;
            }
        default:
            {
            break;
            }
        }
    }
// -----------------------------------------------------------------------------
// CPESingleCallObserver::CallStateChangedWithInband
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESingleCallObserver::CallStateChangedWithInband( 
        const CCPCall::TCallState aState )
    {
    TEFLOGSTRING2( 
        KTAREQIN, 
        "CALL CPESingleCallObserver::CallStateChangedWithInband, Call state: %d", 
        aState );

    switch ( aState )
        {
        case CCPCall::EStateConnecting:
            {
            SetCallState( EPEStateConnecting );
            SendMessage( MEngineMonitor::EPEMessageConnectingWithInband );
            break;
            }
        case CCPCall::EStateDisconnecting:
            {
            SetCallState( EPEStateDisconnecting );
            SendMessage( MEngineMonitor::EPEMessageDisconnectingWithInband );
            break;
            }
        default:
            {
            break;
            }
        }
    }
        
// -----------------------------------------------------------------------------
// CPESingleCallObserver::CallEventOccurred
// An event for the call has occurred. The observer needs to verify
// event correctness.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESingleCallObserver::CallEventOccurred( const MCCECallObserver::TCCECallEvent aEvent )
    {
    TEFLOGSTRING2( KTAREQIN, "CALL CPESingleCallObserver::CallEventOccurred, Call event: %d", aEvent );

    switch ( aEvent )
        {
        case MCCECallObserver::ERemoteTerminated:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Remotely terminated" );
            SendMessage( MEngineMonitor::EPEMessageRemoteTerminated );
            break;
            }
        case MCCECallObserver::ESecureSession: // fall through
        case MCCECallObserver::ENotSecureSession: // fall through
        case MCCECallObserver::EVoIPEventNotSecureSessionWithSips:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Secure information changed" );
            SendMessage( MEngineMonitor::EPEMessageCallSecureStatusChanged );
            break;
            }
        case MCCECallObserver::ECCESecureNotSpecified: 
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Secure not specified" );
            SendMessage( MEngineMonitor::EPEMessageSecureNotSpecified );
            break;
            }            
        case MCCECallObserver::ELocalHold:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Locally held, IGNORED" );
            break;
            }
        case MCCECallObserver::ELocalResume:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Locally resumed, IGNORED" );
            break;
            }
        case MCCECallObserver::ERemoteHold:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Remotely held" );
            SendMessage( MEngineMonitor::EPEMessageRemoteHeld );
            break;
            }
        case MCCECallObserver::ERemoteResume:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Remotely resumed" );
            SendMessage( MEngineMonitor::EPEMessageRemoteResumed );
            break;
            }
        case MCCECallObserver::ERemoteConferenceCreate:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Remotely created conference" );
            SendMessage( MEngineMonitor::EPEMessageRemoteCreatedConference );
            break;
            }
        case MCCECallObserver::ELocalTransfer:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::ELocalTransfer" );
            SendMessage( MEngineMonitor::EPEMessageTransferDone );
            break;
            }
        case MCCECallObserver::ERemoteTransferring:
        case MCCECallObserver::ERemoteTransferAlerting:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::ERemoteTransferring or ERemoteTransferAlerting" );
            SendMessage( MEngineMonitor::EPEMessageTransferring );
            break;
            }
        case MCCECallObserver::ECCENotifyRemotePartyInfoChange:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::ECCENotifyRemotePartyInfoChange" );
            SendMessage( MEngineMonitor::EPEMessageRemotePartyInfoChanged );
            break;
            }
        
        // Indicates that network or call provider has started to play
        // inband tone so our tone playing must be stopped.
        case MCCECallObserver::EVoIPEventEarlyMediaStarted:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::EVoIPEventEarlyMediaStarted" );
            iOwner.HandleInternalMessage( MPEPhoneModel::EPEMessageStopTonePlay );
            break;
            }
            
        case MCCECallObserver::ERemoteForwarding:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::EPEMessageRemoteForwarding" );
            SendMessage( MEngineMonitor::EPEMessageRemoteForwarding );
            break;
            }
        case MCCECallObserver::EVoIPEventMovedPermanently:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::EMovedPermanently" );
            SendMessage( MEngineMonitor::EPEMessageMovedPermanently );
            break;
            }
        case MCCECallObserver::EVoIPEventMultipleChoices:
            {
            TEFLOGSTRING( KTAINT, "CALL CPESingleCallObserver::EMovedPermanently" );
            SendMessage( MEngineMonitor::EPEMessageMultipleChoices );
            break;
            }             
        
        case MCCECallObserver::ELocalDeflectCall:
        case MCCECallObserver::ERemoteBarred:
        case MCCECallObserver::ERemoteWaiting:
        case MCCECallObserver::ELocalBarred:
        default:
            {
            TEFLOGSTRING2( KTAINT, "CALL CPESingleCallObserver::CallEventOccurred, Unspecified/protocol specific call event: %d", aEvent );
            break;
            }
        }
    } 

// -----------------------------------------------------------------------------
// CPESingleCallObserver::CallDurationChanged
// Notifies observer the change in call duration.
// -----------------------------------------------------------------------------
//    
EXPORT_C void CPESingleCallObserver::CallDurationChanged( const TTimeIntervalSeconds aDuration )
    {
    TEFLOGSTRING( KTAREQIN, "CALL CPESingleCallObserver::CallDurationChanged" );
    iDuration = aDuration;
    SendMessage( MEngineMonitor::EPEMessageChangedCallDuration );
    }

// -----------------------------------------------------------------------------
// CPESingleCallObserver::CallCapsChanged
// Notifies observer about the changes in call control caps.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESingleCallObserver::CallCapsChanged( const MCCECallObserver::TCCECallControlCaps aCaps )
    {
    TEFLOGSTRING( KTAREQIN, "CALL CPESingleCallObserver::CallCapsChanged" );
    iControlCaps = aCaps;
    SendMessage( MEngineMonitor::EPEMessageCallControlCapsChanged );
    }

// -----------------------------------------------------------------------------
// CPESingleCallObserver::HandleTransfer
// Notifies observer about transfer made by remote end
// -----------------------------------------------------------------------------
//
EXPORT_C void CPESingleCallObserver::HandleTransfer( const TBool aAttended, 
                                 const TDesC& aDestination )
    {
    TEFLOGSTRING3( KTAREQIN, "CALL CPESingleCallObserver::HandleTransfer: Remote transfer request, attended: %d, destination: %S", aAttended, &aDestination );
    TEFLOGSTRING( KTAREQIN, "CALL CPESingleCallObserver::HandleTransfer: Currently not supported" );
    if ( KNullDesC() != aDestination && EFalse == aAttended && 
            aDestination.Length() <= iTransferTarget.MaxLength() )
        {
        iTransferTarget = aDestination;
        SendMessage( MEngineMonitor::EPEMessageUnattendedTransferRequest );
        }
    else
        {
        iCall->AcceptTransfer( EFalse );
        }
    }

// -----------------------------------------------------------------------------
// CPESingleCallObserver::GetCallDuration
// Method returns the duration of the current call.
// -----------------------------------------------------------------------------
//
void CPESingleCallObserver::GetCallDuration(
        TTimeIntervalSeconds& aDuration )
    {
    TEFLOGSTRING2( 
        KTAINT, 
        "CALL CPESingleCallObserver::GetCallDuration, iDuration: %d", 
        iDuration.Int() );
    aDuration = iDuration;
    }

// -----------------------------------------------------------------------------
// CPESingleCallObserver::GetCallControlCaps
// Method returns call control caps
// -----------------------------------------------------------------------------
//
void CPESingleCallObserver::GetCallControlCaps(
        MCCECallObserver::TCCECallControlCaps& aCallControlCaps )  const
    {
    TEFLOGSTRING2( KTAINT, "CALL CPESingleCallObserver::GetCallControlCaps, iControlCaps: %d", iControlCaps );
    aCallControlCaps = iControlCaps;
    }

// -----------------------------------------------------------------------------
// CPESingleCallObserver::Call
// Returns reference to MCCECall handle
// -----------------------------------------------------------------------------
//
MCCECall& CPESingleCallObserver::Call()
    {
    TEFLOGSTRING2( KTAINT, "CALL CPESingleCallObserver::Call, iCall: %d", iCall );
    return *iCall;
    }
    

// -----------------------------------------------------------------------------
// CPESingleCallObserver::SetCall
// 
// -----------------------------------------------------------------------------
//
void CPESingleCallObserver::SetCall( MCCECall& aCall )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPESingleCallObserver::SetCall, aCall: %d", &aCall );
    iCall = &aCall;
    iCall->SetObserver( *this );
    iControlCaps = iCall->Caps();
    TEFLOGSTRING2( KTAINT, "CALL CPESingleCallObserver::SetCall, iControlCaps: %b", iControlCaps );
    }


// -----------------------------------------------------------------------------
// CPESingleCallObserver::UnattendedTransferTarget
// -----------------------------------------------------------------------------
//
const TPEPhoneNumber& CPESingleCallObserver::UnattendedTransferTarget() const
    {
    TEFLOGSTRING2( KTAREQIN, 
        "CALL CPESingleCallObserver::UnattendedTransferTarget: %S",
        &iTransferTarget );
    
    return iTransferTarget;
    }


// -----------------------------------------------------------------------------
// CPESingleCallObserver::ForwardAddressChoices
// -----------------------------------------------------------------------------
//
const CDesC8Array* CPESingleCallObserver::ForwardAddressChoices() const
    {
    const CDesC8Array* array = NULL;
    TRAP_IGNORE( array = &iCall->GetForwardAddressChoicesL() );

    return array;
    }

// End of File 
