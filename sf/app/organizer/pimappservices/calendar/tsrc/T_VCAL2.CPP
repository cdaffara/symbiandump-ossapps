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
#include <s32file.h>

#include <caldataexchange.h>
#include <caldataformat.h>
#include <calentryview.h>
#include <calalarm.h>
#include <caliterator.h>

#include <tz.h> //Conversion between local and UTC times

#include "caltestlib.h"
#include "caltestoom.h"

LOCAL_D RTest test(_L("T_VCAL2"));
#define KMaxDateStringLength	30

_LIT(KTestCalendarFile,"c:t_vcal2");

_LIT(KLongSoundNameExportFile,"vCalLongSoundNameExport.vcs");


_LIT8(KOOMEntry, 	"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"BEGIN:VEVENT\r\n"
					"DESCRIPTION:description\r\n"
					"ATTACH:attachment\r\n"
					"SUMMARY:summary\r\n"
					"CLASS:PUBLIC\r\n"
					"DTSTART:19990915T090000Z\r\n"
					"DTEND:19990915T100000Z\r\n"
					"END:VEVENT\r\n"
					"END:VCALENDAR\r\n");

//From EXT-5G2CUY "SyncML/versit: Updated VCALENDAR with empty RRULE crash the phone"
_LIT8(KMissingRRule, "BEGIN:VCALENDAR\r\n"
					 "VERSION:1.0\r\n"
					 "BEGIN:VEVENT\r\n"
					 "UID:4\r\n"
					 "SUMMARY:test\r\n"
					 "DTSTART:20021120T090000Z\r\n"
					 "DTEND:20021120T100000Z\r\n"
					 "X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
					 "CLASS:PUBLIC\r\n"
					 "DCREATED:20021120T000000Z\r\n"
					 "RRULE:\r\n"
					 "LAST-MODIFIED:20021120T122700Z\r\n"
					 "PRIORITY:0\r\n"
					 "STATUS:NEEDS ACTION\r\n"
					 "END:VEVENT\r\n"
					 "END:VCALENDAR\r\n");
					
_LIT8(KEmptyAlarm,	"BEGIN:VCALENDAR\r\n"
                    "VERSION:1.0\r\n"
                    "BEGIN:VEVENT\r\n"
					"UID:8\r\n"
					"SUMMARY:empty alarm\r\n"
					"DTSTART:20020916T090000Z\r\n"
					"DTEND:20020916T100000Z\r\n"
					"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
					"AALARM:\r\n"
					"CLASS:PUBLIC\r\n"
					"DCREATED:20020919T000000Z\r\n"
					"LAST-MODIFIED:20020919T082100Z\r\n"
					"PRIORITY:0\r\n"
					"STATUS:NEEDS ACTION\r\n"
					"END:VEVENT\r\n"
                    "END:VCALENDAR\r\n");

_LIT8(KTodoCompletedEntry,	"BEGIN:VCALENDAR\r\n"
							"VERSION:1.0\r\n"
							"BEGIN:VTODO\r\n"
							"DUE:20001224T120000Z\r\n"
							"SUMMARY:Buy Christmas Presents\r\n"
							"STATUS:COMPLETED\r\n"
							"PRIORITY:3\r\n"
							"END:VTODO\r\n"
							"END:VCALENDAR\r\n");

_LIT8(KLongSoundNameAlarmExport, "AALARM;TYPE=X-EPOCSOUND:20020809T083900Z;;;");
_LIT8(KLongSoundName,	"BEGIN:VCALENDAR\r\n"
						"VERSION:1.0\r\n"
						"TZ:+00\r\n"
						"DAYLIGHT:TRUE;+01;20020407T010000;20021031T020000;-05;-04\r\n"
						"BEGIN:VEVENT\r\n"
						"DTSTART:20040809T090000Z\r\n"
						"DTEND:20040809T100000Z\r\n"
						"LAST-MODIFIED:20040804T060600Z\r\n"
						"PRIORITY:4\r\n"
						"CLASS:CONFIDENTIAL\r\n"
						"SUMMARY:An event\r\n"
						"AALARM;TYPE=X-EPOCSOUND:20020809T093900;;0;BigSoundNamewwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww.mp3\r\n"
						"END:VEVENT\r\n"
						"END:VCALENDAR\r\n");

_LIT8(KAlarmedEntry, 	"BEGIN:VCALENDAR\r\n"
						"VERSION:1.0\r\n" 
						"BEGIN:VEVENT\r\n"
						"CLASS:PUBLIC\r\n"
						"DTSTART:20010607T090000\r\n"
						"DTEND:20010607T093000\r\n"
						"AALARM:20010607T084500\r\n"
						"SUMMARY:SUMMARY\r\n"
						"DESCRIPTION:DESCRIPTION\r\n"
						"END:VEVENT\r\n"
						"END:VCALENDAR\r\n");


_LIT8(KPtr,				 "BEGIN:VCALENDAR\r\n" "PRODID:-//Microsoft Corporation//Outlook MIMEDIR//EN\r\n"
                         "VERSION:1.0\r\n"
						 "BEGIN:VEVENT\r\n"
						 "STATUS:ACCEPTED\r\n"
						 "DTSTART:20010418T120000Z\r\n"
						 "DTEND:20010418T130000Z\r\n"
						 "LOCATION:Pub\r\n"
						 "SUMMARY:Eat lunch\r\n"
						 "DESCRIPTION:Working lunch with Example Team. Discuss the preliminary work for new Gee-Whiz promo.\r\n"
						 "PRIORITY:2\r\n"
						 "ATTACH:memo\r\n"
                         "END:VEVENT\r\n"
						 "BEGIN:VEVENT\r\n"
						 "STATUS:CONFIRMED\r\n"
						 "DTSTART:20010418T120000Z\r\n"
						 "DTEND:20010418T130000Z\r\n"
						 "LOCATION:Trendy Night Club\r\n"
						 "SUMMARY:Party!!!\r\n"
						 "DESCRIPTION:Get your glad rags on for the night of the year. Get down to the Example Night Club. Starts 8.00pm. \r\n"
						 "PRIORITY:2\r\n"
						 "ATTACH:memo\r\n"
                         "END:VEVENT\r\n"
						 "BEGIN:VEVENT\r\n"
						 "STATUS:DECLINED\r\n"
						 "DTSTART:20010418T120000Z\r\n"
						 "DTEND:20010418T130000Z\r\n"
						 "LOCATION:Lecture Hall\r\n"
						 "SUMMARY:Train Timetables Lecture \r\n"
						 "DESCRIPTION:An exciting journey through the history of railway timetables.\r\n"
						 "PRIORITY:2\r\n"
						 "ATTACH:memo\r\n"
                         "END:VEVENT\r\n"
						 "BEGIN:VTODO\r\n"					// todo entry
						 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
						 "STATUS:TENTATIVE\r\n"
						 "DESCRIPTION:to-do item with priority 1\r\n"
						 "CLASS:PUBLIC\r\n"
						 "DCREATED:20000113T000000Z\r\n"
						 "LAST-MODIFIED:20000113T115900Z\r\n"
						 "PRIORITY:1\r\n"
						 "END:VTODO\r\n"
						 "BEGIN:VTODO\r\n"					// todo entry
						 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
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
						 "STATUS:DELEGATED\r\n"
						 "DESCRIPTION:Pack Holiday clothes todo\r\n"
						 "CLASS:PUBLIC\r\n"
						 "DCREATED:20010420T000000Z\r\n"
						 "LAST-MODIFIED:20010313T115900Z\r\n"
						 "PRIORITY:3\r\n"
						 "END:VTODO\r\n"
						 "BEGIN:VTODO\r\n"					// todo entry
						 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
						 "STATUS:NEEDS ACTION\r\n"
						 "SUMMARY:Gas Bill\r\n"
						 "DESCRIPTION:Pay this before they cut us off!\r\n"
						 "CLASS:PUBLIC\r\n"
						 "DCREATED:20000213T000000Z\r\n"
						 "LAST-MODIFIED:20000213T115900Z\r\n"
						 "PRIORITY:4\r\n"
                         "END:VTODO\r\n"
 						 "BEGIN:VTODO\r\n"
 						 "UID:6\r\n"
 						 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
 						 "DTSTART:19991101T100000\r\n" //no duedate  - should not cause a problem when retrieving
 						 "END:VTODO\r\n"
 						 "BEGIN:VTODO\r\n"
 						 "UID:7\r\n"
 						 "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
 						 "END:VTODO\r\n"
 						 "BEGIN:VEVENT\r\n"
						 "STATUS:NEEDS ACTION\r\n"
						 "DTSTART:20010421T120000Z\r\n"
						 "DTEND:20010421T180000Z\r\n"
						 "LOCATION:Cardiff\r\n"
						 "DESCRIPTION:\r\n"  // This should not cause a problem!!
						 "SUMMARY: FOOTBALL MATCH FINAL\r\n"
						 "PRIORITY:2\r\n"
						 "ATTACH:memo\r\n"
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
						 "ATTACH:memo\r\n"
                         "END:VEVENT\r\n"
						 "BEGIN:VEVENT\r\n"
						 "STATUS:NEEDS ACTION\r\n"
						 "DTSTART:20010421T120000Z\r\n"
						 "DTEND:20010421T180000Z\r\n"
						 "LOCATION:Example Restaurant\r\n"
						 "SUMMARY:Meal for 4\r\n"
						 "DESCREPTION:Book early to get a good table and reservation time.\r\n"
						 "PRIORITY:2\r\n"
						 "ATTACH:memo\r\n"
                         "END:VEVENT\r\n"
                         "END:VCALENDAR\r\n");
                         
_LIT8(KPtr2,			"BEGIN:VCALENDAR\r\n"	//Entry for Japan's time zone
                         "VERSION:1.0\r\n"
                         "TZ:+09\r\n"
                         "BEGIN:VEVENT\r\n"
                         "DTSTART:20020201T040000Z\r\n"
                         "DTEND:20020201T050000Z\r\n"
                         "AALARM:20020201T035000Z;;1;\r\n"
                         "LAST-MODIFIED:20020513T154705Z\r\n"
                         "PRIORITY:4\r\n"
                         "CLASS:CONFIDENTIAL\r\n"
                         "SUMMARY:S1\r\n"
                         "ATTENDEE;STATUS=CONFIRMED;RSVP=NO:Example Company\r\n"
                         "END:VEVENT\r\n"
                         "BEGIN:VEVENT\r\n"
                         "DTSTART:20020501T040000Z\r\n"
                         "DTEND:20020501T050000Z\r\n"
                         "AALARM:20020501T035000Z;;2;\r\n"
                         "LAST-MODIFIED:20020513T154734Z\r\n"
                         "PRIORITY:4\r\n"
                         "CLASS:CONFIDENTIAL\r\n"
                         "SUMMARY:S1\r\n"
                         "ATTENDEE;STATUS=CONFIRMED;RSVP=NO:Example Company\r\n"
                         "END:VEVENT\r\n"
                         "END:VCALENDAR\r\n\r\n"		//End of entry for Japan
                         "BEGIN:VCALENDAR\r\n"			//Entry for UK's time zone
                         "VERSION:1.0\r\n"
                         "TZ:+00\r\n"
                         "DAYLIGHT:TRUE;+01;20020331T000000;20021031T020000;+00;+01\r\n"
                         "BEGIN:VEVENT\r\n"
                         "DTSTART:20020202T040000Z\r\n"
                         "DTEND:20020202T050000Z\r\n"
                         "AALARM:20020202T035000Z;;1;\r\n"
                         "LAST-MODIFIED:20020513T155829Z\r\n"
                         "PRIORITY:4\r\n"
                         "CLASS:CONFIDENTIAL\r\n"
                         "SUMMARY:S1\r\n"
                         "ATTENDEE;STATUS=CONFIRMED;RSVP=NO:Example Company\r\n"
                         "END:VEVENT\r\n"
                         "BEGIN:VEVENT\r\n"
                         "DTSTART:20020502T040000Z\r\n"
                         "DTEND:20020502T050000Z\r\n"
                         "AALARM:20020502T035000Z;;2;\r\n"
                         "LAST-MODIFIED:20020513T155906Z\r\n"
                         "PRIORITY:4\r\n"
                         "CLASS:CONFIDENTIAL\r\n"
                         "SUMMARY:S1\r\n"
                         "ATTENDEE;STATUS=CONFIRMED;RSVP=NO:Example Company\r\n"
                         "END:VEVENT\r\n"
                         "END:VCALENDAR\r\n\r\n"		//End of entry for UK
                         "BEGIN:VCALENDAR\r\n"			//Entry for New York's time zone
                         "VERSION:1.0\r\n"
                         "TZ:-05\r\n"
                         "DAYLIGHT:TRUE;-04;20020407T010000;20021031T020000;-05;-04\r\n"
                         "BEGIN:VEVENT\r\n"
                         "DTSTART:20020203T040000Z\r\n"
                         "DTEND:20020203T050000Z\r\n"
                         "AALARM:20020203T035000Z;;1;\r\n"
                         "LAST-MODIFIED:20020513T161055Z\r\n"
                         "PRIORITY:4\r\n"
                         "CLASS:CONFIDENTIAL\r\n"
                         "SUMMARY:S1\r\n"
                         "ATTENDEE;STATUS=CONFIRMED;RSVP=NO:Example Company\r\n"
                         "END:VEVENT\r\n"
                         "BEGIN:VEVENT\r\n"
                         "DTSTART:20020503T040000Z\r\n"
                         "DTEND:20020503T050000Z\r\n"
                         "AALARM:20020503T035000Z;;2;\r\n"
                         "LAST-MODIFIED:20020513T161133Z\r\n"
                         "PRIORITY:4\r\n"
                         "CLASS:CONFIDENTIAL\r\n"
                         "SUMMARY:S1\r\n"
                         "ATTENDEE;STATUS=CONFIRMED;RSVP=NO:Example Company\r\n"
                         "END:VEVENT\r\n"
                         "END:VCALENDAR\r\n");			//End of entry for New York

//This vcal caused a leave with KerrNotSupported when a new entry
//whose datetime has not been set is used to setalarm
_LIT8(KAlarmVCal,     "BEGIN:VCALENDAR\r\n"
                        "VERSION:1.0\r\n"
                        "BEGIN:VTODO\r\n"
                        "X-EPOCAGENDAENTRYTYPE:TODO\r\n"
                        "SUMMARY:Task2\r\n"
                        "DTSTART:20090520T000000\r\n"
                        "DUE:20090522T000000\r\n"
                        "CLASS:PUBLIC\r\n"
                        "PRIORITY:2\r\n"
                        "AALARM:20090522T080000;;;\r\n"
                        "STATUS:NEEDS ACTION\r\n"
                        "END:VTODO\r\n"
                        "END:VCALENDAR\r\n");

class CTestApp : public CBase , public MCalTestOomCallBack
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void DoImportL(const TDesC8& aVCalendar);
	void DoTodoCompletedImportL(const TDesC8& aVCalendar);
	void DoTZImportL(const TDesC8& aVCalendar, RPointerArray<CCalEntry>& aReadStream);
	void DoSyncMLImportL(const TDesC8& aVCalendar);
	void DoLongSoundNameImportL(const TDesC& aExportFileName, const TDesC8& aVCalendar);
	void OomImportL(const TDesC8& aVCalendarFile);
	void DoImportInvalidAlarmL(const TDesC& aFileName, const TDesC8& aVCalendar);
	void DoTodoNullDueDateImportL();
	
private:
	void ConstructL();
	
public: // from MCalOomTestCallBack
	void OomTestL(TType /*aType*/, TInt /*aFailAt*/);
	
private:
	CCalTestLibrary* iCalTestLib;
	CCalDataExchange* iCalDataExchange;
	
	TDesC8* iVCalendar; // used in oom test
	TDesC* iFileName; // used in oom test
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
	TRAP_IGNORE(iCalTestLib->DeleteFileL(KTestCalendarFile, ETrue));
	delete iCalDataExchange;
	delete iCalTestLib;
	}

void CTestApp::DoImportL(const TDesC8& aVCalendar)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	iCalTestLib->ImportL(aVCalendar, entryArray);
	
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	
	CleanupStack::PopAndDestroy(&entryArray);
	}


void CTestApp::DoTodoCompletedImportL(const TDesC8& aVCalendar)
	{
	test.Next(_L("Importing All entries - doTodoCompletedImportL"));

	
	// Create ptr array for new entries
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	iCalTestLib->ImportL(aVCalendar, entryArray);

	const TInt KSize = entryArray.Count(); //must be one
	for (TInt i(0) ; i < KSize; ++i)
		{
		CCalEntry* entry = entryArray[i];
		test(entry->CompletedTimeL().TimeLocalL() != Time::NullTTime() && (entry->StatusL() == CCalEntry::ETodoCompleted));
		}

	CleanupStack::PopAndDestroy(&entryArray);	// entryArray.Close()
	}

void CTestApp::DoTodoNullDueDateImportL()
 	{
    test.Next(_L("doTodoNullDueDateImportL"));

 	_LIT(KDateString,"%*E%*D%X%*N%*Y %1 %2 '%3");
 	TBuf<KMaxDateStringLength> dateString;
 	
 	CCalIter* calIter = CCalIter::NewL(iCalTestLib->GetSession());
 	CleanupStack::PushL(calIter);
 	
 	TInt iCount = 0;
 	
 	for (TDesC8* guid = &const_cast<TDesC8&>(calIter->FirstL());
 			guid->Compare(KNullDesC8) != 0;
 			guid = &const_cast<TDesC8&>(calIter->NextL()))
 		{
 		test.Printf(_L("Retrieving Start and End Dates"));
 		iCount++;
 		
 		RPointerArray<CCalEntry> entryArray;
 		CleanupResetAndDestroyPushL(entryArray);
 		iCalTestLib->SynCGetEntryViewL().FetchL(*guid, entryArray);
 
 		// Extract EndDate and StartDate
 		TTime tenddate = entryArray[0]->EndTimeL().TimeLocalL();
 		TTime tstartdate = entryArray[0]->StartTimeL().TimeLocalL();
 		if (tstartdate == Time::NullTTime())
 			{
 			test.Printf(_L("Start date is NullTTime"));
 			}
 		else
 			{
 			tstartdate.FormatL(dateString,KDateString);
 		  	test.Printf(_L("Start date is  %S"), &dateString);
 			}
 		if (tenddate == Time::NullTTime())
 			{
 			test.Printf(_L("End date is NullTTime"));
 			}
 		else
 			{
 			tenddate.FormatL(dateString,KDateString);
 		  	test.Printf(_L("End date is  %S"), &dateString);
 			}
 
 		CleanupStack::PopAndDestroy(&entryArray);
 
 		test.Printf(_L("\n"));
 		}
 	CleanupStack::PopAndDestroy(calIter);
 	}
 
void CTestApp::DoTZImportL(const TDesC8& aVCalendar, RPointerArray<CCalEntry>& aEntryArray)
	{
	const TInt KMinPerHour=60;
	// Create array for new entries
	
	iCalTestLib->ImportL(aVCalendar, aEntryArray);

	const TInt KSize = aEntryArray.Count();
	
	for (TInt i(0); i < KSize; ++i)
		{
		CCalEntry* entry = aEntryArray[i];
		
		TTime ttime = entry->StartTimeL().TimeUtcL(); //system local time
		TInt minutes = ttime.DateTime().Hour() * KMinPerHour + ttime.DateTime().Minute();
		test(minutes == 4*KMinPerHour);
	
		ttime = entry->EndTimeL().TimeUtcL(); // system local
		minutes = ttime.DateTime().Hour() * KMinPerHour + ttime.DateTime().Minute();
		test(minutes == 5*KMinPerHour);
		
		CCalAlarm* alarm = entry->AlarmL();
		CleanupStack::PushL(alarm);
		TDateTime alarmTime = (entry->StartTimeL().TimeUtcL() - alarm->TimeOffset()).DateTime();
		test(alarmTime.Hour()==3 && alarmTime.Minute()==50);
		CleanupStack::PopAndDestroy(alarm);
		}

	}


void CTestApp::DoSyncMLImportL(const TDesC8& aVCalendar)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	
	iCalTestLib->ImportL(aVCalendar, entryArray);

	TLocale locale;

	test.Next(_L("Importing1: JAPAN"));

	
	DoTZImportL(aVCalendar, entryArray);

	test.Next(_L("Importing2: UK"));

	DoTZImportL(aVCalendar, entryArray);

	test.Next(_L("Importing3: New York"));

	DoTZImportL(aVCalendar, entryArray);
	
	CleanupStack::PopAndDestroy(&entryArray);

	locale.Set();
	}

/*
In a vCal entry if the sound file name is too long then the given name will be
ignored and alarm entry will be added with empty sound name.

1. Import vCal file with long sound name
2. Export the same entry into another file
3. Open the exported file and check for sound name, should be empty.
*/


void CTestApp::DoLongSoundNameImportL(const TDesC& aExportFileName, const TDesC8& aVCalendar)
	{
	test.Next(_L("Import with too long sound file name"));

	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	iCalTestLib->ImportL(aVCalendar, entryArray);
	
	const TInt KSize = entryArray.Count();
	test(KSize == 1);
	TLocale locale;

	// Export the entry into another file
	iCalTestLib->ExportL(aExportFileName, entryArray);
	locale.Set();

	// Read exported entry and search for sound name, should be empty
	// get the file size
	RFile exportFile;
	iCalTestLib->TestRegister().OpenTempFileLC(exportFile, aExportFileName);
	TInt pos = 0;
	exportFile.Seek(ESeekEnd,pos);
	TInt fileLength = pos;
	pos = 0;
	exportFile.Seek(ESeekStart,pos);

	HBufC8* buff = HBufC8::NewLC(fileLength);
	TPtr8 pBuff(buff->Des());
	exportFile.Read(pBuff,fileLength);
	TInt err = pBuff.Find(KLongSoundNameAlarmExport());
	test(err != KErrNotFound);

	CleanupStack::PopAndDestroy(buff);
	CleanupStack::PopAndDestroy(&exportFile);
	CleanupStack::PopAndDestroy(&entryArray);
	}
	
void CTestApp::OomImportL(const TDesC8& aVCalendar)
	{
	iVCalendar = const_cast<TDesC8*>(&aVCalendar);
	CCalSession& calSession = iCalTestLib->GetSession();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	}
	
void CTestApp::OomTestL(TType /*aType*/, TInt /*aFailAt*/)
	{
	test.Printf(_L("."));
	DoImportL(*iVCalendar);
	}

static void DoMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);

	test.Printf(_L("Import under normal conditions\n"));
	testApp->DoImportL(KPtr);
	
	test.Printf(_L("Import as if from SX_syncml \n"));
	testApp->DoImportL(KAlarmedEntry);
	
	test.Printf(_L("Importing todo completed entry\n"));
	testApp->DoTodoCompletedImportL(KTodoCompletedEntry);

	test.Printf(_L("Importing todo nullduedate entry\n"));
 	testApp->DoTodoNullDueDateImportL();

	test.Printf(_L("Import an empty alarm\n"));
	testApp->DoImportL(KEmptyAlarm);
	
	test.Printf(_L("Missing RRULE property\n"));
	testApp->DoImportL(KMissingRRule);
	
	test.Printf(_L("Import as if from SyncML Server\n"));
	testApp->DoSyncMLImportL(KPtr2);
	
	test.Printf(_L("Import with too long sound file name\n"));
	testApp->DoLongSoundNameImportL(KLongSoundNameExportFile, KLongSoundName);
	
	test.Printf(_L("Import with simulated heap failure\n"));
	testApp->OomImportL(KOOMEntry);
	
	//This vcal caused a leave with KerrNotSupported when a new entry
	//whose datetime has not been set is used to setalarm	
	test.Printf(_L("Import a vcal with alarm information\n"));
	testApp->DoImportL(KAlarmVCal);
	
	CleanupStack::PopAndDestroy(testApp);
	}


/**

@SYMTestCaseID     PIM-T-VCAL2-0001

*/

TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-VCAL2-0001 T_VCAL2"));

	test.Title();
	
	CActiveScheduler* scheduler=new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	
	TRAPD(ret, DoMainL());
	
	test(ret==KErrNone);
	
	delete theCleanup;
	delete scheduler;
	
	test.End();
	test.Close();
	
	__UHEAP_MARKEND;
	return KErrNone;
	}

