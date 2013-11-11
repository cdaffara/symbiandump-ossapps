/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CSConCSC implementation
*
*/


//
// This program creates a polymorphic interface DLL that is used
// to create capability object
//

// INCLUDE FILES

#include <e32def.h>
#include <e32uid.h>

#include "sconcsc.h"
#include "capinfo.h"
#include "debug.h"

// ============================= MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CreateCSConCSCL()
// Function to construct CSConCSC object. Note that this function
// is exported at ordinal 1 and is not a member of any class.
// -----------------------------------------------------------------------------
//
EXPORT_C CSConCSC* CreateCSConCSCL()
    {
    TRACE_FUNC;
    return CSConCSC::NewL();
    }


// -----------------------------------------------------------------------------
// CSConCSC::NewL()
// Two-phase constructor
// -----------------------------------------------------------------------------
//
CSConCSC* CSConCSC::NewL()
    {
    TRACE_FUNC_ENTRY;
    CSConCSC* self = new (ELeave) CSConCSC();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    TRACE_FUNC_EXIT;
    return self;
    }

// -----------------------------------------------------------------------------
// CSConCSC::~CSConCSC()
// Destructor
// -----------------------------------------------------------------------------
//
CSConCSC::~CSConCSC()
    {
    TRACE_FUNC_ENTRY;
    delete iCapInfo;
    TRACE_FUNC_EXIT;
    }
    
// -----------------------------------------------------------------------------
// CSConCSC::CapabilityObject(CBufFlat& aBuf)
// Creates a capability object data and sets data into aBuf.
// -----------------------------------------------------------------------------
//
TInt CSConCSC::CapabilityObject(CBufFlat& aBuf)
    {
    TRACE_FUNC_ENTRY;
    TRAPD(err, CSConCSC::CapabilityObjectL(aBuf));
    LOGGER_WRITE_1( "CSConCSC::CapabilityObject(CBufFlat& aBuf) : returned %d", err );
    return err;
    }

// -----------------------------------------------------------------------------
// CSConCSC::ConstructL()
// Initializes member data
// -----------------------------------------------------------------------------
//
void CSConCSC::ConstructL()
    {
    iCapInfo = CCapInfo::NewL();
    }

// -----------------------------------------------------------------------------
// CSConCSC::CSConCSC()
// Default constructor
// -----------------------------------------------------------------------------
//
CSConCSC::CSConCSC()
    {
    }

// -----------------------------------------------------------------------------
// CSConCSC::CapabilityObjectL(CBufFlat& aBuf)
// Creates a capability object data and sets data into aBuf.
// -----------------------------------------------------------------------------
//
void CSConCSC::CapabilityObjectL(CBufFlat& aBuf)
    {
    TRACE_FUNC_ENTRY;
    iCapInfo->CapabilityDocumentL( &aBuf );
    TRACE_FUNC_EXIT;
    }

// End of file

