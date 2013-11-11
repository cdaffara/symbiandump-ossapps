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
// This test creates vCals from the text specified below, then imports this into a calendar entry.
// This entry is then exported, and re-imported, then that re-imported entry is compared with the 
// original imported entry to check they are the same.
// The entry comparison focuses on timezone settings and daylight saving rules.
// 
//

#include <e32test.h>
#include <s32file.h>
#include <calrrule.h>
#include <calentryview.h>
#include <calalarm.h>

#include "caltestlib.h"

#include <coreappstest/testserver.h>

static RTest test(_L("t_utc_conversions"));

const TDateTime KFeb1st_00_00(2005, EFebruary, 0, 0, 0, 0, 0);
const TDateTime KFeb1st_23_59(2005, EFebruary, 0, 23, 59, 0, 0);
	
_LIT(KImportFile,"c:t_utc_conversions");

_LIT8(KTimeZoneTokyo, "Asia/Tokyo");
const TInt KHoursFromUTCInWinterInTokyo = 9;

const TDateTime KFeb1st_12_00(2005, EFebruary, 0, 12, 0, 0, 0);
const TDateTime KMar1st_12_00(2005, EMarch, 0, 12, 0, 0, 0);
const TDateTime KJul1st_12_00(2005, EJuly, 0, 12, 0, 0, 0);
const TDateTime KAug1st_12_00(2005, EAugust, 0, 12, 0, 0, 0);

const TTimeIntervalMinutes KAlarmOffset(15);


class CTestApp : public CBase
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void StartL();
		
private:
	void TestApptL();
	void TestEventL();
	void TestTodoL();
	void TestRptDefL();
	void TestAgnExceptionL();
	void TestTimesL(CCalEntry& aEntry);
	
	void ConstructL();
	
private:
	TBuf8<256> iOriginalTz;
	
	CCalTestLibrary* iCalTestLib;
	
	RPIMTestServer iPIMTest;
	
	TInt8 iTestStep;
	};

CTestApp::~CTestApp()
	{
	if (iCalTestLib)
		{
		TRAP_IGNORE(iCalTestLib->SetTimeZoneL(iOriginalTz));
		}
	delete iCalTestLib;
	}
	
CTestApp* CTestApp::NewL()
	{
	CTestApp* self = new (ELeave) CTestApp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestApp::ConstructL()
	{
	iCalTestLib = CCalTestLibrary::NewL();
	iCalTestLib->ReplaceFileL(KImportFile);
	iCalTestLib->OpenFileL(KImportFile);
	
	User::LeaveIfError(iPIMTest.Connect());
	iPIMTest.GetTimeZoneL(iOriginalTz);
	iCalTestLib->SetTimeZoneL(KTimeZoneTokyo());
	}
	
void CTestApp::StartL()
	{
	TestApptL();
	TestEventL();
	TestTodoL();
	TestRptDefL();
	TestAgnExceptionL();
	}

void CTestApp::TestTimesL(CCalEntry& aEntry)
	{
	// set local start and end times to 12:00 (UTC should be 03:00)
	TCalTime startTime;
	startTime.SetTimeLocalL(KFeb1st_12_00);
	TCalTime endTime;
	endTime.SetTimeLocalL(KMar1st_12_00);
	
	aEntry.SetStartAndEndTimeL(startTime, endTime);
	
	TTimeIntervalHours hours(0);
	TTime(KFeb1st_12_00).HoursFrom(aEntry.StartTimeL().TimeUtcL(), hours);
	test(hours.Int() == KHoursFromUTCInWinterInTokyo);
	
	TTime(KMar1st_12_00).HoursFrom(aEntry.EndTimeL().TimeUtcL(), hours);
	test(hours.Int() == KHoursFromUTCInWinterInTokyo);
	
	TTime(KFeb1st_12_00).HoursFrom(aEntry.StartTimeL().TimeLocalL(), hours);
	test(hours.Int() == 0);
	
	TTime(KMar1st_12_00).HoursFrom(aEntry.EndTimeL().TimeLocalL(), hours);
	test(hours.Int() == 0);
	
	// set UTC start and end times to 12:00 (local should be 21:00)
	startTime.SetTimeUtcL(KFeb1st_12_00);
	endTime.SetTimeUtcL(KMar1st_12_00);
	aEntry.SetStartAndEndTimeL(startTime, endTime);
	
	TTime(KFeb1st_12_00).HoursFrom(aEntry.StartTimeL().TimeUtcL(), hours);
	test(hours.Int() == 0);
	
	TTime(KMar1st_12_00).HoursFrom(aEntry.EndTimeL().TimeUtcL(), hours);
	test(hours.Int() == 0);
	
	TTime(KFeb1st_12_00).HoursFrom(aEntry.StartTimeL().TimeLocalL(), hours);
	test(hours.Int() == -KHoursFromUTCInWinterInTokyo);
	
	TTime(KMar1st_12_00).HoursFrom(aEntry.EndTimeL().TimeLocalL(), hours);
	test(hours.Int() == -KHoursFromUTCInWinterInTokyo);
	}

void CTestApp::TestApptL()
	{
	HBufC8* guid = NULL;
	CCalEntry* appt = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::PushL(appt);
	
	TestTimesL(*appt);
	
	CleanupStack::PopAndDestroy(appt);
	}

void CTestApp::TestEventL()
	{
	HBufC8* guid = NULL;
	CCalEntry* event = iCalTestLib->CreateCalEntryL(CCalEntry::EEvent, guid);
	CleanupStack::PushL(event);
	
	TestTimesL(*event);
	
	CleanupStack::PopAndDestroy(event);
	}

void CTestApp::TestTodoL()
	{
	HBufC8* guid = NULL;
	CCalEntry* todo = iCalTestLib->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::PushL(todo);
	
	TestTimesL(*todo);
	
	// set local crossed out time to 12:00 (UTC should be 03:00)
	TCalTime completedTime;
	completedTime.SetTimeLocalL(KFeb1st_12_00);
	todo->SetCompletedL(ETrue, completedTime);
	
	TTimeIntervalHours hours(0);
	TTime(KFeb1st_12_00).HoursFrom(todo->CompletedTimeL().TimeUtcL(), hours);
	test(hours.Int() == KHoursFromUTCInWinterInTokyo);
	
	TTime(KFeb1st_12_00).HoursFrom(todo->CompletedTimeL().TimeLocalL(), hours);
	test(hours.Int() == 0);
	
	// set UTC crossed out time to 12:00 (local should be 21:00)
	completedTime.SetTimeUtcL(KFeb1st_12_00);
	todo->SetCompletedL(ETrue, completedTime);
	TTime(KFeb1st_12_00).HoursFrom(todo->CompletedTimeL().TimeUtcL(), hours);
	test(hours.Int() == 0);
	
	TTime(KFeb1st_12_00).HoursFrom(todo->CompletedTimeL().TimeLocalL(), hours);
	test(hours.Int() == -KHoursFromUTCInWinterInTokyo);
	
	CleanupStack::PopAndDestroy(todo);
	}
	
void CTestApp::TestRptDefL()
	{
	TCalRRule rRule;
	rRule.SetType(TCalRRule::EDaily);
	
	// set local start time to 12:00 (UTC should be 03:00)
	TCalTime startTime;
	startTime.SetTimeLocalL(KFeb1st_12_00);
	rRule.SetDtStart(startTime);
	
	TTimeIntervalHours hours(0);
	TTime(KFeb1st_12_00).HoursFrom(rRule.DtStart().TimeUtcL(), hours);
	test(hours.Int() == KHoursFromUTCInWinterInTokyo);
	
	TTime(KFeb1st_12_00).HoursFrom(rRule.DtStart().TimeLocalL(), hours);
	test(hours.Int() == 0);
	
	// set UTC start time to 12:00 (local should be 21:00)
	startTime.SetTimeUtcL(KFeb1st_12_00);
	rRule.SetDtStart(startTime);
	TTime(KFeb1st_12_00).HoursFrom(rRule.DtStart().TimeUtcL(), hours);
	test(hours.Int() == 0);
	
	TTime(KFeb1st_12_00).HoursFrom(rRule.DtStart().TimeLocalL(), hours);
	test(hours.Int() == -KHoursFromUTCInWinterInTokyo);
	
	// set local end time to 12:00 (UTC should be 03:00)
	TCalTime endTime;
	endTime.SetTimeLocalL(KFeb1st_12_00);
	rRule.SetUntil(endTime);
	
	TTime(KFeb1st_12_00).HoursFrom(rRule.Until().TimeUtcL(), hours);
	test(hours.Int() == KHoursFromUTCInWinterInTokyo);
	
	TTime(KFeb1st_12_00).HoursFrom(rRule.Until().TimeLocalL(), hours);
	test(hours.Int() == 0);
	
	// set UTC end time to 12:00 (UTC should be 21:00)
	endTime.SetTimeUtcL(KFeb1st_12_00);
	rRule.SetUntil(endTime);
	TTime(KFeb1st_12_00).HoursFrom(rRule.Until().TimeUtcL(), hours);
	test(hours.Int() == 0);
	
	TTime(KFeb1st_12_00).HoursFrom(rRule.Until().TimeLocalL(), hours);
	test(hours.Int() == -KHoursFromUTCInWinterInTokyo);
	}
	
void CTestApp::TestAgnExceptionL()
	{
	HBufC8* guid = NULL;
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EEvent, guid);
	CleanupStack::PushL(entry);
	
	TCalTime entryStartTime;
	entryStartTime.SetTimeLocalL(KFeb1st_12_00);
	TCalTime entryEndTime;
	entryEndTime.SetTimeLocalL(TTime(KFeb1st_12_00) + TTimeIntervalHours(1));
	
	entry->SetStartAndEndTimeL(entryStartTime, entryEndTime);
	
	// Add a 'local time' repeat rule to the entry
	TCalRRule rRuleLocal;
	rRuleLocal.SetType(TCalRRule::EDaily);
	
	TCalTime startTime;
	startTime.SetTimeLocalL(KFeb1st_12_00);
	rRuleLocal.SetDtStart(startTime);
	rRuleLocal.SetCount(3); // for 3 days
	
	entry->SetRRuleL(rRuleLocal);
		
	// create a 'local time' exception list
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	
	TCalTime exceptionTime;
	exceptionTime.SetTimeLocalL(TTime(KFeb1st_12_00));
	exceptions.AppendL(exceptionTime);
	
	entry->SetExceptionDatesL(exceptions);
	exceptions.Reset();
	entry->GetExceptionDatesL(exceptions);
	
	TTimeIntervalHours hours(0);
	TTime(KFeb1st_12_00).HoursFrom(exceptions[0].TimeLocalL(), hours);
	test(hours.Int() == 0);
	exceptions.Reset();
	
	entry->ClearRepeatingPropertiesL();
	
	// Add a 'utc' repeat rule to the entry
	TCalRRule rRuleUtc;
	rRuleUtc.SetType(TCalRRule::EDaily);
	
	startTime.SetTimeUtcL(KFeb1st_12_00);
	rRuleUtc.SetDtStart(startTime);
	rRuleUtc.SetCount(3); // for 3 days
	
	entry->SetRRuleL(rRuleUtc);
	
	// set UTC exception time to 12:00 (local should be 21:00)
	exceptionTime.SetTimeUtcL(TTime(KFeb1st_12_00) - TTimeIntervalHours(12));
	exceptions.AppendL(exceptionTime);
	
	entry->SetExceptionDatesL(exceptions);
	exceptions.Reset();
	entry->GetExceptionDatesL(exceptions);
	
	TTime(KFeb1st_12_00).HoursFrom(exceptions[0].TimeUtcL(), hours);
	test(hours.Int() == KHoursFromUTCInWinterInTokyo);
	
	CleanupStack::PopAndDestroy(); //exceptions.Close
	CleanupStack::PopAndDestroy(entry);
	}
	
static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);
	
	testApp->StartL();
	
	CleanupStack::PopAndDestroy(testApp);
	}


/**

@SYMTestCaseID     PIM-T-UTC-CONVERSIONS-0001

*/

TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-UTC-CONVERSIONS-0001 t_utc_conversions"));

	test.Title();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	delete theCleanup;
	delete scheduler;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

