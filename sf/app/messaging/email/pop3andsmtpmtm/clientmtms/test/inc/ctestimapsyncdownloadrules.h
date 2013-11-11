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

#ifndef __CTESTIMAPSYNCDOWNLOADRULES_H__
#define __CTESTIMAPSYNCDOWNLOADRULES_H__

#include <test/tefunit.h>
#include <imapset.h>

class CImapSyncDownloadRules;

/**
@internalTechnology
@prototype
*/
class CTestImapSyncDownloadRules : public CActiveTestFixture
	{
public:
	CTestImapSyncDownloadRules();
	~CTestImapSyncDownloadRules();
	
	virtual void SetupL();

	void Reset();	
	void TestCreateEmptyL();
	void TestAddRulesL();
	void TestAddRuleInvalidArgumentL();
	void TestAddRuleBearerTypeAlreadyExistsL();
	void TestAddRuleOverflowL();
	void TestDeleteRulesL();
	void TestDeleteRuleInvalidArgumentL();
	void TestDeleteBearerTypesL();
	void TestDeleteBearerTypesInvalidArgumentL();
	void TestGetRuleByPosInvalidArgumentL();
	void TestGetRuleByBearerL();
	void TestGetRuleByBearerInvalidArgumentL();
	void TestResetL();
	void TestCopyL();

	static CTestSuite* CreateSuiteL(const TDesC& aName);
	
private:
	CImapSyncDownloadRules* iRules;
	TUint32 iBearerTypes;
	TImImap4GetPartialMailInfo iMailInfo;
	};

#endif __CTESTIMAPSYNCDOWNLOADRULES_H__
