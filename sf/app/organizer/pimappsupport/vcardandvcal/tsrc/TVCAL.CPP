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
 
#include "TVERSIT.H"

void CVCalTest::GetSampleVersitL()
	{
	if(!iParser)
		iParser = CParserVCal::NewL();
	iVersit.Zero();
	iVersit.Append(_L8(" BEgIN : vcalendar"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("VERSION:1.0"));
	iVersit.Append(KVersitTokenCRLF);
#if defined _UNICODE
	iVersit.Append(_L8("DAYLIGHT;CHARSET=UTF-8:TRUE;-06;19990707T025959;20000427T010000;ŚmĬth;EDT"));
	iVersit.Append(KVersitTokenCRLF);
#endif
	iVersit.Append(_L8("DAYLIGHT:FALSE"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("GEO:37.24,-17.87"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PRODID:-//Symbian Foundation Corporation//NONSGML My Product//EN"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TZ:-05"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TZ:+05:30"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("BEGIN:VTODO"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RRULE:MD12 #0"));
	iVersit.Append(KVersitTokenCRLF);
#if defined _UNICODE
	iVersit.Append(_L8("MALARM;CHARSET=UTF-8:66660416T000000;PT1H;24;IRS@us.gov;JÒhÑ ŚmĬth - the Check Is In The Mail!"));
	iVersit.Append(KVersitTokenCRLF);
#endif
	iVersit.Append(_L8("DESCRIPTION;;;CHARSET=ISO-8859-8;ENCODING=QUOTED-PRINTABLE:Don�t forget to order Girl="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8(" Scout cookies from Stacey today!"));
	iVersit.Append(KVersitTokenCRLF);   //QUOTED-PRINTABLE
	iVersit.Append(_L8("DESCRIPTION;ENCODING=QUOTED-PRINTABLE:Meeting Arrangement=0D=0A=0D=0A9:30 -="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8(" 10:30 Meeting room 1 with Example1 and Example2.=0D=0A=0D=0A"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8(" Scout cookies from Stacey today!"));
	iVersit.Append(KVersitTokenCRLF);   //QUOTED-PRINTABLE
	iVersit.Append(_L8("DESCRIPTION:This is a very very very very very very very very very  very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very very long line of text"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DESCRIPTION:This is a very very very very very very very very very very very very very"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ATTACH;VALUE=CONTENT-ID:<anexample.part3.960817T083000.sfMail@host1.com>"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ATTACH;VALUE=URL:file://symbianfoundation.example/pub/reports/r-960812.ps"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ATTENDEE;ROLE=OWNER;STATUS=COMPLETED:anexample@host1.com"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ATTENDEE;ROLE=OWNER;STATUS=CONFIRMED:A Example <aexample@host1.com>"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ATTENDEE;ROLE=ATTENDEE;STATUS=TENTATIVE:AN Example <anexample@host2.com>"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ATTENDEE;ROLE=DELEGATE;STATUS=CONFIRMED:B Example <bexample@host1.com>"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ATTENDEE;ROLE=ATTENDEE;STATUS=CONFIRMED;VALUE=URL;TYPE=VCARD:http://www.xyz.com/~myvcard.vcf"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("AALARM;TYPE=WAVE;VALUE=URL:20000101T000000; ; ; file://mmedia/taps.wav"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("AALARM;TYPE=WAVE;VALUE=CONTENT-ID:20000903T060000;PT15M;4;<example.part2.=960901T083000.sfMail@host1.com>"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("CATEGORIES:APPOINTMENT;EDUCATION"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("CATEGORIES:MEETING"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("CLASS:PUBLIC"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DCREATED:19991231T235959"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("COMPLETED:20000101T000000"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DALARM:29990415T235000;PT5M;2;Your Taxes Are Due !!!"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DUE:40240401T235959Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DTEND:99991231T235959Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXDATE:19990402T010000Z;20000403T010000Z;20010404T010000Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:W2 TU TH #2"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:D1 #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:YM1 6 7 #8"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:D1 #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:D1 19941224T000000Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:W1 #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:W1 20261224T000000Z"));
	iVersit.Append(KVersitTokenCRLF); 
	iVersit.Append(_L8("EXRULE:W2 #0"));
	iVersit.Append(KVersitTokenCRLF);  
	iVersit.Append(_L8("EXRULE:W2 MO WE FR 20001224T000000Z"));
	iVersit.Append(KVersitTokenCRLF); 
	iVersit.Append(_L8("EXRULE:MP1 1+ FR #10"));
	iVersit.Append(KVersitTokenCRLF);  
	iVersit.Append(_L8("EXRULE:MP1 1+ FR 19991231T235959Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:MP2 1+ SU 1- SU #10"));
	iVersit.Append(KVersitTokenCRLF);  
	iVersit.Append(_L8("EXRULE:MP6 2+ MO TU WE TH FR #10"));
	iVersit.Append(KVersitTokenCRLF); 
	iVersit.Append(_L8("EXRULE:MP1 2- MO #6"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:MD1 3- #0"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:MD1 2 15 #10"));
	iVersit.Append(KVersitTokenCRLF); 
	iVersit.Append(_L8("EXRULE:MD1 1 LD #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:MD1 1 1- #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:MD18 10 11 12 13 14 15 #10"));
	iVersit.Append(KVersitTokenCRLF); 
	iVersit.Append(_L8("EXRULE:MD1 2- #5"));
	iVersit.Append(KVersitTokenCRLF);  
	iVersit.Append(_L8("EXRULE:YM1 6 7 #10"));
	iVersit.Append(KVersitTokenCRLF);  
	iVersit.Append(_L8("EXRULE:YM2 1 2 3 #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EXRULE:YD3 1 100 200 #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RRULE:W2 TU TH"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RRULE:D1 #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RRULE:YM1 6 7 #8"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("LAST-MODIFIED:19960817T133000Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("LOCATION:Conference Room - F123, Bldg. 002"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("LOCATION;VALUE=URL;TYPE=VCARD:http://www.symbianfoundation.example/~example.vcf"));
	iVersit.Append(KVersitTokenCRLF);

	iVersit.Append(_L8("RNUM:3"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PRIORITY:2"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PALARM;VALUE=URL:80980415T235000;P2Y10M15DT10H30M20S;2;file://myapps/shockme.exe"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PALARM;VALUE=URL:80990415T235000;PT1H30M;2;file://myapps/shockme.exe"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PALARM;VALUE=URL:81000415T235000;P6W;2;file://myapps/shockme.exe"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PALARM;VALUE=URL:81010415T235000;PT5M;2;file://myapps/shockme.exe"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RELATED-TO:<example.part7.19960817T083000.sfMail@host3.com>"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RELATED-TO:19960401-080045-4000F192713-0052"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RDATE:19960402T010000Z;19960403T010000Z;19960404T010000Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RESOURCES:EASEL;PROJECTOR;VCR"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("SEQUENCE:1"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DTSTART:19960401T235959"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("STATUS:TENTATIVE"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("SUMMARY;LANGUAGE=fr-CA:Department Party"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TRANSP:0"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("URL:http://symbianfoundation.example/pub/calendars/example/mytime.or3"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("UID:19960401-080045-4000F192713-0052"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("X-ABC-MMSUBJ;TYPE=WAV; VALUE=URL: http://symbianfoundation.example/mysubj.wav"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("END:VTODO"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("BEGIN:VEVENT"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DESCRIPTION;charset=AScii;lanGuage=SPanish : sausages"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DTSTART ; ENCODING=7-Bit: 20001127T0930"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("DTEND;ENCODING=8-biT : 20101127T100000Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("RRULE: D1 #10"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("UID:12345"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("X-PSION: 0"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("END : VEVENT"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("END:VCALENDAR"));
	iVersit.Append(KVersitTokenCRLF);
	}
