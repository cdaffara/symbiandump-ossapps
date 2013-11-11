/*
* Copyright (c) 2007, 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Header file for Presence Cache Server
*
*/


#ifndef PRESENCECACHESECURITYPOLICY_H
#define PRESENCECACHESECURITYPOLICY_H

//  INCLUDES
#include "presencecachedefs2.h"  

// DATA TYPES

// Server Security Policy Definition
const TUint KCacheServerRangeCount = 4;

const TInt KCacheServerRanges[KCacheServerRangeCount] = 
	{
	0,              // Read
	NRequest::EWritePresenceInfoSync,      // write
	NRequest::ENoRequestMade,   // read
	NRequest::EWritePresenceInfoAsync // write
	};

// Mapping IPCs to policy element
const TUint8 KCacheServerElementsIndex[KCacheServerRangeCount] = 
	{
    0,
    1,
    0,
    1 
	};
			
// Individual policy elements 
const CPolicyServer::TPolicyElement KCacheServerElements[] = 
	{
    {_INIT_SECURITY_POLICY_C1(ECapabilityReadUserData), CPolicyServer::EFailClient},
    {_INIT_SECURITY_POLICY_C1(ECapabilityWriteUserData), CPolicyServer::EFailClient},    
	};

// Main policy
const CPolicyServer::TPolicy KCacheServerPolicy = 
	{
	CPolicyServer::EAlwaysPass,
	KCacheServerRangeCount,
	KCacheServerRanges,
	KCacheServerElementsIndex,
	KCacheServerElements,
	};
	
#endif // PRESENCECACHESECURITYPOLICY_H
