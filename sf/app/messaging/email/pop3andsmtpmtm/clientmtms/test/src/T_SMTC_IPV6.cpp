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
// Name of test harness: T_SMTC_IPV6
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Query Capability of the SMTP client and sends plaintext email to IPV6 network
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// C:\system\data\hosts
// With globel address and name for email server, in one line.
// for example fe80::2c0:4fff:fe84:61f7 msg6 
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_SMTC\T_IMCM.log
// <DRIVE>:\msglogs\T_SMTC\Entry_Structure.txt
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_SMTC_IPV6.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_SMTC_IPV6.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_SMTC.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_SMTC.exe on the other platform
// 
//

#include "emailtestutils.h"
#include <c32comm.h>
#include <commdb.h>
#include <mtmuids.h>
#include <cemailaccounts.h>

#include "TESTENV.h"
#include <miutlog.h>
#include "SMTCMTM.H" // CSmtpClientMtm
#include "SMTPSET.H" // CImSmtpSettings

#include <mmsvattachmentmanager.h>

// local variables etc //

LOCAL_D RTest test(_L("SMTP.dll Testrig"));
LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CImLog* log;
LOCAL_D CMsvOperation* msvOperation;

LOCAL_D TMsvId smtpService;
LOCAL_D TInt testNo = 1;

#define KMsgSmtpRefreshMailboxPriority 1
#define KPeriod 10000	// period of timer

//

class TestUiTimer : public CTimer
	{
public:
	static TestUiTimer* NewLC(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation);

	void RunL();
	void DoCancel();
	void ConstructL();
	void IssueRequest();
	TPtrC Progress(TInt progressNumber);
	
	CConsoleBase*				iConsole;
	CImEmailOperation*			iEmailOperation;
	TTimeIntervalMicroSeconds32 period;
protected:
	TInt iCount;
	TBool iClosing;
	TestUiTimer(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation);
	};

//
// Progress timer
//	  
TestUiTimer* TestUiTimer::NewLC(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation)
	{
	TestUiTimer* self = new(ELeave) TestUiTimer(aConsole, aEmailOperation);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TestUiTimer::TestUiTimer(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation)
	: CTimer(EPriorityStandard+3), iEmailOperation(aEmailOperation)
	{
	iConsole = aConsole;
	period = KPeriod;
	iClosing = EFalse;
	iCount = 0;
	}

void TestUiTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void TestUiTimer::IssueRequest()
	{
	After(period);
	}

void TestUiTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

TPtrC TestUiTimer::Progress(TInt progressNumber)
	{
	switch (progressNumber)
		{
		case EConnectingToSmtp:
			return _L("Connecting              ");
		case 1:
			return _L("Waiting for reply       ");
		case 2:
			return _L("Authorising Smtp        ");
		case 3:
			return _L("Sending StartTls        ");
		case 4:
			return _L("Setting Security        ");
		case 5:
			return _L("Sending Email           ");
		case 6:
			return _L("Log Data Event          ");
		case 7:
			return _L("Closing Smtp            ");
		default:
			return _L("Unknown Progress        ");
		}
	}

void TestUiTimer::RunL()
	{
	// display the current progress
	if(iEmailOperation!=NULL)
		{
		TImSmtpProgress temp;	
		TPckgC<TImSmtpProgress> paramPack(temp);
		paramPack.Set(iEmailOperation->ProgressL());
		TImSmtpProgress progress=paramPack();	

		iConsole->SetPos(0, 12);
		TBuf<80> progressBuf = Progress(progress.iSendFileProgress.iSessionState);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Operation Progress      : %S           "), &progressBuf);
		iConsole->SetPos(0, 13);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Progress:  %d/%d %d/%d                \n"),
					progress.MsgNo() + 1,
					progress.SendTotal(),
					progress.iSendFileProgress.iBytesSent,
					progress.iSendFileProgress.iBytesSent + progress.iSendFileProgress.iBytesToSend);
		iConsole->SetPos(0, 15);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Error: %d                             \n"), progress.Error());
		}
	IssueRequest();
	};

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

	if (!iSmtpActive.IsActive())   //this is how you check if the MTM is finished or not!
		{
		iSmtpActive.Cancel();
		CActiveScheduler::Stop();
		return;
		}
	else
		After(400);
	}


LOCAL_C void TestSendingMessagesL()
	{
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	testUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	CMsvEntrySelection* sendSel = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(sendSel);
	TInt count=sendSel->Count();
	if (!count)
		{
		log->AppendComment(_L8("\t No messages to send in the outbox"));
		CleanupStack::PopAndDestroy(2); //testActive, sendsel
		return;
		}
	log->AppendError(_L8("\t messages to send in the outbox -"), count);
	testUtils->iSmtpClientMtm->SetCurrentEntryL(testUtils->iMsvSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId));
	msvOperation = testUtils->iSmtpClientMtm->Entry().CopyL(*sendSel, smtpService, testActive->iStatus);
	CleanupStack::PushL(msvOperation);
	CSmtpTimer* timer = CSmtpTimer::NewL(*testActive);
	CleanupStack::PushL(timer);

	testActive->StartL();
	timer->StartL();
	CActiveScheduler::Start();
	//check progress
	log->AppendError(_L8("\t CopyL completed with "), testActive->iStatus.Int());
 	TImSmtpProgress temp;	
	TPckgC<TImSmtpProgress> paramPack(temp);

	const TDesC8& progBuf = msvOperation->ProgressL();
	paramPack.Set(progBuf);
	TImSmtpProgress progress=paramPack();	
	log->AppendError(_L8("\t Progress error ="), progress.Error());
	CleanupStack::PopAndDestroy(4); //testActive, sendsel, timer, msvOperation
	}

// Menu Selection,Main and Init Functions //

LOCAL_C void TestCapabilityL()
	{
	TInt response = 0;
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQuerySupportedBody, response) == KErrNone);
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQuerySupportAttachments, response) == KErrNone);
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQuerySupportSubject, response) == KErrNone);
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQuerySupportsFolder, response) == KErrNotSupported);
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQueryOffLineAllowed, response) == KErrNotSupported);
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQueryCanSendMsg, response) == KErrNone);
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQueryCanReceiveMsg, response) == KErrNotSupported);
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQuerySendAsRequiresRenderedImage, response) == KErrNotSupported);
	test(testUtils->iSmtpClientMtm->QueryCapability(KUidMtmQueryMaxRecipientCount, response) == KErrNotSupported);
	}

GLDEF_C void CommInitL();

LOCAL_C void InitL()
	{
	CommInitL();
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->CreateAllTestDirectories();
	testUtils->GoClientSideL();

	smtpService = testUtils->CreateSmtpServiceL();
	testUtils->CreatePopServiceL();
	
	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("********** T_SMTC_IPV6 Test SMTP Client MTM **********"));
	TBuf8<80> buf;
#if defined(__WINS__)
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}
	
LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("**********    T_SMTC_IPV6 Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}

LOCAL_C TMsvId CreateMessageL()
	{
	testUtils->TestStart(testNo++);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	TMsvPartList partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	TMsvEmailTypeList msvEmailTypeList = 0;
	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestUiTimer* testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
	testActive->StartL();
	testUiTimer->IssueRequest();
	CActiveScheduler::Start();

	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = emailOperation->ProgressL();
	paramPack.Set(progBuf);
	TMsvId messageId=paramPack();
	TInt error = emailOperation->iStatus.Int();

	CleanupStack::PopAndDestroy(3); // testActive, emailOperation, testUiTimer
	testUtils->TestFinish(testNo-1, error);
	log->AppendComment(_L8(" Messages created in the outbox"));

	// fill in details for email message
	testUtils->iMsvEntry->SetEntryL(messageId);
	CMsvStore* store = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store);
	CImHeader* header = CImHeader::NewLC();
	header->RestoreL(*store);
	header->ToRecipients().AppendL(_L("kashifn@msg4"));
	header->SetSubjectL(_L("Message from T_SMTC_IPV6 !!"));
	header->StoreL(*store);
	store->CommitL();
 	CleanupStack::PopAndDestroy(2); //header, store

	//store some body text!!
	CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer* charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);

	TBuf<100> bodyContents = _L("Test body contents so that we can test the store body text code.");
	bodyText->InsertL(0, bodyContents);

	// store the body text
	testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	imEmailMessage->StoreBodyTextL(messageId, *bodyText, testActive->iStatus);
	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete
	CleanupStack::PopAndDestroy(2); //imEmailMessage, testActive

	// Add an attachment
	testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
//mkn start comment
	_LIT(KAttachment1, "C:\\mailtest\\IMCM\\rfc82201.txt");
	imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	testUtils->iMsvEntry->SetEntryL(messageId);
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);
	imEmailMessage->AttachmentManager().AddAttachmentL(KAttachment1,attachmentInfo,testActive->iStatus);
	CleanupStack::Pop(attachmentInfo);
	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete
//mkn end comment
	CleanupStack::PopAndDestroy(5); //testActive, paraFormatLayer, charFormatLayer, bodyText
	return messageId;
	}

LOCAL_C void OverwriteSmtpSettingsL(TBool aRequestReceipt)
	{
	testUtils->iMsvEntry->SetEntryL(smtpService);
	//overwrite the setings with test code one.  Don't want the default settings.
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	CImSmtpSettings* settings = new(ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);
	settings->Reset();
	_LIT(KSmtpServer, "msg6"); 
	// globel address for msg6 is fe80::2c0:4fff:fe84:61f7
	// link local address fe80::1111:1111:dead:beef
	settings->SetServerAddressL(KSmtpServer);
	settings->SetEmailAliasL(_L("SMTP_IPV6 Test"));
	_LIT(KSmtpEmailAddress, "<kashifn@msg4>");
	settings->SetEmailAddressL(KSmtpEmailAddress);
	settings->SetReplyToAddressL(KSmtpEmailAddress);
	settings->SetBodyEncoding(EMsgOutboxMIME);
	settings->SetReceiptAddressL(KSmtpEmailAddress);
	settings->SetRequestReceipts(aRequestReceipt);
	settings->SetPort(25);
	
	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL(smtpService, smtpAccountId);
	accounts->SaveSmtpSettingsL(smtpAccountId, *settings);
	CleanupStack::PopAndDestroy(2); //settings, store/account

	CTestTimer* timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;
	}

LOCAL_C void doMainL()
	{
	InitL();

	test.Printf(_L("\nPerforming Tests\n"));
	testUtils->iMsvEntry->SetEntryL(smtpService);

	CTestTimer* timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;

	// store service as default service
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL(smtpService, smtpAccountId);
	accounts->SetDefaultSmtpAccountL(smtpAccountId);
	CleanupStack::PopAndDestroy(accounts);

	OverwriteSmtpSettingsL(EFalse);
	CreateMessageL();

	// Create a message that will request a receipt
	OverwriteSmtpSettingsL(ETrue);
	CreateMessageL();

	testUtils->InstantiateSmtpClientMtmL();

	timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;

	testUtils->TestStart(testNo++);
	TestSendingMessagesL();
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo++);
	TestCapabilityL();
	testUtils->TestFinish(testNo-1);

	testUtils->TestHarnessCompleted();
	Closedown();
	}


GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_SMTC - Test Harness"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
