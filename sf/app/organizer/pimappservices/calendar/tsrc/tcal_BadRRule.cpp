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

#include "tcal_BadRRule.h"

_LIT(KTestName,"tcal_BadRRule");
_LIT8(KGUID, "A_Global_UID");
_LIT8(KGUID1, "A_TODO_ENTRY");

static RTest test(KTestName);

static void CleanupCCalEntryArray(TAny* aPointer)
	{
	RPointerArray<CCalEntry>* calEntryArray = static_cast<RPointerArray<CCalEntry>*>(aPointer); 
	if (calEntryArray)
		{
		calEntryArray->ResetAndDestroy();
		}
	}


CBadRRule* CBadRRule::NewL(CCalTestLibrary* aTestLib)
	{
	CBadRRule* self = new (ELeave) CBadRRule();
	CleanupStack::PushL(self);
	self->iTestLib = aTestLib ;
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

	
void CBadRRule::ConstructL()
	{
	iTestLib->ReplaceFileL(KTestName);
	iTestLib->OpenFileL(KTestName);		
	}

	
CBadRRule::~CBadRRule()	
	{
	test.Close();
	}

void CBadRRule::CreateToDoEntryL()
{
	RPointerArray<CCalEntry> entryarr ;	
	CleanupResetAndDestroyPushL(entryarr);	

	
	TInt num ;

	HBufC8* guid = KGUID1().AllocLC(); // ownership of guid gets transferred
	CCalEntry *entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0) ;
	CleanupStack::Pop(guid);	
	CleanupStack::PushL(entry) ;

	TCalTime startTime ;
	TCalTime endTime ;	
	startTime.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 8, 10, 30, 0, 0))) ;
	endTime.SetTimeUtcL(startTime.TimeUtcL() + TTimeIntervalDays(1)) ;
			
	entry->SetStartAndEndTimeL(startTime, endTime) ;

	TCalTime endTime1 ;	
	endTime1.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 23, 10, 5, 0, 0))) ;		


	TCalRRule rrule(TCalRRule::EWeekly) ;
	
	rrule.SetDtStart(startTime);
	rrule.SetUntil(endTime1);
	rrule.SetInterval(1);	
	
	RArray<TDay> days;
	CleanupClosePushL(days);	
	days.AppendL(ETuesday);
	rrule.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);

	entry->SetRRuleL(rrule) ; 
	// both entry and rrule start time should be aligned to the first instance
	TCalRRule setRRule ;
	entry->GetRRuleL(setRRule) ;
	
	test(setRRule.DtStart().TimeUtcL() == rrule.DtStart().TimeUtcL()) ;
	entryarr.AppendL(entry) ;
	CleanupStack::Pop(entry);		

	iCalEntryView->StoreL(entryarr, num) ;	
	entryarr.ResetAndDestroy();

	TCalTime recId;
	recId.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 9, 10, 30, 0, 0))); // creating recurrence id

	guid = KGUID1().AllocLC(); // ownership of guid gets transferred
	entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid);			
	CleanupStack::PushL(entry) ;	

	TCalTime startTime3 ;
	TCalTime endTime3 ;	
	startTime3.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 7, 10, 2, 0, 0))) ;
	endTime3.SetTimeUtcL(startTime3.TimeUtcL() + TTimeIntervalHours(1)) ;
	entry->SetStartAndEndTimeL(startTime3, endTime3) ;
	
	entryarr.AppendL(entry) ;
	CleanupStack::Pop(entry) ;
	
	iCalEntryView->StoreL(entryarr, num) ;		
	CleanupStack::PopAndDestroy(&entryarr);
	
	RArray<TTime> insTimes ;

	insTimes.Append(TTime(TDateTime(2006, EJanuary, 7, 11, 2, 0, 0))) ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 16, 10, 30, 0, 0))) ;

	CleanupClosePushL(insTimes) ;
	CheckInstancesL(insTimes) ; 
	CleanupStack::PopAndDestroy(&insTimes) ;
			// time portion of repeat's end eliminates the third instance

}

void CBadRRule::CreateParentL()
{
	RPointerArray<CCalEntry> entryarr ;	
	CleanupResetAndDestroyPushL(entryarr);	

	TInt num ;

	HBufC8* guid = KGUID().AllocLC(); // ownership of guid gets transferred
	CCalEntry *entry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0) ;
	CleanupStack::Pop(guid);	
	CleanupStack::PushL(entry) ;

	TCalTime startTime ;
	TCalTime endTime ;	
	startTime.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 7, 10, 1, 0, 0))) ;
	endTime.SetTimeUtcL(startTime.TimeUtcL() + TTimeIntervalHours(1)) ;
			
	entry->SetStartAndEndTimeL(startTime, endTime) ;

	TCalTime endTime1;
	endTime1.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 23, 10, 5, 0, 0))) ;		


	TCalRRule rrule(TCalRRule::EWeekly) ;
	
	rrule.SetDtStart(startTime);
	rrule.SetUntil(endTime1);
	rrule.SetInterval(1);	
	
	RArray<TDay> days;
	CleanupClosePushL(days);	
	days.AppendL(ETuesday);
	rrule.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);

	entry->SetRRuleL(rrule) ; 
	TCalRRule setRRule ;
	entry->GetRRuleL(setRRule) ;
	
	test(setRRule.DtStart().TimeUtcL() == rrule.DtStart().TimeUtcL()) ;
	entryarr.AppendL(entry) ;
	CleanupStack::Pop(entry);		

	iCalEntryView->StoreL(entryarr, num) ;	
	entryarr.ResetAndDestroy() ;

	iCalEntryView->FetchL(KGUID(), entryarr);
	TCalRRule savedRRule ;
	entryarr[0]->GetRRuleL(savedRRule) ;
	
	TCalTime matchTime ; /* Jan 10, 2006 is the first Tuesday */
	matchTime.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 9, 10, 1, 0, 0))) ;
	//When the entry is stored, the repeat start time is adjusted so that it fits the rule (10th January is Tuesday
	test(savedRRule.DtStart().TimeUtcL() == matchTime.TimeUtcL()) ;
	test(entryarr[0]->StartTimeL().TimeUtcL() == (matchTime.TimeUtcL())) ;
	
	
	CleanupStack::PopAndDestroy(&entryarr) ;

	RArray<TTime> insTimes ;	
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 9, 10, 1, 0, 0))) ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 16, 10, 1, 0, 0))) ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 23, 10, 1, 0, 0))) ;
	
	CleanupClosePushL(insTimes) ;
	CheckInstancesL(insTimes) ;
	CleanupStack::PopAndDestroy(&insTimes) ;
}


void CBadRRule::CreateChild1L()
{
	TInt num ;
	
	TCalTime recId;

	recId.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 9, 10, 1, 0, 0))); // creating recurrence id

	HBufC8* guid1 = KGUID().AllocLC(); // ownership of guid1 gets transferred
	CCalEntry *entry1 = CCalEntry::NewL(CCalEntry::EEvent, guid1, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	CleanupStack::Pop(guid1);			
	CleanupStack::PushL(entry1) ;	

	TCalTime startTime3 ;
	TCalTime endTime3 ;	
	startTime3.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 7, 10, 2, 0, 0))) ;
	endTime3.SetTimeUtcL(startTime3.TimeUtcL() + TTimeIntervalHours(1)) ;
	entry1->SetStartAndEndTimeL(startTime3, endTime3) ;
	
	RPointerArray<CCalEntry> entryarr1 ;	
	entryarr1.AppendL(entry1) ;
	iCalEntryView->StoreL(entryarr1, num) ;		
	entryarr1.Close() ;	
	
	CleanupStack::PopAndDestroy(entry1) ;

}

void CBadRRule::CreateChild2L()
{
	TInt num ;
	
	TCalTime rec2 ;
	rec2.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 16, 10, 1, 0, 0))) ; //Jan 17

	HBufC8* guid_c2 = KGUID().AllocLC(); // ownership of guid1 gets transferred
	CCalEntry *entry_c2 = CCalEntry::NewL(CCalEntry::EEvent, guid_c2, CCalEntry::EMethodNone, 0, rec2, CalCommon::EThisAndAll);
	CleanupStack::Pop(guid_c2);
	CleanupStack::PushL(entry_c2) ;	

	TCalTime startTime_c2 ;
	TCalTime endTime_c2 ;	
	startTime_c2.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 8, 11, 2, 0, 0))) ;
	endTime_c2.SetTimeUtcL(startTime_c2.TimeUtcL() + TTimeIntervalHours(1)) ;
	entry_c2->SetStartAndEndTimeL(startTime_c2, endTime_c2) ;
	
	RPointerArray<CCalEntry> entryarr_c2 ;	
	entryarr_c2.AppendL(entry_c2) ;
	iCalEntryView->StoreL(entryarr_c2, num) ;		
	entryarr_c2.Close() ;
	
	CleanupStack::PopAndDestroy(entry_c2) ;
	
}


void CBadRRule::ModifyEntriesL()
{
	TInt num, err ;

	RPointerArray<CCalEntry> entryarr3 ;
	iCalEntryView->FetchL(KGUID(), entryarr3) ;
	test(entryarr3.Count() == 3) ;
		/* got 3 entries - EThisAndAll of second child do not have any special effect */	

	CleanupStack::PushL(TCleanupItem(CleanupCCalEntryArray, &entryarr3));

	TCalTime startTime4 ;
	TCalTime endTime4 ;	
	startTime4.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 1, 10, 1, 0, 0))) ;
	endTime4.SetTimeUtcL(startTime4.TimeUtcL() + TTimeIntervalHours(1)) ;
	
	entryarr3[0]->SetStartAndEndTimeL(startTime4, endTime4) ;
	
	//The repeating starting time will not be adjusted until the entry is stored
	test(entryarr3[0]->StartTimeL().TimeUtcL() == startTime4.TimeUtcL()) ;
	
	TCalRRule savedRRule ;
	entryarr3[0]->GetRRuleL(savedRRule) ;
	
	//The repeating starting time will not be adjusted until the entry is stored
	test(savedRRule.DtStart().TimeUtcL() == startTime4.TimeUtcL()) ;
	
	
	iCalEntryView->StoreL(entryarr3, num) ;	
			// store succeeds, because children are still aligned to an instance

	CleanupStack::PopAndDestroy(&entryarr3) ;

	RArray<TTime> insTimes ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 2, 10, 1, 0, 0))) ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 7, 10, 2, 0, 0))) ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 8, 11, 2, 0, 0))) ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 23, 10, 1, 0, 0))) ;
	
	CleanupClosePushL(insTimes) ;	
	CheckInstancesL(insTimes) ;
	CleanupStack::PopAndDestroy(&insTimes) ;

		// this is interesting - there could be 4 instance, but as the count value of the
		// repeat rule has been kept intact, we still get 3 instances, 
		// but they are shifted .		

	/* store is successful with modified time - now we shall change rrule */

	RPointerArray<CCalEntry> entryarr4 ;
	iCalEntryView->FetchL(KGUID(), entryarr4) ;
	test(entryarr4.Count() == 3) ;	

	CleanupStack::PushL(TCleanupItem(CleanupCCalEntryArray, &entryarr4));
	
	CCalEntry *entry = entryarr4[0] ;


	TCalTime startTime1 ;
	TCalTime endTime1 ;	
	TCalTime untilTime1 ;	

	startTime1.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 16, 10, 1, 0, 0))) ;
	endTime1.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 16, 11, 1, 0, 0))) ;
	untilTime1.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 26, 10, 5, 0, 0))) ;		


	TCalRRule rrule(TCalRRule::EWeekly) ;		

	rrule.SetDtStart(startTime1);
	rrule.SetUntil(untilTime1);
	rrule.SetInterval(1);	
	
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(ETuesday);  
	rrule.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);

	entry->SetStartAndEndTimeL(startTime1, endTime1);
	entry->SetRRuleL(rrule) ;
	
	TRAP(err, iCalEntryView->StoreL(entryarr4, num)) ;
	
	test(err == -1) ; // due to change in rrule store should fail
	test(num == 2) ; // only parent + child2 have been saved

	CleanupStack::PopAndDestroy(&entryarr4) ;
}


void CBadRRule::CheckInstancesL(RArray<TTime> &insTimes)
{
	RPointerArray<CCalInstance> instanceArray ;

	TCalTime startTime ;
	TCalTime endTime ;	
	startTime.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 1, 10, 1, 0, 0))) ;
	endTime.SetTimeUtcL(TTime(TDateTime(2006, EJanuary, 30, 10, 5, 0, 0))) ;		
	
	CalCommon::TCalTimeRange timeRange(startTime, endTime);
	
	CCalInstanceView &insView = iTestLib->SynCGetInstanceViewL();	
	
	insView.FindInstanceL(instanceArray, CalCommon::EIncludeAll, timeRange);		
	
	TInt count = instanceArray.Count() ;
	TInt count1 = insTimes.Count() ;
	
	test(count == count1) ;
	TInt i, j ;
	
	for(i = 0; i < count1; i++)
		{
		for(j = 0; j < count; j++)
			{
			if (instanceArray[j] == NULL)
				{
				continue ;
				}
			TTime t1 = instanceArray[j]->Time().TimeUtcL() ;		
			TDateTime d1 = t1.DateTime() ;	
			if (insTimes[i] == t1)
				{
				delete instanceArray[j] ;
				instanceArray[j] = NULL ;
				break ;
				}			
			}
			test(j < count) ;		
		}
	
	instanceArray.Close() ;		
}


void CBadRRule::DeleteEntriesL()
{
	
	RPointerArray<CCalEntry> entryarr5 ;
	iCalEntryView->FetchL(KGUID(), entryarr5) ;
	
	test(entryarr5.Count() == 2) ; 
		// one bad child entry could not be saved before

	CleanupStack::PushL(TCleanupItem(CleanupCCalEntryArray, &entryarr5));
	
	CCalEntry *entry2 = entryarr5[0] ; // get the parent
	
	iCalEntryView->DeleteL(*entry2) ;
		

	CleanupStack::PopAndDestroy(&entryarr5) ;
	
	/* now see if it has been deleted */

	RPointerArray<CCalEntry> entryarr6 ;
	iCalEntryView->FetchL(KGUID(), entryarr6) ;
	
	test(entryarr6.Count() == 0) ;
		
	entryarr6.Close() ;	
}



void CBadRRule::TestBadRRuleL()
{

	test.Printf(_L("TestBadRRule start\n"));

	CCalEntryView& view = iTestLib->SynCGetEntryViewL() ;
	iCalEntryView = &view ;

	
	CreateParentL() ;
	
		
	CreateChild2L() ;   // Create child entry with exception

	CreateChild1L() ;   // Creating a another child 	
	
	
	RArray<TTime> insTimes ;

	insTimes.Append(TTime(TDateTime(2006, EJanuary, 7, 10, 2, 0, 0))) ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 8, 11, 2, 0, 0))) ;
	insTimes.Append(TTime(TDateTime(2006, EJanuary, 23, 10, 1, 0, 0))) ;
	
	CleanupClosePushL(insTimes) ;	
	CheckInstancesL(insTimes) ;
	CleanupStack::PopAndDestroy(&insTimes) ;
	
	ModifyEntriesL() ;	// now modifying the parent again 			
	
	DeleteEntriesL() ;		// now try deleting entry 		

	CreateToDoEntryL() ;
	
	
	/* if executed to this point test has actully passed */
	
	test.Printf(_L("TestBadRRule successful\n"));
}

