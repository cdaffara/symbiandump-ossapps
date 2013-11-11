// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ctestimapauthenticate.h"
#include "msvtestutilsbase.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"
#include "emailtestutils.h"
#include <iapprefs.h>

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimaplistfolderinfo.h"
#include "cimaputils.h"
#include "cimapfolderinfo.h"
#include "cimapmailstore.h"
 CMsvServer *server;

_LIT( KLoopbackAddress, "127.0.0.1" );
_LIT(KCDrive,"c:\\");
_LIT(KImapauth,"t_ctestimapauthenticate");

RTest test1(KImapauth);
CTestImapAuthenticate::CTestImapAuthenticate()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	,iImapSettings(NULL)
	,iEntry(NULL)
	,iMsvServer(NULL)
	,iAccounts(NULL)
	{}

CTestImapAuthenticate::~CTestImapAuthenticate()
	{
	
	delete iAccounts;
	delete iImapSettings;
	delete iActiveWaiter;
	delete iImapSession;
	delete iOutputStream;
	delete iInputStream;
	CImapUtils::Delete();
	}

//DO the initial setup

void CTestImapAuthenticate::SetupL()
	{
	ASSERT(iInputStream == NULL);
	ASSERT(iOutputStream == NULL);
	ASSERT(iActiveWaiter == NULL);
	ASSERT(iImapSession == NULL);
	ASSERT(iMsvServer == NULL);
	
	//clear the messaging folder
	CEmailTestUtils* testUtils;
	testUtils = CEmailTestUtils::NewL(test1);
	CleanupStack::PushL(testUtils);
	testUtils->FileSession().SetSessionPath(KCDrive);
	testUtils->ClearEmailAccountsL();
// TODO: How to clean the message folders or close the messaging server without a PANIC ????

//	iTestUtils->CleanMessageFolderL( );

	testUtils->CreateAllTestDirectories( );
	testUtils->FileSession( ).SetSessionPath( _L( "C:\\" ) );


	testUtils->GoServerSideL( );
	testUtils->InstantiateImapServerMtmL( );
	iImapServerMtm = testUtils->iImapServerMtm;
	
	// Create an account
	testUtils->GoClientSideL( );
	iAccounts = CEmailAccounts::NewL( );
	DoCreateImapAccountL( );

	delete iAccounts;
	iAccounts = NULL;
	
	testUtils->GoServerSideL( );
	
	iEntry = testUtils->iServerEntry;

	iServiceId = iImapAccount.iImapService;
	testUtils->InstantiateImapServerMtmL( );
	iImapServerMtm = testUtils->iImapServerMtm;

		
	CImapUtils::CreateL();
	iImapMailStore = CImapMailStore::NewL(*iEntry);	 
	iImapSettings = CImapSettings::NewL(*iEntry);

	iInputStream = CFakeInputStream::NewL(Logger());
	iOutputStream = CFakeOutputStream::NewL(Logger());
	iActiveWaiter = new(ELeave)CActiveWaiter(Logger());
	iImapSession = CImapSession::NewL(*iImapSettings,*iImapMailStore,*iInputStream, *iOutputStream);
	
	iImapSettings->LoadSettingsL(iServiceId);



	INFO_PRINTF1(_L("Setup: ServerGreeting"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK Microsoft Exchange 2000 IMAP4rev1 server version 6.0.6249.0 (lon-cn-exchng2k.msexchange2k.closedtest.intra) ready.\r\n"));
	
	ASSERT_EQUALS(CImapSession::EServerStateNone, iImapSession->ServerState());
	
	iImapSession->ReadServerGreetingL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	

	CleanupStack::PopAndDestroy(testUtils);
	}
			
void CTestImapAuthenticate::TearDownL()
	{
		delete iImapSession;
	iImapSession = NULL;
	
	delete iActiveWaiter;
	iActiveWaiter = NULL;
	
	delete iOutputStream;
	iOutputStream = NULL;
	
	delete iInputStream;
	iInputStream = NULL;
	

	delete iImapMailStore;
	iImapMailStore = NULL;
	
	delete iAccounts;
	iAccounts = NULL;
	
	delete iImapSettings;
	iImapSettings = NULL;

	CImapUtils::Delete();
	
	
	}

// //Test authenticate CramMd5

void CTestImapAuthenticate::TestAuthwithCramMd5L()
	{
	INFO_PRINTF1(_L("TestAuthwithCramMd5"));
	
	iInputStream->AppendInputStringL(_L8("* CAPABILITY IMAP4 IMAP4REV1 ACL NAMESPACE UIDPLUS IDLE LITERAL+ QUOTA ID MULTIAPPEND LISTEXT CHILDREN BINARY ESEARCH LOGIN-REFERRALS UNSELECT SASL-IR STARTTLS AUTH=LOGIN AUTH=PLAIN AUTH=CRAM-MD5 AUTH=DIGEST-MD5 AUTH=GSSAPI AUTH=MSN AUTH=NTLM \r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK CAPABILITY completed\r\n"));
			
	iImapSession->CapabilityL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	iInputStream->AppendInputStringL(_L8("+ PDExLjEyMjcwMDgxNTBAZDA3MDA0MS5wcm9kLmFkLnN5bWJpYW4uaW50cmE\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK  completed\r\n"));
	
	
	
	iImapSession->AuthenticateL(iActiveWaiter->iStatus,CImapAuthMechanismHelper::ECramMD5);

	iActiveWaiter->WaitActive();
	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());

	INFO_PRINTF1(_L("Complete"));
	}
	
//Test authenticate PLAIN
void CTestImapAuthenticate::TestAuthwithPlainL()
	{
	INFO_PRINTF1(_L("TestAuthwithPlain"));
	
	iInputStream->AppendInputStringL(_L8("* CAPABILITY IMAP4 IMAP4REV1 ACL NAMESPACE UIDPLUS IDLE LITERAL+ QUOTA ID MULTIAPPEND LISTEXT CHILDREN BINARY ESEARCH LOGIN-REFERRALS UNSELECT SASL-IR STARTTLS AUTH=LOGIN AUTH=PLAIN AUTH=CRAM-MD5 AUTH=DIGEST-MD5 AUTH=GSSAPI AUTH=MSN AUTH=NTLM \r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK CAPABILITY completed\r\n"));
			
	iImapSession->CapabilityL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	iInputStream->AppendInputStringL(_L8("2 OK  completed\r\n"));
	
	iImapSession->AuthenticateL(iActiveWaiter->iStatus,CImapAuthMechanismHelper::EPlain);
	iActiveWaiter->WaitActive();
	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());

	INFO_PRINTF1(_L("Complete"));
	}
	
	
// //Test authenticate Login
	
void CTestImapAuthenticate::TestAuthwithLoginL()
	{
	INFO_PRINTF1(_L("TestAuthwithLogin"));
	
	iInputStream->AppendInputStringL(_L8("* CAPABILITY IMAP4 IMAP4REV1 ACL NAMESPACE UIDPLUS IDLE LITERAL+ QUOTA ID MULTIAPPEND LISTEXT CHILDREN BINARY ESEARCH LOGIN-REFERRALS UNSELECT SASL-IR STARTTLS AUTH=LOGIN AUTH=PLAIN AUTH=CRAM-MD5 AUTH=DIGEST-MD5 AUTH=GSSAPI AUTH=MSN AUTH=NTLM \r\n"));
	iInputStream->AppendInputStringL(_L8("1 OK CAPABILITY completed\r\n"));
			
	iImapSession->CapabilityL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	
	iInputStream->AppendInputStringL(_L8("+ VXNlcm5hbWU6\r\n"));
	iInputStream->AppendInputStringL(_L8("+ UGFzc3dvcmQ6\r\n"));
	iInputStream->AppendInputStringL(_L8("2 OK  completed\r\n"));
	
	
	iImapSession->AuthenticateL(iActiveWaiter->iStatus,CImapAuthMechanismHelper::ELogin);

	iActiveWaiter->WaitActive();
	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());

	INFO_PRINTF1(_L("Complete"));

	}
//Create IMAP account
void CTestImapAuthenticate::DoCreateImapAccountL( )
	{
	INFO_PRINTF1( _L("Create IMAP account") );
	
	//create objects and initialise with the defaults
	CImImap4Settings* imap4Settings = new ( ELeave ) CImImap4Settings;
	CleanupStack::PushL( imap4Settings );
	
	CImSmtpSettings* smtpSettings = new ( ELeave ) CImSmtpSettings;
	CleanupStack::PushL( smtpSettings );
	
	CImIAPPreferences* imapIap = CImIAPPreferences::NewLC( );
	
	//override some of the defaults
	imap4Settings->SetPasswordL( _L8( "d70041" ) );
	imap4Settings->SetLoginNameL( _L8( "d70041" ) );
	imap4Settings->SetServerAddressL( KLoopbackAddress );
	imap4Settings->SetFolderPathL( _L8( "" ) );
		
	//create the account
	iImapAccount = iAccounts->CreateImapAccountL( _L( "TestAccount" ), *imap4Settings, *imapIap, EFalse );
	//clean up
	CleanupStack::PopAndDestroy( 3,imap4Settings );
	
	INFO_PRINTF1( _L("IMAP account created") );
	}

//Create Test suite
CTestSuite* CTestImapAuthenticate::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestAuthwithCramMd5L);
	ADD_ASYNC_TEST_STEP(TestAuthwithPlainL);
	ADD_ASYNC_TEST_STEP(TestAuthwithLoginL);
	END_SUITE;
	}
