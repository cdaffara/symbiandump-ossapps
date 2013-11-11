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

#include <calinstanceview.h>
#include <calinstance.h>
#include "T_CrEntry.h"

_LIT(KEntries, "entries");
_LIT(KEntries2, "entries2");
_LIT(KEntries3, "entries3");
_LIT(KEntries4, "entries4");
_LIT(KEntries5, "entries5");
_LIT(KEntries6, "entries6");
_LIT(KUTC, "UTC");
_LIT(KNewUTC, "NewUTC");
_LIT(KDates, "dates");

CTestCalInterimApiStoreFetch::CTestCalInterimApiStoreFetch()
	{
	SetTestStepName(KTestCalInterimApiStoreFetch);
	}

CTestCalInterimApiStoreFetch::~CTestCalInterimApiStoreFetch()
	{
	}
	
TVerdict CTestCalInterimApiStoreFetch::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	
	TPtrC dataPtr;
	TPtrC dataPtr2;
	TPtrC dataPtr3;
	TPtrC dataPtr4;
	TPtrC dataPtr5;
	TPtrC dataPtr6;

	TESTL(GetStringFromConfig(ConfigSection(), KEntries, dataPtr));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries2, dataPtr2));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries3, dataPtr3));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries4, dataPtr4));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries5, dataPtr5));
	TESTL(GetStringFromConfig(ConfigSection(), KEntries6, dataPtr6));

	RPointerArray<CCalEntry> entries;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries));
	CreateEntriesL(dataPtr, entries);
	CreateEntriesL(dataPtr2, entries);
	CreateEntriesL(dataPtr3, entries);
	CreateEntriesL(dataPtr4, entries);
	CreateEntriesL(dataPtr5, entries);
	CreateEntriesL(dataPtr6, entries);


	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
			

	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	
	CleanupStack::PopAndDestroy(view);
	view = NULL;
	
	// Create a new view
	CCalProgress* calProgress = new(ELeave)CCalProgress;
	CleanupStack::PushL(calProgress);	
	view = CCalEntryView::NewL(*iSession, *calProgress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(calProgress);
	CActiveScheduler::Start();

	
	// try to retrieve and compare entries

	RPointerArray<CCalEntry> entries2;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries2));
	
	for(TInt i=0; i<cnt; i++)
		{
		view->FetchL(entries[i]->UidL(), entries2);
		TESTL(entries2.Count() == 1); //should be only 1 entry with that ID
		TESTL(entries[i]->CompareL(*(entries2[0])));
		entries2.ResetAndDestroy();
		}
		
	entries.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(5);
	
	return TestStepResult();
	}
//------------------------------------------------------------

CTestCalInterimApiStoreFetchRpt::CTestCalInterimApiStoreFetchRpt()
	{
	SetTestStepName(KTestCalInterimApiStoreFetchRpt);
	}

CTestCalInterimApiStoreFetchRpt::~CTestCalInterimApiStoreFetchRpt()
	{
	}
	
/**
Creates cal entries from ini data. Store it. Change UTC offset. Search instances by time.
NOTE To make test simpler all instanses should have different start time (at least 3 minutes difference)

*/	
	
TVerdict CTestCalInterimApiStoreFetchRpt::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	
	TPtrC initialTZ;
	TESTL(GetStringFromConfig(ConfigSection(), KUTC, initialTZ)); 
	TPtrC newTZ;
	TESTL(GetStringFromConfig(ConfigSection(), KNewUTC, newTZ));
	
	TPtrC dataPtr;
	TESTL(GetStringFromConfig(ConfigSection(), KEntries, dataPtr));
	TPtrC dataPtr2;
	TESTL(GetStringFromConfig(ConfigSection(), KEntries2, dataPtr2));

	SetTZL(initialTZ);
	
	RPointerArray<CCalEntry> entries;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayEntries, &entries));
 	CreateEntriesL(dataPtr, entries);
 	CreateEntriesL(dataPtr2, entries);
	
	TESTL(GetStringFromConfig(ConfigSection(), KDates, dataPtr));
	RArray<TTime> dates;
	GetDatesL(dataPtr, dates);

	TESTL(dates.Count() == entries.Count());  // dates list should match entries list
	
	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();

	
	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	CleanupStack::PopAndDestroy(view);
	view = NULL;
	
	SetTZL(newTZ);
	
	CCalProgress* calProgress = new(ELeave)CCalProgress;
	CleanupStack::PushL(calProgress);

	CCalInstanceView* instanceView=CCalInstanceView::NewL(*iSession, *calProgress) ;
	CleanupStack::PushL(instanceView);
	
	CActiveScheduler::Add(calProgress);
	CActiveScheduler::Start();

	
	RPointerArray<CCalInstance> instance;
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteRPtrArrayInstances, &instance));
 	TTimeIntervalMinutes delta = 1; // To separate values incase dayShift is 0
 	
	for (TInt i=0; i<cnt; i++)
		{
		for (TInt day=0; day <10; day++)
			{
			
			TCalTime startTime;
			TCalTime endTime;
			TTimeIntervalDays dayShift(day); // distribute utc by day
			startTime.SetTimeUtcL((dates[i]-delta+dayShift));
			endTime.SetTimeUtcL((dates[i]+delta+dayShift));
			CalCommon::TCalTimeRange range(startTime, endTime);
			
			instanceView->FindInstanceL(instance, CalCommon::EIncludeAll, range);
			TESTL(instance.Count()==1);  //assume no time overlap between instances
			CCalEntry* ce1 = entries[i];
			CCalEntry& ce2 = instance[0]->Entry();
			TESTL(entries[i]->CompareL(instance[0]->Entry()));
			instance.ResetAndDestroy();
			}
		
		}
	CleanupStack::PopAndDestroy(5);

	RestoreTZL();

	entries.ResetAndDestroy();	
	return TestStepResult();
	}	

void CTestCalInterimApiStoreFetchRpt::GetDatesL(TPtrC& aDataPtr, RArray<TTime>& dates)
	{
	TESTL(MAX_TEXT_MESSAGE >= aDataPtr.Length());
	TBuf<MAX_TEXT_MESSAGE>  data(aDataPtr);
	
	while (data.Length())
		{
		TBuf<MAX_LONG_STRLEN> item;
		TInt pos = data.Locate(';');
		item = data.Left(pos);
		data.Delete(0, pos+1);
		TTime time(item);
		dates.AppendL(time);
		}
		
	}



