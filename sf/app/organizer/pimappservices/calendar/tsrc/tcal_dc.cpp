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
// tcal_dc - Test to ensure backwards compatibility between 9.1 and 9.2 Calendar files.
// This test loads in files from v9.1 and v9.2 and verifies that they are converted correctly to v9.5.
// 
//

// local includes
#include "caltestlib.h"
#include <calalarm.h>
#include <calcategory.h>
#include <calcategorymanager.h>
#include <calcontent.h>
#include <caldataformat.h>
#include <caldataexchange.h>
#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caliterator.h>
#include <calsession.h>
#include <caltime.h>
#include <caluser.h>

// System includes
#include <e32test.h>
#include <s32file.h>
#include <calrrule.h>
#include <vtzrules.h>

// Uncomment out one of these line to regenerate the DC file from scratch against either v9.1 or v9.2 code
// When running the test in v9.3, comment out both these lines.

// Note that the .mmp file needs updating too.
//#define __REGENERATE_9_1_DC_FILE__
//#define __REGENERATE_9_2_DC_FILE__
//#define __REGENERATE_9_4_DC_FILE__
//#define __REGENERATE_9_5_DC_FILE__

#if defined(__REGENERATE_9_1_DC_FILE__) || defined(__REGENERATE_9_2_DC_FILE__) || defined(__REGENERATE_9_4_DC_FILE__) || defined(__REGENERATE_9_5_DC_FILE__)
#define __REGENERATE_DC_FILE__
#endif

#ifdef __REGENERATE_DC_FILE__

#ifndef __REGENERATE_9_5_DC_FILE__
#include "agmentry.h"
#include "agmmodel.h"
#include "..\..\agnmodel\agtest\agntestlibrary.h"
#include <txtfmlyr.h>
#endif

_LIT(KCalendarFileToGenerate,"c:tcal_dc");
#else

_LIT(KCalendar81File,"tcal_dc_81");
_LIT(KCalendar91File,"tcal_dc_91");
_LIT(KCalendar92File,"tcal_dc_92");
_LIT(KCalendar94File,"tcal_dc_94");
_LIT(KCalendar95File,"tcal_dc_95");
_LIT(KCalendar91LargeFile,"tcal_dc_91_1200");
_LIT(KCalendar92CorruptFile,"tcal_dc_92_corrupt");

#endif

LOCAL_D RTest test(_L("tcal_dc"));

const TDateTime KParentStartTDateTime(2006, EJanuary, 0, 0, 0, 0, 0);
const TInt KParentDurationInHours = 2;

_LIT(KParentLocation, "location 1");
_LIT(KParentSummary, "summary 1");
_LIT(KParentDescription, "description 1");

_LIT(KParentAlarmName, "alarm name 1");
_LIT8(KParentAlarmContent, "alarm content 1");
_LIT8(KParentAlarmContentMimeType, "text/http");

_LIT(KChildLocation, "location 2");
_LIT(KChildSummary, "summary 2");
_LIT(KChildDescription, "description 2");

_LIT8(KGUID, "GUID");
_LIT8(KWeeklyGUID, "weekly");
_LIT8(KMonthlyByDayGUID, "monthly1");
_LIT8(KMonthlyByDateGUID, "monthly2");
_LIT8(KYearlyByDayGUID, "yearly1");
_LIT8(KYearlyByDateGUID, "yearly2");

const TDateTime KWeeklyStartTDateTime(2006, EJanuary, 2, 9, 0, 0, 0); // Tuesday
const TDateTime KWeeklyUntilTDateTime(2006, EMarch, 3, 9, 0, 0, 0);

const TDateTime KMonthlyByDayStartTDateTime(2006, EJanuary, 10, 9, 0, 0, 0); // second Wednesday
const TDateTime KMonthlyByDayUntilTDateTime(2006, EOctober, 10, 9, 0, 0, 0);

const TDateTime KMonthlyByDateStartTDateTime(2006, EJanuary, 24, 9, 0, 0, 0);
const TDateTime KMonthlyByDateUntilTDateTime(2006, EAugust, 24, 9, 0, 0, 0);

const TDateTime KYearlyByDayStartTDateTime(2006, EFebruary, 2, 9, 0, 0, 0); // 1st Fri in Feb
const TDateTime KYearlyByDayUntilTDateTime(2008, EFebruary, 2, 9, 0, 0, 0);

const TDateTime KYearlyByDateStartTDateTime(2006, EFebruary, 11, 9, 0, 0, 0);
const TDateTime KYearlyByDateUntilTDateTime(2009, EFebruary, 11, 9, 0, 0, 0);

const TDateTime	KEntryWithoutGuidTDateTime(2006, EApril, 17, 9, 0, 0, 0);

const TDateTime KEntryAllPropLastModTDateTime(2006, EApril, 15, 12, 30, 2, 0);
const TDateTime KEntryAllPropDTStampTDateTime(2006, EApril, 15, 13, 45, 0, 3);
const TDateTime KEntryAllPropCompleteTDateTime(2006, EApril, 15, 16, 58, 0, 0);
const TDateTime KEntryAllPropRDate1TDateTime(2006, EMay, 0, 12, 0, 0, 0);
const TDateTime KEntryAllPropRDate2TDateTime(2006, EMay, 1, 18, 0, 0, 0);
	
_LIT(KUserDefinedCategoryName, "user-defined category");
_LIT(KUserDefinedCategoryName2, "another user-defined one");

_LIT(KAttendeeAddress1, "address 1");
_LIT(KAttendeeSentBy1, "me@me.com");

_LIT(KAttendeeAddress2, "house");
_LIT(KAttendeeSentBy2, "1234567890@zxcvbnm.com");

_LIT(KAttendeeAddress3, "road");
_LIT(KAttendeeSentBy3, "@@@@@");

_LIT8(KPacificAuckland, "Pacific/Auckland");
_LIT8(KGUIDAllProperties, "all-props");

_LIT8(KNegativeAlarmGUID, "negative_alarm");
const TDateTime KNegativeAlarmEventTime(2006, EApril, 12, 9, 0, 0, 0);
const TInt KNegativeAlarmOffset = -180;

_LIT8(KSamedayAlarmGUID, "sameday_alarm");
const TDateTime KSamedayAlarmEventTime(2006, EApril, 12, 8, 0, 0, 0);
const TInt KSamedayAlarmOffset = 180;

_LIT8(KPreviousDayAlarmGUID, "prevday_alarm");
const TDateTime KPreviousDayAlarmEventTime(2006, EApril, 12, 7, 0, 0, 0);
const TInt KPreviousDayAlarmOffset = 1440;

_LIT8(KLargeOffsetAlarmGUID, "largeoffset_alarm");
const TDateTime KLargeOffsetAlarmEventTime(2006, EApril, 12, 5, 0, 0, 0);
const TInt KLargeOffsetAlarmOffset = 12345;

_LIT8(KMidnightAlarmGUID, "midnight_alarm");
const TDateTime KMidnightAlarmEventTime(2006, EApril, 12, 6, 0, 0, 0);
const TInt KMidnightAlarmOffset = 360;

_LIT8(KFloatingAppt, "floating_appt");
const TDateTime KFloatingApptStartTime(2005, EDecember, 0, 9, 0, 0, 0);
const TDateTime KFloatingApptEndTime(2005, EDecember, 1, 9, 0, 0, 0);
const TDateTime KFloatingApptUntilTime(2005, EDecember, 29, 9, 0, 0, 0);

_LIT8(KFloatingTodo, "floating_todo");
const TDateTime KFloatingTodoStartTime(2005, EDecember, 0, 12, 0, 0, 0);
const TDateTime KFloatingTodoEndTime(2005, EDecember, 1, 12, 0, 0, 0);
const TDateTime KFloatingTodoUntilTime(2005, EDecember, 29, 12, 0, 0, 0);

//

class CCalEntryTestApp : public CBase, public MCalProgressCallBack
	{
public:
	~CCalEntryTestApp();

	void DoTestsL();
	
	// from MCalProgressCallBack
	void Progress(TInt aPercentageCompleted);
	void Completed(TInt aError);
	TBool NotifyProgress();
	
private:
	void StoreEntryL(CCalEntry* aEntry);
	void CopyToCDriveL(const TDesC& aFileName);
	void OpenBlankFileL();

 	void CreateDCFileL();
 	void AddParentAndChildEntriesL(CCalEntry::TType aType); 	
	void AddRepeatRuleTypesL();
	void AddEntryWithoutGUIDL();
	void AddEntryWithAllPropertiesSetL();
	void AddAlarmedEntriesL();
	void AddAlarmedEntryL(const TDesC8& aGuid, const TTime& aEventTime, TInt aOffset);
	void AddFloatingEntriesL();
	void AddFloatingEntryL(CCalEntry::TType aType, const TDesC8& aGuid, const TTime& aStartTime, const TTime& aEndTime, const TTime& aUntilTime);

 	void TestFileLoadedL(TBool aTestAlarmAction);
 	void TestFileLoadedV94L();
 	void TestFileLoadedV95L();
 	void TestParentAndChildEntriesL(TBool aTestAlarmAction, TBool aTodoStartTimeStored);
 	void TestRepeatRuleTypesL();
	void TestEntryWithoutGUIDL();
	void TestEntryWithAllPropertiesSetL(TBool aTestTimeZoneYears);
	void TestAlarmedEntriesL();
	void TestAlarmedEntryL(const TDesC8& aGuid, TInt aOffset);
	void TestFloatingEntriesL();
	void TestFloatingEntryL(const TDesC8& aGuid, const TTime& aStartTime, const TTime& aEndTime, const TTime& aUntilTime);

#ifndef __REGENERATE_DC_FILE__
	void TestCategoriesL(const TDesC& aFile);
	void TestFilterCategoriesL();
	void TestAddCategoriesL();
	void TestDeleteCategoriesL();
	void TestCategoryWithoutBuiltIndexesL();
	void TestOpeningFileDuringConversionL();

	// old v9.2 tests
 	void V92AddEntriesDCFileL();
 	void V92ReadEntriesDCFileL();
 	void V92AddEntriesL(CCalEntry::TType aType);
 	void V92UpdateEntriesL();
 	void V92Test92CalendarFileCleanL();
	void V92Test92CalendarFileEntriesAddedL();
	void V92Test91CalendarFileCleanL();
	void V92Test91CalendarFileEntriesAddedL();
#endif

private:
	CCalTestLibrary* iTestLib;
	HBufC8* iFileName;
	TInt iErr;
	};
	
//

void CCalEntryTestApp::Progress(TInt )
	{
	}
	
void CCalEntryTestApp::Completed(TInt aError)
	{
	iErr = aError;
	CActiveScheduler::Stop();
	}

TBool CCalEntryTestApp::NotifyProgress()
	{
	return EFalse;
	}
	
void CCalEntryTestApp::CreateDCFileL()
	{
	// Create a number of entries to test DC.
	// Repeating alarmed entry of each type with summary, location, description and alarm action, and a child entry
	
	AddEntryWithoutGUIDL();
	
	AddParentAndChildEntriesL(CCalEntry::EAppt);
	AddParentAndChildEntriesL(CCalEntry::EEvent);
	AddParentAndChildEntriesL(CCalEntry::EAnniv);
	AddParentAndChildEntriesL(CCalEntry::EReminder);
	AddParentAndChildEntriesL(CCalEntry::ETodo);
	
	AddRepeatRuleTypesL();
	AddEntryWithAllPropertiesSetL();
	AddAlarmedEntriesL();
	AddFloatingEntriesL();
	
	TestFileLoadedL(EFalse);
	}

CCalEntryTestApp::~CCalEntryTestApp()
	{
	delete iTestLib;
	}
	
void CCalEntryTestApp::TestFileLoadedL(TBool aTestAlarmAction)
	{
	test.Printf(_L("Checking calendar entry data...\n"));
 	TestParentAndChildEntriesL(aTestAlarmAction, EFalse);
 	TestRepeatRuleTypesL();
	TestEntryWithoutGUIDL();
	TestEntryWithAllPropertiesSetL(ETrue);
	TestAlarmedEntriesL();
	TestFloatingEntriesL();
	}

void CCalEntryTestApp::TestFileLoadedV94L()
	{
	test.Printf(_L("Checking calendar entry data...\n"));
 	TestParentAndChildEntriesL(ETrue, EFalse);
 	TestRepeatRuleTypesL();
	TestEntryWithoutGUIDL();
	TestEntryWithAllPropertiesSetL(EFalse);
	TestAlarmedEntriesL();
	TestFloatingEntriesL();
	}

void CCalEntryTestApp::TestFileLoadedV95L()
	{
	test.Printf(_L("Checking calendar entry data...\n"));
 	TestParentAndChildEntriesL(ETrue, ETrue);
 	TestRepeatRuleTypesL();
	TestEntryWithAllPropertiesSetL(EFalse);
	TestAlarmedEntriesL();
	TestFloatingEntriesL();
	}

void CCalEntryTestApp::StoreEntryL(CCalEntry* aEntry)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	TInt success = 0;
	entryArray.AppendL(aEntry);
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	CleanupStack::PopAndDestroy(&entryArray);
	}
	
void CCalEntryTestApp::AddParentAndChildEntriesL(CCalEntry::TType aType)
	{
	TTime startTime = TTime(KParentStartTDateTime) + TTimeIntervalDays(aType);
	
	TBuf8<64> buffer;
	buffer.Copy(KGUID);
	buffer.AppendNum(aType);
	HBufC8* hBuf = buffer.AllocLC();
	CCalEntry* parentEntry = iTestLib->CreateCalEntryL(aType, hBuf);
	CleanupStack::Pop(hBuf);
	CleanupStack::PushL(parentEntry);
	
	TCalTime calStartTime, calEndTime;
	calStartTime.SetTimeLocalL(startTime);
	calEndTime.SetTimeLocalL(startTime + TTimeIntervalHours(KParentDurationInHours));
	parentEntry->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	parentEntry->SetLocationL(KParentLocation);
	parentEntry->SetSummaryL(KParentSummary);
	parentEntry->SetDescriptionL(KParentDescription);

	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetAlarmSoundNameL(KParentAlarmName);
	alarm->SetTimeOffset(TTimeIntervalMinutes(5));
	
#ifndef __REGENERATE_9_1_DC_FILE__
	CCalContent* alarmContent = CCalContent::NewL();
	CleanupStack::PushL(alarmContent);
	
	HBufC8* urlContentBuf = KParentAlarmContent().AllocLC();
	HBufC8* urlTypeBuf = KParentAlarmContentMimeType().AllocLC();
	
	alarmContent->SetContentL(urlContentBuf, urlTypeBuf, CCalContent::EDispositionUrl);
	
	alarm->SetAlarmAction(alarmContent);
	CleanupStack::Pop(urlTypeBuf);
	CleanupStack::Pop(urlContentBuf);
	CleanupStack::Pop(alarmContent);
#endif // __REGENERATE_9_2_DC_FILE__
	
	parentEntry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);

	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(1);
	rule.SetDtStart(calStartTime);
	rule.SetCount(100);
	parentEntry->SetRRuleL(rule);
	
	StoreEntryL(parentEntry);
	CleanupStack::PopAndDestroy(parentEntry);
	
	// create child entry
	hBuf = buffer.AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(aType, hBuf, CCalEntry::EMethodNone, 0, calStartTime, CalCommon::EThisOnly);
	CleanupStack::Pop(hBuf);
	CleanupStack::PushL(childEntry);
	
	childEntry->SetStartAndEndTimeL(calStartTime, calStartTime);
	childEntry->SetLocationL(KChildLocation);
	childEntry->SetSummaryL(KChildSummary);
	childEntry->SetDescriptionL(KChildDescription);
	
	StoreEntryL(childEntry);
	CleanupStack::PopAndDestroy(childEntry);
	}

void CCalEntryTestApp::TestParentAndChildEntriesL(TBool aTestAlarmAction, TBool aTodoStartTimeStored)
	{
	// check parent / child entries
	TCalTime startDate;
	startDate.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endDate;
	endDate.SetTimeLocalL(TCalTime::MaxTime());
	
	CalCommon::TCalTimeRange timeRange(startDate, endDate);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll|CalCommon::EIncludeRptsNextInstanceOnly;
	
	RPointerArray<CCalInstance> instanceArray;
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceArray, filter, timeRange);
	test(instanceArray.Count() > 0);
	
	TInt instancesChecked = 0;
	for (TInt i = 0; i < instanceArray.Count(); i++)
		{
		CCalEntry* entry = &instanceArray[i]->Entry();
		if (entry->UidL().Left(KGUID().Length()) == KGUID())
			{
			instancesChecked++;
			TCalRRule rule;
			if (entry->GetRRuleL(rule))
				{
				// must be parent entry
				CCalEntry::TType type = entry->EntryTypeL();
				
				TTime expectedLocalStartTime = TTime(KParentStartTDateTime) + TTimeIntervalDays(type);
				TTime expectedLocalEndTime = expectedLocalStartTime + TTimeIntervalHours(KParentDurationInHours);
				
				if (type == CCalEntry::ETodo && !aTodoStartTimeStored)
					{
					expectedLocalStartTime = expectedLocalEndTime;
					}
				else if (type == CCalEntry::EReminder)
					{
					expectedLocalEndTime = Time::NullTTime();
					}
				
#ifndef __REGENERATE_9_5_DC_FILE__				
				test(entry->StartTimeL().TimeLocalL() == expectedLocalStartTime);
#endif
				test(entry->EndTimeL().TimeLocalL() == expectedLocalEndTime);
				
				test(entry->LocationL() == KParentLocation());
				test(entry->SummaryL() == KParentSummary());
				test(entry->DescriptionL() == KParentDescription());
				test(rule.Count() == 100);
				CCalAlarm* alarm = entry->AlarmL();
				test(alarm->AlarmSoundNameL() == KParentAlarmName());
				test(alarm->TimeOffset() == TTimeIntervalMinutes(5)); 
#ifndef __REGENERATE_9_1_DC_FILE__
				if (aTestAlarmAction)
					{
					CCalContent* content = alarm->AlarmAction();
					
					test(content->Content() == KParentAlarmContent());
					test(content->MimeType() == KParentAlarmContentMimeType());
					}
#endif
				delete alarm;
				}
			else
				{
				test(entry->LocationL() == KChildLocation());
				test(entry->SummaryL() == KChildSummary());
				test(entry->DescriptionL() == KChildDescription());
				}
			}
		}
		
	test(instancesChecked > 0);
	test(instancesChecked == 10); // 5 of each entry type
	
	instanceArray.ResetAndDestroy();
	}


void CCalEntryTestApp::AddRepeatRuleTypesL()
	{
	// weekly
	HBufC8* wkGuid = KWeeklyGUID().AllocLC();
	CCalEntry* wkEntry = CCalEntry::NewL(CCalEntry::EAppt, wkGuid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(wkGuid);
	CleanupStack::PushL(wkEntry);
	
	TCalTime wkStart, wkUntil;
	wkStart.SetTimeLocalL(TTime(KWeeklyStartTDateTime));
	wkUntil.SetTimeLocalL(TTime(KWeeklyUntilTDateTime));
	wkEntry->SetStartAndEndTimeL(wkStart, wkStart);
	
	TCalRRule wkRule(TCalRRule::EWeekly);
	wkRule.SetDtStart(wkStart);
	wkRule.SetUntil(wkUntil);
	
	RArray<TDay> wkDays;
	CleanupClosePushL(wkDays);
	wkDays.AppendL(ETuesday);
	wkRule.SetByDay(wkDays);
	CleanupStack::PopAndDestroy(&wkDays); // wkDays.Close()

	wkEntry->SetRRuleL(wkRule);
	StoreEntryL(wkEntry);
	CleanupStack::PopAndDestroy(wkEntry);
	
	// monthly by day
	HBufC8* mn1Guid = KMonthlyByDayGUID().AllocLC();
	CCalEntry* mn1Entry = CCalEntry::NewL(CCalEntry::EAppt, mn1Guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(mn1Guid);
	CleanupStack::PushL(mn1Entry);
	
	TCalTime mn1Start, mn1Until;
	mn1Start.SetTimeLocalL(TTime(KMonthlyByDayStartTDateTime));
	mn1Until.SetTimeLocalL(TTime(KMonthlyByDayUntilTDateTime));
	mn1Entry->SetStartAndEndTimeL(mn1Start, mn1Start);
	
	TCalRRule mn1Rule(TCalRRule::EMonthly);
	mn1Rule.SetDtStart(mn1Start);
	mn1Rule.SetUntil(mn1Until);
	
	RArray<TCalRRule::TDayOfMonth> mn1Days;
	CleanupClosePushL(mn1Days);
	mn1Days.AppendL(TCalRRule::TDayOfMonth(EWednesday, 2));
	mn1Rule.SetByDay(mn1Days);
	CleanupStack::PopAndDestroy(&mn1Days); // mn1Days.Close()

	mn1Entry->SetRRuleL(mn1Rule);
	StoreEntryL(mn1Entry);
	CleanupStack::PopAndDestroy(mn1Entry);
	
	// monthly by date
	HBufC8* mn2Guid = KMonthlyByDateGUID().AllocLC();
	CCalEntry* mn2Entry = CCalEntry::NewL(CCalEntry::EAppt, mn2Guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(mn2Guid);
	CleanupStack::PushL(mn2Entry);
	
	TCalTime mn2Start, mn2Until;
	mn2Start.SetTimeLocalL(TTime(KMonthlyByDateStartTDateTime));
	mn2Until.SetTimeLocalL(TTime(KMonthlyByDateUntilTDateTime));
	mn2Entry->SetStartAndEndTimeL(mn2Start, mn2Start);
	
	TCalRRule mn2Rule(TCalRRule::EMonthly);
	mn2Rule.SetDtStart(mn2Start);
	mn2Rule.SetUntil(mn2Until);
	
	RArray<TInt> mn2Days;
	CleanupClosePushL(mn2Days);
	mn2Days.AppendL(24);
	mn2Rule.SetByMonthDay(mn2Days);
	CleanupStack::PopAndDestroy(&mn2Days); // mn2Days.Close()

	mn2Entry->SetRRuleL(mn2Rule);
	StoreEntryL(mn2Entry);
	CleanupStack::PopAndDestroy(mn2Entry);
	
	// yearly by day
	HBufC8* yr1Guid = KYearlyByDayGUID().AllocLC();
	CCalEntry* yr1Entry = CCalEntry::NewL(CCalEntry::EAppt, yr1Guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(yr1Guid);
	CleanupStack::PushL(yr1Entry);
	
	TCalTime yr1Start, yr1Until;
	yr1Start.SetTimeLocalL(TTime(KYearlyByDayStartTDateTime));
	yr1Until.SetTimeLocalL(TTime(KYearlyByDayUntilTDateTime));
	yr1Entry->SetStartAndEndTimeL(yr1Start, yr1Start);
	
	TCalRRule yr1Rule(TCalRRule::EYearly);
	yr1Rule.SetDtStart(yr1Start);
	yr1Rule.SetUntil(yr1Until);
	
	RArray<TCalRRule::TDayOfMonth> yr1Days;
	CleanupClosePushL(yr1Days);
	yr1Days.AppendL(TCalRRule::TDayOfMonth(EFriday, 1));
	yr1Rule.SetByDay(yr1Days);
	CleanupStack::PopAndDestroy(&yr1Days); // yr1Days.Close()

	RArray<TMonth> yr1Months;
	CleanupClosePushL(yr1Months);
	yr1Months.AppendL(EFebruary);
	yr1Rule.SetByMonth(yr1Months);
	CleanupStack::PopAndDestroy(&yr1Months); // yr1Months.Close()
	
	yr1Entry->SetRRuleL(yr1Rule);
	StoreEntryL(yr1Entry);
	CleanupStack::PopAndDestroy(yr1Entry);
	
	// yearly by date
	HBufC8* yr2Guid = KYearlyByDateGUID().AllocLC();
	CCalEntry* yr2Entry = CCalEntry::NewL(CCalEntry::EAppt, yr2Guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(yr2Guid);
	CleanupStack::PushL(yr2Entry);
	
	TCalTime yr2Start, yr2Until;
	yr2Start.SetTimeLocalL(TTime(KYearlyByDateStartTDateTime));
	yr2Until.SetTimeLocalL(TTime(KYearlyByDateUntilTDateTime));
	yr2Entry->SetStartAndEndTimeL(yr2Start, yr2Start);
	
	TCalRRule yr2Rule(TCalRRule::EYearly);
	yr2Rule.SetDtStart(yr2Start);
	yr2Rule.SetUntil(yr2Until);
	
	yr2Entry->SetRRuleL(yr2Rule);
	StoreEntryL(yr2Entry);
	CleanupStack::PopAndDestroy(yr2Entry);
	}

void CCalEntryTestApp::TestRepeatRuleTypesL()
	{
	// check repeat rule types
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	iTestLib->SynCGetEntryViewL().FetchL(KWeeklyGUID, entries);
	test(entries.Count() == 1);
	TCalRRule weekly;
	entries[0]->GetRRuleL(weekly);
	test(weekly.Type() == TCalRRule::EWeekly);
	test(weekly.DtStart().TimeLocalL() == KWeeklyStartTDateTime);
	test(weekly.Until().TimeLocalL() == KWeeklyUntilTDateTime);
	RArray<TDay> wkDays;
	weekly.GetByDayL(wkDays);
	test(wkDays.Count() == 1);
	test(wkDays[0] == ETuesday);
	wkDays.Close();
	entries.ResetAndDestroy();
	
	iTestLib->SynCGetEntryViewL().FetchL(KMonthlyByDayGUID, entries);
	test(entries.Count() == 1);
	TCalRRule monthly1;
	entries[0]->GetRRuleL(monthly1);
	test(monthly1.Type() == TCalRRule::EMonthly);
	test(monthly1.DtStart().TimeLocalL() == KMonthlyByDayStartTDateTime);
	test(monthly1.Until().TimeLocalL() == KMonthlyByDayUntilTDateTime);
	RArray<TCalRRule::TDayOfMonth> monthDays;
	monthly1.GetByDayL(monthDays);
	test(monthDays.Count() == 1);
	test(monthDays[0].Day() == EWednesday);
	test(monthDays[0].WeekInMonth() == 2);
	monthDays.Close();
	entries.ResetAndDestroy();
	
	iTestLib->SynCGetEntryViewL().FetchL(KMonthlyByDateGUID, entries);
	test(entries.Count() == 1);
	TCalRRule monthly2;
	entries[0]->GetRRuleL(monthly2);
	test(monthly2.Type() == TCalRRule::EMonthly);
	test(monthly2.DtStart().TimeLocalL() == KMonthlyByDateStartTDateTime);
	test(monthly2.Until().TimeLocalL() == KMonthlyByDateUntilTDateTime);
	RArray<TInt> monthDates;
	monthly2.GetByMonthDayL(monthDates);
	test(monthDates.Count() == 1);
	test(monthDates[0] == 24);
	monthDates.Close();
	entries.ResetAndDestroy();
	
	iTestLib->SynCGetEntryViewL().FetchL(KYearlyByDayGUID, entries);
	test(entries.Count() == 1);
	TCalRRule yearly1;
	entries[0]->GetRRuleL(yearly1);
	test(yearly1.Type() == TCalRRule::EYearly);
	test(yearly1.DtStart().TimeLocalL() == KYearlyByDayStartTDateTime);
	test(yearly1.Until().TimeLocalL() == KYearlyByDayUntilTDateTime);
	RArray<TMonth> months;
	yearly1.GetByMonthL(months);
	test(months.Count() == 1);
	test(months[0] == EFebruary);
	months.Close();
	RArray<TCalRRule::TDayOfMonth> yearlyDayOfMonths;
	yearly1.GetByDayL(yearlyDayOfMonths);
	test(yearlyDayOfMonths.Count() == 1);
	test(yearlyDayOfMonths[0].Day() == EFriday);
	test(yearlyDayOfMonths[0].WeekInMonth() == 1);
	yearlyDayOfMonths.Close();
	entries.ResetAndDestroy();
	
	iTestLib->SynCGetEntryViewL().FetchL(KYearlyByDateGUID, entries);
	test(entries.Count() == 1);
	TCalRRule yearly2;
	entries[0]->GetRRuleL(yearly2);
	test(yearly2.Type() == TCalRRule::EYearly);
	test(yearly2.DtStart().TimeLocalL() == KYearlyByDateStartTDateTime);
	test(yearly2.Until().TimeLocalL() == KYearlyByDateUntilTDateTime);
	entries.ResetAndDestroy();

	CleanupStack::PopAndDestroy(&entries);
	}

void CCalEntryTestApp::AddEntryWithoutGUIDL()
	{
#if defined(__REGENERATE_DC_FILE__) && !defined(__REGENERATE_9_5_DC_FILE__)
	CAgnTest* agnTest = CAgnTest::NewLC();
	CAgnEntryModel* model = agnTest->CreateModelL(CAgnTest::EEntry, KCalendarFileToGenerate());
	model->OpenL(KCalendarFileToGenerate(), 0, 0, 0);
	
	CAgnAppt* appt = static_cast<CAgnAppt*>(agnTest->CreateEntryLC(CAgnEntry::EAppt));
	appt->SetStartAndEndDateTime(KEntryWithoutGuidTDateTime, KEntryWithoutGuidTDateTime);
	model->AddEntryL(appt);
	CleanupStack::PopAndDestroy(appt);
	
	CleanupStack::PopAndDestroy(agnTest);
#endif
	}

void CCalEntryTestApp::TestEntryWithoutGUIDL()
	{
#if ! defined(__REGENERATE_9_5_DC_FILE__)
	// check entry without guid
	TCalTime startDate;
	startDate.SetTimeLocalL(TTime(KEntryWithoutGuidTDateTime));
	RPointerArray<CCalInstance> instanceArray;
	CleanupResetAndDestroyPushL(instanceArray);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceArray, CalCommon::EIncludeAll, CalCommon::TCalTimeRange(startDate, startDate));
	test(instanceArray.Count() == 1);		
	CCalEntry* entryWithoutGuid = &instanceArray[0]->Entry();
	test(entryWithoutGuid != NULL);
	TInt guidLength = entryWithoutGuid->UidL().Length();
	test(guidLength > 0);
	for (TInt i = 0; i < guidLength; i++)
		{
		TChar ch = entryWithoutGuid->UidL()[i];
		test(ch >= '0' && ch <= '9');
		}
	test(entryWithoutGuid->StartTimeL().TimeLocalL() == KEntryWithoutGuidTDateTime);
	CleanupStack::PopAndDestroy(); // instanceArray.ResetAndDestroy()
#endif
	}	

void CCalEntryTestApp::AddEntryWithAllPropertiesSetL()
	{
	HBufC8* guid = KGUIDAllProperties().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime dtStamp, compTime, rDate1, rDate2;
	dtStamp.SetTimeLocalL(TTime(KEntryAllPropDTStampTDateTime));
	compTime.SetTimeLocalL(TTime(KEntryAllPropCompleteTDateTime));
	rDate1.SetTimeLocalL(TTime(KEntryAllPropRDate1TDateTime));
	rDate2.SetTimeLocalL(TTime(KEntryAllPropRDate2TDateTime));

	entry->SetDTStampL(dtStamp);
	entry->SetCompletedL(ETrue, compTime);
	
	RArray<TCalTime> rDateList;
	CleanupClosePushL(rDateList);
	rDateList.AppendL(rDate1);
	rDateList.AppendL(rDate2);
	entry->SetRDatesL(rDateList);
	
	rDateList.Remove(0);
	entry->SetExceptionDatesL(rDateList);
	CleanupStack::PopAndDestroy(&rDateList);
	
	CCalCategory* cat1 = CCalCategory::NewL(CCalCategory::ECalHoliday);
	CleanupStack::PushL(cat1);
	entry->AddCategoryL(cat1);
	CleanupStack::Pop(cat1);

	CCalCategory* cat2 = CCalCategory::NewL(KUserDefinedCategoryName());
	CleanupStack::PushL(cat2);
	entry->AddCategoryL(cat2);
	CleanupStack::Pop(cat2);

	CCalAttendee* att1 = CCalAttendee::NewL(KAttendeeAddress1(), KAttendeeSentBy1());
	CleanupStack::PushL(att1);
	entry->AddAttendeeL(att1);
	CleanupStack::Pop(att1);

	CCalAttendee* att2 = CCalAttendee::NewL(KAttendeeAddress2(), KAttendeeSentBy2());
	CleanupStack::PushL(att2);
	entry->AddAttendeeL(att2);
	entry->SetPhoneOwnerL(att2);
	CleanupStack::Pop(att2);

	CCalAttendee* att3 = CCalAttendee::NewL(KAttendeeAddress3(), KAttendeeSentBy3());
	CleanupStack::PushL(att3);
	entry->SetOrganizerL(att3);
	CleanupStack::Pop(att3);
	
	entry->SetReplicationStatusL(CCalEntry::ERestricted);
	entry->SetPriorityL(66);
	
	TTime startTime = TDateTime(2003, EJanuary, 0, 0, 0, 0, 0);
	TTime endTime = startTime + TTimeIntervalYears(5);
	
	RTz tzServer;
	CleanupClosePushL(tzServer);
	User::LeaveIfError(tzServer.Connect());
	
	CTzId* tzId = CTzId::NewL(KPacificAuckland());
	CleanupStack::PushL(tzId);
	
	CTzRules* rules = tzServer.GetTimeZoneRulesL(*tzId, startTime, endTime, ETzWallTimeReference);
	CleanupStack::PushL(rules);
	entry->SetTzRulesL(*rules);
	CleanupStack::PopAndDestroy(rules);
	
	CleanupStack::PopAndDestroy(tzId);
	CleanupStack::PopAndDestroy(&tzServer);

	StoreEntryL(entry);
	CleanupStack::PopAndDestroy(entry);
	}

void CCalEntryTestApp::TestEntryWithAllPropertiesSetL(TBool aTestTimeZoneYears)
	{
	// check entry with all properties set
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	iTestLib->SynCGetEntryViewL().FetchL(KGUIDAllProperties(), entries);
	test(entries.Count() == 1);
	CCalEntry* entryAllProperties = entries[0];
	TTime lastModTime = entryAllProperties->LastModifiedDateL().TimeLocalL();
	test(lastModTime > TCalTime::MinTime() && lastModTime < TCalTime::MaxTime());
	test(entryAllProperties->DTStampL().TimeLocalL() == TTime(KEntryAllPropDTStampTDateTime));
	test(entryAllProperties->CompletedTimeL().TimeLocalL() == TTime(KEntryAllPropCompleteTDateTime));
	
	RArray<TCalTime> rdates;
	CleanupClosePushL(rdates);
	entryAllProperties->GetRDatesL(rdates);
	test(rdates.Count() == 2);
	test(rdates[0].TimeLocalL() == TTime(KEntryAllPropRDate1TDateTime));
	test(rdates[1].TimeLocalL() == TTime(KEntryAllPropRDate2TDateTime));
	rdates.Reset();
	
	entryAllProperties->GetExceptionDatesL(rdates);
	test(rdates.Count() == 1);
	test(rdates[0].TimeLocalL() == TTime(KEntryAllPropRDate2TDateTime));
	CleanupStack::PopAndDestroy(&rdates);
	
	RPointerArray<CCalCategory> categories = entryAllProperties->CategoryListL();
	test(categories.Count() == 2);
	test(categories[0]->Category() == CCalCategory::ECalHoliday);
	test(categories[1]->Category() == CCalCategory::ECalExtended);
	test(categories[1]->ExtendedCategoryName() == KUserDefinedCategoryName());

	RPointerArray<CCalAttendee> attendees = entryAllProperties->AttendeesL();
	test(attendees.Count() == 2);
	test(attendees[0]->Address() == KAttendeeAddress1());
	test(attendees[0]->SentBy() == KAttendeeSentBy1());
	test(attendees[1]->Address() == KAttendeeAddress2());
	test(attendees[1]->SentBy() == KAttendeeSentBy2());

	test(entryAllProperties->ReplicationStatusL() == CCalEntry::ERestricted);
	test(entryAllProperties->PriorityL() == 66);
	
	if (aTestTimeZoneYears)
		{
#if !(defined(__REGENERATE_9_5_DC_FILE__) || defined(__REGENERATE_9_4_DC_FILE__))  
		// years are different in PREQ1090
		CTzRules* rules = entryAllProperties->TzRulesL();
		CleanupStack::PushL(rules);
		test(rules != NULL);
		test(rules->StartYear() == 2003);
		test(rules->EndYear() == 2008);
		CleanupStack::PopAndDestroy(rules);
#endif
		}
	CleanupStack::PopAndDestroy(&entries);
	}

void CCalEntryTestApp::AddAlarmedEntriesL()
	{
	AddAlarmedEntryL(KNegativeAlarmGUID, KNegativeAlarmEventTime, KNegativeAlarmOffset);
	AddAlarmedEntryL(KSamedayAlarmGUID, KSamedayAlarmEventTime, KSamedayAlarmOffset);
	AddAlarmedEntryL(KPreviousDayAlarmGUID, KPreviousDayAlarmEventTime, KPreviousDayAlarmOffset);
	AddAlarmedEntryL(KLargeOffsetAlarmGUID, KLargeOffsetAlarmEventTime, KLargeOffsetAlarmOffset);
	AddAlarmedEntryL(KMidnightAlarmGUID, KMidnightAlarmEventTime, KMidnightAlarmOffset);
	}

void CCalEntryTestApp::AddAlarmedEntryL(const TDesC8& aGuid, const TTime& aEventTime, TInt aOffset)
	{
	HBufC8* guid = aGuid.AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime startTime;
	if (aOffset < 0)
		{
		// negative offsets only valid on floating alarms
		startTime.SetTimeLocalFloatingL(aEventTime);
		}
	else
		{
		startTime.SetTimeLocalL(aEventTime);
		}
	entry->SetStartAndEndTimeL(startTime, startTime);
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(aOffset);
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	
	StoreEntryL(entry);
	CleanupStack::PopAndDestroy(entry);
	}
	
void CCalEntryTestApp::TestAlarmedEntriesL()
	{
	TestAlarmedEntryL(KNegativeAlarmGUID, KNegativeAlarmOffset);
	TestAlarmedEntryL(KSamedayAlarmGUID, KSamedayAlarmOffset);
	TestAlarmedEntryL(KPreviousDayAlarmGUID, KPreviousDayAlarmOffset);
	TestAlarmedEntryL(KLargeOffsetAlarmGUID, KLargeOffsetAlarmOffset);
	TestAlarmedEntryL(KMidnightAlarmGUID, KMidnightAlarmOffset);
	}

void CCalEntryTestApp::TestAlarmedEntryL(const TDesC8& aGuid, TInt aOffset)
	{
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	iTestLib->SynCGetEntryViewL().FetchL(aGuid, entries);
	test(entries.Count() == 1);
	CCalAlarm* alarm = entries[0]->AlarmL();
	test(alarm->TimeOffset().Int()== aOffset);
	delete alarm;
	
	CleanupStack::PopAndDestroy(&entries);
	}

void CCalEntryTestApp::AddFloatingEntriesL()
	{
	AddFloatingEntryL(CCalEntry::EAppt, KFloatingAppt(), KFloatingApptStartTime, KFloatingApptEndTime, KFloatingApptUntilTime);
	AddFloatingEntryL(CCalEntry::ETodo, KFloatingTodo(), KFloatingTodoStartTime, KFloatingTodoEndTime, KFloatingTodoUntilTime);
	}
	
void CCalEntryTestApp::AddFloatingEntryL(CCalEntry::TType aType, const TDesC8& aGuid, const TTime& aStartTime, const TTime& aEndTime, const TTime& aUntilTime)
	{
	HBufC8* guid = aGuid.AllocLC();
	CCalEntry* entry = CCalEntry::NewL(aType, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime startTime, endTime, untilTime;
	startTime.SetTimeLocalFloatingL(aStartTime);
	endTime.SetTimeLocalFloatingL(aEndTime);
	untilTime.SetTimeLocalFloatingL(aUntilTime);
	entry->SetStartAndEndTimeL(startTime, endTime);
	
	TCalRRule rule(TCalRRule::EDaily);
	if (aType == CCalEntry::ETodo)
		{
		rule.SetDtStart(endTime);
		}
	else
		{
		rule.SetDtStart(startTime);
		}
	rule.SetUntil(untilTime);
	entry->SetRRuleL(rule);
	
	StoreEntryL(entry);
	CleanupStack::PopAndDestroy(entry);
	}

void CCalEntryTestApp::TestFloatingEntriesL()
	{
	TestFloatingEntryL(KFloatingAppt(), KFloatingApptStartTime, KFloatingApptEndTime, KFloatingApptUntilTime);
	TestFloatingEntryL(KFloatingTodo(), KFloatingTodoStartTime, KFloatingTodoEndTime, KFloatingTodoUntilTime);	
	}

void CCalEntryTestApp::TestFloatingEntryL(const TDesC8& aGuid, const TTime& aStartTime, const TTime& aEndTime, const TTime& aUntilTime)
	{
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	iTestLib->SynCGetEntryViewL().FetchL(aGuid, entries);
	test(entries.Count() == 1);
	test(entries[0]->StartTimeL().TimeLocalL() == aStartTime);
	test(entries[0]->EndTimeL().TimeLocalL() == aEndTime);
	TCalRRule rule;
	entries[0]->GetRRuleL(rule);
	test(rule.Until().TimeLocalL() == aUntilTime);
	
	test(entries[0]->StartTimeL().TimeMode() == TCalTime::EFloating);
	test(entries[0]->EndTimeL().TimeMode() == TCalTime::EFloating);
#ifndef __REGENERATE_DC_FILE__
	test(rule.Until().TimeMode() == TCalTime::EFloating);
#endif 
	
	CleanupStack::PopAndDestroy(&entries);
	}

#ifndef __REGENERATE_DC_FILE__
void CCalEntryTestApp::TestCategoriesL(const TDesC& aFile)
	{
	CalCommon::TCalFileVersionSupport status;
	
	CopyToCDriveL(aFile);
	test.Printf(_L("Testing Using Categories without building indexes...\n"));
	iTestLib->GetSession().OpenL(aFile, status);
	TestCategoryWithoutBuiltIndexesL();
	
	CopyToCDriveL(aFile);
	test.Printf(_L("Testing Filtering Categories...\n"));
	iTestLib->GetSession().OpenL(aFile, status);
	iTestLib->SynCGetEntryViewL();
	TestFilterCategoriesL();

	CopyToCDriveL(aFile);
	test.Printf(_L("Testing Adding Categories...\n"));
	iTestLib->DeleteViews();
	iTestLib->GetSession().OpenL(aFile, status);
	iTestLib->SynCGetEntryViewL();
	TestAddCategoriesL();
	
	CopyToCDriveL(aFile);
	test.Printf(_L("Testing Deleting Categories...\n"));
	iTestLib->GetSession().OpenL(aFile, status);
	iTestLib->SynCGetEntryViewL();
	TestDeleteCategoriesL();
	iTestLib->DeleteFileL(aFile, ETrue);
	}

void CCalEntryTestApp::TestFilterCategoriesL()
	{
	test.Printf(_L("Test opening an old file then filtering categories\n"));
	CCalCategoryManager* catMan = CCalCategoryManager::NewL(iTestLib->GetSession());
	CleanupStack::PushL(catMan);

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	CCalCategory* cat1 = CCalCategory::NewL(KUserDefinedCategoryName());
	CleanupStack::PushL(cat1);

	// find all entries in category
	catMan->FilterCategoryL(*cat1, entries, *iTestLib);
	CActiveScheduler::Start();
	
	test(entries.Count() == 1);
	test(entries[0]->UidL() == KGUIDAllProperties());
	entries.ResetAndDestroy();

	CleanupStack::PopAndDestroy(cat1);
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	CleanupStack::PopAndDestroy(catMan);
	}

void CCalEntryTestApp::TestCategoryWithoutBuiltIndexesL()
	{
	test.Printf(_L("Test filtering categories before building the indexes\n"));
	CCalCategoryManager* catMan = CCalCategoryManager::NewL(iTestLib->GetSession());
	CleanupStack::PushL(catMan);

	RPointerArray<CCalEntry> entries;

	CCalCategory* cat1 = CCalCategory::NewL(KUserDefinedCategoryName());
	CleanupStack::PushL(cat1);

	// find all entries in category
	catMan->FilterCategoryL(*cat1, entries, *this);
	CActiveScheduler::Start();
	
	test(iErr == KErrNotSupported);
	test(entries.Count() == 0);

	CleanupStack::PopAndDestroy(cat1);
	CleanupStack::PopAndDestroy(catMan);
	}

void CCalEntryTestApp::TestAddCategoriesL()
	{
	test.Printf(_L("Test opening an old file then adding categories\n"));
	CCalCategoryManager* catMan = CCalCategoryManager::NewL(iTestLib->GetSession());
	CleanupStack::PushL(catMan);

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	CCalCategory* cat1 = CCalCategory::NewL(KUserDefinedCategoryName2());
	CleanupStack::PushL(cat1);
	catMan->AddCategoryL(*cat1);
	
	// find all entries in category
	catMan->FilterCategoryL(*cat1, entries, *iTestLib);
	CActiveScheduler::Start();
	
	test(entries.Count() == 0);

	CleanupStack::PopAndDestroy(cat1);
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	CleanupStack::PopAndDestroy(catMan);
	}

void CCalEntryTestApp::TestDeleteCategoriesL()
	{
	test.Printf(_L("Test opening an old file then deleting categories\n"));
	CCalCategoryManager* catMan = CCalCategoryManager::NewL(iTestLib->GetSession());
	CleanupStack::PushL(catMan);

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	CCalCategory* cat1 = CCalCategory::NewL(KUserDefinedCategoryName());
	CleanupStack::PushL(cat1);

	// delete category
	catMan->DeleteCategoryL(*cat1, *iTestLib);
	CActiveScheduler::Start();

	// find all entries in category
	TRAPD(err, catMan->FilterCategoryL(*cat1, entries, *iTestLib));
	test(err == KErrNotFound);
	test(entries.Count() == 0);

	CleanupStack::PopAndDestroy(cat1);
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	CleanupStack::PopAndDestroy(catMan);
	}

//
// Tests written for v9.2 that should still work
//
// Add a parent and a child entry
//
void CCalEntryTestApp::V92AddEntriesL(CCalEntry::TType aType)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	TTime startTime;
	startTime.HomeTime();
	startTime += TTimeIntervalDays(aType);
	
	TBuf8<64> buffer;
	iTestLib->RandomText8(buffer);
	HBufC8* hBuf = buffer.AllocLC();
	CCalEntry* parentEntry = iTestLib->CreateCalEntryL(aType, hBuf);
	CleanupStack::Pop(hBuf);
	
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	parentEntry->SetStartAndEndTimeL(calStartTime, calStartTime);
	
	parentEntry->SetLocationL(KParentLocation);
	parentEntry->SetSummaryL(KParentSummary);
	parentEntry->SetDescriptionL(KParentDescription);

	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetAlarmSoundNameL(KParentAlarmName);
	alarm->SetTimeOffset(TTimeIntervalMinutes(5));
	
	parentEntry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);

	TCalRRule rule(TCalRRule::EDaily);
	rule.SetInterval(1);
	rule.SetDtStart(calStartTime);
	rule.SetCount(100);
	parentEntry->SetRRuleL(rule);
	
	entryArray.AppendL(parentEntry);
	
	// create child entry
	hBuf = buffer.AllocLC();
	CCalEntry* childEntry = CCalEntry::NewL(aType, hBuf, CCalEntry::EMethodNone, 0, calStartTime, CalCommon::EThisOnly);
	CleanupStack::Pop(hBuf);
	
	entryArray.AppendL(childEntry);
	
	childEntry->SetStartAndEndTimeL(calStartTime, calStartTime);
	childEntry->SetLocationL(KChildLocation);
	childEntry->SetSummaryL(KChildSummary);
	childEntry->SetDescriptionL(KChildDescription);
		
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	CleanupStack::PopAndDestroy(&entryArray);
	}

void CCalEntryTestApp::V92AddEntriesDCFileL()
	{
	// Create a number of entries to test DC.
	// Repeating alarmed entry of each type with summary, location, description and alarm action, and a child entry
	V92AddEntriesL(CCalEntry::EAppt);
	V92AddEntriesL(CCalEntry::EEvent);
	V92AddEntriesL(CCalEntry::EAnniv);
	V92AddEntriesL(CCalEntry::EReminder);
	V92AddEntriesL(CCalEntry::ETodo);
	}
	
void CCalEntryTestApp::V92ReadEntriesDCFileL()
	{
	TCalTime startDate;
	startDate.SetTimeLocalL(TCalTime::MinTime());
	TCalTime endDate;
	endDate.SetTimeLocalL(TCalTime::MaxTime());
	
	CalCommon::TCalTimeRange timeRange(startDate, endDate);
	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll|CalCommon::EIncludeRptsNextInstanceOnly;
	
	RPointerArray<CCalInstance> instanceArray;
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceArray, filter, timeRange);
	
	for (TInt i = 0; i < instanceArray.Count(); i++)
		{
		CCalEntry* entry = &instanceArray[i]->Entry();
		TCalRRule rule;
		if (entry->GetRRuleL(rule))
			{
			// must be parent entry
			test(entry->LocationL() == KParentLocation());
			test(entry->SummaryL() == KParentSummary());
			test(entry->DescriptionL() == KParentDescription());
			test(rule.Count() == 100);
			CCalAlarm* alarm = entry->AlarmL();
			test(alarm->AlarmSoundNameL() == KParentAlarmName());
			delete alarm;
			}
		else
			{
			test(entry->LocationL() == KChildLocation());
			test(entry->SummaryL() == KChildSummary());
			test(entry->DescriptionL() == KChildDescription());
			}
		}
	
	// check stuff
	instanceArray.ResetAndDestroy();
	}

// Fetch and update all the entries - parent and child in the Calendar file
void CCalEntryTestApp::V92UpdateEntriesL()
	{
	CCalIter* entryIter = CCalIter::NewL(iTestLib->GetSession());
	CleanupStack::PushL(entryIter);
	TPtrC8 uidIter(entryIter->FirstL());

	RPointerArray<CCalEntry> calEntryArray;
	CleanupResetAndDestroyPushL(calEntryArray);
	
	RPointerArray<CCalEntry> tempEntryArray;
	CleanupClosePushL(tempEntryArray);
	
	CCalEntry* entry = NULL;
		
	TInt successEntryCount=0;

	// Iterate through all the entries' UIDs and fetch all the entries (parent and child) with the UID
	// Call UpdateL on Parent Entries and StoreL on Child Entries
	while (uidIter != KNullDesC8())
		{
		iTestLib->SynCGetEntryViewL().FetchL(uidIter,calEntryArray);
		
		for(TInt entryCount=0;entryCount<calEntryArray.Count();++entryCount)
			{
			entry = calEntryArray[entryCount];

			tempEntryArray.AppendL(calEntryArray[entryCount]);
			
			if(entry->RecurrenceIdL().TimeUtcL()==Time::NullTTime())
				{
				// Parent Entry
				entry->SetLocationL(_L("Updated Parent Location 1"));
				entry->SetSummaryL(_L("Updated Parent Summary 1"));
				entry->SetDescriptionL(_L("Updated Parent Description 1"));

				successEntryCount=0;
				iTestLib->SynCGetEntryViewL().UpdateL(tempEntryArray, successEntryCount);
				test(successEntryCount==1);
				}
			else
				{
				// Child Entry
				entry->SetLocationL(_L("Updated Child Location 1"));
				entry->SetSummaryL(_L("Updated Child Summary 1"));
				entry->SetDescriptionL(_L("Updated Child Description 1"));
				
				successEntryCount=0;
				//iTestLib->SynCGetEntryViewL().StoreL(RPointerArray<CCalEntry>(&calEntryArray[entryCount],1),successEntryCount);
				iTestLib->SynCGetEntryViewL().StoreL(tempEntryArray, successEntryCount);
				test(successEntryCount==1);
				}
				
			tempEntryArray.Reset();
			}
		calEntryArray.ResetAndDestroy();
		uidIter.Set(entryIter->NextL());
		}
	CleanupStack::PopAndDestroy(&tempEntryArray);
	CleanupStack::PopAndDestroy(&calEntryArray);
	CleanupStack::PopAndDestroy(entryIter);
	}
	
// ===========
// 9.2 Calendar File - Clean / No entries
// ===========


void CCalEntryTestApp::V92Test92CalendarFileCleanL()
	{
	test.Next(_L("9.2 Calendar File - Clean / No entries"));

	
	_LIT(KOriginalCalendarFileBravoCleanFullPath, "z:\\private\\10003a5b\\tcal_dc_92_CalFile_Clean");
	_LIT(KCalendarFileBravoCleanFullPath, "c:\\private\\10003a5b\\tcal_dc_92_CalFile_Clean");
	_LIT(KCalendarFileBravoCleanTestFullPath, "c:\\private\\10003a5b\\tcal_dc_92_CalFile_Clean_Test");
	
	_LIT(KCalendarFileBravoCleanTest, "c:tcal_dc_92_CalFile_Clean_Test");

	// Copy the 9.2 Calendar file from the ROM drive to the C: to enable writing into it
	iTestLib->PIMTestServer().CopyFileL(KOriginalCalendarFileBravoCleanFullPath, KCalendarFileBravoCleanFullPath);
	
	// Step 1 = Create a copy of a clean (no entries) Bravo v9.2 Calendar file for test purposes
	iTestLib->PIMTestServer().CopyFileL(KCalendarFileBravoCleanFullPath, KCalendarFileBravoCleanTestFullPath);
	
	// Step 2 = Open the 9.2 test Calendar file
	iTestLib->OpenFileL(KCalendarFileBravoCleanTest());
	
	// Step 3 = Add multiple 9.2 parent and child entries to the 9.2 test Calendar file
	V92AddEntriesDCFileL();
	
	// Step 4 = Read and verify previously added 9.2 multiple parent and child entries within the 9.2 test Calendar file
	V92ReadEntriesDCFileL();
	
	// Step 5 = Update all entries in the 9.2 Calendar file	
	V92UpdateEntriesL();
	
	// Step 6 = Delete all entries in the 9.2 Calendar file	
	iTestLib->CleanDatabaseL();
	
	// Step 7 = Delete the 9.2 test Calendar files
	iTestLib->DeleteFileL(KCalendarFileBravoCleanTest());
	iTestLib->PIMTestServer().DeleteFileL(KCalendarFileBravoCleanFullPath());
	}

// ===========
// 9.2 Calendar File - Previously Added Entries
// ===========


void CCalEntryTestApp::V92Test92CalendarFileEntriesAddedL()
	{
	test.Next(_L("Calendar File - Previously Added Entries"));

	
	_LIT(KOriginalCalendarFileBravoEntriesAddedFullPath, "z:\\private\\10003a5b\\tcal_dc_92_CalFile_EntriesAdded");
	_LIT(KCalendarFileBravoEntriesAddedFullPath, "c:\\private\\10003a5b\\tcal_dc_92_CalFile_EntriesAdded");
	_LIT(KCalendarFileBravoTestEntriesAddedFullPath, "c:\\private\\10003a5b\\tcal_dc_92_CalFile_EntriesAdded_Test");
	
	_LIT(KCalendarFileBravoTestEntriesAdded,"c:tcal_dc_92_CalFile_EntriesAdded_Test");
	
	// Copy the 9.2 Calendar file from the ROM drive to the C: to enable writing into it
	iTestLib->PIMTestServer().CopyFileL(KOriginalCalendarFileBravoEntriesAddedFullPath, KCalendarFileBravoEntriesAddedFullPath);
	
	// Step 1 = Create a copy of Bravo v9.2 Calendar file saved earlier with 9.2 entries for test purposes
	iTestLib->PIMTestServer().CopyFileL(KCalendarFileBravoEntriesAddedFullPath, KCalendarFileBravoTestEntriesAddedFullPath);
	
	// Step 2 = Open the 9.2 test Calendar file
	iTestLib->OpenFileL(KCalendarFileBravoTestEntriesAdded());
	
	// Step 3 = Read and verify previously added 9.2 multiple parent and child entries within the 9.2 test environment
	V92ReadEntriesDCFileL();
	
	// Step 4 = Delete all entries in the 9.2 Calendar file	
	iTestLib->CleanDatabaseL();
	
	// Step 5 = Add multiple 9.2 parent and child entries to the 9.2 test Calendar file
	V92AddEntriesDCFileL();
	
	// Step 6 = Read and verify previously added 9.2 multiple parent and child entries within the 9.2 test Calendar file
	V92ReadEntriesDCFileL();
	
	// Step 7 = Update all entries in the 9.2 Calendar file	
	V92UpdateEntriesL();
	
	// Step 8 = Delete all entries in the 9.2 Calendar file	
	iTestLib->CleanDatabaseL();

	// Step 9 = Delete the 9.2 test Calendar files
	iTestLib->DeleteFileL(KCalendarFileBravoTestEntriesAdded());
	iTestLib->PIMTestServer().DeleteFileL(KCalendarFileBravoEntriesAddedFullPath());
	}

// ===========
// 9.1 Calendar File - Clean / No entries
// ===========


void CCalEntryTestApp::V92Test91CalendarFileCleanL()
	{
	test.Next(_L("9.1 Calendar File - Clean / No Entries"));

	
	_LIT(KOriginalCalendarFileMikeCleanFullPath, "z:\\private\\10003a5b\\tcal_dc_91_CalFile_Clean");
	_LIT(KCalendarFileMikeCleanFullPath, "c:\\private\\10003a5b\\tcal_dc_91_CalFile_Clean");
	_LIT(KCalendarFileMikeCleanTestFullPath, "c:\\private\\10003a5b\\tcal_dc_91_CalFile_Clean_Test");
	
	_LIT(KCalendarFileMikeCleanTest, "c:tcal_dc_91_CalFile_Clean_Test");
	
	// Copy 9.1 Calendar file from the ROM drive to the C: to enable writing into it
	iTestLib->PIMTestServer().CopyFileL(KOriginalCalendarFileMikeCleanFullPath, KCalendarFileMikeCleanFullPath);
	
	// Step 1 = Create a copy of a clean (no entries) Mike v9.1 Calendar file for test purposes
	iTestLib->PIMTestServer().CopyFileL(KCalendarFileMikeCleanFullPath, KCalendarFileMikeCleanTestFullPath);
	
	// Step 2 = Open the 9.1 Calendar file for test purposes
	iTestLib->OpenFileL(KCalendarFileMikeCleanTest());
		
	// Step 3 = Add multiple 9.2 parent and child entries to the 9.1 test Calendar file
	V92AddEntriesDCFileL();
	
	// Step 4 = Read and verify previously added 9.2 multiple parent and child entries within the 9.1 test Calendar file
	V92ReadEntriesDCFileL();

	// Step 5 = Update all entries in the 9.1 Calendar file	
	V92UpdateEntriesL();
	
	// Step 6 = Delete all entries in the 9.1 Calendar file	
	iTestLib->CleanDatabaseL();

	// Step 7 = Delete the 9.1 test Calendar files
	iTestLib->DeleteFileL(KCalendarFileMikeCleanTest());
	iTestLib->PIMTestServer().DeleteFileL(KCalendarFileMikeCleanFullPath());
	}
	
// ===========
// 9.1 Calendar File - Previously Added Entries
// ===========


void CCalEntryTestApp::V92Test91CalendarFileEntriesAddedL()
	{
	test.Next(_L("9.1 Calendar File - Previously Added Entries"));

	
	_LIT(KOriginalCalendarFileMikeEntriesAddedFullPath, "z:\\private\\10003a5b\\tcal_dc_91_CalFile_EntriesAdded");
	_LIT(KCalendarFileMikeEntriesAddedFullPath, "c:\\private\\10003a5b\\tcal_dc_91_CalFile_EntriesAdded");
	_LIT(KCalendarFileMikeTestEntriesAddedFullPath, "c:\\private\\10003a5b\\tcal_dc_91_CalFile_EntriesAdded_Test");
	
	_LIT(KCalendarFileMikeTestEntriesAdded,"c:tcal_dc_91_CalFile_EntriesAdded_Test");
	
	// Copy the 9.1 Calendar file from the ROM drive to the C: to enable writing into it
	iTestLib->PIMTestServer().CopyFileL(KOriginalCalendarFileMikeEntriesAddedFullPath, KCalendarFileMikeEntriesAddedFullPath);
	
	// Step 1 = Create a copy of Mike v9.1 Calendar file saved earlier with 9.1 entries for test purposes
	iTestLib->PIMTestServer().CopyFileL(KCalendarFileMikeEntriesAddedFullPath, KCalendarFileMikeTestEntriesAddedFullPath);
	
	// Step 2 = Open the 9.1 test Calendar file
	iTestLib->OpenFileL(KCalendarFileMikeTestEntriesAdded());
	
	// Step 3 = Read and verify previously added 9.1 multiple parent and child entries within the 9.2 test environment
	V92ReadEntriesDCFileL();
	
	// Step 4 = Delete all entries in the 9.1 Calendar file	
	iTestLib->CleanDatabaseL();
	
	// Step 5 = Add multiple 9.2 parent and child entries to the 9.1 test Calendar file
	V92AddEntriesDCFileL();
	
	// Step 6 = Read and verify previously added 9.2 multiple parent and child entries within the 9.1 test Calendar file
	V92ReadEntriesDCFileL();
	
	// Step 7 = Update all entries in the 9.1 Calendar file	
	V92UpdateEntriesL();
	
	// Step 8 = Delete all entries in the 9.1 Calendar file	
	iTestLib->CleanDatabaseL();
	
	// Step 9 = Delete the 9.1 test Calendar files
	iTestLib->DeleteFileL(KCalendarFileMikeTestEntriesAdded());
	iTestLib->PIMTestServer().DeleteFileL(KCalendarFileMikeEntriesAddedFullPath());
	}
#endif 

void CCalEntryTestApp::OpenBlankFileL()
	{
	TBuf<8> buf;
	buf.AppendNum(0);
	iTestLib->ReplaceFileL(buf);
	iTestLib->DeleteViews();
	iTestLib->OpenFileL(buf);
	}
	
void CCalEntryTestApp::CopyToCDriveL(const TDesC& aFileName)
	{
	iTestLib->DeleteViews();
	
	_LIT(KCalendarDirectory,"\\private\\10003a5b\\");
	
	TFileName originalLocation, newLocation;
	originalLocation.Copy(_L("z:"));
	originalLocation.Append(KCalendarDirectory);
	originalLocation.Append(aFileName);
	newLocation.Copy(_L("c:"));
	newLocation.Append(KCalendarDirectory);
	newLocation.Append(aFileName);
	TRAPD(err, iTestLib->DeleteFileL(aFileName));
	if (err == KErrInUse)
		{
		OpenBlankFileL();
		iTestLib->DeleteFileL(aFileName);
		}
	TRAP(err, iTestLib->PIMTestServer().CopyFileL(originalLocation, newLocation));
	if (err != KErrNone)
		{
		test.Printf(_L("Failed to copy from %S to %S, err=%d\n"), &originalLocation, &newLocation, err);
		}
	}

#ifndef __REGENERATE_DC_FILE__
static void DoOpenCalendarFileL()
	{
	TInt err = KErrNotFound;
	while (err != KErrNone)
		{
		CCalTestLibrary* testLib = CCalTestLibrary::NewLC(ETrue);
		testLib->OpenFileL(KCalendar91File);
		TRAP(err, testLib->SynCGetEntryViewL());
		test(err == KErrInUse || err == KErrNone);
		CleanupStack::PopAndDestroy(testLib);
		}	
	}

static TInt OpenCalendarFile(TAny*)
	{
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

	TRAPD(err, DoOpenCalendarFileL());

	delete scheduler;
	delete trapCleanup;

	return err;
	}

void CCalEntryTestApp::TestOpeningFileDuringConversionL()
	{
	test.Printf(_L("Loading v9.1 calendar file in two threads at once...\n"));
	CopyToCDriveL(KCalendar91File);
	CalCommon::TCalFileVersionSupport status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->GetSession().OpenL(KCalendar91File, status);
	test(status == CalCommon::EFileNeedsConverting);
	iTestLib->AsynCGetEntryViewL();

	//
	const TInt KMaxHeapSize = 0x20000;
	RThread otherThread;
	TName threadName = _L("TestOpenCalendarThread");
	TInt err = otherThread.Create(threadName, OpenCalendarFile, KDefaultStackSize, 
							      KMinHeapSize, KMaxHeapSize, NULL, EOwnerProcess);
	if (err != KErrNone)
		{
		User::Panic(_L("Thread's not been created"), err);
		}

	TRequestStatus threadStatus;
	otherThread.Logon(threadStatus);
	otherThread.Resume(); 
	//wait until the other thread is dead?
	User::WaitForRequest(threadStatus);
	test(threadStatus.Int() == KErrNone);
	
	CActiveScheduler::Start();
	}
#endif

void CCalEntryTestApp::DoTestsL()
	{
	iTestLib = CCalTestLibrary::NewL(ETrue);

#ifdef __REGENERATE_DC_FILE__

	iTestLib->ReplaceFileL(KCalendarFileToGenerate());
	iTestLib->OpenFileL(KCalendarFileToGenerate());
	CreateDCFileL();
	
#else

	CalCommon::TCalFileVersionSupport status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	
	CopyToCDriveL(KCalendar81File);
	iTestLib->RegisterCalFileL(KCalendar81File);
	test.Printf(_L("Loading v8.1 calendar file...\n"));
	TRAPD(err, iTestLib->GetSession().OpenL(KCalendar81File, status));
	test(err == KErrNotSupported);
	test(status == CalCommon::EUnsupportedFileVersion);
	
	CopyToCDriveL(KCalendar91File);
	iTestLib->RegisterCalFileL(KCalendar91File);
	test.Printf(_L("Loading v9.1 calendar file...\n"));
	status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->GetSession().OpenL(KCalendar91File, status);
	test(status == CalCommon::EFileNeedsConverting);
	TestFileLoadedL(EFalse);
	
	CopyToCDriveL(KCalendar92File);
	iTestLib->RegisterCalFileL(KCalendar92File);
	test.Printf(_L("Loading v9.2 calendar file...\n"));
	status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->GetSession().OpenL(KCalendar92File, status);
	test(status == CalCommon::EFileNeedsConverting);
	TestFileLoadedL(ETrue);

	CopyToCDriveL(KCalendar94File);
	iTestLib->RegisterCalFileL(KCalendar94File);
	test.Printf(_L("Loading v9.4 compacted calendar file...\n"));
	status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->GetSession().OpenL(KCalendar94File, status);
	test(status == CalCommon::EFileNeedsConverting);
	TestFileLoadedV94L();
	
	CopyToCDriveL(KCalendar95File);
	iTestLib->RegisterCalFileL(KCalendar95File);
	test.Printf(_L("Loading v9.5 calendar file...\n"));
	status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->GetSession().OpenL(KCalendar95File, status);
	test(status == CalCommon::EFileIsCurrentVersion);
	TestFileLoadedV95L();
	
	test.Printf(_L("Loading v9.2 calendar file from read-only drive...\n"));
	TFileName fileOnZDrive;
	fileOnZDrive.Copy(_L("z:"));
	fileOnZDrive.Append(KCalendar92File);
	status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->DeleteViews();
	TRAP(err, iTestLib->GetSession().OpenL(fileOnZDrive, status));
	test(err == KErrAccessDenied);
	test(status == CalCommon::EFileNeedsConverting);
	
	test.Printf(_L("Loading converted 9.2 calendar file...\n"));
	status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->DeleteViews();
	iTestLib->GetSession().OpenL(KCalendar92File, status);
	test(status == CalCommon::EFileIsCurrentVersion);
	TestFileLoadedL(ETrue);

	test.Printf(_L("Loading converted 9.4 calendar file ...\n"));
	status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->DeleteViews();
	iTestLib->GetSession().OpenL(KCalendar94File, status);
	test(status == CalCommon::EFileIsCurrentVersion);
	TestFileLoadedV94L();
	
	test.Printf(_L("Loading large v9.1 calendar file...\n"));
	CopyToCDriveL(KCalendar91LargeFile);
	iTestLib->RegisterCalFileL(KCalendar91LargeFile);
	TTime start, end;
	start.UniversalTime();
	status = static_cast<CalCommon::TCalFileVersionSupport>(-1);
	iTestLib->GetSession().OpenL(KCalendar91LargeFile, status);
	test(status == CalCommon::EFileNeedsConverting);
	iTestLib->SynCGetEntryViewL(); // build indexes
	end.UniversalTime();
	TTimeIntervalMicroSeconds microseconds = end.MicroSecondsFrom(start);
	TInt32 milliseconds = microseconds.Int64() / 1000;
	test.Printf(_L("Time to convert 1200 entries from v9.1 to v9.3: %d milliseconds\n"), milliseconds);

	CopyToCDriveL(KCalendar92CorruptFile);
	iTestLib->RegisterCalFileL(KCalendar92CorruptFile);
	test.Printf(_L("Loading corrupt v9.2 calendar file...\n"));
	iTestLib->GetSession().OpenL(KCalendar92CorruptFile, status);
	test(status == CalCommon::EFileNeedsConverting);
	TRAP(err, iTestLib->SynCGetEntryViewL()); // build indexes
	test(err == KErrCorrupt);
	
	TestCategoriesL(KCalendar91File);
	TestCategoriesL(KCalendar92File);
	
	TestOpeningFileDuringConversionL();
	
	OpenBlankFileL();
	// Old v9.2 tests:
	
	// 9.2 Calendar File - Clean / No Entries
	V92Test92CalendarFileCleanL();

	// 9.2 Calendar File - Previously Added Entries
	V92Test92CalendarFileEntriesAddedL();

	// 9.1 Calendar File - Clean / No Entries
	V92Test91CalendarFileCleanL();

	// 9.1 Calendar File - Previously Added Entries
	V92Test91CalendarFileEntriesAddedL();

	//there are still converted files left
	//manual delete of them
	TRAP_IGNORE(iTestLib->DeleteFileL( _L("tcal_dc_91"), 1) );
	TRAP_IGNORE(iTestLib->DeleteFileL( _L("tcal_dc_91_calfile_clean_test_converted"), 1) );
	TRAP_IGNORE(iTestLib->DeleteFileL( _L("tcal_dc_91Idx"), 1) );
	TRAP_IGNORE(iTestLib->DeleteFileL( _L("tcal_dc_92_calfile_clean_test_converted"), 1) );
	TRAP_IGNORE(iTestLib->DeleteFileL( _L("tcal_dc_92_corrupt_converted"), 1) ); // will not be deleted because in use
	
#endif
	}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
	
static void DoTestL()
	{
	CCalEntryTestApp* testManager = new (ELeave) CCalEntryTestApp;
	CleanupStack::PushL(testManager);


	TPerformanceTimer timer(test);
	timer.Start();


	// Run the test suite

	testManager->DoTestsL();
	
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();

	
	CleanupStack::PopAndDestroy(testManager);
	}

void MyCleanL(void)
	{
	CCalTestLibrary	*pTestLib = CCalTestLibrary::NewLC(ETrue);
	pTestLib->DeleteFileL(_L("tcal_dc_92_corrupt_converted"), ETrue);
	CleanupStack::PopAndDestroy(pTestLib);
	}

/**

@SYMTestCaseID     PIM-TCAL-DC-0001

*/

TInt E32Main()
    {
	__UHEAP_MARK;

	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-DC-0001 Calendar Interim API Data Conversion test suite"));

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
	
	TRAP_IGNORE(MyCleanL());

	test.End();
	test.Close();

	delete scheduler;
	delete trapCleanup;	
	
	__UHEAP_MARKEND;

	return (KErrNone);
    }
