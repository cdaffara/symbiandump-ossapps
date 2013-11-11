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
* Description:   Standard proxy of the ECOM plugin
*
*/



#include <implementationproxy.h>
#include "glxmedialiststestcollectionplugin.h"
#include "glxmedialiststestcollectionplugin.hrh"

// ----------------------------------------------------------------------------
// The list of implementations
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] =
    { IMPLEMENTATION_PROXY_ENTRY(KGlxMediaListsTestCollectionImplementationUid,
                                    CGlxMediaListsTestCollectionPlugin::NewL) };

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
