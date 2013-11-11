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
#include <calprogresscallback.h>
#include <consolealarmalertservermain.h>
#include <calcategory.h>
#include <calcategorymanager.h>
#include <caliterator.h>
#include <caluser.h>
#include <e32test.h>

_LIT(KTestName,"TCal_Category");
RTest test(KTestName);
_LIT(KShopCategory,"Shopping Category");

const TInt KNumberOfEntries = 4;
const TInt KNumNormalCategories = 12;


class CDummyCalendarApp;
class CAppActive : public CActive
	{
public:
	enum TDummyAppStep
		{
		EStepOpenModel,
		ESynchronousTest,
		EFilterCategory,
		ESynchronousFilterCategory,
		EDeleteCategory,
		EEnd
		};

	CAppActive(CDummyCalendarApp& aApp);
	~CAppActive();
	void DoCancel();
	void RunL();
	void Start();
private:
	CDummyCalendarApp& iApp;
	TDummyAppStep iStep;
	};
	
class CDummyCalendarApp : public CCalTestLibrary
	{
public:
	static CDummyCalendarApp* NewLC();
	~CDummyCalendarApp();

	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();
	void ConstructL();
	void OpenModelL();
	void Close();
		
	void AddEntryL(TInt aNumToAdd);
	void AddCategoryToListL();
	void IterEntryAndUpdateCatL();
	void SynchronousTestL();
	void FilterCategoryL(TBool aIsAsynchronous);
	void DeleteCategoryL();
	void CheckFilterResult();
	void CheckDeleteResultL();
	void AddAOAndStartL();
	void GetExistingCategoryL();

private: 
	CCalEntryView* iEntryView;			//doesn't own
	CCalCategoryManager* iCategoryManager;
	CCalCategory* iCategory;
	RPointerArray<CCalEntry> iFilteredEntry;
	CAppActive* iActive;
	TBool iProgressCallBackOccurred;
	TBool iCompletionCallBackOccurred;
	};

CDummyCalendarApp* CDummyCalendarApp::NewLC()
	{
	CDummyCalendarApp* self = new (ELeave) CDummyCalendarApp();
	
	CleanupStack::PushL(self);
	self->ConstructL();

	return (self);
	}
	
CDummyCalendarApp::~CDummyCalendarApp()
	{
	delete iActive;
	delete iCategory;
	iFilteredEntry.ResetAndDestroy();
	iFilteredEntry.Close();
	delete iCategoryManager;
	}

CAppActive::CAppActive(CDummyCalendarApp& aApp)
	: CActive(EPriorityHigh),iApp(aApp)
	{
	iStep = EStepOpenModel;
	}

CAppActive::~CAppActive()
	{
	Cancel();
	}

void CAppActive::Start()
	{
	if (!IsActive())
		{
		SetActive();
		}
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status,KErrNone);
	}
	
void CAppActive::DoCancel()
	{
	iApp.Close();
	}
	

void CDummyCalendarApp::ConstructL()
	{
	BaseConstructL();
	iActive = new (ELeave) CAppActive(*this);
	}


void CDummyCalendarApp::Progress(TInt aPercentageCompleted)
	{
	TBuf<64> buf;
	buf.Copy(_L("MCalProgress callback - Percent complete ="));
	buf.AppendNum(aPercentageCompleted);
	test.Printf(buf);
	iProgressCallBackOccurred = ETrue;
	}
	
void CDummyCalendarApp::Completed(TInt aError)
	{	
	TBuf<64> buf;
	buf.Copy(_L("MCalProgress callback - Completed, err ="));
	buf.AppendNum(aError);
	test.Printf(buf);
	iActive->Start();
	iCompletionCallBackOccurred = ETrue;
	}

TBool CDummyCalendarApp::NotifyProgress()
	{
	return ETrue;
	}
	

void CDummyCalendarApp::AddEntryL(TInt aNumToAdd)
	{
	test.Next(_L("Adding Entries"));

	TBuf<50> summary;
	TBuf<50> location;
	TBuf<50> description;
	TBuf<50> address1;
	TBuf<50> address2;

	CCalCategory* holidayCat=NULL;
	CCalCategory* shoppingCat=NULL;

	for (TInt index = 0; index < aNumToAdd; ++index)
		{
		CCalEntry* entry;
		HBufC8* guid = HBufC8::NewL(255);	
		TPtr8 uidP = guid->Des();
		RandomText8(uidP);

		
		if(index%2 == 0)
			{
			entry = CreateCalEntryL(CCalEntry::ETodo, guid);
			}
		else
			{
			entry = CreateCalEntryL(CCalEntry::EAppt, guid);
			}
			
		CleanupStack::PushL(entry);
		SetEntryStartAndEndTimeL(entry);		
		RandomText(summary);
		entry->SetSummaryL(summary);
		
		RandomText(location);
		entry->SetLocationL(location);
		
		RandomText(description);
		entry->SetDescriptionL(description);

		RandomText(address1);
		CCalAttendee* attendee1 = CCalAttendee::NewL(address1);
	    CleanupStack::PushL(attendee1);
		entry->AddAttendeeL(attendee1);
		CleanupStack::Pop(attendee1);

		RandomText(address2);
		CCalAttendee* attendee2 = CCalAttendee::NewL(address2);
	    CleanupStack::PushL(attendee2);
		entry->AddAttendeeL(attendee2);
		CleanupStack::Pop(attendee2);

		holidayCat = iCategoryManager->CategoryL(CCalCategory::ECalHoliday);
		shoppingCat = iCategoryManager->CategoryL(CCalCategory::ECalExtended);
		
		test(holidayCat->Category() == CCalCategory::ECalHoliday);
		test(shoppingCat->Category() == CCalCategory::ECalExtended);
		test(holidayCat->ExtendedCategoryName() == KNullDesC);
		test(shoppingCat->ExtendedCategoryName() == KShopCategory);
		
		entry->AddCategoryL(holidayCat);
		entry->AddCategoryL(shoppingCat);

		RPointerArray<CCalEntry> entriesToStore;
		CleanupResetAndDestroyPushL(entriesToStore);
		entriesToStore.AppendL(entry);		
		TInt entriesStored(0);
		iEntryView->StoreL(entriesToStore, entriesStored); //temp
		test(entriesStored == 1);
		CleanupStack::PopAndDestroy(); // entriesToStore)
		CleanupStack::Pop(entry);
		}
	}


void CDummyCalendarApp::AddCategoryToListL()
	{
	test.Next(_L("Add a new Category"));

	
	iCategoryManager = CCalCategoryManager::NewL(GetSession());
	test(iCategoryManager->CategoryCountL() == KNumNormalCategories);   // 12 normal categories

	CCalCategory* shoppingCat = CCalCategory::NewL(KShopCategory);
	CleanupStack::PushL(shoppingCat);
	iCategoryManager->AddCategoryL(*shoppingCat);   
	CleanupStack::PopAndDestroy(shoppingCat);

	test(iCategoryManager->CategoryCountL() == KNumNormalCategories + 1); // 12 normal categories + 1 just added

	CCalCategory* addedCat = iCategoryManager->CategoryL(KNumNormalCategories);  // as numbered from 0
	test(addedCat->ExtendedCategoryName().Compare(KShopCategory) == 0);
	delete addedCat;
	}

void CDummyCalendarApp::OpenModelL()
	{
	ReplaceFileL(KTestName);
	OpenFileL(KTestName);
	iEntryView = &AsynCGetEntryViewL();
	}


void CDummyCalendarApp::IterEntryAndUpdateCatL()
	{
	test.Next(_L("Test Updated Categories"));

	
	CCalIter& iter = GetIterL();
	
	TPtrC8 Uid(iter.FirstL());

	// go through iterator
	while (Uid != KNullDesC8())
		{
		RPointerArray<CCalEntry> entryList;
	    CleanupResetAndDestroyPushL(entryList);
		iEntryView->FetchL(Uid, entryList);
		CCalEntry* entry = entryList[0];
 		const RPointerArray<CCalCategory>* entryCatList = &(entry->CategoryListL());
		test(entryCatList->Count() == 2);

		test((*entryCatList)[0]->Category() == CCalCategory::ECalHoliday);
		test((*entryCatList)[0]->ExtendedCategoryName() == KNullDesC16());
		test((*entryCatList)[1]->Category() == CCalCategory::ECalExtended);
		test((*entryCatList)[1]->ExtendedCategoryName() == KShopCategory());

		RPointerArray<CCalAttendee>* entryAttendeeList = &(entry->AttendeesL());
		test(entryAttendeeList->Count()==2);
		CleanupStack::PopAndDestroy(); // entryList
		Uid.Set(iter.NextL());
		}

	Uid.Set(iter.FirstL());

	TInt i((0));

  	while (Uid != KNullDesC8())
		{
		++i;
		RPointerArray<CCalEntry> entryList;
	    CleanupResetAndDestroyPushL(entryList);
		iEntryView->FetchL(Uid, entryList);
		CCalEntry* entry = entryList[0];
  		const RPointerArray<CCalCategory>* entryCatList = &(entry->CategoryListL());
		if(i%2)
			{
			entry->DeleteCategoryL(0);
			}
		else
			{
			entry->DeleteCategoryL(1);
			}

		test(entryCatList->Count() == 1);

		RPointerArray<CCalEntry> entriesToUpdate;
		entriesToUpdate.AppendL(entry);
		CleanupClosePushL(entriesToUpdate);
		TInt entriesUpdated(0);
		iEntryView->UpdateL(entriesToUpdate, entriesUpdated);
		test(entriesUpdated == 1);
		CleanupStack::PopAndDestroy(); // entriesToUpdate
		
		CleanupStack::PopAndDestroy(); // entryList
		Uid.Set(iter.NextL());
		}
	}


void CDummyCalendarApp::CheckDeleteResultL()
	{
	test.Next(_L("Test Delete Results"));

	test(iCategoryManager->CategoryCountL() == KNumNormalCategories);

	CCalIter& iter = GetIterL();

	TPtrC8 Uid(iter.FirstL());
	TInt i = 0;
	while (Uid != KNullDesC8)
		{
		RPointerArray<CCalEntry> entryList;
	    CleanupResetAndDestroyPushL(entryList);
		iEntryView->FetchL(Uid, entryList);
		CCalEntry* entry = entryList[0];
  		const RPointerArray<CCalCategory>* entryCatList = &(entry->CategoryListL());
		if (entryCatList->Count()==0)
			{
			++i;
			}

		CleanupStack::PopAndDestroy(); // entryList
		Uid.Set(iter.NextL());
		}

	test(i == KNumberOfEntries / 2);
	}

void CDummyCalendarApp::SynchronousTestL()
	{
	AddCategoryToListL();
	AddEntryL(KNumberOfEntries);
	IterEntryAndUpdateCatL();
	GetExistingCategoryL();
	}

void CDummyCalendarApp::GetExistingCategoryL()
	{
	CCalCategory* holidayCat = CCalCategory::NewL(CCalCategory::ECalHoliday);
	CleanupStack::PushL(holidayCat);
	test(holidayCat->Category() == CCalCategory::ECalHoliday);
	test(holidayCat->ExtendedCategoryName() == KNullDesC);
	CleanupStack::PopAndDestroy(holidayCat);
	}

void CDummyCalendarApp::FilterCategoryL(TBool aIsAsynchronous)
	{
	iCategory = iCategoryManager->CategoryL(CCalCategory::ECalHoliday);
	if(!aIsAsynchronous)
		{
		iCategoryManager->FilterCategoryL(*iCategory,iFilteredEntry);	
		}
	else
		{
		iCategoryManager->FilterCategoryL(*iCategory,iFilteredEntry, *this);	
		CCalCategory* dummyCategory = CCalCategory::NewL(CCalCategory::ECalPhoneCall);
		TRAPD(err, iCategoryManager->AddCategoryL(*dummyCategory));
		test(err == KErrArgument);
		
		TRAP(err, iCategoryManager->DeleteCategoryL(*dummyCategory, *this));
		test(err == KErrLocked);
		
		delete dummyCategory;
		}
	}


void CDummyCalendarApp::CheckFilterResult()
	{
	test.Next(_L("Filter Count"));

	test(iFilteredEntry.Count() == KNumberOfEntries / 2);
	iFilteredEntry.ResetAndDestroy();
	delete iCategory;
	iCategory = NULL;
	}


void CDummyCalendarApp::DeleteCategoryL()
	{
	delete iCategory;
	iCategory = NULL;
	iCategory=iCategoryManager->CategoryL(CCalCategory::ECalExtended);
	iCategoryManager->DeleteCategoryL(*iCategory, *this);
	}

void CAppActive::RunL()
	{
	switch (iStep)
		{
		case EStepOpenModel:
			iApp.OpenModelL();
			iStep = ESynchronousTest;
			break;
		case ESynchronousTest:
			iApp.SynchronousTestL();
			iStep = EFilterCategory;
			Start();
			break;
		case EFilterCategory:
			iApp.FilterCategoryL(ETrue);
			iStep = ESynchronousFilterCategory;
			break;
		case ESynchronousFilterCategory:
			iApp.CheckFilterResult();
			iApp.FilterCategoryL(EFalse);
			iApp.CheckFilterResult();
			iStep = EDeleteCategory;
			Start();
			break;
		case EDeleteCategory:
			iApp.DeleteCategoryL();
			iStep = EEnd;
			break;
		case EEnd:
			iApp.CheckDeleteResultL();
			iApp.Close();
			break;
		default:
			User::Leave(KErrNotFound);
		}
	}

void CDummyCalendarApp::Close()
	{
	CActiveScheduler::Stop();
	}

void CDummyCalendarApp::AddAOAndStartL()
	{
	CActiveScheduler::Add(iActive);
	iActive->Start();
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CDummyCalendarApp* testManager = CDummyCalendarApp::NewLC();
	
	
	TPerformanceTimer timer(test);
	timer.Start();	


	// Run the test suite

	testManager->AddAOAndStartL();
	CActiveScheduler::Start();
	

	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-CATEGORY-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-CATEGORY-0001 Calendar Interim API Category test suite"));

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

