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

#include "ctestimaptransportbuffersizes.h"
#include "cemailaccounts.h"
#include "cimaptransportbuffersizes.h"
#include <cdbcols.h>

/**
Constructor
*/
CTestImapTransportBufferSizes::CTestImapTransportBufferSizes()
	{
	}

/**
Destructor
*/
CTestImapTransportBufferSizes::~CTestImapTransportBufferSizes()
	{
	delete iEmailAccounts;
	delete iSizes;
	}

/**
Test setup
*/
void CTestImapTransportBufferSizes::SetupL()
	{
	iEmailAccounts = CEmailAccounts::NewL();
	iSizes = CImapTransportBufferSizes::NewL();
	SetValues(KBearerTypes000001);
	}

/**
Fill in all the transport buffer sizes

@param aVal Bearer types used to set the sizes values
*/
void CTestImapTransportBufferSizes::SetAllL(TInt aVal)
	{
	SetValues(aVal);

	iBearerTypes = 1;
	for (TInt entry = 0; entry < KMaxImapTransportBufferSizes; ++entry)
		{
		ASSERT_EQUALS(entry, iSizes->AddSizesL(entry, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
		iBearerTypes <<= 1;
		}
	}

/**
Set the values to use in buffer sizes entries

@param aVal Bearer types used to set the sizes values
*/
void CTestImapTransportBufferSizes::SetValues(TInt aVal)
	{
	iBearerTypes = aVal;
	iFetchRequestSize = 1024 * aVal;
	iMaxOutstandingFetchResponses = ((aVal - 1) % 8) + 1;
	}

/**
Check that the currently saved buffer sizes are as expected

@param aNumSizesExpected Number of size entries to expect
@param aVal Bearer types used to set the sizes values
*/
void CTestImapTransportBufferSizes::LoadAndCheckSizesL(TInt aNumSizesExpected, TInt aVal)
	{
	delete iSizes;
	iSizes = NULL;
	iSizes = CImapTransportBufferSizes::NewL();

	TInt err = KErrNone;
	TRAP(err, iEmailAccounts->LoadImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	TUint32 bearerTypes = 1;
	SetValues(aVal);
	TInt fetchRequestSize = iFetchRequestSize;
	TInt maxOutstandingFetchResponses = iMaxOutstandingFetchResponses;

	TInt entry = 0;
	while (entry < KMaxImapTransportBufferSizes)
		{
		SetValues(32);
		if (iSizes->GetSizesL(entry, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses))
			{
			ASSERT_EQUALS(bearerTypes, iBearerTypes);
			ASSERT_EQUALS(fetchRequestSize, iFetchRequestSize);
			ASSERT_EQUALS(maxOutstandingFetchResponses, iMaxOutstandingFetchResponses);
			}
		else
			{
			break;
			}

		bearerTypes <<= 1;
		++entry;
		}

	ASSERT_EQUALS(entry, aNumSizesExpected);
	}

/**
Checks the entry at a given position in the sizes list has the expected values

@param aPos Position in list to check
@param aBearerTypes Used to determine what values to expect
*/
void CTestImapTransportBufferSizes::CheckEntryByPosL(TInt aPos, TInt aBearerTypes)
	{
	SetValues(aBearerTypes);

	TUint32 bearerTypes;
	TInt fetchRequestSize;
	TInt maxOutstandingFetchResponses;

	if (iSizes->GetSizesL(aPos, bearerTypes, fetchRequestSize, maxOutstandingFetchResponses))
		{
		ASSERT_EQUALS(bearerTypes, iBearerTypes);
		ASSERT_EQUALS(fetchRequestSize, iFetchRequestSize);
		ASSERT_EQUALS(maxOutstandingFetchResponses, iMaxOutstandingFetchResponses);
		}
	else
		{
		ASSERT_TRUE(EFalse);
		}
	}

/**
Finds an entry in the list for a given bearer type, and checks that it has the
expected values

@param aBearerTypesToFind Bearer types to find
@param aExpectedPos Position they should be in
@param aBearerTypesValues Used to determine what values to expect
*/
void CTestImapTransportBufferSizes::CheckEntryByBearerTypeL(TInt aBearerTypesToFind, TInt aExpectedPos, TInt aBearerTypesValues)
	{
	SetValues(aBearerTypesValues);
	
	TInt fetchRequestSize;
	TInt maxOutstandingFetchResponses;

	ASSERT_EQUALS(aExpectedPos, iSizes->GetSizesL(aBearerTypesToFind, fetchRequestSize, maxOutstandingFetchResponses));
	ASSERT_EQUALS(fetchRequestSize, iFetchRequestSize);
	ASSERT_EQUALS(maxOutstandingFetchResponses, iMaxOutstandingFetchResponses);
	}


/**
@SYMTestCaseID MSG-IMAP-BufferSizes-0001
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6983
@SYMTestCaseDesc Check that we can get and set the per Bearer settings
                 from CEmailAccounts
*/
void CTestImapTransportBufferSizes::TestGetAndSetBearerSettingsL()
	{
	INFO_PRINTF1(_L("TestGetAndSetBearerSettingsL - Start"));

	// Save empty sizes list
	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	// Load and check empty sizes list
	LoadAndCheckSizesL(0, KBearerTypes000001);

	SetValues(1);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	// Save sizes list
	TRAP(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	// Load and check sizes list
	LoadAndCheckSizesL(1, KBearerTypes000001);

	INFO_PRINTF1(_L("TestGetAndSetBearerSettingsL - End"));	
	}

/**
@SYMTestCaseID MSG-IMAP-BufferSizes-0002
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6983
@SYMTestCaseDesc Check that we can add options and bearer types to
                 CImapTransportBufferSizes using AddSizesL with various
                 parameters, and get expected actions, and error codes
*/
void CTestImapTransportBufferSizes::TestAddingRulesL()
	{
	INFO_PRINTF1(_L("TestAddingRulesL - Start"));

	TInt err = KErrNone;

	// Save empty sizes list
	TRAP(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	// Load and check empty sizes list
	LoadAndCheckSizesL(0, KBearerTypes000001);

	SetValues(KBearerTypes000001);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(0, KBearerTypes000001);

	// Not tested on debug builds because it would panic
#ifndef _DEBUG
	SetValues(KBearerTypes000010);

	iFetchRequestSize = 1023;
	TRAP(err, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

	iFetchRequestSize = 0;
	TRAP(err, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	SetValues(KBearerTypes000010);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(0, KBearerTypes000010);
	CheckEntryByPosL(1, KBearerTypes000001);

	SetValues(KBearerTypes001100);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(0, KBearerTypes001100);
	CheckEntryByPosL(1, KBearerTypes000010);
	CheckEntryByPosL(2, KBearerTypes000001);

	SetValues(KBearerTypes000010);
	ASSERT_EQUALS(KErrAlreadyExists, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	SetValues(KBearerTypes000100);
	ASSERT_EQUALS(KErrAlreadyExists, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	CheckEntryByPosL(0, KBearerTypes001100);
	CheckEntryByPosL(1, KBearerTypes000010);
	CheckEntryByPosL(2, KBearerTypes000001);

	// Save sizes list
	TRAP(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	TRAP(err, iEmailAccounts->LoadImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	CheckEntryByPosL(0, KBearerTypes001100);
	CheckEntryByPosL(1, KBearerTypes000010);
	CheckEntryByPosL(2, KBearerTypes000001);

	// Save empty rules list
	delete iSizes;
	iSizes = NULL;
	iSizes = CImapTransportBufferSizes::NewL();
	TRAP(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	// Load and check empty sizes list
	LoadAndCheckSizesL(0, KBearerTypes000001);

	SetValues(KBearerTypes000001);
	iBearerTypes = 1;
	TInt count = 1;
	for (; count <= 8; ++count)
		{
		ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, count));
		iBearerTypes <<= 1;
		}

	// Not tested on debug builds because it would panic
#ifndef _DEBUG
	TRAP(err, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, count));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	delete iSizes;
	iSizes = NULL;
	iSizes = CImapTransportBufferSizes::NewL();

	// Not tested on debug builds because it would panic
#ifndef _DEBUG
	TRAP(err, iSizes->AddSizesL(KMaxImapTransportBufferSizes, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestAddingRulesL - End"));	
	}

/**
@SYMTestCaseID MSG-IMAP-BufferSizes-0003
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6983
@SYMTestCaseDesc Check that GetSizesL using the Pos works correctly Category.
*/
void CTestImapTransportBufferSizes::TestGettingRulesByPosL()
	{
	INFO_PRINTF1(_L("TestGettingRulesByPosL - Start"));

	SetValues(KBearerTypes000001);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(0, KBearerTypes000001);

	SetValues(KBearerTypes000010);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(0, KBearerTypes000010);
	CheckEntryByPosL(1, KBearerTypes000001);

	SetValues(KBearerTypes001100);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(0, KBearerTypes001100);
	CheckEntryByPosL(1, KBearerTypes000010);
	CheckEntryByPosL(2, KBearerTypes000001);

	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	delete iSizes;
	iSizes = NULL;
	iSizes = CImapTransportBufferSizes::NewL();

	TRAP(err, iEmailAccounts->LoadImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	CheckEntryByPosL(0, KBearerTypes001100);
	CheckEntryByPosL(1, KBearerTypes000010);
	CheckEntryByPosL(2, KBearerTypes000001);

	// Not tested on debug builds because it would panic
#ifndef _DEBUG
	TRAP(err, iSizes->GetSizesL(-1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	ASSERT_FALSE(iSizes->GetSizesL(4, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	INFO_PRINTF1(_L("TestGettingRulesByPosL - End"));
	}

/**
@SYMTestCaseID MSG-IMAP-BufferSizes-0004
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6983
@SYMTestCaseDesc Check that GetSizesL using BearerTypes "masks" works correctly
*/
void CTestImapTransportBufferSizes::TestGettingRulesByBearerTypeL()
	{
	INFO_PRINTF1(_L("TestGettingRulesByBearerTypeL - Start"));

	SetValues(KBearerTypes000001);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByBearerTypeL(KBearerTypes000001, 0, KBearerTypes000001);

	SetValues(KBearerTypes000010);
	ASSERT_EQUALS(KErrNotFound, iSizes->GetSizesL(iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	SetValues(KBearerTypes000010);
	ASSERT_EQUALS(1, iSizes->AddSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	SetValues(KBearerTypes001100);
	ASSERT_EQUALS(2, iSizes->AddSizesL(2, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	delete iSizes;
	iSizes = NULL;
	iSizes = CImapTransportBufferSizes::NewL();

	TRAP(err, iEmailAccounts->LoadImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	CheckEntryByBearerTypeL(KBearerTypes000001, 0, KBearerTypes000001);
	CheckEntryByBearerTypeL(KBearerTypes000010, 1, KBearerTypes000010);
	CheckEntryByBearerTypeL(KBearerTypes000100, 2, KBearerTypes001100);
	CheckEntryByBearerTypeL(KBearerTypes001000, 2, KBearerTypes001100);

	INFO_PRINTF1(_L("TestGettingRulesByBearerTypeL - End"));
	}

/**
@SYMTestCaseID MSG-IMAP-BufferSizes-0005
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6983
@SYMTestCaseDesc Check that DeleteSizesL works for CImapTransportBufferSizes.
*/
void CTestImapTransportBufferSizes::TestDeleteByPosL()
	{
	INFO_PRINTF1(_L("TestDeleteByPosL - Start"));

	SetValues(KBearerTypes000001);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(0, KBearerTypes000001);

	SetValues(KBearerTypes000010);
	ASSERT_EQUALS(1, iSizes->AddSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(1, KBearerTypes000010);

	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	delete iSizes;
	iSizes = NULL;
	iSizes = CImapTransportBufferSizes::NewL();

	TRAP(err, iEmailAccounts->LoadImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	CheckEntryByPosL(0, KBearerTypes000001);
	CheckEntryByPosL(1, KBearerTypes000010);

	ASSERT_TRUE(iSizes->DeleteSizesL(1));
	CheckEntryByPosL(0, KBearerTypes000001);
	ASSERT_FALSE(iSizes->GetSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	ASSERT_FALSE(iSizes->DeleteSizesL(1));

	ASSERT_TRUE(iSizes->DeleteSizesL(0));
	ASSERT_FALSE(iSizes->GetSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	INFO_PRINTF1(_L("TestDeleteByPosL - Start"));
	}

/**
@SYMTestCaseID MSG-IMAP-BufferSizes-0006
@SYMTestType CIT
@SYMPREQ PREQ1307
@SYMREQ REQ6983
@SYMTestCaseDesc Check that DeleteBearerTypesL works for
                 CImapTransportBufferSizes
*/
void CTestImapTransportBufferSizes::TestDeleteByBearerTypeL()
	{
	INFO_PRINTF1(_L("TestDeleteByBearerTypeL - Start"));

	SetValues(KBearerTypes000001);
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(0, KBearerTypes000001);

	SetValues(KBearerTypes000010);
	ASSERT_EQUALS(1, iSizes->AddSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(1, KBearerTypes000010);

	SetValues(KBearerTypes001100);
	ASSERT_EQUALS(2, iSizes->AddSizesL(2, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CheckEntryByPosL(2, KBearerTypes001100);

	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	delete iSizes;
	iSizes = NULL;
	iSizes = CImapTransportBufferSizes::NewL();

	TRAP(err, iEmailAccounts->LoadImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	CheckEntryByPosL(0, KBearerTypes000001);
	CheckEntryByPosL(1, KBearerTypes000010);
	CheckEntryByPosL(2, KBearerTypes001100);

	TBool sizesRemoved;
	ASSERT_EQUALS(0, iSizes->DeleteBearerTypesL(KBearerTypes000001, sizesRemoved));
	ASSERT_TRUE(sizesRemoved);

	CheckEntryByPosL(0, KBearerTypes000010);
	CheckEntryByPosL(1, KBearerTypes001100);

	ASSERT_EQUALS(-1, iSizes->DeleteBearerTypesL(KBearerTypes000001, sizesRemoved));

	ASSERT_EQUALS(1, iSizes->DeleteBearerTypesL(KBearerTypes000100, sizesRemoved));
	ASSERT_FALSE(sizesRemoved);

	CheckEntryByPosL(0, KBearerTypes000010);
	CheckEntryByBearerTypeL(KBearerTypes001000, 1, KBearerTypes001100);

	INFO_PRINTF1(_L("TestDeleteByBearerTypeL - Start"));
	}

/**
Test create empty list
*/
void CTestImapTransportBufferSizes::TestCreateEmptyL()
	{
	INFO_PRINTF1(_L("TestCreateEmptyL - Start"));

	ASSERT_FALSE(iSizes->GetSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	CImapTransportBufferSizes* entries = CImapTransportBufferSizes::NewLC();
	ASSERT_FALSE(entries->GetSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CleanupStack::PopAndDestroy(entries);

	INFO_PRINTF1(_L("TestCreateEmptyL - Complete"));
	}

/**
Test adding new entries
*/
void CTestImapTransportBufferSizes::TestAddEntriesL()
	{
	INFO_PRINTF1(_L("TestAddEntriesL - Start"));

	iBearerTypes = KCommDbBearerWLAN;
	iFetchRequestSize = 4096;
	iMaxOutstandingFetchResponses = 2;
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerWcdma;
	iFetchRequestSize = 8192;
	iMaxOutstandingFetchResponses = 3;
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerLAN;
	iFetchRequestSize = 20480;
	iMaxOutstandingFetchResponses = 8;
	ASSERT_EQUALS(2, iSizes->AddSizesL(31, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	ASSERT_TRUE(iSizes->GetSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerWcdma));
	ASSERT_EQUALS(iFetchRequestSize, 8192);
	ASSERT_EQUALS(iMaxOutstandingFetchResponses, 3);

	ASSERT_TRUE(iSizes->GetSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerWLAN));
	ASSERT_EQUALS(iFetchRequestSize, 4096);
	ASSERT_EQUALS(iMaxOutstandingFetchResponses, 2);

	ASSERT_TRUE(iSizes->GetSizesL(2, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerLAN));
	ASSERT_EQUALS(iFetchRequestSize, 20480);
	ASSERT_EQUALS(iMaxOutstandingFetchResponses, 8);

	INFO_PRINTF1(_L("TestAddEntriesL - Complete"));
	}

/**
Test passing invalid parameters to add sizes routine
*/
void CTestImapTransportBufferSizes::TestAddEntryInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestAddEntryInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	TInt err = KErrNone;
	TRAP(err, iSizes->AddSizesL(-1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

	err = KErrNone;
	TRAP(err, iSizes->AddSizesL(KMaxImapTransportBufferSizes, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

	iBearerTypes = 0;
	err = KErrNone;
	TRAP(err, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

	SetValues(1);
	iFetchRequestSize = 0;
	TRAP(err, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

	SetValues(1);
	iFetchRequestSize = 1025;
	TRAP(err, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

	SetValues(1);
	iMaxOutstandingFetchResponses = 0;
	TRAP(err, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

	SetValues(1);
	iMaxOutstandingFetchResponses = 9;
	TRAP(err, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

#endif //_DEBUG

	INFO_PRINTF1(_L("TestAddEntryInvalidArgumentL - Complete"));
	}

/**
Test trying to add an entry with a bearer type that already exists
*/
void CTestImapTransportBufferSizes::TestAddEntryBearerTypeAlreadyExistsL()
	{
	INFO_PRINTF1(_L("TestAddEntryBearerTypeAlreadyExistsL - Start"));

	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(KErrAlreadyExists, iSizes->AddSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	INFO_PRINTF1(_L("TestAddEntryBearerTypeAlreadyExistsL - Complete"));
	}

/**
Test trying to add too many entries
*/
void CTestImapTransportBufferSizes::TestAddEntryOverflowL()
	{
	INFO_PRINTF1(_L("TestAddEntryOverflowL - Start"));

	iBearerTypes = 1;
	for (TInt entry = 0; entry < KMaxImapTransportBufferSizes; ++entry)
		{
		ASSERT_EQUALS(entry, iSizes->AddSizesL(entry, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
		iBearerTypes <<= 1;
		}

	iBearerTypes = 1;	
	ASSERT_EQUALS(KErrOverflow, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	INFO_PRINTF1(_L("TestAddEntryOverflowL - Complete"));
	}

/**
Test entry deletion
*/
void CTestImapTransportBufferSizes::TestDeleteEntriesL()
	{
	INFO_PRINTF1(_L("TestDeleteEntriesL - Start"));

	iBearerTypes = KCommDbBearerPAN;
	iFetchRequestSize = 4096;
	iMaxOutstandingFetchResponses = 2;
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerWcdma;
	iFetchRequestSize = 8192;
	iMaxOutstandingFetchResponses = 3;
	ASSERT_EQUALS(1, iSizes->AddSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerLAN;
	iFetchRequestSize = 20480;
	iMaxOutstandingFetchResponses = 3;
	ASSERT_EQUALS(2, iSizes->AddSizesL(2, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerWLAN;
	iFetchRequestSize = 40960;
	iMaxOutstandingFetchResponses = 8;
	ASSERT_EQUALS(3, iSizes->AddSizesL(3, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	ASSERT_TRUE(iSizes->DeleteSizesL(2));
	ASSERT_TRUE(iSizes->DeleteSizesL(0));

	ASSERT_TRUE(iSizes->GetSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerWcdma));
	ASSERT_EQUALS(iFetchRequestSize, 8192);
	ASSERT_EQUALS(iMaxOutstandingFetchResponses, 3);

	ASSERT_TRUE(iSizes->GetSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(iBearerTypes, static_cast<TUint32>(KCommDbBearerWLAN));
	ASSERT_EQUALS(iFetchRequestSize, 40960);
	ASSERT_EQUALS(iMaxOutstandingFetchResponses, 8);

	ASSERT_FALSE(iSizes->GetSizesL(2, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_FALSE(iSizes->GetSizesL(3, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	INFO_PRINTF1(_L("TestDeleteEntriesL - Complete"));
	}

/**
Test passing invalid parameters to delete routine
*/
void CTestImapTransportBufferSizes::TestDeleteEntryInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestDeleteEntryInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	TInt err = KErrNone;
	TRAP(err, iSizes->DeleteSizesL(-1));
	ASSERT_EQUALS(err, KErrArgument);

	err = KErrNone;
	TRAP(err, iSizes->DeleteSizesL(KMaxImapTransportBufferSizes));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestDeleteEntryInvalidArgumentL - Complete"));
	}

/**
Test deleting entries by bearer type
*/
void CTestImapTransportBufferSizes::TestDeleteBearerTypesL()
	{
	INFO_PRINTF1(_L("TestDeleteBearerTypesL - Start"));

	iBearerTypes = KCommDbBearerPAN | KCommDbBearerWcdma;
	iFetchRequestSize = 4096;
	iMaxOutstandingFetchResponses = 2;
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerLAN;
	iFetchRequestSize = 8192;
	iMaxOutstandingFetchResponses = 3;
	ASSERT_EQUALS(1, iSizes->AddSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerWLAN;
	iFetchRequestSize = 20480;
	iMaxOutstandingFetchResponses = 8;
	ASSERT_EQUALS(2, iSizes->AddSizesL(2, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	TBool sizesRemoved;

	ASSERT_EQUALS(0, iSizes->DeleteBearerTypesL(KCommDbBearerPAN, sizesRemoved));
	ASSERT_FALSE(sizesRemoved);
	ASSERT_EQUALS(1, iSizes->DeleteBearerTypesL(KCommDbBearerLAN, sizesRemoved));
	ASSERT_TRUE(sizesRemoved);
	ASSERT_FALSE(iSizes->GetSizesL(2, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(KErrNotFound, iSizes->DeleteBearerTypesL(KCommDbBearerWcdma | KCommDbBearerWLAN, sizesRemoved));

	INFO_PRINTF1(_L("TestDeleteBearerTypesL - Complete"));
	}

/**
Test passing invalid parameters to delete bearer types routine
*/
void CTestImapTransportBufferSizes::TestDeleteBearerTypesInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestDeleteBearerTypesInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	iBearerTypes = 0;
	TInt err = KErrNone;
	TBool sizesRemoved;
	TRAP(err, iSizes->DeleteBearerTypesL(iBearerTypes, sizesRemoved));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestDeleteBearerTypesInvalidArgumentL - Complete"));
	}

/**
Test passing invalid parameters to get entry routine
*/
void CTestImapTransportBufferSizes::TestGetEntryByPosInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestGetEntryByPosInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	TInt err = KErrNone;
	TRAP(err, iSizes->GetSizesL(-1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);

	err = KErrNone;
	TRAP(err, iSizes->GetSizesL(KMaxImapTransportBufferSizes, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestGetEntryByPosInvalidArgumentL - Complete"));
	}

/**
Test getting entries for bearer types
*/
void CTestImapTransportBufferSizes::TestGetEntryByBearerL()
	{
	INFO_PRINTF1(_L("TestGetEntryByBearerL - Start"));

	iBearerTypes = KCommDbBearerPAN | KCommDbBearerWcdma;
	iFetchRequestSize = 4096;
	iMaxOutstandingFetchResponses = 2;
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerLAN;
	iFetchRequestSize = 8192;
	iMaxOutstandingFetchResponses = 3;
	ASSERT_EQUALS(1, iSizes->AddSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerWLAN;
	iFetchRequestSize = 20480;
	iMaxOutstandingFetchResponses = 8;
	ASSERT_EQUALS(2, iSizes->AddSizesL(2, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerPAN;
	ASSERT_EQUALS(0, iSizes->GetSizesL(iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerPAN | KCommDbBearerWcdma;
	ASSERT_EQUALS(0, iSizes->GetSizesL(iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerPAN | KCommDbBearerWLAN;
	ASSERT_EQUALS(KErrNotFound, iSizes->GetSizesL(iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iBearerTypes = KCommDbBearerWLAN;
	ASSERT_EQUALS(2, iSizes->GetSizesL(iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	INFO_PRINTF1(_L("TestGetEntryByBearerL - Complete"));	
	}

/**
Test passing invalid parameters to get entry routine
*/
void CTestImapTransportBufferSizes::TestGetEntryByBearerInvalidArgumentL()
	{
	INFO_PRINTF1(_L("TestGetEntryByBearerInvalidArgumentL - Start"));

#ifdef _DEBUG
	INFO_PRINTF1(_L("Not tested on debug builds as it would panic"));
#else //_DEBUG
	iBearerTypes = 0;
	TInt err = KErrNone;
	TRAP(err, iSizes->GetSizesL(iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(err, KErrArgument);
#endif //_DEBUG

	INFO_PRINTF1(_L("TestGetEntryByBearerInvalidArgumentL - Complete"));
	}

/**
Test class reset
*/
void CTestImapTransportBufferSizes::TestResetL()
	{
	INFO_PRINTF1(_L("TestResetL - Start"));

	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	iSizes->Reset();
	ASSERT_FALSE(iSizes->GetSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	INFO_PRINTF1(_L("TestResetL - Complete"));
	}

/**
Test class copy
*/
void CTestImapTransportBufferSizes::TestCopyL()
	{
	INFO_PRINTF1(_L("TestCopyL - Start"));

	iFetchRequestSize = 4096;
	iMaxOutstandingFetchResponses = 2;
	ASSERT_EQUALS(0, iSizes->AddSizesL(0, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	CImapTransportBufferSizes* newSizes = iSizes->CopyL();
	CleanupStack::PushL(newSizes);

	TUint32 bearerTypes;
	ASSERT_TRUE(newSizes->GetSizesL(0, bearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	ASSERT_EQUALS(bearerTypes, iBearerTypes);
	ASSERT_EQUALS(iFetchRequestSize, 4096);
	ASSERT_EQUALS(iMaxOutstandingFetchResponses, 2);

	ASSERT_FALSE(newSizes->GetSizesL(1, iBearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));

	CleanupStack::PopAndDestroy(newSizes);

	newSizes = iSizes->CopyLC();
	ASSERT_TRUE(newSizes->GetSizesL(0, bearerTypes, iFetchRequestSize, iMaxOutstandingFetchResponses));
	CleanupStack::PopAndDestroy(newSizes);

	INFO_PRINTF1(_L("TestCopyL - Complete"));
	}


/**
Test deleting an empty set of transport buffer sizes
*/
void CTestImapTransportBufferSizes::TestDeleteEmptyL()
	{
	INFO_PRINTF1(_L("TestDeleteEmptyL - Start"));

	TRAPD(err, iEmailAccounts->DeleteImapTransportBufferSizesL());
	ASSERT_EQUALS(err, KErrNone);

	INFO_PRINTF1(_L("TestDeleteEmptyL - Complete"));
	}

/**
Test saving a full set of transport buffer sizes
*/
void CTestImapTransportBufferSizes::TestSaveFullL()
	{
	INFO_PRINTF1(_L("TestCreateFullL - Start"));

	SetAllL(1);

	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));

	ASSERT_EQUALS(err, KErrNone);

	LoadAndCheckSizesL(KMaxImapTransportBufferSizes, 1);

	INFO_PRINTF1(_L("TestCreateFullL - Complete"));
	}

/**
Change the full set of transport buffer sizes for another full set
*/
void CTestImapTransportBufferSizes::TestChangeFullToFullL()
	{
	INFO_PRINTF1(_L("TestChangeFullToFullL - Start"));

	SetAllL(2);

	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	LoadAndCheckSizesL(KMaxImapTransportBufferSizes, 2);

	INFO_PRINTF1(_L("TestChangeFullToFullL - Complete"));
	}

/**
Change the full set of transport buffer sizes for an empty set
*/
void CTestImapTransportBufferSizes::TestChangeFullToEmptyL()
	{
	INFO_PRINTF1(_L("TestChangeFullToEmptyL - Start"));

	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	LoadAndCheckSizesL(0, 1);

	INFO_PRINTF1(_L("TestChangeFullToEmptyL - Complete"));
	}

/**
Change the empty set of transport buffer sizes for a full set
*/
void CTestImapTransportBufferSizes::TestChangeEmptyToFullL()
	{
	INFO_PRINTF1(_L("TestChangeEmptyToFullL - Start"));

	SetAllL(1);

	TRAPD(err, iEmailAccounts->SaveImapTransportBufferSizesL(*iSizes));
	ASSERT_EQUALS(err, KErrNone);

	LoadAndCheckSizesL(KMaxImapTransportBufferSizes, 1);

	INFO_PRINTF1(_L("TestChangeEmptyToFullL - Complete"));
	}

/**
Delete a full set of transport buffer sizes
*/
void CTestImapTransportBufferSizes::TestDeleteFullL()
	{
	INFO_PRINTF1(_L("TestDeleteFullL - Start"));

	TRAPD(err, iEmailAccounts->DeleteImapTransportBufferSizesL());
	ASSERT_EQUALS(err, KErrNone);

	INFO_PRINTF1(_L("TestDeleteFullL - Complete"));
	}

CTestSuite* CTestImapTransportBufferSizes::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;

	// Tests defined in the test spec
	ADD_ASYNC_TEST_STEP(TestGetAndSetBearerSettingsL);
	ADD_ASYNC_TEST_STEP(TestAddingRulesL);
	ADD_ASYNC_TEST_STEP(TestGettingRulesByPosL);
	ADD_ASYNC_TEST_STEP(TestGettingRulesByBearerTypeL);
	ADD_ASYNC_TEST_STEP(TestDeleteByPosL);
	ADD_ASYNC_TEST_STEP(TestDeleteByBearerTypeL);

	// Extra transport buffer sizes tests	
	ADD_TEST_STEP(TestCreateEmptyL);
	ADD_TEST_STEP(TestAddEntriesL);
	ADD_TEST_STEP(TestAddEntryInvalidArgumentL);
	ADD_TEST_STEP(TestAddEntryBearerTypeAlreadyExistsL);
	ADD_TEST_STEP(TestAddEntryOverflowL);
	ADD_TEST_STEP(TestDeleteEntriesL);
	ADD_TEST_STEP(TestDeleteEntryInvalidArgumentL);
	ADD_TEST_STEP(TestDeleteBearerTypesL);
	ADD_TEST_STEP(TestDeleteBearerTypesInvalidArgumentL);
	ADD_TEST_STEP(TestGetEntryByPosInvalidArgumentL);
	ADD_TEST_STEP(TestGetEntryByBearerL);	
	ADD_TEST_STEP(TestGetEntryByBearerInvalidArgumentL);
	ADD_TEST_STEP(TestResetL);
	ADD_TEST_STEP(TestCopyL);

	// Extra email account tests
	ADD_ASYNC_TEST_STEP(TestDeleteEmptyL);
	ADD_ASYNC_TEST_STEP(TestSaveFullL);
	ADD_ASYNC_TEST_STEP(TestChangeFullToFullL);
	ADD_ASYNC_TEST_STEP(TestChangeFullToEmptyL);
	ADD_ASYNC_TEST_STEP(TestChangeEmptyToFullL);
	ADD_ASYNC_TEST_STEP(TestDeleteFullL);

	END_SUITE;
	}
