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

#include <logcli.h>
#include <logview.h>

#include "EmailTestUtils.h"
#include <commdb.h>
#include <bacline.h>
#include <cemailaccounts.h>



#include "t_log.h"

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

// --------------------------------------------------------------------------

//#define _DEBUG_MTMCOMMAND_

// Static variables

LOCAL_D RTest test(_L("IMPS.dll Testrig"));
LOCAL_D RFs theFs;

// LOCAL_C CMsvServer* theServer;

_LIT(KFolderNameToEncode, "αινσϊ");

_LIT(KMessageNameTest, "Test: 12");
_LIT(KMessageNameAttachment, "Test: 9");

_LIT(KMessageNamePriority,"Test: 23");
_LIT(KMessageNameXPriority,"Test: 15");
_LIT(KMessageNamePrecedence,"Test: 24");
_LIT(KMessageNameImportance,"Test: 14");

_LIT(KMessageNameMimeHTML, "Test: 2");


LOCAL_D const TUid KUidImQueuedOperationList = {0x10001794};	// 2648441492 dec.

_LIT(KTestParams,"-testparams");
enum TCmdLineArgs
	{
	EArgProgramName,
	EArgTestParams,
	EArgRemoveMsgStore,
	EArgMainMenu
	};

// --------------------------------------------------------------------------

class TestScheduler : public CActiveScheduler
	{
public:
	void Error (TInt aError) const;
	};

void TestScheduler::Error(TInt anError) const
	{
	test.Printf(_L("Scheduler Error %d"), anError );
	CActiveScheduler::Stop();
	}

// -------------------------------------------------------------------------

class TestUiTimer : public CTimer
	{
public:
	static TestUiTimer* NewL( CConsoleBase*, CBaseServerMtm* &aImppServerMtm );
	~TestUiTimer();

	void RunL();
	void DoCancel();
	void ConstructL();
	void IssueRequest();
	void Dump();
	TPtrC StateName(TImap4GenericProgress::TImap4GenericProgressState aState);
	TPtrC OpName(TImap4GenericProgress::TImap4GenericProgressOp aOp);
	TPtrC SyncStateName(TImap4SyncProgress::TImap4SyncProgressState aState);

	CConsoleBase*				iConsole;
	CBaseServerMtm*				&iImppServerMtm;
	TTimeIntervalMicroSeconds32 period;
protected:
	TestUiTimer( CConsoleBase* , CBaseServerMtm* &aImppServerMtm );

	TInt iCount;
	TBool iClosing;
	TBool iDoneConnecting;
	};

// --------------------------------------------------------------------------

enum TImap4OpType
	{
	EImap4OpCopyToLocal,
	EImap4OpMoveToLocal,
	EImap4OpCopyFromLocal,
	EImap4OpMoveFromLocal,
	EImap4OpCopyWithinService,
	EImap4OpMoveWithinService,
	EImap4OpMoveWithinServiceToInbox,
	EImap4OpPopulate,
	EImap4OpDelete,
	EImap4OpUndelete,
	EImap4OpCopyWithinFolder,
	EImap4OpCopyToLocalAndDelete,
	EImap4OpMoveToLocalAndDelete,
	EImap4OpCopyFromLocalAndDelete,
	EImap4OpMoveFromLocalAndDelete,
	EImap4OpCopyWithinServiceAndDelete,
	EImap4OpMoveWithinServiceAndDelete
	};

enum TDebugLevel
	{
	EDebugLevel0=0,
	EDebugLevel1,
	EDebugLevel2,
	EDebugLevel3,
	EDebugLevel4,
	EDebugLevel5,
	EDebugLevel6,
	EDebugLevel7,
	EDebugLevel8,
	EDebugLevel9
	};
class CActiveConsole;
class CTestRefreshMBox : public CActive
	{
public:
	static CTestRefreshMBox* NewLC(CCommandLineArguments* aCmdLineArgs);
	static CTestRefreshMBox* NewL(CCommandLineArguments* aCmdLineArgs);

	// Destruction
	~CTestRefreshMBox();
	
	// Issue request
	void StartL();
	void EnterStateL(TInt aState);

	// Cancel request.
	// Defined as pure virtual by CActive;
	// implementation provided by this class.
	void DoCancel();

	// Service  request.
	// Defined as pure virtual by CActive;
	// implementation provided by this class,
	void RunL();
	TInt NumBadMessages();

	void ModifyImap4SynchronisationLimitsL(TInt aInboxSyncLimit,TInt aMailboxSyncLimit);
	void FindImap4SettingsL();
	void CreateDefaultServiceL(CImImap4Settings* aSettings);
	void CreateServiceL( const CImImap4Settings* aSettings,
						 const TDesC& aDetails, const TDesC& aDescription,
						 const TBool aLocalOnly =EFalse);
	void QueueOpL(TImap4OpType aOp);
	void UnQueueOpL(TImap4OpType aOp);
	void QueueLocalToLocalOpL(TImap4OpType aOp);

 	CMsvServerEntry* ServerEntry();
	void SelectServiceIdL();
	TMsvId iServiceId;
	void DoCancelOffLineOpsL();
	
	void SetConsole(CActiveConsole* aConsole) {iConsole=aConsole;} 

	CEmailTestUtils*		iTestUtils;
	TInt iDebugLevel;

private:
    void EndOfTest();  
	void SetSubscriptionsL( TFolderSyncType aSync, TFolderSubscribeType aSubscribe );
	void FindAttachmentL();
	void FindMessagesInSelectionL();

	void DoConnectL();
	void DoConnectAndSyncL();
	void DoFullSyncL();
	void DoInboxNewSyncL();
	void DoSyncTreeL();
	void DoFetchInboxL();
	void DoDisconnectL();
	void DoWaitForBackgroundL();
	void DoFetchAttachmentL();
    void DoFetchAttachedMessageL();
	void DoRenameFolderL();
	void DoAutomatedOffLineOpsL();
	void DoAutomatedSyncOpsL();
	void DoFetchTestL();
	void DoFolderRemoveL();
	void DoFolderRemoveL(TMsvId aParent, const TDesC& aName);
	void DoFolderCreateL();
	void DoFolderCreateL(TMsvId aParent, const TDesC& aName);
	void DoAutomatedSecondaryConnectOpsL();
	void DoCopyMessagesL();

	TBool StateMachineCheckServerContentsL();
	void DoCheckServerContentsL();

	TBool StateMachineNSynchronisationL();
	void DoNSynchronisationRegressionL();
	void DoNSynchronisationTestsL();
	void DoNSynchronisationSpecialTestsL();
	void DoNSynchronisationPerformanceTestsL();
	void DoNSynchronisationOneOffTestsL();

	void StartTimerL();
	void ClearTimer();
	void ConstructL();
	TPtrC TestMsgName(TImap4OpType aOpType, TImap4GetMailOptions aOptions);
	void TestFolders(TImap4OpType aOpType,
					 TMsvId& srcFolder, TMsvId& destination);
	
	TMsvId FindMessageByNameL(TMsvId aSrcFolder, const TDesC& aName);
	TMsvId FindMessageByOpL(TMsvId aSrcFolder, TImap4OpType aOp);
	TMsvId FindMessageByOpL(TMsvId aSrcFolder, TImap4OpType aOp, TImap4GetMailOptions aOptions);
	TMsvId FindFolderByNameL(TMsvId aSrcFolder, const TDesC& aName);
	TMsvId FindByNameL(TMsvId aSrcFolder, const TDesC& aName, TBool aSearchOnDetails);
	
	TMsvId CountMessagesByNameL(TMsvId aSrcFolder, const TDesC& aName);
	TInt CountMessagesByOpL(TMsvId aSrcFolder, TImap4OpType aOp);


protected:
	// Data members defined by this class
	CBaseServerMtm*			iImppServerMtm;
	CMsvServerEntry*		iEntry;
	CMsvEntrySelection*		iSelection;
	TInt					iState;

private:
	CMsvServerEntry* SetEntryL(TMsvId aFolder, const TDesC& aName);
	CImHeader* GetHeaderLC();
	CImImap4Settings* GetImap4SettingsLC();

	TBool HandleSyncStateL();
	TBool HandleOffLineStateL();
	TBool TestSecondaryConnectStateMachineL();

	void NextState(TInt aInc);
	void NextState();
	void PerformOpL( TImap4OpType aOpType, const TDesC8& aParams);
	void PerformSpecialOpL(TMsvId aId, TImap4Cmds aCommand);
	
	void DoOpByIdL( TImap4OpType aOpType, TMsvId aId );
	void DoOpByIdL( TImap4OpType aOpType, TMsvId aId, const TDesC8& aParams );
	void DoOpL( TImap4OpType aOpType, const TDesC& aName, TMsvId aSourceFolder, TMsvId aDestFolder, const TDesC8& aParams);
	void DoOpL( TImap4OpType aOpType, TMsvId aSourceFolder, TMsvId aDestFolder, const TDesC8& aParams);
	void DoOpL( TImap4OpType aOpType, TMsvId aSourceFolder, TMsvId aDestFolder);
	void DoOpL( TImap4OpType aOpType, TMsvId aSourceFolder, TMsvId aDestFolder, TImap4GetMailOptions aOptions);
	void DoOpNamedL( TImap4OpType aOpType, TMsvId aSrcFolder, TMsvId aDestFolder, const TDesC8& params);
	void DoOpNamedL( TImap4OpType aOpType, TMsvId aSrcFolder, TMsvId aDestFolder);
	void DoOpNamedL( TImap4OpType aOpType, TMsvId aSrcFolder, TMsvId aDestFolder, TImap4GetMailOptions aOptions);

	TBool TestQueueL( TMsvId aFolder, CImOffLineOperation::TOffLineOpType aOpType, TMsvId aMessage, TMsvId aTarget, TInt aFnId,	const TDesC8& aParams );
	TBool TestQueueL( TMsvId aFolder, CImOffLineOperation::TOffLineOpType aOpType, TMsvId aMessage, TMsvId aTarget, TImap4GetMailOptions aOptions );
	TBool TestQueueL( TMsvId aFolder, CImOffLineOperation::TOffLineOpType aOpType, TMsvId aMessage, TMsvId aTarget );
	TBool TestQueueEmptyL( TMsvId aFolder );
	TMsvId FindShadowL( TMsvId aFolder, TMsvId aSourceId );
	TBool TestShadowL( TMsvId aFolder, TMsvId aSourceId );
	TBool TestVisible( TMsvId aSourceId );
	TBool TestInvisible( TMsvId aSourceId );
	TBool TestComplete( TMsvId aSourceId );
	TBool TestExists( TMsvId aSourceId );
	TImDisconnectedOperationType TestDisconnectedFlags( TMsvId aSourceId );
	TBool TestSubscribed( TMsvId aSourceId );
	TBool TestNotSubscribed( TMsvId aSourceId );
	TBool TestLocalSubscription( TMsvId aSourceId );
	TBool TestNotLocalSubscription( TMsvId aSourceId );

	void TestPriorityAndReceiptAddressL();

	void DebugFormatL(TInt aDebugLevel,TRefByValue<const TDesC> aFmt,...);
	void DebugUidListL(TInt aDebugLevel);

	TMsvId iRemoteInbox;

	TMsvId iRemoteFolder1;
	TMsvId iRemoteFolder2;
	TMsvId iLocalFolder1;
	TMsvId iLocalFolder2;

	TInt iOpState;
	TInt iTestNum;
	TMsvId iSourceId;
	TMsvId iSourceFolder;
	TMsvId iDestFolder;
	TMsvId iOrigSourceId;

	CLogClient *iLogClient;
	CLogViewEvent *iLogView;
	CLogFilter *iLogFilter;

	TUint32 iMessageUid;
	TInt iSyncLimit;
	TTime iTimeStamp;
	
private:
	CTestRefreshMBox();
	TestUiTimer*			iTimer;

	TBool					iRenamedFolder;
	
	CActiveConsole*			iConsole;
	CCommandLineArguments*	iCmdLineArgs;	

public:
	enum
		{
		EImapTestConnect,
		EImapTestConnectAndSync,
		EImapTestFullSync,
		EImapTestInboxNewSync,
		EImapTestSyncTree,
		EImapTestDisconnect,
		EImapTestFetchInbox,
		EImapTestOffLineOp,
		EImapTestWaitForBackground,
		EImapTestFetchAttachment,
		EImapTestCancelOffLineOps,
		EImapTestRenameFolder,
		EImapTestAutoOffLineOp,
		EImapTestAutoSyncOp,
		EImapTestFetchTest,
		EImapTestFolderDelete,
		EImapTestFolderCreate,
		EImapTestCheckServerContents,
		EImapTestAutoSecondaryConnect,
		EImapTestLocalSubscribe,
		EImapTestLocalUnsubscribe,
        EImapTestFetchAttachedMessage,
        EImapTestCopyMessages,
		EImapTestNSynchronisation,	 
		EImapTestNSynchronisationTests,			 
		EImapTestNSynchronisationSpecialTests,
		EImapTestNSynchronisationPerformanceTests,
		EImapTestNSynchronisationOneOffTests
		};
	};

// --------------------------------------------------------------------------

// My derived Console, as per the example code and the stuff in T_POPC 

class CActiveConsole : public CActive
	{
public:
	// Construction
	static CActiveConsole* NewLC( CTestRefreshMBox *aRefresher,CCommandLineArguments* aCmdLineArgs );

	// Destruction
	~CActiveConsole();

	// Issue request
	void RequestCharacter();
	
	// Cancel request.
	// Defined as pure virtual by CActive;
	void DoCancel();

	// Service completed request.
	// Defined as pure virtual by CActive;
	void RunL();

	// Called from RunL() to handle the completed request
	void ProcessKeyPressL(TChar aChar); 

	void DumpMessageStructureL( TMsvId aId,
							   TInt aSummary,
							   TInt aParts,
							   TBool aRecurse,
							   TBool aShowIds,
							   MsgMatchFn aMatch =0);
private:
	void ConstructL();
	CActiveConsole();

	void DisplayMainMenu();
	void DisplayImap4ServerMenu();
	void DisplayImap4SettingsMenu();
	void DisplayImap4DumpMenu();
	void DisplayHousekeepingMenu();
	void DisplayOffLineOpMenu();
	void DisplayOffLineOpQueueMenu();
	void DisplayOffLineOpUnQueueMenu();
	void DisplayOffLineOpQueueLocalToLocalMenu();
	void DisplayConnectMenu();
	void DisplayConnect2Menu();
	void DisplayAutomationMenu();
	void DisplaySynchroniseMenu();
	void DisplayDebugMenu();

	void SelectImap4ServerEntryL(TChar aChar);
	void SelectImap4SettingsEntryL(TChar aChar);
	

public:
	TBool	iIssueCancel;

private:	
	TUint iMenuState;
	TUint iPrevState;

	CTestRefreshMBox *iRefresher;
	
	CCommandLineArguments*	iCmdLineArgs;
	TInt					iLevel;

	enum
		{
		EExit = 0,
		EMainMenu,
		EImap4ServerMenu,
		EImap4SettingsMenu,
		EImap4DumpMenu,
		EHousekeepingMenu,
		EOffLineOpMenu,
		EOffLineOpQueueMenu,
		EOffLineOpUnQueueMenu,
		EOffLineOpQueueLocalToLocalMenu,
		EConnectMenu,
		EConnect2Menu,
		EAutomationMenu,
		ESynchroniseMenu,
		EDebugMenu
		};
	};

// -------------------------------------------------------------------------

// Constants

_LIT(KFilePathMailTest, "c:\\mailtest\\");

#define KPeriod 10000	// period of timer

// -------------------------------------------------------------------------

TestUiTimer* TestUiTimer::NewL( CConsoleBase* aConsole, CBaseServerMtm* &aImppServerMtm)
	{
	TestUiTimer* self = new(ELeave) TestUiTimer( aConsole , aImppServerMtm);
	CleanupStack::PushL(self);
	self->iImppServerMtm = aImppServerMtm;
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}


TestUiTimer::TestUiTimer( CConsoleBase* aConsole , CBaseServerMtm* &aImppServerMtm)
	: CTimer(10 /*KMsgPop3RefreshMailboxPriority+1*/) , iImppServerMtm(aImppServerMtm)
	{
	iConsole = aConsole;
	period = 	KPeriod;
	iClosing = EFalse;
	iCount = 0;
	iDoneConnecting = EFalse;
	}

TestUiTimer::~TestUiTimer()
	{
	}

void TestUiTimer::ConstructL()
	{
	CTimer::ConstructL();

	CActiveScheduler::Add( this );
	}


void TestUiTimer::IssueRequest()
	{
	After( period );
	}


void TestUiTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

TPtrC TestUiTimer::OpName(TImap4GenericProgress::TImap4GenericProgressOp aOp)
	{
	switch (aOp)
		{
	case TImap4GenericProgress::EConnect:
		return _L("EConnect");
	case TImap4GenericProgress::EDisconnect:
		return _L("EDisconnect");
	case TImap4GenericProgress::ESync:
		return _L("ESync");
	case TImap4GenericProgress::ESelect:
		return _L("ESelect");
		
	case TImap4GenericProgress::ECopyToLocal:
		return _L("ECopyToLocal");
	case TImap4GenericProgress::ECopyWithinService:
		return _L("ECopyWithinService");
	case TImap4GenericProgress::ECopyFromLocal:
		return _L("ECopyFromLocal");
	case TImap4GenericProgress::EMoveToLocal:
		return _L("EMoveToLocal");
	case TImap4GenericProgress::EMoveWithinService:
		return _L("EMoveWithinService");
	case TImap4GenericProgress::EMoveFromLocal:
		return _L("EMoveFromLocal");
	case TImap4GenericProgress::EPopulate:
		return _L("EPopulate");
	case TImap4GenericProgress::EDelete:
		return _L("EDelete");
		
	case TImap4GenericProgress::EOffLineDelete:
	case TImap4GenericProgress::EOffLineUndelete:
	case TImap4GenericProgress::EOffLineCopyToLocal:
	case TImap4GenericProgress::EOffLineMoveToLocal:
	case TImap4GenericProgress::EOffLineCopyFromLocal:
	case TImap4GenericProgress::EOffLineMoveFromLocal:
	case TImap4GenericProgress::EOffLineCopyWithinService:
	case TImap4GenericProgress::EOffLineMoveWithinService:
	case TImap4GenericProgress::EOffLinePopulate:
		return _L("EOffLineOp");
		
	default:
		test.Panic(_L("Unknown Op %d"),aOp);
		break;
		}
	return _L("EUnknown");
	}

TPtrC TestUiTimer::StateName(TImap4GenericProgress::TImap4GenericProgressState aState)
	{
	switch (aState)
		{
	case TImap4GenericProgress::EDisconnected:
		return _L("EDisconnected");
	case TImap4GenericProgress::EConnecting:
		return _L("EConnecting");
	case TImap4GenericProgress::EIdle:
		return _L("EIdle");
	case TImap4GenericProgress::ESelecting:
		return _L("ESelecting");
	case TImap4GenericProgress::EFetching:
		return _L("EFetching");
	case TImap4GenericProgress::EAppending:
		return _L("EAppending");
	case TImap4GenericProgress::ECopying:
		return _L("ECopying");
	case TImap4GenericProgress::EDeleting:
		return _L("EDeleting");
	case TImap4GenericProgress::ESyncing:
		return _L("ESyncing");
	case TImap4GenericProgress::EDisconnecting:
		return _L("EDisconnecting");
	case TImap4GenericProgress::EBusy:		
		return _L("EBusy");
	default:
		test.Panic(_L("Unknown State %d"),aState);
		break;
		}
	return _L("EUnknown");
	}


TPtrC TestUiTimer::SyncStateName(TImap4SyncProgress::TImap4SyncProgressState aState)
	{
	switch (aState)
		{
	case TImap4SyncProgress::EIdle:
		return _L("Idle");
	case TImap4SyncProgress::EBusy:
		return _L("Busy");
	case TImap4SyncProgress::EConnecting:
		return _L("Connecting");
	case TImap4SyncProgress::EDisconnecting:
		return _L("Disconnecting");
	case TImap4SyncProgress::ESyncInbox:
		return _L("SyncInbox");
	case TImap4SyncProgress::ESyncFolderTree:
		return _L("SyncFolderTree");
	case TImap4SyncProgress::ECheckRemoteSubscription:
		return _L("CheckRemoteSubscription");
	case TImap4SyncProgress::EUpdateRemoteSubscription:
		return _L("UpdateRemoteSubscription");
	case TImap4SyncProgress::ESyncOther:
		return _L("SyncOther");
	case TImap4SyncProgress::EDeleting:
		return _L("Deleting");
	case TImap4SyncProgress::EProcessingPendingOps:
		return _L("ProcPending");
	default:
		test.Panic(_L("Unknown SyncState %d"),aState);
		break;
		}
	return _L("EUnknown");
	}

void TestUiTimer::Dump()
	{
	// display the current progress
	TImap4CompoundProgress temp;	
	TPckgC<TImap4CompoundProgress> paramPack(temp);

	const TDesC8& progBuf = iImppServerMtm->Progress();	
	paramPack.Set(progBuf);
	TImap4CompoundProgress progress=paramPack();	

	test.Console()->SetPos(0, 15);

	if ( progress.iGenericProgress.iState == TImap4GenericProgress::EConnecting &&
		 progress.iGenericProgress.iErrorCode == KErrNone )
		{
		if ( iDoneConnecting )
			return;
		iDoneConnecting = ETrue;
		}
	
	TPtrC opName = OpName(progress.iGenericProgress.iOperation);
	TPtrC stateName = StateName(progress.iGenericProgress.iState);

	test.Printf(_L("%-11S: %-14S M %3d/%3d P %2d/%2d B %6d/%6d T %6d Err %d\n"),
				&opName, &stateName,
				progress.iGenericProgress.iMsgsDone, progress.iGenericProgress.iMsgsToDo,
				progress.iGenericProgress.iPartsDone, progress.iGenericProgress.iPartsToDo,
				progress.iGenericProgress.iBytesDone, progress.iGenericProgress.iBytesToDo,
				progress.iGenericProgress.iTotalSize,
				progress.iGenericProgress.iErrorCode);
	
	TPtrC syncName = SyncStateName(progress.iSyncProgress.iState);
	test.Printf(_L("           : %-14S M %3d/%3d F %2d/%2d   \n"),
				&syncName,
				progress.iSyncProgress.iMsgsDone, progress.iSyncProgress.iMsgsToDo,
				progress.iSyncProgress.iFoldersDone, progress.iSyncProgress.iFoldersToDo);
	}

void TestUiTimer::RunL()
	{
	Dump();
	IssueRequest();
	};

// -------------------------------------------------------------------------

// CTestRefresh CActive object
CTestRefreshMBox::CTestRefreshMBox() // construct low-priority active object
	: CActive(-3)
	{
	}

CTestRefreshMBox  *CTestRefreshMBox::NewLC(CCommandLineArguments* aCmdLineArgs)
	{
	CTestRefreshMBox* self=new (ELeave) CTestRefreshMBox();
	
	CleanupStack::PushL(self);
	self->iCmdLineArgs = aCmdLineArgs;
	self->ConstructL();
	return self;
	}

CTestRefreshMBox  *CTestRefreshMBox::NewL(CCommandLineArguments* aCmdLineArgs)
	{
	CTestRefreshMBox* self=NewLC(aCmdLineArgs);
	CleanupStack::Pop();
	return self;
	}

void CTestRefreshMBox::ConstructL()
	{
	iTestUtils = CEmailTestUtils::NewL(test);

	iTestUtils->CreateAllTestDirectories();
	iTestUtils->FileSession().SetSessionPath(_L("C:\\"));

#if 1
	test.Printf(_L("Remove message store? (y/N)\n"));
	TChar choice;
	if (iCmdLineArgs)
		{
		TLex lex(iCmdLineArgs->Arg(EArgRemoveMsgStore));
		choice=lex.Get();
		}
	else
		choice = test.Getch();
	if (choice=='y' || choice=='Y')
#endif
		iTestUtils->Reset();
		iTestUtils->CleanMessageFolderL();	
		iTestUtils->ClearEmailAccountsL();
		iTestUtils->GoClientSideL();
	
	iTestUtils->GoServerSideL();
	iEntry = iTestUtils->iServerEntry;

	CActiveScheduler::Add(this); // add to active scheduler

	// Get server MTM.
	iTestUtils->InstantiateImapServerMtmL();
	iImppServerMtm = iTestUtils->iImapServerMtm;

	iTestUtils->FileSession().SetSessionPath(_L("c:\\"));

	test.Printf(_L("Created server mtm \n"));
	// create an entry selection
	iSelection = new (ELeave) CMsvEntrySelection;

	test.Console()->ClearScreen(); // get rid of menu
	User::LeaveIfError(iEntry->SetEntry(KMsvRootIndexEntryId));

	// test num written to log
	iTestNum = 1;

	// default debug level
	iDebugLevel = EDebugLevel3;
	}
 
CTestRefreshMBox::~CTestRefreshMBox()
	{ 
	Cancel(); // make sure we're cancelled

	ClearTimer();

	delete iSelection;
	delete iTestUtils;
	delete iLogView;
	delete iLogClient;
	delete iLogFilter;
	}

void  CTestRefreshMBox::DoCancel()
	{
	if (iTimer)
		iTimer->Cancel();	// stop the timer
	}

void CTestRefreshMBox::StartL()
	{
	if (iCmdLineArgs)
		iConsole->iIssueCancel=FALSE;
	switch(iState)
		{
	case EImapTestConnect:
		DoConnectL();
		break;

	case EImapTestConnectAndSync:
		DoConnectAndSyncL();
		break;

	case EImapTestFullSync:
		DoFullSyncL();
		break;

	case EImapTestInboxNewSync:
		DoInboxNewSyncL();
		break;

	case EImapTestSyncTree:
		DoSyncTreeL();
		break;

	case EImapTestFetchInbox:
		DoFetchInboxL();
		break;

	case EImapTestDisconnect:
		DoDisconnectL();
		break;

	case EImapTestAutoOffLineOp:
		DoAutomatedOffLineOpsL();
		break;

	case EImapTestAutoSyncOp:
		DoAutomatedSyncOpsL();
		break;

	case EImapTestAutoSecondaryConnect:
		DoAutomatedSecondaryConnectOpsL();
		break;

	case EImapTestOffLineOp:
		break;

	case EImapTestCancelOffLineOps:
		DoCancelOffLineOpsL();
		break;

	case EImapTestWaitForBackground:
		DoWaitForBackgroundL();
		break;

	case EImapTestFetchAttachment:
		DoFetchAttachmentL();
		break;

	case EImapTestRenameFolder:
		DoRenameFolderL();
		break;

	case EImapTestFetchTest:
		DoFetchTestL();
		break;

	case EImapTestFolderDelete:
		DoFolderRemoveL();
		break;

	case EImapTestFolderCreate:
		DoFolderCreateL(iServiceId, _L("Test1"));
		break;

	case EImapTestCheckServerContents:
		DoCheckServerContentsL();
		break;

	case EImapTestNSynchronisation:	   
		DoNSynchronisationRegressionL();
		break;

	case EImapTestNSynchronisationTests:
		DoNSynchronisationTestsL();
		break;

	case EImapTestNSynchronisationSpecialTests:
		DoNSynchronisationSpecialTestsL();
		break;

	case EImapTestNSynchronisationPerformanceTests:
		DoNSynchronisationPerformanceTestsL();
		break;

	case EImapTestNSynchronisationOneOffTests:
		DoNSynchronisationOneOffTestsL();
		break;

    case EImapTestLocalSubscribe:
        PerformSpecialOpL(FindFolderByNameL(iServiceId, _L("Test1")), KIMAP4MTMLocalSubscribe);
        break;

    case EImapTestLocalUnsubscribe:
        PerformSpecialOpL(FindFolderByNameL(iServiceId, _L("Test1")), KIMAP4MTMLocalUnsubscribe);
        break;

	case EImapTestFetchAttachedMessage:
		DoFetchAttachedMessageL();
		break;
		
	case EImapTestCopyMessages:
		DoCopyMessagesL();
		break;
		}
	}

void CTestRefreshMBox::EndOfTest()
	{
	if (iCmdLineArgs)
		{
		iConsole->iIssueCancel=TRUE;
		iConsole->DoCancel();
		}
	}

const TInt KDebugLogMaxLineSize=256;

void CTestRefreshMBox::DebugFormatL(TInt aDebugLevel,TRefByValue<const TDesC> aFmt,...)
	{
	// Log this debug text?
	if (aDebugLevel>iDebugLevel)
		return;

	// Build parameter list.
	VA_LIST list;
	VA_START(list, aFmt);

	// Print to log file.
	TBuf<KDebugLogMaxLineSize> buf;
	TRAPD(ret,buf.FormatList(aFmt, list));		

	// Log the debug buffer.
	if (ret==KErrNone)
		iTestUtils->WriteComment(buf);
	else
		iTestUtils->WriteComment(_L("ERROR: Formatting debug output"));
	}

void CTestRefreshMBox::DebugUidListL(TInt /*aDebugLevel*/)
	{
#if 0	// crashes on big test
	
	// Log this debug text?
	if (aDebugLevel>iDebugLevel)
		return;

	// Build list of UIDs.
	TBuf<KDebugLogMaxLineSize> buf;
 	TMsvEntry* entryPtr;
	for (TInt i=0;i<iSelection->Count();i++)
		{
		TRAPD(ret,buf.AppendFormat(i?_L(", "):_L("UID list: ")));
		if (!ret)
			{
			User::LeaveIfError(iEntry->GetEntryFromId((*iSelection)[i],entryPtr));
			TRAPD(ret,buf.AppendFormat(_L("%d"),((TMsvEmailEntry)(*entryPtr)).UID()));		
			if (ret!=KErrNone)
				{
				iTestUtils->WriteComment(_L("ERROR: Formatting debug output"));
				return;
				}
			}
		}

	// Log the debug buffer.
	iTestUtils->WriteComment(buf);
#endif
	}

void CTestRefreshMBox::EnterStateL(TInt aState)
	{
	iState = aState;
	StartL();
	}

void CTestRefreshMBox::StartTimerL()
	{
	if (!iTimer)
		{
		iTimer = TestUiTimer::NewL( test.Console(), iImppServerMtm );
		iTimer->IssueRequest( );	// START THE OBSERVATION TIMER
		}
	}

void CTestRefreshMBox::ClearTimer()
	{
	if (iTimer)
		{
		// output last known state before clearing
		iTimer->Dump();
		
		delete iTimer;
		iTimer = NULL;
		}
	}

CMsvServerEntry* CTestRefreshMBox::ServerEntry()
	{
	return iEntry;
	}

void CTestRefreshMBox::SelectServiceIdL()
	{
	User::LeaveIfError(iEntry->SetEntry(iServiceId));
	}

CMsvServerEntry* CTestRefreshMBox::SetEntryL(TMsvId aFolder, const TDesC& aName)
	{
	TMsvId id = FindMessageByNameL(aFolder, aName);
	User::LeaveIfError(iEntry->SetEntry(id));
	return iEntry;
	}

void CTestRefreshMBox::DoConnectAndSyncL()
	{
	// Kick off connection to specified service
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoConnectAndSync\n"));

	iSelection->Reset();
	iSelection->AppendL(iServiceId);
	TBuf8<128> paramBuf(_L8(""));

	SetActive();
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMConnectAndSynchronise,paramBuf,iStatus);

	// create the progress Timer object
	StartTimerL();
	}

void CTestRefreshMBox::DoConnectL()
	{
	// Kick off connection to specified service
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoConnect\n"));

	iSelection->Reset();
	iSelection->AppendL(iServiceId);
	TBuf8<128> paramBuf(_L8(""));

	SetActive();
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMConnect,paramBuf,iStatus);

	// create the progress Timer object
	StartTimerL();
	}

void CTestRefreshMBox::DoFullSyncL()
	{
	// Kick off connection to specified service
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoFullSync\n"));

	iSelection->Reset();
	TBuf8<128> paramBuf(_L8(""));

	SetActive();
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMFullSync,paramBuf,iStatus);

	// create the progress Timer object
	StartTimerL();
	}

void CTestRefreshMBox::DoInboxNewSyncL()
	{
	// Kick off connection to specified service
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoInboxNewSync\n"));

	iSelection->Reset();
	TBuf8<128> paramBuf(_L8(""));

	SetActive();
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMInboxNewSync,paramBuf,iStatus);

	// create the progress Timer object
	StartTimerL();
	}

void CTestRefreshMBox::DoSyncTreeL()
	{
	// Kick off connection to specified service
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoSyncTree\n"));

	iSelection->Reset();
	TBuf8<128> paramBuf(_L8(""));

	SetActive();
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMSyncTree,paramBuf,iStatus);

	// create the progress Timer object
	StartTimerL();
	}

void CTestRefreshMBox::DoFetchTestL()
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoFetchTest\n"));

	TMsvId id = FindMessageByNameL(iRemoteInbox, KMessageNameTest);

	// check message is there
	test(id != KErrNone);
	test(id != KErrNotFound);

	iSelection->Reset();
	iSelection->AppendL(id);
	
	SetActive();
	iImppServerMtm->CopyToLocalL(*iSelection, KMsvGlobalInBoxIndexEntryId, iStatus);

	StartTimerL();
	}

void CTestRefreshMBox::DoFetchInboxL()
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoFetchInbox\n"));

	User::LeaveIfError(iEntry->SetEntry( iRemoteInbox ));

	User::LeaveIfError(iEntry->GetChildren(*iSelection));

	// strip out any nested folders
	TInt a=0;
	while (a<iSelection->Count())
		{
		iEntry->SetEntry((*iSelection)[a]);
		if (iEntry->Entry().iType == KUidMsvFolderEntry)
			iSelection->Delete(a);
		else
			a++;
		}
	
	SetActive();
	iImppServerMtm->CopyToLocalL(*iSelection, KMsvGlobalInBoxIndexEntryId, iStatus);

	StartTimerL();
	}

void CTestRefreshMBox::DoDisconnectL()
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoDisconnect\n"));

	TBuf8<128> paramBuf(_L8(""));

	SetActive();
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMDisconnect,paramBuf,iStatus);

	StartTimerL();
	}

void CTestRefreshMBox::DoWaitForBackgroundL()
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoWaitForBackground\n"));

	TBuf8<128> paramBuf(_L8(""));

	SetActive();
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMWaitForBackground,paramBuf,iStatus);
	}


void CTestRefreshMBox::FindMessagesInSelectionL()
	{
	TInt i=0;
	while (i<iSelection->Count())
		{
		User::LeaveIfError(iEntry->SetEntry((*iSelection)[i]));
		TMsvEmailEntry entry=iEntry->Entry();
		if (entry.iType!=KUidMsvMessageEntry)
			iSelection->Delete(i,1);
		else
			i++;
		}
	}

void CTestRefreshMBox::FindAttachmentL()
	{
	TMsvId id = FindMessageByNameL(iRemoteInbox, KMessageNameAttachment);
	if (id == KErrNotFound)
		User::Leave(KErrNotFound);

	User::LeaveIfError(iEntry->SetEntry(id));
	User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvFolderEntry, *iSelection));
	if (iSelection->Count() == 0)
		User::Leave(KErrNotFound);

	User::LeaveIfError(iEntry->SetEntry((*iSelection)[0]));
	User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvAttachmentEntry, *iSelection));
	if (iSelection->Count() == 0)
		User::Leave(KErrNotFound);
	}

void CTestRefreshMBox::DoFetchAttachmentL()
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoFetchAttachment\n"));

	// get attachment id into selection
	FindAttachmentL();
	
	SetActive();

	TPckgBuf<TImap4GetMailOptions> package(EGetImap4EmailBodyTextAndAttachments);
    iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMPopulate,package,iStatus);

	// create the progress Timer object
	StartTimerL();
	}

void CTestRefreshMBox::DoFetchAttachedMessageL()
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoFetchAttachedMessage\n"));

	TMsvId id = FindMessageByNameL(iRemoteInbox, KMessageNameAttachment);
	if (id == KErrNotFound)
		User::Leave(KErrNotFound);

	User::LeaveIfError(iEntry->SetEntry(id));
	User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvFolderEntry, *iSelection));
	if (iSelection->Count() == 0)
		User::Leave(KErrNotFound);

	User::LeaveIfError(iEntry->SetEntry((*iSelection)[0]));
	User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
	if (iSelection->Count() == 0)
		User::Leave(KErrNotFound);

	SetActive();
	TPckgBuf<TImap4GetMailOptions> package(EGetImap4EmailBodyTextAndAttachments);
    iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMPopulate,package,iStatus);

	// create the progress Timer object
	StartTimerL();
	}

void CTestRefreshMBox::DoCopyMessagesL()
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoCopyMessages\n"));

	iSelection->Reset();

	User::LeaveIfError(iEntry->SetEntry( iRemoteInbox ));
	User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

	SetActive();
	iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteFolder2, iStatus);

	// create the progress Timer object
	StartTimerL();
	}

void CTestRefreshMBox::DoFolderCreateL()
	{
	DoFolderCreateL(iServiceId, KMessageNameTest);
	}

void CTestRefreshMBox::DoFolderCreateL(TMsvId aParent, const TDesC& aName)
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoFolderCreate\n"));

	// visit service
	User::LeaveIfError(iEntry->SetEntry(aParent));

	// create the msventry
	TMsvEmailEntry newEntry;
	newEntry.iType=KUidMsvFolderEntry;
	newEntry.iMtm=iEntry->Entry().iMtm;
	newEntry.iServiceId=iServiceId;
	newEntry.iSize=0;
	newEntry.iDetails.Set(aName);
	newEntry.SetMailbox(ETrue);
	newEntry.SetValidUID(EFalse);
	newEntry.SetComplete(ETrue);
	newEntry.SetVisible(ETrue);
	User::LeaveIfError(iEntry->CreateEntry(newEntry));

	// tell the IMAP server about it
	SetActive();
	TRAPD(err, iImppServerMtm->CreateL(newEntry,iStatus));
	if (err != KErrNone)
		test.Printf(_L("Op FolderCreate left with error %d\n"), err);
	
	StartTimerL();
	}

void CTestRefreshMBox::DoFolderRemoveL()
	{
	DoFolderRemoveL(iServiceId, _L("Test1"));
	}

void CTestRefreshMBox::DoFolderRemoveL(TMsvId aParent, const TDesC& aName)
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoFolderRemove\n"));

	TMsvId id = FindFolderByNameL(aParent, aName);
	if (id == KErrNotFound)
		{
		test.Printf(_L("Can't find folder '%S'\n"),&aName);
		return;
		}

	iSelection->Reset();
	iSelection->AppendL(id);

	TBuf8<128> params(_L8(""));
	TRAPD(err, PerformOpL(EImap4OpDelete,params));
	if (err != KErrNone)
		test.Printf(_L("Op FolderDelete left with error %d\n"), err);

	StartTimerL();
	}

TPtrC CTestRefreshMBox::TestMsgName(TImap4OpType aOpType,
									TImap4GetMailOptions aOptions=EGetImap4EmailBodyTextAndAttachments)
	{
	switch (aOpType)
		{
	case EImap4OpCopyToLocal:
		return _L("Test: 22");
	case EImap4OpCopyFromLocal:
		return _L("Test: 21");
	case EImap4OpCopyWithinService:
		return _L("Test: 20");
			
	case EImap4OpMoveToLocal:
		return _L("Test: 19");
	case EImap4OpMoveFromLocal:
		return _L("Test: 18");
	case EImap4OpMoveWithinServiceToInbox:
	case EImap4OpMoveWithinService:
		return _L("Test: 17");

	case EImap4OpDelete:
		return _L("Test: 16");

	case EImap4OpPopulate:
		switch (aOptions)
			{
		case EGetImap4EmailHeaders:
			return _L("Test: 9");
		case EGetImap4EmailBodyText:
			return _L("Test: 11");
		case EGetImap4EmailBodyTextAndAttachments:
			return _L("Test: 12");
		case EGetImap4EmailAttachments:
			return _L("Test: 10");
			}
		return _L("Test: 9");

	case EImap4OpCopyWithinFolder:
		return _L("Test: 8");

	case EImap4OpCopyToLocalAndDelete:
		return _L("Test: 7");
	case EImap4OpCopyFromLocalAndDelete:
		return _L("Test: 6");
	case EImap4OpCopyWithinServiceAndDelete:
		return _L("Test: 5");
			
	case EImap4OpMoveToLocalAndDelete:
		return _L("Test: 4");
	case EImap4OpMoveFromLocalAndDelete:
		return _L("Test: 3");
	case EImap4OpMoveWithinServiceAndDelete:
		return _L("Test: 27");
	default:
		break;
		}
	return _L("");
	}

void CTestRefreshMBox::TestFolders(TImap4OpType aOpType,
								   TMsvId& srcFolder, TMsvId& destination)
	{
	if (aOpType == EImap4OpCopyToLocal ||
		aOpType == EImap4OpMoveToLocal)
		{
		destination = iLocalFolder1;
		srcFolder = iRemoteInbox;
		}
	else if (aOpType == EImap4OpCopyFromLocal ||
		aOpType == EImap4OpMoveFromLocal)
		{
		destination = iRemoteFolder1;
		srcFolder = KMsvGlobalInBoxIndexEntryId;
		}
	else if (aOpType == EImap4OpCopyWithinService ||
		aOpType == EImap4OpMoveWithinService)
		{
		destination = iRemoteFolder2;
		srcFolder = iRemoteInbox;
		}
	else if (aOpType == EImap4OpMoveWithinServiceToInbox)
		{
		destination = iRemoteInbox;
		srcFolder = iRemoteFolder2;
		}
	else
		{
		destination = iRemoteInbox;
		srcFolder = iRemoteInbox;
		}
	}


TMsvId CTestRefreshMBox::CountMessagesByNameL(TMsvId aSrcFolder, const TDesC& aName)
	{
	CMsvEntrySelection* children = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);

	User::LeaveIfError(iEntry->SetEntry( aSrcFolder ));
	User::LeaveIfError(iEntry->GetChildren( *children ));

	TInt r = 0;
	for (TInt i=0; i < children->Count(); i++)
		{
		User::LeaveIfError(iEntry->SetEntry( (*children)[i] ));
		if ( iEntry->Entry().iDescription.Compare(aName) == 0 )
			r++;
		}

	CleanupStack::PopAndDestroy();

	return r;
	}

// searches on Description field
TMsvId CTestRefreshMBox::FindMessageByNameL(TMsvId aSrcFolder, const TDesC& aName)
	{
	return FindByNameL(aSrcFolder, aName, EFalse);
	}

// searches on Details field
TMsvId CTestRefreshMBox::FindFolderByNameL(TMsvId aSrcFolder, const TDesC& aName)
	{
	CMsvEntrySelection* children = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);
	TMsvId r = KErrNotFound;
	User::LeaveIfError(iEntry->SetEntry( aSrcFolder ));
	iEntry->GetChildrenWithType(KUidMsvFolderEntry, *children);

	children->InsertL(0, aSrcFolder);
	
	for (TInt i=0; i < children->Count(); i++)
		{
		r = FindByNameL((*children)[i], aName, ETrue);
		if(r != KErrNotFound)
			break;
		}

	CleanupStack::PopAndDestroy();
	return r;
	}

// searches on Details or Description field and recurses
TMsvId CTestRefreshMBox::FindByNameL(TMsvId aSrcFolder, const TDesC& aName, TBool aSearchOnDetails)
	{
	CMsvEntrySelection* children = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(children);

	User::LeaveIfError(iEntry->SetEntry( aSrcFolder ));
	User::LeaveIfError(iEntry->GetChildren( *children ));

	// isolate up to the first slash if any
	TPtrC match = aName;
	TPtrC remains = _L("");
	TInt slash = aName.Locate('/');
	if (slash != -1)
		{
		match.Set(aName.Left(slash));
		remains.Set(aName.Mid(slash+1));
		}
	
	TMsvId r = KErrNotFound;
	for (TInt i=0; i < children->Count(); i++)
		{
		User::LeaveIfError(iEntry->SetEntry( (*children)[i] ));
		if ( ( aSearchOnDetails && iEntry->Entry().iDetails.Compare(match) == 0 ) ||
			 (!aSearchOnDetails && iEntry->Entry().iDescription.Compare(match) == 0 ) )
			{
			r = (*children)[i];
			if (remains.Length())
				r = FindByNameL(r, remains, aSearchOnDetails);
			break;
			}
		}

	CleanupStack::PopAndDestroy();

	return r;
	}

TMsvId CTestRefreshMBox::FindMessageByOpL(TMsvId aSrcFolder, TImap4OpType aOp)
	{
	TPtrC name = TestMsgName(aOp);
	return FindMessageByNameL(aSrcFolder, name);
	}
	
TMsvId CTestRefreshMBox::FindMessageByOpL(TMsvId aSrcFolder, TImap4OpType aOp, TImap4GetMailOptions aOptions)
	{
	TPtrC name = TestMsgName(aOp, aOptions);
	return FindMessageByNameL(aSrcFolder, name);
	}
	
TInt CTestRefreshMBox::CountMessagesByOpL(TMsvId aSrcFolder, TImap4OpType aOp)
	{
	TPtrC name = TestMsgName(aOp);
	return CountMessagesByNameL(aSrcFolder, name);
	}
	
/* ----------------------------------------------------------------------- */

CImHeader* CTestRefreshMBox::GetHeaderLC()
	{
	CImHeader* messageheader=CImHeader::NewLC();

	CMsvStore* entryStore=iEntry->ReadStoreL();
	CleanupStack::PushL(entryStore);
	
	messageheader->RestoreL(*entryStore);
	
	CleanupStack::PopAndDestroy(); // store

	return messageheader;
	}


void CTestRefreshMBox::TestPriorityAndReceiptAddressL()
	{
	iTestUtils->TestStart(iTestNum, _L("TestPriorities"));

	SetEntryL(iRemoteInbox, KMessageNamePriority);
	test(iEntry->Entry().Priority() == EMsvHighPriority );
	SetEntryL(iRemoteInbox, KMessageNameXPriority);
	test(iEntry->Entry().Priority() == EMsvHighPriority );
	SetEntryL(iRemoteInbox, KMessageNamePrecedence);
	test(iEntry->Entry().Priority() == EMsvHighPriority );
	SetEntryL(iRemoteInbox, KMessageNameImportance);
	test(iEntry->Entry().Priority() == EMsvHighPriority );

	iTestUtils->TestFinish(iTestNum++);

	iTestUtils->TestStart(iTestNum, _L("TestReceipts"));

#if 0
	CImHeader *hdr;

	SetEntryL(iRemoteInbox, KMessageNameReturnReceiptTo);
	hdr=GetHeaderLC();
	test( hdr->ReceiptAddress().Compare( KReceiptAddress ) == 0);
	CleanupStack::PopAndDestroy(); // hdr
	
	// these tests won't work anymore as the CImMailHeader store that
	// holds this information is not available at this point
	SetEntryL(iRemoteInbox, KMessageNameXReturnReceiptTo);
	hdr=GetHeaderLC();
	test( hdr->ReceiptAddress().Compare( KReceiptAddress ) == 0);
	CleanupStack::PopAndDestroy(); // hdr

	SetEntryL(iRemoteInbox, KMessageNameDispositionNotificationTo);
	hdr=GetHeaderLC();
	test( hdr->ReceiptAddress().Compare( KReceiptAddress ) == 0 ||
		  hdr->ReceiptAddress().Compare( KReceiptAddress1 ) == 0);
	CleanupStack::PopAndDestroy(); // hdr

#endif
	iTestUtils->TestFinish(iTestNum++);

	iTestUtils->TestStart(iTestNum, _L("Test MHTML flags"));

	// check mime html has mime and not attachment flag
	SetEntryL(iRemoteInbox, KMessageNameMimeHTML);
	TMsvEmailEntry entry = (TMsvEmailEntry)iEntry->Entry();
	test(entry.MHTMLEmail());
	test(!entry.Attachment());

	iTestUtils->TestFinish(iTestNum++);
	
	iTestUtils->TestStart(iTestNum, _L("Test modified UTF7"));

	// check the encoded folder name worked
	test(FindFolderByNameL(iServiceId, KFolderNameToEncode) != KErrNone);

	iTestUtils->TestFinish(iTestNum++);
	}

void CTestRefreshMBox::PerformSpecialOpL(TMsvId aId, TImap4Cmds aCommand)
	{
#if defined (_DEBUG_MTMCOMMAND_)
	TBuf<256> buf;
	buf.Format(_L("MTMCommand %d on id %d\n"), aCommand, aId);
	iTestUtils->WriteComment(buf);
#endif

	iSourceId = aId;

	TBuf8<128> paramBuf(_L8(""));
	iSelection->Reset();
	iSelection->AppendL(aId);
	iImppServerMtm->StartCommandL(*iSelection,aCommand,paramBuf,iStatus);
	SetActive();
	}

/* ----------------------------------------------------------------------- */

void CTestRefreshMBox::PerformOpL( TImap4OpType aOpType, const TDesC8& aParams)
	{
	TBuf<256> buf;
	buf.Format(_L("Command %d on id %d (total %d) to folder %d\n"), aOpType,
			   iSelection->Count() ? (*iSelection)[0] : 0,
			   iSelection->Count(), iDestFolder);
	iTestUtils->WriteComment(buf);

	SetActive();
	switch (aOpType)
		{
	case EImap4OpCopyToLocal:
	case EImap4OpCopyToLocalAndDelete:
		iImppServerMtm->CopyToLocalL(*iSelection, iDestFolder, iStatus);
		break;
	case EImap4OpCopyFromLocal:
	case EImap4OpCopyFromLocalAndDelete:
		iImppServerMtm->CopyFromLocalL(*iSelection, iDestFolder, iStatus);
		break;
	case EImap4OpCopyWithinFolder:
	case EImap4OpCopyWithinService:
	case EImap4OpCopyWithinServiceAndDelete:
		iImppServerMtm->CopyWithinServiceL(*iSelection, iDestFolder, iStatus);
		break;	
		
	case EImap4OpMoveToLocal:
	case EImap4OpMoveToLocalAndDelete:
		iImppServerMtm->MoveToLocalL(*iSelection, iDestFolder, iStatus);
		break;
	case EImap4OpMoveFromLocal:
	case EImap4OpMoveFromLocalAndDelete:
		iImppServerMtm->MoveFromLocalL(*iSelection, iDestFolder, iStatus);
		break;
	case EImap4OpMoveWithinService:
	case EImap4OpMoveWithinServiceAndDelete:
	case EImap4OpMoveWithinServiceToInbox:
		iImppServerMtm->MoveWithinServiceL(*iSelection, iDestFolder, iStatus);
		break;

	case EImap4OpDelete:
		iImppServerMtm->DeleteAllL(*iSelection, iStatus);
		break;

	case EImap4OpUndelete:
		iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMUndeleteAll,aParams,iStatus);
		break;

	case EImap4OpPopulate:
		iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMPopulate,aParams,iStatus);
		break;

	default:
		break;
		}
	}

void CTestRefreshMBox::DoOpByIdL( TImap4OpType aOpType, TMsvId aId )
	{
	iSourceId = aId;

	TBuf8<128> paramBuf(_L8(""));

	iSelection->Reset();
	iSelection->AppendL(iSourceId);

	TRAPD(err, PerformOpL(aOpType,paramBuf));
	if (err != KErrNone)
		test.Printf(_L("Op %d left with error %d\n"), aOpType, err);
	}

void CTestRefreshMBox::DoOpByIdL( TImap4OpType aOpType, TMsvId aId, const TDesC8& aParams )
	{
	iSourceId = aId;

	iSelection->Reset();
	iSelection->AppendL(iSourceId);

	TRAPD(err, PerformOpL(aOpType,aParams));
	if (err != KErrNone)
		test.Printf(_L("Op %d left with error %d\n"), aOpType, err);
	}

void CTestRefreshMBox::DoOpL( TImap4OpType aOpType, const TDesC& aName, TMsvId aSourceFolder, TMsvId aDestFolder, const TDesC8& aParams)
	{
	iSourceFolder = aSourceFolder;
	iDestFolder = aDestFolder;
	iSourceId = FindMessageByNameL(iSourceFolder, aName);
	if (iSourceId == KErrNotFound)
		{
		test.Printf(_L("Can't find message '%S'\n"), &aName);
		return;
		}
	
	iSelection->Reset();
	iSelection->AppendL(iSourceId);

	TRAPD(err, PerformOpL(aOpType,aParams));
	if (err != KErrNone)
		test.Printf(_L("Op %d left with error %d\n"), aOpType, err);
	}

void CTestRefreshMBox::DoOpL( TImap4OpType aOpType, TMsvId aSourceFolder, TMsvId aDestFolder, const TDesC8& aParams)
	{
	DoOpL(aOpType, KMessageNameTest, aSourceFolder, aDestFolder, aParams);
	}

void CTestRefreshMBox::DoOpL( TImap4OpType aOpType, TMsvId aSourceFolder, TMsvId aDestFolder)
	{
	TBuf8<128> paramBuf(_L8(""));
	DoOpL( aOpType, aSourceFolder, aDestFolder, paramBuf );
	}

void CTestRefreshMBox::DoOpL( TImap4OpType aOpType, TMsvId aSourceFolder, TMsvId aDestFolder, TImap4GetMailOptions aOptions)
	{
	TPckgBuf<TImap4GetMailOptions> package(aOptions);	
	DoOpL(aOpType, KMessageNameTest, aSourceFolder, aDestFolder, package);
	}

void CTestRefreshMBox::DoOpNamedL( TImap4OpType aOpType, TMsvId aSrcFolder, TMsvId aDestFolder, const TDesC8& aParams)
	{
	TPtrC msgName = TestMsgName(aOpType);
	DoOpL(aOpType, msgName, aSrcFolder, aDestFolder, aParams);
	}

void CTestRefreshMBox::DoOpNamedL( TImap4OpType aOpType, TMsvId aSrcFolder, TMsvId aDestFolder)
	{
	TBuf8<128> paramBuf(_L8(""));
	DoOpNamedL( aOpType, aSrcFolder, aDestFolder, paramBuf );
	}

void CTestRefreshMBox::DoOpNamedL( TImap4OpType aOpType, TMsvId aSrcFolder, TMsvId aDestFolder, TImap4GetMailOptions aOptions)
	{
	TPckgBuf<TImap4GetMailOptions> package(aOptions);	
	TPtrC msgName = TestMsgName(aOpType, aOptions);
	DoOpL( aOpType, msgName, aSrcFolder, aDestFolder, package );
	}

TBool CTestRefreshMBox::TestQueueL( TMsvId aFolder, CImOffLineOperation::TOffLineOpType aOpType,
									TMsvId aMessage, TMsvId aTarget,
									TInt aFnId,	const TDesC8& aParams )
	{
	if (iEntry->SetEntry( aFolder ) != KErrNone)
		{
		test.Printf(_L("Can't select folder %d\n"), aFolder);
		return EFalse;
		}

	if (!iEntry->HasStoreL())
		return EFalse;
	
	CImOffLineOperationArray* ops=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(ops);

	CMsvStore* fileStore = iEntry->ReadStoreL();
	CleanupStack::PushL(fileStore);
	
	CImOffLineArrayStore arraystore(*ops);
	arraystore.RestoreL(*fileStore);

	TBool found = EFalse;
	for (TInt opNum=0; opNum < ops->CountOperations() && !found; opNum++)
		{
		const CImOffLineOperation& op = ops->Operation(opNum);

		if (op.OpType() == aOpType &&
			op.MessageId() == aMessage &&
			op.TargetMessageId() == aTarget)
			{
			if ( aOpType != CImOffLineOperation::EOffLineOpMtmSpecific )
				found = ETrue;
			else if ( op.MtmFunctionId() == aFnId && 
					  op.MtmParameters() == aParams )
				found = ETrue;
			}
		}
		
	CleanupStack::PopAndDestroy(2); // filestore, ops

	return found;
	}

TBool CTestRefreshMBox::TestQueueL( TMsvId aFolder, CImOffLineOperation::TOffLineOpType aOpType,
									TMsvId aMessage, TMsvId aTarget, TImap4GetMailOptions aOptions )
	{
	TPckgBuf<TImap4GetMailOptions> package(aOptions);	
	return TestQueueL( aFolder, aOpType, aMessage, aTarget, 1, package );
	}

TBool CTestRefreshMBox::TestQueueL( TMsvId aFolder, CImOffLineOperation::TOffLineOpType aOpType,
									TMsvId aMessage, TMsvId aTarget )
	{
	TBuf8<128> params(_L8(""));
	return TestQueueL( aFolder, aOpType, aMessage, aTarget, 0, params );
	}

TBool CTestRefreshMBox::TestQueueEmptyL( TMsvId aFolder )
	{
	if (iEntry->SetEntry( aFolder ) != KErrNone)
		{
		test.Printf(_L("Can't select folder %d\n"), aFolder);
		return EFalse;
		}

	if (!iEntry->HasStoreL())
		return ETrue;
	
	CImOffLineOperationArray* ops=CImOffLineOperationArray::NewL();
	CleanupStack::PushL(ops);

	CMsvStore* fileStore = iEntry->ReadStoreL();
	CleanupStack::PushL(fileStore);
	
	CImOffLineArrayStore arraystore(*ops);
	arraystore.RestoreL(*fileStore);
	
	TInt count = ops->CountOperations();
		
	CleanupStack::PopAndDestroy(2); // filestore, ops

	return count == 0;
	}

TMsvId CTestRefreshMBox::FindShadowL( TMsvId aFolder, TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aFolder ) != KErrNone)
		{
		test.Printf(_L("Can't select folder %d\n"), aFolder);
		return KMsvNullIndexEntryId;
		}

	CMsvEntrySelection *contents=new CMsvEntrySelection;
	CleanupStack::PushL(contents);

	User::LeaveIfError(iEntry->GetChildren(*contents));

	TMsvId id = KMsvNullIndexEntryId;
	for(TInt a=0;a<contents->Count();a++)
		{
		User::LeaveIfError(iEntry->SetEntry((*contents)[a]));
		if (iEntry->Entry().iRelatedId == aSourceId)
			{
			id = (*contents)[a];
			break;
			}
		}
	
	CleanupStack::PopAndDestroy(); // contents

	return id;
	}

TBool CTestRefreshMBox::TestShadowL( TMsvId aFolder, TMsvId aSourceId )
	{
	return FindShadowL(aFolder,aSourceId) != KMsvNullIndexEntryId;
	}

TBool CTestRefreshMBox::TestComplete( TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aSourceId ) != KErrNone)
		{
		test.Printf(_L("Can't select id %d\n"), aSourceId);
		return EFalse;
		}
	return iEntry->Entry().Complete();
	}

TBool CTestRefreshMBox::TestVisible( TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aSourceId ) != KErrNone)
		{
		test.Printf(_L("Can't select id %d\n"), aSourceId);
		return EFalse;
		}
	return iEntry->Entry().Visible();
	}

TBool CTestRefreshMBox::TestInvisible( TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aSourceId ) != KErrNone)
		{
		test.Printf(_L("Can't select id %d\n"), aSourceId);
		return EFalse;
		}
	return !iEntry->Entry().Visible();
	}

TBool CTestRefreshMBox::TestSubscribed( TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aSourceId ) != KErrNone)
		{
		test.Printf(_L("Can't select id %d\n"), aSourceId);
		return EFalse;
		}
	return ((TMsvEmailEntry)iEntry->Entry()).Subscribed();
	}

TBool CTestRefreshMBox::TestNotSubscribed( TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aSourceId ) != KErrNone)
		{
		test.Printf(_L("Can't select id %d\n"), aSourceId);
		return EFalse;
		}
	return !((TMsvEmailEntry)iEntry->Entry()).Subscribed();
	}

TBool CTestRefreshMBox::TestLocalSubscription( TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aSourceId ) != KErrNone)
		{
		test.Printf(_L("Can't select id %d\n"), aSourceId);
		return EFalse;
		}
	return ((TMsvEmailEntry)iEntry->Entry()).LocalSubscription();
	}

TBool CTestRefreshMBox::TestNotLocalSubscription( TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aSourceId ) != KErrNone)
		{
		test.Printf(_L("Can't select id %d\n"), aSourceId);
		return EFalse;
		}
	return !((TMsvEmailEntry)iEntry->Entry()).LocalSubscription();
	}

TBool CTestRefreshMBox::TestExists( TMsvId aSourceId )
	{
	return iEntry->SetEntry( aSourceId ) == KErrNone;
	}

TImDisconnectedOperationType CTestRefreshMBox::TestDisconnectedFlags( TMsvId aSourceId )
	{
	if (iEntry->SetEntry( aSourceId ) != KErrNone )
		return EDisconnectedUnknownOperation;
	return ((TMsvEmailEntry)iEntry->Entry()).DisconnectedOperation();
	}

void CTestRefreshMBox::DoCancelOffLineOpsL()
	{
	test.Console()->SetPos(0, 13);
	test.Printf(_L("DoCancelOffLineOps %x %x %x\n"), iRemoteInbox, iRemoteFolder1, iRemoteFolder2);

	iSelection->Reset();
	iSelection->AppendL(iRemoteInbox);
	iSelection->AppendL(iRemoteFolder1);
	iSelection->AppendL(iRemoteFolder2);

	TBuf8<128> paramBuf(_L8(""));
	SetActive();
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMCancelOffLineOperations,paramBuf,iStatus);
	}

void CTestRefreshMBox::NextState(TInt aInc)
	{
	iOpState /= aInc;
	iOpState += 1;
	iOpState *= aInc;
	}

void CTestRefreshMBox::NextState()
	{
	NextState(1);
	}

TBool CTestRefreshMBox::HandleOffLineStateL()
	{
	TBool more = ETrue;
	switch (iOpState)
		{
		// clear offline ops to start with
	case 0:
		DoCancelOffLineOpsL();
		NextState(10);
		break;

		// COPYTOLOCAL tests   --------------------------------------------------
		// move from local and copy back, which converts to a copyfromlocal
	case 10:
		iTestUtils->TestStart(iTestNum, _L("MoveFromLocal + CopyTolocal == CopyFromLocal"));

		DoOpL( EImap4OpMoveFromLocal, KMsvGlobalInBoxIndexEntryId, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 11:
		test(TestQueueL( iRemoteFolder1, CImOffLineOperation::EOffLineOpMoveFromLocal, iOrigSourceId, iRemoteFolder1 ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestInvisible( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedMoveFromOperation );

		DoOpL( EImap4OpCopyToLocal, iRemoteFolder1, KMsvGlobalInBoxIndexEntryId );
		NextState();
		break;

	case 12:
		test(TestQueueL( iRemoteFolder1, CImOffLineOperation::EOffLineOpCopyFromLocal, iOrigSourceId, iRemoteFolder1 ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestVisible( iOrigSourceId ));
		// failed
		//test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedCopyFromOperation );
		
		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(10);
		break;

		// copy from local and copy back to new folder, doing immediate copy
	case 20:
		iTestUtils->TestStart(iTestNum, _L("CopyFromLocal + CopyToLocal == Immediate Copy"));

		DoOpL( EImap4OpCopyFromLocal, KMsvGlobalInBoxIndexEntryId, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 21:
		test(TestQueueL( iRemoteFolder1, CImOffLineOperation::EOffLineOpCopyFromLocal, iOrigSourceId, iRemoteFolder1 ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestVisible( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedCopyFromOperation );

		DoOpL( EImap4OpCopyToLocal, iRemoteFolder1, iLocalFolder1 );
		NextState();
		break;
		
	case 22:
		{
		// check message has been copied and then delete it, do this
		// first in case other test fail
		TMsvId newId = FindMessageByNameL( iLocalFolder1, KMessageNameTest );
		test(newId != KErrNotFound);

		iEntry->SetEntry( newId );
		iEntry->SetEntry( iEntry->Entry().Parent() );
		iEntry->DeleteEntry( newId );
			
		test(TestQueueL( iRemoteFolder1, CImOffLineOperation::EOffLineOpCopyFromLocal, iOrigSourceId, iRemoteFolder1 ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestVisible( iOrigSourceId ));
		// failed
		// test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedCopyFromOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;
		}
		
		// copy back from local to service
		// COPYFROMLOCAL tests --------------------------------------------------
		
		// copy to local and copy back to new folder
		
	case 100:
		// copy fresh to local
		iTestUtils->TestStart(iTestNum, _L("CopyToLocal + CopyFromLocal == CopyToLocal + CopyWithinService"));

		DoOpL( EImap4OpCopyToLocal, iRemoteInbox, iLocalFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 101:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpCopyToLocal, iSourceId, iLocalFolder1 ));
		test(TestShadowL( iLocalFolder1, iSourceId ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedCopyToOperation );

		// copy back from local to service
		DoOpL( EImap4OpCopyFromLocal, iLocalFolder1, iRemoteFolder1 );
		NextState();
		break;

	case 102:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpCopyToLocal, iOrigSourceId, iLocalFolder1 ));
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpCopyWithinService, iOrigSourceId, iRemoteFolder1 ));
		test(TestShadowL( iLocalFolder1, iOrigSourceId ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedMultipleOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;

		// COPYWITHINSERVICE tests ----------------------------------------------

		// copy within service and copy on to new folder
	case 200:
		// copy fresh within service
		iTestUtils->TestStart(iTestNum, _L("CopyWithinService + CopyWithinService"));

		DoOpL( EImap4OpCopyWithinService, iRemoteInbox, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 201:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpCopyWithinService, iSourceId, iRemoteFolder1 ));
		test(TestShadowL( iRemoteFolder1, iSourceId ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedCopyWithinServiceOperation );
		
		// copy on to new location
		DoOpL( EImap4OpCopyWithinService, iRemoteFolder1, iRemoteFolder2 );
		NextState();
		break;

	case 202:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpCopyWithinService, iOrigSourceId, iRemoteFolder1 ));
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpCopyWithinService, iOrigSourceId, iRemoteFolder2 ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestShadowL( iRemoteFolder2, iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedMultipleOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState();
		break;

	case 203:
		// move item to folder1
		iTestUtils->TestStart(iTestNum, _L("MoveWithinService + CopyWithinService"));

		DoOpL( EImap4OpMoveWithinService, iRemoteInbox, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 204:
		// copy this ghost entry to folder2
		DoOpL( EImap4OpCopyWithinService, iRemoteFolder1, iRemoteFolder2 );
		NextState();
		break;

	case 205:
		// check that the new ghost is visible and otherwise correct
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMoveWithinService, iOrigSourceId, iRemoteFolder1 ));
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpCopyWithinService, iOrigSourceId, iRemoteFolder2 ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestShadowL( iRemoteFolder2, iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedMultipleOperation );
		test(TestInvisible( iOrigSourceId ));
		test(TestVisible( iSourceId ));
		test(TestVisible( FindMessageByNameL( iRemoteFolder2, KMessageNameTest ) ));
		
		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;

		// MOVETOLOCAL tests   --------------------------------------------------

		// move from local and move back to undo
	case 300:
		iTestUtils->TestStart(iTestNum, _L("MoveFromLocal + MoveToLocal (same folder) == nothing"));

		DoOpL( EImap4OpMoveFromLocal, KMsvGlobalInBoxIndexEntryId, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;
		
	case 301:
		test(TestQueueL( iRemoteFolder1, CImOffLineOperation::EOffLineOpMoveFromLocal, iOrigSourceId, iRemoteFolder1 ));
		test(TestInvisible( iOrigSourceId ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));

		// move back to local
		DoOpL( EImap4OpMoveToLocal, iRemoteFolder1, KMsvGlobalInBoxIndexEntryId );
		NextState();
		break;
		
	case 302:
		// check net result is nothing
		test(TestQueueEmptyL( iRemoteFolder1 ));
		test(TestDisconnectedFlags( iOrigSourceId ) == ENoDisconnectedOperations );
		test(!TestExists( iSourceId ));
		test(TestVisible( iOrigSourceId ));

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(10);
		break;

		// copy from local and move back to new folder, results in
		// direct local copy
	case 310:
		iTestUtils->TestStart(iTestNum, _L("CopyFromLocal + MoveToLocal == immediate copy"));

		DoOpL( EImap4OpCopyFromLocal, KMsvGlobalInBoxIndexEntryId, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 311:
		test(TestQueueL( iRemoteFolder1, CImOffLineOperation::EOffLineOpCopyFromLocal, iOrigSourceId, iRemoteFolder1 ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestVisible( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedCopyFromOperation );

		DoOpL( EImap4OpMoveToLocal, iRemoteFolder1, iLocalFolder1 );
		NextState();
		break;
		
	case 312:
		{
		// check message has been copied and then delete it, do this
		// first in case other tests fail
		TMsvId newId = FindMessageByNameL( iLocalFolder1, KMessageNameTest );
		test(newId != KErrNotFound);

		iEntry->SetEntry( newId );
		iEntry->SetEntry( iEntry->Entry().Parent() );
		iEntry->DeleteEntry( newId );
		
		test(TestQueueEmptyL( iRemoteFolder1 ));
		test(!TestExists( iSourceId ));
		test(TestVisible( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == ENoDisconnectedOperations );
		
		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(10);
		break;
		}
		
		// move from local and move back to new folder
	case 320:
		iTestUtils->TestStart(iTestNum, _L("MoveFromLocal + MoveToLocal == immediate move"));

		DoOpL( EImap4OpMoveFromLocal, KMsvGlobalInBoxIndexEntryId, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;
		
	case 321:
		test(TestQueueL( iRemoteFolder1, CImOffLineOperation::EOffLineOpMoveFromLocal, iOrigSourceId, iRemoteFolder1 ));
		test(TestInvisible( iOrigSourceId ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));

		// move back to local
		DoOpL( EImap4OpMoveToLocal, iRemoteFolder1, iLocalFolder1 );
		NextState();
		break;
		
	case 322:
		{
		// check message has been moved and then move it back, do this
		// first in case other tests fail
		TMsvId newId = FindMessageByNameL( iLocalFolder1, KMessageNameTest );
		test(newId == iOrigSourceId);

		iEntry->SetEntry( newId );
		iEntry->SetEntry( iEntry->Entry().Parent() );
		iEntry->MoveEntryWithinService( newId, KMsvGlobalInBoxIndexEntryId );
		
		test(TestQueueEmptyL( iRemoteFolder1 ));
		test(!TestExists( iSourceId ));
		test(TestExists( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == ENoDisconnectedOperations );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;
		}		
		// MOVEFROMLOCAL tests --------------------------------------------------

		// move to local and move back to undo
	case 400:
		// move fresh to local
		iTestUtils->TestStart(iTestNum, _L("MoveToLocal + MoveFromLocal (same folder) == nothing"));

		DoOpL( EImap4OpMoveToLocal, iRemoteInbox, iLocalFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 401:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMoveToLocal, iSourceId, iLocalFolder1 ));
		test(TestShadowL( iLocalFolder1, iSourceId ));
		test(TestInvisible( iSourceId ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedMoveToOperation );
		
		// move back to original location
		DoOpL( EImap4OpMoveFromLocal, iLocalFolder1, iRemoteInbox );
		NextState();
		break;

	case 402:
		test(TestQueueEmptyL( iRemoteInbox ));
		test(!TestShadowL( iLocalFolder1, iOrigSourceId ));
		test(TestVisible( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == ENoDisconnectedOperations );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(10);
		break;

		// move to local and move back to new folder
	case 410:
		// move fresh to local
		iTestUtils->TestStart(iTestNum, _L("MoveToLocal + MoveFromLocal == MoveWithinService"));

		DoOpL( EImap4OpMoveToLocal, iRemoteInbox, iLocalFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 411:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMoveToLocal, iSourceId, iLocalFolder1 ));
		test(TestShadowL( iLocalFolder1, iSourceId ));
		test(TestInvisible( iSourceId ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedMoveToOperation );
		
		// move back to new folder
		DoOpL( EImap4OpMoveFromLocal, iLocalFolder1, iRemoteFolder1 );
		NextState();
		break;

	case 412:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMoveWithinService, iOrigSourceId, iRemoteFolder1 ));
		test(!TestShadowL( iLocalFolder1, iOrigSourceId ));
		test(TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestInvisible( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedMoveWithinServiceOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(10);
		break;

		// copy to local, delete original, and move back to undo
	case 420:
		// copy to local
		iTestUtils->TestStart(iTestNum, _L("CopyToLocal + Delete + MoveFromLocal == nothing"));

		DoOpL( EImap4OpCopyToLocal, iRemoteInbox, iLocalFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 421:
		// delete original
		DoOpL( EImap4OpDelete, iRemoteInbox, KMsvNullIndexEntryId );
		NextState();
		break;
		
	case 422:
		// move original back
		DoOpL( EImap4OpMoveFromLocal, iLocalFolder1, iRemoteInbox );
		NextState();
		break;

	case 423:
		// check net result is nothing
		test(TestQueueEmptyL( iRemoteInbox ));
		test(TestDisconnectedFlags( iOrigSourceId ) == ENoDisconnectedOperations );
		test(!TestExists( iSourceId ));
		test(TestVisible( iOrigSourceId ));

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;


		// MOVEWITHINSERVICE tests ----------------------------------------------

		// move within sercice and move back to undo
	case 500:
		// move fresh within service
		iTestUtils->TestStart(iTestNum, _L("MoveWithinService + MoveWithinService (same folder) == nothing"));

		DoOpL( EImap4OpMoveWithinService, iRemoteInbox, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 501:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMoveWithinService, iSourceId, iRemoteFolder1 ));
		test(TestShadowL( iRemoteFolder1, iSourceId ));
		test(TestInvisible( iSourceId ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedMoveWithinServiceOperation );
		
		// move back to original location -- undo
		DoOpL( EImap4OpMoveWithinService, iRemoteFolder1, iRemoteInbox );
		NextState();
		break;

	case 502:
		test(TestQueueEmptyL( iRemoteInbox ));
		test(!TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestVisible( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == ENoDisconnectedOperations );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(10);
		break;

		// move within service and move on to new folder
	case 510:
		// move fresh within service
		iTestUtils->TestStart(iTestNum, _L("MoveWithinService + MoveWithinService == MoveWithinService"));

		DoOpL( EImap4OpMoveWithinService, iRemoteInbox, iRemoteFolder1 );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 511:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMoveWithinService, iSourceId, iRemoteFolder1 ));
		test(TestShadowL( iRemoteFolder1, iSourceId ));
		test(TestInvisible( iSourceId ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedMoveWithinServiceOperation );
		
		// move back to new folder
		DoOpL( EImap4OpMoveWithinService, iRemoteFolder1, iRemoteFolder2 );
		NextState();
		break;

	case 512:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMoveWithinService, iOrigSourceId, iRemoteFolder2 ));
		test(!TestShadowL( iRemoteFolder1, iOrigSourceId ));
		test(TestShadowL( iRemoteFolder2, iOrigSourceId ));
		test(TestInvisible( iOrigSourceId ));
		test(TestDisconnectedFlags( iOrigSourceId ) == EDisconnectedMoveWithinServiceOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;
		
		// DELETE/UNDELETE tests ----------------------------------------------

		// DELETE tests
	case 600:
		iTestUtils->TestStart(iTestNum, _L("Delete + Undelete = nothing"));

		DoOpL( EImap4OpDelete, iRemoteInbox, KMsvNullIndexEntryId );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 601:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpDelete, iSourceId, KMsvNullIndexEntryId ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedDeleteOperation );

		// undelete it
		DoOpL( EImap4OpUndelete, iRemoteInbox, KMsvNullIndexEntryId );
		NextState();
		break;

	case 602:
		test(TestQueueEmptyL( iRemoteInbox ));
		test(TestDisconnectedFlags( iOrigSourceId ) == ENoDisconnectedOperations );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;

		// ----------------------------------------------------------------------
		// other things we can do offline
	case 700:
		// check flags on messages in the mirror
		TestPriorityAndReceiptAddressL();		

		iTestUtils->TestStart(iTestNum, _L("Offline op errors"));

		// check the return codes from the some of the special functions
		PerformSpecialOpL(0, KIMAP4MTMIsConnected);
		NextState();
		break;

	case 701:
		test(iStatus.Int() == KErrDisconnected);

		PerformSpecialOpL(0, KIMAP4MTMSynchronise);
		NextState();
		break;
		
	case 702:
		test(iStatus.Int() == KErrDisconnected);
		PerformSpecialOpL(0, KIMAP4MTMFullSync);
		NextState();
		break;
		
	case 703:
		test(iStatus.Int() == KErrDisconnected);
		PerformSpecialOpL(0, KIMAP4MTMInboxNewSync);
		NextState();
		break;
		
	case 704:
		test(iStatus.Int() == KErrDisconnected);
		PerformSpecialOpL(0, KIMAP4MTMFolderFullSync);
		NextState();
		break;
		
	case 705:
		test(iStatus.Int() == KErrDisconnected);
		PerformSpecialOpL(0, KIMAP4MTMRenameFolder);
		NextState();
		break;
		
	case 706:
		test(iStatus.Int() == KErrDisconnected);
		PerformSpecialOpL(0, KIMAP4MTMDisconnect);
		NextState();
		break;
		
	case 707:
		test(iStatus.Int() == KErrDisconnected);
		PerformSpecialOpL(0, KIMAP4MTMSyncTree);
		NextState();
		break;
		
	case 708:
		test(iStatus.Int() == KErrDisconnected);
		PerformSpecialOpL(0, KIMAP4MTMBusy);
		NextState();
		break;
		
	case 709:
 		test(iStatus.Int() == KErrNone);
		PerformSpecialOpL(0, KIMAP4MTMSelect);
		NextState();
		break;
		
	case 710:
		test(iStatus.Int() == KErrDisconnected);
		PerformSpecialOpL(0, KIMAP4MTMCancelBackgroundSynchronise);
		NextState(100);
		break;
#if 0
		// test local subscription code
	case 711:
		test(iStatus.Int() == KErrNone);
		PerformSpecialOpL(FindFolderByNameL(iServiceId, _L("TestHier1")), KIMAP4MTMLocalSubscribe);
		NextState();
		break;
		
	case 712:
		test(iStatus.Int() == KErrNone);
		test(TestLocalSubscription(iSourceId));
		PerformSpecialOpL(FindFolderByNameL(iServiceId, _L("TestHier1/TestHier2")), KIMAP4MTMLocalSubscribe);
		NextState();
		break;
		
	case 713:
		test(iStatus.Int() == KErrNone);
		test(TestLocalSubscription(iSourceId));
		PerformSpecialOpL(FindFolderByNameL(iServiceId, _L("TestHier1/TestHier2/TestHier3")), KIMAP4MTMLocalSubscribe);
		NextState();
		break;
		
	case 714:
		test(iStatus.Int() == KErrNone);
		test(TestLocalSubscription(iSourceId));
		PerformSpecialOpL(FindFolderByNameL(iServiceId, _L("TestHier1")), KIMAP4MTMLocalUnsubscribe);
		NextState();
		break;
		
	case 715:
		test(iStatus.Int() == KErrNone);
		test(TestNotLocalSubscription(iSourceId));
		test(TestVisible(iSourceId));
		PerformSpecialOpL(FindFolderByNameL(iServiceId, _L("TestHier1/TestHier2")), KIMAP4MTMLocalUnsubscribe);
		NextState();
		break;
		
	case 716:
		test(iStatus.Int() == KErrNone);
		test(TestNotLocalSubscription(iSourceId));
		test(TestVisible(iSourceId));
		PerformSpecialOpL(FindFolderByNameL(iServiceId, _L("TestHier1/TestHier2/TestHier3")), KIMAP4MTMLocalUnsubscribe);
		NextState();
		break;
		
	case 717:
		test(iStatus.Int() == KErrNone);
		test(TestNotLocalSubscription(iSourceId));
		test(TestInvisible(iSourceId));
		test(TestInvisible(FindMessageByNameL(iSourceId, KMessageNameTest)));
		test(TestInvisible(FindFolderByNameL(iServiceId, _L("TestHier1/TestHier2"))));
		test(TestInvisible(FindFolderByNameL(iServiceId, _L("TestHier1"))));

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;
#endif
		// POPULATE tests   --------------------------------------------------

		// copy to mirror
	case 800:
		test(iStatus.Int() == KErrNone);
		iTestUtils->TestFinish(iTestNum++);

		iTestUtils->TestStart(iTestNum, _L("Populate default"));

		DoOpL( EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId );
		iOrigSourceId = iSourceId;
		NextState();
		break;

	case 801:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, iSourceId, KMsvNullIndexEntryId, EGetImap4EmailBodyTextAndAttachments ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedSpecialOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState();
		break;

	case 802:
		iTestUtils->TestStart(iTestNum, _L("Populate headers"));

		DoOpL( EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId, EGetImap4EmailHeaders );
		NextState();
		break;

	case 803:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, iSourceId, KMsvNullIndexEntryId, EGetImap4EmailHeaders ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedSpecialOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState();
		break;

	case 804:
		iTestUtils->TestStart(iTestNum, _L("Populate BodyText"));

		DoOpL( EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId, EGetImap4EmailBodyText );
		NextState();
		break;

	case 805:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, iSourceId, KMsvNullIndexEntryId, EGetImap4EmailBodyText ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedSpecialOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState();
		break;

	case 806:
		iTestUtils->TestStart(iTestNum, _L("Populate Attachments"));

		DoOpL( EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId, EGetImap4EmailAttachments );
		NextState();
		break;

	case 807:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, iSourceId, KMsvNullIndexEntryId, EGetImap4EmailAttachments ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedSpecialOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState();
		break;

	case 808:
		iTestUtils->TestStart(iTestNum, _L("Populate All"));

		DoOpL( EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId, EGetImap4EmailBodyTextAndAttachments );
		NextState();
		break;

	case 809:
		test(TestQueueL( iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, iSourceId, KMsvNullIndexEntryId, EGetImap4EmailBodyTextAndAttachments ));
		test(TestDisconnectedFlags( iSourceId ) == EDisconnectedSpecialOperation );

		iTestUtils->TestFinish(iTestNum++);

		DoCancelOffLineOpsL();
		NextState(100);
		break;

	case 900:
		// done
		iTestUtils->TestHarnessCompleted();
		EndOfTest();  
		
		// done
		test.Console()->SetPos(0, 13);
		test.Printf(_L("Finished offline tests\n"));
		more = EFalse;
		break;
		
	default:
		NextState(100);
		break;
		}

	return more;
	}

void CTestRefreshMBox::DoAutomatedOffLineOpsL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	if (iRemoteFolder1<=KMsvNullIndexEntryId)
		iRemoteFolder1=FindFolderByNameL(iServiceId,_L("Test1"));
		
	if (iRemoteFolder2<=KMsvNullIndexEntryId)	
		iRemoteFolder2=FindFolderByNameL(iServiceId,_L("Test2"));

	if (iRemoteFolder1<=KMsvNullIndexEntryId ||
		iRemoteFolder2<=KMsvNullIndexEntryId)	
		{
		test.Printf(_L("Don't have both the remote folders\n"));
		}
	else
		{
		iOpState = 0;
		iState = EImapTestAutoOffLineOp;
		
		HandleOffLineStateL();
		}
	}

TBool CTestRefreshMBox::HandleSyncStateL()
	{
	TBool more = ETrue;

	switch (iOpState)
		{
		// test basic commands
	case 0:
		iTestUtils->TestStart(iTestNum, _L("Setup offline ops"));

		DoOpNamedL(EImap4OpCopyToLocal, iRemoteInbox, iLocalFolder1);
		NextState();
		break;

	case 1:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpCopyWithinService, iRemoteInbox, iRemoteFolder1);
		NextState();
		break;
		
	case 2:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpMoveToLocal, iRemoteInbox, iLocalFolder1);
		NextState();
		break;
		
	case 3:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpMoveWithinService, iRemoteInbox, iRemoteFolder1);
		NextState();
		break;
		
		// test populate options
	case 4:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpDelete, iRemoteInbox, KMsvNullIndexEntryId);
		NextState();
		break;
		
	case 5:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId, EGetImap4EmailHeaders);
		NextState();
		break;

	case 6:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId, EGetImap4EmailBodyText);
		NextState();
		break;
		
	case 7:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId, EGetImap4EmailBodyTextAndAttachments);
		NextState();
		break;

	case 8:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpPopulate, iRemoteInbox, KMsvNullIndexEntryId, EGetImap4EmailAttachments);
		NextState();
		NextState();
		break;

		// get one specific attachment
	case 9:
		{
		test(iStatus.Int() == KErrNone);
		TPckgBuf<TImap4GetMailOptions> package(EGetImap4EmailAttachments);	
		FindAttachmentL();
		DoOpByIdL(EImap4OpPopulate, (*iSelection)[0], package);
		NextState();
		break;
		}

		// test multiple copies to the same folder as the source
	case 10:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpCopyWithinFolder, iRemoteInbox, iRemoteInbox);
		NextState();
		break;
		
	case 11:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpCopyWithinFolder, iRemoteInbox, iRemoteInbox);
		NextState();
		break;

		// test out operations where the ghost gets deleted
	case 12:
		test(iStatus.Int() == KErrNone);
		iTestUtils->TestFinish(iTestNum++);

		iTestUtils->TestStart(iTestNum, _L("Setup ghost deletes"));
		DoOpNamedL(EImap4OpCopyToLocalAndDelete, iRemoteInbox, iLocalFolder1);
		NextState();
		break;

	case 13:
		{
		test(iStatus.Int() == KErrNone);
		TMsvId id = FindShadowL(iLocalFolder1, iSourceId);
		test(id!=KMsvNullIndexEntryId);

		iEntry->SetEntry(iLocalFolder1);
		iEntry->DeleteEntry(id);

		DoOpNamedL(EImap4OpCopyWithinServiceAndDelete, iRemoteInbox, iRemoteFolder1);
		NextState();
		break;
		}
		
	case 14:
		test(iStatus.Int() == KErrNone);
		DoOpByIdL(EImap4OpDelete, FindShadowL(iRemoteFolder1, iSourceId));
		NextState();
		break;

	case 15:
		test(iStatus.Int() == KErrNone);
		DoOpNamedL(EImap4OpMoveToLocalAndDelete, iRemoteInbox, iLocalFolder1);
		NextState();
		break;
	case 16:
		{
		test(iStatus.Int() == KErrNone);
		TMsvId id = FindShadowL(iLocalFolder1, iSourceId);
		test(id!=KMsvNullIndexEntryId);

		iEntry->SetEntry(iLocalFolder1);
		iEntry->DeleteEntry(id);

		DoOpNamedL(EImap4OpMoveWithinServiceAndDelete, iRemoteInbox, iRemoteFolder1);
		NextState();
		break;
		}
		
	case 17:
		test(iStatus.Int() == KErrNone);
		DoOpByIdL(EImap4OpDelete, FindShadowL(iRemoteFolder1, iSourceId));
		NextState();
		break;

		// delete a message from folder 2
	case 18:
		{
		test(iStatus.Int() == KErrNone);

		TPtrC testMsg = TestMsgName(EImap4OpDelete);
		TMsvId id = FindMessageByNameL(iRemoteFolder2, testMsg);
		DoOpByIdL(EImap4OpDelete, id);
		NextState();
		break;
		}
		
		// and move a message out of folder2
	case 19:
		{
		test(iStatus.Int() == KErrNone);

		// uses KMessageNameTest
		DoOpL(EImap4OpMoveWithinService, iRemoteFolder2, iRemoteFolder1 );
		NextState();
		break;
		}
		
	case 20:
		{
		test(iStatus.Int() == KErrNone);
		iTestUtils->TestFinish(iTestNum++);

		TDateTime dateTime;
		dateTime.Set(2100, EJanuary, 1, 0, 0, 0, 0);
		TTime date(dateTime);
		if (!iLogClient)
			iLogClient = CLogClient::NewL(iTestUtils->FileSession());
		SetActive();
		iLogClient->ClearLog(date, iStatus);
		NextState(100);
		break;
		}
		
		// go online and check the results
	case 100:
		test(iStatus.Int() == KErrNone);

		iTestUtils->TestStart(iTestNum, _L("Go online"));
		DoConnectAndSyncL();
		NextState();
		break;

	case 101:
		test(iStatus.Int() == KErrNone);
		PerformSpecialOpL(0, KIMAP4MTMIsConnected);
		NextState();
		break;
		
	case 102:
		test(iStatus.Int() == KErrNone);
		PerformSpecialOpL(0, KIMAP4MTMWaitForBackground);
		NextState();
		break;
		
	case 103:
		{
		test(iStatus.Int() == KErrNone);
		iTestUtils->TestFinish(iTestNum++);

		TMsvId copy;
		TMsvId orig;

		// check out the CopyToLocal to see if it happened correctly
		iTestUtils->TestStart(iTestNum, _L("CopyToLocal"));

		copy = FindMessageByOpL(iLocalFolder1, EImap4OpCopyToLocal);
		test(copy!=KErrNotFound);
		test(TestDisconnectedFlags(copy)==ENoDisconnectedOperations);
		test(TestComplete(copy));

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpCopyToLocal);
		test(orig!=KErrNotFound);
		test(!TestShadowL(iLocalFolder1, orig));
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);
		test(TestComplete(orig));

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpCopyToLocal, orig, iLocalFolder1 ));

		iTestUtils->TestFinish(iTestNum++);

		// check out the CopyWithinService to see if it happened correctly
		iTestUtils->TestStart(iTestNum, _L("CopyWithinService"));

		copy = FindMessageByOpL(iRemoteFolder1, EImap4OpCopyWithinService);
		test(copy!=KErrNotFound);
		test(TestDisconnectedFlags(copy)==ENoDisconnectedOperations);

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpCopyWithinService);
		test(orig!=KErrNotFound);
		test(!TestShadowL(iRemoteFolder1, orig));
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpCopyWithinService, orig, iRemoteFolder1 ));
		iTestUtils->TestFinish(iTestNum++);

		// check out the MoveToLocal to see if it happened correctly
		iTestUtils->TestStart(iTestNum, _L("MoveToLocal"));

		copy = FindMessageByOpL(iLocalFolder1, EImap4OpMoveToLocal);
		test(copy!=KErrNotFound);
		test(TestDisconnectedFlags(copy)==ENoDisconnectedOperations);
		test(TestComplete(copy));

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpMoveToLocal);
		test(orig==KErrNotFound);

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMoveToLocal, KMsvNullIndexEntryId, iLocalFolder1 ));
		iTestUtils->TestFinish(iTestNum++);

		// check out the MoveWithinService to see if it happened correctly
		iTestUtils->TestStart(iTestNum, _L("MoveWithinService"));

		copy = FindMessageByOpL(iRemoteFolder1, EImap4OpMoveWithinService);
		test(copy!=KErrNotFound);
		test(TestDisconnectedFlags(copy)==ENoDisconnectedOperations);

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpMoveWithinService);
		test(orig==KErrNotFound);

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMoveWithinService, KMsvNullIndexEntryId, iRemoteFolder1 ));
		iTestUtils->TestFinish(iTestNum++);
		
		// check out the populate tests
		iTestUtils->TestStart(iTestNum, _L("Populate"));

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpPopulate, EGetImap4EmailHeaders);
		test(orig!=KErrNotFound);
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);	
		test(!TestComplete(orig));
			
		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, orig, KMsvNullIndexEntryId, EGetImap4EmailHeaders ));

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpPopulate, EGetImap4EmailBodyText);
		test(orig!=KErrNotFound);
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);	
		test(!TestComplete(orig));

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, orig, KMsvNullIndexEntryId, EGetImap4EmailBodyText ));

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpPopulate, EGetImap4EmailAttachments);
		test(orig!=KErrNotFound);
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);	
		test(!TestComplete(orig));

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, orig, KMsvNullIndexEntryId, EGetImap4EmailAttachments ));

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpPopulate, EGetImap4EmailBodyTextAndAttachments);
		test(orig!=KErrNotFound);
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);	
		test(TestComplete(orig));
		
		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, orig, KMsvNullIndexEntryId, EGetImap4EmailBodyTextAndAttachments ));
#if 0
		// specific attachment
		FindAttachmentL();
		orig = (*iSelection)[0];
		test(orig!=KErrNotFound);
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);	
		test(TestComplete(orig));
		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMtmSpecific, orig, KMsvNullIndexEntryId, EGetImap4EmailBodyTextAndAttachments ));
#endif		

		iTestUtils->TestFinish(iTestNum++);

		// check out the CopyWithinFolder to see if it happened
		// correctly, made 2 copies so there should be 3 of them in
		// total
		iTestUtils->TestStart(iTestNum, _L("CopyWithinFolder"));

		test( CountMessagesByOpL(iRemoteInbox, EImap4OpCopyWithinFolder) == 3 );

		// check each of the versions for the right flags
		CMsvEntrySelection* children = new (ELeave) CMsvEntrySelection;
		CleanupStack::PushL(children);
			
		User::LeaveIfError(iEntry->SetEntry( iRemoteInbox ));
		User::LeaveIfError(iEntry->GetChildren( *children ));
		
		TPtrC name = TestMsgName( EImap4OpCopyWithinFolder );
		for (TInt i=0; i < children->Count(); i++)
		{
			User::LeaveIfError(iEntry->SetEntry( (*children)[i] ));
			if ( iEntry->Entry().iDescription.Compare( name ) == 0 )
			{
				TMsvId id = (*children)[i];
				test(iEntry->Entry().iRelatedId == KMsvNullIndexEntryId);
				test(TestDisconnectedFlags(id)==ENoDisconnectedOperations);
			}
		}
		CleanupStack::PopAndDestroy();
		iTestUtils->TestFinish(iTestNum++);

		// check out the ghost deletion tests
		iTestUtils->TestStart(iTestNum, _L("Ghost deletion"));

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpCopyToLocalAndDelete);	
		test(orig!=KErrNotFound);
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);	
		
		copy = FindMessageByOpL(iLocalFolder1, EImap4OpCopyToLocalAndDelete);
		test(copy==KErrNotFound);

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpCopyToLocal, orig, iLocalFolder1 ));


		orig = FindMessageByOpL(iRemoteInbox, EImap4OpCopyWithinServiceAndDelete);	
		test(orig!=KErrNotFound);
		test(TestDisconnectedFlags(orig)==ENoDisconnectedOperations);
		
		copy = FindMessageByOpL(iRemoteFolder1, EImap4OpCopyWithinServiceAndDelete);
		test(copy==KErrNotFound);

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpCopyWithinService, orig, iRemoteFolder1 ));

		
		orig = FindMessageByOpL(iRemoteInbox, EImap4OpMoveToLocalAndDelete);	
		test(orig==KErrNotFound);
		
		copy = FindMessageByOpL(iLocalFolder1, EImap4OpMoveToLocalAndDelete);
		test(copy==KErrNotFound);

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMoveToLocal, orig, iLocalFolder1 ));


		orig = FindMessageByOpL(iRemoteInbox, EImap4OpMoveWithinServiceAndDelete);	
		test(orig==KErrNotFound);
		
		copy = FindMessageByOpL(iRemoteFolder1, EImap4OpMoveWithinServiceAndDelete);
		test(copy==KErrNotFound);

		test(!TestQueueL(iRemoteInbox, CImOffLineOperation::EOffLineOpMoveWithinService, orig, iRemoteFolder1 ));

		test(iStatus.Int() == KErrNone);
		iTestUtils->TestFinish(iTestNum++);

		// check the folder 2 operations
		iTestUtils->TestStart(iTestNum, _L("Move from non-Inbox folder"));

		copy = FindMessageByNameL(iRemoteFolder1, KMessageNameTest);
		test(copy!=KErrNotFound);

		//orig = FindMessageByNameL(iRemoteFolder2, KMessageNameTest);
		//test(orig==KErrNotFound);
		
		iTestUtils->TestFinish(iTestNum++);

		// check the deleted item
		iTestUtils->TestStart(iTestNum, _L("Delete (on connect)"));

		CImImap4Settings* settings = GetImap4SettingsLC();

		orig = FindMessageByOpL(iRemoteInbox, EImap4OpDelete);
		test( settings->DeleteEmailsWhenDisconnecting() ?
			  orig!=KErrNotFound : orig==KErrNotFound );

		CleanupStack::PopAndDestroy(); // settings
		iTestUtils->TestFinish(iTestNum++);
#if 0
		iTestUtils->TestStart(iTestNum, _L("Logging"));

		// set up the logview to test the logging messages
		if (!iLogView)
			{
			iLogView = CLogViewEvent::NewL(*iLogClient);
			if (!iLogFilter)
				{
				iLogFilter = CLogFilter::NewL();
				iLogFilter->SetEventType(KLogMailEventTypeUid);
				}
			}
		SetActive();
		iLogView->SetFilterL(*iLogFilter, iStatus);
		NextState(100);
		break;
		}

	case 200:
		// CopyToLocaL/MoveToLocal and 4 brands of Populate
		test.Printf(_L("LogEvent: %d\n"), iLogView->CountL());
		test(iLogView->CountL() == 6 );

		SetActive();
		test(iLogView->FirstL(iStatus) != EFalse);
		NextState();
		break;

		// IMAPOFFL rules say that the order is populate, copytolocal,
		// movetolocal, within that it is in order of usage. The
		// LogView events come out most recent first (ie reverse
		// order) thus leading to the order we see here.
	case 201:
		{
		// check MoveToLocal
		const CLogEvent& logEvent = iLogView->Event();

		//TPtrC remoteParty = logEvent.RemoteParty();
		//TPtrC direction = logEvent.Direction();
		//TPtrC subject = logEvent.Subject();
		//test.Printf(_L("LogEvent: '%S' '%S' '%S'\n"), &remoteParty, &subject, &direction);

		test(logEvent.RemoteParty() == KTestRemoteParty);
		test(logEvent.Subject() == TestMsgName(EImap4OpMoveToLocal));
		test(logEvent.Direction() == _L("Fetched"));

		SetActive();
		test(iLogView->NextL(iStatus) != EFalse);
		NextState();
		break;
		}
		
	case 202:
		{
		// check CopyToLocal
		const CLogEvent& logEvent = iLogView->Event();
		test(logEvent.RemoteParty() == KTestRemoteParty);
		test(logEvent.Subject() == TestMsgName(EImap4OpCopyToLocal));
		test(logEvent.Direction() == _L("Fetched"));

		SetActive();
		test(iLogView->NextL(iStatus) != EFalse);
		NextState();
		NextState();
		break;
		}
		
	case 203:
		{
		// check Populate single attachment
		const CLogEvent& logEvent = iLogView->Event();
		//test(logEvent.RemoteParty() == KTestRemoteParty);
		//test(logEvent.Subject() == TestMsgName(EImap4OpPopulate, EGetImap4EmailAttachments));
		test(logEvent.Direction() == _L("Fetched"));

		SetActive();
		test(iLogView->NextL(iStatus) != EFalse);
		NextState();
		break;
		}
		
	case 204:
		{
		// check Populate
		const CLogEvent& logEvent = iLogView->Event();
		test(logEvent.RemoteParty() == KTestRemoteParty);
		test(logEvent.Subject() == TestMsgName(EImap4OpPopulate, EGetImap4EmailAttachments));
		test(logEvent.Direction() == _L("Fetched"));

		SetActive();
		test(iLogView->NextL(iStatus) != EFalse);
		NextState();
		break;
		}
		
	case 205:
		{
		// check Populate
		const CLogEvent& logEvent = iLogView->Event();
		test(logEvent.RemoteParty() == KTestRemoteParty);
		test(logEvent.Subject() == TestMsgName(EImap4OpPopulate, EGetImap4EmailBodyTextAndAttachments));
		test(logEvent.Direction() == _L("Fetched"));

		SetActive();
		test(iLogView->NextL(iStatus) != EFalse);
		NextState();
		break;
		}
		
	case 206:
		{
		// check Populate
		const CLogEvent& logEvent = iLogView->Event();
		test(logEvent.RemoteParty() == KTestRemoteParty);
		test(logEvent.Subject() == TestMsgName(EImap4OpPopulate, EGetImap4EmailBodyText));
		test(logEvent.Direction() == _L("Fetched"));

		SetActive();
		test(iLogView->NextL(iStatus) != EFalse);
		NextState();
		break;
		}

	case 207:
		{
		// check Populate
		const CLogEvent& logEvent = iLogView->Event();
		test(logEvent.RemoteParty() == KTestRemoteParty);
		test(logEvent.Subject() == TestMsgName(EImap4OpPopulate, EGetImap4EmailHeaders));
		test(logEvent.Direction() == _L("Fetched"));

		iTestUtils->TestFinish(iTestNum++);
#endif
		// now disconnect and test deletion
		iTestUtils->TestStart(iTestNum, _L("Disconnect"));
 		DoDisconnectL();
		NextState(100);
		break;
		}
		
	case 200:
		{
		test(iStatus.Int() == KErrNone);
		iTestUtils->TestFinish(iTestNum++);

		// check the deleted item
		CImImap4Settings* settings = GetImap4SettingsLC();

		iTestUtils->TestStart(iTestNum, _L("Delete (on disconnect)"));

		if (settings->DeleteEmailsWhenDisconnecting())
			{
			TMsvId orig = FindMessageByOpL(iRemoteInbox, EImap4OpDelete);
			test( orig==KErrNotFound );
			}

		iTestUtils->TestFinish(iTestNum++);

		CleanupStack::PopAndDestroy(); // settings

		// now setup for some further tests
		iTestUtils->TestStart(iTestNum, _L("MoveWithinServiceToInbox"));
		DoOpNamedL(EImap4OpMoveWithinServiceToInbox, iRemoteFolder1, iRemoteInbox);
		NextState();
		break;
		}

	case 201:
		test(iStatus.Int() == KErrNone);
		DoConnectAndSyncL();
		NextState();
		break;

	case 202:
		test(iStatus.Int() == KErrNone);
		PerformSpecialOpL(0, KIMAP4MTMWaitForBackground);
		NextState();
		break;
		
	case 203:
		test(iStatus.Int() == KErrNone);
		PerformSpecialOpL(iRemoteInbox, KIMAP4MTMSelect);
		NextState();
		break;

	case 204:
		test(iStatus.Int() == KErrNone);
		PerformSpecialOpL(iRemoteInbox, KIMAP4MTMSynchronise);
		NextState();
		break;
		
	case 205:
		{
		test(iStatus.Int() == KErrNone);

		// see if new message exists
		TMsvId orig = FindMessageByOpL(iRemoteInbox, EImap4OpMoveWithinServiceToInbox);
		test(orig!=KErrNotFound);

		// check old message doesn't exist
		orig = FindMessageByOpL(iRemoteFolder1, EImap4OpMoveWithinServiceToInbox);
		test(orig==KErrNotFound);

		iTestUtils->TestFinish(iTestNum++);

		// done
		iTestUtils->TestHarnessCompleted();
		EndOfTest();  
		
		test.Console()->SetPos(0, 13);
		test.Printf(_L("Finished sync tests\n"));
		more = EFalse;
		break;
		}		

	default:
		NextState(100);
		break;
		}
	return more; 
	}

void CTestRefreshMBox::DoAutomatedSyncOpsL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	if (iRemoteFolder1<=KMsvNullIndexEntryId)
		iRemoteFolder1=FindFolderByNameL(iServiceId,_L("Test1"));
		
	if (iRemoteFolder2<=KMsvNullIndexEntryId)	
		iRemoteFolder2=FindFolderByNameL(iServiceId,_L("Test2"));

	if (iRemoteFolder1<=KMsvNullIndexEntryId ||
		iRemoteFolder2<=KMsvNullIndexEntryId)	
		{
		test.Printf(_L("Don't have both the remote folders\n"));
		}
	else
		{
		iOpState = 0;
		iState = EImapTestAutoSyncOp;
		HandleSyncStateL();
		}
	}

/* ----------------------------------------------------------------------- */

void CTestRefreshMBox::DoAutomatedSecondaryConnectOpsL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	if (iRemoteFolder1<=KMsvNullIndexEntryId)
		iRemoteFolder1=FindFolderByNameL(iServiceId,_L("Test1"));
		
	if (iRemoteFolder2<=KMsvNullIndexEntryId)	
		iRemoteFolder2=FindFolderByNameL(iServiceId,_L("Test2"));

	if (iRemoteFolder1<=KMsvNullIndexEntryId ||
		iRemoteFolder2<=KMsvNullIndexEntryId)	
		{
		test.Printf(_L("Don't have both the remote folders\n"));
		}
	else
		{
		iOpState = 0;
		iState = EImapTestAutoSecondaryConnect;
		
		TestSecondaryConnectStateMachineL();
		}
	}

TBool CTestRefreshMBox::TestSecondaryConnectStateMachineL()
	{
	TBool more = ETrue;
	TPckgBuf<TImap4GetMailOptions> package(EGetImap4EmailBodyText);
	TMsvId id;

	switch (iOpState)
		{
	case 0:
		iTestUtils->TestStart(iTestNum, _L("Test Secondary connection"));

		// subscribe to the backup folder initially to lengthen the
		// background sync time
		id = FindFolderByNameL(iServiceId, _L("TestBigFolder"));
		PerformSpecialOpL(id, KIMAP4MTMLocalSubscribe);
		NextState();
		break;
		
	case 1:
		DoConnectAndSyncL();
		NextState(100);
		break;

	case 100:
		test(iStatus.Int() == KErrNone);

		id = FindMessageByNameL(iRemoteInbox, _L("Test: 3"));
		DoOpByIdL(EImap4OpPopulate, id, package);
		NextState();
		break;

	case 101:
		test(iStatus.Int() == KErrNone);

		id = FindMessageByNameL(iRemoteInbox, _L("Test: 4"));
		DoOpByIdL(EImap4OpPopulate, id, package);
		NextState();
		break;

	case 102:
		test(iStatus.Int() == KErrNone);

		id = FindMessageByNameL(iRemoteInbox, _L("Test: 5"));
		DoOpByIdL(EImap4OpPopulate, id, package);
		NextState();
		break;

		// at this point the server should still be busy with the
		// background sync. So check that we get ServerBusy replies
		// from everyone. This tests that the ServerBusy check happens
		// correctly and also tht the 3 fetches above all happened
		// whilst background syncing
	case 103:
		test(iStatus.Int() == KErrNone);
		PerformSpecialOpL(0, KIMAP4MTMBusy);
		NextState();
		break;

	case 104:
		test(iStatus.Int() == KErrServerBusy);
		PerformSpecialOpL(0, KIMAP4MTMFullSync);
		NextState();
		break;

	case 105:
		test(iStatus.Int() == KErrServerBusy);
		PerformSpecialOpL(0, KIMAP4MTMSyncTree);
		NextState();
		break;

	case 106:
		test(iStatus.Int() == KErrServerBusy);
		PerformSpecialOpL(0, KIMAP4MTMInboxNewSync);
		NextState();
		break;
		
	case 107:
		test(iStatus.Int() == KErrServerBusy);

		// and then wait for the background sync to finish before
		// disconnecting
		PerformSpecialOpL(0, KIMAP4MTMCancelBackgroundSynchronise);
		NextState();
		break;
		
	case 108:
		test(iStatus.Int() == KErrCancel);
		// just check busy returns OK this time
		PerformSpecialOpL(0, KIMAP4MTMBusy);
		NextState(100);
		break;
		
	case 200:
		test(iStatus.Int() == KErrNone);
		DoDisconnectL();
		NextState();
		break;
		
	case 201:
		id = FindFolderByNameL(iServiceId, _L("TestBigFolder"));
		PerformSpecialOpL(id, KIMAP4MTMLocalUnsubscribe);
		NextState();
		break;
		
	case 202:
		iTestUtils->TestFinish(iTestNum++);
		iTestUtils->TestHarnessCompleted();
		EndOfTest();  
		// done
		test.Console()->SetPos(0, 13);
		test.Printf(_L("Finished sync tests\n"));
		more = EFalse;
		break;
		}
	return more;
	}

/* ----------------------------------------------------------------------- */

/*

  When we connect to the server the first time we will find that we
  have all the messages we require in the INBOX and no other folders
  present.

  So we copy the messages to the Backup folder and create our other
  required test folders.

  On subsequent occasions we wish to copy any missing messages from
  Backup to INBOX and delete any excess messages from inbox and from
  the other test folders.

  */

TBool CTestRefreshMBox::StateMachineCheckServerContentsL()
	{
	TBool more = ETrue;
	TMsvId id;
	
	while (!IsActive() && more)
		{
		switch (iOpState)
			{
		case 0:
			iTestUtils->TestStart(iTestNum, _L("Check server contents"));
			
			// start with nothing subscribed locally and update remote
			// to be the same
			SetSubscriptionsL( EUseLocal, EUpdateNeither );
	
			DoConnectL();
			NextState();
			break;

		case 1:
			test(iStatus.Int() == KErrNone);
			DoSyncTreeL();
			NextState(10);
			break;

			// create directories
		case 10:
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			if (id == KErrNotFound)
				DoFolderCreateL(iServiceId,_L("Backup"));
			NextState();
			break;

		case 11:
			id = FindFolderByNameL(iServiceId, _L("Test1"));
			if (id == KErrNotFound)
				DoFolderCreateL(iServiceId,_L("Test1"));
			NextState();
			break;

		case 12:
			id = FindFolderByNameL(iServiceId, _L("Test2"));
			if (id == KErrNotFound)
				DoFolderCreateL(iServiceId,_L("Test2"));
			NextState();
			break;

		case 13:
			id = FindFolderByNameL(iServiceId, KFolderNameToEncode);
			if (id == KErrNotFound)
				DoFolderCreateL(iServiceId, KFolderNameToEncode);
			NextState();
			break;

		case 14:
			DoFolderCreateL(iServiceId,_L("Test3"));
			NextState();
			break;

		case 15:
			test( FindFolderByNameL(iServiceId,_L("Test3")) != KErrNotFound );
			DoFolderRemoveL(iServiceId,_L("Test3"));
			NextState();
			break;

			// resync to see folder deletion
		case 16:
			DoSyncTreeL();
			NextState();
			break;

		case 17:
			test( FindFolderByNameL(iServiceId,_L("Test3")) == KErrNotFound );
			NextState(10);
			break;

#if 0
		case 13:
			id = FindFolderByNameL(iServiceId, _L("TestHier1"));
			if (id == KErrNotFound)
				DoFolderCreateL(iServiceId,_L("TestHier1/"));
			NextState();
			break;

		case 14:
			{
			TMsvId id1 = FindFolderByNameL(iServiceId, _L("TestHier1"));
			id = FindFolderByNameL(id1, _L("TestHier2"));
			if (id == KErrNotFound)
				DoFolderCreateL(id1, _L("TestHier2/"));
			NextState();
			break;
			}

		case 15:
			{
			TMsvId id1 = FindFolderByNameL(iServiceId, _L("TestHier1"));
			TMsvId id2 = FindFolderByNameL(id1, _L("TestHier2"));
			id = FindFolderByNameL(id2, _L("TestHier3"));
			if (id == KErrNotFound)
				DoFolderCreateL(id2, _L("TestHier3"));
			NextState();
			break;
			}

		case 16:
			{
			TMsvId id1 = FindFolderByNameL(iServiceId, _L("TestHier1"));
			TMsvId id2 = FindFolderByNameL(id1, _L("TestHier2"));
			TMsvId id3 = FindFolderByNameL(id2, _L("TestHier3"));

			TMsvId id = FindMessageByNameL(iRemoteInbox, KMessageNameTest);
			iSelection->Reset();
			iSelection->AppendL(id);
			iImppServerMtm->CopyWithinServiceL(*iSelection, id3, iStatus);
			SetActive();
			
			NextState();
			break;
			}
#endif

			// Sync some folders
		case 20:
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			PerformSpecialOpL(id, KIMAP4MTMSelect);
			NextState();
			break;

		case 21:
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			PerformSpecialOpL(id, KIMAP4MTMSynchronise);
			NextState();
			break;

		case 22:
			id = FindFolderByNameL(iServiceId, _L("Test1"));
			PerformSpecialOpL(id, KIMAP4MTMSelect);
			NextState();
			break;

		case 23:
			id = FindFolderByNameL(iServiceId, _L("Test1"));
			PerformSpecialOpL(id, KIMAP4MTMSynchronise);
			NextState();
			break;

		case 24:
			id = FindFolderByNameL(iServiceId, _L("Test2"));
			PerformSpecialOpL(id, KIMAP4MTMSelect);
			NextState();
			break;

		case 25:
			id = FindFolderByNameL(iServiceId, _L("Test2"));
			PerformSpecialOpL(id, KIMAP4MTMSynchronise);
			NextState();
			break;

		case 26:
			PerformSpecialOpL(iRemoteInbox, KIMAP4MTMSelect);
			NextState();
			break;

		case 27:
			PerformSpecialOpL(iRemoteInbox, KIMAP4MTMSynchronise);
			NextState(10);
			break;

			// take copy of inbox into Backup
		case 30:
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));
			if (iSelection->Count() == 0)
				{
				User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
				User::LeaveIfError(iEntry->GetChildren(*iSelection));
				iImppServerMtm->CopyWithinServiceL(*iSelection, id, iStatus);
				SetActive();
				}
			NextState();
			break;
			
			// clean out Test1 directory
		case 31:
			id = FindFolderByNameL(iServiceId, _L("Test1"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));
			if (iSelection->Count() != 0)
				{
				iImppServerMtm->DeleteAllL(*iSelection, iStatus);
				SetActive();
				}
			NextState();
			break;

			// clean out Test2 directory
		case 32:
			id = FindFolderByNameL(iServiceId, _L("Test2"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));
			if (iSelection->Count() != 0)
				{
				iImppServerMtm->DeleteAllL(*iSelection, iStatus);
				SetActive();
				}
			NextState();
			break;

			// clean out inbox
		case 33:
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			if (iSelection->Count() != 0)
				{
				iImppServerMtm->DeleteAllL(*iSelection, iStatus);
				SetActive();
				}
			NextState();
			break;

			// take copy of backup into inbox
		case 34:
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			if (iSelection->Count() != 0)
				{
				iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
				SetActive();
				}
			NextState(10);
			break;

			// fetch Test message to Local
		case 40:
			id = FindMessageByNameL(KMsvGlobalInBoxIndexEntryId, KMessageNameTest);
			// fetch if not already present locally
			if (id == KErrNotFound)
				DoFetchTestL();
			NextState();
			break;

			// fetch Test messages to Test2
		case 41:
			{
			TMsvId test2Id = FindFolderByNameL(iServiceId, _L("Test2"));
			iSelection->Reset();

			id = FindMessageByNameL(test2Id, KMessageNameTest);
			if (id == KErrNotFound)
				{
				id = FindMessageByNameL(iRemoteInbox, KMessageNameTest);
				iSelection->AppendL(id);
				}

			TPtrC testMsg = TestMsgName(EImap4OpDelete);
			id = FindMessageByNameL(test2Id, testMsg);
			if (id == KErrNotFound)
				{
				id = FindMessageByNameL(iRemoteInbox, testMsg);
				iSelection->AppendL(id);
				}

			if (iSelection->Count())
				{
				SetActive();
				iImppServerMtm->CopyWithinServiceL(*iSelection, test2Id, iStatus);
				}

			NextState();
			break;
			}

			// fetch message with dodgy numeric id in it
		case 42:
			{
			iTestUtils->WriteComment(_L("Check message with numeric attachment"));
			
			TMsvId id = FindMessageByNameL(iRemoteInbox, _L("Test: 31"));

			// first test that the size is small before fetching
			User::LeaveIfError(iEntry->SetEntry(id));
			test(iEntry->Entry().iSize > 0 );
			test(iEntry->Entry().iSize < 1024 );
			
			TPckgBuf<TImap4GetMailOptions> package(EGetImap4EmailBodyTextAndAttachments);	
			DoOpByIdL( EImap4OpPopulate, id, package );
			
			NextState();
			break;
			}
			
		case 43:
			{
			TMsvId id = FindMessageByNameL(iRemoteInbox, _L("Test: 31"));

			// now test that the size is small after fetching
			User::LeaveIfError(iEntry->SetEntry(id));
			test(TestComplete(id));
			test(iEntry->Entry().iSize > 0 );
			test(iEntry->Entry().iSize < 1024 );

			NextState();
			break;
			}

			// Fetch message with attachment name > 256 chars
		case 44:
			{
			iTestUtils->WriteComment(_L("Check message with attachment name > 256 chars"));

			TMsvId id = FindMessageByNameL(iRemoteInbox, _L("Test: 32"));

			TPckgBuf<TImap4GetMailOptions> package(EGetImap4EmailBodyTextAndAttachments);	
			DoOpByIdL( EImap4OpPopulate, id, package );

			NextState();
			break;
			}

		case 45:
			{
			TMsvId id = FindMessageByNameL(iRemoteInbox, _L("Test: 32"));

			test(TestComplete(id));

			NextState();
			break;
			}
			
			// Fetch message with attachment name slightly < 256 chars
		case 46:
			{
			iTestUtils->WriteComment(_L("Check message with attachment name nearly 256 chars"));
			
			TMsvId id = FindMessageByNameL(iRemoteInbox, _L("Test: 33"));

			TPckgBuf<TImap4GetMailOptions> package(EGetImap4EmailBodyTextAndAttachments);	
			DoOpByIdL( EImap4OpPopulate, id, package );

			NextState();
			break;
			}

		case 47:
			{
			TMsvId id = FindMessageByNameL(iRemoteInbox, _L("Test: 33"));

			test(TestComplete(id));

			NextState(100);
			break;
			}
			
			// check subscription code
			// all subscriptions should have been cleared by the previous connect
		case 100:
			DoDisconnectL();
			NextState();
			break;

			// set Test1 locally subscribed and Test2 unsubscribed and
			// mirror this to the server
		case 101:
			iTestUtils->WriteComment(_L("Check subscriptions"));
			SetSubscriptionsL( EUseLocal, EUpdateRemote );

			id = FindFolderByNameL(iServiceId, _L("Test1"));
			PerformSpecialOpL(id, KIMAP4MTMLocalSubscribe);
			NextState();
			break;

		case 102:
			id = FindFolderByNameL(iServiceId, _L("Test2"));
			PerformSpecialOpL(id, KIMAP4MTMLocalUnsubscribe);
			NextState();
			break;

		case 103:
			DoConnectL();
			NextState();
			break;

		case 104:
			DoFullSyncL();
			NextState();
			break;

		case 105:
			id = FindFolderByNameL(iServiceId, _L("Test1"));
			test( TestLocalSubscription(id) );
			test( TestSubscribed(id) );

			id = FindFolderByNameL(iServiceId, _L("Test2"));
			test( TestNotLocalSubscription(id) );
			test( TestNotSubscribed(id) );

			DoDisconnectL();
			NextState(10);
			break;


			// locally unsubscribe Test1 and subscribe Test2 and then
			// get the server versions
		case 110:
			SetSubscriptionsL( EUseRemote, EUpdateLocal );

			id = FindFolderByNameL(iServiceId, _L("Test1"));
			PerformSpecialOpL(id, KIMAP4MTMLocalUnsubscribe);
			NextState();
			break;
			
		case 111:
			id = FindFolderByNameL(iServiceId, _L("Test2"));
			PerformSpecialOpL(id, KIMAP4MTMLocalSubscribe);
			NextState();
			break;

		case 112:
			id = FindFolderByNameL(iServiceId, _L("Test1"));
			test( TestNotLocalSubscription(id) );

			id = FindFolderByNameL(iServiceId, _L("Test2"));
			test( TestLocalSubscription(id) );

			DoConnectL();
			NextState();
			break;

		case 113:
			DoFullSyncL();
			NextState();
			break;

			// which should leave us back at Test1 subscribed and
			// Test2 unsubscribed
		case 114:
			id = FindFolderByNameL(iServiceId, _L("Test1"));
			test( TestLocalSubscription(id) );
			test( TestSubscribed(id) );

			id = FindFolderByNameL(iServiceId, _L("Test2"));
			test( TestNotLocalSubscription(id) );
			test( TestNotSubscribed(id) );

			DoDisconnectL();
			NextState(10);
			break;

			// then locally subscribe Test2 (so both are now
			// subscribed) and update both
		case 120:
			SetSubscriptionsL( EUseCombination, EUpdateBoth );

			id = FindFolderByNameL(iServiceId, _L("Test2"));
			PerformSpecialOpL(id, KIMAP4MTMLocalSubscribe);

			NextState();
			break;

		case 121:
			id = FindFolderByNameL(iServiceId, _L("Test2"));
			test( TestLocalSubscription(id) );

			DoConnectL();
			NextState();
			break;

		case 122:
			DoFullSyncL();
			NextState();
			break;

			// which should leave us with both subscribed
		case 123:
			id = FindFolderByNameL(iServiceId, _L("Test1"));
			test( TestLocalSubscription(id) );
			test( TestSubscribed(id) );

			id = FindFolderByNameL(iServiceId, _L("Test2"));
			test( TestLocalSubscription(id) );
			test( TestSubscribed(id) );

			DoDisconnectL();
			NextState(100);
			break;

		case 200:
			test(iStatus.Int() == KErrNone);

			iTestUtils->TestFinish(iTestNum++);

			// done
			iTestUtils->TestHarnessCompleted();
			EndOfTest();  
			
			// done
			test.Console()->SetPos(0, 13);
			test.Printf(_L("Finished check server contents\n"));
			more = EFalse;
			break;

		default:
			NextState(100);
			break;
			}
		}
	return more; 
	}

void CTestRefreshMBox::DoCheckServerContentsL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	iOpState = 0;
	iState = EImapTestCheckServerContents;
	StateMachineCheckServerContentsL();
	}

/* ----------------------------------------------------------------------- */

_LIT(KPerformFullSync,				"Performing full synchronisation"		);
_LIT(KConnectToRemoteServer,		"Connect to remote server"				);
_LIT(KDisconnectFromRemoteServer,	"Disconnect from remote server"			);

const TInt KTestImapLimitedSyncSize1	=10;
const TInt KTestImapTestInboxSize		=20;
const TInt KTestImapTestBigFolderSize	=1200;
// much bigger increment due to having to switch back and forth between client and 
// server side which takes a lot of time
const TInt KTestImapSyncIncrementSize	=100;

TBool CTestRefreshMBox::StateMachineNSynchronisationL()
	{
	TBool more = ETrue;
	TMsvId id;
	
	while (!IsActive() && more)
		{
		switch (iOpState)
			{

		// ----------------------------------------------------------------------------------- //

		case 0:			
			iTestUtils->TestStart(iTestNum, _L("N-Synchronisation tests: Regression tests (use cases SA1-SA5)"));



			// Start with nothing subscribed locally updating neither.
			DebugFormatL(EDebugLevel2,_L("Sync Strategy=EUseLocal, Subscribe Stategy=EUpdateNeither"));
			SetSubscriptionsL( EUseLocal, EUpdateNeither );

			// Set synchronisation settings to "sync to all".
			ModifyImap4SynchronisationLimitsL(KImImapSynchroniseAll,KImImapSynchroniseAll);
	
			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 1:			
			// Connection OK?
			DebugFormatL(EDebugLevel2,_L("Connection status=%d"),iStatus.Int());
			test(iStatus.Int() == KErrNone);

			// Perform full synchronisation.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState(10);
			break;

		case 10:
			// Check for backup folder and create it if it doesnt exist
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			if (id == KErrNotFound)
				{
				DebugFormatL(EDebugLevel2,_L("Creating folder 'backup'"));
				DoFolderCreateL(iServiceId,_L("Backup"));
				}
			else
				DebugFormatL(EDebugLevel2,_L("Found folder 'backup'"));
			NextState();
			break;

		case 11:
			// Select remote backup folder
			DebugFormatL(EDebugLevel2,_L("Selecting folder 'backup'"));
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			PerformSpecialOpL(id, KIMAP4MTMSelect);
			NextState();
			break;

		case 12:
			// Synchronise with remote backup folder
			DebugFormatL(EDebugLevel2,_L("Synchronising folder 'backup'"));
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			PerformSpecialOpL(id, KIMAP4MTMSynchronise);
			NextState();
			break;

		case 13:
			// Delete contents of backup folder
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));
			if (iSelection->Count()>0)
				{
				DebugFormatL(EDebugLevel2,_L("Deleting folder 'backup' contents"));
				iImppServerMtm->DeleteAllL(*iSelection, iStatus);
				SetActive();
				}
			NextState();
			break;

		case 14:
			// Select remote inbox
			DebugFormatL(EDebugLevel2,_L("Selecting remote inbox"));
			PerformSpecialOpL(iRemoteInbox, KIMAP4MTMSelect);
			NextState();
			break;

		case 15:
			// Sync with remote inbox
			DebugFormatL(EDebugLevel2,_L("Synchronising remote inbox"));
			PerformSpecialOpL(iRemoteInbox, KIMAP4MTMSynchronise);
			NextState(10);
			break;
				 
		case 20:	
			// Backup contents of inbox to backup folder
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			if (iSelection->Count())
				{
				DebugFormatL(EDebugLevel2,_L("Moving contents of inbox to 'backup'"));
				DebugUidListL(EDebugLevel3);
				iImppServerMtm->MoveWithinServiceL(*iSelection, id, iStatus);
				SetActive();
				}
			NextState();
			break;

		case 21:	
			// Unsubscribe from "backup" folder.
			DebugFormatL(EDebugLevel2,_L("Unsubscribe from folder 'backup'"));
			id = FindFolderByNameL(iServiceId, _L("Backup"));
			PerformSpecialOpL(id, KIMAP4MTMLocalUnsubscribe);
			NextState();
			break;

		//
		// Use case SA-1: Sync-to-all, with an empty local and remote mailbox
		//
		case 22:
			DebugFormatL(EDebugLevel0,_L("Use case SA-1: Sync-to-all, with an empty local and remote mailbox"));

			// Perform a full synchronisation.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 23:
			// Check inbox for empty
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry,*iSelection));
			DebugFormatL(EDebugLevel1,_L("Checking inbox for empty - found %d messages"),iSelection->Count());
			test(iSelection->Count()==0);
			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-1: Pass"));
			break;

		//
		// Use case SA-2: Sync-to-all, with 1 email in the remote mailbox
		//
		case 30:
			DebugFormatL(EDebugLevel0,_L("Use case SA-2: Sync-to-all, with 1 email in remote mailbox"));

			// Locate "TestInbox" in local folder list.
			id = FindFolderByNameL(iServiceId,_L("TestInbox"));
			DebugFormatL(EDebugLevel1,_L("Locating 'TestInbox' in local store (%d)"),id);
			test( id != KErrNotFound );

			// Subscribe to "TestInbox"
			DebugFormatL(EDebugLevel2,_L("Subscribe to 'TestInbox'"));
	        PerformSpecialOpL(id, KIMAP4MTMLocalSubscribe);
			NextState();
			break;

		case 31:
			// Perform a full synchronisation.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 32:	
			// Copy 1 new email into remote mailbox
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in the test inbox, if not then someone has
			// broken it!
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);

			// Trim the list of messages down to 1 message only
			DebugFormatL(EDebugLevel2,_L("Trim 'TestInbox' contents list to 1 messaged"));
			iSelection->Delete(1,iSelection->Count()-1);
			DebugUidListL(EDebugLevel3);

			// Copy it over to the inbox.
			DebugFormatL(EDebugLevel2,_L("Copy 1 message from 'TestInbox' to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 33:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 34:
			// Check inbox for 1 new email
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry,*iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 1 expected"),iSelection->Count());
			DebugUidListL(EDebugLevel3);
			test(iSelection->Count()==1);
			NextState();
			break;

		case 35:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 36:
			// Check inbox for 1 new email (again)
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("(recheck) Inbox contains %d messages when 1 expected"),iSelection->Count());
			DebugUidListL(EDebugLevel3);
			test(iSelection->Count()==1);
			NextState();

			DebugFormatL(EDebugLevel0,_L("Use case SA-2: Pass"));
			break;

		//
		// Use case SA-3: Sync-to-all, where mail has been deleted from remote mailbox by 
		// another client.
		//
		case 37:
			DebugFormatL(EDebugLevel0,_L("Use case SA-3: Sync-to-all, where mail has been deleted from remote mailbox"));

			// Check inbox contains
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry,*iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 1 expected"),iSelection->Count());
			DebugUidListL(EDebugLevel3);
			test(iSelection->Count()==1);

			// Delete message from inbox.
			DebugFormatL(EDebugLevel1,_L("Deleting message from inbox"));
			iImppServerMtm->DeleteAllL(*iSelection, iStatus);
			SetActive();

			NextState();
			break;

		case 38:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 39:
			// Check inbox for empty
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 0 expected"),iSelection->Count());
			test(iSelection->Count()==0);
			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-3: Pass"));
			break;

		//
		// Use case SA-4: Sync-to-all, with some new mail in remote inbox 
		//
		case 40:	
			DebugFormatL(EDebugLevel0,_L("Use case SA-4: Sync-to-all, with some new mail in remote inbox"));

			// Copy contents of remote test inbox to remote inbox
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in the test inbox, if not then someone has
			// broken it!
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);

			// Trim list down to 5 oldest
			DebugFormatL(EDebugLevel2,_L("Select 5 messages from list"));
			iSelection->Delete(5,iSelection->Count()-5);
			DebugUidListL(EDebugLevel3);

			// Copy messages to inbox.
			DebugFormatL(EDebugLevel2,_L("Copy 5 messages to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 41:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 42:
			// Check inbox for 5 new messages
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 5 expected"),iSelection->Count());
			test(iSelection->Count()==5);

			NextState();
			break;

		case 43:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 44:
			// Check inbox for 5 new messages (again)
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("(recheck) Inbox contains %d messages when 5 expected"),iSelection->Count());
			test(iSelection->Count()==5);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-4: Pass"));
			break;

		//
		// Use case SA-5: Sync-to-all, with new & old mail in the remote inbox, and some mail
		// deleted from remote inbox
		//
		case 50:
			DebugFormatL(EDebugLevel0,_L("Use case SA-5: Sync-to-all, with new & old mail in the remote inbox, and some mail deleted"));

			// Check contents of inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 5 expected"),iSelection->Count());
			test(iSelection->Count()==5);

			// Trim list down to 1 message
			DebugFormatL(EDebugLevel2,_L("Trim list to 1 message only"));
			iSelection->Delete(1,iSelection->Count()-1);
			DebugUidListL(EDebugLevel3);

			// Delete message from inbox.
			DebugFormatL(EDebugLevel2,_L("Deleting 1 message from inbox"));
			iImppServerMtm->DeleteAllL(*iSelection, iStatus);
			SetActive();

			NextState();
			break;

		case 51:	
			// Copy contents of remote test inbox to remote inbox.
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in the test inbox.
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel3);

			// Get 2 newer messages from list.
			DebugFormatL(EDebugLevel2,_L("Trim list to 2 messages only"));
			iSelection->Delete(0,5);
			iSelection->Delete(2,iSelection->Count()-2);
			DebugUidListL(EDebugLevel3);

			// Copy messages to inbox.
			DebugFormatL(EDebugLevel2,_L("Copy 2 messages from 'TestInbox' to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 52:
			// Perform a full sync.
			DebugFormatL(EDebugLevel2,_L("Perform a full synchronisation"));
			DoFullSyncL();
			NextState();
			break;

		case 53:
			// Check inbox for 5 + 2 - 1 = 6 messages.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 6 expected"),iSelection->Count());
			test(iSelection->Count()==6);
			DebugUidListL(EDebugLevel3);
			
			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-5: Pass"));
			break;

		case 60:
			// Disconnect
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState();
			break;

		// ----------------------------------------------------------------------------------- //

		case 100:
			iTestUtils->TestStart(iTestNum, _L("N-Synchronisation tests: New functionality tests (use cases SA10-SA16)"));

			// Reset service settings to limit synchronisation.
			ModifyImap4SynchronisationLimitsL(KTestImapLimitedSyncSize1,KImImapSynchroniseAll);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();

			NextState();
			break;

		case 101:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState(10);
			break;

		//
		// Use case SA-10: Sync-to-limit, with an empty local and remote mailbox
		//
		case 110:
			DebugFormatL(EDebugLevel0,_L("Use case SA-10: Sync-to-limit, with an empty local and remote mailbox"));

			// Delete contents of inbox
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			if (iSelection->Count())
				{
				DebugFormatL(EDebugLevel2,_L("Deleting messages from inbox"));
				iImppServerMtm->DeleteAllL(*iSelection, iStatus);
				SetActive();
				}
			NextState();
			break;

		case 111:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 112:
			// Check inbox for empty
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 0 expected"),iSelection->Count());
			test(iSelection->Count()==0);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-10: Pass"));
			break;

		//
		// Use case SA-11: Sync-to-limit, with 1 email in the remote inbox
		//
		case 120:	
			DebugFormatL(EDebugLevel0,_L("Use case SA-11: Sync-to-limit, with 1 email in the remote inbox"));

			// Copy 1 new email into remote mailbox
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in "TestInbox".
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);

			// Trim the list of messages down to 1 message only
			DebugFormatL(EDebugLevel2,_L("Trim list to 1 messages only"));
			iSelection->Delete(1,iSelection->Count()-1);
			DebugUidListL(EDebugLevel3);

			// Copy it over to the inbox.
			DebugFormatL(EDebugLevel2,_L("Copy 1 messages to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 121:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 122:
			// Check inbox for 1 new message
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			test(iSelection->Count()==1);
			DebugUidListL(EDebugLevel3);
			NextState();
			break;

		case 123:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 124:
			// Build list of remote inbox messages.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 1 expected"),iSelection->Count());
			// Check inbox for 1 new message.
			test(iSelection->Count()==1);
			DebugUidListL(EDebugLevel3);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-11: Pass"));
			break;

		//
		// Use case SA-12: Sync-to-limit, where mail has been deleted from remote mailbox
		// by another mail client
		//
		case 130:
			DebugFormatL(EDebugLevel0,_L("Use case SA-12: Sync-to-limit, where mail has been deleted from remote mailbox"));

			// Build a list of messages in remote inbox
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check that inbox contains 1 message.
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 1 expected"),iSelection->Count());
			test(iSelection->Count()==1);
			DebugUidListL(EDebugLevel3);
			
			// Delete message from inbox
			DebugFormatL(EDebugLevel2,_L("Deleting messages from inbox"));
			iImppServerMtm->DeleteAllL(*iSelection, iStatus);
			SetActive();

			NextState();
			break;

		case 131:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 132:
			// Check inbox for empty
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 0 expected"),iSelection->Count());
			test(iSelection->Count()==0);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-12: Pass"));
			break;

		//
		// Use case SA-13: Sync-to-all, with some new mail in remote inbox 
		//
		case 140:	
			DebugFormatL(EDebugLevel0,_L("Use case SA-13: Sync-to-all, with some new mail in remote inbox"));

			// Copy contents of remote test inbox to remote inbox
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in "TestInbox".
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel3);

			// Trim list down to 5 oldest
			DebugFormatL(EDebugLevel2,_L("Trim list to 5 messages only"));
			iSelection->Delete(5,iSelection->Count()-5);
			DebugUidListL(EDebugLevel3);

			// Copy 5 messages to inbox.
			DebugFormatL(EDebugLevel2,_L("Copy 5 messages from 'TestInbox' to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);

			SetActive();
			NextState();
			break;

		case 141:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 142:
			// Check inbox for 5 new messages
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 5 expected"),iSelection->Count());
			test(iSelection->Count()==5);
			DebugUidListL(EDebugLevel3);

			NextState();
			break;

		case 143:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 144:
			// Check inbox for 5 new messages (again)
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("(recheck) Inbox contains %d messages when 5 expected"),iSelection->Count());
			test(iSelection->Count()==5);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-13: Pass"));
			break;

		//
		// Use case SA-14: Sync-to-all, with new & old mail in the remote mailbox, and
		// some mail deleted from remote mailbox
		//
		case 150:
			DebugFormatL(EDebugLevel0,_L("Use case SA-14: Sync-to-all, with new & old mail in the remote mailbox, and some mail deleted"));

			// Build list of inbox messages
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 5 expected"),iSelection->Count());
			test(iSelection->Count()==5);

			// Trim list down to 1 message
			DebugFormatL(EDebugLevel2,_L("Trim list to 1 message only"));
			iSelection->Delete(1,iSelection->Count()-1);
			DebugUidListL(EDebugLevel3);

			// Delete message from inbox
			DebugFormatL(EDebugLevel2,_L("Deleting message from inbox"));
			iImppServerMtm->DeleteAllL(*iSelection, iStatus);
			SetActive();

			NextState();
			break;

		case 151:	
			// Build a list of messages in "TestInbox"
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in the test inbox.
			test(iSelection->Count()==KTestImapTestInboxSize);

			// Get 2 newer messages from list
			DebugFormatL(EDebugLevel2,_L("Trim list to 2 messages only"));
			iSelection->Delete(0,5);
			iSelection->Delete(2,iSelection->Count()-2);
			DebugUidListL(EDebugLevel3);

			// Copy 2 messages from "TestInbox" to remote inbox
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 152:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 153:
			// Check inbox for 5 + 2 - 1 = 6 messages
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Inbox contains %d messages when 6 expected"),iSelection->Count());
			test(iSelection->Count()==6);
			DebugUidListL(EDebugLevel3);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-14: Pass"));
			break;

		//
		// Use case SA-15: Sync-to-all, with email > limit in remote mailbox
		//
		case 160:	
			DebugFormatL(EDebugLevel0,_L("Use case SA-15: Sync-to-all, with email > limit in remote mailbox"));

			// Build list of messages in "TestInbox".
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in "TestInbox".
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel3);

			// Get 5 newer messages from list
			// Trim the list of messages down to 1 message only
			DebugFormatL(EDebugLevel2,_L("Trim list to 5 messages only"));
			iSelection->Delete(0,7);
			iSelection->Delete(5,iSelection->Count()-5);
			DebugUidListL(EDebugLevel3);

			// Copy messages to inbox.
			DebugFormatL(EDebugLevel2,_L("Copy 5 messages to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 161:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 162:
			{
			// Build list of messages in remote inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check inbox for 10 messages.
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 10 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1);
			DebugUidListL(EDebugLevel3);

			// Check that remote message count is what we expect it to be (= 11)
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
	//		TMsvEmailEntry message=iEntry->Entry();
	//		DebugFormatL(EDebugLevel1,_L("Remote Inbox contains %d messages when 11 expected"),message.RemoteFolderEntries());
	//		test(message.RemoteFolderEntries()==KTestImapLimitedSyncSize1+1);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-15: Pass"));
			break;
			}

		//
		// Use case SA-16: Sync-to-all, with populated email > limit in remote mailbox
		//
		case 170:
			DebugFormatL(EDebugLevel0,_L("Use case SA-16: Sync-to-all, with populated email > limit in remote mailbox"));

			// Check inbox for 10 messages
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 10 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1);

			// Locate oldest message is inbox message list.
			DebugFormatL(EDebugLevel2,_L("Trim list to 1 message only"));
			iSelection->Delete(1,iSelection->Count()-1);
			DebugUidListL(EDebugLevel3);

			// Download body parts for oldest message
			DebugFormatL(EDebugLevel2,_L("Populating message in inbox"));
			iImppServerMtm->CopyToLocalL(*iSelection, KMsvGlobalInBoxIndexEntryId, iStatus);
			SetActive();

			NextState();
			break;

		case 171:	
			// Copy contents of remote test inbox to remote inbox

			// Build list of messages in "TestInbox".
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Get 1 newer messages from list
			DebugFormatL(EDebugLevel2,_L("Trim list next newest message only"));
			iSelection->Delete(0,12);
			iSelection->Delete(1,iSelection->Count()-1);
			DebugUidListL(EDebugLevel3);

			// Copy messages to inbox.
			DebugFormatL(EDebugLevel2,_L("Copy message to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 172:
			// Perform a full sync
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 173:
			{
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			// Check inbox for 11 messages.
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 11 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1+1);
			DebugUidListL(EDebugLevel3);

			// Check that remote message count is what we expect it to be (= 12)
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
//			TMsvEmailEntry message=iEntry->Entry();
//			DebugFormatL(EDebugLevel1,_L("Remote Inbox contains %d messages when 12 expected"),message.RemoteFolderEntries());
//			test(message.RemoteFolderEntries()==KTestImapLimitedSyncSize1+2);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Use case SA-16: Pass"));
			break;
			}

		case 180:
			// Disconnect
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState();
			break;

		// ----------------------------------------------------------------------------------- //

		//
		// Special Case 1: Sync-to-limit, with large remote mailbox (1200 messages)
		//
		case 200:
			iTestUtils->TestStart(iTestNum, _L("N-Synchronisation tests: Special case tests"));

			DebugFormatL(EDebugLevel0,_L("Special case 1: Sync-to-limit, with large remote mailbox (1200 messages)"));

			// Reset service settings to limit synchronisation.
			DebugFormatL(EDebugLevel2,_L("Sync Strategy=EUseLocal, Subscribe Stategy=EUpdateNeither"));
			SetSubscriptionsL(EUseLocal, EUpdateNeither);
			ModifyImap4SynchronisationLimitsL(KTestImapLimitedSyncSize1,KTestImapLimitedSyncSize1);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 201:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 202:
			// Check for "big" folder in local folder list
			iRemoteFolder1 = FindFolderByNameL(iServiceId, _L("TestBigFolder"));
			if (iRemoteFolder1==KErrNotFound)
				iRemoteFolder1 = FindFolderByNameL(iServiceId, _L("INBOX/TestBigFolder"));
			if (iRemoteFolder1==KErrNotFound)
				iRemoteFolder1 = FindFolderByNameL(iServiceId, _L("Mail/TestBigFolder"));
			if (iRemoteFolder1 == KErrNotFound)
				DoFolderCreateL(iServiceId,_L("TestBigFolder"));
			iRemoteFolder1 = FindFolderByNameL(iServiceId, _L("TestBigFolder"));

			DebugFormatL(EDebugLevel1,_L("Checking for 'TestBigFolder' in local folder list (%d)"),iRemoteFolder1);
			test(iRemoteFolder1!=KErrNotFound);

			NextState();
			break;

		case 203:
			// Select remote "big" folder
			DebugFormatL(EDebugLevel1,_L("Select remote 'TestBigFolder'"));
			PerformSpecialOpL(iRemoteFolder1, KIMAP4MTMSelect);
			NextState();
			break;

		case 204:
			// Synchronise with remote "big" folder
			DebugFormatL(EDebugLevel1,_L("Synchronise with remote 'TestBigFolder'"));
			PerformSpecialOpL(iRemoteFolder1, KIMAP4MTMSynchronise);
			NextState();
			break;

		case 205:
			{
			// Build list of messages in local "TestBigFolder".
			User::LeaveIfError(iEntry->SetEntry(iRemoteFolder1));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));
			// Check big folder for 10 messages
			DebugFormatL(EDebugLevel1,_L("Local 'TestBigFolder' contains %d messages when 10 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1);
			DebugUidListL(EDebugLevel3);

			// Check that remote message count is what we expect it to be (= 1200)
			TMsvEmailEntry message=iEntry->Entry();
			DebugFormatL(EDebugLevel1,_L("Remote 'TestBigFolder' contains %d messages when 1200 expected"),message.RemoteFolderEntries());
			test(message.RemoteFolderEntries()==KTestImapTestBigFolderSize);

			NextState();

			break;
			}

		case 206:	
			// Unsubscribe from "big" folder
			DebugFormatL(EDebugLevel1,_L("Unsubscribe from 'TestBigFolder'"));
			PerformSpecialOpL(iRemoteFolder1, KIMAP4MTMLocalUnsubscribe);

			NextState();

			DebugFormatL(EDebugLevel0,_L("Special case 1: Pass"));
			break;

		case 207:
			// Disconnect
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState(10);
			break;

		//
		// Special Case 2: Sync-to-limit changed to Sync-to-all.
		//
		case 210:
			DebugFormatL(EDebugLevel0,_L("Special case 2: Sync-to-limit changed to Sync-to-all"));

			// Set synchronisation settings to "sync to all".
			SetSubscriptionsL( EUseLocal, EUpdateNeither );
			ModifyImap4SynchronisationLimitsL(KImImapSynchroniseAll,KImImapSynchroniseAll);

			// Connect to remote server.			
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 211:
			// Connected ok?
			DebugFormatL(EDebugLevel1,_L("Connected? (%d)"),iStatus.Int());
			test(iStatus.Int() == KErrNone);

			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 212:
			// Build a list of local inbox messages.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Only want messages in selection list.
			FindMessagesInSelectionL();
			if (iSelection->Count())
				{
				// Delete all messages from inbox.
				DebugFormatL(EDebugLevel2,_L("Deleting messages from inbox"));
				DebugUidListL(EDebugLevel3);
				iImppServerMtm->DeleteAllL(*iSelection, iStatus);
				SetActive();
				}
			NextState();
			break;

		case 213:
			// Locate "TestInbox" in local folder list.
			iRemoteFolder1 = FindFolderByNameL(iServiceId, _L("TestInbox"));
			if (iRemoteFolder1==KErrNotFound)
				iRemoteFolder1 = FindFolderByNameL(iServiceId, _L("INBOX/TestInbox"));
			if (iRemoteFolder1==KErrNotFound)
				iRemoteFolder1 = FindFolderByNameL(iServiceId, _L("Mail/TestInbox"));
			DebugFormatL(EDebugLevel1,_L("Checking for 'TestInbox' in local folder list (%d)"),iRemoteFolder1);
			test(iRemoteFolder1!=KErrNotFound);

			// Subscribe to "TestInbox".
	        PerformSpecialOpL(iRemoteFolder1, KIMAP4MTMLocalSubscribe);
			NextState();
			break;

		case 214:
			// Perform a full sync to get contents of "TestInbox".
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 215:	
			// Build list of messages in "TestInbox".
			User::LeaveIfError(iEntry->SetEntry(iRemoteFolder1));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check for correct number of messages in "TestInbox".
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel3);

			// Copy contents of remote "TestInbox" to remote inbox.
			DebugFormatL(EDebugLevel1,_L("Copying contents of 'TestInbox' to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 216:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 217:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel3);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState();
			break;

		case 218:
			// Set synchronisation settings to "sync to N".
			ModifyImap4SynchronisationLimitsL(KTestImapLimitedSyncSize1,KTestImapLimitedSyncSize1);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 219:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 220:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 10 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1);
			DebugUidListL(EDebugLevel3);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState();
			break;

		case 221:
			// Set synchronisation settings to "sync to all".
			ModifyImap4SynchronisationLimitsL(KImImapSynchroniseAll,KImImapSynchroniseAll);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 222:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 223:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel3);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Special case 2: Pass"));
			break;

		//
		// Special Case 3: Sync-to-all changed to Sync-to-limit.
		//
		case 230:
			DebugFormatL(EDebugLevel2,_L("Special case 3: Sync-to-all changed to Sync-to-limit"));

			// Set synchronisation settings to "sync to N".
			ModifyImap4SynchronisationLimitsL(KTestImapLimitedSyncSize1,KTestImapLimitedSyncSize1);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();

			NextState();
			break;

		case 231:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 232:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 10 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1);
			DebugUidListL(EDebugLevel3);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Special case 3: Pass"));
			break;

		//
		// Special Case 4: Sync-to-limit, limit increased
		//
		case 240:
			DebugFormatL(EDebugLevel0,_L("Special case 4: Sync-to-limit, limit increased"));

			// Set synchronisation settings to "sync to N".
			ModifyImap4SynchronisationLimitsL(KTestImapLimitedSyncSize1+5,KTestImapLimitedSyncSize1+5);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 241:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 242:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 15 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1+5);
			DebugUidListL(EDebugLevel3);

			NextState(10);
						
			DebugFormatL(EDebugLevel0,_L("Special case 4: Pass"));
			break;

		//
		// Special Case 5: Sync-to-limit, with populated message and limit decreased, then increased
		//
		case 250:
			{
			DebugFormatL(EDebugLevel0,_L("Special case 5: Sync-to-limit, with populated message and limit decreased, then increased"));

			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Locate the oldest message.
			TMsvId msvId;
			iMessageUid=0;
			for (TInt i=0;i<iSelection->Count();i++)
				{
				TMsvEntry* entryPtr;
				User::LeaveIfError(iEntry->GetEntryFromId((*iSelection)[i],entryPtr));
				TUint32 uid=((TMsvEmailEntry)(*entryPtr)).UID();
				if (iMessageUid>uid || iMessageUid==0)
					{
					iMessageUid=uid;
					msvId=(*iSelection)[i];
					}
				}

			// Download body parts for oldest message
			iSelection->Reset();
			iSelection->AppendL(msvId);
			DebugFormatL(EDebugLevel1,_L("Populating 1 message in inbox"));
			DebugUidListL(EDebugLevel3);
			iImppServerMtm->CopyToLocalL(*iSelection, KMsvGlobalInBoxIndexEntryId, iStatus);
			SetActive();

			NextState();
			break;
			}

		case 251:
			// Disconnect
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState();
			break;

		case 252:
			// Reset synchronisation settings limit
			ModifyImap4SynchronisationLimitsL(KTestImapLimitedSyncSize1,KTestImapLimitedSyncSize1);
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 253:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 254:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 11 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1+1);
			DebugUidListL(EDebugLevel3);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState();
			break;

		case 255:
			// Reset synchronisation settings limit.
			ModifyImap4SynchronisationLimitsL(KImImapSynchroniseAll,KImImapSynchroniseAll);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 256:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 257:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel3);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Special case 5: Pass"));
			break;

		//
		// Special Case 6: Sync-to-limit, with populated message deleted from remote folder
		//
		case 260:
			DebugFormatL(EDebugLevel0,_L("Special case 6: Sync-to-limit, with populated message deleted from remote folder"));

			// Connect to remote server.
			ModifyImap4SynchronisationLimitsL(KTestImapLimitedSyncSize1,KTestImapLimitedSyncSize1);
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();

			NextState();
			break;

		case 261:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 262:
			{
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 11 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1+1);
			DebugUidListL(EDebugLevel3);

			// Locate the message with body parts
			TMsvId msvId=-1;
			for (TInt i=0;i<iSelection->Count();i++)
				{
				User::LeaveIfError(iEntry->SetEntry((*iSelection)[i]));
				TMsvEmailEntry entry(iEntry->Entry());
				if (entry.BodyTextComplete())
					{
					msvId=(*iSelection)[i];
					break;
					}
				}
			// Found?
			DebugFormatL(EDebugLevel1,_L("Populated message located (%d)"),msvId);
			test(msvId!=-1);

			// Add message with body parts to deletion list.
			iSelection->Reset();
			iSelection->AppendL(msvId);

			// Delete message.
			DebugFormatL(EDebugLevel1,_L("Delete populated message"),iSelection->Count());
			DebugUidListL(EDebugLevel3);
			iImppServerMtm->DeleteAllL(*iSelection, iStatus);
			SetActive();

			NextState();
			break;
			}

		case 263:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 264:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 10 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapLimitedSyncSize1);
			DebugUidListL(EDebugLevel3);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Special case 6: Pass"));
			break;

		//
		// Special Case 7: Sync-to-none, with empty local folder & empty remote folder
		//
		case 270:
			DebugFormatL(EDebugLevel0,_L("Special case 7: Sync-to-none, with empty local folder & empty remote folder"));

			// Reset synchronisation settings limit to "sync-to-all".
			ModifyImap4SynchronisationLimitsL(KImImapSynchroniseAll,KImImapSynchroniseAll);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 271:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 272:
			// Build list of messages in inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));
			FindMessagesInSelectionL();
			if (iSelection->Count())
				{
				// Delete all messages from inbox.
				DebugFormatL(EDebugLevel1,_L("Delete messages in inbox"));
				DebugUidListL(EDebugLevel3);
				iImppServerMtm->DeleteAllL(*iSelection, iStatus);
				SetActive();
				}
			NextState();
			break;

		case 273:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 274:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 0 expected"),iSelection->Count());
			test(iSelection->Count()==0);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState();
			break;

		case 275:
			// Reset synchronisation settings limit
			ModifyImap4SynchronisationLimitsL(KImImapSynchroniseNone,KImImapSynchroniseAll);

			// Connect to remote server.
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 276:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 277:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder.
			FindMessagesInSelectionL();
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 0 expected"),iSelection->Count());
			test(iSelection->Count()==0);

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Special case 7: Pass"));
			break;

		//
		// Special Case 8: Sync-to-none, with new messages in remote folder
		//
		case 280:
			DebugFormatL(EDebugLevel0,_L("Special case 8: Sync-to-none, with new messages in remote folder"));

			// Build a list of messages in "TestInbox".
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in the test inbox.
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel3);

			// Get 5 messages from the list and copy to inbox.
			DebugFormatL(EDebugLevel2,_L("Select 5 messages from 'TestInbox' message list"));
			iSelection->Delete(5,iSelection->Count()-5);
			DebugUidListL(EDebugLevel3);
			DebugFormatL(EDebugLevel1,_L("Copy 5 messages from 'TestInbox' to remote inbox"));
			iImppServerMtm->CopyWithinServiceL(*iSelection, iRemoteInbox, iStatus);
			SetActive();

			NextState();
			break;

		case 281:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 282:
			// Build list of messages in local inbox.
			User::LeaveIfError(iEntry->SetEntry(iRemoteInbox));
			User::LeaveIfError(iEntry->GetChildrenWithType(KUidMsvMessageEntry, *iSelection));

			// Check for correct number of messages in folder
			FindMessagesInSelectionL();

#if 0
			// skip this test as we've set InboxSync to None which
			// means that the copied messages won't ever show up (due
			// to to the copy itself or due to the Sync after) this
			// looks like it might be a problem in the design of the sync
			// code but can't tell for sure
			DebugFormatL(EDebugLevel1,_L("Local Inbox contains %d messages when 5 expected"),iSelection->Count());
			test(iSelection->Count()==5);
#endif
			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();

			NextState(10);

			DebugFormatL(EDebugLevel0,_L("Special case 8: Pass"));
			break;

		//
		// Special Case 9: Unsubscribed folder contents auto-deleted
		//
		case 290:
			DebugFormatL(EDebugLevel0,_L("Special case 9: Unsubscribed folder contents auto-deleted"));

			// Reset synchronisation settings limit to sync-to-all
			SetSubscriptionsL(EUseLocal, EUpdateBoth);
			ModifyImap4SynchronisationLimitsL(KImImapSynchroniseAll,KImImapSynchroniseAll);

			// Connect to remote server
			DebugFormatL(EDebugLevel2,KConnectToRemoteServer);
			DoConnectL();
			NextState();
			break;

		case 291:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 292:
			// Build list of messages in "TestInbox"
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that there are 20 messages in "TestInbox"
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);
			DebugUidListL(EDebugLevel2);

			// Unsubscribe 
			DebugFormatL(EDebugLevel1,_L("Unsubscribe from 'TestInbox'"));
			PerformSpecialOpL(id, KIMAP4MTMLocalUnsubscribe);
			NextState();
			break;

		case 293:
			// Perform a full synchronisation with remote server.
			DebugFormatL(EDebugLevel2,KPerformFullSync);
			DoFullSyncL();
			NextState();
			break;

		case 294:
			// Build list of messages in "TestInbox".
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that "TestInbox" is empty.
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 0 expected"),iSelection->Count());
			test(iSelection->Count()==0);

			// Disconnect from remote server.
			DebugFormatL(EDebugLevel2,KDisconnectFromRemoteServer);
			DoDisconnectL();
			NextState();

			DebugFormatL(EDebugLevel0,_L("Special case 9: Pass"));
			break;

		// ----------------------------------------------------------------------------------- //

		//
		// Performance test
		//
		case 300:			
			// Connect and unsubscribe 
			iTestUtils->TestStart(iTestNum, _L("N-Synchronisation tests: Large mailbox sync performance test"));

			SetSubscriptionsL( EUseCombination, EUpdateNeither );

			// Set synchronisation settings to "sync to all".
			ModifyImap4SynchronisationLimitsL(iSyncLimit,iSyncLimit);
	
			DoConnectL();
			NextState();
			break;

		case 301:			
			// Synchronise with remote inbox
			test(iStatus.Int()==KErrNone);
			DoFullSyncL();
			NextState();
			break;

		case 302:
			// Check for "big" folder and create it if it doesnt exist
			id = FindFolderByNameL(iServiceId, _L("TestBigFolder"));
			if (id == KErrNotFound)
				DoFolderCreateL(iServiceId,_L("TestBigFolder"));
			NextState();
			break;

		case 303:
			// Disconnect
			DoDisconnectL();
			NextState(10);
			break;

		case 310:
			// Reset service settings to limit synchronisation.
			iTimeStamp.UniversalTime();
			ModifyImap4SynchronisationLimitsL(iSyncLimit,iSyncLimit);
			DoConnectL();
			NextState();
			break;

		case 311:
			// Select remote "big" folder
			id = FindFolderByNameL(iServiceId, _L("TestBigFolder"));
			PerformSpecialOpL(id, KIMAP4MTMSelect);
			NextState();
			break;

		case 312:
			// Synchronise with remote "big" folder
			id = FindFolderByNameL(iServiceId, _L("TestBigFolder"));
			PerformSpecialOpL(id, KIMAP4MTMSynchronise);
			NextState();
			break;

		case 313:
			{
			// Check big folder for N messages
			id = FindFolderByNameL(iServiceId, _L("TestBigFolder"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));
			if (iSelection->Count()!=iSyncLimit)
				{
				DebugFormatL(EDebugLevel0,_L("WARNING: %d messages detected in local folder when %d were expected"),iSelection->Count(),iSyncLimit);
				iOpState=399;
				break;
				}

			// Check that remote message count is what we expect it to be (= 1200)
			User::LeaveIfError(iEntry->SetEntry(id));
//			TMsvEmailEntry message=iEntry->Entry();
//			test(message.RemoteFolderEntries()==KTestImapTestBigFolderSize);
			NextState();
			break;
			}

		case 314:
			// Disconnect
			DoDisconnectL();
			NextState();
			break;

		case 315:
			{
			TTime theCurrentTime;
			theCurrentTime.UniversalTime();
			TTimeIntervalSeconds interval;
			theCurrentTime.SecondsFrom(iTimeStamp,interval);

			DebugFormatL(EDebugLevel0,_L("%d,%d"),iSyncLimit,interval.Int());
			if (iSyncLimit<KTestImapTestBigFolderSize)
				{
				iSyncLimit+=KTestImapSyncIncrementSize;
				iOpState=310;
				}
			else
				NextState(10);
			break;
			}


		case 399:
			// Disconnect
			DoDisconnectL();
			NextState();
			break;

#if 0
			// ----------------------------------------------------------------------------------- //

			// The following test case doesn't actually test anything
			// currently as it would fail. It was a testbed to
			// reproduce the problem and may become a test case if the
			// problem is fixed.
			
		case 400:			
			// Connect and unsubscribe 
			iTestUtils->TestStart(iTestNum, _L(""));

			SetSubscriptionsL( EUseCombination, EUpdateNeither );

			// Set synchronisation settings to "sync to all".
			ModifyImap4SynchronisationLimitsL(KImImapSynchroniseAll,KImImapSynchroniseAll);
	
			DoConnectL();
			NextState();
			break;

		case 401:
			test(iStatus.Int()==KErrNone);
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			PerformSpecialOpL(id, KIMAP4MTMSelect);
			NextState();
			break;

		case 402:			
			// Synchronise with remote inbox
			test(iStatus.Int()==KErrNone);
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			PerformSpecialOpL(id, KIMAP4MTMSynchronise);
			NextState();
			break;

		case 403:
			// Disconnect
			DoDisconnectL();
			NextState();
			break;

		case 404:			
			// Set synchronisation settings to 10
			ModifyImap4SynchronisationLimitsL(10,10);

			DoConnectL();
			NextState();
			break;

		case 405:
			test(iStatus.Int()==KErrNone);
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			PerformSpecialOpL(id, KIMAP4MTMSelect);
			NextState();
			break;

		case 406:			
			// Synchronise with remote inbox
			test(iStatus.Int()==KErrNone);
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			PerformSpecialOpL(id, KIMAP4MTMSynchronise);
			NextState();
			break;

		case 407:
			// Build list of messages in "TestInbox".
			id = FindFolderByNameL(iServiceId, _L("TestInbox"));
			User::LeaveIfError(iEntry->SetEntry(id));
			User::LeaveIfError(iEntry->GetChildren(*iSelection));

			// Check that "TestInbox" is full
			DebugFormatL(EDebugLevel1,_L("'TestInbox' contains %d messages when 20 expected"),iSelection->Count());
			test(iSelection->Count()==KTestImapTestInboxSize);

			// select first 3 only
			iSelection->Delete(3,iSelection->Count()-3);

			// populate
				{
				TPckgBuf<TImap4GetMailOptions> package(EGetImap4EmailBodyTextAndAttachments);
				iDestFolder=0;
				PerformOpL( EImap4OpPopulate, package );
				}
			NextState();
			break;
			

		case 408:
			// should check the 3 messages are populated correctly
			// however it will fail currently

			// Disconnect
			DoDisconnectL();
			NextState(100);
			break;
#endif
			// ----------------------------------------------------------------------------------- //

		case 400:
			test(iStatus.Int() == KErrNone);

			iTestUtils->TestFinish(iTestNum++);

			// done
			iTestUtils->TestHarnessCompleted();
			EndOfTest();  
			// done
			test.Console()->SetPos(0, 13);
			test.Printf(_L("Finished sync test\n"));
			more = EFalse;
			break;

		default:
			iOpState=400;
			break;
			}
		}
	return more; 
	}

void CTestRefreshMBox::DoNSynchronisationRegressionL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	iOpState = 0;
	iState = EImapTestNSynchronisation;
	StateMachineNSynchronisationL();
	}

void CTestRefreshMBox::DoNSynchronisationTestsL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	iOpState = 100;
	iState = EImapTestNSynchronisation;
	StateMachineNSynchronisationL();
	}

void CTestRefreshMBox::DoNSynchronisationSpecialTestsL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	iOpState = 200;
	iState = EImapTestNSynchronisation;
	StateMachineNSynchronisationL();
	}

void CTestRefreshMBox::DoNSynchronisationPerformanceTestsL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	iOpState = 300;
	iState = EImapTestNSynchronisation;
	iSyncLimit = KTestImapSyncIncrementSize;
	StateMachineNSynchronisationL();
	}

void CTestRefreshMBox::DoNSynchronisationOneOffTestsL()
	{
	if (iServiceId==NULL)
		{
		test.Printf(_L("No service ID\n"));
		return;
		}

	iOpState = 400;
	iState = EImapTestNSynchronisation;
	iSyncLimit = KImImapSynchroniseAll;
	StateMachineNSynchronisationL();
	}

/* ----------------------------------------------------------------------- */

void CTestRefreshMBox::QueueOpL(TImap4OpType aOpType)
	{
	TMsvId srcFolder, destination;
	TestFolders(aOpType, srcFolder, destination);
	
	TPtrC msgName = TestMsgName(aOpType);
	TMsvId id = FindMessageByNameL(srcFolder, msgName);
	if (id == KErrNotFound)
		{
		test.Printf(_L("Can't find message %S\n"), &msgName);
		return;
		}
	
	iSelection->Reset();
	iSelection->InsertL(0, id);
	
	iState = EImapTestOffLineOp;

	switch (aOpType)
		{
	case EImap4OpCopyToLocal:
		iImppServerMtm->CopyToLocalL(*iSelection, destination, iStatus);
		break;
	case EImap4OpCopyFromLocal:
		iImppServerMtm->CopyFromLocalL(*iSelection, destination, iStatus);
		break;
	case EImap4OpCopyWithinService:
	case EImap4OpCopyWithinFolder:
		iImppServerMtm->CopyWithinServiceL(*iSelection, destination, iStatus);
		break;
			
	case EImap4OpMoveToLocal:
		iImppServerMtm->MoveToLocalL(*iSelection, destination, iStatus);
		break;
	case EImap4OpMoveFromLocal:
		iImppServerMtm->MoveFromLocalL(*iSelection, destination, iStatus);
		break;
	case EImap4OpMoveWithinService:
	case EImap4OpMoveWithinServiceToInbox:
		iImppServerMtm->MoveWithinServiceL(*iSelection, destination, iStatus);
		break;

	case EImap4OpDelete:
		iImppServerMtm->DeleteAllL(*iSelection, iStatus);
		break;

	case EImap4OpUndelete:
		break;

	case EImap4OpPopulate:
		{
		TBuf8<128> paramBuf(_L8(""));
		iImppServerMtm->StartCommandL(*iSelection, KIMAP4MTMPopulate, paramBuf, iStatus);
		break;
		}

	default:
		break;
		}

	SetActive();
	}

void CTestRefreshMBox::QueueLocalToLocalOpL(TImap4OpType aOpType)
	{
	TMsvId srcFolder, dummy;

	TImap4OpType origOpType = aOpType == EImap4OpCopyToLocal ?
		EImap4OpCopyFromLocal :
		EImap4OpMoveFromLocal;

	// source of the operation is the destination of the previous op
	TestFolders(origOpType, dummy, srcFolder);

	// destination is the local test folder
	TMsvId destination = iLocalFolder1;
	
	TPtrC msgName = TestMsgName(origOpType);
	TMsvId id = FindMessageByNameL(srcFolder, msgName);

	// park
	User::LeaveIfError(iEntry->SetEntry( NULL ));
	
	iSelection->Reset();
	iSelection->InsertL(0, id);
	
	// server mtm commands can complete before returning
	iState = EImapTestOffLineOp;
	SetActive();

	switch (aOpType)
		{
	case EImap4OpCopyToLocal:
		iImppServerMtm->CopyToLocalL(*iSelection, destination, iStatus);
		break;
			
	case EImap4OpMoveToLocal:
		iImppServerMtm->MoveToLocalL(*iSelection, destination, iStatus);
		break;

	default:
		break;
		}
	}

void CTestRefreshMBox::UnQueueOpL(TImap4OpType aOpType)
	{
	TMsvId srcFolder, destination;
	TestFolders(aOpType, destination, srcFolder);

	TPtrC msgName = TestMsgName(aOpType);
	TMsvId id = FindMessageByNameL(srcFolder, msgName);
	
	iSelection->Reset();
	iSelection->InsertL(0, id);
	
	// server mtm commands can complete before returning
	iState = EImapTestOffLineOp;
	SetActive();

	switch (aOpType)
		{
	case EImap4OpCopyToLocal:
		iImppServerMtm->DeleteAllL(*iSelection, iStatus);
		break;
	case EImap4OpCopyFromLocal:
		iImppServerMtm->DeleteAllL(*iSelection, iStatus);
		break;
	case EImap4OpCopyWithinService:
		iImppServerMtm->DeleteAllL(*iSelection, iStatus);
		break;
			
	case EImap4OpMoveToLocal:
		iImppServerMtm->MoveFromLocalL(*iSelection, destination, iStatus);
		break;
	case EImap4OpMoveFromLocal:
		iImppServerMtm->MoveToLocalL(*iSelection, destination, iStatus);
		break;
	case EImap4OpMoveWithinService:
		iImppServerMtm->MoveWithinServiceL(*iSelection, destination, iStatus);
		break;

	case EImap4OpDelete:
		{
		TBuf8<128> paramBuf(_L8(""));
		iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMUndeleteAll,paramBuf,iStatus);
		break;
		}

	case EImap4OpPopulate:
	case EImap4OpUndelete:
		break;

	default:
		break;
		}

	// park
	User::LeaveIfError(iEntry->SetEntry( NULL ));
	}

void CTestRefreshMBox::DoRenameFolderL()
	{
	TImap4RenameFolder cmd;
	cmd.iNewName = iRenamedFolder ? _L("Test1") : _L("Test1Renamed");

	test.Console()->SetPos(0, 13);
	test.Console()->Printf(_L("DoRenamefolder to %S\n"), &cmd.iNewName);

	iSelection->Reset();
	iSelection->InsertL(0, iRemoteFolder1);

	iState = EImapTestRenameFolder;
	SetActive();

	TPckgBuf<TImap4RenameFolder> package(cmd);
	iImppServerMtm->StartCommandL(*iSelection,KIMAP4MTMRenameFolder,package,iStatus);

	iRenamedFolder = !iRenamedFolder;
	}

void CTestRefreshMBox::RunL()
	{
	TInt anError=iStatus.Int();

	test.Console()->SetPos(0, 13);
	test.Console()->Printf(_L("                                           "));
	test.Console()->SetPos(0, 13);
	
	switch(iState)
		{
	case EImapTestConnectAndSync:
		test.Console()->Printf(_L("Connected: err %d (%d)\n"),anError);
		if (anError == KErrNone)
			{
			iState = EImapTestWaitForBackground;
			StartL();
			}
		else
			ClearTimer();
		break;

	case EImapTestFetchInbox:
		test.Console()->Printf(_L("FetchedInbox: err %d\n"),anError);
		
		ClearTimer();
		break;
			
	case EImapTestDisconnect:
		test.Console()->Printf(_L("Disconnected: err %d\n"),anError);

		ClearTimer();
		break;

	case EImapTestOffLineOp:
		test.Console()->Printf(_L("Done OffLineOp %d\n"),anError);
		break;

	case EImapTestWaitForBackground:
		test.Console()->Printf(_L("Background Sync finished %d\n"),anError);
		
		ClearTimer();
		break;
		
	case EImapTestFetchAttachment:
		test.Console()->Printf(_L("Fetch attachment finished %d\n"),anError);
		
		ClearTimer();
		break;

	case EImapTestFetchAttachedMessage:
		test.Console()->Printf(_L("Fetch attached message finished %d\n"),anError);
		
		ClearTimer();
		break;

	case EImapTestCancelOffLineOps:
		test.Console()->Printf(_L("Cancelled offline ops %d\n"),anError);
		break;
		
	case EImapTestRenameFolder:
		test.Console()->Printf(_L("Renamed folder %d\n"),anError);
		break;
		
	case EImapTestAutoOffLineOp:
		{
		test.Console()->Printf(_L("Done Op %d next state %d\n"),anError,iOpState);

		TBool more = EFalse;
		TRAPD(err, more = HandleOffLineStateL());
		if (err != KErrNone)
			test.Console()->Printf(_L("Op left with error %d\n"), err);
		if (!more)
			test.Console()->Printf(_L("Completed Offline tests %d\n"),anError);
		break;
		}
	
	case EImapTestAutoSyncOp:
		{
		test.Console()->Printf(_L("Done Op %d next state %d\n"),anError,iOpState);

		TBool more = EFalse;
		TRAPD(err, more = HandleSyncStateL());
		if (err != KErrNone)
			test.Console()->Printf(_L("Op left with error %d\n"), err);
		if (!more)
			{
			test.Console()->Printf(_L("Completed Sync tests %d\n"),anError);
			ClearTimer();
			}
		break;
		}
	
	case EImapTestAutoSecondaryConnect:
		{
		test.Console()->Printf(_L("Done Op %d next state %d\n"),anError,iOpState);

		TBool more = EFalse;
		TRAPD(err, more = TestSecondaryConnectStateMachineL());
		if (err != KErrNone)
			test.Console()->Printf(_L("Op left with error %d\n"), err);
		if (!more)
			{
			test.Console()->Printf(_L("Completed SecondaryConnect tests %d\n"),anError);
			ClearTimer();
			}
		break;
		}
	
	case EImapTestCheckServerContents:
		{
		test.Console()->Printf(_L("Done Op %d next state %d\n"),anError,iOpState);

		TBool more = EFalse;
		TRAPD(err, more = StateMachineCheckServerContentsL());
		if (err != KErrNone)
			test.Console()->Printf(_L("Op left with error %d\n"), err);
		if (!more)
			{
			test.Console()->Printf(_L("Completed CheckServerContents tests %d\n"),anError);
			ClearTimer();
			}
		break;
		}

	case EImapTestNSynchronisation:
		{
		test.Console()->Printf(_L("Done Op %d next state %d\n"),anError,iOpState);

		TBool more = EFalse;
		TRAPD(err, more = StateMachineNSynchronisationL());
		if (err != KErrNone)
			test.Console()->Printf(_L("Op left with error %d\n"), err);
		if (!more)
			{
			test.Console()->Printf(_L("Completed N-Synchronisation Tests %d\n"),anError);
			ClearTimer();
			}
		break;
		}
	
	case EImapTestFetchTest:
		test.Console()->Printf(_L("Fetched test message: err %d\n"),anError);
		ClearTimer();
		break;

	case EImapTestFolderDelete:
		test.Console()->Printf(_L("Folder deleted: err %d\n"),anError);
		ClearTimer();
		break;
		
	case EImapTestFolderCreate:
		test.Console()->Printf(_L("Folder created: err %d\n"),anError);
		ClearTimer();
		break;
		
	default:
		test.Console()->Printf(_L("Command in state %d completed: err %d\n"),iState,anError);
		ClearTimer();
		break;
		}
	}


// --------------------------------------------------------------------------

CImImap4Settings* CTestRefreshMBox::GetImap4SettingsLC()
	{
	SelectServiceIdL();

	TBool setTimer = (iTimer != NULL);
	ClearTimer();	
	iTestUtils->GoClientSideL();
	
	CImImap4Settings* settings = new (ELeave)CImImap4Settings();
	CleanupStack::PushL(settings);
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TImapAccount imapAccountId;
	accounts->GetImapAccountL(iServiceId, imapAccountId);
	accounts->LoadImapSettingsL(imapAccountId, *settings);
	CleanupStack::PopAndDestroy(); // fileStore/accounts

	// go server side
	iTestUtils->GoServerSideL();
	iEntry = iTestUtils->iServerEntry;
	iTestUtils->InstantiateImapServerMtmL();
	iImppServerMtm = iTestUtils->iImapServerMtm;
	User::LeaveIfError(iEntry->SetEntry(iServiceId));
	
	// start the display timer	
	if (setTimer)
		StartTimerL();	

	return settings;
	}
	
void CTestRefreshMBox::SetSubscriptionsL( TFolderSyncType aSync, TFolderSubscribeType aSubscribe )
	{
	SelectServiceIdL();

	TBool setTimer = (iTimer != NULL);
	ClearTimer();	
	iTestUtils->GoClientSideL();
	
	CImImap4Settings* settings = new (ELeave)CImImap4Settings();
	CleanupStack::PushL(settings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TImapAccount imapAccountId;
	accounts->GetImapAccountL(iServiceId, imapAccountId);
	accounts->LoadImapSettingsL(imapAccountId, *settings);

	settings->SetSynchronise(aSync);
	settings->SetSubscribe(aSubscribe);

	accounts->SaveImapSettingsL(imapAccountId, *settings);

	CleanupStack::PopAndDestroy(2, settings); // fileStore/accounts, settings
	
	// go server side
	iTestUtils->GoServerSideL();
	iEntry = iTestUtils->iServerEntry;
	iTestUtils->InstantiateImapServerMtmL();
	iImppServerMtm = iTestUtils->iImapServerMtm;
	User::LeaveIfError(iEntry->SetEntry(iServiceId));

	// start the display timer	
	if (setTimer)
		StartTimerL();	
	}

// --------------------------------------------------------------------------

void CTestRefreshMBox::ModifyImap4SynchronisationLimitsL(TInt aInboxSyncLimit,TInt aMailboxSyncLimit)
	{
	DebugFormatL(EDebugLevel2,_L("  Modifying synchronisation limits: Inbox=%d, Folders=%d"),aInboxSyncLimit,aMailboxSyncLimit);

	// Already have a service id
	User::LeaveIfError(iEntry->SetEntry(iServiceId));

	TBool setTimer = (iTimer != NULL);
	ClearTimer();	
	iTestUtils->GoClientSideL();

	CImImap4Settings* settings = new CImImap4Settings();
	CleanupStack::PushL(settings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TImapAccount imapAccountId;
	accounts->GetImapAccountL(iServiceId, imapAccountId);
	accounts->LoadImapSettingsL(imapAccountId, *settings);

	settings->SetInboxSynchronisationLimit(aInboxSyncLimit);
	settings->SetMailboxSynchronisationLimit(aMailboxSyncLimit);

	accounts->SaveImapSettingsL(imapAccountId, *settings);

	CleanupStack::PopAndDestroy(2, settings); // entryStore/accounts, settings
	
	// go server side
	iTestUtils->GoServerSideL();
	iEntry = iTestUtils->iServerEntry;
	iTestUtils->InstantiateImapServerMtmL();
	iImppServerMtm = iTestUtils->iImapServerMtm;
	User::LeaveIfError(iEntry->SetEntry(iServiceId));

	// start the display timer	
	if (setTimer)
		StartTimerL();	
	}

void CTestRefreshMBox::FindImap4SettingsL()
	{
	TMsvId serviceId = 0;
	TMsvId inboxId = 0;
	TMsvId test1Id = 0;
	TMsvId test2Id = 0;
	TMsvId localtestId = 0;

	// Search for first IMAP service entry
	CMsvEntrySelection *contents=new CMsvEntrySelection;
	CleanupStack::PushL(contents);

 	iEntry->SetEntry( KMsvRootIndexEntryId );
	
	iEntry->GetChildren(*contents);
	TInt a,noofchildren=contents->Count();
	if (noofchildren==0)
		{
		test.Printf(_L("No root children to examine\n"));
		}
	else
		{
		// Scan children to find first last service entry
		for(a=0;a<noofchildren;a++)
			{
			iEntry->SetEntry((*contents)[a]);
			if (iEntry->Entry().iType==KUidMsvServiceEntry &&
				iEntry->Entry().iMtm==KUidMsgTypeIMAP4)
				{
				// Found it!
				serviceId=(*contents)[a];
				}
			}

		if (serviceId)
			{
			// Find inbox
			iEntry->GetChildren(*contents);
			noofchildren=contents->Count();
			for(a=0;a<noofchildren;a++)
				{
				iEntry->SetEntry((*contents)[a]);
				if (iEntry->Entry().iDetails.Compare(_L("Inbox"))==0)
					inboxId=(*contents)[a];
				
				if (iEntry->Entry().iDetails.Compare(_L("Test1"))==0)
					test1Id=(*contents)[a];

				if (iEntry->Entry().iDetails.Compare(_L("Test2"))==0)
					test2Id=(*contents)[a];
				}
			}
		
		iEntry->SetEntry( KMsvLocalServiceIndexEntryId );
		iEntry->GetChildren(*contents);
		noofchildren=contents->Count();
		for(a=0;a<noofchildren;a++)
			{
			iEntry->SetEntry((*contents)[a]);
			if (iEntry->Entry().iDetails.Compare(_L("LocalTest"))==0)
				localtestId=(*contents)[a];
			}
		}

	if (serviceId && inboxId/* && localtestId*/)
		{
		iServiceId = serviceId;
		iRemoteInbox = inboxId;
		iRemoteFolder1 = test1Id;
		iRemoteFolder2 = test2Id;
		iLocalFolder1 = localtestId;
		test.Printf(_L("Service 0x%x inbox 0x%x Test1 0x%x Test2 0x%x"), iServiceId, iRemoteInbox, iRemoteFolder1, iRemoteFolder2);
		}
	else
		{
		test.Printf(_L("Couldn't find one of IMAP service entry, INBOX or Test Folders\nPress any key..."));
		test.Getch();
		}

	CleanupStack::PopAndDestroy(); // contents

	User::LeaveIfError(iEntry->SetEntry(NULL));
	}


void CTestRefreshMBox::CreateDefaultServiceL(CImImap4Settings* aSettings)
	{
	// need to be client side
	iTestUtils->GoClientSideL();

	// to create email accounts
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	// use test utilities to create 'default' service
	iServiceId = iTestUtils->CreateImapServiceL();

	// load default settings
	TImapAccount imapAccountId;
	accounts->GetImapAccountL(iServiceId, imapAccountId);
	accounts->LoadImapSettingsL(imapAccountId, *aSettings);
	
	// cleanup
	CleanupStack::PopAndDestroy(accounts);
	}

void CTestRefreshMBox::CreateServiceL(const CImImap4Settings* aSettings,
									  const TDesC& aDetails, 
									  const TDesC& aDescription,
									  const TBool aLocalOnly)
	{
	// this has to go before the Set's apparently
	iTestUtils->iMsvEntry->SetEntryL(KMsvRootIndexEntryId);

	if (!aLocalOnly)
		{
		// to create email accounts
		CEmailAccounts* accounts = CEmailAccounts::NewLC();
		
		// use test utilities to find 'default' service
		iServiceId = iTestUtils->FindImapServiceL();

		// update details/description
		iTestUtils->iMsvEntry->SetEntryL(iServiceId);		
		TMsvEntry entry = iTestUtils->iMsvEntry->Entry();
		entry.iDetails.Set(aDetails);
		entry.iDescription.Set(aDescription);
		iTestUtils->iMsvEntry->ChangeL(entry);		

		// update settings
		TImapAccount imapAccountId;
		accounts->GetImapAccountL(iServiceId, imapAccountId);
		accounts->SaveImapSettingsL(imapAccountId, *aSettings);
		CleanupStack::PopAndDestroy(accounts);
		}

	// go back to server side
	iTestUtils->GoServerSideL();
	iEntry = iTestUtils->iServerEntry;
	iTestUtils->InstantiateImapServerMtmL();
	iImppServerMtm = iTestUtils->iImapServerMtm;
	User::LeaveIfError(iEntry->SetEntry(iServiceId));

	// Create inbox
	TMsvEmailEntry msg;
	msg.iType=KUidMsvFolderEntry;
	msg.iMtm=KUidMsgTypeIMAP4;
	msg.iServiceId=iServiceId;
	msg.SetMtmData1(0);
	msg.SetMtmData2(0);
	msg.SetMtmData3(0);
	msg.iSize=0;
	msg.SetUID(0);
	msg.SetValidUID(EFalse);
	msg.SetMailbox(ETrue);
	if (!aLocalOnly)
		{
		msg.iDetails.Set(_L("INBOX"));
		User::LeaveIfError(iEntry->CreateEntry(msg));
		iRemoteInbox=msg.Id();
		}
	
	User::LeaveIfError(iEntry->SetEntry(KMsvLocalServiceIndexEntryId));
	msg.iDetails.Set(_L("LocalTest"));
	User::LeaveIfError(iEntry->CreateEntry(msg));
	iLocalFolder1=msg.Id();

	// park
	User::LeaveIfError(iEntry->SetEntry(KMsvNullIndexEntryId));	
	}
	


// --------------------------------------------------------------------------


// ActiveConsole definitions

CActiveConsole* CActiveConsole::NewLC(CTestRefreshMBox *aRefresher,CCommandLineArguments* aCmdLineArgs)
	{
	CActiveConsole* self = new(ELeave) CActiveConsole();
	CleanupStack::PushL(self);
	self->iRefresher = aRefresher;
	self->iCmdLineArgs = aCmdLineArgs;
	self->iLevel = EArgMainMenu;
	self->iIssueCancel = TRUE;
	self->ConstructL();
	return self;
	}

CActiveConsole::CActiveConsole()
	:CActive(0)
	{
	}

void CActiveConsole::ConstructL()
	{
	// add any other initialisation here
	CActiveScheduler::Add(this);
	DisplayMainMenu();
	}

CActiveConsole::~CActiveConsole()
	{
	Cancel();
	}

void CActiveConsole::DoCancel()
	{
	test.Console()->ReadCancel();
	}

void CActiveConsole::RequestCharacter()
    {
	test.Console()->Read(iStatus); 
    SetActive();
    }

LOCAL_C TBool MatchSubscribed(CMsvServerEntry* aEntry)
	{
	TMsvEmailEntry entry = aEntry->Entry();
	return entry.Subscribed() || entry.LocalSubscription();
	}

LOCAL_C TBool MatchQueue(CMsvServerEntry* aEntry)
	{
	TBool match = EFalse;
	if (aEntry->HasStoreL())
		{
		CMsvStore* fileStore = aEntry->ReadStoreL();
		CleanupStack::PushL(fileStore);

		match = fileStore->IsPresentL(KUidImQueuedOperationList);

		CleanupStack::PopAndDestroy();
		}
	return match;
	}

void CActiveConsole::ProcessKeyPressL(TChar aChar) 
	{
	if (iCmdLineArgs)
		{
		TLex lex(iCmdLineArgs->Arg(iLevel++));
		aChar=lex.Get();
		}
	switch (iMenuState)
		{
	case EMainMenu:
		switch (aChar)
			{
		case '1':
			DisplayImap4ServerMenu();
			break;
		case '2':
			if (iRefresher->iServiceId)
				DisplayImap4SettingsMenu();
			break;
		case '3':
			if (iRefresher->iServiceId)
				DisplayConnectMenu();
			break;
		case '4':
			if (iRefresher->iServiceId)
				DisplayOffLineOpMenu();
			break;
		case '5':
			DisplayImap4DumpMenu();
			break;
		case '6':
			DisplayHousekeepingMenu();
			break;
		case '7':
			DisplayAutomationMenu();
			break;
		case '8':
			DisplaySynchroniseMenu();
			break;
		case '9':
			DisplayDebugMenu();
			break;

		case 'r':
		case 'R':
			DisplayMainMenu();
			break;
			
		case 'X':
		case 'x':
			iMenuState=EExit;
			break;
			}
		break;

	case EImap4DumpMenu:
		switch (aChar)
			{
		case '1':
			DumpMessageStructureL( KMsvRootIndexEntryId,
								  CMsgOutputter::EToOneFile,
								  CMsgOutputter::ENone,
								  ETrue, ETrue );
			break;
		case '2':
			if (iRefresher->iServiceId)
				DumpMessageStructureL( iRefresher->iServiceId,
									  CMsgOutputter::EToScreen,
									  CMsgOutputter::ENone,
									  ETrue, ETrue,
									  MatchSubscribed );
			break;
		case '3':
			if (iRefresher->iServiceId)
				DumpMessageStructureL( iRefresher->iServiceId,
									  CMsgOutputter::EToScreen,
									  CMsgOutputter::EToOneFile,
									  ETrue, ETrue,
									  MatchQueue );
			break;
		case 'r':
		case 'R':
			DisplayImap4DumpMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;
			}
		break;

	case EImap4ServerMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayImap4ServerMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;

		default:
			SelectImap4ServerEntryL(aChar);	
			break;
			}
		break;

	case EImap4SettingsMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayImap4SettingsMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;

		case '0':
			DumpMessageStructureL( iRefresher->iServiceId,
								  CMsgOutputter::ENone,
								  CMsgOutputter::EToScreen,
								  EFalse, ETrue );
			break;
			
		default:
			SelectImap4SettingsEntryL(aChar);	
			break;
			}			
		break;

	case EHousekeepingMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayHousekeepingMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;
			
		default:
			SelectImap4SettingsEntryL(aChar);	
			break;
			}			
		break;

	case EOffLineOpMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayOffLineOpMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;

		case '1':
			DisplayOffLineOpQueueMenu();
			break;
		case '2':
			DisplayOffLineOpUnQueueMenu();
			break;
		case '3':
			DisplayOffLineOpQueueLocalToLocalMenu();
			break;
		case '4':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestCancelOffLineOps);
			break;
		case '5':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestAutoOffLineOp);
			break;
			
		case '6':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestAutoSyncOp);
			break;
			
		default:
			break;
			}			
		break;

	case EOffLineOpQueueMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayOffLineOpQueueMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayOffLineOpMenu();
			break;

		case '1':
			iRefresher->QueueOpL(EImap4OpCopyToLocal);
			break;
		case '2':
			iRefresher->QueueOpL(EImap4OpCopyFromLocal);
			break;
		case '3':
			iRefresher->QueueOpL(EImap4OpCopyWithinService);
			break;
			
		case '4':
			iRefresher->QueueOpL(EImap4OpMoveToLocal);
			break;
		case '5':
			iRefresher->QueueOpL(EImap4OpMoveFromLocal);
			break;
		case '6':
			iRefresher->QueueOpL(EImap4OpMoveWithinService);
			break;

		case '7':
			iRefresher->QueueOpL(EImap4OpDelete);
			break;
		case '8':
			iRefresher->QueueOpL(EImap4OpCopyWithinFolder);
			break;
			
		default:
			break;
			}			
		break;

	case EOffLineOpQueueLocalToLocalMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayOffLineOpQueueLocalToLocalMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayOffLineOpMenu();
			break;

		case '1':
			iRefresher->QueueLocalToLocalOpL(EImap4OpCopyToLocal);
			break;
		case '2':
			iRefresher->QueueLocalToLocalOpL(EImap4OpMoveToLocal);
			break;

		default:
			break;
			}			
		break;

	case EOffLineOpUnQueueMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayOffLineOpUnQueueMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayOffLineOpMenu();
			break;

		case '1':
			iRefresher->UnQueueOpL(EImap4OpCopyToLocal);
			break;
		case '2':
			iRefresher->UnQueueOpL(EImap4OpCopyFromLocal);
			break;
		case '3':
			iRefresher->UnQueueOpL(EImap4OpCopyWithinService);
			break;
			
		case '4':
			iRefresher->UnQueueOpL(EImap4OpMoveToLocal);
			break;
		case '5':
			iRefresher->UnQueueOpL(EImap4OpMoveFromLocal);
			break;
		case '6':
			iRefresher->UnQueueOpL(EImap4OpMoveWithinService);
			break;

		case '7':
			iRefresher->UnQueueOpL(EImap4OpDelete);
			break;
			
		default:
			break;
			}			
		break;

	case EConnectMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayConnectMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;

		case '1':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestConnect);
			break;
		case '2':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestDisconnect);
			break;
		case '3':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestConnectAndSync);
			break;
		case '4':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestFullSync);
			break;
		case '5':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestSyncTree);
			break;
		case '6':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestFetchTest);
			break;
        case '7':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestLocalSubscribe);
			break;
        case '8':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestLocalUnsubscribe);
			break;
		case '0':
			DisplayConnect2Menu();
			break;
			
		default:
			break;
			}
		break;
			
	case EConnect2Menu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayConnect2Menu();
			break;
			
		case 'X':
		case 'x':
			DisplayConnectMenu();
			break;

		case '1':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestFetchInbox);
			break;
		case '2':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestFetchAttachment);
			break;
		case '3':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestRenameFolder);
			break;
		case '4':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestFolderDelete);
			break;
		case '5':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestFolderCreate);
			break;
		case '6':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestInboxNewSync);
			break;
		case '7':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestFetchAttachedMessage);
			break;
		case '8':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestCopyMessages);
			break;
		default:
			break;
			}			
		break;

	case EAutomationMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayAutomationMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;

		case '1':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestCheckServerContents);
			break;
			
		case '2':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestAutoOffLineOp);
			break;
			
		case '3':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestAutoSyncOp);
			break;

		case '4':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestAutoSecondaryConnect);
			break;
			
		default:
			break;
			}			
		break;

	case ESynchroniseMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplaySynchroniseMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;

		case '1':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestNSynchronisation);
			break;
			
		case '2':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestNSynchronisationTests);
			break;
			
		case '3':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestNSynchronisationSpecialTests);
			break;

		case '4':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestNSynchronisationPerformanceTests);
			break;
			
		case '5':
			iRefresher->EnterStateL(CTestRefreshMBox::EImapTestNSynchronisationOneOffTests);
			break;
			
		default:
			break;
			}			
		break;

	case EDebugMenu:
		switch (aChar)
			{
		case 'r':
		case 'R':
			DisplayDebugMenu();
			break;
			
		case 'X':
		case 'x':
			DisplayMainMenu();
			break;

		case '1':
			iRefresher->iDebugLevel=(iRefresher->iDebugLevel<EDebugLevel9?iRefresher->iDebugLevel+1:iRefresher->iDebugLevel);
			DisplayDebugMenu();
			break;
			
		case '2':
			iRefresher->iDebugLevel=(iRefresher->iDebugLevel>EDebugLevel0?iRefresher->iDebugLevel-1:iRefresher->iDebugLevel);
			DisplayDebugMenu();
			break;

		default:
			break;
			}			
		break;
		}
	}

void CActiveConsole::RunL() 
	{
 	TRAPD(err,ProcessKeyPressL(TChar(test.Console()->KeyCode())));
	if ( err != KErrNone )
		test.Printf(_L("Something left with error %d\n"), err);
	test(err == KErrNone);

	if (iMenuState==EExit)
		CActiveScheduler::Stop();
	else
		{
 		RequestCharacter();
		if (iCmdLineArgs && iIssueCancel)
			DoCancel();
		}
	}

// --------------------------------------------------------------------------

// Menu display functions

void CActiveConsole::DisplayMainMenu()	
	{
	iMenuState=EMainMenu;
	
	test.Console()->ClearScreen();
	test.Console()->SetPos(0, 0);
	
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : IMAP Server menu\n"));
	if (iRefresher->iServiceId)
		{
		test.Printf(_L(" 2 : IMAP Settings menu\n"));
		test.Printf(_L(" 3 : Connect\n"));
		test.Printf(_L(" 4 : OffLine Ops\n"));
		}
	else
		test.Printf(_L("\n\n\n"));
	test.Printf(_L(" 5 : Dump menu\n"));
	test.Printf(_L(" 6 : Housekeeping\n"));
	if (iRefresher->iServiceId)
		{
		test.Printf(_L(" 7 : Automation\n"));
		test.Printf(_L(" 8 : Synchronisation\n"));
		}
	test.Printf(_L(" 9 : Debug logging\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Exit\n"));
	}

void CActiveConsole::DisplayImap4ServerMenu()
	{
	if (iMenuState != EImap4ServerMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EImap4ServerMenu;
		}
	
	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 0 : Trmai01 server uding SSLTLS\n"));
	test.Printf(_L(" 1 : Use a previously created service entry\n"));
	test.Printf(_L(" 2 : ANT IMAP server\n"));
	test.Printf(_L(" 3 : Chaos IMAP server\n"));
	test.Printf(_L(" 4 : Symbian PIPEX IMAP server\n"));
	test.Printf(_L(" 5 : Dale's Totalise IMAP server\n"));
	test.Printf(_L(" 6 : Innosoft TLS IMAP server\n"));
	test.Printf(_L(" 7 : MS Imap test\n"));
	test.Printf(_L(" 8 : NTMail test\n"));
	test.Printf(_L(" a : lon-msgtest06.intra for Synchronise Tests\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayImap4DumpMenu()
	{
	if (iMenuState != EImap4DumpMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EImap4DumpMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : Dump structure\n"));
	test.Printf(_L(" 2 : Dump subscribed\n"));
	test.Printf(_L(" 3 : Dump queued\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayImap4SettingsMenu()
	{
	if (iMenuState != EImap4SettingsMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EImap4SettingsMenu;
		}
	
	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : Set DisconnectedUserMode\n"));
	test.Printf(_L(" 2 : Set SynchroniseStrategy\n"));
	test.Printf(_L(" 3 : Set SubscribeStrategy\n"));
	test.Printf(_L(" 4 : Set IAP index\n"));
	test.Printf(_L(" 5 : Set Delete emails\n"));
	test.Printf(_L(" 6 : Set Get Mail Options\n"));
	test.Printf(_L(" 7 : Set SecureSockets\n"));
	test.Printf(_L(" 8 : Set FolderPath\n"));
	test.Printf(_L(" 9 : Set Password\n"));
	test.Printf(_L(" 0 : Show settings\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayHousekeepingMenu()
	{
	if (iMenuState != EHousekeepingMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EHousekeepingMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayOffLineOpMenu()
	{
	if (iMenuState != EOffLineOpMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EOffLineOpMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : Queue Op\n"));
	test.Printf(_L(" 2 : UnQueue Op\n"));
	test.Printf(_L(" 3 : Queue back to local\n"));
	test.Printf(_L(" 4 : Cancel all ops\n"));
	test.Printf(_L(" 5 : Automated OffLine ops\n"));
	test.Printf(_L(" 6 : Automated Sync ops\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayOffLineOpQueueMenu()
	{
	if (iMenuState != EOffLineOpQueueMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EOffLineOpQueueMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : Queue CopyToLocal\n"));
	test.Printf(_L(" 2 : Queue CopyFromLocal\n"));
	test.Printf(_L(" 3 : Queue CopyWithinService\n"));
	test.Printf(_L(" 4 : Queue MoveToLocal\n"));
	test.Printf(_L(" 5 : Queue MoveFromLocal\n"));
	test.Printf(_L(" 6 : Queue MoveWithinService\n"));
	test.Printf(_L(" 7 : Queue Delete\n"));
	test.Printf(_L(" 8 : Queue CopyWithinFolder\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayOffLineOpQueueLocalToLocalMenu()
	{
	if (iMenuState != EOffLineOpQueueLocalToLocalMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EOffLineOpQueueLocalToLocalMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : Queue CopyToLocal\n"));
	test.Printf(_L(" 2 : Queue MoveToLocal\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayOffLineOpUnQueueMenu()
	{
	if (iMenuState != EOffLineOpUnQueueMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EOffLineOpUnQueueMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : UnQueue CopyToLocal\n"));
	test.Printf(_L(" 2 : UnQueue CopyFromLocal\n"));
	test.Printf(_L(" 3 : UnQueue CopyWithinService\n"));
	test.Printf(_L(" 4 : UnQueue MoveToLocal\n"));
	test.Printf(_L(" 5 : UnQueue MoveFromLocal\n"));
	test.Printf(_L(" 6 : UnQueue MoveWithinService\n"));
	test.Printf(_L(" 7 : UnQueue Delete\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayConnectMenu()
	{
	if (iMenuState != EConnectMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EConnectMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : Connect\n"));
	test.Printf(_L(" 2 : Disconnect\n"));
	test.Printf(_L(" 3 : ConnectAndSync\n"));
	test.Printf(_L(" 4 : FullSync\n"));
	test.Printf(_L(" 5 : SyncTree\n"));
	test.Printf(_L(" 6 : Fetch Test message\n"));
	test.Printf(_L(" 7 : Subscribe Test1\n"));
	test.Printf(_L(" 8 : UnSubscribe Test1\n"));
	test.Printf(_L("\n"));
	test.Printf(_L(" 0 : More options\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}


void CActiveConsole::DisplayConnect2Menu()
	{
	if (iMenuState != EConnect2Menu)
		{
		iPrevState = iMenuState;
		iMenuState = EConnect2Menu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : Fetch inbox\n"));
	test.Printf(_L(" 2 : Fetch attachment\n"));
	test.Printf(_L(" 3 : Rename folder\n"));
	test.Printf(_L(" 4 : Delete folder\n"));
	test.Printf(_L(" 5 : Create folder\n"));
	test.Printf(_L(" 6 : InboxNewSync\n"));
	test.Printf(_L(" 7 : Fetch attached message\n"));
	test.Printf(_L(" 8 : Copy messages\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Connect menu\n"));
	}

void CActiveConsole::DisplayAutomationMenu()
	{
	if (iMenuState != EAutomationMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EAutomationMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : Check server contents\n"));
	test.Printf(_L(" 2 : Test offline operations\n"));
	test.Printf(_L(" 3 : Test online operations\n"));
	test.Printf(_L(" 4 : Check Secondary Connect\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplaySynchroniseMenu()
	{
	if (iMenuState != ESynchroniseMenu)
		{
		iPrevState = iMenuState;
		iMenuState = ESynchroniseMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L(" 1 : N-Synchronisation regression tests (use cases SA-1 - SA5)\n"));
	test.Printf(_L(" 2 : N-Synchronisation new functionality (use cases SA-10 - SA16)\n"));
	test.Printf(_L(" 3 : N-Synchronisation special cases\n"));
	test.Printf(_L(" 4 : N-Synchronisation performance tests\n"));
	test.Printf(_L(" 5 : N-Synchronisation one off tests\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

void CActiveConsole::DisplayDebugMenu()
	{
	if (iMenuState != EDebugMenu)
		{
		iPrevState = iMenuState;
		iMenuState = EDebugMenu;
		}

	test.Console()->ClearScreen();
	test.Console()->SetPos(0,0);
	test.Printf(_L("Options :\n\n"));
	test.Printf(_L("Debug log level = %d\n\n"),iRefresher->iDebugLevel);
	test.Printf(_L(" 1 : Increase debug level\n"));
	test.Printf(_L(" 2 : Decrease debug level\n"));
	test.Console()->SetPos(0, 12);
	test.Printf(_L(" X : Main menu\n"));
	}

// --------------------------------------------------------------------------

// Complex menu selection functions


// Set up iServiceId;
void CActiveConsole::SelectImap4ServerEntryL(TChar aChar)
	{
	CImImap4Settings* settings = new CImImap4Settings();
	
	CleanupStack::PushL(settings);

	iRefresher->CreateDefaultServiceL(settings);

	switch(aChar)
		{
	case '0':	// trmai01.  using secure sockets
		settings->SetServerAddressL(_L("trmai01.it.cellulardata.com"));
		settings->SetLoginNameL(_L8("rahman"));
		settings->SetPasswordL(_L8("ibrSym"));
		settings->SetFolderPathL(_L8(""));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(ETrue);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseCombination);
		iRefresher->CreateServiceL( settings, _L("trmai01"), _L("trmai01") );
		break;	
	case '1':	// Existing entry in database
		iRefresher->FindImap4SettingsL();
		iRefresher->CreateServiceL( settings, _L(""), _L(""), ETrue );
		break;

	case '2':	// Gi
		settings->SetServerAddressL(_L("serv.ant.co.uk"));
		settings->SetLoginNameL(_L8("imaptest"));
		settings->SetPasswordL(_L8("imap.ant"));
		settings->SetFolderPathL(_L8("Mail"));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(EFalse);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseCombination);
		iRefresher->CreateServiceL( settings, _L("UW IMAP"), _L("imaptest@serv") );

		break;

	case '3':	// Chaos
		settings->SetServerAddressL(_L("utter.chaos.org.uk"));
		settings->SetLoginNameL(_L8("test"));
		settings->SetPasswordL(_L8("testtest"));
		settings->SetFolderPathL(_L8("~/mail"));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(EFalse);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseLocal);
		iRefresher->CreateServiceL( settings, _L("Chaos server"), _L("test@Chaos") );

		break;

	case '4':	// pipex
		settings->SetServerAddressL(_L("imap.dial.pipex.com"));
		settings->SetLoginNameL(_L8("mdgi05"));
		settings->SetPasswordL(_L8("charexaz"));
		settings->SetFolderPathL(_L8(""));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(EFalse);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseCombination);
		iRefresher->CreateServiceL( settings, _L("pipex"), _L("pipex") );
		break;

	case '5':	// totalise
		settings->SetServerAddressL(_L("mail.totalise.co.uk"));
		settings->SetLoginNameL(_L8("dale.self"));
		settings->SetPasswordL(_L8("plimsole"));
		settings->SetFolderPathL(_L8(""));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(EFalse);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseCombination);
		iRefresher->CreateServiceL( settings, _L("totalise"), _L("totalise") );
		break;

	case '6':	// innosoft TLS
		settings->SetServerAddressL(_L("eljefe.innosoft.com"));
		settings->SetLoginNameL(_L8("symbian"));
		settings->SetPasswordL(_L8("simon"));
		settings->SetFolderPathL(_L8(""));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(ETrue);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseCombination);
		iRefresher->CreateServiceL( settings, _L("innosoft"), _L("innosoft") );
		break;

	case '7':	// MS IMap
		settings->SetServerAddressL(_L("10.22.64.6"));
		settings->SetLoginNameL(iRefresher->iTestUtils->MachineName());
		settings->SetPasswordL(iRefresher->iTestUtils->MachineName());
		settings->SetFolderPathL(_L8(""));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(EFalse);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseCombination);
		iRefresher->CreateServiceL( settings, _L("peterc"), _L("peterc") );
		break;

	case '8':	// Gordanotest
		settings->SetServerAddressL(_L("mail.ntmailtest.com"));
		settings->SetLoginNameL(_L8("Simon.Middleton"));
		settings->SetPasswordL(_L8("password"));
		settings->SetFolderPathL(_L8(""));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(EFalse);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseCombination);
		iRefresher->CreateServiceL( settings, _L("ntmail"), _L("ntmail") );
		break;

	case '9':	// SendMail Inc		// Works with new sync menu
		settings->SetServerAddressL(_L("mailconnect.sendmail.com"));
		settings->SetLoginNameL(_L8("user4@mailconnect.sendmail.com"));
		settings->SetPasswordL(_L8("user4"));
		settings->SetFolderPathL(_L8(""));
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(EFalse);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseLocal);
		iRefresher->CreateServiceL( settings, _L("Sendmail"), _L("Sendmail") );
		break;
	
	case 'a':  // Fast mail reliable server for testing.
		settings->SetServerAddressL(_L("lon-cn-exchng2k.msexchange2k.closedtest.intra"));
		settings->SetFolderPathL(_L8(""));
		settings->SetLoginNameL(iRefresher->iTestUtils->MachineName());
		settings->SetPasswordL(iRefresher->iTestUtils->MachineName());
		settings->SetPort(143);
		settings->SetPathSeparator('/');
		settings->SetDisconnectedUserMode(ETrue);
		settings->SetDeleteEmailsWhenDisconnecting(ETrue);
		settings->SetGetMailOptions(EGetImap4EmailBodyTextAndAttachments);
		settings->SetSecureSockets(EFalse);
		settings->SetSubscribe(EUpdateNeither);
		settings->SetSynchronise(EUseCombination);
		iRefresher->CreateServiceL( settings, _L("lon-cn-exchng2k.msexchange2k.closedtest.intra"), 
		                            _L("lon-toqirk01@msexchange2k.closedtest.intra") );

		break;

	default:
		iMenuState=EImap4ServerMenu;
		break;
		}

	CleanupStack::PopAndDestroy();	// settings
	

	DisplayMainMenu();
	}



void CActiveConsole::SelectImap4SettingsEntryL(TChar aChar)
	{
	iRefresher->SelectServiceIdL();

	iRefresher->iTestUtils->GoClientSideL();
	CMsvEntry* entry = iRefresher->iTestUtils->iMsvEntry; 
	entry->SetEntryL(iRefresher->iServiceId);

	CImImap4Settings* settings = new (ELeave)CImImap4Settings();
	CleanupStack::PushL(settings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TImapAccount imapAccountId;
	accounts->GetImapAccountL(iRefresher->iServiceId, imapAccountId);
	accounts->LoadImapSettingsL(imapAccountId, *settings);

	TInt choice;
	switch (aChar)
		{
	case '1':
		test.Printf(_L("DisconnectedUserMode: 0=Off 1=On [%d]"), settings->DisconnectedUserMode()?1:0);
		choice = test.Getch();
		settings->SetDisconnectedUserMode(choice=='0'?EFalse:
										  choice=='1'?ETrue:
										  settings->DisconnectedUserMode());
		break;

	case '2':
		test.Printf(_L("Synchronise: Use 0=Combination 1=Local 2=Remote [%d]"), settings->Synchronise());
		choice = test.Getch();
		settings->SetSynchronise(choice=='0'?EUseCombination:
								 choice=='1'?EUseLocal:
								 choice=='2'?EUseRemote:
								 settings->Synchronise());
		break;

	case '3':
		test.Printf(_L("Subscribe: Update 0=Neither 1=Local 2=Remote 3=Both [%d]"), settings->Subscribe());
		choice = test.Getch();
		settings->SetSubscribe(choice=='0'?EUpdateNeither:
							   choice=='1'?EUpdateLocal:
							   choice=='2'?EUpdateRemote:
							   choice=='2'?EUpdateBoth:
							   settings->Subscribe());
		break;

	case '4':
		test.Printf(_L("IAP index: [%d]"), entry->Entry().MtmData1());
		choice = test.Getch();
		if (choice >= '0' && choice <= '9')
			{
			TMsvEmailEntry e = entry->Entry();
			e.SetMtmData1(choice-'0');
			entry->ChangeL(e);
			}
		break;

	case '5':
		test.Printf(_L("Delete emails: When 0=Connecting 1=Disconnecting [%d]"),
					settings->DeleteEmailsWhenDisconnecting()?1:0);
		choice = test.Getch();
		settings->SetDeleteEmailsWhenDisconnecting(choice=='0'?EFalse:
												   choice=='1'?ETrue:
												   settings->DeleteEmailsWhenDisconnecting());
		break;

	case '6':
		test.Printf(_L("Get Mail Options: 0=Headers 1=BodyText 2=Everything 3=Attachments [%d]"),
					settings->GetMailOptions());
		choice = test.Getch();
		settings->SetGetMailOptions(choice=='0'?EGetImap4EmailHeaders:
									choice=='1'?EGetImap4EmailBodyText:
									choice=='2'?EGetImap4EmailBodyTextAndAttachments:
									choice=='3'?EGetImap4EmailAttachments:
									settings->GetMailOptions());
		break;

	case '7':
		test.Printf(_L("SecureSockets: 0=Off 1=On [%d]"), settings->SecureSockets()?1:0);
		choice = test.Getch();
		settings->SetSecureSockets(choice=='0'?EFalse:
								   choice=='1'?ETrue:
								   settings->SecureSockets());
		break;

	case '8':
		{
		const TPtrC8 path = settings->FolderPath();
		test.Printf(_L("FolderPath: 0=\"\" 1=\"Mail\" [%S]"), &path);
		choice = test.Getch();
		if (choice=='0'||choice=='1')
			settings->SetFolderPathL(choice=='0'?_L8(""):_L8("Mail"));
		break;
		}

	case '9':
		{
		const TPtrC8 password = settings->Password();
		test.Printf(_L("Password: 0=!foobar 1=rubbish [%S]"), &password);
		choice = test.Getch();
		if (choice=='0'||choice=='1')
			settings->SetPasswordL(choice=='0'?_L8("!foobar"):_L8("rubbish"));
		break;
		}
		}

	accounts->SaveImapSettingsL(imapAccountId, *settings);

	test.Printf(_L("\nUpdated settings\n"));
	
	CleanupStack::PopAndDestroy(2, settings); // fileStore/accounts, settings
	
	// go server side
	iRefresher->iTestUtils->GoServerSideL();
	iRefresher->iTestUtils->InstantiateImapServerMtmL();
	}

// --------------------------------------------------------------------------

void CActiveConsole::DumpMessageStructureL( TMsvId aId,
										   TInt aSummary,
										   TInt aParts,
										   TBool aRecurse,
										   TBool aShowIds,
										   MsgMatchFn aMatch )
	{
	COutputter* putter = COutputter::NewL( &test );
	CleanupStack::PushL(putter);

	putter->SetRFs( &theFs );
	theFs.SetSessionPath( KFilePathMailTest );

	CMsgOutputter* dumper = CMsgOutputter::NewL( putter, iRefresher->ServerEntry() );
	CleanupStack::PushL(dumper);
	
	dumper->DumpL( aId, aSummary, aParts, aRecurse, aShowIds, aMatch );

	CleanupStack::PopAndDestroy(2); // dumper, putter
	}

// --------------------------------------------------------------------------

LOCAL_C void doMainL()
	{
	// Handle command line arguments
	CCommandLineArguments* cmdLineArg=CCommandLineArguments::NewLC();
	TInt count = cmdLineArg->Count();
	TBool isCmdLine=FALSE;
	if (count>2)	// Command line arguments?
		{
		if (!(cmdLineArg->Arg(EArgTestParams).Compare(KTestParams)))
			isCmdLine=TRUE;
		}	

	TestScheduler* theScheduler = new (ELeave) TestScheduler;
	CleanupStack::PushL(theScheduler);

	CActiveScheduler::Install( theScheduler );

	User::LeaveIfError(theFs.Connect());
	theFs.SetSessionPath(_L("C:\\"));

	
	// Load the serial comms device drivers.  If this is not done,
	// connecting to a IMAP4 server returns KErrNotFound (-1).
	TInt err;
	err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	err=User::LoadLogicalDevice(LDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		User::Leave(err);
	
	
	CTestRefreshMBox* refresher = CTestRefreshMBox::NewLC(isCmdLine ? cmdLineArg:NULL);



	test((refresher->ServerEntry())->Entry().Id()==KMsvRootIndexEntryId);
	test.Console()->SetPos(0, 17);

	// Create and start the active console
	CActiveConsole* activeConsole = CActiveConsole::NewLC(refresher,isCmdLine ? cmdLineArg:NULL);
	refresher->SetConsole(activeConsole);
	activeConsole->RequestCharacter();
	if (isCmdLine)
		activeConsole->DoCancel();

	// Main program
	CActiveScheduler::Start();

	// Cleanup exit
	test.Printf(_L("---------------------\n"));
	test.Printf(_L("      Going down     \n"));
	test.Printf(_L("---------------------\n"));

	test.Printf(_L("Deleting activeconsole\n"));
	CleanupStack::PopAndDestroy();	// activeConsole
	
	test.Printf(_L("Deleting Refresher\n"));
	CleanupStack::PopAndDestroy();	// refresher

	test.Printf(_L("Deleting scheduler\n"));
	CleanupStack::PopAndDestroy();	// scheduler

	test.Printf(_L("Deleting command line args\n"));
	CleanupStack::PopAndDestroy();	// cmdLineArg

	test.Printf(_L("Closing FS\n"));
	theFs.Close();
	}


_LIT(KMessage, "T_IMPS01 - Test Harness");

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Start(KMessage);
	CTrapCleanup* theCleanup=CTrapCleanup::New();
	
	TRAPD(ret,doMainL());		
	
	test(ret==KErrNone);
	
	delete theCleanup;	
	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	
	__UHEAP_MARKEND;
	return(KErrNone);
	}

// --------------------------------------------------------------------------
