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

#include "caltestlib.h"

RTest test(_L("TFAILDEL"));

static const TInt KMyError(-12345);

_LIT(KTestCalendarFile, "tfaildel");

class CTestApp : public CBase
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void AddEntriesL();
	void TestFailL(TInt aEntryToDelete);
	void StoreEntryL(const CCalEntry* aEntry);

private:
	void ConstructL();

private:
	CCalTestLibrary* iCalTestLib;
	CDesC8Array* iGuids;

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

void CTestApp::AddEntriesL()
	{
	HBufC8* guid = NULL;
	CCalEntry* entry = iCalTestLib->CreateCalEntryL(CCalEntry::EAppt, guid);
	CleanupStack::PushL(entry);
	iGuids->AppendL(*guid);
	TTime now;
	now.HomeTime();
	iCalTestLib->SetEntryStartAndEndTimeL(entry, now, now + TTimeIntervalMinutes(60));
	entry->SetSummaryL(_L("an appt"));
	StoreEntryL(entry);
	CleanupStack::PopAndDestroy(entry);
	
	guid = NULL;
	entry = iCalTestLib->CreateCalEntryL(CCalEntry::ETodo, guid);
	CleanupStack::PushL(entry);
	iGuids->AppendL(*guid);
	entry->SetSummaryL(_L("todo"));
	StoreEntryL(entry);
	CleanupStack::PopAndDestroy(entry);
	}


void CTestApp::TestFailL(TInt aEntryToDelete)
	{
#if defined(_DEBUG)
	TInt failAt(0);
#endif
	
	FOREVER
		{
		CDesC8Array* guids = new(ELeave) CDesC8ArrayFlat(1);
		CleanupStack::PushL(guids);
		
		guids->AppendL((*iGuids)[aEntryToDelete]);
		
#if defined(_DEBUG)
		iCalTestLib->FileSession().SetErrorCondition(KMyError, ++failAt);
#endif
		TRAPD(ret, iCalTestLib->SynCGetEntryViewL().DeleteL(*guids));
#if defined(_DEBUG)
		iCalTestLib->FileSession().SetErrorCondition(KErrNone);
#endif
		CleanupStack::PopAndDestroy(guids);
		
		test(ret == KErrNone || ret == KMyError || ret == KErrNotFound);
		
		// if the entry gets deleted from the store, then commit fails but the subsequnet revert fails also
		// then the entry cannot be found in the store next time around
		if (ret == KErrNone || ret == KErrNotFound)
			{
			break;
			}
		
		}
	}



static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);
	
	testApp->AddEntriesL();

	test.Next(_L("Test 1 - Delete Appointment"));

	testApp->TestFailL(0);
	
	test.Next(_L("Test 2 - Delete Todo"));

	testApp->TestFailL(1);
	
	CleanupStack::PopAndDestroy(testApp);
	}

/**

@SYMTestCaseID     PIM-TFAILDEL-0001

*/

TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-TFAILDEL-0001 TFAILDEL.CPP"));

	test.Title();
	CTrapCleanup* theCleanup = CTrapCleanup::New();
	CActiveScheduler* scheduler = new CActiveScheduler;
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

