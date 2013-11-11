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

#include "caltestlib.h"
#include <calentry.h>
#include <calentryview.h>
#include <calalarm.h>
#include <calcategory.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calrrule.h>
#include <caliterator.h>
#include <e32test.h>
#include <s32file.h>

LOCAL_D RTest test(_L("tcal_oomfind"));

class CCalOOMTest : public CBase
	{
public:
	static CCalOOMTest* NewLC();
 	void DoTestL();
	~CCalOOMTest();

private:
	void ConstructL();
	void CreateEntriesL();
	void TestFindInstancesL(const TDesC& aFindStartTime, const TDesC& aFindEndTime, CalCommon::TCalViewFilter aFilter);
private:
	CCalTestLibrary* iTestLib;
	};


_LIT(KCalName, "OOMDb");

	
void CCalOOMTest::CreateEntriesL()
	{
	_LIT8(KGuid1, "Reminder-1");
	_LIT8(KGuid2, "Appt-1");
		
	_LIT(KEntryStartTime, 		"20121119:013000.000000"); // 20-12-2012 01.30
	_LIT(KEntryEndTime, 		"20121119:090000.000000"); // 20-12-2012 09.00
	
	_LIT(KTimeUntil, 			"20131119:013000.000000"); // 20-12-2013 01.30
	
	_LIT(KSummary1, "THIS SUMMARY");
	_LIT(KSummary2, "Appt summary");
	
	
	//entry 1	
	HBufC8* guid8 = HBufC8::NewL(KGuid1().Length());
	guid8->Des().Copy(KGuid1);
	CleanupStack::PushL(guid8);
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EReminder, guid8, CCalEntry::EMethodAdd, (TUint)0);	
	CleanupStack::Pop(guid8);
	entry->SetSummaryL(KSummary1);
	
	// entry2
	guid8 = HBufC8::NewL(KGuid2().Length());
	guid8->Des().Copy(KGuid2);
	CleanupStack::PushL(guid8);
	CCalEntry* entry2 = CCalEntry::NewL(CCalEntry::EAppt, guid8, CCalEntry::EMethodAdd, (TUint)0);	
	CleanupStack::Pop(guid8);
	entry2->SetSummaryL(KSummary2);
	
	TTime time1(KEntryStartTime());
	TTime time2(KEntryEndTime());
	TCalTime st;
	st.SetTimeUtcL(time1);
	TCalTime end;
	end.SetTimeUtcL(time2);
		
	entry->SetStartAndEndTimeL(st, end); 
	entry2->SetStartAndEndTimeL(st, end);
	
	TCalRRule* rRule=new (ELeave) TCalRRule(TCalRRule::EDaily);
	TCalTime calTime = entry->StartTimeL();
	rRule->SetDtStart(calTime);
	rRule->SetInterval(1);
	TTime until(KTimeUntil());
	TCalTime untilTime;
	untilTime.SetTimeUtcL(until);
	rRule->SetUntil(untilTime); 
	
	entry->SetRRuleL(*rRule);
	entry2->SetRRuleL(*rRule);
	
	TInt						entriesStored(0);
	RPointerArray<CCalEntry>	entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
	entriesToStore.AppendL(entry);
	entriesToStore.AppendL(entry2);
	iTestLib->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);

	delete rRule;
	CleanupStack::PopAndDestroy(&entriesToStore);
	
	// Find
	_LIT(KFindSt, 		"20131117:013000.000000"); // The event "overlaps" with this search range, hence must be caught. The reminder must not be caught (its end time = start time).
	_LIT(KFindEnd, 		"20131119:093000.000000");

	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TestFindInstancesL(KFindSt, KFindEnd, filter); // calling OOM test function
	}


void CCalOOMTest::TestFindInstancesL(const TDesC& aFindStartTime, const TDesC& aFindEndTime, CalCommon::TCalViewFilter aFilter)
	{
	test.Next(_L("OOM tests for FindInstanceL"));

	_LIT(KSearchText,				"summary");
	TCalTime findSt;
	findSt.SetTimeLocalL(TTime(aFindStartTime));
	TCalTime findEnd;
	findEnd.SetTimeLocalL(TTime(aFindEndTime));
	CalCommon::TCalTimeRange range(findSt, findEnd);
	
	RPointerArray<CCalInstance> instances;
	CCalInstanceView::TCalSearchParams params(KSearchText, CalCommon::EFoldedTextSearch);
	
	CCalInstanceView& view = iTestLib->SynCGetInstanceViewL();
	
	TInt tryCount = 1;
	TInt err = 0;
	
	// OOM LOOP
	for ( ;; )
		{
		
		RDebug::Printf("%d", tryCount);
		instances.ResetAndDestroy();
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		
		__UHEAP_MARK;
		TRAP(err, view.FindInstanceL(instances, aFilter, range, params));
		if(	instances.Count()>0 )
		{
			instances.ResetAndDestroy();	//we are responsible for Cleanup of this array
		}
		__UHEAP_MARKEND;
	
		if ( err==KErrNone ) 
		{
			__UHEAP_RESET;
			RDebug::Printf("Memory allocation testing for FindInstance is done");
			break;
		}
		test(err == KErrNoMemory);
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		tryCount++;
		}
	// OOM LOOP
	}
	
void CCalOOMTest::DoTestL()
	{
		CreateEntriesL();
	}

CCalOOMTest* CCalOOMTest::NewLC()
	{
	CCalOOMTest* self = new (ELeave) CCalOOMTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CCalOOMTest::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KCalName());
	iTestLib->OpenFileL(KCalName());
	}
	
CCalOOMTest::~CCalOOMTest()
	{
	delete iTestLib;
	}


static void DoMainL()
	{
	CCalOOMTest* testApp = CCalOOMTest::NewLC();
	testApp->DoTestL();
	CleanupStack::PopAndDestroy(testApp);
	}
	
	
/**

@SYMTestCaseID     PIM-TCAL-OOMFIND-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	test(scheduler != NULL);
	CActiveScheduler::Install(scheduler);
	
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-OOMFIND-0001 OOM tests for CalinterimAPI"));

	test.Title();
	TRAPD(r, DoMainL());
	test(r == KErrNone);
	test.End();
	test.Close();
	
	delete scheduler;
	delete trapCleanup;

	__UHEAP_MARKEND;
	return KErrNone;
    }

