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

#include "tcal_dataexchange.h"

#include "caltestlib.h"

#include <caldataformat.h>
#include <s32file.h>
#include <e32test.h>
#include <caluser.h>
#include <calcategory.h>
#include <calentry.h>
#include <calentryview.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <caliterator.h>
#include <calrrule.h>
#include <coreappstest/testserver.h>
#include <calalarm.h>
#include <vtzrules.h>
#include <calattachment.h>
#include <vtoken.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <s32mem.h>
#include <calsession.h>

RTest test(_L("tcal_dataexchange"));

const TInt KNumberOfEntriesToHandleAtOnce = 5;
const TInt KMaxSize = 1024;

_LIT(KCalendarFile,     "tcal_dataexchange");
_LIT(KEntryImportFile,  "tcal_dataexchange_import%d.vcs");
_LIT(KEntryExportFile,  "tcal_dataexchange_export%d.vcs");
_LIT(KEntry1File, "tcal_entry1.vcs" );
_LIT(KEntryExportFileAttendee,"tcal_dataexchange_export_atttendee.vcs");
_LIT(KEntryExportFileGSEntries,"tcal_dataexchange_export_gsentries.vcs");
_LIT(KEntryExportFileReminderEntries,"tcal_dataexchange_export_gsrmdrentries.vcs");
_LIT(KAsyncExportFile,  "export_file_async.vcs");
_LIT(KBadExceptionFile, "badexceptiondate.vcs");
_LIT(KTestTzImportFile, "tcal_datax_importtz.vcs");
_LIT(KTestTzExportFile, "tcal_datax_exporttz.vcs");
_LIT(KTestTypeSeqFile, 	"tcal_datax_typeseq.vcs");
_LIT(KTestRRuleEndDateFile,	"tcal_datax_rruleenddate.vcs");
_LIT(KOrganizerTestFile, "OrganizerTestFile.vcs");
_LIT(KTestTzFile, "tcal_test_tz.vcs");
_LIT(KTestTzFile2, "tcal_test_tz2.vcs");
_LIT(KTestRRuleWithInvalidDateFile, "tcal_datax_invalid_rruledate.vcs");
_LIT(KTestExportFile, "tcal_export.vcs");
_LIT(KTestTodoWithRuleImportFile, "tcal_datax_import_Rule.vcs");
_LIT(KTestTodoWithRuleImportExportedFile, "tcal_datax_exported.vcs");

_LIT(KExportFilePath, "c:\\logs\\tcal_dataexchange\\temp\\");


#ifdef __WINS__
_LIT(KTestWithLongUTFUID, "c:\\LongUTFuid.vcs");
_LIT(KTestNewExportLongUID, "c:\\NewExportLongUID.vcs");
#else //not wins
_LIT(KTestWithLongUTFUID, "z:\\LongUTFuid.vcs");
_LIT(KTestNewExportLongUID, "z:\\NewExportLongUID.vcs");
#endif
_LIT(KExportedUTFUID, "ExportedLongUTFuid.vcs");
_LIT(KLongExportedUID, "ExportLongUID.vcs");

_LIT(KTestTodoWithTzImportFile, "tcal_datax_import_todo_tz.vcs");
_LIT(KTestTodoWithXEPOCALARMImportFile, "tcal_datax_import_todo_Xalarm.vcs");

_LIT(KTestTodoWithWrongTzImportFile, "tcal_datax_import_todo_wrong_tz.vcs");

_LIT8(KTzStanley, "Atlantic/Stanley");
_LIT(KMultipleTZAsyncFilename, "c:\\logs\\tcal_dataexchange\\temp\\multipletzasync.vcs");

_LIT8(KDummyID,         "DUMMY_ID");
_LIT8(KEntry1UID,       "666");
_LIT8(KEntry1UID2,      "222");
_LIT8(KEntry2UID,       "669");
_LIT8(KEntry3UID,       "1001");
_LIT8(KEntry13UID, "13");
_LIT8(KEntry14UID, "14");
_LIT8(KEntry15UID, "15");
_LIT8(KEntry16UID, "16");
_LIT8(KEntry17UID, "17");
_LIT8(KEntry18UID, "18");
_LIT8(KEntry20UID, "20");
_LIT8(KEntry22UID, "22");
_LIT8(KEntry24UID, "24");
// GEO vCal UIDs
_LIT8(KGEOInBoundsPositiveImportTestVCalUID, "123001");
_LIT8(KGEOInBoundsNegativeImportTestVCalUID, "123002");
_LIT8(KGEOOnBoundsPositiveImportTestVCalUID, "123003");
_LIT8(KGEOOnBoundsNegativeImportTestVCalUID, "123004");
_LIT8(KGEOValidZeroImportTestVCalUID, "123005");
_LIT8(KGEOOutOfBoundsPositiveImportTestVCalUID, "123006");
_LIT8(KGEOOutOfBoundsNegativeImportTestVCalUID, "123007");
_LIT8(KGEOTruncationRoundingUpPositiveImportTestVCalUID, "123008");
_LIT8(KGEOTruncationRoundingUpNegativeImportTestVCalUID, "123009");
_LIT8(KGEOTruncationRoundingDownPositiveImportTestVCalUID, "123010");
_LIT8(KGEOTruncationRoundingDownNegativeImportTestVCalUID, "123011");

_LIT8(KGEONoDecimalPositiveImportTestVCalUID, "123012");
_LIT8(KGEONoDecimalNegativeImportTestVCalUID, "123013");

_LIT8(KGEOFewDecimalsPositiveImportTestVCalUID, "123014");
_LIT8(KGEOFewDecimalsNegativeImportTestVCalUID, "123015");

_LIT8(KGEOInvalidEmptyValuesImportTestVCalUID, "123016");
_LIT8(KGEOInvalidDataCharsImportTestVCalUID, "123017");
_LIT8(KGEOInvalidDataAlphaNumImportTestVCalUID, "123018");
_LIT8(KGEOInvalidSeparatorImportTestVCalUID, "123019");

_LIT(KEntry3Organizer,  "Example Person");
_LIT(KEntry3PhoneOwner, "Another Example");

_LIT(KFileName, "vcal_alarm.vcs");
_LIT(KFileNameExp, "vcal_alarm_exp.vcs");

_LIT8(KGUID,            "A_Global_UID");
_LIT(KSUMMARY,          "First Entry");
_LIT(KAttachmentFile, "tcal_datax_attachment.vcs");
_LIT(KAttachmentCidFile, "c:\\calendar_attachments\\tcal_attach.jpg");
_LIT(KCalendarAttachmentsDir, "c:\\private\\10003a5b\\attachments");

_LIT8(KTzEuropeHelsinki, "Europe/Helsinki");
_LIT8(KTzAmericaVancouver, "America/Vancouver");
_LIT8(KTzAsiaTokyo, "Asia/Tokyo");
_LIT8(KTzAtlanticStanley, "Atlantic/Stanley");

//PDEF111292
_LIT8(KTzGMTMinusNine, "Etc/GMT-9");
_LIT8(KTzGMTPlusEight, "Etc/GMT+8");
//PDEF111292

_LIT8(KEntry1,	"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:666\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "TRANSP:2\r\n"
				 "GEO:50.123456,95.123456\r\n"
				 "ATTENDEE;ROLE=ATTENDEE:\r\n"				
				 "X-METHOD:REQUEST\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+01\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
				 "UID:222\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:temp\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
				 "ATTENDEE;ROLE=OWNER:\r\n"				
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KEntry2,	"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
				 "UID:669\r\n"
				 "SEQUENCE:2\r\n"
				 "SUMMARY:second entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050811T100000Z\r\n"
				 "DTEND:20050811T200000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "ATTENDEE;ROLE=ATTENDEE:\r\n"				
				 "X-METHOD:COUNTER\r\n"
				 "LOCATION:Someone's Office\r\n"
				 "TRANSP:0\r\n"
				 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND:19990101T090000;;0;\r\n"
				"X-EPOCALARM;VALUE=INLINE;X-CONTENTTYPE=;ENCODING=BASE64;CHARSET=UTF-8:\r\n"
				"    MTk5OTAxMDFUMDkwMDAw\r\n"
				"\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KEntry3,	"BEGIN:VCALENDAR\r\n"
				 "VERSION:1.0\r\n"
				"BEGIN:VEVENT\r\n"
				 "UID:1001\r\n"
				 "SUMMARY:GS appt\r\n"
				 "ATTACH:Unsupported\r\n"
				 "DESCRIPTION:Some description\r\n"
				 "DTSTART:20050503T070000Z\r\n"
				 "DTEND:20050503T080000Z"
				 "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
				 "CLASS:PUBLIC\r\n"
				 "SEQUENCE:0\r\n"
				 "X-METHOD:REQUEST\r\n"
				 "ATTENDEE;ROLE=ORGANIZER;STATUS=ACCEPTED;RSVP=NO;EXPECT=FYI;X-CN=Example Person;X-STATUS=NEEDS-ACTION;X-ROLE=REQ-PARTICIPANT:ex.person@example.com\r\n"
				 "ATTENDEE;ROLE=DELEGATE;STATUS=NEEDSACTION;RSVP=NO;EXPECT=FYI;X-ROLE=REQ-PARTICIPANT;X-STATUS=NEEDS-ACTION:ex.person2@example.com\r\n"
				 "ATTENDEE;ROLE=DELEGATE;X-PHONEOWNER;STATUS=NEEDSACTION;RSVP=NO;EXPECT=FYI;X-ROLE=REQ-PARTICIPANT;X-CN=Another Example;X-STATUS=NEEDS-ACTION:an.example@example.com\r\n"
				 "STATUS:NEEDS ACTION\r\n"
				 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KEntry4,	"BEGIN:VCALENDAR\r\n"
				 "VERSION:1.0\r\n"
				 "TZ:+02\r\n"
				 "DAYLIGHT:TRUE;+03;20050327T030000;20051030T040000;+02;+03\r\n"
				"BEGIN:VEVENT\r\n"
				 "UID:01112005062233401500\r\n"
				 "SUMMARY;QUOTED-PRINTABLE:Recurrence test\r\n"
				 "DTSTART:20051107T060000Z\r\n"
				 "DTEND:20051107T063000Z\r\n"
				 "STATUS:CONFIRMED\r\n"
				 "CLASS:PUBLIC\r\n"
				 "LOCATION;QUOTED-PRINTABLE:Server\r\n"
				 "RRULE:W1 MO TU WE TH FR 20051111T000000\r\n"
				 "EXDATE:20051108T000000\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);


//			   
// from former tcal_entry


_LIT(KVCalendarFile,"c:\\vcal2.vcs");
_LIT(KVCalLastDayFile, "c:\\vcallastday.vcs");

// the following vcal contains 11 entries
const TInt KEntry5Number = 11;
					
_LIT8(KEntry5,	"BEGIN:VCALENDAR\r\n"
				 "PRODID:-//Microsoft Corporation//Outlook MIMEDIR//EN\r\n"
				 "VERSION:1.0\r\n"
				"BEGIN:VEVENT\r\n"
				 "STATUS:ACCEPTED\r\n"
				 "DTSTART:20010418T120000Z\r\n"
				 "DTEND:20010418T130000Z\r\n"
				 "LOCATION:Random Pub\r\n"
				 "SUMMARY:Eat lunch\r\n"
				 "DESCRIPTION:Working lunch with Team X. Discuss the preliminary work for new Gee-Whiz promo.\r\n"
				 "PRIORITY:2\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				 "STATUS:CONFIRMED\r\n"
				 "DTSTART:20010418T120000Z\r\n"
				 "DTEND:20010418T130000Z\r\n"
				 "LOCATION:Trendy Night Club\r\n"
				 "SUMMARY:Party!!!\r\n"
				 "DESCRIPTION:Get your glad rags on for the night of the year. Get down to Example Night Club. Starts 8.00pm. \r\n"
				 "PRIORITY:2\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				 "STATUS:DECLINED\r\n"
				 "DTSTART:20010418T120000Z\r\n"
				 "DTEND:20010418T130000Z\r\n"
				 "LOCATION:Lecture Hall 1\r\n"
				 "SUMMARY:Train Timetables Lecture \r\n"
				 "DESCRIPTION:An exciting journey through the history of railway timetables. Learn all about the intricacies and complexities of the science of creating a timetable. \r\n"
				 "PRIORITY:2\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VTODO\r\n"					// todo entry
				 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				 "X-EPOCTODOLIST:Business\r\n"
				 "STATUS:TENTATIVE\r\n"
				 "DESCRIPTION:to-do item with priority 1\r\n"
				 "CLASS:PUBLIC\r\n"
				 "DCREATED:20000113T000000Z\r\n"
				 "LAST-MODIFIED:20000113T115900Z\r\n"
				 "PRIORITY:1\r\n"
				"END:VTODO\r\n"
				"BEGIN:VTODO\r\n"					// todo entry
				 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				 "X-EPOCTODOLIST:business\r\n"
				 "STATUS:SENT\r\n"
				 "SUMMARY:to-do item with priority 2\r\n"
				 "CLASS:PUBLIC\r\n"
				 "DCREATED:20000113T000000Z\r\n"
				 "LAST-MODIFIED:20000113T115900Z\r\n"
				 "PRIORITY:2\r\n"
				"END:VTODO\r\n"
				"BEGIN:VTODO\r\n"					// Holiday todo entry - completed
				 "STATUS:COMPLETED\r\n"
				 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				 "X-EPOCTODOLIST:Holiday\r\n"
				 "SUMMARY:Book Holiday\r\n"
				 "DESCRIPTION:Need a holiday. Book somewhere nice and warm.\r\n"
				 "CLASS:PUBLIC\r\n"
				 "DCREATED:20010113T000000Z\r\n"
				 "COMPLETED:20010125T113000Z\r\n"
				 "LAST-MODIFIED:20000113T115900Z\r\n"
				 "PRIORITY:1\r\n"
				"END:VTODO\r\n"
				"BEGIN:VTODO\r\n"					// Holiday todo entry - uncompleted
				 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				 "X-EPOCTODOLIST:Holiday\r\n"
				 "STATUS:DELEGATED\r\n"
				 "DESCRIPTION:Pack Holiday clothes todo\r\n"
				 "CLASS:PUBLIC\r\n"
				 "DCREATED:20010420T000000Z\r\n"
				 "LAST-MODIFIED:20010313T115900Z\r\n"
				 "PRIORITY:3\r\n"
				"END:VTODO\r\n"
				"BEGIN:VTODO\r\n"					// todo entry
				 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				 "X-EPOCTODOLIST:Home\r\n"
				 "STATUS:NEEDS ACTION\r\n"
				 "SUMMARY:Gas Bill\r\n"
				 "DESCRIPTION:Pay this before they cut us off!\r\n"
				 "CLASS:PUBLIC\r\n"
				 "DCREATED:20000213T000000Z\r\n"
				 "LAST-MODIFIED:20000213T115900Z\r\n"
				 "PRIORITY:4\r\n"
				"END:VTODO\r\n"
				"BEGIN:VEVENT\r\n"
				 "STATUS:NEEDS ACTION\r\n"
				 "DTSTART:20010421T120000Z\r\n"
				 "DTEND:20010421T180000Z\r\n"
				 "LOCATION:Cardiff\r\n"
				 "DESCRIPTION:\r\n"  // This should not cause a problem!!
				 "SUMMARY: Football Match FINAL\r\n"
				 "PRIORITY:2\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				 "STATUS:NEEDS ACTION\r\n"
				 "DTSTART:20010421T120000Z\r\n"
				 "DTEND:20010421T180000Z\r\n"
				 "LOCATION:Meeting Room 1\r\n"
				 "SUMMARY:MIME Lecture\r\n"
				 "DESCRIPTION;ENCODING=QUOTED-PRINTABLE:"
				 "  MIME (Multipurpose Internet Mail Extensions) Part Two:=0D=0A"
				 "  Message Header Extensions for Non-ASCII Text=0D=0A"
				 "  Status of this Memo =0D=0A="
				 "  This RFC specifies an Internet standards track protocol for the Interne="
				 "  t community, and requests discussion and suggestions for improvements. ="
				 "  Please refer to the current edition of the Internet Official Protocol S="
				 "  tandards for the standardization state and status of this protocol.  Di="
				 "  stribution of this memo is unlimited.=0D=0AAbstract=0D=0A"
				 "  This memo describes an extension to the message format defined in RFC 1="
				 "  521 [1], to allow the representation of character sets other than ASCII="
				 "  in RFC 822 (STD 11) message headers.  The extensions described were des="
				 "  igned to be highly compatible with existing Internet mailhandling softw="
				 "  are, and to be easily implemented in mail readers that support RFC 1521="
				 "  .=0D=0A   1. Introduction =0D=0A"
				 "  RFC 1521 describes a mechanism for denoting textual body parts which are="
				 "  coded in various character sets, as well as methods for encoding such bo="
				 "  dy parts as sequences of printable ASCII characters.  This memo describe="
				 "  s similar techniques to allow the encoding of non-ASCII text in various ="
				 "  portions of a RFC 822 [2] message header, in a manner which is unlikely ="
				 "  to confuse existing message handling software. Like the encoding techniq="
				 "  ues described in RFC 1521, the techniques outlined here were designed to="
				 "  allow the use of non-ASCII characters in message headers in a way which ="
				 "  is unlikely to be disturbed by the quirks of existing Internet mail hand="
				 "  ling programs.  In particular, some mail relaying programs are known to ="
				 "  (a) delete some message header fields while retaining others, (b) rearra="
				 "  nge the order of addresses in To or Cc fields, (c) rearrange the (vertic="
				 "  al) order of header fields, and/or (d) \"wrap\" message headers at diffe="
				 "  rent places than those in the original message.  In addition, some mail ="
				 "  reading programs are known to have difficulty correctly parsing message=0D=0A"
				 "  headers which, while legal according to RFC 822, make use of backslash-q="
				 "  uoting to \"hide\" special characters such as \"<\", \",\", or \":\", or="
				 "  which exploit other infrequently-used features of that  specification.=0D=0A"
				 "  While it is unfortunate that these programs do not correctly interpret R="
				 "  FC 822 headers, to \"break\" these programs would cause severe operation="
				 "  al problems for the Internet mail system.  The extensions described in t="
				 "  his memo therefore do not rely on little- used features of RFC 822. Inst="
				 "  ead, certain sequences of \"ordinary\" printable ASCII characters (known="
				 "  as \"encoded-words\") are reserved for use as encoded data.  The syntax ="
				 "  of encoded-words is such that they are unlikely to \"accidentally\" appe="
				 "  ar as normal text in message headers. Furthermore, the characters used i="
				 "  n encoded-words are restricted to those which do not have special meanin="
				 "  gs in the context in which the encoded-word appears.Generally,an \"encod="
				 "  ed-word\" is a sequence of printable ASCII characters that begins with \"="
				 "  =? \", ends with \"?=\", and has two \"?\"s in between.  It specifies a ="
				 "  character set and an encoding method, and also includes the original tex="
				 "  t encoded as graphic ASCII characters, according to the rules for that e="
				 "  ncoding method.=0D=0A"
				 "  A mail composer that implements this specification will provide a means ="
				 "  of inputting non-ASCII text in header fields, but will translate these f="
				 "  ields (or appropriate portions of these fields) into encoded-words befor="
				 "  e inserting them into the message header. A mail reader that implements ="
				 "  this specification will recognize encoded-words when they appear in cert="
				 "  ain portions of the message header.  Instead of displaying the encoded-w="
				 "  ord \"as is\", it will reverse the encoding and display the original tex="
				 "  t in the designated character set.\r\n"
				 "PRIORITY:2\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				 "STATUS:NEEDS ACTION\r\n"
				 "DTSTART:20010421T120000Z\r\n"
				 "DTEND:20010421T180000Z\r\n"
				 "LOCATION:Random Restaurant\r\n"
				 "SUMMARY:Meal for 4\r\n"
				 "DESCREPTION:Book early to get a good table and reservation time.\r\n"
				 "PRIORITY:2\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KEntry19,	"BEGIN:VCALENDAR\r\n"
				"TZ:+01\r\n"
				"VERSION:1.0\r\n"
				"BEGIN:VEVENT\r\n"
		 		"UID:entry13 1 \r\n"
		 		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
			   	"SUMMARY:this entry is not repeating\r\n"
				"DESCRIPTION:this entry is not repeating\r\n"
				"DTSTART:19990101T100000Z\r\n"
				"DTEND:19991101T103000Z\r\n"
				"END:VEVENT\r\n"			
				"BEGIN:VEVENT\r\n"
		 		"UID:entry13 2\r\n"
		 		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
			   	"SUMMARY:entry with tz +1 start at 10\r\n"
				"DESCRIPTION:a description\r\n"
				"DTSTART:19990101T100000\r\n"
				"DTEND:19991101T103000\r\n"
 				"RRULE:D1 20001101T000000\r\n"
				"TRANSP:2\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				"BEGIN:VCALENDAR\r\n"
				"TZ:+02\r\n"
				"BEGIN:VEVENT\r\n"
				"UID:entry13 3\r\n"
				"SUMMARY:entry with tz +2 start at 14\r\n"
				"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
				"DTSTART:19990101T140000\r\n"
				"DTEND:19991101T143000\r\n"
				"RRULE:YM1 1 20001101T000000\r\n"
				"AALARM:19991101T090000\r\n"
				"LAST-MODIFIED:19991101T124400\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				"BEGIN:VCALENDAR\r\n"
				"BEGIN:VEVENT\r\n"
				"UID: entry13 4\r\n"
			   	"SUMMARY:floating entry start at 11\r\n"
				"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
				"DTSTART:19990101T110000\r\n"
				"DTEND:19990101T120000\r\n"
				"RRULE:YM1 1 20001101T000000\r\n"
				"LAST-MODIFIED:19991101T124400\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

// used to test that the last day of the month is set correctly
// The Uids must match the ones in KLastDay!
_LIT8(KLastDayUID1, "111");
_LIT8(KLastDayUID2, "112");

// the following vcal contains 2 entries
const TInt KLastDayNumber = 2;

_LIT8(KLastDay,	"BEGIN:VCALENDAR\r\n"
				 "VERSION:1.0\r\n"
				"BEGIN:VEVENT\r\n"
				 "UID:111\r\n"
				 "DTEND:20050630T100000\r\n"	 
				 "DTSTART:20050630T090000\r\n"	//Repeat on last day of every month starting from 30th June
				 "LOCATION:India\r\n"
				 "RRULE:MD1 1- 20060630T100000\r\n"			// Frequency Modifier LD used here
				 "SUMMARY:Meeting-Recurrence\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				 "UID:112\r\n"
				 "DTEND:20070630T100000\r\n"	 
				 "DTSTART:20070630T090000\r\n"	//Repeat on last day of every month starting from 30th June
				 "LOCATION:India\r\n"
				 "RRULE:MD1 1- 20080630T100000\r\n"			// Frequency Modifier 1- used here
				 "SUMMARY:Meeting-Recurrence\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KFloatingTodo,	"BEGIN:VCALENDAR\r\n"
						"VERSION:1.0\r\n"
						"BEGIN:VTODO\r\n"
						"UID:4\r\n"
						"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
						"DTSTART:19991101T100000\r\n"
						"DUE:19991101T113000\r\n"
						"AALARM:19991101T090000\r\n"
						"LAST-MODIFIED:19991101T124400\r\n"
						"END:VTODO\r\n"
						"END:VCALENDAR\r\n"
						);

_LIT8(KFixedTodo,	"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"TZ:+02\r\n"
					"BEGIN:VTODO\r\n"
					"UID:4\r\n"
					"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:19991101T100000\r\n"
					"DUE:19991101T113000\r\n"
					"AALARM:19991101T090000\r\n"
					"LAST-MODIFIED:19991101T124400\r\n"
					"END:VTODO\r\n"
					"END:VCALENDAR\r\n"
					);

_LIT8(KFloatingTodoWithoutDueDate,
					"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"BEGIN:VTODO\r\n"
					"UID:6\r\n"
					"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:19991101T100000\r\n"
					"END:VTODO\r\n"
					"END:VCALENDAR\r\n"
					);

_LIT8(KFixedTodoWithoutDueDate,
					"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"TZ:+02\r\n"
					"BEGIN:VTODO\r\n"
					"UID:6\r\n"
					"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:19991101T100000Z\r\n"
					"END:VTODO\r\n"
					"END:VCALENDAR\r\n"
					);

_LIT8(KFloatingAlarmedTodoWithoutDueDate,
					"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"BEGIN:VTODO\r\n"
					"UID:6\r\n"
					"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:19991101T100000\r\n"
					"AALARM:19991101T102500\r\n"
					"END:VTODO\r\n"
					"END:VCALENDAR\r\n"
					);

_LIT8(KFixedAlarmedTodoWithoutDueDate,
					"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"TZ:+02\r\n"
					"BEGIN:VTODO\r\n"
					"UID:6\r\n"
					"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:19991101T100000Z\r\n"
					"AALARM:19991101T102500Z\r\n"
					"END:VTODO\r\n"
					"END:VCALENDAR\r\n"
					);

_LIT8(KEntriesWithoutDates,
					"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:4\r\n"
					"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
					"END:VEVENT\r\n"
					"BEGIN:VTODO\r\n"
					"UID:6\r\n"
					"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"END:VTODO\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:5\r\n"
					"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
					"END:VEVENT\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:3\r\n"
					"X-EPOCAGENDAENTRYTYPE:REMINDER\r\n"
					"END:VEVENT\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:2\r\n"
					"X-EPOCAGENDAENTRYTYPE:ANNIVERSARY\r\n"
					"END:VEVENT\r\n"
					"END:VCALENDAR\r\n"
					);
					
_LIT8(KFloatingException,
					"BEGIN:VCALENDAR\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:todoparententry@example.com\r\n"
					"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
					"DTSTART:20050516T133000\r\n"
					"DTEND:20050516T143000\r\n"
					"RRULE:D1 20050525T143000\r\n"
					"EXDATE:20050517T133000\r\n"
					"LAST-MODIFIED:20060213T105113\r\n"
					"END:VEVENT\r\n"
					"END:VCALENDAR\r\n"
					);

_LIT8(KTodoCompletedDate,
					"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"BEGIN:VTODO\r\n"
					"UID:5\r\n"
					"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
					"DTSTART:19991101T100000\r\n"
					"DUE:19991101T113000\r\n"
					"AALARM:19991101T090000\r\n"
					"COMPLETED:19991101T080000\r\n"
					"LAST-MODIFIED:19991101T124400\r\n"
					"END:VTODO\r\n"
					"END:VCALENDAR\r\n"
					);

_LIT8(KEntry6, "BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"TZ:+0200\r\n"
				"BEGIN:VTODO\r\n"
				"UID:15\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTSTART:19991101T104500\r\n"
				"DUE:19991101T113000\r\n"
				"AALARM:19991101T092500\r\n"
				"LAST-MODIFIED:19991101T124400\r\n"
				"END:VTODO\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KEntry6a, "BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"TZ:+0200\r\n"
				"BEGIN:VTODO\r\n"
				"UID:15\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTSTART:19991101T104500\r\n"
				"DUE:19991101T113000\r\n"
				"X-METHOD:NONE\r\n"
				"X-EPOCALARM:19990101T104000\r\n"
				"LAST-MODIFIED:19991101T124400\r\n"
				"END:VTODO\r\n"
				"END:VCALENDAR\r\n"

				"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"TZ:+0200\r\n"
				"BEGIN:VTODO\r\n"
				"UID:14\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTSTART:19991001T111000\r\n"
				"DUE:    19991001T113200\r\n"
				"LAST-MODIFIED:19991101T113000\r\n"
				"END:VTODO\r\n"
				"END:VCALENDAR\r\n"

				"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"BEGIN:VTODO\r\n"
				"UID;ENCODING=QUOTED-PRINTABLE:20060904T120610-techview_agenda=40example.com\r\n"
				"SUMMARY:qwerqwerq\r\n"
				"DUE:20060910T230000Z\r\n"
				"X-EPOCTODOLIST:TODO\r\n"
				"STATUS:NEEDS ACTION\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"CLASS:PUBLIC\r\n"
				"SEQUENCE:0\r\n"
				"X-METHOD:NONE\r\n"
				"ATTENDEE;ROLE=ORGANIZER;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI;ENCODING=QUOTED-PRINTABLE:=\r\n"
				"techview_agenda=40example.com\r\n"
				"AALARM;TYPE=X-EPOCSOUND:20060910T230000Z;;0;Rings\r\n"
				"LAST-MODIFIED:20060904T120644Z\r\n"
				"PRIORITY:1\r\n"
				"X-SYMBIAN-LUID:5\r\n"
				"END:VTODO\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8(KEntry6b, "BEGIN:VCALENDAR\r\n" 
				"VERSION:1.0\r\n" 
				"BEGIN:VEVENT\r\n" 
				"UID:todoparententryanish@example.com\r\n" 
				"DTSTART:20050516T133000\r\n" 
				"DTEND:20050516T143000\r\n" 
				"RRULE:D1 20050528T143000\r\n" 
				"X-EPOCAGENDAENTRYTYPE:EVENT\r\n" 
				"LAST-MODIFIED:20060213T105113Z\r\n" 
				"EXDATE:20050519T133000\r\n" 
				"RDATE:20050529T133000\r\n" 
				"END:VEVENT\r\n" 
				"END:VCALENDAR\r\n" 
				
				"BEGIN:VCALENDAR\r\n" 
				"VERSION:1.0\r\n" 
				"BEGIN:VEVENT\r\n" 
				"UID:todoparententryanish@example.com\r\n" 
				"DTSTART:20050526T133000\r\n" 
				"DTEND:20050526T143000\r\n" 
				"X-RECURRENCE-ID:20050528T133000\r\n" 
				"RRULE:D1 20050529T133000\r\n" 
				"LAST-MODIFIED:20060213T105113Z\r\n" 
				"X-EPOCAGENDAENTRYTYPE:EVENT\r\n" 
				"X-SYMBIAN-LUID:5\r\n" 
				"END:VEVENT\r\n" 
				"END:VCALENDAR\r\n" 
			    );

_LIT8(KEntry7, "BEGIN:VCALENDAR\r\n"
				"VERSION:0.1\r\n"
				"TZ:a1\r\n"
				"BEGIN:VTODO\r\n"
				"UID:13\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTSTART:19991101T100000\r\n"
				"DUE:19991101T113000\r\n"
				"AALARM:19991101T090000\r\n"
				"LAST-MODIFIED:19991101T124400\r\n"
				"END:VTODO\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KEntry8, "BEGIN:VCALENDAR\r\n"
 			   "VERSION:1.0\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:REMINDER\r\n"
 			   "DTSTART:19991101T100001\r\n"
 			   "DTEND:20001101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:3\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:ANNIVERSARY\r\n"
 			   "DTSTART:19991101T100002\r\n"
 			   "DTEND:20011101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "END:VCALENDAR\r\n"
 			   "BEGIN:VCALENDAR\r\n"
 			   "VERSION:1.0\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:4\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:REMINDER\r\n"
 			   "DTSTART:19991101T100003\r\n"
 			   "DTEND:20021101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:5\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
 			   "DTSTART:19991101T100004\r\n"
 			   "DTEND:20031101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "END:VCALENDAR\r\n"
 			   );
   
_LIT8(KEntry9, "BEGIN:VCALENDAR\r\n"
 			   "VERSION:1.0\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
 			   "DTSTART:19991101T100001\r\n"
 			   "DTEND:20001101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:REMINDER\r\n"
 			   "DTSTART:19991101T100002\r\n"
 			   "DTEND:20011101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "END:VCALENDAR\r\n"
 			   "BEGIN:VCALENDAR\r\n"
 			   "VERSION:1.0\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:ANNIVERSARY\r\n"
 			   "DTSTART:19991101T100003\r\n"
 			   "DTEND:20021101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
 			   "DTSTART:19991101T100004\r\n"
 			   "DTEND:20031101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "END:VCALENDAR\r\n"
 			   );
   
_LIT8(KEntry10, "BEGIN:VCALENDAR\r\n"
			   "VERSION:1.0\r\n"
			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
 			   "DTSTART:19991101T100001\r\n"
 			   "DTEND:20001101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:REMINDER\r\n"
 			   "DTSTART:19991101T100002\r\n"
 			   "DTEND:20011101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "END:VCALENDAR\r\n"
 			   "BEGIN:VCALENDAR\r\n"
 			   "VERSION:1.0\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:REMINDER\r\n"
 			   "DTSTART:19991101T100003\r\n"
 			   "DTEND:20021101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "BEGIN:VEVENT\r\n"
 			   "UID:2\r\n"
 			   "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
 			   "DTSTART:19991101T100004\r\n"
 			   "DTEND:20031101T113000\r\n"
 			   "AALARM:19991101T090000\r\n"
 			   "LAST-MODIFIED:19991101T124400\r\n"
 			   "END:VEVENT\r\n"
 			   "END:VCALENDAR\r\n"
 			   );
 
_LIT8(KEntry11,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"BEGIN:VEVENT\r\n"
				"UID:2\r\n"
				"X-SYMBIAN-LUID:192837\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"AALARM:19991101\r\n"
				"LAST-MODIFIED:19991191T124400\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				"X-SYMBIAN-LUID:192830\r\n"
				"UID:3\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTSTART:19991101\r\n"
				"DTEND:19991101T010203\r\n"
				"AALARM:19991101\r\n"
				"LAST-MODIFIED:19991191T124400\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				"X-SYMBIAN-LUID:192838\r\n"
				"UID:4\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTSTART:19991101\r\n"
				"AALARM:19991101\r\n"
				"LAST-MODIFIED:19991191T124400\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				"X-SYMBIAN-LUID:192839\r\n"
				"UID:5\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTEND:19991101T010203\r\n"
				"AALARM:19991101\r\n"
				"LAST-MODIFIED:19991191T124400\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n");

_LIT8(KEntry12,	"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"BEGIN:VEVENT\r\n"
				"UID:test1\r\n"
				"DTSTART:20050513T090000\r\n"
				"DTEND:20050513T090000\r\n"
				"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
				"CLASS:PUBLIC\r\n"
				"SEQUENCE:0\r\n"
				"X-METHOD:REQUEST\r\n"
				"AALARM;TYPE=X-EPOCSOUND:20050513T074000;;0;\r\n"
				"LAST-MODIFIED:20060206T231148Z\r\n"
				"PRIORITY:0\r\n"
				"X-SYMBIAN-LUID:25\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
				
_LIT8( KUpdatedParentVCalendar,
				"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"TZ:+02\r\n"
				"DAYLIGHT:TRUE;+03;20060326T030000;20061029T040000;+02;+03\r\n"
				"BEGIN:VEVENT\r\n"
				"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
				"SUMMARY:Corruption bug\r\n"
				"DTSTART:20060213T180000Z\r\n"
				"DTEND:20060213T183000Z\r\n"
				"CLASS:PRIVATE\r\n"
				"LOCATION:PARENT\r\n"
				"SEQUENCE:0\r\n"
				"X-METHOD:NONE\r\n"
				"RRULE:D1 20060219T000000\r\n"
				"EXDATE:20060214T000000;20060215T000000\r\n"
				"LAST-MODIFIED:20060213T133601Z\r\n"
				"PRIORITY:2\r\n"
				"STATUS:CONFIRMED\r\n"
				"UID:testCorruptionBugL\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n");

_LIT8(KExceptionEntry, 
				"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"TZ:+01\r\n"
				"BEGIN:VEVENT\r\n"
				"UID:5\r\n"
				"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
				"DTSTART:19990101T100000\r\n"
				"DTEND:19990101T113000\r\n"
				"RRULE:D1 #4\r\n"
				"RDATE:19990105T120000;18990106T090000\r\n"
				"EXDATE:19990101T100000;19990102T100000;18990102T100000\r\n"
				"LAST-MODIFIED:19991101T124400\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n");

 // Only start date specified.
_LIT8( KEntry13, "BEGIN:VCALENDAR\r\n"
			     "VERSION:1.0\r\n"
                 "BEGIN:VTODO\r\n"
                 "UID:13\r\n"
                 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
                 "DTSTART:20051107T060000Z\r\n"
                 "END:VTODO\r\n"
                 "END:VCALENDAR\r\n" );

// Start and due date specified. End date should = due date.
_LIT8( KEntry14, "BEGIN:VCALENDAR\r\n"
			     "VERSION:1.0\r\n"
                 "BEGIN:VTODO\r\n"
                 "UID:14\r\n"
                 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
                 "DTSTART:20051007T060000Z\r\n"
 			     "DUE:20051208T063000Z\r\n"
                 "END:VTODO\r\n"
                 "END:VCALENDAR\r\n" );

// Only the end date is specified. Start and due date should = end date.
_LIT8( KEntry15, "BEGIN:VCALENDAR\r\n"
			     "VERSION:1.0\r\n"
                 "BEGIN:VTODO\r\n"
                 "UID:15\r\n"
                 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
 			     "DTEND:20051107T060000Z\r\n"
                 "END:VTODO\r\n"
                 "END:VCALENDAR\r\n" );

// Only the due date is specified. Start and end date should = due date.
_LIT8( KEntry16, "BEGIN:VCALENDAR\r\n"
			     "VERSION:1.0\r\n"
                 "BEGIN:VTODO\r\n"
                 "UID:16\r\n"
                 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
 			     "DUE:20051107T060000Z\r\n"
                 "END:VTODO\r\n"
                 "END:VCALENDAR\r\n" );

// Neither the start, end, or due date are specified. Start and end date should
// be null and the repeat rull should be ignored.
_LIT8( KEntry17, "BEGIN:VCALENDAR\r\n"
			     "VERSION:1.0\r\n"
                 "BEGIN:VTODO\r\n"
                 "UID:17\r\n"
                 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
                 "RRULE:W1 TU #0\r\n"
                 "END:VTODO\r\n"
                 "END:VCALENDAR\r\n" );

// Used to test whether roles/x-roles and status/x-status can be converted back
// and forth properly. 
_LIT8( KEntry18, "BEGIN:VCALENDAR\r\n"
                 "VERSION:1.0\r\n"
                 "BEGIN:VEVENT\r\n"
                 "UID:18\r\n"
                 "SUMMARY:Summary 1\r\n"
                 "DESCRIPTION:Desciption 1\r\n"
                 "DTSTART:19990101T100000\r\n"
                 "DTEND:19991101T113000\r\n"
                 "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
                 "CLASS:CONFIDENTIAL\r\n"
                 "LOCATION:Location 1\r\n"
                 "SEQUENCE:0\r\n"
                 "X-METHOD:REQUEST\r\n"
                 "ATTENDEE;ROLE=ORGANIZER;STATUS=DECLINED:Person A\r\n"
                 "ATTENDEE;ROLE=OWNER;STATUS=DELEGATED:Person B\r\n"
                 "ATTENDEE;ROLE=ATTENDEE;STATUS=NEEDS ACTION:Person C\r\n"
                 "ATTENDEE;ROLE=DELEGATE;STATUS=DECLINED:Another Person\r\n"
			     "ATTENDEE;STATUS=DELEGATED;RSVP=NO;EXPECT=FYI;X-ROLE=CHAIR;X-STATUS=DELEGATED:Person B\r\n"
                 "ATTENDEE;ROLE=ATTENDEE;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI;X-ROLE=REQ-PARTICIPANT;X-STATUS=NEEDS-ACTION:Person C\r\n"
                 "ATTENDEE;STATUS=DECLINED;RSVP=NO;EXPECT=FYI;X-ROLE=REQ-PARTICIPANT;X-STATUS=DECLINED:Another Person\r\n"
                 "RRULE:D1 20001101T100000\r\n"
                 "AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:19990101T090000;;0;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
                 "LAST-MODIFIED:19991101T124400Z\r\n"
                 "CATEGORIES:APPOINTMENT;EDUCATION;BUSINESS;MISCELLANEOUS;HOLIDAY;MEETING;PERSONAL;PHONECALL;SICKDAY;SPECIALOCCASION;TRAVEL;VACATION\r\n"
                 "PRIORITY:0\r\n"
                 "STATUS:CONFIRMED\r\n"
                 "X-SYMBIAN-LUID:0\r\n"
                 "END:VEVENT\r\n"
                 "END:VCALENDAR\r\n" );

 _LIT8(KEntry18a,"BEGIN:VCALENDAR\r\n"
                "TZ:+03\r\n"
                "VERSION:1.0\r\n"			
                "BEGIN:VEVENT\r\n"
                "UID:13UID1\r\n"
                "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
                "SEQUENCE:5\r\n"
               	"SUMMARY:first entry\r\n"
                "DESCRIPTION:a description\r\n"
                "DTSTART:20050411T100000Z\r\n"
                "DTEND:20080411T120000Z\r\n"
                "CATEGORIES:APPOINTMENT\r\n"
                "RRULE:MP1 #100\r\n"
                "ATTENDEE;ROLE=ATTENDEE:\r\n"				
                "X-METHOD:REQUEST\r\n"
                "END:VEVENT\r\n"
                "END:VCALENDAR\r\n"
                "BEGIN:VCALENDAR\r\n"
                "TZ:+01\r\n"
                "VERSION:1.0\r\n"			
                "BEGIN:VEVENT\r\n"
                "UID:13UID2\r\n"
                "SEQUENCE:5\r\n"
               	"SUMMARY:second entry\r\n"
                "DESCRIPTION:a description\r\n"
                "DTSTART:20050411T100000Z\r\n"
                "DTEND:20080411T120000Z\r\n"
                "CATEGORIES:APPOINTMENT\r\n"
                "ATTENDEE;ROLE=OWNER:\r\n"				
                "END:VEVENT\r\n"
                "END:VCALENDAR\r\n"
                );

 _LIT8(KEntry18b,"BEGIN:VCALENDAR\r\n"
                "TZ:+03\r\n"
                "VERSION:1.0\r\n"			
                "BEGIN:VEVENT\r\n"
                "UID:13UID1\r\n"
                "X-SYMBIAN-DTSTAMP:20050411T200000Z\r\n"
                "SEQUENCE:5\r\n"
               	"SUMMARY:first modified entry\r\n"
                "DESCRIPTION:a description\r\n"
                "DTSTART:20050411T200000Z\r\n"
                "DTEND:20080411T130000Z\r\n"
                "CATEGORIES:APPOINTMENT\r\n"
                "RRULE:MP1 #100\r\n"
                "ATTENDEE;ROLE=ATTENDEE:\r\n"				
                "X-METHOD:REQUEST\r\n"
                "END:VEVENT\r\n"
                "END:VCALENDAR\r\n"
                "BEGIN:VCALENDAR\r\n"
                "TZ:+01\r\n"
                "VERSION:1.0\r\n"			
                "BEGIN:VEVENT\r\n"
                "UID:13UID2\r\n"
                "SEQUENCE:5\r\n"
               	"SUMMARY:second modified entry\r\n"
                "DESCRIPTION:a description\r\n"
                "DTSTART:20050411T200000Z\r\n"
                "DTEND:20080411T320000Z\r\n"
                "CATEGORIES:APPOINTMENT\r\n"
                "ATTENDEE;ROLE=OWNER:\r\n"				
                "END:VEVENT\r\n"
                "END:VCALENDAR\r\n"
                );
                
// fixed entries in positive time zone
_LIT8( KUntilDateCheck1,        // UTC until date
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "TZ:+02\r\n"
       "DAYLIGHT:TRUE;+03;20060326T030000;20061029T040000;+02;+03\r\n"
       "BEGIN:VEVENT\r\n"
       "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
       "DTSTART:20060213T180000Z\r\n"
       "DTEND:20060213T183000Z\r\n"
       "SEQUENCE:0\r\n"
       "X-METHOD:NONE\r\n"
       "RRULE:D1 20060619T000000Z\r\n"
       "UID:UntilDateCheck\r\n"
       "END:VEVENT\r\n"
       "END:VCALENDAR\r\n"
    );

_LIT8( KUntilDateCheck2,        // local until date
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "TZ:+02\r\n"
       "DAYLIGHT:TRUE;+03;20060326T030000;20061029T040000;+02;+03\r\n"
       "BEGIN:VEVENT\r\n"
       "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
       "DTSTART:20060213T180000Z\r\n"
       "DTEND:20060213T183000Z\r\n"
       "SEQUENCE:0\r\n"
       "X-METHOD:NONE\r\n"
       "RRULE:D1 20060619T000000\r\n"
       "UID:UntilDateCheck\r\n"
       "END:VEVENT\r\n"
       "END:VCALENDAR\r\n"
    );

// fixed entries in negative time zone
_LIT8( KUntilDateCheck3,        // UTC until date
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "TZ:-03\r\n"
       "DAYLIGHT:TRUE;-02;20060326T030000;20061029T040000;-03;-02\r\n"
       "BEGIN:VEVENT\r\n"
       "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
       "DTSTART:20060213T000000Z\r\n"
       "DTEND:20060213T000000Z\r\n"
       "SEQUENCE:0\r\n"
       "X-METHOD:NONE\r\n"
       "RRULE:D1 20060619T000000Z\r\n"
       "UID:UntilDateCheck\r\n"
       "END:VEVENT\r\n"
       "END:VCALENDAR\r\n"
    );

_LIT8( KUntilDateCheck4,        // local until date
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "TZ:-03\r\n"
       "DAYLIGHT:TRUE;-02;20060326T030000;20061029T040000;-03;-02\r\n"
       "BEGIN:VEVENT\r\n"
       "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
       "DTSTART:20060213T000000Z\r\n"
       "DTEND:20060213T000000Z\r\n"
       "SEQUENCE:0\r\n"
       "X-METHOD:NONE\r\n"
       "RRULE:D1 20060619T000000\r\n"
       "UID:UntilDateCheck\r\n"
       "END:VEVENT\r\n"
       "END:VCALENDAR\r\n"
    );

// UTC entries in unspecified time zone
_LIT8( KUntilDateCheck5,        // UTC until date
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "BEGIN:VEVENT\r\n"
       "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
       "DTSTART:20060213T180000Z\r\n"
       "DTEND:20060213T183000Z\r\n"
       "SEQUENCE:0\r\n"
       "X-METHOD:NONE\r\n"
       "RRULE:D1 20060619T000000Z\r\n"
       "UID:UntilDateCheck\r\n"
       "END:VEVENT\r\n"
       "END:VCALENDAR\r\n"
    );

_LIT8( KUntilDateCheck6,        // local until date
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "BEGIN:VEVENT\r\n"
       "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
       "DTSTART:20060213T180000Z\r\n"
       "DTEND:20060213T183000Z\r\n"
       "SEQUENCE:0\r\n"
       "X-METHOD:NONE\r\n"
       "RRULE:D1 20060619T000000\r\n"
       "UID:UntilDateCheck\r\n"
       "END:VEVENT\r\n"
       "END:VCALENDAR\r\n"
    );

// floating entries
_LIT8( KUntilDateCheck7,        // UTC until date
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "BEGIN:VEVENT\r\n"
       "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
       "DTSTART:20060213T233000\r\n"
       "DTEND:20060213T233000\r\n"
       "SEQUENCE:0\r\n"
       "X-METHOD:NONE\r\n"
       "RRULE:D1 20060619T000000Z\r\n"
       "UID:UntilDateCheck\r\n"
       "END:VEVENT\r\n"
       "END:VCALENDAR\r\n"
    );

_LIT8( KUntilDateCheck8,        // local until date
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "BEGIN:VEVENT\r\n"
       "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
       "DTSTART:20060213T233000\r\n"
       "DTEND:20060213T233000\r\n"
       "SEQUENCE:0\r\n"
       "X-METHOD:NONE\r\n"
       "RRULE:D1 20060619T000000\r\n"
       "UID:UntilDateCheck\r\n"
       "END:VEVENT\r\n"
       "END:VCALENDAR\r\n"
    );

_LIT8( KAlarmedAnnivVCalendar, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VEVENT\r\n"
		"X-EPOCAGENDAENTRYTYPE:ANNIVERSARY\r\n"
		"SUMMARY;QUOTED-PRINTABLE:toinen yritys vuosittain toistuva 8.4. alarm at 7.4 6:00\r\n"
		"DTSTART:20060408T000000\r\n"
		"DTEND:20060409T000000\r\n"
		"STATUS:CONFIRMED\r\n"
		"CLASS:PUBLIC\r\n"
		"AALARM:20060407T060000;;;\r\n"
		"RRULE:YM1 4 #0\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
	);

_LIT8(KFloatingRecId,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:todoparententry@example.com\r\n"
	"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
	"DTSTART:20050516T133000\r\n"
	"DTEND:20050516T143000\r\n"
	"RRULE:D1 20050525T143000\r\n"
	"EXDATE:20050519T133000\r\n"
	"LAST-MODIFIED:20060213T105113\r\n"
	"END:VEVENT\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:todoparententry@example.com\r\n"
	"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
	"DTSTART:20050527T133000\r\n"
	"DTEND:20050527T143000\r\n"
	"X-RECURRENCE-ID:20050517T133000\r\n"
	"AALARM:20050527T123000\r\n"
	"X-SYMBIAN-DTSTAMP:20050901T130000\r\n"
	"LAST-MODIFIED:20060213T105113\r\n"
	"END:VEVENT\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:todoparententry@example.com\r\n"
	"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
	"DTSTART:20050528T143000\r\n"
	"DTEND:20050528T153000\r\n"
	"X-RECURRENCE-ID:20050518T133000\r\n"
	"AALARM:20050528T123000\r\n"
	"X-SYMBIAN-DTSTAMP:20050901T130000\r\n"
	"LAST-MODIFIED:20060213T105113\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);

_LIT8(KMonthlyRepeatDate,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"BEGIN:VEVENT\r\n"
	"CATEGORIES:MEETING\r\n"
	"DTSTART:20060322T080000\r\n"
	"CLASS:PUBLIC\r\n"
	"DTEND:20060322T090000\r\n"
	"RRULE:MD1 20070322T090000\r\n"
	"DESCRIPTION:Sgh-d500\r\n"
	"SUMMARY:Rec\r\n"
	"PRIORITY:0\r\n"
	"X-IRMC-LUID:51\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);

_LIT8(KEntry20,	"BEGIN:VCALENDAR\r\n"
                "TZ:+03\r\n"
                "VERSION:1.0\r\n"			
                "BEGIN:VEVENT\r\n"
                "UID:20\r\n"
                "SEQUENCE:1\r\n"
               	"SUMMARY:Parent entry\r\n"
                "DESCRIPTION:a description\r\n"
                "DTSTART:20060411T100000Z\r\n"
                "DTEND:20060411T120000Z\r\n"
                "RRULE:D1 #10\r\n"
                "EXDATE:20060413\r\n"
                "END:VEVENT\r\n"
                "BEGIN:VEVENT\r\n"
                "UID:20\r\n"
                "X-RECURRENCE-ID:20060413\r\n"
                "SEQUENCE:2\r\n"
               	"SUMMARY:Child entry\r\n"
                "DESCRIPTION:a description for child\r\n"
                "DTSTART:20060413T140000Z\r\n"
                "DTEND:20060413T150000Z\r\n"
                "END:VEVENT\r\n"
                "END:VCALENDAR\r\n"
                );
                
_LIT8(KRRuleEndDateTest,
				"BEGIN:VCALENDAR\r\n"					// used by TestRRuleEndDate()
				"VERSION:1.0\r\n"
				"%S"									// "TZ:+01\r\n"
				"BEGIN:VEVENT\r\n"
				"UID:5\r\n"
				"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
				"DTSTART:19990101T120000%c\r\n"
				"DTEND:19991101T130000%c\r\n"
				"RRULE:D1 20001101T100000%c\r\n"		// RRULE with enddate	
				"RDATE:20001102T120000\r\n"				// RDATE outside RRULE's range
				"AALARM:19991101T090000\r\n"
				"LAST-MODIFIED:19991101T124400\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);  

_LIT8(KEntry21, "BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"TZ:+02\r\n"
				"DAYLIGHT:TRUE;+03;20060326T010000Z;20061029T010000Z\r\n"
				"BEGIN:VEVENT\r\n"
				"UID:newNonSuppRecEntry\r\n"
				"SUMMARY:rdated entry\r\n"
				"DTSTART:20060410T144724Z\r\n"
				"DTEND:20060410T154724Z\r\n"
				"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
				"CLASS:PUBLIC\r\n"
				"SEQUENCE:0\r\n"
				"X-METHOD:NONE\r\n"
				"RDATE:20060410T144724Z;20060411T144724Z;20060412T144724Z\r\n"
				"20060413T144724Z\r\n"
				"LAST-MODIFIED:20060410T144743Z\r\n"
				"PRIORITY:0\r\n"
				"X-SYMBIAN-LUID:48\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KEntry22, "BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
		        "BEGIN:VTODO\r\n"
		        "UID:22\r\n"
		        "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
		        "DTSTART:19991102T010000\r\n"
		        "AALARM:19991102T020100\r\n"
		        "LAST-MODIFIED:19991102T124400\r\n"
		        "END:VTODO\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KEntryMailto,	"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
	                "BEGIN:VEVENT\r\n"
					"CATEGORIES:APPOINTMENT\r\n"
					"STATUS:NEEDS ACTION\r\n"
					"DTSTART:20000111T180000\r\n"
					"SUMMARY:This is an entry with mailto for the attendees addresses\r\n"
					"DESCRIPTION:make sure they are mixed case\r\n"
					"ATTENDEE;ROLE=OWNER;STATUS=CONFIRMED:mailto:someaddy@example.com;\r\n"
					"ATTENDEE;ROLE=ATTENDEE;STATUS=DECLINED:MAILTO:me@example-home.com;\r\n"
					"ATTENDEE;ROLE=ATTENDEE;X-PHONEOWNER:Mailto:someone.else@example-away.com\r\n"
					"UID:9000i-131097-0\r\n"
					"END:VEVENT\r\n"
					"END:VCALENDAR\r\n"
					);

_LIT8( KParentChildVCalEventWithTz,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "TZ:+01\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050527T133000\r\n"
        "DTEND:20050527T143000\r\n"
        "RRULE:D1 20050610T133000\r\n"
        "RDATE:20050611T153000;20050612T090000\r\n"
        "EXDATE:20050528T133000;20050529T133000\r\n"
        "X-RECURRENCE-ID;RANGE=THISANDFUTURE:20050517T133000\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KParentChildVCalEventNoTz,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050527T133000\r\n"
        "DTEND:20050527T143000\r\n"
        "RRULE:D1 20050610T133000\r\n"
        "RDATE:20050611T153000;20050612T090000\r\n"
        "EXDATE:20050528T133000;20050529T133000\r\n"
        "X-RECURRENCE-ID;RANGE=THISANDFUTURE:20050517T133000\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KParentChildVCalEventEqualBeforeAndAfterRecId,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050520T133000\r\n"
        "DTEND:20050520T143000\r\n"
        "RRULE:D1 20050522T133000\r\n"
        "X-RECURRENCE-ID:20050521T133000\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KParentChildVCalOverlappingChildHeadTrimmed,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050520T133000\r\n"
        "DTEND:20050520T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "X-RECURRENCE-ID:20050524T133000\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KParentChildVCalOverlappingChildTailTrimmed,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050520T133000\r\n"
        "DTEND:20050520T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "RDATE:20050611T153000\r\n"
        "X-RECURRENCE-ID:20050524T133000\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KParentChildVCalNonOverlappingChildTailTrimmed,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050527T133000\r\n"
        "DTEND:20050527T143000\r\n"
        "RRULE:D1 20050529T133000\r\n"
        "X-RECURRENCE-ID:20050524T133000\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KParentChildVCalNonOverlappingChildHeadTrimmed,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050510T133000\r\n"
        "DTEND:20050510T143000\r\n"
        "RRULE:D1 20050515T133000\r\n"
        "X-RECURRENCE-ID:20050524T133000\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");
         
_LIT8( KParentChildVCalNonOverlappingChildRecIdIsParentRDate,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "RDATE:20050528T153000\r\n"        
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050510T133000\r\n"
        "DTEND:20050510T143000\r\n"
        "RRULE:D1 20050515T133000\r\n"
        "X-RECURRENCE-ID:20050528T153000\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");        

_LIT8( KParentChildVCalNonOverlappingChildDateOnlyRecIdIsParentHead,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050510T133000\r\n"
        "DTEND:20050510T143000\r\n"
        "RRULE:D1 20050515T133000\r\n"
        "X-RECURRENCE-ID:20050516\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KParentChildVCalNonOverlappingChildDateOnlyRecIdIsParentTail,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "BEGIN:VEVENT\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050510T133000\r\n"
        "DTEND:20050510T143000\r\n"
        "RRULE:D1 20050515T133000\r\n"
        "X-RECURRENCE-ID:20050525\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KParentChildVCalNonOverlappingChildDateOnlyRecIdHeadTrimmed,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VTODO\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050516T133000\r\n"
        "DTEND:20050516T143000\r\n"
        "RRULE:D1 20050525T133000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VTODO\r\n"
        "BEGIN:VTODO\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
        "DTSTART:20050510T133000\r\n"
        "DTEND:20050510T143000\r\n"
        "RRULE:D1 20050515T133000\r\n"
        "X-RECURRENCE-ID:20050518\r\n"
        "AALARM:20050527T123000\r\n"
        "X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
        "LAST-MODIFIED:20060213T105113\r\n"
        "END:VTODO\r\n"
        "END:VCALENDAR\r\n");
        
        
_LIT8( KBadDaylightRulePanic,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"TZ:+01\r\n"
	"DAYLIGHT:FALSE;+02;20050326T010000Z;20051029T010000Z;;\r\n"
	"BEGIN:VTODO\r\n"
	"UID:todoparententryanish1@example.com\r\n"
	"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
	"DTSTART:20050516T133000\r\n"
	"DUE:20050516T143000\r\n"
	"RRULE:D1 20050525T133000\r\n"
	"COMPLETED:20070401T080000\r\n"
	"AALARM:20050516T133000\r\n"
	"LAST-MODIFIED:20060213T105113Z\r\n"
	"X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
	"END:VTODO\r\n"
	"END:VCALENDAR\r\n"
	);
	
_LIT8( KBadDaylightRuleNoTz,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"DAYLIGHT:TRUE;+01;20050326T010000Z;20051029T010000Z;;\r\n"
	"DAYLIGHT:TRUE;+02;20070325T010000Z;20071028T010000Z;;\r\n"
	"BEGIN:VTODO\r\n"
	"UID:todoparententryanish@example.com\r\n"
	"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
	"DTSTART:20050516T133000\r\n"
	"DUE:20050516T143000\r\n"
	"RRULE:D1 20050525T133000\r\n"
	"COMPLETED:20070401T080000\r\n"
	"AALARM:20050516T133000\r\n"
	"LAST-MODIFIED:20060213T105113Z\r\n"
	"X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
	"END:VTODO\r\n"
	"END:VCALENDAR\r\n"
	);

_LIT8( KNotMatchingParentChild1,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "BEGIN:VTODO\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
        "DTSTART:20050516T133000\r\n"
        "DUE:20050516T143000\r\n"
        "RRULE:D1 20050525T143000\r\n"
        "EXDATE:20050517T133000Z\r\n"
        "LAST-MODIFIED:20060213T105113Z\r\n"
        "END:VTODO\r\n"
        "BEGIN:VTODO\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
        "DTSTART:20050527T133000\r\n"
        "DUE:20050527T143000\r\n"
        "X-RECURRENCE-ID:20050517T133000\r\n"
        "LAST-MODIFIED:20060213T105113Z\r\n"
        "END:VTODO\r\n"
        "BEGIN:VTODO\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
        "DTSTART:20050528T143000\r\n"
        "DUE:20050528T153000\r\n"
        "X-RECURRENCE-ID:20050518T133000\r\n"
        "LAST-MODIFIED:20060213T105113Z\r\n"
        "END:VTODO\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KNotMatchingParentChild2,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
        "TZ:+02\r\n"
        "BEGIN:VTODO\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
        "DTSTART:20050516T133000\r\n"
        "DUE:20050516T143000\r\n"
        "RRULE:D1 20050525T143000\r\n"
        "EXDATE:20050519T133000Z\r\n"
        "LAST-MODIFIED:20060213T105113Z\r\n"
        "END:VTODO\r\n"
        "BEGIN:VTODO\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
        "DTSTART:20050527T133000\r\n"
        "DUE:20050527T143000\r\n"
        "X-RECURRENCE-ID:20050517T133000\r\n"
        "LAST-MODIFIED:20060213T105113Z\r\n"
        "END:VTODO\r\n"
        "BEGIN:VTODO\r\n"
        "UID:todoparententry@example.com\r\n"
        "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
        "DTSTART:20050528T143000\r\n"
        "DUE:20050528T153000\r\n"
        "X-RECURRENCE-ID:20050518T133000\r\n"
        "LAST-MODIFIED:20060213T105113Z\r\n"
        "END:VTODO\r\n"
        "END:VCALENDAR\r\n");


_LIT8( KInvalidChildVCalendar,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententry@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050516T133000Z\r\n"
		"DTEND:20050516T143000\r\n"
		"RRULE:D1 20050525T143000\r\n"
		"EXDATE:20050517T133000\r\n"
		"LAST-MODIFIED:20060213T105113\r\n"
		"END:VEVENT\r\n"
		"BEGIN:VTODO\r\n"
		"UID:todoparententry@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
		"DTSTART:20050528T143000\r\n"
		"DTEND:20050528T153000\r\n"
		"X-RECURRENCE-ID:20050518T133000Z\r\n"
		"LAST-MODIFIED:20060213T105113\r\n"
		"END:VTODO\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententry@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050527T133000\r\n"
		"DTEND:20050527T143000\r\n"
		"X-RECURRENCE-ID:20050517T133000Z\r\n"
		"LAST-MODIFIED:20060213T105113\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
	);
	
_LIT8(KUndatedAlarmedTodoVCalendar, 
       "BEGIN:VCALENDAR\r\n"
       "VERSION:1.0\r\n"
       "BEGIN:VTODO\r\n"
       "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
       "SUMMARY;QUOTED-PRINTABLE:Undated todo with alarm at 26.5. at 8:00\r\n"
       "CLASS:PUBLIC\r\n"
       "PRIORITY:2\r\n"
       "AALARM:20060526T080000;;;\r\n"
       "STATUS:NEEDS ACTION\r\n"
       "END:VTODO\r\n"
       "END:VCALENDAR\r\n"
   		);

_LIT8( KTestingChildDTSTARTExport,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
		"BEGIN:VTODO\r\n"
		"UID:5\r\n"
		"DTSTART:19990101T100000\r\n"
		"DTEND:19990102T100000\r\n"
		"DUE:19990102T100000\r\n"
		"RRULE:D1 #3\r\n"
		"END:VTODO\r\n"
		"BEGIN:VTODO\r\n"
		"X-RECURRENCE-ID:19990103T100000\r\n"
		"UID:5\r\n"
		"DTSTART:19990104T100000\r\n"
		"DTEND:19990105T100000\r\n"
		"RRULE:D1 #3\r\n"
		"END:VTODO\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:6\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050516T133000\r\n"
		"DTEND:20050516T143000\r\n"
		"RRULE:D1 20050525T143000\r\n"
		"EXDATE:20050519T133000\r\n"
		"LAST-MODIFIED:20060213T105113\r\n"
		"END:VEVENT\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:6\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050527T133000\r\n"
		"DTEND:20050527T143000\r\n"
		"X-RECURRENCE-ID:20050517T133000\r\n"
		"AALARM:20050527T123000\r\n"
		"X-SYMBIAN-DTSTAMP:20050901T130000\r\n"
		"LAST-MODIFIED:20060213T105113\r\n"
		"END:VEVENT\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KTestingParentDTENDExport,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
		"BEGIN:VTODO\r\n"
		"UID:5\r\n"
		"DTSTART:19990101T100000\r\n"
		"DTEND:19990101T113000\r\n"
		"DUE:19990101T113000\r\n"
		"RRULE:D1 #3\r\n"
		"END:VTODO\r\n"
		"BEGIN:VTODO\r\n"
		"X-RECURRENCE-ID:19990102T113000\r\n"
		"UID:5\r\n"
		"DTSTART:19990101T100000\r\n"
		"DTEND:19990101T123100\r\n"
		"RRULE:D1 #3\r\n"
		"END:VTODO\r\n"
        "END:VCALENDAR\r\n");

_LIT8( KExportRecIDRange,
        "BEGIN:VCALENDAR\r\n"
        "VERSION:1.0\r\n"
		"BEGIN:VTODO\r\n"
		"UID:5\r\n"
		"DTSTART:19990101T100000\r\n"
		"DTEND:19990101T113000\r\n"
		"DUE:19990101T113000\r\n"
		"RRULE:D1 #3\r\n"
		"END:VTODO\r\n"
		"BEGIN:VTODO\r\n"
		"X-RECURRENCE-ID;RANGE=THISANDPRIOR:19990102T113000\r\n"
		"UID:5\r\n"
		"DTSTART:19990101T100000\r\n"//ThisAndPrior
		"DTEND:19990101T110000\r\n"
		"RRULE:D1 #2\r\n"
		"END:VTODO\r\n"
        "END:VCALENDAR\r\n");
        
_LIT8( KExceptionDateTime,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:9\r\n"
		"X-EPOCAGENDAENTRYTYPE:ANNIVERSARY\r\n"
		"DTSTART:19990101T100000\r\n"
		"DUE:19990101T113000\r\n"
		"RRULE:D1 #3\r\n"
		"EXDATE:19990101\r\n"
		"LAST-MODIFIED:19991101T124400\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n" );

 
_LIT8( KInvalidDLBadStartDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "TZ:-08\r\n"
      "DAYLIGHT:TRUE;-07;18990326T010000Z;20071029T010000Z;;\r\n"
      "BEGIN:VTODO\r\n"
      "UID:InvalidDayLightBadStartDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLBadEndDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "TZ:+01\r\n"
      "DAYLIGHT:TRUE;+02;20010326T010000Z;21011029T010000Z;;\r\n"
      "BEGIN:VTODO\r\n"
      "UID:InvalidDayBadEndDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLEndBeforeStartDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "TZ:+01\r\n"
      "DAYLIGHT:TRUE;+02;20010326T010000Z;19991029T010000Z;;\r\n"
      "BEGIN:VTODO\r\n"
      "UID:InvalidDaylightEndBeforeStartDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLOneGoodRuleDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "TZ:-08\r\n"
      "DAYLIGHT:TRUE;-07;20000326T010000Z;20071029T010000Z;;\r\n"
      "BEGIN:VTODO\r\n"
      "UID:InvalidDaylightOneGoodRuleDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLMissingDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "TZ:-08\r\n"
      "BEGIN:VTODO\r\n"
      "UID:InvalidDayightMissingDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLTwoBadRulesDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "TZ:-08\r\n"
      "DAYLIGHT:TRUE;+02;20010326T010000Z;21011029T010000Z;;\r\n" // bad end date
      "DAYLIGHT:TRUE;+02;20010326T010000Z;19991029T010000Z;;\r\n" // end before start
      "BEGIN:VTODO\r\n"
      "UID:InvalidDaylightTwoBadRulesDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLOneBadOneGoodRuleDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "TZ:-08\r\n"
      "DAYLIGHT:TRUE;-07;20000326T010000Z;20071029T010000Z;;\r\n"  // good one
      "DAYLIGHT:TRUE;-07;18990326T010000Z;20071029T010000Z;;\r\n"  // bad start date
      "BEGIN:VTODO\r\n"
      "UID:InvalidDayLightOneBadOneGoodRuleDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLNoTZOneGoodRuleDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "DAYLIGHT:TRUE;-07;20000326T010000Z;20071029T010000Z;;\r\n"
      "BEGIN:VTODO\r\n"
      "UID:InvalidDaylightNoTZOneGoodRuleDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLNoTZMissingDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "BEGIN:VTODO\r\n"
      "UID:InvalidDayightNoTZMissingDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLNoTZTwoBadRulesDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "DAYLIGHT:TRUE;+02;20010326T010000Z;21011029T010000Z;;\r\n" // bad end date
      "DAYLIGHT:TRUE;+02;20010326T010000Z;19991029T010000Z;;\r\n" // end before start
      "BEGIN:VTODO\r\n"
      "UID:InvalidDaylightNoTZTwoBadRulesDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8( KInvalidDLNoTZOneBadOneGoodRuleDT,	
      "BEGIN:VCALENDAR\r\n"
      "VERSION:1.0\r\n"			
      "DAYLIGHT:TRUE;-07;20000326T010000Z;20071029T010000Z;;\r\n"  // good one
      "DAYLIGHT:TRUE;-07;18990326T010000Z;20071029T010000Z;;\r\n"  // bad start date
      "BEGIN:VTODO\r\n"
      "UID:InvalidDayLightNoTZOneBadOneGoodRuleDT\r\n"
      "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
      "DTSTART:20050516T133000\r\n"
      "DUE:20050516T143000\r\n"
      "RRULE:D1 20050525T133000\r\n"
      "COMPLETED:20070401T080000\r\n"
      "AALARM:20050516T133000\r\n"
      "LAST-MODIFIED:20060213T105113Z\r\n"
      "X-SYMBIAN-DTSTAMP:20040901T130000\r\n"
      "END:VTODO\r\n"
      "END:VCALENDAR\r\n"
     );

_LIT8(KRepeatEntryHasCountAndEndDate1,
//repeat count comes before repeat end time
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:5\r\n"
	"DTSTART:19990101T100000\r\n"
	"DTEND:19990101T113000\r\n"
	"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
	"RRULE:MP2 1+ SU #8 20000703T060000\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);
	
_LIT8(KRepeatEntryHasCountAndEndDate2,
//repeat count comes after repeat end time
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:5\r\n"
	"DTSTART:19990101T100000\r\n"
	"DTEND:19990101T113000\r\n"
	"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
	"RRULE:MP2 1+ SU #200 20000703T060000\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);
	
_LIT8( KRepeatEntryHasInvalidDate,
//repeat rule has invalid date
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:RepeatRuleHasInvalidDate\r\n"
	"DTSTART:20050516T205100\r\n"
	"DTEND:20050516T215100\r\n"
	"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
	"CLASS:PUBLIC\r\n"
	"SEQUENCE:0\r\n"
	"X-METHOD:NONE\r\n"
	"RRULE:MD1 16 17 39 #10\r\n"
	"LAST-MODIFIED:20060213T105113Z\r\n"
	"PRIORITY:0\r\n"
	"X-SYMBIAN-LUID:2\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);

_LIT8(KImportInvalidChildEntryTestParent,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.0\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050516T133000\r\n"
    "DTEND:20050516T143000\r\n"
    "RRULE:D1 20050528T143000\r\n"
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n" 
    );

// Invalid child entry for KImportInvalidChildEntryTestParent. The child has a recurrence id that is
// the same as the parent's start date and a repeat rule that covers the entire
// parent entry. It is invalid for a child entry to replace the entire parent.
_LIT8(KChildEntryMoreThanOverlapParentWithRDate,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.0\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050516T133000\r\n"
    "DTEND:20050516T143000\r\n"
    "X-RECURRENCE-ID:20050518T133000\r\n"
    "RRULE:D1 20050529T143000\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "X-SYMBIAN-LUID:5\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    );

_LIT8(KChildEntryWhollyContainsParent,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050501T133000\r\n"
    "DTEND:20050501T143000\r\n"
    "X-RECURRENCE-ID:20050518T133000\r\n"
    "RRULE:D1 20050601T143000\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "X-SYMBIAN-LUID:5\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    );

_LIT8(KChildEntryRecIdBeforeParentStart,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050520T133000\r\n"
    "DTEND:20050520T143000\r\n"
    "X-RECURRENCE-ID:20050501T133000\r\n"
    "RRULE:D1 20050525T143000\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "X-SYMBIAN-LUID:5\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    );

_LIT8(KChildEntryRecIdAfterParentEnd,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050520T133000\r\n"
    "DTEND:20050520T143000\r\n"
    "X-RECURRENCE-ID:20050601T153000\r\n"
    "RRULE:D1 20050525T143000\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "X-SYMBIAN-LUID:5\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    );
    
_LIT8(KChildEntryPartialOverlapParentTailInvalidRecID,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050520T133000\r\n"
    "DTEND:20050520T143000\r\n"
    "X-RECURRENCE-ID:20050530T143000\r\n"
    "RRULE:D1 20050610T143000\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "X-SYMBIAN-LUID:5\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    );
    
_LIT8(KChildEntryPartialOverlapParentHeadInvalidRecID,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050510T133000\r\n"
    "DTEND:20050510T143000\r\n"
    "X-RECURRENCE-ID:20050512T143000\r\n"
    "RRULE:D1 20050520T143000\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "X-SYMBIAN-LUID:5\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    );

_LIT8(KChildEntryRecIdInvalidDateOnly,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050520T133000\r\n"
    "DTEND:20050520T143000\r\n"
    "X-RECURRENCE-ID:20050529\r\n"
    "RRULE:D1 20050525T143000\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "X-SYMBIAN-LUID:5\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n"
    );        
    
_LIT8(KImportInvalidChildEntryTestParentWithRDate, 
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.0\r\n"
    "TZ:+01\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:23\r\n"
    "DTSTART:20050516T133000\r\n"
    "DTEND:20050516T143000\r\n"
    "RRULE:D1 20050528T143000\r\n"
    "RDATE:20050530T000000\r\n"    
    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
    "LAST-MODIFIED:20060213T105113Z\r\n"
    "END:VEVENT\r\n"
    "END:VCALENDAR\r\n" 
    );
    

// This entry contains a DALARM. It should be imported the same way that an
// AALARM is.
_LIT8( KEntry24,
    "BEGIN:VCALENDAR\r\n"
    "VERSION:1.0\r\n"
    "BEGIN:VEVENT\r\n"
    "UID:24\r\n"
	"DTSTART:20060317T070000Z\r\n"
	"DTEND:20060317T073000Z\r\n"
	"SUMMARY:Meeting 09.00-09.30\r\n"
	"LOCATION:Salo\r\n"
	"DALARM:20060317T063000Z\r\n"
	"LAST-MODIFIED:20060315T084427Z\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
    );



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * TCalTimeUtil
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class TCalTimeUtil
	{
public:
	static TCalTime CalTimeL(TInt aY, TMonth aM, TInt aD, TInt aH, TInt aMin, TInt aSec, TInt amSec,
							TCalTime::TTimeMode aMode = TCalTime::EFixedUtc);
	static void FormatL(TDes& aDes, TTime aTime);
	static void FormatL(TDes& aDes, const TCalTime& aCalTime);
	};

void TCalTimeUtil::FormatL(TDes& aDes, TTime aTime)
    {
    _LIT(KFormatTxt,"%04d%02d%02dT%02d%02d%02d");
    TDateTime dt = aTime.DateTime();
    aDes.Format(KFormatTxt,dt.Year(),TInt(dt.Month()+1),dt.Day()+1,dt.Hour(),dt.Minute(),dt.Second());
    }

void TCalTimeUtil::FormatL(TDes& aDes, const TCalTime& aCalTime)
	{
	TBuf<64> timestr;
	switch(aCalTime.TimeMode())
		{
		case TCalTime::EFloating:  
				FormatL(timestr, aCalTime.TimeLocalL());
				aDes.Format(_L("%S(floating)"), &timestr);
				break;
		case TCalTime::EFixedUtc:
				FormatL(timestr, aCalTime.TimeUtcL());
				aDes.Format( _L("%SZ(utc)"),	&timestr);
				break;
		case TCalTime::EFixedTimeZone:
				FormatL(timestr, aCalTime.TimeUtcL());
				aDes.Format(_L("%SZ(fixedTZ)"), &timestr);
				break;
		default: ASSERT(0);
		};
	}

TCalTime TCalTimeUtil::CalTimeL(TInt aY, TMonth aM, TInt aD, 
							   TInt aH, TInt aMin, TInt aSec, TInt amSec, 
							   TCalTime::TTimeMode aMode/*=EFixedUtc*/)
	{
	TCalTime calTime;
	TTime time = TDateTime(aY, aM, aD, aH, aMin, aSec, amSec);
	switch (aMode)
		{
		case TCalTime::EFloating: calTime.SetTimeLocalFloatingL(time); break; 
		case TCalTime::EFixedUtc: calTime.SetTimeUtcL(time); break;
		case TCalTime::EFixedTimeZone:	calTime.SetTimeLocalL(time); break;
		default: ASSERT(0);
		}
	return calTime;
	};
	
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * CDataExchangeTestManager
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

_LIT8(KOutOfRangeRDate,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"TZ:+01\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:5\r\n"
	"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
	"DTSTART:19990101T100000\r\n"
	"DTEND:19990101T113000\r\n"
	"RRULE:D1 #4\r\n"
	"RDATE:18990501T120000;21010601T090000\r\n"
	"LAST-MODIFIED:19991101T124400\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);

_LIT8(KOutOfRangeExDate,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"TZ:+01\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:5\r\n"
	"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
	"DTSTART:19990101T100000\r\n"
	"DTEND:19990101T113000\r\n"
	"RRULE:D1 #4\r\n"
	"EXDATE:18990102T100000;21010102T100000\r\n"
	"LAST-MODIFIED:19991101T124400\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);

_LIT8(KInvalidExceptions,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"TZ:+01\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:5\r\n"
	"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
	"DTSTART:19990101T100000\r\n"
	"DTEND:19990101T113000\r\n"
	"RRULE:D1 #4\r\n"
	"EXDATE:19990105T100000;19990106T100000;20000102T100000\r\n"
	"LAST-MODIFIED:19991101T124400\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);
   
_LIT8(KAttachmentVcal, 
"BEGIN:VCALENDAR\r\n"
"VERSION:1.0\r\n"
"BEGIN:VEVENT\r\n"
"DTSTART:20000221T070000Z\r\n"
"DTEND:20000221T071500Z\r\n"
"SUMMARY:TEST ATTACHMENT\r\n"
"UID:AttachTest\r\n"
"ATTACH;VALUE=URL:ftp://xyzCorp.com/pub/reports/r-960812.ps\r\n"
"ATTACH;VALUE=CID:jsmith.part3.960817T083000.xyzMail@example.com\r\n"
"ATTACH;X-CONTENTTYPE=image/basic;X-NAME=Holiday.jp;X-FILEDATE=19971210T080000Z;ENCODING=BASE64;VALUE=BINARY:\r\n"
"    ZmlsZTovLy9qcXB1YmxpYy5naWY=\r\n"
"\r\n"
"END:VEVENT\r\n"
"END:VCALENDAR\r\n");

_LIT8( KEntryDaylightOverlapNow,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"TZ:+01\r\n"
	"DAYLIGHT:TRUE;+02;20060326T010000Z;20061029T010000Z;+01;+02\r\n"
	"DAYLIGHT:TRUE;+02;20070325T010000Z;20071028T010000Z;+01;+02\r\n"
	"BEGIN:VEVENT\r\n"
	"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
	"SUMMARY:15 Jan 2006\r\n"
	"DTSTART:20060115T110000Z\r\n"
	"DTEND:20060115T113000Z\r\n"
	"STATUS:CONFIRMED\r\n"
	"CLASS:PUBLIC\r\n"
	"RRULE:MD1 15+ 20070615T000000\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
    );

/*create an entry with attachment - file name and binaries*/

void CAsyncImportDataExCallback::Progress( TInt aPercentageCompleted )
    {
    test.Printf( _L( "Multi async import is now %d per cent complete\n"),
        aPercentageCompleted);    
    }


void CAsyncImportDataExCallback::Completed()
    {
	test.Printf( _L( "Multi async test completed\n") );
	CActiveScheduler::Stop();
    }


CDataExchangeTestManager* CDataExchangeTestManager::NewLC()
	{
	CDataExchangeTestManager* self = new (ELeave) CDataExchangeTestManager();
	
	CleanupStack::PushL(self);
	self->ConstructL();
	
	return (self);
	}

	
void CDataExchangeTestManager::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL(EFalse);
	iTestLib->ReplaceFileL(KCalendarFile);
	iTestLib->OpenFileL(KCalendarFile);	
	
	iDataExchange = CCalDataExchange::NewL(iTestLib->GetSession());
	
	iAsyncImportCallback = new(ELeave)CAsyncImportDataExCallback;
	}

	
CDataExchangeTestManager::~CDataExchangeTestManager()	
	{
	TRAP_IGNORE(iTestLib->DeleteFileL(KCalendarFile));
	delete iDataExchange;	
	delete iTestLib;
	delete iAsyncImportCallback;
	}


void CDataExchangeTestManager::PrintAttendeeL(CCalAttendee* aAttendee)
	{
	TPtrC ptrAddress(aAttendee->Address());
	TPtrC ptrSentBy(aAttendee->SentBy());
	TPtrC ptrCommonName(aAttendee->CommonName());
	TInt status = aAttendee->StatusL();
	TInt role = aAttendee->RoleL();
	TBool rsvp = aAttendee->ResponseRequested();		
		
	test.Printf(_L("~~:ATTENDEE DETAILS:~~\n"));
	test.Printf(_L("Address:\t%S\n"), &ptrAddress);
	test.Printf(_L("Sent-By:\t%S\n"), &ptrSentBy);
	test.Printf(_L("Common Name:\t%S\n"), &ptrCommonName);
	test.Printf(_L("RSVP:\t%d\n"), rsvp);
	test.Printf(_L("CalStatus:\t%d\n"), status);
	test.Printf(_L("CalRole:\t%d\n"), role);
	}


void CDataExchangeTestManager::CreateTestFileL(const TDesC8& aVCalData, const TDesC& aFileName)
	{
	RFile file;
	
	iTestLib->TestRegister().CreateTempFileLC(file, aFileName);
	
	User::LeaveIfError(file.Write(aVCalData));
	
	CleanupStack::PopAndDestroy(&file); //file.Close()
	}


void CDataExchangeTestManager::ImportL(const TDesC& aImportVCalendarFile, RPointerArray<CCalEntry>& aEntriesImported, TBool aIsSynch)
	{	
	RFile infile;
	iTestLib->TestRegister().OpenTempFileLC(infile, aImportVCalendarFile);
	RFileReadStream readStream(infile);
	CleanupClosePushL(readStream);
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(iTestLib->GetSession());
	CleanupStack::PushL(dataExchange);
	
	if(aIsSynch)
		{
		dataExchange->ImportL(KUidVCalendar, readStream, aEntriesImported);
		}
	else
		{
		dataExchange->ImportAsyncL(KUidVCalendar, readStream, aEntriesImported, *iAsyncImportCallback /*callback*/, 0);
		CActiveScheduler::Start();			
		}
	CleanupStack::PopAndDestroy(dataExchange);
	CleanupStack::PopAndDestroy(&readStream); // readStream.Close()
	CleanupStack::PopAndDestroy(&infile); // infile.Close()
	}

void CDataExchangeTestManager::ExportL(const TDesC& aExportVCalendarFile, RPointerArray<CCalEntry>& aEntriesExported, TBool aIsSynch)
	{
	RFile outFile;
	iTestLib->TestRegister().CreateTempFileLC(outFile, aExportVCalendarFile);
	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(iTestLib->GetSession());
	CleanupStack::PushL(dataExchange);	
	if(aIsSynch)
		{
		dataExchange->ExportL(KUidVCalendar, writeStream, aEntriesExported);
		}
	else
		{	
		dataExchange->ExportAsyncL(KUidVCalendar, writeStream, aEntriesExported, *iAsyncImportCallback);
		CActiveScheduler::Start();
		}

	writeStream.CommitL();
	CleanupStack::PopAndDestroy(dataExchange);
	CleanupStack::PopAndDestroy(&writeStream); // writeStream.Close()
	CleanupStack::PopAndDestroy(&outFile); // outFile.Close()
	}
		
void CDataExchangeTestManager::TestToDoEntryL(CCalEntry& aEntry) 
	{
	test(aEntry.StartTimeL().TimeLocalL() == TTime(TDateTime(1999, ENovember, 1, 1, 0, 0, 0)));
	CCalAlarm* alarm = aEntry.AlarmL();
	CleanupStack::PushL(alarm);
	TTime alarmTime = aEntry.EndTimeL().TimeLocalL() - alarm->TimeOffset();
	test(alarmTime == TTime(TDateTime(1999, ENovember, 1, 2, 1, 0, 0)));
	CleanupStack::PopAndDestroy(alarm);
	}
	
TBool CDataExchangeTestManager::TestEntry1L(CCalEntry& aEntry) 
	{
	TBool passed = ETrue;
	
	CCalEntry::TMethod method = aEntry.MethodL();
	TInt sequenceID = aEntry.SequenceNumberL();

	if(method != CCalEntry::EMethodRequest)
		{
		passed = EFalse;
		}
		
	if(sequenceID != 5)
		{
		passed = EFalse;
		}
		
	if(aEntry.TimeTransparencyL() != CCalEntry::ETranspTentative)
		{
		passed = EFalse;
		}
		
	return passed;	
	}


TBool CDataExchangeTestManager::TestEntry2L(CCalEntry& aEntry) 
	{
	TBool passed = ETrue;
	
	CCalEntry::TMethod method = aEntry.MethodL();
	TInt sequenceID = aEntry.SequenceNumberL();

	if(method != CCalEntry::EMethodCounter)
		{
		passed = EFalse;
		}
		
	if(sequenceID != 2)
		{
		passed = EFalse;
		}
		
	if(aEntry.TimeTransparencyL() != CCalEntry::ETranspBusy)
		{
		passed = EFalse;
		}
		
	return passed;
	}


TBool CDataExchangeTestManager::TestEntry3L(CCalEntry& aEntry) 
	{
	TBool passed = ETrue;

	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iTestLib->SynCGetEntryViewL().FetchL(KEntry3UID, entryArray);	
	
	test(entryArray.Count() == 1);
	
	//	although there are 3 ATTENDEES, X-ORGANIZER causes
	//	one of the attendees to become an iCal ORGANIZER
	//	and is thus not added to the attendeeList!
	test(entryArray[0]->AttendeesL().Count() == 2);
	CCalUser* organizerInMemory = aEntry.OrganizerL();	
	CCalUser* phoneownerInMemory = aEntry.PhoneOwnerL();	
	CCalUser* organizer = entryArray[0]->OrganizerL();	
	CCalUser* phoneowner = entryArray[0]->PhoneOwnerL();	

	// DEF073855: Symbian: Erroneous CCalDataExchange::ImportL output (Summary field)
	// == //
	_LIT(KSummary_Field_Validation_DEF073855, "GS appt");
	const TDesC& summary = entryArray[0]->SummaryL();
	if(summary.Length())
		{
		test.Printf(_L("Entry3: Validating Summary Field...\n"));
		test(summary == KSummary_Field_Validation_DEF073855);
		test.Printf(_L("Entry3: Summary Field - Validated.\n"));
		}
	// === //


	if(organizer)
		{
		test.Printf(_L("Entry3 Organizer:\n"));
		test(organizer->CommonName() == KEntry3Organizer);
		}
	else
		{
		test.Printf(_L("Entry3 Organizer cannot be retrieved from file\n"));
		passed = EFalse;
		}
		
	if(phoneowner)
		{
		test.Printf(_L("Entry3 PhoneOwner:\n"));
		test(phoneowner->CommonName() == KEntry3PhoneOwner);
		}
	else
		{
		test.Printf(_L("Entry3 Phone Owner cannot be retrieved from file\n"));
		passed = EFalse;
		}
		
	if(organizerInMemory)
		{
		test.Printf(_L("Entry3 OrganizerIM:\n"));
		test(organizerInMemory->CommonName() == KEntry3Organizer);
		}
	else
		{
		test.Printf(_L("Entry3 Organizer cannot be retrieved from Memory\n"));
		passed = EFalse;
		}
			
	if(phoneownerInMemory)
		{
		test.Printf(_L("Entry3 PhoneOwnerIM:\n"));
		test(phoneownerInMemory->CommonName() == KEntry3PhoneOwner);
		}
	else
		{
		test.Printf(_L("Entry3 PhoneOwner cannot be retrieved from Memory\n"));
		passed = EFalse;
		}
		
	CleanupStack::PopAndDestroy(&entryArray);		//	entryArray
	
	return passed;
	}


void CDataExchangeTestManager::TestImportEntriesL(TBool aIsSynch)
	{
	RPointerArray<HBufC8> bufArray;
	CleanupResetAndDestroyPushL(bufArray);
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	bufArray.AppendL(KEntry1().AllocL());
	bufArray.AppendL(KEntry2().AllocL());
	bufArray.AppendL(KEntry3().AllocL());
	bufArray.AppendL(KEntry13().AllocL());
	bufArray.AppendL(KEntry14().AllocL());
	bufArray.AppendL(KEntry15().AllocL());
	bufArray.AppendL(KEntry16().AllocL());
	bufArray.AppendL(KEntry17().AllocL());
	bufArray.AppendL(KEntry18().AllocL());
	bufArray.AppendL(KEntry20().AllocL());
	bufArray.AppendL(KEntry22().AllocL());
	bufArray.AppendL(KEntry24().AllocL());

	
	test.Next(_L("test data exchange: IMPORT METHOD\n"));	
	TBuf<64> importFileNames;	
	for (TInt i = 0; i < bufArray.Count(); ++i)
		{	
		importFileNames.Format(KEntryImportFile(), i);
	
		test.Next(_L("Writing test entry data to vcs file\n"));
				
		CreateTestFileL(*bufArray[i], importFileNames);
		ImportL(importFileNames, entryArray, aIsSynch);
		}
		
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	test(entryArray.Count() == success);
	
	//	test the imported entries!
	CCalEntry* anEntry;

	for(TInt i = 0; i < entryArray.Count(); ++i)
		{
		anEntry = entryArray[i];
		
		TPtrC8 uidValue(anEntry->UidL());
		
		if(uidValue == KEntry1UID)
			{
			test(TestEntry1L(*anEntry));
			}
		else if(uidValue == KEntry1UID2)
			{
			//	not tested yet. (multiple entries in same vcalendar)
			}
		else if(uidValue == KEntry2UID)
			{
			test(TestEntry2L(*anEntry));
			}
		else if(uidValue == KEntry3UID)
			{
			test(TestEntry3L(*anEntry));
			}
        else if (uidValue == KEntry13UID || 
              	 uidValue == KEntry14UID ||
                 uidValue == KEntry15UID ||
                 uidValue == KEntry16UID ||
                 uidValue == KEntry17UID )
            {
	        test( TestTodoDateImportL( *anEntry, uidValue ) );
            }
        else if (uidValue == KEntry18UID)
            {
            // Verify imported entry is correct and then export the calendar 
            // again. 
            TestAttendeeImportL( *anEntry );
            ExportL( KEntryExportFileAttendee, entryArray, aIsSynch);
            }
        else if (uidValue == KEntry20UID)
            {
            // Verify MS Outlook style recurrence id and exception date are supported
            // because only thedate portion is exported
            CCalEntry* aChildEntry = entryArray[i+1];
            ++i;
            TestRecurrenceIdImportL(*anEntry, *aChildEntry);
            }
        else if (uidValue == KEntry22UID)
            {
            // Verify that DUE tuime is the same as START time
            // and different from ALARM time
            TestToDoEntryL(*anEntry);
            }
        else if (uidValue == KEntry24UID)
            {
            // A DALARM should be imported the same as an AALARM. Verify that 
            // the alarm was imported.
            test.Printf( _L("Verifying that a DALARM is imported." ) );
            CCalAlarm* alarm = anEntry->AlarmL();
            test( alarm != NULL );
            delete alarm;
            }
		else
			{
			test.Printf(_L("Entry test not found or available.\n"));
			test(0);	// panic
			}
		}
		
	CleanupStack::PopAndDestroy();		//	entryArray
	CleanupStack::PopAndDestroy();		//	bufArray
	}

void CDataExchangeTestManager::TestLUIDFromImportEntriesL(TBool aIsSynch)
	{
	test.Next(_L("test data exchange: LUID IMPORT METHOD"));	

	RPointerArray<CCalEntry> storedEntries;
	CleanupResetAndDestroyPushL(storedEntries);

	// Import the entries
	RPointerArray<HBufC8> bufArray;
	CleanupResetAndDestroyPushL(bufArray);

	bufArray.AppendL(KEntry18a().AllocL());
	
	TBuf<64> importFileNames;	
	for (TInt i = 0; i < bufArray.Count(); ++i)
		{	
		importFileNames.Format(KEntryImportFile(), i);
	
		test.Printf(_L("Writing test entry data to vcs file\n"));
				
		CreateTestFileL(*bufArray[i], importFileNames);
		ImportL(importFileNames, storedEntries, aIsSynch);
		}

	CleanupStack::PopAndDestroy(&bufArray);
		
	for(TInt i=0 ; i<storedEntries.Count(); ++i)
		{
		test.Printf(_L("Entry[%d] LUID: Imported=%d\n"), i, storedEntries[i]->LocalUidL());
		}

		
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(storedEntries, success);
	test(storedEntries.Count() == success);
	

	test.Printf(_L("Verify Stored and Fetched LUID match\n"));
	for(TInt i=0 ; i<storedEntries.Count(); ++i)
		{
		RPointerArray<CCalEntry> fetchedEntries;
		CleanupResetAndDestroyPushL(fetchedEntries);
		iTestLib->SynCGetEntryViewL().FetchL(storedEntries[i]->UidL(), fetchedEntries);	

		test(fetchedEntries.Count() == 1);
		test.Printf(_L("Entry[%d] LUID: Stored=%d, Fetched=%d\n"), i, storedEntries[i]->LocalUidL(), fetchedEntries[0]->LocalUidL());
		test(fetchedEntries[0]->LocalUidL() == storedEntries[i]->LocalUidL());

		CleanupStack::PopAndDestroy(&fetchedEntries);
		}

	
	//update imported entries by re-sync'ing; they should replace old entries
	test.Printf(_L("Update imported entries by re-importing them\n"));
	RPointerArray<CCalEntry> updatedEntries;
	CleanupResetAndDestroyPushL(updatedEntries);

	CleanupResetAndDestroyPushL(bufArray);
	bufArray.AppendL(KEntry18b().AllocL());

	for (TInt i = 0; i < bufArray.Count(); ++i)
		{	
		importFileNames.Format(KEntryImportFile(), i);
	
		test.Printf(_L("Writing test entry data to vcs file\n"));
				
		CreateTestFileL(*bufArray[i], importFileNames);
		ImportL(importFileNames, updatedEntries, aIsSynch);
		}
	CleanupStack::PopAndDestroy(&bufArray);

	test(storedEntries.Count() == updatedEntries.Count());
	test.Printf(_L("Verify Imported (non-stored) LUID don't match\n"));
	for(TInt i=0 ; i<updatedEntries.Count(); ++i)
		{
		test.Printf(_L("Entry[%d] LUID: Stored=%d, Imported=%d\n"), i, storedEntries[i]->LocalUidL(), updatedEntries[i]->LocalUidL());
		test(storedEntries[i]->LocalUidL() != updatedEntries[i]->LocalUidL());
		}

	iTestLib->SynCGetEntryViewL().StoreL(updatedEntries, success);
	test(updatedEntries.Count() == success);
	test(storedEntries.Count() == updatedEntries.Count());
	
	//	test the imported entries!
	test.Printf(_L("Verify Updated and Fetched LUID match\n"));
	for(TInt i=0 ; i<updatedEntries.Count(); ++i)
		{
		RPointerArray<CCalEntry> fetchedEntries;
		CleanupResetAndDestroyPushL(fetchedEntries);
		iTestLib->SynCGetEntryViewL().FetchL(updatedEntries[i]->UidL(), fetchedEntries);	
		
		test(fetchedEntries.Count() == 1);

		test.Printf(_L("Entry[%d] LUID: Updated=%d, Fetched=%d\n"), i, updatedEntries[i]->LocalUidL(), fetchedEntries[0]->LocalUidL());
		test(fetchedEntries[0]->LocalUidL() == updatedEntries[i]->LocalUidL());
		test(storedEntries[i]->LocalUidL() == updatedEntries[i]->LocalUidL());
		CleanupStack::PopAndDestroy(&fetchedEntries);
		}
	CleanupStack::PopAndDestroy(&updatedEntries);
	CleanupStack::PopAndDestroy(&storedEntries);
	}

void CDataExchangeTestManager::TestExportEntriesL(TBool aIsSynch)
	{	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	CCalIter& iter = iTestLib->GetIterL();
	TPtrC8 Uid(iter.FirstL());

	test.Next(_L("test data exchange: EXPORT METHOD\n"));		

	TBuf<64> exportFileNames;
	for(TInt i=0; Uid != KNullDesC8(); ++i)
		{
		exportFileNames.Format(KEntryExportFile(), i);
		
		entryArray.ResetAndDestroy();
		iTestLib->SynCGetEntryViewL().FetchL(Uid, entryArray);
		ExportL(exportFileNames, entryArray, aIsSynch);
		Uid.Set(iter.NextL());
		}
		
	CleanupStack::PopAndDestroy();		//	entryarray
	}

TBool CDataExchangeTestManager::TestTodoDateImportL( CCalEntry& aEntry, TPtrC8 aUidValue )
 	{
 	test.Next( _L( "TestTodoDateImportL" ) );
 	_LIT( KDateString,"%*E%*D%X%*N%*Y %1 %2 '%3" );
 	const TInt KMaxDateStringLength	= 30;
    TBuf<KMaxDateStringLength> dateString;
    test.Printf(_L("Retrieving Start and End Dates\n"));
 
 	// Extract end date and start date. If something is messed up it
    // will leave.
 	TTime startDate = aEntry.StartTimeL().TimeUtcL();
 	TTime endDate = aEntry.EndTimeL().TimeUtcL();
 
 	if (startDate == Time::NullTTime()) 
 		{
 		test.Printf(_L("Start date is NullTTime"));
 		}
 	else
 		{
 		startDate.FormatL( dateString, KDateString );
 	  	test.Printf(_L( "Start date is %S"), &dateString );
 		}
 
 	if (endDate == Time::NullTTime())
 		{
 		test.Printf(_L("End date is NullTTime"));
 		}
 	else
 		{
 		endDate.FormatL( dateString, KDateString );
 	  	test.Printf(_L( "End date is %S"), &dateString );
 		}
 	// Ensure that the times retrieved are as expected.
 	if (aUidValue == KEntry13UID ||
 		aUidValue == KEntry15UID ||
 		aUidValue == KEntry16UID )
 		{
 		_LIT( KDate, "20051006:060000" );
 		const TDesC& dateDes = KDate();
 		TTime expectedTime( dateDes );
 		test( startDate == expectedTime );
 		test( endDate == expectedTime );
 		}
    else if (aUidValue == KEntry14UID)
         {
         // The minutes of the start time will be set to the minutes of the 
         // end time.
         _LIT( KDate, "20050906:060000" );
         const TDesC& dateDesStart = KDate();
         TTime expectedStartTime( dateDesStart );
 
         _LIT( KDate2, "20051107:063000" );
         const TDesC& dateDesEnd = KDate2();
         TTime expectedEndTime( dateDesEnd );
 
         test( startDate == expectedStartTime);
         test( endDate == expectedEndTime);
         }
     else if (aUidValue == KEntry17UID)
         {
         test( startDate == Time::NullTTime() );
         test( endDate == Time::NullTTime() );
         }
 
 	test.Printf( _L( "\n" ) );
 	
 	return ETrue;
 	}

void CDataExchangeTestManager::TestAttendeeImportL( CCalEntry& aEntry )
    {
	test.Next( _L( "TestAttendeeImport" ) );
	test.Printf( _L( "Verifying role and status.\n" ) );

    RPointerArray<CCalAttendee>& attendees = aEntry.AttendeesL();
    
    // Ensure there are 6 attendees. The organizer isn't considered 
    // an attendee.
    TInt numAttendees = attendees.Count();
    test( numAttendees == 6 );

    // Verify the x-role and x-status are all set correctly. If X-ROLE wasn't
    // specified then it's converted from the ROLE and vice-versa.
    CCalAttendee::TCalRole role = attendees[0]->RoleL();
    test( role == CCalAttendee::EVCalOwner );
    CCalAttendee::TCalStatus status = attendees[0]->StatusL();
    test( status == CCalAttendee::EDelegated );

    role = attendees[1]->RoleL();
    test( role == CCalAttendee::EVCalAttendee );
    status = attendees[1]->StatusL();
    test( status == CCalAttendee::ENeedsAction);

    role = attendees[2]->RoleL();
    test( role == CCalAttendee::EVCalDelegate );
    status = attendees[2]->StatusL();
    test( status == CCalAttendee::EDeclined );

    // This entry didn't define ROLE. It should be OWNER which is the 
    // equivalent of iCal's chair when later exported.
    role = attendees[3]->RoleL();
    test( role == CCalAttendee::EChair );
    status = attendees[3]->StatusL();
    test( status == CCalAttendee::EDelegated );

    role = attendees[4]->RoleL();
    test( role == CCalAttendee::EReqParticipant );
    status = attendees[4]->StatusL();
    test( status == CCalAttendee::ENeedsAction );

    // This entry didn't have ROLE defined, only X-ROLE. The ROLE should be 
    // ATTENDEE when later exported.
    role = attendees[5]->RoleL();
    test( role == CCalAttendee::EReqParticipant );
    status = attendees[5]->StatusL();
    test( status == CCalAttendee::EDeclined );
    }

void CDataExchangeTestManager::TestRecurrenceIdImportL( CCalEntry& aParentEntry, CCalEntry& aChildEntry )
    {
	test.Next( _L( "TestRecurrenceIdImportL" ) );
	test.Printf( _L( "Verifying recurrence id and exception date.\n") );
	
	TTime expectedExDate(TDateTime(2006, EApril, 12, 10, 0, 0, 0));
	TTime expectedRecurId(TDateTime(2006, EApril, 12, 10, 0, 0, 0));
	
	RArray<TCalTime> exDates;
	CleanupClosePushL(exDates);
	aParentEntry.GetExceptionDatesL(exDates);
	test (exDates.Count() == 1);
	TCalTime exDate = exDates[0];
	TTime exDateTime = exDate.TimeUtcL();
	test (exDateTime == expectedExDate);
	CleanupStack::PopAndDestroy();	// exDates
	
	TCalTime recurId = aChildEntry.RecurrenceIdL();
	TTime recurIdTime = recurId.TimeUtcL();
	test (recurIdTime == expectedRecurId);
    }

void CDataExchangeTestManager::TestImportNEntriesL()
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	TBuf<64> importFileNames;
	importFileNames.Format(KEntryImportFile(), 0);

	for (TInt i = 0; i < 3; i++)
		{
		test.Printf(_L("Importing %d entries from %S\n"), i, &importFileNames);

		RFile infile;
		iTestLib->TestRegister().OpenTempFileLC(infile, importFileNames);
		RFileReadStream readStream(infile);
		CleanupClosePushL(readStream);
		
		entryArray.ResetAndDestroy();
		iDataExchange->ImportL(KUidVCalendar, readStream, entryArray, 0, i);
		test(entryArray.Count() == i);

		if (i == 1)
			{
			// check that the next entry in the stream can be read
			RPointerArray<CCalEntry> otherEntryArray;
			CleanupResetAndDestroyPushL(otherEntryArray);
			iDataExchange->ImportL(KUidVCalendar, readStream, otherEntryArray, 0, 1);
			test(otherEntryArray.Count() == 1);
			test(otherEntryArray[0]->CompareL(*entryArray[0]) == EFalse);
			CleanupStack::PopAndDestroy(&otherEntryArray); // entryArray.ResetAndDestroy()
			}
			
		CleanupStack::PopAndDestroy(&readStream); // readStream.Close()
		CleanupStack::PopAndDestroy(&infile); // infile.Close()
		}
		
	CleanupStack::PopAndDestroy(&entryArray); // entryArray.ResetAndDestroy()
	}


void CDataExchangeTestManager::TestImportEntriesWithTrailingSpaceL(TBool aIsSynch)
	{
	test.Next(_L("Testing for import of entries with trailing space")) ;
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);	
	
	HBufC8* guid1 = _L8("TrailingSpace").AllocLC();
	CCalEntry* entry1 = CCalEntry::NewL(CCalEntry::EEvent, guid1, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid1);
	CleanupStack::PushL(entry1);
 
	TTime startTime(TDateTime(2005, EMay, 12, 9, 0, 0, 0)); // 09:00 on 13 May
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);

	entry1->SetStartAndEndTimeL(calStartTime, calStartTime);	
	
	entry1->SetSummaryL(_L("Trailing Space \x2029")) ;
 
	entryArray.AppendL(entry1);
	CleanupStack::Pop(entry1);

	ExportImportAndCheckL(entryArray, aIsSynch) ;
	
	CleanupStack::PopAndDestroy(&entryArray); // entryArray.ResetAndDestroy()
	}

void CDataExchangeTestManager::TestImportingInvalidChildEntriesL(TBool aIsSynch)
	{
	test.Printf(_L("Testing import of child entries with different types to parent")) ;

	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	//	create new test file with invalid children
	_LIT(KInvalidChildCard, "InvalidChild.vcs");
	CreateTestFileL(KInvalidChildVCalendar, KInvalidChildCard);	
	
	ImportL(KInvalidChildCard, entryArray, aIsSynch);

	TInt success(0);
	const TInt totalImportedEntries(3);
	
	test(entryArray.Count() == totalImportedEntries);
	test.Printf(_L("Entries to be imported from vCalendar file:\n")) ;
	ListEntriesL(entryArray);	
	
	TBuf8<64> guid;
	guid.Copy(entryArray[0]->UidL());
	TRAPD(error, iTestLib->SynCGetEntryViewL().StoreL(entryArray, success));
	
	//	should leave after storing 1st entry since 2nd entry is invalid
	test(error == KErrArgument);
	const TInt correctlyImportedEntries(1);
	
	test(success == correctlyImportedEntries);
		
	entryArray.ResetAndDestroy();	
	iTestLib->SynCGetEntryViewL().FetchL(guid, entryArray);

	//	check that entries can be retrieved from file
	test.Printf(_L("Entry retrieved from store:\n")) ;
	ListEntriesL(entryArray);	
	test(entryArray.Count() == correctlyImportedEntries);
	
	CleanupStack::PopAndDestroy(&entryArray);	
	}



void CDataExchangeTestManager::ResetAgendaFileL()
	{
	delete iDataExchange;
	
	iDataExchange = NULL;
	
	iTestLib->OpenFileL(KCalendarFile);	
	
	iDataExchange = CCalDataExchange::NewL(iTestLib->GetSession());	
	}

	
void CDataExchangeTestManager::TestPhoneOwnerPersistenceL()
	{
	//	create a calentry and a caluser
	_LIT(KFredFlintstone, "Fred.Flintstone@hotmail.com");
	_LIT8(KDentistAppointmentGUID, "Dentist Appt");

	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	HBufC8* tempGUID = KDentistAppointmentGUID().AllocL();
	CCalEntry* newEntry = CCalEntry::NewL(CCalEntry::EAppt, tempGUID, CCalEntry::EMethodNone, 0);
	CleanupStack::PushL(newEntry);
	TCalTime calStartTime = TCalTimeUtil::CalTimeL(2005, EJune, 12, 9, 0, 0, 0);	// 09:00 on 13 June
	newEntry->SetStartAndEndTimeL(calStartTime, calStartTime);

	//	set one to be a phone owner
	CCalAttendee* newAttendee = CCalAttendee::NewL(KFredFlintstone);
	CleanupStack::PushL(newAttendee);
	newEntry->AddAttendeeL(newAttendee);
	CleanupStack::Pop(newAttendee);
	newEntry->SetPhoneOwnerL(newAttendee);
	entryArray.AppendL(newEntry);
	CleanupStack::Pop(newEntry);
	//	store calentry using ccalview
	TInt stored = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, stored);
	test(stored == entryArray.Count());

	CleanupStack::PopAndDestroy(&entryArray);	//	entryArray.ResetAndDestroy();

	//	close and reopen the agenda model
	ResetAgendaFileL();
	
	//	fetch ccalview
	iTestLib->SynCGetEntryViewL().FetchL(KDentistAppointmentGUID, entryArray);	

	//	check that phone owner is correct	
	CCalUser* phoneOwner = entryArray[0]->PhoneOwnerL();
	test(phoneOwner->Address() == KFredFlintstone);
	
	entryArray.ResetAndDestroy();
	}


static void CreateGSEntriesL(RPointerArray<CCalEntry>& aEntryArray)
	{	
	// create original repeating entry 
	
// Entry Number 1 parent
	HBufC8* guid1 = _L8("test1").AllocLC();
	CCalEntry* entry1 = CCalEntry::NewL(CCalEntry::EAppt, guid1, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid1);
	CleanupStack::PushL(entry1);

	TTime startTime(TDateTime(2005, EMay, 12, 9, 0, 0, 0)); // 09:00 on 13 May
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	
	TCalRRule rule1(TCalRRule::EDaily);
	rule1.SetInterval(2);
	rule1.SetDtStart(calStartTime); // every 2 days from 13 May
	rule1.SetCount(30);
	
	entry1->SetStartAndEndTimeL(calStartTime, calStartTime);
	entry1->SetRRuleL(rule1);

// Entry Number 2 child of 1
	TTime recId(startTime + TTimeIntervalDays(4)); // 09:00 on 17 May
	TCalTime calRecId;
	calRecId.SetTimeLocalL(recId);

	// create modifying entry with different start time
	guid1 = _L8("test1").AllocLC();
	CCalEntry* entry2 = CCalEntry::NewL(CCalEntry::EAppt, guid1, CCalEntry::EMethodNone, 0,
			calRecId, CalCommon::EThisAndFuture);
	CleanupStack::Pop(guid1);
	CleanupStack::PushL(entry2);

	TTime newStartTime(recId + TTimeIntervalHours(3)); // 12:00 on 17 May
	TCalTime calNewStartTime;
	calNewStartTime.SetTimeLocalL(newStartTime);
	entry2->SetStartAndEndTimeL(calNewStartTime, calNewStartTime);
	
	//
	// create original repeating entry 

// Entry Number 3 parent
	HBufC8* guid2 = _L8("test2").AllocLC();
	CCalEntry* entry3 = CCalEntry::NewL(CCalEntry::EAppt, guid2, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid2);
	CleanupStack::PushL(entry3);

	TTime startTime2(TDateTime(2005, EJune, 12, 9, 0, 0, 0)); // 09:00 on 13 June
	calStartTime.SetTimeLocalL(startTime2);
	
	TCalRRule rule2(TCalRRule::EDaily);
	rule2.SetInterval(2);
	rule2.SetDtStart(calStartTime); // every 2 days from 13 June
	rule2.SetCount(30);
	
	entry3->SetStartAndEndTimeL(calStartTime, calStartTime);
	entry3->SetRRuleL(rule2);

//Entry Number 4 child of 3
	TTime recId2(startTime2 + TTimeIntervalDays(4)); // 09:00 on 17 June
	calRecId.SetTimeLocalL(recId2);

	// create modifying entry with different start time AND RDATE
	guid2 = _L8("test2").AllocLC();
	CCalEntry* entry4 = CCalEntry::NewL(CCalEntry::EAppt, guid2, CCalEntry::EMethodNone, 0,
			calRecId, CalCommon::EThisAndFuture);
	CleanupStack::Pop(guid2);
	CleanupStack::PushL(entry4);

	TTime newStartTime2(recId2 + TTimeIntervalHours(3)); // 12:00 on 17 June
	calNewStartTime.SetTimeLocalL(newStartTime2);
	entry4->SetStartAndEndTimeL(calNewStartTime, calNewStartTime);
	
	TTime newRDateTime(recId2 + TTimeIntervalHours(3) + TTimeIntervalDays(1)); // 12:00 on 18 June
	calNewStartTime.SetTimeLocalL(newRDateTime);

	RArray<TCalTime> rDates;
	rDates.AppendL(calNewStartTime);
	entry4->SetRDatesL(rDates);
	rDates.Reset();

	//
	// create original repeating entry 

//Entry Number 5
	HBufC8* guid3 = _L8("test3").AllocLC();
	CCalEntry* entry5 = CCalEntry::NewL(CCalEntry::EAppt, guid3, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid3);
	CleanupStack::PushL(entry5);

	TTime startTime3(TDateTime(2005, EJuly, 12, 9, 0, 0, 0)); // 09:00 on 13 July
	calStartTime.SetTimeLocalL(startTime3);
	
	TCalRRule rule3(TCalRRule::EDaily);
	rule3.SetInterval(2);
	rule3.SetDtStart(calStartTime); // every 2 days from 13 July
	rule3.SetCount(30);
	
	entry5->SetStartAndEndTimeL(calStartTime, calStartTime);
	entry5->SetRRuleL(rule3);

// Entry Number 6 (child of 5)
	TTime recId3(startTime3 + TTimeIntervalDays(4)); // 09:00 on 17 July
	calRecId.SetTimeLocalL(recId3);

	// create modifying entry with different start time AND RDATE
	guid3 = _L8("test3").AllocLC();
	CCalEntry* entry6 = CCalEntry::NewL(CCalEntry::EAppt, guid3, CCalEntry::EMethodNone, 0,
			calRecId, CalCommon::EThisAndFuture);
	CleanupStack::Pop(guid3);
	CleanupStack::PushL(entry6);

	TTime newStartTime3(recId3 + TTimeIntervalHours(3)); // 12:00 on 17 July
	calNewStartTime.SetTimeLocalL(newStartTime3);
	entry6->SetStartAndEndTimeL(calNewStartTime, calNewStartTime);
	
	TCalRRule rule4(TCalRRule::EDaily);
	rule4.SetInterval(3);
	rule4.SetDtStart(calNewStartTime); // every 3 days from 17 July
	
	TCalTime rptEnd;
	rptEnd.SetTimeLocalL(newStartTime3 + TTimeIntervalDays(30));
	rule4.SetUntil(rptEnd);
	entry6->SetRRuleL(rule4);
	
	aEntryArray.AppendL(entry1);
	aEntryArray.AppendL(entry2);
	aEntryArray.AppendL(entry3);
	aEntryArray.AppendL(entry4);
	aEntryArray.AppendL(entry5);
	aEntryArray.AppendL(entry6);
	
	CleanupStack::Pop(entry6);
	CleanupStack::Pop(entry5);
	CleanupStack::Pop(entry4);
	CleanupStack::Pop(entry3);
	CleanupStack::Pop(entry2);
	CleanupStack::Pop(entry1);
	}


void CDataExchangeTestManager::CreateEntriesL(CCalEntry::TType aType, RPointerArray<CCalEntry>& aEntryArray)
	{
	HBufC8* guid4 = _L8("test4").AllocLC();
	CCalEntry* entry7 = CCalEntry::NewL(aType, guid4, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid4);
	CleanupStack::PushL(entry7);

	TTime startTime4(TDateTime(2005, EJuly, 12, 9, 0, 0, 0)); // 09:00 on 13 July
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime4);
	TTime endTime4(TDateTime(2005, EJuly, 12, 10, 0, 0, 0)); // 10:00 on 13 July
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime4);
	entry7->SetStartAndEndTimeL(calStartTime, calEndTime);

	aEntryArray.AppendL(entry7);
	
	CleanupStack::Pop(entry7);
	}

	
void CDataExchangeTestManager::CreateAlarmTestEntriesL(CCalEntry::TType aType, RPointerArray<CCalEntry>& aEntryArray)
	{
	HBufC8* guid5 = _L8("test5").AllocLC();
	CCalEntry* entry8 = CCalEntry::NewL(aType, guid5, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid5);
	CleanupStack::PushL(entry8);

	TTime startTime5(TDateTime(2005, EJuly, 12, 9, 0, 0, 0)); // 09:00 on 13 July
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime5);
	TTime endTime5(TDateTime(2005, EJuly, 12, 10, 0, 0, 0)); // 10:00 on 13 July
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime5);
	entry8->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	CCalAlarm* aAlarm = CCalAlarm::NewL();
	CleanupStack::PushL(aAlarm);
	aAlarm->SetTimeOffset(10);
	_LIT(SoundName, "hello");
	aAlarm->SetAlarmSoundNameL(SoundName);
	entry8->SetAlarmL(aAlarm);
	CleanupStack::PopAndDestroy(aAlarm);

	// create repeating alarm test entry 
	
	HBufC8* guid6 = _L8("test6").AllocLC();
	CCalEntry* entry9 = CCalEntry::NewL(aType, guid6, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid6);
	CleanupStack::PushL(entry9);
	
	TCalRRule rule3(TCalRRule::EDaily);
	rule3.SetInterval(2);
	rule3.SetDtStart(calStartTime); // every 2 days from 13 July
	rule3.SetCount(30);
	
	entry9->SetStartAndEndTimeL(calStartTime, calEndTime);
	entry9->SetRRuleL(rule3);
	
	CCalAlarm* aAlarm2 = CCalAlarm::NewL();
	CleanupStack::PushL(aAlarm2);
	
	aAlarm2->SetTimeOffset(10);
	aAlarm2->SetAlarmSoundNameL(SoundName);
	entry9->SetAlarmL(aAlarm2);
	
	CleanupStack::PopAndDestroy(aAlarm2);


	aEntryArray.AppendL(entry8);
	aEntryArray.AppendL(entry9);
	
	CleanupStack::Pop(entry9);	
	CleanupStack::Pop(entry8);
	}

/*************************************************************
*CDataExchangeTestManager::CheckStringInFileL() checks whether a string exists in a prticular file.
* @param aString: The string/descriptor to be searched for.
* @param aFileName: The file to be searched in. 
* @param aToBeFound: If true search for existance of the string in the file. If false search for 
*             nonexistence of the string in the file
* @return ETrue if the string exists in the File and aToBeFound == ETrue
*         EFalse if the string does not exist in the File and aToBeFound == EFalse
***************************************************************/
TBool CDataExchangeTestManager::CheckStringInFileL(const TDesC8 &aString, const TDesC &aFileName, TBool aToBeFound)
	{
	TBool result = EFalse ;
	RFile file;
	HBufC8* buffer = HBufC8::NewL(KMaxSize);
	
	TInt err(file.Open(iTestLib->FileSession(), aFileName, EFileRead | EFileShareAny));
	if (err == KErrNone)
		{			
		TPtr8 ptr(buffer->Des());
		err = file.Read(ptr);
		if (err == KErrNone)
			{
			TInt pos(ptr.Find(aString));
			
			result = ((pos != KErrNotFound) && (aToBeFound)) 
							|| ((pos == KErrNotFound) && (!aToBeFound)) ;			
			
			file.Close();
			}		
			
		}
	delete buffer ;		
	return result ;
	}

void CDataExchangeTestManager::ExportImportAndCheckL(RPointerArray<CCalEntry>& aEntryArray, TBool aIsSynch)
	{
	ExportL(KEntryExportFileGSEntries, aEntryArray, aIsSynch);
	const TInt entrycount = aEntryArray.Count();
	for (TInt ii=0;ii<entrycount;++ii)
		{
		const TInt attachcount = aEntryArray[ii]->AttachmentCountL();
		for(TInt jj=0;jj<attachcount;++jj)
			{
			CCalAttachment* attach = aEntryArray[ii]->AttachmentL(jj);	
			if(attach && attach->FileAttachment())
				{
				attach->FileAttachment()->LoadBinaryDataL();	
				}
			}
		}

	RPointerArray<CCalEntry> importedArray;
	CleanupResetAndDestroyPushL(importedArray);
	ImportL(KEntryExportFileGSEntries, importedArray, aIsSynch);
	test(aEntryArray.Count() == importedArray.Count());
	
	// remove each of the imported entries from entryArray to check they are all there
	test(CheckTwoArrayEntryL(aEntryArray, importedArray));
	
	CleanupStack::PopAndDestroy(&importedArray);
	}

TBool CDataExchangeTestManager::CheckTwoArrayEntryL(RPointerArray<CCalEntry>& aSouceArray, RPointerArray<CCalEntry>& aCompareArray)
	{
	for (TInt i = 0; i < aSouceArray.Count(); i++)
		{
		for (TInt j = 0; j < aCompareArray.Count(); j++)
			{
			if (aCompareArray[j]->CompareL(*aSouceArray[i]))
				{
				delete aCompareArray[j];
				aCompareArray.Remove(j);
				j = aCompareArray.Count();
				}
			}
		}

	return aCompareArray.Count()==0;
	}

void CDataExchangeTestManager::TestGSEntriesL(TBool aIsSynch)
	{
	test.Next(_L("TestGSEntriesL()"));
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CreateGSEntriesL(entryArray);

	// store the entries in the database, this will change them slightly
	TInt stored;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, stored);
	test(stored == entryArray.Count());
	entryArray.ResetAndDestroy();
	// fetch the first pair of entries, export and import them and check they haven't changed
	iTestLib->SynCGetEntryViewL().FetchL(_L8("test1"), entryArray);
	ExportImportAndCheckL(entryArray, aIsSynch);
	entryArray.ResetAndDestroy();
	
	// fetch the next pair of entries, export and import them and check they haven't changed
	iTestLib->SynCGetEntryViewL().FetchL(_L8("test2"), entryArray);
	ExportImportAndCheckL(entryArray, aIsSynch);
	entryArray.ResetAndDestroy();
	
	// fetch the next pair of entries, export and import them and check they haven't changed
	iTestLib->SynCGetEntryViewL().FetchL(_L8("test3"), entryArray);
	ExportImportAndCheckL(entryArray, aIsSynch);
	
	CleanupStack::PopAndDestroy(&entryArray);	
	}


void CDataExchangeTestManager::TestEntriesL(CCalEntry::TType aType, TBool aIsSynch)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CreateEntriesL(aType, entryArray);
	
	ExportImportAndCheckL(entryArray, aIsSynch);
	
	// store the entries in the database
	TInt stored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, stored);
	test(stored == entryArray.Count());
	entryArray.ResetAndDestroy();
	
	//fetch the entries, export and import them and check.
	iTestLib->SynCGetEntryViewL().FetchL(_L8("test4"), entryArray);
	ExportImportAndCheckL(entryArray, aIsSynch);
	
	CleanupStack::PopAndDestroy(&entryArray);	
	}

	
void CDataExchangeTestManager::TestAlarmEntriesL(CCalEntry::TType aType, TBool aIsSynch)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CreateAlarmTestEntriesL(aType, entryArray);
	
	ExportImportAndCheckL(entryArray, aIsSynch);
	
	// store the entries in the database
	TInt stored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, stored);
	test(stored == entryArray.Count());
	entryArray.ResetAndDestroy();
	
	//fetch the entries, export and import them and check.
	iTestLib->SynCGetEntryViewL().FetchL(_L8("test5"), entryArray);
	ExportImportAndCheckL(entryArray, aIsSynch);
	
	entryArray.ResetAndDestroy();
	
	//fetch the entries, export and import them and check.
	iTestLib->SynCGetEntryViewL().FetchL(_L8("test6"), entryArray);
	ExportImportAndCheckL(entryArray, aIsSynch);
	
	CleanupStack::PopAndDestroy(&entryArray);		
	}


void CDataExchangeTestManager::TestEntryUpdateL(TBool aIsSynch)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	//	import existing entry generated earlier in previous test
	TBuf<64> GeneratedVCSFile;
	GeneratedVCSFile.Format(KEntryImportFile(), 2);
	ImportL(GeneratedVCSFile, entryArray, aIsSynch);	

	//	store imported entry into the agenda file
	TInt stored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, stored);
	test(stored == entryArray.Count());
	test(entryArray[0]->AttendeesL().Count() == 2);

	entryArray.ResetAndDestroy();	
	
	//	fetch entry from the agenda file
	iTestLib->SynCGetEntryViewL().FetchL(KEntry3UID, entryArray);	
	TInt NumOfAttendees = entryArray[0]->AttendeesL().Count();
	
	test.Printf(_L("attendees in GUID 1001 before updating!\n"));
	for(TInt i=0; i< NumOfAttendees; ++i)
		{
		PrintAttendeeL(entryArray[0]->AttendeesL()[i]);
		}
	
	//	delete attendee from the entry and change common name and role of existing attendee	
	entryArray[0]->DeleteAttendeeL(0);
	RPointerArray<CCalAttendee> attendeeList = entryArray[0]->AttendeesL();
	NumOfAttendees = attendeeList.Count();
	attendeeList[0]->SetRoleL(CCalAttendee::EChair);
	_LIT(KCommonName, "Common Name");
	attendeeList[0]->SetCommonNameL(KCommonName);
	
	//	update the changes in the agenda file
	iTestLib->SynCGetEntryViewL().UpdateL(entryArray, stored);
	test(stored == entryArray.Count());
	
	//	fetch resulting entry from the agenda file
	iTestLib->SynCGetEntryViewL().FetchL(KEntry3UID, entryArray);
	NumOfAttendees = entryArray[0]->AttendeesL().Count();

	//	test update has worked 
	test(NumOfAttendees == 1);
	test(entryArray[0]->AttendeesL()[0]->CommonName() == KCommonName);	
	
	test.Printf(_L("attendees in GUID 1001 after updating!\n"));
	for(TInt i=0; i< NumOfAttendees; ++i)
		{
		PrintAttendeeL(entryArray[0]->AttendeesL()[i]);
		}
		
	CleanupStack::PopAndDestroy(&entryArray);
	}


void CDataExchangeTestManager::TestOrganizerIsStoredL(TBool aIsSynch)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	_LIT(KExampleCommonName, "George Best");
	_LIT(KExampleAddress, "GeorgeIsTheBest@hotmail.com");	

	test.Next(_L("test that the organizer is stored and retrievable during re-import\n"));		
	
	//	create new test file with organizer
	CreateTestFileL(KEntry3, KOrganizerTestFile);
	TInt stored(0);	
	ImportL(KOrganizerTestFile, entryArray, aIsSynch);
	test(entryArray[0]->OrganizerL() != NULL);

	//	replace the organizer in the agenda file with a new one	
	CCalUser* newOrganizer = CCalUser::NewL(KExampleAddress);
	CleanupStack::PushL(newOrganizer);
	newOrganizer->SetCommonNameL(KExampleCommonName);
	entryArray[0]->SetOrganizerL(newOrganizer);
	CleanupStack::Pop(newOrganizer);
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, stored);
	test(stored == entryArray.Count());
	entryArray.ResetAndDestroy();	

	iTestLib->SynCGetEntryViewL().FetchL(KEntry3UID, entryArray);
	test(entryArray[0]->OrganizerL()->CommonName() == KExampleCommonName);		
	entryArray.ResetAndDestroy();		
	
	//	test that organizer gets overwritten when updating over it
	ImportL(KOrganizerTestFile, entryArray, aIsSynch);
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, stored);
	test(stored == entryArray.Count());
	entryArray.ResetAndDestroy();	
	
	iTestLib->SynCGetEntryViewL().FetchL(KEntry3UID, entryArray);
	test(entryArray[0]->OrganizerL()->CommonName() == _L("Example Person"));
			
	CleanupStack::PopAndDestroy(&entryArray);	
	}

	
//Test code for DEF064929
void CDataExchangeTestManager::TestReminderEntryExportL(TBool aIsSynch)
	{
	TInt success(0);
	TTime time1;
	TCalTime caltime1;
	TCalTime caltime2;
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	test.Next(_L("Exporting Reminder Entry to vcal\n"));	
	
	time1.HomeTime();
	TTime time2(time1);
	time2 += TTimeIntervalHours(2);
	time2 += TTimeIntervalYears(5);
	
	//Create a Reminder entry
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EReminder, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	caltime1.SetTimeUtcL(time1);
	caltime2.SetTimeUtcL(time2);
	
	entry->SetStartAndEndTimeL(caltime1, caltime2);
	
	entry->SetSummaryL(KSUMMARY);
	
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	test(entryArray.Count() == success);
	
	delete entry;
	entry = NULL;
	entryArray.Reset();
	
	iTestLib->SynCGetEntryViewL().FetchL(KGUID, entryArray);
	test(entryArray.Count() == 1);
	
	//Export Reminder entry
	ExportL(KEntryExportFileReminderEntries, entryArray, aIsSynch);
	
	CleanupStack::PopAndDestroy(&entryArray);	//	entryArray.ResetAndDestroy();
	}


//Test code for DEF064929
void CDataExchangeTestManager:: TestReminderEntryImportL(TBool aIsSynch)
	{
	TInt success(0);
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	test.Next(_L("Importing Reminder Entry from vcal\n"));
	
	//Import Reminder entry
	ImportL(KEntryExportFileReminderEntries, entryArray, aIsSynch);
	
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	test(entryArray.Count() == success);
	
	//Check whether imported entryType which is Reminder is equal to exported
	CCalEntry::TType entryType = entryArray[0]->EntryTypeL();
	test(entryType == CCalEntry::EReminder);
	
	//Check whether imported summary is equal to exported
	TPtrC summary = entryArray[0]->SummaryL();
	test(summary == KSUMMARY);
	
	TPtrC description = entryArray[0]->DescriptionL();
	test(description == KNullDesC);
	
	CleanupStack::PopAndDestroy(&entryArray);	//	entryArray.ResetAndDestroy();
	}


void CDataExchangeTestManager::TestAsyncExportL(const TDesC& aFileName)
	{
	RPointerArray<CCalEntry> entriesToExport;
	CleanupResetAndDestroyPushL(entriesToExport);
	TTime time(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0)); // 9:00 on 1 Jan 05

	// create a number of entries, each with a different UID and start date.
	for (TInt i = 0; i < iNumEntriesToImportAndExport; i++)
		{
		TBuf8<16> uid;
		uid.Copy(KDummyID);
		uid.AppendNum(i);
		
		HBufC8* uidHBuf = uid.AllocLC();
		CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, uidHBuf, CCalEntry::EMethodNone, 0);
		CleanupStack::Pop(uidHBuf);
		CleanupStack::PushL(entry);
		
		TCalTime entryTime;
		entryTime.SetTimeLocalL(time);
		entry->SetStartAndEndTimeL(entryTime, entryTime);
		
		entriesToExport.AppendL(entry);
		CleanupStack::Pop(entry);
		
		time += TTimeIntervalDays(1); // increment the start date for the next entry
		}
	
	test.Printf(_L("Exporting entries asynchronously\n"));
	
	ExportL(aFileName, entriesToExport, EFalse);

	CleanupStack::PopAndDestroy(&entriesToExport); // entriesToExport.ResetAndDestroy();
	}

void CDataExchangeTestManager::TestAsyncImportL(const TDesC& aFileName)
	{
	// re-import the entries that have just been exported
	RPointerArray<CCalEntry> importedEntries;
	CleanupResetAndDestroyPushL(importedEntries);

	test.Printf(_L("Importing entries asynchronously\n"));

	RFile infile;
	iTestLib->TestRegister().OpenTempFileLC(infile, aFileName);
	RFileReadStream readStream(infile);
	CleanupClosePushL(readStream);
	ImportL(aFileName, importedEntries, EFalse);
	
	test(importedEntries.Count() == iNumEntriesToImportAndExport);
	
	CreateTestFileL(KNullDesC8, KEntryExportFile);
	ExportL(KEntryExportFile, importedEntries, EFalse);
	
	RPointerArray<CCalEntry> reImportedEntry;
	CleanupResetAndDestroyPushL(reImportedEntry);
			
	ImportL(KEntryExportFile, reImportedEntry, EFalse);
	test(importedEntries.Count() == reImportedEntry.Count());
	
	// remove each of the imported entries from entryArray to check they are all there
	test(CheckTwoArrayEntryL(importedEntries, reImportedEntry));
	CleanupStack::PopAndDestroy(4, &importedEntries);
	}
	
void CDataExchangeTestManager::TestFloatingExceptionL(TBool aIsSynch)
	{
	_LIT(KFloatingExceptionFile, "floating_exception1.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	test.Printf(_L("Importing entry with floating exception is imported"));
	CreateTestFileL(KFloatingException, KFloatingExceptionFile);
	ImportL(KFloatingExceptionFile, entries, aIsSynch);
	test.Printf(_L("Check exception was imported ok"));
	test(entries.Count() == 1);
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	entries[0]->GetExceptionDatesL(exceptions);
	test(exceptions.Count() == 1);
	test(exceptions[0].TimeMode() == TCalTime::EFloating);
	CleanupStack::PopAndDestroy(&exceptions); // exceptions.Close

	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy
	}
	

void CDataExchangeTestManager::TestMultipleAsyncImportL()
    {
	RPointerArray<HBufC8> entryData;
	CleanupResetAndDestroyPushL( entryData );
	
    // Write vcal entries to file.
	entryData.AppendL( KEntry1().AllocL() );
    CreateTestFileL( *entryData[0], KEntry1File );
    CleanupStack::PopAndDestroy( &entryData );

    // Create read stream to file for importing.
	RFile importFile;
	iTestLib->TestRegister().OpenTempFileLC( importFile, KEntry1File );
	RFileReadStream readStream( importFile);
	CleanupClosePushL( readStream );

    // Import the calendar entries asynchronously. 
    RPointerArray<CCalEntry> importedEntries;
    CleanupResetAndDestroyPushL( importedEntries );
    iDataExchange->ImportAsyncL( KUidVCalendar, readStream, importedEntries, 
	    *iAsyncImportCallback, 0 );

	// Start the scheduler. It is stopped when the async import is complete, 
	// allowing us to verify the number of entries imported below.
    CActiveScheduler::Start();
    
    TInt numEntries = importedEntries.Count();
    test( numEntries == 2 );

    importedEntries.ResetAndDestroy();
    CleanupStack::PopAndDestroy( &importedEntries );
    CleanupStack::PopAndDestroy( &readStream );  // readStream.Close()
    CleanupStack::PopAndDestroy( &importFile );  // importFile.Close()
    }
void CDataExchangeTestManager::SetNumEntriesToImportAndExport(TInt aNumEntries)
	{
	iNumEntriesToImportAndExport = aNumEntries;
	}


void CDataExchangeTestManager::BadExceptionDateTestL(TBool aIsSynch)
	{
	// MS Outlook doesn't set exception dates properly.
	CreateTestFileL(KEntry4, KBadExceptionFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	ImportL(KBadExceptionFile, entries, aIsSynch);

	test(entries.Count() == 1);

	CCalEntry* entry = entries[0];
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	entry->GetExceptionDatesL(exceptions);
	
	test(exceptions.Count() == 1);
	TTime exceptionDate = exceptions[0].TimeUtcL();
	const TTime KComparisonDate = TTime(TDateTime(2005, ENovember, 7, 6, 0, 0, 0)); // 6am on 8 Nov
	test(exceptionDate == KComparisonDate);
	
	CleanupStack::PopAndDestroy();//exceptions.Reset();
	CleanupStack::PopAndDestroy();//entries.ResetAndDestroy();
	}

void CDataExchangeTestManager::TestFixedAndFloatingTodoL(TBool aIsSynch)
	{
	_LIT(KFloatingTodoFile, "floating_todo1.vcs");
	_LIT(KFixedTodoFile, "fixed_todo1.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	test.Printf(_L("Importing floating todo\n"));
	CreateTestFileL(KFloatingTodo, KFloatingTodoFile);
	ImportL(KFloatingTodoFile, entries, aIsSynch);
	CCalEntry* entry = entries[0];
	test.Printf(_L("Check start time not the same as end time\n"));
	test(entry->StartTimeL().TimeLocalL() != entry->EndTimeL().TimeLocalL());
	
	entries.ResetAndDestroy();
	
	test.Printf(_L("Importing fixed todo\n"));
	CreateTestFileL(KFixedTodo, KFixedTodoFile);
	ImportL(KFixedTodoFile, entries, aIsSynch);
	entry = entries[0];
	test.Printf(_L("Check start time not the same as end time\n"));
	test(entry->StartTimeL().TimeLocalL() != entry->EndTimeL().TimeLocalL());
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy
	}
 
void CDataExchangeTestManager::TestTodoWithoutDueDateL(TBool aIsSynch)
	{
	_LIT(KFloatingTodoFile, "floating_todo2.vcs");
	_LIT(KFixedTodoFile, "fixed_todo2.vcs");
	const TTime KExpectedTime(TDateTime(1999, ENovember, 0, 10, 0, 0, 0));
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	test.Printf(_L("Importing floating todo without due date\n"));
	CreateTestFileL(KFloatingTodoWithoutDueDate, KFloatingTodoFile);
	ImportL(KFloatingTodoFile, entries, aIsSynch);
	CCalEntry* entry = entries[0];
	test.Printf(_L("Check start time is non-null time and end time is null time\n"));
	test(entry->StartTimeL().TimeLocalL() == KExpectedTime);
	test(entry->EndTimeL().TimeLocalL() == KExpectedTime);
	
	entries.ResetAndDestroy();
	
	test.Printf(_L("Importing fixed todo without due date\n"));
	CreateTestFileL(KFixedTodoWithoutDueDate, KFixedTodoFile);
	ImportL(KFixedTodoFile, entries, aIsSynch);
	entry = entries[0];
	test.Printf(_L("Check start time is non-null time and end time is null time\n"));
	test(entry->StartTimeL().TimeUtcL() == KExpectedTime);
	test(entry->EndTimeL().TimeUtcL() == KExpectedTime);
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy
	}
 
void CDataExchangeTestManager::TestAlarmedTodoWithoutDueDateL(TBool aIsSynch)
	{
	_LIT(KFloatingTodoFile, "floating_alm_todo2.vcs");
	_LIT(KFixedTodoFile, "fixed_alm_todo2.vcs");
	const TTime KExpectedStartTime(TDateTime(1999, ENovember, 0, 10, 0, 0, 0));
	const TTime KExpectedEndTime(TDateTime(1999, ENovember, 0, 10, 25, 0, 0));
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	test.Printf(_L("Importing floating alarmed todo without due date\n"));
	CreateTestFileL(KFloatingAlarmedTodoWithoutDueDate, KFloatingTodoFile);
	ImportL(KFloatingTodoFile, entries, aIsSynch);
	CCalEntry* entry = entries[0];
	test.Printf(_L("Check start time is non-null time and end time is null time\n"));
	test(entry->StartTimeL().TimeLocalL() == KExpectedStartTime);
//	test(entry->EndTimeL().TimeLocalL() == KExpectedEndTime);
	CCalAlarm* alarm = entry->AlarmL();
	test(alarm->TimeOffset().Int() != 0);
	delete alarm;
	
	entries.ResetAndDestroy();
	
	test.Printf(_L("Importing fixed alarmed todo without due date\n"));
	CreateTestFileL(KFixedAlarmedTodoWithoutDueDate, KFixedTodoFile);
	ImportL(KFixedTodoFile, entries, aIsSynch);
	entry = entries[0];
	test.Printf(_L("Check start time is non-null time and end time is null time\n"));
	test(entry->StartTimeL().TimeUtcL() == KExpectedStartTime);
//	test(entry->EndTimeL().TimeUtcL() == KExpectedEndTime);
	alarm = entry->AlarmL();
	test(alarm->TimeOffset().Int() != 0);
	delete alarm;
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy
	}

void CDataExchangeTestManager::TestEntriesWithoutDatesL(TBool aIsSynch)
	{
	_LIT(KEntriesWithoutDatesFile, "entries_without_dates1.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	test.Printf(_L("Importing entries without dates\n"));
	CreateTestFileL(KEntriesWithoutDates, KEntriesWithoutDatesFile);
	ImportL(KEntriesWithoutDatesFile, entries, aIsSynch);
	test.Printf(_L("Check that only the todo entry can be imported without a date\n"));
	test(entries.Count() == 1);
	test(entries[0]->EntryTypeL() == CCalEntry::ETodo);

	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy
	}

void CDataExchangeTestManager::TestTodoCompletedDateL(TBool aIsSynch)
	{
	_LIT(KCompletedTodoFile, "completed_todo1.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	test.Printf(_L("Importing completed todo\n"));
	CreateTestFileL(KTodoCompletedDate, KCompletedTodoFile);
	
	ImportL(KCompletedTodoFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	test.Printf(_L("Check completed was imported ok\n"));
	CCalEntry* entry = entries[0];
	TTime compUtc = entry->CompletedTimeL().TimeUtcL();
	test(compUtc == TTime(TDateTime(1999, ENovember, 0, 8, 0, 0, 0))); // COMPLETED:19991101T080000
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy
	}

void CDataExchangeTestManager::ModifyRRuleL(CCalEntry& aEntry, 
											TInt aStartYear, TInt aEndYear, TBool aRepeatForever, TMonth aMonth)
	{
	// entry happens once in two years on the 7th of aMonth, starting aStartYear.
	// Its duration is 30 minutes (from 6:00 to 6:30).
	// The repeats end on the 7th of aMonth on aEndYear (unless aRepeatForever is specified).
	TCalTime startTime = TCalTimeUtil::CalTimeL(aStartYear, aMonth, 6, 06,00,00, 0);
	TCalTime entryEndTime = TCalTimeUtil::CalTimeL(aStartYear, aMonth, 6, 06,30,00, 0);

	CTzRules* tzRules = aEntry.TzRulesL();
	CleanupStack::PushL(tzRules);

	aEntry.SetStartAndEndTimeL(startTime, entryEndTime);
	
	TCalRRule yearlyRpt(TCalRRule::EYearly);
	yearlyRpt.SetInterval(2);
	yearlyRpt.SetDtStart(startTime);
	if (!aRepeatForever)
		{
		TCalTime rptEndTime = TCalTimeUtil::CalTimeL(aEndYear, aMonth, 6, 06,00,00, 0);
		yearlyRpt.SetUntil(rptEndTime);
		}

	TRAPD(err, aEntry.SetRRuleL(yearlyRpt));
	test(err!=KErrArgument);

	aEntry.SetTzRulesL(*tzRules);	// keep tz rules
	
	CleanupStack::PopAndDestroy(tzRules);
	}


TBool CDataExchangeTestManager::TestEntriesForTzPropertiesL(RPointerArray<CCalEntry>& aEntryArray, 
														    TInt* aExpectedYears, TBool aIsSynch)
	{
	TBool success(ETrue);
	
	ExportL(KTestTzExportFile, aEntryArray, aIsSynch);	
	
	RArray<TInt> yearsPresent;
	CleanupClosePushL(yearsPresent);
	GetDaylightYearsL(KTestTzExportFile, yearsPresent);
	
	// check
	TInt count;
	for (count=0; aExpectedYears[count]!=(-1); count++)
		{
		if (yearsPresent.Find(aExpectedYears[count])==KErrNotFound)
			{
			success = EFalse;
			}
		}
	if (yearsPresent.Count() != count)
		{
		success = EFalse;
		}
	
	CleanupStack::PopAndDestroy(&yearsPresent);
	
	return success;
	}
	

TInt CDataExchangeTestManager::GetDaylightYear(const TDesC8& aLine)
	{
	_LIT8(KDaylight,"DAYLIGHT");

	TInt retValue(-1);

	if (aLine.Length()>=23)
		{
		TBufC8<8> first8chars(aLine.Left(8));
		if ( first8chars.Compare(KDaylight())==0 )
			{
			TLex8 yearStr(aLine.Mid(18,4));
			if (yearStr.Val(retValue) != KErrNone)
				{
				retValue = -1;
				}
			}
		}
	return retValue;
	}


TInt CDataExchangeTestManager::GetDaylightYearsL(const TDesC& aFile, RArray<TInt>& aYears)
	{

	TFileName fullPath;
	iTestLib->TestRegister().GetTempPath(fullPath);
	fullPath.Append(aFile);
	RFile inFile;
	TInt result = inFile.Open(iTestLib->FileSession(), fullPath, EFileShareReadersOnly);
	User::LeaveIfError(result);
	RFileReadStream stream(inFile);
	CleanupClosePushL(stream);

	TBuf8<0x100> line;
	TInt err = KErrNone;

	aYears.Reset();
	do 	{
		TRAP(err,stream.ReadL(line,TChar(0xa)));
		if (err!=KErrNone && err!=KErrEof)
			{
			return err;
			}
		TInt year = GetDaylightYear(line);
		if ( year != -1 )
			{
			aYears.Append(year);
			}
		}
	while (err!=KErrEof);

	CleanupStack::PopAndDestroy(); // stream.Close()
	
	return KErrNone;
	}

void CDataExchangeTestManager::ChangeTimeZoneAndRunTestL(const TDesC8& aTimeZone, Test aTestToRun, TBool aIsSynch)
	{
	SetTimeZoneL(aTimeZone);
	(this->*aTestToRun)(aIsSynch);
	SetTimeZoneL(KDefaultTimeZone);
	}

void CDataExchangeTestManager::SetTimeZoneL(const TDesC8& aTimeZone)
	{
	// Set up user time zone
	TBuf<64> temp;
	temp.Copy(aTimeZone);
	test.Printf(_L("\nSetting timezone to: %S..."), &temp);
	iTestLib->SetTimeZoneL(aTimeZone);
	}

void CDataExchangeTestManager::TestTzPropertiesL(TBool aIsSynch)
	{
	// DEF075815
	// DAYLIGHT properties exported for start year of event, and for not more than 5 years
	// from current year onwards.
	test.Next(_L("Test TZ and DAYLIGHT properties:\n"));

	TTime oldTime; 
	oldTime.HomeTime();

   	const TInt currYear = 2005;

	iTestLib->PIMTestServer().SetHomeTime(TDateTime(currYear, EJanuary, 10, 10, 0, 0, 0));	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	CreateTestFileL(KEntry4, KTestTzImportFile);
	ImportL(KTestTzImportFile, entryArray, aIsSynch);
	test(entryArray.Count()==1);
	// we work only with one entry in the array at all times
	CCalEntry& entry = *entryArray[0];

	// original rules are preserved for the entry 
	// for KEntry4, expected output DAYLIGHT properties for only 1 year: { 2005 }.
	TInt expected0[] = { 2005, -1 };
	test( TestEntriesForTzPropertiesL(entryArray, expected0, aIsSynch) );

	entry.SetTzRulesL();	// drop old rules, and get tzrules from tz database

 	// Forever repeating entry: (2005: 1916-forever) expected output: { 1916, 2005-2009 }
 	// Why 1916? For the timezone we are using for this test; Europe/London, we 
 	// only have rules that go back as far as 21st May 1916 
 	// (see common\app-services\tzcompiler\Release\Data\europe). Any further back
 	// and we will generate unknown results 	
 	
 	// Set the start month of the event to June to ensure it's in the summer. 
 	// In this case the daylight saving rule for the start year of the event 
 	// should be output along with the 5 subsequent years from the current year (inclusive).
 	ModifyRRuleL(entry, 1916,0, ETrue, EJune);
	TInt expected1[] = { 1916, 2003, 2004, 2005, 2006, 2007, 2008, 2009, -1 };
	test( TestEntriesForTzPropertiesL(entryArray, expected1, aIsSynch) );

 	// Set the start month of the event to November to ensure it's in the winter. 
 	// In this case only 5 subsequent years from the current year (inclusive) 
 	// should be output.
 	ModifyRRuleL(entry, 1916,0, ETrue, ENovember);
	TInt expected2[] = { 2003, 2004, 2005, 2006, 2007, 2008, 2009, -1 };
	test( TestEntriesForTzPropertiesL(entryArray, expected2, aIsSynch) );
	
	// entry in the past: (2005: 1996-2001) expected output: { 1996 }.
	ModifyRRuleL(entry, 1996, 2001,	EFalse, EJune);	
	TInt expected3[] = { 1996, -1 };
	test( TestEntriesForTzPropertiesL(entryArray, expected3, aIsSynch) );
	
	// entry ends earlier than in 5 years: (2005: 1996, 2005-2007) expected output: { 1996, 2005-2007 }
	ModifyRRuleL(entry, 1996, currYear+2, EFalse, EJune);
	TInt expected4[] = { 1996, 2003, 2004, 2005, 2006, 2007, -1 };
	test( TestEntriesForTzPropertiesL(entryArray, expected4, aIsSynch) );
	
	// future entry: (2005: 2007-2013) expected output: { 2007-2011 }
	ModifyRRuleL(entry, currYear+2, currYear+8, EFalse, EJune);	
	TInt expected5[] = { 2007, 2008, 2009, 2010, 2011, -1 };
	test( TestEntriesForTzPropertiesL(entryArray, expected5, aIsSynch) );
	
	CleanupStack::PopAndDestroy(&entryArray);
	iTestLib->PIMTestServer().SetHomeTime(oldTime);	
	}
	
void CDataExchangeTestManager::ExtractAlarmL(TBool aIsSynch)
	{
	CreateTestFileL(KEntry11, KFileName);
	
	test.Printf(_L("Importing All entries\n"));
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CreateTestFileL(KEntry11, KFileName);
	ImportL(KFileName, entryArray, aIsSynch);
	test(entryArray.Count() == 4);	
		
	CCalEntry* entry;
	CCalAlarm* alarm;
	for (TInt i=0; i<entryArray.Count(); i++)
		{
		entry = entryArray[i] ;	
			
		alarm = entry->AlarmL() ;
		
		test.Printf(_L("Alarm was extracted ok\n"));		
		
		delete alarm;
		}
	
	CleanupStack::PopAndDestroy(&entryArray);	
	}
	
void CDataExchangeTestManager::ExportFloatingAlarmL(TBool aIsSynch)
	{
	// Create the vcal file "vcal_alarm.vcs"
	test.Next(_L("Test exporting floating alarm"));
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	CreateTestFileL(KEntry12, KFileName);

	test.Printf(_L("Importing the entry\n"));
	
	ImportL(KFileName(), entryArray, aIsSynch);
	test(entryArray.Count() == 1);	
		
	CCalEntry* entry;
	CCalAlarm* alarm;
	entry = entryArray[0] ;	
	alarm = entry->AlarmL() ;
	TTimeIntervalMinutes ato1 = alarm->TimeOffset();
	delete alarm;
	test(ato1.Int() == 80); // alarm offset in KEntry12 is 1h20
	
	TTime startTTime = entry->StartTimeL().TimeLocalL();
	TTime alarmTTime = startTTime - ato1;//calculate actual alarm time
	TDateTime startTime = startTTime.DateTime();
	TDateTime alarmTime = alarmTTime.DateTime();
	test.Printf(_L("Imported startdate: %d.%d.%d at %d:%d\n"),startTime.Day()+1, startTime.Month()+1, startTime.Year(), startTime.Hour(), startTime.Minute());
	test.Printf(_L("Imported alarm: %d.%d.%d at %d:%d\n"),alarmTime.Day()+1, alarmTime.Month()+1, alarmTime.Year(), alarmTime.Hour(), alarmTime.Minute());

	ExportL(KFileNameExp, entryArray, aIsSynch);

	CleanupStack::PopAndDestroy(&entryArray);

	RPointerArray<CCalEntry> entryArray1;
	CleanupResetAndDestroyPushL(entryArray1);
	
	ImportL(KFileNameExp, entryArray1, aIsSynch);
	
	test(entryArray1.Count() == 1);	

	entry = entryArray1[0] ;	
	alarm = entry->AlarmL() ;
	TTimeIntervalMinutes ato2 = alarm->TimeOffset();
	delete alarm;

	startTTime = entry->StartTimeL().TimeLocalL();
	alarmTTime = startTTime - ato2;//calculate actual alarm time
	startTime = startTTime.DateTime();
	alarmTime = alarmTTime.DateTime();

	test.Printf(_L("Imported startdate: %d.%d.%d at %d:%d\n"),startTime.Day()+1, startTime.Month()+1, startTime.Year(), startTime.Hour(), startTime.Minute());
	test.Printf(_L("Imported alarm: %d.%d.%d at %d:%d\n"),alarmTime.Day()+1, alarmTime.Month()+1, alarmTime.Year(), alarmTime.Hour(), alarmTime.Minute());
	test(ato1 == ato2);
	test(alarmTime.Hour() == 7 && alarmTime.Minute() == 40);
	
	CleanupStack::PopAndDestroy(&entryArray1);
	}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *	Test vCal Type Sequence
 *	Checks that entry types read correctly when importing multiple vCal entities.
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CDataExchangeTestManager::ListEntriesL(RPointerArray<CCalEntry>& aEntryArray)
	{
	TBuf<256> line;
	TBuf<64> timestr;
	TBuf<64> guid;
	
	TInt entriesCount = aEntryArray.Count();
	test.Printf(_L("Total %d entries:"), entriesCount);  
	
	for (TInt i=0; i<entriesCount; i++)
		{
		line.Zero();
		
		CCalEntry* entry = aEntryArray[i];
		line.AppendFormat(_L("Entry #%d:\t"), i);
		
		guid.Copy(entry->UidL());
		line.AppendFormat(_L("GUID=%S\t"), &guid);		

		switch (entry->EntryTypeL())
			{
		case CCalEntry::EAppt:		line.Append(_L("Appt\t")); break;
		case CCalEntry::ETodo:		line.Append(_L("Todo\t")); break;
		case CCalEntry::EEvent:		line.Append(_L("Event\t")); break;
		case CCalEntry::EReminder:	line.Append(_L("Reminder ")); break;
		case CCalEntry::EAnniv:		line.Append(_L("Anniv\t")); break;
			default:	line.Append(_L("UnknownType"));
			}

		if (entry->EntryTypeL() != CCalEntry::ETodo)
			{
			TCalTimeUtil::FormatL(timestr, entry->StartTimeL());
			line.AppendFormat(_L("DTSTART: %S\t"), &timestr);
			}
			
		TCalTime end = entry->EndTimeL();
		if (end.TimeUtcL() > TCalTime::MinTime())
			{
			TCalTimeUtil::FormatL(timestr, end); 
			}
		else
			{
			timestr.Copy(_L("NULL"));	
			}
		line.AppendFormat(_L("DTEND: %S\n"), &timestr);
			
		test.Printf(line);
		}
	}
	
static TInt CompareHBufC8(const HBufC8& aFirst, const HBufC8& aSecond)
	{
	return aFirst.CompareC(aSecond);
	}
	
void CDataExchangeTestManager::TestVCalTypeSequenceL(
					const TDesC8& aVCalData, 	// vCal data for test
					TInt aKEntryNum,			// KEntry number (used only for test name)
					TInt aExpectToStore,		// number of entries expected to be stored
					TEntryCheckData& aCheckData,// expected entry check data
					TBool aIsSynch
					)
	{
	TBuf<128> testName;
	testName.Format(_L("TestVCalTypeSequenceL(KEntry%d)"), aKEntryNum);
	test.Next(testName);
	CreateTestFileL(aVCalData, KTestTypeSeqFile);	

	RPointerArray<CCalEntry> entriesImported;
	CleanupResetAndDestroyPushL(entriesImported);
	RPointerArray<HBufC8> guidsArray;
	CleanupResetAndDestroyPushL(guidsArray);
	RPointerArray<CCalEntry> entriesRetrieved;
	CleanupResetAndDestroyPushL(entriesRetrieved);

	// Import entries
	ImportL(KTestTypeSeqFile, entriesImported, aIsSynch);
	test.Printf(_L("Listing imported entries...\n"));
	ListEntriesL(entriesImported);
	TLinearOrder<HBufC8> order(CompareHBufC8);
	TInt i = 0;
	for (i=0; i<entriesImported.Count(); i++)
		{
		HBufC8* guid = entriesImported[i]->UidL().AllocL();
		if ( guidsArray.InsertInOrder(guid, order) == KErrAlreadyExists )	// do not allow repeats
			{
			delete guid;	
			}
		}

	// Store entries
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entriesImported, success);
	test(entriesImported.Count() == success);
		
	// Retrieve stored entries
	test.Printf(_L("Retrieving stored entries...\n"));
	for(i=0; i<guidsArray.Count(); i++)
		{
		iTestLib->SynCGetEntryViewL().FetchL(*(guidsArray[i]), entriesRetrieved);
		}
	ListEntriesL(entriesRetrieved);
	test(entriesRetrieved.Count() == aExpectToStore);

	// Check expected entry	
	CCalEntry* checkEntry = entriesImported[aCheckData.iPos];
	test(checkEntry->EntryTypeL()==aCheckData.iType);
	test(checkEntry->StartTimeL().TimeLocalL()==aCheckData.iStartTime);
	test(checkEntry->EndTimeL().TimeLocalL()==aCheckData.iEndTime);

	CleanupStack::PopAndDestroy(&entriesRetrieved);	
	CleanupStack::PopAndDestroy(&guidsArray);
	CleanupStack::PopAndDestroy(&entriesImported);	
	}


void CDataExchangeTestManager::TestVCalTypeSequenceL(TBool aIsSynch)
	{
	TEntryCheckData expectedEntry;
	expectedEntry.iPos = 3;
	expectedEntry.iType = CCalEntry::EAppt;
	expectedEntry.iStartTime = TDateTime(1999, ENovember, 0, 10, 00, 04, 0);
	expectedEntry.iEndTime = TDateTime(2003, ENovember, 0, 11, 30, 00, 0);

	TestVCalTypeSequenceL(KEntry8(), 8,  4, expectedEntry, aIsSynch);	
	TestVCalTypeSequenceL(KEntry9(), 9,  1, expectedEntry, aIsSynch);	
	TestVCalTypeSequenceL(KEntry10(),10, 1, expectedEntry, aIsSynch);
	}


void CDataExchangeTestManager::TestImportTodoWithTzL(TBool aIsSynch)
	{
	//Importing Fixed Type ToDo vCal

	test.Next(_L("Importing Fixed Type ToDo vCal"));

	RPointerArray<CCalEntry> importedArray;
	CleanupResetAndDestroyPushL(importedArray);

	CreateTestFileL(KEntry6, KTestTodoWithTzImportFile);

	// date-times is converted using device local Tz
	ImportL(KTestTodoWithTzImportFile, importedArray, aIsSynch);

	test(importedArray.Count()==1);	

	test.Printf(_L("Check start and end times\n"));
	
	// start time is same as end time because according to the CCalEntry::SetStartAndEndTimeL() API documentation
	// The time portion of the start date will not be stored for todos, only the number of days until the due date.
	test(importedArray[0]->StartTimeL().TimeUtcL() == TTime(TDateTime(1999, ENovember, 0, 8, 45, 0, 0))) ;
	test(importedArray[0]->EndTimeL().TimeUtcL() == TTime(TDateTime(1999, ENovember, 0, 9, 30, 0, 0))) ;

	CleanupStack::PopAndDestroy(&importedArray);
	}

//! @SYMTestCaseID 
//! @SYMTestCaseDesc Tests when vCal imported with just X-EPOCALARM should not panic.
//! @SYMFssID 
//! @SYMTestStatus Implemented
//! @SYMTestPriority Medium
//! @SYMTestActions Imports 3 entries of which the first one contains  X-EPOCALARM without an AAL 
//! @SYMTestExpectedResults  The entry should be imported ignoring the X-EPOCALARM .
//! @SYMTestType 
void CDataExchangeTestManager::TestImportTodoWithXEpocAlarmL(TBool aIsSynch)
	{
	test.Next(_L("Importing ToDo vCal with X-Epoc-Alarm"));	
	RPointerArray<CCalEntry> importedArray;
	CleanupResetAndDestroyPushL(importedArray);

	CreateTestFileL(KEntry6a, KTestTodoWithXEPOCALARMImportFile);

	ImportL(KTestTodoWithXEPOCALARMImportFile, importedArray, aIsSynch);
	test(importedArray.Count()==3);	


	test.Printf(_L("Check start and end times"));
	// Checking whether the entries are imported Properly
	test(importedArray[0]->EntryTypeL()==CCalEntry::ETodo);
	test(importedArray[0]->StartTimeL().TimeUtcL() == TTime(TDateTime(1999, ENovember, 0, 8, 45, 0, 0))) ;
	test(importedArray[0]->EndTimeL().TimeUtcL() == TTime(TDateTime(1999, ENovember, 0, 9, 30, 0, 0))) ;
	
	test(importedArray[1]->EntryTypeL()==CCalEntry::ETodo);
	test(importedArray[1]->StartTimeL().TimeUtcL() == TTime(TDateTime(1999, EOctober, 0, 9, 10, 0, 0))) ;
	test(importedArray[1]->EndTimeL().TimeUtcL() == TTime(TDateTime(1999, EOctober, 0, 9, 32, 0, 0))) ;
	
	test(importedArray[2]->EntryTypeL()==CCalEntry::ETodo);
	test(importedArray[2]->StartTimeL().TimeUtcL() == TTime(TDateTime(2006, ESeptember, 9, 23, 0, 0, 0))) ;
	test(importedArray[2]->EndTimeL().TimeUtcL() ==   TTime(TDateTime(2006, ESeptember, 9, 23, 0, 0, 0))) ;
	CCalAlarm *alarm = NULL;
	alarm = importedArray[2]->AlarmL();
	test(alarm !=   NULL) ;
	if(alarm)
	delete alarm;
	
	CleanupStack::PopAndDestroy(&importedArray);
	}
//! @SYMTestCaseID 
//! @SYMTestCaseDesc Tests when vCal imported with out a RRule in the parent should not panic.
//! @SYMFssID 
//! @SYMTestStatus Implemented
//! @SYMTestPriority High
//! @SYMTestActions Imports  entries and store it again import (now it won't have a RRule in parent ) then store it .
//! @SYMTestExpectedResults  The second store should not panic.
//! @SYMTestType 
void CDataExchangeTestManager::TestImportParentWithoutRRuleL(TBool aIsSynch)
	{
	test.Next(_L("Importing Parent without a RRULE "));
	test.Printf(_L("Check start and end times"));

	iTestLib->CleanDatabaseL();		
	CCalEntryView& view=	iTestLib->SynCGetEntryViewL();
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	TInt numEntries=0;
	
	//import the entries from the  vcs file
	CreateTestFileL(KEntry6b, KTestTodoWithRuleImportFile);
	ImportL(KTestTodoWithRuleImportFile, entryArray, aIsSynch);
	test(entryArray.Count()==2);	
	
	//store the entries 
	view.StoreL(entryArray,numEntries);
	
	//get the uid of the parent	
	HBufC8* calUid;
	calUid=entryArray[0]->UidL().AllocL();
	CleanupStack::PushL(calUid);
	entryArray.ResetAndDestroy();
	
	view.FetchL(*calUid, entryArray) ;
	//get the entries from the calender
	ExportL(KTestTodoWithRuleImportExportedFile,entryArray, aIsSynch);
	entryArray.ResetAndDestroy();
	
	//import the entries from calender
	ImportL(KTestTodoWithRuleImportExportedFile, entryArray, aIsSynch);	
	//store the imported entries 
	view.StoreL(entryArray,numEntries);
	entryArray.ResetAndDestroy();
	
	// fetch all the entries and check the count.
	RPointerArray< CCalInstance > instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	FetchAllL(instanceList);
	
	test(instanceList.Count()==5);
	
	CleanupStack::PopAndDestroy(&instanceList);	
	CleanupStack::PopAndDestroy(calUid);		
	CleanupStack::PopAndDestroy(&entryArray);	
	}


void CDataExchangeTestManager::TestImportTodoWithWrongTzL(TBool aIsSynch)
	{
	//Importing Fixed Type ToDo vCal

	test.Next(_L("Importing Fixed Type ToDo vCal"));

	RPointerArray<CCalEntry> importedArray;
	CleanupResetAndDestroyPushL(importedArray);

	CreateTestFileL(KEntry7, KTestTodoWithWrongTzImportFile);

	// date-times is converted using device local Tz
	ImportL(KTestTodoWithWrongTzImportFile, importedArray, aIsSynch);

	test(importedArray.Count()==1);	

	test.Printf(_L("Check start and end times\n"));
	
	// start time is same as end time because according to the CCalEntry::SetStartAndEndTimeL() API documentation
	// The time portion of the start date will not be stored for todos, only the number of days until the due date.
	test(importedArray[0]->StartTimeL().TimeLocalL() == TTime(TDateTime(1999, ENovember, 0, 10, 00, 0, 0))) ;
	test(importedArray[0]->EndTimeL().TimeLocalL() == TTime(TDateTime(1999, ENovember, 0, 11, 30, 0, 0))) ;

	CleanupStack::PopAndDestroy(&importedArray);
	}

 //
// from former tcal_entry
	
/**
 * Create a vCalendar test file.
 * @param aVcalData vCalendar data to use for the file
 * @param aFileName Name of file to create
 */
 
void CDataExchangeTestManager::CreateVcalFileL(const TDesC8& aVCalData, const TDesC& aFileName)
	{
	RFile file;
	
	CleanupClosePushL(file);
	
	User::LeaveIfError(file.Replace(iTestLib->FileSession(), aFileName, EFileWrite+EFileShareAny+EFileStreamText));
	User::LeaveIfError(file.Write(aVCalData));
	
	CleanupStack::PopAndDestroy(&file); //file.Close()
	}


void CDataExchangeTestManager::DeleteVcalFileL(const TDesC& aFileName)
	{
	iTestLib->DeleteFileL(aFileName, EFalse);
	}
	

void CDataExchangeTestManager::DoImportL(const TDesC& aVCalendarFile)
	{
	test.Next(_L("Importing All entries\n"));
	
	RFile infile;
	
	User::LeaveIfError(infile.Open(iTestLib->FileSession(),aVCalendarFile,EFileRead));	

	CleanupClosePushL(infile);
	RFileReadStream readStream(infile);
	
	CleanupClosePushL(readStream);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CCalDataExchange* sync = CCalDataExchange::NewL(iTestLib->GetSession());
	CleanupStack::PushL(sync);
	
	TUid uidVCalendar(KUidVCalendar);
	sync->ImportL(uidVCalendar, readStream, entryArray, KCalDataExchangeImportStatusAsVCalendar);
	
	CleanupStack::PopAndDestroy(sync);
	test(entryArray.Count() == KEntry5Number);

	test.Printf(_L("Adding entries\n"));	
	TInt entriesCompleted = 0;	
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, entriesCompleted);
	test(entriesCompleted == KEntry5Number);	
	
	CleanupStack::PopAndDestroy(3); //infile, readStream, entryArray
	}

	
void CDataExchangeTestManager::DoImportLastDayL(const TDesC& aVCalendarFile)
	{
	test.Next(_L("Importing All Last Day entries\n"));
	
	RFile infile;
	
	User::LeaveIfError(infile.Open(iTestLib->FileSession(),aVCalendarFile,EFileRead));
	
	CleanupClosePushL(infile);
	RFileReadStream readStream(infile);
	
	CleanupClosePushL(readStream);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CCalDataExchange* sync = CCalDataExchange::NewL(iTestLib->GetSession());
	CleanupStack::PushL(sync);
	
	TUid uidVCalendar(KUidVCalendar);
	sync->ImportL(uidVCalendar, readStream, entryArray);
	
	CleanupStack::PopAndDestroy(sync);
	
	test.Printf(_L("Adding last day entries\n"));
	TInt entriesCompleted = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray,entriesCompleted);
	test(entriesCompleted == KLastDayNumber);
	
	TestEntriesL();

	CleanupStack::PopAndDestroy(3); //infile, readStream, entryArray
	}


void CDataExchangeTestManager::TestEntriesL()
	{
	TTime dtStart(TDateTime(2005, EJanuary, 0, 9, 0, 0, 0));
	TTime dtEnd(TDateTime(2008, EDecember, 30, 9, 0, 0, 0));
	TCalTime startCalTime;
	TCalTime endCalTime;
	
	startCalTime.SetTimeLocalL(dtStart);
	endCalTime.SetTimeLocalL(dtEnd);
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);

	CalCommon::TCalViewFilter filter(CalCommon::EIncludeAll);
	CalCommon::TCalTimeRange  timeRange(startCalTime, endCalTime);

	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, filter, timeRange);

	for (TInt i = 0; i < instances.Count(); i++)
		{
		TDateTime dt = instances[i]->Time().TimeLocalL().DateTime();
		
		// be sure to check only the recurring meetings not other entries
		
		const TDesC8& uid = instances[i]->Entry().UidL();
		
		if( uid == KLastDayUID1 || uid == KLastDayUID2 )
			{
			
			// Displaying last day entries

			test.Printf(_L("Found instance on %d: %d/%d/%d\n"), i, 
						dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour());

			// check that recurring meeting is set in the last day of the month

			switch( dt.Month() )
				{			
				case EJanuary:
					test(dt.Day() == 30);
					break;

				case EFebruary:
					// leap year?
					if( 0 != (dt.Year() % 4) )
						{
						test(dt.Day() == 27);
						}
					else
						{
						// leap year					
						test(dt.Day() == 28);
						}
					break;

				case EMarch:
					test(dt.Day() == 30);
					break;

				case EApril:
					test(dt.Day() == 29);
					break;

				case EMay:
					test(dt.Day() == 30);
					break;

				case EJune:
					test(dt.Day() == 29);
					break;

				case EJuly:
					test(dt.Day() == 30);
					break;

				case EAugust:
					test(dt.Day() == 30);
					break;

				case ESeptember:
					test(dt.Day() == 29);
					break;

				case EOctober:
					test(dt.Day() == 30);
					break;

				case ENovember:
					test(dt.Day() == 29);
					break;

				case EDecember:
					test(dt.Day() == 30);
					break;

				default:
					// ops... "alien" month?
					test(0);
					break;			
				}
			}
		}   	
   	
  	CleanupStack::PopAndDestroy(); //instances.ResetAndDestroy();
	}
	
	
// Test code for DEF069308: Test if a repeating child entry is properly stored in the datebase and updated (using StoreL)

void CDataExchangeTestManager::TestChildEntryL()
{
	// Create weekly repeating entry every Monday at 2pm.
	TTime startTime(TDateTime(2005, EJanuary, 0, 14, 0, 0, 0)); // 2pm
	TTime endTime(startTime + TTimeIntervalHours(1)); // 3pm
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	HBufC8* guid = KGUID().AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	TCalTime startTimeCal;
	startTimeCal.SetTimeLocalL(startTime);
	TCalTime endTimeCal;
	endTimeCal.SetTimeLocalL(endTime);
	entry->SetStartAndEndTimeL(startTimeCal, endTimeCal);
	
	TCalRRule weekly(TCalRRule::EWeekly);
	weekly.SetDtStart(startTimeCal);
	weekly.SetCount(60);


	RArray<TDay> days;

	CleanupClosePushL(days);
	days.AppendL(EMonday);
	weekly.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
		
	entry->SetRRuleL(weekly);
	entry->SetLocationL(_L("location 1"));

	TInt success(0);
	entries.AppendL(entry);
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	entries.Reset();

	CleanupStack::PopAndDestroy(entry);

	
	// create repeating child entry
	TTime recId(TDateTime(2005, EMarch, 13, 14, 0, 0, 0));
	TCalTime recIdCal;
	recIdCal.SetTimeLocalL(recId);

	guid = KGUID().AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 1, recIdCal, CalCommon::EThisAndFuture);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	entry->SetLocationL(_L("location 2"));
	entry->SetStartAndEndTimeL(recIdCal, recIdCal);
	weekly.SetDtStart(recIdCal);
	entry->SetRRuleL(weekly);
	
	entries.AppendL(entry);
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	entries.Reset();
	
	CleanupStack::PopAndDestroy(entry);


	// modify repeating child entry
	guid = KGUID().AllocLC();
	entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 2, recIdCal, CalCommon::EThisAndFuture);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);

	entry->SetLocationL(_L("location 3"));
	entry->SetStartAndEndTimeL(recIdCal, recIdCal);
	entry->SetRRuleL(weekly);
	
	entries.AppendL(entry);
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());	
	entries.Reset();
	
	CleanupStack::PopAndDestroy(2,&entries);	
}

//
//

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * from MCalDataExchangeCallBack
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CDataExchangeTestManager::Progress(TInt aPercentageCompleted)
	{
	test.Printf(_L("Operation is now %d per cent complete\n"), aPercentageCompleted);

	if (aPercentageCompleted < 15)
		{
		test.Printf(_L("Attempting an import during an asynchronous import/export operation\n"));
		TRAPD(err, TestAsyncImportL(KEntryExportFileGSEntries));
		test(err == KErrInUse);
		test.Printf(_L("Import failed correctly\n"));
		}
	else if (aPercentageCompleted < 25)
		{
		test.Printf(_L("Attempting an export during an asynchronous import/export operation\n"));
		TRAPD(err, TestAsyncExportL(KEntryExportFileGSEntries));
		test(err == KErrInUse);
		test.Printf(_L("Export failed correctly\n"));
		}
	}

	
void CDataExchangeTestManager::Completed()
	{
	test.Printf(_L("Operation completed\n"));
	CActiveScheduler::Stop();
	}

	
TInt CDataExchangeTestManager::NumberOfEntriesToHandleAtOnce()
	{
	return KNumberOfEntriesToHandleAtOnce;
	}

void CDataExchangeTestManager::StoreEntryL(CCalEntry* aEntry)
    {
    TInt success = 0;
    RPointerArray<CCalEntry> entries;
    entries.AppendL(aEntry);
    iTestLib->SynCGetEntryViewL().StoreL(entries, success);
    ASSERT(success == entries.Count());
    entries.Reset();
    }
   
void CDataExchangeTestManager::FetchAllL( RPointerArray<CCalInstance>& aInstances )
    {
    CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
   
    TCalTime startTime;
    startTime.SetTimeUtcL(TCalTime::MinTime());
    TCalTime endTime;
    endTime.SetTimeUtcL(TCalTime::MaxTime());
    CalCommon::TCalTimeRange timeRange(startTime, endTime);
   
    iTestLib->SynCGetInstanceViewL().FindInstanceL(aInstances, filter, timeRange);
    }
   
void CDataExchangeTestManager::OutputAllL( RPointerArray<CCalInstance>& aInstances )
    {
    for (TInt i = 0; i < aInstances.Count(); i++)
        {
        TDateTime dt = aInstances[i]->Time().TimeLocalL().DateTime();
        test.Printf(_L("instance at %d:%d on %d/%d/%d"), dt.Hour(), dt.Minute(),
            dt.Day()+1, dt.Month()+1, dt.Year());
        }
    }
   
void CDataExchangeTestManager::TestOrphanedEntryL(TBool aIsSynch)
    {
    iTestLib->CleanDatabaseL();
   
    _LIT8(KTestGuid, "testCorruptionBugL");
    // Create parent
    HBufC8* guid = KTestGuid().AllocLC();
    CCalEntry* appt = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
    CleanupStack::Pop(guid);
    CleanupStack::PushL(appt);
   
    TCalTime start;
    start.SetTimeUtcL( TTime( TDateTime( 2006, EFebruary, 13 - 1, 18, 0, 0, 0) ) );
    TCalTime end;
    end.SetTimeUtcL( TTime( TDateTime( 2006, EFebruary, 13 - 1, 18, 30, 0, 0) ) );
    appt->SetStartAndEndTimeL( start, start );
    appt->SetSummaryL( _L("Corruption bug") );
    appt->SetLocationL( _L("PARENT") );
   
    TCalRRule rrule( TCalRRule::EDaily );
    rrule.SetDtStart( start );
    rrule.SetInterval( 1 );
    const TInt KInstanceCount = 8;
    TCalTime until;
    until.SetTimeLocalL( start.TimeLocalL() + TTimeIntervalDays( KInstanceCount - 1 ) );
    rrule.SetUntil( until );
   
    appt->SetRRuleL( rrule );

    StoreEntryL(appt);

    CleanupStack::PopAndDestroy( appt );
    // Check that all instances are found
    RPointerArray< CCalInstance > instances;
    CleanupResetAndDestroyPushL( instances );

    FetchAllL( instances );
   
    ASSERT( instances.Count() == KInstanceCount );
    ASSERT( instances[0] != NULL );
    ASSERT( instances[0]->Entry().UidL() == KTestGuid );
    ASSERT( instances[0]->StartTimeL().TimeUtcL() == start.TimeUtcL() );
    CleanupStack::PopAndDestroy( &instances );
    // Fetch parent from database
   
    RPointerArray<CCalEntry> entryArray;
    CleanupClosePushL(entryArray);
    iTestLib->SynCGetEntryViewL().FetchL(KTestGuid, entryArray);
   
    CCalEntry* parent = entryArray[0];
    CleanupStack::PushL( parent );
    ASSERT( parent );
    // Create child for 2nd day
    TCalTime childStart;
    childStart.SetTimeLocalL( start.TimeLocalL() + TTimeIntervalDays(1) );
    TCalTime childEnd;
    childEnd.SetTimeLocalL( childStart.TimeLocalL() ); //+ CalenAgendaUtils::DurationL( *parent ) );
   
   
    TCalTime recurrenceID( childStart );
   
    HBufC8* childGuid = parent->UidL().AllocLC();
    // create (child) entry.
    // This simulates what is happening in editors
    CCalEntry* child = CCalEntry::NewL( parent->EntryTypeL(),
                                        childGuid,
                                        parent->MethodL(),
                                        0, // sequence number
                                        recurrenceID,
                                        CalCommon::EThisOnly);
    CleanupStack::Pop( childGuid ); // ownership transferred       
    CleanupStack::PushL( child );
   
    // copy all the data from parent
    child->CopyFromL( *parent );
    // modify time to be instance time
    child->SetStartAndEndTimeL( childStart, childEnd );
    // modify location
    child->SetLocationL( _L("CHILD") );
    // reset local UID
    child->SetLocalUidL( TCalLocalUid( 0 ) );
    // clear repeat rule properties
    child->ClearRepeatingPropertiesL();
    child->SetLastModifiedDateL();
    // Store child
    StoreEntryL( child );
   
    CleanupStack::PopAndDestroy( child );
    CleanupStack::PopAndDestroy( parent );
    CleanupStack::PopAndDestroy(&entryArray); // entryArray.Close()
    // Test that child was correcly stored
    RPointerArray< CCalEntry > entries;
    CleanupResetAndDestroyPushL( entries );
   
    iTestLib->SynCGetEntryViewL().FetchL( KTestGuid, entries );
    ASSERT( entries.Count() == 2 );
   
    CleanupStack::PopAndDestroy( &entries );
   
    // Simulate synch step. Parent is received as vCalendar and StoreL is done for it:
    _LIT(KTestOrphanFile, "orphan.vcs");
    CreateTestFileL(KUpdatedParentVCalendar, KTestOrphanFile);

    RPointerArray<CCalEntry> importedArray;
    CleanupClosePushL(importedArray);
    ImportL(KTestOrphanFile, importedArray, aIsSynch);
     
    CCalEntry* updatedParent = importedArray[0];
    CleanupStack::PushL( updatedParent );
   
    ASSERT( updatedParent );
    StoreEntryL( updatedParent );
    CleanupStack::PopAndDestroy( updatedParent );
    CleanupStack::PopAndDestroy(&importedArray); // importedArray.Close()
 
    // Fetch all instances
    CleanupResetAndDestroyPushL( instances );   
    FetchAllL( instances );
    OutputAllL( instances );

    // Instance count is f*cked up:  TS_ASSERT_EQUALS( instances.Count(), KInstanceCount - 1 - 1 - 1 ); //own child + exception date, last instance bug 
    CleanupStack::PopAndDestroy( &instances );

    // Now delete all, simulate Calendar application
    TCalTime max;
    max.SetTimeUtcL( TCalTime::MaxTime() );
    TCalTime min;
    min.SetTimeUtcL( TCalTime::MinTime() );
    CalCommon::TCalTimeRange range( min, max );
    // synchronous wrapper for entry view's DeleteL using activeschedulerwait
    iTestLib->SynCGetEntryViewL().DeleteL( range, CalCommon::EIncludeAll, *iTestLib);
    CActiveScheduler::Start();
   
    // now fetch all instances, one is returned. If you try to read details of instance, crash happens => database is corrupted
    CleanupResetAndDestroyPushL( instances );   
    FetchAllL( instances );
    OutputAllL( instances );

    ASSERT( instances.Count() == 0 ); // one instance is found
    CleanupStack::PopAndDestroy( &instances ); 
    }

void CDataExchangeTestManager::TestUIDImportL(const TDesC& aFileToImport, const TDesC& aFileToExport, TBool aIsSynch) //Added for DEF079011 - CCalenImporter::ImportVCalL() method cuts long uid   
	{
	// Import vCal with long UTF-8 UID to an empty calendar file.
	// After storing the entry, export it and re-import.
	// Only one entry should be present in the calendar file.

	test.Next(_L("Test Importing long UTF Uid"));
	test.Printf(_L("Removing All Entries from the calendar file\n"));
	iTestLib->CleanDatabaseL();
	RPointerArray<CCalEntry> aEntriesImported;
	CleanupResetAndDestroyPushL(aEntriesImported);
	RFile infile;
	User::LeaveIfError(infile.Open(iTestLib->FileSession(), aFileToImport, EFileRead));
	CleanupClosePushL(infile);
	RFileReadStream readStream(infile);
	CleanupClosePushL(readStream);
	
	test.Printf(_L("Importing from ORIGINAL VCS file and storing it.\n"));
	iDataExchange->ImportL(KUidVCalendar, readStream, aEntriesImported);
	test(aEntriesImported.Count() == 1);
	CleanupStack::PopAndDestroy(2,&infile);	
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(aEntriesImported, success);
	test(aEntriesImported.Count() == success);
	CleanupStack::PopAndDestroy(&aEntriesImported);

	test.Printf(_L("Fetching the entry from Calendar file\n"));
	CCalIter* iter = CCalIter::NewL(iTestLib->GetSession());
	CleanupStack::PushL(iter);
	TPtrC8 Uid(iter->FirstL());
	RPointerArray<CCalEntry> entryList;
	CleanupResetAndDestroyPushL(entryList);
	iTestLib->SynCGetEntryViewL().FetchL(Uid, entryList);
	
	ExportL(aFileToExport, entryList, aIsSynch);
	CleanupStack::PopAndDestroy(&entryList); 
	CleanupStack::PopAndDestroy(iter);
	
	CleanupResetAndDestroyPushL(aEntriesImported);
	ImportL(aFileToExport,aEntriesImported, aIsSynch);
	test(aEntriesImported.Count() == 1);
	
	test.Printf(_L("Storing Imported entry\n"));
	success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(aEntriesImported, success);
	test(aEntriesImported.Count() == success);
	CleanupStack::PopAndDestroy(&aEntriesImported);
	
	test.Printf(_L("Verification : ONLY ONE ENTRY SHOULD BE THERE\n"));	
	iter = CCalIter::NewL(iTestLib->GetSession());
	CleanupStack::PushL(iter);
	test(iter->FirstL() != KNullDesC8);
	test(iter->NextL() == KNullDesC8);
	CleanupStack::PopAndDestroy(iter);	
	}

void CDataExchangeTestManager::ImportAndExportL(const TDesC8& aVCalData, const TDesC& aImportVCalendarFile, TInt aNumEntry, TBool aIsSynch)
	{
	//Import entry from des KAttachmentVcal
	test.Next(_L("Import And ExportL"));	
	CreateTestFileL(aVCalData, aImportVCalendarFile );

	RPointerArray<CCalEntry> importedentry;
	CleanupResetAndDestroyPushL(importedentry);

	ImportL(aImportVCalendarFile, importedentry, aIsSynch);
	test(importedentry.Count() == aNumEntry);

	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(importedentry, success);

	test(success == aNumEntry);

	CreateTestFileL(KNullDesC8, KEntryExportFile);
	ExportL(KEntryExportFile, importedentry, aIsSynch);
	
	RPointerArray<CCalEntry> reImportedEntry;
	CleanupResetAndDestroyPushL(reImportedEntry);
			
	ImportL(KEntryExportFile, reImportedEntry, aIsSynch);
	test(importedentry.Count() == reImportedEntry.Count());
	
	// remove each of the imported entries from entryArray to check they are all there
	test(CheckTwoArrayEntryL(importedentry, reImportedEntry));
	
	CleanupStack::PopAndDestroy(&reImportedEntry);
	CleanupStack::PopAndDestroy(&importedentry);
	}

void CDataExchangeTestManager::TestExceptionDatesConversionL(TBool aIsSynch)
    {
    iTestLib->CleanDatabaseL();
    
    test.Next(_L("Check exception date on vcard import..."));

    _LIT(KExceptionFileName, "exceptionvcard.vcs");

	CreateTestFileL(KExceptionEntry(), KExceptionFileName);

    RPointerArray<CCalEntry> entryArray;
    CleanupResetAndDestroyPushL(entryArray);
	
	ImportL(KExceptionFileName, entryArray, aIsSynch);
    test(entryArray.Count()==1);

	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	test(entryArray.Count() == success);
	CleanupStack::PopAndDestroy(&entryArray);

	test.Printf(_L("Fetching the entry from Calendar file\n"));

	CCalIter* iter = CCalIter::NewL(iTestLib->GetSession());
	CleanupStack::PushL(iter);
	TPtrC8 Uid(iter->FirstL());
	RPointerArray<CCalEntry> entryList;
	CleanupResetAndDestroyPushL(entryList);
	iTestLib->SynCGetEntryViewL().FetchL(Uid, entryList);

    RArray<TCalTime> aExDateList;
    CleanupClosePushL(aExDateList);
    
    CCalEntry* entry = entryList[0];
    TDateTime startDate = entry->StartTimeL().TimeUtcL().DateTime();
    test.Printf(_L("Start time = %d/%d/%d %d:%d:%d\n"),startDate.Day()+1, startDate.Month()+1,
    	startDate.Year(), startDate.Hour(), startDate.Minute(), startDate.Second());
	test(startDate.Hour() == 9);
   
    RArray<TCalTime> aRDateList;
    entry->GetRDatesL(aRDateList);
    startDate = aRDateList[0].TimeUtcL().DateTime();
    test.Printf(_L("RDate time = %d/%d/%d %d:%d:%d\n"),startDate.Day()+1, startDate.Month()+1,
    	startDate.Year(), startDate.Hour(), startDate.Minute(), startDate.Second());
	test(startDate.Hour() == 11);
    aRDateList.Close();
	
    entry->GetExceptionDatesL(aExDateList);
    
    //Only two exceptions are imported as there is one invalid exception
    test(aExDateList.Count()==2);
    
    TDateTime exDate = aExDateList[0].TimeUtcL().DateTime();
     
    test.Printf(_L("Exception time = %d/%d/%d %d:%d:%d\n"),exDate.Day()+1, exDate.Month()+1,
    	exDate.Year(), exDate.Hour(), exDate.Minute(), exDate.Second());
	test(exDate.Hour() == 9);
	
    exDate = aExDateList[1].TimeUtcL().DateTime();
    
    test.Printf(_L("Exception time = %d/%d/%d %d:%d:%d\n"),exDate.Day()+1, exDate.Month()+1,
    	exDate.Year(), exDate.Hour(), exDate.Minute(), exDate.Second());
	test(exDate.Hour() == 9);
   
    CleanupStack::PopAndDestroy(3, iter);//aExDateList, entryList
    }

void CDataExchangeTestManager::TestRRuleEndDateL(TBool aIsSynch)
	{
	test.Next(_L("Test RRule's enddate:"));
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);	

	const TInt KTzOffset = -05;		// try various tz for vCal, and various local time zones

	_LIT(KTextTz, "TZ");
	_LIT(KTextNoTz, "no TZ");
	TBuf8<512> litEntry;
	TBuf8<32>  tzstr;
	TBuf<64>   timestr;
	TBuf<256>  line;

	tzstr.Format(_L8("TZ:%c%02d\r\n"), KTzOffset>=0 ? '+':'-', Abs(KTzOffset));		// (TZ: +01)

	const TTime KDtStart=TDateTime(1999,EJanuary,  0,12,0,0,0); // DTSTART: 19990101T120000(Z)	= 12:00  Jan 1, 1999
	const TTime KDtEnd 	=TDateTime(1999,ENovember, 0,13,0,0,0); // DTEND:	19991101T130000(Z)	= 13:00  Nov 1, 1999
	const TTime KUntil 	=TDateTime(2000,ENovember, 0, 10,0,0,0); // RRULE:D1 20001101T000000(Z)	=  0:00  Nov 1, 2000
	// This test case tests various combinations of time modes for DTSTART, DTEND and RRULE's enddate (until)
	// and presence or absence of TZ property in a vCal.
	// If any of these datetime values has 'Z' designator at the end, it is treated as UTC time, as per ISO8601.
	// Time mode of DTSTART determines time mode of calendar entry and affects how DTEND and until are interpreted.
	//
	TBool dtstartIsUtc=EFalse;
	for (TBool tzPresent=EFalse; tzPresent<=ETrue; tzPresent++)
//	 for (TBool dtstartIsUtc=EFalse; dtstartIsUtc<=ETrue; dtstartIsUtc++)
	  for (TBool dtendIsUtc=EFalse; dtendIsUtc<=ETrue; dtendIsUtc++)
	   for (TBool untilIsUtc=EFalse; untilIsUtc<=ETrue; untilIsUtc++)
		{
		test.Printf(_L(" "));
		test.Printf(_L("Testing variant=(%S, DTSTART:%c DTEND:%c UNTIL:%c) : "), 
				tzPresent?&KTextTz:&KTextNoTz, dtstartIsUtc?'Z':'f', dtendIsUtc?'Z':'f', untilIsUtc?'Z':'f');

		litEntry.Format(KRRuleEndDateTest, tzPresent?&tzstr:&KNullDesC8, dtstartIsUtc?'Z':' ', dtendIsUtc?'Z':' ', untilIsUtc?'Z':' ');
		CreateTestFileL(litEntry, KTestRRuleEndDateFile);
		ImportL(KTestRRuleEndDateFile, entryArray, aIsSynch);
		ListEntriesL(entryArray);
		test(entryArray.Count() == 1);
		CCalEntry* entry = entryArray[0];

		TCalRRule rrule;
		test(entry->GetRRuleL(rrule)); 	// MUST have rrule

		TCalTime entryStart = entry->StartTimeL(); 
		TCalTime entryEnd   = entry->EndTimeL();
		TCalTime rruleStart = rrule.DtStart();
		TCalTime rruleUntil = rrule.Until();

		TCalTimeUtil::FormatL(timestr, rrule.Until());
		test.Printf(_L("UNTIL=%S"), &timestr);

		// Validate expected behaviour:
		TBool floatingMode = !tzPresent && !dtstartIsUtc;

		// check ENTRY's START TIME
		// entry's time mode is floating only if no tz and start time is non-UTC
		TCalTime::TTimeMode expEntryMode = floatingMode ? TCalTime::EFloating : TCalTime::EFixedUtc;
		test(entryStart.TimeMode() == expEntryMode);
		// expected start time
		TTime expStart = KDtStart;
		if (tzPresent && !dtstartIsUtc)
			{
			// if TZ information available and DTSTART is non-UTC, it is converted and stored as UTC
			expStart -= TTimeIntervalHours(KTzOffset);	// expect converted value
			}
			
		if (floatingMode)
			{
			test(entryStart.TimeLocalL() == expStart);
			}
		else
			{
			test(entryStart.TimeUtcL() == expStart);
			}

		// check ENTRY's END TIME
		// time mode of entry's end time always matches start time mode of the entry
		// even if DTEND mode is different from time mode of DTSTART, the latter takes precedence
		test(entryEnd.TimeMode() == expEntryMode);
		// expected end time
		TTime expEnd = KDtEnd;
		if (tzPresent && !dtendIsUtc ) 
			{
			// if TZ information available and DTEND is non-UTC, it is converted and stored as UTC
			expEnd -= TTimeIntervalHours(KTzOffset);	// expect converted value
			}
			
		if (floatingMode)
			{
			test(entryEnd.TimeLocalL() == expEnd);
			}
		else
			{
			test(entryEnd.TimeUtcL() == expEnd);
			}

		// check REPEAT START TIME
		test(rruleStart.TimeMode() == expEntryMode);
		// rrule's start time always matches entry's start time:
		if (floatingMode)
			{
			test(rruleStart.TimeLocalL() == expStart);
			}
		else
			{
			test(rruleStart.TimeUtcL() == expStart);
			}

		// check REPEAT END TIME (UNTIL time)
		test(rruleUntil.TimeMode() == expEntryMode);	

		TDateTime dtDate = KUntil.DateTime();
		TTime startVCalLocal = KDtStart;
		if (tzPresent && dtstartIsUtc) 
			{
			startVCalLocal += TTimeIntervalHours(KTzOffset); // convert back to local time
			}
		TDateTime dtTime = startVCalLocal.DateTime();
		TTime expUntil = TDateTime(dtDate.Year(), dtDate.Month(), dtDate.Day(),dtTime.Hour(), dtTime.Minute(), dtTime.Second(), 0);
		
		TTime rruleUntilVCalLocal;
		if (floatingMode)
			{
			rruleUntilVCalLocal = rruleUntil.TimeLocalL();
			}
		else if (tzPresent)
			{
			rruleUntilVCalLocal = rruleUntil.TimeUtcL() + TTimeIntervalHours(KTzOffset);
			}
		else
			{
			rruleUntilVCalLocal = rruleUntil.TimeLocalL(); // UTC mode, no TZ present
			}

		test(rruleUntilVCalLocal == expUntil);


		// Export, re-import and validate again
		ExportL(KTestRRuleEndDateFile, entryArray, aIsSynch);
		entryArray.ResetAndDestroy();

		ImportL(KTestRRuleEndDateFile, entryArray, aIsSynch);
		ListEntriesL(entryArray);
		test(entryArray.Count()==1);

		entry = entryArray[0];
		TCalRRule rrule2;	
		test(entry->GetRRuleL(rrule2)); 	// MUST have rrule
		entryStart = entry->StartTimeL(); 
		entryEnd   = entry->EndTimeL();
		rruleStart = rrule2.DtStart();
		rruleUntil = rrule2.Until();

		// after entries have been exported / re-imported time values are not expected to change, 
		// although UTC time designators ('Z') get mixed up for DTEND and RRule's enddate.
		test(entryStart.TimeMode() == expEntryMode);
		test(entryEnd.TimeMode()   == expEntryMode);	// might have been changed forcibly
		test(rruleStart.TimeMode() == expEntryMode);
		test(rruleUntil.TimeMode() == expEntryMode);	// originally could have been floating

		// check DTSTART, DTEND, and rrule's start time
		if (floatingMode)
			{
			test(entryStart.TimeLocalL() == expStart);
			test(entryEnd.TimeLocalL()   == expEnd);
			test(rruleStart.TimeLocalL() == expStart);
			}
		else
			{
			test(entryStart.TimeUtcL() == expStart);
			test(entryEnd.TimeUtcL()   == expEnd);
 			test(rruleStart.TimeUtcL() == expStart);
 			}
 			
 		//check until time	
		if (floatingMode)
			{
			rruleUntilVCalLocal = rruleUntil.TimeLocalL();
			}
		else if (tzPresent)
			{
			rruleUntilVCalLocal = rruleUntil.TimeUtcL()+TTimeIntervalHours(KTzOffset);
			}
		else
			{
			rruleUntilVCalLocal = rruleUntil.TimeLocalL(); // UTC mode, no TZ present
			}

		test(rruleUntilVCalLocal == expUntil);	
		
		entryArray.ResetAndDestroy();
		}	

	CleanupStack::PopAndDestroy(&entryArray);

	test.Printf(_L("RRULE end date test completed successfully."));
	test.Printf(_L(" "));	
	}

void CDataExchangeTestManager::PrepareFileForContentIdAttachmentL()
	{
	_LIT(KOriginalDrive, "z:");
	
	TInt err = iTestLib->FileSession().MkDirAll(KAttachmentCidFile);
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	
	// required for passing attachment file handles to server
	User::LeaveIfError(iTestLib->FileSession().ShareProtected()); 

	CFileMan* fileCopier = CFileMan::NewL(iTestLib->FileSession());
	CleanupStack::PushL(fileCopier);

	TFileName initialLocationOfFile = KAttachmentCidFile();
	initialLocationOfFile.Replace(0, 2, KOriginalDrive());

	// get current time
	TTime now;
	now.HomeTime();
	
	// clear any read only attribute if the file is present such we avoid errors at copy time
	fileCopier->Attribs(KAttachmentCidFile, 0, KEntryAttReadOnly, now);

	// copy the file from Z: drive to C: drive (ROM to RAM)
	User::LeaveIfError(fileCopier->Copy(initialLocationOfFile, KAttachmentCidFile));
	
	// clear any read only attribute such we avoid errors at open time (true on real hardware)
	User::LeaveIfError(fileCopier->Attribs(KAttachmentCidFile, 0, KEntryAttReadOnly, now));
	CleanupStack::PopAndDestroy(fileCopier);
	}

void CDataExchangeTestManager::TestAttachmentImportExportL(TBool aIsSynch, TBool aNegtiveTest)
	{
	//Import entry from des KAttachmentVcal
	test.Next(_L("test data exchange: Attachment Import and Export"));	

	CreateTestFileL(KAttachmentVcal, KAttachmentFile);
	RPointerArray<CCalEntry> importedentry;
	CleanupResetAndDestroyPushL(importedentry);
	ImportL(KAttachmentFile, importedentry, aIsSynch);

	//One of attachment is content id type, so we need to associate the CID with the file which contained attachments
	PrepareFileForContentIdAttachmentL();
	
	CCalEntry& impEntry0 = *importedentry[0];
	const TInt KCount 	 = impEntry0.AttachmentCountL();
	TInt index			 = 0;
	RFile file;
	for ( ; index < KCount; ++index )
		{
		CCalAttachment* attach = impEntry0.AttachmentL(index);
		if(attach->FileAttachment() && attach->FileAttachment()->ContentId().Length() > 0)
			{
			User::LeaveIfError(file.Open(iTestLib->FileSession(), KAttachmentCidFile(), EFileWrite));
			CleanupClosePushL(file);
			
			attach->FileAttachment()->SetResourceL(file);
			CleanupStack::PopAndDestroy(&file);
			if(aNegtiveTest)
				{
				_LIT(KDrive,"q");
				attach->FileAttachment()->SetDriveL(KDrive);	
				}
			
			attach->SetAttribute(CCalAttachment::EExportInline);
			
			break;
			}
		}
	
	HBufC8* uid = impEntry0.UidL().AllocLC();

	TInt success = 0;
	RPointerArray<CCalEntry> fetchedentry;
	CleanupResetAndDestroyPushL(fetchedentry);
	TRAPD(err, iTestLib->SynCGetEntryViewL().StoreL(importedentry, success));
	iTestLib->SynCGetEntryViewL().FetchL(*uid, fetchedentry);
	if(aNegtiveTest)
		{
		test (err == KErrNotReady);
		test(fetchedentry.Count() == 0);	
		}
	else
		{
		test (err == KErrNone);
		test(importedentry.Count() == success);
		test(fetchedentry.Count() == 1);
		test(fetchedentry[0]->AttachmentCountL() == 3);	
		}


	//export, reimport and compare the fetchedentry and reported entry - they should be identical
	ExportImportAndCheckL(fetchedentry, aIsSynch);
	
	CleanupStack::PopAndDestroy(&fetchedentry);	
	CleanupStack::PopAndDestroy(uid);
		
	CleanupStack::PopAndDestroy(&importedentry);		
	}
	
void CDataExchangeTestManager::TestAttachmentExportImportL(TBool aIsSynch)
	{
	test.Next(_L("test data exchange: Attachment Add and Eport"));	

	//Create entry array
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	//Create an entry and add append it to the array	
	HBufC8* uid=NULL;
	CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, uid);
	CleanupStack::PushL(entry);
	iTestLib->SetEntryStartAndEndTimeL(entry, Time::NullTTime(), Time::NullTTime());
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	
	// Create an attachment and add it to the entry
	_LIT8(KBinaryData1, "fvgbuygbrvgbrfguyrt4fg67463t23875rt87ty8754yt89y96y0u6908yu896uyrtuyirfuygbcfyugyuvertgvrwsrv67er847rt87ei7rektyiufyhosiuydgdryiu");
	_LIT8(KMimeType1, "dummy/gibberish");
	_LIT(KName1, "binary data attachment name");
	
	HBufC8* data = KBinaryData1().AllocLC();
	CCalAttachment* attachment = CCalAttachment::NewFileL(data);
	CleanupStack::Pop(data);
	CleanupStack::PushL(attachment);
	
	attachment->SetMimeTypeL(KMimeType1());
	attachment->SetLabelL(KName1());

	entry->AddAttachmentL(*attachment);
	CleanupStack::Pop(attachment);

	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	test(success == entryArray.Count());
	
	TCalLocalUid localUid = entryArray[0]->LocalUidL();
	
	entryArray.Reset(); 
	delete entry;
	
	//fetch the entry and add it the array
	entry = iTestLib->SynCGetEntryViewL().FetchL(localUid);
	CleanupStack::PushL(entry);
	test(entry->AttachmentCountL() == 1);
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	
//	export and check
	ExportImportAndCheckL(entryArray, aIsSynch);
	CleanupStack::PopAndDestroy(&entryArray);	
	}


void CDataExchangeTestManager::TestToDoStartDateL(TBool aIsSynch)
	{
 	_LIT(KEntryToDoExportFile,"tcal_todo_export.vcs");
 	
 	RPointerArray<CCalEntry> entryArray;
 	CleanupResetAndDestroyPushL(entryArray);	
 
 	test.Printf(_L("Exporting TODO entry with start and end date\n"));
 	
 	HBufC8* guid4 = _L8("test4").AllocLC();
 	CCalEntry* entry7 = CCalEntry::NewL(CCalEntry::ETodo, guid4, CCalEntry::EMethodNone, 0);
 	CleanupStack::Pop(guid4);
 	CleanupStack::PushL(entry7);
 	
	/* setting start and end time */
 
 	TTime startTime4(TDateTime(2005, ENovember, 29, 10, 0, 0, 0)); // 2005-11-30
 	TCalTime calStartTime;
 	calStartTime.SetTimeLocalL(startTime4);
 	TTime endTime4(TDateTime(2005, EDecember, 0, 10, 0, 0, 0)); // 2005-12-01
 	TCalTime calEndTime;
 	calEndTime.SetTimeLocalL(endTime4);
 	entry7->SetStartAndEndTimeL(calStartTime, calEndTime);
 
 	entryArray.AppendL(entry7);
 	CleanupStack::Pop(entry7);
 	
 	/* exporting then importing the entry */
 	ExportL(KEntryToDoExportFile, entryArray, aIsSynch);
 
 	RPointerArray<CCalEntry> importedArray;
 	CleanupResetAndDestroyPushL(importedArray);
 			
 	ImportL(KEntryToDoExportFile, importedArray, aIsSynch);
 	test(entryArray.Count() == importedArray.Count());	
 	
 	/* checking whether date has been set properly */
 	
 	test(importedArray[0]->CompareL(*entryArray[0])) ;
	test(importedArray[0]->StartTimeL().TimeLocalL() == TTime(TDateTime(2005, ENovember, 29, 10, 0, 0, 0))) ;
	test(importedArray[0]->EndTimeL().TimeLocalL() == TTime(TDateTime(2005, EDecember, 0, 10, 0, 0, 0))) ;	
 	
 	delete importedArray[0];
 	importedArray.Remove(0);
 	
 	test.Printf(_L("Imported TODO entry has start and end date\n"));
 			
 	CleanupStack::PopAndDestroy(&importedArray);
 
 	CleanupStack::PopAndDestroy(&entryArray);		
 	}

// PDEF101009
//
// Test that a ToDo with Start Date and Due Date the same is exported with
// both DTSTART and DUE fields
//
// based on TestToDoStartDateL() above 
void CDataExchangeTestManager::TestExportToDoStartDateEqualDueDateL(TBool aIsSynch) 
	{
 	_LIT(KEntryToDoExportFile,"tcal_todo_export.vcs");
 	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);	

	test.Printf(_L("Exporting TODO entry with start and end dates equal"));
	
	HBufC8* guid4 = _L8("test4").AllocLC();
	CCalEntry* entry7 = CCalEntry::NewL(CCalEntry::ETodo, guid4, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid4);
	CleanupStack::PushL(entry7);
	
	/* setting start and end time */
	TTime startTime4(TDateTime(2005, ENovember, 29, 10, 0, 0, 0)); // 2005-11-30
	TCalTime calTime;
	calTime.SetTimeLocalL(startTime4);
	
	entry7->SetStartAndEndTimeL(calTime, calTime);

	entryArray.AppendL(entry7);
	CleanupStack::Pop(entry7);
	
	/* exportthe entry */
	ExportL(KEntryToDoExportFile, entryArray, aIsSynch);	
	
	/* check whether fields are present */
	_LIT8(KTODODueField, "DUE:");
	_LIT8(KTODOStartDateField, "DTSTART:");
	
	/* find the exported file -- it should be in the temp directory */
    TFileName exportedFile;
    iTestLib->TestRegister().GetTempPath(exportedFile);
    exportedFile.Append(KEntryToDoExportFile);    

    test(CheckStringInFileL(KTODODueField, exportedFile, ETrue)); 
    test(CheckStringInFileL(KTODOStartDateField, exportedFile, ETrue));

	test.Printf(_L("Exported TODO entry has both DUE and DTSTART fields present"));
			
	CleanupStack::PopAndDestroy(&entryArray);		
	}
 
 void CDataExchangeTestManager::TestFloatingToDoStartDateL(TBool aIsSynch)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);	

	test.Printf(_L("Exporting TODO entry with start and end date"));
	
	HBufC8* guid4 = _L8("test4").AllocLC();
	CCalEntry* entry7 = CCalEntry::NewL(CCalEntry::ETodo, guid4, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid4);
	CleanupStack::PushL(entry7);
	
	/* setting start and end time */
	TTime startTime4(TDateTime(2005, EJuly, 29, 10, 0, 0, 0)); // 2005-07-30
	TCalTime calStartTime;
	calStartTime.SetTimeLocalFloatingL(startTime4);
	TTime endTime4(TDateTime(2005, EAugust, 0, 10, 0, 0, 0)); // 2005-08-01
	TCalTime calEndTime;
	calEndTime.SetTimeLocalFloatingL(endTime4);
	entry7->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	entryArray.AppendL(entry7);
	CleanupStack::Pop(entry7);
	
	//check the DTSTART for the exported todo
	_LIT8(KTokenToFind, "DTSTART:20050730T100000");
	_LIT(KExportFilePath, "c:\\logs\\tcal_dataexchange\\temp\\");
	TBuf<100> fileName(KExportFilePath);
	fileName.Append(KEntryExportFileGSEntries);
	
	ExportImportAndCheckL(entryArray, aIsSynch);
	test(CheckStringInFileL(KTokenToFind, fileName, ETrue));
	
	CleanupStack::PopAndDestroy(&entryArray);
	}
 
void CDataExchangeTestManager::TestUntilDateImportForVCalL(const TDesC8& aVcal, TInt aDay, TInt aHour, TCalTime::TTimeMode aTimeMode, TBool aIsSynch)
    {
    _LIT(KImportFile, "until_date_import.vcs");
   
    RPointerArray<CCalEntry> entries;
    CleanupResetAndDestroyPushL(entries);

    CreateTestFileL(aVcal, KImportFile);
    ImportL(KImportFile, entries, aIsSynch);
   
    test(entries.Count() == 1);
    TCalRRule rule;
    entries[0]->GetRRuleL(rule);
    TDateTime dt = rule.Until().TimeUtcL().DateTime();
    test(dt.Month() == EJune);
    test(dt.Day()+1 == aDay);//TDateTime::iDay start from 0
    test(dt.Hour() == aHour);
   	test(rule.Until().TimeMode()==aTimeMode);
    CleanupStack::PopAndDestroy(&entries);
    }

void CDataExchangeTestManager::TestUntilDateImportL(TBool aIsSynch)
    {
    TestUntilDateImportForVCalL(KUntilDateCheck1, 19, 17, TCalTime::EFixedUtc, aIsSynch); // end date is 19th June at 17:00 UTC (+TZ)
    TestUntilDateImportForVCalL(KUntilDateCheck2, 19, 17, TCalTime::EFixedUtc, aIsSynch);
    TestUntilDateImportForVCalL(KUntilDateCheck3, 18, 23, TCalTime::EFixedUtc, aIsSynch); // end date is 18th June at 23:00 UTC (-TZ)
    TestUntilDateImportForVCalL(KUntilDateCheck4, 19, 23, TCalTime::EFixedUtc, aIsSynch);
    TestUntilDateImportForVCalL(KUntilDateCheck5, 19, 18, TCalTime::EFixedUtc, aIsSynch); // end date is 19th June at 18:00 UTC (no TZ)
    TestUntilDateImportForVCalL(KUntilDateCheck6, 19, 18, TCalTime::EFixedUtc, aIsSynch); // end date is 19th June at 18:00 UTC (no TZ)
    TestUntilDateImportForVCalL(KUntilDateCheck7, 19, 22, TCalTime::EFloating, aIsSynch); // end date is 19th June at 23:30 local (floating)
    TestUntilDateImportForVCalL(KUntilDateCheck8, 19, 22, TCalTime::EFloating, aIsSynch); // end date is 19th June at 23:30 local (floating)
    }

void CDataExchangeTestManager::TestImportingAlarmedFloatingAniversaryL(TBool aIsSynch)
	{
	RPointerArray<CCalEntry> storedEntries;
	CleanupResetAndDestroyPushL(storedEntries);

	// Import the entry
	test.Printf(_L("Writing test entry data to vcs file\n"));
				
	CreateTestFileL(KAlarmedAnnivVCalendar, KEntryImportFile());
	ImportL(KEntryImportFile, storedEntries, aIsSynch);
	
	test(storedEntries.Count() == 1);
   
	CCalEntry* anniv = storedEntries[0];
    test( anniv->StartTimeL().TimeMode() == TCalTime::EFloating );
    TDateTime startDateTime = anniv->StartTimeL().TimeLocalL().DateTime();
    test( startDateTime.Hour() == 0); // Ensures that start time 
    TTime start = anniv->StartTimeL().TimeLocalL();
   
    CCalAlarm* alarm = anniv->AlarmL();
    if (! alarm) 
        {
        User::Leave( KErrNotFound );
        }
    TTimeIntervalMinutes offset = alarm->TimeOffset();
    delete alarm;
  
    TTime alarmTime = start - offset;
    TDateTime alarmDateTime = alarmTime.DateTime();
    test( alarmDateTime.Hour() == 6 ); // Ensures that alarm floats
    CleanupStack::PopAndDestroy( &storedEntries );
	}

void CDataExchangeTestManager::TestOutOfRangeRDateL(TBool aIsSynch)
	{
	_LIT(KRDateFile, "out_of_range.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CreateTestFileL(KOutOfRangeRDate, KRDateFile);
	ImportL(KRDateFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	RArray<TCalTime> dates;
	entries[0]->GetRDatesL(dates);
	test(dates.Count() == 0);
	entries.ResetAndDestroy();
	
	// ex dates
	CreateTestFileL(KOutOfRangeExDate, KRDateFile);
	ImportL(KRDateFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	entries[0]->GetExceptionDatesL(dates);
	test(dates.Count() == 0);

	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}

void CDataExchangeTestManager::TestInvalidUntilDateL(TBool aIsSynch)
	{
	_LIT8(KInvalidUntilDate,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:5\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"DTSTART:19990101T100000\r\n"
		"DTEND:19990101T113000\r\n"
		"RRULE:D1 19980106T000000\r\n"
		"LAST-MODIFIED:19991101T124400\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
		
	_LIT8(KOutOfRangeUntilDate,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:5\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"DTSTART:19990101T100000\r\n"
		"DTEND:19990101T113000\r\n"
		"RRULE:D1 18990106T000000\r\n"
		"LAST-MODIFIED:19991101T124400\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);

	_LIT(KInvalidUntilDateFile, "until_date_test.vcs");
	_LIT(KOutOfRangeUntilDateFile, "until_date_test2.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CreateTestFileL(KInvalidUntilDate, KInvalidUntilDateFile);
	ImportL(KInvalidUntilDateFile, entries, aIsSynch);
	test(entries.Count() == 1);
	TCalRRule dummy;
	test(entries[0]->GetRRuleL(dummy) == EFalse);
	entries.ResetAndDestroy();
	
	CreateTestFileL(KOutOfRangeUntilDate, KOutOfRangeUntilDateFile);
	ImportL(KOutOfRangeUntilDateFile, entries, aIsSynch);
	test(entries.Count() == 1);
	test(entries[0]->GetRRuleL(dummy) == EFalse);
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}

void CDataExchangeTestManager::TestInvalidExceptionsL(TBool aIsSynch)
	{
	_LIT(KInvalidExceptionsFile, "exception_test.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CreateTestFileL(KInvalidExceptions, KInvalidExceptionsFile);
	ImportL(KInvalidExceptionsFile, entries, aIsSynch);
	test(entries.Count() == 1);
	RArray<TCalTime> exceptions;
	entries[0]->GetExceptionDatesL(exceptions);
	test(exceptions.Count() == 0);
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}

void CDataExchangeTestManager::TestFloatingRecIdL(TBool aIsSynch)
	{
	_LIT(KFloatingRecIdFile, "floating_rec_id.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CreateTestFileL(KFloatingRecId, KFloatingRecIdFile);
	ImportL(KFloatingRecIdFile, entries, aIsSynch);
	test(entries.Count() == 3);
	
	for (TInt i = 0; i < entries.Count(); i++)
		{
		TCalTime recId = entries[i]->RecurrenceIdL();
		if (recId.TimeUtcL() != Time::NullTTime())
			{
			test(recId.TimeMode() == TCalTime::EFloating);
			}
		}
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	
	HBufC8* guid = entries[0]->UidL().AllocLC();
	entries.ResetAndDestroy();
	
	iTestLib->SynCGetEntryViewL().FetchL(*guid, entries);
	test(entries.Count() == 3);
	CleanupStack::PopAndDestroy(guid); 
	
	for (TInt i = 0; i < entries.Count(); i++)
		{
		TCalTime recId = entries[i]->RecurrenceIdL();
		if (recId.TimeUtcL() != Time::NullTTime())
			{
			test(recId.TimeMode() == TCalTime::EFloating);
			}
		}
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}

void CDataExchangeTestManager::TestMonthlyRepeatDateL(TBool aIsSynch)
	{
	_LIT(KMonthlyRepeatDateFile, "floating_rec_id.vcs");
	
	iTestLib->CleanDatabaseL();
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CreateTestFileL(KMonthlyRepeatDate, KMonthlyRepeatDateFile);
	ImportL(KMonthlyRepeatDateFile, entries, aIsSynch);
	test(entries.Count() == 1);

	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	FetchAllL(instances);
	test(instances.Count() > 0);
	for (TInt i = 0; i < instances.Count(); i++)
		{
		TDateTime instDate = instances[i]->Time().TimeLocalL().DateTime();
		test(instDate.Day() == 21); // check date is 22nd of each month
		}
	
	CleanupStack::PopAndDestroy(&instances); // instances.ResetAndDestroy()
	}

/**
Create a test file containing a pair of vCal parent-child entries as specified, then import the 
entries in the file and store the entries into the current view.  Retrieve from the current view 
all the entries sharing the same UID as the ones imported, then export those entries into a test 
file, and re-import it again.

Verify that after the import->export->import steps, the entries maintain consistency.  Caller can
also check if the parent entry's repeating range has been changed as a result of the overlapping
child repeating range.

@param aVcal The text containing the parent-child vcal entries to be tested
@param aTestParentRptStartTrimToRecId if ETrue, the parent repeat range start will be tested 
       against the RecurID to check if they are the same.  If they are not, then the test will
       fail.  If EFalse, no checking will be performed.
@param aTestParentRptUntilTrimToRecId if ETrue, the parent repeat range end will be tested 
       against the RecurID to check if they are the same.  If they are not, then the test will
       fail.  If EFalse, no checking will be performed.
@param aNewRecId ignore if value is equal to Time::NullTTime().  Otherwise, check if the recID
       equals to this value after it's been imported and stored.
*/
void CDataExchangeTestManager::TestImportExportParentChildEntriesL(
    const TDesC8& aVcal,
    TBool aIsSynch,
    const TBool aTestParentRptStartTrimToRecId,
    const TBool aTestParentRptUntilTrimToRecId,
    const TTime& aNewRecId)
	{
	TBuf<64> exportFileName;
    TBuf<64> importFileName;
    exportFileName.Format(KEntryExportFile(), 0);
    importFileName.Format(KEntryImportFile(), 0);

    // Create a test file with calender entries
    test.Printf(_L("Writing test entry data to vcs file"));
               
    CreateTestFileL(aVcal, importFileName);

    // keep a copy of the Uid, the RecurID of entries to be imported
    TCalTime recID;
    RBuf8   uId; 
    CleanupClosePushL(uId);

    // Import the calender entries from the test file
    RPointerArray<CCalEntry> storedEntries;
    CleanupResetAndDestroyPushL(storedEntries);
    ImportL(importFileName, storedEntries, aIsSynch);    
    test(storedEntries.Count() == 2);       // 2 entries - one parent & one child
   
    CCalEntry* parentry = storedEntries[0]; //parent entry
    uId.CreateL(parentry->UidL());          // remembers the Uid
    
    TDateTime DDateTime = parentry->StartTimeL().TimeLocalL().DateTime();
    test.Printf(_L("Parent Start time: %d.%d.%d %d:%d:%d"), DDateTime.Day()+1, DDateTime.Month()+1,
        DDateTime.Year(), DDateTime.Hour(), DDateTime.Minute(), DDateTime.Second());

    CCalEntry* entry = storedEntries[1]; //child entry
    recID = entry->RecurrenceIdL();         // remembers the Recurrent ID
    TDateTime recIDDateTime = recID.TimeLocalL().DateTime();
    test.Printf(_L("Child RecurrenceID time: %d.%d.%d %d:%d:%d"), recIDDateTime.Day()+1, recIDDateTime.Month()+1,
        recIDDateTime.Year(), recIDDateTime.Hour(), recIDDateTime.Minute(), recIDDateTime.Second());
    CalCommon::TRecurrenceRange range = entry->RecurrenceRangeL();
   
     // store the imported entries into the current view
    TInt success = 0;
    iTestLib->SynCGetEntryViewL().StoreL(storedEntries, success);
    test(storedEntries.Count() == success); // ensure all entries are stored

    CleanupStack::PopAndDestroy(&storedEntries);

    // retrieve from the current view all the entries matching the UID of the entries
    // previously imported 
    RPointerArray<CCalEntry> exportEntries;
    CleanupResetAndDestroyPushL(exportEntries);
    iTestLib->SynCGetEntryViewL().FetchL(uId, exportEntries); 
    test(exportEntries.Count() == success);

    // Export those entries into a test file
    ExportL(exportFileName, exportEntries, aIsSynch);

    // Flush all data contained within the current view
    iTestLib->CleanDatabaseL();
 
    // Import the entries that are just exported from the view
    RPointerArray<CCalEntry> importedEntries;
    CleanupResetAndDestroyPushL(importedEntries);
  
    ImportL(exportFileName, importedEntries, aIsSynch);
   
    test(importedEntries.Count() == exportEntries.Count()); // entries imported should be the same
                                                            // as previously exported    

    // Store the imported entries into the current view again
    iTestLib->SynCGetEntryViewL().StoreL(importedEntries, success);
    test(importedEntries.Count() == success);   // ensure all entries imported are stored               

    CleanupStack::PopAndDestroy(&importedEntries);

    // fetch the imported entries from the current view again
    RPointerArray<CCalEntry> postExportEntries;
    CleanupResetAndDestroyPushL(postExportEntries);
    iTestLib->SynCGetEntryViewL().FetchL(uId, postExportEntries); 

    // check if the entries  export are the same
    test(exportEntries.Count() == postExportEntries.Count());
    test(exportEntries.Count() == 2);
    test(exportEntries[0]->CompareL(*postExportEntries[0]));
    test(exportEntries[1]->CompareL(*postExportEntries[1]));
    
    // 1. Child Entry check - ensure import/export is consistent
    TCalTime expRecID = postExportEntries[1]->RecurrenceIdL();
   
    TDateTime expRecIDDateTime = expRecID.TimeLocalL().DateTime();
    test.Printf(_L("Child RecurrenceID time: %d.%d.%d %d:%d:%d"), expRecIDDateTime.Day()+1, expRecIDDateTime.Month()+1,
        expRecIDDateTime.Year(), expRecIDDateTime.Hour(), expRecIDDateTime.Minute(), expRecIDDateTime.Second());
 
    CalCommon::TRecurrenceRange expRange = postExportEntries[1]->RecurrenceRangeL();

    // RecurID may be changed if it is imported DATE only, thus the RecID is not checked against 
    // the original import version.  
    // Note: In the current implementation, when the recurID (for both DATE only or DateTime recurID), is not a valid 
    // instance of the parent, it will attempt to find the nearest parent instance in the future.  If the nearest 
    // instance is within the same day as the original recurID day in local time, the recurID will be replaced with
    // the date & time of this instance.  
    // test(exportEntries[1]->RecurrenceIdL().TimeLocalL() == recID.TimeLocalL());
    // test(postExportEntries[1]->RecurrenceIdL().TimeLocalL() == recID.TimeLocalL());    

    // Check if the recurID is changed to a specified date/time.
    if (aNewRecId != Time::NullTTime())
        {
        test(postExportEntries[1]->RecurrenceIdL().TimeLocalL() == aNewRecId);
        }
    test(postExportEntries[1]->RecurrenceIdL().TimeLocalL() == exportEntries[1]->RecurrenceIdL().TimeLocalL());
    test(exportEntries[1]->RecurrenceRangeL() == postExportEntries[1]->RecurrenceRangeL());

    // Range check skipped as StoreL() changes the range by re-interpreting the range
    // base on the relative position of RecID from the beginning & end of the child
    // repeating range.  Refer to the document Interim API Migration.doc or 
    // AgnModel's implementation of CAgnEntryModel::AddRepeatingChildEntryUpdateParentRuleL
    // for more details
    // test(exportEntries[1]->RecurrenceRangeL() == range);

    // 2. Parent entry check - ensure import/export is consistent
    ASSERT(exportEntries[0]->RecurrenceIdL().TimeUtcL() == Time::NullTTime());    
    ASSERT(postExportEntries[0]->RecurrenceIdL().TimeUtcL() == Time::NullTTime());    

    // check if both before and after contains a Repeat Rule or if both 
    // do not    
    TCalRRule exportRRuleAfterImport;
    TCalRRule exportRRuleAfterReImport;
    TBool ruleExistsAfterImport = exportEntries[0]->GetRRuleL(exportRRuleAfterImport);
    TBool ruleExistsAfterReImport = postExportEntries[0]->GetRRuleL(exportRRuleAfterReImport);
    test(ruleExistsAfterImport == ruleExistsAfterReImport);

    // if both contains a repeat rule
    if (ruleExistsAfterImport && ruleExistsAfterReImport)
        {
        // ensure the repeat rules' start & end time are the same
        test(exportRRuleAfterImport.DtStart().TimeUtcL() == exportRRuleAfterReImport.DtStart().TimeUtcL());
        test(exportRRuleAfterImport.Until().TimeUtcL() == exportRRuleAfterReImport.Until().TimeUtcL());

        // test if the repeat rule's start / end time is adjusted due to:
        // 1. Recurrence range: ThisAndFuture / ThisAndPrior
        // 2. Recurrence range: ThisOnly - modified the parent range to remove 
        //                                 overlapping parent entries
        if (aTestParentRptStartTrimToRecId)
            {
            test.Printf(_L("Check if Parent's repeat range trimmed in the head after StoreL()..."));

            // NOTE that imported recID maybe changed after StoreL.  Use the Fetched entries for 
            // verification.
            test(exportRRuleAfterImport.DtStart().TimeUtcL() == exportEntries[1]->RecurrenceIdL().TimeUtcL());
            }
        else if (aTestParentRptUntilTrimToRecId)
            {
            test.Printf(_L("Check if Parent's repeat range trimmed in the tail after StoreL()..."));

            // NOTE that imported recID maybe changed after StoreL.  Use the Fetched entries for 
            // verification.
            test(exportRRuleAfterImport.Until().TimeUtcL() == exportEntries[1]->RecurrenceIdL().TimeUtcL());
            }
        }
 

    // clean up
    CleanupStack::PopAndDestroy(&postExportEntries);
    CleanupStack::PopAndDestroy(&exportEntries);
    CleanupStack::PopAndDestroy(&uId);
    }

/**
The purpose of this test is to verify that vCal entries with repeating child can be safely imported, stored 
then exported.  The exported entries when re-imported should be consistent throughout.

Note that the actual text file may not be the same when comparing what's imported and exported.  The focus 
here is the representation in memory during runtime. 
*/
void CDataExchangeTestManager::TestImportStoreParentChildEntriesL(TBool aIsSynch)
     {
	test.Printf(_L("Start import->store->export->import->export repeating child entry test - TestImportStoreParentChildEntriesL..."));    

    TestImportExportParentChildEntriesL(KParentChildVCalEventWithTz, aIsSynch);
    TestImportExportParentChildEntriesL(KParentChildVCalEventNoTz, aIsSynch,
                                        EFalse,
                                        ETrue);     // Tail trimmed to RecID

    // Equal amount of child rpt time range before RecID & after RecID
    // --> ThisAndFuture instances
    //
    // Parent: Rpt Range<May 16 - 25 13:30 - 14:30>
    // Child:  Rpt Range<May 20 - 22> RecID<May 21 13:30>
    TestImportExportParentChildEntriesL(KParentChildVCalEventEqualBeforeAndAfterRecId, aIsSynch, 
                                        EFalse, 
                                        ETrue);     // Tail trimmed to RecID

    // Greater amount of child rpt time range before RecID then after RecID
    // --> ThisAndPrior instances    
    //
    // Parent: Rpt<May 16 - 25 13:30 - 14:30>
    // Child:  Rpt<May 20 - 25> RecID<May 24 13:30> 
    TestImportExportParentChildEntriesL(KParentChildVCalOverlappingChildHeadTrimmed, aIsSynch,
                                        ETrue,      // Head trimmed to RecID     
                                        EFalse);   

    // Smaller amount of child rpt time range before RecID then after RecID
    // --> ThisAndFuture instances    
    //
    // KParentChildVCalendar5 is the same as KParentChildVCalendar4 except
    // an extra RDate in the child
    //
    // Parent: Rpt<May 16 - 25>
    // Child:  Rpt<May 20 - Jun 11> RecID<May 24> 
    TestImportExportParentChildEntriesL(KParentChildVCalOverlappingChildTailTrimmed, aIsSynch,
                                        EFalse,
                                        ETrue);     // Tail trimmed to RecID

    // Smaller amount of child rpt time range before RecID then after RecID
    // --> ThisAndFuture instances    
    //
    // Parent: Rpt<May 16 - 25>
    // Child:  Rpt<May 27 - Jun 29> RecID<May 24> 
    TestImportExportParentChildEntriesL(KParentChildVCalNonOverlappingChildTailTrimmed, aIsSynch,
                                        EFalse,
                                        ETrue);     // Tail trimmed to RecID    

    // Greater amount of child rpt time range before RecID then after RecID
    // --> ThisAndPrior instances    
    //
    // Parent: Rpt<May 16 - 25>
    // Child:  Rpt<May 10 - 15> RecID<May 24> 
    TestImportExportParentChildEntriesL(KParentChildVCalNonOverlappingChildHeadTrimmed, aIsSynch,
                                        ETrue,      // Head trimmed to RecID     
                                        EFalse);

    // Greater amount of child rpt time range before RecID then after RecID
    // --> ThisAndFuture instances    
    //    
    // Parent: May 16 - 25 / Daily / 13:30 - 14:30
    //         May 28 / 15:30            
    // Child:  May 10 - 15 / Daily / 13:30 - 14:30 / RecID=May 28 15:30  
    TestImportExportParentChildEntriesL(KParentChildVCalNonOverlappingChildRecIdIsParentRDate, aIsSynch,
                                        EFalse,      // Head trimmed to RecID     
                                        ETrue);

    // Valid Parent & Child repeating rules, RecID is DATE only on parent start
    // --> ThisAndPrior instances    
    //    
    // Parent: May 16 - 25 / Daily / 13:30 - 14:30
    // Child:  May 10 - 15 / Daily / 13:30 - 14:30 / RecID=May 16 DATE only
    TestImportExportParentChildEntriesL(KParentChildVCalNonOverlappingChildDateOnlyRecIdIsParentHead, aIsSynch,
                                        ETrue,      // Head trimmed to RecID     
                                        EFalse,
                                        TTime(_L("20050415:133000.")));

    // Valid Parent & Child repeating rules, RecID is DATE only on parent end
    // --> ThisAndFuture instances    
    //    
    // Parent: May 16 - 25 / Daily / 13:30 - 14:30
    // Child:  May 10 - 15 / Daily / 13:30 - 14:30 / RecID=May 25 DATE only
    TestImportExportParentChildEntriesL(KParentChildVCalNonOverlappingChildDateOnlyRecIdIsParentTail, aIsSynch,
                                        EFalse,      
                                        ETrue,      // Tail trimmed to RecID
                                        TTime(_L("20050424:133000.")));  

    // Valid Parent & Child repeating rules, RecID is DATE only with the parent
    // repeat range
    // --> ThisAndFuture instances    
    //    
    // Parent: May 16 - 25 / Daily / 13:30 - 14:30
    // Child:  May 10 - 15 / Daily / 13:30 - 14:30 / RecID=May 18 DATE only
    TestImportExportParentChildEntriesL(KParentChildVCalNonOverlappingChildDateOnlyRecIdHeadTrimmed, aIsSynch,
                                        ETrue,      // Head trimmed to RecID
                                        EFalse,
                                        TTime(_L("20050417:143000.")));      

	test.Printf(_L("Completed import->store->export->import->export repeating child entry test - TestImportStoreParentChildEntriesL..."));
     }
 

void CDataExchangeTestManager::TestRDateAndExceptionL(TBool aIsSynch)
	{
	test.Next(_L("test import entry with three RDate and set an exception on it"));	
	_LIT(KImportFile, "tcal_entry21_import.vcs");
	
    iTestLib->CleanDatabaseL();	

	RPointerArray<CCalEntry> importedentry;
	CleanupResetAndDestroyPushL(importedentry);
	//	"RDATE:20060410T144724Z;20060411T144724Z;20060412T144724Z\r\n"
	// Import a vCal with three RDate as above and save it in the file
	CreateTestFileL(KEntry21, KImportFile);
	ImportL(KImportFile, importedentry, aIsSynch);
	test(importedentry.Count()==1);

	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(importedentry, success);

	test(success == 1);

	//Create a Child entry replace the last RDate
	TCalTime recId;
	TDateTime datetime(2006, EApril, 11, 14, 47, 24, 0);
	recId.SetTimeUtcL(datetime);
	HBufC8* guid = importedentry[0]->UidL().AllocL();
	CCalEntry* child = CCalEntry::NewL(importedentry[0]->EntryTypeL(), guid, CCalEntry::EMethodNone, 0, recId, CalCommon::EThisOnly);
	//Child start time is 5 days in future
	TTimeIntervalDays day(5);
	TTime childStartTime(datetime);
	childStartTime = childStartTime+day;

	TCalTime childStartTimecalTime;
	childStartTimecalTime.SetTimeUtcL(childStartTime);
	child->SetStartAndEndTimeL(childStartTimecalTime, childStartTimecalTime);
	//Save the child to the file
	importedentry.ResetAndDestroy();
	importedentry.AppendL(child);
	iTestLib->SynCGetEntryViewL().StoreL(importedentry, success);

	CleanupStack::PopAndDestroy(&importedentry);
	//Find instances from the file. Set time range so that the child is not included	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL( instances );

	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime startTime;
	startTime.SetTimeUtcL(TCalTime::MinTime());
	childStartTime = childStartTime-day;
	TCalTime endTime;
	endTime.SetTimeUtcL(childStartTime);
	CalCommon::TCalTimeRange timeRange(startTime, endTime);

	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, filter, timeRange);
	//Instances should be 2 since the one 20060412T144724Z is replaced by the child
	test(instances.Count()==2);
	RArray<TCalTime> rDateList;
    instances[0]->Entry().GetRDatesL(rDateList);
	//the 20060410T144724Z RDate should not be added, 
	//because it has the same date/time as the entry itself
	test(rDateList.Count() == 2);
	rDateList.Close();
	CleanupStack::PopAndDestroy( &instances );  
	}

void CDataExchangeTestManager::BadDaylightRuleL(TBool aIsSynch)
	{
	_LIT(KBadDaylightFilePanic, "bad_daylight_rule_panic.vcs");
	_LIT(KBadDaylightFileNoTz,  "bad_daylight_rule_no_tz.vcs");

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

    CreateTestFileL(KBadDaylightRulePanic, KBadDaylightFilePanic);
    CreateTestFileL(KBadDaylightRuleNoTz, KBadDaylightFileNoTz);
    
    // check panic
    ImportL(KBadDaylightFilePanic, entries, aIsSynch);
    test(entries.Count() == 1);
    CTzRules* tzRules = entries[0]->TzRulesL();
    test(tzRules->Count() == 1);
    delete tzRules; tzRules = NULL;
    entries.ResetAndDestroy();

	// check Daylight rule with no TZ
    ImportL(KBadDaylightFileNoTz, entries, aIsSynch);
    test(entries.Count() == 1);
    tzRules = entries[0]->TzRulesL();
    test(tzRules != NULL);
    
    // This time zone contains 5 rules:
    // 0-2005, 2005 DAYLIGHT RULE, 2005-2007, 2007 DAYLIGHT RULE, 2007-2100
    test(tzRules->Count() == 5); 
    delete tzRules;
    
    CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}

void CDataExchangeTestManager::TestImportParentChildEntriesL(const TDesC8& aVcal, TBool aIsSynch)
    {
    RPointerArray<CCalEntry> storedEntries;
    CleanupResetAndDestroyPushL(storedEntries);
    TBuf<64> exportFileName;
    TBuf<64> importFileName;
    exportFileName.Format(KEntryExportFile(), 0);
    importFileName.Format(KEntryImportFile(), 0);

    // Import the entry
    test.Printf(_L("Import and store not matching parent and child entries"));
               
    CreateTestFileL(aVcal, importFileName);
    ImportL(importFileName, storedEntries, aIsSynch);
    test(storedEntries.Count() == 3);
    
    CCalEntry* parentry = storedEntries[0]; //parent entry
    TDateTime DDateTime = parentry->EndTimeL().TimeLocalL().DateTime();
    test.Printf(_L("Parent End time: %d.%d.%d %d:%d:%d"), DDateTime.Day()+1, DDateTime.Month()+1,
        DDateTime.Year(), DDateTime.Hour(), DDateTime.Minute(), DDateTime.Second());
    TInt hour = DDateTime.Hour();
    TInt minute = DDateTime.Minute();
    TInt second = DDateTime.Second();

    CCalEntry* entry = storedEntries[1]; //child entry
    TCalTime recID = entry->RecurrenceIdL();
    TDateTime recIDDateTime = recID.TimeLocalL().DateTime();
    test.Printf(_L("Child RecurrenceID time: %d.%d.%d %d:%d:%d"), recIDDateTime.Day()+1, recIDDateTime.Month()+1,
        recIDDateTime.Year(), recIDDateTime.Hour(), recIDDateTime.Minute(), recIDDateTime.Second());
    CalCommon::TRecurrenceRange range = entry->RecurrenceRangeL();
   
    TInt success = 0;
    iTestLib->SynCGetEntryViewL().StoreL(storedEntries, success);
    test(storedEntries.Count() == success);
    CleanupStack::PopAndDestroy( &storedEntries );

	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);

	//Fetch all created entries
    CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
    TCalTime startTime;
    startTime.SetTimeUtcL(TTime(TDateTime(2005, EMay, 15, 0, 0, 0, 0)));
    TCalTime endTime;
    endTime.SetTimeUtcL(TTime(TDateTime(2005, EMay, 28, 0, 0, 0, 0)));
    CalCommon::TCalTimeRange timeRange(startTime, endTime);
    iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, filter, timeRange);

	test(instances.Count() > 0);
	for (TInt i = 0; i < instances.Count(); i++)
		{
    	TDateTime instTime = instances[i]->Time().TimeLocalL().DateTime();
    	test.Printf(_L("instance time: %d.%d.%d %d:%d:%d"), instTime.Day()+1, instTime.Month()+1,
        	instTime.Year(), instTime.Hour(), instTime.Minute(), instTime.Second());
		
    	TTime recIDTime = instances[i]->Entry().RecurrenceIdL().TimeLocalL();
    	if (recIDTime != Time::NullTTime())
    		{
	    	TDateTime recTime = recIDTime.DateTime();
	    	test.Printf(_L("recurrence ID time: %d.%d.%d %d:%d:%d"), recTime.Day()+1, recTime.Month()+1,
	        	recTime.Year(), recTime.Hour(), recTime.Minute(), recTime.Second());
			test(recTime.Hour() == hour);
			test(recTime.Minute() == minute);
			test(recTime.Second() == second);
    		}
		}
	
	CleanupStack::PopAndDestroy(&instances); // instances.ResetAndDestroy()
    }

void CDataExchangeTestManager::TestImportStoreNotMatchingParentChildEntriesL(TBool aIsSynch)
    {
    TestImportParentChildEntriesL(KNotMatchingParentChild1, aIsSynch);
    TestImportParentChildEntriesL(KNotMatchingParentChild2, aIsSynch);
    }
void CDataExchangeTestManager::TestMailtoL()
	{
	RDesReadStream stream(KEntryMailto);
	CleanupClosePushL(stream);
	
	test.Next(_L("Importing All entries"));

	// Create ptr array for new entries
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	iDataExchange->ImportL(KUidVCalendar, stream, entryArray);
	
	const TInt KEntryArrayCount = entryArray.Count();
	for (TInt i(0) ; i < KEntryArrayCount ; ++i)
		{
		CCalEntry* entry = entryArray[i];

		// List the current entry
		test.Printf(entry->SummaryL());
		
		RPointerArray<CCalAttendee> attendees = entry->AttendeesL(); // not owned
		
		const TInt KAttendeesCount = attendees.Count();
		for (TInt j(0) ; j < KAttendeesCount ; ++j)
			{ 
			CCalAttendee* attendee = attendees[j];
			
			test.Printf(_L("Address:%S"), &attendee->Address());
			test.Printf(_L("\n"));
			test(attendee->Address().Left(7).CompareF(_L("MAILTO:")) != 0);
			}

		test.Printf(_L("\n"));
		}

	
	CleanupStack::PopAndDestroy(&entryArray);
	CleanupStack::PopAndDestroy(&stream);
	}
	
void CDataExchangeTestManager::TestExDateTzProblemL(TBool aIsSynch)
	{
	iTestLib->CleanDatabaseL();
	
	// Create ptr array for new entries
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	HBufC8* guid = NULL;
	CCalEntry* entry = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::PushL(entry);
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	TCalTime startTime, endTime;
	startTime.SetTimeLocalL(TDateTime(2005, EJanuary, 15, 12, 30, 0, 0));
	endTime.SetTimeLocalL(TDateTime(2005, EJanuary, 15, 13, 30, 0, 0));
	entry->SetStartAndEndTimeL(startTime, endTime);
				
	// add an RRule	(monthle for 10 months)		
	RArray<TInt> monthDays;
	CleanupClosePushL(monthDays);
	monthDays.AppendL(15);
	TCalRRule rRule(TCalRRule::EMonthly);
	rRule.SetDtStart(startTime);
	rRule.SetInterval(1);
	rRule.SetCount(10);							
	rRule.SetByMonthDay(monthDays);
	CleanupStack::PopAndDestroy(&monthDays);
	entry->SetRRuleL(rRule);							
	
	// add 5 exceptions
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	TCalTime exDate;
	exDate.SetTimeLocalL(TDateTime(2005, EMarch, 15, 12, 30, 0, 0));
	exceptions.AppendL(exDate);
	exDate.SetTimeLocalL(TDateTime(2005, EApril, 15, 12, 30, 0, 0));
	exceptions.AppendL(exDate);
	exDate.SetTimeLocalL(TDateTime(2005, EMay, 15, 12, 30, 0, 0));
	exceptions.AppendL(exDate);
	exDate.SetTimeLocalL(TDateTime(2005, EJune, 15, 12, 30, 0, 0));
	exceptions.AppendL(exDate);
	exDate.SetTimeLocalL(TDateTime(2005, EJuly, 15, 12, 30, 0, 0));
	exceptions.AppendL(exDate);
	
	entry->SetExceptionDatesL(exceptions);
	CleanupStack::PopAndDestroy(&exceptions);
	
	// store the entry
	TInt numSuc;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	
	// test that there is 5 instances
	TCalTime start;
	start.SetTimeLocalL(TDateTime(2003, EMarch, 0, 0, 0, 0, 0));
	TCalTime end;
	end.SetTimeLocalL(TDateTime(2008, EMarch, 0, 0, 0, 0, 0));
	CalCommon::TCalTimeRange timeRange(start, end);
	RPointerArray<CCalInstance> instanceList;
	CleanupResetAndDestroyPushL(instanceList);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceList, CalCommon::EIncludeAll, timeRange);
	test(instanceList.Count() == 5);
	iTestLib->CleanDatabaseL();
	
	// export the entry and re-import it
	_LIT(KExDateTzProblem, "exdate_tz_problem.vcs");
	ExportL(KExDateTzProblem, entryArray, aIsSynch);
	entryArray.ResetAndDestroy();
	ImportL(KExDateTzProblem, entryArray, aIsSynch);
	
	// store the re-imported entry
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	
	// test that the entry still has 5 instances 
	instanceList.ResetAndDestroy();
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instanceList, CalCommon::EIncludeAll, timeRange);
	test(instanceList.Count() == 5);
	
	CleanupStack::PopAndDestroy(&instanceList);
	CleanupStack::PopAndDestroy(&entryArray);
	}
	
void CDataExchangeTestManager::TestTzInstanceCountL(TBool aIsSynch)
	{
	_LIT8(KTzInstanceCount, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:1\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:19991101T100000\r\n"
		"DTEND:19991101T113000\r\n"
		"RRULE:D1 #3\r\n"
		"AALARM:19991101T090000\r\n"
		"RDATE:19991105T130000\r\n"
		"LAST-MODIFIED:19991101T124400\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	
	_LIT(KTzInstanceCountFile, "tz_inst_count.vcs");
	
	iTestLib->CleanDatabaseL();
	
	CreateTestFileL(KTzInstanceCount, KTzInstanceCountFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KTzInstanceCountFile, entries, aIsSynch);
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);	
	CleanupStack::PopAndDestroy(&entries);
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	FetchAllL(instances);
	test(instances.Count() == 4);
	instances.ResetAndDestroy();
	
	iTestLib->SetTimeZoneL(KTzStanley);
	
	FetchAllL(instances);
	test(instances.Count() == 4);
	instances.ResetAndDestroy();
	
	CleanupStack::PopAndDestroy(&instances);
	iTestLib->SetTimeZoneL(KDefaultTimeZone);

	}

void CDataExchangeTestManager::TestFloatingEntryFixedExDateL(TBool aIsSynch)
	{
	_LIT8(KFloatingEntryFixedExDate, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententry@Symbian.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050516T133000\r\n"
		":20050516T143000\r\n"
		"RRULE:D1 20050525T143000\r\n"
		"EXDATE:20050517T133000Z\r\n"
		"LAST-MODIFIED:20060213T105113Z\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	
	_LIT(KFloatingEntryFixedExDateFile, "float_fixed_exdate.vcs");
	
	CreateTestFileL(KFloatingEntryFixedExDate, KFloatingEntryFixedExDateFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KFloatingEntryFixedExDateFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	CleanupStack::PopAndDestroy(&entries);
	}

void CDataExchangeTestManager::TestExceptedEntryWithRDateL(TBool aIsSynch)
	{
	_LIT8(KExceptedEntryWithRDate, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:5\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"DTSTART:19990101T100000\r\n"
		"DTEND:19990101T113000\r\n"
		"RRULE:D1 #1\r\n"
		"EXDATE:19990101T100000\r\n"
		"RDATE:19990102T100000\r\n"
		"LAST-MODIFIED:19991101T124400\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	
	_LIT(KExceptedEntryWithRDateFile, "excepted_entry_with_rdate.vcs");
	
	CreateTestFileL(KExceptedEntryWithRDate, KExceptedEntryWithRDateFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KExceptedEntryWithRDateFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == 1);
	
	CleanupStack::PopAndDestroy(&entries);
	}

void CDataExchangeTestManager::TestRDateExceptionL(TBool aIsSynch)
	{
	test.Next(_L("Create entry with an exception on an RDate"));
	
	_LIT8(KRDateException, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:5\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"DTSTART:19990101T100000\r\n"
		"DTEND:19990101T113000\r\n"
		"RRULE:D1 #3\r\n"
		"RDATE:19990105T100000\r\n"
		"EXDATE:19990101T100000;19990102T100000;19990105T100000\r\n"
		"LAST-MODIFIED:19991101T124400\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
		
	_LIT(KRDateExceptionFile, "rdate_exception.vcs");
	
	CreateTestFileL(KRDateException, KRDateExceptionFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KRDateExceptionFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == 1);
	
	TCalLocalUid luid = entries[0]->LocalUidL();
	
	CleanupStack::PopAndDestroy(&entries);
	
	CCalEntry* fetchedEntry = iTestLib->SynCGetEntryViewL().FetchL(luid);
	CleanupStack::PushL(fetchedEntry);
	
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	fetchedEntry->GetExceptionDatesL(exceptions);
	const TInt KExceptionCount = exceptions.Count();
	for (TInt i = 0; i < KExceptionCount; i++)
		{
		TDateTime dt = exceptions[i].TimeLocalL().DateTime();
		test.Printf(_L("Found an exception at %d:%d on %d/%d/%d \n"), 
			dt.Hour(), dt.Minute(), dt.Day()+1, dt.Month()+1, dt.Year());
		}
	test(KExceptionCount == 3);
	CleanupStack::PopAndDestroy(&exceptions);

	CleanupStack::PopAndDestroy(fetchedEntry);
	}

void CDataExchangeTestManager::PanicOnUpdateL(TBool aIsSynch)
	{
	_LIT8(KPanicOnUpdateInitialEntry,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+02\r\n"
		"DAYLIGHT:TRUE;+03;20060326T030000;20061029T040000;;\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:28032006114126139375\r\n"
		"SUMMARY:test\r\n"
		"DTSTART:20060403T050000Z\r\n"
		"DTEND:20060403T053000Z\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"CLASS:PUBLIC\r\n"
		"LOCATION:server\r\n"
		"SEQUENCE:0\r\n"
		"X-METHOD:NONE\r\n"
		"RRULE:W1 MO TU WE TH FR 20060408T080000\r\n"
		"LAST-MODIFIED:20060328T114432Z\r\n"
		"PRIORITY:0\r\n"
		"STATUS:CONFIRMED\r\n"
		"X-SYMBIAN-LUID:2\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR"
		);
		
	_LIT8(KPanicOnUpdateModifyingEntry,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+02\r\n"
		"DAYLIGHT:TRUE;+03;20060326T030000;20061029T040000;+02;+03\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:28032006114126139375\r\n"
		"SUMMARY;QUOTED-PRINTABLE:test\r\n"
		"DTSTART:20060403T050000Z\r\n"
		"DTEND:20060403T053000Z\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"STATUS:CONFIRMED\r\n"
		"CLASS:PUBLIC\r\n"
		"LOCATION;QUOTED-PRINTABLE:server\r\n"
		"RRULE:W1 MO TU WE TH FR 20060408T080000\r\n"
		"EXDATE:20060404T000000\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR"
		);
	
	_LIT(KPanicOnUpdateFile, "panic_on_update.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	// create the initial entry	
	CreateTestFileL(KPanicOnUpdateInitialEntry, KPanicOnUpdateFile);
	ImportL(KPanicOnUpdateFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	// store the initial entry in the database	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	entries.ResetAndDestroy();
	
	// create the modifying entry	
	CreateTestFileL(KPanicOnUpdateModifyingEntry, KPanicOnUpdateFile);
	ImportL(KPanicOnUpdateFile, entries, aIsSynch);
	test(entries.Count() == 1);
	entries[0]->SetLocalUidL(100);
	
	// update the entry - this should not panic
	iTestLib->SynCGetEntryViewL().UpdateL(entries, success);
	test(success == entries.Count());
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}

void CDataExchangeTestManager::TestMidnightRecIdL(TBool aIsSynch)
	{
	_LIT8(KMidnightRecurrenceId,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:-10\r\n"
		"DAYLIGHT:TRUE;+01;20060326T010000Z;20061029T010000Z;;\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:20060523T144001-techview_agenda@example.com\r\n"
		"SUMMARY:parent\r\n"
		"DTSTART:20060522T010000Z\r\n"
		"DTEND:20060522T020000Z\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"CLASS:PUBLIC\r\n"
		"SEQUENCE:0\r\n"
		"X-METHOD:NONE\r\n"
		"ATTENDEE;ROLE=ORGANIZER;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI:techview_agenda@example.com\r\n"
		"RRULE:D1 20060526T090000\r\n"
		"EXDATE:20060524T080000Z\r\n"
		"LAST-MODIFIED:20060523T144024Z\r\n"
		"PRIORITY:0\r\n"
		"X-SYMBIAN-LUID:18\r\n"
		"END:VEVENT\r\n"	
		"BEGIN:VEVENT\r\n"
		"UID:20060523T144001-techview_agenda@example.com\r\n"
		"SUMMARY:child\r\n"
		"DTSTART:20060524T143000Z\r\n"
		"DTEND:20060524T160000Z\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"CLASS:PUBLIC\r\n"
		"SEQUENCE:0\r\n"
		"X-METHOD:NONE\r\n"
		"X-RECURRENCE-ID:20060524\r\n"
		"ATTENDEE;ROLE=ORGANIZER;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI:techview_agenda@example.com\r\n"
		"LAST-MODIFIED:20060523T144015Z\r\n"
		"PRIORITY:0\r\n"
		"X-SYMBIAN-LUID:19\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);	
	
	_LIT(KMidnightRecurrenceIdFile, "midnight_rec_id.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CreateTestFileL(KMidnightRecurrenceId, KMidnightRecurrenceIdFile);
	ImportL(KMidnightRecurrenceIdFile, entries, aIsSynch);
	test(entries.Count() == 2);
	for (TInt i = 0; i < entries.Count(); ++i)
		{
		test(entries[i]->AttendeesL().Count() == 0);
		test(entries[i]->OrganizerL() != NULL);
		}
	
	ExportImportAndCheckL(entries, aIsSynch);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);

	test(success == 2);
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}


//
// testing invalid imported daylight rule re:DEF087898
// expected result: rule should be ignored if invalid 
//
// test this by importing test data, exporting to a file and searching for DAYLIGHT token
//
// results vary with input, TBool aExpectDaylight indicates whether DAYLIGHT token is expected
// to appear in the exported file
//
// 
void CDataExchangeTestManager::TestImportingInvalidDayLightL(TImportedDaylightTest aTestToRun, TBool aExpectDaylight, TBool aIsSynch)
    {
    _LIT( KInvalidDLPropertyFileIn, "tcal_invalidDLin.vcs" );
    _LIT( KInvalidDLPropertyFileOut, "tcal_invalidDLout.vcs" );

    _LIT8( KDayLightToken, "DAYLIGHT");  // search token for verifying test
	
    if (aExpectDaylight)
       {
       test.Next(_L("test DAYLIGHT rule appears"));
       }
    else
       {
       test.Next(_L("test NO DAYLIGHT rule appears"));
       }

 	const TInt currYear = 2006;

	TTime oldTime; 
	oldTime.HomeTime();

	RTz tz;
	User::LeaveIfError(tz.Connect());
	CleanupClosePushL(tz);
	tz.SetHomeTime(TDateTime(currYear, EJanuary, 10, 10, 0, 0, 0));	

    // create desired test file based on aTestToRun
    switch (aTestToRun)
        {
        case EImportedDLRuleBadStart:
        CreateTestFileL(KInvalidDLBadStartDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLRuleBadEnd:
        CreateTestFileL(KInvalidDLBadEndDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLRuleEndBeforeStart:
        CreateTestFileL(KInvalidDLEndBeforeStartDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLRuleMissing:
        CreateTestFileL(KInvalidDLMissingDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLOneGoodRule:
        CreateTestFileL(KInvalidDLOneGoodRuleDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLTwoBadRules:
        CreateTestFileL(KInvalidDLTwoBadRulesDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLOneBadOneGoodRule:
        CreateTestFileL(KInvalidDLOneBadOneGoodRuleDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLNoTZOneGoodRule:
        CreateTestFileL(KInvalidDLNoTZOneGoodRuleDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLNoTZTwoBadRules:
        CreateTestFileL(KInvalidDLNoTZTwoBadRulesDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLNoTZOneBadOneGoodRule:
        CreateTestFileL(KInvalidDLNoTZOneBadOneGoodRuleDT, KInvalidDLPropertyFileIn);
        break;

        case EImportedDLNoTZMissing:
        CreateTestFileL(KInvalidDLNoTZMissingDT, KInvalidDLPropertyFileIn);
        break;

        default:
          User::Panic(_L("Unexpected argument."),0);      
        }

    // import data from test file just created
    RPointerArray<CCalEntry> importEntry;
    CleanupResetAndDestroyPushL(importEntry);

    ImportL(KInvalidDLPropertyFileIn, importEntry, aIsSynch);
    test(importEntry.Count() == 1);
	
    // export entry to a file to check on DAYLIGHT property existence
    ExportL(KInvalidDLPropertyFileOut, importEntry, aIsSynch);

    // find the exported file -- it should be in the temp directory
    TFileName exportedFile;
    iTestLib->TestRegister().GetTempPath(exportedFile);
    exportedFile.Append(KInvalidDLPropertyFileOut);    

    if (aExpectDaylight)
       {
       // last parameter indicates we expect to find token in exportedFile
       test(CheckStringInFileL(KDayLightToken, exportedFile, ETrue)); 
       }
    else
       {
       // should not find the token 
       // EFalse does not give expected behaviour
       test(!CheckStringInFileL(KDayLightToken, exportedFile, ETrue));        
       }  
 
    CleanupStack::PopAndDestroy(&importEntry);
    
 	tz.SetHomeTime(oldTime);
	CleanupStack::PopAndDestroy(&tz);     
    }

void CDataExchangeTestManager::TestLUIDL(TBool aIsSynch)
	{
_LIT8(KEntryLUID, "BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"BEGIN:VEVENT\r\n"
				"UID:2\r\n"
				"X-SYMBIAN-LUID:192837\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"AALARM:19991101\r\n"
				"LAST-MODIFIED:19991191T124400\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				"X-SYMBIAN-LUID:3\r\n"
				"UID:3\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTSTART:19991101\r\n"
				"DTEND:19991101T010203\r\n"
				"AALARM:19991101\r\n"
				"LAST-MODIFIED:19991191T124400\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				"X-SYMBIAN-LUID:192837\r\n"
				"UID:4\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTSTART:19991101\r\n"
				"AALARM:19991101\r\n"
				"LAST-MODIFIED:19991191T124400\r\n"
				"END:VEVENT\r\n"
				"BEGIN:VEVENT\r\n"
				"X-SYMBIAN-LUID:192839\r\n"
				"UID:5\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"DTEND:19991101T010203\r\n"
				"AALARM:19991101\r\n"
				"LAST-MODIFIED:19991191T124400\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n");
	// Create the vcal file "vcal_alarm.vcs"
	iTestLib->CleanDatabaseL();
    test.Next(_L("Test checking LUIDs"));
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CreateTestFileL(KEntryLUID, KFileName);
	ImportL(KFileName, entryArray, aIsSynch);
	test(entryArray.Count() == 4);	
	
	RArray<TCalLocalUid> localIdBeforeStore;
	CleanupClosePushL(localIdBeforeStore);
	
	// Saving the local Uid of the entry as soon as the entry is imported.
	for( TInt i = 0; i < entryArray.Count(); i++ )
		{
		localIdBeforeStore.AppendL(entryArray[i]->LocalUidL());
		}
		
	// Store entries
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, success);
	test(entryArray.Count() == success);
		
	// Check the local Uid of the entry after storing the imported entries
	for (TInt i=0; i<entryArray.Count(); i++)
		{
		TCalLocalUid localUid = entryArray[i]->LocalUidL();
		TCalLocalUid localUid2 = localIdBeforeStore[i];
		test(localUid == localUid2);
		}
	CleanupStack::PopAndDestroy(&localIdBeforeStore);
	CleanupStack::PopAndDestroy(&entryArray);
	}

void CDataExchangeTestManager::TestRRuleUntilTimeL(TBool aIsSynch)
	{
	_LIT(KExportFilePath, "c:\\logs\\tcal_dataexchange\\temp\\");
	_LIT8(KTokenToFind, "20091117T170000");
	_LIT8(KEntryWithTZRule, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:-02\r\n"
		"DAYLIGHT:TRUE;-03;20050904T020000;20060416T020000;;\r\n"
		"BEGIN:VEVENT\r\n"
		"UID;ENCODING=QUOTED-PRINTABLE:todoparententryanish=40example.com\r\n"
		"DTSTART:20050516T170000\r\n"
		"DTEND:20050516T180000\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"CLASS:PUBLIC\r\n"
		"SEQUENCE:0\r\n"
		"X-METHOD:NONE\r\n"
		"RRULE:MP6 3+ MO TU #10\r\n"
		"LAST-MODIFIED:20060213T105113Z\r\n"
		"PRIORITY:0\r\n"
		"X-SYMBIAN-LUID:2\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	
	_LIT(KEntryWithTZRuleFile, "entry_with_rrule.vcs");
	
	CreateTestFileL(KEntryWithTZRule, KEntryWithTZRuleFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KEntryWithTZRuleFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	
	RPointerArray<CCalEntry> fetchedEntries;
	CleanupResetAndDestroyPushL(fetchedEntries);

	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(entries[0]->LocalUidL());
	CleanupStack::PushL(entry);
	fetchedEntries.AppendL(entry); /* ownership transferred */
	CleanupStack::Pop();

	ExportL(KEntryExportFile, fetchedEntries, aIsSynch);	
		
		
	TBuf<100> fileName(KExportFilePath);
	fileName.Append(KEntryExportFile);
			
	test(CheckStringInFileL(KTokenToFind, fileName, ETrue));

	CleanupStack::PopAndDestroy(2, &entries);//fetchedEntries
	}

void CDataExchangeTestManager::TestMidnightRecId2L(TBool aIsSynch)
	{
	//This test ensures that when recurrence Id is without time 
	//and timezone information, a correct instance is nudged for
	//daily repeat entries.
	_LIT8(KMidnightRecurrenceId,	
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"TZ:+10\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:i36ZGF7w4EDKX240d65552\r\n"
	"SUMMARY:Daily 5\r\n"
	"DTSTART:20060508T220000Z\r\n"
	"DTEND:20060508T230000Z\r\n"
	"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
	"CLASS:PRIVATE\r\n"
	"LOCATION:Home\r\n"
	"SEQUENCE:0\r\n"
	"X-METHOD:NONE\r\n"
	"RRULE:D1 #2\r\n"
	"LAST-MODIFIED:20060428T135510Z\r\n"
	"PRIORITY:2\r\n"
	"X-SYMBIAN-LUID:1205\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);
	
	_LIT8(KMidnightRecurrenceId2,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	
	"BEGIN:VEVENT\r\n"
	"UID:i36ZGF7w4EDKX240d65552\r\n"
	"SUMMARY:Daily exc\r\n"
	"DTSTART:20060509T150000Z\r\n"
	"DTEND:20060509T160000Z\r\n"
	"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
	"CLASS:PRIVATE\r\n"
	"LOCATION:Away\r\n"
	"SEQUENCE:0\r\n"
	"X-METHOD:NONE\r\n"
	"X-RECURRENCE-ID:20060510\r\n"
	"LAST-MODIFIED:20060428T135617Z\r\n"
	"PRIORITY:2\r\n"
	"X-SYMBIAN-LUID:1206\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);	
	
	_LIT(KMidnightRecurrenceIdFile, "midnight_rec_id.vcs");
	_LIT(KMidnightRecurrenceIdFile2, "midnight_rec_id2.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CreateTestFileL(KMidnightRecurrenceId, KMidnightRecurrenceIdFile);
	CreateTestFileL(KMidnightRecurrenceId2, KMidnightRecurrenceIdFile2);
	ImportL(KMidnightRecurrenceIdFile, entries, aIsSynch);
	ImportL(KMidnightRecurrenceIdFile2, entries, aIsSynch);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);

	test(success == 2);
	
	//The following test was added for defect fix PDEF097211
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	TCalTime startTime;
	startTime.SetTimeUtcL( TTime( TDateTime( 2006, EMay, 7, 0, 0, 0, 0) ) );
    TCalTime endTime;
    endTime.SetTimeUtcL( TTime( TDateTime( 2006, EMay, 8, 23, 59, 0, 0) ) );
    CalCommon::TCalTimeRange timeRange(startTime, endTime);
   	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
    iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, filter, timeRange);
	test(instances.Count() == 2);

	CleanupStack::PopAndDestroy(&instances);	
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}

void CDataExchangeTestManager::TestMidnightRecId3L(TBool aIsSynch)
	{
	//This test ensures that when recurrence Id is without time 
	//and timezone information, a correct instance is nudged for 
	//weekly repeat entries.	
	_LIT8(KMidnightRecurrenceId,	
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	"TZ:+10\r\n"
	"BEGIN:VEVENT\r\n"
	"UID:i36ZGF7w4EDKX240d65552\r\n"
	"SUMMARY:Weekly 5\r\n"
	"DTSTART:20060507T220000Z\r\n"
	"DTEND:20060507T230000Z\r\n"
	"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
	"CLASS:PRIVATE\r\n"
	"LOCATION:Home\r\n"
	"SEQUENCE:0\r\n"
	"X-METHOD:NONE\r\n"
	"RRULE:W1 MO #5\r\n"
	"LAST-MODIFIED:20060428T135510Z\r\n"
	"PRIORITY:2\r\n"
	"X-SYMBIAN-LUID:1205\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);
	
	_LIT8(KMidnightRecurrenceId2,
	"BEGIN:VCALENDAR\r\n"
	"VERSION:1.0\r\n"
	
	"BEGIN:VEVENT\r\n"
	"UID:i36ZGF7w4EDKX240d65552\r\n"
	"SUMMARY:Weekly exc\r\n"
	"DTSTART:20060514T150000Z\r\n"
	"DTEND:20060514T160000Z\r\n"
	"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
	"CLASS:PRIVATE\r\n"
	"LOCATION:Away\r\n"
	"SEQUENCE:0\r\n"
	"X-METHOD:NONE\r\n"
	"X-RECURRENCE-ID:20060515\r\n"
	"LAST-MODIFIED:20060428T135617Z\r\n"
	"PRIORITY:2\r\n"
	"X-SYMBIAN-LUID:1206\r\n"
	"END:VEVENT\r\n"
	"END:VCALENDAR\r\n"
	);	
	
	_LIT(KMidnightRecurrenceIdFile, "midnight_rec_id.vcs");
	_LIT(KMidnightRecurrenceIdFile2, "midnight_rec_id2.vcs");
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	CreateTestFileL(KMidnightRecurrenceId, KMidnightRecurrenceIdFile);
	CreateTestFileL(KMidnightRecurrenceId2, KMidnightRecurrenceIdFile2);
	ImportL(KMidnightRecurrenceIdFile, entries, aIsSynch);
	ImportL(KMidnightRecurrenceIdFile2, entries, aIsSynch);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);

	test(success == 2);
	
	CleanupStack::PopAndDestroy(&entries); // entries.ResetAndDestroy()
	}
	
void CDataExchangeTestManager::AlarmOffsetForUndatedTodoL(TBool aIsSynch)
	{
	//Test the for for DEF081992: Remote sync/To do: Task with only alarm set gets wrong alarm time
	test.Next(_L("test an alarm's offset for an undated todo"));	
	_LIT(KImportFile, "tcal_entry11_import.vcs");
	CreateTestFileL(KUndatedAlarmedTodoVCalendar, KImportFile);
	RPointerArray<CCalEntry> importedentry;
	CleanupResetAndDestroyPushL(importedentry);
	ImportL(KImportFile, importedentry, aIsSynch);
	test(importedentry.Count()==1);

	CCalAlarm* alarm =  importedentry[0]->AlarmL();
	test(alarm->TimeOffset().Int()==0);
	delete alarm;
	CleanupStack::PopAndDestroy(&importedentry);		
	}

//! @SYMTestCaseID DataExchange_Child_DTSTART_Export
//! @SYMTestCaseDesc Tests that child's DTSTART is exported.
//! @SYMFssID App-Engines/CalInterimAPI/Repeat.005
//! @SYMTestStatus Implemented
//! @SYMTestPriority Medium
//! @SYMTestActions 1) Import parent and child entries, store them and fetch the child entry only. 
//! 				2) Export the child entry and check if a DTSTART parameter exists in the exported file.
//! @SYMTestExpectedResults DTSTART parameter should exist in the exported file.
//! @SYMTestType CT
void CDataExchangeTestManager::TestChildDTSTARTExportL(TBool aIsSynch)
	{
	_LIT(KWritingData, "Writing test entry data to vcs file");
	_LIT8(KTokenToFind, "DTSTART");
	const TInt KNumberOfEntries = 4;//2 todos and 2 events
	iTestLib->CleanDatabaseL();
	const TInt KMaxSize = 1024;
	RPointerArray<CCalEntry> storedEntries;
	CleanupResetAndDestroyPushL(storedEntries);
	TBuf<64> exportFileName;
	TBuf<64> importFileName;
	exportFileName.Format(KEntryExportFile(), 0);
	importFileName.Format(KEntryImportFile(), 0);

	// Import the entry
	test.Printf(KWritingData);
	           
	CreateTestFileL(KTestingChildDTSTARTExport, importFileName);
	ImportL(importFileName, storedEntries, aIsSynch);
	test(storedEntries.Count() == KNumberOfEntries);

	TInt stored(0);
	iTestLib->SynCGetEntryViewL().StoreL(storedEntries, stored);
	test(storedEntries.Count() == stored);

	RPointerArray<CCalEntry> fetchedEntries;
	CleanupResetAndDestroyPushL(fetchedEntries);
	CCalEntry* childEntry;
	for (TInt i = 1; i < storedEntries.Count(); i+=2)
		{
		childEntry = iTestLib->SynCGetEntryViewL().FetchL(storedEntries[i]->LocalUidL());
		CleanupStack::PushL(childEntry);
		fetchedEntries.AppendL(childEntry);
		CleanupStack::Pop();//childEntry

		ExportL(exportFileName, fetchedEntries, aIsSynch);

		RFile exportedFile;
		HBufC8* buffer = HBufC8::NewL(KMaxSize);
		TPtr8 ptr(buffer->Des());
		TBool result(EFalse);
		TBuf<100> fileName(KExportFilePath);
		fileName.Append(exportFileName);
		TInt err(exportedFile.Open(iTestLib->FileSession(), fileName, EFileRead | EFileShareAny));
		if (err == KErrNone)
			{
			err = exportedFile.Read(ptr);
			if (err == KErrNone)
				{
				TInt pos(ptr.Find(KTokenToFind));
				if (pos != KErrNotFound)
					{
					result = ETrue;
					}
				}
			exportedFile.Close();
			}
		delete buffer;
		test(result);
		}
	CleanupStack::PopAndDestroy(2, &storedEntries);//fetchedEntries
	}

//! @SYMTestCaseID DataExchange_RecurrenceID_Range
//! @SYMTestCaseDesc Tests that child's recurrence range is correct.
//! @SYMFssID App-Engines/CalInterimAPI/Repeat.005
//! @SYMTestStatus Implemented
//! @SYMTestPriority Medium
//! @SYMTestActions 1) Import parent and child entries, store them and fetch the child entry only. 
//! 				2) Checks the child's enddate - should be the same as in the original entry.
//!					3) Checks that parent enddate was changed to be the same as child recurrenceID
//! @SYMTestExpectedResults Recurrence range - should be "ThisAndFuture"
//! @SYMTestType CT
void CDataExchangeTestManager::TestReccurenceIDRangeL(TBool aIsSynch)
    {
	_LIT(KChildEndTimeFormat, "Child endtime: %d.%d.%d %d:%d:%d");
	const TInt KNumberOfEntries = 2;//1 parent and 1 child
	iTestLib->CleanDatabaseL();
	RPointerArray<CCalEntry> storedEntries;
	CleanupResetAndDestroyPushL(storedEntries);
	TBuf<64> importFileName;
	importFileName.Format(KEntryImportFile(), 0);

	// Import the entry
	CreateTestFileL(KExportRecIDRange, importFileName);
	ImportL(importFileName, storedEntries, aIsSynch);
	test(storedEntries.Count() == KNumberOfEntries);

	CCalEntry* entry = storedEntries[1]; //child entry
	TDateTime originalEndDateTime(entry->EndTimeL().TimeLocalL().DateTime());
	test.Printf(KChildEndTimeFormat, originalEndDateTime.Day()+1, originalEndDateTime.Month()+1,
		originalEndDateTime.Year(), originalEndDateTime.Hour(), originalEndDateTime.Minute(), 
		originalEndDateTime.Second());

	TInt stored(0);
	iTestLib->SynCGetEntryViewL().StoreL(storedEntries, stored);
	test(storedEntries.Count() == stored);

	//Fetch all created child entries
	CCalEntry* childEntry = iTestLib->SynCGetEntryViewL().FetchL(storedEntries[1]->LocalUidL());
	CleanupStack::PushL(childEntry);

	//Fetch parent entries
	CCalEntry* parentEntry = iTestLib->SynCGetEntryViewL().FetchL(storedEntries[0]->LocalUidL());
	CleanupStack::PushL(parentEntry);

	TDateTime endDateTime(childEntry->EndTimeL().TimeLocalL().DateTime());
	test.Printf(KChildEndTimeFormat, endDateTime.Day()+1, endDateTime.Month()+1,
		endDateTime.Year(), endDateTime.Hour(), endDateTime.Minute(), endDateTime.Second());

	test(TTime(endDateTime) == TTime(originalEndDateTime));
	
	TTime parentEndTime = parentEntry->EndTimeL().TimeLocalL();
	TTime childRecIDTime = childEntry->RecurrenceIdL().TimeLocalL();
	
	test(parentEndTime == childRecIDTime);

	CalCommon::TRecurrenceRange range = childEntry->RecurrenceRangeL();
	test(range == CalCommon::EThisAndPrior);

	CleanupStack::PopAndDestroy(2, childEntry);//parentEntry
	CleanupStack::PopAndDestroy(&storedEntries);
	}

//DEF086733
void CDataExchangeTestManager::TestExDateTimeL(TBool aIsSynch)
	{
	/**	@SYMTestCaseID APPENG-CalInterimApi-UT-ExceptionalDateTime  
	 * @SYMTestCaseDesc Test importing exception date without time
	 * @SYMTestStatus Implemented
	 * @SYMTestPriority Medium
	 * @SYMTestType UT
	 * @SYMTestActions Import the vcs file to an array of entries, then store it into the database. 
	 * Fecth the entries and check the local time of the exception date.The date should be the same
	 * as imported. The time should be the same as DTSTART time.
	 * @SYMTestExpectedResults Pass
	 * @SYMDEF DEF086733: Importing exception date without time
	 */
  	 
	_LIT(KTest, "Test Exceptional date & time");
	test.Printf(KTest);

	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	//	create new test file 
	_LIT(KExceptionDateTimeFile, "ExceptionDateTime.vcs");
	CreateTestFileL(KExceptionDateTime, KExceptionDateTimeFile);		
	ImportL(KExceptionDateTimeFile, entryArray, aIsSynch);
	test(entryArray.Count()==1);
	
	// store the entries in the database
	TInt stored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, stored);
	test(stored == entryArray.Count());
	
	RArray<TCalTime> exDateList;
   	CleanupClosePushL(exDateList);

	TTime exTime;
 
	const TTime KExpectedExDate(TDateTime(1999, EJanuary, 0, 10, 0, 0, 0));

	//fetch entry from DB according to uid
	RPointerArray<CCalEntry> fetchedEntries;
	CleanupResetAndDestroyPushL(fetchedEntries);
	
	iTestLib->SynCGetEntryViewL().FetchL(entryArray[0]->UidL(), fetchedEntries);	
	test(fetchedEntries.Count() == 1);
    
   	fetchedEntries[0]->GetExceptionDatesL(exDateList);
 	test (exDateList.Count() == 1);
		
	exTime = exDateList[0].TimeLocalL();
	test (exTime == KExpectedExDate);
			
	CleanupStack::PopAndDestroy(&fetchedEntries);
	CleanupStack::PopAndDestroy(&exDateList); 
	CleanupStack::PopAndDestroy(&entryArray);	 
 	}

/**
* Tests that the TZ field gets exported properly. The TZ offset should be that of the
* current time zone which is set to Atlantic/Stanley. TZ is only exported for entries
* that have a repeat date. The exported entry is stored in in the file 'KTestTzFile'.
*/
void CDataExchangeTestManager::TestTzExportL(TBool aIsSynch)
    {
   	SetTimeZoneL( KTzAtlanticStanley );
    
	// Create the calendar entries.
	HBufC8* uidBuf = _L8( "TZExportTestUid" ).AllocL();
	CleanupStack::PushL( uidBuf );
	
	// Create the start and end times for the entry. This entry's start date 
	// falls in a TZ rule that is in DST time.
	_LIT( startString, "19990101:100000.000000" );
	_LIT( endString,   "19990101:113000.000000" );
	TTime startTTime( startString );
	TTime endTTime( endString );
	TCalTime start;
	start.SetTimeLocalL( startTTime );
	TCalTime end;
	end.SetTimeLocalL( endTTime );
	
	// Create the entry.
	CCalEntry* entry = CCalEntry::NewL( CCalEntry::ETodo, uidBuf, 
	    CCalEntry::EMethodNone, 0 );
	CleanupStack::Pop( uidBuf );
	CleanupStack::PushL( entry );
    
    // Set start and end date.
	entry->SetStartAndEndTimeL( start, end );
	
	// Set repeat rule.
	TCalRRule rrule( TCalRRule::EDaily );
	rrule.SetInterval( 1 );
	rrule.SetDtStart( start );
	rrule.SetCount( 3 );
	entry->SetRRuleL( rrule );
	
	// Set the rdates.
	_LIT( rdateString, "19990105:090000.000000" );
	TTime rdateTTime( rdateString );
	TCalTime rruleCalTime;
	rruleCalTime.SetTimeLocalL( rdateTTime );
	RArray<TCalTime> rdatesList;
	CleanupClosePushL( rdatesList );
    rdatesList.Append( rruleCalTime );
	entry->SetRDatesL( rdatesList );
    CleanupStack::PopAndDestroy( &rdatesList ); 
  	
    // Now create the child entry.
	_LIT( recurIdString, "19990101:113000.000000" ); 
	TTime recurIdTTime( recurIdString );
	TCalTime recurId;
	recurId.SetTimeLocalL( recurIdTTime );

	HBufC8* uidBuf2 = _L8( "TZExportTestUid" ).AllocL();
    CCalEntry* childEntry = CCalEntry::NewL( CCalEntry::ETodo, uidBuf2,
        CCalEntry::EMethodNone, 0, recurId, CalCommon::EThisOnly );
	CleanupStack::PushL( childEntry );

    // Set the start and end dates.
	_LIT( childStartString, "19980411:100000.000000" );
	_LIT( childEndString,   "19980411:110000.000000" );
	TTime childStartTTime( childStartString );
	TTime childEndTTime( childEndString );
	TCalTime childStart;
	childStart.SetTimeLocalL( childStartTTime );
	TCalTime childEnd;
	childEnd.SetTimeLocalL( childEndTTime );
	childEntry->SetStartAndEndTimeL( childStart, childEnd );

    //-----------------------------------------------------------------------
    // Entry 3 - Entry with an rdate long before the start date. The DAYLIGHT
    // field should not use this date when determining the time for that field.
    // It is based on the repeat definition only.
    //-----------------------------------------------------------------------
    
    HBufC8* uidBuf3 = _L8( "3" ).AllocL();
    CCalEntry* entry3 = CCalEntry::NewL( CCalEntry::ETodo, uidBuf3,
        CCalEntry::EMethodNone, 0 );
	CleanupStack::PushL( entry3 );

    // Set the start and end dates. This entry's start date falls in a TZ rule
    // that is not in standard time.
	_LIT( startStr3, "19990519:230000.000000" );
	TTime startTTime3( startStr3 );
	TCalTime entry3Start;
	entry3Start.SetTimeLocalL( startTTime3 );

	_LIT( endStr3, "19990520:000000.000000" );
	TTime endTTime3( endStr3 );
	TCalTime entry3End;
	entry3End.SetTimeLocalL( endTTime3 );
	entry3->SetStartAndEndTimeL( entry3Start, entry3End );

	// Set repeat rule.
	TCalRRule rrule3( TCalRRule::EDaily );
	rrule3.SetInterval( 1 );
	rrule3.SetDtStart( entry3Start );
	rrule3.SetCount( 3 );
	entry3->SetRRuleL( rrule3 );
	
	// Set the rdates.
	_LIT( rdateStr3, "19500105:090000.000000" );
	TTime rdateTTime3( rdateStr3 );
	TCalTime rruleCalTime3;
	rruleCalTime3.SetTimeLocalL( rdateTTime3 );
	RArray<TCalTime> rdatesList3;
	CleanupClosePushL( rdatesList3 );
    rdatesList3.Append( rruleCalTime3 );
	entry3->SetRDatesL( rdatesList3 );
    CleanupStack::PopAndDestroy( &rdatesList3 ); 


	// Add all the entries to the list.
    RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL( entryArray );
    entryArray.Append( entry );
    entryArray.Append( childEntry );
    entryArray.Append( entry3 );

    // Store the entries in the database.
	TInt success = 0;
    iTestLib->SynCGetEntryViewL().StoreL( entryArray, success );
	test( entryArray.Count() == success );

	// Export the entries to a file
	ExportL( KTestTzFile, entryArray, aIsSynch);
	CleanupStack::PopAndDestroy( &entryArray );
	CleanupStack::PopAndDestroy( entry3 );
	CleanupStack::PopAndDestroy( childEntry );
	CleanupStack::PopAndDestroy( entry );
	
	// Now re-import the file and export it again. It relies on the calendar
	// file created in this method.
	TestTzImportL(aIsSynch);
	}

// PDEF111292 Test Cases - Begin
/**
* Tests that the TZ field gets exported properly. The TZ offset should be that of the
* current time zone which is set to Etc/GMT-9 so no DST rules are in effect. TZ is only exported for entries
* that have a repeat date. The exported entry is stored in in the file 'KTestTzFile'.
*/
void CDataExchangeTestManager::TestTzExportKSTL(TBool aIsSynch)
	{
	test.Next(_L("Test TZ exported properly with repeat-rule, start and end in UTC crossing over to next day"));	

	// Test Data
	// Asia/Tokyo (+9) 
	// appointment type
	// state date 2007/08/03 8:00am local time => 2007/08/02 11:00pm UTC
	// end date 2007/08/03 8:00am local time => 2007/08/02 11:00pm UTC
	// repeat daily for 5 days
		
	// Set start and end date in UTC that crosses over to next day in local time
	// create a repeating rule daily for 5 days
	// make sure rrule end date is correct with respsect to local time and that tz is also exported 

	SetTimeZoneL( KTzGMTMinusNine );

	// uidBuf4 owned by the 'entry4' so does not need to be deleted here
	HBufC8* uidBuf = _L8( "PDEF111292 - KST no DST rules" ).AllocL();
	CCalEntry* entry = CCalEntry::NewL( CCalEntry::EAppt, 
	                                     uidBuf, 
	                                     CCalEntry::EMethodNone, 
	                                     0 );
	CleanupStack::PushL( entry );

	_LIT( startStr, "20070701:230000.000000" ); // month and day are zero offset
	TTime startTTime( startStr );
	TCalTime entryStart;
	entryStart.SetTimeUtcL( startTTime );

	_LIT( endStr, "20070701:230000.000000" ); // month and day are zero offset
	TTime endTTime( endStr );
	TCalTime entryEnd;
	entryEnd.SetTimeUtcL( endTTime );
	entry->SetStartAndEndTimeL( entryStart, entryEnd ); // times are exported in UTC

	// Set repeat rule.
	TCalRRule rrule( TCalRRule::EDaily );
	rrule.SetInterval( 1 );
	rrule.SetDtStart( entryStart );
	rrule.SetCount( 5 );
	entry->SetRRuleL( rrule );

	// Add all the entries to the list.
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL( entryArray );

	entryArray.Append( entry );

	// Store the entries in the database.
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL( entryArray, success );
	test( entryArray.Count() == success );

	// Export the entries to a file.   
	ExportL( KTestTzFile, entryArray, aIsSynch);

	// Now re-import the file and export it again. It relies on the calendar
	// file created in this method.
	TestTzImportL(aIsSynch);

	SetTimeZoneL(KDefaultTimeZone);

	// entry, and entryArray
	CleanupStack::PopAndDestroy( 2 ); 
	}
	
/**
* Tests that the TZ field gets exported properly. The TZ offset should be that of the
* current time zone which is set to Etc/GMT+8 so no DST rules are in effect. TZ is only exported for entries
* that have a repeat date. The exported entry is stored in in the file 'KTestTzFile'.
*/
void CDataExchangeTestManager::TestTzExportPSTL(TBool aIsSynch)
	{
	test.Next(_L("Test TZ exported properly with repeat-rule, start and end in UTC crossing over to previous day"));	
	// Set start and end date in UTC that crosses over to previous day in local time
	// create a repeating rule daily for 5 days
	// make sure rrule end date is correct with respsect to local time and that tz is also exported 

	SetTimeZoneL( KTzGMTPlusEight );

	// uidBuf4 owned by the 'entry4' so does not need to be deleted here
	HBufC8* uidBuf = _L8( "PDEF111292 - PST no DST rules" ).AllocL();
	CCalEntry* entry = CCalEntry::NewL( CCalEntry::EAppt, 
	                                     uidBuf, 
	                                     CCalEntry::EMethodNone, 
	                                     0 );
	CleanupStack::PushL( entry );

	_LIT( startStr, "20070703:010000.000000" ); // month and day are zero offset
	TTime startTTime( startStr );
	TCalTime entryStart;
	entryStart.SetTimeUtcL( startTTime );

	_LIT( endStr, "20070703:010000.000000" ); // month and day are zero offset
	TTime endTTime( endStr );
	TCalTime entryEnd;
	entryEnd.SetTimeUtcL( endTTime );
	entry->SetStartAndEndTimeL( entryStart, entryEnd ); // times are exported in UTC

	// Set repeat rule.
	TCalRRule rrule( TCalRRule::EDaily );
	rrule.SetInterval( 1 );
	rrule.SetDtStart( entryStart );
	rrule.SetCount( 5 );
	entry->SetRRuleL( rrule );

	// Add all the entries to the list.
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL( entryArray );

	entryArray.Append( entry );

	// Store the entries in the database.
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL( entryArray, success );
	test( entryArray.Count() == success );

	// Export the entries to a file.   
	ExportL( KTestTzFile, entryArray, aIsSynch);

	// Now re-import the file and export it again. It relies on the calendar
	// file created in this method.
	TestTzImportL(aIsSynch);

	SetTimeZoneL(KDefaultTimeZone);

	// entry, and entryArray
	CleanupStack::PopAndDestroy( 2 ); 
	}
    
// PDEF111292 Test Cases - End    

void CDataExchangeTestManager::TestTzImportL(TBool aIsSynch)
    {
    // Set the time zone to something different than what is contained in the 
    // entry we are importing. When we export the same entry, the TZ field should
    // be the same as the imported entry and not the offset for the current system
    // timezone.
   	SetTimeZoneL( KTzAmericaVancouver );

    // Import the entries from the file created by TestTzExportL().
    TBuf<64> importFileName;	
	importFileName.Copy( KTestTzFile() );
    RPointerArray<CCalEntry> entryArray;
    CleanupResetAndDestroyPushL( entryArray );
    ImportL( importFileName, entryArray, aIsSynch);
    
    // Store the entries in the database.
	TInt success = 0;
    iTestLib->SynCGetEntryViewL().StoreL( entryArray, success );
	test( entryArray.Count() == success );
    
    // Export the entries to a file again.   
    TBuf<64> exportFileName;	
	exportFileName.Copy( KTestTzFile2() );
    ExportL( exportFileName, entryArray, aIsSynch);
    
    CleanupStack::PopAndDestroy( &entryArray );  
    SetTimeZoneL(KDefaultTimeZone);
    }

//! @SYMTestCaseID DataExchange_Parent_DTEND_Export
//! @SYMTestCaseDesc Tests that DTEND is not exported for ToDo entries.
//! @SYMFssID App-Engines/CalInterimAPI/Repeat.005
//! @SYMTestStatus Implemented
//! @SYMTestPriority Medium
//! @SYMTestActions Import parent and child entries, store them and Export them. 
//! @SYMTestExpectedResults DTEND parameter should not exist in the exported file.
//! @SYMTestType CT
void CDataExchangeTestManager::TestParentDTENDExportL(TBool aIsSynch)
	{
	_LIT(KWritingData, "Writing test entry data to vcs file");
	_LIT8(KTokenToFind, "DTEND");
	
	const TInt KNumberOfEntries = 2;
			/* one entry would suffice, but to match the defect descrition (DEF087006)
			two entries being used*/
			
	iTestLib->CleanDatabaseL();
		
	RPointerArray<CCalEntry> storedEntries;
	CleanupResetAndDestroyPushL(storedEntries);

	// Import the entry
	test.Printf(KWritingData);
	           
	CreateTestFileL(KTestingParentDTENDExport, KEntryImportFile);
	
	ImportL(KEntryImportFile, storedEntries, aIsSynch);
 	test(storedEntries.Count() == KNumberOfEntries);

	TInt stored(0);
	iTestLib->SynCGetEntryViewL().StoreL(storedEntries, stored);
	test(storedEntries.Count() == stored);

	RPointerArray<CCalEntry> fetchedEntries;
	CleanupResetAndDestroyPushL(fetchedEntries);
	

	CCalEntry* entry = iTestLib->SynCGetEntryViewL().FetchL(storedEntries[0]->LocalUidL());
	CleanupStack::PushL(entry);
	fetchedEntries.AppendL(entry); /* ownership transferred */
	CleanupStack::Pop();

	ExportL(KEntryExportFile, fetchedEntries, aIsSynch);	
		
		
	TBuf<100> fileName(KExportFilePath);
	fileName.Append(KEntryExportFile);

			
	test(CheckStringInFileL(KTokenToFind, fileName, EFalse));

	CleanupStack::PopAndDestroy(2, &storedEntries);//fetchedEntries
	}

void CDataExchangeTestManager::TestBadTimeZoneL(TBool aIsSynch)
	{
	//	setting system clock so that test case is consistent
	_LIT(KSystemDate, "20080101:000000");
	TTime systemTime;
	systemTime.Set(KSystemDate);
	User::SetHomeTime(systemTime);	
	
	// create first entry in 1999
	_LIT8(KGuid, "5");
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* entry1 = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry1);
	
	TCalTime startTime, endTime;
	startTime.SetTimeLocalL(TDateTime(1999, EApril, 16, 23, 0, 0, 0));
	endTime.SetTimeLocalL(TDateTime(1999, EApril, 17, 0, 0, 0, 0));
	entry1->SetStartAndEndTimeL(startTime, endTime);
	
	TCalRRule rule(TCalRRule::EDaily);
	rule.SetDtStart(endTime);
	rule.SetInterval(1);
	rule.SetCount(3);
	entry1->SetRRuleL(rule);
	
	RArray<TCalTime> rdateArray;
	CleanupClosePushL(rdateArray);

	TCalTime rdate1;
	rdate1.SetTimeLocalL(TDateTime(1900, EFebruary, 5, 9, 0, 0, 0));
	rdateArray.AppendL(rdate1);
	entry1->SetRDatesL(rdateArray);
	CleanupStack::PopAndDestroy(&rdateArray);
	
	iTestLib->StoreEntryL(*entry1);
	CleanupStack::PopAndDestroy(entry1);



	// create 2nd entry in 2005
	_LIT8(KGuid2, "4");
	HBufC8* guid2 = KGuid2().AllocLC();
	CCalEntry* entry2 = CCalEntry::NewL(CCalEntry::ETodo, guid2, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid2);
	CleanupStack::PushL(entry2);

	TCalTime startTime2, endTime2;
	startTime2.SetTimeLocalL(TDateTime(2005, EApril, 16, 23, 0, 0, 0));
	endTime2.SetTimeLocalL(TDateTime(2005, EApril, 17, 0, 0, 0, 0));
	entry2->SetStartAndEndTimeL(startTime2, endTime2);
	
	rule.SetDtStart(endTime2);
	entry2->SetRRuleL(rule);
	
	iTestLib->StoreEntryL(*entry2);
	CleanupStack::PopAndDestroy(entry2);
	
	// fetch entry and export	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iTestLib->SynCGetEntryViewL().FetchL(KGuid, entryArray);
	iTestLib->SynCGetEntryViewL().FetchL(KGuid2, entryArray);
	test(entryArray.Count() == 2);

	_LIT(KBadTzFile, "bad_tz.vcs");
	ExportL(KBadTzFile, entryArray, aIsSynch);
	
	TFileName fileName(KExportFilePath);
	fileName.Append(KBadTzFile);
	
	_LIT8(KTzMinus4, "TZ:-04");
	test(CheckStringInFileL(KTzMinus4, fileName, ETrue));
	
	_LIT8(KDaylightRuleIn2005, ";2005");
	test(CheckStringInFileL(KDaylightRuleIn2005, fileName, ETrue));

	CleanupStack::PopAndDestroy(&entryArray);
	
	}

/*
 * Tests that generated uids are unique due to underlying timer resolution issues,
 * see DEF088340 for more info
 */
void CDataExchangeTestManager::TestImportvCalsGeneratesUniqueIdL(TBool aIsSynch)
	{
	_LIT(KvCalsFile, "vcals.vcs");
	_LIT8(KvCals,
	"BEGIN:VCALENDAR   			   	\r\n"
	"PRODID:Symbian Generated        \r\n"
	"TZ:+01                          \r\n"
	"VERSION:1.0                     \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 000  \r\n"
	"DTSTART:20060701T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 111  \r\n"
	"DTSTART:20060702T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 222  \r\n"
	"DTSTART:20060703T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 333  \r\n"
	"DTSTART:20060704T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 444  \r\n"
	"DTSTART:20060705T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 555  \r\n"
	"DTSTART:20060706T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 666  \r\n"
	"DTSTART:20060707T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 777  \r\n"
	"DTSTART:20060708T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 888  \r\n"
	"DTSTART:20060709T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"BEGIN:VEVENT                    \r\n"
	"SUMMARY;CHARSET=macintosh: 999  \r\n"
	"DTSTART:20060710T230000Z        \r\n"
	"END:VEVENT                      \r\n"
	"END:VCALENDAR                   \r\n"
	);
	iTestLib->CleanDatabaseL();
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	CreateTestFileL(KvCals, KvCalsFile);
	ImportL(KvCalsFile, entries, aIsSynch);
	
	test(entries.Count() == 10 );
	
	TBuf8<128> previousUid;
	TBuf8<128> uid;
	
	for (TInt i=0; i<entries.Count(); i++)
		{
		CCalEntry* entry = entries[i];
		uid = entry->UidL();
		test(uid.Compare(previousUid)!= 0);
		previousUid = uid;
		}
	CleanupStack::PopAndDestroy(&entries);
	}

void CDataExchangeTestManager::TestRepeatEntryHasCountAndEndDateL(TBool aIsSynch)
	{//when importing, the repeating end data of the entry should take whichever comes first if both repeat end date and count are imported from versit
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
//1. Test case: repeat count comes first	
	CreateTestFileL(KRepeatEntryHasCountAndEndDate1, KFileName);
	ImportL(KFileName, entryArray, aIsSynch);
	test(entryArray.Count() == 1);	
	
	TTime expectedDateTime (TDateTime(2000, EMarch, 4, 10, 0, 0, 0));
	//This is the repeating end date based on repeating count
	
	TCalRRule repeat;
	CCalEntry* entry = entryArray[0];
	test(entry->GetRRuleL(repeat));
	test(repeat.Until().TimeLocalL()== expectedDateTime);
	entryArray.Remove(0);
	delete entry;
//2. Test case: repeat until time ccmes first	
	CreateTestFileL(KRepeatEntryHasCountAndEndDate2, KFileName);
	ImportL(KFileName, entryArray, aIsSynch);
	test(entryArray.Count() == 1);	
	
	expectedDateTime = TDateTime(2000, EJuly, 2, 10, 0, 0, 0);
	//This is the repeating end date imported
	TCalRRule repeat1;
	test(entryArray[0]->GetRRuleL(repeat1));
	test(repeat1.Until().TimeLocalL()== expectedDateTime);
	CleanupStack::PopAndDestroy(&entryArray);
	}

void CDataExchangeTestManager::TestCategoryL(TBool aIsSynch)
	{
	_LIT(KCategoryImportFile, "category_import.vcs");
	_LIT(KCategoryExportFile, "category_export.vcs");
	_LIT8(KCategory,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"			
		"BEGIN:VEVENT\r\n"
		"UID:Category_uid\r\n"
		"SUMMARY:first entry\r\n"
		"DTSTART:20050411T100000Z\r\n"
		"DTEND:20080411T120000Z\r\n"
		"CATEGORIES:EXTENDED-CAT;X-DUMMY\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	_LIT(KExtendedCategoryName1, "EXTENDED-CAT");
	_LIT8(KExtendedCategoryNameInFile, "CATEGORIES:EXTENDED-CAT;DUMMY");
	_LIT(KExtendedCategoryName2, "DUMMY");
	
	CreateTestFileL(KCategory, KCategoryImportFile);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	ImportL(KCategoryImportFile, entryArray, aIsSynch);
	test(entryArray.Count() == 1);

	const RPointerArray<CCalCategory>& KCatList = entryArray[0]->CategoryListL();
	
	test(KCatList.Count() == 2);
	test(KCatList[0]->ExtendedCategoryName() == KExtendedCategoryName1);
	test(KCatList[1]->ExtendedCategoryName() == KExtendedCategoryName2);
	
	ExportL(KCategoryExportFile, entryArray, aIsSynch);
	CheckStringInFileL(KExtendedCategoryNameInFile, KCategoryExportFile, ETrue);
	
	CleanupStack::PopAndDestroy(&entryArray);
	}

void CDataExchangeTestManager::TestImportWithInvalidRRuleDateL(TBool aIsSynch)
	{
	test.Next(_L("Importing Repeat entry with invalid date"));

	iTestLib->CleanDatabaseL();
	
	RPointerArray<CCalEntry> importedArray;
	CleanupResetAndDestroyPushL(importedArray);
	
	// Create vCal with invalid MonthlyByDayRepeat date in RRule
	// RRULE:MD1 16 17 39 #10
	CreateTestFileL(KRepeatEntryHasInvalidDate, KTestRRuleWithInvalidDateFile);
	
	// Invalid MonthlyByDayRepeat date should be ignored during import
	ImportL(KTestRRuleWithInvalidDateFile, importedArray, aIsSynch);
	test(importedArray.Count()==1);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(importedArray, success);
	CleanupStack::PopAndDestroy(&importedArray); // importedArray.ResetAndDestroy()
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	FetchAllL(instances);
	
	// Instances should be 20 since RRULE:MD1 16 17 39 #10 has one invalid date (39)
	test(instances.Count() == 20);

	CleanupStack::PopAndDestroy(&instances);
	}

void CDataExchangeTestManager::TestExDateUntilTimeL(TBool aIsSynch)
	{
	_LIT(KExportFilePath, "c:\\logs\\tcal_dataexchange\\temp\\");
	_LIT8(KTokenToFindRRule, "RRULE:D1 20050519T133000");
	_LIT8(KTokenToFindExDates, "EXDATE:20050518T123000Z;20050519T123000Z");
	_LIT8(KEntryWithTZRule, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententryanish@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050516T133000\r\n"
		"DTEND:20050516T143000\r\n"
		"RRULE:D1 20050525T143000\r\n"
		"LAST-MODIFIED:20060213T105113Z\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententryanish@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050527T133000\r\n"
		"DTEND:20050527T143000\r\n"
		"X-RECURRENCE-ID:20050519\r\n"
		"LAST-MODIFIED:20060213T105113Z\r\n"
		"RRULE:D1 20050528T143000\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententryanish@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050527T133000\r\n"
		"DTEND:20050527T143000\r\n"
		"X-RECURRENCE-ID:20050518\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	
	_LIT(KEntryWithTZRuleFile, "entry_with_exdate.vcs");
	
	iTestLib->CleanDatabaseL();
	CreateTestFileL(KEntryWithTZRule, KEntryWithTZRuleFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KEntryWithTZRuleFile, entries, aIsSynch);
	test(entries.Count() == 3);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	
	RPointerArray<CCalEntry> fetchedEntries;
	CleanupResetAndDestroyPushL(fetchedEntries);
	

	iTestLib->SynCGetEntryViewL().FetchL(entries[0]->UidL(), fetchedEntries);

	ExportL(KEntryExportFile, fetchedEntries, aIsSynch);	
		
		
	TBuf<100> fileName(KExportFilePath);
	fileName.Append(KEntryExportFile);
			
	test(CheckStringInFileL(KTokenToFindRRule, fileName, ETrue));
	test(CheckStringInFileL(KTokenToFindExDates, fileName, ETrue));

	CleanupStack::PopAndDestroy(2, &entries);//fetchedEntries
	}

void CDataExchangeTestManager::TestOOMExportL(TBool aIsSynch)
	{
	_LIT8(KEntry, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:2\r\n"
		"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
		"DTSTART:19991101T100000\r\n"
		"DTEND:19991101T113000\r\n"
		"AALARM:19991101T090000\r\n"
		"LAST-MODIFIED:19991101T124400\r\n"
		"X-SYMBIAN-DTSTAMP:18990901T130000\r\n"
		"COMPLETED:19991101T080000\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	
	_LIT(KEntryFile, "entry_with_exdate.vcs");
	
	iTestLib->CleanDatabaseL();
	CreateTestFileL(KEntry, KEntryFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KEntryFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	
	RPointerArray<CCalEntry> fetchedEntries;
	CleanupResetAndDestroyPushL(fetchedEntries);
	

	iTestLib->SynCGetEntryViewL().FetchL(entries[0]->UidL(), fetchedEntries);

	TInt tryCount = 1;
	TInt err = 0;
	
	// OOM LOOP
	for ( ;; )
		{
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		
		TRAP(err, ExportL(KEntryExportFile, fetchedEntries, aIsSynch));
	
		if ( err==KErrNone ) 
			{
			__UHEAP_RESET;
			RDebug::Printf("Memory allocation testing for Export is done");
			break;
			}
		test(err == KErrNoMemory);
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		++tryCount;
		}
	// OOM LOOP
	CleanupStack::PopAndDestroy(2, &entries);//fetchedEntries
	}

/**
* Attempts to import a child entry that replaces the entire parent entry. This
* is invalid. When the entry is stored, it should leave with KErrArgument.
*/
void CDataExchangeTestManager::ImportInvalidChildEntryL(TBool aIsSynch)
    {

	test.Printf(_L("Start import invalid repeating child entry test - ImportInvalidChildEntry..."));    
    
    //
    // 1. Invalid child repeat range tests //
    //

    // Test if child entry time range is exactly the same as parent entry, but RecID
    // is valid
    // Parent: May 16 - 28 Daily 13:30 - 14:30
    // Child:  May 16 - 29 + RecID May 18 13:30
    TestImportParentAndChildEntryL(KImportInvalidChildEntryTestParent, 
                                   KChildEntryMoreThanOverlapParentWithRDate, 
                                   1,
                                   aIsSynch
                                   );

    // Test if child entry time range wholly embeds parent entry's, but RecID
    // is valid
    // Parent: May 16 - 28 Daily 13:30 - 14:30
    // Child:  May 1 - Jun 1 + RecID May 18 13:30   
    TestImportParentAndChildEntryL(KImportInvalidChildEntryTestParent, 
                                   KChildEntryWhollyContainsParent, 
                                   1,
                                   aIsSynch
                                   );

      //
    // 2. Valid child repeat range tests //
    //

    // Test if child entry time range is within parent entry's, but RecID
    // is out of parent's range
    //
    // Parent: May 16 - 28 Daily 13:30 - 14:30
    // Child:  May 20 - 25 + RecID May 1 13:30
    TestImportParentAndChildEntryL(KImportInvalidChildEntryTestParent, 
                                   KChildEntryRecIdBeforeParentStart, 
                                   1,
                                   aIsSynch
                                   );    

    // Test if child entry time range is within parent entry's, but RecID
    // is out of parent's range
    // Parent: May 16 - 28 Daily 13:30 - 14:30
    // Child:  May 20 - 25 + RecID Jun 1 13:30
    TestImportParentAndChildEntryL(KImportInvalidChildEntryTestParent, 
                                   KChildEntryRecIdAfterParentEnd, 
                                   1,
                                   aIsSynch);

    // Test valid child entry time range that overlaps parent entry's start, 
    // with RecID out of range
    // Parent: May 16 - 28 Daily 13:30 - 14:30
    // Child:  May 10 - 20 + RecID May 12 13:30
    TestImportParentAndChildEntryL(KImportInvalidChildEntryTestParent, 
                                   KChildEntryPartialOverlapParentHeadInvalidRecID, 
                                   1,
                                   aIsSynch
                                   );    

    // Test valid child entry time range that overlaps parent entry's end,
    // with RecID out of range
    // Parent: May 16 - 28 Daily 13:30 - 14:30
    // Child:  May 20 - Jun 10 + RecID May 30 13:30
    TestImportParentAndChildEntryL(KImportInvalidChildEntryTestParent, 
                                   KChildEntryPartialOverlapParentTailInvalidRecID, 
                                   1, 
                                   aIsSynch
                                   );

    // Test valid child entry time range that is within parent entry's, but RecID
    // is not a valid parent instance 
    // Parent: May 16 - 28 Daily 13:30 - 14:30
    // Child:  May 20 - Jun 10 + RecID May 29 00:00
    TestImportParentAndChildEntryL(KImportInvalidChildEntryTestParent, 
                                   KChildEntryRecIdInvalidDateOnly, 
                                   1,
                                   aIsSynch
                                   );
 

    // Test valid child entry time range that is within parent entry's, but RecID
    // is not a valid parent instance 
    // Parent: May 16 - 28 Daily 13:30 - 14:30, RDATE May 30 00:00
    // Child:  May 20 - Jun 10 + RecID May 29 00:00
    TestImportParentAndChildEntryL(KImportInvalidChildEntryTestParentWithRDate, 
                                   KChildEntryRecIdInvalidDateOnly, 
                                   1,
                                   aIsSynch
                                   );
                                   
	test.Printf(_L("Completed import invalid repeating child entry test - ImportInvalidChildEntry..."));    
    }
 

void CDataExchangeTestManager::TestImportParentAndChildEntryL(
    const TDesC8& aParentEntry,
    const TDesC8& aChildEntry,
    const TInt aExpectedNumOfEntriesStored,
    TBool aIsSynch)
    {
    // flush all existing entries
    iTestLib->CleanDatabaseL();
 
     // Write the test entries to a file.
 	RFile file;
 	iTestLib->TestRegister().CreateTempFileLC( file, KTestExportFile );

	User::LeaveIfError( file.Write(aParentEntry) );
    User::LeaveIfError( file.Write(aChildEntry) );
 	CleanupStack::PopAndDestroy( &file ); // file.Close()
 
     // Import the entries from file.
	RPointerArray<CCalEntry> importedEntries;
	CleanupResetAndDestroyPushL( importedEntries );

    ImportL( KTestExportFile, importedEntries, aIsSynch);
 
     // Store the entries. Only the parent entry should be stored.
 	TInt numEntriesStored = 0;

	TRAPD( err, iTestLib->SynCGetEntryViewL().StoreL( importedEntries, numEntriesStored ) );	
 	test(err != KErrNone);

	test( numEntriesStored == aExpectedNumOfEntriesStored );
   
	CleanupStack::PopAndDestroy();		//	importedEntries
    }
    
void CDataExchangeTestManager::EmptyAttachmentL(TBool aIsSynch)
	{
	_LIT(KEmptyAttachmentFile, "empty_attach.vcs");
	_LIT8(KEmptyAttachment,
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VEVENT\r\n"
		"DTSTART:20000221T070000\r\n"
		"DTEND:20000221T071500\r\n"
		"SUMMARY:TEST ATTACHMENT\r\n"
		"UID:AttachTest\r\n"
		"RRULE:D1 20000225T070000\r\n"
		"ATTACH;;jk;\r\n"
		"ATTACH;yuyt\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:AttachTest1\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20000221T070000\r\n"
		"DTEND:20000221T071500\r\n"
		"RRULE:D1 20000225T070000\r\n"
		"SUMMARY:TEST ATTACHMENT\r\n"
		"ATTACH;VALUE=CID:\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);

	CreateTestFileL(KEmptyAttachment, KEmptyAttachmentFile);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
    ImportL(KEmptyAttachmentFile, entryArray, aIsSynch);
    
    // check two entries imported, neither of which have any attachments
    test(entryArray.Count() == 2);
    test(entryArray[0]->AttachmentCountL() == 0);
    test(entryArray[1]->AttachmentCountL() == 0);
    
	CleanupStack::PopAndDestroy(&entryArray);
	}

void CDataExchangeTestManager::TestGEOImportAndExportL(TBool aIsSynch)
	{
	// Setup vcals for import and export
	// Bounds checking
	// In bounds
	_LIT8(KGEOInBoundsPositiveImportTestVCal,
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123001\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:50.123456,95.123456\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

	_LIT8(KGEOInBoundsNegativeImportTestVCal, "BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123002\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:-50.123456,-95.123456\r\n"				
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
	
	// On bounds
	_LIT8(KGEOOnBoundsPositiveImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123003\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:90.000000,180.000000\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

	_LIT8(KGEOOnBoundsNegativeImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123004\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:-90.000000,-180.000000\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
	
	// Zero valid data
	_LIT8(KGEOValidZeroImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:1230015\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:0.000000,0.000000\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);		
	
	// Out of bounds
	_LIT8(KGEOOutOfBoundsPositiveImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123006\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:90.123456,180.123456\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

	_LIT8(KGEOOutOfBoundsNegativeImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123007\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:-90.123456,-180.123456\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

	// Truncation and rounding up
	_LIT8(KGEOTruncationRoundingUpPositiveImportTestVCal,
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123008\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:45.123456723,95.123456723\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

	_LIT8(KGEOTruncationRoundingUpNegativeImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123009\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:-45.123456723,-95.123456723\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);
	
	// Truncation and rounding  down
	_LIT8(KGEOTruncationRoundingDownPositiveImportTestVCal,
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123010\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:45.123456123,95.123456123\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

	_LIT8(KGEOTruncationRoundingDownNegativeImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123011\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:-45.123456123,-95.123456123\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KGEONoDecimalPositiveImportTestVCal, 
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123012\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:50,95\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KGEONoDecimalNegativeImportTestVCal, 
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123013\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:-50,-95\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KGEOFewDecimalsPositiveImportTestVCal, 
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123014\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:50.123,95.123\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

_LIT8(KGEOFewDecimalsNegativeImportTestVCal, 
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123015\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:-50.123,-95.123\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

	// Empty data
	_LIT8(KGEOInvalidEmptyValuesImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123016\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);	

	// Invalid data - characters
	_LIT8(KGEOInvalidDataCharsImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123017\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:abc.defghi,j.klmnop\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);	

	// Invalid data - alpha numeric
	_LIT8(KGEOInvalidDataAlphaNumImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123018\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:10.defghi,j.123456\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);	

	// Invalid GEO value separator - ';' instead of ','
	_LIT8(KGEOInvalidSeparatorImportTestVCal,	
				"BEGIN:VCALENDAR\r\n"
				 "TZ:+03\r\n"
				 "VERSION:1.0\r\n"			
				"BEGIN:VEVENT\r\n"
		 		 "UID:123019\r\n"
		 		 "X-SYMBIAN-DTSTAMP:20050411T100000Z\r\n"
				 "SEQUENCE:5\r\n"
			   	 "SUMMARY:first entry\r\n"
				 "DESCRIPTION:a description\r\n"
				 "DTSTART:20050411T100000Z\r\n"
				 "DTEND:20080411T120000Z\r\n"
				 "CATEGORIES:APPOINTMENT\r\n"
 				 "RRULE:MP1 #100\r\n"
				 "GEO:45.123456;95.123456\r\n"		
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				);

	// Create all the files
	// Import files
	RPointerArray<HBufC8> bufArray;
	CleanupResetAndDestroyPushL(bufArray);
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	bufArray.AppendL(KGEOInBoundsPositiveImportTestVCal().AllocL());
	bufArray.AppendL(KGEOInBoundsNegativeImportTestVCal().AllocL());

	bufArray.AppendL(KGEOOnBoundsPositiveImportTestVCal().AllocL());
	bufArray.AppendL(KGEOOnBoundsNegativeImportTestVCal().AllocL());

	bufArray.AppendL(KGEOValidZeroImportTestVCal().AllocL());

	bufArray.AppendL(KGEOOutOfBoundsPositiveImportTestVCal().AllocL());
	bufArray.AppendL(KGEOOutOfBoundsNegativeImportTestVCal().AllocL());

	bufArray.AppendL(KGEOTruncationRoundingUpPositiveImportTestVCal().AllocL());
	bufArray.AppendL(KGEOTruncationRoundingUpNegativeImportTestVCal().AllocL());

	bufArray.AppendL(KGEOTruncationRoundingDownPositiveImportTestVCal().AllocL());
	bufArray.AppendL(KGEOTruncationRoundingDownNegativeImportTestVCal().AllocL());
	
	bufArray.AppendL(KGEONoDecimalPositiveImportTestVCal().AllocL());
	bufArray.AppendL(KGEONoDecimalNegativeImportTestVCal().AllocL());

	bufArray.AppendL(KGEOFewDecimalsPositiveImportTestVCal().AllocL());
	bufArray.AppendL(KGEOFewDecimalsNegativeImportTestVCal().AllocL());

	// Invalid data
	bufArray.AppendL(KGEOInvalidEmptyValuesImportTestVCal().AllocL());
	bufArray.AppendL(KGEOInvalidDataCharsImportTestVCal().AllocL());
	bufArray.AppendL(KGEOInvalidDataAlphaNumImportTestVCal().AllocL());
	bufArray.AppendL(KGEOInvalidSeparatorImportTestVCal().AllocL());

	// Import
	TestGEOImportL(bufArray, entryArray, aIsSynch);
	
	// Export		
	TestGEOExportL(entryArray, aIsSynch);
	
	// Export, Import and Compare entries
	ExportImportAndCheckL(entryArray, aIsSynch);
	
	CleanupStack::PopAndDestroy(&entryArray);
	CleanupStack::PopAndDestroy(&bufArray);
	}

void CDataExchangeTestManager::TestGEOImportL(RPointerArray<HBufC8>& aBufArray, RPointerArray<CCalEntry>& aEntryArray, TBool aIsSynch)
	{	
	TBuf<64> importFileName;	
	for (TInt i = 0; i < aBufArray.Count(); ++i)
		{	
		importFileName.Format(KEntryImportFile(), i);
	
		test.Next(_L("Writing test entry data to vcs file\n"));
				
		CreateTestFileL(*aBufArray[i], importFileName);
		ImportL(importFileName, aEntryArray, aIsSynch);
		}

	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(aEntryArray, success);
	test(aEntryArray.Count() == success);
	
	// test the imported entries!
	CCalEntry* importedEntry = NULL;
	
	for(TInt i = 0; i < aEntryArray.Count(); ++i)
		{
		importedEntry = aEntryArray[i];

		TPtrC8 uidValue(importedEntry->UidL());
		
		if(uidValue == KGEOInBoundsPositiveImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(50.123456,95.123456,*importedEntry));
			}
			
		else if(uidValue == KGEOInBoundsNegativeImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(-50.123456,-95.123456,*importedEntry));
			}
		
		else if(uidValue == KGEOOnBoundsPositiveImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(90.000000,180.000000,*importedEntry));
			}
			
		else if(uidValue == KGEOOnBoundsNegativeImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(-90.000000,-180.000000,*importedEntry));
			}
			
		else if(uidValue == KGEOValidZeroImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(0.000000,0.000000,*importedEntry));
			}
			
		else if(uidValue == KGEOOutOfBoundsPositiveImportTestVCalUID)
			{
			test(!TestGEOVerifyImportL(90.123456,180.123456,*importedEntry));
			}
		
		else if(uidValue == KGEOOutOfBoundsNegativeImportTestVCalUID)
			{
			test(!TestGEOVerifyImportL(-90.123456,-180.123456,*importedEntry));
			}
			
		else if(uidValue == KGEOTruncationRoundingUpPositiveImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(45.123457,95.123457,*importedEntry));
			}
			
		else if(uidValue == KGEOTruncationRoundingUpNegativeImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(-45.123457,-95.123457,*importedEntry));
			}
			
		else if(uidValue == KGEOTruncationRoundingDownPositiveImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(45.123456,95.123456,*importedEntry));
			}
			
		else if(uidValue == KGEOTruncationRoundingDownNegativeImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(-45.123456,-95.123456,*importedEntry));
			}

		else if(uidValue == KGEONoDecimalPositiveImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(50,95,*importedEntry));
			}

		else if(uidValue == KGEONoDecimalNegativeImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(-50,-95,*importedEntry));
			}

		else if(uidValue == KGEOFewDecimalsPositiveImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(50.123,95.123,*importedEntry));
			}

		else if(uidValue == KGEOFewDecimalsNegativeImportTestVCalUID)
			{
			test(TestGEOVerifyImportL(-50.123,-95.123,*importedEntry));
			}

		else if(uidValue == KGEOInvalidEmptyValuesImportTestVCalUID)
			{
			test(!TestGEOVerifyImportL(0,0,*importedEntry));
			}

		else if(uidValue == KGEOInvalidDataCharsImportTestVCalUID)
			{
			test(!TestGEOVerifyImportL(0,0,*importedEntry));
			}

		else if(uidValue == KGEOInvalidDataAlphaNumImportTestVCalUID)
			{
			test(!TestGEOVerifyImportL(0,0,*importedEntry));
			}
			
		else if(uidValue == KGEOInvalidSeparatorImportTestVCalUID)
			{
			test(!TestGEOVerifyImportL(0,0,*importedEntry));
			}		
		}

	}
	
TBool CDataExchangeTestManager::TestGEOVerifyImportL(const TReal& aExpLat, const TReal& aExpLong, CCalEntry& aEntry)
	{
	CCalGeoValue* geoValue = aEntry.GeoValueL();
	
	if(geoValue)
		{
		CleanupStack::PushL(geoValue);
		TReal getLat;
		TReal getLong;
		test(geoValue->GetLatLong(getLat, getLong));
		test(getLat==aExpLat && getLong==aExpLong);
		CleanupStack::PopAndDestroy(geoValue);
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

void CDataExchangeTestManager::TestGEOExportL(RPointerArray<CCalEntry>& aEntryArray, TBool aIsSynch)
	{
	RPointerArray<CCalEntry> extractedEntryArray;
	CleanupClosePushL(extractedEntryArray);
	
	TBuf<64> exportFileName;
	for(TInt i=0; i<aEntryArray.Count();++i)
		{
		exportFileName.Format(KEntryExportFile(), i);
	
		extractedEntryArray.Reset();
		extractedEntryArray.AppendL(aEntryArray[i]);
		ExportL(exportFileName, extractedEntryArray, aIsSynch);
		TestGEOVerifyExportL(exportFileName, *aEntryArray[i]);
		}
	CleanupStack::PopAndDestroy(&extractedEntryArray);
	}

TBool CDataExchangeTestManager::TestGEOVerifyExportL(const TDesC &aFileName, CCalEntry& aEntry)
	{
	CCalGeoValue* geoValue = aEntry.GeoValueL();
	
	if(geoValue)
		{
		CleanupStack::PushL(geoValue);	
		TReal geoLatitude;
		TReal geoLongitude;
		test(geoValue->GetLatLong(geoLatitude, geoLongitude));
		
		const TUint KGEOMaxWidthOfGeoValue = 11;
		
		// Convert the geo values from numbers to string
		TBuf8<KGEOMaxWidthOfGeoValue*2+1> geoString;
		TBuf8<KGEOMaxWidthOfGeoValue> geoLatString;
		TBuf8<KGEOMaxWidthOfGeoValue> geoLongString;
		
		// Maximum width of a GEO value and max number of decimal places
		TRealFormat geoFormat(KGEOMaxWidthOfGeoValue,KCalGEOMaxDecimalPlaces);
		
		_LIT8(KGeoStringFormat,"%S%c%S");
		
		// Ensure correct conversion from stored numeric values to descriptors
		if((geoLatString.Num(geoLatitude,geoFormat)>0) && (geoLongString.Num(geoLongitude,geoFormat)>0))
			{
			geoString.AppendFormat(KGeoStringFormat,&geoLatString,KVersitTokenCommaVal,&geoLongString);
			
			TBuf<100> fileName(KExportFilePath);
			fileName.Append(aFileName);
					
			test(CheckStringInFileL(geoString,fileName, ETrue));
			CleanupStack::PopAndDestroy(geoValue);
			return ETrue;
			}
		 CleanupStack::PopAndDestroy(geoValue);
		 return EFalse;
		}
	else
		{
		return EFalse;
		}
	}
	
void CDataExchangeTestManager::TestMP1RRuleL(TBool aIsSynch)
	{
	_LIT(KExportFilePath, "c:\\logs\\tcal_dataexchange\\temp\\");
	_LIT8(KTokenToFindRRule, "RRULE:MP1 3+ MO 20060220T205100");
	_LIT8(KEntryWithTZRule, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:-0351\r\n"
		"DAYLIGHT:TRUE;-03;20050904T020000;20060416T020000;;\r\n"
		"BEGIN:VEVENT\r\n"
		"UID;ENCODING=QUOTED-PRINTABLE:todoparententryanish=example.com\r\n"
		"DTSTART:20050516T205100\r\n"
		"DTEND:20050516T215100\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"CLASS:PUBLIC\r\n"
		"SEQUENCE:0\r\n"
		"X-METHOD:NONE\r\n"
		"RRULE:MP1 #10\r\n"
		"LAST-MODIFIED:20060213T105113Z\r\n"
		"PRIORITY:0\r\n"
		"X-SYMBIAN-LUID:2\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	
	_LIT(KImportFile, "import_testmp1rrule.vcs");
	_LIT(KExportFile, "export_testmp1rrule.vcs");
	iTestLib->CleanDatabaseL();
	CreateTestFileL(KEntryWithTZRule, KImportFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KImportFile, entries, aIsSynch);
	test(entries.Count() == 1);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	
	RPointerArray<CCalEntry> fetchedEntries;
	CleanupResetAndDestroyPushL(fetchedEntries);
	
	iTestLib->SynCGetEntryViewL().FetchL(entries[0]->UidL(), fetchedEntries);

	ExportL(KExportFile, fetchedEntries, aIsSynch);	
		
	TBuf<100> fileName(KExportFilePath);
	fileName.Append(KExportFile);
			
	test(CheckStringInFileL(KTokenToFindRRule, fileName, ETrue));

	CleanupStack::PopAndDestroy(2, &entries);//fetchedEntries
	}

void CDataExchangeTestManager::TestLostRangeL(TBool aIsSynch)
	{
	const TTime KParentStartTime(TDateTime(2006, EJanuary, 0, 12, 0, 0, 0)); // 1200 on  1 Jan 06
	const TTime KParentUntilTime(TDateTime(2006, EJanuary, 10, 12, 0, 0, 0)); // 1200 on 11 Jan 06
	const TTime KRecurrenceIdTime(TDateTime(2006, EJanuary, 5, 12, 0, 0, 0)); // 1200 on  6 Jan 06
	const TTime KChildStartTime(TDateTime(2006, EJanuary, 5, 16, 0, 0, 0)); // 1200 on  6 Jan 06
	const TTime KChildUntilTime(KParentUntilTime);

	_LIT8(KGuid, "lost-range-guid1");
	_LIT(KExportFile, "lostRange.vcs");

	// create repeating parent
	HBufC8* guid = KGuid().AllocLC();
	CCalEntry* parent = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(parent);

	TCalTime parentStartTime, parentUntilTime;
	parentStartTime.SetTimeLocalL(KParentStartTime);
	parentUntilTime.SetTimeLocalL(KParentUntilTime);
	parent->SetStartAndEndTimeL(parentStartTime, parentStartTime);

	TCalRRule parentRule(TCalRRule::EDaily);
	parentRule.SetDtStart(parentStartTime);
	parentRule.SetUntil(parentUntilTime);
	parentRule.SetInterval(1);
	parent->SetRRuleL(parentRule);

	iTestLib->StoreEntryL(*parent);
	CleanupStack::PopAndDestroy(parent);

	// create repeating child entry with unexpected recurrence range
	TCalTime recId;
	recId.SetTimeLocalL(KRecurrenceIdTime);

	guid = KGuid().AllocLC();
	CCalEntry* child = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0,
	recId, CalCommon::EThisAndPrior);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(child);

	TCalTime childStartTime, childUntilTime;
	childStartTime.SetTimeLocalL(KChildStartTime);
	childUntilTime.SetTimeLocalL(KChildUntilTime);
	child->SetStartAndEndTimeL(childStartTime, childUntilTime);

	TCalRRule childRule(TCalRRule::EDaily);
	childRule.SetDtStart(childStartTime);
	childRule.SetUntil(childUntilTime);
	childRule.SetInterval(1);
	child->SetRRuleL(childRule);

	iTestLib->StoreEntryL(*child);
	CleanupStack::PopAndDestroy(child);

	// fetch and export entries, then reimport. the recurrence range is lost so must be recalculated
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);

	iTestLib->SynCGetEntryViewL().FetchL(KGuid(), entries);
	test(entries.Count() == 2);

	ExportL(KExportFile, entries, aIsSynch);
	entries.ResetAndDestroy();
	ImportL(KExportFile, entries, aIsSynch);

	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == 2);

	CleanupStack::PopAndDestroy(&entries);
	}
	
void CDataExchangeTestManager::TestChildAddedToFirstInstanceL(TBool aIsSynch)
	{
	_LIT(KExceptionOnRDatesFile, "entry_with_parentrdaterepeat.vcs");
	_LIT8(KEntryKExceptionOnRDates, 
			"BEGIN:VCALENDAR\r\n"
			"VERSION:1.0\r\n"
			"TZ:-06\r\n"
			"DAYLIGHT:TRUE;-05;20060402T020000;20061029T020000;;\r\n"
			"BEGIN:VEVENT\r\n"
			"UID:UA03i59J4EDde240cE60M3\r\n"
			"SUMMARY:Meeting 1\r\n"
			"DTSTART:20061025T160000Z\r\n"
			"DTEND:20061025T170000Z\r\n"
			"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
			"CLASS:PUBLIC\r\n"
			"LOCATION:Dallas\r\n"
			"SEQUENCE:0\r\n"
			"X-METHOD:NONE\r\n"
			"RDATE:20061025T160000Z;20061027T160000Z;20061028T160000Z;\r\n"
			"  20061030T160000Z\r\n"
			"EXDATE:20061027T160000Z\r\n"
			"LAST-MODIFIED:20061011T163551Z\r\n"
			"PRIORITY:0\r\n"
			"X-SYMBIAN-LUID:71\r\n"
			"END:VEVENT\r\n"
			"END:VCALENDAR\r\n"	);

	CalCommon::TCalViewFilter filter = CalCommon::EIncludeAll;
	TCalTime startTime;
	startTime.SetTimeUtcL(TCalTime::MinTime());
	TCalTime endTime;
	endTime.SetTimeUtcL(TCalTime::MaxTime());
	CalCommon::TCalTimeRange timeRange(startTime, endTime);

	iTestLib->CleanDatabaseL();

	// Create and import the vcal file, and store the entries.
	CreateTestFileL(KEntryKExceptionOnRDates, KExceptionOnRDatesFile);
	RPointerArray<CCalEntry> entriesArray;
	CleanupResetAndDestroyPushL(entriesArray);
	ImportL(KExceptionOnRDatesFile, entriesArray, aIsSynch);
	
	HBufC8* uId = entriesArray[0]->UidL().AllocLC();
	TInt success(0);
	iTestLib->SynCGetEntryViewL().StoreL(entriesArray, success);
	CleanupStack::Pop(uId);
	CleanupStack::PopAndDestroy();
	CleanupStack::PushL(uId);
	test.Printf(_L("vCal imported\n"));

	// Verify that there are 4 instances initially.
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, filter, timeRange);
	TInt count = instances.Count(); 
	
	test(count == 3);
	
	// Print the details of the initial instances.
	test.Printf(_L("instances found are -- %d\n"),count);
	TInt i;
	for(i = 0 ; i < count; ++i)
		{
		test.Printf(_L("Entry summary referenced by this instance -- "));   	
		test.Printf(instances[i]->Entry().SummaryL());
		TDateTime dt = instances[i]->StartTimeL().TimeLocalL().DateTime();
		test.Printf(_L("Instance start date in local  %d/%d/%d, %d:%d:%d\n"),	dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
		}
	CleanupStack::PopAndDestroy( &instances );

	// Create a child entry for one of the instances.
	TCalTime recurrenceID;
	recurrenceID.SetTimeUtcL(TDateTime(2006, EOctober, 24, 16, 0, 0, 0));
	CCalEntry* child = CCalEntry::NewL(CCalEntry::EAppt, uId, CCalEntry::EMethodNone, 1, recurrenceID, CalCommon::EThisOnly);
	CleanupStack::Pop(uId);
	CleanupStack::PushL(child);
	_LIT(KSummary,"Child Summary");
	TCalTime childstrt;
	TCalTime childend;
	childstrt.SetTimeUtcL(TDateTime(2006, EOctober, 24, 11, 0, 0, 0));
	childend.SetTimeUtcL(TDateTime(2006, EOctober, 24, 12, 0, 0, 0));
	child->SetSummaryL(KSummary());
	child->SetStartAndEndTimeL(childstrt, childend);

	// Store the child entry.
	CleanupResetAndDestroyPushL(entriesArray);
	entriesArray.AppendL(child);
	iTestLib->SynCGetEntryViewL().StoreL(entriesArray, success);
	CleanupStack::PopAndDestroy();
	CleanupStack::Pop();
	test.Printf(_L("Added a non-repeating child on 1rd instance i.e 25th October\n"));
	CleanupResetAndDestroyPushL(instances);

	// Fetch all the instances again, and verify that no additional instance has been created.
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, filter, timeRange);
	count = instances.Count(); 
	test(count == 3);
	test.Printf(_L("instances found are -- %d\n"),count);
	for( i = 0 ; i < count; ++i)
		{
		test.Printf(_L("Entry summary referenced by this instance -- "));
		test.Printf(instances[i]->Entry().SummaryL());
		TDateTime dt = instances[i]->StartTimeL().TimeLocalL().DateTime();
		test.Printf(_L("Instance start date in local  %d/%d/%d, %d:%d:%d\n"),	dt.Day()+1, dt.Month()+1, dt.Year(), dt.Hour(), dt.Minute(), dt.Second());
		}
	CleanupStack::PopAndDestroy( &instances );	
	}
/**
@SYMTestCaseID DEF099686
@SYMTestType UT
@SYMTestPriority Normal
@SYMTestCaseDesc entry without a RRule and some sporadic dates exports incorrect alarm time .

@SYMTestActions
1.Import and store an entry without Rrule but with Rdates into the calender
2.Export the entry.
3.Check the alarm time..

@SYMTestExpectedResults For the above tests:
1.Alarmtime should not change while exporting.
*/

void CDataExchangeTestManager::TestEntryAlarmTimeL(TBool aIsSynch)
	{
	_LIT8(KVcalEntry, "BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:auid\r\n"
					"AALARM:20000221T060000\r\n"
					"DTSTART:20000221T070000\r\n"
					"DTEND:20000221T071500\r\n"
					"RDATE:20000402T010000;20000403T010000;20000403T020000\r\n"
					"END:VEVENT\r\n"
					"END:VCALENDAR\r\n"
					);

	_LIT(KVcalFilename, "vcal.vcs");
	_LIT(KVcalOutFilename, "vcalout.vcs");
	_LIT8(KUID, "auid");
	
	iTestLib->CleanDatabaseL();		
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	TInt numEntries=0;
	
	//import the entries from the  vcs file
	CreateTestFileL(KVcalEntry, KVcalFilename);
	ImportL(KVcalFilename, entryArray, aIsSynch);
	test(entryArray.Count() == 1);	
	
	//store the entries 
	iTestLib->SynCGetEntryViewL().StoreL(entryArray, numEntries);
	entryArray.ResetAndDestroy();

	//Fetch the entry and then export it
	iTestLib->SynCGetEntryViewL().FetchL(KUID, entryArray);	
	ExportL(KVcalOutFilename, entryArray, aIsSynch);
	CleanupStack::PopAndDestroy(&entryArray);
	
	//compare the alarm time
	test(CheckStringInFileL(_L8("20000221T060000"), _L("c:\\logs\\tcal_dataexchange\\temp\\vcalout.vcs"), ETrue));
	test.Printf(_L("TestEntryAlarmTime: Successful\n"));
	}
void CDataExchangeTestManager::TestRDATEOOMExportL(TBool aIsSynch)
	{
	_LIT8(KEntry, 
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VTODO\r\n"
		"UID;ENCODING=QUOTED-PRINTABLE:20061211T162705-techview_agenda=example.com\r\n"
		"SUMMARY:summary todo\r\n"
		"DESCRIPTION:des todo\r\n"
		"DUE:20061211T120000Z\r\n"
		"X-EPOCTODOLIST:TODO\r\n"
		"STATUS:NEEDS ACTION\r\n"
		"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
		"CLASS:PUBLIC\r\n"
		"LOCATION:location todo\r\n"
		"SEQUENCE:0\r\n"
		"X-METHOD:NONE\r\n"
		"ATTENDEE;ROLE=ORGANIZER;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI;X-PHONEOWNER;ENCODING=QUOTED-PRINTABLE:=\r\n"
		"techview_agenda=40symbian.com\r\n"
		"ATTENDEE;X-SENTBY=l;ROLE=ATTENDEE;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI;X-ROLE=REQ-PARTICIPANT;X-STATUS=NEEDS-ACTION;X-CN=l:l\r\n"
		"RRULE:MD2 11+ 20070211T070000\r\n"
		"RDATE:20071211T162800Z;20081211T162800Z\r\n"
		"AALARM;TYPE=X-EPOCSOUND:20061211T114500Z;;;Rings\r\n"
		"LAST-MODIFIED:20061211T162823Z\r\n"
		"CATEGORIES:MEETING;X-extended\r\n"
		"PRIORITY:1\r\n"
		"X-SYMBIAN-LUID:3\r\n"
		"END:VTODO\r\n"
		"END:VCALENDAR\r\n"
		"\r\n"
		"BEGIN:VCALENDAR\r\n"
		"VERSION:1.0\r\n"
		"TZ:+01\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententry@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050516T133000\r\n"
		"DTEND:20050516T143000\r\n"
		"RRULE:D1 20050525T133000\r\n"
		"LAST-MODIFIED:20060213T105113\r\n"
		"END:VEVENT\r\n"
		"\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententry@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050527T133000\r\n"
		"DTEND:20050527T143000\r\n"
		"X-RECURRENCE-ID:20050517T133000\r\n"
		"AALARM:20050527T123000\r\n"
		"X-SYMBIAN-DTSTAMP:20050901T130000\r\n"
		"RRULE:D1 20050610T133000\r\n"
		"RDATE:20050611T153000;20050612T090000\r\n"
		"EXDATE:20050528T133000;20050529T133000\r\n"
		"LAST-MODIFIED:20060213T105113\r\n"
		"END:VEVENT\r\n"
		"\r\n"
		"BEGIN:VEVENT\r\n"
		"UID:todoparententry@example.com\r\n"
		"X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
		"DTSTART:20050614T143000\r\n"
		"DTEND:20050614T153000\r\n"
		"X-RECURRENCE-ID:20050516T133000\r\n"
		"X-SYMBIAN-DTSTAMP:20050901T130000\r\n"
		"LAST-MODIFIED:20060213T105113\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		);
	
	_LIT(KEntryFile, "entry_with_rdate.vcs");
	
	test.Printf(_L("\n\nTestRDATEOOMExportL: Starts\n"));
	iTestLib->CleanDatabaseL();
	CreateTestFileL(KEntry, KEntryFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KEntryFile, entries, aIsSynch);
	test(4 == entries.Count());
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == entries.Count());
	
	RPointerArray<CCalEntry> fetchedEntries;
	CleanupResetAndDestroyPushL(fetchedEntries);
	
	_LIT8(KUid1, "20061211T162705-techview_agenda@example.com");
	_LIT8(KUid2, "todoparententry@example.com");
	
	iTestLib->SynCGetEntryViewL().FetchL(KUid1, fetchedEntries);
	iTestLib->SynCGetEntryViewL().FetchL(KUid2, fetchedEntries);
	test(4 == fetchedEntries.Count());

	TInt tryCount = 1;
	TInt err = 0;
	TBuf<64> exportFileName;
	exportFileName.Format(KEntryExportFile(), 131); // Test no. of this function in this file.
	
	// OOM LOOP
	for ( ;; )
		{
		RDebug::Printf("Try count: %d", tryCount);
		__UHEAP_SETFAIL(RHeap::EFailNext, tryCount);
		
		TRAP(err, ExportL(exportFileName, fetchedEntries, aIsSynch));
		
		if( err==KErrNone ) 
		{
			__UHEAP_RESET;
			RDebug::Printf("Memory allocation testing for Export is done");
			break;
		}
		
		test(err == KErrNoMemory);
		__UHEAP_SETFAIL(RHeap::ENone, 0);		
		tryCount++;
		__UHEAP_RESET;
		break;
		}
	// OOM LOOP
	
	CleanupStack::PopAndDestroy(2, &entries);//fetchedEntries
	
	// check whether fields are present 
	_LIT8(KTODORDATEFieldToken, "RDATE:20071211T162800Z;20081211T162800Z");
	
	_LIT(KExportFilePath, "c:\\logs\\tcal_dataexchange\\temp\\");
	TBuf<100> fileName(KExportFilePath);
	fileName.Append(exportFileName);
	
	test(CheckStringInFileL(KTODORDATEFieldToken, fileName, ETrue));
	
	CleanupResetAndDestroyPushL(fetchedEntries);

	ImportL(exportFileName, fetchedEntries, aIsSynch);
	test(4 == fetchedEntries.Count());
	
	CleanupStack::PopAndDestroy(&fetchedEntries);
	
	test.Printf(_L("TestRDATEOOMExportL: Successful\n"));
	}
	
/**
 @SYMTestCaseID DEF099683
 @SYMTestType UT
 @SYMTestPriority NORMAL
 @SYMDEF DEF099683
 @SYMTestCaseDesc Tests multiple vCals import under OOM conditions
 
 @SYMTestActions
 1) Import multiple vCals under OOM conditions.
 2) Test that the multiple vCals are imported.

 @SYMTestExpectedResults multiple vCals are imported successfully under OOM conditions.
*/ 	
void CDataExchangeTestManager::TestOOMImportL(TBool aIsSynch)
	{
#ifdef _DEBUG // oom testing only works in debug builds
	_LIT8(KEntry,
				"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"BEGIN:VTODO\r\n"
				"UID;ENCODING=QUOTED-PRINTABLE:20011208T162426-techview_agenda=example.com\r\n"
				"SUMMARY:todo test\r\n"
				"DESCRIPTION:todo description\r\n"
				"DUE:20011204T000000Z\r\n"
				"X-EPOCTODOLIST:TODO\r\n"
				"COMPLETED:20011208T162548Z\r\n"
				"STATUS:COMPLETED\r\n"
				"X-EPOCAGENDAENTRYTYPE:TODO\r\n"
				"CLASS:PUBLIC\r\n"
				"LOCATION:todo location\r\n"
				"SEQUENCE:0\r\n"
				"X-METHOD:NONE\r\n"
				"ATTENDEE;ROLE=ORGANIZER;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI;X-PHONEOWNER;ENCODING=QUOTED-PRINTABLE:=\r\n"
				"techview_agenda=40symbian.com\r\n"
				"ATTENDEE;X-SENTBY=Anish;ROLE=ATTENDEE;STATUS=NEEDS ACTION;RSVP=NO;EXPECT=FYI;X-ROLE=REQ-PARTICIPANT;X-STATUS=NEEDS-ACTION;X-CN=Symbian;ENCODING=QUOTED-PRINTABLE:=\r\n"
				"southwark=09\r\n"
				"LAST-MODIFIED:20011208T162623Z\r\n"
				"CATEGORIES:X-Bank Holiday;APPOINTMENT;HOLIDAY\r\n"
				"PRIORITY:1\r\n"
				"X-SYMBIAN-LUID:29\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960402T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960403T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960404T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960405T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960406T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960407T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960408T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960409T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960410T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960411T010000Z\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"																	
				"END:VCALENDAR\r\n"
				"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"TZ:+05:30\r\n"
				"BEGIN:VTODO\r\n"
				"RDATE:19960402T010000\r\n"
				"AALARM;TYPE=X-EPOCSOUND;ENCODING=QUOTED-PRINTABLE:20000221T060000Z;;;z:=5Csystem=5CSystemSounds=5Calarm.wav\r\n"
				"END:VTODO\r\n"
				"END:VCALENDAR\r\n"
				"BEGIN:VCALENDAR\r\n"
				"VERSION:1.0\r\n"
				"BEGIN:VEVENT\r\n"
				"UID:auid\r\n"
				"AALARM:20000221T060000\r\n"
				"DTSTART:20000221T070000\r\n"
				"DTEND:20000221T071500\r\n"
				"RDATE:20000402T010000;20000403T010000;20000403T020000\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				"\r\n"
				"BEGIN:VCALENDAR\r\n"
				"PRODID:-//Microsoft Corporation//Outlook 11.0 MIMEDIR//EN\r\n"
				"VERSION:2.0\r\n"
				"METHOD:PUBLISH\r\n"
				"BEGIN:VEVENT\r\n"
				"ORGANIZER:MAILTO:example.person@example.com\r\n"
				"DTSTART;VALUE=DATE:20011122\r\n"
				"DTEND;VALUE=DATE:20011123\r\n"
				"TRANSP:TRANSPARENT\r\n"
				"SEQUENCE:0\r\n"
				"UID:040000008200E00074C5B7101A82E0080000000030D6C760450EC7010000000000000000100\r\n"
				"00000DB618D9AE630C04788E2849C75A96C28\r\n"
				"DTSTAMP:20011122T144914Z\r\n"
				"SUMMARY:All day event\r\n"
				"PRIORITY:5\r\n"
				"X-MICROSOFT-CDO-IMPORTANCE:1\r\n"
				"CLASS:PUBLIC\r\n"
				"END:VEVENT\r\n"
				"END:VCALENDAR\r\n"
				"\r\n"
				);
	
_LIT(KEntryFile, "entry_for_OOM_Import.vcs");
	
	test.Printf(_L("\n\tTestOOMImportL: Starts\n"));
	
	iTestLib->CleanDatabaseL();		

	CreateTestFileL(KEntry, KEntryFile);
	
	RPointerArray<CCalEntry> entries;//Imported Vcal
	CleanupResetAndDestroyPushL(entries);
	ImportL(KEntryFile, entries, aIsSynch);
	test(entries.Count()==14);
	entries.ResetAndDestroy();
	
	TInt tryCount = 0;
	TInt err = KErrNone;
	
	// OOM LOOP
	for (;;)
		{
		RDebug::Printf("%d", tryCount);
		__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
		
		__UHEAP_MARK;
		TRAP(err, ImportL(KEntryFile, entries, aIsSynch));
		
		if (err == KErrNone) 
			{
			HBufC8* buffer; 
			TRAPD(err2,buffer=HBufC8::NewL(KMaxSize))
			if(err2 != KErrNone)
				{
				RDebug::Printf("Memory allocation testing for ImportL is done");
				__UHEAP_RESET;
				test(entries.Count() == 14);
				break;
				}
			else
				{
				RDebug::Printf("Memory allocation failure was ignored, contine the test.");
				delete buffer;
				err=KErrNoMemory;
				}
							
			}

		__UHEAP_RESET;
		test(err == KErrNoMemory);
		entries.ResetAndDestroy();
	 	__UHEAP_MARKEND;
		tryCount++;
		}// OOM LOOP
	
	CleanupStack::PopAndDestroy(&entries);//Imported Vcal	
	test.Printf(_L("TestOOMImportL: Successful\n"));
#else
	aIsSynch = ETrue;//To supress warning using the variable for none
	test.Printf(_L("TestOOMImportL: Skipped\n"));
#endif
	
	}

// Test for PDEF106038. 
// This test uses a day-note that ends right on the AgnDateTime::MaxDateAsTTime() limit.
// The duration of the event is 24 hours.
// The imported entry uses local time for DTSTART/DTEND.
// The duration of the event as calculated using local and utc time should be 24 hours.
// Prior to this fix the value returned using utc was 26 hours, ie actual duration +/- tz offset. 
void CDataExchangeTestManager::TestLocalToUtcConversionAgainstEventDurationL(TBool aIsSynch)	
	{
	test.Next(_L("Test the conversion of Local to UTC Time at AgnDateTime::MaxDateAsTTime() limit and it's effect on calculating event duration"));
	
	_LIT8(KLocalEntry,	"BEGIN:VCALENDAR\r\n"
	                    "VERSION:1.0\r\n"
	                    "BEGIN:VEVENT\r\n"
	                    "UID:tD5fMCA24EEsX818X2GVU1\r\n"
	                    "SUMMARY:dn\r\n"
	                    "DTSTART:21001230T000000\r\n"
	                    "DTEND:21001231T000000\r\n"
	                    "X-EPOCAGENDAENTRYTYPE:EVENT\r\n"
	                    "CLASS:PRIVATE\r\n"
	                    "SEQUENCE:0\r\n"
	                    "X-METHOD:NONE\r\n"
	                    "LAST-MODIFIED:20070122T093103Z\r\n"
	                    "PRIORITY:2\r\n"
	                    "X-SYMBIAN-LUID:3\r\n"
	                    "END:VEVENT\r\n"
	                    "END:VCALENDAR\r\n");
	                    
	_LIT(KFileName, "INC102846_Test_vcal.vcs");
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	test.Printf(_L("Importing the INC102846_Test_vcal entry..."));
	
	CreateTestFileL(KLocalEntry, KFileName);
	ImportL(KFileName, entryArray, aIsSynch);

	test(entryArray.Count() == 1);
	test.Printf(_L("Import OK..."));
	
	CCalEntry* entry;
	entry = entryArray[0];
	
	// Calculate Duration
    TTimeIntervalHours durationlocal(0);
    TTimeIntervalHours durationutc(0);
   
    TCalTime entrystart = entry->StartTimeL();
    TTime startutc = entrystart.TimeUtcL();
    TTime startlocal = entrystart.TimeLocalL();

    TCalTime entryend = entry->EndTimeL();
    TTime endutc = entryend.TimeUtcL();
    TTime endlocal = entryend.TimeLocalL();

    test.Printf(_L("startlocal %d%d%dT%+02d%+02d%+02d"), startlocal.DateTime().Year(), 
                                                startlocal.DateTime().Month()+1,
                                                startlocal.DateTime().Day() + 1,
                                                startlocal.DateTime().Hour(), 
                                                startlocal.DateTime().Minute(), 
                                                startlocal.DateTime().Second());

    test.Printf(_L("startutc   %d%d%dT%+02d%+02d%+02d"), startutc.DateTime().Year(), 
                                                startutc.DateTime().Month()+1,
                                                startutc.DateTime().Day() + 1,
                                                startutc.DateTime().Hour(), 
                                                startutc.DateTime().Minute(), 
                                                startutc.DateTime().Second());

    test.Printf(_L("endlocal   %d%d%dT%+02d%+02d%+02d"), endlocal.DateTime().Year(), 
                                                endlocal.DateTime().Month()+1,
                                                endlocal.DateTime().Day() + 1,
                                                endlocal.DateTime().Hour(), 
                                                endlocal.DateTime().Minute(), 
                                                endlocal.DateTime().Second());
    
    test.Printf(_L("endutc     %d%d%dT%+02d%+02d%+02d"), endutc.DateTime().Year(), 
                                                endutc.DateTime().Month()+1,
                                                endutc.DateTime().Day() + 1,
                                                endutc.DateTime().Hour(), 
                                                endutc.DateTime().Minute(), 
                                                endutc.DateTime().Second());
                                                   
    endutc.HoursFrom(startutc, durationutc);
    endlocal.HoursFrom(startlocal, durationlocal);
    
    test(24 == durationlocal.Int());
    test.Printf(_L("Duration local is %d hours."), durationlocal.Int());
    test.Printf(_L("Duration utc   is %d hours."), durationutc.Int());
   
    test(durationlocal == durationutc);
    test.Printf(_L("Duration calculated using both Local and UTC start/end times is 24 hours."));    

	CleanupStack::PopAndDestroy(&entryArray);
	}
// Test for PDEF107122.
// Create an appontment that has a monthly repeat-rule that
// continues forever.
// Store the entry and then retrieve it's repeat-rule.
// Get the returned repeat-rule's 'until' datetime as local-time. This value 
// will exceed max datetime due to the timezone.
// Set the localtime of a new TCalTime object with this value.
// It should not crash but set it to the maximum value.
void CDataExchangeTestManager::TestMaxDateTimeHandlingForRepeatRuleLocalTimeL(TBool /*aIsSynch*/)
	{
	test.Next(_L("Test Max Datetime Handling For Repeat-Rule LocalTime"));	
	
	//create an entry
	RPointerArray<CCalEntry> entriesToStore;
	CleanupResetAndDestroyPushL(entriesToStore);
	_LIT8(KUid, "UID_xx");
	HBufC8* guid = KUid().AllocLC();
	CCalEntry* entryP = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entryP);
	entriesToStore.AppendL(entryP);
	CleanupStack::Pop(entryP);

	TTime startTime (TDateTime(2005, EFebruary, 22, 8, 0, 0, 0)); 
	TCalTime calStartTime;
	calStartTime.SetTimeLocalL(startTime);
	TTime endTime (TDateTime(2005, EFebruary, 22, 8, 30, 0, 0)); 
	TCalTime calEndTime;
	calEndTime.SetTimeLocalL(endTime);
	entryP->SetStartAndEndTimeL(calStartTime, calEndTime);
	
	TCalRRule* rpt = new (ELeave) TCalRRule(TCalRRule::EMonthly);
	CleanupStack::PushL(rpt);

	rpt->SetDtStart(calStartTime);
	
	TCalRRule::TDayOfMonth monthday(ESunday, -1);
	RArray<TCalRRule::TDayOfMonth> dayofmonthArray;
	CleanupClosePushL(dayofmonthArray);
	dayofmonthArray.AppendL(monthday);
	rpt->SetByDay(dayofmonthArray);
	CleanupStack::PopAndDestroy(&dayofmonthArray);
	
	entryP->SetRRuleL(*rpt);//take a copy of the rule
	CleanupStack::PopAndDestroy(rpt);
	
	TInt entriesStored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entriesToStore, entriesStored);
	entriesToStore.ResetAndDestroy();
	
	iTestLib->SynCGetEntryViewL().FetchL(KUid, entriesToStore);
	entryP =  entriesToStore[0];
	
	//get attributes of the entry
	TCalRRule retrievedrule;
	entryP->GetRRuleL(retrievedrule);
	TCalTime repeatuntil = retrievedrule.Until();
	TTime repeatuntillocal= repeatuntil.TimeLocalL();
	
	TInt err(KErrNone);
	TCalTime newtime;
	TRAP(err, newtime.SetTimeLocalL(repeatuntillocal));
	test(KErrArgument != err);
	
	CleanupStack::PopAndDestroy(&entriesToStore);
	
	test.Printf(_L("Test Max Datetime Handling For Repeat-Rule LocalTime OK"));	
	}

TInt CDataExchangeTestManager::NumberAllEntryL()
	{//Find number of entries in Calendar file
	test.Next(_L("Entry count="));
	
	CCalIter* iter = CCalIter::NewL(iTestLib->GetSession());
	CleanupStack::PushL(iter);
	CDesC8ArrayFlat* uids = new(ELeave) CDesC8ArrayFlat(10);
	CleanupStack::PushL(uids);

	TPtrC8 uid(iter->FirstL());
	TInt ii = 0;
	while (uid != KNullDesC8())
		{
		uids->AppendL(uid);
		++ii;
		uid.Set(iter->NextL());
		}

 	TInt numEntries = uids->Count();
 	CleanupStack::PopAndDestroy(2, iter);
 	test.Printf(_L("Number of entries is %d"), numEntries);
	return numEntries;
	}
	
void CDataExchangeTestManager::TestImportEntryWithoutGuidL(TBool aIsSynch)
	{//PDEF112568
	_LIT(KEntryWithoutGuidFile, "entryWithourGuid.vcs");
	_LIT8(KEntryWithoutGuid,
		"BEGIN:VCALENDAR\r\n"
		"PRODID:PalmDesktop Generated\r\n"
		"TZ:+01\r\n"
		"VERSION:1.0\r\n"
		"BEGIN:VEVENT\r\n"
		"SUMMARY;CHARSET=macintosh:Nyårsdagen,röd,flaggdag\r\n"
		"DTSTART:20051231T230000Z\r\n"
		"CATEGORIES:Helgdag\r\n"
		"END:VEVENT\r\n"
		"END:VCALENDAR\r\n"
		"\r\n"
		);
	
 	test.Printf(_L("Testing importing GUIDless entries\n"));	
	
	const TInt KNumEntries = 200;
	
	iTestLib->CleanDatabaseL();
	
	RFile file;
	iTestLib->TestRegister().CreateTempFileLC(file, KEntryWithoutGuidFile); 
	for(TInt i=0; i<KNumEntries; ++i)
		{
		User::LeaveIfError(file.Write(KEntryWithoutGuid()));	
		}
	
	CleanupStack::PopAndDestroy(&file); //file.Close()

	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	ImportL(KEntryWithoutGuidFile, entries, aIsSynch);
	test(entries.Count() == KNumEntries);
	
	TInt success = 0;
	iTestLib->SynCGetEntryViewL().StoreL(entries, success);
	test(success == KNumEntries);
	
 	test.Printf(_L("GUIDless entries test completed\n"));	
	
	CleanupStack::PopAndDestroy(&entries);
	
	test(KNumEntries==NumberAllEntryL());
	}


void CDataExchangeTestManager::TestMultipleTZPropertiesL()
	{
	test.Next(_L("test that entries in different time zones are exported correctly using the asycronous export"));
	test.Printf(_L("clear the calendar file"));
	iTestLib->CleanDatabaseL();
	
	RTz tz;
	CleanupClosePushL(tz);
	User::LeaveIfError(tz.Connect());

	const TTime KEntryStart(TTime(TDateTime(2007, EMay, 23, 12, 0, 0, 0)));
	const TTime KEntryEnd(TTime(TDateTime(2007, EMay, 23, 12, 30, 0, 0)));
	
	test.Printf(_L("set the system time zone to Hawaii"));
	CTzId* hawaii = CTzId::NewL(_L8("Pacific/Honolulu"));
	CleanupStack::PushL(hawaii);
	tz.SetTimeZoneL(*hawaii);
	CleanupStack::PopAndDestroy(hawaii);

	test.Printf(_L("create the honolulu entry"));
	HBufC8* guid1 = NULL;
	CCalEntry* entry1 = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid1);
	CleanupStack::PushL(entry1);
	TCalTime entryStart1;
	entryStart1.SetTimeLocalL(KEntryStart);
	TCalTime entryEnd1;
	entryEnd1.SetTimeLocalL(KEntryEnd);
	entry1->SetStartAndEndTimeL(entryStart1, entryEnd1);
	TCalRRule rRule1(TCalRRule::EDaily);
	rRule1.SetCount(0);
	rRule1.SetDtStart(entry1->StartTimeL());
	rRule1.SetInterval(1);
	entry1->SetRRuleL(rRule1);
	entry1->SetSummaryL(_L("meeting"));
	entry1->SetDescriptionL(_L("meeting"));
	entry1->SetLocationL(_L("Honolulu"));
	iTestLib->StoreEntryL(*entry1);
	TCalLocalUid localUid1 = entry1->LocalUidL();
	CleanupStack::PopAndDestroy(entry1);
	
	test.Printf(_L("set the system time zone to Vladivostok"));
	CTzId* vladivostok = CTzId::NewL(_L8("Asia/Vladivostok"));
	CleanupStack::PushL(vladivostok);
	tz.SetTimeZoneL(*vladivostok);
	CleanupStack::PopAndDestroy(vladivostok);
	
	test.Printf(_L("create the vladivostok entry"));
	HBufC8* guid2 = NULL;
	CCalEntry* entry2 = iTestLib->CreateCalEntryL(CCalEntry::EAppt, guid2);
	CleanupStack::PushL(entry2);
	TCalTime entryStart2;
	entryStart2.SetTimeLocalL(KEntryStart);
	TCalTime entryEnd2;
	entryEnd2.SetTimeLocalL(KEntryEnd);
	entry2->SetStartAndEndTimeL(entryStart2, entryEnd2);
	TCalRRule rRule2(TCalRRule::EWeekly);
	rRule2.SetCount(0);
	rRule2.SetDtStart(entry2->StartTimeL());
	rRule2.SetInterval(1);
	
	RArray<TDay> days;
	CleanupClosePushL(days);
	days.AppendL(EThursday);
	rRule2.SetByDay(days);
	CleanupStack::PopAndDestroy(&days);
	
	entry2->SetRRuleL(rRule2);
	entry2->SetSummaryL(_L("meeting"));
	entry2->SetDescriptionL(_L("meeting"));
	entry2->SetLocationL(_L("Vladivostok"));
	iTestLib->StoreEntryL(*entry2);
	TCalLocalUid localUid2 = entry2->LocalUidL();
	CleanupStack::PopAndDestroy(entry2);
	
	test.Printf(_L("set the system time zone to London"));
	CTzId* london = CTzId::NewL(_L8("Europe/London"));
	CleanupStack::PushL(london);
	tz.SetTimeZoneL(*london);
	CleanupStack::PopAndDestroy(london);
	
	test.Printf(_L("fetch the entries"));
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	CCalEntry* fetchedEntry1 = iTestLib->SynCGetEntryViewL().FetchL(localUid1);
	CleanupStack::PushL(fetchedEntry1);
	entries.AppendL(fetchedEntry1);
	CleanupStack::Pop(fetchedEntry1);
	CCalEntry* fetchedEntry2 = iTestLib->SynCGetEntryViewL().FetchL(localUid2);
	CleanupStack::PushL(fetchedEntry2);
	entries.AppendL(fetchedEntry2);
	CleanupStack::Pop(fetchedEntry2);
	
	test.Printf(_L("export these two entries using asychronous export"));
	RFile file;
	CleanupClosePushL(file);
	iTestLib->FileSession().MkDirAll(KMultipleTZAsyncFilename);
	file.Replace(iTestLib->FileSession(), KMultipleTZAsyncFilename, EFileWrite); 
	RFileWriteStream writeStream(file);
	CleanupClosePushL(writeStream);
	
	iDataExchange->ExportAsyncL(KUidVCalendar, writeStream, entries, *this);
	CActiveScheduler::Start();
	
	CleanupStack::PopAndDestroy(&writeStream);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&entries);
	
	test.Printf(_L("delete the entries from the calendar file"));
	iTestLib->CleanDatabaseL();
	
	test.Printf(_L("import the two entries from the vCal file"));
	CleanupResetAndDestroyPushL(entries);
	CleanupClosePushL(file);
	file.Open(iTestLib->FileSession(), KMultipleTZAsyncFilename, EFileRead);
	RFileReadStream readStream(file);
	CleanupClosePushL(readStream);
	iDataExchange->ImportL(KUidVCalendar, readStream, entries);
	
	test.Printf(_L("check that two entries have been imported"));
	const TInt KEntryCount(entries.Count());
	test(KEntryCount == 2);
	
	test.Printf(_L("store the two entries"));
	TInt numStored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entries, numStored);
	
	CleanupStack::PopAndDestroy(&readStream);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&entries);
	
	test.Printf(_L("fetch an instance of each entry"));
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	TCalTime startRange;
	startRange.SetTimeLocalL(TDateTime(2007, EMay, 30, 0, 0, 0, 0));
	TCalTime endRange;
	endRange.SetTimeLocalL(TDateTime(2007, EMay, 30, 23, 59, 0, 0));
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
	
	test.Printf(_L("test that two instances were found"));
	const TInt KInstanceCount(instances.Count());
	test(KInstanceCount == 2);
	
	for (TInt i(0) ; i < KInstanceCount ; ++i)
		{
		test.Printf(_L("%S day = %d, hour = %d, local uid = %d"),
											&instances[i]->Entry().LocationL(),
											instances[i]->Time().TimeLocalL().DateTime().Day(),
											instances[i]->Time().TimeLocalL().DateTime().Hour(),
											instances[i]->Entry().LocalUidL());
		}
	
	test.Printf(_L("test the vladivostock instance time"));
	test(instances[0]->Time().TimeLocalL() == TTime(TDateTime(2007, EMay, 30, 2, 0, 0, 0))); // the vladivostok
	
	test.Printf(_L("test the honolulu instance time"));
	test(instances[1]->Time().TimeLocalL() == TTime(TDateTime(2007, EMay, 30, 23, 0, 0, 0)) ); // the hawaii instance
	
	iTestLib->FileSession().Delete(KMultipleTZAsyncFilename);
	CleanupStack::PopAndDestroy(&instances);
	CleanupStack::PopAndDestroy(&tz);
	}

void CDataExchangeTestManager::TestDaylightExportOverlapL(TBool aIsSynch)
	{
	test.Next(_L("Test that day light savings rules that start in the past and ovelap into now are still exported"));
	
	iTestLib->CleanDatabaseL();
	
	// The time must be the beginning of 2008
	User::SetUTCTime(TDateTime(2008, EMay, 27, 12, 0, 0, 0));
	
	_LIT(KEntryDaylightOverlapNowFile, "overlapdaylightpastnow.vcs");
	
	// import this entry
	CreateTestFileL(KEntryDaylightOverlapNow, KEntryDaylightOverlapNowFile);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	ImportL(KEntryDaylightOverlapNowFile, entries, aIsSynch);
	ExportL(KEntryDaylightOverlapNowFile, entries, aIsSynch);
	ImportL(KEntryDaylightOverlapNowFile, entries, aIsSynch);
	
	iTestLib->StoreEntryL(*entries[1]);
		
	TCalTime startRange;
	startRange.SetTimeLocalL(TDateTime(2000, EMay, 0, 0, 0, 0, 0));
	TCalTime endRange;
	endRange.SetTimeLocalL(TDateTime(2009, EMay, 0, 0, 0, 0, 0));
	
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	//
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
	
	const TInt KInstanceCount(instances.Count());
	for (TInt i(0) ; i < KInstanceCount ; ++i)
		{
		test.Printf(_L("%S year = %d, month = %d, day = %d, hour = %d, local uid = %d"),
											&instances[i]->Entry().LocationL(),
											instances[i]->Time().TimeLocalL().DateTime().Year(),
											instances[i]->Time().TimeLocalL().DateTime().Month(),
											instances[i]->Time().TimeLocalL().DateTime().Day(),
											instances[i]->Time().TimeLocalL().DateTime().Hour(),
											instances[i]->Entry().LocalUidL());
		
		test(instances[i]->Time().TimeLocalL().DateTime().Hour() == 11);
		}
	
	CleanupStack::PopAndDestroy(&instances);
	CleanupStack::PopAndDestroy(&entries);
	}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * DoTestL()
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void CDataExchangeTestManager::SyncAndAsynchronousTests(TBool aIsSynch)
	{	
		
	TPerformanceTimer timer(test);
	timer.Start();
	// Run the test suite
 	test.Printf(_L("Test vCalendar import & export\n"));
	ExtractAlarmL(aIsSynch);
	TestImportEntriesL(aIsSynch);
	TestLUIDFromImportEntriesL(aIsSynch);
	TestImportNEntriesL();
	TestImportEntriesWithTrailingSpaceL(aIsSynch);
	TestExportEntriesL(aIsSynch);
	TestOrganizerIsStoredL(aIsSynch);
	TestGSEntriesL(aIsSynch);
	TestEntriesL(CCalEntry::EAnniv, aIsSynch);
	TestEntriesL(CCalEntry::EReminder, aIsSynch);
	TestEntriesL(CCalEntry::ETodo, aIsSynch);
	TestEntriesL(CCalEntry::EEvent, aIsSynch);	
	TestEntriesL(CCalEntry::EAppt, aIsSynch);

	TestAlarmEntriesL(CCalEntry::ETodo, aIsSynch);//memory leak caused by alarmentries
	TestAlarmEntriesL(CCalEntry::EAppt, aIsSynch);
	TestAlarmEntriesL(CCalEntry::EReminder, aIsSynch);
	TestAlarmEntriesL(CCalEntry::EEvent, aIsSynch);
	TestAlarmEntriesL(CCalEntry::EAnniv, aIsSynch);
	TestPhoneOwnerPersistenceL();
	TestEntryUpdateL(aIsSynch); 
	TestReminderEntryExportL(aIsSynch);
	TestReminderEntryImportL(aIsSynch);

	TestImportTodoWithTzL(aIsSynch);
	TestImportTodoWithWrongTzL(aIsSynch);

	TestFixedAndFloatingTodoL(aIsSynch);
	TestTodoWithoutDueDateL(aIsSynch);
	TestAlarmedTodoWithoutDueDateL(aIsSynch); 
	TestEntriesWithoutDatesL(aIsSynch);
	TestFloatingExceptionL(aIsSynch);
	TestVCalTypeSequenceL(aIsSynch);
	TestTodoCompletedDateL(aIsSynch);	
	TestOutOfRangeRDateL(aIsSynch);
	TestInvalidUntilDateL(aIsSynch);
	TestInvalidExceptionsL(aIsSynch);

	TestAttachmentImportExportL(aIsSynch);
	TestAttachmentImportExportL(aIsSynch);
	TestAttachmentExportImportL(aIsSynch);

	EmptyAttachmentL(aIsSynch);
	// clean up files before starting new batch of tests

	ResetAgendaFileL();
	// from former tcal_entry
	test.Printf(_L("Test vCalendar import (former tcal_entry)\n"));

	CreateVcalFileL(KEntry5, KVCalendarFile);
	CreateVcalFileL(KLastDay, KVCalLastDayFile);

	DoImportL(KVCalendarFile);
	DoImportLastDayL(KVCalLastDayFile);
	
	DeleteVcalFileL(KVCalendarFile);
	DeleteVcalFileL(KVCalLastDayFile);
	TestChildEntryL();

	TestImportTodoWithTzL(aIsSynch);
	TestImportTodoWithXEpocAlarmL(aIsSynch);
	TestImportTodoWithWrongTzL(aIsSynch);

	//DEF094045:	
	TestImportParentWithoutRRuleL(aIsSynch);

	TestOrphanedEntryL(aIsSynch);
	
	TestFloatingExceptionL(aIsSynch);
	
	ImportAndExportL(KEntry19, KEntryImportFile, 4, ETrue);
	ImportAndExportL(KEntry19, KEntryImportFile, 4, EFalse);		


	TestToDoStartDateL(aIsSynch);
	TestFloatingToDoStartDateL(aIsSynch);

	TestOrphanedEntryL(aIsSynch);
	TestExceptionDatesConversionL(aIsSynch);
	TestUntilDateImportL(aIsSynch);

	TestImportingAlarmedFloatingAniversaryL(aIsSynch);
	TestMidnightRecId2L(aIsSynch);
	TestMidnightRecId3L(aIsSynch);

	// PDEF101009
	TestExportToDoStartDateEqualDueDateL(aIsSynch);

	TestVCalTypeSequenceL(aIsSynch);
	//checking importing of VCALS before and after fix for DEF069294
	TestUIDImportL(KTestWithLongUTFUID, KExportedUTFUID, aIsSynch);	
	TestUIDImportL(KTestNewExportLongUID, KLongExportedUID, aIsSynch);			

	TestFloatingRecIdL(aIsSynch);
	TestMonthlyRepeatDateL(aIsSynch);

	TestImportingAlarmedFloatingAniversaryL(aIsSynch);
   	TestImportStoreParentChildEntriesL(aIsSynch);
	TestFloatingEntryFixedExDateL(aIsSynch);
	TestExceptedEntryWithRDateL(aIsSynch);
	TestRDateExceptionL(aIsSynch);
	PanicOnUpdateL(aIsSynch);
	TestMidnightRecIdL(aIsSynch);
	
	// DEF085706
	TestRDateAndExceptionL(aIsSynch);	
	TestImportStoreNotMatchingParentChildEntriesL(aIsSynch);
	TestMailtoL();
	TestExDateTzProblemL(aIsSynch);
	
	TestImportingInvalidChildEntriesL(aIsSynch);
	AlarmOffsetForUndatedTodoL(aIsSynch);
	TestCategoryL(aIsSynch);

	// tests which change the TZ come last because some other tests are TZ-dependent
	TestTzPropertiesL(aIsSynch);
	ChangeTimeZoneAndRunTestL(KTzAsiaTokyo, &CDataExchangeTestManager::BadExceptionDateTestL, aIsSynch);
	ChangeTimeZoneAndRunTestL(KTzEuropeHelsinki, &CDataExchangeTestManager::ExportFloatingAlarmL, aIsSynch);
	ChangeTimeZoneAndRunTestL(KTzAmericaVancouver, &CDataExchangeTestManager::TestRRuleEndDateL, aIsSynch);

	TestTzInstanceCountL(aIsSynch);
	BadDaylightRuleL(aIsSynch);
	
	TestImportingInvalidChildEntriesL(aIsSynch);

	TestParentDTENDExportL(aIsSynch);

	//DEF086733
	TestExDateTimeL(aIsSynch);
	
	AlarmOffsetForUndatedTodoL(aIsSynch);
	TestExceptedEntryWithRDateL(aIsSynch);
	
	TestChildDTSTARTExportL(aIsSynch);
	TestReccurenceIDRangeL(aIsSynch);
	
	ChangeTimeZoneAndRunTestL(KTzStanley, &CDataExchangeTestManager::TestBadTimeZoneL, aIsSynch);
	
	//DEF087898
	// these expect no DAYLIGHT rule to be exported
	TestImportingInvalidDayLightL(EImportedDLRuleBadStart, EFalse, aIsSynch);
	TestImportingInvalidDayLightL(EImportedDLRuleBadEnd, EFalse, aIsSynch);
	TestImportingInvalidDayLightL(EImportedDLRuleEndBeforeStart, EFalse, aIsSynch);
	TestImportingInvalidDayLightL(EImportedDLRuleMissing, EFalse, aIsSynch);
	TestImportingInvalidDayLightL(EImportedDLTwoBadRules, EFalse, aIsSynch);
	TestImportingInvalidDayLightL(EImportedDLNoTZTwoBadRules, EFalse, aIsSynch); 
	TestImportingInvalidDayLightL(EImportedDLNoTZMissing, EFalse, aIsSynch);
	
	// next two produce a TZ property, but no DAYLIGHT property 
	TestImportingInvalidDayLightL(EImportedDLNoTZOneGoodRule, EFalse, aIsSynch);
	TestImportingInvalidDayLightL(EImportedDLNoTZOneBadOneGoodRule, EFalse, aIsSynch);

	// these expect a DAYLIGHT rule to be exported
	TestImportingInvalidDayLightL(EImportedDLOneGoodRule, ETrue, aIsSynch);
	TestImportingInvalidDayLightL(EImportedDLOneBadOneGoodRule, ETrue, aIsSynch);

	TestTzExportL(aIsSynch);
	TestLUIDL(aIsSynch);
//	TestRepeatEntryHasCountAndEndDateL(aIsSynch);
	TestImportvCalsGeneratesUniqueIdL(aIsSynch);
	TestRRuleUntilTimeL(aIsSynch);
//	TestRepeatEntryHasCountAndEndDateL(aIsSynch);
	TestExDateUntilTimeL(aIsSynch);
	//DEF093631
	TestImportWithInvalidRRuleDateL(aIsSynch);

	ImportInvalidChildEntryL(aIsSynch);

	TestGEOImportAndExportL(aIsSynch);	

	TestMP1RRuleL(aIsSynch);
	TestLostRangeL(aIsSynch);
	TestChildAddedToFirstInstanceL(aIsSynch);

	// testManager->TestOOMExportL();
	TestEntryAlarmTimeL(aIsSynch);
	
	ChangeTimeZoneAndRunTestL(KTzEuropeHelsinki, &CDataExchangeTestManager::TestMaxDateTimeHandlingForRepeatRuleLocalTimeL, EFalse);
	
	// TestImportEntryWithoutGuidL(aIsSynch);	
	// commenting out test which is crashing until defect DEF112827 has been fixed.
 	// testManager->TestRDATEOOMExportL();
	TestOOMImportL(aIsSynch);	
	ChangeTimeZoneAndRunTestL(KTzEuropeHelsinki, &CDataExchangeTestManager::TestLocalToUtcConversionAgainstEventDurationL, aIsSynch);

    //PDEF111292 - Begin
	TestTzExportKSTL(aIsSynch);
    TestTzExportPSTL(aIsSynch);

    //PDEF111292 - End

	TestMultipleTZPropertiesL();
	
	TestDaylightExportOverlapL(aIsSynch);
	
	timer.Stop();
	test.Printf(_L("Done\n"));
	// printout performance time
	timer.PrintOut();
			
	}
	
static void DoTestL()
	{
	CDataExchangeTestManager* testManager = CDataExchangeTestManager::NewLC();
	//calling Tests for Synchronous ImportL/ExportL
	test.Printf(_L("Tests starting for Synchronous data exchange\n"));
	testManager->SyncAndAsynchronousTests(ETrue);
	//calling Tests for Asynchronous ImportL/ExportL
	test.Printf(_L("Tests starting for Asynchronous data exchange\n"));
	testManager->SyncAndAsynchronousTests(EFalse); 
	
	testManager->SetNumEntriesToImportAndExport(50);
	testManager->TestAsyncExportL(KAsyncExportFile);
	testManager->TestAsyncImportL(KAsyncExportFile);	
	
	testManager->SetNumEntriesToImportAndExport(0);
	testManager->TestAsyncExportL(KAsyncExportFile);
	testManager->TestAsyncImportL(KAsyncExportFile);
	testManager->TestMultipleAsyncImportL();

    CleanupStack::PopAndDestroy(testManager);	
    }

TInt E32Main()
	{
	__UHEAP_MARK;

	test.Start(_L("Calendar Interim API Data Exchange test suite"));
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
	RDebug::Print(_L("DoTestL returned: %d"), ret);
	
	test(ret == KErrNone);
	
	delete scheduler;
	delete trapCleanup;	

	test.End();
	test.Close();

	__UHEAP_MARKEND;

	return (KErrNone);
	}
