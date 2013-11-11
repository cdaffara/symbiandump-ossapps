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

#include "ctestimapsyncdownloadrulessettings.h"
#include "cemailaccounts.h"
#include <smtpset.h>
#include "IAPPrefs.h"
#include "cimapsyncdownloadrules.h"

_LIT16(KImapSyncDownloadRulesAccountName1, "ISDRAN1");
_LIT16(KImapSyncDownloadRulesAccountName2, "ISDRAN2");

/**
Constructor
*/
CTestImapSyncDownloadRulesSettings::CTestImapSyncDownloadRulesSettings()
	{
	}

/**
Destructor
*/
CTestImapSyncDownloadRulesSettings::~CTestImapSyncDownloadRulesSettings()
	{
	delete iEmailAccounts;
	delete iRules;
	}

/**
Test setup
*/
void CTestImapSyncDownloadRulesSettings::SetupL()
	{
	iEmailAccounts = CEmailAccounts::NewL();
	iRules = CImapSyncDownloadRules::NewL();
	}

/*
Test teardown
*/
void CTestImapSyncDownloadRulesSettings::TearDownL()
	{
	RArray<TSmtpAccount> smtpAccounts;
	CleanupClosePushL(smtpAccounts);
	iEmailAccounts->GetSmtpAccountsL(smtpAccounts);
	for (TInt acc = 0; acc < smtpAccounts.Count(); ++acc)
		{
		iEmailAccounts->DeleteSmtpAccountL(smtpAccounts[acc]);
		}
	CleanupStack::PopAndDestroy(&smtpAccounts);

	RArray<TImapAccount> imapAccounts;
	CleanupClosePushL(imapAccounts);
	iEmailAccounts->GetImapAccountsL(imapAccounts);
	for (TInt acc = 0; acc < imapAccounts.Count(); ++acc)
		{
		iEmailAccounts->DeleteImapAccountL(imapAccounts[acc]);
		}
	CleanupStack::PopAndDestroy(&imapAccounts);

	delete iEmailAccounts;
	iEmailAccounts = NULL;

	delete iRules;
	iRules = NULL;
	}

/**
Fill in all the download rules
*/
void CTestImapSyncDownloadRulesSettings::SetAllRulesL(TInt aVal)
	{
	SetRulesValues(aVal);

	delete iRules;
	iRules = NULL;
	iRules = CImapSyncDownloadRules::NewL();

	iBearerTypes = 1;
	for (TInt rule = 0; rule < KMaxImapSyncDownloadRules; ++rule)
		{
		ASSERT_EQUALS(rule, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, rule, iBearerTypes, iMailInfo));
		ASSERT_EQUALS(rule, iRules->AddRuleL(CImapSyncDownloadRules::EFolderRulesType, rule, iBearerTypes, iMailInfo));
		iBearerTypes <<= 1;
		}
	}

/**
Set the values to use in download rules
*/
void CTestImapSyncDownloadRulesSettings::SetRulesValues(TInt aVal)
	{
	iMailInfo.iDestinationFolder = 0;				

	if (aVal == 1)
		{
		iBearerTypes = 1;
		iMailInfo.iTotalSizeLimit = KMaxTInt;
		iMailInfo.iBodyTextSizeLimit = KMaxTInt;
		iMailInfo.iAttachmentSizeLimit = KMaxTInt;
		iMailInfo.iPartialMailOptions = ENoSizeLimits;
		iMailInfo.iMaxEmailSize = KMaxTInt;
		iMailInfo.iGetMailBodyParts = EGetImap4EmailHeaders;
		}
	else if (aVal == 2)
		{
		iBearerTypes = 2;
		iMailInfo.iTotalSizeLimit = 1;
		iMailInfo.iBodyTextSizeLimit= 2;
		iMailInfo.iAttachmentSizeLimit = 3;
		iMailInfo.iPartialMailOptions = EBodyTextOnly;
		iMailInfo.iMaxEmailSize = 4;
		iMailInfo.iGetMailBodyParts = EGetImap4EmailBodyText;
		}
	else
		{
		iBearerTypes = 4;
		iMailInfo.iTotalSizeLimit = 10;
		iMailInfo.iBodyTextSizeLimit= 20;
		iMailInfo.iAttachmentSizeLimit = 30;
		iMailInfo.iPartialMailOptions = EAttachmentsOnly;
		iMailInfo.iMaxEmailSize = 40;
		iMailInfo.iGetMailBodyParts = EGetImap4EmailBodyTextAndAttachments;
		iMailInfo.iDestinationFolder = 0;				
		}
	}

/**
Check that the currently saved rules are as expected
*/
void CTestImapSyncDownloadRulesSettings::CheckRulesL(const TImapAccount& aAccount, TInt aNumRulesExpected, TInt aVal)
	{
	delete iRules;
	iRules = NULL;
	iRules = CImapSyncDownloadRules::NewL();

	TInt err = KErrNone;
	TRAP(err, iEmailAccounts->LoadImapSyncDownloadRulesL(aAccount, *iRules));
	ASSERT_EQUALS(err, KErrNone);

	SetRulesValues(aVal);
	TUint32 bearerTypes = 1;
	TImImap4GetPartialMailInfo mailInfo = iMailInfo;

	TInt rule = 0;
	while (rule < KMaxImapSyncDownloadRules)
		{
		SetRulesValues(3);
		if (iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, rule, iBearerTypes, iMailInfo))
			{
			ASSERT_EQUALS(bearerTypes, iBearerTypes);
			ASSERT_EQUALS(mailInfo.iTotalSizeLimit, iMailInfo.iTotalSizeLimit);
			ASSERT_EQUALS(mailInfo.iBodyTextSizeLimit, iMailInfo.iBodyTextSizeLimit);
			ASSERT_EQUALS(mailInfo.iAttachmentSizeLimit, iMailInfo.iAttachmentSizeLimit);
			ASSERT_EQUALS(mailInfo.iPartialMailOptions, iMailInfo.iPartialMailOptions);
			ASSERT_EQUALS(mailInfo.iMaxEmailSize, iMailInfo.iMaxEmailSize);
			ASSERT_EQUALS(mailInfo.iGetMailBodyParts, iMailInfo.iGetMailBodyParts);
			}
		else
			{
			break;
			}

		SetRulesValues(3);
		if (iRules->GetRuleL(CImapSyncDownloadRules::EFolderRulesType, rule, iBearerTypes, iMailInfo))
			{
			ASSERT_EQUALS(bearerTypes, iBearerTypes);
			ASSERT_EQUALS(mailInfo.iTotalSizeLimit, iMailInfo.iTotalSizeLimit);
			ASSERT_EQUALS(mailInfo.iBodyTextSizeLimit, iMailInfo.iBodyTextSizeLimit);
			ASSERT_EQUALS(mailInfo.iAttachmentSizeLimit, iMailInfo.iAttachmentSizeLimit);
			ASSERT_EQUALS(mailInfo.iPartialMailOptions, iMailInfo.iPartialMailOptions);
			ASSERT_EQUALS(mailInfo.iMaxEmailSize, iMailInfo.iMaxEmailSize);
			ASSERT_EQUALS(mailInfo.iGetMailBodyParts, iMailInfo.iGetMailBodyParts);
			}
		else
			{
			break;
			}

		bearerTypes <<= 1;
		++rule;
		}

	ASSERT_EQUALS(rule, aNumRulesExpected);
	}

/**
Find an IMAP account from its name
*/
TBool CTestImapSyncDownloadRulesSettings::FindAccountL(const TDesC& aAccName, TImapAccount& aAccount)
	{
	RArray<TImapAccount> accountList;
	CleanupClosePushL(accountList);
	iEmailAccounts->GetImapAccountsL(accountList);
	CleanupStack::Pop(&accountList);

	TInt accPos;
	for (accPos = 0; accPos < accountList.Count(); ++accPos)
		{
		if (aAccName.Compare(accountList[accPos].iImapAccountName) == 0)
			{
			break;
			}
		}

	if (accPos < accountList.Count())
		{
		aAccount = accountList[accPos];
		return ETrue;
		}
	
	return EFalse;
	}

/**
Create an IMAP account
*/
TBool CTestImapSyncDownloadRulesSettings::CreateImapAccountL(const TDesC& aAccName, TImapAccount& aAccount, TBool aUseRules)
	{
	TBool retVal = EFalse;

	CImImap4Settings* imapSettings = new (ELeave) CImImap4Settings;
	CleanupStack::PushL(imapSettings);
	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);
	CImIAPPreferences* imapIAPPrefs = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAPPrefs = CImIAPPreferences::NewLC();

	iEmailAccounts->PopulateDefaultImapSettingsL(*imapSettings, *imapIAPPrefs);
	iEmailAccounts->PopulateDefaultSmtpSettingsL(*smtpSettings, *smtpIAPPrefs);

	TInt err = KErrNone;
	if (aUseRules)
		{
		TRAP(err, aAccount = iEmailAccounts->CreateImapAccountL(aAccName, *imapSettings, *imapIAPPrefs, *iRules, EFalse));
		}
	else
		{
		TRAP(err, aAccount = iEmailAccounts->CreateImapAccountL(aAccName, *imapSettings, *imapIAPPrefs, EFalse));		
		}
	ASSERT_EQUALS(err, KErrNone);

	if (err == KErrNone)
		{
		TSmtpAccount smtpAccount = iEmailAccounts->CreateSmtpAccountL(aAccount, *smtpSettings, *smtpIAPPrefs, EFalse);
		iEmailAccounts->SetDefaultSmtpAccountL(smtpAccount);
		retVal = ETrue;
		}

	CleanupStack::PopAndDestroy(4, imapSettings);
	
	return retVal;
	}

/**
Create an IMAP account with a full set of sync download rules
*/
void CTestImapSyncDownloadRulesSettings::TestCreateImapAccountFullL()
	{
	INFO_PRINTF1(_L("TestCreateImapAccountFullL - Start"));

	SetAllRulesL(1);

	TImapAccount account;
	TBool created = CreateImapAccountL(KImapSyncDownloadRulesAccountName1(), account, ETrue);

	if (created)
		{
		CheckRulesL(account, KMaxImapSyncDownloadRules, 1);
		}

	INFO_PRINTF1(_L("TestCreateImapAccountL - Complete"));
	}

/**
Create an IMAP account with an empty set of sync download rules
*/
void CTestImapSyncDownloadRulesSettings::TestCreateImapAccountEmptyL()
	{
	INFO_PRINTF1(_L("TestCreateImapAccountEmptyL - Start"));

	TImapAccount account;
	TBool created = CreateImapAccountL(KImapSyncDownloadRulesAccountName2(), account, ETrue);

	if (created)
		{
		CheckRulesL(account, 0, 1);
		}

	INFO_PRINTF1(_L("TestCreateImapAccountEmptyL - Complete"));
	}

/**
Change the set of full sync download rules for another set of full sync download rules
*/
void CTestImapSyncDownloadRulesSettings::TestChangeSyncDownloadRulesFullToFullL()
	{
	INFO_PRINTF1(_L("TestChangeSyncDownloadRulesFullToFullL - Start"));

	SetAllRulesL(1);

	TImapAccount account;
	TBool created = CreateImapAccountL(KImapSyncDownloadRulesAccountName1(), account, ETrue);

	if (created)
		{
		SetAllRulesL(2);

		TInt err = KErrNone;
		TRAP(err, iEmailAccounts->SaveImapSyncDownloadRulesL(account, *iRules));
		ASSERT_EQUALS(err, KErrNone);

		CheckRulesL(account, KMaxImapSyncDownloadRules, 2);
		}

	INFO_PRINTF1(_L("TestChangeSyncDownloadRulesFullToFullL - Complete"));
	}

/**
Change the set of full sync download rules for an empty set
*/
void CTestImapSyncDownloadRulesSettings::TestChangeSyncDownloadRulesFullToEmptyL()
	{
	INFO_PRINTF1(_L("TestChangeSyncDownloadRulesFullToEmptyL - Start"));

	SetAllRulesL(1);

	TImapAccount account;
	TBool created = CreateImapAccountL(KImapSyncDownloadRulesAccountName1(), account, ETrue);

	if (created)
		{
		// Clear all the rules
		delete iRules;
		iRules = NULL;
		iRules = CImapSyncDownloadRules::NewL();

		TInt err = KErrNone;
		TRAP(err, iEmailAccounts->SaveImapSyncDownloadRulesL(account, *iRules));
		ASSERT_EQUALS(err, KErrNone);

		CheckRulesL(account, 0, 1);
		}

	INFO_PRINTF1(_L("TestChangeSyncDownloadRulesFullToEmptyL - Complete"));
	}

/**
Change the empty set of sync download rules for a full set
*/
void CTestImapSyncDownloadRulesSettings::TestChangeSyncDownloadRulesEmptyToFullL()
	{
	INFO_PRINTF1(_L("TestChangeSyncDownloadRulesEmptyToFullL - Start"));

	TImapAccount account;
	TBool created = CreateImapAccountL(KImapSyncDownloadRulesAccountName2(), account, ETrue);

	if (created)
		{
		SetAllRulesL(1);

		TInt err = KErrNone;
		TRAP(err, iEmailAccounts->SaveImapSyncDownloadRulesL(account, *iRules));
		ASSERT_EQUALS(err, KErrNone);

		CheckRulesL(account, KMaxImapSyncDownloadRules, 1);
		}

	INFO_PRINTF1(_L("TestChangeSyncDownloadRulesEmptyToFullL - Complete"));
	}

/**
Delete an IMAP account with a full set of sync download rules
*/
void CTestImapSyncDownloadRulesSettings::TestDeleteImapAccountFullL()
	{
	INFO_PRINTF1(_L("TestDeleteImapAccountFullL - Start"));

	SetAllRulesL(1);

	TImapAccount account;
	TBool created = CreateImapAccountL(KImapSyncDownloadRulesAccountName1(), account, ETrue);

	if (created)
		{
		TInt err = KErrNone;
		TRAP(err, iEmailAccounts->DeleteImapAccountL(account));
		ASSERT_EQUALS(err, KErrNone);
		}

	INFO_PRINTF1(_L("TestDeleteImapAccountFullL - Complete"));
	}

/**
Delete an IMAP account with an empty set of sync download rules
*/
void CTestImapSyncDownloadRulesSettings::TestDeleteImapAccountEmptyL()
	{
	INFO_PRINTF1(_L("TestDeleteImapAccountEmptyL - Start"));

	TImapAccount account;
	TBool created = CreateImapAccountL(KImapSyncDownloadRulesAccountName2(), account, ETrue);

	if (created)
		{
		TInt err = KErrNone;
		TRAP(err, iEmailAccounts->DeleteImapAccountL(account));
		ASSERT_EQUALS(err, KErrNone);
		}

	INFO_PRINTF1(_L("TestDeleteImapAccountEmptyL - Complete"));
	}

/**
Create an IMAP account using the API which does not take sync download rules
and then add rules to it.
*/
void CTestImapSyncDownloadRulesSettings::TestAddRulesToExistingAccountL()
	{
	INFO_PRINTF1(_L("TestAddRulesToExistingAccountL - Start"));

	TImapAccount account;
	TBool created = CreateImapAccountL(KImapSyncDownloadRulesAccountName1(), account, EFalse);

	if (created)
		{
		SetAllRulesL(1);

		TInt err = KErrNone;
		TRAP(err, iEmailAccounts->SaveImapSyncDownloadRulesL(account, *iRules));
		ASSERT_EQUALS(err, KErrNone);

		CheckRulesL(account, KMaxImapSyncDownloadRules, 1);
		}

	INFO_PRINTF1(_L("TestAddRulesToExistingAccountL - Complete"));
	}

CTestSuite* CTestImapSyncDownloadRulesSettings::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestCreateImapAccountFullL);
	ADD_ASYNC_TEST_STEP(TestCreateImapAccountEmptyL);
	ADD_ASYNC_TEST_STEP(TestChangeSyncDownloadRulesFullToFullL);
	ADD_ASYNC_TEST_STEP(TestChangeSyncDownloadRulesFullToEmptyL);
	ADD_ASYNC_TEST_STEP(TestChangeSyncDownloadRulesEmptyToFullL);
	ADD_ASYNC_TEST_STEP(TestDeleteImapAccountFullL);
	ADD_ASYNC_TEST_STEP(TestDeleteImapAccountEmptyL);
	ADD_ASYNC_TEST_STEP(TestAddRulesToExistingAccountL);
	END_SUITE;
	}
