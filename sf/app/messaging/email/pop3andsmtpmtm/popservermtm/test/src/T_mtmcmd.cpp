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
//

#include "emailtestutils.h"
#include "miutlog.h"
#include "miutmsg.h"
#include <txtrich.h>
#include "T_mtmcmd.h"


_LIT(K_CHANGE_COMMAND, "Change");
_LIT(K_COPY_FROM_LOCAL_COMMAND, "Copy from local");
_LIT(K_COPY_TO_LOCAL_COMMAND, "Copy to local");
_LIT(K_COPY_WITHIN_SERVICE_COMMAND, "Copy within service");
_LIT(K_DELETE_ALL_COMMAND, "Delete all");
_LIT(K_MOVE_FROM_LOCAL_COMMAND, "Move from local");
_LIT(K_MOVE_TO_LOCAL_COMMAND, "Move to local");
_LIT(K_MOVE_WITHIN_SERVICE_COMMAND, "Move within service");
_LIT(K_START_COMMAND, "Start command");
_LIT(K_TEST_ON_ALL, " (run on all messages)");
_LIT(K_TEST_COMPLETED, "Completed - ");
_LIT(K_TEST_STARTED, "Started - ");
_LIT(K_TEST_FAILED, "Failed - ");
_LIT(K_TEST_COMPLETED_OKAY, "Test Completed OK");
_LIT(K_TEST_ON_SINGLE, " (run on single message)");
_LIT(K_TEST_ON_SECOND, " (run on second message)");
_LIT(K_TEST_ON_SECOND_CHILDREN, " (run on children of second message)");
_LIT(K_TOP_POPULATE_COMMAND, "Populate using TOP");

// Generic Server MTM Test Harness

//

CMtmTestHarness* CMtmTestHarness::NewL(CBaseServerMtm& aServerMtm, CTestUtils& aTestUtils)
	{
	CMtmTestHarness* self = new (ELeave) CMtmTestHarness(aServerMtm, aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// Test harness set up
void CMtmTestHarness::Reset()
	{
	Cancel();

	TInt index = iCommandList->Count();
	while (index != 0)
		{
		index--;
		delete (*iCommandList)[index];
		}

	iCommandList->Reset();
	iCurrentCommand = 0;
	iCommandIndex = 0;
	}

void CMtmTestHarness::AddCommandL(CMtmTestCommand* aCommand)
	{
	iCommandList->AppendL(aCommand);
	}

// Run the test harness
void CMtmTestHarness::StartL(const TDesC& aTestName, TRequestStatus& aStatus)
	{
	iFailed = EFalse;
	iCommandIndex = 0;
	iCurrentCommand = 0;
	iStatus = KRequestPending;
	iReportStatus = &aStatus;
	*iReportStatus = KRequestPending;

	iTestUtils.WriteComment(aTestName);

	if (iCommandIndex < iCommandList->Count())
		{
		RunCurrentCommandL();
		}
	else
		{
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

TInt CMtmTestHarness::StartL(const TDesC& aTestName)
	{
	CTestActive* waiter = new (ELeave) CTestActive;
	CleanupStack::PushL(waiter);
	waiter->StartL();
	StartL(aTestName, waiter->iStatus);
	CActiveScheduler::Start();
	TInt status = waiter->iStatus.Int();
	CleanupStack::PopAndDestroy(); // waiter
	return status;
	}

// Status information
const TDesC8& CMtmTestHarness::Progress()
	{
	return iServerMtm.Progress();
	}

CBaseServerMtm&	CMtmTestHarness::ServerMtm()
	{
	return iServerMtm;
	}

// Constructor & destructor
CMtmTestHarness::CMtmTestHarness(CBaseServerMtm& aServerMtm, CTestUtils& aTestUtils) : CActive(EPriorityNormal), iServerMtm(aServerMtm), iTestUtils(aTestUtils)
	{}

CMtmTestHarness::~CMtmTestHarness()
	{
	Reset();
	delete iCommandList;
	}

void CMtmTestHarness::ConstructL()
	{
	CActiveScheduler::Add(this);
	iCommandList = new (ELeave) CArrayFixFlat<CMtmTestCommand*>(6);
	Reset();
	}

void CMtmTestHarness::DoCancel()
	{
	if (iCurrentCommand)
		{
		iCurrentCommand->Cancel();
		}
	}

TInt CMtmTestHarness::RunError(TInt aError)
	{
	if (iReportStatus)
		User::RequestComplete(iReportStatus,aError);
	return KErrNone;
	}

void CMtmTestHarness::RunL()
	{
	if (iCurrentCommand)
		{
		TBool failed = EFalse;
		MCommandTester* tester = iCurrentCommand->Tester();
		if (tester)
			{
			if (!(tester->EndL()))
				{
				HBufC* logString = HBufC::NewLC(((TDesC&)K_TEST_FAILED).Size() + iCurrentCommand->DetailsL().Size());
				*logString = (TDesC&)K_TEST_FAILED;
				logString->Des().Append(iCurrentCommand->DetailsL());
				iTestUtils.WriteComment(logString->Des());
				CleanupStack::PopAndDestroy(); // logString


				// log 'FAILED + the info string from the tester'
				HBufC* failureInfo = tester->TextInfoL();
				if (failureInfo)
					{
					CleanupStack::PushL(failureInfo);
					iTestUtils.WriteComment(failureInfo->Des());
					CleanupStack::PopAndDestroy(); // failureInfo
					}

				iFailed = ETrue;
				failed = ETrue;
				}
			}

		if (!failed)
			{
			HBufC* logString = HBufC::NewLC(((TDesC&)K_TEST_COMPLETED).Size() + iCurrentCommand->DetailsL().Size());
			*logString = (TDesC&)K_TEST_COMPLETED;
			logString->Des().Append(iCurrentCommand->DetailsL());
			iTestUtils.WriteComment(logString->Des());
			CleanupStack::PopAndDestroy(); // logString
			}
		}

	// Run the next command if there is one.
	iCommandError = KErrNone;
 	iCommandIndex++;
	if ((iCommandIndex < iCommandList->Count())
		&& (iStatus == KErrNone))
		{
		// Check for any Command Errors
		TRAPD(err, RunCurrentCommandL());
		if (err != KErrNone)
			{
			// Store the Command Error & Leave
			iCommandError = err;
			User::Leave(err);
			}
		}
	else
	// otherwise complete the report status.
		{
		iTestUtils.WriteComment(K_TEST_COMPLETED_OKAY);
		User::RequestComplete(iReportStatus, iStatus.Int());
		Reset();
		}
	}

void CMtmTestHarness::RunCurrentCommandL()
	{
	iStatus = KRequestPending;
	SetActive();
	iCurrentCommand = (*iCommandList)[iCommandIndex];
	MCommandTester* tester = iCurrentCommand->Tester();

	if (tester)
		tester->InitialiseL();

	iCurrentCommand->ExecuteCommandL(iStatus);

	if (tester)
		tester->StartL();

	HBufC* logString = HBufC::NewLC(((TDesC&)K_TEST_STARTED).Size() + iCurrentCommand->DetailsL().Size());
	*logString = (TDesC&)K_TEST_STARTED;
	logString->Des().Append(iCurrentCommand->DetailsL());
	iTestUtils.WriteComment(logString->Des());
	CleanupStack::PopAndDestroy(); // logString
	}

//

// Run this command
void CMtmTestCommand::DoCancel()
	{
	iServerMtm.Cancel();
	}

TInt CMtmTestCommand::RunError(TInt aError)
	{
	if (iReportStatus)
		User::RequestComplete(iReportStatus,aError);
	return KErrNone;
	}

void CMtmTestCommand::RunL()
	{
	TPop3Progress temp;	
	TPckgC<TPop3Progress> paramPack(temp);
	const TDesC8& progBuf = iServerMtm.Progress();	
	paramPack.Set(progBuf);
	TPop3Progress progress=paramPack();	

	TInt err=iStatus.Int();
	if (!err && progress.iErrorCode )
		err=progress.iErrorCode; //pass the error
	User::RequestComplete(iReportStatus, err);
	}

CMtmTestCommand::~CMtmTestCommand()
	{
	delete iSelection;
	delete iTester;
	delete iDetails;
	}

void CMtmTestCommand::CopySelectionL(const CMsvEntrySelection* aSelection)
	{
	delete iSelection;
	iSelection = 0;
	if (aSelection)
		iSelection = aSelection->CopyL();
	}

CMtmTestCommand::CMtmTestCommand(CBaseServerMtm& aServerMtm) : CActive(EPriorityNormal),iServerMtm(aServerMtm)
	{
	CActiveScheduler::Add(this);
	}

void CMtmTestCommand::Queue(TRequestStatus& aReportStatus)
	{
	iReportStatus = &aReportStatus;
	iStatus = KRequestPending;
	SetActive();
	}

void CMtmTestCommand::SetCommandTester(MCommandTester* aTester)
	{
	delete iTester;
	iTester = 0;
	iTester = aTester;
	}

MCommandTester* CMtmTestCommand::Tester()
	{
	return iTester;
	}

TPtrC CMtmTestCommand::DetailsL()
	{
	delete iDetails;
	iDetails = NULL;
	iDetails = CommandDetailsL();
	if (iDetails == NULL)
		{
		iDetails = HBufC::NewL(0);
		}

	return TPtrC(iDetails->Des());
	}


//

void CMtmTestChange::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.ChangeL(iNewEntry, iStatus);
	}

HBufC* CMtmTestChange::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_CHANGE_COMMAND).Size());
	*details = (TDesC&)K_CHANGE_COMMAND;
	return details;
	}

CMtmTestChange* CMtmTestChange::NewL(CBaseServerMtm& aServerMtm, TMsvEntry aNewEntry)
	{
	CMtmTestChange* self = new (ELeave) CMtmTestChange(aServerMtm, aNewEntry);
	return self;
	}

CMtmTestChange::CMtmTestChange(CBaseServerMtm& aServerMtm, TMsvEntry aNewEntry) : CMtmTestCommand(aServerMtm),iNewEntry(aNewEntry)
	{}

//

void CMtmTestDeleteAll::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.DeleteAllL(*iSelection, iStatus);
	}

HBufC* CMtmTestDeleteAll::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_DELETE_ALL_COMMAND).Size());
	*details = (TDesC&)K_DELETE_ALL_COMMAND;
	return details;
	}

CMtmTestDeleteAll* CMtmTestDeleteAll::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection)
	{
	CMtmTestDeleteAll* self = new (ELeave) CMtmTestDeleteAll(aServerMtm, aSelection);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	CleanupStack::Pop(); // self
	return self;
	}

CMtmTestDeleteAll::CMtmTestDeleteAll(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/) : CMtmTestCommand(aServerMtm)
	{}

//

void CMtmTestCopyToLocal::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.CopyToLocalL(*iSelection, iDest, iStatus);
	}

HBufC* CMtmTestCopyToLocal::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_COPY_TO_LOCAL_COMMAND).Size());
	*details = (TDesC&)K_COPY_TO_LOCAL_COMMAND;
	return details;
	}

CMtmTestCopyToLocal* CMtmTestCopyToLocal::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest)
	{
	CMtmTestCopyToLocal* self = new (ELeave) CMtmTestCopyToLocal(aServerMtm, aSelection, aDest);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	CleanupStack::Pop(); // self
	return self;
	}

CMtmTestCopyToLocal::CMtmTestCopyToLocal(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/, TMsvId aDest) : CMtmTestCommand(aServerMtm), iDest(aDest)
	{}

//

void CMtmTestCopyFromLocal::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.CopyFromLocalL(*iSelection, iDest, iStatus);
	}

HBufC* CMtmTestCopyFromLocal::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_COPY_FROM_LOCAL_COMMAND).Size());
	*details = (TDesC&)K_COPY_FROM_LOCAL_COMMAND;
	return details;
	}

CMtmTestCopyFromLocal* CMtmTestCopyFromLocal::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest)
	{
	CMtmTestCopyFromLocal* self = new (ELeave) CMtmTestCopyFromLocal(aServerMtm, aSelection, aDest);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	CleanupStack::Pop(); // self
	return self;
	}

CMtmTestCopyFromLocal::CMtmTestCopyFromLocal(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/, TMsvId aDest) : CMtmTestCommand(aServerMtm), iDest(aDest)
	{}

//

void CMtmTestTopPopulate::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	TInt command = KPOP3MTMPopulate;
	TImPop3PopulateOptions pop3GetMailInfo;	
	pop3GetMailInfo.SetPopulationLimit(64);
	TPckgC<TImPop3PopulateOptions> paramPack(pop3GetMailInfo);
	
	iServerMtm.StartCommandL(*iSelection,command,paramPack,iStatus);
	}

HBufC* CMtmTestTopPopulate::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_TOP_POPULATE_COMMAND).Size());
	*details = (TDesC&)K_TOP_POPULATE_COMMAND;
	return details;
	}

CMtmTestTopPopulate* CMtmTestTopPopulate::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest)
	{
	CMtmTestTopPopulate* self = new (ELeave) CMtmTestTopPopulate(aServerMtm, aSelection, aDest);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	CleanupStack::Pop(self); // self
	return self;
	}

CMtmTestTopPopulate::CMtmTestTopPopulate(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/, TMsvId aDest) : CMtmTestCommand(aServerMtm), iDest(aDest)
	{}

//

void CMtmTestCopyWithinService::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.CopyWithinServiceL(*iSelection, iDest, iStatus);
	}

HBufC* CMtmTestCopyWithinService::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_COPY_WITHIN_SERVICE_COMMAND).Size());
	*details = (TDesC&)K_COPY_WITHIN_SERVICE_COMMAND;
	return details;
	}

CMtmTestCopyWithinService* CMtmTestCopyWithinService::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest)
	{
	CMtmTestCopyWithinService* self = new (ELeave) CMtmTestCopyWithinService(aServerMtm, aSelection, aDest);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	CleanupStack::Pop(); // self
	return self;
	}

CMtmTestCopyWithinService::CMtmTestCopyWithinService(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/, TMsvId aDest) : CMtmTestCommand(aServerMtm), iDest(aDest)
	{}

//

void CMtmTestMoveToLocal::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.MoveToLocalL(*iSelection, iDest, iStatus);
	}

HBufC* CMtmTestMoveToLocal::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_MOVE_TO_LOCAL_COMMAND).Size());
	*details = (TDesC&)K_MOVE_TO_LOCAL_COMMAND;
	return details;
	}

CMtmTestMoveToLocal* CMtmTestMoveToLocal::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest)
	{
	CMtmTestMoveToLocal* self = new (ELeave) CMtmTestMoveToLocal(aServerMtm, aSelection, aDest);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	CleanupStack::Pop(); // self
	return self;
	}

CMtmTestMoveToLocal::CMtmTestMoveToLocal(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/, TMsvId aDest) : CMtmTestCommand(aServerMtm), iDest(aDest)
	{}

//

void CMtmTestMoveFromLocal::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.MoveFromLocalL(*iSelection, iDest, iStatus);
	}

HBufC* CMtmTestMoveFromLocal::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_MOVE_FROM_LOCAL_COMMAND).Size());
	*details = (TDesC&)K_MOVE_FROM_LOCAL_COMMAND;
	return details;
	}

CMtmTestMoveFromLocal* CMtmTestMoveFromLocal::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest)
	{
	CMtmTestMoveFromLocal* self = new (ELeave) CMtmTestMoveFromLocal(aServerMtm, aSelection, aDest);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	CleanupStack::Pop(); // self
	return self;
	}

CMtmTestMoveFromLocal::CMtmTestMoveFromLocal(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/, TMsvId aDest) : CMtmTestCommand(aServerMtm), iDest(aDest)
	{}

//

void CMtmTestMoveWithinService::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.MoveWithinServiceL(*iSelection, iDest, iStatus);
	}

HBufC* CMtmTestMoveWithinService::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_MOVE_WITHIN_SERVICE_COMMAND).Size());
	*details = (TDesC&)K_MOVE_WITHIN_SERVICE_COMMAND;
	return details;
	}

CMtmTestMoveWithinService* CMtmTestMoveWithinService::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest)
	{
	CMtmTestMoveWithinService* self = new (ELeave) CMtmTestMoveWithinService(aServerMtm, aSelection, aDest);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	CleanupStack::Pop(); // self
	return self;
	}

CMtmTestMoveWithinService::CMtmTestMoveWithinService(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/, TMsvId aDest) : CMtmTestCommand(aServerMtm), iDest(aDest)
	{}

//

void CMtmTestStartCommand::ExecuteCommandL(TRequestStatus& aStatus)
	{
	Queue(aStatus);
	iServerMtm.StartCommandL(*iSelection, iCommandId, iParameter->Des(), iStatus);
	}

HBufC* CMtmTestStartCommand::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_START_COMMAND).Size());
	*details = (TDesC&)K_START_COMMAND;
	return details;
	}

CMtmTestStartCommand* CMtmTestStartCommand::NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TInt aCommandId, TDesC8& aParameter)
	{
	CMtmTestStartCommand* self = new (ELeave) CMtmTestStartCommand(aServerMtm, aSelection, aCommandId, aParameter);
	CleanupStack::PushL(self);
	self->CopySelectionL(aSelection);
	self->iParameter = HBufC8::NewL(aParameter.Size());
	*(self->iParameter) = aParameter;
	CleanupStack::Pop(); // self
	return self;
	}

CMtmTestStartCommand::~CMtmTestStartCommand()
	{
	delete iParameter;
	}

CMtmTestStartCommand::CMtmTestStartCommand(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* /*aSelection*/, TInt aCommandId, TDesC8& /*aParamter*/) : CMtmTestCommand(aServerMtm), iCommandId(aCommandId)
	{}

//

// Runs a command on all messages directly under a given message entry.
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.

void CMtmTestCommandOnAllMessages::ExecuteCommandL(TRequestStatus& aStatus)
	{
	// Get the selection of messages.
	User::LeaveIfError(iServerEntry.SetEntry(iRootId));
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	User::LeaveIfError(iServerEntry.GetChildren(*selection));

	// Set the entry selection of the child command.
	iRealCommand->CopySelectionL(selection);
	CleanupStack::PopAndDestroy(); // selection

	// Run the child command.
	Queue(aStatus);
	iRealCommand->ExecuteCommandL(iStatus);
	}

HBufC* CMtmTestCommandOnAllMessages::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_TEST_ON_ALL).Size() + iRealCommand->DetailsL().Size());
	*details = iRealCommand->DetailsL();
	details->Des().Append((TDesC&)K_TEST_ON_ALL);
	return details;
	}

CMtmTestCommandOnAllMessages* CMtmTestCommandOnAllMessages::NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry)
	{
	CMtmTestCommandOnAllMessages* self = new (ELeave) CMtmTestCommandOnAllMessages(aServerMtm, aCommand, aRootId, aEntry);
	return self;
	}

CMtmTestCommandOnAllMessages::~CMtmTestCommandOnAllMessages()
	{
	iRealCommand->Cancel();
	delete iRealCommand;
	}

CMtmTestCommandOnAllMessages::CMtmTestCommandOnAllMessages(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry) : CMtmTestCommand(aServerMtm), iRealCommand(aCommand), iRootId(aRootId), iServerEntry(aEntry)
	{
	}

//

// Runs a command on the second message directly under a given message entry.
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.

void CMtmTestCommandOnSecondMessage::ExecuteCommandL(TRequestStatus& aStatus)
	{
	// Get the selection of messages.
	User::LeaveIfError(iServerEntry.SetEntry(iRootId));
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	User::LeaveIfError(iServerEntry.GetChildren(*selection));
	CMsvEntrySelection* secondSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(secondSelection);
	secondSelection->AppendL((*selection)[1]);

	// Set the entry selection of the child command.
	iRealCommand->CopySelectionL(secondSelection);
	CleanupStack::PopAndDestroy(2); // secondSelection, selection

	// Run the child command.
	Queue(aStatus);
	iRealCommand->ExecuteCommandL(iStatus);
	}

HBufC* CMtmTestCommandOnSecondMessage::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_TEST_ON_SECOND).Size() + iRealCommand->DetailsL().Size());
	*details = iRealCommand->DetailsL();
	details->Des().Append((TDesC&)K_TEST_ON_SECOND);
	return details;
	}

CMtmTestCommandOnSecondMessage* CMtmTestCommandOnSecondMessage::NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry)
	{
	CMtmTestCommandOnSecondMessage* self = new (ELeave) CMtmTestCommandOnSecondMessage(aServerMtm, aCommand, aRootId, aEntry);
	return self;
	}

CMtmTestCommandOnSecondMessage::~CMtmTestCommandOnSecondMessage()
	{
	iRealCommand->Cancel();
	delete iRealCommand;
	}

CMtmTestCommandOnSecondMessage::CMtmTestCommandOnSecondMessage(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry) : CMtmTestCommand(aServerMtm), iRealCommand(aCommand), iRootId(aRootId), iServerEntry(aEntry)
	{
	}

//

// Runs a command on the children of the second message directly under a given message entry.
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.

void CMtmTestCommandOnChildrenOf2ndMsg::ExecuteCommandL(TRequestStatus& aStatus)
	{
	// Get the selection of messages.
	User::LeaveIfError(iServerEntry.SetEntry(iRootId));
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	User::LeaveIfError(iServerEntry.GetChildren(*selection));
	CMsvEntrySelection* secondSelection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(secondSelection);

	// Get the selection of children of the second message
	User::LeaveIfError(iServerEntry.SetEntry((*selection)[1]));	
	User::LeaveIfError(iServerEntry.GetChildren(*secondSelection));

	// Set the entry selection of the child command.
	iRealCommand->CopySelectionL(secondSelection);
	CleanupStack::PopAndDestroy(2); // secondSelection, selection

	// Run the child command.
	Queue(aStatus);
	iRealCommand->ExecuteCommandL(iStatus);
	}

HBufC* CMtmTestCommandOnChildrenOf2ndMsg::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_TEST_ON_SECOND_CHILDREN).Size() + iRealCommand->DetailsL().Size());
	*details = iRealCommand->DetailsL();
	details->Des().Append((TDesC&)K_TEST_ON_SECOND_CHILDREN);
	return details;
	}

CMtmTestCommandOnChildrenOf2ndMsg* CMtmTestCommandOnChildrenOf2ndMsg::NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry)
	{
	CMtmTestCommandOnChildrenOf2ndMsg* self = new (ELeave) CMtmTestCommandOnChildrenOf2ndMsg(aServerMtm, aCommand, aRootId, aEntry);
	return self;
	}

CMtmTestCommandOnChildrenOf2ndMsg::~CMtmTestCommandOnChildrenOf2ndMsg()
	{
	iRealCommand->Cancel();
	delete iRealCommand;
	}

CMtmTestCommandOnChildrenOf2ndMsg::CMtmTestCommandOnChildrenOf2ndMsg(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry) : CMtmTestCommand(aServerMtm), iRealCommand(aCommand), iRootId(aRootId), iServerEntry(aEntry)
	{
	}

//

//

// Runs a command on the specified messages directly under a given message entry.
// The messages must exist, e.g. remote mailbox must be populated or this object leaves.
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.

void CMtmTestCommandOnSpecifiedMessages::ExecuteCommandL(TRequestStatus& aStatus)
	{
	// Get the selection of messages.
	if (!iSpecifiedEntries || iSpecifiedEntries->Count()==0)
		User::Leave(KErrNotFound);

	// Set the entry selection of the child command.
	iRealCommand->CopySelectionL(iSpecifiedEntries);

	// Run the child command.
	Queue(aStatus);
	iRealCommand->ExecuteCommandL(iStatus);
	}

HBufC* CMtmTestCommandOnSpecifiedMessages::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_TEST_ON_ALL).Size() + iRealCommand->DetailsL().Size());
	*details = iRealCommand->DetailsL();
	details->Des().Append((TDesC&)K_TEST_ON_ALL);
	return details;
	}

CMtmTestCommandOnSpecifiedMessages* CMtmTestCommandOnSpecifiedMessages::NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry, CMsvEntrySelection* aSpecifiedEntries)
	{
	CMtmTestCommandOnSpecifiedMessages* self = new (ELeave) CMtmTestCommandOnSpecifiedMessages(aServerMtm, aCommand, aRootId, aEntry,aSpecifiedEntries);
	return self;
	}

CMtmTestCommandOnSpecifiedMessages::~CMtmTestCommandOnSpecifiedMessages()
	{
	iRealCommand->Cancel();
	delete iRealCommand;
	delete iSpecifiedEntries;
	}

CMtmTestCommandOnSpecifiedMessages::CMtmTestCommandOnSpecifiedMessages(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry, CMsvEntrySelection* aSpecifiedEntries) 
: CMtmTestCommand(aServerMtm), iRealCommand(aCommand), iRootId(aRootId), iServerEntry(aEntry), iSpecifiedEntries(aSpecifiedEntries)
	{
	}


//

// Runs a command on the specified message directly under a given message entry.
// (NB specified as a positive integer value NOT TMsvId !!!)
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.

void CMtmTestCommandOnSingleMessage::ExecuteCommandL(TRequestStatus& aStatus)
	{
	// Get the selection of messages.
	CMsvEntrySelection* selection = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(selection);
	selection->AppendL( iSpecifiedEntry );
	iRealCommand->CopySelectionL(selection);
	CleanupStack::PopAndDestroy(selection); // selection

	// Run the child command.
	Queue(aStatus);
	iRealCommand->ExecuteCommandL(iStatus);
	}

HBufC* CMtmTestCommandOnSingleMessage::CommandDetailsL()
	{
	HBufC* details = HBufC::NewL(((TDesC&)K_TEST_ON_SINGLE).Size() + iRealCommand->DetailsL().Size());
	*details = iRealCommand->DetailsL();
	details->Des().Append((TDesC&)K_TEST_ON_SINGLE);
	return details;
	}

CMtmTestCommandOnSingleMessage* CMtmTestCommandOnSingleMessage::NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry, TMsvId aSpecifiedEntry)
	{
	CMtmTestCommandOnSingleMessage* self = new (ELeave) CMtmTestCommandOnSingleMessage(aServerMtm, aCommand, aRootId, aEntry,aSpecifiedEntry);
	return self;
	}

CMtmTestCommandOnSingleMessage::~CMtmTestCommandOnSingleMessage()
	{
	iRealCommand->Cancel();
	delete iRealCommand;
	}

CMtmTestCommandOnSingleMessage::CMtmTestCommandOnSingleMessage(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry, TMsvId aSpecifiedEntry) 
: CMtmTestCommand(aServerMtm), iRealCommand(aCommand), iRootId(aRootId), iServerEntry(aEntry), iSpecifiedEntry(aSpecifiedEntry)
	{
	}



