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

#include "TestCalIndexFileCreateDbStep.h"
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

CTestCalIndexFileCreateDbStep::~CTestCalIndexFileCreateDbStep()
/**
 * Destructor
 */
	{
	}

CTestCalIndexFileCreateDbStep::CTestCalIndexFileCreateDbStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTestCalIndexFileCreateDbStep);
	}

TVerdict CTestCalIndexFileCreateDbStep::doTestStepPostambleL()
	{
	return TestStepResult();
	}

TVerdict CTestCalIndexFileCreateDbStep::doTestStepL()
	{
	// this method assumes a pass. Checks that fail will stop
	// the test step
	
	SetUpCalDirL();
	
	OpenAgendaL();
	AddEntryL();
	CloseAgendaL();
	
	// should have calendar and index file now
	if (!CheckCalendarFilePresentL())
		{
		SetTestStepResult(EFail);
		return EFail;
		}
		
	if (!CheckIndexFilePresentL())
		{
		SetTestStepResult(EFail);
		return EFail;
		}
	
	
	// use base class version of OpenAgendaL since calendar file is now
	// known
	CTestCalIndexFileStepBase::OpenAgendaL();
	if (!ValidateDbContentsL())
		{
		INFO_PRINTF1(_L("Open Step Failed validating DB contents with idx file"));
		CloseAgendaL();
		SetTestStepResult(EFail);
		return EFail;
		}	
	CloseAgendaL();

	return EPass;
	}

TBool CTestCalIndexFileCreateDbStep::ValidateDbContentsL()
	{
	TInt numEntries = 0; // there were initially no entries	
	RPointerArray<CConfigTestEntryInfo> entriesInfo;
	CleanupResetAndDestroyPushL(entriesInfo);	

	GetEntryInfoFromConfigL(entriesInfo, numEntries, ETrue);
			
	// see if we can find a match in the database using an instance view
	INFO_PRINTF1(_L("Trying to validate entry info"));
	CConfigTestEntryInfo* infoToUse = entriesInfo[0]; // we have only added 1
	
	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);  

	CCalInstanceView* view = CCalInstanceView::NewL(GetSession(), *progress);
	CleanupStack::PushL(view);
	
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
	CleanupResetAndDestroyPushL(instances);
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
		
	TInt counter = 0;
	TInt max = instances.Count();
	INFO_PRINTF2(_L("%d instances to compare against"),max);
	TBool found = EFalse;
	while ((found == EFalse) && (counter < max))
		{
		INFO_PRINTF2(_L("compare info to entry %d"), (counter+1));
		if (infoToUse->CompareToEntryL((instances[counter])->Entry()))
			{
			INFO_PRINTF2(_L("Entry info matched instance %d"), (counter+1));
			found = ETrue;
			}
		else
			{
			counter++;
			}
		}
		CleanupStack::PopAndDestroy(&instances);
		CleanupStack::PopAndDestroy(view);
		CleanupStack::PopAndDestroy(progress);
		
		if (!found)
			{
			CleanupStack::PopAndDestroy(&entriesInfo);
			INFO_PRINTF1(_L("Could not validate entry info"));
			return EFalse;
			}
		
	CleanupStack::PopAndDestroy(&entriesInfo);
	return ETrue;
	}

void CTestCalIndexFileCreateDbStep::OpenAgendaL()
	{
	SetSession(CCalSession::NewL());
	
	TFileName calName;
	MakeCalendarFileName(calName);	
	iSession->CreateCalFileL(calName);
	
	iSession->OpenL(calName);
	}

void CTestCalIndexFileCreateDbStep::GetEntryInfoFromConfigL(RPointerArray<CConfigTestEntryInfo>& aEntriesInfo, TInt& aNumEntries, TBool /*aPerformCrudOps*/)
	{
	// for this subclass we don't care about CRUD operations.
	
	// add the new entry info to the list
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

