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
// ctestimapcommnad.cpp
// 
//

#include "ctestimapcommand.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"

#include "cimapsession.h"
#include "cimapsessionconsts.h"
#include "cimaputils.h"

CTestImapCommand::CTestImapCommand()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapCommand::~CTestImapCommand()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapCommand::SetupL()
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
	
	SetupServerGreetingL();
	}
	
void CTestImapCommand::TearDownL()
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

void CTestImapCommand::SetupServerGreetingL()
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
void CTestImapCommand::TestTaggedOkOnlyL()
	{
	INFO_PRINTF1(_L("TestTaggedOkOnlyL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
	
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCommand::TestTaggedNoL()
	{
	INFO_PRINTF1(_L("TestTaggedNoL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 NO LOGOUT not allowed right now\r\n"));
	
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive(KErrImapNo);
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCommand::TestTaggedBadL()
	{
	INFO_PRINTF1(_L("TestTaggedBadL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 BAD malformed just testing\r\n"));
	
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive(KErrImapBad);
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCommand::TestKnownThenTaggedOkL()
	{
	INFO_PRINTF1(_L("TestKnownThenTaggedOkL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
	
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCommand::TestUnilateralThenTaggedOkL()
	{
	INFO_PRINTF1(_L("TestUnilateralThenTaggedOkL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 23 EXISTS\r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapCommand::TestUnknownLineThenTaggedOkL()
	{
	INFO_PRINTF1(_L("TestUnknownLineThenTaggedOkL"));
		
	iInputStream->ResetInputStrings();
	// Emulates a one-line extension response
	iInputStream->AppendInputStringL(_L8("* XWIBBLE wobble\r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapCommand::TestUnknownLiteralThenTaggedOkL()
	{
	INFO_PRINTF1(_L("TestUnknownLiteralThenTaggedOkL"));
	
	iInputStream->ResetInputStrings();
	// Emulates a server-pushed FETCH (which we don't handle)
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BODY[1]<0> {19}\r\n"));
	iInputStream->AppendInputStringL(_L8("a small test body\r\n"));
	iInputStream->AppendInputStringL(_L8("UID 2717 FLAGS (\\Seen))\r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapCommand::TestUnknownMultiLiteralThenTaggedOkL()
	{
	INFO_PRINTF1(_L("TestUnknownMultiLiteralThenTaggedOkL"));
	// Emulates a server-pushed FETCH (which we don't handle), with two literals
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BODY[1]<0> {19}\r\n"));
	iInputStream->AppendInputStringL(_L8("a small test body\r\n"));
	iInputStream->AppendInputStringL(_L8(" BODY[1.MIME] {766}\r\n"));
	iInputStream->AppendInputStringL(_L8("Received: from LONDAVIDS02 ([10.23.152.33]) by msexchange2k.closedtest.intra with Microsoft SMTPSVC(5.0.2195.6713);\r\n"));
	iInputStream->AppendInputStringL(_L8("	 Wed, 22 Mar 2006 14:43:56 +0000\r\n"));
	iInputStream->AppendInputStringL(_L8("Message-ID: <001901c64dbf$c42ec6d0$0100a8c0@intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("From: \"IMAP\" <davids@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("To: \"David Test Account\" <davids@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: t1\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Wed, 22 Mar 2006 14:49:05 -0000\r\n"));
	iInputStream->AppendInputStringL(_L8("MIME-Version: 1.0\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Type: text/plain;\r\n"));
	iInputStream->AppendInputStringL(_L8("	charset=\"iso-8859-1\"\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Transfer-Encoding: 7bit\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Priority: 3\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MSMail-Priority: Normal\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Mailer: Microsoft Outlook Express 6.00.2800.1506\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2800.1506\r\n"));
	iInputStream->AppendInputStringL(_L8("Return-Path: davids@msexchange2k.closedtest.intra\r\n"));
	iInputStream->AppendInputStringL(_L8("X-OriginalArrivalTime: 22 Mar 2006 14:43:56.0562 (UTC) FILETIME=[0C365F20:01C64DBF]\r\n"));
	iInputStream->AppendInputStringL(_L8("\r\n"));
	iInputStream->AppendInputStringL(_L8(" UID 2717 FLAGS (\\Seen))\r\n"));
	// Let's have an extra (known) untagged response, too
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n"));
	// And finally, the tagged response
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	// Not in the selected state, so not folder info
	CImapFolderInfo* folderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NULL(folderInfo);
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCommand::TestEncodeMailboxsL()
	{
	INFO_PRINTF1(_L("TestEncodeMailboxL"));
	
	DoTestEncodeMailboxL(_L16("inbox"), _L8("inbox"));
	DoTestEncodeMailboxL(_L16("inbox/folder"), _L8("inbox/folder"));
	DoTestEncodeMailboxL(_L16("inbox\\folder"), _L8("\"inbox\\\\folder\""));
	DoTestEncodeMailboxL(_L16("my folder"), _L8("\"my folder\""));
	DoTestEncodeMailboxL(_L16("daft folder name with \"newline\"\r\nand a smily face \x263a symbol"), _L8("\"daft folder name with \\\"newline\\\"&AA0ACg-and a smily face &Jjo- symbol\""));
	DoTestEncodeMailboxL(_L16("bits&pieces"), _L8("bits&-pieces"));
	DoTestEncodeMailboxL(_L16("smiley:\x263a"), _L8("smiley:&Jjo-"));
	DoTestEncodeMailboxL(_L16("folder[square]"), _L8("\"folder[square]\""));
	DoTestEncodeMailboxL(_L16("toplevel"), _L8("toplevel"));
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapCommand::DoTestEncodeMailboxL(const TDesC16& aUnicode, const TDesC8& aExpectedEncoding)
	{
	HBufC8* encodedMailbox = CImapCommand::EncodeMailboxNameForSendL(aUnicode);
	CleanupStack::PushL(encodedMailbox);
	
	ASSERT_EQUALS((const TDesC8&)*encodedMailbox, aExpectedEncoding);
	
	CleanupStack::PopAndDestroy();
	}

CTestSuite* CTestImapCommand::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestTaggedOkOnlyL);
	ADD_ASYNC_TEST_STEP(TestTaggedNoL);
	ADD_ASYNC_TEST_STEP(TestTaggedBadL);
	ADD_ASYNC_TEST_STEP(TestKnownThenTaggedOkL);
	ADD_ASYNC_TEST_STEP(TestUnilateralThenTaggedOkL);
	ADD_ASYNC_TEST_STEP(TestUnknownLineThenTaggedOkL);
	ADD_ASYNC_TEST_STEP(TestUnknownLiteralThenTaggedOkL);
	ADD_ASYNC_TEST_STEP(TestUnknownMultiLiteralThenTaggedOkL);
	ADD_ASYNC_TEST_STEP(TestEncodeMailboxsL);
	END_SUITE;
	}
