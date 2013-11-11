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

#include "ctestimapcancelandflush.h"

#include "cfakeoutputstream.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimapfolderinfo.h"
#include "cimaplistfolderinfo.h"
#include "cimaputils.h"

CTestImapCancelAndFlush::CTestImapCancelAndFlush()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapCancelAndFlush::~CTestImapCancelAndFlush()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapCancelAndFlush::SetupL()
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
	
void CTestImapCancelAndFlush::TearDownL()
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
void CTestImapCancelAndFlush::TestListInPartsL()
	{
	INFO_PRINTF1(_L("TestListInPartsL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Drafts\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" INBOX\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder1\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder2\r\n"));
		
	iInputStreamNotifyOp = ENotifyOpListInParts;
	iInputStream->NotifyWhenStreamIsEmpty(*this);
	
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L(""), _L("%"), folderList);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(folderList.Count(), 7);

	folderList.ResetAndDestroy();
 	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCancelAndFlush::TestCancelAndFlushL()
	{
	INFO_PRINTF1(_L("TestCancelAndFlushL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Drafts\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" INBOX\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder1\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder2\r\n"));
	
	iInputStreamNotifyOp = ENotifyOpCancelWaitActive;
	iInputStream->NotifyWhenStreamIsEmpty(*this);
	
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L(""), _L("%"), folderList);
	iActiveWaiter->WaitActive(KErrCancel); // returns because a cancel occured.
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder3\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder4\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder5\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK LIST completed\r\n"));
		
	iImapSession->FlushCancelledCommand(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	folderList.ResetAndDestroy();
 	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCancelAndFlush::TestCancelAndFlushWithTimeoutL()
	{
	INFO_PRINTF1(_L("TestCancelAndFlushWithTimeoutL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Drafts\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" INBOX\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder1\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder2\r\n"));
		
	iInputStreamNotifyOp = ENotifyOpCancelWaitActive;
	iInputStream->NotifyWhenStreamIsEmpty(*this);
	
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L(""), _L("%"), folderList);
	iActiveWaiter->WaitActive(KErrCancel); // returns because a cancel occured.
		
	// Unlike TestCancelAndFlushL(), the remainder of the response is not 
	// provided fake input stream.  So FlushCancelledCommand should time out.
	
	iImapSession->FlushCancelledCommand(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive(KErrImapFlushTimeout);
	
	folderList.ResetAndDestroy();
 	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCancelAndFlush::OnInputStreamIsEmptyL()
	{
	switch (iInputStreamNotifyOp)
		{
		case ENotifyOpCancelWaitActive:
			{
			iActiveWaiter->CancelWaitActive(*this);
			}
			break;
		case ENotifyOpListInParts:
			{
			iInputStream->ResetInputStrings();
			iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder3\r\n"));
			iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder4\r\n"));
			iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Folder5\r\n"));
			iInputStream->AppendInputStringL(_L8("3 OK LIST completed\r\n"));
			}
			break;
		}
	}
	
void CTestImapCancelAndFlush::DoCancel()
	{
	iImapSession->Cancel();
	}
	
CTestSuite* CTestImapCancelAndFlush::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestListInPartsL);
	ADD_ASYNC_TEST_STEP(TestCancelAndFlushL);
	ADD_ASYNC_TEST_STEP(TestCancelAndFlushWithTimeoutL);
	END_SUITE;
	}
