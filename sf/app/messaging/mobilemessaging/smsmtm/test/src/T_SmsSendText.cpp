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

#include "T_SmsSendText.h"
#include <smuthdr.h>
#include <txtrich.h>

#include <testconfigfileparser.h>
#include <simtsy.h>
#include <smsuaddr.h>
#include <e32property.h>
#include "msvtestutils.h"
RTest test(_L("T_SmsSendText Testrig"));
CTrapCleanup* theCleanup;

CSmsSendTextTest::~CSmsSendTextTest()
/**
	destructor
 */
	{
          //	delete iTextSession;
	}

void CSmsSendTextTest::TestSendingL()
/**
	Tests Sending
 */
	{
	DoTestSendingL(iScriptFile);
	}

void SetTestNumberL(TInt aTestNumber)
  	{
  	test.Printf(_L("Changing the Test Number to %d\n"), aTestNumber);
	TInt testState;
	TInt error = RProperty::Get(KUidSystemCategory, KUidSASimTsyTestNumberValue, testState);
	if(KErrNone != error)
		{
		User::LeaveIfError(RProperty::Define(KUidSystemCategory, KUidSASimTsyTestNumberValue, RProperty::EInt));
		}
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KUidSASimTsyTestNumberValue, aTestNumber));
  	}

void CSmsSendTextTest::RunAutoL()
/**
	Runs tests in sequence, checks iStatus
 */
	{
	SetTestNumberL(0);

	iSmsTest.TestStart(++iNextTest);
	TestSendNoRecipientsL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	CSmsSendTestBase::RunAutoL();
	}

void CSmsSendTextTest::RunL()
/**
	Handles completed async operations
 */
	{
	if (iOperation)
		iSmsTest.SetProgressL(*iOperation);

	switch (iSendTextTestState)
		{
		case EStateSendNoRecipients:
			DoRunSendNoRecipientsL();
			break;
		default:
			CSmsSendTestBase::RunL();
			break;
		}
	}

void CSmsSendTextTest::TestSendNoRecipientsL()
/**
	Tests sending with no receipients.
	The error -1 (KErrNotFound) is expected.
 */
	{
	iState = EStateOther;
	iSendTextTestState = EStateSendNoRecipients;

	iSmsTest.Test().Next(_L("Sending No Recipients"));

	iSmsTest.DeleteSmsMessagesL(KMsvGlobalOutBoxIndexEntryId);

	TTime now;
	now.HomeTime();
	now += (TTimeIntervalSeconds) 5;

	iSelection->Reset();

	TBool read = EFalse;
	TRAPD(err, read = iSmsTest.ReadScriptL(iScriptFile, KMsvGlobalOutBoxIndexEntryId, *iSelection, now));

	iSmsTest.Test()(!err && read);


	//Remove Recipients
	TInt count = iSelection->Count();

	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iSmsTest.iRichText);
	CleanupStack::PushL(header);

	while (count--)
		{
		iSmsTest.SetEntryL(iSelection->At(count));
		CMsvStore* store = iSmsTest.EditStoreL();
		CleanupStack::PushL(store);

		header->RestoreL(*store);

		header->Recipients().ResetAndDestroy();

		header->StoreL(*store);
		store->CommitL();

		CleanupStack::PopAndDestroy(); //store
		}

	CleanupStack::PopAndDestroy(); //header

	iSmsTest.SetEntryL(KMsvGlobalOutBoxIndexEntryId);

	delete iOperation;
	iOperation = NULL;
	iOperation = MsvEntry().CopyL(*iSelection, iSmsTest.iSmsServiceId, iStatus);
		
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsSendTextTest::DoRunSendNoRecipientsL()
/**
	Checks the status and error.
	The error -1 (KErrNotFound) is expected
 */
	{
	CActiveScheduler::Stop();

	TSmsProgress prog = iSmsTest.iProgress;

	test(prog.iError == KErrNone || iStatus.Int() == KErrNone);
	iStatus = KErrNone;

	TInt error = KErrNone;

	iSmsTest.DisplaySendingStatesL(*iSelection);
	test(iSmsTest.SendingCompleteL(*iSelection, error));
	test(error == KErrNotFound);

	iState = CSmsSendTestBase::EStateWaiting;
	iSendTextTestState = EStateWaiting;

	delete iOperation;
	iOperation = NULL;
	}

void CSmsSendTextTest::TestSchedulingL()
/**
	Tests schedule sending
 */
	{
	DoTestSchedulingL(iScriptFile);
	}


CSmsSendTextTest::CSmsSendTextTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
: CSmsSendTestBase(aSmsTest, aScriptFile, aCurrentTest)
/**
	Constructor
 */
	{
	}

LOCAL_C void doMainL()
	{
	// Copying the config file to the root directory

	_LIT(KFileName,"config.txt");
	_LIT(KFileName2,"\\msgtest\\sms\\sms.script");
	
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	fs.SetSessionPath(_L("c:\\"));
	fs.Delete(KFileName);					//< Ignore Error

	CFileMan* fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);

	User::LeaveIfError(fileMan->Copy(KFileName2, KFileName));

	CleanupStack::PopAndDestroy(fileMan);
	CleanupStack::PopAndDestroy(&fs);

	// Open the socket server

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

	CSmsSendTextTest* textTest = CSmsSendTextTest::NewL(*smsTest, KSmsScript, nextTest);
	CleanupStack::PushL(textTest);

	textTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmsTest, iScheduler, textTest

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

CSmsSendTextTest* CSmsSendTextTest::NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
	{
	CSmsSendTextTest* self = new (ELeave) CSmsSendTextTest(aSmsTest, aScriptFile, aCurrentTest);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();
	return self;
	}

/*void CSmsSendTextTest::TestSendWithDataCallL()
	{
	TestConnectingL();
	DoTestSendingL(KSmsScript);

	iTextSession->Disconnect();
	WaitForEndOfDataCallL();
	test.Printf(_L("  Waiting 10s\n"));
	User::After(10000000);
	delete iTextSession;
	iTextSession = NULL;
	}


 void CSmsSendTextTest::WaitForEndOfDataCallL()
	{
	// Connect to etel
	RTelServer etel;
	User::LeaveIfError(etel.Connect());
	CleanupClosePushL(etel);

	// Get tsy name
	TName tsy;
	FaxUtils::GetTsyNameL(tsy);

	// Load the TSY
	User::LeaveIfError(etel.LoadPhoneModule(tsy));

	// Get phone for the TSY
	RTelServer::TPhoneInfo info;
	test(FaxUtils::GetPhoneInfoForTsyL(etel, tsy, info));

	// Load the phone
	RPhone phone;
	User::LeaveIfError(phone.Open(etel, info.iName));
	CleanupClosePushL(phone);

	// Get data line info
	RPhone::TLineInfo data;
	test(GetDataLineInfoForPhoneL(phone, data));

	// Open the phone line
	RLine line;
	User::LeaveIfError(line.Open(phone, data.iName));
	CleanupClosePushL(line);

	// Get line hook status
	RCall::THookStatus hook;
	User::LeaveIfError(line.GetHookStatus(hook));

	// Wait until the line is on hook
	if (hook == RCall::EHookStatusOff)
		{
		test.Printf(_L("  Waiting for data call to end\n"));
		TRequestStatus status;
		line.NotifyHookChange(status, hook);
		User::WaitForRequest(status);
		}

	// Unload everything
	CleanupStack::PopAndDestroy(2); // line, phone
	User::LeaveIfError(etel.UnloadPhoneModule(tsy));
	CleanupStack::PopAndDestroy(); // etel
	}

void CSmsSendTextTest::TestConnectingL()
	{
	// Create a waiter
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();

	// Create text sever session
	iTextSession = CImTextServerSession::NewL();

	// Create a timer
	CTestTimer* timer = CTestTimer::NewL();
	CleanupStack::PushL(timer);

	// Connect to the internet
	const TInt maxCount = 5;
	TInt count = maxCount;

	CImIAPPreferences* pref = CImIAPPreferences::NewLC();

	do
		{
		test.Printf(_L("  Connecting to the internet (attempt %d of %d, last error %d)\n"), maxCount - count + 1, maxCount, wait->iStatus.Int());
		iTextSession->QueueConnectL(wait->iStatus,_L("pop3.demon.co.uk"), 110, *pref);
		wait->Start();
		CActiveScheduler::Start();
		} while (wait->iStatus != KErrNone && count--);

	CleanupStack::PopAndDestroy(pref);

	// Receive stuff
	while (wait->iStatus == KErrNone)
		{
		// Receive some text
		test.Printf(_L("  Receiving data\n"));
		iTextSession->QueueReceiveNextTextLine(wait->iStatus);
		wait->Start();

		// Start a timer - so we give up eventually - 10s
		timer->After(10000000);
		CActiveScheduler::Start();

		// Did the timer complete?
		if (!timer->IsActive())
			{
			// Cancel the outstanding request
			iTextSession->Cancel();
			CActiveScheduler::Start();
			}
		else
			{
			// Get the data
			timer->Cancel();
			TBuf8<1020> textLineNarrow;
			TBuf<1024> textLineWide;
			iTextSession->GetCurrentTextLine(textLineNarrow);
			textLineWide.Copy(textLineNarrow);

			// Make it look nice!
			if (textLineWide.Length() > 0)
				{
				textLineWide.Insert(0, _L("  "));
				textLineWide.Append(_L("\n"));
				test.Printf(textLineWide);
				}
			}
		}
	
	// Tidy up
	test(wait->iStatus == KErrNone || wait->iStatus == KErrCancel);
	CleanupStack::PopAndDestroy(2); // timer, wait
	}



TBool CSmsSendTextTest::GetDataLineInfoForPhoneL(RPhone& aPhone, RPhone::TLineInfo& aLineInfo)
	{
	// Get number of lines
	TInt line;
	User::LeaveIfError(aPhone.EnumerateLines(line));

	// Iterate through all the lines
	while(line--)
		{
		// Get line info
		RPhone::TLineInfo lineInfo;
		User::LeaveIfError(aPhone.GetLineInfo(line, lineInfo));

		// Does it support fax?
		if ((lineInfo.iLineCapsFlags & RLine::KCapsData) != 0)
			{
			aLineInfo = lineInfo;
			break;
			}
		}

	// Did we find a fax line on the phone?
	return line >= 0;
	}
*/
