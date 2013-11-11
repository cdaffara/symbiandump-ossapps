#ifndef AGNVERSIT2TESTCONSTANTS_H
#define AGNVERSIT2TESTCONSTANTS_H/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Constants used by the AgnVersit2 Tests
*
*/



// iCalendar MIME type
_LIT8(KICalMimeType, "text/calendar");

// Test Server name
_LIT(KServerName, "AgnVersit2TestServer");

// Empty ICS file needed when OOM testing
_LIT(KEmptyImportIcs, "E://testing//data//unittest//agnversit2//input//empty.ics");
_LIT(KEmptyExportIcs, "E://testing//data//unittest//agnversit2//output//empty.ics");

// Test step names
_LIT(KVersit2TestStepName, "Versit2Test");
_LIT(KAgnVersit2TestStepName, "AgnVersit2TestStep");
_LIT(KImportExportStepName, "ImportExportTest");

// Tokens used in the ini file - used by all test steps
_LIT(KImportToken, "ImportFileName");
_LIT(KExportToken, "ExportFileName");
_LIT(KCheckToken, "CheckFileName");
_LIT(KSettings, "SETTINGS");
_LIT8(KInputDir, "InputDirectory");
_LIT8(KOutputDir, "OutputDirectory");
_LIT(KOomTest,"OOMTest");
_LIT(KAgnVersit2TestPanic,"Memory Leak");
_LIT(KOnError, "OnError");

// Fixed buffer sizes
const TInt KICalMaxContentLineLength = 100;
const TInt KFileNameLength = 75;

// Testing
_LIT8(KSpace2, "= ");
_LIT8(KEndofLine, "\r\n");
_LIT8(KError, "OnError");

// Files
_LIT8(KImportFileName, "ImportFileName");
_LIT8(KExportFileName1, "ExportFileName1");
_LIT8(KExportFileName2, "ExportFileName2");
_LIT8(KCheckFileName, "CheckFileName");

// Test cases for Versit2
_LIT8(KVersit2PropertyValidation, "VERSIT2_FILTER_PROPS");
_LIT8(KVersit2EmbeddingValidation, "VERSIT2_FILTER_COMPS");
_LIT8(KVersit2EscapeSequence, "VERSIT2_ESCAPE_SEQ");
_LIT8(KVersit2VJournal, "VERSIT2_VJOURNAL");
_LIT8(KVersit2VFreeBusy, "VERSIT2_VFREEBUSY");
_LIT8(KVersit2Corrupt1, "VERSIT2_CORRUPT1");
_LIT8(KVersit2Corrupt2, "VERSIT2_CORRUPT2");
_LIT8(KVersit2Corrupt3, "VERSIT2_CORRUPT3");
_LIT8(KVersit2NotCorrupt, "VERSIT2_NOT_CORRUPT");

// Test cases for AgnVesit2
_LIT8(KAgnVersit2SVEvent, "SIMPLE_VEVENT");
_LIT8(KAgnVersit2SVEventAttendee, "SIMPLE_VEVENT_ATTENDEE");
_LIT8(KAgnVersit2SVEventCategories, "SIMPLE_VEVENT_CATEGORIES");
_LIT8(KAgnVersit2SVEventDescription, "SIMPLE_VEVENT_DESCRIPTION");
_LIT8(KAgnVersit2SVEventDuration, "SIMPLE_VEVENT_DURATION");
_LIT8(KAgnVersit2SVEventExDate, "SIMPLE_VEVENT_EXDATE");
_LIT8(KAgnVersit2SVEventLastMod, "SIMPLE_VEVENT_LASTMOD");
_LIT8(KAgnVersit2SVEventLocation, "SIMPLE_VEVENT_LOCATION");
_LIT8(KAgnVersit2SVEventOrganizer, "SIMPLE_VEVENT_ORGANIZER");
_LIT8(KAgnVersit2SVEventRDate, "SIMPLE_VEVENT_RDATE");
_LIT8(KAgnVersit2SVEventRecurId, "SIMPLE_VEVENT_RECURID");
_LIT8(KAgnVersit2SVEventRRule, "SIMPLE_VEVENT_RRULE");
_LIT8(KAgnVersit2SVEventSequence, "SIMPLE_VEVENT_SEQUENCE");
_LIT8(KAgnVersit2SVEventStatus, "SIMPLE_VEVENT_STATUS");
_LIT8(KAgnVersit2SVEventSummary, "SIMPLE_VEVENT_SUMMARY");
_LIT8(KAgnVersit2SVEventRRuleTz, "VEVENT_RRULE_TZ");
_LIT8(KAgnVersit2OutlookAllDayRepeatWeekly, "OUTLOOK_ALLDAY_REPEAT_WEEKLY");
_LIT8(KAgnVersit2NotSupportedContinue, "NOTSUPPORTED_CONTINUE");
_LIT8(KAgnVersit2NotSupportedSkip, "NOTSUPPORTED_SKIP");
_LIT8(KAgnVersit2NotSupportedStop, "NOTSUPPORTED_STOP");
_LIT8(KAgnVersit2NotSupportedLeave, "NOTSUPPORTED_LEAVE");
_LIT8(KAgnVersit2NotSupportedDie, "NOTSUPPORTED_DIE");
_LIT8(KAgnVersit2RRuleNumberingWeekly, "RRULE_NUMBERING_WEEKLY");
_LIT8(KAgnVersit2RRuleNumberingMonthly, "RRULE_NUMBERING_MONTHLY");
_LIT8(KAgnVersit2RRuleNumberingYearly, "RRULE_NUMBERING_YEARLY");
// iTip Method Tests
_LIT8(KAgnVersit2VEventPublish, "VEVENT_ITIP_PUBLISH");
_LIT8(KAgnVersit2VEventRequest, "VEVENT_ITIP_REQUEST");
_LIT8(KAgnVersit2VEventReply, "VEVENT_ITIP_REPLY");
_LIT8(KAgnVersit2VEventAdd, "VEVENT_ITIP_ADD");
_LIT8(KAgnVersit2VEventCancel, "VEVENT_ITIP_CANCEL");
_LIT8(KAgnVersit2VEventRefresh, "VEVENT_ITIP_REFRESH");
_LIT8(KAgnVersit2VEventCounter, "VEVENT_ITIP_COUNTER");
_LIT8(KAgnVersit2VEventDeclineCounter, "VEVENT_ITIP_DECLINECOUNTER");
_LIT8(KAgnVersit2VTodoPublish, "VTODO_ITIP_PUBLISH");
_LIT8(KAgnVersit2VTodoRequest, "VTODO_ITIP_REQUEST");
_LIT8(KAgnVersit2VTodoReply, "VTODO_ITIP_REPLY");
_LIT8(KAgnVersit2VTodoAdd, "VTODO_ITIP_ADD");
_LIT8(KAgnVersit2VTodoCancel, "VTODO_ITIP_CANCEL");
_LIT8(KAgnVersit2VTodoRefresh, "VTODO_ITIP_REFRESH");
_LIT8(KAgnVersit2VTodoCounter, "VTODO_ITIP_COUNTER");
_LIT8(KAgnVersit2VTodoDeclineCounter, "VTODO_ITIP_DECLINECOUNTER");
_LIT8(KAgnVersit2NotAnIcal, "NOT_AN_ICAL");
_LIT8(KAgnVersit2RRuleTzAllDaysFromInt, "RRULE_TZ_ALL_DAYS_FROM_INT");
_LIT8(KAgnVersit2FloatingTime, "FLOATING_TIME");
_LIT8(KAgnVersit2AddAnniversary, "ADD_ANNIVERSARY");
_LIT8(KAgnVersit2AllCategories, "ALL_CATEGORIES");
_LIT8(KAgnVersit2AttendeeRoleStatus, "ATTENDEE_ROLE_STATUS");
_LIT8(KAgnVersit2VTodoStatus, "VTODO_STATUS");
_LIT8(KAgnVersit2VEventStatus, "VEVENT_STATUS");

// Error and warning messages
_LIT(KFileMisMatch, "Exported data in %S does not match expected output in %S");
_LIT(KImpError, "Import Error:");
_LIT(KImpErrorNotSupported, "Import Error (not supported):");
_LIT(KImpErrorMissingData, "Import Error (missing data):");
_LIT(KImpErrorInvalidData, "Import Error (invalid data):");
_LIT(KExpError, "Export Error:");
_LIT(KExpFailed, "Export %d failed with");
_LIT(KImpFailed, "Import failed with");

// Error response types
_LIT(KResponseContinue, "Continue");
_LIT(KResponseSkip, "Skip");
_LIT(KResponseStop, "Stop");
_LIT(KResponseLeave, "Leave");
_LIT(KResponseDie, "Die");

// These properties will be excluded from the import / export comparison tests
// as the dates they contain should be generated at run time.
_LIT8(KDtStamp, "DTSTAMP:");
_LIT8(KCreated, "CREATED:");
_LIT8(KExDate, "EXDATE:");
_LIT8(KRDate, "RDATE:");
_LIT8(KLastMod,"LAST-MODIFIED:");
_LIT8(KProdId, "PRODID:"); 

//Number of succesful test completions before an overall pass is accepted
const TInt KSuccessfulTestCompletions = 10;

const TUint KNewline('\n');

#endif // AGNVERSIT2TESTCONSTANTS_H
