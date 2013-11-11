#ifndef ICALKEYWORDS_H
#define ICALKEYWORDS_H/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Contains the shared literals for versit2. These strings form
*  				part of the iCalendar specification and will not be localised.
*
*/



// Control

_LIT(KICalBegin, "BEGIN");
_LIT(KICalEnd, "END");

// Components

_LIT(KICalVCalendar, "VCALENDAR");

_LIT(KICalEvent, "VEVENT");
_LIT(KICalTodo, "VTODO");
_LIT(KICalJournal, "VJOURNAL");
_LIT(KICalAlarm, "VALARM");
_LIT(KICalTimeZone, "VTIMEZONE");
_LIT(KICalFreeBusy, "VFREEBUSY");

// Timezone sub-components

_LIT(KICalStandard, "STANDARD");
_LIT(KICalDaylight, "DAYLIGHT");

// Properties

_LIT(KICalVersion, "VERSION");
_LIT(KICalProdId, "PRODID");
_LIT(KICalUid, "UID");
_LIT(KICalMethod, "METHOD");
_LIT(KICalCalscale, "CALSCALE");
_LIT(KICalSequence, "SEQUENCE");
_LIT(KICalRecurrenceId, "RECURRENCE-ID");
_LIT(KICalRange, "RANGE");
_LIT(KICalRRule, "RRULE");
_LIT(KICalAction, "ACTION");
_LIT(KICalAttach, "ATTACH");
_LIT(KICalAttendee, "ATTENDEE");
_LIT(KICalCategories, "CATEGORIES");
_LIT(KICalClass, "CLASS");
_LIT(KICalComment, "COMMENT");
_LIT(KICalCompleted, "COMPLETED");
_LIT(KICalContact, "CONTACT");
_LIT(KICalCreated, "CREATED");
_LIT(KICalDescription, "DESCRIPTION");
_LIT(KICalDtstamp, "DTSTAMP");
_LIT(KICalDtstart, "DTSTART");
_LIT(KICalDue, "DUE");
_LIT(KICalDuration, "DURATION");
_LIT(KICalExdate, "EXDATE");
_LIT(KICalExrule, "EXRULE");
_LIT(KICalFreebusy, "FREEBUSY");
_LIT(KICalGeo, "GEO");
_LIT(KICalLastmodified, "LAST-MODIFIED");
_LIT(KICalLocation, "LOCATION");
_LIT(KICalOrganizer, "ORGANIZER");
_LIT(KICalPercentcomplete, "PERCENT-COMPLETE");
_LIT(KICalPriority, "PRIORITY");
_LIT(KICalRequeststatus, "REQUEST-STATUS");
_LIT(KICalRelatedto, "RELATED-TO");
_LIT(KICalRepeat, "REPEAT");
_LIT(KICalResources, "RESOURCES");
_LIT(KICalRdate, "RDATE");
_LIT(KICalStatus, "STATUS");
_LIT(KICalSummary, "SUMMARY");
_LIT(KICalTransp, "TRANSP");
_LIT(KICalTrigger, "TRIGGER");
_LIT(KICalTzid, "TZID");
_LIT(KICalTzname,"TZNAME");
_LIT(KICalTzoffsetfrom, "TZOFFSETFROM");
_LIT(KICalTzoffsetto, "TZOFFSETTO");
_LIT(KICalTzurl, "TZURL");
_LIT(KICalUrl, "URL");
_LIT(KICalDtend, "DTEND");
_LIT(KICalXProperty, "X-");

// Reccurence ranges

_LIT(KICalThisAndPrior, "THISANDPRIOR");
_LIT(KICalThisAndFuture, "THISANDFUTURE");

// RRule segments

_LIT(KICalFreq, "FREQ");
_LIT(KICalUntil, "UNTIL");
_LIT(KICalCount, "COUNT");
_LIT(KICalInterval, "INTERVAL");
_LIT(KICalBySecond, "BYSECOND");
_LIT(KICalByMinute, "BYMINUTE");
_LIT(KICalByHour, "BYHOUR");
_LIT(KICalByDay, "BYDAY");
_LIT(KICalByMonthDay, "BYMONTHDAY");
_LIT(KICalByYearDay, "BYYEARDAY");
_LIT(KICalByWeekDay, "BYWEEKDAY");
_LIT(KICalByWeekNo, "BYWEEKNO");
_LIT(KICalByMonth, "BYMONTH");
_LIT(KICalWkSt, "WKST");
_LIT(KICalBySetPos, "BYSETPOS");

// Methods

_LIT(KICalPublish, "PUBLISH");
_LIT(KICalRequest, "REQUEST");
_LIT(KICalReply, "REPLY");
_LIT(KICalAdd, "ADD");
_LIT(KICalCancel, "CANCEL");
_LIT(KICalRefresh, "REFRESH");
_LIT(KICalCounter, "COUNTER");
_LIT(KICalDeclineCounter, "DECLINECOUNTER");

// Separators

_LIT(KICalColon, ":");
_LIT(KICalSemiColon, ";");
_LIT(KICalEquals, "=");
_LIT(KICalComma, ",");

const TUint KICalDateTimeSeparator('T');

// Escape characters

_LIT(KICalBackslash, "//");
_LIT(KICalQuote, "\"");

// Escaped characters

_LIT(KICalBackslashN, "\\n");	// The character '\', followed by the character 'n', *not* newline.

const TUint KICalBackslashChar('\\');
const TUint KICalCarriageReturnChar('\r');
const TUint KICalColonChar(':');
const TUint KICalCommaChar(',');
const TUint KICalEqualsChar('=');
const TUint KICalLowerCaseNChar('n');
const TUint KICalNewlineChar('\n');
const TUint KICalQuoteChar('"');
const TUint KICalSemiColonChar(';');
const TUint KICalUpperCaseNChar('N');

// Whitespace characters

const TUint KICalSpaceChar(' ');
const TUint KICalHTabChar('\t');

// Parameters

_LIT(KICalAtlrep, "ALTREP");
_LIT(KICalCn, "CN");
_LIT(KICalCuType, "CUTYPE");
_LIT(KICalDelegatedFrom, "DELEGATED-FROM");
_LIT(KICalDelegatedTo, "DELEGATED-TO");
_LIT(KICalDir, "DIR");
_LIT(KICalEncoding, "ENCODING");
_LIT(KICalFmtType, "FMTTYPE");
_LIT(KICalFbType, "FBTYPE");
_LIT(KICalLanguage, "LANGUAGE");
_LIT(KICalMember, "MEMBER");
_LIT(KICalPartStat, "PARTSTAT");
_LIT(KICalRelated, "RELATED");
_LIT(KICalRelType, "RELTYPE");
_LIT(KICalRole, "ROLE");
_LIT(KICalRsvp, "RSVP");
_LIT(KICalSentBy, "SENT-BY");
_LIT(KICalValue, "VALUE");
_LIT(KICalIndividual, "INDIVIDUAL");
_LIT(KICalGroup, "GROUP");
_LIT(KICalResource, "RESOURCE");
_LIT(KICalRoom, "ROOM");
_LIT(KICalUnknown, "UNKNOWN");
_LIT(KICalBase64, "BASE64");
_LIT(KICalFree, "FREE");
_LIT(KICalBusy, "BUSY");
_LIT(KICalBusyUnavailable, "BUSY-UNAVAILABLE");
_LIT(KICalBusyTentative, "BUSY-TENTATIVE");
_LIT(KICalNeedsAction, "NEEDS-ACTION");
_LIT(KICalAccepted, "ACCEPTED");
_LIT(KICalDeclined, "DECLINED");
_LIT(KICalTentative, "TENTATIVE");
_LIT(KICalDelegated, "DELEGATED");
_LIT(KICalInProcess, "IN-PROCESS");
_LIT(KICalStart, "START");
_LIT(KICalParent, "PARENT");
_LIT(KICalChild, "CHILD");
_LIT(KICalSibling, "SIBLING");
_LIT(KICalChair, "CHAIR");
_LIT(KICalReqParticipant, "REQ-PARTICIPANT");
_LIT(KICalOptParticipant, "OPT-PARTICIPANT");
_LIT(KICalNonParticipant, "NON-PARTICIPANT");
_LIT(KICalTrue, "TRUE");
_LIT(KICalFalse, "FALSE");
_LIT(KICalSecondly, "SECONDLY");
_LIT(KICalMinutely, "MINUTELY");
_LIT(KICalHourly, "HOURLY");
_LIT(KICalDaily, "DAILY");
_LIT(KICalWeekly, "WEEKLY");
_LIT(KICalMonthly, "MONTHLY");
_LIT(KICalYearly, "YEARLY");
_LIT(KICalSunday, "SU");
_LIT(KICalMonday, "MO");
_LIT(KICalTuesday, "TU");
_LIT(KICalWednesday, "WE");
_LIT(KICalThursday, "TH");
_LIT(KICalFriday, "FR");
_LIT(KICalSaturday, "SA");
_LIT(KICalPublic, "PUBLIC");
_LIT(KICalPrivate, "PRIVATE");
_LIT(KICalConfidential, "CONFIDENTIAL");
_LIT(KICalConfirmed, "CONFIRMED");
_LIT(KICalCancelled, "CANCELLED");
_LIT(KICalDraft, "DRAFT");
_LIT(KICalFinal, "FINAL");
_LIT(KICalDateTime, "DATE-TIME");
_LIT(KICalDate, "DATE");
_LIT(KICalPeriod, "PERIOD");
_LIT(KICalOpaque, "OPAQUE");
_LIT(KICalTransparent, "TRANSPARENT");
_LIT(KICalAudio, "AUDIO");
_LIT(KICalDisplay, "DISPLAY");
_LIT(KICalEmail, "EMAIL");
_LIT(KICalProcedure, "PROCEDURE");

#endif	// ICALKEYWORDS_H

// End of File
