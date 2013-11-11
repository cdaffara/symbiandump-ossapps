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

#ifndef CTESTIMAPIDLE_H
#define CTESTIMAPIDLE_H

#include <test/tefunit.h>

#include "cactivewaiter.h"


// Forward Declarations
class CFakeInputStream;
class CFakeOutputStream;
class CImapSession;

/**
This test fixture tests CImapIdle.
@internalTechnology
@prototype
*/
class CTestImapIdle : public CActiveTestFixture
					, public MActiveWaiterObserver
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapIdle();
	virtual ~CTestImapIdle();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();

	// Tests
	void TestIdleEnterDoneL();
	void TestIdleEnterDoneFlatL();
	void TestIdleEnterWaitDoneL();
	void TestIdleEnterWaitCancelDoneL();
	void TestIdleEnterWaitCancelDataDoneL();
	void TestIdleSequenceL();
	void TestIdleSequenceWithCancelL();
	void TestIdleSequenceWithCancelAndFlatL();
			
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
	// Implement MActiveWaiterObserver
	void DoCancel();
	
private:
	CFakeInputStream* iInputStream;
	CFakeOutputStream* iOutputStream;
	CActiveWaiter* iActiveWaiter;
	CImapSession* iImapSession;
	};
	
#endif // CTESTIMAPIDLE_H
