// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <f32file.h>
#include <s32file.h>

#include <calentry.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calrrule.h>

#include "caltestlib.h"
#include "t_recurrence.h"

_LIT(KTestFile, "c:t_recurrence");

static RTest test(_L("t_recurrence"));
	
CTestApp* CTestApp::NewL()
	{
	CTestApp* self = new(ELeave) CTestApp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}	
	
void CTestApp::ConstructL()
	{
	iCalTestLib = CCalTestLibrary::NewL();
	iCalTestLib->ReplaceFileL(KTestFile());
	iCalTestLib->OpenFileL(KTestFile());
	}
	
CTestApp::~CTestApp()
	{
	delete iCalTestLib;
	}


void CTestApp::RunTestL(const TDesC8& aVCalFile, TInt aNumberOfOccurences, TTime aEndTime, TBool aExpectedImport)
	{
	test.Next(_L("Import repeat rule test"));

	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iCalTestLib->ImportL(aVCalFile, entryArray);//31 Jan, 31 Mar, 31 May, 31 July, 31 Jan 5 matched months but last 7 months to reach the end so agnmodel returns 7
	
	if (aExpectedImport)
		{
		test.Printf(_L("Impoted %d entries\n"), entryArray.Count());
		test(entryArray.Count() == 1);
		
		test.Printf(entryArray[0]->SummaryL());

		TCalRRule rRule;
		
		if (aNumberOfOccurences == 0)
			{
			test (entryArray[0]->GetRRuleL(rRule));
			test(rRule.Count() == 0);
			test(rRule.Until().TimeLocalL() == TCalTime::MaxTime());
			}
		else if (aNumberOfOccurences > 1)
			{	
			test (entryArray[0]->GetRRuleL(rRule));
			TInt instances = rRule.Count();

			test.Printf( _L("Instances found %d\n"), instances);
			test(instances == aNumberOfOccurences);
			
			TDateTime dt(rRule.Until().TimeLocalL().DateTime());
		
			test.Printf(_L("The end time of the repeat rule is %d:%d %d/%d/%d \n"), dt.Hour(), dt.Minute(), dt.Day(), dt.Month(), dt.Year());
				
			if(aEndTime != Time::NullTTime())
				{
				test(rRule.Until().TimeLocalL() == aEndTime);
				}
			}
		else
			{
			test(!entryArray[0]->GetRRuleL(rRule));
			test.Printf(_L("The repeat was not added because the second instance was passed max date.\n"));	
			}
			
		}
	else
		{
		test(entryArray.Count() == 0);
		}
	
	CleanupStack::PopAndDestroy(&entryArray);
	}

static void RunTestsL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);

	testApp->RunTestL(KWeekly,				KWeeklyCount,				Time::NullTTime(),		1);
	testApp->RunTestL(KMonthlyByDate1,		KMonthlyByDateCount1,		Time::NullTTime(), 		1);
	testApp->RunTestL(KMonthlyByDate2,		KMonthlyByDateCount2,		Time::NullTTime(), 		1);
	testApp->RunTestL(KMonthlyByDate3,		KMonthlyByDateCount3,		Time::NullTTime(), 		1);
	testApp->RunTestL(KMonthlyByDays1,		KMonthlyByDaysCount1,		Time::NullTTime(), 		1);
	testApp->RunTestL(KMonthlyByDays2,		KMonthlyByDaysCount2, 		Time::NullTTime(), 		1);
	testApp->RunTestL(KMonthlyByDays3,		KMonthlyByDaysCount3,		Time::NullTTime(), 		1);
	testApp->RunTestL(KYearlyByDate1, 		KYearlyByDateCount1,		Time::NullTTime(), 		1);
	testApp->RunTestL(KYearlyByDate2, 		KYearlyByDateCount2,		Time::NullTTime(), 		1);
	testApp->RunTestL(KYearlyByDate3, 		KYearlyByDateCount3,		Time::NullTTime(), 		1);
	testApp->RunTestL(KMonthlyByDates31,	KMonthlyByDates31Count,		Time::NullTTime(), 		1);
	
	// Test importing repeats that pass the agenda max time
	testApp->RunTestL(KDailyOver1,			KDailyOverCount1,			Time::NullTTime(), 		1);
	testApp->RunTestL(KDailyOver2,			KDailyOverCount2,			Time::NullTTime(), 		0);
	testApp->RunTestL(KMonthlyByDateOver1,	KMonthlyByDateOverCount1,	KMonthlyByDateOverEndDay1, 		1);
	testApp->RunTestL(KMonthlyByDateOver2,	KMonthlyByDateOverCount2,	Time::NullTTime(), 		1);
	testApp->RunTestL(KYearlyByDateOver1,	KYearlyByDateOverCount1,	Time::NullTTime(), 		1);
	testApp->RunTestL(KYearlyByDateOver2,	KYearlyByDateOverCount2,	Time::NullTTime(), 		1);
	testApp->RunTestL(KWeeklyOver1,			KWeeklyOverCount1,			Time::NullTTime(), 		1);
	testApp->RunTestL(KWeeklyOver2,			KWeeklyOverCount2,			Time::NullTTime(), 		1);
	testApp->RunTestL(KAfterMaxDate,		KAfterMaxDateCount,			Time::NullTTime(), 		0);
	
	CleanupStack::PopAndDestroy(testApp);
	}


/**

@SYMTestCaseID     PIM-T-RECURRENCE-0001

*/

TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-RECURRENCE-0001 t_recurrence"));

	test.Title();
	CActiveScheduler* scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	
	CTrapCleanup* cleanupTrap = CTrapCleanup::New();
	
	TRAPD(ret, RunTestsL());
	test(ret==KErrNone);
	
	delete cleanupTrap;
	delete scheduler;
	
	test.End();
	test.Close();
	
	__UHEAP_MARKEND;
	return KErrNone;
	}


