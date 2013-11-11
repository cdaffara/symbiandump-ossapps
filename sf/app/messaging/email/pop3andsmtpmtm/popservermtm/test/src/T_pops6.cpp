// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Generic POPS Test Harness
// 
//


#include "emailtestutils.h"
#include "scripttestutils.h"
#include <miutlog.h>
#include <miutmsg.h>
#include <txtrich.h>
#include <popsmtm.h>
#include "T_mtmcmd.h"
#include <iapprefs.h>
#include <securesocket.h>
#include <commsdattypesv1_1.h>
#include <commsdat.h>
using namespace CommsDat;
#include <cdbpreftable.h>
#include <bacline.h>
#include <cemailaccounts.h>
#include <securesocket_internal.h>

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

LOCAL_D CTrapCleanup* theCleanup;

_LIT(POPS_SCRIPT_FILE_NAME, "c:\\logs\\email\\imsk110.scr");


// local variables etc //

_LIT(KImcmTest, "T_POPS6 - Generic POPS Functionality Testing");
RTest test(KImcmTest);

_LIT(KTestParams,"-testparams");
enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgServerAddress,
	EArgLogin,
	EArgPassword,
	EArgUseApop,
	EArgUseSecureSockets,
	EArgEnd
	};


class CPopsTestHarness : public CBase
	{
public:
	static CPopsTestHarness* NewLC(CEmailTestUtils& aTestUtils);
	void InitL();
	void ResetL();
	void SetUpPopServiceL();
	~CPopsTestHarness();

	void StartTestL(TInt aTestNumber, CMtmTestHarness& aTestCommands);
	void EndTestL();
	void EndTestHarnessL();

private:
	CPopsTestHarness(CEmailTestUtils& aTestUtils);
	void ConstructL();

	void CreateTestHarnessesL();
	void ResetMtmL();

public:
	// These should be generated on request in future releases
	// of this test harness.
	CMtmTestHarness*	iConnect;
	CMtmTestHarness*	iConnectAndQuit;
	CMtmTestHarness*	iConnectAndCopyAll;
	CMtmTestHarness*	iConnectAndPopulateAll;
	CMtmTestHarness*	iCopyAllWithoutConnect;
	CMtmTestHarness*	iCopyAllOffline;
	CMtmTestHarness*	iOfflineDelete;
	CMtmTestHarness*	iCancelOfflineDeleteOnline;
	CMtmTestHarness*	iCancelOfflineDelete;
	CMtmTestHarness*	iConnectAndMoveAll;
	CMtmTestHarness*	iMoveAll;
	CMtmTestHarness*	iConnectAndTopPopulateAll;


private:
	TInt				iTestError;
	TInt				iTestNumber;
	TInt				iGlobalError;

	TMsvId				iPop3Service;
	TMsvId				iSmtpService;
	TMsvId				iNewMessageId;

	CEmailTestUtils&	iTestUtils;
	};

//
LOCAL_C TBool GetTrueOrFalse()
	{
	TKeyCode k = test.Getch();
	TChar key = k;
	test.Printf(_L("%c"), key);
	switch (key)
		{
		case 'Y':
		case 'y':
			return ETrue;
		default:
			return EFalse;
		};
	}


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
//

CPopsTestHarness* CPopsTestHarness::NewLC(CEmailTestUtils& aTestUtils)
	{
	CPopsTestHarness* self = new (ELeave) CPopsTestHarness(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//

void CPopsTestHarness::ResetMtmL()
	{
	// Get server MTM.
	iTestUtils.Reset();
	iTestUtils.CleanMessageFolderL();
	iTestUtils.GoServerSideL();
	SetUpPopServiceL();
	iTestUtils.InstantiatePopServerMtmL();
	}

//

void CPopsTestHarness::ResetL()
	{
	ResetMtmL();
	CreateTestHarnessesL();
	}

//

void CPopsTestHarness::StartTestL(TInt aTestNumber, CMtmTestHarness& aTestCommands)
	{
	iTestNumber = aTestNumber;
	iTestError = KErrNone;
	iTestUtils.TestStart(aTestNumber);
	// Run the commands.
	iTestError=aTestCommands.StartL(_L("T_POPS6 - POPS Test Harness"));
//	TInt err = error(iTestUtils->iPopServerMtm->Progress());
}

//

void CPopsTestHarness::EndTestL()
	{
	if ((iTestError != KErrNone) && (iGlobalError == KErrNone))
		{
		iGlobalError = iTestError;
		}

	iTestUtils.TestFinish(iTestNumber, iTestError);
	}

//

void CPopsTestHarness::EndTestHarnessL()
	{
	if (iGlobalError == KErrNone)
		{
		iTestUtils.TestHarnessCompleted();
		}
	else
		{
		iTestUtils.TestHarnessFailed(iGlobalError);
		}
	}


void CPopsTestHarness::InitL()
	{
	iTestUtils.CreateAllTestDirectories();
	iTestUtils.FileSession().SetSessionPath(_L("C:\\"));
	iTestUtils.CleanMessageFolderL();
	iTestUtils.GoServerSideL();
	
	test.Next(_L("Create Data Component FileStores"));

	iTestUtils.FileSession().SetSessionPath(_L("c:\\"));
	}

//

void CPopsTestHarness::ConstructL()
	{
	InitL();
	}

//

CPopsTestHarness::~CPopsTestHarness()
	{
	delete iConnect;
	delete iConnectAndCopyAll;
	delete iConnectAndPopulateAll;
	delete iConnectAndMoveAll;
	delete iMoveAll;
	delete iCopyAllWithoutConnect;
	delete iCopyAllOffline;
	delete iConnectAndQuit;
	delete iOfflineDelete;
	delete iCancelOfflineDeleteOnline;
	delete iCancelOfflineDelete;
	delete iConnectAndTopPopulateAll;
	}

//

CPopsTestHarness::CPopsTestHarness(CEmailTestUtils& aTestUtils) : iGlobalError(KErrNone), iTestUtils(aTestUtils)
	{}

//

void CPopsTestHarness::SetUpPopServiceL()
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

	// Create the POP3 service and set the settings for this test.
	iPop3Service = iTestUtils.CreatePopServiceL();
	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
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
	settings->SetLoginNameL(iTestUtils.MachineName());
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
	settings->SetPasswordL(iTestUtils.MachineName());
	settings->SetPort(110);
	settings->SetDisconnectedUserMode(ETrue); // eh xxxx, must test this when set to EFalse
	TChar choice;
	if (isCmdLine)
		{
		TLex lex(cmdLineArg->Arg(EArgUseApop));
		choice=lex.Get();
		test.Printf(_L("Use APOP: %c\n"),choice);
		if (choice=='y' || choice=='Y')
			settings->SetApop(ETrue);
		else
			settings->SetApop(EFalse);
		}
	else
		{
		test.Printf(_L("\n Use APOP? Enter (Y/N): "));
		settings->SetApop(GetTrueOrFalse());
		}
	if (isCmdLine)
		{
		TLex lex(cmdLineArg->Arg(EArgUseSecureSockets));
		choice=lex.Get();
		test.Printf(_L("Use secure sockets: %c\n"),choice);
		if (choice=='y' || choice=='Y')
			settings->SetSecureSockets(ETrue);
		else
			settings->SetSecureSockets(EFalse);
		}
	else
		{
		test.Printf(_L("\n Use secure sockets? Enter (Y/N): "));
		settings->SetSecureSockets(GetTrueOrFalse());
		}
	
	// Add IAP's to iIAPPreferences
	TImIAPChoice iap;
	CImIAPPreferences* prefs = CImIAPPreferences::NewLC();	
	TInt iapId = 0;
	CMDBSession* iDBSession = CMDBSession::NewL(CMDBSession::LatestVersion());
	CCDConnectionPrefsRecord *connPrefRecord = static_cast<CCDConnectionPrefsRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdConnectionPrefsRecord));
	CleanupStack::PushL(connPrefRecord);
	connPrefRecord->iDirection = ECommDbConnectionDirectionOutgoing;
	if(!connPrefRecord->FindL(*iDBSession))
		{
		User::Leave(KErrNotFound);					
		}	
	iapId = iap.iIAP = connPrefRecord->iDefaultIAP;
	
	CleanupStack::PopAndDestroy(connPrefRecord);	
	delete iDBSession;
	iDBSession= NULL;

	iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	prefs->AddIAPL(iap);
	iap.iIAP = iapId;
	iap.iDialogPref = ECommDbDialogPrefPrompt;
	prefs->AddIAPL(iap, 1);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
 	TPopAccount popAccount;
 	accounts->GetPopAccountL(iPop3Service, popAccount);						
 	accounts->SavePopSettingsL(popAccount, *settings);
	accounts->SavePopIapSettingsL(popAccount, *prefs);
	CleanupStack::PopAndDestroy(4, cmdLineArg); // (accounts,) prefs, settings, (store,) cmdLineArg
	test.Printf(_L("\nPerforming Tests\n"));
	}

//

void CPopsTestHarness::CreateTestHarnessesL()
	{
	CMsvEntrySelection* serviceSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(serviceSelection);
	serviceSelection->AppendL(iPop3Service);
	TBuf8<128> parameter;

	delete iConnectAndCopyAll;
	iConnectAndCopyAll = NULL;
	iConnectAndCopyAll = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Connect
	iConnectAndCopyAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Copy all messages under the service to the local inbox.
	CMtmTestCommand* copyToLocal = CMtmTestCopyToLocal::NewL(*(iTestUtils.iPopServerMtm), 0, KMsvGlobalInBoxIndexEntryId);
	iConnectAndCopyAll->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), copyToLocal, iPop3Service, *(iTestUtils.iServerEntry)));
	// Quit
	iConnectAndCopyAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	delete iConnectAndPopulateAll;
	iConnectAndPopulateAll = NULL;
	iConnectAndPopulateAll = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Connect
	iConnectAndPopulateAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Copy all messages under the service to the local inbox.
	copyToLocal = CMtmTestCopyWithinService::NewL(*(iTestUtils.iPopServerMtm), 0, iPop3Service);
	iConnectAndPopulateAll->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), copyToLocal, iPop3Service, *(iTestUtils.iServerEntry)));
	// Quit
	iConnectAndPopulateAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	delete iConnectAndMoveAll;
	iConnectAndMoveAll = NULL;
	iConnectAndMoveAll = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Connect
	iConnectAndMoveAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Move all messages under the service to the local inbox.
	CMtmTestCommand* moveToLocal = CMtmTestMoveToLocal::NewL(*(iTestUtils.iPopServerMtm), 0, KMsvGlobalInBoxIndexEntryId);
	iConnectAndMoveAll->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), moveToLocal, iPop3Service, *(iTestUtils.iServerEntry)));
	// Quit
	iConnectAndMoveAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	delete iMoveAll;
	iMoveAll = NULL;
	iMoveAll = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Move all messages under the service to the local inbox.
	moveToLocal = CMtmTestMoveToLocal::NewL(*(iTestUtils.iPopServerMtm), 0, KMsvGlobalInBoxIndexEntryId);
	iMoveAll->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), moveToLocal, iPop3Service, *(iTestUtils.iServerEntry)));

	delete iConnect;
	iConnect = NULL;
	iConnect = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Connect
	iConnect->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));

	delete iConnectAndQuit;
	iConnectAndQuit = NULL;
	iConnectAndQuit = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Connect
	iConnectAndQuit->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Quit
	iConnectAndQuit->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	delete iCopyAllWithoutConnect;
	iCopyAllWithoutConnect = NULL;
	iCopyAllWithoutConnect = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Copy all messages under the service to the local inbox.
	copyToLocal = CMtmTestCopyToLocal::NewL(*(iTestUtils.iPopServerMtm), 0, KMsvGlobalInBoxIndexEntryId);
	iCopyAllWithoutConnect->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), copyToLocal, iPop3Service, *(iTestUtils.iServerEntry)));
	// Quit
	iCopyAllWithoutConnect->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	delete iCopyAllOffline;
	iCopyAllOffline = NULL;
	iCopyAllOffline = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Copy all messages under the service to the local inbox.
	copyToLocal = CMtmTestCopyToLocal::NewL(*(iTestUtils.iPopServerMtm), 0, KMsvGlobalInBoxIndexEntryId);
	iCopyAllOffline->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), copyToLocal, iPop3Service, *(iTestUtils.iServerEntry)));
	// Connect
	iCopyAllOffline->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Quit
	iCopyAllOffline->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	delete iOfflineDelete;
	iOfflineDelete = NULL;
	iOfflineDelete = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Connect
	iOfflineDelete->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Quit
	iOfflineDelete->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));
	// Delete all the entries
	CMtmTestDeleteAll* deleteAll = CMtmTestDeleteAll::NewL(*(iTestUtils.iPopServerMtm), 0);
	iOfflineDelete->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), deleteAll, iPop3Service, *(iTestUtils.iServerEntry)));

	delete iCancelOfflineDeleteOnline;
	iCancelOfflineDeleteOnline = NULL;
	iCancelOfflineDeleteOnline = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Connect
	iCancelOfflineDeleteOnline->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Cancel offline delete
	CMtmTestStartCommand* cancelAllDeletes = CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), 0, KPOP3MTMCancelOfflineOperations, parameter);
	iCancelOfflineDeleteOnline->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), cancelAllDeletes, iPop3Service, *(iTestUtils.iServerEntry)));
	// Quit
	iCancelOfflineDeleteOnline->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	delete iCancelOfflineDelete;
	iCancelOfflineDelete = NULL;
	iCancelOfflineDelete = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Cancel offline delete
	cancelAllDeletes = CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), 0, KPOP3MTMCancelOfflineOperations, parameter);
	iCancelOfflineDelete->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), cancelAllDeletes, iPop3Service, *(iTestUtils.iServerEntry)));

	// Top Test addition
	delete iConnectAndTopPopulateAll;
	iConnectAndTopPopulateAll = NULL;
	iConnectAndTopPopulateAll = CMtmTestHarness::NewL(*(iTestUtils.iPopServerMtm), iTestUtils);
	// Connect
	iConnectAndTopPopulateAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMConnect, parameter));
	// Copy all messages under the service to the local inbox.
	copyToLocal = CMtmTestTopPopulate::NewL(*(iTestUtils.iPopServerMtm), 0, iPop3Service);
	iConnectAndTopPopulateAll->AddCommandL(CMtmTestCommandOnAllMessages::NewL(*(iTestUtils.iPopServerMtm), copyToLocal, iPop3Service, *(iTestUtils.iServerEntry)));
	// Quit
	iConnectAndTopPopulateAll->AddCommandL(CMtmTestStartCommand::NewL(*(iTestUtils.iPopServerMtm), serviceSelection, KPOP3MTMDisconnect, parameter));

	CleanupStack::PopAndDestroy(); // serviceSelection
	}


//

LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler;
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);


	CEmailTestUtils* testUtils = CEmailTestUtils::NewLC(test);

	testUtils->FileSession().Delete(POPS_SCRIPT_FILE_NAME);
	
	CPopsTestHarness* popsTestHarness = CPopsTestHarness::NewLC(*testUtils);

//	popsTestHarness->SetUpPopServiceL();

	testUtils->FileSession().Delete(_L("c:\\logs\\email\\imsk110.scr"));

// RUN THE TESTS 

	popsTestHarness->ResetL();
	popsTestHarness->StartTestL(1, *(popsTestHarness->iConnectAndPopulateAll));
	popsTestHarness->EndTestL();

	// Wait a bit
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(10000000);
	CActiveScheduler::Start();
	delete timer;


	popsTestHarness->EndTestHarnessL();
	CSecureSocketLibraryLoader::Unload();

	CleanupStack::PopAndDestroy(3); // popsTestHarness, testUtils, scheduler
	}


//

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_POPS6 Test, General POPS Tests"));
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

