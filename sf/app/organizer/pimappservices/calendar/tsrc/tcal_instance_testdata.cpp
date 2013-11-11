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

#include "tcal_instance_testdata.h"
#include "caltestlib.h"
	
#include <calalarm.h>
#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calrrule.h>
#include <caluser.h>
#include "caltestlib.h"
#include <e32debug.h>
#include <e32test.h>


CTestDataStruct::CTestDataStruct()
	{
	iFilter = CalCommon::EIncludeAll;
	iSearchBehaviour = CalCommon::EExactTextSearch;
	}

	
CTestDataStruct::~CTestDataStruct()
	{
	iTimes.Reset();
	
	if(iFunction)
		{
		iFunction->CallBack();
		}
	
	delete iFunction;
	delete iSearchString;
	}


CTestDataStruct* CTestData::GenerateTestDataLC(TTestNumber aTestNumber, CCalInstanceView* aInstanceView, CCalEntryView* aEntryView)
	{
	CTestDataStruct* testData = new (ELeave) CTestDataStruct();
	CleanupStack::PushL(testData);
	
	switch (aTestNumber)
		{
		case EDailyTest1:	CreateDailyTest1L(*testData, aEntryView); 	break;
		case EDailyTest2:	CreateDailyTest2L(*testData, aEntryView); 	break;
		case EDailyTest3:	CreateDailyTest3L(*testData, aEntryView); 	break;
		case EDailyTest4:	CreateDailyTest4L(*testData, aEntryView); 	break;
		case EDailyTest5:	CreateDailyTest5L(*testData, aEntryView); 	break;
		case EDailyTest6:	CreateDailyTest6L(*testData, aEntryView); 	break;
		case EDailyTest7:	CreateDailyTest7L(*testData, aEntryView); 	break;
		case EWeeklyTest1:	CreateWeeklyTest1L(*testData, aEntryView); 	break;
		case EWeeklyTest2:	CreateWeeklyTest2L(*testData, aEntryView); 	break;
		case EWeeklyTest3:	CreateWeeklyTest3L(*testData, aEntryView); 	break;
		case EWeeklyTest4:	CreateWeeklyTest4L(*testData, aEntryView); 	break;
		case EWeeklyTest1980_5: CreateWeeklyTest1980_5L(*testData, aEntryView); break;
		case EMonthlyTest1:	CreateMonthlyTest1L(*testData, aEntryView); break;
		case EMonthlyTest2:	CreateMonthlyTest2L(*testData, aEntryView); break;
		case EMonthlyTest3:	CreateMonthlyTest3L(*testData, aEntryView); break;
		case EMonthlyTest4:	CreateMonthlyTest4L(*testData, aEntryView); break;
		case EYearlyTest1:	CreateYearlyTest1L(*testData, aEntryView); 	break;
		case EYearlyTest2:	CreateYearlyTest2L(*testData, aEntryView); 	break;
		case EYearlyTest3:	CreateYearlyTest3L(*testData, aEntryView); 	break;
		case EYearlyTest4:	CreateYearlyTest4L(*testData, ETrue, aEntryView); 	break;
		case EYearlyTest5:	CreateYearlyTest4L(*testData, EFalse, aEntryView); 	break;
		case EYearlyTest1980_6: CreateYearlyTest1980_6L(*testData, aEntryView); break;
		case EMultipleTest1:CreateMultipleRuleTest1L(*testData, aEntryView);	break;
		case EMultipleTest2:CreateMultipleRuleTest2L(*testData, aEntryView);	break;
		case EFilterTest1:  CreateFilterTest1L(*testData, aEntryView); break;
		case EFilterTest2: 	CreateFilterTest2L(*testData, aEntryView); break;
		case EFilterTest3: 	CreateFilterTest3L(*testData, aEntryView); break;
		case EFilterTest4: 	CreateFilterTest4L(*testData, aEntryView); break;
		case EFilterTest5: 	CreateFilterTest5L(*testData, aEntryView); break;
		case EFilterTest6: 	CreateFilterTest6L(*testData, aEntryView); break;
		case EFilterTest7: 	CreateFilterTest7L(*testData, aEntryView); break;
		case EDeleteTest1: 	CreateDeleteTest1L(*testData, aEntryView, aInstanceView); break;
		case EDeleteTest2: 	CreateDeleteTest2L(*testData, aEntryView, aInstanceView); break;
		case EDeleteTest3: 	CreateDeleteTest3L(*testData, aEntryView, aInstanceView); break;
		case EDeleteTest4: 	CreateDeleteTest4L(*testData, aEntryView, aInstanceView); break;
		case EFindStringTest1: CreateStringTest1L(*testData, aEntryView); break;
		case EFindStringTest2: CreateStringTest2L(*testData, aEntryView); break;
		case EFindStringTest3: CreateStringTest3L(*testData, aEntryView); break;
		case EFindStringTest4: CreateStringTest4L(*testData, aEntryView); break;
		case EFindStringInFieldTest1: CreateStringInFieldTest1L(*testData, aEntryView); break;
		case EFindStringInFieldTest2: CreateStringInFieldTest2L(*testData, aEntryView); break;
		case EFindStringInFieldTest3: CreateStringInFieldTest3L(*testData, aEntryView); break;
		case EFindStringInFieldTest4: CreateStringInFieldTest4L(*testData, aEntryView); break;
		case EFindStringInFieldTest5: CreateStringInFieldTest5L(*testData, aEntryView); break;
		case EFindStringInFieldTest6: CreateStringInFieldTest6L(*testData, aEntryView); break;
		case EStartEndTimesTest1: CreateStartEndTimesTest1L(*testData, aEntryView); break;
		case EStartEndTimesTest2: CreateStartEndTimesTest2L(*testData, aEntryView); break;
		case EStartEndTimesTest3: CreateStartEndTimesTest3L(*testData, aEntryView); break;
		case EStartEndTimesTest4: CreateStartEndTimesTest4L(*testData, aEntryView); break;
		case EStartEndTimesTest5: CreateStartEndTimesTest5L(*testData, aEntryView); break;
		case EStartEndTimesTest6: CreateStartEndTimesTest6L(*testData, aEntryView); break;
		case EStartEndTimesTest7: CreateStartEndTimesTest7L(*testData, aEntryView); break;
		case EManyInstancesTest: CreateManyInstancesTestL(*testData, aEntryView); break;
		case EManyInstancesTest2: CreateManyInstancesTest2L(*testData, aEntryView); break;
		case EManyInstancesTest3: CreateManyInstancesTest3L(*testData, aEntryView); break;
		case EMaxRangeTest: CreateMaxRangeTestL(*testData, aEntryView); break;
		case EMaxRangeTest2: CreateMaxRangeTest2L(*testData, aEntryView); break;
		case EMaxRangeTest3: CreateMaxRangeTest3L(*testData, aEntryView); break;
		case EMaxRangeTest4: CreateMaxRangeTest4L(*testData, aEntryView); break;
		case EMaxRangeTest5: CreateMaxRangeTest5L(*testData, aEntryView); break;
		case EMaxRangeTest6: CreateMaxRangeTest6L(*testData, aEntryView); break;
		case EMaxRangeTest7: CreateMaxRangeTest7L(*testData, aEntryView); break;
		case EMaxRangeTest8: CreateMaxRangeTest8L(*testData, aEntryView); break;
		case EEmptyDatabase1: EmptyDatabase1L(*testData, aEntryView); break;
		case EEmptyDatabase2: EmptyDatabase2L(*testData, aEntryView); break;
		case ESecondGranularityTest: CreateRepeatingWithSecondGranularityL(*testData, aEntryView); 	break;
		case EMSecondGranularityTest: CreateRepeatingWithMSecondGranularityL(*testData, aEntryView); 	break;
		case ERuleAndDateTest: CreateRepeatingWithRuleAndDatesL(*testData, aEntryView); 	break;
		case ENextInstanceOnlyTest: CreateNextInstanceOnlyTestL(*testData, aEntryView); 	break;
		case EDeleteInstanceTest:DeleteEntryLastInstanceL(*testData, aEntryView, aInstanceView); 	break;
		case EUpdateInstanceTest : TestUpdateInstanceL(*testData, aEntryView);	break;
        case EFindInstanceWithTimeTest: TestFindInstanceWithTimeL( *testData, aEntryView ); break;
		case ERDateBeforeDtStartTest: CreateRDateBeforeDtStartTestL(*testData, aEntryView);	break;
		case EDstBoundaryTest1: CreateDstBoundaryTest1L(*testData, aEntryView);	break;
		case EDstBoundaryTest2: CreateDstBoundaryTest2L(*testData, aEntryView);	break;
		case EDstBoundaryTest3: CreateDstBoundaryTest3L(*testData, aEntryView);	break;
		case EDstBoundaryTest4: CreateDstBoundaryTest4L(*testData, aEntryView);	break;
		case EDstBoundaryTest5: CreateDstBoundaryTest5L(*testData, aEntryView);	break;
		case EExcludeInstanceAdjoiningRangeTestNonRepeat: ExcludeInstanceAdjoiningRangeTestNonRepeatL(*testData, aEntryView); break;
		case EExcludeInstanceAdjoiningRangeTestRepeat: ExcludeInstanceAdjoiningRangeTestRepeatL(*testData, aEntryView); break;
		case ETestTodoOnMaxDate: TestTodoOnMaxDateL(*testData, aEntryView); break;
		// shouldn't get here 
		default:			User::Leave(KErrCorrupt); break;
		}
		
	return testData;
	}


// Add an entry to the calendar file with the given repeat rule
void CTestData::AddRuleL(const TCalRRule& aRpt, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType, TTimeIntervalHours aDuration)
	{
	CCalEntry* entry = CreateEntryLC(aType, aGuid, aRpt.DtStart().TimeLocalL(), aRpt.DtStart().TimeLocalL() + aDuration);
	entry->SetRRuleL(aRpt); // doesn't take ownership
		
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry);
	}


CCalEntry* CTestData::CreateEntryLC(CCalEntry::TType aType, const TDesC8& aGuid, const TTime& aStartTimeLocal, const TTime& aEndTimeLocal)
	{
	HBufC8* guid = aGuid.AllocLC();
	CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime calTime;
	calTime.SetTimeLocalL(aStartTimeLocal);
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(aEndTimeLocal);
	entry->SetStartAndEndTimeL(calTime, calEndTime);
	
	return entry;
	}


void CTestData::StoreEntryWithTextL(CCalEntryView* aEntryView, CCalEntry::TType aType, const TDesC8& aGuid, const TTime& aStartTimeLocal, const TDesC& aSummaryText)
	{
	CCalEntry* entry = CreateEntryLC(aType, aGuid, aStartTimeLocal, aStartTimeLocal + TTimeIntervalHours(1));
	entry->SetSummaryL(aSummaryText);
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PopAndDestroy(entry);
	}


void CTestData::CreateDailyTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05. Get instances between 28 Feb 05 and 5 Mar 05."));

	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetDtStart(startTime); 	// 24 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 14, 0, 0, 0));
	rpt.SetUntil(endTime);			// 2008
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EFebruary, 27, 0, 0, 0, 0);		// 28 Feb
	aData.iEndDate = TDateTime(2005, EMarch, 4, 0, 0, 0, 0); 			// 5 Mar

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMarch, 1, 0, 0, 0, 0))); // 2 Mar
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMarch, 4, 0, 0, 0, 0))); // 5 Mar
	}

	
void CTestData::CreateDailyTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05. Get instances between 20 Feb 05 and 24 Feb 05."));
	// check instance can be found when it starts at end search time

	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetDtStart(startTime);
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2005, EMarch, 30, 0, 0, 0, 0));
	rpt.SetUntil(endTime);			// 31 Mar
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EFebruary, 19, 0, 0, 0, 0);		// 20 Feb
	aData.iEndDate = TDateTime(2005, EFebruary, 23, 0, 0, 0, 0); 			// 24 Feb

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0))); // 24 Feb
	}


void CTestData::CreateDailyTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05 until 29 Mar. Get instances between 29 Mar 05 and 6 Apr 05."));
	// check instance can be found when it finishes at start search time

	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0)); 	// 24 Feb
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2005, EMarch, 28, 0, 0, 0, 0));		// 29 Mar
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EMarch, 28, 0, 0, 0, 0);		// 29 Mar
	aData.iEndDate = TDateTime(2005, EApril, 5, 0, 0, 0, 0); 			// 6 Apr

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMarch, 28, 0, 0, 0, 0))); // 29 Mar
	}


void CTestData::CreateDailyTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05 until 27 Feb. Get instances between 24 Feb 05 and 27 Feb 05."));
	// check instance can be found when it starts at start search time and ends at end search time

	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0)); 	// 24 Feb
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2005, EFebruary, 26, 0, 0, 0, 0));
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EFebruary, 23, 0, 0, 0, 0);		// 24 Feb
	aData.iEndDate = TDateTime(2005, EFebruary, 26, 0, 0, 0, 0); 		// 27 Feb

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0))); // 24 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 26, 0, 0, 0, 0))); // 27 Feb
	}


void CTestData::CreateDailyTest5L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 10 days from 28 Jan 05. Get instances between 28 Jan 05 and 28 Jan 05."));
	// check instance can be found when the search time range is 0

	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(10);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 27, 0, 0, 0, 0)); 	// 28 Jan
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EApril, 1, 0, 0, 0, 0));		// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EJanuary, 27, 0, 0, 0, 0);	// 28 Jan
	aData.iEndDate = TDateTime(2005, EJanuary, 27, 0, 0, 0, 0); 	// 28 Jan

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJanuary, 27, 0, 0, 0, 0))); // 28 Jan
	}


void CTestData::CreateDailyTest6L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every day from 26 Jan 05. Get instances between 28 Jan 05 and 27 Jan 05 (end date earlier than start)."));
	// check instance isn't found when the search time range is invalid

	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(1);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0)); 	// 26 Jan
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 0, 0, 0, 0));		// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EJanuary, 27, 0, 0, 0, 0);	// 28 Jan
	aData.iEndDate = TDateTime(2005, EJanuary, 26, 0, 0, 0, 0); 	// 27 Jan

	// expected times:
	// NONE
	}


//Test code added for DEF063984
void CTestData::CreateDailyTest7L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Set Until date equal to Start date which is 24 Feb 05 and check whether CCalEntry::SetRRuleL leaves with EAgmErrBadEntry"));
	
	RTest test(_L("CreateDailyTest7L"));
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(1);
	TCalTime startTime;
	TCalTime endTime;
	
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetDtStart(startTime); 	// 24 Feb 2005
	endTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetUntil(endTime);		// 24 Feb 2005
	TRAPD(ret,AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt));
	test(ret==KErrArgument);
	
	aData.iStartDate = TDateTime(2005, EFebruary, 23, 0, 0, 0, 0);
	aData.iEndDate = TDateTime(2005, EFebruary, 23, 0, 0, 0, 0);
	}


void CTestData::CreateWeeklyTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every Wed/Thur from 6 Apr 05 until 21 Apr 05. Get instances between Sun 27 Mar 05 and Sun 24 Apr 05."));

	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EWednesday);
	days.AppendL(EThursday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 5, 0, 0, 0, 0)); 		// 6 Apr
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2005, EApril, 20, 0, 0, 0, 0));			// 21 Apr
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EMarch, 26, 0, 0, 0, 0);			// 27 Mar
	aData.iEndDate = TDateTime(2005, EApril, 23, 0, 0, 0, 0); 			// 24 Apr

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 5, 0, 0, 0, 0)));		// Wed   6 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 6, 0, 0, 0, 0)));		// Thur  7 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0)));	// Wed  13 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 13, 0, 0, 0, 0)));	// Thur 14 Apr	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 19, 0, 0, 0, 0)));	// Wed 20 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 20, 0, 0, 0, 0)));	// Thur 21 Apr
	}


void CTestData::CreateWeeklyTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every Wed/Thur from 3 Apr 05. Get instances between Thur 14 Apr 05 and Wed 27 Apr 05."));
	
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EWednesday);
	days.AppendL(EThursday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 2, 0, 0, 0, 0)); 		// 3 Apr
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2005, EApril, 26, 0, 0, 0, 0));			// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EApril, 13, 0, 0, 0, 0);			// 14 Apr
	aData.iEndDate = TDateTime(2005, EApril, 26, 0, 0, 0, 0); 			// 27 Apr

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 13, 0, 0, 0, 0)));	// Thur 14 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 19, 0, 0, 0, 0)));	// Wed  20 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 20, 0, 0, 0, 0)));	// Thur 21 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 26, 0, 0, 0, 0)));	// Wed  27 Apr
	}


void CTestData::CreateWeeklyTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every day from 3 Apr 05. Get instances between 2 Apr 05 and 15 Apr 05."));
	
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EMonday);
	days.AppendL(ETuesday);
	days.AppendL(EWednesday);
	days.AppendL(EThursday);
	days.AppendL(EFriday);
	days.AppendL(ESaturday);
	days.AppendL(ESunday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 2, 0, 0, 0, 0)); 		// 3 Apr
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 0, 0, 0, 0));			// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EApril, 1, 0, 0, 0, 0);			// 2 Apr
	aData.iEndDate = TDateTime(2005, EApril, 14, 0, 0, 0, 0); 			// 15 Apr

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 2, 0, 0, 0, 0)));	//  3 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 3, 0, 0, 0, 0)));	//  4 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 4, 0, 0, 0, 0)));	//  5 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 5, 0, 0, 0, 0)));	//  6 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 6, 0, 0, 0, 0)));	//  7 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));	//  8 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 8, 0, 0, 0, 0)));	//  9 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 9, 0, 0, 0, 0)));	// 10 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 10, 0, 0, 0, 0)));// 11 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 11, 0, 0, 0, 0)));// 12 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0)));// 13 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 13, 0, 0, 0, 0)));// 14 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));// 15 Apr
	}


void CTestData::CreateWeeklyTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 10th Sunday from 3 Apr 05. Get instances between 3 Apr 05 and 1 Dec 05."));
	
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(10);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(ESunday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 2, 0, 0, 0, 0)); 		// 3 Apr
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 0, 0, 0, 0));			// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EApril, 2, 0, 0, 0, 0);			// 3 Apr
	aData.iEndDate = TDateTime(2005, EDecember, 0, 0, 0, 0, 0); 		// 1 Dec

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 2, 0, 0, 0, 0)));		//  3 Apr
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJune, 11, 0, 0, 0, 0)));		// 12 Jun
	aData.iTimes.AppendL(TTime(TDateTime(2005, EAugust, 20, 0, 0, 0, 0)));	// 21 Aug
	aData.iTimes.AppendL(TTime(TDateTime(2005, EOctober, 29, 0, 0, 0, 0)));	// 30 Oct
	}


void CTestData::CreateMonthlyTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every month on the 1st and 31st from 1 Apr 05. Get instances between 1 Nov 05 and 1 Mar 06."));

	TCalRRule rpt(TCalRRule::EMonthly);
	rpt.SetInterval(1);
	RArray<TInt> days;
	CleanupClosePushL(days);
	days.AppendL(0);
	days.AppendL(30);
	rpt.SetByMonthDay(days);
	CleanupStack::PopAndDestroy(&days);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 0, 0, 0, 0, 0)); 		// 1 Apr
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 0, 0, 0, 0));			// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, ENovember, 0, 0, 0, 0, 0);		// 1 Nov 05
	aData.iEndDate = TDateTime(2006, EMarch, 0, 0, 0, 0, 0); 			// 1 Mar 06

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, ENovember, 0, 0, 0, 0, 0)));	//  1 Nov 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, ENovember, 29, 0, 0, 0, 0)));	// 30 Nov 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EDecember, 0, 0, 0, 0, 0)));	//  1 Dec 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EDecember, 30, 0, 0, 0, 0)));	// 31 Dec 05
	aData.iTimes.AppendL(TTime(TDateTime(2006, EJanuary, 0, 0, 0, 0, 0)));	//  1 Jan 06
	aData.iTimes.AppendL(TTime(TDateTime(2006, EJanuary, 30, 0, 0, 0, 0)));	// 31 Jan 06
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 0, 0, 0, 0, 0)));	//  1 Feb 06
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 27, 0, 0, 0, 0)));	// 28 Feb 06
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 0, 0, 0, 0, 0)));		//  1 Mar 06
	}


void CTestData::CreateMonthlyTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 5 months on the 10th from 11 Apr 05. Get instances between 1 Jan 05 and 1 Jan 07."));

	TCalRRule rpt(TCalRRule::EMonthly);
	rpt.SetInterval(5);
	RArray<TInt> days;
	CleanupClosePushL(days);
	days.AppendL(9);
	rpt.SetByMonthDay(days);
	CleanupStack::PopAndDestroy(&days);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 10, 0, 0, 0, 0)); 		// 11 Apr
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 0, 0, 0, 0));			// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0);		// 1 Jan 05
	aData.iEndDate = TDateTime(2007, EJanuary, 0, 0, 0, 0, 0); 			// 1 Jan 07

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, ESeptember, 9, 0, 0, 0, 0)));// 10 Sep 05
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 9, 0, 0, 0, 0)));	// 10 Feb 06
	aData.iTimes.AppendL(TTime(TDateTime(2006, EJuly, 9, 0, 0, 0, 0)));		// 10 Jul 06
	aData.iTimes.AppendL(TTime(TDateTime(2006, EDecember, 9, 0, 0, 0, 0)));	// 10 Dec 06
	}


void CTestData::CreateMonthlyTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 4th + last Mon in the month from 11 Jun 05. Get instances between 1 Jul 05 and 31 Oct 05."));

	TCalRRule rpt(TCalRRule::EMonthly);
	rpt.SetInterval(1);
	
	RArray<TCalRRule::TDayOfMonth> dayArray;
	CleanupClosePushL(dayArray);
	TCalRRule::TDayOfMonth day1(EMonday, - 1);
	dayArray.AppendL(day1);
	TCalRRule::TDayOfMonth day2(EMonday, 4);
	dayArray.AppendL(day2);
	rpt.SetByDay(dayArray);
	CleanupStack::PopAndDestroy(&dayArray);
	
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJune, 10, 10, 0, 0, 0)); 			// 11 Jun
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 0, 0, 0, 0));			// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EJuly, 0, 0, 0, 0, 0);			// 1 Jul 05
	aData.iEndDate = TDateTime(2005, EOctober, 30, 10, 0, 0, 0); 		// 31 Oct 05

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJuly, 24, 10, 0, 0, 0)));		// 25 Jul 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EAugust, 21, 10, 0, 0, 0)));	// 22 Aug 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EAugust, 28, 10, 0, 0, 0)));	// 29 Aug 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, ESeptember, 25, 10, 0, 0, 0)));// 26 Sep 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EOctober, 23, 10, 0, 0, 0)));	// 24 Oct 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EOctober, 30, 10, 0, 0, 0)));	// 31 Oct 05
	}


void CTestData::CreateMonthlyTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every second Wed every 3 months from 11 Jun 05. Get instances between 1 Jun 05 and 1 Mar 06."));

	TCalRRule rpt(TCalRRule::EMonthly);
	rpt.SetInterval(3);
	
	RArray<TCalRRule::TDayOfMonth> dayArray;
	CleanupClosePushL(dayArray);
	TCalRRule::TDayOfMonth day(EWednesday, 2);
	dayArray.AppendL(day);
	rpt.SetByDay(dayArray);
	CleanupStack::PopAndDestroy(&dayArray);
	
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJune, 10, 10, 0, 0, 0)); 			// 11 Jun
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 0, 0, 0, 0));			// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EJune, 0, 0, 0, 0, 0);			// 1 Jun 05
	aData.iEndDate = TDateTime(2006, EMarch, 0, 0, 0, 0, 0); 			// 1 Mar 06

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, ESeptember, 13, 10, 0, 0, 0)));// 14 Sep 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EDecember, 13, 10, 0, 0, 0)));	// 14 Dec 05
	}


void CTestData::CreateWeeklyTest1980_5L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Three consecutive Mondays from 8 March 1920."));
	
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EMonday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);

	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime (1920,EMarch,1,21,30,0,0)); 
	rpt.SetDtStart(startTime); 
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(1920,EMarch,1,21,30,0,0);	
	aData.iEndDate = TDateTime(1920,EMarch,24,22,30,0,0); 	

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(1920, EMarch, 7, 21,30, 0, 0)));		//  8 Mar
	aData.iTimes.AppendL(TTime(TDateTime(1920, EMarch, 14, 21,30, 0, 0)));		// 15 Mar
	aData.iTimes.AppendL(TTime(TDateTime(1920, EMarch, 21, 21,30, 0, 0)));	// 22 Mar
	}


void CTestData::CreateYearlyTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every year from 29 Feb 04. Get instances between 28 Feb 06 and 1 Jun 08."));
	
	TCalRRule rpt(TCalRRule::EYearly);
	rpt.SetInterval(1);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2004, EFebruary, 28, 0, 0, 0, 0)); 	// 29 Feb 2004
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2009, EMarch, 30, 0, 0, 0, 0));			// 2009
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2006, EFebruary, 27, 0, 0, 0, 0);		// 28 Feb 06
	aData.iEndDate = TDateTime(2008, EJune, 0, 0, 0, 0, 0); 			//  1 Jun 08

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 27, 0, 0, 0, 0)));	// 28 Feb 06
	aData.iTimes.AppendL(TTime(TDateTime(2007, EFebruary, 27, 0, 0, 0, 0)));	// 28 Feb 07
	aData.iTimes.AppendL(TTime(TDateTime(2008, EFebruary, 28, 0, 0, 0, 0)));	// 29 Feb 08
	}


void CTestData::CreateYearlyTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 2 years from 15 Jun 05. Get instances between 1 Jan 05 and 1 Jan 10."));
	
	TCalRRule rpt(TCalRRule::EYearly);
	rpt.SetInterval(2);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJune, 14, 0, 0, 0, 0)); 		// 15 Wed Jun
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2012, EMarch, 26, 0, 0, 0, 0));			// 2012
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0);		// 1 Jan 05
	aData.iEndDate = TDateTime(2010, EJanuary, 0, 0, 0, 0, 0); 			// 1 Jan 10

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJune, 14, 0, 0, 0, 0)));	// 15 Jun 05
	aData.iTimes.AppendL(TTime(TDateTime(2007, EJune, 14, 0, 0, 0, 0)));	// 15 Jun 07
	aData.iTimes.AppendL(TTime(TDateTime(2009, EJune, 14, 0, 0, 0, 0)));	// 15 Jun 09
	}
	

void CTestData::CreateYearlyTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every year on last Sun in Feb from 1 Jan 05. Get instances between 1 Jan 05 and 1 Jan 07."));

	TCalRRule rpt(TCalRRule::EYearly);
	rpt.SetInterval(1);
	
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 0, 0, 0, 0, 0)); 	// 1 Jan
	rpt.SetDtStart(startTime); 
	
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2012, EMarch, 30, 14, 0, 0, 0)); 	// 2012
	rpt.SetUntil(endTime); 
	
	TCalRRule::TDayOfMonth dayOfMonth(ESunday, - 1);
	RArray<TCalRRule::TDayOfMonth> dayArray;
	CleanupClosePushL(dayArray);
	dayArray.AppendL(dayOfMonth);
	rpt.SetByDay(dayArray);
	CleanupStack::PopAndDestroy(&dayArray);
	
	RArray<TMonth> monthArray;
	CleanupClosePushL(monthArray);
	monthArray.AppendL(EFebruary);
	rpt.SetByMonth(monthArray);
	CleanupStack::PopAndDestroy(&monthArray);
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0);		// 1 Jan 05
	aData.iEndDate = TDateTime(2007, EJanuary, 0, 0, 0, 0, 0); 			// 1 Jan 07

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 26, 0, 0, 0, 0)));	// 27 Feb 05
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 25, 0, 0, 0, 0)));	// 26 Feb 06
	}


void CTestData::CreateYearlyTest4L(CTestDataStruct& aData, TBool aRepeatForever, CCalEntryView* aEntryView)
	{
	if (!aRepeatForever)
		{
		aData.iDescription.Copy(_L("Every 2 years on 2nd Thu in Nov, from 1 Jan 05. Get instances between 1 Jan 05 and 1 Jan 10."));
		}
	else
		{
		aData.iDescription.Copy(_L("Every 2 years on 2nd Thu in Nov, from 1 Jan 05. Get instances between 1 Jan 05 and 1 Jan 10. (No repeat end date)"));
		}
		
	TCalRRule rpt(TCalRRule::EYearly);
	rpt.SetInterval(2);
	
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 0, 0, 0, 0, 0)); 	// 1 Jan
	rpt.SetDtStart(startTime); 

	if (!aRepeatForever)
		{
		TCalTime endTime;
		endTime.SetTimeLocalL(TDateTime(2012, EMarch, 30, 14, 0, 0, 0)); 	// 2012
		rpt.SetUntil(endTime);
		}

	TCalRRule::TDayOfMonth dayOfMonth(EThursday, 2);
	RArray<TCalRRule::TDayOfMonth> dayArray;
	CleanupClosePushL(dayArray);
	dayArray.AppendL(dayOfMonth);
	rpt.SetByDay(dayArray);
	CleanupStack::PopAndDestroy(&dayArray);
	
	RArray<TMonth> monthArray;
	CleanupClosePushL(monthArray);
	monthArray.AppendL(ENovember);
	rpt.SetByMonth(monthArray);
	CleanupStack::PopAndDestroy(&monthArray);
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0);		// 1 Jan 05
	aData.iEndDate = TDateTime(2010, EJanuary, 0, 0, 0, 0, 0); 			// 1 Jan 10

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, ENovember, 9, 0, 0, 0, 0)));	// 10 Nov 05
	aData.iTimes.AppendL(TTime(TDateTime(2007, ENovember, 7, 0, 0, 0, 0)));	//  8 Nov 07
	aData.iTimes.AppendL(TTime(TDateTime(2009, ENovember, 11, 0, 0, 0, 0)));	// 12 Nov 09
	}


void CTestData::CreateYearlyTest1980_6L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Three Repeats of 3 year each of Appt,Event,Todo from 1901 to 1917"));
	
	TCalRRule rpt(TCalRRule::EYearly);
	rpt.SetInterval(1);
	rpt.SetCount(3);
	TCalTime startTime;
	startTime.SetTimeLocalL((TDateTime (1901,EJanuary,24,21,30,0,0))); 	
	rpt.SetDtStart(startTime); 
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);

	startTime.SetTimeLocalL((TDateTime (1910,EAugust,15,21,30,0,0))); 	
	rpt.SetDtStart(startTime); 
	AddRuleL(rpt, aEntryView, KGUID2(), CCalEntry::EEvent);

	startTime.SetTimeLocalL((TDateTime (1915,EMarch,5,21,30,0,0))); 	
	rpt.SetDtStart(startTime); 
	AddRuleL(rpt, aEntryView, KGUID3(), CCalEntry::ETodo);


	aData.iStartDate = TDateTime(1900,EJanuary,24,21,30,0,0);	
	aData.iEndDate = TDateTime(1919,EJanuary,0,0,0,0,0); 	

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(1901,EJanuary,24,21,30,0,0)));	
	aData.iTimes.AppendL(TTime(TDateTime(1902,EJanuary,24,21,30,0,0)));	
	aData.iTimes.AppendL(TTime(TDateTime(1903,EJanuary,24,21,30,0,0)));

	aData.iTimes.AppendL(TTime(TDateTime(1910,EAugust,15,21,30,0,0)));
	aData.iTimes.AppendL(TTime(TDateTime(1911,EAugust,15,21,30,0,0)));
	aData.iTimes.AppendL(TTime(TDateTime(1912,EAugust,15,21,30,0,0)));

    aData.iTimes.AppendL(TTime(TDateTime(1915,EMarch,5,22,30,0,0)));
   	aData.iTimes.AppendL(TTime(TDateTime(1916,EMarch,5,22,30,0,0)));
   	aData.iTimes.AppendL(TTime(TDateTime(1917,EMarch,5,22,30,0,0)));
	}

 	
void CTestData::CreateMultipleRuleTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05 and every Wed/Thur from 3 Apr 05 until 17 Apr 05. Get instances between 1 Apr and 30 Apr 05."));

	// create two entries with repeat rules at overlapping dates
	// Every 3 days from 24 Feb 05
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetDtStart(startTime); 	// 24 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 14, 0, 0, 0));
	rpt.SetUntil(endTime);			// 2008
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	// Every Wed/Thur from 6 Apr 05 until 17 Apr 05
	TCalRRule rpt2(TCalRRule::EWeekly);
	rpt2.SetInterval(1);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EWednesday);
	days.AppendL(EThursday);
	rpt2.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 2, 0, 0, 0, 0)); 		// 3 Apr
	rpt2.SetDtStart(startTime); 
	endTime.SetTimeLocalL(TDateTime(2005, EApril, 16, 0, 0, 0, 0));			// 17 Apr
	rpt2.SetUntil(endTime);	
	AddRuleL(rpt2, aEntryView, KGUID2(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EApril, 0, 0, 0, 0, 0);			//  1 Apr 05
	aData.iEndDate = TDateTime(2005, EApril, 29, 0, 0, 0, 0); 			// 30 Apr 05
	
	aData.iTimes.Reset();
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 0, 0, 0, 0, 0)));		//  1 Apr 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 3, 0, 0, 0, 0)));		//  4 Apr 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 5, 0, 0, 0, 0)));		//  6 Apr 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 6, 0, 0, 0, 0)));		//  7 Apr 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 6, 0, 0, 0, 0)));		//  7 Apr 05 - 3-daily (must mention both instances)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 9, 0, 0, 0, 0)));		// 10 Apr 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0)));	// 13 Apr 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0)));	// 13 Apr 05 - 3-daily
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 13, 0, 0, 0, 0)));	// 14 Apr 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 15, 0, 0, 0, 0)));	// 16 Apr 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 18, 0, 0, 0, 0)));	// 19 Apr 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 Apr 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 24, 0, 0, 0, 0)));	// 25 Apr 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 27, 0, 0, 0, 0)));	// 28 Apr 05
	}

	
void CTestData::CreateMultipleRuleTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every month on the 1st and 31st and every Wed/Thur from 3 Apr 05. Get instances between 20 Apr and 10 Jun 05."));
	
	// create two entries with repeat rules at overlapping dates
	// Every Wed/Thur from 3 Apr 05
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EWednesday);
	days.AppendL(EThursday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 2, 0, 0, 0, 0)); 		// 3 Apr
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EApril, 26, 0, 0, 0, 0));			// 2008
	rpt.SetUntil(endTime);	
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	// Every month on the 1st and 31st from 1 Apr 05
	TCalRRule rpt2(TCalRRule::EMonthly);
	rpt2.SetInterval(1);
	RArray<TInt> days2;
	CleanupClosePushL(days2);
	days2.AppendL(0);
	days2.AppendL(30);
	rpt2.SetByMonthDay(days2);
	CleanupStack::PopAndDestroy(&days2);
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 0, 0, 0, 0, 0)); 		// 1 Apr
	rpt2.SetDtStart(startTime); 
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 0, 0, 0, 0));			// 2008
	rpt2.SetUntil(endTime);	
	AddRuleL(rpt2, aEntryView, KGUID2(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2005, EApril, 19, 0, 0, 0, 0);			// 20 Apr 05
	aData.iEndDate = TDateTime(2005, EJune, 9, 0, 0, 0, 0); 			// 10 Jun 05
	
	aData.iTimes.Reset();
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 19, 0, 0, 0, 0)));	// 20 Apr 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 20, 0, 0, 0, 0)));	// 21 Apr 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 26, 0, 0, 0, 0)));	// 27 Apr 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 27, 0, 0, 0, 0)));	// 28 Apr 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EApril, 29, 0, 0, 0, 0)));	// 30 Apr 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 0, 0, 0, 0, 0)));		//  1 May 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 3, 0, 0, 0, 0)));		//  4 May 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 4, 0, 0, 0, 0)));		//  5 May 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 10, 0, 0, 0, 0)));		// 11 May 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 11, 0, 0, 0, 0)));		// 12 May 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 17, 0, 0, 0, 0)));		// 18 May 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 18, 0, 0, 0, 0)));		// 19 May 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 24, 0, 0, 0, 0)));		// 25 May 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 25, 0, 0, 0, 0)));		// 26 May 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EMay, 30, 0, 0, 0, 0)));		// 31 May 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJune, 0, 0, 0, 0, 0)));		//  1 Jun 05
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJune, 0, 0, 0, 0, 0)));		//  1 Jun 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJune, 1, 0, 0, 0, 0)));		//  2 Jun 05 - Thur
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJune, 7, 0, 0, 0, 0)));		//  8 Jun 05 - Wed
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJune, 8, 0, 0, 0, 0)));		//  9 Jun 05 - Thur
	}

	
// create a weekly repeating rule for the specified day of the week
void CTestData::AddWeeklyRuleL(TDay aDay, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType)
	{
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	
	TCalTime startTime;
	TCalTime endTime;
	
	if (aDay == EMonday)
		{
		startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 2, 0, 0, 0, 0)); 		// 3 Jan 05
		endTime.SetTimeLocalL(TDateTime(2012, EMarch, 25, 0, 0, 0, 0));			// 26 Mar 2012
		}
	else if (aDay == ETuesday)
		{
		startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 3, 0, 0, 0, 0)); 		// 4 Jan 05
		endTime.SetTimeLocalL(TDateTime(2012, EMarch, 26, 0, 0, 0, 0));			// 27 Mar 2012		
		}
	else if (aDay == EWednesday)
		{
		startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 4, 0, 0, 0, 0)); 		// 5 Jan 05
		endTime.SetTimeLocalL(TDateTime(2012, EMarch, 27, 0, 0, 0, 0));			// 28 Mar 2012	
		}
	else if (aDay == EThursday)
		{
		startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 5, 0, 0, 0, 0)); 		// 6 Jan 05
		endTime.SetTimeLocalL(TDateTime(2012, EMarch, 28, 0, 0, 0, 0));			// 29 Mar 2012
		}
	else if (aDay == EFriday)
		{
		startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 6, 0, 0, 0, 0)); 		// 7 Jan 05
		endTime.SetTimeLocalL(TDateTime(2012, EMarch, 29, 0, 0, 0, 0));			// 30 Mar 2012
		}						

	rpt.SetDtStart(startTime); 
	rpt.SetUntil(endTime);	
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(aDay);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	
	AddRuleL(rpt, aEntryView, aGuid, aType);
	}


// Create 5 weekly repeating rules - Appts every Monday, Todos every Tuesday, etc.
// This is used for the filter tests.
void CTestData::CreateFilterTestDataL(CCalEntryView* aEntryView)
	{
	AddWeeklyRuleL(EMonday, aEntryView, KGUID1(), CCalEntry::EAppt);
	AddWeeklyRuleL(ETuesday, aEntryView, KGUID2(), CCalEntry::ETodo);
	AddWeeklyRuleL(EWednesday, aEntryView, KGUID3(), CCalEntry::EEvent);
	AddWeeklyRuleL(EThursday, aEntryView, KGUID4(), CCalEntry::EReminder);
	AddWeeklyRuleL(EFriday, aEntryView, KGUID5(),CCalEntry::EAnniv);
	}


void CTestData::CreateFilterTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter out annivs (Fri) and appts (Mon) only. Get all instances between 1 Feb 05 and 12 Feb 05."));

	CreateFilterTestDataL(aEntryView);
	aData.iFilter = CalCommon::EIncludeAppts|CalCommon::EIncludeAnnivs;
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 	//  1 Feb 05
	aData.iEndDate = TDateTime(2005, EFebruary, 11, 0, 0, 0, 0); 	// 12 Feb 05
	
	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 3, 0, 0, 0, 0)));	//  Anniv
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 6, 0, 0, 0, 0)));	// Appt
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 10, 0, 0, 0, 0)));	// Anniv
	}


void CTestData::CreateFilterTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter out events (Wed) only. Get all instances between 1 Feb 05 and 28 Feb 05."));

	CreateFilterTestDataL(aEntryView);
	aData.iFilter = CalCommon::EIncludeEvents;
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 	//  1 Feb 05
	aData.iEndDate = TDateTime(2005, EFebruary, 27, 0, 0, 0, 0); 	// 28 Feb 05
	
	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 1, 0, 0, 0, 0)));	//  2 Feb 05 - Wed (Event)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 8, 0, 0, 0, 0)));	//  9 Feb 05 - Wed (Event)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 15, 0, 0, 0, 0)));	// 16 Feb 05 - Wed (Event)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 22, 0, 0, 0, 0)));	// 23 Feb 05 - Wed (Event)
	}


void CTestData::CreateFilterTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter out Todos (Tue) and Reminders (Thu). Get all instances between 1 Feb 05 and 28 Feb 05."));

	CreateFilterTestDataL(aEntryView);
	aData.iFilter = CalCommon::EIncludeCompletedTodos|CalCommon::EIncludeIncompletedTodos|CalCommon::EIncludeReminder;
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 	//  1 Feb 05
	aData.iEndDate = TDateTime(2005, EFebruary, 27, 0, 0, 0, 0); 	// 28 Feb 05
	
	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 0, 1, 0, 0, 0)));    	//  1 Feb 05 - Tue (Todo)	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 2, 0, 0, 0, 0)));		//  3 Feb 05 - Thu (Reminder)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 7, 1, 0, 0, 0)));    	//  8 Feb 05 - Tue (Todo)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 9, 0, 0, 0, 0)));		// 10 Feb 05 - Thu (Reminder)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 14, 1, 0, 0, 0)));    // 15 Feb 05 - Tue (Todo)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 16, 0, 0, 0, 0)));	// 17 Feb 05 - Thu (Reminder)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 21, 1, 0, 0, 0)));    // 22 Feb 05 - Tue (Todo)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0)));	// 24 Feb 05 - Thu (Reminder)
	}


void CTestData::CreateFilterTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for weekly events: No events included."));
	
	CreateFilterTestDataL(aEntryView);
	aData.iFilter = 0; // empty filter!
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 	//  1 Feb 05
	aData.iEndDate = TDateTime(2005, EFebruary, 27, 0, 0, 0, 0); 	// 28 Feb 05
	
	// expected times:
	// NONE
	}


CCalEntry* CTestData::CreateTodoLC(const TDesC8& aGuid, const TTime& aTimeLocal, const TTime& aCompletedTimeLocal)
	{
	CCalEntry* entry = CreateEntryLC(CCalEntry::ETodo, aGuid, aTimeLocal, aTimeLocal);
	
	TCalTime calTime;
	if (aCompletedTimeLocal == Time::NullTTime())
		{
		entry->SetCompletedL(EFalse, calTime);
		}
	else
		{
		calTime.SetTimeLocalL(aCompletedTimeLocal);
		entry->SetCompletedL(ETrue, calTime);
		}
		
	return entry;
	}


void CTestData::CreateFilterTest5L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for incompleted todos only."));
	
	// create one completed todo and one incomplete todo
	TTime todoTime1(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0));
	CCalEntry* entry = CreateTodoLC(KGUID1(), todoTime1, todoTime1); // 26 Jan - completed entry
		
	TTime todoTime2(TDateTime(2005, EJanuary, 15, 0, 0, 0, 0)); 
	CCalEntry* entry2 = CreateTodoLC(KGUID1(), todoTime2, Time::NullTTime()); // 16 Jan - incomplete entry
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	array.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry);
	
	aData.iFilter = CalCommon::EIncludeIncompletedTodos;
	
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0); 	//  1 Jan 05
	aData.iEndDate = TDateTime(2005, EJanuary, 30, 0, 0, 0, 0); 	// 31 Jan 05

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJanuary, 15, 0, 0, 0, 0)));	// 16 Jan 05
	}


void CTestData::CreateFilterTest6L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for completed todos only."));
	
	// create one completed todo and one incomplete todo
	TTime todoTime1(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0));
	CCalEntry* entry = CreateTodoLC(KGUID1(), todoTime1, todoTime1); // 26 Jan - completed entry
		
	TTime todoTime2(TDateTime(2005, EJanuary, 15, 0, 0, 0, 0)); 
	CCalEntry* entry2 = CreateTodoLC(KGUID2(), todoTime2, Time::NullTTime()); // 16 Jan - incomplete entry
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	array.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry);
	
	aData.iFilter = CalCommon::EIncludeCompletedTodos;
	
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0); 	//  1 Jan 05
	aData.iEndDate = TDateTime(2005, EJanuary, 30, 0, 0, 0, 0); 	// 31 Jan 05
	
	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0)));	// 26 Jan 05
	}


void CTestData::CreateFilterTest7L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Weekly alarmed entry every Monday, non-alarmed entry every Tuesday. Get alarmed instances between 1 Feb 05 and 28 Feb 05."));

	TTime startTime(TDateTime(2005, EFebruary, 0, 0, 0, 0, 0)); 	//  1 Feb 05
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	
	// create alarmed entry
	CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KGUID1(), startTime, startTime + TTimeIntervalHours(1));
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(30);
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	
	// make this entry repeat every monday
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	rpt.SetDtStart(calStartTime);
	RArray<TDay> dayArray;
	CleanupClosePushL(dayArray);
	dayArray.AppendL(EMonday);
	rpt.SetByDay(dayArray);
	CleanupStack::PopAndDestroy(&dayArray);
	
	entry->SetRRuleL(rpt);

	// create non-alarmed entry
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::EAppt, KGUID2(), startTime, startTime + TTimeIntervalHours(1));
	
	// make this entry repeat every tuesday
	TCalRRule rpt2(TCalRRule::EWeekly);
	rpt2.SetInterval(1);
	rpt2.SetDtStart(calStartTime);
	dayArray.AppendL(ETuesday);
	rpt2.SetByDay(dayArray);
	dayArray.Reset();
	
	entry2->SetRRuleL(rpt2);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	array.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry);
	
	aData.iFilter = CalCommon::EIncludeAlarmedOnly|CalCommon::EIncludeAppts;
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 	//  1 Feb 05
	aData.iEndDate = TDateTime(2005, EFebruary, 27, 0, 0, 0, 0); 	// 28 Feb 05

	aData.iTimes.Reset();
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 6, 0, 0, 0, 0)));	//  7 Feb 05 - Mon (alarmed)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 13, 0, 0, 0, 0)));	// 14 Feb 05 - Mon (alarmed)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 20, 0, 0, 0, 0)));	// 21 Feb 05 - Mon (alarmed)
	aData.iTimes.AppendL(TTime(TDateTime(2005, EFebruary, 27, 0, 0, 0, 0)));	// 28 Feb 05 - Mon (alarmed)
	}


void CTestData::CreateDeleteTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView, CCalInstanceView* aInstanceView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05, with 2 Mar only deleted. Get instances from 24 Feb to 8 Mar."));

	// set up rule: Every 3 days from 24 Feb 05
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetDtStart(startTime); 	// 24 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 14, 0, 0, 0));
	rpt.SetUntil(endTime);			// 2008

	AddRuleL(rpt, aEntryView, KGUID1());
	
	// get instance on 2 Mar
	RPointerArray<CCalInstance> instanceList;
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime instDate;
	instDate.SetTimeLocalL(TDateTime(2005, EMarch, 1, 0, 0, 0, 0)); // 2 Mar
	CalCommon::TCalTimeRange timeRange(instDate, instDate);
	aInstanceView->FindInstanceL(instanceList, filter, timeRange);
	ASSERT(instanceList.Count() == 1); // check instance has been found

	// delete 2 Mar instance only
	CCalInstance* instToDelete = instanceList[0];
	aInstanceView->DeleteL(instToDelete, CalCommon::EThisOnly);
	instanceList.Remove(0); // remove first item
	instanceList.ResetAndDestroy();
	
	aData.iStartDate = TDateTime(2005, EFebruary, 23, 0, 0, 0, 0); 	// 24 Feb
	aData.iEndDate = TDateTime(2005, EMarch, 7, 0, 0, 0, 0); 		//  8 Mar
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));	// 24 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 26, 0, 0, 0, 0));	// 27 Feb
	aData.iTimes.AppendL(TDateTime(2005, EMarch, 4, 0, 0, 0, 0));		//  5 Mar
	aData.iTimes.AppendL(TDateTime(2005, EMarch, 7, 0, 0, 0, 0));		//  8 Mar
	}


void CTestData::CreateDeleteTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView, CCalInstanceView* aInstanceView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05, then delete all up to 2 Mar. Get instances from 24 Feb to 8 Mar."));

	// set up rule: Every 3 days from 24 Feb 05
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetDtStart(startTime); 	// 24 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 14, 0, 0, 0));
	rpt.SetUntil(endTime);			// 2008
	
	AddRuleL(rpt, aEntryView, KGUID1());
	
	// get instance on 2 Mar
	RPointerArray<CCalInstance> instanceList;
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime instDate;
	instDate.SetTimeLocalL(TDateTime(2005, EMarch, 1, 0, 0, 0, 0)); // 2 Mar
	CalCommon::TCalTimeRange timeRange(instDate, instDate);
	aInstanceView->FindInstanceL(instanceList, filter, timeRange);
	ASSERT(instanceList.Count() == 1); // check instance has been found

	// delete 2 Mar instance and all instances before
	CCalInstance* instToDelete = instanceList[0];
	aInstanceView->DeleteL(instToDelete, CalCommon::EThisAndPrior);
	instanceList.Remove(0); // remove first item
	instanceList.ResetAndDestroy();
	
	aData.iStartDate = TDateTime(2005, EFebruary, 23, 0, 0, 0, 0); 	// 24 Feb
	aData.iEndDate = TDateTime(2005, EMarch, 7, 0, 0, 0, 0); 		//  8 Mar
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EMarch, 4, 0, 0, 0, 0));		//  5 Mar
	aData.iTimes.AppendL(TDateTime(2005, EMarch, 7, 0, 0, 0, 0));		//  8 Mar
	}


void CTestData::CreateDeleteTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView, CCalInstanceView* aInstanceView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05, then delete all after 2 Mar. Get instances from 24 Feb to 8 Mar."));

	// set up rule: Every 3 days from 24 Feb 05
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetDtStart(startTime); 	// 24 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 14, 0, 0, 0));
	rpt.SetUntil(endTime);			// 2008
	
	AddRuleL(rpt, aEntryView, KGUID1());
	
	// get instance on 2 Mar
	RPointerArray<CCalInstance> instanceList;
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime instDate;
	instDate.SetTimeLocalL(TDateTime(2005, EMarch, 1, 0, 0, 0, 0)); // 2 Mar
	CalCommon::TCalTimeRange timeRange(instDate, instDate);
	aInstanceView->FindInstanceL(instanceList, filter, timeRange);
	ASSERT(instanceList.Count() == 1); // check instance has been found

	// delete 2 Mar instance and all instances after
	CCalInstance* instToDelete = instanceList[0];
	aInstanceView->DeleteL(instToDelete, CalCommon::EThisAndFuture);
	instanceList.Remove(0); // remove first item
	instanceList.ResetAndDestroy();
	
	aData.iStartDate = TDateTime(2005, EFebruary, 23, 0, 0, 0, 0); 	// 24 Feb
	aData.iEndDate = TDateTime(2005, EMarch, 7, 0, 0, 0, 0); 		//  8 Mar
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));	// 24 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 26, 0, 0, 0, 0));	// 27 Feb
	}


void CTestData::CreateDeleteTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView, CCalInstanceView* aInstanceView)
	{
	aData.iDescription.Copy(_L("Every 3 days from 24 Feb 05, then delete all instances. Get instances from 1 Jan 05 to 1 Jan 06."));

	// set up rule: Every 3 days from 24 Feb 05
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetInterval(3);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));
	rpt.SetDtStart(startTime); 	// 24 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EMarch, 30, 14, 0, 0, 0));
	rpt.SetUntil(endTime);			// 2008
	
	AddRuleL(rpt, aEntryView, KGUID1());
	
	// get instance on 2 Mar
	RPointerArray<CCalInstance> instanceList;
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime instDate;
	instDate.SetTimeLocalL(TDateTime(2005, EMarch, 1, 0, 0, 0, 0)); // 2 Mar
	CalCommon::TCalTimeRange timeRange(instDate, instDate);
	aInstanceView->FindInstanceL(instanceList, filter, timeRange);
	ASSERT(instanceList.Count() == 1); // check instance has been found

	// delete all instances
	CCalInstance* instToDelete = instanceList[0];
	aInstanceView->DeleteL(instToDelete, CalCommon::EThisAndAll);
	instanceList.Remove(0); // remove first item
	instanceList.ResetAndDestroy();
	
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0); 	// 1 Jan 05
	aData.iEndDate = TDateTime(2006, EJanuary, 0, 0, 0, 0, 0); 		// 1 Jan 06
	
	// expected times:
	// NONE
	}


// creates a weekly entry repeating on a certain weekday with the given summary string and UID
void CTestData::CreateWeeklyEntryWithThisStringL(TDay aDay, const TDesC& aString, CCalEntryView* aEntryView, const TDesC8& aGuid)
	{
	TCalRRule rpt1(TCalRRule::EWeekly);
	rpt1.SetInterval(1);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(aDay);
	rpt1.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 0, 0, 0, 0, 0)); 		// 1 Jan 05
	rpt1.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2008, EJanuary, 0, 0, 0, 0, 0));			// 1 Jan 08
	rpt1.SetUntil(endTime);	
	
	TTime entryStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0);	// on 1 Jan 05
	CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, aGuid, entryStartDate, entryStartDate + TTimeIntervalHours(1));
	
	entry->SetRRuleL(rpt1); // doesn't take ownership
	entry->SetSummaryL(aString);
	
	RPointerArray<CCalEntry> entryList;
	CleanupClosePushL(entryList);
	entryList.AppendL(entry);
	TInt success(0);
	aEntryView->StoreL(entryList, success); // takes ownership
	CleanupStack::PopAndDestroy(&entryList);
	
	CleanupStack::PopAndDestroy(entry);
	}

	
void CTestData::CreateStringTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every Wed, summary 'This is search text'. Every Thu, 'Do not find this téxt'. Get instances in Feb 05 matching exactly 'text'."));

	_LIT(KSummary1, "This is search text");
	_LIT(KSummary2, "Do not find this téxt");
	_LIT(KSearchString1, "text");
	// weekly entry every Wed from 1 Jan 05, summary = "This is search text"
	CreateWeeklyEntryWithThisStringL(EWednesday, KSummary1(), aEntryView, KGUID1());
	CreateWeeklyEntryWithThisStringL(EThursday, KSummary2(), aEntryView, KGUID2());
	
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 	//  1 Feb 05
	aData.iEndDate = TDateTime(2005, EFebruary, 27, 0, 0, 0, 0); 	// 28 Feb 05
	aData.iSearchString = KSearchString1().AllocL();
	aData.iSearchBehaviour = CalCommon::EExactTextSearch;

	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 1, 0, 0, 0, 0));	//  2 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 8, 0, 0, 0, 0));	//  9 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 15, 0, 0, 0, 0));// 16 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 22, 0, 0, 0, 0));// 23 Feb
	}

	
void CTestData::CreateStringTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Every Wed, summary 'This is search text'. Every Thu, 'Also find this téxt'. Get instances in Feb 05 matching 'text'."));

	_LIT(KSummary1, "This is search text");
	_LIT(KSummary2, "Also find this téxt");
	_LIT(KSearchString1, "text");
	// weekly entry every Wed from 1 Jan 05, summary = "This is search text"
	CreateWeeklyEntryWithThisStringL(EWednesday, KSummary1(), aEntryView, KGUID1());
	CreateWeeklyEntryWithThisStringL(EThursday, KSummary2(), aEntryView, KGUID2());
	
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 	//  1 Feb 05
	aData.iEndDate = TDateTime(2005, EFebruary, 27, 0, 0, 0, 0); 	// 28 Feb 05
	aData.iSearchString = KSearchString1().AllocL();
	aData.iSearchBehaviour = CalCommon::EFoldedTextSearch;

	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 1, 0, 0, 0, 0));	//  2 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 2, 0, 0, 0, 0));	//  3 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 8, 0, 0, 0, 0));	//  9 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 9, 0, 0, 0, 0));	// 10 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 15, 0, 0, 0, 0));// 16 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 16, 0, 0, 0, 0));// 17 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 22, 0, 0, 0, 0));// 23 Feb
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 23, 0, 0, 0, 0));// 24 Feb
	}

	
void CTestData::CreateStringTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test entries crossing start/end of search range. Search for 'text', find entry with summary 'This is search text' but not 'Do not find this téxt'."));

	_LIT(KSummary1, "This is search text");
	_LIT(KSummary2, "Do not find this téxt");
	_LIT(KSearchString1, "text");
	
	// weekly entry every Wed from 1 Jan 05, summary = "This is search text"	
	TTime entryStartDate1 = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0);	// 00:00 on  1 Jan 05 until 00:00 on  4 Jan 05
	CCalEntry* entry1 = CreateEntryLC(CCalEntry::EAppt, KGUID1(), entryStartDate1, entryStartDate1 + TTimeIntervalDays(3));
	entry1->SetSummaryL(KSummary1());

	TTime entryStartDate2 = TDateTime(2005, EJanuary, 10, 0, 0, 0, 0);	// 00:00 on 11 Jan 05 until 00:00 on 14 Jan 05
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::EAppt, KGUID2(), entryStartDate2, entryStartDate2 + TTimeIntervalDays(3));
	entry2->SetSummaryL(KSummary2());
	
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	entries.AppendL(entry1);
	entries.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(entries, success);
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry1);
	
	aData.iStartDate = TDateTime(2005, EJanuary, 1, 0, 0, 0, 0); 	// 00:00 on  2 Jan 05
	aData.iEndDate = TDateTime(2005, EJanuary, 11, 0, 0, 0, 0); 	// 00:00 on 12 Jan 05
	aData.iSearchString = KSearchString1().AllocL();
	aData.iSearchBehaviour = CalCommon::EExactTextSearch;

	// expected times:
	aData.iTimes.AppendL(entryStartDate1);	// 1st entry,  1 Jan 05
	}

	
void CTestData::CreateStringTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test entries crossing start/end of search range. Search for 'text' with filter, find entry with summary 'This is search text' and 'Also find this téxt'."));

	_LIT(KSummary1, "This is search text");
	_LIT(KSummary2, "Also find this téxt");
	_LIT(KSearchString1, "text");
	
	// weekly entry every Wed from 1 Jan 05, summary = "This is search text"	
	TTime entryStartDate1 = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0);	// 00:00 on  1 Jan 05 until 00:00 on  4 Jan 05
	CCalEntry* entry1 = CreateEntryLC(CCalEntry::EAppt, KGUID1(), entryStartDate1, entryStartDate1 + TTimeIntervalDays(3));
	entry1->SetSummaryL(KSummary1());

	TTime entryStartDate2 = TDateTime(2005, EJanuary, 10, 0, 0, 0, 0);	// 00:00 on 11 Jan 05 until 00:00 on 14 Jan 05
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::EAppt, KGUID2(), entryStartDate2, entryStartDate2 + TTimeIntervalDays(3));
	entry2->SetSummaryL(KSummary2());
	
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	entries.AppendL(entry1);
	entries.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(entries, success);
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry1);
	
	aData.iStartDate = TDateTime(2005, EJanuary, 1, 0, 0, 0, 0); 	// 00:00 on  2 Jan 05
	aData.iEndDate = TDateTime(2005, EJanuary, 11, 0, 0, 0, 0); 	// 00:00 on 12 Jan 05
	aData.iSearchString = KSearchString1().AllocL();
	aData.iSearchBehaviour = CalCommon::EFoldedTextSearch;

	// expected times:
	aData.iTimes.AppendL(entryStartDate1);	// 1st entry,  1 Jan 05
	aData.iTimes.AppendL(entryStartDate2);	// 2nd entry, 11 Jan 05
	}


void CTestData::SetupStartEndTimesTestL(CCalEntryView* aEntryView)
	{
	// create single todo entry which crosses start of search time range
	TTime startTime1(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0));
	TTime endTime1(TDateTime(2005, EFebruary, 4, 0, 0, 0, 0));
	CCalEntry* entry1 = CreateEntryLC(CCalEntry::ETodo, KGUID1(), startTime1, endTime1);
	//
	// create single appt entry which crosses start of search time range
	TTime startTime2(TDateTime(2005, EJanuary, 26, 1, 0, 0, 0));
	TTime endTime2(TDateTime(2005, EFebruary, 5, 1, 0, 0, 0));
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::EAppt, KGUID2(), startTime2, endTime2);
	//
	// create single todo entry which crosses end of search time range
	TTime startTime3(TDateTime(2005, EFebruary, 7, 2, 0, 0, 0));
	TTime endTime3(TDateTime(2005, EFebruary, 17, 2, 0, 0, 0));
	CCalEntry* entry3 = CreateEntryLC(CCalEntry::ETodo, KGUID3(), startTime3, endTime3);
	//
	// create single appt entry which crosses end of search time range
	TTime startTime4(TDateTime(2005, EFebruary, 6, 3, 0, 0, 0));
	TTime endTime4(TDateTime(2005, EFebruary, 16, 3, 0, 0, 0));
	CCalEntry* entry4 = CreateEntryLC(CCalEntry::EAppt, KGUID4(), startTime4, endTime4);
	//
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry1);
	array.AppendL(entry2);
	array.AppendL(entry3);
	array.AppendL(entry4);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry4);
	CleanupStack::PopAndDestroy(entry3);
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry1);
	}


void CTestData::CreateStartEndTimesTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for appt/todo with only start/end date in time range."));
	
	// search time range is 1 - 11 February
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 
	aData.iEndDate = TDateTime(2005, EFebruary, 10, 0, 0, 0, 0); 
	
	SetupStartEndTimesTestL(aEntryView);
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EJanuary, 26, 1, 0, 0, 0));
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 4, 0, 0, 0, 0));
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 6, 3, 0, 0, 0));
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 17, 2, 0, 0, 0));
	}


void CTestData::CreateStartEndTimesTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for appt with only end date in time range."));
	
	// create single appt entry which crosses start of search time range
	TTime startTime2(TDateTime(2005, EJanuary, 26, 1, 0, 0, 0));
	TTime endTime2(TDateTime(2005, EFebruary, 5, 1, 0, 0, 0));
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::EAppt, KGUID2(), startTime2, endTime2);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry2);
		
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 
	aData.iEndDate = TDateTime(2005, EFebruary, 10, 0, 0, 0, 0); 
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EJanuary, 26, 1, 0, 0, 0));
	}


void CTestData::CreateStartEndTimesTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for appt/todo with only start date in time range."));
	
	// search time range is 1 - 11 February
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 
	aData.iEndDate = TDateTime(2005, EFebruary, 10, 0, 0, 0, 0); 
	
	aData.iFilter = CalCommon::EIncludeAll | CalCommon::EInstanceStartTimeWithinRange;
	SetupStartEndTimesTestL(aEntryView);
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 6, 3, 0, 0, 0));
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 17, 2, 0, 0, 0));
	}


void CTestData::CreateStartEndTimesTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for appt/todo with only end date in time range."));
	
	// search time range is 1 - 11 February
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 
	aData.iEndDate = TDateTime(2005, EFebruary, 10, 0, 0, 0, 0); 
	
	aData.iFilter = CalCommon::EIncludeAll | CalCommon::EInstanceEndTimeWithinRange;
	SetupStartEndTimesTestL(aEntryView);
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2005, EJanuary, 26, 1, 0, 0, 0));
	aData.iTimes.AppendL(TDateTime(2005, EFebruary, 4, 0, 0, 0, 0));
	}


void CTestData::CreateStartEndTimesTest5L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for appt/todo with both start and end date in time range."));
	
	// search time range is 1 - 11 February
	aData.iStartDate = TDateTime(2005, EFebruary, 0, 0, 0, 0, 0); 
	aData.iEndDate = TDateTime(2005, EFebruary, 10, 0, 0, 0, 0); 
	
	aData.iFilter = CalCommon::EIncludeAll | CalCommon::EInstanceStartTimeWithinRange | CalCommon::EInstanceEndTimeWithinRange;
	SetupStartEndTimesTestL(aEntryView);
	
	// expected times:
	// NONE
	}


void CTestData::CreateStartEndTimesTest6L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for appt span a long time range."));

	// create single appt entry which crosses start of search time range
	TTime startTime3(TDateTime(1980, EOctober, 21, 1, 0, 0, 0));
	TTime endTime3(TDateTime(1980, EOctober, 23, 1, 0, 0, 0));
	CCalEntry* entry3 = CreateEntryLC(CCalEntry::EAppt, KGUID2(), startTime3, endTime3);
	
	entry3->SetSummaryL(_L("1980 to 2100 appointment"));
	entry3->SetDescriptionL(_L("1980 to 2100 appointment"));

	TCalTime ct3;
	ct3.SetTimeLocalL(startTime3);
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(ct3);
	rule.SetCount(1000);
	rule.SetInterval(7);
	
	entry3->SetRRuleL(rule);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry3);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry3);
		
	aData.iStartDate = TDateTime(1980, EOctober, 22, 1, 0, 0, 0); 
	aData.iEndDate   = TDateTime(2099, EDecember,30, 0, 0, 0, 0); 
	
	aData.iSearchString = _L("1980 to 2100 appointment").AllocL();

    TInt count = 0;
    
	// expected times - too many!
	while (startTime3 < aData.iEndDate && count++ < 1000)
		{
		aData.iTimes.AppendL(startTime3);
		startTime3 += TTimeIntervalDays(7);
		}
		
	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}

void CTestData::CreateStartEndTimesTest7L(CTestDataStruct& aData, CCalEntryView* aView)
	{
	aData.iDescription.Copy(_L("Check appointment overlapping with second instance's end date."));
	
	_LIT8(KGuid1, "Reminder-1");
	_LIT8(KGuid2, "Appt-1");
		
	_LIT(KEntryStartTime, 		"20121119:013000.000000"); // 20-12-2012 01.30
	_LIT(KEntryEndTime, 		"20121119:090000.000000"); // 20-12-2012 09.00
	
	_LIT(KTimeUntil, 			"20131119:013000.000000"); // 20-12-2013 01.30
	
	_LIT(KSummary1, "Reminder summary");
	_LIT(KSummary2, "Appt summary");
	
	//entry 1	
	HBufC8* guid8 = HBufC8::NewL(KGuid1().Length());
	guid8->Des().Copy(KGuid1);	
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EReminder, guid8, CCalEntry::EMethodAdd, (TUint)0);	
	entry->SetSummaryL(KSummary1);
	
	// entry2
	guid8 = HBufC8::NewL(KGuid2().Length());
	guid8->Des().Copy(KGuid2);	
	CCalEntry* entry2 = CCalEntry::NewL(CCalEntry::EAppt, guid8, CCalEntry::EMethodAdd, (TUint)0);	
	entry2->SetSummaryL(KSummary2);
	
	TTime time1(KEntryStartTime());
	TTime time2(KEntryEndTime());
	TCalTime st;
	st.SetTimeUtcL(time1);
	TCalTime end;
	end.SetTimeUtcL(time2);
		
	entry->SetStartAndEndTimeL(st, end); 
	entry2->SetStartAndEndTimeL(st, end);
	
	TCalRRule rrule(TCalRRule::EYearly);
	TCalTime calTime = entry->StartTimeL();
	rrule.SetDtStart(calTime);
	rrule.SetInterval(1);
	TTime until(KTimeUntil());
	TCalTime untilTime;
	untilTime.SetTimeUtcL(until);
	rrule.SetUntil(untilTime); 
	
	entry->SetRRuleL(rrule);
	entry2->SetRRuleL(rrule);
	
	TInt						entriesStored(0);
	RPointerArray<CCalEntry>	entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
	
	entriesToStore.AppendL(entry);
	entriesToStore.AppendL(entry2);
	aView->StoreL(entriesToStore, entriesStored);

	CleanupStack::PopAndDestroy(&entriesToStore); // entriesToStore.ResetAndDestroy()
	
	// Find
	_LIT(KFindSt, 		"20131119:013100.000000"); // The event "overlaps" with this search range, hence must be caught. The reminder must not be caught (its end time = start time).
	_LIT(KFindEnd, 		"20131119:093000.000000");
		
	aData.iStartDate = TTime(KFindSt); 
	aData.iEndDate   = TTime(KFindEnd); 
	
	_LIT(KSearchText, "summary");
	aData.iSearchString = KSearchText().AllocL();
	
	aData.iFilter = CalCommon::EIncludeAll;
	aData.iSearchBehaviour = CalCommon::EFoldedTextSearch;
	
	aData.iTimes.AppendL(TDateTime(2013, EDecember, 19, 1, 30, 0, 0)); 
	}
	
void CTestData::EmptyDatabase1L(CTestDataStruct& aData, CCalEntryView* /*aView*/)
	{
	aData.iDescription.Copy(_L("Search an empty database with filter range of MinTime and MaxTime."));
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iFilter = CalCommon::EIncludeAll;
	}


void CTestData::EmptyDatabase2L(CTestDataStruct& aData, CCalEntryView* )
	{
	aData.iDescription.Copy(_L("Search an empty database with filter range of a Month")); 
	
	TTime startTime(TDateTime(2005, EMarch, 0, 0, 0, 0, 0)); 	//  1 Mar 05

	aData.iStartDate = startTime;
	aData.iEndDate = startTime + TTimeIntervalMonths(1); 
	
	aData.iFilter = CalCommon::EIncludeAll;
	}


void CTestData::CreateManyInstancesTestL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test many instances"));
	
	TTime t1(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0));
	TTime t2(TDateTime(2005, EJanuary, 0, 12, 0, 0, 0));
	TTime t3(TDateTime(2005, EJanuary, 0, 15, 0, 0, 0));
	
	TCalTime ct1;
	ct1.SetTimeLocalL(t1);
	TCalTime ct2;
	ct2.SetTimeLocalL(t2);
	TCalTime ct3;
	ct3.SetTimeLocalL(t3);
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(ct1);
	rule.SetCount(273);
	rule.SetInterval(1);
	AddRuleL(rule, aEntryView, _L8("09:00"), CCalEntry::EAppt);
	rule.SetDtStart(ct2);
	AddRuleL(rule, aEntryView, _L8("12:00"), CCalEntry::EAppt);
	rule.SetDtStart(ct3);
	AddRuleL(rule, aEntryView, _L8("15:00"), CCalEntry::EAppt);
		
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0); 
	aData.iEndDate = TDateTime(2005, EOctober, 0, 0, 0, 0, 0); 
	
	// expected times - too many!
	while (t1 < aData.iEndDate)
		{
		aData.iTimes.AppendL(t1); // add an hour because todo instance occurs on end date (due date)
		aData.iTimes.AppendL(t2);
		aData.iTimes.AppendL(t3);
		t1 += TTimeIntervalDays(1);
		t2 += TTimeIntervalDays(1);
		t3 += TTimeIntervalDays(1);
		}
		
	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::CreateManyInstancesTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test finding many instances with text search"));
	
	TTime t1(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0));
	TTime t2(TDateTime(2005, EJanuary, 0, 12, 0, 0, 0));
	TTime t3(TDateTime(2005, EJanuary, 0, 15, 0, 0, 0));
	
	TCalTime ct1;
	ct1.SetTimeLocalL(t1);
	TCalTime ct2;
	ct2.SetTimeLocalL(t2);
	TCalTime ct3;
	ct3.SetTimeLocalL(t3);
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(ct1);
	rule.SetCount(300);
	rule.SetInterval(1);
	AddRuleL(rule, aEntryView, _L8("09:00"), CCalEntry::EAppt);
	rule.SetDtStart(ct2);
	AddRuleL(rule, aEntryView, _L8("12:00"), CCalEntry::EAppt);
	rule.SetDtStart(ct3);
	
	// add entry with text
	CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, _L8("15:00"), ct3.TimeLocalL(), ct3.TimeLocalL() + TTimeIntervalHours(1));
	entry->SetRRuleL(rule); // doesn't take ownership
	entry->SetSummaryL(_L("text to find"));
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	CleanupStack::PopAndDestroy(entry);
		
	aData.iStartDate = TDateTime(2005, EJanuary, 0, 0, 0, 0, 0); 
	aData.iEndDate = TDateTime(2005, EOctober, 0, 0, 0, 0, 0); 
	
	// expected times - too many!
	while (t3 < aData.iEndDate)
		{
		aData.iTimes.AppendL(t3);
		t3 += TTimeIntervalDays(1);
		}
		
	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iSearchString = _L("text to find").AllocL();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


TInt CTestData::CompleteManyInstanceTest(TAny* aPtr)
	{
	if (aPtr != NULL)
		{
		TTime* startTime = static_cast<TTime*>(aPtr);
		TTime endTime;
		endTime.UniversalTime();
		
		TTimeIntervalMicroSeconds micros = endTime.MicroSecondsFrom(*startTime);
		
		TUint32 ms = micros.Int64() / 1000;
		RDebug::Print(_L("Time to find many instances: %d \n"), ms);
		
		delete startTime;
		}	
	return KErrNone;
	}


void CTestData::CreateManyInstancesTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test many dated todos"));
	
	TTime t1(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0));
	TTime t2(TDateTime(2005, EJanuary, 0, 12, 0, 0, 0));
	TTime t3(TDateTime(2005, EJanuary, 0, 15, 0, 0, 0));
	
	TCalTime ct1;
	ct1.SetTimeLocalL(t1);
	TCalTime ct2;
	ct2.SetTimeLocalL(t2);
	TCalTime ct3;
	ct3.SetTimeLocalL(t3);
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(ct1);
	rule.SetCount(250);
	rule.SetInterval(1);
	AddRuleL(rule, aEntryView, _L8("09:00"), CCalEntry::ETodo);
	rule.SetDtStart(ct2);
	AddRuleL(rule, aEntryView, _L8("12:00"), CCalEntry::ETodo);
	rule.SetDtStart(ct3);
	AddRuleL(rule, aEntryView, _L8("15:00"), CCalEntry::ETodo);
		
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	// expected times - too many!
	TInt count = 0;
	while (count < 250)
		{
        aData.iTimes.AppendL(t1 + TTimeIntervalHours(1)); // add an hour because todo instance occurs on end date (due date)
        aData.iTimes.AppendL(t2 + TTimeIntervalHours(1));
        aData.iTimes.AppendL(t3 + TTimeIntervalHours(1));
		t1 += TTimeIntervalDays(1);
		t2 += TTimeIntervalDays(1);
		t3 += TTimeIntervalDays(1);
		count++;
		}
		
	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::CreateMaxRangeTestL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test non-repeating instances found correctly when searching maximum range"));
	
	TTime t1(TDateTime(2005, EJanuary, 10, 9, 0, 0, 0)); // 11 Jan, 09:00
	TTime t2(TDateTime(2005, EJanuary, 10, 10, 0, 0, 0)); // 11 Jan, 10:00
	CCalEntry* entry1 = CreateEntryLC(CCalEntry::EAppt, _L8("GUID1"), t1, t2);
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::EEvent, _L8("GUID2"), t1, t2);
	CCalEntry* entry3 = CreateEntryLC(CCalEntry::ETodo, _L8("GUID3"), t1, t2);
	CCalEntry* entry4 = CreateEntryLC(CCalEntry::EReminder, _L8("GUID4"), t1, t2);
	CCalEntry* entry5 = CreateEntryLC(CCalEntry::EAnniv, _L8("GUID5"), t1, t2);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry1);
	array.AppendL(entry2);
	array.AppendL(entry3);
	array.AppendL(entry4);
	array.AppendL(entry5);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(5, entry1);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime();
	
	aData.iTimes.AppendL(t1);
	aData.iTimes.AppendL(t1);
	aData.iTimes.AppendL(t1);
	aData.iTimes.AppendL(t1);
	aData.iTimes.AppendL(t2);
	
	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::CreateMaxRangeTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test repeating instances found correctly when searching maximum range"));
	
	TTime t1(TDateTime(2005, EJanuary, 10, 9, 0, 0, 0)); // 11 Jan, 09:00
	TTime t2(TDateTime(2005, EJanuary, 10, 10, 0, 0, 0)); // 11 Jan, 10:00
	
	TCalTime dtStartCal;
	dtStartCal.SetTimeLocalL(t1);
	TCalRRule rule(TCalRRule::EWeekly);
	rule.SetDtStart(dtStartCal);
	rule.SetCount(2);
	
	RArray<TDay> days;
	days.AppendL(ETuesday);
	rule.SetByDay(days);
	days.Reset();
	AddRuleL(rule, aEntryView, _L8("GUID1"), CCalEntry::EAppt);
	AddRuleL(rule, aEntryView, _L8("GUID2"), CCalEntry::ETodo);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iTimes.AppendL(t1);
	aData.iTimes.AppendL(t1 + TTimeIntervalDays(7));
	aData.iTimes.AppendL(t2);
	aData.iTimes.AppendL(t2 + TTimeIntervalDays(7));
	
	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::CreateMaxRangeTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test text-search instances found correctly when searching maximum range"));
		
	TTime t1(TDateTime(2005, EJanuary, 10, 9, 0, 0, 0)); // 11 Jan, 09:00
	TTime t2(TDateTime(2005, EFebruary, 10, 9, 0, 0, 0)); // 11 Feb, 09:00
	TTime t3(TDateTime(2005, EMarch, 10, 9, 0, 0, 0)); // 11 Mar, 09:00
	TTime t4(TDateTime(2005, EApril, 10, 9, 0, 0, 0)); // 11 Apr, 09:00
	TTime t5(TDateTime(2005, EMay, 10, 9, 0, 0, 0)); // 11 May, 09:00
	TTime t6(TDateTime(2005, EJune, 10, 9, 0, 0, 0)); // 11 Jun, 09:00
	TTime t7(TDateTime(2005, EJuly, 10, 9, 0, 0, 0)); // 11 Jul, 09:00
	TTime t8(TDateTime(2005, EAugust, 10, 9, 0, 0, 0)); // 11 Aug, 09:00
	TTime t9(TDateTime(2005, ESeptember, 10, 9, 0, 0, 0)); // 11 Sep, 09:00
	TTime t0(TDateTime(2005, EOctober, 10, 9, 0, 0, 0)); // 11 Oct, 09:00
	
	// add entry with text
	StoreEntryWithTextL(aEntryView, CCalEntry::EAppt, _L8("GUID1"), t1, _L("text to find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::EAppt, _L8("GUID2"), t2, _L("text to not find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::ETodo, _L8("GUID3"), t3, _L("text to find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::ETodo, _L8("GUID4"), t4, _L("text to not find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::EEvent, _L8("GUID5"), t5, _L("text to find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::EEvent, _L8("GUID6"), t6, _L("text to not find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::EAnniv, _L8("GUID7"), t7, _L("text to find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::EAnniv, _L8("GUID8"), t8, _L("text to not find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::EReminder, _L8("GUID9"), t9, _L("text to find"));
	StoreEntryWithTextL(aEntryView, CCalEntry::EReminder, _L8("GUID0"), t0, _L("text to not find"));
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime();
	aData.iSearchString = _L("text to find").AllocL();
	aData.iFilter = CalCommon::EIncludeAppts|CalCommon::EIncludeEvents|CalCommon::EIncludeCompletedTodos|CalCommon::EIncludeIncompletedTodos;
	
	aData.iTimes.AppendL(t1);
	aData.iTimes.AppendL(t3 + TTimeIntervalHours(1)); // add one hour because Todo has end date as instance date
	aData.iTimes.AppendL(t5);

	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::CreateMaxRangeTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test filtering instances found correctly when searching maximum range"));
		
	TTime t1(TDateTime(2005, EJanuary, 10, 9, 0, 0, 0)); // 11 Jan, 09:00
	TTime t2(TDateTime(2005, EFebruary, 10, 9, 0, 0, 0)); // 11 Feb, 09:00
	TTime t3(TDateTime(2005, EMarch, 10, 9, 0, 0, 0)); // 11 Mar, 09:00
	TTime t4(TDateTime(2005, EApril, 10, 9, 0, 0, 0)); // 11 Apr, 09:00
	TTime t5(TDateTime(2005, EMay, 10, 9, 0, 0, 0)); // 11 May, 09:00
	
	// add entry with text
	StoreEntryWithTextL(aEntryView, CCalEntry::EAppt, _L8("GUID1"), t1, _L(""));
	StoreEntryWithTextL(aEntryView, CCalEntry::EEvent, _L8("GUID2"), t2, _L(""));
	StoreEntryWithTextL(aEntryView, CCalEntry::EReminder, _L8("GUID3"), t3, _L(""));
	StoreEntryWithTextL(aEntryView, CCalEntry::ETodo, _L8("GUID4"), t4, _L(""));
	StoreEntryWithTextL(aEntryView, CCalEntry::EAnniv, _L8("GUID5"), t5, _L(""));
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime();
	aData.iFilter = CalCommon::EIncludeReminder|CalCommon::EIncludeAnnivs|CalCommon::EIncludeIncompletedTodos;
	
	aData.iTimes.AppendL(t3);
	aData.iTimes.AppendL(t4 + TTimeIntervalHours(1));
	aData.iTimes.AppendL(t5);

	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}

	
void CTestData::CreateMaxRangeTest5L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test filtering incompleted todos found correctly when searching maximum range"));

	// create one completed todo and one incomplete todo
	TTime todoTime1(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0));
	CCalEntry* entry = CreateTodoLC(KGUID1(), todoTime1, todoTime1); // 26 Jan - completed entry
		
	TTime todoTime2(TDateTime(2005, EJanuary, 15, 0, 0, 0, 0)); 
	CCalEntry* entry2 = CreateTodoLC(KGUID1(), todoTime2, Time::NullTTime()); // 16 Jan - incomplete entry
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	array.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry);
	
	aData.iFilter = CalCommon::EIncludeIncompletedTodos;
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime();

	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJanuary, 15, 0, 0, 0, 0)));	// 16 Jan 05

	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::CreateMaxRangeTest6L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for completed todos only."));
	
	// create one completed todo and one incomplete todo
	TTime todoTime1(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0));
	CCalEntry* entry = CreateTodoLC(KGUID1(), todoTime1, todoTime1); // 26 Jan - completed entry
		
	TTime todoTime2(TDateTime(2005, EJanuary, 15, 0, 0, 0, 0)); 
	CCalEntry* entry2 = CreateTodoLC(KGUID2(), todoTime2, Time::NullTTime()); // 16 Jan - incomplete entry
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	array.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry);
	
	aData.iFilter = CalCommon::EIncludeCompletedTodos;
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime();
	
	// expected times:
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJanuary, 25, 0, 0, 0, 0)));	// 16 Jan 05

	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::CreateMaxRangeTest7L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for alarmed entries only (max range)."));
	
	TTime startTime1(TDateTime(2005, EFebruary, 0, 0, 0, 0, 0)); 	//  1 Feb 05
	
	// create alarmed entry
	CCalEntry* entry1 = CreateEntryLC(CCalEntry::EAppt, KGUID1(), startTime1, startTime1 + TTimeIntervalHours(1));
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(30);
	entry1->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	
	TTime startTime2(TDateTime(2005, EMarch, 0, 0, 0, 0, 0)); 	//  1 Mar 05
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::EAppt, KGUID2(), startTime2, startTime2 + TTimeIntervalHours(1));
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry1);
	array.AppendL(entry2);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry1);
	
	aData.iFilter = CalCommon::EIncludeAlarmedOnly|CalCommon::EIncludeAll;
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime();
	
	// expected times:
	aData.iTimes.AppendL(startTime1);	// 16 Jan 05

	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::CreateMaxRangeTest8L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Filter test for next repeat only (max range)."));
	
	TCalRRule rpt(TCalRRule::EYearly);
	rpt.SetInterval(12);
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2004, EFebruary, 28, 0, 0, 0, 0)); 	// 29 Feb 2004
	rpt.SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2099, EMarch, 30, 0, 0, 0, 0));			// 2009
	rpt.SetUntil(endTime);
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iFilter = CalCommon::EIncludeRptsNextInstanceOnly|CalCommon::EIncludeAppts;
	
	// expected times:
	aData.iTimes.AppendL(startTime.TimeLocalL());	// 29 Feb 04

	TTime* currentTime = new (ELeave) TTime;
	currentTime->UniversalTime();
	aData.iFunction = new (ELeave) TCallBack(&CTestData::CompleteManyInstanceTest, (TAny*)currentTime);
	}


void CTestData::SetupMultipleTextFieldsTestL(CCalEntryView* aView)
	{
	// This test creates a number of entries with these test strings in different fields.
	// These strings are searched for separately in a number of tests that call this function.
	_LIT(KTestString1, "test1");
	_LIT(KTestString2, "test2");
	_LIT(KTestString3, "test3");
	_LIT(KTestString4, "test4");
	_LIT(KTestString5, "test5");
	_LIT(KTestString6, "test6");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	// First entry is at 12:00 on 1 Jan 2006
	TTime entryTime1 = TDateTime(2006, EJanuary, 0, 12, 0, 0, 0); 
	CCalEntry* entry1 = CreateEntryLC(CCalEntry::EAppt, KGUID1, entryTime1, entryTime1);
	entry1->SetDescriptionL(KTestString1);
	entry1->SetLocationL(KTestString2);
	
	CCalAttendee* attendee1 = CCalAttendee::NewL(KTestString2, KTestString1);
	CleanupStack::PushL(attendee1);
	attendee1->SetCommonNameL(KTestString6);
	entry1->AddAttendeeL(attendee1);
	CleanupStack::Pop(attendee1);

	entries.AppendL(entry1);
	CleanupStack::Pop(entry1);
	
	// Entry at 12:00 on 1 Feb 2006
	TTime entryTime2 = TDateTime(2006, EFebruary, 0, 12, 0, 0, 0);
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::ETodo, KGUID2, entryTime2, entryTime2);
	entry2->SetDescriptionL(KTestString6);
	entry2->SetLocationL(KTestString1);
	
	CCalAttendee* attendee2 = CCalAttendee::NewL(KTestString1, KTestString2);
	CleanupStack::PushL(attendee2);
	attendee2->SetCommonNameL(KTestString3);
	entry2->AddAttendeeL(attendee2);
	CleanupStack::Pop(attendee2);

	entries.AppendL(entry2);
	CleanupStack::Pop(entry2);
	
	// Entry at 12:00 on 1 Mar 2006
	TTime entryTime3 = TDateTime(2006, EMarch, 0, 12, 0, 0, 0);
	CCalEntry* entry3 = CreateEntryLC(CCalEntry::ETodo, KGUID3, entryTime3, entryTime3);
	entry3->SetDescriptionL(KTestString6);
	entry3->SetLocationL(KTestString3);
	
	CCalAttendee* attendee3 = CCalAttendee::NewL(KTestString4, KTestString4);
	CleanupStack::PushL(attendee3);
	attendee3->SetCommonNameL(KTestString5);
	entry3->AddAttendeeL(attendee3);
	CleanupStack::Pop(attendee3);
	
	entries.AppendL(entry3);
	CleanupStack::Pop(entry3);
	
	// Entry at 12:00 on 1 Apr 2006
	TTime entryTime4 = TDateTime(2006, EApril, 0, 12, 0, 0, 0);
	CCalEntry* entry4 = CreateEntryLC(CCalEntry::EReminder, KGUID4, entryTime4, entryTime4);
	entry4->SetDescriptionL(KTestString3);
	entry4->SetLocationL(KTestString6);
	
	CCalAttendee* attendee4 = CCalAttendee::NewL(KTestString3, KTestString4);
	CleanupStack::PushL(attendee4);
	attendee4->SetCommonNameL(KTestString5);
	entry4->SetOrganizerL(attendee4);
	CleanupStack::Pop(attendee4);

	entries.AppendL(entry4);
	CleanupStack::Pop(entry4);
	
	TInt success = 0;
	aView->StoreL(entries, success);
	
	CleanupStack::PopAndDestroy(&entries);
	}

	
void CTestData::CreateStringInFieldTest1L(CTestDataStruct& aData, CCalEntryView* aView)
	{
	aData.iDescription.Copy(_L("Filter test for description and attendee address."));
	
	SetupMultipleTextFieldsTestL(aView);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iSearchBehaviour = CalCommon::TCalSearchBehaviour(CalCommon::ESearchEntryDescription | CalCommon::ESearchAttendeeAddress);
	aData.iSearchString = _L("test1").AllocL();
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2006, EJanuary, 0, 12, 0, 0, 0));	// 1 Jan
	aData.iTimes.AppendL(TDateTime(2006, EFebruary, 0, 12, 0, 0, 0));	// 1 Feb 
	}

	
void CTestData::CreateStringInFieldTest2L(CTestDataStruct& aData, CCalEntryView* aView)
	{
	aData.iDescription.Copy(_L("Filter test for location and attendee sent-by address."));
	
	SetupMultipleTextFieldsTestL(aView);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iSearchBehaviour = CalCommon::TCalSearchBehaviour(CalCommon::ESearchEntryLocation | CalCommon::ESearchAttendeeSentByAddress);
	aData.iSearchString = _L("test2").AllocL();
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2006, EJanuary, 0, 12, 0, 0, 0));	// 1 Jan
	aData.iTimes.AppendL(TDateTime(2006, EFebruary, 0, 12, 0, 0, 0));	// 1 Feb 
	}

	
void CTestData::CreateStringInFieldTest3L(CTestDataStruct& aData, CCalEntryView* aView)
	{
	aData.iDescription.Copy(_L("Filter test for attendee common name and organizer address."));
	
	SetupMultipleTextFieldsTestL(aView);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iSearchBehaviour = CalCommon::TCalSearchBehaviour(CalCommon::ESearchAttendeeCommonName | CalCommon::ESearchOrganizerAddress);
	aData.iSearchString = _L("test3").AllocL();
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2006, EFebruary, 0, 12, 0, 0, 0));	// 1 Feb
	aData.iTimes.AppendL(TDateTime(2006, EApril, 0, 12, 0, 0, 0));	// 1 Apr
	}

	
void CTestData::CreateStringInFieldTest4L(CTestDataStruct& aData, CCalEntryView* aView)
	{
	aData.iDescription.Copy(_L("Filter test for organizer sent-by address."));
	
	SetupMultipleTextFieldsTestL(aView);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iSearchBehaviour = CalCommon::ESearchOrganizerSentByAddress;
	aData.iSearchString = _L("test4").AllocL();
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2006, EApril, 0, 12, 0, 0, 0));	// 1 Apr 
	}

	
void CTestData::CreateStringInFieldTest5L(CTestDataStruct& aData, CCalEntryView* aView)
	{
	aData.iDescription.Copy(_L("Filter test for organizer common name."));
	
	SetupMultipleTextFieldsTestL(aView);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iSearchBehaviour = CalCommon::ESearchOrganizerCommonName;
	aData.iSearchString = _L("test5").AllocL();
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2006, EApril, 0, 12, 0, 0, 0));	// 1 Apr 
	}

	
void CTestData::CreateStringInFieldTest6L(CTestDataStruct& aData, CCalEntryView* aView)
	{
	aData.iDescription.Copy(_L("Filter test for multiple selected fields."));
	
	SetupMultipleTextFieldsTestL(aView);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime(); 
	
	aData.iSearchBehaviour = CalCommon::ESearchAllTextFields;
	aData.iSearchString = _L("test6").AllocL();
	
	// expected times:
	aData.iTimes.AppendL(TDateTime(2006, EJanuary, 0, 12, 0, 0, 0));	// 1 Jan 
	aData.iTimes.AppendL(TDateTime(2006, EFebruary, 0, 12, 0, 0, 0));	// 1 Feb 
	aData.iTimes.AppendL(TDateTime(2006, EMarch, 0, 12, 0, 0, 0));	// 1 Mar 
	aData.iTimes.AppendL(TDateTime(2006, EApril, 0, 12, 0, 0, 0));	// 1 Apr 
	}

void CTestData::CreateRepeatingWithSecondGranularityL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Search for repeating entries with granularity in seconds")); 	
	
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EWednesday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);

	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2006, EFebruary, 0, 9, 0, 5, 0));
	rpt.SetDtStart(startTime); 	// 1 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2006, EFebruary, 22, 12, 0, 5, 0));
	rpt.SetUntil(endTime);			// Feb 22
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2006, EFebruary, 0, 0, 0, 0, 0);		//  1 Feb
	aData.iEndDate = TDateTime(2006, EFebruary, 27, 0, 0, 0, 0); 		// 28 Feb

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary,  0, 9, 0, 5, 0))); //  1 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary,  7, 9, 0, 5, 0))); //  8 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 14, 9, 0, 5, 0))); // 15 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 21, 9, 0, 5, 0))); // 22 Feb
	}

void CTestData::CreateRepeatingWithMSecondGranularityL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Search for repeating entries with granularity in microseconds")); 	
	
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EWednesday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);

	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2006, EFebruary, 0, 9, 30, 5, 100));
	rpt.SetDtStart(startTime); 	// 1 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2006, EFebruary, 22, 23, 59, 59, 999));
	rpt.SetUntil(endTime);			// Feb 23
	AddRuleL(rpt, aEntryView, KGUID1(), CCalEntry::EAppt);
	
	aData.iStartDate = TDateTime(2006, EFebruary, 0, 0, 0, 0, 0);		//  1 Feb
	aData.iEndDate = TDateTime(2006, EFebruary, 27, 0, 0, 0, 0); 		// 28 Feb

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary,  0, 9, 30, 5, 100))); //  1 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary,  7, 9, 30, 5, 100))); //  8 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 14, 9, 30, 5, 100))); // 15 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 21, 9, 30, 5, 100))); // 22 Feb
	}


void CTestData::CreateRepeatingWithRuleAndDatesL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Search for repeating entries with both RRules and RDates")); 	
	
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EWednesday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);

	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2006, EFebruary, 0, 9, 0, 0, 0));
	rpt.SetDtStart(startTime); 	// 1 Feb
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2006, EFebruary, 22, 12, 0, 0, 0));
	rpt.SetUntil(endTime);			// Feb 22

	RArray<TCalTime> rdates;
	CleanupClosePushL(rdates);
	TCalTime rdate1;
	rdate1.SetTimeLocalL(TTime(TDateTime(2006,EFebruary, 7, 11, 30, 0, 0)));	// 8 Feb
	rdates.AppendL(rdate1);

	CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KGUID1(), rpt.DtStart().TimeLocalL(), rpt.DtStart().TimeLocalL() + TTimeIntervalHours(1));
	entry->SetRRuleL(rpt);
	entry->SetRDatesL(rdates);
		
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.AppendL(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::PopAndDestroy(&array);
	
	CleanupStack::PopAndDestroy(entry);

	CleanupStack::PopAndDestroy(&rdates);
	
	aData.iStartDate = TDateTime(2006, EFebruary, 0, 0, 0, 0, 0);		//  1 Feb
	aData.iEndDate = TDateTime(2006, EFebruary, 27, 0, 0, 0, 0); 		// 28 Feb

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary,  0,  9, 0, 0, 0)));	//  1 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary,  7,  9, 0, 0, 0)));	//  8 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary,  7, 11, 30, 0, 0)));	//  8 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 14,  9, 0, 0, 0)));	// 15 Feb
	aData.iTimes.AppendL(TTime(TDateTime(2006, EFebruary, 21,  9, 0, 0, 0)));	// 22 Feb
	}
	
void CTestData::CreateNextInstanceOnlyTestL(CTestDataStruct& aData, CCalEntryView* aView)
	{
	aData.iDescription.Copy(_L("Search for next instance only of a repeating weekly and daily entry")); 	
	
	// Every Wednesday from 1st January 2006, 0900
	TCalRRule rpt(TCalRRule::EWeekly);
	rpt.SetInterval(1);
	
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EWednesday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
 
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 9, 0, 0, 0));
	rpt.SetDtStart(startTime);
	rpt.SetCount(100);
	AddRuleL(rpt, aView, KGUID1(), CCalEntry::ETodo);

	// Every 3 days from 9th January 2006, 1500
	TCalRRule rpt2(TCalRRule::EDaily);
	rpt2.SetInterval(3);

	startTime.SetTimeLocalL(TDateTime(2006, EJanuary, 8, 15, 0, 0, 0));
	rpt2.SetDtStart(startTime); 
	rpt2.SetCount(100);
	AddRuleL(rpt2, aView, KGUID2(), CCalEntry::ETodo);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime();

	aData.iFilter = CalCommon::EIncludeAll|CalCommon::EIncludeRptsNextInstanceOnly;
	
	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2006, EJanuary, 3, 10, 0, 0, 0)));	//  4 Jan (weekly rule)
	aData.iTimes.AppendL(TTime(TDateTime(2006, EJanuary, 8, 16, 0, 0, 0)));	//  9 Jan (3-daily rule)
	}

void CTestData::DeleteEntryLastInstanceL(CTestDataStruct& aData, CCalEntryView* aEntryView, CCalInstanceView* aInstanceView)
	{
	aData.iDescription.Copy(_L("Check if deleting an instance would delete the child")); 	
	
	// Add Entries for Test
	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);

	//create an simple entry and store it to the file
	_LIT8(KUid, "UID_xx");
	HBufC8* guid = KUid().AllocLC();	
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->SetSummaryL(_L("random text"));
	entriesToStore.AppendL(entry);
	CleanupStack::Pop(entry);
		
	TTime startTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
	TTime endTime (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)); 
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);

	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EDaily);
	CleanupStack::PushL(rpt);
	TTime daily1end(TDateTime(2005, EJanuary, 8, 10, 0, 0, 0));
	TCalTime rptendTime;
	rptendTime.SetTimeLocalL(daily1end);

	rpt->SetDtStart(calStartTime);
	rpt->SetUntil(rptendTime);
	rpt->SetInterval(1);
	entry->SetRRuleL(*rpt);//take a copy of the rule
	CleanupStack::PopAndDestroy(rpt);
	
	TInt entriesStored(0);
	aEntryView->StoreL(entriesToStore, entriesStored);
	ASSERT(entriesStored == 1);
	entriesToStore.ResetAndDestroy();
	
	//create a child entry and store it
	guid = KUid().AllocLC();
	TTime ttime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0));
	TCalTime recurrenceId;
	recurrenceId.SetTimeUtcL(ttime);
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodRequest, 0, recurrenceId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	// Set child entry time
	TTime newStartDate (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0));
	TTime newEndDate (TDateTime(2005, EJanuary, 7, 12, 0, 0, 0));
	
	TCalTime newCalstartTime;
	newCalstartTime.SetTimeUtcL(newStartDate);	
	TCalTime newCalendTime;
	newCalendTime.SetTimeUtcL(newEndDate);
	entry->SetStartAndEndTimeL(newCalstartTime, newCalendTime);
	
	entriesToStore.AppendL(entry);
	CleanupStack::Pop(entry);
	aEntryView->StoreL(entriesToStore, entriesStored);
	entriesToStore.ResetAndDestroy();
	
	aEntryView->FetchL(KUid, entriesToStore);
	ASSERT(entriesToStore.Count() == 2); //1 parent and 1 child so far
	entriesToStore.ResetAndDestroy();
	
	//search for the second instance
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	TCalTime start;
	start.SetTimeLocalL(TDateTime(2005, EJanuary, 8, 10, 0, 0, 0));
	TCalTime end;
	end.SetTimeLocalL(TDateTime(2005, EJanuary, 8, 11, 0, 0, 0));
	
	CalCommon::TCalTimeRange timeRange(start, end);
	aInstanceView->FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
	ASSERT(instances.Count() == 1);

	//Delete the second instance which should not delete the child but update the parent
	CCalInstance* instance = instances[0];
	aInstanceView->DeleteL(instance, CalCommon::EThisOnly);
	instances.Remove(0);
	instances.ResetAndDestroy();
	aEntryView->FetchL(KUid, entriesToStore);
	
	aData.iStartDate = TCalTime::MinTime();
	aData.iEndDate = TCalTime::MaxTime();
	aData.iFilter = CalCommon::EIncludeAll;
	
	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)));	//  4 Jan (weekly rule)
	
	CleanupStack::PopAndDestroy(&instances);
	CleanupStack::PopAndDestroy(&entriesToStore);
	}	

// Creates an entry that falls on the same day as the search criteria but the
// instance time falls outside of the time of the search. The entry should not
// be found.
void CTestData::TestFindInstanceWithTimeL( CTestDataStruct& aData, 
    CCalEntryView* aEntryView )
    {
    aData.iDescription.Copy( _L( "Instance in calendar in afternoon, should not be found when looking in morning" ) );
	
    // Create entry that starts in the afternoon: May 26, 2-3 pm.
    TTime startTime1( TDateTime( 2006, EMay, 25, 14, 0, 0, 0 ) );
    CCalEntry* entry1 = CreateEntryLC( CCalEntry::EAppt, KGUID1(), startTime1,
        startTime1 + TTimeIntervalHours(1) );
	
    // Store the entry.
    RPointerArray<CCalEntry> entryList;
    CleanupClosePushL( entryList );
    entryList.AppendL( entry1 );
    TInt success = 0;
    aEntryView->StoreL( entryList, success );
    CleanupStack::PopAndDestroy( &entryList );
    CleanupStack::PopAndDestroy( entry1 );
	
    // Set search time range. The test expects no instances to be found so set 
    // the search time range to find all instances on May 26 from 10-11 AM. No 
    // matches should be found, there is 1 entry in the afternoon only.
    aData.iStartDate = TDateTime( 2006, EMay, 25, 10, 0, 0, 0 );
    aData.iEndDate = TDateTime( 2006, EMay, 25, 11, 0, 0, 0 );
    }

void CTestData::TestUpdateInstanceL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	//This test is to test that child entry's recurrence Id can still be fetched 
	//After a copy operation is performed.
	
	// Add Entries for Test
	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);

	//create an simple entry and store it to the file
	_LIT8(KUid, "UID_xx");
	HBufC8* guid = KUid().AllocLC();	
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->SetSummaryL(_L("random text"));
	entriesToStore.AppendL(entry);
	CleanupStack::Pop(entry);
		
	TTime startTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
	TTime endTime (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)); 
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);

	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EDaily);
	CleanupStack::PushL(rpt);
	TTime daily1end(TDateTime(2005, EJanuary, 8, 10, 0, 0, 0));
	TCalTime rptendTime;
	rptendTime.SetTimeLocalL(daily1end);

	rpt->SetDtStart(calStartTime);
	rpt->SetUntil(rptendTime);
	rpt->SetInterval(1);
	entry->SetRRuleL(*rpt);//take a copy of the rule
	CleanupStack::PopAndDestroy(rpt);
	
	TInt entriesStored(0);
	aEntryView->StoreL(entriesToStore, entriesStored);
	ASSERT(entriesStored == 1);
	entriesToStore.ResetAndDestroy();
	
	//create a child entry and store it
	guid = KUid().AllocLC();
	TTime ttime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0));
	TCalTime recurrenceId;
	recurrenceId.SetTimeUtcL(ttime);
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodRequest, 0, recurrenceId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	// Set child entry time
	TTime newStartDate (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0));
	TTime newEndDate (TDateTime(2005, EJanuary, 8, 12, 0, 0, 0));
	
	TCalTime newCalstartTime;
	newCalstartTime.SetTimeUtcL(newStartDate);	
	TCalTime newCalendTime;
	newCalendTime.SetTimeUtcL(newEndDate);
	entry->SetStartAndEndTimeL(newCalstartTime, newCalendTime);
	
	entriesToStore.AppendL(entry);
	aEntryView->StoreL(entriesToStore, entriesStored);
	entriesToStore.ResetAndDestroy();
	CleanupStack::Pop(entry);
	
	//Fetch the entry
	aEntryView->FetchL(KUid, entriesToStore);
	ASSERT(entriesToStore.Count() == 2); //1 parent and 1 child so far
	
	//Following is the copy entry action.
	// Allocate new guid for the entry.
	entry = entriesToStore[1];
	
	HBufC8* copyGuid = KUid().AllocLC();
	CCalEntry::TType type = entry->EntryTypeL();
    CCalEntry::TMethod method = entry->MethodL();
    TInt seqNumber = entry->SequenceNumberL();
    recurrenceId.SetTimeUtcL(Time::NullTTime());//daily1end);
		
	// Create a new child entry for the copy operation.
	CCalEntry*	newEntry = CCalEntry::NewL(type,copyGuid,method,seqNumber, recurrenceId, CalCommon::EThisOnly);
    CleanupStack::Pop(copyGuid);
    CleanupStack::PushL(newEntry);
    // Set the summary to the new child.
    
    newEntry->SetSummaryL(_L("rename"));
	
	 // Retrieve the GUID before copying.
    TPtrC8 guidBeforeCopy = newEntry->UidL();
    ASSERT( guidBeforeCopy == KUid);
    
    // Copy the original entry to the new entry.
	newEntry->CopyFromL(*entry, CCalEntry::ECopyAll);

	//Store the new child entry.
	entriesToStore.ResetAndDestroy();
	entriesToStore.AppendL(newEntry);
	CleanupStack::Pop(newEntry);
	TInt numEntriesStored(0);
    aEntryView->StoreL( entriesToStore, numEntriesStored );
   
	//An instance should be found for the child
	aData.iStartDate = TDateTime(2005, EJanuary, 7, 11, 0, 0, 0);		
	aData.iEndDate = TDateTime(2005, EJanuary, 7, 12, 0, 0, 0);
	
	aData.iTimes.AppendL(TTime(TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)));	//  expected times
	
	CleanupStack::PopAndDestroy(&entriesToStore);
    }

void CTestData::CreateRDateBeforeDtStartTestL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	// to test defect DEF085855
	aData.iDescription.Copy(_L("Add an RDate to a child entry before the DTSTART date"));
	
	// add parent entry, repeating at 10:00 every Tuesday from 2 Feb 2008 for 5 occurences, RDate on Feb 8 at 09:00
	HBufC8* guid = KGUID1().AllocLC();
	CCalEntry* parent = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(parent);
	
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2008, EFebruary, 1, 10, 0, 0, 0));
	parent->SetStartAndEndTimeL(startTime, startTime);
	
	TCalRRule rule(TCalRRule::EWeekly);
	rule.SetDtStart(startTime);
	rule.SetCount(5);
	rule.SetInterval(1);

	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(ETuesday);
	rule.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	
	parent->SetRRuleL(rule);
	
	RArray<TCalTime> rdateArray;
	CleanupClosePushL(rdateArray);
	TCalTime rdate;
	rdate.SetTimeLocalL(TDateTime(2008, EFebruary, 7, 9, 0, 0, 0));
	rdateArray.AppendL(rdate);
	parent->SetRDatesL(rdateArray);
	CleanupStack::PopAndDestroy(&rdateArray);
	
	RArray<TCalTime> exceptionArray;
	CleanupClosePushL(exceptionArray);
	TCalTime exdate1, exdate2;
	exdate1.SetTimeLocalL(TDateTime(2008, EFebruary, 7, 10, 0, 0, 0));
	exdate2.SetTimeLocalL(TDateTime(2008, EFebruary, 22, 10, 0, 0, 0));
	exceptionArray.AppendL(exdate1);
	exceptionArray.AppendL(exdate2);
	parent->SetExceptionDatesL(exceptionArray);
	CleanupStack::PopAndDestroy(&exceptionArray);
	
	// add child entry to replace second instance of repeating parent's rule
	// child entry has DTSTART on 2 March 2008 at 10:00 and RDate on 8 Feb 2008 at 10:00
	TCalTime recId;
	recId.SetTimeLocalL(TDateTime(2008, EFebruary, 11, 10, 0, 0, 0));
	guid = KGUID1().AllocLC();
	CCalEntry* child = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisAndFuture);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(child);
	
	TCalTime chStartTime;
	chStartTime.SetTimeLocalL(TDateTime(2008, EMarch, 1, 10, 0, 0, 0));
	child->SetStartAndEndTimeL(chStartTime, chStartTime);
	
	CleanupClosePushL(rdateArray);
	rdate.SetTimeLocalL(TDateTime(2008, EFebruary, 7, 10, 0, 0, 0));
	rdateArray.AppendL(rdate);
	child->SetRDatesL(rdateArray);
	CleanupStack::PopAndDestroy(&rdateArray);
	
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	TInt success = 0;
	entries.AppendL(parent);
	entries.AppendL(child);
	aEntryView->StoreL(entries, success);
	CleanupStack::PopAndDestroy(&entries);

	CleanupStack::PopAndDestroy(child);
	CleanupStack::PopAndDestroy(parent); 	
	
	aData.iStartDate = TDateTime(2008, EJanuary, 0, 0, 0, 0, 0);	// 1 Jan
	aData.iEndDate = TDateTime(2008, EApril, 0, 0, 0, 0, 0); 		// 1 Apr

	// expected times:	
	// add parent entry, repeating at 10:00 every Tuesday from 2 Feb 2008 for 5 occurences, RDate on Feb 8 at 09:00
	aData.iTimes.AppendL(TTime(TDateTime(2008, EFebruary,  4, 10, 0, 0, 0)));	//  first instance of parent
	aData.iTimes.AppendL(TTime(TDateTime(2008, EFebruary,  7,  9, 0, 0, 0)));	//  RDate of parent
	aData.iTimes.AppendL(TTime(TDateTime(2008, EFebruary,  7, 10, 0, 0, 0)));	//  RDate of child
	aData.iTimes.AppendL(TTime(TDateTime(2008, EMarch,     1, 10, 0, 0, 0)));	//  DTSTART of child
	}

void CTestData::CreateDstBoundaryTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Add a repeating entry with an instance in the 'missing hour' that occurs when the clocks go forwards"));

	TCalTime dtStart;
	dtStart.SetTimeLocalL(TDateTime(2006, EMarch, 24, 1, 30, 0, 0)); // 01:30 on 25th March 06, clocks change on 26th

	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(dtStart);
	rule.SetInterval(1);
	rule.SetCount(4);
		
	AddRuleL(rule, aEntryView, KGUID1);

	aData.iStartDate = TDateTime(2006, EMarch, 24, 0, 0, 0, 0);		// 25 Mar
	aData.iEndDate = TDateTime(2006, EApril, 0, 0, 0, 0, 0); 		//  1 Apr

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 24, 1, 30, 0, 0))); // 01:30 on 25th March
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 25, 2, 30, 0, 0))); // 02:30 on 26th March (01:30 doesn't exist)
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 26, 1, 30, 0, 0))); // 01:30 on 27th March
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 27, 1, 30, 0, 0))); // 01:30 on 28th March
	}

void CTestData::CreateDstBoundaryTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Add a repeating entry with an instance at the start of the 'missing hour' that occurs when the clocks go forwards"));

	TCalTime dtStart;
	dtStart.SetTimeLocalL(TDateTime(2006, EMarch, 24, 1, 0, 0, 0)); // 01:00 on 25th March 06, clocks change on 26th

	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(dtStart);
	rule.SetInterval(1);
	rule.SetCount(4);
		
	AddRuleL(rule, aEntryView, KGUID1);

	aData.iStartDate = TDateTime(2006, EMarch, 24, 0, 0, 0, 0);		// 25 Mar
	aData.iEndDate = TDateTime(2006, EApril, 0, 0, 0, 0, 0); 		//  1 Apr

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 24, 1, 0, 0, 0))); // 01:00 on 25th March
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 25, 2, 0, 0, 0))); // 02:00 on 26th March (01:00 doesn't exist)
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 26, 1, 0, 0, 0))); // 01:00 on 27th March
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 27, 1, 0, 0, 0))); // 01:00 on 28th March
	}

void CTestData::CreateDstBoundaryTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Add a repeating entry with an instance at the end of the 'missing hour' that occurs when the clocks go forwards"));

	TCalTime dtStart;
	dtStart.SetTimeLocalL(TDateTime(2006, EMarch, 24, 2, 0, 0, 0)); // 02:00 on 25th March 06, clocks change on 26th

	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(dtStart);
	rule.SetInterval(1);
	rule.SetCount(4);
		
	AddRuleL(rule, aEntryView, KGUID1);

	aData.iStartDate = TDateTime(2006, EMarch, 24, 0, 0, 0, 0);		// 25 Mar
	aData.iEndDate = TDateTime(2006, EApril, 0, 0, 0, 0, 0); 		//  1 Apr

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 24, 2, 0, 0, 0))); // 02:00 on 25th March
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 25, 2, 0, 0, 0))); // 02:00 on 26th March
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 26, 2, 0, 0, 0))); // 02:00 on 27th March
	aData.iTimes.AppendL(TTime(TDateTime(2006, EMarch, 27, 2, 0, 0, 0))); // 02:00 on 28th March
	}

void CTestData::CreateDstBoundaryTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Add a repeating entry with an instance in the 'repeated hour' that occurs when the clocks go backwards"));

	TCalTime dtStart;
	dtStart.SetTimeLocalL(TDateTime(2006, EOctober, 27, 1, 30, 0, 0)); // 01:30 on 28th October 06, clocks change on 29th

	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(dtStart);
	rule.SetInterval(1);
	rule.SetCount(4);
		
	AddRuleL(rule, aEntryView, KGUID1);

	aData.iStartDate = TDateTime(2006, EOctober, 27, 0, 0, 0, 0);		// 28 Oct
	aData.iEndDate = TDateTime(2006, ENovember, 0, 0, 0, 0, 0); 		//  1 Nov

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2006, EOctober, 27, 1, 30, 0, 0))); // 01:30 on 28th EOctober
	aData.iTimes.AppendL(TTime(TDateTime(2006, EOctober, 28, 1, 30, 0, 0))); // 01:30 on 29th EOctober
	aData.iTimes.AppendL(TTime(TDateTime(2006, EOctober, 29, 1, 30, 0, 0))); // 01:30 on 30th EOctober
	aData.iTimes.AppendL(TTime(TDateTime(2006, EOctober, 30, 1, 30, 0, 0))); // 01:30 on 31st EOctober
	}

void CTestData::CreateDstBoundaryTest5L(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Add an entry when the clocks change and start search from 1 second afterwards"));

	TCalTime dtStart, until;
	dtStart.SetTimeUtcL(TDateTime(2015, EMarch, 28, 1, 0, 0, 0)); // 01:00 on 29th March 2015, when clocks change
	until.SetTimeUtcL(TDateTime(2016, EDecember, 19, 0, 0, 0, 0)); // December 2016
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(dtStart);
	rule.SetInterval(1);
	rule.SetUntil(until);
		
	AddRuleL(rule, aEntryView, KGUID1);

	aData.iStartDate = TDateTime(2015, EMarch, 28, 2, 0, 1, 0);		// 02:00:01 on 29th March 2015 local
	aData.iEndDate = TDateTime(2015, EMarch, 28, 5, 30, 0, 0); 		// 05:30:00 on 29th March 2015 local

	// expected times:	
	aData.iTimes.AppendL(TTime(TDateTime(2015, EMarch, 28, 2, 0, 0, 0))); // 02:00 on 29th March 2015 local
	}

void CTestData::ExcludeInstanceAdjoiningRangeTestNonRepeatL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test excluding non-repeating instances that adjoin the search range"));
	
	aData.iStartDate = TDateTime(2015, EApril, 27, 0, 0, 0, 0);
	aData.iEndDate = TDateTime(2015, EApril, 28, 0, 0, 0, 0);
	
	aData.iFilter |= CalCommon::EExcludeInstanceAdjoiningRange;
	
	CCalEntry* entry1 = CreateEntryLC(CCalEntry::EAppt, _L8("ExcludeInstanceAdjoiningRange1"), aData.iStartDate - TTimeIntervalHours(1), aData.iStartDate);
	CCalEntry* entry2 = CreateEntryLC(CCalEntry::EAppt, _L8("ExcludeInstanceAdjoiningRange2"), aData.iStartDate, aData.iStartDate);
	CCalEntry* entry3 = CreateEntryLC(CCalEntry::EAppt, _L8("ExcludeInstanceAdjoiningRange3"), aData.iEndDate, aData.iEndDate);
	CCalEntry* entry4 = CreateEntryLC(CCalEntry::EAppt, _L8("ExcludeInstanceAdjoiningRange4"), aData.iEndDate, aData.iEndDate + TTimeIntervalHours(1));

	RPointerArray<CCalEntry> entryList;
	CleanupClosePushL(entryList);
	entryList.AppendL(entry1);
	entryList.AppendL(entry2);
	entryList.AppendL(entry3);
	entryList.AppendL(entry4);
	TInt success(0);
	aEntryView->StoreL(entryList, success); // takes ownership
	CleanupStack::PopAndDestroy(&entryList);
	CleanupStack::PopAndDestroy(entry4);
	CleanupStack::PopAndDestroy(entry3);
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry1);
	
	// Expected times:	
	// With EExcludeInstanceAdjoiningRange set we should only expect the instance that starts and ends on the start range
	aData.iTimes.AppendL(aData.iStartDate);
	}

void CTestData::ExcludeInstanceAdjoiningRangeTestRepeatL(CTestDataStruct& aData, CCalEntryView* aEntryView)
	{
	aData.iDescription.Copy(_L("Test excluding repeating instances that adjoin the search range"));
	
	aData.iStartDate = TDateTime(2015, EApril, 27, 0, 0, 0, 0);
	aData.iEndDate = TDateTime(2015, EApril, 28, 0, 0, 0, 0);
	
	aData.iFilter |= CalCommon::EExcludeInstanceAdjoiningRange;
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(2);
	rule.SetCount(3);
	
	TCalTime dtStart;
			
	dtStart.SetTimeLocalL(aData.iStartDate - TTimeIntervalHours(1));
	rule.SetDtStart(dtStart);
	AddRuleL(rule, aEntryView, _L8("ExcludeInstanceAdjoiningRange1"), CCalEntry::EEvent, TTimeIntervalHours(1));
	
	dtStart.SetTimeLocalL(aData.iStartDate);
	rule.SetDtStart(dtStart);
	AddRuleL(rule, aEntryView, _L8("ExcludeInstanceAdjoiningRange2"), CCalEntry::EEvent, TTimeIntervalHours(0));
	
	dtStart.SetTimeLocalL(aData.iEndDate);
	rule.SetDtStart(dtStart);
	AddRuleL(rule, aEntryView, _L8("ExcludeInstanceAdjoiningRange3"), CCalEntry::EEvent, TTimeIntervalHours(0));
	
	dtStart.SetTimeLocalL(aData.iEndDate);
	rule.SetDtStart(dtStart);
	AddRuleL(rule, aEntryView, _L8("ExcludeInstanceAdjoiningRange4"), CCalEntry::EEvent, TTimeIntervalHours(1));
	
	// Expected times:	
	// With EExcludeInstanceAdjoiningRange set we should only expect the instance that starts and ends on the start range
	aData.iTimes.AppendL(aData.iStartDate);
	}

void CTestData::TestTodoOnMaxDateL( CTestDataStruct& aData, CCalEntryView* aEntryView )
	{
    aData.iDescription.Copy( _L( "Create Todo's (dated and undated) on Max Calendar Date,and find instances, There should not be any duplicate instances" ) );
	
	TTime time;	
	time.HomeTime();
	// Setting the seconds and microseconds to 0, Please refer to NOTE in CDummyCalendarApp::ExtractFromDatabaseL() 
	// Making sure the code below and the result check are done in the same minute by setting the time at the start
	// of the current minute
	TDateTime dateTime = time.DateTime();
	dateTime.SetMicroSecond(0);
	dateTime.SetSecond(0);
	TTime newTime(dateTime);
	User::SetHomeTime(newTime);
	RPointerArray<CCalEntry> entryList;
    CleanupResetAndDestroyPushL( entryList );
    
    // Create todo entry that on: Dec 31 2100
    TTime startTime( TDateTime( 2100, EDecember, 30, 0, 0, 0, 0 ) );
    CCalEntry* entry = CreateEntryLC( CCalEntry::ETodo, KGUID1(), startTime,startTime);
    entryList.AppendL( entry );
    CleanupStack::Pop( entry ); 
	
	// Create todo entry that on: Dec 31 2100
    entry = CreateEntryLC( CCalEntry::ETodo, KGUID2(), startTime,startTime);
    entryList.AppendL( entry );
    CleanupStack::Pop( entry );
	
	// Create an undated todo entry
	TTime nullTime = Time::NullTTime();
    entry = CreateEntryLC( CCalEntry::ETodo, KGUID3(), nullTime, nullTime);
    entryList.AppendL( entry );
    CleanupStack::Pop( entry );
	
	// Create an undated todo entry
    entry = CreateEntryLC( CCalEntry::ETodo, KGUID4(), nullTime, nullTime);
    entryList.AppendL( entry );
    CleanupStack::Pop( entry );

    TInt success = 0;
    aEntryView->StoreL( entryList, success );
    // Store the entry
    CleanupStack::PopAndDestroy( &entryList );
  	
    // Set search time range to Min and Max time.
    aData.iStartDate = TCalTime::MinTime().DateTime();  //Search Range
    aData.iEndDate = TCalTime::MaxTime().DateTime();
    aData.iTimes.AppendL(TTime(TDateTime( 2100, EDecember, 30, 0, 0, 0, 0 )));	//  expected times
    aData.iTimes.AppendL(TTime(TDateTime( 2100, EDecember, 30, 0, 0, 0, 0 )));	//  expected times
    aData.iTimes.AppendL(TTime(dateTime));	//  expected times
    aData.iTimes.AppendL(TTime(dateTime));	//  expected times		
	}

