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
// Name of test harness: T_IMCM03_HEAP
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Heap Failure tests for following functions:
// - CImEmailMessage::AddAttachmentL()	
// - CImEmailMessage::AddMessageAsAttachmentL()
// - CImEmailMessage::AddRelatedPartL()	
// - CImEmailMessage::DeleteAttachmentL()
// - CImEmailMessage::DeleteAttachedMessageL()	
// - CImEmailMessage::StoreBodyTextL()
// Detailed description of test harness:
// As above
// Input files required to run test harness:
// \MSG\IMCMTSRC\rfc82201.txt					<DRIVE>:\msgtest\IMCM\rfc82201.txt
// \MSG\IMCMTSRC\rfc82202.txt					<DRIVE>:\msgtest\IMCM\rfc82202.txt
// \MSG\IMCMTSRC\rfc82203.txt					<DRIVE>:\msgtest\IMCM\rfc82203.txt
// \MSG\IMCMTSRC\rfc82204.txt					<DRIVE>:\msgtest\IMCM\rfc82204.txt
// \MSG\IMCMTSRC\rfc82205.txt					<DRIVE>:\msgtest\IMCM\rfc82205.txt
// \MSG\IMCMTSRC\rfc82206.txt					<DRIVE>:\msgtest\IMCM\rfc82206.txt
// \MSG\IMCMTSRC\Rfc82201.jig					<DRIVE>:\msgtest\IMCM\rfc822\Rfc82201.jig
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_IMCM03_HEAP\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMCM03_HEAP.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMCM03_HEAP.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMCM03_HEAP.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMCM03_HEAP.exe on the other platform
// 
//

#include "emailtestutils.h"

#include <miutlog.h>
#include <miutmsg.h>

#include <cntdb.h>
#include <cntfldst.h>
#include <cntitem.h>


#include <apfrec.h>
#include <apgicnfl.h>
#include <ecom/ecom.h>

#include <mmsvattachmentmanager.h>
#include <cemailaccounts.h>

// local variables etc //

#define KPeriod					10000	// period of timer

_LIT(KImcm03Test, "T_IMCM03_HEAP - CImEmailMessage Tests");
RTest test(KImcm03Test);

LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D CImLog* log;
LOCAL_D CEmailTestUtils* testUtils;

LOCAL_D TMsvId smtpService;
LOCAL_D TMsvId newMessageId;
LOCAL_D TMsvId newHtmlMessageId;
LOCAL_D TMsvId attachmentId;
LOCAL_D TMsvId attachmentMessageId;

LOCAL_D TInt outputProgressNo = 1;

//


LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->ClearEmailAccountsL();
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	testUtils->WriteComment(_L("CImEmailOperation Tests"));
	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("********* T_IMCM03_heap Test CImEmailOperation *********"));
	TBuf8<80> buf;

#if defined(__WINS__)	
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}
	
LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("**********    T_IMCM03_heap Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	User::After(5000000);
	}

LOCAL_C void OutputProgressL()
	{
	test.Console()->SetPos(26, 6);
	TInt tempCount = outputProgressNo%4;
	if (tempCount == 0)
		test.Printf(_L("|"));
	else if (tempCount == 1)
		test.Printf(_L("/"));
	else if (tempCount == 2)
		test.Printf(_L("-"));
	else if (tempCount == 3)
		test.Printf(_L("\\"));
	outputProgressNo++;
	}

LOCAL_C void CreateNewPlaintextMessageL()
	{
	/*
	// Create new Plaintext message
	*/
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	TMsvEmailTypeList msvEmailTypeList = 0;
	TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestMsvOperationTimer* testMsvOperationTimer = TestMsvOperationTimer::NewLC(test.Console(), emailOperation, test);
	testActive->StartL();
	testMsvOperationTimer->IssueRequest();
	CActiveScheduler::Start();

	//check progress
	TMsvId temp;
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = emailOperation->ProgressL();
	paramPack.Set(progBuf);
	newMessageId = paramPack();
	if (newMessageId != NULL)
		log->AppendComment(_L8("\t Created New plaintext message"));

	CleanupStack::PopAndDestroy(3); // testActive, emailOperation, testMsvOperationTimer
	OutputProgressL();
	}

LOCAL_C void CreateNewHtmlMessageL()
	{
	/*
	// Create new HTML message
	*/
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	TMsvEmailTypeList msvEmailTypeList = KMsvEmailTypeListMHTMLMessage;
	TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestMsvOperationTimer* testMsvOperationTimer = TestMsvOperationTimer::NewLC(test.Console(), emailOperation, test);
	testActive->StartL();
	testMsvOperationTimer->IssueRequest();
	CActiveScheduler::Start();

	//check progress
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = emailOperation->ProgressL();	
	paramPack.Set(progBuf);
	newHtmlMessageId=paramPack();	
	if (newHtmlMessageId!=NULL)
		log->AppendComment(_L8("\t Created New HTML message"));

	CleanupStack::PopAndDestroy(3); // testActive, emailOperation, testMsvOperationTimer
	OutputProgressL();
	}

LOCAL_C void TestStoreBodyTextL(TMsvId aMessageId)
	{
	CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer* charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);

	TBuf<100> bodyContents = _L("Test body contents so that we can test the store body text code.");
	bodyText->InsertL(0, bodyContents);

	// store the body text
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	TBool finished = EFalse;
	CTestActive* testActive;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, imEmailMessage->StoreBodyTextL(aMessageId, *bodyText, testActive->iStatus));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testActive->StartL();
			CActiveScheduler::Start();	// wait for the asynch operation to complete
			TMsvId temp;	
			TPckgC<TMsvId> paramPack(temp);
			const TDesC8& progBuf = imEmailMessage->ProgressL();	
			paramPack.Set(progBuf);
			test(paramPack() == aMessageId);
			finished = ETrue;
			}
		else
			test(error == KErrNoMemory);
		CleanupStack::PopAndDestroy(); //testActive
		}

	CleanupStack::PopAndDestroy(); // imEmailMessage
		
	CRichText* validRichText1=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(validRichText1);

	// restore the body text
	testActive=NULL;
	testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	imEmailMessage->GetBodyTextL(testActive->iStatus, aMessageId, CImEmailMessage::EThisMessageOnly, *validRichText1, *paraFormatLayer, *charFormatLayer);
	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete

	TBuf<100> bufa = bodyText->Read(0);
	TBuf<100> bufb = validRichText1->Read(0);
//
//  Remove comment once CImEmailMessage code has been fixed to get body text in Related folders
//
	test((bufa.Compare(bufb))==0);
	CleanupStack::PopAndDestroy(4); //imEmailMessage, testActive, validRichText1, bodyText
	
	// message now contains a richtext store so try and store rich text again
	bodyContents = _L("Some other stuff");
	bufb.Append(bodyContents);

	bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);
	bodyText->InsertL(0, bufb);

	// store the body text
	imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	finished = EFalse;
#if defined(_DEBUG)
	failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, imEmailMessage->StoreBodyTextL(aMessageId, *bodyText, testActive->iStatus));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testActive->StartL();
			CActiveScheduler::Start();	// wait for the asynch operation to complete
			TMsvId temp;	
			TPckgC<TMsvId> paramPack(temp);
			const TDesC8& progBuf = imEmailMessage->ProgressL();	
			paramPack.Set(progBuf);
			test(paramPack() == aMessageId);
			finished = ETrue;
			}
		else
			test(error == KErrNoMemory);
		CleanupStack::PopAndDestroy(); //testActive
		}

	CleanupStack::PopAndDestroy(); // imEmailMessage

	validRichText1=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(validRichText1);

	// restore the body text
	testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	imEmailMessage->GetBodyTextL(testActive->iStatus, aMessageId, CImEmailMessage::EThisMessageOnly, *validRichText1, *paraFormatLayer, *charFormatLayer);
	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete

	bufa = bodyText->Read(0);
	bufb = validRichText1->Read(0);
//
//  Remove comment once CImEmailMessage code has been fixed to get body text in Related folders
//
	test((bufa.Compare(bufb))==0);

	CleanupStack::PopAndDestroy(6); // validRichText1, imEmailMessage, testActive, bodyText, charFormatLayer, paraFormatLayer
	_LIT8(KComment, "\t Stored Body Text to message 00%x");
	TBuf8<80> buf;
	buf.Format(KComment, aMessageId);
	log->AppendComment(buf);
	OutputProgressL();
	}

LOCAL_C void TestAddAttachmentL(TMsvId aMessageId)
	{
	_LIT(KAttachment1, "C:\\mailtest\\IMCM\\rfc82201.txt");
	// add an attachment to the message
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	TBool finished = EFalse;
	CTestActive* testActive;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		testUtils->iMsvEntry->SetEntryL(aMessageId);
		CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
		CleanupStack::PushL(attachmentInfo);
		__UHEAP_FAILNEXT(failCount++);

		TRAPD(error,imEmailMessage->AttachmentManager().AddAttachmentL(KAttachment1, attachmentInfo, testActive->iStatus));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			CleanupStack::Pop(attachmentInfo);
			testActive->StartL();
			CActiveScheduler::Start();	// wait for the asynch operation to complete
			TMsvId temp;	
			TPckgC<TMsvId> paramPack(temp);
			const TDesC8& progBuf = imEmailMessage->ProgressL();	
			paramPack.Set(progBuf);
			attachmentId = paramPack();	
			test(attachmentId != KMsvNullIndexEntryId);
			finished = ETrue;
			}
		else
			{
			test(error == KErrNoMemory);
			CleanupStack::PopAndDestroy(attachmentInfo);
			}
		CleanupStack::PopAndDestroy(); //testActive
		}

	CleanupStack::PopAndDestroy(); // imEmailMessage
	_LIT8(KComment, "\t Attachment added with Id 00%x to message 00%x");
	TBuf8<80> buf;
	buf.Format(KComment, attachmentId, aMessageId);
	log->AppendComment(buf);
	OutputProgressL();
	} 

LOCAL_C void TestAddMessageAsAttachmentL(TMsvId aMessageId)
	{
	// add a message as an attachment to the message
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));

	TBool finished = EFalse;
	CTestActive* testActive;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		testUtils->iMsvEntry->SetEntryL(aMessageId);
		CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvMessageEntry);
		CleanupStack::PushL(attachmentInfo);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error,imEmailMessage->AttachmentManager().AddEntryAsAttachmentL(0x00100002, attachmentInfo, testActive->iStatus));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			CleanupStack::Pop(attachmentInfo);				
			testActive->StartL();
			CActiveScheduler::Start();	// wait for the asynch operation to complete
			TMsvId temp;	
			TPckgC<TMsvId> paramPack(temp);
			const TDesC8& progBuf = imEmailMessage->ProgressL();	
			paramPack.Set(progBuf);
			attachmentMessageId = paramPack();	
			test(attachmentMessageId != KMsvNullIndexEntryId);
			finished = ETrue;
			}
		else
			{
			test(error == KErrNoMemory);
			CleanupStack::PopAndDestroy(attachmentInfo);						
			}
		CleanupStack::PopAndDestroy(); //testActive
		}

	CleanupStack::PopAndDestroy(); // imEmailMessage
	_LIT8(KComment, "\t Message with Id 00%x added as attachment to message 00%x");
	TBuf8<80> buf;
	buf.Format(KComment, attachmentMessageId, aMessageId);
	log->AppendComment(buf);
	OutputProgressL();
	} 

LOCAL_C void TestRemoveAttachmentL(TMsvId aMessageId, TMsvId aAttachmentId)
	{
	// remove attachment from the message
	testUtils->iMsvEntry->SetEntryL(aMessageId);
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	
	TBool finished = EFalse;
	CTestActive* testActive;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		TMsvAttachmentId attachmentId = aAttachmentId;
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, imEmailMessage->AttachmentManager().RemoveAttachmentL(attachmentId, testActive->iStatus));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testActive->StartL();
			CActiveScheduler::Start();	// wait for the asynch operation to complete
			TMsvId temp;	
			TPckgC<TMsvId> paramPack(temp);
			const TDesC8& progBuf = imEmailMessage->ProgressL();	
			paramPack.Set(progBuf);
			test(paramPack() == aMessageId);	
			finished = ETrue;
			}
		else
			test(error == KErrNoMemory);
		CleanupStack::PopAndDestroy(); //testActive
		}

	CleanupStack::PopAndDestroy(); // imEmailMessage
	_LIT8(KComment, "\t Attachment with Id 00%x Removed from message 00%x");
	TBuf8<80> buf;
	buf.Format(KComment, aAttachmentId, aMessageId);
	log->AppendComment(buf);
	OutputProgressL();
	}

LOCAL_C void TestRemoveAttachedMessageL(TMsvId aMessageId, TMsvId aAttachedMessageId)
	{
	testUtils->iMsvEntry->SetEntryL(aMessageId);
	// remove attachment from the message
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	
	TBool finished = EFalse;
	CTestActive* testActive;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		TMsvAttachmentId attachmentId = aAttachedMessageId;
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, imEmailMessage->AttachmentManager().RemoveAttachmentL(attachmentId, testActive->iStatus));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testActive->StartL();
			CActiveScheduler::Start();	// wait for the asynch operation to complete
			TMsvId temp;	
			TPckgC<TMsvId> paramPack(temp);
			const TDesC8& progBuf = imEmailMessage->ProgressL();	
			paramPack.Set(progBuf);
			test(paramPack() == aMessageId);	
			finished = ETrue;
			}
		else
			test(error == KErrNoMemory);
		CleanupStack::PopAndDestroy(); //testActive
		}

	CleanupStack::PopAndDestroy(); // imEmailMessage
	_LIT8(KComment, "\t Attached Message with Id 00%x Removed from message 00%x");
	TBuf8<80> buf;
	buf.Format(KComment, aAttachedMessageId, aMessageId);
	log->AppendComment(buf);
	OutputProgressL();
	}

LOCAL_C void TestAddRelatedAttachmentL(TMsvId aMessageId, TMsvId aRelatedPartId)
	{
	testUtils->iMsvEntry->SetEntryL(aMessageId);
	_LIT(KAttachment1, "C:\\mailtest\\IMCM\\rfc82201.txt");
	_LIT8(KContentId, "abcdefgh.1234567@symbian.com");
	// add an attachment to the message
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));

	TBool finished = EFalse;
	CTestActive* testActive;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, imEmailMessage->AddRelatedPartL(aMessageId, KAttachment1, testActive->iStatus, aRelatedPartId, KContentId));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testActive->StartL();
			CActiveScheduler::Start();	// wait for the asynch operation to complete
			TMsvId temp;	
			TPckgC<TMsvId> paramPack(temp);
			const TDesC8& progBuf = imEmailMessage->ProgressL();	
			paramPack.Set(progBuf);
			attachmentId = paramPack();
			test(attachmentId != KMsvNullIndexEntryId);
			finished = ETrue;
			}
		else
			test(error == KErrNoMemory);
		CleanupStack::PopAndDestroy(); //testActive
		}

	CleanupStack::PopAndDestroy(); // imEmailMessage
	_LIT8(KComment, "\t Attachment added with Id 00%x to message 00%x");
	TBuf8<80> buf;
	buf.Format(KComment, attachmentId, aMessageId);
	log->AppendComment(buf);
	OutputProgressL();
	}

LOCAL_C void TestAddRelatedImageL(TMsvId aMessageId, TMsvId aRelatedPartId)
	{
	testUtils->iMsvEntry->SetEntryL(aMessageId);
	_LIT(KAttachment1, "C:\\mailtest\\rfc822\\Rfc82201.jig");
	_LIT8(KContentId, "abcdefgh.1234567@symbian.com");
	// add an attachment to the message
	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	
	TBool finished = EFalse;
	CTestActive* testActive;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, imEmailMessage->AddRelatedPartL(aMessageId, KAttachment1, testActive->iStatus, aRelatedPartId, KContentId));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testActive->StartL();
			CActiveScheduler::Start();	// wait for the asynch operation to complete
			TMsvId temp;	
			TPckgC<TMsvId> paramPack(temp);
			const TDesC8& progBuf = imEmailMessage->ProgressL();	
			paramPack.Set(progBuf);
			attachmentId = paramPack();
			test(attachmentId != KMsvNullIndexEntryId);
			finished = ETrue;
			}
		else
			test(error == KErrNoMemory);
		CleanupStack::PopAndDestroy(); //testActive
		}

	CleanupStack::PopAndDestroy(); // imEmailMessage
	_LIT8(KComment, "\t Attachment added with Id 00%x to message 00%x");
	TBuf8<80> buf;
	buf.Format(KComment, attachmentId, aMessageId);
	log->AppendComment(buf);
	OutputProgressL();
	}

LOCAL_C TMsvId GetRelatedPartIdL(TMsvId aMessageId)
	{
	testUtils->iMsvEntry->SetEntryL(aMessageId);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	imEmailMessage->FindFirstHTMLPageL(aMessageId, testActive->iStatus);
	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete

	TMsvId htmlEntryId;
	TBool found = EFalse;

	HBufC* temp = imEmailMessage->GetUniversalResourceIdentifierL(htmlEntryId, found);
	delete temp;

	test(found);
	CleanupStack::PopAndDestroy(2); // imEmailMessage, testActive
	OutputProgressL();
	return htmlEntryId;
	}

LOCAL_C void doMainL()
	{
	InitL();
	testUtils->FileSession().SetSessionPath(_L("c:\\"));

	testUtils->GoClientSideL();
	smtpService = testUtils->CreateSmtpServiceL();
	testUtils->CreatePopServiceL();

	test.Printf(_L("\nPerforming Tests:   "));
	

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(smtpService, smtpAccount);
	accounts->SetDefaultSmtpAccountL(smtpAccount);
	CleanupStack::PopAndDestroy(accounts);

	//
	// Test with Plaintext Messages
	//

	TInt testNo = 1;
	_LIT8(KIMCMTestPassed, "Test %d passed");
	TBuf8<80> buf;
	testUtils->TestStart(testNo);
	// test StoreBodyText when no text entry exists under message entry
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when text entry exists under message entry
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when text entry exists under message entry
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add attachment when there are no other entries
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already an entry under message entry - should create multipart/mixed
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already an entry under message entry - should create multipart/mixed
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestAddAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already a multipart/mixed folder
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestAddAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	//
	// Test with HTML Messages
	//
	// test StoreBodyText when no text entry exists under message entry
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add attachment when there are no other entries
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already an entry under message entry - should create multipart/mixed
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already an entry under message entry - should create multipart/mixed
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestAddAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	//
	// test adding of attachments which are related to HTML entries
	//

	// test StoreBodyText when no text entry exists under message entry
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	// find the id of the HTML part
	TMsvId relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add attachment when there are no other entries
	CreateNewHtmlMessageL();
	// find the id of the HTML part
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already an entry under message entry - should create multipart/mixed
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestStoreBodyTextL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already an entry under message entry - should create multipart/mixed
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestStoreBodyTextL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestStoreBodyTextL(newHtmlMessageId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestStoreBodyTextL(newHtmlMessageId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	//
	// Test with HTML Messages
	//

	// test StoreBodyText when there is already an entry under message entry - should create multipart/mixed
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	// find the id of the HTML part
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	// find the id of the HTML part
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId); //KEVAL - PROBLEM here!!
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already an entry under message entry - should create multipart/mixed
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	// find the id of the HTML part
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestAddAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	// find the id of the HTML part
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedImageL(newHtmlMessageId, relatedPartId);
	TestAddAttachmentL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	//
	// Test Add Message as Attachment functionality
	//

	//
	// Test with Plaintext Messages
	//

	// test add attachment when there are no other entries
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestRemoveAttachedMessageL(newMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already an entry under message entry - should create multipart/mixed
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestRemoveAttachedMessageL(newMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already an entry under message entry - should create multipart/mixed
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	TestRemoveAttachedMessageL(newMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already a multipart/mixed folder
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestStoreBodyTextL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	TestRemoveAttachedMessageL(newMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestRemoveAttachedMessageL(newMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	TestRemoveAttachedMessageL(newMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment and store body text
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment and store body text
	CreateNewPlaintextMessageL();
	TestAddAttachmentL(newMessageId);
	TestAddMessageAsAttachmentL(newMessageId);
	TestStoreBodyTextL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment
	CreateNewPlaintextMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	TestRemoveAttachmentL(newMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	//
	// Test with HTML Messages
	//
	
	// test add attachment when there are no other entries
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestRemoveAttachedMessageL(newHtmlMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already an entry under message entry - should create multipart/mixed
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	TestRemoveAttachedMessageL(newHtmlMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already an entry under message entry - should create multipart/mixed
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestRemoveAttachedMessageL(newHtmlMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test StoreBodyText when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestRemoveAttachedMessageL(newHtmlMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestRemoveAttachedMessageL(newHtmlMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test add Attachment when there is already a multipart/mixed folder
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	TestRemoveAttachedMessageL(newHtmlMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	//
	// test adding of attachments which are related to HTML entries
	//

	// test adding message as attachment
	CreateNewHtmlMessageL();
	// find the id of the HTML part
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestRemoveAttachedMessageL(newHtmlMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test adding message as attachment
	CreateNewHtmlMessageL();
	// find the id of the HTML part
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// test remove attachment
	CreateNewHtmlMessageL();
	relatedPartId = GetRelatedPartIdL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestAddRelatedAttachmentL(newHtmlMessageId, relatedPartId);
	TestRemoveAttachedMessageL(newHtmlMessageId, attachmentMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newMessageId);
	TestAddAttachmentL(newMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment and store body text
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment and store body text
	CreateNewHtmlMessageL();
	TestAddAttachmentL(newHtmlMessageId);
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestStoreBodyTextL(newHtmlMessageId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestStart(testNo);
	// Add message as attachment and normal attachment
	CreateNewHtmlMessageL();
	TestAddMessageAsAttachmentL(newHtmlMessageId);
	TestAddAttachmentL(newHtmlMessageId);
	TestRemoveAttachmentL(newHtmlMessageId, attachmentId);
	buf.Format(KIMCMTestPassed, testNo++);
	log->AppendComment(buf);
	testUtils->TestFinish(testNo-1);

	testUtils->TestHarnessCompleted();

	Closedown();
	
	// shouldn't have to do this, but convertor plugins aren't 
	// doing it yet so we get a false memory leak so remove this
	// when they fix that.
	REComSession::FinalClose();
	}

GLDEF_C TInt E32Main()
	{	
	FbsStartup();
	TInt ret=RFbsSession::Connect();
		test(!ret);


	__UHEAP_MARK;
	test.Start(_L("T_IMCM03_HEAP Test CImEmailMessage class\n"));
	theCleanup=CTrapCleanup::New();
	TRAP(ret,doMainL());
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
