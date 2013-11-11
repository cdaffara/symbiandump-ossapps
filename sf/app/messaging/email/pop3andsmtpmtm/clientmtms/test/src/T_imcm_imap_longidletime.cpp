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
// Name of test harness: T_IMPC01
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests CImImap4GetMail - tests combination of copy/move/populate 
// new/all/selection when the user is already connected; connects, does 
// operation, disconnects; connects, does operation and stays online.
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMPC01.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMPC01.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMPC01.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMPC01.exe on the other platform
// 
//

#include "emailtestutils.h"
#include <txtrich.h>
#include <commsdattypesv1_1.h>
#include <commsdat.h>
using namespace CommsDat;
#include "TESTENV.h"
#include <d32comm.h>
#include <cemailaccounts.h>



//#include "ImapConnectionObserver.h"	//	MMsvImapConnectionObserver
#include <imapconnectionobserver.h>
#include <cdbover.h>

//#include <cdbstd.h>
// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

// local variables etc //

_LIT(KImcmTest, "T_IMPC - Testing CImImap4GetMail");
RTest test(KImcmTest);

LOCAL_D TMsvId imap4Service;
LOCAL_D TInt globalError;

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;

LOCAL_D CMsvOperation* msvOperation;
LOCAL_D CMsvEntrySelection* msvSelection;
LOCAL_D CConsoleBase* console;

class COperationActive;
LOCAL_D COperationActive* opActive;

/*_LIT(KImapServer, "trmai02.it.cellulardata.com");
_LIT8(KLoginName, "kamineni");
_LIT8(KPassword, "vamSym");
_LIT8(KFolderPath, "");*/

_LIT(KImapServer, "msexchange01.closedtest.intra");
_LIT8(KLoginName, "n.kothuri");
_LIT8(KPassword, "mechoes");

// Constants
/*#if 1
_LIT(KImapServer, "imap.dial.pipex.com");
_LIT8(KLoginName, "melz64");
_LIT8(KPassword, "ajilahaz");
_LIT8(KFolderPath, "");

#else
_LIT(KImapServer, "utter.chaos.org.uk");
_LIT8(KLoginName, "test");
_LIT8(KPassword, "testtest");
_LIT8(KFolderPath, "~/mail");
#endif*/

class CDummyConnectionObserver : public MMsvImapConnectionObserver
	{
		void HandleImapConnectionEvent(TImapConnectionEvent aConnectionEvent);
	};

void CDummyConnectionObserver::HandleImapConnectionEvent(TImapConnectionEvent)
	{
	}

LOCAL_D CDummyConnectionObserver *dummyObserver = new(ELeave)CDummyConnectionObserver;
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
	TPtrC Progress(TInt progressNumber);
	void SetCancelState(TInt aState);
protected:
	TestUiTimer(CConsoleBase* aConsole);
	
	CConsoleBase*		iConsole;
public:
	CMsvOperation*		iOperation;
private:
	TInt	iCancelState;
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
	: CTimer(1), iCancelState(KMaxTInt)
	{
	iConsole = aConsole;
	period = 1000;//1 millisecond
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

TPtrC TestUiTimer::Progress(TInt progressNumber)
	{
	switch (progressNumber)
		{
		case 0:
			return _L("Disconnected              ");
		case 1:
			return _L("Connecting                ");
		case 2:
			return _L("Idle                      ");
		case 3:
			return _L("Selecting                 ");
		case 4:
			return _L("Fetching                  ");
		case 5:
			return _L("Appending                 ");
		case 6:
			return _L("Copying                   ");
		case 7:
			return _L("Deleting                  ");
		case 8:
			return _L("Syncing                   ");
		case 9:
			return _L("Disconnecting             ");
		case 10:
			return _L("Busy                      ");
		case 11:
			return _L("Moving                    ");
		case 12:
			return _L("Copying New Mail          ");
		case 13:
			return _L("Moving New Mail           ");
		case 14:
			return _L("Copying Mail Selection    ");
		case 15:
			return _L("Moving Mail Selection     ");
		case 16:
			return _L("Copying All Mail          ");
		case 17:
			return _L("Moving All Mail           ");
		case 18:
			return _L("Populating New Mail       ");
		case 19:
			return _L("Populating All Mail       ");
		case 20:
			return _L("Populating Mail Selection ");
		default:
			return _L("Unknown Progress          ");
		}
	}

void TestUiTimer::SetCancelState(TInt aState)
	{
	iCancelState = aState;
	}

void TestUiTimer::RunL()
	{
	// display the current progress
	if(iOperation!=NULL)
		{
		TImap4GenericProgress temp;	
		TPckgC<TImap4GenericProgress> paramPack(temp);
		paramPack.Set(iOperation->ProgressL());
		TImap4GenericProgress progress=paramPack();	

		if (iCancelState == progress.iImap4SubStateProgress)
			{
			iConsole->Printf(_L("	Calling Cancel\n"));
			iOperation->Cancel();
			}

		iConsole->SetPos(0, 12);
		TBuf<80> progressBuf = Progress(progress.iState);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Super Operation Progress: %S           "), &progressBuf);
		iConsole->SetPos(0, 13);
		progressBuf = Progress(progress.iImap4SubStateProgress);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Sub Operation Progress  : %S           "), &progressBuf);
		iConsole->SetPos(0, 14);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Progress:  %d/%d %d/%d Total:%d             \n"),
					progress.iMsgsDone,
					progress.iMsgsToDo,
					progress.iBytesDone,
					progress.iBytesToDo,
					progress.iTotalSize);

		iConsole->SetPos(0, 15);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Error: %d                             \n"), progress.iErrorCode);
		if (!globalError && progress.iErrorCode)
			globalError=progress.iErrorCode;
		}
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
	void SetCancelState(TInt aCancelState);
	
protected:
	void DoCancel();
	void RunL();
public:	
	TestUiTimer* iTimer;
private:
	TBool iSetActive;
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
	iTimer->DoCancel();
	}

void COperationActive::StartL()
	{
	globalError=0;
	iTimer->IssueRequest();	// Start the connect observation timer
	SetActive();
	test.Console()->SetPos(0, 17);
	test.Printf(TRefByValue<const TDesC>_L("Operation TRequestStatus %d"), iStatus);
	}

void COperationActive::SetOperation(CMsvOperation *aOperation)
	{
	iTimer->SetOperation(aOperation);
	}

void COperationActive::SetCancelState(TInt aCancelState)
	{
	iTimer->SetCancelState(aCancelState);
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





LOCAL_C void ConnectAndSyncCompleteL()
	{
	testUtils->WriteComment(_L("Connecting to the IMAP4 server"));
	
	TPckg<MMsvImapConnectionObserver*> aParameter(dummyObserver);

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndSyncCompleteAfterDisconnect, *msvSelection, aParameter, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the connect active object

	}

/*
LOCAL_C void NewSyncRemoteInbox()
	{
	testUtils->WriteComment(_L("New Sync of Remote Inbox")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);

	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMInboxNewSync, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the active object
	}
*/

LOCAL_C void DisconnectFromServerL()
	{
	testUtils->WriteComment(_L("Disconnecting from the IMAP4 server"));
	TBuf8<1> aParameter;
	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect, *msvSelection, aParameter, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the disconnect active object
	}

LOCAL_C void ConnectToServerL()
	{
	testUtils->WriteComment(_L("Connecting to the IMAP4 server"));
	TBuf8<1> aParameter;
	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	
	opActive->iStatus = KRequestPending;
	
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnect, *msvSelection, aParameter, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the connect active object
	}


LOCAL_C void ConnectAndDisconnectTwiceL()
	{
	console->SetPos(0, 7);
	test.Printf(_L("Reading timeout values from Commdb                     \n"));

	TUint32 current_socket_activity_tout;
	TUint32 current_session_closed_tout;
	TUint32 current_socket_closed_tout;
	CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(dbSession);		
	CMDBRecordSet<CCDModemBearerRecord>* modemBearerRecord = new(ELeave) CMDBRecordSet<CCDModemBearerRecord>(KCDTIdModemBearerRecord);
	CleanupStack::PushL(modemBearerRecord);
	modemBearerRecord->LoadL(*dbSession);
	current_socket_activity_tout = ((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSocketActivityTimeout;
	current_session_closed_tout = ((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSessionClosedTimeout;
	current_socket_closed_tout = ((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSocketClosedTimeout;
	
	
	//Setting the idletimeout values for short duration
	console->SetPos(0, 7);
	test.Printf(_L("Setting timeout values in Commdb                       \n"));
	((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSocketActivityTimeout = (TUint32)3600;
	((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSessionClosedTimeout = (TUint32)3600;
	((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSocketClosedTimeout = (TUint32)3600;
	((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->ModifyL(*dbSession);		

	ConnectToServerL();
	CActiveScheduler::Start();

	// Connetcing to server and syncing the folders for shorter idletimeout
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                   \n"));
	ConnectAndSyncCompleteL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		return;

	//disconnecting from server

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		return;

	console->SetPos(0, 7);
	test.Printf(_L("Restoring timeout values to Commdb                     \n"));

	((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSocketActivityTimeout = current_socket_activity_tout;
	((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSessionClosedTimeout = current_session_closed_tout;
	((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->iLastSocketClosedTimeout = current_socket_closed_tout;
	((CCDModemBearerRecord*)modemBearerRecord->iRecords[0])->ModifyL(*dbSession);	
	CleanupStack::PopAndDestroy(2); //dbSession, modemBearerRecord 

	console->SetPos(0, 7);
	test.Printf(_L("Waiting for 5 seconds                                  \n"));
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;
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
	testUtils->ClearEmailAccountsL();
	
	testUtils->GoClientSideL();
	
	console->SetPos(0, 3);
	test.Printf(_L("Create Services                        "));
	testUtils->WriteComment(_L("Create Services                        "));
	// Create Services
	imap4Service = testUtils->CreateImapServiceL();

	testUtils->WriteComment(_L("********** T_IMCM_IMAP_LONGIDLETIME Test CImapConnectAndSyncOp class **********"));
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler
	}



LOCAL_C void doMainL()
	{

	InitL();
	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	testUtils->FileSession().SetSessionPath(_L("c:\\"));
		// Delete any *.scr files in c:\logs\email directory.
	testUtils->DeleteScriptFilesFromLogsDirL();

	testUtils->iMsvEntry->SetEntryL(imap4Service);

/*	CCommsDatabase* db =CCommsDatabase::NewL();
	CleanupStack::PushL(db);
	TUint32 timeoutInSeconds = 30; // (30 Seconds)
	db->SetAgentClientTimeoutL(TPtrC(LAST_SOCKET_ACTIVITY_TIMEOUT),timeoutInSeconds);
	db->SetAgentRouteTimeoutL(TPtrC(DIAL_OUT_ISP),timeoutInSeconds);
// overrides don't work but this should be changed to use an IAPPrefs class
	CleanupStack::PopAndDestroy(); //db*/

	//overwrite the settings with test code one.  Don't want the default settings.
	CImImap4Settings* settings = new(ELeave) CImImap4Settings();
	CleanupStack::PushL(settings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	TImapAccount imapAccount;
	accounts->GetImapAccountL(imap4Service, imapAccount);
	accounts->LoadImapSettingsL(imapAccount, *settings);
	settings->SetServerAddressL(KImapServer);
	settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	settings->SetAcknowledgeReceipts(ETrue);
//	settings->SetAutoSendOnConnect(ETrue);
	settings->SetLoginNameL(testUtils->MachineName());
	settings->SetPasswordL(testUtils->MachineName());
	settings->SetPort(143);
	accounts->SaveImapSettingsL(imapAccount, *settings);
	CleanupStack::PopAndDestroy(2, settings); // store/accounts, settings


	console->SetPos(0, 3);
	test.Printf(_L("Instantiating IMAP4 Client MTM"));
	testUtils->WriteComment(_L("Instantiating IMAP4 Client MTM"));
	testUtils->InstantiateImapClientMtmL();
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	//
	//
	//  Test 1
	//
	//

	testUtils->TestStart(1);


	// connect and disconnect with short and long timeout values
	ConnectAndDisconnectTwiceL();
	testUtils->TestFinish(1,globalError);



	if (globalError)
		testUtils->TestHarnessFailed(globalError);
	else
		testUtils->TestHarnessCompleted();

	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	delete dummyObserver;
	testUtils->Reset();
	Closedown();
 
	

	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCM_IMAP_LONGIDLETIME Test CImapConnectAndSyncOp class"));
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
