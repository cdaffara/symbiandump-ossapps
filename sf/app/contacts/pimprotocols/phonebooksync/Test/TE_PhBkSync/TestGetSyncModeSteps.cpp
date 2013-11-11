// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains PhbkSync API GetSyncMode() related Unit Test Steps
// 
//


#include "TE_PhBkSyncBase.h"
#include "TestGetSyncModeSteps.h"

/** 
 * Test getting the synchronisation mode and print this information
 * to screen. Repeat this for all supported phonebooks.
 */
//
//    Test step1 -- Test#80 in Unit Test Spec. Doc. //
//
CPhbkSyncGetSyncModeTest_01::CPhbkSyncGetSyncModeTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncGetSyncModeTest_01"));
	}

CPhbkSyncGetSyncModeTest_01::~CPhbkSyncGetSyncModeTest_01()
	{
	}

TVerdict CPhbkSyncGetSyncModeTest_01::doTestStepL( )
	{
	TInt result;
	RPhoneBookSession::TPhonebookSyncMode syncMode;
	for (int i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		result = iSession.GetSyncMode(syncMode, iPhoneBook[i]);
		TESTCHECKL(result, KErrNone);
		TESTCHECK(syncMode, RPhoneBookSession::EAutoCurrentIcc);
		}

	return TestStepResult();
	}
