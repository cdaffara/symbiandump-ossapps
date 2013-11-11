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
#include <calnotification.h>
#include <calentryview.h>
#include <calsession.h>
#include <calprogresscallback.h>
#include <e32test.h>
#include <e32property.h>

LOCAL_D RTest test(_L("tcal_pubsubnotification"));

_LIT(KCalendarFileName, "c:TCal_PubSubNotification");


class CPubSubObserver : public CActive
	{
public:
	static CPubSubObserver* NewL(TInt aWatchValue);
	~CPubSubObserver();
	
	void Start();
	TBool WasCalled() const;
	TCalPubSubData GetDataL() const;
	
	// from CActive
	void RunL();
	void DoCancel();
	
	
private:
	// avoid any method but NewL instancing the class

	CPubSubObserver();
	
	// no copy constructor and assignment operator
	CPubSubObserver(CPubSubObserver& );
	CPubSubObserver& operator = (const CPubSubObserver& );	

	void ConstructL(TInt aWatchValue);


private:
	RProperty iProperty;
	TBool iWasCalled;
	TInt iWatchValue;
	};


CPubSubObserver* CPubSubObserver::NewL(TInt aWatchValue)
	{
	CPubSubObserver* self = new (ELeave) CPubSubObserver;
	
	CleanupStack::PushL(self);
	self->ConstructL(aWatchValue);
	CleanupStack::Pop(self);
	
	return (self);
	}

	
CPubSubObserver::CPubSubObserver() : CActive(EPriorityLow)
	{
	CActiveScheduler::Add(this);
	}


CPubSubObserver::~CPubSubObserver()
	{
	Cancel();
	}


void CPubSubObserver::ConstructL(TInt aWatchValue)
	{
	iWatchValue = aWatchValue;
	User::LeaveIfError(iProperty.Attach(KCalPubSubCategory, aWatchValue));
	}


void CPubSubObserver::Start()
	{
	if (!IsActive())
		{
		iProperty.Subscribe(iStatus);
		SetActive();
		}
	iWasCalled = EFalse;
	}

	
void CPubSubObserver::RunL()
	{
	iWasCalled = ETrue;
	test.Printf(_L("Got publish and subscribe callback\n"));

	CActiveScheduler::Stop();
	}

	
void CPubSubObserver::DoCancel()
	{
	iProperty.Cancel();
	}


TBool CPubSubObserver::WasCalled() const
	{
	return iWasCalled;
	}


TCalPubSubData CPubSubObserver::GetDataL() const
	{
	TPckgBuf<TCalPubSubData> psBuf;
	User::LeaveIfError(iProperty.Get(KCalPubSubCategory, iWatchValue, psBuf));
	return psBuf();
	}


//


class CPubSubNotificationTestManager : public CBase
	{
public:
	static CPubSubNotificationTestManager* NewLC();
	~CPubSubNotificationTestManager();
	void OpenFileL();

	void TestNegativeL();
	void TestStoreLotEntriesL();
	void TestTodosL();
	void TestAppointmentsL();
	void TestRemindersL();
	void TestEventsL();
	void TestAnniversariesL();

private:

	// avoid any method but NewL instancing the class

	CPubSubNotificationTestManager() { }
	
	// no copy constructor and assignment operator
	CPubSubNotificationTestManager(CPubSubNotificationTestManager& );
	CPubSubNotificationTestManager& operator = (const CPubSubNotificationTestManager& );	
	
	void ConstructL();
	
	void RunTestL(CPubSubObserver* aObserver, CCalEntry::TType aType);
	void CheckThisObserverFired(CPubSubObserver* aObserver);
	void CheckNoObserversFiredL();	
	void CheckPSDataL(CPubSubObserver* aObserver);
	void PrepareForTest();
	

private:
	CCalTestLibrary* iTestLib;
	CPubSubObserver* iPubSubTodoObserver;
	CPubSubObserver* iPubSubEventObserver;
	TTime            iTestStartTime;
	};

// Constructor/Destructor

CPubSubNotificationTestManager* CPubSubNotificationTestManager::NewLC()
	{
	CPubSubNotificationTestManager* self = new (ELeave) CPubSubNotificationTestManager();

	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}


void CPubSubNotificationTestManager::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	}
	
void CPubSubNotificationTestManager::OpenFileL()
	{
	test.Printf(_L("Opening calendar file...\n"));	
	
	iTestLib->ReplaceFileL(KCalendarFileName());
	iTestLib->OpenFileL(KCalendarFileName());
	
	iPubSubTodoObserver = CPubSubObserver::NewL(ECalPubSubTodoNotification);
	iPubSubEventObserver = CPubSubObserver::NewL(ECalPubSubEventNotification);
	}

	
CPubSubNotificationTestManager::~CPubSubNotificationTestManager()
	{
	delete iPubSubTodoObserver;
	delete iPubSubEventObserver;
	delete iTestLib;
	}


static	TInt CancelWait(TAny* )
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}		


void CPubSubNotificationTestManager::CheckNoObserversFiredL()
	{		
	test.Printf(_L("Waiting for callback\n"));
	
	CPeriodic* cancelTimer = CPeriodic::NewL(EPriorityHigh);
	CleanupStack::PushL(cancelTimer);
	
	TTimeIntervalMicroSeconds32 cancelWait = 500000;
	cancelTimer->Start(cancelWait, cancelWait, TCallBack(CancelWait));
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(cancelTimer);
	
	test(!iPubSubTodoObserver->WasCalled());
	test(!iPubSubEventObserver->WasCalled());
	}

	
void CPubSubNotificationTestManager::CheckThisObserverFired(CPubSubObserver* aObserver)
	{
	test(aObserver == iPubSubTodoObserver || aObserver == iPubSubEventObserver);
	
	test.Printf(_L("Waiting for callback\n"));
	CActiveScheduler::Start();
	
	test(aObserver->WasCalled());
	if (aObserver == iPubSubTodoObserver)
		{
		test(!iPubSubEventObserver->WasCalled());
		}
	else
		{
		test(!iPubSubTodoObserver->WasCalled());
		}
	}


void CPubSubNotificationTestManager::CheckPSDataL(CPubSubObserver* aObserver)
	{
	TCalPubSubData psData = aObserver->GetDataL();
	TTimeIntervalSeconds seconds;
	psData.iTimeOfChangeUtc.SecondsFrom(iTestStartTime, seconds);
	
	// check notification happened within 2 seconds
	test(seconds.Int() >= 0);
	test(seconds.Int() < 2);
	
	TFileName* fileName = new(ELeave) TFileName;
	CleanupStack::PushL(fileName);
	
	iTestLib->GetSession().GetFileNameL(psData, *fileName);
	test(iTestLib->GetSession().IsFileNameL(psData, *fileName));
	test(iTestLib->GetSession().IsOpenedFileL(psData));
	test(fileName->CompareF(KCalendarFileName()) == 0);
	
	CleanupStack::PopAndDestroy(fileName);
	}


void CPubSubNotificationTestManager::PrepareForTest()
	{
	iPubSubTodoObserver->Start();
	iPubSubEventObserver->Start();
	iTestStartTime.UniversalTime();
	}


void CPubSubNotificationTestManager::RunTestL(CPubSubObserver* aObserver, CCalEntry::TType aType)
	{
	RPointerArray<CCalEntry> entriesToAdd;
	CleanupClosePushL(entriesToAdd);
		
	test.Printf(_L("Create new entry\n"));
	_LIT8(KUid, "DUMMY UID 1234567890");
	HBufC8* uid = KUid().AllocLC();	
	CCalEntry* entry = iTestLib->CreateCalEntryL(aType, uid);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	CCalTestLibrary::SetEntryStartAndEndTimeL(entry);
	
	
	test.Printf(_L("Store entry\n"));
	PrepareForTest();
	CCalEntryView* entryView = &iTestLib->SynCGetEntryViewL();
	entriesToAdd.AppendL(entry);
	TInt success(0);
	entryView->StoreL(entriesToAdd, success);
	CheckThisObserverFired(aObserver);
	CheckPSDataL(aObserver);
		
	test.Printf(_L("Update entry\n"));
	PrepareForTest();
	entriesToAdd[0]->SetSummaryL(_L("dummy summary"));
	entryView->UpdateL(entriesToAdd, success);
	CheckThisObserverFired(aObserver);
	CheckPSDataL(aObserver);
	
	
	test.Printf(_L("Delete entry asynchronously\n"));
	PrepareForTest();
	TCalTime aStartTime;
	TCalTime aEndTime;
	aStartTime.SetTimeLocalL(TCalTime::MinTime());
	aEndTime.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange aTimeRange(aStartTime, aEndTime);
	entryView->DeleteL(aTimeRange, CalCommon::EIncludeAll, *iTestLib);
	
	
	// wait for delete operation to complete
	CActiveScheduler::Start();
	CheckThisObserverFired(aObserver);
	CheckPSDataL(aObserver);


	entriesToAdd.Reset();
	CleanupStack::PopAndDestroy(entry);
	
	uid = _L8("DUMMY UID 1234567890").AllocLC();
	entry = iTestLib->CreateCalEntryL(aType, uid);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entry);
	CCalTestLibrary::SetEntryStartAndEndTimeL(entry);
	
	
	iTestLib->GetSession().DisablePubSubNotificationsL();
	test.Printf(_L("Disable pub-sub test\n"));
	PrepareForTest();
	entriesToAdd.AppendL(entry);
	entryView->StoreL(entriesToAdd, success);	
	CheckNoObserversFiredL();
	
	
	iTestLib->GetSession().EnablePubSubNotificationsL();
	test.Printf(_L("Delete entry\n"));
	PrepareForTest();
	entryView->DeleteL(*entry);
	CheckThisObserverFired(aObserver);
	CheckPSDataL(aObserver);
	
 		
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(); //entriesToAdd.Reset();	
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Test cases forming the Pub Sub notification test suite
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	

void CPubSubNotificationTestManager::TestTodosL()
	{
	test.Next(_L("Test Todos"));

	RunTestL(iPubSubTodoObserver,  CCalEntry::ETodo);
	}


void CPubSubNotificationTestManager::TestAppointmentsL()
	{
	test.Next(_L("Test Appointments"));

	RunTestL(iPubSubEventObserver, CCalEntry::EAppt);
	}


void CPubSubNotificationTestManager::TestRemindersL()
	{
	test.Next(_L("Test Reminders"));

	RunTestL(iPubSubEventObserver, CCalEntry::EReminder);
	}
	

void CPubSubNotificationTestManager::TestEventsL()
	{
	test.Next(_L("Test Events"));

	RunTestL(iPubSubEventObserver, CCalEntry::EEvent);
	}


void CPubSubNotificationTestManager::TestAnniversariesL()
	{
	test.Next(_L("Test Anniversaries"));

	RunTestL(iPubSubEventObserver, CCalEntry::EAnniv);
	}
	
void CPubSubNotificationTestManager::TestStoreLotEntriesL()
	{//Store more than 20 entries. It should have stoped the notifation before entries are stored and  but resumed after all entries are stored.
	//Add this test to ensure that it has not stoped the notication for the rest test cases.
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	for(TInt ii=0;ii<30;++ii)
		{
		HBufC8* dummy = NULL;
		CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, dummy);
		CleanupStack::PushL(entry);
		entries.AppendL(entry);
		CleanupStack::Pop(entry);
		CCalTestLibrary::SetEntryStartAndEndTimeL(entry);
		}
	TInt success;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);	
	CheckNoObserversFiredL();
	CleanupStack::PopAndDestroy(&entries);
	}
	
void CPubSubNotificationTestManager::TestNegativeL()
	{//Call those APIs without creating\opening a calendar file
	iTestLib->GetSession().DisablePubSubNotificationsL();
	iTestLib->GetSession().EnablePubSubNotificationsL();
	iTestLib->GetSession().DisableChangeBroadcast();
	iTestLib->GetSession().EnableChangeBroadcast();
	TCalFileId id;
	
	iTestLib->GetSession().FileIdL(id);
	test(id == 0);
	TCalPubSubData pubsub = { 0, 0 };
	TBuf<10> filename;
	TInt err;
	TRAP(err, iTestLib->GetSession().GetFileNameL(pubsub, filename));
	test(filename ==  KNullDesC());
	_LIT(KFileName,"abc");
	test( EFalse == iTestLib->GetSession().IsFileNameL(pubsub, KFileName));
	
	TRAP(err, iTestLib->GetSession().IsOpenedFileL(pubsub)); 
	test(err == KErrArgument);
	}
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CPubSubNotificationTestManager* testManager = CPubSubNotificationTestManager::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();
	
	testManager->TestNegativeL();

	// Run the test suite
	testManager->OpenFileL();
	testManager->TestTodosL();
	testManager->TestAppointmentsL();
	testManager->TestRemindersL();
	testManager->TestEventsL();
	testManager->TestAnniversariesL();
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-PUBSUBNOTIFICATION-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-PUBSUBNOTIFICATION-0001 Calendar Interim API Publish and Subscribe test suite"));

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

