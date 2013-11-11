/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: ECOM plugin implementation table definitions.
*
*/

#include "ipssosaopluginheaders.h"

//--------------------------------
// Exported proxy for instantiation method resolution
// Use the interface UID and Implementation factory method
// as a pair for ECom instantiation.
//--------------------------------
const TImplementationProxy KIpsAOPlgImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY( 0x200100BA, CIpsSosAOPluginEComInterface::NewL )
	};

// ======== LOCAL FUNCTIONS ========

//--------------------------------
// ImplementationGroupProxy()
// Give the table to the caller
//--------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
    FUNC_LOG;
	aTableCount = sizeof(KIpsAOPlgImplementationTable) / sizeof(TImplementationProxy);
	return KIpsAOPlgImplementationTable;
	}

// End of file

