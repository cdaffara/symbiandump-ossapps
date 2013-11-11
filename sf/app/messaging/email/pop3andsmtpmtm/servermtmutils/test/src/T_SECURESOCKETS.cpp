// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Brief description of test harness:
// Tests SSL/TLS functionality of IMSK using POP commands
// Input files required to run test harness:
// The following files should be copied to <drive>:\msgtest\imut\
// scriptwithSSLTLS.scr
// scriptwitoutSSLTLS.scr
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_SECURESOCKETS.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \messaging\msg\imut\
// bldmake bldfiles
// abld test build t_SECURESOCKETS
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 2. Build the test utilities:
// cd \messaging\msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_SECURESOCKETS.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_SECURESOCKETS.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Copy \epoc32\wins\c\msgtest\imut\*.scr to
// <DRIVE>:\msgtest\imut\
// Note that this drive should be a CF card as the test files are large
// 5. Run T_SECURESOCKETS.exe on the other platform with the CF card
// 
//

#include <imsk.h>

#include <s32mem.h>
#include <s32file.h>
#include <iapprefs.h>

#include "emailtestutils.h"

TInt EBufferNotTerminated= -9999;
LOCAL_D RTest test(_L("CImTextServerSession SSL/TLS Test using scripts"));
LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D RFs theFs;	
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D CEmailTestUtils* testUtils; 

_LIT(T_SSLSCRIPT, "scriptwithSSLTLS.scr");
_LIT(T_WITOUTSSLSCRIPT, "scriptwithoutSSLTLS.scr");

#define EActivePriorityLow		-1

// create an active object to send the message

class CSendMessage : public CActive
	{
public: // functions
	// construct/destruct
	CSendMessage();
	static CSendMessage *NewL();
	static CSendMessage *NewLC();
	void ConstructL();
	~CSendMessage();
	void ChangeStateL();	
	void RunL();
	void DoCancel();
	void SetTestNumber(TInt aTestNo);

private:
	CImTextServerSession*	iImSocket;
	TInt					iSessionState;
	CImIAPPreferences*		iPrefs;
	TInt iTestNo;
	TBuf8<512>	iReceive;

private:
	enum				// States of "session" state machine
		{
		EConnecting,
		EWaitingForReply,
		ERequestingTLS,
		EAuthorisingUser,
		EAuthorisingPass,
		ESelectInbox,
		EStat,
		EDisconnect
		};

	};

CSendMessage::CSendMessage() // construct high-priority active object
	: CActive(EActivePriorityLow)
	{
	}

CSendMessage  *CSendMessage::NewLC()
	{
	CSendMessage* self=new (ELeave) CSendMessage();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSendMessage  *CSendMessage::NewL()
	{
	CSendMessage* self=NewLC();
	CleanupStack::Pop();
	return self;
	}

void CSendMessage::ConstructL()
	{
	CActiveScheduler::Add(this); // add to active scheduler
	iImSocket= CImTextServerSession::NewL();
	iSessionState=EConnecting;
	}

CSendMessage::~CSendMessage()
	{ 
	Cancel(); // make sure we're cancelled
	delete iPrefs;
	delete iImSocket;
	}

void CSendMessage::SetTestNumber(TInt aTestNo)
	{iTestNo=aTestNo;}

void  CSendMessage::DoCancel()
	{
	iImSocket->Cancel();
	}


void CSendMessage::RunL()
	{
	if (iStatus.Int()!=KErrNone)
		{
		if (iStatus.Int()==KImskSSLTLSNegotiateFailed)
			testUtils->TestFinish(iTestNo,KErrNone);
		else
			testUtils->TestFinish(iTestNo,iStatus.Int());
		CActiveScheduler::Stop();
		return;
		}
	switch (iSessionState)
	   {
	case EConnecting:
		iSessionState=EWaitingForReply;
		break;
	case EWaitingForReply:
		iSessionState=ERequestingTLS;
		break;
	case ERequestingTLS:
		iSessionState=EAuthorisingUser;
		break;
	case EAuthorisingUser:
		iSessionState=EAuthorisingPass;
		break;
	case EAuthorisingPass:
		iSessionState=EStat;
		break;
	case EDisconnect:
		testUtils->TestFinish(iTestNo,iStatus.Int());
		CActiveScheduler::Stop();
		return;
	default:
		iSessionState=EDisconnect;
		break;
	   };
	ChangeStateL();
	}


void CSendMessage::ChangeStateL()
	{
   switch (iSessionState)
	   {
	case EConnecting:
		iPrefs = CImIAPPreferences::NewLC();
		CleanupStack::Pop(iPrefs);
		iImSocket->QueueConnectL(iStatus,_L("abc.def.uk"),110,*iPrefs);
		break;
	case EWaitingForReply:
		iImSocket->QueueReceiveNextTextLine(iStatus);
		break;		
	case ERequestingTLS:
		iImSocket->SetSSLTLSResponseL(_L8("+OK"));
		iImSocket->SendQueueReceive(iStatus,_L8("STLS\r\n"));
		break;
	case EAuthorisingUser:
		iImSocket->SendQueueReceive(iStatus, _L8("USER abcd\r\n"));
		break;
	case EAuthorisingPass:
		iImSocket->SendQueueReceive(iStatus,_L8("PASS efgh\r\n"));
		break;
	case EStat:
		iImSocket->SendQueueReceive(iStatus,_L8("STAT\r\n"));
		break;
	case EDisconnect:
		iImSocket->SendQueueReceive(iStatus,_L8("QUIT\r\n"));
		break;
	default: 	
		iImSocket->Disconnect();
		break;
		}
	SetActive();
	}


LOCAL_C void CopyScriptFileL(const TDesC& aScriptFile,TInt aPortNo)
	{
	TParse parsedScriptFile;
	parsedScriptFile.Set(aScriptFile,0,0);
	if (!parsedScriptFile.DrivePresent())
		// This isn't a full path name so we need to resolve it
		{
		testUtils->ResolveFile(_L("IMUT"), aScriptFile, parsedScriptFile);
		}
	if (aPortNo==143)
		testUtils->CopyScriptFileL(parsedScriptFile.FullName(), _L("143"));
	else if (aPortNo==110)
		testUtils->CopyScriptFileL(parsedScriptFile.FullName(), _L("110"));
	else
		testUtils->CopyScriptFileL(parsedScriptFile.FullName(), _L("25"));
	}

//

LOCAL_C void Init()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install( scheduler );

	User::LeaveIfError(theFs.Connect());
	theFs.SetSessionPath(_L("C:\\"));
	}
	
LOCAL_C void Closedown()
	{
	theFs.Close();
	CleanupStack::PopAndDestroy(testUtils);
	delete scheduler;
	}



//

LOCAL_C void doMainL()
	{
	Init();
	theFs.SetSessionPath(_L("c:\\"));
	theFs.MkDir(_L("c:\\logs\\"));
	theFs.MkDir(_L("c:\\logs\\email\\"));
	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->WriteComment(_L("T_SECURESOCKETS Testing CImTextServerSesion secure socket"));

	CSendMessage *sendMessage = CSendMessage::NewLC();
	testUtils->TestStart(1,_L("testing for pop using failed secure sockets"));
	CopyScriptFileL(T_WITOUTSSLSCRIPT,110);
	sendMessage->SetTestNumber(1);
	sendMessage->ChangeStateL();
 	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(sendMessage);
	sendMessage=NULL;

	sendMessage = CSendMessage::NewLC();
	testUtils->TestStart(2,_L("testing for pop using secure sockets"));
	CopyScriptFileL(T_SSLSCRIPT,110);
	sendMessage->SetTestNumber(2);
	sendMessage->ChangeStateL();
 	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(sendMessage);

	testUtils->WriteComment(_L("**********       T_SECURESOCKETS Tests Complete       **********"));
	testUtils->TestHarnessCompleted();

	Closedown();
	}


GLDEF_C TInt E32Main()
	{
	test.Title();
	test.Start(_L("Testing CImTextServerSession Secure Sockets"));
	__UHEAP_MARK;

	theCleanup=CTrapCleanup::New();
	test (theCleanup!=NULL);
	TRAPD(ret,doMainL());		
	test (ret==KErrNone);
	delete theCleanup;

	__UHEAP_MARKEND;
	test.End();
	return 0;
	}
