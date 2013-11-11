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
* Description:  This file contains the implementation of CPEDtmfHandling class 
*                member functions.
*
*/


// INCLUDE FILES
#include "cpedtmfhandling.h"
#include "cpecallhandling.h"

#include <pevirtualengine.h>
#include <talogger.h>
#include <featmgr.h>
#include <mccedtmfinterface.h>
#include <ccce.h>


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
// CPEDtmfHandling::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEDtmfHandling* CPEDtmfHandling::NewL( 
        CPECallHandling& aOwner,
        MCCEDtmfInterface& aDtmfInterface )
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPEDtmfHandling::NewL start." );
    CPEDtmfHandling* self = new ( ELeave ) CPEDtmfHandling( aOwner, 
                                                            aDtmfInterface );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    TEFLOGSTRING( KTAOBJECT, "CALL CPEDtmfHandling::NewL complete." );
    return self;
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::~CPEDtmfHandling
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEDtmfHandling::~CPEDtmfHandling()
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPEDtmfHandling::~CPEDtmfHandling start." );
    TEFLOGSTRING( KTAOBJECT, "CALL CPEDtmfHandling::~CPEDtmfHandling complete." );
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::CPEDtmfHandling
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------

EXPORT_C CPEDtmfHandling::CPEDtmfHandling( 
        CPECallHandling& aOwner,
        MCCEDtmfInterface& aDtmfInterface
        ) : iOwner( aOwner ),
            iDtmfInterface( aDtmfInterface )  
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPEDtmfHandling::CPEDtmfHandling start." );
    iDtmfInterface.SetObserver( *this );
    TEFLOGSTRING( KTAOBJECT, "CALL CPEDtmfHandling::CPEDtmfHandling complete." );
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEDtmfHandling::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "CALL CPEDtmfHandling::ConstructL start." );
    TEFLOGSTRING( KTAOBJECT, "CALL CPEDtmfHandling::ConstructL complete." );
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::CancelDtmfString
// Method cancels the ongoing DTMF tones string sending.
// -----------------------------------------------------------------------------
//
void CPEDtmfHandling::CancelDtmfString()
    {
    TEFLOGSTRING( KTAMESOUT, "CALL CPEDtmfHandling::CancelDtmfString" );
    iDtmfInterface.CancelDtmfStringSending();
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::ContinueDtmfSending
// Continue the sending of a DTMF string when it was stopped by the use of 
// 'w' char in the string.
// -----------------------------------------------------------------------------
//
void CPEDtmfHandling::ContinueDtmfSending()
    {
    TEFLOGSTRING( KTAMESOUT, 
        "CALL CPEDtmfHandling::ContinueDtmfSending, EPEMessageContinuingDTMFSending" );
    iOwner.SendMessage( MEngineMonitor::EPEMessageContinuingDTMFSending );
    TEFLOGSTRING( KTAREQOUT, 
        "CALL CPEDtmfHandling::ContinueDtmfSending > MCCEDtmfInterface::ContinueStringSending( ETrue )" );
    TInt errorCode = iDtmfInterface.ContinueStringSending( ETrue );
    TEFLOGSTRING2( KTAREQEND, 
        "CALL CPEDtmfHandling::ContinueDtmfSending > MCCEDtmfInterface::ContinueStringSending( ETrue ); errorCode = %d", errorCode );          
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::SendDtmfString
// Starts sending of DTMF tones string.
// -----------------------------------------------------------------------------
//
TInt CPEDtmfHandling::SendDtmfString( 
        const TDesC& aTones )
    {
    TEFLOGSTRING( KTAREQOUT, 
        "CALL CPEDtmfHandling::SendDtmfString > MCCEDtmfInterface::SendDtmfToneString()" );
    TInt errorCode = iDtmfInterface.SendDtmfToneString( aTones );
    TEFLOGSTRING2( KTAREQEND, 
        "CALL   CPEDtmfHandling::SendDtmfString > MCCEDtmfInterface::SendDtmfToneString(); errorCode = %d"
        , errorCode );
    if( errorCode == KErrNone )
        {
        iOwner.SendMessage( MEngineMonitor::EPEMessageSendingDTMF );
        }
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::StartDtmfTone
// Starts the transmission of a single DTMF tone across a connected and active
// voice call. 
// -----------------------------------------------------------------------------
//
TInt CPEDtmfHandling::StartDtmfTone( 
        const TChar& aTone ) const
    {
    TEFLOGSTRING( KTAREQOUT, 
        "CALL CPEDtmfHandling::StartDtmfTone > MCCEDtmfInterface::StartDtmfTone()" );
    TInt errorCode = iDtmfInterface.StartDtmfTone( aTone );
    TEFLOGSTRING2( KTAREQEND, 
        "CALL CPEDtmfHandling::StartDtmfTone > MCCEDtmfInterface::StartDtmfTone(); errorCode = %d", errorCode );
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::StopDtmfSending
// Cancels dtmf sending after 'w'-characte
// -----------------------------------------------------------------------------
//
void CPEDtmfHandling::StopDtmfSending()
    {
    TEFLOGSTRING( KTAREQEND, 
        "CALL CPEDtmfHandling::StopDtmfSending > MCCEDtmfInterface::CancelDtmfStringSending()" );
    iDtmfInterface.CancelDtmfStringSending();
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::StopDtmfTone
// Stops the transmission of a single DTMF tone, which had previously been
// started by use of the StartDTMFTone function member, across a connected 
// and active voice call.
// -----------------------------------------------------------------------------
//
TInt CPEDtmfHandling::StopDtmfTone() const
    {
    TEFLOGSTRING( KTAREQOUT, 
        "CALL CPEDtmfHandling::StopDtmfTone > MCCEDtmfInterface::StopDtmfTone()" );
    TInt errorCode = iDtmfInterface.StopDtmfTone();
    TEFLOGSTRING2( KTAREQEND,
        "CALL PEDtmfHandling::StopDtmfTone > MCCEDtmfInterface::StopDtmfTone(), errorCode = %d"
        , errorCode );
    return errorCode;
    }

// -----------------------------------------------------------------------------
// CPEDtmfHandling::HandleDtmfEvent
// From MCCEDtmfObserver.
// All the occurred DTMF events are notified through this interface.
// -----------------------------------------------------------------------------
//
EXPORT_C void CPEDtmfHandling::HandleDTMFEvent( 
    const MCCEDtmfObserver::TCCEDtmfEvent aEvent, 
    const TInt aError, 
    const TChar /*aTone*/ )
    {
    TEFLOGSTRING3( KTAMESIN, 
        "CALL CPEDtmfHandling::HandleDtmfEvent: aEvent %d, aError %d", 
        aEvent, 
        aError );
        
    switch( aEvent )
        {
        case ECCEDtmfUnknown:
            iOwner.SendErrorMessage( KPECallIdNotUsed, KErrUnknown );
            break;
            
        case ECCEDtmfManualStart:
        case ECCEDtmfSequenceStart:
            // Handled in audio handling
            break;
            
        case ECCEDtmfManualStop:
        case ECCEDtmfSequenceStop:
            // Handled in audio handling
            break;
            
        case ECCEDtmfManualAbort:
        case ECCEDtmfSequenceAbort:
            iOwner.SendMessage( MEngineMonitor::EPEMessageDTMFSendingAborted );
            break;
            
        case ECCEDtmfStopInDtmfString:
            iOwner.SendMessage( MEngineMonitor::EPEMessageStoppedDTMF );
            break;
            
        case ECCEDtmfStringSendingCompleted:
            iOwner.SendMessage( MEngineMonitor::EPEMessageSentDTMF );
            break;

        default:
            TEFLOGSTRING( KTAERROR, 
                "CALL CPEDtmfHandling::HandleDtmfEvent: NOT HANDLED ENUM VALUE" );
            break;
        }
  
    if( aError != KErrNone && 
        !( aEvent == ECCEDtmfStringSendingCompleted && aError == KErrEtelCallNotActive ) )
        {
        iOwner.SendErrorMessage( KPECallIdNotUsed, aError );    
        }
    }
    
// ================= OTHER EXPORTED FUNCTIONS ===============================
// None.

//  End of File 
