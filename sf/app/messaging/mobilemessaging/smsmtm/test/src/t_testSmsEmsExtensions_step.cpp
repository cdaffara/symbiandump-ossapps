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
#include "t_testSmsEmsExtensions_step.h"

CTestSmsEmsExtensionsStep::CTestSmsEmsExtensionsStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KTestSmsEmsExtensions);
	}
		
CTestSmsEmsExtensionsStep::~CTestSmsEmsExtensionsStep()
	{
	}		

TVerdict CTestSmsEmsExtensionsStep::doTestStepL()
	{			
	SetTestStepResult(iSmcmTest->TestSmsEmsExtensionsL());
	return TestStepResult();
	}	

	
	


