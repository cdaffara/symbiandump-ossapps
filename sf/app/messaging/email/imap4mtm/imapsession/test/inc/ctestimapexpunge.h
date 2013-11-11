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

#ifndef CTESTIMAPEXPUNGE_H
#define CTESTIMAPEXPUNGE_H

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include <test/tefunit.h>

// Forward Declarations
class CFakeInputStream;
class CFakeOutputStream;
class CActiveWaiter;
class CImapSession;
class CImapFolderInfo;

/**
This test fixture tests CImapExpunge.
@internalTechnology
@prototype
*/
class CTestImapExpunge : public CActiveTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapExpunge();
	virtual ~CTestImapExpunge();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestExpungeL();
			
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	CFakeInputStream* iInputStream;
	CFakeOutputStream* iOutputStream;
	CActiveWaiter* iActiveWaiter;
	CImapSession* iImapSession;
	};
	
#endif //CTESTIMAPEXPUNGE_H
