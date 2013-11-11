/*
 * Copyright (c) 2000 Nokia Corporation and/or its subsidiary(-ies). 
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
 * Description: Implementation of applicationmanagement components
 *
 */

#ifndef __PLATFORM_SECURITY_POLICIES__
#define __PLATFORM_SECURITY_POLICIES__

// INCLUDES

#include <e32base.h>
#include "ApplicationManagementClientServer.h"

// CONSTANTS

const TUint KApplicationManagementRangeCount = 3;

const TInt KApplicationManagementRanges[KApplicationManagementRangeCount] =
    {

    NApplicationManagement::EAddTrust, //For Management
            NApplicationManagement::EPerformRfs,
            NApplicationManagement::ELast
,//ENotSupported
};

const TUint8
        KApplicationManagementSecurityElementsIndex[KApplicationManagementRangeCount] =
            {
            0, //applies to 1st range 
                    1,
                    CPolicyServer::ENotSupported
            };

//capability checks
const CPolicyServer::TPolicyElement KApplicationManagementSecurityElements[] =
    {
        {
        _INIT_SECURITY_POLICY_C1(ECapabilityTrustedUI), CPolicyServer::EFailClient
        },
        {
        _INIT_SECURITY_POLICY_S1(0x101f9a02, ECapabilityDiskAdmin), CPolicyServer::EFailClient
        }
    };

//Package all the above together into a policy
//CPolicyServer::TPolicy KApplicationManagementSecurityPolicy;

#endif

//  End of File
