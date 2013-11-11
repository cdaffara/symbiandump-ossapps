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
// Name of test harness: T_POPC
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests the client-side API of POP3 MTM; Query capability; standard POP3 
// functionality; CImPop3GetMail API.
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_POPC\T_IMCM.log
// <DRIVE>:\msglogs\T_POPC\Entry_Structure.txt
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_POPC.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_POPC.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_POPC.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_POPC.exe on the other platform
// 
//

#include "emailtestutils.h"
#include <txtrich.h>
#include <c32comm.h>
#include <commdb.h>
#include <mtmuids.h>
#include "TESTENV.h"
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

_LIT(KImcmTest, "T_POPC - Testing CImPop3GetMail");
RTest test(KImcmTest);

_LIT(KTestParams,"-testparams");
enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgServerAddress,
	EArgLogin,
	EArgPassword,
	EArgPopulationLimit,
	EArgEnd
	};


LOCAL_D TMsvId Pop3Service;
LOCAL_D TMsvId smtpService;

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;

LOCAL_D CMsvOperation* msvOperation;
LOCAL_D CMsvEntrySelection* msvSelection;

LOCAL_D CMsvEntrySelection* messageSelection;

LOCAL_D CConsoleBase* console;

class COperationActive;
LOCAL_D COperationActive* opActive;
LOCAL_D TInt testNo = 1;
LOCAL_D TInt globalError = 0;
LOCAL_D TBool iDoCancel = EFalse;

_LIT(KTestSettingsServerAddress,"2.2.2.2");
const TInt KTestSettingsPort = 111;
_LIT8(KTestSettingsLoginName,"testuser");
_LIT8(KTestSettingsPassword,"password");
const TInt KTestPopulationLimit=30;

_LIT(KServer, "lon-cn-exchng2k.msexchange2k.closedtest.intra");
_LIT8(KLoginName, "t_popc");
_LIT8(KPassword, "t_popc");


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
			return _L("Refreshing              ");
		case 1:
			return _L("Copying                 ");
		case 2:
			return _L("Deleting                ");
		case 3:
			return _L("Connecting              ");
		case 4:
			return _L("Tidying                 ");
		case 5:
			return _L("Connected and Idle      ");
		case 6:
			return _L("Disconnecting           ");
		case 7:
			return _L("Disconnected            ");
		case 8:
			return _L("Moving                  ");
		case 9:
			return _L("Copying New Mail        ");
		case 10:
			return _L("Moving New Mail         ");
		case 11:
			return _L("Copying Mail Selection  ");
		case 12:
			return _L("Moving Mail Selection   ");
		case 13:
			return _L("Copying All Mail        ");
		case 14:
			return _L("Moving All Mail         ");
		default:
			return _L("Unknown Progress        ");
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
		if(iDoCancel == TBool(ETrue))
			{
			iOperation->Cancel();
			}
		TPop3Progress temp;	
		TPckgC<TPop3Progress> paramPack(temp);
		paramPack.Set(iOperation->ProgressL());
		TPop3Progress progress=paramPack();	

		if (iCancelState == progress.iPop3SubStateProgress)
			{
			iConsole->Printf(_L("	Calling Cancel\n"));
			iOperation->Cancel();
			}

		iConsole->SetPos(0, 12);
		TBuf<80> progressBuf = Progress(progress.iPop3Progress);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Super Operation Progress: %S           "), &progressBuf);
		iConsole->SetPos(0, 13);
		progressBuf = Progress(progress.iPop3SubStateProgress);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Sub Operation Progress  : %S           "), &progressBuf);
		iConsole->SetPos(0, 14);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Progress:  %d/%d %d/%d Total:%d                \n"),
					progress.iTotalMsgs-progress.iMsgsToProcess,
					progress.iTotalMsgs,
					progress.iBytesDone,
					progress.iTotalBytes,
					progress.iTotalSize);
		iConsole->SetPos(0, 15);
		iConsole->Printf(TRefByValue<const TDesC>_L("   Error: %d                             \n"), progress.iErrorCode);
		if (progress.iErrorCode && !globalError)
			{
			if(!iDoCancel || progress.iErrorCode!=KErrCancel)
				{// we should not report KErrCancel as an error when iDoCancel is true because that is expected behviour
				globalError=progress.iErrorCode;
				}
			}
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
	void StartL(TBool aDisableProgress = EFalse);
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

void COperationActive::StartL(TBool aDisableProgress)
	{
	if(aDisableProgress)
		{
		iTimer->IssueRequest();	// Start the connect observation timer
		SetActive();
		test.Printf(TRefByValue<const TDesC>_L("POP3 progress disabled"));
		}
	else
		{	
		TPop3Progress temp;	
		TPckgC<TPop3Progress> paramPack(temp);
		paramPack.Set(iTimer->iOperation->ProgressL());
		TPop3Progress progress=paramPack();	

		switch(progress.iPop3Progress)
			{
			case TPop3Progress::EPopDisconnected:
			case TPop3Progress::EPopDisconnecting:
				// don't set active
				iSetActive = EFalse;
				break;
			case TPop3Progress::EPopCopyNewMail:
			case TPop3Progress::EPopMoveNewMail:
			case TPop3Progress::EPopCopyMailSelection:
			case TPop3Progress::EPopMoveMailSelection:
			case TPop3Progress::EPopCopyAllMail:
			case TPop3Progress::EPopMoveAllMail:
			case TPop3Progress::EPopPopulating:
			default:
				switch(progress.iPop3SubStateProgress)
					{
					// don't set active if we are in the disconnecting state or if we have disconnected.
					case TPop3Progress::EPopDisconnected:
					case TPop3Progress::EPopDisconnecting:
					default:
						// don't set active
						iSetActive = EFalse;	
						break;
					case TPop3Progress::EPopCopying:
					case TPop3Progress::EPopDeleting:
					case TPop3Progress::EPopConnectedAndIdle:
					case TPop3Progress::EPopMoving:
					case TPop3Progress::EPopRefreshing:
					case TPop3Progress::EPopConnecting:
					case TPop3Progress::EPopTidying:
						{
						if (iSetActive && iTimer->iOperation->IsActive())
							{
							
							iTimer->IssueRequest();	// Start the connect observation timer
							SetActive();
							
							}
						}
					}
			}

		test.Console()->SetPos(0, 17);
		test.Printf(TRefByValue<const TDesC>_L("Operation TRequestStatus %d"), iStatus);
		
		}
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

LOCAL_C void StoreClientSettingsL()
//
// Dummy settings for checking that StoreSettingsL work for the client mtm
//	
	{
	testUtils->WriteComment(_L("Storing settings in client mtm"));
	CImPop3Settings* pop3Settings = new(ELeave)CImPop3Settings;
	CleanupStack::PushL(pop3Settings);
	pop3Settings->CopyL(testUtils->iPopClientMtm->Settings());
    pop3Settings->SetServerAddressL(KTestSettingsServerAddress);
	pop3Settings->SetPort(KTestSettingsPort);
	pop3Settings->SetLoginNameL(KTestSettingsLoginName);
	pop3Settings->SetPasswordL(KTestSettingsPassword);
	pop3Settings->SetPopulationLimitL(KTestPopulationLimit);
	testUtils->iPopClientMtm->SetSettingsL(*pop3Settings);
	CleanupStack::PopAndDestroy(pop3Settings);
	testUtils->iPopClientMtm->StoreSettingsL();
	}

LOCAL_C TInt VerifyStoredClientSettingsL()
//
// Comparison of the settings stored to see that the client commits after storing
//
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

	testUtils->WriteComment(_L("Checking stored settings in client mtm"));
	testUtils->iPopClientMtm->RestoreSettingsL();
	CImPop3Settings* pop3Settings = new(ELeave)CImPop3Settings;
	CleanupStack::PushL(pop3Settings);
	pop3Settings->CopyL(testUtils->iPopClientMtm->Settings());

    TInt result=0;
	result += pop3Settings->ServerAddress().Compare(KTestSettingsServerAddress);
	result += pop3Settings->Port() - KTestSettingsPort;
	result += pop3Settings->LoginName().Compare(KTestSettingsLoginName);
	result += pop3Settings->Password().Compare(KTestSettingsPassword);
    result += (pop3Settings->PopulationLimit()!=KTestPopulationLimit);

	pop3Settings->Reset();

	if (isCmdLine)
		{
		TBuf<100> buf;
		TLex lex(cmdLineArg->Arg(EArgServerAddress));
		buf=lex.NextToken();
		pop3Settings->SetServerAddressL(buf);
		}
	else
		{
		pop3Settings->SetServerAddressL(KServer);
		}

	if (isCmdLine)
		{
		TBuf<100> buf;
		TBuf8<64> data;
		TLex lex(cmdLineArg->Arg(EArgLogin));
		buf=lex.NextToken();
		data.Copy(buf);
		pop3Settings->SetLoginNameL(data);
		}
	else
		{
		pop3Settings->SetLoginNameL(KLoginName);
		}

	if (isCmdLine)
		{
		TBuf<100> buf;
		TBuf8<64> data;
		TLex lex(cmdLineArg->Arg(EArgPassword));
		buf=lex.NextToken();
		data.Copy(buf);
		pop3Settings->SetPasswordL(data);
		}
	else
		{
		pop3Settings->SetPasswordL(KPassword);
		}

	if (isCmdLine)
		{
		TBuf<100> buf;
		TLex lex(cmdLineArg->Arg(EArgPopulationLimit));
		buf=lex.NextToken();
		TInt limit;
		TLex lex2(buf);
		lex2.Val(limit);
		pop3Settings->SetPopulationLimitL(limit);
		}
	else
		{
		TInt populationLimit=40;
		pop3Settings->SetPopulationLimitL(populationLimit);
		}

	
	pop3Settings->SetPort(110);
	testUtils->iPopClientMtm->SetSettingsL(*pop3Settings);
	testUtils->iPopClientMtm->StoreSettingsL();
	
   	TRequestStatus st;
   	TMsvId s=0;
   	TMsvPartList retPart=KMsvMessagePartBody;
   	TUint id=0;
   	TInt add=0;
   	
   	TBuf<1> des;
   	TBuf8<1> des8;

	TRAP_IGNORE(testUtils->iPopClientMtm->SaveMessageL());
	TRAP_IGNORE(testUtils->iPopClientMtm->ReplyL(s,retPart,st));
	TRAP_IGNORE(testUtils->iPopClientMtm->ForwardL(s,retPart,st));
	testUtils->iPopClientMtm->ValidateMessage(retPart);
	TRAP_IGNORE(testUtils->iPopClientMtm->AddAddresseeL(des));
	TRAP_IGNORE(testUtils->iPopClientMtm->AddAddresseeL(des,des));
	testUtils->iPopClientMtm->RemoveAddressee(add);
	TRAP_IGNORE(testUtils->iPopClientMtm->AddAttachmentL(des, des8,id,st));
	TRAP_IGNORE(testUtils->iPopClientMtm->AddLinkedAttachmentL(des,des8,id,st));
	TRAP_IGNORE(testUtils->iPopClientMtm->AddEntryAsAttachmentL(s,st));
	TRAP_IGNORE(testUtils->iPopClientMtm->CreateMessageL(s));
	TRAP_IGNORE(testUtils->iPopClientMtm->DefaultServiceL());
	TRAP_IGNORE(testUtils->iPopClientMtm->RemoveDefaultServiceL());
	TRAP_IGNORE(testUtils->iPopClientMtm->ChangeDefaultServiceL(s));
	
	CleanupStack::PopAndDestroy(pop3Settings);
	CleanupStack::PopAndDestroy();	// cmdLineArg
	return result;
	}

LOCAL_C void ConnectToServerL()
	{
	testUtils->WriteComment(_L("Connecting to the Pop3 server"));
	TBuf8<1> aParameter;
	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnect, *msvSelection, aParameter, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the connect active object
	}

LOCAL_C void DisconnectFromServerL()
	{
	testUtils->WriteComment(_L("Disconnecting from the Pop3 server"));
	TBuf8<1> aParameter;
	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMDisconnect, *msvSelection, aParameter, opActive->iStatus);
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
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMCopyNewMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	// Start the copy new mail active object
	}


LOCAL_C void MoveNewMailWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Moving New Mail when already connected"));
	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002);
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize=KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder= KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMMoveNewMailWhenAlreadyConnected,*msvSelection,package,opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);
	}


LOCAL_C void CopyAllMailWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Copying All Mail when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMCopyAllMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	// Start the copy all mail active object
	}


LOCAL_C void MoveAllMailWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Moving All Mails when already connected"));
	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMMoveAllMailWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	
	}


LOCAL_C void CopyMailSelectionWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Copying Mail Selection when already connected")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);

	testUtils->SetEntryL(Pop3Service);
	CMsvEntrySelection* sel = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(sel);
	if (sel->Count()>1)
		{
		msvSelection->AppendL((*sel)[0]); 
		msvSelection->AppendL((*sel)[1]); 
		}
	else if (sel->Count())
		msvSelection->AppendL((*sel)[0]);
	CleanupStack::PopAndDestroy(); //sel
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMCopyMailSelectionWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	// Start the copy mail selection active object
	}


LOCAL_C void MoveMailSelectionWhenAlreadyConnectedL()
	{
	testUtils->WriteComment(_L("Moving Mail Selection when already connected")); 
	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);	//Pop3Service is already set while connecting to the server
	testUtils->SetEntryL(Pop3Service);
	CMsvEntrySelection* sel = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(sel);
	if (sel->Count()>1)
		{
		msvSelection->AppendL((*sel)[0]); 
		msvSelection->AppendL((*sel)[1]); 
		}
	else if (sel->Count())
		msvSelection->AppendL((*sel)[0]);
	CleanupStack::PopAndDestroy(); //sel
		
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMMoveMailSelectionWhenAlreadyConnected, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	// Start the move mail selection active object
	}


//
// Connect, Get Mail and Disconnect tests
//

LOCAL_C void ConnectAndCopyNewMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Copying New Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndCopyNewMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy new mail active object
	}

LOCAL_C void ConnectAndMoveNewMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Move New Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndMoveNewMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
	}

LOCAL_C void ConnectAndCopyAllMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Copying All Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndCopyAllMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy all mail active object
	}


LOCAL_C void ConnectAndMoveAllMailAndDisconnectL()
	{
	testUtils->WriteComment(_L("Connect, Moving All Mail, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndMoveAllMailAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
	}





LOCAL_C void ConnectAndCopyMailSelectionAndDisconnectL(TBool aEmptyMessageSelection = EFalse)
	{
	testUtils->WriteComment(_L("Connect, Copying Mail Selection, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);

	if (!aEmptyMessageSelection)
		{
		testUtils->SetEntryL(Pop3Service);
		CMsvEntrySelection* sel = testUtils->iMsvEntry->ChildrenL();
		CleanupStack::PushL(sel);
		if (sel->Count()>1)
			{
			msvSelection->AppendL((*sel)[0]); 
			msvSelection->AppendL((*sel)[1]); 
			}
		else if (sel->Count())
			msvSelection->AppendL((*sel)[0]);
		CleanupStack::PopAndDestroy(); //sel
		}
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndCopyMailSelectionAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy all mail active object
	}


LOCAL_C void ConnectAndMoveMailSelectionAndDisconnectL(TBool aEmptyMessageSelection = EFalse)
	{
	testUtils->WriteComment(_L("Connect, Move Mail Selection, Disconnect")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);

	if (!aEmptyMessageSelection)
		{
		testUtils->SetEntryL(Pop3Service);
		CMsvEntrySelection* sel = testUtils->iMsvEntry->ChildrenL();
		CleanupStack::PushL(sel);
		if (sel->Count()>1)
			{
			msvSelection->AppendL((*sel)[0]); 
			msvSelection->AppendL((*sel)[1]); 
			}
		else if (sel->Count())
			msvSelection->AppendL((*sel)[0]);
		CleanupStack::PopAndDestroy(); //sel
		}
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndMoveMailSelectionAndDisconnect, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();
	}

//
// Connect, Get Mail and Stay Online tests
//

LOCAL_C void ConnectAndCopyNewMailAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Copying New Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndCopyNewMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy new mail active object
	}


LOCAL_C void ConnectAndMoveNewMailAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Moving New Mail, Stay Online"));
	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndMoveNewMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();
	}

LOCAL_C void ConnectAndCopyAllMailAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Copying All Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndCopyAllMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy all mail active object
	}


LOCAL_C void ConnectAndMoveAllMailAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Move All Mail, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);
	msvSelection->AppendL(0x00100002); // remote inbox
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndMoveAllMailAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
	}


LOCAL_C void ConnectAndCopyMailSelectionAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Copying Mail Selection, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);

	testUtils->SetEntryL(Pop3Service);
	CMsvEntrySelection* sel = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(sel);
	if (sel->Count()>1)
		{
		msvSelection->AppendL((*sel)[0]); 
		msvSelection->AppendL((*sel)[1]); 
		}
	else if (sel->Count())
		msvSelection->AppendL((*sel)[0]);
	CleanupStack::PopAndDestroy(); //sel
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndCopyMailSelectionAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	// Start the copy message selection active object
	}

LOCAL_C void ConnectAndMoveMailSelectionAndStayOnlineL()
	{
	testUtils->WriteComment(_L("Connect, Move Mail Selection, Stay Online")); 

	msvSelection->ResizeL(0);
	msvSelection->AppendL(Pop3Service);

	testUtils->SetEntryL(Pop3Service);
	CMsvEntrySelection* sel = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(sel);
	if (sel->Count()>1)
		{
		msvSelection->AppendL((*sel)[0]); 
		msvSelection->AppendL((*sel)[1]); 
		}
	else if (sel->Count())
		msvSelection->AppendL((*sel)[0]);
	CleanupStack::PopAndDestroy(); //sel
	
	TImPop3GetMailInfo Pop3GetMailInfo;
	Pop3GetMailInfo.iMaxEmailSize = KMaxTInt;
	Pop3GetMailInfo.iDestinationFolder = KMsvGlobalInBoxIndexEntryIdValue;
	TPckgBuf<TImPop3GetMailInfo> package(Pop3GetMailInfo);
	
	opActive->iStatus = KRequestPending;
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMConnectAndMoveMailSelectionAndStayOnline, *msvSelection, package, opActive->iStatus);
	opActive->SetOperation(msvOperation);
	opActive->StartL();	
	}


//

LOCAL_C void ConnectAndDisconnectL()
	{
	console->SetPos(0, 6);
	test.Printf(_L("\nPerforming Tests\n"));
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to Pop server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
	}

//


LOCAL_C void CreateReplyEmailMessage()
	{
	testUtils->WriteComment(_L("Creating a Reply Email Message"));
	msvSelection->ResizeL(0);
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	//CleanupStack::PushL(messageSelection);
	msvSelection->AppendL(KMsvGlobalOutBoxIndexEntryId); //destination id
	msvSelection->AppendL(messageSelection->At(5)); // message id to which the reply is to be assigned
	//CleanupStack::PopAndDestroy(); 	 //messageSelection
	
	//create an object of type TImCreateMessageOptions
	TImCreateMessageOptions Pop3CreateMessage;
	
	//Initialize the member variables of TImCreateMessageOptions suitably 
	Pop3CreateMessage.iMsvPartList = KMsvMessagePartBody;
	Pop3CreateMessage.iMsvEmailTypeList = 0;
	Pop3CreateMessage.iMessageType = KUidMsgTypeSMTP;
	TPckgBuf<TImCreateMessageOptions> package(Pop3CreateMessage);
	
	//set the status
	opActive->iStatus = KRequestPending;
	
	//invoke the func InvokeAsyncFunctionL()
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMCreateReplyEmailMessage, *msvSelection, package, opActive->iStatus);
	
	//start the operation
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	
	delete messageSelection;
	}


LOCAL_C void CreateForwardEmailMessage()
	{
	testUtils->WriteComment(_L("Creating a Forward Email Message"));
	msvSelection->ResizeL(0);
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	msvSelection->AppendL(KMsvGlobalOutBoxIndexEntryId); //destination id
	msvSelection->AppendL(messageSelection->At(5)); // message id to which the reply is to be assigned
		
	//create an object of type TImCreateMessageOptions
	TImCreateMessageOptions Pop3CreateMessage;
	
	//Initialize the member variables of TImCreateMessageOptions suitably 
	Pop3CreateMessage.iMsvPartList = KMsvMessagePartBody;
	Pop3CreateMessage.iMsvEmailTypeList = 0;
	Pop3CreateMessage.iMessageType = KUidMsgTypeSMTP;
	TPckgBuf<TImCreateMessageOptions> package(Pop3CreateMessage);
	
	//set the status
	opActive->iStatus = KRequestPending;
	
	//invoke the func InvokeAsyncFunctionL()
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMCreateForwardEmailMessage, *msvSelection, package, opActive->iStatus);
	
	//start the operation
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	
	delete messageSelection;
	}


LOCAL_C void CreateForwardAsAttachmentEmailMessage()
	{
	testUtils->WriteComment(_L("Creating a Forward As Attachment Email Message"));
	msvSelection->ResizeL(0);
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	
	msvSelection->AppendL(Pop3Service); //destination id
	msvSelection->AppendL(messageSelection->At(1)); // message id to which the reply is to be assigned
	
	
	//create an object of type TImCreateMessageOptions
	TImCreateMessageOptions Pop3CreateMessage;
	
	//Initialize the member variables of TImCreateMessageOptions suitably 
	Pop3CreateMessage.iMsvPartList =KMsvMessagePartBody | KMsvMessagePartAttachments;
	Pop3CreateMessage.iMsvEmailTypeList = KMsvEmailTypeListMHTMLMessage;
	Pop3CreateMessage.iMessageType = KUidMsgTypeSMTP;
	TPckgBuf<TImCreateMessageOptions> package(Pop3CreateMessage);
	
	//set the status
	opActive->iStatus = KRequestPending;
	
	//invoke the func InvokeAsyncFunctionL()
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMCreateForwardAsAttachmentEmailMessage, *msvSelection, package, opActive->iStatus);
	
	//start the operation
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	
	delete messageSelection;
	}


LOCAL_C void CreateReceiptEmailMessage()
	{
	testUtils->WriteComment(_L("Creating a Receipt Email Message"));
	msvSelection->ResizeL(0);
	
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	
	msvSelection->AppendL(KMsvGlobalOutBoxIndexEntryId); //destination id
	msvSelection->AppendL(messageSelection->At(5)); // message id 
	
	
	//create an object of type TImCreateMessageOptions
	TImCreateMessageOptions Pop3CreateMessage;
	
	//Initialize the member variables of TImCreateMessageOptions suitably 
	Pop3CreateMessage.iMsvPartList = KMsvMessagePartBody;
	Pop3CreateMessage.iMsvEmailTypeList = 0;
	Pop3CreateMessage.iMessageType = KUidMsgTypeSMTP;
	TPckgBuf<TImCreateMessageOptions> package(Pop3CreateMessage);
	
	//set the status
	opActive->iStatus = KRequestPending;
	
	//invoke the func InvokeAsyncFunctionL()
	msvOperation = testUtils->iPopClientMtm->InvokeAsyncFunctionL(KPOP3MTMCreateReceiptEmailMessage, *msvSelection, package, opActive->iStatus);
	
	//start the operation
	opActive->SetOperation(msvOperation);
	opActive->StartL(ETrue);	
	delete messageSelection;
	}


LOCAL_C void TestCapabilityL()
	{
	TInt response = 0;
	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQuerySupportedBody, response) == KErrNone);
	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQuerySupportAttachments, response) == KErrNone);
//	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQuerySupportSubject, response) == KErrNone);
//	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQuerySupportsFolder, response) == KErrNotSupported);
//	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQueryOffLineAllowed, response) == KErrNotSupported);
	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQueryCanSendMsg, response) == KErrNotSupported);
	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQueryCanReceiveMsg, response) == KErrNone);
	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQuerySendAsRequiresRenderedImage, response) == KErrNotSupported);
	test(testUtils->iPopClientMtm->QueryCapability(KUidMtmQueryMaxRecipientCount, response) == KErrNotSupported);
	}

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
	testUtils->iMsvEntry->DeleteL(Pop3Service);
	testUtils->iMsvEntry->DeleteL(smtpService);

	// Create Services
	smtpService = testUtils->CreateSmtpServiceL();
	Pop3Service = testUtils->CreatePopServiceL();

	testUtils->iMsvEntry->SetEntryL(Pop3Service);

//	TUint32 iap=0;
//	CCommsDatabase* db =CCommsDatabase::NewL();
//	CleanupStack::PushL(db);
//	db->GetGlobalSettingL(TPtrC(DIAL_OUT_IAP),iap);
//	CleanupStack::PopAndDestroy(); //db

	//overwrite the settings with test code one.  Don't want the default settings.
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
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
		{
		settings->SetServerAddressL(KServer);
		}

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
		{
		settings->SetLoginNameL(KLoginName);
		}

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
		{
		settings->SetPasswordL(KPassword);
		}
	
	settings->SetPort(110);
	TPopAccount pop3AccountId;
	accounts->GetPopAccountL(Pop3Service, pop3AccountId);
	accounts->SavePopSettingsL( pop3AccountId, *settings);
	
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
	testUtils->ClearEmailAccountsL();
	// Create Services
	testUtils->GoClientSideL();
	testUtils->ClearEmailAccountsL();
	smtpService = testUtils->CreateSmtpServiceL();
	Pop3Service = testUtils->CreatePopServiceL();
	testUtils->GoServerSideL();
	
	console->SetPos(0, 3);
	test.Printf(_L("Create Data Component FileStores"));
	testUtils->CreateSmtpServerMtmRegL();
	testUtils->CreatePopServerMtmRegL();

	// Load the DLLs
	testUtils->CreateRegistryObjectAndControlL();
	testUtils->InstallSmtpMtmGroupL();
	testUtils->InstallPopMtmGroupL();

	// Create Services

	testUtils->WriteComment(_L("********** T_POPC Test CImPop3GetMail class **********"));

	}
	
LOCAL_C void Closedown()
	{
	// mkn testUtils->WriteComment(_L("**********      T_POPC Tests Complete      **********"));

	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler
	}

//

LOCAL_C TPtrC Progress(TInt progressNumber)
	{
	switch (progressNumber)
		{
		case 0:
			return _L("Refreshing              ");
		case 1:
			return _L("Copying                 ");
		case 2:
			return _L("Deleting                ");
		case 3:
			return _L("Connecting              ");
		case 4:
			return _L("Tidying                 ");
		case 5:
			return _L("Connected and Idle      ");
		case 6:
			return _L("Disconnecting           ");
		case 7:
			return _L("Disconnected            ");
		case 8:
			return _L("Moving                  ");
		case 9:
			return _L("Copying New Mail        ");
		case 10:
			return _L("Moving New Mail         ");
		case 11:
			return _L("Copying Mail Selection  ");
		case 12:
			return _L("Moving Mail Selection   ");
		case 13:
			return _L("Copying All Mail        ");
		case 14:
			return _L("Moving All Mail         ");
		default:
			return _L("Unknown Progress        ");
		}
	}

LOCAL_C void doMainL()
	{
	InitL();
	testUtils->FileSession().SetSessionPath(_L("c:\\"));

	testUtils->CreateMessageFilesL(Pop3Service, KMsvGlobalInBoxIndexEntryId, _L("c:\\mailtest\\IMCM\\"));
	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);

	// Delete any *.scr files in c:\logs\email directory.
	testUtils->DeleteScriptFilesFromLogsDirL();

	// Instantiate the Pop3 Client MTM
	testUtils->GoClientSideL();

	testUtils->iMsvEntry->SetEntryL(Pop3Service);

	TMsvEntry entry=testUtils->iMsvEntry->Entry();
	testUtils->iMsvEntry->ChangeL(entry);

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
	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	
	TPopAccount pop3AccountId;
	accounts->GetPopAccountL(Pop3Service, pop3AccountId);
	accounts->LoadPopSettingsL(pop3AccountId, *settings);

	if (isCmdLine)
		{
		TBuf<100> buf;
		TLex lex(cmdLineArg->Arg(EArgServerAddress));
		buf=lex.NextToken();
		test.Printf(_L("Server address: %S\n"),&buf);
		settings->SetServerAddressL(buf);
		}
	else
		{
		settings->SetServerAddressL(KServer);
		}

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
		{
		settings->SetLoginNameL(KLoginName);
		}

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
		{
		settings->SetPasswordL(KPassword);
		}

	if (isCmdLine)
		{
		TBuf<100> buf;
		TLex lex(cmdLineArg->Arg(EArgPopulationLimit));
		buf=lex.NextToken();
		TInt limit;
		TLex lex2(buf);
		lex2.Val(limit);
		settings->SetPopulationLimitL(limit);
		}
	else
		{
		TInt populationLimit=40;
		settings->SetPopulationLimitL(populationLimit);
		}
	
	settings->SetPort(110);
	accounts->SavePopSettingsL(pop3AccountId, *settings);
	CleanupStack::PopAndDestroy(2); // store/accounts, settings
	CleanupStack::PopAndDestroy();	// cmdLineArg

	console->SetPos(0, 3);
	test.Printf(_L("Instantiating POP3 Client MTM"));
	testUtils->InstantiatePopClientMtmL();
	testUtils->WriteComment(_L("Instantiated POP3 Client MTM"));
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	// Check that CPop3ClientMtm::StoreSettingsL() really works.
	testUtils->TestStart(testNo++);
	console->SetPos(0, 7);
	test.Printf(_L("Storing settings								 \n"));
	StoreClientSettingsL();
	TInt result = VerifyStoredClientSettingsL();
	test(result == KErrNone);
	testUtils->TestFinish(testNo-1);

	// Copy New Mail when already connected to server
	testUtils->TestStart(testNo++);
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to Pop server                         \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	
	
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Copy New Mail when already connected to server   \n"));
	CopyNewMailWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	iDoCancel = ETrue;
	CActiveScheduler::Start();
	iDoCancel = EFalse;

		{
	// wait 5 seconds so that user can see the final progess!
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(10000000);
	CActiveScheduler::Start();  
	delete timer;
		}
		
		
	testUtils->TestFinish(testNo-1);
	
	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	testUtils->TestStart(testNo++);
	/*Step1: Connect to the POP3 server*/
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0,7);
	test.Printf(_L("Connecting to Pop server\n"));
	ConnectToServerL();
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy();	//opActive
	
	/*Step2: Move new mails*/
	opActive =  COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0,7);
	test.Printf(_L("Move new mail when already connected to server  \n"));
	MoveNewMailWhenAlreadyConnectedL();	
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy();	//opActive
	
	/*Step 3: Disconnect from the server */
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0,7);
	test.Printf(_L("Disconnecting from server\n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
	
	testUtils->TestFinish(testNo-1);
	//cleanup for next test
	CleanupStack::PopAndDestroy(2);  //opActive, msvSelection
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	testUtils->TestStart(testNo++);
	// Copy All Mail when already connected to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to Pop server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);  
	test.Printf(_L("Copy All Mail when already connected to server   \n"));
	CopyAllMailWhenAlreadyConnectedL();
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

  	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);


	testUtils->TestStart(testNo++);
	// Step 1: connect to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to Pop server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	//Step 2: Move All Mails 
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);  
	test.Printf(_L("Move All Mail when already connected to server   \n"));
	MoveAllMailWhenAlreadyConnectedL();
	CActiveScheduler::Start();
		
	//Step 3: Disconnect the server	
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);
		
	testUtils->TestStart(testNo++);
	// Copy Mail Selection when already connected to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to Pop server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);			
	test.Printf(_L("Copy Msg selection when already connected        \n"));
	CopyMailSelectionWhenAlreadyConnectedL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);


	testUtils->TestStart(testNo++);
	//Step 1: connect to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to Pop server                        \n"));
	ConnectToServerL();
	CActiveScheduler::Start();

	//Step 2: Move Selected mails when already connected
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);			
	test.Printf(_L("Move Mail selection when already connected        \n"));
	MoveMailSelectionWhenAlreadyConnectedL();
	CActiveScheduler::Start();
	
	//Step 3:Disconnect the server
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);
	
	//clean up for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	//
	// 			Tests for Creating Replies and Forwards     				//
	//
	//Creating a Reply Email Message

	testUtils->TestStart(testNo++);
	// Connecting to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to server             \n"));
	ConnectToServerL();
	CActiveScheduler::Start();
			
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);			
	test.Printf(_L("Create Reply Email Message               \n"));
	CreateReplyEmailMessage();
	CActiveScheduler::Start();
			
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
			
	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	//Creating a Forward Email Message

	testUtils->TestStart(testNo++);
	// Connecting to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to server             \n"));
	ConnectToServerL();
	CActiveScheduler::Start();
			
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);			
	test.Printf(_L("Create Reply Email Message               \n"));
	CreateForwardEmailMessage();
	CActiveScheduler::Start();
			
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
			
	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);


	//Creating a Forward As Attachment Email Message

	testUtils->TestStart(testNo++);
	// Connecting to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to server             \n"));
	ConnectToServerL();
	CActiveScheduler::Start();
				
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);			
	test.Printf(_L("Create a Forward As Attachment Email Message               \n"));
	CreateForwardAsAttachmentEmailMessage();
	CActiveScheduler::Start();
				
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
				
	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	//Create Receipt Email Message

	testUtils->TestStart(testNo++);
	// Connecting to server
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connecting to server             \n"));
	ConnectToServerL();
	CActiveScheduler::Start();
					
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);			
	test.Printf(_L("Create a Receipt Email Message               \n"));
	CreateReceiptEmailMessage();
	CActiveScheduler::Start();
					
	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
					
	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	testUtils->TestStart(testNo++);
	// Connect, Copy New Mail and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	
	test.Printf(_L("Connect, Copy New Mail, Disconnect               \n"));
	ConnectAndCopyNewMailAndDisconnectL();
	CActiveScheduler::Start();

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);


	testUtils->TestStart(testNo++);
	// Connect, Move New Mail and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
		
	test.Printf(_L("Connect, Move New Mail, Disconnect               \n"));
	ConnectAndMoveNewMailAndDisconnectL();
	CActiveScheduler::Start();

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	testUtils->TestStart(testNo++);
	// Connect, Copy All Mail and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy All Mail, Disconnect               \n"));
	ConnectAndCopyAllMailAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	testUtils->TestStart(testNo++);

	// Connect, Move All Mail and then disconnect
		opActive = COperationActive::NewL();
		CleanupStack::PushL(opActive);
		console->SetPos(0, 7);
		test.Printf(_L("Connect,Move All Mail, Disconnect               \n"));
		ConnectAndMoveAllMailAndDisconnectL();
		CActiveScheduler::Start();

		testUtils->TestFinish(testNo-1);

		// cleanup for next test
		CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
		DeleteAndCreateServicesL();
		testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

		msvSelection = new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(msvSelection);
		testUtils->TestStart(testNo++);

	
	// connect and then disconnect to update mailbox
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Disconnect                              \n"));
	ConnectAndDisconnectL();
	CleanupStack::PopAndDestroy(); // opActive

	// Connect, Copy Mail Selection and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy Mail Selection, Disconnect         \n"));
	ConnectAndCopyMailSelectionAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	testUtils->TestStart(testNo++);
	

	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Move Mail Selection, Disconnect         \n"));
	ConnectAndMoveMailSelectionAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);


	testUtils->TestStart(testNo++);
	// Connect, Copy New Mail and Stay Online
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy New Mail, Stay Online              \n"));
	ConnectAndCopyNewMailAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);


	testUtils->TestStart(testNo++);
	// Connect, Move New Mail and Stay Online
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Move New Mail, Stay Online              \n"));
	ConnectAndMoveNewMailAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);
	

	
	
	testUtils->TestStart(testNo++);
	// Connect, Copy All Mail and Stay Online
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy All Mail, Stay Online              \n"));
	ConnectAndCopyAllMailAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	testUtils->TestStart(testNo++);
	
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Move All Mail, Stay Online              \n"));
	ConnectAndMoveAllMailAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);
	
	
	testUtils->TestStart(testNo++);
	// connect and then disconnect to update mailbox
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Disconnect                              \n"));
	ConnectAndDisconnectL();
	CleanupStack::PopAndDestroy(); // opActive

	// Connect, Copy Mail Selection and Stay Online
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy Msg Selection, Stay Online         \n"));
	ConnectAndCopyMailSelectionAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
	
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Move Msg Selection, Stay Online         \n"));
	ConnectAndMoveMailSelectionAndStayOnlineL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(); // opActive
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Disconnecting from the server                    \n"));
	DisconnectFromServerL();
	CActiveScheduler::Start();
	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);
		
	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);
	
	
//
//	Test an empty message selection (does not cause a panic!)
//

	testUtils->TestStart(testNo++);
	// Connect, Copy All Mail and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	test.Printf(_L("Connect, Copy Empty Message Selection, Disconnect\n"));
	ConnectAndCopyMailSelectionAndDisconnectL(ETrue);
	CActiveScheduler::Start();

	// check that final progress showing the correct information
	TPckgBuf<TPop3Progress> progress;
	progress.Copy(opActive->iTimer->iOperation->FinalProgress());

	console->SetPos(0, 11);
	console->Printf(_L("Final Progress:                    "));
	console->SetPos(0, 12);
	TBuf<80> progressBuf = Progress(progress().iPop3Progress);
	console->Printf(TRefByValue<const TDesC>_L("   Super Operation Progress: %S           "), &progressBuf);
	console->SetPos(0, 13);
	progressBuf = Progress(progress().iPop3SubStateProgress);
	console->Printf(TRefByValue<const TDesC>_L("   Sub Operation Progress  : %S           "), &progressBuf);
	console->SetPos(0, 14);
	console->Printf(TRefByValue<const TDesC>_L("   Progress:  %d/%d %d/%d Total:%d               \n"),
				progress().iTotalMsgs-progress().iMsgsToProcess,
				progress().iTotalMsgs,
				progress().iBytesDone,
				progress().iTotalBytes,
				progress().iTotalSize);
	console->SetPos(0, 15);
	console->Printf(TRefByValue<const TDesC>_L("   Error: %d                             \n"), progress().iErrorCode);

	// wait 5 seconds so that user can see the final progess!
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;

	testUtils->TestFinish(testNo-1);

//
//	Do Cancel Operation Tests
//

	testUtils->WriteComment(_L("Doing Cancel Tests"));
	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	testUtils->TestStart(testNo++);
	// Connect, Copy New Mail and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 11);
	console->Printf(_L("                                       "));
	console->SetPos(0, 7);
	opActive->SetCancelState(TPop3Progress::EPopConnecting);
	test.Printf(_L("Connect, Copy New Mail, Disconnect               \n"));
	ConnectAndCopyNewMailAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	testUtils->TestStart(testNo++);
	// Connect, Copy New Mail and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	opActive->SetCancelState(TPop3Progress::EPopRefreshing);
	test.Printf(_L("Connect, Copy New Mail, Disconnect               \n"));
	ConnectAndCopyNewMailAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);

	testUtils->TestStart(testNo++);
	// Connect, Copy New Mail and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	opActive->SetCancelState(TPop3Progress::EPopCopying);
	test.Printf(_L("Connect, Copy New Mail, Disconnect               \n"));
	ConnectAndCopyNewMailAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	DeleteAndCreateServicesL();
	testUtils->iPopClientMtm->SwitchCurrentEntryL(Pop3Service);

	msvSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);
	
	

	testUtils->TestStart(testNo++);
	// Connect, Copy New Mail and then disconnect
	opActive = COperationActive::NewL();
	CleanupStack::PushL(opActive);
	console->SetPos(0, 7);
	opActive->SetCancelState(TPop3Progress::EPopDisconnecting);
	test.Printf(_L("Connect, Copy New Mail, Disconnect               \n"));
	ConnectAndCopyNewMailAndDisconnectL();
	CActiveScheduler::Start();

	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo++);
	TestCapabilityL();
	testUtils->TestFinish(testNo-1);
	if (globalError)
		testUtils->TestHarnessFailed(globalError);
	else
		testUtils->TestHarnessCompleted();
	// cleanup for next test
	CleanupStack::PopAndDestroy(2); //msvSelection, opActive, 
	testUtils->Reset();
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_POPC Test CImPop3GetMail class"));
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
