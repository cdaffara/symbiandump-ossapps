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
#include <calrrule.h>
#include <centralrepository.h>
#include <e32property.h>
#include <asshdalarm.h>
#include <calrrule.h>
#include <coreappstest/utilities.h>
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
#include <calinstanceview.h>
#include <calsession.h>
#include <calinstance.h>
#endif
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <asshdalarmcal.h>
#endif //SYMBIAN_ENABLE_SPLIT_HEADERS

RTest test(_L("tcal_alarm"));

_LIT8(KGUID,  "A_Global_UID");
_LIT8(KGUID1, "A_Global_UID1");
_LIT8(KGUID2, "A_Global_UID2");
_LIT8(KGUID3, "A_Global_UID3");
_LIT8(KGUID4, "A_Global_UID4");
_LIT8(KGUID5, "A_Global_UID5");
_LIT(KCalendarFile, "tcal_alarm");
_LIT(KCalendarFileLoc, "c:tcal_alarm");
_LIT(KCalendarFile2, "tcal_alarm_secondFile");


// Timezone constants

_LIT8(KEuropeBerlin, "Europe/Berlin");
_LIT8(KAsiaTokyo,    "Asia/Tokyo");
_LIT8(KEuropeLondon,    "Europe/London");
_LIT8(KEuropeParis,    "Europe/Paris");
_LIT8(KAtlanticStanley,    "Atlantic/Stanley");
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
_LIT8(KEuropeHelsinki, "Europe/Helsinki");
#endif

const TInt KAlarmOffset = 10;

//back off period. Used when setting the time just before alarm expiry.
const TTimeIntervalSeconds KAlarmExpiryBackOffPeriod(5);

// Central Repository Uid
const TUid KUidCentralRepositoryAgenda = { 0x10003a5b };

// Used for configuration of whether or not to notify missing alarms
const TUint KKeyQueueMissingAlarm = 100;

//Timeout period of 2 minutes
const TInt KTimeOutPeriod = 120000000;

enum TCenRepValue
	{
	// Skipped alarms will notify, but not be queued
	ESkippedAlarmsNotifySubscriber = 0,
	// Skipped alarms will queue and expire (default)
	ESkippedAlarmsQueueAndExpire = 1	
	};
	
enum TPubSubValue
	{
	ENoChange = 0, // There is no published data from the alarmserver
	ETzChanged = 1,//Time zone has been changed but there are no missing alarms
	EAlarmMissed = 2 // There are some missing alarms when system time has been put forward	
	};
	
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
enum TSystemTimeOrUTC
	{
	ESystemTime,
	EUTC
	};
#endif

struct TFloatingAndFixedEntryData
	{
	TCalTime iParentStartTime;
	TCalTime iChildStartTime;
	TTime iParentAlarmTimeLocal;
	TTime iChildAlarmTimeLocal;
	TCalLocalUid iParentLocalUid;
	TCalLocalUid iChildLocalUid;
	};

class CAlarmTestManager : public CActive
	{
public:
	static CAlarmTestManager* NewLC();
	~CAlarmTestManager();	

	//Set the value in central repository to test different behaviour. An application is normally 
	//not able to do so since it is constrained by their security policy.
	void SetCenRepValuseL(TCenRepValue aValue);
	void Start();
	
private:
	// avoid any method but NewL instancing the class
	CAlarmTestManager();
	// no copy constructor and assignment operator
	CAlarmTestManager(CAlarmTestManager& );
	CAlarmTestManager& operator = (const CAlarmTestManager& );
	void ConstructL();

	void SetupTestAppL();
	void TestUpdateAlarmL(TBool aFloating = EFalse);
	void TestMultipleAlarmsL(TBool aFloating = EFalse);
	void TestDEF079490L(TBool aFloating = EFalse);
	void SetTimeZoneL(const TDesC8& aTimeZone);
	void SetupAlarmsBeforeTzChangeL();
	void CheckAlarmsBeforeTzChangeL();
	void CheckNonStopTodoAlarmL();
	void TestAlarmEntryL();
	void TestAlarmOffsetL();
	void SnoozeAlarmAndUpdateEntryL();

	// Test suite
	void TestAddAlarmL(const TBool aFloating = EFalse);
	void TestAddAlarmsSummerWinterL(const TBool aFloating = EFalse);
	void TestDeleteAlarmsSummerWinterL(const TBool aFloating = EFalse);
	void TestFloatingAndFixedEntryAlarmTimeSetupL(TBool aIsFloatingEntry);
	void TestFloatingAndFixedEntryAlarmTimeCheckL(TBool aIsFloatingEntry);
	void TestFloatingAndFixedEntryAlarmTimeReopenAndCheckL(TBool aIsFloatingEntry);
	void TestFloatingAndFixedEntryAlarmTimeCheckAgainL(const TBool aIsFloatingEntry);
	void CreateAndStoreEntriesWithAlarmAttachmentsLC(TBool aIsFloatingEntry, CCalEntry*&aParentEntry, CCalEntry*& aChildEntry);
	
	void TestAddAlarmL(const TTime& aStartTime, const TInt aAlarmMinutes, HBufC8* aGuid = NULL, const TBool aFloating = EFalse);
	void TestAddAlarmInTodoL(const TTime& aStartTime, const TTime& aEndTime, const TBool aFloating = EFalse);
	void TestDeleteAlarmL(const TTime& aStartTime, const TInt aAlarmMinutes, const TBool aFloating = EFalse);

	CCalEntry* CreateAlarmedEntryLC(CCalEntry::TType aType, const TTime& aStartTimeLocal, 
								    const TTime& aEndTimeLocal, TInt aTimeOffset, HBufC8* aGuid = NULL, TBool aFloating = EFalse);
	TAlarmId CheckAlarmL(const TTime& aAlarmTime, TCalLocalUid aLocalUid, const TDesC8& aGuid = KNullDesC8, const TBool aFloating=EFalse);
	void CheckAndSnoozeAlarmL(const TTime& aAlarmTime, TCalLocalUid aLocalUid, const TDesC8& aGuid, const TBool aFloating=EFalse);
	void CheckNoAlarmWentOffL(const TTime& aAlarmTime);
	void ClearAllAlarmsL();
	TCalLocalUid AddEntryWithAlarmL(const TTime& aStartTime, TInt aAlarmMinutes, HBufC8* aGuid = NULL, TBool aFloating = EFalse);
	void TestPubSubValue(TPubSubValue aValue);
	void TestChangeSystemTimeL(TBool aIsFloating, TBool aChangeSysTimeSameAsAlarmTime);
	void AddFourEntriesWithAlarmsL();
	void TestFloatingAlarmNotificationL();
	void CheckAlarmAttachmentSizeL();
	void TestNotifiedAlarmL();
	void TestAlarmNotDeletedInDiffFileL();
	void TestAlarmDeletedInSameFileL();
	void TestAlarmDeletedWhenFileDeletedL();
	void TestRetrieveSkippedAlarmsAfterSystemTimeChangeL();
	
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS	
	void CheckNextQueuedAlarmL(const TTime& aAlarmTime);	
	void RetrieveAlarmedInstancesLC(RPointerArray<CCalInstance>& aInstances);
	void RetrieveAlarmedInstancesLC(RPointerArray<CCalInstance>& aInstances, TPckgBuf<TASShdAlarmedInstanceParams>& aParamsBuf);
	void WaitForAlarmAdditionL();
	void CreateAndAddEntriesL(TBool aFloating1, const TTime& aTime1, TBool aFloating2, const TTime& aTime2);
	void CreateAndAddEntryL(TBool aFloating, const TTime& aTime);
	
	void NotifySubscriberWithCalendarRunningL();
	void NotifySubscriberWithoutCalendarRunningL();
	void RetrieveSkippedAlarmsAfterRestartL(TSystemTimeOrUTC aSystemTimeOrUTC);
	void TestFloatingAlarmWithPositiveUtcOffsetL();
	void TestFloatingAlarmWithNegativeUtcOffsetL();
	void TestTimeAndUtcChangeL();
	void TestFixedAndFloatingWithUtcChangeL();
	void TestTimeChangeWithNoSkippedAlarmL();
#endif	
	
	// from CActive
	void RunL();
	void DoCancel();
			    
private:
	CCalTestLibrary*	iTestLibrary;	
	RASCliSession		iAlarmServer;
	TTime				iExpectedInstanceTime;
	TBuf8<256>			iCurrentTimeZone;
	TInt				iTestStep;
	TFloatingAndFixedEntryData	iFloatingAndFixedData;
	TCenRepValue iQueueAlarm;
	TTime iTimeSysTimeChanged;
 	};


// Constructor/Destructor
	
CAlarmTestManager* CAlarmTestManager::NewLC()
	{
	CAlarmTestManager* self = new (ELeave) CAlarmTestManager;

	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}

CAlarmTestManager::CAlarmTestManager() : CActive(EPriorityIdle)
	{
	CActiveScheduler::Add(this);
	}


void CAlarmTestManager::ConstructL()
	{	
	iTestLibrary = CCalTestLibrary::NewL(EFalse);
	
	iTestLibrary->PIMTestServer().GetTimeZoneL(iCurrentTimeZone);
	SetTimeZoneL(KEuropeBerlin);
	
	iTestLibrary->ReplaceFileL(KCalendarFile());
	iTestLibrary->OpenFileL(KCalendarFile());
	
	// Connect to the alarm server
	User::LeaveIfError(iAlarmServer.Connect());
	
	// Make sure that the alarm queue is empty.
	ClearAllAlarmsL();
	}

	
CAlarmTestManager::~CAlarmTestManager()
	{
	SetCenRepValuseL(ESkippedAlarmsQueueAndExpire);//default behaviour
	if ( iTestLibrary && iCurrentTimeZone.Length() )
		{
		TRAP_IGNORE(SetTimeZoneL(iCurrentTimeZone));
		}	

	iAlarmServer.Close();
	TRAP_IGNORE( iTestLibrary->DeleteFileL(KCalendarFile) );
	delete iTestLibrary;
	}

void CAlarmTestManager::SetTimeZoneL(const TDesC8& aTimeZone)
	{
	iTestLibrary->SetTimeZoneL(aTimeZone);
	User::After(500000);
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Test cases forming the Alarm test suite
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
// case 0

// case 1
void CAlarmTestManager::TestAddAlarmL(const TBool aFloating)
	{
	TTime startTime(TDateTime(2005, EJanuary, 7, 10, 0, 0, 0));
	 
	TestAddAlarmInTodoL(startTime, startTime + TTimeIntervalDays(1), aFloating);	// alarm on 9th Jan at 9:50
	TestAddAlarmInTodoL(startTime, Time::NullTTime(), aFloating);	// alarm on 8th Jan at 9:50
	}	

// case 2
void CAlarmTestManager::TestAddAlarmsSummerWinterL(const TBool aFloating)
	{
	// test adding alarms in summer and winter time zones with "when entry is set to floating" option
	TestAddAlarmL(TDateTime(2005, EMarch, 7, 10, 0, 0, 0), 0, NULL, aFloating); 		// alarm on 8th Mar at 10:00
	TestAddAlarmL(TDateTime(2005, EMarch, 7, 10, 0, 0, 0), 1000, NULL, aFloating);	// alarm on 7th Mar at 17:20
	TestAddAlarmL(TDateTime(2005, EJuly, 7, 10, 0, 0, 0), 0, NULL, aFloating); 		// alarm on 8th Jul at 10:00
	TestAddAlarmL(TDateTime(2005, EJuly, 7, 10, 0, 0, 0), 1000, NULL, aFloating);	// alarm on 7th Jul at 17:20
	// test negative alarms
	TestAddAlarmL(TDateTime(2005, EMarch, 7, 10, 0, 0, 0), -10); 	// alarm on 8th Mar at 10:10
	TestAddAlarmL(TDateTime(2005, EMarch, 7, 10, 0, 0, 0), -839); 	// alarm on 8th Mar at 23:59
	}	

// case 3
void CAlarmTestManager::TestMultipleAlarmsL(const TBool aFloating)
	{
	TTime alarm1(TDateTime(2005, EFebruary, 3, 9, 0, 0, 0));
	TTime alarm2(TDateTime(2005, EFebruary, 3, 9, 5, 0, 0));
	TTime alarm3(TDateTime(2005, EFebruary, 3, 9, 6, 0, 0));
	TTime alarm4(TDateTime(2005, EFebruary, 3, 9, 10, 0, 0));
	
	iTestLibrary->PIMTestServer().SetHomeTime(alarm1 - TTimeIntervalHours(1)); // alarm must be in the future when store entry
		
	CCalEntry* entry1 = CreateAlarmedEntryLC(CCalEntry::EAppt, alarm1, alarm1, 0, KGUID1().AllocLC(), aFloating); // 09:00 on 4 Feb 05, alm at 9:00
	CCalEntry* entry2 = CreateAlarmedEntryLC(CCalEntry::EAppt, alarm2, alarm2, 1, KGUID2().AllocLC(), aFloating); // 09:05 on 4 Feb 05, alm at 9:04
	CCalEntry* entry3 = CreateAlarmedEntryLC(CCalEntry::EAppt, alarm3, alarm3, 1, KGUID3().AllocLC(), aFloating); // 09:06 on 4 Feb 05, alm at 9:05
	CCalEntry* entry4 = CreateAlarmedEntryLC(CCalEntry::EAppt, alarm4, alarm4, 5, KGUID4().AllocLC(), aFloating); // 09:10 on 4 Feb 05, alm at 9:05

	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	
	entriesToStore.AppendL(entry1);
	entriesToStore.AppendL(entry2);
	entriesToStore.AppendL(entry3);
	entriesToStore.AppendL(entry4);
	
	test.Printf(_L("Adding alarmed entries at 9:00, 9:04, 9:05 and 9:05\n"));

	TInt entriesStored = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);	
	
	TCalLocalUid localUid1 = entry1->LocalUidL();
	TCalLocalUid localUid2 = entry2->LocalUidL();
	TCalLocalUid localUid3 = entry3->LocalUidL();
	TCalLocalUid localUid4 = entry4->LocalUidL();
	
	CleanupStack::PopAndDestroy(5);	//&entriesToStore, entry4, entry3, entry2, entry1
	
	iExpectedInstanceTime = alarm1;
	CheckAlarmL(alarm1, localUid1, KGUID1(), aFloating );
	iExpectedInstanceTime = alarm2;
	CheckAlarmL(alarm2 - TTimeIntervalMinutes(1), localUid2, KGUID2(), aFloating);
	iExpectedInstanceTime = alarm3;
	CheckAlarmL(alarm3 - TTimeIntervalMinutes(1), localUid3, KGUID3(), aFloating);

	// The alarms for entry3 and entry4 are set for the same time.
	// This means the next test will receive the details for entry3.
	iExpectedInstanceTime = alarm3;
	CheckAlarmL(alarm4 - TTimeIntervalMinutes(5), localUid3, KGUID3(), aFloating);
	}

// case 4
/* removed, was SetTimeZoneL(KAsiaTokyo) */

// case 5
void CAlarmTestManager::TestDeleteAlarmsSummerWinterL(const TBool aFloating)
	{
	// Make sure that the alarm queue is empty.
	ClearAllAlarmsL();	
	
	// test deleting alarms in summer and winter time zones with "when entry is set to floating" option		
	TestDeleteAlarmL(TDateTime(2005, EMarch, 7, 10, 0, 0, 0), 0, aFloating); 	// alarm on 8th Mar at 10:00
	TestDeleteAlarmL(TDateTime(2005, EMarch, 7, 10, 0, 0, 0), 1000, aFloating);	// alarm on 7th Mar at 17:20
	TestDeleteAlarmL(TDateTime(2005, EJuly, 7, 10, 0, 0, 0), 0, aFloating); 	// alarm on 8th Jul at 10:00
	TestDeleteAlarmL(TDateTime(2005, EJuly, 7, 10, 0, 0, 0), 1000, aFloating);	// alarm on 7th Jul at 17:20
	}

// case 6
void CAlarmTestManager::TestUpdateAlarmL(const TBool aFloating)
	{
	TTime alarm1(TDateTime(2005, EJanuary, 20, 9, 0, 0, 0));
	TTime alarm2(TDateTime(2005, EJanuary, 20, 9, 1, 0, 0));
	TTime alarm3(TDateTime(2005, EJanuary, 20, 9, 5, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(alarm1 - TTimeIntervalHours(1)); // alarm must be in the future when store entry
	
	CCalEntry* entry1 = CreateAlarmedEntryLC(CCalEntry::EAppt, alarm1, alarm1, 0, KGUID1().AllocLC(), aFloating); // 09:00 on 21 Jan 05, alm at 9:00
	CCalEntry* entry2 = CreateAlarmedEntryLC(CCalEntry::EAppt, alarm2, alarm2, 0, KGUID2().AllocLC(), aFloating); // 09:01 on 21 Jan 05, alm at 9:01
	CCalEntry* entry3 = CreateAlarmedEntryLC(CCalEntry::EAppt, alarm3, alarm3, 0, KGUID3().AllocLC(), aFloating); // 09:05 on 21 Jan 05, alm at 9:05
	
	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	
	entriesToStore.AppendL(entry1);
	entriesToStore.AppendL(entry2);
	entriesToStore.AppendL(entry3);
	
	test.Printf(_L("Adding alarmed entries at 9:00, 9:01, 9:05\n"));
	
	TInt entriesStored = 0;	
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);	
	
	test.Printf(_L("Deleting entry at 9:00\n"));
	iTestLibrary->SynCGetEntryViewL().DeleteL(*entry1);
	
	test.Printf(_L("Moving entry from 9:05 to 9:03\n"));
	CCalAlarm* almEntry3 = entry3->AlarmL();
	CleanupStack::PushL(almEntry3);
	almEntry3->SetTimeOffset(TTimeIntervalMinutes(2));
	entry3->SetAlarmL(almEntry3);
	
	entriesToStore.Reset();
	entriesToStore.AppendL(entry3);
	iTestLibrary->SynCGetEntryViewL().UpdateL(entriesToStore, entriesStored);

	CleanupStack::PopAndDestroy(almEntry3);
	CleanupStack::PopAndDestroy(&entriesToStore);
	CleanupStack::PopAndDestroy(entry3);
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry1);
	
	// Connect to the alarm server
	User::LeaveIfError(iAlarmServer.Connect());
	}

// case 7
/* removed, was "Start floating entry test" */

// case 8
/* removed, was TestAddAlarmL(ETrue) */

// case 9
/* removed, was TestAddAlarmsSummerWinterL(ETrue) */
	
// case 10
/* removed, was TestMultipleAlarmsL(ETrue) */
	
// case 11
/* removed, was SetTimeZoneL(KAsiaTokyo) */

// case 12
/* removed, was TestDeleteAlarmsSummerWinterL(ETrue) */
	
// case 13
/* removed, was TestUpdateAlarmL(ETrue) */
		

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


CCalEntry* CAlarmTestManager::CreateAlarmedEntryLC(CCalEntry::TType aType, const TTime& aStartTimeLocal, 
								const TTime& aEndTimeLocal, TInt aTimeOffset, HBufC8* const aGuid, const TBool aFloating)
	{
	HBufC8* guid = aGuid;
	
	if ( ! guid )
		{
		guid = KGUID().AllocLC();
		}
		
	CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime calStartTime;
	TCalTime calEndTime;
	
	if (aFloating)
		{
		calStartTime.SetTimeLocalFloatingL(aStartTimeLocal);
		calEndTime.SetTimeLocalFloatingL(aEndTimeLocal);
		}
	else
		{
		calStartTime.SetTimeLocalL(aStartTimeLocal);
		calEndTime.SetTimeLocalL(aEndTimeLocal);
		}
		
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(aTimeOffset);
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);

	return ( entry );
	}


void CAlarmTestManager::CheckAlarmsBeforeTzChangeL() 
    { 
    // expecting 4 alarms at UTC times given in above function. 
    // current time zone is Helsinki 
 
    iExpectedInstanceTime = TDateTime(2006, EMarch, 9, 2, 0, 0, 0); 
    CheckAlarmL(TDateTime(2006, EMarch, 9, 1, 50, 0, 0), 0, _L8("SetupAlarmsBeforeTzChangeL_1")); 
    CheckAlarmL(TDateTime(2006, EMarch, 9, 2, 0, 0, 0), 0, _L8("SetupAlarmsBeforeTzChangeL_2")); 
    CheckAlarmL(TDateTime(2006, EMarch, 8, 17, 00, 0, 0), 0, _L8("SetupAlarmsBeforeTzChangeL_3")); 
    } 


TAlarmId CAlarmTestManager::CheckAlarmL(const TTime& aAlarmTime, const TCalLocalUid aLocalUid, const TDesC8& aGuid, const TBool aFloating)
	{
	const TInt KQueuedAlarms = iAlarmServer.AlarmCountByState(EAlarmStateQueued);

	iTestLibrary->PIMTestServer().SetHomeTime(aAlarmTime - KAlarmExpiryBackOffPeriod);
	
	CAlarmEventWatcher* alarmwatcher = CAlarmEventWatcher::NewL(iAlarmServer);
	CleanupStack::PushL(alarmwatcher);
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventSystemDateTimeChanged, KTimeOutPeriod);
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventAlarmAddition, KTimeOutPeriod);
	CleanupStack::PopAndDestroy(alarmwatcher);
	
	TTime homeTime;
	homeTime.HomeTime();
	_LIT(KFormatTime,"%D %H:%T:%S.   ");
	TBuf<128> timeTxt;
	homeTime.FormatL(timeTxt, KFormatTime);

	TDateTime dt = aAlarmTime.DateTime();
	test.Printf(_L("CheckAlarmL: Expecting the next due alarm at = %d/%d/%d, %d:%d"), 
					dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute());
	
	TAlarmId almId;
	TASShdAlarm alarmData;
	if (iAlarmServer.GetNextDueAlarmId(almId) == KErrNone)
		{ 
		if (iAlarmServer.GetAlarmDetails(almId, alarmData) == KErrNone)
			{
			test(alarmData.IsFloating()==aFloating);
		
			test.Printf(_L("CheckAlarmL: Next Due Alarm Id = %d "), almId);
			test.Printf(_L("CheckAlarmL: No of alarms queued at the beginning = %d"), KQueuedAlarms);
			test.Printf(_L("CheckAlarmL: Entry localuid = %d"), aLocalUid);

			TTime alarmNextDueTime = alarmData.NextDueTime();
			if(!alarmData.IsFloating())
				{
				//The alarm next due time is in UTC
				//add the UTC offset to find teh local time of the alarm
				alarmNextDueTime+=User::UTCOffset();
				}
			
			TDateTime dueDt = alarmNextDueTime.DateTime();
			alarmNextDueTime.FormatL(timeTxt, KFormatTime);	
			test.Printf(_L("CheckAlarmL: Next Due Alarm Time = %d/%d/%d, %d:%d"), 
					dueDt.Day()+1, dueDt.Month()+1, dueDt.Year(), dueDt.Hour(), dueDt.Minute());
			
			test (alarmNextDueTime==aAlarmTime);

			// Get the alarm data from the Alarm Server...
			HBufC8* data = NULL;
			test(iAlarmServer.GetAlarmData(almId, data) == KErrNone);
			CleanupStack::PushL(data);
			
			// Use the CalInterimAPi to read the alarm data...
			CCalEntryId* entryId = CCalEntryId::NewL(data);
			CleanupStack::Pop(data); // CCalEntryId taken ownership of data.
			CleanupStack::PushL(entryId);
			
			// Check that the data is of the expected value.
			TPtrC storeFileName = entryId->StoreFileNameL();
			test(storeFileName.Compare(KCalendarFileLoc) == 0);
			
			// Can only compare guid if it has been passed into this method.
			if (aGuid.Length() != 0)
				{
				TPtrC8 uid = entryId->IdL();
				test(aGuid.Compare(uid) == 0);
				}
							
			TTime instTimeLocal = entryId->InstanceTimeL().TimeLocalL();
			test(instTimeLocal == iExpectedInstanceTime);
			
			TCalLocalUid tempUid = entryId->LocalUidL();
			
			test(entryId->LocalUidL() == aLocalUid);
			
			CleanupStack::PopAndDestroy(entryId);
			}
		else
			{
			test.Printf(_L("Error in getting Alarm details : %d"), iAlarmServer.GetAlarmDetails(almId, alarmData));
			test(0);
			}
		}
	else
		{
		test.Printf(_L("Error in getting Alarm ID : %d"), iAlarmServer.GetNextDueAlarmId(almId));
		test(0);
		}

	test(iTestLibrary->WaitForAlarmEvent(almId,EAlarmChangeEventTimerExpired, EAlarmStateNotifying, KAlarmExpiryBackOffPeriod.Int()*2*1000000));

	const TInt KQueuedAlarmsAfter = iAlarmServer.AlarmCountByState(EAlarmStateQueued);
	test.Printf(_L("CheckAlarmL: No of alarms queued after = %d \n"), KQueuedAlarmsAfter);
	
	if ( KQueuedAlarmsAfter != KQueuedAlarms - 1 )
		{
		TInt nextId;
		iAlarmServer.GetNextDueAlarmId(nextId);
		test(nextId != almId);
		}
	return almId;
	}
	
/**
This works only in the debug builds as the __DbgSnoozeAlarm is available only in debug builds.
*/	
void CAlarmTestManager::CheckAndSnoozeAlarmL(const TTime& aAlarmTime, TCalLocalUid aLocalUid, const TDesC8& aGuid,  const TBool aFloating)
	{
	// Wait for the expected alarm to start notifying.
	// With the live server this shows the alarm as Waiting to Notify for some
	// strange reason, although the alarm does expire, and can be seen.
	TAlarmId almId = CheckAlarmL(aAlarmTime, aLocalUid, aGuid, aFloating);

	// Found alarm, now find details to snooze it (Only works in debug builds)
	test.Printf(_L("Snoozing the alarm..."));
	TInt ret = iAlarmServer.__DbgSnoozeAlarm(almId, aAlarmTime + TTimeIntervalMinutes(2));
	}



void CAlarmTestManager::CheckNoAlarmWentOffL(const TTime& aAlarmTime)
	{
#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
	iTestLibrary->PIMTestServer().SetHomeTime(aAlarmTime - KAlarmExpiryBackOffPeriod);
	
	// We do not expect any alarm (i.e. KNullAlarmId) to change to the notified state
	test(!iTestLibrary->WaitForAlarmEvent(KNullAlarmId, EAlarmChangeEventState, EAlarmStateNotified, KAlarmExpiryBackOffPeriod.Int()*2*1000000));	
#else
	const TInt KQueuedAlarms = iAlarmServer.AlarmCountByState(EAlarmStateQueued);
	test.Printf(_L("%d alarms queued, expecting none to go off\n"), KQueuedAlarms);
	
   	iTestLibrary->PIMTestServer().SetHomeTime(aAlarmTime - KAlarmExpiryBackOffPeriod);

   	//we do not expect any alarm to expire (KNullAlarmId means any alarm)
   	test(!iTestLibrary->WaitForAlarmEvent(KNullAlarmId, EAlarmChangeEventTimerExpired, EAlarmStateQueued /*not used*/, KAlarmExpiryBackOffPeriod.Int()*2*1000000));
	
	TInt KQueuedAlarmsAfter = iAlarmServer.AlarmCountByState(EAlarmStateQueued);
	test(KQueuedAlarmsAfter == KQueuedAlarms);
#endif
	}


void CAlarmTestManager::TestAddAlarmL(const TTime& aStartTime, const TInt aAlarmMinutes, HBufC8* aGuid, const TBool aFloating)
	{	
	iTestLibrary->PIMTestServer().SetHomeTime(aStartTime - TTimeIntervalDays(1)); // set alarm time in the future
	TCalLocalUid localUid = AddEntryWithAlarmL(aStartTime, aAlarmMinutes, aGuid, aFloating);
	iExpectedInstanceTime = aStartTime;
	CheckAlarmL(aStartTime - TTimeIntervalMinutes(aAlarmMinutes), localUid, KNullDesC8,  aFloating);
	}

TCalLocalUid CAlarmTestManager::AddEntryWithAlarmL(const TTime& aStartTime, TInt aAlarmMinutes, HBufC8* aGuid, TBool aFloating)
	{	
 
	TDateTime dt = (aStartTime - TTimeIntervalMinutes(aAlarmMinutes)).DateTime();
	test.Printf(_L("Adding alarmed entry, alarm set to: %d/%d/%d, %d:%d\n"),
				dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute());
	
	CCalEntry* entry = CreateAlarmedEntryLC(CCalEntry::EAppt, aStartTime, aStartTime + TTimeIntervalHours(1),
									 		aAlarmMinutes, aGuid, aFloating); // 10-11am on 8 Apr 05, alm at 10:00

	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	
	entriesToStore.AppendL(entry);
	
	TInt entriesStored = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);
	
	TCalLocalUid localUid = entry->LocalUidL();

	CleanupStack::PopAndDestroy(2); //&entriesToStore, entry
	return localUid;
	}

	
void CAlarmTestManager::TestAddAlarmInTodoL(const TTime& aStartTime, const TTime& aEndTime, const TBool aFloating)
	{	
	iTestLibrary->PIMTestServer().SetHomeTime(aStartTime - TTimeIntervalDays(1)); // alarm must be in the future when store entry
	CCalEntry* entry=NULL;
	
	const TInt KAlarmMin = 10;
	TRAPD
		(ret,
			{
			entry = CreateAlarmedEntryLC(CCalEntry::ETodo, aStartTime, aEndTime, KAlarmMin, NULL, aFloating);
			CleanupStack::Pop(entry);
			}
		);
		
	CleanupStack::PushL(entry);

    test(ret == KErrNone);
	
	if ( aEndTime == Time::NullTTime() )
		{
		CleanupStack::PopAndDestroy(entry);
		return;
		}
	else
		{
		TDateTime dt = (aEndTime - TTimeIntervalMinutes(KAlarmMin)).DateTime();
		test.Printf(_L("Adding alarmed entry, alarm set to: %d/%d/%d, %d:%d\n"),
					dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute());		
		}
		
	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	
	entriesToStore.AppendL(entry);

	TInt entriesStored = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);

	TCalLocalUid localUid = entry->LocalUidL();
	
	if ( aEndTime != Time::NullTTime() )
		{
		iExpectedInstanceTime = aEndTime;		
		CheckAlarmL(aEndTime - TTimeIntervalMinutes(KAlarmMin), localUid, KNullDesC8, aFloating);
		}
	else if ( aStartTime != Time::NullTTime() )
		{
		iExpectedInstanceTime = aStartTime;		
		// if a todo only has a start time with no end time, then an alarm should be queued
		CheckAlarmL(aStartTime - TTimeIntervalMinutes(KAlarmMin), localUid, KNullDesC8, aFloating);		
		}

	CleanupStack::PopAndDestroy(2); //&entriesToStore, entry
	}
	

void CAlarmTestManager::TestDeleteAlarmL(const TTime& aStartTime, const TInt aAlarmMinutes, const TBool aFloating)
	{
	iTestLibrary->PIMTestServer().SetHomeTime(aStartTime - TTimeIntervalDays(1)); // alarm must be in the future when store entry

	const TInt KQueuedAlarms = iAlarmServer.AlarmCountByState(EAlarmStateQueued);
	test.Printf(_L("%d alarms currently queued\n"), KQueuedAlarms);	

	TDateTime dt = (aStartTime - TTimeIntervalMinutes(aAlarmMinutes)).DateTime();
	test.Printf(_L("Adding alarmed entry, alarm set to: %d/%d/%d, %d:%d\n"),
				dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute());
				
	CCalEntry* entry = CreateAlarmedEntryLC(CCalEntry::EAppt, aStartTime, aStartTime + TTimeIntervalHours(1), aAlarmMinutes, NULL, aFloating);
									 
	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	
	entriesToStore.AppendL(entry);				
				
	TInt entriesStored = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);

	const TInt KQueuedAlarmsAfterAdding = iAlarmServer.AlarmCountByState(EAlarmStateQueued);
	test(KQueuedAlarmsAfterAdding == KQueuedAlarms + 1);

	test.Printf(_L("Deleting alarmed entry\n"));
	iTestLibrary->SynCGetEntryViewL().DeleteL(*entry);
	
	CheckNoAlarmWentOffL(aStartTime);
	
	const TInt KQueuedAlarmsAfterDeleting = iAlarmServer.AlarmCountByState(EAlarmStateQueued);
	test(KQueuedAlarmsAfterDeleting == KQueuedAlarms);

	CleanupStack::PopAndDestroy(2); //&entriesToStore, entry
	}


void CAlarmTestManager::TestDEF079490L(TBool aFloating)
	{
	const TInt K2000Days = 2000*60*24; //2000 days *(minutes in a hour) * (hours in a day)
	TTime alarm(TDateTime(2005, EJanuary, 20, 9, 0, 0, 0));
	
	iTestLibrary->PIMTestServer().SetHomeTime(alarm - TTimeIntervalHours(1)); // alarm must be in the future when store entry
	
	CCalEntry* entry = CreateAlarmedEntryLC(CCalEntry::EAppt, alarm, alarm, K2000Days, KGUID1().AllocLC(), aFloating); // 09:00 on 21 Jan 05, alm at 9:00
	
	CCalAlarm* almEntry = entry->AlarmL();
	CleanupStack::PushL(almEntry);

	CleanupStack::PopAndDestroy(2);	//almEntry, entry

	// Connect to the alarm server
	User::LeaveIfError(iAlarmServer.Connect());
	}


void CAlarmTestManager::CheckNonStopTodoAlarmL()
	{
	_LIT8(KGuid, "nonstop-todo");
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* todo = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(todo);
	
	TCalTime start, end;
	start.SetTimeLocalL(TDateTime(2005, ENovember, 10, 0, 0, 0, 0)); // starts in Nov 05
	end.SetTimeLocalL(TDateTime(2006, EMarch, 29, 12, 0, 0, 0)); // due at 12:00 on 30 Mar 05
	todo->SetStartAndEndTimeL(start, end);

	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(120);
	todo->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
 
	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	
	entriesToStore.AppendL(todo);
	
	TInt entriesStored = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);
	
	CleanupStack::PopAndDestroy(2); // entries.Close(), todo
	
	
	//
	TTime errorTime = TDateTime(2006, EMarch, 29, 0, 0, 0, 0);
	iTestLibrary->PIMTestServer().SetHomeTime(errorTime);
	
	delete iTestLibrary;
	iTestLibrary = NULL;
	
	iTestLibrary = CCalTestLibrary::NewL(EFalse);
	iTestLibrary->OpenFileL(KCalendarFile());
	iTestLibrary->SynCGetEntryViewL(); // make the indexes get built
	
	const TInt KQueuedAlarms = iAlarmServer.AlarmCountByState(EAlarmStateQueued);
	test(KQueuedAlarms == 1);	
	const TInt KNotifiedAlarms = iAlarmServer.AlarmCountByState(EAlarmStateNotified);
	test(KNotifiedAlarms == 0);
	}


//! @SYMTestCaseID     PIM-TCAL-ALARM-0002
//! @SYMDEF INC091574
//! @SYMTestType UT
//! @SYMTestPriority High
//! @SYMDEF 
//! @SYMTestCaseDesc  Changing an alarmed entry after snoozing the alarm causes 2 alarms.
//! @SYMTestActions 
//! 1. Create entry with alarm.
//! 2. When alarm is triggered snooze it.
//! 3. While alarm is snoozed update the entry start time, end time and alarm time.
//! @SYMTestExpectedResults 
//! The old snoozed alarm should be deleted and the new alarm queued.

void CAlarmTestManager::SnoozeAlarmAndUpdateEntryL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-ALARM-0002 Test updating an entry and its alarm while its initial alarm is snoozed."));	
	// Set the home time to now
	ClearAllAlarmsL();
	TTime now(TDateTime(2006, EAugust, 4, 9, 0, 0, 0)); 	// now = 9 o clock
	iTestLibrary->PIMTestServer().SetHomeTime(now); 
	
	// Create and store a simple alarmed entry
	test.Printf(_L("Adding alarmed entry \n"));
	TTime appStart = now + TTimeIntervalHours(1);             // 10 o clock
	TTime appEnd = appStart + TTimeIntervalHours(1);          // 11 o clock
	CCalEntry* entry = CreateAlarmedEntryLC(CCalEntry::EAppt, appStart, appEnd, 59 , KGUID5().AllocLC(), EFalse); 
	RPointerArray<CCalEntry> entries;
	CleanupClosePushL(entries);
	TInt entriesStored(0);
	entries.AppendL(entry);
	iTestLibrary->SynCGetEntryViewL().StoreL(entries, entriesStored);
	TCalLocalUid luid = entry->LocalUidL();
	TInt totalAlarmsBefore = iAlarmServer.NumberOfAlarmsActiveInQueue();

	// Receive alarm from Alarm Notification Server and snooze it
	iExpectedInstanceTime = appStart;
	CheckAndSnoozeAlarmL(now + TTimeIntervalMinutes(1), luid, KGUID5()); // alarm at 9:01

	// Update the entry, while the alarm for it has been snoozed, including alarm time
	test.Printf(_L("Updating entry while snoozed alrarm  \n"));
	TTime newAppStart(now + TTimeIntervalMinutes(3));           // 9:03 o clock
	TTime newAppEnd = newAppStart + TTimeIntervalMinutes(2);    // 9:05 o clock
	CCalAlarm* almEntry = entry->AlarmL();
	CleanupStack::PushL(almEntry);
	TCalTime calStartTime;
	TCalTime calEndTime;
	calStartTime.SetTimeLocalFloatingL(newAppStart);
	calEndTime.SetTimeLocalFloatingL(newAppEnd);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);
	almEntry->SetTimeOffset(TTimeIntervalMinutes(1));             // 9:02 o clock
	entry->SetAlarmL(almEntry);
	CleanupStack::PopAndDestroy(almEntry);
	entries.Reset();
	entries.AppendL(entry);
	iTestLibrary->SynCGetEntryViewL().UpdateL(entries, entriesStored);
	
	// Check that the no of alarms is 1, on updating old alarm should have beeen deleted.

	TInt totalAlarmsAfter = iAlarmServer.NumberOfAlarmsActiveInQueue();

	// Receive alarm from Alarm Notification Server and snooze it
	iExpectedInstanceTime = newAppStart;
	CheckAndSnoozeAlarmL(now + TTimeIntervalMinutes(2), luid, KGUID5(), ETrue); // alarm at 9:02

	test(totalAlarmsBefore == totalAlarmsAfter);

	test.Next(_L("Test SnoozeAlarmAndUpdateEntryL passed, the snoozed alarm was deleted."));
	// Clean up 
	iTestLibrary->SynCGetEntryViewL().DeleteL(*entry);
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(entry);	
	}


//! @SYMTestCaseID PIM-DataExchange_Test_Alarm_Entry
//! @SYMTestCaseDesc Tests that stored alarm offset is correct
//! @SYMFssID App-Engines/CalInterimAPI/Repeat.005
//! @SYMTestStatus Implemented
//! @SYMTestPriority Medium
//! @SYMTestActions 1) Creates two repeating entries with alarm (with positive and negative offset),
//!					2) Adds a sporadic date that is before the repeating rule
//!					3) Exports entries, imports them and checks that alarm offset was not changed
//! @SYMTestExpectedResults Alarm offset should not be changed
//! @SYMTestType CT
void CAlarmTestManager::TestAlarmEntryL()
	{
	const TInt KAlarmPositiveOffset(60);
	const TInt KAlarmNegativeOffset(-60);
	_LIT8(KGUID1, "AlarmEntry_UID1");
	_LIT8(KGUID2, "AlarmEntry_UID2");
	_LIT(KExportFileName, "exportedalarmentry.vcs");
	
	// Create daily repeating entry
	TTime startTime(TDateTime(1999, EApril, 17, 23, 0, 0, 0));
	TTime endTime(startTime + TTimeIntervalHours(1));
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	HBufC8* guid1 = KGUID1().AllocLC();
	CCalEntry* entry1 = CCalEntry::NewL(CCalEntry::ETodo, guid1, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid1);

	CleanupStack::PushL(entry1);
	entries.AppendL(entry1);
	CleanupStack::Pop(entry1);

	HBufC8* guid2 = KGUID2().AllocLC();
	CCalEntry* entry2 = CCalEntry::NewL(CCalEntry::ETodo, guid2, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid2);

	CleanupStack::PushL(entry2);
	entries.AppendL(entry2);
	CleanupStack::Pop(entry2);
	
	TCalTime startTimeCal;
	startTimeCal.SetTimeLocalL(startTime);
	TCalTime endTimeCal;
	endTimeCal.SetTimeLocalL(endTime);
	entry1->SetStartAndEndTimeL(startTimeCal, endTimeCal);
	entry2->SetStartAndEndTimeL(startTimeCal, endTimeCal);
	
	TCalRRule daily(TCalRRule::EDaily);
	daily.SetDtStart(startTimeCal);
	daily.SetCount(3);

	entry1->SetRRuleL(daily);
	entry1->SetLocationL(_L("location 1"));
	entry2->SetRRuleL(daily);
	entry2->SetLocationL(_L("location 2"));
	
	TCalTime calRDateTime;
	TTime rDateTime(TDateTime(1900, EFebruary, 5, 9, 0, 0, 0));
	calRDateTime.SetTimeLocalL(rDateTime);
	
	RArray<TCalTime> rDates;
	CleanupClosePushL(rDates);
	rDates.AppendL(calRDateTime);
	entry1->SetRDatesL(rDates);
	entry2->SetRDatesL(rDates);
	CleanupStack::PopAndDestroy(&rDates);

	CCalAlarm* alarm1 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm1);
	alarm1->SetTimeOffset(KAlarmPositiveOffset);//60
	entry1->SetAlarmL(alarm1);
	CleanupStack::PopAndDestroy(alarm1);

	CCalAlarm* alarm2 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm2);
	alarm2->SetTimeOffset(KAlarmNegativeOffset);//-60
	entry2->SetAlarmL(alarm2);
	CleanupStack::PopAndDestroy(alarm2);

	TInt success(0);
	
	iTestLibrary->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	TCalLocalUid uid1 = entries[0]->LocalUidL();
	TCalLocalUid uid2 = entries[1]->LocalUidL();

	entries.ResetAndDestroy();

	entry1 = iTestLibrary->SynCGetEntryViewL().FetchL(uid1);
	CleanupStack::PushL(entry1);
	entries.AppendL(entry1);
	CleanupStack::Pop(entry1);
	
	entry2 = iTestLibrary->SynCGetEntryViewL().FetchL(uid2);
	CleanupStack::PushL(entry2);
	entries.AppendL(entry2);
	CleanupStack::Pop(entry2);
	test(entries.Count() == 2);
	iTestLibrary->ExportL(KExportFileName, entries);

	entries.ResetAndDestroy();

	iTestLibrary->ImportL(KExportFileName, entries);
 	test(entries.Count() == 2);

	CCalAlarm* importedAlarm;
	for (TInt i = 0; i < entries.Count(); i++)
		{
		importedAlarm = entries[i]->AlarmL();
		TTimeIntervalMinutes alarmOffset = importedAlarm->TimeOffset();
		delete importedAlarm;
		if (i == 0)
			{
			test(alarmOffset.Int() == KAlarmPositiveOffset);
			}
		else
			{
			test(alarmOffset.Int() == KAlarmNegativeOffset);
			}
		}
	CleanupStack::PopAndDestroy(&entries);
	}


//! @SYMTestCaseID PIM-TCAL-ALARM-0003
//! @SYMTestCaseDesc Test_Alarm_Offset
//! @SYMFssID 
//! @SYMTestStatus Implemented
//! @SYMTestPriority Medium
//! @SYMTestActions 1) Creates three  entries with alarm (with positive and negative offset),
//!	SYMTestExpectedResults Alarm setting should fail without panic according to the day and offset
//! @SYMTestType CIT
void CAlarmTestManager::TestAlarmOffsetL()
	{
	test.Next(_L("@SYMTESTCaseID:PIM-TCAL-ALARM-0003 Alarm Offset tests"));
	TCalTime calStartTime;
	TCalTime calEndTime;
	
	// Create Entry object
	calStartTime.SetTimeLocalL(TDateTime (2006,EJuly,10,1,0,0,0));	
	calEndTime.SetTimeLocalL(TDateTime (2006,EJuly,10,22,0,0,0));	
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EEvent, KGUID1().AllocL(), CCalEntry::EMethodNone, 0);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);

	// Create an alarm object
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
		
	// Offset -1441, alarm greater than a day before event, so should fail
	alarm->SetTimeOffset(-1449);
	TRAPD(Err,entry->SetAlarmL(alarm)) ;
	test(Err == KErrNotSupported);
	
	// Offset -1439, as start time is 60 mins, this is next day, so should fail
	alarm->SetTimeOffset(-1439);
	TRAP(Err,entry->SetAlarmL(alarm)) ;
	test(Err == KErrNotSupported);

	// Offset 1441, no restriction on alarm 1 day before event
	alarm->SetTimeOffset(1441);
	TRAP(Err,entry->SetAlarmL(alarm)) ;
	test(Err == KErrNone);

	test.Printf(_L("Alarm Offset Tests Passed"));
	CleanupStack::PopAndDestroy(2); // alarm, entry
	}


void CAlarmTestManager::ClearAllAlarmsL()
	{
	// Delete all alarms
	RArray<TAlarmCategory> categories;
	
	iAlarmServer.GetAvailableCategoryListL(categories);
	
	const TInt KCount = categories.Count();
	for(TInt i = 0; i < KCount; ++i)
		{
		const TAlarmCategory KCategory = categories[i];
		test(iAlarmServer.AlarmDeleteAllByCategory(KCategory, EFalse) == KErrNone);
		}
		
	categories.Close();

	// Restore alarm sound
	iAlarmServer.SetAlarmSoundState(EAlarmGlobalSoundStateOn);
	}

/**
@SYMTestCaseID  PIM-UT-1090-Test_Enhancements-Case6-ID-0001
@SYMTestCaseDesc This function is used to test the alarm times for both floating and fixed entries. 
The function is re-used for fixed and floating entries depending on the boolean parameter passed in.
@SYMFssID SGL.GT0284.410 PREQ1090 Test Enhancement_v0.1
@SYMPREQ PREQ1090
@SYMTestPriority 3
@SYMTestActions See below:

The following steps of creating the parent and child entries with alarm attachments are implemented in another helper api
1. 	Create a parent and child entry with alarm attachments - these are either Floating or Fixed depending on the parameter
2.	Store the entries

The following steps that involve checking alarm times are implemented thereafter.
3.	Check the alarm has been queued with the alarmserver and they both have correct due time
4.	Change the time zone
5.	Repeat the test step 3 and ensure that the floating time entry has the same due time and the fix time entry has changed due time.
6.	Close the file
7.	Reopen the file again
8.	Repeat the test step 5
9.	Change the time zone back
10.	Repeat the test 5

@SYMTestExpectedResults Alarm time for alarms with attachments for fixed parent and/or child entries should change with change in tz whereas
the alarm time for floating entries should remain the same
@param aIsFloatingEntry - Boolean to test a floating or fixed entries
@return void
@capability writedevicedata
@internalComponent
*/
void CAlarmTestManager::TestFloatingAndFixedEntryAlarmTimeSetupL(TBool aIsFloatingEntry)
	{
	iTestLibrary->CleanDatabaseL();
	
	// Initialise Entries - Parent and Child
	CCalEntry* parentEntry = NULL;
	CCalEntry* childEntry = NULL;
	
	// Create parent and child entries with alarms and attachments
	CreateAndStoreEntriesWithAlarmAttachmentsLC(aIsFloatingEntry, parentEntry, childEntry);
	
	// Record the localuids and entry times for alarm verification in the future
	iFloatingAndFixedData.iParentLocalUid = parentEntry->LocalUidL();
	iFloatingAndFixedData.iChildLocalUid = childEntry->LocalUidL();

	iFloatingAndFixedData.iParentStartTime = parentEntry->StartTimeL();
	iFloatingAndFixedData.iChildStartTime = childEntry->StartTimeL();
	
	iFloatingAndFixedData.iParentAlarmTimeLocal = iFloatingAndFixedData.iParentStartTime.TimeLocalL()-TTimeIntervalMinutes(KAlarmOffset);
	iFloatingAndFixedData.iChildAlarmTimeLocal = iFloatingAndFixedData.iChildStartTime.TimeLocalL()-TTimeIntervalMinutes(KAlarmOffset);
	
	// Start checking alarm times
	
	// Check Alarms before Tz change - Floating and Fixed should have the same alarm time
	iExpectedInstanceTime = iFloatingAndFixedData.iParentStartTime.TimeLocalL();
	CheckAlarmL(iFloatingAndFixedData.iParentAlarmTimeLocal, iFloatingAndFixedData.iParentLocalUid, KNullDesC8, aIsFloatingEntry);
	
	iExpectedInstanceTime = iFloatingAndFixedData.iChildStartTime.TimeLocalL();
	CheckAlarmL(iFloatingAndFixedData.iChildAlarmTimeLocal, iFloatingAndFixedData.iChildLocalUid, KNullDesC8, aIsFloatingEntry);
	
	CleanupStack::PopAndDestroy(2, parentEntry); // parentEntry, childEntry
	}
	
void CAlarmTestManager::TestFloatingAndFixedEntryAlarmTimeCheckL(TBool aIsFloatingEntry)
	{
	// Check alarm time after tz change
	// Floating entries should have the same alarm time whereas fixed entries should have changed alarm times
	if(aIsFloatingEntry)
		{
		iExpectedInstanceTime = iFloatingAndFixedData.iParentStartTime.TimeLocalL();
		CheckAlarmL(iFloatingAndFixedData.iParentAlarmTimeLocal, iFloatingAndFixedData.iParentLocalUid, KNullDesC8, aIsFloatingEntry);
		
		iExpectedInstanceTime = iFloatingAndFixedData.iChildStartTime.TimeLocalL();
		CheckAlarmL(iFloatingAndFixedData.iChildAlarmTimeLocal, iFloatingAndFixedData.iChildLocalUid, KNullDesC8, aIsFloatingEntry);
		}
	else
		{
		iExpectedInstanceTime = iFloatingAndFixedData.iParentStartTime.TimeLocalL();
		CheckAlarmL(iFloatingAndFixedData.iParentAlarmTimeLocal+TTimeIntervalHours(1), iFloatingAndFixedData.iParentLocalUid, KNullDesC8, aIsFloatingEntry);
		
		iExpectedInstanceTime = iFloatingAndFixedData.iChildStartTime.TimeLocalL();
		CheckAlarmL(iFloatingAndFixedData.iChildAlarmTimeLocal+TTimeIntervalHours(1), iFloatingAndFixedData.iChildLocalUid, KNullDesC8, aIsFloatingEntry);	
		}

	// Close and re-open calendar file
	delete iTestLibrary;
	iTestLibrary = NULL;
	
	iTestLibrary = CCalTestLibrary::NewL(EFalse);
	iTestLibrary->OpenFileL(KCalendarFile());
	iTestLibrary->SynCGetEntryViewL(); // make the indexes get built
	}

void CAlarmTestManager::TestFloatingAndFixedEntryAlarmTimeReopenAndCheckL(TBool aIsFloatingEntry)
	{
	// Check alarm times after closing and re-opening the calendar file
	// Floating entries should have the same alarm time whereas fixed entries should have changed alarm times
	if(aIsFloatingEntry)
		{
		iExpectedInstanceTime = iFloatingAndFixedData.iParentStartTime.TimeLocalL();
		CheckAlarmL(iFloatingAndFixedData.iParentAlarmTimeLocal, iFloatingAndFixedData.iParentLocalUid, KNullDesC8, aIsFloatingEntry);
		
		iExpectedInstanceTime = iFloatingAndFixedData.iChildStartTime.TimeLocalL();
		CheckAlarmL(iFloatingAndFixedData.iChildAlarmTimeLocal, iFloatingAndFixedData.iChildLocalUid, KNullDesC8, aIsFloatingEntry);
		}
	else
		{
		iExpectedInstanceTime = iFloatingAndFixedData.iParentStartTime.TimeLocalL();
		CheckAlarmL(iFloatingAndFixedData.iParentAlarmTimeLocal+TTimeIntervalHours(1), iFloatingAndFixedData.iParentLocalUid);
		
		iExpectedInstanceTime = iFloatingAndFixedData.iChildStartTime.TimeLocalL();
		CheckAlarmL(iFloatingAndFixedData.iChildAlarmTimeLocal+TTimeIntervalHours(1), iFloatingAndFixedData.iChildLocalUid);	
		}
	}

void CAlarmTestManager::TestFloatingAndFixedEntryAlarmTimeCheckAgainL(TBool aIsFloatingEntry)
	{
	iExpectedInstanceTime = iFloatingAndFixedData.iParentStartTime.TimeLocalL();
	CheckAlarmL(iFloatingAndFixedData.iParentAlarmTimeLocal, iFloatingAndFixedData.iParentLocalUid, KNullDesC8, aIsFloatingEntry);
		
	iExpectedInstanceTime = iFloatingAndFixedData.iChildStartTime.TimeLocalL();
	CheckAlarmL(iFloatingAndFixedData.iChildAlarmTimeLocal, iFloatingAndFixedData.iChildLocalUid, KNullDesC8, aIsFloatingEntry);	
	}

/**
@SYMTestCaseID  PIM-UT-1090-Test Enhancements-Case6-ID-0002
@SYMTestCaseDesc This function is used to create a parent and a child calendar entry with alarms and alarm attachments
The function is re-used for fixed and floating entries depending on the boolean parameter passed in.
@SYMFssID SGL.GT0284.410 PREQ1090 Test Enhancement_v0.1
@SYMPREQ PREQ1090
@SYMTestPriority 3
@SYMTestActions See below:
1. 	Create a parent and child entry with alarm attachments - these are either Floating or Fixed depending on the parameter
2.	Store the entries
@SYMTestExpectedResults Two floating or fixed parent and child entries with alarms and alarm attachments created and 
assigned to the pointers passed in as parameters
@param aIsFloatingEntry - Boolean to test a floating or fixed entries
@param aParentEntry - A reference to a pointer to a parent entry
@param aChildEntry - A reference to a pointer to a child entry
@return void
@internalComponent
*/
void CAlarmTestManager::CreateAndStoreEntriesWithAlarmAttachmentsLC(TBool aIsFloatingEntry, CCalEntry*& aParentEntry, CCalEntry*& aChildEntry)
	{
	_LIT8(KGUID_Floating,  "TE_Case6_Floating_UID");
	_LIT8(KGUID_Fixed,  "TE_Case6_Fixed_UID");
	
	// Initialise parent and child entry start and end times
	TCalTime calParentStartTime;
	TCalTime calParentEndTime;
	TCalTime calChildStartTime;
	TCalTime calChildEndTime;
	
	// Setup - Tz to start the use case in
	SetTimeZoneL(KEuropeLondon);
	iTestLibrary->PIMTestServer().SetHomeTime(TTime(TDateTime(2006, EJanuary, 5, 8, 0, 0, 0))); // 6th January 2006 at 0800
	
	HBufC8* guidParent = NULL;
	
	// Floating or fixed entry
	if(aIsFloatingEntry)
		{
		 guidParent = KGUID_Floating().AllocLC();
		}
	else
		{
		guidParent = KGUID_Fixed().AllocLC();
		}
	
	// Floating or Fixed
	if(aIsFloatingEntry)
		{
		// Parent and Child Entries - Floating
		calParentStartTime.SetTimeLocalFloatingL(TTime(TDateTime(2006, EJanuary, 5, 9, 0, 0, 0))); 	// 6th January 2006 at 0900
		calParentEndTime.SetTimeLocalFloatingL(TTime(TDateTime(2006, EJanuary, 5, 10, 0, 0, 0)));  	// 6th January 2006 at 1000

		// Parent and Child Entries - Floating
		calChildStartTime.SetTimeLocalFloatingL(TTime(TDateTime(2006, EJanuary, 10, 11, 0, 0, 0))); 	// 11th January 2006 at 1100
		calChildEndTime.SetTimeLocalFloatingL(TTime(TDateTime(2006, EJanuary, 10, 12, 0, 0, 0)));  	// 11th January 2006 at 1200
		}
	else
		{
		// Parent and Child Entries - Fixed
		calParentStartTime.SetTimeLocalL(TTime(TDateTime(2006, EJanuary, 18, 13, 30, 0, 0))); 	// 19th January 2006 at 1330
		calParentEndTime.SetTimeLocalL(TTime(TDateTime(2006, EJanuary, 18, 14, 30, 0, 0)));  	// 19th January 2006 at 1430

		// Parent and Child Entries - Fixed
		calChildStartTime.SetTimeLocalL(TTime(TDateTime(2006, EJanuary,20, 15, 30, 0, 0))); 	// 21th January 2006 at 1530
		calChildEndTime.SetTimeLocalL(TTime(TDateTime(2006, EJanuary, 20, 16, 30, 0, 0)));  	// 21th January 2006 at 1630
		}

	// == Parent Entry ==
	// Parent Entry - Create one
	aParentEntry = CCalEntry::NewL(CCalEntry::EAppt, guidParent, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guidParent);
	CleanupStack::PushL(aParentEntry);

	// Parent Entry - Start and End Times
	aParentEntry ->SetStartAndEndTimeL(calParentStartTime, calParentEndTime);

	// Parent Entry - Rpt Rule - Daily
	TCalRRule parentRrule(TCalRRule::EDaily);
	parentRrule.SetDtStart(calParentStartTime);
	parentRrule.SetCount(10);
	parentRrule.SetInterval(1);	
	aParentEntry->SetRRuleL(parentRrule);	

	// Parent Entry - Alarm - Create an alarm with content
	CCalAlarm *parentAlarm = CCalAlarm::NewL();
	CleanupStack::PushL(parentAlarm);
	parentAlarm->SetTimeOffset(TTimeIntervalMinutes(KAlarmOffset)) ;
	
	// Parent Entry - Alarm Content
	CCalContent *parentAlarmContent = CCalContent::NewL();
	CleanupStack::PushL(parentAlarmContent);
	_LIT8(KParentAlarmContentUrl, "www.example-site.com");
	_LIT8(KParentAlarmContentUrlMimeType,"text/http");
	
	// SetContentL takes ownership of the URL and Mime data/link
	parentAlarmContent->SetContentL(KParentAlarmContentUrl().AllocLC(),KParentAlarmContentUrlMimeType().AllocLC(),CCalContent::EDispositionUrl);
	CleanupStack::Pop(2); // KParentAlarmContentUrl, KParentAlarmContentUrlMimeType
	// Alarm takes ownership of the alarm content
	parentAlarm->SetAlarmAction(parentAlarmContent);
	CleanupStack::Pop(parentAlarmContent);
	// Entry does not take ownership of the alarm
	aParentEntry->SetAlarmL(parentAlarm);
	CleanupStack::PopAndDestroy(parentAlarm);
	
	// == Child Entry ==
	// Child Entry - Recurrence Id
	TCalTime childRecId;
	
	// Fixed or Floating entry
	if(aIsFloatingEntry)
		{
		childRecId.SetTimeLocalFloatingL(TTime(TDateTime(2006, EJanuary, 10, 9, 0, 0, 0))); // 11th Jan 2006 0900
		}
	else
		{
		childRecId.SetTimeLocalL(TTime(TDateTime(2006, EJanuary, 20, 13, 30, 0, 0))); // 21th Jan 2006 1330 
		}
	
	HBufC8* guidChild = NULL;
	
	if(aIsFloatingEntry)
		{
		guidChild = KGUID_Floating().AllocLC();
		}
	else
		{
		guidChild = KGUID_Fixed().AllocLC();
		}
	
	// Child Entry - Create One
	aChildEntry = CCalEntry::NewL(CCalEntry::EAppt, guidChild, CCalEntry::EMethodNone, 1,childRecId,CalCommon::EThisAndFuture);
	CleanupStack::Pop(guidChild);
	CleanupStack::PushL(aChildEntry);
	
	// Child Entry -  Start and End Times
	aChildEntry->SetStartAndEndTimeL(calChildStartTime, calChildEndTime);
	
	// Child Entry - Alarm - Create an alarm with content
	CCalAlarm* childAlarm = CCalAlarm::NewL();
	CleanupStack::PushL(childAlarm);
	childAlarm->SetTimeOffset(TTimeIntervalMinutes(KAlarmOffset));
	
	// Child Entry - Alarm Content
	CCalContent *childAlarmContent = CCalContent::NewL();
	CleanupStack::PushL(childAlarmContent);
	_LIT8(KChildAlarmContentUrl, "www.another-example-site.com");
	_LIT8(KChildAlarmContentUrlMimeType,"text/http");
	// SetContentL takes ownership of the URL and Mime data/link
	childAlarmContent->SetContentL(KChildAlarmContentUrl().AllocLC(),KChildAlarmContentUrlMimeType().AllocLC(),CCalContent::EDispositionUrl);
	CleanupStack::Pop(2); // KChildAlarmContentUrl, KChildAlarmContentUrlMimeType

	// Alarm takes ownership of the alarm content
	childAlarm->SetAlarmAction(childAlarmContent);
	CleanupStack::Pop(childAlarmContent);
	// Entry does not take ownership of the alarm
	aChildEntry->SetAlarmL(childAlarm);
	CleanupStack::PopAndDestroy(childAlarm);
	
	// == Storing Entries ==
	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	
	// Append parent and child entries
	entriesToStore.AppendL(aParentEntry);
	entriesToStore.AppendL(aChildEntry);
	
	// Store the entries
	TInt entriesStored = 0;
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);
	CleanupStack::PopAndDestroy(&entriesToStore);
	}
	
void CAlarmTestManager::Start()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	if ( ! IsActive())
		{
		SetActive();
		}
	}
	
void CAlarmTestManager::DoCancel()
	{
	}

void CAlarmTestManager::TestChangeSystemTimeL(TBool aIsFloating, TBool aChangeSysTimeSameAsAlarmTime)
	{
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();

	TTime now;
	now.HomeTime();
	AddEntryWithAlarmL(now + TTimeIntervalHours(1) ,0,  NULL, aIsFloating); // Add an alarm one hour ahead in the future
	test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1); // One alarm should be quequed
	if (aChangeSysTimeSameAsAlarmTime)
		{
		iTestLibrary->PIMTestServer().SetHomeTime(now + TTimeIntervalHours(1)); // Change system time exactly same as the alarm's due time
		}
	else
		{
		iTestLibrary->PIMTestServer().SetHomeTime(now + TTimeIntervalHours(2)); // Change system time ahead of alarm's due time
		}
		
	iTimeSysTimeChanged.UniversalTime();
	
	User::After(2000000);
	if (iQueueAlarm == ESkippedAlarmsQueueAndExpire || aChangeSysTimeSameAsAlarmTime)
		{
		//  The alarm should be notified if its due time is "now"
		// Missed alarm should be notified by the defalut behaviour currently.
		test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 0);//Alarms in the past have been notified so no alarm should be queued.
		}
	else
		{//Otherwise the alarm in the past will be dequeued when the system time has been put forward
		RArray<TAlarmId> alarmIds;
   		CleanupClosePushL(alarmIds);

 		iAlarmServer.GetAlarmIdListL(alarmIds);
 	
   		test(alarmIds.Count()==0);
  		CleanupStack::PopAndDestroy(&alarmIds);
		TestPubSubValue(EAlarmMissed);
		}
	//Reset the environment 
	ClearAllAlarmsL();
	
	if (aChangeSysTimeSameAsAlarmTime)
		{
		iTestLibrary->PIMTestServer().SetHomeTime(now - TTimeIntervalHours(1));
		}
	else
		{
		iTestLibrary->PIMTestServer().SetHomeTime(now - TTimeIntervalHours(2));
		}
	}
	
void CAlarmTestManager::TestNotifiedAlarmL()
	{//This is to test fix for INC127511. That is, notified alarms is not missed alarm.
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();
	
	TTime now;
	now.HomeTime();
	
	TTime alarmTime(TDateTime(2008, EApril, 5, 12, 0, 0, 0));

	iTestLibrary->PIMTestServer().SetHomeTime(alarmTime - KAlarmExpiryBackOffPeriod); 

	AddEntryWithAlarmL(alarmTime, 0,  NULL, EFalse); // Add an alarm one minute ahead in the future
	test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1); // One alarm should be quequed
	User::After(2000000);

	iTestLibrary->PIMTestServer().SetHomeTime(alarmTime + TTimeIntervalSeconds(5)); // Change system time ahead of alarm's due time
	test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 0);//Alarms in the past have been notified so no alarm should be queued.
	TestPubSubValue(ENoChange);
	//Reset the environment 
	ClearAllAlarmsL();
	
	iTestLibrary->PIMTestServer().SetHomeTime(now);
	}	
	
void CAlarmTestManager::AddFourEntriesWithAlarmsL()
	{
	iTestLibrary->CleanDatabaseL();
	ClearAllAlarmsL();
	
	TTime now;
	now.HomeTime();

	//Add a floating alarm one hour ahead
	_LIT8(KGuid1, "guid1");
	HBufC8* guid1 = KGuid1().AllocLC(); 
	AddEntryWithAlarmL(now + TTimeIntervalHours(1) ,ETrue,  guid1); 
	//Add a floating alarm two hours ahead
	_LIT8(KGuid2, "guid2");
	HBufC8* guid2 = KGuid2().AllocLC(); 
	AddEntryWithAlarmL(now + TTimeIntervalHours(2) ,ETrue,  guid2);

	//Add a fixed alarm one hour ahead
	_LIT8(KGuid3, "guid3");
	HBufC8* guid3 = KGuid3().AllocLC(); 
	AddEntryWithAlarmL(now + TTimeIntervalHours(1) ,EFalse,  guid3);

	//Add a fixed alarm two hours ahead
	_LIT8(KGuid4, "guid4");
	HBufC8* guid4 = KGuid4().AllocLC(); 
	AddEntryWithAlarmL(now + TTimeIntervalHours(2) ,EFalse,  guid4);
	}


void CAlarmTestManager::TestFloatingAlarmNotificationL()
	{
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();
	
	iTestLibrary->PIMTestServer().SetTimeZoneL(_L8("Asia/Calcutta"));
	
	TTime now;
	now.HomeTime();
	AddEntryWithAlarmL(now + TTimeIntervalMinutes(30) ,0,  NULL, ETrue);  // Add an alarm one hour ahead in the future
	test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1); // One alarm should be quequed
	
	iTimeSysTimeChanged.UniversalTime();
								
	iTestLibrary->PIMTestServer().SetTimeZoneL(_L8("Asia/Singapore"));	
	
	User::After(2000000);
	
	if (iQueueAlarm == ESkippedAlarmsQueueAndExpire)
		{
		//  The alarm should be notified if its due time is "now"
		// Missed alarm should be notified by the defalut behaviour currently.
		test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 0);//Alarms in the past have been notified so no alarm should be queued.
		}
	else
		{//Otherwise the alarm in the past will be dequeued when the system time has been put forward
		RArray<TAlarmId> alarmIds;
   		CleanupClosePushL(alarmIds);

 		iAlarmServer.GetAlarmIdListL(alarmIds);
 	
   		test(alarmIds.Count() == 0);
  		CleanupStack::PopAndDestroy(&alarmIds);
		TestPubSubValue(EAlarmMissed);
		}
		
	//Reset the environment 
	ClearAllAlarmsL();
	
	iTestLibrary->PIMTestServer().SetTimeZoneL(_L8("Europe/London"));
	}
	
// Testing that when the calendar server open a new file, it should not delete the alarm in the old file
void CAlarmTestManager::TestAlarmNotDeletedInDiffFileL()
    {
    ClearAllAlarmsL();
    iTestLibrary->CleanDatabaseL();
     
    TTime now;
    now.HomeTime();
    
    TTime alarmTime(TDateTime(2008, EApril, 5, 12, 0, 0, 0));
    iTestLibrary->PIMTestServer().SetHomeTime(alarmTime - KAlarmExpiryBackOffPeriod); 

    AddEntryWithAlarmL(alarmTime, 0,  NULL, EFalse); // Add an alarm 5 seconds ahead in the future
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1); // One alarm should be quequed
   
    iTestLibrary->ReplaceFileL(KCalendarFile2());
    iTestLibrary->OpenFileL(KCalendarFile2());
    iTestLibrary->SynCGetEntryViewL();//force the entry view is build so that the alarms will be deleted.
    
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1); // One alarm should be still quequed
    //Wait the alarm to go off
    test(iTestLibrary->WaitForAlarmEvent(KNullAlarmId, EAlarmChangeEventState, EAlarmStateNotifying, KAlarmExpiryBackOffPeriod.Int()*2*1000000));   

     
    iTestLibrary->DeleteFileL(KCalendarFile2());
    iTestLibrary->ReplaceFileL(KCalendarFile());
    iTestLibrary->OpenFileL(KCalendarFile());
    ClearAllAlarmsL();
    iTestLibrary->PIMTestServer().SetHomeTime(now);
    }

//Testing that when calendar server open a file, it should delete all alarms in the same file
void CAlarmTestManager::TestAlarmDeletedInSameFileL()
    {
    ClearAllAlarmsL();
    iTestLibrary->CleanDatabaseL();
     
    TTime now;
    now.HomeTime();
    const TInt offSet = 11;// alarm offset is 11 seconds
    TTime alarmTime(TDateTime(2008, EApril, 5, 12, 0, 0, 0));
    iTestLibrary->PIMTestServer().SetHomeTime(alarmTime - TTimeIntervalSeconds(offSet)); 

    HBufC8* guid1 = KGUID1().AllocLC();
    AddEntryWithAlarmL(alarmTime, 0,  guid1, EFalse); // Add an alarm 5 seconds ahead in the future
   
    HBufC8* guid2 = KGUID2().AllocLC();
    AddEntryWithAlarmL(alarmTime, 0,  guid2, EFalse); // Add an alarm 5 seconds ahead in the future
       
    iTestLibrary->CloseAgendaServer();
    test.Printf(_L("Calendar server shut down\n")); 

    // Two alarm should be quequed
    test(iAlarmServer.GetAlarmCountForCategory(KUidAgendaModelAlarmCategory) == 2);
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 2); 
    iTestLibrary->OpenFileL(KCalendarFile());
    test(iAlarmServer.GetAlarmCountForCategory(KUidAgendaModelAlarmCategory) == 2);
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 2); 
    iTestLibrary->SynCGetEntryViewL();//force the entry view is build so that the alarms will be deleted.

    // One alarm should be quequed
    test(iAlarmServer.GetAlarmCountForCategory(KUidAgendaModelAlarmCategory) == 1);
    test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1);
    //Wait the alarm to go off
    test(iTestLibrary->WaitForAlarmEvent(KNullAlarmId, EAlarmChangeEventState, EAlarmStateNotifying, offSet*1000000));   
    ClearAllAlarmsL();
    iTestLibrary->CleanDatabaseL();
    iTestLibrary->PIMTestServer().SetHomeTime(now);
    }

//Testing that when a calendar file is deleted, all alarms in the file should be deleted.
void CAlarmTestManager::TestAlarmDeletedWhenFileDeletedL()
    {
    ClearAllAlarmsL();
    iTestLibrary->CleanDatabaseL();
     
    TTime now;
    now.HomeTime();
    const 
    TTime alarmTime(TDateTime(2008, EApril, 5, 12, 0, 0, 0));
    iTestLibrary->PIMTestServer().SetHomeTime(alarmTime - TTimeIntervalSeconds(11)); 

    HBufC8* guid1 = KGUID1().AllocLC();
    AddEntryWithAlarmL(alarmTime, 0,  guid1, EFalse); // Add an alarm 5 seconds ahead in the future
    test(iAlarmServer.GetAlarmCountForCategory(KUidAgendaModelAlarmCategory) == 1);
   
    HBufC8* guid2 = KGUID2().AllocLC();
    AddEntryWithAlarmL(alarmTime, 0,  guid2, EFalse); // Add an alarm 5 seconds ahead in the future
    test(iAlarmServer.GetAlarmCountForCategory(KUidAgendaModelAlarmCategory) == 1);
       
    iTestLibrary->DeleteFileL(KCalendarFile());
    test(iAlarmServer.GetAlarmCountForCategory(KUidAgendaModelAlarmCategory) == 0);
  
    iTestLibrary->ReplaceFileL(KCalendarFile());
    iTestLibrary->OpenFileL(KCalendarFile());
    ClearAllAlarmsL();
    iTestLibrary->PIMTestServer().SetHomeTime(now);
    }
// Testing that large alarm attachments from external applications
// do not panic the agenda server  when deleting calendar entries
// that have alarm attachments
void CAlarmTestManager::CheckAlarmAttachmentSizeL()
	{
	_LIT(KStepTitle, "Testing external alarm size influence on entry deletion");
	test.Next(KStepTitle);	
	// Queue an alarm with a large attachment
	_LIT(KQueueAlarm, "Create and queue an alarm with a large attachment");
	test.Printf(KQueueAlarm);
	
	const TTimeIntervalHours KOneHourDelay(1);
	TASShdAlarm bigAttachAlarm;
	
	bigAttachAlarm.NextDueTime().HomeTime();
	bigAttachAlarm.NextDueTime() += KOneHourDelay;
	
	const TInt KProblemSize(1024);
	HBufC8* problemData = HBufC8::NewLC(KProblemSize);
	for (TInt i = 0; i < KProblemSize; ++i)
		{
		problemData->Des().Append(TChar('X'));
		}
		
	User::LeaveIfError(iAlarmServer.AlarmAdd(bigAttachAlarm, *problemData));
	CleanupStack::PopAndDestroy(problemData);
	
	// Create entry with alarm attachment
	_LIT(KCalEntryAttach, "Create a calendar entry with an alarm attachment");
	test.Printf(KCalEntryAttach);
	
	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
	
	_LIT8(KEntryGuid, "large-attach-guid");
	HBufC8* guidBuf = KEntryGuid().AllocLC();
	
	CCalEntry* calEntry = iTestLibrary->CreateCalEntryL(CCalEntry::EAppt, guidBuf);
	CleanupStack::Pop(guidBuf);
	
	TTime time = TDateTime(2008, EJuly, 17, 0, 0, 0, 0);
	iTestLibrary->SetEntryStartAndEndTimeL(calEntry, time, time + KOneHourDelay);
		
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	calEntry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	
	entriesToStore.AppendL(calEntry);
	
	TInt countStored;
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, countStored);
	
	_LIT(KCheckEntryStored, "Test entry was store successfully");
	test.Printf(KCheckEntryStored);
	test(countStored == 1);
	
	// Deleting entry will cause panic if client can't deal with
	// large attachments in alarm queue
	_LIT(KDeleteEntry, "Attempting to delete entry");
	test.Printf(KDeleteEntry);
	iTestLibrary->SynCGetEntryViewL().DeleteL(*calEntry);
	
	CleanupStack::PopAndDestroy(&entriesToStore);
	}

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
void CAlarmTestManager::CheckNextQueuedAlarmL(const TTime& aAlarmTime)
	{
	TAlarmId alarmId;
	User::LeaveIfError(iAlarmServer.GetNextDueAlarmId(alarmId));

	TASShdAlarm aAlarm;
	User::LeaveIfError(iAlarmServer.GetAlarmDetails(alarmId, aAlarm));
	TTimeIntervalSeconds utcOffset = User::UTCOffset();
	TTime nextDueTime = aAlarm.NextDueTime() + User::UTCOffset();
	test(nextDueTime == aAlarmTime);
	}

void CAlarmTestManager::RetrieveAlarmedInstancesLC(RPointerArray<CCalInstance>& aInstances)
	{
	TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf; 
	RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf);
	RetrieveAlarmedInstancesLC(aInstances, instanceParamsBuf);
	}

void CAlarmTestManager::RetrieveAlarmedInstancesLC(RPointerArray<CCalInstance>& aInstances, 
												   TPckgBuf<TASShdAlarmedInstanceParams>& aParamsBuf)
	{
	test.Printf(_L("Retrieving alarmed instances\n"));		
	CCalSession& session = iTestLibrary->GetSession();
	CCalInstanceView* instanceView = CCalInstanceView::NewL(session);
	CleanupStack::PushL(instanceView);	
	instanceView->FindAlarmedInstanceL(aInstances, aParamsBuf());
	CleanupStack::PopAndDestroy(instanceView);	
	CleanupClosePushL(aInstances);	
	}

void CAlarmTestManager::WaitForAlarmAdditionL()
	{
	TRequestStatus status;
	while (status.Int() != EAlarmChangeEventAlarmData && status.Int() != EAlarmChangeEventAlarmAddition)
		{
		TAlarmId alarmId;
		iAlarmServer.NotifyChange(status, alarmId);
		User::WaitForRequest(status);
		User::LeaveIfError(status.Int());
		}
	}

void CAlarmTestManager::CreateAndAddEntriesL(TBool aFloating1, const TTime& aTime1, TBool aFloating2, const TTime& aTime2)
	{
	test.Printf(_L("Creating appointments\n"));
	CCalEntry* entry1 = CreateAlarmedEntryLC(CCalEntry::EAppt, aTime1, aTime1, 0, KGUID1().AllocLC(), aFloating1);
	CCalEntry* entry2 = CreateAlarmedEntryLC(CCalEntry::EAppt, aTime2, aTime2, 0, KGUID2().AllocLC(), aFloating2);
	
	test.Printf(_L("Adding alarmed entries\n"));	
	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	entriesToStore.AppendL(entry1);
	entriesToStore.AppendL(entry2);	
	TInt entriesStored = 0;	
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);

	WaitForAlarmAdditionL();
	
	CleanupStack::PopAndDestroy(&entriesToStore);	
	CleanupStack::PopAndDestroy(entry2);
	CleanupStack::PopAndDestroy(entry1);
	}

void CAlarmTestManager::CreateAndAddEntryL(TBool aFloating, const TTime& aTime)
	{
	test.Printf(_L("Creating appointment\n"));
	CCalEntry* entry1 = CreateAlarmedEntryLC(CCalEntry::EAppt, aTime, aTime, 0, KGUID1().AllocLC(), aFloating);
	
	test.Printf(_L("Adding alarmed entries\n"));	
	RPointerArray<CCalEntry> entriesToStore;
	CleanupClosePushL(entriesToStore);
	entriesToStore.AppendL(entry1);
	TInt entriesStored = 0;	
	iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	test(entriesToStore.Count() == entriesStored);	
	
	WaitForAlarmAdditionL();
	
	CleanupStack::PopAndDestroy(&entriesToStore);	
	CleanupStack::PopAndDestroy(entry1);
	}

/**
@SYMTestCaseID PIM-ALS-MAL-NOT-0003
@SYMTestCaseDesc Verify that a system time change causes skipped alarms and the
skipped alarms can be retrieved from Calendar.
@SYMPREQ PREQ2088
@SYMTestPriority 3
@SYMTestActions
1.	Set skipped alarms behaviour to "notify subscriber".
2.	Set system time to be on a Sunday.
3.	Create an appointment with an alarm to expire at 10:00 on Monday.
4.	Create an appointment with an alarm to expire at 10:00 on Wednesday.
5.	Change system time forward to 09:59 on Wednesday so that the alarm set to expire at 10:00 on Monday is skipped.
6.	Query Calendar for alarmed Calendar instances using Alarm Server skipped alarm instances property value.
@SYMTestExpectedResults 
1.	Alarm does not expire
2.	The alarm due to expire at 10:00 on Wednesday is unaffected and expires next.
3.	The number of skipped alarms is one.
*/
void CAlarmTestManager::NotifySubscriberWithCalendarRunningL()
	{
	_LIT(KStepTitle, "@SYMTESTCaseID:PIM-ALS-MAL-NOT-0003 Testing skipped alarm with calendar running");
	test.Next(KStepTitle);
	
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();	
	
	TTime now;
	now.HomeTime();
	
	test.Printf(_L("Setting subscriber notification to ON\n"));
	SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);
		
	test.Printf(_L("Setting system time to Sunday, Nov 2, 2008 10:00\n"));
	const TTime KSunday(TDateTime(2008, ENovember, 2, 10, 0, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KSunday);
	
	const TTime KMonday(TDateTime(2008, ENovember, 3, 10, 0, 0, 0));
	const TTime KWednesday(TDateTime(2008, ENovember, 5, 10, 0, 0, 0));
	CreateAndAddEntriesL(EFalse, KMonday, EFalse, KWednesday);

	const TTime K959Wednesday(TDateTime(2008, ENovember, 5, 9, 59, 0, 0));
	
	// Change time and verify no alarms expired
	CheckNoAlarmWentOffL(K959Wednesday);
	
	// The next due alarm should be Wednesday
	CheckNextQueuedAlarmL(KWednesday);
	
    RPointerArray<CCalInstance> instanceList;
    RetrieveAlarmedInstancesLC(instanceList);
    
	test(instanceList.Count() == 1);						// Verify that only one alarm was missed
	test(instanceList[0]->Time().TimeLocalL() == KMonday);	// Verify the alarm was Monday's (alarm offest is 0)
		
	// Verify that the Wednesday alarm expires as usual
	const TTime K1001Wednesday(TDateTime(2008, ENovember, 5, 10, 1, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(K1001Wednesday);
	test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 0);	
	
	// Cleanup
	instanceList.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&instanceList);
	iTestLibrary->PIMTestServer().SetHomeTime(now);
	}

/**
@SYMTestCaseID PIM-ALS-MAL-NOT-0004
@SYMTestCaseDesc Verify that a system time change causes skipped alarms and the
details of the change is published when Calendar is not running.
@SYMPREQ PREQ2088
@SYMTestPriority 3
@SYMTestActions
1.	Set skipped alarms behaviour to "notify subscriber".
2.	Set system time to be on a Sunday.
3.	Create an appointment with an alarm to expire at 10:00 on Monday.
4.	Create an appointment with an alarm to expire at 10:00 on Wednesday.
5.	Stop Calendar.
6.	Change system time forward to 09:59 on Wednesday so that the alarm set to expire at 10:00 Monday is skipped.
7.	Query Calendar for alarmed Calendar instances using Alarm Server skipped alarm instances property value.
@SYMTestExpectedResults 
1.  One skipped alarm instance is retrieved from Calendar.
2.  The alarm on Monday does not expire.
3.  The alarm on Wednesday is the next alarm to expire.
4.  The number of skipped alarms is one.
*/
void CAlarmTestManager::NotifySubscriberWithoutCalendarRunningL()
	{
	_LIT(KStepTitle, "@SYMTESTCaseID:PIM-ALS-MAL-NOT-0004 Testing skipped alarm without the calendar running");
	test.Next(KStepTitle);
	
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();	
	
	TTime now;
	now.HomeTime();
	
	test.Printf(_L("Setting subscriber notification to ON\n"));
	SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);
		
	test.Printf(_L("Setting system time to Sunday, Nov 2, 2008 10:00\n"));
	const TTime KSunday(TDateTime(2008, ENovember, 2, 10, 0, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KSunday);
	
	const TTime KMonday(TDateTime(2008, ENovember, 3, 10, 0, 0, 0));
	const TTime KWednesday(TDateTime(2008, ENovember, 5, 10, 0, 0, 0));
	CreateAndAddEntriesL(EFalse, KMonday, EFalse, KWednesday);	
	
	// Stop Calendar
	test.Printf(_L("Shutting down the calendar server\n"));	
	iTestLibrary->CloseAgendaServer();
	test.Printf(_L("Calendar server shut down\n"));	
		
	const TTime K959Wednesday(TDateTime(2008, ENovember, 5, 9, 59, 0, 0));
	
	// Change time and verify no alarms expired
	CheckNoAlarmWentOffL(K959Wednesday);
	
	// The next due alarm should be Wednesday
	CheckNextQueuedAlarmL(KWednesday);

	test.Printf(_L("Restarting calendar server\n"));	
	iTestLibrary->ConnectAgendaServerL();
	iTestLibrary->OpenFileL(KCalendarFile());
	
    RPointerArray<CCalInstance> instanceList;
    RetrieveAlarmedInstancesLC(instanceList);
	
	test(instanceList.Count() == 1);						// Verify that only one alarm was missed
	test(instanceList[0]->Time().TimeLocalL() == KMonday);	// Verify the alarm was Monday's (alarm offest is 0)
		
	// Verify that the Wednesday alarm expires as usual
	const TTime K1001Wednesday(TDateTime(2008, ENovember, 5, 10, 1, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(K1001Wednesday);

	test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 0);	
	
	// Cleanup
	instanceList.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&instanceList);
	iTestLibrary->PIMTestServer().SetHomeTime(now);
	}

/**
@SYMTestCaseID PIM-ALS-MAL-NOT-0005
@SYMTestCaseDesc Verify that skipped alarms and their details can be retrieved
after a simulated power cycle (stop and start Alarm Server).
@SYMPREQ PREQ2088
@SYMTestPriority 3
@SYMTestActions
1.	Set system time to 12:00 on Sunday.
2.	Create an appointment with an alarm to expire at 10:00 on Monday.
3.	Create an appointment with an alarm to expire at 10:00 on Wednesday.
4.	Set the system time forward to 12:00 on Thursday.
5.	Stop Alarm Server.
6.	Start Alarm Server.
7.	Query Calendar for alarmed Calendar instances using Alarm Server skipped alarm instances property value.
Repeat this test case changing the time zone instead of the system time.
@SYMTestExpectedResults There are two skipped alarms.
*/
void CAlarmTestManager::RetrieveSkippedAlarmsAfterRestartL(TSystemTimeOrUTC aSystemTimeOrUTC)
	{
	_LIT(KStepTitle, "@SYMTESTCaseID:PIM-ALS-MAL-NOT-0005 Testing skipped alarms can be retrieved after a restart");
	test.Next(KStepTitle);
	
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();
		
	TTime now;
	now.HomeTime();
	
	test.Printf(_L("Setting subscriber notification to ON\n"));
	SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);		
	
	SetTimeZoneL(KEuropeLondon);	
			
	test.Printf(_L("Setting system time to Sunday, Nov 2, 2008 12:00\n"));
	const TTime KSunday(TDateTime(2008, ENovember, 2, 12, 0, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KSunday);
	
	const TTime KMonday(TDateTime(2008, ENovember, 3, 10, 0, 0, 0));
	const TTime KWednesday(TDateTime(2008, ENovember, 5, 10, 0, 0, 0));	
	CreateAndAddEntriesL(EFalse, KMonday, EFalse, KWednesday);
	
	// Store Skipped Alarm Instances data before the time change
	TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf; 
	RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf);	
		
	if (aSystemTimeOrUTC == ESystemTime)
		{
		// Change time to Thursday, Oct 9, 2008 12:00
		const TTime KThursday(TDateTime(2008, ENovember, 6, 12, 0, 0, 0));
		iTestLibrary->PIMTestServer().SetHomeTime(KThursday);
		}
	else
		{
		// Change time to right before the Monday alarm, then change the timezone
		const TTime KEarlyMonday(TDateTime(2008, ENovember, 3, 9, 55, 0, 0));
		iTestLibrary->PIMTestServer().SetHomeTime(KEarlyMonday);
		SetTimeZoneL(KEuropeHelsinki);		
		}
	
	// Store Skipped Alarm Instances data before the server restart
	TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf2; 
	RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf2);

	test.Printf(_L("Stopping Alarm Server\n"));
	iAlarmServer.__DbgShutDownServer();
	iAlarmServer.Close();
	test.Printf(_L("Restarting the Alarm Server\n"));
	User::LeaveIfError(iAlarmServer.Connect());
		
    RPointerArray<CCalInstance> instanceList;
	RetrieveAlarmedInstancesLC(instanceList);
	
	if (aSystemTimeOrUTC == ESystemTime)
		{
		// There are 2 missed alarms when the system time changes
		test(instanceList.Count() == 2);							// Verify that two alarms were missed
		test(instanceList[0]->Time().TimeLocalL() == KMonday);		// Verify alarm was Monday's (alarm offest is 0)
		test(instanceList[1]->Time().TimeLocalL() == KWednesday);	// Verify alarm was Wednesday's (alarm offest is 0)		
		}
	else
		{
		TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf3; 
		RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf3);
		
		// Expect that the Skipped Alarm Instances data is the same before and after the time change
		// (i.e. no missed alarms)
		test(instanceParamsBuf().iLocalStartTime == instanceParamsBuf2().iLocalStartTime);
		test(instanceParamsBuf().iLocalEndTime == instanceParamsBuf2().iLocalEndTime);	
		test(instanceParamsBuf().iTimeType == instanceParamsBuf2().iTimeType);			
		
		// Expect that the Skipped Alarm Instances data is the same as before the restart
		test(instanceParamsBuf2().iLocalStartTime == instanceParamsBuf3().iLocalStartTime);
		test(instanceParamsBuf2().iLocalEndTime == instanceParamsBuf3().iLocalEndTime);	
		test(instanceParamsBuf2().iTimeType == instanceParamsBuf3().iTimeType);	
		}
	
	// Cleanup
	SetTimeZoneL(KEuropeLondon);
	instanceList.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(&instanceList);
	iTestLibrary->PIMTestServer().SetHomeTime(now);
	
	// Restart the agenda server so that it can restore its connection to the 
	// alarm server, otherwise, all subsequent alarm server calls within the
	// agenda server will fail with KErrServerTerminated.
	iTestLibrary->CloseAgendaServer();
	iTestLibrary->ConnectAgendaServerL();
	iTestLibrary->OpenFileL(KCalendarFile());
	}

/**
@SYMTestCaseID PIM-CAL-MAL-EVC-0001
@SYMTestCaseDesc Verify that floating alarms are skipped when there is a time 
zone change which causes a positive change in UTC offset. 
@SYMPREQ PREQ2088
@SYMTestPriority 3
@SYMTestActions
1.	Set system time to 10:00 on Monday.
2.	Create a floating time mode appointment with an alarm to expire at 10:30 on Monday.
3.	Create a fixed time mode appointment with an alarm to expire at 11:00 on Monday.
4.	Set time zone to be Europe/Helsinki (UTC offset changes by +2 hours).
5.	Query Calendar for alarmed Calendar instances using Alarm Server skipped alarm instances property value.
@SYMTestExpectedResults The floating time mode alarm scheduled for expiry at 10:30 on Monday is skipped.
*/
void CAlarmTestManager::TestFloatingAlarmWithPositiveUtcOffsetL()
	{
	_LIT(KStepTitle, "@SYMTESTCaseID:PIM-CAL-MAL-EVC-0001 Testing floating alarm with a positive UTC offset");
	test.Next(KStepTitle);
	
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();	
	
	TTime now;
	now.HomeTime();
	
	test.Printf(_L("Setting subscriber notification to ON\n"));
	SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);	
	
	test.Printf(_L("Setting system time to Monday, Nov 3, 2008 10:00\n"));
	const TTime KInitialTime(TDateTime(2008, ENovember, 3, 10, 0, 0, 0));
	SetTimeZoneL(KEuropeLondon);
	iTestLibrary->PIMTestServer().SetHomeTime(KInitialTime);
		
	const TTime KFloating(TDateTime(2008, ENovember, 3, 10, 30, 0, 0));
	const TTime KFixed(TDateTime(2008, ENovember, 3, 11, 0, 0, 0));
	CreateAndAddEntriesL(ETrue, KFloating, EFalse, KFixed);
	
	SetTimeZoneL(KEuropeHelsinki);
			
	RPointerArray<CCalInstance> instanceList;
	RetrieveAlarmedInstancesLC(instanceList);
	
	test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1);
	test(instanceList.Count() == 1);							// Verify that only one alarm was missed
	test(instanceList[0]->Time().TimeLocalL() == KFloating);	// Verify the alarm was the floating (alarm offest is 0)
		
	// Cleanup
	instanceList.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(&instanceList);
	SetTimeZoneL(KEuropeLondon);
	iTestLibrary->PIMTestServer().SetHomeTime(now);
	}

/**
@SYMTestCaseID PIM-CAL-MAL-EVC-0002
@SYMTestCaseDesc Verify floating alarms are not skipped when there is a time 
zone change which causes a negative change in UTC offset.  
@SYMPREQ PREQ2088
@SYMTestPriority 3
@SYMTestActions
1.	Set system time to 09:00 on Monday.
2.	Note the value of the Alarm Server skipped alarm instances property.
3.	Set the time zone to be Europe/Helsinki.
4.	Create a floating time mode appointment with an alarm to expire at 09:30 on Monday.
5.	Create a fixed time mode appointment with an alarm to expire at 10:00 on Monday.
6.	Set time zone to be Europe/London (UTC offset changes by -2 hours).
7.	Note the value of the Alarm Server skipped alarm instances property.
@SYMTestExpectedResults There is no change in the value of the Alarm Server skipped alarm instances property.
*/
void CAlarmTestManager::TestFloatingAlarmWithNegativeUtcOffsetL()
	{
	_LIT(KStepTitle, "@SYMTESTCaseID:PIM-CAL-MAL-EVC-0002 Testing floating alarm with a negative UTC offset");
	test.Next(KStepTitle);
	
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();	
	
	TTime now;
	now.HomeTime();
	
	test.Printf(_L("Setting subscriber notification to ON\n"));
	SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);	
	
	test.Printf(_L("Setting system time to Monday, Nov 3, 2008 9:00\n"));
	const TTime KInitialTime(TDateTime(2008, ENovember, 3, 9, 0, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KInitialTime);
	
	TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf; 
	RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf);		
	
	SetTimeZoneL(KEuropeHelsinki);	
		
	const TTime KFloating(TDateTime(2008, ENovember, 3, 9, 30, 0, 0));
	const TTime KFixed(TDateTime(2008, ENovember, 3, 10, 0, 0, 0));
	CreateAndAddEntriesL(ETrue, KFloating, EFalse, KFixed);
		
	SetTimeZoneL(KEuropeLondon);
		
	TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf2; 
	RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf2);

	test(instanceParamsBuf().iLocalStartTime == instanceParamsBuf2().iLocalStartTime);
	test(instanceParamsBuf().iLocalEndTime == instanceParamsBuf2().iLocalEndTime);	
	test(instanceParamsBuf().iTimeType == instanceParamsBuf2().iTimeType);	
		
	// Cleanup
	SetTimeZoneL(KEuropeLondon);
	iTestLibrary->PIMTestServer().SetHomeTime(now);
	}

/**
@SYMTestCaseID PIM-CAL-MAL-EVC-0003
@SYMTestCaseDesc Verify that fixed and floating alarms are skipped according to
the appropriate environment changes. 
@SYMPREQ PREQ2088
@SYMTestPriority 3
@SYMTestActions
1.	Set system time to 10:00 on Monday.
2.	Create a fixed time mode appointment with an alarm to expire at 11:00 on Monday.
3.	Set the system time to 12:00.
4.	Check there is one skipped alarm.
5.	Create a floating time mode appointment with an alarm to expire at 13:00 on Monday.
6.	Set time zone to be Europe/Helsinki (UTC offset changes by +2 hours).
7.	Query Calendar for alarmed Calendar instances using Alarm Server skipped alarm instances property value.
@SYMTestExpectedResults There is one skipped alarm.
@return void
@internalComponent
*/
void CAlarmTestManager::TestTimeAndUtcChangeL()
	{
	_LIT(KStepTitle, "@SYMTESTCaseID:PIM-CAL-MAL-EVC-0003 Testing system time and UTC change");
	test.Next(KStepTitle);
	
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();	
	
	TTime now;
	now.HomeTime();
	
	test.Printf(_L("Setting subscriber notification to ON\n"));
	SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);	
	
	SetTimeZoneL(KEuropeLondon);	
	
	test.Printf(_L("Setting system time to Monday, Nov 3, 2008 10:00\n"));
	const TTime KMonday(TDateTime(2008, ENovember, 3, 10, 0, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KMonday);
		
	const TTime KFixed(TDateTime(2008, ENovember, 3, 11, 0, 0, 0));
	CreateAndAddEntryL(EFalse, KFixed);
	
	test.Printf(_L("Setting system time to Monday, Nov 3, 2008 12:00\n"));
	const TTime KMondayNoon(TDateTime(2008, ENovember, 3, 12, 0, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KMondayNoon);
	
    RPointerArray<CCalInstance> instanceList;
	RetrieveAlarmedInstancesLC(instanceList);

	test(instanceList.Count() == 1);						// Verify that only one alarm was missed
	test(instanceList[0]->Time().TimeLocalL() == KFixed);	// Verify the alarm was the fixed (alarm offest is 0)
	
	instanceList.ResetAndDestroy();
	CleanupStack::PopAndDestroy(&instanceList);	

	const TTime KFloating(TDateTime(2008, ENovember, 3, 13, 0, 0, 0));
	CreateAndAddEntryL(ETrue, KFloating);
		
	SetTimeZoneL(KEuropeHelsinki);
		
	RetrieveAlarmedInstancesLC(instanceList);
	test(instanceList.Count() == 1);							// Verify that only one alarm was missed
	test(instanceList[0]->Time().TimeLocalL() == KFloating);	// Verify the alarm was the floating (alarm offest is 0)
			
	// Cleanup
	instanceList.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(&instanceList);
	SetTimeZoneL(KEuropeLondon);
	iTestLibrary->PIMTestServer().SetHomeTime(now);	
	}

/**
@SYMTestCaseID PIM-CAL-MAL-EVC-0004
@SYMTestCaseDesc Verify that fixed and floating alarms are skipped according to
the appropriate environment changes.
@SYMPREQ PREQ2088
@SYMTestPriority 3
@SYMTestActions
1.	Set system time to 17:59 on Monday.
2.	Create a fixed time mode appointment with an alarm to expire at 18:00 on Monday.
3.	Create a floating time mode appointment with an alarm to expire at 18:01 on Monday.
4.	Set the time zone to be Europe/Frankfurt (UTC offset changes by +1 hour).
5.	Query Calendar for alarmed Calendar instances using Alarm Server skipped alarm instances property value.
@SYMTestExpectedResults There is one skipped alarm.
*/
void CAlarmTestManager::TestFixedAndFloatingWithUtcChangeL()
	{
	_LIT(KStepTitle, "@SYMTESTCaseID:PIM-CAL-MAL-EVC-0004 Testing fixed and floating alarms with UTC change");
	test.Next(KStepTitle);
	
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();	
	
	TTime now;
	now.HomeTime();
	
	test.Printf(_L("Setting subscriber notification to ON\n"));
	SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);	
	
	SetTimeZoneL(KEuropeLondon);
	
	test.Printf(_L("Setting system time to Monday, Nov 3, 2008 17:59\n"));
	const TTime KMonday(TDateTime(2008, ENovember, 3, 17, 59, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KMonday);
		
	const TTime KFixed(TDateTime(2008, ENovember, 3, 18, 0, 0, 0));
	const TTime KFloating(TDateTime(2008, ENovember, 3, 18, 1, 0, 0));

	CreateAndAddEntriesL(EFalse, KFixed, ETrue, KFloating);
	
	SetTimeZoneL(KEuropeBerlin);
	
    RPointerArray<CCalInstance> instanceList;
	RetrieveAlarmedInstancesLC(instanceList);

	test(instanceList.Count() == 1);							// Verify that only one alarm was missed
	test(instanceList[0]->Time().TimeLocalL() == KFloating);	// Verify the alarm was the floating (alarm offest is 0)
				
	// Cleanup
	instanceList.ResetAndDestroy();	
	CleanupStack::PopAndDestroy(&instanceList);
	SetTimeZoneL(KEuropeLondon);
	iTestLibrary->PIMTestServer().SetHomeTime(now);	
	}

/**
@SYMTestCaseID PIM-CAL-MAL-EVC-0005
@SYMTestCaseDesc Verify that if there are no skipped alarms in skipped time 
interval that Alarm Server does not publish a new property value and Calendar 
returns no alarmed Calendar instances in the skipped time interval.
@SYMPREQ PREQ2088
@SYMTestPriority 3
@SYMTestActions
1.	Set system time to 12:00 on Sunday.
2.	Create an appointment with an alarm to expire at 10:00 on Monday.
3.	Note the value of the Alarm Server skipped alarm instances property.
4.	Set the system time forward to 09:59 on Monday.
5.	Note the value of the Alarm Server skipped alarm instances property.
@SYMTestExpectedResults There is no change in the value of the Alarm Server 
skipped alarm instances property.
*/
void CAlarmTestManager::TestTimeChangeWithNoSkippedAlarmL()
	{
	_LIT(KStepTitle, "@SYMTESTCaseID:PIM-CAL-MAL-EVC-0005 Testing time change with no skipped alarms");
	test.Next(KStepTitle);
	
	ClearAllAlarmsL();
	iTestLibrary->CleanDatabaseL();	
	
	TTime now;
	now.HomeTime();
	
	test.Printf(_L("Setting subscriber notification to ON\n"));
	SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);	
		
	test.Printf(_L("Setting system time to Sunday, Nov 2, 2008 12:00\n"));
	const TTime KSunday(TDateTime(2008, ENovember, 2, 12, 0, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KSunday);
	
	const TTime KAlarm(TDateTime(2008, ENovember, 3, 10, 0, 0, 0));
	CreateAndAddEntryL(EFalse, KAlarm);
			
	TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf; 
	RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf);
		
	const TTime KNewTime(TDateTime(2008, ENovember, 2, 9, 59, 0, 0));
	iTestLibrary->PIMTestServer().SetHomeTime(KNewTime);	
	
	TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf2; 
	RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf2);

	test(instanceParamsBuf().iLocalStartTime == instanceParamsBuf2().iLocalStartTime);
	test(instanceParamsBuf().iLocalEndTime == instanceParamsBuf2().iLocalEndTime);	
	test(instanceParamsBuf().iTimeType == instanceParamsBuf2().iTimeType);	
	
	// Cleanup
	iTestLibrary->PIMTestServer().SetHomeTime(now);		
	}
#endif

void CAlarmTestManager::RunL()
	{
	test.Printf(_L("CAlarmTestManager::RunL - step %d\n"), iTestStep);
	TBool completed = EFalse;
	TTime now;
	switch (iTestStep++)
		{
		case 0:
  			SetTimeZoneL(KEuropeBerlin);
			break;
		case 1:
			TestAddAlarmL();
			TestAddAlarmsSummerWinterL();
			TestMultipleAlarmsL();
			break;
		case 2:
			SetTimeZoneL(KAsiaTokyo);
			break;
		case 3:
			TestDeleteAlarmsSummerWinterL();
			TestUpdateAlarmL();
			TestDEF079490L();
			break;
		case 4:
			// repeat the test cases for floating entry
			//! @SYMTestCaseID PIM-CALENDAR-ALARM-0001
			//! @SYMPREQ PREQ1357
			//! @SYMTestCaseDesc Floating entries with alarm
			//! @SYMFssID App-Engines/CalInterimAPI/Data.015
			//! @SYMFssID App-Engines/CalInterimAPI/Data.018
			//! @SYMTestStatus Implemented
			//! @SYMTestPriority Medium
			//! @SYMTestActions 1) Create floating entries with alarm, check alarms. 
			//! 				2) Delete entries, check alarm.
			//! 				3) Create and update entries, check alarms. 
			//! @SYMTestExpectedResults Check that alarms expire as expected. For case 2, alarms are removed for the deleted entries For case 3, the updated alarms are found.
			//! @SYMTestType CIT
			test.Printf(_L("********** Start floating time entry test **********\n"));	
			SetTimeZoneL(KEuropeBerlin);	
			break;
		case 5:
			TestAddAlarmL(ETrue);
			TestAddAlarmsSummerWinterL(ETrue);
			TestMultipleAlarmsL(ETrue);
			break;
		case 6:
			SetTimeZoneL(KAsiaTokyo);
			break;
		case 7:
			TestDeleteAlarmsSummerWinterL(ETrue);	
			TestUpdateAlarmL(ETrue);
			TestDEF079490L(ETrue);
			break;
		case 8:
			SetTimeZoneL(KEuropeBerlin);
			break;
		case 9:
			CheckNonStopTodoAlarmL();
			break;
		case 10:
			// Test floating entry alarm times: PREQ 1090 - Test Enhancements: Case 6
			// Setup Tz
			SetTimeZoneL(KEuropeLondon);
			break;
		case 11:
			TestFloatingAndFixedEntryAlarmTimeSetupL(ETrue);
			break;
		case 12:
			SetTimeZoneL(KEuropeParis);
			break;
		case 13:
			TestFloatingAndFixedEntryAlarmTimeCheckL(ETrue);
			break;
		case 14:
			SetTimeZoneL(KEuropeParis); // need to set again
			break;
		case 15:
			TestFloatingAndFixedEntryAlarmTimeReopenAndCheckL(ETrue);
			break;
		case 16:
			SetTimeZoneL(KEuropeLondon);
			break;
		case 17:
			TestFloatingAndFixedEntryAlarmTimeCheckAgainL(ETrue);
			break;
		case 18:
			// Test fixed entry alarm times: PREQ 1090 - Test Enhancements: Case 6
			TestFloatingAndFixedEntryAlarmTimeSetupL(EFalse);
			break;
		case 19:
			SetTimeZoneL(KEuropeParis);
			break;
		case 20:
			TestFloatingAndFixedEntryAlarmTimeCheckL(EFalse);
			break;
		case 21:
			SetTimeZoneL(KEuropeParis); // need to set again
			break;
		case 22:
			TestFloatingAndFixedEntryAlarmTimeReopenAndCheckL(EFalse);
			break;
		case 23:
			SetTimeZoneL(KEuropeLondon);
			break;
		case 24:
			TestFloatingAndFixedEntryAlarmTimeCheckAgainL(EFalse);
			break;
		case 25:
			SetTimeZoneL(KAtlanticStanley);
			break;
		case 26:
			TestAlarmEntryL();
			break;
		case 27:
			SetTimeZoneL(KEuropeLondon);
			break;
		case 28:
			SnoozeAlarmAndUpdateEntryL();
			break;
		case 29:
			TestAlarmOffsetL();
			iTestLibrary->PIMTestServer().SetTimeZoneL(_L8("Europe/London"));
			break;
       case 30:
       		/*
       		case 30 - 35 are tests for the defect fixing INC112196
       		Those cases are runing twice for two different behaviours according to the date set in Central Repository
       		First time (iQueueAlarm == ESkippedAlarmsQueueAndExpire) , it tests the default behaviour. That is:
       		- Alarms missed due to system time change will be notified
       		Second time (iQueueAlarm == ESkippedAlarmsNotifySubscriber), it tests the behaviour required by s60. That is
       		- Alarms missed due to system time change will not be notified.
       		- The Pub & Sub value will be set to 2, if some alarms have been missed due to system time\time zone changes.
       		
       		There is a common feature for both behaviours:
       		- The Pub & Sub value will be set to 1, if the time zone has been changed but there are no missing alarms
       		*/
       	
       		TestFloatingAlarmNotificationL();
       	break;
       	
		case 31: 		
			// case 32 - 33 are tests in the situation that agenda server is runing when the system time changes 
				//Floating alarm, system time will be put forward to the alarm due time
			TestChangeSystemTimeL(ETrue, ETrue);
				//Floating alarm, system time will be put the same as the alarm due time	
			TestChangeSystemTimeL(ETrue, EFalse);
				//UTC alarm, system time will put forward to the alarm due time
			TestChangeSystemTimeL(EFalse, ETrue);
				//UTC alarm, system time will be put the same as the alarm due time
			TestChangeSystemTimeL(EFalse, EFalse);
			
			TestNotifiedAlarmL();
			
			break;
		case 32:
			//Add a fixed entry with an alarm. When time zone changes, the alarm will not be missed but alarmserver 
			// will publish the data indicating that the time zone has been changed.
			ClearAllAlarmsL();
			iTestLibrary->CleanDatabaseL();
			now.HomeTime();
			AddEntryWithAlarmL(now + TTimeIntervalHours(1) ,0); 
			iTimeSysTimeChanged.UniversalTime();
			iTestLibrary->PIMTestServer().SetTimeZoneL(_L8("Europe/Berlin"));
			break;
		case 33:
			User::After(1000000);
			//For UTC alarms, they should not be notified when TZ changes
			test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 1);
			TestPubSubValue(ETzChanged);
			break;
		case 34:
			//case 33 - 34 are tests in the situation that agenda server is not running when the system time changes 
				// Add some floating and UTC alarms
			AddFourEntriesWithAlarmsL();
			//disconnect to the agenda server
			iTestLibrary->CloseAgendaServer();
			break;
		
		case 35:
			test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 4);
			now.HomeTime();
			//Make all four alarms are in the past
			iTestLibrary->PIMTestServer().SetHomeTime(now + TTimeIntervalHours(3)); 
			iTimeSysTimeChanged.UniversalTime();
			User::After(1000000);
			
			if(iQueueAlarm == ESkippedAlarmsQueueAndExpire)
				{
				test(iAlarmServer.AlarmCountByState(EAlarmStateQueued) == 0);//Alarms in the past have been notified so no alarm should be queued.
				}
			else
				{//All four alarms are dequeued
				RArray<TAlarmId> alarmIds;
   				CleanupClosePushL(alarmIds);

 				iAlarmServer.GetAlarmIdListL(alarmIds);
 	
   				test(alarmIds.Count()==0);
   				CleanupStack::PopAndDestroy(&alarmIds);

				TestPubSubValue(EAlarmMissed);
				}
				
			//Reset the system time
			iTestLibrary->PIMTestServer().SetHomeTime(now - TTimeIntervalHours(3));
			if(iQueueAlarm == ESkippedAlarmsQueueAndExpire)
				{
				//Reset environment and prepare for the new behaviour test 

				iTestStep=30;
				SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);
				iTestLibrary->ConnectAgendaServerL();
				iTestLibrary->ReplaceFileL(KCalendarFile());
				iTestLibrary->OpenFileL(KCalendarFile());
				iTestLibrary->SynCGetEntryViewL(); 
				
				iTestLibrary->PIMTestServer().SetTimeZoneL(_L8("Europe/London"));
				}
			break;
			
		case 36:
			iTestLibrary->ConnectAgendaServerL();
			iTestLibrary->ReplaceFileL(KCalendarFile());
			iTestLibrary->OpenFileL(KCalendarFile());
			
			CheckAlarmAttachmentSizeL();
			break;
		case 37:
	        TestAlarmNotDeletedInDiffFileL();
	        TestAlarmDeletedInSameFileL();
	        TestAlarmDeletedWhenFileDeletedL();
	        break;

#ifdef SYMBIAN_SKIPPED_CALENDAR_ALARMS
		case 38:
			NotifySubscriberWithCalendarRunningL();
			break;
			
		case 39:
			NotifySubscriberWithoutCalendarRunningL();
			break;
			
		case 40:
			RetrieveSkippedAlarmsAfterRestartL(ESystemTime);
			break;
			
		case 41:
			RetrieveSkippedAlarmsAfterRestartL(EUTC);
			break;
			
		case 42:
			TestFloatingAlarmWithPositiveUtcOffsetL();
			break;
			
		case 43:
			TestFloatingAlarmWithNegativeUtcOffsetL();
			break;
			
		case 44:
			TestTimeAndUtcChangeL();
			break;
			
		case 45:
			TestFixedAndFloatingWithUtcChangeL();
			break;
			
		case 46:
			TestTimeChangeWithNoSkippedAlarmL();
			break;
	     case 47:
	            SetTimeZoneL(KAtlanticStanley);
	            break;
        case 48:
            TestRetrieveSkippedAlarmsAfterSystemTimeChangeL();
            break;

#endif			
			
		default:
			completed = ETrue;
       		break;
		}

	if (completed)
		{
		CActiveScheduler::Stop();
		}
	else
		{
		Start();
		}
	}

void CAlarmTestManager::TestRetrieveSkippedAlarmsAfterSystemTimeChangeL()
    {
    iTestLibrary->CloseAgendaServer();
    SetCenRepValuseL(ESkippedAlarmsNotifySubscriber);
    iTestLibrary->OpenFileL(KCalendarFile());

    _LIT(KStepTitle, "@SYMTESTCaseID:PIM-ALS-MAL-NOT-0005 Testing skipped alarms can be retrieved after a restart");
    test.Next(KStepTitle);
    
    ClearAllAlarmsL();
    iTestLibrary->CleanDatabaseL();
        
    TTime now;
    now.HomeTime();
    
    test.Printf(_L("Setting subscriber notification to ON\n")); 
    test.Printf(_L("Setting system time to Sunday, Nov 2, 2008 12:00\n"));
    const TTime KCurrentTime(TDateTime(2009, EJuly, 28, 5, 30, 0, 0));
    iTestLibrary->PIMTestServer().SetHomeTime(KCurrentTime);
    
    const TTime KAlarmTime(TDateTime(2009, EAugust, 1, 5, 0, 0, 0));
    //create an entry its start time, end time and alarm time all in 2009 2 August 5:00
    CCalEntry* entry = CreateAlarmedEntryLC(CCalEntry::EAppt, KAlarmTime, KAlarmTime, 0, KGUID1().AllocLC(), EFalse);       
    test.Printf(_L("Adding alarmed entries\n"));    
    RPointerArray<CCalEntry> entriesToStore;
    CleanupClosePushL(entriesToStore);
    entriesToStore.AppendL(entry);
    TInt entriesStored = 0; 
    iTestLibrary->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
    test(entriesToStore.Count() == entriesStored);

    //make sure alarm is added
    WaitForAlarmAdditionL();
    
    CleanupStack::PopAndDestroy(&entriesToStore);   
    CleanupStack::PopAndDestroy(entry);
   
   // Change time to Thursday, Oct 9, 2008 12:00
    const TTime KNewSystemTime(TDateTime(2009, EAugust, 1, 5, 30, 0, 0));
    iTestLibrary->PIMTestServer().SetHomeTime(KNewSystemTime);

    CCalSession& session = iTestLibrary->GetSession();
    RPointerArray<CCalInstance> instanceList;
    CleanupClosePushL(instanceList);
  
    CCalInstanceView* instanceView = CCalInstanceView::NewL(session);
    CleanupStack::PushL(instanceView);   
    TPckgBuf<TASShdAlarmedInstanceParams> instanceParamsBuf; 
    RProperty::Get(KAlarmServerPubSubCategory, KSkippedAlarmInstancesPubSubKey, instanceParamsBuf);
    instanceView->FindAlarmedInstanceL(instanceList, instanceParamsBuf());
    CleanupStack::PopAndDestroy(instanceView);    
  
    test(instanceList.Count() == 1);                           
    test(instanceList[0]->Time().TimeLocalL() == KAlarmTime);     
 
    // Cleanup
    SetTimeZoneL(KEuropeLondon);
    instanceList.ResetAndDestroy(); 
    CleanupStack::PopAndDestroy(&instanceList);
    iTestLibrary->PIMTestServer().SetHomeTime(now);
    
    iTestLibrary->CloseAgendaServer();
    SetCenRepValuseL(ESkippedAlarmsQueueAndExpire);//default
    iTestLibrary->OpenFileL(KCalendarFile());
    }

void CAlarmTestManager::TestPubSubValue(TPubSubValue aValue)
	{
	test.Printf(_L("Fetch and test the pub sub value"));
	TPckgBuf<TMissedAlarmPubSubData> pubSubBuf;
	TBool propertyPublished= ETrue;
	RTimer timeoutTimer;
	CleanupClosePushL(timeoutTimer);
	User::LeaveIfError(timeoutTimer.CreateLocal());
	TRequestStatus timeoutStatus;
	timeoutTimer.After(timeoutStatus, 1000000);
	RProperty property;
	CleanupClosePushL(property);
	property.Attach(KAlarmServerPubSubCategory, KMissingAlarmPubSubKey);
	TRequestStatus propetryStatus;
	property.Subscribe(propetryStatus);
	User::WaitForRequest(propetryStatus, timeoutStatus);
	
	if (timeoutStatus.Int() == KRequestPending)
		{
		test.Printf(_L("The property has been published so cancel the timeout timer"));
		timeoutTimer.Cancel();
		User::WaitForRequest(timeoutStatus);
		}
	else
		{
		propertyPublished = EFalse;
		test.Printf(_L("The property must have already been published when we subscribed to it"));
		property.Cancel();
		User::WaitForRequest(propetryStatus);
		}
	
	property.Get(pubSubBuf);
	CleanupStack::PopAndDestroy(&property);
	CleanupStack::PopAndDestroy(&timeoutTimer);
	TUint8 getValue = pubSubBuf().iValue;
	TDateTime getTime = pubSubBuf().iTimeOfChangeUtc.DateTime();
	getTime.SetMicroSecond(0);
	getTime.SetSecond(0);
	TDateTime expTime = iTimeSysTimeChanged.DateTime();
	expTime.SetMicroSecond(0);
	expTime.SetSecond(0);
	if(aValue == ENoChange)
		{
		test (!propertyPublished); 
		}
	else
		{
		test(getValue == aValue);
		test(TTime(getTime) == TTime(expTime));
		}
	}

void CAlarmTestManager::SetCenRepValuseL(TCenRepValue aCenRepValuse)
	{//Set the value in Central repositoty to get the right behaviour to test
	TInt value;
	CRepository* repository = CRepository::NewLC(KUidCentralRepositoryAgenda);
	User::LeaveIfError(repository->Set(KKeyQueueMissingAlarm, aCenRepValuse));
	User::LeaveIfError(repository->Get(KKeyQueueMissingAlarm, value));
	CleanupStack::PopAndDestroy(repository);
	test(value == aCenRepValuse);
	iQueueAlarm = aCenRepValuse;
	}
	

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CAlarmTestManager* testManager = CAlarmTestManager::NewLC();
	testManager->SetCenRepValuseL(ESkippedAlarmsQueueAndExpire);//default behaviour

	TPerformanceTimer timer(test);
	timer.Start();	
	
	// Run the test suite
	testManager->Start();
	CActiveScheduler::Start();	

	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();
		
	CleanupStack::PopAndDestroy(testManager);
	}


/**

@SYMTestCaseID     PIM-TCAL-ALARM-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-ALARM-0001 Calendar Interim API Alarm test suite"));	
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
