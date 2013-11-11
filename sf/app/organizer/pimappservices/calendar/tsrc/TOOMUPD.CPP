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
#include <conpics.h>
#include <e32test.h>

#include <calentryview.h>
#include <calrrule.h>

#include "caltestlib.h"
#include "caltestoom.h"

RTest test(_L("TOOMUPD"));

static const TTime _1Jan96(TDateTime(1996,EJanuary,2,0,0,0,0));

_LIT(KTestCalendarFile, "toomupd");

class CTestApp : public CBase, public MCalTestOomCallBack
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void AddSomeEntriesL(TBool aCreateAnotherStream);
	CCalEntry* FetchEntryLC(TInt aEntryToFetch);
	CCalEntry* CopyAndAlterEntryLC(const CCalEntry& aCalEntry);
	void StartUpdateEntryOomTestL(TInt aEntryToFetch, TBool aTestWithoutBeingLoaded, TType aType);
	void StoreEntryL(const CCalEntry* aEntry);

private:
	void ConstructL();
	
public: // from MCalTestOomCallBack
	void OomTestL(TType aType, TInt aFailAt);

private:
	CCalTestLibrary* iCalTestLib;
	
	CDesC8Array* iGuids;
	
	TBool iTestWithoutBeingLoaded;
	
	TInt iEntryToFetch;
	
	CCalEntry* iOriginalEntry; // not owned
	CCalEntry* iFetchedEntry; // not owned
	CCalEntry* iUpdatedEntry; // not owned
	
	TType iType;
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
	delete iGuids;
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

void CTestApp::AddSomeEntriesL(TBool aCreateAnotherStream)
	{
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
	CCalEntry* anniv = iCalTestLib->CreateCalEntryL(CCalEntry::EAnniv, guidAnniv);
	CleanupStack::PushL(anniv);
	iGuids->AppendL(*guidAnniv);
	
	HBufC8* guidTodo = NULL;
	CCalEntry* todo = iCalTestLib->CreateCalEntryL(CCalEntry::ETodo, guidTodo);
	CleanupStack::PushL(todo);
	iGuids->AppendL(*guidTodo);
	
	iCalTestLib->SetEntryStartAndEndTimeL(appt, _1Jan96, _1Jan96 + TTimeIntervalMinutes(60));
	appt->SetSummaryL(_L("A"));

	TCalRRule rRuleAppt(TCalRRule::EDaily);
	// set rRule start
	TCalTime startTime;
	startTime.SetTimeLocalL(_1Jan96);
	rRuleAppt.SetDtStart(startTime);
	
	// set rRule end
	TCalTime endTime;
	endTime.SetTimeLocalL(_1Jan96 + TTimeIntervalDays(30));
	rRuleAppt.SetUntil(endTime);
	
	rRuleAppt.SetInterval(4);
	
	appt->SetRRuleL(rRuleAppt);

	StoreEntryL(appt);

	dayNote->SetSummaryL(_L("D"));
	iCalTestLib->SetEntryStartAndEndTimeL(dayNote, _1Jan96, _1Jan96);
	StoreEntryL(dayNote);

	event->SetSummaryL(_L("E"));
	iCalTestLib->SetEntryStartAndEndTimeL(event, _1Jan96, _1Jan96);
	StoreEntryL(event);	

	anniv->SetSummaryL(_L("N"));
	iCalTestLib->SetEntryStartAndEndTimeL(anniv, _1Jan96, _1Jan96);
	StoreEntryL(anniv);

	todo->SetSummaryL(_L("T"));
	iCalTestLib->SetEntryStartAndEndTimeL(todo, _1Jan96, _1Jan96);
	StoreEntryL(todo);

	
	// The first 4 entries will be	put into the first stream, by adding some other entries
	// so that another stream is used it means fetching when the entries being tested are both
	// loaded in memory and not in memory can both be tested
	if (aCreateAnotherStream)
		{
		const TInt KNumExtraEntries(14);
		for (TInt i(0) ; i < KNumExtraEntries ; ++i)
			{
			HBufC8* guid = NULL;
			CCalEntry* scrapEvent = iCalTestLib->CreateCalEntryL(CCalEntry::EEvent, guid);
			CleanupStack::PushL(scrapEvent);
			iCalTestLib->SetEntryStartAndEndTimeL(scrapEvent, _1Jan96, _1Jan96);
			scrapEvent->SetSummaryL(_L("Scrap"));
			StoreEntryL(scrapEvent);
			if ( i == KNumExtraEntries - 1 )
				{
				// store the guid of the last of these entries
				iGuids->AppendL(*guid);
				}
			CleanupStack::PopAndDestroy(scrapEvent);
			}
		}

	CleanupStack::PopAndDestroy(todo);
	CleanupStack::PopAndDestroy(anniv);
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(dayNote);
	CleanupStack::PopAndDestroy(appt);
	}

CCalEntry* CTestApp::FetchEntryLC(TInt aEntryToFetch)
	{
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iCalTestLib->SynCGetEntryViewL().FetchL((*iGuids)[aEntryToFetch], entryArray);
	test(entryArray.Count() == 1);
	CCalEntry* entry = entryArray[0];
	entryArray.Remove(0);
	CleanupStack::PopAndDestroy(&entryArray);
	CleanupStack::PushL(entry);
	return entry;
	}
	
CCalEntry* CTestApp::CopyAndAlterEntryLC(const CCalEntry& aCalEntry)
	{
	HBufC8* guid = aCalEntry.UidL().AllocLC();
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(aCalEntry.EntryTypeL(), guid);
	CleanupStack::Pop(guid);
	CleanupStack::PushL(entry);
	
	entry->CopyFromL(aCalEntry);

	switch (aCalEntry.EntryTypeL())
		{
		case CCalEntry::EAppt:
			entry->SetSummaryL(_L("New appt title"));
			break;
		case CCalEntry::EReminder:
			entry->SetSummaryL(_L("New day note title"));
			break;		
		case CCalEntry::EEvent:
			entry->SetSummaryL(_L("New event title"));
			break;
		case CCalEntry::EAnniv:
			entry->SetSummaryL(_L("New anniv title"));
			break;
		case CCalEntry::ETodo:
			entry->SetSummaryL(_L("New todo title"));
			break;
		}
	
	return entry;
	}
	
//
// This function is called 5 times, 4 of the entries have memos and one does not.
// For the appt, which has a memo, its memo is deleted without first having been looked at (thus not loading it)
// For the daynote, which has a memo, its memo is deleted after having been looked at (thus it is loaded)
// For the event, which has a memo, its title is altered without its memo having been looked at
// For the anniv, which has a memo, its title is altered with its memo having been looked at
// For the todo, which hasn't a memo, it is given a memo.
//
void CTestApp::StartUpdateEntryOomTestL(TInt aEntryToFetch, TBool aTestWithoutBeingLoaded, TType aType)
	{
	iType = aType;
	iTestWithoutBeingLoaded = aTestWithoutBeingLoaded;
	iEntryToFetch = aEntryToFetch;
	iOriginalEntry = FetchEntryLC(aEntryToFetch);
	iOriginalEntry->CompareL(*iOriginalEntry); // this is to make sure the summary is loaded 
	CCalSession& calSession = iCalTestLib->GetSession();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);	
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);	
	CleanupStack::PopAndDestroy(iOriginalEntry);
	}

void CTestApp::OomTestL(TType aType, TInt /*aFailAt*/)
	{
	if (aType == iType)
		{
		test.Printf(_L("."));
		
		iFetchedEntry = FetchEntryLC(iEntryToFetch);
		
		// compare the fetched and original to make sure it hasn't changed
		test(iFetchedEntry->CompareL(*iOriginalEntry));
		
		iUpdatedEntry = CopyAndAlterEntryLC(*iFetchedEntry);
		
		if (iTestWithoutBeingLoaded)
			{
			// fetch an entry from another stream
			RPointerArray<CCalEntry> entryArray;
			CleanupResetAndDestroyPushL(entryArray);
			iCalTestLib->SynCGetEntryViewL().FetchL((*iGuids)[iGuids->Count() - 1], entryArray);
			CleanupStack::PopAndDestroy();
			}
			
		RPointerArray<CCalEntry> entryArray;
		CleanupClosePushL(entryArray);
		entryArray.AppendL(iUpdatedEntry);
		TInt numSuc;
		iCalTestLib->SynCGetEntryViewL().UpdateL(entryArray, numSuc);
		CleanupStack::PopAndDestroy(&entryArray);
		CleanupStack::PopAndDestroy(iUpdatedEntry);
		CleanupStack::PopAndDestroy(iFetchedEntry);
		}
	}


static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);

// Test with the entry being updated not already present in memory
	test.Next(_L("Adding entries ..."));

	testApp->AddSomeEntriesL(ETrue);
	
	test.Next(_L("testing appt"));

	testApp->StartUpdateEntryOomTestL(0, ETrue, MCalTestOomCallBack::EClient);
	test.Next(_L("testing day note"));

	testApp->StartUpdateEntryOomTestL(1, ETrue, MCalTestOomCallBack::EClient);
	test.Next(_L("testing event"));

	testApp->StartUpdateEntryOomTestL(2, ETrue, MCalTestOomCallBack::EClient);
	test.Next(_L("testing anniv"));

	testApp->StartUpdateEntryOomTestL(3, ETrue, MCalTestOomCallBack::EClient);
	test.Next(_L("testing todo"));

	testApp->StartUpdateEntryOomTestL(4, ETrue, MCalTestOomCallBack::EClient);
	
    // Test with the entry being updated already present in memory
	test.Next(_L("Adding entries 2..."));

	testApp->AddSomeEntriesL(EFalse);
	
	test.Next(_L("testing appt"));

	testApp->StartUpdateEntryOomTestL(0, EFalse, MCalTestOomCallBack::EClient);
	test.Next(_L("testing day note"));

	testApp->StartUpdateEntryOomTestL(1, EFalse, MCalTestOomCallBack::EClient);
	test.Next(_L("testing event"));

	testApp->StartUpdateEntryOomTestL(2, EFalse, MCalTestOomCallBack::EClient);
	test.Next(_L("testing anniv"));

	testApp->StartUpdateEntryOomTestL(3, EFalse, MCalTestOomCallBack::EClient);
	test.Next(_L("testing todo"));

	testApp->StartUpdateEntryOomTestL(4, EFalse, MCalTestOomCallBack::EClient);
	
	// Test with the entry being updated not already present in memory
	test.Next(_L("Adding entries ..."));

	testApp->AddSomeEntriesL(ETrue);
	
	test.Next(_L("testing appt"));

	testApp->StartUpdateEntryOomTestL(0, ETrue, MCalTestOomCallBack::EServer);
	test.Next(_L("testing day note"));

	testApp->StartUpdateEntryOomTestL(1, ETrue, MCalTestOomCallBack::EServer);
	test.Next(_L("testing event"));

	testApp->StartUpdateEntryOomTestL(2, ETrue, MCalTestOomCallBack::EServer);
	test.Next(_L("testing anniv"));

	testApp->StartUpdateEntryOomTestL(3, ETrue, MCalTestOomCallBack::EServer);
	test.Next(_L("testing todo"));

	testApp->StartUpdateEntryOomTestL(4, ETrue, MCalTestOomCallBack::EServer);
	
    // Test with the entry being updated already present in memory
	test.Next(_L("Adding entries 2..."));

	testApp->AddSomeEntriesL(EFalse);
	
	test.Next(_L("testing appt"));

	testApp->StartUpdateEntryOomTestL(0, EFalse, MCalTestOomCallBack::EServer);
	test.Next(_L("testing day note"));

	testApp->StartUpdateEntryOomTestL(1, EFalse, MCalTestOomCallBack::EServer);
	test.Next(_L("testing event"));

	testApp->StartUpdateEntryOomTestL(2, EFalse, MCalTestOomCallBack::EServer);
	test.Next(_L("testing anniv"));

	testApp->StartUpdateEntryOomTestL(3, EFalse, MCalTestOomCallBack::EServer);
	test.Next(_L("testing todo"));

	testApp->StartUpdateEntryOomTestL(4, EFalse, MCalTestOomCallBack::EServer);
	
	CleanupStack::PopAndDestroy(testApp);
	}

/**

@SYMTestCaseID     PIM-TOOMUPD-0001

*/

TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-TOOMUPD-0001 TOOMUPD"));

	test.Title();
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	TRAPD(ret,doMainL());
	delete scheduler;	
	test(ret==KErrNone);
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

