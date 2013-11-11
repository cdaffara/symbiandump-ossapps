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

#include "TestCalIndexFileModifyEntryStep.h"
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
#include <calentry.h>
#include <calentryview.h>
#include "caltestlib.h"


CTestCalIndexFileModifyEntryStep::~CTestCalIndexFileModifyEntryStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFileModifyEntryStep::CTestCalIndexFileModifyEntryStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalIndexFileModifyEntryStep);
	}

TVerdict CTestCalIndexFileModifyEntryStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFileModifyEntryStep::doTestStepL()
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
		INFO_PRINTF1(_L("Modify Entry Step Failed validating DB contents before entry modification"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}
		
	ModifyEntryL();
	// The index file should be deleted as it is now out of date.
	// It will be rebuilt when the session closes.
	if (CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Modify Entry Step Failed - index file present when dirty"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}
		
	CloseAgendaL();
	if (!CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Modify Entry Step Failed - no index file created after modify and close"));
		SetTestStepResult(EFail);
		return EFail;
		}
		
	// make sure the added entry is still there
	OpenAgendaL();
	if (!ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Modify Entry Step Failed validating DB contents after modification"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}	
	CloseAgendaL();

	SetTestStepResult(EPass);
	return EPass;
	}

void CTestCalIndexFileModifyEntryStep::ModifyEntryL()
	{
	TPtrC entryString;
	TBool readRes = EFalse;
	readRes = GetStringFromConfig(ConfigSection(), KModifyEntry, entryString);
	if (!readRes)
		{
		INFO_PRINTF1(_L("Error in CTestCalIndexFileModifyEntryStep::ModifyEntryL - entrytomodify not found in config file"));
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
		CleanupClosePushL(instances);

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
			INFO_PRINTF2(_L("compare modify info to entry %d"), (counter+1));
			if (inf->CompareToEntryL((instances[counter])->Entry()))
				{
				INFO_PRINTF2(_L("Entry info matched instance %d"), (counter+1));
				found = ETrue;
				
				CCalProgress* progress1 = new(ELeave)CCalProgress;
				CleanupStack::PushL(progress1);  
				
				CCalEntryView* eView = CCalEntryView::NewL(*iSession, *progress);
				CleanupStack::PushL(eView);
				
				CActiveScheduler::Start();

				CCalEntry* eEntry = &((instances[counter])->Entry());
				eEntry->SetDescriptionL(KTCIModifiedText());
				RPointerArray<CCalEntry> eList;
				CleanupResetAndDestroyPushL(eList);
				eList.Reset();
				TInt numChanged = 0;
				eList.Append(eEntry);
				eView->UpdateL(eList, numChanged);
				if (numChanged != 1)
					{
					INFO_PRINTF1(_L("Error modifiying entry"));
					User::Leave(KErrGeneral);
					}
				CleanupStack::PopAndDestroy(&eList);
				CleanupStack::PopAndDestroy(2);
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
		INFO_PRINTF1(_L("Could not find entrytomodify in database"));
		User::Leave(KErrNotFound);
		}
	}
	

void CTestCalIndexFileModifyEntryStep::GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo, TInt& aNumEntries, TBool aPerformCrudOps)
	{
	// first get the default entries from the file
	CTestCalIndexFileStepBase::GetEntryInfoFromConfigL(aEntriesInfo, aNumEntries, aPerformCrudOps);
	
	if (aPerformCrudOps)
		{
		TInt entryToMod;
		TBool readRes = EFalse;
		readRes = GetIntFromConfig(ConfigSection(), KNumModifyEntry, entryToMod);
		if (!readRes)
			{
			INFO_PRINTF1(_L("Error in CTestCalIndexFileModifyEntryStep::GetEntryInfoFromConfigL - entrynumtomodify not found in config file"));
			User::Leave(KErrNotFound);
			}
			
		if (entryToMod >= aNumEntries)
			{
			INFO_PRINTF1(_L("Error in CTestCalIndexFileDeleteEntryStep::GetEntryInfoFromConfigL - entrynumtomodify too big"));
			User::Leave(KErrTooBig);
			}

		iEntryNumToMod = entryToMod-1;
		CConfigTestEntryInfo* entryInfo = aEntriesInfo[iEntryNumToMod];
		
		TInt len = KTCIModifiedText().Length();
		if (entryInfo->iDescription)
			{
			delete entryInfo->iDescription;
			entryInfo->iDescription = NULL;
			}
		entryInfo->iDescription = HBufC::NewL(len);
		*(entryInfo->iDescription) = KTCIModifiedText();

		}
	}
