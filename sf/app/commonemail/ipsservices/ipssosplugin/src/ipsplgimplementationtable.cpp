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
* Description: ECOM proxy definitions.
*
*/


#include "emailtrace.h"
#include "ipsplgheaders.h"


//--------------------------------
// Exported proxy for instantiation method resolution
// Use the interface UID and Implementation factory method
// as a pair for ECom instantiation.
//--------------------------------
const TImplementationProxy KIpsPlgImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY( 0x2000E53F, CIpsPlgImap4Plugin::NewL ),
	IMPLEMENTATION_PROXY_ENTRY( 0x2000E53E, CIpsPlgPop3Plugin::NewL )
	};

// ======== LOCAL FUNCTIONS ========

//--------------------------------
// ImplementationGroupProxy()
// Give the table to the caller
//--------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
    FUNC_LOG;
	aTableCount = sizeof(KIpsPlgImplementationTable) / sizeof(TImplementationProxy);
	return KIpsPlgImplementationTable;
	}







