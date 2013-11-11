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
* Description:  Implementation of CPEParserSSCallHandler class.
*
*/


// INCLUDE FILES
#include "cpemanualcallcontrolhandler.h"
#include "cpemessagehandler.h"
#include "cpeparsersscallhandler.h"
#include "mpephonemodelinternal.h"
#include <etel.h>
#include <etelmm.h>
#include <gsmerror.h>
#include <mcceconferencecallobserver.h>
#include <mpedatastore.h>
#include <talogger.h>


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

// ============================= LOCAL FUNCTIONS ===============================
// None.


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::CPEParserSSCallHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEParserSSCallHandler::CPEParserSSCallHandler
        ( 
        CPEMessageHandler& aOwner, // Reference to owner of this object.  
        MPEPhoneModelInternal& aModel,
        CPEManualCallControlHandler& aManualCallControlHandler
        ) : iOwner( aOwner ),
            iModel( aModel ),
            iManualCallControlHandler( aManualCallControlHandler )
    {
    }

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::~CPEParserSSCallHandler
// Destructor.
// -----------------------------------------------------------------------------
//
CPEParserSSCallHandler::~CPEParserSSCallHandler
        (
        // None.
        )
    {    
    }

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::ProcessEndCall
// End or reject a call. Activate entering string "0" + SEND
// If there is waiting call reject it.
// If there is not waiting call but tere is held call then end held call.
// If there is no waiting or held call then set error code ECCPErrorNotAllowed.
// -----------------------------------------------------------------------------
//
void CPEParserSSCallHandler::ProcessEndOrRejectL
        ( 
        // None.
        )
    {   
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSCallHandler::ProcessEndOrRejectL: Start" ); 
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    
    TInt errorCode = iManualCallControlHandler.HandleChldZero();
    iOwner.SetGsmParserErrorCode( errorCode );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSCallHandler::ProcessEndOrRejectL: Stop" ); 
    }

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::ProcessEndAndAccept
// Ends a call and accepts another. Activate entering string "1" + SEND
// 1. Check if conference call or voice call is active and release it.
// 2. If there is waiting call then answer waiting call.
// 3. If there is no waiting and if there is held call then resume held call. 
// -----------------------------------------------------------------------------
//
void CPEParserSSCallHandler::ProcessEndAndAcceptL
        ( 
        // None.
        )
    {
    TEFLOGSTRING( KTAINT, 
        "PE CPEParserSSCallHandler::ProcessEndAndAcceptL: Start" );      
    TInt error( ECCPErrorNotAllowed );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    
    error = iManualCallControlHandler.HandleChldOne();
    iOwner.SetGsmParserErrorCode( error );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAINT, 
        "PE CPEParserSSCallHandler::ProcessEndAndAcceptL: Stop" ); 
    }  

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::ProcessEndCall
// End specific active call. Activate entering string "1x" + SEND 
// -----------------------------------------------------------------------------
//
void CPEParserSSCallHandler::ProcessEndCallL
        ( 
        TInt aCallNo // The number of the call.
        )
    {
    TEFLOGSTRING( KTAINT,"PE CPEParserSSCallHandler::ProcessEndCallL: Start" ); 
    TInt errorCode( ECCPErrorNotAllowed );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    
    TRAP( errorCode, iManualCallControlHandler.HandleChldOneXL( aCallNo ) );
    iOwner.SetGsmParserErrorCode( errorCode );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAINT,"PE CPEParserSSCallHandler::ProcessEndCallL: Stop" ); 
    }

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::ProcessSwap
// Swaps active and held calls. Activate entering string "2" + SEND.
// -----------------------------------------------------------------------------
//
void CPEParserSSCallHandler::ProcessSwapL
        ( 
        // None.
        )
    { 
    TEFLOGSTRING( KTAINT, "PE CPEParserSSCallHandler::ProcessSwapL: Start" );    
    TInt errorCode( ECCPErrorNotAllowed );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    errorCode = iManualCallControlHandler.HandleChldTwo();
    iOwner.SetGsmParserErrorCode( errorCode );

    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAINT, "PE: CPEParserSSCallHandler::ProcessSwapL: End" );
    }

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::ProcessEndCall
// Hold all active calls except specific call. Activate entering string "2x" + SEND.
// -----------------------------------------------------------------------------
//
void CPEParserSSCallHandler::ProcessHoldAllCallsExceptL
        ( 
        TInt aCallNo    // The number of the call.
        )
    {
    TEFLOGSTRING( KTAINT, 
        "PE CPEParserSSCallHandler::ProcessHoldAllCallsExceptL: Start" );
    TInt errorCode( ECCPErrorNotAllowed );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    
    TRAP( errorCode, iManualCallControlHandler.HandleChldTwoXL( aCallNo ) );
    iOwner.SetGsmParserErrorCode( errorCode );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
        
    TEFLOGSTRING( KTAINT, 
        "PE CPEParserSSCallHandler::ProcessHoldAllCallsExceptL: Stop" ); 
    }

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::ProcessJoinL
// Build conference call or add new member to conference call.
// Activate entering string "3" + SEND.
// 1. If there is no conference call then join two calls to conference.
// 2. If there is active conference call and held call then add member 
// (held call) to conference call.
// 3. If there is held conference call and active call then add member 
// (active call) to conference call.
// -----------------------------------------------------------------------------
//
void CPEParserSSCallHandler::ProcessJoinL
        ( 
        // None.
        )
    { 
    TEFLOGSTRING( KTAINT, "PE CPEParserSSCallHandler::ProcessJoinL: Start" );
    TInt errorCode( ECCPErrorNotAllowed );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );

    errorCode = iManualCallControlHandler.HandleChldThree();
    iOwner.SetGsmParserErrorCode( errorCode );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAINT, "PE CPEParserSSCallHandler::ProcessJoinL: Stop" ); 
    }

// -----------------------------------------------------------------------------
// CPEParserSSCallHandler::ProcessEndCall
// Perform explicit call transfer. Activate entering string "4" + SEND.
// -----------------------------------------------------------------------------
//
void CPEParserSSCallHandler::ProcessExplicitCallTransferL
        ( 
        // None.
        )
    {
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSCallHandler::ProcessExplicitCallTransferL: Start" );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuingSSRequest );
    
    TInt errorCode = iManualCallControlHandler.HandleChldFour();
    iOwner.SetGsmParserErrorCode( errorCode );
    
    iModel.SendMessage( MEngineMonitor::EPEMessageIssuedSSRequest );
    
    TEFLOGSTRING( KTAINT,
        "PE CPEParserSSCallHandler::ProcessExplicitCallTransferL: Stop" ); 
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None.

//  End of File  
