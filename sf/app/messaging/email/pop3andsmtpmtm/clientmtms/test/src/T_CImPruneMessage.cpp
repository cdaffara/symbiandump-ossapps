// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Test for CImPruneMessage class. 
// This class removes body text and attachment data from messages, leaving the message structure intact
// 
//

#include "emailtestutils.h"
#include <miutlog.h>
#include <cacheman.h>
#include <cimprunemessage.h>

_LIT(K_PLAIN_MSG, "c:\\mailtest\\rfc822\\plain_text.txt");
_LIT(K_HTML_ATTACHMENT_MSG, "c:\\mailtest\\rfc822\\html_with_attachment.txt" );
_LIT(K_TEST_INFO, "T_CImPruneMessage Test");
_LIT(K_DESCRIPTION, "Testing pruning of messages containing text and attachment.");
_LIT(K_MSG_NOT_PRUNED,"\nWarning email not pruned\n* BODY text or attachment data still exists!*---Message entry id: %d--");
_LIT(K_MSG_PRUNED,"\nSuccess: email  pruned! Message entry id: %d ");
#define KErrMsgNotPruned 5601

RTest test(K_TEST_INFO);
LOCAL_D TMsvId pop3Service;
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D	TInt globalError = KErrNone;


LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();

	testUtils->WriteComment(K_TEST_INFO);
	
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->ClearEmailAccountsL();
	testUtils->GoServerSideL();
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler
	}

LOCAL_C void TestMessagePartsL(CMsvEntry& aEntry)
	{
	CMsvEntrySelection* sel = aEntry.ChildrenL();
	CleanupStack::PushL(sel);
	
	TInt count = sel->Count();
	for(TInt ii=0; ii<sel->Count(); ++ii)
		{
		aEntry.SetEntryL(sel->At(ii));

		CMsvStore* store = aEntry.ReadStoreL();
		CleanupStack::PushL(store);		
		if (store->IsPresentL(KMsvEntryRichTextBody))
			{
			// body text exist	
	    	globalError = KErrMsgNotPruned;
			}

		MMsvAttachmentManager& attachmentMgr = store->AttachmentManagerL();		
		if(attachmentMgr.AttachmentCount() > 0)
			{
			CMsvAttachment* attachmentInfo = attachmentMgr.GetAttachmentInfoL(0);
			CleanupStack::PushL(attachmentInfo);

			if ((attachmentInfo->Type() == CMsvAttachment::EMsvFile) && (attachmentInfo->Size() != 0))
				{
				// attachment data exist	
		   	 	globalError = KErrMsgNotPruned;			
				}
			CleanupStack::PopAndDestroy(attachmentInfo);	
			}
		CleanupStack::PopAndDestroy(store);	

		
		// recursively test other message parts
		TestMessagePartsL(aEntry);
		}
	CleanupStack::PopAndDestroy(sel);
	}

LOCAL_C void doMainL()
	{
	// CImPruneMessage class removes body text and attachment data from messages, 
	// leaving the message structure intact	
	
	InitL();

	testUtils->FileSession().SetSessionPath(_L("c:\\"));

	testUtils->GoClientSideL();
	pop3Service = testUtils->CreatePopServiceL();

	testUtils->GoServerSideL();
  
	testUtils->CreateMessageL(K_PLAIN_MSG, pop3Service, pop3Service);
	testUtils->CreateMessageL(K_HTML_ATTACHMENT_MSG, pop3Service, pop3Service);
	testUtils->CreateMessageL(K_PLAIN_MSG, pop3Service, pop3Service);	
	testUtils->CreateMessageL(K_HTML_ATTACHMENT_MSG, pop3Service, pop3Service);	
 
	test.Printf(_L("\nPerforming Prune Message Tests\n"));
	
	testUtils->GoClientSideL();

	testUtils->iMsvEntry->SetEntryL(pop3Service);
	TMsvEntry entry = testUtils->iMsvEntry->Entry();
	entry.SetVisible(ETrue);
	testUtils->iMsvEntry->ChangeL(entry);
	
	CImPruneMessage* pruneMessage = CImPruneMessage::NewL(*testUtils->iMsvEntry, testUtils->FileSession());
	CleanupStack::PushL(pruneMessage);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	testActive->StartL();

	testUtils->TestStart(0, K_DESCRIPTION);

	pruneMessage->StartL(pop3Service, testActive->iStatus);

	CActiveScheduler::Start();

	
	// Get a list of pop messages (created above)
	testUtils->iMsvEntry->SetEntryL(pop3Service);
	CMsvEntrySelection* popMessages = testUtils->iMsvEntry->ChildrenL();	
    CleanupStack::PushL(popMessages);
    
    TInt messageCount = popMessages->Count();
   	TMsvId msgId;
   	TBuf<128> msgFail;
   	TBuf<128> msgPass;    
    
   	for(TInt ii=0; ii<messageCount; ++ii)
		{
		// Check that there are no body text and no data in the attachment file for this message
    	msgId = popMessages->At(ii);
    	testUtils->iMsvEntry->SetEntryL(msgId);
  
  		// Go through the message parts and check that body text and attachment data are removed		    		
	    TestMessagePartsL(*testUtils->iMsvEntry);
	    
		if (globalError == KErrNone)
			{		
		    //email successfully prunned
		    msgPass.Format(K_MSG_PRUNED,msgId);		    	
		    testUtils->WriteComment(msgPass);
			}			
		else
			{
		    msgFail.Format(K_MSG_NOT_PRUNED,msgId);
		    testUtils->WriteComment(msgFail);	
		    break;	
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

	CleanupStack::PopAndDestroy(3,pruneMessage); // pruneMessage, testActive, popMessages

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_CImPruneMessage Test\n"));
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

