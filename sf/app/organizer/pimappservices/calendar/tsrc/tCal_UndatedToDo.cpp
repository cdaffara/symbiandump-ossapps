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

#include "caltestlib.h"

#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caliterator.h>
#include <calprogresscallback.h>
#include <calrrule.h>
#include <calsession.h>
#include <e32test.h>
#include <tz.h>

RTest test(_L("tcal_undatedtodo"));

_LIT(KCalendarFile, "tcal_undatedtodo");
_LIT(KDateFormat, "%02d:%02d %02d/%02d/%04d\0");

class CTestApp : public CBase
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void DoTestsL();
	
private:
	void ConstructL();
	
	void TestTodoSortOrderL(const CalCommon::TCalTimeRange& aSearchRange);
	void TestFindInstanceNonRepeatingTodoL();
	void TestFindInstanceRepeatingTodoL();
	void TestNextInstanceL();
	
	void CreateAndTestNonRptTodoL(const TTime& aStartTime, const TTime& aEndTime, const TTime& aCompletedTime, const CalCommon::TCalTimeRange& aTimeRange, TBool aExpected);
	CCalEntry* CreateTodoLC(const TTime& aStartTime, const TTime& aEndTime, const TTime& aCompletedTime);

	void CreateAndTestRptTodoL(const TTime& aStartTime, const TTime& aEndTime, const TTime& aCompletedTime, TCalRRule::TType aRepeatType, TInt aInterval, TInt aCount, const CalCommon::TCalTimeRange& aTimeRange, RArray<TTime> aExpectedLocalInstanceTimes);
	CCalEntry* CreateRepeatingTodoLC(const TTime& aStartTime, const TTime& aEndTime, const TTime& aCompletedTime, TCalRRule::TType aRepeatType, TInt aInterval, TInt aCount);
		
	void PrintDetailsInstancesL(const RPointerArray<CCalInstance>& aInstanceList);
	HBufC* TimeBufLC(const TTime& aLocalTime);
		
private: // member data
	CCalTestLibrary* iTestLib;
	
	CCalEntryView* iEntryView;		// not owned
	CCalInstanceView* iInstanceView;// not owned
	
	TTime iToday;
	};

CTestApp* CTestApp::NewL()
	{
	CTestApp* app = new (ELeave) CTestApp();
	CleanupStack::PushL(app);
	app->ConstructL();
	CleanupStack::Pop(app);
	return app;
	}
	
CTestApp::~CTestApp()
	{
	delete iTestLib;
	}

void CTestApp::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();

	iTestLib->ReplaceFileL(KCalendarFile());
	iTestLib->OpenFileL(KCalendarFile());

	iInstanceView = &iTestLib->SynCGetInstanceViewL();
	iEntryView = &iTestLib->SynCGetEntryViewL();
	
	iToday.HomeTime();
	}
	
HBufC* CTestApp::TimeBufLC(const TTime& aLocalTime)
	{
	TBuf<17> timeBuf;
	TDateTime localDateTime = aLocalTime.DateTime();
	timeBuf.AppendFormat(KDateFormat,
							localDateTime.Hour(),
							localDateTime.Minute(),
							localDateTime.Day() + 1, 
							static_cast<TInt>(localDateTime.Month()) + 1, 
							localDateTime.Year());
							
	return timeBuf.AllocLC();
	}
		
void CTestApp::PrintDetailsInstancesL(const RPointerArray<CCalInstance>& aInstanceList)
	{
	const TInt KInstanceCount(aInstanceList.Count());
	
	if (KInstanceCount == 0)
		{	
		test.Printf(_L("No instances found!"));
		}
	else
		{
		test.Printf(_L("%d instances found"), KInstanceCount);
		
		// print the entry details
		if (aInstanceList[0]->Entry().EndTimeL().TimeLocalL() != Time::NullTTime())
			{
			// get the instance and entry due dates as strings	
			HBufC* entryEndTimeBuf = TimeBufLC(aInstanceList[0]->Entry().EndTimeL().TimeLocalL());
			
			// does entry have start time?
			TCalTime entryStartTime = aInstanceList[0]->Entry().StartTimeL();
			if (entryStartTime.TimeLocalL() == Time::NullTTime())
				{
				test.Printf(_L("Entry due date := %s"), entryEndTimeBuf->Ptr());
				}
			else
				{
				// get the instance and entry start times as strings
				HBufC* entryStartTimeBuf = TimeBufLC(aInstanceList[0]->Entry().StartTimeL().TimeLocalL());
			
				test.Printf(_L("Entry start and due date := %s - %s"), entryStartTimeBuf->Ptr(), entryEndTimeBuf->Ptr());
				CleanupStack::PopAndDestroy(entryStartTimeBuf);
				}
			
			CleanupStack::PopAndDestroy(entryEndTimeBuf);
			}
		else
			{
			test.Printf(_L("undated entry"));
			}
		}
	
	for (TInt i(0) ; i < KInstanceCount ; ++i)
		{	
		if (aInstanceList[i]->Entry().EndTimeL().TimeLocalL() != Time::NullTTime())
			{
			// get the instance and entry due dates as strings	
			HBufC* instanceEndTimeBuf = TimeBufLC(aInstanceList[i]->EndTimeL().TimeLocalL());
			
			// does entry have start time?
			TCalTime entryStartTime = aInstanceList[i]->Entry().StartTimeL();
			if (entryStartTime.TimeLocalL() == Time::NullTTime())
				{
				test.Printf(_L("Instance %d due date := %s"), i + 1, instanceEndTimeBuf->Ptr());
				}
			else
				{
				// get the instance and entry start times as strings
				HBufC* instanceStartTimeBuf = TimeBufLC(aInstanceList[i]->StartTimeL().TimeLocalL());
			
				test.Printf(_L("Instance %d start and due date := %s - %s"), i + 1, instanceStartTimeBuf->Ptr(), instanceEndTimeBuf->Ptr());
				CleanupStack::PopAndDestroy(instanceStartTimeBuf);
				}
			
			CleanupStack::PopAndDestroy(instanceEndTimeBuf);
			}
		else
			{
			test.Printf(_L("undated"));
			}
		
		// display the completed time if the is one
		TTime completedTimeLocal = aInstanceList[i]->Entry().CompletedTimeL().TimeLocalL();
		if (completedTimeLocal != Time::NullTTime())
			{
			HBufC* completedTimeBuf = TimeBufLC(completedTimeLocal);
			test.Printf(_L("Completed time = %s"), completedTimeBuf->Ptr());
			CleanupStack::PopAndDestroy(completedTimeBuf);
			}
		else
			{
			test.Printf(_L("incomplete"));
			}
		}
	}

CCalEntry* CTestApp::CreateTodoLC(const TTime& aStartTime, const TTime& aEndTime,  const TTime& aCompletedTime)
	{
	HBufC8* guid = NULL;
	CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::PushL(entry);
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(aStartTime);
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(aEndTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);

	if (aCompletedTime != Time::NullTTime())
		{
		TCalTime calTime;
		calTime.SetTimeLocalL(aCompletedTime);
		entry->SetCompletedL(ETrue, calTime);
		}

	return entry;
	}
	
CCalEntry* CTestApp::CreateRepeatingTodoLC(const TTime& aStartTime, const TTime& aEndTime, const TTime& aCompletedTime, TCalRRule::TType aRepeatType, TInt aInterval, TInt aCount)
	{
	HBufC8* guid = NULL;
	CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::PushL(entry);
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(aStartTime);
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(aEndTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	TCalRRule rRule(aRepeatType);
	rRule.SetDtStart(calEndTime);
	rRule.SetInterval(aInterval);
	rRule.SetCount(aCount);
	
	switch (aRepeatType)
		{
		case TCalRRule::EWeekly:
			{
			RArray<TDay> days;
			CleanupClosePushL(days);
			days.AppendL(aEndTime.DayNoInWeek());
			rRule.SetByDay(days);
			CleanupStack::PopAndDestroy(&days);
			}
			break;
		case TCalRRule::EMonthly:
			{
			RArray<TInt> monthDays;
			CleanupClosePushL(monthDays);
			monthDays.AppendL(aEndTime.DayNoInMonth());
			rRule.SetByMonthDay(monthDays);
			CleanupStack::PopAndDestroy(&monthDays);
			}
			break;
		case TCalRRule::EYearly:
			{
			RArray<TMonth> months;
			CleanupClosePushL(months);
			months.AppendL(aEndTime.DateTime().Month());
			rRule.SetByMonth(months);
			CleanupStack::PopAndDestroy(&months);
			}
			break;
		}
	
	entry->SetRRuleL(rRule);
	
	if (aCompletedTime != Time::NullTTime())
		{
		TCalTime calTime;
		calTime.SetTimeLocalL(aCompletedTime);
		entry->SetCompletedL(ETrue, calTime);
		}
		
	return entry;
	}
	
void CTestApp::TestTodoSortOrderL(const CalCommon::TCalTimeRange& aSearchRange)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	
	// Create 3 TODOs 2 days in the past
	// with different priorities
	entryArray.AppendL(CreateTodoLC(iToday - TTimeIntervalDays(2), iToday - TTimeIntervalDays(2), Time::NullTTime()));
	entryArray[entryArray.Count() - 1]->SetPriorityL(4);
	entryArray.AppendL(CreateTodoLC(iToday - TTimeIntervalDays(2), iToday - TTimeIntervalDays(2), Time::NullTTime()));
	entryArray[entryArray.Count() - 1]->SetPriorityL(2);
	entryArray.AppendL(CreateTodoLC(iToday - TTimeIntervalDays(2), iToday - TTimeIntervalDays(2), Time::NullTTime()));
	entryArray[entryArray.Count() - 1]->SetPriorityL(3);
	
	// Create 3 TODOs 1 day in the past
	// with different completed times
	entryArray.AppendL(CreateTodoLC(iToday - TTimeIntervalDays(1), iToday - TTimeIntervalDays(1), iToday - TTimeIntervalDays(1) - TTimeIntervalMinutes(4)));
	entryArray.AppendL(CreateTodoLC(iToday - TTimeIntervalDays(1), iToday - TTimeIntervalDays(1), iToday - TTimeIntervalDays(1) - TTimeIntervalMinutes(2)));
	entryArray.AppendL(CreateTodoLC(iToday - TTimeIntervalDays(1), iToday - TTimeIntervalDays(1), iToday - TTimeIntervalDays(1) - TTimeIntervalMinutes(7)));

	// Create 2 undated TODOs
	entryArray.AppendL(CreateTodoLC(Time::NullTTime(), Time::NullTTime(), Time::NullTTime()));
	entryArray.AppendL(CreateTodoLC(Time::NullTTime(), Time::NullTTime(), Time::NullTTime()));
	// Create 2 undated TODOs
	// with completed times
	entryArray.AppendL(CreateTodoLC(Time::NullTTime(), Time::NullTTime(), iToday - TTimeIntervalMinutes(4)));
	entryArray.AppendL(CreateTodoLC(Time::NullTTime(), Time::NullTTime(), iToday - TTimeIntervalMinutes(7)));
	
	// Create a TODO in the future
	entryArray.AppendL(CreateTodoLC(iToday + TTimeIntervalDays(2), iToday + TTimeIntervalDays(2), Time::NullTTime()));	
	
	TInt success(0);
	iEntryView->StoreL(entryArray, success);
	test(success == entryArray.Count());
	
	CleanupStack::PopAndDestroy(entryArray.Count());
	CleanupStack::PopAndDestroy(&entryArray);
	RPointerArray<CCalInstance> instanceArray;
	CleanupResetAndDestroyPushL(instanceArray);

	iInstanceView->FindInstanceL(instanceArray, CalCommon::EIncludeAll, aSearchRange);
	
	test.Printf(_L("Test we found the expected number of instances!"));
	//test(instanceArray.Count() == 11);

	const TInt KInstanceCount(instanceArray.Count());
	for (TInt i(0) ; i < KInstanceCount ; ++i)
		{
		TDateTime dt = instanceArray[i]->Time().TimeLocalL().DateTime();
		test.Printf(_L("the time of instance [%d] = %d:%d %d/%d/%d: \n"), i, dt.Hour(), dt.Minute(),dt.Day() + 1, dt.Month() + 1, dt.Year());
		}

	test.Printf(_L("Test the times of the todos"));
	test(instanceArray[0]->Time().TimeLocalL() == iToday - TTimeIntervalDays(2));
	test(instanceArray[1]->Time().TimeLocalL() == iToday - TTimeIntervalDays(2));
	test(instanceArray[2]->Time().TimeLocalL() == iToday - TTimeIntervalDays(2));
		
	// completed todos are sorted by completed time
	test(instanceArray[3]->Time().TimeLocalL() == iToday - TTimeIntervalDays(1) - TTimeIntervalMinutes(7));
	test(instanceArray[4]->Time().TimeLocalL() == iToday - TTimeIntervalDays(1) - TTimeIntervalMinutes(4));
	test(instanceArray[5]->Time().TimeLocalL() == iToday - TTimeIntervalDays(1) - TTimeIntervalMinutes(2));	
	
	// due date should be Time::NullTTime but instance time should be set to completed time
	test(instanceArray[6]->Entry().EndTimeL().TimeLocalL() == Time::NullTTime());
	test(instanceArray[7]->Entry().EndTimeL().TimeLocalL() == Time::NullTTime());
	test(instanceArray[6]->Time().TimeLocalL() == iToday - TTimeIntervalMinutes(7));
	test(instanceArray[7]->Time().TimeLocalL() == iToday - TTimeIntervalMinutes(4));
	
	test(instanceArray[8]->Entry().EndTimeL().TimeLocalL() == Time::NullTTime());
	test(instanceArray[9]->Entry().EndTimeL().TimeLocalL() == Time::NullTTime());
	
	test(instanceArray[10]->Time().TimeLocalL() == iToday + TTimeIntervalDays(2));
	
	test.Printf(_L("Test the priority of the todos"));
	test(instanceArray[0]->Entry().PriorityL() == 2);
	test(instanceArray[1]->Entry().PriorityL() == 3);
	test(instanceArray[2]->Entry().PriorityL() == 4);

	CleanupStack::PopAndDestroy(&instanceArray);
	}
	
	
void CTestApp::CreateAndTestNonRptTodoL(const TTime& aStartTime, const TTime& aEndTime, const TTime& aCompletedTime, const CalCommon::TCalTimeRange& aTimeRange, TBool aExpected)
	{
	iTestLib->CleanDatabaseL();
	
	// Create and store entry
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	CCalEntry* todo = CreateTodoLC(aStartTime, aEndTime, aCompletedTime);
	entryArray.AppendL(todo);
	CleanupStack::Pop(todo);
	TInt numSuc;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	CleanupStack::PopAndDestroy(&entryArray);
	
	// find the todo
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceList, CalCommon::EIncludeAll, aTimeRange);
	PrintDetailsInstancesL(instanceList);
	
	if (aExpected)
		{
		// We are expecting an instance so test it
		test(instanceList.Count() == 1);
		
		// test the entry times
		if (aEndTime != Time::NullTTime())
			{
			// dated
			test(instanceList[0]->Entry().EndTimeL().TimeLocalL() == aEndTime);
			}
		else
			{
			// undated
			test(instanceList[0]->Entry().StartTimeL().TimeLocalL() == Time::NullTTime());
			test(instanceList[0]->Entry().EndTimeL().TimeLocalL() == Time::NullTTime());
			}
		
		// test the instance times
		if (instanceList[0]->Entry().CompletedTimeL().TimeLocalL() == Time::NullTTime())
			{
			// incomplete
			if (aEndTime != Time::NullTTime())
				{
				// the todo is dated
				
				// the instance due date should always be the same as the entry due date 
				test(instanceList[0]->EndTimeL().TimeLocalL() == aEndTime);
				
				if (aStartTime != Time::NullTTime())
					{
					// The entry has a start time so the instance should have the same start time
					test(instanceList[0]->StartTimeL().TimeLocalL() == aStartTime);
					}
				else
					{
					// The entry was not stored with a start time
					// so the instance start time should be the same as the due date
					test(instanceList[0]->StartTimeL().TimeLocalL() == instanceList[0]->EndTimeL().TimeLocalL());
					}
				}
			else
				{
				// The todo is undated
				// so test that the start and end times are the same and that they are set as some time today
				test(instanceList[0]->StartTimeL().TimeLocalL() == instanceList[0]->EndTimeL().TimeLocalL());
				test(instanceList[0]->StartTimeL().TimeLocalL() != Time::NullTTime());
				test(instanceList[0]->StartTimeL().TimeLocalL().DaysFrom(iToday).Int() == 0);
				}
			}
		else
			{
			// complete
			test(instanceList[0]->StartTimeL().TimeLocalL() == instanceList[0]->Entry().CompletedTimeL().TimeLocalL());
			test(instanceList[0]->EndTimeL().TimeLocalL() == instanceList[0]->Entry().CompletedTimeL().TimeLocalL());
			}
		}
	else
		{
		test(instanceList.Count() == 0);
		}
	CleanupStack::PopAndDestroy(&instanceList);
	}

void CTestApp::CreateAndTestRptTodoL(const TTime& aStartTime, const TTime& aEndTime, const TTime& aCompletedTime, TCalRRule::TType aRepeatType, TInt aInterval, TInt aCount, const CalCommon::TCalTimeRange& aTimeRange, RArray<TTime> aExpectedLocalInstanceTimes)
	{
	iTestLib->CleanDatabaseL();
	
	// Create and store repeating todo entry
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	CCalEntry* rptTodo = CreateRepeatingTodoLC(aStartTime, aEndTime, aCompletedTime, aRepeatType, aInterval, aCount);
	entryArray.AppendL(rptTodo);
	CleanupStack::Pop(rptTodo);
	TInt numSuc;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	CleanupStack::PopAndDestroy(&entryArray);
	
	// find the todo
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceList, CalCommon::EIncludeAll, aTimeRange);
	PrintDetailsInstancesL(instanceList);
	
	test(instanceList.Count() == aExpectedLocalInstanceTimes.Count());
	
	for (TInt i(0) ; i < instanceList.Count() ; ++i)
		{
		test(instanceList[i]->EndTimeL().TimeLocalL() == aExpectedLocalInstanceTimes[i]);
		}
		
	CleanupStack::PopAndDestroy(&instanceList);
	}
	
void CTestApp::TestFindInstanceNonRepeatingTodoL()
	{
	// set up a search range +- 10 days from now
	TCalTime startDateTime1;
	startDateTime1.SetTimeLocalL(iToday - TTimeIntervalDays(10));	
	TCalTime endDateTime1;
	endDateTime1.SetTimeLocalL(iToday + TTimeIntervalDays(10));
	CalCommon::TCalTimeRange timeRangeWithToday(startDateTime1, endDateTime1);
	
	TCalTime startDateTime2;
	startDateTime2.SetTimeLocalL(iToday + TTimeIntervalDays(1));	
	TCalTime endDateTime2;
	endDateTime2.SetTimeLocalL(iToday + TTimeIntervalDays(20));
	CalCommon::TCalTimeRange timeRangeAfterToday(startDateTime2, endDateTime2);
	
	TCalTime startDateTime3;
	startDateTime3.SetTimeLocalL(iToday - TTimeIntervalDays(20));	
	TCalTime endDateTime3;
	endDateTime3.SetTimeLocalL(iToday - TTimeIntervalDays(1));
	CalCommon::TCalTimeRange timeRangeBeforeToday(startDateTime3, endDateTime3);
	
	// completed todos
	test.Printf(_L("\n--Completed todo with completion date before the time range (due before)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2), // due before
						timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2),
						timeRangeWithToday,
						EFalse);
	
	test.Printf(_L("\n--Completed todo with completion date before the time range (due inside)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.StartTime().TimeLocalL() + TTimeIntervalDays(2), // due inside
						timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2),
						timeRangeWithToday,
						EFalse);
	
	test.Printf(_L("\n--Completed todo with completion date before the time range (due after)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2), // due after
						timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2),
						timeRangeWithToday,
						EFalse);
						
	test.Printf(_L("\n--Completed todo with completion date after the time range (due after)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2), // due after
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2),
						timeRangeWithToday,
						EFalse);
	
	test.Printf(_L("\n--Completed todo with completion date after the time range (due inside)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2), // due inside
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2),
						timeRangeWithToday,
						EFalse);
						
	test.Printf(_L("\n--Completed todo with completion date after the time range (due inside)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2), // due before
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2),
						timeRangeWithToday,
						EFalse);
						
	test.Printf(_L("\n--Completed todo with completion date inside the time range (due before)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2), // due before
						timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2),
						timeRangeWithToday,
						ETrue);
		
	test.Printf(_L("\n--Completed todo with completion date inside the time range (due after)--"));				
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2), // due after
						timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2),
						timeRangeWithToday,
						ETrue);
				
	test.Printf(_L("\n--Completed todo with completion date inside the time range (due inside)--"));		
	CreateAndTestNonRptTodoL(Time::NullTTime(),
						timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2), // due inside
						timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2),
						timeRangeWithToday,
						ETrue);
						
	// incomplete todos
	test.Printf(_L("\n--incomplete dated todo with start and due date before time range--"));
	CreateAndTestNonRptTodoL(timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2), // before
						timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2),
						Time::NullTTime(),
						timeRangeWithToday,
						EFalse);
			
	test.Printf(_L("\n--incomplete dated todo with only due date before time range--"));			
	CreateAndTestNonRptTodoL(Time::NullTTime(), // no start date
						timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2), // only due date before
						Time::NullTTime(),
						timeRangeWithToday,
						EFalse);
						
	test.Printf(_L("\n--incomplete dated todo crossing the start time of the search range--"));	
	CreateAndTestNonRptTodoL(timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(1), 
						timeRangeWithToday.StartTime().TimeLocalL() + TTimeIntervalDays(1), // crossing the start time
						Time::NullTTime(),
						timeRangeWithToday,
						ETrue);
						
	test.Printf(_L("\n--incomplete dated todo with start and due date inside the search range--"));
	CreateAndTestNonRptTodoL(timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2),
						timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2), // totally inside
						Time::NullTTime(),
						timeRangeWithToday,
						ETrue);
						
	test.Printf(_L("\n--incomplete dated todo with only due date inside the search range--"));	
	CreateAndTestNonRptTodoL(Time::NullTTime(), // no start date
						timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2), // only due date inside
						Time::NullTTime(),
						timeRangeWithToday,
						ETrue);
					
	test.Printf(_L("\n--incomplete dated todo crossing the end of the search range--"));
	CreateAndTestNonRptTodoL(timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(1), 
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(1), // crossing the end of the range
						Time::NullTTime(),
						timeRangeWithToday,
						ETrue);
						
	test.Printf(_L("\n--incomplete dated todo with start and due date after the search range--"));
	CreateAndTestNonRptTodoL(timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2),
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2), // totally after range
						Time::NullTTime(),
						timeRangeWithToday,
						EFalse);
	
	test.Printf(_L("\n--incomplete dated todo with only due date after the search range--"));					
	CreateAndTestNonRptTodoL(Time::NullTTime(), // no start date
						timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2), // only due after
						Time::NullTTime(),
						timeRangeWithToday,
						EFalse);
	
	// undated todos
	test.Printf(_L("\n--undated todo (completed before search range)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
								Time::NullTTime(),
								timeRangeWithToday.StartTime().TimeLocalL() - TTimeIntervalDays(2),// before
								timeRangeWithToday,
								EFalse); 
						
	test.Printf(_L("\n--undated todo (completed inside search range)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
								Time::NullTTime(),
								timeRangeWithToday.EndTime().TimeLocalL() - TTimeIntervalDays(2), // during
								timeRangeWithToday,
								ETrue);
						
	test.Printf(_L("\n--undated todo (completed after search range)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
								Time::NullTTime(),
								timeRangeWithToday.EndTime().TimeLocalL() + TTimeIntervalDays(2), // after
								timeRangeWithToday,
								EFalse);
						
	test.Printf(_L("\n--undated todo (incomplete)--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
								Time::NullTTime(),
								Time::NullTTime(), // incomplete
								timeRangeWithToday,
								ETrue);
								
	test.Printf(_L("\n----Test undated todos are not found when search range does not include today--"));
	test.Printf(_L("\n--Search for undated todos after today--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
								Time::NullTTime(),
								Time::NullTTime(), // incomplete
								timeRangeAfterToday,
								EFalse);
	
	test.Printf(_L("\n--Search for undated todos before today--"));
	CreateAndTestNonRptTodoL(Time::NullTTime(),
								Time::NullTTime(),
								Time::NullTTime(), // incomplete
								timeRangeBeforeToday,
								EFalse);
	
	}
	
void CTestApp::TestFindInstanceRepeatingTodoL()
	{	
	RArray<TTime> expectedLocalInstanceTimes;
	CleanupClosePushL(expectedLocalInstanceTimes);
	
	const TTime KArbitraryTime(TDateTime(2006, ENovember, 22, 11, 30, 0, 0));
	
// set up a search range +- 10 days from (11:30 23 November 2006)
	TCalTime startDateTime;
	startDateTime.SetTimeLocalL(KArbitraryTime - TTimeIntervalDays(10));	
	TCalTime endDateTime;
	endDateTime.SetTimeLocalL(KArbitraryTime + TTimeIntervalDays(10));
	CalCommon::TCalTimeRange timeRange(startDateTime, endDateTime);
	
// Create repeating todos of all different repeat types
	// Make sure instances of all repeat types have instances before, after, in and on the time range
	// 1. With and without a completed date set
	// 2. With and without a start date
	test.Printf(_L("\n--Testing Daily Repeats--"));
	test.Printf(_L("\n--incomplete with start date and due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(1));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(6));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(11));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(16));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(21));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(6), // The middle of the first instance is 5 days before
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4),
							Time::NullTTime(), // incomplete
							TCalRRule::EDaily,
							5, // interval
							8, // count
							timeRange,
							expectedLocalInstanceTimes);
	
	test.Printf(_L("\n--incomplete with only due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(1));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(6));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(11));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(16));
	CreateAndTestRptTodoL(Time::NullTTime(), // without start date
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4),
							Time::NullTTime(), // incomplete
							TCalRRule::EDaily,
							5,
							7,
							timeRange,
							expectedLocalInstanceTimes);
	
	test.Printf(_L("\n--Complete with only due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(1));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(6));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(11));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(16));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(21));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(6),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4),
							timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2), // complete inside
							TCalRRule::EDaily,
							5,
							7,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Complete with only due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(1));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(6));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(11));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(16));
	CreateAndTestRptTodoL(Time::NullTTime(), // without start date
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4),
							timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2), // complete outside
							TCalRRule::EDaily,
							5,
							7,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Testing weekly Repeats--"));
	test.Printf(_L("\n--Complete with only due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(3));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(10));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(17));
	CreateAndTestRptTodoL(Time::NullTTime(), // without start date
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(2), // complete outside
							TCalRRule::EWeekly,
							1,
							7,
							timeRange,
							expectedLocalInstanceTimes);
	
	test.Printf(_L("\n--Complete with start and due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(3));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(10));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(17));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(24));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(9),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(2), // complete outside
							TCalRRule::EWeekly,
							1,
							7,
							timeRange,
							expectedLocalInstanceTimes);
	test.Printf(_L("\n--Incomplete with only due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(3));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(10));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(17));
	CreateAndTestRptTodoL(Time::NullTTime(), // without start date
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4),
							Time::NullTTime(), // incomplete
							TCalRRule::EWeekly,
							1,
							7,
							timeRange,
							expectedLocalInstanceTimes);
	
	test.Printf(_L("\n--Incomplete with start and due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(3));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(10));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(17));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(24));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(9),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4),
							Time::NullTTime(), // incomplete
							TCalRRule::EWeekly,
							1,
							7,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Testing monthly Repeats--"));
	test.Printf(_L("\n--Complete with start and due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2) + TTimeIntervalMonths(1));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalMonths(1) - TTimeIntervalDays(20),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalMonths(1) + TTimeIntervalDays(2),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4), // complete before
							TCalRRule::EMonthly,
							1,
							4,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Complete with only due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2));
	CreateAndTestRptTodoL(Time::NullTTime(),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalMonths(1) + TTimeIntervalDays(2),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(4), // complete before
							TCalRRule::EMonthly,
							1,
							4,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Incomplete with start and due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2));
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2) + TTimeIntervalMonths(1));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalMonths(1) - TTimeIntervalDays(20),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalMonths(1) + TTimeIntervalDays(2),
							Time::NullTTime(), // incomplete
							TCalRRule::EMonthly,
							1,
							4,
							timeRange,
							expectedLocalInstanceTimes);
	test.Printf(_L("\n--Incomplete with only due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2));
	CreateAndTestRptTodoL(Time::NullTTime(),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalMonths(1) + TTimeIntervalDays(2),
							Time::NullTTime(), // incomplete
							TCalRRule::EMonthly,
							1,
							4,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Testing yearly Repeats--"));
	test.Printf(_L("\n--Complete with only due date set--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2));
	CreateAndTestRptTodoL(Time::NullTTime(),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalYears(1) + TTimeIntervalDays(2),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(2), // complete before
							TCalRRule::EYearly,
							1,
							3,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Complete with start and due date set (across start of range)--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(2));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalYears(1) - TTimeIntervalDays(2),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalYears(1) + TTimeIntervalDays(2),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(2), // complete before
							TCalRRule::EYearly,
							1,
							3,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Complete with start and due date set (inside range)--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.StartTime().TimeLocalL() + TTimeIntervalDays(4));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalYears(1) + TTimeIntervalDays(2),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalYears(1) + TTimeIntervalDays(4),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(2), // complete before
							TCalRRule::EYearly,
							1,
							3,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Complete with start and due date set (across end of range)--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.EndTime().TimeLocalL() + TTimeIntervalDays(2));
	CreateAndTestRptTodoL(timeRange.EndTime().TimeLocalL() - TTimeIntervalYears(1) - TTimeIntervalDays(2),
							timeRange.EndTime().TimeLocalL() - TTimeIntervalYears(1) + TTimeIntervalDays(2),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(2), // complete before
							TCalRRule::EYearly,
							1,
							3,
							timeRange,
							expectedLocalInstanceTimes);
							
	test.Printf(_L("\n--Complete with start and due date set (across whole range)--"));
	expectedLocalInstanceTimes.Reset();
	expectedLocalInstanceTimes.AppendL(timeRange.EndTime().TimeLocalL() + TTimeIntervalDays(2));
	CreateAndTestRptTodoL(timeRange.StartTime().TimeLocalL() - TTimeIntervalYears(1) - TTimeIntervalDays(2),
							timeRange.EndTime().TimeLocalL() - TTimeIntervalYears(1) + TTimeIntervalDays(2),
							timeRange.StartTime().TimeLocalL() - TTimeIntervalDays(2), // complete before
							TCalRRule::EYearly,
							1,
							3,
							timeRange,
							expectedLocalInstanceTimes);
							
							
	CleanupStack::PopAndDestroy(&expectedLocalInstanceTimes);
	}
	
void CTestApp::TestNextInstanceL()
	{
	// create TCalTime for a day after today
	TCalTime today;
	today.SetTimeLocalL(iToday + TTimeIntervalDays(1));
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	// create a todo that is 20 days in the future
	CCalEntry* entry = CreateTodoLC(iToday + TTimeIntervalDays(20),
									iToday + TTimeIntervalDays(20),
									Time::NullTTime());
									
	HBufC* entryDueTimeBuf = TimeBufLC(entry->EndTimeL().TimeLocalL());			
	test.Printf(_L("Adding an entry with due time := %s"), entryDueTimeBuf->Ptr());
	CleanupStack::PopAndDestroy(entryDueTimeBuf);
	
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	TInt numSuc;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	entryArray.ResetAndDestroy();
	
	TCalTime nextTime = iTestLib->SynCGetInstanceViewL().NextInstanceL(CalCommon::EIncludeAll, today);
	
	HBufC* nextTimeBuf = TimeBufLC(nextTime.TimeLocalL());
	test.Printf(_L("Original NextInstanceL(), from tomorrow := %s"), nextTimeBuf->Ptr());
	CleanupStack::PopAndDestroy(nextTimeBuf);
	
	CCalEntry* todo = CreateTodoLC(Time::NullTTime(),
									Time::NullTTime(),
									Time::NullTTime());
	
	entryArray.AppendL(todo);
	CleanupStack::Pop(todo);
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	CleanupStack::PopAndDestroy(&entryArray);
	
	TCalTime nextTime2 = iTestLib->SynCGetInstanceViewL().NextInstanceL(CalCommon::EIncludeAll, today);
	
	HBufC* nextTime2Buf = TimeBufLC(nextTime.TimeLocalL());
	test.Printf(_L("NextInstanceL() with undated todo added, from tomorrow := %s"), nextTime2Buf->Ptr());
	CleanupStack::PopAndDestroy(nextTime2Buf);
	
	test(nextTime.TimeLocalL() == nextTime2.TimeLocalL());
	}



void CTestApp::DoTestsL()
	{
	// Set up time range for +- 10 days
	TCalTime startDateTime;
	startDateTime.SetTimeLocalL(iToday - TTimeIntervalDays(10));	
	TCalTime endDateTime;
	endDateTime.SetTimeLocalL(iToday + TTimeIntervalDays(10));
	CalCommon::TCalTimeRange timeRange(startDateTime, endDateTime);
	
	// Set up time range for min max time
	TCalTime minCalTime;
	minCalTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxCalTime;
	maxCalTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRangeMinToMax(minCalTime, maxCalTime);


	test.Next(_L("Test that todos are sorted in the correct order (with time range +- 10 days)"));

	iTestLib->CleanDatabaseL();
	TestTodoSortOrderL(timeRange);
	
	test.Next(_L("Test that todos are sorted in the correct order (with time range min at max time)"));

	iTestLib->CleanDatabaseL();
	TestTodoSortOrderL(timeRangeMinToMax);

	
	test.Next(_L("Test all types of non-repeating todos are returned correctly"));

	iTestLib->CleanDatabaseL();
	TestFindInstanceNonRepeatingTodoL();
	
	test.Next(_L("Test all types of repeating todos are returned correctly"));

	iTestLib->CleanDatabaseL();
	TestFindInstanceRepeatingTodoL();

	test.Next(_L("Test NextInstanceL with and without an undated todo"));

	iTestLib->CleanDatabaseL();
	TestNextInstanceL();
	}

static void doMainL()
	{
	CTestApp* app = CTestApp::NewL();
	CleanupStack::PushL(app);
	
	app->DoTestsL();

	CleanupStack::PopAndDestroy(app);
	}

/**

@SYMTestCaseID     PIM-TCAL-UNDATEDTODO-0001

*/

TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-UNDATEDTODO-0001 tCal_Instance"));

	test.Title();
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	TRAPD(ret,doMainL());
	delete scheduler;
	test(ret == KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

