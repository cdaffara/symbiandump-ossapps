/*
* ==============================================================================
*  Name        : dllmain.cpp
*  Part of     : 
*  Description : 
*  Version     : 
*
*  Copyright ?2004 Nokia Corporation.
*  This material, including documentation and any related
*  computer programs, is protected by copyright controlled by
*  Nokia Corporation. All rights are reserved. Copying,
*  including reproducing, storing, adapting or translating, any
*  or all of this material requires the prior written consent of
*  Nokia Corporation. This material also contains confidential
*  information which may not be disclosed to others without the
*  prior written consent of Nokia Corporation.
* ==============================================================================
*/
#include <e32std.h>
#include <implementationproxy.h>
#include "clcvtengine.h"

// Map the interface UIDs to implementation factory functions
const TImplementationProxy Implementations[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x20021342, CLcVtEngine::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
//
// Exported proxy for instantiation method resolution
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aCount )
    {
    aCount = sizeof( Implementations ) / sizeof( TImplementationProxy );
    return Implementations;
    }
