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

#include "emailtestutils.h"
#include "scripttestutils.h"
#include <commdb.h>
#include <cemailaccounts.h>

_LIT(KSmtsTest, "T_SMTS00 - Testing SMTS Server MTM with script files");

_LIT(KImskOutFile, "checkagainst\\imsk25.out");
_LIT(KSmtsComponent,"SMTS");
_LIT(KSmtsRFCEmails,"RFC822\\");
_LIT(KSmtsScriptFile,"scripts\\smtslog.scr");



RTest test(KSmtsTest);

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils; 
LOCAL_D TMsvId smtpService;
LOCAL_D CBaseServerMtm* serverMtm;
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D TInt globalError;

LOCAL_C void DisplayProgress()
	{
 	TImSmtpProgress temp;	
	TPckgC<TImSmtpProgress> paramPack(temp);

	const TDesC8& progBuf =serverMtm->Progress();
	paramPack.Set(progBuf);
	TImSmtpProgress progress=paramPack();	
	test(progress.ServiceId()==smtpService);
	//  State of Smtp session?
	TBuf<128> logString;
	TInt state = progress.Status();
	switch(state)
		{
		case EMsgOutboxProgressWaiting:
			testUtils->WriteComment(_L("Waiting to start\n"));
			break;
		case EMsgOutboxProgressDone:
			testUtils->WriteComment(_L("Connection Closed.\n"));
			logString.Format(_L("%d messages: Sent %d, failed to send %d, didn't attempt to send %d.\n"), 
				progress.SendTotal(),
				progress.Sent(),
				progress.FailedToSend(),
				progress.NotSent());
			testUtils->WriteComment(logString);
			break;
		case EMsgOutboxProgressConnecting:
			logString.Format(_L("Connecting to SMTP server. Sending %d messages.\n"),
				progress.SendTotal());
			testUtils->WriteComment(logString);
			break;
		case EMsgOutboxProgressSending:
			logString.Format(_L("Sending message No.%d/%d. Transmitted %d bytes of %d            \n"),     
				progress.MsgNo()+1,
				progress.SendTotal(),
				progress.iSendFileProgress.iBytesSent,
				progress.iSendFileProgress.iBytesToSend);
			testUtils->WriteComment(logString);
			break;
		}
	}

class CSmtpTimer : public CTimer
	{
public:
	static CSmtpTimer* NewL(CTestActive& aActive);
	void StartL();
private:
	CSmtpTimer(CTestActive& aActive);
	void RunL();	
	CTestActive& iSmtpActive;
	};

CSmtpTimer::CSmtpTimer(CTestActive& aActive)
: CTimer(EPriorityLow),	iSmtpActive(aActive)

	{}


CSmtpTimer* CSmtpTimer::NewL(CTestActive& aActive)
	{
	CSmtpTimer* self = new(ELeave) CSmtpTimer(aActive);
	CleanupStack::PushL(self);
	CActiveScheduler::Add(self);
	CleanupStack::Pop();
	return self;
	}

void CSmtpTimer::StartL()
	{
	CTimer::ConstructL();
	TRequestStatus* status=&iStatus;
	User::RequestComplete(status,KErrNone);
	SetActive();
	}

void CSmtpTimer::RunL()
	{
	test.Console()->SetPos(0, 10);
	DisplayProgress();

	if (!iSmtpActive.IsActive())   //this is how you check if the MTM is finished or not!
		{
		iSmtpActive.Cancel();
		CActiveScheduler::Stop();
		return;
		}
	else
		After(400000);
	}


LOCAL_C void TestSendingMessagesL()
	{
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CMsvEntrySelection* sendSel = new(ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sendSel);

	testUtils->iServerEntry->SetEntry(KMsvGlobalOutBoxIndexEntryId);
	User::LeaveIfError(testUtils->iServerEntry->GetChildren(*sendSel));
	TInt count=sendSel->Count();
	if (!count)
		{
		testUtils->WriteComment(_L("\t No messages to send in the outbox"));
		CleanupStack::PopAndDestroy(2); //testActive, sendsel
		globalError=KErrNotFound;
		return;
		}
	TBuf<100> logString;
	testUtils->WriteComment(_L("Issuing CopyFromLocal request...\n"));
	logString.Format(_L("\t messages to send in the outbox - %d"), count);
	testUtils->WriteComment(logString);
	

	serverMtm->CopyFromLocalL(*sendSel,smtpService, testActive->iStatus);
	CSmtpTimer* timer=CSmtpTimer::NewL(*testActive);
	CleanupStack::PushL(timer);

	testActive->StartL();
	timer->StartL();
	CActiveScheduler::Start();
	//check progress
	DisplayProgress();
	logString.Zero();
	logString.Format(_L("\t CopyFromLocalL completed with %d"), testActive->iStatus.Int());
	testUtils->WriteComment(logString);
 	TImSmtpProgress temp;	
	TPckgC<TImSmtpProgress> paramPack(temp);

	const TDesC8& progBuf =serverMtm->Progress();
	paramPack.Set(progBuf);
	TImSmtpProgress progress=paramPack();	
	globalError=progress.Error();
	CleanupStack::PopAndDestroy(3); //testActive, sendsel, /*timer*/

	}

LOCAL_C void ClosedownL()
	{
	if (globalError)
		testUtils->TestHarnessFailed(globalError);
	else
		{
		testUtils->TestHarnessCompleted();
		#if !defined(_DEBUG)
			// Cannot compare logs on REL builds, since the log file is not generated
			testUtils->WriteComment(_L("!!!! Logs comparison skipped on REL builds !!!!"));
			testUtils->WriteComment(_L("!!!! Run on DEB build for complete test    !!!!"));
			testUtils->TestHarnessCompleted();
			TBool trueB = ETrue; // avoid compiler warning
			if (trueB) return;
		#endif
		CScriptTestUtils* utls=CScriptTestUtils::NewLC(*testUtils);
		RFile file;
		TParse check;
		User::LeaveIfError(testUtils->ResolveFile(KSmtsComponent,KImskOutFile,check));
		TInt err = file.Open(testUtils->FileSession(), check.FullName(), EFileRead);
		file.Close();
		if(err==KErrNone) // file does not exist - create it
			{
			if (!utls->CheckLogOutputL(25, check.FullName()))
				testUtils->WriteComment(_L("imsk25.out didn't match"));
			else
				{
				testUtils->WriteComment(_L("imsk25.out matched"));
				testUtils->TestHarnessCompleted();
				}
			}
		CleanupStack::PopAndDestroy();  //utls,

		}
	CleanupStack::PopAndDestroy(2);  //testUtils, ischeduler
	}

LOCAL_C void InitL()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->WriteComment(KSmtsTest);

	testUtils->CleanMessageFolderL();
	testUtils->ClearEmailAccountsL();

	if (!testUtils->FileSession().MkDir(KEmailDataDir))
		{
		testUtils->Printf(_L("Created c:\\logs\\email directory\n"));
		}

	testUtils->GoClientSideL();
	smtpService = testUtils->CreateSmtpServiceL();

	CImSmtpSettings* settings = new(ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	
	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL(smtpService,smtpAccountId); 
	accounts->LoadSmtpSettingsL(smtpAccountId, *settings);
	
	_LIT(KSmtpServer, "smtp.demon.co.uk");
	settings->SetServerAddressL(KSmtpServer);
	settings->SetEmailAliasL(_L("SMTP Test1"));
	_LIT(KSmtpEmailAddress, "<test@psion9.demon.co.uk>");
	settings->SetEmailAddressL(KSmtpEmailAddress);
	settings->SetReplyToAddressL(KSmtpEmailAddress);
	settings->SetBodyEncoding(EMsgOutboxMIME);
	settings->SetReceiptAddressL(KSmtpEmailAddress);
	settings->SetPort(25);

	accounts->SaveSmtpSettingsL(smtpAccountId, *settings);

	CleanupStack::PopAndDestroy(3, testUtils); // accounts, settings, testUtils
	testUtils = CEmailTestUtils::NewLC(test, ETuGoServerSide);

	testUtils->InstantiateSmtpServerMtmL();
	serverMtm=testUtils->iSmtpServerMtm;
	
	TParse scriptFile;
	testUtils->ResolveFile(KSmtsComponent,KSmtsScriptFile,scriptFile);
	testUtils->CopyScriptFileL(scriptFile.FullName(), _L("25"));
	}

LOCAL_C void doMainL()
	{
	InitL();

	//overwrite the setings with test code one.  Don't want the default settings.


	testUtils->WriteComment(_L(" smtp server created"));

	TParse dir;
	testUtils->ResolveFile(KSmtsComponent,KSmtsRFCEmails,dir);
	testUtils->CreateMessageFilesL(smtpService,KMsvGlobalOutBoxIndexEntryId,dir.FullName());
	testUtils->WriteComment(_L(" Messages created in the outbox"));
	testUtils->WriteComment(_L("\nPerforming Tests\n"));
	testUtils->TestStart(1);
	TestSendingMessagesL();
	testUtils->TestFinish(1,globalError);
	ClosedownL();
	}


GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_SMTS00 Test SMTP server MTM class"));
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
