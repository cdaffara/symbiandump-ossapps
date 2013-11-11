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

#include "TestCalIndexFilePerfStep.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <s32file.h>


CTestCalIndexFilePerfStep::~CTestCalIndexFilePerfStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFilePerfStep::CTestCalIndexFilePerfStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalIndexFilePerfStep);
	}

TVerdict CTestCalIndexFilePerfStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFilePerfStep::doTestStepL()
	{
	// this method assumes a pass. Checks that fail will stop
	// the test step
	SetUpCalDirL();
	
	// check if we can open the agenda and have the index file
	// created
	AddCalendarFileL();
	OpenAgendaL();
	if (!ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Open Step Failed validating DB contents without idx file"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}	
	CloseAgendaL();
	if (!CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Open Step Failed - no index file created"));
		SetTestStepResult(EFail);
		return EFail;
		}
		
	// check if we can open with the index file present
	OpenAgendaL();
	if (!ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Open Step Failed validating DB contents with idx file"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}	
	CloseAgendaL();
	
	SetTestStepResult(EPass);
	return EPass;
	}
