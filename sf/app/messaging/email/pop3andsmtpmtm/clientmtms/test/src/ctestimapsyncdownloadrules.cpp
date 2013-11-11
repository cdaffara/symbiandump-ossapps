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

#include "ctestimapsyncdownloadrules.h"
#include "cimapsyncdownloadrules.h"
#include <cdbcols.h>

/**
Constructor
*/
CTestImapSyncDownloadRules::CTestImapSyncDownloadRules()
	{
	}

/**
Destructor
*/
CTestImapSyncDownloadRules::~CTestImapSyncDownloadRules()
	{
	delete iRules;
	}

/**
Test setup
*/
void CTestImapSyncDownloadRules::SetupL()
	{
	iRules = CImapSyncDownloadRules::NewL();
	Reset();
	}

/**
Reset member variables
*/
void CTestImapSyncDownloadRules::Reset()
	{
	iBearerTypes = KCommDbBearerCSD;
	iMailInfo.iTotalSizeLimit = KMaxTInt;
	iMailInfo.iBodyTextSizeLimit = KMaxTInt;
	iMailInfo.iAttachmentSizeLimit = KMaxTInt;
	iMailInfo.iPartialMailOptions = ENoSizeLimits;
	iMailInfo.iMaxEmailSize = KMaxTInt;
	iMailInfo.iGetMailBodyParts = EGetImap4EmailHeaders;
	iMailInfo.iDestinationFolder = 0;
	}

/**
Test create empty rules list
*/
void CTestImapSyncDownloadRules::TestCreateEmptyL()
	{
	INFO_PRINTF1(_L("TestCreateEmptyL - Start"));

	ASSERT_FALSE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));

	CImapSyncDownloadRules* rules = CImapSyncDownloadRules::NewLC();
	ASSERT_FALSE(rules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));
	CleanupStack::PopAndDestroy(rules);

	INFO_PRINTF1(_L("TestCreateEmptyL - Complete"));
	}

/**
Test adding new rules
*/
void CTestImapSyncDownloadRules::TestAddRulesL()
	{
	INFO_PRINTF1(_L("TestAddRulesL - Start"));

	iMailInfo.iBodyTextSizeLimit = 2;
	iMailInfo.iAttachmentSizeLimit = 3;
	iMailInfo.iPartialMailOptions = EBodyTextOnly;
	iMailInfo.iMaxEmailSize = 4;
	iMailInfo.iGetMailBodyParts = EGetImap4EmailBodyText;

	iBearerTypes = KCommDbBearerWLAN;
	iMailInfo.iTotalSizeLimit = 1;
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerWcdma;
	iMailInfo.iTotalSizeLimit = 100;
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerLAN;
	iMailInfo.iTotalSizeLimit = 200;
	ASSERT_EQUALS(2, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 31, iBearerTypes, iMailInfo));

	Reset();
	ASSERT_TRUE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iBodyTextSizeLimit), 2);
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iAttachmentSizeLimit), 3);
	ASSERT_EQUALS(iMailInfo.iPartialMailOptions, EBodyTextOnly);
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iMaxEmailSize), 4);
	ASSERT_EQUALS(iMailInfo.iGetMailBodyParts, EGetImap4EmailBodyText);
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerWcdma));
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iTotalSizeLimit), 100);

	Reset();
	ASSERT_TRUE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 1, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerWLAN));
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iTotalSizeLimit), 1);

	Reset();
	ASSERT_TRUE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 2, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerLAN));
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iTotalSizeLimit), 200);

	iBearerTypes = KCommDbBearerLAN;
	iMailInfo.iTotalSizeLimit = 200;
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EFolderRulesType, 0, iBearerTypes, iMailInfo));
	Reset();

	ASSERT_TRUE(iRules->GetRuleL(CImapSyncDownloadRules::EFolderRulesType, 0, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerLAN));
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iTotalSizeLimit), 200);

	INFO_PRINTF1(_L("TestAddRulesL - Complete"));
	}

/**
Test passing invalid parameters to add rule routine
*/
void CTestImapSyncDownloadRules::TestAddRuleInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestAddRuleInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	TInt err = KErrNone;
	TRAP(err, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, -1, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(err, KErrArgument);

	err = KErrNone;
	TRAP(err, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, KMaxImapSyncDownloadRules, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(err, KErrArgument);

	iBearerTypes = 0;
	err = KErrNone;
	TRAP(err, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestAddRuleInvalidArgumentL - Complete"));
	}

/**
Test trying to add a rule with a bearer type that already exists
*/
void CTestImapSyncDownloadRules::TestAddRuleBearerTypeAlreadyExistsL()
	{
	INFO_PRINTF1(_L("TestAddRuleBearerTypeAlreadyExistsL - Start"));

	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(KErrAlreadyExists, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 1, iBearerTypes, iMailInfo));

	INFO_PRINTF1(_L("TestAddRuleBearerTypeAlreadyExistsL - Complete"));
	}

/**
Test trying to add too many rules
*/
void CTestImapSyncDownloadRules::TestAddRuleOverflowL()
	{
	INFO_PRINTF1(_L("TestAddRuleOverflowL - Start"));

	iBearerTypes = 1;
	for (TInt rule = 0; rule < KMaxImapSyncDownloadRules; ++rule)
		{
		ASSERT_EQUALS(rule, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, rule, iBearerTypes, iMailInfo));
		iBearerTypes <<= 1;
		}

	iBearerTypes = 1;	
	ASSERT_EQUALS(KErrOverflow, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));

	INFO_PRINTF1(_L("TestAddRuleOverflowL - Complete"));
	}

/**
Test rule deletion
*/
void CTestImapSyncDownloadRules::TestDeleteRulesL()
	{
	INFO_PRINTF1(_L("TestDeleteRulesL - Start"));

	iBearerTypes = KCommDbBearerPAN;
	iMailInfo.iTotalSizeLimit = 1;
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerWcdma;
	iMailInfo.iTotalSizeLimit = 100;
	ASSERT_EQUALS(1, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 1, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerLAN;
	iMailInfo.iTotalSizeLimit = 200;
	ASSERT_EQUALS(2, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 2, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerWLAN;
	iMailInfo.iTotalSizeLimit = 300;
	ASSERT_EQUALS(3, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 3, iBearerTypes, iMailInfo));

	Reset();

	ASSERT_TRUE(iRules->DeleteRuleL(CImapSyncDownloadRules::EInboxRulesType, 2));
	ASSERT_TRUE(iRules->DeleteRuleL(CImapSyncDownloadRules::EInboxRulesType, 0));

	ASSERT_TRUE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerWcdma));
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iTotalSizeLimit), 100);

	Reset();
	ASSERT_TRUE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 1, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerWLAN));
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iTotalSizeLimit), 300);

	Reset();
	ASSERT_FALSE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 2, iBearerTypes, iMailInfo));
	ASSERT_FALSE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 3, iBearerTypes, iMailInfo));

	INFO_PRINTF1(_L("TestDeleteRulesL - Complete"));
	}

/**
Test passing invalid parameters to add delete routine
*/
void CTestImapSyncDownloadRules::TestDeleteRuleInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestDeleteRuleInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	TInt err = KErrNone;
	TRAP(err, iRules->DeleteRuleL(CImapSyncDownloadRules::EInboxRulesType, -1));
	ASSERT_EQUALS(err, KErrArgument);

	err = KErrNone;
	TRAP(err, iRules->DeleteRuleL(CImapSyncDownloadRules::EInboxRulesType, KMaxImapSyncDownloadRules));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestDeleteRuleInvalidArgumentL - Complete"));
	}

/**
Test deleting rules by bearer type
*/
void CTestImapSyncDownloadRules::TestDeleteBearerTypesL()
	{
	INFO_PRINTF1(_L("TestDeleteBearerTypesL - Start"));

	iBearerTypes = KCommDbBearerPAN | KCommDbBearerWcdma;
	iMailInfo.iTotalSizeLimit = 1;
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerLAN;
	iMailInfo.iTotalSizeLimit = 200;
	ASSERT_EQUALS(1, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 1, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerWLAN;
	iMailInfo.iTotalSizeLimit = 300;
	ASSERT_EQUALS(2, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 2, iBearerTypes, iMailInfo));

	TBool ruleRemoved;

	ASSERT_EQUALS(0, iRules->DeleteBearerTypesL(CImapSyncDownloadRules::EInboxRulesType, KCommDbBearerPAN, ruleRemoved));
	ASSERT_FALSE(ruleRemoved);
	ASSERT_EQUALS(1, iRules->DeleteBearerTypesL(CImapSyncDownloadRules::EInboxRulesType, KCommDbBearerLAN , ruleRemoved));
	ASSERT_TRUE(ruleRemoved);
	ASSERT_FALSE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 2, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(KErrNotFound, iRules->DeleteBearerTypesL(CImapSyncDownloadRules::EInboxRulesType, KCommDbBearerWcdma | KCommDbBearerWLAN, ruleRemoved));

	INFO_PRINTF1(_L("TestDeleteBearerTypesL - Complete"));
	}

/**
Test passing invalid parameters to delete bearer types routine
*/
void CTestImapSyncDownloadRules::TestDeleteBearerTypesInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestDeleteBearerTypesInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	iBearerTypes = 0;
	TInt err = KErrNone;
	TBool ruleRemoved;
	TRAP(err, iRules->DeleteBearerTypesL(CImapSyncDownloadRules::EInboxRulesType, iBearerTypes, ruleRemoved));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestDeleteBearerTypesInvalidArgumentL - Complete"));
	}

/**
Test passing invalid parameters to get rule routine
*/
void CTestImapSyncDownloadRules::TestGetRuleByPosInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestGetRuleByPosInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	TInt err = KErrNone;
	TRAP(err, iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, -1, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(err, KErrArgument);

	err = KErrNone;
	TRAP(err, iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, KMaxImapSyncDownloadRules, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestGetRuleByPosInvalidArgumentL - Complete"));
	}

/**
Test getting rules for bearer types
*/
void CTestImapSyncDownloadRules::TestGetRuleByBearerL()
	{
	INFO_PRINTF1(_L("TestGetRuleByBearerL - Start"));

	iBearerTypes = KCommDbBearerPAN | KCommDbBearerWcdma;
	iMailInfo.iTotalSizeLimit = 1;
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerLAN;
	iMailInfo.iTotalSizeLimit = 200;
	ASSERT_EQUALS(1, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 1, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerWLAN;
	iMailInfo.iTotalSizeLimit = 300;
	ASSERT_EQUALS(2, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 2, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerPAN;
	ASSERT_EQUALS(0, iRules->GetMailInfoL(CImapSyncDownloadRules::EInboxRulesType, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerPAN | KCommDbBearerWcdma;
	ASSERT_EQUALS(0, iRules->GetMailInfoL(CImapSyncDownloadRules::EInboxRulesType, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerPAN | KCommDbBearerWLAN;
	ASSERT_EQUALS(KErrNotFound, iRules->GetMailInfoL(CImapSyncDownloadRules::EInboxRulesType, iBearerTypes, iMailInfo));

	iBearerTypes = KCommDbBearerWLAN;
	ASSERT_EQUALS(2, iRules->GetMailInfoL(CImapSyncDownloadRules::EInboxRulesType, iBearerTypes, iMailInfo));

	INFO_PRINTF1(_L("TestGetRuleByBearerL - Complete"));	
	}

/**
Test passing invalid parameters to get rule routine
*/
void CTestImapSyncDownloadRules::TestGetRuleByBearerInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestGetRuleByBearerInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	iBearerTypes = 0;
	TInt err = KErrNone;
	TRAP(err, iRules->GetMailInfoL(CImapSyncDownloadRules::EInboxRulesType, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestGetRuleByBearerInvalidArgumentL - Complete"));
	}

/**
Test class reset
*/
void CTestImapSyncDownloadRules::TestResetL()
	{
	INFO_PRINTF1(_L("TestResetL - Start"));

	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EFolderRulesType, 0, iBearerTypes, iMailInfo));

	iRules->Reset();
	ASSERT_FALSE(iRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));
	ASSERT_FALSE(iRules->GetRuleL(CImapSyncDownloadRules::EFolderRulesType, 0, iBearerTypes, iMailInfo));

	INFO_PRINTF1(_L("TestResetL - Complete"));
	}

/**
Test class copy
*/
void CTestImapSyncDownloadRules::TestCopyL()
	{
	INFO_PRINTF1(_L("TestCopyL - Start"));

	iMailInfo.iBodyTextSizeLimit = 1;
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, iBearerTypes, iMailInfo));
	iMailInfo.iAttachmentSizeLimit = 2;
	ASSERT_EQUALS(0, iRules->AddRuleL(CImapSyncDownloadRules::EFolderRulesType, 0, iBearerTypes, iMailInfo));

	CImapSyncDownloadRules* newRules = iRules->CopyL();
	CleanupStack::PushL(newRules);

	TUint32 bearerTypes;
	ASSERT_TRUE(newRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, bearerTypes, iMailInfo));
	ASSERT_EQUALS(bearerTypes, iBearerTypes);
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iBodyTextSizeLimit), 1);

	ASSERT_FALSE(newRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 1, iBearerTypes, iMailInfo));

	ASSERT_TRUE(newRules->GetRuleL(CImapSyncDownloadRules::EFolderRulesType, 0, bearerTypes, iMailInfo));
	ASSERT_EQUALS(bearerTypes, iBearerTypes);
	ASSERT_EQUALS(static_cast<TInt>(iMailInfo.iAttachmentSizeLimit), 2);

	ASSERT_FALSE(newRules->GetRuleL(CImapSyncDownloadRules::EFolderRulesType, 1, iBearerTypes, iMailInfo));

	CleanupStack::PopAndDestroy(newRules);

	newRules = iRules->CopyLC();
	ASSERT_TRUE(newRules->GetRuleL(CImapSyncDownloadRules::EInboxRulesType, 0, bearerTypes, iMailInfo));
	ASSERT_TRUE(newRules->GetRuleL(CImapSyncDownloadRules::EFolderRulesType, 0, bearerTypes, iMailInfo));
	CleanupStack::PopAndDestroy(newRules);

	INFO_PRINTF1(_L("TestCopyL - Complete"));
	}

CTestSuite* CTestImapSyncDownloadRules::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_TEST_STEP(TestCreateEmptyL);
	ADD_TEST_STEP(TestAddRulesL);
	ADD_TEST_STEP(TestAddRuleInvalidArgumentL);
	ADD_TEST_STEP(TestAddRuleBearerTypeAlreadyExistsL);
	ADD_TEST_STEP(TestAddRuleOverflowL);
	ADD_TEST_STEP(TestDeleteRulesL);
	ADD_TEST_STEP(TestDeleteRuleInvalidArgumentL);
	ADD_TEST_STEP(TestDeleteBearerTypesL);
	ADD_TEST_STEP(TestDeleteBearerTypesInvalidArgumentL);
	ADD_TEST_STEP(TestGetRuleByPosInvalidArgumentL);
	ADD_TEST_STEP(TestGetRuleByBearerL);	
	ADD_TEST_STEP(TestGetRuleByBearerInvalidArgumentL);
	ADD_TEST_STEP(TestResetL);
	ADD_TEST_STEP(TestCopyL);
	END_SUITE;
	}
