/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Standard proxy of the ECOM plugin
*
*/




/**
 * @internal reviewed 11/07/2007 by M Byrne
 */

#include <ecom/implementationproxy.h>
#include "glxdatasourcemds.h"
#include "glxdatasourcemds.hrh"

// ----------------------------------------------------------------------------
// The list of implementations
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    { IMPLEMENTATION_PROXY_ENTRY(KGlxDataSourceMdeImplementationUid,
                                    CGlxDataSourceMde::NewL) };

// ----------------------------------------------------------------------------
// The proxy of implementations
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

// End of File
