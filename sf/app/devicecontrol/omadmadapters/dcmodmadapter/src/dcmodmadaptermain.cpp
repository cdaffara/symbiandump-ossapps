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
* Description:  DCMO DM Adapter implementation
*
*/

// INCLUDE
#include <implementationproxy.h>
#include "dcmodmadapter.h"

// ----------------------------------------------------------------------------
// ImplementationTable
// Ecom plug-in implementation table
// Status : Draft
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
	IMPLEMENTATION_PROXY_ENTRY( 0x2001FD3F, CDcmoDMAdapter::NewL )
    };

// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// Instance of implementation proxy
// (exported)
// Status : Draft
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// End Of File
