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
// Name of test harness: T_MIUT10
// Component: IMCM
// Owner: EH
// Brief description of test harness:
// Cachemanager tests - Automatic test that proves that the cachemanager 
// recursively prunes bodytext and attachment data.
// Detailed description of test harness:
// The test harness checks that each message has no child entires since,
// prune-messages should leave message entry 
// and therefore each message should have no child entries.
// please see function doMainL(), section commented DEF066273 
// for further details.
// Input files required to run test harness:
// \MSG\IMCMTSRC\plain_text.txt				<DRIVE>:\msgtest\IMCM\rfc822\plain_text.txt
// \MSG\IMCMTSRC\html_with_attachment.txt		<DRIVE>:\msgtest\IMCM\rfc822\html_with_attachment.txt
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_MIUT10\Entry_822Headers.txt
// <DRIVE>:\msglogs\T_MIUT10\Entry_MimeHeaders.txt
// <DRIVE>:\msglogs\T_MIUT10\Entry_RichTextBodies.txt
// <DRIVE>:\msglogs\T_MIUT10\Entry_Structure.txt
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT10.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT10.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT10.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT10.exe on the other platform
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
_LIT(K_T_MIUT10_TEST_INFO, "MIUT10 - Cache Management Tests");
_LIT(K_T_MIUT10_DESCRIPTION, "Testing pruning of a remote service containing plain test and MHTML mails.");
_LIT(K_T_MIUT10_PROGRESS, "   Messages pruned %d/%d		\n");
_LIT(K_T_MIUT10_MSG_NOT_PRUNED,"\nWarning email not pruned\n* BODY text or attachment data still exists!*---Message entry id: %d--");
_LIT(K_T_MIUT10_MSG_PRUNED,"\nSuccess: email  pruned!--Message entry id: %d--");

RTest test(K_T_MIUT10_TEST_INFO);

#define DONT_PRUNE_ID 0x100002
#define KPeriod 10000	// period of timer
#define KErrMsgNotPruned 5601
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

    TBuf<128> strProgress;
    strProgress.Format(K_T_MIUT10_PROGRESS, progress.iMessagesProcessed,
                progress.iTotalMessages);
                
    test.Printf(strProgress);
    
    // write progress info to log file, too (to test DEF042552)
    testUtils->WriteComment(strProgress);
 
	IssueRequest();
	};

//

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();

	testUtils->WriteComment(K_T_MIUT10_TEST_INFO);
	
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->ClearEmailAccountsL();
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler
	}

LOCAL_C void doMainL()
	{
	InitL();

	TInt globalError = KErrNone;
	testUtils->FileSession().SetSessionPath(_L("c:\\"));

	testUtils->GoClientSideL();
	testUtils->CreateSmtpServiceL();
	pop3Service = testUtils->CreatePopServiceL();
	testUtils->GoServerSideL();

	testUtils->CreateMessageL(K_T_MIUT10_PLAIN_MSG, pop3Service, pop3Service);
	testUtils->CreateMessageL(K_T_MIUT10_PLAIN_MSG, pop3Service, pop3Service);
	testUtils->CreateMessageL(K_T_MIUT10_HTML_ATTACHMENT_MSG, pop3Service, pop3Service);

	test.Printf(_L("\nPerforming Cache Management Tests\n"));
	
	testUtils->GoClientSideL();

	testUtils->iMsvEntry->SetEntryL(pop3Service);
	TMsvEntry entry = testUtils->iMsvEntry->Entry();
	entry.SetVisible(ETrue);
	testUtils->iMsvEntry->ChangeL(entry);
	


	

	TRequestStatus observerStatus;
	CImCacheManager* cacheManager = CFilteredCacheManager::NewL(*testUtils->iMsvSession, observerStatus);
	CleanupStack::PushL(cacheManager);
	TestUiTimer* testUiTimer = TestUiTimer::NewLC(test.Console(), cacheManager);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	testActive->StartL();
	testUiTimer->IssueRequest();

	testUtils->TestStart(0, K_T_MIUT10_DESCRIPTION);

	cacheManager->StartL(pop3Service, testActive->iStatus);

	CActiveScheduler::Start();
	
//	DEF066273: rewrite-Messaging regression test T_MIUT10 failing consistently
//  DEF049479: is no longer valid it has therefore not been taken into consideration

    //ensure at correct service entry
	testUtils->iMsvEntry->SetEntryL(pop3Service);
    //store msg entires of this service 
	CMsvEntrySelection* popEntrySelection;
   	popEntrySelection=testUtils->iMsvEntry->ChildrenL();	
    CleanupStack::PushL(popEntrySelection);
    
    TInt count=popEntrySelection->Count();
    TInt childEntries;
   	TMsvId msgId;
   	TMsvEntry msgEntry;
   	TBuf<128> msgFail,msgPass;
    				
    
   	for(TInt i=0; i<count; ++i)
		{
  
    	//ensure at correct location 
    		msgId=popEntrySelection->At(i);
    		testUtils->iMsvEntry->SetEntryL(msgId);
    	//	testUtils->iMsvEntry->Entry(); 
    	
    		//ensure that there are no more child entries
    	   	childEntries=testUtils->iMsvEntry->Count();
    	
    	    	if (childEntries!=0 && DONT_PRUNE_ID!=msgId)
    	    		{
    	    			//body text or attachments still exist
    	    		   	msgFail.Format(K_T_MIUT10_MSG_NOT_PRUNED,msgId);
    	   				testUtils->WriteComment(msgFail);
    	    			globalError = KErrMsgNotPruned;

    	    		}
    	    	else
    	    		{
    	    			//email successfully-prunned header saved.
    	    			msgPass.Format(K_T_MIUT10_MSG_PRUNED,msgId);
    	    			testUtils->WriteComment(msgPass);
    	    		
    	    		}
    	  
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

	CleanupStack::PopAndDestroy(4,cacheManager); // testActive, testUiTimer, cacheManager,popEntrySelection

	Closedown();

	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_MIUT10 Test CImCacheManager class\n"));
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

