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
#include <commdb.h>
#include "TESTENV.h"
#include <imapconnectionobserver.h>
#include <bacline.h>
#include <cemailaccounts.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "cimmessagepart.h"
#endif

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

// local variables etc //

class CDummyConnectionObserver : public MMsvImapConnectionObserver
	{
		void HandleImapConnectionEvent(TImapConnectionEvent aConnectionEvent);
	};

void CDummyConnectionObserver::HandleImapConnectionEvent(TImapConnectionEvent)
	{
	}



_LIT(KImcmTest, "T_IMPC - Testing CImImap4GetMail");
RTest test(KImcmTest);

_LIT(KTestParams,"-testparams");
enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgServerAddress,
	EArgLogin,
	EArgPassword,
	EArgFolderPath,
	EArgEnd
	};


LOCAL_D TMsvId imap4Service;
LOCAL_D TMsvId smtpService;
LOCAL_D TInt globalError;

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;

LOCAL_D CMsvOperation* msvOperation;
LOCAL_D CMsvEntrySelection* msvSelection;
LOCAL_D CConsoleBase* console;

LOCAL_D CDummyConnectionObserver* dummyObserver;

class COperationActive;
LOCAL_D COperationActive* opActive;


LOCAL_D CMsvEntrySelection* messageSelection;

// Constants
//For the IR use this account
//_LIT(KImapServer, "mail.totalise.co.uk");
//_LIT8(KLoginName, "n.kothuri");
//_LIT8(KPassword, "mechoes");
//_LIT8(KFolderPath, "");


#if 1
_LIT(KImapServer, "msexchange01.closedtest.intra");
_LIT8(KFolderPath, "");

#else
_LIT(KImapServer, "msexchange01.closedtest.intra");
_LIT8(KLoginName, "test");
_LIT8(KPassword, "testtest");
_LIT8(KFolderPath, "~/mail");
#endif



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

//

LOCAL_C void ConnectAndSyncCompleteL()
	{
	testUtils->WriteComment(_L("Inside ConnectAndSyncCompleteL "));

	testUtils->WriteComment(_L("Connecting to the IMAP4 server"));

	//CleanupStack::PushL(dummyObserver);

	TPckg<MMsvImapConnectionObserver*> aParameter(dummyObserver);

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);

	opActive->iStatus = KRequestPending;
	
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndSyncCompleteAfterDisconnect, *msvSelection, aParameter, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the connect active object
	testUtils->WriteComment(_L("End of  ConnectAndSyncCompleteL "));
	}


LOCAL_C void ConnectAndSyncCompleteAfterConnect()
	{
	testUtils->WriteComment(_L("Connect And Sync Complete After Connect"));
	msvSelection->ResizeL(0);
	
	TPckg<MMsvImapConnectionObserver*> aParameter(dummyObserver);
	
	msvSelection->AppendL(imap4Service);
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndSyncCompleteAfterConnect, *msvSelection, aParameter, opActive->iStatus);
	
	//start the operationh 
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
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

LOCAL_C void SynchronizeRemoteInboxL()
	{
	testUtils->WriteComment(_L("Synchronizing Remote Inbox")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);

	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMFullSync, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the active object
	}

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

//
// Get Mail when already Connected tests
//

LOCAL_C void CopyNewMailWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Copying New Mail when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCopyNewMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy new mail active object
	}

LOCAL_C void CopyAllMailWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Copying All Mail when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCopyAllMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy all mail active object
	}

LOCAL_C void CopyMailSelectionWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Copying Mail Selection when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+2); // message in remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCopyMailSelectionWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy mail selection active object
	}

LOCAL_C void PopulateNewMessagesWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Populating new messages when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox

	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service+1; // remote inbox
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateNewMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate new mail active object
	}

LOCAL_C void PopulateAllMessagesWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Populating all messages when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service+1; // remote inbox
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateAllMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate all mail active object
	}

LOCAL_C void PopulateMessageSelectionWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Populating remote msg when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+2); // message in the remote inbox (to populate)
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service; // service
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate message selection active object
	}

//
// Connect, Get Mail and Disconnect tests
//

LOCAL_C void ConnectAndCopyNewMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Copying New Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndCopyNewMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy new mail active object
	}

LOCAL_C void ConnectAndCopyAllMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Copying All Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndCopyAllMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy all mail active object
	}

LOCAL_C void ConnectAndCopyMailSelectionAndDisconnectL(TBool aEmptyMessageSelection = EFalse)
	{
	testUtils->WriteComment(_L("Connect, Copying Mail Selection, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);

	if (!aEmptyMessageSelection)
		msvSelection->AppendL(imap4Service+2); // message in remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndCopyMailSelectionAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy all mail active object
	}

LOCAL_C void ConnectAndPopulateNewMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Populate New Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service; // service
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateNewMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the Populate new mail active object
	}

LOCAL_C void ConnectAndPopulateAllMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Populate All Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service; // service
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateAllMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the Populate all mail active object
	}

LOCAL_C void ConnectAndPopulateMailSelectionAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Populating remote msg, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+2); // message in the remote inbox (to populate)
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service; // service
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate message selection active object
	}

//
// Connect, Get Mail and Stay Online tests
//

LOCAL_C void ConnectAndCopyNewMailAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Copying New Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndCopyNewMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy new mail active object
	}

LOCAL_C void ConnectAndCopyAllMailAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Copying All Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndCopyAllMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy all mail active object
	}

LOCAL_C void ConnectAndCopyMailSelectionAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Copying Mail Selection, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+2); // message in remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndCopyMailSelectionAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy message selection active object
	}

LOCAL_C void ConnectAndPopulateNewMailAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Populate New Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service; // service
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateNewMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the Populate new mail active object
	}

LOCAL_C void ConnectAndPopulateAllMailAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Populate All Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service; // service
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateAllMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the Populate all mail active object
	}

LOCAL_C void ConnectAndPopulateMailSelectionAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Populating remote msg, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+2); // message in the remote inbox (to populate)
	
	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service; // service
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate message selection active object
	}

LOCAL_C void CreateReplyEmailMessage()
	{
	testUtils->WriteComment(_L("Creating a Reply Email Message"));
	msvSelection->ResizeL(0);
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	CleanupStack::PushL(messageSelection);
	msvSelection->AppendL(imap4Service); //destination id
	msvSelection->AppendL(messageSelection->At(1)); // message id to which the reply is to be assigned
	
	//create an object of type TImCreateMessageOptions
	TImCreateMessageOptions ImapCreateMessage;
	
	//Initialize the member variables of TImCreateMessageOptions suitably 
	ImapCreateMessage.iMsvPartList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	ImapCreateMessage.iMsvEmailTypeList = KMsvEmailTypeListMHTMLMessage;
	ImapCreateMessage.iMessageType = KUidMsgTypeSMTP;
	TPckgBuf<TImCreateMessageOptions> package(ImapCreateMessage);
	
	//set the status
	opActive->iStatus = KRequestPending;
	
	//invoke the func InvokeAsyncFunctionL()
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCreateReplyEmailMessage, *msvSelection, package, opActive->iStatus);
	
	//start the operation
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
	CleanupStack::PopAndDestroy();      //messageSelection
	testUtils->iMsvEntry->SetEntryL(imap4Service);
	}

LOCAL_C void CreateForwardEmailMessage()
	{
	testUtils->WriteComment(_L("Creating a Forward Email Message"));
	msvSelection->ResizeL(0);
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	msvSelection->AppendL(imap4Service); //destination id
	msvSelection->AppendL(messageSelection->At(1)); // message id to which the reply is to be assigned
		
	//create an object of type TImCreateMessageOptions
	TImCreateMessageOptions ImapCreateMessage;
	
	//Initialize the member variables of TImCreateMessageOptions suitably 
	ImapCreateMessage.iMsvPartList = KMsvMessagePartBody| KMsvMessagePartAttachments;;
	ImapCreateMessage.iMsvEmailTypeList = KMsvEmailTypeListMHTMLMessage;
	ImapCreateMessage.iMessageType = KUidMsgTypeSMTP;
	TPckgBuf<TImCreateMessageOptions> package(ImapCreateMessage);
	
	//set the status
	opActive->iStatus = KRequestPending;
	
	//invoke the func InvokeAsyncFunctionL()
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCreateForwardEmailMessage, *msvSelection, package, opActive->iStatus);
																	
	//start the operationh 
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
	delete messageSelection;
	}

LOCAL_C void CreateNewEmailMessage()
	{
	testUtils->WriteComment(_L("Creating a New Email Message"));
	
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
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCreateNewEmailMessage, *msvSelection, options, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the active object
	}

LOCAL_C void CreateForwardAsAttachmentEmailMessage()
	{
	testUtils->WriteComment(_L("Creating Forward As Attachment Email Message"));
	msvSelection->ResizeL(0);
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	msvSelection->AppendL(imap4Service); //destination id
	msvSelection->AppendL(messageSelection->At(1)); // message id to which the reply is to be assigned
		
	//create an object of type TImCreateMessageOptions
	TImCreateMessageOptions ImapCreateMessage;
	
	//Initialize the member variables of TImCreateMessageOptions suitably 
	ImapCreateMessage.iMsvPartList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	ImapCreateMessage.iMsvEmailTypeList = KMsvEmailTypeListMHTMLMessage;
	ImapCreateMessage.iMessageType = KUidMsgTypeSMTP;
	TPckgBuf<TImCreateMessageOptions> package(ImapCreateMessage);
	
	//set the status
	opActive->iStatus = KRequestPending;
	
	//invoke the func InvokeAsyncFunctionL()
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCreateForwardAsAttachmentEmailMessage, *msvSelection, package, opActive->iStatus);
																	
	//start the operationh 
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
	delete messageSelection;
	}

LOCAL_C void CreateReceiptEmailMessage()
	{
	testUtils->WriteComment(_L("Creating a Forward Email Message"));
	msvSelection->ResizeL(0);
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	msvSelection->AppendL(KMsvGlobalOutBoxIndexEntryId); //destination id
	msvSelection->AppendL(messageSelection->At(4)); // message id to which the reply is to be assigned
		
	//create an object of type TImCreateMessageOptions
	TImCreateMessageOptions ImapCreateMessage;
	
	//Initialize the member variables of TImCreateMessageOptions suitably 
	ImapCreateMessage.iMsvPartList = KMsvMessagePartBody;
	ImapCreateMessage.iMsvEmailTypeList = 0;
	ImapCreateMessage.iMessageType = KUidMsgTypeSMTP;
	TPckgBuf<TImCreateMessageOptions> package(ImapCreateMessage);
	
	//set the status
	opActive->iStatus = KRequestPending;
	
	//invoke the func InvokeAsyncFunctionL()
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMCreateReceiptEmailMessage, *msvSelection, package, opActive->iStatus);
																	
	//start the operationh 
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
	delete messageSelection;
	}



//

LOCAL_C void ConnectAndSyncRemoteInboxAndDisconnectL()
	{
	console->SetPos(0, 6);
	test.Printf(_L("\nPerforming Tests\n"));
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Synchronize Remote Inbox                         \n"));
	SynchronizeRemoteInboxL();
	

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();

	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("ConnectAndSyncCompletedAfterDisconnect                         \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	ConnectAndSyncCompleteL();

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();

	CActiveScheduler::Start();
	if (globalError)
		return;
	}

LOCAL_C void ConnectAndSyncRemoteInboxSyncCompleteAfterConnectAndDisconnectL()
	{
	console->SetPos(0, 6);
	test.Printf(_L("\nPerforming Tests\n"));
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Synchronize Remote Inbox                         \n"));
	SynchronizeRemoteInboxL();
	

	CActiveScheduler::Start();

/*	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();

	CActiveScheduler::Start();	*/
	
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("ConnectAndSyncCompletedAfterDisconnect                         \n"));
	//ConnectToServerL();
	//CActiveScheduler::Start();	

	ConnectAndSyncCompleteAfterConnect();

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();

	CActiveScheduler::Start();
	if (globalError)
		return;
	}


//

LOCAL_C void DeleteAndCreateServicesL()
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


	// delete services
	testUtils->iMsvEntry->SetEntryL(KMsvRootIndexEntryId);
	testUtils->iMsvEntry->DeleteL(imap4Service);
	testUtils->iMsvEntry->DeleteL(smtpService);

	// delete all messages in the Inbox			
/*	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);		
	CMsvEntrySelection* children = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(children);
	TInt total = children->Count();
	for (TInt i=0; i<total; i++)
		testUtils->iMsvEntry->DeleteL((*children)[i]);
	CleanupStack::PopAndDestroy(children);*/

	// Create Services
	smtpService = testUtils->CreateSmtpServiceL();
	imap4Service = testUtils->CreateImapServiceL();

	testUtils->iMsvEntry->SetEntryL(imap4Service);


	//overwrite the settings with test code one.  Don't want the default settings.
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	CImImap4Settings* settings = new(ELeave) CImImap4Settings();
	CleanupStack::PushL(settings);
	settings->Reset();

	if (isCmdLine)
		{
		TBuf<100> buf;
		TLex lex(cmdLineArg->Arg(EArgServerAddress));
		buf=lex.NextToken();
		settings->SetServerAddressL(buf);
		}
	else
		settings->SetServerAddressL(KImapServer);

	settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	settings->SetAcknowledgeReceipts(ETrue);
	settings->SetAutoSendOnConnect(ETrue);

	if (isCmdLine)
		{
		TBuf<100> buf;
		TBuf8<64> data;
		TLex lex(cmdLineArg->Arg(EArgLogin));
		buf=lex.NextToken();
		data.Copy(buf);
		settings->SetLoginNameL(data);
		}
	else
		settings->SetLoginNameL(testUtils->MachineName());

	if (isCmdLine)
		{
		TBuf<100> buf;
		TBuf8<64> data;
		TLex lex(cmdLineArg->Arg(EArgPassword));
		buf=lex.NextToken();
		data.Copy(buf);
		settings->SetPasswordL(data);
		}
	else
		settings->SetPasswordL(testUtils->MachineName());

	settings->SetPort(143);

	if (isCmdLine)
		{
		TBuf<100> buf;
		TBuf8<64> data;
		TLex lex(cmdLineArg->Arg(EArgFolderPath));
		buf=lex.NextToken();
		data.Copy(buf);
		settings->SetFolderPathL(data);
		}
	else
		settings->SetFolderPathL(KFolderPath);

	TImapAccount imapAccount;
	accounts->GetImapAccountL(imap4Service, imapAccount);
	accounts->SaveImapSettingsL(imapAccount, *settings);

	CleanupStack::PopAndDestroy(2, accounts); //settings, store/accounts
	CleanupStack::PopAndDestroy(cmdLineArg);	// cmdLineArg
	}



LOCAL_C void InitL()
	{
	dummyObserver = new(ELeave)CDummyConnectionObserver;
	CleanupStack::PushL(dummyObserver);
	
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
	smtpService = testUtils->CreateSmtpServiceL();
	imap4Service = testUtils->CreateImapServiceL();

	testUtils->WriteComment(_L("********** T_IMPC01 Test CImImap4GetMail class **********"));
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(3,dummyObserver);  //testUtils, scheduler, dummyObserver
	}

//

LOCAL_C TPtrC Progress(TInt progressNumber)
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
	
	testUtils->GoServerSideL();
	testUtils->CreateMessageFilesL(imap4Service, KMsvGlobalInBoxIndexEntryId, _L("c:\\mailtest\\IMCM\\"));
	
	// Delete any *.scr files in c:\logs\email directory.
	testUtils->DeleteScriptFilesFromLogsDirL();

	testUtils->GoClientSideL();
	testUtils->iMsvEntry->SetEntryL(imap4Service);

/*	CCommsDatabase* db =CCommsDatabase::NewL();
	CleanupStack::PushL(db);
	TUint32 timeoutInSeconds = 30; // (30 Seconds)
	db->SetAgentClientTimeoutL(TPtrC(DIAL_OUT_ISP),timeoutInSeconds);
	db->SetAgentRouteTimeoutL(TPtrC(DIAL_OUT_ISP),timeoutInSeconds);
// overrides don't work but this should be changed to use an IAPPrefs class
	CleanupStack::PopAndDestroy(); //db*/

	// Handle command line arguments
	CCommandLineArguments* cmdLineArg=CCommandLineArguments::NewLC();
	TInt count = cmdLineArg->Count();
	TBool isCmdLine=FALSE;
	if (count>2)	// Command line arguments?
		{
		if ((!(cmdLineArg->Arg(EArgTestParams).Compare(KTestParams))) && count==EArgEnd)
			isCmdLine=TRUE;
		}	

	//overwrite the settings with test code one.  Don't want the default settings.
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	CImImap4Settings* settings = new(ELeave) CImImap4Settings();
	CleanupStack::PushL(settings);
	settings->Reset();
	
	if (isCmdLine)
		{
		TBuf<100> buf;
		TLex lex(cmdLineArg->Arg(EArgServerAddress));
		buf=lex.NextToken();
		test.Printf(_L("Server address: %S\n"),&buf);
		settings->SetServerAddressL(buf);
		}
	else
		settings->SetServerAddressL(KImapServer);

	settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	settings->SetAcknowledgeReceipts(ETrue);
//	settings->SetAutoSendOnConnect(ETrue);

	if (isCmdLine)
		{
		TBuf<100> buf;
		TBuf8<64> data;
		TLex lex(cmdLineArg->Arg(EArgLogin));
		buf=lex.NextToken();
		test.Printf(_L("Login: %S\n"),&buf);
		data.Copy(buf);
		settings->SetLoginNameL(data);
		}
	else
		settings->SetLoginNameL(testUtils->MachineName());

	if (isCmdLine)
		{
		TBuf<100> buf;
		TBuf8<64> data;
		TLex lex(cmdLineArg->Arg(EArgPassword));
		buf=lex.NextToken();
		test.Printf(_L("Password: %S\n"),&buf);
		data.Copy(buf);
		settings->SetPasswordL(data);
		}
	else
		settings->SetPasswordL(testUtils->MachineName());

	settings->SetPort(143);

	if (isCmdLine)
		{
		TBuf<100> buf;
		TBuf8<64> data;
		TLex lex(cmdLineArg->Arg(EArgFolderPath));
		buf=lex.NextToken();
		test.Printf(_L("Folder Path: %S\n"),&buf);
		data.Copy(buf);
		settings->SetFolderPathL(data);
		}
	else
		settings->SetFolderPathL(KFolderPath);

	TImapAccount imapAccount;
	accounts->GetImapAccountL(imap4Service, imapAccount);
	accounts->SaveImapSettingsL(imapAccount, *settings);
	
	CleanupStack::PopAndDestroy(2); //settings, store
	CleanupStack::PopAndDestroy();	// cmdLineArg

	console->SetPos(0, 3);
	test.Printf(_L("Instantiating IMAP4 Client MTM"));
	testUtils->WriteComment(_L("Instantiating IMAP4 Client MTM"));
	testUtils->InstantiateImapClientMtmL();
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	//opActive = COperationActive::NewL();
	//CleanupStack::PushL(opActive);

		testUtils->TestStart(24);
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Connecting to IMAP server                        \n"));
		ConnectToServerL();
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Create a Reply Email Message   \n"));
		CreateReplyEmailMessage();
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Disconnecting from the server                    \n"));
		DisconnectFromServerL();
		CActiveScheduler::Start();

		// cleanup for next test
		CleanupStack::PopAndDestroy(2,msvSelection); // opActive, msvSelection
	//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
		testUtils->TestFinish(24,globalError);
		if (globalError)
			{
			Closedown();
			return;
			}

		DeleteAndCreateServicesL();
		testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

		msvSelection = new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(msvSelection);

		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);

		// connect, do full sync and then disconnect so that the inbox can be synced
		ConnectAndSyncRemoteInboxAndDisconnectL();
		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
	
		testUtils->TestStart(24);
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Connecting to IMAP server                        \n"));
		ConnectToServerL();
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Create a Forward Email Message   \n"));
	//	testUtils->GoServerSideL();
	//	testUtils->CreateMessageFilesL(imap4Service, KMsvGlobalInBoxIndexEntryId, _L("c:\\mailtest\\IMCM\\"));
	//	testUtils->GoClientSideL();
	//	testUtils->iMsvEntry->SetEntryL(imap4Service);
		CreateForwardEmailMessage();
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Disconnecting from the server                    \n"));
		DisconnectFromServerL();
		CActiveScheduler::Start();

		// cleanup for next test
		CleanupStack::PopAndDestroy(2,msvSelection); // opActive, msvSelection
	//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
		testUtils->TestFinish(24,globalError);
		if (globalError)
			{
			Closedown();
			return;
			}

		DeleteAndCreateServicesL();
		testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

		msvSelection = new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(msvSelection);

		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);

		// connect, do full sync and then disconnect so that the inbox can be synced
		ConnectAndSyncRemoteInboxAndDisconnectL();
		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
	
		testUtils->TestStart(25);
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Connecting to IMAP server                        \n"));
		ConnectToServerL();
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Create a New Email Message   \n"));
		CreateNewEmailMessage();
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Disconnecting from the server                    \n"));
		DisconnectFromServerL();
		CActiveScheduler::Start();

		// cleanup for next test
		CleanupStack::PopAndDestroy(2,msvSelection); // opActive, msvSelection
	//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
		testUtils->TestFinish(25,globalError);
		if (globalError)
			{
			Closedown();
			return;
			}

		DeleteAndCreateServicesL();
		testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

		msvSelection = new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(msvSelection);

		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);

		// connect, do full sync and then disconnect so that the inbox can be synced
		ConnectAndSyncRemoteInboxAndDisconnectL();
		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		
	testUtils->TestStart(26);
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Create Forward As Attachment Email Message   \n"));
	CreateForwardAsAttachmentEmailMessage();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); // opActive, msvSelection
		//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(26,globalError);
	if (globalError)
		{
		Closedown();
		return;
		}

	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
		
 	testUtils->TestStart(27);
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Connecting to IMAP server                        \n"));
		ConnectToServerL();
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Create Forward As Attachment Email Message   \n"));
		CreateReceiptEmailMessage();
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Disconnecting from the server                    \n"));
		DisconnectFromServerL();
		CActiveScheduler::Start();

		// cleanup for next test
		CleanupStack::PopAndDestroy(2,msvSelection); // opActive, msvSelection
			//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
		testUtils->TestFinish(27,globalError);
		if (globalError)
			{
			Closedown();
			return;
			}

		DeleteAndCreateServicesL();
		testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);
		msvSelection = new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(msvSelection);

		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);

		// connect, do full sync and then disconnect so that the inbox can be synced
		ConnectAndSyncRemoteInboxAndDisconnectL();
		CleanupStack::PopAndDestroy(opActive); // opActive
		if (globalError)
			{
			Closedown();
			return;
			}
		
	
	//
	//
	//  Test 1
	//
	//
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	testUtils->TestStart(1);
	// connect, do full sync and then disconnect so that the inbox can be synced
	//ConnectAndSyncRemoteInboxAndDisconnectL();
	ConnectAndSyncRemoteInboxSyncCompleteAfterConnectAndDisconnectL();
	
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, ETrue, EFalse); // replace existing file
	testUtils->TestFinish(1,globalError);
	// Copy New Mail when already connected to server
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}

	//
	//
	// Test 2
	//
	testUtils->TestStart(2);
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Copy New Mail when already connected to server   \n"));
	CopyNewMailWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); // opActive, msvSelection
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(2,globalError);
	if (globalError)
		{
		Closedown();
		return;
		}

	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

		
	//
	//
	// Test 3
	//
	//
	testUtils->TestStart(3);
	// Copy All Mail when already connected to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Copy All Mail when already connected to server   \n"));
	CopyAllMailWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(3,globalError);

  	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}


	//
	//
	// Test 4
	//
	//
	testUtils->TestStart(4);
	// Copy Mail Selection when already connected to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Copy Msg selection when already connected        \n"));
	CopyMailSelectionWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(4,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	if (globalError)
		{
		Closedown();
		return;
		}
	//
	//
	//  Test 5
	//
	//
	testUtils->TestStart(5);
	// Populate new mail in remote inbox when already connected to server
	CleanupStack::PopAndDestroy(opActive); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populating new mail when already connected       \n"));
	PopulateNewMessagesWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(5,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	//
	//
	//   Test 6
	//
	//
	testUtils->TestStart(6);
	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	// Populate all mail in remote inbox when already connected to server
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populating all mail when already connected       \n"));
	PopulateAllMessagesWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(6,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	//
	//
	//  Test 7
	//
	//
	testUtils->TestStart(7);
	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	// Populate mail selection when already connected to server
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populating msg selection when already connected  \n"));
	PopulateMessageSelectionWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(7,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);


	//
	//
	//   Test 8
	//
	//
	testUtils->TestStart(8);
	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	// Connect, Copy New Mail and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy New Mail, Disconnect               \n"));
	ConnectAndCopyNewMailAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(8,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(9);
	// Connect, Copy All Mail and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy All Mail, Disconnect               \n"));
	ConnectAndCopyAllMailAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(9,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(10);
	// Connect, Copy Mail Selection and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy Mail Selection, Disconnect         \n"));
	ConnectAndCopyMailSelectionAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(10,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(11);
	// Connect, Populate New Mail and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate New Mail, Disconnect           \n"));
	ConnectAndPopulateNewMailAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(11,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(12);
	// Connect, Populate All Mail and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate All Mail, Disconnect           \n"));
	ConnectAndPopulateAllMailAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(12,globalError);

  	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(13);
	// Connect, Populate Mail Selection and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate Mail Selection, Disconnect     \n"));
	ConnectAndPopulateMailSelectionAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(13,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(14);
	// Connect, Copy New Mail and Stay Online
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy New Mail, Stay Online              \n"));
	ConnectAndCopyNewMailAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(14,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(15);
	// Connect, Copy All Mail and Stay Online
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy All Mail, Stay Online              \n"));
	ConnectAndCopyAllMailAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(15,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(16);
	// Connect, Copy Mail Selection and Stay Online
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy Msg Selection, Stay Online         \n"));
	ConnectAndCopyMailSelectionAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(16,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(17);
	// Connect, Populate New Mail and Stay Online
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate New Mail, Stay Online          \n"));
	ConnectAndPopulateNewMailAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(17,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(18);
	// Connect, Populate All Mail and Stay Online
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate All Mail, Stay Online          \n"));
	ConnectAndPopulateAllMailAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(18,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(19);
	// Connect, Populate Mail Selection and Stay Online
	CleanupStack::PopAndDestroy(opActive); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate Mail Selection, Stay Online    \n"));
	ConnectAndPopulateMailSelectionAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(19,globalError);


//
//	Test the cancelling of operations
//
	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(20);
	// Connect, Copy All Mail and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy All Mail, Disconnect               \n"));
//	opActive->SetCancelState(TImap4GenericProgress::EConnecting);
	ConnectAndCopyAllMailAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(20,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(21);
	// Connect, Copy All Mail and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy All Mail, Disconnect               \n"));
	opActive->SetCancelState(TImap4GenericProgress::ESelecting);
	ConnectAndCopyAllMailAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(21,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(22);
	// Connect, Copy All Mail and then disconnect
	if (globalError)
		{
		Closedown();
		return;
		}
	CleanupStack::PopAndDestroy(opActive); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy All Mail, Disconnect               \n"));
	opActive->SetCancelState(TImap4GenericProgress::EFetching);
	ConnectAndCopyAllMailAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(22,globalError);
	
	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

	testUtils->TestStart(23);
	// Connect, Copy All Mail and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy All Mail, Disconnect               \n"));
//	opActive->SetCancelState(TImap4GenericProgress::EDisconnecting);
	ConnectAndCopyAllMailAndDisconnectL();
	CActiveScheduler::Start();

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(23,globalError);

//
//	Test an empty message selection (does not cause a panic!)
//

	// cleanup for next test
	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);

		
	testUtils->TestStart(24);
	// Connect, Copy All Mail and then disconnect
	CleanupStack::PopAndDestroy(opActive); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy Empty Message Selection, Disconnect\n"));
	ConnectAndCopyMailSelectionAndDisconnectL(ETrue);
	CActiveScheduler::Start();

	// check that final progress showing the correct information
	TPckgBuf<TImap4GenericProgress> progress;
	progress.Copy(opActive->iTimer->iOperation->FinalProgress());

	console->SetPos(0, 11);
	console->Printf(_L("Final Progress:                    "));
	console->SetPos(0, 12);
	TBuf<80> progressBuf = Progress(progress().iState);
	console->Printf(TRefByValue<const TDesC>_L("   Super Operation Progress: %S           "), &progressBuf);
	console->SetPos(0, 13);
	progressBuf = Progress(progress().iImap4SubStateProgress);
	console->Printf(TRefByValue<const TDesC>_L("   Sub Operation Progress  : %S           "), &progressBuf);
	console->SetPos(0, 14);
	console->Printf(TRefByValue<const TDesC>_L("   Progress:  %d/%d %d/%d  Total:%d              \n"),
				progress().iMsgsDone,
				progress().iMsgsToDo,
				progress().iBytesDone,
				progress().iBytesToDo,
				progress().iTotalSize);
	console->SetPos(0, 15);
	console->Printf(TRefByValue<const TDesC>_L("   Error: %d                             \n"), progress().iErrorCode);

	// wait 5 seconds so that user can see the final progess!
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;

	//delete all messages in inbox
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);		
		CMsvEntrySelection* children = testUtils->iMsvEntry->ChildrenL();
		CleanupStack::PushL(children);
		TInt total = children->Count();
		for (TInt i=0; i<total; i++)
			testUtils->iMsvEntry->DeleteL((*children)[i]);
		CleanupStack::PopAndDestroy(children);
		
//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(24,globalError);
	if (globalError)
		testUtils->TestHarnessFailed(globalError);
	else
		testUtils->TestHarnessCompleted();

	CleanupStack::PopAndDestroy(2,msvSelection); //opActive, msvSelection
	testUtils->Reset();
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMPC01 Test CImImap4GetMail class"));
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
