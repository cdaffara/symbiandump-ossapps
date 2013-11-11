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

// Local includes
#include "caltestlib.h"
#include <calentry.h>
#include <calentryview.h>
#include <calalarm.h>
#include <calcategory.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <calrrule.h>

// System includes
#include <e32test.h>
#include <s32file.h>
#include <asshdalarm.h>
#include <asshddefs.h>
#include <asclisession.h>

LOCAL_D RTest test(_L("tCal_alarmattach"));

_LIT8(KGUID3, "GUID_TestDeleteSnoozedAlarmL"); /* do not use it for other tests */
class CCalAlarmAttachTest : public CBase
	{
public:
	static CCalAlarmAttachTest* NewLC();
 	void DoTestL();
	~CCalAlarmAttachTest();


	void TestDeleteSnoozedAlarmL() ;
	void TestDeleteAlarmL() ;
	
	void TestFetchEntryAlarmContentL();
	void TestDeleteAlarmWithAlarmContentL();
	void TestCopyEntryWithAlarmContentL();
	void TestCopyTwiceEntryWithAlarmContentL();
    void TestUpdateEntryHavingAlarmcontentL();
		
private:
	void ConstructL();
	void CreateEntriesL();
	void TestAlarmL( CCalEntry* aEntry, const TDesC8& expectedContent,
        const TDesC8& expectedMimeType );
	void FetchEntryL( const TDesC8& expectedContent, 
        const TDesC8& expectedMimeType );
	void ClearAllAlarmsL();
	TInt CountAlarmsL(TAlarmId& aFirstAlarmId);
	CCalAlarm* StoreEntryWithAlarmContentLC(const TDesC8& aUid);


private:
	CCalTestLibrary* iTestLib;
	RASCliSession		iAlarmServer;
	};
	

_LIT8(KGUID1, "A_Global_UID1");


_LIT(KCalName, "TestCalendar");

_LIT8(KContent, "C:\\test.jpg");
_LIT8(KMimeType, "mime type");
_LIT(KSummary, "Summary");
_LIT(KDescription, "Description");



/**
* Tests behaviour of the alarm attachments.
*
* Create 3 entries, a parent, and two child entries. The first child entry has
* an alarm attachment and stores the entry. The second child also has an 
* attachment and stores the entry. The second child's alarm attachment data 
* should overwrite the first child's alarm data.
*/
void CCalAlarmAttachTest::CreateEntriesL()
	{
    //-----------------------------------------------------------------------
	// Create parent entry with repeat rule
    //-----------------------------------------------------------------------
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	HBufC8* guid = KGUID1().AllocLC();
	CCalEntry* parentEntry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(parentEntry);
	
	TCalTime calTime1;
	calTime1.SetTimeUtcL(TDateTime(2007,EFebruary,15, 13, 30, 0, 0));
	TCalTime calTime2;
	calTime2.SetTimeUtcL(TDateTime(2007,EFebruary,15, 14, 30, 0, 0));
	parentEntry->SetStartAndEndTimeL(calTime1, calTime2);
	
	parentEntry->SetSummaryL(_L("Test for Alarms"));
	
	TCalRRule rptRule;
    rptRule.SetDtStart( calTime1 );
    rptRule.SetType( TCalRRule::EDaily );
    rptRule.SetInterval( 1 );
    rptRule.SetCount(5);
	
	parentEntry->SetRRuleL(rptRule);

	entries.AppendL(parentEntry);
   
    // Store parent entry.
	TInt entriesStored = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
	
	CleanupStack::Pop(parentEntry);
	CleanupStack::PopAndDestroy(&entries);
	
    //-----------------------------------------------------------------------
	// Create child entry with recurrence id on 17th and with alarm attachment.
    //-----------------------------------------------------------------------
	
	RPointerArray<CCalEntry> entriesToStore1;
	CleanupResetAndDestroyPushL(entriesToStore1);
	
	TCalTime recurrenceId;
	recurrenceId.SetTimeUtcL(TDateTime(2007,EFebruary,17, 13, 30, 0, 0));
	
	HBufC8* parentGuid = KGUID1().AllocLC();
	CCalEntry* childEntry1 = CCalEntry::NewL(CCalEntry::EEvent, parentGuid, 
        CCalEntry::EMethodNone, 0, recurrenceId, CalCommon::EThisOnly);
	CleanupStack::Pop(parentGuid);
	CleanupStack::PushL(childEntry1);
	
	TCalTime calTime3;
	calTime3.SetTimeUtcL(TDateTime(2007,EFebruary,21, 13, 30, 0, 0));
	TCalTime calTime4;
	calTime4.SetTimeUtcL(TDateTime(2007,EFebruary,21, 14, 30, 0, 0));
	childEntry1->SetStartAndEndTimeL(calTime3, calTime4);
	
	// Add an attachment to the alarm.
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(1);
	
	CCalContent* almContent = CCalContent::NewL();
	CleanupStack::PushL(almContent);
	// Add content and mimetype for the alarm.
	_LIT8(KContent, "C:\\test.jpg");
	_LIT8(KMimeType, "mime type");
	HBufC8* content = KContent().AllocLC();
	HBufC8* mimetype = KMimeType().AllocLC();
	
    // Takes ownership of content and mimetype.
	almContent->SetContentL(content, mimetype, CCalContent::EDispositionInline);
	alarm->SetAlarmAction(almContent); // Takes ownership of almContent.
	
	childEntry1->SetAlarmL(alarm);
	
	CleanupStack::Pop(mimetype);
	CleanupStack::Pop(content);    
	CleanupStack::Pop(almContent); 
	CleanupStack::PopAndDestroy(alarm);
		
	TInt entriesStored1 = 0;
	entriesToStore1.AppendL(childEntry1);
	iTestLib->SynCGetEntryViewL().StoreL(entriesToStore1, entriesStored1);
	CleanupStack::Pop(childEntry1);
		
    // Test if the alarm details retrieved are as expected.
	CCalEntry* entry1 = entriesToStore1[0];
	TestAlarmL(entry1, KContent(), KMimeType());

    // Fetch the entry and ensure the alarm details are still as expected.
    FetchEntryL( KContent(), KMimeType() );

	CleanupStack::PopAndDestroy(&entriesToStore1);

    //-----------------------------------------------------------------------
	// Create another child entry with recurrence id and range same as above. 
    // The entry should be updated.
    //-----------------------------------------------------------------------

	RPointerArray<CCalEntry>	entriesToStore2;
	CleanupResetAndDestroyPushL(entriesToStore2);
	
	HBufC8* parentGuid1 = KGUID1().AllocLC();
	
	TCalTime recurrenceId1;
	recurrenceId1.SetTimeUtcL(TDateTime(2007,EFebruary,17, 13, 30, 0, 0));
	
	CCalEntry* childEntry2 = CCalEntry::NewL(CCalEntry::EEvent, parentGuid1, 
        CCalEntry::EMethodNone, 0, recurrenceId1, CalCommon::EThisOnly);
	CleanupStack::Pop(parentGuid1);
	CleanupStack::PushL(childEntry2);
	
	TCalTime calTime5;
	calTime5.SetTimeUtcL(TDateTime(2007,EFebruary,22, 13, 30, 0, 0));
	TCalTime calTime6;
	calTime6.SetTimeUtcL(TDateTime(2007,EFebruary,22, 14, 30, 0, 0));
	childEntry2->SetStartAndEndTimeL(calTime5, calTime6);
	
	childEntry2->SetSummaryL(_L("Update the child entry1"));
	childEntry2->SetPriorityL(2);
	
	// Add attachment to the alarm.
	CCalAlarm* alarm1 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm1);
	alarm1->SetTimeOffset(1);
	
	CCalContent* almContent1 = CCalContent::NewL();
	CleanupStack::PushL(almContent1);

	// Add alarm attachment to the entry.
	_LIT8(KContent1, "C:\\longtail.jpg");
    _LIT8(KMimeType1, "mime type 2");
	HBufC8* content1 = KContent1().AllocLC();
	HBufC8* mimetype1 = KMimeType1().AllocLC();
	
	almContent1->SetContentL(content1, mimetype1, CCalContent::EDispositionInline);
	alarm1->SetAlarmAction(almContent1);
	
	childEntry2->SetAlarmL(alarm1);
	
	CleanupStack::Pop(mimetype1);
	CleanupStack::Pop(content1);
	CleanupStack::Pop(almContent1);
	CleanupStack::PopAndDestroy(alarm1);
	
	entriesToStore2.AppendL(childEntry2);
	
    // Store the child entry.
	TInt entriesStored2 = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entriesToStore2, entriesStored2);

    // Store the entry again. The stream id for the alarm action will be 
    // re-used (in CAgnEntry). RAgendaServ::UpdateEntryL() will call 
    // CAgnEntry::UpdateAlarmAction() instead of StoreAlarmAction().
	iTestLib->SynCGetEntryViewL().StoreL(entriesToStore2, entriesStored2);

	CleanupStack::Pop(childEntry2);
	
	CCalEntry* entry2 = entriesToStore2[0];

	// Test if the alarm details are retrieved as expected. 
	TestAlarmL(entry2, KContent1(), KMimeType1());

    // Fetch the entry and ensure the alarm details are still as expected.
    FetchEntryL( KContent1(), KMimeType1() );

	CleanupStack::PopAndDestroy(&entriesToStore2);
	}

/**
* Test the alarm properties of the entry that has alarm content.
*
* @param aEntry The calendar entry to test.
* @param expectedContent The expected content in aEntry.
* @param expectedMimeType The expected mime type in aEntry.
*/	
void CCalAlarmAttachTest::TestAlarmL( CCalEntry* aEntry, 
    const TDesC8& expectedContent, const TDesC8& expectedMimeType )
	{
	CCalAlarm* almEntry = aEntry->AlarmL();
	CleanupStack::PushL(almEntry);
    test( almEntry != NULL );

	CCalContent* alarmcontent1 = almEntry->AlarmAction();
    test( alarmcontent1 != NULL );

	const TDesC8& content = alarmcontent1->Content();
    test( content == expectedContent );
	const TDesC8& mimetype = alarmcontent1->MimeType();
    test( mimetype == expectedMimeType );
    
    CleanupStack::PopAndDestroy(almEntry);
	}

/**
* Fetches all entries and checks the alarm properties for the child entry.
* There should be two entries, a parent and the child entry.
*
* @param expectedContent The expected content of the child entry.
* @param expectedMimeType The expected mime type of the child entry.
*/
void CCalAlarmAttachTest::FetchEntryL( const TDesC8& expectedContent,
        const TDesC8& expectedMimeType )
    {
	RPointerArray<CCalEntry> entriesFetched;
	CleanupResetAndDestroyPushL(entriesFetched);
	HBufC8* parentGuid1 = KGUID1().AllocLC();
	iTestLib->SynCGetEntryViewL().FetchL(parentGuid1->Des(), entriesFetched);
	
    // 1 parent and 1 child entry.
	test(entriesFetched.Count() == 2); 
	
    // The child entry should be the second entry.
	CCalEntry* childEntry = entriesFetched[1];
	
	TPtrC summary = childEntry->SummaryL();

	// Test if the alarm details are retrieved as expected. 
	TestAlarmL(childEntry, expectedContent, expectedMimeType);

    CleanupStack::PopAndDestroy(parentGuid1);
	CleanupStack::PopAndDestroy(&entriesFetched);
	}
	
void CCalAlarmAttachTest::ClearAllAlarmsL()
	{
	iTestLib->CleanDatabaseL();
	
	// Delete all alarms
	RArray<TAlarmCategory> categories;
	CleanupClosePushL(categories);
	
	iAlarmServer.GetAvailableCategoryListL(categories);
	
	TInt count = categories.Count();
	for(TInt i = 0; i < count; i++)
		{
		const TAlarmCategory& category = categories[i];
		test(iAlarmServer.AlarmDeleteAllByCategory(category, EFalse) == KErrNone);
		}
		
	CleanupStack::PopAndDestroy(&categories);

	// Restore alarm sound
	iAlarmServer.SetAlarmSoundState(EAlarmGlobalSoundStateOn);
	}
	
void CCalAlarmAttachTest::DoTestL()
	{
	CreateEntriesL();
	}

CCalAlarmAttachTest* CCalAlarmAttachTest::NewLC()
	{
	CCalAlarmAttachTest* self = new (ELeave) CCalAlarmAttachTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CCalAlarmAttachTest::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KCalName());
	iTestLib->OpenFileL(KCalName());
	
	User::LeaveIfError(iAlarmServer.Connect());
	}
	
CCalAlarmAttachTest::~CCalAlarmAttachTest()
	{
	iAlarmServer.Close();
	delete iTestLib;
	}

static void PrintEventDetails(TInt aEvent)
	{
	switch(aEvent)
		{
	case EAlarmChangeEventState:
		test.Printf(_L("\tEvent => EAlarmChangeEventState\n"));
		break;
	case EAlarmChangeEventStatus:
		test.Printf(_L("\tEvent => EAlarmChangeEventStatus\n"));
		break;
	case EAlarmChangeEventCharacteristics:
		test.Printf(_L("\tEvent => EAlarmChangeEventCharacteristics\n"));
		break;
	case EAlarmChangeEventAlarmDeletion:
		test.Printf(_L("\tEvent => EAlarmChangeEventAlarmDeletion\n"));
		break;
	case EAlarmChangeEventAlarmAddition:
		test.Printf(_L("\tEvent => EAlarmChangeEventAlarmAddition\n"));
		break;
	case EAlarmChangeEventTimerExpired:
		test.Printf(_L("\tEvent => EAlarmChangeEventTimerExpired\n"));
		break;
	case EAlarmChangeEventSoundPlaying:
		test.Printf(_L("\tEvent => EAlarmChangeEventSoundPlaying\n"));
		break;
	case EAlarmChangeEventSoundStopped:
		test.Printf(_L("\tEvent => EAlarmChangeEventSoundStopped\n"));
		break;
	case EAlarmChangeEventPlayIntervalsChanged:
		test.Printf(_L("\tEvent => EAlarmChangeEventPlayIntervalsChanged\n"));
		break;
	case EAlarmChangeEventGlobalSoundStateChanged:
		test.Printf(_L("\tEvent => EAlarmChangeEventGlobalSoundStateChanged\n"));
		break;
	case EAlarmChangeEventHeadQueueItemChanged:
		test.Printf(_L("\tEvent => EAlarmChangeEventHeadQueueItemChanged\n"));
		break;
	case EAlarmChangeEventUndefined:
		test.Printf(_L("\tEvent => EAlarmChangeEventUndefined\n"));
		break;
	case EAlarmChangeEventAlarmUIVisible:
		test.Printf(_L("\tEvent => EAlarmChangeEventAlarmUIVisible\n"));	
		break;
	case EAlarmChangeEventAlarmUIInvisible:
		test.Printf(_L("\tEvent => EAlarmChangeEventAlarmUIInvisible\n"));	
		break;		
	case EAlarmChangeEventAlarmData:
		test.Printf(_L("\tEvent => EAlarmChangeEventAlarmData\n"));	
		break;			
	default:
		test.Printf(_L("\tEvent => Opss %d?\n"), aEvent);	
		break;
		}
	}

TInt CCalAlarmAttachTest::CountAlarmsL(TAlarmId& aFirstAlarmId)
	{
	RArray< TAlarmId > alarmIdArr;
	
	iAlarmServer.GetAlarmIdListL(alarmIdArr);
	
	const TInt KCount = alarmIdArr.Count();
	if (KCount > 0)
		{
		aFirstAlarmId = alarmIdArr[0];
		}
	TASShdAlarm alarmDetails;
	for (TInt i = 0; i < KCount; i++)
		{
		User::LeaveIfError(iAlarmServer.GetAlarmDetails(alarmIdArr[i], alarmDetails));
		RDebug::Print(_L("alarm %d, state=%d, id=%d"), i, alarmDetails.State(), alarmIdArr[i]);
		}
	RDebug::Print(_L("alarm count: %d"), KCount);
	alarmIdArr.Close();
	
	return KCount;
	}

void CCalAlarmAttachTest::TestDeleteSnoozedAlarmL()
	{
	
	CCalEntryView &view = iTestLib->SynCGetEntryViewL() ;
	CCalEntryView *calentryview = &view ;
	
	TInt num ;	

	ClearAllAlarmsL() ;	
	
	test.Printf(_L("Going to check deletion of snoozed alarms\n")) ;
	
	/* create repeating parent entry	*/

	HBufC8* guid = KGUID3().AllocLC(); // ownership of guid gets transferred
 	CCalEntry *entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0) ;
	CleanupStack::Pop(guid);	
	CleanupStack::PushL(entry) ;
	
	TTime now1;
	now1.UniversalTime();
	now1 += TTimeIntervalSeconds(30);

	TCalTime startTime ;
	TCalTime endTime ;	
	startTime.SetTimeUtcL(now1) ;
	endTime.SetTimeUtcL(now1 + TTimeIntervalSeconds(30)) ;
			
	entry->SetStartAndEndTimeL(startTime, endTime) ;

	TCalRRule rrule(TCalRRule::EDaily) ;
	rrule.SetDtStart(startTime);
	
	TCalTime endRTime ;		
	endRTime.SetTimeUtcL(now1 + TTimeIntervalDays(7)) ;	
	rrule.SetUntil(endRTime);
	rrule.SetInterval(1);	
	entry->SetRRuleL(rrule) ;

	CCalAlarm *alarm = CCalAlarm::NewL() ;
	CleanupStack::PushL(alarm) ;			
	alarm->SetTimeOffset(TTimeIntervalMinutes(0)) ;
	entry->SetAlarmL(alarm) ; //ownership does not get transferred
	
	RPointerArray<CCalEntry> entryarr ;		
	entryarr.AppendL(entry) ; //ownership does not got transferred			
	calentryview->StoreL(entryarr, num) ;
	entryarr.Close() ;		
		
	CleanupStack::PopAndDestroy(alarm) ;	
	CleanupStack::PopAndDestroy(entry) ;	
	
	/* Now let the alarm go off */
	TAlarmId alarmId;
	TInt cnt = CountAlarmsL(alarmId);
	
	test(cnt > 0) ;
	
	TRequestStatus status = 0;
	
	
	test.Printf(_L("Waiting one minute for the alarm to go off...\n")) ;
	
	for(;;)
		{
		iAlarmServer.NotifyChange(status, alarmId);
		User::WaitForRequest(status);
		
		PrintEventDetails(status.Int());
		
		if	(status.Int() == EAlarmChangeEventTimerExpired)
			{			
			break; // alarm expired
			}
		}

	
	/* snooze the alarm */
	
	TTime snooze = now1 + TTimeIntervalMinutes(2);	
	iAlarmServer.__DbgSnoozeAlarm(alarmId, snooze);
	for(;;)
		{
		iAlarmServer.NotifyChange(status, alarmId);
		User::WaitForRequest(status);
		
		PrintEventDetails(status.Int());		
		if (status.Int() == EAlarmChangeEventSoundStopped)
			{			
			break; // snooze has taken place 
			}
		}

	/* now crossout the first entry */	

	RPointerArray< CCalEntry > calEntryArray ;
	calentryview->FetchL(KGUID3(), calEntryArray) ;
	
	test(calEntryArray.Count() == 1) ;
	
	CCalEntry *entry1 = calEntryArray[0] ;	
	
	entry1->SetCompletedL(ETrue, startTime) ;			
		
	calentryview->StoreL(calEntryArray, num) ;
	
	delete entry1 ;	
	calEntryArray.Close() ;
	
	/* now check if the alarm still exists or not */
	TAlarmId alarmId1;
	TInt cnt1 = CountAlarmsL(alarmId1);
	
	test(cnt1 < cnt) ;		
	
	if (cnt1 > 0)
		{
		test(alarmId != alarmId1) ;
		}		
	
	ClearAllAlarmsL() ;
	
	test.Printf(_L("Checking of snoozed alarms successful\n")) ;
	
	}


void CCalAlarmAttachTest::TestDeleteAlarmL()
	{
	
	CCalEntryView &view = iTestLib->SynCGetEntryViewL() ;
	CCalEntryView *calentryview = &view ;
	
	TInt num ;	

	ClearAllAlarmsL() ;	
	
	test.Printf(_L("Going to check deletion of non-snoozed alarms\n")) ;
	
	/* create repeating parent entry	*/

	HBufC8* guid = KGUID3().AllocLC(); // ownership of guid gets transferred
 	CCalEntry *entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0) ;
	CleanupStack::Pop(guid);	
	CleanupStack::PushL(entry) ;
	
	TTime now1;
	now1.UniversalTime();
	now1 += TTimeIntervalSeconds(30);

	TCalTime startTime ;
	TCalTime endTime ;	
	startTime.SetTimeUtcL(now1) ;
	endTime.SetTimeUtcL(now1 + TTimeIntervalSeconds(30)) ;
			
	entry->SetStartAndEndTimeL(startTime, endTime) ;

	TCalRRule rrule(TCalRRule::EDaily) ;
	rrule.SetDtStart(startTime);
	
	TCalTime endRTime ;		
	endRTime.SetTimeUtcL(now1 + TTimeIntervalDays(7)) ;	
	rrule.SetUntil(endRTime);
	rrule.SetInterval(1);	
	entry->SetRRuleL(rrule) ;

	CCalAlarm *alarm = CCalAlarm::NewL() ;
	CleanupStack::PushL(alarm) ;			
	alarm->SetTimeOffset(TTimeIntervalMinutes(0)) ;
	entry->SetAlarmL(alarm) ; //ownership does not get transferred
	
	RPointerArray<CCalEntry> entryarr ;		
	entryarr.AppendL(entry) ; //ownership does not got transferred			
	calentryview->StoreL(entryarr, num) ;
	entryarr.Close() ;		
		
	CleanupStack::PopAndDestroy(alarm) ;	
	CleanupStack::PopAndDestroy(entry) ;	
	
	/* Now let the alarm go off */
	
	RArray< TAlarmId > alarmIdArr ;
	
	iAlarmServer.GetAlarmIdListL(alarmIdArr) ;
	
	TInt cnt = alarmIdArr.Count() ;
	
	test(cnt > 0) ;
	
	TRequestStatus status = 0;
	TAlarmId alarmId = alarmIdArr[0];
	
	alarmIdArr.Close() ;
	
	test.Printf(_L("Waiting one minute for the alarm to go off...\n")) ;
	
	for(;;)
		{
		iAlarmServer.NotifyChange(status, alarmId);
		User::WaitForRequest(status);
		
		PrintEventDetails(status.Int());
		
		if	(status.Int() == EAlarmChangeEventTimerExpired)
			{			
			break; // alarm expired
			}
		}

	/* delete this entry - without snoozing */

	RPointerArray< CCalEntry > calEntryArray ;
	calentryview->FetchL(KGUID3(), calEntryArray) ;
	
	test(calEntryArray.Count() == 1) ;
	
	CCalEntry *entry1 = calEntryArray[0] ;		
	
	entry1->SetCompletedL(ETrue, startTime) ;
		
	calentryview->StoreL(calEntryArray, num) ;
	
	delete entry1 ;	
	calEntryArray.Close() ;
	
	/* now check if the alarm still exists or not */
	
	RArray< TAlarmId > alarmIdArr1 ;
	
	iAlarmServer.GetAlarmIdListL(alarmIdArr1) ;
	
	TInt cnt1 = alarmIdArr1.Count() ;
	
	test(cnt1 < cnt) ;		
	
	if (cnt1 > 0)
		{
		TAlarmId alarmId1 = alarmIdArr1[0];	
		test(alarmId != alarmId1) ;
		}			
	
	alarmIdArr1.Close() ;	
	
	ClearAllAlarmsL() ;
	
	test.Printf(_L("Checking of non snoozed alarms successful\n")) ;
	
	}

static void DoMainL()
	{
	CCalAlarmAttachTest* testApp = CCalAlarmAttachTest::NewLC();
	
	TPerformanceTimer timer(test);
	timer.Start();
	
	testApp->DoTestL();
	testApp->TestDeleteSnoozedAlarmL() ;
	testApp->TestDeleteAlarmL() ;
	
	testApp->TestFetchEntryAlarmContentL();
	testApp->TestDeleteAlarmWithAlarmContentL();
	testApp->TestCopyEntryWithAlarmContentL();
	testApp->TestCopyTwiceEntryWithAlarmContentL();
	testApp->TestUpdateEntryHavingAlarmcontentL();
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();
	
	CleanupStack::PopAndDestroy(testApp);
	}

CCalAlarm* CCalAlarmAttachTest::StoreEntryWithAlarmContentLC(const TDesC8& aUid)
    {
    test.Printf(_L("Store an alarmed entry with alarm content\n"));
    // Create an attachment to the alarm.
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL(alarm);
    alarm->SetTimeOffset(1);
    
    CCalContent* almContent = CCalContent::NewL();
    CleanupStack::PushL(almContent);
    // Add content and mimetype for the alarm.
    HBufC8* content = KContent().AllocLC();
    HBufC8* mimetype = KMimeType().AllocLC();
    
    almContent->SetContentL(content, mimetype, CCalContent::EDispositionInline);
    CleanupStack::Pop(mimetype);
    CleanupStack::Pop(content);    
    alarm->SetAlarmAction(almContent); // Takes ownership of almContent.
    CleanupStack::Pop(almContent); 
  
    //Create the entry with the alarm and store it
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);
    HBufC8* guid = aUid.AllocLC();
    CCalEntry* entry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(guid);
    CleanupStack::PushL(entry);
    entries.AppendL(entry);
    CleanupStack::Pop(entry);

    TCalTime calTime;
    calTime.SetTimeUtcL(TDateTime(2007,EFebruary,15, 13, 30, 0, 0));
    entry->SetStartAndEndTimeL(calTime, calTime);
    entry->SetSummaryL(KSummary());
    
    entry->SetDescriptionL(KDescription());
    entry->SetAlarmL(alarm);
    TInt entriesStored = 0;
    iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
    CleanupStack::PopAndDestroy(&entries);
    return alarm;
    }
    
void CCalAlarmAttachTest::TestFetchEntryAlarmContentL()
    {
    test.Printf(_L("Test fetching entry twice and check it alarm conten\n"));
    _LIT8(KEntryUidFetchEntry, "FetchEntryUid");
    CCalAlarm* alarm = StoreEntryWithAlarmContentLC(KEntryUidFetchEntry());
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);

    //Fetch the entry and test the alarm  content
    iTestLib->SynCGetEntryViewL().FetchL(KEntryUidFetchEntry(), entries);
    TInt entriesStored = 0;
    iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
    entries.ResetAndDestroy();
    iTestLib->SynCGetEntryViewL().FetchL(KEntryUidFetchEntry(), entries);
    CCalEntry* entry = entries[0];
    test( KSummary() == entry->SummaryL() );
    test( KDescription() == entry->DescriptionL());
    TestAlarmL(entry, KContent(), KMimeType());
    entries.ResetAndDestroy();

    //Close the serve and fetch the entry again
    iTestLib->CloseAgendaServer();
    iTestLib->OpenFileL(KCalName());

    iTestLib->SynCGetEntryViewL().FetchL(KEntryUidFetchEntry(), entries);
    TestAlarmL(entries[0], KContent(), KMimeType());
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(alarm);
    }

void CCalAlarmAttachTest::TestDeleteAlarmWithAlarmContentL()
    {
    test.Printf(_L("Test deleting alarm from an entry \n"));
    _LIT8(KEntryUidDeleteAlarm, "DeleteAlarmUid");
    CCalAlarm* alarm = StoreEntryWithAlarmContentLC(KEntryUidDeleteAlarm());
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);

    //Update the entry by deleting its alarm
    iTestLib->SynCGetEntryViewL().FetchL(KEntryUidDeleteAlarm(), entries);
    CCalEntry* entry = entries[0];
    entry->SetAlarmL(NULL);
    TInt entriesStored = 0;
    iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
    
    //Update the entry by adding an alarm but without alarm content
    entries.ResetAndDestroy();
    iTestLib->SynCGetEntryViewL().FetchL(KEntryUidDeleteAlarm(), entries);
    alarm->SetAlarmAction(NULL);
    entry = entries[0];
    entry->SetAlarmL(alarm);
    iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
    entries.ResetAndDestroy();

    //Fetch the entry and test the entry has an alarm but not alarm content
    iTestLib->SynCGetEntryViewL().FetchL(KEntryUidDeleteAlarm(), entries);
    CCalAlarm* almEntry = entries[0]->AlarmL();
    CleanupStack::PushL(almEntry);
    test( almEntry != NULL );

    CCalContent* alarmcontent = almEntry->AlarmAction();
    test( alarmcontent == NULL );
    CleanupStack::PopAndDestroy(almEntry);
    CleanupStack::PopAndDestroy(&entries);
    CleanupStack::PopAndDestroy(alarm);
    }

void CCalAlarmAttachTest::TestCopyEntryWithAlarmContentL()
    {
    test.Printf(_L("Test copying entry but delete the original one\n"));
    _LIT8(KUidEntryOriginal, "EntryOriginalUid");
    CCalAlarm* alarm = StoreEntryWithAlarmContentLC(KUidEntryOriginal());
    CleanupStack::PopAndDestroy(alarm);
    
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);
    iTestLib->SynCGetEntryViewL().FetchL(KUidEntryOriginal(), entries);
    
    // Copy the entry from the original one
    _LIT8(KUidEntryCopy, "EntryCopyUid");
    HBufC8* guid1 = KUidEntryCopy().AllocLC();
    CCalEntry* copiedEntry = CCalEntry::NewL(CCalEntry::EEvent, guid1, CCalEntry::EMethodNone,0);
    CleanupStack::Pop(guid1);
    CleanupStack::PushL(copiedEntry);
    copiedEntry->CopyFromL(*entries[0],CCalEntry::EDontCopyId);//restore the alarmaction
    entries.ResetAndDestroy();
    entries.AppendL(copiedEntry);
    CleanupStack::Pop(copiedEntry);
     
 //delete the original entry that will delete the alarm and its content 
    CDesC8ArrayFlat* guidArray = new(ELeave)CDesC8ArrayFlat(1);
    CleanupStack::PushL(guidArray);
    guidArray->AppendL(KUidEntryOriginal());
    iTestLib->SynCGetEntryViewL().DeleteL(*guidArray);
    CleanupStack::PopAndDestroy(guidArray);
    
    //Store the copied entry and access its alarm content
    TestAlarmL(copiedEntry, KContent(), KMimeType());
    TInt entriesStored = 0;
    iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
    entries.ResetAndDestroy();
    iTestLib->SynCGetEntryViewL().FetchL(KUidEntryCopy(), entries);

    TestAlarmL(entries[0], KContent(), KMimeType());
    CleanupStack::PopAndDestroy(&entries);
    }

void CCalAlarmAttachTest::TestCopyTwiceEntryWithAlarmContentL()
    {
    test.Printf(_L("Test copying entry twice \n"));
    _LIT8(KUidEntryOriginal, "EntryOriginalUid1");
    CCalAlarm* alarm = StoreEntryWithAlarmContentLC(KUidEntryOriginal());
    CleanupStack::PopAndDestroy(alarm);
    
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);
    iTestLib->SynCGetEntryViewL().FetchL(KUidEntryOriginal(), entries);
    
    // Copy the entry from the original one
    _LIT8(KUidEntryCopy1, "EntryCopyUid1");
    HBufC8* guid1 = KUidEntryCopy1().AllocLC();
    CCalEntry* copiedEntry1 = CCalEntry::NewL(CCalEntry::EEvent, guid1, CCalEntry::EMethodNone,0);
    CleanupStack::Pop(guid1);
    CleanupStack::PushL(copiedEntry1);
    copiedEntry1->CopyFromL(*entries[0],CCalEntry::EDontCopyId);//Copied entry1 doesn't have the handel to the server but have iAlarmAction
    
    _LIT8(KUidEntryCopy2, "EntryCopyUid2");
    HBufC8* guid2 = KUidEntryCopy2().AllocLC();
    CCalEntry* copiedEntry2 = CCalEntry::NewL(CCalEntry::EEvent, guid2, CCalEntry::EMethodNone,0);
    CleanupStack::Pop(guid2);
    CleanupStack::PushL(copiedEntry2);
    copiedEntry2->CopyFromL(*copiedEntry1,CCalEntry::EDontCopyId);//Copyied entry1 should be reset with iAlarmAction
    TestAlarmL(copiedEntry1, KContent(), KMimeType());
    TestAlarmL(copiedEntry2, KContent(), KMimeType());
    TestAlarmL(entries[0], KContent(), KMimeType());
    CleanupStack::PopAndDestroy(copiedEntry2);
    CleanupStack::PopAndDestroy(copiedEntry1);
    CleanupStack::PopAndDestroy(&entries);
    }

void CCalAlarmAttachTest::TestUpdateEntryHavingAlarmcontentL()
    {
    test.Printf(_L("Test updating an entry having AlarmContent \n"));
    
    //Create a Calendar entry with Alarm Content
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);
    HBufC8* guid = KGUID1().AllocLC();
    CCalEntry* calentry = CCalEntry::NewL(CCalEntry::EEvent, guid, CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(guid);
    CleanupStack::PushL(calentry);
    
    TCalTime calTime1;
    calTime1.SetTimeUtcL(TDateTime(2007,EFebruary,15, 13, 30, 0, 0));
    TCalTime calTime2;
    calTime2.SetTimeUtcL(TDateTime(2007,EFebruary,15, 14, 30, 0, 0));
    calentry->SetStartAndEndTimeL(calTime1, calTime2);
    calentry->SetSummaryL(_L("Test for Alarms"));
    CCalAlarm* alarm = CCalAlarm::NewL();
    CleanupStack::PushL(alarm);
    alarm->SetTimeOffset(1);
        
    CCalContent* almContent = CCalContent::NewL();
    CleanupStack::PushL(almContent);
    // Add content and mimetype for the alarm.
    HBufC8* content = KContent().AllocLC();
    HBufC8* mimetype = KMimeType().AllocLC();
        
    // Takes ownership of content and mimetype.
    almContent->SetContentL(content, mimetype, CCalContent::EDispositionInline);
    alarm->SetAlarmAction(almContent); // Takes ownership of almContent.
        
    calentry->SetAlarmL(alarm);
        
    CleanupStack::Pop(mimetype);
    CleanupStack::Pop(content);    
    CleanupStack::Pop(almContent); 
    CleanupStack::PopAndDestroy(alarm);

    TCalRRule rptRule;
    rptRule.SetDtStart( calTime1 );
    rptRule.SetType( TCalRRule::EYearly );
    rptRule.SetCount(5);  
    calentry->SetRRuleL(rptRule);

    entries.AppendL(calentry);
   
    // Store entry with Alarm content
    TInt entriesStored = 0;
    iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);   
    CleanupStack::Pop(calentry);
    CleanupStack::PopAndDestroy(&entries);
    
    //Fetch stored entry and modify alarm time.
    CleanupResetAndDestroyPushL(entries);
    iTestLib->SynCGetEntryViewL().FetchL(KGUID1(), entries);
    CCalEntry* entry = entries[0];
    alarm = entry->AlarmL();
    CleanupStack::PushL(alarm);
    alarm->SetTimeOffset(1);
    entry->SetAlarmL(alarm);
    CleanupStack::PopAndDestroy(alarm);

    //Update the entry with changes.
    iTestLib->SynCGetEntryViewL().UpdateL(entries, entriesStored); 
    CleanupStack::PopAndDestroy(&entries);

    //Fetch updated entry and check if Alarm content is intact.
    CleanupResetAndDestroyPushL(entries);
    iTestLib->SynCGetEntryViewL().FetchL(KGUID1(), entries);
    TestAlarmL(entries[0], KContent(), KMimeType());
    CleanupStack::PopAndDestroy(&entries);
    }
	
/**

@SYMTestCaseID     PIM-TCAL-ALARMATTACH-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;
	CTrapCleanup* trapCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	test(scheduler != NULL);
	CActiveScheduler::Install(scheduler);
	
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-ALARMATTACH-0001 Calendar entry alarm attachment tests"));

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
