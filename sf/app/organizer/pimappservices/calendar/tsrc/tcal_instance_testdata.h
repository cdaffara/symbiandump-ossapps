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

#ifndef __tCal_Instance_TestData_h__
#define __tCal_Instance_TestData_h__

#include <e32base.h>
#include <calinstanceview.h>
#include <calentry.h>

class TCalRRule;
class CCalEntryView;

_LIT8(KGUID1, "A_Global_UID");
_LIT8(KGUID2, "B_Global_UID");
_LIT8(KGUID3, "C_Global_UID");
_LIT8(KGUID4, "D_Global_UID");
_LIT8(KGUID5, "E_Global_UID");

//
// Class for storing test data for each test. This makes it easier to create new tests.
// 
// iDescription describes the test. This is outputted to the test server.
// iTimes stores the expected results of the test - the instances that should be found.
// iStartDate and iEndDate store the time range to be passed into FindInstanceL.
// iFilter stores the filter to be passed into FindInstanceL.
// iSearchString and iSearchBehaviour store the search parameters to be passed into FindInstanceL.
// iFunction can specify a pointer to a function which can be called to verify results. Not currently used.
//
class CTestDataStruct : public CBase
	{
public:
	CTestDataStruct();
	~CTestDataStruct();
	
	RArray<TTime>					iTimes;
	TTime							iStartDate;
	TTime							iEndDate;
	TBuf<256>						iDescription;
	CalCommon::TCalViewFilter		iFilter;
	TCallBack*						iFunction;
	HBufC*							iSearchString;
	CalCommon::TCalSearchBehaviour	iSearchBehaviour;
	};

//
// Class for setting up the test data.
// The current test number is passed into GetTestDataL, which sets up the entry to be tested.
//
class CTestData : public CBase
	{
public:
	enum TTestNumber
		{
		EDailyTest1,				// 0
		EDailyTest2,
		EDailyTest3,
		EDailyTest4,
		EDailyTest5,
		EDailyTest6,
		EDailyTest7,
		EWeeklyTest1,
		EWeeklyTest2,
		EWeeklyTest3,
		EWeeklyTest4,				// 10
		EWeeklyTest1980_5,
		EMonthlyTest1,
		EMonthlyTest2,
		EMonthlyTest3,
		EMonthlyTest4,
		EYearlyTest1,
		EYearlyTest2,
		EYearlyTest3,
		EYearlyTest4,
		EYearlyTest5,				// 20
		EYearlyTest1980_6,
		EMultipleTest1,
		EMultipleTest2,
		EFilterTest1,
		EFilterTest2,
		EFilterTest3,
		EFilterTest4,
		EFilterTest5,
		EFilterTest6,
		EFilterTest7,				// 30
		EDeleteTest1,
		EDeleteTest2,
		EDeleteTest3,
		EDeleteTest4,
		EFindStringTest1,
		EFindStringTest2,
		EFindStringTest3,
		EFindStringTest4,
		EStartEndTimesTest1,
		EStartEndTimesTest2,		// 40
		EStartEndTimesTest3,
		EStartEndTimesTest4,
		EStartEndTimesTest5,
		EStartEndTimesTest6,
		EStartEndTimesTest7,
		EManyInstancesTest,
		EManyInstancesTest2,
		EManyInstancesTest3,
		EMaxRangeTest,
		EMaxRangeTest2,				// 50
		EMaxRangeTest3,
		EMaxRangeTest4,
		EMaxRangeTest5,
		EMaxRangeTest6,
		EMaxRangeTest7,
		EMaxRangeTest8,
		EFindStringInFieldTest1,
		EFindStringInFieldTest2,
		EFindStringInFieldTest3,
		EFindStringInFieldTest4,	// 60
		EFindStringInFieldTest5,
		EFindStringInFieldTest6,
		EEmptyDatabase1,
		EEmptyDatabase2,
		ESecondGranularityTest,
		EMSecondGranularityTest,
		ERuleAndDateTest,
		ENextInstanceOnlyTest,
		EDeleteInstanceTest,
		ERDateBeforeDtStartTest,	// 70
		EDstBoundaryTest1,
		EDstBoundaryTest2,
		EDstBoundaryTest3,
		EDstBoundaryTest4,
		EDstBoundaryTest5,
		EExcludeInstanceAdjoiningRangeTestNonRepeat,
		EExcludeInstanceAdjoiningRangeTestRepeat,
        EFindInstanceWithTimeTest,
        EUpdateInstanceTest,
	    ETestTodoOnMaxDate,
		ENumberOfTests,
		};
	static CTestDataStruct* GenerateTestDataLC(TTestNumber aTestNumber, CCalInstanceView* aView, CCalEntryView* aEntryView);

private:
	// utility functions
	static void	AddRuleL(const TCalRRule& aRpt, CCalEntryView* aView, const TDesC8& aGuid, CCalEntry::TType aType = CCalEntry::EAppt, TTimeIntervalHours aDuration = 1);
	static void AddWeeklyRuleL(TDay aDay, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType);
	static void CreateWeeklyEntryWithThisStringL(TDay aDay, const TDesC& aString, CCalEntryView* aInstanceView, const TDesC8& aGuid);
	static CCalEntry* CreateEntryLC(CCalEntry::TType aType, const TDesC8& aGuid, const TTime& aStartTimeLocal, const TTime& aEndTimeLocal);
	static void StoreEntryWithTextL(CCalEntryView* aEntryView, CCalEntry::TType aType, const TDesC8& aGuid, const TTime& aStartTimeLocal, const TDesC& aSummaryText);
	static CCalEntry* CreateTodoLC(const TDesC8& aGuid, const TTime& aTimeUtc, const TTime& aCompletedTimeUtc);
	static void	CreateFilterTestDataL(CCalEntryView* aView);
	static void	SetupMultipleTextFieldsTestL(CCalEntryView* aView);

	// the tests...
	static void CreateDailyTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateDailyTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateDailyTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateDailyTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateDailyTest5L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateDailyTest6L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateDailyTest7L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateWeeklyTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateWeeklyTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateWeeklyTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateWeeklyTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateWeeklyTest1980_5L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateMonthlyTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateMonthlyTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateMonthlyTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateMonthlyTest4L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateYearlyTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateYearlyTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateYearlyTest3L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateYearlyTest4L(CTestDataStruct& aData, TBool aRepeatForever, CCalEntryView* aEntryView);
	static void CreateYearlyTest1980_6L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateMultipleRuleTest1L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateMultipleRuleTest2L(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void	CreateFilterTest1L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateFilterTest2L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateFilterTest3L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateFilterTest4L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateFilterTest5L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateFilterTest6L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateFilterTest7L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateDeleteTest1L(CTestDataStruct& aData, CCalEntryView* aView, CCalInstanceView* aInstanceView);
	static void	CreateDeleteTest2L(CTestDataStruct& aData, CCalEntryView* aView, CCalInstanceView* aInstanceView);
	static void	CreateDeleteTest3L(CTestDataStruct& aData, CCalEntryView* aView, CCalInstanceView* aInstanceView);
	static void	CreateDeleteTest4L(CTestDataStruct& aData, CCalEntryView* aView, CCalInstanceView* aInstanceView);
	static void	CreateStringTest1L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStringTest2L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStringTest3L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStringTest4L(CTestDataStruct& aData, CCalEntryView* aView);
	static void SetupStartEndTimesTestL(CCalEntryView* aView);
	static void	CreateStartEndTimesTest1L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStartEndTimesTest2L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStartEndTimesTest3L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStartEndTimesTest4L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStartEndTimesTest5L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStartEndTimesTest6L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStartEndTimesTest7L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateUndatedTodoTest1L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	EmptyDatabase1L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	EmptyDatabase2L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateManyInstancesTestL(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateManyInstancesTest2L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateManyInstancesTest3L(CTestDataStruct& aData, CCalEntryView* aView);
	static TInt CompleteManyInstanceTest(TAny* aPtr);
	static void	CreateMaxRangeTestL(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateMaxRangeTest2L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateMaxRangeTest3L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateMaxRangeTest4L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateMaxRangeTest5L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateMaxRangeTest6L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateMaxRangeTest7L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateMaxRangeTest8L(CTestDataStruct& aData, CCalEntryView* aView);
	static void CreateRepeatingWithSecondGranularityL(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateRepeatingWithMSecondGranularityL(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void CreateRepeatingWithRuleAndDatesL(CTestDataStruct& aData, CCalEntryView* aEntryView);
	static void	CreateNextInstanceOnlyTestL(CTestDataStruct& aData, CCalEntryView* aView);
	static void DeleteEntryLastInstanceL(CTestDataStruct& aData, CCalEntryView* aView, CCalInstanceView* aInstanceView);

    static void TestFindInstanceWithTimeL( CTestDataStruct& aData, CCalEntryView* aEntryView );
    static void TestUpdateInstanceL(CTestDataStruct& aData, CCalEntryView* aEntryView);

	static void	CreateStringInFieldTest1L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStringInFieldTest2L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStringInFieldTest3L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStringInFieldTest4L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStringInFieldTest5L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateStringInFieldTest6L(CTestDataStruct& aData, CCalEntryView* aView);
	static void	CreateRDateBeforeDtStartTestL(CTestDataStruct& aData, CCalEntryView* aView);
	static void CreateDstBoundaryTest1L(CTestDataStruct& aData, CCalEntryView* aView);
	static void CreateDstBoundaryTest2L(CTestDataStruct& aData, CCalEntryView* aView);
	static void CreateDstBoundaryTest3L(CTestDataStruct& aData, CCalEntryView* aView);
	static void CreateDstBoundaryTest4L(CTestDataStruct& aData, CCalEntryView* aView);
	static void CreateDstBoundaryTest5L(CTestDataStruct& aData, CCalEntryView* aView);
	static void ExcludeInstanceAdjoiningRangeTestNonRepeatL(CTestDataStruct& aData, CCalEntryView* aView);
	static void ExcludeInstanceAdjoiningRangeTestRepeatL(CTestDataStruct& aData, CCalEntryView* aView);
	static void TestTodoOnMaxDateL(CTestDataStruct& aData, CCalEntryView* aEntryView);
	};

#endif //__tCal_Instance_TestData_h__

