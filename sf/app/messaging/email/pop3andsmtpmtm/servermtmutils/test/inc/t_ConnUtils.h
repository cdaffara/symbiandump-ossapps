// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <eikenv.h> 

// =============================================== //
class CSendPopCommandMessage;

enum TTestMode{
	  ESingleDefaultConn = 0,
	  ESingleSpecifiedConn,
	  ETwoDefaultSerialConn,
	  ETwoDefaultParallelConn,
	  ETwoDifferentSerialConn,
	  ETwoDifferentParallelConn,
	  EExplicitDefaultConn
	  };

// Parent active object which controls the children the sequence of 
// Connect, Send and count messages and Disconnect.
class CTestHarnessParent : public CActive
	{
public:
	static CTestHarnessParent* NewL();
	static CTestHarnessParent* NewLC();
	void ChildFinished();
	CSendPopCommandMessage* MakeBasicConnectionLC(TDes& aServAddress, 
												TDes8& aUser, TDes8& aPass);
	CSendPopCommandMessage* MakeOverrideConnectionLC(TDes& aServAddress,
												TDes8& aUser, TDes8& aPass);
	CSendPopCommandMessage* MakeExplicitDefaultConnectionLC(TDes& aServAddress, 
												TDes8& aUser, TDes8& aPass);
	void TestSingleDefaultConnL();
	void TestSingleSpecifiedConnL();
	void TestTwoDefaultConnectionsSeriallyL();
	void TestTwoDefaultConnectionsParallellyL();
	void TestTwoDifferentConnectionsSeriallyL();
	void TestTwoDifferentConnectionsParallellyL();
	void TestExplicitDefaultConnL();
	void SetTestMode(TTestMode aTestMode);
private:
	CTestHarnessParent();
	~CTestHarnessParent();
	void ConstructL();
	void RunL();
	void DoCancel();
	void SingleDefaultConnection();
	void SingleSpecifiedConnection();
	void TwoSerialConnectionsL();
	void TwoParallelConnections();
	typedef enum TState{
	  EConnect = 0,
	  ESend,
	  EDisConnect,
	  EConnect2,
	  ESend2,
	  EDisConnect2,
	  EStop
	  };
	TState					iState;		
	CSendPopCommandMessage	*iPopCmdServiceA;
	CSendPopCommandMessage	*iPopCmdServiceB;
	TInt					iConnectionType;
	TTestMode				iTestMode;

	};

// =============================================== //

// Child active object whose instances will be controled by parent for 
// Connect,Send and count messages and Disconnect

class CSendPopCommandMessage : public CActive
	{
public: // functions                                 
	// construct/destruct
	static CSendPopCommandMessage *NewL(TDesC& aServerAddress, 
							TInt aPort, 
							TDesC8& aUser,
							TDesC8& aPass,
							CTestHarnessParent* aParent,
							CImIAPPreferences* aConnPref = NULL);

	static CSendPopCommandMessage *NewLC(TDesC& aServerAddress, 
							TInt aPort, 
							TDesC8& aUser,
							TDesC8& aPass,
							CTestHarnessParent* aParent,
							CImIAPPreferences* aConnPrefaConnPref = NULL);

	// issue request
	void RequestSend();
	void SetConnectionPrefs(CImIAPPreferences* aConnPref);
	void Complete(TInt aError);
	TInt RunError(TInt aError);
	void ConnectionL();
	void List();
	TBool IsChildCompleteSet();
	~CSendPopCommandMessage();
	void DisConnect();
	void SetLastError(TInt aError);
	TInt GetLastError();
private:
	void RunL();
	void QueueNextReceive();
	void ConstructL();
	void SetSelfComplete(TBool aComplete);

	CSendPopCommandMessage();
	CSendPopCommandMessage(TDesC& aServerAddress, 
							TInt aPort, 
							TDesC8& aUser,
							TDesC8& aPass,
							CTestHarnessParent* aParent,
							CImIAPPreferences* aConnPref);

	void Quit();
	void DisplayErrorReason(TInt& aReason);
	void DoCancel();
	typedef enum TSendState{
	  EConnect = 0,
	  EUserName,
	  EPassword,
	  EStat,
	  EList,
	  EQuit,
	  EComplete,
	  EConnectDone,
	  EQuitDone
	};
	CImTextServerSession*	iImSocket;
	TSendState				iSendState;
	TStreamId				iId;
	HBufC*					iBuf;
	TInt					iCurrentState;
	TInt					iLineStatus;
	TBool					iReceive;
	TSockXfrLength			iLen;
	TDesC&					iServerAddress;
	TInt					iPort;
	TDesC8&					iUserName;
	TDesC8&					iPass;
	CTestHarnessParent*		iParent;
	CImIAPPreferences*		iIAPPreferences;
	TInt					iLastError;
	TBool					iComplete;
	};

//====================================//

// Test harness calss which installs the scheduler and loads the logical 
// and physical device

class  CTestHarnessUtil : public CBase
	{
public:
	static CTestHarnessUtil* NewL();
	~CTestHarnessUtil();
private:
	void ConstructL();
	CTestHarnessUtil();
	void InitSchedulerL();
	TInt CommInit();
private:
	RFs					iFs;
	CActiveScheduler	*iScheduler;
	};

//====================================//



