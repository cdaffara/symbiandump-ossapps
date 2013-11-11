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
* Description:  This file contains the header file of the class NetworkHandlingProxy.
*
*/

#include <smcmockclassincludes.h>
#include <networkhandlingproxy.h>
#include <nwhandlingengine.h> 

// ============================ MEMBER FUNCTIONS ===============================

CNWSession* CreateL( MNWMessageObserver& aMessage, TNWInfo& aTNWInfo )
    {
    SMC_MOCK_METHOD2(CNWSession*, 
        MNWMessageObserver&, aMessage, 
        TNWInfo&, aTNWInfo);
    }
