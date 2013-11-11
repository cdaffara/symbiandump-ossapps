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
// Tests SSL/TLS functionality in IMPS
// Input files required to run test harness:
// None
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMPSSECURESOCKET.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \messaging\msg\imps\
// bldmake bldfiles
// abld test build T_IMPSSECURESOCKET
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 2. Build the test utilities:
// cd \messaging\msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_IMPSSECURESOCKET.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMPSSECURESOCKET.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 5. Run T_IMPSSECURESOCKET.exe on the other platform with the CF card
// 
//

#include "EmailTestUtils.h"
#include <miutmsg.h>
#include "impsmtm.h"
#include <securesocket.h>
#include <bacline.h>
#include <cemailaccounts.h>

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif


LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils; 
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D RFs theFs;	
LOCAL_D TMsvId	imapService;


_LIT(KImpsTest, "T_IMPSSECURESOCKET - IMPS SecureSockets Functionality Testing");

RTest test(KImpsTest);

_LIT(KTestParams,"-testparams");
enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgServerAddress,
	EArgLogin,
	EArgPassword,
	EArgEnd
	};


LOCAL_C TInt readConsoleLineBuf(const TDesC& aPrompt, TDes& aBuf)
	{
	test.Printf(_L("\n "));
	TBool quit = EFalse;
	_LIT(KDeleteChar, "\b \b");

	test.Printf(aPrompt);
	while (!quit)
		{
		TKeyCode k = test.Getch();
		TChar key = k;
		switch (key)
			{
		case EKeyBackspace:
			if (aBuf.Length() > 0)
				{
				test.Printf(KDeleteChar);
				aBuf = aBuf.Left(aBuf.Length() - 1);
				}
			break;
		case EKeyEnter:
			if (aBuf.Length())
				quit = ETrue;
			else
				test.Printf(_L("\nLogin, password and server address cannot be zero length "));
			break;
		default:
			test.Printf(_L("%c"), key);
			aBuf.Append(key);
			break;
			}
		}
	return(KErrNone);
	}



class CImpsTestHarness : public CActive
	{
public:
	static CImpsTestHarness* NewLC();
	void SetUpImpsServiceL();
	~CImpsTestHarness();
	void StartTestL();
	void EndTestL();
	void RunL();
	void DoCancel();
private:
	CImpsTestHarness();
	void ConstructL();
private:
	TInt				iTestError;
	TInt				iTestNumber;
	CImap4ServerMtm*	imapServerMtm;
	CMsvEntrySelection* iSel;
	TImap4Cmds iCommand;
	};


//

CImpsTestHarness  *CImpsTestHarness::NewLC()
	{
	CImpsTestHarness* self=new (ELeave) CImpsTestHarness();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CImpsTestHarness::~CImpsTestHarness()
	{
	delete iSel;
	}

CImpsTestHarness::CImpsTestHarness()
	: CActive(-1)
	{
	}

void CImpsTestHarness::ConstructL()
	{
	CActiveScheduler::Add(this); // add to active scheduler
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
	// Get server MTM.
	testUtils->Reset();
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
	SetUpImpsServiceL();
	testUtils->InstantiateImapServerMtmL();
	imapServerMtm=(CImap4ServerMtm*)testUtils->iImapServerMtm;
	}

void CImpsTestHarness::SetUpImpsServiceL()
	{
	// Handle command line arguments
	CCommandLineArguments* cmdLineArg=CCommandLineArguments::NewLC();
	TInt count = cmdLineArg->Count();
	TBool isCmdLine=FALSE;
	if (count>2)	// Command line arguments?
		{
		if ((!(cmdLineArg->Arg(EArgTestParams).Compare(KTestParams))) && count==EArgEnd)
			isCmdLine=TRUE;
		}	

	imapService=testUtils->CreateImapServiceL();
	testUtils->iServerEntry->SetEntry(imapService);

	CImImap4Settings* settings = new(ELeave) CImImap4Settings();
	CleanupStack::PushL(settings);
	settings->Reset();


	TBuf<100> buf;
	if (isCmdLine)
		{
		TLex lex(cmdLineArg->Arg(EArgServerAddress));
		buf=lex.NextToken();
		test.Printf(_L("Server address: %S\n"),&buf);
		}
	else
		{
		readConsoleLineBuf(_L("Server address: "),buf);
		}
	settings->SetServerAddressL(buf);
	buf.Zero();
	if (isCmdLine)
		{
		TLex lex(cmdLineArg->Arg(EArgLogin));
		buf=lex.NextToken();
		test.Printf(_L("Login: %S\n"),&buf);
		}
	else
		{
		readConsoleLineBuf(_L("login: "),buf);
		}
	TBuf8<64> data;
	data.Copy(buf);
	settings->SetLoginNameL(testUtils->MachineName());
	data.Zero();
	buf.Zero();
	if (isCmdLine)
		{
		TLex lex(cmdLineArg->Arg(EArgPassword));
		buf=lex.NextToken();
		test.Printf(_L("Password: %S\n"),&buf);
		}
	else
		{
		readConsoleLineBuf(_L("password: "),buf);
		}
	data.Copy(buf);
	settings->SetPasswordL(testUtils->MachineName());

	settings->SetFolderPathL(_L8(""));
	settings->SetPort(143);
	settings->SetPathSeparator('/');
	settings->SetDisconnectedUserMode(ETrue);
	settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
	settings->SetSecureSockets(ETrue); 
	settings->SetSubscribe(EUpdateNeither);
	settings->SetSynchronise(EUseCombination);
	
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
 	TImapAccount imapAccount;
 	accounts->GetImapAccountL(imapService, imapAccount);						
 	accounts->SaveImapSettingsL(imapAccount, *settings);
	CleanupStack::PopAndDestroy(3, settings); // accounts, settings, cmdLineArg
	}

void CImpsTestHarness::StartTestL()
	{
	testUtils->TestStart(1);
	iSel = new (ELeave) CMsvEntrySelection;
	iSel->AppendL(imapService);
	TBuf8<128> parameter;
	testUtils->WriteComment(_L("Calling KIMAP4MTMConnect ")); 
	iCommand=KIMAP4MTMConnect;
	imapServerMtm->StartCommandL(*iSel, iCommand, parameter, iStatus);
	SetActive();
	}

void CImpsTestHarness::EndTestL()
	{
	testUtils->TestFinish(1, iTestError);
	}


void  CImpsTestHarness::DoCancel()
	{
	imapServerMtm->Cancel();
	}

void CImpsTestHarness::RunL()
	{
	TImap4CompoundProgress temp;	
	TPckgC<TImap4CompoundProgress> paramPack(temp);

	const TDesC8& progBuf = imapServerMtm->Progress();	
	paramPack.Set(progBuf);
	TImap4CompoundProgress progress=paramPack();	
	TInt err=progress.iGenericProgress.iErrorCode;

	if (iStatus.Int()!=KErrNone || iCommand==KIMAP4MTMDisconnect || err)
		{
		testUtils->TestFinish(1,err);
		CActiveScheduler::Stop();
		return;
		}
	if (iCommand==KIMAP4MTMConnect)
		{
		testUtils->WriteComment(_L("Calling KIMAP4MTMInboxNewSync ")); 
		iCommand=KIMAP4MTMInboxNewSync;
		TBuf8<128> parameter;
		imapServerMtm->StartCommandL(*iSel, iCommand, parameter, iStatus);
		SetActive();
		return;
		}

	if (iCommand==KIMAP4MTMInboxNewSync)
		{
		iCommand=KIMAP4MTMDisconnect;
		TBuf8<128> parameter;
		imapServerMtm->StartCommandL(*iSel, iCommand, parameter, iStatus);
		SetActive();
		return;

		}
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
	CSecureSocketLibraryLoader::Unload();
	}


LOCAL_C void doMainL()
	{
	Init();


	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);


	theFs.SetSessionPath(_L("c:\\"));
	theFs.MkDir(_L("c:\\logs\\"));
	theFs.MkDir(_L("c:\\logs\\email\\"));
	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->WriteComment(_L("T_IMPSECURESOCKETS  - Testing IMPS secure sockets"));
	testUtils->WriteComment(_L("Before you run this test harness, please create commdb dat file ")); 
	testUtils->WriteComment(_L("with server details which supports TLS."));
	CImpsTestHarness* impsTestHarness = CImpsTestHarness::NewLC();
	impsTestHarness->StartTestL();
 	CActiveScheduler::Start();
	testUtils->TestHarnessCompleted();
	CleanupStack::PopAndDestroy(impsTestHarness); 
	Closedown();
	}


GLDEF_C TInt E32Main()
	{
	test.Title();
	test.Start(_L("Testing IMPS Secure Socket connection"));
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

