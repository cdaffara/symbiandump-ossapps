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

#include "../caltestlib.h"
#include "../caltestoom.h"
#include <calentry.h>
#include <calentryview.h>
#include <calinstanceview.h>
#include <calsession.h>
#include <e32test.h>
#include <caluser.h>
#include <calrrule.h>
#include <calinstance.h>

_LIT(KTestName, "tcal_fetchinstance");
RTest test(KTestName);

// oom times
TTime gOomParentStartTimeLocal(TDateTime(2007, EJuly, 20, 5, 0, 0, 0));
TTime gOomParentEndTimeLocal(TDateTime(2007, EJuly, 20, 6, 0, 0, 0));
TTime gOomParentRDateTimeLocal(TDateTime(2007, EJuly, 27, 9, 0, 0, 0));
TTime gOomParentExDateTimeLocal(TDateTime(2007, EJuly, 23, 5, 0, 0, 0));

TCalLocalUid gOomParentLocalUid;

// the child entry times
TTime gOomChildStartTimeLocal(TDateTime(2007, EJuly, 21, 8, 0, 0, 0));
TTime gOomChildEndTimeLocal(TDateTime(2007, EJuly, 21, 9, 0, 0, 0));
TTime gOomChildRecIdTimeLocal(TDateTime(2007, EJuly, 21, 5, 0, 0, 0));
TCalLocalUid gOomChildLocalUid;

_LIT8(KOomGuid, "oom_tcal_fetchinstance");
_LIT8(KGuid, "tcal_fetchinstance");

class CInstanceIteratorTestManager : public CBase, public MCalTestOomCallBack
	{
public:

    static CInstanceIteratorTestManager* NewLC();
	~CInstanceIteratorTestManager();
	
	void CreateTestDataL();
	
	// Fetch all different kinds of instances:
	void TestFetchCompletedTodoInstanceL();
	void TestFetchUndatedTodoInstanceL();
	void TestFetchDatedTodoInstanceL();
	void TestFetchEventInstanceL();
	void TestFetchRepeatingTodoInstanceL();
	void TestFetchRepeatingEventAndChildInstanceL();
	void TestFindInstanceEndingBehaviour();
	
	void OomTestL();

private: // from MCalTestOomCallBack
	void OomTestL(TType aType, TInt aFailAt);

private:
	void ConstructL();
	
private:
	CCalTestLibrary* 	iTestLibrary;
	};

CInstanceIteratorTestManager* CInstanceIteratorTestManager::NewLC()
	{
	CInstanceIteratorTestManager* self = new (ELeave) CInstanceIteratorTestManager();
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}


void CInstanceIteratorTestManager::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	iTestLibrary->ReplaceFileL(KTestName);
	iTestLibrary->OpenFileL(KTestName);	
	}	


CInstanceIteratorTestManager::~CInstanceIteratorTestManager()	
	{
	delete iTestLibrary;
	}

void CInstanceIteratorTestManager::TestFetchCompletedTodoInstanceL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// the entry times
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2007, EMarch, 20, 5, 0, 0, 0));
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2007, EMarch, 20, 6, 0, 0, 0));
	TCalTime completedTime;
	completedTime.SetTimeLocalL(TDateTime(2007, EMarch, 20, 7, 0, 0, 0));
	_LIT(KEntryDescription, "Completed Todo");
	
	// create a completed todo
	test.Printf(_L("create a completed todo"));
	HBufC8* guid = NULL;
	CCalEntry* completedTodo = iTestLibrary->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::PushL(completedTodo);
	
	completedTodo->SetDescriptionL(KEntryDescription);
	completedTodo->SetStartAndEndTimeL(startTime, endTime);
	completedTodo->SetCompletedL(ETrue, completedTime);
	
	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*completedTodo);
	TCalLocalUid localUid = completedTodo->LocalUidL();
	CleanupStack::PopAndDestroy(completedTodo);
	
	// do some testing
	test.Printf(_L("fetch the instance"));
	CCalInstance* compltedInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, completedTime);
	CleanupStack::PushL(compltedInstance);
	
	test.Printf(_L("test the instance time"));
	test(compltedInstance->Time().TimeLocalL() == completedTime.TimeLocalL());
	
	test.Printf(_L("test that the entry is completed "));
	test(compltedInstance->Entry().CompletedTimeL().TimeLocalL() == completedTime.TimeLocalL());
	
	test.Printf(_L("fetch the description from the entry"));
	test(compltedInstance->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	
	CleanupStack::PopAndDestroy(compltedInstance);
	
	test.Printf(_L("try to fetch the instance using the wrong local uid "));
	TRAPD(wrongLocalUidError, compltedInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid + 1, completedTime));
	test(wrongLocalUidError == KErrNotFound);
	
	test.Printf(_L("try to fetch the instance using the wrong time"));
	TRAPD(wrongTimeError, compltedInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, endTime));
	test(wrongTimeError == KErrNotFound);
	}


void CInstanceIteratorTestManager::TestFindInstanceEndingBehaviour()
	{
	iTestLibrary->CleanDatabaseL();

	// the entry times 
	TCalTime startTimeA, startTimeB, startTimeC;
	TCalTime endTimeA, endTimeB, endTimeC;
	startTimeA.SetTimeLocalL(TDateTime(2004, EJuly, 10, 4, 0, 0, 0));
	endTimeA.SetTimeLocalL(TDateTime(2004, EJuly, 10, 10, 0, 0, 0));
	startTimeB.SetTimeLocalL(TDateTime(2004, EJuly, 10, 2, 0, 0, 0));
	endTimeB.SetTimeLocalL(TDateTime(2004, EJuly, 10, 10, 0, 0, 0));
	startTimeC.SetTimeLocalL(TDateTime(2004, EJuly, 10, 4, 0, 0, 0));
	endTimeC.SetTimeLocalL(TDateTime(2004, EJuly, 10, 13, 0, 0, 0));

	_LIT(KEntryDescriptionA, "Event A");	
	_LIT(KEntryDescriptionB, "Event B");	
	_LIT(KEntryDescriptionC, "Event C");	

	// create and store event A
	test.Printf(_L("create event A"));
	HBufC8* guidA = NULL;
	CCalEntry* eventA = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, guidA);
	CleanupStack::PushL(eventA);
	eventA->SetDescriptionL(KEntryDescriptionA);
	eventA->SetStartAndEndTimeL(startTimeA, endTimeA);
	iTestLibrary->StoreEntryL(*eventA);
	CleanupStack::PopAndDestroy(eventA);	

	// create and store event B
	test.Printf(_L("create event B"));
	HBufC8* guidB = NULL;
	CCalEntry* eventB = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, guidB);
	CleanupStack::PushL(eventB);
	eventB->SetDescriptionL(KEntryDescriptionB);
	eventB->SetStartAndEndTimeL(startTimeB, endTimeB);
	iTestLibrary->StoreEntryL(*eventB);
	CleanupStack::PopAndDestroy(eventB);
	
	// create and store event C
	test.Printf(_L("create event C"));
	HBufC8* guidC = NULL;
	CCalEntry* eventC = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, guidC);
	CleanupStack::PushL(eventC);
	eventC->SetDescriptionL(KEntryDescriptionC);
	eventC->SetStartAndEndTimeL(startTimeC, endTimeC);
	iTestLibrary->StoreEntryL(*eventC);
	CleanupStack::PopAndDestroy(eventC);
	
	// test
	RPointerArray<CCalInstance> array;
	CleanupResetAndDestroyPushL(array);
	TInt flags = CalCommon::EInstanceEndTimeWithinRange;

	flags |= CalCommon::EIncludeEvents;
	flags |= CalCommon::EIncludeRptsNextInstanceOnly;

	// the search entry range
	TCalTime searchStart, searchEnd;
	searchStart.SetTimeLocalL(TDateTime(2004, EJuly, 10, 3, 0, 0, 0));
	searchEnd.SetTimeLocalL(TDateTime(2004, EJuly, 10, 11, 0, 0, 0));
	CalCommon::TCalTimeRange range(searchStart, searchEnd);
		
	iTestLibrary->SynCGetInstanceViewL().FindInstanceL(array, flags, range);

	TInt fetchedInstances = array.Count();
	test (fetchedInstances == 2);

	test.Printf(_L("Expected entries : Event A, Event B \n"));
	test.Printf(_L("Entries found :"));

	//	only two entries Event A and Event B should be found
	for(TInt i=0; i < fetchedInstances; ++i)
		{
		CCalEntry& entry = array[i]->Entry();
		test.Printf(entry.DescriptionL());
		test((entry.DescriptionL()==KEntryDescriptionA) || (entry.DescriptionL()==KEntryDescriptionB));
		}
	CleanupStack::PopAndDestroy(&array);
	}


void CInstanceIteratorTestManager::TestFetchUndatedTodoInstanceL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// declare the entry details
	_LIT(KEntryDescription, "undated todo");
	
	// create an undated todo
	test.Printf(_L("create an undated todo"));
	HBufC8* guid = NULL;
	CCalEntry* undatedTodo = iTestLibrary->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::PushL(undatedTodo);
	
	undatedTodo->SetDescriptionL(KEntryDescription);
	
	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*undatedTodo);
	TCalLocalUid localUid = undatedTodo->LocalUidL();
	CleanupStack::PopAndDestroy(undatedTodo);

	// test
	TCalTime nullTime;
	CCalInstance* undatedInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, nullTime);
	CleanupStack::PushL(undatedInstance);
	
	test.Printf(_L("test that the undated todo instance time has been set"));
	test(undatedInstance->Time().TimeLocalL() != Time::NullTTime());
	
	test.Printf(_L("fetch the description from the entry"));
	test(undatedInstance->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	
	CleanupStack::PopAndDestroy(undatedInstance);
	}

void CInstanceIteratorTestManager::TestFetchDatedTodoInstanceL()
	{
	iTestLibrary->CleanDatabaseL();

	// the entry times
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2007, EApril, 20, 5, 0, 0, 0));
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2007, EApril, 20, 6, 0, 0, 0));
	
	_LIT(KEntryDescription, "dated todo");
	
	// create a dated todo
	test.Printf(_L("create a dated todo"));
	HBufC8* guid = NULL;
	CCalEntry* datedTodo = iTestLibrary->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::PushL(datedTodo);
	
	datedTodo->SetDescriptionL(KEntryDescription);
	datedTodo->SetStartAndEndTimeL(startTime, endTime);
	
	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*datedTodo);
	TCalLocalUid localUid = datedTodo->LocalUidL();
	CleanupStack::PopAndDestroy(datedTodo);
	
	// test fetching the instance
	CCalInstance* datedTodoInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, endTime);
	CleanupStack::PushL(datedTodoInstance);
	
	test.Printf(_L("test the instance time"));
	test(datedTodoInstance->Time().TimeLocalL() == endTime.TimeLocalL());
	
	test.Printf(_L("test the entry start and end times"));
	test(datedTodoInstance->Entry().StartTimeL().TimeLocalL() == startTime.TimeLocalL());
	test(datedTodoInstance->Entry().EndTimeL().TimeLocalL() == endTime.TimeLocalL());
	
	test.Printf(_L("fetch the description from the entry"));
	test(datedTodoInstance->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	
	CleanupStack::PopAndDestroy(datedTodoInstance);
	
	test.Printf(_L("try to fetch the instance using the wrong local uid "));
	TRAPD(wrongLocalUidError, datedTodoInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid + 1, endTime));
	test(wrongLocalUidError == KErrNotFound);
	
	test.Printf(_L("try to fetch the instance using the wrong time"));
	TRAPD(wrongTimeError, datedTodoInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, startTime));
	test(wrongTimeError == KErrNotFound);
	}

void CInstanceIteratorTestManager::TestFetchEventInstanceL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// the entry times
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2007, EMay, 20, 5, 0, 0, 0));
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2007, EMay, 20, 6, 0, 0, 0));
	_LIT(KEntryDescription, "event");
	
	// create an event
	test.Printf(_L("create an event"));
	HBufC8* guid = NULL;
	CCalEntry* event = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, guid);
	CleanupStack::PushL(event);
	
	event->SetDescriptionL(KEntryDescription);
	event->SetStartAndEndTimeL(startTime, endTime);
	
	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*event);
	TCalLocalUid localUid = event->LocalUidL();
	CleanupStack::PopAndDestroy(event);
	
	// test
	CCalInstance* eventInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, startTime);
	CleanupStack::PushL(eventInstance);
	
	test.Printf(_L("test the instance time"));
	test(eventInstance->Time().TimeLocalL() == startTime.TimeLocalL());
	
	test.Printf(_L("test that the entry start and end times"));
	test(eventInstance->Entry().StartTimeL().TimeLocalL() == startTime.TimeLocalL());
	test(eventInstance->Entry().EndTimeL().TimeLocalL() == endTime.TimeLocalL());
	
	test.Printf(_L("fetch the description from the entry"));
	test(eventInstance->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	
	CleanupStack::PopAndDestroy(eventInstance);
	
	test.Printf(_L("try to fetch the instance using the wrong local uid "));
	TRAPD(wrongLocalUidError, eventInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid + 1, startTime));
	test(wrongLocalUidError == KErrNotFound);
	
	test.Printf(_L("try to fetch the instance using the wrong time"));
	TRAPD(wrongTimeError, eventInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, endTime));
	test(wrongTimeError == KErrNotFound);
	}

void CInstanceIteratorTestManager::TestFetchRepeatingTodoInstanceL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// the entry time
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(2007, EJune, 20, 5, 0, 0, 0));
	TCalTime endTime;
	endTime.SetTimeLocalL(TDateTime(2007, EJune, 20, 6, 0, 0, 0));
	TCalTime rDateTime;
	rDateTime.SetTimeLocalL(TDateTime(2007, EJune, 27, 9, 0, 0, 0));
	TCalTime exDateTime;
	exDateTime.SetTimeLocalL(TDateTime(2007, EJune, 23, 6, 0, 0, 0));
	_LIT(KEntryDescription, "repeating todo description");
	
	
	// create a repeating todo
	test.Printf(_L("create a repeating todo"));
	HBufC8* guid = NULL;
	CCalEntry* repeatingTodo = iTestLibrary->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::PushL(repeatingTodo);
	
	repeatingTodo->SetDescriptionL(KEntryDescription);
	repeatingTodo->SetStartAndEndTimeL(startTime, endTime);
	
	// create the repeat rule
	TCalRRule repeatingTodoRRule(TCalRRule::EDaily);
	repeatingTodoRRule.SetDtStart(endTime);
	repeatingTodoRRule.SetInterval(1);
	repeatingTodoRRule.SetCount(5);
	
	// set the repeat rule
	repeatingTodo->SetRRuleL(repeatingTodoRRule);
	
	// create a sporadic date
	RArray<TCalTime> repeatingTodoRDates;
	CleanupClosePushL(repeatingTodoRDates);
	repeatingTodoRDates.AppendL(rDateTime);
	repeatingTodo->SetRDatesL(repeatingTodoRDates);
	CleanupStack::PopAndDestroy(&repeatingTodoRDates);
	
	// create a exception date
	RArray<TCalTime> repeatingTodoExDates;
	CleanupClosePushL(repeatingTodoExDates);
	repeatingTodoExDates.AppendL(exDateTime);
	repeatingTodo->SetExceptionDatesL(repeatingTodoExDates);
	CleanupStack::PopAndDestroy(&repeatingTodoExDates);
	
	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*repeatingTodo);
	TCalLocalUid localUid = repeatingTodo->LocalUidL();
	CleanupStack::PopAndDestroy(repeatingTodo);
	
	// test
	test.Printf(_L("test fetching the first instance"));
	TCalTime instanceTime1;
	instanceTime1.SetTimeLocalL(endTime.TimeLocalL() + TTimeIntervalDays(0));
	CCalInstance* repeatingTodoInstance1 = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, instanceTime1);
	CleanupStack::PushL(repeatingTodoInstance1);
	test.Printf(_L("test the instance time"));
	test(repeatingTodoInstance1->Time().TimeLocalL() == instanceTime1.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	
	// Modified Start-> if DtStart date is equal to end date then align start date with DtStart date and 
	// end date = end date + duration of the entry. 
	// Thus moving the time window based on DtStart date   
	test(repeatingTodoInstance1->Entry().StartTimeL().TimeLocalL() == endTime.TimeLocalL()); 
	
	TTime newEndTime(endTime.TimeLocalL());
	TTimeIntervalMinutes deltaMinutes;
	endTime.TimeLocalL().MinutesFrom(startTime.TimeLocalL(), deltaMinutes);
	newEndTime += deltaMinutes;
	    
	TTimeIntervalMicroSeconds deltaMicroSeconds = 
	        endTime.TimeLocalL().MicroSecondsFrom(startTime.TimeLocalL() + deltaMinutes);
	newEndTime += deltaMicroSeconds;
	
	test(repeatingTodoInstance1->Entry().EndTimeL().TimeLocalL() == newEndTime);
	//Modified End<-
	
	test.Printf(_L("fetch the description"));
	test(repeatingTodoInstance1->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	CleanupStack::PopAndDestroy(repeatingTodoInstance1);
	
	TCalTime instanceTime2;
	instanceTime2.SetTimeLocalL(endTime.TimeLocalL() + TTimeIntervalDays(1));
	CCalInstance* repeatingTodoInstance2 = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, instanceTime2);
	CleanupStack::PushL(repeatingTodoInstance2);
	test.Printf(_L("test the instance time"));
	test(repeatingTodoInstance2->Time().TimeLocalL() == instanceTime2.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	
	// Modified Start-> if DtStart date is equal to end date then align start date with DtStart date and 
    // end date = end date + duration of the entry. 
    // Thus moving the time window based on DtStart date   
    test(repeatingTodoInstance2->Entry().StartTimeL().TimeLocalL() == endTime.TimeLocalL()); 
    
    newEndTime = endTime.TimeLocalL();
    
    endTime.TimeLocalL().MinutesFrom(startTime.TimeLocalL(), deltaMinutes);
    newEndTime += deltaMinutes;
        
    deltaMicroSeconds = 
            endTime.TimeLocalL().MicroSecondsFrom(startTime.TimeLocalL() + deltaMinutes);
    newEndTime += deltaMicroSeconds;
    
    test(repeatingTodoInstance2->Entry().EndTimeL().TimeLocalL() == newEndTime);
    //Modified End<-
    
	test.Printf(_L("fetch the description"));
	test(repeatingTodoInstance2->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	CleanupStack::PopAndDestroy(repeatingTodoInstance2);
	
	TCalTime instanceTime3;
	instanceTime3.SetTimeLocalL(endTime.TimeLocalL() + TTimeIntervalDays(2));
	CCalInstance* repeatingTodoInstance3 = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, instanceTime3);
	CleanupStack::PushL(repeatingTodoInstance3);
	test.Printf(_L("test the instance time"));
	test(repeatingTodoInstance3->Time().TimeLocalL() == instanceTime3.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	
	// Modified Start-> if DtStart date is equal to end date then align start date with DtStart date and 
    // end date = end date + duration of the entry. 
    // Thus moving the time window based on DtStart date   
    test(repeatingTodoInstance3->Entry().StartTimeL().TimeLocalL() == endTime.TimeLocalL()); 
    
    newEndTime = endTime.TimeLocalL();
    
    endTime.TimeLocalL().MinutesFrom(startTime.TimeLocalL(), deltaMinutes);
    newEndTime += deltaMinutes;
        
    deltaMicroSeconds = 
            endTime.TimeLocalL().MicroSecondsFrom(startTime.TimeLocalL() + deltaMinutes);
    newEndTime += deltaMicroSeconds;
    
    test(repeatingTodoInstance3->Entry().EndTimeL().TimeLocalL() == newEndTime);
    //Modified End<-
    
	test.Printf(_L("fetch the description"));
	test(repeatingTodoInstance3->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	CleanupStack::PopAndDestroy(repeatingTodoInstance3);
	
	TCalTime instanceTime4;
	instanceTime4.SetTimeLocalL(endTime.TimeLocalL() + TTimeIntervalDays(4));
	CCalInstance* repeatingTodoInstance4 = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, instanceTime4);
	CleanupStack::PushL(repeatingTodoInstance4);
	test.Printf(_L("test the instance time"));
	test(repeatingTodoInstance4->Time().TimeLocalL() == instanceTime4.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	
	// Modified Start-> if DtStart date is equal to end date then align start date with DtStart date and 
    // end date = end date + duration of the entry. 
    // Thus moving the time window based on DtStart date   
    test(repeatingTodoInstance4->Entry().StartTimeL().TimeLocalL() == endTime.TimeLocalL()); 
    
    newEndTime = endTime.TimeLocalL();
    
    endTime.TimeLocalL().MinutesFrom(startTime.TimeLocalL(), deltaMinutes);
    newEndTime += deltaMinutes;
        
    deltaMicroSeconds = 
            endTime.TimeLocalL().MicroSecondsFrom(startTime.TimeLocalL() + deltaMinutes);
    newEndTime += deltaMicroSeconds;
    
    test(repeatingTodoInstance4->Entry().EndTimeL().TimeLocalL() == newEndTime);
    //Modified End<-
    
	test.Printf(_L("fetch the description"));
	test(repeatingTodoInstance4->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	CleanupStack::PopAndDestroy(repeatingTodoInstance4);
	
	// fetch the rDate
	CCalInstance* rDateInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, rDateTime);
	CleanupStack::PushL(rDateInstance);
	test.Printf(_L("test the instance time"));
	test(rDateInstance->Time().TimeLocalL() == rDateTime.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	
	// Modified Start-> if DtStart date is equal to end date then align start date with DtStart date and 
    // end date = end date + duration of the entry. 
    // Thus moving the time window based on DtStart date   
    test(rDateInstance->Entry().StartTimeL().TimeLocalL() == endTime.TimeLocalL()); 
    
    newEndTime = endTime.TimeLocalL();
    
    endTime.TimeLocalL().MinutesFrom(startTime.TimeLocalL(), deltaMinutes);
    newEndTime += deltaMinutes;
        
    deltaMicroSeconds = 
            endTime.TimeLocalL().MicroSecondsFrom(startTime.TimeLocalL() + deltaMinutes);
    newEndTime += deltaMicroSeconds;
    
    test(rDateInstance->Entry().EndTimeL().TimeLocalL() == newEndTime);
    //Modified End<-
    
	test.Printf(_L("fetch the description"));
	test(rDateInstance->Entry().DescriptionL().Compare(KEntryDescription) == 0);
	CleanupStack::PopAndDestroy(rDateInstance);
	
	test.Printf(_L("try to fetch the exDate (negative test)"));
	TRAPD(err, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, exDateTime) );
	test(err == KErrNotFound);
	
	test.Printf(_L("try to fetch the instance using the wrong local uid "));
	TRAPD(wrongLocalUidError, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid + 1, endTime));
	test(wrongLocalUidError == KErrNotFound);
	
	test.Printf(_L("try to fetch the instance using the wrong time"));
	TRAPD(wrongTimeError, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(localUid, startTime));
	test(wrongTimeError == KErrNotFound);
	}

void CInstanceIteratorTestManager::TestFetchRepeatingEventAndChildInstanceL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// the parent entry times
	TCalTime parentStartTime;
	parentStartTime.SetTimeLocalL(TDateTime(2007, EJuly, 20, 5, 0, 0, 0));
	TCalTime parentEndTime;
	parentEndTime.SetTimeLocalL(TDateTime(2007, EJuly, 20, 6, 0, 0, 0));
	TCalTime parentRDateTime;
	parentRDateTime.SetTimeLocalL(TDateTime(2007, EJuly, 27, 9, 0, 0, 0));
	TCalTime parentExDateTime;
	parentExDateTime.SetTimeLocalL(TDateTime(2007, EJuly, 23, 5, 0, 0, 0));
	_LIT(KParentEntryDescription, "parent entry");
	
	// the child entry times
	TCalTime childStartTime;
	childStartTime.SetTimeLocalL(TDateTime(2007, EJuly, 21, 8, 0, 0, 0));
	TCalTime childEndTime;
	childEndTime.SetTimeLocalL(TDateTime(2007, EJuly, 21, 9, 0, 0, 0));
	TCalTime childRecIdTime;
	childRecIdTime.SetTimeLocalL(TDateTime(2007, EJuly, 21, 5, 0, 0, 0));
	_LIT(KChildEntryDescription, "child entry");
	
	// create a repeating event
	test.Printf(_L("create a repeating event"));
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* repeatingEvent = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(repeatingEvent);
	
	repeatingEvent->SetDescriptionL(KParentEntryDescription);
	repeatingEvent->SetStartAndEndTimeL(parentStartTime, parentEndTime);
	
	// create the repeat rule
	TCalRRule repeatingEventRRule(TCalRRule::EDaily);
	repeatingEventRRule.SetDtStart(parentStartTime);
	repeatingEventRRule.SetInterval(1);
	repeatingEventRRule.SetCount(5);
	repeatingEvent->SetRRuleL(repeatingEventRRule);
	
	// create a sporadic date
	RArray<TCalTime> repeatingEventRDates;
	CleanupClosePushL(repeatingEventRDates);
	repeatingEventRDates.AppendL(parentRDateTime);
	repeatingEvent->SetRDatesL(repeatingEventRDates);
	CleanupStack::PopAndDestroy(&repeatingEventRDates);
	
	// create a exception date
	RArray<TCalTime> repeatingEventExDates;
	CleanupClosePushL(repeatingEventExDates);
	repeatingEventExDates.AppendL(parentExDateTime);
	repeatingEvent->SetExceptionDatesL(repeatingEventExDates);
	CleanupStack::PopAndDestroy(&repeatingEventExDates);
	
	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*repeatingEvent);
	TCalLocalUid parentLocalUid = repeatingEvent->LocalUidL();
	CleanupStack::PopAndDestroy(repeatingEvent);
	
	// create a child to the repeating event
	guid = KGuid().AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0, childRecIdTime, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(childEntry);
	
	childEntry->SetDescriptionL(KChildEntryDescription);
	childEntry->SetStartAndEndTimeL(childStartTime, childEndTime);

	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*childEntry);
	TCalLocalUid childLocalUid = childEntry->LocalUidL();
	CleanupStack::PopAndDestroy(childEntry);
	
	// test
	test.Printf(_L("test fetching instance 1"));
	TCalTime instanceTime1;
	instanceTime1.SetTimeLocalL(parentStartTime.TimeLocalL() + TTimeIntervalDays(0));
	CCalInstance* repeatingInstance1 = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(parentLocalUid, instanceTime1);
	CleanupStack::PushL(repeatingInstance1);
	test.Printf(_L("test the instance time"));
	test(repeatingInstance1->Time().TimeLocalL() == instanceTime1.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	test(repeatingInstance1->Entry().StartTimeL().TimeLocalL() == parentStartTime.TimeLocalL());
	test(repeatingInstance1->Entry().EndTimeL().TimeLocalL() == parentEndTime.TimeLocalL());
	test.Printf(_L("fetch the description"));
	test(repeatingInstance1->Entry().DescriptionL().Compare(KParentEntryDescription) == 0);
	CleanupStack::PopAndDestroy(repeatingInstance1);
	
	test.Printf(_L("test fetching instance 2"));
	TCalTime instanceTime2;
	instanceTime2.SetTimeLocalL(parentStartTime.TimeLocalL() + TTimeIntervalDays(2));
	CCalInstance* repeatingInstance2 = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(parentLocalUid, instanceTime2);
	CleanupStack::PushL(repeatingInstance2);
	test.Printf(_L("test the instance time"));
	test(repeatingInstance2->Time().TimeLocalL() == instanceTime2.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	test(repeatingInstance2->Entry().StartTimeL().TimeLocalL() == parentStartTime.TimeLocalL());
	test(repeatingInstance2->Entry().EndTimeL().TimeLocalL() == parentEndTime.TimeLocalL());
	test.Printf(_L("fetch the description"));
	test(repeatingInstance2->Entry().DescriptionL().Compare(KParentEntryDescription) == 0);
	CleanupStack::PopAndDestroy(repeatingInstance2);
	
	test.Printf(_L("test fetching instance 3"));
	TCalTime instanceTime3;
	instanceTime3.SetTimeLocalL(parentStartTime.TimeLocalL() + TTimeIntervalDays(4));
	CCalInstance* repeatingInstance3 = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(parentLocalUid, instanceTime3);
	CleanupStack::PushL(repeatingInstance3);
	test.Printf(_L("test the instance time"));
	test(repeatingInstance3->Time().TimeLocalL() == instanceTime3.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	test(repeatingInstance3->Entry().StartTimeL().TimeLocalL() == parentStartTime.TimeLocalL());
	test(repeatingInstance3->Entry().EndTimeL().TimeLocalL() == parentEndTime.TimeLocalL());
	test.Printf(_L("fetch the description"));
	test(repeatingInstance3->Entry().DescriptionL().Compare(KParentEntryDescription) == 0);
	CleanupStack::PopAndDestroy(repeatingInstance3);
	
	test.Printf(_L("fetch the rDate"));
	CCalInstance* rDateInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(parentLocalUid, parentRDateTime);
	CleanupStack::PushL(rDateInstance);
	test.Printf(_L("test the instance time"));
	test(rDateInstance->Time().TimeLocalL() == parentRDateTime.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	test(rDateInstance->Entry().StartTimeL().TimeLocalL() == parentStartTime.TimeLocalL());
	test(rDateInstance->Entry().EndTimeL().TimeLocalL() == parentEndTime.TimeLocalL());
	test.Printf(_L("fetch the description"));
	test(rDateInstance->Entry().DescriptionL().Compare(KParentEntryDescription) == 0);
	CleanupStack::PopAndDestroy(rDateInstance);
	
	test.Printf(_L("fetch the child instance"));
	CCalInstance* childInstance = iTestLibrary->SynCGetInstanceViewL().FindInstanceL(childLocalUid, childStartTime);
	CleanupStack::PushL(childInstance);
	test.Printf(_L("test the instance time"));
	test(childInstance->Time().TimeLocalL() == childStartTime.TimeLocalL());
	test.Printf(_L("test the entry start and end times"));
	test(childInstance->Entry().StartTimeL().TimeLocalL() == childStartTime.TimeLocalL());
	test(childInstance->Entry().EndTimeL().TimeLocalL() == childEndTime.TimeLocalL());
	test.Printf(_L("fetch the description"));
	test(childInstance->Entry().DescriptionL().Compare(KChildEntryDescription) == 0);
	CleanupStack::PopAndDestroy(childInstance);
	
	test.Printf(_L("try to fetch the exDate (negative test)"));
	TRAPD(err, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(parentLocalUid, parentExDateTime) );
	test(err == KErrNotFound);
	
	test.Printf(_L("try to fetch the instance that has been excepted by the child entry (negative test)"));
	TCalTime exceptedByChildInstanceTime;
	exceptedByChildInstanceTime.SetTimeLocalL(parentStartTime.TimeLocalL() + TTimeIntervalDays(1));
	TRAP(err, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(parentLocalUid, exceptedByChildInstanceTime));
	test(err == KErrNotFound);
	
	test.Printf(_L("try to fetch an instance using the wrong local uid "));
	TRAPD(wrongLocalUidError, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(parentLocalUid + 2, parentStartTime));
	test(wrongLocalUidError == KErrNotFound);
	
	test.Printf(_L("try to fetch an instance of the parent using the wrong time"));
	TRAPD(wrongTimeError1, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(parentLocalUid, parentEndTime));
	test(wrongTimeError1 == KErrNotFound);
	
	test.Printf(_L("try to fetch an instance of the child using the wrong time"));
	TRAPD(wrongTimeError2, iTestLibrary->SynCGetInstanceViewL().FindInstanceL(childLocalUid, childEndTime));
	test(wrongTimeError2 == KErrNotFound);
	}
	
void CInstanceIteratorTestManager::OomTestL(TType aType, TInt aFailAt)
	{
	// print the oom status
	if (aFailAt % 10 == 0)
		{
		test.Printf(_L("oom status: %d %d"), aType, aFailAt);
		}
	
	TCalTime instanceTime1;
	instanceTime1.SetTimeLocalL(gOomParentStartTimeLocal + TTimeIntervalDays(0));
	delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomParentLocalUid, instanceTime1);
	
	TCalTime instanceTime2;
	instanceTime2.SetTimeLocalL(gOomParentStartTimeLocal + TTimeIntervalDays(2));
	delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomParentLocalUid, instanceTime2);
	
	TCalTime instanceTime3;
	instanceTime3.SetTimeLocalL(gOomParentStartTimeLocal + TTimeIntervalDays(4));
	delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomParentLocalUid, instanceTime3);
	
	TCalTime parentRDateTime;
	parentRDateTime.SetTimeLocalL(gOomParentRDateTimeLocal);
	delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomParentLocalUid, parentRDateTime);
	
	TCalTime childStartTime;
	childStartTime.SetTimeLocalL(gOomChildStartTimeLocal);
	delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomChildLocalUid, childStartTime);
	
	
	TCalTime parentExDateTime;
	parentExDateTime.SetTimeLocalL(gOomParentExDateTimeLocal);
	TRAPD(err, delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomParentLocalUid, parentExDateTime) );

	TCalTime childRecIdTime;
	childRecIdTime.SetTimeLocalL(gOomChildRecIdTimeLocal);
	TRAP(err, delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomParentLocalUid, childRecIdTime));
	
	TCalTime parentStartTime;
	parentStartTime.SetTimeLocalL(gOomParentStartTimeLocal);	
	TRAP(err, delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomParentLocalUid + 2, parentStartTime));
	
	TCalTime parentEndTime;
	parentEndTime.SetTimeLocalL(gOomParentEndTimeLocal);
	TRAP(err, delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomParentLocalUid, parentEndTime));
	
	TCalTime childEndTime;
	childEndTime.SetTimeLocalL(gOomChildEndTimeLocal);
	TRAP(err, delete iTestLibrary->SynCGetInstanceViewL().FindInstanceL(gOomChildLocalUid, childEndTime));
	}	
	
void CInstanceIteratorTestManager::OomTestL()
	{
	iTestLibrary->CleanDatabaseL();
	
	// set up the times
	TCalTime parentStartTime;
	parentStartTime.SetTimeLocalL(gOomParentStartTimeLocal);
	TCalTime parentEndTime;
	parentEndTime.SetTimeLocalL(gOomParentEndTimeLocal);
	TCalTime parentRDateTime;
	parentRDateTime.SetTimeLocalL(gOomParentRDateTimeLocal);
	TCalTime parentExDateTime;
	parentExDateTime.SetTimeLocalL(gOomParentExDateTimeLocal);

	// the child entry times
	TCalTime childStartTime;
	childStartTime.SetTimeLocalL(gOomChildStartTimeLocal);
	TCalTime childEndTime;
	childEndTime.SetTimeLocalL(gOomChildEndTimeLocal);
	TCalTime childRecIdTime;
	childRecIdTime.SetTimeLocalL(gOomChildRecIdTimeLocal);
	
	// create a repeating event
	test.Printf(_L("create a repeating event"));
	HBufC8* guid = KOomGuid().AllocLC();
	CCalEntry* repeatingEvent = iTestLibrary->CreateCalEntryL(CCalEntry::EEvent, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(repeatingEvent);
	
	repeatingEvent->SetStartAndEndTimeL(parentStartTime, parentEndTime);
	
	// create the repeat rule
	TCalRRule repeatingEventRRule(TCalRRule::EDaily);
	repeatingEventRRule.SetDtStart(parentStartTime);
	repeatingEventRRule.SetInterval(1);
	repeatingEventRRule.SetCount(5);
	repeatingEvent->SetRRuleL(repeatingEventRRule);
	
	// create a sporadic date
	RArray<TCalTime> repeatingEventRDates;
	CleanupClosePushL(repeatingEventRDates);
	repeatingEventRDates.AppendL(parentRDateTime);
	repeatingEvent->SetRDatesL(repeatingEventRDates);
	CleanupStack::PopAndDestroy(&repeatingEventRDates);
	
	// create a exception date
	RArray<TCalTime> repeatingEventExDates;
	CleanupClosePushL(repeatingEventExDates);
	repeatingEventExDates.AppendL(parentExDateTime);
	repeatingEvent->SetExceptionDatesL(repeatingEventExDates);
	CleanupStack::PopAndDestroy(&repeatingEventExDates);
	
	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*repeatingEvent);
	gOomParentLocalUid = repeatingEvent->LocalUidL();
	CleanupStack::PopAndDestroy(repeatingEvent);
	
	// create a child to the repeating event
	guid = KOomGuid().AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0, childRecIdTime, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(childEntry);
	
	childEntry->SetStartAndEndTimeL(childStartTime, childEndTime);

	// store the entry
	test.Printf(_L("store the entry"));
	iTestLibrary->StoreEntryL(*childEntry);
	gOomChildLocalUid = childEntry->LocalUidL();
	CleanupStack::PopAndDestroy(childEntry);
	
	// test
	CalTestOom::OutOfMemoryTestL(*this, MCalTestOomCallBack::EClient, iTestLibrary->GetSession());
	CalTestOom::OutOfMemoryTestL(*this, MCalTestOomCallBack::EServer, iTestLibrary->GetSession());
	}
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


static void DoTestL()
	{
	CInstanceIteratorTestManager* testManager = CInstanceIteratorTestManager::NewLC();
	
	// Fetch all different kinds of instances:
	
	test.Next(_L("TestFetchCompletedTodoInstanceL"));

	testManager->TestFetchCompletedTodoInstanceL();
	
	test.Next(_L("TestFetchUndatedTodoInstanceL"));

	testManager->TestFetchUndatedTodoInstanceL();
	
	test.Next(_L("TestFetchDatedTodoInstanceL"));

	testManager->TestFetchDatedTodoInstanceL();
	
	test.Next(_L("TestFetchEventInstanceL"));

	testManager->TestFetchEventInstanceL();
	
	test.Next(_L("TestFetchRepeatingTodoInstanceL"));

	testManager->TestFetchRepeatingTodoInstanceL();
	
	test.Next(_L("TestFetchRepeatingEventAndChildInstanceL"));

	testManager->TestFetchRepeatingEventAndChildInstanceL();

	test.Next(_L("TestFindInstanceEndingBehaviour"));

	testManager->TestFindInstanceEndingBehaviour();
	
	// oom test

	test.Next(_L("omTestL"));

	testManager->OomTestL();
	
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-FETCHINSTANCE-0001

*/


TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-FETCHINSTANCE-0001 tcal_fetchinstance"));

	test.Title();

	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;

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
