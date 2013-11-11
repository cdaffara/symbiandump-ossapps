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

void CVCardTest::GetSampleVersitL()
	{
	if(!iParser)
		iParser = CParserVCard::NewL();
	iVersit.Zero();
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("     "));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("     "));
	iVersit.Append(_L8("BEGIN:vcarD"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ADR:;;\\A street\\\n"));
	iVersit.Append(_L8("two lines;;;;K"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ADR:;;\\A street\\\r\n"));
	iVersit.Append(_L8("two lines;;;;K"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("REV:19981116T165048Z"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("UID:00-e01a767f8aa250-0"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("N;X-EPOCCNTMODELLABEL0=Surname;X-EPOCCNTMODELLABEL1=First-name;X-EPOCFID=0:Godfrey;Steve;"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("TEL;HOME;VOICE;X-EPOCCNTMODELFIELDLABEL=Home-phone;X-EPOCFID=2:0181 357 4332"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("TEL;WORK;VOICE;X-EPOCCNTMODELFIELDLABEL=Work-phone;X-EPOCFID=3:0181 357 4332"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("EMAIL;INTERNET;X-EPOCCNTMODELFIELDLABEL=Email;X-EPOCFID=4:steve@sgsoftware.co.uk"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("ADR;WORK;X-EPOCCNTMODELFIELDLABEL=Work-addr;X-EPOCFID=5:"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("ADR;HOME;X-EPOCCNTMODELFIELDLABEL=Home-addr;X-EPOCFID=6:"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("TEL;WORK;FAX;X-EPOCCNTMODELFIELDLABEL=Work-fax;X-EPOCFID=7:"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("TEL;CELL;X-EPOCCNTMODELFIELDLABEL=Mobile;X-EPOCFID=8:"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("URL;X-EPOCCNTMODELFIELDLABEL=Web-page;X-EPOCFID=9:"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("TITLE;X-EPOCCNTMODELFIELDLABEL=Job-title;X-EPOCFID=10:"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(_L8("ORG;X-EPOCCNTMODELFIELDLABEL=Company;X-EPOCFID=11:"));
	iVersit.Append(KVersitTokenCRLF);
    iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("VERSION:2.1"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("     "));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TEL;HOME:+1-919-555-1234"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("NOTE;ENCODING=QUOTED-PRINTABLE:Don’t remember to order Girl="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8(" Scout cookies from Stacey today!")); //QUOTED-PRINTABLE
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("A.B.C.TEL;HOME:+1-213-555-1234"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("A.NOTE:This is my vacation home."));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PHOTO:http://www.symbianfoundation.example/dir_photos/my_photo.gif"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("SOUND;VALUE=CONTENT-ID:<jsmith.part3.960817T083000.xyzMail@host1.com"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("FN;CHARSET=ISO-8859-8;VALUE=URL;TYPE=GIF:Mr. John Q. Public, Esq."));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("N:Public;John;Quinlan;Mr.;Esq."));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("N:Veni, Vidi, Vici;The Restaurant."));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PHOTO;VALUE=URL:file://jqpublic.gif"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("PHOTO;ENCODING=BASE64;TYPE=GIF:UklGRhAsAABXQVZFZm10IBAAAAABAAEAESsAABErAAABAAgAZGF0YesrAACAg4eCeXR4e3uAhoiIiYmKjIiDfnx5eX6CgoKEhYWDenV5fH6BhISGiIiDfHZ2eXt/hIiKjY2IhH12d3Vyc3uDiIiFf3l7fn18eXl+houFf319fnyAgHl5eoCIiISChIeAfnt2..."));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("BDAY:19950415"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("BDAY:1995-04-15"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ADR;DOM;HOME:P.O. Box 101;Suite 101;123 Main Street;Any Town;CA;91921-1234;"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ADR;DOM;WORK;HOME;POSTAL:P.O. Box 101;;;Any Town;CA;91921-1234;"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("LABEL;DOM;POSTAL;ENCODING=QUOTED-PRINTABLE:P. O. Box 456=0D=0A="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8(" 123 Main Street=0D=0A="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("Any Town, CA 91921-1234"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("LABEL;INTL;PARCEL;ENCODING=QUOTED-PRINTABLE:Suite 101=0D=0A="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8(" 123 Main Street=0D=0A="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("Any Town, CA 91921-1234=0D=0A="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("U.S.A.")); //QUOTED PRINTABLE
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TEL;PREF;WORK;MSG;FAX:+1-800-555-1234"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TEL;WORK;HOME;VOICE;FAX:+1-800-555-1234"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("EMAIL;INTERNET:symbian.foundation@exp.example.test"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("MAILER:ccMail 2.2"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TZ:-0500"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TZ:-08:00"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("GEO:37.24,-17.87"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TITLE:V.P., Research and Development"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ROLE:Executive"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("LOGO;ENCODING=BASE64;TYPE=GIF:UklGRhAsAABXQVZFZm10IBAAAAABAAEAESsAABErAAABAAgAZGF0YesrAACAg4eCeXR4e3uAhoiIiYmKjIiDfnx5eX6CgoKEhYWDenV5fH6BhISGiIiDfHZ2eXt/hIiKjY2IhH12d3Vyc3uDiIiFf3l7fn18eXl+houFf319fnyAgHl5eoCIiISChIeAfnt2"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("AGENT:"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("BEGIN:VCARD"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("VERSION:2.1"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("N:Friday;Fred"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TEL;WORK;VOICE:+1-213-555-1234"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TEL;WORK;FAX:+1-213-555-5678"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("END:VCARD"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("ORG:Symbian Foundation, Inc.;North American Division;Marketing"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("NOTE;ENCODING=QUOTED-PRINTABLE:This facsimile machine if operational="));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8(" 0830 to 1715 hours"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("REV:19951031T222710"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("REV:1995-10-31T22:27:10Z"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("SOUND:JON Q PUBLIK"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("SOUND;VALUE=URL:file//multimed/audio/jqpublic.wav"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("URL:http://symbianfoundation.example/pub/directory/northam/sfpublic.ecd"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("UID:19950401-080045-40000F192713-0052"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("VERSION:2.1"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("X-ABC-VIDEO;MPEG2:http://symbianfoundation.example/anexample.mpg"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("X-DL;X-DesignWorkGroup:List Item 1;List Item 2;List Item 3"));
	iVersit.Append(KVersitTokenCRLF);
#if defined _UNICODE
	iVersit.Append(_L8("N;X-EPOCCNTMODELLABEL0=Last name;X-EPOCCNTMODELLABEL1=First name;X-EPOCCNTMODELLABEL2=Middle name;X-EPOCCNTMODELLABEL3=Title;X-EPOCCNTMODELLABEL4=Suffix;CHARSET=UTF-8;ENCODING=QUOTED-PRINTABLE:Jones;ÄŽÄ…Å•Å•Ä™Å‹;;;"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("TEL;HOME;CELL;X-EPOCCNTMODELFIELDLABEL=CÄ™llphone:1234"));
	iVersit.Append(KVersitTokenCRLF);

	// Added to test ISO-X export
	iVersit.Append(_L8("NOTE:Testing  (255) ®(169) ¦(177) ­(240) Ã(199) ¦(180) ü(129) â(131)"));
	iVersit.Append(KVersitTokenCRLF);
#endif
	iVersit.Append(_L8("END:VCARD"));
	iVersit.Append(KVersitTokenCRLF);
	iVersit.Append(_L8("Randomnoisewe'renot(yet)interestedin"));
	}
