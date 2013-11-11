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

#include "caltestlib.h"
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calrrule.h>
#include <e32test.h>
#include <s32file.h>
#include <vtzrules.h>

static RTest test(_L("tcal_tz_instance"));

// Device time zone
_LIT8(KAmericaChicago, "America/Chicago");

class CCalTzInstanceTest : public CBase
	{
public:
	static CCalTzInstanceTest* NewLC();
 	void DoTestL();
    void DoCountYearlyByDayTestL();
    void DoUntilAfterDSTChangeTestL();
    void DoCountTestL();
	~CCalTzInstanceTest();

	void DoOOMTestTzClientCacheClearingL();
	void SupportCodeTestTzClientCacheClearingL();
private:
	void ConstructL();

private:
	CCalTestLibrary* iTestLib;
	};

_LIT(KCalName, "TestCalendarFile");

void CCalTzInstanceTest::DoCountYearlyByDayTestL()
    {
    iTestLib->CleanDatabaseL();
   
    TCalRRule rule(TCalRRule::EYearly);
   
    // create entry repeating on the second Tuesday on August for four instances, setting the Count
    TCalTime dtStart;
    dtStart.SetTimeLocalL(TDateTime(2006, EAugust, 0, 7, 0, 0, 0));
    rule.SetDtStart(dtStart);
   
    RArray<TMonth> months;
    CleanupClosePushL(months);
    months.AppendL(EAugust);
    rule.SetByMonth(months);
    CleanupStack::PopAndDestroy(&months);
   
    RArray<TCalRRule::TDayOfMonth> dayofmonths;
    CleanupClosePushL(dayofmonths);
    dayofmonths.AppendL(TCalRRule::TDayOfMonth(ETuesday, 2));
    rule.SetByDay(dayofmonths);
    CleanupStack::PopAndDestroy(&dayofmonths);
   
    rule.SetCount(4);
   
    HBufC8* guid = _L8("guid").AllocLC();
    CCalEntry* dummy = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(guid);
    CleanupStack::PushL(dummy);
   
    dummy->SetStartAndEndTimeL(dtStart, dtStart);
    dummy->SetRRuleL(rule);
   
    TCalRRule rule2;
    dummy->GetRRuleL(rule2);
   
    TDateTime endDate = rule2.Until().TimeLocalL().DateTime();
   
    RPointerArray<CCalEntry> entryArray;
    CleanupClosePushL(entryArray);
    entryArray.AppendL(dummy);
    TInt success = 0;
    iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
    CleanupStack::PopAndDestroy(&entryArray);
   
    CleanupStack::PopAndDestroy(dummy);
   
    // Define the time range to find the instances
    TCalTime startTime;
    TCalTime endTime;
    startTime.SetTimeUtcL(TCalTime::MinTime());
    endTime.SetTimeUtcL(TCalTime::MaxTime());
   
    CalCommon::TCalTimeRange timeRange(startTime, endTime);
   
    RPointerArray<CCalInstance> instanceList;
   
    // Find all the instances within the specified time range
    iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceList, CalCommon::EIncludeAll, timeRange);
   
    // Display all the instances found
    for (TInt instanceCounter = 0; instanceCounter < instanceList.Count(); ++instanceCounter)
        {
        TDateTime instanceDateTime = instanceList[instanceCounter]->StartTimeL().TimeUtcL().DateTime();
        RDebug::Print(_L("Found instance at %d:%d on %d/%d/%d"),
        instanceDateTime.Hour(), instanceDateTime.Minute(), instanceDateTime.Day()+1, instanceDateTime.Month()+1, instanceDateTime.Year());
        }

    // Verify the number of instances found
    test(instanceList.Count() == 4);
       
    instanceList.ResetAndDestroy();
    }

void CCalTzInstanceTest::DoUntilAfterDSTChangeTestL()
    {
    iTestLib->CleanDatabaseL();

	RPointerArray<CCalEntry> entries;
	
	CCalEntry* entry;
	HBufC8* uid;
	TCalTime startTime;
	TCalTime endTime;
	TCalTime untilTime;
	TCalRRule rrule;
	CTzRules* tzRules;
		
	// Allocate a UID.
	uid = HBufC8::NewL(50);
	uid->Des().Copy(_L8("{70706C03-015C-4550-BFAF-1B7067BF0141}"));
	
	// Symbian's day of month values are 0-based.
	startTime.SetTimeUtcL(TTime(TDateTime(2005, EMarch, 19, 19, 00, 0, 0))); 		
	endTime.SetTimeUtcL(TTime(TDateTime(2005, EMarch, 19, 19, 30, 0, 0)));			
	untilTime.SetTimeUtcL(TTime(TDateTime(2005, EApril, 9, 19, 00, 0, 0)));
	
	// Create the recurrence rule
	rrule.SetDtStart( startTime );
	rrule.SetInterval( 1 );
	rrule.SetUntil( untilTime );
	rrule.SetType( TCalRRule::EDaily );
	
	// Create time zone rules
	tzRules = CTzRules::NewL();
	tzRules->SetInitialStdTimeOffset( +420 );
	tzRules->SetStartYear( 0 );
	tzRules->SetEndYear( 9999 );
	// Arizona does not observe daylight savings time, so do not add any additional rules.

	// Setting current device Time Zone to USA/Chicago (Observers DST)
	//
	// USA, Chicago - UTC-6hrs Winter Time and UTC-5hrs Summer Time
	// DST starts on Sunday, 2 April 2005, 02:00 local standard time 
	// DST ends on Sunday, 29 October 2005, 02:00 local daylight time

	// Device Time Zone 
	iTestLib->SetTimeZoneL(KAmericaChicago);
				
	// Create the  entry.
	entry = CCalEntry::NewL( CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0 );
	
	entry->SetStartAndEndTimeL( startTime, endTime );
	
	entry->SetSummaryL( _L("Daily 3/20/2005 thru 4/10/2005 at 12:00pm") );
	
	// Set entry's repeat rule
	entry->SetRRuleL( rrule );
	
	// Set entry's repeat rule's time zone (different from the device time zone)
	entry->SetTzRulesL( *tzRules );
	
	delete tzRules;

	entries.Append( entry );
	
	// Store the recurring entry
	TInt successCount;
	TRAPD( err, iTestLib->SynCGetEntryViewL().StoreL( entries, successCount ) );
	
	// CHECKPOINT 1: Check that all the entries were successfully added (one entry)
	test(err == KErrNone && successCount == entries.Count());
	
	entries.ResetAndDestroy();
		
	// Define the time range to find the instances
	startTime.SetTimeUtcL(TCalTime::MinTime());
	endTime.SetTimeUtcL(TCalTime::MaxTime());
	
	CalCommon::TCalTimeRange timeRange(startTime, endTime);
	
	RPointerArray<CCalInstance> instanceList;
	
    // Find all the instances
    iTestLib->SynCGetInstanceViewL().FindInstanceL( instanceList, CalCommon::EIncludeAll, timeRange );
	
	// Display all the instances found
	for (TInt instanceCounter = 0; instanceCounter < instanceList.Count(); ++instanceCounter)
		{
		TDateTime instanceDateTime = instanceList[instanceCounter]->StartTimeL().TimeUtcL().DateTime();
		RDebug::Print(_L("Found instance at %d:%d on %d/%d/%d"),
		instanceDateTime.Hour(), instanceDateTime.Minute(), instanceDateTime.Day()+1, instanceDateTime.Month()+1, instanceDateTime.Year());
		}
	
	// Retrieve the last instance / last day for testing purposes
	CCalInstance* lastInstance = instanceList[instanceList.Count() - 1];
	
	TInt lastDay = lastInstance->StartTimeL().TimeUtcL().DateTime().Day();
	
	// CHECKPOINT 2: Check that the last day is valid
	test(lastDay == untilTime.TimeUtcL().DateTime().Day());

	instanceList.ResetAndDestroy();
	}

void CCalTzInstanceTest::DoCountTestL()
    {
    iTestLib->CleanDatabaseL();
   
    RPointerArray<CCalEntry> entries;
   
    // create entry with repeat rule set by the Count, then change the time zone and check end date is correct
    CCalEntry* entry;
    HBufC8* uid;
    TCalTime startTime;
    TCalTime endTime;
    TCalTime untilTime;
    TCalRRule rrule;
    CTzRules* tzRules;
       
    // Allocate a UID.
    uid = HBufC8::NewL(50);
    uid->Des().Copy(_L8("{70706C03-015C-4550-BFAF-1B7067BF0141}"));
   
    // Symbian's day of month values are 0-based.
    startTime.SetTimeUtcL(TTime(TDateTime(2005, EMarch, 19, 19, 00, 0, 0)));         
    endTime.SetTimeUtcL(TTime(TDateTime(2005, EMarch, 19, 19, 30, 0, 0)));           
   
    // Create the recurrence rule
    rrule.SetDtStart( startTime );
    rrule.SetInterval( 1 );
    rrule.SetCount( 22 );
    rrule.SetType( TCalRRule::EDaily );
   
    // Create time zone rules
    tzRules = CTzRules::NewL();
    tzRules->SetInitialStdTimeOffset( -420 );
    tzRules->SetStartYear( 0 );
    tzRules->SetEndYear( 9999 );

    // Create the  entry.
    entry = CCalEntry::NewL( CCalEntry::EAppt, uid, CCalEntry::EMethodNone, 0 );
   
    entry->SetStartAndEndTimeL( startTime, endTime );
   
    entry->SetSummaryL( _L("Daily 3/20/2005 thru 4/10/2005 at 12:00pm") );
   
    // Set entry's repeat rule
    entry->SetRRuleL( rrule );
   
    // Set entry's repeat rule's time zone (different from the device time zone)
    entry->SetTzRulesL( *tzRules );
   
    delete tzRules;

    entries.Append( entry );
   
    // Store the recurring entry
    TInt successCount;
    TRAPD( err, iTestLib->SynCGetEntryViewL().StoreL( entries, successCount ) );
   
    // CHECKPOINT 1: Check that all the entries were successfully added
    test(err == KErrNone && successCount == entries.Count());
   
    entries.ResetAndDestroy();
       
    // Define the time range to find the instances
    startTime.SetTimeUtcL(TCalTime::MinTime());
    endTime.SetTimeUtcL(TCalTime::MaxTime());
   
    CalCommon::TCalTimeRange timeRange(startTime, endTime);
   
    RPointerArray<CCalInstance> instanceList;
   
    // Find all the instances
    iTestLib->SynCGetInstanceViewL().FindInstanceL( instanceList, CalCommon::EIncludeAll, timeRange );
   
    // CHECKPOINT 2: Check all the instances are found
    test(instanceList.Count() == 22);

    instanceList.ResetAndDestroy();
    }

CCalTzInstanceTest* CCalTzInstanceTest::NewLC()
	{
	CCalTzInstanceTest* self = new (ELeave) CCalTzInstanceTest();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}

void CCalTzInstanceTest::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	
	iTestLib->ReplaceFileL(KCalName());
	iTestLib->OpenFileL(KCalName());
	}
	
CCalTzInstanceTest::~CCalTzInstanceTest()
	{
	delete iTestLib;
	}


/**
 @SYMTestCaseID PIM-TCAL-TZ-INSTANCE-0002
 @SYMDEF DEF113539
 @SYMTestType UT
 @SYMTestPriority NORMAL
 @SYMDEF DEF113539
 @SYMTestCaseDesc Check if cache at Tz Client is cleared using the API ClearTzClientCacheL().
 
 @SYMTestActions
 1) Clean Tz Clientside cache before starting OOM test.
 2) Run OOM test.
 3) Clean Tz Clientside cache allocated inside OOM loop.
 4) Test if any memory allocated inside OOM loop has not been released.
 5) Clean Tz Clientside cache and RESTART caching.

 @SYMTestExpectedResults There should not be any memory leaks.
*/ 	

void CCalTzInstanceTest::DoOOMTestTzClientCacheClearingL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-TZ-INSTANCE-0002 Testing Functionality for cleaning up Tz client cache."));

	
	TInt tryCount = 1;
	TInt err = 0;
	TBool cleanCache = EFalse;
	
	// OOM LOOP BEGINS
	for ( ;; )
		{
		iTestLib->__dbgClearTzClientCacheL(cleanCache);
		
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		__UHEAP_MARK;
		TRAP(err, SupportCodeTestTzClientCacheClearingL());

		iTestLib->__dbgClearTzClientCacheL(cleanCache);
	
		if ( err==KErrNone ) 
			{
			__UHEAP_RESET;
			RDebug::Printf("OOM test for checking cleaning of Tz client cache compelete");
			break;
			}
		test(err == KErrNoMemory);
		__UHEAP_MARKEND;
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		++tryCount;
		}
	// OOM LOOP ENDS
	cleanCache = ETrue;
	iTestLib->__dbgClearTzClientCacheL(cleanCache); //RESTARTING CACHING
	}

/**
Support function for OOM test function DoOOMTestTzClientCacheClearingL.
Code attempts to convert different times so that Tz client cache keep variying.
*/
void CCalTzInstanceTest::SupportCodeTestTzClientCacheClearingL()
	{
	
	TTime gettime;
	TCalTime calStartTime;
	
	TTime recentTime(TDateTime(2005, EMay, 12, 9, 0, 0, 0)); 
	calStartTime.SetTimeUtcL(recentTime);
	
	TTime eighties(TDateTime(1985, EAugust, 12, 9, 0, 0, 0)); 
	calStartTime.SetTimeLocalL(eighties);
	gettime = calStartTime.TimeUtcL();
	
	TTime seventies(TDateTime(1975, EAugust, 12, 9, 0, 0, 0)); 
	calStartTime.SetTimeLocalL(seventies);
	gettime = calStartTime.TimeUtcL();
	
	TTime sixties(TDateTime(1965, EAugust, 12, 9, 0, 0, 0)); 
	calStartTime.SetTimeUtcL(sixties);
	gettime = calStartTime.TimeLocalL();
	
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CCalTzInstanceTest* testManager = CCalTzInstanceTest::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite
	
    testManager->DoCountYearlyByDayTestL();
    testManager->DoUntilAfterDSTChangeTestL();
    testManager->DoCountTestL();
	testManager->DoOOMTestTzClientCacheClearingL();

    
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

    
	CleanupStack::PopAndDestroy(testManager);
	}
	

/**

@SYMTestCaseID     PIM-TCAL-TZ-INSTANCE-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-TZ-INSTANCE-0001 Calendar Interim API Time Zone and Instance test suite"));

	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;

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
