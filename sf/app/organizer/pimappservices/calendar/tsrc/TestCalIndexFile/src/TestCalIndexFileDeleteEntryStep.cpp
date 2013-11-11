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

#include "TestCalIndexFileDeleteEntryStep.h"
#include "TestCalIndexFileDefs.h"
#include <fbs.h>
#include <e32math.h>
#include <calcategory.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calsession.h>
#include <s32file.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include "caltestlib.h"


CTestCalIndexFileDeleteEntryStep::~CTestCalIndexFileDeleteEntryStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFileDeleteEntryStep::CTestCalIndexFileDeleteEntryStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalIndexFileDeleteEntryStep);
	}


TVerdict CTestCalIndexFileDeleteEntryStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFileDeleteEntryStep::doTestStepL()
	{
	// this method assumes a pass. Checks that fail will stop
	// the test step
	
	SetUpCalDirL();
	
	AddCalendarFileL();
	AddIndexFileL();
	OpenAgendaL();
	
	// use the base class version of ValidateDbContentsL as 
	// we have not deleted the entry yet
	if (!CTestCalIndexFileStepBase::ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Delete Entry Step Failed validating DB contents before entry deletion"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}
		
	DeleteEntryL();
	// The index file should be deleted as it is now out of date.
	// It will be rebuilt when the session closes.
	if (CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Delete Entry Step Failed - index file present when dirty"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}
		
	CloseAgendaL();
	if (!CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Delete Entry Step Failed - no index file created after delete and close"));
		SetTestStepResult(EFail);
		return EFail;
		}
		
	// make sure the deleted entry is gone
	OpenAgendaL();
	if (!ValidateDbContentsL(ETrue))
		{
		INFO_PRINTF1(_L("Delete Entry Step Failed validating DB contents after deletion"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}	
	CloseAgendaL();

	SetTestStepResult(EPass);
	return EPass;
	}
	
void CTestCalIndexFileDeleteEntryStep::DeleteEntryL()
	{
	TPtrC entryString;
	TBool readRes = EFalse;
	readRes = GetStringFromConfig(ConfigSection(), KDeleteEntry, entryString);
	if (!readRes)
		{
		INFO_PRINTF1(_L("Error in CTestCalIndexFileDeleteEntryStep::DeleteEntryL - entrytodelete not found in config file"));
		User::Leave(KErrNotFound);
		}
	CConfigTestEntryInfo* inf = new(ELeave)CConfigTestEntryInfo();
	CleanupStack::PushL(inf);
	ParseEntryStringL(*inf, entryString);
				
	//  see if we can find a match in the database using an instance view
		
		CCalProgress* progress = new(ELeave)CCalProgress;
		CleanupStack::PushL(progress);  
	
		CCalInstanceView* view = CCalInstanceView::NewL(*iSession, *progress);
		CleanupStack::PushL(view);
		
		CActiveScheduler::Start();
		
		CalCommon::TCalViewFilter filter = CalCommon::EIncludeAppts;
		switch (inf->iType)
			{
			case (CCalEntry::EAppt):
				filter = CalCommon::EIncludeAppts;
				break;
			case (CCalEntry::ETodo):
				filter = CalCommon::EIncludeCompletedTodos | CalCommon::EIncludeIncompletedTodos;
				break;
			case (CCalEntry::EEvent):
				filter = CalCommon::EIncludeEvents;
				break;
			case (CCalEntry::EReminder):
				filter = CalCommon::EIncludeReminder;
				break;
			case (CCalEntry::EAnniv):
				filter = CalCommon::EIncludeAnnivs;
				break;
			default:
				User::Leave(KErrCorrupt);
				break;
			}
		RPointerArray<CCalInstance> instances;
		CleanupResetAndDestroyPushL(instances);
		// look for instances between (startdate -1 day) and
		// (enddate + 1 day)
		TTimeIntervalDays oneDay(1);
		TTime beginTTime = (inf->iStartTime) - oneDay;
		TTime endTTime = (inf->iEndTime) + oneDay;
		TCalTime begin;
		begin.SetTimeUtcL(beginTTime);
		TCalTime end;
		end.SetTimeUtcL(endTTime);
		CalCommon::TCalTimeRange timerange(begin, end);	
		
		view->FindInstanceL(instances, filter, timerange);
		
		TInt counter = 0;
		TInt max = instances.Count();
		INFO_PRINTF2(_L("%d instances to compare against"),max);
		TBool found = EFalse;
		while ((found == EFalse) && (counter < max))
			{
			INFO_PRINTF2(_L("compare delete info to entry %d"), (counter+1));
			if (inf->CompareToEntryL((instances[counter])->Entry()))
				{
				INFO_PRINTF2(_L("Entry info matched instance %d"), (counter+1));
				found = ETrue;
				view->DeleteL(instances[counter], CalCommon::EThisAndAll);
				instances.Remove(counter);
				}
			else
				{
				counter++;
				}
			}
		CleanupStack::PopAndDestroy(&instances);
		CleanupStack::PopAndDestroy(view);
		CleanupStack::PopAndDestroy(progress);
	
	CleanupStack::PopAndDestroy(inf);
	
	if (!found)
		{
		INFO_PRINTF1(_L("Could not find entrytodelete in database"));
		User::Leave(KErrNotFound);
		}

	}
	
void CTestCalIndexFileDeleteEntryStep::GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo, TInt& aNumEntries, TBool aPerformCrudOps)
	{
	// first get the default entries from the file
	CTestCalIndexFileStepBase::GetEntryInfoFromConfigL(aEntriesInfo, aNumEntries, aPerformCrudOps);
	
	if (aPerformCrudOps)
		{
		TInt entryToDelete;
		TBool readRes = EFalse;
		readRes = GetIntFromConfig(ConfigSection(), KNumDeleteEntry, entryToDelete);
		if (!readRes)
			{
			INFO_PRINTF1(_L("Error in CTestCalIndexFileDeleteEntryStep::GetEntryInfoFromConfigL - entrynumtodelete not found in config file"));
			User::Leave(KErrNotFound);
			}
			
		if (entryToDelete >= aNumEntries)
			{
			INFO_PRINTF1(_L("Error in CTestCalIndexFileDeleteEntryStep::GetEntryInfoFromConfigL - entrynumtodelete too big"));
			User::Leave(KErrTooBig);
			}

		iEntryNumToDelete = entryToDelete-1;
		delete aEntriesInfo[iEntryNumToDelete];
		aEntriesInfo.Remove(iEntryNumToDelete);
		aNumEntries--;
		}
	}
