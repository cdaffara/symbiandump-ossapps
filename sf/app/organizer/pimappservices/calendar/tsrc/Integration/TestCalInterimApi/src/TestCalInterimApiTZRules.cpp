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

#include "TestCalInterimApiTZRules.h"
#include <tz.h>
#include <vtzrules.h>
#include <calrrule.h>

// Class CTestCalInterimApiTZRulesCommon  - This class feches a paticular time zone rule specified by the time zone in ini file
CTestCalInterimApiTZRulesCommon::CTestCalInterimApiTZRulesCommon()
: 	iRules(NULL)
	{
	}

CTestCalInterimApiTZRulesCommon::~CTestCalInterimApiTZRulesCommon()
	{
	delete iRules;
	}

/** Get time zone rules for a particular time zone (specified by iPtrTimeZone - taken from ini file)
@test
*/
void CTestCalInterimApiTZRulesCommon::GetTZRuleL()
	{
	RTz tz;
	CleanupClosePushL(tz);
	User::LeaveIfError(tz.Connect());

	CTzId* id = NULL;

	// iPtrTimeZone is determined from the ini file.  Specifies which time zone rule to fetch
	HBufC8* timeZone8 = HBufC8::NewLC(iPtrTimeZone.Length());
	timeZone8->Des().Copy(iPtrTimeZone);
	id = CTzId::NewL(timeZone8->Des());
	CleanupStack::PushL(id);

	iField = KTZRuleStartDate;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrTZRuleStartDate)); // gets expected start date for filter

	iField = KTZRuleEndDate;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrTZRuleEndDate)); // gets expected end date for filter

	iRules = tz.GetTimeZoneRulesL(*id, iStartdate.DateTime(), iEnddate.DateTime(), ETzUtcTimeReference);
	// iRules will be used to set the time zone rule
	
	CleanupStack::PopAndDestroy(id);
	CleanupStack::PopAndDestroy(timeZone8);
	CleanupStack::PopAndDestroy(&tz);

	//convert to UTC for setting the calendar entry, because this is a foreign rule
	iRules->ConvertToUtcL(iStartdate);
	iRules->ConvertToUtcL(iEnddate);
	}
	
/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiCreateRptTZRule  - This class creates a repeating entry, with a TZ rule.
// The TZ rule is either created by using the local time zone or by fecthing a time zone rule
CTestCalInterimApiCreateRptTZRule::CTestCalInterimApiCreateRptTZRule()
: 	iCalEntry(NULL)
,	iEntryType(CCalEntry::EAppt)
, 	iEntryView(NULL)
	{
	SetTestStepName(KTestCalInterimApiCreateRptTZRule);
	}

CTestCalInterimApiCreateRptTZRule::~CTestCalInterimApiCreateRptTZRule()
	{
	delete iCalEntry;
	delete iEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test  doTestStepL - Creates a repeating entry, with a TZ rule which is either set by the local time
zone or by fecthing a particular time zone rule (specified in ini file)
*/
TVerdict CTestCalInterimApiCreateRptTZRule::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiCreateRptTZRule::doTestStepL() start"));

	iField.Zero();
	iField.Append(KSoakTest);
	GetIntFromConfig(ConfigSection(),iField,iTZCallRepeatCount); // gets the number of times to call SetTZRule Api

	GetDataFromIni();

	if (!iBoolSetTZLocal) // If utc, then able to call SetTzRulesL(*iRules)
		{
		GetTZRuleL(); // determines iRules

		SetStartAndEndL(EFalse); // set local times, determined by EFalse
		SetRepeatL();

		if (iTZCallRepeatCount != 0)
			{
			// Soak testing, sets time zone rules multiple times
			for (TInt index = 0; index < iTZCallRepeatCount; index++)
				{
				INFO_PRINTF2(_L("SetTzRules(*rules) calls: %d"), index+1);
				iCalEntry->SetTzRulesL(*iRules); // iRules determined by function GetTZRuleL
				}
			}
		else
			{
			iCalEntry->SetTzRulesL(*iRules); // sets time zone rule (iRules contains the rules fetched for a 
			}								 // particular time zone - determined in ini file)
		}
	else // if utc times
		{
		SetStartAndEndL(ETrue); // Set utc times, determined by ETrue
		SetRepeatL();

		if (iTZCallRepeatCount != 0)
			{
			//soak testing, determined by ini file, sets time zone rules multiple times
			for (TInt index = 0; index < iTZCallRepeatCount; index++)
				{
				INFO_PRINTF2(_L("SetTzRules(*rules) calls: %d"), index+1);
				iCalEntry->SetTzRulesL();
				}
			}
		else
			{
			iCalEntry->SetTzRulesL(); // sets time zone rules using the local time zone
			}
		}

	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray); // reason for doing this,is because I want to own iCalEntry
								  // and not pass ownership to entryarray

	entryArray.AppendL(iCalEntry);
	TInt entriesAdded(0);

	iEntryView=CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	iEntryView->StoreL(entryArray, entriesAdded);

	CleanupStack::PopAndDestroy(&entryArray);// entryArray.reset & entryArray.destroy;

	INFO_PRINTF1(_L("CTestCalInterimApiCreateRptTZRule::doTestStepL() finish"));
	return TestStepResult();
	}

/** Gets data from ini file TestCalTZRules.ini
@test
*/
void CTestCalInterimApiCreateRptTZRule::GetDataFromIni()
	{
	iField.Zero();
	iField.Append(KLocalTimeZone);
	GetStringFromConfig(ConfigSection(),iField,iPtrTimeZone); // gets time zone to fetch rules for

	iField.Zero();
	iField.Append(KEntryUid);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrEntryUid)); // gets the entries uid

	iField.Zero();
	iField.Append(KSetTZRule);
	GetBoolFromConfig(ConfigSection(),iField,iBoolSetTZLocal); // gets the number of calls for SetTZRule Api

	iField.Zero();
	iField.Append(KEntryType);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrEntryType)); // gets the entries entry type

	SetEntryType();
	}

/** Sets the start and end for entry
@param	aLocal True or False to indicate whether to set times in local or utc
@test
*/
void CTestCalInterimApiCreateRptTZRule::SetStartAndEndL(TBool aLocal)
	{
	iEntryId = HBufC8::NewLC(iPtrEntryUid.Length());
	iEntryId->Des().Copy(iPtrEntryUid);

	iCalEntry = CCalEntry::NewL(iEntryType, iEntryId, CCalEntry::EMethodAdd, 0);

	CleanupStack::Pop(iEntryId);

	iField = KStartDate;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrStartDate)); // gets start date for entry

	iField = KEndDate;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrEndDate)); // gets end date for entry

	iStartdate.Set(iPtrStartDate);

	iEnddate.Set(iPtrEndDate);

	if (aLocal)
		{
		iStartCalTime.SetTimeLocalL(iStartdate);
		iEndCalTime.SetTimeLocalL(iEnddate);
		}
	else
		{
		SetCalTimeL(iStartdate, iStartCalTime, TCalTime::EFixedUtc);
		SetCalTimeL(iEnddate, iEndCalTime, TCalTime::EFixedUtc);
		}

	iCalEntry->SetStartAndEndTimeL(iStartCalTime, iStartCalTime);
	}

/** Sets repeat definition for entry
@test
*/
void CTestCalInterimApiCreateRptTZRule::SetRepeatL()
	{
	iField = KInterval;
	TESTL(GetIntFromConfig(ConfigSection(),iField,iInterval)); // gets interval for repeat rule

	iField = KRepeatType;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrRepeatType)); // gets repeat type

	iField = KRepeatDay;
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrDay)); // gets day to repeat

	if (iPtrRepeatType == KRepeatMonthly)
		{
		iType=TCalRRule::EMonthly;
		}
	else
		{
		iType=TCalRRule::EWeekly;
		}

	//create repeat rule
	TCalRRule rpt(iType);
	rpt.SetDtStart(iStartCalTime);
	rpt.SetUntil(iEndCalTime);
	rpt.SetInterval(iInterval);

	TDay day;
	if (iPtrDay == KRepeatTuesday)
		{
		day = ETuesday;
		}

	RArray<TDay> days;
	days.AppendL(day);
	rpt.SetByDay(days);

	iCalEntry->SetRRuleL(rpt);
	}

/** Sets the entries type  (specified by iPtrEntryType - taken from ini file)
@test
*/
void CTestCalInterimApiCreateRptTZRule::SetEntryType()
	{
	if ( iPtrEntryType.Compare(KEntryToDo) == 0 )
		{
		iEntryType = CCalEntry::ETodo;
		}
	else if ( iPtrEntryType.Compare(KEntryEvent) == 0 )
		{
		iEntryType = CCalEntry::EEvent;
		}
	else if ( iPtrEntryType.Compare(KEntryReminder) == 0 )
		{
		iEntryType = CCalEntry::EReminder;
		}
	else if ( iPtrEntryType.Compare(KEntryAppt) == 0 )
		{
		iEntryType = CCalEntry::EAppt;
		}
	else if ( iPtrEntryType.Compare(KEntryAnniversary) == 0 )
		{
		iEntryType = CCalEntry::EAnniv;
		}
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiSetTZRuleEntry  - This class creates a non-repeating entry in local/utc times and attempts to set Time Zone rule
CTestCalInterimApiSetTZRuleEntry::CTestCalInterimApiSetTZRuleEntry()
	{
	SetTestStepName(KTestCalInterimApiSetTZRuleEntry);
	}

CTestCalInterimApiSetTZRuleEntry::~CTestCalInterimApiSetTZRuleEntry()
	{
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test doTestStepL - this verifies that when attempting to set time zone rule either by using the local time zone 
or by fecthing a time zone rule on a non-repeating entry should fail
*/
TVerdict CTestCalInterimApiSetTZRuleEntry::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiSetTZRuleEntry::doTestStepL() start"));

	GetDataFromIni();
	
	if (iBoolSetTZLocal)
		{
		SetStartAndEndL(ETrue); // sets time in local times
		}
	else
		{
		SetStartAndEndL(EFalse); // sets time in utc times
		}

	if (iBoolSetTZLocal) 
		{
		TRAPD(err, iCalEntry->SetTzRulesL()); // attempting to set time zone rule (using local time zone) on non-repeating should fail
		
		if (err == KErrNone)
			{
			ERR_PRINTF1(_L("Time Zone Rule Set on non-repeating entry"));
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		else
			{
			INFO_PRINTF1(KInfoSetTZRules);
			}
		}
	else 
		{
		GetTZRuleL(); // fetches a time zone rule for a particular time zone

		TRAPD(err, iCalEntry->SetTzRulesL(*iRules)); // attempting to set time zone rule on non-repeating should fail

		if (err == KErrNone)
			{
			ERR_PRINTF1(_L("Time Zone Rule Set on non-repeating entry"));
			SetTestStepResult(EFail);
			SetTestStepError(err);
			}
		else
			{
			INFO_PRINTF1(KInfoSetTZRules);
			}
		}

	INFO_PRINTF1(_L("CTestCalInterimApiSetTZRuleEntry::doTestStepL() finish"));
	return TestStepResult();
	}

/*------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------*/

// Class CTestCalInterimApiVerifyTZRules  - This class compares two TZ rules
CTestCalInterimApiVerifyTZRules::CTestCalInterimApiVerifyTZRules()
: 	iEntryView(NULL)
	{
	SetTestStepName(KTestCalInterimApiVerifyTZRules);
	}

CTestCalInterimApiVerifyTZRules::~CTestCalInterimApiVerifyTZRules()
	{
	delete iEntryView;
	}

/**
Base class pure virtual.
@return		EPass or EFail indicating the result of the test step.
@test  Compares two time zone rules - from the entry itself together with a time zone rule for a particular time zone
which is specified in ini file
*/
TVerdict CTestCalInterimApiVerifyTZRules::doTestStepL()
	{
	TESTL(TestStepResult() == EPass);
	INFO_PRINTF1(_L("CTestCalInterimApiVerifyTZRules::doTestStepL() start"));

	iField.Zero();
	iField.Append(KLocalTimeZone);
	GetStringFromConfig(ConfigSection(),iField,iPtrTimeZone); // gets time zone to fetch rules for

	iField.Zero();
	iField.Append(KEntryUid);
	TESTL(GetStringFromConfig(ConfigSection(),iField,iPtrEntryUid)); // gets the entries uid

	HBufC8*	entryGuid = HBufC8::NewLC(iPtrEntryUid.Length());
	entryGuid->Des().Copy(iPtrEntryUid);

	//Gets time zone rules for time zone specified by iPtrTimeZone
	GetTZRuleL();

	//Getting time zone rule from entry
	RPointerArray<CCalEntry> fetchEntryArray;
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyEntryArray, &fetchEntryArray));

	iEntryView = CCalEntryView::NewL(GetSession(), *this);

	CActiveScheduler::Start();

	TRAPD(err, iEntryView->FetchL(entryGuid->Des(), fetchEntryArray));

	if (err==KErrNone)
		{
		INFO_PRINTF1(KInfoFecthedEntryDatabase);
		}
	else
		{
		ERR_PRINTF1(_L("Entry is not fetched successfully"));
		SetTestStepResult(EFail);
		SetTestStepError(err);
		User::Leave(err);
		}
	CCalEntry* entry;
	entry = fetchEntryArray[0];

	CTzRules* rule1 = entry->TzRulesL();
	CleanupStack::PushL(rule1);

	//compares both TZ rules retrieved
	CompareTZRulesL(iRules, rule1);

	CleanupStack::PopAndDestroy(rule1);
	CleanupStack::PopAndDestroy(&fetchEntryArray);// entryArray.reset & entryArray.destroy;
	CleanupStack::PopAndDestroy(entryGuid);

	INFO_PRINTF1(_L("CTestCalInterimApiVerifyTZRules::doTestStepL() finish"));
	return TestStepResult();
	}

/** Compares two time zone rules
@param	aRule The time zone rule to compare with the second time zone rule aRule1
@param	aRule1 The time zone rule to compare with the first time zone rule aRule
@test
*/
void CTestCalInterimApiVerifyTZRules::CompareTZRulesL(CTzRules *aRule, CTzRules *aRule1)
	{
	TESTL(aRule->Count() == aRule1->Count());

	// Compare rules stored (aRule1) with rules retrieved (aRule)
	TInt count = aRule->Count();
	TTzRule oneRule;
	TTzRule anotherRule;
	for (TInt i = 0; i < count; i++)
		{
		oneRule = (*aRule)[i];
		anotherRule = (*aRule1)[i];

		TESTL(oneRule.iFrom == anotherRule.iFrom);
		TESTL(oneRule.iTo == anotherRule.iTo);
		TESTL(oneRule.iOldLocalTimeOffset == anotherRule.iOldLocalTimeOffset);
		TESTL(oneRule.iNewLocalTimeOffset == anotherRule.iNewLocalTimeOffset);
		TESTL(oneRule.iMonth == anotherRule.iMonth);
		TESTL(oneRule.iDayRule == anotherRule.iDayRule);
		TESTL(oneRule.iDayOfMonth == anotherRule.iDayOfMonth);
		TESTL(oneRule.iDayOfWeek == anotherRule.iDayOfWeek);
		TESTL(oneRule.iTimeReference == anotherRule.iTimeReference);
		TESTL(oneRule.iTimeOfChange == anotherRule.iTimeOfChange);
		}
	}



