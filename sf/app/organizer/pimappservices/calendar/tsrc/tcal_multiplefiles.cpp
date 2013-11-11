// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <calsession.h>
#include <e32test.h>
#include <caliterator.h>
#include <calrrule.h>
#include <s32file.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <e32base.h>
#include <calinstanceiterator.h>
#include <calattachment.h>
#include <calattachmentmanager.h>

_LIT(KTestName,"TCal_MultipleFiles");
_LIT(KWorkCalendar,"Work_Calendar");
_LIT(KPersonalCalendar,"Personal_Calendar");
_LIT(KSportsCalendar, "Sports_Calendar");
_LIT(KWorkCalendarIndexFile, "c:\\private\\10003a5b\\work_calendarIdx");

RTest test(KTestName);
const TInt KShortFileId1 = 1;
const TInt KShortFileId2 = 2;
_LIT8(KUid, "MFTUid");
_LIT8(KUid1, "MFTUid1");


class CTestFileObservers : public CBase, MCalChangeCallBack2
	{
public:
	static CTestFileObservers* NewL(const TDesC& aFileName);
	// from MCalChangeCallBack2
	void CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems);
	~CTestFileObservers();
		
private:
	void ConstructL(const TDesC& aFileName);
private:
	CCalSession* iNotificationSession;
	};

class CTestMultipleCalendar : public CBase, MCalProgressCallBack
	{
public:
    static CTestMultipleCalendar* NewL();
	void RunAllTestsL();
	~CTestMultipleCalendar();	
	
private:
	void ConstructL();
	CTestMultipleCalendar();
	
	// Test cases
	void TestEntryViewAndFileConsistencyL(TBool aAddingSameEntry);
	void TestInstanceViewL();
	void TestFindInstanceL(TBool aSessionToFindInstanceFrom, TInt aExpectedValue);
	void TestFindInstanceWithInstanceIdL();
	void TestUpdateDeleteFunctionalityL();
	void TestShortFileIdL();
	void TestSwitchInstanceViewsL();
	void TestNegativeForSecondSessionL();
	void TestNegativeForCollectionIdL();
	void TestCopyEntryBetweenCalendarsL();
	
	// Data creation and check functions
	TCalInstanceId AddAndCheckEntryL(CCalEntryView& aEntryView, CCalSession& aSession, TBool aSameEntry = ETrue);
	void AddLargeNumberOfEntriesL(CCalEntryView& aEntryView);
	void CreateNotificationSessionL();
	void CreateSecondSessionL();
	void CreateAndTestInstanceIteratorL(CCalInstanceView& aInstanceView, TInt aExpectedCount);
	TInt CheckIteratorL(CCalInstanceIterator& aIterator, RPointerArray<CCalInstance>& aInstances);
	TBool CheckInstanceIdL(TCalInstanceId aId1, TCalInstanceId aId2);
	void CheckInstanceDetailsL(CCalInstance* aInstance, TCalInstanceId aInstanceId);
	void CheckInstanceViewL(CCalInstanceView& aInstanceView, TInt aNumInstance);
	void DeleteSecondCalendarFileL();
	void ClearStoredDataL();
	
	void Progress(TInt aPercentageCompleted); 
	void Completed(TInt aError);
	TBool NotifyProgress();
	
private:
	CCalTestLibrary* iTestLib;
	TInt 	iProgress;
	CCalSession* iSecondSession;
	TCalTime iStartTime;
	TCalTime iEndTime;
	CTestFileObservers* iObserverForWorkCalendar;
	CTestFileObservers* iObserverForPersonalCalendar;
	};

CTestFileObservers* CTestFileObservers::NewL(const TDesC& aFileName)
	{
	CTestFileObservers* self = new (ELeave) CTestFileObservers();
	CleanupStack::PushL(self);
	self->ConstructL(aFileName);
	CleanupStack::Pop(self);
	return self;	
	}

void CTestFileObservers::ConstructL(const TDesC& aFileName)
	{
	iNotificationSession = CCalSession::NewL();
	iNotificationSession->OpenL(aFileName);
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	CCalChangeNotificationFilter* filter = CCalChangeNotificationFilter::NewL(MCalChangeCallBack2::EChangeEntryAll, ETrue, timeRange);
	CleanupStack::PushL(filter);
	iNotificationSession->StartChangeNotification(*this, *filter);
	CleanupStack::PopAndDestroy(filter);
	}

CTestFileObservers::~CTestFileObservers()
	{
	delete iNotificationSession;
	}

void CTestFileObservers::CalChangeNotification(RArray<TCalChangeEntry>& aChangeItems)
	{
	test.Printf(_L("MCalChangeCallBack2 notification happened\n") );
	test(aChangeItems.Count() == 1);
	TCalChangeEntry change = aChangeItems[0];
	switch(change.iChangeType)
		{
		case MCalChangeCallBack2::EChangeAdd:
			{
			test.Printf(_L("Calendar entry has been added"));
			break;
			}
		case MCalChangeCallBack2::EChangeDelete:
			{
			test.Printf(_L("Calendar entry has been deleted"));
			break;
			}
		case MCalChangeCallBack2::EChangeModify:
			{
			test.Printf(_L("Calendar entry has been modified"));
			break;
			}
		case MCalChangeCallBack2::EChangeUndefined:
			{
			test.Printf(_L("Undefined calendar entry change"));
			break;
			}
		case MCalChangeCallBack2::EChangeTzRules:
			{
			test.Printf(_L("Calendar entry's time zone rules have been changed"));
			break;
			}
		}
	CActiveScheduler::Stop();
	}

CTestMultipleCalendar::CTestMultipleCalendar() : iProgress(0)
	{
	}

CTestMultipleCalendar* CTestMultipleCalendar::NewL()
	{
	CTestMultipleCalendar* self = new (ELeave) CTestMultipleCalendar();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestMultipleCalendar::ConstructL()
	{
	iStartTime.SetTimeLocalL(TDateTime(2007, EMarch, 20, 5, 0, 0, 0)); 
	iEndTime.SetTimeLocalL(TDateTime(2007, EMarch, 20, 6, 0, 0, 0));
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KWorkCalendar);
	iTestLib->OpenFileL(KWorkCalendar);
	}	
	
CTestMultipleCalendar::~CTestMultipleCalendar()	
	{
	TRAP_IGNORE(DeleteSecondCalendarFileL());
	delete iObserverForWorkCalendar;
	delete iObserverForPersonalCalendar;
	delete iTestLib;
	delete iSecondSession;
	}

TCalInstanceId CTestMultipleCalendar::AddAndCheckEntryL(CCalEntryView& aEntryView, CCalSession& aSession, TBool aSameEntry)
	{
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);	
			
	// create an appt
	test.Printf(_L("Create an appointment \n"));
	HBufC8* guid; 
	CCalEntry* entry;
	if(aSameEntry)
		{
		guid = KUid().AllocLC();
		entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
		}
	else
		{
		guid = KUid1().AllocLC();
		entry = iTestLib->CreateCalEntryL(CCalEntry::EEvent, guid);
		}
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);	
	
	test.Printf(_L("Set the start and end time for the entry \n"));
	entry->SetStartAndEndTimeL(iStartTime, iEndTime);		
	
	test.Printf(_L("Set the repeat definition \n"));	
	TCalRRule repeat;
	repeat.SetType(TCalRRule::EDaily);
	repeat.SetDtStart(iStartTime);
	repeat.SetInterval(1);
	repeat.SetCount(10);
	entry->SetRRuleL(repeat);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
		
	// Store the entry
	test.Printf(_L("Store the entry in the session"));
	TInt entriesStored(0);
	aEntryView.StoreL(entries, entriesStored);
	test(entriesStored == 1);
	entries.ResetAndDestroy();
	
	//Fetch entry
	test.Printf(_L("Fetch the entry and verify it \n"));
	
	if(aSameEntry)
		{
		aEntryView.FetchL(KUid, entries);		
		}	
	else
		{
		aEntryView.FetchL(KUid1, entries);
		}
	test(entries.Count()==1);
	entry = entries[0];
	
	test.Printf(_L("Get the instance id \n"));
	TCalInstanceId instanceId;
	instanceId.iEntryLocalId = entry->LocalUidL(); 
	instanceId.iCollectionId = aSession.CollectionIdL();
	instanceId.iInstanceTime = iStartTime;
	
	CleanupStack::PopAndDestroy(&entries);
	return instanceId;
	}

void CTestMultipleCalendar::AddLargeNumberOfEntriesL(CCalEntryView& aEntryView)
	{
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);	
			
	// create an appt
	test.Printf(_L("Create an appointment \n"));
	
	CCalEntry* entry;
	TBuf8<64> uid;
	
	HBufC8* guid; 
	for(TInt k = 0; k < 50; k++)
		{
		uid.Copy(KUid());
		uid.AppendNum(k);
		guid = uid.AllocLC();
		entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
		CleanupStack::Pop(guid);
		CleanupStack::PushL(entry);	
		
		test.Printf(_L("Set the start and end time for the entry \n"));
		entry->SetStartAndEndTimeL(iStartTime, iEndTime);		
		
		test.Printf(_L("Set the repeat definition \n"));	
		TCalRRule repeat;
		repeat.SetType(TCalRRule::EDaily);
		repeat.SetDtStart(iStartTime);
		repeat.SetInterval(1);
		repeat.SetCount(10);
		entry->SetRRuleL(repeat);
		entries.AppendL(entry);
		CleanupStack::Pop(entry);
			
		// Store the entry
		test.Printf(_L("Store the entry in the session"));
		TInt entriesStored(0);
		aEntryView.StoreL(entries, entriesStored);
		test(entriesStored == 1);
		entries.ResetAndDestroy();
		}	
	CleanupStack::PopAndDestroy(&entries);
	}


void CTestMultipleCalendar::Progress(TInt aPercentageCompleted) 
	{
	iProgress++;
	test(aPercentageCompleted >= 0);
	test(aPercentageCompleted <=100);
	test.Printf(_L("MCalProgressCallBack::Progress is %d complete\n"), aPercentageCompleted);
	}

void CTestMultipleCalendar::Completed(TInt aError)
	{
	test(aError == KErrNone);
	test.Printf(_L("MCalProgressCallBack::Completed\n") );
	CActiveScheduler::Stop();
	}

TBool CTestMultipleCalendar::NotifyProgress()
	{
	return ETrue;
	}

void CTestMultipleCalendar::CreateSecondSessionL()
	{
	test.Printf(_L("Create and open the second calendar file \n"));
	iSecondSession = CCalSession::NewL(iTestLib->GetSession());
	iSecondSession->CreateCalFileL(KPersonalCalendar);
	iSecondSession->OpenL(KPersonalCalendar);	
	}

void CTestMultipleCalendar::DeleteSecondCalendarFileL()
	{
	test.Printf(_L("Delete the calendar file"));
	
	if(!iSecondSession)
		{
		iSecondSession = CCalSession::NewL(iTestLib->GetSession());
		}	
	TRAPD(err, iSecondSession->DeleteCalFileL(KPersonalCalendar));
	test(err == KErrNone || err == KErrNotFound);
	}

void CTestMultipleCalendar::ClearStoredDataL()
	{
	// Clear data
	DeleteSecondCalendarFileL();
	delete iSecondSession;
	iSecondSession = NULL;
	iTestLib->ReplaceFileL(KWorkCalendar);
	iTestLib->OpenFileL(KWorkCalendar);
	}

void CTestMultipleCalendar::CreateAndTestInstanceIteratorL(CCalInstanceView& aInstanceView, TInt aExpectedCount)
	{	
	test.Printf(_L("Create the instance iterator to iterate through all the instances \n"));
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	TCalTime start;
	start.SetTimeUtcL(TCalTime::MinTime());
	TCalTime end;
	end.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(start, end);
	aInstanceView.FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
	test(instances.Count() == aExpectedCount);
	instances.ResetAndDestroy();

	// Instance iterator
	CCalFindInstanceSettings* findSetting = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findSetting);
	CCalInstanceIterator* iterator = aInstanceView.FindInstanceL(*findSetting);
	CleanupStack::PopAndDestroy(findSetting);
	CleanupStack::PushL(iterator);
	
	test.Printf(_L("Created iterator, now check the previous and next instances \n"));
	TInt count = CheckIteratorL(*iterator, instances);
	test(count == aExpectedCount);
	
	test.Printf(_L("Finished iterating \n"));
	test.Printf(_L("Deleting the instances \n"));
	CleanupStack::PopAndDestroy(2, &instances); //instances, iterator
	}

TInt CTestMultipleCalendar::CheckIteratorL(CCalInstanceIterator& aIterator, RPointerArray<CCalInstance>& aInstances)
	{
	test.Printf(_L("Iterate through the instances and verify the start and end time"));
	
	TTime previousInstanceTime = Time::NullTTime();
	TInt count = 0;
	while(CCalInstance* nextInstance = aIterator.NextL())
		{
		CleanupStack::PushL(nextInstance);
		
		if(count > 0)
			{
			CCalInstance* preInstance = aIterator.PreviousL();
			CleanupStack::PushL(preInstance);
			test(preInstance!=NULL);
			test(nextInstance->Time().TimeLocalL() >= preInstance->Time().TimeLocalL());
			CleanupStack::PopAndDestroy(preInstance);
			
			CleanupStack::PopAndDestroy(nextInstance);
			nextInstance = aIterator.NextL();
			CleanupStack::PushL(nextInstance);
			}
		
		test(nextInstance->Time().TimeLocalL() >= previousInstanceTime);
		previousInstanceTime = nextInstance->Time().TimeLocalL();
		aInstances.AppendL(nextInstance);
		CleanupStack::Pop(nextInstance);
		++count;
		
		test.Printf(_L("Instance count = %d"), count);
		}
	return count;
	}

void CTestMultipleCalendar::CheckInstanceViewL(CCalInstanceView& aInstanceView, TInt aNumInstance)
	{
	test.Printf(_L("Find the instances and verify the count \n"));
	
	RPointerArray<CCalInstance> calInstances;
	CleanupResetAndDestroyPushL(calInstances);
	
	TCalTime startCalTime;
	startCalTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime endCalTime;
	endCalTime.SetTimeUtcL(TCalTime::MaxTime());
		
	CalCommon::TCalTimeRange timeRange(startCalTime, endCalTime);
	aInstanceView.FindInstanceL(calInstances, CalCommon::EIncludeAll, timeRange);
	test(calInstances.Count() == aNumInstance);
	CleanupStack::PopAndDestroy(&calInstances);
	}

void CTestMultipleCalendar::CheckInstanceDetailsL(CCalInstance* aInstance, TCalInstanceId aInstanceId)
	{
	test.Printf(_L("test the instance time"));
	test(aInstance->Time().TimeLocalL() == iStartTime.TimeLocalL());
	
	test.Printf(_L("test the entry start and end times"));
	test(aInstance->Entry().StartTimeL().TimeLocalL() == iStartTime.TimeLocalL());
	test(aInstance->Entry().EndTimeL().TimeLocalL() == iEndTime.TimeLocalL());
	
	test.Printf(_L("test short file id"));
	test(aInstance->InstanceIdL().iCollectionId == aInstanceId.iCollectionId);	
	}

TBool CTestMultipleCalendar::CheckInstanceIdL(TCalInstanceId aId1, TCalInstanceId aId2)
	{
	TBool aReturn = ETrue;
	if(aId1.iEntryLocalId != aId2.iEntryLocalId)
		{
		aReturn = EFalse;
		}	
	if(aId1.iInstanceTime.TimeMode() != aId2.iInstanceTime.TimeMode())
		{
		aReturn = EFalse;
		}
	if(aId1.iInstanceTime.TimeLocalL() != aId2.iInstanceTime.TimeLocalL())
		{
		aReturn = EFalse;
		}
	if(aId1.iCollectionId != aId2.iCollectionId)
		{
		aReturn = EFalse;
		}
	return aReturn;
	}

/*
@SYMTestCaseID 				PIM-TCAL-MULTIPLE-FILES-0001
@SYMPreq					PREQ2554
@SYMTestCaseDesc			Make sure a session is closed when the last CCalSession referencing it is closed.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Open a CCalSession object
							2.	Open a second CCalSession object, sharing the connection from the first
							3.	Destroy both CCalSession objects					
@SYMTestExpectedResults		There should be no memory leaks and the calendar server process should close 
							after destroying both CCalSession objects.
**/

void CTestMultipleCalendar::TestEntryViewAndFileConsistencyL(TBool aAddingSameEntry)
	{
	iTestLib->ReplaceFileL(KWorkCalendar);
	iTestLib->OpenFileL(KWorkCalendar);
	// Clear the previously stored files
	ClearStoredDataL();
	
	// Create the second session
	CreateSecondSessionL();
	CCalEntryView* secondView =CCalEntryView::NewL(*iSecondSession);
	CleanupStack::PushL(secondView);
	
	test.Printf(_L("Add one entry in each file \n"));
	if(aAddingSameEntry)
		{
		// Add an entry to the first calendar file
		AddAndCheckEntryL(iTestLib->SynCGetEntryViewL(), iTestLib->GetSession());
		// Add an entry to the second calendar file
		AddAndCheckEntryL(*secondView, *iSecondSession);
		}
	else
		{
		// Add an entry to the first calendar file
		AddAndCheckEntryL(iTestLib->SynCGetEntryViewL(), iTestLib->GetSession());
		// Add an entry to the second calendar file
		AddAndCheckEntryL(*secondView, *iSecondSession, EFalse);
		}
	
	CleanupStack::PopAndDestroy(secondView);
	
	test.Printf(_L("Server is shuting down \n") );
	iTestLib->CloseAgendaServer();// Should  shut the server
	delete iSecondSession;
	iSecondSession = NULL;
	
	test.Printf(_L("Open the two files again to check for file consistency \n"));	
	// Open the two files and get the entries - check for file consistency
	iTestLib->OpenFileL(KWorkCalendar);
	iSecondSession = CCalSession::NewL(iTestLib->GetSession());
	iSecondSession->OpenL(KPersonalCalendar);
	secondView =CCalEntryView::NewL(*iSecondSession);
	CleanupStack::PushL(secondView);
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	if(aAddingSameEntry)
		{
		secondView->FetchL(KUid, entries);
		}
	else
		{
		secondView->FetchL(KUid1, entries);
		}	
	test(entries.Count()==1);
	iTestLib->SynCGetEntryViewL().FetchL(KUid, entries);
	test(entries.Count()==2);
	
	// Compare the two entries
	if(aAddingSameEntry)
		{
		test(entries[0]->CompareL(*entries[1]));
		}
	else
		{
		test(!entries[0]->CompareL(*entries[1]));
		}
	
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(secondView);
	
	test.Printf(_L("Server is shuting down \n") );
	iTestLib->CloseAgendaServer(); // Should shut the server
	iSecondSession->DeleteCalFileL(KPersonalCalendar);
	delete iSecondSession;
	iSecondSession = NULL;
	iTestLib->OpenFileL(KWorkCalendar);
	}

/**
@SYMTestCaseID 				PIM-TCAL-MULTIPLE-FILES-0002
@SYMPreq					PREQ2554
@SYMTestCaseDesc			Make sure we can find instances in different calendar collections from one instance view.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Store a single entry in one file
							2.	Store a single entry in a second file
							3.	Open an instance view on both files
							4.	Fetch all instance in both files
							5.	Make sure we get the two expected instance
@SYMTestExpectedResults		We should get the two instances that we were expecting
*/

void CTestMultipleCalendar::TestInstanceViewL()
	{
	// Clear the previously stored files
	ClearStoredDataL();
	
	// Create the second session
	CreateSecondSessionL();
	CCalEntryView* secondView =CCalEntryView::NewL(*iSecondSession);
	CleanupStack::PushL(secondView);
	
	test.Printf(_L("Add one entry in each file \n"));
	// Add an entry to the first calendar file
	AddLargeNumberOfEntriesL(iTestLib->SynCGetEntryViewL());
	// Add an entry to the second calendar file
	AddLargeNumberOfEntriesL(*secondView);
	CleanupStack::PopAndDestroy(secondView);

	test.Printf(_L("Server is shuting down \n") );
	iTestLib->CloseAgendaServer();// Should  shut the server
	delete iSecondSession;
	iSecondSession = NULL;
	
	test.Printf(_L("Open the calendar files \n"));
	iTestLib->OpenFileL(KWorkCalendar);
	iSecondSession = CCalSession::NewL(iTestLib->GetSession());
	iSecondSession->OpenL(KPersonalCalendar);
		
	// Get instances from two files. - instances from different files.
	RPointerArray<CCalSession> sessions;
	CleanupClosePushL(sessions);
	sessions.AppendL(&iTestLib->GetSession());
	sessions.AppendL(iSecondSession);
	
	// Deleting the file so that index is build and progress is tracked
	iTestLib->DeleteFileL(KWorkCalendarIndexFile, EFalse);
	
	CCalInstanceView* instanceView = CCalInstanceView::NewL(sessions, *this);
	CleanupStack::PushL(instanceView);
	CActiveScheduler::Start();
	test(iProgress > 0);
	CreateAndTestInstanceIteratorL(*instanceView, 1000);
		
	// Cleanup
	CleanupStack::PopAndDestroy(instanceView);
	CleanupStack::PopAndDestroy(&sessions);
	iSecondSession->DeleteCalFileL(KPersonalCalendar);
	delete iSecondSession;
	iSecondSession = NULL;
	}

/*
@SYMTestCaseID 				PIM-TCAL-MULTIPLE-FILES-0003
@SYMPreq					Preq2554
@SYMTestCaseDesc			Make sure we can update/delete in different calendar collections.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Store a single entry in one file
							2.	Store a single entry in a second file
							3.	Open entry views and update an entry
							4.	Delete entry and check the entry ids modified
							5.	Make sure we get the expected result
@SYMTestExpectedResults		We should get the correct number of modified entries
**/

void CTestMultipleCalendar::TestUpdateDeleteFunctionalityL()
	{
	test.Next(_L("Test the update/delete functionality "));
	
	// Clear the previously stored files
	ClearStoredDataL();
	
	CreateSecondSessionL();
	CCalEntryView* secondView =CCalEntryView::NewL(*iSecondSession);
	CleanupStack::PushL(secondView);
	
	// Get instances from two different files
	RPointerArray<CCalSession> sessions;
	CleanupClosePushL(sessions);
	sessions.AppendL(&(iTestLib->GetSession()));
	sessions.AppendL(iSecondSession);
	CCalInstanceView* instanceView = CCalInstanceView::NewL(sessions);
	CleanupStack::PushL(instanceView);
	
	test.Printf(_L("Add entries to both the files \n"));
	TCalInstanceId instanceId1 = AddAndCheckEntryL(iTestLib->SynCGetEntryViewL(), iTestLib->GetSession());
	TCalInstanceId instanceId2 = AddAndCheckEntryL(*secondView, *iSecondSession);
	TDateTime startTime(2003, EMarch, 27, 1, 0, 0, 0);
	TCalTime startCalTime;
	startCalTime.SetTimeLocalL(startTime);
	
	test.Printf(_L("Fetch the entry \n"));
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	secondView->FetchL(KUid, entries);
	test(entries.Count()==1);
	iTestLib->SynCGetEntryViewL().FetchL(KUid, entries);
	test(entries.Count()==2);
	
	TDateTime startTimeNew(2004, EMarch, 27, 1, 0, 0, 0);
	startCalTime.SetTimeLocalL(startTimeNew);
	entries[0]->SetStartAndEndTimeL(startCalTime, startCalTime);
	entries[1]->SetStartAndEndTimeL(startCalTime, startCalTime);
	
	test.Printf(_L("Update the entry \n"));
	TInt numEntries;
	
	secondView->UpdateL(entries, numEntries);
	test(numEntries == 2); // Only one entry is updated
	iTestLib->SynCGetEntryViewL().UpdateL(entries, numEntries);
	test(numEntries == 2); // Only one entry is updated
		
	//Delete the entry
	RArray<TCalLocalUid> entriesLocalUids;
	CleanupClosePushL(entriesLocalUids);
	entriesLocalUids.AppendL(instanceId1.iEntryLocalId);
	
	test.Printf(_L("Delete the entries \n"));
	TCalTime calTime;
	TTime time;
	time.HomeTime();
	calTime.SetTimeLocalL(time);
	TInt success;
	iTestLib->SynCGetEntryViewL().DeleteL(entriesLocalUids, success);
	test(success == 1);
	entriesLocalUids.Reset();
	
	iTestLib->SynCGetEntryViewL().GetIdsModifiedSinceDateL(calTime, entriesLocalUids);
	test.Printf(_L("Number of returned entries from GetIdsModifiedSinceDateL = %d\n"), entriesLocalUids.Count());
	test(entriesLocalUids.Count() == 0);
	entriesLocalUids.Reset();
	
	entriesLocalUids.AppendL(instanceId2.iEntryLocalId);
	secondView->DeleteL(entriesLocalUids, success);
	test(success == 1);
	secondView->GetIdsModifiedSinceDateL(calTime, entriesLocalUids);
	test.Printf(_L("Number of returned entries from GetIdsModifiedSinceDateL = %d\n"), entriesLocalUids.Count());
	test(entriesLocalUids.Count() == 1);

	//Test instance view
	CheckInstanceViewL(*instanceView, 0);
		
	CleanupStack::PopAndDestroy(&entriesLocalUids);
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(instanceView);
	CleanupStack::PopAndDestroy(&sessions);
	CleanupStack::PopAndDestroy(secondView);
	iSecondSession->DeleteCalFileL(KPersonalCalendar);
	delete iSecondSession;
	iSecondSession = NULL;
	}

/* 
@SYMTestCaseID				PIM-TCAL-MULTIPLE-FILES-0004
@SYMPreq					Preq2554 
@SYMTestCaseDesc			Make sure we can find instances in different calendar collections using the same instance view.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Store a single entry in one file
							2.	Store a single entry in a second file
							3.	Open an instance view on both files
							4.	Fetch instance from both files
							5.	Make sure we get the two expected instance
				
@SYMTestExpectedResults		We should get the two instances that we were expecting
**/

void CTestMultipleCalendar::TestFindInstanceL(TBool aSessionToFindInstanceFrom, TInt aExpectedValue)
	{
	test.Next(_L("Test finding instances using instance id for each session \n"));
	
	// Clear the previously stored files
	ClearStoredDataL();	
	
	CreateSecondSessionL();
	CCalEntryView* secondView =CCalEntryView::NewL(*iSecondSession);
	CleanupStack::PushL(secondView);
	//Observer for work calendar	
	iObserverForWorkCalendar = CTestFileObservers::NewL(KWorkCalendar);
	test.Printf(_L("Add entries to both the files \n"));
	TCalInstanceId instanceIdForFirstSession = AddAndCheckEntryL(iTestLib->SynCGetEntryViewL(), iTestLib->GetSession());
	CActiveScheduler::Start();
	//Observer for personal calendar
	iObserverForPersonalCalendar = CTestFileObservers::NewL(KPersonalCalendar);
	TCalInstanceId instanceIdForSecondSession = AddAndCheckEntryL(*secondView, *iSecondSession);
	CActiveScheduler::Start();
		
	// Get instances from two different files
	RPointerArray<CCalSession> sessions;
	CleanupClosePushL(sessions);
	sessions.AppendL(&iTestLib->GetSession());
	sessions.AppendL(iSecondSession);
	CCalInstanceView* instanceView = CCalInstanceView::NewL(sessions);
	CleanupStack::PushL(instanceView);
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	TCalTime start;
	start.SetTimeLocalL(TCalTime::MinTime());
	TCalTime end;
	end.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(start, end);
	instanceView->FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
	test(instances.Count()==20);
	instances.ResetAndDestroy();

	test.Printf(_L("Create instance iterator \n"));
	// Instance iterator
	CCalFindInstanceSettings* findSetting = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, timeRange);
	CleanupStack::PushL(findSetting);
	CCalInstanceIterator* iterator;
	if(aSessionToFindInstanceFrom)
		{
		iterator = instanceView->FindInstanceL(*findSetting, instanceIdForSecondSession);
		}
	else
		{
		iterator = instanceView->FindInstanceL(*findSetting, instanceIdForFirstSession);
		}
	CleanupStack::PopAndDestroy(findSetting);
	CleanupStack::PushL(iterator);
	
	test.Printf(_L("Created iterator \n"));
	TInt count = CheckIteratorL(*iterator, instances);
	test(count == aExpectedValue);
	
	test.Printf(_L("Finished iterating \n"));
	test.Printf(_L("Deleting the instances \n"));
	instances.ResetAndDestroy();
	CleanupStack::PopAndDestroy(2, &instances);//instances, iterator
	CleanupStack::PopAndDestroy(instanceView);
	CleanupStack::PopAndDestroy(&sessions);
	CleanupStack::PopAndDestroy(secondView);
	delete iObserverForWorkCalendar;
	iObserverForWorkCalendar = NULL;
	delete iObserverForPersonalCalendar;
	iObserverForPersonalCalendar = NULL;
	iSecondSession->DeleteCalFileL(KPersonalCalendar);
	delete iSecondSession;
	iSecondSession = NULL;
	}

/* 
@SYMTestCaseID				PIM-TCAL-MULTIPLE-FILES-0005
@SYMPreq					Preq2554 
@SYMTestCaseDesc			Make sure we can find instances in different calendar collections using TCalInstanceId.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Store a single entry in one file
							2.	Store a single entry in a second file
							3.	Open an instance view on both files
							4.	Fetch instance from both files using their respective TCalInstanceId
							5.	Make sure we get the two expected instance
				
@SYMTestExpectedResults		We should get the two instances that we were expecting
**/

void CTestMultipleCalendar::TestFindInstanceWithInstanceIdL()
	{
	test.Next(_L("Test find instance with instance id \n"));
	
	// Clear the previously stored files
	ClearStoredDataL();
	
	CreateSecondSessionL();
	CCalEntryView* secondView =CCalEntryView::NewL(*iSecondSession);
	CleanupStack::PushL(secondView);
	
	test.Printf(_L("Add two entries to the calendar files \n"));
	TCalInstanceId instanceIdForFirstSession = AddAndCheckEntryL(iTestLib->SynCGetEntryViewL(), iTestLib->GetSession());
	TCalInstanceId instanceIdForSecondSession = AddAndCheckEntryL(*secondView, *iSecondSession);
			
	RPointerArray<CCalSession> sessions;
	CleanupClosePushL(sessions);
	sessions.AppendL(&iTestLib->GetSession());
	sessions.AppendL(iSecondSession);	
	
	CCalInstanceView* instanceView = CCalInstanceView::NewL(sessions);
	CleanupStack::PushL(instanceView);
	
	CCalInstance* instance = instanceView->FindInstanceL(instanceIdForFirstSession);
	CleanupStack::PushL(instance);
	CheckInstanceDetailsL(instance, instanceIdForFirstSession);
	CleanupStack::PopAndDestroy(instance); 
	instance = instanceView->FindInstanceL(instanceIdForSecondSession);
	CleanupStack::PushL(instance);
	CheckInstanceDetailsL(instance, instanceIdForSecondSession);
	
	CleanupStack::PopAndDestroy(instance);
	CleanupStack::PopAndDestroy(instanceView);
	CleanupStack::PopAndDestroy(&sessions);
	CleanupStack::PopAndDestroy(secondView);
	iSecondSession->DeleteCalFileL(KPersonalCalendar);
	delete iSecondSession;
	iSecondSession = NULL;
	}

/* 
@SYMTestCaseID				PIM-TCAL-MULTIPLE-FILES-0006
@SYMPreq					Preq2554 
@SYMTestCaseDesc			Make sure we can find TCalCollectionId and TCalInstanceId for the files stored.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Create second session from first one
							2.	Get the TCalCollectionId for both session
							3.	Store a single entry in one file
							4.	Store a single entry in a second file
							5.	Get TCalInstanceId for both files
							6.	Make sure we get the expected results
				
@SYMTestExpectedResults		We should get the two TCalCollectionId and TCalInstanceId that we were expecting
**/

void CTestMultipleCalendar::TestShortFileIdL()
	{
	// Clear the previously stored files
	ClearStoredDataL();
	
	CreateSecondSessionL();
	CCalEntryView* secondView =CCalEntryView::NewL(*iSecondSession);
	CleanupStack::PushL(secondView);
		
	TCalCollectionId firstSessionFileId = iTestLib->GetSession().CollectionIdL();
	test(firstSessionFileId == KShortFileId1);
	TCalCollectionId secondSessionFileId = iSecondSession->CollectionIdL();
	test(secondSessionFileId == KShortFileId2);
	
	TCalInstanceId instanceIdForFirstSession = AddAndCheckEntryL(iTestLib->SynCGetEntryViewL(), iTestLib->GetSession());
	TCalInstanceId instanceIdForSecondSession = AddAndCheckEntryL(*secondView, *iSecondSession);

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	secondView->FetchL(KUid, entries);
	test(entries.Count()==1);
	iTestLib->SynCGetEntryViewL().FetchL(KUid, entries);
	test(entries.Count()==2);
	//Compare two entries - they should be the same
	test(entries[0]->CompareL(*entries[1]));
	CleanupStack::PopAndDestroy(&entries);
	
	RPointerArray<CCalSession> sessions;
	CleanupClosePushL(sessions);
	CCalSession& firstsession = iTestLib->GetSession();
	sessions.AppendL(&firstsession);
	sessions.AppendL(iSecondSession);
	CCalInstanceView* instanceView = CCalInstanceView::NewL(sessions);
	CleanupStack::PushL(instanceView);
		
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	TCalTime start;
	start.SetTimeUtcL(TCalTime::MinTime());
	TCalTime end;
	end.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(start, end);
	instanceView->FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
	TInt count = instances.Count();
	test(count == 20);
	TCalInstanceId calId1 = instances[0]->InstanceIdL();		
	test(CheckInstanceIdL(calId1, instanceIdForFirstSession));
	TCalInstanceId calId2 = instances[1]->InstanceIdL();
	test(CheckInstanceIdL(calId2, instanceIdForSecondSession));
	CleanupStack::PopAndDestroy(&instances);
	CleanupStack::PopAndDestroy(instanceView);
	CleanupStack::PopAndDestroy(&sessions);
	CleanupStack::PopAndDestroy(secondView);
	iSecondSession->DeleteCalFileL(KPersonalCalendar);
	delete iSecondSession;
	iSecondSession = NULL;
	}

/* 
@SYMTestCaseID				PIM-TCAL-MULTIPLE-FILES-0007
@SYMPreq					Preq2554 
@SYMTestCaseDesc			Make sure we can switch instance views
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Create and open a file with each session and get the collection id
							2.	Store a single entry in one file
							3.	Store a single entry in a second file
							4.	Open an instance view on both files
							5.	Fetch instance from both files
							6.	Destroy the first session
							7. 	Create another session, create and open a calendar file
							8. 	Fetch instances from second and third file 
							9. 	Make sure we get the expected results
				
@SYMTestExpectedResults		We should get the instances that we were expecting
**/

void CTestMultipleCalendar::TestSwitchInstanceViewsL()
	{
	// Clear the previously stored files
	ClearStoredDataL();
	
	CCalSession& session1 = iTestLib->GetSession();
	TCalCollectionId shortFileId1 = session1.CollectionIdL();
	test(shortFileId1 != 0);
	CCalSession* session2 = CCalSession::NewL(session1); 
	CleanupStack::PushL(session2);
	session2->CreateCalFileL(KPersonalCalendar);
	session2->OpenL(KPersonalCalendar);
	TCalCollectionId shortFileId2 = session2->CollectionIdL();
	test(shortFileId2 != 0);
	
	CCalEntryView* secondView = CCalEntryView::NewL(*session2);
	CleanupStack::PushL(secondView);
	
	TCalInstanceId instanceId1 = AddAndCheckEntryL(iTestLib->SynCGetEntryViewL(), iTestLib->GetSession());
	TCalInstanceId instanceId2 = AddAndCheckEntryL(*secondView, *session2);
	
	RPointerArray<CCalSession> calSessions;
	CleanupClosePushL(calSessions);	
	calSessions.AppendL(&session1);
	calSessions.AppendL(session2);
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	CCalInstanceView* instanceView = CCalInstanceView::NewL(calSessions);
	CleanupStack::PushL(instanceView);
		
	TCalTime start;
	start.SetTimeUtcL(TCalTime::MinTime());
	TCalTime end;
	end.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(start, end);
	instanceView->FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
	test(instances.Count() == 20);
	
	CleanupStack::PopAndDestroy(instanceView);
	instances.ResetAndDestroy();
	calSessions.Reset();
	delete iTestLib;
	iTestLib = NULL;
	
	CCalSession* session3 = CCalSession::NewL(*session2);
	CleanupStack::PushL(session3);
	session3->CreateCalFileL(KSportsCalendar);
	session3->OpenL(KSportsCalendar);

	calSessions.AppendL(session2);
	calSessions.AppendL(session3);

	instanceView = CCalInstanceView::NewL(calSessions);
	CleanupStack::PushL(instanceView);

	instanceView->FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);//It will only return instances from file2 and file3
	test(instances.Count() == 10);
		
	CleanupStack::PopAndDestroy(instanceView);
	session3->DeleteCalFileL(KSportsCalendar);
	CleanupStack::PopAndDestroy(session3);	
	CleanupStack::PopAndDestroy(&instances);
	CleanupStack::PopAndDestroy(&calSessions);
	CleanupStack::PopAndDestroy(secondView);
	session2->DeleteCalFileL(KPersonalCalendar);
	CleanupStack::PopAndDestroy(session2);
	
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KWorkCalendar);
	iTestLib->OpenFileL(KWorkCalendar);
	}

/* 
@SYMTestCaseID				PIM-TCAL-MULTIPLE-FILES-0008
@SYMPreq					Preq2554 
@SYMTestCaseDesc			Make sure we cannot open same file by two sessions. 
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Create two sessions
							2.	Open KWorkCalendar with the first session
							3.	Open KWorkCalendar with the second session
							4.	Test the return code for OpenL()
											
@SYMTestExpectedResults		We should get KErrArgument while opening KWorkCalendar using second session
**/

void CTestMultipleCalendar::TestNegativeForSecondSessionL()
	{
	test.Printf(_L("Test that the second session cannot open the file already opened by the first session"));
	
	// Clear the previously stored files
	ClearStoredDataL();
	delete iSecondSession;
	iSecondSession = NULL;
	
	iSecondSession = CCalSession::NewL(iTestLib->GetSession());
	// KCalendarFilename1 refers to the file opened by the first session
	TRAPD(err, iSecondSession->OpenL(KWorkCalendar));
	// Test that the error returned is KErrArgument
	test.Printf(_L("Test whether opening the file results in KErrArgument"));
	test(err == KErrArgument);
	
	delete iSecondSession;
	iSecondSession = NULL;
	}

/* 
@SYMTestCaseID				PIM-TCAL-MULTIPLE-FILES-0009
@SYMPreq					Preq2554 
@SYMTestCaseDesc			Make sure TCalCollectionId overflows beyond KMaxTUint8.
@SYMTestPriority            1
@SYMTestType                CIT
@SYMTestStatus 				Implemented
@SYMTestActions 			1.	Ensure there are two sessions with the server
							2.	For a session, create and open calendar files
							3.	Check the error code while opening the file
							
@SYMTestExpectedResults		We should get KErrOverflow after KMaxTUint8 files have been added
**/

void CTestMultipleCalendar::TestNegativeForCollectionIdL()
	{
	// Clear the previously stored files
	ClearStoredDataL();
	
	test.Printf(_L("Test whether short file id overflows beyond KMaxTUint8"));
	TFileName* fileName;
	iSecondSession = CCalSession::NewL(iTestLib->GetSession());
			
	TInt err(0);
	TInt j=0;
	while(err == KErrNone)
		{		
		fileName = new(ELeave) TFileName;
		CleanupStack::PushL(fileName);
		fileName->Format(KPersonalCalendar);
		fileName->AppendNum(j+1);
		iSecondSession->CreateCalFileL(*fileName);
		TRAP(err, iSecondSession->OpenL(*fileName));
		fileName->Format(KPersonalCalendar);
		fileName->AppendNum(j);
		if(j != 0)
			{
			iSecondSession->DeleteCalFileL(*fileName);
			}		
		CleanupStack::PopAndDestroy(fileName);
		++j;
		}
	
	if(err != KErrNone)
		{
		test(err == KErrOverflow);
		}
	
	fileName = new(ELeave) TFileName;
	CleanupStack::PushL(fileName);
	fileName->Format(KPersonalCalendar);
	fileName->AppendNum(KMaxTUint8);
	iSecondSession->DeleteCalFileL(*fileName);
	CleanupStack::PopAndDestroy(fileName);		
	}

/* Test the defect fix DEF141396 "Attachments are not shown in the attachment list view after calendar name is c..."
 Test action:
 1. Create Calendar file "Work_Calendar"
 2. Add and entry with file attachment 
 3. Create another Calenda file "Personal_Calender"
 3. Fetch the attachment from "Work_Calendar" and Copied the entry
 4. Store the copied entry to "Personal_Calendar"
 5. Delete the Calendar file "Work_Calendar"
 6. Fetched the copied entry from "Personal_Calendar"
 7. Test that the attachment in copied entry has the expected attachment.
*/
void CTestMultipleCalendar::TestCopyEntryBetweenCalendarsL()
    {
    TInt success;
    iTestLib->ReplaceFileL(KWorkCalendar);
    iTestLib->OpenFileL(KWorkCalendar);
    // Clear the previously stored files
    ClearStoredDataL();
     // Create the second session
    CreateSecondSessionL();
    CCalEntryView* personalView =CCalEntryView::NewL(*iSecondSession);
    CleanupStack::PushL(personalView);
    test.Printf(_L("Add one entry in each file \n"));
    _LIT8(KBinaryData1, "zzzzzzzzzzzzzzzzzz");
    test.Printf(_L("Store an entry with a binary data attachment\n"));
    // set up attachment properties
    HBufC8* data = KBinaryData1().AllocLC();
    CCalAttachment* attachment = CCalAttachment::NewFileL(data);
    CleanupStack::Pop(data);

    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);   
            
    // create an appt
    test.Printf(_L("Create an appointment \n"));
    HBufC8* guid; 
    CCalEntry* entry;
    guid = KUid().AllocLC();
    entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
    CleanupStack::Pop(guid);
    CleanupStack::PushL(entry); 
    
    test.Printf(_L("Set the start and end time for the entry \n"));
    entry->SetStartAndEndTimeL(iStartTime, iEndTime);       
    entry->AddAttachmentL(*attachment);
    test.Printf(_L("Set the repeat definition \n"));    
    entries.AppendL(entry);
    CleanupStack::Pop(entry);
    // Store the entry
    test.Printf(_L("Store the entry in the session"));
    TInt entriesStored(0);
    iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
    test(entriesStored == 1);
    entries.ResetAndDestroy();
    
    //Fetch entry
    test.Printf(_L("Fetch the entry and verify it \n"));
    
    iTestLib->SynCGetEntryViewL().FetchL(KUid, entries);       
    test(entries.Count()==1);
    CCalAttachment* originalAttachment = entries[0]->AttachmentL(0);
    test(originalAttachment->FileAttachment() != NULL);
    _LIT8(KUid2,"AttachTest2");
    HBufC8* guid2 = KUid2().AllocLC();
    entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid2);
    CleanupStack::Pop(guid2);
    CleanupStack::PushL(entry);
    entry->CopyFromL(*entries[0], CCalEntry::EDontCopyId);
    entries.ResetAndDestroy();
    entries.AppendL(entry);
    CleanupStack::Pop(entry);
    personalView->StoreL(entries, success);
    
    entries.ResetAndDestroy();   
    iTestLib->DeleteFileL(KWorkCalendar,ETrue);
    personalView->FetchL(KUid2, entries);
    TInt count = entries.Count();
    entries[0]->AttachmentL(0)->FileAttachment()->LoadBinaryDataL(); 
    test(entries[0]->AttachmentL(0)->Value().Length() == KBinaryData1().Length());
    CleanupStack::PopAndDestroy(&entries);
    
    CCalSession& session = iTestLib->GetSession();
    CCalAttachmentManager* attachmentManager = CCalAttachmentManager::NewL(*iSecondSession, *iTestLib);
    CleanupStack::PushL(attachmentManager);
    CActiveScheduler::Start();
    CCalAttachmentIterator* attachments = attachmentManager->FetchAttachmentsL(CCalAttachmentManager::ESortBySizeLargestFirst);
    CleanupStack::PushL(attachments);
    test(attachments->Count() == 1);
    CCalAttachment* att = attachments->NextL();
    test(att != NULL);
    CleanupStack::PushL(att);
    att->FileAttachment()->LoadBinaryDataL(); 
    test(att->Value().Length() == KBinaryData1().Length());

    CleanupStack::PopAndDestroy(att);
    CleanupStack::PopAndDestroy(attachments);
    CleanupStack::PopAndDestroy(attachmentManager);
    CleanupStack::PopAndDestroy(personalView);
    
    iSecondSession->DeleteCalFileL(KPersonalCalendar);
    delete iSecondSession;
    iSecondSession = NULL;
    iTestLib->ReplaceFileL(KWorkCalendar);
    iTestLib->OpenFileL(KWorkCalendar);
    }

void CTestMultipleCalendar::RunAllTestsL()
	{
	// Test performing basic operations
	test.Next(_L("Test creating instance view and fetch instances"));
	TestInstanceViewL();
	
	test.Next(_L("Test creating entry view and basic sync/async operations"));
	TestEntryViewAndFileConsistencyL(ETrue);
	TestEntryViewAndFileConsistencyL(EFalse);
	
	test.Next(_L("Test update/delete functionality"));
	TestUpdateDeleteFunctionalityL();
	
	// New API for FindInstanceL() test
	test.Next(_L("Test find instances using first session instance id"));
	TestFindInstanceL(ETrue, 18);
	test.Next(_L("Test find instances using second session instance id"));
	TestFindInstanceL(EFalse, 19);
	test.Next(_L("Test find instance with TCalInstanceId"));
	TestFindInstanceWithInstanceIdL();
	
	// Short file ID and instance ID test
	test.Next(_L("Test short file id"));
	TestShortFileIdL();
	
	// Testing switching instance views
	test.Next(_L("Test switching instance views"));
	TestSwitchInstanceViewsL(); 
	
    // Testing copying entry with file attachment between different Calendar files
    test.Next(_L("Test copying entry with attachment file"));
	TestCopyEntryBetweenCalendarsL();

	// Negative tests
	test.Next(_L("Negative test for second session"));
	TestNegativeForSecondSessionL();
	test.Next(_L("Negative test for collection id"));
	TestNegativeForCollectionIdL();
	}

LOCAL_C void doMainL()
	{
	CTestMultipleCalendar* testManager = CTestMultipleCalendar::NewL();
	TRAPD(ret, testManager->RunAllTestsL());
	test(ret == KErrNone);
	delete testManager;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(KTestName);
	test.Title();
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	TRAPD(ret, doMainL());
	RDebug::Print(_L("DoMainL returned: %d"), ret);
	delete scheduler;
	test(ret == KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

