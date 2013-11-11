// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "caltestlib.h"
#include <coreappstest/testserver.h>

#include <calentry.h>
#include <calentryview.h>
#include <calsession.h>
#include <calrrule.h>
#include <caldataexchange.h>
#include <caldataformat.h>
#include <calinstance.h>
#include <calinstanceview.h>

#include <e32test.h>
#include <s32file.h>
#include <e32base.h>
#include <e32math.h>

_LIT8(KLondon, "Europe/London");
_LIT8(KTokyo, "Asia/Tokyo");
_LIT8(KHelsinki,"Europe/Helsinki");

_LIT8(KGuid, "%d_Schedule");	//global UID for entry

_LIT(KCalendarFileFullName, "c:\\private\\10003a5b\\tcal_filesize");

_LIT(KTestName, "tcal_filesize");
_LIT(KVCalFile, "tcal_filesize.vcs");

const TInt KSummaryLength(200);
const TInt KDescriptionLength(600);
const TInt KCRUDAmount(10);

const TTime KTestTime(TDateTime(2007, EMarch, 0, 0, 0, 0, 0));

GLDEF_C RTest test(KTestName);

TInt gTotalEntries(0);

class CFileSizeTest : public CBase
	{
public:
    static CFileSizeTest* NewLC();
	void RunAllTestsL();
	~CFileSizeTest();	

private:
	CFileSizeTest();
	void ConstructL();
	
	void TestStartupTimeL(const TDesC& aCalendarFile);
	void TestCRUDL(TBool aRepeating);
	
	void StoreEntriesL(const TDesC8& aTimeZone, TBool aRepeating, TInt aNumberofEntry);
	void StoreEntriesL(RPointerArray<CCalEntry>& aEntries);
	HBufC* CreateStringLC(TInt aLength);
	
	void DeleteEntriesL(TInt aNumberofEntry);
	
	void ExportL(TInt aNumEntries);
	void ImportL();
	
private:
	CCalTestLibrary* iTestLib;
	
	CTestRegister* iTestRegister;
	
	TPerformanceTimer iPerformanceTimer;
	};

CFileSizeTest* CFileSizeTest::NewLC()
	{
	CFileSizeTest* self = new(ELeave) CFileSizeTest;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CFileSizeTest::CFileSizeTest()
	:iPerformanceTimer(test)
	{
	}

void CFileSizeTest::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KTestName);
	iTestLib->OpenFileL(KTestName);
	
	iTestRegister = CTestRegister::NewL();
	
	User::LeaveIfError(User::SetUTCTime(KTestTime));
	}	
	
CFileSizeTest::~CFileSizeTest()	
	{
	TRAP_IGNORE(iTestLib->GetSession().DeleteCalFileL(KTestName));
	delete iTestLib;
	delete iTestRegister;
	}

HBufC* CFileSizeTest::CreateStringLC(TInt aLength)
	{
	HBufC* string = HBufC::NewLC(aLength);
	
	TTime seedTime;
	seedTime.HomeTime();
	TInt64 seed(seedTime.Int64());
	
	for (TInt i(0) ; i < aLength ; ++i)
		{
		// Add random upper and lower case letters
		string->Des().Append( 'A' + (Math::Rand(seed) % 26) + ((Math::Rand(seed) % 2) * ' ') );
		}
	
	return string;
	}

void CFileSizeTest::StoreEntriesL(const TDesC8& aTimeZone, TBool aIsRepeating, TInt aNumberofEntry)
	{
	test.Printf(_L("storing entries\nNumber of entries\tServer heap size(KB)\tClient heap Size(KB)\tFile Size(KB)\ttime so far (microseconds)\n"));
				
	iTestLib->SetTimeZoneL(aTimeZone);
	
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	// Always start with the same seed so that we always
	// create the same entries
	// this is so the time it takes to populate a month view
	// is always the same for the same month
	TInt64 seed(0xC0FFE7);
	
	TInt ii;
	for (ii = 0 ; ii < aNumberofEntry ; ++ii)
		{
		HBufC8* guid = HBufC8::NewLC(KGuid().Length() + 4);
		guid->Des().Format(KGuid, ++gTotalEntries);
		CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0); //non-repeating entry NewL
	    CleanupStack::Pop(guid);
	    CleanupStack::PushL(entry);
		entries.AppendL(entry);
		CleanupStack::Pop(entry);
		
		TInt year 	 = Math::Rand(seed) % 10 + 1998;
		TInt month   = Math::Rand(seed) % 11;
		TInt day     = Math::Rand(seed) % 27;
		TInt hours   = Math::Rand(seed) % 23;
		TInt minutes = Math::Rand(seed) % 59;
		
		// Set Entry Times
		TCalTime startCalTime;
		TCalTime endCalTime;
		startCalTime.SetTimeLocalL(TDateTime(year, static_cast<TMonth>(month), day, hours, minutes, 0, 0));
		endCalTime.SetTimeLocalL(startCalTime.TimeLocalL() + TTimeIntervalHours(1));
	    entry->SetStartAndEndTimeL(startCalTime, endCalTime);

	    HBufC* summary = CreateStringLC(KSummaryLength);
		entry->SetSummaryL(*summary);
		CleanupStack::PopAndDestroy(summary);
		
		HBufC* description = CreateStringLC(KDescriptionLength);
		entry->SetDescriptionL(*description);
		CleanupStack::PopAndDestroy(description);
		
		if(aIsRepeating)
			{
			// Set the repeat rule
			TCalRRule rptRule(TCalRRule::EDaily);
			rptRule.SetDtStart(startCalTime);
			rptRule.SetCount(15);
			entry->SetRRuleL(rptRule);
			}
		
		if ( (gTotalEntries % 100) == 0 )
			{
			StoreEntriesL(entries);
			}
		}
	
	StoreEntriesL(entries);
	
    CleanupStack::PopAndDestroy(&entries);
	}

void CFileSizeTest::DeleteEntriesL(TInt aNumberofEntry)
	{
	CDesC8Array* uidList = new(ELeave) CDesC8ArraySeg(8);
	CleanupStack::PushL(uidList);
	
	for (TInt ii = 0 ; ii < aNumberofEntry ; ++ii)
		{
		HBufC8* guid = HBufC8::NewLC(KGuid().Length() + 4);
		guid->Des().Format(KGuid, gTotalEntries + ii + 1);
		uidList->AppendL(*guid);
		CleanupStack::PopAndDestroy(guid);
		}
	
	iTestLib->SynCGetEntryViewL().DeleteL(*uidList);
	
	CleanupStack::PopAndDestroy(uidList);
	}
	
void CFileSizeTest::StoreEntriesL(RPointerArray<CCalEntry>& aEntries)
	{
	TInt entriesStored(0);
	iTestLib->SynCGetEntryViewL().StoreL(aEntries, entriesStored);
	aEntries.ResetAndDestroy();
	
	TInt totalAllocSizeServer = iTestLib->GetSession()._DebugRequestAllocatedHeapSizeL();
	TInt totalAllocSizeClient;
	TInt cellCountClient = User::AllocSize(totalAllocSizeClient);
	TInt fileSize = iTestLib->PIMTestServer().GetFileSizeL(KCalendarFileFullName);
	
	iPerformanceTimer.Stop();
	
	test.Printf(_L("%d\t%d\t%d\t%d\t%d\n"), gTotalEntries, totalAllocSizeServer / 1024, totalAllocSizeClient / 1024, fileSize / 1024, iPerformanceTimer.ElapsedTime().Int());
	}
	
void CFileSizeTest::TestStartupTimeL(const TDesC& aCalendarFile)
	{
	iTestLib->CloseAgendaServer();
	iTestLib->OpenFileL(aCalendarFile);
	
	iPerformanceTimer.Start();
	iTestLib->SynCGetEntryViewL();
	
	// fetch a month view
	TCalTime startRange;
	startRange.SetTimeLocalL(KTestTime);
	TCalTime endRange;
	endRange.SetTimeLocalL(KTestTime + TTimeIntervalMonths(1));
	CalCommon::TCalTimeRange timeRange(startRange, endRange);
	
	RPointerArray<CCalInstance> instances;
	CleanupResetAndDestroyPushL(instances);
	
	iTestLib->SynCGetInstanceViewL().FindInstanceL(instances, CalCommon::EIncludeAll, timeRange);
	
	test.Printf(_L("instances found = %d\n"), instances.Count());
	
	CleanupStack::PopAndDestroy(&instances);
	
	iPerformanceTimer.Stop();
	test.Printf(_L("time taken is %d\n"), iPerformanceTimer.ElapsedTime().Int());
	}



void CFileSizeTest::TestCRUDL(TBool aRepeating)
	{
	// Add 10
	test.Next(_L("Test adding 10 entries"));

	iPerformanceTimer.Start();
	StoreEntriesL(KLondon, aRepeating, KCRUDAmount);
	iPerformanceTimer.Stop();
	
	// Update 10	
	test.Next(_L("Test updating 10 entries"));

	// this means we will update the entries we just added
	gTotalEntries -= KCRUDAmount;
	iPerformanceTimer.Start();
	StoreEntriesL(KHelsinki, aRepeating, KCRUDAmount);
	iPerformanceTimer.Stop();
	
	// Delete 10
	test.Next(_L("Test deleting 10 entries"));

	// this means we will delete the entries that we just added and updated
	gTotalEntries -= KCRUDAmount;
	iPerformanceTimer.Start();
	DeleteEntriesL(KCRUDAmount);
	iPerformanceTimer.Stop();
	test.Printf(_L("time taken is %d\n"), iPerformanceTimer.ElapsedTime().Int());
	}



void CFileSizeTest::ExportL(TInt aNumEntries)
	{
	test.Next(_L("Export entries"));

	
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(iTestLib->GetSession());
	CleanupStack::PushL(dataExchange);
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	for (TInt ii(0) ; ii < aNumEntries ; ++ii)
		{
		HBufC8* guid = HBufC8::NewLC(KGuid().Length() + 4);
		guid->Des().Format(KGuid, gTotalEntries - ii);
		iTestLib->SynCGetEntryViewL().FetchL(*guid, entries);
		CleanupStack::PopAndDestroy(guid);
		}
	
	// fetch some entries to export
	RFile file;
	iTestRegister->CreateTempFileLC(file, KVCalFile);
	
	RFileWriteStream fileWriteStream(file);
	CleanupClosePushL(fileWriteStream);
	
	iPerformanceTimer.Start();
	dataExchange->ExportL(KUidVCalendar, fileWriteStream, entries);
	iPerformanceTimer.Stop();
	test.Printf(_L("time taken is %d\n"), iPerformanceTimer.ElapsedTime().Int());
	
	CleanupStack::PopAndDestroy(&fileWriteStream);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(dataExchange);
	}



void CFileSizeTest::ImportL()
	{
	test.Next(_L("Test importing entries"));

	
	CCalDataExchange* dataExchange = CCalDataExchange::NewL(iTestLib->GetSession());
	CleanupStack::PushL(dataExchange);
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
	
	RFile file;
	iTestRegister->OpenTempFileLC(file, KVCalFile);
	
	RFileReadStream fileWriteStream(file);
	CleanupClosePushL(fileWriteStream);
	
	iPerformanceTimer.Start();
	
	dataExchange->ImportL(KUidVCalendar, fileWriteStream, entries);
	TInt numSuc(0);
	iTestLib->SynCGetEntryViewL().StoreL(entries, numSuc);
	
	iPerformanceTimer.Stop();
	test.Printf(_L("time taken is %d\n"), iPerformanceTimer.ElapsedTime().Int());
	
	test(numSuc == KCRUDAmount);
	
	CleanupStack::PopAndDestroy(&fileWriteStream);
	CleanupStack::PopAndDestroy(&file);
	CleanupStack::PopAndDestroy(&entries);
	CleanupStack::PopAndDestroy(dataExchange);
	}



void CFileSizeTest::RunAllTestsL()
	{
	test.Next(_L("Store 2500 repeating entries"));

	iPerformanceTimer.Start();
	StoreEntriesL(KLondon, ETrue, 834);
	StoreEntriesL(KTokyo, ETrue, 833);
	StoreEntriesL(KHelsinki, ETrue, 833);
	
	test.Next(_L("Measure startup time with 2500 repeating entries"));

	TestStartupTimeL(KTestName);
	
	TestCRUDL(ETrue);
	
	ExportL(10);
	ImportL();
	
	iTestLib->ReplaceFileL(KTestName);
	iTestLib->OpenFileL(KTestName);
	gTotalEntries = 0;

	test.Next(_L("Store 2500 NON-repeating entries"));

	iPerformanceTimer.Start();
	StoreEntriesL(KLondon, EFalse, 834);
	StoreEntriesL(KTokyo, EFalse, 833);
	StoreEntriesL(KHelsinki, EFalse, 833);

	test.Next(_L("Measure startup time with 2500 NON-repeating entries"));

	TestStartupTimeL(KTestName);
	
	TestCRUDL(EFalse);
	
	ExportL(10);
	ImportL();
	}
	
LOCAL_C void doMainL()
	{
	CFileSizeTest* fileSizeTest = CFileSizeTest::NewLC();
	fileSizeTest->RunAllTestsL();
	CleanupStack::PopAndDestroy(fileSizeTest);
	}

/**

@SYMTestCaseID     PIM-TCAL-FILESIZE-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-FILESIZE-0001 tcal_filesize"));

	test.Title();
	
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	
	TRAPD(ret, doMainL());
	test(ret == KErrNone);
	
	delete scheduler;
	delete theCleanup;	
	
	test.End();
	test.Close();
	
	__UHEAP_MARKEND;
	return ret;
	}

