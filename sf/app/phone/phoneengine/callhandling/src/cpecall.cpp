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
* Description:  This file contains the implementation of CPECall class 
*                member functions.
*
*/


// INCLUDE FILES
#include <talogger.h>

#include "cpecall.h"
#include "mpemessagesender.h"

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
// CPECall::CPECall
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CPECall::CPECall
        ( 
        MPEMessageSender& aOwner
        ) : iOwner( aOwner ),
            iCallState( EPEStateIdle )
    {
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPECall::cpecall: start." );
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPECall::cpecall: complete." );
    }

// Destructor
EXPORT_C CPECall::~CPECall
        (
        // None.
        )
    {
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPECall::~CPECall: Start." );
    TEFLOGSTRING( KTAOBJECT, "cpecall: CPECall::~CPECall: Complete." );
    }    

// -----------------------------------------------------------------------------
// CPECall::SendErrorMessage
// Method reroutes error message from other modules to the CPEPCallHandling-object
// -----------------------------------------------------------------------------
//
EXPORT_C void CPECall::SendErrorMessage( const TInt aErrorCode )
    {
    iErrorCode = aErrorCode;
    iOwner.SendErrorMessage( iCallId, aErrorCode );
    }

// -----------------------------------------------------------------------------
// CPECall::SendMessage
// Method reroutes messages from other modules to the CPECallHandling-object
// -----------------------------------------------------------------------------
//
void CPECall::SendMessage
        ( 
        const MEngineMonitor::TPEMessagesFromPhoneEngine aMessage
        )
    {
    iOwner.SendMessage( aMessage, iCallId );
    }

// -----------------------------------------------------------------------------
// CPECall::GetErrorCode
// Get the error code of the call.
// -----------------------------------------------------------------------------
//
void CPECall::GetErrorCode
        (
        TInt& aErrorCode
        ) const
    {
    aErrorCode = iErrorCode;
    }
    
// -----------------------------------------------------------------------------
// CPECall::SetCallState
// Method sets call state.
// -----------------------------------------------------------------------------
//
void CPECall::SetCallState
        ( 
        const TPEState aCallState
        )
    {
    iCallState =  aCallState;
    }

// -----------------------------------------------------------------------------
// CPECall::GetCallState
// Method returns call state.
// -----------------------------------------------------------------------------
//
TPEState CPECall::GetCallState
        (
        // None.
        ) const
    {
    return iCallState;
    }

// -----------------------------------------------------------------------------
// CPECall::SetCallId
// Method sets call identification numbers
// -----------------------------------------------------------------------------
//
void CPECall::SetCallId
        ( 
        const TInt aCallId
        )
    {
    iCallId = aCallId;
    }

// -----------------------------------------------------------------------------
// CPECall::GetCallId
// Method returns call identification numbers
// -----------------------------------------------------------------------------
//
TInt CPECall::GetCallId
        (
        // None.
        ) const
    {
    return iCallId;
    }
    
// ================= OTHER EXPORTED FUNCTIONS ===============================
// None.

// End of File 
