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

#include "TestCalInterimApiFileAccess.h"
#include <calsession.h>

_LIT(KInfoListingAllAgendaFiles,	"Listing All Agenda Files");
_LIT(KAllocTest,					"alloctest");
_LIT(KErrListingAllAgendaFiles,		"ERROR -- No Agenda Files Found");

CTestCalInterimApiFileAccess::CTestCalInterimApiFileAccess()
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiFileAccess);
	}

TVerdict CTestCalInterimApiFileAccess::doTestStepL()
	{
	if ( TestStepResult()==EPass )
		{
		TBool allocTest;
		GetBoolFromConfig(ConfigSection(), KAllocTest,	allocTest);
		if( allocTest )
			{
			doAllocTestL();
			}
		else
			{
			ListsAllAgendaFilesL();	
			}
		}

	return TestStepResult();
	}
	
/**
@SYMTestCaseID			PIM-APP-ENGINES-CALINTERIMAPI-CIT-0006-HP
@SYMTestType			CIT
@SYMTestPriority		Medium
@SYMPREQ				1098
@SYMFssID				3.2.1 006
@SYMTestCaseDesc		Lists all calendar  files
@SYMTestActions			List all aganda files
@SYMTestExpectedResults	Return all aganda files
*/
void CTestCalInterimApiFileAccess::ListsAllAgendaFilesL()
	{
	INFO_PRINTF1(KInfoListingAllAgendaFiles);
	CDesCArray* 	agendaFiles(GetSession().ListCalFilesL());
	CleanupStack::PushL(agendaFiles);
	TInt			count;
	for( count=0 ; count<agendaFiles->MdcaCount(); count++ )
		{
		INFO_PRINTF1(agendaFiles->MdcaPoint(count));
		}
	CleanupStack::PopAndDestroy(agendaFiles);
	if ( count==0 )
		{
		ERR_PRINTF1(KErrListingAllAgendaFiles);
		SetTestStepResult(EFail);
		}
	else
		{
		iSession->DeleteCalFileL(iCalenderFileName);
		INFO_PRINTF1(_L("Listing out the files after the deletion of the file in the previous list"));
		CDesCArray* 	agendaFileList(GetSession().ListCalFilesL());
		CleanupStack::PushL(agendaFileList);
		TInt 			fileCount;
		for( fileCount=0; fileCount<agendaFileList->MdcaCount(); fileCount++)
			{
			INFO_PRINTF1(agendaFileList->MdcaPoint(fileCount));
			}
		if( fileCount == (count-1) )
			{
			INFO_PRINTF1(_L("ListCalFileL() has listed the files correctly"));
			iSession->CreateCalFileL(iCalenderFileName);
			}
		else
			{
			ERR_PRINTF1(_L("Listing is not done properly"));
			SetTestStepResult(EFail);
			}
		CleanupStack::PopAndDestroy(agendaFileList);
		}	
	}

void CTestCalInterimApiFileAccess::doAllocTestL()
	{
	INFO_PRINTF1(_L("Alloc test"));
	TInt failCount = 1;
	for( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, failCount);
		TRAPD( err, ListsAllAgendaFilesL());
		if ( err==KErrNone ) 
			{
			__UHEAP_RESET;
			break;
			}		
		if ( err != KErrNoMemory )
			SetTestStepResult(EFail);
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		failCount++;
		}
	INFO_PRINTF1(_L("Memory allocation testing for fileaccess is done"));
	INFO_PRINTF2(_L("FAILCOUNT %d"), failCount);	
	}
	
