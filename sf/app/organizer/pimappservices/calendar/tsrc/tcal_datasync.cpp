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
#include <caldataformat.h>
#include <s32file.h>
#include <e32test.h>
#include <calentry.h>
#include <calentryview.h>
#include <calsession.h>
#include <calrrule.h>
#include <tz.h>
#include <caldataexchange.h>

RTest test(_L("tcal_datasync"));
_LIT(KCalendarFile1,"c:tcal_datasync1");
_LIT(KCalendarFile2,"c:tcal_datasync2");


const TInt KNumOfEntriesToTest = 500;


class CDataSyncTestManager : public CBase
	{
public:
    static CDataSyncTestManager* NewLC();
    ~CDataSyncTestManager();
	
	// Test Methods
	void TestConsistentLocalIdL();
	void TestFileIdL();
	void TestGetIdsModifiedSinceDateL();
	void TestSyncDuringDeleteL();
	void TestGetIdsModifiedOnCurrentTimeL();
	void TestDeleteParentAndChildEntriesL();
	
private:
	void ConstructL();
	
	void CreateEntriesL(RPointerArray<CCalEntry>& aCalEntryArray, TInt aNumOfEntries);


private:
	CCalTestLibrary* iTestLibrary;
	
	};


CDataSyncTestManager* CDataSyncTestManager::NewLC()
	{
	CDataSyncTestManager* self = new(ELeave) CDataSyncTestManager;
	
	CleanupStack::PushL(self);
	self->ConstructL();

	return (self);
	}


void CDataSyncTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	iTestLibrary->ReplaceFileL(KCalendarFile1);
	iTestLibrary->OpenFileL(KCalendarFile1);	
	}


CDataSyncTestManager::~CDataSyncTestManager()	
	{
	TRAP_IGNORE(iTestLibrary->DeleteFileL(KCalendarFile1));
	TRAP_IGNORE(iTestLibrary->DeleteFileL(KCalendarFile2));
	delete iTestLibrary;
	}


void CDataSyncTestManager::CreateEntriesL(RPointerArray<CCalEntry>& aCalEntryArray, TInt aNumOfEntries)
	{
	TCalTime dummyTime;
	dummyTime.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 0, 0, 0, 0));
	for (TInt i(0) ; i < aNumOfEntries ; ++i)
		{
		HBufC8* uid = HBufC8::NewLC(255);
		TPtr8 ptr = uid->Des();
		iTestLibrary->RandomText8(ptr);
		//create and append the entry
		CCalEntry* calEntry = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, uid);
		CleanupStack::Pop(uid);
		CleanupStack::PushL(calEntry);
		calEntry->SetStartAndEndTimeL(dummyTime, dummyTime);
		aCalEntryArray.AppendL(calEntry);
		CleanupStack::Pop(calEntry);
		}
	}



void CDataSyncTestManager::TestGetIdsModifiedSinceDateL()
	{
	test.Next(_L("Test TestGetIdsModifiedSinceDateL"));

	// test we don't get back unmodified entries and deleted entries
	
	RPointerArray<CCalEntry> calEntryArray;
	CleanupResetAndDestroyPushL(calEntryArray);
	RArray<TCalLocalUid> calLocalIds;
	CleanupClosePushL(calLocalIds);
	RArray<TCalLocalUid> calLocalIdsToDelete;
	CleanupClosePushL(calLocalIdsToDelete);
	
	TInt entriesModified(0);
	TInt deletedEntries(0);
	
	// create some entries
	CreateEntriesL(calEntryArray, KNumOfEntriesToTest);
	TInt numCompleted(0);
	test.Printf(_L("Total number of entries created and stored = %d\n"), calEntryArray.Count());
	iTestLibrary->SynCGetEntryViewL().StoreL(calEntryArray, numCompleted);
	test(calEntryArray.Count() == numCompleted);
	
	calEntryArray.ResetAndDestroy();
	
	
	TCalTime calTime2000;
	calTime2000.SetTimeLocalL(TTime(_L("20000101:120000.000000")));
	
	iTestLibrary->SynCGetEntryViewL().GetIdsModifiedSinceDateL(calTime2000, calLocalIds);
	test(calLocalIds.Count()==KNumOfEntriesToTest);

	// update a third of the entries 
	const TInt KThirdEntries = KNumOfEntriesToTest / 3;
	test.Printf(_L("number of entries modified = %d\n"), KThirdEntries);
	for (TInt i(0) ; i < KThirdEntries ; ++i)
		{
		CCalEntry* calEntryFetched = iTestLibrary->SynCGetEntryViewL().FetchL(calLocalIds[i]);
		calEntryFetched->SetLastModifiedDateL();
		calEntryArray.AppendL(calEntryFetched);
		++entriesModified;
		}

	// record the time
	TCalTime calTime;
	TTime time;
	time.HomeTime();
	calTime.SetTimeLocalL(time);

	iTestLibrary->SynCGetEntryViewL().UpdateL(calEntryArray, numCompleted);	
	
	// delete half of those
	const TInt KHalfUpdatedEntries = KThirdEntries / 2;
	test.Printf(_L("number of modified entries that we have deleted = %d\n"), KHalfUpdatedEntries);
	for (TInt i(0) ; i < KHalfUpdatedEntries ; ++i)
		{
		calLocalIdsToDelete.AppendL(calLocalIds[i]);
		++deletedEntries;
		}
		
	// delete the entries by local id entry	
	TInt numEntries(0);
	iTestLibrary->SynCGetEntryViewL().DeleteL(calLocalIdsToDelete, numEntries);
	
	test(numEntries == calLocalIdsToDelete.Count());
	
	calLocalIds.Reset();
	// There should be five modified since the date
	iTestLibrary->SynCGetEntryViewL().GetIdsModifiedSinceDateL(calTime, calLocalIds);
	test.Printf(_L("number of returned entries from GetIdsModifiedSinceDateL = %d\n"), calLocalIds.Count());
	test( calLocalIds.Count() == (entriesModified - deletedEntries) );
	
	iTestLibrary->CleanDatabaseL();
	
	CleanupStack::PopAndDestroy(&calLocalIdsToDelete);
	CleanupStack::PopAndDestroy(&calLocalIds);
	CleanupStack::PopAndDestroy(&calEntryArray);
	}
	
	

void CDataSyncTestManager::TestGetIdsModifiedOnCurrentTimeL()
	{
	test.Next(_L("Test TestGetIdsModifiedSinceDate1L"));

	_LIT8(KEntry,	"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:1\r\n"
 			   		"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:20050415:133000.000000\r\n"
					"DTEND:20050415:143000.000000\r\n"
					"SUMMARY:hi there\r\n"
					"END:VEVENT\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:2\r\n"
 			   		"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:20050415:133000.000000\r\n"
					"DTEND:20050415:143000.000000\r\n"
					"SUMMARY:hi there\r\n"
					"END:VEVENT\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:3\r\n"
 			   		"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:20050415:133000.000000\r\n"
					"DTEND:20050415:143000.000000\r\n"
					"SUMMARY:hi there\r\n"
					"END:VEVENT\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:4\r\n"
 			   		"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:20050415:133000.000000\r\n"
					"DTEND:20050415:143000.000000\r\n"
					"SUMMARY:hi there\r\n"
					"END:VEVENT\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:5\r\n"
 			   		"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:20050415:133000.000000\r\n"
					"DTEND:20050415:143000.000000\r\n"
					"SUMMARY:hi there\r\n"
					"END:VEVENT\r\n"
					"END:VCALENDAR\r\n");
	
    _LIT(KFileName, "c:\\modifiedvcard.vcs");

	TTime oldTime; 
	oldTime.HomeTime();
	RTz tzServer;
	
	User::LeaveIfError(tzServer.Connect());
	CleanupClosePushL(tzServer);

	tzServer.SetHomeTime(TTime(_L("19990627:000000.000000")));

    RFile outfile;
    TInt err = outfile.Replace(iTestLibrary->FileSession(), KFileName(), EFileWrite);
    User::LeaveIfError(err);
  
    outfile.Write(KEntry());   
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
   
    test(entryArray.Count()==5);

    TInt success = 0;
    iTestLibrary->SynCGetEntryViewL().StoreL(entryArray, success);
    test(entryArray.Count() == success);
    
    CleanupStack::PopAndDestroy(3, &infile);//entryArray, readStream

    iTestLibrary->DeleteFileL(KFileName, 0);
    
	RArray<TCalLocalUid> calLocalIds;
	CleanupClosePushL(calLocalIds);
	
	tzServer.SetHomeTime(TTime(_L("19990627:010000.000000")));
	TCalTime calTime2000;
	calTime2000.SetTimeLocalL(TTime(_L("19990627:000000.000000")));

	iTestLibrary->SynCGetEntryViewL().GetIdsModifiedSinceDateL(calTime2000, calLocalIds);
	// There should be five modified since the time of creation
	test.Printf(_L("number of returned entries from GetIdsModifiedSinceDateL = %d\n"), calLocalIds.Count());
	test(calLocalIds.Count()==5);
	
	calLocalIds.Reset();

	// There should be no modified since current time
	calTime2000.SetTimeLocalL(TTime(_L("19990627:010000.000000")));
	iTestLibrary->SynCGetEntryViewL().GetIdsModifiedSinceDateL(calTime2000, calLocalIds);
	test.Printf(_L("number of returned entries from GetIdsModifiedSinceDateL = %d\n"), calLocalIds.Count());
	test( calLocalIds.Count() == 0 );
	
	iTestLibrary->CleanDatabaseL();
	tzServer.SetHomeTime(oldTime);

	CleanupStack::PopAndDestroy(2, &tzServer);//calLocalIds
	}



void CDataSyncTestManager::TestFileIdL()
	{	
	test.Next(_L("Test the file ids are different"));

	TCalFileId fileId1;
	TCalFileId fileId2;
	iTestLibrary->GetSession().FileIdL(fileId1);
	iTestLibrary->ReplaceFileL(KCalendarFile2);
	iTestLibrary->OpenFileL(KCalendarFile2);
	iTestLibrary->GetSession().FileIdL(fileId2);
	
	test.Printf(_L("File ID 1 = %d\nFile ID 2 =  %d\n"), fileId1, fileId2);
	
	test(fileId1 != fileId2);
	
	// reopen the original file
	iTestLibrary->OpenFileL(KCalendarFile1);
	}



void CDataSyncTestManager::TestConsistentLocalIdL()
	{//This test is to test the fix for the defect  DEF067513  "Local UID is not preserved when Updating CCalEntrys" 

	test.Next(_L("Test entry's local uid is the same after being updated"));

	
	
	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
	

	_LIT8(KUid, "UID_BB");

	TInt entriesStored = 0;
	HBufC8* 	guid = NULL;
	CCalEntry*	entry = NULL;


//
	test.Printf(_L("Create a simple entry and store it to the file\n"));
	
	TBuf<50> summary;
	TBuf<50> location;
	TBuf<50> description;

	guid = KUid().AllocLC();
	entry = iTestLibrary->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	iTestLibrary->SetEntryStartAndEndTimeL(entry);		
	iTestLibrary->RandomText(summary);
	entry->SetSummaryL(summary);
	iTestLibrary->RandomText(location);
	entry->SetLocationL(location);
	iTestLibrary->RandomText(description);
	entry->SetDescriptionL(description);
	
	User::LeaveIfError(entriesToStore.Append(entry));
	CleanupStack::Pop(entry);

	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);
	
	TCalLocalUid storedUid1 = entriesToStore[0]->LocalUidL();
	test.Printf(_L("storedUid1 = %d\n"), storedUid1);


	entriesToStore.ResetAndDestroy();


//
	test.Printf(_L("Fetch and store it again to make sure local UId is still the same\n"));

	iTestLibrary->SynCGetEntryViewL().FetchL(KUid, entriesToStore);
	test(entriesToStore.Count() == 1);
	
	TCalLocalUid fetchedUid = entriesToStore[0]->LocalUidL();
	test.Printf(_L("fetchedUid = %d\n"), fetchedUid);	
	
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);	

	TCalLocalUid storedUid2 = entriesToStore[0]->LocalUidL();
	test.Printf(_L("storedUid2 = %d\n"), storedUid2);
	
	test(storedUid1 == fetchedUid);
	test(storedUid2 == fetchedUid);	


	entriesToStore.ResetAndDestroy();


//
	test.Printf(_L("Delete entries from the file\n"));
	
	iTestLibrary->CleanDatabaseL();	
		
	iTestLibrary->SynCGetEntryViewL().FetchL(KUid, entriesToStore);
	test(entriesToStore.Count() == 0);

	
	entriesToStore.ResetAndDestroy();
	
	
//
	test.Printf(_L("Create a repeating entry (parent) and store it to the file\n"));
		
	guid = KUid().AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	TTime startTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	TTime endTime (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)); 
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);

	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EDaily);
	CleanupStack::PushL(rpt);
	TTime daily1end(TDateTime(2005, EJanuary, 9, 10, 0, 0, 0));
	TCalTime rptendTime;
	rptendTime.SetTimeLocalL(daily1end);

	rpt->SetDtStart(calStartTime);
	rpt->SetUntil(rptendTime);
	rpt->SetInterval(1);
	entry->SetRRuleL(*rpt);//take a copy of the rule
	CleanupStack::PopAndDestroy(rpt);
	
	entriesToStore.Append(entry);
	CleanupStack::Pop(entry);
	
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);


	entriesToStore.ResetAndDestroy();


//
	test.Printf(_L("Create a child entry and store it\n"));
			
	guid = KUid().AllocLC();
	TTime ttime (TDateTime(2005, EJanuary, 8, 10, 0, 0, 0));
	TCalTime recurrenceId;
	recurrenceId.SetTimeUtcL(ttime);
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodRequest, 0, recurrenceId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	// Start date same as RecId. End time is 1 hour later.
	TTime newStartDate (TDateTime(2005, EJanuary, 8, 11, 0, 0, 0));
	TTime newEndDate (TDateTime(2005, EJanuary, 8, 12, 0, 0, 0));
	
	TCalTime newCalstartTime;
	newCalstartTime.SetTimeUtcL(newStartDate);
	
	TCalTime newCalendTime;
	newCalendTime.SetTimeUtcL(newEndDate);
	
	entry->SetStartAndEndTimeL(newCalstartTime, newCalendTime);
	
	entriesToStore.Append(entry);
	CleanupStack::Pop(entry);

	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);	


	entriesToStore.ResetAndDestroy();

	
//
	test.Printf(_L("Store the parent which should delete the child\n"));
	
	iTestLibrary->SynCGetEntryViewL().FetchL(KUid, entriesToStore);
	test(entriesToStore.Count() == 2); // 1 parent and 1 child so far
	
	CCalEntry* parent = entriesToStore[0];
	TCalLocalUid parentId = parent->LocalUidL();
	test.Printf(_L("parentId = %d\n"), parentId);
	CCalEntry* child = entriesToStore[1];
	entriesToStore.Remove(1);
	delete child;

 	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
 	test(entriesToStore.Count() == entriesStored);

	entriesToStore.ResetAndDestroy();
	
	iTestLibrary->SynCGetEntryViewL().FetchL(KUid, entriesToStore);
	test(entriesToStore.Count() == 1);//make sure children is deleted from the file when the parent is updated
	TCalLocalUid fetchedParentId = entriesToStore[0]->LocalUidL();
	test.Printf(_L("fetchedParentId = %d\n"),fetchedParentId);
	
	test(parentId == fetchedParentId);


	entriesToStore.ResetAndDestroy();


//
	test.Printf(_L("Delete entries from the file\n"));
	
	iTestLibrary->CleanDatabaseL();

	iTestLibrary->SynCGetEntryViewL().FetchL(KUid, entriesToStore);
	test(entriesToStore.Count() == 0);


	CleanupStack::PopAndDestroy(&entriesToStore);
	}

void CDataSyncTestManager::TestSyncDuringDeleteL()
	{
	test.Printf(_L("Adding 1000 entries\n"));
	_LIT8(KGuid, "entryToDelete");
	TBuf8<32> buffer;
	TCalTime startTime;
	const TTime KInitialTime(TDateTime(2006, EJanuary, 0, 0, 0, 0, 0));
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	for (TInt i = 0; i < 1000; i++)
		{
		buffer.Copy(KGuid);
		buffer.AppendNum(i);
		HBufC8* guid = buffer.AllocLC();
		CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(entry);
		
		startTime.SetTimeLocalL(KInitialTime + TTimeIntervalDays(i));
		entry->SetStartAndEndTimeL(startTime, startTime);
		TCalRRule rule(TCalRRule::EDaily);
		rule.SetDtStart(startTime);
		rule.SetCount(50);
		entry->SetRRuleL(rule);
		
		entryArray.AppendL(entry);
		CleanupStack::Pop(entry);
		
		if (i % 100 == 99)
			{
			TInt success = 0;
			iTestLibrary->SynCGetEntryViewL().StoreL(entryArray, success);
			test.Printf(_L("Stored %d entries...\n"), i+1);
			entryArray.ResetAndDestroy();
			}
		}
	CleanupStack::PopAndDestroy(&entryArray);
	
	test.Printf(_L("Delete all entries asynchronously...\n"));
	// now delete all entries
	TCalTime minTime, maxTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	iTestLibrary->SynCGetEntryViewL().DeleteL(CalCommon::TCalTimeRange(minTime, maxTime), CalCommon::EIncludeAll, *iTestLibrary);
	
	test.Printf(_L("Fetch all local uids\n"));
	RArray<TCalLocalUid> luids;
	CleanupClosePushL(luids);
	TRAPD(err, iTestLibrary->SynCGetEntryViewL().GetIdsModifiedSinceDateL(minTime, luids));
	test(err == KErrLocked);
	CleanupStack::PopAndDestroy(&luids);
	
	test.Printf(_L("Complete delete operation\n"));
	// wait for entries to be deleted
	CActiveScheduler::Start();
	
	iTestLibrary->SynCGetEntryViewL().GetIdsModifiedSinceDateL(minTime, luids);
	test(luids.Count() == 0);
	}



void CDataSyncTestManager::TestDeleteParentAndChildEntriesL()
	{
	test.Next(_L("Create many parent and child entries and delete them all by local UID"));

	
	const TInt KNumEntryPairs = 50;
	_LIT8(KGuidStart, "uid:");
	TBuf8<16> guidBuf;
	TTime startTime(TDateTime(2006, EJanuary, 0, 9, 0, 0, 0)); // 0900 on 1 Jan
	for (TInt i = 0; i < KNumEntryPairs; i++, startTime += TTimeIntervalDays(1))
		{
		guidBuf.Copy(KGuidStart);
		guidBuf.AppendNum(i);
		HBufC8* guid = guidBuf.AllocLC();
		
		CCalEntry* parent = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(parent);

		TCalTime calStartTime;
		calStartTime.SetTimeLocalL(startTime);
		parent->SetStartAndEndTimeL(calStartTime, calStartTime);
		
		TCalRRule rule(TCalRRule::EDaily);
		rule.SetDtStart(calStartTime);
		rule.SetCount(5);
		parent->SetRRuleL(rule);

		iTestLibrary->StoreEntryL(*parent);
		CleanupStack::PopAndDestroy(parent);
		
		TCalTime recId;
		recId.SetTimeLocalL(startTime + TTimeIntervalDays(2));
		guid = guidBuf.AllocLC();
		
		CCalEntry* child = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(child);

		child->SetStartAndEndTimeL(recId, recId);
		
		iTestLibrary->StoreEntryL(*child);
		CleanupStack::PopAndDestroy(child);
		}
	
	TCalTime sinceDate;
	sinceDate.SetTimeLocalL(TCalTime::MinTime());
	
	RArray<TCalLocalUid> luids;
	CleanupClosePushL(luids);
	iTestLibrary->SynCGetEntryViewL().GetIdsModifiedSinceDateL(sinceDate, luids);
	TInt success = 0;
	iTestLibrary->SynCGetEntryViewL().DeleteL(luids, success);
	test(success == luids.Count());
	CleanupStack::PopAndDestroy(&luids);
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CDataSyncTestManager* testManager = CDataSyncTestManager::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	testManager->TestConsistentLocalIdL();
	testManager->TestGetIdsModifiedSinceDateL();
	testManager->TestGetIdsModifiedOnCurrentTimeL();
	testManager->TestFileIdL();
	testManager->TestSyncDuringDeleteL();
	testManager->TestDeleteParentAndChildEntriesL();
	

	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
	CleanupStack::PopAndDestroy(testManager);		
	}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         


/**

@SYMTestCaseID     PIM-TCAL-DATASYNC-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-DATASYNC-0001 Calendar Interim API Data Sync test suite"));

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

