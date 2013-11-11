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
* Description:
*
*/
//debug
#include "calendarui_debug.h"

#include "CalenThaiPluginUids.hrh"
#include "CalenThaiPlugin.h"
#include <e32std.h>
#include <ecom/implementationproxy.h>

const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( CALENTHAIPLUGIN_IMPLEMENTATION_UID,
                                CCalenThaiPlugin::NewL )
    };

// This should be only exported function of ECom plugin DLL 
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( TInt& aTableCount )
    {
    TRACE_ENTRY_POINT;
    
    aTableCount = sizeof( ImplementationTable) / sizeof(TImplementationProxy);
    
    TRACE_EXIT_POINT;
    return ImplementationTable;
    }
