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

#include "tcal_gsentry_testdata.h"
	
#include <calentryview.h>
#include <calinstance.h>
#include <calrrule.h>
#include <calalarm.h>

template <class T>
class CleanupResetAndDestroy
	{
public:
	inline static void PushL(T& aRef);
private:
	static void ResetAndDestroy(TAny *aPtr);
	};
	
template <class T> inline void CleanupResetAndDestroyPushL(T& aRef);


template <class T> inline void CleanupResetAndDestroy<T>::PushL(T& aRef)
	{
	CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&aRef));
	}
	
template <class T> void CleanupResetAndDestroy<T>::ResetAndDestroy(TAny *aPtr)
	{
	(static_cast<T*>(aPtr))->ResetAndDestroy();
	}


template <class T> inline void CleanupResetAndDestroyPushL(T& aRef)
	{
	CleanupResetAndDestroy<T>::PushL(aRef);
	}
	

TTestDataStruct::TTestDataStruct()
	{
	iFilter = CalCommon::EIncludeAll;
	iSearchBehaviour = CalCommon::EExactTextSearch;
	iFunction = NULL;
	iSearchString = NULL;
	}
	
TTestDataStruct::~TTestDataStruct()
	{
	iTimes.Reset();
	delete iFunction;
	delete iSearchString;
	iGuidsToExtract.Close();
	iRecIds.Reset();
	}
	
TTestDataStruct* CTestData::GetTestDataL(TTestNumber aTestNumber, CCalInstanceView* aInstanceView, CCalEntryView* aEntryView, TBool aFloating, TBool aSummerTime)
	{
	TTestDataStruct* testData = new (ELeave) TTestDataStruct();
	testData->iTimes.Reset();
	testData->iRecIds.Reset();
	switch (aTestNumber)
		{
		
		case ECreateParent:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: ECreateParent\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;		
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			}
			break;		
		case ECreateAndDeleteParent:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: ECreateAndDeleteParent, delete using DeleteL(const CDesC8Array&)\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			DeleteParentGsEntryL(aEntryView, KGUID1());
			}
			break;
		case ECreateSameParentTwice:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: ECreateSameParentTwice, create another Parent with the same Guid - 2nd should replace 1st\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			
			delete testData;
			TTestDataStruct* testData1 = new (ELeave) TTestDataStruct();
			testData1->iTimes.Reset();
			testData1->iRecIds.Reset();
			testData1->iGuidsToExtract.AppendL(KGUID1());
			testData1->iDescription.Copy(_L("Entry Test: ECreateSameParentTwice, Create Parent, then create another with the same Guid - 2nd should replace 1st\n"));
			testData1->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetDailyTestData1L(*testData1), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			testData = testData1;
			}
			break;

		case ECreateAndSingleDeleteParent:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: ECreateAndSingleDeleteParent, delete using DeleteL(CCalEntry&)\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			SingleDeleteParentGsEntryL(aEntryView, KGUID1());
			}
			break;
			
		case ECreateParentAnd1Child: 
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: ECreateParentAnd1Child\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			CreateChildGsEntryL(testData, 4, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			}
			break;


		case ECreateParentAnd2Child:
			{
			testData->iGuidsToExtract.AppendL(KGUID2());
			testData->iDescription.Copy(_L("Entry Test: ECreateParentAnd2Child\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID2(), CCalEntry::EAppt, aFloating);
			CreateChildGsEntryL(testData, 4, aEntryView, KGUID2(), CCalEntry::EAppt, aFloating);
			CreateChildGsEntryL(testData, 7, aEntryView, KGUID2(), CCalEntry::EAppt, aFloating);
			}
			break;

		case EAddMultipleEntries:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iGuidsToExtract.AppendL(KGUID2());
			testData->iGuidsToExtract.AppendL(KGUID3());
			testData->iDescription.Copy(_L("Entry Test: EAddMultipleEntries, create multiple Parent GS entries\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			AddMultipleEntriesL(GetDailyTestData1L(*testData), aEntryView, CCalEntry::EAppt, aFloating);
			}
			break;
			
			
		case EReplaceExistingChild:
			{				
			testData->iGuidsToExtract.AppendL(KGUID2());
			testData->iDescription.Copy(_L("Entry Test: EReplaceExistingChild, Add an already existing child (should replace it)\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
            CreateParentGsEntryL(GetDailyTestData1L(*testData, aSummerTime), aEntryView, KGUID2(), CCalEntry::EAppt, aFloating);
			CreateChildGsEntryL(testData, 4, aEntryView, KGUID2(), CCalEntry::EAppt, aFloating);
			CreateChildGsEntryL(testData, 7, aEntryView, KGUID2(), CCalEntry::EAppt, aFloating);
			CreateChildGsEntryL(testData, 4, aEntryView, KGUID2(), CCalEntry::EAppt, aFloating); // Adding identical child should replace the existing one
			}
			break;
			
		case EGetInstancesParentOnly:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentOnly\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;		
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			
			// expected times:
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 5, 0, 0, 0, 0)));		//  6 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 6, 0, 0, 0, 0)));		//  7 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 7, 0, 0, 0, 0)));		//  8 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0)));		//  9 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 9, 0, 0, 0, 0)));		// 10 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 10, 0, 0, 0, 0)));	// 11 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 11, 0, 0, 0, 0)));	// 12 March
			}
			break;

		case EGetInstancesParentWithExceptionDates:
		// Set some exception dates on the parent
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithExceptionDates, Parent with some exception dates\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;		
			CreateParentGsEntryWithExceptionsL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			
			// expected times:
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 6, 0, 0, 0, 0)));		//  7 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 7, 0, 0, 0, 0)));		//  8 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 9, 0, 0, 0, 0)));		// 10 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 10, 0, 0, 0, 0)));	// 11 March
			}
			break;
				
		case EGetInstancesParentWithSporadicDates:
		// Set some sporadic dates on the parent
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithSporadicDates, Parent with some sporadic dates\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;		
			CreateParentGsEntryWithSporadicsL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			
			// expected times:
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 2, 0, 0, 0, 0)));		//  3 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 5, 0, 0, 0, 0)));		//  6 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 6, 0, 0, 0, 0)));		//  7 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 7, 0, 0, 0, 0)));		//  8 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0)));		//  9 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 9, 0, 0, 0, 0)));		// 10 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 10, 0, 0, 0, 0)));	// 11 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 11, 0, 0, 0, 0)));	// 12 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 12, 0, 0, 0, 0)));	// 13 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 18, 0, 0, 0, 0)));	// 19 March
			}
			break;
				
		case EGetInstancesParentAnd1ChildException:
		// Child specifies a modification to a date using RecId and Start/End date
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentAnd1ChildException, Parent and 1 Child exception\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;		
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			CreateChildExceptionGsEntryL(testData, 4, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			
			// expected times:
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 5, 0, 0, 0, 0)));		//  6 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 6, 0, 0, 0, 0)));		//  7 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 7, 0, 0, 0, 0)));		//  8 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 9, 0, 0, 0, 0)));		// 10 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 10, 0, 0, 0, 0)));	// 11 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 11, 0, 0, 0, 0)));	// 12 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 18, 0, 0, 0, 0)));	// 19 March
			}
			break;
		
		case EGetInstancesParentAnd1ChildRule:
		// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentAnd1ChildRule, Parent weekly RRule and Child with RRule (range=This+Future)\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;		
			if (aFloating)
				{
				CreateParentGsEntryL(GetWeeklyTestData1FloatingL(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 2, CalCommon::EThisAndFuture, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateParentGsEntryL(GetWeeklyTestData1L(*testData), aEntryView, KGUID1());
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 2, CalCommon::EThisAndFuture, aEntryView, KGUID1());
				}
			
			// expected times:
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0)));		//  9 March
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
			}
			break;
			
		case EGetInstancesParentAnd1ChildRuleMultiFetch:
		// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentAnd1ChildRuleMultiFetch, Parent weekly RRule and Child with RRule (range=This+Future)\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;		
			if (aFloating)
				{
				CreateParentGsEntryL(GetWeeklyTestData1FloatingL(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 2, CalCommon::EThisAndFuture, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateParentGsEntryL(GetWeeklyTestData1L(*testData), aEntryView, KGUID1());
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 2, CalCommon::EThisAndFuture, aEntryView, KGUID1());
				}
			
			DoMultipleFetchAndStoreTestRuleChildL(testData, aEntryView, KGUID1(), 2);
			 
			// expected times:
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0)));		//  9 March
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
			}
			break;
		
		case ETestGetSetMethods:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iTestType = TTestDataStruct::EEntryTest;
			testData->iDescription.Copy(_L("Entry Test: ETestGetSetMethods\n"));
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			CreateChildGsEntryL(testData, 4, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				TestSetGetMethodsFloatingL(aEntryView,KGUID1());
				}
			else
				{
				TestSetGetMethodsL(aEntryView,KGUID1());
				}
			}
			break;
			
		case ETestUpdate:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iTestType = TTestDataStruct::EEntryTest;
			testData->iDescription.Copy(_L("Entry Test: ETestUpdate\n"));
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			CreateChildGsEntryL(testData, 7, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			TRAP_IGNORE(TestUpdateL(aEntryView,KGUID1()));
			}
			break;
	
		case EGetInstancesParentWithExceptionsAnd1ChildRuleThisAndFuture:
			{
			// Parent has rpt rule plus exceptions.
			// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef.	
			testData->iGuidsToExtract.Reset();
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithExceptionsAnd1ChildRuleThisAndFuture, Parent,weekly rpt rule plus some exception and 1 Child with RRule (This+Future)\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithExceptionsRdatesL(*testData, aEntryView, KGUID1(), EException, CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 2, CalCommon::EThisAndFuture, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 2, CalCommon::EThisAndFuture, aEntryView, KGUID1());
				}
			
			// Expected Instances if no parameters are changed: 8 April, 15, 22
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
			}
			break;
			
		case EGetInstancesParentWithExceptionsRDatesAnd1ChildRuleThisAndFuture:
			{
			// Parent has rpt rule plus exceptions and sporadic rdates.
			// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef.	
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithExceptionsRDatesAnd1ChildRuleThisAndFuture, Parent,weekly rpt rule, some exceptions & rdates and 1 Child with RRule (This+Future)\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithExceptionsRdatesL(*testData, aEntryView, KGUID1(), EExceptionAndRDate, CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 5, CalCommon::EThisAndFuture, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 5, CalCommon::EThisAndFuture, aEntryView, KGUID1());
				}
			
			// Expected Instances if no parameters are changed: 16March, 17, 24, 8April, 15, 22
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 15, 0, 0, 0, 0)));	// 16 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 16, 0, 0, 0, 0)));	// 17 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 23, 0, 0, 0, 0)));	// 24 March
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
			}
			break;
			
		case EGetInstancesParentWithExceptionsRDatesNonRptChildsAnd1ChildRuleThisAndFuture:
			{
			// Parent has rpt rule plus exceptions, sporadic rdates and non-repeating child entry.
			// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef.	
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithExceptionsRDatesNonRptChildsAnd1ChildRuleThisAndFuture, Parent,weekly rpt rule, some exceptions & rdates and 1 Child with RRule (This+Future)\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithExceptionsRdatesL(*testData, aEntryView, KGUID1(), EExceptionAndRDate, CCalEntry::EAppt, aFloating);
			CreateChildExceptionGsEntryL(testData, 2, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 5, CalCommon::EThisAndFuture, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 5, CalCommon::EThisAndFuture, aEntryView, KGUID1());
				}
			
			// Expected Instances if no parameters are changed: 17March, 19, 24, 8April, 15, 22
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 16, 0, 0, 0, 0)));	// 17 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 18, 0, 0, 0, 0)));	// 19 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 23, 0, 0, 0, 0)));	// 24 March
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
			}
			break;
			
		case EGetInstancesParentWithExceptionsAnd1ChildRuleThisAndPrior:
			{
			// Parent has rpt rule plus exceptions.
			// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef.	
			testData->iGuidsToExtract.Reset();
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithExceptionsAnd1ChildRuleThisAndPrior, Parent,weekly rpt rule plus some exception and 1 Child with RRule (This+Prior)\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithExceptionsRdatesL(*testData, aEntryView, KGUID1(), EException, CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 2, CalCommon::EThisAndPrior, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 2, CalCommon::EThisAndPrior, aEntryView, KGUID1());
				}
			
			// Expected Instances if no parameters are changed: 6April, 8, 13, 15, 20, 22
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 5, 0, 0, 0, 0)));		//  6 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0)));	// 13 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 19, 0, 0, 0, 0)));	// 20 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
			}
			break;
			
		case EGetInstancesParentWithExceptionsRDatesAnd1ChildRuleThisAndPrior:
			{
			// Parent has rpt rule plus exceptions and sporadic rdates.
			// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef.	
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithExceptionsRDatesAnd1ChildRuleThisAndPrior, Parent,weekly rpt rule, some exceptions & rdates and 1 Child with RRule (This+Prior)\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithExceptionsRdatesL(*testData, aEntryView, KGUID1(), EExceptionAndRDate, CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 5, CalCommon::EThisAndPrior, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 5, CalCommon::EThisAndPrior, aEntryView, KGUID1());
				}

			// Expected Instances if no parameters are changed: 8April, 13, 15, 20, 22
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0)));	// 13 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 19, 0, 0, 0, 0)));	// 20 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 23, 0, 0, 0, 0)));	// 24 April
			}
			break;
			
		case EGetInstancesParentWithExceptionsRDatesNonRptChildsAnd1ChildRuleThisAndPrior:
			{
			// Parent has rpt rule plus exceptions, sporadic rdates and non-repeating child entry.
			// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef.	
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithExceptionsRDatesNonRptChildsAnd1ChildRuleThisAndPrior, Parent,weekly rpt rule, some exceptions & rdates and 1 Child with RRule (This+Prior)\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithExceptionsRdatesL(*testData, aEntryView, KGUID1(), EExceptionAndRDate, CCalEntry::EAppt, aFloating);
			CreateChildExceptionGsEntryL(testData, 2, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			CreateChildExceptionGsEntry_1L(testData, 7, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 5, CalCommon::EThisAndPrior, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 5, CalCommon::EThisAndPrior, aEntryView, KGUID1());
				}
			
			// Expected Instances if no parameters are changed: 8April, 13, 15, 18, 22
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0)));	// 13 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 17, 0, 0, 0, 0)));	// 18 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 23, 0, 0, 0, 0)));	// 24 April
			}
			break;	
			
		/* 
		 * This tests adding a second child-rule entry to a parent.
		 * Adding a second child-rule entry is dis-allowed and causes a Leave.
		 * However this test is commented out as the TRAPD below was for some reason not catching the Leave.
		 */
		case EGetInstancesParentWithExceptionsAnd2ChildRuleThisAndFuture:
			{
			// Parent has rpt rule plus exceptions.
			// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef.	
			testData->iGuidsToExtract.Reset();
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithExceptionsAnd2ChildRuleThisAndFuture, Parent,weekly rpt rule plus some exception and 2 Child with RRule\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithExceptionsRdatesL(*testData, aEntryView, KGUID1(), EException, CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 2, CalCommon::EThisAndPrior, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
				}
			else
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 2, CalCommon::EThisAndPrior, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);	
				}


			// CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 4, CalCommon::EThisAndFuture, aEntryView, KGUID1());
			TCalRRule* rule = NULL;
			if (aFloating)
				{
				rule = GetWeeklyTestDataModification1FloatingL(*testData);
				}
			else
				{
				rule = GetWeeklyTestDataModification1L(*testData);
				}
			TRAP_IGNORE(
				CreateChildRRuleGsEntryL(rule, testData, 4, CalCommon::EThisAndFuture, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
				);

			// Expected Instances if no parameters are changed: 6April, 8, 13, 15, 20, 22
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 5, 0, 0, 0, 0)));		//  6 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 7, 0, 0, 0, 0)));		//  8 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0)));	// 13 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 14, 0, 0, 0, 0)));	// 15 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 19, 0, 0, 0, 0)));	// 20 April
			testData->iTimes.Append(TTime(TDateTime(2005, EApril, 21, 0, 0, 0, 0)));	// 22 April
	
			}
			break;

		case EGetInstancesParentWithRDateBeforeRRule:
			{
			testData->iGuidsToExtract.Reset();
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithRDateBeforeRRule\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithRDateRRuleL(*testData, aEntryView, KGUID1(), ERDateBeforeRRule, CCalEntry::EAppt, aFloating);
			
			// Expected Instances
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EDecember, 11, 9, 0, 0, 0)));		// 12 Dec
			testData->iTimes.Append(TTime(TDateTime(2005, EDecember, 13, 10, 0, 0, 0)));		// 14 Dec
			testData->iTimes.Append(TTime(TDateTime(2005, EDecember, 14, 10, 0, 0, 0)));		// 15 Dec
			testData->iTimes.Append(TTime(TDateTime(2005, EDecember, 15, 10, 0, 0, 0)));		// 16 Dec
			}
			break;

		case EGetInstancesParentWithRDateAfterRRule:
			{
			testData->iGuidsToExtract.Reset();
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentWithRDateAfterRRule\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithRDateRRuleL(*testData, aEntryView, KGUID1(), ERDateAfterRRule, CCalEntry::EAppt, aFloating);
			
			// Expected Instances
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EDecember, 13, 10, 0, 0, 0)));		// 14 Dec
			testData->iTimes.Append(TTime(TDateTime(2005, EDecember, 14, 10, 0, 0, 0)));		// 15 Dec
			testData->iTimes.Append(TTime(TDateTime(2005, EDecember, 15, 10, 0, 0, 0)));		// 16 Dec
			testData->iTimes.Append(TTime(TDateTime(2005, EDecember, 19, 9, 0, 0, 0)));		// 20 Dec
			}
			break;
			
		case EDeleteAllChildExceptions:
			{
			// Parent has rpt rule plus exceptions, sporadic rdates and non-repeating child entry.
			// Child specifies a change to parent's repeat rule by defining a RecId and new RptDef.	
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: EDeleteAllChildExceptions, Delete all child exceptions and see what happens\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetWeeklyTestData2L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			CreateChildExceptionGsEntryL(testData, 1, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			CreateChildExceptionGsEntryL(testData, 2, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			TestDeletionOfChildExceptionsL(aEntryView, aInstanceView, KGUID1());
			}
			break;
			
		case ECreateParentInvalidChildFloating: 
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: ECreateParentInvalidChildFloating\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
			TRAPD(err, CreateChildGsEntryL(testData, 4, aEntryView, KGUID1()));
			test(err == KErrArgument);
			}
			break;
			
		case ECreateParentInvalidChildFloating2:
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: ECreateParentInvalidChildFloating2\n"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1());
			TRAPD(err, CreateChildGsEntryL(testData, 4, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue));
			test(err == KErrArgument);
			}
			break;
			
		case EGetInstancesParentAnd1ChildExceptionMultiFetch:
			// Child specifies a modification to a date using RecId and Start/End date
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentAnd1ChildExceptionMultiFetch, Parent and 1 Child exception\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;		
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			CreateChildExceptionGsEntryL(testData, 4, aEntryView, KGUID1(), CCalEntry::EAppt, aFloating);
			
			DoMultipleFetchAndStoreTestExceptionChildL(testData, aEntryView, KGUID1(), 4);
			
			// expected times:
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 5, 0, 0, 0, 0)));		//  6 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 6, 0, 0, 0, 0)));		//  7 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 7, 0, 0, 0, 0)));		//  8 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 9, 0, 0, 0, 0)));		// 10 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 10, 0, 0, 0, 0)));	// 11 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 11, 0, 0, 0, 0)));	// 12 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 18, 0, 0, 0, 0)));	// 19 March
			}
			break;

		case EDeleteEntryThatHasNoInstances:
			{
			testData->iTestType = TTestDataStruct::EInstanceTest;
			testData->iDescription.Copy(_L("Instance Test: EDeleteEntryThatHasNoInstances\n"));
			testData->iGuidsToExtract.AppendL(KGUID1());	
			TestDeletionOfEntryThatHasNoInstancesL(GetDailyTestData1L(*testData), *testData, aEntryView, aInstanceView,
												   KGUID1(), CCalEntry::EAppt, aFloating);
			}
			break;
			
		case EStoreRestoreParentWithAChild:
			{
			testData->iGuidsToExtract.Reset();
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EStoreRestoreParentWithAChild, Update parent who had one child\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;
			CreateWeeklyParentGsEntryWithExceptionsRdatesL(*testData, aEntryView, KGUID1(), EException, CCalEntry::EAppt, aFloating);
			if (aFloating)
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1FloatingL(*testData), testData, 2, CalCommon::EThisAndFuture, aEntryView, KGUID1(), CCalEntry::EAppt, ETrue);
				}
			else
				{
				CreateChildRRuleGsEntryL(GetWeeklyTestDataModification1L(*testData), testData, 2, CalCommon::EThisAndFuture, aEntryView, KGUID1());
				}
			
			StoreRestoreL(aEntryView, KGUID1());
			
			// Expected Instances - none. 
			// because child entry was deleted when parent was updated and only child held actual instances
			testData->iTimes.Reset();
			}
			break;
			
		case EGetInstancesParentAnd1ChildExceptionModify:
		// Child specifies a modification to a date using RecId and Start/End date
		// Try to modify this child entry, by getting it from the instance.
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Instance Test: EGetInstancesParentAnd1ChildExceptionModify, Parent and 1 Child exception modify\n"));
			testData->iTestType = TTestDataStruct::EInstanceTest;		
			CreateParentGsEntryL(GetDailyTestData1L(*testData), aEntryView, KGUID1(), CCalEntry::EAppt);
			
			// here 4 means array offset ie 5+4 = 9 march
			CreateChildExceptionGsEntryL(testData, 4, aEntryView, KGUID1(), CCalEntry::EAppt);
			TestEntryFromInstanceAndModifyL(testData, aEntryView, aInstanceView, KGUID1(), 4);
			
			// expected times:
			testData->iTimes.Reset();
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 5, 0, 0, 0, 0)));		//  6 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 6, 0, 0, 0, 0)));		//  7 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 7, 0, 0, 0, 0)));		//  8 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 9, 0, 0, 0, 0)));		// 10 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 10, 0, 0, 0, 0)));	// 11 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 11, 0, 0, 0, 0)));	// 12 March
			testData->iTimes.Append(TTime(TDateTime(2005, EMarch, 21, 0, 0, 0, 0)));	// 19 March
			}
			break;
		case EStoreParentHaveNoInstanceAndChildren:	
			{
			testData->iGuidsToExtract.AppendL(KGUID1());
			testData->iDescription.Copy(_L("Entry Test: EStoreParentHaveNoInstanceAndChildren"));
			testData->iTestType = TTestDataStruct::EEntryTest;
			StoreParentHaveNoInstanceAndChildrenL(aEntryView, KGUID1());
			}
			break;

		default:
			ASSERT(0);
			break;
		}
		
	return testData;
	}

void CTestData::StoreParentHaveNoInstanceAndChildrenL(CCalEntryView* aEntryView, const TDesC8& aGuid)
    {// Add an entry
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

//create an simple entry and store it to the file
	TBuf<50> summary;
	
	HBufC8* guid = aGuid.AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	entry->SetSummaryL(_L( "Parent Entry" ));

    TTime startTime = TDateTime( 2006, EMay, 24, 13, 0, 0, 0 );
    TTime endTime = TDateTime( 2006, EMay, 24, 14, 0, 0, 0 );

	TCalTime calStartTime;
	calStartTime.SetTimeUtcL(startTime);
	TCalTime calEndTime;
	calEndTime.SetTimeUtcL(endTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);

	TCalRRule rptRule;
    
    RArray<TDay> daysArray;
    CleanupClosePushL( daysArray );
    daysArray.AppendL( calStartTime.TimeLocalL().DayNoInWeek() );
    
    TCalTime untilCalTime;
   	TTime untilTime = TDateTime( 2006, EJune, 0, 14, 0, 0, 0 );
    untilCalTime.SetTimeUtcL( untilTime );
    
    rptRule.SetDtStart( calStartTime );
    rptRule.SetType( TCalRRule::EWeekly );
    rptRule.SetByDay(  daysArray );
    rptRule.SetUntil( untilCalTime );
    rptRule.SetInterval( 1 );
	
	CleanupStack::PopAndDestroy( &daysArray );
	
	entry->SetRRuleL(rptRule);
   
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	
	TInt entriesStored = 0;
	aEntryView->StoreL(entries, entriesStored);
	TInt localuid = entries[0]->LocalUidL();
	test(entriesStored ==1);
	entries.ResetAndDestroy();

//update the first instance
    guid = aGuid.AllocLC();
    entry = CCalEntry::NewL( CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0, calStartTime, CalCommon::EThisOnly );
    CleanupStack::Pop( guid );
    
	entries.AppendL( entry );
	
    entry->SetStartAndEndTimeL( calStartTime, calEndTime );
	entry->SetSummaryL( _L("Repeating Entry") );
	entry->SetLocationL( _L( "Prague" ) );
	
    aEntryView->StoreL(entries, entriesStored);
    localuid = entries[0]->LocalUidL();
	entries.ResetAndDestroy();
	
//update the sencond instance

    TTime recTime;
    TCalTime startCalTimeChanged; 
    TCalTime endCalTimeChanged;
    TCalTime recCalTime;
    startTime = calStartTime.TimeUtcL();
    recTime = startTime + TTimeIntervalDays( 7 );
    recCalTime.SetTimeUtcL( recTime );
    
    guid = aGuid.AllocLC();
    entry = CCalEntry::NewL( CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0, recCalTime, CalCommon::EThisOnly );
    CleanupStack::Pop( guid );
    
	entries.AppendL( entry );
	
	startTime = recTime + TTimeIntervalHours( 1 );
	endTime = startTime + TTimeIntervalHours( 1 );
	
	startCalTimeChanged.SetTimeUtcL( startTime );
	endCalTimeChanged.SetTimeUtcL( endTime );
	
    entry->SetStartAndEndTimeL( startCalTimeChanged, endCalTimeChanged );
	entry->SetSummaryL( _L("Repeating Entry") );
	entry->SetLocationL( _L( "Dallas" ) );
	
    aEntryView->StoreL(entries, entriesStored);
    localuid = entries[0]->LocalUidL();
    entries.ResetAndDestroy();
   
//Fetch the entry
    aEntryView->FetchL(aGuid, entries);
   
//store again
	aEntryView->StoreL(entries, entriesStored);
   
  	CleanupStack::PopAndDestroy(&entries);
	}

// Sub-routine that tests fetching and storing of a parent and child multiple times making sure the child's recId is correct.
// This should only be called for a Parent and a single non-repeating child.
void CTestData::DoMultipleFetchAndStoreTestExceptionChildL(TTestDataStruct* aTestData, CCalEntryView* aEntryView, const TDesC8& aGuid, TInt aRecIdToUse)
	{
	--aRecIdToUse;
	
	// Fetch both parent and child which have been stored
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	HBufC8* guid = aGuid.AllocLC();
	aEntryView->FetchL(*guid, entryArray);
	test(entryArray.Count() == 2);
	// Check RecId of child entry
	TCalTime recId = entryArray[1]->RecurrenceIdL();
	test(recId.TimeLocalL() == aTestData->iRecIds[aRecIdToUse]);
	
	// Fetch only the child	(via its local id)
	TCalLocalUid luid = entryArray[1]->LocalUidL();
	CCalEntry* child = aEntryView->FetchL(luid);
	CleanupStack::PushL(child);
	TCalTime recId2 = child->RecurrenceIdL();
	test(recId2.TimeLocalL() == aTestData->iRecIds[aRecIdToUse]);
	test(child->UidL() == aGuid);
	
	// Edit child in some way
	child->SetLastModifiedDateL();
	
	// Store only the child
	RPointerArray<CCalEntry> arrayCh;
	CleanupClosePushL(arrayCh);
	arrayCh.Append(child);
	TInt successCh(0);
	aEntryView->StoreL(arrayCh, successCh);
	test(successCh == 1);
	CleanupStack::Pop(&arrayCh);
	arrayCh.Reset();
	
	CleanupStack::PopAndDestroy(child);

	// Fetch both parent & child again
	RPointerArray<CCalEntry> entryArray2;
	CleanupResetAndDestroyPushL(entryArray2);
	aEntryView->FetchL(*guid, entryArray2);
	test(entryArray2.Count() == 2);
	// Test RecId of Child
	recId = entryArray2[1]->RecurrenceIdL();
	test(recId.TimeLocalL() == aTestData->iRecIds[aRecIdToUse]);
		
	// Store child only
	RPointerArray<CCalEntry> arrayCh2;
	CleanupClosePushL(arrayCh2);
	arrayCh2.Append(entryArray2[1]);
	TInt successCh2(0);
	aEntryView->StoreL(arrayCh2, successCh2);
	test(successCh2 == 1);

	// Fetch only the child	(via its local id)
	luid = arrayCh2[0]->LocalUidL();
	CCalEntry* child2 = aEntryView->FetchL(luid);
	CleanupStack::PushL(child2);
	recId2 = child2->RecurrenceIdL();
	test(recId2.TimeLocalL() == aTestData->iRecIds[aRecIdToUse]);	
	
	// Store both the parent & child again
	RPointerArray<CCalEntry> arrayS;
	CleanupClosePushL(arrayS);
	arrayS.Append(entryArray2[0]);
	arrayS.Append(child2);
	TInt success(0);
	aEntryView->StoreL(arrayS, success);
	CleanupStack::Pop(&arrayS);
	arrayS.Reset();	

	CleanupStack::PopAndDestroy(child2);
	CleanupStack::Pop(&arrayCh2);
	arrayCh2.Reset();
	CleanupStack::Pop(&entryArray2);
	entryArray2.ResetAndDestroy();
	
	// Fetch both parent & child again
	RPointerArray<CCalEntry> entryArray3;
	CleanupResetAndDestroyPushL(entryArray3);
	aEntryView->FetchL(*guid, entryArray3);
	test(entryArray3.Count() == 2);
	// Test RecId of Child
	recId = entryArray3[1]->RecurrenceIdL();
	test(recId.TimeLocalL() == aTestData->iRecIds[aRecIdToUse]);
	CleanupStack::Pop(&entryArray3);
	
	// Finally store both again
	success = 0;
	aEntryView->StoreL(entryArray3, success);
	
	entryArray3.ResetAndDestroy();
		
	CleanupStack::PopAndDestroy(guid);
	CleanupStack::Pop(&entryArray);
	entryArray.ResetAndDestroy();
	}


// Sub-routine that tests fetching of a parent and child making sure the child's recId is correct.
// This should only be called for a Parent and a single repeating child.
// Note we can not re-store the fetched child object as it will not contain its recId range value.
void CTestData::DoMultipleFetchAndStoreTestRuleChildL(TTestDataStruct* aTestData, CCalEntryView* aEntryView, const TDesC8& aGuid, TInt aRecIdToUse)
	{
	--aRecIdToUse;
	
	// Fetch both parent and child which have been stored
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	HBufC8* guid = aGuid.AllocLC();
	aEntryView->FetchL(*guid, entryArray);
	test(entryArray.Count() == 2);
	// Check RecId of child entry
	TCalTime recId = entryArray[1]->RecurrenceIdL();
	test(recId.TimeLocalL() == aTestData->iRecIds[aRecIdToUse]);
	
	// Fetch only the child	(via its local id)
	TCalLocalUid luid = entryArray[1]->LocalUidL();
	CCalEntry* child = aEntryView->FetchL(luid);
	CleanupStack::PushL(child);
	TCalTime recId2 = child->RecurrenceIdL();
	test(recId2.TimeLocalL() == aTestData->iRecIds[aRecIdToUse]);
	test(child->UidL() == aGuid);
	
	CleanupStack::PopAndDestroy(child);
	CleanupStack::PopAndDestroy(guid);
	CleanupStack::Pop(&entryArray);
	entryArray.ResetAndDestroy();
	}

// Fetch a child using the instance method, and then modify the same.	
void CTestData::TestEntryFromInstanceAndModifyL(TTestDataStruct* aTestData, CCalEntryView* aEntryView, CCalInstanceView* aInstanceView, const TDesC8& aGuid, TInt aRecIdToUse)
	{
		--aRecIdToUse;
	
		// The exception child was created on 19 march.
		TTime filterStartTime(TDateTime(2005, EMarch, 18, 0, 0, 0, 0));
		TTime filterEndTime(TDateTime(2005, EMarch, 18, 23, 0, 0, 0));
		TCalTime filterStartTimeCal;
		TCalTime filterEndTimeCal;
		filterStartTimeCal.SetTimeLocalL(filterStartTime);
		filterEndTimeCal.SetTimeLocalL(filterEndTime);
	
		// Fetch the instance 
		RPointerArray< CCalInstance > instanceList;
		CalCommon::TCalTimeRange aTimeRange(filterStartTimeCal,filterEndTimeCal) ;
		aInstanceView->FindInstanceL( instanceList, CalCommon::EIncludeAppts, aTimeRange);
		test(1 == instanceList.Count());
		
		// Get the recurrance id from the instance.
		CCalEntry & entryFromIns = instanceList[0]->Entry();
		TCalTime recId = entryFromIns.RecurrenceIdL();
		test(recId.TimeLocalL() == aTestData->iRecIds[aRecIdToUse]);
		instanceList.ResetAndDestroy();
		
		// Using this rec id obtained from instanceL, create a new modifying entry 
		HBufC8* guid = aGuid.AllocLC();
		CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 2, recId, CalCommon::EThisOnly);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(entry);
		TTime secondModStartTime(TDateTime(2005, EMarch, 21, 0, 0, 0, 0)); 
		TTime secondModEndTime(secondModStartTime + TTimeIntervalHours(1));
		TCalTime startTimeCal;
		startTimeCal.SetTimeLocalL(secondModStartTime);
		TCalTime endTimeCal;
		endTimeCal.SetTimeLocalL(secondModEndTime);
		entry->SetStartAndEndTimeL(startTimeCal, endTimeCal);
	
		RPointerArray<CCalEntry> entries;
		CleanupClosePushL(entries);
		entries.AppendL(entry);
		TInt success(0);
		aEntryView->StoreL(entries, success);
		test(success == entries.Count());
		CleanupStack::Pop(&entries);
		entries.Reset();
		CleanupStack::PopAndDestroy(entry);
	}


CCalEntry* CTestData::CreateParentLC(TCalRRule* aRpt, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{

	HBufC8* guid = aGuid.AllocLC();  // Create GUID on heap

	// Below takes ownership of guid
	CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodRequest, (TUint)0);
	
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime startTime;
	TCalTime calEndTime;
	if (aFloating)
		{
		startTime.SetTimeLocalFloatingL(aRpt->DtStart().TimeLocalL());
		
		// Set the start time to be same as repeat def's start time
		// and the end time to be 1 hour after start time.
		TTime endTime = aRpt->DtStart().TimeLocalL() + TTimeIntervalHours(1);
		calEndTime.SetTimeLocalFloatingL(endTime);
		}
	else
		{
		startTime = aRpt->DtStart();

		// Set the start time to be same as repeat def's start time
		// and the end time to be 1 hour after start time.
		TTime endTime = aRpt->DtStart().TimeUtcL() + TTimeIntervalHours(1);
		calEndTime.SetTimeUtcL(endTime);
		}
	entry->SetStartAndEndTimeL(startTime, calEndTime);
	entry->SetRRuleL(*aRpt); // doesn't take ownership
	
	return entry;
	}


void CTestData::CreateParentGsEntryL(TCalRRule* aRpt, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{
	CleanupStack::PushL(aRpt);
	
	CCalEntry* entry = CreateParentLC(aRpt, aGuid, aType, aFloating);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::Pop(&array);
	array.Reset();
	
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(aRpt);
	}


// Creates a parent entry with a weekly rpt rule (GetWeeklyTestData1L())
// and adds exceptions and/or rdates to the parent entry. 
void CTestData::CreateWeeklyParentGsEntryWithExceptionsRdatesL(TTestDataStruct& aTestData, CCalEntryView* aEntryView, const TDesC8& aGuid, TExceptionRDate aExcpnRDate, CCalEntry::TType aType, TBool aFloating)
	{
	TCalRRule* rptRule;
	if (aFloating)
		{
		rptRule = GetWeeklyTestData1FloatingL(aTestData);
		}
	else
		{
		rptRule = GetWeeklyTestData1L(aTestData);
		}
	CleanupStack::PushL(rptRule);
	
	CCalEntry* entry = CreateParentLC(rptRule, aGuid, aType, aFloating);
	
	RArray<TCalTime> exArray;
	RArray<TCalTime> rDateArray;
	
	// Add exceptions or rdates to parent rpt rule

	if ((aExcpnRDate == EException) || (aExcpnRDate == EExceptionAndRDate))
		{ 
		// Set 3 exceptions on the parent rpt rule (GetWeeklyTestData1L()) of 6,13,20 April 2005
		TCalTime exTime1;
		TCalTime exTime2;
		TCalTime exTime3;
		if (aFloating)
			{
			exTime1.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 8, 0, 0, 0, 0));  // 9 March
			exTime2.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 22, 0, 0, 0, 0)); // 23 March
			exTime3.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 29, 0, 0, 0, 0)); // 30 March
			}
		else
			{
			exTime1.SetTimeLocalL(TDateTime(2005, EMarch, 8, 0, 0, 0, 0));  // 9 March
			exTime2.SetTimeLocalL(TDateTime(2005, EMarch, 22, 0, 0, 0, 0)); // 23 March
			exTime3.SetTimeLocalL(TDateTime(2005, EMarch, 29, 0, 0, 0, 0)); // 30 March
			}
		exArray.Append(exTime1);
		exArray.Append(exTime2);
		exArray.Append(exTime3);
		
		entry->SetExceptionDatesL(exArray);
		}

	if ((aExcpnRDate == ERDate) || (aExcpnRDate == EExceptionAndRDate))
		{
		// Set 3 sporadic dates on the parent (3, 24 March & 24 April 2005)
	
		TCalTime rDate1;
		TCalTime rDate2;
		TCalTime rDate3;

		if (aFloating)
			{
			rDate1.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 16, 0, 0, 0, 0));  // 17 March
			rDate2.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 23, 0, 0, 0, 0)); // 24 March
			rDate3.SetTimeLocalFloatingL(TDateTime(2005, EApril, 23, 0, 0, 0, 0)); // 24 April
			}
		else
			{
			rDate1.SetTimeLocalL(TDateTime(2005, EMarch, 16, 0, 0, 0, 0));  // 17 March
			rDate2.SetTimeLocalL(TDateTime(2005, EMarch, 23, 0, 0, 0, 0)); // 24 March
			rDate3.SetTimeLocalL(TDateTime(2005, EApril, 23, 0, 0, 0, 0)); // 24 April
			}

		rDateArray.Append(rDate1);
		rDateArray.Append(rDate2);
		rDateArray.Append(rDate3);
		
	
		entry->SetRDatesL(rDateArray);			
		}
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);		
	CleanupStack::Pop(&array);
	array.Reset();
	
	rDateArray.Reset();
	exArray.Reset();
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(rptRule);
	}

		
TCalRRule* CTestData::GetDailyTestData1L(TTestDataStruct& aData,TBool aSummerTime)
	{
	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EDaily);
	rpt->SetInterval(1);
	TCalTime startTime;
    if (!aSummerTime)
        {
        startTime.SetTimeLocalL(TDateTime(2005, EMarch, 5, 0, 0, 0, 0));
        }
    else
        {
        startTime.SetTimeLocalFloatingL(TDateTime(2005, EAugust, 5, 0, 0, 0, 0));
        }
       
	rpt->SetDtStart(startTime); 	// 6 March 2005
	TCalTime endTime;
    if (!aSummerTime)
        {
        endTime.SetTimeLocalL(TDateTime(2005, EMarch, 11, 0, 0, 0, 0));
        }
    else
        {
        endTime.SetTimeLocalFloatingL(TDateTime(2005, EAugust, 11, 0, 0, 0, 0));       
        }
	rpt->SetUntil(endTime);			// 12 March 2005
	
    if (!aSummerTime)
        {
        aData.iStartDate = TDateTime(2005, EMarch, 0, 0, 0, 0, 0);        //  1 March 2005
        aData.iEndDate = TDateTime(2005, EMarch, 30, 0, 0, 0, 0);         // 31 March 2005
        aData.iDescription.Append(_L(" Every 1 days from 6 March 05\n"));

        // Set up the RecId's (used for Child entries)
        aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 5, 0, 0, 0, 0)));     //  6 Mar
        aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 6, 0, 0, 0, 0)));     //  7 Mar
        aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 7, 0, 0, 0, 0)));     //  8 Mar
        aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0)));     //  9 Mar
        aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 9, 0, 0, 0, 0)));     // 10 Mar
        aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 10, 0, 0, 0, 0)));     // 11 Mar
        aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 11, 0, 0, 0, 0)));     // 12 Mar
        }
    else
        {
        aData.iStartDate = TDateTime(2005, EAugust, 0, 0, 0, 0, 0);        //  1 August 2005
        aData.iEndDate = TDateTime(2005, EAugust, 30, 0, 0, 0, 0);         // 31 August 2005
        aData.iDescription.Append(_L(" Every 1 days from 6 August 05\n"));

		// Set up the RecId's (used for Child entries)
        aData.iRecIds.Append(TTime(TDateTime(2005, EAugust, 5, 0, 0, 0, 0)));     //  6 Aug
        aData.iRecIds.Append(TTime(TDateTime(2005, EAugust, 6, 0, 0, 0, 0)));     //  7 Aug
        aData.iRecIds.Append(TTime(TDateTime(2005, EAugust, 7, 0, 0, 0, 0)));     //  8 Aug
        aData.iRecIds.Append(TTime(TDateTime(2005, EAugust, 8, 0, 0, 0, 0)));     //  9 Aug
        aData.iRecIds.Append(TTime(TDateTime(2005, EAugust, 9, 0, 0, 0, 0)));     // 10 Aug
        aData.iRecIds.Append(TTime(TDateTime(2005, EAugust, 10, 0, 0, 0, 0)));     // 11 Aug
        aData.iRecIds.Append(TTime(TDateTime(2005, EAugust, 11, 0, 0, 0, 0)));     // 12 Aug
        }
	return rpt;
	}
	
	
TCalRRule* CTestData::GetWeeklyTestData1L(TTestDataStruct& aData)
	{
	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EWeekly);
	rpt->SetInterval(1);
	RArray<TDay> days;
	days.Append(EWednesday);
	rpt->SetByDay(days);
	days.Reset();
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EMarch, 5, 0, 0, 0, 0));  // 6 March
	rpt->SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2005, EApril, 20, 0, 0, 0, 0));	// 21 Apr
	rpt->SetUntil(endTime);	
	
	aData.iStartDate = TDateTime(2005, EMarch, 0, 0, 0, 0, 0);		//  1 March 2005
	aData.iEndDate = TDateTime(2005, EApril, 26, 0, 0, 0, 0); 		// 27 April 2005
	aData.iDescription.Append(_L(" Every Wed from 6 Mar 05 until 21 Apr 05\n"));
	
	// Set up the RecId's (used for Child entries)
	// Clear array first
	aData.iRecIds.Reset();
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0))); 	// 09 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 15, 0, 0, 0, 0))); 	// 16 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 22, 0, 0, 0, 0))); 	// 23 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 28, 23, 0, 0, 0))); 	// 30 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 04, 23, 0, 0, 0))); 	// 06 April
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 11, 23, 0, 0, 0))); 	// 13 April
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 18, 23, 0, 0, 0))); 	// 20 April
	
	return rpt;
	}

TCalRRule* CTestData::GetWeeklyTestData1FloatingL(TTestDataStruct& aData)
	{
	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EWeekly);
	rpt->SetInterval(1);
	RArray<TDay> days;
	days.Append(EWednesday);
	rpt->SetByDay(days);
	days.Reset();
	TCalTime startTime;
	startTime.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 5, 0, 0, 0, 0));  // 6 March
	rpt->SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalFloatingL(TDateTime(2005, EApril, 20, 0, 0, 0, 0));	// 21 Apr
	rpt->SetUntil(endTime);	
	
	aData.iStartDate = TDateTime(2005, EMarch, 0, 0, 0, 0, 0);		//  1 March 2005
	aData.iEndDate = TDateTime(2005, EApril, 26, 0, 0, 0, 0); 		// 27 April 2005
	aData.iDescription.Append(_L(" Every Wed from 6 Mar 05 until 21 Apr 05\n"));
	
	// Set up the RecId's (used for Child entries)
	// Clear array first
	aData.iRecIds.Reset();
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0))); 	// 09 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 15, 0, 0, 0, 0))); 	// 16 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 22, 0, 0, 0, 0))); 	// 23 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 29, 0, 0, 0, 0))); 	// 30 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 05, 0, 0, 0, 0))); 	// 06 April
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0))); 	// 13 April
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 19, 0, 0, 0, 0))); 	// 20 April
	
	return rpt;
	}
	
TCalRRule* CTestData::GetWeeklyTestData2L(TTestDataStruct& aData)
	{
	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EWeekly);
	rpt->SetInterval(1);
	RArray<TDay> days;
	days.Append(EWednesday);
	rpt->SetByDay(days);
	days.Reset();
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EMarch, 8, 0, 0, 0, 0));  // 9 March
	rpt->SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2005, EMarch, 15, 0, 0, 0, 0));	// 16 Apr
	rpt->SetUntil(endTime);	
	
	aData.iStartDate = TDateTime(2005, EMarch, 0, 0, 0, 0, 0);		//  1 March 2005
	aData.iEndDate = TDateTime(2005, EApril, 26, 0, 0, 0, 0); 		// 27 April 2005
	aData.iDescription.Append(_L(" Every Wed from 9 Mar 05 until 16 Mar 05\n"));
	
	// Set up the RecId's (used for Child entries)
	// Clear array first
	aData.iRecIds.Reset();
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0))); 	// 09 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 15, 0, 0, 0, 0))); 	// 16 Mar
	
	return rpt;
	}
	

//	This creates a rule which proposes a change to the rule in GetWeeklyTestData1L()
//	ie Weekly, wed from 6 Mar to 10 Apr 2005.
//	This rrule proposes Weekly, Fri from 11 Mar to Fri 8 April.
TCalRRule* CTestData::GetWeeklyTestDataModification1L(TTestDataStruct& aData)
	{
	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EWeekly);
	rpt->SetInterval(1);
	RArray<TDay> days;
	days.Append(EFriday);
	rpt->SetByDay(days);
	days.Reset();
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2005, EApril, 5, 0, 0, 0, 0));  // 6th April
	rpt->SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2005, EApril, 21, 0, 0, 0, 0));	// 22 Apr
	rpt->SetUntil(endTime);	
	
	aData.iStartDate = TDateTime(2005, EMarch, 0, 0, 0, 0, 0);		//  1 March 2005
	aData.iEndDate = TDateTime(2005, EApril, 26, 0, 0, 0, 0); 		// 27 April 2005
	aData.iDescription.Append(_L(" Modifies a parent rrule. This child's rrule is every Fri from 11 Mar to Fri 22 Apr 05\n"));

	// Set up the RecIds (used for Child entries) - these are the instance dates of the parent (GetWeeklyTestData1L())
	// Note CCalEntry::NewL() expects the RecIds in Utc hence they're values after BST are expressed as localtime - 1hr.
	// Clear array first
	aData.iRecIds.Reset();
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0))); 	// 09 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 15, 0, 0, 0, 0))); 	// 16 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 22, 0, 0, 0, 0))); 	// 23 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 28, 23, 0, 0, 0))); 	// 30 Mar  	BST
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 04, 23, 0, 0, 0))); 	// 06 April BST
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 11, 23, 0, 0, 0))); 	// 13 April BST
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 18, 23, 0, 0, 0))); 	// 20 April BST
	
	return rpt;
	}

//	This function is the floating version of GetWeeklyTestDataModification1L()
TCalRRule* CTestData::GetWeeklyTestDataModification1FloatingL(TTestDataStruct& aData)
	{
	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EWeekly);
	rpt->SetInterval(1);
	RArray<TDay> days;
	days.Append(EFriday);
	rpt->SetByDay(days);
	days.Reset();
	TCalTime startTime;
	startTime.SetTimeLocalFloatingL(TDateTime(2005, EApril, 5, 0, 0, 0, 0));  // 6th April
	rpt->SetDtStart(startTime); 
	TCalTime endTime;
	endTime.SetTimeLocalFloatingL(TDateTime(2005, EApril, 21, 0, 0, 0, 0));	// 22 Apr
	rpt->SetUntil(endTime);	
	
	aData.iStartDate = TDateTime(2005, EMarch, 0, 0, 0, 0, 0);		//  1 March 2005
	aData.iEndDate = TDateTime(2005, EApril, 26, 0, 0, 0, 0); 		// 27 April 2005
	aData.iDescription.Append(_L(" Modifies a parent rrule. This child's rrule is every Fri from 11 Mar to Fri 22 Apr 05\n"));

	// Set up the RecIds (used for Child entries) - these are the instance dates of the parent (GetWeeklyTestData1L())
	// Note CCalEntry::NewL() expects the RecIds in Utc hence they're values after BST are expressed as localtime - 1hr.
	// Clear array first
	aData.iRecIds.Reset();
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 8, 0, 0, 0, 0))); 	// 09 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 15, 0, 0, 0, 0))); 	// 16 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 22, 0, 0, 0, 0))); 	// 23 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EMarch, 29, 0, 0, 0, 0))); 	// 30 Mar
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 05, 0, 0, 0, 0))); 	// 06 April
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 12, 0, 0, 0, 0))); 	// 13 April
	aData.iRecIds.Append(TTime(TDateTime(2005, EApril, 19, 0, 0, 0, 0))); 	// 20 April
	
	return rpt;
	}
	
	
void CTestData::DeleteParentGsEntryL(CCalEntryView* aEntryView, const TDesC8& aGuid)
	{
	CDesC8ArrayFlat* guidArray = new(ELeave)CDesC8ArrayFlat(1);
	CleanupStack::PushL(guidArray);
	guidArray->AppendL(aGuid);
	aEntryView->DeleteL(*guidArray);
	CleanupStack::PopAndDestroy(guidArray);
	}
	
void CTestData::SingleDeleteParentGsEntryL(CCalEntryView* aEntryView, const TDesC8& aGuid)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	HBufC8* guid = aGuid.AllocLC();
	aEntryView->FetchL(*guid, entryArray);
	// Expect only 1 (parent) entry
	if(entryArray.Count())
		{
		CCalEntry* parent = entryArray[0];
		aEntryView->DeleteL(*parent);
		}
	
	CleanupStack::PopAndDestroy(guid);
	CleanupStack::Pop(&entryArray);
	entryArray.ResetAndDestroy();
	}

void CTestData::CreateChildGsEntryL(TTestDataStruct* aTestData, TInt aRecIdToUse, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{

	HBufC8* guid = aGuid.AllocLC();  // Create GUID on heap
	
	// Use Rec-id as given by test-data
	TTime ttime = aTestData->iRecIds[aRecIdToUse-1];
	TCalTime recurrenceId;
	if (aFloating)
		{
		recurrenceId.SetTimeLocalFloatingL(ttime);
		}
	else
		{
		recurrenceId.SetTimeUtcL(ttime);
		}
	CalCommon::TRecurrenceRange range = CalCommon::EThisOnly;	
	
	// Create a child
	// Below takes ownership of guid
	CCalEntry* entry = CCalEntry::NewL(	aType, guid, CCalEntry::EMethodRequest, (TUint)0,
										recurrenceId, range);
	
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	// Start date same as RecId. End time is 1 hour later.
	TTime startDate = aTestData->iRecIds[aRecIdToUse-1];
	TTime endDate   = (startDate + TTimeIntervalHours(1));
	
	TCalTime startTime;
	TCalTime endTime;

	if (aFloating)
		{
		startTime.SetTimeLocalFloatingL(startDate);
		endTime.SetTimeLocalFloatingL(endDate);
		}
	else
		{
		startTime.SetTimeUtcL(startDate);
		endTime.SetTimeUtcL(endDate);
		}
	entry->SetStartAndEndTimeL(startTime, endTime);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	
	// Ensure that storeL has not modified its recurrence Id.
	test(recurrenceId.TimeLocalL() == entry->RecurrenceIdL().TimeLocalL());	

	CleanupStack::Pop(&array);
	array.Reset();
	
	CleanupStack::PopAndDestroy(entry);
	}


void CTestData::AddMultipleEntriesL(TCalRRule* aRpt, CCalEntryView* aEntryView, CCalEntry::TType aType, TBool aFloating)
	{
	
	CleanupStack::PushL(aRpt);
	
	
	CDesC8ArrayFlat* guidArray = new(ELeave) CDesC8ArrayFlat(3);
	CleanupStack::PushL(guidArray);		// CleanupArrayDeletePushL(guidArray);
	guidArray->AppendL(KGUID1());
	guidArray->AppendL(KGUID2());
	guidArray->AppendL(KGUID3());
	
	TCalTime startTime;
	TCalTime calEndTime;
	if (aFloating)
		{
		startTime.SetTimeLocalFloatingL(aRpt->DtStart().TimeLocalL());
		
		TTime endTime = aRpt->DtStart().TimeLocalL() + TTimeIntervalHours(1);
		calEndTime.SetTimeLocalFloatingL(endTime);		
		}
	else
		{
		startTime = aRpt->DtStart();
		TTime endTime = aRpt->DtStart().TimeUtcL() + TTimeIntervalHours(1);
		calEndTime.SetTimeUtcL(endTime);
		}

	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	for(TInt a = 0; a < guidArray->Count(); a++ )
		{
		HBufC8* guid = (*guidArray)[a].AllocLC();  // Create GUID on heap
		
		// Below takes ownership of guid
		CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodRequest, (TUint)0);
		
		CleanupStack::Pop(guid);
		CleanupStack::PushL(entry);
		
		// Set same rpt rule on all entries
		entry->SetStartAndEndTimeL(startTime, calEndTime);
		entry->SetRRuleL(*aRpt); // doesn't take ownership
		
		entryArray.Append(entry);
		CleanupStack::Pop(entry);
		}
	
	TInt success(0);
	aEntryView->StoreL(entryArray, success);
	
	//CleanupStack::PopAndDestroy(&entryArray);
	CleanupStack::Pop(&entryArray);
	entryArray.ResetAndDestroy();
	
	CleanupStack::PopAndDestroy(guidArray);
	CleanupStack::PopAndDestroy(aRpt);

	}

void CTestData::DeleteAllEntriesByGuidL(CCalEntryView* aEntryView)
	{
	
	CDesC8ArrayFlat* guidArray = new(ELeave) CDesC8ArrayFlat(3);
	CleanupArrayDeletePushL(guidArray);
	guidArray->AppendL(KGUID1());
	guidArray->AppendL(KGUID2());
	guidArray->AppendL(KGUID3());

	aEntryView->DeleteL(*guidArray);
	
	CleanupStack::PopAndDestroy(guidArray);
	}
	
void CTestData::TestSetGetMethodsL(CCalEntryView* aEntryView, const TDesC8& aGuid)
	{
	// create test values
	_LIT(KSummary,"VIM");
	_LIT(KDescription,"Very Important Meeting");
	_LIT(KLocation,"London");
	
	TTime tempTime;
	
	TCalTime startTime;
	tempTime.Set(_L("20101123:120000")); //YYYYMMDD:HHMMSS.MMMMMM
	startTime.SetTimeLocalL(tempTime);
	
	TCalTime endTime;
	tempTime.Set(_L("20101123:130000")); //YYYYMMDD:HHMMSS.MMMMMM
	endTime.SetTimeLocalL(tempTime);
	
	//	TBool completed(EFalse);
	
	TCalTime completedTime;
	tempTime.Set(_L("20101123:140000")); //YYYYMMDD:HHMMSS.MMMMMM
	completedTime.SetTimeLocalL(tempTime);
	
	//TCalRRule* calRRule;
	
	tempTime.Set(_L("20101124:120000")); // date1
	TCalTime date1;
	date1.SetTimeLocalL(tempTime);
	tempTime.Set(_L("20101125:120000")); // date2
	TCalTime date2;
	date2.SetTimeLocalL(tempTime);
	
	RArray<TCalTime> rDateList1;
	rDateList1.Append(date1);
	rDateList1.Append(date2);
	
	RArray<TCalTime> rDateList2;
	
	RArray<TCalTime> exDateList;
	exDateList.Append(date2);
		
	CCalEntry::TStatus status(CCalEntry::ETentative);
	CCalEntry::TReplicationStatus replicationStatus(CCalEntry::EOpen);
	
	TUint priority(22);
	
	CCalEntry::TMethod method(CCalEntry::EMethodNone);
	TInt seq(0);
	
	// get an entry out of the calendar file
	RPointerArray<CCalEntry> calEntryArray;
	CleanupResetAndDestroyPushL(calEntryArray);
	aEntryView->FetchL(aGuid,calEntryArray);
	
	// call all the set methods	
	calEntryArray[0]->SetStartAndEndTimeL(startTime, endTime);
	calEntryArray[0]->SetLastModifiedDateL();
	calEntryArray[0]->ClearRepeatingPropertiesL();
	calEntryArray[0]->SetRDatesL(rDateList1);
	calEntryArray[0]->SetExceptionDatesL(exDateList);
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	TTimeIntervalMinutes offset(5);
	alarm->SetTimeOffset(offset);
	calEntryArray[0]->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	calEntryArray[0]->SetStatusL(status);
	calEntryArray[0]->SetReplicationStatusL(replicationStatus);
	calEntryArray[0]->SetPriorityL(priority);
	calEntryArray[0]->SetSummaryL(KSummary());
	calEntryArray[0]->SetDescriptionL(KDescription());
	calEntryArray[0]->SetLocationL(KLocation());
	calEntryArray[0]->SetMethodL(method);
	calEntryArray[0]->SetSequenceNumberL(seq);
	
	// call all the get methods
	// test that they are as expected
	test( startTime.TimeLocalL() == calEntryArray[0]->StartTimeL().TimeLocalL());
	test( endTime.TimeLocalL() == calEntryArray[0]->EndTimeL().TimeLocalL());
	RArray<TCalTime> returnedRDateList;
	calEntryArray[0]->GetRDatesL(returnedRDateList);
	test( date1.TimeLocalL() == returnedRDateList[0].TimeLocalL() );
	test( date2.TimeLocalL() == returnedRDateList[1].TimeLocalL() );
	RArray<TCalTime> returnedEXDateList;
	calEntryArray[0]->GetExceptionDatesL(returnedEXDateList);
	test( date2.TimeLocalL() == returnedEXDateList[0].TimeLocalL() );
	alarm = calEntryArray[0]->AlarmL();
	test( offset == alarm->TimeOffset() );
	delete alarm;
	test( replicationStatus == calEntryArray[0]->ReplicationStatusL() );
	test( priority == calEntryArray[0]->PriorityL());
	test( calEntryArray[0]->CompareL(*calEntryArray[0]) );
	TPtrC temp(calEntryArray[0]->SummaryL());
	test( KSummary().CompareF(calEntryArray[0]->SummaryL()) == 0 );
	test( KDescription().CompareF(calEntryArray[0]->DescriptionL()) == 0); 
	test( KLocation().CompareF(calEntryArray[0]->LocationL()) == 0);
	test( method == calEntryArray[0]->MethodL() );
	test( seq == calEntryArray[0]->SequenceNumberL() );
	test( status == calEntryArray[0]->StatusL() ); // TStatus status(calEntryArray[0]->StatusL());
	
	// Set RDates with an empty list, we should delete the exception date aswell !!
	calEntryArray[0]->SetRDatesL(rDateList2);
	
	rDateList1.Close();
	rDateList2.Close();
	exDateList.Close();
	returnedRDateList.Close();
	returnedEXDateList.Close();
	
	CleanupStack::PopAndDestroy(); // calEntryArray
	
	}

void CTestData::TestSetGetMethodsFloatingL(CCalEntryView* aEntryView, const TDesC8& aGuid)
	{
	// create test values
	_LIT(KSummary,"VIM");
	_LIT(KDescription,"Very Important Meeting");
	_LIT(KLocation,"London");
	
	TTime tempTime;
	
	TCalTime startTime;
	tempTime.Set(_L("20101123:120000")); //YYYYMMDD:HHMMSS.MMMMMM
	startTime.SetTimeLocalFloatingL(tempTime);
	
	TCalTime endTime;
	tempTime.Set(_L("20101123:130000")); //YYYYMMDD:HHMMSS.MMMMMM
	endTime.SetTimeLocalFloatingL(tempTime);
	
	//	TBool completed(EFalse);
	
	TCalTime completedTime;
	tempTime.Set(_L("20101123:140000")); //YYYYMMDD:HHMMSS.MMMMMM
	completedTime.SetTimeLocalFloatingL(tempTime);
	
	tempTime.Set(_L("20101124:120000")); // date1
	TCalTime date1;
	date1.SetTimeLocalFloatingL(tempTime);
	tempTime.Set(_L("20101125:120000")); // date2
	TCalTime date2;
	date2.SetTimeLocalFloatingL(tempTime);
	
	RArray<TCalTime> rDateList1;
	rDateList1.Append(date1);
	rDateList1.Append(date2);
	
	RArray<TCalTime> rDateList2;
	
	RArray<TCalTime> exDateList;
	exDateList.Append(date2);
		
	CCalEntry::TStatus status(CCalEntry::ETentative);
	CCalEntry::TReplicationStatus replicationStatus(CCalEntry::EOpen);
	
	TUint priority(22);
	
	CCalEntry::TMethod method(CCalEntry::EMethodNone);
	TInt seq(0);
	
	// get an entry out of the calendar file
	RPointerArray<CCalEntry> calEntryArray;
	CleanupResetAndDestroyPushL(calEntryArray);
	aEntryView->FetchL(aGuid,calEntryArray);
	
	// call all the set methods	
	calEntryArray[0]->SetStartAndEndTimeL(startTime, endTime);
	calEntryArray[0]->SetLastModifiedDateL();
	calEntryArray[0]->ClearRepeatingPropertiesL();
	calEntryArray[0]->SetRDatesL(rDateList1);
	calEntryArray[0]->SetExceptionDatesL(exDateList);
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	TTimeIntervalMinutes offset(5);
	alarm->SetTimeOffset(offset);
	calEntryArray[0]->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	calEntryArray[0]->SetStatusL(status);
	calEntryArray[0]->SetReplicationStatusL(replicationStatus);
	calEntryArray[0]->SetPriorityL(priority);
	calEntryArray[0]->SetSummaryL(KSummary());
	calEntryArray[0]->SetDescriptionL(KDescription());
	calEntryArray[0]->SetLocationL(KLocation());
	calEntryArray[0]->SetMethodL(method);
	calEntryArray[0]->SetSequenceNumberL(seq);
	
	// call all the get methods
	// test that they are as expected
	test( startTime.TimeLocalL() == calEntryArray[0]->StartTimeL().TimeLocalL());
	test( endTime.TimeLocalL() == calEntryArray[0]->EndTimeL().TimeLocalL());
	RArray<TCalTime> returnedRDateList;
	calEntryArray[0]->GetRDatesL(returnedRDateList);
	test( date1.TimeLocalL() == returnedRDateList[0].TimeLocalL() );
	test( date2.TimeLocalL() == returnedRDateList[1].TimeLocalL() );
	RArray<TCalTime> returnedEXDateList;
	calEntryArray[0]->GetExceptionDatesL(returnedEXDateList);
	test( date2.TimeLocalL() == returnedEXDateList[0].TimeLocalL() );
	alarm = calEntryArray[0]->AlarmL();
	test( offset == alarm->TimeOffset() );
	delete alarm;
	test( replicationStatus == calEntryArray[0]->ReplicationStatusL() );
	test( priority == calEntryArray[0]->PriorityL());
	test( calEntryArray[0]->CompareL(*calEntryArray[0]) );
	TPtrC temp(calEntryArray[0]->SummaryL());
	test( KSummary().CompareF(calEntryArray[0]->SummaryL()) == 0 );
	test( KDescription().CompareF(calEntryArray[0]->DescriptionL()) == 0); 
	test( KLocation().CompareF(calEntryArray[0]->LocationL()) == 0);
	test( method == calEntryArray[0]->MethodL() );
	test( seq == calEntryArray[0]->SequenceNumberL() );
	test( status == calEntryArray[0]->StatusL() ); // TStatus status(calEntryArray[0]->StatusL());
	
	// Set RDates with an empty list, we should delete the exception date aswell !!
	calEntryArray[0]->SetRDatesL(rDateList2);
	
	rDateList1.Close();
	rDateList2.Close();
	exDateList.Close();
	returnedRDateList.Close();
	returnedEXDateList.Close();
	
	CleanupStack::PopAndDestroy(); // calEntryArray
	
	}
	
void CTestData::TestUpdateL(CCalEntryView* aEntryView, const TDesC8& aGuid)
	{
	RPointerArray<CCalEntry> calEntryArray;
	aEntryView->FetchL(aGuid,calEntryArray);
	CleanupResetAndDestroyPushL(calEntryArray);
	
	// Change the description of the entry
	_LIT(KDescription,"Very Important Meeting"); 
	calEntryArray[0]->SetDescriptionL(KDescription());
	
	//remove the child entry
	delete calEntryArray[1];
	calEntryArray.Remove(1);
	
	TInt NumberOfEntries(0);
	aEntryView->UpdateL(calEntryArray,NumberOfEntries);
	
	// Test the entry was updated
	test( NumberOfEntries == calEntryArray.Count());

	calEntryArray.ResetAndDestroy();
	// Fetch the entry again
	aEntryView->FetchL(aGuid,calEntryArray);

	test( !KDescription().CompareF(calEntryArray[0]->DescriptionL()) );

	CleanupStack::PopAndDestroy(); // calEntryArray
	}
	
	
// Creates a Parent entry and sets some exception dates on the parent
// Intended to be used with GetDailyTestData1L().
void CTestData::CreateParentGsEntryWithExceptionsL(TCalRRule* aRpt, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{
	CleanupStack::PushL(aRpt);
	
	CCalEntry* entry = CreateParentLC(aRpt, aGuid, aType, aFloating);
	
	// Set 3 exceptions on the parent (6, 9, 12 March 2005)
	RArray<TCalTime> exArray;
	
	TCalTime exTime1;
	TCalTime exTime2;
	TCalTime exTime3;

	if (aFloating)
		{
		exTime1.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 5, 0, 0, 0, 0)); // 6 March
		exTime2.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 8, 0, 0, 0, 0)); // 9 March
		exTime3.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 11, 0, 0, 0, 0)); // 12 March
		}
	else
		{
		exTime1.SetTimeUtcL(TDateTime(2005, EMarch, 5, 0, 0, 0, 0)); // 6 March
		exTime2.SetTimeUtcL(TDateTime(2005, EMarch, 8, 0, 0, 0, 0)); // 9 March
		exTime3.SetTimeUtcL(TDateTime(2005, EMarch, 11, 0, 0, 0, 0)); // 12 March
		}

	exArray.Append(exTime1);
	exArray.Append(exTime2);
	exArray.Append(exTime3);
	
	entry->SetExceptionDatesL(exArray);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::Pop(&array);
	array.Reset();
	
	exArray.Reset();
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(aRpt);
	}
	
	
void CTestData::CreateParentGsEntryWithSporadicsL(TCalRRule* aRpt, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{
	CleanupStack::PushL(aRpt);
	
	CCalEntry* entry = CreateParentLC(aRpt, aGuid, aType, aFloating);
	
	// Set 3 sporadic dates on the parent (3, 13, 19 March 2005)
	RArray<TCalTime> rDateArray;
	
	TCalTime rDate1;
	TCalTime rDate2;
	TCalTime rDate3;

	if (aFloating)
		{
		rDate1.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 2, 0, 0, 0, 0));  // 3 March
		rDate2.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 12, 0, 0, 0, 0)); // 13 March
		rDate3.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 18, 0, 0, 0, 0)); // 19 March
		}
	else
		{
		rDate1.SetTimeUtcL(TDateTime(2005, EMarch, 2, 0, 0, 0, 0));  // 3 March
		rDate2.SetTimeUtcL(TDateTime(2005, EMarch, 12, 0, 0, 0, 0)); // 13 March
		rDate3.SetTimeUtcL(TDateTime(2005, EMarch, 18, 0, 0, 0, 0)); // 19 March
		}

	rDateArray.Append(rDate1);
	rDateArray.Append(rDate2);
	rDateArray.Append(rDate3);
	
	entry->SetRDatesL(rDateArray);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::Pop(&array);
	array.Reset();
	
	rDateArray.Reset();
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(aRpt);	
	}
	
// Creates a child exception entry which has a RecId refering to an occurrence in the parent
// and specifies a new occurrence to replace the original according to the child entry's start/end date.
void CTestData::CreateChildExceptionGsEntryL(TTestDataStruct* aTestData, TInt aRecIdToUse, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{
	HBufC8* guid = aGuid.AllocLC();  // Create GUID on heap
	
	// Use Rec-id as given by test-data
	TTime ttime = aTestData->iRecIds[aRecIdToUse-1];
	TCalTime recurrenceId;
	if (aFloating)
		{
		recurrenceId.SetTimeLocalFloatingL(ttime);
		}
	else
		{
		recurrenceId.SetTimeUtcL(ttime);
		}
	CalCommon::TRecurrenceRange range = CalCommon::EThisOnly;	
	
	// Create a child
	// Below takes ownership of guid
	CCalEntry* entry = CCalEntry::NewL(	aType, guid, CCalEntry::EMethodRequest, (TUint)0,
										recurrenceId, range);
	
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	// Set Start date to be somewhere outside of parent's schedule. End time is 1 hour later.
	TTime startDate = TDateTime(2005, EMarch, 18, 0, 0, 0, 0);  // 19 March
	TTime endDate   = (startDate + TTimeIntervalHours(1));
	
	TCalTime startTime;
	TCalTime endTime;

	if (aFloating)
		{
		startTime.SetTimeLocalFloatingL(startDate);
		endTime.SetTimeLocalFloatingL(endDate);
		}
	else
		{
		startTime.SetTimeUtcL(startDate);
		endTime.SetTimeUtcL(endDate);
		}
	entry->SetStartAndEndTimeL(startTime, endTime);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::Pop(&array);
	array.Reset();
	
	CleanupStack::PopAndDestroy(entry);
	}
	
// Creates a child exception entry which has a RecId refering to an occurrence in the parent
// and specifies a new occurrence to replace the original according to the child entry's start/end date.
void CTestData::CreateChildExceptionGsEntry_1L(TTestDataStruct* aTestData, TInt aRecIdToUse, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{
	HBufC8* guid = aGuid.AllocLC();  // Create GUID on heap
	
	// Use Rec-id as given by test-data
	TTime ttime = aTestData->iRecIds[aRecIdToUse-1];
	TCalTime recurrenceId;
	if (aFloating)
		{
		recurrenceId.SetTimeLocalFloatingL(ttime);
		}
	else
		{
		recurrenceId.SetTimeUtcL(ttime);
		}
	CalCommon::TRecurrenceRange range = CalCommon::EThisOnly;	
	
	// Create a child
	// Below takes ownership of guid
	CCalEntry* entry = CCalEntry::NewL(	aType, guid, CCalEntry::EMethodRequest, (TUint)0,
										recurrenceId, range);
	
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	// Set Start date to be somewhere outside of parent's schedule. End time is 1 hour later.
	TTime startDate = TDateTime(2005, EApril, 17, 0, 0, 0, 0);  // 18 April
	TTime endDate   = (startDate + TTimeIntervalHours(1));
	
	TCalTime startTime;
	TCalTime endTime;

	if (aFloating)
		{
		startTime.SetTimeLocalFloatingL(startDate);
		endTime.SetTimeLocalFloatingL(endDate);
		}
	else
		{
		startTime.SetTimeLocalL(startDate);
		endTime.SetTimeLocalL(endDate);
		}
	entry->SetStartAndEndTimeL(startTime, endTime);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::Pop(&array);
	array.Reset();
	
	CleanupStack::PopAndDestroy(entry);
	}
	
// Creates a child rpt rule entry which has a RecId refering to an occurrence in the parent
// and specifies a rpt rule to modify the parent's rrule.
// The parent rrule this is intended to work with is defined in GetWeeklyTestData1L()
// and is Weekly, Wed from 6 March to 10 April 2005.
void CTestData::CreateChildRRuleGsEntryL(TCalRRule* aRpt, TTestDataStruct* aTestData, TInt aRecIdToUse, CalCommon::TRecurrenceRange aRange, CCalEntryView* aEntryView, const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{
	CleanupStack::PushL(aRpt);
	
	HBufC8* guid = aGuid.AllocLC();  // Create GUID on heap
	
	// Use Rec-id as given by test-data
	TTime ttime = aTestData->iRecIds[aRecIdToUse-1];
	TCalTime recurrenceId;
	if (aFloating)
		{
		recurrenceId.SetTimeLocalFloatingL(ttime);
		}
	else
		{
		recurrenceId.SetTimeUtcL(ttime);
		}
	
	// Create a child
	// Below takes ownership of guid
	CCalEntry* entry = CCalEntry::NewL(	aType, guid, CCalEntry::EMethodRequest, (TUint)0,
										recurrenceId, aRange);
	
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	// Set the start time to be same as repeat def's start time
	// and the end time to be 1 hour after start time.
	TCalTime startTime;
	TCalTime calEndTime;
	if (aFloating)
		{
		startTime.SetTimeLocalFloatingL(aRpt->DtStart().TimeLocalL());
		TTime endTime = aRpt->DtStart().TimeLocalL() + TTimeIntervalHours(1);
		calEndTime.SetTimeLocalFloatingL(endTime);
		}
	else
		{
		startTime = aRpt->DtStart();
		TTime endTime = aRpt->DtStart().TimeUtcL() + TTimeIntervalHours(1);
		calEndTime.SetTimeUtcL(endTime);
		}

	entry->SetStartAndEndTimeL(startTime, calEndTime);
	entry->SetRRuleL(*aRpt); // doesn't take ownership
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	
	aEntryView->StoreL(array, success);

	CleanupStack::Pop(&array);
	array.Reset();
	
	CleanupStack::PopAndDestroy(entry);	
	CleanupStack::PopAndDestroy(aRpt);
	}
	
void CTestData::TestDeletionOfChildExceptionsL(CCalEntryView* aEntryView, CCalInstanceView* aInstanceView, const TDesC8& aGuid)
	{
	RPointerArray<CCalEntry> calEntryList;
	CleanupResetAndDestroyPushL(calEntryList);
	aEntryView->FetchL(aGuid, calEntryList);
	
	// delete the child exceptions
	// this should delete the parent as it will be redundant
	const TInt KEntryCount(calEntryList.Count());
	for (TInt i(1) ; i < KEntryCount ; ++i)
		{
		aEntryView->DeleteL(*calEntryList[i]);
		}	
	
	// set up time range to search for instances
	TCalTime startCalTime;
	TCalTime endCalTime;
	startCalTime.SetTimeLocalL(TTime(TDateTime(2005, EMarch, 0, 0, 0, 0, 0))); //  1 March 2005
	endCalTime.SetTimeLocalL(TTime(TDateTime(2005, EApril, 26, 0, 0, 0, 0))); 		// 27 April 2005
	CalCommon::TCalTimeRange calTimeRange(startCalTime, endCalTime);
	
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	aInstanceView->FindInstanceL(instanceList, CalCommon::EIncludeAll, calTimeRange);
	
	// test that there are no instances in the time range
	test(instanceList.Count() == 0);		
	
	CleanupStack::PopAndDestroy(2); // instanceList, calEntryList	
	}
	
// Creates a parent entry with a daily rpt rule
// and adds rdates to the parent entry. 
void CTestData::CreateWeeklyParentGsEntryWithRDateRRuleL(TTestDataStruct& aTestData, CCalEntryView* aEntryView, const TDesC8& aGuid, TExceptionRDate aExcpnRDate, CCalEntry::TType aType, TBool aFloating)
	{
	TCalRRule rptRule(TCalRRule::EDaily);
	if (aFloating)
		{
		rptRule.SetInterval(1);
		TCalTime startTime;
		startTime.SetTimeLocalFloatingL(TDateTime(2005, EDecember, 13, 10, 0, 0, 0));
		rptRule.SetDtStart(startTime); 	// 13 Dec 2005
		TCalTime endTime;
		endTime.SetTimeLocalFloatingL(TDateTime(2005, EDecember, 15, 10, 0, 0, 0));
		rptRule.SetUntil(endTime);			// 16 Dec 2005
		
		aTestData.iStartDate = TDateTime(2005, EDecember, 0, 0, 0, 0, 0);		//  1 Dec 2005
		aTestData.iEndDate = TDateTime(2005, EDecember, 30, 0, 0, 0, 0); 		// 31 Dec 2005
		aTestData.iDescription.Append(_L(" Every 1 days from 13 Dec 05\n"));

		// Set up the RecId's
		aTestData.iRecIds.Append(TTime(TDateTime(2005, EDecember, 13, 10, 0, 0, 0))); 	//  14 Dec
		aTestData.iRecIds.Append(TTime(TDateTime(2005, EDecember, 14, 10, 0, 0, 0))); 	//  15 Dec
		aTestData.iRecIds.Append(TTime(TDateTime(2005, EDecember, 15, 10, 0, 0, 0))); 	//  16 Dec
		}
	else
		{
		rptRule.SetInterval(1);
		TCalTime startTime;
		startTime.SetTimeLocalL(TDateTime(2005, EDecember, 13, 10, 0, 0, 0));
		rptRule.SetDtStart(startTime); 	// 13 Dec 2005
		TCalTime endTime;
		endTime.SetTimeLocalL(TDateTime(2005, EDecember, 15, 10, 0, 0, 0));
		rptRule.SetUntil(endTime);			// 16 Dec 2005
		
		aTestData.iStartDate = TDateTime(2005, EDecember, 0, 0, 0, 0, 0);		//  1 Dec 2005
		aTestData.iEndDate = TDateTime(2005, EDecember, 30, 0, 0, 0, 0); 		// 31 Dec 2005
		aTestData.iDescription.Append(_L(" Every 1 days from 13 Dec 05\n"));

		// Set up the RecId's
		aTestData.iRecIds.Append(TTime(TDateTime(2005, EDecember, 13, 10, 0, 0, 0))); 	//  14 Dec
		aTestData.iRecIds.Append(TTime(TDateTime(2005, EDecember, 14, 10, 0, 0, 0))); 	//  15 Dec
		aTestData.iRecIds.Append(TTime(TDateTime(2005, EDecember, 15, 10, 0, 0, 0))); 	//  16 Dec
		}
	
	CCalEntry* entry = CreateParentLC(&rptRule, aGuid, aType, aFloating);
	
	RArray<TCalTime> rDateArray;
	CleanupClosePushL(rDateArray);
	
	// Add rdates to parent rpt rule
	TCalTime rDate1;
	if (aExcpnRDate == ERDateBeforeRRule) // etrue for now
		{
		if (aFloating)
			{
			rDate1.SetTimeLocalFloatingL(TDateTime(2005, EDecember, 11, 9, 0, 0, 0));  // 12 Dec
			}
		else
			{
			rDate1.SetTimeLocalL(TDateTime(2005, EDecember, 11, 9, 0, 0, 0));  // 12 Dec
			}
		}
	else if (aExcpnRDate == ERDateAfterRRule)
		{
		if (aFloating)
			{
			rDate1.SetTimeLocalFloatingL(TDateTime(2005, EDecember, 19, 9, 0, 0, 0));  // 20 Dec
			}
		else
			{
			rDate1.SetTimeLocalL(TDateTime(2005, EDecember, 19, 9, 0, 0, 0));  // 20 Dec
			}
		}
	rDateArray.Append(rDate1);
	entry->SetRDatesL(rDateArray);
	
	// set RRule again as mentioned in defect DEF076160
	//TCalRRule rptRule2;
	//entry->GetRRuleL(rptRule2);
	//entry->SetRRuleL(rptRule2);
	
	RPointerArray<CCalEntry> array;
	CleanupClosePushL(array);
	array.Append(entry);
	TInt success(0);
	aEntryView->StoreL(array, success);
	CleanupStack::Pop(&array);
	array.Reset();
	
	CleanupStack::PopAndDestroy(); // rDateArray
	CleanupStack::PopAndDestroy(entry);
	}


void CTestData::SetExceptionsL(CCalEntry* aEntry, RArray<TTime>& aTimes, TBool aFloating)
	{
	RArray<TCalTime> exArray;
	CleanupClosePushL(exArray);
	
	TCalTime calTime;
	TInt nExceptions = aTimes.Count();
	for (TInt i=0; i<nExceptions; i++)
		{
		aFloating ? calTime.SetTimeLocalFloatingL(aTimes[i]) : calTime.SetTimeUtcL(aTimes[i]);		
		exArray.Append(calTime);
		}
	aEntry->SetExceptionDatesL(exArray);
	CleanupStack::PopAndDestroy(&exArray);	
	}

void CTestData::TestDeletionOfEntryThatHasNoInstancesL(TCalRRule* aRpt, TTestDataStruct& aData, 
										CCalEntryView* aEntryView, CCalInstanceView* /*aInstanceView*/,
									   	const TDesC8& aGuid, CCalEntry::TType aType, TBool aFloating)
	{
	CleanupStack::PushL(aRpt);

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	// Attempt addition of an entry that has all its instances marked as exceptions.
	// Such entry has no effective occurences. StoreL() ignores this fact and 
	// allows to store such dummy entry without problem.
	CCalEntry* entry = CreateParentLC(aRpt, aGuid, aType, aFloating);
	SetExceptionsL(entry, aData.iRecIds, aFloating);
	entries.Append(entry);
	CleanupStack::Pop(entry);
	TInt success(0);
	TRAPD(err, aEntryView->StoreL(entries, success));
	test(err==KErrNone);	// allow dummy entry (all instances are exceptions) to be stored
	entries.ResetAndDestroy();
	
	// Store valid entry first, then fetch it, set exceptions on all instances, store again. 
	// Expect that StoreL() results in deletion of the entry as it no longer has any instances.
	entry = CreateParentLC(aRpt, aGuid, aType, aFloating);
	entries.Append(entry);
	CleanupStack::Pop(entry);
	aEntryView->StoreL(entries, success);
	test(success==1);
	entries.ResetAndDestroy();		
	
	aEntryView->FetchL(aGuid, entries);   
	test(entries.Count()==1); 

	SetExceptionsL(entries[0], aData.iRecIds, aFloating);	
	TRAP(err, aEntryView->StoreL(entries, success));
	test(err==KErrNone);
	test(success==1);	// success is 1, although in fact the entry was deleted, check this below.
	entries.ResetAndDestroy();		
	
	aEntryView->FetchL(aGuid, entries);
	TInt nEntries = entries.Count();
	test(nEntries==0);
	
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(aRpt);
	}


void CTestData::StoreRestoreL(CCalEntryView* aEntryView, const TDesC8& aGuid)
	{
	// parent:	CreateWeeklyParentGsEntryWithExceptionsRdatesL(EException)
	// 		march:X[09]wed [16] X[23] X[30] april:[06] [13] [20]		(7 instances, 3 exceptions)
	//
	// child: 	GetWeeklyTestDataModification1L()
	//		april: [08]fri [15] [22]
	//
	// hang the child on the 2nd instance of the parent (16/03) This&Future:
	//
	// parent becomes:
	//		march:X[09] X[16] - others trimmed							(2 instances, 2 exceptions)
	//
	// TEST: fetch the parent, and store it back again without modification.
	// EXPECTED RESULT: although all instances of the parent are exceptioned, you would expect the parent
	//					entry not to be deleted, because of a child.
	//					The truth is that the child is deleted when parent is updated.
	//					If you re-store (update) the parent one more time (without a child this time), 
	//					such parent will be deleted as a result of the update operation.

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);	
	
	aEntryView->FetchL(aGuid, entries);   
	test(entries.Count()==2); // a parent and a child
	
	delete entries[1]; // discard the child
	entries.Remove(1);

	TInt success(0);
	TRAPD(err, aEntryView->StoreL(entries, success));	// re-store parent only
	test(err==KErrNone);
	test(success==1);
	entries.ResetAndDestroy();		
	aEntryView->FetchL(aGuid, entries);
	test(entries.Count()==1);	// child is deleted as a result of update of the parent entry

	TRAP(err, aEntryView->StoreL(entries, success));	// re-store parent (has no child any more)
	test(err==KErrNone);
	test(success==1);
	entries.ResetAndDestroy();		
	aEntryView->FetchL(aGuid, entries);
	test(entries.Count()==0);	// single parent was deleted as a result of update operation

	CleanupStack::PopAndDestroy(&entries);
	}
