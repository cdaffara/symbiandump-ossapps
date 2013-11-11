// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../caltestlib.h"

#include <calentryview.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <e32test.h>
#include <caluser.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceiterator.h>

_LIT(KTestName, "tcal_instanceiterator_performance");
_LIT(KPersonalCalendar, "Personal_calendar");

const TTime KStartingInstanceTimeLocal(TTime(TDateTime(1996, ENovember, 12, 11, 30, 0, 0)));
const TInt KNumOfEntriesLarge(100);
const TInt KNumOfEntriesSmall(9);
const TInt KNumOfInstancesPerEntry(100);
const TInt KFirstFileId = 1;
const TInt KSecondFileId = 2;

RTest test(KTestName);
		
class CInstanceIteratorTestManager : public CBase
	{
private:
	enum TIteratorType
		{
		EIteratorAll,
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
		EIteratorSorted,
		EIteratorPriorityFilter,
#endif
		EIteratorTypeFilter
		};
	
public:
    static CInstanceIteratorTestManager* NewLC();
	~CInstanceIteratorTestManager();
		
	void RunTestsL();
	
private:
	void ConstructL();
	
	// Utility methods
	void PrintHeapUsage();
	void SetupDBL(TInt aNumEntries);
	void SetupDBForMultipleFilesL(TInt aNumEntries);
	
	// Creating entries
	CCalEntry* CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount);
	CCalEntry* CreateNonRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime);
	
	// performance test
	void TestIteratorPerformanceL(TIteratorType aIteratorType);
		
private:
	CCalTestLibrary* iTestLibrary;
	
	TCalLocalUid iLocalUid;
	TCalLocalUid iLocalUidSecondSession;
	CCalSession* iSecondSession;
	CCalEntryView* iSecondEntryView;
	CCalInstanceView* iInstanceView;
	RPointerArray<CCalSession> iSessions;
	TBool iMultipleFilesTest;
	TCalInstanceId iId1;
	TCalInstanceId iId2;
	};

CInstanceIteratorTestManager* CInstanceIteratorTestManager::NewLC()
	{
	CInstanceIteratorTestManager* self = new (ELeave) CInstanceIteratorTestManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	return (self);
	}

void CInstanceIteratorTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	iTestLibrary->ReplaceFileL(KTestName);
	iTestLibrary->OpenFileL(KTestName);	
	
	iSecondSession = CCalSession::NewL(iTestLibrary->GetSession());
	iSecondSession->CreateCalFileL(KPersonalCalendar);
	iSecondSession->OpenL(KPersonalCalendar);
	
	iSecondEntryView = CCalEntryView::NewL(*iSecondSession);
	iSessions.AppendL(&iTestLibrary->GetSession());
	iSessions.AppendL(iSecondSession);
	
	iInstanceView = CCalInstanceView::NewL(iSessions);	
	}	


CInstanceIteratorTestManager::~CInstanceIteratorTestManager()	
	{
	iSessions.Close();
	delete iInstanceView;
	delete iTestLibrary;
	delete iSecondEntryView;
	TRAP_IGNORE(iSecondSession->DeleteCalFileL(KPersonalCalendar));
	delete iSecondSession;
	}

void CInstanceIteratorTestManager::PrintHeapUsage()
	{
	TInt clientHeapSize(0);
	TInt serverHeapSize(0);
	
	TInt clientCellCount = User::AllocSize(clientHeapSize);
	TInt err(KErrNone);
	TRAP(err, serverHeapSize = iTestLibrary->GetSession()._DebugRequestAllocatedHeapSizeL());
	
	test.Printf(_L("Client Heap = %d KB : Server Heap = %d"), clientHeapSize / 1024, serverHeapSize / 1024);
	}

CCalEntry* CInstanceIteratorTestManager::CreateRepeatingEntryLC(CCalEntry::TType aEntryType, TTime aLocalStartTime, TInt aRepeatCount)
	{
	TCalTime entryStart;
	entryStart.SetTimeLocalL(aLocalStartTime);
	TCalTime entryEnd;
	entryEnd.SetTimeLocalL(aLocalStartTime + TTimeIntervalHours(1));
	
	HBufC8* guid = NULL;
	CCalEntry* entry = iTestLibrary->CreateCalEntryL(aEntryType, guid);
	CleanupStack::PushL(entry);
	
	entry->SetStartAndEndTimeL(entryStart, entryEnd);
	
	// Add the RRule
	TCalRRule rRule(TCalRRule::EDaily);
	rRule.SetDtStart(entryStart);
	rRule.SetCount(aRepeatCount);
	rRule.SetInterval(1);
	entry->SetRRuleL(rRule);
	
	return entry;
	}

void CInstanceIteratorTestManager::SetupDBL(TInt aNumEntries)
	{
	iTestLibrary->CleanDatabaseL();
	
	test.Printf(_L("Start storing the entries"));
	for (TInt i(0) ; i < aNumEntries ; ++i)
		{
		CCalEntry::TType entryType = (i % 2) ? CCalEntry::EEvent : CCalEntry::EAppt;
		TUint priority = (i % 2);
		
		CCalEntry* entry = CreateRepeatingEntryLC(entryType, KStartingInstanceTimeLocal + TTimeIntervalMinutes(i), KNumOfInstancesPerEntry);
		entry->SetPriorityL(priority);
		iTestLibrary->StoreEntryL(*entry);
		
		if (i == 0)
			{
			iLocalUid = entry->LocalUidL();			
			}
		
		CleanupStack::PopAndDestroy(entry);
		
		test.Printf(_L("."));
		}
	}

void CInstanceIteratorTestManager::SetupDBForMultipleFilesL(TInt aNumEntries)
	{
	iTestLibrary->CleanDatabaseL();
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	test.Printf(_L("Start storing the entries in first file"));
	TCalTime startTime;
	TInt entriesToAdd = aNumEntries/2;
	CCalEntry* entry = NULL;
	for (TInt i(0) ; i < entriesToAdd ; ++i)
		{
		CCalEntry::TType entryType = (i % 2) ? CCalEntry::EEvent : CCalEntry::EAppt;
		TUint priority = (i % 2);
		
		entry = CreateRepeatingEntryLC(entryType, KStartingInstanceTimeLocal + TTimeIntervalMinutes(i), KNumOfInstancesPerEntry);
		entry->SetPriorityL(priority);
		iTestLibrary->StoreEntryL(*entry);
		
		if (i == 0)
			{
			iLocalUid = entry->LocalUidL();		
			startTime = entry->StartTimeL();
			}
		
		CleanupStack::PopAndDestroy(entry);
		
		test.Printf(_L("."));
		}
	
	iId1.iEntryLocalId = iLocalUid;
	iId1.iInstanceTime = startTime;
	iId1.iCollectionId = KFirstFileId; 
	
	test.Printf(_L("Start storing the entries in second file"));
	for (TInt j(0) ; j < entriesToAdd ; ++j)
		{
		CCalEntry::TType entryType = (j % 2) ? CCalEntry::EEvent : CCalEntry::EAppt;
		TUint priority = (j % 2);
		
		entry = CreateRepeatingEntryLC(entryType, KStartingInstanceTimeLocal + TTimeIntervalMinutes(j), KNumOfInstancesPerEntry);
		entry->SetPriorityL(priority);
		entries.AppendL(entry);
		CleanupStack::Pop(entry);
		TInt numEntries(0);
		iSecondEntryView->StoreL(entries, numEntries);
		test.Printf(_L("."));
		if (j == 0)
			{
			iLocalUidSecondSession = entries[0]->LocalUidL();	
			startTime = entries[0]->StartTimeL();
			}
		}
	iId2.iEntryLocalId = iLocalUidSecondSession;
	iId2.iInstanceTime = startTime;
	iId2.iCollectionId = KSecondFileId; 
		
	CleanupStack::PopAndDestroy(&entries);
	}

void CInstanceIteratorTestManager::TestIteratorPerformanceL(TIteratorType aIteratorType)
	{
	switch (aIteratorType)
		{
		case EIteratorAll: test.Next(_L("Iterator performance test (All)")); break;
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
		case EIteratorSorted: test.Next(_L("Iterator performance test (sorted)")); break;
		case EIteratorPriorityFilter: test.Next(_L("Iterator performance test (priority filter)")); break;	
#endif
		case EIteratorTypeFilter: test.Next(_L("Iterator performance test (type filter)")); break;
		};
	
	TPerformanceTimer createTimer(test);
	TPerformanceTimer iteratingTimer(test);
	TPerformanceTimer countTimer(test);
	
	PrintHeapUsage();
	
	// Create the find instance settings
	TCalTime startRange;
	TCalTime endRange;
	startRange.SetTimeLocalL(TCalTime::MinTime());
	endRange.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange searchTimeRange(startRange, endRange);
	CCalFindInstanceSettings* findInstanceSettings = CCalFindInstanceSettings::NewL(CalCommon::EIncludeAll, searchTimeRange);
	CleanupStack::PushL(findInstanceSettings);
	
	test.Printf(_L("create the iterator"));
	
	TCalTime startingInstanceTime;
	startingInstanceTime.SetTimeLocalL(KStartingInstanceTimeLocal);
	
	createTimer.Start();
	
	CCalInstanceIterator* instanceIterator(NULL);
	
	switch (aIteratorType)
		{
		case EIteratorAll:
			if(iMultipleFilesTest)
				{
				instanceIterator = iInstanceView->FindInstanceL(*findInstanceSettings, iId1);
				}
			else
				{
				instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, iLocalUid, startingInstanceTime);
				}
			break;
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
		case EIteratorSorted:
			findInstanceSettings->SortCriteria().AppendCriterionL(CalCommon::ESortAttrStartTime, CalCommon::EAscendingOrder);
			if(iMultipleFilesTest)
				{
				instanceIterator = iInstanceView->FindInstanceL(*findInstanceSettings, iId1);
				}
			else
				{
				instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, iLocalUid, startingInstanceTime);
				}			
			break;
		case EIteratorPriorityFilter:
			// only find instances with a priority of 0
			findInstanceSettings->SetPriorityRange(CalCommon::TCalPriorityRange(0, 0));
			if(iMultipleFilesTest)
				{
				instanceIterator = iInstanceView->FindInstanceL(*findInstanceSettings, iId1);
				}
			else
				{
				instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, iLocalUid, startingInstanceTime);
				}			
			break;
#endif
		case EIteratorTypeFilter:
			// find everything except appointments
			findInstanceSettings->SetFilter(CalCommon::EIncludeAppts);
			if(iMultipleFilesTest)
				{
				instanceIterator = iInstanceView->FindInstanceL(*findInstanceSettings, iId1);
				}
			else
				{
				instanceIterator = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(*findInstanceSettings, iLocalUid, startingInstanceTime);
				}			
			break;
		default:
			test.Printf(_L("An invalid iterator test was selected"));
			test(EFalse);
			break;
		};
	
	createTimer.Stop();
	test.Printf(_L("iterator created"));
	PrintHeapUsage();
	
	CleanupStack::PopAndDestroy(findInstanceSettings);
	CleanupStack::PushL(instanceIterator);
	
	countTimer.Start();
	TInt foundInstanceCount = instanceIterator->Count();
	countTimer.Stop();
	TInt foundInstancesForwards(0);
	
	test.Printf(_L("iterating forwards"));
	
	iteratingTimer.Start();
	
	CCalInstance* instance = instanceIterator->NextL();
	
	while (instance)
		{
		delete instance;
		instance = instanceIterator->NextL();
		foundInstancesForwards++;
		}
	
	iteratingTimer.Stop();
	
	test.Printf(_L("finished iterating"));
	
	test.Printf(_L("found %d instances"), foundInstancesForwards);

	PrintHeapUsage();
	
	CleanupStack::PopAndDestroy(instanceIterator);
	
	test.Printf(_L("destroyed iterator"));
	PrintHeapUsage();
	
	test.Printf(_L("Time to create iterator = %d\tTime to count instances = %d\tTime to iterate = %d\t"), createTimer.ElapsedTime().Int(), countTimer.ElapsedTime().Int(), iteratingTimer.ElapsedTime().Int());
	}

void CInstanceIteratorTestManager::RunTestsL()
	{
	// Performance testing
	test.Printf(_L("Do performance testing with 900 instances"));
	SetupDBL(KNumOfEntriesSmall);
	TestIteratorPerformanceL(EIteratorAll);
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	TestIteratorPerformanceL(EIteratorSorted);
#endif
	TestIteratorPerformanceL(EIteratorTypeFilter);
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	TestIteratorPerformanceL(EIteratorPriorityFilter);
#endif
	
	test.Printf(_L("Do performance testing with 10,000 instances"));
	SetupDBL(KNumOfEntriesLarge);
	TestIteratorPerformanceL(EIteratorAll);
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	TestIteratorPerformanceL(EIteratorSorted);
#endif
	TestIteratorPerformanceL(EIteratorTypeFilter);
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	TestIteratorPerformanceL(EIteratorPriorityFilter);
#endif
	
	test.Printf(_L("Do performance testing with 10,000 instances and two calendar files"));
	SetupDBForMultipleFilesL(KNumOfEntriesLarge);
	iMultipleFilesTest = ETrue;
	TestIteratorPerformanceL(EIteratorAll);
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	TestIteratorPerformanceL(EIteratorSorted);
#endif
	TestIteratorPerformanceL(EIteratorTypeFilter);
#ifdef SYMBIAN_CALENDAR_ENHANCEDSEARCHANDSORT
	TestIteratorPerformanceL(EIteratorPriorityFilter);
#endif
	}
	
static void DoTestL()
	{
	CInstanceIteratorTestManager* testManager = CInstanceIteratorTestManager::NewLC();
	testManager->RunTestsL();
	CleanupStack::PopAndDestroy(testManager);
	}

/**
@SYMTestCaseID     PIM-TCAL-INSTANCEITERATOR-PERFORMANCE-0001
*/

TInt E32Main()
    {
	__UHEAP_MARK;
	
	test.Title();
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-INSTANCEITERATOR-PERFORMANCE-0001 tcal_instanceiterator_performance"));

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	if (!scheduler)
		{
		test(EFalse);
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);
	
	TInt ret(KErrNone);
	TRAP(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return ret;
    }
