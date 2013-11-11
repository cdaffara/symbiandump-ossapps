// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

/**
 @file
 @internalTechnology
*/
#ifndef PBAPSERVERSECURITYPOLICY_H
#define PBAPSERVERSECURITYPOLICY_H


const TUint KPbapServerRangeCount = 3;

const TInt KPbapServerRanges[KPbapServerRangeCount] = 
	{
	0,		//range is 0-2 inclusive
	3,		//range is 3-6 inclusive
	7,		//range is 7-KMaxTInt inclusive
	};

const TUint8 KPbapServerElementsIndex[KPbapServerRangeCount] = 
	{
	0,
	1,
	CPolicyServer::ENotSupported
	};

const CPolicyServer::TPolicyElement KPbapServerElements[] = 
	{
	{ _INIT_SECURITY_POLICY_C2( ECapabilityLocalServices, ECapabilityReadUserData), CPolicyServer::EFailClient},	// policy 0:  range 0 - 2
	{ _INIT_SECURITY_POLICY_C1( ECapability_None), CPolicyServer::EFailClient}	// policy 1:  range 3 - 6
	};


const CPolicyServer::TPolicy KPbapServerPolicy = 
	{
	CPolicyServer::EAlwaysPass,
	KPbapServerRangeCount,
	KPbapServerRanges,
	KPbapServerElementsIndex,
	KPbapServerElements
	};


#endif	//PBAPSERVERSECURITYPOLICY_H
