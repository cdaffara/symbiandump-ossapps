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

#ifndef __CTESTIMAPCOMMAND_H__
#define __CTESTIMAPCOMMAND_H__

#include <test/tefunit.h>

// Forward Declarations
class CFakeInputStream;
class CFakeOutputStream;
class CActiveWaiter;
class CImapSession;

/**
This test fixture tests CImapCommand's base class functionality, including
- its ability to handle unsolicited responses.
- its ability to encode mailbox names.
@internalTechnology
@prototype
*/
class CTestImapCommand : public CActiveTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapCommand();
	virtual ~CTestImapCommand();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();
	
	void SetupServerGreetingL();

	// Tests
	void TestTaggedOkOnlyL();
	void TestTaggedNoL();
	void TestTaggedBadL();
	void TestKnownThenTaggedOkL();
	void TestUnilateralThenTaggedOkL();
	void TestUnknownLineThenTaggedOkL();
	void TestUnknownLiteralThenTaggedOkL();
	void TestUnknownMultiLiteralThenTaggedOkL();
	void TestEncodeMailboxsL();
	
	static CTestSuite* CreateSuiteL(const TDesC& aName);
private:
	void DoTestEncodeMailboxL(const TDesC16& aUnicode, const TDesC8& aExpectedEncoding);

private:
	CFakeInputStream* iInputStream;
	CFakeOutputStream* iOutputStream;
	CActiveWaiter* iActiveWaiter;
	CImapSession* iImapSession;
	};
	
#endif __CTESTIMAPCOMMAND_H__
