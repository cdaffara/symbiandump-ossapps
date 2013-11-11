// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalInterimApiDeleteInstances.h"
#include <calsession.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calcommon.h>

_LIT(KFilterStartDateTime,			"filterstartdatetime");
_LIT(KFilterEndDateTime,			"filterenddatetime");
_LIT(KInstanceDateTime,				"instancedatetime");
_LIT(KRecurrenceRange,				"recurrencerange");
_LIT(KFilter,						"filter");
_LIT(KExpectedCount,				"expectedcountofinstances");
_LIT(KTimeMode,						"timemode");
_LIT(KAllocTest,					"alloctest");
_LIT(KFormatDate, 					"%D%M%Y%/0%1%/1%2%/2%3%/3");

_LIT(KTypeThisAndAll,				"ThisAndAll");
_LIT(KTypeThisAndFuture,			"ThisAndFuture");
_LIT(KTypeThisAndPrior,				"ThisAndPrior");

_LIT(KInfoInstanceDateTime,			"Instance DateTime %S, %d, %d");
_LIT(KInfoDelete,					"Delete Successful");

_LIT(KErrInDelete,					"Error::DeleteL() failed");
_LIT(KErrExpectedCount,				"FindInstance found %d. Expected %d");

CTestCalInterimApiDeleteInstances::CTestCalInterimApiDeleteInstances()
:	CTestCalInterimApiSuiteStepBase()
,	iAllocTest(EFalse)
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalInterimApiDeleteInstances);
	}

CTestCalInterimApiDeleteInstances::~CTestCalInterimApiDeleteInstances()
/**
 * Destructor
 */
	{
	delete iInstanceView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
*/
TVerdict CTestCalInterimApiDeleteInstances::doTestStepL()
	{
	TPtrC	filterStartDateTime;
	TPtrC	filterEndDateTime;
	TPtrC	instanceDateTime;
	TPtrC	timeMode;
	TInt	expectedCountOfInstances;

	TESTL( GetStringFromConfig(ConfigSection(),  KFilterStartDateTime,  filterStartDateTime) );
	TESTL( GetStringFromConfig(ConfigSection(),  KFilterEndDateTime,  filterEndDateTime) );
	TESTL( GetStringFromConfig(ConfigSection(),  KInstanceDateTime, instanceDateTime) );
	TESTL( GetStringFromConfig(ConfigSection(),  KRecurrenceRange, iRecurrenceRange) );
	TESTL( GetIntFromConfig(ConfigSection(),  KFilter, iFilter) );
	TESTL( GetIntFromConfig(ConfigSection(),  KExpectedCount, expectedCountOfInstances) );
	// Dont mind if it is not given in the ini file
	GetStringFromConfig(ConfigSection(), KTimeMode, timeMode);
	GetBoolFromConfig(ConfigSection(), KAllocTest, iAllocTest);
	
	iInstanceView = CCalInstanceView::NewL(GetSession(), *this);
	CActiveScheduler::Start();

	// To convert TPtrc to TTime
	TTime	startDateTime;
	startDateTime.Set(filterStartDateTime);
	TTime	endDateTime;
	endDateTime.Set(filterEndDateTime);
	TTime	instanceData;
	instanceData.Set(instanceDateTime);

	// To change TTime as TCalTime and set the time in fixed or utc based on the time mode given
	TCalTime	calStartDateTime;
	SetCalTimeL(startDateTime, iCalStartDateTime, SetTimeMode(timeMode));
	TCalTime	calEndDateTime;
	SetCalTimeL(endDateTime, iCalEndDateTime, SetTimeMode(timeMode));
	TCalTime	instance;
	SetCalTimeL(instanceData, iInstanceDateTime, SetTimeMode(timeMode));
	
	FindTheInstancesAndDeleteL(expectedCountOfInstances);	

	return TestStepResult();
	}

/** Find the instances of the entry before deleting the instances.
@param	aExpectedCountOfInstances Data to check teh expected count of the instances
*/
void CTestCalInterimApiDeleteInstances::FindTheInstancesAndDeleteL(const TInt& aExpectedCountOfInstances)
	{
	RPointerArray<CCalInstance>	instanceArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &instanceArray));

	CalCommon::TCalTimeRange	calTimeRange(iCalStartDateTime,iCalEndDateTime);
	iInstanceView->FindInstanceL(instanceArray, CalCommon::TCalViewFilter(iFilter), calTimeRange);
	if ( instanceArray.Count() == aExpectedCountOfInstances )
		{
		PrintInstancesDateTimeL(instanceArray);
		CompareInstanceDateTimeAndDeleteL(instanceArray);
		}
	else
		{
		ERR_PRINTF3(KErrExpectedCount, instanceArray.Count(), aExpectedCountOfInstances);
		SetTestStepResult(EFail);
		}
	CleanupStack::PopAndDestroy(&instanceArray);
	}
	
/** Compare the instance in the array list with the instance fetched from ini and delete
the instances based upon the recurrence range
@param	aInstanceArray Array containing the instance list
*/	
void CTestCalInterimApiDeleteInstances::CompareInstanceDateTimeAndDeleteL(RPointerArray<CCalInstance>& aInstanceArray)
	{
	TInt	instCount(0);
	TBuf<KMaxTestExecuteCommandLength>	 instBuf;
	while (instCount < aInstanceArray.Count())
		{
		CCalInstance*	instance = aInstanceArray[instCount];
		// Compares the instance that is fetched from ini with the instance in the array list and 
		// then deletes the instances.
		if ( instance->Time().TimeUtcL() == iInstanceDateTime.TimeUtcL() )
			{
			CalCommon::TRecurrenceRange	recurrenceRange = ConvertToRecurrenceRange(iRecurrenceRange);
			if ( iAllocTest )
				{
				PerformOOMTestsForDeleteInstancesL(instance, recurrenceRange);
				aInstanceArray.Remove(instCount++);
				break;	
				}
			else
				{
				TRAPD( err, iInstanceView->DeleteL(instance, recurrenceRange) );
				if ( err == KErrNone )
					{
					INFO_PRINTF1(KInfoDelete);
					}
				else
					{
					ERR_PRINTF1(KErrInDelete);
					SetTestStepResult(EFail);
					}
				aInstanceArray.Remove(instCount++);
				break;	
				}
			}
		instCount++;
		}
	}


/** Print the instances date and time
@param	aInstanceArray Array containing the instances
*/	
void CTestCalInterimApiDeleteInstances::PrintInstancesDateTimeL(RPointerArray<CCalInstance>& aInstanceArray)
	{
	TInt	instCount(0);
	TTime	instTime;
	TBuf<KMaxTestExecuteCommandLength>	 instBuf;
	while (instCount < aInstanceArray.Count())
		{
		CCalInstance*	instance = aInstanceArray[instCount];
		instTime = instance->Time().TimeUtcL();
		instTime.FormatL(instBuf, KFormatDate());
		INFO_PRINTF4(KInfoInstanceDateTime, &instBuf, instTime.DateTime().Hour(), instTime.DateTime().Minute());
		instCount++;
		}
	}


/** Gets the data from the ini file and converts it of type CalCommon::TRecurrenceRange
@param	aRecurrenceRange recurrence range of the instances
@test
*/	
CalCommon::TRecurrenceRange CTestCalInterimApiDeleteInstances::ConvertToRecurrenceRange(const TPtrC& aRecurrenceRange)
	{
	CalCommon::TRecurrenceRange	retrecurrence = CalCommon::EThisOnly;
	if ( aRecurrenceRange.Compare(KTypeThisAndAll) == 0 )
		{
		retrecurrence = CalCommon::EThisAndAll;
		}
	else if ( aRecurrenceRange.Compare(KTypeThisAndFuture) == 0 )
		{
		retrecurrence = CalCommon::EThisAndFuture;
		}
	else if ( aRecurrenceRange.Compare(KTypeThisAndPrior) == 0 )
		{
		retrecurrence = CalCommon::EThisAndPrior;
		}
	return retrecurrence;
	}
	
/** OOM testing for Finding the instances and delete the instances
@param	aInstance Pointer to CCalInstance
@param	aRecurrenceRange Recurrence range of the instance
*/	
void CTestCalInterimApiDeleteInstances::PerformOOMTestsForDeleteInstancesL(CCalInstance* aInstance, CalCommon::TRecurrenceRange aRecurrenceRange)
	{
	TInt	tryCount = 1;
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		
		TRAPD( err, iInstanceView->DeleteL(aInstance, aRecurrenceRange) );
				
		if ( err==KErrNone ) 
			{
			__UHEAP_RESET;
			INFO_PRINTF1(_L("Memory allocation testing for Delete Instances is done"));
			break;
			}
		if ( err != KErrNoMemory )
			SetTestStepResult(EFail);
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		tryCount++;
		}	
	INFO_PRINTF2(_L("TRYCOUNT %d"), tryCount);
	}





