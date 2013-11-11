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
// This is the test which tests the behaviour when two clients, presented by CMultiThreadTestApp1 and CMultiThreadTestApp2,
// access the same Calendar file spontaneously:
// 1. Both threads try to create a view and delete it repeatedly, which results in the server building indexes and aborting it constantly.
// 2. One thread (CMultiThreadTestApp1 and CAppActive1) is acted as decribed 1 continuously. The other thread finishes index building and doing some normal operation, e.g. find instances
// 
//

#include "caltestlib.h"
#include <calentry.h>
#include <calinstance.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <caluser.h>
#include <e32test.h>
#include <calsession.h>
#include <caldataformat.h>
#include <s32file.h>

_LIT(KTestName,"TCal_Multi_Threads_Access");
_LIT(KTestFile,"TCal_Two_threads");
_LIT(KThreadName, "tcal_other_thread");
_LIT(KSyncSemaphoreOne, "SyncSemaphoreOne");
_LIT(KSyncSemaphoreTwo, "SyncSemaphoreTwo");

RTest test(KTestName);
const TInt KNumberOfEntriesToAdd = 100;

static const TInt KMaxHeapSize = 0x40000;


class CMultiThreadTestApp;

class CAppActive : public CActive
	{
public:

	enum TDummyAppStep
		{
		EStepCreateView,
		EStepAddEntryAndReopenfile,
		EStepStartBuildIndex,
		EStepDeleteView,
		EStepMutipleOperation1,
		EStepMutipleOperation2,
		EEnd
		};

	CAppActive();
	~CAppActive();
	void DoCancel();
	void Start();
	void SetApp(CMultiThreadTestApp* aApp);
protected:
	CMultiThreadTestApp* 	iApp;//not owned
	TDummyAppStep 			iStep;
	TInt 		   	  iCount;
	TBool 			  iCancelBuildIndex;
	CCalEntryView*	  iEntryView;
	};

class CAppActive1 : public CAppActive
	{
public:
	~CAppActive1();

private:
	void RunL();
	};

class CAppActive2 : public CAppActive
	{
public:
	~CAppActive2();

private:
	void RunL();
	CCalInstanceView* iInstanceView;
	};
	
class CMultiThreadTestApp : public CCalTestLibrary, MCalDataExchangeCallBack
	{
public:
	enum TStatus
		{
		ENone,
		EBuildIndexCancel,
		EBuildIndexComplete,
		ESynchentry
		};
	
	~CMultiThreadTestApp();

	void ConstructL(CAppActive* aAO);
	
	virtual void  Close() = 0;
	virtual void SynchWithOtherThread() = 0;
	virtual void GetThreadL() = 0;
	
	void AddAOAndStartL();
	void AddEntryL(CCalEntryView& aView, const TInt aNumToAdd);
	void FetchEntryL(CCalEntryView& aView, RPointerArray<CCalEntry>& aEntries);

	TStatus Status();
	void SetStatus(TStatus aStatus);
	void DeleteEntryL(CCalEntryView& aView, CCalEntry::TType aType, TBool aIsSynchronous);
	void SynchEntryL(CCalEntryView& aView);

private: 
	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();

	void Completed();

protected:
	RSemaphore			iSyncSemaphoreOne;
	RSemaphore			iSyncSemaphoreTwo;
	RThread				iThread;
	RPointerArray<CCalEntry> iEntries;

private:
	CAppActive* 		iActive;
	TBool 				iStatus;
	};

class CMultiThreadTestApp1 : public CMultiThreadTestApp
	{
public:
	static CMultiThreadTestApp1* NewL();
	~CMultiThreadTestApp1();
	void Close(); 
	void ConstructL();
	void GetThreadL();
	void SynchWithOtherThread();
	};

class CMultiThreadTestApp2 : public CMultiThreadTestApp
	{
public:

	static CMultiThreadTestApp2* NewL();
	void ConstructL();
	void Close();
	void SynchWithOtherThread();
private:
	void GetThreadL();
	};


// class CMultiThreadTestApp //
CMultiThreadTestApp::~CMultiThreadTestApp()
	{
	delete iActive;
	iSyncSemaphoreOne.Close();
	iSyncSemaphoreTwo.Close();
	iEntries.ResetAndDestroy();
	}
	
void CMultiThreadTestApp::ConstructL(CAppActive* aAO)
	{
	TInt error = iSyncSemaphoreOne.CreateGlobal(KSyncSemaphoreOne(), 0);
	if (error == KErrAlreadyExists || error == KErrNone)
		{
		iSyncSemaphoreOne.OpenGlobal(KSyncSemaphoreOne());
		}
	else
		{
		User::Leave(error);
		}
		
	error = iSyncSemaphoreTwo.CreateGlobal(KSyncSemaphoreTwo(), 0);
	if (error == KErrAlreadyExists || error == KErrNone)
		{
		iSyncSemaphoreTwo.OpenGlobal(KSyncSemaphoreTwo());
		}
	else
		{
		User::Leave(error);
		}		
	iActive = aAO;
	}

	
void CMultiThreadTestApp::Progress(TInt /*aPercentageCompleted*/)
	{
	//Due to fix@DEF129938 Completed() is also called in case an error occurs.
	//So not going to do anything here.
	}
	
void CMultiThreadTestApp::Completed(TInt aError)
	{
	iStatus = (aError == KErrCancel)?EBuildIndexCancel:EBuildIndexComplete;
	iActive->Start();
	}
	
void CMultiThreadTestApp::Completed()
	{
	CActiveScheduler::Stop();
	iActive->Start();
	}

TBool CMultiThreadTestApp::NotifyProgress()
	{
	return ETrue;
	}
	
void CMultiThreadTestApp::AddAOAndStartL()
	{
	CActiveScheduler::Add(iActive);
	iActive->Start();
	}


CMultiThreadTestApp::TStatus CMultiThreadTestApp::Status()
	{
	return (CMultiThreadTestApp::TStatus)iStatus;
	}

void CMultiThreadTestApp::SetStatus(CMultiThreadTestApp::TStatus aStatus)
	{
	iStatus = aStatus;
	}



void CMultiThreadTestApp::AddEntryL(CCalEntryView& aView, const TInt aNumToAdd)
	{
	// Add Entries for Test
	RTest test(KTestName);
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
		
		RandomText8(uidP);
		
		if( (index %2) == 0 )
			{
			entry = CreateCalEntryL(CCalEntry::ETodo, guid);
			}
		else
			{
			entry = CreateCalEntryL(CCalEntry::EAppt, guid);
			}
	
		TInt err = entriesToStore.Append(entry);
		test(err == KErrNone);
		
		SetEntryStartAndEndTimeL(entry);
				
		RandomText(summary);
		entry->SetSummaryL(summary);
		
		RandomText(location);
		entry->SetLocationL(location);
		
		RandomText(description);
		entry->SetDescriptionL(description);
		}

	TInt entriesStored(0);
	aView.StoreL(entriesToStore, entriesStored); //temp
	test(entriesStored == aNumToAdd);
	test.Close();
	
	CleanupStack::PopAndDestroy(&entriesToStore);
	}

void CMultiThreadTestApp::FetchEntryL(CCalEntryView& aView, RPointerArray<CCalEntry>& aEntries)
	{
	RArray<TCalLocalUid> entryIds;
	CleanupClosePushL(entryIds);
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());

	aView.GetIdsModifiedSinceDateL(minTime, entryIds);
	
	TInt count = entryIds.Count();
	for (TInt ii = 0; ii<count; ++ii)
		{
		CCalEntry* entry = aView.FetchL(entryIds[ii]);
		aEntries.AppendL(entry);
		}
	CleanupStack::PopAndDestroy(&entryIds);
	}
	
void CMultiThreadTestApp::DeleteEntryL(CCalEntryView& aView, CCalEntry::TType aType, TBool aIsSynchronous)
	{
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());

	if(aIsSynchronous)
		{
		RPointerArray<CCalEntry> entries;
		CleanupResetAndDestroyPushL(entries);
		FetchEntryL(aView, entries);
		TInt count = entries.Count();
		for (TInt ii = 0; ii < count; ++ii)
			{
			CCalEntry* entry = entries[ii];
			if(entry->EntryTypeL() == aType)
				{
				aView.DeleteL(*entry);	
				}
			}
		CleanupStack::PopAndDestroy(&entries);
		}
	else
		{
		const CalCommon::TCalTimeRange timeRange(minTime, maxTime);
		CalCommon::TCalViewFilter filter;
		if(aType==CCalEntry::EAppt)
			{
			filter = CalCommon::EIncludeAppts;	
			}
		else
			{
			filter = ~ (CalCommon::EIncludeAll & CalCommon::EIncludeAppts);	
			}
		aView.DeleteL(timeRange, filter, *this);	
		}
	}
	
void CMultiThreadTestApp::SynchEntryL(CCalEntryView& aView)
	{
	FetchEntryL(aView, iEntries);
	if(iEntries.Count()>0)
		{
		_LIT(KExportFile,"Multithread_export");
		RFile outFile;
		TestRegister().CreateTempFileLC(outFile, KExportFile);
		RFileWriteStream writeStream(outFile);
		CleanupClosePushL(writeStream);
		DataExchangeL().ExportAsyncL(KUidVCalendar, writeStream, iEntries, *this);
		CActiveScheduler::Start();
		CleanupStack::PopAndDestroy(2, &outFile); // outFile.Close(), writeStream.Close()
		}
	}
	
// class 	CMultiThreadTestApp1 //
CMultiThreadTestApp1* CMultiThreadTestApp1::NewL()
	{
	CMultiThreadTestApp1* self = new (ELeave) CMultiThreadTestApp1();

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return (self);
	}
	
CMultiThreadTestApp1::~CMultiThreadTestApp1()
	{
	iThread.Close();
	}
	
TInt RunTestThread(TAny* /*aArgs*/)
	{//This is called by main thread. The second thread is created
	TInt err;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler;
	
	scheduler = new CActiveScheduler;
	
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);

		CMultiThreadTestApp2* app2 = NULL;
		TRAP(err, app2 = CMultiThreadTestApp2::NewL());
		if (err == KErrNone)
			{
			TRAP(err, app2->AddAOAndStartL());
			CActiveScheduler::Start();
			}

		delete app2;
		delete scheduler;
		delete trapCleanup;
		}
		
	return err;
	}
	
void CMultiThreadTestApp1::ConstructL()
	{
	CAppActive* ao = new (ELeave) CAppActive1();
	ao->SetApp(this);
	BaseConstructL();
	CMultiThreadTestApp::ConstructL(ao);
	}

void CMultiThreadTestApp1::Close()
	{
	iSyncSemaphoreOne.Wait();
	CActiveScheduler::Stop();
	}
	
void CMultiThreadTestApp1::GetThreadL()
	{
	User::LeaveIfError(iThread.Create(KThreadName(), RunTestThread, KDefaultStackSize, KMinHeapSize, KMaxHeapSize, NULL, EOwnerProcess));
	iThread.Resume();
	}

void CMultiThreadTestApp1::SynchWithOtherThread()
	{
	iSyncSemaphoreTwo.Wait();
	iSyncSemaphoreOne.Signal();			
	}
	
// class 	CMultiThreadTestApp2 //
void CMultiThreadTestApp2::Close()
	{
	 // signals main thread it has completed
	iSyncSemaphoreOne.Signal();
	CActiveScheduler::Stop();
	}
	
void CMultiThreadTestApp2::GetThreadL()
	{
	iThread.Duplicate(RThread()); // get a handle to the current thread	}
	}
	
CMultiThreadTestApp2* CMultiThreadTestApp2::NewL()
	{
	CMultiThreadTestApp2* self = new (ELeave) CMultiThreadTestApp2();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return (self);
	}

void CMultiThreadTestApp2::ConstructL()
	{
	CAppActive* ao = new (ELeave) CAppActive2();
	ao->SetApp(this);
	BaseConstructL();
	CMultiThreadTestApp::ConstructL(ao);
	GetThreadL();
	}
	
void CMultiThreadTestApp2::SynchWithOtherThread()
	{
	iSyncSemaphoreTwo.Signal();
	iSyncSemaphoreOne.Wait();
	}
	
// class CAppActive //

CAppActive::CAppActive()
	: CActive(EPriorityHigh)
	{
	iStep = EStepCreateView;
	}

CAppActive::~CAppActive()
	{
	Cancel();
	delete iEntryView;
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
	}
	
void CAppActive::SetApp(CMultiThreadTestApp* aApp)
	{
	iApp = aApp;
	}
// class ~CAppActive1 //
CAppActive1::~CAppActive1()
	{
	}
	
void CAppActive1::RunL()
	{
	switch (iStep)
		{
		case EStepCreateView:
		//Spet 1 Open an empty file
			iApp->ReplaceFileL(KTestFile);
			iApp->GetSession().OpenL(KTestFile);
			iStep = EStepAddEntryAndReopenfile;
			iEntryView = CCalEntryView::NewL(iApp->GetSession(), *iApp);
			break;
		case EStepAddEntryAndReopenfile:
		// Spet 2
		//1. Add some entries
		//2. Reopen the file, which result in the file opened currently is closed and indexes are rebuilt
		//3. Create and run the second thread
			iApp->AddEntryL(*iEntryView, KNumberOfEntriesToAdd);
			delete iEntryView;
			iEntryView = NULL;
			iApp->OpenFileL(KTestFile);//It will close the file and delete the index
			iApp->GetThreadL();
			//No break so that it goes the next step
		case EStepStartBuildIndex:
		//Spet 3
		//Create a view, which kick of index building
			iCount++;
			
			if(iCancelBuildIndex)
				{
				test(iApp->Status()==CMultiThreadTestApp::EBuildIndexCancel);
				}
			else
				{
				test(iApp->Status()==CMultiThreadTestApp::EBuildIndexComplete);
				}
			if(!iEntryView)	
				{
				if(iCount<30)
					{//Test asynchronous view building
					iEntryView = CCalEntryView::NewL(iApp->GetSession(), *iApp);
					}
				else
					{//Test synchronous view building
					iEntryView = CCalEntryView::NewL(iApp->GetSession());
					iApp->SetStatus(CMultiThreadTestApp::EBuildIndexComplete);
					}	
				}
				
			iStep = EStepDeleteView;
			Start();
			break;
		case EStepDeleteView:
		// Step 4
		//Delete the view which result in cancel building index
		// iStep is set back to step 3 so that indexes are built and cancelled repeatedly
			iStep = iCount>30?EStepMutipleOperation1:EStepStartBuildIndex;
			if(iCount<30)
				{
				delete iEntryView;
				iEntryView=NULL;
				iCancelBuildIndex = ETrue;	
				}
			else
				{//If the entry view is created synchronously, it can not be cancelled
				iCancelBuildIndex = EFalse;
				Start();	
				}
			break;
		case EStepMutipleOperation1:
			{
			// wait untill the second thread is ready to make sure operations are carried spontaneously
			// Synch Point 1
			iApp->SynchWithOtherThread();
			iApp->AddEntryL(*iEntryView, KNumberOfEntriesToAdd);
			// Synch Point 2
			iApp->SynchWithOtherThread();
			RPointerArray<CCalEntry> entries;
			CleanupResetAndDestroyPushL(entries);
			iApp->FetchEntryL(*iEntryView, entries);
			CleanupStack::PopAndDestroy(&entries);
			// Synch Point 3
			iApp->SynchWithOtherThread();
			iApp->DeleteEntryL(*iEntryView, CCalEntry::EAppt, EFalse);
			iStep = EStepMutipleOperation2;
			break;
			}
		case EStepMutipleOperation2:
		// Synch Point 4
			iApp->SynchWithOtherThread();
			iApp->DeleteEntryL(*iEntryView, CCalEntry::ETodo, ETrue);
			iStep = EEnd;
			Start();

		case EEnd:
		//Step 5 wait for the other thread before it finishes
			iApp->Close();
			break;
		default:
			User::Leave(KErrNotFound);
		}
	}

// class CAppActive2 //
CAppActive2::~CAppActive2()
	{
	delete iInstanceView;
	}

void CAppActive2::RunL()
	{
	RTest test(KTestName);
	iCount++;
	switch (iStep)
		{
		case EStepCreateView:
			{
		// Step 1
		// 1. Open the existing file which is created by the main thread
		// 2. Create a view which result in building indexes
		// 3. For the first few times, iStep is set to step 2 so that the building index process is started and cancelled repeatedly.
		// Later on, the iStep is set to step 3 and wait for the index building is completed.
			if(iCancelBuildIndex)
				{
				test(iApp->Status()==CMultiThreadTestApp2::EBuildIndexCancel);
				}
			iApp->OpenFileL(KTestFile);
			iApp->GetSession().OpenL(KTestFile);
			iStep = EStepMutipleOperation1;
			iInstanceView = CCalInstanceView::NewL(iApp->GetSession(), *iApp);
			if(iCount<50)
				{//Test that both threads build index and cancel it spotenously
				iStep = EStepDeleteView;			
				Start();
				}
			}
			break;
		case EStepDeleteView:
			{
		// Step 2
		// Delete the view which result in cancelling the index building
			iCancelBuildIndex = ETrue;
			delete iInstanceView;
			iInstanceView=NULL;
			iStep = EStepCreateView;
			}
			break;
		case EStepMutipleOperation1:
			{
			//Step 3
			//The other thread cancel the building index request which shouldn't affect the server to build index for this thread
			// Test the index is built and it is able to use it to find instances.

			test(iApp->Status()==CMultiThreadTestApp2::EBuildIndexComplete);
			RPointerArray<CCalInstance>instArray;
			CleanupResetAndDestroyPushL(instArray);
			TCalTime calStartTime;
			TCalTime calEndTime;
			calStartTime.SetTimeLocalL(TCalTime::MinTime());
			calEndTime.SetTimeLocalL(TCalTime::MaxTime());
			CalCommon::TCalTimeRange timeRange(calStartTime, calEndTime);
			iInstanceView->FindInstanceL(instArray, CalCommon::EIncludeAll, timeRange);
			test(instArray.Count() == KNumberOfEntriesToAdd); 
			CleanupStack::PopAndDestroy(&instArray); 
			
			// Wait for the first thread is ready to make sure operations are carried spontaneously
			// (Synch Point 1)
			iApp->SynchWithOtherThread();
			iEntryView = CCalEntryView::NewL(iApp->GetSession());
			iApp->AddEntryL(*iEntryView, KNumberOfEntriesToAdd);
			
			// (Synch Point 2)
			iApp->SynchWithOtherThread();
			RPointerArray<CCalEntry> entries;
			CleanupResetAndDestroyPushL(entries);
			iApp->FetchEntryL(*iEntryView, entries);
			CleanupStack::PopAndDestroy(&entries);
			
			// (Synch Point 3)
			iApp->SynchWithOtherThread();
			TRAPD(err,iApp->AddEntryL(*iEntryView,KNumberOfEntriesToAdd));	
			test.Printf(_L("Synch Point 3 err is: %d\n"),err);
			test(err==KErrLocked || err == KErrNone);
			// (Synch Point 4)
			iApp->SynchWithOtherThread();
			iApp->SetStatus(CMultiThreadTestApp::ESynchentry);
			iApp->SynchEntryL(*iEntryView);
			iStep = EEnd;
			break;
			}
		case EEnd:
			{
			iApp->Close();
			}
			break;
		default:
			User::Leave(KErrNotFound);
		}
	test.Close();
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	//Start the main thread. The second thread will be created and run in
	// CAppActive1::RunL()  when iApp->ResumeThreadL(); is called
	CMultiThreadTestApp1* testManager = CMultiThreadTestApp1::NewL();
	CleanupStack::PushL(testManager);

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

@SYMTestCaseID     PIM-TCAL-MULTI-THREADS-ACCESS-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-MULTI-THREADS-ACCESS-0001 Calendar Interim API Multi Thread Access test suite"));

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

