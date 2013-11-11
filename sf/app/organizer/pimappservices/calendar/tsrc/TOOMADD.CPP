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
#include <txtrich.h>
#include <conpics.h>
#include <e32test.h>

#include <badesca.h> 

#include "caltestlib.h"
#include "caltestoom.h"

#include <calentryview.h>

RTest test(_L("TOOMADD"));

_LIT(KTestCaledarFile, "toomadd");

class CTestApp : public CBase , public MCalTestOomCallBack
	{
public:
	static CTestApp* NewL();
	~CTestApp();
	
	void TestWithUserHeapFailureL(CCalEntry::TType aEntryType);
	void StoreEntryL(const CCalEntry* aEntry);

private:
	void ConstructL();	
	
public: // from MCalOomTestCallBack
	void OomTestL(TType aType, TInt aFailAt);

private:
	CCalTestLibrary* iCalTestLib;
	CCalEntry::TType iEntryType; // used in oom test
	
	CCalEntry* iOomEntryAppt;
	CCalEntry* iOomEntrySaved;
	
	TBool iStoreOnly;
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
	iCalTestLib->ReplaceFileL(KTestCaledarFile);
	iCalTestLib->OpenFileL(KTestCaledarFile);
	}
		
CTestApp::~CTestApp()
	{
	TRAP_IGNORE(iCalTestLib->DeleteFileL(KTestCaledarFile ,ETrue));
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

void CTestApp::OomTestL(TType /*aType*/, TInt /*aFailAt*/)
	{
	test.Printf(_L("."));
	if (iStoreOnly)
		{
		iOomEntrySaved->CompareL(*iOomEntryAppt);
		StoreEntryL(iOomEntryAppt);
		iOomEntrySaved->CompareL(*iOomEntryAppt);
		}
	else
		{
		HBufC8* guid = NULL;
		CCalEntry* entry = iCalTestLib->CreateCalEntryL(iEntryType, guid);
		CleanupStack::PushL(entry);
		iCalTestLib->SetEntryStartAndEndTimeL(entry, TTime(_L("20051123:120000.000000")), TTime(_L("20051123:130000.000000")));
		StoreEntryL(entry);
		CleanupStack::PopAndDestroy(entry);
		}
	}

void CTestApp::TestWithUserHeapFailureL(CCalEntry::TType aEntryType)
	{		
	// make sure the entry view has been created before we start the oom test
	iCalTestLib->SynCGetEntryViewL();
	iEntryType = aEntryType;
	CCalSession& calSession = iCalTestLib->GetSession();
	CalTestOom::OutOfMemoryTestL(*this, EClient, calSession);
	CalTestOom::OutOfMemoryTestL(*this, EServer, calSession);
	}

static void doMainL()
	{
	CTestApp* testApp = CTestApp::NewL();
	CleanupStack::PushL(testApp);

	test.Next(_L("Starting Testing CAgnEntryModel with user-heap failure"));
	
	testApp->TestWithUserHeapFailureL(CCalEntry::EAppt);
	testApp->TestWithUserHeapFailureL(CCalEntry::EEvent);	
	testApp->TestWithUserHeapFailureL(CCalEntry::EAnniv);
	testApp->TestWithUserHeapFailureL(CCalEntry::ETodo);
	testApp->TestWithUserHeapFailureL(CCalEntry::EReminder);
	
	CleanupStack::PopAndDestroy(testApp);
	}

/**

@SYMTestCaseID     PIM-TOOMADD-0001

*/

TInt E32Main()
	{	

	__UHEAP_MARK;
	test.Start(_L("@SYMTESTCaseID:PIM-TOOMADD-0001 TOOMADD.CPP"));

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

