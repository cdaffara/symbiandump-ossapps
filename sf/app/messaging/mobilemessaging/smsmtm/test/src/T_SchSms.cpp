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
#include "T_SchSms.h"

CSchSmsTest::~CSchSmsTest()
	{
		if (iSmsTest.iMsvSession)
			{
			iSmsTest.iMsvSession->RemoveObserver(*this);
			}
	}


void CSchSmsTest::TestDeleteScheduleL()
	{
	}

void CSchSmsTest::TestCheckScheduleL()
	{
	}


void CSchSmsTest::DoTestSendingL(const TDesC& aScript)
//
//
//
	{
	CSmsTestBase::DoTestSendingL(aScript);
	iState = EStateSending;
	CActiveScheduler::Start();
	}

void CSchSmsTest::DoTestSchedulingL(const TDesC& aScript, TTimeIntervalSeconds aFromNow)
	{
	CSmsTestBase::DoTestSchedulingL(aScript, EFalse, aFromNow);
//	iState = EStateScheduling;
	CActiveScheduler::Start();
	}

CSchSmsTest::CSchSmsTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
: CSmsTestBase(aSmsTest, aScriptFile, aCurrentTest), iState(EStateWaiting)
	{
	}

void CSchSmsTest::RunAutoL()
	{
	iSmsTest.TestStart(++iNextTest, _L("Check Schedule"));
	TestCheckScheduleL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Delete Schedule"));
	TestDeleteScheduleL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	}

void CSchSmsTest::ConstructL()
	{
	iSmsTest.SetLogToFile();

	iSelection = new (ELeave) CMsvEntrySelection();

	iSmsTest.iMsvSession->AddObserverL(*this);

	CActiveScheduler::Add(this);
	}

void CSchSmsTest::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	
	if (!IsActive())
		return;

/*	switch (iState)
		{
		case EStateSchedulingThenSend:
		case EStateSending:
			{
			TInt error = KErrNone;

			DisplaySendingStatesL(*entries);

			if (SendingCompleteL(error) && iState == EStateSchedulingThenSend)
				{
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, error);
				}
			break;
			}
		default:
			break;
		}*/
	}


void CSchSmsTest::ShowMenuL()
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Check Schedule Test"));
	iSmsTest.AppendToMenuL(_L("Delete Schedule Test"));
	iSmsTest.AppendToMenuL(_L("Display Log"));
	iSmsTest.AppendToMenuL(_L("Clear Log"));

	TInt result = iSmsTest.DisplayMenu(_L("SMS Send Test"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			TestCheckScheduleL();
			break;
		case 2:
			TestDeleteScheduleL();
			break;
		case 3:
			DisplayLogL();
			break;
		case 4:
			ClearLogL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

	ShowMenuL();
	}

void CSchSmsTest::RunL()
	{
	if (iOperation)
		iSmsTest.SetProgressL(*iOperation);

/*	switch (iState)
		{
		case EStateScheduling:
			DoRunSchedulingL();
			break;
		case EStateSending:
		case EStateSchedulingThenSend:
			DoRunSendingL();
			break;
		default:
			{
			break;
			}
		}*/

	delete iOperation;
	iOperation = NULL;
	}

void CSchSmsTest::DoCancel()
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

void CSchSmsTest::DoRunSchedulingL()
	{
/*	iSmsTest.Printf(_L("Scheduling completed with error %d\n"), iStatus);

	TBool sent = EFalse;

	if (iStatus == KErrNone)
		{
		iSmsTest.DisplaySendingStatesL(*iSelection);
		TInt error = KErrNone;
		sent = SendingCompleteL(error);
		iStatus = error;
		}
	
	if (!sent)
		{
		iStatus = KRequestPending;
		SetActive();
		iState = EStateSchedulingThenSend;
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
		}*/
	}

void CSchSmsTest::DoRunSendingL()
	{
/*	CActiveScheduler::Stop();
	iState = EStateWaiting;
	iSmsTest.Printf(_L("Sending completed with error %d\n"), iStatus);

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Test().Printf(_L("\nPress any key to continue...\n"));
		iSmsTest.Test().Getch();
		}*/
	}

LOCAL_C void doMainL(RTest& aTest)
	{
	CActiveScheduler* iScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(iScheduler);
	CActiveScheduler::Install( iScheduler );

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(aTest);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;


	CSchSmsTest* schTest = CSchSmsTest::NewLC(*smsTest, KNullDesC, nextTest);

	schTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmsTest, iScheduler
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	RTest test(_L("SchSms Test"));
	test.Start(_L("Setup"));
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL(test));		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

CSchSmsTest* CSchSmsTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
	{
	CSchSmsTest* self = new (ELeave) CSchSmsTest(aSmsTest, aScriptFile, aCurrentTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
