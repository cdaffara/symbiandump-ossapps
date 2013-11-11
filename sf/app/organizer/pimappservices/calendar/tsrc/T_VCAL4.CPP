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
#include <e32std.h>
#include <s32file.h>
#include <s32mem.h>

#include <caldataexchange.h>
#include <calentryview.h>
#include <calrrule.h>
#include <calalarm.h>
#include <caliterator.h>
#include <caldataformat.h>

#include <coreappstest/testserver.h>
#include "caltestlib.h"

// This testcode reproduces defect EDNSAID-4A6HSS (ER5 defects database)
// "Time is being shifted at IrDA Transfer"
// 
// In order to test that this defect is fixed, a vCalendar stream containing 
// 3 appointments and a to-do item is exported.  This provides 100% coverage of 
// all date/time properties used in Agenda Model.  The following properties are
// exported : LAST-MODIFIED, DTSTART, DTEND, DUE, AALARM, EXDATE, RRULE.
//
// To check that the test was successful, the localised vCalendar stream is imported
// and the entries are compared to the original entry definitions to check they have
// the same start/end times etc.  These files can also be inspected manually.  They 
// should be identical, other than minor differences in the last modified date.   

static RTest test(_L("T_VCAL4"));

_LIT(KFrenchFile,"c:\\french.vcs");
_LIT(KHomeFile,"c:\\home.vcs");
_LIT(KJapanFile,"c:\\japan.vcs");
_LIT(KNewYorkFile,"c:\\newyork.vcs");
_LIT(KTempFileName,"c:\\temp.vcs");
_LIT(KUkFile,"c:\\uk.vcs");


_LIT8(KAsiaTokyo, "Asia/Tokyo");
_LIT8(KEuropeLondon, "Europe/London");
_LIT8(KAmericaNewYork, "America/New_York");
	
_LIT8(KGuidAppt,		"t_vcal_guid_appt");
_LIT8(KGuidAlarmAppt,	"t_vcal_guid_alarmappt");
_LIT8(KGuidTodo,		"t_vcal_guid_todo");
_LIT8(KGuidAnniv,		"t_vcal_guid_anniv");

_LIT(KTestCalendarFile,		"c:t_vcal4");

class CTestApp : public CBase
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void CreateToDoL();
	void CreateAppointmentL();
	void CreateAnniversaryEntryL();
	void CreateTimedAlarmedApptEntryL();
	void ExportAsVCalendarL(const TDesC& aVCalendarStream, RPointerArray<CCalEntry>& aEntries, const TDesC8& aTimeZone);
	void ExportAsVCalendarL(const TDesC& aVCalendarStream);
	void DoSyncMLExportTestL();
	void SummaryOrDescriptionOnlyTestL();
	void CheckLastChangedDateOnVcalImportL();
	void CleanFileL();
	void ImportAsVCalendarL(const TDesC& aVCalendarStream);
	void DeleteFile(const TDesC& aFileName);
	void CheckEntriesL();
	TBool AreFilesToBeDeleted() //Leave files in the c drive if there is an error for debugging purposes
	    {
		return iDeleteFiles;
	    }


private:
    CTestApp();
	void ConstructL();

	TInt Diff(RFile anInFile, RFile anOutFile);
	void CheckOutputL(const TDesC& aFile, const TDesC& aFile2);
	
	TInt ContainsTextL(const TDesC& aFileName, const TDesC8& aText);

private:
	CCalTestLibrary* iCalTestLib;
	
	TCalTime iToday;
	TBool iDeleteFiles;
	};
	
CTestApp* CTestApp::NewL()
	{
	CTestApp* self = new(ELeave) CTestApp;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestApp::DeleteFile(const TDesC& aFileName)
	{
	iCalTestLib->FileSession().Delete(aFileName);
	}
	
void CTestApp::ConstructL()
	{
	iCalTestLib = CCalTestLibrary::NewL();
	iCalTestLib->ReplaceFileL(KTestCalendarFile);
	iCalTestLib->OpenFileL(KTestCalendarFile);
	
	iToday.SetTimeLocalL(TTime(_L("20051123:120000.000000"))); // 12:00 23rd November 2005
	}
CTestApp::CTestApp() : 	iDeleteFiles(ETrue)
{}
		
CTestApp::~CTestApp()
	{
	delete iCalTestLib;
	}
	
void CTestApp::CleanFileL()
	{
	iCalTestLib->CleanDatabaseL();
	}


TInt CTestApp::Diff(RFile anInFile, RFile anOutFile)
	{
	TBuf8<256> inBuf;
	TBuf8<256> outBuf; 
	do	{
		inBuf.SetLength(0);
		outBuf.SetLength(0);
		anInFile.Read(inBuf, 256);
		anOutFile.Read(outBuf, 256);
		TInt err = inBuf.Compare(outBuf);
		if (err)
			{
		    iDeleteFiles = EFalse;
			return err;
			}
		}
	while (inBuf.Length() && outBuf.Length());
	return KErrNone;
	}

void CTestApp::CheckOutputL(const TDesC& aFile, const TDesC& aFile2)
	{
	RFile inFile;
	User::LeaveIfError(inFile.Open(iCalTestLib->FileSession(), aFile, EFileRead));
	CleanupClosePushL(inFile);
	RFile outFile;
	User::LeaveIfError(outFile.Open(iCalTestLib->FileSession(), aFile2, EFileRead));
	CleanupClosePushL(outFile);
	TInt err = Diff(inFile, outFile);
	inFile.Close();
	outFile.Close();
	test(err==KErrNone);
	CleanupStack::PopAndDestroy(2);
	}

// Create a to-do item, with a due date of five days after the testcode is run.
//
void CTestApp::CreateToDoL()
	{
	HBufC8* guid = KGuidTodo().AllocLC();
	CCalEntry* todo = iCalTestLib->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(todo);
	
	TCalTime startTime;
	startTime.SetTimeLocalL(iToday.TimeLocalL() + TTimeIntervalDays(5));
	TCalTime endTime;
	endTime.SetTimeLocalL(iToday.TimeLocalL() + TTimeIntervalDays(6));
	
	todo->SetStartAndEndTimeL(startTime, endTime);
	todo->SetSummaryL(_L("A to-do entry"));
	
	todo->SetCompletedL(ETrue, iToday);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(todo);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	CleanupStack::PopAndDestroy(); // entryArray.close()
	CleanupStack::PopAndDestroy(todo);
	}


// Create a one hour appointment between 9-10am on the day the testcode is run.
//
void CTestApp::CreateAppointmentL()
	{
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(iToday.TimeLocalL().DateTime().Year(), iToday.TimeLocalL().DateTime().Month(), iToday.TimeLocalL().DateTime().Day(),9,0,0,0));
	TCalTime endTime;
	endTime.SetTimeLocalL(startTime.TimeLocalL() + TTimeIntervalMinutes(60));
	HBufC8* guid = KGuidAppt().AllocLC();
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(startTime, endTime);
	entry->SetSummaryL(_L("An Appointment"));
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(entry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	CleanupStack::PopAndDestroy(); // entryArray.close()
	CleanupStack::PopAndDestroy(entry);
	}

// Create a repeating entry.
// The repeat is set for yearly on June 24th starting in 1998, with the exception of 24/06/2000
//
void CTestApp::CreateAnniversaryEntryL()
	{
	HBufC8* guid = KGuidAnniv().AllocLC();
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAnniv, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	entry->SetSummaryL(_L("Anniversary of company"));
	
	TCalTime symbianCreation;
	symbianCreation.SetTimeLocalL(TTime(TDateTime(1998,EJune,24,0,0,0,0)));
	
	entry->SetStartAndEndTimeL(symbianCreation, symbianCreation);
	
	TCalRRule rRule(TCalRRule::EYearly);
	rRule.SetDtStart(symbianCreation);
	rRule.SetInterval(1);
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	rRule.SetUntil(maxTime);
	entry->SetRRuleL(rRule);
	
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	TCalTime exception;
	exception.SetTimeLocalL(TTime(TDateTime(2000,EJune,24,0,0,0,0)));
	exceptions.AppendL(exception);

	entry->SetExceptionDatesL(exceptions);
	
	CleanupStack::PopAndDestroy(); // exceptions.close()
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(entry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	CleanupStack::PopAndDestroy(); // entryArray.close()
	CleanupStack::PopAndDestroy(entry); // entry
	}


// Create an alarmed appointment
//
void CTestApp::CreateTimedAlarmedApptEntryL()
	{
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(iToday.TimeLocalL().DateTime().Year(), iToday.TimeLocalL().DateTime().Month(), iToday.TimeLocalL().DateTime().Day(),10,0,0,0));
	TCalTime endTime;
	endTime.SetTimeLocalL(startTime.TimeLocalL() + TTimeIntervalMinutes(90));
	HBufC8* guid = KGuidAlarmAppt().AllocLC();
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(startTime, endTime);
	
	entry->SetSummaryL(_L("app engines weekly meeting APPOINTMENT"));
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetAlarmSoundNameL(_L("Fanfare"));
	alarm->SetTimeOffset(TTimeIntervalMinutes(15));// entry->SetAlarm(0,15);
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(entry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	CleanupStack::PopAndDestroy(); // entryArray.close()
	
	CleanupStack::PopAndDestroy(entry);
	}

// Export vCalendar file
//
void CTestApp::ExportAsVCalendarL(const TDesC& aVCalendarStream)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	CCalIter* calIter = CCalIter::NewL(iCalTestLib->GetSession());
	CleanupStack::PushL(calIter);
	
	for (TDesC8* guid = const_cast<TDesC8*>(&calIter->FirstL());
			guid->CompareF(KNullDesC8) != 0;
			guid = const_cast<TDesC8*>(&calIter->NextL()))
		{
		iCalTestLib->SynCGetEntryViewL().FetchL(*guid, entryArray);
		}
		
	CleanupStack::PopAndDestroy(calIter);
	
	RFile outFile;
	User::LeaveIfError(outFile.Replace(iCalTestLib->FileSession(), aVCalendarStream, EFileWrite));
	RFileWriteStream writeStream(outFile);
	
	iCalTestLib->DataExchangeL().ExportL(KUidVCalendar, writeStream, entryArray);
	writeStream.Close();
	outFile.Close();
	CleanupStack::PopAndDestroy(); // entryArray.Close()
	}

// Import vCalendar stream
//

void CTestApp::ImportAsVCalendarL(const TDesC& aVCalendarStream)
	{
	test.Next(_L("Importing all entries"));

	RFile infile;
	infile.Open(iCalTestLib->FileSession(), aVCalendarStream, EFileRead);
	RFileReadStream readStream(infile);

	// Create ptr array for new entries
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iCalTestLib->DataExchangeL().ImportL(KUidVCalendar, readStream, entryArray);

	readStream.Close();
	infile.Close();
	
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	
	CleanupStack::PopAndDestroy(&entryArray); // entryArray.Close()
	}

void CTestApp::CheckLastChangedDateOnVcalImportL()
	{
	test.Next(_L("Check last changed date on vcard import..."));


	_LIT8(KVcal,	"BEGIN:VCALENDAR\r\n"
					"VERSION:1.0\r\n"
					"BEGIN:VEVENT\r\n"
					"UID:4\r\n"
					"SUMMARY:test\r\n"
					"DTSTART:20021120T090000Z\r\n"
					"DTEND:20021120T100000Z\r\n"
					"X-EPOCAGENDAENTRYTYPE:APPOINTMENT\r\n"
					"CLASS:PUBLIC\r\n"
					"LAST-MODIFIED:20021120T152700Z\r\n"
					"PRIORITY:0\r\n"
					"STATUS:NEEDS ACTION\r\n"
					"END:VEVENT\r\n"
					"END:VCALENDAR\r\n");
	
	_LIT(KDateTime,"20/11/2002 15:27:00");
	TTime lastChangedDateExpected;
	lastChangedDateExpected.Parse(KDateTime);
	
	TBuf<30> dateString;
	TBuf<30> timeString;
	
	_LIT(KExpectedDateFormat,"Expected date: %S %S\n");
	_LIT(KImportedDateFormat,"Imported date: %S %S\n");
	_LIT(KDateString,"%D%M%Y%/0%1%/1%2%/2%3%/3");
	_LIT(KTimeString,"%-B%:0%J%:1%T%:2%S%:3%+B");

	lastChangedDateExpected.FormatL(dateString,KDateString);
	lastChangedDateExpected.FormatL(timeString,KTimeString);
	
	test.Printf(KExpectedDateFormat,&dateString,&timeString);
	
	RDesReadStream readStream(KVcal);
	CleanupClosePushL(readStream);

	// Create ptr array for new entries
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	//Import vcal
	iCalTestLib->DataExchangeL().ImportL(KUidVCalendar, readStream, entryArray);

	test(entryArray.Count() == 1);
	CCalEntry* entry = entryArray[0];
	
	//Print last changed date from the entry imported
	entry->LastModifiedDateL().TimeUtcL().FormatL(dateString,KDateString);
	entry->LastModifiedDateL().TimeUtcL().FormatL(timeString,KTimeString);
	test.Printf(KImportedDateFormat,&dateString,&timeString);
	
	//Check last changed date
	test(entry->LastModifiedDateL().TimeUtcL() == lastChangedDateExpected);
	
	//now add the entry 
	RPointerArray<CCalEntry> storeEntryArray;
	CleanupClosePushL(storeEntryArray);
	storeEntryArray.AppendL(entry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(storeEntryArray, numSuc);
	CleanupStack::PopAndDestroy(); // entryArray.close()
	
	//Check that the add entry hasn't changed the date
	test(entry->LastModifiedDateL().TimeLocalL() == lastChangedDateExpected); 
	lastChangedDateExpected=entry->LastModifiedDateL().TimeLocalL();
	
	// test that an update does change the last modified date
	RPointerArray<CCalEntry> updateEntryArray;
	CleanupClosePushL(updateEntryArray);
	updateEntryArray.AppendL(entry);
	iCalTestLib->SynCGetEntryViewL().UpdateL(updateEntryArray, numSuc);
	CleanupStack::PopAndDestroy(); // updateEntryArray.close()
	TTime fetchedChangedDate = entry->LastModifiedDateL().TimeLocalL();
	test(fetchedChangedDate != lastChangedDateExpected); 

	CleanupStack::PopAndDestroy(2,&readStream); //entryArray
	}


// Check that the entries are correct when they are imported
//
void CTestApp::CheckEntriesL()
	{	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	
	// todo
	iCalTestLib->SynCGetEntryViewL().FetchL(KGuidTodo, entryArray);
	test(entryArray.Count() == 1);
	test.Printf(_L("Testing the todo; %S\n"), &entryArray[0]->SummaryL());
	test(entryArray[0]->EntryTypeL() == CCalEntry::ETodo);
	test(entryArray[0]->StartTimeL().TimeUtcL() == TTime(TDateTime(2005, EDecember, 28, 12, 0, 0, 0)));
	test(entryArray[0]->EndTimeL().TimeUtcL() == TTime(TDateTime(2005, EDecember, 29, 12, 0, 0, 0)));
	test(entryArray[0]->CompletedTimeL().TimeUtcL() == TTime(TDateTime(2005, EDecember, 23, 12, 0, 0, 0)));
	
	// anniv
	entryArray.ResetAndDestroy();
	iCalTestLib->SynCGetEntryViewL().FetchL(KGuidAnniv, entryArray);
	test(entryArray.Count() == 1);
	test.Printf(_L("Testing the anniv; %S\n"), &entryArray[0]->SummaryL());
	test(entryArray[0]->EntryTypeL() == CCalEntry::EAnniv);
	test(entryArray[0]->StartTimeL().TimeUtcL() == TTime(TDateTime(1998,EJune,23,23,0,0,0)));
	test(entryArray[0]->EndTimeL().TimeUtcL() == TTime(TDateTime(1998,EJune,23,23,0,0,0)));
	
	// appt
	entryArray.ResetAndDestroy();
	iCalTestLib->SynCGetEntryViewL().FetchL(KGuidAppt, entryArray);
	test(entryArray.Count() == 1);
	test.Printf(_L("Testing the appt; %S\n"), &entryArray[0]->SummaryL());
	test(entryArray[0]->EntryTypeL() == CCalEntry::EAppt);
	test(entryArray[0]->StartTimeL().TimeUtcL() == TTime(TDateTime(2005, EDecember, 23, 9, 0, 0, 0)));
	test(entryArray[0]->EndTimeL().TimeUtcL() == TTime(TDateTime(2005, EDecember, 23, 10, 0, 0, 0)));
	
	// alarmed appt
	entryArray.ResetAndDestroy();
	iCalTestLib->SynCGetEntryViewL().FetchL(KGuidAlarmAppt, entryArray);
	test(entryArray.Count() == 1);
	test.Printf(_L("Testing the alarmed appt; %S\n"), &entryArray[0]->SummaryL());
	test(entryArray[0]->EntryTypeL() == CCalEntry::EAppt);
	test(entryArray[0]->StartTimeL().TimeUtcL() == TTime(TDateTime(2005, EDecember, 23, 10, 0, 0, 0)));
	test(entryArray[0]->EndTimeL().TimeUtcL() == TTime(TDateTime(2005, EDecember, 23, 11, 30, 0, 0)));
	
	CCalAlarm* alarm = entryArray[0]->AlarmL();
	CleanupStack::PushL(alarm);
	test(alarm != NULL);
	test(alarm->TimeOffset() == TTimeIntervalMinutes(15));
	CleanupStack::PopAndDestroy(alarm);
	
	CleanupStack::PopAndDestroy(&entryArray);	
	}

//
void CTestApp::ExportAsVCalendarL(const TDesC& aVCalendarStream, RPointerArray<CCalEntry>& aEntries, const TDesC8& aTimeZone)
	{
    RPIMTestServer serv3;
    User::LeaveIfError(serv3.Connect());
    serv3.SetTimeZoneL(aTimeZone);

	TTime time;
	time.HomeTime();
	TDateTime dateTime=time.DateTime();
	dateTime.SetMinute(0);
	dateTime.SetSecond(0);
	dateTime.SetMicroSecond(0);
	time=dateTime;
	
    serv3.SetHomeTime(time);
    serv3.Close();

	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().UpdateL(aEntries, numSuc);
	
	RFile outFile;
	User::LeaveIfError(outFile.Replace(iCalTestLib->FileSession(), aVCalendarStream, EFileWrite));
	RFileWriteStream writeStream(outFile);
	
	iCalTestLib->DataExchangeL().ExportL(KUidVCalendar, writeStream, aEntries);
	writeStream.Close();
	outFile.Close();
	}

// Create two entries (summer and winter) and export in different timezones as if to SyncML Server.
//

void CTestApp::DoSyncMLExportTestL()
	{
	const TInt Hours=10;
	const TInt MinsPerHour=60;
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	test.Next(_L("Checking SyncML Exports"));

	
	//Create 1st entry (with alarm)
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(iToday.TimeLocalL().DateTime().Year(),EFebruary,2,Hours,0,0,0));
	TCalTime endTime;
	endTime.SetTimeLocalL(startTime.TimeLocalL() + TTimeIntervalMinutes(60));
	HBufC8* guid1 = NULL;
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid1);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(startTime,endTime);
	entry->SetSummaryL(_L("Winter"));
	
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(TTimeIntervalMinutes(Hours*MinsPerHour-5));
	entry->SetAlarmL(alarm);
	entryArray.AppendL(entry);
	CleanupStack::PopAndDestroy(alarm);
	CleanupStack::Pop(entry);
	
	//create 2nd entry (without alarm)
	HBufC8* guid2 = NULL;
	entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid2);
	CleanupStack::PushL(entry);
	startTime.SetTimeLocalL(startTime.TimeLocalL() += TTimeIntervalMonths(3));
	endTime.SetTimeLocalL(startTime.TimeLocalL() + TTimeIntervalMinutes(60));
	
	entry->SetStartAndEndTimeL(startTime, endTime);
	entry->SetSummaryL(_L("Summer"));
	
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);

	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	
	TTime time;
	time.HomeTime();
	TDateTime dateTime=time.DateTime();
	dateTime.SetHour(12);
	TTime newTime=dateTime;
	
    RPIMTestServer serv1;
    User::LeaveIfError(serv1.Connect());
    serv1.SetHomeTime(newTime);
    serv1.Close();

	time.HomeTime();
	dateTime=time.DateTime();
	TLocale oldLocale;
	ExportAsVCalendarL(KJapanFile, entryArray, KAsiaTokyo);		//Japan
	ExportAsVCalendarL(KUkFile, entryArray, KEuropeLondon);			//UK
	ExportAsVCalendarL(KNewYorkFile, entryArray, KAmericaNewYork);	//New York
	
	CleanupStack::PopAndDestroy(&entryArray);
	
	oldLocale.Set();
	time+=TTimeIntervalMinutes(1);
	
    RPIMTestServer serv4;
    User::LeaveIfError(serv4.Connect());
    serv4.SetHomeTime(time);
    serv4.Close();

	CheckOutputL(KJapanFile,KUkFile);
	CheckOutputL(KUkFile,KNewYorkFile);
	}

TInt CTestApp::ContainsTextL(const TDesC& aFileName, const TDesC8& aText)
	{
	//open file
	RFile file;
	User::LeaveIfError(file.Open(iCalTestLib->FileSession(), aFileName, EFileShareReadersOnly));
	CleanupClosePushL(file);
	//alloc buffer
	TInt sizeOfFile;
	User::LeaveIfError(file.Size(sizeOfFile));
	HBufC8* buffer = HBufC8::NewLC(sizeOfFile);
	//copy file into buffer
	TPtr8 ptr(buffer->Des());
	User::LeaveIfError(file.Read(ptr,sizeOfFile));
	//look for the descriptor
	TInt found=ptr.Find(aText);
	CleanupStack::PopAndDestroy(2,&file);
	return found;
	}

void CTestApp::SummaryOrDescriptionOnlyTestL()
	{
	test.Next(_L("Testing function: CAgendaEntryToVCalConverter::AddEntryPropertiesL ..."));

	//Initialisation
	_LIT(KSummaryText, "summary");
	_LIT(KDescriptionText, "description");
	
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(iToday.TimeLocalL().DateTime().Year(), iToday.TimeLocalL().DateTime().Month(), iToday.TimeLocalL().DateTime().Day(),10,0,0,0));
	TCalTime endTime;
	endTime.SetTimeLocalL(startTime.TimeLocalL() + TTimeIntervalMinutes(90));
	//
	
	//Create appt agenda entry with description only	
	HBufC8* guidDescOnly = NULL;
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidDescOnly);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(startTime, endTime);
	entry->SetDescriptionL(KDescriptionText());
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	
	//Create appt agenda entry with summary only
	HBufC8* guidSumOnly = NULL;
	entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidSumOnly);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(startTime, endTime);
	entry->SetSummaryL(KSummaryText);
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	
	//Create appt agenda entry with both summary and description
	HBufC8* guidBoth = NULL;	
	entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidBoth);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(startTime, endTime);
	entry->SetSummaryL(KSummaryText);
	entry->SetDescriptionL(KDescriptionText());
	entryArray.AppendL(entry);
	CleanupStack::Pop(entry);
	
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	
	//Check exported data
	_LIT8(KCheckBeamingSummaryOnly,"SUMMARY:summary");
	_LIT8(KCheckSummary,"SUMMARY:summary");
	_LIT8(KCheckDescription,"DESCRIPTION:description");
	_LIT8(KTagSummary,"SUMMARY:");
	_LIT8(KTagDescription,"DESCRIPTION:");

 	//
	//Check summary only export
	// beaming
	
	RPointerArray<CCalEntry> exportEntryArray;
	CleanupClosePushL(exportEntryArray);
	
	exportEntryArray.AppendL(entryArray[1]);
	ExportAsVCalendarL(KTempFileName, exportEntryArray, KEuropeLondon);
	test(ContainsTextL(KTempFileName,KCheckBeamingSummaryOnly)!=KErrNotFound); // check summary value is present in the summary field
	test(ContainsTextL(KTempFileName,KTagSummary)!=KErrNotFound); // check summary tag is present
	//syncML
	ExportAsVCalendarL(KTempFileName, exportEntryArray, KEuropeLondon);
	test(ContainsTextL(KTempFileName,KCheckSummary)!=KErrNotFound); // check summary
	test(ContainsTextL(KTempFileName,KTagDescription)==KErrNotFound); // check no description
	//
	//Check description only export
	exportEntryArray.Reset();
	exportEntryArray.AppendL(entryArray[0]);
	// beaming
	ExportAsVCalendarL(KTempFileName, exportEntryArray, KEuropeLondon);
	test(ContainsTextL(KTempFileName,KTagSummary)==KErrNotFound); // check no summary
	test(ContainsTextL(KTempFileName,KCheckDescription)!=KErrNotFound); // check description
	//syncML
	ExportAsVCalendarL(KTempFileName, exportEntryArray, KEuropeLondon);
	test(ContainsTextL(KTempFileName,KTagSummary)==KErrNotFound); // check no summary
	test(ContainsTextL(KTempFileName,KCheckDescription)!=KErrNotFound); // check description
	//
	//Check description and summary export
	exportEntryArray.Reset();
	exportEntryArray.AppendL(entryArray[2]);
	// beaming
	ExportAsVCalendarL(KTempFileName, exportEntryArray, KEuropeLondon);
	test(ContainsTextL(KTempFileName,KCheckSummary)!=KErrNotFound); // check summary
	test(ContainsTextL(KTempFileName,KCheckDescription)!=KErrNotFound); // check description
	//syncML
	ExportAsVCalendarL(KTempFileName, exportEntryArray, KEuropeLondon);
	test(ContainsTextL(KTempFileName,KCheckSummary)!=KErrNotFound); // check summary
	test(ContainsTextL(KTempFileName,KCheckDescription)!=KErrNotFound); // check description
	
	CleanupStack::PopAndDestroy(&exportEntryArray);
	CleanupStack::PopAndDestroy(&entryArray);
	}
	




static void doMainL()
	{
	//get the initial time
 	TTime homeTime;
 	homeTime.HomeTime();

	TDateTime dt(1999, EJanuary, 0, 0, 0, 0, 0);
	TTime arbitary(dt);
	
    RPIMTestServer serv2;
    User::LeaveIfError(serv2.Connect());
    serv2.SetHomeTime(arbitary);

	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);

	// create entries
	testApp->CreateAppointmentL();
	testApp->CreateAnniversaryEntryL();
	testApp->CreateTimedAlarmedApptEntryL();
	testApp->CreateToDoL();
	
	// export a vCalendar with locale not set (home)
	
	test.Next(_L("Exporting all entries (home locale)"));

	testApp->ExportAsVCalendarL(KHomeFile);
	
	// change locale to French (+1hr GMT/UTC)
	User::SetUTCOffset(TTimeIntervalSeconds(60*60));

	// export a vCalendar with the locale set to French 
	
	test.Next(_L("Exporting all entries (french locale)"));

	testApp->ExportAsVCalendarL(KFrenchFile);
	User::SetUTCOffset(TTimeIntervalSeconds(0));

	testApp->DoSyncMLExportTestL();
	testApp->SummaryOrDescriptionOnlyTestL();
	testApp->CheckLastChangedDateOnVcalImportL();

	testApp->CleanFileL();
	
	testApp->ImportAsVCalendarL(KFrenchFile);
 	testApp->CheckEntriesL();
 
 	//Set back the time to the initial time
    serv2.SetHomeTime(homeTime);
    serv2.Close();
    if (testApp->AreFilesToBeDeleted())
        {
    	testApp->DeleteFile(KFrenchFile);
	    testApp->DeleteFile(KHomeFile);
	    testApp->DeleteFile(KJapanFile);
	    testApp->DeleteFile(KNewYorkFile);
	    testApp->DeleteFile(KTempFileName);
 	    testApp->DeleteFile(KUkFile);
        }
    

	CleanupStack::PopAndDestroy(testApp);
	}


/**

@SYMTestCaseID     PIM-T-VCAL4-0001

*/

TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-VCAL4-0001 T_VCAL4"));

	test.Title();
	CActiveScheduler* scheduler = new CActiveScheduler;
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

