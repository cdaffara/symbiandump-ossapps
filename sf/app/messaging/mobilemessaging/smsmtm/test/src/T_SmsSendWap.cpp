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

#include "T_SmsSendWap.h"
#include <smsuaddr.h>

RTest test(_L("T_SmsSendWap Testrig"));
CTrapCleanup* theCleanup;


void CSmsSendWapTest::TestSendingL()
/**
	Tests Sending
 */
	{
	DoTestSendingL(iScriptFile);
	}

void CSmsSendWapTest::TestSchedulingL()
/**
	Tests schedule sending
 */
	{
//	DoTestSchedulingL(KWapScript);
	}

void CSmsSendWapTest::ConstructL()
	{
	CSmsSendTestBase::ConstructL();
	iSmsTest.SetCommDbServiceCenterFromScriptL(KWapScript);
	}


CSmsSendWapTest::CSmsSendWapTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
: CSmsSendTestBase(aSmsTest, aScriptFile, aCurrentTest)
	{
	}

LOCAL_C void doMainL()
	{
	RSocketServ serv;
	RSocket socket;

	User::LeaveIfError(serv.Connect());
	CleanupClosePushL(serv);
	
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(serv.FindProtocol(protocolname,protoinfo));
	User::LeaveIfError(socket.Open(serv,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(socket.Bind(smsaddr));

	CleanupClosePushL(socket);
	
	CActiveScheduler* iScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(iScheduler);
	CActiveScheduler::Install( iScheduler );

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;

	smsTest->NotifySaPhoneOnL();
	smsTest->WaitForInitializeL();

	CSmsSendWapTest* wapTest = CSmsSendWapTest::NewL(*smsTest, KWapScript, nextTest);
	CleanupStack::PushL(wapTest);

	wapTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmsTest, iScheduler, wapTest
	
	CleanupStack::PopAndDestroy(&socket);
	CleanupStack::PopAndDestroy(&serv);
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("Setup"));
	theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

CSmsSendWapTest* CSmsSendWapTest::NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
	{
	CSmsSendWapTest* self = new (ELeave) CSmsSendWapTest(aSmsTest, aScriptFile, aCurrentTest);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();
	return self;
	}

void CSmsSendWapTest::DoRunSendingL()
/**
	Checks status. 
	The message is rescheduled if sending failed.
 */
	{
	if (iStatus == KErrNone)
		iStatus = iSmsTest.iProgress.iError;

	iSmsTest.Printf(_L("WAP sending completed with error %d\n"), iStatus.Int());

	iState = EStateWaiting;
	TInt err = KErrNone;

	if (!iSmsTest.SendingCompleteL(*iSelection, err))
		{
		iSmsTest.Printf(_L("Waiting for all messages to send or fail\n"), iStatus);
		iStatus = KRequestPending;
		SetActive();
		iState = EStateSchedulingThenSend;
		}
	else
		{
		CActiveScheduler::Stop();

		iStatus = err;
		iSmsTest.Printf(_L("Sending completed with error %d\n"), iStatus);

		if (iStatus == KErrNone)
			{
			TTimeIntervalMicroSeconds32 wait = KWaitForMsgToSend.Int() * iSelection->Count();
			iSmsTest.Printf(_L("Waiting %d seconds for sending to complete\nGo have a cup of tea. You deserve a break!\n"), wait.Int() / 1000000);
			User::After(wait);
			}

		if (!iSmsTest.RunAuto())
			{
			iSmsTest.Test().Printf(_L("\nPress any key to continue...\n"));
			iSmsTest.Test().Getch();
			}
		}
	}

