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

#include <e32std.h>
#include <e32test.h>
#include <asclisession.h>
#include <asshdalarm.h>
#include <tz.h>
#include <calalarm.h>
#include <calentry.h>
#include <calentryview.h>
#include <calprogresscallback.h>
#include <calsession.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caliterator.h>


RTest test(_L("tcal_recur"));

_LIT(KCalendarFile, "tcal_recur");
_LIT8(KGUID, "A_Global_UID");

//useful for debugging purposes, allows you to view the instances of an entry
// Timezone constants

_LIT8(KEuropeBerlin, "Europe/Berlin");

class CTestApp : public CBase
	{
public:
	CTestApp();
	~CTestApp();

	static CTestApp *NewLC() ;
	void ConstructL();
	
	void RunTestL();
	void TestRecurrenceIdPanicL();	
	void ClearAllAlarmsL() ;
    void RunRecurrenceTestL();
    void RecurrenceTestL(TDateTime aParentStartTime, TDateTime aParentEndTime, TDateTime aChildStartTime, TDateTime aChildEndTime, TDateTime aRecurrenceId);

	void ChildRecurrenceIdTestL();
	void ClearDatabaseL();
	void CheckRRangeL( RArray<CalCommon::TRecurrenceRange> &aRanges );
	void CheckWithFindInstanceL(TCalTime& aStartRange,TCalTime& aEndRange,CalCommon::TCalViewFilter aFilter,TInt aExpectedInstances);
	TInt CreateRepeatingChildL(TCalTime& aChildRptStart,TCalTime& aChildRptEnd,TCalTime& aRecurId, CalCommon::TRecurrenceRange& aRange, TCalRRule::TType& aRType );
	void CreateParentL(TCalTime& aParentRptStart,TCalTime& aParentRptEnd, TCalRRule::TType& aRType );
	void TestRecurrenceRangeL();
	void MultipleChildsWithRRulesL();
	void TestRDatesForParentChildL();
	void TestSingleDayRepeatL();
	void VerifyInstancesAfterLeaveWhileAddingChildL();
	void MiscellaneousTestCasesL();


public:
	CCalTestLibrary*	iTestLibrary;	
	RTz iTzServer;
	
private:		
	RASCliSession iAlarmServer;
	CTzId*				iTzID;
	
	CCalEntryView*		iEntryView;	// not owned
	CCalInstanceView*	iInstanceView;// not owned
	};


CTestApp::CTestApp()
	{
	}


CTestApp::~CTestApp()
	{
 	if (iTzID && iTestLibrary)
		{
		TRAP_IGNORE(iTestLibrary->SetTimeZoneL(iTzID->TimeZoneNameID()));
		}

	iAlarmServer.Close();	
	iTzServer.Close();
	delete iTestLibrary;
	delete iTzID;
	}


void CTestApp::ClearAllAlarmsL()
	{
	// Delete all alarms
	RArray<TAlarmCategory> categories;
	iAlarmServer.GetAvailableCategoryListL(categories);
	TInt count = categories.Count();
	for(TInt i=0; i<count; i++)
		{
		const TAlarmCategory category = categories[i];
		test(iAlarmServer.AlarmDeleteAllByCategory(category, EFalse) == KErrNone);
		}
	categories.Close();

	// Restore alarm sound
	iAlarmServer.SetAlarmSoundState(EAlarmGlobalSoundStateOn);
	}


CTestApp* CTestApp::NewLC()
	{
	CTestApp* self = new (ELeave) CTestApp();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}


void CTestApp::ConstructL()
	{
	iTestLibrary = CCalTestLibrary::NewL();
	
	iTestLibrary->SetTimeZoneL(KEuropeBerlin);
	
	iTestLibrary->ReplaceFileL(KCalendarFile());
	iTestLibrary->OpenFileL(KCalendarFile());

	iInstanceView = &iTestLibrary->SynCGetInstanceViewL();
	iEntryView = &iTestLibrary->SynCGetEntryViewL();

	User::LeaveIfError(iTzServer.Connect());
	iTzID = iTzServer.GetTimeZoneIdL();
	
	// Connect to the alarm server
	User::LeaveIfError(iAlarmServer.Connect());
	
	// Make sure that the alarm queue is empty.
	ClearAllAlarmsL();
	}


void CTestApp::RunTestL()
	{
// create repeating parent entry	

	HBufC8* guid = KGUID().AllocLC();
	CCalEntry *entry = CCalEntry::NewL(CCalEntry::EReminder, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);	
	CleanupStack::PushL(entry);
	

	TCalTime startTime;
	TCalTime endTime;	
	startTime.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 8, 10, 1, 0, 0))) ;
	endTime.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 14, 10, 0, 0, 0))) ;		
			
	entry->SetStartAndEndTimeL(startTime, endTime) ;


	TCalRRule rrule(TCalRRule::EDaily);
	rrule.SetDtStart(startTime);
	rrule.SetUntil(endTime);
	rrule.SetInterval(1);	
	entry->SetRRuleL(rrule);				

	CCalAlarm *alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);			
	alarm->SetTimeOffset(TTimeIntervalMinutes(0)) ;
	entry->SetAlarmL(alarm);
	
	RPointerArray<CCalEntry> entryarr;
	CleanupClosePushL(entryarr);
	
	entryarr.AppendL(entry);
	
	TInt num = 0;
	iEntryView->StoreL(entryarr, num);
	test(entryarr.Count() == num);
	
	CleanupStack::PopAndDestroy(&entryarr);
	CleanupStack::PopAndDestroy(alarm);	
	CleanupStack::PopAndDestroy(entry);	
		
		
// Create child entry with exception

		
	TTime ttime1(TDateTime(2006, EJanuary, 9, 10, 1, 0, 0));
	TCalTime recId;
	recId.SetTimeUtcL(ttime1); // creating recurrence id


	HBufC8* guid1 = KGUID().AllocLC();
	CCalEntry *entry1 = CCalEntry::NewL(CCalEntry::EReminder, guid1, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid1);			
	CleanupStack::PushL(entry1);	

	TCalTime startTime1;
	TCalTime endTime1;	
	startTime1.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 9, 10, 2, 0, 0)));
	endTime1.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 11, 10, 0, 0, 0)));		
	entry1->SetStartAndEndTimeL(startTime1, endTime1);
	
	CCalAlarm* alarm1 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm1) ;			
	alarm1->SetTimeOffset(TTimeIntervalMinutes(0));
	entry1->SetAlarmL(alarm1);
	
		
	RPointerArray<CCalEntry> entryarr1;	
	CleanupClosePushL(entryarr1);
	
	entryarr1.AppendL(entry1);	
	
	iEntryView->StoreL(entryarr1, num);
	test(entryarr1.Count() == num);
	
	CleanupStack::PopAndDestroy(&entryarr1);
	CleanupStack::PopAndDestroy(alarm1);
	CleanupStack::PopAndDestroy(entry1);	
	
	// for debugging
	RPointerArray<CCalInstance> ins_arr;
	CleanupResetAndDestroyPushL(ins_arr);
	
	CalCommon::TCalTimeRange trange(startTime, endTime);
	
	iInstanceView->FindInstanceL(ins_arr, CalCommon::EIncludeAll, trange);
	
	const TInt KCount = ins_arr.Count();
	
	for(int i = 0; i < KCount; i++)
		{
		CCalEntry &ent = ins_arr[i]->Entry();
		TCalTime tt = ent.RecurrenceIdL();		
		}		
	
	CleanupStack::PopAndDestroy(&ins_arr);
	
	// now check what we have got in alarm servers
	for(int i = 0; i < 2; i++)
		{	
		TTime time(TDateTime(2006, EJanuary, 8 + i, 10, 0, 0, 0)); // how to set recurrance rule?
		User::SetUTCTime(time);	
		User::After(1000000);
		
		TAlarmId id;
		iAlarmServer.GetNextDueAlarmId(id);	
	
		HBufC8* alarmData = NULL;
		TInt err = iAlarmServer.GetAlarmData(id, alarmData);

		if( (err != KErrNone) || (!alarmData))
    		{
    		User::Leave(-1);
    		}

		CleanupStack::PushL( alarmData );
	
		CCalEntryId* entryId = CCalEntryId::NewL( alarmData );
		CleanupStack::Pop(alarmData); 
		CleanupStack::PushL(entryId);

		TPtrC8 id1 = entryId->IdL();
		TPtrC name = entryId->StoreFileNameL();
	
		TTime recurrenceId = entryId->RecurrenceIdL().TimeUtcL(); 
		TTime instanceTime = entryId->InstanceTimeL().TimeLocalL(); // OK, has start time of the child
		
		if (i == 0)
			{
			test(recurrenceId == Time::NullTTime()); // for the parent
			}
		else if (i == 1)
			{
			test(recurrenceId != Time::NullTTime()); // for one child
			}
		
		CleanupStack::PopAndDestroy(entryId);	
		}
	}

void CTestApp::TestRecurrenceIdPanicL()
	{
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);
    
	// Create an entry
	_LIT8(KUid, "12345");
	CCalEntry *entry = CCalEntry::NewL(CCalEntry::EAppt, KUid().AllocLC(), CCalEntry::EMethodRequest, 0);
	CleanupStack::Pop(); // uid
	CleanupStack::PushL(entry);

	TDateTime dtStart, dtEnd;
	dtStart.Set(2006, EApril, 19, 15, 30, 0, 0);
	dtEnd.Set(2006, EApril, 19, 15, 40, 0, 0);
	TCalTime startTime,endTime;
	startTime.SetTimeUtcL(TTime(dtStart));
	endTime.SetTimeUtcL(TTime(dtEnd));
	entry->SetStartAndEndTimeL(startTime, endTime);
	entry->SetSummaryL(_L("summary"));

	//Add a daily repeat
	TCalRRule rule;
	rule.SetType(TCalRRule::EDaily);
	rule.SetDtStart(startTime);
	rule.SetCount(10);

	entry->SetRRuleL(rule);

	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	
	    //Create an exception
	CCalEntry *child = CCalEntry::NewL(entry->EntryTypeL(),
	                                               entry->UidL().AllocL(),
	                                               entry->MethodL(),
	                                               entry->SequenceNumberL(),
	                                               startTime,
	                                               CalCommon::EThisOnly);
	CleanupStack::PushL(child);

	child->SetSummaryL(_L("EXEX"));
	child->SetLocationL(_L("EXEX"));
	child->SetStartAndEndTimeL(startTime, endTime);

	entries.AppendL(child);
	CleanupStack::Pop(child);
    
    TInt success = 0;
    iTestLibrary->SynCGetEntryViewL().StoreL(entries, success);
    entries.ResetAndDestroy();

    // Refetch entries from db
    iTestLibrary->SynCGetEntryViewL().FetchL(KUid, entries);
        
    TInt entryCount = entries.Count();
    
    ASSERT(entryCount == 2);
    CCalEntry* ex = entries[1];    //0 = parent, 1 = child
    
	HBufC8* guid = ex->UidL().AllocLC();
	CCalEntry* copy = CCalEntry::NewL( ex->EntryTypeL(),
		                            guid,
		                            ex->MethodL(),
		                            ex->SequenceNumberL(),
		                            ex->RecurrenceIdL(),
		                            ex->RecurrenceRangeL() );
	CleanupStack::Pop( guid ); // ownership was passed 
	CleanupStack::PushL( copy );
	copy->CopyFromL( *ex );
    
    //Access the recurrence id fields
    TCalTime exTime1 = ex->RecurrenceIdL();
    TCalTime cpTime1 = copy->RecurrenceIdL();
    
    //Delete all from db
    iTestLibrary->CleanDatabaseL();
    
    //Access the recurrence id fields again
    TCalTime exTime2 = ex->RecurrenceIdL();
    TCalTime cpTime2 = copy->RecurrenceIdL();   //Causes KERN-EXEC-3

	CleanupStack::PopAndDestroy( copy );
    CleanupStack::PopAndDestroy(&entries);
	}

/**
@SYMTestCaseID  PIM-CALENDAR-RECUR-0001
@SYMDEF DEF078776
@SYMTestType UT
@SYMTestPriority Critical
@SYMDEF DEF078776
@SYMTestCaseDesc Create a parent calendar entry with a child entry.

@SYMTestActions
1. Tests a parent entry and child entry with a standard valid recurrence ID with date and time
2. Tests a parent entry and child entry with a recurrence ID with date but no time
3. Tests a parent entry and child entry with a recurrence ID with a date and time. The time is set so it that is not within the parents occurence.
4. Tests a parent entry and child entry with a recurrence ID with a date that is not within the parent occurence.

@SYMTestExpectedResults For the above tests:
1. No leaves or panics should occur
2. No leaves or panics should occur
3. Should leave with KErrNotFound
4. Should leave with KErrNotFound
*/
void CTestApp::RunRecurrenceTestL()
    {
    // Test normal recurrenceId with date and time:
    TDateTime parentStartTime(2006, EJanuary, 8, 10, 0, 0, 0);
    TDateTime parentEndTime(2006, EJanuary, 14, 10, 0, 0, 0);
    TDateTime childStartTime(2006, EJanuary, 9, 10, 0, 0, 0);
    TDateTime childEndTime(2006, EJanuary, 11, 10, 0, 0, 0);
    TDateTime recurrenceId(2006, EJanuary, 9, 10, 0, 0, 0);
    RecurrenceTestL(parentStartTime, parentEndTime, childStartTime, childEndTime, recurrenceId);
   
    // Test recurrenceId that has a date but no time:
    recurrenceId = TDateTime(2006, EJanuary, 9, 0, 0, 0, 0);
    RecurrenceTestL(parentStartTime, parentEndTime, childStartTime, childEndTime, recurrenceId);
   
    // Test recurrenceId that has a date but no time but is not within the same date as the parent occurence
    recurrenceId = TDateTime(2006, EJune, 9, 0, 0, 0, 0);
    TRAPD(ret, RecurrenceTestL(parentStartTime, parentEndTime, childStartTime, childEndTime, recurrenceId));
    test(ret == KErrNotFound); // ret should be KErrNotFound as the recurrenceId is not valid
   
    // Test recurrenceId that has a date and time which is not within the same date as the parent occurence
    recurrenceId = TDateTime(2006, EJanuary, 9, 10, 30, 0, 0);
    TRAP(ret, RecurrenceTestL(parentStartTime, parentEndTime, childStartTime, childEndTime, recurrenceId));
    test(ret == KErrNotFound); // ret should be KErrNotFound as the recurrenceId is not valid
    }


void CTestApp::RecurrenceTestL(TDateTime aParentStartTime, TDateTime aParentEndTime, TDateTime aChildStartTime, TDateTime aChildEndTime, TDateTime aRecurrenceId)
    {  
// Create repeating parent entry
    HBufC8* guid = KGUID().AllocLC();
    CCalEntry *entry = CCalEntry::NewL(CCalEntry::EReminder, guid, CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(guid);   
    CleanupStack::PushL(entry);
   

    TCalTime startTime;
    TCalTime endTime;
    startTime.SetTimeUtcL(TTime(aParentStartTime));
    endTime.SetTimeUtcL(TTime(aParentEndTime));
           
    entry->SetStartAndEndTimeL(startTime, endTime);

    TCalRRule rrule(TCalRRule::EDaily);
    rrule.SetDtStart(startTime);
    rrule.SetUntil(endTime);
    rrule.SetInterval(1);   
    entry->SetRRuleL(rrule);
   
    RPointerArray<CCalEntry> entryarr;
    CleanupClosePushL(entryarr);
    
    entryarr.AppendL(entry);

    TInt num = 0;
    iEntryView->StoreL(entryarr, num);
    test(entryarr.Count() == num);
    
    CleanupStack::PopAndDestroy(&entryarr);
    CleanupStack::PopAndDestroy(entry);
       
// Create child entry with recurrence Id
    TTime ttime1(aRecurrenceId);
    TCalTime recId;
    recId.SetTimeUtcL(ttime1); // creating recurrence id

    HBufC8* guid1 = KGUID().AllocLC();
    CCalEntry *entry1 = CCalEntry::NewL(CCalEntry::EReminder, guid1, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
    CleanupStack::Pop(guid1);           
    CleanupStack::PushL(entry1);

    TCalTime startTime1;
    TCalTime endTime1;   
    startTime1.SetTimeUtcL(TTime(aChildStartTime));
    endTime1.SetTimeUtcL(TTime(aChildEndTime));
    entry1->SetStartAndEndTimeL(startTime1, endTime1);
       
    RPointerArray<CCalEntry> entryarr1;
    CleanupClosePushL(entryarr1);
    
    entryarr1.AppendL(entry1);

    iEntryView->StoreL(entryarr1, num);
    test(entryarr1.Count() == num);    
    
    CleanupStack::PopAndDestroy(&entryarr1);
    CleanupStack::PopAndDestroy(entry1);
    }

/**
@SYMTestCaseID PIM-TCAL-RECUR-0002
@SYMDEF DEF080989
@SYMTestType UT
@SYMTestPriority Critical
@SYMDEF DEF080989
@SYMTestCaseDesc RecurrenceIdL() returns NullTTime for new CCalEntry

@SYMTestActions
1) create a  REPEATING Parent
2) create a Child Entry with a Recurrence Id which wud be an instance of the parent entry.
3) call ChildEntry->CopyFromL(Parent);
4) check for Recurrence ID using ChildEntry->RecurrenceIdL()

@SYMTestExpectedResults For the above tests:
Recurrence ID using ChildEntry->RecurrenceIdL() should not equal null
*/


void CTestApp::ChildRecurrenceIdTestL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-RECUR-0002 Child Recurrance Id test"));

	HBufC8 *uid = NULL;
	CCalEntry* entry = iTestLibrary->CreateCalEntryL(CCalEntry::EReminder, uid);
	CleanupStack::PushL( entry );
	
	TCalTime timeStart;
	TCalTime timeEnd;
	TCalTime entryEnd;
	timeStart.SetTimeUtcL( TDateTime(2006,EJanuary,2,3,4,5,6) );
	timeEnd.SetTimeUtcL( TDateTime(2006,EJanuary,10,3,4,5,6) );
	entryEnd.SetTimeUtcL( TDateTime(2006,EJanuary,2,4,4,5,6) );

	TCalRRule rule(TCalRRule::EDaily) ;
	rule.SetDtStart(timeStart);
	rule.SetUntil(timeEnd);
	rule.SetInterval(1);	
	
	entry->SetStartAndEndTimeL(timeStart,entryEnd);
	entry->SetRRuleL( rule );
		
	RPointerArray< CCalEntry > calEntryList;
	CleanupClosePushL( calEntryList );
	
	calEntryList.Append( entry );
	
	TInt many = 0;
	iEntryView->StoreL( calEntryList, many );
	test(calEntryList.Count() == many);
	
	// create repeating child entry
	TTime recId(TDateTime(2006, EJanuary, 5, 3, 4, 5, 6));
	TCalTime recIdCal;
	recIdCal.SetTimeLocalL(recId);
	
	HBufC8* guid = uid->AllocLC();
	CCalEntry* 	entry2 = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 1, recIdCal, CalCommon::EThisAndAll);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry2);
	entry2->CopyFromL(*entry);
	// this should give User 84 panic without the fix.
	test(entry2->RecurrenceIdL().TimeLocalL()!=Time::NullTTime()); 
	CleanupStack::PopAndDestroy(entry2);
	
	CleanupStack::PopAndDestroy(&calEntryList);
	CleanupStack::PopAndDestroy(entry);
	}


TInt CTestApp::CreateRepeatingChildL(TCalTime& aChildRptStart,TCalTime& aChildRptEnd,TCalTime& aRecurId, CalCommon::TRecurrenceRange& aRange, TCalRRule::TType& aRType)
	{ 
	test.Printf(_L("Creating Child Entry"));
	_LIT(KLocation,"Child Entry");
	TTime endTime(aChildRptStart.TimeLocalL() + TTimeIntervalHours(1)); 
	TCalTime childend; 
	childend.SetTimeLocalL(endTime);
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* child = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 1, aRecurId, aRange);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(child);
	child->SetLocationL(KLocation());
	child->SetStartAndEndTimeL(aChildRptStart, childend);
	TCalRRule rRule(aRType);
	rRule.SetDtStart(aChildRptStart);
	rRule.SetUntil(aChildRptEnd);
	child->SetRRuleL(rRule);
	
	RPointerArray<CCalEntry> childEntries;
	CleanupClosePushL(childEntries);

	childEntries.AppendL(child);
	
	TInt success = 0;
	TRAPD(errStore, iEntryView->StoreL(childEntries, success));
	//test(childEntries.Count() == success);
	
	CleanupStack::PopAndDestroy(&childEntries);
	CleanupStack::PopAndDestroy(child);
	
	if(errStore != KErrNone)
		{
		test.Printf(_L("Failed to Create Child Entry"));	
		}
		
	return errStore;	
	}
   	
void CTestApp::CreateParentL(TCalTime& aParentRptStart,TCalTime& aParentRptEnd, TCalRRule::TType& aRType)
	{ 
	test.Printf(_L("Creating Parent Entry"));	
	_LIT(KLocation,"Parent Entry");
	TTime endTime(aParentRptStart.TimeLocalL() + TTimeIntervalHours(1)); 
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* parent = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(parent);
	TCalTime endTimeCal;
	endTimeCal.SetTimeLocalL(endTime);
	parent->SetStartAndEndTimeL(aParentRptStart, endTimeCal);
	TCalRRule rRule(aRType);
	rRule.SetDtStart(aParentRptStart);
	rRule.SetUntil(aParentRptEnd);
	parent->SetRRuleL(rRule);
	parent->SetLocationL(KLocation());
	
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	
	entries.AppendL(parent);

	TInt success(0);
	iEntryView->StoreL(entries, success);
	test(entries.Count() == success);
	
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(parent);
	}



void CTestApp::TestRecurrenceRangeL()
	{
	test.Next(_L("Verifying recurrence ranges of child entries"));

	
	//Parent Entry Data  
	TTime startTime(TDateTime(2006, EJanuary, 0, 14, 0, 0, 0));
	TCalTime parRptStrt;
	TCalTime parRptEnd;
	parRptStrt.SetTimeLocalL(TDateTime(2006, EJanuary, 0, 14, 0, 0, 0));
	parRptEnd.SetTimeLocalL(TDateTime(2006, EApril, 19, 14, 0, 0, 0));

	TCalTime recurrenceId;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EMarch, 13, 14, 0, 0, 0));//RECURRENCE ID
	RArray<CalCommon::TRecurrenceRange> recurRanges;
	CleanupClosePushL(recurRanges);
	TCalRRule::TType rRType = TCalRRule::EDaily;
	ClearDatabaseL();
	CreateParentL(parRptStrt,parRptEnd,rRType); //Creating Parent Entry

	//Checking EThisAndPrior
	TTime childstart(TDateTime(2006, EMarch, 13, 16, 0, 0, 0));
	CalCommon::TRecurrenceRange rRange = CalCommon::EThisAndPrior;
	TCalTime chRptStrt;
	TCalTime chRptEnd;
	chRptStrt.SetTimeLocalL(TDateTime(2005, EDecember, 10, 10, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EMarch, 28, 16, 0, 0, 0));
	//Adding repeating child entry.
	TInt retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);
	test(retVal == KErrNone);
	recurRanges.AppendL(CalCommon::EThisOnly);
	recurRanges.AppendL(CalCommon::EThisAndPrior);
	CheckRRangeL(recurRanges);	//Checking Recurrence Range
	recurRanges.Reset();

	//Checking EThisAndFuture  
	ClearDatabaseL();
	CreateParentL(parRptStrt,parRptEnd,rRType);	//Creating Parent Entry 
	rRange = CalCommon::EThisAndFuture;
	chRptStrt.SetTimeLocalL(TDateTime(2006, EFebruary, 10, 10, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EApril, 22, 16, 0, 0, 0));
	//Adding repeating child entry.
	retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);
	recurRanges.AppendL(CalCommon::EThisOnly);
	recurRanges.AppendL(CalCommon::EThisAndFuture);
	CheckRRangeL(recurRanges);	//Checking Recurrence Range
	test(retVal == KErrNone);
	recurRanges.Reset();
	
	CleanupStack::PopAndDestroy(&recurRanges);
	}
  
void CTestApp::ClearDatabaseL()
	{
	CCalIter* iter = CCalIter::NewL(iTestLibrary->GetSession());
	CleanupStack::PushL(iter);
	TPtrC8 nextId(iter->FirstL());
	CDesC8ArrayFlat* idArray = new (ELeave) CDesC8ArrayFlat(8);
	CleanupStack::PushL(idArray);
	while (nextId != KNullDesC8())
		{
		idArray->AppendL(nextId);
		nextId.Set(iter->NextL());
		}
	iEntryView->DeleteL(*idArray);
	
	CleanupStack::PopAndDestroy(idArray);
	CleanupStack::PopAndDestroy(iter);
	} 

void CTestApp::CheckWithFindInstanceL(TCalTime& aStartRange,TCalTime& aEndRange,CalCommon::TCalViewFilter aFilter,TInt aExpectedInstances)
	{
	test.Printf(_L("Checking Instances"));

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);

	CalCommon::TCalTimeRange timeRange(aStartRange, aEndRange);
	iInstanceView->FindInstanceL(instances, aFilter, timeRange);
	
	const TInt KCount=instances.Count();
	for (TInt i = 0; i < KCount; i++)
		{
		TDateTime dt = instances[i]->Time().TimeLocalL().DateTime();
		RDebug::Print(_L("instance of %d at %d:%d on %d/%d/%d"), 
			instances[i]->Entry().LocalUidL(), dt.Hour(), dt.Minute(), dt.Day()+1, dt.Month()+1, dt.Year());
		}

	test.Printf(_L("no of instances found are %d"), KCount);
	test(aExpectedInstances == KCount);
	
	CleanupStack::PopAndDestroy(&instances);
	}

void CTestApp::CheckRRangeL( RArray< CalCommon::TRecurrenceRange > &aRanges )
	{ 
	test.Printf(_L("Checking Recurrence ranges"));

    CCalIter& iter = iTestLibrary->GetIterL();
    TPtrC8 uid(iter.FirstL());

	RPointerArray<CCalEntry> entryList;
	CleanupResetAndDestroyPushL(entryList);
	
	iEntryView->FetchL(uid, entryList);
	
	const TInt KEntryCount = entryList.Count();
	for(TInt count = 0; count < KEntryCount; ++count)	
		{
		switch(entryList[count]->RecurrenceRangeL())
		{ 
		case CalCommon::EThisAndFuture:
			{
			 test.Printf(_L("Recurrence Range found:: EThisAndFuture "));	
			}
		break;
		case CalCommon::EThisAndPrior:
			{
			 test.Printf(_L("Recurrence Range found:: EThisAndPrior "));	
			}
		break;
		case CalCommon::EThisOnly:
			{
			test.Printf(_L("Recurrence Range found:: EThisOnly "));	
			}
		break;
		}

		test(aRanges[count] == entryList[count]->RecurrenceRangeL());
		}
		
	test.Printf(_L("\n"));
	
	CleanupStack::PopAndDestroy(&entryList); 
	}
	


void CTestApp::MultipleChildsWithRRulesL()
	{
	test.Next(_L("Multiple repeating children for Parent"));

	TTime startTime(TDateTime(2006, EJanuary, 10, 16, 0, 0, 0));
	TCalTime parRptStrt;
	TCalTime parRptEnd;
	parRptStrt.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 16, 0, 0, 0));
	parRptEnd.SetTimeLocalL(TDateTime(2006, EMarch, 29, 16, 0, 0, 0));
	TCalRRule::TType rRType = TCalRRule::EDaily;

	ClearDatabaseL();
	CreateParentL(parRptStrt,parRptEnd,rRType);	//Parent
	TTime childstart(TDateTime(2006, EMarch, 13, 10, 0, 0, 0));
	CalCommon::TRecurrenceRange rRange = CalCommon::EThisAndPrior;
	TCalTime chRptStrt;
	TCalTime chRptEnd;
	TCalTime recurrenceId;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EMarch, 13, 16, 0, 0, 0));	//Recurrence ID
	chRptStrt.SetTimeLocalL(TDateTime(2005, EDecember, 10, 10, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EMarch, 10, 16, 0, 0, 0));
	TInt retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);
	test(retVal == KErrNone);
	//adding another repeating child entry-- should not be allowed.
	TTime childtime(TDateTime(2006, EMarch, 22, 19, 0, 0, 0));
	CalCommon::TRecurrenceRange recurRange = CalCommon::EThisAndFuture;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EMarch, 22, 16, 0, 0, 0));	//Recurrence ID
	chRptStrt.SetTimeLocalL(TDateTime(2006, EMarch, 23, 10, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EApril, 10, 16, 0, 0, 0));
	retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,recurRange,rRType);
	test(retVal == KErrNotSupported);
	}


void CTestApp::TestRDatesForParentChildL()
	{
	test.Next(_L("Add Child having only RDates to a repeating Parent"));

	TTime startTime(TDateTime(2006, EJanuary, 10, 16, 0, 0, 0));
	TCalTime minRange;
	TCalTime maxRange;
	minRange.SetTimeLocalL(TCalTime::MinTime());
	maxRange.SetTimeLocalL(TCalTime::MaxTime());

	TCalTime parRptStrt;
	TCalTime parRptEnd;
	parRptStrt.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 16, 0, 0, 0));
	parRptEnd.SetTimeLocalL(TDateTime(2006, EMarch, 29, 16, 0, 0, 0));
	TCalRRule::TType rRType = TCalRRule::EDaily;

	ClearDatabaseL();
	CreateParentL(parRptStrt,parRptEnd,rRType);	//Parent
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,79);
	_LIT(KLocation,"Child Entry");
	TCalTime childstrt;
	TCalTime childend;
	TCalTime recurrenceID;
	recurrenceID.SetTimeLocalL(TDateTime(2006, EJanuary, 20, 16, 0, 0, 0));
	childstrt.SetTimeLocalL(TDateTime(2006, EJanuary, 20, 18, 0, 0, 0));
	childend.SetTimeLocalL(TDateTime(2006, EJanuary, 20, 19, 0, 0, 0));
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* child = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 1, recurrenceID, CalCommon::EThisAndFuture);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(child);
	child->SetLocationL(KLocation());
	child->SetStartAndEndTimeL(childstrt, childend);
	TCalTime rDate;
	RArray<TCalTime> rDateArray;
	CleanupClosePushL(rDateArray);
	rDate.SetTimeLocalL(TDateTime(2006, EJanuary, 22, 18, 0, 0, 0));
	rDateArray.Append(rDate);
	rDate.SetTimeLocalL(TDateTime(2006, EFebruary, 20, 18, 0, 0, 0));
	rDateArray.Append(rDate);
	rDate.SetTimeLocalL(TDateTime(2006, EMarch, 20, 18, 0, 0, 0)); 
	rDateArray.Append(rDate);
	child->SetRDatesL(rDateArray);
	
	RPointerArray<CCalEntry> childEntries;
	CleanupClosePushL(childEntries);
	
	childEntries.AppendL(child);
	test.Printf(_L("Storing Child with only RDates"));
	
	TInt success = 0;
	TRAPD(errStore,iEntryView->StoreL(childEntries, success));
	test(errStore == KErrNone);	//Rdates of child are reffered if child has no repeat rule.
								//and first of them is taken as the repeat start date.
	test(childEntries.Count() == success);

	CleanupStack::PopAndDestroy(&childEntries);
	rDateArray.Reset();
	CleanupStack::PopAndDestroy(&rDateArray);
	CleanupStack::PopAndDestroy(child);	
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,14);	// 10 of parent,4 of child.

	test.Next(_L("Add Repeating Child to Parent having only RDates"));

	ClearDatabaseL();
	_LIT(KParent,"Parent Entry");
	guid = KGUID().AllocLC();
	CCalEntry* parent = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(parent);
	TCalTime startTimeCal;
	TCalTime endTimeCal;
	startTimeCal.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 15, 0, 0, 0));
	endTimeCal.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 17, 0, 0, 0));
	parent->SetStartAndEndTimeL(startTimeCal, endTimeCal);
	parent->SetLocationL(KParent());
	CleanupClosePushL(rDateArray);
	rDate.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 15, 0, 0, 0));
	rDateArray.Append(rDate);
	rDate.SetTimeLocalL(TDateTime(2006, EJanuary, 24, 18, 0, 0, 0));
	rDateArray.Append(rDate);
	rDate.SetTimeLocalL(TDateTime(2006, EFebruary, 05, 10, 0, 0, 0)); 
	rDateArray.Append(rDate);
	rDate.SetTimeLocalL(TDateTime(2006, EFebruary, 18, 14, 0, 0, 0)); 
	rDateArray.Append(rDate);
	rDate.SetTimeLocalL(TDateTime(2006, EMarch, 02, 10, 0, 0, 0)); 
	rDateArray.Append(rDate);
	parent->SetRDatesL(rDateArray);
	
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	
	entries.AppendL(parent);
	test.Printf(_L("Creating Parent Entry"));
	success = 0;
	iEntryView->StoreL(entries, success);
	test(entries.Count() == success);
	
	CleanupStack::PopAndDestroy(&entries);
	
	rDateArray.Reset();
	CleanupStack::PopAndDestroy(&rDateArray);
	CleanupStack::PopAndDestroy(parent);
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,5); //5 RDates
	//adding repeating child entry
	CalCommon::TRecurrenceRange rRange = CalCommon::EThisAndFuture;
	TTime childtime(TDateTime(2006, EJanuary, 24, 20, 0, 0, 0));
	recurrenceID.SetTimeLocalL(TDateTime(2006, EFebruary, 05, 10, 0, 0,0));	//Recurrence ID
	TCalTime chRptStrt;
	TCalTime chRptEnd;
	chRptStrt.SetTimeLocalL(TDateTime(2006, EJanuary, 24, 20, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EMarch, 10, 20, 0, 0, 0));
	TInt retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceID,rRange,rRType);
	test(retVal == KErrNone);
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,48); //2 RDates + 46 child
	}



void CTestApp::TestSingleDayRepeatL()
	{
	test.Next(_L("Test Single Day Repeat"));

	test.Printf(_L("Adding repeating Child to single day repeating Parent"));	//Not Allowed
	TTime startTime(TDateTime(2006, EJanuary, 1, 9, 0, 0, 0));
	TCalTime parRptStrt;
	TCalTime parRptEnd;
	parRptStrt.SetTimeLocalL(TDateTime(2006, EJanuary, 1, 9, 0, 0, 0));
	parRptEnd.SetTimeLocalL(TDateTime(2006, EJanuary, 1, 15, 0, 0, 0));
	TCalRRule::TType rRType = TCalRRule::EDaily;
	ClearDatabaseL();
	CreateParentL(parRptStrt,parRptEnd,rRType);	//PARENT
	CalCommon::TRecurrenceRange rRange = CalCommon::EThisAndPrior;
	TCalTime recurrenceId;
	TCalTime chRptStrt;
	TCalTime chRptEnd;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EJanuary, 1, 9, 0, 0, 0));	//Recurrence ID
	chRptStrt.SetTimeLocalL(TDateTime(2005, EDecember, 10, 9, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2005, EDecember, 30, 9, 0, 0, 0));
	TInt retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);	//EThisAndPrior
	test(retVal == KErrArgument); 
	test.Printf(_L("Adding single day repeating Child to repeating Parent")); //Allowed
	ClearDatabaseL();
	TTime startParent(TDateTime(2006, EFebruary, 10, 9, 0, 0, 0));
	parRptStrt.SetTimeLocalL(TDateTime(2006, EFebruary,10,9,0,0,0));
	parRptEnd.SetTimeLocalL(TDateTime(2006, EMarch, 10,9,0,0,0));
	CreateParentL(parRptStrt,parRptEnd,rRType);//PARENT 
	TTime childStart(TDateTime(2006, EFebruary, 20, 1, 0, 0, 0));
	recurrenceId.SetTimeLocalL((TDateTime(2006, EFebruary, 20, 9, 0, 0, 0)));	//Recurrence ID
	chRptStrt.SetTimeLocalL(TDateTime(2006, EFebruary, 20, 1, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EFebruary, 20, 8, 0, 0, 0));
	retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);	//EThisAndPrior
	test(retVal == KErrNone);
	}



void CTestApp::VerifyInstancesAfterLeaveWhileAddingChildL()
	{
	test.Next(_L("Verifying if any new instances are added when a leave happens while adding a repeating child"));

	TCalTime minRange;
	TCalTime maxRange;
	minRange.SetTimeLocalL(TCalTime::MinTime());
	maxRange.SetTimeLocalL(TCalTime::MaxTime());
	//Parent Data  
	TTime startTime(TDateTime(2006, EJanuary, 10, 14, 0, 0, 0));
	TCalTime parRptStrt;
	TCalTime parRptEnd;
	parRptStrt.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 14, 0, 0, 0));
	parRptEnd.SetTimeLocalL(TDateTime(2006, EMarch, 10, 14, 0, 0, 0)); //a total of 60 instances.
	TCalTime recurrenceId;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EMarch, 10, 14, 0, 0, 0));	//RECURRENCE ID
	TCalRRule::TType rRType = TCalRRule::EDaily;
	ClearDatabaseL();
	CreateParentL(parRptStrt,parRptEnd,rRType);//PARENT
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,60);	//60 instances should be there

	//Leaving when recurrence range is EThisAndPrior
	TTime childstart(TDateTime(2006, EMarch, 10, 14, 0, 0, 0));
	CalCommon::TRecurrenceRange rRange = CalCommon::EThisAndPrior;
	TCalTime chRptStrt;
	TCalTime chRptEnd;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EMarch, 10, 14, 0, 0, 0));	//RECURRENCE ID
	chRptStrt.SetTimeLocalL(TDateTime(2005, EDecember, 10, 10, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EFebruary, 19, 16, 0, 0, 0));
	TInt retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);
	test(retVal == KErrArgument);
	//After leave, the instance should still be 60.
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,60); //60 instances should be there	

	//Leaving when recurrence range is EThisAndFuture
	recurrenceId.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 14, 0, 0, 0)); //RECURRENCE ID
	rRange = CalCommon::EThisAndFuture;
	chRptStrt.SetTimeLocalL(TDateTime(2005, EDecember, 10, 10, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EFebruary, 19, 16, 0, 0, 0));
	retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);
	test(retVal == KErrArgument);
	//After leave, the instance should still be 60.
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,60); //60 instances should be there	

	rRange = CalCommon::EThisAndAll;
	//Leaving when recurrence range is EThisAndAll
	retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);
	test(retVal == KErrArgument);
	//After leave, the instance should still be 60.
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,60);//60 instances should be there
	}



void CTestApp::MiscellaneousTestCasesL()
	{
	test.Next(_L("replace first instance with a non-repeating child and add repeating child which removes all other future instances."));

	TCalTime minRange;
	TCalTime maxRange;
	minRange.SetTimeLocalL(TCalTime::MinTime());
	maxRange.SetTimeLocalL(TCalTime::MaxTime());
	TTime startTime(TDateTime(2006, EJanuary, 10, 16, 0, 0, 0));
	TCalTime parRptStrt;
	TCalTime parRptEnd;
	parRptStrt.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 16, 0, 0, 0));
	parRptEnd.SetTimeLocalL(TDateTime(2006, EJanuary, 12, 16, 0, 0, 0));
	TCalRRule::TType rRType = TCalRRule::EDaily;
	ClearDatabaseL();
	CreateParentL(parRptStrt,parRptEnd,rRType); //Parent
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,3);

	_LIT(KLocation,"Child Entry");
	TCalTime childstrt;
	TCalTime childend;
	TCalTime recurrenceId;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 16, 0, 0, 0));
	childstrt.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 18, 0, 0, 0));
	childend.SetTimeLocalL(TDateTime(2006, EJanuary, 10, 19, 0, 0, 0));
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* child = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 1, recurrenceId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(child);
	child->SetLocationL(KLocation());
	child->SetStartAndEndTimeL(childstrt, childend);
	
	RPointerArray<CCalEntry> childEntries;
	CleanupClosePushL(childEntries);
	
	childEntries.AppendL(child);
	
	TInt success = 0;
	TRAPD(errStore,iEntryView->StoreL(childEntries, success));
	test(errStore == KErrNone);
	test(childEntries.Count() == success);

	CleanupStack::PopAndDestroy(&childEntries);
	CleanupStack::PopAndDestroy(child);
	
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,3);

	TTime childstart(TDateTime(2006, EJanuary, 11, 17, 0, 0, 0));
	CalCommon::TRecurrenceRange rRange = CalCommon::EThisAndFuture;
	TCalTime chRptStrt;
	TCalTime chRptEnd;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EJanuary, 11, 16, 0, 0, 0));  //RECURRENCE ID
	chRptStrt.SetTimeLocalL(TDateTime(2005, EDecember, 10, 10, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EFebruary, 19, 16, 0, 0, 0));
	TInt retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);
	test(retVal == KErrNone);
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,73);   //1+(21+31+20)=73

	test.Next(_L("replace last instance with a non-repeating child and add repeating child which removes all past instances."));

	ClearDatabaseL();
	CreateParentL(parRptStrt,parRptEnd,rRType);  //Parent
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,3);
	recurrenceId.SetTimeLocalL(TDateTime(2006, EJanuary, 12, 16, 0, 0, 0));
	childstrt.SetTimeLocalL(TDateTime(2006, EJanuary, 12, 18, 0, 0, 0));
	childend.SetTimeLocalL(TDateTime(2006, EJanuary, 12, 19, 0, 0, 0));
	guid = KGUID().AllocLC();
	child = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 1, recurrenceId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(child);
	child->SetLocationL(KLocation());
	child->SetStartAndEndTimeL(childstrt, childend);
	CleanupResetAndDestroyPushL(childEntries);
	success = 0;
	childEntries.AppendL(child);
	TRAPD(secStore,iEntryView->StoreL(childEntries, success));
	test(secStore == KErrNone);
	test(childEntries.Count() == success);
	CleanupStack::PopAndDestroy(&childEntries);
	CleanupStack::Pop(child);
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,3);
	TTime childStart(TDateTime(2006, EJanuary, 11, 18, 0, 0, 0));
	rRange = CalCommon::EThisAndPrior;
	recurrenceId.SetTimeLocalL(TDateTime(2006, EJanuary, 11, 16, 0, 0, 0));	//RECURRENCE ID
	chRptStrt.SetTimeLocalL(TDateTime(2005, EDecember, 10, 10, 0, 0, 0));
	chRptEnd.SetTimeLocalL(TDateTime(2006, EFebruary, 19, 16, 0, 0, 0));
	retVal = CreateRepeatingChildL(chRptStrt,chRptEnd,recurrenceId,rRange,rRType);
	test(retVal == KErrNone);
	CheckWithFindInstanceL(minRange,maxRange,CalCommon::EIncludeAppts,73);
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CTestApp* testManager = CTestApp::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	//set date to constant value to achieve consistent results
	TDateTime dtime(2006,EJanuary,2,3,4,5,6);
	TTime newtime(dtime);
	TTime oldtime; 
	oldtime.HomeTime();
	
	User::LeaveIfError( testManager->iTzServer.SetHomeTime(newtime) );
	
	testManager->RunRecurrenceTestL();
	testManager->RunTestL();
	testManager->TestRecurrenceIdPanicL();
	testManager->ChildRecurrenceIdTestL();
	testManager->TestRecurrenceRangeL();
	testManager->TestSingleDayRepeatL();
 	testManager->MultipleChildsWithRRulesL();
 	testManager->VerifyInstancesAfterLeaveWhileAddingChildL();
 	testManager->TestRDatesForParentChildL();
 	testManager->MiscellaneousTestCasesL();
		 	
	testManager->iTestLibrary->DeleteFileL(KCalendarFile, ETrue);
	User::LeaveIfError( testManager->iTzServer.SetHomeTime(oldtime) );//revert to current time
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-RECUR-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-RECUR-0001 Calendar Interim API Recurrence test suite"));

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
