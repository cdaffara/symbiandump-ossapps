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
#include "t_testSmsSettings_Step.h"

CTestSmsSettingsStep::CTestSmsSettingsStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestSmsSettings);
	}
		
CTestSmsSettingsStep::~CTestSmsSettingsStep()
	{
	}		

TVerdict CTestSmsSettingsStep::doTestStepL()
	{			
	SetTestStepResult(iSmcmTest->TestSettingsL());
	return TestStepResult();
	}	


	


