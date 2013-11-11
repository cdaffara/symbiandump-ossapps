// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiIterator.h"
#include <calsession.h>
#include <caliterator.h>

_LIT(KExpectedCount,					"expectedcount");
_LIT(KAllocTest,						"alloctest");

_LIT(KInfoExpectedCount,				"Expected count %d");
_LIT(KInfoAllocTest,					"alloctest %d");
_LIT(KInfoFileName,						"FileName %S");
_LIT(KInfoIterateCalFileUid,			"Entry Found with UID:- %S");

_LIT(KErrIterateCalFileInvalidEntries,	"ERROR -- Entries in %S are %d, Entries Iterated %d");
_LIT(KErrIterateEmptyFileNoEntries,		"ERROR -- Invalid Error Returned %d");/*@}*/

CTestCalInterimApiIterator::CTestCalInterimApiIterator()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiIterator);
	}
	
TVerdict CTestCalInterimApiIterator::doTestStepL()
	{
	TBool			allocTest=EFalse;
	GetBoolFromConfig(ConfigSection(), KAllocTest, allocTest);
	INFO_PRINTF2(KInfoAllocTest, allocTest);

	if ( allocTest )
		{
		doAllocTestL();
		}
	else
		{
		IterateEntriesL();
		}

	return TestStepResult();
	}

	
/**
@SYMTestCaseID			PIM-CIT-CallnterimAPI-1098AP-13				 
@SYMTestType			CIT
@SYMTestPriority		Medium
@SYMPREQ				1098
@SYMFssID				3.2.13, 001, 002, 003, 005, 
@SYMTestCaseDesc		Fetch entries
@SYMTestActions			Create a CCalIter, and move through the entries
@SYMTestExpectedResults	The iteration should be move through the entries successfully
*/
void CTestCalInterimApiIterator::IterateEntriesL()
	{
	TInt	expectedCount=0;
	GetIntFromConfig(ConfigSection(), KExpectedCount, expectedCount);
	INFO_PRINTF2(KInfoExpectedCount, expectedCount);
	
	const TFileName&		fileName=GetCalenderFileName();
	INFO_PRINTF2(KInfoFileName, &fileName);
	TInt					count=0;
	CCalIter*				iter=CCalIter::NewL(GetSession());
	CleanupStack::PushL(iter);

	TPtrC8	uid(iter->FirstL());
	while (KNullDesC8()!=uid)
		{
		++count;
		TBuf<MAX_TEXT_MESSAGE>	iterateUid;
		iterateUid.Copy(uid);
		INFO_PRINTF2(KInfoIterateCalFileUid, &iterateUid);
		uid.Set(iter->NextL());
		}

	//make sure we have iterated through all entries
	if ( expectedCount!=count )
		{
		ERR_PRINTF4(KErrIterateCalFileInvalidEntries, &fileName, expectedCount, count);
		SetTestStepResult(EFail);
		}

	if( TestStepResult() == EPass )
		{
		TRAPD(err, uid.Set(iter->NextL()));

		TInt	expectedErr=KErrNone;
		if ( count==0 )
			{
			expectedErr=KErrCorrupt;
			}
		if ( err!=expectedErr )
			{
			ERR_PRINTF2(KErrIterateEmptyFileNoEntries, err);
			SetTestStepResult(EFail);
			}	
		}
	CleanupStack::PopAndDestroy(iter);
	}

void CTestCalInterimApiIterator::doAllocTestL()
	{
	INFO_PRINTF1(_L("Alloc test"));
	TInt failCount = 1;
	for( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD( err, IterateEntriesL());
		if ( err==KErrNone ) 
			{
			INFO_PRINTF1(_L("Memory allocation testing for iterators is done"));
			__UHEAP_RESET;
			break;
			}		
		if ( err != KErrNoMemory )
			SetTestStepResult(EFail);
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		failCount++;
		}
		INFO_PRINTF2(_L("FAILCOUNT %d"), failCount);
	}
	
