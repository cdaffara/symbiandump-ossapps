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
#include <calalarm.h>
#include <calcategory.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calrrule.h>
#include <caliterator.h>
#include <tz.h>
#include <vtzrules.h>
#include <e32test.h>
#include <s32file.h>

static RTest test(_L("tcal_entryattributes"));

_LIT8(KGUID1, "A_Global_UID1");
_LIT8(KGUID2, "A_Global_UID2");
_LIT8(KGUID3, "A_Global_UID3");
_LIT8(KGUID4, "A_Global_UID4");
_LIT8(KGUID5, "A_Global_UID5");
_LIT(KCalName, "TestCalendar");

_LIT(KSummary1, "dummy summary text");
_LIT(KDescription1, "dummy description text");

class CEntryAttributesTestManager : public CBase
	{
public:
	static CEntryAttributesTestManager* NewLC();
	~CEntryAttributesTestManager();

	void CreateEntriesL(TBool aFloating = EFalse);
	void TestFetchAndDeleteNonExistentEntryL();
	void TestLiteEntriesL();
	void TestLastChangedDateL();
	void TestLastChangedDateWithStoreL();
	void StoreLastChangedDatesL();
	void CheckForValuesL(TBool aFlip);
	void UpdateEntriesL();
	void TestCopySummaryAndDescriptionL();
	void TestAlarmedCompletedTodoL();
	void FindSummaryL();
	void TestEndTimeBeforeStartTimeL();
	void TestEndTimeNullL();
	void TestEndTimeForeverL();
	void TestAlarmedTodoWithoutDueDateL();
	void TestGEOAttributesL();
	void TestGEOValueSetAndGetL();
	void TestGEOEntrySetAndGetL();
	void TestGEOVerifySetAndGetL(const TReal& aSetLat, const TReal& aSetLong, const TReal& aExpLat, const TReal& aExpLong, CCalGeoValue& aCalGeoValue) const;
	void TestGEOInternalizeAndExternalizeL();
	void TestGEOCompareAndCopyFromL();
	void UpdateEntryHaveRdateOnlyL();
	void TestStartDateTestL();
	void TestStoreEntryWithTzL();
	void TestRdateL();
	void TestDeleteInstancesL(TInt aLoop=0);
	void GetInstanceCrossDstL();
	void FindInstancesL(TInt aExpectedNumInstances);
	// from former tcal_CopyEntryImpl
	
	void CreateEntriesForCopyL();
	void CreateEventForCopyPasteL();
	
	// for DEF080249: Addition of additional child entry with repeat rule should fail
	void AddTwoRepeatingChildL();
	
	void StoreAndExportL();

private:

	CEntryAttributesTestManager();
	void TestEndTimeNullL(CCalEntry::TType aType);

	// no copy constructor and assignment operator
	CEntryAttributesTestManager(CEntryAttributesTestManager& );
	CEntryAttributesTestManager& operator = (const CEntryAttributesTestManager& );

	void ConstructL();
	void TestCalEntryL(CCalEntry* aEntry, TBool aFloating = EFalse);
	void TestThisLiteEntryL(CCalEntry* aEntry);
	
	// from former tcal_CopyEntryImpl
	
	void CopyEventForCopyPasteL(const CCalEntry& aEntry);
	void CopyEntryL(const CCalEntry& aEntry);


private:
	CCalTestLibrary* iTestLib;
	CArrayFix<TTime>* iDateArray;
	};


// Constructor/Destructor

CEntryAttributesTestManager* CEntryAttributesTestManager::NewLC()
	{
	CEntryAttributesTestManager* self = new (ELeave) CEntryAttributesTestManager();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}

CEntryAttributesTestManager::CEntryAttributesTestManager()
	{
	}

void CEntryAttributesTestManager::ConstructL()
	{
	iDateArray = new(ELeave) CArrayFixFlat<TTime>(5);
	
	iTestLib = CCalTestLibrary::NewL(EFalse);
	
	iTestLib->ReplaceFileL(KCalName());
	iTestLib->OpenFileL(KCalName());
	}

	
CEntryAttributesTestManager::~CEntryAttributesTestManager()
	{
	TRAP_IGNORE(iTestLib->DeleteFileL(KCalName));
	delete iDateArray;
	delete iTestLib;
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Test cases forming the Entry Attributes test suite
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CEntryAttributesTestManager::TestDeleteInstancesL(TInt aLoop)
	{
	test.Printf(_L("Test TestDeleteInstancesL started\n"));

	
	_LIT8(KRnzLine1, "BEGIN:VCALENDAR\r\n"
					 "VERSION:1.0\r\n"
					 "TZ:+00\r\n"
					 "DAYLIGHT:TRUE;+01;20010325T010000Z;20011028T010000Z;;\r\n"
					 "DAYLIGHT:TRUE;+01;20070325T010000Z;20071028T010000Z;;\r\n"
					 "DAYLIGHT:TRUE;+01;20080330T010000Z;20081026T010000Z;;\r\n"
					 "DAYLIGHT:TRUE;+01;20090329T010000Z;20091025T010000Z;;\r\n"
					 "DAYLIGHT:TRUE;+01;20100328T010000Z;20101031T010000Z;;\r\n"
					 "DAYLIGHT:TRUE;+01;20110327T010000Z;20111030T010000Z;;\r\n"
					 "BEGIN:VEVENT\r\n"
					 "UID:5\r\n"
					 "DUE:19990416T230000Z\r\n"
					 "DTSTART:20010925T170000Z\r\n"
					 "DTEND:20010925T170000Z\r\n"
					 "X-EPOCAGENDAENTRYTYPE:ANNIVERSARY\r\n"
					 "CLASS:PUBLIC\r\n"
					 "SEQUENCE:0\r\n"
					 "X-METHOD:ADD\r\n"
					 "RRULE:YM1 9 #2\r\n"
					 "END:VEVENT\r\n"
					 "END:VCALENDAR\r\n");	
	
	RPointerArray<CCalEntry> importedEntryArr;
	CleanupResetAndDestroyPushL(importedEntryArr);

	iTestLib->ImportL(KRnzLine1, importedEntryArr);
	test.Printf(_L("File imported\n"));
	
	// store in the cal file
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(importedEntryArr, success);
	test(success == 1);
	
	CleanupStack::PopAndDestroy(&importedEntryArr);
	
	
	// fetch instances
	TTime		startDate(TDateTime(2000, EJanuary, 0, 12, 0, 0, 0));
	TCalTime	start;
	start.SetTimeUtcL(startDate);
		
	TTime		endDate(TDateTime(2098, EJanuary, 0, 12, 0, 0, 0));
	TCalTime	end;
	end.SetTimeUtcL(endDate);

	CalCommon::TCalTimeRange	timeRange(start, end);
	CalCommon::TCalViewFilter	filter=CalCommon::EIncludeAll;
	RPointerArray<CCalInstance> fetchedInstanceArr;
	CleanupClosePushL(fetchedInstanceArr);	
	iTestLib->SynCGetInstanceViewL().FindInstanceL(fetchedInstanceArr, filter, timeRange);
	TInt countInstances=fetchedInstanceArr.Count();
	
	// try to delete one-by-one

	if( aLoop==0 )
		{
		// one way to do it
		for(TInt i=countInstances-1;i>=0;i--)
			{
			iTestLib->SynCGetInstanceViewL().DeleteL(fetchedInstanceArr[i], CalCommon::EThisAndFuture); 
			}
		test.Printf(_L("TestDeleteInstances1 Finished\n"));
		}
	else
		{
		// another way to delete instaces
		for(TInt i=0;i<countInstances; i++)
			{
			iTestLib->SynCGetInstanceViewL().DeleteL(fetchedInstanceArr[i], CalCommon::EThisAndPrior); 
			}
		// cleanup
		test.Printf(_L("TestDeleteInstances2 Finished\n"));
		}
	CleanupStack::PopAndDestroy(&fetchedInstanceArr);
}
	
/* Set and get each property on a CCalEntry and check the two match. */ 	


void CEntryAttributesTestManager::TestCalEntryL(CCalEntry* aEntry, TBool aFloating)
	{
	TTime time1;
	time1.HomeTime();
	TTime time2(time1);
	time2 += TTimeIntervalHours(2);
	time2 += TTimeIntervalYears(1);
	
	TCalTime calTime1;
	TCalTime calTime2;

	if (aFloating)
		{
		calTime1.SetTimeLocalFloatingL(time1);
		calTime2.SetTimeLocalFloatingL(time2);
		}
	else
		{
		calTime1.SetTimeUtcL(time1);
		calTime2.SetTimeUtcL(time2);		
		}
	
	aEntry->SetStartAndEndTimeL(calTime1, calTime2);

	test.Next(_L("Start date"));

	if (aEntry->EntryTypeL() == CCalEntry::ETodo)
		{
		if (aFloating)
			{
			test(aEntry->StartTimeL().TimeLocalL().DaysFrom(time1).Int() == 0);
			}
		else
			{
			test(aEntry->StartTimeL().TimeUtcL().DaysFrom(time1).Int() == 0);
			}
		}
	else
		{
		if (aFloating)
			{
			test(aEntry->StartTimeL().TimeLocalL() == time1);
			}
		else
			{
			test(aEntry->StartTimeL().TimeUtcL() == time1);
			}
		}

	if (aEntry->EntryTypeL() == CCalEntry::EReminder)
		{
	
		test.Next(_L("End date"));

		if (aFloating)
			{
			test(aEntry->EndTimeL().TimeLocalL() == Time::NullTTime());	
			}
		else
			{
			test(aEntry->EndTimeL().TimeUtcL() == Time::NullTTime());
			}
		}
	else 
		{

		test.Next(_L("End date"));

		if (aFloating)
			{
			test(aEntry->EndTimeL().TimeLocalL() == time2);
			}
		else
			{
			test(aEntry->EndTimeL().TimeUtcL() == time2);
			}
		}

	if (aFloating)
		{

		test.Next(_L("time mode"));

		test(aEntry->StartTimeL().TimeMode() == TCalTime::EFloating);

		if (aEntry->EntryTypeL() != CCalEntry::EReminder)
			{
			test(aEntry->EndTimeL().TimeMode() == TCalTime::EFloating);
			}
		}


	test.Next(_L("alarm setting"));

	CCalAlarm* alarm = aEntry->AlarmL();
	test(alarm == NULL);
	TTime time4(time1);
	time4 += TTimeIntervalHours(5);
	
	alarm = CCalAlarm::NewL();
	
	const TInt KAlarmOffset(600);
	alarm->SetTimeOffset(KAlarmOffset);
	alarm->SetAlarmSoundNameL(KNullDesC());
	aEntry->SetAlarmL(alarm);
	delete alarm;
	alarm = NULL;
	TTimeIntervalHours hours2(0);
	alarm = aEntry->AlarmL();
	test(alarm->TimeOffset().Int() == KAlarmOffset);
	test(alarm->AlarmSoundNameL().Length() == 0);
	alarm->SetAlarmSoundNameL(_L("dummy"));
	test(alarm->AlarmSoundNameL().Compare(_L("dummy")) == 0);
	delete alarm;
	alarm = NULL;
	aEntry->SetAlarmL(NULL);
	test(aEntry->AlarmL() == NULL);
	
	const TInt KHugeAlarmOffset(600000);
	alarm = CCalAlarm::NewL();
	alarm->SetTimeOffset(KHugeAlarmOffset);
	aEntry->SetAlarmL(alarm);
	delete alarm;
	alarm = NULL;
	alarm = aEntry->AlarmL();
	test(alarm->TimeOffset().Int() == KHugeAlarmOffset);
	delete alarm;
	
	TTime time5(time1);
	time5 += TTimeIntervalHours(2);
	time5 += TTimeIntervalDays(5);

	if (aEntry->EntryTypeL() == CCalEntry::ETodo)
		{

		test.Next(_L("status"));

		aEntry->SetStatusL(CCalEntry::ETodoNeedsAction);
		test(aEntry->StatusL() == CCalEntry::ETodoNeedsAction);
		aEntry->SetStatusL(CCalEntry::ETodoInProcess);
		test(aEntry->StatusL() == CCalEntry::ETodoInProcess);

		test.Next(_L("complete date"));

		TCalTime completedCalTime;
		completedCalTime.SetTimeUtcL(time5);
		aEntry->SetCompletedL(ETrue, completedCalTime);
		test(aEntry->StatusL() == CCalEntry::ETodoCompleted);
		test(aEntry->CompletedTimeL().TimeUtcL() == time5);
		aEntry->SetCompletedL(EFalse, completedCalTime);
		test(aEntry->StatusL() != CCalEntry::ETodoCompleted);
		}
	else 
		{

		test.Next(_L("status"));

		aEntry->SetStatusL(CCalEntry::EConfirmed);
		test(aEntry->StatusL() == CCalEntry::EConfirmed);
		aEntry->SetStatusL(CCalEntry::ETentative);
		test(aEntry->StatusL() == CCalEntry::ETentative);
		}
		
	test.Next(_L("priority"));

	aEntry->SetPriorityL(5);
	test(aEntry->PriorityL() == 5);
	aEntry->SetPriorityL(9);
	test(aEntry->PriorityL() == 9);
	
	test.Next(_L("summary"));

	TBuf<32> buf1;
	buf1.Copy(KSummary1);
	aEntry->SetSummaryL(buf1);
	test(aEntry->SummaryL().Compare(buf1) == 0);
	
	test.Next(_L("Description"));

	TBuf<32> buf2;
	buf2.Copy(KDescription1);
	aEntry->SetDescriptionL(buf2);
	test(aEntry->DescriptionL().Compare(buf2) == 0);

	test.Next(_L("Location"));

	TBuf<32> buf3;
	buf3.Copy(_L("dummy Location text"));
	aEntry->SetLocationL(buf3);
	test(aEntry->LocationL().Compare(buf3) == 0);

	test.Next(_L("Category"));

	const RPointerArray<CCalCategory> categoryList = aEntry->CategoryListL();
	test(categoryList.Count() == 0);
	TBuf<32> buf4;
	buf4.Copy(_L("dummy Category text"));
	
	CCalCategory* category = CCalCategory::NewL(buf4);
	CleanupStack::PushL(category);
	aEntry->AddCategoryL(category);
	CleanupStack::Pop(category);
	const RPointerArray<CCalCategory> categoryList1 = aEntry->CategoryListL();
	test(categoryList1.Count() == 1);
	test(categoryList1[0]->ExtendedCategoryName().Compare(buf4) == 0);
		
	test.Next(_L("DTSTAMP"));

	TTime now;
	now.UniversalTime();
	TCalTime dTStampTime;
	dTStampTime.SetTimeUtcL(now);
	aEntry->SetDTStampL(dTStampTime);
	test(dTStampTime.TimeUtcL() == aEntry->DTStampL().TimeUtcL());
	
	// copy this entry

	test.Next(_L("entry copying"));

	HBufC8* uid = aEntry->UidL().AllocLC();
	CCalEntry* entryCopy = CCalEntry::NewL(aEntry->EntryTypeL(), uid, aEntry->MethodL(), 
		aEntry->SequenceNumberL(), aEntry->RecurrenceIdL(), aEntry->RecurrenceRangeL());
	CleanupStack::Pop(uid);
	CleanupStack::PushL(entryCopy);
	entryCopy->CopyFromL(*aEntry);
	test(entryCopy->CompareL(*aEntry));

	CleanupStack::PopAndDestroy(entryCopy);

	test.Next(_L("last modified date"));

	now.UniversalTime();
	aEntry->SetLastModifiedDateL();
	TTimeIntervalSeconds secs;
	aEntry->LastModifiedDateL().TimeUtcL().SecondsFrom(now, secs);
	test(secs.Int() < 2);
	
	test.Next(_L("method"));

		{
		// positive testing	
		
		aEntry->SetMethodL(CCalEntry::EMethodAdd);
		test(aEntry->MethodL() == CCalEntry::EMethodAdd);
		aEntry->SetMethodL(CCalEntry::EMethodCancel);
		test(aEntry->MethodL() == CCalEntry::EMethodCancel);
		aEntry->SetMethodL(CCalEntry::EMethodNone);
		test(aEntry->MethodL() == CCalEntry::EMethodNone);	
		aEntry->SetMethodL(CCalEntry::EMethodPublish);
		test(aEntry->MethodL() == CCalEntry::EMethodPublish);
		aEntry->SetMethodL(CCalEntry::EMethodRequest);
		test(aEntry->MethodL() == CCalEntry::EMethodRequest);
		aEntry->SetMethodL(CCalEntry::EMethodReply);
		test(aEntry->MethodL() == CCalEntry::EMethodReply);
		aEntry->SetMethodL(CCalEntry::EMethodRefresh);
		test(aEntry->MethodL() == CCalEntry::EMethodRefresh);
		aEntry->SetMethodL(CCalEntry::EMethodCounter);
		test(aEntry->MethodL() == CCalEntry::EMethodCounter);
		aEntry->SetMethodL(CCalEntry::EMethodDeclineCounter);
		test(aEntry->MethodL() == CCalEntry::EMethodDeclineCounter);
		
		// negative testing	
		
		TRAPD(err1, aEntry->SetMethodL(CCalEntry::TMethod(CCalEntry::EMethodNone-1)) );
		test(err1 == KErrArgument);

		TRAPD(err2, aEntry->SetMethodL(CCalEntry::TMethod(CCalEntry::EMethodDeclineCounter+1)) );
		test(err2 == KErrArgument);
		}	

		test.Next(_L("sequence number"));

		{
		aEntry->SetSequenceNumberL(1);
		test(aEntry->SequenceNumberL() == 1);
		}	
	
	test.Next(_L("replication status"));

		{
		// positive testing
		
		aEntry->SetReplicationStatusL(CCalEntry::EOpen);
		test(aEntry->ReplicationStatusL() == CCalEntry::EOpen);
		aEntry->SetReplicationStatusL(CCalEntry::EPrivate);
		test(aEntry->ReplicationStatusL() == CCalEntry::EPrivate);
		aEntry->SetReplicationStatusL(CCalEntry::ERestricted);
		test(aEntry->ReplicationStatusL() == CCalEntry::ERestricted);
		
		// negative testing	
		
		TRAPD(err1, aEntry->SetReplicationStatusL(CCalEntry::TReplicationStatus(CCalEntry::EOpen-1)) );
		test(err1 == KErrArgument);
		TRAPD(err2, aEntry->SetReplicationStatusL(CCalEntry::TReplicationStatus(CCalEntry::ERestricted+1)) );
		test(err2 == KErrArgument);
		}

		test.Next(_L("transparency"));

		{
		// positive testing	
		
		aEntry->SetTimeTransparencyL(CCalEntry::ETranspTentative);
		test(aEntry->TimeTransparencyL() == CCalEntry::ETranspTentative);
		aEntry->SetTimeTransparencyL(CCalEntry::ETranspBusy);
		test(aEntry->TimeTransparencyL() == CCalEntry::ETranspBusy);
		aEntry->SetTimeTransparencyL(CCalEntry::ETranspFree);
		test(aEntry->TimeTransparencyL() == CCalEntry::ETranspFree);
		aEntry->SetTimeTransparencyL(CCalEntry::ETranspOutOfOffice);
		test(aEntry->TimeTransparencyL() == CCalEntry::ETranspOutOfOffice);	

		// negative testing	

		TRAPD(err1, aEntry->SetTimeTransparencyL(CCalEntry::TTransp(-1)) );
		test(err1 == KErrArgument);	
		}	
		
	// .. other properties?
	}
	
	


void CEntryAttributesTestManager::TestThisLiteEntryL(CCalEntry* aEntry)
	{
/* Get properties on a CCalEntry (set as a lite entry) then compare with getting properties from the full entry. */

	test.Next(_L("Testing lite entry"));
	
	test.Next(_L("Start date"));

	TTime time1	= aEntry->StartTimeL().TimeUtcL();
	
	test.Next(_L("End date"));

	TTime time2 = aEntry->EndTimeL().TimeUtcL();
		
	test.Next(_L("priority"));

	TInt priority = aEntry->PriorityL();

	TTime time3;

	if (aEntry->EntryTypeL() == CCalEntry::ETodo)
		{

		test.Next(_L("complete date"));

		time3 = aEntry->CompletedTimeL().TimeUtcL();
		}
	
	// now use full aEntry and compare results
	aEntry->SetStatusL(aEntry->StatusL());
	
	test.Next(_L("Testing full aEntry"));
	
	test.Next(_L("Start date"));

	test(time1 == aEntry->StartTimeL().TimeUtcL());
	
	test.Next(_L("End date"));

	TDateTime dt1 = time2.DateTime();
	TDateTime dt2 = aEntry->EndTimeL().TimeUtcL().DateTime();
	CCalEntry::TType type = aEntry->EntryTypeL();
	test(time2 == aEntry->EndTimeL().TimeUtcL());

	test.Next(_L("priority"));

	test(priority == aEntry->PriorityL());

	if (aEntry->EntryTypeL() == CCalEntry::ETodo)
		{

		test.Next(_L("complete date"));

		test(time3 == aEntry->CompletedTimeL().TimeUtcL());
		}
	
	test(aEntry->TimeTransparencyL() == CCalEntry::ETranspOutOfOffice);
	}

void CEntryAttributesTestManager::CreateEntriesL(TBool aFloating)
	{
	TTime time1;
	time1.HomeTime();
	TTime time2(time1);
	time2 += TTimeIntervalHours(2);
	time2 += TTimeIntervalMonths(30);
	RPointerArray<CCalEntry> array;
	CleanupResetAndDestroyPushL(array);
	HBufC8* guid = KGUID1().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	TCalTime calTime1;
	TCalTime calTime2;

	if (aFloating)
		{
		calTime1.SetTimeLocalFloatingL(time1);
		calTime2.SetTimeLocalFloatingL(time2);
		}
	else
		{
		calTime1.SetTimeUtcL(time1);
		calTime2.SetTimeUtcL(time2);
		}

	entry->SetStartAndEndTimeL(calTime1, calTime2);
	TestCalEntryL(entry, aFloating);
	array.AppendL(entry);
	CleanupStack::Pop(entry);
	
	TInt success(0);
	
	iTestLib->SynCGetEntryViewL().StoreL(array, success);
	//Test storing an entry array twice and make sure it doesn't leave
	test.Next(_L("Storing an entry array twice"));

	iTestLib->SynCGetEntryViewL().StoreL(array, success);
	delete entry;
	entry = NULL;

	array.Reset();
	guid = KGUID2().AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(calTime1, calTime2);
	TestCalEntryL(entry, aFloating);
	
	array.AppendL(entry);
	CleanupStack::Pop(entry);
	iTestLib->SynCGetEntryViewL().StoreL(array, success);
	delete entry;
	entry = NULL;
	array.Reset();
	
	guid = KGUID3().AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(calTime1, calTime2);
	TestCalEntryL(entry, aFloating);
	
	array.AppendL(entry);
	CleanupStack::Pop(entry);
	iTestLib->SynCGetEntryViewL().StoreL(array, success);
	delete entry;
	entry = NULL;
	array.Reset();
	
	guid = KGUID4().AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EAnniv, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(calTime1, calTime2);
	TestCalEntryL(entry, aFloating);
	
	array.AppendL(entry);
	CleanupStack::Pop(entry);
	iTestLib->SynCGetEntryViewL().StoreL(array, success);
	delete entry;
	entry = NULL;
	array.Reset();
	
	guid = KGUID5().AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EReminder, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(calTime1, calTime2);
	TestCalEntryL(entry, aFloating);
	
	array.AppendL(entry);
	CleanupStack::Pop(entry);
	iTestLib->SynCGetEntryViewL().StoreL(array, success);
	delete entry;
	entry = NULL;
	array.Reset();
	CleanupStack::Pop(&array);
	}

	
void CEntryAttributesTestManager::TestLiteEntriesL()
	{
// retrieve lite entries from database and test them, including checking that full entries can be retrieved
	RPointerArray<CCalInstance> array;
	CleanupResetAndDestroyPushL(array);
	
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime startDate;
	startDate.SetTimeLocalL(TDateTime(2000, EJanuary, 0, 0, 0, 0, 0));
	TCalTime endDate;
	endDate.SetTimeLocalL(TDateTime(2010, EJanuary, 0, 0, 0, 0, 0));
	
	CalCommon::TCalTimeRange timeRange(startDate, endDate);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(array, filter, timeRange);
	TInt i = 0;
	while (i < array.Count())
		{
		TestThisLiteEntryL(&array[i]->Entry());
		i++;
		}
		
	CleanupStack::PopAndDestroy(&array);
	}
	

void CEntryAttributesTestManager::StoreLastChangedDatesL()
	{
	test.Next(_L("Storing LastChangedDate before Modifying entries"));

	
	CCalIter& iter = iTestLib->GetIterL();
	
	for( TPtrC8 Uid(iter.FirstL()); Uid != KNullDesC8(); Uid.Set(iter.NextL()) )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		
		iTestLib->SynCGetEntryViewL().FetchL(Uid, entryList);
		iDateArray->AppendL(entryList[0]->LastModifiedDateL().TimeUtcL());
		
		CleanupStack::PopAndDestroy(&entryList); 
		}
	}


void CEntryAttributesTestManager::CheckForValuesL(TBool aFlip)
	{
	test.Next(_L("Checking entries"));

	
	CCalIter& iter = iTestLib->GetIterL();
	
	TInt i = 0;
	for( TPtrC8 Uid(iter.FirstL()); Uid != KNullDesC8(); Uid.Set(iter.NextL()), ++i )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		
		iTestLib->SynCGetEntryViewL().FetchL(Uid, entryList);

		TDateTime dt= iDateArray->At(i).DateTime();
		test.Printf(_L("Initial Lastmodified date is  %d/%d/%d, %d:%d:%d\n"),dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
		dt= entryList[0]->LastModifiedDateL().TimeUtcL().DateTime();

		if(aFlip) //while fetching last changed time should not be altered.
		{
			test(iDateArray->At(i) == entryList[0]->LastModifiedDateL().TimeUtcL());
			test.Printf(_L("Fetching after 1 seconds   %d/%d/%d, %d:%d:%d\n\n"),dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
		}	
		else	 //while Updating last changed time should be set to current time.
		{
			test(!(iDateArray->At(i) == entryList[0]->LastModifiedDateL().TimeUtcL()));	
			test.Printf(_L("After Updating   %d/%d/%d, %d:%d:%d\n\n"),dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
		}
		
		CleanupStack::PopAndDestroy(&entryList);	
		}
	}


void CEntryAttributesTestManager::UpdateEntriesL()
	{
	test.Next(_L("Updating entries so the LastChangedDate shud change"));

	
	CCalIter& iter = iTestLib->GetIterL();
	
	for( TPtrC8 Uid(iter.FirstL()); Uid != KNullDesC8(); Uid.Set(iter.NextL()) )
		{
		RPointerArray<CCalEntry> entryList;
		CleanupResetAndDestroyPushL(entryList);
		iTestLib->SynCGetEntryViewL().FetchL(Uid, entryList);

		TBuf<32> bufSummary;
		bufSummary.Copy(_L("dummy summary text"));
		entryList[0]->SetSummaryL(bufSummary);
		TBuf<32> bufDescription;
		bufDescription.Copy(_L("dummy description text"));
		entryList[0]->SetDescriptionL(bufDescription);
		TInt success=0;
		iTestLib->SynCGetEntryViewL().UpdateL(entryList, success);
		test(success==entryList.Count());
		entryList.ResetAndDestroy();
		iTestLib->SynCGetEntryViewL().FetchL(Uid, entryList);
		test(entryList[0]->SummaryL()==bufSummary);
		test(entryList[0]->DescriptionL()==bufDescription);
		entryList[0]->SetSummaryL(KNullDesC);
		entryList[0]->SetDescriptionL(KNullDesC);
		iTestLib->SynCGetEntryViewL().UpdateL(entryList, success);
		entryList.ResetAndDestroy();
		iTestLib->SynCGetEntryViewL().FetchL(Uid, entryList);
		test(entryList[0]->SummaryL()==KNullDesC);
		test(entryList[0]->DescriptionL()==KNullDesC);
		CleanupStack::PopAndDestroy(&entryList);	
		}
	}	
	
void CEntryAttributesTestManager::TestLastChangedDateL()
	{
	// check last modified date changes during an update:
	StoreLastChangedDatesL();
	CheckForValuesL(ETrue);

	UpdateEntriesL();
	CheckForValuesL(EFalse);	

	// check last modified date changes during a StoreL on a parent (not allowed on a child)
	TestLastChangedDateWithStoreL();
	}

void CEntryAttributesTestManager::TestLastChangedDateWithStoreL()
	{
	_LIT8(KGuid, "test StoreL with LastModifiedDateL");
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* parent = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(parent);
	
	TTime time = TDateTime(2006, EJanuary, 0, 9, 0, 0, 0);
	TCalTime dummyTime;
	dummyTime.SetTimeLocalL(time); // 0900 on 1 Jan 06
	parent->SetStartAndEndTimeL(dummyTime, dummyTime);
	
	const TTime KOriginalLastModTime = parent->LastModifiedDateL().TimeLocalL();
	
	iTestLib->StoreEntryL(*parent);	
	CleanupStack::PopAndDestroy(parent);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	iTestLib->SynCGetEntryViewL().FetchL(KGuid, entries);
	test(entries.Count() == 1);
	parent = entries[0];
	test(KOriginalLastModTime == parent->LastModifiedDateL().TimeLocalL());
	
	// wait a second so the last modified date will be different
	User::After(1000000);
	// update the parent
	_LIT(KSummary, "zxcvbnm");
	parent->SetSummaryL(KSummary);
	iTestLib->StoreEntryL(*parent);
	
	entries.ResetAndDestroy();
	
	// get updated parent
	iTestLib->SynCGetEntryViewL().FetchL(KGuid, entries);
	test(entries.Count() == 1);
	parent = entries[0];
	test(KOriginalLastModTime < parent->LastModifiedDateL().TimeLocalL());
	test(parent->LastModifiedDateL().TimeLocalL() < KOriginalLastModTime + TTimeIntervalSeconds(3));
	
	CleanupStack::PopAndDestroy(&entries);
	}
	
void CEntryAttributesTestManager::TestCopySummaryAndDescriptionL()
	{
	CreateEntriesL(EFalse);
	
	RPointerArray<CCalEntry> entryList;
	CleanupResetAndDestroyPushL(entryList);
	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
	
	iTestLib->SynCGetEntryViewL().FetchL(KGUID1(), entryList);
	_LIT8(KNewGuid, "zxcvbnm");
	HBufC8* uid = KNewGuid().AllocLC();
	CCalEntry* newEntry = CCalEntry::NewL(CCalEntry::ETodo, uid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uid);
	CleanupStack::PushL(newEntry);
	newEntry->CopyFromL(*entryList[0], CCalEntry::EDontCopyId);
	
	entriesToStore.AppendL(newEntry);
	CleanupStack::Pop(newEntry);

	TInt success(0);
	iTestLib->SynCGetEntryViewL().StoreL(entriesToStore, success);
	test(success == 1);
	
	CDesC8Array* uidArray = new (ELeave) CDesC8ArrayFlat(1);
	CleanupStack::PushL(uidArray);
	uidArray->AppendL(KGUID1());
	iTestLib->SynCGetEntryViewL().DeleteL(*uidArray);
	CleanupStack::PopAndDestroy(uidArray);
	
	entryList.ResetAndDestroy();
	iTestLib->SynCGetEntryViewL().FetchL(KNewGuid(), entryList);
	test(entryList.Count() == 1);
	
	test(entryList[0]->SummaryL() == KSummary1());
	test(entryList[0]->DescriptionL() == KDescription1());
	CleanupStack::PopAndDestroy(); // entriesToStore
	CleanupStack::PopAndDestroy(); // entryList
	}

void CEntryAttributesTestManager::TestAlarmedCompletedTodoL()
	{
	_LIT8(KGuid1, "guid-1");
	
	_LIT(KTime1, "20030127:020000.000000");
	_LIT(KTime2, "20040127:020000.000000");
		
	HBufC8* guid = KGuid1().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodAdd, (TUint)0);	
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TTime time1(KTime1());
	TCalTime st;
	st.SetTimeUtcL(time1);
	entry->SetStartAndEndTimeL(st, st);
	
	TTime time2(KTime2());
	TCalTime comp;
	comp.SetTimeUtcL(time2);
	entry->SetCompletedL(ETrue, comp);
	
	TTime compTime = entry->CompletedTimeL().TimeUtcL();
	test(compTime == time2);
	
	CCalAlarm* alarm=CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(20);
	alarm->SetAlarmSoundNameL(_L("SHHH"));
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	
	CCalAlarm* alarm1 = entry->AlarmL();
	test(alarm1 == NULL);
			
	iTestLib->StoreEntryL(*entry);
	CleanupStack::PopAndDestroy(entry);
	
	// fetch the entry
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	iTestLib->SynCGetEntryViewL().FetchL(KGuid1, entries);
	
	entry = entries[0];
	
	TTime compTime2 = entry->CompletedTimeL().TimeUtcL();
	test(compTime2 == time2);
	
	CCalAlarm* alarm2 = entry->AlarmL();
	test(alarm2 == NULL);
	CleanupStack::PopAndDestroy(&entries);
	}

// from former tcal_CopyEntryImpl

void CEntryAttributesTestManager::CopyEntryL(const CCalEntry& aEntry)
	{
	HBufC8* guid = aEntry.UidL().AllocLC();
	CCalEntry::TType type = aEntry.EntryTypeL();
    CCalEntry::TMethod method = aEntry.MethodL();
    TInt seqNumber = aEntry.SequenceNumberL();
    
    CCalEntry* entry = CCalEntry::NewL(type,guid,method,seqNumber);
    CleanupStack::Pop(guid);
    CleanupStack::PushL(entry);
    
	const TDesC8& original_uid = aEntry.UidL();
	const TDesC8& uid_before_copy = entry->UidL();
	
	__ASSERT_ALWAYS(original_uid == uid_before_copy, User::Invariant());
	
	entry->CopyFromL(aEntry);
	
	const TDesC8& uid_after_copy = entry->UidL();
	
	__ASSERT_ALWAYS(original_uid == uid_after_copy, User::Invariant());
   
    CleanupStack::PopAndDestroy(entry);
   	}
   	

void CEntryAttributesTestManager::CopyEventForCopyPasteL(const CCalEntry& aEntry)
	{
    CCalEntry::TType type = aEntry.EntryTypeL();
    CCalEntry::TMethod method = aEntry.MethodL();
    TInt seqNumber = aEntry.SequenceNumberL();
	_LIT8(KGuid, "ABCDEFGHIJ");
	HBufC8* newUid = KGuid().AllocLC();
    CCalEntry* entry = CCalEntry::NewL(type,newUid,method,seqNumber);
    CleanupStack::Pop(newUid);
    CleanupStack::PushL(entry);
    TCalLocalUid newLocalUid(4);
	entry->SetLocalUidL(newLocalUid);
	entry->CopyFromL(aEntry ,CCalEntry::EDontCopyId);
	
	HBufC8* copyUid = entry->UidL().AllocLC();

	const TDesC8& original_uid = aEntry.UidL();
	const TDesC8& uid_after_copy = entry->UidL();
	TCalLocalUid originalUniqueId = aEntry.LocalUidL();
	TCalLocalUid copiedUniqueId = entry->LocalUidL();
	
	__ASSERT_ALWAYS(originalUniqueId != copiedUniqueId, User::Invariant());
	__ASSERT_ALWAYS(original_uid != uid_after_copy, User::Invariant());
   
   	entry->CopyFromL(aEntry, CCalEntry::ECopyAll);
	
	const TDesC8& original_entry_uid = aEntry.UidL();
	const TDesC8& after_copy_uid = entry->UidL();
	copiedUniqueId = entry->LocalUidL();
	
	__ASSERT_ALWAYS(originalUniqueId == copiedUniqueId, User::Invariant());
	__ASSERT_ALWAYS(original_entry_uid == after_copy_uid, User::Invariant());
   
   	CleanupStack::PopAndDestroy(copyUid);
   	CleanupStack::PopAndDestroy(entry);
    }
    
	
void CEntryAttributesTestManager::CreateEntriesForCopyL()
	{
	TTime time1;
	time1.HomeTime();
	TTime time2(time1);
	time2 += TTimeIntervalHours(2);
	time2 += TTimeIntervalMonths(30);
	
	HBufC8* guid = KGUID1().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime calTime1;
	calTime1.SetTimeUtcL(time1);
	TCalTime calTime2;
	calTime2.SetTimeUtcL(time2);
	entry->SetStartAndEndTimeL(calTime1, calTime2);
	
	CopyEntryL(*entry);

	CleanupStack::PopAndDestroy(entry);
	}
	

void CEntryAttributesTestManager::CreateEventForCopyPasteL()
	{
	TTime time1;
	time1.HomeTime();
	TTime time2(time1);
	time2 += TTimeIntervalHours(2);
	time2 += TTimeIntervalMonths(30);
	
	HBufC8* guid = KGUID1().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime calTime1;
	calTime1.SetTimeUtcL(time1);
	TCalTime calTime2;
	calTime2.SetTimeUtcL(time2);
	entry->SetStartAndEndTimeL(calTime1, calTime2);
	
	CopyEventForCopyPasteL(*entry);
	
	CleanupStack::PopAndDestroy(entry);
	}


void CEntryAttributesTestManager::FindSummaryL()
	{
	_LIT8(KGuid1, "guid-1");
	_LIT(KSummary1, "SAMPLE SUMMARY");
	_LIT(KTime1, "20020815:230000.000000");
	
	_LIT(KTimeFindSt, "20020715:230000.000000");
	_LIT(KTimeFindEnd, "20020915:230000.000000");
	
	HBufC8* guid8 = KGuid1().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EReminder, guid8, CCalEntry::EMethodAdd, (TUint)0);	
	CleanupStack::Pop(guid8);
	CleanupStack::PushL(entry);
	TTime time(KTime1());
	TCalTime st;
	st.SetTimeUtcL(time);
	
	entry->SetStartAndEndTimeL(st, st);
	entry->SetSummaryL(KSummary1);
	
	TInt						entriesStored(0);
	RPointerArray<CCalEntry>	entriesToStore;
	entriesToStore.AppendL(entry);
	TRAPD(err, iTestLib->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored));
	test(err == KErrNone);
	CleanupStack::PopAndDestroy(entry);
	entriesToStore.Reset();
	
	// added to test with a new session //
	delete iTestLib;
	iTestLib = NULL;
	iTestLib = CCalTestLibrary::NewL(EFalse);
	iTestLib->OpenFileL(KCalName());
	
	// testing if entry exists with new session //
	RPointerArray<CCalEntry>	entries;
	iTestLib->SynCGetEntryViewL().FetchL(KGuid1, entries);
	test(entries[0]->SummaryL() == KSummary1);
	entries.ResetAndDestroy();

	RPointerArray<CCalInstance>	instances;
	
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	
	TTime time1(KTimeFindSt());
	st.SetTimeUtcL(time1);
	TTime time2(KTimeFindEnd());
	TCalTime end;
	end.SetTimeUtcL(time2);
	CalCommon::TCalTimeRange timerange(st, end);
	
	CalCommon::TCalSearchBehaviour behaviour = CalCommon::EExactTextSearch;
	
	CCalInstanceView::TCalSearchParams params(KSummary1, behaviour);
	
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, filter, timerange, params);
		
	test.Printf(_L("Instances found with correct summary = %D\n"), instances.Count());
	test(instances.Count() == 1);
	
	instances.ResetAndDestroy();
	}


void CEntryAttributesTestManager::AddTwoRepeatingChildL()
	{
	// Add Entries for Test
	test.Next(_L("Add Two Repeating Children"));


	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);

	_LIT8(KUid, "UID_AABB");
	HBufC8* guid = KUid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	TTime startTime (TDateTime(2005, EJanuary, 7, 10, 0, 0, 0)); 
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	TTime endTime (TDateTime(2005, EJanuary, 7, 11, 0, 0, 0)); 
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);

	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EDaily);
	CleanupStack::PushL(rpt);
	TTime daily1end(TDateTime(2005, EJanuary, 20, 10, 0, 0, 0));
	TCalTime rptendTime;
	rptendTime.SetTimeLocalL(daily1end);

	rpt->SetDtStart(calStartTime);
	rpt->SetUntil(rptendTime);
	rpt->SetInterval(1);
	entry->SetRRuleL(*rpt);//take a copy of the rule
	CleanupStack::PopAndDestroy(rpt);
	entriesToStore.Append(entry);
	CleanupStack::Pop(entry);
	
//create a child entry and store it
	guid = KUid().AllocLC();
	TTime ttime (TDateTime(2005, EJanuary, 15, 10, 0, 0, 0));
	TCalTime recurrenceId;
	recurrenceId.SetTimeUtcL(ttime);
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodRequest, 0, recurrenceId, CalCommon::EThisAndFuture);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	// Start date same as RecId. End time is 1 hour later.
	TTime newStartDate (TDateTime(2005, EJanuary, 15, 11, 0, 0, 0));
	TTime newEndDate (TDateTime(2005, EJanuary, 15, 12, 0, 0, 0));
	
	TCalTime newCalstartTime;
	newCalstartTime.SetTimeUtcL(newStartDate);
	
	TCalTime newCalendTime;
	newCalendTime.SetTimeUtcL(newEndDate);
	
	entry->SetStartAndEndTimeL(newCalstartTime, newCalendTime);
	
	rpt = new (ELeave) TCalRRule(TCalRRule::EDaily);
	CleanupStack::PushL(rpt);
	TTime dailyend(TDateTime(2005, EJanuary, 25, 11, 0, 0, 0));
	TCalTime rptChildendTime;
	rptChildendTime.SetTimeLocalL(dailyend);

	rpt->SetDtStart(newCalstartTime);
	rpt->SetUntil(rptChildendTime);
	rpt->SetInterval(1);
	entry->SetRRuleL(*rpt);//take a copy of the rule
	CleanupStack::PopAndDestroy(rpt);
	entriesToStore.Append(entry);
	CleanupStack::Pop(entry);
	
	TInt entriesStored;
	iTestLib->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesStored ==2);
	delete entriesToStore[0];
	entriesToStore.Reset();
	CleanupStack::PushL(entry);
	//create another child
	guid = KUid().AllocLC();
	ttime = TDateTime(2005, EJanuary, 10, 10, 0, 0, 0);
	TCalTime recurrenceId1;
	recurrenceId1.SetTimeUtcL(ttime);
	CCalEntry* otherChild = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodRequest, 0, recurrenceId1, CalCommon::EThisAndPrior);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(otherChild);
	otherChild->CopyFromL(*entry, CCalEntry::EDontCopyId);
	TCalRRule otherChildRpt;
	test(otherChild->GetRRuleL(otherChildRpt));
	otherChild->SetStartAndEndTimeL(calStartTime, calStartTime);
	otherChildRpt.SetDtStart(calStartTime);
	otherChildRpt.SetUntil(recurrenceId1);
	otherChild->SetRRuleL(otherChildRpt);
	entriesToStore.Append(otherChild);
	CleanupStack::Pop(otherChild);
	CleanupStack::PopAndDestroy(entry);
	TRAPD(err, iTestLib->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored));
	test(err == KErrNotSupported);
	
	CleanupStack::PopAndDestroy(&entriesToStore);
	}

void CEntryAttributesTestManager::TestEndTimeBeforeStartTimeL()
	{
	_LIT8(KGuid, "guid");
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodRequest, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	TCalTime startTime, endTime;
	startTime.SetTimeLocalL(TDateTime(2005, EFebruary, 20, 10, 0, 0, 0));
	endTime.SetTimeLocalL(TDateTime(2005, EFebruary, 20, 9, 0, 0, 0));

	test.Printf(_L("Test can't set end time earlier than start time"));
	TRAPD(err, entry->SetStartAndEndTimeL(startTime, endTime));
	test(err == KErrArgument);

	CleanupStack::PopAndDestroy(entry);	
	}

void CEntryAttributesTestManager::TestAlarmedTodoWithoutDueDateL()
	{
	// check that a Todo without a Due date cannot have an alarm
	const TInt KTimeOffset = 10;
	_LIT8(KGuid, "alarmed-todo");
	
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* todo = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(todo);

	TCalTime start, end;
	start.SetTimeLocalL(TDateTime(2006, EJuly, 20, 9, 0, 0, 0));
	end.SetTimeLocalL(Time::NullTTime());
	todo->SetStartAndEndTimeL(start, end);
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(KTimeOffset);
	todo->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	alarm = NULL;
	
	// check alarm not set
	alarm = todo->AlarmL();
	test(alarm == NULL);
	CleanupStack::PopAndDestroy(todo);
	}

void CEntryAttributesTestManager::TestFetchAndDeleteNonExistentEntryL()
	{
	_LIT8(KGuid, "guid");
	
	// test fetching non-existing entry by GUID
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	iTestLib->SynCGetEntryViewL().FetchL(KGuid, entries);
	test(entries.Count() == 0);
	CleanupStack::PopAndDestroy(&entries);
	
	// test fetching non-existing entry by LUID
	test(iTestLib->SynCGetEntryViewL().FetchL(5) == NULL);
	
	// test deleting non-existing entry by GUID
	CDesC8ArrayFlat* guidArray = new (ELeave) CDesC8ArrayFlat(1);
	CleanupStack::PushL(guidArray);
	guidArray->AppendL(KGuid);
	TRAPD(err, iTestLib->SynCGetEntryViewL().DeleteL(*guidArray));
	test(err == KErrNotFound);
	CleanupStack::PopAndDestroy(guidArray);
	
	// test deleting non-existing entry by LUID
	RArray<TCalLocalUid> luids;
	CleanupClosePushL(luids);
	luids.AppendL(40); // dummy number
	TInt success = 0;
	TRAP(err, iTestLib->SynCGetEntryViewL().DeleteL(luids, success)); // doesn't leave
	test(err == KErrNone);
	test(success == 1);
	CleanupStack::PopAndDestroy(&luids);
	}

void CEntryAttributesTestManager::TestEndTimeNullL(CCalEntry::TType aType)
	{
	const TTime KStartTime(TDateTime(2006, EJanuary, 0, 0, 0, 0, 0));
	
	_LIT8(KGuid, "guid");
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime calStartTime, calNullTime;
	calStartTime.SetTimeLocalL(KStartTime);
	calNullTime.SetTimeLocalL(Time::NullTTime());
	entry->SetStartAndEndTimeL(calStartTime, calNullTime);
	if (aType == CCalEntry::ETodo)
		{
		test(entry->StartTimeL().TimeLocalL() == Time::NullTTime());
		}
	else
		{
		test(entry->StartTimeL().TimeLocalL() == KStartTime);
		}
	if (aType == CCalEntry::EReminder ||
		aType == CCalEntry::ETodo)
		{
		test(entry->EndTimeL().TimeLocalL() == Time::NullTTime());
		}
	else
		{
		test(entry->EndTimeL().TimeLocalL() == KStartTime);
		}
	CleanupStack::PopAndDestroy(entry);
	}

void CEntryAttributesTestManager::TestEndTimeNullL()
	{
	TestEndTimeNullL(CCalEntry::EAppt);
	TestEndTimeNullL(CCalEntry::EEvent);
	TestEndTimeNullL(CCalEntry::EAnniv);
	// for these entry types, the end time remains NULL
	TestEndTimeNullL(CCalEntry::EReminder);
	TestEndTimeNullL(CCalEntry::ETodo);
	}

/**

@SYMTestCaseID     PIM-TCAL-ENTRYATTRIBUTES-0037

*/

void CEntryAttributesTestManager::TestGEOAttributesL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-ENTRYATTRIBUTES-0037 GEO testing"));

	
	// Test set and get on the CCalGeoValue object - bounds, truncation and signs
	TestGEOValueSetAndGetL();
	// Test set and get on the entry and verify GEO values
	TestGEOEntrySetAndGetL();
	// Test internalizing and externalizing entry and verify GEO values
	TestGEOInternalizeAndExternalizeL();	
	// Test Compare and CopyFrom on the entry and verify GEO values
	TestGEOCompareAndCopyFromL();
	}

void CEntryAttributesTestManager::TestGEOValueSetAndGetL()
	{
	// Positive testing
	TReal geoTestLatOrig(0);
	TReal geoTestLongOrig(0);

	TReal geoTestLat(geoTestLatOrig);
	TReal geoTestLong(geoTestLongOrig);
	
	// Expected values
	TReal geoTestLatEx;
	TReal geoTestLongEx;

	// Create a CCalGeoValue object for testing
	CCalGeoValue* geoValue = CCalGeoValue::NewL();
	CleanupStack::PushL(geoValue);
	
	// Get - Check the default initialisation
	test(!geoValue->GetLatLong(geoTestLat, geoTestLong));
	// Check no change in values
	test(geoTestLat==geoTestLatOrig && geoTestLong==geoTestLongOrig);
		
	// Test Set
	// Bounds checking
	// Within bounds
	geoTestLat 	=  45.123456;
	geoTestLong	=  90.123456;
	
	geoTestLatEx 	=  45.123456;
	geoTestLongEx	=  90.123456;
	
	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);
	
	geoTestLat	=  -45.123456;
	geoTestLong	=  -90.123456;
	
	geoTestLatEx 	=  -45.123456;
	geoTestLongEx	=  -90.123456;
	
	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);
	
	// Truncation and rounding
	// Rounding up
	geoTestLat 	=  45.1234567123;
	geoTestLong	=  90.1234567123;
	
	geoTestLatEx 	=  45.123457;
	geoTestLongEx	=  90.123457;
	
	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);

	geoTestLat 	=  -45.1234567123;
	geoTestLong	=  -90.1234567123;
	
	geoTestLatEx 	=  -45.123457;
	geoTestLongEx	=  -90.123457;
	
	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);
	
	// Rounding down
	geoTestLat 	=  45.1234561234;
	geoTestLong	=  90.1234561234;
	
	geoTestLatEx 	=  45.123456;
	geoTestLongEx	=  90.123456;
	
	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);

	geoTestLat 	=  -45.1234561234;
	geoTestLong	=  -90.1234561234;
	
	geoTestLatEx 	=  -45.123456;
	geoTestLongEx	=  -90.123456;
	
	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);
	
	// On bounds
	geoTestLat	= 0.000000;
	geoTestLong	= 0.000000;
	
	geoTestLatEx 	=  0.000000;
	geoTestLongEx	=  0.000000;

	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);

	geoTestLat	= KCalGEOMaxLatitude;
	geoTestLong	= KCalGEOMaxLongitude;
	
	geoTestLatEx	= KCalGEOMaxLatitude;
	geoTestLongEx	= KCalGEOMaxLongitude;	

	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);
	
	geoTestLat	= KCalGEOMinLatitude;
	geoTestLong	= KCalGEOMinLongitude;
	
	geoTestLatEx	= KCalGEOMinLatitude;
	geoTestLongEx	= KCalGEOMinLongitude;	

	TestGEOVerifySetAndGetL(geoTestLat,geoTestLong, geoTestLatEx,  geoTestLongEx, *geoValue);
	
	// Out of the bounds
	TInt geoErr=KErrNone;
	
	// latitude - posititve 
	geoTestLat	=  95.123456;
	geoTestLong	=  45.123456;
	
	TRAP(geoErr, geoValue->SetLatLongL(geoTestLat, geoTestLong));
	test(geoErr==KErrArgument);
	
	// longitude - posititve 
	geoTestLat	=  50.123456;
	geoTestLong	=  185.123456;

	TRAP(geoErr, geoValue->SetLatLongL(geoTestLat, geoTestLong));
	test(geoErr==KErrArgument);
	
	// latitude - negative
	geoTestLat	=  -95.123456;
	geoTestLong	=  -45.123456;
	
	TRAP(geoErr, geoValue->SetLatLongL(geoTestLat, geoTestLong));
	test(geoErr==KErrArgument);
	
	// longitude - negative
	geoTestLat	=  -50.123456;
	geoTestLong	=  -185.123456;

	TRAP(geoErr, geoValue->SetLatLongL(geoTestLat, geoTestLong));
	test(geoErr==KErrArgument);
	
	// latitude and longitude - positive
	geoTestLat	=  95.123456;
	geoTestLong	=  185.123456;

	TRAP(geoErr, geoValue->SetLatLongL(geoTestLat, geoTestLong));
	test(geoErr==KErrArgument);

	// latitude and longitude - negative
	geoTestLat	=  -95.123456;
	geoTestLong	=  -185.123456;

	TRAP(geoErr, geoValue->SetLatLongL(geoTestLat, geoTestLong));
	test(geoErr==KErrArgument);
		
	CleanupStack::PopAndDestroy(geoValue);
	}

void CEntryAttributesTestManager::TestGEOVerifySetAndGetL(const TReal& aSetLat, const TReal& aSetLong, const TReal& aExpLat, const TReal& aExpLong, CCalGeoValue& aCalGeoValue) const
	{
	TReal getLat;
	TReal getLong;
	
	aCalGeoValue.SetLatLongL(aSetLat,aSetLong);
	test(aCalGeoValue.GetLatLong(getLat, getLong));
	test(getLat==aExpLat && getLong==aExpLong);
	}

void CEntryAttributesTestManager::TestGEOEntrySetAndGetL()
	{
	HBufC8* guid = NULL;
	
	// Entry for test purposes
	CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt,guid);
	CleanupStack::PushL(entry);
	
	// == Test default initialisation of GEO values on the entry
	// Retrieve GEO value from an entry when it has not been set
	CCalGeoValue* geoValue = entry->GeoValueL();
	// Default get on GEO values on the entry should return NULL 
	test(geoValue==NULL);
	
	// == Test Set and Get on the entry
	CCalGeoValue* geoValueSet = CCalGeoValue::NewL();
	CleanupStack::PushL(geoValueSet);
	
	// Set
	geoValueSet->SetLatLongL(TReal(KCalGEOMaxLatitude),TReal(KCalGEOMaxLongitude));
	// Set on entry
	entry->SetGeoValueL(*geoValueSet);
	
	CleanupStack::PopAndDestroy(geoValueSet);
	
	// Get and validate the retrieved CCalGeoValue
	// Get from entry
	CCalGeoValue* geoValueGet = entry->GeoValueL();
	CleanupStack::PushL(geoValueGet);
	TReal geoLat;
	TReal geoLong;
	// Get
	geoValueGet->GetLatLong(geoLat,geoLong);
	
	test(geoLat==KCalGEOMaxLatitude);
	test(geoLong==KCalGEOMaxLongitude);
	
	CleanupStack::PopAndDestroy(geoValueGet);
	
	// == Test clearing or resetting GEO value on the entry
	// Clear entry GEO values
	entry->ClearGeoValueL();
	
	// Retrieve GEO value from an entry when it has been cleared
	CCalGeoValue* geoValueClear = entry->GeoValueL();
	// Get on GEO values after clearing them on the entry should return NULL 
	test(geoValueClear==NULL);

	CleanupStack::PopAndDestroy(entry);
	}

void CEntryAttributesTestManager::TestGEOInternalizeAndExternalizeL()
	{
	// == Create a test entry with a GEO value
	HBufC8* guid = NULL;

	CCalEntry* storeEntry = iTestLib->CreateCalEntryL(CCalEntry::EAppt,guid);
	CleanupStack::PushL(storeEntry);
	
	iTestLib->SetEntryStartAndEndTimeL(storeEntry);
	
	// Set GEO value on the entry to be stored
	CCalGeoValue* geoValueSet = CCalGeoValue::NewL();
	CleanupStack::PushL(geoValueSet);
	
	geoValueSet->SetLatLongL(TReal(KCalGEOMaxLatitude),TReal(KCalGEOMaxLongitude));
	storeEntry->SetGeoValueL(*geoValueSet);
	
	CleanupStack::PopAndDestroy(geoValueSet);
	
	// == Store Entry - Externalize to file
	iTestLib->StoreEntryL(*storeEntry);

	TCalLocalUid storeEntryUid = storeEntry->LocalUidL();
	
	CleanupStack::PopAndDestroy(storeEntry);
	
	// Close file
	delete iTestLib;
	iTestLib = NULL;
	
	iTestLib = CCalTestLibrary::NewL(EFalse);
	iTestLib->OpenFileL(KCalName());
	
	// Re-open file - internalize from file
	CCalEntry* fetchEntry = iTestLib->SynCGetEntryViewL().FetchL(storeEntryUid);
	CleanupStack::PushL(fetchEntry); 
	
	// Get and validate the retrieved CCalGeoValue from the fetched entry 
	CCalGeoValue* geoValueGet = fetchEntry->GeoValueL();
	CleanupStack::PushL(geoValueGet);
	TReal geoLat;
	TReal geoLong;
	geoValueGet->GetLatLong(geoLat,geoLong);
	
	test(geoLat==KCalGEOMaxLatitude);
	test(geoLong==KCalGEOMaxLongitude);
	
	CleanupStack::PopAndDestroy(geoValueGet);	
	CleanupStack::PopAndDestroy(fetchEntry);	
	}
	
void CEntryAttributesTestManager::TestGEOCompareAndCopyFromL()
	{
	// Test Compare
	// Create two entries with GEO values and compare them
	// == First entry
	HBufC8* guidFirst = NULL;

	CCalEntry* entryFirst = iTestLib->CreateCalEntryL(CCalEntry::EAppt,guidFirst);
	CleanupStack::PushL(entryFirst);
	
	iTestLib->SetEntryStartAndEndTimeL(entryFirst);
	
	// Set GEO value on the entry
	CCalGeoValue* geoValueFirst = CCalGeoValue::NewL();
	CleanupStack::PushL(geoValueFirst);
	
	geoValueFirst->SetLatLongL(TReal(KCalGEOMaxLatitude),TReal(KCalGEOMaxLongitude));
	entryFirst->SetGeoValueL(*geoValueFirst);
	
	CleanupStack::PopAndDestroy(geoValueFirst);

	// == Second entry
	HBufC8* guidSecond = NULL;

	CCalEntry* entrySecond = iTestLib->CreateCalEntryL(CCalEntry::EAppt,guidSecond);
	CleanupStack::PushL(entrySecond);
	
	iTestLib->SetEntryStartAndEndTimeL(entrySecond, entryFirst->StartTimeL().TimeUtcL(),entryFirst->EndTimeL().TimeUtcL());
	
	// Set GEO value on the entry
	CCalGeoValue* geoValueSecond = CCalGeoValue::NewL();
	CleanupStack::PushL(geoValueSecond);
	
	geoValueSecond->SetLatLongL(TReal(KCalGEOMaxLatitude),TReal(KCalGEOMaxLongitude));
	entrySecond->SetGeoValueL(*geoValueSecond);
	
	CleanupStack::PopAndDestroy(geoValueSecond);

	// == Third entry
	HBufC8* guidThird = NULL;

	CCalEntry* entryThird = iTestLib->CreateCalEntryL(CCalEntry::EAppt,guidThird);
	CleanupStack::PushL(entryThird);
	
	iTestLib->SetEntryStartAndEndTimeL(entryThird, entryFirst->StartTimeL().TimeUtcL(),entryFirst->EndTimeL().TimeUtcL());
	
	// Set GEO value on the entry
	CCalGeoValue* geoValueThird = CCalGeoValue::NewL();
	CleanupStack::PushL(geoValueThird);
	
	geoValueThird->SetLatLongL(TReal(KCalGEOMinLatitude),TReal(KCalGEOMinLongitude));
	entryThird->SetGeoValueL(*geoValueThird);
	
	CleanupStack::PopAndDestroy(geoValueThird);

	test(entryFirst->CompareL(*entrySecond));
	test(!entryFirst->CompareL(*entryThird));

	// Test CopyFrom
	// == Fourth entry
	HBufC8* guidFourth = NULL;

	CCalEntry* entryFourth = iTestLib->CreateCalEntryL(CCalEntry::EAppt,guidFourth);
	CleanupStack::PushL(entryFourth);
	
	entryFourth->CopyFromL(*entrySecond);
	
	test(entryFourth->CompareL(*entrySecond));
	
	CleanupStack::PopAndDestroy(entryFourth);
	CleanupStack::PopAndDestroy(entryThird);
	CleanupStack::PopAndDestroy(entrySecond);	
	CleanupStack::PopAndDestroy(entryFirst);
	}
	
	
void CEntryAttributesTestManager::UpdateEntryHaveRdateOnlyL()
{
	    iTestLib->CleanDatabaseL();
	   	RPointerArray<CCalEntry> calEntryList;
		CleanupResetAndDestroyPushL(calEntryList);

		_LIT8(KUid, "INC099532");
		HBufC8* guid = KUid().AllocLC();

		CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop( guid );
		CleanupStack::PushL(entry);
		TTime startTime = TDateTime( 2006, EOctober, 24, 16, 0, 0, 0 );
		TTime endTime = TDateTime( 2006, EOctober, 24, 17, 0, 0, 0 );

		TTime r1 = TDateTime( 2006, EOctober, 24, 16, 0, 0, 0 );
		TTime r2 = TDateTime( 2006, EOctober, 26, 16, 0, 0, 0 );
		TCalTime r1Cal;
		TCalTime r2Cal;

		r1Cal.SetTimeLocalL( r1 );
		r2Cal.SetTimeLocalL( r2 );

		TCalTime startCalTime;
		TCalTime endCalTime;
		startCalTime.SetTimeLocalL( startTime );
		endCalTime.SetTimeLocalL( endTime );

		calEntryList.AppendL( entry );
		CleanupStack::Pop( entry );
		TInt success(0);

		entry->SetStartAndEndTimeL( startCalTime, endCalTime );

		//Fill RDates
		RArray<TCalTime> rDatesArray;
		CleanupClosePushL( rDatesArray );
		rDatesArray.AppendL( r1Cal );
		rDatesArray.AppendL( r2Cal );
		//Store the entry	
		entry->SetRDatesL( rDatesArray ); 
		iTestLib->SynCGetEntryViewL().StoreL(calEntryList, success);
		CleanupStack::PopAndDestroy( &rDatesArray );
		calEntryList.ResetAndDestroy();	
		//Fetch the entry	
		iTestLib->SynCGetEntryViewL().FetchL(KUid, calEntryList);
		test(calEntryList.Count()==1);
		//Update the first instance
		TCalTime recurrenceId;
		TTime recurrenceIdTime = TDateTime( 2006, EOctober, 24, 0, 0, 0, 0 );
		recurrenceId.SetTimeLocalL(recurrenceIdTime);
		guid = KUid().AllocLC();
		entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0, recurrenceId, CalCommon::EThisOnly);
		CleanupStack::Pop( guid );
		CleanupStack::PushL(entry);
			
		TTime newStartDate (TDateTime(2006, EOctober, 20, 16, 0, 0, 0));
		TTime newEndDate (TDateTime(2006, EOctober, 20, 18, 0, 0, 0));
			
		TCalTime newCalstartTime;
		newCalstartTime.SetTimeUtcL(newStartDate);
			
		TCalTime newCalendTime;
		newCalendTime.SetTimeUtcL(newEndDate);
			
		entry->SetStartAndEndTimeL(newCalstartTime, newCalendTime);
		 
		calEntryList.AppendL(entry);
		CleanupStack::Pop( entry );
		iTestLib->SynCGetEntryViewL().StoreL(calEntryList, success);
		CleanupStack::PopAndDestroy( &calEntryList );
			
		CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
			
		TCalTime minTime;
		minTime.SetTimeUtcL(TCalTime::MinTime());
		TCalTime maxTime;
		maxTime.SetTimeUtcL(TCalTime::MaxTime());
		CalCommon::TCalTimeRange timeRange(minTime, maxTime);
			
		RPointerArray<CCalInstance> instances; 
		CleanupResetAndDestroyPushL(instances);
			
		iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, filter, timeRange);
		TInt count = instances.Count();
		TBuf<100> tempBuf;	
		_LIT(KStartDateFormat,"Start Time: %H:%T:%S:%*C3 Date:%*1/%2/%3");
		_LIT(KEndDateFormat,"End Time: %H:%T:%S:%*C3 Date:%*1/%2/%3");

		test(count==2);
		for (TInt i=0;i<count;i++)
			{
				TCalTime instancesStartTime = instances[i]->StartTimeL();	
				instancesStartTime.TimeLocalL().FormatL(tempBuf,KStartDateFormat);
				RDebug::Print(_L("%S\n\n"), &tempBuf);
				TCalTime instancesEndTime = instances[i]->EndTimeL();
				instancesEndTime.TimeLocalL().FormatL(tempBuf,KEndDateFormat);
				RDebug::Print(_L("%S\n\n"), &tempBuf);
				}
	CleanupStack::PopAndDestroy( &instances );
}
void CEntryAttributesTestManager::TestStartDateTestL()
	{
	_LIT8(KGuid, "guid-startdate");
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	const TTime KOriginalStart(TDateTime(2006, ENovember, 12, 9, 0, 0, 0)); // 0900 on 13 Nov (Monday)
	const TTime KModifiedStart(TDateTime(2006, ENovember, 13, 11, 0, 0, 0)); // 1100 on 14 Nov (Tuesday)
	
	TCalTime startTime;
	startTime.SetTimeLocalL(KOriginalStart);
	entry->SetStartAndEndTimeL(startTime, startTime);
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(startTime);
	rule.SetCount(10);
	rule.SetInterval(7);
	entry->SetRRuleL(rule);
	iTestLib->StoreEntryL(*entry);
	TCalLocalUid luid = entry->LocalUidL();
	CleanupStack::PopAndDestroy(entry);
	//
	
	entry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(entry);
	
	TCalTime modifiedStartTime;
	modifiedStartTime.SetTimeLocalL(KModifiedStart);
	rule.SetDtStart(modifiedStartTime);
	rule.SetCount(10);
	entry->SetRRuleL(rule);
	iTestLib->StoreEntryL(*entry);
	
	const TTime KExpectedNewEntryStart(TDateTime(2006, ENovember, 13, 9, 0, 0, 0)); // 1100 on 14 Nov (Tuesday)
 	TDateTime dt = entry->StartTimeL().TimeLocalL().DateTime();
	test(KExpectedNewEntryStart == dt);
	CleanupStack::PopAndDestroy(entry);
	}
	
void CEntryAttributesTestManager::TestStoreEntryWithTzL()
	{
	iTestLib->CleanDatabaseL();
	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
//create a repeating entry (parent) and store it to the file
	_LIT8(KUid, "UID_EntryWithTz");
	HBufC8* guid = KUid().AllocLC();
	CCalEntry*	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entriesToStore.AppendL(entry);
	CleanupStack::Pop(entry);

	TTime startTime (TDateTime(2007, EFebruary, 1, 23, 30, 0, 0)); 
	TTime endTime (TDateTime(2007, EFebruary, 2, 0, 30, 0, 0)); 
	TTime rptendTime(TDateTime(2100, EDecember, 29, 0, 30, 0, 0));
	
	TCalTime calStartTime;
	calStartTime.SetTimeUtcL(startTime);
	TCalTime calEndTime;
	calEndTime.SetTimeUtcL(endTime);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	TCalRRule rpt = TCalRRule(TCalRRule::EWeekly);

	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(ESaturday);
	rpt.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	
	TCalTime rptendTimeCal;
	rptendTimeCal.SetTimeUtcL(rptendTime);

	rpt.SetDtStart(calStartTime);
	rpt.SetUntil(rptendTimeCal);
	rpt.SetInterval(1);
	entry->SetRRuleL(rpt);//take a copy of the rule


	RTz tz;
	User::LeaveIfError(tz.Connect());
	CleanupClosePushL(tz);
	_LIT8(KBeijing, "Asia/Hong_Kong");
	CTzId* tzId = CTzId::NewL(KBeijing);
	CleanupStack::PushL(tzId);
	CTzRules* tzBeijing = tz.GetTimeZoneRulesL(*tzId, startTime, rptendTime, ETzUtcTimeReference);
	CleanupStack::PopAndDestroy(tzId);
	CleanupStack::PushL(tzBeijing);
	entry->SetTzRulesL(*tzBeijing);

	CleanupStack::PopAndDestroy(tzBeijing);
	CleanupStack::PopAndDestroy(&tz);
	
	TInt entriesStored;
	iTestLib->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesStored ==1);
	entriesToStore.ResetAndDestroy();
	
	iTestLib->SynCGetEntryViewL().FetchL(KUid, entriesToStore);
	test(entriesToStore.Count() == 1);
	//Check the first instance datetime
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;

	const TTime KFirstDayWithInstance(TDateTime(2007, EFebruary, 1, 23, 30, 0, 0));

	TCalTime start;
	start.SetTimeLocalL(TCalTime::MinTime());
	TCalTime instanceFind = iTestLib->SynCGetInstanceViewL().NextInstanceL(filter, start);
	TTime instanceFindTTime = instanceFind.TimeLocalL();
	//The first instance date should be found

	RDebug::Print(_L("Min time NextInstanceL"));
	TBuf<100> tempBuf;	
	_LIT(KTTimeDateFormat,"Time: %H:%T:%S:%*C3 Date:%*1/%2/%3");
	instanceFindTTime.FormatL(tempBuf,KTTimeDateFormat);
	RDebug::Print(_L("%S\n\n"), &tempBuf);
	
	test (instanceFindTTime == KFirstDayWithInstance);
	CleanupStack::PopAndDestroy(&entriesToStore);
	}


void CEntryAttributesTestManager::StoreAndExportL()
	{
	//Defect PDEF111565 
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	_LIT8(KGUID, "108188");
	HBufC8* guid = KGUID().AllocLC();
		
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	
	TTime startTime = TDateTime(2007, EJune, 3, 07, 0, 0, 0);
	TTime endTime = TDateTime(2007, EJune, 3, 07, 30, 0, 0);
	TTime untilTime = TDateTime(2007, EJune, 10, 07, 0, 0, 0);
 
	TCalTime startCalTime;
	TCalTime endCalTime;
	TCalTime untilCalTime;
	startCalTime.SetTimeUtcL(startTime);
	endCalTime.SetTimeUtcL(endTime);
	untilCalTime.SetTimeUtcL(untilTime);
	
	entry->SetStartAndEndTimeL(startCalTime, endCalTime);
	entry->SetSummaryL(_L("Weekly"));

	TCalRRule weekly(TCalRRule::EWeekly);
	weekly.SetDtStart(startCalTime);
	weekly.SetUntil(untilCalTime);
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EMonday);
	weekly.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	entry->SetRRuleL(weekly);
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	entry->SetExceptionDatesL(exceptions);//Set an empty exceptions that shouldn't set SetHasExceptions(ETrue)
	CleanupStack::PopAndDestroy(&exceptions);
	
	TInt numStored = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, numStored);
	test(numStored == 1);
	
	entries.ResetAndDestroy();
	iTestLib->SynCGetEntryViewL().FetchL(KGUID(),entries);
	
	_LIT(KExportFile, "tcal_entry_108188.vcs");
	iTestLib->ExportL(KExportFile, entries);
	
	CleanupStack::PopAndDestroy(&entries);
	}


void CEntryAttributesTestManager::TestRdateL()
	{
	const TDesC8& uid1 = KGUID1();
	HBufC8* guid = uid1.AllocLC();
		
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TTime startTime = TDateTime(2007, EJuly, 01, 16, 0, 0, 0);
	TTime endTime = TDateTime(2007, EJuly, 01, 17, 0, 0, 0);
	TTime untilTime = TDateTime(2007, EAugust, 05, 16, 0, 0, 0);
 
	TTime rdate1 = TDateTime(2007, EJuly, 15, 16, 0, 0, 0);
	TTime rdate2 = TDateTime(2007, EJune, 17, 16, 0, 0, 0);
	
	// Fill RDates
	TCalTime calRdate1; // 16/07/2007 @ 16:00 UTC
	TCalTime calRdate2; // 18/06/2007 @ 16:00 UTC
	
	calRdate1.SetTimeUtcL(rdate1);
	calRdate2.SetTimeUtcL(rdate2);
	
	TCalTime startCalTime, endCalTime, untilCalTime;
	startCalTime.SetTimeUtcL(startTime);
	endCalTime.SetTimeUtcL(endTime);
	untilCalTime.SetTimeUtcL(untilTime);
	
	entry->SetStartAndEndTimeL(startCalTime, endCalTime);
	entry->SetSummaryL(_L("Weekly"));
	entry->SetLocationL(_L("Dallas"));

	TCalRRule rptRule;
	
	rptRule.SetDtStart(startCalTime);
	rptRule.SetType(TCalRRule::EMonthly);
	
	RArray<TCalRRule::TDayOfMonth> byDayArray;
	CleanupClosePushL(byDayArray);
	TInt8 weekInMonth = 1;
	TDay day = EMonday;
	
	TCalRRule::TDayOfMonth byDay(day, weekInMonth);
	byDayArray.Append(byDay);
	
	rptRule.SetByDay(byDayArray);
	CleanupStack::PopAndDestroy(&byDayArray); 
	
	rptRule.SetUntil(untilCalTime);	 // 06/08/2007 @ 16:00 UTC
	rptRule.SetInterval(1);   // In 1 month period
	entry->SetRRuleL(rptRule);  // 1.st monday of the month
	
	RArray<TCalTime> rDatesArray;
	CleanupClosePushL(rDatesArray);
	rDatesArray.AppendL(calRdate1);
	rDatesArray.AppendL(calRdate2);
	
	entry->SetRDatesL(rDatesArray); 
	
	CleanupStack::PopAndDestroy(&rDatesArray);

	RPointerArray<CCalEntry> entries;
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	CleanupResetAndDestroyPushL(entries);
	TInt numStored = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, numStored);
	test(numStored == 1);
	
	CleanupStack::PopAndDestroy(&entries);

	//------------------------------
	// Find all entries.
	//------------------------------
	TDateTime startDateTime(rdate2.DateTime());
	TDateTime endDateTime(rdate2.DateTime());

	startDateTime.SetHour(0);
	startDateTime.SetMinute(0);
	startDateTime.SetSecond(0);
	startDateTime.SetMicroSecond(0);

	endDateTime.SetHour(23);
	endDateTime.SetMinute(59);
	endDateTime.SetSecond(59);
	endDateTime.SetMicroSecond(0);

	TCalTime endDate;
	endDate.SetTimeLocalL(TTime(endDateTime));

	TCalTime startDate;
	startDate.SetTimeLocalL(TTime(startDateTime));

	CalCommon::TCalTimeRange dayrange(startDate, endDate);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;

	RPointerArray<CCalInstance> instancesFound;
	CleanupResetAndDestroyPushL(instancesFound);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instancesFound, filter, dayrange);

	for (TInt i = 0; i < instancesFound.Count(); i++)
		{
		TDateTime dt = instancesFound[i]->Time().TimeUtcL().DateTime();
		test.Printf(_L("Found instance on %d: %d/%d/%d"), i, 
			dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour());
		}
	
	test(instancesFound.Count() == 1);
	test(instancesFound[0]->Time().TimeUtcL() == rdate2);
	CleanupStack::PopAndDestroy(&instancesFound);
	}

void CEntryAttributesTestManager::GetInstanceCrossDstL()
	{
	iTestLib->CleanDatabaseL();
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	_LIT8(KUid,"nonExistTime_guid");
	HBufC8* guid = KUid().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	TCalTime nonExistStartTime;
	TCalTime nonExistEndTime;
	nonExistStartTime.SetTimeLocalL(TDateTime(2007, EMarch, 10, 1, 1, 0, 0));
	nonExistEndTime.SetTimeLocalL(TDateTime(2007, EMarch, 10, 2, 1, 0, 0));
	entry->SetStartAndEndTimeL(nonExistStartTime, nonExistEndTime);
	
	TCalRRule rpt(TCalRRule::EDaily);
	rpt.SetDtStart(nonExistStartTime);
	rpt.SetInterval(1);
	rpt.SetCount(16);
	entry->SetRRuleL(rpt);//take a copy of the rule
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	TInt num;
	iTestLib->SynCGetEntryViewL().StoreL(entries, num);
	FindInstancesL(16);
	entries.ResetAndDestroy();
	iTestLib->CleanDatabaseL();
	FindInstancesL(0);
	guid = KUid().AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	nonExistStartTime.SetTimeLocalL(TDateTime(2007, EMarch, 23, 2, 0, 0, 0));
	nonExistEndTime.SetTimeLocalL(TDateTime(2007, EMarch, 23, 2, 0, 0, 0));
	entry->SetStartAndEndTimeL(nonExistStartTime, nonExistEndTime);
	rpt.SetDtStart(nonExistStartTime);
	rpt.SetCount(3);
	entry->SetRRuleL(rpt);
	
	entries.AppendL(entry);
	CleanupStack::Pop(entry);
	iTestLib->SynCGetEntryViewL().StoreL(entries, num);
	FindInstancesL(3);

	CleanupStack::PopAndDestroy(&entries);
	iTestLib->CleanDatabaseL();
//	
	}

void CEntryAttributesTestManager::FindInstancesL(TInt aExpectedNumInstances)
	{
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime minTime;
	minTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(minTime, maxTime);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceList,filter,timeRange);
	TInt count = instanceList.Count();
	for(TInt i=0;i<instanceList.Count();i++)
		{
		TCalTime instanceStart = instanceList[i]->StartTimeL();
		TCalTime instanceEnd = instanceList[i]->EndTimeL();
		TTime instanceStartTTime = instanceStart.TimeLocalL();
		TTime instanceEndTTime = instanceEnd.TimeLocalL();
		RDebug::Print(_L("Next Instance start time"));
		TBuf<100> tempBuf;	
		_LIT(KTTimeDateFormat,"Time: %H:%T:%S:%*C3 Date:%*1/%2/%3");
		instanceStartTTime.FormatL(tempBuf,KTTimeDateFormat);
		RDebug::Print(_L("%S\n\n"), &tempBuf);
		instanceEndTTime.FormatL(tempBuf,KTTimeDateFormat);
		RDebug::Print(_L("%S\n\n"), &tempBuf);
		}
	
	test(aExpectedNumInstances == instanceList.Count());
	CleanupStack::PopAndDestroy(&instanceList);
		
	}


/** This testcase has been added to validate the fix for PDEF115349. 
The defect was reported on yearly repeating appointments with a 
repeat forever on 9.3. The time zone is changed to one with positive offset 
like Beijing or Sydney. An appointment is added with a repeat of yearly
on day. On editing the appointment, the repeat type is selected same as 
default entry's. The 'forever' option should be marked. 
*/
void CEntryAttributesTestManager::TestEndTimeForeverL()
	{	
	test.Printf(_L("\nAdding an appointment which repeats forever"));
	TTime startTime(TDateTime(2007, EDecember, 0, 12, 0, 0, 0)); 
	TTime endTime(TCalTime::MaxTime()); 
	
	RTz tz;
	User::LeaveIfError(tz.Connect());
	CleanupClosePushL(tz);
	
	test.Printf(_L("Setting the time zone to Sydney"));
	iTestLib->SetTimeZoneL(_L8("Australia/Sydney"));
	_LIT8(KGUID, "Repeat Forever");
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);

	TCalTime startTimeCal;
	startTimeCal.SetTimeLocalL(startTime);
	TCalTime endTimeCal;
	endTimeCal.SetTimeLocalL(endTime);
	entry->SetStartAndEndTimeL(startTimeCal, endTimeCal);

	TCalRRule yearly(TCalRRule::EYearly);
	yearly.SetDtStart(startTimeCal);
	yearly.SetUntil(endTimeCal);
	
	TCalRRule::TDayOfMonth dayOfMonth(EThursday, 2);
	RArray<TCalRRule::TDayOfMonth> dayArray;
	dayArray.AppendL(dayOfMonth);
	yearly.SetByDay(dayArray);
	dayArray.Reset();
		
	RArray<TMonth> monthArray;
	monthArray.AppendL(EDecember);
	yearly.SetByMonth(monthArray);
	monthArray.Reset();
	
	entry->SetRRuleL(yearly);
	
	RPointerArray<CCalEntry> entries;
	TInt success(0);
	entries.AppendL(entry);
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == 1);
	entries.ResetAndDestroy();
	
	iTestLib->SynCGetEntryViewL().FetchL(KGUID, entries);
	
	test.Printf(_L("Editing the entry. Storing the same details as before"));
	
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == 1);
	
	TCalRRule rptRule;
	
	test(entries[0]->GetRRuleL(rptRule)); //Get the repeat rules of the entry
	TCalTime rptEndDate = rptRule.Until(); //Check the end date of the entry
	
	entries.ResetAndDestroy();
	
	TTime rptEndDateAsTTime = rptEndDate.TimeLocalL();
	
	test(rptEndDateAsTTime == TCalTime::MaxTime()); // Is the end time maxtime?
	test.Printf(_L("The entry repeats forever. Test is successful\n")); // Entry repeats forever
		
	iTestLib->SetTimeZoneL(KDefaultTimeZone);
	
	CleanupStack::PopAndDestroy(&tz);
	entries.ResetAndDestroy();
	}

	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CEntryAttributesTestManager* testManager = CEntryAttributesTestManager::NewLC();


	TPerformanceTimer timer(test);
	timer.Start();	
	

	// Run the test suite
	testManager->TestFetchAndDeleteNonExistentEntryL();

	testManager->CreateEntriesL();
	testManager->TestLiteEntriesL();
	
	/*
	 @SYMTestCaseID PIM-TCAL-ENTRYATTRIBUTES-Set_Get_Floating_Appointment-0001
	 */
	/*
	@SYMTestCaseID PIM-TCAL-ENTRYATTRIBUTES-Set_Get_Floating_Todo-0001
	 */
	/*
	@SYMTestCaseID PIM-TCAL-ENTRYATTRIBUTES-Set_Get_Floating_Event-0001
	 */
	/*
	@SYMTestCaseID PIM-TCAL-ENTRYATTRIBUTES-Set_Get_Floating_Anniversary-0001
	@SYMTestCaseDesc Set and get time for floating entries
	@SYMFssID App-Engines/CalInterimAPI/Data.007
	@SYMFssID App-Engines/CalInterimAPI/Data.009
	@SYMTestStatus Implemented
	@SYMTestPriority Medium
	@SYMTestActions Create floating appointment, todo, event and anniversary. Store into CalInterimAPI. 
	 				Get the lite entries to check that the time and time mode are the same. 
					Then get the full entries to check that the time and time mode are the same.
	@SYMTestExpectedResults The check shows that the time and time mode are as expected
	@SYMTestType CT	
	*/
	testManager->CreateEntriesL(ETrue);
	testManager->TestLiteEntriesL();
	testManager->TestLastChangedDateL();	
	testManager->TestCopySummaryAndDescriptionL();
	testManager->TestAlarmedCompletedTodoL();
	testManager->FindSummaryL();
	testManager->TestEndTimeBeforeStartTimeL();
	testManager->TestEndTimeNullL();
	testManager->TestEndTimeForeverL();
	testManager->TestAlarmedTodoWithoutDueDateL();
	
	// from former tcal_CopyEntryImpl

	testManager->StoreAndExportL();
	
	testManager->CreateEntriesForCopyL();
	testManager->CreateEventForCopyPasteL();
	testManager->AddTwoRepeatingChildL();	
	testManager->TestGEOAttributesL();	
	testManager->UpdateEntryHaveRdateOnlyL();
	testManager->TestStartDateTestL();
	testManager->TestStoreEntryWithTzL();
	testManager->TestRdateL();
	testManager->GetInstanceCrossDstL();
	testManager->TestDeleteInstancesL(0);
	testManager->TestDeleteInstancesL(1);
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();
	
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-ENTRYATTRIBUTES-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-ENTRYATTRIBUTES-0001 Calendar Interim API Entry Attributes test suite"));

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
