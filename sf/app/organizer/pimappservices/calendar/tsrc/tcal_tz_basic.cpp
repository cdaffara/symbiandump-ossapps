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

#include <e32test.h>
#include <e32base.h>
#include "caltestlib.h"
#include <calentry.h>
#include <calentryview.h>
#include <calsession.h>
#include <calrrule.h>
#include <calinstance.h>
#include <calinstanceview.h>
#include <vtzrules.h> 
#include <vcal.h>

_LIT(KTestName,"tcal_tz_basic");
_LIT8(KLondon,"Europe/London");
_LIT8(KTokyo,"Asia/Tokyo");
_LIT8(KHelsinki,"Europe/Helsinki");

GLDEF_C RTest test(KTestName);
// The tests use the following start and end times so put them here
TDateTime startTime(2007, ENovember, 15, 19, 0, 0, 0);
TDateTime endTime(2007, ENovember, 15, 20, 0, 0, 0);

class CTCalBasicTest : public CBase
	{
public:
    static CTCalBasicTest* NewL();
	void RunAllTestsL();
	~CTCalBasicTest();	
private:
	void ConstructL();
	
	TCalLocalUid StoreEntryL(const TDesC8& aGuid, const TDesC8& aTimeZone, TBool aIsRepeating);
	CTzRules* GetTimeZoneRuleL(const TDesC8& aGuid);
	CTzRules* GetSystemTimeZoneRuleL();
	CCalEntry* CreateCalEntryCopyL(const TDesC8& aGuid, const TDesC8& aGuidCopy, CCalEntry::TCopyType aCopyType=CCalEntry::ECopyAll);
	void GetEntryTzRuleAndCheckL(CTzRules& aTzRule, const TDesC8& aGuid);
	void DeleteEntryL(const TDesC8& aGuid);
//test cases
	void TestSettingTzRuleL();
	void TestCopyEntryL(CCalEntry::TCopyType aCopyType);
	void TestUpdateEntryL();
	void TestInstanceViewL(TDateTime& aStartTime, TDateTime& aEndTime, TInt aNumInstance);
	void TestTzInstancesL();
	void TestImportExportL();
	
private:
	CCalTestLibrary* iTestLib;
	RTz iTz;
	};

CTCalBasicTest* CTCalBasicTest::NewL()
	{
	CTCalBasicTest* self = new (ELeave) CTCalBasicTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CTCalBasicTest::ConstructL()
	{
	iTestLib = CCalTestLibrary::NewL();
	iTestLib->ReplaceFileL(KTestName);
	User::LeaveIfError(iTz.Connect());
	}

CTCalBasicTest::~CTCalBasicTest()	
	{
	iTz.Close();
	TRAPD(err, iTestLib->DeleteFileL(KTestName, ETrue));
	delete iTestLib;
	}

TCalLocalUid  CTCalBasicTest::StoreEntryL(const TDesC8& aGuid, const TDesC8& aTimeZone, TBool aIsRepeating)
	{
	iTestLib->SetTimeZoneL(aTimeZone);
	RPointerArray<CCalEntry> entries;
	CleanupResetAndDestroyPushL(entries);
 	
	HBufC8* guid = aGuid.AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0); //non-repeating entry NewL
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	entries.AppendL(entry);
	CleanupStack::Pop(entry);

	//set Calendar times with values local to the current system time zone
	TCalTime startCalTime;
	TCalTime endCalTime;
	startCalTime.SetTimeLocalL(startTime);
	endCalTime.SetTimeLocalL(endTime);

	//Set Entry Times
	entry->SetStartAndEndTimeL(startCalTime, endCalTime);

	// Set Summary(200characters)
	_LIT(KSummary,"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqr");

	entry->SetSummaryL(KSummary);

	// Set Description(600characters)
	_LIT(KDescription,"abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzab");
	entry->SetDescriptionL(KDescription);

	if(aIsRepeating)
		{
		// Set the repeat rule
		TCalRRule rptRule(TCalRRule::EDaily);
		rptRule.SetDtStart(startCalTime);
		entry->SetRRuleL(rptRule);
		}
	
	//Save the entry into the view
	TInt entriesStored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entries, entriesStored);
	test(entries.Count()==entriesStored);
	TCalLocalUid calLocalUid =  entries[0]->LocalUidL();
    CleanupStack::PopAndDestroy(&entries);
    return calLocalUid;
	}
	
CTzRules* CTCalBasicTest::GetSystemTimeZoneRuleL()
	{
	CTzId*  tzId = iTz.GetTimeZoneIdL();
	CleanupStack::PushL(tzId);
	CTzRules* systemTimeZoneRules = iTz.GetTimeZoneRulesL(*tzId, 0, 9999, ETzUtcTimeReference);
	CleanupStack::PopAndDestroy(tzId);
	return systemTimeZoneRules;
	}

CTzRules* CTCalBasicTest::GetTimeZoneRuleL(const TDesC8& aGuid)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iTestLib->SynCGetEntryViewL().FetchL(aGuid, entryArray);
	CTzRules* tzRules = entryArray[0]->TzRulesL();
	CleanupStack::PopAndDestroy(&entryArray);
	return tzRules;
	}

CCalEntry* CTCalBasicTest::CreateCalEntryCopyL(const TDesC8& aGuid, const TDesC8& aGuidCopy, CCalEntry::TCopyType aCopyType)
	{
	HBufC8* guid = aGuidCopy.AllocLC();
	CCalEntry* entry = CCalEntry::NewL(CCalEntry::EAppt, guid, CCalEntry::EMethodNone, 0); //non-repeating entry NewL
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
		
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iTestLib->SynCGetEntryViewL().FetchL(aGuid, entryArray);
	CCalEntry* storedEntry = entryArray[0];
	
	entry->CopyFromL(*storedEntry, aCopyType);
	CleanupStack::PopAndDestroy(&entryArray);
	CleanupStack::Pop(entry);
	return entry;
	}

void CTCalBasicTest::DeleteEntryL(const TDesC8& aGuid)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iTestLib->SynCGetEntryViewL().FetchL(aGuid, entryArray);
	test (entryArray.Count() == 1); //There should only be a single entry
	CCalEntry* calEntry = entryArray[0];
	iTestLib->SynCGetEntryViewL().DeleteL(*calEntry);
	CleanupStack::PopAndDestroy(&entryArray);
	}



void CTCalBasicTest::TestSettingTzRuleL()
	{
	test.Next(_L("Test setting a Tz rule"));

//	Create and store an entry
//	Fetch the entry and store its time zone rule
//	shutdown server
//	change the time zone
//	open the calendar and fetch the entry and check its time zone rule
	_LIT8(KEntryTz, "Check Tz Entry");
	iTestLib->OpenFileL(KTestName);
	TCalLocalUid  calLocalUid = StoreEntryL(KEntryTz(), KTokyo(), ETrue);
	
	CTzRules* systemTimeZoneRules = GetSystemTimeZoneRuleL();
	
	CleanupStack::PushL(systemTimeZoneRules);
	
	GetEntryTzRuleAndCheckL(*systemTimeZoneRules, KEntryTz());
	
	iTestLib->CloseAgendaServer();
	
	iTestLib->SetTimeZoneL(KLondon());
	
	iTestLib->OpenFileL(KTestName);
	
	GetEntryTzRuleAndCheckL(*systemTimeZoneRules, KEntryTz());
	
	CleanupStack::PopAndDestroy(systemTimeZoneRules);
	}



void CTCalBasicTest::TestCopyEntryL(CCalEntry::TCopyType aCopyType)
	{
	test.Next(_L("Test copying entries with Tz rules"));

	_LIT8(KEntryTzOrig, "An appointment");
	_LIT8(KEntryTzCopy, "An appointment copy");
	iTestLib->CleanDatabaseL();
	iTestLib->OpenFileL(KTestName);
	TCalLocalUid  calLocalUid = StoreEntryL(KEntryTzOrig(), KTokyo(), ETrue);
	//Make a copy of the time zone rules from the system
	CTzRules* systemTimeZoneRules = GetSystemTimeZoneRuleL();
	CleanupStack::PushL(systemTimeZoneRules);
	//Change the time zone
	iTestLib->SetTimeZoneL(KLondon());
	CCalEntry* calEntry = CreateCalEntryCopyL(KEntryTzOrig(), KEntryTzCopy(), aCopyType);
	CleanupStack::PushL(calEntry);
	iTestLib->StoreEntryL(*calEntry);
	CleanupStack::PopAndDestroy(calEntry);
		
	iTestLib->CloseAgendaServer();
		
	iTestLib->OpenFileL(KTestName);
	
	GetEntryTzRuleAndCheckL(*systemTimeZoneRules, KEntryTzOrig());
	
	TDateTime startTimeInstance(2007, ENovember, 15, 0, 0, 0, 0);
    TDateTime endTimeInstance(2007, ENovember, 25, 0, 0, 0, 0);
    
	
	if (aCopyType == CCalEntry::EDontCopyId)
		{
		test.Printf(_L("EDontCopyId is tested, there should be 20 instances\n"));
		GetEntryTzRuleAndCheckL(*systemTimeZoneRules, KEntryTzCopy());
		TestInstanceViewL(startTimeInstance, endTimeInstance, 20);
		}
	else
		{
		test.Printf(_L("ECopyAll is tested, there should be 10 instances\n"));
		GetEntryTzRuleAndCheckL(*systemTimeZoneRules, KEntryTzOrig());
		TestInstanceViewL(startTimeInstance, endTimeInstance, 10);
		}
	
	CleanupStack::PopAndDestroy(systemTimeZoneRules);
	}

void CTCalBasicTest::GetEntryTzRuleAndCheckL(CTzRules& aTzRule, const TDesC8& aGuid)
	{
	CTzRules* timeZoneRulesCopy = GetTimeZoneRuleL(aGuid);
	if (timeZoneRulesCopy)
		{
		CleanupStack::PushL(timeZoneRulesCopy);
		test (aTzRule.IsEqualTo(*timeZoneRulesCopy));
		CleanupStack::PopAndDestroy(timeZoneRulesCopy);
		}
	else
		{
		test (EFalse);
		}
	}

void CTCalBasicTest::TestInstanceViewL(TDateTime& aStartTime, TDateTime& aEndTime, TInt aNumInstance)
	{
	RPointerArray<CCalInstance> calInstances;
	CleanupResetAndDestroyPushL(calInstances);
	
	//set Calendar times with values local to the current system time zone
	TCalTime startCalTime;
	TCalTime endCalTime;
	startCalTime.SetTimeLocalL(aStartTime);
	endCalTime.SetTimeLocalL(aEndTime);
	
	CalCommon::TCalTimeRange timeRange(startCalTime, endCalTime);
	iTestLib->SynCGetInstanceViewL().FindInstanceL(calInstances, CalCommon::EIncludeAll, timeRange);
	test (calInstances.Count() == aNumInstance);
	CleanupStack::PopAndDestroy(&calInstances);
	}



void CTCalBasicTest::TestUpdateEntryL()
	{
	test.Next(_L("Test updating entries with Tz rules"));

	_LIT8(KUpdateEntry, "Entry to be Updated");
	iTestLib->CleanDatabaseL();
	CTzRules* systemTimeZoneRules = GetSystemTimeZoneRuleL();
	CleanupStack::PushL(systemTimeZoneRules);
	
	iTestLib->OpenFileL(KTestName);
	TCalLocalUid  calLocalUid = StoreEntryL(KUpdateEntry(), KTokyo(), EFalse); 
	//Change the time zone
	iTestLib->SetTimeZoneL(KLondon());
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iTestLib->SynCGetEntryViewL().FetchL(KUpdateEntry(), entryArray);
	
	TInt numEntries(0);
	//set Calendar times with values local to the current system time zone
	TCalTime startCalTime;
	startCalTime.SetTimeLocalL(startTime);
	TCalRRule rptRule(TCalRRule::EDaily);
	rptRule.SetDtStart(startCalTime);
	entryArray[0]->SetRRuleL(rptRule);// Set the repeat rule
	iTestLib->SynCGetEntryViewL().UpdateL(entryArray, numEntries);
	CleanupStack::PopAndDestroy(&entryArray);
	iTestLib->CloseAgendaServer();
			
	iTestLib->OpenFileL(KTestName);
	GetEntryTzRuleAndCheckL(*systemTimeZoneRules, KUpdateEntry());
	
	CleanupStack::PopAndDestroy(systemTimeZoneRules);
	}



void CTCalBasicTest::TestTzInstancesL()
	{
	test.Next(_L("Test creating and fetching instances with Tz rules"));

	_LIT8(KUpdateEntry, "Instances and delete");
	iTestLib->CleanDatabaseL();
	
	iTestLib->OpenFileL(KTestName);
//	Store an entry
	TCalLocalUid  calLocalUid = StoreEntryL(KUpdateEntry(), KTokyo(), ETrue);
//	Test the number of instances, set the time up to cover the period when the instances were set up
	TDateTime startTime(2007, ENovember, 15, 0, 0, 0, 0);
	TDateTime endTime(2007, ENovember, 25, 0, 0, 0, 0);
	TestInstanceViewL(startTime, endTime, 10);

//	Update the entry start time
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iTestLib->SynCGetEntryViewL().FetchL(KUpdateEntry(), entryArray);
	TInt numEntries(0);
	test.Printf(_L("Update entry to start the appointment 30 minutes later\n"));
	TDateTime startTimeNew(2007, ENovember, 15, 19, 30, 0, 0);
	TDateTime endTimeNew(2007, ENovember, 15, 20, 0, 0, 0);
	TCalTime startCalTime;
	TCalTime endCalTime;
	startCalTime.SetTimeLocalL(startTimeNew);
	endCalTime.SetTimeLocalL(endTimeNew);
	entryArray[0]->SetStartAndEndTimeL(startCalTime, endCalTime);
	iTestLib->SynCGetEntryViewL().UpdateL(entryArray, numEntries);
	CleanupStack::PopAndDestroy(&entryArray);
	iTestLib->SetTimeZoneL(KHelsinki());
//	Test the number of instances
	test.Printf(_L("Testing the number of instances, which should be 10\n"));
	TestInstanceViewL(startTime, endTime, 10);
//  Delete the entry
	DeleteEntryL(KUpdateEntry());
	TestInstanceViewL(startTime, endTime, 0);
//	Shutdown server
	iTestLib->CloseAgendaServer();
//	Check that there are no instances		
	iTestLib->OpenFileL(KTestName);
	TestInstanceViewL(startTime, endTime, 0);
	}


void CTCalBasicTest::TestImportExportL()
	{
	test.Next(_L("Test importing and exporting entries with Tz rules"));

	_LIT8(KImportExportEntryTokyo, "Import export test Tokyo");
	_LIT8(KImportExportEntryHelsinki, "Import export test Helsinki");
	_LIT8(KImportExportEntryLondon, "Import export test London");
	_LIT(KExportTzExportFile, "tcal_tz_basic.vcs");
	iTestLib->OpenFileL(KTestName);
	iTestLib->CleanDatabaseL();
	
	
	test.Printf(_L("Store several entries with different time zones\n"));
	TCalLocalUid  calLocalUidTokyo = StoreEntryL(KImportExportEntryTokyo(), KTokyo(), ETrue);
	TCalLocalUid  calLocalUidHelsinki = StoreEntryL(KImportExportEntryHelsinki(), KHelsinki(), ETrue);
	TCalLocalUid  calLocalUidLondon = StoreEntryL(KImportExportEntryLondon(), KLondon(), ETrue);
	
	// Retrieve them and keep a record of their time zone rules
	CTzRules* TokyoTzRule = GetTimeZoneRuleL(KImportExportEntryTokyo());
	CleanupStack::PushL(TokyoTzRule);
	CTzRules* HelsinkiTzRule = GetTimeZoneRuleL(KImportExportEntryHelsinki());
	CleanupStack::PushL(HelsinkiTzRule);
	CTzRules* LondonTzRule = GetTimeZoneRuleL(KImportExportEntryLondon());
	CleanupStack::PushL(LondonTzRule);
	
	// Export the entries
	test.Printf(_L("Export the entries\n"));
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	CCalEntry* calEntryTokyo = iTestLib->SynCGetEntryViewL().FetchL(calLocalUidTokyo);
	TTime tokyoStartTimeUtc = calEntryTokyo->StartTimeL().TimeUtcL();
	TTime tokyoStartTimeLocal = calEntryTokyo->StartTimeL().TimeLocalL();
	entryArray.AppendL(calEntryTokyo);
	CCalEntry* calEntryHelsinki = iTestLib->SynCGetEntryViewL().FetchL(calLocalUidHelsinki);
	TTime helsinkiStartTimeUtc = calEntryHelsinki->StartTimeL().TimeUtcL();
	TTime helsinkiStartTimeLocal = calEntryHelsinki->StartTimeL().TimeLocalL();
	entryArray.AppendL(calEntryHelsinki);
	CCalEntry* calEntryLondon = iTestLib->SynCGetEntryViewL().FetchL(calLocalUidLondon);
	TTime londonStartTimeUtc = calEntryLondon->StartTimeL().TimeUtcL();
	TTime londonStartTimeLocal = calEntryLondon->StartTimeL().TimeLocalL();
	entryArray.AppendL(calEntryLondon);
	
	iTestLib->ExportL(KExportTzExportFile(), entryArray);
	CleanupStack::PopAndDestroy(&entryArray);
	
//	Reset the Db
	iTestLib->CleanDatabaseL();
	
//	Import data from the exported file into the Db
	test.Printf(_L("Import the entries\n"));
	RPointerArray<CCalEntry> entryImportArray;
	CleanupResetAndDestroyPushL(entryImportArray);
	iTestLib->ImportL(KExportTzExportFile(), entryImportArray);
	test (entryImportArray.Count() == 3);
	test.Printf(_L("Store the entries\n"));
	TInt entriesStored(0);
	iTestLib->SynCGetEntryViewL().StoreL(entryImportArray, entriesStored);
	test(entryImportArray.Count()==entriesStored);
	
	CleanupStack::PopAndDestroy(&entryImportArray);
	CleanupStack::PopAndDestroy(3); //TokyoTzRule, HelsinkiTzRule, LondonTzRule
	}
	
void CTCalBasicTest::RunAllTestsL()
	{
	TestSettingTzRuleL();
	
	TestTzInstancesL();
	
	TestCopyEntryL(CCalEntry::ECopyAll);
	
	TestCopyEntryL(CCalEntry::EDontCopyId);
	
	TestUpdateEntryL();
	
	TestImportExportL();
	}
	

	
LOCAL_C void doMainL()
	{
	CTCalBasicTest* testManager = CTCalBasicTest::NewL();
	TRAPD(ret, testManager->RunAllTestsL());
	test(ret == KErrNone);
	delete testManager;
	}

/**

@SYMTestCaseID     PIM-TCAL-TZ-BASIC-0001

*/

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-TCAL-TZ-BASIC-0001 tcal_tz_basic"));

	test.Title();
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler();
	CActiveScheduler::Install(scheduler);
	TRAPD(ret, doMainL());
	delete scheduler;
	test(ret == KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

