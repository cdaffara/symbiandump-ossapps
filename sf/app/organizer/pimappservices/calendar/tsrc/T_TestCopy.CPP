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

#include <s32file.h>
#include <e32test.h> 

#include <txtrich.h>
#include <conpics.h>

#include <calentryview.h>
#include <calrrule.h>
#include <calcategory.h>
#include <calcontent.h>
#include <caluser.h>
#include <calalarm.h>

#include "caltestlib.h"
#include "caltestoom.h"

RTest test(_L("t_testcopy"));

static const TTime KStartTime(TDateTime(2005, ENovember, 23, 12, 0, 0, 0));

_LIT(KTestFile, "t_testcopy");

TBufC8<25> _SmallContent(_L8("small content"));
TBufC8<25> _MimeType(_L8("mime type"));

class CTestApp : public CBase, public MCalTestOomCallBack
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void SetEntryDetailsL(CCalEntry& aEntry);
	
	void TestCopyL();
	void OomTestCopyL();

public: // from MCalOomTestCallBack
	void OomTestL(TType aType, TInt aFailAt);

private:
	void ConstructL();
	
private:
	CCalTestLibrary* iCalTestLib;

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
	iCalTestLib->ReplaceFileL(KTestFile());
	iCalTestLib->OpenFileL(KTestFile());
	}
	
CTestApp::~CTestApp()
	{
	delete iCalTestLib;
	}
	
void CTestApp::OomTestCopyL()
	{
	test.Printf(_L("Starting out of memory test\n"));
	CCalSession& calSession = iCalTestLib->GetSession();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
 	}
	
void CTestApp::OomTestL(TType /*aType*/, TInt /*aFailAt*/)
	{
	test.Printf(_L("."));
	TestCopyL();
	}
	
void CTestApp::SetEntryDetailsL(CCalEntry& aEntry)
	{
	iCalTestLib->SetEntryStartAndEndTimeL(&aEntry, KStartTime, KStartTime + TTimeIntervalHours(1));
	
	// create a repeat rule
	TCalTime dtStart;
	dtStart.SetTimeLocalL(KStartTime);
	TCalTime until;
	until.SetTimeLocalL(KStartTime + TTimeIntervalDays(7));
	
	TCalRRule rRule(TCalRRule::EDaily);
	rRule.SetDtStart(dtStart);
	rRule.SetUntil(until);
	rRule.SetInterval(1);
	aEntry.SetRRuleL(rRule);
	
	// add an exception
	RArray<TCalTime> exceptions;
	CleanupClosePushL(exceptions);
	TCalTime exception;
	exception.SetTimeLocalL(KStartTime + TTimeIntervalDays(3));
	exceptions.AppendL(exception);
	aEntry.SetExceptionDatesL(exceptions);
	CleanupStack::PopAndDestroy(&exceptions);
	
	// add an rdate
	RArray<TCalTime> rDates;
	CleanupClosePushL(rDates);
	TCalTime rDate;
	rDate.SetTimeLocalL(KStartTime + TTimeIntervalDays(3) + TTimeIntervalHours(3));
	rDates.AppendL(rDate);
	aEntry.SetExceptionDatesL(rDates);
	CleanupStack::PopAndDestroy(&rDates);
	
	// add a category predefined and an extended
	CCalCategory* predefinedCategory = CCalCategory::NewL(CCalCategory::ECalBusiness);
	aEntry.AddCategoryL(predefinedCategory);
	CCalCategory* extendedCategory = CCalCategory::NewL(_L("Extended Category"));
	aEntry.AddCategoryL(extendedCategory);
	
	// oom test this
	aEntry.CategoryListL();
	
	CCalCategory* extendedCategory2 = CCalCategory::NewL(_L("dummy category"));
	aEntry.AddCategoryL(extendedCategory2);
	aEntry.DeleteCategoryL(2);
	
	// set a different time zone rule
	// ?
	
	// set the description
	aEntry.SetDescriptionL(_L("The Description"));
	
	// set the summary
	aEntry.SetSummaryL(_L("The Summary"));
	
	// set the organizer
	CCalUser* organizer = CCalUser::NewL(_L("organizer@symbian.com"));
	aEntry.SetOrganizerL(organizer);
	
	// set an attendee
	CCalAttendee* attendee = CCalAttendee::NewL(_L("attendee@symbian.com"));
	aEntry.AddAttendeeL(attendee);

	// set the phone owner
	RPointerArray<CCalAttendee>& attendList = aEntry.AttendeesL();
	aEntry.SetPhoneOwnerL(attendList[0]);
	
	// set priority
	aEntry.SetPriorityL(1);
	
	// add an alarm
	CCalAlarm* alarm = CCalAlarm::NewL();
	CleanupStack::PushL(alarm);
	
	alarm->SetTimeOffset(15);
	alarm->SetAlarmSoundNameL(_L("alarm sound"));
	
	CCalContent* content = CCalContent::NewL();
	CleanupStack::PushL(content);
	
	content->SetContentL(_MimeType.AllocLC(), _SmallContent.AllocLC(), CCalContent::EDispositionUnknown);
	CleanupStack::Pop(2);
	alarm->SetAlarmAction(content);
	CleanupStack::Pop(content);
	aEntry.SetAlarmL(alarm);
	CleanupStack::PopAndDestroy(alarm);
	}

void CTestApp::TestCopyL()
	{
	// create the original entry
	HBufC8* guidEntry = NULL;
	CCalEntry* originalEntry = iCalTestLib->CreateCalEntryL(CCalEntry::ETodo, guidEntry);
	CleanupStack::PushL(originalEntry);
	SetEntryDetailsL(*originalEntry);
		
	HBufC8* guidCopy = NULL;
	CCalEntry* copiedEntry = iCalTestLib->CreateCalEntryL(CCalEntry::ETodo, guidCopy);
	CleanupStack::PushL(copiedEntry);
	
	// copy and compare the entries
	copiedEntry->CopyFromL(*originalEntry);
	test(copiedEntry->CompareL(*originalEntry));
	
	// Save the copy in the DB
	RPointerArray<CCalEntry> calEntries;
	CleanupClosePushL(calEntries);
	calEntries.AppendL(copiedEntry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(calEntries, numSuc);
	CleanupStack::PopAndDestroy(&calEntries); //copiedEntry.Close()
	CleanupResetAndDestroyPushL(calEntries);
	
	// Read the copy out of the DB
	// and compare the fetched entry with the copy that was stored
	iCalTestLib->SynCGetEntryViewL().FetchL(copiedEntry->UidL(), calEntries);
	test(copiedEntry->CompareL(*calEntries[0]));
	test(originalEntry->CompareL(*calEntries[0]));
	calEntries.ResetAndDestroy();
	
	// Read the copy out of the DB
	// copy the the fetched entry and then compare with the original
	iCalTestLib->SynCGetEntryViewL().FetchL(copiedEntry->UidL(), calEntries);
	HBufC8* guidCopy2 = NULL;
	CCalEntry* copiedEntry2 = iCalTestLib->CreateCalEntryL(CCalEntry::ETodo, guidCopy2);
	CleanupStack::PushL(copiedEntry2);
	copiedEntry2->CopyFromL(*calEntries[0]);
	test(copiedEntry2->CompareL(*calEntries[0]));
	calEntries.ResetAndDestroy();
	
	// cleanup	
	CleanupStack::PopAndDestroy(copiedEntry2);
	CleanupStack::PopAndDestroy(&calEntries);
	CleanupStack::PopAndDestroy(copiedEntry);
	CleanupStack::PopAndDestroy(originalEntry);
	}

static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);
	
	testApp->TestCopyL();
	testApp->OomTestCopyL();
	
	CleanupStack::PopAndDestroy(testApp);
	}

/**

@SYMTestCaseID     PIM-T-TESTCOPY-0001

*/

TInt E32Main()
	{
	__UHEAP_MARK;
	
	test.Start(_L("@SYMTESTCaseID:PIM-T-TESTCOPY-0001 T_TestCopy"));

	test.Title();
	
	CTrapCleanup* CleanupTrap = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	
	delete scheduler;
	delete CleanupTrap;
	
	test.End();
	test.Close();
	
	__UHEAP_MARKEND;
	return(KErrNone);
	}

