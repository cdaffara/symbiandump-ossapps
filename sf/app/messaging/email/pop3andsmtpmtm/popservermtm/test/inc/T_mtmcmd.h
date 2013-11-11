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
// Generic MTM Test Harness
// 
//

class CMtmTestCommand;
class CMtmTestHarness : public CActive
	{
public:
	static CMtmTestHarness* NewL(CBaseServerMtm& aServerMtm, CTestUtils& aTestUtils);
	
	// Test harness set up
	void Reset();
	void AddCommandL(CMtmTestCommand* aCommand);
	TInt RunError(TInt aError);

	// Run the test harness
	void StartL(const TDesC& aTestName, TRequestStatus& aStatus);	// async
	TInt StartL(const TDesC& aTestName);							// sync

	// Status information
	const TDesC8& Progress();
	CBaseServerMtm&	ServerMtm();
	TInt CommandError() const {return iCommandError;};

	// Constructor & destructor
	CMtmTestHarness(CBaseServerMtm& aServerMtm, CTestUtils& aTestUtils);
	~CMtmTestHarness();
	void ConstructL();

	void DoCancel();
	void RunL();

private:
	void RunCurrentCommandL();

private:
	CBaseServerMtm&						iServerMtm;
	CArrayFixFlat<CMtmTestCommand*>*	iCommandList;
	CMtmTestCommand*					iCurrentCommand;
	TInt								iCommandIndex;
	TRequestStatus*						iReportStatus;
	TBool								iFailed;
	CTestUtils&							iTestUtils;	
	TInt								iCommandError;
	};

//

class MCommandTester
// This class can be used to check the state of the MTM before, during
// and after a command has been issued.
// In can be used to check that the message tree is as expected, or
// could be combined with a timer to check progress as an operation is
// being run.
	{
public:
	virtual void InitialiseL() = 0;
	// Called just before the command is started.

	virtual void StartL() = 0;
	// Called just after the command has been started.

	virtual TBool EndL() = 0; 
	// Called after the MTM command has completed.
	// Returns ETrue if the results were as expected, otherwise return EFalse.
	
	virtual HBufC* TextInfoL() = 0;
	// Returns some text to describe what (if anything) has failed.
	};

//

class CMtmTestCommand : public CActive
	{
public:
	// Run this command
	virtual void ExecuteCommandL(TRequestStatus& aStatus) = 0;
	TPtrC DetailsL();

	void Queue(TRequestStatus& aReportStatus);

	void DoCancel();
	void RunL();
	TInt RunError(TInt aError);

	virtual ~CMtmTestCommand();
	void CopySelectionL(const CMsvEntrySelection* aSelection);

	void SetCommandTester(MCommandTester* aTester);
	MCommandTester* Tester();

protected:
	virtual HBufC* CommandDetailsL() = 0;
	CMtmTestCommand(CBaseServerMtm& aServerMtm);

public:

protected:
	CMsvEntrySelection* iSelection;
	TRequestStatus*		iReportStatus;
	TInt				iError;
	CBaseServerMtm&		iServerMtm;
	MCommandTester*		iTester;
	HBufC*				iDetails;
	};

//

class CMtmTestChange : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestChange* NewL(CBaseServerMtm& aServerMtm, TMsvEntry aNewEntry);
	CMtmTestChange(CBaseServerMtm& aServerMtm, TMsvEntry aNewEntry);

private:
	TMsvEntry iNewEntry;
	};

//

class CMtmTestDeleteAll : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestDeleteAll* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection);
	CMtmTestDeleteAll(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection);
	};

//

class CMtmTestCopyToLocal : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCopyToLocal* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);
	CMtmTestCopyToLocal(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);

private:
	TMsvId iDest;
	};

//

class CMtmTestCopyFromLocal : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCopyFromLocal* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);
	CMtmTestCopyFromLocal(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);

private:
	TMsvId iDest;
	};

//

class CMtmTestTopPopulate : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestTopPopulate* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);
	CMtmTestTopPopulate(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);

private:
	TMsvId iDest;
	};

//

class CMtmTestCopyWithinService : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCopyWithinService* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);
	CMtmTestCopyWithinService(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);

private:
	TMsvId iDest;
	};

//

class CMtmTestMoveToLocal : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestMoveToLocal* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);
	CMtmTestMoveToLocal(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);

private:
	TMsvId iDest;
	};

//

class CMtmTestMoveFromLocal : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestMoveFromLocal* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);
	CMtmTestMoveFromLocal(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);

private:
	TMsvId iDest;
	};

//

class CMtmTestMoveWithinService : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestMoveWithinService* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);
	CMtmTestMoveWithinService(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TMsvId aDest);

private:
	TMsvId iDest;
	};

//

class CMtmTestStartCommand : public CMtmTestCommand
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestStartCommand* NewL(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TInt aCommandId, TDesC8& aParamter);
	~CMtmTestStartCommand();
	CMtmTestStartCommand(CBaseServerMtm& aServerMtm, const CMsvEntrySelection* aSelection, TInt aCommandId, TDesC8& aParamter);

private:
	TInt iCommandId;
	HBufC8* iParameter;
	};

//

class CMtmTestCommandOnAllMessages : public CMtmTestCommand
// Runs a command on all messages directly under a given message entry.
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCommandOnAllMessages* NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry);
	~CMtmTestCommandOnAllMessages();
	CMtmTestCommandOnAllMessages(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry);

private:
	CMtmTestCommand* iRealCommand;
	TMsvId iRootId;
	CMsvServerEntry& iServerEntry;
	};

//

class CMtmTestCommandOnSecondMessage : public CMtmTestCommand
// Runs a command on the second message directly under a given message entry.
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCommandOnSecondMessage* NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry);
	~CMtmTestCommandOnSecondMessage();
	CMtmTestCommandOnSecondMessage(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry);

private:
	CMtmTestCommand* iRealCommand;
	TMsvId iRootId;
	CMsvServerEntry& iServerEntry;
	};

//

class CMtmTestCommandOnChildrenOf2ndMsg : public CMtmTestCommand
// Runs a command on the children of the second message directly under a given message entry.
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.	
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCommandOnChildrenOf2ndMsg* NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry);
	~CMtmTestCommandOnChildrenOf2ndMsg();
	CMtmTestCommandOnChildrenOf2ndMsg(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry);

private:
	CMtmTestCommand* iRealCommand;
	TMsvId iRootId;
	CMsvServerEntry& iServerEntry;
	};

//

class CMtmTestCommandOnSpecifiedMessages : public CMtmTestCommand
// Runs a command on the specified messages directly under a given message entry.
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCommandOnSpecifiedMessages* NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry, CMsvEntrySelection* aSpecifiedEntries);
	~CMtmTestCommandOnSpecifiedMessages();
	CMtmTestCommandOnSpecifiedMessages(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry, CMsvEntrySelection* aSpecifiedEntries);

private:
	CMtmTestCommand* iRealCommand;
	TMsvId iRootId;
	CMsvServerEntry& iServerEntry;
	CMsvEntrySelection* iSpecifiedEntries;
	};

//

class CMtmTestCommandOnSingleMessage : public CMtmTestCommand
// Runs a command on the specified message passed in the constructor
// This command object takes ownership of the given command.
// The given command should have all of it's parameters set before
// this command is created, with the exception of it's selection
// parameter.
	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCommandOnSingleMessage* NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry, TMsvId aSpecifiedEntry);
	~CMtmTestCommandOnSingleMessage();
	CMtmTestCommandOnSingleMessage(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TMsvId aRootId, CMsvServerEntry& aEntry, TMsvId aSpecifiedEntry);

private:
	CMtmTestCommand* iRealCommand;
	TMsvId iRootId;
	CMsvServerEntry& iServerEntry;
	TMsvId iSpecifiedEntry;
	};
/*
class CMtmTestCommandOnSingleMessage : public CMtmTestCommand

	{
public:
	void ExecuteCommandL(TRequestStatus& aStatus);
	HBufC* CommandDetailsL();
	static CMtmTestCommandOnSingleMessage* NewL(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TUint aSpecificEntry);
	~CMtmTestCommandOnSingleMessage();
	CMtmTestCommandOnSingleMessage(CBaseServerMtm& aServerMtm, CMtmTestCommand* aCommand, TUint	aSpecificEntry);

private:
	CMtmTestCommand* iRealCommand;
	TUint iSpecifiedEntry;
	};
*/
//
