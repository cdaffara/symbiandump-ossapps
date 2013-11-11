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

#ifndef __CTESTIMAPSERVERMTMCOPYTOLOCAL_H__
#define __CTESTIMAPSERVERMTMCOPYTOLOCAL_H__

#include "ctestimapservermtmbase.h"

// Note that all test suites must begin with "CTest"
class CTestImapServerMtmCopyToLocal : public CTestImapServerMtmBase
	{
public:
	CTestImapServerMtmCopyToLocal( );
	virtual ~CTestImapServerMtmCopyToLocal( );

	// Tests
	void TestCopyToLocalL( );
	
	static CTestSuite* CreateSuiteL( const TDesC& aName );

	};
	
#endif //__CTESTIMAPSERVERMTMCOPYTOLOCAL_H__