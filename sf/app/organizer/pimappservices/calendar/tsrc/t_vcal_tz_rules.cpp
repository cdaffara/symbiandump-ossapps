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
// This test creates vCals from the text specified below, then imports this into a calendar entry.
// This entry is then exported, and re-imported, then that re-imported entry is compared with the 
// original imported entry to check they are the same.
// The entry comparison focuses on timezone settings and daylight saving rules.
// 
//

#include <e32test.h>
#include <s32file.h>
#include <e32test.h>
#include <ecom/ecom.h>
#include <badesca.h>
#include <coreappstest/testserver.h>
#include <vtzrules.h>

#include <caldataexchange.h>
#include <caldataformat.h>
#include <calcategory.h>
#include <calrrule.h>

#include "caltestlib.h"

LOCAL_D RTest test(_L("t_vcal_tz_rules"));

_LIT(KTestCalendarFile,"c:t_vcal_tz_rules");

_LIT(KEntryImportFile,"t_vcal_tz_rules_import%d.vcs");
_LIT(KEntryExportFile,"t_vcal_tz_rules_export%d.vcs");
_LIT(KExpectedEntryFile,"t_vcal_tz_rules_expected%d.vcs");

_LIT(KEntryExportTimeZoneAFile,"t_vcal_tz_rules_export_tzA%d.vcs");
_LIT(KEntryExportTimeZoneBFile,"t_vcal_tz_rules_export_tzB%d.vcs");

//
//	HEMISPHERE: Northern
//	DST: 		Single
//	START DATE: Past, Winter time
//
_LIT8(KEntry1,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
				"DAYLIGHT:TRUE;+06;20000331T050000;20001028T000000;GMT;BST\r\n"                
               	"DAYLIGHT:TRUE;+06;20030331T050000;20031028T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040325T050000;20041028T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050330T050000;20051026T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20060329T050000;20061025T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20000111T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KExpectedEntry1,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20030331T050000;20031028T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20040325T050000;20041028T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050330T050000;20051026T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20060329T050000;20061025T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
   				"UID:9000i-131097-0\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"DTSTART:20000111T130000Z\r\n"
				"DTEND:20000111T130000Z\r\n"
				"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
				"CLASS:PUBLIC\r\n"
				"ATTENDEE;ROLE=ATTENDEE;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI;X-ROLE=REQ-PARTICIPANT;X-STATUS=NEEDS-ACTION:Example Person\r\n"
				"RRULE:W2 TU #0\r\n"
				"LAST-MODIFIED:20060103T030407Z\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"PRIORITY:0\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"X-SYMBIAN-LUID:0\r\n"
   				"END:VEVENT\r\n"
   				"END:VCALENDAR\r\n"
   				);

//
//	HEMISPHERE: Northern
//	DST: 		Single
//	START DATE: Past, Summer time
//   				
_LIT8(KEntry2,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
                "DAYLIGHT:TRUE;+06;20000331T050000;20001028T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050330T040000Z;20051029T002000Z;\r\n"
               	"DAYLIGHT:TRUE;+06;20060328T060000Z;20061027T003000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040327T030000;20041026T001000Z;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20030331T050000;20031028T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:EVENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20000711T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KExpectedEntry2,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
                "DAYLIGHT:TRUE;+06;20000331T050000;20001028T000000;;\r\n"
                "DAYLIGHT:TRUE;+06;20030331T050000;20031028T000000;;\r\n"
                "DAYLIGHT:TRUE;+06;20040327T030000;20041026T001000Z;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050330T040000Z;20051029T002000Z;\r\n"
               	"DAYLIGHT:TRUE;+06;20060328T060000Z;20061027T003000;;\r\n"
                "BEGIN:VEVENT\r\n"
   				"CATEGORIES:EVENT\r\n"
   				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20000711T120000Z\r\n"
   				"SUMMARY:repeat every other week\r\n"
   				"DESCRIPTION:repeat every other week\r\n"
   				"RRULE:W2 #0\r\n"
   				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
   				"UID:9000i-131097-0\r\n"
   				"END:VEVENT\r\n"
   				"END:VCALENDAR\r\n"
   				);

//
//	HEMISPHERE: Northern
//	DST: 		Single
//	START DATE: Past, No DST
//
_LIT8(KEntry3,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05:30\r\n"
               	"DAYLIGHT:TRUE;+06;20050330T040000Z;20051029T002000Z;\r\n"
               	"DAYLIGHT:TRUE;+06;20060328T060000Z;20061027T003000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040327T030000;20041026T001000Z;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20030331T050000;20031028T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:EVENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20000711T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KExpectedEntry3,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05:30\r\n"
                "DAYLIGHT:TRUE;+06;20030331T050000;20031028T000000;;\r\n"
                "DAYLIGHT:TRUE;+06;20040327T030000;20041026T001000Z;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050330T040000Z;20051029T002000Z;\r\n"
               	"DAYLIGHT:TRUE;+06;20060328T060000Z;20061027T003000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:EVENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20000711T123000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Northern
//	DST: 		Single
//	START DATE: Future, DST
//
_LIT8(KEntry4,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20050330T040000Z;20051029T002000Z;\r\n"
               	"DAYLIGHT:TRUE;+06;20060328T060000Z;20061027T003000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040327T030000;20041026T001000Z;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20030331T050000;20031028T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:EVENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040711T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KExpectedEntry4,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
                "DAYLIGHT:TRUE;+06;20040327T030000;20041026T001000Z;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050330T040000Z;20051029T002000Z;\r\n"
               	"DAYLIGHT:TRUE;+06;20060328T060000Z;20061027T003000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:EVENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040711T120000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Northern
//	DST: 		Double
//	START DATE: Future, Winter time
//
_LIT8(KEntry5,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05:30\r\n"
               	"DAYLIGHT:TRUE;+06;20030331T050000;20030628T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20030628T000000;20030828T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20030828T000000;20031028T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040331T050000;20040628T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20040628T000000;20040828T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040828T000000;20041028T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050331T050000;20050628T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20050628T000000;20050828T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050828T000000;20051028T000000;GMT;BST\r\n"
               	"BEGIN:VEVENT\r\n"
				"CATEGORIES:EVENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040111T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KExpectedEntry5,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05:30\r\n"
               	"DAYLIGHT:TRUE;+06;20040331T050000;20040628T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20040628T000000;20040828T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20040828T000000;20041028T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050331T050000;20050628T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20050628T000000;20050828T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050828T000000;20051028T000000;;\r\n"
               	"BEGIN:VEVENT\r\n"
				"CATEGORIES:EVENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040111T123000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Southern
//	DST: 		Single
//	START DATE: Future, Summer time
//
_LIT8(KEntry6,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20041025T050000;20050328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20051030T050000;20060326T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20061029T050000;20070325T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20071031T050000;20080328T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040111T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KExpectedEntry6,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20041025T050000;20050328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20051030T050000;20060326T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20061029T050000;20070325T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20071031T050000;20080328T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040111T120000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Southern
//	DST: 		Single
//	START DATE: Past, Summer time
//
_LIT8(KEntry7,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20041025T050000;20050328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20051030T050000;20060326T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20061029T050000;20070325T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20010111T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KExpectedEntry7,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20041025T050000;20050328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20051030T050000;20060326T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20061029T050000;20070325T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20010111T120000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Southern
//	DST: 		Single
//	START DATE: Past, Winter time
//
_LIT8(KEntry8,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20041025T050000;20050328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20051030T050000;20060326T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20061029T050000;20070325T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20010411T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KExpectedEntry8,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
				"DAYLIGHT:TRUE;+06;20001031T050000;20010328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20041025T050000;20050328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20051030T050000;20060326T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20061029T050000;20070325T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20010411T130000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Southern
//	DST: 		Single
//	START DATE: Future, Summer time
//
_LIT8(KEntry9,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20041025T050000;20050328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20051030T050000;20060326T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20061029T050000;20070325T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040111T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
   				
_LIT8(KExpectedEntry9,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20041025T050000;20050328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20051030T050000;20060326T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20061029T050000;20070325T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040111T120000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Southern
//	DST: 		Double
//	START DATE: Past, Summer time
//
_LIT8(KEntry10,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20010128T000000;20010228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20010228T000000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;GMT;BST\r\n"
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20010111T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KExpectedEntry10,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
                "DAYLIGHT:TRUE;+06;20001031T050000;20010128T000000;;\r\n"
				"DAYLIGHT:TRUE;+07;20010128T000000;20010228T000000;;\r\n"
				"DAYLIGHT:TRUE;+06;20010228T000000;20010328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;;\r\n"
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20010111T120000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
//
//	HEMISPHERE: Southern
//	DST: 		Double
//	START DATE: Past, Double Summer time
//
_LIT8(KEntry11,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20010128T000000;20010228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20010228T000000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;GMT;BST\r\n"
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20010211T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KExpectedEntry11,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
				"DAYLIGHT:TRUE;+06;20001031T050000;20010128T000000;;\r\n"
                "DAYLIGHT:TRUE;+07;20010128T000000;20010228T000000;;\r\n"
				"DAYLIGHT:TRUE;+06;20010228T000000;20010328T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;;\r\n"
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20010211T110000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Southern
//	DST: 		Double
//	START DATE: Future, Winter time
//
_LIT8(KEntry12,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20010128T000000;20010228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20010228T000000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;GMT;BST\r\n"
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040511T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KExpectedEntry12,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
             	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;;\r\n"     
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040511T130000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Southern
//	DST: 		Double
//	START DATE: Future, Summer time
//

_LIT8(KEntry13,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20010128T000000;20010228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20010228T000000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;GMT;BST\r\n"
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040311T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KExpectedEntry13,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
             	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;;\r\n"     
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040311T120000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

//
//	HEMISPHERE: Southern
//	DST: 		Double
//	START DATE: Future, Double Summer time
//

_LIT8(KEntry14,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
                "TZ:+05\r\n"
               	"DAYLIGHT:TRUE;+06;20001031T050000;20010128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20010128T000000;20010228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20010228T000000;20010328T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;GMT;BST\r\n"
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;GMT;BST\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;GMT;BST\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040211T180000\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KExpectedEntry14,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
	             "TZ:+05\r\n"
	          	"DAYLIGHT:TRUE;+06;20031031T050000;20040128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20040128T000000;20040228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20040228T000000;20040328T000000;;\r\n"     
              	"DAYLIGHT:TRUE;+06;20041031T050000;20050128T000000;;\r\n"
               	"DAYLIGHT:TRUE;+07;20050128T000000;20050228T000000;;\r\n"
               	"DAYLIGHT:TRUE;+06;20050228T000000;20050328T000000;;\r\n"
                "BEGIN:VEVENT\r\n"
				"CATEGORIES:APPOINTMENT\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"DTSTART:20040211T110000Z\r\n"
				"SUMMARY:repeat every other week\r\n"
				"DESCRIPTION:repeat every other week\r\n"
				"RRULE:W2 #0\r\n"
				"ATTENDEE;ROLE=ATTENDEE:Example Person\r\n"
				"UID:9000i-131097-0\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

				
_LIT(KCategoryNameA,"Meeting 1");
_LIT(KCategoryNameSameAsA,"Meeting 1");

class CTestApp : public CBase
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void SetLocalTimeL(const TDesC8& aTimeZone);
	void CreateTestFileL(const TDesC8& aVCalData, const TDesC& aFileName);
	void ImportL(const TDesC& aImportVCalendarFile, RPointerArray<CCalEntry>& aEntriesImported);
	void ExportL(const TDesC& aExportVCalendarFile, RPointerArray<CCalEntry>& aEntriesToExport);
	void CompareEntriesL(RPointerArray<CCalEntry>& aEntries1, const RPointerArray<CCalEntry>& aEntries2);

private:
	void ConstructL();

private:
	CCalTestLibrary* iCalTestLib;
	CCalDataExchange* iCalDataExchange;

	};
	
CTestApp* CTestApp::NewL()
	{
	CTestApp* self = new(ELeave) CTestApp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestApp::ConstructL()
	{
	iCalTestLib = CCalTestLibrary::NewL();
	iCalTestLib->ReplaceFileL(KTestCalendarFile);
	iCalTestLib->OpenFileL(KTestCalendarFile);
	iCalDataExchange = CCalDataExchange::NewL(iCalTestLib->GetSession());
	}
		
CTestApp::~CTestApp()
	{
	delete iCalDataExchange;
	delete iCalTestLib;
	}
		
void CTestApp::SetLocalTimeL(const TDesC8& aTimeZone)
	{
	TBuf<64> buf;
	buf.Copy(aTimeZone);
	test.Printf(_L("Setting time zone to %S"), &buf);
	iCalTestLib->SetTimeZoneL(aTimeZone);
	}

void CTestApp::CreateTestFileL(const TDesC8& aVCalData, const TDesC& aFileName)
	{
	RFile file;
	iCalTestLib->TestRegister().CreateTempFileLC(file, aFileName);
	User::LeaveIfError(file.Write(aVCalData));
	CleanupStack::PopAndDestroy(&file);
	}
	
void CTestApp::ImportL(const TDesC& aImportVCalendarFile, RPointerArray<CCalEntry>& aEntriesImported)
	{	
	test.Printf(_L("Importing entry from %S"), &aImportVCalendarFile);
	
	RFile infile;
	iCalTestLib->TestRegister().OpenTempFileLC(infile, aImportVCalendarFile);
	
	RFileReadStream readStream(infile);
	CleanupClosePushL(readStream);
	
	iCalDataExchange->ImportL(KUidVCalendar, readStream, aEntriesImported);

	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&infile);
	}

void CTestApp::ExportL(const TDesC& aExportVCalendarFile, RPointerArray<CCalEntry>& aEntriesToExport)
	{
	test.Printf(_L("Exporting entry to %S"), &aExportVCalendarFile);
	
	RFile outfile;
	iCalTestLib->TestRegister().CreateTempFileLC(outfile, aExportVCalendarFile);

	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	
	iCalDataExchange->ExportL(KUidVCalendar, writeStream, aEntriesToExport);

	writeStream.CommitL();

	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(&outfile);
	}

void CTestApp::CompareEntriesL(RPointerArray<CCalEntry>& aEntries1, const RPointerArray<CCalEntry>& aEntries2)
	{
	test.Printf(_L("Comparing entries"));
	
	const TInt KNumEntries(aEntries1.Count());
	test(KNumEntries == aEntries2.Count());
	
	for (TInt i(0) ; i < KNumEntries ; ++i)
		{
		CCalEntry* entry1 = aEntries1[i];
		CCalEntry* entry2 = aEntries2[i];
	
		test(entry1->CompareL(*entry2));	
		
		CCalCategory* educationCategory = CCalCategory::NewL(CCalCategory::ECalEducation);		
		CleanupStack::PushL(educationCategory);
		CCalCategory* meetingCategory = CCalCategory::NewL(CCalCategory::ECalMeeting);		
		CleanupStack::PushL(meetingCategory);
		CCalCategory* personalCategory = CCalCategory::NewL(CCalCategory::ECalPersonal);		
		CleanupStack::PushL(personalCategory);
		
		CCalCategory* educationCategory2 = CCalCategory::NewL(CCalCategory::ECalEducation);		
		CleanupStack::PushL(educationCategory2);
		CCalCategory* meetingCategory2 = CCalCategory::NewL(CCalCategory::ECalMeeting);		
		CleanupStack::PushL(meetingCategory2);
		CCalCategory* personalCategory2 = CCalCategory::NewL(CCalCategory::ECalPersonal);		
		CleanupStack::PushL(personalCategory2);	
		
		entry1->AddCategoryL(educationCategory);
		entry1->AddCategoryL(meetingCategory);
		entry1->AddCategoryL(personalCategory);
		entry2->AddCategoryL(personalCategory2);
		entry2->AddCategoryL(meetingCategory2);
	
		//	entry2 is short a category : Education
		test(!entry1->CompareL(*entry2));				
	
		entry2->AddCategoryL(educationCategory2);
		
		//	this tests to ensure that compare can compare unsorted entries
		test(entry1->CompareL(*entry2));	

		CleanupStack::Pop(6);	
		
		CCalCategory* extendedCategoryA = CCalCategory::NewL(KCategoryNameA);		
		CleanupStack::PushL(extendedCategoryA);		
		CCalCategory* extendedCategorySameAsA = CCalCategory::NewL(KCategoryNameSameAsA);		
		CleanupStack::PushL(extendedCategorySameAsA);	
		
		//	test extended categories
		entry1->AddCategoryL(extendedCategoryA);
		entry2->AddCategoryL(extendedCategorySameAsA);		

		test(entry1->CompareL(*entry2));	

		CleanupStack::Pop(2);
		
		TCalRRule rRule1;
		TCalRRule rRule2;
		
		if ( entry1->GetRRuleL(rRule1) )
			{
			CTzRules* rules1 = entry1->TzRulesL();
			CTzRules* rules2 = entry2->TzRulesL();
			if (rules1 && rules2)
				{
				test(rules1->Count() == rules2->Count());
				for (TInt i = 0; i < rules1->Count(); i++)
					{
					TTzRule rule1 = (*rules1)[i];
					TTzRule rule2 = (*rules2)[i];
					test(rule1.iTo.iTime.DateTime().Year() == rule2.iTo.iTime.DateTime().Year());
					test(rule1.iFrom.iTime.DateTime().Year() == rule2.iFrom.iTime.DateTime().Year());
					test(rule1.iOldLocalTimeOffset == rule2.iOldLocalTimeOffset);
					test(rule1.iNewLocalTimeOffset == rule2.iNewLocalTimeOffset);
					test(rule1.iMonth == rule2.iMonth);
					test(rule1.iDayRule == rule2.iDayRule);
					test(rule1.iDayOfMonth == rule2.iDayOfMonth);
					test(rule1.iDayOfWeek == rule2.iDayOfWeek);
					test(rule1.iTimeReference == rule2.iTimeReference);
					test(rule1.iTimeOfChange == rule2.iTimeOfChange);
					}
				}
			else
				{
				test(rules1 == rules2);
				}
			delete rules1;
			delete rules2;
			}
		else
			{
			test(!entry2->GetRRuleL(rRule2));
			}
		}
	}




static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);
	
	// Connect to the TZ server and set auto update to on, as needed
	// by the DST tests. ETZAutoDSTUpdateOn enables automatic DST update
	RTz tz;
	CleanupClosePushL(tz);
	User::LeaveIfError(tz.Connect());
	tz.SetAutoUpdateBehaviorL(RTz::ETZAutoDSTUpdateOn);
	
	//set date to constant value to achieve consistent results
	TDateTime dtime(2003,EJanuary,2,3,4,5,6);
	TTime newtime(dtime);
	TTime oldtime; 
	oldtime.HomeTime();
	
	TInt err = tz.SetHomeTime(newtime);
	User::LeaveIfError( err );
	
	//specify the timezone to achieve consistent results
	testApp->SetLocalTimeL(_L8("Europe/London"));
		
	CDesC8Array* bufArray = new(ELeave) CDesC8ArraySeg(4);
	CleanupStack::PushL(bufArray);
	
	bufArray->AppendL(KEntry1);
	bufArray->AppendL(KEntry2);
	bufArray->AppendL(KEntry3);
	bufArray->AppendL(KEntry4);
	bufArray->AppendL(KEntry5);
	bufArray->AppendL(KEntry6);
	bufArray->AppendL(KEntry7);
	bufArray->AppendL(KEntry8);
	bufArray->AppendL(KEntry9);
	bufArray->AppendL(KEntry10);
	bufArray->AppendL(KEntry11);
	bufArray->AppendL(KEntry12);
	bufArray->AppendL(KEntry13);
	bufArray->AppendL(KEntry14);

	CDesC8Array* expectedBufArray = new(ELeave) CDesC8ArraySeg(4);

	CleanupStack::PushL(expectedBufArray);

	expectedBufArray->AppendL(KExpectedEntry1);	
	expectedBufArray->AppendL(KExpectedEntry2);
	expectedBufArray->AppendL(KExpectedEntry3);
	expectedBufArray->AppendL(KExpectedEntry4);
	expectedBufArray->AppendL(KExpectedEntry5);
	expectedBufArray->AppendL(KExpectedEntry6);
	expectedBufArray->AppendL(KExpectedEntry7);
	expectedBufArray->AppendL(KExpectedEntry8);
	expectedBufArray->AppendL(KExpectedEntry9);
	expectedBufArray->AppendL(KExpectedEntry10);
	expectedBufArray->AppendL(KExpectedEntry11);
	expectedBufArray->AppendL(KExpectedEntry12);
	expectedBufArray->AppendL(KExpectedEntry13);
	expectedBufArray->AppendL(KExpectedEntry14);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	RPointerArray<CCalEntry> entryArray2;
	CleanupResetAndDestroyPushL(entryArray2);

	RPointerArray<CCalEntry> entryArrayExpected;
	CleanupResetAndDestroyPushL(entryArrayExpected);
		

	TBuf<64> impFileName;				// t_vcal_tz_rules_import#n.vcs
	TBuf<64> expFileName;				// t_vcal_tz_rules_export#n.vcs
	TBuf<64> expectedFileName;			// t_vcal_tz_rules_expected#n.vcs

 	test.Next(_L("Importing entries, exporting them, re-importing them. Importing the expected entries and comparing the two sets of imported entries"));

	const TInt KBufArrayCount(bufArray->Count());
	
	for (TInt i(0) ; i < KBufArrayCount ; ++i)
		{
		impFileName.Format(KEntryImportFile(), i+1);
		expFileName.Format(KEntryExportFile(), i+1);
		expectedFileName.Format(KExpectedEntryFile(), i+1);
		testApp->CreateTestFileL((*bufArray)[i], impFileName);
		testApp->ImportL(impFileName, entryArray);
		testApp->ExportL(expFileName, entryArray);
		testApp->ImportL(expFileName, entryArray2);
		testApp->CreateTestFileL((*expectedBufArray)[i], expectedFileName);
		testApp->ImportL(expectedFileName, entryArrayExpected);
		testApp->CompareEntriesL(entryArray2, entryArrayExpected);
		entryArray.ResetAndDestroy();
		entryArray2.ResetAndDestroy();
		entryArrayExpected.ResetAndDestroy();
		}
	
	test.Next(_L("Importing entries, exporting them, then re-importing them in a different time zone. Importing the expected data and comparing the two sets of imported entries"));

	// do same tests but export in one time zone and import in another
	for (TInt i(0); i < KBufArrayCount; ++i)
		{
		impFileName.Format(KEntryImportFile(), i+1);
		expFileName.Format(KEntryExportTimeZoneAFile(), i+1);
		expectedFileName.Format(KExpectedEntryFile(), i+1);
		testApp->CreateTestFileL((*bufArray)[i], impFileName);
		testApp->SetLocalTimeL(_L8("Asia/Tokyo"));
		testApp->ImportL(impFileName, entryArray);
		testApp->ExportL(expFileName, entryArray);
		testApp->SetLocalTimeL(_L8("Europe/Paris"));
		testApp->ImportL(expFileName, entryArray2);
		testApp->CreateTestFileL((*expectedBufArray)[i], expectedFileName);
		testApp->ImportL(expectedFileName, entryArrayExpected);
		testApp->CompareEntriesL(entryArray2, entryArrayExpected);
		entryArray.ResetAndDestroy();
		entryArray2.ResetAndDestroy();
		entryArrayExpected.ResetAndDestroy();
		}	
	test.Next(_L("Importing entries, then export and re-import them in a different time zone. Importing the expected data and comparing the two sets of imported entries"));

	// do same tests but import in one time zone and export in another
	for (TInt i(0); i < KBufArrayCount; ++i)
		{
		impFileName.Format(KEntryImportFile(), i+1);
		expFileName.Format(KEntryExportTimeZoneBFile(), i+1);
		expectedFileName.Format(KExpectedEntryFile(), i+1);
		testApp->CreateTestFileL((*bufArray)[i], impFileName);
		testApp->SetLocalTimeL(_L8("Asia/Tokyo"));
		testApp->ImportL(impFileName, entryArray);
		testApp->SetLocalTimeL(_L8("Europe/Paris"));
		testApp->ExportL(expFileName, entryArray);
		testApp->ImportL(expFileName, entryArray2);
		testApp->CreateTestFileL((*expectedBufArray)[i], expectedFileName);
		testApp->ImportL(expectedFileName, entryArrayExpected);
		testApp->CompareEntriesL(entryArray2, entryArrayExpected);
		entryArray.ResetAndDestroy();
		entryArray2.ResetAndDestroy();
		entryArrayExpected.ResetAndDestroy();
		}	
	CleanupStack::PopAndDestroy(&entryArrayExpected); 
	CleanupStack::PopAndDestroy(&entryArray2); 
	CleanupStack::PopAndDestroy(&entryArray); 
	CleanupStack::PopAndDestroy(expectedBufArray);
	CleanupStack::PopAndDestroy(bufArray);
	CleanupStack::PopAndDestroy(&tz);
	CleanupStack::PopAndDestroy(testApp);
	}


/**

@SYMTestCaseID     PIM-T-VCAL-TZ-RULES-0001

*/

TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-VCAL-TZ-RULES-0001 t_vcal_tz_rules"));

	test.Title();
	CActiveScheduler* scheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	delete theCleanup;
	delete scheduler;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

