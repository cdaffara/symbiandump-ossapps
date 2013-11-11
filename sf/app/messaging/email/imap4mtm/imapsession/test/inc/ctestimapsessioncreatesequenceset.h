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

#ifndef CTESTIMAPSESSIONCREATESEQUENCESET_H
#define CTESTIMAPSESSIONCREATESEQUENCESET_H

#include <test/tefunit.h>

/**
This test fixture tests the ability of CImapSession::CreateSequenceSetL() to generate
the correct sequence set string for a given set of message uids.
CImapSession::CreateSequenceSetL() is static and synchronous, 
so it does not need the full setup and teardown of the other CImapSession tests.
@internalTechnology
@prototype
*/
class CTestImapSessionCreateSequenceSet : public CTestFixture
// Note that all test suites must begin with "CTest"
// WARNING: Despite being a C class, CActiveTestFixture is NOT derrived from CBase
// ... so don't expect your data members to be zero'ed
	{
public:
	// Constructor & Destructor (explicitly virtual because CTestFixture is NOT derrived from CBase)
	CTestImapSessionCreateSequenceSet();
	virtual ~CTestImapSessionCreateSequenceSet();
	
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();
	
	// Tests
	void TestSingleValueL();
	void TestSingleShortRangeL();
	void TestSingleMediumRangeL();
	void TestSingleLongRangeL();
	void TestIndividualValuesL();
	void TestSingleValueFollowedByRangeL();
	void TestRangeFollowedBySingleValueL();
	void TestShortRangeSurroundedBySingleValuesL();
	void TestMediumRangeSurroundedBySingleValuesL();
	void TestSingleValueSurroundedByRangesL();
	void TestTwoRangesL();
	void TestThreeRangesL();
	void TestLongResultStringL();
	void TestOutOfOrderSingleRangeL();
	void TestOutOfOrderMixedValuesL();
	void TestDuplicatesSingleRangeL();
	void TestDuplicatesMixedValuesL();
	void TestDuplicatesAndOutOfOrderMixedValuesL();

	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	void DoTestL(TUint* aUids, TInt aCountUids, const TDesC8& aExpectedResult);

private:
	RArray<TUint> iUids;
	};
	
#endif CTESTIMAPSESSIONCREATESEQUENCESET_H
