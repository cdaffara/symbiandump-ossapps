// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains PhbkSync API SetSyncMode() related Unit Test Steps
// 
//


#include "TE_PhBkSyncBase.h"
#include "TestSetSyncModeSteps.h"

/** 
 * Test setting the synchronisation mode to EAutoCurrentIcc.
 * Repeat this for all spported phonebooks.
 */
//
//    Test step1 -- Test#81 in Unit Test Spec. Doc. //
//
CPhbkSyncSetSyncModeTest_01::CPhbkSyncSetSyncModeTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncSetSyncModeTest_01"));
	}

CPhbkSyncSetSyncModeTest_01::~CPhbkSyncSetSyncModeTest_01()
	{
	}

TVerdict CPhbkSyncSetSyncModeTest_01::doTestStepL( )
	{
    TInt result;
	RPhoneBookSession::TPhonebookSyncMode syncMode; // sync mode
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		result = iSession.SetSyncMode(RPhoneBookSession::EAutoCurrentIcc,  iPhoneBook[i]);
		TESTCHECK(result, KErrNone);
		result = iSession.GetSyncMode(syncMode, iPhoneBook[i]);
		TESTCHECK(result, KErrNone);
		TESTCHECK(syncMode, RPhoneBookSession::EAutoCurrentIcc);
		}

	return TestStepResult();
	}

/** 
 * Test setting the synchronisation mode to EAutoSameIcc.
 * Repeat this for all spported phonebooks.
 */
//
//    Test step2 -- Test#82 in Unit Test Spec. Doc.  //
//
CPhbkSyncSetSyncModeTest_02::CPhbkSyncSetSyncModeTest_02()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncSetSyncModeTest_02"));
	}

CPhbkSyncSetSyncModeTest_02::~CPhbkSyncSetSyncModeTest_02()
	{
	}

TVerdict CPhbkSyncSetSyncModeTest_02::doTestStepL( )
	{
	TInt result;
	RPhoneBookSession::TPhonebookSyncMode syncMode; // sync mode
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		result = iSession.SetSyncMode(RPhoneBookSession::EAutoSameIcc, iPhoneBook[i]);
		TESTCHECK(result, KErrNone);
		result = iSession.GetSyncMode(syncMode, iPhoneBook[i]);
		TESTCHECK(result, KErrNone);
		TESTCHECK(syncMode, RPhoneBookSession::EAutoSameIcc);
		}

	return TestStepResult();
	}

/** 
 * Test setting the synchronisation mode to EManual.
 * Repeat this for all spported phonebooks.
 */
//
//    Test step3 -- Test#83 in Unit Test Spec. Doc.  //
//
CPhbkSyncSetSyncModeTest_03::CPhbkSyncSetSyncModeTest_03()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncSetSyncModeTest_03"));
	}

CPhbkSyncSetSyncModeTest_03::~CPhbkSyncSetSyncModeTest_03()
	{
	}

TVerdict CPhbkSyncSetSyncModeTest_03::doTestStepL( )
	{
	TInt result;
	RPhoneBookSession::TPhonebookSyncMode syncMode; // sync mode
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		result = iSession.SetSyncMode(RPhoneBookSession::EManual, iPhoneBook[i]);
		TESTCHECK(result, KErrNone);
		result = iSession.GetSyncMode(syncMode, iPhoneBook[i]);
		TESTCHECK(result, KErrNone);
		TESTCHECK(syncMode, RPhoneBookSession::EManual);
		}

	return TestStepResult();
	}
