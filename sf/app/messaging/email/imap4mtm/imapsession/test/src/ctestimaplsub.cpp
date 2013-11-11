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

#include "ctestimaplsub.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimaputils.h"

CTestImapLsub::CTestImapLsub()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapLsub::~CTestImapLsub()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapLsub::SetupL()
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
	
void CTestImapLsub::TearDownL()
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
void CTestImapLsub::TestLsubWithFlagsL()
	{
	INFO_PRINTF1(_L("TestLsubWithFlagsL"));
	iInputStream->ResetInputStrings();
	
	iInputStream->AppendInputStringL(_L8("* LSUB (\\Marked) \".\" #news.comp.mail.mime\r\n"));
	iInputStream->AppendInputStringL(_L8("* LSUB (\\Unmarked \\Noinferiors) \".\" #news.comp.mail.misc\r\n"));
	iInputStream->AppendInputStringL(_L8("* LSUB (\\NoSelect) \".\" #news.comp.mail\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK LSUB completed\r\n"));
	
	RArrayImapListFolderInfo folderList;
	iImapSession->LsubL(iActiveWaiter->iStatus, _L("#news."), _L("comp.mail.*"), folderList);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("LSUB Response"));
	
	// Lsub sorts the folders alphabetically
	ASSERT_EQUALS(folderList.Count(), 3);
	
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'.');
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16("#news.comp.mail"));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_TRUE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	ASSERT_EQUALS(folderList[1]->iHierarchySeperator, (TChar)'.');
	ASSERT_EQUALS(folderList[1]->FolderName(), _L16("#news.comp.mail.mime"));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_TRUE(folderList[1]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	ASSERT_EQUALS(folderList[2]->iHierarchySeperator, (TChar)'.');
	ASSERT_EQUALS(folderList[2]->FolderName(), _L16("#news.comp.mail.misc"));
	ASSERT_TRUE(folderList[2]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[2]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[2]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_TRUE(folderList[2]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	folderList.Reset();
	folderList.Close();
	INFO_PRINTF1(_L("Complete"));
	}
	
CTestSuite* CTestImapLsub::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestLsubWithFlagsL);
	END_SUITE;
	}
