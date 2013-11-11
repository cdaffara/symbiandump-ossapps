/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*
*/

#include <e32std.h>
#include <implementationproxy.h>
#include <cindexingplugin.h>
#include "ccalendarplugin.h"


// Map the interface UIDs to implementation factory functions
const TImplementationProxy KImplementationTable[] = 
	{
	{{0x2001F704},( TProxyNewLPtr) CCalendarPlugin::NewL}		
	};

// Exported proxy for instantiation method resolution
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
	{
	aTableCount = sizeof( KImplementationTable ) / sizeof( TImplementationProxy );
	return KImplementationTable;
	}

