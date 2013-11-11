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
#include <calsession.h>
#include <e32test.h>
#include <caliterator.h>
#include <caluser.h>

// the objective of this test suite is to test the CCalIter interface and object behaviour

_LIT(KTestName,"tcal_iterator");

RTest test(KTestName);


const TInt KNumberOfEntries = 20;

class CIteratorTestManager : public CBase
	{
public:

    static CIteratorTestManager* NewLC();
	~CIteratorTestManager();
	
	inline void CleanDatabaseL();
	void TestEmptyIteratorL();
	void TestAddEntryL(const TInt aNumToAdd);	
	void TestIterEntryAndFetchL(const TInt aNumOfEntries);
	void IterEntryAndFetchL(const TInt aNumOfIterations);
	void TestIterEntryAndFetchAndAddL(const TInt aNumOfIterations, const TInt aNumToAdd, const TInt aNumOfEntries);
	void TestIterEntryAndFetchAndDeleteL(const TInt aNumOfIterations, const TInt aNumToRemove, const TInt aNumOfEntries);
	
	
private:

	// avoid any method but NewL instancing the class
	CIteratorTestManager() {	}
	// no copy constructor and assignment operator
	CIteratorTestManager(CIteratorTestManager& );
	CIteratorTestManager& operator = (const CIteratorTestManager& );

	void ConstructL();

	
private:

	CCalTestLibrary* iTestLibrary;
	};


// Constructor/Destructor

CIteratorTestManager* CIteratorTestManager::NewLC()
	{
	CIteratorTestManager* self = new (ELeave) CIteratorTestManager();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}


void CIteratorTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	iTestLibrary->ReplaceFileL(KTestName);
	iTestLibrary->OpenFileL(KTestName);	
	}	


CIteratorTestManager::~CIteratorTestManager()	
	{
	delete iTestLibrary;
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Test cases forming the Iterator test suite
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
inline void CIteratorTestManager::CleanDatabaseL()
	{
	iTestLibrary->CleanDatabaseL();
	}


void CIteratorTestManager::TestEmptyIteratorL()
	{
	// Test Empty Iterator Returns NULL Des	
	test.Next(_L("Test Empty Iterator"));

	
	CCalIter& iter = iTestLibrary->GetIterL();
	
	TPtrC8 Uid(iter.FirstL());
	test(Uid == KNullDesC8()); 
	
	// Try and iterate beyond the end of the iterator
	TRAPD(err, Uid.Set(iter.NextL()));  
	test(err == KErrCorrupt);
	}



void CIteratorTestManager::TestAddEntryL(const TInt aNumToAdd)
	{
	// Add Entries for Test
	test.Next(_L("Adding entries"));

	
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
		
		if( (index %2) == 0 )
			{
			entry = iTestLibrary->CreateCalEntryL(CCalEntry::ETodo, guid);
			}
		else
			{
			entry = iTestLibrary->CreateCalEntryL(CCalEntry::EAppt, guid);
			}
	
		TInt err = entriesToStore.Append(entry);
		test(err == KErrNone);
		
		iTestLibrary->SetEntryStartAndEndTimeL(entry);
				
		iTestLibrary->RandomText(summary);
		entry->SetSummaryL(summary);
		
		iTestLibrary->RandomText(location);
		entry->SetLocationL(location);
		
		iTestLibrary->RandomText(description);
		entry->SetDescriptionL(description);
		}

	TInt entriesStored(0);
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored); //temp
	test(entriesStored == aNumToAdd);
	
	CleanupStack::PopAndDestroy(&entriesToStore);
	}


void CIteratorTestManager::TestIterEntryAndFetchL(const TInt aNumOfEntries)
	{
	test.Next(_L("Check Iterator entries"));

	
	CCalIter& iter = iTestLibrary->GetIterL();
	
	TInt i = 0;
		
	for( TPtrC8 Uid(iter.FirstL()); Uid != KNullDesC8(); Uid.Set(iter.NextL()), ++i )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iTestLibrary->SynCGetEntryViewL().FetchL(Uid, entryList);
		CleanupStack::PopAndDestroy(&entryList);			
		}
		
  	test(i == aNumOfEntries);
	}


void CIteratorTestManager::IterEntryAndFetchL(const TInt aNumOfIterations)
	{
	test.Next(_L("Iterate entries"));

	
	CCalIter& iter = iTestLibrary->GetIterL();
	
	TInt i = 0;
	
	for( TPtrC8 Uid(iter.FirstL()); Uid != KNullDesC8() || i < aNumOfIterations; Uid.Set(iter.NextL()), ++i )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iTestLibrary->SynCGetEntryViewL().FetchL(Uid, entryList);
		CleanupStack::PopAndDestroy(&entryList);			
		}
	}



void CIteratorTestManager::TestIterEntryAndFetchAndAddL(const TInt aNumOfIterations, const TInt aNumToAdd, const TInt aNumOfEntries)
	{
	test.Next(_L("Iterate entries and Add new"));

		
	CCalIter& iter = iTestLibrary->GetIterL();
	
	TInt   i = 0;
	TPtrC8 Uid(iter.FirstL());

	for( ; Uid != KNullDesC8() && i < aNumOfIterations; Uid.Set(iter.NextL()), ++i )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iTestLibrary->SynCGetEntryViewL().FetchL(Uid, entryList);
		CleanupStack::PopAndDestroy(&entryList);			
		}		
	
	TestAddEntryL(aNumToAdd);
	
	for( ; Uid != KNullDesC8(); Uid.Set(iter.NextL()), ++i )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iTestLibrary->SynCGetEntryViewL().FetchL(Uid, entryList);
		CleanupStack::PopAndDestroy(&entryList);			
		}

	test(i == aNumOfEntries);
	}


void CIteratorTestManager::TestIterEntryAndFetchAndDeleteL(const TInt aNumOfIterations, const TInt aNumToRemove, const TInt aNumOfEntries)
	{
	test.Next(_L("Iterate entries and Delete some"));

		
	CCalIter& iter = iTestLibrary->GetIterL();
	
	TInt   i = 0;	
	TPtrC8 Uid(iter.FirstL());

	for( ; Uid != KNullDesC8() && i < aNumOfIterations; Uid.Set(iter.NextL()), ++i )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iTestLibrary->SynCGetEntryViewL().FetchL(Uid, entryList);
		CleanupStack::PopAndDestroy(&entryList);			
		}		


	for( TInt j = 0; Uid != KNullDesC8() && j < aNumToRemove; Uid.Set(iter.NextL()), ++j )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iTestLibrary->SynCGetEntryViewL().FetchL(Uid, entryList);
		for( TInt k = 0; k < entryList.Count(); ++k )
			{
			iTestLibrary->SynCGetEntryViewL().DeleteL( *(entryList[k]) );
			}
		CleanupStack::PopAndDestroy(&entryList);			
		}	


	for( ; Uid != KNullDesC8(); Uid.Set(iter.NextL()), ++i )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iTestLibrary->SynCGetEntryViewL().FetchL(Uid, entryList);
		CleanupStack::PopAndDestroy(&entryList);			
		}

	test(i == aNumOfEntries);
	}
	

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CIteratorTestManager* testManager = CIteratorTestManager::NewLC();
	
	
	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	// case 1: test the iterator when the database is empty
	
	testManager->TestEmptyIteratorL();


	// case 2: test the iterator twice with some parent entries in the database

	testManager->TestAddEntryL(KNumberOfEntries);

	// iterate once to verify all the parent entries are seen
	testManager->TestIterEntryAndFetchL(KNumberOfEntries);
	// iterate again to verify all the parent entries are seen a second time
	testManager->TestIterEntryAndFetchL(KNumberOfEntries);


	// case 3: test the iterator twice with some parent entries add on top
	//         of the previous ones in the database

	testManager->TestAddEntryL(KNumberOfEntries);

	// make sure the parent entries are the double now...
	testManager->TestIterEntryAndFetchL(KNumberOfEntries * 2);
	// and check a second time
	testManager->TestIterEntryAndFetchL(KNumberOfEntries * 2);


	// case 4: cleanup and restart from scratch the test of the iterator twice
	//         with some parent entries in the database; making sure the iterator
	//         behaves correctly working on a clean database

	testManager->CleanDatabaseL();

	testManager->TestEmptyIteratorL();

	testManager->TestAddEntryL(KNumberOfEntries);

	// iterate once to verify all the parent entries are seen
	testManager->TestIterEntryAndFetchL(KNumberOfEntries);
	// iterate again to verify all the parententries are seen a second time
	testManager->TestIterEntryAndFetchL(KNumberOfEntries);	
	
	
	// case 5: cleanup and restart from scratch the test of the iterator 
	//         stopping half way, adding some entries in the database and
	//         running again

	testManager->CleanDatabaseL();

	testManager->TestEmptyIteratorL();

	testManager->TestAddEntryL(KNumberOfEntries);

	// just iterate half of the parent entries
	testManager->IterEntryAndFetchL(KNumberOfEntries / 2);
	
	// add some more parent entries
	testManager->TestAddEntryL(KNumberOfEntries);	

	// make sure the parent entries are the double now...
	testManager->TestIterEntryAndFetchL(KNumberOfEntries * 2);
	// and check a second time
	testManager->TestIterEntryAndFetchL(KNumberOfEntries * 2);
	
	
	// case 6: cleanup and restart from scratch the test of the iterator 
	//         stopping half way, adding some parent entries in the database
	//         and continue running
	
	testManager->CleanDatabaseL();

	testManager->TestEmptyIteratorL();

	testManager->TestAddEntryL(KNumberOfEntries);	
/*** TODO: This test is failing at the moment, it just gets a partial number of new total entries
	testManager->TestIterEntryAndFetchAndAddL(KNumberOfEntries / 2, KNumberOfEntries, KNumberOfEntries * 2);
***/


	// case 7: cleanup and restart from scratch the test of the iterator 
	//         stopping half way, deleting some parent entries in the database
	//         and continue running

	testManager->CleanDatabaseL();

	testManager->TestEmptyIteratorL();

	testManager->TestAddEntryL(KNumberOfEntries);	

	testManager->TestIterEntryAndFetchAndDeleteL( (KNumberOfEntries / 2), (KNumberOfEntries / 4), (KNumberOfEntries - (KNumberOfEntries / 4)) );	


	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();


	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-ITERATOR-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-ITERATOR-0001 Calendar Interim API Iterator test suite"));

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
