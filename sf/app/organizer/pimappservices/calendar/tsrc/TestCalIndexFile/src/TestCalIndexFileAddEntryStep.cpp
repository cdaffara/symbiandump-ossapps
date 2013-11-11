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

#include "TestCalIndexFileAddEntryStep.h"
#include "TestCalIndexFileDefs.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <s32file.h>


CTestCalIndexFileAddEntryStep::~CTestCalIndexFileAddEntryStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFileAddEntryStep::CTestCalIndexFileAddEntryStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTestCalIndexFileAddEntryStep);
	}


TVerdict CTestCalIndexFileAddEntryStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFileAddEntryStep::doTestStepL()
	{
	// this method assumes a pass. Checks that fail will stop
	// the test step. If this method Leaves, then the TEF will 
	// catch it and the step will not be recorded as a pass.
	
	SetUpCalDirL();
	
	AddCalendarFileL();
	AddIndexFileL();
	OpenAgendaL();
	
	// use the base class version of ValidateDbContentsL as 
	// we have not added the entry
	if (!CTestCalIndexFileStepBase::ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Add Entry Step Failed validating DB contents before entry addition"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}
		
	AddEntryL();
	// The index file should be deleted as it is now out of date.
	// It will be rebuilt when the session closes.
	if (CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Add Entry Step Failed - index file present when dirty"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}
		
	CloseAgendaL();
	if (!CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Add Entry Step Failed - no index file created after add and close"));
		SetTestStepResult(EFail);
		return EFail;
		}
		
	// make sure the added entry is still there
	OpenAgendaL();
	if (!ValidateDbContentsL(ETrue))
		{
		INFO_PRINTF1(_L("Add Entry Step Failed validating DB contents after addition"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}	
	CloseAgendaL();

	SetTestStepResult(EPass);
	return EPass;
	}


void CTestCalIndexFileAddEntryStep::GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo, TInt& aNumEntries, TBool aPerformCrudOps)
	{
	// first get the default entries from the file
	CTestCalIndexFileStepBase::GetEntryInfoFromConfigL(aEntriesInfo, aNumEntries, aPerformCrudOps);
	
	if (aPerformCrudOps)
		{
		// now add the new entry info to the list
		TPtrC entryString;
		TBool readRes = EFalse;
		readRes = GetStringFromConfig(ConfigSection(), KAddEntry, entryString);
		if (!readRes)
			{
			INFO_PRINTF1(_L("Error in CTestCalIndexFileAddEntryStep::GetEntryInfoFromConfigL - entrytoadd not found in config file"));
			User::Leave(KErrNotFound);
			}
		CConfigTestEntryInfo* inf = new(ELeave)CConfigTestEntryInfo();
		CleanupStack::PushL(inf);
		ParseEntryStringL(*inf, entryString);
		aEntriesInfo.Append(inf);
		CleanupStack::Pop(inf); // now held in array
		aNumEntries++;
		}
	}


