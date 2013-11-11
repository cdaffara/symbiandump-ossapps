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

#include "../caltestlib.h"
#include "../caltestoom.h"
#include <calentry.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <e32test.h>
#include <caluser.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceiterator.h>


// The objective of this test suite is to test
// the CCalInstanceIterator interface

_LIT(KTestName,"tcal_instanceIterator");
RTest test(KTestName);

_LIT(KRepeatingEntryDes, "Repeating Entry Summary");
_LIT(KNonRepeatingEntryDes, "Single Entry Summary");
_LIT(KChildEntryDes, "Child Entry Summary");

_LIT(KFormatDateTime, "%D%M%Y%/0%1%/1%2%/2%3%/3 %H:%T:%S.%C");

const TInt KNumberOfEntries = 200;
const TInt KNumberOfEntriesInOom = 10;
const TInt KNumberOfEntriesStoreAtOnce = 30;
const TInt KRepeatCount = 10;
const TInt KRepeatingEntry = 3;

class CInstanceIteratorTestManager : public CBase, public MCalTestOomCallBack
	{
public:

    static CInstanceIteratorTestManager* NewLC();
	~CInstanceIteratorTestManager();
	
	void AddEntryL(const TInt aNumToAdd);	
	void TestIteratingInstanceL(const TInt aNumInstance, const TDesC& aSearchString);
	
	TInt RepeatingInstanceCount() const;
	TInt SingleInstanceCount() const;
	TInt ChildInstanceCount() const;
	void NegtiveTestL() const;
	void PerformanceComparisonL();
	void OomTestL();

private: // from MCalTestOomCallBack
	void OomTestL(TType aType, TInt aFailAt);

private:

	// avoid any method but NewL instancing the class
	CInstanceIteratorTestManager() {};
	void ConstructL();
	// no copy constructor and assignment operator
	CInstanceIteratorTestManager(CInstanceIteratorTestManager& );
	CInstanceIteratorTestManager& operator = (const CInstanceIteratorTestManager& );
	CCalInstanceIterator* CreateIteratorLC(const TDesC& aSearchString, CalCommon::TCalTimeRange aTimeRange) const;
	
	void PrintRamUsage();
	
private:

	CCalTestLibrary* 	iTestLibrary;
	CCalInstanceView* 	iInstanceView;	//Not owned
	CCalEntryView* 		iEntryView;		//Not owned
	TInt 				iNumRepeatingInstance; //Those instances have summary as "Repeating Entry Summary"
	TInt 				iNumSingleInstance; 	//Those instances have summary as "Single Entry Summary
	TInt 				iNumChildInstance;
	
	HBufC*				iOomSearchString;
	};


// Constructor/Destructor

CInstanceIteratorTestManager* CInstanceIteratorTestManager::NewLC()
	{
	CInstanceIteratorTestManager* self = new (ELeave) CInstanceIteratorTestManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	//
	return (self);
	}


void CInstanceIteratorTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	iTestLibrary->ReplaceFileL(KTestName);
	iTestLibrary->OpenFileL(KTestName);	
	
	iInstanceView = & iTestLibrary->SynCGetInstanceViewL();
	iEntryView = & iTestLibrary->SynCGetEntryViewL();
	}	


CInstanceIteratorTestManager::~CInstanceIteratorTestManager()	
	{
	delete iTestLibrary;
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Test cases forming the Iterator test suite
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
	
TInt CInstanceIteratorTestManager::RepeatingInstanceCount() const
	{
	return iNumRepeatingInstance;	
	}

TInt CInstanceIteratorTestManager::SingleInstanceCount() const
	{
	return iNumSingleInstance;	
	}
	
TInt CInstanceIteratorTestManager::ChildInstanceCount() const
	{
	return iNumChildInstance;	
	}
	
void CInstanceIteratorTestManager::PrintRamUsage()
	{
	TInt heapSizeInBytes;
	TInt heapCellCount = User::AllocSize(heapSizeInBytes);
	test.Printf(_L("%d KB of RAM on the heap"), heapSizeInBytes / 1024);
	}
	
void CInstanceIteratorTestManager::AddEntryL(const TInt aNumToAdd)
	{
	// Add Entries for Test
	test.Printf(_L("Adding %d entries"), aNumToAdd);
	
	TBuf<50> summary;
	TBuf<50> location;
	TBuf<50> description;


	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
	
	for (TInt index = 0; index < aNumToAdd; ++index)
		{
		CCalEntry* entry;
		HBufC8*    guid = HBufC8::NewL(255);
		TPtr8      uidP = guid->Des();
		
		iTestLibrary->RandomText8(uidP);
		
		if((index %2) == 0)
			{
			entry = iTestLibrary->CreateCalEntryL(CCalEntry::ETodo, guid);
			}
		else
			{
			entry = iTestLibrary->CreateCalEntryL(CCalEntry::EAppt, guid);
			}
	
		entriesToStore.AppendL(entry);
		
		iTestLibrary->SetEntryStartAndEndTimeL(entry);
				
		entry->SetSummaryL(KNonRepeatingEntryDes);
		
		iTestLibrary->RandomText(location);
		entry->SetLocationL(location);
		
		iTestLibrary->RandomText(description);
		entry->SetDescriptionL(description);
		
		
		if (index % KRepeatingEntry == 0)
			{
			//Every KRepeatingEntry, make it daily repeat with KRepeatCount count
			TCalRRule rpt(TCalRRule::EDaily);
			rpt.SetInterval(1);
			TCalTime repeatStart;
			if(entry->EntryTypeL() != CCalEntry::ETodo)
				{
				repeatStart.SetTimeLocalL((entry->StartTimeL()).TimeLocalL());	
				}
			else
				{
				repeatStart.SetTimeLocalL((entry->EndTimeL()).TimeLocalL());		
				}
			rpt.SetCount(KRepeatCount);	
			
			rpt.SetDtStart(repeatStart); 
			entry->SetRRuleL(rpt);
			
			// Set an RDate
			RArray<TCalTime> rDateList;
			CleanupClosePushL(rDateList);
			TCalTime rDateTime;
			rDateTime.SetTimeLocalL(repeatStart.TimeLocalL() + TTimeIntervalDays(2) + TTimeIntervalHours(5));
			rDateList.AppendL(rDateTime);
			entry->SetRDatesL(rDateList);
			CleanupStack::PopAndDestroy(&rDateList);
			
			// Set an exception date
			RArray<TCalTime> exDateList;
			CleanupClosePushL(exDateList);
			TCalTime exDateTime;
			exDateTime.SetTimeLocalL(repeatStart.TimeLocalL() + TTimeIntervalDays(4));
			exDateList.AppendL(exDateTime);
			entry->SetExceptionDatesL(exDateList);
			CleanupStack::PopAndDestroy(&exDateList);
			
			// Create a child entry!
			TCalTime recId;
			recId.SetTimeLocalL(repeatStart.TimeLocalL() + TTimeIntervalDays(6));
			CCalEntry* childEntry = CCalEntry::NewL(entry->EntryTypeL(), guid->AllocLC(), CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);

			CleanupStack::Pop(); // childGuid
			CleanupStack::PushL(childEntry);
			entriesToStore.AppendL(childEntry);
			CleanupStack::Pop(childEntry);

			iTestLibrary->SetEntryStartAndEndTimeL(childEntry,
													repeatStart.TimeLocalL() + TTimeIntervalDays(6) + TTimeIntervalHours(6),
													repeatStart.TimeLocalL() + TTimeIntervalDays(6) + TTimeIntervalHours(7));
			
			
			childEntry->SetSummaryL(KChildEntryDes);
			
			// Set the parent summary
			entry->SetSummaryL(KRepeatingEntryDes);
			iNumRepeatingInstance += KRepeatCount - 1;
			++iNumChildInstance;
			}
		else
			{
			++iNumSingleInstance;	
			}
			
		// store the entries in batches of KNumberOfEntriesToStoreAtOnce
		if (index % KNumberOfEntriesStoreAtOnce == 0)
			{
			TInt entriesStored(0);
			iEntryView->StoreL(entriesToStore, entriesStored); //temp
			entriesToStore.ResetAndDestroy();
			test.Printf(_L("Stored %d entries so far"), index);
			}
		}
	
	// store any extra entries
	TInt entriesStored(0);
	iEntryView->StoreL(entriesToStore, entriesStored); //temp
	test.Printf(_L("Stored %d entries in total\n"), aNumToAdd);
	
	CleanupStack::PopAndDestroy(&entriesToStore);
	}
	
CCalInstanceIterator* CInstanceIteratorTestManager::CreateIteratorLC(const TDesC& aSearchString, CalCommon::TCalTimeRange aTimeRange) const
	{
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	
	CCalFindInstanceSettings* findSetting = CCalFindInstanceSettings::NewL(filter, aTimeRange);
	CleanupStack::PushL(findSetting);
	
	CCalInstanceView::TCalSearchParams searchParams(aSearchString, CalCommon::TCalSearchBehaviour(CalCommon::EFoldedTextSearch));
	findSetting->SetTextSearchL(searchParams);
	CCalInstanceIterator* iterator = iInstanceView->FindInstanceL(*findSetting);
	CleanupStack::PopAndDestroy(findSetting);
	CleanupStack::PushL(iterator);
	return iterator;
	}
	
void CInstanceIteratorTestManager::TestIteratingInstanceL(const TInt aNumInstance, const TDesC& aSearchString)
	{
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);

	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	
	test.Printf(_L("Before creating iterator"));
	PrintRamUsage();
	
  	CCalInstanceIterator* iterator = CreateIteratorLC(aSearchString, timeRange);//Push the CCalFindInstanceSettings object
	
	test.Printf(_L("Created iterator"));
	PrintRamUsage();

	TTime previorsInstanceTime = Time::NullTTime();
	TInt count = 0;

	while(CCalInstance* nextinstance = iterator->NextL())
		{
		CleanupStack::PushL(nextinstance);
		test(nextinstance!=NULL);
		
		if(count>0)
			{
			CCalInstance* preinstance = iterator->PreviousL();
			CleanupStack::PushL(preinstance);
			test(preinstance!=NULL);
			test(nextinstance->Time().TimeLocalL() >= preinstance->Time().TimeLocalL());
			CleanupStack::PopAndDestroy(preinstance);
			
			CleanupStack::PopAndDestroy(nextinstance);
			nextinstance = iterator->NextL();
			CleanupStack::PushL(nextinstance);
			}
			
		
		TBuf<50> timeBuf;
		nextinstance->Time().TimeLocalL().FormatL(timeBuf, KFormatDateTime());
		test.Printf(timeBuf);
		
		test(nextinstance->Time().TimeLocalL() >= previorsInstanceTime);
		previorsInstanceTime = nextinstance->Time().TimeLocalL();
		instances.AppendL(nextinstance);
		CleanupStack::Pop(nextinstance);
		++count;
		
		test.Printf(_L("instance count = %d"), count);
		}
	
	test(aNumInstance==instances.Count());
	test(count==aNumInstance);
	
	test.Printf(_L("Finished iterating"));
	PrintRamUsage();
	test.Printf(_L("Deleteing the instances"));
	instances.ResetAndDestroy();
	PrintRamUsage();
	
	CleanupStack::PopAndDestroy(2, &instances);//instances, iterator
	}
	
void CInstanceIteratorTestManager::PerformanceComparisonL()
	{
	PrintRamUsage();
	test.Printf(_L("Get all instances using FindInstanceL()"));
	TPerformanceTimer timer(test);
	timer.Start();
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	iInstanceView->FindInstanceL(instances, filter, timeRange);
	timer.Stop();
	timer.PrintOut();
	PrintRamUsage();
	test(instances.Count() == RepeatingInstanceCount() + ChildInstanceCount() + SingleInstanceCount());
	CleanupStack::PopAndDestroy(&instances);
	PrintRamUsage();
	
 	test.Printf(_L("Create iterator and get the first instance using iterator"));
	timer.Start();
	CCalInstanceIterator* iterator = CreateIteratorLC(KNullDesC, timeRange);
	
	PrintRamUsage();

	CCalInstance* nextinstance = iterator->NextL();
	timer.Stop();
	timer.PrintOut();
	test(nextinstance!=NULL);
	delete nextinstance;
	CleanupStack::PopAndDestroy(iterator);

	PrintRamUsage();

	test.Printf(_L("Create iterator and get all instances using iterator"));
	timer.Start();

	RPointerArray<CCalInstance> instancesI;
	CleanupResetAndDestroyPushL(instancesI);

	CCalInstanceIterator* iteratorI = CreateIteratorLC(KNullDesC, timeRange);
	
	PrintRamUsage();

	while(CCalInstance* instance = iteratorI->NextL())
		{
		CleanupStack::PushL(instance);
		instancesI.AppendL(instance);
		CleanupStack::Pop(instance);
		}
		
	timer.Stop();
	timer.PrintOut();
	
	test(RepeatingInstanceCount() + ChildInstanceCount() + SingleInstanceCount() == instancesI.Count());
	
	PrintRamUsage();
	
	test.Printf(_L("Delete the instances"));
	instancesI.ResetAndDestroy();
	
	PrintRamUsage();
	
	test.Printf(_L("Delete the iterator"));
	CleanupStack::PopAndDestroy(iteratorI);
	CleanupStack::PopAndDestroy(&instancesI);
	
	PrintRamUsage();
	}
	
	
void CInstanceIteratorTestManager::NegtiveTestL() const
	{
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);

	CCalInstanceIterator* iterator = CreateIteratorLC(KNullDesC, timeRange);//Push the CCalFindInstanceSettings object

	test.Printf(_L("PreviousL should return NULL when the iterator is at the first"));
	test(!iterator->PreviousL());

	test.Printf(_L("Move the iterator to the last"));	
	while(iterator->HasMore())
		{
		CCalInstance* nextinstance = iterator->NextL();
		delete nextinstance;
		}

	test.Printf(_L("NextL should return NULL when the iterator is at the last"));
	test(!iterator->NextL());
	
	CleanupStack::PopAndDestroy(iterator);
	}
	
void CInstanceIteratorTestManager::OomTestL(TType aType, TInt aFailAt)
	{
	// Print the oom test status
	if (aFailAt % 100 == 0)
		{
		TBuf<200> message;
		message.Append(_L("OOM fail at "));
		message.AppendNum(aFailAt);
		message.Append(_L(" on the "));
		
		if (aType == EClient)
			{
			message.Append(_L("client side"));
			}
		else
			{
			message.Append(_L("server side"));
			}
		
		test.Printf(message);
		}
	
	TTime searchMin(TDateTime(2000, EJanuary, 0, 0, 0, 0, 0));
	TCalTime minTime;
	minTime.SetTimeLocalL(searchMin);

	TTime searchMax(TDateTime(2001, EJanuary, 0, 0, 0, 0, 0));
	TCalTime maxTime;
	maxTime.SetTimeLocalL(searchMax);
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);

	CCalInstanceIterator* iterator = CreateIteratorLC(*iOomSearchString, timeRange);//Push the CCalFindInstanceSettings object

	TInt count = 0;

	while(iterator->HasMore())
		{
		CCalInstance* nextinstance = iterator->NextL();
		delete nextinstance;
		if(count>0)
			{
			CCalInstance* preinstance = iterator->PreviousL();
			delete preinstance;			
			nextinstance = iterator->NextL();
			delete nextinstance;
			}
		++count;
		}
		
	test.Printf(_L("OOM success at %d allocations!\n"), aFailAt);
	CleanupStack::PopAndDestroy(iterator);
	}	
	
void CInstanceIteratorTestManager::OomTestL()
	{
	test.Printf(_L("Cleaning the database"));
	iTestLibrary->CleanDatabaseL();
	AddEntryL(KNumberOfEntriesInOom);
	
	test.Printf(_L("Test oom with KRepeatingEntryDes"));
	iOomSearchString = KRepeatingEntryDes().AllocL();
	CCalSession& calSession = iTestLibrary->GetSession();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	delete iOomSearchString;
	iOomSearchString = NULL;
	
	test.Printf(_L("Test oom with KNonRepeatingEntryDes"));
	iOomSearchString = KNonRepeatingEntryDes().AllocL();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	delete iOomSearchString;
	iOomSearchString = NULL;
	
	test.Printf(_L("Test oom with KChildEntryDes"));
	iOomSearchString = KChildEntryDes().AllocL();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	delete iOomSearchString;
	iOomSearchString = NULL;
	}
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 

static void DoTestL()
	{
	CInstanceIteratorTestManager* testManager = CInstanceIteratorTestManager::NewLC();
	
	
	TPerformanceTimer timer(test);
	
	test.Next(_L("Test the iterator when the database is empty"));

	testManager->TestIteratingInstanceL(0, KNullDesC);
	
	// Add the test data
	testManager->AddEntryL(KNumberOfEntries);
	
	test.Next(_L("Test the iterator to find all instances in the database"));

	timer.Start();
	testManager->TestIteratingInstanceL(testManager->RepeatingInstanceCount() + testManager->ChildInstanceCount() + testManager->SingleInstanceCount(),KNullDesC);
	timer.Stop();
	timer.PrintOut();

	test.Next(_L("search for instances with string KRepeatingEntryDes as summary"));// it is equal to the number of instances of repeating entries

	timer.Start();
	testManager->TestIteratingInstanceL(testManager->RepeatingInstanceCount(), KRepeatingEntryDes);
	timer.Stop();
	timer.PrintOut();

	test.Next(_L("Search for instances with string KChildEntryDes as summary"));// it is equal to the number of instances of child entries

	timer.Start();
	testManager->TestIteratingInstanceL(testManager->ChildInstanceCount(), KChildEntryDes);
	timer.Stop();
	timer.PrintOut();

	test.Next(_L("search for instances with string KNonRepeatingEntryDes as summary"));// it is equal to the number of non-repeating entries

	timer.Start();
	testManager->TestIteratingInstanceL(testManager->SingleInstanceCount(), KNonRepeatingEntryDes);
	timer.Stop();
	timer.PrintOut();
	
	test.Next(_L("Negtive test"));

	testManager->NegtiveTestL();

	test.Next(_L("Performance comparison"));

	testManager->PerformanceComparisonL();
	
	test.Next(_L("OOM test"));

	testManager->OomTestL();
	
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-INSTANCEITERATOR-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-INSTANCEITERATOR-0001 Calendar Interim API Iterator test suite"));

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
