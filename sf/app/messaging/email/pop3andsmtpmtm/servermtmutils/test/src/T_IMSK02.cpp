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
// The purpose of this test harness is to test a fix for INC37442.
// In order to test this, it is necessary to start 2 connections to the same IAP off and then
// cancel one of them. This must be done before the CImConnect has finished connecting.
// 
//


#include "TESTENV.h"
#include <nd_err.h>

#include <imsk.h>

#include <s32mem.h>
#include <s32file.h>
#include <iapprefs.h>
#include "emailtestutils.h"
#include <commdb.h>


LOCAL_D RTest test(_L("IMSK Multiple CImTextServerSessions Test"));

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D RFs theFs;
LOCAL_D CActiveScheduler* scheduler;
LOCAL_D CEmailTestUtils* testUtils;

// Test Harness Prototypes
LOCAL_C void DoTestThreadWorkL();
static TInt DoTestThreadStart(TAny*);

// Foward declare
class CMsgConnect;
CMsgConnect* gConnect;


// we use a freeserve account;
// this will need to be set up as COMMDB_ID 1 in the cdbv3.dat file
_LIT(KServerAddressConn0, "pop.freeserve.net");
_LIT(KServerAddressConn1, "www.guardian.co.uk");
const TInt KPortNumber = 110;

const TInt KConnectingProgressValue = 2500;
const TInt KConnectionMadeProgressValue = 7000;


// create an active object to connect
class CMsgConnect : public CActive
	{
	public:
		static CMsgConnect* NewLC(TInt aConn);
		~CMsgConnect();

	public:		// CActive
		void RunL();
		void DoCancel();

	public:
		void RequestConnectL();
		void CancelConnectL();
		TInt ConnectProgress();
		TInt GetIAPValue(TUint32 &aIAP);
		TInt GetIAPBearer(TUint32 &aBearer);

	private:
		CMsgConnect(TInt aConn);
		void ConstructL();

	private:
		CImTextServerSession*	iImSocket;
		CImIAPPreferences*		iIAPPreferences;
		TInt					iConn;
	};


CMsgConnect::CMsgConnect(TInt aConn)
:CActive(EPriorityNormal),
 iConn(aConn)
	{
	}

CMsgConnect* CMsgConnect::NewLC(TInt aConn)
	{
	CMsgConnect* self = new (ELeave) CMsgConnect(aConn);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CMsgConnect::ConstructL()
	{
	CActiveScheduler::Add(this); // add to active scheduler
	iImSocket = CImTextServerSession::NewL();
	testUtils->WriteComment(_L("CMsgConnect::ConstructL() OK"));
	}

CMsgConnect::~CMsgConnect()
	{
	Cancel();
	delete iImSocket;
	delete iIAPPreferences;
	}

void CMsgConnect::DoCancel()
	{
	iImSocket->Cancel();
	}

void CMsgConnect::RunL()
	{
	testUtils->WriteComment(_L("CMsgConnect::RunL() start"));

	// if we have cancelled the connection, then the value here will be KErrCancel
	if(iStatus.Int() == KErrCancel)
		{
		testUtils->WriteComment(_L("CMsgConnect - connection cancelled"));
		test.Printf(_L("cancelled"));
		return;
		}

	testUtils->WriteComment(_L("CMsgConnect - connection made"));
	// if we've connected, then say so
	test.Printf(_L("connected"));
	CActiveScheduler::Stop();
	}

void CMsgConnect::RequestConnectL()
	{
	testUtils->WriteComment(_L("CMsgConnect::RequestConnectL() start"));
	test.Next(_L("connecting"));

	iIAPPreferences = CImIAPPreferences::NewLC();		// PUSH		(there is not a NewL() function)
	CleanupStack::Pop(iIAPPreferences);					// POP

	TImIAPChoice iap;
	iap.iIAP = 1;		// COMMDB_ID 1
	iap.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	iIAPPreferences->AddIAPL(iap);

	TBuf<32> serverAddress;
	if(iConn == 0)
		serverAddress = KServerAddressConn0;
	else
		serverAddress = KServerAddressConn1;

	testUtils->WriteComment(_L("CMsgConnect - queuing connect with CImTextServerSesison"));
	iImSocket->QueueConnectL(iStatus, serverAddress, KPortNumber, *iIAPPreferences);
	SetActive();
	}

void CMsgConnect::CancelConnectL()
	{
	testUtils->WriteComment(_L("CMsgConnect cancelling connection"));
	test.Next(_L("Cancel connecting"));
	iImSocket->Disconnect();
	}

TInt CMsgConnect::ConnectProgress()
	{
	return iImSocket->GetConnectionStage();
	}

TInt CMsgConnect::GetIAPValue(TUint32 &aIAP)
	{
	return iImSocket->GetIAPValue(aIAP);
	}

TInt CMsgConnect::GetIAPBearer(TUint32 &aBearer)
	{
	return iImSocket->GetIAPBearer(aBearer);
	}

//

LOCAL_C void Init()
	{
	scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);

	User::LeaveIfError(theFs.Connect());

	// set up the log file
	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->TestStart(0);
	testUtils->SetLogToFile(ETrue);
	testUtils->WriteComment(_L("This tests a change to CImConnect so that it no longer calls iConn.Stop()"));
	testUtils->WriteComment(_L("in its DoCancel(), but iConn.Close() instead."));
	testUtils->WriteComment(_L("This should not cancel other connections to the internet using that IAP."));
	testUtils->WriteComment(_L("\n"));
	}

LOCAL_C void Closedown()
	{
	testUtils->TestHarnessCompleted();
	CleanupStack::PopAndDestroy(testUtils);

	theFs.Close();
	delete scheduler;
	}
// This tests the change made for "INC040630 - Panic while disconnecting a 
// service". Calling CImTextServerSession::GetConnectionStage with a cancelled
// RConnection caused a panic.
LOCAL_C void TestCancelWithProgress()
	{
	_LIT(KTestThreadName, "CancelWithProgress Test Thread");
	const TInt KMinTestHeapSize = 0x10000;
    const TInt KMaxTestHeapSize = 0x100000;

	TRequestStatus requestStatus;
	RThread testThread;

	gConnect = CMsgConnect::NewLC(0);	// PUSH	

	User::SetJustInTime(EFalse);

	// Set up the conditions which cause the panic. I.e. request a connection
	// then cancel it.
	gConnect->RequestConnectL();

	// Call GetConnectionStage, GetIAPValue, and GetIAPBearer to ensure they
	// return the correct results.
	TUint32 bearer = 1;
	TUint32 iap = 1;
	TInt returnValue;
	
	// Test normal behaviour in this state with a valid but unconnected connection.
	returnValue = gConnect->GetIAPBearer(bearer);
	(*testUtils)(returnValue == KErrNotReady);
	returnValue = gConnect->GetIAPValue(iap);
	(*testUtils)(returnValue == KErrNone || returnValue == KErrNotReady);
	(*testUtils)(iap == 1);
	returnValue = gConnect->ConnectProgress();
	(*testUtils)(returnValue != KErrBadHandle);

	// Cancel the CImTextServerSession 
	gConnect->CancelConnectL();

	// Test after cancel.
	returnValue = gConnect->GetIAPBearer(bearer);
	// GetIAPBearer now returns KErrBadHandle. 
	(*testUtils)(returnValue == KErrBadHandle);

	returnValue = gConnect->GetIAPValue(iap);
	// GetIAPValue now returns KErrBadHandle.
	(*testUtils)(returnValue == KErrBadHandle);

	// CImConnect::Progress now returns KErrBadHandle.
	returnValue = gConnect->ConnectProgress();
	(*testUtils)(returnValue == KErrBadHandle);

	// Create a separate thread to call GetConnectionStage
	testThread.Create(
		KTestThreadName, 
		DoTestThreadStart, // replace
		KDefaultStackSize, 
		KMinTestHeapSize, 
		KMaxTestHeapSize, 
		NULL, 
		EOwnerThread);
	testThread.Logon(requestStatus);

    // Let the thread run
    testThread.Resume();
    User::WaitForRequest(requestStatus);

	// Test whether the thread exitied with a panic.
	TInt result = (testThread.ExitType() == EExitPanic);
	if (result)
		testUtils->WriteComment(
			_L("GetConnectionStage caused panic after Cancel"));
	// Pass or fail the test based on whether the thread stopped because of a
	// panic.
	(*testUtils)(!result);
    testThread.Close();
	User::SetJustInTime(ETrue);

	CleanupStack::PopAndDestroy(gConnect);
	}

LOCAL_C void DoTestThreadWorkL()
    {
    CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
    CActiveScheduler::Install(scheduler);
    CleanupStack::PushL(scheduler);
	// ConnectProgress calls GetConnectionStage in CImTextServerSession.
	gConnect->ConnectProgress();
	CleanupStack::PopAndDestroy(scheduler);
	}

static TInt DoTestThreadStart(TAny*)
    {
    // Entry function for the child thread which is created then killed.
    CTrapCleanup* cleanup = CTrapCleanup::New();
    test(cleanup != NULL);

    TRAPD(ret, DoTestThreadWorkL());
    delete cleanup;
    return 0;
    }


//

LOCAL_C void doMainL()
	{
	Init();

	TInt testNo = 1;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(
		_L("Testing calls to CImTextServerSession Cancel then GetConnectionStage"));
	TestCancelWithProgress();
	testUtils->TestFinish(testNo);
	testNo += 1;

	CMsgConnect* connect1 = CMsgConnect::NewLC(0);	// PUSH
	CMsgConnect* connect2 = CMsgConnect::NewLC(1);	// PUSH

	testUtils->WriteComment(_L("1st connect object connecting"));
	connect1->RequestConnectL();
	// wait a while before starting the next connection
	testUtils->WriteComment(_L("waiting a little while before starting the 2nd connection"));
	User::After(1000000);

	testUtils->WriteComment(_L("2nd connect object connecting"));
	connect2->RequestConnectL();

	// we wait until the connect2 is actually connecting,
	// this is when the progress value = 2500
	testUtils->WriteComment(_L("we should only cancel the connection when it is connecting"));

	for(TInt a = 0; a < 100; a++)
		{	
		// if connect2 has got so far as connecting, then we should fail the test because
		// this test relies on us cancelling the connection when it is connecting
		if(connect2->ConnectProgress() == KConnectionMadeProgressValue)
			{
			testUtils->WriteComment(_L("connection made before we can cancel it"));
			testUtils->WriteComment(_L("TEST FAILED"));
			CleanupStack::PopAndDestroy(2);			// connect2 & connect1
			Closedown();
			return;
			}

		// note that the connect progress could have skipped past the connecting value, whilst we
		// are waiting for the 1second to pass, so if the progress is higher than the connecting
		// value, then we should cancel it as well
		if(connect2->ConnectProgress() > KConnectingProgressValue)
			{
			testUtils->WriteComment(_L("cancelling the 2nd connection"));
			connect2->CancelConnectL();
			break;
			}
		
		User::After(1000000);
		}
	
	// wait until the 1st one has connected correctly
	testUtils->WriteComment(_L("wait for the 1st connection object to connect"));
	CActiveScheduler::Start();

	// cancel the connection of the connection that is up and running
	testUtils->WriteComment(_L("cancel the 1st connection object"));
	connect1->CancelConnectL();

	CleanupStack::PopAndDestroy(2);					// connect2 & connect1
	Closedown();
	}


GLDEF_C TInt E32Main()
	{
	test.SetLogged(ETrue);
	test.Title();
	test.Start(_L("Testing Multiple connections on one IAP using CImTextServerSession"));
	__UHEAP_MARK;

	theCleanup=CTrapCleanup::New();
	test (theCleanup!=NULL);
	TRAPD(ret,doMainL());
	test (ret==KErrNone);
	delete theCleanup;

	__UHEAP_MARKEND;
	test.End();
	return 0;
	}
