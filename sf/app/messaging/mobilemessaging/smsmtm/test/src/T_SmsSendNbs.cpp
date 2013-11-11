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

#include "T_SmsSendNbs.h"
#include <smsuaddr.h>

RTest test(_L("T_SmsSendNbs Testrig"));
CTrapCleanup* theCleanup;

void CSmsSendNbsTest::TestSendingL()
/**
	Tests Sending
 */
	{
	DoTestSendingL(iScriptFile);
	}

void CSmsSendNbsTest::TestSchedulingL()
/**
	Tests schedule sending
 */
	{
	DoTestSchedulingL(iScriptFile);
	}


CSmsSendNbsTest::CSmsSendNbsTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
: CSmsSendTestBase(aSmsTest, aScriptFile, aCurrentTest)
/**
	Constructor
 */
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

	CSmsSendNbsTest* nbsTest = CSmsSendNbsTest::NewL(*smsTest, KNbsScript, nextTest);
	CleanupStack::PushL(nbsTest);

	nbsTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmsTest, iScheduler

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

CSmsSendNbsTest* CSmsSendNbsTest::NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
	{
	CSmsSendNbsTest* self = new (ELeave) CSmsSendNbsTest(aSmsTest, aScriptFile, aCurrentTest);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();
	return self;
	}

