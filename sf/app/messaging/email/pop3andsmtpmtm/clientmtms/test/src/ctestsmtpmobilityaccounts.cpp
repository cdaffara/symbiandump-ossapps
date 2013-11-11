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

#include "ctestsmtpmobilityaccounts.h"
#include <cemailaccounts.h>
#include <smtpset.h>
#include <pop3set.h>
#include <imapset.h>
#include <iapprefs.h>
#include <cimsmtpmobilitysettings.h>

_LIT(KSmtpAcc1Name, "SMTP Acc 1");
_LIT(KSmtpAcc2Name, "SMTP Acc 2");
_LIT(KSmtpAcc3Name, "SMTP Acc 3");
_LIT(KMobAcc1Name, "Mob Acc 1");
_LIT(KMobAcc2Name, "Mob Acc 2");
_LIT(KMobAcc3Name, "Mob Acc 3");
_LIT(KPopAcc1Name, "POP Acc 1");
_LIT(KPopAcc2Name, "POP Acc 2");
_LIT(KImapAcc1Name, "IMAP Acc 1");
_LIT(KImapAcc2Name, "IMAP Acc 1");
_LIT8(KSmtpLoginName, "SMTP Login");
_LIT8(KMobLoginName, "Mob Login");
_LIT8(KTestLoginName, "TestLoginName");

/**
Constructor
*/
CTestSmtpMobilityAccounts::CTestSmtpMobilityAccounts()
	{
	}

/**
Destructor
*/
CTestSmtpMobilityAccounts::~CTestSmtpMobilityAccounts()
	{
	delete iEmailAccounts;
	delete iSmtpAccSettings;
	delete iIapPrefs;
	delete iMobAccSettings;
	iIapList.Close();
	}

/**
Test setup
*/
void CTestSmtpMobilityAccounts::SetupL()
	{
	iEmailAccounts = CEmailAccounts::NewL();

	iSmtpAccSettings = new (ELeave) CImSmtpSettings();
	iIapPrefs = CImIAPPreferences::NewLC();
	CleanupStack::Pop(iIapPrefs);
	iEmailAccounts->PopulateDefaultSmtpSettingsL(*iSmtpAccSettings, *iIapPrefs);
	iSmtpAccSettings->SetLoginNameL(KSmtpLoginName);

	iMobAccSettings = CImSmtpMobilitySettings::NewL();
	iEmailAccounts->PopulateDefaultSmtpMobilitySettingsL(*iMobAccSettings);
	iMobAccSettings->SetLoginNameL(KMobLoginName);

	TImIAPChoice iapChoice;
	iapChoice.iIAP = 1;
	iapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	iIapPrefs->AddIAPL(iapChoice, 0);

	iIapList.AppendL(1);

	ResetAccountsL();
	}

/**
Test cleanup
*/
void CTestSmtpMobilityAccounts::TearDownL()
	{
	ResetAccountsL();

	delete iEmailAccounts;
	iEmailAccounts = NULL;

	delete iSmtpAccSettings;
	iSmtpAccSettings = NULL;

	delete iMobAccSettings;
	iMobAccSettings = NULL;

	delete iIapPrefs;
	iIapPrefs = NULL;

	iIapList.Reset();
	}

/**
Delete all email accounts
*/
void CTestSmtpMobilityAccounts::ResetAccountsL()
	{
	RArray<TPopAccount> popAccounts;
	CleanupClosePushL(popAccounts);
	iEmailAccounts->GetPopAccountsL(popAccounts);
	for (TInt count = 0; count < popAccounts.Count(); ++count)
		{
		iEmailAccounts->DeletePopAccountL(popAccounts[count]);
		}
	CleanupStack::PopAndDestroy(&popAccounts);

	RArray<TImapAccount> imapAccounts;
	CleanupClosePushL(imapAccounts);
	iEmailAccounts->GetImapAccountsL(imapAccounts);
	for (TInt count = 0; count < imapAccounts.Count(); ++count)
		{
		iEmailAccounts->DeleteImapAccountL(imapAccounts[count]);
		}
	CleanupStack::PopAndDestroy(&imapAccounts);

	RArray<TSmtpAccount> smtpAccounts;
	CleanupClosePushL(smtpAccounts);
	iEmailAccounts->GetSmtpAccountsL(smtpAccounts);
	for (TInt count = 0; count < smtpAccounts.Count(); ++count)
		{
		iEmailAccounts->DeleteSmtpAccountL(smtpAccounts[count]);
		}
	CleanupStack::PopAndDestroy(&smtpAccounts);
	}

/**
Check number of SMTP accounts matches expected value

@param aNum Number expected
*/
void CTestSmtpMobilityAccounts::CheckNumberSMTPAccountsL(TInt aNum)
	{
	RArray<TSmtpAccount> accounts;
	CleanupClosePushL(accounts);
	iEmailAccounts->GetSmtpAccountsL(accounts);
	ASSERT_EQUALS(aNum, accounts.Count());
	CleanupStack::PopAndDestroy(&accounts);
	}

/**
Check SMTP account settings match expected values

@param aAcount Account to check
@param aSettings Settings to compare against
*/
void CTestSmtpMobilityAccounts::CheckSmtpAccountSettingsL(TSmtpAccount aAccount, const CImSmtpSettings& aSettings)
	{
	CImSmtpSettings* settings = new (ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);

	iEmailAccounts->LoadSmtpSettingsL(aAccount, *settings);

	ASSERT_EQUALS(*settings, aSettings);

	CleanupStack::PopAndDestroy(settings);
	}

/**
Check number of SMTP mobility accounts matches expected value

@param aNum Number expected
*/
void CTestSmtpMobilityAccounts::CheckNumberMobilityAccountsL(TInt aNum)
	{
	RArray<TSmtpMobilityAccount> accounts;
	CleanupClosePushL(accounts);
	iEmailAccounts->GetSmtpMobilityAccountsL(accounts);
	ASSERT_EQUALS(aNum, accounts.Count());
	CleanupStack::PopAndDestroy(&accounts);
	}

/**
Check number of SMTP mobility accounts matches expected value for a
given parent SMTP account

@param aNum Number expected
@param aSmtpAcc SMTP account
*/
void CTestSmtpMobilityAccounts::CheckNumberMobilityAccountsL(TInt aNum, TSmtpAccount aSmtpAcc)
	{
	RArray<TSmtpMobilityAccount> accounts;
	CleanupClosePushL(accounts);
	iEmailAccounts->GetSmtpMobilityAccountsL(aSmtpAcc, accounts);
	ASSERT_EQUALS(aNum, accounts.Count());
	CleanupStack::PopAndDestroy(&accounts);
	}

/**
Check mobility account details match expected values.
Looks up the account given the account name, and checks the settings,
IAP list, and read only flag.

@param aAccName Account name
@param aSmtpSettings Expected settings
@param aIapList Expected IAP list
@param aReadOnly Expected read only flag value
@param aSmtpAcc Parent SMTP account
*/
void CTestSmtpMobilityAccounts::CheckMobilityAccountDetailsL(const TDesC& aAccName, const CImSmtpSettings& aSmtpSettings, const RArrayIAP& aIapList, TBool aReadOnly, TSmtpAccount aSmtpAcc)
	{
	RArray<TSmtpMobilityAccount> accounts;
	CleanupClosePushL(accounts);
	iEmailAccounts->GetSmtpMobilityAccountsL(aSmtpAcc, accounts);
	for (TInt count = 0; count < accounts.Count(); ++count)
		{
		if (accounts[count].iName.Compare(aAccName) == 0)
			{
			CImSmtpMobilitySettings* mobAccSettings = CImSmtpMobilitySettings::NewLC();
			iEmailAccounts->LoadSmtpMobilitySettingsL(accounts[count], *mobAccSettings);
			ASSERT_EQUALS(mobAccSettings->SmtpSettings(), aSmtpSettings);
			CleanupStack::PopAndDestroy(mobAccSettings);

			RArrayIAP iapList;
			CleanupClosePushL(iapList);
			iEmailAccounts->LoadSmtpMobilityAccountIapListL(accounts[count], iapList);
			ASSERT_EQUALS(iapList.Count(), aIapList.Count());
			for (TInt iap = 0; iap < iapList.Count(); ++iap)
				{
				ASSERT_EQUALS(iapList[iap], aIapList[iap]);
				}
			CleanupStack::PopAndDestroy(&iapList);

			TBool isReadOnly = iEmailAccounts->IsAccountReadOnlyL(accounts[count]);
			ASSERT_EQUALS(isReadOnly, aReadOnly);
			CleanupStack::PopAndDestroy(&accounts);
			return;
			}
		}

	// Account not found, so signal a failure
	ASSERT_TRUE(EFalse);

	CleanupStack::PopAndDestroy(&accounts);	
	}

/**
Checks that mobility account does not exist

@param aAccName Mobility account name
*/
void CTestSmtpMobilityAccounts::CheckMobilityAccountMissingL(const TDesC& aAccName)
	{
	TBool found = EFalse;

	RArray<TSmtpMobilityAccount> accounts;
	CleanupClosePushL(accounts);
	iEmailAccounts->GetSmtpMobilityAccountsL(accounts);
	for (TInt count = 0; count < accounts.Count(); ++count)
		{
		if (accounts[count].iName.Compare(aAccName) == 0)
			{
			found = ETrue;
			break;
			}
		}

	CleanupStack::PopAndDestroy(&accounts);

	ASSERT_FALSE(found);
	}

/**
@SYMTestCaseID SMTP-MOB-BASE-0001
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests SMTP account operations
*/
void CTestSmtpMobilityAccounts::TestSmtpAccountL()
	{
	INFO_PRINTF1(_L("TestSmtpAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	CheckNumberSMTPAccountsL(1);

	// Load SMTP settings
	CImSmtpSettings* smtpAccSettings = new (ELeave) CImSmtpSettings();
	CleanupStack::PushL(smtpAccSettings);
	iEmailAccounts->LoadSmtpSettingsL(smtpAcc, *smtpAccSettings);

	// Check SMTP settings
	ASSERT_EQUALS(*iSmtpAccSettings, *smtpAccSettings);

	// Load IAP settings
	CImIAPPreferences* iapPrefs = CImIAPPreferences::NewLC();
	iEmailAccounts->LoadSmtpIapSettingsL(smtpAcc, *iapPrefs);

	// Check IAP settings
	ASSERT_EQUALS(iapPrefs->NumberOfIAPs(), iIapPrefs->NumberOfIAPs());
	TImIAPChoice iapChoice1;
	TImIAPChoice iapChoice2;
	for (TInt count = 0; count < iapPrefs->NumberOfIAPs(); ++count)
		{
		iapChoice1 = iapPrefs->IAPPreference(count);
		iapChoice2 = iIapPrefs->IAPPreference(count);
		ASSERT_EQUALS(iapChoice1.iIAP, iapChoice2.iIAP);
		ASSERT_EQUALS(iapChoice1.iDialogPref, iapChoice2.iDialogPref);
		}

	// Save new IAP settings
	iIapPrefs->RemoveIAPL(0);
	TImIAPChoice iapChoice;
	iapChoice.iIAP = 2;
	iapChoice.iDialogPref = ECommDbDialogPrefDoNotPrompt;
	iIapPrefs->AddIAPL(iapChoice, 0);
	iEmailAccounts->SaveSmtpIapSettingsL(smtpAcc, *iIapPrefs);

	// Save new SMTP settings
	iSmtpAccSettings->SetLoginNameL(KTestLoginName);
	iEmailAccounts->SaveSmtpSettingsL(smtpAcc, *iSmtpAccSettings);

	iEmailAccounts->LoadSmtpIapSettingsL(smtpAcc, *iapPrefs);

	// Check IAP settings
	ASSERT_EQUALS(iapPrefs->NumberOfIAPs(), iIapPrefs->NumberOfIAPs());
	for (TInt count = 0; count < iapPrefs->NumberOfIAPs(); ++count)
		{
		iapChoice1 = iapPrefs->IAPPreference(count);
		iapChoice2 = iIapPrefs->IAPPreference(count);
		ASSERT_EQUALS(iapChoice1.iIAP, iapChoice2.iIAP);
		ASSERT_EQUALS(iapChoice1.iDialogPref, iapChoice2.iDialogPref);
		}

	CheckSmtpAccountSettingsL(smtpAcc, *iSmtpAccSettings);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CheckNumberSMTPAccountsL(0);

	CleanupStack::PopAndDestroy(2, smtpAccSettings);

	INFO_PRINTF1(_L("TestSmtpAccountL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0001
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding single SMTP mobility account
*/
void CTestSmtpMobilityAccounts::Test1MobilityAccountL()
	{
	INFO_PRINTF1(_L("Test1MobilityAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	
	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc);

	CheckNumberMobilityAccountsL(0);
	CheckNumberMobilityAccountsL(0, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(1);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("Test1MobilityAccountL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0002
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding single SMTP mobility account and deleting parent
*/
void CTestSmtpMobilityAccounts::Test1MobilityAccountWithParentDeletionL()
	{
	INFO_PRINTF1(_L("Test1MobilityAccountWithParentDeletionL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(0);

	INFO_PRINTF1(_L("Test1MobilityAccountWithParentDeletionL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0003
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 SMTP mobility accounts
*/
void CTestSmtpMobilityAccounts::Test2MobilityAccountsL()
	{
	INFO_PRINTF1(_L("Test2MobilityAccountsL - Start"));

	TSmtpAccount smtpAcc1 = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount smtpAcc2 = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc1, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc2, *iMobAccSettings, ETrue);

	CheckNumberMobilityAccountsL(2);
	CheckNumberMobilityAccountsL(1, smtpAcc1);
	CheckNumberMobilityAccountsL(1, smtpAcc2);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc1);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc2);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc1);
	CheckNumberMobilityAccountsL(0, smtpAcc2);

	CheckMobilityAccountMissingL(KMobAcc2Name);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc1);

	CheckNumberSMTPAccountsL(2);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberMobilityAccountsL(0);
	CheckNumberMobilityAccountsL(0, smtpAcc1);
	CheckNumberMobilityAccountsL(0, smtpAcc2);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(2);

	CheckSmtpAccountSettingsL(smtpAcc1, *iSmtpAccSettings);
	CheckSmtpAccountSettingsL(smtpAcc2, *iSmtpAccSettings);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc1);
	iEmailAccounts->DeleteSmtpAccountL(smtpAcc2);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("Test2MobilityAccountsL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0004
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 SMTP mobility accounts and deleting parents
*/
void CTestSmtpMobilityAccounts::Test2MobilityAccountsWithParentDeletionL()
	{
	INFO_PRINTF1(_L("Test2MobilityAccountsWithParentDeletionL - Start"));

	TSmtpAccount smtpAcc1 = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount smtpAcc2 = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc1, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc2, *iMobAccSettings, ETrue);

	CheckNumberMobilityAccountsL(2);
	CheckNumberMobilityAccountsL(1, smtpAcc1);
	CheckNumberMobilityAccountsL(1, smtpAcc2);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc1);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc2);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc2);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc1);

	CheckMobilityAccountMissingL(KMobAcc2Name);

	CheckNumberSMTPAccountsL(1);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc1);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(0);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("Test2MobilityAccountsWithParentDeletionL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0005
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 SMTP mobility accounts with the same parent and deleting parent
*/
void CTestSmtpMobilityAccounts::Test2MobilityAccountsWithSameParentDeletionL()
	{
	INFO_PRINTF1(_L("Test2MobilityAccountsWithSameParentDeletionL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, *iMobAccSettings, ETrue);

	CheckNumberMobilityAccountsL(2);
	CheckNumberMobilityAccountsL(2, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);

	CheckNumberSMTPAccountsL(0);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("Test2MobilityAccountsWithSameParentDeletionL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0006
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 mobility account with same parent
*/
void CTestSmtpMobilityAccounts::Test2MobilityAccountsWithSameParentL()
	{
	INFO_PRINTF1(_L("Test2MobilityAccountsWithSameParentL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, *iMobAccSettings, ETrue);

	CheckNumberMobilityAccountsL(2);
	CheckNumberMobilityAccountsL(2, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberSMTPAccountsL(1);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberSMTPAccountsL(1);

	CheckNumberMobilityAccountsL(0);
	CheckNumberMobilityAccountsL(0, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("Test2MobilityAccountsWithSameParentL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0007
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding single SMTP mobility account to POP associated SMTP account
*/
void CTestSmtpMobilityAccounts::TestMobilityAccountWithPopL()
	{
	INFO_PRINTF1(_L("TestMobilityAccountWithPopL - Start"));

	CImPop3Settings* settings = new (ELeave) CImPop3Settings();

	TPopAccount popAcc = iEmailAccounts->CreatePopAccountL(KPopAcc1Name, *settings, *iIapPrefs, EFalse);

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(popAcc, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	
	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(0);

	iEmailAccounts->DeletePopAccountL(popAcc);

	INFO_PRINTF1(_L("TestMobilityAccountWithPopL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0008
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding single SMTP mobility account to IMAP associated SMTP account
*/
void CTestSmtpMobilityAccounts::TestMobilityAccountWithImapL()
	{
	INFO_PRINTF1(_L("TestMobilityAccountWithImapL - Start"));

	CImImap4Settings* settings = new (ELeave) CImImap4Settings();

	TImapAccount imapAcc = iEmailAccounts->CreateImapAccountL(KImapAcc1Name, *settings, *iIapPrefs, EFalse);

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(imapAcc, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(0);

	iEmailAccounts->DeleteImapAccountL(imapAcc);

	INFO_PRINTF1(_L("TestMobilityAccountWithImapL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0009
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 3 SMTP mobility accounts to same parent and deleting them in the order first, second, third
*/
void CTestSmtpMobilityAccounts::Test3MobilityAccountsDelete123L()
	{
	INFO_PRINTF1(_L("Test3MobilityAccountsDelete123L - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, *iMobAccSettings, ETrue);

	RArrayIAP iapList2;
	CleanupClosePushL(iapList2);
	iapList2.AppendL(3);

	TSmtpMobilityAccount mobAcc3 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc3Name, iapList2, smtpAcc, *iMobAccSettings, ETrue);

	CheckNumberMobilityAccountsL(3, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc3Name, iMobAccSettings->SmtpSettings(), iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberMobilityAccountsL(2, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc3Name, iMobAccSettings->SmtpSettings(), iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberMobilityAccountsL(1, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountDetailsL(KMobAcc3Name, iMobAccSettings->SmtpSettings(), iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc3);

	CheckNumberMobilityAccountsL(0, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(2, &iapList);

	INFO_PRINTF1(_L("Test3MobilityAccountsDelete123L - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0010
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 3 SMTP mobility accounts to same parent and deleting them in the order third, second, first
*/
void CTestSmtpMobilityAccounts::Test3MobilityAccountsDelete321L()
	{
	INFO_PRINTF1(_L("Test3MobilityAccountsDelete321L - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, *iMobAccSettings, ETrue);

	RArrayIAP iapList2;
	CleanupClosePushL(iapList2);
	iapList2.AppendL(3);

	TSmtpMobilityAccount mobAcc3 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc3Name, iapList2, smtpAcc, *iMobAccSettings, ETrue);

	CheckNumberMobilityAccountsL(3, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc3Name, iMobAccSettings->SmtpSettings(), iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc3);

	CheckNumberMobilityAccountsL(2, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberMobilityAccountsL(1, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberMobilityAccountsL(0, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(2, &iapList);

	INFO_PRINTF1(_L("Test3MobilityAccountsDelete321L - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0011
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 3 SMTP mobility accounts to same parent and deleting them in the order second, first, third
*/
void CTestSmtpMobilityAccounts::Test3MobilityAccountsDelete213L()
	{
	INFO_PRINTF1(_L("Test3MobilityAccountsDelete213L - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, *iMobAccSettings, ETrue);

	RArrayIAP iapList2;
	CleanupClosePushL(iapList2);
	iapList2.AppendL(3);

	TSmtpMobilityAccount mobAcc3 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc3Name, iapList2, smtpAcc, *iMobAccSettings, ETrue);

	CheckNumberMobilityAccountsL(3, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, iMobAccSettings->SmtpSettings(), iapList, ETrue, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc3Name, iMobAccSettings->SmtpSettings(), iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberMobilityAccountsL(2, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountDetailsL(KMobAcc3Name, iMobAccSettings->SmtpSettings(), iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberMobilityAccountsL(1, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountDetailsL(KMobAcc3Name, iMobAccSettings->SmtpSettings(), iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc3);

	CheckNumberMobilityAccountsL(0, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(2, &iapList);

	INFO_PRINTF1(_L("Test3MobilityAccountsDelete213L - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0012
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests changing mobility account settings
*/
void CTestSmtpMobilityAccounts::TestChangeMobilityAccountSettingsL()
	{
	INFO_PRINTF1(_L("TestChangeMobilityAccountSettingsL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);

	iMobAccSettings->SetLoginNameL(KTestLoginName);

	iEmailAccounts->SaveSmtpMobilitySettingsL(mobAcc, *iMobAccSettings);

	iMobAccSettings->SetLoginNameL(KMobLoginName);

	iEmailAccounts->LoadSmtpMobilitySettingsL(mobAcc, *iMobAccSettings);

	ASSERT_EQUALS(iMobAccSettings->LoginName().Compare(KTestLoginName), 0);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestChangeMobilityAccountSettingsL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-PAR-ACC-0013
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests changing mobility account IAP list
*/
void CTestSmtpMobilityAccounts::TestChangeMobilityAccountIapListL()
	{
	INFO_PRINTF1(_L("TestChangeMobilityAccountIapListL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, iMobAccSettings->SmtpSettings(), iIapList, EFalse, smtpAcc);

	iIapList.AppendL(2);

	iEmailAccounts->SaveSmtpMobilityAccountIapListL(mobAcc, iIapList);

	iIapList.Reset();

	iEmailAccounts->LoadSmtpMobilityAccountIapListL(mobAcc, iIapList);

	ASSERT_EQUALS(iIapList.Count(), 2);
	ASSERT_TRUE(iIapList[0] == 1);
	ASSERT_TRUE(iIapList[1] == 2);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestChangeMobilityAccountIapListL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0001
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding single linked SMTP mobility account
*/
void CTestSmtpMobilityAccounts::Test1LinkedMobilityAccountL()
	{
	INFO_PRINTF1(_L("Test1LinkedMobilityAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc);

	CheckNumberMobilityAccountsL(0);
	CheckNumberMobilityAccountsL(0, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(2);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	INFO_PRINTF1(_L("Test1LinkedMobilityAccountL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0002
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding single linked SMTP mobility account and deleting parent
*/
void CTestSmtpMobilityAccounts::Test1LinkedMobilityAccountWithLinkedDeletionL()
	{
	INFO_PRINTF1(_L("Test1LinkedMobilityAccountWithLinkedDeletionL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);

	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(1);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("Test1LinkedMobilityAccountWithLinkedDeletionL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0003
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 linked SMTP mobility accounts
*/
void CTestSmtpMobilityAccounts::Test2LinkedMobilityAccountsL()
	{
	INFO_PRINTF1(_L("Test2LinkedMobilityAccountsL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc1 = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc2 = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc3Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc1, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, linkedAcc2, ETrue);

	CheckNumberMobilityAccountsL(2);
	CheckNumberMobilityAccountsL(2, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc2Name);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);

	CheckNumberSMTPAccountsL(3);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberMobilityAccountsL(0);
	CheckNumberMobilityAccountsL(0, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(3);

	CheckSmtpAccountSettingsL(smtpAcc, *iSmtpAccSettings);
	CheckSmtpAccountSettingsL(linkedAcc1, *iSmtpAccSettings);
	CheckSmtpAccountSettingsL(linkedAcc2, *iSmtpAccSettings);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc1);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc2);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("Test2LinkedMobilityAccountsL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0004
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 linked SMTP mobility accounts and deleting parents
*/
void CTestSmtpMobilityAccounts::Test2LinkedMobilityAccountsWithLinkedDeletionL()
	{
	INFO_PRINTF1(_L("Test2LinkedMobilityAccountsWithLinkedDeletionL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc1 = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc2 = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc3Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc1, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, linkedAcc2, ETrue);

	CheckNumberMobilityAccountsL(2);
	CheckNumberMobilityAccountsL(2, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpAccountL(linkedAcc2);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc2Name);

	CheckNumberSMTPAccountsL(2);

	iEmailAccounts->DeleteSmtpAccountL(linkedAcc1);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(1);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("Test2LinkedMobilityAccountsWithLinkedDeletionL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0005
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 linked SMTP mobility accounts with the same parent and deleting parent
*/
void CTestSmtpMobilityAccounts::Test2LinkedMobilityAccountsWithSameLinkedDeletionL()
	{
	INFO_PRINTF1(_L("Test2LinkedMobilityAccountsWithSameLinkedDeletionL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, linkedAcc, ETrue);

	CheckNumberMobilityAccountsL(2);
	CheckNumberMobilityAccountsL(2, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);

	CheckNumberSMTPAccountsL(1);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("Test2LinkedMobilityAccountsWithSameLinkedDeletionL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0006
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 linked mobility account with same parent
*/
void CTestSmtpMobilityAccounts::Test2LinkedMobilityAccountsWithSameLinkedL()
	{
	INFO_PRINTF1(_L("Test2LinkedMobilityAccountsWithSameLinkedL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, linkedAcc, ETrue);

	CheckNumberMobilityAccountsL(2);
	CheckNumberMobilityAccountsL(2, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberSMTPAccountsL(2);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberSMTPAccountsL(2);

	CheckNumberMobilityAccountsL(0);
	CheckNumberMobilityAccountsL(0, smtpAcc);

	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("Test2LinkedMobilityAccountsWithSameLinkedL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0007
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding single linked SMTP mobility account to POP associated SMTP account
*/
void CTestSmtpMobilityAccounts::TestLinkedMobilityAccountWithPopL()
	{
	INFO_PRINTF1(_L("TestLinkedMobilityAccountWithPopL - Start"));

	CImPop3Settings* settings = new (ELeave) CImPop3Settings();

	TPopAccount popAcc1 = iEmailAccounts->CreatePopAccountL(KPopAcc1Name, *settings, *iIapPrefs, EFalse);
	TPopAccount popAcc2 = iEmailAccounts->CreatePopAccountL(KPopAcc2Name, *settings, *iIapPrefs, EFalse);

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(popAcc1, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(popAcc2, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);

	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(1);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeletePopAccountL(popAcc1);
	iEmailAccounts->DeletePopAccountL(popAcc2);

	INFO_PRINTF1(_L("TestLinkedMobilityAccountWithPopL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0008
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding single linked SMTP mobility account to IMAP associated SMTP account
*/
void CTestSmtpMobilityAccounts::TestLinkedMobilityAccountWithImapL()
	{
	INFO_PRINTF1(_L("TestLinkedMobilityAccountWithImapL - Start"));

	CImImap4Settings* settings = new (ELeave) CImImap4Settings();

	TImapAccount imapAcc1 = iEmailAccounts->CreateImapAccountL(KImapAcc1Name, *settings, *iIapPrefs, EFalse);
	TImapAccount imapAcc2 = iEmailAccounts->CreateImapAccountL(KImapAcc2Name, *settings, *iIapPrefs, EFalse);

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(imapAcc1, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(imapAcc2, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);

	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	CheckNumberMobilityAccountsL(0);

	CheckMobilityAccountMissingL(KMobAcc1Name);

	CheckNumberSMTPAccountsL(1);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteImapAccountL(imapAcc1);
	iEmailAccounts->DeleteImapAccountL(imapAcc2);

	INFO_PRINTF1(_L("TestLinkedMobilityAccountWithImapL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0009
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 3 linked SMTP mobility accounts to same parent and deleting them in the order first, second, third
*/
void CTestSmtpMobilityAccounts::Test3LinkedMobilityAccountsDelete123L()
	{
	INFO_PRINTF1(_L("Test3LinkedMobilityAccountsDelete123L - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, linkedAcc, ETrue);

	RArrayIAP iapList2;
	CleanupClosePushL(iapList2);
	iapList2.AppendL(3);

	TSmtpMobilityAccount mobAcc3 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc3Name, iapList2, smtpAcc, linkedAcc, ETrue);

	CheckNumberMobilityAccountsL(3, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc3Name, *iSmtpAccSettings, iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberMobilityAccountsL(2, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc3Name, *iSmtpAccSettings, iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberMobilityAccountsL(1, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountDetailsL(KMobAcc3Name, *iSmtpAccSettings, iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc3);

	CheckNumberMobilityAccountsL(0, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	CleanupStack::PopAndDestroy(2, &iapList);

	INFO_PRINTF1(_L("Test3LinkedMobilityAccountsDelete123L - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0010
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 3 linked SMTP mobility accounts to same parent and deleting them in the order third, second, first
*/
void CTestSmtpMobilityAccounts::Test3LinkedMobilityAccountsDelete321L()
	{
	INFO_PRINTF1(_L("Test3LinkedMobilityAccountsDelete321L - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, linkedAcc, ETrue);

	RArrayIAP iapList2;
	CleanupClosePushL(iapList2);
	iapList2.AppendL(3);

	TSmtpMobilityAccount mobAcc3 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc3Name, iapList2, smtpAcc, linkedAcc, ETrue);

	CheckNumberMobilityAccountsL(3, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc3Name, *iSmtpAccSettings, iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc3);

	CheckNumberMobilityAccountsL(2, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberMobilityAccountsL(1, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberMobilityAccountsL(0, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	CleanupStack::PopAndDestroy(2, &iapList);

	INFO_PRINTF1(_L("Test3LinkedMobilityAccountsDelete321L - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0011
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 3 linked SMTP mobility accounts to same parent and deleting them in the order second, first, third
*/
void CTestSmtpMobilityAccounts::Test3LinkedMobilityAccountsDelete213L()
	{
	INFO_PRINTF1(_L("Test3LinkedMobilityAccountsDelete213L - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, linkedAcc, ETrue);

	RArrayIAP iapList2;
	CleanupClosePushL(iapList2);
	iapList2.AppendL(3);

	TSmtpMobilityAccount mobAcc3 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc3Name, iapList2, smtpAcc, linkedAcc, ETrue);

	CheckNumberMobilityAccountsL(3, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc2Name, *iSmtpAccSettings, iapList, ETrue, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc3Name, *iSmtpAccSettings, iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc2);

	CheckNumberMobilityAccountsL(2, smtpAcc);
	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountDetailsL(KMobAcc3Name, *iSmtpAccSettings, iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc1);

	CheckNumberMobilityAccountsL(1, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountDetailsL(KMobAcc3Name, *iSmtpAccSettings, iapList2, ETrue, smtpAcc);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc3);

	CheckNumberMobilityAccountsL(0, smtpAcc);
	CheckMobilityAccountMissingL(KMobAcc1Name);
	CheckMobilityAccountMissingL(KMobAcc2Name);
	CheckMobilityAccountMissingL(KMobAcc3Name);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	CleanupStack::PopAndDestroy(2, &iapList);

	INFO_PRINTF1(_L("Test3LinkedMobilityAccountsDelete213L - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-LNK-ACC-0012
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests changing linked mobility account IAP list
*/
void CTestSmtpMobilityAccounts::TestChangeLinkedMobilityAccountIapListL()
	{
	INFO_PRINTF1(_L("TestChangeLinkedMobilityAccountIapListL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	CheckNumberMobilityAccountsL(1);
	CheckNumberMobilityAccountsL(1, smtpAcc);

	CheckMobilityAccountDetailsL(KMobAcc1Name, *iSmtpAccSettings, iIapList, EFalse, smtpAcc);

	iIapList.AppendL(2);

	iEmailAccounts->SaveSmtpMobilityAccountIapListL(mobAcc, iIapList);

	iIapList.Reset();

	iEmailAccounts->LoadSmtpMobilityAccountIapListL(mobAcc, iIapList);

	ASSERT_EQUALS(iIapList.Count(), 2);
	ASSERT_TRUE(iIapList[0] == 1);
	ASSERT_TRUE(iIapList[1] == 2);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	INFO_PRINTF1(_L("TestChangeLinkedMobilityAccountIapListL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0001
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding SMTP mobility account linked to parent
*/
void CTestSmtpMobilityAccounts::TestMobilityAccountLinkedToParentL()
	{
	INFO_PRINTF1(_L("TestMobilityAccountLinkedToParentL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc;
	TRAPD(err, mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, smtpAcc, EFalse));

	ASSERT_EQUALS(err, KErrArgument);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestMobilityAccountLinkedToParentL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0002
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 2 SMTP mobility accounts with same parent and IAP
*/
void CTestSmtpMobilityAccounts::Test2MobilityAccountsWithSameParentAndIapL()
	{
	INFO_PRINTF1(_L("Test2MobilityAccountsWithSameParentAndIapL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	TSmtpMobilityAccount mobAcc2;
	TRAPD(err, mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iIapList, smtpAcc, *iMobAccSettings, EFalse));

	ASSERT_EQUALS(err, KErrAlreadyExists);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("Test2MobilityAccountsWithSameParentAndIapL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0003
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding SMTP mobility account with no IAPs
*/
void CTestSmtpMobilityAccounts::TestMobilityAccountWith0IapsL()
	{
	INFO_PRINTF1(_L("TestMobilityAccountWith0IapsL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);

	TSmtpMobilityAccount mobAcc;
	TRAPD(err, mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iapList, smtpAcc, smtpAcc, EFalse));

	ASSERT_EQUALS(err, KErrArgument);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("TestMobilityAccountWith0IapsL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0004
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding SMTP mobility account with 33 IAPs
*/
void CTestSmtpMobilityAccounts::TestMobilityAccountWith33IapsL()
	{
	INFO_PRINTF1(_L("TestMobilityAccountWith33IapsL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	for (TInt count = 1; count <= 33; ++count)
		{
		iapList.AppendL(count);
		}

	TSmtpMobilityAccount mobAcc;
	TRAPD(err, mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iapList, smtpAcc, smtpAcc, EFalse));

	ASSERT_EQUALS(err, KErrArgument);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("TestMobilityAccountWith33IapsL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0005
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding 33 SMTP mobility accounts to same parent
*/
void CTestSmtpMobilityAccounts::TestCreate33MobilityAccountsWithSameParentL()
	{
	INFO_PRINTF1(_L("TestCreate33MobilityAccountsWithSameParentL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);

	TSmtpMobilityAccount mobAcc;
	for (TInt count = 1; count <= 32; ++count)
		{
		iapList.AppendL(count);
		mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iapList, smtpAcc, *iMobAccSettings, EFalse);
		iapList.Remove(0);
		}

	iapList.AppendL(33);

	TRAPD(err, mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iapList, smtpAcc, *iMobAccSettings, EFalse));

	ASSERT_EQUALS(err, KErrOverflow);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("TestCreate33MobilityAccountsWithSameParentL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0006
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests adding SMTP mobility accounts with too long account name
*/
void CTestSmtpMobilityAccounts::TestCreateMobilityAccountWithTooLongAccountNameL()
	{
	INFO_PRINTF1(_L("TestCreateMobilityAccountWithTooLongAccountNameL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	_LIT(KMaxLengthName, "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456");

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMaxLengthName, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	_LIT(KTooLongName, "12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567");
	
	TSmtpMobilityAccount mobAcc2;
	TRAPD(err, mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KTooLongName, iIapList, smtpAcc, *iMobAccSettings, EFalse));

	ASSERT_EQUALS(err, KErrArgument);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestCreateMobilityAccountWithTooLongAccountNameL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0007
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests deleting non existing SMTP mobility account
*/
void CTestSmtpMobilityAccounts::TestDeleteNonExistentMobilityAccountL()
	{
	INFO_PRINTF1(_L("TestDeleteNonExistentMobilityAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc);

	TRAPD(err, iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc));

	ASSERT_EQUALS(err, KErrNotFound);	

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestDeleteNonExistentMobilityAccountL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0008
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests loading settings for non existent SMTP mobility account
*/
void CTestSmtpMobilityAccounts::TestLoadMobilitySettingsForNonExistentMobilityAccountL()
	{
	INFO_PRINTF1(_L("TestLoadMobilitySettingsForNonExistentMobilityAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc);

	TRAPD(err, iEmailAccounts->LoadSmtpMobilitySettingsL(mobAcc, *iMobAccSettings));

	ASSERT_EQUALS(err, KErrNotFound);	

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestLoadMobilitySettingsForNonExistentMobilityAccountL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0009
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests saving settings for linked SMTP mobility account
*/
void CTestSmtpMobilityAccounts::TestSaveMobilitySettingsForLinkedMobilityAccountL()
	{
	INFO_PRINTF1(_L("TestSaveMobilitySettingsForLinkedMobilityAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);
	TSmtpAccount linkedAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc2Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, linkedAcc, EFalse);

	TRAPD(err, iEmailAccounts->SaveSmtpMobilitySettingsL(mobAcc, *iMobAccSettings));

	ASSERT_EQUALS(err, KErrNotSupported);	

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);
	iEmailAccounts->DeleteSmtpAccountL(linkedAcc);

	INFO_PRINTF1(_L("TestSaveMobilitySettingsForLinkedMobilityAccountL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0010
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests saving settings for non existent SMTP mobility account
*/
void CTestSmtpMobilityAccounts::TestSaveMobilitySettingsForNonExistentMobilityAccountL()
	{
	INFO_PRINTF1(_L("TestSaveMobilitySettingsForNonExistentMobilityAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc);

	TRAPD(err, iEmailAccounts->SaveSmtpMobilitySettingsL(mobAcc, *iMobAccSettings));

	ASSERT_EQUALS(err, KErrNotFound);	

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestSaveMobilitySettingsForNonExistentMobilityAccountL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0011
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests loading IAP list for non existent SMTP mobility account
*/
void CTestSmtpMobilityAccounts::TestLoadIapListForNonExistentMobilityAccountL()
	{
	INFO_PRINTF1(_L("TestLoadIapListForNonExistentMobilityAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc);

	TRAPD(err, iEmailAccounts->LoadSmtpMobilityAccountIapListL(mobAcc, iIapList));

	ASSERT_EQUALS(err, KErrNotFound);	

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestLoadIapListForNonExistentMobilityAccountL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0012
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests saving IAP list with no IAPs
*/
void CTestSmtpMobilityAccounts::TestSaveIapListWith0IapsL()
	{
	INFO_PRINTF1(_L("TestSaveIapListWith0IapsL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);

	TRAPD(err, iEmailAccounts->SaveSmtpMobilityAccountIapListL(mobAcc, iapList));

	ASSERT_EQUALS(err, KErrArgument);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("TestSaveIapListWith0IapsL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0013
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests saving IAP list with 33 IAPs
*/
void CTestSmtpMobilityAccounts::TestSaveIapListWith33IapsL()
	{
	INFO_PRINTF1(_L("TestSaveIapListWith33IapsL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	for (TInt count = 1; count <= 33; ++count)
		{
		iapList.AppendL(count);
		}

	TRAPD(err, iEmailAccounts->SaveSmtpMobilityAccountIapListL(mobAcc, iapList));

	ASSERT_EQUALS(err, KErrArgument);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("TestSaveIapListWith33IapsL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0014
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests saving IAP list with IAP that already exists in the IAP list for another mobility account under the same parent SMTP account
*/
void CTestSmtpMobilityAccounts::TestSaveIapListWithExistingIapL()
	{
	INFO_PRINTF1(_L("TestSaveIapListWithExistingIapL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc1 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	RArrayIAP iapList;
	CleanupClosePushL(iapList);
	iapList.AppendL(2);

	TSmtpMobilityAccount mobAcc2 = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc2Name, iapList, smtpAcc, *iMobAccSettings, EFalse);

	iapList.AppendL(1);
	TRAPD(err, iEmailAccounts->SaveSmtpMobilityAccountIapListL(mobAcc2, iapList));

	ASSERT_EQUALS(err, KErrAlreadyExists);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CleanupStack::PopAndDestroy(&iapList);

	INFO_PRINTF1(_L("TestSaveIapListWithExistingIapL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0015
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests saving IAP list for non existent SMTP mobility account
*/
void CTestSmtpMobilityAccounts::TestSaveIapListForNonExistentMobilityAccountL()
	{
	INFO_PRINTF1(_L("TestSaveIapListForNonExistentMobilityAccountL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	iEmailAccounts->DeleteSmtpMobilityAccountL(mobAcc);

	TRAPD(err, iEmailAccounts->SaveSmtpMobilityAccountIapListL(mobAcc, iIapList));

	ASSERT_EQUALS(err, KErrNotFound);	

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestSaveIapListForNonExistentMobilityAccountL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-OTH-ACC-0016
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6981
@SYMTestCaseDesc Tests trying to perform SMTP account operations on SMTP mobility accounts, and vice versa
*/
void CTestSmtpMobilityAccounts::TestWrongAccountTypesL()
	{
	INFO_PRINTF1(_L("TestWrongAccountTypesL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	TInt err = KErrNone;
	TSmtpAccount brokenSmtpAcc;

	TRAP(err, iEmailAccounts->GetSmtpAccountL(mobAcc.iAccountId, brokenSmtpAcc));
	ASSERT_EQUALS(err, KErrNotFound);

	brokenSmtpAcc = smtpAcc;
	brokenSmtpAcc.iSmtpAccountId = mobAcc.iAccountId;

	TSmtpMobilityAccount brokenMobAcc = mobAcc;
	brokenMobAcc.iAccountId = smtpAcc.iSmtpAccountId;

	TRAP(err, iEmailAccounts->DeleteSmtpAccountL(brokenSmtpAcc));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->DeleteSmtpMobilityAccountL(brokenMobAcc));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->SetDefaultSmtpAccountL(brokenSmtpAcc));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->LoadSmtpSettingsL(brokenSmtpAcc, *iSmtpAccSettings));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->SaveSmtpSettingsL(brokenSmtpAcc, *iSmtpAccSettings));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->LoadSmtpIapSettingsL(brokenSmtpAcc, *iIapPrefs));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->SaveSmtpIapSettingsL(brokenSmtpAcc, *iIapPrefs));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->LoadSmtpMobilitySettingsL(brokenMobAcc, *iMobAccSettings));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->SaveSmtpMobilitySettingsL(brokenMobAcc, *iMobAccSettings));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->SaveSmtpMobilityAccountIapListL(brokenMobAcc, iIapList));
	ASSERT_EQUALS(err, KErrNotFound);

	TRAP(err, iEmailAccounts->LoadSmtpMobilityAccountIapListL(brokenMobAcc, iIapList));
	ASSERT_EQUALS(err, KErrNotFound);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestWrongAccountTypesL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-UT-0001
@SYMTestType UT
@SYMPREQ PREQ1307
@SYMTestCaseDesc Tests loading the mobility settings for a given SMTP account and IAP
*/
void CTestSmtpMobilityAccounts::TestLoadMobilitySettingsForSmtpAccountAndIapL()
	{
	INFO_PRINTF1(_L("TestLoadMobilitySettingsForSmtpAccountAndIapL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	CImSmtpMobilitySettings* mobAccSettings = CImSmtpMobilitySettings::NewLC();
	ASSERT_TRUE(iEmailAccounts->LoadSmtpMobilitySettingsL(smtpAcc.iSmtpAccountId, 1, *mobAccSettings));
	ASSERT_EQUALS(mobAccSettings->SmtpSettings(), iMobAccSettings->SmtpSettings());
	CleanupStack::PopAndDestroy(mobAccSettings);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestLoadMobilitySettingsForSmtpAccountAndIapL - End"));
	}

/**
@SYMTestCaseID SMTP-MOB-UT-0002
@SYMTestType UT
@SYMPREQ PREQ1307
@SYMTestCaseDesc Tests trying to load the mobility settings for a non existent SMTP account and IAP
*/
void CTestSmtpMobilityAccounts::TestLoadMobilitySettingsForNonExistentSmtpAccountAndIapL()
	{
	INFO_PRINTF1(_L("TestLoadMobilitySettingsForNonExistentSmtpAccountAndIapL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	CImSmtpMobilitySettings* mobAccSettings = CImSmtpMobilitySettings::NewLC();
	ASSERT_FALSE(iEmailAccounts->LoadSmtpMobilitySettingsL(smtpAcc.iSmtpAccountId, 1, *mobAccSettings));
	CleanupStack::PopAndDestroy(mobAccSettings);

	INFO_PRINTF1(_L("TestLoadMobilitySettingsForNonExistentSmtpAccountAndIapL - End"));	
	}

/**
@SYMTestCaseID SMTP-MOB-UT-0003
@SYMTestType UT
@SYMPREQ PREQ1307
@SYMTestCaseDesc Tests trying to load the mobility settings for a SMTP account and non existent IAP
*/
void CTestSmtpMobilityAccounts::TestLoadMobilitySettingsForSmtpAccountAndNonExistentIapL()
	{
	INFO_PRINTF1(_L("TestLoadMobilitySettingsForSmtpAccountAndNonExistentIapL - Start"));

	TSmtpAccount smtpAcc = iEmailAccounts->CreateSmtpAccountL(KSmtpAcc1Name, *iSmtpAccSettings, *iIapPrefs, EFalse);

	TSmtpMobilityAccount mobAcc = iEmailAccounts->CreateSmtpMobilityAccountL(KMobAcc1Name, iIapList, smtpAcc, *iMobAccSettings, EFalse);

	CImSmtpMobilitySettings* mobAccSettings = CImSmtpMobilitySettings::NewLC();
	ASSERT_FALSE(iEmailAccounts->LoadSmtpMobilitySettingsL(smtpAcc.iSmtpAccountId, 2, *mobAccSettings));
	CleanupStack::PopAndDestroy(mobAccSettings);

	iEmailAccounts->DeleteSmtpAccountL(smtpAcc);

	INFO_PRINTF1(_L("TestLoadMobilitySettingsForSmtpAccountAndNonExistentIapL - End"));	
	}

/**
Creates the test suite

@param aName Test suite name

@return Test suite
*/
CTestSuite* CTestSmtpMobilityAccounts::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	ADD_ASYNC_TEST_STEP(TestSmtpAccountL);

	ADD_ASYNC_TEST_STEP(Test1MobilityAccountL);
	ADD_ASYNC_TEST_STEP(Test1MobilityAccountWithParentDeletionL);
	ADD_ASYNC_TEST_STEP(Test2MobilityAccountsL);
	ADD_ASYNC_TEST_STEP(Test2MobilityAccountsWithParentDeletionL);
	ADD_ASYNC_TEST_STEP(Test2MobilityAccountsWithSameParentDeletionL);
	ADD_ASYNC_TEST_STEP(Test2MobilityAccountsWithSameParentL);
	ADD_ASYNC_TEST_STEP(TestMobilityAccountWithPopL);
	ADD_ASYNC_TEST_STEP(TestMobilityAccountWithImapL);
	ADD_ASYNC_TEST_STEP(Test3MobilityAccountsDelete123L);
	ADD_ASYNC_TEST_STEP(Test3MobilityAccountsDelete321L);
	ADD_ASYNC_TEST_STEP(Test3MobilityAccountsDelete213L);
	ADD_ASYNC_TEST_STEP(TestChangeMobilityAccountSettingsL);
	ADD_ASYNC_TEST_STEP(TestChangeMobilityAccountIapListL);

	ADD_ASYNC_TEST_STEP(Test1LinkedMobilityAccountL);
	ADD_ASYNC_TEST_STEP(Test1LinkedMobilityAccountWithLinkedDeletionL);
	ADD_ASYNC_TEST_STEP(Test2LinkedMobilityAccountsL);
	ADD_ASYNC_TEST_STEP(Test2LinkedMobilityAccountsWithLinkedDeletionL);
	ADD_ASYNC_TEST_STEP(Test2LinkedMobilityAccountsWithSameLinkedDeletionL);
	ADD_ASYNC_TEST_STEP(Test2LinkedMobilityAccountsWithSameLinkedL);
	ADD_ASYNC_TEST_STEP(TestLinkedMobilityAccountWithPopL);
	ADD_ASYNC_TEST_STEP(TestLinkedMobilityAccountWithImapL);
	ADD_ASYNC_TEST_STEP(Test3LinkedMobilityAccountsDelete123L);
	ADD_ASYNC_TEST_STEP(Test3LinkedMobilityAccountsDelete321L);
	ADD_ASYNC_TEST_STEP(Test3LinkedMobilityAccountsDelete213L);
	ADD_ASYNC_TEST_STEP(TestChangeLinkedMobilityAccountIapListL);

	ADD_ASYNC_TEST_STEP(TestMobilityAccountLinkedToParentL);
	ADD_ASYNC_TEST_STEP(Test2MobilityAccountsWithSameParentAndIapL);
	ADD_ASYNC_TEST_STEP(TestMobilityAccountWith0IapsL);
	ADD_ASYNC_TEST_STEP(TestMobilityAccountWith33IapsL);
	ADD_ASYNC_TEST_STEP(TestCreate33MobilityAccountsWithSameParentL);
	ADD_ASYNC_TEST_STEP(TestCreateMobilityAccountWithTooLongAccountNameL);
	ADD_ASYNC_TEST_STEP(TestDeleteNonExistentMobilityAccountL);
	ADD_ASYNC_TEST_STEP(TestLoadMobilitySettingsForNonExistentMobilityAccountL);
	ADD_ASYNC_TEST_STEP(TestSaveMobilitySettingsForLinkedMobilityAccountL);
	ADD_ASYNC_TEST_STEP(TestSaveMobilitySettingsForNonExistentMobilityAccountL);
	ADD_ASYNC_TEST_STEP(TestLoadIapListForNonExistentMobilityAccountL);
	ADD_ASYNC_TEST_STEP(TestSaveIapListWith0IapsL);
	ADD_ASYNC_TEST_STEP(TestSaveIapListWith33IapsL);
	ADD_ASYNC_TEST_STEP(TestSaveIapListWithExistingIapL);
	ADD_ASYNC_TEST_STEP(TestSaveIapListForNonExistentMobilityAccountL);
	ADD_ASYNC_TEST_STEP(TestWrongAccountTypesL);

	ADD_ASYNC_TEST_STEP(TestLoadMobilitySettingsForSmtpAccountAndIapL);
	ADD_ASYNC_TEST_STEP(TestLoadMobilitySettingsForNonExistentSmtpAccountAndIapL);
	ADD_ASYNC_TEST_STEP(TestLoadMobilitySettingsForSmtpAccountAndNonExistentIapL);

	END_SUITE;
	}
