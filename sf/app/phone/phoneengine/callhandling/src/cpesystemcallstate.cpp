/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPESystemCallState class 
*                member functions.
*
*/


// INCLUDE FILES
#include <talogger.h>
#include <pepanic.pan>
#include <ctsydomainpskeys.h>

#include "cpesystemcallstate.h"
#include "cpecallarrayowner.h"
#include "mpedatastore.h"
#include "cpesinglecall.h"

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

// FORWARD DECLARATIONS
// None.

// ==================== LOCAL FUNCTIONS ====================
// None.

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CPESystemCallState::CPESystemCallState
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPESystemCallState::CPESystemCallState( 
         CPECallArrayOwner& aCallArrayOwner,
         MPEDataStore& aDataStore ) : 
         iCallArrayOwner( aCallArrayOwner ),
         iDataStore( aDataStore ),
         iCallStateTsy( EPSCTsyCallStateUninitialized ),
         iCallTypeTsy( EPSCTsyCallTypeUninitialized )
    {
    }

// Destructor
CPESystemCallState::~CPESystemCallState()
    {
    TEFLOGSTRING( KTAOBJECT, 
        "CALL CPESystemCallState::~CPESystemCallState start" );
    iCallStateProperty.Close();
    iCallTypeProperty.Close();
    TEFLOGSTRING( KTAOBJECT, 
        "CALL CPESystemCallState::~CPESystemCallState complete" );
    }    

// -----------------------------------------------------------------------------
// CPESystemCallState::NewL
// -----------------------------------------------------------------------------
//
CPESystemCallState* CPESystemCallState::NewL( 
        CPECallArrayOwner& aCallArrayOwner,
        MPEDataStore& aDataStore )
    {
    TEFLOGSTRING( KTAOBJECT,
         "CALL CPESystemCallState::NewL: start." );
    
    CPESystemCallState* self = new ( ELeave ) CPESystemCallState( aCallArrayOwner, aDataStore );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TEFLOGSTRING( KTAOBJECT, 
        "CALL CPESystemCallState::NewL: complete." );
    
    return self;
    }
    
// -----------------------------------------------------------------------------
// CPESystemCallState::ConstructL
// -----------------------------------------------------------------------------
//
void CPESystemCallState::ConstructL()

    {
    // Attach and initialize publish & subscribe properties
    TInt r( KErrNone );
    RProperty::TType type( RProperty::EInt );
    TSecurityPolicy readPolicy( ECapability_None );
    TSecurityPolicy writePolicy( ECapabilityWriteDeviceData );
    
    TEFLOGSTRING( KTAOBJECT,
         "CALL CPESystemCallState::ConstructL: start." );
    iCallStateInCenRep = EPSCTsyCallStateNone;
    iCallTypeInCenRep = EPSCTsyCallTypeNone;
    
    User::LeaveIfError( 
        iCallStateProperty.Attach( KPSUidCtsyCallInformation, KCTsyCallState ) );  
    r =  iCallStateProperty.Set( iCallStateInCenRep );
    if ( KErrNotFound == r )
        {
        // Not defined yet - do so here
        User::LeaveIfError( RProperty::Define( KPSUidCtsyCallInformation, 
            KCTsyCallState, 
            type,
            readPolicy,
            writePolicy ) );
        r = iCallStateProperty.Set( iCallStateInCenRep );
        }
    User::LeaveIfError( r );
    
    User::LeaveIfError( 
        iCallTypeProperty.Attach( KPSUidCtsyCallInformation, KCTsyCallType ) );   
    r =  iCallTypeProperty.Set( iCallTypeInCenRep );
    if ( KErrNotFound == r )
        {
        // Not defined yet - do so here
        User::LeaveIfError( RProperty::Define( KPSUidCtsyCallInformation, 
            KCTsyCallType, 
            type,
            readPolicy,
            writePolicy ) );
        r = iCallTypeProperty.Set( iCallTypeInCenRep );
        }
    User::LeaveIfError( r );
    
    TEFLOGSTRING( KTAOBJECT, 
        "CALL CPESystemCallState::ConstructL: complete." );
    }
    
// -----------------------------------------------------------------------------
// CPESystemCallState::CreateCallL
// -----------------------------------------------------------------------------
//
void CPESystemCallState::NotifySystemCallStateChanged( 
        MEngineMonitor::TPEMessagesFromPhoneEngine aMessage,
        TInt aCallId )
    {
    TEFLOGSTRING3( 
        KTAINT, 
        "CALL CPECallHandling::NotifySystemCallStateChanged, message id: %d, call id: %d", 
        aMessage, 
        aCallId );
        
    iCallStateTsy = EPSCTsyCallStateUninitialized;
    iCallTypeTsy = EPSCTsyCallTypeUninitialized;
    TInt updateError( KErrNone );
    TInt updateErrorTsy( KErrNone );
    
    switch ( aMessage )
        {
        case MEngineMonitor::EPEMessageIncoming:
            HandleIncoming( aCallId );
            break;
        case MEngineMonitor::EPEMessageAnswering:
            HandleAnswering( aCallId );
            break;
        case MEngineMonitor::EPEMessageDialing:
            HandleDialling( aCallId );
            break;
        case MEngineMonitor::EPEMessageConnecting:
        case MEngineMonitor::EPEMessageConnectingWithInband:
            HandleConnecting( aCallId );
            break;
        case MEngineMonitor::EPEMessageConnected:
            HandleConnected( aCallId );
            break;
        case MEngineMonitor::EPEMessageDisconnecting:
        case MEngineMonitor::EPEMessageDisconnectingWithInband:
            HandleDisconnecting( aCallId );
            break;
        case MEngineMonitor::EPEMessageIdle:
            HandleIdle( aCallId );
            break;
        case MEngineMonitor::EPEMessageHeld:
            HandleHeld( aCallId );
            break;
        case MEngineMonitor::EPEMessageCallControlCapsChanged:
            HandleCallControlCapsChanged();
            break;
        case MEngineMonitor::EPEMessageRemotePartyInfoChanged:
            HandleRemotePartyChanged( aCallId );
            break;
        default:
            // Other messages cause no action.
            break;
        } 

    if ( iCallStateTsy != EPSCTsyCallStateUninitialized && 
         iCallStateTsy != iCallStateInCenRep )
        {
        TEFLOGSTRING2( 
            KTAREQOUT, 
            "CALL CPECallHandling::NotifySystemCallStateChanged request rproperty::set, Call State Tsy: %d", 
            iCallStateTsy );
        updateErrorTsy = RProperty::Set( KPSUidCtsyCallInformation, 
        KCTsyCallState, iCallStateTsy );
        iCallStateInCenRep = iCallStateTsy;
        }

    if ( iCallTypeTsy != EPSCTsyCallTypeUninitialized && 
         !updateError && 
         !updateErrorTsy && 
         iCallTypeTsy != iCallTypeInCenRep )
        {
        TEFLOGSTRING2( 
            KTAREQOUT, 
            "CALL CPECallHandling::NotifySystemCallStateChanged request rproperty::set, Call Type: %d", 
            iCallTypeTsy );
        updateError = iCallTypeProperty.Set( iCallTypeTsy );
        updateErrorTsy = RProperty::Set( KPSUidCtsyCallInformation, 
        KCTsyCallType, iCallTypeTsy );
        iCallTypeInCenRep = iCallTypeTsy;
        }

    if ( updateError || updateErrorTsy )
        {// Logs error - but does not return it
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::NOTIFYSYSTEMCALLSTATECHANGED: UNABLE TO UPDATE SYSTEM KEYS, ERROR = %d", 
            updateError || updateErrorTsy );
        }
    }

// -----------------------------------------------------------------------------
// CPESystemCallState::HandleIncoming
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleIncoming( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleIncoming" );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( !callData )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::HandleIncoming: call not found, call id  = %d", 
            aCallId );
        __ASSERT_DEBUG( callData, Panic( EPEPanicCallDataNull) );
        return;
        }
    iCallStateTsy = EPSCTsyCallStateRinging;
    SetCallInformatioAPICallType( *callData );
    }
    
// -----------------------------------------------------------------------------
// CPESystemCallState::HandleDialling
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleDialling( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleDialling" );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( !callData )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::HandleDialling: call not found, call id  = %d", 
            aCallId );
        __ASSERT_DEBUG( callData, Panic( EPEPanicCallDataNull) );
        return;
        }
    iCallStateTsy = EPSCTsyCallStateDialling;
    SetCallInformatioAPICallType( *callData );
    }
    
// -----------------------------------------------------------------------------
// CPESystemCallState::HandleConnected
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleConnected( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleConnected" );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( !callData )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::HandleConnected: call not found, call id  = %d", 
            aCallId );
        __ASSERT_DEBUG( callData, Panic( EPEPanicCallDataNull) );
        return;
        }
    
    if ( !iCallArrayOwner.CallByState( EPEStateRinging ) )
        {
        iCallStateTsy = EPSCTsyCallStateConnected;
        // Needs update also CallType. Even when replace Video Call, see error ELYG-7AWC2N.    
        SetCallInformatioAPICallType( *callData );
        }
    }

// -----------------------------------------------------------------------------
// CPESystemCallState::HandleRemotePartyChanged
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleRemotePartyChanged( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleRemotePartyChanged" );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( !callData )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::HandleRemotePartyChanged: call not found, call id  = %d", 
            aCallId );
        __ASSERT_DEBUG( callData, Panic( EPEPanicCallDataNull) );
        return;
        }
    CPESingleCall* connectedCallData = iCallArrayOwner.CallPointerByState( EPEStateConnected );

    if ( connectedCallData )
        {
        iCallStateTsy = EPSCTsyCallStateConnected;
        // Needs update also CallType. Even when replace Video Call, see error ELYG-7AWC2N.    
        SetCallInformatioAPICallType( *callData );
        }
    }
    
// -----------------------------------------------------------------------------
// CPESystemCallState::HandleDisconnecting
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleDisconnecting( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleDisconnecting" );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( !callData )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::HandleDisconnecting: call not found, call id  = %d", 
            aCallId );
        __ASSERT_DEBUG( callData, Panic( EPEPanicCallDataNull) );
        return;
        }
    if ( !iCallArrayOwner.CallByState( EPEStateConnected ) &&
         !iCallArrayOwner.CallByState( EPEStateHeld ) &&
         !iCallArrayOwner.CallByState( EPEStateDialing ) && 
         !iCallArrayOwner.CallByState( EPEStateConnecting ) && 
         !iCallArrayOwner.CallByState( EPEStateRinging ) )
        {
        iCallStateTsy = EPSCTsyCallStateDisconnecting;
        SetCallInformatioAPICallType( *callData );
        }
    }
    
// -----------------------------------------------------------------------------
// CPESystemCallState::HandleIdle
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleIdle( TInt /*aCallId*/ )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleIdle" );
    if ( !iCallArrayOwner.ActiveCallCount() )
        {
        iCallStateTsy = EPSCTsyCallStateNone;
        iCallTypeTsy = EPSCTsyCallTypeNone;
        }
    else
        {
         // There is other active / waiting call(s)
        CPESingleCall* ringingCallData = iCallArrayOwner.CallPointerByState( EPEStateRinging );
        if ( !ringingCallData )
            {
            CPESingleCall* connectedCallData = iCallArrayOwner.CallPointerByState( EPEStateConnected );
            CPESingleCall* heldCallData = iCallArrayOwner.CallPointerByState( EPEStateHeld );
            if ( connectedCallData )
                {
                iCallStateTsy = EPSCTsyCallStateConnected;
                SetCallInformatioAPICallType( *connectedCallData );
                }                    
            else if ( heldCallData )
                {
                iCallStateTsy = EPSCTsyCallStateHold;
                SetCallInformatioAPICallType( *heldCallData );
                }
            }
        else
            {
            iCallStateTsy = EPSCTsyCallStateRinging;
            SetCallInformatioAPICallType( *ringingCallData );
            }   
        }
    }
    
// -----------------------------------------------------------------------------
// CPESystemCallState::HandleHeld
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleHeld( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleHeld" );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( !callData )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::HandleHeld: call not found, call id  = %d", 
            aCallId );
        __ASSERT_DEBUG( callData, Panic( EPEPanicCallDataNull) );
        return;
        }

    if( !iCallArrayOwner.CallByState( EPEStateDialing ) &&
        !iCallArrayOwner.CallByState( EPEStateRinging ) &&
        !iCallArrayOwner.CallByState( EPEStateConnecting ) &&
        !iCallArrayOwner.CallByState( EPEStateConnected )&&
        !iCallArrayOwner.CallByState( EPEStateAnswering ) )
        {
        iCallStateTsy = EPSCTsyCallStateHold;
        SetCallInformatioAPICallType( *callData );
        }
    }

// -----------------------------------------------------------------------------
// CPESystemCallState::HandleAnswering
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleAnswering( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleAnswering" );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( !callData )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::HandleHeld: call not found, call id  = %d", 
            aCallId );
        __ASSERT_DEBUG( callData, Panic( EPEPanicCallDataNull) );
        return;
        }
    
    iCallStateTsy = EPSCTsyCallStateAnswering; 
    SetCallInformatioAPICallType( *callData );
    }

// -----------------------------------------------------------------------------
// CPESystemCallState::HandleConnecting
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleConnecting( TInt aCallId )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleConnecting" );
    CPESingleCall* callData = iCallArrayOwner.GetCallObject( aCallId );
    if( !callData )
        {
        TEFLOGSTRING2( 
            KTAERROR, 
            "CALL CPECallHandling::HandleHeld: call not found, call id  = %d", 
            aCallId );
        __ASSERT_DEBUG( callData, Panic( EPEPanicCallDataNull) );
        return;
        }
    
    iCallStateTsy = EPSCTsyCallStateAlerting;
    SetCallInformatioAPICallType( *callData );
    }

// -----------------------------------------------------------------------------
// CPESystemCallState::HandleCallControlCapsChanged
// -----------------------------------------------------------------------------
//
void CPESystemCallState::HandleCallControlCapsChanged()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::HandleCallControlCapsChanged" );
    CPESingleCall* callData = iCallArrayOwner.CallPointerByState( EPEStateConnected );
    if( callData && !iCallArrayOwner.CallByState( EPEStateRinging ) )
        {
        iCallStateTsy = EPSCTsyCallStateConnected;
        SetCallInformatioAPICallType( *callData );
        }
    }
    
// -----------------------------------------------------------------------------
// CPESystemCallState::SetCallInformatioAPICallType
// -----------------------------------------------------------------------------
//
void CPESystemCallState::SetCallInformatioAPICallType( CPESingleCall& aCallData )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::SetCallInformatioAPICallType" );
    switch ( aCallData.Call().Parameters().CallType() )
        {
        case CCPCall::ECallTypeCSVoice:
            TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::SetCallInformatioAPICallType EPSCTsyCallTypeCSVoice" );
            iCallTypeTsy = EPSCTsyCallTypeCSVoice;
            break;
        case CCPCall::ECallTypePS:
            TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::SetCallInformatioAPICallType EPSCTsyCallTypeVoIP" );
            iCallTypeTsy = EPSCTsyCallTypeVoIP;
            break;
        case CCPCall::ECallTypeVideo:
            TEFLOGSTRING( KTAINT, "CALL CPESystemCallState::SetCallInformatioAPICallType EPSCTsyCallTypeH324Multimedia" );
            iCallTypeTsy = EPSCTsyCallTypeH324Multimedia;
            break;
        default:
            TEFLOGSTRING( 
                KTAERROR, 
                "CALL CPECallHandling::SetCallInformatioAPICallType: call type not recognised" );
            __ASSERT_DEBUG( EFalse, Panic( EPEPanicCallDataNull) );
            break;
        }
    }
    
// ================= OTHER EXPORTED FUNCTIONS ===============================


// End of File 
