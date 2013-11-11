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
// ctestimapservermtmiscancelbackgsync.h
// 
//

#ifndef __CTESTIMAPSERVERMTMCANCELBACKGSYNC_H__
#define __CTESTIMAPSERVERMTMCANCELBACKGSYNC_H__

#include "ctestimapservermtmbase.h"

// Note that all test suites must begin with "CTest"
class CTestImapServerMtmCancelBackgSync : public CTestImapServerMtmBase
	{
public:
	CTestImapServerMtmCancelBackgSync( );
	virtual ~CTestImapServerMtmCancelBackgSync( );

	// Tests
	void TestCancelBackgSyncL( );
	
	static CTestSuite* CreateSuiteL( const TDesC& aName );

	};
	
#endif //__CTESTIMAPSERVERMTMCANCELBACKGSYNC_H__