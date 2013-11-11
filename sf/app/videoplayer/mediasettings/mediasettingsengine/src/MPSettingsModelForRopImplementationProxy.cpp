/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation group proxy for ROP specific media settings model*
*/


// Version : %version: 3 %




// INCLUDE FILES
#include    <e32std.h>
#include    <implementationproxy.h>
#include    "MPSettEngPluginImplementationUIDs.hrh"
#include    "MPSettingsModelForROP.h"


// CONSTANTS
// Table of available ECOM plugin implementations for CMPSettingsModel in this DLL.
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY(KMPSettEngImplUidROP, CMPSettingsModelForROP::NewL)
    };


// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// ImplementationProxy
// ECOM implementation table entry.
// 
// Returns: Table of available ECOM plugin implementations for CMPSettingsModel
// in this DLL.
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

    return ImplementationTable;
    }

//  End of File  
