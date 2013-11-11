/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This module contains the implementation of Contact Hanling 
*                panic function
*
*/


// INCLUDE FILES
#include <pepanic.pan>
#include "e32std.h"
// EXTERNAL DATA STRUCTURES
//None

// EXTERNAL FUNCTION PROTOTYPES  
//None

// CONSTANTS
//None

// MACROS
//None

// LOCAL CONSTANTS AND MACROS
//None

// MODULE DATA STRUCTURES
//None

// LOCAL FUNCTION PROTOTYPES
//None

// FORWARD DECLARATIONS
//None


// ==================== LOCAL FUNCTIONS =====================================
// None

// ================= MEMBER FUNCTIONS =======================================
// None

// ================= OTHER EXPORTED FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Panic implements...
// Contact Handling panic function
// Returns: None
// -----------------------------------------------------------------------------
//
GLDEF_C void Panic
        ( 
        TPEPanic aPanic 
        )
    {
    _LIT(KPEContactHandlingPanic,"ContactHandling Panic!");
    User::Panic( KPEContactHandlingPanic, aPanic);
    }

// End of File
