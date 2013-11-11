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

#include "ctestimapsession.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"

#include "cimapservergreetinginfo.h"
#include "cimapcapabilityinfo.h"
#include "cimapfolderinfo.h"
#include "cimaputils.h"

class CImapSettings;
class CImapMailStore;

CTestImapSession::CTestImapSession()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapSession::~CTestImapSession()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapSession::SetupL()
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
	
	iImapSession = CImapSession::NewL(*imapSettings, *imapMailStore, *iInputStream, *iOutputStream);
	}
	
void CTestImapSession::TearDownL()
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

void CTestImapSession::SetupServerGreetingL()
	{
	INFO_PRINTF1(_L("Setup: ServerGreeting"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK Microsoft Exchange 2000 IMAP4rev1 server version 6.0.6249.0 (lon-cn-exchng2k.msexchange2k.closedtest.intra) ready.\r\n"));
	
	ASSERT_EQUALS(CImapSession::EServerStateNone, iImapSession->ServerState());
	
	iImapSession->ReadServerGreetingL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(CImapSession::EServerStateNotAuthenticated, iImapSession->ServerState());
	
	INFO_PRINTF1(_L("Setup: Complete"));
	}

// Tests
void CTestImapSession::TestOneLineL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestOneLineL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestTwoLinesSimpleL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestTwoLinesSimpleL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestTwoLinesSplitBeforeFirstCrlfL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestTwoLinesSplitBeforeFirstCrlfL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Serv"));
	iInputStream->AppendInputStringL(_L8("er logging out\r\n1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestTwoLinesSplitAfterFirstCrlfL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestTwoLinesSplitAfterFirstCrlfL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n1 OK LOGOUT co"));
	iInputStream->AppendInputStringL(_L8("mpleted\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestTwoLinesSplitOverFirstCrlfL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestTwoLinesSplitOverFirstCrlfL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r"));
	iInputStream->AppendInputStringL(_L8("\n1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestTwoLinesSplitHeavilyOverFirstCrlfL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestTwoLinesSplitHeavilyOverFirstCrlfL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r"));
	iInputStream->AppendInputStringL(_L8("\n")); // <<== This line needs special handling in CImapSession
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestTwoLinesInOneBlockL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestTwoLinesInOneBlockL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestSevenLinesSimpleL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestSevenLinesSimpleL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n"));
	iInputStream->AppendInputStringL(_L8("* BYE second\r\n"));
	iInputStream->AppendInputStringL(_L8("* BYE third\r\n"));
	iInputStream->AppendInputStringL(_L8("* BYE fourth\r\n"));
	iInputStream->AppendInputStringL(_L8("* BYE fifth\r\n"));
	iInputStream->AppendInputStringL(_L8("* BYE sixth\r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestSevenLinesIn3ChunksL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestSevenLinesIn3ChunksL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n* BYE second"));
	iInputStream->AppendInputStringL(_L8("\r\n* BYE third\r\n* BYE fourth\r\n* BYE fi"));
	iInputStream->AppendInputStringL(_L8("fth\r\n* BYE sixth\r\n1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapSession::TestOneLineFragmentedL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestOneLineFragmentedL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 O"));
	iInputStream->AppendInputStringL(_L8("K LOG"));
	iInputStream->AppendInputStringL(_L8("O"));
	iInputStream->AppendInputStringL(_L8("UT co"));
	iInputStream->AppendInputStringL(_L8("mpleted"));
	iInputStream->AppendInputStringL(_L8("\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapSession::TestOneLineHeavilyFragmentedL()
	{
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("TestOneLineHeavilyFragmentedL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1"));
	iInputStream->AppendInputStringL(_L8(" "));
	iInputStream->AppendInputStringL(_L8("O"));
	iInputStream->AppendInputStringL(_L8("K"));
	iInputStream->AppendInputStringL(_L8(" "));
	iInputStream->AppendInputStringL(_L8("L"));
	iInputStream->AppendInputStringL(_L8("O"));
	iInputStream->AppendInputStringL(_L8("G"));
	iInputStream->AppendInputStringL(_L8("O"));
	iInputStream->AppendInputStringL(_L8("U"));
	iInputStream->AppendInputStringL(_L8("T"));
	iInputStream->AppendInputStringL(_L8(" "));
	iInputStream->AppendInputStringL(_L8("c"));
	iInputStream->AppendInputStringL(_L8("o"));
	iInputStream->AppendInputStringL(_L8("m"));
	iInputStream->AppendInputStringL(_L8("p"));
	iInputStream->AppendInputStringL(_L8("l"));
	iInputStream->AppendInputStringL(_L8("e"));
	iInputStream->AppendInputStringL(_L8("t"));
	iInputStream->AppendInputStringL(_L8("e"));
	iInputStream->AppendInputStringL(_L8("d"));
	iInputStream->AppendInputStringL(_L8("\r"));
	iInputStream->AppendInputStringL(_L8("\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapSession::TestLoginSequenceL()
	{
	INFO_PRINTF1(_L("TestLoginSequenceL"));
	
	INFO_PRINTF1(_L("...ServerGreeting"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK Microsoft Exchange 2000 IMAP4rev1 server version 6.0.6249.0 (lon-cn-exchng2k.msexchange2k.closedtest.intra) ready.\r\n"));
		
	ASSERT_EQUALS(CImapServerGreetingInfo::ETagNone, iImapSession->ServerGreetingInfo().ResponseTag());
		
	iImapSession->ReadServerGreetingL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(CImapServerGreetingInfo::ETagOk, iImapSession->ServerGreetingInfo().ResponseTag());
	
	
	INFO_PRINTF1(_L("...Capability"));
	iInputStream->ResetInputStrings();
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	iInputStream->AppendInputStringL(_L8("* CAPABILITY IMAP4rev1 STARTTLS AUTH=GSSAPI LOGINDISABLED BINARY\r\n"));
#else
	iInputStream->AppendInputStringL(_L8("* CAPABILITY IMAP4rev1 STARTTLS AUTH=GSSAPI LOGINDISABLED\r\n"));
#endif
	iInputStream->AppendInputStringL(_L8("1 OK CAPABILITY completed\r\n"));
			
	iImapSession->CapabilityL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	const CImapCapabilityInfo& capabilityInfo = iImapSession->CapabilityInfo();
		
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EVersion4Rev1));
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EStartTls));
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::ELoginDisabled));
	ASSERT_FALSE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EIdle));
	ASSERT_FALSE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EAuthPlain));
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EBinaryCap));
#endif

	INFO_PRINTF1(_L("...StartTLS"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("2 OK Begin TLS negotiation now\r\n"));
			
	iImapSession->StartTlsL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();	
	
	
	INFO_PRINTF1(_L("...Capability"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* CAPABILITY IMAP4rev1 AUTH=GSSAPI AUTH=PLAIN IDLE\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK CAPABILITY completed\r\n"));
			
	iImapSession->CapabilityL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EVersion4Rev1));
	ASSERT_FALSE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EStartTls));
	ASSERT_FALSE(capabilityInfo.QueryFlag(CImapCapabilityInfo::ELoginDisabled));
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EIdle));
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EAuthPlain));

	
	INFO_PRINTF1(_L("...Login"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("4 OK [CAPABILITY IMAP4rev1 IDLE] LOGIN completed\r\n"));
			
	iImapSession->LoginL(iActiveWaiter->iStatus, _L8("username"), _L8("password"));
	iActiveWaiter->WaitActive();
	
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EVersion4Rev1));
	ASSERT_FALSE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EStartTls));
	ASSERT_FALSE(capabilityInfo.QueryFlag(CImapCapabilityInfo::ELoginDisabled));
	ASSERT_TRUE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EIdle));
	ASSERT_FALSE(capabilityInfo.QueryFlag(CImapCapabilityInfo::EAuthPlain));

	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapSession::TestServerStateL()
	{
	INFO_PRINTF1(_L("TestServerStateL"));

	INFO_PRINTF1(_L("...ServerGreeting"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK Microsoft Exchange 2000 IMAP4rev1 server version 6.0.6249.0 (lon-cn-exchng2k.msexchange2k.closedtest.intra) ready.\r\n"));
	
	ASSERT_EQUALS(CImapSession::EServerStateNone, iImapSession->ServerState());
	
	iImapSession->ReadServerGreetingL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(CImapSession::EServerStateNotAuthenticated, iImapSession->ServerState());
	
	
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


	INFO_PRINTF1(_L("...Close"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("3 OK CLOSE completed\r\n"));
		
	iImapSession->CloseL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());
	ASSERT_NULL(iImapSession->SelectedFolderInfo());
	
		
	INFO_PRINTF1(_L("...Logout"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n4 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(CImapSession::EServerStateNone, iImapSession->ServerState());


	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapSession::TestLiteralL()
	{
	INFO_PRINTF1(_L("TestLiteralL"));
	
	SetupServerGreetingL();
	
	INFO_PRINTF1(_L("...Login"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK LOGIN completed\r\n"));
			
	iImapSession->LoginL(iActiveWaiter->iStatus, _L8("username"), _L8("password"));
	iActiveWaiter->WaitActive();
	
	
	INFO_PRINTF1(_L("...Status"));	
	iInputStream->ResetInputStrings();
/*
	iInputStream->AppendInputStringL(_L8("* STATUS {12}\r\nHello\r\nWorld (MESSAGES 231)\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK STATUS completed\r\n"));
*/
	iInputStream->AppendInputStringL(_L8("* STATUS {12}\r\nHello\r\nWor"));
	iInputStream->AppendInputStringL(_L8("ld (MESSAGES 231)\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK STATUS completed\r\n"));

		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	iImapSession->StatusL(iActiveWaiter->iStatus, _L16("Hello\r\nWorld"), _L8("MESSAGES"), *folderInfo);
	iActiveWaiter->WaitActive();
	
	CleanupStack::PopAndDestroy(folderInfo);
	
	INFO_PRINTF1(_L("Complete"));
	}

	
CTestSuite* CTestImapSession::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestOneLineL);
	ADD_ASYNC_TEST_STEP(TestTwoLinesSimpleL);
	ADD_ASYNC_TEST_STEP(TestTwoLinesSplitBeforeFirstCrlfL);
	ADD_ASYNC_TEST_STEP(TestTwoLinesSplitAfterFirstCrlfL);
	ADD_ASYNC_TEST_STEP(TestTwoLinesSplitOverFirstCrlfL);
	ADD_ASYNC_TEST_STEP(TestTwoLinesSplitHeavilyOverFirstCrlfL);
	ADD_ASYNC_TEST_STEP(TestTwoLinesInOneBlockL);
	ADD_ASYNC_TEST_STEP(TestSevenLinesSimpleL);
	ADD_ASYNC_TEST_STEP(TestSevenLinesIn3ChunksL);
	ADD_ASYNC_TEST_STEP(TestOneLineFragmentedL);	
	ADD_ASYNC_TEST_STEP(TestOneLineHeavilyFragmentedL);
	ADD_ASYNC_TEST_STEP(TestLoginSequenceL);
	ADD_ASYNC_TEST_STEP(TestServerStateL);
	ADD_ASYNC_TEST_STEP(TestLiteralL);
	END_SUITE;
	}

