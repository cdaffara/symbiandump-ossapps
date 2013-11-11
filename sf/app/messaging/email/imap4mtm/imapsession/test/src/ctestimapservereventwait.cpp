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

#include "ctestimapservereventwait.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"

#include "cimapfolderinfo.h"
#include "cimaputils.h"

CTestImapServerEventWait::CTestImapServerEventWait()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapServerEventWait::~CTestImapServerEventWait()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapServerEventWait::SetupL()
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
	
void CTestImapServerEventWait::TearDownL()
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


// Tests
void CTestImapServerEventWait::TestSimpleL()
	{
	INFO_PRINTF1(_L("TestSimpleL"));
	
	iInputStream->ResetInputStrings();	
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n"));
				
	iImapSession->WaitForServerEventL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	CImapFolderInfo* selectedFolderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(selectedFolderInfo);
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 1);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)22);
		
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapServerEventWait::TestTwoEventsL()
	{
	INFO_PRINTF1(_L("TestTwoEventsL"));
	
	iInputStream->ResetInputStrings();	
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n* 321 EXPUNGE\r\n"));
				
	iImapSession->WaitForServerEventL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iImapSession->WaitForServerEventL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	CImapFolderInfo* selectedFolderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(selectedFolderInfo);
	
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages().Count(), 2);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[0], (TUint)22);
	ASSERT_EQUALS(selectedFolderInfo->ExpungedMessages()[1], (TUint)321);
		
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapServerEventWait::TestCancelAfterNoEventL()
	{
	INFO_PRINTF1(_L("TestCancelAfterNoEventL"));
	
	iInputStream->ResetInputStrings();	
					
	iImapSession->WaitForServerEventL(iActiveWaiter->iStatus);
	iActiveWaiter->SetActiveAndCancel(*this);
	
	iImapSession->FlushCancelledCommand(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	// Check that some other command can now be run
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK noop\r\n"));
	
	iImapSession->NoopL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();	
		
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapServerEventWait::TestCancelAfterWholeEventL()
	{
	INFO_PRINTF1(_L("TestCancelAfterWholeEventL"));

	iInputStream->ResetInputStrings();	
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n"));
	
	iImapSession->WaitForServerEventL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
					
	iImapSession->WaitForServerEventL(iActiveWaiter->iStatus);
	iActiveWaiter->SetActiveAndCancel(*this);
	
	iImapSession->FlushCancelledCommand(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	// Check that some other command can now be run
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK noop\r\n"));
	
	iImapSession->NoopL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();	
		
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapServerEventWait::TestCancelDuringEventL()
	{
	INFO_PRINTF1(_L("TestCancelDuringEventL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* XLITERAL {10}\r\n")); // simulate an extension response that happens to return a literal
	
	iInputStreamNotifyOp = ENotifyOpCompleteXLiteralAndCancel;
	iInputStream->NotifyWhenStreamIsEmpty(*this);
					
	iImapSession->WaitForServerEventL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive(KErrCancel); // this operation will be cancelled by the OnInputStreamIsEmptyL() callback
	
	iImapSession->FlushCancelledCommand(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	// Check that some other command can now be run
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK noop\r\n"));
	
	iImapSession->NoopL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();	
		
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapServerEventWait::OnInputStreamIsEmptyL()
	{
	switch (iInputStreamNotifyOp)
		{
		case ENotifyOpCompleteXLiteralAndCancel:
			{
			iInputStream->ResetInputStrings();
			iInputStream->AppendInputStringL(_L8("1234567890\r\n"));
			iActiveWaiter->CancelWaitActive(*this);
			}
			break;
		}
	}	

void CTestImapServerEventWait::DoCancel()
	{
	if (iImapSession)
		{
		iImapSession->Cancel();
		}
	}
	
CTestSuite* CTestImapServerEventWait::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestSimpleL);
	ADD_ASYNC_TEST_STEP(TestTwoEventsL);
	ADD_ASYNC_TEST_STEP(TestCancelAfterNoEventL);
	ADD_ASYNC_TEST_STEP(TestCancelAfterWholeEventL);
	ADD_ASYNC_TEST_STEP(TestCancelDuringEventL);
	END_SUITE;
	}
