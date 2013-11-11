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

#include "TestCalInterimApiFilter.h"
#include <calsession.h>
#include <calinstance.h>
#include <calinstanceview.h>

_LIT(KStartTime,				"starttime");
_LIT(KEndTime,					"endtime");
_LIT(KSearchText,				"searchtext");
_LIT(KSearchBehaviour,			"searchbehaviour");
_LIT(KAllocTest,				"alloctest");
_LIT(KFilter,					"filter");
_LIT(KExpectedCount,			"expectedcount");
_LIT(KGUID,						"guid");
_LIT(KInstanceLocalTime,		"instancelocaltime");
_LIT(KExpectedInstances,		"expectedinstances");
_LIT(KPerformanceTest,			"performancetest");
_LIT(KTimeLimit,	 			"maxtimelimit");

_LIT(KInfoGUID,					"GUID: %S");
_LIT(KInfoSummary,				"Summary: %S");
_LIT(KInfoDescription,			"Description: %S");
_LIT(KInfoLocation,				"Location: %S");
_LIT(KNewLine,					"\r\n");
_LIT(KInfoType,					"Entry Type: %D");
_LIT(KInfoCount,				"Checking count");
_LIT(KInfoInstanceFound,		"The instance in section: %S was found by FindInstanceL");
_LIT(KInfoNoOfInstancesFound, 	"No. of Instances found: %D");

_LIT(KErrInstanceNotFound,		"The instance in section: %S was NOT found by FindInstanceL");
_LIT(KErrNotInOrder,			"The instance is not chronologically after the previous instance");

CTestCalInterimApiFilter::CTestCalInterimApiFilter()
:	CTestCalInterimApiSuiteStepBase(), iInstanceView(NULL)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiFilter);
	}

CTestCalInterimApiFilter::~CTestCalInterimApiFilter()
/**
 * Destructor
 */
	{
	delete iInstanceView;
	iInstanceView = NULL;
	}

/**
@SYMTestCaseID			PIM-CIT-CallnterimAPI-1098AP-11			 
@SYMTestType			CIT
@SYMTestPriority		Medium
@SYMFssID Filter		3.2.16
@SYMTestCaseDesc		Setting the filter for fetching entries
@SYMTestActions			Open calendar file and fetch filterd entries
@SYMTestExpectedResults	The entries fetched should be filtered
*/
TVerdict CTestCalInterimApiFilter::doTestStepL()
	{
	TPtrC		startTimeStr;
	TPtrC		endTimeStr;

	TInt 		expectedCount(-1);
	TInt 		filter(0);
	
	TBool		allocTest(EFalse);
	TPtrC		expectedInstances(KNullDesC);

	TBool		performanceTest(EFalse);
	TReal32		maxTimeLimit(5.0);
	TPtrC 		timeLimitString;

	// We dont mind if these are not mentioned. They are only additional tests
	GetIntFromConfig   (ConfigSection(), KExpectedCount, expectedCount);
	GetBoolFromConfig  (ConfigSection(), KAllocTest, allocTest);
	GetStringFromConfig(ConfigSection(), KExpectedInstances, expectedInstances);
	
	GetBoolFromConfig(ConfigSection(), KPerformanceTest, performanceTest);
	// Assign defaults if these are not provided
	if(! GetStringFromConfig(ConfigSection(), KStartTime, startTimeStr))
		{// min time
		iStartDate.SetTimeLocalL(TCalTime::MinTime());	
		}
	else
		{
		TTime startTime(startTimeStr);
		iStartDate.SetTimeLocalL(startTime);
		}	
	
	if(! GetStringFromConfig(ConfigSection(), KEndTime, endTimeStr))
		{// max time
		iEndDate.SetTimeLocalL(TCalTime::MaxTime());
		}
	else
		{
		TTime endTime(endTimeStr);
		iEndDate.SetTimeLocalL(endTime);
		}	
		
	if(! GetIntFromConfig(ConfigSection(), KFilter, filter))
		{// include all
		filter = CalCommon::EIncludeAll;
		}
		
	if (!performanceTest)
		{
		if ( TestStepResult()==EPass )
			{
			if (!iInstanceView)
				{
				iInstanceView=CCalInstanceView::NewL(GetSession(), *this);
				CActiveScheduler::Start();
				}
			
			FilterEntriesL(CalCommon::TCalViewFilter(filter), expectedCount, allocTest, expectedInstances);
			}
		}
	else
		{
		// Performance test to determine the time taken for creating an instance view
		GetStringFromConfig(ConfigSection(), KTimeLimit, timeLimitString);
		maxTimeLimit = GetTReal32(timeLimitString);
  
		if 	(maxTimeLimit <= 0.0)
			{
			maxTimeLimit = 5.0;
			}
			
		PerformanceTestInstanceViewL(maxTimeLimit);
		}

	return TestStepResult();
	}
	
/**
@SYMTestCaseID 			PIM-CIT-CallnterimAPI-1098AP-11			 
@SYMTestType 			CIT
@SYMTestPriority 		Medium
@SYMFssID 				Filter 3.2.16
@SYMTestCaseDesc 		Setting the filter for fetching entries
@SYMTestActions 		Set the filter date and fetch filterd entries, these are then compared to actual values
@SYMTestExpectedResults	The entries fetched should be filtered and they should be == to defined values
*/
void CTestCalInterimApiFilter::FilterEntriesL(CalCommon::TCalViewFilter aFilter, TInt aExpectedCount, TBool aAllocTest, const TPtrC& aExpectedInstances)
	{
	RPointerArray<CCalInstance>	instanceArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));
	
	CalCommon::TCalTimeRange	timeRange(iStartDate, iEndDate);
	
	TInt 		err(KErrNone);
	TPtrC		searchText(KNullDesC);
	TPtrC		searchBehaviour(KNullDesC);
	
	GetStringFromConfig(ConfigSection(), KSearchText, searchText);
	GetStringFromConfig(ConfigSection(), KSearchBehaviour, searchBehaviour);

	
	CalCommon::TCalSearchBehaviour behaviour = GetBehaviourL(searchBehaviour);
	
	if( aAllocTest )
		{// Run FinInstance in OOM LOOP
		TInt tryCount = 1;
		for ( ;; )
			{
			instanceArray.ResetAndDestroy();
			__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
			
			TRAP(err, PerformSearchL(instanceArray, aFilter, timeRange, searchText, behaviour));
					
			if ( err==KErrNone ) 
				{
				__UHEAP_RESET;
				INFO_PRINTF1(_L("Memory allocation testing for filters is done"));
				break;
				}
			if ( err != KErrNoMemory )
				SetTestStepResult(EFail);
			__UHEAP_SETFAIL(RHeap::ENone, 0);		
			tryCount++;
			}	
			INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
		}
	else
		{
		TRAP(err, PerformSearchL(instanceArray, aFilter, timeRange, searchText, behaviour));
		}
		
	// If there is error in FindInstanceL, no point in continuing	
	SetTestStepError(err);
	
	if(aExpectedCount > -1)
		{
		// Check the count
		INFO_PRINTF1(KInfoCount);
		TEST1(instanceArray.Count() == aExpectedCount, ETrue);
		}	
		
	// Check the instances we are interested in
	if(aExpectedInstances != KNullDesC)
		{
		RArray<TPtrC> sections;
		TokenizeStringL(aExpectedInstances, sections);
		CheckInstancesL(instanceArray, sections);
		}
		
	PrintAllInstancesL(instanceArray);	
		
	CleanupStack::PopAndDestroy(&instanceArray);
	}

/**
Performs the FindInstanceL search
*/
void CTestCalInterimApiFilter::PerformSearchL(RPointerArray<CCalInstance>& aInstanceArray, CalCommon::TCalViewFilter aFilter, CalCommon::TCalTimeRange aTimeRange, const TPtrC& aSearchText, CalCommon::TCalSearchBehaviour aBehaviour)
	{
	if(aSearchText != KNullDesC)
		{
		iInstanceView->FindInstanceL(aInstanceArray, aFilter, aTimeRange, CCalInstanceView::TCalSearchParams(aSearchText, aBehaviour));
		}
	else
		{
		iInstanceView->FindInstanceL(aInstanceArray, aFilter, aTimeRange);
		}
	}	

/**
 * Checks the instances found against the ones mentioned in the ini.
 */
void CTestCalInterimApiFilter::CheckInstancesL(RPointerArray<CCalInstance>& aInstanceArray, RArray<TPtrC>& aSections)
	{
	TPtrC gUID;
	TPtrC timeStr; 
	
	for(TInt i = 0; i < aSections.Count(); ++i)
		{// Loop through the list of sections
		
		TESTL(GetStringFromConfig(aSections[i], KGUID, gUID));
				
		// Construct an 8-bit version
		HBufC8* gUID8 = HBufC8::NewLC(gUID.Length());
		gUID8->Des().Copy(gUID);
		
		// Construct the expected instance time
		TCalTime expectedInstanceTime;
		
		if(GetStringFromConfig(aSections[i], KInstanceLocalTime, timeStr))
			{
			TTime time(timeStr);
			expectedInstanceTime.SetTimeLocalL(time);
			}

		TInt cnt = 0;
		for(; cnt < aInstanceArray.Count(); ++cnt)
			{// Check against each instance that FindInstanceL returned
			if(aInstanceArray[cnt]->Entry().UidL() == gUID8->Des())
				{
				if(timeStr != KNullDesC)
					{// if instance local time is specified in ini compare it and break if it matches
					if(aInstanceArray[cnt]->Time().TimeLocalL() == expectedInstanceTime.TimeLocalL())
						{
						break;
						}
					}
				else
					{// instance local time not specified in ini. Just break here as GUID has matched
					 // in case of undated todos we don't check the time
					break;
					}	
				}
			} // cnt
		
		if(cnt >= aInstanceArray.Count())
			{// Instance not found
			ERR_PRINTF2(KErrInstanceNotFound, &aSections[i]);
			SetTestStepResult(EFail);
			}
		else
			{
			INFO_PRINTF2(KInfoInstanceFound, &aSections[i]);	
			}	
			
		CleanupStack::PopAndDestroy(gUID8);	
		
		}// i
	}

/**
 * Prints GUID, Type and Time of all instances in the array
 */
void CTestCalInterimApiFilter::PrintAllInstancesL(RPointerArray<CCalInstance>& aInstanceArray)
	{
	INFO_PRINTF2(KInfoNoOfInstancesFound, aInstanceArray.Count());
	TTime previousInstanceTime(Time::NullTTime());
	
	for(TInt cnt = 0; cnt < aInstanceArray.Count(); ++cnt)
		{
		// Print GUID
		HBufC* foundGUID = HBufC::NewLC(aInstanceArray[cnt]->Entry().UidL().Length());
		foundGUID->Des().Copy(aInstanceArray[cnt]->Entry().UidL());
		INFO_PRINTF1(KNewLine);
		INFO_PRINTF2(KInfoGUID, foundGUID);
		CleanupStack::PopAndDestroy(foundGUID);
		
		// Print Entry Type
		INFO_PRINTF2(KInfoType, (TInt)aInstanceArray[cnt]->Entry().EntryTypeL());
		
		// Print Instance Time
		PrintTimeL(aInstanceArray[cnt]->Time().TimeLocalL());
		
		// Print Instance End Time
		PrintTimeL(aInstanceArray[cnt]->EndTimeL().TimeLocalL());
				
		// Check chronological order
		if(aInstanceArray[cnt]->Time().TimeUtcL() < previousInstanceTime)
			{
			ERR_PRINTF1(KErrNotInOrder);
			SetTestStepResult(EFail);
			}	
		previousInstanceTime = aInstanceArray[cnt]->Time().TimeUtcL();
		
		// Print Summary
		INFO_PRINTF2(KInfoSummary, &(aInstanceArray[cnt]->Entry().SummaryL()));
		
		// Print Description
		INFO_PRINTF2(KInfoDescription, &(aInstanceArray[cnt]->Entry().DescriptionL()));
		
		// Print Location
		INFO_PRINTF2(KInfoLocation, &(aInstanceArray[cnt]->Entry().LocationL()));
		
		INFO_PRINTF1(KNewLine);
		
		// Delete the instance as, if there are very large no. of instances we will get
		// KErrNoMemory due to loading of "FullEntry" when getting Summary etc.
		delete aInstanceArray[cnt];
		aInstanceArray[cnt] = NULL;
		//??aInstanceArray.Remove(cnt);
			
		} // cnt
	}

CalCommon::TCalSearchBehaviour CTestCalInterimApiFilter::GetBehaviourL(const TPtrC aSearchBehaviour)
	{
	_LIT(KFoldedTextSearch, 				"EFoldedTextSearch");
	_LIT(KExactTextSearch, 					"EExactTextSearch");
	
	//? For functional change tests
	_LIT(KSearchEntryLocation, 				"ESearchEntryLocation");
	_LIT(KSearchEntryDescription, 			"ESearchEntryDescription");
	_LIT(KSearchAttendeeAddress, 			"ESearchAttendeeAddress");
	_LIT(KSearchAttendeeSentByAddress, 		"ESearchAttendeeSentByAddress");
	_LIT(KSearchAttendeeCommonName, 		"ESearchAttendeeCommonName");
	_LIT(KSearchOrganizerAddress, 			"ESearchOrganizerAddress");
	_LIT(KSearchOrganizerSentByAddress, 	"ESearchOrganizerSentByAddress");
	_LIT(KSearchOrganizerCommonName, 		"ESearchOrganizerCommonName");
	_LIT(KSearchAllTextFields, 				"ESearchAllTextFields");
		
	TUint behaviour = 0x0;
	
	RArray<TPtrC> list;
	TokenizeStringL(aSearchBehaviour, list);
	
	for(TInt i = 0; i < list.Count(); ++i)
		{
		if(	list[i] == KFoldedTextSearch)
			{
			behaviour 	|= CalCommon::EFoldedTextSearch;
			}
		else if(list[i] == KExactTextSearch)	
			{
			behaviour 	|= CalCommon::EExactTextSearch;
			}
		else if(list[i] == KSearchEntryLocation)	
			{
			behaviour 	|= CalCommon::ESearchEntryLocation;
			}
		else if(list[i] == KSearchEntryDescription)	
			{
			behaviour 	|= CalCommon::ESearchEntryDescription;
			}
		else if(list[i] == KSearchAttendeeAddress)	
			{
			behaviour 	|= CalCommon::ESearchAttendeeAddress;
			}
		else if(list[i] == KSearchAttendeeSentByAddress)	
			{
			behaviour 	|= CalCommon::ESearchAttendeeSentByAddress;
			}
		else if(list[i] == KSearchAttendeeCommonName)	
			{
			behaviour 	|= CalCommon::ESearchAttendeeCommonName;
			}
		else if(list[i] == KSearchOrganizerAddress)	
			{
			behaviour 	|= CalCommon::ESearchOrganizerAddress;
			}
		else if(list[i] == KSearchOrganizerSentByAddress)	
			{
			behaviour 	|= CalCommon::ESearchOrganizerSentByAddress;
			}
		else if(list[i] == KSearchOrganizerCommonName)	
			{
			behaviour 	|= CalCommon::ESearchOrganizerCommonName;
			}
		else if(list[i] == KSearchAllTextFields)	
			{
			behaviour 	|= CalCommon::ESearchAllTextFields;
			}
		}

	return (CalCommon::TCalSearchBehaviour)behaviour;	
	}

/**
  *Creates an instance view from a version 9.1 or version 9.2 calendar file.
  *Sets test step to EFail if the time taken is more than the maximum time
  *limit provided in the .ini file.
  @param aMaxTimeLimit maximum time limit in which the operation should complete
*/
void CTestCalInterimApiFilter::PerformanceTestInstanceViewL(TReal32& aMaxTimeLimit)
	{

	// Starts recording time
	TTime startTime, endTime;
	startTime.HomeTime();

	// Creates an instance view from v9.1 or v9.2 calendar file.
	if (iInstanceView)
		{
		delete iInstanceView;
		iInstanceView = NULL;
		}
		
	iInstanceView = CCalInstanceView::NewL(GetSession(), *this);
	
	CActiveScheduler::Start();
	// Stops recording time
	endTime.HomeTime();

	if ( TestStepResult() == EPass )
		{
		if (!ComputeTimeDifference(startTime, endTime, aMaxTimeLimit))
			{
			SetTestStepResult(EFail);
			}
		}
	}
