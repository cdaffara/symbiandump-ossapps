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

#include "TestCalIndexFileDeleteDbStep.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <s32file.h>
#include "caltestlib.h"


CTestCalIndexFileDeleteDbStep::~CTestCalIndexFileDeleteDbStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFileDeleteDbStep::CTestCalIndexFileDeleteDbStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalIndexFileDeleteDbStep);
	}

TVerdict CTestCalIndexFileDeleteDbStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFileDeleteDbStep::doTestStepL()
	{
	// this method assumes a pass. Checks that fail will stop
	// the test step
	
	SetUpCalDirL();
	
	AddCalendarFileL();
	AddIndexFileL();

	DeleteDbL();
	
	if (CheckCalendarFilePresentL() || CheckIndexFilePresentL())
		{
		SetTestStepResult(EFail);
		return EFail;
		}

	return EPass;
	}


void CTestCalIndexFileDeleteDbStep::DeleteDbL()
	{
	TFileName calName;
	MakeCalendarFileName(calName);	
	iSession = CCalSession::NewL();

	iSession->DeleteCalFileL(calName);
	
	delete iSession;
	iSession = NULL;
	}
	
