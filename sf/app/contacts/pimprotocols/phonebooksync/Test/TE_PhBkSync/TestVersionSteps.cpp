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
// This contains PhbkSync API Version() related Unit Test Steps
// 
//

#include "TE_PhBkSyncBase.h"
#include "TestVersionSteps.h"

//
//    Test step1 -- Test#9 in Unit Test Spec. Doc. //
//
CPhbkSyncVersionTest_01::CPhbkSyncVersionTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncVersionTest_01"));
	}

CPhbkSyncVersionTest_01::~CPhbkSyncVersionTest_01()
	{
	}

/** 
 * Test getting the current version of the phonebook 
 * synchroniser serer and print the result to screen.
 */
TVerdict CPhbkSyncVersionTest_01::doTestStepL( )
	{
	TVersion version;
	version = iSession.Version();
	INFO_PRINTF4(_L("The version number of the phonebook synchroniser is %d.%d.%d"),
		version.iMajor, version.iMinor, version.iBuild);

	TESTCHECK(version.iMajor, 1);
	TESTCHECK(version.iMinor, 2);
	TESTCHECK(version.iBuild, 1);

	return TestStepResult();
	}
