/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPELogHandlingProxy
*              : class member functions.
*
*/



//  INCLUDE FILES
#include "cpeloghandling.h"
#include "cpeloghandlingdummy.h"
#include "cpeloghandlingproxy.h"
#include <talogger.h>
#include <pepanic.pan>
#include "mpeloghandling.h"
#include <mpephonemodelinternal.h>
#include <mpedatastore.h>

// EXTERNAL DATA STRUCTURES
// None

// EXTERNAL FUNCTION PROTOTYPES  
/// None

// CONSTANTS
// None

// MACROS
// None

// LOCAL CONSTANTS AND MACROS
// None

// MODULE DATA STRUCTURES
// None

// LOCAL FUNCTION PROTOTYPES
// None

// ================= MEMBER FUNCTIONS =======================
// -----------------------------------------------------------------------------
// CPELogHandlingProxy::CreateFirstPhaseL
// This method creates the log handling proxy and dummy implementation
// -----------------------------------------------------------------------------
//
EXPORT_C MPELogHandling* CPELogHandlingProxy::CreateFirstPhaseL()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::CreateFirstPhaseL start." );
    CPELogHandlingProxy* self = new ( ELeave ) CPELogHandlingProxy();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::CreateFirstPhaseL complete" );
    return self;
    }

// -----------------------------------------------------------------------------
// PELogHandling::CreateSecondPhaseL
// This method creates the actual log handling implementation
// -----------------------------------------------------------------------------
//
EXPORT_C void CPELogHandlingProxy::CreateSecondPhaseL(
        MPEPhoneModelInternal& aModel,        // Owner of the object
        RFs& aFsSession                       // Handle to a file server session
        )
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::CreateSecondPhaseL start." );
    iLogHandling = CPELogHandling::NewL( aModel, aFsSession );
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::CreateSecondPhaseL complete." );
    }

// -----------------------------------------------------------------------------
// CPELogHandlingProxy::~CPELogHandlingProxy
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPELogHandlingProxy::~CPELogHandlingProxy()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::~CPELogHandlingProxy" );
    delete iLogHandling;
    delete iLogHandlingDummy;
    }

// -----------------------------------------------------------------------------
// CPELogHandlingProxy::CPELogHandlingProxy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPELogHandlingProxy::CPELogHandlingProxy()
    {
    }

// -----------------------------------------------------------------------------
// CPELogHandlingProxy::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPELogHandlingProxy::ConstructL()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingProxy::ConstructL" );
    // Create an instance of the log handling dummy implementation
    iLogHandlingDummy = CPELogHandlingDummy::NewL();
    }

// -----------------------------------------------------------------------------
// CPELogHandlingProxy::SaveCallEntry
// Redirects the function call to either log handling implementation or
// log handling dummy implementation.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPELogHandlingProxy::SaveCallEntry
        ( 
        const TInt aCallId
        )
    {
    if ( iLogHandling )
        {
        TEFLOGSTRING( KTAINT, "LOG CPELogHandlingProxy::SaveCallEntry > CPELogHandling::SaveCallEntry" );
        return iLogHandling->SaveCallEntry( aCallId );
        }
    else if ( iLogHandlingDummy )
        {
        TEFLOGSTRING( KTAINT, "LOG CPELogHandlingProxy::SaveCallEntry > CPELogHandlingDummy::SaveCallEntry" );
        return iLogHandlingDummy->SaveCallEntry( aCallId );
        }
    else
        {
        TEFLOGSTRING( KTAERROR, "LOG CPELOGHANDLINGPROXY::SAVECALLENTRY ! NEITHER LOG HANDLING NOR LOG HANDLING DUMMY EXISTS" );
        User::Invariant();
        }
    return KErrBadHandle;
    }

// End of File
