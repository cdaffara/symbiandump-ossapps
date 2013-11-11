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

#include "ctestimapstore.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimaputils.h"
#include "cimapfolderinfo.h"

_LIT(KMessageIdAndFlags, "%d %S");

CTestImapStore::CTestImapStore()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapStore::~CTestImapStore()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapStore::SetupL()
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
	
void CTestImapStore::TearDownL()
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
void CTestImapStore::TestStoreWithFlagsL()
	{
	INFO_PRINTF1(_L("TestStoreWithFlagsL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 2 FETCH (FLAGS (\\Deleted \\Seen) UID 2)\r\n"));
	iInputStream->AppendInputStringL(_L8("* 3 FETCH (FLAGS (\\Deleted) UID 3)\r\n"));
	iInputStream->AppendInputStringL(_L8("* 4 FETCH (FLAGS (\\Deleted \\Flagged \\Seen) UID 4)\r\n"));
	iInputStream->AppendInputStringL(_L8("* 5 FETCH (FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft) UID 5)\r\n"));
	iInputStream->AppendInputStringL(_L8("* 6 FETCH (FLAGS (\\Answered \\Flagged \\Deleted \\Seen \\Draft))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK STORE completed\r\n"));
	
	RArrayMessageFlagInfo messageFlagInfo;
	iImapSession->StoreL(iActiveWaiter->iStatus, _L8("2:5"), _L8("+FLAGS"), _L8("(Deleted)"), 0, messageFlagInfo);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("FLAGS LIST"));
	
	TInt countMessageFlagInfo = messageFlagInfo.Count();
	ASSERT_EQUALS(countMessageFlagInfo, 4);
	
	for (TInt i = 0; i < countMessageFlagInfo; ++i)
		{
		// create account Id
		TMessageFlagInfo entry = messageFlagInfo[i];
		TUint messageUid = entry.MessageUid();
		TBuf<100> mesageUidAndFlags;
		TPtrC flag;
		if (entry.QueryFlag(TMessageFlagInfo::EDeleted) && entry.QueryFlag(TMessageFlagInfo::EFlagged) 
			&& entry.QueryFlag(TMessageFlagInfo::ESeen) && entry.QueryFlag(TMessageFlagInfo::EDraft) && entry.QueryFlag(TMessageFlagInfo::EAnswered))
			{
			// Check the other flags are false
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::ERecent));

			// Check that this is the expected entry
			ASSERT_EQUALS(i, 3);
			
			flag.Set(_L("Answered Flagged Deleted Seen Draft"));
			mesageUidAndFlags.AppendFormat(KMessageIdAndFlags, messageUid, &flag);
			INFO_PRINTF1(mesageUidAndFlags);	
			}
		else if (entry.QueryFlag(TMessageFlagInfo::EDeleted) && entry.QueryFlag(TMessageFlagInfo::EFlagged) && entry.QueryFlag(TMessageFlagInfo::ESeen))
			{
			// Check the other flags are false
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::EAnswered));			
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::EDraft));
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::ERecent));

			// Check that this is the expected entry
			ASSERT_EQUALS(i, 2);
			
			flag.Set(_L("Deleted Flagged Seen"));
			mesageUidAndFlags.AppendFormat(KMessageIdAndFlags, messageUid, &flag);
			INFO_PRINTF1(mesageUidAndFlags);	
			}
		else if (entry.QueryFlag(TMessageFlagInfo::EDeleted) && entry.QueryFlag(TMessageFlagInfo::ESeen))
			{
			// Check the other flags are false
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::EAnswered));
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::EFlagged));			
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::EDraft));
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::ERecent));

			// Check that this is the expected entry
			ASSERT_EQUALS(i, 0);
			
			flag.Set(_L("Deleted Seen"));
			mesageUidAndFlags.AppendFormat(KMessageIdAndFlags, messageUid, &flag);
			INFO_PRINTF1(mesageUidAndFlags);	
			}
		else if (entry.QueryFlag(TMessageFlagInfo::EDeleted))
			{
			// Check the other flags are false
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::ESeen));
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::EAnswered));
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::EFlagged));
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::EDraft));
			ASSERT_FALSE(entry.QueryFlag(TMessageFlagInfo::ERecent));

			// Check that this is the expected entry
			ASSERT_EQUALS(i, 1);
			
			flag.Set(_L("Deleted"));
			mesageUidAndFlags.AppendFormat(KMessageIdAndFlags, messageUid, &flag);
			INFO_PRINTF1(mesageUidAndFlags);	
			}
		}
	messageFlagInfo.Reset();
	messageFlagInfo.Close();
	INFO_PRINTF1(_L("Complete"));
	}
	
CTestSuite* CTestImapStore::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestStoreWithFlagsL);
	END_SUITE;
	}
