/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This file contains the implementation of CPELogHandlingDummy
*              : class member functions.
*
*/



//  INCLUDE FILES
#include "cpeloghandlingdummy.h"
#include "cpelogevent.h"
#include <talogger.h>
#include <pepanic.pan>
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

// -----------------------------------------------------------------------------
// CPELogHandlingDummy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
MPELogHandling* CPELogHandlingDummy::NewL()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingDummy::NewL start" );
    CPELogHandlingDummy* self = new ( ELeave ) CPELogHandlingDummy();

    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingDummy::NewL complete" );
    return self;
    }

// -----------------------------------------------------------------------------
// CPELogHandlingDummy::~CPELogHandlingDummy
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPELogHandlingDummy::~CPELogHandlingDummy()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingDummy::~CPELogHandlingDummy" );
    }
    
// -----------------------------------------------------------------------------
// CPELogHandlingDummy::CPELogHandlingDummy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPELogHandlingDummy::CPELogHandlingDummy()
    {
    TEFLOGSTRING( KTAOBJECT, "LOG CPELogHandlingDummy::CPELogHandlingDummy" );
    }

// -----------------------------------------------------------------------------
// CPELogHandlingDummy::SaveCallEntry
// Dummy method
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPELogHandlingDummy::SaveCallEntry
        ( 
        const TInt /*aCallId*/
        )
    {
    return KErrNone;
    }

// End of File
