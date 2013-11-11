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

#include <e32test.h>
#include <e32uid.h>
#include <e32std.h>
#include <smuthdr.h>
#include <smscmds.h>
#include "T_SmsCancel.h"

#define KSmsScript _L("sms.script")

CSmsCancelTest::~CSmsCancelTest()
	{
	if (iSmsTest.iMsvSession)
		{
		iSmsTest.iMsvSession->RemoveObserver(*this);
		}
		iSocket.Close();
		iSocketServ.Close();
	}

CSmsCancelTest::CSmsCancelTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
: CSmsTestBase(aSmsTest, aScriptFile, aCurrentTest), iState(EStateWaiting)
	{
	}

void CSmsCancelTest::RunAutoL()
	{
	
	iSmsTest.TestStart(++iNextTest, _L("Cancel Sending"));
	TestCancelSendingL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Cancel Sending after Scheduling"));
	TestCancelSendingAfterScheduleL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest);
	TestCancelReadScL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest);
	TestCancelWriteScL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest);
	TestCancelReadSimParamsL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest);
	TestCancelEnumerateL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	}

void CSmsCancelTest::ConstructL()
	{
	iSmsTest.SetLogToFile();

	iSelection = new (ELeave) CMsvEntrySelection();

	iSmsTest.iMsvSession->AddObserverL(*this);

	CActiveScheduler::Add(this);
	
	User::LeaveIfError(iSocketServ.Connect());
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(iSocketServ.FindProtocol(protocolname,protoinfo));
	User::LeaveIfError(iSocket.Open(iSocketServ,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));
	
	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(iSocket.Bind(smsaddr));
	}

void CSmsCancelTest::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	CMsvEntrySelection* entries = NULL;

	switch (aEvent)
		{
		case EMsvEntriesCreated:
		case EMsvEntriesChanged:
		case EMsvEntriesMoved:
			entries = STATIC_CAST(CMsvEntrySelection*, aArg1);
			break;
		default:
			return;
		}

	if (!IsActive())
		return;

	switch (iState)
		{
		case EStateSending:
			{
			iSmsTest.DisplaySendingStatesL(*entries);
			CancelIfSendingL(*entries);

			TInt error = KErrNone;

			if (iSmsTest.SendingCompleteL(*iSelection, error) && !iOperation && iStatus == KRequestPending)
				{
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, error);
				}
			break;
			}
		default:
			break;
		}
	}

void CSmsCancelTest::CancelIfSendingL(const CMsvEntrySelection& aSelection)
	{
	const TInt count = aSelection.Count();

	for (TInt i = 0; i < count; i++)
		{
		TRAPD(err, iSmsTest.SetEntryL(aSelection[i]));

		if (!err)
			{
			TMsvEntry entry = iSmsTest.Entry();

			if (entry.SendingState() == KMsvSendStateSending)
				{
				if (iOperation)
					{
					iOperation->Cancel();
					return;
					}
				else
					{
					entry.SetSendingState(KMsvSendStateSuspended);
					TRAP(err, iSmsTest.ChangeEntryL(entry)); //ignore error

					if (err)
						{
						iSmsTest.Printf(_L("Error Cancelling Entry %d: On ChangeEntryL()\n"), aSelection[i]);
						} //end if
					} //end if
				} //end if
			}
		else
			{
			iSmsTest.Printf(_L("Error Cancelling Entry %d: On SetEntryL()\n"), aSelection[i]);
			}
		}
	}

void CSmsCancelTest::ShowMenuL()
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Test Cancel Sending"));
	iSmsTest.AppendToMenuL(_L("Test Cancel Scheduling"));

	TInt result = iSmsTest.DisplayMenu(_L("SMS Cancel Test"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			TestCancelSendingL();
			break;
		case 2:
			TestCancelSendingAfterScheduleL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

	ShowMenuL();
	}

void CSmsCancelTest::RunL()
	{
	if (iOperation)
		iSmsTest.SetProgressL(*iOperation);

	switch (iState)
		{
		case EStateScheduling:
			DoRunSchedulingL();
			break;
		case EStateSending:
			DoRunSendingL();
			break;
		default:
			{
			break;
			}
		}

	delete iOperation;
	iOperation = NULL;
	}

void CSmsCancelTest::DoCancel()
	{
	if (iOperation)
		{
		iOperation->Cancel();
		iSmsTest.Printf(_L("Operation Cancelled!\n"));
		}
	else
		{
		iSmsTest.Printf(_L("No operation to cancel!\n"));
		}

	delete iOperation;
	iOperation = NULL;
	}

void CSmsCancelTest::DoRunSchedulingL()
	{
	if (iStatus == KErrNone)
		iStatus = iSmsTest.iProgress.iError;

	iSmsTest.Printf(_L("Scheduling completed with error %d\n"), iStatus);

	TBool sent = EFalse;

	if (iStatus == KErrNone)
		{
		iSmsTest.DisplaySendingStatesL(*iSelection);
		TInt error = KErrNone;
		sent = iSmsTest.SendingCompleteL(*iSelection, error);
		iStatus = error;
		}
	
	if (!sent)
		{
		iStatus = KRequestPending;
		SetActive();
		iState = EStateSending;
		}
	else
		{
		CActiveScheduler::Stop();
		iState = EStateWaiting;
		iSmsTest.Printf(_L("Scheduling completed with error %d\n"), iStatus);

		if (!iSmsTest.RunAuto())
			{
			iSmsTest.Test().Printf(_L("\nPress any key to continue...\n"));
			iSmsTest.Test().Getch();
			}
		}
	}

void CSmsCancelTest::DoRunSendingL()
	{
	CActiveScheduler::Stop();

	if (!iOperation) //scheduled
		{
		TTimeIntervalMicroSeconds32 wait = 5000000;
		iSmsTest.Printf(_L("\nWaiting %d seconds for SMSS to complete...\n\n"), wait.Int() / 1000000);
		User::After(wait);
		}

	if (iStatus == KErrNone)
		iStatus = iSmsTest.iProgress.iError;

	iState = EStateWaiting;
	iSmsTest.Printf(_L("Sending completed with error %d\n"), iStatus);

	if (iStatus == KErrCancel)
		{
		iSmsTest.Printf(_L("Error %d is expected and OK\n"), iStatus);
		iStatus = KErrNone;
		}

	iSmsTest.Printf(_L("Final Sending States of Messages:\n\n"), iStatus);
	iSmsTest.DisplaySendingStatesL(*iSelection);

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Test().Printf(_L("\nPress any key to continue...\n"));
		iSmsTest.Test().Getch();
		}
	}

RTest test(_L("SchSms Test"));


LOCAL_C void doMainL()
	{
	CActiveScheduler* iScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(iScheduler);
	CActiveScheduler::Install( iScheduler );

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;
	
	smsTest->NotifySaPhoneOnL();

	CSmsCancelTest* cancelTest = CSmsCancelTest::NewLC(*smsTest, KSmsScript, nextTest);

	smsTest->WaitForInitializeL();

	cancelTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmsTest, iScheduler
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("Setup"));
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

CSmsCancelTest* CSmsCancelTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
	{
	CSmsCancelTest* self = new (ELeave) CSmsCancelTest(aSmsTest, aScriptFile, aCurrentTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CSmsCancelTest::TestCancelReadScL()
	{
	}

void CSmsCancelTest::TestCancelWriteScL()
	{
	}

void CSmsCancelTest::TestCancelReadSimParamsL()
	{
	}

void CSmsCancelTest::TestCancelEnumerateL()
	{
	}

void CSmsCancelTest::TestCancelSendingL()
	{
	DoTestSendingL(iScriptFile);
	iState = EStateSending;
	CActiveScheduler::Start();
	}

void CSmsCancelTest::TestCancelSendingAfterScheduleL()
	{
	DoTestSchedulingL(iScriptFile);
	iState = EStateScheduling;
	CActiveScheduler::Start();
	}
