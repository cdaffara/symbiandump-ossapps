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

#include <caldataexchange.h>
#include <calentryview.h>
#include <calrrule.h>
#include <calalarm.h>
#include <caliterator.h>
#include <caldataformat.h>
#include <calsession.h>

#include "caltestlib.h"
#include "caltestoom.h"

LOCAL_D RTest test(_L("T_VCAL5OOM"));
_LIT8(KOOMEntryGuid,  "OOM_Entry_Guid");

_LIT(KTestCalendarFile,	"c:t_vcal5oom");
_LIT(KVCalFile,			"c:\\t_vcal5oom.vcs");

class CTestApp : public CBase, public MCalTestOomCallBack
	{
public:
	enum TOOMTestCase
		{
		ETestAllEntry,
		ETestTemporaryEntry,
		EAmountTestCases
		};
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void CreateAppointmentL();
	void CreateAnniversaryEntryL();
	void CreateTimedAlarmedApptEntryL();
	
	CCalEntry* CreateTodoLC(TBool isOOMTest);
	
	void ExportAsVCalendarL(const TDesC& aVCalendarFile);
	void ExportAsVCalendarL(const TDesC& aVCalendarFile, CCalEntry* aEntry);
	
	void DoOomTestL();
	
	
public: // from MCalOomTestCallBack
	void OomTestL(TType aType, TInt aFailAt);
	
private:
	void ConstructL();

private:
	CCalTestLibrary* iCalTestLib; //owned
	
	TInt iOOMTestCase;
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
	}
		
CTestApp::~CTestApp()
	{
	delete iCalTestLib;
	}
	
void CTestApp::DoOomTestL()
	{
	CCalSession& calSession = iCalTestLib->GetSession();
	
	for (iOOMTestCase = 0 ; iOOMTestCase < EAmountTestCases; ++iOOMTestCase)
		{
		test.Printf(_L("Starting oom test %d\n"), iOOMTestCase);
		CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
		CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
		}
	}
	
void CTestApp::OomTestL(TType /*aType*/, TInt /*aFailAt*/)
	{
	test.Printf(_L("."));
	
	switch (iOOMTestCase)
		{
		case ETestAllEntry:
			{
			ExportAsVCalendarL(KVCalFile);
			}
			break;
		case ETestTemporaryEntry:
			{
			CCalEntry* entry = CreateTodoLC(ETrue);
			ExportAsVCalendarL(KVCalFile, entry);
			CleanupStack::PopAndDestroy(entry);
			}
			break;
		default:
			{
			User::Leave(KErrGeneral);
			}
		}
	}

// Create an appointment with Unicode text
//
void CTestApp::CreateAppointmentL()
	{
	TTime today;
	today.HomeTime();
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(today.DateTime().Year(),today.DateTime().Month(),today.DateTime().Day(),9,0,0,0));
	TCalTime endTime;
	endTime.SetTimeLocalL(startTime.TimeLocalL() + TTimeIntervalMinutes(60));
	HBufC8* guid = NULL;
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(startTime, endTime);
	TBuf16<60> apptSummary;
	apptSummary.AppendFormat(_L16("Here are some European letters: %c%c%c%c%c%c%c%c%c%c. "), 0x0117, 0x010d, 0x00f1, 0x00df, 0x00d8, 0x0142, 0x0151, 0x016f, 0x0131, 0x00c9);
	entry->SetSummaryL(apptSummary);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(entry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	CleanupStack::PopAndDestroy(); // entryArray.Close()
	CleanupStack::PopAndDestroy(entry);
	}

// Create a repeating entry.
// The repeat is set for yearly on June 24th starting in 1998, with the exception of 24/06/2000
//
void CTestApp::CreateAnniversaryEntryL()
	{
	HBufC8* guid = NULL;
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAnniv, guid);
	CleanupStack::PushL(entry);
	TBuf16<60> text;
	text.AppendFormat(_L16("Here are some Han characters: %c%c%c%c%c%c%c%c%c%c. "),   0x7f0c, 0x7a92, 0x6d90, 0x6272, 0x5934, 0x5973, 0x55ce, 0x516c, 0x4ec0, 0x4e00);
	entry->SetSummaryL(text);
	TCalTime annivTime;
	annivTime.SetTimeLocalL(TTime(TDateTime(1998,EJune,24,0,0,0,0)));
	
	entry->SetStartAndEndTimeL(annivTime, annivTime);  
	
	TCalRRule rRule(TCalRRule::EYearly);
	
	rRule.SetDtStart(annivTime);  
	TCalTime maxTime;
	maxTime.SetTimeUtcL(TCalTime::MaxTime());
	rRule.SetUntil(maxTime);
	rRule.SetInterval(1);
	entry->SetRRuleL(rRule);
	
	TCalTime exception;
	exception.SetTimeLocalL(TTime(TDateTime(2000,EJune,24,0,0,0,0)));
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	exceptions.AppendL(exception);
	entry->SetExceptionDatesL(exceptions);
	CleanupStack::PopAndDestroy(&exceptions);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(entry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	
	CleanupStack::PopAndDestroy(); // entryArray.Close()
	CleanupStack::PopAndDestroy(entry); //entry
	}


// Create an alarmed appointment
//
void CTestApp::CreateTimedAlarmedApptEntryL()
	{
	TTime today;
	today.HomeTime();
	TCalTime startTime;
	startTime.SetTimeLocalL(TDateTime(today.DateTime().Year(),today.DateTime().Month(),today.DateTime().Day(),10,0,0,0));
	TCalTime endTime;
	endTime.SetTimeLocalL(startTime.TimeLocalL() + TTimeIntervalMinutes(90));
	HBufC8* guid = NULL;
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::PushL(entry);
	entry->SetStartAndEndTimeL(startTime, endTime);
	TBuf16<60> text;
	text.AppendFormat(_L16("Here are some Japanese kana: %c%c%c%c%c%c%c%c%c%c. "),    0x30a2, 0x30c0, 0x30d5, 0x30b7, 0x30ba, 0x3091, 0x3093, 0x3055, 0x3074, 0x3080);
	entry->SetSummaryL(text);

	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	alarm->SetTimeOffset(TTimeIntervalMinutes(15));
	TBuf16<30> alarmName;
	alarmName.AppendFormat(_L16("%c%c%c%c%c%c%c%c%c%c. "),    0x03a3, 0x03bf, 0x03bc, 0x03b4, 0x03b5, 0x0345, 0x03a9, 0x0392, 0x0395, 0x03a1);
	alarm->SetAlarmSoundNameL(alarmName);
	entry->SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(entry);
	TInt numSuc;
	
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	
	CleanupStack::PopAndDestroy(); // entryArray.Close()
	CleanupStack::PopAndDestroy(entry);
	}

CCalEntry* CTestApp::CreateTodoLC(TBool isOOMTest)
	{
	HBufC8* guid = NULL; 
	
	CCalEntry* todo = NULL;
	if(isOOMTest)
		{
		guid = KOOMEntryGuid().AllocLC();
		todo = CCalEntry::NewL(CCalEntry::ETodo, guid, CCalEntry::EMethodAdd, 0);
		CleanupStack::Pop(guid);
		}
	else
		{
		todo = iCalTestLib->CreateCalEntryL(CCalEntry::ETodo, guid);
		}

	CleanupStack::PushL(todo);
	TTime now;		  
	now.UniversalTime();
	
	TCalTime startTime;
	startTime.SetTimeLocalL(now + TTimeIntervalDays(5));
	TCalTime endTime;
	endTime.SetTimeLocalL(now + TTimeIntervalDays(5) + TTimeIntervalMinutes(500));
	
	todo->SetStartAndEndTimeL(startTime, endTime);
	
	TBuf16<60> text;
	text.AppendFormat(_L16("A Selection of Dingbats: %c%c%c%c%c%c%c%c%c "), 0x2780, 0x270d, 0x27ab, 0x2704, 0x2706, 0x2707, 0x272d, 0x2729, 0x270f, 0x278b);
	todo->SetSummaryL(text);
	TCalTime now2;
	now2.SetTimeLocalL(now);
	todo->SetCompletedL(ETrue, now2);
	
	return todo;
	}

void CTestApp::ExportAsVCalendarL(const TDesC& aVCalendarFile)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);

	CCalIter* calIter = NULL;
	calIter = CCalIter::NewL(iCalTestLib->GetSession());
	CleanupStack::PushL(calIter);
	
	for (TDesC8* guid = const_cast<TDesC8*>(&calIter->FirstL());
			guid->CompareF(KNullDesC8) != 0;
			guid = const_cast<TDesC8*>(&calIter->NextL()))
		{
			iCalTestLib->SynCGetEntryViewL().FetchL(*guid, entryArray);
		}
		
	CleanupStack::PopAndDestroy(calIter);
	
	RFile outFile;
	CleanupClosePushL(outFile);

	User::LeaveIfError(outFile.Replace(iCalTestLib->FileSession(), aVCalendarFile, EFileWrite));


	RFileWriteStream writeStream(outFile);
	CleanupClosePushL(writeStream);
	
	CCalDataExchange* dataExchange = NULL;
	dataExchange = CCalDataExchange::NewL(iCalTestLib->GetSession());
	CleanupStack::PushL(dataExchange);
	
	dataExchange->ExportL(KUidVCalendar, writeStream, entryArray);
	
	CleanupStack::PopAndDestroy(dataExchange);
	CleanupStack::PopAndDestroy(); // writeStream.Close()
	CleanupStack::PopAndDestroy(); // outFile.Close()
	CleanupStack::PopAndDestroy(); // entryArray.Close()
	}

void CTestApp::ExportAsVCalendarL(const TDesC& aVCalendarStream, CCalEntry* aEntry)
	{
	RFile outfile;
	CleanupClosePushL(outfile);
	User::LeaveIfError(outfile.Replace(iCalTestLib->FileSession(),aVCalendarStream,EFileWrite));
	
	RFileWriteStream writeStream(outfile);
	CleanupClosePushL(writeStream);
	
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(aEntry);

	CCalDataExchange* dataExchange = NULL;
	dataExchange = CCalDataExchange::NewL(iCalTestLib->GetSession());

	CleanupStack::PushL(dataExchange);
	dataExchange->ExportL(KUidVCalendar, writeStream, entryArray);
	
	writeStream.CommitL();
	
	CleanupStack::PopAndDestroy(dataExchange);
	CleanupStack::PopAndDestroy(); // entryArray.Close();
	CleanupStack::PopAndDestroy(); // writeStream.Close();
	CleanupStack::PopAndDestroy(); // outfile.Close();
	}	



static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);
	
	// create entries	
	testApp->CreateAppointmentL();
	testApp->CreateAnniversaryEntryL();
	testApp->CreateTimedAlarmedApptEntryL();
	
	// export a vCalendar stream

	test.Next(_L("Exporting all entries"));

	testApp->ExportAsVCalendarL(KVCalFile);
	
	// export a single vCalendar object

	test.Next(_L("Exporting a single vTodo object"));

	CCalEntry* todo = testApp->CreateTodoLC(EFalse);
	testApp->ExportAsVCalendarL(KVCalFile, todo);
	CleanupStack::PopAndDestroy(todo);
	
	// Start the oom tests
	
	test.Next(_L("Starting the oom tests"));

	testApp->DoOomTestL();

	CleanupStack::PopAndDestroy(testApp);
	}

static void CleanupFilesL()
	{
	CCalTestLibrary* testLibrary = CCalTestLibrary::NewL(EFalse);
	CleanupStack::PushL(testLibrary);
	testLibrary->DeleteFileL(KVCalFile, EFalse);

	CleanupStack::PopAndDestroy(testLibrary);
	}
	
/**

@SYMTestCaseID     PIM-T-VCAL5OOM-0001

*/

TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-T-VCAL5OOM-0001 T_VCAL5OOM"));

	test.Title();
	CActiveScheduler* scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());	
	test(ret==KErrNone);
	TRAP_IGNORE(CleanupFilesL());
	delete theCleanup;	
	delete scheduler;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
