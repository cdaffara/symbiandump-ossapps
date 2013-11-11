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

#include "TestCalIndexFileDeleteAllStep.h"
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

CTestCalIndexFileDeleteAllStep::~CTestCalIndexFileDeleteAllStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFileDeleteAllStep::CTestCalIndexFileDeleteAllStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalIndexFileDeleteAllStep);
	}

TVerdict CTestCalIndexFileDeleteAllStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFileDeleteAllStep::doTestStepL()
	{
	// this method assumes a pass. Checks that fail will stop
	// the test step
	
	SetUpCalDirL();
	
	AddCalendarFileL();
	AddIndexFileL();
	OpenAgendaL();
	
	// use the base class version of ValidateDbContentsL as 
	// we have not deleted the entries yet
	if (!CTestCalIndexFileStepBase::ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Delete Entry Step Failed validating DB contents before entry deletion"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}
		
	DeleteAllEntriesL();
	// The index file should be deleted as it is now out of date.
	// It will be rebuilt when the session closes.
	if (CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Delete All Step Failed - index file present when dirty"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}
		
	CloseAgendaL();
	if (!CheckIndexFilePresentL())
		{
		INFO_PRINTF1(_L("Delete All Step Failed - no index file created after delete and close"));
		SetTestStepResult(EFail);
		return EFail;
		}
		
	// make sure there are no entries is left
	OpenAgendaL();
	if (!ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Delete All Step Failed validating DB contents after deletion"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}	
	CloseAgendaL();

	SetTestStepResult(EPass);
	return EPass;
	}



void CTestCalIndexFileDeleteAllStep::DeleteAllEntriesL()
	{
	

	INFO_PRINTF1(_L("enter CTestCalIndexFileDeleteAllStep::DeleteAllEntriesL()"));
	// for each entry in the database validate ALL relevant
	// fields against info for the entry in the ini file
	TInt numEntries;
	TBool readRes = EFalse;
	readRes = GetIntFromConfig(ConfigSection(),KTCINumEntries, numEntries);
	if (!readRes)
		{
		INFO_PRINTF1(_L("Error in CTestCalIndexFileStepBase::ValidateDbContentsL - numentriespresent not found in config file"));
		User::Leave(KErrNotFound);
		}
	
	if (numEntries == 0)
		{
		return;
		}
	
	RPointerArray<CConfigTestEntryInfo> entriesInfo;	
	CleanupResetAndDestroyPushL(entriesInfo);	

	GetEntryInfoFromConfigL(entriesInfo, numEntries, EFalse);
			
	// for each entry in the list of entries, see if we can find a 
	// match in the database using an instance view
	for (TInt j = 0; j < numEntries; j++)
		{
		INFO_PRINTF2(_L("Trying to validate entry info - %d"),(j+1));
		CConfigTestEntryInfo* infoToUse = entriesInfo[j];
		
		CCalProgress* progress = new(ELeave)CCalProgress;
		CleanupStack::PushL(progress);  //1
	
		CCalInstanceView* view = CCalInstanceView::NewL(*iSession, *progress);
		CleanupStack::PushL(view); //2
		
		CActiveScheduler::Start();
		
		CalCommon::TCalViewFilter filter = CalCommon::EIncludeAppts;
		switch (infoToUse->iType)
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
		CleanupClosePushL (instances);
				
		// look for instances between (startdate -1 day) and
		// (enddate + 1 day)
		TTimeIntervalDays oneDay(1);
		TTime beginTTime = (infoToUse->iStartTime) - oneDay;
		TTime endTTime = (infoToUse->iEndTime) + oneDay;
		TCalTime begin;
		begin.SetTimeUtcL(beginTTime);
		TCalTime end;
		end.SetTimeUtcL(endTTime);
		CalCommon::TCalTimeRange timerange(begin, end);	
		
		view->FindInstanceL(instances, filter, timerange);
		
		TInt count = instances.Count();
		for (TInt i = 0 ; i < count; i++)
			{
			TRAP_IGNORE(view->DeleteL(instances[i], CalCommon::EThisAndAll));
			}

		CleanupStack::PopAndDestroy(&instances);
		CleanupStack::PopAndDestroy(view);
		CleanupStack::PopAndDestroy(progress);
		
		}
	CleanupStack::PopAndDestroy(&entriesInfo);
	INFO_PRINTF1(_L("exit CTestCalIndexFileDeleteAllStep::DeleteAllEntriesL()"));	
	}
	
TBool CTestCalIndexFileDeleteAllStep::ValidateDbContentsL()
	{
	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);  //1

	CCalInstanceView* view = CCalInstanceView::NewL(*iSession, *progress);
	CleanupStack::PushL(view); //2
	
	CActiveScheduler::Start();
	
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
		
	// Safe to look for everything since DB should be empty
	TCalTime begin;
	begin.SetTimeUtcL(TCalTime::MinTime());
	TCalTime end;
	end.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timerange(begin, end);	
	
	view->FindInstanceL(instances, filter, timerange);
	
	if (instances.Count() != 0)
		{
		INFO_PRINTF2(_L("CTestCalIndexFileDeleteAllStep::ValidateDbContentsL found %d entries"), instances.Count());
		User::Leave(KErrGeneral);
		}
	CleanupStack::PopAndDestroy(&instances);
	CleanupStack::PopAndDestroy(view);
	CleanupStack::PopAndDestroy(progress);

	return ETrue;
	}
