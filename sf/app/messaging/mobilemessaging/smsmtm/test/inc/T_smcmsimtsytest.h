// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "smstestbase.h"

class CAutoTest;

class CMessageSendStates : public CBase
	{
	public:
		static CMessageSendStates* NewLC(TMsvId aId);
		static CMessageSendStates* NewL(TMsvId aId);
		~CMessageSendStates();

	private:
		CMessageSendStates(TMsvId aId);
		void ConstructL();

	public:
		TMsvId iId;
		RArray<TInt> iStates;
	};

class RMessageSendStatesArray : public RPointerArray<CMessageSendStates>
	{
	public:
		RMessageSendStatesArray();
		TInt Find(TMsvId aId) const;
		void CopyInSelectionL(const CMsvEntrySelection& aSelection);
	};


class CSmcmSendTest : public CActive, MMsvSessionObserver
	{
public:
	static CSmcmSendTest* NewLC(CAutoTest& aAutoTest, CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt aExpectedError = KErrNone, TInt aPriority = EPriorityStandard);
	~CSmcmSendTest();

	void TestSendingL(TRequestStatus& aStatus);
	void TestSchedulingL(TRequestStatus& aStatus);
	void TestReceiveClass2L(TInt aTestNumber, TRequestStatus& aStatus);

private:
	CSmcmSendTest(CAutoTest& aAutoTest, CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt aExpectedError, TInt aPriority = EPriorityStandard);
	void ConstructL();

	void RunL();
	void DoCancel();
	
	void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

	void DoRunSendingL();
	void DoRunSchedulingL();

	CMsvSession& Session()	{return *iSmsTest.iMsvSession;}
	CMsvEntry& MsvEntry()	{return *iSmsTest.iMsvEntry;}
	CSmsClientMtm& Client()	{return *iSmsTest.iSmsClientMtm;}

	void Queue(TRequestStatus& aStatus);
	void Complete(TInt aError);
	TInt RunError(TInt aError);

	TBool CheckSendingStates(const RMessageSendStatesArray& sendStateArray);
	void PrintSendingStates(const RMessageSendStatesArray& sendStateArray);

	TBool CheckParentOfReceivedMessageL(const CMsvEntrySelection& aSelection);
	void SetMessagesToReceiveL(TInt aTestNumber);

	TBool CheckWatchersStartedL();

	enum TSmsSendTestState
		{
		EStateWaiting,
		EStateSending,
		EStateScheduling,
		EStateSchedulingThenSend,
		EStateOther,
		EStateTestReceiveClass2
		} iState;

	TRequestStatus* iReport;					//< A request status
	CAutoTest& iAutoTest;						//< An auto test

	CMsvOperation* iOperation;
	CMsvEntrySelection* iSelection;
	CTestTimer* iTimer;

	TFileName iScriptFile;
	CSmsTestUtils& iSmsTest;

	RMessageSendStatesArray iCurrentMessages;
	TInt iMessagesToReceive;
	TInt iMessagesReceived;
	CSmsSettings* iSmsSettings;
	CTestUtilsWatcherStarter* iWatcherStarter;
	TInt iExpectedError;
	};

