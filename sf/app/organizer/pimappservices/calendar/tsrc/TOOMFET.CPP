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
#include <badesca.h>

#include <calentryview.h>
#include <calrrule.h>

#include "caltestlib.h"
#include "caltestoom.h"

RTest test(_L("TOOMFET"));

static const TTime _1Jan96(TDateTime(1996,EJanuary,2,0,0,0,0));

_LIT(KTestCalendarFile, "TOOMFET");

class CTestApp : public CBase, public MCalTestOomCallBack
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void AddSomeEntriesL();
	void FetchEntriesL(TInt aGuid, const TDesC& aTitle);
	void StoreEntryL(const CCalEntry* aEntry);

private:
	void ConstructL();

public: // from MCalOomTestCallBack
	void OomTestSetupL();
	void OomTestL(TType aType, TInt aFailAt);

private:
	CCalTestLibrary* iCalTestLib;
	CDesC8Array* iGuids;

	TInt iGuidToFetch;
	const TDesC* iTitle;
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

void CTestApp::AddSomeEntriesL()
	{
	HBufC8* guidAppt = NULL;
	CCalEntry* appt = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidAppt);
	CleanupStack::PushL(appt);
	iGuids->AppendL(*guidAppt);
	
	HBufC8* guidEvent = NULL;
	CCalEntry* event = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guidEvent);
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

	appt->SetSummaryL(_L("A"));
	iCalTestLib->SetEntryStartAndEndTimeL(appt, _1Jan96, _1Jan96 + TTimeIntervalMinutes(60));
	
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
	
		
	event->SetSummaryL(_L("xE"));
	iCalTestLib->SetEntryStartAndEndTimeL(event, _1Jan96, _1Jan96);
	
	StoreEntryL(event);

	anniv->SetSummaryL(_L("xN"));
	iCalTestLib->SetEntryStartAndEndTimeL(anniv, _1Jan96, _1Jan96);
	StoreEntryL(anniv);

	todo->SetSummaryL(_L("xT"));
	iCalTestLib->SetEntryStartAndEndTimeL(todo, _1Jan96, _1Jan96);
	StoreEntryL(todo);

	
	// The first 4 entries will be	put into the first stream, by adding some other entries
	// so that another stream is used it means fetching when the entries being tested are both
	// loaded in memory and not in memory can both be tested
	
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
		
	CleanupStack::PopAndDestroy(todo);
	CleanupStack::PopAndDestroy(anniv);
	CleanupStack::PopAndDestroy(event);
	CleanupStack::PopAndDestroy(appt);
	}

void CTestApp::OomTestSetupL()
	{
	}
	
void CTestApp::OomTestL(TType /*aType*/, TInt /*aFailAt*/)
	{
	test.Printf(_L("."));
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iCalTestLib->SynCGetEntryViewL().FetchL((*iGuids)[iGuidToFetch], entryArray);
	if (entryArray.Count() == 0)
		{
		User::Leave(KErrNoMemory);
		}
	else
		{
		test(entryArray.Count() == 1);
		}
	
	// test that the summary is correct
	TBuf<1> title;
	title.Append(entryArray[0]->SummaryL().Left(1));
	//test(title == *iTitle);
	
	CleanupStack::PopAndDestroy(); //entryArray.ResetAndDestroy();
	}

void CTestApp::FetchEntriesL(TInt aGuidToFetch, const TDesC& aTitle)
	{ 
	// fetch an entry from another stream so that the one we are testing will not be loaded in memory
	RPointerArray<CCalEntry> entryArray;
	CleanupResetAndDestroyPushL(entryArray);
	iCalTestLib->SynCGetEntryViewL().FetchL((*iGuids)[iGuids->Count() - 1], entryArray); // the last entry that was added will be in a different stream
	CleanupStack::PopAndDestroy(); //entryArray.ResetAndDestroy();

	iGuidToFetch = aGuidToFetch;
	iTitle = &aTitle;

	CCalSession& calSession = iCalTestLib->GetSession();
	// fetch it once
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	// now that its been fetched it should be in memory, so fetching it again will execute different code
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	
	// fetch it once
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	// now that its been fetched it should be in memory, so fetching it again will execute different code
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	}


static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);

	test.Next(_L("Adding entries ..."));

	testApp->AddSomeEntriesL();
	test.Next(_L("testing appt"));

	testApp->FetchEntriesL(0,_L("A"));
	test.Next(_L("testing event"));

	testApp->FetchEntriesL(1,_L("E"));
	test.Next(_L("testing anniv"));

	testApp->FetchEntriesL(2,_L("N"));
	test.Next(_L("testing todo"));

	testApp->FetchEntriesL(3,_L("T"));
/*
	test.Next(_L("Now testing with fast storing"));

	//view->Model()->SetBufferedStoring(ETrue);
	CAgnAppt* appt=AgnTest->CreateApptLC();
	appt->RichTextL()->InsertL(0,_L("xA"));
	appt->SetStartAndEndDateTime(_1Jan96);
	view->Model()->AddEntryL(appt);
	CleanupStack::PopAndDestroy(appt);
	FetchEntriesL(view,TAgnInstanceId(apptId,_1Jan96),_L("A"));
*/
	CleanupStack::PopAndDestroy(testApp);
	}


/**

@SYMTestCaseID     PIM-TOOMFET-0001

*/

TInt E32Main()
	{	

	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-TOOMFET-0001 TOOMFET.CPP"));

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

