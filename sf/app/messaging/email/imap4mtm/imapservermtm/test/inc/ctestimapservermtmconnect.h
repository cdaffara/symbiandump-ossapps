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

#ifndef __CTESTIMAPSERVERMTMCONNECT_H__
#define __CTESTIMAPSERVERMTMCONNECT_H__

#include "ctestimapservermtmbase.h"

// Note that all test suites must begin with "CTest"
class CTestImapServerMtmConnect : public CTestImapServerMtmBase
	{
public:
	CTestImapServerMtmConnect( );
	virtual ~CTestImapServerMtmConnect( );

	// Tests
	void TestConnectL( );
	void TestConnectWhenConnectedL( );
	void TestConnectWithBadLoginL( );
	void TestConnectWithBadPasswordL( );
	
	static CTestSuite* CreateSuiteL( const TDesC& aName );

	};
	
#endif //__CTESTIMAPSERVERMTMCONNECT_H__
