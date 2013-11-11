/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:
*
*/

#include <implementationproxy.h>

#include "irpresetimpl.h"
#include "iruid3.hrh"

// The implementation proxy table that lists all interfaces and 
// their implementations contained within this plugin.
const TImplementationProxy KIRPresetPluginImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( UID3_IRPRESETPLUGIN_DLL_IMPLEMENTATION_UID, CIRPresetImpl:: NewL) 
    };


// ---------------------------------------------------------------------------
// Entry point for ECOM plugins.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( KIRPresetPluginImplementationTable ) / 
                  sizeof(TImplementationProxy);
    return KIRPresetPluginImplementationTable;
    }
