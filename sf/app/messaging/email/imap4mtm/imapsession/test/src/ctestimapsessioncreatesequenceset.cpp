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

#include "ctestimapsessioncreatesequenceset.h"

#include "cimapsession.h"

CTestImapSessionCreateSequenceSet::CTestImapSessionCreateSequenceSet()
	{}

CTestImapSessionCreateSequenceSet::~CTestImapSessionCreateSequenceSet()
	{
	iUids.Reset();
	}

void CTestImapSessionCreateSequenceSet::SetupL()
	{}
	
void CTestImapSessionCreateSequenceSet::TearDownL()
	{
	iUids.Reset();
	}

/**
3 ==> 3
*/
void CTestImapSessionCreateSequenceSet::TestSingleValueL()
	{
	INFO_PRINTF1(_L("TestSingleValueL: 3 ==> 3"));
	
	iUids.AppendL(3);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "3");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
1 2	==> 1:2
*/
void CTestImapSessionCreateSequenceSet::TestSingleShortRangeL()
{	
	INFO_PRINTF1(_L("TestSingleShortRangeL: 1 2	==> 1:2"));
		
	iUids.AppendL(1);
	iUids.AppendL(2);
		
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "1:2");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
1 2 3 4 5 ==> 1:5
*/
void CTestImapSessionCreateSequenceSet::TestSingleMediumRangeL()
{	
	INFO_PRINTF1(_L("TestSingleMediumRangeL: 1 2 3 4 5 ==> 1:5"));
		
	iUids.AppendL(1);
	iUids.AppendL(2);
	iUids.AppendL(3);
	iUids.AppendL(4);
	iUids.AppendL(5);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "1:5");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
1 2 3 4 5 ... 100 ==> 1:100
*/
void CTestImapSessionCreateSequenceSet::TestSingleLongRangeL()
{	
	INFO_PRINTF1(_L("TestSingleLongRangeL: 1 2 3 4 5 ... 100 ==> 1:100"));
		
	for (TUint i=1; i<=100; i++)
		{
		iUids.AppendL(i);
		}
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "1:100");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
1 3 5 7 ==> 1,3,5,7
*/
void CTestImapSessionCreateSequenceSet::TestIndividualValuesL()
{	
	INFO_PRINTF1(_L("TestIndividualValuesL: 1 3 5 7 ==> 1,3,5,7"));
		
	iUids.AppendL(1);
	iUids.AppendL(3);
	iUids.AppendL(5);
	iUids.AppendL(7);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "1,3,5,7");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
5 8 9 10 11 ==> 5,8:11
*/
void CTestImapSessionCreateSequenceSet::TestSingleValueFollowedByRangeL()
{	
	INFO_PRINTF1(_L("TestSingleValueFollowedByRangeL: 5 8 9 10 11 ==> 5,8:11"));
		
	iUids.AppendL(5);
	iUids.AppendL(8);
	iUids.AppendL(9);
	iUids.AppendL(10);
	iUids.AppendL(11);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "5,8:11");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
This tests the example given in the insource documentation for CreateSequenceSetLC
3 4 5 6 21 ==> 3:6,21
*/
void CTestImapSessionCreateSequenceSet::TestRangeFollowedBySingleValueL()
	{	
	INFO_PRINTF1(_L("TestRangeFollowedBySingleValueL: 3 4 5 6 21"));
		
	iUids.AppendL(3);
	iUids.AppendL(4);
	iUids.AppendL(5);
	iUids.AppendL(6);
	iUids.AppendL(21);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "3:6,21");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
1 3 4 6 ==> 1,3:4,6
*/
void CTestImapSessionCreateSequenceSet::TestShortRangeSurroundedBySingleValuesL()
{	
	INFO_PRINTF1(_L("TestShortRangeSurroundedBySingleValuesL: 1 3 4 6 ==> 1,3:4,6"));
		
	iUids.AppendL(1);
	iUids.AppendL(3);
	iUids.AppendL(4);
	iUids.AppendL(6);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "1,3:4,6");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
1 3 4 5 6 10 ==> 1,3:6,10
*/
void CTestImapSessionCreateSequenceSet::TestMediumRangeSurroundedBySingleValuesL()
{	
	INFO_PRINTF1(_L("TestMediumRangeSurroundedBySingleValuesL: 1 3 4 5 6 10 ==> 1,3:6,10"));
		
	iUids.AppendL(1);
	iUids.AppendL(3);
	iUids.AppendL(4);
	iUids.AppendL(5);
	iUids.AppendL(6);
	iUids.AppendL(10);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "1,3:6,10");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
5 6 7 8 11 15 16 17 18 ==> 5:8,11,15:18
*/
void CTestImapSessionCreateSequenceSet::TestSingleValueSurroundedByRangesL()
{	
	INFO_PRINTF1(_L("TestSingleValueSurroundedByRangesL: 5 6 7 8 11 15 16 17 18 ==> 5:8,11,15:18"));
		
	TUint uids[] = { 5,6,7,8,11,15,16,17,18 };
	TInt countUids = sizeof(uids) / sizeof(TUint);
	
	DoTestL(uids, countUids, _L8("5:8,11,15:18"));

	INFO_PRINTF1(_L("Complete"));
	}

/**
10 11 12 15 16 ==> 10:12,15:16
*/
void CTestImapSessionCreateSequenceSet::TestTwoRangesL()
{	
	INFO_PRINTF1(_L("TestTwoRangesL: 10 11 12 15 16 ==> 10:12,15:16"));
		
	iUids.AppendL(10);
	iUids.AppendL(11);
	iUids.AppendL(12);
	iUids.AppendL(15);
	iUids.AppendL(16);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "10:12,15:16");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
1 2 3 5 6 7 10 11 12 13 ==> 1:3,5:7,10:13
*/
void CTestImapSessionCreateSequenceSet::TestThreeRangesL()
{	
	INFO_PRINTF1(_L("TestThreeRangesL: 1 2 3 5 6 7 10 11 12 13 ==> 1:3,5:7,10:13"));
	
	TUint uids[] = { 1,2,3,5,6,7,10,11,12,13 };
	TInt countUids = sizeof(uids) / sizeof(TUint);
	
	DoTestL(uids, countUids, _L8("1:3,5:7,10:13"));

	INFO_PRINTF1(_L("Complete"));
	}

/**
2,4,6,8,10,...,200 ==> 2,4,6,8,10,...,200

CImapSession::CreateSequenceSetLC() uses an expandable buffer to build the
sequence set string.  This test checks that the buffer does indeed expand
when required.
*/
void CTestImapSessionCreateSequenceSet::TestLongResultStringL()
{	
	INFO_PRINTF1(_L("TestLongResultStringL: 2,4,6,8,10,...,200 ==> 2,4,6,8,10,...,200"));
	
	TBuf8<400> bufExpectedResult;
	
	iUids.AppendL(2);
	bufExpectedResult.Append(_L8("2"));
	
	for (TUint i=2; i<=100; i++)
		{
		iUids.AppendL(i*2);
		bufExpectedResult.AppendFormat(_L8(",%d"), i*2);
		}
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	ASSERT_EQUALS((TDesC8&)*sequenceSet, (TDesC8&)bufExpectedResult);

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
10 11 12 15 16 ==> 10:12,15:16
*/
void CTestImapSessionCreateSequenceSet::TestOutOfOrderSingleRangeL()
{	
	INFO_PRINTF1(_L("TestOutOfOrderSingleRangeL: 10 11 12 15 16 ==> 10:12,15:16"));
		
	iUids.AppendL(10);
	iUids.AppendL(11);
	iUids.AppendL(12);
	iUids.AppendL(15);
	iUids.AppendL(16);
	
	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	
	_LIT8(KCompare, "10:12,15:16");
	ASSERT_EQUALS((TDesC8&)*sequenceSet, KCompare());

	CleanupStack::PopAndDestroy(sequenceSet);
	INFO_PRINTF1(_L("Complete"));
	}

/**
16 10 3 7 14 5 15 4 12 13 6  ==> 3:7,12:16
*/
void CTestImapSessionCreateSequenceSet::TestOutOfOrderMixedValuesL()
{	
	INFO_PRINTF1(_L("TestOutOfOrderMixedValuesL: 16 3 7 14 5 15 4 12 13 6  ==> 3:7,10,12:16"));
		
	TUint uids[] = { 16,10,3,7,14,5,15,4,12,13,6 };
	TInt countUids = sizeof(uids) / sizeof(TUint);
	
	DoTestL(uids, countUids, _L8("3:7,10,12:16"));

	INFO_PRINTF1(_L("Complete"));
	}

/**
3 3 4 5 5 5 6 6 7 7 7 7 ==> 3:7
*/
void CTestImapSessionCreateSequenceSet::TestDuplicatesSingleRangeL()
{	
	INFO_PRINTF1(_L("TestDuplicatesSingleRangeL: 3 3 4 5 5 5 6 6 7 7 7 7 ==> 3:7"));
		
	TUint uids[] = { 3,3,4,5,5,5,6,6,7,7,7,7 };
	TInt countUids = sizeof(uids) / sizeof(TUint);
	
	DoTestL(uids, countUids, _L8("3:7"));

	INFO_PRINTF1(_L("Complete"));
	}

/**
12 12 15 15 16 20 21 21 22 22 30 30 30 ==> 12,15:16,20:22:30
*/
void CTestImapSessionCreateSequenceSet::TestDuplicatesMixedValuesL()
{	
	INFO_PRINTF1(_L("TestDuplicatesMixedValuesL: 12 12 15 15 16 20 21 21 22 22 30 30 30 ==> 12,15:16,20:22:30"));

	TUint uids[] = { 12,12,15,15,16,20,21,21,22,22,30,30,30 };
	TInt countUids = sizeof(uids) / sizeof(TUint);
	
	DoTestL(uids, countUids, _L8("12,15:16,20:22,30"));

	INFO_PRINTF1(_L("Complete"));
	}
	
/**
3 7 16 20 10 11 10 15 11 2 2 1 20 3 4 7 10 2 14 20 15 ==> 1:4,7,10:11,14:16,20
*/
void CTestImapSessionCreateSequenceSet::TestDuplicatesAndOutOfOrderMixedValuesL()
{	
	INFO_PRINTF1(_L("TestDuplicatesAndOutOfOrderMixedValuesL: 3 7 16 20 10 11 10 15 11 2 2 1 20 3 4 7 10 2 14 20 15 ==> 1:4,7,10:11,14:16,20"));

	TUint uids[] = { 3,7,16,20,10,11,10,15,11,2,2,1,20,3,4,7,10,2,14,20,15 };
	TInt countUids = sizeof(uids) / sizeof(TUint);
	
	DoTestL(uids, countUids, _L8("1:4,7,10:11,14:16,20"));

	INFO_PRINTF1(_L("Complete"));
	}


void CTestImapSessionCreateSequenceSet::DoTestL(TUint* aUids, TInt aCountUids, const TDesC8& aExpectedResult)
	{
	for (TInt i = 0; i < aCountUids; i++)
		{
		iUids.AppendL(aUids[i]);
		}

	HBufC8* sequenceSet = CImapSession::CreateSequenceSetLC(iUids);
	ASSERT_EQUALS((TDesC8&)*sequenceSet, aExpectedResult);	
	
	CleanupStack::PopAndDestroy(sequenceSet);
	}

	
CTestSuite* CTestImapSessionCreateSequenceSet::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_TEST_STEP(TestSingleValueL);
	ADD_TEST_STEP(TestSingleShortRangeL);
	ADD_TEST_STEP(TestSingleMediumRangeL);
	ADD_TEST_STEP(TestSingleLongRangeL);
	ADD_TEST_STEP(TestIndividualValuesL);
	ADD_TEST_STEP(TestSingleValueFollowedByRangeL);
	ADD_TEST_STEP(TestRangeFollowedBySingleValueL);
	ADD_TEST_STEP(TestShortRangeSurroundedBySingleValuesL);
	ADD_TEST_STEP(TestMediumRangeSurroundedBySingleValuesL);
	ADD_TEST_STEP(TestSingleValueSurroundedByRangesL);
	ADD_TEST_STEP(TestTwoRangesL);
	ADD_TEST_STEP(TestThreeRangesL);
	ADD_TEST_STEP(TestLongResultStringL);
	ADD_TEST_STEP(TestOutOfOrderSingleRangeL);
	ADD_TEST_STEP(TestOutOfOrderMixedValuesL);
	ADD_TEST_STEP(TestDuplicatesSingleRangeL);
	ADD_TEST_STEP(TestDuplicatesMixedValuesL);
	ADD_TEST_STEP(TestDuplicatesAndOutOfOrderMixedValuesL);
	END_SUITE;
	}

