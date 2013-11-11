// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_MIUT10A
// Component: IMCM
// Owner: 
// Brief description of test harness:
// Cachemanager tests - Automatic test that proves that the cachemanager 
// recursively prunes bodytext and attachment data.
// This is identical to T_MIUT10, except that it starts the cachemanager
// with a selection of message entries to prune instead of a service entry.
// Written as part of the fix for DEF043085, propagated from Hurricane (DEF042552).
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// \MSG\IMCMTSRC\plain_text.txt				<DRIVE>:\msgtest\IMCM\rfc822\plain_text.txt
// \MSG\IMCMTSRC\html_with_attachment.txt		<DRIVE>:\msgtest\IMCM\rfc822\html_with_attachment.txt
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_MIUT10A\Entry_822Headers.txt
// <DRIVE>:\msglogs\T_MIUT10A\Entry_MimeHeaders.txt
// <DRIVE>:\msglogs\T_MIUT10A\Entry_RichTextBodies.txt
// <DRIVE>:\msglogs\T_MIUT10A\Entry_Structure.txt
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT10A.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT10A.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT10A.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT10A.exe on the other platform
// 
//

#include "emailtestutils.h"
#include <miutlog.h>
#include <cacheman.h>

// local variables etc //

// Message file names
_LIT(K_T_MIUT10_PLAIN_MSG, "c:\\mailtest\\rfc822\\plain_text.txt");
_LIT(K_T_MIUT10_HTML_ATTACHMENT_MSG, "c:\\mailtest\\rfc822\\html_with_attachment.txt" );

// Logging strings
_LIT(K_T_MIUT10_TEST_INFO, "MIUT10A - Cache Management Tests");
_LIT(K_T_MIUT10_TEXT_LEFT, "The text stream on entry was not removed.");
_LIT(K_T_MIUT10_ATTACHMENT_LEFT, "The attachment on entry was not removed.");
_LIT(K_T_MIUT10_FILTER_FAILED, "The filter didn't work.");
_LIT(K_T_MIUT10_DESCRIPTION, "Testing pruning of a remote service containing plain test and MHTML mails.");
_LIT(K_T_MIUT10_PROGRESS, "   Messages pruned %d/%d		\n");

RTest test(K_T_MIUT10_TEST_INFO);

#define DONT_PRUNE_ID 0x100002
#define KErrBodyTextNotRemoved 5599
#define KErrAttachmentNotRemoved 5600
#define KErrFilterNotWorking 5601
#define KPeriod 10000	// period of timer
LOCAL_D TMsvId pop3Service;

LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;

class CFilteredCacheManager : public CImCacheManager
	{
public:
	CFilteredCacheManager(CMsvSession& aSession, TRequestStatus& aObserverRequestStatus);
	static CFilteredCacheManager* NewL(CMsvSession& aSession, TRequestStatus& aObserverRequestStatus);

private:
	TBool Filter() const;
	};

CFilteredCacheManager::CFilteredCacheManager(CMsvSession& aSession, TRequestStatus& aObserverRequestStatus) : CImCacheManager(aSession, aObserverRequestStatus)
	{
	}

CFilteredCacheManager* CFilteredCacheManager::NewL(CMsvSession& aSession, TRequestStatus& aObserverRequestStatus)
	{
	CFilteredCacheManager* self = new (ELeave) CFilteredCacheManager(aSession, aObserverRequestStatus);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

TBool CFilteredCacheManager::Filter() const
	{
	return (iCurrentEntry->Entry().Id() == DONT_PRUNE_ID) ? EFalse : ETrue;
	}


//


class CImCacheManager;
class TestUiTimer : public CTimer
	{
public:
	static TestUiTimer* NewLC(CConsoleBase* aConsole, CImCacheManager* aCacheManager);
	void RunL();
	void DoCancel();
	void ConstructL();
	void IssueRequest();
	
	CConsoleBase*				iConsole;
	CImCacheManager*			iCacheManager;
	TTimeIntervalMicroSeconds32 period;
protected:
	TInt iCount;
	TBool iClosing;
	TestUiTimer(CConsoleBase* aConsole, CImCacheManager* aCacheManager);
	};

//
// Progress timer
//	  
TestUiTimer* TestUiTimer::NewLC(CConsoleBase* aConsole, CImCacheManager* aCacheManager)
	{
	TestUiTimer* self = new(ELeave) TestUiTimer(aConsole, aCacheManager);
	CleanupStack::PushL(self);
	self->iCacheManager = aCacheManager;
	self->ConstructL();
	return self;
	}

TestUiTimer::TestUiTimer(CConsoleBase* aConsole, CImCacheManager* aCacheManager)
	: CTimer(EPriorityStandard+3), iCacheManager(aCacheManager)
	{
//	counter =0;
	iConsole = aConsole;
	period = KPeriod;
	iClosing = EFalse;
	iCount = 0;
	}

void TestUiTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void TestUiTimer::IssueRequest()
	{
	After(period);
	}

void TestUiTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

void TestUiTimer::RunL()
	{
	// display the current progress
	TImCacheManagerProgress temp;	
	TPckgC<TImCacheManagerProgress> paramPack(temp);

	const TDesC8& progBuf = iCacheManager->ProgressL();	
	paramPack.Set(progBuf);
	TImCacheManagerProgress progress=paramPack();	


	test.Console()->SetPos(0, 10);

	TBuf<128> strProgress;
	strProgress.Format(K_T_MIUT10_PROGRESS, progress.iMessagesProcessed,
				progress.iTotalMessages);
	test.Printf(strProgress);
	
	testUtils->WriteComment(strProgress);

	IssueRequest();
	};

//

LOCAL_C void Init()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();

	testUtils->WriteComment(K_T_MIUT10_TEST_INFO);
	
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler
	}

LOCAL_C void doMainL()
	{
	Init();
	TInt globalError = KErrNone;
	testUtils->FileSession().SetSessionPath(_L("c:\\"));

	testUtils->CreateSmtpServiceL();
	pop3Service = testUtils->CreatePopServiceL();

	testUtils->CreateMessageL(K_T_MIUT10_PLAIN_MSG, pop3Service, pop3Service);
	testUtils->CreateMessageL(K_T_MIUT10_PLAIN_MSG, pop3Service, pop3Service);
	testUtils->CreateMessageL(K_T_MIUT10_HTML_ATTACHMENT_MSG, pop3Service, pop3Service);

	test.Printf(_L("\nPerforming Cache Management Tests\n"));
	
	testUtils->GoClientSideL();

	testUtils->iMsvEntry->SetEntryL(pop3Service);
	TMsvEntry entry = testUtils->iMsvEntry->Entry();
	entry.SetVisible(ETrue);
	testUtils->iMsvEntry->ChangeL(entry);


	// create a selection of message entries to pass to the cache manager
	CMsvEntrySelection* mySelection = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(mySelection);
	ASSERT(mySelection->Count() == 3);


	TRequestStatus observerStatus;
	CImCacheManager* cacheManager = CFilteredCacheManager::NewL(*testUtils->iMsvSession, observerStatus);
	CleanupStack::PushL(cacheManager);
	TestUiTimer* testUiTimer = TestUiTimer::NewLC(test.Console(), cacheManager);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	testActive->StartL();
	testUiTimer->IssueRequest();

	testUtils->TestStart(0, K_T_MIUT10_DESCRIPTION);

	cacheManager->StartL(*mySelection, testActive->iStatus);

	CActiveScheduler::Start();

	testUtils->FindChildrenL(KMsvRootIndexEntryId);
	CMsvStore* store=NULL;
	// added HasStore check for: DEF049479 CImCacheManager doesn't remove CMsvStores 
	// Check that the items have been removed...
	if (testUtils->iMsvEntry->HasStoreL())
		{
		testUtils->iMsvEntry->SetEntryL(0x100004);
		CMsvStore* store = testUtils->iMsvEntry->ReadStoreL();
		CleanupStack::PushL(store);

		// Check that body text has been removed
		if (store->HasBodyTextL())
			{
			// Body text hasn't been removed.
			testUtils->WriteComment(K_T_MIUT10_TEXT_LEFT);
			globalError = KErrBodyTextNotRemoved;
			// User::Leave(KErrBodyTextNotRemoved);
			}
		CleanupStack::PopAndDestroy(store); 
		store=NULL;
		}
	test.Printf(_L("\nBody Text Removed\n"));

	if (testUtils->iMsvEntry->HasStoreL())
		{
		store = testUtils->iMsvEntry->ReadStoreL();
		CleanupStack::PushL(store);
	// Check that empty directory exists (try deleting it)
		if (testUtils->FileSession().RmDir(_L("c:\\private\\mail\\1000484b\\mail\\00100001_S\\b\\")) != KErrNone)	
			{
			// Attachment data hasn't been removed.
			testUtils->WriteComment(K_T_MIUT10_ATTACHMENT_LEFT);
			globalError = KErrAttachmentNotRemoved;
			}
			CleanupStack::PopAndDestroy(store); 
		}
	test.Printf(_L("\nAttachment Removed\n"));
	store=NULL;

	if (testUtils->iMsvEntry->HasStoreL())
		{
		testUtils->iMsvEntry->SetEntryL(0x100003);
		store=testUtils->iMsvEntry->ReadStoreL();
		CleanupStack::PushL(store);
		if (!(store->HasBodyTextL()))
			{
			// The filter hasn't worked
			testUtils->WriteComment(K_T_MIUT10_FILTER_FAILED);
			globalError = KErrFilterNotWorking;
			}
		CleanupStack::PopAndDestroy(store);
		store=NULL;
		}
	testUtils->TestFinish(0, globalError);

	if (globalError == KErrNone)
		{
		testUtils->TestHarnessCompleted();
		}
	else
		{
		testUtils->TestHarnessFailed(globalError);
		}

	CleanupStack::PopAndDestroy(4); // mySelection, testActive, testUiTimer, cacheManager

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_MIUT10A Test CImCacheManager class\n"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}

