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

// local includes
#include "caltestlib.h"

// System includes
#include <e32test.h>
#include <calalarm.h>
#include <calcategory.h>
#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caliterator.h>
#include <calrrule.h>
#include <calsession.h>
#include <caluser.h>
#include <tz.h>
#include <vtzrules.h>

#include <hal.h>

 
static RTest test(_L("tcal_startup_performance"));

_LIT8(KEntryGuid, "A_DUMMY_GUID");

_LIT(KCalendarFile, "tcal_startup_performance");
_LIT(KCapacityTestFile, "tcal_performance_capacity");
_LIT(KCalDirectory, "c:\\private\\10003a5b\\");

// Constants
const TUint KNumEntries = 3000;


class CStartupPerformanceTestManager : public CBase
	{
public:

	// Constructor / Destructor
	static CStartupPerformanceTestManager* NewLC();
	~CStartupPerformanceTestManager();
	
	// Test suite
	void CreateCalendarFileL();
	void TimeOpeningCalendarFileL();
	void TimeFindingEntriesInAWeekL();
	void TestDatabaseCapacityL();

private:

	// avoid any method but NewL instancing the class
	CStartupPerformanceTestManager();
	// no copy constructor and assignment operator
	CStartupPerformanceTestManager(CStartupPerformanceTestManager& );
	CStartupPerformanceTestManager& operator = (const CStartupPerformanceTestManager& );

	void ConstructL();
	

	void AddCategoriesAndAttendeesToEntryL(CCalEntry& aEntry);
	void AddEntriesL(CCalEntry::TType aType);
	void CapacityTestAddEntriesL(TInt& aEntriesAdded);


private:

	CCalTestLibrary*	iTestLibrary;
	TCalTime			iFirstCalEntryStartTime;
	TTime				iInitialTime;
	TInt                iEntryCounter;
	};


// Constructor / Destructor
	
CStartupPerformanceTestManager* CStartupPerformanceTestManager::NewLC()
	{
	CStartupPerformanceTestManager* self = new (ELeave) CStartupPerformanceTestManager;

	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}

CStartupPerformanceTestManager::CStartupPerformanceTestManager()
	{
	}

void CStartupPerformanceTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL(EFalse);
	iInitialTime.HomeTime();
	iFirstCalEntryStartTime.SetTimeLocalL(Time::NullTTime());
	iTestLibrary->PIMTestServer().SetHomeTime(TDateTime(2006, EJanuary, 1, 0, 0, 0, 0)); // Jan 2nd 2006
	}


CStartupPerformanceTestManager::~CStartupPerformanceTestManager()
	{
	if (iTestLibrary)
		{
		iTestLibrary->PIMTestServer().SetHomeTime(iInitialTime); 
		}
	delete iTestLibrary;
	}
	
	
void CStartupPerformanceTestManager::CreateCalendarFileL()
	{
	test.Printf(_L("Creating calendar file...\n"));	
	
	iTestLibrary->ReplaceFileL(KCalendarFile());
	
	test.Printf(_L("Opening calendar file...\n"));
		
	iTestLibrary->OpenFileL(KCalendarFile());


	test.Printf(_L("Populating calendar file with %d entries...\n"), KNumEntries);

	TTime start, end;
	start.UniversalTime();

	iEntryCounter = 0;
	TRAPD(err, 
		{
		AddEntriesL(CCalEntry::EAppt);	
		AddEntriesL(CCalEntry::ETodo);
		AddEntriesL(CCalEntry::EEvent);
		AddEntriesL(CCalEntry::EReminder);
		AddEntriesL(CCalEntry::EAnniv);
		});
		
	if (err == KErrNoMemory)
		{
		test.Printf(_L("\nWARNING: Out of Memory Error trapped when adding entries\n"));
		test.Printf(_L("Added %d entries of %d\n"), iEntryCounter, KNumEntries);
		}
	else if (err != KErrNone)
		{
		// Re-throw error
		User::Leave(err);
		}
		
	end.UniversalTime();

	// get the time it took to populate the calendar

	TTimeIntervalMicroSeconds ms = end.MicroSecondsFrom(start);
	test.Printf(_L("Time to populate calendar: %d milliseconds\n"), ms.Int64()/1000);
	}


void CStartupPerformanceTestManager::AddCategoriesAndAttendeesToEntryL(CCalEntry& aEntry)
	{
	_LIT(KDummyCategoryName, "new category");
	_LIT(KDummyAddress1, "dummy address 1");
	_LIT(KDummyAddress2, "dummy address 2");
	_LIT(KDummyAddress3, "dummy address 3");
	_LIT(KDummyAddress4, "dummy address 4");
	_LIT(KDummyAddress5, "dummy address 5");
	_LIT(KDummySentBy1, "dummy sent by 1");
	_LIT(KDummySentBy2, "dummy sent by 2");
	
	// add categories and attendees to 1 in 10 entries
	CCalCategory* category1 = CCalCategory::NewL(KDummyCategoryName);
	CleanupStack::PushL(category1);
	
	CCalCategory* category2 = CCalCategory::NewL(CCalCategory::ECalHoliday);
	CleanupStack::PushL(category2);
	
	CCalAttendee* attendee1 = CCalAttendee::NewL(KDummyAddress1);
	CleanupStack::PushL(attendee1);
	
	CCalAttendee* attendee2 = CCalAttendee::NewL(KDummyAddress2);
	CleanupStack::PushL(attendee2);
	
	CCalAttendee* attendee3 = CCalAttendee::NewL(KDummyAddress3);
	CleanupStack::PushL(attendee3);
	
	CCalAttendee* attendee4 = CCalAttendee::NewL(KDummyAddress4, KDummySentBy1);
	CleanupStack::PushL(attendee4);
	
	CCalAttendee* attendee5 = CCalAttendee::NewL(KDummyAddress5, KDummySentBy2);
	CleanupStack::PushL(attendee5);
	
	aEntry.AddCategoryL(category1); 
	aEntry.AddCategoryL(category2); 
	
	aEntry.AddAttendeeL(attendee1); 
	aEntry.AddAttendeeL(attendee2); 
	aEntry.AddAttendeeL(attendee3); 
	aEntry.AddAttendeeL(attendee4); 
	aEntry.AddAttendeeL(attendee5); 

	CleanupStack::Pop(attendee5);
	CleanupStack::Pop(attendee4);
	CleanupStack::Pop(attendee3);
	CleanupStack::Pop(attendee2);
	CleanupStack::Pop(attendee1);
	
	CleanupStack::Pop(category2);
	CleanupStack::Pop(category1);
	}


// Add 1000 (KNumEntries / 5) entries of a particular type. These entry's data is specified
// by the following test data:
//
// For each 1000 entries,
// 400 entries will contain basic details (start and end time, location and description)
// 100 entries will contain basic details plus an alarm
// 100 entries will contain basic details plus a 100+ character summary
// 100 entries will contain basic details plus 2 categories and 5 attendees
// 100 entries will contain basic details plus a repeat rule (repeating daily forever)
// 100 entries will contain basic details but will be modifying entries of an existing repeating entries (Group Scheduling functionality)
// 100 entries will contain basic details plus a 100+ character summary, 2 categories and 5 attendees and a repeat rule
// 
void CStartupPerformanceTestManager::AddEntriesL(CCalEntry::TType aType)
	{
	const TInt KNumEntriesOfEachType = KNumEntries / 5;
	
	test.Printf(_L("adding %d entries of type %d\n"), KNumEntriesOfEachType, aType);
	
	TInt entriesAdded = 0;
	RPointerArray<CCalEntry> entriesToAdd;
	CleanupResetAndDestroyPushL(entriesToAdd);
	
	// set up start and end dates
	TTime startTime;
	startTime.UniversalTime();
	
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime + TTimeIntervalDays(1));

	if (iFirstCalEntryStartTime.TimeLocalL() == Time::NullTTime())
		{
		iFirstCalEntryStartTime = calStartTime;
		}

	TTime endTime(startTime + TTimeIntervalHours(4));
	
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime + TTimeIntervalDays(1));

	TCalTime calRecIdTime;
	if (aType == CCalEntry::ETodo)
		{
		calRecIdTime.SetTimeLocalL(endTime + TTimeIntervalMonths(1));
		}
	else
		{
		calRecIdTime.SetTimeLocalL(startTime + TTimeIntervalMonths(1));
		}
	
	// create alarm and repeat rule objects for use later on
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(1);
	rule.SetDtStart(calStartTime);
	
	_LIT(KSummaryText, "abcdefghijklmnopqrstuvwxyz...abcdefghijklmnopqrstuvwxyz...abcdefghijklmnopqrstuvwxyz...abcdefghijklmnopqrstuvwxyz...abcdefghijklmnopqrstuvwxyz...");
	
	// split the entries into 10ths. Work out which 10th this entry is
	const TInt KTenthOfEntries = KNumEntriesOfEachType / 10;
	for (TInt i = 0; i < KNumEntriesOfEachType; ++i)
		{
		// get UID
		TBuf8<50> buf;
		
		buf.Copy(KEntryGuid());
		buf.AppendNum(aType);
		buf.Append(_L8("_"));
		buf.AppendNum(i);
		HBufC8* guid = buf.AllocLC();
		
		// create basic entry
		CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(entry);

		// set basic data
		entry->SetStartAndEndTimeL(calStartTime, calEndTime);
		entry->SetDescriptionL(_L("dummy description"));
		entry->SetLocationL(_L("dummy location"));

		// add extra data depending on which 10th this is
		TInt tenth = i / KTenthOfEntries;
		switch (tenth)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				// do nothing for first 4/10ths
				break;
			
			case 4:
				// add alarm to 1 in 10 entries
				alarm->SetTimeOffset(i);
				entry->SetAlarmL(alarm); 
				break;
				
			case 5:
				// add summary to 1 in 10 entries
				entry->SetSummaryL(KSummaryText());
				break;
				
			case 6:
				// add categories and attendees to 1 in 10 entries
				AddCategoriesAndAttendeesToEntryL(*entry);
				break;
				
			case 7:
                {
				// add repeat rule (daily forever) to 1 in 10 entries
				entry->SetRRuleL(rule); 

                // Set this entry's time zone to something other than the 
                // current time zone.
                TUint vcrTzId = 1320;  // America/Vancouver time zone id.
                CTzId* tzId = CTzId::NewL( vcrTzId );
                CleanupStack::PushL( tzId );

                // Create the tz rules.
               	RTz tz;
               	User::LeaveIfError( tz.Connect() );
               	CleanupClosePushL( tz );
                TTime startTime = entry->StartTimeL().TimeUtcL();
                TTime endTime = entry->EndTimeL().TimeUtcL();
                CTzRules* rules = tz.GetTimeZoneRulesL( *tzId, startTime, 
                    endTime, ETzUtcTimeReference );
                CleanupStack::PushL( rules );

                // Set tz rules for the entry.
                entry->SetTzRulesL( *rules );
                CleanupStack::PopAndDestroy( 3 );  // tzId, tz, rules
				break;
				}

			case 8:
				// 1 in 10 entries is a modifying entry of an original repeating entry
				// (Group Scheduling functionality)
				delete entry;
				entry = NULL;
				
				buf.Copy(KEntryGuid());
				buf.AppendNum(aType);
				buf.Append(_L8("_"));
				buf.AppendNum(i - KTenthOfEntries);
				guid = buf.AllocLC();
				
				entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodRefresh, 1, 
										calRecIdTime, CalCommon::EThisAndFuture);
				CleanupStack::Pop(guid);
				entry->SetStartAndEndTimeL(calStartTime, calEndTime);
				break;
				
			case 9:
				// 1 in 10 entries has a summary, categories, attendees and a repeat rule
				entry->SetSummaryL(KSummaryText());
				AddCategoriesAndAttendeesToEntryL(*entry);
				entry->SetRRuleL(rule); 
				break;
				
			default:
				User::Leave(KErrCorrupt);
				break;
			}

		entriesToAdd.AppendL(entry);
		
		CleanupStack::Pop(entry);

		// store entries after 100 have been created
		if (i % 100 == 99)
			{
			test.Printf(_L("About to store %d entries, "), entriesToAdd.Count());
			TRAPD(err, iTestLibrary->SynCGetEntryViewL().StoreL(entriesToAdd, entriesAdded));
			iEntryCounter += entriesAdded;				
			User::LeaveIfError(err);
			
			test(entriesAdded == entriesToAdd.Count());
			
			entriesToAdd.ResetAndDestroy();
			test.Printf(_L("done\n"));
			}
		}
		
	CleanupStack::PopAndDestroy(alarm);

	if (entriesToAdd.Count() > 0)
		{
		test.Printf(_L("About to store %d entries, "), entriesToAdd.Count());
		// store any remaining entries
		iTestLibrary->SynCGetEntryViewL().StoreL(entriesToAdd, entriesAdded);
		
		test(entriesAdded == entriesToAdd.Count());
		
		entriesToAdd.ResetAndDestroy();
		test.Printf(_L("done\n"));
		}
	
	CleanupStack::Pop(); // entriesToAdd.Close()
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Test cases forming the Startup Performance test suite
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// the main routine for timing the opening of the calendar file
void CStartupPerformanceTestManager::TimeOpeningCalendarFileL()
	{
	test.Printf(_L("\n_______________\n"));
	test.Printf(_L("Closing and re-opening calendar file...\n"));

	TInt ramBeforeOpenEntryView = 0;
	TInt ramAfter = 0;

	TTime startOpen, endOpen;
	startOpen.UniversalTime();
	delete iTestLibrary;
	iTestLibrary = CCalTestLibrary::NewL(EFalse);
	iTestLibrary->OpenFileL(KCalendarFile());
	
	endOpen.UniversalTime();
	TTimeIntervalMicroSeconds msOpen = endOpen.MicroSecondsFrom(startOpen);
	test.Printf(_L("Time to close and reopen calendar file: %d milliseconds\n"), msOpen.Int64()/1000);
	//
	test.Printf(_L("\n_______________\n"));
	test.Printf(_L("Opening entry view...\n"));
	
	TTime startView, endView;
	startView.UniversalTime();
	
	User::LeaveIfError(HAL::Get(HALData::EMemoryRAMFree, ramBeforeOpenEntryView));
	
	
	CCalEntryView& calEntryView = iTestLibrary->SynCGetEntryViewL();
		
	endView.UniversalTime();
	// get the time it took to load the calendar
	TTimeIntervalMicroSeconds msView = endView.MicroSecondsFrom(startView);
	test.Printf(_L("Time to open entry view: %d milliseconds\n"), msView.Int64()/1000);

	User::LeaveIfError(HAL::Get(HALData::EMemoryRAMFree, ramAfter));
	test.Printf(_L("RAM used after built indexes: %d bytes\n"), ramBeforeOpenEntryView - ramAfter);
	}


void CStartupPerformanceTestManager::TimeFindingEntriesInAWeekL()
	{
	test.Printf(_L("_______________\n"));
	test.Printf(_L("Load instances within a week...\n"));	


	// create time (week) filter

	// Due to the large number of entries in the database, this search only gets 30 hours worth of instances.
	// 7 days worth of instances returns about 9000 instances which causes an out of memory error.
	
	// this test fetches 1400 instances
	TTime startSearch = iFirstCalEntryStartTime.TimeLocalL() + TTimeIntervalDays(4);
	
	TCalTime startTime, endTime;
	startTime.SetTimeLocalL(startSearch);
	endTime.SetTimeLocalL(startSearch + TTimeIntervalHours(30));
	
	CalCommon::TCalTimeRange timeRange( startTime, endTime );

	TInt ramBefore = 0;
	TInt ramAfter = 0;

	User::LeaveIfError(HAL::Get(HALData::EMemoryRAMFree, ramBefore));

	// array to store the loaded instances
	
	RPointerArray<CCalInstance> instanceArray;
	CleanupResetAndDestroyPushL(instanceArray);


	// measure the time to performe the operation

	TTime startFind, endFind;
	startFind.UniversalTime();
	
	TInt filter = CalCommon::EIncludeAll;
	TRAPD(err, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instanceArray, filter, timeRange));
	
	if (err != KErrNone)
		{
		test.Printf(_L("Couldn't fetch instances, error %d\n"), err);	
		}
	else
		{
		endFind.UniversalTime();
		// get the time it took to find the instances
		TTimeIntervalMicroSeconds msFind = endFind.MicroSecondsFrom(startFind);
		test.Printf(_L("Time to open entry view: %d milliseconds\n"), msFind.Int64()/1000);
		
		test.Printf(_L("Found %d instances\n"), instanceArray.Count());	

		User::LeaveIfError(HAL::Get(HALData::EMemoryRAMFree, ramAfter));
		TInt ramUsed = ramBefore - ramAfter;
		test.Printf(_L("RAM used: %d bytes\n"), ramUsed);
		}
	
	CleanupStack::PopAndDestroy();
	
	TFileName calFile(KCalDirectory);
	calFile.Append(KCalendarFile);
	TInt fileSize = iTestLibrary->PIMTestServer().GetFileSizeL(calFile);
	test.Printf(_L("Calendar file size: %d bytes\n"), fileSize);
	}

void CStartupPerformanceTestManager::CapacityTestAddEntriesL(TInt& aEntriesAdded)
	{
	_LIT8(KGuid, "guid");
	_LIT(KDummySummary, "summary");
	
	TBuf8<16> uidBuf;
	
	TCalTime time;
	time.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 0, 0, 0, 0));
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(1);
	rule.SetCount(5);
	rule.SetDtStart(time);
	
	const TInt KEntriesAddedThisTime = aEntriesAdded + 100;
	
	while (aEntriesAdded < KEntriesAddedThisTime)
		{
		uidBuf.Copy(KGuid);
		uidBuf.AppendNum(aEntriesAdded);
		HBufC8* guid = uidBuf.AllocLC();
		CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(entry);
		
		entry->SetStartAndEndTimeL(time, time);
		entry->SetSummaryL(KDummySummary);
		
		if (aEntriesAdded % 3 == 0)
			{
			entry->SetRRuleL(rule); // 1 in 3 entries has a repeat rule
			}
		
		iTestLibrary->StoreEntryL(*entry);
		aEntriesAdded++;
		
		CleanupStack::PopAndDestroy(entry);
		}
	}

void CStartupPerformanceTestManager::TestDatabaseCapacityL()
	{
	test.Printf(_L("\n_______________\n"));
	test.Printf(_L("Testing database capacity\n"));
	iTestLibrary->ReplaceFileL(KCapacityTestFile());
	iTestLibrary->OpenFileL(KCapacityTestFile());
	
	TTime start, end;
	start.UniversalTime();
	
	TBool databaseFull = EFalse;
	TInt entriesAdded = 0;
	while ( ! databaseFull)
		{
		TRAPD(err, CapacityTestAddEntriesL(entriesAdded));
		test.Printf(_L("Added %d entries...\n"), entriesAdded);
		if (err == KErrNoMemory || err == KErrDiskFull)
			{
			databaseFull = ETrue;
			}
		else if (err != KErrNone)
			{
			test.Printf(_L("Failed to add entry %d, error=%d...\n"), entriesAdded, err);
			User::Leave(err);
			}
		}
	end.UniversalTime();
	// get the time it took to find the instances
	TTimeIntervalMicroSeconds ms = end.MicroSecondsFrom(start);
	test.Printf(_L("Time to fill database: %d milliseconds\n"), ms.Int64()/1000);

	test.Printf(_L("Database can store %d entries\n"), entriesAdded);
	
	// print out the time it took to load the calendar instances
	
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CStartupPerformanceTestManager* testManager = CStartupPerformanceTestManager::NewLC();

		
	// Run the test suite

	test.Printf(_L("Start up performance test\n"));

	testManager->CreateCalendarFileL();
	testManager->TimeOpeningCalendarFileL();
	testManager->TimeFindingEntriesInAWeekL();
	CleanupStack::PopAndDestroy(testManager);
	
	testManager = CStartupPerformanceTestManager::NewLC();
	testManager->TestDatabaseCapacityL();
		
	CleanupStack::PopAndDestroy(testManager);
	}

static void DoCleanupL()
	{
	CCalTestLibrary* testLib = CCalTestLibrary::NewLC();
	testLib->DeleteFileL(KCapacityTestFile);
	testLib->DeleteFileL(KCalendarFile);
	CleanupStack::PopAndDestroy(testLib);	
	}

/**

@SYMTestCaseID     PIM-TCAL-STARTUP-PERFORMANCE-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-STARTUP-PERFORMANCE-0001 Calendar Interim API Startup Performance test suite"));

	test.Title();

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
	TRAP_IGNORE(DoCleanupL());
	if (ret != KErrNone)
		{
		test.Printf(_L("Test failed with error %d\n"), ret);
		}
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
