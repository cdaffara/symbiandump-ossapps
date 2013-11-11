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
#include <calalarm.h>
#include <asclisession.h>
#include <asshdalarm.h>
#include <tz.h>
#include <coreappstest/utilities.h>
#include "T_CrAlarm.h"

_LIT8(KUIDFmt, "UID-%d");
_LIT(KSumFmt, "summary - %d");
_LIT(KDescFmt, "description - %d");
_LIT(KLocFmt, "location - %d");
_LIT(KUTC, "UTC");
_LIT(KNewUTC, "NewUTC");
_LIT(KCMySound, "c:\\mysound");
_LIT(KCMySound2, "c:\\mysound2");
_LIT(KWait, "Wait");
_LIT(KAlarms, "Alarms");
_LIT(KType, "Type");
_LIT(KAlarmTime, "AlarmTime");
_LIT(KFixed, "fixed");
_LIT(KAlarmIdFmt, "AlarmId: %i");
_LIT(KTimeFmt, "%F%Y%M%D-%H:%T:%S");
_LIT(KStartLocalFmt, "Start (local): %s");
_LIT(KStartUtcFmt, "Start (utc): %s");
_LIT(KEndLocalFmt, "End (local): %s");
_LIT(KEndUtcFmt, "End (utc): %s");
_LIT(KAlarmStartFmt, "AlarmName: %s");
_LIT(KAlarmDetailsFmt, "AlarmId: %i Message: %s Due: %s");
_LIT(KAfterWait, "AfterWait");
_LIT(KAfterUtc, "AfterUtc");
_LIT(KTimeLocal, "Local time now: %s");
_LIT(KTimeUtc, "Utc time now: %s");

//Keeping timeout period to 2 minutes
//For current tests, all notifications should arrive within this time
const TInt KTimeOutPeriod = 120000000;

//Alarm notify checker timeout counter
const TInt KRetryMax = 600;
const TTimeIntervalMicroSeconds32 KRetryPeriod = 100000;

CTestCalInterimApiAlarmBasic::CTestCalInterimApiAlarmBasic()
	{
	SetTestStepName(KTestCalInterimApiAlarmBasic);
	}

CTestCalInterimApiAlarmBasic::~CTestCalInterimApiAlarmBasic()
	{
	}

void CTestCalInterimApiAlarmBasic::CreateTwoAlarmedEntriesL(RPointerArray<CCalEntry>& aEntries)
	{
	TBuf8<MAX_PROPERTY_STRLEN> uidStr;	
	uidStr.Format(KUIDFmt, 1);
	
	HBufC8* uId = HBufC8::NewLC(MAX_UID_BUF_SIZE);
	*uId = uidStr;
	
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, uId, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uId); // CCalEntry takes ownership
	CleanupStack::PushL(entry);
		
	TBuf<MAX_LONG_STRLEN> str;
	str.Format(KSumFmt, 1);
	entry->SetSummaryL(str);
	
	str.Format(KDescFmt, 1);
	
	entry->SetDescriptionL(str);
	
	str.Format(KLocFmt, 1);
	entry->SetLocationL(str);

	TCalTime start;
	TCalTime end;
	
	TTime now;
	now.HomeTime();
	
	TTime myStart1 = now;
	TTime myEnd1 = myStart1 + TTimeIntervalHours(1);
	
	start.SetTimeLocalFloatingL(myStart1);
	end.SetTimeLocalFloatingL(myEnd1);
	entry->SetStartAndEndTimeL(start, end);
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	
	alarm->SetAlarmSoundNameL(KCMySound);
	alarm->SetTimeOffset(1);
	entry->SetAlarmL(alarm);

	CleanupStack::PopAndDestroy(alarm);

	aEntries.AppendL(entry);
	CleanupStack::Pop(entry);
	
	uidStr.Format(KUIDFmt, 2);
	
	HBufC8* uId2 = HBufC8::NewLC(MAX_UID_BUF_SIZE);
	*uId2 = uidStr;
	
	CCalEntry* entry2 = CCalEntry::NewL(CCalEntry::EAppt, uId2, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(uId2); // CCalEntry takes ownership
	CleanupStack::PushL(entry2); 
			
	str.Format(KSumFmt, 2);
	entry2->SetSummaryL(str);
	
	str.Format(KDescFmt, 2);
	entry2->SetDescriptionL(str);
	
	str.Format(KLocFmt, 2);
	entry2->SetLocationL(str);

	TCalTime start2;
	TCalTime end2;
	
	// The 2 hours represents the difference between the two UTC zones in the
	// data file.
	TTime myStart2 = now + TTimeIntervalHours(2);
	TTime myEnd2 = myStart2 + TTimeIntervalHours(1);
	
	start2.SetTimeLocalFloatingL(myStart2);
	end2.SetTimeLocalFloatingL(myEnd2);
	entry2->SetStartAndEndTimeL(start2, end2);
	
	CCalAlarm* alarm2 = CCalAlarm::NewL();
	CleanupStack::PushL(alarm2); 
	
	alarm2->SetAlarmSoundNameL(KCMySound2);
	alarm2->SetTimeOffset(1);
	entry2->SetAlarmL(alarm2);
	
	CleanupStack::PopAndDestroy(alarm2);
	
	aEntries.AppendL(entry2);
	CleanupStack::Pop(entry2);
	}

TVerdict CTestCalInterimApiAlarmBasic::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
		
	TPtrC initialTZ;
	TESTL(GetStringFromConfig(ConfigSection(), KUTC, initialTZ)); 
	TPtrC newTZ;
	TESTL(GetStringFromConfig(ConfigSection(), KNewUTC, newTZ));

	RTz tz;
	CleanupClosePushL(tz);
	TESTL(tz.Connect() == KErrNone);
	
	TDateTime initDate(2005, EOctober, 14, 12, 0, 0, 0);
	TTime initTime (initDate);
	tz.SetHomeTime(initTime);
	CleanupStack::PopAndDestroy();
	
	SetTZL(initialTZ);

	TBuf<32> buf;
	TTime now;
	now.HomeTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeLocal, buf.PtrZ());
	
	now.UniversalTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeUtc, buf.PtrZ());
		
	// Check for expired alarms before we start
	RASCliSession alarmSess;
	CleanupClosePushL(alarmSess);
	User::LeaveIfError(alarmSess.Connect());
	
	TESTL(alarmSess.AlarmDeleteAllByCategory(KUidAgendaModelAlarmCategory, EFalse) == KErrNone);
	TESTL(alarmSess.AlarmDeleteAllByCategory(TUid::Null(), EFalse) == KErrNone);

	RArray<TAlarmId> idList;
	CleanupClosePushL(idList);
	alarmSess.GetAlarmIdListByStateL(EAlarmStateQueued, idList);
	TESTL(idList.Count() == 0);

	RPointerArray<CCalEntry> entries;
	CleanupRPtrArrayDeleteAndCloseL(entries);
	
	CreateTwoAlarmedEntriesL(entries);

	CCalProgress* progress = new(ELeave)CCalProgress;
	CleanupStack::PushL(progress);
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view);
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();
			
	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());

	CAlarmEventWatcher* alarmwatcher = CAlarmEventWatcher::NewL(alarmSess);
	CleanupStack::PushL(alarmwatcher);
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventHeadQueueItemChanged, KTimeOutPeriod);
	

	RArray<TAlarmId> idList2;
	CleanupClosePushL(idList2);
	alarmSess.GetAlarmIdListByStateL(EAlarmStateQueued, idList2);
	TESTL(idList2.Count() != 0);
	TAlarmId idFirst = idList2[0];
	
	
	// change UTC offset
	SetTZL(newTZ);
	//Existing alarm should expire and expected alarm should be the head alarm.
	//Then wait for the expected alarm to expire
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventHeadQueueItemChanged, KTimeOutPeriod);
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventTimerExpired, KTimeOutPeriod);

	now.HomeTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeLocal, buf.PtrZ());
	
	now.UniversalTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeUtc, buf.PtrZ());
		
	RArray<TAlarmId> idList3;
	CleanupClosePushL(idList3);
	
	INFO_PRINTF1(_L("Check for Notifying Alarms"));
	
	//Check the alarm list untill we timeout, or we get the results we want(1 minute)
	for(TInt notifyRetryCount = 0; idList3.Count() == 0 && notifyRetryCount < KRetryMax; notifyRetryCount ++)
		{
		alarmSess.GetAlarmIdListByStateL(EAlarmStateNotifying, idList3);
		if(idList3.Count() == 0)
			{
			INFO_PRINTF1(_L("Alert-Server has not responded back to Alarm-Server, so expected alarm is now waiting to notify"));
			alarmSess.GetAlarmIdListByStateL(EAlarmStateWaitingToNotify, idList3);
			}
		//wait 100ms so we don't thrash this loop
		User::After(KRetryPeriod);
		}
		
	TESTL(idList3.Count() != 0);
	TAlarmId idSecond = idList3[0];
	TESTL(idFirst != idSecond);
	
	TASShdAlarm shalarm;
	TESTL(alarmSess.GetAlarmDetails(idList3[0], shalarm) == KErrNone);
	
	TBuf<50> secSum;
	secSum.Format(KSumFmt, 2);
	TESTL(shalarm.Message() == secSum);
			
	TESTL(alarmSess.AlarmDeleteAllByCategory(KUidAgendaModelAlarmCategory, EFalse) == KErrNone);

	CleanupStack::PopAndDestroy(8);
	
	RestoreTZL();
	
	return TestStepResult();
	}
//------------------------------------------------------------

CTestCalInterimApiAlarmMany::CTestCalInterimApiAlarmMany()
	{
	SetTestStepName(KTestCalInterimApiAlarmMany);
	}

CTestCalInterimApiAlarmMany::~CTestCalInterimApiAlarmMany()
	{
	}

TVerdict CTestCalInterimApiAlarmMany::doTestStepL()	
	{
	TESTL(TestStepResult() == EPass);
	
	TPtrC initialTZ;
	TESTL(GetStringFromConfig(ConfigSection(), KUTC, initialTZ)); 
	TPtrC newTZ;
	TESTL(GetStringFromConfig(ConfigSection(), KNewUTC, newTZ));
	TInt waitPeriod;
	TESTL(GetIntFromConfig(ConfigSection(), KWait, waitPeriod));
	TPtrC afterWaitAlarm;
	TESTL(GetStringFromConfig(ConfigSection(), KAfterWait, afterWaitAlarm));
	TPtrC afterUtcAlarm;
	TESTL(GetStringFromConfig(ConfigSection(), KAfterUtc, afterUtcAlarm));
	TPtrC alarmsList;
	TESTL(GetStringFromConfig(ConfigSection(), KAlarms, alarmsList));
	
	SetTZL(initialTZ);
	
	RArray<TTestAlarm> alarms;
	CleanupClosePushL(alarms); 
	GetAlarmsFromConfigL(alarmsList, alarms);

	RPointerArray<CCalEntry> entries;
	CleanupRPtrArrayDeleteAndCloseL(entries); 
	
	RPointerArray<CCalAlarm> calAlarms;
	CleanupRPtrArrayDeleteAndCloseL(calAlarms); 
	
	CreateAlarmedEntriesL(entries, alarms, calAlarms);
	
	RASCliSession alarmSess;
	CleanupClosePushL(alarmSess); 
	TESTL(alarmSess.Connect() == KErrNone);
	
	TESTL(alarmSess.AlarmDeleteAllByCategory(KUidAgendaModelAlarmCategory, EFalse) == KErrNone);
	TESTL(alarmSess.AlarmDeleteAllByCategory(TUid::Null(), EFalse) == KErrNone);
	
	RArray<TAlarmId> idList;
	CleanupClosePushL(idList); 
	
	alarmSess.GetAlarmIdListByStateL(EAlarmStateQueued, idList);

	for(TInt i = 0; i < idList.Count(); ++i)
		{
		INFO_PRINTF2(KAlarmIdFmt, idList[i]);
		}
	
	TESTL(idList.Count() == 0);
		
	CCalProgress* progress = new (ELeave) CCalProgress;
	CleanupStack::PushL(progress); // 6
	
	CCalEntryView* view = CCalEntryView::NewL(*iSession, *progress);
	CleanupStack::PushL(view); // 7
	
	CActiveScheduler::Add(progress);
	CActiveScheduler::Start();

	TInt cnt;
	view->StoreL(entries, cnt);
	TESTL(cnt == entries.Count());
	
	//Once the entries have been stored, Calendar should queue the next alarm and this
	//will cause the following event to be generated by alarmserver.
	CAlarmEventWatcher* alarmwatcher = CAlarmEventWatcher::NewL(alarmSess);
	CleanupStack::PushL(alarmwatcher);
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventHeadQueueItemChanged, KTimeOutPeriod);

	alarmSess.GetAlarmIdListL(idList);
	TASShdAlarm shalarm;
	TBuf<32> buf;

	TTime now;

	now.HomeTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeLocal, buf.PtrZ());

	now.UniversalTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeUtc, buf.PtrZ());
	
	for(TInt i = 0; i < idList.Count(); ++i)
		{
		TESTL(alarmSess.GetAlarmDetails(idList[i], shalarm) == KErrNone);
		shalarm.NextDueTime().FormatL(buf, KTimeFmt);
		INFO_PRINTF4(KAlarmDetailsFmt, shalarm.Id(), shalarm.Message().PtrZ(), buf.PtrZ());
		}
		
	TESTL(idList.Count() > 0);
	
	//The queued alarm after this event should be the expected alarm
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventHeadQueueItemChanged, KTimeOutPeriod);
	alarmSess.GetAlarmIdListByStateL(EAlarmStateQueued, idList);
	
	now.HomeTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeLocal, buf.PtrZ());

	now.UniversalTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeUtc, buf.PtrZ());

	for(TInt i = 0; i < idList.Count(); ++i)
		{
		TESTL(alarmSess.GetAlarmDetails(idList[i], shalarm) == KErrNone);
		shalarm.NextDueTime().FormatL(buf, KTimeFmt);
		INFO_PRINTF4(KAlarmDetailsFmt, shalarm.Id(), shalarm.Message().PtrZ(), buf.PtrZ());
		}

	TESTL(idList.Count() > 0);
	TESTL(alarmSess.GetAlarmDetails(idList[0], shalarm) == KErrNone);
	TESTL(shalarm.Message() == afterWaitAlarm);

	//change timezone and wait for an alarm to go off
	//check the notifying alarm message is same as expected.
	SetTZL(newTZ);
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventHeadQueueItemChanged, KTimeOutPeriod);
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventTimerExpired, KTimeOutPeriod);
	
	alarmSess.GetAlarmIdListByStateL(EAlarmStateNotifying, idList);
	
	now.HomeTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeLocal, buf.PtrZ());

	now.UniversalTime();
	now.FormatL(buf, KTimeFmt);
	INFO_PRINTF2(KTimeUtc, buf.PtrZ());
	
	for(TInt i = 0; i < idList.Count(); ++i)
		{
		TESTL(alarmSess.GetAlarmDetails(idList[i], shalarm) == KErrNone);
		shalarm.NextDueTime().FormatL(buf, KTimeFmt);
		INFO_PRINTF4(KAlarmDetailsFmt, shalarm.Id(), shalarm.Message().PtrZ(), buf.PtrZ());
		}
	
	TESTL(idList.Count() > 0);
	TESTL(alarmSess.GetAlarmDetails(idList[0], shalarm) == KErrNone);
	TESTL(shalarm.Message().Match(afterUtcAlarm) != KErrNotFound);
	
	TESTL(alarmSess.AlarmDeleteAllByCategory(KUidAgendaModelAlarmCategory, EFalse) == KErrNone);

	CleanupStack::PopAndDestroy(8);
	RestoreTZL();
	
	return TestStepResult();
	}	

void CTestCalInterimApiAlarmMany::GetAlarmsFromConfigL(const TDesC& aList, RArray<TTestAlarm>& aAlarms)
	{
	TLex lexer(aList);
	
	while(!lexer.Eos())
		{
		lexer.SkipSpaceAndMark();
		
		while(!lexer.Eos() && lexer.Get() != TChar(','))
			{
			}
		
		if(!lexer.Eos()) 
			{
			lexer.UnGet(); // Do not include trailing ','
			}
		
		TTestAlarm alarm;
		alarm.iName = lexer.MarkedToken();
		GetAlarmFromConfigL(lexer.MarkedToken(), alarm);
		aAlarms.AppendL(alarm);
		lexer.Inc();
		}		
	}

void CTestCalInterimApiAlarmMany::GetAlarmFromConfigL(const TDesC& aAlarmName, TTestAlarm& aAlarm)
	{
	TPtrC type;
	TESTL(GetStringFromConfig(aAlarmName, KType, type));
	TESTL(GetIntFromConfig(aAlarmName, KAlarmTime, aAlarm.iAlarmTime));
	aAlarm.iFixed = (type == KFixed);
	aAlarm.iName = aAlarmName;
	}

void CTestCalInterimApiAlarmMany::CreateAlarmedEntriesL(RPointerArray<CCalEntry>& aEntries, RArray<TTestAlarm> aAlarms, RPointerArray<CCalAlarm>& aCalAlarms)
	{
	for(TInt i = 0; i < aAlarms.Count(); ++i)
		{
		TBuf8<MAX_PROPERTY_STRLEN> uidStr;	
		uidStr.Format(KUIDFmt, i+1);
		
		HBufC8* uId = HBufC8::NewLC(MAX_UID_BUF_SIZE);
		*uId = uidStr;
		
		CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, uId, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop(uId); // CCalEntry takes ownership
		CleanupStack::PushL(entry);
			
		entry->SetSummaryL(aAlarms[i].iName);
		
		TBuf<MAX_LONG_STRLEN> str;
		str.Format(KDescFmt, i+1);
		
		entry->SetDescriptionL(str);
		
		str.Format(KLocFmt, i+1);
		entry->SetLocationL(str);
		
		TTime now;
		
		if(aAlarms[i].iFixed)
			{
			now.UniversalTime();
			}
		else
			{
			now.HomeTime();
			}
		
		TTime myStart1 = now + TTimeIntervalMinutes(aAlarms[i].iAlarmTime);
		TTime myEnd1 = myStart1 + TTimeIntervalHours(1);
		
		TCalTime start;
		TCalTime end;

		if(aAlarms[i].iFixed)
			{
			start.SetTimeUtcL(myStart1);
			end.SetTimeUtcL(myEnd1);
			}
		else
			{
			start.SetTimeLocalFloatingL(myStart1);
			end.SetTimeLocalFloatingL(myEnd1);
			}

		INFO_PRINTF2(KAlarmStartFmt, aAlarms[i].iName.PtrZ());
		TBuf<32> buf;
		start.TimeLocalL().FormatL(buf, KTimeFmt);
		INFO_PRINTF2(KStartLocalFmt, buf.PtrZ());

		start.TimeUtcL().FormatL(buf, KTimeFmt);
		INFO_PRINTF2(KStartUtcFmt, buf.PtrZ());

		end.TimeLocalL().FormatL(buf, KTimeFmt);
		INFO_PRINTF2(KEndLocalFmt, buf.PtrZ());

		end.TimeUtcL().FormatL(buf, KTimeFmt);
		INFO_PRINTF2(KEndUtcFmt, buf.PtrZ());

		entry->SetStartAndEndTimeL(start, end);
		
		CCalAlarm* alarm = CCalAlarm::NewL();
		
		CleanupStack::PushL(alarm);
		aCalAlarms.AppendL(alarm);
		CleanupStack::Pop(alarm);
		
		alarm->SetAlarmSoundNameL(KCMySound);
		alarm->SetTimeOffset(0);
		
		entry->SetAlarmL(alarm);
		
		aEntries.AppendL(entry);
		CleanupStack::Pop(entry);
		}
	}
