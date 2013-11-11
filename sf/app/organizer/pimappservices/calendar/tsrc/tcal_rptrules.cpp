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

// local includes
#include "caltestlib.h"

// System includes
#include <e32std.h>
#include <e32test.h>

#include <calalarm.h>
#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <calrrule.h>

#include "tcal_BadRRule.h"

RTest test(_L("tCal_RptRules"));

_LIT8(KGUID, "A_Global_UID");
_LIT(KCalendarFile, "tCal_RptRules");
_LIT(KFormatDate, "%D%M%Y%/0%1%/1%2%/2%3%/3");
const TInt KNudgeCount = 10;

// Further tests to add:
//  - test multiple overlapping repeat rules
//  - 

class CTestApp : public CBase
	{
public:
	static CTestApp* NewLC();
 	void DoTestL();
	~CTestApp();

private:

	enum {
		EDailyRptTest1,
		EDailyRptTest2,
		EDailyRptTest3,
		EWeeklyRptTest1,
		EWeeklyRptTest2,
		EWeeklyRptTest3,
		EMonthByDayRptTest1,
		EMonthByDayRptTest2,
		EMonthByDayRptTest3,
		EMonthByDateRptTest1,
		EMonthByDateRptTest2,
		EMonthByDateRptTest3,
		EYearByDateRptTest1,
		EYearByDateRptTest2,
		EYearByDayRptTest1,
		EYearByDayRptTest2,
		ENumberOfTests,
		};
		
	CTestApp();
	void ConstructL();
	
	void TestInvalidDataL();
	void SetUpTestL(TInt aTest);
	void AddEntryToDatabaseL(const TCalRRule& aRpt);
	void CheckThisTimeL(TTime aTime);
	void TestNextAndPreviousInstanceL();
 	void NegativeTestL();
	void TestRepeatRuleTypeChangeL();
	void TestNudgesL(CCalEntry::TType aType);
	void TestNudgingOnEntryL();
	void TestRDateNudgingOnEntryL();
	
	TTime TestNextInstanceL(TTime aTime);
	void TestPreviousInstanceL(TTime aTime);
	
	void TestDurationExtremeL();

 	void SetupDailyRpt1L();
	void SetupDailyRpt2L();
	void SetupDailyRpt3L();
	void SetupWeeklyRpt1L();
	void SetupWeeklyRpt2L();
	void SetupWeeklyRpt3L();
	void SetupMonthlyByDayRpt1L();
	void SetupMonthlyByDayRpt2L();
	void SetupMonthlyByDayRpt3L();
	void SetupMonthlyByDateRpt1L();
	void SetupMonthlyByDateRpt2L();
	void SetupMonthlyByDateRpt3L();
	void SetupYearlyByDateRpt1L();
	void SetupYearlyByDateRpt2L();
	void SetupYearlyByDayRpt1L();
	void SetupYearlyByDayRpt2L();

	void CheckRptExtremeL();
	void RDatesTestL();
	void NegativeTest2L();
	void UpdateOfEndDateInRepeatRuleTest();
	
public:
	CCalTestLibrary* iTestLib;

private:	
	CCalEntryView* iEntryView; // not owned
	CCalInstanceView* iInstanceView;  // not owned
	
	RArray<TTime> iOccurrenceArray;	// stores expected results
	TTime iPrevTime;
	TBuf<KMaxTimeFormatSpec> iBuffer;
	TInt iNudgeCount;
	CCalEntry::TType iType;
	};

CTestApp* CTestApp::NewLC()
	{
	CTestApp* self = new (ELeave) CTestApp();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestApp::CTestApp()
	{
	}

void CTestApp::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KCalendarFile());
	iTestLib->OpenFileL(KCalendarFile());
	
	iEntryView = &iTestLib->SynCGetEntryViewL();
	iInstanceView = &iTestLib->SynCGetInstanceViewL();
	}
	
CTestApp::~CTestApp()
	{
	delete iTestLib;
	iOccurrenceArray.Reset();
	}

// Check that this time is in the expected results
void CTestApp::CheckThisTimeL(TTime aTime)
	{
	aTime.FormatL(iBuffer,KFormatDate());
	test.Printf(_L(" instance %d is %S\n"), iNudgeCount, &iBuffer);
	
	// check new instance is different from previous one
	if (iNudgeCount > 0)
		{
		if (iPrevTime == aTime)
			{
			test.Printf(_L("Failed: instance %d not different from previous one\n"), iNudgeCount);
			test(0);
			}
		}
		
	// check new instance is in the expected instances list
	TInt timeArrayCount = 0;
	TBool foundTime = EFalse;
	while (timeArrayCount < iOccurrenceArray.Count())
		{
		if (aTime == iOccurrenceArray[timeArrayCount])
			{
			foundTime = ETrue;
			break;
			}
		timeArrayCount++;
		}
	
	if (!foundTime)
		{
		test.Printf(_L("Failed: instance %d has unexpected date\n"), iNudgeCount);
		}

	test(foundTime);
	
	iPrevTime = aTime;
	}

// Find the next 10 instances and see if they are as expected
TTime CTestApp::TestNextInstanceL(TTime aTime)
	{
	aTime -= TTimeIntervalDays(1);
	
	test.Printf(_L("Finding next instance\n"));
	iNudgeCount = 0;
	while (iNudgeCount < KNudgeCount)
		{
		// get next instance and display it
		TCalTime calTimeForFnCall;
		TCalTime calTimeReturnVal;
		calTimeForFnCall.SetTimeLocalL(aTime);
		calTimeReturnVal = iInstanceView->NextInstanceL(CalCommon::EIncludeAll, calTimeForFnCall);
		aTime = calTimeReturnVal.TimeLocalL();
		
		CheckThisTimeL(aTime);
		iNudgeCount++;
		}
	
	return aTime;
	}

// Find the previous 10 instances and see if they are as expected
void CTestApp::TestPreviousInstanceL(TTime aTime)
	{	
	iNudgeCount = 0;
	test.Printf(_L("Finding previous instance\n"));
	while (iNudgeCount < KNudgeCount)
		{
		// get previous instance and display it
		TCalTime calTimeForFnCall;
		TCalTime calTimeReturnVal;
		calTimeForFnCall.SetTimeLocalL(aTime);
		calTimeReturnVal = iInstanceView->PreviousInstanceL(CalCommon::EIncludeAll, calTimeForFnCall);
		aTime = calTimeReturnVal.TimeLocalL();
		aTime.FormatL(iBuffer,KFormatDate());
		
		CheckThisTimeL(aTime);
		
		iNudgeCount++;
		}
	}

void CTestApp::TestNudgingOnEntryL()
	{
	ASSERT(iOccurrenceArray.Count() == 10); // 10 entries are expected, if they are not present the test will fail later
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	iEntryView->FetchL(KGUID(), entryArray);
	test(entryArray.Count() == 1);
	CCalEntry* entry = entryArray[0];
	
	iPrevTime -= TTimeIntervalHours(1); // take off an hour before starting

	TCalTime calTime, nextTime;
	calTime.SetTimeLocalL(iPrevTime);
	for (TInt i = 0; i < 10; ++i)
		{
		nextTime = entry->NextInstanceForLocalUIDL(calTime);
		CheckThisTimeL(nextTime.TimeLocalL());
		calTime = nextTime;
		}
	
	for (TInt i = 0; i < 9; ++i)
		{
		nextTime = entry->PreviousInstanceForLocalUIDL(calTime);
		CheckThisTimeL(nextTime.TimeLocalL());
		calTime = nextTime;
		}
	
	CleanupStack::PopAndDestroy(&entryArray);
	
	// delete the entry from the calendar file
	RPointerArray<CCalInstance> instArray;
	CalCommon::TCalTimeRange timeRange(calTime, calTime);
	iInstanceView->FindInstanceL(instArray, CalCommon::EIncludeAll, timeRange);
	ASSERT(instArray.Count() > 0); // if this fails, lastInstanceTime doesn't point to an instance
	iInstanceView->DeleteL(instArray[0], CalCommon::EThisAndAll);
	instArray.Remove(0);
	instArray.ResetAndDestroy();
	}

void CTestApp::TestNextAndPreviousInstanceL()
	{
	ASSERT(iOccurrenceArray.Count() == 10); // 10 entries are expected, if they are not present the test will fail later
	
	TTime lastInstanceTime(TestNextInstanceL(iPrevTime));
	
	// go forward one day before nudging backwards
	TestPreviousInstanceL(lastInstanceTime + TTimeIntervalDays(1));
	
	// delete the entry from the calendar file
	RPointerArray<CCalInstance> instArray;
	TCalTime calTime;
	calTime.SetTimeLocalL(lastInstanceTime);
	CalCommon::TCalTimeRange timeRange(calTime, calTime);
	iInstanceView->FindInstanceL(instArray, CalCommon::EIncludeAll, timeRange);
	ASSERT(instArray.Count() > 0); // if this fails, lastInstanceTime doesn't point to an instance
	iInstanceView->DeleteL(instArray[0], CalCommon::EThisAndAll);
	instArray.Remove(0);
	instArray.ResetAndDestroy();
	}

// Create an entry with the specified repeat rule
void CTestApp::AddEntryToDatabaseL(const TCalRRule& aRpt)
	{
	//create an entry and set the repeating rule and set its start time with rule's start time
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(iType, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	TTime localDtStart(aRpt.DtStart().TimeLocalL());
	
	TCalTime entryStartTime;
	TCalTime entryEndTime;
	if (iType == CCalEntry::ETodo)
		{
		entryStartTime.SetTimeLocalL(localDtStart - TTimeIntervalHours(1));
		entryEndTime.SetTimeLocalL(localDtStart);
		}
	else
		{
		entryStartTime.SetTimeLocalL(localDtStart);
		entryEndTime.SetTimeLocalL(localDtStart + TTimeIntervalHours(1));
		}
	
	entry->SetStartAndEndTimeL(entryStartTime, entryEndTime);
	entry->SetRRuleL(aRpt);

	// test getrrule function here
	TCalRRule ruleToCompare;
	entry->GetRRuleL(ruleToCompare);
	if (aRpt.Until().TimeUtcL() != Time::NullTTime())
		{
		test(ruleToCompare.Until().TimeUtcL() == aRpt.Until().TimeUtcL());	
		}
	test(ruleToCompare.Type() == aRpt.Type());
	test(ruleToCompare.Interval() == aRpt.Interval());
	
	RPointerArray<CCalEntry> array;
	array.Append(entry);
	TInt success(0);
	iEntryView->StoreL(array, success);
	test(success == array.Count());
	array.Reset();
	
	CleanupStack::PopAndDestroy(entry);
	}


void CTestApp::SetupDailyRpt1L()
	{
	test.Next(_L("--- Daily test, every 1 day ---"));

	
	TCalRRule rpt(TCalRRule::EDaily);
	
	TTime daily1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime daily1end(TDateTime(2006, EJanuary, 17, 0, 0, 0, 0));
	
	TCalTime startTime;
	startTime.SetTimeLocalL(daily1start);
	rpt.SetDtStart(startTime);
	
	TCalTime endTime;
	endTime.SetTimeLocalL(daily1end);
	rpt.SetUntil(endTime);
	
	rpt.SetInterval(1);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 18, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 19, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 20, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 21, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 22, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 23, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 24, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 26, 0, 0, 0, 0)));
	iPrevTime = daily1start;
	AddEntryToDatabaseL(rpt);
	}
	


void CTestApp::SetupDailyRpt2L()
	{
	test.Next(_L("--- Daily test, every 5 days ---"));

	
	TCalRRule rpt(TCalRRule::EDaily);
	
	TTime daily1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(daily1start);
	rpt.SetDtStart(startTime);
	rpt.SetCount(100);
		
	rpt.SetInterval(5);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 22, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 1, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 6, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 11, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 16, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 21, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 26, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 3, 0, 0, 0, 0)));
	iPrevTime = daily1start;
	AddEntryToDatabaseL(rpt);
	}
	


void CTestApp::SetupDailyRpt3L()
	{
	test.Next(_L("--- Daily test, every 32 days ---"));

	
	TCalRRule rpt(TCalRRule::EDaily);
	
	
	TTime daily1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime daily1end(TDateTime(2006, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(daily1start);
	rpt.SetDtStart(startTime);
	
	TCalTime endTime;
	endTime.SetTimeLocalL(daily1end);
	rpt.SetUntil(endTime);
	
	rpt.SetInterval(32);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 18, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 22, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EApril, 23, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMay, 25, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJune, 26, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJuly, 28, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EAugust, 29, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EOctober, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ENovember, 1, 0, 0, 0, 0)));
	iPrevTime = daily1start;
	AddEntryToDatabaseL(rpt);
	}



void CTestApp::SetupWeeklyRpt1L()
	{
	test.Next(_L("--- Weekly test, every Monday every week ---"));

	
	TCalRRule rpt(TCalRRule::EWeekly);
	
	TTime weekly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime weekly1end(TDateTime(2008, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(weekly1start);
	rpt.SetDtStart(startTime);
	
	TCalTime endTime;
	endTime.SetTimeLocalL(weekly1end);
	rpt.SetUntil(endTime);
	
	RArray<TDay> array;
	array.Append(EMonday);
	rpt.SetByDay(array);
	array.Reset();
	rpt.SetInterval(1);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 23, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 6, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 20, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 6, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 20, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 27, 0, 0, 0, 0)));
	iPrevTime = weekly1start;
	AddEntryToDatabaseL(rpt);
	}
	


void CTestApp::SetupWeeklyRpt2L()
	{
	test.Next(_L("--- Weekly test, every Tuesday and Friday every 2 weeks ---"));

	
	TCalRRule rpt(TCalRRule::EWeekly);
	
	TTime weekly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	
	TCalTime startTime;
	startTime.SetTimeLocalL(weekly1start);
	rpt.SetDtStart(startTime);
	
	rpt.SetCount(100);
	
	RArray<TDay> array;
	array.Append(ETuesday);
	array.Append(EFriday);
	rpt.SetByDay(array);
	array.Reset();
	rpt.SetInterval(2);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 20, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 3, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 14, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 17, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 3, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 14, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 17, 0, 0, 0, 0)));
	iPrevTime = weekly1start;
	AddEntryToDatabaseL(rpt);
	}
	


void CTestApp::SetupWeeklyRpt3L()
	{
	test.Next(_L("--- Weekly test, every Wednesday and Thursday every 10 weeks ---"));

	
	TCalRRule rpt(TCalRRule::EWeekly);
	
	TTime weekly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime weekly1end(TDateTime(2008, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(weekly1start);
	rpt.SetDtStart(startTime);
	
	TCalTime endTime;
	endTime.SetTimeLocalL(weekly1end);
	rpt.SetUntil(endTime);
	
	RArray<TDay> array;
	array.Append(EWednesday);
	array.Append(EThursday);
	rpt.SetByDay(array);
	array.Reset();
	rpt.SetInterval(10);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 18, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 19, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 29, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJune, 7, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJune, 8, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EAugust, 16, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EAugust, 17, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EOctober, 25, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EOctober, 26, 0, 0, 0, 0)));
	iPrevTime = weekly1start;
	AddEntryToDatabaseL(rpt);
	}



void CTestApp::SetupMonthlyByDayRpt1L()
	{
	test.Next(_L("--- Monthly by day test, every 1st Saturday of every month ---"));

	
	TCalRRule rpt(TCalRRule::EMonthly);

	
	TTime monthly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(monthly1start);
	rpt.SetDtStart(startTime);
		
	RArray<TCalRRule::TDayOfMonth> dayArray;
	TCalRRule::TDayOfMonth day(ESaturday, 1);
	dayArray.Append(day);
	rpt.SetByDay(dayArray);
	dayArray.Reset();
	
	rpt.SetInterval(1);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 4, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 4, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EApril, 1, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMay, 6, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJune, 3, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJuly, 1, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EAugust, 5, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ESeptember, 2, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EOctober, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ENovember, 4, 0, 0, 0, 0)));
	iPrevTime = monthly1start;
	AddEntryToDatabaseL(rpt);
	}



void CTestApp::SetupMonthlyByDayRpt2L()
	{
	test.Next(_L("--- Monthly by day test, every 3rd Wednesday and last Thursday of every 2 months ---"));

	
	TCalRRule rpt(TCalRRule::EMonthly);
	
	TTime monthly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime monthly1end(TDateTime(2008, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(monthly1start);
	rpt.SetDtStart(startTime);
	
	TCalTime endTime;
	endTime.SetTimeLocalL(monthly1end);
	rpt.SetUntil(endTime);
	
	RArray<TCalRRule::TDayOfMonth> dayArray;
	TCalRRule::TDayOfMonth day1(EWednesday, 3);
	dayArray.AppendL(day1);
	TCalRRule::TDayOfMonth day2(EThursday, - 1);
	dayArray.AppendL(day2);
	rpt.SetByDay(dayArray);
	dayArray.Reset();
	
	rpt.SetInterval(2);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 18, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 26, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 15, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMay, 17, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMay, 25, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJuly, 19, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJuly, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ESeptember, 20, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ESeptember, 28, 0, 0, 0, 0)));
	iPrevTime = monthly1start;
	AddEntryToDatabaseL(rpt);
	}



void CTestApp::SetupMonthlyByDayRpt3L()
	{
	test.Next(_L("--- Monthly by day test, last Sunday of every 13 months ---"));

	
	TCalRRule rpt(TCalRRule::EMonthly);
	
	TTime monthly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(monthly1start);
	rpt.SetDtStart(startTime);
	
	RArray<TCalRRule::TDayOfMonth> dayArray;
	TCalRRule::TDayOfMonth day(ESunday, - 1);
	dayArray.Append(day);
	rpt.SetByDay(dayArray);
	dayArray.Reset();
	
	rpt.SetInterval(13);
	rpt.SetCount(16);

	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 29, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2006, EFebruary, 25, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2007, EMarch, 24, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2008, EApril, 26, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2009, EMay, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2010, EJune, 26, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2011, EJuly, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2012, EAugust, 25, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2013, ESeptember, 28, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2014, EOctober, 25, 0, 0, 0, 0)));
	iPrevTime = monthly1start;
	AddEntryToDatabaseL(rpt);
	}



void CTestApp::SetupMonthlyByDateRpt1L()
	{
	test.Next(_L("--- Monthly by date test, every month on the 31st ---"));

	
	TCalRRule rpt(TCalRRule::EMonthly);

	TTime monthly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime monthly1end(TDateTime(2018, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(monthly1start);
	rpt.SetDtStart(startTime);
	
	TCalTime endTime;
	endTime.SetTimeLocalL(monthly1end);
	rpt.SetUntil(endTime);
	
	RArray<TInt> array;
	array.Append(30);
	rpt.SetByMonthDay(array);
	array.Reset();
	rpt.SetInterval(1);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJanuary, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EApril, 29, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMay, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJune, 29, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJuly, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EAugust, 30, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ESeptember, 29, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EOctober, 30, 0, 0, 0, 0)));
	iPrevTime = monthly1start;
	AddEntryToDatabaseL(rpt);
	}



void CTestApp::SetupMonthlyByDateRpt2L()
	{
	test.Next(_L("--- Monthly by date test, every 2 months on the 1st and 10th ---"));

	
	TCalRRule rpt(TCalRRule::EMonthly);

	TTime monthly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(monthly1start);
	rpt.SetDtStart(startTime);
	
	rpt.SetCount(30);
	
	RArray<TInt> array;
	array.Append(0);
	array.Append(9);
	rpt.SetByMonthDay(array);
	array.Reset();
	rpt.SetInterval(2);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMarch, 9, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMay, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EMay, 9, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJuly, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EJuly, 9, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ESeptember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ESeptember, 9, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ENovember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, ENovember, 9, 0, 0, 0, 0)));
	iPrevTime = monthly1start;
	AddEntryToDatabaseL(rpt);
	}
	


void CTestApp::SetupMonthlyByDateRpt3L()
	{
	test.Next(_L("--- Monthly by date test, every 13 months on the 14th ---"));

	
	TCalRRule rpt(TCalRRule::EMonthly);

	TTime monthly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime monthly1end(TDateTime(2018, EJanuary, 17, 0, 0, 0, 0));

	TCalTime startTime;
	startTime.SetTimeLocalL(monthly1start);
	rpt.SetDtStart(startTime);
	
	TCalTime endTime;
	endTime.SetTimeLocalL(monthly1end);
	rpt.SetUntil(endTime);
	
	RArray<TInt> array;
	array.Append(13);
	rpt.SetByMonthDay(array);
	array.Reset();
	rpt.SetInterval(13);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2006, EFebruary, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2007, EMarch, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2008, EApril, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2009, EMay, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2010, EJune, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2011, EJuly, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2012, EAugust, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2013, ESeptember, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2014, EOctober, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2015, ENovember, 13, 0, 0, 0, 0)));
	iPrevTime = monthly1start;
	AddEntryToDatabaseL(rpt);
	}
	


void CTestApp::SetupYearlyByDateRpt1L()
	{
	test.Next(_L("--- Yearly by date test, every year on the 29th February ---"));


	TTime yearly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime yearly1end(TDateTime(2055, EJanuary, 17, 0, 0, 0, 0));
	
	TCalRRule rpt(TCalRRule::EYearly);
	
	TCalTime endTime;
	endTime.SetTimeLocalL(yearly1end);
	rpt.SetUntil(endTime);
	
	TTime startDate2(TDateTime(2008, EFebruary, 28, 0, 0, 0, 0));
	TCalTime startTime;
	startTime.SetTimeLocalL(startDate2);
	rpt.SetDtStart(startTime);
	
	rpt.SetInterval(1);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2008, EFebruary, 28, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2009, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2010, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2011, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2012, EFebruary, 28, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2013, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2014, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2015, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2016, EFebruary, 28, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2017, EFebruary, 27, 0, 0, 0, 0)));
	iPrevTime = startDate2;
	AddEntryToDatabaseL(rpt);
	}
	


void CTestApp::SetupYearlyByDateRpt2L()
	{
	test.Next(_L("--- Yearly by date test, every 3 years on the 1st December ---"));


	TTime yearly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	
	TCalRRule rpt(TCalRRule::EYearly);
	rpt.SetCount(30);
	
	TTime startDate(TDateTime(2005, EDecember, 0, 0, 0, 0, 0));
	TCalTime startTime;
	startTime.SetTimeLocalL(startDate);
	rpt.SetDtStart(startTime);
	rpt.SetInterval(3);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2008, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2011, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2014, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2017, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2020, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2023, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2026, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2029, EDecember, 0, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2032, EDecember, 0, 0, 0, 0, 0)));
	iPrevTime = yearly1start;
	AddEntryToDatabaseL(rpt);
	}



void CTestApp::SetupYearlyByDayRpt1L()
	{
	test.Next(_L("--- Yearly by day test, every year on the last Wednesday in February ---"));


	TTime yearly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	TTime yearly1end(TDateTime  (2025, EJanuary, 17, 0, 0, 0, 0));
	
	TCalRRule rpt(TCalRRule::EYearly);
	TCalTime startTime;
	startTime.SetTimeLocalL(yearly1start);
	rpt.SetDtStart(startTime);
	TCalTime endTime;
	endTime.SetTimeLocalL(yearly1end);
	rpt.SetUntil(endTime);
	
	TCalRRule::TDayOfMonth dayOfMonth(EWednesday, - 1);
	RArray<TCalRRule::TDayOfMonth> dayArray;
	dayArray.AppendL(dayOfMonth);
	rpt.SetByDay(dayArray);
	dayArray.Reset();
	
	RArray<TMonth> monthArray;
	monthArray.AppendL(EFebruary);
	rpt.SetByMonth(monthArray);
	monthArray.Reset();
	
	rpt.SetInterval(1);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EFebruary, 22, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2006, EFebruary, 21, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2007, EFebruary, 27, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2008, EFebruary, 26, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2009, EFebruary, 24, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2010, EFebruary, 23, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2011, EFebruary, 22, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2012, EFebruary, 28, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2013, EFebruary, 26, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2014, EFebruary, 25, 0, 0, 0, 0)));
	iPrevTime = yearly1start;
	AddEntryToDatabaseL(rpt);
	}



void CTestApp::SetupYearlyByDayRpt2L()
	{
	test.Next(_L("--- Yearly by date test, every 2nd year on the 2nd Monday in December ---"));


	TTime yearly1start(TDateTime(2005, EJanuary, 17, 0, 0, 0, 0));
	
	TCalRRule rpt(TCalRRule::EYearly);
	TCalTime startTime;
	startTime.SetTimeLocalL(yearly1start);
	rpt.SetDtStart(startTime);
	rpt.SetCount(30);
	
	TCalRRule::TDayOfMonth dayOfMonth(EMonday, 2);
	RArray<TCalRRule::TDayOfMonth> dayArray;
	dayArray.AppendL(dayOfMonth);
	rpt.SetByDay(dayArray);
	dayArray.Reset();
	
	RArray<TMonth> monthArray;
	monthArray.AppendL(EDecember);
	rpt.SetByMonth(monthArray);
	monthArray.Reset();
	
	rpt.SetInterval(2);
	iOccurrenceArray.Reset();
	iOccurrenceArray.AppendL(TTime(TDateTime(2005, EDecember, 11, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2007, EDecember, 9, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2009, EDecember, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2011, EDecember, 11, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2013, EDecember, 8, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2015, EDecember, 13, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2017, EDecember, 10, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2019, EDecember, 8, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2021, EDecember, 12, 0, 0, 0, 0)));
	iOccurrenceArray.AppendL(TTime(TDateTime(2023, EDecember, 10, 0, 0, 0, 0)));
	iPrevTime = yearly1start;
	AddEntryToDatabaseL(rpt);
	}

void CTestApp::TestInvalidDataL()
	{
	TCalRRule rulew(TCalRRule::EWeekly);
	RArray<TDay> weekDays;
	rulew.SetByDay(weekDays);
	rulew.GetByDayL(weekDays);
	test(weekDays.Count() == 0);
	
	TCalRRule rulem1(TCalRRule::EMonthly);
	RArray<TInt> monthDates;
	RArray<TCalRRule::TDayOfMonth> monthDays;
	rulem1.SetByMonthDay(monthDates);
	rulem1.GetByMonthDayL(monthDates);
	test(monthDates.Count() == 0);
	rulem1.GetByDayL(monthDays);
	test(monthDays.Count() == 0);
	
	TCalRRule rulem2(TCalRRule::EMonthly);
	rulem2.SetByDay(monthDays);
	rulem2.GetByDayL(monthDays);
	test(monthDays.Count() == 0);
	
	TCalRRule ruley(TCalRRule::EYearly);
	RArray<TMonth> months;
	ruley.SetByMonth(months);
	ruley.GetByMonthL(months);
	test(months.Count() == 0);
	ruley.SetByDay(monthDays);
	ruley.GetByDayL(monthDays);
	test(monthDays.Count() == 0);
	}

void CTestApp::SetUpTestL(TInt aTest)
	{
	switch (aTest)
		{
		case EDailyRptTest1: 	SetupDailyRpt1L(); break;
		case EDailyRptTest2: 	SetupDailyRpt2L(); break;
		case EDailyRptTest3: 	SetupDailyRpt3L(); break;
		case EWeeklyRptTest1: 	SetupWeeklyRpt1L(); break;
		case EWeeklyRptTest2: 	SetupWeeklyRpt2L(); break;
		case EWeeklyRptTest3: 	SetupWeeklyRpt3L(); break;
		case EMonthByDayRptTest1: 	SetupMonthlyByDayRpt1L(); break;
		case EMonthByDayRptTest2: 	SetupMonthlyByDayRpt2L(); break;
		case EMonthByDayRptTest3: 	SetupMonthlyByDayRpt3L(); break;
		case EMonthByDateRptTest1: 	SetupMonthlyByDateRpt1L(); break;
		case EMonthByDateRptTest2: 	SetupMonthlyByDateRpt2L(); break;
		case EMonthByDateRptTest3: 	SetupMonthlyByDateRpt3L(); break;
		case EYearByDateRptTest1: 	SetupYearlyByDateRpt1L(); break;
		case EYearByDateRptTest2: SetupYearlyByDateRpt2L(); break;
		case EYearByDayRptTest1: SetupYearlyByDayRpt1L(); break;
		case EYearByDayRptTest2: SetupYearlyByDayRpt2L(); break;
		}
	}

void CTestApp::TestNudgesL(CCalEntry::TType aType)
	{
	iType = aType;
	for (TInt i = 0; i < ENumberOfTests; i++)
		{
		SetUpTestL(i);
		TestNextAndPreviousInstanceL();
		}
	for (TInt i = 0; i < ENumberOfTests; i++)
		{
		SetUpTestL(i);
		TestNudgingOnEntryL();
		}
	}
		
// run each of the tests
void CTestApp::DoTestL()
	{
	TestNudgesL(CCalEntry::EAppt);
	TestNudgesL(CCalEntry::ETodo);
	TestInvalidDataL();
	NegativeTestL();
	NegativeTest2L();
	RDatesTestL();
	CheckRptExtremeL();	
	TestRepeatRuleTypeChangeL();
	TestRDateNudgingOnEntryL();
	TestDurationExtremeL();
	UpdateOfEndDateInRepeatRuleTest();
	}

void CTestApp::TestDurationExtremeL()
	{
	iTestLib->CleanDatabaseL();
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	_LIT8(KUid, "UID_duration");
	HBufC8* guid = KUid().AllocLC();
	CCalEntry* entryP = iTestLib->CreateCalEntryL(CCalEntry::EEvent, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entryP);
	
	entries.AppendL(entryP);
	CleanupStack::Pop(entryP);
	
	TTime startTime (TDateTime(2007, ENovember, 10, 0, 0, 0, 0)); 
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	TTime endTime (TDateTime(2080, ENovember, 10, 0, 0, 0, 0));
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime);
	entryP->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetDtStart(calStartTime);
	rpt.SetInterval(50);
	entryP->SetRRuleL(rpt);
	
	TInt entriesStored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
	test(entriesStored ==1);
	CleanupStack::PopAndDestroy(&entries);
			
	//Check instance which falls in the entry start date
					
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime searchEnd;
	searchEnd.SetTimeLocalL(startTime + TTimeIntervalDays(1));
	CalCommon::TCalTimeRange timeRange(calStartTime, searchEnd);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceList,filter,timeRange);
	TInt count = instanceList.Count();
	test(count==1);
	instanceList[0]->StartTimeL().TimeLocalL() == startTime;
	instanceList[0]->EndTimeL(). TimeLocalL() == endTime;
	CleanupStack::PopAndDestroy(&instanceList);
	}

void CTestApp::CheckRptExtremeL()
	{
// Verification for INC077695  Calendar: Repeating meeting starting and ending 23:59 duplicates ent
// Defect in the repeat rules code meant FindInstancesL() retrieved 60 entries for the day as opposed
// to 1 when the repeat time is set to the extreme of 23:59
	_LIT8(KEntryUid,"Extreme");
	HBufC8* uid = KEntryUid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EEvent,uid, CCalEntry::EMethodAdd, 0);
	CleanupStack::Pop();
	CleanupStack::PushL(entry);
		{
		// Set a repeat to the last minute of the day
		TDateTime dtStart(2006,EJanuary,20,23,59,0,0);
		TCalTime startCalTime;
		startCalTime.SetTimeUtcL(dtStart);
		entry->SetStartAndEndTimeL(startCalTime,startCalTime);
		TCalRRule calRule(TCalRRule::EYearly);
		calRule.SetDtStart(startCalTime);
		calRule.SetInterval(1);
		// Runs for 2 years but not important
		TDateTime dtUntil(2008,EJanuary,20,23,59,59,0);
		TCalTime untilCalTime;
		untilCalTime.SetTimeUtcL(dtUntil);
		calRule.SetUntil(untilCalTime);	
		entry->SetRRuleL(calRule);
		}
		{
		// Store the entry
		CCalEntryView& entryView = iTestLib->SynCGetEntryViewL();
		RPointerArray<CCalEntry> entryArray;
		CleanupClosePushL(entryArray);
		entryArray.AppendL(entry);
		TInt entriesAdded(0);
		entryView.StoreL(entryArray, entriesAdded);
		CleanupStack::PopAndDestroy(&entryArray);
		}
		{
		// Call FindInstanceL() for that day
		CCalInstanceView* instanceView = &iTestLib->SynCGetInstanceViewL();
		TCalTime calTimeStart;
		calTimeStart.SetTimeUtcL(TDateTime(2006, EJanuary, 20, 0,0 , 0, 0));
		TCalTime calTimeEnd;
		calTimeEnd.SetTimeUtcL(TDateTime(2006, EJanuary, 20,23, 59, 59, 0));
		RPointerArray<CCalInstance> instances;
		CleanupResetAndDestroyPushL(instances);
		CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
		CalCommon::TCalTimeRange timeRange(calTimeStart, calTimeEnd);
		instanceView->FindInstanceL(instances, filter, timeRange);
		test(instances.Count() == 1);
		CleanupStack::PopAndDestroy();
		}
	CleanupStack::PopAndDestroy(entry);
	}
 


void CTestApp::RDatesTestL()
    {
    test.Next(_L("Running RDatesTest"));

	HBufC8* dummy=NULL;
    RArray<TCalTime> rDateArray;
    CleanupClosePushL(rDateArray);
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);

    test.Printf(_L("creating an entry on 3rd September at 9:30\n"));

    // creating a todo entry
    CCalEntry* calEntry = iTestLib->CreateCalEntryL(CCalEntry::ETodo, dummy);
    CleanupStack::PushL(calEntry);

    TDateTime dtStart(2005,ESeptember,10,10,30,0,0);
    TCalTime startCalTime;
    TCalTime endCalTime;

    startCalTime.SetTimeUtcL(dtStart);

    endCalTime.SetTimeUtcL(dtStart);   

    calEntry->SetStartAndEndTimeL(startCalTime,endCalTime);

    //setting the repeat rule
    TCalRRule calRule(TCalRRule::EYearly);
    calRule.SetDtStart(startCalTime);
    calRule.SetInterval(1);
    calRule.SetCount( 5 );

    calEntry->SetRRuleL(calRule);

    TCalTime rDate1;
    TCalTime rDate2;
    TCalTime rDate3;
    TCalTime rDate4;
    TCalTime rDate5;

    rDate1.SetTimeUtcL(TDateTime(2005, EOctober, 2, 10, 0, 0, 0));
    rDate2.SetTimeUtcL(TDateTime(2005, EOctober, 2, 10, 40, 0, 0)); // Same date like before but differs only by time
    rDate3.SetTimeUtcL(TDateTime(2005, EOctober, 2, 10, 40, 25, 0)); // Same date like before but differs only by time
    rDate4.SetTimeUtcL(TDateTime(2005, EOctober, 2, 10, 40, 25, 12)); // Same date like before but differs only by time
    rDate5.SetTimeUtcL(TDateTime(2005, EOctober, 3, 10, 0, 0, 0));
   
    rDateArray.AppendL(rDate1);
    rDateArray.AppendL(rDate2);
    rDateArray.AppendL(rDate3);
    rDateArray.AppendL(rDate4);
    rDateArray.AppendL(rDate5);
   
    calEntry->SetRDatesL(rDateArray);
   
    TInt stored(0);
    entries.AppendL(calEntry);
	CleanupStack::Pop(calEntry);
	iEntryView->StoreL(entries, stored);
	test(stored == entries.Count());
	CleanupStack::PopAndDestroy(2, &rDateArray);//entries

    TCalTime calTimeStart;
    TCalTime calTimeEnd;

    calTimeStart.SetTimeUtcL(TDateTime(2005, EAugust, 30, 2, 7, 0, 0));
    calTimeEnd.SetTimeUtcL(TDateTime(2011, EOctober, 30, 2, 10, 0, 0));
   
    RPointerArray<CCalInstance> instances;
    CleanupResetAndDestroyPushL(instances);
    CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
    CalCommon::TCalTimeRange timeRange(calTimeStart, calTimeEnd);
   
    test.Printf(_L("Calling FindInstance\n"));
   
    iInstanceView->FindInstanceL(instances, filter, timeRange);

    CCalInstance* calInstance;
    for (TInt i = 0; i<instances.Count(); i++)
        {
        calInstance = instances[i];
        calInstance->Time().TimeLocalL().FormatL(iBuffer,KFormatDate());
        test.Printf(_L(" instance %d is %S\n"), i, &iBuffer);
        }

    test ( instances.Count() == 10 );//5 Repeat instances + 5 Sporadic instances

    iInstanceView->DeleteL(instances[0],CalCommon::EThisAndAll);//delete all the instances for other tests
    instances.Remove(0);
  	CleanupStack::PopAndDestroy(&instances);
    }

void CTestApp::NegativeTestL()
	{
//Set a repeat rule to a todo entry which doesn't have due date	
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	TTime endTime (Time::NullTTime()); 
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime);
	entry->SetStartAndEndTimeL(calEndTime, calEndTime);

	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EDaily);
	CleanupStack::PushL(rpt);
	TTime daily1end(TDateTime(2005, EJanuary, 9, 10, 0, 0, 0));
	TCalTime rptendTime;
	rptendTime.SetTimeLocalL(daily1end);

	TTime startTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	rpt->SetDtStart(calStartTime);
	rpt->SetUntil(rptendTime);
	rpt->SetInterval(1);
	TRAPD(err,entry->SetRRuleL(*rpt));//take a copy of the rule
	test(err==KErrArgument);
	CleanupStack::PopAndDestroy(2,entry);
	}

void CTestApp::TestRepeatRuleTypeChangeL()
	{
	// create entry repeating from Monday to Friday
	_LIT8(KRptTypeGuid, "guid1");
	
	HBufC8* guid = KRptTypeGuid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime startTime, untilTime;
	startTime.SetTimeLocalL(TDateTime(2006, EApril, 2, 9, 0, 0, 0)); // 0900 on Mon 3rd April 2006
	untilTime.SetTimeLocalL(TDateTime(2006, EApril, 6, 9, 0, 0, 0)); // 0900 on Fri 7th April 2006
	
	entry->SetStartAndEndTimeL(startTime, startTime);
	
	TCalRRule dailyRule(TCalRRule::EDaily);
	dailyRule.SetDtStart(startTime);
	dailyRule.SetInterval(1);
	dailyRule.SetUntil(untilTime);
	entry->SetRRuleL(dailyRule);
	iTestLib->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	
	// fetch and delete instance on Friday
	RPointerArray<CCalInstance> instances;
	CleanupClosePushL(instances);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(untilTime, untilTime));
	test(instances.Count() == 1);
	TCalLocalUid luid = instances[0]->Entry().LocalUidL();
	iTestLib->SynCGetInstanceViewL().DeleteL(instances[0], CalCommon::EThisOnly);
	CleanupStack::PopAndDestroy(&instances); // instances.Close()

	// fetch instance
	entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	
	TCalTime midTime;
	midTime.SetTimeLocalL(TDateTime(2006, EApril, 4, 9, 0, 0, 0)); // 0900 on Wed 5th April 2006
	entry->SetStartAndEndTimeL(midTime, midTime);
	
	TCalRRule weeklyRule(TCalRRule::EWeekly);
	weeklyRule.SetDtStart(startTime);
	weeklyRule.SetInterval(1);
	weeklyRule.SetUntil(midTime);

	RArray<TDay> weekDays;
	CleanupClosePushL(weekDays);
	weekDays.Append(EWednesday);
	weeklyRule.SetByDay(weekDays);
	CleanupStack::PopAndDestroy(&weekDays); // weekDays.Close()

	entry->SetRRuleL(weeklyRule);
	iTestLib->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	
	// fetch entry and check its non repeating
	entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	
	TCalRRule fetchedRule;
	test(entry->GetRRuleL(fetchedRule) == EFalse);
	
	CleanupStack::PopAndDestroy(entry);
	}



void CTestApp::TestRDateNudgingOnEntryL()
	{
	test.Next(_L("test nudging on entry with RDates"));

	
	_LIT8(KGuidA, "guidA");
	HBufC8* guid = KGuidA().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime startTime, rdate1, rdate2;
	startTime.SetTimeLocalL(TDateTime(2006, EApril, 0, 9, 0, 0, 0));// 0900 on 1 April
	rdate1.SetTimeLocalL(TDateTime(2006, EMarch, 0, 9, 0, 0, 0)); 	// 0900 on 1 March
	rdate2.SetTimeLocalL(TDateTime(2006, EMay, 0, 9, 0, 0, 0)); 	// 0900 on 1 May
	
	entry->SetStartAndEndTimeL(startTime, startTime);
	RArray<TCalTime> rdateArray;
	CleanupClosePushL(rdateArray);
	rdateArray.AppendL(rdate1);
	rdateArray.AppendL(rdate2);
	entry->SetRDatesL(rdateArray);
	CleanupStack::PopAndDestroy(&rdateArray);
	
	TCalTime nudgeTime;
	nudgeTime.SetTimeLocalL(rdate1.TimeLocalL() - TTimeIntervalDays(1));
	nudgeTime = entry->NextInstanceForLocalUIDL(nudgeTime);
	test(nudgeTime.TimeLocalL() == rdate1.TimeLocalL());
	nudgeTime = entry->NextInstanceForLocalUIDL(nudgeTime);
	test(nudgeTime.TimeLocalL() == startTime.TimeLocalL());
	nudgeTime = entry->NextInstanceForLocalUIDL(nudgeTime);
	test(nudgeTime.TimeLocalL() == rdate2.TimeLocalL());
	nudgeTime = entry->NextInstanceForLocalUIDL(nudgeTime);
	test(nudgeTime.TimeLocalL() == Time::NullTTime());
	
	nudgeTime.SetTimeLocalL(rdate2.TimeLocalL() + TTimeIntervalDays(1));
	nudgeTime = entry->PreviousInstanceForLocalUIDL(nudgeTime);
	test(nudgeTime.TimeLocalL() == rdate2.TimeLocalL());
	nudgeTime = entry->PreviousInstanceForLocalUIDL(nudgeTime);
	test(nudgeTime.TimeLocalL() == startTime.TimeLocalL());
	nudgeTime = entry->PreviousInstanceForLocalUIDL(nudgeTime);
	test(nudgeTime.TimeLocalL() == rdate1.TimeLocalL());
	nudgeTime = entry->PreviousInstanceForLocalUIDL(nudgeTime);
	test(nudgeTime.TimeLocalL() == Time::NullTTime());
	
	CleanupStack::PopAndDestroy(entry);
	}

void CTestApp::NegativeTest2L()
    {
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime startTime;
	startTime.SetTimeUtcL(TTime(TDateTime(2006, ENovember, 8, 10, 0, 0, 0)));
	TCalTime endTime;
	endTime.SetTimeUtcL(TTime(TDateTime(2006, ENovember, 8, 12, 0, 0, 0)));
	
	entry->SetStartAndEndTimeL(startTime, endTime);
	
	TCalRRule rrule(TCalRRule::EYearly);
	RArray<TCalRRule::TDayOfMonth> days;
	CleanupClosePushL(days);
	TCalRRule::TDayOfMonth day(ESaturday, 1);
	days.AppendL(day);
	rrule.SetByDay(days);
	
	/* Don't do this, although required
	RArray<TMonth> month;
	month.AppendL(ENovember);
	rrule.SetByMonth(month);
	month.Reset()
    */
	
	CleanupStack::PopAndDestroy(&days);
	TCalTime untilTime; 
	untilTime.SetTimeUtcL(TTime(TDateTime(2010, ENovember, 8, 10, 0, 0, 0)));

	rrule.SetDtStart(startTime);
	rrule.SetUntil(untilTime);
	 
	TRAPD(err,entry->SetRRuleL(rrule));
	test(err==KErrArgument);
	test(!entry->GetRRuleL(rrule));

	CleanupStack::PopAndDestroy();
	
    }

void CTestApp::UpdateOfEndDateInRepeatRuleTest()
	{
	test.Next(_L("test updating of end date of repeat rules"));
	
	//Daily Repeat 
	TCalRRule rpt(TCalRRule::EDaily);
	
	//Setting the repeat start time
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2009, EMarch, 01, 9, 0, 0, 0));
	rpt.SetDtStart(startTime);
	//Setting the repeat End time
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2009, EMarch, 29, 9, 0, 0, 0));

	//	set repeat count instead of until time
	rpt.SetCount(30);
	rpt.SetInterval(1);
	
	RPointerArray<CCalEntry> array;
	CleanupResetAndDestroyPushL(array);
	
	//Add entry to the Database
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(iType, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	array.AppendL(entry);
	CleanupStack::Pop(entry);
	
	TTime localDtEntryStart(rpt.DtStart().TimeLocalL());
	TTime localDtEntryEnd(rpt.DtStart().TimeLocalL()+ TTimeIntervalHours(2));
	
	TCalTime entryStartTime;
	TCalTime entryEndTime;
	entryStartTime.SetTimeLocalL(localDtEntryStart);
	entryEndTime.SetTimeLocalL(localDtEntryEnd);
	entry->SetStartAndEndTimeL(entryStartTime, entryEndTime);
	entry->SetRRuleL(rpt);
	//Store the entry with repeat rule
	TInt success(0);
	iEntryView->StoreL(array, success);
	test(success == array.Count());
	array.ResetAndDestroy();
	
	// test getrrule until time after fetching 
	iEntryView->FetchL(KGUID(), array);	
	TCalRRule ruleToCompare;
	array[0]->GetRRuleL(ruleToCompare);

	TCalTime expectedUntilTime;
	expectedUntilTime.SetTimeLocalL(TDateTime(2009, EMarch, 30, 11, 0, 0, 0));
	//Checking that setting the count has correctly set the until time from the fetched entry 
	test(ruleToCompare.Until().TimeLocalL() == expectedUntilTime.TimeLocalL());	

	//	test that setting the count resets the until time and vice versa 
	rpt.SetCount(10);
	test(rpt.Until().TimeLocalL() == Time::NullTTime() && rpt.Count() == 10);
	endTime.SetTimeLocalL(TDateTime(2009, EDecember, 29, 0, 0, 0, 0));

	// set a new until time that is later than the original until time and update the entry 
	rpt.SetUntil(endTime);
	test(rpt.Until().TimeLocalL() == endTime.TimeLocalL() && rpt.Count() == -1);
	
	array[0]->SetRRuleL(rpt);
	success = 0;
	iEntryView->UpdateL(array, success);
	test(success == array.Count());
	array.ResetAndDestroy();
	
	//Fetch entry again to verify the modified until time is set properly
	iEntryView->FetchL(KGUID(), array);	
	TCalRRule ruleWithModifiedUntilTime;
	array[0]->GetRRuleL(ruleWithModifiedUntilTime);
	//Checking the until time from fetched entry with new modified until time
	test(ruleWithModifiedUntilTime.Until().TimeLocalL() == rpt.Until().TimeLocalL());	

	//	calling getrrule will return a rule with populated count and untiltime
	test(ruleWithModifiedUntilTime.Until().TimeLocalL() != Time::NullTTime());
	test(ruleWithModifiedUntilTime.Count() > 0);
			
	//	check that the calendar can handle updating with a rule that contains both count and untiltime
	//	there should be no change in the entry
	array[0]->SetRRuleL(ruleWithModifiedUntilTime);
	success = 0;
	iEntryView->UpdateL(array, success);
	test(success == array.Count());
	array.ResetAndDestroy();

	//Fetch entry again to verify it still contains the correct until time
	iEntryView->FetchL(KGUID(), array);	
	TCalRRule ruleWithUnmodifiedModifiedUntilTime;
	array[0]->GetRRuleL(ruleWithUnmodifiedModifiedUntilTime);
	
	test(ruleWithUnmodifiedModifiedUntilTime.Until().TimeLocalL() == ruleWithModifiedUntilTime.Until().TimeLocalL());
	test(ruleWithUnmodifiedModifiedUntilTime.Count() == ruleWithModifiedUntilTime.Count());
		
	CleanupStack::PopAndDestroy(&array); 
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CTestApp* testApp = CTestApp::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite
	
	testApp->DoTestL();
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
 	CBadRRule* testManager = CBadRRule::NewL(testApp->iTestLib);
 	CleanupStack::PushL(testManager);
   	
 	testManager->TestBadRRuleL();
     
 	CleanupStack::PopAndDestroy(testManager);
 	 	
	CleanupStack::PopAndDestroy(testApp);
	}


/**

@SYMTestCaseID     PIM-TCAL-RPTRULES-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-RPTRULES-0001 Calendar Interim API Repeat Rule test suite"));

	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler();
	if (!scheduler)
		{
		delete trapCleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);	

	TRAPD(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
