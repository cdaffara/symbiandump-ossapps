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
// Name of the test harness : T_MSGCONNTEST
// Component: IMUT
// Owner: 
// Brief description of test harness:
// Tests the changes IMUT for multihoming which allows more than one connection 
// Detailed description of test harness:
// Implements a range of tests for IMUT testing the following
// funtionality:
// -Single default connection
// -Single specified connection
// -Two Default connections serially (both the connections being default) 
// -Two Default connections parallelly
// -Two Different connections serially(one Default and the other Specified)
// -Two Different connections parallelly
// -Two Connection , one default connection and another explicit connection
// to the default IAP.	
// Input files required to run test harness:
// Intermediate files produced while running test harness:	
// Output files produced by running test harness:
// \msglogs\T_MSGCONNTEST.WINS.DEB
// Description of how to build test harness:
// -Get the 3GComms code from the directory ---TYPHOON CODE-----  and build it
// -Get the messagiing code from //EPOC/development/messaging/3gcomm/msg/ (Perforce database)
// and build it.
// Description of how to run test harness:
// -Navigate to the directory \epoc32\release\wins\udeb from the command prompt
// -Run T_MsgConnTest.exe from this directory.
// -To view the test results check the file \epoc32\wins\c\MsgLogs\T_MSGCONNTEST.WINS.DEB
// 
//

#include <nd_err.h>
#include <imsk.h>
#include <iapprefs.h>
#include "t_ConnUtils.h"
#include "emailtestutils.h"

#define EActivePriorityLow		-1

#define ETestSingleDefaultConn '1'
#define ETestSingleSpecifiedConn '2'
#define ETestTwoDefaultSerialConns '3'
#define ETestTwoDefaultParallelConns '4'
#define ETestTwoDifferentSerialConns '5'
#define ETestTwoDifferentParallelConns '6'
#define ETestExplicitDefaultConn '7'
#define ETestAllCases 'a'

const TInt KMaxSizeOfBinaryData = 200;

LOCAL_D RTest g_Test(_L("T_MSG_CONN CImTextServerSession Test"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* gTestUtils; 
LOCAL_D TBuf8<KMaxSizeOfBinaryData> someBinaryData=_L8("");

LOCAL_D TBuf<30> KPipexServer =_L("pop.dial.pipex.com");
LOCAL_D TBuf8<30> KPipexUser= _L8("USER abf75\r\n");
LOCAL_D TBuf8<30> KPipexPass= _L8("PASS oodifama\r\n");
LOCAL_D TInt KPopPort=110;
// Can use either the demon server or the fastmail.fm account
LOCAL_D TBuf<16> KDemonServer = _L("194.217.242.59");//_L("209.61.183.86");//
LOCAL_D TBuf8<30> KDemonUser = _L8("USER psion8\r\n");//_L8("USER catherine2k\r\n");//
LOCAL_D TBuf8<30> KDemonPass = _L8("PASS Bag8Nut\r\n");//_L8("PASS junkjunk\r\n");//

LOCAL_D TInt g_NoOfConnections;
LOCAL_D TInt g_Count;

//=======================================//

#if defined (__WINS__)
#define PDD_NAME	_L("ECDRV")
#define LDD_NAME	_L("ECOMM")
#define LDD_FNAME	_L("ECOMM")
#else
#define PDD_NAME	_L("EUART1")
#define PDD2_NAME	 _L("EUART2")
#define LDD_NAME	_L("ECOMM")
#define LDD_FNAME	_L("FCOMM")
#endif

TInt CTestHarnessUtil::CommInit()
	{
	TInt err=User::LoadPhysicalDevice(PDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		return err;

	err=User::LoadLogicalDevice(LDD_NAME);
	if (err!=KErrNone && err!=KErrAlreadyExists)
		return err;

	return KErrNone;
	}

// Installs the scheduler
void CTestHarnessUtil::InitSchedulerL()
	{
	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install( iScheduler );
	}

CTestHarnessUtil* CTestHarnessUtil::NewL()
	{
	CTestHarnessUtil* me = new (ELeave) CTestHarnessUtil();
	CleanupStack::PushL(me);
	me->ConstructL();
	CleanupStack::Pop(me);
	return me;
	}

void CTestHarnessUtil::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iFs.SetSessionPath(_L("C:\\"));
	iFs.SetSessionPath(_L("c:\\"));
	iFs.MkDir(_L("c:\\logs\\"));
	iFs.MkDir(_L("c:\\logs\\email\\"));

	InitSchedulerL();
	CommInit();
	}

CTestHarnessUtil::CTestHarnessUtil()
	{
	}

CTestHarnessUtil::~CTestHarnessUtil()
	{
	delete iScheduler;
	iFs.Close();
	}


//=====================================//
//CTestHarnessParent

CTestHarnessParent::CTestHarnessParent() // construct high-priority active object
	: CActive(EPriorityHigh)
	{
	}

CTestHarnessParent  *CTestHarnessParent::NewLC()
	{
	CTestHarnessParent* self=new (ELeave) CTestHarnessParent();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CTestHarnessParent  *CTestHarnessParent::NewL()
	{
	CTestHarnessParent* self=NewLC();
	CleanupStack::Pop();
	return self;
	}

void CTestHarnessParent::ConstructL()
	{
	iState=EConnect;
	iPopCmdServiceA=NULL;
	iPopCmdServiceB=NULL;

	CActiveScheduler::Add(this); // add to active scheduler
	}

CTestHarnessParent::~CTestHarnessParent()
	{ 
	Cancel(); // make sure we're cancelled
	if(iPopCmdServiceA!=NULL)
		{
		delete iPopCmdServiceA;
		iPopCmdServiceA=NULL;
		}

	if(iPopCmdServiceB!=NULL)
		{
		delete iPopCmdServiceB;
		iPopCmdServiceB=NULL;
		}	
	}

void CTestHarnessParent::TestSingleDefaultConnL()
	{
	gTestUtils->WriteComment(_L("Test Single Default Connection Started"));
	iPopCmdServiceA=MakeBasicConnectionLC(KPipexServer, 
												KPipexUser, 
												KPipexPass);
	iPopCmdServiceA->ConnectionL();
	iState = ESend;
	iStatus = KRequestPending;
	SetActive();
	CActiveScheduler::Start();

	gTestUtils->WriteComment(_L("End of Single Default Connection"));
	gTestUtils->WriteComment(_L("\r\n"));
	}	

void CTestHarnessParent::TestSingleSpecifiedConnL()
	{
	gTestUtils->WriteComment(_L("Test Single Specified Connection Started"));
	iPopCmdServiceA=MakeOverrideConnectionLC(KDemonServer, 
												KDemonUser, 
												KDemonPass);
	
	iPopCmdServiceA->ConnectionL();
	iState = ESend;
	iStatus = KRequestPending;
	SetActive();
	CActiveScheduler::Start();

	gTestUtils->WriteComment(_L("End of Single Specified Connection"));
	gTestUtils->WriteComment(_L("\r\n"));
	}	

void CTestHarnessParent::TestTwoDefaultConnectionsSeriallyL()
	{
	gTestUtils->WriteComment(_L("Test Two Default Serial Connections Started"));

	iPopCmdServiceA=MakeBasicConnectionLC(KPipexServer, 
												KPipexUser, 
												KPipexPass);

	iPopCmdServiceB=MakeBasicConnectionLC(KDemonServer, 
												KDemonUser, 
												KDemonPass);
	iPopCmdServiceA->ConnectionL();
	iState = ESend;
	iStatus = KRequestPending;
	SetActive();
	CActiveScheduler::Start();
	
	gTestUtils->WriteComment(_L("End of Test Two Default Serial Connections "));
	gTestUtils->WriteComment(_L("\r\n"));
	}

void  CTestHarnessParent::TestTwoDefaultConnectionsParallellyL()
	{
	gTestUtils->WriteComment(_L("Test Two Default Parallel Connections Started"));
	gTestUtils->WriteComment(_L("\nFirst Connection Start"));
	iPopCmdServiceA=MakeBasicConnectionLC(KPipexServer, 
												KPipexUser, 
												KPipexPass);
	iPopCmdServiceA->ConnectionL();

	gTestUtils->WriteComment(_L("\nSecond Connection Start"));
	iPopCmdServiceB=MakeBasicConnectionLC(KDemonServer, 
												KDemonUser, 
												KDemonPass);
	iPopCmdServiceB->ConnectionL();

	iState = ESend;
	iStatus = KRequestPending;
	SetActive();
	CActiveScheduler::Start();

	gTestUtils->WriteComment(_L("End of Test Two Default Parallel Connections "));
	gTestUtils->WriteComment(_L("\r\n"));
	}

void CTestHarnessParent::TestTwoDifferentConnectionsSeriallyL()
	{
	gTestUtils->WriteComment(_L("Test Two Different Serial Connections Started"));
	gTestUtils->WriteComment(_L("\nFirst Connection Start"));

	iPopCmdServiceA=MakeBasicConnectionLC(KPipexServer, 
												KPipexUser, 
												KPipexPass);

	iPopCmdServiceB=MakeOverrideConnectionLC(KDemonServer, 
												KDemonUser, 
												KDemonPass);
	iPopCmdServiceA->ConnectionL();
	iState = ESend;
	iStatus = KRequestPending;
	SetActive();
	CActiveScheduler::Start();
	
	gTestUtils->WriteComment(_L("End of Test Two Different Serial Connections "));
	gTestUtils->WriteComment(_L("\r\n"));

	}

void  CTestHarnessParent::TestTwoDifferentConnectionsParallellyL()
	{
	gTestUtils->WriteComment(_L("Test Two Differenet Parallel Connections Started"));
	gTestUtils->WriteComment(_L("\nFirst Connection Start"));
	
	iPopCmdServiceA=MakeBasicConnectionLC(KPipexServer, 
												KPipexUser, 
												KPipexPass);
	iPopCmdServiceA->ConnectionL();

	gTestUtils->WriteComment(_L("\nSecond Connection Start"));
	iPopCmdServiceB=MakeOverrideConnectionLC(KDemonServer, 
												KDemonUser, 
												KDemonPass);
	iPopCmdServiceB->ConnectionL();

	iState = ESend;
	iStatus = KRequestPending;
	SetActive();
	CActiveScheduler::Start();

	gTestUtils->WriteComment(_L("End of Test Two Different Parallel Connections "));
	gTestUtils->WriteComment(_L("\r\n"));
	}


void CTestHarnessParent::TestExplicitDefaultConnL()
	{
	gTestUtils->WriteComment(_L("Test Explicit-Default Parallel Connections Started"));
	gTestUtils->WriteComment(_L("\nFirst Connection Start"));

	iPopCmdServiceA=MakeBasicConnectionLC(KPipexServer, 
												KPipexUser, 
												KPipexPass);
	iPopCmdServiceA->ConnectionL();

	gTestUtils->WriteComment(_L("\nSecond Connection Start"));
	iPopCmdServiceB=MakeExplicitDefaultConnectionLC(KDemonServer, 
												KDemonUser, 
												KDemonPass);
	iPopCmdServiceB->ConnectionL();

	iState = ESend;
	iStatus = KRequestPending;
	SetActive();
	CActiveScheduler::Start();

	gTestUtils->WriteComment(_L("End of Test Explicit-Default Parallel Connections"));
	gTestUtils->WriteComment(_L("\r\n"));
	}

void CTestHarnessParent::DoCancel()
	{
	}

void CTestHarnessParent::SetTestMode(TTestMode aTestMode)
	{
	iTestMode=aTestMode;
	}

// Called by active scheduler if there is an outstanding task
void CTestHarnessParent::RunL()
	{
	switch(iTestMode)
		{
		case ESingleDefaultConn:
			SingleDefaultConnection();
		break;
		
		case ESingleSpecifiedConn:
			SingleDefaultConnection();
		break;
		
		case ETwoDefaultSerialConn: 
		case ETwoDifferentSerialConn:
			TwoSerialConnectionsL();
		break;
	
		case ETwoDefaultParallelConn:
		case ETwoDifferentParallelConn:
		case EExplicitDefaultConn:
			TwoParallelConnections();
		break;
		default:
		break;
		}
	}

void CTestHarnessParent::TwoSerialConnectionsL()
	{
	switch (iState)
		{
		case EConnect:
		break;
		case ESend:	
			g_Count=g_NoOfConnections;
			iState=EDisConnect;
			iStatus = KRequestPending;
			SetActive();

			if(!iPopCmdServiceA->IsChildCompleteSet())
				{
				if(iPopCmdServiceA->iStatus.Int() == KErrNone || 
							iPopCmdServiceA->iStatus.Int() == KErrInUse)
					{
					iPopCmdServiceA->List();
					}
				}
			else
				ChildFinished();
		break;
		case EDisConnect:
			g_Count=g_NoOfConnections;
			iState=EConnect2;
			iStatus = KRequestPending;
			SetActive();
			if(!iPopCmdServiceA->IsChildCompleteSet())
				{
				iPopCmdServiceA->DisConnect();
				}
			else
				ChildFinished();
		break;
		case EConnect2:
			g_Count=g_NoOfConnections;
			iPopCmdServiceB->ConnectionL();
			iState=ESend2;
			iStatus = KRequestPending;
			SetActive();
		break;
		case ESend2:
			g_Count=g_NoOfConnections;
			iState=EDisConnect2;
			iStatus = KRequestPending;
			SetActive();

			if(!iPopCmdServiceB->IsChildCompleteSet())
				{
				if(iPopCmdServiceB->iStatus.Int() == KErrNone || 
							iPopCmdServiceB->iStatus.Int() == KErrInUse)
					{
					iPopCmdServiceB->List();
					}
				}
			else
				ChildFinished();
			break;
		case EDisConnect2:
			g_Count=g_NoOfConnections;
			iState=EStop;
			iStatus = KRequestPending;
			SetActive();

			if(!iPopCmdServiceB->IsChildCompleteSet())
				{
				iPopCmdServiceB->DisConnect();
				}
			else
				ChildFinished();
		break;
		case EStop:
			CActiveScheduler::Stop();
		break;
		default:
		break;
		}
	}


void CTestHarnessParent::TwoParallelConnections()
	{
	switch (iState)
		{
		case EConnect:
		break;
		case ESend:	// have set encryption
			g_Count=g_NoOfConnections;
			iState=EDisConnect;
			iStatus = KRequestPending;
			SetActive();

			if(!iPopCmdServiceA->IsChildCompleteSet())
				{
				if(iPopCmdServiceA->iStatus.Int() == KErrNone || 
						iPopCmdServiceA->iStatus.Int() == KErrInUse)
					{
					iPopCmdServiceA->List();
					}
				}
			else
				ChildFinished();
					
			if(!iPopCmdServiceB->IsChildCompleteSet())
				{
				if(iPopCmdServiceB->iStatus.Int() == KErrNone || 
						iPopCmdServiceB->iStatus.Int() == KErrInUse)
					{
					iPopCmdServiceB->List();
					}
				}
			else
				ChildFinished();
		break;
		case EDisConnect:
			g_Count=g_NoOfConnections;
			iState=EStop;
			iStatus = KRequestPending;
			SetActive();

			if(!iPopCmdServiceA->IsChildCompleteSet())
				iPopCmdServiceA->DisConnect();
			else
				ChildFinished();

			if(!iPopCmdServiceB->IsChildCompleteSet())
				iPopCmdServiceB->DisConnect();
			else
				ChildFinished();
		break;
		case EStop:
			CActiveScheduler::Stop();
		break;
		default:
		break;
		} // end of switch
	}

void CTestHarnessParent::SingleDefaultConnection()
	{
	switch (iState)
		{
		case EConnect:
		break;
		case ESend:	
			g_Count=g_NoOfConnections;
			iState=EDisConnect;
			iStatus = KRequestPending;
			SetActive();
			if(!iPopCmdServiceA->IsChildCompleteSet())
				{
				if(iPopCmdServiceA->iStatus.Int() == KErrNone || 
						iPopCmdServiceA->iStatus.Int() == KErrInUse)
					{
					iPopCmdServiceA->List();
					}
				}
			else
				ChildFinished();
		break;
		case EDisConnect:
			g_Count=g_NoOfConnections;
			iState=EStop;
			iStatus = KRequestPending;
			SetActive();
			
			if(!iPopCmdServiceA->IsChildCompleteSet())
				{
				iPopCmdServiceA->DisConnect();
				}
			else
				ChildFinished();
		break;
		case EStop:
			CActiveScheduler::Stop();
		break;
		default:
		break;
		} // end of switch
	}

void CTestHarnessParent::ChildFinished() 
	{
	TRequestStatus *status;
	status = &iStatus;
	g_Count-- ;
	if(g_Count==0)
		User::RequestComplete(status,KErrNone);
	}

// Default connection
CSendPopCommandMessage* CTestHarnessParent::MakeBasicConnectionLC(TDes& aServAddress, TDes8& aUser, TDes8& aPass/*,CTestHarnessParent* aParent*/)
	{
	CSendPopCommandMessage* popCmdService = CSendPopCommandMessage::NewL(aServAddress,KPopPort, aUser, aPass,this);
	return popCmdService;
	}

// Override connection
CSendPopCommandMessage* CTestHarnessParent::MakeOverrideConnectionLC(TDes& aServAddress, TDes8& aUser, TDes8& aPass/*,CTestHarnessParent* aParent*/)
	{
	CSendPopCommandMessage* popCmdService = CSendPopCommandMessage::NewL(aServAddress,KPopPort, aUser, aPass, this);
	
	TImIAPChoice choice;
	choice.iIAP = 1; // Specified IAP 
	choice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	CImIAPPreferences*		theIAPPreferences = CImIAPPreferences::NewLC();
	theIAPPreferences->AddIAPL(choice);
	
	popCmdService->SetConnectionPrefs(theIAPPreferences);
	CleanupStack::Pop(theIAPPreferences);
	return popCmdService;
	}

// Explicit connection to the default IAP
CSendPopCommandMessage* CTestHarnessParent::MakeExplicitDefaultConnectionLC(TDes& aServAddress, TDes8& aUser, TDes8& aPass)
	{
	CSendPopCommandMessage* popCmdService = CSendPopCommandMessage::NewL(aServAddress,KPopPort, aUser, aPass, this);
	
	TImIAPChoice choice;
	choice.iIAP = 0;  // Explicitly connecting to the default IAP
	choice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	CImIAPPreferences*		theIAPPreferences = CImIAPPreferences::NewLC();
	theIAPPreferences->AddIAPL(choice);
	
	popCmdService->SetConnectionPrefs(theIAPPreferences);
	CleanupStack::Pop(theIAPPreferences);
	return popCmdService;
	}

// Displays the error code 
void CSendPopCommandMessage::DisplayErrorReason(TInt& aReason)
	{
	if (aReason!=KErrNone)
		{
		gTestUtils->WriteComment(_L("\nERROR - "));
		}

	TBuf<64> logString;
	switch (aReason)
		{
		case KErrNone:
			gTestUtils->WriteComment(_L("Session completed successfully"));
		break;
		case KErrCancel:
			gTestUtils->WriteComment(_L("Session cancelled"));
			logString.Format(_L(" Session Cancelled %d"),KErrCancel);
			gTestUtils->WriteComment(logString);
		break;
		case KErrAlreadyExists:
			gTestUtils->WriteComment(_L(" something Already Exists"));
			logString.Format(_L(" something Already Exists %d"),KErrAlreadyExists);
			gTestUtils->WriteComment(logString);
		break;
		case KErrEof:
			gTestUtils->WriteComment(_L("No messages to send"));
			logString.Format(_L(" No messages to send %d"),KErrEof);
			gTestUtils->WriteComment(logString);
		break;
		case KErrNoMemory:
			gTestUtils->WriteComment(_L("No memory"));
			logString.Format(_L(" No Memory %d"),KErrNoMemory);
			gTestUtils->WriteComment(logString);
		break;
		case KErrAccessDenied:
			gTestUtils->WriteComment(_L("Switch link off"));
			logString.Format(_L(" Switch link off %d"),KErrAccessDenied);
			gTestUtils->WriteComment(logString);
		break;
		case KErrExitNoModem:
			gTestUtils->WriteComment(_L("No modem"));
			logString.Format(_L(" No Modem %d"),KErrExitNoModem);
			gTestUtils->WriteComment(logString);
		break;
		case KErrExitLoginFail:
			gTestUtils->WriteComment(_L("Login failed"));
			logString.Format(_L(" Login Failed %d"),KErrExitLoginFail);
			gTestUtils->WriteComment(logString);
		break;
		case KErrExitModemError:
			gTestUtils->WriteComment(_L("Modem error"));
			logString.Format(_L(" Modem Error %d"),KErrExitModemError);
			gTestUtils->WriteComment(logString);
		break;
		case KErrExitScriptError:
			gTestUtils->WriteComment(_L("Script error"));
			logString.Format(_L(" Script Error %d"),KErrExitScriptError);
			gTestUtils->WriteComment(logString);
		break;
		case KImskErrorDNSNotFound:
			gTestUtils->WriteComment(_L("Failed to find SMTP server"));
			logString.Format(_L(" Failed to find SMTP server %d"),KImskErrorDNSNotFound);
			gTestUtils->WriteComment(logString);
		break;
		case KErrTimedOut:
			gTestUtils->WriteComment(_L("Timed out waiting to communicate to server"));
			logString.Format(_L(" Timed out waiting to communicate to server %d"),KErrTimedOut);
			gTestUtils->WriteComment(logString);
		break;
		case KErrDisconnected:
			gTestUtils->WriteComment(_L("Server disconnected from socket"));
			logString.Format(_L(" Server disconnected from socket %d"),KErrDisconnected);
			gTestUtils->WriteComment(logString);
		break;
		case KErrCouldNotConnect:
			gTestUtils->WriteComment(_L("Could not connect to the server"));
			logString.Format(_L(" Could not connect to the server %d"),KErrCouldNotConnect);
			gTestUtils->WriteComment(logString);
		break;
		case KErrCouldNotDisconnect:
			gTestUtils->WriteComment(_L("Could not disconnect from the server"));
			logString.Format(_L(" Could not disconnect from the server %d"),KErrCouldNotDisconnect);
			gTestUtils->WriteComment(logString);
		break;
		case KErrServerTerminated:
			gTestUtils->WriteComment(_L("Server terminated"));
			logString.Format(_L(" Server terminated %d"),KErrServerTerminated);
			gTestUtils->WriteComment(logString);
		break;
		default:
			// unknown error 
			logString.Format(_L(" Error %d"),aReason);
			gTestUtils->WriteComment(logString);
		}
	}


// create an active object to send the message

CSendPopCommandMessage::CSendPopCommandMessage() 
	: CActive(EPriorityHigh), 
	iServerAddress(KPipexServer), 
	iPort(KPopPort),
	iUserName(KPipexUser),
	iPass(KPipexPass)
	{
	}

CSendPopCommandMessage::CSendPopCommandMessage(TDesC& aServerAddress, 
											   TInt aPort, 
											   TDesC8& aUser,
											   TDesC8& aPass ,
											   CTestHarnessParent* aParent,
											   CImIAPPreferences* aConnPref)
	: CActive(EPriorityHigh), 
	iServerAddress(aServerAddress), 
	iPort(aPort),
	iUserName(aUser),
	iPass(aPass),
	iParent(aParent),
	iIAPPreferences(aConnPref) 

	{
	}

CSendPopCommandMessage  *CSendPopCommandMessage::NewLC(TDesC& aServerAddress, 
													   TInt aPort, 
													   TDesC8& aUser,
													   TDesC8& aPass,
													   CTestHarnessParent* aParent,
													   CImIAPPreferences* aConnPref)
	{
	CSendPopCommandMessage* self=new (ELeave) CSendPopCommandMessage(aServerAddress, 
																		aPort,
																		aUser,
																		aPass,
																		aParent,
																		aConnPref);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CSendPopCommandMessage  *CSendPopCommandMessage::NewL(TDesC& aServerAddress, 
													  TInt aPort, 
													  TDesC8& aUser,
													  TDesC8& aPass,
													  CTestHarnessParent* aParent,
													  CImIAPPreferences* aConnPref)
	{
	CSendPopCommandMessage* self=NewLC(aServerAddress, aPort, aUser, aPass, 
														aParent,aConnPref);
	CleanupStack::Pop();
	return self;
	}

void CSendPopCommandMessage::ConstructL()
	{
	iSendState=EConnect;
	CActiveScheduler::Add(this); // add to active scheduler
	iImSocket= CImTextServerSession::NewL();
	iReceive=EFalse;
	}

TBool CSendPopCommandMessage::IsChildCompleteSet()
	{
	return iComplete;	
	}

void CSendPopCommandMessage::SetConnectionPrefs(CImIAPPreferences* aConnPref)
	{
	if (iIAPPreferences)
		delete iIAPPreferences;
	iIAPPreferences = aConnPref;
	}

CSendPopCommandMessage::~CSendPopCommandMessage()
	{ 
	Cancel(); // make sure we're cancelled
	delete iImSocket;
	delete iBuf;
	delete iIAPPreferences;
	}

void CSendPopCommandMessage::SetSelfComplete(TBool aComplete)
	{
	iComplete=aComplete;
	}

void CSendPopCommandMessage::DoCancel()
	{
	iImSocket->Cancel();
	}

void CSendPopCommandMessage::SetLastError(TInt aError)
	{
	iLastError=aError;
	}

TInt CSendPopCommandMessage::GetLastError()
	{
	return iLastError;
	}

// Displays the error reason, disconnects the server connection and starts the active
void CSendPopCommandMessage::Complete(TInt aError)
	{
	iImSocket->LogError(_L8("\t\tEnd of POP3 g_Test session; completion code="),aError);
	TBuf8<KImMailMaxBufferSize> buffer;
	iLineStatus=iImSocket->GetCurrentTextLine(buffer);
	g_Test.Printf(TRefByValue<const TDesC>_L("Final %S\n "),&buffer);

	SetSelfComplete(ETrue);
	DisplayErrorReason(aError);
	if (aError==KErrNone)
		gTestUtils->WriteComment(_L("PASSED g_Test"));
	else
		gTestUtils->WriteComment(_L("FAILED g_Test"));
	}

// Called by active scheduler if there is an outstanding task
void  CSendPopCommandMessage::RunL()
	{
	switch (iSendState)
		{
		case EConnect:
			if(iStatus.Int()==KErrNone || iStatus.Int()==KErrInUse)
				{	
				iReceive=ETrue;
				QueueNextReceive();
				iSendState = EConnectDone;
				}
			else
				{
				SetLastError(iStatus.Int());
				DisConnect();
				}
		break;
		case EConnectDone:
			iParent->ChildFinished();
			iSendState = EUserName;
		break;
		case EStat:	// have set encryption
			iReceive=EFalse;
			RequestSend();
			iSendState=EList;
		break;
		case EUserName:	// response from sending user name
		case EPassword:	// response from sending password`
		case EList:
			List();
		break;
		case EQuit:	
			Quit();	
		break;
		case EQuitDone:
			iParent->ChildFinished();
		break;
		case EComplete:
			Complete(GetLastError());
			iParent->ChildFinished();
		break;
		default:
		break;
		} // end of switch 
	}

// Called by RunL when it leaves
TInt CSendPopCommandMessage::RunError(TInt aError)
	{
	Cancel(); // make sure we're cancelled
	delete iImSocket;
	delete iBuf;
	delete iIAPPreferences;

	if(aError != KErrNone)
		return aError;

	return KErrNone;
	}

// Disconect the server connection
void CSendPopCommandMessage::DisConnect()
	{
	iImSocket->Disconnect(iStatus);
	iSendState=EComplete;
	SetActive();
	}

// USER,PASS and LIST command sent to the server
void CSendPopCommandMessage::List()
	{
	someBinaryData=_L8("");
	if(iStatus.Int()!=KErrNone)
		{
		SetLastError(iStatus.Int());
		DisConnect();
		return;
		}
	else
		{
		TBuf8<40> buffer;
		TBuf<40> buffer16;
		iLineStatus=iImSocket->GetCurrentTextLine(buffer);
		buffer16.Copy(buffer);
		g_Test.Printf(TRefByValue<const TDesC>_L("%S"),&buffer16);
		if(iReceive)
			{
			if(iLineStatus==EBufferTooSmall)
				{
				while(iLineStatus!=ECRLFTerminated)
					{
					iLineStatus=iImSocket->GetCurrentTextLine(buffer);
					g_Test.Printf(TRefByValue<const TDesC>_L("%S"),&buffer);
					}
				}
			iReceive=EFalse;
			RequestSend();
			TInt iSt = (TInt) iSendState;
			iSt++;
			iSendState = (TSendState)iSt;
			}
		else
			{
			iReceive=ETrue;
			QueueNextReceive();
			}
		}
	}

// Receives the data from the server and sends the QUIT message
void CSendPopCommandMessage::Quit()
	{
	TInt length=0;
	if(iStatus.Int()!=KErrNone)
		{
		SetLastError(iStatus.Int());
		DisConnect();
		return;
		}
	length=someBinaryData.Length();
	if (length>2)
		{
		TBuf<KMaxSizeOfBinaryData> buffer16;
		buffer16.Copy(someBinaryData);
		g_Test.Printf(TRefByValue<const TDesC>_L("%S"),&buffer16);
		if ((someBinaryData[length-3]=='.' && someBinaryData[length-2]==0x0D 
										&& someBinaryData[length-1]==0x0A)
			||(someBinaryData[length-3]=='-' && someBinaryData[length-2]=='E' 
										&& someBinaryData[length-1]=='R'))
			{
			// got ".CRLF" string (-ERR string) from LIST command, so go onto next state
			iReceive=EFalse;
			RequestSend();
			iSendState=EQuitDone;
			return;
			}
		}
	// queue a read
	iReceive=ETrue;
	iImSocket->ReceiveBinaryData(iStatus,someBinaryData,someBinaryData.MaxLength());
	SetActive();
	}

// Receives new request from socket
void CSendPopCommandMessage::QueueNextReceive()
	{
	//queues a new request from the socket 
	iImSocket->QueueReceiveNextTextLine(iStatus);
	SetActive();
	}

// Request function for the active object
void CSendPopCommandMessage::RequestSend()
	{
	_LIT8(KImStat,"STAT\r\n");
	_LIT8(KImList,"LIST\r\n");
	_LIT8(KImQuit,"QUIT\r\n");

	switch(iSendState)
		{
		case EConnect:
		case EConnectDone:
		case EQuitDone:
		break;
		case EUserName:
	        gTestUtils->WriteComment(_L("IP address of client is "));
			gTestUtils->WriteComment(iImSocket->LocalName());
	
			TUint32 convalue;
			TInt err;
			err=iImSocket->GetIAPValue(convalue);
			if(err==KErrNone)
				g_Test.Printf(_L("GetIAPValue returned %u\n"),convalue);
			else 
				g_Test.Printf(_L("GetIAPValue error %d\n"),err);
			err=iImSocket->GetIAPBearer(convalue);
			if(err==KErrNone)
				g_Test.Printf(_L("GetIAPBearer returned %u\n"),convalue);
			else 
				g_Test.Printf(_L("GetIAPBearer error %d\n"),err);
			iImSocket->Send(iStatus,iUserName);
			gTestUtils->WriteComment(_L("\tSent Message USER"));
			g_Test.Printf(_L("\tSent Message USER\n"));
		break;
		case EPassword:
			iImSocket->Send(iStatus,iPass);
			gTestUtils->WriteComment(_L("\tSent Message PASS"));
			g_Test.Printf(_L("\tSent Message PASS\n"));
		break;
		case EStat:
			iImSocket->Send(iStatus,KImStat());
			gTestUtils->WriteComment(_L("\tSent Message STAT"));
			g_Test.Printf(_L("\tSent Message STAT\n"));
		break;
		case EList:
			iImSocket->Send(iStatus,KImList());
			gTestUtils->WriteComment(_L("\tSent Message LIST"));
			g_Test.Printf(_L("\tSent Message LIST\n"));
		break;
		case EQuit:
			iImSocket->Send(iStatus,KImQuit());
			gTestUtils->WriteComment(_L("\tSent Message QUIT"));
			g_Test.Printf(_L("\tSent Message QUIT\n"));
		break;
		case EComplete:
			gTestUtils->WriteComment(_L("\tReq Complete"));
		break;
		}
	SetActive();
	}
	
void CSendPopCommandMessage::ConnectionL()
	{
	g_Test.Next(_L("QueueConnect"));
	SetSelfComplete(EFalse);
	if (!iIAPPreferences)
		{
		iIAPPreferences = CImIAPPreferences::NewLC();
		CleanupStack::Pop(); // iIAPPrefernces
		}
	iImSocket->QueueConnectL(iStatus,iServerAddress,iPort,*iIAPPreferences);
	SetActive();
	g_Test.Next(_L("Connect Queued"));
	gTestUtils->WriteComment(_L("Connect Queued"));
	}

//=========================================//

// Clears the screen
LOCAL_C void ClearScreen()
	{
	g_Test.Console()->ClearScreen();
	}

// Draws menu on screen for user input 
/*
LOCAL_C void DrawMenu()
	{
	g_Test.Printf(_L("\r\n"));
	g_Test.Printf(_L("(1) Single Default Connection (IAP A)\r\n"));
	g_Test.Printf(_L("(2) Single Specified Connection (IAP B)\r\n"));
	g_Test.Printf(_L("(3) Two Default Connections Serially (IAP A and B)\r\n"));
	g_Test.Printf(_L("(4) Two Default Connections Parallelly (IAP A and B)\r\n"));
	g_Test.Printf(_L("(5) One Default and One Specified Connections Serially (IAP A and B)\r\n"));
	g_Test.Printf(_L("(6) One Default and One Specified Connections Parallely (IAP A and B)\r\n"));
	g_Test.Printf(_L("(7) Explicitly connecting to the default IAP\r\n"));
	g_Test.Printf(_L("(a) Test All Cases\r\n"));
	g_Test.Printf(_L("(x) Exit\r\n"));
	g_Test.Printf(_L("\r\n"));
	g_Test.Printf(_L("Select: "));
	}
*/
//Accepts the user input and calls the desired test case
LOCAL_C void doMainL()
	{
	TChar option('0');
	g_NoOfConnections=0;
	
	CTestHarnessUtil* testHarnessUtil=CTestHarnessUtil::NewL();
	CleanupStack::PushL(testHarnessUtil); 

	gTestUtils = CEmailTestUtils::NewLC(g_Test);
	gTestUtils->WriteComment(_L("T_MSG_CONN Testing Email Test Socket"));
	gTestUtils->WriteComment(_L("Please make sure that pipex is the default IAP."));

	CTestHarnessParent *test_harness = NULL;
	test_harness=CTestHarnessParent::NewL();
	CleanupStack::PushL(test_harness); 

//	DrawMenu();
//	option = TChar(g_Test.Getch());
//	g_Test.Printf(_L("%c\r\n"), option);
//	ClearScreen();
	option=ETestAllCases;

	switch(option)
		{
		case ETestSingleDefaultConn:
			g_NoOfConnections=1;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ESingleDefaultConn);
			test_harness->TestSingleDefaultConnL();
		break;
		case ETestSingleSpecifiedConn:
			g_NoOfConnections=1;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ESingleSpecifiedConn);
			test_harness->TestSingleSpecifiedConnL();
		break;
		case ETestTwoDefaultSerialConns:
			g_NoOfConnections=1;	//Coz, connections one after the other
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ETwoDefaultSerialConn);
			test_harness->TestTwoDefaultConnectionsSeriallyL();
		break;
		case ETestTwoDefaultParallelConns:
			g_NoOfConnections=2;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ETwoDefaultParallelConn);
			test_harness->TestTwoDefaultConnectionsParallellyL();
		break;
		case ETestTwoDifferentSerialConns:
			g_NoOfConnections=1;	//Coz, connections one after the other
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ETwoDifferentSerialConn);
			test_harness->TestTwoDifferentConnectionsSeriallyL();
		break;
		case ETestTwoDifferentParallelConns:
			g_NoOfConnections=2;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ETwoDifferentParallelConn);
			test_harness->TestTwoDifferentConnectionsParallellyL();
		break;
		case ETestExplicitDefaultConn:
			g_NoOfConnections=2;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(EExplicitDefaultConn);
			test_harness->TestExplicitDefaultConnL();
			break;
		case ETestAllCases:
			g_NoOfConnections=1;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ESingleDefaultConn);
			test_harness->TestSingleDefaultConnL();

			g_NoOfConnections=1;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ESingleSpecifiedConn);
			test_harness->TestSingleSpecifiedConnL();

			g_NoOfConnections=1;	//Coz, connections one after the other
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ETwoDefaultSerialConn);
			test_harness->TestTwoDefaultConnectionsSeriallyL();

			g_NoOfConnections=2;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ETwoDefaultParallelConn);
			test_harness->TestTwoDefaultConnectionsParallellyL();

			g_NoOfConnections=1;	//Coz, connections one after the other
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ETwoDifferentSerialConn);
			test_harness->TestTwoDifferentConnectionsSeriallyL();

			g_NoOfConnections=2;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(ETwoDifferentParallelConn);
			test_harness->TestTwoDifferentConnectionsParallellyL();

			g_NoOfConnections=2;
			g_Count=g_NoOfConnections;
			test_harness->SetTestMode(EExplicitDefaultConn);
			test_harness->TestExplicitDefaultConnL();
		break;
		default:
			gTestUtils->WriteComment(_L("Exit"));
		}
	ClearScreen();
	gTestUtils->WriteComment(_L("**********       T_MSG_CONN Tests Complete       **********"));
	gTestUtils->TestHarnessCompleted();

	CleanupStack::PopAndDestroy(test_harness);
	CleanupStack::PopAndDestroy(gTestUtils);
	CleanupStack::PopAndDestroy(testHarnessUtil);
	}

// Entry point 
GLDEF_C TInt E32Main()
	{
	g_Test.Title();
	g_Test.Start(_L("Testing Email Text Socket"));
	__UHEAP_MARK;

	theCleanup=CTrapCleanup::New();
	g_Test (theCleanup!=NULL);
	TRAPD(ret,doMainL());		
	g_Test (ret==KErrNone);
	delete theCleanup;

	__UHEAP_MARKEND;
	g_Test.End();
	return 0;
	}
