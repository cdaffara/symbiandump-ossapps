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

#ifndef __TCAL_GSENTRY_TESTDATA_H__
#define __TCAL_GSENTRY_TESTDATA_H__

#include <e32base.h>
#include <calentry.h>
#include <calinstanceview.h>
#include <calcommon.h>
#include <e32test.h>

class TCalRRule;
class CCalEntryView;

static RTest test(_L("tCal_GsEntry"));

_LIT8(KGUID1, "A_Global_UID_1");
_LIT8(KGUID2, "A_Global_UID_2");
_LIT8(KGUID3, "A_Global_UID_3");


enum TExceptionRDate
	{
	EException,
	ERDate,
	EExceptionAndRDate,
	ERDateBeforeRRule,
	ERDateAfterRRule
	};
	
	
struct TTestDataStruct
	{
public:
	enum TTestType
		{
		EEntryTest,
		EInstanceTest,
		EGetSetTest,
		EUpdateTest
		};
		
	static const TInt KDescriptionLen = 628;
		
public:
	TTestDataStruct();
	~TTestDataStruct();
	
	RArray<TTime> 				iTimes; // local time
	TTime 						iStartDate;
	TTime 						iEndDate;
	TBuf<KDescriptionLen> 		iDescription;
	CalCommon::TCalViewFilter 	iFilter;
	TCallBack*					iFunction;
	HBufC*						iSearchString;
	CalCommon::TCalSearchBehaviour iSearchBehaviour;
	
	RArray<TPtrC8> 				iGuidsToExtract;
	TTestType					iTestType;
	RArray<TTime> 				iRecIds; // Utc for fixed time test, local for floating time test
	};

class CTestData : public CBase
	{
public:
	enum TTestNumber
		{
		// Entry Tests
		ECreateParent,
		ECreateAndDeleteParent,
		ECreateSameParentTwice,
		ECreateAndSingleDeleteParent,
		ECreateParentAnd1Child,
		ECreateParentAnd2Child,
		EReplaceExistingChild,
		EAddMultipleEntries,
		ETestGetSetMethods,
		ETestUpdate,
		
		
		// Instance Tests
		EGetInstancesParentOnly,
		EGetInstancesParentWithExceptionDates,
		EGetInstancesParentWithSporadicDates,
		EGetInstancesParentAnd1ChildException,
		EGetInstancesParentAnd1ChildExceptionMultiFetch,
		EGetInstancesParentAnd1ChildRule,
		EGetInstancesParentAnd1ChildRuleMultiFetch,
		
		// Instance Tests specifically to test a child entry modifying the parent rpt rule
		EGetInstancesParentWithExceptionsAnd1ChildRuleThisAndFuture,
		EGetInstancesParentWithExceptionsRDatesAnd1ChildRuleThisAndFuture,
		EGetInstancesParentWithExceptionsRDatesNonRptChildsAnd1ChildRuleThisAndFuture,
		EGetInstancesParentWithExceptionsAnd1ChildRuleThisAndPrior,
		EGetInstancesParentWithExceptionsRDatesAnd1ChildRuleThisAndPrior,
		EGetInstancesParentWithExceptionsRDatesNonRptChildsAnd1ChildRuleThisAndPrior,
		
		EDeleteAllChildExceptions,
		EDeleteEntryThatHasNoInstances,
		EStoreRestoreParentWithAChild,
		
		// These are similar to the above but test peculiar situations
		EGetInstancesParentWithExceptionsAnd2ChildRuleThisAndFuture,	// Attempt storing 2 child rpt rules

		EGetInstancesParentAnd1ChildExceptionModify, // DEF078804: get the rec id of child through instance.
		EGetInstancesParentWithRDateBeforeRRule,
		EGetInstancesParentWithRDateAfterRRule,
		
		ENumberOfTestsForFixedAndFloating,
		
		// failure test
		ECreateParentInvalidChildFloating = ENumberOfTestsForFixedAndFloating, // use the same enum so that floating test can continue
		ECreateParentInvalidChildFloating2,
	
		// Some defect fixes
		EStoreParentHaveNoInstanceAndChildren,

		ENumberOfTests,
		};
	
    static TTestDataStruct* GetTestDataL(TTestNumber aTestNumber, CCalInstanceView* aView, CCalEntryView* aEntryView, TBool aFloating, TBool aSummerTime = EFalse);

private:
	static void	CreateParentGsEntryL(TCalRRule* aRpt, CCalEntryView* aView, const TDesC8& aGuid, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	static CCalEntry* CreateParentLC(TCalRRule* aRpt, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating);
	static void DeleteParentGsEntryL(CCalEntryView* aEntryView, const TDesC8& aGuid);
	static void SingleDeleteParentGsEntryL(CCalEntryView* aEntryView, const TDesC8& aGuid);
	static void CreateChildGsEntryL(TTestDataStruct* aTestData, TInt aRecIdToUse, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	
	static void DeleteAllEntriesByGuidL(CCalEntryView* aView);
	
	static void AddMultipleEntriesL(TCalRRule* aRpt, CCalEntryView* aView, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	
	static void SetExceptionsL(CCalEntry* aEntry, RArray<TTime>& aTimes, TBool aFloating);
	static void CreateParentGsEntryWithExceptionsL(TCalRRule* aRpt, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	static void CreateParentGsEntryWithSporadicsL(TCalRRule* aRpt, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	
	static void CreateChildExceptionGsEntryL(TTestDataStruct* aTestData, TInt aRecIdToUse, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	static void CreateChildExceptionGsEntry_1L(TTestDataStruct* aTestData, TInt aRecIdToUse, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	static void CreateChildRRuleGsEntryL(TCalRRule* aRpt, TTestDataStruct* aTestData, TInt aRecIdToUse, CalCommon::TRecurrenceRange aRange, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	
	static TCalRRule* GetWeeklyRuleL(TDay aDay);

    static TCalRRule* GetDailyTestData1L(TTestDataStruct& aData, TBool aSummerTime = EFalse);
	static TCalRRule* GetWeeklyTestData1L(TTestDataStruct& aData);
	static TCalRRule* GetWeeklyTestData1FloatingL(TTestDataStruct& aData);
	static TCalRRule* GetWeeklyTestData2L(TTestDataStruct& aData);
	static TCalRRule* GetWeeklyTestDataModification1L(TTestDataStruct& aData);
	static TCalRRule* GetWeeklyTestDataModification1FloatingL(TTestDataStruct& aData);

	static void TestSetGetMethodsL(CCalEntryView* aEntryView, const TDesC8& aGuid);
	static void TestSetGetMethodsFloatingL(CCalEntryView* aEntryView, const TDesC8& aGuid);
	static void TestUpdateL(CCalEntryView* aEntryView, const TDesC8& aGuid);
	static void TestDeletionOfChildExceptionsL(CCalEntryView* aEntryView, CCalInstanceView* aInstanceView, const TDesC8& aGuid);
	static void TestDeletionOfEntryThatHasNoInstancesL(TCalRRule* aRpt, TTestDataStruct& aData, CCalEntryView* aEntryView, CCalInstanceView* aInstanceView,
													   const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating);
	static void StoreRestoreL(CCalEntryView* aEntryView, const TDesC8& aGuid);
	
	static void CreateWeeklyParentGsEntryWithExceptionsRdatesL(TTestDataStruct& aData, CCalEntryView* aEntryView, const TDesC8& aGuid, TExceptionRDate aExcpnRDate, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);
	static void CreateWeeklyParentGsEntryWithRDateRRuleL(TTestDataStruct& aData, CCalEntryView* aEntryView, const TDesC8& aGuid, TExceptionRDate aExcpnRDate, CCalEntry::TType aType = CCalEntry::EAppt, TBool aFloating = EFalse);

	static void DoMultipleFetchAndStoreTestExceptionChildL(TTestDataStruct* aTestData, CCalEntryView* aEntryView, const TDesC8& aGuid, TInt aRecIdToUse);
	static void DoMultipleFetchAndStoreTestRuleChildL(TTestDataStruct* aTestData, CCalEntryView* aEntryView, const TDesC8& aGuid, TInt aRecIdToUse);
		
	static void TestEntryFromInstanceAndModifyL(TTestDataStruct* aTestData, CCalEntryView* aView, CCalInstanceView* aInstanceView, const TDesC8& aGuid, TInt aRecIdToUse);
	static void StoreParentHaveNoInstanceAndChildrenL(CCalEntryView* aEntryView, const TDesC8& aGuid);
	};


#endif //__TCAL_GSENTRY_TESTDATA_H__

