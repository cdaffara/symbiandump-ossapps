// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "TestCalIndexFileOpenwithIndexStep.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <s32file.h>


CTestCalIndexFileOpenwithIndexStep::~CTestCalIndexFileOpenwithIndexStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFileOpenwithIndexStep::CTestCalIndexFileOpenwithIndexStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalIndexFileOpenwithIndexStep);
	}

TVerdict CTestCalIndexFileOpenwithIndexStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFileOpenwithIndexStep::doTestStepL()
	{
	// this method assumes a pass. Checks that fail will stop
	// the test step
	
	SetUpCalDirL();
	
	AddCalendarFileL();
	AddIndexFileL();

	// insure that we can open with the given index file
	OpenAgendaL();
	if (!ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Open with Index Step Failed validating DB contents without idx file"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}	
	CloseAgendaL();

	SetTestStepResult(EPass);
	return EPass;
	}
