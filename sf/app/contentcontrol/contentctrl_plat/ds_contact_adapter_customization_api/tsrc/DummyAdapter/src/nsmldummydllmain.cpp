/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  DS Dummy adapter entry point
*
*/

#include <ImplementationProxy.h>
#include "nsmldebug.h"
#include "nsmlDummydataprovider.h"

// -----------------------------------------------------------------------------
// ImplementationGroupProxy array
// -----------------------------------------------------------------------------
//
#ifndef IMPLEMENTATION_PROXY_ENTRY
#define IMPLEMENTATION_PROXY_ENTRY( aUid, aFuncPtr ) {{aUid},(aFuncPtr)}
#endif

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( KNSmlDummyAdapterImplUid, CNSmlDummyDataProvider::NewL )
    };

// -----------------------------------------------------------------------------
// ImplementationGroupProxy
// -----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    _DBG_FILE("ImplementationGroupProxy() for CNSmlDummyDataProvider: begin");

    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    _DBG_FILE("ImplementationGroupProxy() for CNSmlDummyDataProvider: end");
    return ImplementationTable;
    }
