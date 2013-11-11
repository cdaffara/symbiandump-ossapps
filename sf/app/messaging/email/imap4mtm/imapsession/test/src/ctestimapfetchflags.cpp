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

#include "ctestimapfetchflags.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimapfolderinfo.h"
#include "cimaputils.h"

CTestImapFetchFlags::CTestImapFetchFlags()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapFetchFlags::~CTestImapFetchFlags()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapFetchFlags::SetupL()
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
	
void CTestImapFetchFlags::TearDownL()
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
void CTestImapFetchFlags::TestFetchFlagsWithFlagsL()
	{
	INFO_PRINTF1(_L("TestFetchFlagsWithFlagsL"));
	iInputStream->ResetInputStrings();

	iInputStream->AppendInputStringL(_L8("* 12 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 18 FLAGS (\\Seen \\Recent))\r\n"));
	iInputStream->AppendInputStringL(_L8("* 2 FETCH (UID 19 FLAGS (\\Seen \\Draft \\Recent))\r\n"));
	iInputStream->AppendInputStringL(_L8("* 3 FETCH (FLAGS (\\Seen \\Deleted) UID 20)\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK completed\r\n"));
		
	RArrayMessageFlagInfo messageFlagInfo;
	iImapSession->FetchFlagsL(iActiveWaiter->iStatus, _L8("18:20"), messageFlagInfo);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(messageFlagInfo.Count(), 3);
	ASSERT_EQUALS(messageFlagInfo[0].MessageUid(), (TUint)18);
	ASSERT_TRUE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_TRUE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::ERecent));
	
	ASSERT_EQUALS(messageFlagInfo[1].MessageUid(), (TUint)19);
	ASSERT_TRUE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_FALSE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_TRUE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_TRUE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::ERecent));
	
	ASSERT_EQUALS(messageFlagInfo[2].MessageUid(), (TUint)20);
	ASSERT_TRUE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_TRUE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::ERecent));

	CImapFolderInfo* folderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(folderInfo);
	ASSERT_FALSE(folderInfo->MessageFlagsChanged());	

	messageFlagInfo.Reset();

	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Tests that unknown extension flags are succsfully ignored without crashing
*/
void CTestImapFetchFlags::TestFetchFlagsWithUnknownExtensionFlagsL()
	{
	INFO_PRINTF1(_L("TestFetchFlagsWithUnknownExtensionFlagsL"));
	iInputStream->ResetInputStrings();

	iInputStream->AppendInputStringL(_L8("* 12 EXISTS\r\n"));
	// Try a couple of flag-extension's.
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 18 FLAGS (\\FlagExtension \\Recent))\r\n"));
	iInputStream->AppendInputStringL(_L8("* 2 FETCH (UID 19 FLAGS (\\Seen \\Unread \\Recent))\r\n"));
	// Try a flag-keyword's (defined as an atom without the forward slash)
	iInputStream->AppendInputStringL(_L8("* 3 FETCH (FLAGS (\\Seen Keyword) UID 20)\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK completed\r\n"));
		
	RArrayMessageFlagInfo messageFlagInfo;
	iImapSession->FetchFlagsL(iActiveWaiter->iStatus, _L8("18:20"), messageFlagInfo);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(messageFlagInfo.Count(), 3);
	ASSERT_EQUALS(messageFlagInfo[0].MessageUid(), (TUint)18);
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_FALSE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_TRUE(messageFlagInfo[0].QueryFlag(TMessageFlagInfo::ERecent));
	
	ASSERT_EQUALS(messageFlagInfo[1].MessageUid(), (TUint)19);
	ASSERT_TRUE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_FALSE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_FALSE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_TRUE(messageFlagInfo[1].QueryFlag(TMessageFlagInfo::ERecent));
	
	ASSERT_EQUALS(messageFlagInfo[2].MessageUid(), (TUint)20);
	ASSERT_TRUE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_FALSE(messageFlagInfo[2].QueryFlag(TMessageFlagInfo::ERecent));

	messageFlagInfo.Reset();

	INFO_PRINTF1(_L("Complete"));
	}	
	
/**
Check that a flags response without a UID is classed as an unsolicitied
response and is not stored in the message flag info.
*/
void CTestImapFetchFlags::TestFetchFlagsWithMissingUidL()
	{
	INFO_PRINTF1(_L("TestFetchFlagsWithMissingUidL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (FLAGS (\\Seen \\Recent))\r\n")); // Missing UID
	iInputStream->AppendInputStringL(_L8("3 OK completed\r\n"));

	RArrayMessageFlagInfo messageFlagInfo;
	iImapSession->FetchFlagsL(iActiveWaiter->iStatus, _L8("18:20"), messageFlagInfo);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(messageFlagInfo.Count(), 0);
	messageFlagInfo.Reset();

	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Check that corrupt data is detected without causing a panic and without hanging.
*/
void CTestImapFetchFlags::TestFetchFlagsCorruptMissingFlagsL()
	{
	INFO_PRINTF1(_L("TestFetchFlagsCorruptMissingFlagsL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 2 FETCH (UID 19)\r\n")); // Missing FLAGS
	iInputStream->AppendInputStringL(_L8("3 OK completed\r\n"));
		
	RArrayMessageFlagInfo messageFlagInfo;
	iImapSession->FetchFlagsL(iActiveWaiter->iStatus, _L8("18:20"), messageFlagInfo);
	iActiveWaiter->WaitActive(KErrImapCorrupt);

	messageFlagInfo.Reset();

	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Check that corrupt data is detected without causing a panic and without hanging.
*/
void CTestImapFetchFlags::TestFetchFlagsCorruptNoDataItemsL()
	{
	INFO_PRINTF1(_L("TestFetchFlagsCorruptNoDataItemsL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 3 FETCH ()\r\n")); // No data items
	iInputStream->AppendInputStringL(_L8("3 OK completed\r\n"));
		
	RArrayMessageFlagInfo messageFlagInfo;
	iImapSession->FetchFlagsL(iActiveWaiter->iStatus, _L8("18:20"), messageFlagInfo);
	iActiveWaiter->WaitActive(KErrImapCorrupt);
	
	messageFlagInfo.Reset();

	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Check that corrupt data is detected without causing a panic and without hanging.
*/
void CTestImapFetchFlags::TestFetchFlagsCorruptNoFetchDataL()
	{
	INFO_PRINTF1(_L("TestFetchFlagsCorruptNoFetchDataL"));
	iInputStream->ResetInputStrings();

	iInputStream->AppendInputStringL(_L8("* 3 FETCH\r\n")); // No FETCH data at all
	iInputStream->AppendInputStringL(_L8("3 OK completed\r\n"));
		
	RArrayMessageFlagInfo messageFlagInfo;
	iImapSession->FetchFlagsL(iActiveWaiter->iStatus, _L8("18:20"), messageFlagInfo);
	iActiveWaiter->WaitActive(KErrImapCorrupt);
	
	messageFlagInfo.Reset();

	INFO_PRINTF1(_L("Complete"));
	}
	
/**
Check that corrupt data is detected without causing a panic and without hanging.
*/
void CTestImapFetchFlags::TestFetchFlagsCorruptMismatchedTagL()
	{
	INFO_PRINTF1(_L("TestFetchFlagsCorruptMismatchedTagL"));
	iInputStream->ResetInputStrings();

	iInputStream->AppendInputStringL(_L8("* 1 FETCH (FLAGS (\\Seen \\Recent) UID 18)\r\n")); // OK
	iInputStream->AppendInputStringL(_L8("4 OK completed\r\n")); // Mismatched
		
	RArrayMessageFlagInfo messageFlagInfo;
	iImapSession->FetchFlagsL(iActiveWaiter->iStatus, _L8("18:20"), messageFlagInfo);
	iActiveWaiter->WaitActive(KErrImapCorrupt);
	
	messageFlagInfo.Reset();

	INFO_PRINTF1(_L("Complete"));
	}
	
CTestSuite* CTestImapFetchFlags::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestFetchFlagsWithFlagsL);
	ADD_ASYNC_TEST_STEP(TestFetchFlagsWithUnknownExtensionFlagsL);
	ADD_ASYNC_TEST_STEP(TestFetchFlagsWithMissingUidL);
	ADD_ASYNC_TEST_STEP(TestFetchFlagsCorruptMissingFlagsL);
	ADD_ASYNC_TEST_STEP(TestFetchFlagsCorruptNoDataItemsL);
	ADD_ASYNC_TEST_STEP(TestFetchFlagsCorruptNoFetchDataL);
	ADD_ASYNC_TEST_STEP(TestFetchFlagsCorruptMismatchedTagL);
	END_SUITE;
	}
