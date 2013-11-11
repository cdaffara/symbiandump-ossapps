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

#ifndef __CTESTTLSSSLDOMAINSETTING_H__
#define __CTESTTLSSSLDOMAINSETTING_H__

#include <test/tefunit.h>

class CEmailAccounts;

/**
@internalTechnology
@prototype
*/
class CTestTlsSslDomainSetting : public CActiveTestFixture
	{
public:
	CTestTlsSslDomainSetting();
	~CTestTlsSslDomainSetting();

	virtual void SetupL();

	void TestImapSettingL();
	void TestPopSettingL();
	void TestSmtpSettingL();

	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	CEmailAccounts* iEmailAccounts;
	};

#endif __CTESTTLSSSLDOMAINSETTING_H__
