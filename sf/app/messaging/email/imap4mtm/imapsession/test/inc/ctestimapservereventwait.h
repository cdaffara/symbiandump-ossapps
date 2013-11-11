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

#ifndef CTESTIMAPSERVEREVENTWAIT_H
#define CTESTIMAPSERVEREVENTWAIT_H

#include <test/tefunit.h>

#include "cfakeinputstream.h"
#include "cactivewaiter.h"

// Forward Declarations
class CFakeOutputStream;
class CImapSession;

/**
This test fixture tests CTestImapServerEventWait.
@internalTechnology
@prototype
*/
class CTestImapServerEventWait  : public CActiveTestFixture
								, public MFakeInputStreamIsEmptyObserver
								, public MActiveWaiterObserver
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapServerEventWait();
	virtual ~CTestImapServerEventWait();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();
	
	// Tests
	void TestSimpleL();
	void TestTwoEventsL();
	void TestCancelAfterNoEventL();
	void TestCancelAfterWholeEventL();
	void TestCancelDuringEventL();
		
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
	// Implement MFakeInputStreamIsEmptyObserver
	void OnInputStreamIsEmptyL();
	
	// Implement MActiveWaiterObserver
	void DoCancel();
	
private:
	CFakeInputStream* iInputStream;
	CFakeOutputStream* iOutputStream;
	CActiveWaiter* iActiveWaiter;
	CImapSession* iImapSession;
	
	enum 
		{
		ENotifyOpCompleteXLiteralAndCancel
		} 
		iInputStreamNotifyOp;
	};
	
#endif // CTESTIMAPSERVEREVENTWAIT_H
