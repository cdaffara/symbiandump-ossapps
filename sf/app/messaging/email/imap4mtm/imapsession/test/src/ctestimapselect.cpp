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

#include "ctestimapselect.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"

#include "cimapfolderinfo.h"
#include "cimapsessionconsts.h"
#include "cimaputils.h"


CTestImapSelect::CTestImapSelect()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapSelect::~CTestImapSelect()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapSelect::SetupL()
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
	}
	
void CTestImapSelect::TearDownL()
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
Tests the example given in section 6.3.1 of RFC3501
*/
void CTestImapSelect::TestSelectBasicL()
	{
	INFO_PRINTF1(_L("TestSelectBasicL"));
		
	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 172 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 1 RECENT\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [UNSEEN 12] Message 12 is first unseen\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [UIDVALIDITY 3857529045] UIDs valid\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [UIDNEXT 4392] Predicted next UID\r\n"));
	iInputStream->AppendInputStringL(_L8("* FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft)\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [PERMANENTFLAGS (\\Deleted \\Seen \\*)] Limited\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK [READ-WRITE] SELECT completed\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(CImapSession::EServerStateSelected, iImapSession->ServerState());
	ASSERT_EQUALS(folderInfo, iImapSession->SelectedFolderInfo());
	
	ASSERT_EQUALS(folderInfo->ExpungedMessages().Count(), 0);
	ASSERT_EQUALS(folderInfo->Exists(), 172);
	ASSERT_EQUALS(folderInfo->Recent(), 1);
	ASSERT_EQUALS(folderInfo->Unseen(), 12u);
	ASSERT_EQUALS(folderInfo->UidValidity(), 3857529045u);
	ASSERT_EQUALS(folderInfo->UidNext(), 4392u);
	ASSERT_TRUE(folderInfo->QueryFlag(CImapFolderInfo::EAnswered));
	ASSERT_TRUE(folderInfo->QueryFlag(CImapFolderInfo::EFlagged));
	ASSERT_TRUE(folderInfo->QueryFlag(CImapFolderInfo::EDeleted));
	ASSERT_TRUE(folderInfo->QueryFlag(CImapFolderInfo::ESeen));
	ASSERT_TRUE(folderInfo->QueryFlag(CImapFolderInfo::EDraft));
	ASSERT_TRUE(folderInfo->GetReadWrite());
	
	folderInfo = NULL;
	
	INFO_PRINTF1(_L("Complete"));
	}

/**
Tests a corrupt select response
*/
void CTestImapSelect::TestSelectCorruptNo2ndPartL()
	{
	INFO_PRINTF1(_L("TestSelectCorruptNo2ndPartL"));
		
	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive(); // parser ignores the untagged response.

	ASSERT_EQUALS(CImapSession::EServerStateSelected, iImapSession->ServerState());
		
	folderInfo = NULL;
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Tests that an untagged NO response will be ignored.
RFC 4315 - UIDPLUS gives an example of an untagged NO response.
We don't support UIDPLUS, so we should ignore this response.
*/
void CTestImapSelect::TestSelectUntaggedNoL()
	{
	INFO_PRINTF1(_L("TestSelectUntaggedNoL"));
		
	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* NO [UIDNOTSTICKY] Non-persistent UIDs\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive();
		
	// Since the untagged NO was ignored, the session should remain selected
	ASSERT_EQUALS(CImapSession::EServerStateSelected, iImapSession->ServerState());
		
	folderInfo = NULL;
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Tests a corrupt select response
*/
void CTestImapSelect::TestSelectCorruptNoResponseTextCodeL()
	{
	INFO_PRINTF1(_L("TestSelectCorruptNoResponseTextCodeL"));
		
	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK UNSEEN not within square brackets\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive(KErrImapCorrupt);
		
	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());
		
	folderInfo = NULL;
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Tests a corrupt select response
*/
void CTestImapSelect::TestSelectCorruptEmptyResponseCodeTextL()
	{
	INFO_PRINTF1(_L("TestSelectCorruptEmptyResponseCodeTextL"));
		
	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK [] Predicted next UID\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive(KErrImapCorrupt);
	
	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());
	
	folderInfo = NULL;
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Tests a corrupt select response
*/
void CTestImapSelect::TestSelectCorruptNoFlagsL()
	{
	INFO_PRINTF1(_L("TestSelectCorruptNoFlagsL"));
		
	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* FLAGS ()\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive();
	
	// strictly speaking, no flags is corrupt, but we ignore this one.
	ASSERT_EQUALS(CImapSession::EServerStateSelected, iImapSession->ServerState());
	
	folderInfo = NULL;
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Tests a corrupt select response
*/
void CTestImapSelect::TestSelectCorruptFlagMissingCloseBracketL()
	{
	INFO_PRINTF1(_L("TestSelectCorruptFlagMissingCloseBracketL"));
		
	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* FLAGS (\\Deleted\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive(KErrImapCorrupt);
		
	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());
		
	folderInfo = NULL;
	
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Tests a corrupt select response
*/
void CTestImapSelect::TestSelectCorruptTaggedOkWrongResponseCodeL()
	{
	INFO_PRINTF1(_L("TestSelectCorruptTaggedOkWrongResponseCodeL"));
		
	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 172 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 1 RECENT\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [UNSEEN 12] Message 12 is first unseen\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [UIDVALIDITY 3857529045] UIDs valid\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [UIDNEXT 4392] Predicted next UID\r\n"));
	iInputStream->AppendInputStringL(_L8("* FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft)\r\n"));
	iInputStream->AppendInputStringL(_L8("* OK [PERMANENTFLAGS (\\Deleted \\Seen \\*)] Limited\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK [WIBBLE] should be READ-WRITE\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));
	CleanupStack::Pop(folderInfo);
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive();
	
	// Unrecognised response text code ignored, because it is tagged.
	ASSERT_EQUALS(CImapSession::EServerStateSelected, iImapSession->ServerState());
		
	folderInfo = NULL;
	
	INFO_PRINTF1(_L("Complete"));
	}
	
CTestSuite* CTestImapSelect::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestSelectBasicL);
	ADD_ASYNC_TEST_STEP(TestSelectCorruptNo2ndPartL);
	ADD_ASYNC_TEST_STEP(TestSelectUntaggedNoL);
	ADD_ASYNC_TEST_STEP(TestSelectCorruptNoResponseTextCodeL);
	ADD_ASYNC_TEST_STEP(TestSelectCorruptEmptyResponseCodeTextL);
	ADD_ASYNC_TEST_STEP(TestSelectCorruptNoFlagsL);
	ADD_ASYNC_TEST_STEP(TestSelectCorruptFlagMissingCloseBracketL);
	ADD_ASYNC_TEST_STEP(TestSelectCorruptTaggedOkWrongResponseCodeL);
	END_SUITE;
	}


