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

#include "tcal_compact.h"

CCompactTestManager* CCompactTestManager::NewLC()
	{
	CCompactTestManager* self = new (ELeave) CCompactTestManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}

CCompactTestManager* CCompactTestManager::NewL()
	{
	CCompactTestManager* self = CCompactTestManager::NewLC();
	CleanupStack::Pop(self);
	return (self);
	}

void CCompactTestManager::ConstructL()
	{	
	iTestLibrary = CCalTestLibrary::NewL();	
	iTestLibrary->ReplaceFileL(KCalendarFile());
	iTestLibrary->OpenFileL(KCalendarFile());
	}

CCompactTestManager::CCompactTestManager()
	{
	}
	
CCompactTestManager::~CCompactTestManager()
	{
	TRAP_IGNORE(iTestLibrary->DeleteFileL(KCalendarFile()));
	delete iTestLibrary;
	}

// Populate the Calendar file with random entries exceeding the operations threshold triggering compact
// Compare file sizes before adding and after deleting the same entry to ensure compacting was triggered
void CCompactTestManager::TestSynchronousCompactingL()
	{	
	// Add multiple entries to the calendar file ensuring that the number of operations does not exceed 
	// the compact threshold defined in the agenda model
	AddEntriesToCalendarFileL(KMinEntriesBeforeCompact);
	
	// The steps below shall add and delete an entry and compare the file sizes before and after.
	// Adding the entry is expected to trigger the compacting and thus once the same entry is deleted,
	// the file size of the Calendar file after deleting the entry is expected to be less than its size before 
	// adding the entry
	
	// Record the Calendar file size before initiating compacting
	TInt fileSizeBeforeCompact = iTestLibrary->PIMTestServer().GetFileSizeL(KCalendarFilePath);
	
	test.Printf(_L("Calendar file size before compacting = %d"), fileSizeBeforeCompact);
		
	// Record timestamp before adding an entry that would trigger compacting
	TTime univTime;
	univTime.UniversalTime();
	User::After(500000);
	
	TCalTime timeStamp;
	timeStamp.SetTimeUtcL(univTime);
	
	// Exceed the operations threshold which will trigger the compacting
	AddEntriesToCalendarFileL(1);
	
	RArray<TCalLocalUid> entriesLocalUids;
	CleanupClosePushL(entriesLocalUids);

	iTestLibrary->SynCGetEntryViewL().GetIdsModifiedSinceDateL(timeStamp, entriesLocalUids); 
	test(entriesLocalUids.Count()==1);

	// Delete the last entry
	TInt entriesDeleted;
	iTestLibrary->SynCGetEntryViewL().DeleteL(entriesLocalUids,entriesDeleted);
	test(entriesDeleted==1);
		
	CleanupStack::PopAndDestroy(&entriesLocalUids);
	
	// Measure the file size after adding and deleting the same entry and thus verify compacting
	TInt fileSizeAfterCompact = iTestLibrary->PIMTestServer().GetFileSizeL(KCalendarFilePath);
	test.Printf(_L("Calendar file size after compacting = %d"), fileSizeAfterCompact);
	
	// Compacting should have reduced the size of the Calendar file even after adding and deleting 
	// the same entry
	test(fileSizeAfterCompact<fileSizeBeforeCompact);
	}

// Add fixed number of random entries to the Calendar file in one go
void CCompactTestManager::AddEntriesToCalendarFileL(TInt aNumEntries)
	{
	RPointerArray<CCalEntry> entriesToAdd;
	CleanupResetAndDestroyPushL(entriesToAdd);
	
	// Set up start and end dates
	TTime startTime;
	startTime.UniversalTime();
	
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime + TTimeIntervalDays(1));

	TTime endTime(startTime + TTimeIntervalHours(4));
	
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime + TTimeIntervalDays(1));

	TCalTime calRecIdTime;
	calRecIdTime.SetTimeLocalL(startTime + TTimeIntervalMonths(1));
	
	// Create alarm and repeat rule objects for use later on
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	
	// Daily repeat
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(1);
	rule.SetDtStart(calStartTime);
	
	_LIT(KSummaryText, "abcdefghijklmnopqrstuvwxyz...abcdefghijklmnopqrstuvwxyz...abcdefghijklmnopqrstuvwxyz...abcdefghijklmnopqrstuvwxyz...abcdefghijklmnopqrstuvwxyz...");
	
	for (TInt i = 0; i < aNumEntries; ++i)
		{
		// get UID
		TBuf8<50> buf;
		iTestLibrary->RandomText8(buf);

		HBufC8* guid = buf.AllocLC();
		
		// create basic entry
		CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(entry);

		// set basic data
		entry->SetStartAndEndTimeL(calStartTime, calEndTime);
		entry->SetDescriptionL(_L("dummy description"));
		entry->SetLocationL(_L("dummy location"));

		alarm->SetTimeOffset(i);
		entry->SetAlarmL(alarm);
		entry->SetSummaryL(KSummaryText());
		
		AddCategoriesAndAttendeesToEntryL(*entry);
		
		entry->SetRRuleL(rule); 

		entriesToAdd.AppendL(entry);
		
		CleanupStack::Pop(entry);
		}
		
		TInt entriesAdded = 0;
		if (entriesToAdd.Count() > 0)
			{
			test.Printf(_L("About to store %d entries, "), entriesToAdd.Count());
			// store any remaining entries
			iTestLibrary->SynCGetEntryViewL().StoreL(entriesToAdd, entriesAdded);
			
			test(entriesAdded == entriesToAdd.Count());
			
			entriesToAdd.ResetAndDestroy();
			test.Printf(_L("done\n"));
			}
					
	CleanupStack::PopAndDestroy(alarm);
	CleanupStack::Pop(); // entriesToAdd
	}

void CCompactTestManager::AddCategoriesAndAttendeesToEntryL(CCalEntry& aEntry)
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
	
static void DoTestL()
	{
	CCompactTestManager* testManager = CCompactTestManager::NewLC();

	testManager->TestSynchronousCompactingL();
		
	CleanupStack::PopAndDestroy(testManager);
	}

/**

@SYMTestCaseID     PIM-TCAL-COMPACT-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-COMPACT-0001 Calendar Interim API - Calendar File Compacting Unit Test"));

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
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
