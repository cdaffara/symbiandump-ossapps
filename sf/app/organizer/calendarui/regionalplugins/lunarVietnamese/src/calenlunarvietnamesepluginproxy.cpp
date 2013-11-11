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

#include "CalenLunarVietnamesePlugin.h"
#include "CalenRegionalPluginUids.h"


// ----------------------------------------------------------------------------
// ImplementationTable
// ----------------------------------------------------------------------------
const TImplementationProxy ImplementationTable[] =
    {
    IMPLEMENTATION_PROXY_ENTRY( CALENLUNARPLUGIN_VIETNAMESE_IMPLEMENTATION_UID,
                                CCalenLunarVietnamesePlugin::NewL )
    };


// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// ----------------------------------------------------------------------------
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
    {
    aTableCount = sizeof(ImplementationTable) / sizeof (TImplementationProxy);
    return ImplementationTable;
    }

// ----------------------------------------------------------------------------
// End of file
// ----------------------------------------------------------------------------



