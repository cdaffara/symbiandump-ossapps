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
* Description: mmstransport implementation
*
*/



// INCLUDE FILES
#include <e32std.h>
#include "mmsconst.h"

// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// ==================== LOCAL FUNCTIONS ====================
// ================= OTHER EXPORTED FUNCTIONS ==============

// ---------------------------------------------------------
// gPanic implements
// panic function, should be used by debug version only
//
GLDEF_C void gPanic(
    TMmsPanic aPanic ) // error number enumerations
    {
    _LIT(KMmsPanic,"MMS");
    User::Panic( KMmsPanic, aPanic );
    }

//  End of File  

