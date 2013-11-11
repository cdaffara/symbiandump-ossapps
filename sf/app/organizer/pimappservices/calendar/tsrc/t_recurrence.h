// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <f32file.h>
#include <s32file.h>

#include <calentry.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calrrule.h>

#include "caltestlib.h"

const TInt KMonthlyByDateCount1(20);
_LIT8(KMonthlyByDate1,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Monthly repeat by date every 1 month\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MD1 11+ 16+ #10\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");

const TInt KMonthlyByDateCount2(20);
_LIT8(KMonthlyByDate2,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Monthly repeat by date every 2 months\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MD2 11+ 16+ #10\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KMonthlyByDateCount3(20);
_LIT8(KMonthlyByDate3,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Monthly repeat by date evert 3 months\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MD3 11+ 16+ #10\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");

const TInt KMonthlyByDates31Count(5);
_LIT8(KMonthlyByDates31,	"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Monthly repeat by date on last day of month\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:20030131T090000Z\r\n"
							"DTEND:20030131T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MD2 31+ #5\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");

const TInt KMonthlyByDaysCount1(20);
_LIT8(KMonthlyByDays1,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Repeat Monthly by Days every 1 month\r\n"
							"DESCRIPTION:Days\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MP1 2+ TU 1- TU #10\r\n"
							"LAST-MODIFIED:20030213T131600Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KMonthlyByDaysCount2(20);
_LIT8(KMonthlyByDays2,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Repeat Monthly by Days every 2 months\r\n"
							"DESCRIPTION:Days\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MP2 2+ TU 1- TU #10\r\n"
							"LAST-MODIFIED:20030213T131600Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KMonthlyByDaysCount3(20);
_LIT8(KMonthlyByDays3,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Repeat Monthly by Days every 3 months\r\n"
							"DESCRIPTION:Days\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MP3 2+ TU 1- TU #10\r\n"
							"LAST-MODIFIED:20030213T131600Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");

const TInt KWeeklyCount(20);
_LIT8(KWeekly,				"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Repeat every Tu, Thur\r\n"
							"DESCRIPTION:Other week x 10 times\r\n"
							"DTSTART:20030128T090000Z\r\n"
							"DTEND:20030128T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030128T000000Z\r\n"
							"RRULE:W2 TU TH #10\r\n"
							"LAST-MODIFIED:20030128T160400Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
						
const TInt KYearlyByDateCount1(10);		
_LIT8(KYearlyByDate1,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Yearly by date\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:YM1 2 #10\r\n"
							"LAST-MODIFIED:20030213T132200Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KYearlyByDateCount2(10);		
_LIT8(KYearlyByDate2,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Yearly by date\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:YM2 2 #10\r\n"
							"LAST-MODIFIED:20030213T132200Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KYearlyByDateCount3(10);		
_LIT8(KYearlyByDate3,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Yearly by date\r\n"
							"DTSTART:20030211T090000Z\r\n"
							"DTEND:20030211T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:YM3 2 #10\r\n"
							"LAST-MODIFIED:20030213T132200Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KMonthlyByDateOverCount1(0);
const TTime KMonthlyByDateOverEndDay1(TDateTime(2100,ENovember,29,9,0,0,0));
_LIT8(KMonthlyByDateOver1,	"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Monthly spanning over end date\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:21000531T090000\r\n"
							"DTEND:21000531T100000\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MD3 31+ #5\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KMonthlyByDateOverCount2(0);
_LIT8(KMonthlyByDateOver2,	"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Monthly over end date with only 1 instance\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:21000531T090000Z\r\n"
							"DTEND:21000531T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:MD18 31+ #5\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");


const TInt KYearlyByDateOverCount1(0);
_LIT8(KYearlyByDateOver1,	"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Yearly spanning over max date with only 1 instance\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:21000531T090000Z\r\n"
							"DTEND:21000531T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:YM1 5 #5\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KYearlyByDateOverCount2(0);
_LIT8(KYearlyByDateOver2,	"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Yearly spanning over max date\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:20990531T090000Z\r\n"
							"DTEND:20990531T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:YM1 #5\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KDailyOverCount1(0);
_LIT8(KDailyOver1,			"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Daily with lots of instances\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:21000531T090000Z\r\n"
							"DTEND:21000531T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:D1 #356\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KDailyOverCount2(0);
_LIT8(KDailyOver2,			"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Daily repeat over max date with only one instance\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:21001231T090000Z\r\n"
							"DTEND:21001231T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:D1 #3\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");

							
const TInt KWeeklyOverCount1(0);
_LIT8(KWeeklyOver1,			"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Weekly that spans over max date\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:21001205T090000Z\r\n"
							"DTEND:21001205T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:W1 #52\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");
							
const TInt KWeeklyOverCount2(0);
_LIT8(KWeeklyOver2,			"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Weekly that spans over max date with only one instance\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:21001230T090000Z\r\n"
							"DTEND:21001230T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:W1 #52\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");

const TInt KAfterMaxDateCount(0);
_LIT8(KAfterMaxDate,		"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VEVENT\r\n"
							"SUMMARY:Starting after max date\r\n"
							"DESCRIPTION:by date\r\n"
							"DTSTART:22001230T090000Z\r\n"
							"DTEND:22001230T100000Z\r\n"
							"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
							"CLASS:PUBLIC\r\n"
							"DCREATED:20030213T000000Z\r\n"
							"RRULE:W1 #52\r\n"
							"LAST-MODIFIED:20030213T131500Z\r\n"
							"PRIORITY:0\r\n"
							"STATUS:NEEDS ACTION\r\n"
							"END:VEVENT\r\n"
							"END:VCALENDAR\r\n");

class CTestApp : public CBase
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void RunTestL(const TDesC8& aVCal, TInt aNumberOfOccurences, TTime aEndTime, TInt aNumExpectedEntries);
	
private:
	void ConstructL();
	
private:
	CCalTestLibrary* iCalTestLib;
	};
