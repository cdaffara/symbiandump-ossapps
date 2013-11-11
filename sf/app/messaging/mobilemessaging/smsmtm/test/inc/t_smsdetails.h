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
//

#ifndef __T_SMSDETAILS_H__
#define __T_SMSDETAILS_H__

#include <e32base.h>
#include <msvapi.h>
#include <es_sock.h>
//#include <logwrap.h>

class CSmsTestUtils;
class CSmsClientMtm;
class CSmsSettings;
class CSmsHeader;
class CTestUtilsWatcherStarter;
class CTestDetailsBase;

class MTestObserver
	{
public:

	virtual void TestFailed() =0;
	virtual void TestPassed() =0;
	
	};

class CTestEngine : public CBase,
					public MTestObserver
					
	{
public:		// methods

	static CTestEngine* NewL();
	virtual ~CTestEngine();
	void DoTestsL();
	
private:	// methods from MTestObserver

	virtual void TestFailed();
	virtual void TestPassed();
	
private:	// methods

	CTestEngine();
	void ConstructL();
	
	void RunTestCaseL(CTestDetailsBase& aTestCase);
	
	TBool WatchersAlreadyRunningL();
	void StartWatcherL();

private:	// attributes

	CSmsTestUtils*				iTestUtils;
	CTestUtilsWatcherStarter*	iWatchers;
	RSocketServ 	iSocketServ;
	RSocket 		iSocket;
	
	TInt			iTestFailCount;
	};
	
class CTestDetailsBase : public CActive,
						 public MMsvEntryObserver
	{
public:

	virtual ~CTestDetailsBase();
	void Start();
	
private:	// methods from CActive

	virtual void RunL();
	virtual void DoCancel();
	virtual TInt RunError(TInt aError);
	
private:	// methods from MMsvEntryObserver

	virtual void HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);

protected:

	CTestDetailsBase(CSmsTestUtils& aTestUtils, MTestObserver& aObserver);
	void ConstructL();

	void CompleteSelf();
	void ClearFoldersL();
	void Test(TBool aTest);
	void CreateMessageL();
	void SendMessageL();
	
	CMsvSession& Session();
	CMsvEntry& MsvEntry();
	CSmsClientMtm& Client();
	CSmsSettings& ServiceSettings();
	
protected:

	CSmsTestUtils&	iTestUtils;
	MTestObserver&	iObserver;
	CMsvEntry*		iEntryForObserver;
	CMsvOperation*	iOperation;
	TMsvId			iMessageId;

private:

	void ChangeServiceSettingsL();
	void SetRecipientsL(CSmsHeader& aHeader);

	virtual void CheckReceivedMessageL(TMsvId aMessageId) =0;
	virtual const TDesC& TestName() =0;

private:

	enum TSendTestState
		{
		EClearFolders,
		EChangeServiceSettings,
		ECreateMessage,
		ESendMessage,
		EMessageSent,
		EPendingReceivedMessage,
		EDone
		};			
	
private:
	
	TSendTestState	iState;
	};
	
class CTestReceiveDetails_1 : public CTestDetailsBase
	{
public:

	static CTestReceiveDetails_1* NewL(CSmsTestUtils& aTestUtils, MTestObserver& aObserver);
	virtual ~CTestReceiveDetails_1();
	
private:	// methods from CTestSendBase

	virtual void CheckReceivedMessageL(TMsvId aMessageId);
	virtual const TDesC& TestName();

private:
	
	CTestReceiveDetails_1(CSmsTestUtils& aTestUtils, MTestObserver& aObserver);

	};
	
class CTestReceiveDetails_2 : public CTestDetailsBase
	{
public:

	static CTestReceiveDetails_2* NewL(CSmsTestUtils& aTestUtils, MTestObserver& aObserver);
	virtual ~CTestReceiveDetails_2();
	
private:	// methods from CTestSendBase

	virtual void CheckReceivedMessageL(TMsvId aMessageId);
	virtual const TDesC& TestName();

private:
	
	CTestReceiveDetails_2(CSmsTestUtils& aTestUtils, MTestObserver& aObserver);
	void ConstructL();

	};
	
class CTestReceiveDetails_3 : public CTestDetailsBase
	{
public:

	static CTestReceiveDetails_3* NewL(CSmsTestUtils& aTestUtils, MTestObserver& aObserver);
	virtual ~CTestReceiveDetails_3();
	
private:	// methods from CTestSendBase

	virtual void CheckReceivedMessageL(TMsvId aMessageId);
	virtual const TDesC& TestName();

private:
	
	CTestReceiveDetails_3(CSmsTestUtils& aTestUtils, MTestObserver& aObserver);
	void ConstructL();
	
	};
	
#endif
