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
#include <badesca.h>

#include <calrrule.h>
#include <calentryview.h>

#include "caltestlib.h"
#include "caltestoom.h"

RTest test(_L("TOOMDEL"));
_LIT(KTestCalendarFile, "toomdel");

const TInt KNumEntriesInStream(16);

static const TTime _1Jan96(TDateTime(1996,EJanuary,2,0,0,0,0));

class CTestApp : public CBase, public MCalTestOomCallBack
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void AddScrapEntriesL(TInt aNumEntries);
	void AddEntriesL(TBool aCreateAnotherStream);
	void DeleteEntryL(TInt aEntryToDelete, TBool aTestWithoutBeingLoaded);
	void ReOpenFileL();
	void StoreEntryL(const CCalEntry* aEntry);

private:
	void ConstructL();
	
public:
	void OomTestL(TType aType, TInt aFailAt);

private:
	CCalTestLibrary* iCalTestLib;
	CDesC8Array* iGuids;
	
	TInt iEntryToDelete;
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
	
	iGuids = new(ELeave) CDesC8ArrayFlat(8);
	}
		
CTestApp::~CTestApp()
	{
	delete iCalTestLib;
	}
	
void CTestApp::StoreEntryL(const CCalEntry* aEntry)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupClosePushL(entryArray);
	entryArray.AppendL(aEntry);
	TInt numSuc;
	iCalTestLib->SynCGetEntryViewL().StoreL(entryArray, numSuc);
	test(numSuc == entryArray.Count());
	CleanupStack::PopAndDestroy(&entryArray);
	}
	
void CTestApp::ReOpenFileL()
	{
	iCalTestLib->OpenFileL(KTestCalendarFile);
	}
	
void CTestApp::AddScrapEntriesL(TInt aNumEntries)
	{
	for (TInt i(0) ; i < aNumEntries ; ++i)
		{
		HBufC8* guidScrap = NULL;
		CCalEntry* scrap = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidScrap);
		CleanupStack::PushL(scrap);
		scrap->SetSummaryL(_L("Scrap"));
		iCalTestLib->SetEntryStartAndEndTimeL(scrap, _1Jan96, _1Jan96 + TTimeIntervalMinutes(60));
		StoreEntryL(scrap);
		CleanupStack::PopAndDestroy(scrap);
		}
	}

void CTestApp::AddEntriesL(TBool aCreateAnotherStream)
	{
	iGuids->Reset();
	
	HBufC8* guidAppt = NULL;
	CCalEntry* appt = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidAppt);
	CleanupStack::PushL(appt);
	iGuids->AppendL(*guidAppt);
	
	HBufC8* guidDayNote = NULL;
	CCalEntry* dayNote = iCalTestLib->CreateCalEntryL(CCalEntry::EReminder, guidDayNote);
	CleanupStack::PushL(dayNote);
	iGuids->AppendL(*guidDayNote);
	
	HBufC8* guidEvent = NULL;
	CCalEntry* event = iCalTestLib->CreateCalEntryL(CCalEntry::EEvent, guidEvent);
	CleanupStack::PushL(event);
	iGuids->AppendL(*guidEvent);
	
	HBufC8* guidAnniv = NULL;
	CCalEntry* anniv = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidAnniv);
	CleanupStack::PushL(anniv);
	iGuids->AppendL(*guidAnniv);
	
	HBufC8* guidTodo = NULL;
	CCalEntry* todo = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidTodo);
	CleanupStack::PushL(todo);
	iGuids->AppendL(*guidTodo);

	
	AddScrapEntriesL(KNumEntriesInStream);
	AddScrapEntriesL(KNumEntriesInStream - 1);
		
	appt->SetSummaryL(_L("Appt"));
	iCalTestLib->SetEntryStartAndEndTimeL(appt, _1Jan96, _1Jan96 + TTimeIntervalMinutes(60));
	TCalRRule rRule(TCalRRule::EDaily);
	TCalTime rRuleStartTime;
	rRuleStartTime.SetTimeLocalL(_1Jan96);
	TCalTime rRuleEndTime;
	rRuleEndTime.SetTimeUtcL(_1Jan96 + TTimeIntervalDays(30));
	rRule.SetDtStart(rRuleStartTime);
	rRule.SetUntil(rRuleEndTime);
	rRule.SetInterval(4);
	appt->SetRRuleL(rRule);
		
	StoreEntryL(appt);

	AddScrapEntriesL(KNumEntriesInStream - 1);
	
	iCalTestLib->SetEntryStartAndEndTimeL(dayNote, _1Jan96, _1Jan96);
	dayNote->SetSummaryL(_L("Day note"));
	
	StoreEntryL(dayNote);
		
	AddScrapEntriesL(KNumEntriesInStream - 1);
	
	iCalTestLib->SetEntryStartAndEndTimeL(event, _1Jan96, _1Jan96);
	event->SetSummaryL(_L("Event"));
	
	StoreEntryL(event);	

	AddScrapEntriesL(KNumEntriesInStream - 1);
	
	iCalTestLib->SetEntryStartAndEndTimeL(anniv, _1Jan96, _1Jan96);
	anniv->SetSummaryL(_L("Anniv"));
	StoreEntryL(anniv);

	AddScrapEntriesL(KNumEntriesInStream - 1);
	
	iCalTestLib->SetEntryStartAndEndTimeL(todo, _1Jan96, _1Jan96);
	todo->SetSummaryL(_L("Todo"));
	StoreEntryL(todo);
	
	// By adding some other entries
	// so that another stream is used it means fetching when the entries being tested are both
	// loaded in memory and not in memory can both be tested
	if (aCreateAnotherStream)
		{
		AddScrapEntriesL(30);
		HBufC8* guidScrap = NULL;
		CCalEntry* scrap = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidScrap);
		CleanupStack::PushL(scrap);
		iGuids->AppendL(*guidScrap);
		scrap->SetSummaryL(_L("Scrap"));
		iCalTestLib->SetEntryStartAndEndTimeL(scrap, _1Jan96, _1Jan96 + TTimeIntervalMinutes(60));
		StoreEntryL(scrap);
		CleanupStack::PopAndDestroy(scrap);
		}

	CleanupStack::PopAndDestroy(todo);
	CleanupStack::PopAndDestroy(anniv);
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(dayNote);
	CleanupStack::PopAndDestroy(appt);
	}
	
void CTestApp::OomTestL(TType /*aType*/, TInt /*aFailAt*/)
	{
	test.Printf(_L("."));
	CDesC8Array* guids = new(ELeave) CDesC8ArrayFlat(8);
	CleanupStack::PushL(guids);
	guids->AppendL((*iGuids)[iEntryToDelete]);
	iCalTestLib->SynCGetEntryViewL().DeleteL(*guids);
	CleanupStack::PopAndDestroy(guids);
	}


void CTestApp::DeleteEntryL(TInt aEntryToDelete, TBool aTestWithoutBeingLoaded)
	{
	if (aTestWithoutBeingLoaded)
		{
		test.Printf(_L("fetch the entry to be deleted\n"));
		// fetch an entry from another stream so that the one we are testing will not be loaded in memory,
		// causing it to be fetched first
		RPointerArray<CCalEntry> entryArray;
		CleanupResetAndDestroyPushL(entryArray);
		iCalTestLib->SynCGetEntryViewL().FetchL((*iGuids)[iGuids->Count()-1], entryArray);
		CleanupStack::PopAndDestroy(&entryArray);
		}
	
	test.Printf(_L("start the oom delete\n"));
	iEntryToDelete = aEntryToDelete;
	CCalSession& calSession = iCalTestLib->GetSession();
	TRAPD(err, CalTestOom::OutOfMemoryTestL(*this, EClient, calSession));
	test(err == KErrNone || err ==KErrNotFound);
	TRAP(err, CalTestOom::OutOfMemoryTestL(*this, EServer, calSession));
	test(err == KErrNone || err ==KErrNotFound);
	}



static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);

	test.Next(_L("Adding entries ..."));

	testApp->AddEntriesL(ETrue);
	
	test.Next(_L("testing day appt"));

	testApp->DeleteEntryL(0, ETrue);

	test.Next(_L("testing day note"));

	testApp->DeleteEntryL(1, ETrue);

	test.Next(_L("testing event"));

	testApp->DeleteEntryL(2, ETrue);

	test.Next(_L("testing anniv"));

	testApp->DeleteEntryL(3, ETrue);
	
	test.Next(_L("testing todo"));

	testApp->DeleteEntryL(4, ETrue);

	testApp->ReOpenFileL();
	
	test.Next(_L("Adding entries ..."));

	testApp->AddEntriesL(EFalse);
	
	test.Next(_L("testing appt"));

	testApp->DeleteEntryL(0, EFalse);

	test.Next(_L("testing day note"));

	testApp->DeleteEntryL(1 ,EFalse);

	test.Next(_L("testing event"));

	testApp->DeleteEntryL(2, EFalse);

	test.Next(_L("testing anniv"));

	testApp->DeleteEntryL(3, EFalse);

	test.Next(_L("testing todo"));

	testApp->DeleteEntryL(4, EFalse);

	CleanupStack::PopAndDestroy(testApp);
	}

/**

@SYMTestCaseID     PIM-TOOMDEL-0001

*/

TInt E32Main()
	{	

	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-TOOMDEL-0001 TOOMDEL.CPP"));

	test.Title();
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	TRAPD(ret,doMainL());
	delete scheduler;	
	if (ret != KErrNone)
		test.Printf(_L("ret is:%d\n"),ret);
	test(ret==KErrNone);
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

