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
#include <calrrule.h>
#include <calsession.h>
#include <e32test.h>
#include <vtzrules.h>
#include <calalarm.h>
#include <s32file.h>
#include <caliterator.h>
#include <caldataexchange.h>
#include <caldataformat.h>

RTest test(_L("tcal_tz"));

_LIT(KCalendarFile, "tcal_tz");

// Timezone constants

_LIT8(KEuropeLondon, "Europe/London");
_LIT8(KAsiaTokyo, "Asia/Tokyo");
_LIT8(KAsiaTaiwan, "Asia/Taiwan");
_LIT8(KAsiaBangkok, "Asia/Bangkok");
_LIT8(KEuropeStockholm, "Europe/Stockholm");
_LIT8(KEuropeHelsinki, "Europe/Helsinki");
_LIT8(KUsNewYork, "America/New_York");
_LIT8(KUsCentral, "US/Central");
_LIT8(KUsLosAngeles, "US/LosAngeles");
_LIT8(KEuropeTirane, "Europe/Tirane");
_LIT8(KPacificHonolulu, "Pacific/Honolulu");

// Watch out for confusion between this and
// the custom rule we create that matches this zone.
_LIT8(KPacificAuckland, "Pacific/Auckland");

// Time constants

const TDateTime KTokyoEntryTime(2005,EMarch,6,2,45,15,0);
const TDateTime KExpectedTokyoEntryTime(2005,EMarch,7,2,45,15,0);


class CTzTestManager : public CActive
	{
public:
	static CTzTestManager* NewLC();
	~CTzTestManager();

	void Start();

private:
	CTzTestManager();
	void ConstructL();
	
	void TestSetTimeZoneL(const TDesC8& aTimeZoneName);
	void TestSetAndGetTzRulesL();
	void TestRemoteRepeatL();
	void TestRepeatRuleTzSetupL(TBool aFloating, TBool aException = EFalse, TBool aRDate = EFalse);
	void TestRepeatRuleLocalTimeL();
	void TestRepeatRuleTzStartEndTimeL();
	void TestRepeatRuleTzStartEndTzRulesL();
	void TestRepeatRuleTzRulesL();
	void DefectTestL();
	void TestRDatesTzSetupL();
	void TestFloatingEntrySetupL(); // Create a floating entry in calendar
	void TestFloatingEntryCheckL(const TTime& aExpectedTime); // Check the previously stored floating entry
	void TestFloatingRepeatCheckL(); // Check the previously stored floating repeating entry
	void TestFloatingExceptionCheckL(); // Check the previously stored floating repeating entry with exception date
	void TestFloatingRDateCheckL(); // Check the previously stored floating repeating entry with RDate

	void TestForeignZoneRepeatRule1L();
	void TestForeignZoneRepeatRule2L();
	void TestLocalRepeatRuleL();
	void TestLocalRepeatRule2L();
	void TestLocalTimeZoneL();
	
	void TestAlarmTzUpdateSetupL();
	void TestAlarmTzUpdateCheckL();
	void SetupEventCrossingMidnightL();
	void CheckEventCrossingMidnightL();
	void CheckDSTThresholdL();
	
	void TestCopyRepeatRulesL();
	void TestExportTzRulesInHemisphereL(TBool aSouthernHemisphere);
	void ImportFloatingEntryL();
	void TestTimeModeL();
	void TestAlarmOffsetL();
	void CheckVcalImportFromS40L();
	void CheckUtcOffsetL(TCalTime& aTestLocalTime, TInt aExpectedOffset);

	void CheckEntryInTimeZoneL(const TTime& aExpectedTime);
	
	void TestCreateEntryWithMidnightInHomeTimeL(TBool aFloating = EFalse);
	void TestCreateEntryWithDstChangeL(TBool aFloating = EFalse);	
	
	// no copy constructor and assignment operator
	CTzTestManager(CTzTestManager& );
	CTzTestManager& operator = (const CTzTestManager& );

	void CheckIsAnInstanceInTimeModeL(const TTime& aLocalTime, TBool aFound = ETrue, TCalTime::TTimeMode aTimeMode = TCalTime::EFixedTimeZone);
	void CheckIsAnInstanceInSpecifiedZoneL(const TTime& aTime);
	void VerifyRulesL(const CTzRules& aRules,const TCalTime& aStartRange, const TCalTime& aEndRange);

	// from CActive
	void RunL();
	void DoCancel();

private:
	CCalTestLibrary*	iTestLibrary; // need a connection to a session to create a CCalEntry
	CCalEntry*			iCalEntry;
	TBuf8<256>			iCurrentTimeZone;
	TInt 				iTestStep;
	};


// Constructor/Destructor
CTzTestManager::CTzTestManager() : CActive(EPriorityLow)
	{
	CActiveScheduler::Add(this);
	}

CTzTestManager* CTzTestManager::NewLC()
	{
	CTzTestManager* self = new (ELeave) CTzTestManager;
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}


void CTzTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	iTestLibrary->ReplaceFileL(KCalendarFile());
	iTestLibrary->OpenFileL(KCalendarFile());

	iTestLibrary->PIMTestServer().GetTimeZoneL(iCurrentTimeZone);
	}
	

CTzTestManager::~CTzTestManager()
	{
	if (iTestLibrary && iCurrentTimeZone.Length())
		{
		TRAP_IGNORE(TestSetTimeZoneL(iCurrentTimeZone));
		}
		
	delete iCalEntry;
	delete iTestLibrary;
	}

void CTzTestManager::Start()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	if ( ! IsActive())
		{
		SetActive();
		}
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Test cases forming the Time Zone test suite
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


void CTzTestManager::TestSetTimeZoneL(const TDesC8& aTimeZoneName)
	{
	iTestLibrary->SetTimeZoneL(aTimeZoneName);
	}


// TestRulesProvided: Creates a collection of TTzRule's and with it a CAgnTzZone. 
// Then it tests the time conversion methods in CAgnTzZone.
/** @SYMTestCaseID     PIM-TCAL-TZ-0002 */
void CTzTestManager::TestSetAndGetTzRulesL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0002 Running tcal_tz - TestSetAndGetTzRulesL")); 
	HBufC8* dummy = NULL;
	
	delete iCalEntry;
	iCalEntry = NULL;
	iCalEntry = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, dummy);
	
	// Create arbitrary tz rule - add one hour between March 81 and October 96
	TTzRule tRule1(1981,1996,0,60,EMarch,ETzDayInLastWeekOfMonth,0,ESunday,ETzUtcTimeReference,60);
	TTzRule tRule2(1981,1996,60,0,EOctober,ETzDayInLastWeekOfMonth,0,ESunday,ETzUtcTimeReference,60);
	TTzRule tRule3(0,KMaxTUint16,0,0,EJanuary,ETzFixedDate,0,0,ETzWallTimeReference,0);	
	CTzRules* tzRules = CTzRules::NewL(2003,2007);
	CleanupStack::PushL(tzRules); // PUSH
	tzRules->AddRuleL(tRule1);
	tzRules->AddRuleL(tRule2);
	tzRules->AddRuleL(tRule3);	

	// create weekly repeat rule every Tuesday from 1 May 05 to 1 Dec 05
	TDateTime dtStart(2005,EMay,0,10,0,0,0);
	TDateTime dtEnd(2005,EDecember,0,10,0,0,0);
	TTime dtStart1(dtStart);
	TTime dtEnd1(dtEnd);
	TCalTime startCalTime;
	TCalTime endCalTime;
	tzRules->ConvertToUtcL(dtStart1);
	startCalTime.SetTimeUtcL(dtStart1);
	tzRules->ConvertToUtcL(dtEnd1);
	startCalTime.SetTimeUtcL(dtEnd1);

	iCalEntry->SetStartAndEndTimeL(startCalTime, startCalTime);
	
	TCalRRule calRule(TCalRRule::EWeekly);
	calRule.SetDtStart(startCalTime);
	calRule.SetUntil(endCalTime);
	calRule.SetInterval(1);
	RArray<TDay> days;
	days.AppendL(ETuesday);
	calRule.SetByDay(days);
	iCalEntry->SetRRuleL(calRule);
	days.Reset();

	
	iCalEntry->SetTzRulesL(*tzRules);
	
	CTzRules* tzRules2 = iCalEntry->TzRulesL();
	CleanupStack::PushL(tzRules2);
	
	test(tzRules->Count() == tzRules2->Count());
	
	// Compare rules stored (tzRules) with rules retrieved (tzRules2)
	TInt count = tzRules->Count();
	TTzRule oneRule;
	TTzRule anotherRule;
	for (TInt i = 0; i < count; i++)
		{
		oneRule = (*tzRules)[i];
		anotherRule = (*tzRules2)[i];
		
		test(oneRule.iFrom == anotherRule.iFrom);
		test(oneRule.iTo == anotherRule.iTo);
		test(oneRule.iOldLocalTimeOffset == anotherRule.iOldLocalTimeOffset);
		test(oneRule.iNewLocalTimeOffset == anotherRule.iNewLocalTimeOffset);
		test(oneRule.iMonth == anotherRule.iMonth);
		test(oneRule.iDayRule == anotherRule.iDayRule);
		test(oneRule.iDayOfMonth == anotherRule.iDayOfMonth);
		test(oneRule.iDayOfWeek == anotherRule.iDayOfWeek);
		test(oneRule.iTimeReference == anotherRule.iTimeReference);
		test(oneRule.iTimeOfChange == anotherRule.iTimeOfChange);
		}

	CleanupStack::PopAndDestroy(tzRules2);
	CleanupStack::PopAndDestroy(tzRules);
	delete iCalEntry;
	iCalEntry = NULL;
	}

	
void CTzTestManager::CheckEntryInTimeZoneL(const TTime& aExpectedTime)
	{
	// Check that instance dates and times are properly converted to local times
	TTime newTime = iCalEntry->StartTimeL().TimeLocalL();
	test(newTime == aExpectedTime);
	}


// Tests that the start time of an entry changes correctly in different time zones
// (using zone extracted from TZ server)
/** @SYMTestCaseID     PIM-TCAL-TZ-0003 */
void CTzTestManager::TestRemoteRepeatL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0003 Running tcal_tz - TestRemoteRepeatL")); 	
	HBufC8* dummy = NULL;
	
	delete iCalEntry;
	iCalEntry = NULL;
	iCalEntry = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, dummy);
	
	// create rule repeating every Monday from 7 Mar 05 to 7 Dec 05
	TDateTime dtEnd(2005,EDecember,6,10,0,0,0);
	TCalTime startCalTime;
	TCalTime endCalTime;
	// OK to use SetTimeLocal, since we are using SetTzRulesL();
	startCalTime.SetTimeLocalL(KTokyoEntryTime);
	endCalTime.SetTimeLocalL(dtEnd);

	iCalEntry->SetStartAndEndTimeL(startCalTime, startCalTime);

	TCalRRule calRule(TCalRRule::EWeekly);
	calRule.SetDtStart(startCalTime);
	calRule.SetUntil(endCalTime);
	calRule.SetInterval(1);
	RArray<TDay> days;
	days.AppendL(EMonday);
	calRule.SetByDay(days);
	days.Reset();
	
	iCalEntry->SetRRuleL(calRule);	
	iCalEntry->SetTzRulesL();
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0004 */
void CTzTestManager::TestFloatingEntrySetupL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0004 Running tcal_tz - TestFloatingEntrySetupL")); 
	iTestLibrary->CleanDatabaseL();

	delete iCalEntry;
	iCalEntry = NULL;
	
	HBufC8* dummy = NULL;
	iCalEntry = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, dummy);
	
	// create rule repeating every Tuesday from 7 Mar 05 to 7 Dec 05
	TDateTime dtEnd(2005,EDecember,6,10,0,0,0);
	TCalTime startCalTime;
	TCalTime endCalTime;
	startCalTime.SetTimeLocalFloatingL(KTokyoEntryTime);
	endCalTime.SetTimeLocalFloatingL(dtEnd);
	
	iCalEntry->SetStartAndEndTimeL(startCalTime, startCalTime);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	
	
	entryArray.AppendL(iCalEntry);
	TInt entriesAdded = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entryArray, entriesAdded);


	CleanupStack::PopAndDestroy(&entryArray);	
	}


void CTzTestManager::TestFloatingEntryCheckL(const TTime& aExpectedTime)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
		

	iTestLibrary->SynCGetEntryViewL().FetchL(iCalEntry->UidL(), entryArray);

	// Check that entry dates and times are properly converted to local times
	test(entryArray.Count() == 1);
	TTime newTime = entryArray[0]->StartTimeL().TimeLocalL();
	test(newTime == aExpectedTime);
	
	
	CleanupStack::PopAndDestroy(&entryArray);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0005 */
void CTzTestManager::TestRepeatRuleTzSetupL(TBool aFloating, TBool aException, TBool aRDate)
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0005 Running tcal_tz - TestRepeatRuleTzL")); 	
	CCalEntryView& entryView = iTestLibrary->SynCGetEntryViewL();
	if (aFloating)
		{
		entryView.DeleteL(*iCalEntry);
		}
		
	delete iCalEntry;
	iCalEntry = NULL;
	HBufC8* dummy = NULL;
	iCalEntry = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, dummy);
	
	TTime dtStart(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0));
	TTime dtEnd(TDateTime(2005, EDecember, 30, 9, 0, 0, 0));
	TCalTime startCalTime;
	TCalTime endCalTime;
	
	if (aFloating)
		{
		// Create a floating repeat entry in Auckland timezone
		// create rule repeating every 2 days from 9am (local time)
		startCalTime.SetTimeLocalFloatingL(dtStart);
		endCalTime.SetTimeLocalFloatingL(dtEnd);
		
		TCalRRule rpt(TCalRRule::EDaily);
		rpt.SetDtStart(startCalTime);
		rpt.SetUntil(endCalTime);
		rpt.SetInterval(2);

		iCalEntry->SetStartAndEndTimeL(startCalTime, startCalTime);

		// add the repeat rule and the timezone rules to the entry
		iCalEntry->SetRRuleL(rpt);
	
		TRAPD(err, iCalEntry->SetTzRulesL());
		test(err == KErrArgument);
		}
	else
		{
		// set time zone to Auckland, NZ
		// DST ends on Sunday, 20 March 2005, 03:00 local daylight time 
		// DST starts on Sunday, 2 October 2005, 02:00 local standard time
		// create rule repeating every 2 days from 9am (NZ time)
		
		// fetch foreign rules from TZ server for Auckland
		RTz tz;
		User::LeaveIfError(tz.Connect());
		CTzId* id = CTzId::NewL(KPacificAuckland);
		CleanupStack::PushL(id);
	
		CTzRules* rules = tz.GetTimeZoneRulesL(*id, TDateTime(2000, EJanuary, 0, 0, 0, 0, 0), TDateTime(2010, EJanuary, 0, 0, 0, 0, 0), ETzUtcTimeReference);
		CleanupStack::PopAndDestroy(id);
		CleanupStack::PushL(rules);
		
		tz.Close();
		
		// convert to UTC for setting the calendar entry, because this is a foreign rule
		rules->ConvertToUtcL(dtStart);
		rules->ConvertToUtcL(dtEnd);
		startCalTime.SetTimeUtcL(dtStart);
		endCalTime.SetTimeUtcL(dtEnd);
		
		iCalEntry->SetStartAndEndTimeL(startCalTime, startCalTime);

		// create repeat rule
		TCalRRule rpt(TCalRRule::EDaily);
		rpt.SetDtStart(startCalTime);
		rpt.SetUntil(endCalTime);
		rpt.SetInterval(2);

		iCalEntry->SetRRuleL(rpt);
			
		iCalEntry->SetTzRulesL(*rules); // set tz rules to be local
		
		CleanupStack::PopAndDestroy(rules);
		}

	if (aException)
		{
		// Set 2 exceptions on the parent
		
		RArray<TCalTime> exArray;
		CleanupClosePushL(exArray);		
		
		TCalTime exTime1;
		exTime1.SetTimeLocalFloatingL(TDateTime(2005, EMarch, 23, 9, 0, 0, 0));	// March 24rd 9am
		exArray.Append(exTime1);

		TCalTime exTime2;
		exTime2.SetTimeLocalFloatingL(TDateTime(2005, ESeptember, 29, 9, 0, 0, 0));	// September 30th 9am
		exArray.Append(exTime2);

		iCalEntry->SetExceptionDatesL(exArray);
		
		RArray<TCalTime> exArray2;
		CleanupClosePushL(exArray2);
		
		iCalEntry->GetExceptionDatesL(exArray2);
		test(exArray.Count() == exArray2.Count());
		for (TInt i = 0; i < exArray.Count(); i++)
			{
			test(exArray[i].TimeMode() == exArray2[i].TimeMode());
			test(exArray[i].TimeLocalL() == exArray2[i].TimeLocalL());
			}
			
		CleanupStack::PopAndDestroy(); // reset exArray2
		
		CleanupStack::PopAndDestroy(); // reset exArray
		}

	if (aRDate)
		{
		// Set 2 sporadic dates on the parent

		RArray<TCalTime> rDateArray;
		CleanupClosePushL(rDateArray);

		TCalTime rDate1;
		rDate1.SetTimeLocalFloatingL(TDateTime(2004, EDecember, 2, 9, 0, 0, 0));
		rDateArray.Append(rDate1);

		TCalTime rDate2;
		rDate2.SetTimeLocalFloatingL(TDateTime(2006, EMarch, 23, 9, 0, 0, 0));
		rDateArray.Append(rDate2);
		
		iCalEntry->SetRDatesL(rDateArray);			
		CleanupStack::PopAndDestroy(); // reset rDateArray
		}
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	
	entryArray.AppendL(iCalEntry);
	TInt entriesAdded = 0;
	entryView.StoreL(entryArray, entriesAdded);
	
	CleanupStack::PopAndDestroy(&entryArray);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0006 */
void CTzTestManager::TestRepeatRuleLocalTimeL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0006 Running tcal_tz - TestRepeatRuleLocalTimeL")); 
	//
	// Check instances are correct in local London time
	//
	
	// Local time zone is London, Repeat Rule has TzRules
	// similar to Pacific/Auckland.
	// London DST starts on 
	//  Sunday, 27 March 2005, 01:00 local daylight time 
	// London DST ends on 
	//  Sunday, 30 October 2005, 02:00 local standard time
	
	// Jan 1th 9am NZ time = Dec 31 8pm UK time
	// NZ = UK+13 (NZ DST on, UK DST off)

	TDateTime time1(2005, EMarch, 16, 20, 0, 0, 0);	// March 18th 9am NZ time = March 17th 8pm UK time
													// NZ = UK+13 (NZ DST on, UK DST off)
													
	TDateTime time2(2005, EMarch, 22, 21, 0, 0, 0);	// March 24rd 9am NZ time = March 23nd 9pm UK time
													// NZ = UK+12 (NZ DST off, UK DST off)
													
	TDateTime time3(2005, EMarch, 28, 22, 0, 0, 0);	// March 30th 9am NZ time = March 29th 10pm UK time
													// NZ = UK+11 (NZ DST off, UK DST on)
	
	TDateTime time4(2005, ESeptember, 28, 22, 0, 0, 0);	// September 30th 9am NZ time = September 29th 10pm UK time
														// NZ = UK+11 (NZ DST off, UK DST on)
														
	TDateTime time5(2005, EOctober, 12, 21, 0, 0, 0);	// October 14th 9am NZ time = October 13th 9pm UK time
														// NZ = UK+12 (NZ DST on, UK DST on)
														
	TDateTime time6(2005, ENovember, 1, 20, 0, 0, 0);	// November 3rd 9am NZ time = November 2nd 8pm UK time
														// NZ = UK+13 (NZ DST on, UK DST off)
	
	
	CheckIsAnInstanceInTimeModeL(time1);
	CheckIsAnInstanceInTimeModeL(time2);
	CheckIsAnInstanceInTimeModeL(time3);
	CheckIsAnInstanceInTimeModeL(time4);
	CheckIsAnInstanceInTimeModeL(time5);
	CheckIsAnInstanceInTimeModeL(time6);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0007 */
void CTzTestManager::TestRepeatRuleTzRulesL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0007 Running tcal_tz - TestRepeatRuleTzRulesL")); 	
	//
	// Check instances are correct in entry's timezone
	//
	
	// Local time zone is Europe/London, Repeat Rule has TzRules
	// similar to Pacific/Auckland.
	
	TDateTime time1(2005, EMarch, 17, 9, 0, 0, 0);	// March 18th 9am NZ time = March 17th 8pm UK time
													// NZ = UK+13 (NZ DST on, UK DST off)
													
	TDateTime time2(2005, EMarch, 23, 9, 0, 0, 0);	// March 24rd 9am NZ time = March 23nd 9pm UK time
													// NZ = UK+12 (NZ DST off, UK DST off)
													
	TDateTime time3(2005, EMarch, 29, 9, 0, 0, 0);	// March 30th 9am NZ time = March 29th 10pm UK time
													// NZ = UK+11 (NZ DST off, UK DST on)
	
	TDateTime time4(2005, ESeptember, 29, 9, 0, 0, 0);	// September 30th 9am NZ time = September 29th 10pm UK time
														// NZ = UK+11 (NZ DST off, UK DST on)
														
	TDateTime time5(2005, EOctober, 13, 9, 0, 0, 0);	// October 14th 9am NZ time = October 13th 9pm UK time
														// NZ = UK+12 (NZ DST on, UK DST on)
														
	TDateTime time6(2005, ENovember, 2, 9, 0, 0, 0);	// November 3rd 9am NZ time = November 2nd 8pm UK time
														// NZ = UK+13 (NZ DST on, UK DST off)

	CheckIsAnInstanceInSpecifiedZoneL(time1);
	CheckIsAnInstanceInSpecifiedZoneL(time2);
	CheckIsAnInstanceInSpecifiedZoneL(time3);
	CheckIsAnInstanceInSpecifiedZoneL(time4);
	CheckIsAnInstanceInSpecifiedZoneL(time5);
	CheckIsAnInstanceInSpecifiedZoneL(time6);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0008 */
void CTzTestManager::TestRepeatRuleTzStartEndTimeL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0008 Running tcal_tz - TestRepeatRuleTzStartEndTimeL")); 	
	//
	// Check start and end time 
	// converted to local time (Asia/Tokyo).

	// The start and end times were set to this 
	// in Pacific/Auckland summer time:
	TTime time(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0));
	
	// Local time is now Asia/Tokyo (no DST rule)
	time -= TTimeIntervalHours(13-9);

	TDateTime dbgStartTime = iCalEntry->StartTimeL().TimeUtcL().DateTime();

	TTime start = iCalEntry->StartTimeL().TimeLocalL();
	test(time == start);

	TTime end = iCalEntry->EndTimeL().TimeLocalL();
	test(time == end);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0009 */
void CTzTestManager::TestRepeatRuleTzStartEndTzRulesL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0009 Running tcal_tz - TestRepeatRuleTzStartEndTzRulesL")); 	
	//
	// Check start and end time 
	// converted with entry's TzRules.

	// The start and end times were set to this 
	TTime time(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0));
	
	CTzRules* rules = iCalEntry->TzRulesL();
	CleanupStack::PushL(rules);

	TTime start = iCalEntry->StartTimeL().TimeUtcL();
	rules->ConvertToLocalL(start);
	test(time == start);

	TTime end = iCalEntry->EndTimeL().TimeUtcL();
	rules->ConvertToLocalL(end);
	test(time == end);
	
	CleanupStack::PopAndDestroy(rules);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0010 */
void CTzTestManager::TestFloatingRepeatCheckL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0010 Running tcal_tz - TestFloatingRepeatCheckL")); 	
	// changed time zone to London
	// check instance are correct in local UK time, should be the same as Auckland time because entry time is floating
	TDateTime time1(2005, EMarch, 17, 9, 0, 0, 0);	// March 18th 9am floating time 
													
	TDateTime time2(2005, EMarch, 23, 9, 0, 0, 0);	// March 24rd 9am floating time 
													
	TDateTime time3(2005, EMarch, 29, 9, 0, 0, 0);	// March 30th 9am floating time 
	
	TDateTime time4(2005, ESeptember, 29, 9, 0, 0, 0);	// September 30th 9am floating time 
														
	TDateTime time5(2005, EOctober, 13, 9, 0, 0, 0);	// October 14th 9am floating time 
														
	TDateTime time6(2005, ENovember, 2, 9, 0, 0, 0);	// November 3rd 9am floating time 
	
	CheckIsAnInstanceInTimeModeL(time1, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time2, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time3, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time4, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time5, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time6, ETrue, TCalTime::EFloating);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0011 */
void CTzTestManager::TestFloatingExceptionCheckL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0011 Running tcal_tz - TestFloatingExceptionCheckL")); 	
	// changed time zone to London
	// check instance are correct in local UK time, should be the same as Auckland time because entry time is floating
	TDateTime time1(2005, EMarch, 17, 9, 0, 0, 0);	// March 18th 9am floating time 
													
	TDateTime time2(2005, EMarch, 23, 9, 0, 0, 0);	// March 24rd 9am floating time 
													
	TDateTime time3(2005, EMarch, 29, 9, 0, 0, 0);	// March 30th 9am floating time 
	
	TDateTime time4(2005, ESeptember, 29, 9, 0, 0, 0);	// September 30th 9am floating time 
														
	TDateTime time5(2005, EOctober, 13, 9, 0, 0, 0);	// October 14th 9am floating time 
														
	TDateTime time6(2005, ENovember, 2, 9, 0, 0, 0);	// November 3rd 9am floating time 

	
	CheckIsAnInstanceInTimeModeL(time1, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time3, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time5, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time6, ETrue, TCalTime::EFloating);

	CheckIsAnInstanceInTimeModeL(time2, EFalse, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time4, EFalse, TCalTime::EFloating);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0012 */
void CTzTestManager::TestFloatingRDateCheckL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0012 Running tcal_tz - TestFloatingRDateCheckL")); 	
	// changed time zone to London
	// check instance are correct in local UK time, should be the same as Auckland time because entry time is floating
	TDateTime time1(2005, EMarch, 17, 9, 0, 0, 0);	// March 18th 9am floating time 
													
	TDateTime time2(2005, EMarch, 23, 9, 0, 0, 0);	// March 24rd 9am floating time 
													
	TDateTime time3(2005, EMarch, 29, 9, 0, 0, 0);	// March 30th 9am floating time 
	
	TDateTime time4(2005, ESeptember, 29, 9, 0, 0, 0);	// September 30th 9am floating time 
														
	TDateTime time5(2005, EOctober, 13, 9, 0, 0, 0);	// October 14th 9am floating time 
														
	TDateTime time6(2005, ENovember, 2, 9, 0, 0, 0);	// November 3rd 9am floating time 

	TDateTime time7(2004, EDecember, 2, 9, 0, 0, 0);  // rDate1

	TDateTime time8(2006, EMarch, 23, 9, 0, 0, 0); // rDate2
	
	CheckIsAnInstanceInTimeModeL(time1, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time2, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time3, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time4, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time5, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time6, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time7, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInTimeModeL(time8, ETrue, TCalTime::EFloating);
	}


/*
This test is a variation of a repeat rule test. 
It was adapted from an unofficial version
of tcal_tz.cpp written for reproducing a defect.
*/
/** @SYMTestCaseID     PIM-TCAL-TZ-0013 */
void CTzTestManager::DefectTestL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0013 Running tcal_tz - DefectTestL")); 	
	HBufC8* uid = _L8("DUMMY_UID").AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0);
	
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);

	delete iCalEntry;		
	iCalEntry = entry;
	
	CTzRules* tzRules = CTzRules::NewL(2000, 2010);
	CleanupStack::PushL(tzRules);

	// Specify start/end time
	TDateTime dtStart(2001,EJuly,5,20,30,0,0); //9:30pm in London 06 of July (Wednesday) 10:30 in Stockholm 07 of July (Thursday)
	TDateTime dtEnd(2001,EJuly,5,21,30,0,0);
	TCalTime startCalTime;
	TCalTime endCalTime;
	startCalTime.SetTimeUtcL(dtStart);
	endCalTime.SetTimeUtcL(dtEnd);
	entry->SetStartAndEndTimeL(startCalTime,endCalTime);
	entry->SetSummaryL(_L("QGsUtilsTest:: TestTzL"));
	entry->SetDescriptionL(_L("TZ test. "));
	
	TCalRRule rule;
	TCalTime ruleStart;
	ruleStart.SetTimeUtcL(dtStart);
	// Create weekly repeat rule on Thursday and Saturday in Sweden
	rule.SetType(TCalRRule::EWeekly);
	rule.SetInterval(1);
	rule.SetCount(2);
	rule.SetDtStart(ruleStart);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.Append(EThursday); // 7th in Sweden is a Thursday
	days.Append(ESaturday);
	rule.SetByDay(days);
	days.Reset();
	days.Close();
	entry->SetRRuleL(rule);
	CleanupStack::PopAndDestroy(&days);
	
	tzRules->SetInitialStdTimeOffset(60); // Swedish rules
	TTzRule rule1(2000, 2010, 60, 120, EMarch, ETzDayInLastWeekOfMonth, 0, ESunday, ETzUtcTimeReference, 120);
	TTzRule rule2(2000, 2010, 120, 60, EOctober, ETzDayInLastWeekOfMonth, 0, ESunday, ETzUtcTimeReference, 180);
	tzRules->AddRuleL(rule1);
	tzRules->AddRuleL(rule2);
	entry->SetTzRulesL(*tzRules); // set tz rules to be Swedish


	// store entry

	CCalEntryView& entryView = iTestLibrary->SynCGetEntryViewL();

	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	
	entryArray.AppendL(entry);
	TInt entriesAdded = 0;
	entryView.StoreL(entryArray, entriesAdded);
	
	CleanupStack::PopAndDestroy(&entryArray);
	CleanupStack::PopAndDestroy(tzRules);


	// fetch instances

	CCalInstanceView* instanceView = &iTestLibrary->SynCGetInstanceViewL();
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	TCalTime calTimeStart;
	calTimeStart.SetTimeUtcL(TDateTime(2001, EJuly, 0, 0, 0, 0, 0)); // 00:00 on 1 July 2005
	TCalTime calTimeEnd;
	calTimeEnd.SetTimeUtcL(TDateTime(2001, EAugust, 0, 0, 0, 0, 0)); // 00:00 on 1 August 2005
			
	CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
	CalCommon::TCalTimeRange timeRange(calTimeStart, calTimeEnd);
	
	instanceView->FindInstanceL(instances, filter, timeRange);
	test(instances.Count() == 2);
	for (TInt i = 0; i < instances.Count(); i++)
		{
		TDateTime dt = instances[i]->Time().TimeUtcL().DateTime();
		test.Printf(_L("found instance #%d: %d/%d/%d, %d:%d:%d UTC\n"), i, 
			dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
		}
		
	CleanupStack::PopAndDestroy(&instances); // instances.ResetAndDestroy();
	
	
	// needed by CheckIsAnInstanceInSpecifiedZoneL
	iCalEntry = entry;
	// 7/7/2001, 21:30 London, 22:30 Stockholm
	TDateTime time1a(2001, EJuly, 6, 21, 30, 0, 0);
	TDateTime time1b(2001, EJuly, 6, 22, 30, 0, 0);
	CheckIsAnInstanceInTimeModeL(time1a, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInSpecifiedZoneL(time1b);

	// 7/12/2001, 21:30 London, 22:30 Stockholm
	TDateTime time2a(2001, EJuly, 11, 21, 30, 0, 0);
	TDateTime time2b(2001, EJuly, 11, 22, 30, 0, 0);
	CheckIsAnInstanceInTimeModeL(time2a, ETrue, TCalTime::EFloating);
	CheckIsAnInstanceInSpecifiedZoneL(time2b);

	iCalEntry = NULL;
	
	CleanupStack::PopAndDestroy(entry);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0014 */
void CTzTestManager::TestRDatesTzSetupL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0014 Running tcal_tz - TestRDatesTzSetupL")); 	
	HBufC8* dummy = NULL;
	delete iCalEntry;
	iCalEntry = NULL;
	iCalEntry = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, dummy);
	
	TTime dtStart(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0));
	TTime dtEnd(TDateTime(2005, EJanuary, 0, 10, 0, 0, 0));
	TCalTime startCalTime;
	TCalTime endCalTime;
	startCalTime.SetTimeLocalL(dtStart);
	endCalTime.SetTimeLocalL(dtEnd);
	iCalEntry->SetStartAndEndTimeL(startCalTime, startCalTime);

	RArray<TCalTime> dates;
	CleanupClosePushL(dates);
	TCalTime rdate;
	TDateTime date1(2005, EJanuary, 10, 9, 0, 0, 0);
	rdate.SetTimeLocalL(date1);
	dates.AppendL(rdate);
	TDateTime date2(2005, EJanuary, 20, 9, 0, 0, 0);
	rdate.SetTimeLocalL(date2);
	dates.AppendL(rdate);
	iCalEntry->SetRDatesL(dates);
	CleanupStack::PopAndDestroy(&dates);

	RTz tz;
	User::LeaveIfError(tz.Connect());
	CTzId* id = CTzId::NewL(KPacificAuckland);
	CleanupStack::PushL(id);
	CTzRules* rules = tz.GetTimeZoneRulesL(*id, TDateTime(2000, EJanuary, 0, 0, 0, 0, 0), TDateTime(2010, EJanuary, 0, 0, 0, 0, 0), ETzUtcTimeReference);
	CleanupStack::PopAndDestroy(id);
	tz.Close();
	
	CleanupStack::PushL(rules);
	
	iCalEntry->SetTzRulesL(*rules); // set tz rules to be local
	CleanupStack::PopAndDestroy(rules);
	
	CCalEntryView& entryView = iTestLibrary->SynCGetEntryViewL();
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	
	entryArray.AppendL(iCalEntry);
	TInt entriesAdded = 0;
	entryView.StoreL(entryArray, entriesAdded);
	
	CleanupStack::PopAndDestroy(&entryArray);

	// Now retrieve and test results

	CheckIsAnInstanceInSpecifiedZoneL(date1);
	CheckIsAnInstanceInSpecifiedZoneL(date2);
	}


void CTzTestManager::CheckIsAnInstanceInTimeModeL(const TTime& aLocalTime, TBool aFound, TCalTime::TTimeMode aTimeMode)
	{
	CCalInstanceView& instanceView = iTestLibrary->SynCGetInstanceViewL();
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);	

	// Set up filter to find instance at aTime:
	CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
	TCalTime calTime;
	if(aTimeMode == TCalTime::EFloating)
		{
		calTime.SetTimeLocalFloatingL(aLocalTime);
		}
	else if(aTimeMode == TCalTime::EFixedTimeZone)
		{
		calTime.SetTimeLocalL(aLocalTime);
		}
	
	CalCommon::TCalTimeRange timeRange(calTime, calTime);

	instanceView.FindInstanceL(instances, filter, timeRange);
	
	TDateTime dt = aLocalTime.DateTime();
	test.Printf(_L("expecting an instance at %d:%d:%d on %d/%d/%d"), 
		dt.Hour(), dt.Minute(), dt.Second(), dt.Day()+1, dt.Month()+1, dt.Year());
		
	for (TInt i = 0; i < instances.Count(); i++)
		{
		dt = instances[i]->Time().TimeLocalL().DateTime();
	test.Printf(_L("found instance %d at %d:%d:%d on %d/%d/%d"), i,
		dt.Hour(), dt.Minute(), dt.Second(), dt.Day()+1, dt.Month()+1, dt.Year());
		}
		
	if (aFound)
		{
		test(instances.Count() == 1);
		test(instances[0]->Time().TimeLocalL() == aLocalTime);
		}
	else
		{
		test(instances.Count() == 0);
		}

	CleanupStack::PopAndDestroy(&instances); // instances.ResetAndDestroy();
	}


// aTime should be relative to iCalEntry's own CTzRules.
void CTzTestManager::CheckIsAnInstanceInSpecifiedZoneL(const TTime& aTime)
	{
	CCalInstanceView& instanceView = iTestLibrary->SynCGetInstanceViewL();
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);	

	// Set up filter to find instance at aTime:
	CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
	TCalTime calTime;
	CTzRules* rules = iCalEntry->TzRulesL();
	CleanupStack::PushL(rules);
	TTime queryTime = aTime;
	rules->ConvertToUtcL(queryTime);
	calTime.SetTimeUtcL(queryTime);
	CalCommon::TCalTimeRange timeRange(calTime, calTime);
	
	instanceView.FindInstanceL(instances, filter, timeRange);
	test(instances.Count() == 1);

	TTime instanceTime = instances[0]->Time().TimeUtcL();
	TDateTime dbgTime2 = instanceTime.DateTime();
	rules->ConvertToLocalL(instanceTime);
	test(instanceTime == aTime);

	CleanupStack::PopAndDestroy(rules);
	CleanupStack::PopAndDestroy(&instances); // instances.ResetAndDestroy();
	}


void CTzTestManager::VerifyRulesL(const CTzRules& aRules,const TCalTime& aStartRange,const TCalTime& aEndRange)
	{
	CCalInstanceView* instanceView = &iTestLibrary->SynCGetInstanceViewL();

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);		
		
	CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
	CalCommon::TCalTimeRange timeRange(aStartRange,aEndRange);
	instanceView->FindInstanceL(instances, filter, timeRange);
	for (TInt i = 0; i < instances.Count(); i++)
		{
		CCalEntry& entry = instances[i]->Entry();
		CTzRules* rules = entry.TzRulesL();
		CleanupStack::PushL(rules);
		test(rules->IsEqualTo(aRules));
		CleanupStack::PopAndDestroy(rules);
		}
		
	CleanupStack::PopAndDestroy(&instances);
	}
	
	
// DEF074085
// SetRRulesL() did not preserve the timezone rules of the CAgnEntry repeat def
// Any subsequent UpdateL would see a null timezone and reset to the local one
// Fix to SetRRulesL() preserves the CAgnRptDef tz rules
// This test case:
// Sets a repeat rule with SetRRulesL()
// Sets an arbitrary repeat timezone rule 
// StoreL() to the database
// Verifies the retrieved tz rules are the original ones saved
// FetchL() the saved entry
// Modifies the summary with SetSummaryL()
// Calls SetRRulesL() again (Defect was here)
// Calls UpdateL()
// Verifies the retrieved tz rules are the original ones saved
void CTzTestManager::TestCopyRepeatRulesL()
	{
	iTestLibrary->CleanDatabaseL();
	
	delete iCalEntry;
	iCalEntry = NULL;

	_LIT8(KEntryUid,"DEF074805");
	HBufC8* uid = KEntryUid().AllocLC();

	iCalEntry=CCalEntry::NewL(CCalEntry::EEvent,uid, CCalEntry::EMethodAdd, 0);

	CleanupStack::Pop();
	iCalEntry->SetSummaryL(_L("Original"));
	TDateTime dtStart(2005,EMarch,0,8,0,0,0);
	TCalTime startCalTime;
	startCalTime.SetTimeLocalL(dtStart);
	iCalEntry->SetStartAndEndTimeL(startCalTime,startCalTime);
	TCalRRule calRule(TCalRRule::EWeekly);
	calRule.SetDtStart(startCalTime);
	calRule.SetCount(8);
	calRule.SetInterval(1);
	RArray<TDay> days;
	days.AppendL(ESunday);
	calRule.SetByDay(days);
	// This call to SetRRuleL() has a null timezone
	iCalEntry->SetRRuleL(calRule);
	days.Reset();

	// Arbitrary timezone (Not the same as local)
	TTzRule tRule1(1981,1996,0,60,EMarch,ETzDayInLastWeekOfMonth,0,ESunday,ETzUtcTimeReference,60);
	TTzRule tRule2(1981,1996,60,0,EOctober,ETzDayInLastWeekOfMonth,0,ESunday,ETzUtcTimeReference,60);
	TTzRule tRule3(0,KMaxTUint16,0,0,EJanuary,ETzFixedDate,0,0,ETzWallTimeReference,0);	
	CTzRules* origRules = CTzRules::NewL(2003,2007);
	CleanupStack::PushL(origRules);
	origRules->AddRuleL(tRule1);
	origRules->AddRuleL(tRule2);
	origRules->AddRuleL(tRule3);
	// Sets the timezone in the repeat def
	iCalEntry->SetTzRulesL(*origRules);
		{
		CCalEntryView& entryView = iTestLibrary->SynCGetEntryViewL();
		RPointerArray<CCalEntry> entryArray;
		CleanupClosePushL(entryArray);
		
		entryArray.AppendL(iCalEntry);
		TInt entriesAdded = 0;
		// Save to database
		entryView.StoreL(entryArray, entriesAdded);
		
		CleanupStack::PopAndDestroy(&entryArray);
		}
	// Set up the range for entries to read
	TCalTime startRange;
	startRange.SetTimeLocalL(TDateTime(2005,EMarch,0,8,0,0,0));
	TCalTime endRange;
	endRange.SetTimeLocalL(TDateTime(2005,EApril,15, 8, 0, 0, 0));
	// Verify the StoreL() preserved the original rules
	VerifyRulesL(*origRules,startRange,endRange);
		{
		// Fetch the entry ready for update
		CCalEntryView& entryView = iTestLibrary->SynCGetEntryViewL();
		RPointerArray<CCalEntry> entryArray;
		CleanupResetAndDestroyPushL(entryArray);
				
		entryView.FetchL(KEntryUid, entryArray);	
		
		// Update the rule (DEFECT was here)
		entryArray[0]->SetRRuleL(calRule);
		entryArray[0]->SetSummaryL(_L("Modified"));
		TInt entriesAdded = 0;
		// Updates the database
		entryView.UpdateL(entryArray, entriesAdded);
		
		CleanupStack::PopAndDestroy(&entryArray);
		}
	// Verifies defect fix
	VerifyRulesL(*origRules,startRange,endRange);
	
	CleanupStack::PopAndDestroy(origRules);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0015 */
void CTzTestManager::TestForeignZoneRepeatRule1L()
	{
	iTestLibrary->CleanDatabaseL();
	
	HBufC8* uid = _L8("DUMMY_UID").AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
		
	CTzRules* tzRules = CTzRules::NewL(2000, 2010);
	CleanupStack::PushL(tzRules);
	
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0015 Creating repeating entry every Thursday and Saturday from 7 July at 00:30 Stockholm time (6 July 22:30 UTC)")); 	
	TDateTime dtStart(2005,EJuly,5,22,30,0,0); //11:30pm in London 06 of Juli (Wednessday) 00:30 in Stockholm 07 of Juli (Thursday)
	TDateTime dtEnd(2005,EJuly,5,23,20,0,0);
	TCalTime startCalTime;
	TCalTime endCalTime;
	startCalTime.SetTimeUtcL(dtStart);
	endCalTime.SetTimeUtcL(dtEnd);
	entry->SetStartAndEndTimeL(startCalTime,endCalTime);
	entry->SetSummaryL(_L("QGsUtilsTest:: TestTzL"));
	entry->SetDescriptionL(_L("TZ test. "));
	
	TCalRRule rule;
	TCalTime ruleStart;
	ruleStart.SetTimeUtcL(dtStart);
	// Create weekly repeat rule on Thursday and Saturday in Sweden
	rule.SetType(TCalRRule::EWeekly);
	rule.SetInterval(1);
	rule.SetCount(2);
	rule.SetDtStart(ruleStart);
	RArray<TDay> days;
	days.Append(EThursday); // 7th is a Thursday
	days.Append(ESaturday);
	rule.SetByDay(days);
	days.Reset();
	days.Close();
	entry->SetRRuleL(rule);
	
	tzRules->SetInitialStdTimeOffset(60); // Swedish rules
	TTzRule rule1(2000, 2010, 60, 120, EMarch, ETzDayInLastWeekOfMonth, 0, ESunday, ETzUtcTimeReference, 120);
	TTzRule rule2(2000, 2010, 120, 60, EOctober, ETzDayInLastWeekOfMonth, 0, ESunday, ETzUtcTimeReference, 180);
	tzRules->AddRuleL(rule1);
	tzRules->AddRuleL(rule2);
	entry->SetTzRulesL(*tzRules); // set tz rules to be Swedish
	
	CCalEntryView* entryView = &iTestLibrary->SynCGetEntryViewL();
	TInt success = 0;
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	
	entries.Append(entry);
	entryView->StoreL(entries, success); // Store the CCalEntry
	test(success == 1);
	
	CleanupStack::PopAndDestroy(&entries);//entries.Reset();	
	CleanupStack::PopAndDestroy(tzRules);
	CleanupStack::PopAndDestroy(entry);
	}

void CTzTestManager::TestForeignZoneRepeatRule2L()
	{
	CCalInstanceView* instanceView = &iTestLibrary->SynCGetInstanceViewL();
	
	TCalTime calTimeStart;
	calTimeStart.SetTimeUtcL(TDateTime(2005, EJuly, 0, 0, 0, 0, 0)); // 00:00 on 1 July 2005
	TCalTime calTimeEnd;
	calTimeEnd.SetTimeUtcL(TDateTime(2005, EJuly, 30, 23, 59, 59, 0)); // 23:59 on 31 July 2005
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
	CalCommon::TCalTimeRange timeRange(calTimeStart, calTimeEnd);
	
	instanceView->FindInstanceL(instances, filter, timeRange);
	test(instances.Count() == 2);
	for (TInt i = 0; i < instances.Count(); i++)
		{
		TDateTime dt = instances[i]->Time().TimeLocalL().DateTime();
		test.Printf(_L("found instance on %d: %d/%d/%d, %d:%d:%d\n"), i, 
			dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
		
		//1-st instance is on July 06 at 23:30 - Wednesday (London timezone)
		//2-nd instance is on July 08 at 23:30 - Friday (London time zone)
		test((TTime(dt) == TTime(TDateTime(2005,EJuly,5,23,30,0,0))) ||
			(TTime(dt) == TTime(TDateTime(2005,EJuly,7,23,30,0,0))));
		}
	
	CleanupStack::PopAndDestroy(&instances); //instances.ResetAndDestroy();
	}


// DEF 066566 
// Test recurring entry has correctly updating instance time
/** @SYMTestCaseID     PIM-TCAL-TZ-0016 */
void CTzTestManager::TestLocalRepeatRuleL()
	{
	iTestLibrary->CleanDatabaseL();
	
	TestSetTimeZoneL(KEuropeHelsinki);

	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0016 Create repeating entry every 4th day of the month, from 4 Sept 2005 at 11:30 Helsinki time (08:30 UTC)")); 	test.Printf(_L("Check that the UTC time changes between different months and the local time doesn't"));
	
	// ADD RECURRING ENTRY
	HBufC8* uid = HBufC8::NewL( 10 );
	uid->Des().Copy( _L8("myuid") );
	CCalEntry* entry = CCalEntry::NewL( CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0 );

	TCalTime startTime;
	TCalTime endTime;
	startTime.SetTimeUtcL(TTime(TDateTime(2005, ESeptember, 3, 8, 30, 0, 0)));
	endTime.SetTimeUtcL(TTime(TDateTime(2005, ESeptember, 3, 9, 00, 0, 0)));
	
	entry->SetStartAndEndTimeL( startTime, endTime );

	// recurrence rule
	TCalRRule recurrenceRule;						
	recurrenceRule.SetType( TCalRRule::EMonthly );
	recurrenceRule.SetCount( 12 );
	recurrenceRule.SetDtStart( startTime );
	recurrenceRule.SetInterval( 1 );
	
	RArray<TInt> monthDays;
	monthDays.Append( 3 );
	recurrenceRule.SetByMonthDay( monthDays );
	monthDays.Close();			
	
	entry->SetRRuleL( recurrenceRule );
	
	// time zone rules - use Helsinki time
	entry->SetTzRulesL();
			
	// write entry to database
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	entries.Append( entry );
	TInt successCount = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL( entries, successCount );

	CleanupStack::PopAndDestroy(&entries); //entries.ResetAndDestroy();
	
	TCalTime calMinTimeRange;
	calMinTimeRange.SetTimeUtcL(TCalTime::MinTime());
	TCalTime calMaxTimeRange;
	calMaxTimeRange.SetTimeUtcL(TCalTime::MaxTime());

	// find all instances
	CalCommon::TCalTimeRange timeRange(calMinTimeRange, calMaxTimeRange);
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	
	iTestLibrary->SynCGetInstanceViewL().FindInstanceL( instanceList, CalCommon::EIncludeAll, timeRange );

	TInt count = instanceList.Count();

	// Check that the UTC time of the zone varies
	TInt utcHour = startTime.TimeUtcL().DateTime().Hour(); // 8
	TInt localHour = startTime.TimeLocalL().DateTime().Hour(); // 11

	TBool utcTimeChanged = EFalse;
	TBool localTimeChanged = EFalse;

	for( TInt i = 0; i < count; i++ )
		{
		TDateTime dt = instanceList[i]->Time().TimeLocalL().DateTime();
		TInt startHourUtcInstance   = instanceList[i]->Time().TimeUtcL().DateTime().Hour();
		
		test.Printf(_L("local instance time: %d:%d:%d on %d/%d/%d, utc hour=%d \n"), 
			dt.Hour(), dt.Minute(), dt.Second(), dt.Day()+1, dt.Month()+1, dt.Year(), startHourUtcInstance);
			
		if (utcHour != startHourUtcInstance)
			{
			utcTimeChanged = ETrue;
			}
		if (dt.Hour() != localHour)
			{
			localTimeChanged = ETrue;
			}
		}

	CleanupStack::PopAndDestroy(&instanceList); //instanceList.ResetAndDestroy();
	
	// check that the UTC times were not all the same over the course of the year
	test(utcTimeChanged);
	
	// check that the local time did not change
	test(!localTimeChanged);
	}


// Defect 68010 - check that repeating instance times change w.r.t UTC
/** @SYMTestCaseID     PIM-TCAL-TZ-0017 */
void CTzTestManager::TestLocalRepeatRule2L()
	{
	iTestLibrary->CleanDatabaseL();
	
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0017 Create repeating entry every day from 6 January 2005 at 09:00 Central US time")); 	test.Printf(_L("Check that the UTC time changes between different months and the local time doesn't"));
	
	TTime startTime(TDateTime(2005, EJanuary, 5, 9, 0, 0, 0));
	HBufC8* uid = _L8("GUID1").AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	
	TCalTime startTimeCal;
	startTimeCal.SetTimeLocalL(startTime);
	entry->SetStartAndEndTimeL(startTimeCal, startTimeCal);
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(startTimeCal);
	rule.SetCount(400);
	
	entry->SetRRuleL(rule);
	
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	
	entries.AppendL(entry);
	TInt success = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entries, success);
	
	CleanupStack::PopAndDestroy(&entries); //entries.Reset();
	CleanupStack::PopAndDestroy(entry);

	RPointerArray<CCalInstance>	instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	
	CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll); 
	TTime searchTime1(TDateTime(2005, EApril, 0, 0, 0, 0, 0));
	TTime searchTime2(TDateTime(2005, EApril, 7, 0, 0, 0, 0));
	TCalTime searchCalTime1;
	searchCalTime1.SetTimeUtcL(searchTime1);
	TCalTime searchCalTime2;
	searchCalTime2.SetTimeUtcL(searchTime2);
	
	CalCommon::TCalTimeRange searchRange(searchCalTime1, searchCalTime2);
	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instanceList, filter, searchRange);
	
	TInt localHour = startTime.DateTime().Hour();
	TInt utcHour = startTimeCal.TimeUtcL().DateTime().Hour();
	
	TBool utcTimeChanged = EFalse;
	TBool localTimeChanged = EFalse;

	TInt count = instanceList.Count();
	for( TInt i = 0; i < count; i++ )
		{
		TDateTime dt = instanceList[i]->Time().TimeLocalL().DateTime();
		TInt startHourUtcInstance = instanceList[i]->Time().TimeUtcL().DateTime().Hour();
		
		test.Printf(_L("local instance time: %d:%d:%d on %d/%d/%d, utc hour=%d \n"), 
			dt.Hour(), dt.Minute(), dt.Second(), dt.Day()+1, dt.Month()+1, dt.Year(), startHourUtcInstance);
			
		if (utcHour != startHourUtcInstance)
			{
			utcTimeChanged = ETrue;
			}
		if (dt.Hour() != localHour)
			{
			localTimeChanged = ETrue;
			}
		}

	CleanupStack::PopAndDestroy(&instanceList); //instanceList.ResetAndDestroy();
	
	// check that the UTC times were not all the same over the course of the year
	test(utcTimeChanged);
	
	// check that the local time did not change
	test(!localTimeChanged);
	}


// Defect 70132 - check that November in Helsinki is 2 hours from UTC
/** @SYMTestCaseID     PIM-TCAL-TZ-0018 */
void CTzTestManager::TestLocalTimeZoneL()
	{
	iTestLibrary->CleanDatabaseL();
	
	TestSetTimeZoneL(KEuropeHelsinki);
	
	RTz tz;
	User::LeaveIfError(tz.Connect());
	
	TTime start(TDateTime(2005, ENovember, 0, 0, 0, 0, 0));
	TTime end(TDateTime(2005, EDecember, 0, 0, 0, 0, 0));
	
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0018 Fetch time zone rules for Helsinki for November and use them to convert a time (should be 2 hours difference)")); 	CTzRules* rules = tz.GetTimeZoneRulesL(start, end, ETzStdTimeReference);
	CleanupStack::PushL(rules);
	
	TTime time(TDateTime(2005, ENovember, 6, 12, 0, 0, 0));
	
	TDateTime dt1 = time.DateTime();
	rules->ConvertToUtcL(time);
	TDateTime dt2 = time.DateTime();

	test.Printf(_L("Local time %d:%d on %d/%d converts to %d:%d UTC"), 
			dt1.Hour(), dt1.Minute(), dt1.Day()+1, dt1.Month()+1, dt2.Hour(), dt2.Minute());
	
	test(dt1.Hour() - dt2.Hour() == 2);
	
	CleanupStack::PopAndDestroy(rules);
	}

	
// DEF076327 - check that alarm time for Todo and other entries is updated according to timezone change
/** @SYMTestCaseID     PIM-TCAL-TZ-0019 */
void CTzTestManager::TestAlarmTzUpdateSetupL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0019 Check that entry alarm time is updated according to timezone change")); 	iTestLibrary->CleanDatabaseL();

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);	

	HBufC8* uid = _L8("Todo_Test").AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, uid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	
	uid = _L8("Appt_Test").AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);

	uid = _L8("Event_Test").AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EEvent, uid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);

	uid = _L8("Reminder_Test").AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EReminder, uid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);

	uid = _L8("Anniv_Test").AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EAnniv, uid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	
	TCalTime calTime;
	TTime time(TDateTime(2005, ENovember, 0, 10, 0, 0, 0));
	TInt count = entries.Count();
	for (TInt i = 0; i < count; i++)
		{
		calTime.SetTimeLocalL(time);
		entries[i]->SetStartAndEndTimeL(calTime, calTime);
		
		CCalAlarm* alarm = CCalAlarm::NewL();
		CleanupStack::PushL(alarm);
		alarm->SetTimeOffset(60); // set alarm to be 60 minutes before due time
		entries[i]->SetAlarmL(alarm);
		CleanupStack::PopAndDestroy(alarm);
		}
		
	TInt success = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entries, success);
	CleanupStack::PopAndDestroy(); // entries
	}


void CTzTestManager::TestAlarmTzUpdateCheckL()
	{
	TCalTime calMinTimeRange;
	calMinTimeRange.SetTimeUtcL(TCalTime::MinTime());
	TCalTime calMaxTimeRange;
	calMaxTimeRange.SetTimeUtcL(TCalTime::MaxTime());

	// find all instances
	CalCommon::TCalTimeRange timeRange(calMinTimeRange, calMaxTimeRange);

	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);	

	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instanceList, CalCommon::EIncludeAll, timeRange);

	TInt count = instanceList.Count();
	test(count == 5); // get the 5 entries saved above

	TTime expectedTime(TDateTime(2005, ENovember, 0, 9, 0, 0, 0)); // expected start and end time after timezone change
	TTimeIntervalMinutes expectedAlarmOffset(60); // because start and end time are changed according to time zone already,
												  // we expect alarm offset to be same as before
	for (TInt i = 0; i < count; i++)
		{
		CCalInstance* instance = instanceList[i];

		TCalTime startCalTime = instance->StartTimeL();
		TCalTime endCalTime = instance->EndTimeL();
		test(startCalTime.TimeLocalL() == expectedTime);
		test(endCalTime.TimeLocalL() == expectedTime);

		CCalEntry &insEntry = instance->Entry();
		startCalTime = insEntry.StartTimeL();
		endCalTime = insEntry.EndTimeL();
		test(startCalTime.TimeLocalL() == expectedTime);
		if (insEntry.EntryTypeL() != CCalEntry::EReminder) // reminder entry have null end time
			{
			test(endCalTime.TimeLocalL() == expectedTime);
			}

		CCalAlarm* alarm = insEntry.AlarmL();
		CleanupStack::PushL(alarm);
		test(alarm->TimeOffset() == expectedAlarmOffset);
		CleanupStack::PopAndDestroy(alarm);
		}
		
	CleanupStack::PopAndDestroy(); // close instanceList
	}

//Test code for DEF080617
//Timemode of instances should be floating
/** @SYMTestCaseID     PIM-TCAL-TZ-0020 */
void CTzTestManager::ImportFloatingEntryL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0020 Importing floating entry")); 	_LIT8(KFloatingEntry,	"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"UID:1\r\n"
							"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
							"DTSTART:19991101T100000\r\n"
							"DTEND:19991101T113000\r\n"
							"RRULE:D1 #3\r\n"
							"AALARM:19991101T090000\r\n"
							"RDATE:19991105T130000\r\n"
							"LAST-MODIFIED:19991101T124400\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");

    _LIT(KFileName, "c:\\floatingvcard.vcs");
    RFile outfile;
    TInt err = outfile.Replace(iTestLibrary->FileSession(), KFileName(), EFileWrite);
   
    User::LeaveIfError(err);
  
    outfile.Write(KFloatingEntry());   
    outfile.Close();

	RFile infile;
	User::LeaveIfError(infile.Open(iTestLibrary->FileSession(),KFileName,EFileRead));
	CleanupClosePushL(infile);
	RFileReadStream readStream(infile);
	CleanupClosePushL(readStream);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CCalDataExchange* sync = CCalDataExchange::NewL(iTestLibrary->GetSession());
	CleanupStack::PushL(sync);
	TUid uidVCalendar(KUidVCalendar);
	
	sync->ImportL(uidVCalendar, readStream, entryArray);
	CleanupStack::PopAndDestroy(sync);
	
	test(entryArray.Count() == 1);

	TInt entriesCompleted=0;
	test.Printf(_L("Store entry\n"));
	
	iTestLibrary->SynCGetEntryViewL().StoreL(entryArray,entriesCompleted);
	test(entriesCompleted==entryArray.Count());

	CleanupStack::PopAndDestroy(3,&infile);//infile, readStream, entryArray
    iTestLibrary->FileSession().Delete(KFileName);
	}

//Test code for DEF080617
//Timemode of instances should be floating
void CTzTestManager::TestTimeModeL()
	{
    test.Printf(_L("Testing TimeMode"));

    CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
    TCalTime startTime;
    startTime.SetTimeUtcL(TTime(TDateTime(1999, ESeptember, 0, 0, 0, 0, 0)));
    TCalTime endTime;
    endTime.SetTimeUtcL(TTime(TDateTime(1999, ENovember, 6, 0, 0, 0, 0)));
    CalCommon::TCalTimeRange timeRange(startTime, endTime);
   
    RPointerArray<CCalInstance> aInstances;
	CleanupResetAndDestroyPushL(aInstances);

    iTestLibrary->SynCGetInstanceViewL().FindInstanceL(aInstances, filter, timeRange);
    
    for (TInt i = 0; i < aInstances.Count(); i++)
        {
        TDateTime instanceTime = aInstances[i]->Time().TimeLocalL().DateTime();
        RDebug::Print(_L("instance time at %d:%d on %d/%d/%d"), instanceTime.Hour(), instanceTime.Minute(),
            instanceTime.Day()+1, instanceTime.Month()+1, instanceTime.Year());
        TCalTime::TTimeMode instanceTimeMode = aInstances[i]->Time().TimeMode();
        test(instanceTimeMode == TCalTime::EFloating);

        TDateTime startTime = aInstances[i]->StartTimeL().TimeLocalL().DateTime();
        RDebug::Print(_L("instance start time at %d:%d on %d/%d/%d"), startTime.Hour(), startTime.Minute(),
            startTime.Day()+1, startTime.Month()+1, startTime.Year());
        TCalTime::TTimeMode startTimeMode = aInstances[i]->StartTimeL().TimeMode();
        test(startTimeMode == TCalTime::EFloating);

        TDateTime endTime = aInstances[i]->EndTimeL().TimeLocalL().DateTime();
        RDebug::Print(_L("instance end time at %d:%d on %d/%d/%d"), endTime.Hour(), endTime.Minute(),
            endTime.Day()+1, endTime.Month()+1, endTime.Year());
        TCalTime::TTimeMode endTimeMode = aInstances[i]->EndTimeL().TimeMode();
        test(endTimeMode == TCalTime::EFloating);
        RDebug::Print(_L("\n"));
        }
	CleanupStack::PopAndDestroy(&aInstances);    
	}

void CTzTestManager::TestExportTzRulesInHemisphereL(TBool aSouthernHemisphere)
	{
	_LIT(KTestExportFileSouth, "export_southern_hemisphere_tz.vcs");
	_LIT(KTestExportFileNorth, "export_northern_hemisphere_tz.vcs");	
	_LIT8(KTestGuid, "guid123");
	
	TBuf<64> fileName;
	if (aSouthernHemisphere)
		{
		fileName.Copy(KTestExportFileSouth());
		}
	else
		{
		fileName.Copy(KTestExportFileNorth());
		}
	
	
	HBufC8* guid = KTestGuid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TTime now;
	now.HomeTime();
	TInt currYear = now.DateTime().Year();

	TDateTime entryStart(currYear, EMarch, 19, 9, 0, 0, 0); // 0900 on 20 Mar This Year
	TDateTime entryUntil(currYear+2, EJune, 9, 9, 0, 0, 0); // 0900 on 10 Jun Two Years Time

	TCalTime calStart;
	calStart.SetTimeLocalL(entryStart);
	TCalTime calUntil;
	calUntil.SetTimeLocalL(entryUntil);
	
	entry->SetStartAndEndTimeL(calStart, calStart);
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(1);
	rule.SetDtStart(calStart);
	rule.SetUntil(calUntil);
	entry->SetRRuleL(rule);
	
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	
	// fetch the entry and export it
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	iTestLibrary->SynCGetEntryViewL().FetchL(KTestGuid(), entries);
	test(entries.Count() == 1);

	iTestLibrary->ExportL(fileName, entries);
	entries.ResetAndDestroy();

	iTestLibrary->ImportL(fileName, entries);
	test(entries.Count() == 1);
	CTzRules* fetchedTzRules = entries[0]->TzRulesL();
	CleanupStack::PushL(fetchedTzRules);

	test(fetchedTzRules != NULL);
	TTzRule lastRule = (*fetchedTzRules)[fetchedTzRules->Count()-1];
	TDateTime lastRuleTime = lastRule.iFrom.iTime.DateTime();
	if (aSouthernHemisphere)
		{
		// check the last rule goes up to the first half of the year in Three Years Time
		test(lastRuleTime.Year() == currYear + 3);
		test(lastRuleTime.Month() < EJune);
		}
	else
		{
		// check the last rule goes up to the second half of the year in Two Years Time
		test(lastRuleTime.Year() == currYear + 2);
		test(lastRuleTime.Month() > EJuly);
		}

	CleanupStack::PopAndDestroy(fetchedTzRules);
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy
	}

// new test cases
/** @SYMTestCaseID     PIM-TCAL-TZ-0021 */
void CTzTestManager::TestCreateEntryWithMidnightInHomeTimeL(TBool aFloating)
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0021 Running tcal_tz - TestCreateEntryWithMidnightInHomeTimeL")); 

	// start the test case with a clean calendar database

	iTestLibrary->CleanDatabaseL();


	// this sets the number of iterations in the below for-loop of entries creation

	const TInt KNumberOfIterations = 4;


	// array of entry types

	RArray<CCalEntry::TType> typeArray;
	CleanupClosePushL(typeArray);
	
	typeArray.AppendL(CCalEntry::EAppt);
	typeArray.AppendL(CCalEntry::ETodo);
	typeArray.AppendL(CCalEntry::EEvent);
	typeArray.AppendL(CCalEntry::EAnniv);	


	// array of time zones

	RPointerArray<TDesC8> lit8Array;
	CleanupClosePushL(lit8Array);

	lit8Array.AppendL(&KAsiaTokyo);
	lit8Array.AppendL(&KEuropeLondon);
	lit8Array.AppendL(&KPacificAuckland);
	lit8Array.AppendL(&KEuropeStockholm);	
	lit8Array.AppendL(&KUsCentral);
	lit8Array.AppendL(&KEuropeHelsinki);
	lit8Array.AppendL(&KUsNewYork);
	lit8Array.AppendL(&KAsiaTaiwan);	
	lit8Array.AppendL(&KUsLosAngeles);
	lit8Array.AppendL(&KAsiaBangkok);	


	for( TInt i = 0; i < typeArray.Count() * KNumberOfIterations; ++i )
		{		
		RPointerArray<CCalEntry> entryArray;
		CleanupResetAndDestroyPushL(entryArray);


		// create different types of entry
		HBufC8* dummy = NULL;
		// [i % typeArray.Count()] makes sure that even doing multiple iterations we don't exceed array boundaries
		CCalEntry* entry = iTestLibrary->CreateCalEntryL(typeArray[i % typeArray.Count()], dummy);
		entryArray.AppendL(entry);

		// set an entry using GMT0 time such it contains at least midnight in Tokyo time
		// and insert an entry every other 2 days
		TTime dtStart(TDateTime(2005, EJanuary, i * 2, 8, 0, 0, 0));
		TTime dtEnd(TDateTime(2005, EJanuary, i * 2, 20, 0, 0, 0));
		TCalTime startCalTime;
		TCalTime endCalTime;


		// the entry home time is London
		TestSetTimeZoneL(KEuropeLondon);

		if ( aFloating )
			{
			// Create a floating entry
			startCalTime.SetTimeLocalFloatingL(dtStart);
			endCalTime.SetTimeLocalFloatingL(dtEnd);
			}
		else
			{
			startCalTime.SetTimeLocalL(dtStart);		
			endCalTime.SetTimeLocalL(dtEnd);				
			}

		entry->SetStartAndEndTimeL(startCalTime, endCalTime);

		TInt entriesAdded = 0;
		iTestLibrary->SynCGetEntryViewL().StoreL(entryArray, entriesAdded);
		test(entryArray.Count() == entriesAdded);


		CleanupStack::PopAndDestroy(&entryArray);



		// check the entry, changing the time zones (especially with midnight in)

		for( TInt j = 0; j < lit8Array.Count(); ++j )
			{
			// we change the time zone such in local time the set event contains midnight

			TestSetTimeZoneL(*lit8Array[j]);


			// fetch instances

			// create time filter
			
			TCalTime startTime, endTime;
			if (aFloating)
				{
				startTime.SetTimeLocalL(dtStart);
				endTime.SetTimeLocalL(dtStart + TTimeIntervalDays(1));
				}
			else
				{
				startTime.SetTimeLocalFloatingL(dtStart);
				endTime.SetTimeLocalFloatingL(dtStart + TTimeIntervalDays(1));
				}
			CalCommon::TCalTimeRange timeRange(startTime, endTime);
			
			// array to store the loaded instances
			
			RPointerArray<CCalInstance> instances;
			CleanupResetAndDestroyPushL(instances);


			iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
			// sanity check
			test(instances.Count() != 0);

			CCalInstance& instance = *(instances[0]);
			
			TTime entryStartLocalT(instance.Entry().StartTimeL().TimeLocalL());			
			TTime entryEndLocalT(instance.Entry().EndTimeL().TimeLocalL());			
			
			TTime instStartLocalT(instance.StartTimeL().TimeLocalL());			
			TTime instEndLocalT(instance.EndTimeL().TimeLocalL());
												
			test.Printf( _L("found instance: ") );
			TDateTime startDt = instStartLocalT.DateTime();			
			test.Printf( _L("%d/%d/%d, %d:%d:%d - "), 
						 startDt.Day()+1, startDt.Month()+1, startDt.Year(), 
						 startDt.Hour(),  startDt.Minute(),  startDt.Second() );			
			TDateTime endDt = instEndLocalT.DateTime();
			test.Printf( _L("%d/%d/%d, %d:%d:%d\n"),
						 endDt.Day()+1, endDt.Month()+1, endDt.Year(), 
						 endDt.Hour(),  endDt.Minute(),  endDt.Second() );

			// check the entry times					
			test( entryStartLocalT == startCalTime.TimeLocalL() );
			test( entryEndLocalT == endCalTime.TimeLocalL() );

/* TODO: instance time is not matching entry one in floating scenario.
		 To be fixed - raised defect DEF081789 - 
		 and remove the if( ! aFloating ) to prove it */

			if( ! aFloating )
				{
/***/				
				// check the instance times
				test( instStartLocalT == startCalTime.TimeLocalL() );
				test( instEndLocalT == endCalTime.TimeLocalL() );
/***/				
				}
/* ODOT */

			CleanupStack::PopAndDestroy(&instances);
			}
		}


	CleanupStack::PopAndDestroy(&lit8Array);
	
	
	CleanupStack::PopAndDestroy(&typeArray);	
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0022 */
void CTzTestManager::TestCreateEntryWithDstChangeL(TBool aFloating)
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0022 Running tcal_tz - TestCreateEntryWithDstChangeL")); 

	// array of entry types

	RArray<CCalEntry::TType> typeArray;
	CleanupClosePushL(typeArray);
	
	typeArray.AppendL(CCalEntry::EAppt);
	typeArray.AppendL(CCalEntry::ETodo);
	typeArray.AppendL(CCalEntry::EEvent);
	typeArray.AppendL(CCalEntry::EAnniv);	


	for( TInt i = 0; i < typeArray.Count(); ++i )
		{
		// start with a clean calendar database

		iTestLibrary->CleanDatabaseL();


		RPointerArray<CCalEntry> entryArray;
		CleanupResetAndDestroyPushL(entryArray);


		// create different types of entry
		HBufC8* dummy = NULL;
		// [i % typeArray.Count()] makes sure that even doing multiple iterations we don't exceed array boundaries
		CCalEntry* entry = iTestLibrary->CreateCalEntryL(typeArray[i % typeArray.Count()], dummy);
		entryArray.AppendL(entry);

		// set an entry using GMT0 time such it contains at least DST time
		TTime dtStart(TDateTime(2005, EMarch, 25, 20, 0, 0, 0));
		TTime dtEnd(TDateTime(2005, EMarch, 26, 8, 0, 0, 0));
		TCalTime startCalTime;
		TCalTime endCalTime;

	
		// the entry home time is London
		TestSetTimeZoneL(KEuropeLondon);
		
		// change the time to start one to simulate a non DST
		iTestLibrary->PIMTestServer().SetHomeTime(dtStart);
		

		if ( aFloating )
			{
			// Create a floating entry
			startCalTime.SetTimeLocalFloatingL(dtStart);
			endCalTime.SetTimeLocalFloatingL(dtEnd);
			}
		else
			{
			startCalTime.SetTimeLocalL(dtStart);		
			endCalTime.SetTimeLocalL(dtEnd);				
			}

		entry->SetStartAndEndTimeL(startCalTime, endCalTime);

		TInt entriesAdded = 0;
		iTestLibrary->SynCGetEntryViewL().StoreL(entryArray, entriesAdded);
		test(entryArray.Count() == entriesAdded);


		CleanupStack::PopAndDestroy(&entryArray);



		// change the time to end one to simulate a DST

		iTestLibrary->PIMTestServer().SetHomeTime(dtEnd);
		

		// check the entry


		// fetch instances

		// create time filter
		
		TCalTime startTime, endTime;
		if (aFloating)
			{
			startTime.SetTimeLocalL(dtStart);
			endTime.SetTimeLocalL(dtStart + TTimeIntervalDays(1));
			}
		else
			{
			startTime.SetTimeLocalFloatingL(dtStart);
			endTime.SetTimeLocalFloatingL(dtStart + TTimeIntervalDays(1));
			}
		CalCommon::TCalTimeRange timeRange(startTime, endTime);
		
		// array to store the loaded instances
		
		RPointerArray<CCalInstance> instances;
		CleanupResetAndDestroyPushL(instances);


		iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
		// sanity check
		test(instances.Count() != 0);

		CCalInstance& instance = *(instances[0]);
		
		TTime entryStartLocalT(instance.Entry().StartTimeL().TimeLocalL());			
		TTime entryEndLocalT(instance.Entry().EndTimeL().TimeLocalL());			
		
		TTime instStartLocalT(instance.StartTimeL().TimeLocalL());			
		TTime instEndLocalT(instance.EndTimeL().TimeLocalL());
											
		test.Printf( _L("found instance: ") );
		TDateTime startDt = instStartLocalT.DateTime();			
		test.Printf( _L("%d/%d/%d, %d:%d:%d - "), 
					 startDt.Day()+1, startDt.Month()+1, startDt.Year(), 
					 startDt.Hour(),  startDt.Minute(),  startDt.Second() );			
		TDateTime endDt = instEndLocalT.DateTime();
		test.Printf( _L("%d/%d/%d, %d:%d:%d\n"),
					 endDt.Day()+1, endDt.Month()+1, endDt.Year(), 
					 endDt.Hour(),  endDt.Minute(),  endDt.Second() );

		// check the entry times					
		test( entryStartLocalT == startCalTime.TimeLocalL() );
		test( entryEndLocalT == endCalTime.TimeLocalL() );
		
		// check the instance times
		test( instStartLocalT == startCalTime.TimeLocalL() );
		test( instEndLocalT == endCalTime.TimeLocalL() );


		CleanupStack::PopAndDestroy(&instances);
		}
	
	
	CleanupStack::PopAndDestroy(&typeArray);
	}

/** @SYMTestCaseID     PIM-TCAL-TZ-0023 */
void CTzTestManager::CheckVcalImportFromS40L()
    {
    test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0023 Check alarm time on vcard import...")); 
    _LIT8(KS40Entry,    "BEGIN:VCALENDAR\r\n"
                        "VERSION:1.0\r\n"
                        "BEGIN:VEVENT\r\n"
                        "DTSTART:20060215T090000\r\n"
                        "DTEND:20060215T093000\r\n"
                        "AALARM:20060215T084500\r\n"
                        "DALARM:20060215T084500\r\n"
                        "RRULE:W1 20060510T090000\r\n"
                        "CATEGORIES:MEETING\r\n"
                        "SUMMARY:Meeting repeating weekly at 8 to 9\r\n"
                        "LOCATION:My location\r\n"
                        "X-NOK-CAT:icon_indi_cale_appointment\r\n"
                        "END:VEVENT\r\n"
                        "END:VCALENDAR\r\n");
                       
    _LIT(KFileName, "c:\\S40vcard.vcs");

   RFile outfile;
    TInt err = outfile.Replace(iTestLibrary->FileSession(), KFileName(), EFileWrite);
   
    User::LeaveIfError(err);
   
    outfile.Write(KS40Entry());   
    outfile.Close();

    RFile infile;
   
    User::LeaveIfError(infile.Open(iTestLibrary->FileSession(),KFileName,EFileRead));   

    CleanupClosePushL(infile);
   
    RFileReadStream readStream(infile);
    CleanupClosePushL(readStream);
   
    RPointerArray<CCalEntry> entryArray;
    CleanupResetAndDestroyPushL(entryArray);
   
    CCalDataExchange* sync = CCalDataExchange::NewL(iTestLibrary->GetSession());
    CleanupStack::PushL(sync);

    sync->ImportL(KUidVCalendar, readStream, entryArray);
   
    CleanupStack::PopAndDestroy(sync);   
    
    test(entryArray.Count()==1);

    CCalEntry* entry = entryArray[0];
    CCalAlarm* alarm = entry->AlarmL();
    TTimeIntervalMinutes alarmOffset = alarm->TimeOffset();
    delete alarm;
   
   
    TTime startTTime = entry->StartTimeL().TimeLocalL();
    TTime alarmTTime = startTTime - alarmOffset;//calculate actual alarm time
   
    TDateTime startTime = startTTime.DateTime();
    TDateTime alarmTime = alarmTTime.DateTime();

    test.Printf(_L("Imported startdate from S40: %d.%d.%d at %d:%d\n"),startTime.Day()+1, startTime.Month()+1, startTime.Year(), startTime.Hour(), startTime.Minute());
    test(startTTime == TTime(TDateTime(2006, EFebruary, 14, 9, 0, 0, 0)));
    test.Printf(_L("Imported alarm from S40: %d.%d.%d at %d:%d\n"),alarmTime.Day()+1, alarmTime.Month()+1, alarmTime.Year(), alarmTime.Hour(), alarmTime.Minute());
    test(alarmTTime == TTime(TDateTime(2006, EFebruary, 14, 8, 45, 0, 0)));
   
    CleanupStack::PopAndDestroy(3, &infile);//entryArray, readStream
    iTestLibrary->FileSession().Delete(KFileName);
    }

void CTzTestManager::SetupEventCrossingMidnightL()
	{
	iTestLibrary->CleanDatabaseL();
	
	HBufC8* guid = _L8("midnightcross").AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	// set event from 0300 to 2100 on 1 Jan 2006, local time Stockholm
	TCalTime start, end;
	start.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 3, 0, 0, 0));
	end.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 21, 0, 0, 0));
	entry->SetStartAndEndTimeL(start, end);
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	}

void CTzTestManager::CheckEventCrossingMidnightL()
	{
	// the event created in SetupEventCrossingMidnightL starts at
	// 1500 on 1 Jan 2006 and finishes on 0900 on 2 Jan 2006 in Auckland time
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	// check the event appears on the first day it appears
	TCalTime start1, end1;
	start1.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 0, 0, 0, 0));
	end1.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 23, 59, 59, 999));
	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(start1, end1));
	test(instances.Count() == 1);
	TCalTime instanceDate = instances[0]->Time();
	instances.ResetAndDestroy();

	// check the event appears on the second day it appears
	TCalTime start2, end2;
	start2.SetTimeLocalL(TDateTime(2006, EJanuary, 1, 0, 0, 0, 0));
	end2.SetTimeLocalL(TDateTime(2006, EJanuary, 1, 23, 59, 59, 999));
	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(start2, end2));
	test(instances.Count() == 1);
	TCalTime instanceDate2 = instances[0]->Time();
	test(instanceDate.TimeUtcL() == instanceDate2.TimeUtcL());
	
	CleanupStack::PopAndDestroy(&instances);
	}



void CTzTestManager::CheckDSTThresholdL()
	{
	test.Printf(_L("Start - CTestHarness::CheckDSTThresholdL\n"));
	iTestLibrary->CleanDatabaseL();
	
	_LIT8(KGuid1, "guid-1");
	_LIT(KTime1, "20030227:010000.000000");
	_LIT(KTime2, "20030227:030000.000000");
	_LIT(KTimeUntil, "20041116:000000.000000");
		
	HBufC8* guid8 = KGuid1().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EEvent, guid8, CCalEntry::EMethodAdd, 0);
	CleanupStack::Pop(guid8);
	CleanupStack::PushL(entry);

	TTime time1(KTime1());
	TTime time2(KTime2());
	TCalTime st;
	st.SetTimeUtcL(time1);
	TCalTime end;
	end.SetTimeUtcL(time2);
	entry->SetStartAndEndTimeL(st, end);
	
	TCalRRule rrule(TCalRRule::EDaily);
	TCalTime calTime = entry->StartTimeL();
	rrule.SetDtStart(calTime);
	rrule.SetInterval(1);
	TTime until(KTimeUntil());
	TCalTime untilTime;
	untilTime.SetTimeUtcL(until);
	rrule.SetUntil(untilTime); // where untilTime is 20041116:000000.000000
	entry->SetRRuleL(rrule);
	entry->SetTzRulesL();
	
	CCalAlarm* alarm=CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(0);
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	
	iTestLibrary->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);

	// check for instances
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	TCalTime start1, end1;
	start1.SetTimeLocalL(TDateTime(2004, EMarch, 27, 0, 0, 0, 0));
	end1.SetTimeLocalL(TDateTime(2004, EMarch, 27, 23, 59, 59, 999));
	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(start1, end1));
	
	for(TInt i=0; i < instances.Count(); i++)
		{
		CCalInstance* instance = instances[i];
		TCalTime insCalTime = instance->Time();
		TTime insTime = insCalTime.TimeLocalL();
		TDateTime dateTime = insTime.DateTime();
		test.Printf(_L("Local DateTime %d/%d/%d %d:%d\n"), dateTime.Day()+1, dateTime.Month()+1, dateTime.Year(), 
			dateTime.Hour(), dateTime.Minute());
		insTime = insCalTime.TimeUtcL();
		dateTime = insTime.DateTime();
		test.Printf(_L("Utc DateTime %d/%d/%d %d:%d\n"), dateTime.Day()+1, dateTime.Month()+1, dateTime.Year(), 
			dateTime.Hour(), dateTime.Minute());
		}

	// expect the instance in Mar 28 will be adjusted to 2am because 1am is non-existance local time
	test(instances.Count() == 1);
	TCalTime instanceDate = instances[0]->Time();
	TTime expectedTime(TDateTime(2004, EMarch, 27, 2, 0, 0, 0)); 
	test(instanceDate.TimeLocalL() == expectedTime);

	// test deleting instance in DST threshold
	iTestLibrary->SynCGetInstanceViewL().DeleteL(instances[0], CalCommon::EThisOnly);
	instances.Reset();
	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(start1, end1));	

	test(instances.Count() == 0);

	CleanupStack::PopAndDestroy(&instances);
	}

void CTzTestManager::TestAlarmOffsetL()
	{
	test.Printf(_L("Start - CTestHarness::TestAlarmOffsetL\n"));
		
	_LIT8(KGuid1, "guid-1");
	_LIT(KSummary1, "Test Entry 1");
	
	_LIT(KTime1, "20030227:010000.000000");
	_LIT(KTime2, "20030227:023000.000000");
	_LIT(KTimeUntil, "20041116:000000.000000");
	
	
	//entry 1	
	HBufC8* guid8 = HBufC8::NewL(KGuid1().Length());
	guid8->Des().Copy(KGuid1);	
	CleanupStack::PushL(guid8);
	CCalEntry* iEntry = CCalEntry::NewL(CCalEntry::EAppt, guid8, CCalEntry::EMethodAdd, (TUint)0);	
	CleanupStack::Pop(guid8);
	TTime time1(KTime1());
	TTime time2(KTime2());

	TCalTime st;
	st.SetTimeUtcL(time1);
		
	TCalTime end;
	end.SetTimeUtcL(time2);
	iEntry->SetStartAndEndTimeL(st, end);
	
	iEntry->SetSummaryL(KSummary1);
	
	
	TCalRRule* iRRule=new (ELeave) TCalRRule(TCalRRule::EMonthly);
	TCalTime calTime = iEntry->StartTimeL();
	iRRule->SetDtStart(calTime);
	iRRule->SetInterval(2);
	
	RArray<TCalRRule::TDayOfMonth>	arrayWeekInMonth;
	arrayWeekInMonth.Append(TCalRRule::TDayOfMonth(ESunday, -1));
	iRRule->SetByDay(arrayWeekInMonth);
	
	TTime until(KTimeUntil());
	TCalTime untilTime;
	untilTime.SetTimeUtcL(until);
	
	iRRule->SetUntil(untilTime); 
	iEntry->SetRRuleL(*iRRule);
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	alarm->SetTimeOffset(0);
	iEntry->SetAlarmL(alarm);
	
	TInt						entriesStored(0);
	RPointerArray<CCalEntry>	entries;
	CleanupResetAndDestroyPushL(entries);
	entries.AppendL(iEntry);
	iTestLibrary->SynCGetEntryViewL().StoreL(entries, entriesStored);
	arrayWeekInMonth.Close();
	delete iRRule;
	entries.ResetAndDestroy();
	
	iTestLibrary->SynCGetEntryViewL().FetchL(KGuid1(), entries);
	
	test(entries.Count() == 1);
	
	CCalAlarm* alarm2 = entries[0]->AlarmL();
	
	_LIT(KTimeFmt, "%F%Y%M%D-%H:%T:%S");
	TBuf<32> buf;
	entries[0]->StartTimeL().TimeUtcL().FormatL(buf, KTimeFmt);
	test.Printf(_L("Entry start time: %S"), &buf);
		
	test.Printf(_L("alarm offset before StoreL: %d"), alarm->TimeOffset().Int()); 
	test.Printf(_L("alarm offset after StoreL: %d"), alarm2->TimeOffset().Int()); 
			
	test(alarm->TimeOffset() == alarm2->TimeOffset()); 
	
	CleanupStack::PopAndDestroy(&entries);
	
		
	delete alarm;
	delete alarm2;
	}

void CTzTestManager::DoCancel()
	{
	}

void CTzTestManager::RunL()
	{
	test.Printf(_L("CTzTestManager::RunL - test step %d\n"), iTestStep);
	TBool completed = EFalse;
	
	switch (iTestStep++)
		{
		case 0:
			test.Printf(_L("Start no floating entry test\n"));
			TestSetAndGetTzRulesL();
			break;
		case 1:
			TestSetTimeZoneL(KAsiaTokyo);
			break;
		case 2:
			TestRemoteRepeatL();
			break;
		case 3:
			TestSetTimeZoneL(KEuropeLondon);
			break;
		case 4:
			CheckEntryInTimeZoneL(TTime(KTokyoEntryTime) - (TTimeIntervalHours)9);
			break;
		case 5:
			TestSetTimeZoneL(KPacificAuckland);
			break;
		case 6:
			CheckEntryInTimeZoneL(TTime(KTokyoEntryTime) + (TTimeIntervalHours)4);
			break;
		case 7:
			TestSetTimeZoneL(KAsiaTokyo);
			break;
		case 8:
			TestRepeatRuleTzSetupL(EFalse);
			break;
		case 9:
			TestSetTimeZoneL(KEuropeLondon);
			break;
		case 10:
			TestRepeatRuleLocalTimeL();
			break;
		case 11:
			TestRepeatRuleTzRulesL();
			break;
		case 12:
			TestSetTimeZoneL(KAsiaTokyo);
			break;
		case 13:
			TestRepeatRuleTzStartEndTimeL();
			TestRepeatRuleTzStartEndTzRulesL();
			break;
		case 14:
			TestSetTimeZoneL(KEuropeLondon);
			break;
		case 15:
			DefectTestL();
			TestRDatesTzSetupL();
			TestSetTimeZoneL(KEuropeStockholm); // Set device timezone
			break;
		case 16:
			TestForeignZoneRepeatRule1L();
			TestSetTimeZoneL(KEuropeLondon);
			break;
		case 17:
			TestForeignZoneRepeatRule2L();
			TestLocalRepeatRuleL();
			break;
		case 18:
			TestSetTimeZoneL(KUsCentral);
			break;
		case 19:
			TestLocalRepeatRule2L();
			TestLocalTimeZoneL();
			break;
		case 20:
			/* @SYMTestCaseID PIM-TCAL-TZ-Floating_Entry_UTC_Change
			   @SYMTestCaseDesc Set and get time for floating entries with utc change
			   @SYMFssID App-Engines/CalInterimAPI/Data.007
			   @SYMTestStatus Implemented
			   @SYMTestPriority Medium
			   @SYMTestActions 1) Create floating entry, change timezone, check that new time values are as expected
					2) Create floating repeating entry, check time stored, change timezone, check that new time values are as expected
			   		3) Create floating repeating entry with exception, check time stored, change timezone, check that new time values are as expected
			   		4) Create floating repeating entry with RDate, check time stored, change timezone, check that new time values are as expected
			   @SYMTestExpectedResults The check shows that the expected time values are found
			   @SYMTestType CT
			*/

			test.Printf(_L("Start floating entry test\n"));
			TestFloatingEntrySetupL();			
			break;
		case 21:
			TestSetTimeZoneL(KPacificAuckland);
			break;
		case 22:
			TestFloatingEntryCheckL(TTime(KTokyoEntryTime));
			TestRepeatRuleTzSetupL(ETrue);		
			TestFloatingRepeatCheckL(); // check floating entry without timezone change		
			break;
		case 23:
			TestSetTimeZoneL(KEuropeLondon);
			break;
		case 24:
			TestFloatingRepeatCheckL(); // check floating entry after timezone change
			TestRepeatRuleTzSetupL(ETrue, ETrue);			
			TestFloatingExceptionCheckL(); // check floating entry without timezone change
			break;
		case 25:
			TestSetTimeZoneL(KPacificAuckland);
			break;
		case 26:
			TestFloatingExceptionCheckL(); // check floating entry after timezone change
			TestRepeatRuleTzSetupL(ETrue, EFalse, ETrue);
			TestFloatingRDateCheckL(); // check floating entry without timezone change
			break;
		case 27:
			TestSetTimeZoneL(KEuropeLondon);
			break;
		case 28:
			TestFloatingRDateCheckL(); // check floating entry after timezone change
			break;
		case 29:
			TestSetTimeZoneL(KEuropeStockholm);
			break;
		case 30:
			TestAlarmTzUpdateSetupL();
			break;
		case 31:
			TestSetTimeZoneL(KEuropeLondon);
			break;
		case 32:
			TestAlarmTzUpdateCheckL();
			TestCopyRepeatRulesL();
			TestCreateEntryWithMidnightInHomeTimeL();
			TestCreateEntryWithMidnightInHomeTimeL(ETrue);
			TestCreateEntryWithDstChangeL();
			TestCreateEntryWithDstChangeL(ETrue);
			break;
		case 33:
			TestSetTimeZoneL(KEuropeLondon);
			break;
		case 34:
			ImportFloatingEntryL();
			break;
		case 35:
			TestSetTimeZoneL(KEuropeTirane);
			break;
		case 36:
			TestTimeModeL();
			break;
		case 37:
			// southern hemisphere
			TestSetTimeZoneL(KPacificAuckland);
			break;
		case 38:
			TestExportTzRulesInHemisphereL(ETrue);
			break;
		case 39:
			TestSetTimeZoneL(KEuropeStockholm);
			break;
		case 40:
			TestExportTzRulesInHemisphereL(EFalse);
			CheckVcalImportFromS40L();
			// northern hemisphere
			SetupEventCrossingMidnightL();
			break;
		case 41:
			// southern hemisphere
			TestSetTimeZoneL(KPacificAuckland);
			break;
		case 42:
			// southern hemisphere
			SetupEventCrossingMidnightL();
			break;
		case 43:
    		TestSetTimeZoneL(KEuropeLondon);
			break;
		case 44:
		    CheckDSTThresholdL();
		    TestAlarmOffsetL();	
			break;
		case 45:
			TestSetTimeZoneL(KPacificHonolulu);
			break;
		case 46:
			{
			TInt aExpectedOffset = -600; 		//utc offset for hawaii time zone is -600mins
			TDateTime dtStart(2006,ESeptember,1,10,0,0,0);
			TCalTime startCalTime;
			startCalTime.SetTimeLocalL(dtStart);
			CheckUtcOffsetL(startCalTime, aExpectedOffset);
			}
			break;
		case 47:
			completed = ETrue;
			break;
		}
	
	if (completed)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		Start();
		}
	}

/** @SYMTestCaseID     PIM-TCAL-TZ-0024 */
void CTzTestManager::CheckUtcOffsetL(TCalTime& aTestLocalTime, TInt aExpectedOffset)
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0024 Check for Utc offset")); 		
	TTime localTime = aTestLocalTime.TimeLocalL();
	TTime utcTime = aTestLocalTime.TimeUtcL();
	TTimeIntervalMinutes utcOffset;
	test(localTime.MinutesFrom(utcTime, utcOffset) == KErrNone);
	test(utcOffset.Int() == aExpectedOffset);
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CTzTestManager* testManager = CTzTestManager::NewLC();

	TPerformanceTimer timer(test);
	timer.Start();

	// Run the test suite
	testManager->Start();
	CActiveScheduler::Start();

	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	CleanupStack::PopAndDestroy(testManager);
	}


/** @SYMTestCaseID     PIM-TCAL-TZ-0001 */
TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-TZ-0001 Calendar Interim API Time Zone test suite")); 	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler();
	if (!scheduler)
		{
		delete trapCleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);	

	TRAPD(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
