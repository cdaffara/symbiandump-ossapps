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
* Description:  CSP Ecom proxyfile        
*
*/



#include <e32base.h>
#include <implementationproxy.h>

#include "csprovider.h"
#include "csplogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// TImplementationProxy::ImplementationTable
// ---------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x102828E5, CSProvider::NewL )
    };

// ---------------------------------------------------------------------------
// TImplementationProxy::ImplementationGroupProxy
// ---------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
                                                        TInt& aTableCount )
    {
    CSPLOGSTRING(CSPOBJECT, "CSPPROXY - A");
    aTableCount = 
        sizeof( ImplementationTable ) / sizeof( TImplementationProxy );
    CSPLOGSTRING(CSPOBJECT, "CSPPROXY - B");
    return ImplementationTable;
    }

//  End of File  
