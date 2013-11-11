/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements the import functionality of CAgnVersit2
*
*/



#include "AgnVersit2.h"				//CAgnVersit2
#include <ecom/implementationproxy.h>	//TImplementationProxy

//debug
#include "calendarengines_debug.h"

/**
Defines the pairing required by the ECOM framework to correctly identify the
instantiation method pointer for client's request.
@publishedAll
*/
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10206C80, CAgnVersit2::NewL)
	};

/**
Returns the TImplementationProxy for AgnVersit2
@publishedAll
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	TRACE_ENTRY_POINT;
	
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);

	TRACE_EXIT_POINT;
	return ImplementationTable;
	}
	
// End of file
