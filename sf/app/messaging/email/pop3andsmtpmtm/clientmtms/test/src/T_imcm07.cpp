// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_IMCM07
// Component: IMCM
// Owner: VK
// Brief description of test harness:
// Tests CImEmailAutoSend class.  This test code creates POP and SMTS 
// mtms and calls InvokeAsync (with command KIMAP4MTMConnect/KPOP3MTMConnect) 
// on them which creates SMTC and tries to autosend.
// Detailed description of test harness:
// As above.
// updated to not use TestUtils when instantiating email accounts - TestUtils does
// not associate an SMTP account with POP or IMAP accounts.
// Input files required to run test harness:
// \MSG\IMCMTSRC\RFCT_IMCM07\rfc82202.txt		<DRIVE>:\msgtest\IMCM\RFCT_IMCM07\rfc82202.txt
// \MSG\IMCMTSRC\RFCT_IMCM07\rfc82203.txt		<DRIVE>:\msgtest\IMCM\RFCT_IMCM07\rfc82203.txt
// \MSG\IMCMTSRC\RFCT_IMCM07\rfc82204.txt		<DRIVE>:\msgtest\IMCM\RFCT_IMCM07\rfc82204.txt
// \MSG\IMCMTSRC\RFCT_IMCM07\rfc82205.txt		<DRIVE>:\msgtest\IMCM\RFCT_IMCM07\rfc82205.txt
// \MSG\IMCMTSRC\RFCT_IMCM07\rfc82206.txt		<DRIVE>:\msgtest\IMCM\RFCT_IMCM07\rfc82206.txt
// Intermediate files produced while running test harness:
// None
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMCM07.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_IMCM07.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMCM07.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMCM07.exe on the other platform
// 
//


#include "emailtestutils.h"
#include <commdb.h>
#include <pop3set.h>
#include <smtpset.h>
#include <autosend.h>
#include <imcmutil.h>
#include <bacline.h>
#include <cemailaccounts.h>
#include <iapprefs.h>

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

// local variables etc //

_LIT(KImcmTest, "T_IMCM07 - Testing AutoSend of Email Messages");
_LIT(KDontPanic, "All unsent messages were suspended--this is the expected behaviour");
_LIT(KServer, "lon-cn-exchng2k.msexchange2k.closedtest.intra");
RTest test(KImcmTest);

_LIT(KTestParams,"-testparams");
enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgPopServerAddress,
	EArgPopLogin,
	EArgPopPassword,
	EArgImapServerAddress,
	EArgImapLogin,
	EArgImapPassword,
	EArgSmtpServerAddress,
	EArgSmtpEmail,
	EArgEnd
	};

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CActiveScheduler* scheduler;
LOCAL_C CMsvEntrySelection* msvSelection;
LOCAL_D TInt globalError;
LOCAL_D TMsvId Pop3ServiceId;
LOCAL_D TMsvId SmtpServiceId;
LOCAL_D TMsvId ImapServiceId;
LOCAL_D TMsvId SmtpService2Id;

//

//
// The CUndertaker class notifies its client of the death of a particular thread
// or process. When this occurs it stops the active scheduler.



// Test harness code.

LOCAL_C void DoAutoSendL(TBool aPop)
	{
	TBuf8<1> aParameter;
	msvSelection->ResizeL(0);

	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	CMsvOperation* msvOperation=NULL;
	if (!aPop)
		{
		testUtils->WriteComment(_L("Connecting to the Pop3 server"));
		msvSelection->AppendL(Pop3ServiceId);
		msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnect, *msvSelection, aParameter, testActive->iStatus);
		}
		
	CleanupStack::PushL(msvOperation);
	testActive->StartL();	// Start the connect active object
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2); //testactive, msvOperation
	aParameter.Zero();
	msvSelection->ResizeL(0);

	testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	msvOperation=NULL;
	if (!aPop)
		{
		testUtils->WriteComment(_L("Disconnecting from the Pop3 server"));
		msvSelection->AppendL(Pop3ServiceId);
		msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMDisconnect, *msvSelection, aParameter, testActive->iStatus);
		}
		
	CleanupStack::PushL(msvOperation);
	testActive->StartL();	// Start the connect active object
	CActiveScheduler::Start();

	TBuf<64> logString;
	if (!aPop)
		{
		TPop3Progress temp;	
		TPckgC<TPop3Progress> paramPack(temp);
		paramPack.Set(msvOperation->ProgressL());
		TPop3Progress progress=paramPack();	
		logString.Format(_L(" InvokeAsyncFunctionL completed with %d"),progress.iErrorCode);
		testUtils->WriteComment(logString);
		globalError=progress.iErrorCode;
		}
	CleanupStack::PopAndDestroy(2); //testactive, msvOperation
	}
	
LOCAL_C void Closedown()
	{
	if (globalError)
		testUtils->TestHarnessFailed(globalError);
	else
		testUtils->TestHarnessCompleted();
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler, 
	}

// updates POP account settings 
LOCAL_C void UpdatePopSettingsL(CImPop3Settings& aPopSettings)
	{
	aPopSettings.SetServerAddressL(KServer);
	aPopSettings.SetLoginNameL(testUtils->MachineName());
	aPopSettings.SetPasswordL(testUtils->MachineName());
	aPopSettings.SetPort(110);
	aPopSettings.SetAutoSendOnConnect(ETrue);
	}

// updates SMTP account settings 
LOCAL_C void UpdateSmtpSettingsL(CImSmtpSettings& aSmtpSettings)
	{
	aSmtpSettings.SetServerAddressL(KServer);
	_LIT(KSmtpEmailAddress, "<T_IMCM07@msexchange2k.closedtest.intra>");
	aSmtpSettings.SetEmailAddressL(KSmtpEmailAddress);
	aSmtpSettings.SetReplyToAddressL(KSmtpEmailAddress);
	aSmtpSettings.SetReceiptAddressL(KSmtpEmailAddress);
	aSmtpSettings.SetEmailAliasL(_L("IMCM07"));
	aSmtpSettings.SetBodyEncoding(EMsgOutboxMIME);
	aSmtpSettings.SetPort(25);
	}

LOCAL_C void InitL()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	testUtils = CEmailTestUtils::NewLC(test);

	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	}
	

LOCAL_C void InitPopL()
	{
	// delete all accounts, emails etc
	testUtils->CleanMessageFolderL();
	testUtils->ClearEmailAccountsL();
	testUtils->GoClientSideL();

	// Create POP account via CEmailAccounts...
	CEmailAccounts* emailAccounts = CEmailAccounts::NewLC();

	// Create objects requried for POP account
	CImIAPPreferences* popIapPreferences = CImIAPPreferences::NewLC();

	CImPop3Settings*   popSettings = new (ELeave) CImPop3Settings;
	CleanupStack::PushL(popSettings);
	
	// Create objects requried for SMTP account
	CImIAPPreferences* smtpIapPreferences = CImIAPPreferences::NewLC();

	CImSmtpSettings *smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);

	// Set the default POP and SMTP settings
	emailAccounts->PopulateDefaultPopSettingsL(*popSettings, *popIapPreferences);
	emailAccounts->PopulateDefaultSmtpSettingsL(*smtpSettings, *smtpIapPreferences);
	
	// overwrite the IAP preferences
	TImIAPChoice iapChoice;
	iapChoice.iIAP = 1;
	iapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;

	TInt index = 0;
	popIapPreferences->AddIAPL(iapChoice, index);
	smtpIapPreferences->AddIAPL(iapChoice, index);
	
	// update POP, IMAP and SMTP settings
	UpdatePopSettingsL(*popSettings);
	UpdateSmtpSettingsL(*smtpSettings);
	
	// Create the POP account
	_LIT(KPopAccountName, "PopAccount");
	TPopAccount  popAccount  = emailAccounts->CreatePopAccountL(KPopAccountName, *popSettings,*popIapPreferences,EFalse);
	TSmtpAccount smtpAccount = emailAccounts->CreateSmtpAccountL(popAccount, *smtpSettings, *smtpIapPreferences, EFalse);
	emailAccounts->SetDefaultSmtpAccountL(smtpAccount);

	// set the global service ID variables
	Pop3ServiceId = popAccount.iPopService;
	SmtpServiceId = smtpAccount.iSmtpService;
	
	// tell the test utils what the ids are.	
	testUtils->iPopServiceId  = Pop3ServiceId;
	testUtils->iSmtpServiceId = SmtpServiceId;
	
	CleanupStack::PopAndDestroy(5, emailAccounts);
	
	// Create test message files
	testUtils->GoServerSideL();
	testUtils->CreateMessageFilesL(SmtpServiceId, KMsvDraftEntryId,_L("c:\\mailtest\\rfcT_IMCM07\\"));
	testUtils->GoClientSideL();
	}

LOCAL_C void EmptyDraftsFolderL()
	{
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	testUtils->iMsvEntry->SetEntryL(KMsvDraftEntryIdValue);
	CMsvEntrySelection* draftSel = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(draftSel);
	CMsvOperation* operation = testUtils->iMsvEntry->DeleteL(*draftSel, testActive->iStatus);
	CleanupStack::PushL(operation);
	testActive->StartL();	// Start the connect active object
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(3, testActive); //testActive, draftSel, operation
	}

	
LOCAL_C void SetDifferentSendingStatesOnMessagesL(CMsvEntrySelection& msgList)
	{
	test(msgList.Count()>=3); //otherwise following will fail!!

	TMsvEmailEntry entry;

	testUtils->iMsvEntry->SetEntryL(msgList.At(0));
	entry = testUtils->iMsvEntry->Entry();
	if (entry.SendingState() != KMsvSendStateResend)
		entry.SetSendingState(KMsvSendStateResend);
	testUtils->ChangeEntryL(entry);

	testUtils->iMsvEntry->SetEntryL(msgList.At(1));
	entry = testUtils->iMsvEntry->Entry();
	if (entry.SendingState() != KMsvSendStateScheduled)
		entry.SetSendingState(KMsvSendStateScheduled);
	testUtils->ChangeEntryL(entry);

	//This message should not be sent!
	testUtils->iMsvEntry->SetEntryL(msgList.At(2));
	entry = testUtils->iMsvEntry->Entry();
	if (entry.SendingState() != KMsvSendStateSuspended)
		entry.SetSendingState(KMsvSendStateSuspended);
	testUtils->ChangeEntryL(entry);

	for (TInt count = 3; count<msgList.Count(); count++)
		{
			// all others to waiting - these should be sent as well!!
			testUtils->iMsvEntry->SetEntryL(msgList.At(count));
			entry = testUtils->iMsvEntry->Entry();
			if (entry.SendingState() != KMsvSendStateWaiting)
				entry.SetSendingState(KMsvSendStateWaiting);
			testUtils->ChangeEntryL(entry);
		}
	}

LOCAL_C void CheckMessagesCreatedAndCopyToOutboxL()
	{
	testUtils->iMsvEntry->SetEntryL(KMsvDraftEntryId);
	CMsvEntrySelection* msgList = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(msgList);
	TInt count=msgList->Count();
	CMsvOperation* msvOperation=NULL;
	if (count)
		{
		CMsvEntry* msvEntry = testUtils->iMsvSession->GetEntryL(KMsvDraftEntryId);
		CleanupStack::PushL(msvEntry);
		TRequestStatus status=KRequestPending;
		msvOperation = msvEntry->CopyL(*msgList, KMsvGlobalOutBoxIndexEntryId,status);
		CleanupStack::PushL(msvOperation);

		TInt err=KErrNone;
		while(status==KRequestPending)
			{
			CActiveScheduler::Current()->RunIfReady(err,0);
			User::WaitForAnyRequest();
			}

		test(err==KErrNone);
		test(msvOperation->iStatus.Int() == KErrNone);
		testUtils->WriteComment(_L("Messages created sucessfully in the Outbox"));
		testUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
		CMsvEntrySelection* outboxSel = testUtils->iMsvEntry->ChildrenL();
		CleanupStack::PushL(outboxSel);
		// set sending states on the messages
		SetDifferentSendingStatesOnMessagesL(*outboxSel);
		testUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
		CleanupStack::PopAndDestroy(3); //msvOperation, msvEntry, outboxSel
		}
	else
		{
		testUtils->WriteComment(_L("Failed to create messages"));
		Closedown();
		return;
		}
	CleanupStack::PopAndDestroy(msgList);
	}



LOCAL_C TInt CountUnsentEmailsSuspended(CMsvEntrySelection& aUnsentEmails)
/**
Counts how many of the unsent emails were suspended, and hence shouldn't have been sent anyway!

  @param aUnsentEmails a CMsvEntrySelection of the emails that were left behind in the outbox.
  @return The number of unsent emails that were marked as suspended
*/
	{
	TInt suspendedCount = 0;
	TMsvEntry entry;
	for (TInt count = 0; count<aUnsentEmails.Count(); count++)
		{
			// use testUtils' iMsvEntry member to "pick up" the current entry
			testUtils->iMsvEntry->SetEntryL(aUnsentEmails.At(count));
			entry = testUtils->iMsvEntry->Entry();

			//Was it suspended?
			if (entry.SendingState() == KMsvSendStateSuspended)
				//if so, increment the count
				suspendedCount++;
		}

	testUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);	//Remove lock so entry can be deleted

	return suspendedCount;
	}


// Get a handle on the SMTP autosend process/thread and pass it to the undertaker.
// The undertaker will handle death notifications from the process or thread and stop
// the active scheduler accordingly.
// Note: When running on WINS the autosend is launched in a separate thread as opposed to a
// fully fledged process.
LOCAL_C void WaitForAutosendToComplete()
	{
	// Give the process a chance to get started first...
	User::After(3*1000*1000);
	
	RProcess thread;  // Process masquerading as a thread.
	_LIT(KAutoSendProcess, "AUTOSEND*");
	TFullName name;
	TFindProcess findAutoSendProcess(KAutoSendProcess);
	TInt errorInt = findAutoSendProcess.Next(name);
    if (errorInt == KErrNone)
    	errorInt = thread.Open(findAutoSendProcess);

	// Get undertaker to wait for Autosend to finish. 
	// KErrNone might indicate that it finished already.
	if(errorInt == KErrNone)
		{
		TRequestStatus status; 
		thread.Logon(status);
		User::WaitForRequest(status);
		thread.Close();
		}
	}


LOCAL_C void CheckMessagesSentL(TInt aTestNumber)	
	{
	testUtils->TestStart(aTestNumber,_L("Checking all messages in Outbox have been sent"));

	// make sure that selection on outbox is refreshed by changing context to an
	// alternative mailbox prior to getting the outbox selection.
	testUtils->iMsvEntry->SetEntryL(KMsvDraftEntryIdValue);
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryIdValue);
	CMsvEntrySelection* selection = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(selection);

	// Are there any messages in the outbox?
	// if so, delete them and report an error if any remain that should have been sent.
	if (selection->Count()!=0)
		{
		TInt unsent = CountUnsentEmailsSuspended(*selection);
		CTestActive* testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		testUtils->WriteComment(_L("Not all messages sent - deleting unsent messages"));
		
		//only interested in the ones that *should've* been sent
		globalError = selection->Count() - unsent;
		CMsvOperation* operation = testUtils->iMsvEntry->DeleteL(*selection, testActive->iStatus);
		CleanupStack::PushL(operation);
		testActive->StartL();	// Start the connect active object
		CActiveScheduler::Start();
		CleanupStack::PopAndDestroy(2, testActive); //testActive, operation

		if (!globalError)
			{
			testUtils->WriteComment(KDontPanic);
			}
		}

	CleanupStack::PopAndDestroy(selection);
	testUtils->TestFinish(aTestNumber,globalError);
	}



LOCAL_C void TestPop3AutosendL()
	{
	InitPopL();


	testUtils->InstantiatePopClientMtmL();
	testUtils->InstantiateSmtpClientMtmL();

	CheckMessagesCreatedAndCopyToOutboxL();

	testUtils->TestStart(1,_L("Connect to pop and auto send messages from the outbox"));
	DoAutoSendL(EFalse);
	testUtils->TestFinish(1,globalError);

	WaitForAutosendToComplete();

	CheckMessagesSentL(2);
	}


LOCAL_C void doMainL()
	{
	InitL();

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	// Test Autosend via POP3
	TestPop3AutosendL();

	CleanupStack::PopAndDestroy(msvSelection);
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCM07 Test CImEmailAutoSend Functionality"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
