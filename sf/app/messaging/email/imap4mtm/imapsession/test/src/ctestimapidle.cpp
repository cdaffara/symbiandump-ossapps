// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ctestimapidle.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimapfolderinfo.h"
#include "cimaputils.h"


CTestImapIdle::CTestImapIdle()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapIdle::~CTestImapIdle()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapIdle::SetupL()
	{
	ASSERT(iInputStream == NULL);
	ASSERT(iOutputStream == NULL);
	ASSERT(iActiveWaiter == NULL);
	ASSERT(iImapSession == NULL);

	CImapUtils::CreateL();
	iInputStream = CFakeInputStream::NewL(Logger());
	iOutputStream = CFakeOutputStream::NewL(Logger());
	iActiveWaiter = new(ELeave)CActiveWaiter(Logger());
	
	CImapSettings* imapSettings=NULL; 
	CImapMailStore* imapMailStore=NULL;
	
	iImapSession = CImapSession::NewL(*imapSettings,*imapMailStore,*iInputStream, *iOutputStream);
	
	INFO_PRINTF1(_L("Setup: ServerGreeting"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK Microsoft Exchange 2000 IMAP4rev1 server version 6.0.6249.0 (lon-cn-exchng2k.msexchange2k.closedtest.intra) ready.\r\n"));
	
	ASSERT_EQUALS(CImapSession::EServerStateNone, iImapSession->ServerState());
	
	iImapSession->ReadServerGreetingL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("...Login"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK LOGIN completed\r\n"));
			
	iImapSession->LoginL(iActiveWaiter->iStatus, _L8("username"), _L8("password"));
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());


	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 23 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 1 RECENT\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [UNSEEN 12] Message 12 is first unseen\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [UIDVALIDITY 3857529045] UIDs valid\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK [READ-WRITE] SELECT completed\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(CImapSession::EServerStateSelected, iImapSession->ServerState());
	ASSERT_EQUALS(folderInfo, iImapSession->SelectedFolderInfo());
	folderInfo = NULL;
	}
	
void CTestImapIdle::TearDownL()
	{
	delete iImapSession;
	iImapSession = NULL;
	
	delete iActiveWaiter;
	iActiveWaiter = NULL;
	
	delete iOutputStream;
	iOutputStream = NULL;
	
	delete iInputStream;
	iInputStream = NULL;

	CImapUtils::Delete();
	}

/**
Simple test that enters idle, then sends DONE to leave idle.
No idle data is received.
*/
void CTestImapIdle::TestIdleEnterDoneL()
	{
	INFO_PRINTF1(_L("TestIdleEnterDoneL"));
	
	iInputStream->ResetInputStrings();	
	iInputStream->AppendInputStringL(_L8("+ idling\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK IDLE terminated\r\n"));
		
	iImapSession->EnterIdleL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iImapSession->DoneIdle(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Simple test that enters idle, then sends DONE to leave idle.
No idle data is received.
*/
void CTestImapIdle::TestIdleEnterDoneFlatL()
	{
	INFO_PRINTF1(_L("TestIdleEnterDoneFlatL"));
	
	iInputStream->ResetInputStrings();	
	iInputStream->AppendInputStringL(_L8("+ idling\r\n3 OK IDLE terminated\r\n"));
			
	iImapSession->EnterIdleL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iImapSession->DoneIdle(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Simple test that enters idle, 
waits for some data (the data is received)
then sends DONE to leave idle.
*/
void CTestImapIdle::TestIdleEnterWaitDoneL()
	{
	INFO_PRINTF1(_L("TestIdleEnterWaitDoneL"));
	
	iInputStream->ResetInputStrings();	
	iInputStream->AppendInputStringL(_L8("+ idling\r\n"));
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK IDLE terminated\r\n"));
			
	iImapSession->EnterIdleL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iImapSession->DoneIdle(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Simple test that enters idle, 
waits for some data, but "cancels" before any is received.
then sends DONE to leave idle.
*/
void CTestImapIdle::TestIdleEnterWaitCancelDoneL()
	{
	INFO_PRINTF1(_L("TestIdleEnterWaitCancelDoneL"));
	
	iInputStream->ResetInputStrings();	
	iInputStream->AppendInputStringL(_L8("+ idling\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK IDLE terminated\r\n"));
			
	iImapSession->EnterIdleL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->SetActiveAndCancel(*this);
			
	iImapSession->DoneIdle(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	CImapFolderInfo* info = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(info);
	ASSERT_EQUALS(info->ExpungedMessages().Count(), 0);
	
	INFO_PRINTF1(_L("Complete"));
	}

/**
Simple test that enters idle, 
waits for some data, but "cancels" before it is received.
then sends DONE to leave idle.
*/
void CTestImapIdle::TestIdleEnterWaitCancelDataDoneL()
	{
	INFO_PRINTF1(_L("TestIdleEnterWaitCancelDataDoneL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("+ idling\r\n"));
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK IDLE terminated\r\n"));
			
	iImapSession->EnterIdleL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->SetActiveAndCancel(*this);
	
	iImapSession->DoneIdle(iActiveWaiter->iStatus); // possibly don't need aStatus for DoneIdle()
	iActiveWaiter->WaitActive();
	
	CImapFolderInfo* info = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(info);
	ASSERT_EQUALS(info->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(info->ExpungedMessages()[0], (TUint)22);
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Lots of data is received here
*/
void CTestImapIdle::TestIdleSequenceL()
	{
	INFO_PRINTF1(_L("TestIdleSequenceL"));
	
	CImapFolderInfo* selectedFolderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(selectedFolderInfo);
	
	iInputStream->ResetInputStrings();
	
	// initial summary information
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("* 23 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 3 RECENT\r\n"));
	iInputStream->AppendInputStringL(_L8("* 14 FETCH (FLAGS (Seen Deleted))\r\n")); // this line should be ignored by the parser
	iInputStream->AppendInputStringL(_L8("+ idling\r\n"));
	// events come in one at a time
	iInputStream->AppendInputStringL(_L8("* 321 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("* 12 EXPUNGE\r\n"));
	// done here
	iInputStream->AppendInputStringL(_L8("* 27 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK IDLE terminated\r\n"));
			
	iImapSession->EnterIdleL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)22);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 23);
	ASSERT_EQUALS(selectedFolderInfo->Recent(), 3);
	
	selectedFolderInfo->ResetExpungedMessages();
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 0);
	
	// receive the 1st expunge event.
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)321);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 22);	
	
	// receive the 2nd expunge event.
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 2);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)321);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[1], (TUint)12);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 21);	
	
	iImapSession->DoneIdle(iActiveWaiter->iStatus); // possibly don't need aStatus for DoneIdle()
	iActiveWaiter->WaitActive();
		
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 27);
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Lots of data is received here, both before and after continuation.
A wait for idle event is canceled.
*/
void CTestImapIdle::TestIdleSequenceWithCancelL()
	{
	INFO_PRINTF1(_L("TestIdleSequenceWithCancelL"));
	
	CImapFolderInfo* selectedFolderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(selectedFolderInfo);
	
	iInputStream->ResetInputStrings();
	
	// initial summary information
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("* 23 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 3 RECENT\r\n"));
	iInputStream->AppendInputStringL(_L8("* 14 FETCH (FLAGS (Seen Deleted))\r\n")); // this line should be ignored by the parser
	iInputStream->AppendInputStringL(_L8("+ idling\r\n"));
	// events come in one at a time
	iInputStream->AppendInputStringL(_L8("* 321 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("* 12 EXPUNGE\r\n"));
	// cancel & done here
	iInputStream->AppendInputStringL(_L8("* 27 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 100 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK IDLE terminated\r\n"));
		
	iImapSession->EnterIdleL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)22);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 23);
	ASSERT_EQUALS(selectedFolderInfo->Recent(), 3);
	
	selectedFolderInfo->ResetExpungedMessages();
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 0);
	
	// receive the 1st expunge event.
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)321);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 22);	
	
	// receive the 2nd expunge event.
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 2);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)321);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[1], (TUint)12);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 21);	
	
	selectedFolderInfo->ResetExpungedMessages();
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 0);
	
	// start listening for more data, but then cancel
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);	
	iActiveWaiter->SetActiveAndCancel(*this);
	
	iImapSession->DoneIdle(iActiveWaiter->iStatus); // possibly don't need aStatus for DoneIdle()
	iActiveWaiter->WaitActive();
		
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)100);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 26); // == 27 - 1
	
	INFO_PRINTF1(_L("Complete"));
	}

/**
Lots of data is received here, both before and after continuation.
Data is delivered arrives in blocks that need to be separated by the session
while also sending one notification per line during the wait phase.
*/
void CTestImapIdle::TestIdleSequenceWithCancelAndFlatL()
	{
	INFO_PRINTF1(_L("TestIdleSequenceWithCancelAndFlatL"));
	
	CImapFolderInfo* selectedFolderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(selectedFolderInfo);
	
	iInputStream->ResetInputStrings();
	
	// initial summary information
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("* 23 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 3 RECENT\r\n"));
	iInputStream->AppendInputStringL(_L8("* 14 FETCH (FLAGS (Seen Deleted))\r\n")); // this line should be ignored by the parser
	iInputStream->AppendInputStringL(_L8("+ idling\r\n"));
	// events come in as 2 split blocks but are processed one at a time.
	iInputStream->AppendInputStringL(_L8("* 321 EXPUNGE\r\n* 12 EXPUNGE\r\n* 27 EXISTS\r\n* 100 EXPU"));
	iInputStream->AppendInputStringL(_L8("NGE\r\n3 OK IDLE terminated\r\n"));
		
	iImapSession->EnterIdleL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)22);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 23);
	ASSERT_EQUALS(selectedFolderInfo->Recent(), 3);
	
	selectedFolderInfo->ResetExpungedMessages();
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 0);
	
	// receive the 1st expunge event.
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)321);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 22);	
	
	// receive the 2nd expunge event.
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 2);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)321);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[1], (TUint)12);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 21);	
	
	selectedFolderInfo->ResetExpungedMessages();
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 0);
	
	// start listening for more data, but then cancel
	iImapSession->WaitForIdleEvent(iActiveWaiter->iStatus);
	iActiveWaiter->SetActiveAndCancel(*this);
	
		
	iImapSession->DoneIdle(iActiveWaiter->iStatus); // possibly don't need aStatus for DoneIdle()
	iActiveWaiter->WaitActive();
		
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)100);
	ASSERT_EQUALS(selectedFolderInfo->Exists(), 26); // == 27 - 1
	
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapIdle::DoCancel()
	{
	if (iImapSession)
		{
		iImapSession->Cancel();
		}
	}

CTestSuite* CTestImapIdle::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestIdleEnterDoneL);
	ADD_ASYNC_TEST_STEP(TestIdleEnterDoneFlatL);
	ADD_ASYNC_TEST_STEP(TestIdleEnterWaitDoneL);
	ADD_ASYNC_TEST_STEP(TestIdleEnterWaitCancelDoneL);
	ADD_ASYNC_TEST_STEP(TestIdleEnterWaitCancelDataDoneL);
	ADD_ASYNC_TEST_STEP(TestIdleSequenceL);
	ADD_ASYNC_TEST_STEP(TestIdleSequenceWithCancelL);
	ADD_ASYNC_TEST_STEP(TestIdleSequenceWithCancelAndFlatL);
	END_SUITE;
	}
