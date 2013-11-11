// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Class test for CSmsSettings.
// To build: bldmake bldfiles
// abld test build wins udeb
// To run from windows command prompt:
// cd epoc32\release\wins\udeb 
// T_CSMSSETTINGS -dtextshell --
// 
//
 

#include <e32cons.h>
#include <e32test.h>
#include <s32file.h>

// Message Server headers for linking to msgs.dll. 
#include "msvapi.h"
#include "msvids.h"
#include "msvuids.h"

// Class under test.
#include "smutset.h"
#include <csmsaccount.h>

// Test utility classes
class CMyScheduler : public CActiveScheduler
	{
private:
	void Error(TInt aError) const;
	};
void CMyScheduler::Error(TInt /*aError*/) const {}


class CObserver : public MMsvSessionObserver
	{
	public:
		void HandleSessionEvent(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
		void HandleSessionEventL(TMsvSessionEvent aEvent, TAny* aArg1, TAny* aArg2, TAny* aArg3);
	};
void CObserver::HandleSessionEvent(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/) {}
void CObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)	{}


// Test Case Prototypes
LOCAL_C TBool TestConstructionL();
LOCAL_C TBool TestRestoreWithNoStoreL();
LOCAL_C TBool TestRestoreWhenOnlyExternalSettingsIsPresentL();
LOCAL_C TBool TestRestoreWhenOnlyStoreSettingsIsPresentL();
LOCAL_C TBool TestStoreL();
LOCAL_C TBool TestSaveExternalL();
LOCAL_C TBool TestRestoreWhenExternalSettingsIsMoreRecentL();
LOCAL_C TBool TestRestoreWhenStoreSettingsIsMoreRecentL();


// Test Harness Prototypes
LOCAL_C void InitL();
LOCAL_C void UninitL();
LOCAL_C void doTest();
LOCAL_C void doMainL();
GLDEF_C TInt E32Main();


// Test harness constants and global variables...
_LIT(KTitle, "T_CSMSSETTINGS");
_LIT(KTxtPressAnyKey, "Press any key to return.");
CTrapCleanup* pcleanup = NULL;
CMyScheduler* pscheduler = NULL;
RTest test(KTitle);
TMsvId gTestEntryId;
RFs gFs;
CMsvSession* pSession = NULL;
CObserver* pObserver = NULL;
CMsvEntry* pContext = NULL;
const TUid KMsgFileSmsSettingsUid = {0x1000996E};
_LIT(KExternalSettingsFilename, "c:\\system\\data\\sms_settings.dat");


LOCAL_C TBool TestConstructionL()
	{
	// Test construction via NewL().
	__UHEAP_MARK;
	CSmsSettings* obj = NULL;
	obj = CSmsSettings::NewL();
	CleanupStack::PushL(obj);
	test(obj != NULL);
	CleanupStack::PopAndDestroy();
	obj = NULL;
	__UHEAP_MARKEND;

	// Test for memory leaks during heap cell failure at specific allocations.
	TInt failureRate = 0;
	while (failureRate < 32)
		{
		__UHEAP_MARK;
		__UHEAP_SETFAIL(RHeap::EDeterministic, failureRate++);
		TRAPD(err, obj = CSmsSettings::NewL());
		__UHEAP_RESET;
		if (err == KErrNone)
			{
			delete obj;
			obj = NULL;
			}
		else
			{
			test(err == KErrNoMemory);
			}
		__UHEAP_MARKEND;
		}

	return ETrue;
	}


LOCAL_C TBool TestRestoreWhenOnlyStoreSettingsIsPresentL()
/**
Calling CSmsSettings::RestoreL should return KErrNone, and the settings
should be set to that of the store.
*/
	{
	// Make sure there is no external settings.
	gFs.Delete(KExternalSettingsFilename);

	__UHEAP_MARK;
	CSmsAccount* account = CSmsAccount::NewLC();

	// Save settings to store only.
	CSmsSettings* obj = CSmsSettings::NewL();
	CleanupStack::PushL(obj);
	obj->SetDescriptionLength(3446238);
	account->SaveSettingsL(*obj);
	CleanupStack::PopAndDestroy(obj);

	// Restore settings and check it is from the store.
	obj = CSmsSettings::NewL();
	CleanupStack::PushL(obj);
	
	// Test that the settings are un-initialised.
	test(obj->DescriptionLength() != 3446238);
	account->LoadSettingsL(*obj);
	
	// Test that the settings are comming from the store.
	test(obj->DescriptionLength() == 3446238);
	
	CleanupStack::PopAndDestroy(obj);
	CleanupStack::PopAndDestroy(account);
	__UHEAP_MARKEND;
	
	return ETrue;
	}
	
	


LOCAL_C void InitL()
	{
	// Connect to the file system...
	gFs.Connect();

	// Connect to the Message Server...
	pObserver = new (ELeave) CObserver();
	CleanupStack::PushL(pObserver);
	pSession = CMsvSession::OpenSyncL(*pObserver);
	CleanupStack::PushL(pSession);

	// Create a test entry with some dummy values so the Message Server
	// will not panic when the entry is created in the message store...
	pContext = CMsvEntry::NewL(*pSession, KMsvRootIndexEntryIdValue, TMsvSelectionOrdering());
	CleanupStack::PushL(pContext);
	TMsvEntry testEntry;
	TUid entryMtm;
	entryMtm.iUid = 11;
	testEntry.iMtm = entryMtm;
	testEntry.iServiceId = pContext->OwningService();
	testEntry.iType = KUidMsvServiceEntry;

	// Create the entry in the message store and set the current context to it...
	pContext->CreateL(testEntry);
	gTestEntryId = testEntry.Id();
	pContext->SetEntryL(gTestEntryId);
	}


LOCAL_C void UninitL()
	{
	gFs.Delete(KExternalSettingsFilename);
	pContext->SetEntryL(KMsvRootIndexEntryIdValue);
	pContext->DeleteL(gTestEntryId);
	CleanupStack::PopAndDestroy(pContext);
	CleanupStack::PopAndDestroy(pSession);
	CleanupStack::PopAndDestroy(pObserver);
	gFs.Close();
	}


LOCAL_C void doTest()
	{
	test.Start(_L("Testing construction/destruction"));
	test(TestConstructionL());
	test.Next(_L("Testing RestoreL when only store settings are present"));
	test(TestRestoreWhenOnlyStoreSettingsIsPresentL());
	test.End();
	}


LOCAL_C void doMainL()
	{
	InitL();
	doTest();
	test.Getch();
	UninitL();
	}


GLDEF_C TInt E32Main()
	{
	pcleanup = CTrapCleanup::New();
	pscheduler = new (ELeave) CMyScheduler();
	CActiveScheduler::Install(pscheduler);
	TRAPD(ret,doMainL());		
	delete pcleanup;
	return(KErrNone);
	}
