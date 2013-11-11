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

#include "caltestlib.h"
#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <e32test.h>
#include <caliterator.h>
#include <caluser.h>
#include <calrrule.h>

_LIT(KTestName,"tcal_finddaywithinstance");

RTest test(KTestName);


class CFindInstanceDayTestManager : public CBase
	{
public:

    static CFindInstanceDayTestManager* NewLC();
	~CFindInstanceDayTestManager();
	void StoreEntryL();
	TInt FindInstanceL();
	void FindDayWithInstanceL();
	
private:

	// avoid any method but NewL instancing the class
	CFindInstanceDayTestManager() {	}
	// no copy constructor and assignment operator
	CFindInstanceDayTestManager(CFindInstanceDayTestManager& );
	CFindInstanceDayTestManager& operator = (const CFindInstanceDayTestManager& );

	void ConstructL();

	
private:

	CCalTestLibrary* iTestLibrary;
	};
//entry1
const TTime KEntry1Start(TDateTime(2000, EJune, 0, 1, 1, 0, 0));	//Thursday
const TTime KEntry1End(TDateTime(2000, EJune, 0, 2, 2, 0, 0));		
//entry2
const TTime KEntry2Start(TDateTime(2000, EJune, 0, 13, 0, 0, 0));	//Thursday
const TTime KEntry2End(TDateTime(2000, EJune, 0, 14, 0, 0, 0));	
//entry3 (todo)
const TTime KEntry3End(TDateTime(2000, EJune, 0, 23, 0, 0, 0));		//Thursday

// Constructor/Destructor

CFindInstanceDayTestManager* CFindInstanceDayTestManager::NewLC()
	{
	CFindInstanceDayTestManager* self = new (ELeave) CFindInstanceDayTestManager();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}


void CFindInstanceDayTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	iTestLibrary->ReplaceFileL(KTestName);
	iTestLibrary->OpenFileL(KTestName);	
	}	


CFindInstanceDayTestManager::~CFindInstanceDayTestManager()	
	{
	delete iTestLibrary;
	}


void CFindInstanceDayTestManager::StoreEntryL()
	{
	test.Next(_L("Store entries"));


	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
	
	//Create first entry appt, starting at midnight 1st June 2000 weekly repeating for three weeks
	_LIT8(KGuidId1, "KGuidId1");
	HBufC8* guid1 = KGuidId1().AllocLC();

	CCalEntry::TType entryType = CCalEntry::EAppt;
	CCalEntry* entry1 = iTestLibrary->CreateCalEntryL(entryType, guid1);
	CleanupStack::Pop(guid1);
	
	CleanupStack::PushL(entry1);
	
	TCalTime start;
	TCalTime end;
	start.SetTimeLocalL(KEntry1Start);
	end.SetTimeLocalL(KEntry1End);
	
	entry1->SetStartAndEndTimeL(start, end);
	TCalRRule rpt1(TCalRRule::EWeekly);
	rpt1.SetInterval(1);
	TCalTime repeatStart;
	repeatStart.SetTimeLocalL(KEntry1Start);
	rpt1.SetDtStart(repeatStart);
	
	RArray<TDay> days1;
	CleanupClosePushL(days1);
	days1.AppendL(EThursday);
	rpt1.SetByDay(days1);
	CleanupStack::PopAndDestroy(&days1);

	rpt1.SetCount(3);
	entry1->SetRRuleL(rpt1);
	entriesToStore.AppendL(entry1);
	CleanupStack::Pop(entry1);
	

	//Create the second entry appt, starting at mid day 1st June 2000 weekly repeating for three weeks
	_LIT8(KGuidId2, "KGuidId2");
	HBufC8* guid2 = KGuidId2().AllocLC();

	entryType = CCalEntry::EAppt;
	CCalEntry* entry2 = iTestLibrary->CreateCalEntryL(entryType, guid2);
	CleanupStack::Pop(guid2);
	CleanupStack::PushL(entry2);
	
	start.SetTimeLocalL(KEntry2Start);
	end.SetTimeLocalL(KEntry2End);
	entry2->SetStartAndEndTimeL(start, end);
	
	TCalRRule rpt2(TCalRRule::EWeekly);
	rpt2.SetInterval(1);
	rpt2.SetDtStart(start);
	RArray<TDay> days2;
	CleanupClosePushL(days2);
	days2.AppendL(EThursday);
	rpt2.SetByDay(days2);
	CleanupStack::PopAndDestroy(&days2);

	rpt2.SetCount(3);
	entry2->SetRRuleL(rpt2);
	entriesToStore.AppendL(entry2);
	CleanupStack::Pop(entry2);

	//Create the third entry todo, due date is midnight 2st June 2000 weekly repeating for three weeks
	_LIT8(KGuidId3, "KGuidId3");
	HBufC8* guid3 = KGuidId3().AllocLC();

	entryType = CCalEntry::ETodo;
	CCalEntry* entry3 = iTestLibrary->CreateCalEntryL(entryType, guid3);
	CleanupStack::Pop(guid3);
	CleanupStack::PushL(entry3);
	
	start.SetTimeLocalL(Time::NullTTime());
	end.SetTimeLocalL(KEntry3End);
	entry3->SetStartAndEndTimeL(start, end);

	TCalRRule rpt3(TCalRRule::EWeekly);
	rpt3.SetInterval(1);
	rpt3.SetDtStart(end);
	RArray<TDay> days3;
	CleanupClosePushL(days3);
	days3.AppendL(EThursday);
	rpt3.SetByDay(days3);
	CleanupStack::PopAndDestroy(&days3);

	rpt3.SetCount(3);
	entry3->SetRRuleL(rpt3);
	entriesToStore.AppendL(entry3);
	CleanupStack::Pop(entry3);
	
	TInt num;
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore,num);
	test(num=3);
	CleanupStack::PopAndDestroy(&entriesToStore);
	}



TInt CFindInstanceDayTestManager::FindInstanceL()
	{
	test.Next(_L("Find Instance to make sure there are right number of instances"));


	TCalTime minTime;
	minTime.SetTimeLocalL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeLocalL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);

	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(instances, filter, timeRange);
	TInt instanceNum = instances.Count();
	for (TInt ii=0; ii<instanceNum; ++ii)
		{
		TTime instancetime = (instances[ii])->Time().TimeLocalL();
	
		TBuf<100> tempBuf;	
		_LIT(KTTimeDateFormat,"Time: %H:%T:%S:%*C3 Date:%*1/%2/%3");
		instancetime.FormatL(tempBuf,KTTimeDateFormat);
		RDebug::Print(_L("%S"), &tempBuf);
		}
	CleanupStack::PopAndDestroy(&instances);
	return 	instanceNum;
	}
	


void CFindInstanceDayTestManager::FindDayWithInstanceL()
	{
	test.Next(_L("Find Instance in the expected day"));


	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	
	const TTime KFirstDayWithInstance(TDateTime(2000, EJune, 0, 1, 1, 0, 0));
	const TTime KSecondDayWithInstance(TDateTime(2000, EJune, 7, 1,1, 0, 0));
	const TTime KLastDayWithInstance(TDateTime(2000, EJune, 14, 23, 0, 0, 0));

	TCalTime start;
	start.SetTimeLocalL(TCalTime::MinTime());
	TCalTime instanceFind = iTestLibrary->SynCGetInstanceViewL().NextInstanceL(filter, start);
	TTime instanceFindTTime = instanceFind.TimeLocalL();
	//The first instance date should be found
	test (instanceFindTTime == KFirstDayWithInstance);

	RDebug::Print(_L("Min time NextInstanceL"));
	TBuf<100> tempBuf;	
	_LIT(KTTimeDateFormat,"Time: %H:%T:%S:%*C3 Date:%*1/%2/%3");
	instanceFindTTime.FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("%S\n\n"), &tempBuf);
	
	start.SetTimeLocalL(TCalTime::MaxTime());
	instanceFind = iTestLibrary->SynCGetInstanceViewL().PreviousInstanceL(filter, start);
	instanceFindTTime = instanceFind.TimeLocalL();
	// The last instance date should be found
	test (instanceFindTTime == KLastDayWithInstance);
	
	RDebug::Print(_L("Max time PreviousInstanceL"));
	instanceFindTTime.FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("%S\n\n"), &tempBuf);

	start.SetTimeLocalL(KEntry1Start);
	instanceFind = iTestLibrary->SynCGetInstanceViewL().NextInstanceL(filter, start);
	instanceFindTTime = instanceFind.TimeLocalL();
	//The second Thurday should be found
	test (instanceFindTTime == KSecondDayWithInstance );

	start.TimeLocalL().FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("start NextInstanceL - %S"), &tempBuf);
	instanceFindTTime.FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("%S\n\n"), &tempBuf);

	start.SetTimeLocalL(KFirstDayWithInstance + TTimeIntervalDays(1));
	instanceFind = iTestLibrary->SynCGetInstanceViewL().NextInstanceL(filter, start);
	instanceFindTTime = instanceFind.TimeLocalL();
	//The second Thurday should be found
	test (instanceFindTTime == KSecondDayWithInstance );

	start.TimeLocalL().FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("start NextInstanceL- %S"), &tempBuf);
	instanceFindTTime.FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("%S\n\n"), &tempBuf);

	start.SetTimeLocalL(KEntry1Start);
	instanceFind=iTestLibrary->SynCGetInstanceViewL().PreviousInstanceL(filter, start);
	//There is no instance before the first instance
	test(instanceFind.TimeLocalL() == Time::NullTTime());
	
	start.TimeLocalL().FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("start PreviousInstanceL- %S"), &tempBuf);
	instanceFind.TimeLocalL().FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("%S\n\n"), &tempBuf);
	
	start.SetTimeLocalL(KLastDayWithInstance);
	instanceFind = iTestLibrary->SynCGetInstanceViewL().NextInstanceL(filter, start);
	//There is no instances after the Last instance
	test(instanceFind.TimeLocalL() == Time::NullTTime());

	start.TimeLocalL().FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("start NextInstanceL - %S"), &tempBuf);
	instanceFind.TimeLocalL().FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("%S\n\n"), &tempBuf);
	}
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CFindInstanceDayTestManager* testManager = CFindInstanceDayTestManager::NewLC();
	//Store 3 entries each of them is weekly repeating for 3 weeks
	testManager->StoreEntryL();
	//Make sure there are 9 instances are found
	test (testManager->FindInstanceL() == 9);
	testManager->FindDayWithInstanceL();
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-FINDDAYWITHINSTANCE-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-FINDDAYWITHINSTANCE-0001 Calendar Interim API finddaywithinstance test suite"));

	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	if (!trapCleanup)
		{
		return KErrNoMemory;
		}

	CActiveScheduler* scheduler = new CActiveScheduler();
	if (!scheduler)
		{
		delete trapCleanup;
		return KErrNoMemory;
		}
	CActiveScheduler::Install(scheduler);	

	TRAPD(ret, DoTestL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
    }
