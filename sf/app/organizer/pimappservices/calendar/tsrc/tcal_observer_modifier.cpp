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

#include "tcal_observer_modifier.h"

void ResetAndDestroyCalEntryArray(TAny*);

LOCAL_D const TInt KNumberBulkEntries(50);
LOCAL_D const TInt KNumberBulkEntriesToOverflowNotificationBuffer(51);

_LIT8(KGuid1, "guid");
_LIT8(KEntryGuid, "A_DUMMY_GUID");
_LIT(KSummary1, "Reminder Parent entry");
_LIT(KDescription1, "Source entry");
_LIT(KSummary2, "Reminder child entry with repeat rule");
_LIT(KDescription2, "Updating the Parent entry");

CTestAppModifier* CTestAppModifier::NewL()
	{
	CTestAppModifier* self = new(ELeave) CTestAppModifier;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTestAppModifier::ConstructL()
	{
	CTestApp::ConstructL();
	iCalTestLibrary = CCalTestLibrary::NewL();
	iCalTestLibrary->OpenFileL(KCalendarFile());
	
	iChunk.OpenGlobal(KChunkName(), EFalse);
	iNumExpectedChanges = reinterpret_cast<TInt*>(iChunk.Base());
	(*iNumExpectedChanges) = 0;
	iExpectedChanges = sizeof(TInt) + reinterpret_cast<TCalChangeEntry*>(iChunk.Base());
	}

CTestAppModifier::~CTestAppModifier()
	{
	delete iCalTestLibrary;
	}
	
void CTestAppModifier::ClearEntryL()
	{
	// this function deletes all entries with the UID used in this test harness
	for (TInt i(0) ; i < iNumEntriesToDelete ; ++i)
		{
		iExpectedChanges[i].iChangeType = MCalChangeCallBack2::EChangeDelete;
		}
	iNumEntriesToDelete = 0;
	
	iCalTestLibrary->AsynCGetEntryViewL().DeleteL(*iUidArray);
	
	delete iUidArray;
	iUidArray = NULL;
	}

CCalEntry* CTestAppModifier::CreateEntryLC(CCalEntry::TType aType, TTime aTimeLocal)
	{
	// create a new unique guid every time we create an entry   
	TBuf8<255> buf; // 255 is the Maximum guid length;
	buf.Num(iGuidNum++);
	HBufC8* guid = buf.AllocLC();
	
	if (!iUidArray)
		{
		iUidArray = new (ELeave) CDesC8ArraySeg(1000);
		}
	
	iUidArray->AppendL(*guid);
	
	CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	// Set the start and end time as passed to us
	TCalTime calTime1;
	TCalTime calTime2;
	calTime1.SetTimeLocalL(aTimeLocal);
	
	//for undated todo, both start and end time shud be NULL
	if(aTimeLocal != Time::NullTTime())
		{
		calTime2.SetTimeLocalL(aTimeLocal + TTimeIntervalHours(1));
		}
	else
		{
		calTime2.SetTimeLocalL(aTimeLocal);
		}

	entry->SetStartAndEndTimeL(calTime1, calTime2);
	return entry;
	}
	
void CTestAppModifier::UpdateEntriesL(TBool aInRange)
	{
	// update every entry the we have added to the store so far
	const TInt KUidCount(iUidArray->Count());
	for (TInt j(0) ; j < KUidCount ; ++j)
		{
		RPointerArray<CCalEntry> calEntries;
		CleanupResetAndDestroyPushL(calEntries);
		iCalTestLibrary->SynCGetEntryViewL().FetchL((*iUidArray)[j], calEntries);
		
		const TInt KEntryCount(calEntries.Count());
		for (TInt i(0) ; i < KEntryCount ; ++i)
			{
			calEntries[i]->SetLastModifiedDateL();
			if (aInRange)
				{
				iExpectedChanges[*iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeModify;
				iExpectedChanges[*iNumExpectedChanges].iEntryId = calEntries[i]->LocalUidL();
				++(*iNumExpectedChanges);	
				}
			}
			
		TInt numSucsessful;
		iCalTestLibrary->AsynCGetEntryViewL().UpdateL(calEntries, numSucsessful);
		
		CleanupStack::PopAndDestroy(); //calEntries.ResetAndDestroy();
		}	

	if(KUidCount>20)
		{//bulk operation
		*iNumExpectedChanges = 1;
		}

	}
	
void CTestAppModifier::StoreEntryLD(CCalEntry* aEntry, TBool aInRange)
	{
	StoreEntryLC(aEntry, aInRange);
	CleanupStack::PopAndDestroy(aEntry);
	}
	
void CTestAppModifier::StoreEntryLC(CCalEntry* aEntry, TBool aInRange)
	{
	CleanupStack::PushL(aEntry);
	
	RPointerArray<CCalEntry> entriesToAdd;
	CleanupClosePushL(entriesToAdd);
	
	entriesToAdd.AppendL(aEntry);
	
	if (aInRange)
		{
		iExpectedChanges[*iNumExpectedChanges].iChangeType = MCalChangeCallBack2::EChangeAdd;
		if (aEntry->EntryTypeL() == CCalEntry::ETodo)
			{
			iExpectedChanges[*iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
			}
		else
			{
			iExpectedChanges[*iNumExpectedChanges].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;	
			}
		iExpectedChanges[*iNumExpectedChanges].iEntryId = 0;
		++(*iNumExpectedChanges);
		++iNumEntriesToDelete;
		}
	
	TInt successfulAdd(0);
	iCalTestLibrary->SynCGetEntryViewL().StoreL(entriesToAdd, successfulAdd);
	
	CleanupStack::PopAndDestroy(); // entriesToAdd.Close()
	}

void CTestAppModifier::RunTestL()
	{
	while (*iTestCase < EAmount)
		{
		NextTestL();         // Prepare the test case
		iSemaphore.Signal(); // Done preparing: Report to the manager thread that it can continue now
		RThread().Suspend(); // Rest here until the manager thread has finished this test case
		}
	}

	
void CTestAppModifier::NextTestL()
	{		
	// Create some variables for use in the test cases
	TCalTime startBoundaryStartTime;
	startBoundaryStartTime.SetTimeLocalL(KRangeStartTime - TTimeIntervalDays(1));
	TCalTime startBoundaryEndTime;
	startBoundaryEndTime.SetTimeLocalL(KRangeStartTime - TTimeIntervalDays(1) + TTimeIntervalHours(1));

	TCalTime endBoundaryStartTime;
	endBoundaryStartTime.SetTimeLocalL(KRangeEndTime - TTimeIntervalDays(1));
	TCalTime endBoundaryEndTime;
	endBoundaryStartTime.SetTimeLocalL(KRangeEndTime - TTimeIntervalDays(1) + TTimeIntervalHours(1));
	
	TCalTime outsideStartTime;
	outsideStartTime.SetTimeLocalL(KRangeEndTime + TTimeIntervalDays(1));
	TCalTime outsideEndTime;
	outsideEndTime.SetTimeLocalL(KRangeEndTime + TTimeIntervalDays(1) + TTimeIntervalHours(1));
	
	TCalRRule rRuleStartBoundary(TCalRRule::EDaily);
	rRuleStartBoundary.SetDtStart(startBoundaryStartTime);
	rRuleStartBoundary.SetCount(5);
	rRuleStartBoundary.SetInterval(1);
	
	TCalRRule rRuleEndBoundary(TCalRRule::EDaily);
	rRuleEndBoundary.SetDtStart(endBoundaryStartTime);
	rRuleEndBoundary.SetCount(5);
	rRuleEndBoundary.SetInterval(1);
	
	TCalRRule rRuleOutside(TCalRRule::EDaily);
	rRuleOutside.SetDtStart(outsideStartTime);
	rRuleOutside.SetInterval(1);
	rRuleOutside.SetCount(5);
			
	TInt numSucsess;
	
	switch (*iTestCase)
		{
		case EEntryOutOfRange:
			{
			HBufC8* guid = KEntryGuid().AllocLC();
			CCalEntry* entry = CCalEntry::NewL(CCalEntry::EReminder, guid, CCalEntry::EMethodNone, 0);
			CleanupStack::Pop(guid);
			CleanupStack::PushL(entry);	
		 
			TTime startTime(TDateTime(1984, EJune, 9, 10, 0, 0, 0)); // 10 June 1984 10am
			TCalTime calTime1;
			calTime1.SetTimeLocalFloatingL(startTime);
			entry->SetStartAndEndTimeL(calTime1, calTime1); // end time is ignored for for reminder
			entry->SetSummaryL(KSummary1());
			entry->SetDescriptionL(KDescription1());

			TCalRRule rule(TCalRRule::EDaily);
			rule.SetDtStart(calTime1);
			rule.SetInterval(1);
			rule.SetCount(5);
			entry->SetRRuleL(rule);

			CleanupStack::Pop(entry);
			StoreEntryLD(entry,ETrue); // store the entry which is out of range
			*iNumExpectedChanges = 0;
			}
			break;
		case EEntryOutOfRange1:
			{
			HBufC8* guid2 = KEntryGuid().AllocLC();

			TTime recurId(TDateTime(1984, EJune, 11, 10, 0, 0, 0)); // 12 June 1984 10am
			TCalTime calRecurId;
			calRecurId.SetTimeLocalFloatingL(recurId);

			CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EReminder, guid2, CCalEntry::EMethodNone, 0, calRecurId, CalCommon::EThisAndFuture);
			
			CleanupStack::Pop(guid2);
			CleanupStack::PushL(childEntry);	

			TTime startTime2(TDateTime(1984, EJune, 15, 15, 0, 0, 0)); // 16 June 1984 3pm
			TTime endTime2(TDateTime(1984, EJune, 15, 16, 0, 0, 0)); // 16 June 1984 4pm
			
			TCalTime calStartTime2;
			calStartTime2.SetTimeLocalFloatingL(startTime2);
			TCalTime calEndTime2;
			calEndTime2.SetTimeLocalFloatingL(endTime2);
			childEntry->SetStartAndEndTimeL(calStartTime2, calEndTime2);
			
			childEntry->SetSummaryL(KSummary2);
			childEntry->SetDescriptionL(KDescription2);

			TCalRRule rule2(TCalRRule::EDaily);
			rule2.SetDtStart(calStartTime2);
			rule2.SetInterval(1);
			rule2.SetCount(5);
			childEntry->SetRRuleL(rule2);

			CleanupStack::Pop(childEntry);
			StoreEntryLD(childEntry, ETrue); // store the entry which is out of range
			*iNumExpectedChanges = 0;
			}
			break;
		case EApptInsideRange1:
			{
			// Add an appointment inside the range
			CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, ETrue);
			}
			break;
		case EApptInsideRange2:
			{
			UpdateEntriesL(ETrue);
			}
			break;
		case EApptInsideRange3:
			{
			*iNumExpectedChanges = 1;
			ClearEntryL();
			}
			break;	
		case EApptOutsideRange1:
			{
			// add appt outside time range
			CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime - TTimeIntervalMonths(3));
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, EFalse);
			}
			break;
		case EApptOutsideRange2:
			{
			UpdateEntriesL(EFalse);
			}
			break;
		case EApptOutsideRange3:
			{
			ClearEntryL();
			}
			break;

		case ENotifyNonResponsive:
			{
			// Disable notification then add a ToDo.
			iCalTestLibrary->GetSession().DisableChangeBroadcast();
			CCalEntry* entry = CreateEntryLC(CCalEntry::ETodo, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, EFalse);
			
			/* this client will be non responsive */
 			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeUndefined;
 			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryAll;
 			iExpectedChanges[0].iEntryId = 0;
//The timer which enbles the broadcast added in 9.3 is not implemented here
 			*iNumExpectedChanges = 0;
 			//*iNumExpectedChanges = 1;
//
			}
			break;
		case ENotifyNonResponsive1:
			{
//The timer which enbles the broadcast added in 9.3 is not implemented here
			*iNumExpectedChanges = 0;
			iNumEntriesToDelete = 0;
			//*iNumExpectedChanges = 1;
			//iNumEntriesToDelete = 1;
//			
			ClearEntryL();
			/* notification is still not enabled */
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeUndefined;				
			}
			break;

		case ETodoInsideRange1:
			{
			// Add a todo within the time range, but disable notification
			iCalTestLibrary->GetSession().DisableChangeBroadcast();
			CCalEntry* entry = CreateEntryLC(CCalEntry::ETodo, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, EFalse);
			}
			break;
		case ETodoInsideRange2:
			{
			// re-enable notification and set up the expected values 
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeUndefined;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryAll;
			iExpectedChanges[0].iEntryId = 0;
			*iNumExpectedChanges = 1;
			iCalTestLibrary->GetSession().EnableChangeBroadcast();
			}
			break;
		case ETodoInsideRange3:
			{
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
			UpdateEntriesL(ETrue);
			}
			break;
		case ETodoInsideRange4:
			{
			*iNumExpectedChanges = iNumEntriesToDelete = 1;
			ClearEntryL();
			}
			break;
		case ETodoOutsideRange1:
			{
			// Add a todo outside of the time range
			CCalEntry* entry = CreateEntryLC(CCalEntry::ETodo, KRangeEndTime + TTimeIntervalHours(4));
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, EFalse);
			}
			break;
		case ETodoOutsideRange2:
			{
			UpdateEntriesL(EFalse);
			}
			break;
			
		case ETodoOutsideRange3:
			{
			ClearEntryL();
			}
			break;
		case EAddUndatedTodo:
			{
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeAdd;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
			iExpectedChanges[0].iEntryId = 0;
			*iNumExpectedChanges = 0;

			CCalEntry* entry = CreateEntryLC(CCalEntry::ETodo, Time::NullTTime());
			entry->SetSummaryL(_L("original todo"));
			CleanupStack::Pop(entry);
			StoreEntryLC(entry, ETrue);
			iExpectedChanges[0].iEntryId = entry->LocalUidL();
			CleanupStack::PopAndDestroy(entry);
			}
			break;
		case EUpdateUndatedTodo:
			{
			//Create a modified todo with new GUID.
			CCalEntry* newEntry = CreateEntryLC(CCalEntry::ETodo, Time::NullTTime());
			iUidArray->Delete(0);
			newEntry->SetSummaryL(_L("Modified todo"));
			
			CCalEntry* entry = iCalTestLibrary->SynCGetEntryViewL().FetchL(iExpectedChanges[0].iEntryId);
			CleanupStack::PushL(entry);
			entry->CopyFromL(*newEntry);
			entry->SetLocalUidL(iExpectedChanges[0].iEntryId);
			CleanupStack::Pop(entry);
			CleanupStack::PopAndDestroy(newEntry);
			
			*iNumExpectedChanges = 1;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryTodo;
			StoreEntryLD(entry, EFalse);
			--iNumEntriesToDelete;
			}
			break;
		case EClearUndatedTodo:
			{	
			*iNumExpectedChanges = iNumEntriesToDelete = 1;
			ClearEntryL();
			}
			break;
		case EMultipleInAndOutside1:
			{
			// Add multiple entries in and outside range
			CCalEntry* todoOutRange = CreateEntryLC(CCalEntry::ETodo, KRangeEndTime + TTimeIntervalHours(4));
			CleanupStack::Pop(todoOutRange);
			StoreEntryLD(todoOutRange, EFalse);
			CCalEntry* apptOutOfRange = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime - TTimeIntervalMonths(3));
			CleanupStack::Pop(apptOutOfRange);
			StoreEntryLD(apptOutOfRange, EFalse);
			CCalEntry* apptInRange = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(apptInRange);
			StoreEntryLD(apptInRange);
			CCalEntry* todoInRange = CreateEntryLC(CCalEntry::ETodo, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(todoInRange);
			StoreEntryLD(todoInRange);
			}
			break;
		case EMultipleInAndOutside2:
			{	
			*iNumExpectedChanges = 2;
			ClearEntryL();
			}
			break;
		case EBulkAdd:
			{
			// add a lot of entries quickly to see if the notification can keep up
			for (TInt i(0) ; i < KNumberBulkEntries; ++i)
				{
				CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime + TTimeIntervalDays(5));
				CleanupStack::Pop(entry);
				StoreEntryLD(entry);
				}
			}
			break;
		case EBulkDelete:
			{
			// Delete all the entries in the bulk add
			*iNumExpectedChanges = 1;//bulk operation
			ClearEntryL();
			}
			break;
		case EBulkAddToOverflowNotificationBuffer:
			{
			// Add a lot of entries and wait for 5 seconds (in the manager) and overflow the notification buffer
			for (TInt i(0) ; i < KNumberBulkEntriesToOverflowNotificationBuffer; ++i)
				{
				CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime + TTimeIntervalDays(5));
				CleanupStack::Pop(entry);
				StoreEntryLD(entry);
				}
				
			// set up expected data
			*iNumExpectedChanges = 1;
			// Expecting one Undefined Change
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryAll;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeUndefined;
			iExpectedChanges[0].iEntryId = 0;
			}
			break;
		case EBulkDeleteToOverflowNotificationBuffer:
			{
			// Delete all the entries in the bulk add
			*iNumExpectedChanges = 1;
			iNumEntriesToDelete = KNumberBulkEntriesToOverflowNotificationBuffer;
			ClearEntryL();
			// Expecting one Undefined Change
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeUndefined;
			}
			break;
		case ERepeatingEitherSideOfRange1:
			{
			// Create an entry that has instances before and after the observation range
			CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime - TTimeIntervalDays(1));
			TCalRRule rRule(TCalRRule::EYearly);
			TCalTime calTime;
			calTime.SetTimeLocalL(KRangeStartTime - TTimeIntervalDays(1));
			rRule.SetDtStart(calTime);
			rRule.SetCount(2);
			rRule.SetInterval(2);
			entry->SetRRuleL(rRule);
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, EFalse);
			}
			break;	
		case ERepeatingEitherSideOfRange2:
			{
			UpdateEntriesL(EFalse);
			}
			break;
		case ERepeatingEitherSideOfRange3:
			{
			ClearEntryL();
			}
			break;
		case ERepeatingAcrossStartBoundary1:
			{
			// Create a repeating entry starting outside and finishing inside
			CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime - TTimeIntervalDays(1));
			entry->SetRRuleL(rRuleStartBoundary);
			CleanupStack::Pop(entry);
			StoreEntryLD(entry);
			}
			break;
		case ERepeatingAcrossStartBoundary2:
			{
			RPointerArray<CCalEntry> calEntryArray;
			CleanupResetAndDestroyPushL(calEntryArray);
			iCalTestLibrary->SynCGetEntryViewL().FetchL((*iUidArray)[iUidArray->Count() - 1], calEntryArray);
			// Move the repeating entry to outside the range
			calEntryArray[0]->SetStartAndEndTimeL(outsideStartTime, outsideEndTime);
			calEntryArray[0]->SetRRuleL(rRuleOutside);
	
			// set up expected data
			*iNumExpectedChanges = iNumEntriesToDelete = 1;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = calEntryArray[0]->LocalUidL();
			
			iCalTestLibrary->SynCGetEntryViewL().UpdateL(calEntryArray, numSucsess);
			CleanupStack::PopAndDestroy();//calEntryArray.ResetAndDestroy();
			}
			break;
		case ERepeatingAcrossStartBoundary3:
			{
			ClearEntryL();
			}
			break;
		case ERepeatingAcrossStartBoundary4:
			{
			// Create a repeating entry outside time range
			CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeEndTime + TTimeIntervalDays(5));
			entry->SetRRuleL(rRuleOutside);
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, EFalse);
			}
			break;
		case ERepeatingAcrossStartBoundary5:
			{
			// get the entry
			RPointerArray<CCalEntry> calEntryArray;
			CleanupResetAndDestroyPushL(calEntryArray);
			iCalTestLibrary->SynCGetEntryViewL().FetchL((*iUidArray)[iUidArray->Count() - 1], calEntryArray);
			// Move the repeating entry to inside the time range
			calEntryArray[0]->SetStartAndEndTimeL(startBoundaryStartTime, startBoundaryEndTime);
			calEntryArray[0]->SetRRuleL(rRuleStartBoundary);
	
			// set up expected data
			*iNumExpectedChanges = iNumEntriesToDelete = 1;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = calEntryArray[0]->LocalUidL();
	
			iCalTestLibrary->SynCGetEntryViewL().UpdateL(calEntryArray, numSucsess);
			CleanupStack::PopAndDestroy(); //calEntryArray.ResetAndDestroy();
			}
			break;
		case ERepeatingAcrossStartBoundary6:
			{
			*iNumExpectedChanges = 1;
			ClearEntryL();
			}
			break;
		case ERepeatingAcrossEndBoundary1:
			{
			// Create a repeating across the end boundary
			CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeEndTime - TTimeIntervalDays(1));
			entry->SetRRuleL(rRuleEndBoundary);
			CleanupStack::Pop(entry);
			StoreEntryLD(entry);
			}
			break;
		case ERepeatingAcrossEndBoundary2:
			{
			// get the entry
			RPointerArray<CCalEntry> calEntryArray;
			CleanupResetAndDestroyPushL(calEntryArray);
			iCalTestLibrary->SynCGetEntryViewL().FetchL((*iUidArray)[iUidArray->Count() - 1], calEntryArray);

			// Move the repeating entry to outside the time range
			calEntryArray[0]->SetStartAndEndTimeL(outsideStartTime, outsideEndTime);
			calEntryArray[0]->SetRRuleL(rRuleOutside);
	
			// set up expected data
			*iNumExpectedChanges = iNumEntriesToDelete = 1;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = calEntryArray[0]->LocalUidL();
	
			iCalTestLibrary->SynCGetEntryViewL().UpdateL(calEntryArray, numSucsess);
			CleanupStack::PopAndDestroy(); //calEntryArray.ResetAndDestroy();
			}
			break;
		case ERepeatingAcrossEndBoundary3:
			{
			*iNumExpectedChanges = 0;
			ClearEntryL();
			}
			break;
		case ERepeatingAcrossEndBoundary4:
			{
			// Create a repeating entry outside the time range
			CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeEndTime + TTimeIntervalDays(5));
			entry->SetRRuleL(rRuleOutside);
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, EFalse);
			}
			break;	
		case ERepeatingAcrossEndBoundary5:
			{
			RPointerArray<CCalEntry> calEntryArray;
			CleanupResetAndDestroyPushL(calEntryArray);
			iCalTestLibrary->SynCGetEntryViewL().FetchL((*iUidArray)[iUidArray->Count() - 1], calEntryArray);
			
			// Move the repeating entry to across the end boundary
			calEntryArray[0]->SetStartAndEndTimeL(endBoundaryStartTime, endBoundaryEndTime);
			calEntryArray[0]->SetRRuleL(rRuleEndBoundary);
	
			// set up expected data
			*iNumExpectedChanges = iNumEntriesToDelete = 1;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = calEntryArray[0]->LocalUidL();

			iCalTestLibrary->SynCGetEntryViewL().UpdateL(calEntryArray, numSucsess);
			CleanupStack::PopAndDestroy(); //calEntryArray.ResetAndDestroy();
			}
			break;
		case ERepeatingAcrossEndBoundary6:
			{
			*iNumExpectedChanges = 1;
			ClearEntryL();
			}
			break;
		case EFilterOnlyEventEntries1:
			{
			// do nothing, filter is changing
			*iNumExpectedChanges = 0;
			}
			break;
		case EFilterOnlyEventEntries2:
			{
			// Add todo entry, should not be notified
			CCalEntry* entryTodo = CreateEntryLC(CCalEntry::ETodo, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(entryTodo);
			StoreEntryLD(entryTodo, EFalse);
			*iNumExpectedChanges = 0;
			}
			break;
		case EFilterOnlyEventEntries3:
			{
			// Add event entry, should be notified
			CCalEntry* entryAppt = CreateEntryLC(CCalEntry::EEvent, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(entryAppt);
			StoreEntryLD(entryAppt, ETrue);	
			*iNumExpectedChanges = 1;
			}
			break;
		case EFilterOnlyEventEntries4:
			{
			*iNumExpectedChanges = 1;
			ClearEntryL();
			}
			break;
		case EFilterOnlyTodoEntries1:
			{
			// do nothing, filter is changing
			*iNumExpectedChanges = 0;
			}
			break;
		case EFilterOnlyTodoEntries2:
			{
			// Add todo entry, should be notified
			CCalEntry* entryTodo = CreateEntryLC(CCalEntry::ETodo, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(entryTodo);
			StoreEntryLD(entryTodo, ETrue);
			*iNumExpectedChanges = 1;
			}
			break;
		case EFilterOnlyTodoEntries3:
			{	
			// Add event entry, should not be notified
			CCalEntry* entryAppt = CreateEntryLC(CCalEntry::EEvent, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(entryAppt);
			StoreEntryLD(entryAppt, EFalse);	
			*iNumExpectedChanges = 0;
			}
			break;
		case EFilterOnlyTodoEntries4:
			{
			*iNumExpectedChanges = 1;
			ClearEntryL();
			}
			break;
		case EFilterOnlyTodoEntries5:
			{
			// do nothing, filter is changing
			*iNumExpectedChanges = 0;
			}
			break;
		case EAddAndUpdateParentAndChild1:
			{
			// add a parent entry
			HBufC8* guid = KGuid1().AllocL();
			CCalEntry* entryAppt = iCalTestLibrary->CreateCalEntryL(CCalEntry::EAppt, guid);
			CleanupStack::PushL(entryAppt);
			iCalTestLibrary->SetEntryStartAndEndTimeL(entryAppt, KRangeStartTime + TTimeIntervalDays(5), KRangeStartTime + TTimeIntervalDays(5) + TTimeIntervalHours(1));
			
			TCalRRule rRule(TCalRRule::EDaily);
			TCalTime rRuleDtStart;
			rRuleDtStart.SetTimeLocalL(KRangeStartTime + TTimeIntervalDays(5));
			rRule.SetDtStart(rRuleDtStart);
			rRule.SetInterval(1);
			rRule.SetCount(5);
			
			entryAppt->SetRRuleL(rRule);
			
			CleanupStack::Pop(entryAppt);
			StoreEntryLD(entryAppt, ETrue);
			}
			break;
		case EAddAndUpdateParentAndChild2:
			{
			//update the parent entry
			RPointerArray<CCalEntry> entryArray;
			CleanupResetAndDestroyPushL(entryArray);
			iCalTestLibrary->SynCGetEntryViewL().FetchL(KGuid1, entryArray);
			if (entryArray.Count() == 0)
				{
				RDebug::Print(_L("entry not found"));
				User::Leave(KErrNotFound);
				}
			entryArray[0]->SetSummaryL(_L("modified parent"));
			TInt numSuc;
			
			// we are expecting an update on the parent
			*iNumExpectedChanges = iNumEntriesToDelete = 1;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = entryArray[0]->LocalUidL();
			
			iCalTestLibrary->SynCGetEntryViewL().UpdateL(entryArray, numSuc);
			
			CleanupStack::PopAndDestroy(&entryArray);
			}
			break;
		case EAddAndUpdateParentAndChild3:
			{
			// add a child entry
			HBufC8* guid = KGuid1().AllocL();
			TCalTime recId;
			recId.SetTimeLocalL(KRangeStartTime + TTimeIntervalDays(5) + TTimeIntervalDays(4));
			CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
			childEntry->SetStartAndEndTimeL(recId, recId);
			
			// we are expecting an update on the parent
			// and an add for the child
			*iNumExpectedChanges = iNumEntriesToDelete = 2;
			iExpectedChanges[1].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[1].iChangeType = MCalChangeCallBack2::EChangeAdd;
			iExpectedChanges[1].iEntryId = childEntry->LocalUidL();
			
			StoreEntryLC(childEntry, EFalse);
			iExpectedChanges[1].iEntryId = childEntry->LocalUidL();
			CleanupStack::PopAndDestroy(childEntry);
			}
			break;
		case EAddAndUpdateParentAndChild4:
			{
			// update the child entry
			RPointerArray<CCalEntry> entryArray;
			CleanupResetAndDestroyPushL(entryArray);
			
			iCalTestLibrary->SynCGetEntryViewL().FetchL(KGuid1, entryArray);
			
			// we are expecting only a modification on the child
			*iNumExpectedChanges = iNumEntriesToDelete = 1;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = entryArray[1]->LocalUidL();
			
			delete entryArray[0];
			entryArray.Remove(0);
			
			entryArray[0]->SetSummaryL(_L("modified child"));
			
			StoreEntryLD(entryArray[0], EFalse);
			
			entryArray.Remove(0);
			
			CleanupStack::PopAndDestroy(&entryArray);
			}
			break;
		case EAddAndUpdateParentAndChild5:
			{
			// store the parent again
			RPointerArray<CCalEntry> entryArray;
			CleanupResetAndDestroyPushL(entryArray);
			
			iCalTestLibrary->SynCGetEntryViewL().FetchL(KGuid1, entryArray);
			
			// we are expecting a modification on the parent
			// and a delete on the child
			*iNumExpectedChanges = iNumEntriesToDelete = 2;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = entryArray[0]->LocalUidL();
			iExpectedChanges[1].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[1].iChangeType = MCalChangeCallBack2::EChangeDelete;
			iExpectedChanges[1].iEntryId = entryArray[1]->LocalUidL();
			
			delete entryArray[1];
			entryArray.Remove(1);
			
			TInt numSuc;
			
			entryArray[0]->SetSummaryL(_L("modified parent"));
			
			
			iCalTestLibrary->SynCGetEntryViewL().StoreL(entryArray, numSuc);
			
			CleanupStack::PopAndDestroy(&entryArray);
			}
			break;
		case EAddAndUpdateParentAndChild6:
			{
			// add another child
			HBufC8* guid = KGuid1().AllocL();
			TCalTime recId;
			recId.SetTimeLocalL(KRangeStartTime + TTimeIntervalDays(5) + TTimeIntervalDays(4));
			CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
			childEntry->SetStartAndEndTimeL(recId, recId);
			
			// we are expecting an add notification for the child
			// and an update for the parent
			*iNumExpectedChanges = iNumEntriesToDelete = 2;
			iExpectedChanges[1].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[1].iChangeType = MCalChangeCallBack2::EChangeAdd;
			
			StoreEntryLC(childEntry, EFalse);
			
			iExpectedChanges[1].iEntryId = childEntry->LocalUidL();
			CleanupStack::PopAndDestroy(childEntry);
			}
			break;
		case EAddAndUpdateParentAndChild7:
			{
			// delete the child
			RPointerArray<CCalEntry> entryArray;
			CleanupResetAndDestroyPushL(entryArray);
			
			iCalTestLibrary->SynCGetEntryViewL().FetchL(KGuid1, entryArray);
			
			// we are expecting only a delete notification for the child
			*iNumExpectedChanges = iNumEntriesToDelete = 2;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = entryArray[0]->LocalUidL();			
			iExpectedChanges[1].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[1].iChangeType = MCalChangeCallBack2::EChangeDelete;
			iExpectedChanges[1].iEntryId = entryArray[1]->LocalUidL();
				
			iCalTestLibrary->SynCGetEntryViewL().DeleteL(*entryArray[1]);
			
			CleanupStack::PopAndDestroy(&entryArray);
			}
			break;
		case EAddAndUpdateParentAndChild8:
			{
			// add another child
			HBufC8* guid = KGuid1().AllocL();
			TCalTime recId;
			recId.SetTimeLocalL(KRangeStartTime + TTimeIntervalDays(5) + TTimeIntervalDays(4));
			CCalEntry* childEntry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
			childEntry->SetStartAndEndTimeL(recId, recId);
			
			// fetch the parent so we can get the local uid
			RPointerArray<CCalEntry> entryArray;
			CleanupResetAndDestroyPushL(entryArray);
			iCalTestLibrary->SynCGetEntryViewL().FetchL(KGuid1, entryArray);
			
			// we are expecting an add notification for the child
			// and an update for the parent
			*iNumExpectedChanges = iNumEntriesToDelete = 2;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeModify;
			iExpectedChanges[0].iEntryId = entryArray[0]->LocalUidL();
			iExpectedChanges[1].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[1].iChangeType = MCalChangeCallBack2::EChangeAdd;
		
			CleanupStack::PopAndDestroy(&entryArray);
			
			StoreEntryLC(childEntry, EFalse);
			
			iExpectedChanges[1].iEntryId = childEntry->LocalUidL();
			CleanupStack::PopAndDestroy(childEntry);
			}
			break;
		case EAddAndUpdateParentAndChild9:
			{
			// delete the parent
			RPointerArray<CCalEntry> entryArray;
			CleanupResetAndDestroyPushL(entryArray);
			
			iCalTestLibrary->SynCGetEntryViewL().FetchL(KGuid1, entryArray);
			
			// we are expectiond a delete notification for both the parent and the child
			*iNumExpectedChanges = iNumEntriesToDelete = 2;
			iExpectedChanges[0].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[0].iChangeType = MCalChangeCallBack2::EChangeDelete;
			iExpectedChanges[0].iEntryId = entryArray[0]->LocalUidL();
			iExpectedChanges[1].iEntryType = MCalChangeCallBack2::EChangeEntryEvent;
			iExpectedChanges[1].iChangeType = MCalChangeCallBack2::EChangeDelete;
			iExpectedChanges[1].iEntryId = entryArray[1]->LocalUidL();

			
			iCalTestLibrary->SynCGetEntryViewL().DeleteL(*entryArray[0]);
			
			CleanupStack::PopAndDestroy(&entryArray);
			}
			break;

		case EChangesToOtherFile:
			{
			// Make changes to another file (should not be notified)
			iCalTestLibrary->ReplaceFileL(KCalendarFileOther);
			iCalTestLibrary->OpenFileL(KCalendarFileOther);
			CCalEntry* entry = CreateEntryLC(CCalEntry::EAppt, KRangeStartTime + TTimeIntervalDays(5));
			CleanupStack::Pop(entry);
			StoreEntryLD(entry, EFalse);
			}
			break;
		case EDeleteMultipleEntriesSetup:
			{
			*iNumExpectedChanges =  0;
			_LIT8(KValidGuid, "valid-guid");
			HBufC8* guid = KValidGuid().AllocLC();
			CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
			CleanupStack::Pop(guid);
			StoreEntryLD(entry, ETrue);
			}
			break;
		case EDeleteMultipleEntriesFail:
			{
			// Try to delete two entries, the second should fail and the first delete should be rolled back.
			// No notification must be received.
			_LIT8(KValidGuid, "valid-guid");
			_LIT8(KNoSuchGuid, "no-such-guid");
			CDesC8ArrayFlat* descArray = new (ELeave) CDesC8ArrayFlat(2);
			CleanupStack::PushL(descArray);
			descArray->AppendL(KValidGuid());
			descArray->AppendL(KNoSuchGuid());
			TRAPD(err, iCalTestLibrary->SynCGetEntryViewL().DeleteL(*descArray));
			__ASSERT_ALWAYS(err == KErrNotFound, User::Invariant());
			CleanupStack::PopAndDestroy(descArray);
			
			// check that the first entry has not been deleted
			RPointerArray<CCalEntry> entryArray;
			CleanupResetAndDestroyPushL(entryArray);
			iCalTestLibrary->SynCGetEntryViewL().FetchL(KValidGuid(), entryArray);
			__ASSERT_ALWAYS(entryArray.Count() == 1, User::Invariant());
			CleanupStack::PopAndDestroy(&entryArray);
			}
			break;
		default:
			{
			User::Panic(_L("A case statement for this test has not been added"), *iTestCase);
			}
			break;
		}
	}

// modifying thread entry point
TInt RunTestThread(TAny* /*aArgs*/)
	{
	TInt err;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler;
	CTestAppModifier* modifier = NULL;
	
	scheduler = new CActiveScheduler;
   	
	if (scheduler)
		{
		CActiveScheduler::Install(scheduler);

		// Create a semaphore
		RSemaphore semaphore;
		err = semaphore.CreateGlobal(KSemName(), 0);
		if (err == KErrAlreadyExists)
			{
			semaphore.OpenGlobal(KSemName());
			}

	
		TRAP(err, modifier = CTestAppModifier::NewL());
		if (err == KErrNone)
			{
			TRAP(err, modifier->RunTestL());
			}

		delete modifier;
		delete scheduler;
		delete trapCleanup;

		semaphore.Signal();
		semaphore.Close();
		}
	return err;
	}

