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
//

#include "EmailTestUtils.h"
#include "miutlog.h"
#include <txtrich.h>
#include <c32comm.h>

// local variables etc //

_LIT(KImcmTest, "T_IMCM04 - Testing MTM::InvokeAsyncFunctionL()");
RTest test(KImcmTest);

LOCAL_D TMsvId imap4Service;
LOCAL_D TMsvId smtpService;
LOCAL_D TMsvId pop3Service;

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;

LOCAL_D CMsvOperation* msvOperation;
LOCAL_D CMsvEntrySelection* msvSelection;
LOCAL_D CConsoleBase* console;

class COperationActive;
LOCAL_D COperationActive* opActive;

//
class TestUiTimer : public CTimer
	{
public:
	static TestUiTimer* NewLC(CConsoleBase* aConsole);
	~TestUiTimer();

	void RunL();
	void DoCancel();
	void ConstructL();
	void IssueRequest();
	void SetOperation(CMsvOperation* aOperation);
	TTimeIntervalMicroSeconds32 period;
protected:
	TestUiTimer(CConsoleBase* aConsole);
	
	CConsoleBase*		iConsole;
	CMsvOperation*		iOperation;
	};

//
TestUiTimer* TestUiTimer::NewLC(CConsoleBase* aConsole)
	{
	TestUiTimer* self = new(ELeave) TestUiTimer(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TestUiTimer::TestUiTimer(CConsoleBase* aConsole)
	: CTimer(1)
	{
	iConsole = aConsole;
	period = 10000;
	}

TestUiTimer::~TestUiTimer()
	{
	delete iOperation;
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

void TestUiTimer::SetOperation(CMsvOperation* aOperation)
	{
	if(iOperation)
		{
		delete iOperation;
		iOperation=NULL;
		}
	iOperation = aOperation;
	}

void TestUiTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

void TestUiTimer::RunL()
	{
	IssueRequest();
	};

//

class COperationActive : public CActive
	{
public:
	COperationActive();
	~COperationActive();
	static COperationActive* NewL();
	void StartL();
	void SetOperation(CMsvOperation*);
	
protected:
	void DoCancel();
	void RunL();
	
	TestUiTimer* iTimer;
	};

//
COperationActive::COperationActive()
: CActive(0)
	{
	}

COperationActive::~COperationActive()
	{
	Cancel();
	delete iTimer;
	}

COperationActive* COperationActive::NewL()
	{
	COperationActive* self = new (ELeave) COperationActive;
	CActiveScheduler::Add(self);

	self->iSetActive = ETrue;
	self->iTimer = TestUiTimer::NewLC(test.Console());
	CleanupStack::Pop();
	return self;
	}

void COperationActive::DoCancel()
	{
	}

void COperationActive::StartL()
	{
	iTimer->IssueRequest();	// Start the connect observation timer

	SetActive();
	test.Console()->SetPos(0, 17);
	test.Printf(TRefByValue<const TDesC>_L("Operation TRequestStatus %d"), iStatus);
	}

void COperationActive::SetOperation(CMsvOperation *aOperation)
	{
	iTimer->SetOperation(aOperation);
	}

void COperationActive::RunL() 
	{
	iTimer->Cancel();
	test.Console()->SetPos(25, 17);
	test.Printf(_L("                "));
	test.Console()->SetPos(0, 17);
	test.Printf(TRefByValue<const TDesC>_L("Operation TRequestStatus %d"), iStatus);
	CActiveScheduler::Stop();
	}

//

LOCAL_C void CreateNewPlaintextMessageViaPop3MtmL()
	{
	log->AppendComment(_L8("Create new plaintext msg via POP3 MTM"));
	
	TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
	TMsvEmailTypeList msvEmailTypeList = 0;
	
	TImCreateMessageOptions msgOptions;
	msgOptions.iMessageType = KUidMsgTypeSMTP;
	msgOptions.iMsvPartList = partList;
	msgOptions.iMsvEmailTypeList = msvEmailTypeList;
	TPckgBuf<TImCreateMessageOptions> options(msgOptions);

	msvSelection->ResizeL(0);
	msvSelection->AppendL(KMsvGlobalOutBoxIndexEntryId); // destination folder is Global Outbox
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMCreateNewEmailMessage, *msvSelection, options, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the active object
	}

//

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	console = test.Console();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	// Create Services
	smtpService = testUtils->CreateSmtpServiceL();
	pop3Service = testUtils->CreatePopServiceL();
	imap4Service = testUtils->CreateImapServiceL();

	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("**** T_IMCM04 - Testing MTM::InvokeAsyncFunctionL() ****"));
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
	log->AppendComment(_L8("**********    T_IMCM04 Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}

//

LOCAL_C void doMainL()
	{
	InitL();
	testUtils->FileSession().SetSessionPath(_L("c:\\"));

	// Instantiate the POP3 Client MTM
	testUtils->GoClientSideL();
	testUtils->InstantiatePopClientMtmL();
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Creating new plaintext message via POP3 MTM       \n"));
	CreateNewPlaintextMessageViaPop3MtmL();
	CActiveScheduler::Start();
	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse);

	CleanupStack::PopAndDestroy(); //opActive
	CleanupStack::PopAndDestroy(); //msvSelection
	testUtils->Reset();
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCM04 - Testing MTM::InvokeAsyncFunctionL()"));
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
