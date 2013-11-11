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

#ifndef __CTESTSMTPMOBILITYACCOUNTS_H__
#define __CTESTSMTPMOBILITYACCOUNTS_H__

#include <test/tefunit.h>
#include "cemailaccounts.h"

class CImSmtpSettings;
class CImIAPPreferences;
class CImSmtpMobilitySettings;
class CImSmtpMobilitySettings;

/**
@internalTechnology
@prototype
*/
class CTestSmtpMobilityAccounts : public CActiveTestFixture
	{
public:
	CTestSmtpMobilityAccounts();
	~CTestSmtpMobilityAccounts();
	
	virtual void SetupL();
	virtual void TearDownL();

	void TestSmtpAccountL();
	void Test1MobilityAccountL();
	void Test1MobilityAccountWithParentDeletionL();
	void Test2MobilityAccountsL();
	void Test2MobilityAccountsWithParentDeletionL();
	void Test2MobilityAccountsWithSameParentDeletionL();
	void Test2MobilityAccountsWithSameParentL();
	void TestMobilityAccountWithPopL();
	void TestMobilityAccountWithImapL();
	void Test3MobilityAccountsDelete123L();
	void Test3MobilityAccountsDelete321L();
	void Test3MobilityAccountsDelete213L();
	void TestChangeMobilityAccountSettingsL();
	void TestChangeMobilityAccountIapListL();

	void Test1LinkedMobilityAccountL();
	void Test1LinkedMobilityAccountWithLinkedDeletionL();
	void Test2LinkedMobilityAccountsL();
	void Test2LinkedMobilityAccountsWithLinkedDeletionL();
	void Test2LinkedMobilityAccountsWithSameLinkedDeletionL();
	void Test2LinkedMobilityAccountsWithSameLinkedL();
	void TestLinkedMobilityAccountWithPopL();
	void TestLinkedMobilityAccountWithImapL();
	void Test3LinkedMobilityAccountsDelete123L();
	void Test3LinkedMobilityAccountsDelete321L();
	void Test3LinkedMobilityAccountsDelete213L();
	void TestChangeLinkedMobilityAccountIapListL();

	void TestMobilityAccountLinkedToParentL();
	void Test2MobilityAccountsWithSameParentAndIapL();
	void TestMobilityAccountWith0IapsL();
	void TestMobilityAccountWith33IapsL();
	void TestCreate33MobilityAccountsWithSameParentL();
	void TestCreateMobilityAccountWithTooLongAccountNameL();
	void TestDeleteNonExistentMobilityAccountL();
	void TestLoadMobilitySettingsForNonExistentMobilityAccountL();
	void TestSaveMobilitySettingsForLinkedMobilityAccountL();
	void TestSaveMobilitySettingsForNonExistentMobilityAccountL();
	void TestLoadIapListForNonExistentMobilityAccountL();
	void TestSaveIapListWith0IapsL();
	void TestSaveIapListWith33IapsL();
	void TestSaveIapListWithExistingIapL();
	void TestSaveIapListForNonExistentMobilityAccountL();
	void TestWrongAccountTypesL();

	void TestLoadMobilitySettingsForSmtpAccountAndIapL();
	void TestLoadMobilitySettingsForNonExistentSmtpAccountAndIapL();
	void TestLoadMobilitySettingsForSmtpAccountAndNonExistentIapL();

	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	void ResetAccountsL();
	void CheckNumberSMTPAccountsL(TInt aNum);
	void CheckSmtpAccountSettingsL(TSmtpAccount aAccount, const CImSmtpSettings& aSettings);
	void CheckNumberMobilityAccountsL(TInt aNum);
	void CheckNumberMobilityAccountsL(TInt aNum, TSmtpAccount aSmtpAcc);
	void CheckMobilityAccountDetailsL(const TDesC& aAccName, const CImSmtpSettings& aSmtpSettings, const RArrayIAP& aIapList, TBool aReadOnly, TSmtpAccount aSmtpAcc);
	void CheckMobilityAccountMissingL(const TDesC& aAccName);

private:
	CEmailAccounts* iEmailAccounts;
	CImSmtpSettings* iSmtpAccSettings;
	CImIAPPreferences* iIapPrefs;
	CImSmtpMobilitySettings* iMobAccSettings;
	RArrayIAP iIapList;
	};

#endif __CTESTSMTPMOBILITYACCOUNTS_H__
