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

#ifndef __CTESTIMAPTRANSPORTBUFFERSIZES_H__
#define __CTESTIMAPTRANSPORTBUFFERSIZES_H__

#include <test/tefunit.h>

class CEmailAccounts;
class CImapTransportBufferSizes;

/**
@internalTechnology
@prototype
*/
class CTestImapTransportBufferSizes : public CActiveTestFixture
	{
public:
	CTestImapTransportBufferSizes();
	~CTestImapTransportBufferSizes();

	virtual void SetupL();

	// Tests defined in the test spec
	void TestGetAndSetBearerSettingsL();
	void TestAddingRulesL();
	void TestGettingRulesByPosL();
	void TestGettingRulesByBearerTypeL();
	void TestDeleteByPosL();
	void TestDeleteByBearerTypeL();

	// Extra transport buffer sizes tests	
	void TestCreateEmptyL();
	void TestAddEntriesL();
	void TestAddEntryInvalidArgumentL();
	void TestAddEntryBearerTypeAlreadyExistsL();
	void TestAddEntryOverflowL();
	void TestDeleteEntriesL();
	void TestDeleteEntryInvalidArgumentL();
	void TestDeleteBearerTypesL();
	void TestDeleteBearerTypesInvalidArgumentL();
	void TestGetEntryByPosInvalidArgumentL();
	void TestGetEntryByBearerL();
	void TestGetEntryByBearerInvalidArgumentL();
	void TestResetL();
	void TestCopyL();

	// Extra email account tests
	void TestDeleteEmptyL();
	void TestSaveFullL();
	void TestChangeFullToFullL();
	void TestChangeFullToEmptyL();
	void TestChangeEmptyToFullL();
	void TestDeleteFullL();

	static CTestSuite* CreateSuiteL(const TDesC& aName);

private:
	enum
		{
		KBearerTypes000001 = 1,
		KBearerTypes000010 = 2,
		KBearerTypes000100 = 4,
		KBearerTypes001000 = 8,
		KBearerTypes001100 = 12
		};

private:
	void SetAllL(TInt aVal);
	void SetValues(TInt aVal);
	void LoadAndCheckSizesL(TInt aNumSizesExpected, TInt aVal);
	void CheckEntryByPosL(TInt aPos, TInt aBearerTypes);
	void CheckEntryByBearerTypeL(TInt aBearerTypesToFind, TInt aExpectedPos, TInt aBearerTypesValues);

private:
	CEmailAccounts* iEmailAccounts;
	CImapTransportBufferSizes* iSizes;
	TUint32 iBearerTypes;
	TInt iFetchRequestSize;
	TInt iMaxOutstandingFetchResponses;
	};

#endif __CTESTIMAPTRANSPORTBUFFERSIZES_H__
