/*
* Copyright (c) 2007-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of the RFS ECOM plugin proxy
*
*/


#include <implementationproxy.h>

#include "psrfsplugin.h"

// The implementation proxy table that lists all interfaces and their implementations contained within this plugin.
const TImplementationProxy KPSRfsPluginImplementationTable[] =
    {
    /*lint -save -e611 (Warning -- Suspicious cast)*/
    IMPLEMENTATION_PROXY_ENTRY( 0x10282BB3, CPSRfsPlugin::NewL )
    /*lint -restore*/
    };

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

// ======== GLOBAL FUNCTIONS ========

// ---------------------------------------------------------------------------
// Entry point for ECOM plugins.
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( KPSRfsPluginImplementationTable ) / sizeof( TImplementationProxy );
    return KPSRfsPluginImplementationTable;
    }

