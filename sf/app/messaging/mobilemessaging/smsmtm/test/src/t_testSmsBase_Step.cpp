// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <test/testexecutelog.h>
#include "t_testSmsBase_Step.h"

GLDEF_D RTest				test(_L("SMCM Test Harness"));

CTestSmsBaseStep::CTestSmsBaseStep()
	{
	}
		
CTestSmsBaseStep::~CTestSmsBaseStep()
	{
	delete iSmcmTest;	
	delete iSmsTestUtils;
	delete iScheduler;
	}

TVerdict CTestSmsBaseStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	
	// Create and install the active scheduler
 	iScheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(iScheduler);
	
	iSmsTestUtils = CSmsTestUtils::NewL(test);
	TInt nextTest=0;
	iSmcmTest = CSmutTest::NewLC(*iSmsTestUtils, KNullDesC, nextTest,*this);
	CleanupStack::Pop(iSmcmTest);
	
	return TestStepResult();
	}		

TVerdict CTestSmsBaseStep::doTestStepL()
	{			
	SetTestStepResult(EPass);
	return TestStepResult();
	}	

TVerdict CTestSmsBaseStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
	
	


