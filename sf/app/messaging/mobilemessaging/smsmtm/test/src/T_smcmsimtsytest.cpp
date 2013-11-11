// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "T_smcmsimtsytest.h"
#include <e32test.h>
#include <e32uid.h>
#include <e32std.h>
#include <smuthdr.h>
#include <txtrich.h> 
#include <smscmds.h>
#include <smsclnt.h>
#include <smsuaddr.h>

// Autotest library header file
#include "autotest.h"

#include <testconfigfileparser.h>
#include <simtsy.h>
#include <csmsaccount.h>

#include <e32property.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <tmsvsmsentry.h>
#endif

// Global pointer for use by the autogTest.h macro's
CAutoTest* gAutoTest;


RTest gTest(_L("T_SmcmSimTsyTest Testrig"));
TBool HasCreatedSmsService = EFalse;
CTrapCleanup* theCleanup;

#define LOCAL_CHECKPOINT_CODE(a,b) a.TestCheckPointCodeL((b), (__FILE__), (__LINE__))
#define LOCAL_CHECKPOINT(a,b) a.TestCheckPointL((b), (__FILE__), (__LINE__))
#define LOCAL_CHECKPOINT_COMPARE(a,b,c,d) a.TestCheckPointCompareL((b),(c),(d),(__FILE__),(__LINE__))

//SMS Scripting
_LIT(KSmsScript,"smcmsimtsytest_config.txt");
_LIT(KSmsScriptFull,"\\msgtest\\sms\\smcmsimtsytest_config.txt");

void DeleteSmsStores(RFs& aFs)
	{
	_LIT(KReassemblyStoreName,"C:\\System\\Data\\smsreast.dat");
	_LIT(KSegmentationStoreName,"C:\\System\\Data\\smssegst.dat");
	aFs.Delete(KReassemblyStoreName);
	aFs.Delete(KSegmentationStoreName);
	}


CMessageSendStates::CMessageSendStates(TMsvId aId)
	{
	iId = aId;
	}

CMessageSendStates* CMessageSendStates::NewLC(TMsvId aId)
	{
	CMessageSendStates* self=new (ELeave) CMessageSendStates(aId);
	CleanupStack::PushL(self);
	//self->ConstructL();
	return self;
	}

CMessageSendStates* CMessageSendStates::NewL(TMsvId aId)
	{
	CMessageSendStates* self = NewLC(aId);
	CleanupStack::Pop(self);
	return self;
	}

void CMessageSendStates::ConstructL()
	{
	}

CMessageSendStates::~CMessageSendStates()
	{
	iStates.Close();
	}

RMessageSendStatesArray::RMessageSendStatesArray()
	{
	}
		
TInt RMessageSendStatesArray::Find(TMsvId aId) const
	{
	const RMessageSendStatesArray& sendArray = *this;
	TInt count = sendArray.Count();

	for (TInt i=0; i<count; ++i)
		{
		if (sendArray[i]->iId == aId)
			return i;
		}

	return -1;
	}

void RMessageSendStatesArray::CopyInSelectionL(const CMsvEntrySelection& aSelection)
	{
	ResetAndDestroy();
	TInt count = aSelection.Count();

	while (count--)
		{
		const TMsvId id = aSelection[count];
		CMessageSendStates* msg = CMessageSendStates::NewLC(id);
		User::LeaveIfError(Append(msg));
		CleanupStack::Pop(msg);
		}
	}

CSmcmSendTest* NewSmcmTestLCC(TInt aTestNumber, TInt aExpectedError=KErrNone)
/**
	Constructs and returns CSmsScTest object. 
	Notifies the system agent that the phone is on.

	@return		CSmsScTest*		A pointer to SmsSc test.  
 */
	{
	CSmsTestUtils* testUtils = CSmsTestUtils::NewL(gTest, HasCreatedSmsService?ETuCreateTestDirectories|ETuGoClientSide:ETuMinimalClientSide);
	if (HasCreatedSmsService)
		testUtils->iServiceSettings = CSmsSettings::NewL();
	HasCreatedSmsService = ETrue;
	CleanupStack::PushL(testUtils);

	// System agent fix... this ensures that the correct test script is used by SIM.TSY
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidSASimTsyTestNumberValue, aTestNumber));
//	testUtils->SetSimTsyTestNumberL(aTestNumber);
	testUtils->NotifySaPhoneOffL();
	User::After(5000000); //wait for SmsProt to go to sleep
	DeleteSmsStores(testUtils->FileSession());

	TInt nextTest = 0;

	testUtils->NotifySaPhoneOnL();
	testUtils->WaitForInitializeL();

	return CSmcmSendTest::NewLC(*gAutoTest, *testUtils, KSmsScript, aExpectedError, nextTest);
	}

TBool CSmcmSendTest::CheckWatchersStartedL()
	{
	TBool found = EFalse;
	TFullName fullName;
	TFindThread finder(_L("*"));
	//
	while(finder.Next(fullName) == KErrNone && !found)
		{
		// Open the thread
		RThread thread;
		User::LeaveIfError(thread.Open(fullName));

		// Open the process which owns this thread
		RProcess process;
		const TInt error = thread.Process(process);
		if (error == KErrNone)
			{
			const TFileName fileName(process.FileName());
			if (fileName.Compare(_L("Z:\\System\\Libs\\watcher.exe")) == 0)
				found = ETrue;
			process.Close();
			}
		thread.Close();
		}

	return found;
	}

void CSmcmSendTest::TestReceiveClass2L(TInt aTestNumber, TRequestStatus& aStatus)
	{
	iState = EStateTestReceiveClass2;
	iMessagesReceived = 0;
	SetMessagesToReceiveL(aTestNumber);

	//Set the class 2 folder
	iSmsSettings->SetClass2Folder(KMsvGlobalOutBoxIndexEntryId);
	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);
	CSmsAccount* account = CSmsAccount::NewLC();
	account->SaveSettingsL(*iSmsSettings);
	CleanupStack::PopAndDestroy(account);

	iSmsTest.Printf(_L("Waiting to recv %d msgs [test=%d inbox=%d class2=%d]\n"),
		iMessagesToReceive, aTestNumber, KMsvGlobalInBoxIndexEntryId, iSmsSettings->Class2Folder()),

	delete iWatcherStarter;
	iWatcherStarter = NULL;

	TBool watcherRunning = EFalse;
	TRAPD(ignor, watcherRunning = CheckWatchersStartedL());
	if (watcherRunning)
		{
		iSmsTest.Printf(_L("Watchers running.\n"));
		}
	else
		{
		iSmsTest.Printf(_L("Watchers not running. Starting watchers....\n"));
		iWatcherStarter = CTestUtilsWatcherStarter::NewL(CActive::EPriorityHigh);
		}

	iStatus = KRequestPending;
	Queue(aStatus);
	SetActive();
	}

void CSmcmSendTest::TestSendingL(TRequestStatus& aStatus)
	{
	iState = EStateSending;

	TTime now;
	now.HomeTime();
	now += (TTimeIntervalSeconds) 5;

	iSmsTest.DeleteSmsMessagesL(KMsvGlobalOutBoxIndexEntryId);
	iSelection->Reset();

	// Create the test messages specified in the script file and place in the Outbox.
	iSmsTest.Printf(_L("Creating msgs in outbox from script %S\n"), &iScriptFile);
	TBool read = EFalse;
	TRAPD(err, read = iSmsTest.ReadScriptL(iScriptFile, KMsvGlobalOutBoxIndexEntryId, *iSelection, now));

	//iSmsTest.Test()(!err && read);
	LOCAL_CHECKPOINT(iAutoTest, !err && read);

	iSmsTest.Printf(_L("Sending %d messages...\n"), iSelection->Count());
	iSmsTest.DisplayMessagesL(*iSelection);
	iSmsTest.SetEntryL(KMsvGlobalOutBoxIndexEntryId);

	delete iOperation;
	iOperation = NULL;

	// Send the message, by copying it to the SMS Service.
	iCurrentMessages.ResetAndDestroy();
	iCurrentMessages.CopyInSelectionL(*iSelection);

	iOperation = MsvEntry().CopyL(*iSelection, iSmsTest.iSmsServiceId, iStatus);
	
	SetActive();
	Queue(aStatus);
	}

void CSmcmSendTest::TestSchedulingL(TRequestStatus& aStatus)
	{
	iState = EStateScheduling;

	iSmsTest.DeleteSmsMessagesL(KMsvGlobalOutBoxIndexEntryId);
	iSelection->Reset();

	TTimeIntervalSeconds fromNow = 5;
	TTime now;
	now.HomeTime();
	now += fromNow;

	iSmsTest.Printf(_L("Creating msgs in outbox from script %S\n"), &iScriptFile);
	TBool read = EFalse;
	TRAPD(err, read = iSmsTest.ReadScriptL(iScriptFile, KMsvGlobalOutBoxIndexEntryId, *iSelection, now));

	//iSmsTest.Test()(!err && read);
	LOCAL_CHECKPOINT(iAutoTest, !err && read);

	iSmsTest.Printf(_L("Scheduled %d messages...\n"), iSelection->Count());
	iSmsTest.DisplayMessagesL(*iSelection);
	MsvEntry().SetEntryL(KMsvGlobalOutBoxIndexEntryId);

	delete iOperation;
	iOperation = NULL;

	iCurrentMessages.ResetAndDestroy();
	iCurrentMessages.CopyInSelectionL(*iSelection);
	
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandScheduleCopy, TPtrC8(), iStatus);

	SetActive();
	Queue(aStatus);
	}

void Test1L()
	{
	CSmcmSendTest* smcmTest = NewSmcmTestLCC(0, KErrServerTerminated);

	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	smcmTest->TestSendingL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("TestSendingL completed with error"));

	//CleanupStack::PopAndDestroy(waiter);
	//CleanupStack::PopAndDestroy(smcmTest);
	//CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(3);
	}

void Test2L()
	{
	CSmcmSendTest* smcmTest = NewSmcmTestLCC(0, KErrServerTerminated);

	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	smcmTest->TestSchedulingL(waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("TestSchedulingL completed with error"));

	CleanupStack::PopAndDestroy(waiter);
	CleanupStack::PopAndDestroy(smcmTest);
	CleanupStack::PopAndDestroy();
	}

void Test3L()
	{
	CSmcmSendTest* smcmTest = NewSmcmTestLCC(2);

	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	smcmTest->TestReceiveClass2L(2, waiter->iStatus);
	waiter->Start();
	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("TestReceiveClass2L completed with error"));

	CleanupStack::PopAndDestroy(waiter);
	CleanupStack::PopAndDestroy(smcmTest);
	CleanupStack::PopAndDestroy();
	}


void Test4L()
	{
	CMsvOperationWait* waiter = CMsvOperationWait::NewLC();
	waiter->Start();

	CTestUtilsWatcherStarter* watcherStarter = CTestUtilsWatcherStarter::NewL(waiter->iStatus, 20000000);
	CleanupStack::PushL(watcherStarter);

	CActiveScheduler::Start();
	GLOBAL_CHECKPOINT_COMPARE(waiter->iStatus.Int(), KErrNone, _L("TestReceiveClass2L completed with error"));

	CleanupStack::PopAndDestroy(watcherStarter);
	CleanupStack::PopAndDestroy(waiter);
	}


CSmcmSendTest::~CSmcmSendTest()
	{
	Cancel();

	if (iSmsTest.iMsvSession)
		{
		iSmsTest.iMsvSession->RemoveObserver(*this);
		}

	if (iOperation)
		{
		iOperation->Cancel();
		delete iOperation;
		iOperation = NULL;
		}

	delete iSelection;
	iSelection = NULL;

	if (iTimer)
		{
		iTimer->Cancel();
		delete iTimer;
		iTimer = NULL;
		}

	iCurrentMessages.ResetAndDestroy();
	delete iSmsSettings;
	delete iWatcherStarter;
	}

CSmcmSendTest::CSmcmSendTest(CAutoTest& aAutoTest, CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt aExpectedError, TInt aPriority)
: CActive(aPriority), iState(EStateWaiting), iAutoTest(aAutoTest), iScriptFile(aScriptFile), iSmsTest(aSmsTest), iExpectedError(aExpectedError)
	{
	CActiveScheduler::Add(this);
	}

CSmcmSendTest* CSmcmSendTest::NewLC(CAutoTest& aAutoTest, CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt aExpectedError, TInt aPriority)
	{
	CSmcmSendTest* self = new (ELeave) CSmcmSendTest(aAutoTest, aSmsTest, aScriptFile, aExpectedError, aPriority);
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
	}

void CSmcmSendTest::ConstructL()
	{
	iSmsTest.SetLogToFile();

	iSelection = new (ELeave) CMsvEntrySelection();

	iSmsTest.iMsvSession->AddObserverL(*this);
	iSmsTest.InstantiateClientMtmsL();

	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);
	iSmsSettings = CSmsSettings::NewL();

	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iSmsSettings);
	CleanupStack::PopAndDestroy(account);
	}

void CSmcmSendTest::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	CMsvEntrySelection* entries = NULL;

	switch (aEvent)
		{
//		case EMsvEntriesCreated:
		case EMsvEntriesChanged:
		case EMsvEntriesMoved:
			entries = STATIC_CAST(CMsvEntrySelection*, aArg1);
			break;
		default:
			return;
		}

	if (!IsActive())
		return;

	TRequestStatus* status = &iStatus;

	switch (iState)
		{
		case EStateSchedulingThenSend:
		case EStateSending:
			{
			if (aEvent == EMsvEntriesCreated)
				return;

			TInt error = KErrNone;

			iSmsTest.DisplaySendingStatesL(*entries);

			TInt count = entries->Count();
			
			while (count--)
				{
				const TMsvId id = (*entries)[count];
				const TInt find = iCurrentMessages.Find(id);
				
				if (find != KErrNotFound)
					{
					CMessageSendStates& messageSendStates = *iCurrentMessages[find];
					CMsvEntry* cEntry = Session().GetEntryL(id);
					CleanupStack::PushL(cEntry);
					const TMsvEntry& entry = cEntry->Entry();
					User::LeaveIfError(messageSendStates.iStates.Append(entry.SendingState()));					
					CleanupStack::PopAndDestroy(cEntry);
					}
				}

			if (iSmsTest.SendingCompleteL(*iSelection, error) && iState == EStateSchedulingThenSend && iStatus == KRequestPending)
				{
				User::RequestComplete(status, error);
				}
			break;
			}
		case EStateTestReceiveClass2:

			if (aEvent != EMsvEntriesChanged)
				return;

			if (!CheckParentOfReceivedMessageL(*entries))
				{
				iSmsTest.Printf(_L("Parent of messages don't match\n"));
				User::RequestComplete(status, KErrGeneral);
				}
			else if (iMessagesReceived >= iMessagesToReceive)
				{
				iSmsTest.Printf(_L("All messages received\n"));
				User::RequestComplete(status, KErrNone);
				}

		default:
			break;
		}
	}

LOCAL_C const TAutoTestCase SmsScTestCases[] =
/**
	Sets up an array of test functions
 */
	{
	{Test1L,_S("Test Sending")},
	{Test2L,_S("Test Schedule Sending")},
	{Test3L,_S("Test Receiving Class2 Messages")},
	{Test4L,_S("Test Start Stop Watchers")}
	};

LOCAL_C void doMainL()
/**
	Constructs sctive scheduler and starts off the tests
 */
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );
	
	// Copying the config file to the root directory
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CSmsTestUtils::CopyToSimTsyConfigFileL(fs, KSmsScriptFull);
	DeleteSmsStores(fs);

	CleanupStack::PopAndDestroy(&fs);

	// Open the socket server

	RSocketServ serv;
	RSocket socket;

	User::LeaveIfError(serv.Connect());
	CleanupClosePushL(serv);
	
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(serv.FindProtocol(protocolname,protoinfo));
	User::LeaveIfError(socket.Open(serv,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(socket.Bind(smsaddr));

	CleanupClosePushL(socket);

	_LIT(KTestOutput,"smcmsimtsy_testreport.txt");
	_LIT8(KComponentInfo,"smcm and smss");
	// Library defined macro
	// Uses a global pointer instance of CAutoTest
	GLOBAL_AUTOTEST_EXECUTE(SmsScTestCases, KTestOutput, KComponentInfo, gTest);

	CleanupStack::PopAndDestroy(&socket);
	CleanupStack::PopAndDestroy(&serv);
	CleanupStack::PopAndDestroy(scheduler);
	}

GLDEF_C TInt E32Main()
/**
	Calls doMainL and checks for error
 */
	{
	__UHEAP_MARK;

	gTest.Start(_L("Setup"));
	theCleanup = CTrapCleanup::New();


	TRAPD(ret,doMainL());

	if (ret != KErrNone)
		gTest.Printf(_L("Test completed with %d"), ret);

	delete theCleanup;	
	gTest.Console()->SetPos(0, 13);
	gTest.End();
	gTest.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

void CSmcmSendTest::RunL()
	{
	if (iOperation)
		iSmsTest.SetProgressL(*iOperation);

	switch (iState)
		{
		case EStateScheduling:
			DoRunSchedulingL();
			break;
		case EStateSending:
		case EStateSchedulingThenSend:
			DoRunSendingL();
			break;
		case EStateTestReceiveClass2:
			break;
		default:
			break;
		}

	delete iOperation;
	iOperation = NULL;

	if (!IsActive())
		Complete(iStatus.Int());
	}

void CSmcmSendTest::Complete(TInt aError)
/**
	Indicates that the operation is complete with aError.

	@param		aError		error returned by the operation
 */
	{
	iSmsTest.Printf(_L("CSmsScTest::Complete [aError=%d]\n"), aError);
	if (iExpectedError == aError && aError)
		{
		iSmsTest.Printf(_L("CSmsScTest::Complete [%d Expected - PASSED]\n"), aError);
		aError = KErrNone;
		}

	if (iReport != NULL)
		User::RequestComplete(iReport, aError);
	}

TInt CSmcmSendTest::RunError(TInt aError)
/**

	@param		aError		error returned by the operation
	@return		TInt		Returns the KErrNone. 
 */
	{
	Complete(aError);
	return KErrNone;
	}

void CSmcmSendTest::Queue(TRequestStatus& aStatus)
/**

	@param		aStatus		Indicates the status of the operation.
 */
	{
	__ASSERT_DEBUG(iReport==NULL, User::Invariant());

	aStatus=KRequestPending;
	iReport=&aStatus;
	}

void CSmcmSendTest::DoCancel()
	{
	if (iOperation)
		{
		iOperation->Cancel();
		iSmsTest.Printf(_L("Operation Cancelled!\n"));
		}
	else	
		{
		iSmsTest.Printf(_L("No operation to cancel!\n"));
		}

	delete iOperation;
	iOperation = NULL;
	}

void CSmcmSendTest::DoRunSchedulingL()
	{
	if (iStatus == KErrNone && iOperation != NULL)
		iStatus = iSmsTest.iProgress.iError;

	iSmsTest.Printf(_L("Scheduling completed with error %d\n"), iStatus);

	TBool sent = EFalse;

	if (iStatus == KErrNone)
		{
		iSmsTest.DisplaySendingStatesL(*iSelection);
		TInt error = KErrNone;
		sent = iSmsTest.SendingCompleteL(*iSelection, error);
		iStatus = error;
		}
	
	if (!sent)
		{
		iStatus = KRequestPending;
		SetActive();
		iState = EStateSchedulingThenSend;
		}
	else
		{
		iState = EStateWaiting;
		iSmsTest.Printf(_L("Scheduling completed with error %d\n"), iStatus);
		}
	}

void CSmcmSendTest::DoRunSendingL()
	{
	if (iStatus == KErrNone && iOperation != NULL)
		iStatus = iSmsTest.iProgress.iError;

	iState = EStateWaiting;
	iSmsTest.Printf(_L("Sending completed with error %d\n"), iStatus);

	TInt err = KErrNone;

	if (!iSmsTest.SendingCompleteL(*iSelection, err))
		{
		iSmsTest.Printf(_L("Waiting for all messages to send or fail\n"), iStatus);
		iStatus = KRequestPending;
		SetActive();
		iState = EStateSchedulingThenSend;
		}
	else
		{
		PrintSendingStates(iCurrentMessages);
		LOCAL_CHECKPOINT(iAutoTest, CheckSendingStates(iCurrentMessages));
		}
	}

TBool CSmcmSendTest::CheckSendingStates(const RMessageSendStatesArray& sendStateArray)
	{
	const TInt count = sendStateArray.Count();

	for (TInt i=0; i < count; ++i)
		{
		const CMessageSendStates& sendState = *sendStateArray[i];
		const TInt stateCount = sendState.iStates.Count();

		for (TInt j=0; j < stateCount-1; ++j)
			{
			if (sendState.iStates[j] == KMsvSendStateFailed && (sendState.iStates[j+1] == KMsvSendStateResend || sendState.iStates[j+1] == KMsvSendStateScheduled))
				return EFalse;
			}
		}

	return ETrue;
	}

void CSmcmSendTest::PrintSendingStates(const RMessageSendStatesArray& sendStateArray)
	{
	iSmsTest.Printf(_L("=============== Sending States =============== \n\n"));

	TInt count = sendStateArray.Count();
	iSmsTest.Printf(_L("Number of messages= %d\n\n"), count);

	for (TInt i=0; i < count; ++i)
		{
		iSmsTest.Printf(_L("================= Message %d ================\n\n"), i);

		const CMessageSendStates& sendState = *sendStateArray[i];
		TInt stateCount = sendState.iStates.Count();

		for (TInt j=0; j < stateCount; ++j)
			{
			//iSmsTest.Printf(_L("%d\n"), sendState.iStates[j]);

			switch (sendState.iStates[j])
				{
				case KMsvSendStateSending:
					iSmsTest.Printf(_L("Sending %d\n"), sendState.iStates[j]);
					break;
				case KMsvSendStateResend:
					iSmsTest.Printf(_L("Resend %d\n"), sendState.iStates[j]);
					break;
				case KMsvSendStateScheduled:
					iSmsTest.Printf(_L("Scheduled %d\n"), sendState.iStates[j]);
					break;
				case KMsvSendStateFailed:
					iSmsTest.Printf(_L("Failed %d\n"), sendState.iStates[j]);
					break;
				case KMsvSendStateWaiting:
					iSmsTest.Printf(_L("Waiting %d\n"), sendState.iStates[j]);
					break;
				case KMsvSendStateSent:
					iSmsTest.Printf(_L("Sent %d\n"), sendState.iStates[j]);
					break;
				case KMsvSendStateSuspended:
					iSmsTest.Printf(_L("Suspended %d\n"), sendState.iStates[j]);
					break;
				default:
					break;
					}
			}

		iSmsTest.Printf(_L("============== End of Message %d =============\n\n"), i);
		}

	iSmsTest.Printf(_L("=============== End Sending States ============= \n"));
	}


TBool CSmcmSendTest::CheckParentOfReceivedMessageL(const CMsvEntrySelection& aSelection)
	{
	TInt count = aSelection.Count();
	TBool retVal = ETrue;

	while (count-- && retVal)
		{
		const TMsvId id = aSelection[count];
		iSmsTest.SetEntryL(id);
		const TMsvSmsEntry entry(iSmsTest.Entry());

		if (entry.iType == KUidMsvMessageEntry && entry.iMtm == KUidMsgTypeSMS)
			{
			iMessagesReceived++;
			iSmsTest.Printf(_L("%d: %d/%d received\n"),
				id, iMessagesReceived, iMessagesToReceive);

			TSmsDataCodingScheme::TSmsClass smsClass;
			TMsvId expectedFolder = KMsvGlobalInBoxIndexEntryId;

			const TBool hasClass = entry.Class(smsClass);

			if (hasClass && smsClass == TSmsDataCodingScheme::ESmsClass2)
				expectedFolder = iSmsSettings->Class2Folder();

			retVal = (expectedFolder == entry.Parent());

			if (!retVal)
				{
				iSmsTest.Printf(_L("Parent of message %d invalid [parent=%d expected=%d hasClass=%d class=%d smsSettings=%d\n"),
					id, entry.Parent(), expectedFolder, hasClass, smsClass, iSmsSettings->Class2Folder());
				}
			}
		}

	return retVal;
	}

void CSmcmSendTest::SetMessagesToReceiveL(TInt aTestNumber)
	{
	TParse parse;
	User::LeaveIfError(iSmsTest.ResolveFile(_L("SMS"), KSmsScript, parse));

	CTestConfig* configFile = CTestConfig::NewLC(iSmsTest.FileSession(), KNullDesC, parse.FullName());

	TBuf8<16> name;
	name.AppendFormat(_L8("test%d"), aTestNumber);

	const CTestConfigSection* section = configFile->Section(name);

	if (configFile == NULL)
		User::Leave(KErrNotFound);

	iMessagesToReceive = section->ItemCount(_L8("SmsRx"));
	CleanupStack::PopAndDestroy(configFile);
	}


