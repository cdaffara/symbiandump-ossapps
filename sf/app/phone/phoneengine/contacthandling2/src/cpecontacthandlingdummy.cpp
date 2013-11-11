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
* Description:  This module contains the implementation of CPEContactHandlingDummy class
*
*/


//  INCLUDE FILES
#include "cpecontacthandlingdummy.h"
#include "cpecontactmatch.h"
#include <mpedatastore.h>
#include <e32std.h>
#include <mpephonemodelinternal.h>
#include <pepanic.pan>
#include <barsc.h> 
#include <barsread.h>
#include <talogger.h>
#include <bldvariant.hrh>
#include <featmgr.h>

// ================= MEMBER FUNCTIONS =======================================
// -----------------------------------------------------------------------------
// CPEContactHandlingDummy::CPEContactHandlingDummy
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPEContactHandlingDummy::CPEContactHandlingDummy()
    {
    }

// -----------------------------------------------------------------------------
// CPEContactHandlingDummy::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CPEContactHandlingDummy* CPEContactHandlingDummy::NewL()
    {
    TEFLOGSTRING(KTAOBJECT, "CNT CPEContactHandlingDummy::NewL start.");
    CPEContactHandlingDummy* self = new ( ELeave ) CPEContactHandlingDummy();
    TEFLOGSTRING(KTAOBJECT, "CNT CPEContactHandlingDummy::NewL Complete.");
    return self;
    }

// Destructor
CPEContactHandlingDummy::~CPEContactHandlingDummy()
    {
    TEFLOGSTRING( KTAOBJECT, "CNT CPEContactHandlingDummy::~CPEContactHandlingDummy()" );
    }

// -----------------------------------------------------------------------------
// CPEContactHandlingDummy::FindContactInfoSync
// Calls method FindContactInfoSyncL which can leave. This leave is trapped in 
// this function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CPEContactHandlingDummy::FindContactInfoSync( 
    const TInt /*aCallId*/, const TPEFindContactKey /*aFindKey*/ )     
    {
    TEFLOGSTRING( KTAOBJECT, "CNT CPEContactHandlingDummy::FindContactInfoSync()" );
    return KErrNone;
    }

// ================= OTHER EXPORTED FUNCTIONS ===============================

//  End of File 
