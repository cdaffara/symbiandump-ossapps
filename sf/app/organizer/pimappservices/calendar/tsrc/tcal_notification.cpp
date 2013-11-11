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
#include <e32test.h>
#include <calentryview.h>
#include <calprogresscallback.h>
#include <calsession.h>

LOCAL_D RTest test(_L("tcal_notification"));

_LIT8(KEntryGuid, "A_DUMMY_GUID");
_LIT(KCalendarFile1, "tcal_notification1");
_LIT(KCalendarFile2, "tcal_notification2");
_LIT(KSemName, "TCAL_NOTIFICATION_SEM");

LOCAL_D const TInt KMaxHeapSize = 0x20000;

class CTestApp : public CBase
	{
public:
	virtual void RunTestL() = 0;
	~CTestApp();

protected:
	CCalTestLibrary* iTestLib;	
	CPeriodic* iTimer;
	};
	
CTestApp::~CTestApp()
	{
	if (iTimer)
		{
		iTimer->Cancel();
		}
	delete iTimer;
	delete iTestLib;
	}
		

class CNotificationTestManager : public CTestApp, public MCalChangeCallBack
	{
public:
	void InitialiseL();
	static TInt StopWaitingForNotification(TAny* );
	void DoStopWaitingForNotification();
	void RunTestL();

	// from MCalChangeCallBack
	void CalChangeNotification(TChangeEntryType aChangeEntryType);

public:
	enum TCallBackState
		{
		ENotStarted = 0,
		EWaitingForAddApptInRange,
		EWaitingForAddApptOutRange,
		EWaitingForAddTodoInRange,
		EWaitingForAddTodoOutRange,
		ENotificationReceived,
		EWaitingToDeleteAppt,
		EWaitingToDeleteTodo,
		};

private:
	TCallBackState	iState;
	};


class CTestAppModifier : public CTestApp
	{
public:
	void RunTestL();
	static TInt RunTestThread(TAny* aArgs);

private:
	void StoreAndDestroyEntryL(CCalEntry* aEntry);
	CCalEntry* CreateEntryL(CCalEntry::TType aType, TTime aTime);
	void ClearEntryL();
	};


//CNotificationTestManager-------------------------------------------------------------------------

TInt CNotificationTestManager::StopWaitingForNotification(TAny* aPtr)
	{
	CNotificationTestManager* manager = static_cast<CNotificationTestManager*>(aPtr);
	
	if (manager)
		{
		manager->DoStopWaitingForNotification();
		}
		
	CActiveScheduler::Stop();
	
	return KErrNone;
	}


void CNotificationTestManager::DoStopWaitingForNotification()
	{
	test.Printf(_L("No notification received\n"));
	
	iTimer->Cancel();
	}


// callback function for change notification
void CNotificationTestManager::CalChangeNotification(TChangeEntryType aChangeEntryType)
	{
	test.Printf(_L("Notification received - type = %d\n"), aChangeEntryType);
	
	switch (iState)
		{
		case EWaitingForAddApptInRange:
		case EWaitingToDeleteAppt:
			__ASSERT_ALWAYS(aChangeEntryType == EChangeEntryEvent, User::Invariant());
			break;
		case EWaitingForAddTodoInRange:
		case EWaitingToDeleteTodo:
			__ASSERT_ALWAYS(aChangeEntryType != EChangeEntryEvent, User::Invariant());
			break;
		case EWaitingForAddApptOutRange:
		case EWaitingForAddTodoOutRange:
			test.Printf(_L("Got call back from an out-of-range entry!!\n"));
			test(0);
			break;
		default:
			break;
		}
		
	iTimer->Cancel();
	iState = ENotificationReceived;
	
	CActiveScheduler::Stop();
	}

	
void CNotificationTestManager::InitialiseL()
	{
	iTestLib = CCalTestLibrary::NewL();
	
	test.Printf(_L("Opening calendar file...\n"));		
	
	iTestLib->ReplaceFileL(KCalendarFile2());	
	iTestLib->OpenFileL(KCalendarFile2());
	
	// force creation of a CEntryView object
	test.Printf(_L("Opening entry view...\n"));
	
	iTestLib->AsynCGetEntryViewL();
	CActiveScheduler::Start();
	}



void CNotificationTestManager::RunTestL()
	{
	TInt err;	
		
	test.Next(_L("Running Notification Test\n"));

	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KCalendarFile1());
	test.Printf(_L("Opening calendar file...\n"));	
	iTestLib->OpenFileL(KCalendarFile1());
	
	// force creation of a CEntryView object
	test.Printf(_L("Opening entry view...\n"));	
	iTestLib->SynCGetEntryViewL();
	
	iTimer = CPeriodic::NewL(CActive::EPriorityStandard);
	const TTimeIntervalMicroSeconds32 KWait1second(1000000); // 1 second
	
	// set up notification (all entries in 2005)
	TTime startTime(TDateTime(2005, EJanuary, 0, 0, 0, 0, 0)); // 1 Jan 05
	TTime endTime(startTime + TTimeIntervalYears(1)); // 1 Jan 06
	iTestLib->GetSession().StartChangeNotification(this, EChangeEntryAll, ETrue, startTime, endTime);

	
	// Create another thread to test the change callback.
	test.Printf(_L("Setting up other thread for modifier object...\n"));
	RThread otherThread;
	TName threadName = _L("TestModifierThread");
	err = otherThread.Create(threadName, CTestAppModifier::RunTestThread, KDefaultStackSize, 
							      KMinHeapSize, KMaxHeapSize, NULL, EOwnerProcess);
	if (err != KErrNone)
		{
		User::Panic(_L("Thread's not been created"), err);
		}

	// Create a semaphore
	TName semName;
	semName.Append(KSemName);

	RSemaphore sem;
	err = sem.CreateGlobal(semName, 0);
	if (err == KErrAlreadyExists)
		{
		sem.OpenGlobal(semName);
		}

	// add appt within time range

	test.Next(_L("Add an appointment within the notification time range"));

	iState = EWaitingForAddApptInRange;
	
	// CNotificationTestManager::RunTestL hands over execution to the function 
	// CTestAppModifier::RunTestL in the other thread for it to make 
	// changes. The manager waits until the modifier has done what it 
	// needs to and then signals the manager.
	// 
	// See "// Sync point 1" comments to trace back and forth between 
	// the two functions.
	
	otherThread.Resume();
	sem.Wait(); // Sync point 1
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState == ENotificationReceived); // check callback notification was received for adding appt

	test.Next(_L("Delete this appointment"));

	iState = EWaitingToDeleteAppt;
	otherThread.Resume();
	sem.Wait(); // Sync point 2
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState == ENotificationReceived); // check callback notification was received for delete
	
	// add appt outside time range

	test.Next(_L("Add an appointment outside the notification time range"));

	iState = EWaitingForAddApptOutRange;
	otherThread.Resume();
	sem.Wait(); // Sync point 3
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState != ENotificationReceived); // check callback notification was NOT received for adding appt

	test.Next(_L("Delete this appointment"));

	iState = EWaitingToDeleteAppt;
	otherThread.Resume();
	sem.Wait(); // Sync point 4
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState != ENotificationReceived); // check callback notification was NOT received for delete
	
	// notification should now be disabled by the other thread

	// add todo within time range

	test.Next(_L("Disable change notification then add a todo within the notification time range"));

	iState = EWaitingForAddTodoInRange;
	otherThread.Resume();
	sem.Wait(); // Sync point 5
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState != ENotificationReceived); // check callback notification was NOT received (disabled notification)

	// notification should now be re-enabled by the other thread

	test.Next(_L("Re-enable change notification"));

	otherThread.Resume();
	sem.Wait(); // Sync point 6
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState == ENotificationReceived); // check callback notification was received for todo added during disable

	test.Next(_L("Delete this todo"));

	iState = EWaitingToDeleteTodo;
	otherThread.Resume();
	sem.Wait(); // Sync point 7
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState == ENotificationReceived); // check callback notification was received for delete
	
	// add todo out of time range

	test.Next(_L("Add a todo entry outside the notification time range"));

	iState = EWaitingForAddTodoOutRange;
	otherThread.Resume();
	sem.Wait(); // Sync point 8
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState != ENotificationReceived); // check callback notification was NOT received for adding todo

	test.Next(_L("Delete this todo"));

	iState = EWaitingToDeleteTodo;
	otherThread.Resume();
	sem.Wait(); // Sync point 9
	iTimer->Start(KWait1second, KWait1second, TCallBack(StopWaitingForNotification, this));
	CActiveScheduler::Start();
	test(iState != ENotificationReceived); // check callback notification was NOT received for delete

	otherThread.Resume(); // allow the thread to tidy up;
	sem.Wait();

	otherThread.Close();
	sem.Close();

	iTestLib->GetSession().StopChangeNotification();
	}



//CTestAppModifier------------------------------------------------------------------------

// this function deletes all entries with the UID used in this test harness
void CTestAppModifier::ClearEntryL()
	{
	CDesC8Array* uidArray = new (ELeave) CDesC8ArrayFlat(1);
	CleanupStack::PushL(uidArray);
	
	uidArray->AppendL(KEntryGuid());
	iTestLib->SynCGetEntryViewL().DeleteL(*uidArray);
	
	CleanupStack::PopAndDestroy(uidArray);
	}

// this function creates an entries with the type and time specified
CCalEntry* CTestAppModifier::CreateEntryL(CCalEntry::TType aType, TTime aTime)
	{
	HBufC8* guid1 = KEntryGuid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(aType, guid1, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid1);
	
	TCalTime calTime1;
	TCalTime calTime2;
	calTime1.SetTimeLocalL(aTime);
	calTime2.SetTimeLocalL(aTime + TTimeIntervalHours(1));
	entry->SetStartAndEndTimeL(calTime1, calTime2);
	
	return entry;
	}
	
// this function stores an entry then deletes it
void CTestAppModifier::StoreAndDestroyEntryL(CCalEntry* aEntry)
	{
	CleanupStack::PushL(aEntry);
	
	RPointerArray<CCalEntry> entriesToAdd;
	CleanupClosePushL(entriesToAdd);
	
	entriesToAdd.AppendL(aEntry);
	
	TInt successfulAdd(0);
	iTestLib->SynCGetEntryViewL().StoreL(entriesToAdd, successfulAdd);
		
	test(successfulAdd == entriesToAdd.Count());
	
	CleanupStack::PopAndDestroy(); // entriesToAdd.Close()
	CleanupStack::PopAndDestroy(aEntry);
	}

TInt CTestAppModifier::RunTestThread(TAny* /*aArgs*/)
	{
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	if ( ! scheduler)
		{
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);

	// Create a semaphore
	TName semName;
	semName.Append(KSemName);

	RSemaphore sem;
	TInt err = sem.CreateGlobal(semName, 0);
	if (err == KErrAlreadyExists)
		{
		sem.OpenGlobal(semName);
		}

	CTestAppModifier* modifier = new CTestAppModifier;
	if ( ! modifier)
		{
		return KErrNoMemory;
		}
	TRAP(err, modifier->RunTestL());

	delete modifier;
	delete scheduler;
	delete trapCleanup;

	sem.Signal();
	sem.Close();
	return err;
	}

void CTestAppModifier::RunTestL()
	{
	RThread thisThread;
	thisThread.Duplicate(RThread()); // get a handle to the current thread


	iTestLib = CCalTestLibrary::NewL();
	iTestLib->OpenFileL(KCalendarFile1());
	
	// force creation of a CEntryView object
	iTestLib->SynCGetEntryViewL();
		
	// Create a semaphore
	TName semName;
	semName.Append(KSemName);

	RSemaphore sem;
	TInt err = sem.CreateGlobal(semName, 0);
	if (err == KErrAlreadyExists)
		{
		sem.OpenGlobal(semName);
		}

	// The function CNotificationTestManager::RunTestL hands over execution 
	// from the other thread to CTestAppModifier::RunTestL for it to 
	// make changes. The manager waits until the modifier has done 
	// what it needs to and then signals the manager.
	// 
	// See "// Sync point 1" comments to trace back and forth between 
	// the two functions.
	
	// Sync point 1
	// add appt within time range
	TTime startTime(TDateTime(2005, EJanuary, 0, 0, 0, 0, 0)); // 1 Jan 05
	TTime endTime(startTime + TTimeIntervalYears(1)); // 1 Jan 06
	CCalEntry* apptInRange = CreateEntryL(CCalEntry::EAppt, startTime + TTimeIntervalDays(5));
	StoreAndDestroyEntryL(apptInRange);
	sem.Signal();
	thisThread.Suspend();	
	
	// Sync point 2
	// delete it
	ClearEntryL();
	sem.Signal();
	thisThread.Suspend();	
	
	// Sync point 3
	// add appt outside time range
	CCalEntry* apptOutOfRange = CreateEntryL(CCalEntry::EAppt, startTime - TTimeIntervalMonths(3));
	StoreAndDestroyEntryL(apptOutOfRange);
	sem.Signal();
	thisThread.Suspend();	

	// Sync point 4
	// delete it
	ClearEntryL();
	sem.Signal();
	thisThread.Suspend();	

	// Sync point 5
	iTestLib->GetSession().DisableChangeBroadcast();

	// add todo within time range
	CCalEntry* disableAppt = CreateEntryL(CCalEntry::ETodo, startTime + TTimeIntervalDays(5));
	StoreAndDestroyEntryL(disableAppt);
	sem.Signal();
	thisThread.Suspend();	

	// Sync point 6
	iTestLib->GetSession().EnableChangeBroadcast();
	sem.Signal();
	thisThread.Suspend();	

	// Sync point 7
	// delete it
	ClearEntryL();
	sem.Signal();
	thisThread.Suspend();	
	
	// Sync point 8
	// add todo out of time range

	CCalEntry* todoOutRange = CreateEntryL(CCalEntry::ETodo, endTime + TTimeIntervalHours(4));
	StoreAndDestroyEntryL(todoOutRange);
	sem.Signal();
	thisThread.Suspend();	

	// Sync point 9
	// delete it
	ClearEntryL();
	sem.Signal();
	thisThread.Suspend();	

	sem.Close();
	}
	
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

static void DoTestL()
	{
	CNotificationTestManager* testManager = new (ELeave) CNotificationTestManager();
	CleanupStack::PushL(testManager);
	
	
	TPerformanceTimer timer(test);
	timer.Start();
	
	
	// Run the test suite	

	// initialise the calendar file
	testManager->RunTestL();


	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();
	
	
	CleanupStack::PopAndDestroy(testManager);
	
	
	//Test code added for DEF063285
	test.Next(_L("To initialise CalenderProgressObserver,Entryview,CalSession..."));

	test.Next(_L("...and destroy entryview before it is built completely..."));

	test.Next(_L("...to check any asynchronous operation is cancelled"));

	
	testManager = new (ELeave) CNotificationTestManager();
	CleanupStack::PushL(testManager);
	
	//Intialise CalenderProgressObserver,Entryview,CalSession
	TRAPD(err, testManager->InitialiseL());
	test(err == KErrNone);
	
	//After initialising try to destroy CalenderProgressObserver which contains entryview
	//and session to check any asynchronous operation is cancelled if the entryview is not built completely
	CleanupStack::PopAndDestroy(testManager);
	}
	
	
/**

@SYMTestCaseID     PIM-TCAL-NOTIFICATION-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-NOTIFICATION-0001 Calendar Interim API Notification test suite"));

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

