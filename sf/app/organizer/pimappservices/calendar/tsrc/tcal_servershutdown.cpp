// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// tcal_servershutdown:
// This test is intended to test the delay timer added to the calendar file before it is closed
// 
//

#include "tcal_servershutdown.h"
#include "calinstanceview.h"
#include "calinstance.h" 
#include "calrrule.h" 
#include <caliterator.h>
#include "calalarm.h" 


CServerShutdownTestManager::CServerShutdownTestManager(TInt aPriority):CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	}

// Record a timestamp before initiating server shutdown
// Server shutdown period is checked against the expected range when the process exits
void CServerShutdownTestManager::RecordTimeStampBeforeServerShutdown()
	{
	iSrvTimerStartTicks = User::NTickCount();
	}

// Calculate server shutdown period and test that it is within the expected range
void CServerShutdownTestManager::TestServerShutdownTime()
	{
	TUint32 srvTimerStopTicks = User::NTickCount();
	TUint32 srvTimerShutdownPeriodTicks = srvTimerStopTicks - iSrvTimerStartTicks;
	TTimeIntervalMicroSeconds32 srvTimerShutdownPeriod = srvTimerShutdownPeriodTicks * iNanoTickPeriod;
	_LIT(KAgendaServerShutdownPeriod, "Agenda server shutdown period - %d microseconds\n");
	test.Printf(KAgendaServerShutdownPeriod, srvTimerShutdownPeriod);
	test(srvTimerShutdownPeriod >= KServerShutdownDelayMin);
	test(srvTimerShutdownPeriod <= KServerShutdownDelayMax);
	}

// Entry view progress callback
void CServerShutdownTestManager::Progress(TInt /*aPercentageCompleted*/)
	{
	_LIT(KDot, ".");
	test.Printf(KDot);
		
	_LIT(KUnexpectedCallback, "The index should already be built so we should not get progress callbacks\n");
	
	switch (iTestCase)
		{
		case ETestServerShutdownCancelWithIndexCancelled:
		case ETestServerShutdownCancelWithIndexShutdown:
			{
			test.Printf(KUnexpectedCallback);
			test(EFalse);
			}
			break;
		case ETestServerSingleSessionOpenTwiceShutdown:
		case ETestServerMultiSessionSingleFileOpen:
			{
			test.Printf(KUnexpectedCallback);
			test(EFalse);
			}
		break;
		default:
			{
			// do nothing
			}
			break;
		}
	}

// Entry view complete callback
void CServerShutdownTestManager::Completed(TInt aError)
	{
	_LIT(KEntryViewCompletedWithErr, "Entry View open complete with error = %d\n");
	test.Printf(KEntryViewCompletedWithErr, aError);
	
	switch(iTestCase)
		{
		case ETestServerShutdownOpenInvalidFileLeave:
			{
			test(aError == KErrNotFound);
			}
		break;
		case ETestServerShutdownOOMDeleteFile:
		case ETestServerShutdownOOMOpenCloseFile:
			{
			// OOM test will expectedly fail with KErrNoMemory
			test(aError == KErrNoMemory);
			}
		break;
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
		case ETestServerShutdownWithSystemStateNotificationAsync:
			{
			test(aError == KErrNone || aError == KErrCancel);
			}
		break;
#endif
		default:
			{
			test(aError == KErrNone);
			}
		break;
		}
	
	CActiveScheduler::Stop();
 	}

TBool CServerShutdownTestManager::NotifyProgress()
	{
	return ETrue;
	}

// The RunL is called when the agenda server process exits as a result of the logon to the process
// The process can exit in unexpected test cases' states and will result in an error
// If the process exits in an expected the test cases' state, the server shutdown period is calculated and checked against the expected time range
// There are other test cases' states where we expect the server to shutdown immediately e.g. after deleting a file, hence we do nothing
void CServerShutdownTestManager::RunL()
	{
	_LIT(KAgendaServerClosedWithStatus, "The agenda server has closed with iStatus = %d\n");
	_LIT(KAgendaServerUnexpectedClose, "The server closed down when it shouldn't have\n");
	_LIT(KInvalidTestCaseVariable, "The test case state variable was not set or was set to an invalid value\n");
	
	test.Printf(KAgendaServerClosedWithStatus, iStatus.Int());
	
	__ASSERT_ALWAYS(iStatus.Int() == KErrNone, User::Invariant());

	switch (iTestCase)
		{
		case ETestServerShutdownWithIndex:
			{
			// Calculate server shutdown period and test that it is within the expected range
			TestServerShutdownTime();
			}
		break;
		case ETestServerShutdownCancelWithIndexStart:
		case ETestServerShutdownCancelWithIndexCancelled:
			{
			// The server should not have shut down here
			test.Printf(KAgendaServerUnexpectedClose);
			test(EFalse);
			}
			break;
		case ETestServerShutdownCancelWithIndexShutdown:
			{
			// Calculate server shutdown period and test that it is within the expected range
			TestServerShutdownTime();
			}
			break;
		case ETestServerShutdownDeleteFile:
			{
			// Expecting the agenda server to shutdown immediately
			}
			break;
		case ETestServerShutdownDeleteMultiFiles:
			{
			// Calculate server shutdown period and test that it is within the expected range
			TestServerShutdownTime();
			}
			break;
		case ETestServerShutdownProcessExit:
			{
			// Expecting the agenda server to shutdown immediately
			}
			break;
		case ETestServerSingleSessionOpenTwiceStart:
			{
			// the server should not have shut down here since opening the file the second time should cancel the delay timer
			test.Printf(KAgendaServerUnexpectedClose);
			test(EFalse);
			}
			break;
		case ETestServerSingleSessionOpenTwiceShutdown:
			{
			// Calculate server shutdown period and test that it is within the expected range
			TestServerShutdownTime();
			}
			break;
		case ETestServerMultiSessionSingleFileOpenStart:
			{
			// the server should not have shut down here when a session out of multiple file sessions to multiple files is closed
			test.Printf(KAgendaServerUnexpectedClose);
			test(EFalse);
			}
			break;
		case ETestServerMultiSessionSingleFileOpenShutdown:
			{
			// Calculate server shutdown period and test that it is within the expected range
			TestServerShutdownTime();
			}
			break;
		case ETestServerMultiSessionMultiFileOpenStart:
			{
			// the server should not have shut down here when the last session to a calendar file is closed but there are other files open
			test.Printf(KAgendaServerUnexpectedClose);
			test(EFalse);
			}
			break;
		case ETestServerMultiSessionMultiFileOpenShutdown:
			{
			// Calculate server shutdown period and test that it is within the expected range
			TestServerShutdownTime();
			}
			break;				
		case ETestServerShutdownOpenInvalidFileLeave:
			{
			// Expecting the agenda server to shutdown immediately (not with a delay)
			}
			break;			
		case ETestServerShutdownv91CalendarFileConversion:
			{
			// Calculate server shutdown period and test that it is within the expected range
			TestServerShutdownTime();
			}
			break;
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
		case ETestServerShutdownWithSystemStateNotification:
		
			{
			TestSystemAckNotification();
			test (iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1);
			ClearAllAlarmsL();
			}
			break;
		case ETestServerShutdownWithSystemStateNotificationAsync:	
		case ETestServerShutdownWithSystemStateNotificationNormal:	
		case ETestServerShutdownWithSystemStateNotificationInstance:
			{
			TestSystemAckNotification();
			}
			break;
#endif
		default:
			{
			// The test case state variable was not set or was set to an invalid value
			test.Printf(KInvalidTestCaseVariable);
			test(EFalse);
			}
			break;
		};

	// Close the handle to the agenda server process
	iProcess.Close();
	// Stop the active scheduler since the agenda server process has now exited
	CActiveScheduler::Stop();
	}

void CServerShutdownTestManager::DoCancel()
	{
	iProcess.LogonCancel(iStatus);
	}

// Trigger an asynchronous delay which allows the server to shutdown and flag an error if it was unexpected
void CServerShutdownTestManager::AsyncDelay(TTimeIntervalMicroSeconds32 aMicroseconds)
	{
	_LIT(KAsyncDelayStart, "Async delay for %d microseconds is starting...\n");
	
	test.Printf(KAsyncDelayStart, aMicroseconds.Int());
	TCallBack callBack(CServerShutdownTestManager::AsyncDelayCallBack, this);
	iPeriodic->Start(aMicroseconds, aMicroseconds, callBack);
	CActiveScheduler::Start();
	}

// Called when the asynchronous delay expires
TInt CServerShutdownTestManager::AsyncDelayCallBack(TAny* aPtr)
	{
	_LIT(KAsyncDelayFinish, "...the delay has finished\n");
	test.Printf(KAsyncDelayFinish);
	
	static_cast<CServerShutdownTestManager*>(aPtr)->iPeriodic->Cancel();
	CActiveScheduler::Stop();
	return KErrNone;
	}

// Create a session to the agenda server
void CServerShutdownTestManager::CreateSessionL()
	{
	_LIT(KCreatingCalSession, "Creating a session\n");
	test.Printf(KCreatingCalSession);
	
	delete iSession;
	iSession = NULL;
	iSession = CCalSession::NewL();
	}

// Create an additional session to the agenda server
void CServerShutdownTestManager::CreateAdditionalSessionL()
	{
	_LIT(KCreatingCalAdditionalSession, "Creating an additional session\n");
	test.Printf(KCreatingCalAdditionalSession);
	
	delete iAdditionalSession;
	iAdditionalSession = NULL;
	iAdditionalSession = CCalSession::NewL();
	}

// Close the session and wait / do not wait for the agenda server to close
void CServerShutdownTestManager::CloseSession(TBool aWaitToClose)
	{
	_LIT(KClosingCalSession, "Closing the session\n");
	test.Printf(KClosingCalSession);
	
	delete iSession;
	iSession = NULL;
	
	if (aWaitToClose)
		{
		// Make sure any ecom sessions are destroyed before waiting for the agenda server to close
		REComSession::FinalClose();
		_LIT(KWaitingForAgendaServerClose, "Wait for the agenda server to close...\n");
		test.Printf(KWaitingForAgendaServerClose);
		CActiveScheduler::Start();
		}
	}

// Close the additional session to the agenda server
void CServerShutdownTestManager::CloseAdditionalSession(TBool aWaitToClose)
	{
	_LIT(KClosingAdditionalCalSession, "Closing the additional session\n");
	test.Printf(KClosingAdditionalCalSession);
	
	delete iAdditionalSession;
	iAdditionalSession = NULL;
	
	if (aWaitToClose)
		{
		// Make sure any ecom sessions are destroyed before waiting for the agenda server to close
		REComSession::FinalClose();
		_LIT(KWaitingForAgendaServerClose, "Wait for the agenda server to close...\n");
		test.Printf(KWaitingForAgendaServerClose);
		CActiveScheduler::Start();
		}
	}

// Create a calendar entry view 
void CServerShutdownTestManager::CreateEntryViewL(const TDesC& aCalFile, TBool aWaitToOpen)
	{
	_LIT(KCreatingCalEntryView, "Creating the Entry View\n");
	test.Printf(KCreatingCalEntryView);
		
	iSession->OpenL(aCalFile);

	iEntryView = CCalEntryView::NewL(*iSession, *this);
	
	if (aWaitToOpen)
		{
		CActiveScheduler::Start();
		}
	}
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
void CServerShutdownTestManager::CreateInstanceViewL(TBool aWaitToOpen)
	{
	_LIT(KCreatingCalInstanceView, "Creating an Instance View\n");
	test.Printf(KCreatingCalInstanceView);
	
	iInstanceView = CCalInstanceView::NewL(*iSession, *this);
	
	if (aWaitToOpen)
		{
		CActiveScheduler::Start();
		}
	}

// Create a test entry
void CServerShutdownTestManager::CreateEntry(const TDesC8& aGuid)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
//	TRAP_IGNORE(iEntryView->FetchL(aGuid, entryArray)); //If not found then this function leaves!

//	const TDateTime KEntryDateTime(2008, EApril, 17, 9, 0, 0, 0);
	TTime now;
	now.HomeTime();
	
	HBufC8* guid = aGuid.AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	TCalTime startTime;
	TCalTime endTime;
	startTime.SetTimeLocalL(now + TTimeIntervalHours(1));
	endTime.SetTimeLocalL(now + TTimeIntervalHours(2));
	entry->SetStartAndEndTimeL(startTime, endTime);
    //Add an alarm
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	TTimeIntervalMinutes alarmMinutes(20);
	alarm->SetTimeOffset(alarmMinutes);
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);

	TInt success = 0;
	entryArray.AppendL(entry);

	test (entryArray.Count()==1);
	TRAPD(errStore, iEntryView->StoreL(entryArray, success));
	CleanupStack::PopAndDestroy(entry);
	CleanupStack::PopAndDestroy(&entryArray);
	}
#endif
// Create an additional calendar entry view 
void CServerShutdownTestManager::CreateAdditionalEntryViewL(const TDesC& aCalFile, TBool aWaitToOpen)
	{
	_LIT(KCreatingCalEntryView, "Creating an additional Entry View\n");
	test.Printf(KCreatingCalEntryView);
		
	iAdditionalSession->OpenL(aCalFile);

	iAdditionalEntryView = CCalEntryView::NewL(*iAdditionalSession, *this);
	
	if (aWaitToOpen)
		{
		CActiveScheduler::Start();
		}
	}

// Close the calendar file entry view 
void CServerShutdownTestManager::CloseEntryView()
	{
	_LIT(KClosingCalEntryView, "Closing the Entry View\n");
	test.Printf(KClosingCalEntryView);
	
	delete iEntryView;
	iEntryView = NULL;
	}

// Close the additional calendar file entry view 
void CServerShutdownTestManager::CloseAdditionalEntryView()
	{
	_LIT(KClosingCalAdditionalEntryView, "Closing the additional Entry View\n");
	test.Printf(KClosingCalAdditionalEntryView);
	
	delete iAdditionalEntryView;
	iAdditionalEntryView = NULL;
	}
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
void CServerShutdownTestManager::CloseInstanceView()
	{
	_LIT(KClosingCalInstanceView, "Closing the Instance View\n");
	test.Printf(KClosingCalInstanceView);
	
	delete iInstanceView;
	iInstanceView = NULL;
	}
#endif
CServerShutdownTestManager* CServerShutdownTestManager::NewLC(TInt aPriority)
	{
	CServerShutdownTestManager* self = new (ELeave) CServerShutdownTestManager(aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();

	return (self);
	}

CServerShutdownTestManager::~CServerShutdownTestManager()
	{
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	iSystemStateNotification.Close();
#endif
	delete iFileRegister;
	iPIMTestServer.Close();

	delete iPeriodic;
	iProcess.Close();
	delete iSession;
	delete iAdditionalSession;
	
	delete iEntryView;
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	delete iInstanceView;
#endif
	delete iAdditionalEntryView;
	}

void CServerShutdownTestManager::ConstructL()
	{
	iPeriodic = CPeriodic::NewL(EPriorityIdle);

	User::LeaveIfError(HAL::Get(HAL::ENanoTickPeriod, iNanoTickPeriod));
	User::LeaveIfError(iPIMTestServer.Connect());
	
	// Copy the test Calendar files from the ROM drive to the C: used by the test cases later
	iPIMTestServer.CopyFileL(KServerShutdownCalFileROM, KServerShutdownCalFileRAM);
	iPIMTestServer.CopyFileL(KServerShutdownCalFileMultiFileROM, KServerShutdownCalFileMultiFileRAM);
	iPIMTestServer.CopyFileL(KServerShutdown91CalFileROM, KServerShutdown91CalFileRAM);
	
	iFileRegister = CTestRegister::NewL();
		
	iFileRegister->RegisterL(KServerShutdownCalFile, EFileTypeAgn);
	iFileRegister->RegisterL(KServerShutdownCalFileMultifFile, EFileTypeAgn);
	iFileRegister->RegisterL(KServerShutdown91CalFile, EFileTypeAgn);
		
	// Start the console alarm alert server required by the agenda server
	
#if __WINS__
	CCalTestLibrary::StartAlarmServerL();
#endif
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT	
	CreateSystemStateNotificationProperty();
	// Connect to the alarm server
	User::LeaveIfError(iAlarmServer.Connect());

	// Make sure that the alarm queue is empty.
	ClearAllAlarmsL();
#endif
	}

// Run all the test cases the delay added to the calendar file before it is closed
void CServerShutdownTestManager::RunAllTestsL()
	{
	
	// Test the file close delay timer with a single calendar file
	TestServerShutdownWithIndexL();
	
	// Test the cancellation of a file close delay timer with a single calendar file
	TestServerShutdownCancelWithIndexL();

	// Test the file close delay timer avoids the rebuilding of indexes when the same calendar file is opened twice in the same session
	TestServerShutdownSingleSessionOpenTwiceL();
	
	// Test the file close delay timer does not shutdown the server when a session out of multiple sessions to a single calendar file is closed and
	// that the indexes are not rebuilt when another session reconnects to the same calendar file
	TestServerShutdownMultiSessionSingleFileOpenL();
	
	// Test the file close delay timer does not shutdown the server when a session out of multiple sessions to two separate calendar files is closed
	TestServerShutdownMultiSessionMultiFileOpenL();

	// Test the file close delay timer is not triggered and the calendar file is closed immediately when opening a calendar file leaves
	TestServerShutdownOpenInvalidFileLeaveL();
	
	// Test the file close delay timer is not triggered when the server is killed and terminated both with or without an entry view
	TestServerShutdownProcessExitL();
	
	// Test the file close delay timer is not triggered when the calendar file is deleted
	TestServerShutdownDeleteFileL();
	
	// Test the file close delay timer is cancelled on a calendar file if a session associated with a different calendar file deletes it
	// It should not return with KErrInUse because of the delay timer
	TestServerShutdownDeleteMultiFilesL();
	
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	
	//Test harness to simulate the system state shutdown notifications
	TestServerShutdownWithSystemStateNotificationsL();
	
	TestServerShutdownWithSystemStateNotificationsAsyncL();
	
	TestServerShutdownWithNormalSystemStateNotification();
	
	TestServerShutdownWithStateNotificationAndInstanceViewL();
	
#endif
	
#if __WINS__
	// Test the file close delay timer with opening and closing and deleting a calendar file under OOM conditions
//	TestServerShutdownOOML();
#endif	
	}

// Logon to the agenda server process in order to be notified when the process exits / dies
void CServerShutdownTestManager::ProcessLogonL()
	{
	_LIT(KAgendaServerUIDMatch, "*[10003a5b]*");
	_LIT(KAgendaServerProcessLogon, "Logon to agenda server shutdown\n");
	_LIT(KAgendaServerProcessNotFound, "Couldnt find the agenda server process\n");

	// the name of the agenda server process includes - [10003a5b]
	TFindProcess findProcess(KAgendaServerUIDMatch);
	
	TFullName fullName(KNullDesC);
	
	TInt findProcessResult(KErrNone);
	findProcessResult = findProcess.Next(fullName);
	
	User::LeaveIfError(findProcessResult);
	
	while (findProcessResult == KErrNone)
		{
		findProcessResult = findProcess.Next(fullName);
		}
	
	// The agenda server process is running 
	User::LeaveIfError(iProcess.Open(fullName));

	if(!IsActive())
		{
		iProcess.Logon(iStatus); // ask for a callback when the process ends
		SetActive();
		test.Printf(KAgendaServerProcessLogon);
		}
	else
		{
		test.Printf(KAgendaServerProcessNotFound);
		User::Leave(KErrNotFound);
		}
	}

void CServerShutdownTestManager::ShutdownProcessL(const TDesC& aProcessUid)
	{
	_LIT(KAgendaServerProcessNotFound, "Couldnt find the agenda server process\n");

	// the name of the agenda server process includes the uid
	TFindProcess findProcess(aProcessUid);

	TFullName fullName(KNullDesC);

	TInt findProcessResult(KErrNone);
	findProcessResult = findProcess.Next(fullName);

	User::LeaveIfError(findProcessResult);

	while (findProcessResult == KErrNone)
		{
		findProcessResult = findProcess.Next(fullName);
		}
	RProcess process;
	User::LeaveIfError(process.Open(fullName));
	process.Kill(0);
	}

// Test the file close delay timer with a single calendar file

void CServerShutdownTestManager::TestServerShutdownWithIndexL()
	{
	_LIT(KTestServerShutdownWithIndex,"Testing calendar file close timer...");
	test.Next(KTestServerShutdownWithIndex);

	
	iTestCase = ETestServerShutdownWithIndex;
	
	CreateSessionL();
	
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();

	CreateEntryViewL(KServerShutdownCalFile, ETrue);  // the index should be built here
	CloseEntryView();

	// Record a timestamp before initiating server shutdown
	// Server shutdown period is checked against the expected range when the process exits
	RecordTimeStampBeforeServerShutdown();
	
	// Close the session which will trigger the delay timer and wait for the agenda server to close since this is the end of the test case
	CloseSession(ETrue);
	}

// Test the cancellation of a file close delay timer with a single calendar file

void CServerShutdownTestManager::TestServerShutdownCancelWithIndexL()
	{
	_LIT(KTestServerShutdownCancelWithIndex,"Testing calendar file close timer cancellation...");
	test.Next(KTestServerShutdownCancelWithIndex);


	iTestCase = ETestServerShutdownCancelWithIndexStart;
	
	CreateSessionL();
	
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();

	CreateEntryViewL(KServerShutdownCalFile, ETrue);  // the index should be built here
	CloseEntryView();
	
	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);

	// Trigger an asynchronous delay with more than half the period of the expected sever shutdown period
	// We will trigger another identical asynchronous delay with the same period and then test that the server is still alive
	AsyncDelay(KServerCancelDelay);
	
	CreateSessionL();

	iTestCase = ETestServerShutdownCancelWithIndexCancelled;
	CreateEntryViewL(KServerShutdownCalFile, ETrue);  // the index should NOT be rebuilt here

	_LIT(KAgendaServerCloseNotExpected,"The agenda shutdown should be cancelled agenda server should not close now\n");
	test.Printf(KAgendaServerCloseNotExpected);
	
	// The agenda server should not shutdown in the course of this asynchronous delay 
	AsyncDelay(KServerCancelDelay);

	iTestCase = ETestServerShutdownCancelWithIndexShutdown;
	
	// Record the time before initiating server shutdown
	// Server shutdown period is checked against the expected range when the process exits
	RecordTimeStampBeforeServerShutdown();
	
	CloseEntryView();
	
	// Close the session which will trigger the delay timer and wait for the agenda server to close since this is the end of the test case
	CloseSession(ETrue);
	}

// Test the file close delay timer by opening and closing a calendar file under OOM conditions
// This function is called by the OOM callback


void CServerShutdownTestManager::TestServerShutdownOpenCloseFileOOML()
	{
	_LIT(KTestServerShutdownOpenCloseFileOOM,"Testing open and close calendar file under OOM conditions");
	test.Next(KTestServerShutdownOpenCloseFileOOM);

	
	CCalSession* calSession = CCalSession::NewL();
	CleanupStack::PushL(calSession);
	
	_LIT(KCreatingCalEntryViewOOM, "Creating the Entry View under OOM\n");
	test.Printf(KCreatingCalEntryViewOOM);
	
	calSession->OpenL(KServerShutdownCalFile);

	CCalEntryView* calEntryView = CCalEntryView::NewL(*calSession);
	CleanupStack::PushL(calEntryView); 
		
	CleanupStack::PopAndDestroy(calEntryView);
	CleanupStack::PopAndDestroy(calSession);
	}

// Test the file close delay timer by deleting a calendar file under OOM conditions
// The calendar file must exist before deletion. This is ensured by OOM callback which then calls this test case


void CServerShutdownTestManager::TestServerShutdownDeleteFileOOML(TInt aFailAt)
	{
	_LIT(KTestServerShutdownDeleteFileOOM, "Testing calendar file deletion under OOM conditions");
	test.Next(KTestServerShutdownDeleteFileOOM);


	// Set an invalid file position
	TInt filePostion = -1;
	
	CCalSession* calSession = CCalSession::NewL();
	CleanupStack::PushL(calSession);	

	// Reset the server side OOM allocation failure until we invoke the deletion
	calSession->_DebugSetHeapFailL(RAllocator::ENone, 0);

	_LIT(KCheckFileExistsBeforeDeletionOOM, "Ensure that the calendar file exists before deletion - OOM test");
	test.Printf(KCheckFileExistsBeforeDeletionOOM);
	
	// Check the calendar file exists before deletion
	CDesCArray* fileArray = NULL;
	fileArray = calSession->ListCalFilesL();
	CleanupStack::PushL(fileArray);

	fileArray->Find(KServerShutdownCalFileToDeleteOOM,filePostion);
	test(fileArray->Count());
	test(filePostion != fileArray->Count());
	
	CleanupStack::PopAndDestroy(fileArray);
	
	// Set the server side OOM allocation failure
	calSession->_DebugSetHeapFailL(RAllocator::EDeterministic, aFailAt);
	
	// Time the delete operation and test that the delay timer is not triggered
	TPerformanceTimer deleteTimer (test);
	deleteTimer.Start();
	calSession->DeleteCalFileL(KServerShutdownCalFileToDeleteOOM);
	deleteTimer.Stop();
	test(deleteTimer.ElapsedTime() < KServerShutdownDelayMin);
	
	// Reset the server side OOM allocation failure
	calSession->_DebugSetHeapFailL(RAllocator::ENone, 0);
	
	fileArray = calSession->ListCalFilesL();

	CleanupStack::PushL(fileArray);
	fileArray->Find(KServerShutdownCalFileToDeleteOOM,filePostion);

	// Ensure that the file was deleted
	if(fileArray->Count())
		{
		test(filePostion == fileArray->Count());	
		}
	else
		{
		test(filePostion == 0);
		}

	_LIT(KFileSuccessfullyDeletedOOM, "Calendar file deleted successfully - OOM test");
	test.Next(KFileSuccessfullyDeletedOOM);

	
	CleanupStack::PopAndDestroy(fileArray);
	CleanupStack::PopAndDestroy(calSession);
	}

// Test the file close delay timer is not triggered when the calendar file is deleted


void CServerShutdownTestManager::TestServerShutdownDeleteFileL()
	{
	_LIT(KTestServerShutdownDeleteFile, "Testing calendar file deletion...");

	test.Next(KTestServerShutdownDeleteFile);

	
	iTestCase = ETestServerShutdownDeleteFile;
	
	// Make a copy of the calendar file before testing its deletion
	iPIMTestServer.CopyFileL(KServerShutdownCalFileRAM, KServerShutdownCalFileToDeleteRAM);
	// In case this test case fails, the register will cleanup the file
	iFileRegister->RegisterL(KServerShutdownCalFileToDelete, EFileTypeAgn);
	
	// Set an invalid file position
	TInt filePostion = -1;
	
	// Create a session - this will start the agenda server
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	CreateEntryViewL(KServerShutdownCalFileToDelete, ETrue);
	CloseEntryView();
	
	_LIT(KCheckFileExistsBeforeDeletion, "Ensure that the calendar file exists before deletion");
	test.Printf(KCheckFileExistsBeforeDeletion);

	CDesCArray* fileArray = NULL;
	fileArray = iSession->ListCalFilesL();
	CleanupStack::PushL(fileArray);
	
	fileArray->Find(KServerShutdownCalFileToDelete,filePostion);
	test(fileArray->Count());
	test(filePostion != fileArray->Count());
	
	CleanupStack::PopAndDestroy(fileArray);

	// Time the delete operation and test that the delay timer is not triggered
	TPerformanceTimer deleteTimer (test);
	deleteTimer.Start();
	TRAPD(err, iSession->DeleteCalFileL(KServerShutdownCalFileToDelete));
	test(err==KErrNone);
	deleteTimer.Stop();
	test(deleteTimer.ElapsedTime() < KServerShutdownDelayMin);
	
	fileArray = iSession->ListCalFilesL();

	CleanupStack::PushL(fileArray);
	
	fileArray->Find(KServerShutdownCalFileToDelete,filePostion);
	
	// Ensure that the file was deleted
	if(fileArray->Count())
		{
		test(filePostion == fileArray->Count());	
		}
	else
		{
		test(filePostion == 0);
		}

	_LIT(KFileSuccessfullyDeleted, "Calendar file deleted successfully");

	test.Next(KFileSuccessfullyDeleted);


	CleanupStack::PopAndDestroy(fileArray);
	
	// Close the session which will trigger the delay timer and wait for the agenda server to close since this is the end of the test case
	CloseSession(ETrue);
	}

// Test the file close delay timer is cancelled on a calendar file if a session associated with a different calendar file deletes it
// It should not return with KErrInUse because of the delay timer

void CServerShutdownTestManager::TestServerShutdownDeleteMultiFilesL()
	{
	_LIT(KTestServerShutdownDeleteMultiFiles, "Testing  calendar file deletion with multiple files...");
	test.Next(KTestServerShutdownDeleteMultiFiles);

	
	iTestCase = ETestServerShutdownDeleteMultiFiles;
	
	// Make a copy of the calendar file before testing its deletion
	iPIMTestServer.CopyFileL(KServerShutdownCalFileRAM, KServerShutdownCalFileToDeleteRAM);
	// In case this test case fails, the register will cleanup the file
	iFileRegister->RegisterL(KServerShutdownCalFileToDelete, EFileTypeAgn);
	
	// Create two sessions and open two entry views on different files
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	CreateEntryViewL(KServerShutdownCalFileToDelete, ETrue);
	CloseEntryView();
	
	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);
		
	CreateAdditionalSessionL();
	CreateAdditionalEntryViewL(KServerShutdownCalFileMultifFile, ETrue);
	CloseAdditionalEntryView();
	
	// Delete a calendar file NOT associated with the current session
	TRAPD(err, iAdditionalSession->DeleteCalFileL(KServerShutdownCalFileToDelete));
	// Should cancel  the timer on the calendar file and then delete it. It should not return with KErrInUse because of the delay timer
	test(err ==KErrNone);
	
	// Record the time before initiating server shutdown
	// Server shutdown period is checked against the expected range when the process exits
	RecordTimeStampBeforeServerShutdown();
	
	// Wait for the agenda server to close
	CloseAdditionalSession(ETrue);	
	}

// Test the file close delay timer is not triggered when the server is killed and terminated both with or without an entry view

void CServerShutdownTestManager::TestServerShutdownProcessExitL()
	{
	_LIT(KTestServerShutdownProcessExit, "Testing agenda server shutdown by killing or terminating the server process");
	test.Next(KTestServerShutdownProcessExit);

	
	iTestCase = ETestServerShutdownProcessExit;

	TPerformanceTimer killTimer(test);
	
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	killTimer .Start();
	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);
	// Test Kill - Session only
	iProcess.Kill(KErrNone);
	CActiveScheduler::Start();
	killTimer .Stop();
	test(killTimer .ElapsedTime() < KServerShutdownDelayMin);

	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	killTimer .Start();
	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);
	// Test Terminate - Session only
	iProcess.Terminate(KErrNone);
	CActiveScheduler::Start();
	killTimer .Stop();
	test(killTimer .ElapsedTime() < KServerShutdownDelayMin);
	
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();

	CreateEntryViewL(KServerShutdownCalFile, ETrue);  // the index should be built here
	CloseEntryView();
	killTimer .Start();
	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);
	// Test Kill - Session and Entry View
	iProcess.Kill(KErrNone);
	CActiveScheduler::Start();
	killTimer .Stop();
	test(killTimer .ElapsedTime() < KServerShutdownDelayMin);
	
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);  // the index should be built here
	CloseEntryView();
	killTimer .Start();
	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);
	// Test Terminate - Session and Entry View
	iProcess.Terminate(KErrNone);
	CActiveScheduler::Start();
	killTimer .Stop();
	test(killTimer .ElapsedTime() < KServerShutdownDelayMin);
	}

// OOM Callback 
// Test the file close delay timer by opening and closing and deleting a calendar file under OOM conditions
void CServerShutdownTestManager::OomTestL(TType aType, TInt aFailAt)
	{
	if(aType == EServer)
		{
		_LIT(KOOMFailAt, "OOM %d\n");
		test.Printf(KOOMFailAt, aFailAt);
	
		switch(iTestCase)
			{
			case ETestServerShutdownOOMDeleteFile:
				{
				// Test deleting a calendar file under OOM conditions
				TestServerShutdownDeleteFileOOML(aFailAt);
				}
			break;		
			case ETestServerShutdownOOMOpenCloseFile:
				{
				// Test opening and closing a calendar file under OOM conditions
				TestServerShutdownOpenCloseFileOOML();
				}
			break;		
			default:
				{
				}
			break;
			}
		}

	}

// Test the file close delay timer under OOM conditions whilst opening and closing and deleting a calendar file

void CServerShutdownTestManager::TestServerShutdownOOML()
	{
	_LIT(KTestServerShutdownOOM, "Test opening and closing the session, deleting calendar file and B&R under OOM conditions");
	test.Next(KTestServerShutdownOOM);


	// Make a copy of the calendar file before testing its deletion
	iPIMTestServer.CopyFileL(KServerShutdownCalFileRAM, KServerShutdownCalFileToDeleteRAM_OOM);
	iFileRegister->RegisterL(KServerShutdownCalFileToDeleteOOM, EFileTypeAgn);
	

	//OutOfMemoryTestL() needs a calsession.
	CreateSessionL();
	
	// Test open and close of a calendar file under OOM conditions
	iTestCase = ETestServerShutdownOOMOpenCloseFile; 
	CalTestOom::OutOfMemoryTestL(*this, EServer, *iSession); 
	
	// Test deletion of a calendar file under OOM conditions
	iTestCase = ETestServerShutdownOOMDeleteFile;
	CalTestOom::OutOfMemoryTestL(*this, EServer, *iSession); 
	}

// Test the file close delay timer avoids the rebuilding of indexes when the same calendar file is opened twice in the same session

void CServerShutdownTestManager::TestServerShutdownSingleSessionOpenTwiceL()
	{
	_LIT(KTestServerShutdownSingleSessionOpenTwice, "Test that open agenda twice in a single session prevents the rebuilding of indexes");
	test.Next(KTestServerShutdownSingleSessionOpenTwice);

	
	iTestCase = ETestServerSingleSessionOpenTwiceStart;
	
	// Create a session - this will start the agenda server
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);
	CloseEntryView();
	
	iTestCase = ETestServerSingleSessionOpenTwiceShutdown;

	CreateEntryViewL(KServerShutdownCalFile, ETrue); // Indexes should NOT be rebuilt here
	CloseEntryView();
	
	// Record the time before initiating server shutdown
	// Server shutdown period is checked against the expected range when the process exits
	RecordTimeStampBeforeServerShutdown();
	
	// Close the session which will trigger the delay timer and wait for the agenda server to close since this is the end of the test case
	CloseSession(ETrue);
	}

// Test the file close delay timer does not shutdown the server when a session out of multiple sessions to a single calendar file is closed and
// that the indexes are not rebuilt when another session reconnects to the file

void CServerShutdownTestManager::TestServerShutdownMultiSessionSingleFileOpenL()
	{
	_LIT(KTestServerShutdownMultiSessionSingleFileOpen, "Testing agenda server's shutdown behaviour when closing a session out of multiple sessions to a calendar file");
	test.Next(KTestServerShutdownMultiSessionSingleFileOpen);

	
	iTestCase = ETestServerMultiSessionSingleFileOpenStart;
	
	// Create two sessions: Open two entry views on the same file, close one, wait and reopen the first one
	// and ensure that the indexes dont get rebuilt
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);
	CloseEntryView();
	
	CreateAdditionalSessionL();
	CreateAdditionalEntryViewL(KServerShutdownCalFile, ETrue);
	CloseAdditionalEntryView();
	
	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);
	
	// The agenda server should NOT shutdown in the course of this asynchronous delay
	AsyncDelay(KServerShutdownDelayMax);

	iTestCase = ETestServerMultiSessionSingleFileOpen;
	
	// Re-open the same calendar file to ensure that the indexes are NOT rebuilt
	CreateSessionL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);
	CloseEntryView();

	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);

	iTestCase = ETestServerMultiSessionSingleFileOpenShutdown;
	
	// Record the time before initiating server shutdown
	// Server shutdown period is checked against the expected range when the process exits
	RecordTimeStampBeforeServerShutdown();

	// Wait for the agenda server to close
	CloseAdditionalSession(ETrue);
	}

// Test the file close delay timer does not shutdown the server when a session out of multiple sessions to two separate calendar files is closed

void CServerShutdownTestManager::TestServerShutdownMultiSessionMultiFileOpenL()
	{
	_LIT(KTestServerShutdownMultiSessionMultiFileOpen, "Testing agenda server's shutdown behaviour when closing a session out of multiple sessions to a multiple calendar files");
	test.Next(KTestServerShutdownMultiSessionMultiFileOpen);


	iTestCase = ETestServerMultiSessionMultiFileOpenStart;
	
	// Create two sessions and open two entry views on different files
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);
	CloseEntryView();

	CreateAdditionalSessionL();
	CreateAdditionalEntryViewL(KServerShutdownCalFileMultifFile, ETrue);
	CloseAdditionalEntryView();
	
	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseAdditionalSession(EFalse);	
	
	// Trigger an asynchronous delay with more than half the period of the expected sever shutdown period
	// We will trigger another identical asynchronous delay with the same period and then test that the server is still alive
	AsyncDelay(KServerCancelDelay);
	
	// Record the time before initiating server shutdown
	// Server shutdown period is checked against the expected range when the process exits
	RecordTimeStampBeforeServerShutdown();

	// Close the session which will trigger the delay timer but do not wait for the agenda server to close since we need proceed with the rest of the test case
	CloseSession(EFalse);

	// The agenda server should not shutdown in the course of this asynchronous delay 
	AsyncDelay(KServerCancelDelay);

	iTestCase = ETestServerMultiSessionMultiFileOpenShutdown;
	// Wait for the agenda server to shutdown
	CActiveScheduler::Start();
	}

// Test the file close delay timer is not triggered and the calendar file is closed immediately when opening a calendar file leaves

void CServerShutdownTestManager::TestServerShutdownOpenInvalidFileLeaveL()
	{
	_LIT(KTestServerShutdownOpenInvalidFileLeave, "Testing agenda server's behaviour when a leave occurs when opening an invalid calendar file");
	test.Next(KTestServerShutdownOpenInvalidFileLeave);


	iTestCase = ETestServerShutdownOpenInvalidFileLeave;
	
	// Create two sessions and open two entry views on different files
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	
	TPerformanceTimer deleteTimer (test);
	deleteTimer.Start();
	TRAPD(err, CreateEntryViewL(KServerShutdownBadCalFileName, ETrue));
	test(err==KErrNotFound);
	deleteTimer.Stop();
	test(deleteTimer.ElapsedTime() < KServerShutdownDelayMin);

	// Close the session which will trigger the delay timer and wait for the agenda server to close since this is the end of the test case
	CloseSession(ETrue);
	}

void CServerShutdownTestManager::TestServerShutdownTestOldCalendarFileConversionL()
	{
	_LIT(KTestServerShutdownTestOldCalendarFileConversion, "Testing closing of calender file when conversion is required");
	test.Next(KTestServerShutdownTestOldCalendarFileConversion);

	
	iTestCase = ETestServerShutdownv91CalendarFileConversion;
	
	// Create a session to the agenda server
	CreateSessionL();
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	// Open a older verison of the calendar file - v9.1. This will initiate conversion and test the closing of the calendar file
	CreateEntryViewL(KServerShutdown91CalFile, ETrue);
	CloseEntryView();

	// Record the time before initiating server shutdown
	// Server shutdown period is checked against the expected range when the process exits
	RecordTimeStampBeforeServerShutdown();
	
	// Close the session which will trigger the delay timer and wait for the agenda server to close since this is the end of the test case
	CloseSession(ETrue);
	}
#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT


void CServerShutdownTestManager::TestServerShutdownWithSystemStateNotificationsL()
	{
	_LIT(KTestServerShutdownTestSystemStateShutdown, "Testing closing of calender file after system state shutdown notifications.");
	_LIT8(KTestSystemShutdown,"system shutdown test1");
	test.Next(KTestServerShutdownTestSystemStateShutdown);

	iTestCase = ETestServerShutdownWithSystemStateNotification;
	
	// Create a session to the agenda server
	CreateSessionL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);
	CreateEntry(KTestSystemShutdown);
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	SubscribeToSystemNotification();
	SendSystemStateNotification(ESystemStateShutdown);
	TestPostShutdownOperationsL(KTestSystemShutdown, KErrLocked, 1);
	CloseEntryView();
	CloseSession(EFalse);
	User::WaitForRequest(iStatus);
	CActiveScheduler::Start();
	}


void CServerShutdownTestManager::TestServerShutdownWithNormalSystemStateNotification()
	{
	_LIT(KTestServerShutdownTestSystemStateShutdown, "Testing closing of calender file after system state shutdown notifications.");
	_LIT8(KTestSystemShutdown,"system shutdown normal");
	test.Next(KTestServerShutdownTestSystemStateShutdown);

	iTestCase = ETestServerShutdownWithSystemStateNotificationNormal;

	// Create a session to the agenda server
	CreateSessionL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);
	CreateEntry(KTestSystemShutdown);
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	SubscribeToSystemNotification();
	SendSystemStateNotification(ESystemStateShutdown);
	TestPostShutdownOperationsL(KTestSystemShutdown, KErrLocked, 1);
	SendSystemStateNotification(ESystemStateNormal);
	TestPostShutdownOperationsL(KTestSystemShutdown, KErrNone, 1);
	CloseEntryView();
	CloseSession(EFalse);
	User::WaitForRequest(iStatus);
	CActiveScheduler::Start();
	}

void CServerShutdownTestManager::TestServerShutdownWithSystemStateNotificationsAsyncL()
	{
	_LIT(KTestServerShutdownTestSystemStateShutdownAsync, "Testing closing of calender file after system state shutdown notifications and an asychronous delete operation.");
	test.Next(KTestServerShutdownTestSystemStateShutdownAsync);

	_LIT8(KTestServerShutdown, "shutdown test async");
	iTestCase = ETestServerShutdownWithSystemStateNotificationAsync;

	// Create a session to the agenda server
	CreateSessionL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);
	CreateEntry(KTestServerShutdown);
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	TestAsycnDeleteOperationL();
	SubscribeToSystemNotification();
	SendSystemStateNotification(ESystemStateShutdown);
	CloseEntryView();
	CloseSession(EFalse);
	User::WaitForRequest(iStatus);
	CActiveScheduler::Start();
	}

void CServerShutdownTestManager::CreateRecurringEntriesL(const TDesC8& aGuid)
	{
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	test.Printf(_L("creating an entry on 3rd September at 9:30\n"));
    HBufC8* guid = aGuid.AllocLC();
	// creating a todo entry
	CCalEntry* calEntry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(calEntry);

	TDateTime dtStart(2008,ESeptember,10,10,30,0,0);
	TCalTime startCalTime;
	TCalTime endCalTime;

	startCalTime.SetTimeUtcL(dtStart);

	endCalTime.SetTimeUtcL(dtStart);   

	calEntry->SetStartAndEndTimeL(startCalTime,endCalTime);

	//setting the repeat rule
	TCalRRule calRule(TCalRRule::EYearly);
	calRule.SetDtStart(startCalTime);
	calRule.SetInterval(1);
	calRule.SetCount( 5 );

	calEntry->SetRRuleL(calRule);

	TCalTime rDate1;
	TCalTime rDate2;
	TCalTime rDate3;
	TCalTime rDate4;
	TCalTime rDate5;

	rDate1.SetTimeUtcL(TDateTime(2008, EOctober, 2, 10, 0, 0, 0));
	rDate2.SetTimeUtcL(TDateTime(2008, EOctober, 2, 10, 40, 0, 0)); // Same date like before but differs only by time
	rDate3.SetTimeUtcL(TDateTime(2008, EOctober, 2, 10, 40, 25, 0)); // Same date like before but differs only by time
	rDate4.SetTimeUtcL(TDateTime(2008, EOctober, 2, 10, 40, 25, 12)); // Same date like before but differs only by time
	rDate5.SetTimeUtcL(TDateTime(2008, EOctober, 3, 10, 0, 0, 0));
	
	RArray<TCalTime> rDateArray;
	CleanupClosePushL(rDateArray);
	rDateArray.AppendL(rDate1);
	rDateArray.AppendL(rDate2);
	rDateArray.AppendL(rDate3);
	rDateArray.AppendL(rDate4);
	rDateArray.AppendL(rDate5);

	calEntry->SetRDatesL(rDateArray);
	CleanupStack::PopAndDestroy(&rDateArray);
	TInt stored(0);
	entries.AppendL(calEntry);
	CleanupStack::Pop(calEntry);
	iEntryView->StoreL(entries, stored);
	test(stored == entries.Count());
	CleanupStack::PopAndDestroy(&entries);//entries
	}

void CServerShutdownTestManager::TestInstanceViewOperationsL()
	{
	TCalTime calTimeStart;
	TCalTime calTimeEnd;

	calTimeStart.SetTimeUtcL(TDateTime(2008, EAugust, 30, 2, 7, 0, 0));
	calTimeEnd.SetTimeUtcL(TDateTime(2011, EOctober, 30, 2, 10, 0, 0));

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
	CalCommon::TCalTimeRange timeRange(calTimeStart, calTimeEnd);

	test.Printf(_L("Calling FindInstance\n"));

	TRAPD(err, iInstanceView->FindInstanceL(instances, filter, timeRange));
	
	TInt instanceCount = instances.Count();
//	test ( instances.Count() == 10 );//5 Repeat instances + 5 Sporadic instances

	TRAPD(errDelete, iInstanceView->DeleteL(instances[0],CalCommon::EThisAndAll));//delete all the instances for other tests
	test (errDelete == KErrLocked);
	CleanupStack::PopAndDestroy(&instances);
	}


void CServerShutdownTestManager::TestServerShutdownWithStateNotificationAndInstanceViewL()
	{
	_LIT(KTestServerShutdownWithStateNotificationAndInstanceView, "Testing making Instance view operations after shutdown notification");
	test.Next(KTestServerShutdownWithStateNotificationAndInstanceView);

	_LIT8(KInstanceView, "Instance View test2");
	iTestCase = ETestServerShutdownWithSystemStateNotificationInstance;
	// Create a session to the agenda server
	CreateSessionL();
	CreateEntryViewL(KServerShutdownCalFile, ETrue);
	CreateInstanceViewL(ETrue);
	CreateRecurringEntriesL(KInstanceView);
	// Logon to the agenda server process in order to be notified when the process exits / dies
	ProcessLogonL();
	SubscribeToSystemNotification();
	SendSystemStateNotification(ESystemStateShutdown);
	TestInstanceViewOperationsL();
	CloseInstanceView();
	CloseEntryView();
	CloseSession(ETrue);
	}

void CServerShutdownTestManager::SubscribeToSystemNotification()
	{
	TInt err = iSystemStateNotification.Attach(iMyUid, itcal_key);
	iSystemStateNotification.Subscribe(iStatus);
	}

void CServerShutdownTestManager::CreateSystemStateNotificationProperty()
	{
	itcal_key = 111;
	TUid myUid = {0xE6AEDA09};
	iMyUid = myUid;
	iSystemStateNotification.Define(iMyUid, itcal_key, RProperty::EInt);
	}

void CServerShutdownTestManager::SendSystemStateNotification(TSystemStateNotifications aSystemStateNotification)
	{
	iSystemStateNotification.Set(iMyUid, itcal_key, aSystemStateNotification);
	}

void CServerShutdownTestManager::TestSystemAckNotification()
	{
	TInt value;
	iSystemStateNotification.Get(value);
	test(value == ESystemStateAck);
	}
void CServerShutdownTestManager::TestPostShutdownOperationsL(const TDesC8& aGuid, TInt aErr, TInt aAlarmCount)
	{
	_LIT(KLocation, "London");

	RPointerArray<CCalEntry> entryList;
	CleanupResetAndDestroyPushL(entryList);
	TRAPD(errFetch, iEntryView->FetchL(aGuid, entryList));
//	test (errFetch == KErrNone);
	CCalEntry* entry = entryList[0];
	entry->SetLocationL(KLocation());
	
	//Check that a single alarm has been queued.
	test (iAlarmServer.AlarmCountByState(EAlarmStateQueued) == aAlarmCount);
	TInt numEntries = 0;
	TRAPD(errUpdate, iEntryView->UpdateL(entryList, numEntries));
	test (errUpdate == aErr);
	TRAPD(errDelete, iEntryView->DeleteL(*entry));
	test (errDelete == aErr);
	
	CleanupStack::PopAndDestroy(&entryList);
	}

void CServerShutdownTestManager::TestAsycnDeleteOperationL()
	{
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TRAPD(err, iEntryView->DeleteL(timeRange, filter, *this));
	test (err == KErrNone);
	
	CActiveScheduler::Start();
	}

void CServerShutdownTestManager::ClearAllAlarmsL()
	{
	// Delete all alarms
	RArray<TAlarmCategory> categories;
	
	iAlarmServer.GetAvailableCategoryListL(categories);
	
	const TInt KCount = categories.Count();
	for(TInt i = 0; i < KCount; ++i)
		{
		const TAlarmCategory KCategory = categories[i];
		test(iAlarmServer.AlarmDeleteAllByCategory(KCategory, EFalse) == KErrNone);
		}
		
	categories.Close();

	// Restore alarm sound
	iAlarmServer.SetAlarmSoundState(EAlarmGlobalSoundStateOn);
	}
#endif

// Run all the test cases
static void DoTestsL()
	{
	CServerShutdownTestManager* testManager = CServerShutdownTestManager::NewLC(CActive::EPriorityIdle);

	// Run all the test cases the delay added to the calendar file before it is closed
	testManager->RunAllTestsL();
	
	CleanupStack::PopAndDestroy(testManager);
	}

/**

@SYMTestCaseID     PIM-TCAL-SERVERSHUTDOWN-0001

*/

TInt E32Main()
	{
	__UHEAP_MARK;

	_LIT(KTCalServerShutdownStart, "@SYMTESTCaseID:PIM-TCAL-SERVERSHUTDOWN-0001 Agenda Server Shutdown Test");
	test.Start(KTCalServerShutdownStart);

	
	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);	

	TRAPD(ret, DoTestsL());

	_LIT(KTestResult, "Test steps returned with - %d");
	test.Printf(KTestResult, ret);
	
	test(ret == KErrNone);

	delete scheduler;
	delete trapCleanup;	

	_LIT(KTCalServerShutdownEnd, "Done\n");
	test.Printf(KTCalServerShutdownEnd);

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
	}


