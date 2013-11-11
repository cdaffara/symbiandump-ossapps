// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ctesttlsssldomainsetting.h"
#include "cemailaccounts.h"
#include <imapset.h>
#include <pop3set.h>
#include <smtpset.h>
#include <iapprefs.h>

_LIT16(KAccountName, "Account Name");
_LIT8(KTlsSslDomain, "test.test.com");

/**
Constructor
*/
CTestTlsSslDomainSetting::CTestTlsSslDomainSetting()
	{
	}

/**
Destructor
*/
CTestTlsSslDomainSetting::~CTestTlsSslDomainSetting()
	{
	delete iEmailAccounts;
	}

/**
Test setup
*/
void CTestTlsSslDomainSetting::SetupL()
	{
	iEmailAccounts = CEmailAccounts::NewL();
	}

/**
@SYMTestCaseID MSG-IMAP-TlsSslDomainSetting-0001
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6980
@SYMTestCaseDesc Check adding, fetching, deleting TLS SSL domain name
*/
void CTestTlsSslDomainSetting::TestImapSettingL()
	{
	INFO_PRINTF1(_L("TestImapSettingL - Start"));

	CImImap4Settings* settings = new (ELeave) CImImap4Settings;
	CleanupStack::PushL(settings);
	CImIAPPreferences* iapPrefs = CImIAPPreferences::NewLC();

	// Get default settings & check domain name is empty
	iEmailAccounts->PopulateDefaultImapSettingsL(*settings, *iapPrefs);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	TImapAccount acc = iEmailAccounts->CreateImapAccountL(KAccountName, *settings, *iapPrefs, EFalse);

	// Set domain name and read it back to check it
	settings->SetTlsSslDomainL(KTlsSslDomain);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Compare(KTlsSslDomain));

	// Load account settings & check domain name is cleared
	iEmailAccounts->LoadImapSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	// Set domain name, save settings and read them back to check it
	settings->SetTlsSslDomainL(KTlsSslDomain);
	iEmailAccounts->SaveImapSettingsL(acc, *settings);
	settings->SetTlsSslDomainL(KNullDesC8);
	iEmailAccounts->LoadImapSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Compare(KTlsSslDomain));

	// Clear domain name, save settings and read them back to check it
	settings->SetTlsSslDomainL(KNullDesC8);
	iEmailAccounts->SaveImapSettingsL(acc, *settings);
	settings->SetTlsSslDomainL(KTlsSslDomain);
	iEmailAccounts->LoadImapSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	iEmailAccounts->DeleteImapAccountL(acc);

	CleanupStack::PopAndDestroy(2, settings);

	INFO_PRINTF1(_L("TestImapSettingL - End"));	
	}

/**
@SYMTestCaseID MSG-POP-TlsSslDomainSetting-0001
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6980
@SYMTestCaseDesc Check adding, fetching, deleting TLS SSL domain name
*/
void CTestTlsSslDomainSetting::TestPopSettingL()
	{
	INFO_PRINTF1(_L("TestPopSettingL - Start"));

	CImPop3Settings* settings = new (ELeave) CImPop3Settings;
	CleanupStack::PushL(settings);
	CImIAPPreferences* iapPrefs = CImIAPPreferences::NewLC();

	// Get default settings & check domain name is empty
	iEmailAccounts->PopulateDefaultPopSettingsL(*settings, *iapPrefs);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	TPopAccount acc = iEmailAccounts->CreatePopAccountL(KAccountName, *settings, *iapPrefs, EFalse);

	// Set domain name and read it back to check it
	settings->SetTlsSslDomainL(KTlsSslDomain);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Compare(KTlsSslDomain));

	// Load account settings & check domain name is cleared
	iEmailAccounts->LoadPopSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	// Set domain name, save settings and read them back to check it
	settings->SetTlsSslDomainL(KTlsSslDomain);
	iEmailAccounts->SavePopSettingsL(acc, *settings);
	settings->SetTlsSslDomainL(KNullDesC8);
	iEmailAccounts->LoadPopSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Compare(KTlsSslDomain));

	// Clear domain name, save settings and read them back to check it
	settings->SetTlsSslDomainL(KNullDesC8);
	iEmailAccounts->SavePopSettingsL(acc, *settings);
	settings->SetTlsSslDomainL(KTlsSslDomain);
	iEmailAccounts->LoadPopSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	iEmailAccounts->DeletePopAccountL(acc);

	CleanupStack::PopAndDestroy(2, settings);

	INFO_PRINTF1(_L("TestPopSettingL - End"));
	}

/**
@SYMTestCaseID MSG-SMTP-TlsSslDomainSetting-0001
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6980
@SYMTestCaseDesc Check adding, fetching, deleting TLS SSL domain name
*/
void CTestTlsSslDomainSetting::TestSmtpSettingL()
	{
	INFO_PRINTF1(_L("TestSmtpSettingL - Start"));

	CImSmtpSettings* settings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(settings);
	CImIAPPreferences* iapPrefs = CImIAPPreferences::NewLC();

	// Get default settings & check domain name is empty
	iEmailAccounts->PopulateDefaultSmtpSettingsL(*settings, *iapPrefs);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	TSmtpAccount acc = iEmailAccounts->CreateSmtpAccountL(KAccountName, *settings, *iapPrefs, EFalse);

	// Set domain name and read it back to check it
	settings->SetTlsSslDomainL(KTlsSslDomain);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Compare(KTlsSslDomain));

	// Load account settings & check domain name is cleared
	iEmailAccounts->LoadSmtpSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	// Set domain name, save settings and read them back to check it
	settings->SetTlsSslDomainL(KTlsSslDomain);
	iEmailAccounts->SaveSmtpSettingsL(acc, *settings);
	settings->SetTlsSslDomainL(KNullDesC8);
	iEmailAccounts->LoadSmtpSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Compare(KTlsSslDomain));

	// Clear domain name, save settings and read them back to check it
	settings->SetTlsSslDomainL(KNullDesC8);
	iEmailAccounts->SaveSmtpSettingsL(acc, *settings);
	settings->SetTlsSslDomainL(KTlsSslDomain);
	iEmailAccounts->LoadSmtpSettingsL(acc, *settings);
	ASSERT_EQUALS(0, settings->TlsSslDomain().Length());

	iEmailAccounts->DeleteSmtpAccountL(acc);

	CleanupStack::PopAndDestroy(2, settings);

	INFO_PRINTF1(_L("TestSmtpSettingL - End"));
	}


CTestSuite* CTestTlsSslDomainSetting::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_ASYNC_TEST_STEP(TestImapSettingL);
	ADD_ASYNC_TEST_STEP(TestPopSettingL);
	ADD_ASYNC_TEST_STEP(TestSmtpSettingL);

	END_SUITE;
	}
