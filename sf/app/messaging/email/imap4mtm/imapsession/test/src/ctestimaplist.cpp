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

#include "ctestimaplist.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimaplistfolderinfo.h"
#include "cimaputils.h"
#include "cimapfolderinfo.h"

CTestImapList::CTestImapList()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapList::~CTestImapList()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapList::SetupL()
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
	
void CTestImapList::TearDownL()
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
void CTestImapList::TestListWithFlagsL()
	{
	INFO_PRINTF1(_L("TestListWithFlagsL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Drafts\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" INBOX\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\Marked) \"/\" \"Sent Items\"\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\Marked \\Noinferiors) NIL \"My Folder\"\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK LIST completed\r\n"));
	
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L(""), _L("%"), folderList);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("LIST Response"));

	// List sorts the folders alphabetically
	ASSERT_EQUALS(folderList.Count(), 4);
	
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16("Drafts"));
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'/');
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_TRUE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
		
	ASSERT_EQUALS(folderList[1]->FolderName(), _L16("INBOX"));
	ASSERT_EQUALS(folderList[1]->iHierarchySeperator, (TChar)'/');
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_TRUE(folderList[1]->QueryFlag(CImapListFolderInfo::EUnmarked));

	ASSERT_EQUALS(folderList[2]->FolderName(), _L16("My Folder"));
	ASSERT_EQUALS(folderList[2]->iHierarchySeperator, (TChar)'\0');
	ASSERT_TRUE(folderList[2]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[2]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_TRUE(folderList[2]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[2]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	ASSERT_EQUALS(folderList[3]->FolderName(), _L16("Sent Items"));
	ASSERT_EQUALS(folderList[3]->iHierarchySeperator, (TChar)'/');
	ASSERT_FALSE(folderList[3]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[3]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_TRUE(folderList[3]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[3]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	folderList.ResetAndDestroy();
	INFO_PRINTF1(_L("Complete"));
	}
	
/**
When you do an "empty" list request - e.g. when you're only interested in getting back the 
heirarchy separator, you might get an empty flags list.
This is legal, as RFC3501 says mailbox-list = "(" [mbx-list-flags] ")" SP (DQUOTE QUOTED-CHAR DQUOTE / nil) SP mailbox
So mbx-list-flags may not be present.
*/
void CTestImapList::TestListWithNoFlagsL()
	{
	INFO_PRINTF1(_L("TestListWithNoFlagsL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST () \"/\" \"\"\r\n")); // without escapes: * LIST () "/" ""
	iInputStream->AppendInputStringL(_L8("2 OK LIST completed\r\n"));
	
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L(""), _L(""), folderList);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("LIST Response"));

	// List sorts the folders alphabetically
	ASSERT_EQUALS(folderList.Count(), 1);
	
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16(""));
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'/');
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
		
	folderList.ResetAndDestroy();
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapList::TestListWithEmptyDataL()
	{
	INFO_PRINTF1(_L("TestListWithEmptyDataL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\Noselect) \"/\" \"\"\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK LIST completed\r\n"));
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L(""), _L(""), folderList);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("LIST Response"));

	ASSERT_EQUALS(folderList.Count(), 1);
	
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'/');
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16(""));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_TRUE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	folderList.ResetAndDestroy();
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapList::TestListWithSlashL()
	{
	INFO_PRINTF1(_L("TestListWithSlashL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\Noselect) \"/\" /\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK LIST completed\r\n"));
	
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L("/usr/staff/jones"), _L(""), folderList);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("LIST Response"));

	ASSERT_EQUALS(folderList.Count(), 1);
	
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'/');
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16(""));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_TRUE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	folderList.ResetAndDestroy();
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapList::TestListWithDotL()
	{
	INFO_PRINTF1(_L("TestListWithDotL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\Noselect) \".\" #news.\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK LIST completed\r\n"));
	
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L("#news.comp.mail.misc"), _L(""), folderList);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("LIST Response"));

	ASSERT_EQUALS(folderList.Count(), 1);
	
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'.');
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16("#news"));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_TRUE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	folderList.ResetAndDestroy();	
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapList::TestListWithInverterL()
	{
	INFO_PRINTF1(_L("TestListWithInverterL"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\Noselect) \"/\" ~/Mail/foo\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\Noselect \\Noinferiors) \"/\" ~/Mail/meetings\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK LIST completed\r\n"));
	
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L("~/Mail/"), _L("%"), folderList);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("LIST Response"));

	ASSERT_EQUALS(folderList.Count(), 2);
	
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'/');
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16("~/Mail/foo"));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_TRUE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	ASSERT_EQUALS(folderList[1]->iHierarchySeperator, (TChar)'/');
	ASSERT_EQUALS(folderList[1]->FolderName(), _L16("~/Mail/meetings"));
	ASSERT_TRUE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_TRUE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	folderList.ResetAndDestroy();
	INFO_PRINTF1(_L("Complete"));
	}

CTestSuite* CTestImapList::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestListWithFlagsL);
	ADD_ASYNC_TEST_STEP(TestListWithNoFlagsL);
	ADD_ASYNC_TEST_STEP(TestListWithEmptyDataL);
	ADD_ASYNC_TEST_STEP(TestListWithSlashL);
	ADD_ASYNC_TEST_STEP(TestListWithDotL);
	ADD_ASYNC_TEST_STEP(TestListWithInverterL);
	END_SUITE;
	}
