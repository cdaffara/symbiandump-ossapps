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
* Description:   Calendar Lunar Plugin 
*
*/



#include <implementationproxy.h>

#include "calencustomisation.h"
#include "CalenLunarChinesePlugin.h"
#include "CalenRegionalPluginUids.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CalenLunarChinesePluginProxyTraces.h"
#endif



// ----------------------------------------------------------------------------
// ImplementationTable
// ----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( CALENLUNARPLUGIN_CHINESE_IMPLEMENTATION_UID,
                                CCalenLunarChinesePlugin::NewL )
    
    };


// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// ----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    OstTraceFunctionEntry0( _IMPLEMENTATIONGROUPPROXY_ENTRY );
    aTableCount = sizeof(ImplementationTable) / sizeof (TImplementationProxy);
    OstTraceFunctionExit0( _IMPLEMENTATIONGROUPPROXY_EXIT );
    return ImplementationTable;
    }

// ----------------------------------------------------------------------------
// End of file
// ----------------------------------------------------------------------------



