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

#ifndef CTESTIMAPSESSION_H
#define CTESTIMAPSESSION_H

#include <test/tefunit.h>

// Forward Declarations
class CFakeInputStream;
class CFakeOutputStream;
class CActiveWaiter;
class CImapSession;

/**
This test fixture tests CImapSession's ability to deliver lines and literals to CImapCommand objects.
This is a white-box test, using the simple CImapLogout class to receive lines.
Set a breakpoint in CImapLogout::ParseMessageL() to ensure that the expected lines are being delivered.
@internalTechnology
@prototype
*/
class CTestImapSession : public CActiveTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapSession();
	virtual ~CTestImapSession();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();
	
	void SetupServerGreetingL();

	// Tests
	void TestOneLineL();
	void TestTwoLinesSimpleL();
	void TestTwoLinesSplitBeforeFirstCrlfL();
	void TestTwoLinesSplitAfterFirstCrlfL();
	void TestTwoLinesSplitOverFirstCrlfL();
	void TestTwoLinesSplitHeavilyOverFirstCrlfL();
	void TestTwoLinesInOneBlockL();
	void TestSevenLinesSimpleL();
	void TestSevenLinesIn3ChunksL();
	void TestOneLineFragmentedL();
	void TestOneLineHeavilyFragmentedL();

	void TestLoginSequenceL();
	void TestServerStateL();
	
	void TestLiteralL();
		
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	CFakeInputStream* iInputStream;
	CFakeOutputStream* iOutputStream;
	CActiveWaiter* iActiveWaiter;
	CImapSession* iImapSession;
	};
	
#endif CTESTIMAPSESSION_H
