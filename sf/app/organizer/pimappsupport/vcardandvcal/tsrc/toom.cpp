// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32test.h>

#include <versit.h>
#include <vcal.h>
#include <vcard.h>

#include <coreappstest/testserver.h>

_LIT(KTestName,"Versit OOM Tests");

LOCAL_D RFs fs;

LOCAL_D RTest test(KTestName);

_LIT8(KVCalendarAllMandatoryProperties,	
"BEGIN:VCALENDAR\r\n" 
"VERSION:1.0\r\n"
"BEGIN:VEVENT\r\n"
"CATEGORIES:APPOINTMENT;MEETING\r\n"
"SUMMARY:Meeting with SyncML team\r\n"
"DESCRIPTION:Check mandatory vCalendar compliance\r\n"
"DTSTART:20010316T170000Z\r\n"
"DTEND:20010316T180000Z\r\n"
"STATUS:NEEDS ACTION\r\n"
"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
//"AALARM:\r\n"
"AALARM:20010607T084500\r\n"
"AALARM;TYPE=X-EPOCSOUND:20000110T084500Z;;0;Chimes\r\n"
"END:VEVENT\r\n"
"BEGIN:VTODO\r\n"
"DESCRIPTION:To-do item\r\n"
"PRIORITY:1\r\n"
"DUE:20010316T000000Z\r\n"
"COMPLETED:20010314T000000Z\r\n"
//"RRULE:\r\n" Standard leave
"RRULE:D1 #0\r\n"
"RRULE:MD1 10+ #0\r\n"
"RRULE:MP1 2+ MO #0\r\n"
"RRULE:MP12 2+ MO #0\r\n"
"RRULE:W1 MO #0\r\n"
"RRULE:YM1 1 #0\r\n"
"RRULE:W2 SU TU TH SA 19991130T000000\r\n"
"RRULE:M1 19990821T000000\r\n" //invalid format from 9110
"CLASS:PUBLIC\r\n"
//"PRIORITY:\r\n" KErrNotFound
"PRIORITY:9\r\n"
"DCREATED:20000113T000000Z\r\n"
"LAST-MODIFIED:20000113T122400Z\r\n"
"UID:28\r\n"
"PRODID:-//Microsoft Corporation//Outlook 9.0 MIMEDIR//EN\r\n"
"TRANSP:1\r\n"
"END:VTODO\r\n"
"END:VCALENDAR\r\n");

_LIT8(KVCardAllMandatoryProperties,
"BEGIN:VCARD\r\n"
"VERSION:2.1\r\n"
"REV:20010608T125751Z\r\n"
"TEL;CAR:\r\n"
"TEL;HOME;VOICE:\r\n"
"TEL;WORK;VOICE:\r\n"
"N:Contact;Starfish;;\r\n"
"NOTE:\r\n"
"TEL;PAGER:\r\n"
//"BDAY:\r\n" standard leave
"ADR;WORK:;;;;;;\r\n"
"ADR;HOME:;;;;;;\r\n"
"URL:\r\n"
"TEL;CELL:\r\n"
"TEL;FAX;HOME:\r\n"
"ORG:\r\n"
"EMAIL;INTERNET:\r\n"
"TEL;FAX;WORK:\r\n"
"END:VCARD\r\n");

_LIT(KVCalImportFileName,"c:\\oomvcal.vcs");
_LIT(KVCalExportFileName,"c:\\oomvcal2.vcs");
_LIT(KVCardImportFileName,"c:\\oomvcard.vcf");
_LIT(KVCardExportFileName,"c:\\oomvcard2.vcf");

/** Defines an interface for Versit OOM tests */
class MVersitOOMTest 
	{
public:
	virtual void DoTestL() = 0;
	};

/** versit alarm creation OOM test */
class CVersitAlarmCreateTest: public CBase, public MVersitOOMTest
	{
public:
	static CVersitAlarmCreateTest* NewLC();
	~CVersitAlarmCreateTest();
public: //From MVersitOOMTest
	void DoTestL();
private:
	CVersitAlarmCreateTest();
	};

/** vCalendar import OOM test */
class CVersitVCalImportTest : public CBase, public MVersitOOMTest
	{
public:
	static CVersitVCalImportTest* NewLC();
	~CVersitVCalImportTest();
public: //From MVersitOOMTest
	void DoTestL();
private:
	void CheckingImportResultL(CParserVCal& aVCalParser);
	CVersitVCalImportTest();
	};


/** vCalendar export OOM test */
class CVersitVCalExportTest : public CBase, public MVersitOOMTest
	{
public:
	static CVersitVCalExportTest* NewLC();
	~CVersitVCalExportTest();
public: //From MVersitOOMTest
	void DoTestL();
private:
	CVersitVCalExportTest();
	};


/** vCard import OOM test */
class CVersitVCardImportTest : public CBase, public MVersitOOMTest
	{
public:
	static CVersitVCardImportTest* NewLC();
	~CVersitVCardImportTest();
public: //From MVersitOOMTest
	void DoTestL();
private:
	CVersitVCardImportTest();
	};

/** vCard export OOM test */
class CVersitVCardExportTest : public CBase, public MVersitOOMTest
	{
public:
	static CVersitVCardExportTest* NewLC();
	~CVersitVCardExportTest();
public: //From MVersitOOMTest
	void DoTestL();
private:
	CVersitVCardExportTest();
	};

// Cleanup callback for versit entities.
GLREF_C void ResetAndDestroyArrayOfEntities(TAny* aObject)
	{
	CArrayPtr<CVersitParser>* array=REINTERPRET_CAST(CArrayPtr<CVersitParser>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}

CVersitAlarmCreateTest* CVersitAlarmCreateTest::NewLC()
	{
	CVersitAlarmCreateTest* self = new(ELeave) CVersitAlarmCreateTest;
	CleanupStack::PushL(self);
	return self;	
	}

void CVersitAlarmCreateTest::DoTestL()
	{
	TDateTime date(2001,ENovember,1,0,15,0,0);
	TVersitDateTime* runtime = new(ELeave) TVersitDateTime(date, TVersitDateTime::EIsMachineLocal);
	CleanupStack::PushL(runtime);
	CVersitAlarm* alarm = CVersitAlarm::NewL(runtime,0,0,_L("Fanfare"),KNullDesC);
	CleanupStack::Pop(runtime);
	delete alarm;
	}

CVersitAlarmCreateTest::~CVersitAlarmCreateTest()
	{
	}

CVersitAlarmCreateTest::CVersitAlarmCreateTest()
	{
	}

CVersitVCalImportTest* CVersitVCalImportTest::NewLC()
	{
	CVersitVCalImportTest* self = new(ELeave) CVersitVCalImportTest;
	CleanupStack::PushL(self);
	return self;
	}

void CVersitVCalImportTest::DoTestL()
	{
	CParserVCal* parser = CParserVCal::NewL();
	CleanupStack::PushL(parser);
	RFile infile;
	User::LeaveIfError(infile.Open(fs,KVCalImportFileName,EFileRead));
	CleanupClosePushL(infile);
	RFileReadStream readStream(infile);
	CleanupClosePushL(readStream);
	parser->InternalizeL(readStream);
	CheckingImportResultL(*parser);
	CleanupStack::PopAndDestroy(3);
	}

void CVersitVCalImportTest::CheckingImportResultL(CParserVCal& aVCalParser)
	{
	//Test get entities and properties without taking ownership
	CArrayPtr<CVersitParser>* entities = aVCalParser.EntityL(KVersitVarTokenVTODO,EFalse);	
	test(entities != NULL);
	CleanupStack::PushL(entities);
	CArrayPtr<CParserProperty>* properties = (*entities)[0]->PropertyL(KVersitTokenRRULE, TUid::Uid(KVCalPropertyRecurrenceUid), EFalse);
	test(properties != NULL);
	delete properties;
	CleanupStack::PopAndDestroy(entities);
		
	//Test get entities and properties with taking ownership
	entities = aVCalParser.EntityL(KVersitVarTokenVEVENT,ETrue);
	test(entities != NULL);
	CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfEntities, entities));
	properties = (*entities)[0]->PropertyL(KVersitTokenCATEGORIES, TUid::Uid(KVersitPropertyCDesCArrayUid), ETrue);
	test(properties != NULL);
	properties->ResetAndDestroy();
	delete properties;
	CleanupStack::PopAndDestroy(entities);
	}

CVersitVCalImportTest::CVersitVCalImportTest()
	{
	}

CVersitVCalImportTest::~CVersitVCalImportTest()
	{
	}

CVersitVCalExportTest* CVersitVCalExportTest::NewLC()
	{
	CVersitVCalExportTest* self = new(ELeave) CVersitVCalExportTest;
	CleanupStack::PushL(self);
	return self;
	}

CVersitVCalExportTest::~CVersitVCalExportTest()
	{
	}

void CVersitVCalExportTest::DoTestL()
	{
	CParserVCal* parser = CParserVCal::NewL();
	CleanupStack::PushL(parser);
	CParserVCalEntity* entity =  CParserVCalEntity::NewL();	
	CleanupStack::PushL(entity);
	entity->SetEntityNameL(KVersitVarTokenVEVENT);

	TInt KUidValue=0;
	CParserPropertyValueInt* intValue= new(ELeave) CParserPropertyValueInt(KUidValue);
	CleanupStack::PushL(intValue);
	CParserProperty* UIDprop = CParserProperty::NewL(*intValue, KVersitTokenUID, NULL);  
	CleanupStack::Pop(intValue);	
	entity->AddPropertyL(UIDprop);
	
	_LIT(KVCalDescription,"Small devices come in many shapes and sizes");
	CParserPropertyValueHBufC* hbufValue = CParserPropertyValueHBufC::NewL(KVCalDescription);
	CleanupStack::PushL(hbufValue);
	CParserProperty* DESCRIPTIONprop = CParserProperty::NewL(*hbufValue, KVersitTokenDESCRIPTION, NULL);
	CleanupStack::Pop(hbufValue);
	entity->AddPropertyL(DESCRIPTIONprop);

	CleanupStack::Pop(entity);
	parser->AddEntityL(entity);

	RFile outfile;
	User::LeaveIfError(outfile.Replace(fs,KVCardExportFileName,EFileWrite));
	CleanupClosePushL(outfile);
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	parser->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(3);
	}

CVersitVCalExportTest::CVersitVCalExportTest()
	{
	}

CVersitVCardImportTest* CVersitVCardImportTest::NewLC()
	{
	CVersitVCardImportTest* self = new(ELeave) CVersitVCardImportTest;
	CleanupStack::PushL(self);
	return self;
	}

void CVersitVCardImportTest::DoTestL()
	{
	CParserVCard* parser = CParserVCard::NewL();
	CleanupStack::PushL(parser);
	RFile infile;
	User::LeaveIfError(infile.Open(fs,KVCardImportFileName,EFileRead));
	CleanupClosePushL(infile);
	RFileReadStream readStream(infile);
	CleanupClosePushL(readStream);
	parser->InternalizeL(readStream);
	CleanupStack::PopAndDestroy(3);
	}

CVersitVCardImportTest::CVersitVCardImportTest()
	{
	}

CVersitVCardImportTest::~CVersitVCardImportTest()
	{
	}

CVersitVCardExportTest* CVersitVCardExportTest::NewLC()
	{
	CVersitVCardExportTest* self = new(ELeave) CVersitVCardExportTest;
	CleanupStack::PushL(self);
	return self;
	}

CVersitVCardExportTest::~CVersitVCardExportTest()
	{
	}

void CVersitVCardExportTest::DoTestL()
	{
	CParserVCard* parser = CParserVCard::NewL();
	CleanupStack::PushL(parser);

	TTime lastModified;
	lastModified.UniversalTime();
	TVersitDateTime* dateTime=new(ELeave)TVersitDateTime(lastModified.DateTime(),TVersitDateTime::EIsMachineLocal);
	CleanupStack::PushL(dateTime);
	CParserPropertyValueDateTime* valueDateTime=new(ELeave) CParserPropertyValueDateTime(dateTime);
	CleanupStack::Pop(dateTime);	
	CleanupStack::PushL(valueDateTime);
	CParserGroupedProperty* property=CParserGroupedProperty::NewL(*valueDateTime,KVersitTokenREV,NULL,NULL);
	CleanupStack::Pop(valueDateTime);
	parser->AddPropertyL(property);
	
	RFile outfile;
	User::LeaveIfError(outfile.Replace(fs,KVCalExportFileName,EFileWrite));
	CleanupClosePushL(outfile);
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	parser->ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(3);
	}

CVersitVCardExportTest::CVersitVCardExportTest()
	{
	}

void runOOMTestL(MVersitOOMTest& aTest)
	{
	TInt ret=KErrNoMemory;
	TInt failAt=0;
	while (ret!=KErrNone)
		{
		failAt++;
		TBuf<200>indicator;
		indicator.Format(_L("Memory fail at \t%d\x0d\x0a"),failAt);
		if(failAt%100==0)
			test.Printf(indicator);
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAP(ret, aTest.DoTestL());
		__UHEAP_MARKEND;
		__UHEAP_RESET;
		if (ret!=KErrNoMemory&&ret!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),ret);
			test.Getch();
			}
		test(ret==KErrNoMemory||ret==KErrNone);
		}
	}

LOCAL_C void ExpandCleanupStackL()
	{
	// reserve some space on the cleanup stack:
	{for (TInt ii=0;ii<1000;++ii)
		CleanupStack::PushL(&ii);}
	CleanupStack::Pop(1000);
	}
void createTestFilesL()
	{
	RFile file;
	TInt err=file.Replace(fs,KVCalImportFileName, EFileWrite+EFileShareAny+EFileStreamText);
	User::LeaveIfError(err);
	file.Write(KVCalendarAllMandatoryProperties);
	file.Close();

	err=file.Replace(fs,KVCardImportFileName, EFileWrite+EFileShareAny+EFileStreamText);
	User::LeaveIfError(err);
	file.Write(KVCardAllMandatoryProperties);
	file.Close();
	}

/**
@SYMTestCaseID PIM-TOOM-0001
*/	
void runOOMTestsL()
	{
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);	

	CTestRegister* TempFiles = CTestRegister::NewLC();
	TempFiles->RegisterL(KVCalImportFileName);
	TempFiles->RegisterL(KVCalExportFileName);
	TempFiles->RegisterL(KVCardImportFileName);
	TempFiles->RegisterL(KVCardExportFileName);

	test.Start(_L("@SYMTestCaseID PIM-TOOM-0001 Test One"));
	createTestFilesL();
	ExpandCleanupStackL();

	_LIT(KTestvCalImport,"vCal Import Test\n");
	test.Printf(KTestvCalImport);
	CVersitVCalImportTest* oomtest1 = CVersitVCalImportTest::NewLC();
	runOOMTestL(*oomtest1);
	CleanupStack::PopAndDestroy(oomtest1);
	
	_LIT(KTestvCardImport,"vCard Import Test\n");
	test.Printf(KTestvCardImport);
	CVersitVCardImportTest* oomtest2 = CVersitVCardImportTest::NewLC();
	runOOMTestL(*oomtest2);
	CleanupStack::PopAndDestroy(oomtest2);

	_LIT(KTestvCalExport,"vCal Export Test\n");
	test.Printf(KTestvCalExport);
	CVersitVCalExportTest* oomtest3 = CVersitVCalExportTest::NewLC();
	runOOMTestL(*oomtest3);
	CleanupStack::PopAndDestroy(oomtest3);

	_LIT(KTestvCardExport,"vCard Export Test\n");
	test.Printf(KTestvCardExport);
	CVersitVCardExportTest* oomtest4 = CVersitVCardExportTest::NewLC();
	runOOMTestL(*oomtest4);
	CleanupStack::PopAndDestroy(oomtest4);
	
	_LIT(KTestVersitAlarm,"Versit Alarm OOM Test\n");
	test.Printf(KTestVersitAlarm);
	CVersitAlarmCreateTest* oomtest5 = CVersitAlarmCreateTest::NewLC();
	runOOMTestL(*oomtest5);
	CleanupStack::PopAndDestroy(oomtest5);
	
	test.End();
	test.Close();
	CleanupStack::PopAndDestroy(2); // TempFiles, fs
	}

GLDEF_C TInt E32Main()
	{	
	
	TTime startTime;
	startTime.UniversalTime();
	
	__UHEAP_MARK;

	CTrapCleanup* theCleanup=CTrapCleanup::New();
	TRAPD(ret,runOOMTestsL());
	test(ret==KErrNone);
	delete theCleanup;	

	__UHEAP_MARKEND;
	
	TTime finishTime;
	finishTime.UniversalTime();
	TReal elapsed = (TReal)finishTime.MicroSecondsFrom(startTime).Int64();
	test.Printf(_L("Elapsed time: %.4f\n"), elapsed/1000000);
	
	return(KErrNone);
	}

