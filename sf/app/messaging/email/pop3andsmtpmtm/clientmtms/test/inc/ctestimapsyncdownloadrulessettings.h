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

#ifndef __CTESTIMAPSYNCDOWNLOADRULESSETTINGS_H__
#define __CTESTIMAPSYNCDOWNLOADRULESSETTINGS_H__

#include <test/tefunit.h>
#include <imapset.h>

class CEmailAccounts;
class CImapSyncDownloadRules;
class TImapAccount;

/**
@internalTechnology
@prototype
*/
class CTestImapSyncDownloadRulesSettings : public CActiveTestFixture
	{
public:
	CTestImapSyncDownloadRulesSettings();
	~CTestImapSyncDownloadRulesSettings();
	
	virtual void SetupL();
	virtual void TearDownL();

	void SetAllRulesL(TInt aVal);
	void SetRulesValues(TInt aVal);
	void CheckRulesL(const TImapAccount& aAccount, TInt aNumRulesExpected, TInt aVal);
	TBool FindAccountL(const TDesC& aAccName, TImapAccount& aAccount);
	TBool CreateImapAccountL(const TDesC& aAccName, TImapAccount& aAccount, TBool aUseRules);
	void TestCreateImapAccountFullL();
	void TestCreateImapAccountEmptyL();
	void TestChangeSyncDownloadRulesFullToFullL();
	void TestChangeSyncDownloadRulesFullToEmptyL();
	void TestChangeSyncDownloadRulesEmptyToFullL();
	void TestDeleteImapAccountFullL();
	void TestDeleteImapAccountEmptyL();
	void TestAddRulesToExistingAccountL();

	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	CEmailAccounts* iEmailAccounts;
	CImapSyncDownloadRules* iRules;
	TUint32 iBearerTypes;
	TImImap4GetPartialMailInfo iMailInfo;
	};

#endif __CTESTIMAPSYNCDOWNLOADRULESSETTINGS_H__
