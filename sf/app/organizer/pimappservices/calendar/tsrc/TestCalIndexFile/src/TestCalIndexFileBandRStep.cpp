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

#include "TestCalIndexFileBandRStep.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <s32file.h>


CTestCalIndexFileBandRStep::~CTestCalIndexFileBandRStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFileBandRStep::CTestCalIndexFileBandRStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalIndexFileBandRStep);
	}

TVerdict CTestCalIndexFileBandRStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFileBandRStep::doTestStepL()
	{
	// this method assumes a pass. Checks that fail will stop
	// the test step
	
	SetUpCalDirL();
	
	AddCalendarFileL();
	AddIndexFileL();
	
	BackupL();
	RestoreL();
	
	// After a backup and restore the calendar file should be 
	// present but the index file should be gone.
	if (!CheckCalendarFilePresentL())
		{
		SetTestStepResult(EFail);
		return EFail;
		}
		
	if (CheckIndexFilePresentL())
		{
		SetTestStepResult(EFail);
		return EFail;
		}

	return EPass;
	}

void CTestCalIndexFileBandRStep::BackupL()
	{
	}
	
void CTestCalIndexFileBandRStep::RestoreL()
	{
	}
	

