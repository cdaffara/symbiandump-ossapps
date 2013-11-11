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

#include "ctestsmtpmobilitysettings.h"
#include "cimsmtpmobilitysettings.h"
#include <smtpset.h>

_LIT(KServerAddress, "Server address");
_LIT(KEmailAlias, "Email alias");
_LIT(KEmailAddress, "Email address");
_LIT8(KLoginName, "Login name");
_LIT8(KPassword, "Password");
_LIT8(KTlsSslDomain, "TLS SSL domain");
const TUint KPort = 12345678;
const TUid KCharset = TUid::Uid(12345678);

/**
Constructor
*/
CTestSmtpMobilitySettings::CTestSmtpMobilitySettings()
	{
	}

/**
Destructor
*/
CTestSmtpMobilitySettings::~CTestSmtpMobilitySettings()
	{
	delete iSettings;
	}

/**
Test setup
*/
void CTestSmtpMobilitySettings::SetupL()
	{
	delete iSettings;
	iSettings = NULL;
	iSettings = CImSmtpMobilitySettings::NewL();
	}

/**
Sets all the settings to known values
*/
void CTestSmtpMobilitySettings::SetValuesL(CImSmtpMobilitySettings& aSettings)
	{
	aSettings.SetServerAddressL(KServerAddress);
	aSettings.SetPort(KPort);
	aSettings.SetSecureSockets(ETrue);
	aSettings.SetSSLWrapper(ETrue);
	aSettings.SetEmailAliasL(KEmailAlias);
	aSettings.SetEmailAddressL(KEmailAddress);
	aSettings.SetDefaultMsgCharSet(KCharset);
	aSettings.SetLoginNameL(KLoginName);
	aSettings.SetPasswordL(KPassword);
	aSettings.SetSMTPAuth(ETrue);
	aSettings.SetTlsSslDomainL(KTlsSslDomain);
	}

/**
Check that the settings are empty.

Note that the DefaultMsgCharset setting is not checked here, because the
SMTP settings reset routine does not reset it to a default value.

@param aSettings Settings to check
*/
void CTestSmtpMobilitySettings::CheckSettingsEmpty(const CImSmtpMobilitySettings& aSettings)
	{
	ASSERT_EQUALS(aSettings.ServerAddress().Compare(KNullDesC), 0);
	ASSERT_EQUALS(aSettings.Port(), static_cast<TUint>(KSMTPDefaultPortNumber));
	ASSERT_FALSE(aSettings.SecureSockets());
	ASSERT_FALSE(aSettings.SSLWrapper());
	ASSERT_EQUALS(aSettings.EmailAlias().Compare(KNullDesC), 0);
	ASSERT_EQUALS(aSettings.EmailAddress().Compare(KNullDesC), 0);
	ASSERT_EQUALS(aSettings.LoginName().Compare(KNullDesC8), 0);
	ASSERT_EQUALS(aSettings.Password().Compare(KNullDesC8), 0);
	ASSERT_FALSE(aSettings.SMTPAuth());
	ASSERT_EQUALS(aSettings.TlsSslDomain().Compare(KNullDesC8), 0);
	}

/**
Checks that all the settings have been set to known values

@param aSettings Settings to check
*/
void CTestSmtpMobilitySettings::CheckSettingsSet(const CImSmtpMobilitySettings& aSettings)
	{
	ASSERT_EQUALS(aSettings.ServerAddress().Compare(KServerAddress), 0);
	ASSERT_EQUALS(aSettings.Port(), KPort);
	ASSERT_TRUE(aSettings.SecureSockets() || aSettings.SSLWrapper());
	ASSERT_EQUALS(aSettings.EmailAlias().Compare(KEmailAlias), 0);
	ASSERT_EQUALS(aSettings.EmailAddress().Compare(KEmailAddress), 0);
	ASSERT_EQUALS(aSettings.DefaultMsgCharSet(), KCharset);
	ASSERT_EQUALS(aSettings.LoginName().Compare(KLoginName), 0);
	ASSERT_EQUALS(aSettings.Password().Compare(KPassword), 0);
	ASSERT_TRUE(aSettings.SMTPAuth());
	ASSERT_EQUALS(aSettings.TlsSslDomain().Compare(KTlsSslDomain), 0);
	}

/**
Test create empty settings class
*/
void CTestSmtpMobilitySettings::TestCreateEmptyL()
	{
	INFO_PRINTF1(_L("TestCreateEmptyL - Start"));

	CImSmtpMobilitySettings* settings = CImSmtpMobilitySettings::NewLC();
	CheckSettingsEmpty(*settings);
	CleanupStack::PopAndDestroy(settings);

	settings = CImSmtpMobilitySettings::NewL();
	CheckSettingsEmpty(*settings);
	delete settings;

	INFO_PRINTF1(_L("TestCreateEmptyL - Complete"));
	}

/**
Test set values
*/
void CTestSmtpMobilitySettings::TestSetValuesL()
	{
	INFO_PRINTF1(_L("TestSetValuesL - Start"));

	SetValuesL(*iSettings);

	CheckSettingsSet(*iSettings);

	INFO_PRINTF1(_L("TestSetValuesL - Complete"));
	}

/**
Test class reset
*/
void CTestSmtpMobilitySettings::TestResetL()
	{
	INFO_PRINTF1(_L("TestResetL - Start"));

	SetValuesL(*iSettings);
	iSettings->Reset();
	CheckSettingsEmpty(*iSettings);

	INFO_PRINTF1(_L("TestResetL - Complete"));
	}

/**
Test class copy
*/
void CTestSmtpMobilitySettings::TestCopyL()
	{
	INFO_PRINTF1(_L("TestCopyL - Start"));

	SetValuesL(*iSettings);

	CImSmtpMobilitySettings* settings = iSettings->CopyLC();
	CheckSettingsSet(*settings);
	CleanupStack::PopAndDestroy(settings);

	settings = iSettings->CopyL();
	CheckSettingsSet(*settings);
	delete settings;

	INFO_PRINTF1(_L("TestCopyL - Complete"));
	}

/**
Test direct SMTP settings access
*/
void CTestSmtpMobilitySettings::TestSmtpSettingsL()
	{
	INFO_PRINTF1(_L("TestSmtpSettingsL - Start"));
	
	CImSmtpSettings* smtpSettings = new(ELeave) CImSmtpSettings();

	// Ownership of SMTP settings is passed to SMTP mobility settings class
	iSettings->SetSmtpSettings(smtpSettings);

	ASSERT_EQUALS(&(iSettings->SmtpSettings()), smtpSettings);

	INFO_PRINTF1(_L("TestSmtpSettingsL - Complete"));
	}

CTestSuite* CTestSmtpMobilitySettings::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP(TestCreateEmptyL);
	ADD_TEST_STEP(TestSetValuesL);
	ADD_TEST_STEP(TestResetL);
	ADD_TEST_STEP(TestCopyL);
	ADD_TEST_STEP(TestSmtpSettingsL);
	END_SUITE;
	}
