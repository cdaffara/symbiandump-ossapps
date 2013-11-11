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

#include "TestCalInterimApiTZChanges.h"
#include <calalarm.h>
#include <caliterator.h>
#include <fbs.h>
#include <s32file.h>
#include <e32test.h>

// Class CTestCalInterimApiTZChange - This class verifies the times of entry and instance after TZ Change, DST change and system time change
CTestCalInterimApiTZChange::CTestCalInterimApiTZChange()
:	iInstanceView(NULL)
, 	iIsFloating(FALSE)
	{
	SetTestStepName(KTestCalInterimApiTZChange);
	}

CTestCalInterimApiTZChange::~CTestCalInterimApiTZChange()
	{
	delete iInstanceView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiTZChange::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiTZChange::doTestStepL() start"));
	
	iInstanceView = CCalInstanceView::NewL(GetSession(), *this);
		
	iField = KIsFloating;
	TESTL(GetBoolFromConfig(ConfigSection(), iField, iIsFloating));	
		
	CActiveScheduler::Start();

	SetTimeRangeL();
	
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	CalCommon::TCalTimeRange timeRange(iStartDate, iEndDate);
	
	RPointerArray<CCalInstance> fetchInstanceArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyInstanceArray, &fetchInstanceArray));
	
	TRAPD(err, iInstanceView->FindInstanceL(fetchInstanceArray, filter, timeRange));
	
	if (err == KErrNone)
		{
		for ( TInt index = 0; index < fetchInstanceArray.Count(); index++ )
			{
			INFO_PRINTF2(_L("Instance:- %d"), index);
			CheckInstancesTimesL(fetchInstanceArray, index);
			}
		INFO_PRINTF1(KInstanceTimesCorrect);
		CheckEntryTimesL(fetchInstanceArray);
		}
	else
		{
		ERR_PRINTF1(_L("Instances not fetched successfully"));
		SetTestStepResult(EFail);
		SetTestStepError(err);
		}
		
	CleanupStack::PopAndDestroy(&fetchInstanceArray); // fetchInstanceArray.reset & fetchInstanceArray.destroy;
	
	INFO_PRINTF1(_L("CTestCalInterimApiTZChange::doTestStepL() finish"));
	return TestStepResult();
	}

/** Sets the filter start and end date (the data is taken from the ini file TestCalTZChanges.ini)
@test
*/
void CTestCalInterimApiTZChange::SetTimeRangeL()
	{
	iField = KFilterStart;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrFilterStart)); // gets expected start date for filter
	
	iField = KFilterEnd;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrFilterEnd)); // gets expected end date for filter
	
	if (iIsFloating)
		{
		// To convert Tptrc to TTime
		TTime filterstartdate;
		filterstartdate.Set(iPtrFilterStart);
		
		SetCalTimeL(filterstartdate, iStartDate, TCalTime::EFloating);
		
		TTime filterenddate;
		filterenddate.Set(iPtrFilterEnd);
		SetCalTimeL(filterenddate, iEndDate, TCalTime::EFloating);
		}
	else
		{	
		// To convert Tptrc to TTime
		TTime filterstartdate;
		filterstartdate.Set(iPtrFilterStart);
		
		SetCalTimeL(filterstartdate, iStartDate, TCalTime::EFixedUtc);
		
		TTime filterenddate;
		filterenddate.Set(iPtrFilterEnd);
		SetCalTimeL(filterenddate, iEndDate, TCalTime::EFixedUtc);
		}
	}

/** Checks the start, end and time of an instance
@param	aFetchInstanceArray Array of instances 
@param	aIndex Used to retrieve the correct instance from array
@test
*/
void CTestCalInterimApiTZChange::CheckInstancesTimesL(RPointerArray<CCalInstance>& aFetchInstanceArray, TInt aIndex)
	{
	TDateTime startTime;
	TDateTime endTime;
	CCalInstance* instance = NULL;
	
	GetInstanceDataFromIniFileL(aIndex);
	
	if (iIsFloating)
		{
		instance = aFetchInstanceArray[aIndex];
		startTime = instance->StartTimeL().TimeLocalL().DateTime();
		INFO_PRINTF7(_L("Instance Start Date Year: %d, Month: %d, Day: %d, Instance Start Time Hr: %d, Min: %d, Sec: %d "), startTime.Year(), startTime.Month(), startTime.Day(), startTime.Hour(), startTime.Minute(), startTime.Second());
		TESTL(instance->StartTimeL().TimeLocalL() == instance->Time().TimeLocalL());
		INFO_PRINTF1(_L("Start and Time of instance match"));
		TESTL(FormatDateTime(iPtrExpectedInstanceStart) == instance->StartTimeL().TimeLocalL());
		endTime = instance->EndTimeL().TimeLocalL().DateTime();
		INFO_PRINTF7(_L("Instance End Date Year: %d, Month: %d, Day: %d, Instance Start Time Hr: %d, Min: %d, Sec: %d "), endTime.Year(), endTime.Month(), endTime.Day(), endTime.Hour(), endTime.Minute(), endTime.Second());
		TESTL(FormatDateTime(iPtrExpectedInstanceEnd) == instance->EndTimeL().TimeLocalL());
		TESTL(instance->StartTimeL().TimeMode() == TCalTime::EFloating); // defect stored as fixedutc
		TESTL(instance->Time().TimeMode() == TCalTime::EFloating);
		TESTL(instance->EndTimeL().TimeMode() == TCalTime::EFloating);
		}
	else
		{
		instance = aFetchInstanceArray[aIndex];
		startTime = instance->StartTimeL().TimeUtcL().DateTime();
		INFO_PRINTF7(_L("Instance Start Date Year: %d, Month: %d, Day: %d, Instance Start Time Hr: %d, Min: %d, Sec: %d "), startTime.Year(), startTime.Month(), startTime.Day(), startTime.Hour(), startTime.Minute(), startTime.Second());
		TESTL(instance->StartTimeL().TimeUtcL() == instance->Time().TimeUtcL());
		INFO_PRINTF1(_L("Start and Time of instance match"));
		TESTL(FormatDateTime(iPtrExpectedInstanceStart) == instance->StartTimeL().TimeUtcL());
		endTime = instance->EndTimeL().TimeUtcL().DateTime();
		INFO_PRINTF7(_L("Instance End Date Year: %d, Month: %d, Day: %d, Instance Start Time Hr: %d, Min: %d, Sec: %d "), endTime.Year(), endTime.Month(), endTime.Day(), endTime.Hour(), endTime.Minute(), endTime.Second());
		TESTL(FormatDateTime(iPtrExpectedInstanceEnd) == instance->EndTimeL().TimeUtcL());
		TESTL(instance->StartTimeL().TimeMode() == TCalTime::EFixedUtc);
		TESTL(instance->Time().TimeMode() == TCalTime::EFixedUtc);
		TESTL(instance->EndTimeL().TimeMode() == TCalTime::EFixedUtc);
		}	
	}

/** Gets data from the ini file TestCalTZChanges.ini
@param	aIndex to indicate which expected times to retrieve from ini file
@test
*/
void CTestCalInterimApiTZChange::GetInstanceDataFromIniFileL(TInt aIndex)
	{
	iField.Zero();
	iField.Append(KExpectedInstanceStart);
	iField.AppendNum(aIndex);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedInstanceStart)); // gets expected instance's start time
	
	iField.Zero();
	iField.Append(KExpectedInstanceEnd);
	iField.AppendNum(aIndex);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedInstanceEnd));	// gets expected instance's end time
	}

/** Checks the start, end and time of an entry
@param	aFetchInstanceArray Array of instances 
@test
*/
void CTestCalInterimApiTZChange::CheckEntryTimesL(RPointerArray<CCalInstance>& aFetchInstanceArray)
	{
	CCalEntry& entry = aFetchInstanceArray[0]->Entry();
	
	const TDesC8& entryUid = entry.UidL();
	
	for ( TInt index = 0; index < aFetchInstanceArray.Count(); index++ )
		{
		const TDesC8& entryUid1 = aFetchInstanceArray[index]->Entry().UidL();
		TESTL(!entryUid.Compare(entryUid1));
		}
		
	GetEntryDataFromIniFileL();
	
	TDateTime startTime;
	TDateTime endTime;
	
	if (iIsFloating)
		{
		startTime = entry.StartTimeL().TimeLocalL().DateTime();
		INFO_PRINTF7(_L("Entry Start Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), startTime.Year(), startTime.Month(), startTime.Day(), startTime.Hour(), startTime.Minute(), startTime.Second());
		TESTL(FormatDateTime(iPtrExpectedEntryStart) == entry.StartTimeL().TimeLocalL());
		endTime = entry.EndTimeL().TimeLocalL().DateTime();
		INFO_PRINTF7(_L("Entry End Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), endTime.Year(), endTime.Month(), endTime.Day(), endTime.Hour(), endTime.Minute(), endTime.Second());
		TESTL(FormatDateTime(iPtrExpectedEntryEnd) == entry.EndTimeL().TimeLocalL());
		TESTL(entry.StartTimeL().TimeMode() == TCalTime::EFloating);
		TESTL(entry.EndTimeL().TimeMode() == TCalTime::EFloating);
		}
	else
		{
		startTime = entry.StartTimeL().TimeUtcL().DateTime();
		INFO_PRINTF7(_L("Entry Start Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), startTime.Year(), startTime.Month(), startTime.Day(), startTime.Hour(), startTime.Minute(), startTime.Second());
		TESTL(FormatDateTime(iPtrExpectedEntryStart) == entry.StartTimeL().TimeUtcL());
		endTime = entry.EndTimeL().TimeUtcL().DateTime();
		INFO_PRINTF7(_L("Entry End Date Year: %d, Month: %d, Day: %d, Imported Time Hr: %d, Min: %d, Sec: %d "), endTime.Year(), endTime.Month(), endTime.Day(), endTime.Hour(), endTime.Minute(), endTime.Second());
		TESTL(FormatDateTime(iPtrExpectedEntryEnd) == entry.EndTimeL().TimeUtcL());
		TESTL(entry.StartTimeL().TimeMode() == TCalTime::EFixedUtc);
		TESTL(entry.EndTimeL().TimeMode() == TCalTime::EFixedUtc);
		}
		
	INFO_PRINTF1(KEntryTimesCorrect);	
	
	CCalAlarm* alarm1 = entry.AlarmL();
	TESTL(alarm1->TimeOffset().Int() == iIntAlarmOffset);
	delete alarm1;
	INFO_PRINTF1(KAlarmOffsetTZ);	
	}

/** Gets data from the ini file TestCalTZChanges.ini
@test
*/
void CTestCalInterimApiTZChange::GetEntryDataFromIniFileL()
	{
	iField = KExpectedEntryStart;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedEntryStart)); // gets expected entry's start time
	
	iField = KExpectedEntryEnd;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrExpectedEntryEnd)); // gets expected entry's end time
	
	iField = KExpectedEntryAlarmOffset;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iIntAlarmOffset)); // gets expected entry's alarm offset
	}
	
/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/
// Class CTestCalInterimDSTChange - WAits for one second to enable DST change
CTestCalInterimApiDSTChange::CTestCalInterimApiDSTChange()
	{
	SetTestStepName(KTestCalInterimApiDSTChange);
	}

CTestCalInterimApiDSTChange::~CTestCalInterimApiDSTChange()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test
*/
TVerdict CTestCalInterimApiDSTChange::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiDSTChange::doTestStepL() start"));
	INFO_PRINTF1(_L("Waiting for DST change......."));
	
	PrintHomeTimeL();
	User::After(KTimeDelayForDSTChange);
	PrintHomeTimeL();
	
	INFO_PRINTF1(_L("DST Starts"));
	INFO_PRINTF1(_L("CTestCalInterimApiDSTChange::doTestStepL() finish"));
	return TestStepResult();
	}


