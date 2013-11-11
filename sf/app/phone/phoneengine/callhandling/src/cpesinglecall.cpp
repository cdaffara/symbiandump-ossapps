/*
* Copyright (c) 2003-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPESingleCall class 
*                member functions.
*
*/


// INCLUDE FILES
#include <pepanic.pan>
#include <talogger.h>

#include "cpesinglecall.h"
#include "mpecallowner.h"
#include "cpevideocallhandling.h"


// ================= MEMBER FUNCTIONS =======================
// Destructor
//
CPESingleCall::~CPESingleCall()
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCall::~CPESingleCall: Start." );
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCall::~CPESingleCall: Complete." );
    }

// -----------------------------------------------------------------------------
// CPESingleCall::CPESingleCall
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPESingleCall::CPESingleCall( 
        MPEMessageSender& aOwner
        ) : CPESingleCallObserver( aOwner ),
            iMissedCall( EFalse)
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCall::CPESingleCall start." );
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCall::CPESingleCall complete." );
    }

// -----------------------------------------------------------------------------
// CPEGsmVoiceCall::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPESingleCall::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCall:::ConstructL start." );
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCall::ConstructL complete." );
    }

// -----------------------------------------------------------------------------
// CPEGsmVoiceCall::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPESingleCall* CPESingleCall::NewL( 
        MPEMessageSender& aOwner )
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCall::NewL start." );
    CPESingleCall* self = new ( ELeave ) CPESingleCall( aOwner );
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop( self );

    TEFLOGSTRING( KTAOBJECT, "CALL CPESingleCall::NewL complete." );
    return self;
    }

// -----------------------------------------------------------------------------
// CPESingleCall::SendMessage
// Method reroutes messages from other modules to the CPEPhoneData-object
// Note that it does not use base class SendMessage.
// -----------------------------------------------------------------------------
//
void CPESingleCall::SendMessage( 
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage )
    {
    iErrorCode = KErrNone;
    if( aMessage == MEngineMonitor::EPEMessageIdle )
        {
        // Get the call duration before the call object is closed.
        iDuration = Call().CallDuration();
        TEFLOGSTRING2( 
            KTAREQEND, 
            "CALL CPESingleCall::SendMessage: MCCECall::GetCallDuration() called, Duration: %d", 
            iDuration.Int() );
        }
   
    if ( aMessage != MEngineMonitor::EPEMessageUnknown )
        {
        CPECall::SendMessage( aMessage );
        }
        
    }

// -----------------------------------------------------------------------------
// CPESingleCall::Answer
// Method Answers the incoming call.
// Method creates iCallInfo parameter package and
// makes answer request to CPECallCommand-parameter.
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::Answer()
    {
    SetMissedCall( EFalse );
    return Call().Answer();    
    }

// -----------------------------------------------------------------------------
// CPESingleCall::Dial
// creates dial request to the CPECallCommand object.
// Method opens the line for the new call.
// If error occurs the method returns error value.
// Method makes call parameter package and
// starts monitoring call statuses and call events and
// makes a dial request to the CPECallCommand object.
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::Dial( 
        const RMobileCall::TTelNumberC& aNumber )
    {
    TInt errorCode( KErrNone );
    iDialledNumber = aNumber;
    
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::Dial");
    errorCode = Call().Dial(KNullDesC8);
    
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPESingleCall::DialEmergency
// creates emergency dial request to the CPECallCommand object
// -----------------------------------------------------------------------------
//
void CPESingleCall::DialEmergency(
        const TPEPhoneNumber& aEmergencyNumber )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::DialEmergency" );
    Call().DialEmergencyCall( aEmergencyNumber );
    }

// -----------------------------------------------------------------------------
// CPESingleCall::DisableFDNCheck
// Disables FDN checking for SIM ATK originated calls
// -----------------------------------------------------------------------------
//
void CPESingleCall::DisableFDNCheck()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::DisableFDNCheck UNSUPPORTED" );
    
    // NOTE! Currently disabled due to incomplete TSY implementation
    // Remove comments after this task is completed
    
    //iCall->NoFDNCheck();
    }
        
// -----------------------------------------------------------------------------
// CPESingleCall::Hold
// creates hold request to the CPECallCommand object
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::Hold()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::Hold" );
    return Call().Hold();
    }

// -----------------------------------------------------------------------------
// CPESingleCall::Resume
// Method resumes held call
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::Resume()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::Resume" );
    return Call().Resume();
    }

// -----------------------------------------------------------------------------
// CPESingleCall::Swap
// Method swaps active and held call
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::Swap()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::Swap" );
    return Call().Swap();
    }

// -----------------------------------------------------------------------------
// CPESingleCall::Transfer
// Method Transfers the active and the held call.
// -----------------------------------------------------------------------------
//
void CPESingleCall::Transfer( const TDesC& aTransferTarget )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::Transfer" );
    Call().AttendedTransfer( aTransferTarget );
    }

// -----------------------------------------------------------------------------
// CPESingleCall::Tone
// Returns the inband tone needed to play in state transition
// -----------------------------------------------------------------------------
//
TCCPTone CPESingleCall::Tone()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::Tone" );
    return Call().Tone();
    }

// -----------------------------------------------------------------------------
// CPESingleCall::HangUp
// 
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::HangUp()
    {
    TEFLOGSTRING( KTAREQOUT, "CALL CPESingleCall::HangUp" );
    SetMissedCall( EFalse );
    return Call().HangUp();
    }

// -----------------------------------------------------------------------------
// CPESingleCall::GetCallInfo
// Method returns call information.
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::GetCallInfo(
        RMobileCall::TMobileCallInfoV3& aCallInfo )
    {
    TEFLOGSTRING( KTAREQOUT, 
            "CALL CPESingleCall::GetCallInfo > RMobileCall::GetMobileCallInfo()" );
    RMobileCall::TMobileCallInfoV3Pckg callInfoPckg( iCallInfo );
    Call().GetMobileCallInfo( callInfoPckg );

    iCallInfo = callInfoPckg();
   
    aCallInfo = iCallInfo;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPESingleCall::SetMissedCall
// Set the missed call indicator = ETrue, if the current call
// was hanged up on state ringing by remote party.
// -----------------------------------------------------------------------------
//
void CPESingleCall::SetMissedCall( TBool aMissed )
    {
    TEFLOGSTRING2( KTAINT, "CALL CPESingleCall::SetMissedCall: Call Missed. %d", aMissed );
    iMissedCall = aMissed; 
    }

// -----------------------------------------------------------------------------
// CPESingleCall::GetMissedCall
// Returns the missed data call indicator.
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::GetMissedCall(
        TBool& aMissedCall) const
    {
    aMissedCall = iMissedCall;
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CPESingleCall::IsSecureCall
// -----------------------------------------------------------------------------
//
TBool CPESingleCall::IsSecureCall()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::IsSecureCall" );
    return Call().IsSecured();
    }

// -----------------------------------------------------------------------------
// CPESingleCall::SecureSpecified
// -----------------------------------------------------------------------------
//
TBool CPESingleCall::SecureSpecified()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::SecureSpecified" );
    return Call().SecureSpecified(); 
    }

// -----------------------------------------------------------------------------
// CPESingleCall::AcceptUnattendedTransfer
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::AcceptUnattendedTransfer()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::AcceptUnattendedTransfer" );
    
    return Call().AcceptTransfer( ETrue );
    }


// -----------------------------------------------------------------------------
// CPESingleCall::RejectUnattendedTransfer
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::RejectUnattendedTransfer()
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::RejectUnattendedTransfer" );
    
    return Call().AcceptTransfer( EFalse );
    }

// -----------------------------------------------------------------------------
// CPESingleCall::UnattendedTransfer
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::UnattendedTransfer( const TDesC& aTransferTo )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::UnattendedTransfer" );

    return Call().UnattendedTransfer( aTransferTo );
    }

// -----------------------------------------------------------------------------
// CPESingleCall::ForwardCallToAddress
// -----------------------------------------------------------------------------
//
TInt CPESingleCall::ForwardCallToAddress( TInt aIndex )
    {
    TEFLOGSTRING( KTAINT, "CALL CPESingleCall::ForwardToAddress" );
    TRAPD( err, Call().ForwardToAddressL( aIndex ) );
    return err;
    }
    
// End of File 
