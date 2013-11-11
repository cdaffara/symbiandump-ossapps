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

// System includes
#include <e32debug.h>
#include <e32test.h>

#include <asclisession.h>
#include <calalarm.h>
#include <calentryview.h>
#include <calrrule.h>
#include <calsession.h>
#include <tz.h>

#include <coreappstest/utilities.h>

RTest test(_L("tcal_queueAlarmsOnStartup"));

_LIT8(KEntryGuid, "A_DUMMY_GUID");
_LIT(KCalendarFile, "tcal_queueAlarmsOnStartup");
_LIT(KFormatDateTime, "%D%M%Y%/0%1%/1%2%/2%3%/3 %H:%T:%S");

	
class CTestApp : public CBase, public MCalProgressCallBack
	{
public:
	~CTestApp();
	
	void DoOpenCalendarL(TTime aAlarmTime);
	void DoNotOpenCalendarL(TTime aSampleTime, TTimeIntervalSeconds aTimeout, TInt aInitialExpectedCount, TInt aFinalExpectedCount);
	
	static void ClearAlarmServerL();
	static void CreateCalendarFileL(CCalTestLibrary* aTestLib);
	
	// from MCalProgressCallBack
	void Progress(TInt ) {};
	TBool NotifyProgress() {return EFalse; };
	void Completed(TInt aError);
	
private:
	static void AddAlarmedEntryL( CCalEntryView& aEntryView, CCalEntry::TType aType, 
	    TTime aStartTime, TCalRRule* aRule, TInt aAlarmOffset = 1, 
	    TBool aFloating = EFalse );
	void CompletedEntryViewOpenL();
	    
private:
	CCalSession* iSession;
	CCalEntryView* iEntryView;
	TTime iAlarmTime;
	};

// Add an alarmed entry, alarm to expire 1 minute before aStartTime, except for Todos.
// Entry end time is one hour after start time (so Todo alarm time will be 59 minutes after aStartTime)
void CTestApp::AddAlarmedEntryL( CCalEntryView& aEntryView, CCalEntry::TType aType, 
    TTime aStartTime, TCalRRule* aRule, TInt aAlarmOffset, TBool aFloating )
	{
	test.Printf(_L("Adding entry of type %d\n"), aType);
	
	RPointerArray<CCalEntry> entriesToAdd;
	CleanupClosePushL(entriesToAdd);
	
	TCalTime calStartTime;
	TTime endTime(aStartTime + TTimeIntervalHours(1));
	TCalTime calEndTime;

	if (aFloating)
		{
		calStartTime.SetTimeLocalFloatingL(aStartTime);
		calEndTime.SetTimeLocalFloatingL(endTime);
		}
	else
		{
		calStartTime.SetTimeLocalL(aStartTime);
		calEndTime.SetTimeLocalL(endTime);
		}

	TBuf<50> startBuf;
    if ( aType == CCalEntry::ETodo )
        {
        endTime.FormatL(startBuf, KFormatDateTime());
        }
    else
        {
        aStartTime.FormatL(startBuf, KFormatDateTime());
        }
	test.Printf(_L("Entry Time is %S, Alarm Offset is %d minute\n"), &startBuf, aAlarmOffset);

	
	TBuf8<64> uid;
	uid.Copy(KEntryGuid());
	uid.AppendNum(aStartTime.Int64());
	if ( aAlarmOffset >= 0 )
	    {
	    uid.Append(_L("+"));
	    }
	uid.AppendNum(aAlarmOffset);
	HBufC8* guid = uid.AllocLC();
	CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	entry->SetStartAndEndTimeL(calStartTime, calEndTime);
	if (aRule)
		{
		entry->SetRRuleL(*aRule);
		}
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(aAlarmOffset);
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);

	entriesToAdd.AppendL(entry);
	CleanupStack::Pop(entry);
	
	TInt entriesAdded(0);
	aEntryView.StoreL(entriesToAdd, entriesAdded);
	test(entriesAdded == 1);

	entriesToAdd.ResetAndDestroy();
	CleanupStack::Pop(); // entriesToAdd.Close()

	}

// clear all alarms
void CTestApp::ClearAlarmServerL()
	{
	RASCliSession almSession;
	test(almSession.Connect() == KErrNone);
	CleanupClosePushL(almSession);

	// delete all alarms
	RArray<TAlarmId> idArray;
	CleanupClosePushL(idArray);
	almSession.GetAlarmIdListL(idArray);
	for (TInt index = idArray.Count() - 1; index >= 0; --index)
		{
		almSession.AlarmDelete(idArray[index]);
		}
	CleanupStack::PopAndDestroy(&idArray);

	CleanupStack::PopAndDestroy(&almSession);
	}

// set up the calendar file with 4 alarms
void CTestApp::CreateCalendarFileL(CCalTestLibrary* aTestLib)
	{
	test.Printf(_L("Creating Calendar File\n"));
	
	aTestLib->ReplaceFileL(KCalendarFile());
	aTestLib->OpenFileL(KCalendarFile());
	
	CCalEntryView& entryView = aTestLib->SynCGetEntryViewL();

	TBuf<50> homeBuf;
	TTime homeTime;
	homeTime.HomeTime();
    homeTime.FormatL(homeBuf,KFormatDateTime());
	test.Printf(_L("Current Time is %S\n"), &homeBuf);

    //
	// Create the entries for orphaned alarms - set to expire while Calendar is closed.
    //
        // With positive alarm offsets (1):
	// 1st January, 8:05:00 - alarm at 8:04
	TTime appt1Time(TDateTime(2005, EJanuary, 0, 8, 5, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EAppt, appt1Time, NULL);

	// 1st January, 8:10:00 - alarm at 8:09
	TTime rem1Time(TDateTime(2005, EJanuary, 0, 8, 10, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EReminder, rem1Time, NULL);

	// 1st January, 8:15:00 - alarm at 8:14
	TTime todo1Time(TDateTime(2005, EJanuary, 0, 7, 15, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::ETodo, todo1Time, NULL);

	// 1st January, 8:20:00 - alarm at 8:19
	TTime anniv1Time(TDateTime(2005, EJanuary, 0, 8, 20, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EAnniv, anniv1Time, NULL);

	// 1st January, 8:25:00 - alarm at 8:24
	TTime event1Time(TDateTime(2005, EJanuary, 0, 8, 25, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EEvent, event1Time, NULL);

        // With Negative alarm offsets (-1):
	// 1st January, 10:05:00 - alarm at 10:06
	TTime appt2Time(TDateTime(2005, EJanuary, 0, 10, 5, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EAppt, appt2Time, NULL, -1);

	// 1st January, 10:10:00 - alarm at 10:11
	TTime rem2Time(TDateTime(2005, EJanuary, 0, 10, 10, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EReminder, rem2Time, NULL, -1);

	// 1st January, 10:15:00 - alarm at 10:16
	TTime todo2Time(TDateTime(2005, EJanuary, 0, 9, 15, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::ETodo, todo2Time, NULL, -1);

	// 1st January, 10:20:00 - alarm at 10:21
	TTime anniv2Time(TDateTime(2005, EJanuary, 0, 10, 20, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EAnniv, anniv2Time, NULL, -1);

	// 1st January, 10:25:00 - alarm at 10:26
	TTime event2Time(TDateTime(2005, EJanuary, 0, 10, 25, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EEvent, event2Time, NULL, -1);

    //
    // Create the entries for session alarms - set to expire while Calendar is opened
    //
	// 5th April, 16:03:00 - alarm at 16:02
	TTime apptTime(TDateTime(2005, EApril, 4, 16, 3, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::EAppt, apptTime, NULL);
	
	// 23rd April, 11:26:00 - alarm at 12:25
	TTime todoTime(TDateTime(2005, EApril, 22, 11, 26, 0, 0));
	AddAlarmedEntryL(entryView, CCalEntry::ETodo, todoTime, NULL);

	TCalTime calTime;
		
	// 6th May, 09:47:00 - alarm at 09:46 on 10th
	TTime apptRptTime(TDateTime(2005, EMay, 5, 9, 47, 0, 0));
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(2);
	rule.SetCount(5);
	calTime.SetTimeLocalL(apptRptTime);
	rule.SetDtStart(calTime);
	AddAlarmedEntryL(entryView, CCalEntry::EAppt, apptRptTime, &rule);

	// 18th June, 14:58:00 - alarm at 15:57 on 22nd
	TTime todoRptTime(TDateTime(2005, EJune, 17, 14, 58, 0, 0));
	TCalRRule rule2(TCalRRule::EDaily);
	rule2.SetInterval(2);
	rule2.SetCount(5);
	calTime.SetTimeLocalL(todoRptTime);
	rule2.SetDtStart(calTime);
	AddAlarmedEntryL(entryView, CCalEntry::ETodo, todoRptTime, &rule2);
	
	//-------------------
	// Negative alarms.
	//-------------------

	// 00:00 on April 1st 05
	TTime negAppt1Time(TDateTime(2005, EApril, 0, 0, 0, 0, 0)); 
	// alarm at 09:00
	AddAlarmedEntryL(entryView, CCalEntry::EAppt, negAppt1Time, NULL, -540); 

    // 00:00 on April 1st 05
	TTime negTodo1Time(TDateTime(2005, EApril, 0, 0, 0, 0, 0)); 
	// alarm at 11:00
	AddAlarmedEntryL(entryView, CCalEntry::ETodo, negTodo1Time, NULL, -600); 

    //---------------------------
	// Negative floating alarms.
    //---------------------------

	// 00:00 on April 11th 05
	TTime negAppt2Time(TDateTime(2005, EApril, 10, 0, 0, 0, 0)); 
	// alarm at 09:00
	AddAlarmedEntryL(entryView, CCalEntry::EAppt, negAppt2Time, NULL, -540, ETrue); 

    // 00:00 on April 11th 05
	TTime negTodo2Time(TDateTime(2005, EApril, 10, 0, 0, 0, 0)); 
	// alarm at 11:00
	AddAlarmedEntryL(entryView, CCalEntry::ETodo, negTodo2Time, NULL, -600, ETrue); 

    //----------------------------
	// Repeating negative alarms.
    //----------------------------

	TTime negAppt1RptTime(TDateTime(2005, EMay, 0, 0, 0, 0, 0));
	TCalRRule rule3(TCalRRule::EDaily);
	rule3.SetInterval(1);
	rule3.SetCount(5);
	calTime.SetTimeLocalL(negAppt1RptTime);
	rule3.SetDtStart(calTime);
	// alarms at 09:00 on 1st-5th May 05
	AddAlarmedEntryL(entryView, CCalEntry::EAppt, negAppt1RptTime, &rule3, -540); 
	// alarms at 11:00 on 1st-5th May 05
	AddAlarmedEntryL(entryView, CCalEntry::ETodo, negAppt1RptTime, &rule3, -600); 

	TTime negAppt2RptTime(TDateTime(2005, EMay, 10, 0, 0, 0, 0));
	TCalRRule rule4(TCalRRule::EDaily);
	rule4.SetInterval(1);
	rule4.SetCount(5);
	calTime.SetTimeLocalFloatingL(negAppt2RptTime);
	rule4.SetDtStart(calTime);
	// alarms at 09:00 on 11th-15th May 05	
	AddAlarmedEntryL(entryView, CCalEntry::EAppt, negAppt2RptTime, &rule4, -540, ETrue); 
	// alarms at 11:00 on 11th-15th May 05
	AddAlarmedEntryL(entryView, CCalEntry::ETodo, negAppt2RptTime, &rule4, -600, ETrue); 
	}
	
CTestApp::~CTestApp()
	{
	delete iEntryView;
	delete iSession;
	CCalTestLibrary::WaitForAgendaServerClose();
	}
	
void CTestApp::Completed(TInt /*aError*/)
	{
	RDebug::Print(_L("Entry view opening complete\n"));
	TRAP_IGNORE(CompletedEntryViewOpenL());
		
	CActiveScheduler::Stop();
	}

void CTestApp::CompletedEntryViewOpenL()
	{
	RASCliSession almSession;
	if (almSession.Connect() == KErrNone)
		{
		TInt notified = almSession.AlarmCountByState(EAlarmStateNotified) + almSession.AlarmCountByState(EAlarmStateNotifying);
		TTime homeTime;
		homeTime.HomeTime();
		RDebug::Print(_L("Time=%d:%d:%d\n"), homeTime.DateTime().Hour(), homeTime.DateTime().Minute(), homeTime.DateTime().Second());
		RDebug::Print(_L("Expecting alarm at %d:%d\n"), iAlarmTime.DateTime().Hour(), iAlarmTime.DateTime().Minute());
		RDebug::Print(_L("Alarms notified: %d\n"), notified);
		
		// keep waiting until an alarm goes off
		while (!notified)
			{
			User::After(1000000); // wait 1s
			notified = almSession.AlarmCountByState(EAlarmStateNotified) + almSession.AlarmCountByState(EAlarmStateNotifying);
			homeTime.HomeTime();
			RDebug::Print(_L("Time=%d:%d:%d, alarms notified=%d\n"), homeTime.DateTime().Hour(), homeTime.DateTime().Minute(), homeTime.DateTime().Second(), notified);
			}
		RDebug::Print(_L("Alarm went off\n"));
		
		
		// delete all alrms in state EAlarmStateNotifying
		RArray<TAlarmId> idArray;
		CleanupClosePushL(idArray);
		almSession.GetAlarmIdListByStateL(EAlarmStateNotifying, idArray);
		for (TInt index = idArray.Count() - 1; index >= 0; --index)
			{
			almSession.AlarmDelete(idArray[index]);
			}
		CleanupStack::PopAndDestroy(&idArray);
		
		// delete all alrms in state EAlarmStateNotified
		CleanupClosePushL(idArray);
		almSession.GetAlarmIdListByStateL(EAlarmStateNotified, idArray);
		for (TInt index = idArray.Count() - 1; index >= 0; --index)
			{
			almSession.AlarmDelete(idArray[index]);
			}
		CleanupStack::PopAndDestroy(&idArray);
		}
	else
		{
		RDebug::Print(_L("Error connecting to alarm server. Test failure!!\n"));
		User::Panic(_L("Error connecting to alarm server"), 0);
		}
	}

void CTestApp::DoNotOpenCalendarL(TTime aSampleTime, TTimeIntervalSeconds aTimeout, TInt aInitialExpectedCount, TInt aFinalExpectedCount)
    {
	iAlarmTime = aSampleTime;

	RTz tz;
	User::LeaveIfError(tz.Connect());
	CleanupClosePushL(tz);
	User::LeaveIfError(tz.SetHomeTime(iAlarmTime));
	CleanupStack::PopAndDestroy(&tz); // tz.Close()
	
	RASCliSession almSession;
	CleanupClosePushL(almSession);
	if (almSession.Connect() != KErrNone)
	    {
	    RDebug::Print(_L("Error connecting to alarm server. Test failure!!\n"));
		User::Panic(_L("Error connecting to alarm server"), 0);
	    }

	RDebug::Print(_L("Testing for alarms without Opening Calendar File\n"));
	TInt notifying = almSession.AlarmCountByState(EAlarmStateNotifying);
	TInt notified = almSession.AlarmCountByState(EAlarmStateNotified);
	TTime homeTime;
	homeTime.HomeTime();
	TBuf<50> homeBuf;
    homeTime.FormatL(homeBuf,KFormatDateTime());
	RDebug::Print(_L("Time=%S, alarms notifying=%d, notified=%d, expected=%d\n"), &homeBuf, notifying, notified, aInitialExpectedCount);
	if (notified+notifying != aInitialExpectedCount)
	    {
	    User::Panic(_L("Unexpected Notifying Alarms"), 0);
	    }

	// wait for the event or until the timeout after aTimeout seconds
	CAlarmEventWatcher* alarmwatcher = CAlarmEventWatcher::NewL(almSession);
	CleanupStack::PushL(alarmwatcher);
	const TInt KTimeOutPeriod(aTimeout.Int() * 1000000);
	alarmwatcher->StartWaitingForEventL(EAlarmChangeEventTimerExpired, KTimeOutPeriod);
	CleanupStack::PopAndDestroy(alarmwatcher);
	
    notifying = almSession.AlarmCountByState(EAlarmStateNotifying);
	notified = almSession.AlarmCountByState(EAlarmStateNotified);
	homeTime.HomeTime();
    homeTime.FormatL(homeBuf,KFormatDateTime());
	RDebug::Print(_L("Time=%S, alarms notifying=%d, notified=%d, expected=%d\n"), &homeBuf, notifying, notified, aFinalExpectedCount);
	if (notified+notifying != aFinalExpectedCount)
	    {
		User::Panic(_L("Unexpected Notifying Alarms"), 0);
	    }

	RArray<TAlarmId> idArray;
	CleanupClosePushL(idArray);
	almSession.GetAlarmIdListByStateL(EAlarmStateNotifying, idArray);
	for (TInt index = idArray.Count() - 1; index >= 0; --index)
		{
		almSession.AlarmDelete(idArray[index]);
		}
	CleanupStack::PopAndDestroy(&idArray);
		
	// delete all alrms in state EAlarmStateNotified
	CleanupClosePushL(idArray);
	almSession.GetAlarmIdListByStateL(EAlarmStateNotified, idArray);
	for (TInt index = idArray.Count() - 1; index >= 0; --index)
		{
		almSession.AlarmDelete(idArray[index]);
		}
	CleanupStack::PopAndDestroy(&idArray);
	
    CleanupStack::PopAndDestroy(&almSession);
    }

void CTestApp::DoOpenCalendarL(TTime aAlarmTime)
	{
	iAlarmTime = aAlarmTime;
	
	RTz tz;
	User::LeaveIfError(tz.Connect());
	CleanupClosePushL(tz);
	User::LeaveIfError(tz.SetHomeTime(iAlarmTime - TTimeIntervalSeconds(3)));
	CleanupStack::PopAndDestroy(&tz); // tz.Close()
	
	iSession = CCalSession::NewL();
	
	RDebug::Print(_L("Opening calendar file...\n"));
	iSession->OpenL(KCalendarFile);
	
	TTime t1, t2;
	t1.UniversalTime();
	
	RDebug::Print(_L("Opening entry view...\n"));
	iEntryView = CCalEntryView::NewL(*iSession, *this);
	CActiveScheduler::Start();

	t2.UniversalTime();
	
	TTimeIntervalMicroSeconds ms = t2.MicroSecondsFrom(t1);
	RDebug::Print(_L("Time to open calendar file: %d ms"), ms.Int64()/1000);
	}

static void TestClosedAlarmL(TDateTime aDateTime, TTimeIntervalSeconds aTimeout, TInt aInitialExpectedCount, TInt aFinalExpectedCount)
    {
    CTestApp* testApp = new (ELeave) CTestApp();
    CleanupStack::PushL(testApp);
    testApp->DoNotOpenCalendarL(TTime(aDateTime), aTimeout, aInitialExpectedCount, aFinalExpectedCount);
    CleanupStack::PopAndDestroy(testApp);
    }

static void TestThisAlarmTimeL(TDateTime aDateTime)
	{
	CTestApp* testApp = new (ELeave) CTestApp();
	CleanupStack::PushL(testApp);
	testApp->DoOpenCalendarL(TTime(aDateTime));
	CleanupStack::PopAndDestroy(testApp);
	}

static void SetTimeZoneL(const TDesC8& aTimeZoneName)
	{
	CCalTestLibrary* testLib = CCalTestLibrary::NewLC();
	testLib->SetTimeZoneL(aTimeZoneName);
	CleanupStack::PopAndDestroy(testLib);
	}
	
static void DoTestsL()
	{
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 8,  3, 55, 0), TTimeIntervalSeconds(10), 0, 1); // appt
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 8,  8, 55, 0), TTimeIntervalSeconds(10), 0, 1); // reminder
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 8, 13, 55, 0), TTimeIntervalSeconds(10), 0, 1); // todo
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 8, 18, 55, 0), TTimeIntervalSeconds(10), 0, 1); // anniv
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 8, 23, 55, 0), TTimeIntervalSeconds(10), 0, 1); // event

	TestThisAlarmTimeL(TDateTime(2005, EJanuary, 0, 8,  4, 0, 0)); // appt
	TestThisAlarmTimeL(TDateTime(2005, EJanuary, 0, 8,  9, 0, 0)); // reminder
	TestThisAlarmTimeL(TDateTime(2005, EJanuary, 0, 8, 14, 0, 0)); // todo
	TestThisAlarmTimeL(TDateTime(2005, EJanuary, 0, 8, 19, 0, 0)); // anniv
	TestThisAlarmTimeL(TDateTime(2005, EJanuary, 0, 8, 24, 0, 0)); // event

	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 10,  5, 55, 0), TTimeIntervalSeconds(10), 0, 1); // appt
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 10, 10, 55, 0), TTimeIntervalSeconds(10), 0, 1); // reminder
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 10, 15, 55, 0), TTimeIntervalSeconds(10), 0, 1); // todo
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 10, 20, 55, 0), TTimeIntervalSeconds(10), 0, 1); // anniv
	TestClosedAlarmL(TDateTime(2005, EJanuary, 0, 10, 25, 55, 0), TTimeIntervalSeconds(10), 0, 1); // event

	TestThisAlarmTimeL(TDateTime(2005, EApril, 4, 16, 2, 0, 0));	// non-repeating appt
	TestThisAlarmTimeL(TDateTime(2005, EApril, 22, 12, 25, 0, 0));	// non-repeating todo
	TestThisAlarmTimeL(TDateTime(2005, EMay, 9, 9, 46, 0, 0));		// repeating appt
	TestThisAlarmTimeL(TDateTime(2005, EJune, 21, 15, 57, 0, 0));	// repeating todo

	// appt, negative alarm
	TestThisAlarmTimeL(TDateTime(2005, EApril, 0, 9, 0, 0, 0)); 
	// todo, negative alarm
	TestThisAlarmTimeL(TDateTime(2005, EApril, 0, 11, 0, 0, 0)); 
	// appt, negative alarm (floating)
	TestThisAlarmTimeL(TDateTime(2005, EApril, 10, 9, 0, 0, 0)); 
	// todo, negative alarm (floating)
	TestThisAlarmTimeL(TDateTime(2005, EApril, 10, 11, 0, 0, 0)); 

  	// appt, repeating negative alarm
  	TestThisAlarmTimeL(TDateTime(2005, EMay, 1, 9, 0, 0, 0)); 
	// todo, repeating negative alarm
	TestThisAlarmTimeL(TDateTime(2005, EMay, 1, 11, 0, 0, 0)); 
	// appt, repeating negative alarm (floating)
	TestThisAlarmTimeL(TDateTime(2005, EMay, 11, 9, 0, 0, 0)); 
	// todo, repeating negative alarm (floating)
	TestThisAlarmTimeL(TDateTime(2005, EMay, 11, 11, 0, 0, 0)); 
	
	// Change time zone and retest the above times (6 or 7 hours earlier)
	// Note that fixed negative alarms don't work when the time zone changes - 
	// this is a known limitation.
	_LIT8(KChicago, "America/Chicago");
	SetTimeZoneL(KChicago);

    // non-repeating appt
	TestThisAlarmTimeL(TDateTime(2005, EApril, 4, 10, 2, 0, 0));	
	// non-repeating todo
	TestThisAlarmTimeL(TDateTime(2005, EApril, 22, 6, 25, 0, 0));	
	// repeating appt
	TestThisAlarmTimeL(TDateTime(2005, EMay, 9, 3, 46, 0, 0));		
	// repeating todo
	TestThisAlarmTimeL(TDateTime(2005, EJune, 21, 9, 57, 0, 0));	

    // appt, negative alarm (floating)
	TestThisAlarmTimeL(TDateTime(2005, EApril, 10, 9, 0, 0, 0)); 
    // todo, negative alarm (floating)	
	TestThisAlarmTimeL(TDateTime(2005, EApril, 10, 11, 0, 0, 0)); 
	// appt, repeating negative alarm (floating)
	TestThisAlarmTimeL(TDateTime(2005, EMay, 11, 9, 0, 0, 0)); 
	 // todo, repeating negative alarm (floating)
	TestThisAlarmTimeL(TDateTime(2005, EMay, 11, 11, 0, 0, 0));
	}
	
static TInt TestAlarms(TAny* )
	{
	RDebug::Print(_L("New Thread started...\n"));
	TInt err(KErrNone);
	CTrapCleanup* cleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler();
	CActiveScheduler::Install(scheduler);

	TRAP(err, DoTestsL());
	
	delete scheduler;
	delete cleanup;
	
	RDebug::Print(_L("...Thread closed\n"));
	return err;
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	test.Printf(_L("Deleting Alarmerver.ini file\n"));
	_LIT(KIniFileName, "c:\\private\\101f5027\\AlarmServer.ini");
		
	TPerformanceTimer timer(test);
	timer.Start();

	// Run the test suite

	RFs fs;
	User::LeaveIfError(fs.Connect());
	fs.Delete(KIniFileName);	//IgnoreError
	fs.Close();
	
	CCalTestLibrary* testLib = CCalTestLibrary::NewLC(EFalse);

	RTz tz;
	User::LeaveIfError(tz.Connect());
	CleanupClosePushL(tz);
	User::LeaveIfError(tz.SetHomeTime(TTime(TDateTime(2005, EJanuary, 0, 0, 0, 0, 0))));
	CleanupStack::PopAndDestroy(&tz); // tz.Close()

	// clear all alarms
	CTestApp::ClearAlarmServerL();

	// initialise the calendar file
	CTestApp::CreateCalendarFileL(testLib);
    CleanupStack::PopAndDestroy(testLib);
    
	// create a new thread to test that alarms are queued correctly on startup
	test.Printf(_L("Opening new thread to test alarms"));
	RThread thread;
	thread.Create(_L("test on startup"), TestAlarms, KDefaultStackSize, &User::Allocator(), NULL);
	
    thread.Resume();
    TRequestStatus exitStatus;
    thread.Logon(exitStatus);
    User::WaitForRequest(exitStatus);
    test.Printf(_L("Thread Closed: Status=%d, ExitType=%d, ExitReason=%d)"), exitStatus.Int(), thread.ExitType(), thread.ExitReason());
    test(exitStatus.Int() == KErrNone);
    test(thread.ExitType() == EExitKill);
    test(thread.ExitReason() == KErrNone);

	testLib = CCalTestLibrary::NewLC(EFalse);
	testLib->DeleteFileL(KCalendarFile);
	CleanupStack::PopAndDestroy(testLib);
	    
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();
	}


/**

@SYMTestCaseID     PIM-TCAL-QUEUEALARMONSTARTUP-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-QUEUEALARMONSTARTUP-0001 Calendar Interim API Alarms Queued While Building Indexes test suite"));

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

