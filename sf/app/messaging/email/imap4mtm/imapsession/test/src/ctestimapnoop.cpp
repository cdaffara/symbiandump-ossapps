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

#include "ctestimapnoop.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimaputils.h"
#include "cimapfolderinfo.h"

CTestImapNoop::CTestImapNoop()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapNoop::~CTestImapNoop()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapNoop::SetupL()
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
	
	ResetFolderInfo(*folderInfo);	
	folderInfo = NULL;
	}
	
void CTestImapNoop::TearDownL()
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
void CTestImapNoop::TestNoopWithFlagsL()
	{
	INFO_PRINTF1(_L("TestNoopWithFlagsL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 22 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("* 23 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 3 RECENT\r\n"));
	iInputStream->AppendInputStringL(_L8("* 14 FETCH (FLAGS (Seen Deleted))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK NOOP completed\r\n"));
	
	iImapSession->NoopL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("NOOP Response"));
	
	CImapFolderInfo* folderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(folderInfo);
	
	TPtrC flag;
	RArray <TUint> expungedMessages = folderInfo->ExpungedMessages();
	ASSERT_EQUALS(expungedMessages.Count(), 1);
	ASSERT_EQUALS(expungedMessages[0], 22u);
	ASSERT_EQUALS(folderInfo->Exists(), 23);
	ASSERT_EQUALS(folderInfo->Recent(), 3);
	ASSERT_TRUE(folderInfo->MessageFlagsChanged());
	
 	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapNoop::TestNoopWithoutFlagsL()
	{
	INFO_PRINTF1(_L("TestNoopWithoutFlagsL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 12 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("* 10 EXPUNGE\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK NOOP completed\r\n"));
			
	iImapSession->NoopL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("NOOP Response"));
	
	CImapFolderInfo* folderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(folderInfo);
	
	TPtrC flag;
	RArray <TUint> expungedMessages = folderInfo->ExpungedMessages();
	ASSERT_EQUALS(expungedMessages.Count(), 2);
	ASSERT_EQUALS(expungedMessages[0], 12u);
	ASSERT_EQUALS(expungedMessages[1], 10u);
	ASSERT_EQUALS(folderInfo->Exists(), 21); // select said there were 23 but now 2 have been expunged.
	ASSERT_EQUALS(folderInfo->Recent(), 0);
	ASSERT_FALSE(folderInfo->MessageFlagsChanged());
	
 	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Protocol Controller resets folder info after a sync, so that it can detect changes received later
*/
void CTestImapNoop::ResetFolderInfo(CImapFolderInfo& folderInfo)
	{
	folderInfo.ResetExpungedMessages();
	folderInfo.SetRecent(0);
	folderInfo.SetMessageFlagsChanged(EFalse);
	}
	
CTestSuite* CTestImapNoop::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestNoopWithFlagsL);
	ADD_ASYNC_TEST_STEP(TestNoopWithoutFlagsL);
	END_SUITE;
	}
