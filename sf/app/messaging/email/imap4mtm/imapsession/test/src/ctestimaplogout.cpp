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

#include "ctestimaplogout.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimaputils.h"

CTestImapLogout::CTestImapLogout()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	{}

CTestImapLogout::~CTestImapLogout()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

void CTestImapLogout::SetupL()
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
	
	iImapSession = CImapSession::NewL(*imapSettings,*imapMailStore, *iInputStream, *iOutputStream);
	
	INFO_PRINTF1(_L("Setup: ServerGreeting"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK Microsoft Exchange 2000 IMAP4rev1 server version 6.0.6249.0 (lon-cn-exchng2k.msexchange2k.closedtest.intra) ready.\r\n"));
	
	ASSERT_EQUALS(CImapSession::EServerStateNone, iImapSession->ServerState());
	
	iImapSession->ReadServerGreetingL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(CImapSession::EServerStateNotAuthenticated, iImapSession->ServerState());
	
	INFO_PRINTF1(_L("Setup: Complete"));
	}
	
void CTestImapLogout::TearDownL()
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
void CTestImapLogout::TestSimpleLogoutL()
	{
	INFO_PRINTF1(_L("TestSimpleLogoutL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
	
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(iOutputStream->PtrOutput(), _L8("1 LOGOUT\r\n"));
	
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapLogout::TestLogoutWithByeL()
	{
	INFO_PRINTF1(_L("TestLogoutWithByeL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* BYE IMAP4rev1 Server logging out\r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK LOGOUT completed\r\n"));
		
	iImapSession->LogoutL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	ASSERT_EQUALS(iOutputStream->PtrOutput(), _L8("1 LOGOUT\r\n"));
	
	INFO_PRINTF1(_L("Complete"));
	}
	
CTestSuite* CTestImapLogout::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestSimpleLogoutL);
	ADD_ASYNC_TEST_STEP(TestLogoutWithByeL);
	END_SUITE;
	}
