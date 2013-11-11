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

#include "T_SmsSendBase.h"
#include <e32test.h>
#include <e32uid.h>
#include <e32std.h>
#include <smuthdr.h>
#include <txtrich.h> 
#include <smscmds.h>
#include <smsclnt.h>

void CSmsSendTestBase::DoTestSendingL(const TDesC& aScript)
/**
	Tests Sending: Creates and sends sms messages defined in aScript.

	@param		aScript		Script file containing sms messages 
 */
	{
	CSmsTestBase::DoTestSendingL(aScript);
	iState = EStateSending;
	CActiveScheduler::Start();
	}

void CSmsSendTestBase::DoTestSchedulingL(const TDesC& aScript, TTimeIntervalSeconds aFromNow)
/**
	Tests Schedule Sending: Creates and schedules sms messages defined in aScript.

	@param		aScript			Script file containing sms messages 
	@param		aFromNow		Time interval 
 */
	{
	CSmsTestBase::DoTestSchedulingL(aScript, EFalse, aFromNow);
	iState = EStateScheduling;
	CActiveScheduler::Start();
	}

CSmsSendTestBase::~CSmsSendTestBase()
/**
	destructor
 */
	{
	if (iSmsTest.iMsvSession)
		{
		iSmsTest.iMsvSession->RemoveObserver(*this);
		}
	}

CSmsSendTestBase::CSmsSendTestBase(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
: CSmsTestBase(aSmsTest, aScriptFile, aCurrentTest), iState(EStateWaiting)
	{
	}

void CSmsSendTestBase::RunAutoL()
/**
	Runs tests in sequence, checks iStatus
 */
	{
	// Test sending SMSs
	iSmsTest.TestStart(++iNextTest);
	TestSendingL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	// Test Scheduling
	iSmsTest.TestStart(++iNextTest);
	TestSchedulingL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	// Test forwarding an already sent SMS
	iSmsTest.TestStart(++iNextTest);
	TestForwardSentMsgL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	}

void CSmsSendTestBase::ConstructL()
	{
	iSmsTest.SetLogToFile();

	iSelection = new (ELeave) CMsvEntrySelection();

	iSmsTest.iMsvSession->AddObserverL(*this);

	CActiveScheduler::Add(this);
	}

void CSmsSendTestBase::HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
/**
	Handles Session Event, used by MMsvSessionObserver

	@param		aEvent		Msv Session Event
	@param		aArg1		Argument 1
	@param		aArg2		Argument 2 
	@param		aArg3		Argument 3 
 */
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
		case EStateSchedulingThenSend:
		case EStateSending:
			{
			TInt error = KErrNone;

			iSmsTest.DisplaySendingStatesL(*entries);

			if (iSmsTest.SendingCompleteL(*iSelection, error) && iState == EStateSchedulingThenSend && iStatus == KRequestPending)
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


void CSmsSendTestBase::ShowMenuL()
/**
	Shows menu
 */
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Send Messages"));
	iSmsTest.AppendToMenuL(_L("Schedule Messages"));
	iSmsTest.AppendToMenuL(_L("Display Log"));
	iSmsTest.AppendToMenuL(_L("Clear Log"));
	iSmsTest.AppendToMenuL(_L("Navigate Message Store"));

	TInt result = iSmsTest.DisplayMenu(_L("SMS Send Test"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			TestSendingL();
			break;
		case 2:
			TestSchedulingL();
			break;
		case 3:
			DisplayLogL();
			break;
		case 4:
			ClearLogL();
			break;
		case 5:
			iSmsTest.NavigateMessageStoreL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

	ShowMenuL();
	}

void CSmsSendTestBase::RunL()
/**
	Handles completed async operations
 */
	{
	if (iOperation)
		iSmsTest.SetProgressL(*iOperation);

	switch (iState)
		{
		case EStateScheduling:
			DoRunSchedulingL();
			break;
		case EStateSending:
		case EStateSchedulingThenSend:
			DoRunSendingL();
			break;
		default:
			break;
		}

	delete iOperation;
	iOperation = NULL;
	}

void CSmsSendTestBase::TestForwardSentMsgL()
/**
	Tests forwarding a Sent Message
 */
	{
	// Get all the messages in the Sent Folder
	SmsTest().SetEntryL(KMsvSentEntryId);
	CMsvEntrySelection* sentEntries = SmsTest().ChildrenWithMtmLC(KUidMsgTypeSMS);

	// If there are any Sent Messages, then Forward the first one
	if (sentEntries->Count())
		{
		// This will create and send the forwarded SMS
		TMsvId sentId = sentEntries->At(0);
		DoTestForwardMsgL(sentId);

		iState = EStateSending;
		CActiveScheduler::Start();
		}

	CleanupStack::PopAndDestroy(); //sentEntries
	}

void CSmsSendTestBase::DoCancel()
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

void CSmsSendTestBase::DoRunSchedulingL()
/**
	Checks status. 
	The message is rescheduled if sending failed.
 */
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
		}
	}

void CSmsSendTestBase::DoRunSendingL()
/**
	Checks status. 
	The message is rescheduled if sending failed.
 */
	{
	if (iStatus == KErrNone)
		iStatus = iSmsTest.iProgress.iError;

	iState = EStateWaiting;
	iSmsTest.Printf(_L("Sending completed with error %d\n"), iStatus);

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

		if (!iSmsTest.RunAuto())
			{
			iSmsTest.Test().Printf(_L("\nPress any key to continue...\n"));
			iSmsTest.Test().Getch();
			}
		}
	}
