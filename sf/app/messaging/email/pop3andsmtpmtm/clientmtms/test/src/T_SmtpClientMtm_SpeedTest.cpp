// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Speed test for CSmtpClientMtm::AddAttachmentL function
// 
//

#include "emailtestutils.h"

#include "TESTENV.h"
#include "SMTCMTM.H" // CSmtpClientMtm


LOCAL_D RTest test(_L("CSmtpClientMtm::AddAttachmentL Speed Test"));

_LIT(KAttachment1, "C:\\mailtest\\IMCM\\rfc82201.txt");
_LIT8(KMimeType, "text/x-vcard");

enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgServerAddress,
	EArgEmail,
	EArgEnd
	};

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D TInt testNo = 1;

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

LOCAL_C void InitL()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->WriteComment(_L("SetSessionPath"));
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->WriteComment(_L("CleanMessageFolderL"));
	testUtils->CleanMessageFolderL();
	testUtils->WriteComment(_L("learEmailAccountsL"));
	testUtils->ClearEmailAccountsL();
	testUtils->WriteComment(_L("CreateAllTestDirectories"));
	testUtils->CreateAllTestDirectories();
	testUtils->WriteComment(_L("GoClientSideL"));
	testUtils->GoClientSideL();
	testUtils->WriteComment(_L("CreateSmtpServiceL"));

	testUtils->CreateSmtpServiceL();
	testUtils->WriteComment(_L("CreatePopServiceL"));
	testUtils->CreatePopServiceL();
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2, scheduler); // scheduler, testUtils
	}

LOCAL_C void CreateMessageL()
	{	
	testUtils->TestStart(testNo++);
	testUtils->WriteComment(_L("CreateMessageL"));
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

	CleanupStack::PopAndDestroy(3, testActive); // testActive, emailOperation, testUiTimer
	testUtils->TestFinish(testNo-1, error);

	// fill in details for email message
	testUtils->iMsvEntry->SetEntryL(messageId);
	CMsvStore* store = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store);
	CImHeader* header = CImHeader::NewLC();
	header->RestoreL(*store);
	header->ToRecipients().AppendL(_L("t_smtc@lon-msgtest06.intra"));
	header->SetSubjectL(_L("Message from T_SMTC Typhoon!!"));
	header->StoreL(*store);
	store->CommitL();
 	CleanupStack::PopAndDestroy(2, store); // header, store

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
	CleanupStack::PopAndDestroy(5, paraFormatLayer); // imEmailMessage, testActive, bodyText, charFormatLayer, paraFormatLayer
	}


LOCAL_C void CreateAttachmentL()
	{
	testUtils->WriteComment(_L("CreateAttachmentL"));

	// Add an attachment
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	
	TRAPD(error, testUtils->iSmtpClientMtm->AddAttachmentL(KAttachment1(), KMimeType(), 0, testActive->iStatus));
	TBuf<100> buffer2;
	_LIT(KComment2, " Error %d\n");
	buffer2.Format(KComment2, error);
	testUtils->WriteComment(buffer2);
	
	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete
	test(testActive->iStatus.Int() == KErrNone);
	CleanupStack::PopAndDestroy(testActive);	
	}

LOCAL_C void AddAttachmentSpeedTestL(TInt aRepeat)
	{
	testUtils->WriteComment(_L("AddAttachmentSpeedTestL"));

	testUtils->iSmtpClientMtm->SetCurrentEntryL(testUtils->iMsvEntry);
	
	TTime starttime;
	TTime endtime;
	
	starttime.UniversalTime();
	for (TInt ii=0; ii<aRepeat; ++ii)
		{
		CreateAttachmentL();
		}
	endtime.UniversalTime();
	TTimeIntervalMicroSeconds elaspedtime = endtime.MicroSecondsFrom(starttime);	
		
	TInt elaspedtimeInSec = (TInt64&)elaspedtime/1000000;		
	TBuf<100> buffer;
	_LIT(KComment, " Adding %d attachments took %ds\n");
	buffer.Format(KComment, aRepeat, elaspedtimeInSec);
	testUtils->WriteComment(buffer);	

	TBuf<100> buffer2;
	_LIT(KComment2, " Average time to add one attachment is %ds\n");
	buffer2.Format(KComment2, elaspedtimeInSec/aRepeat);
	testUtils->WriteComment(buffer2);
	}

LOCAL_C void doMainL()
	{
	InitL();
	
	test.Printf(_L("\nPerforming Tests\n"));
	
	CreateMessageL();
	testUtils->InstantiateSmtpClientMtmL();
	
	testUtils->TestStart(testNo++);
	AddAttachmentSpeedTestL(100);
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
