// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_PARTIALFETCH
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests CImImap4GetMail - tests combination of Partial populate 
// new/all/selection when the user is already connected; connects, does 
// operation, disconnects; connects, does operation and stays online.
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_PARTIALFETCH.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_PARTIALFETCH.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_PARTIALFETCH.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_PARTIALFETCH.exe on the other platform
// 
//

#include "emailtestutils.h"
#include <txtrich.h>
#include <commdb.h>
#include "TESTENV.h"
#include <imapconnectionobserver.h>
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

// local variables etc //

class CDummyConnectionObserver : public MMsvImapConnectionObserver
	{
		void HandleImapConnectionEvent(TImapConnectionEvent aConnectionEvent);
	};

void CDummyConnectionObserver::HandleImapConnectionEvent(TImapConnectionEvent)
	{
	}


_LIT(KImcmTest, "T_PARTIALFETCH - Testing CImImap4GetMail");
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

class COperationActive;
LOCAL_D COperationActive* opActive;

#if 1
_LIT(KImapServer, "lon-msgtest06.intra");
_LIT8(KFolderPath, "");

#else
_LIT(KImapServer, "utter.chaos.org.uk");
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
	: CTimer(1), iCancelState(200)
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
LOCAL_C void CreateMailSelection(CMsvEntrySelection& aSelection)
	{
//	Create a selection of all IMAP4 messages in the parent folder. 
	testUtils->iMsvEntry->SetEntryL(imap4Service+1);
//	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service+1);
	CMsvEntrySelection* selection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);

	for(TInt i=0 ; i<selection->Count(); i++)
		 aSelection.AppendL(selection->At(i));
	testUtils->iMsvEntry->SetEntryL(imap4Service);
	}

LOCAL_C void ConnectAndSyncCompleteL()
	{
	testUtils->WriteComment(_L("Inside ConnectAndSyncCompleteL "));

	testUtils->WriteComment(_L("Connecting to the IMAP4 server"));

	CDummyConnectionObserver *dummyObserver = new(ELeave)CDummyConnectionObserver;
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

LOCAL_C void SynchronizeRemoteInbox()
	{
	testUtils->WriteComment(_L("Synchronizing Remote Inbox")); 
	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);

	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyText;//EGetImap4EmailBodyTextAndAttachments;
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
LOCAL_C void PopulateNewMessagesPartiallyWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Populating new messages when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox

	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateNewMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate new mail active object
	}

LOCAL_C void PopulateAllMessagesPartiallyWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Populating all messages partially when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 5000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateAllMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate all mail active object
	}

LOCAL_C void PopulateAllMessagesWithCumulativeOption()
	{
	testUtils->WriteComment(_L("Populating all messages partially when already connected with TImImap4PartialMailOptions = ECumulative")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=ECumulative;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateAllMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate all mail active object
	}

LOCAL_C void PopulateAllMessagesWithNoSizeLimitOption()
	{
	testUtils->WriteComment(_L("Populating all messages with no size limits")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=ENoSizeLimits;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateAllMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate all mail active object
	}


LOCAL_C void PopulateMessageSelectionPartiallyWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Populate mail selection partially when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	CreateMailSelection(*msvSelection); // message in the remote inbox (to populate)

	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMPopulateMailSelectionWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate message selection active object
	}


//
// Connect, Get Mail and Disconnect tests
//
LOCAL_C void ConnectAndPopulateNewMailPartiallyAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Populate New Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox

	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateNewMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the Populate new mail active object
	}

LOCAL_C void ConnectAndPopulateAllMailPartiallyAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Populate All Mail Partially, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox

	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateAllMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the Populate all mail active object
	}


LOCAL_C void ConnectAndPopulateAllMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Full download after partial download \n")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox

	TImImap4GetMailInfo imap4GetMailInfo;
	imap4GetMailInfo.iMaxEmailSize = KMaxTInt;
	imap4GetMailInfo.iDestinationFolder = imap4Service+2; // service
	imap4GetMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
	TPckgBuf<TImImap4GetMailInfo> package(imap4GetMailInfo);
	
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateAllMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the Populate all mail active object
	}

LOCAL_C void ConnectAndPopulateMailSelectionPartiallyAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Populating remote msg, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	CreateMailSelection(*msvSelection); // message in the remote inbox (to populate)
	
	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateMailSelectionAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate message selection active object
	}

//
// Connect, Get Mail and Stay Online tests
//
LOCAL_C void ConnectAndPopulateNewMailPartiallyAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Populate New Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);

	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateNewMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the Populate new mail active object
	}

LOCAL_C void ConnectAndPopulateAllMailPartiallyAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Populate All Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(imap4Service);
	msvSelection->AppendL(imap4Service+1); // remote inbox
	
	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);

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
	CreateMailSelection(*msvSelection); // message in the remote inbox (to populate)
	
	TImImap4GetPartialMailInfo imapGetPartialMailInfo;
	imapGetPartialMailInfo.iPartialMailOptions=EBodyTextAndAttachments;
	imapGetPartialMailInfo.iDestinationFolder = imap4Service+1;//KMsvGlobalInBoxIndexEntryIdValue;	
	imapGetPartialMailInfo.iBodyTextSizeLimit = 7000;	
	imapGetPartialMailInfo.iAttachmentSizeLimit = 7000;
	imapGetPartialMailInfo.iTotalSizeLimit = 9000;
	TPckgBuf<TImImap4GetPartialMailInfo> package(imapGetPartialMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndPopulateMailSelectionAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the populate message selection active object
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

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Synchronize Remote Inbox                         \n"));
	SynchronizeRemoteInbox();

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();

	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		return;
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("ConnectAndSyncCompletedAfterDisconnect                         \n"));

	ConnectAndSyncCompleteL();

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
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
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	CMsvEntrySelection* children = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(children);
	TInt total = children->Count();
	for (TInt i=0; i<total; i++)
		testUtils->iMsvEntry->DeleteL((*children)[i]);
	CleanupStack::PopAndDestroy(children);

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

	CleanupStack::PopAndDestroy(2); //settings, store/accounts
	CleanupStack::PopAndDestroy();	// cmdLineArg
	}

LOCAL_C void CreateServicesL()
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

	settings->SetDeleteEmailsWhenDisconnecting(EFalse);
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
	accounts->GetImapAccountL(testUtils->iMsvEntry->EntryId(), imapAccount);
	accounts->SaveImapSettingsL(imapAccount, *settings);

	CleanupStack::PopAndDestroy(2); //settings, store/accounts
	CleanupStack::PopAndDestroy();	// cmdLineArg
	}


LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	console = test.Console();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoClientSideL();
	
	console->SetPos(0, 3);
	test.Printf(_L("Create Services                        "));
	testUtils->WriteComment(_L("Create Services                        "));
	// Create Services
	smtpService = testUtils->CreateSmtpServiceL();
	imap4Service = testUtils->CreateImapServiceL();

	testUtils->WriteComment(_L("********** T_PARTIALFETCH Test  **********"));
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler
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
	// Delete any *.scr files in c:\logs\email directory.
	testUtils->DeleteScriptFilesFromLogsDirL();

	testUtils->iMsvEntry->SetEntryL(imap4Service);

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

	settings->SetDeleteEmailsWhenDisconnecting(EFalse);
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
	
	CleanupStack::PopAndDestroy(2); //settings, store/accounts
	CleanupStack::PopAndDestroy();	// cmdLineArg

	console->SetPos(0, 3);
	test.Printf(_L("Instantiating IMAP4 Client MTM"));
	testUtils->WriteComment(_L("Instantiating IMAP4 Client MTM"));
	testUtils->InstantiateImapClientMtmL();
// Tests start here

//
//
// Test 1
// Populating all mail partially when already connected       	
//
//

	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	testUtils->TestStart(1);
	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	CleanupStack::PopAndDestroy(); // opActive
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

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populating all mails partially when already connected \n"));
	PopulateAllMessagesPartiallyWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
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

	testUtils->TestFinish(1,globalError);
	CleanupStack::PopAndDestroy(2); // opActive, msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}
	
//
//
// Test 2
// Populate new mails partially when already connected to server
//
//
	
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	testUtils->TestStart(2);
	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	if (globalError)
		{
		Closedown();
		return;
		}

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to IMAP server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populate new mails partially when already connected to server \n"));
	PopulateNewMessagesPartiallyWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
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

	testUtils->TestFinish(2,globalError);
	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	if (globalError)
		{
		Closedown();
		return;
		}
		
//
//
//  Test 3
//	Populate mail selection partially when already connected to server
//
//

	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	testUtils->TestStart(3);
	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	// Populate mail selection when already connected to server
	CleanupStack::PopAndDestroy(); // opActive
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

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populate mail selection partially when already connected to server\n"));
	PopulateMessageSelectionPartiallyWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
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

	testUtils->TestFinish(3,globalError);
	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	if (globalError)
		{
		Closedown();
		return;
		}

//
//
// Test 4
// Connect and populate new mails partially and disconnect
//
//
	
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	ConnectAndSyncRemoteInboxAndDisconnectL();

	testUtils->TestStart(4);
	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate New Mails Partially, Disconnect           \n"));
	ConnectAndPopulateNewMailPartiallyAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(4,globalError);
	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	if (globalError)
		{
		Closedown();
		return;
		}

//
//
// Test 5
// Connect and populate all mails partially and disconnect
//
//
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	ConnectAndSyncRemoteInboxAndDisconnectL();

	testUtils->TestStart(5);
	// Connect, Populate All Mail and then disconnect
	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate All Mails Partially, Disconnect           \n"));
	ConnectAndPopulateAllMailPartiallyAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(5,globalError);

  	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	if (globalError)
		{
		Closedown();
		return;
		}
	
//
//
//	Test 6
//	Connect, Populate Mail Selection Partially, Stay Online
//
//
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	testUtils->TestStart(6);
	// Connect, Populate Mail Selection and Stay Online
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate Mail Selection Partially, Stay Online    \n"));
	ConnectAndPopulateMailSelectionAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
	testUtils->TestFinish(6,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	if (globalError)
		{
		Closedown();
		return;
		}
	
//
//
//	Test 7
//	Connect, Populate New Mails Partially, Stay Online
//
//
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	testUtils->TestStart(7);
	// Connect, Populate New Mail and Stay Online
	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate New Mails Partially, Stay Online          \n"));
	ConnectAndPopulateNewMailPartiallyAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
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
	testUtils->TestFinish(7,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	if (globalError)
		{
		Closedown();
		return;
		}
//
//
//	Test 8
//	Connect, Populate All Mails partially, Stay Online
//
//
	
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	testUtils->TestStart(8);
	// Connect, Populate All Mail and Stay Online
	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate All Mails Partially, Stay Online          \n"));
	ConnectAndPopulateAllMailPartiallyAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
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
	testUtils->TestFinish(8,globalError);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	if (globalError)
		{
		Closedown();
		return;
		}

//
//
// Test 9
// Populating all mails partially when already connected with TImImap4PartialMailOptions = ENoSizeLimits        	
//
//
	
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	
	testUtils->TestStart(9);
	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	CleanupStack::PopAndDestroy(); // opActive
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

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populating all mails with No Size Limits \n"));
	PopulateAllMessagesWithNoSizeLimitOption();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
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

	testUtils->TestFinish(9,globalError);
	CleanupStack::PopAndDestroy(2); // opActive,msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}

//
//
// Test 10
// Populating all mails partially when already connected with TImImap4PartialMailOptions = ECumulative      	
//
//

	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	testUtils->TestStart(10);
	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	CleanupStack::PopAndDestroy(); // opActive
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

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populating all mails partially when already connected with TImImap4PartialMailOptions = ECumulative \n"));
	PopulateAllMessagesWithCumulativeOption();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
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

	testUtils->TestFinish(10,globalError);
	CleanupStack::PopAndDestroy(2); // opActive,msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}

//
//
//	Test 11
//	Test for full download of emails after partial download
//
//

	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// Populating all mails partially when already connected       	
	testUtils->TestStart(11);
	// connect, do full sync and then disconnect so that the inbox can be synced
 	ConnectAndSyncRemoteInboxAndDisconnectL();
	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Populating all mails partially when already connected       \n"));

	ConnectAndPopulateAllMailPartiallyAndDisconnectL();

	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}

	testUtils->WriteComment(_L("Doing full download of emails after partial download")); 
	// Now do a full download of emails after partial download
	CreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Full download all mail when already connected       \n"));

	ConnectAndPopulateAllMailAndDisconnectL();
	
	CActiveScheduler::Start();

	testUtils->TestFinish(11,globalError);
	CleanupStack::PopAndDestroy(2); // opActive,msvSelection
	if (globalError)
		{
		Closedown();
		return;
		}

//
//
//	Test 12
//	Connect and populate mail selection Partially and disconnect
//
//
	
	DeleteAndCreateServicesL();
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);

	// connect, do full sync and then disconnect so that the inbox can be synced
	ConnectAndSyncRemoteInboxAndDisconnectL();

	testUtils->TestStart(12);
	// Connect, Populate Mail Selection and then disconnect
	CleanupStack::PopAndDestroy(); // opActive
	if (globalError)
		{
		Closedown();
		return;
		}
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Populate Mail Selection Partially, Disconnect     \n"));
	ConnectAndPopulateMailSelectionPartiallyAndDisconnectL();
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

//	testUtils->FindChildrenL(KMsvRootIndexEntryId, EFalse, EFalse);
	testUtils->TestFinish(12,globalError);
	if (globalError)
		testUtils->TestHarnessFailed(globalError);
	else
		testUtils->TestHarnessCompleted();

	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 

	testUtils->Reset();
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_PARTIALFETCH Test for partial fetch of emails"));
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
