/*
* Copyright (c) 2002-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPEConferenceCall  class 
*                member functions.
*
*/


// INCLUDE FILES
#include "cpeconferencecall.h"
#include "mpemessagesender.h"
#include <pevirtualengine.h>
#include <talogger.h>
#include <mccpconferencecallobserver.h>
#include <pepanic.pan>

// EXTERNAL DATA STRUCTURES
// None.

// EXTERNAL FUNCTION PROTOTYPES  
// None.

// CONSTANTS
// None.

// MACROS
// None.

// LOCAL CONSTANTS AND MACROS
// None.

// MODULE DATA STRUCTURES
// None.

// LOCAL FUNCTION PROTOTYPES
// None.

// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================================

// -----------------------------------------------------------------------------
// CPEConferenceCall::CPEConferenceCall
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEConferenceCall::CPEConferenceCall( 
        MPEMessageSender& aOwner,
        MCCEConferenceCall& aCall
        ) : CPECall( aOwner ), iConferenceCall( &aCall )
    {
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPEConferenceCall::CPEConferenceCall start." );
    SetCallState( EPEStateConferenceIdle );
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPEConferenceCall::CPEConferenceCall complete." );
    }
    
    // Destructor
CPEConferenceCall::~CPEConferenceCall
        (
        // None.
        )
    {
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPEConferenceCall::~CPEConferenceCall: Start." );
    iConferenceCall->Release();
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPEConferenceCall::~CPEConferenceCall: Complete." );
    } 

// -----------------------------------------------------------------------------
// CPEConferenceCall::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::ConstructL(
        CCCE& aConvergedCallEngine )
    {
    TEFLOGSTRING( KTAOBJECT, "cpecallgsm: CPEGsmConferenceCall::ConstructL start." );
    iConferenceCall = &( aConvergedCallEngine.ConferenceCallL( *this ) );
    iConferenceCall->SetObserver( *this );
    TEFLOGSTRING( KTAOBJECT, "cpecallgsm: CPEGsmConferenceCall::ConstructL complete." );
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEConferenceCall* CPEConferenceCall::NewL( 
        MPEMessageSender& aOwner,
        CCCE& aConvergedCallEngine
        )
    {
    TEFLOGSTRING( KTAOBJECT, "cpecallgsm: CPEGsmConferenceCall::NewL start." );
    MCCEConferenceCall* temp = NULL;
    
    CPEConferenceCall* self = new ( ELeave ) CPEConferenceCall( aOwner, *temp );
    CleanupStack::PushL( self );
    self->ConstructL( aConvergedCallEngine );
    CleanupStack::Pop( self );

    TEFLOGSTRING( KTAOBJECT, "cpecallgsm: CPEGsmConferenceCall::NewL complete." );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPEConferenceCall* CPEConferenceCall::NewL( 
        MPEMessageSender& aOwner,
        MCCEConferenceCall& aCall
        )
    {
    TEFLOGSTRING( KTAOBJECT, "cpecallgsm: CPEGsmConferenceCall::NewL start." );
    
    CPEConferenceCall* self = new ( ELeave ) CPEConferenceCall( aOwner, aCall );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TEFLOGSTRING( KTAOBJECT, "cpecallgsm: CPEGsmConferenceCall::NewL complete." );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "cpecallgsm: CPEGsmConferenceCall::ConstructL start." );
    iConferenceCall->SetObserver( *this );
    TEFLOGSTRING( KTAOBJECT, "cpecallgsm: CPEGsmConferenceCall::ConstructL complete." );
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::AddCall
// Adds new member to Conference Call
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::AddCallL
        ( 
        MCCECall& aCall
        )
    {
    if( GetCallState() == EPEStateConferenceIdle )
        {
        SetCallState( EPEStateCreatingConference );
        }
    iConferenceCall->AddCallL( aCall );
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::CallEventOccurred
// A conference call event has occurred.
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::CallEventOccurred( 
        const MCCEConferenceCallObserver::TCCEConferenceCallEvent aEvent,
        MCCECall* aReferredCall  )
    {
    TEFLOGSTRING2( KTAREQEND, "CALL CPEConferenceCall::ConferenceCallEventOccurred, event: %d", aEvent );
    
    if ( aReferredCall )
        {
        //resolve the referred call first
        RMobileCall::TMobileCallInfoV1 callInfo;
        RMobileCall::TMobileCallInfoV1Pckg callInfoPckg( callInfo );
        TEFLOGSTRING( KTAREQOUT, "CALL CPEConferenceCall::ConferenceCallEventOccurred: Request MCCECall::GetMobileCallInfo()" );
        aReferredCall->GetMobileCallInfo( callInfoPckg );
        callInfo = callInfoPckg();
        iCallName = callInfo.iCallName;
        
        // now that the call name has been stored, owner may fetch it
        // upon receiving a conference call related indication
        }
    else
        {
        iCallName = KNullDesC;
        }

    switch ( aEvent )
        {
        case MCCEConferenceCallObserver::ECCEConferenceCallAdded:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallEventOccurred, Call added to conference" );
            SendMessage( MEngineMonitor::EPEMessageAddedConferenceMember, iCallName );
            break;
            }
        case MCCEConferenceCallObserver::ECCEConferenceCallRemoved:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallEventOccurred, Call removed from conference" );
            SendMessage( MEngineMonitor::EPEMessageDroppedConferenceMember, iCallName );
            break;
            }
        case MCCEConferenceCallObserver::ECCEConferenceBuilt:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallEventOccurred, Conference call was built" );
            break;
            }
        case MCCEConferenceCallObserver::ECCEConferenceTerminated:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallEventOccurred, Conference call was terminated" );
            break;
            }
        case MCCEConferenceCallObserver::ECCEConferenceSwapped:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallEventOccurred, Conference call was swapped" );
            break;
            }
        case MCCEConferenceCallObserver::ECCEConferenceSplit:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallEventOccurred, Conference call was split" );
            SendMessage( MEngineMonitor::EPEMessageWentOneToOne, iCallName );
            break;
            }
        default:
            {
            TEFLOGSTRING( KTAERROR, "CALL CPEConferenceCall::CallEventOccurred, UNKNOWN CONFERENCE EVENT!" );       
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::CallStateChanged
// A conference call state has changed.
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::CallStateChanged( 
         const MCCEConferenceCallObserver::TCCEConferenceCallState aState )
    {
    switch ( aState )
        {
        case MCCEConferenceCallObserver::ECCEConferenceIdle:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallStateChanged, ECCEConferenceIdle" );
            SetCallState( EPEStateConferenceIdle );
            iLastConferenceDuration = iDuration;
            SendMessage( MEngineMonitor::EPEMessageConferenceIdle );
            break;
            }
        case MCCEConferenceCallObserver::ECCEConferenceActive:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallStateChanged, ECCEConferenceActive" );
            SetCallState( EPEStateConnectedConference );
            SendMessage( MEngineMonitor::EPEMessageConnectedConference );
            break;
            }
        case MCCEConferenceCallObserver::ECCEConferenceHold:
            {
            TEFLOGSTRING( KTAINT, "CALL CPEConferenceCall::CallStateChanged, ECCEConferenceHold" );
            SetCallState( EPEStateHeldConference );
            SendMessage( MEngineMonitor::EPEMessageHeldConference );
            break;
            }
        default:
            {
            TEFLOGSTRING( KTAERROR, "CALL CPEConferenceCall::CallStateChanged, UNKNOWN CONFERENCE STATE!" );       
            break;
            }
        }
    }
    
// -----------------------------------------------------------------------------
// CPEConferenceCall::CallCapsChanged
// Conference call capabilities have changed.
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::CallCapsChanged( 
        const MCCEConferenceCallObserver::TCCEConferenceCallCaps aCaps )
    {
    TEFLOGSTRING2( KTAREQEND, "CALL CPEConferenceCall::CallCapsChanged, caps: %d", aCaps );
    iCaps = aCaps;
    SendMessage( MEngineMonitor::EPEMessageConferenceCapsChange );
    }
    
// -----------------------------------------------------------------------------
// CPEConferenceCall::ErrorOccurred
// 
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::ErrorOccurred( TCCPConferenceCallError aError )
    {
    TEFLOGSTRING2( KTAERROR, "CALL CPEConferenceCall::ErrorOccurred, error: %d", aError );
    
    iOwner.SendErrorMessage( KPEConferenceCallID, aError );

    }
    
// -----------------------------------------------------------------------------
// CPEConferenceCall::SendMessage
// Forwards message to CPEConferenceCall object
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::SendMessage( 
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage )
    {
    iOwner.SendMessage( aMessage, KPEConferenceCallID );
    }

    
// -----------------------------------------------------------------------------
// CPEConferenceCall::SendMessage
// Forwards message to CPEConferenceCall object
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::SendMessage( 
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
        const TName& aName )
    {
    iOwner.SendMessage( aMessage, aName );
    }
// -----------------------------------------------------------------------------
// CPEConferenceCall::GetConferenceDuration
// Returns Conference duration.
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::GetCallDuration
        (
        TTimeIntervalSeconds& aDuration
        )
    {    
    TPEState callState = GetCallState();
    if ( callState != EPEStateConferenceIdle )
        {
        aDuration = iDuration;
        }
    else
        {
        aDuration = iLastConferenceDuration;
        }
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::EnumerateCalls
// Returns number of conference members
// -----------------------------------------------------------------------------
//
TInt CPEConferenceCall::EnumerateCalls()
    {
    TEFLOGSTRING( KTAREQOUT, "cpecall: CPEConferenceCall::EnumerateCalls: MCCEConferenceCall::EnumerateCalls()" );    
    return iConferenceCall->EnumerateCalls();
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::GetCallCaps
// Returns conference call capabilities
// -----------------------------------------------------------------------------
//
MCCEConferenceCallObserver::TCCEConferenceCallCaps 
    CPEConferenceCall::CallCaps()
    {
    return iCaps;
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::HangUp
// Ends conference call.
// -----------------------------------------------------------------------------
//
TInt CPEConferenceCall::HangUp()
    {
    return iConferenceCall->HangUp();
    }
        
// -----------------------------------------------------------------------------
// CPEConferenceCall::Swap
// Swaps conference between active and held states
// -----------------------------------------------------------------------------
//
TInt CPEConferenceCall::Swap()
    {
    return iConferenceCall->Swap();
    }


// -----------------------------------------------------------------------------
// CPEConferenceCall::GetConferenceMemberName
// Returns TName information of added or removed member
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::GetConferenceMemberName
        ( 
        TName& aCallName
        ) const
    {
    aCallName = iCallName;
    }

// -----------------------------------------------------------------------------
// CPEConferenceCall::GoOneToOneL
// Go one to one
// -----------------------------------------------------------------------------
//
void CPEConferenceCall::GoOneToOneL( MCCECall& aCall )
    {
    iConferenceCall->GoOneToOneL( aCall );
    }


// -----------------------------------------------------------------------------
// CPEConferenceCall::CallDurationChanged
// -----------------------------------------------------------------------------
//    
void CPEConferenceCall::CallDurationChanged( const TTimeIntervalSeconds aDuration )
    {
    iDuration = aDuration;
    iOwner.SendMessage( MEngineMonitor::EPEMessageChangedCallDuration, iCallId );
    }

// End Of File
