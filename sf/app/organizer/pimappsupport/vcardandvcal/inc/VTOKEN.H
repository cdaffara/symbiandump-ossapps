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

/**
 @file
 @publishedAll
 @released 
*/
 
#ifndef __VTOKEN_H__
#define __VTOKEN_H__

#include <e32base.h>

#define KMaxExternalizedTokenLength			70
#define KMinLineLengthToWriteTo				1

/** A colon string. */
_LIT8(KVersitTokenColon,					":");
/** A Unicode colon string. */
_LIT(KVersitTokenColonUnicode,				":");
/** A semi-colon string. */
_LIT8(KVersitTokenSemiColon,				";");
/** A Unicode semi-colon string. */
_LIT(KVersitTokenSemiColonUnicode,			";");
/** A backslash string (escaped). */
_LIT8(KVersitBackSlash,						"\\");
/** An escaped semi-colon string. */
_LIT8(KVersitEscapedSemiColon,				"\\;");
/** An escaped Unicode semi-colon string. */
_LIT(KVersitEscapedSemiColonUnicode,		"\\;");
/** An equals sign string. */
_LIT8(KVersitTokenEquals,					"=");
/** A full stop string. */
_LIT8(KVersitTokenPeriod,					".");
/** A space string. */
_LIT8(KVersitTokenSpace,					" ");
/** A minus sign string. */
_LIT8(KVersitTokenMinus,					"-");
/** A plus sign string. */
_LIT8(KVersitTokenPlus,						"+");
/** A line break string. */
_LIT8(KVersitLineBreak,						"\r\n  ");
/** The string used at the start of a time interval string. */
_LIT8(KVersitTimePeriodBegin,				"P");
/** The string appended to the number of years when encoding a time interval. */
_LIT8(KVersitTimePeriodYear,				"Y");
/** The string appended to the number of months when encoding a time interval. */
_LIT8(KVersitTimePeriodMonth,				"M");
/** Not used. */
_LIT8(KVersitTimePeriodWeek,				"W");		//unused
/** The string appended to the number of days when encoding a time interval. */
_LIT8(KVersitTimePeriodDay,					"D");
/** The string added before the time of day when encoding a Versit date/time.

Also the string added before the time of day when encoding a time interval. */
_LIT8(KVersitTimePeriodTime,				"T");
/** The string appended to the hour value when encoding a time interval. */
_LIT8(KVersitTimePeriodHour,				"H");
/** The string appended to the minutes value when encoding a time interval. */
_LIT8(KVersitTimePeriodMinute,				"M");
/** The string appended to the seconds value when encoding a time interval. */
_LIT8(KVersitTimePeriodSecond,				"S");
/**  The string appended to a date/time when encoding a Versit 
date/time, if the value is in universal time. */
_LIT8(KVersitTokenUniversalTime,			"Z");

/** A colon character. */
const TUint KVersitTokenColonVal			=':';
/** A semi-colon character. */
const TUint KVersitTokenSemiColonVal		=';';
/** A backslash character. */
const TUint KVersitTokenBackslashVal		='\\';
/** An equals sign character. */
const TUint KVersitTokenEqualsVal			='=';
/** A full stop character. */
const TUint KVersitTokenPeriodVal			='.';
/** A comma character. */
const TUint KVersitTokenCommaVal			=',';
/** A left square bracket character. */
const TUint KVersitTokenLSquareBracketVal	='[';
/** A right square bracket character. */
const TUint KVersitTokenRSquareBracketVal	=']';
/** A minus sign character. */
const TUint KVersitTokenMinusVal			='-';
/** A plus sign character. */
const TUint KVersitTokenPlusVal				='+';
/** A Yen character: the escape character for the 'ShiftJis' character set. */
const TUint KVersitTokenYenVal				=0xa5;
/** The character at the beginning of a time interval string. */
const TUint KVersitTimePeriodBeginVal		='P';
/** The character following the number of years in a time interval string. */
const TUint KVersitTimePeriodYearVal		='Y';
/** The character following the number of months in a time interval string. */
const TUint KVersitTimePeriodMonthVal		='M';
/** The character following the number of weeks in a time interval string. */
const TUint KVersitTimePeriodWeekVal		='W';
/** The character following the number of days in a time interval string. */
const TUint KVersitTimePeriodDayVal			='D';
/** The character preceding a length of day (hours,minutes and seconds) in a time 
interval string.

Also the character preceding the time of day in a date/time string. */
const TUint KVersitTimePeriodTimeVal		='T';
/** The character following the number of hours in a time interval string. */
const TUint KVersitTimePeriodHourVal		='H';
/** The character following the number of minutes in a time interval string. */
const TUint KVersitTimePeriodMinuteVal		='M';
/** The character following the number of seconds in a time interval string. */
const TUint KVersitTimePeriodSecondVal		='S';
/** The character at the end of a universal date/time string. */
const TUint KVersitTokenUniversalTimeVal	='Z';
/** The character at the start of a daily recurring entity string. */
const TUint KVersitRecurrenceDailyVal		='D';
/** The character at the start of a weekly recurring entity string. */
const TUint KVersitRecurrenceWeeklyVal		='W';
/** The character which precedes the number of recurrences, when specified, in 
a recurring entity. */
const TUint KVersitRecurrenceNumberOfVal	='#';
/** The string at the beginning of a 'monthly by position' recurring entity 
(see CVersitRecurrenceMonthlyByPos). Used when internalising. */
_LIT(KVersitRecurrenceMonthlyByPos,		"MP");
/** Not used. */
_LIT(KVersitRecurrenceMonthlyByDay,		"MD");
/** Not used. */
_LIT(KVersitRecurrenceYearlyByMonth,		"YM");
/** The string at the beginning of a 'yearly by day' recurring entity, 
(see CVersitRecurrenceYearlyByDay). Used when internalising. */
_LIT(KVersitRecurrenceYearlyByDay,			"YD");
/** A string in the date list (if a date list is defined) of an entity 
that recurs either weekly or 'monthly by position' on Mondays. Used when 
internalising. */
_LIT(KVersitRecurrenceMonday,				"MO");
/** A string in the date list (if a date list is defined) of an entity 
that recurs either weekly or 'monthly by position' on Tuesdays. Used when internalising. */
_LIT(KVersitRecurrenceTuesday,				"TU");
/** A string in the date list (if a date list is defined) of an entity 
that recurs either weekly or 'monthly by position' on Wednesdays. Used when internalising. */
_LIT(KVersitRecurrenceWednesday,			"WE");
/** A string in the date list (if a date list is defined) of an entity that 
recurs either weekly or 'monthly by position' on Thursdays. Used when internalising. */
_LIT(KVersitRecurrenceThursday,				"TH");
/** A string in the date list (if a date list is defined) of an entity that 
recurs either weekly or 'monthly by position' on Fridays. Used when internalising. */
_LIT(KVersitRecurrenceFriday,				"FR");
/** A string in the date list (if a date list is defined) of an entity that 
recurs either weekly or 'monthly by position' on Saturdays. Used when internalising. */
_LIT(KVersitRecurrenceSaturday,				"SA");
/** A string in the date list (if a date list is defined) of an entity that 
recurs either weekly or 'monthly by position' on Sundays. Used when internalising. */
_LIT(KVersitRecurrenceSunday,				"SU");
/** The string appended to the 'occurrence list' section of an entity that 
recurs on the last day of the month. Used when internalising. */
_LIT(KVersitRecurrenceLastDay,				"LD");

/** The string used at the start of a daily recurring entity. */
_LIT8(KVersitRecurrenceDaily,				"D");
/** The string used at the start of a weekly recurring entity. */
_LIT8(KVersitRecurrenceWeekly,				"W");
/** The string appended just before the number of occurrences, when encoding a recurring 
entity as a descriptor. */
_LIT8(KVersitRecurrenceNumberOf,			"#");
/** The string used at the start of a 'monthly by position' recurring entity, 
(see CVersitRecurrenceMonthlyByPos). Used when externalising. */
_LIT8(KVersitRecurrenceMonthlyByPos8,		"MP");
/** Not used. */
_LIT8(KVersitRecurrenceMonthlyByDay8,		"MD");
/** Not used. */
_LIT8(KVersitRecurrenceYearlyByMonth8,		"YM");
/** The string used at the start of a 'yearly by day' recurring entity, 
(see CVersitRecurrenceYearlyByDay). Used when externalising. */
_LIT8(KVersitRecurrenceYearlyByDay8,			"YD");
/** The string appended to the 'occurrence list' section of an entity that recurs on Mondays. 
Used when externalising. */
_LIT8(KVersitRecurrenceMonday8,				"MO");
/** The string appended to the 'occurrence list' section of an entity that recurs on Tuesdays. 
Used when externalising. */
_LIT8(KVersitRecurrenceTuesday8,				"TU");
/** The string appended to the 'occurrence list' section of an entity that recurs on Wednesdays. 
Used when externalising. */
_LIT8(KVersitRecurrenceWednesday8,			"WE");
/** The string appended to the 'occurrence list' section of an entity that recurs on Thursdays. 
Used when externalising. */
_LIT8(KVersitRecurrenceThursday8,				"TH");
/** The string appended to the 'occurrence list' section of an entity that recurs on Fridays. 
Used when externalising. */
_LIT8(KVersitRecurrenceFriday8,				"FR");
/** The string appended to the 'occurrence list' section of an entity that recurs on Saturdays. 
Used when externalising. */
_LIT8(KVersitRecurrenceSaturday8,				"SA");
/** The string appended to the 'occurrence list' section of an entity that recurs on Sundays. 
Used when externalising. */
_LIT8(KVersitRecurrenceSunday8,				"SU");
/** The string appended to the 'occurrence list' section of an entity that recurs on the last day of 
the month. Used when externalising. */
_LIT8(KVersitRecurrenceLastDay8,				"LD");
/** The string at the start of an entity encoded in a stream. 

This is implemented as the name of a property, but this property is not stored in the entity's array 
of properties. */
_LIT8(KVersitTokenBEGIN,					"BEGIN");
/** The string at the start of an agent vCard entity encoded in a stream. 

The reason for needing a Unicode version, is that the agent vCard is held in a 
property value in the parent vCard, and all property values are converted 
to Unicode when they are internalised, in order to make them character set-independent. 

This is implemented as the name of a property, but this property is not stored in the 
entity's array of properties. */
_LIT(KVersitVarTokenBEGIN,					"BEGIN");
/** The string which follows the body of data of a vCard/vCalendar encoded in a stream. 
It is followed only by the entity name. 

This is implemented as the name of a property, but this property is not stored in the 
entity's array of properties. */
_LIT8(KVersitTokenEND,						"END");
/** A carriage return and line feed */
_LIT8(KVersitTokenCRLF,						"\r\n"); // carriage return, line feed
/** A boolean true value. Not used. */
_LIT8(KVersitTokenTRUE,						"TRUE");
/** A boolean true value. */
_LIT(KVersitVarTokenTRUE,					"TRUE");
/** A boolean false value. Not used. */
_LIT8(KVersitTokenFALSE,					"FALSE");
/** A boolean false value. */
_LIT(KVersitVarTokenFALSE,					"FALSE");
/** The symbol added to the beginning of an unrecognised property name while 
internalising from a stream. */
_LIT8(KVersitTokenXDashEPOC,				"X-EPOC");
_LIT8(KVersitTokenXDash,					"X-");
/** An empty string. */
_LIT8(KVersitTokenEmptyNarrow,				"");
/** An empty string. */
_LIT(KVersitTokenEmpty,						"");

//The following names should not be used, versit now uses CharConv character set names.
// params
_LIT8(KVersitTokenENCODING,					"ENCODING");
_LIT8(KVersitTokenBASE64,					"BASE64");
_LIT8(KVersitTokenQUOTEDPRINTABLE,			"QUOTED-PRINTABLE");
_LIT8(KVersitToken8BIT,						"8-BIT");
_LIT8(KVersitTokenCHARSET,					"CHARSET");
_LIT8(KVersitTokenUTF8,						"UTF-8");
_LIT8(KVersitTokenUTF7,						"UTF-7");
_LIT8(KVersitTokenISO1,						"ISO-8859-1");
_LIT8(KVersitTokenISO2,						"ISO-8859-2");
_LIT8(KVersitTokenISO4,						"ISO-8859-4");
_LIT8(KVersitTokenISO5,						"ISO-8859-5");
_LIT8(KVersitTokenISO7,						"ISO-8859-7");
_LIT8(KVersitTokenISO9,						"ISO-8859-9");
_LIT8(KVersitTokenTYPE,						"TYPE");	//used for photo format type 

// parameters for the X-EPOCALARM 
_LIT8(KVersitTokenVALUE,					"VALUE");
_LIT8(KVersitTokenCONTENTTYPE,				"X-CONTENTTYPE");
_LIT8(KVersitTokenINLINE, 					"INLINE");
_LIT8(KVersitTokenBINARY, 					"BINARY");

// additional character sets
_LIT8(KVersitTokenISO3,						"ISO-8859-3");
_LIT8(KVersitTokenISO10,					"ISO-8859-10");
_LIT8(KVersitTokenShiftJIS,					"SHIFT_JIS");
_LIT8(KVersitTokenGB2312,					"GB 2312-80");		//This and the following contain the wrong name.
_LIT8(KVersitTokenGBK,						"GBK");
_LIT8(KVersitTokenBIG5,						"BIG5 level 1 and 2");
_LIT8(KVersitTokenISO2022JP,				"ISO 2022-JP");
_LIT8(KVersitTokenEUCJP,					"EUC-JP");
_LIT8(KVersitTokenJIS,						"JIS");


// entities
_LIT8(KVersitTokenVCALENDAR,				"VCALENDAR");
_LIT8(KVersitTokenVCARD,					"VCARD");
_LIT(KVersitVarTokenVCALENDAR,				"VCALENDAR");
_LIT(KVersitVarTokenVCARD,					"VCARD");
_LIT(KVersitVarTokenVEVENT,					"VEVENT");
_LIT(KVersitVarTokenVTODO,					"VTODO");

// properties
_LIT8(KVersitTokenXALARM,					"X-EPOCALARM");
_LIT8(KVersitTokenAALARM,					"AALARM");
_LIT8(KVersitTokenDALARM,					"DALARM");
_LIT8(KVersitTokenPALARM,					"PALARM");
_LIT8(KVersitTokenMALARM,					"MALARM");
_LIT8(KVersitTokenDAYLIGHT,					"DAYLIGHT");
_LIT(KVersitVarTokenDAYLIGHT,				"DAYLIGHT");
_LIT8(KVersitTokenVERSION,					"VERSION");
_LIT8(KVersitTokenCATEGORIES,				"CATEGORIES");
_LIT8(KVersitTokenRESOURCES,				"RESOURCES");
_LIT8(KVersitTokenDCREATED,					"DCREATED");
_LIT8(KVersitTokenDTSTART,					"DTSTART");
_LIT8(KVersitTokenDTEND,					"DTEND");
_LIT8(KVersitTokenLASTMODIFIED,				"LAST-MODIFIED");
_LIT8(KVersitTokenCOMPLETED,				"COMPLETED");
_LIT8(KVersitTokenDUE,						"DUE");
_LIT8(KVersitTokenEXDATE,					"EXDATE");
_LIT8(KVersitTokenEXRULE,					"EXRULE");
_LIT8(KVersitTokenRDATE,					"RDATE");
_LIT8(KVersitTokenRRULE,					"RRULE");
_LIT8(KVersitTokenRNUM,						"RNUM");
_LIT8(KVersitTokenPRIORITY,					"PRIORITY");
_LIT8(KVersitTokenSEQUENCE,					"SEQUENCE");
_LIT8(KVersitTokenTRANSP,					"TRANSP");
_LIT8(KVersitTokenBDAY,						"BDAY");
_LIT8(KVersitTokenAGENT,					"AGENT");
_LIT8(KVersitTokenLABEL,					"LABEL");
_LIT8(KVersitTokenPHOTO,					"PHOTO");
_LIT8(KVersitTokenEMAIL,					"EMAIL");
_LIT8(KVersitTokenINTERNET,					"INTERNET");
_LIT8(KVersitTokenTITLE,					"TITLE");
_LIT8(KVersitTokenROLE,						"ROLE");
_LIT8(KVersitTokenLOGO,						"LOGO");
_LIT8(KVersitTokenNOTE,						"NOTE");
_LIT8(KVersitTokenSOUND,					"SOUND");
_LIT8(KVersitTokenMAILER,					"MAILER");
_LIT8(KVersitTokenPRODID,					"PRODID");
_LIT8(KVersitTokenATTACH,					"ATTACH");
_LIT8(KVersitTokenATTENDEE,					"ATTENDEE");
_LIT8(KVersitTokenCLASS,					"CLASS");
_LIT8(KVersitTokenDESCRIPTION,				"DESCRIPTION");
_LIT8(KVersitTokenLOCATION,					"LOCATION");
_LIT8(KVersitTokenRELATEDTO,				"RELATED-TO");
_LIT8(KVersitTokenSTATUS,					"STATUS");
_LIT8(KVersitTokenSUMMARY,					"SUMMARY");
_LIT8(KVersitTokenN,						"N");
_LIT8(KVersitTokenTZ,						"TZ");
_LIT8(KVersitTokenADR,						"ADR");
_LIT8(KVersitTokenORG,						"ORG");
_LIT8(KVersitTokenREV,						"REV");
_LIT8(KVersitTokenFN,						"FN");
_LIT8(KVersitTokenTEL,						"TEL");
_LIT8(KVersitTokenURL,						"URL");
_LIT8(KVersitTokenGEO,						"GEO");
_LIT8(KVersitTokenUID,						"UID");
_LIT8(KVersitTokenKEY,						"KEY");
_LIT8(KVersitTokenSECONDNAME,				"X-EPOCSECONDNAME");
_LIT8(KVersitTokenXMETHOD,					"X-METHOD");        // For Group Scheduling
_LIT8(KVersitTokenXRECURRENCEID,			"X-RECURRENCE-ID"); // For Group Scheduling
_LIT8(KVersitTokenXLOCALUID,				"X-SYMBIAN-LUID");
_LIT8(KVersitTokenXDTSTAMP,					"X-SYMBIAN-DTSTAMP");
_LIT(KVersitVarTokenINTERNET,				"INTERNET");		//Used by Jave Phone and not by versit
_LIT8(KVersitTokenAssistant,				"X-ASSISTANT");
_LIT8(KVersitTokenAssistantTel,				"X-ASSISTANT-TEL");
_LIT8(KVersitTokenAnniversary,				"X-ANNIVERSARY");
_LIT8(KVersitTokenSpouse,					"X-SPOUSE");
_LIT8(KVersitTokenChildren,					"X-CHILDREN");
_LIT8(KVersitTokenClass,					"X-CLASS");
_LIT8(KVersitAttachLabel, 					"X-NAME");
_LIT8(KVersitAttachMimeType, 				"X-FMTTYPE");

// favourite property
_LIT8(KVCalTokenXFavourite,					"X-FAVOURITE");

#endif
