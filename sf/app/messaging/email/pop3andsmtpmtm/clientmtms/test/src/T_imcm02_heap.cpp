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
// Name of test harness: T_IMCM02_HEAP
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Heap Failure tests for following functions:
// - CImEmailOperation::CreateNewL()			
// - CImEmailOperation::CreateReplyL()
// - CImEmailOperation::CreateForwardL()			
// - CImEmailOperation::CreateForwardAsAttachmentL()
// - CImEmailOperation::CreateReceiptL()
// Detailed description of test harness:
// As above
// Input files required to run test harness:
// \MSG\IMCMTSRC\contacts.cdb					<DRIVE>:\system\data\contacts.cdb
// \MSG\IMCMTSRC\rfc82201.txt					<DRIVE>:\msgtest\IMCM\rfc82201.txt
// \MSG\IMCMTSRC\rfc82202.txt					<DRIVE>:\msgtest\IMCM\rfc82202.txt
// \MSG\IMCMTSRC\rfc82203.txt					<DRIVE>:\msgtest\IMCM\rfc82203.txt
// \MSG\IMCMTSRC\rfc82204.txt					<DRIVE>:\msgtest\IMCM\rfc82204.txt
// \MSG\IMCMTSRC\rfc82205.txt					<DRIVE>:\msgtest\IMCM\rfc82205.txt
// \MSG\IMCMTSRC\rfc82206.txt					<DRIVE>:\msgtest\IMCM\rfc82206.txt
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_IMCM02_HEAP\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMCM02_HEAP.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMCM02_HEAP.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMCM02_HEAP.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMCM02_HEAP.exe on the other platform
// 
//

// test harness for testing heap failure of CImEmailOperation
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

_LIT(KImcm02Test, "T_IMCM02_HEAP - CImEmailOperation Tests");
_LIT(KMsvServerPattern, "!MsvServer*");
RTest test(KImcm02Test);

LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D CImLog* log;

LOCAL_D TMsvId pop3Service;
LOCAL_D TMsvId smtpService;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CMsvEntrySelection* messageSelection;
LOCAL_D TInt testNo = 1;

class TestUiTimer : public CTimer
	{
public:
	static TestUiTimer* NewLC(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation);

	void RunL();
	void DoCancel();
	void ConstructL();
	void IssueRequest();
	
	CConsoleBase*				iConsole;
	CImEmailOperation*			iEmailOperation;
	TTimeIntervalMicroSeconds32 period;
protected:
	TInt iCount;
	TBool iClosing;
	TestUiTimer(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation);
	};

//
// Progress timer
//	  
TestUiTimer* TestUiTimer::NewLC(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation)
	{
	TestUiTimer* self = new(ELeave) TestUiTimer(aConsole, aEmailOperation);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TestUiTimer::TestUiTimer(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation)
	: CTimer(EPriorityStandard+3), iEmailOperation(aEmailOperation)
	{
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
/*	// display the current progress
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);

	const TDesC8& progBuf = iEmailOperation->ProgressL();	
	paramPack.Set(progBuf);
	TMsvId progress=paramPack();	

	test.Console()->SetPos(0, 10);
	test.Printf(_L("   MessageId %d		\n"),progress);
*/
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
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	testUtils->WriteComment(_L("CImEmailOperation Tests"));
	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("********* T_IMCM02_heap Test CImEmailOperation *********"));
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
	log->AppendComment(_L8("**********    T_IMCM02_heap Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	User::After(5000000);
	}

LOCAL_C void TestMessageProgressL(CImEmailOperation& aEmailOperation, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	test.Console()->SetPos(26, 7);
	TInt tempCount = aTestNo%4;
	if (tempCount == 0)
		test.Printf(_L("|"));
	else if (tempCount == 1)
		test.Printf(_L("/"));
	else if (tempCount == 2)
		test.Printf(_L("-"));
	else if (tempCount == 3)
		test.Printf(_L("\\"));
	//check progress
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = aEmailOperation.ProgressL();
	paramPack.Set(progBuf);
	TMsvId messageId=paramPack();
	TInt error = aEmailOperation.iStatus.Int();

	_LIT8(KPassed, "\t\tPASSED Test: %d");
	_LIT8(KFailed, "\t\tFAILED Test: %d - Tried to Create message:");

	TBuf8<80> buf;
	if (messageId!=NULL)
		{
		buf.Format(KPassed, aTestNo, messageId);
		log->AppendComment(buf);
		testUtils->TestFinish(aTestNo);
		}
	else
		{
		buf.Format(KFailed, aTestNo, messageId);
		log->AppendError(buf, error);
		if (aMsvEmailTypeList & KMsvEmailTypeListMHTMLMessage)
			log->AppendError(_L8("\t\t\tHTML message"), error);
		if (aPartList & KMsvMessagePartBody)
			log->AppendError(_L8("\t\t\t- With Body"), error);
		if (aPartList & KMsvMessagePartAttachments)
			log->AppendError(_L8("\t\t\t- With Attachments"), error);
		if (aPartList & KMsvMessagePartDescription)
			log->AppendError(_L8("\t\t\t- With Subject"), error);
		if (aPartList & KMsvMessagePartOriginator)
			log->AppendError(_L8("\t\t\t- With Originator"), error);
		if (aPartList & KMsvMessagePartRecipient)
			log->AppendError(_L8("\t\t\t- With Recipient"), error);
		}
	}

LOCAL_C void CreateNewWithHeapFailureL(TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	TBool finished = EFalse;
	CImEmailOperation* emailOperation = NULL;
	CTestActive* testActive;
	TestUiTimer* testUiTimer;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
			testActive->StartL();
			testUiTimer->IssueRequest();
			CActiveScheduler::Start();
			TestMessageProgressL(*emailOperation, aPartList, aMsvEmailTypeList, aTestNo);
			delete emailOperation;
			emailOperation = NULL;
			CleanupStack::PopAndDestroy(2); // testActive, testUiTimer
			finished = ETrue;
			}
		else
			{
			test(error == KErrNoMemory);
			CleanupStack::PopAndDestroy(); // testActive
			}
		}
	}

LOCAL_C void CreateReplyWithHeapFailureL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	TBool finished = EFalse;
	CImEmailOperation* emailOperation = NULL;
	CTestActive* testActive;
	TestUiTimer* testUiTimer;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, emailOperation = CImEmailOperation::CreateReplyL(testActive->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
			testActive->StartL();
			testUiTimer->IssueRequest();
			CActiveScheduler::Start();
			TestMessageProgressL(*emailOperation, aPartList, aMsvEmailTypeList, aTestNo);
			delete emailOperation;
			emailOperation = NULL;
			CleanupStack::PopAndDestroy(2); // testActive, testUiTimer
			finished = ETrue;
			}
		else
			{
			test(error == KErrNoMemory);
			CleanupStack::PopAndDestroy(); // testActive
			}
		}
	}

LOCAL_C void CreateForwardWithHeapFailureL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	TBool finished = EFalse;
	CImEmailOperation* emailOperation = NULL;
	CTestActive* testActive;
	TestUiTimer* testUiTimer;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, emailOperation = CImEmailOperation::CreateForwardL(testActive->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
			testActive->StartL();
			testUiTimer->IssueRequest();
			CActiveScheduler::Start();
			TestMessageProgressL(*emailOperation, aPartList, aMsvEmailTypeList, aTestNo);
			delete emailOperation;
			emailOperation = NULL;
			CleanupStack::PopAndDestroy(2); // testActive, testUiTimer
			finished = ETrue;
			}
		else
			{
			test(error == KErrNoMemory);
			CleanupStack::PopAndDestroy(); // testActive
			}
		}
	}

LOCAL_C void CreateForwardAsAttachmentWithHeapFailureL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	TBool finished = EFalse;
	CImEmailOperation* emailOperation = NULL;
	CTestActive* testActive;
	TestUiTimer* testUiTimer;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, emailOperation = CImEmailOperation::CreateForwardAsAttachmentL(testActive->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
			testActive->StartL();
			testUiTimer->IssueRequest();
			CActiveScheduler::Start();
			TestMessageProgressL(*emailOperation, aPartList, aMsvEmailTypeList, aTestNo);
			delete emailOperation;
			emailOperation = NULL;
			CleanupStack::PopAndDestroy(2); // testActive, testUiTimer
			finished = ETrue;
			}
		else
			{
			test(error == KErrNoMemory);
			CleanupStack::PopAndDestroy(); // testActive
			}
		}
	}

LOCAL_C void CreateReceiptWithHeapFailureL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	TBool finished = EFalse;
	CImEmailOperation* emailOperation = NULL;
	CTestActive* testActive;
	TestUiTimer* testUiTimer;
#if defined(_DEBUG)
	TInt failCount = 0;
#endif
	while (!finished)
		{
		testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);
		__UHEAP_FAILNEXT(failCount++);
		TRAPD(error, emailOperation = CImEmailOperation::CreateReceiptL(testActive->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP));
		__UHEAP_RESET;
		if (error==KErrNone)
			{
			testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
			testActive->StartL();
			testUiTimer->IssueRequest();
			CActiveScheduler::Start();
			TestMessageProgressL(*emailOperation, aPartList, aMsvEmailTypeList, aTestNo);
			delete emailOperation;
			emailOperation = NULL;
			CleanupStack::PopAndDestroy(2); // testActive, testUiTimer
			finished = ETrue;
			}
		else
			{
			test(error == KErrNoMemory);
			CleanupStack::PopAndDestroy(); // testActive
			}
		}
	}

/*
Uncomment this function if the Contacts database has been changed and the test harness fails for this reason!
LOCAL_C void CreateVCardL()
	{
	// create own card file.
	CContactDatabase* db = CContactDatabase::ReplaceL();
	CleanupStack::PushL(db);

	CContactItemField* field=CContactItemField::NewLC(KStorageTypeText,KUidContactFieldFamilyName);
	field->TextStorage()->SetTextL(_L("Symbian"));
	CContactItem* item = CContactCard::NewLC();
	item->AddFieldL(*field);
	TContactItemId id;
	id=db->AddNewContactL(*item);
	db->SetOwnCardL(*item);
	CleanupStack::PopAndDestroy(); // item
	CleanupStack::Pop(); // field
	CleanupStack::PopAndDestroy(); // db
	}
*/

LOCAL_C void SetAddVCardToEmailL(TBool addVCard, TMsvId serviceId)
	{
	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(serviceId, smtpAccount);
	accounts->LoadSmtpSettingsL(smtpAccount, *smtpSettings);

	smtpSettings->SetAddVCardToEmail(addVCard);
	
	accounts->SaveSmtpSettingsL(smtpAccount, *smtpSettings);

	CleanupStack::PopAndDestroy(2); // smtpSettings, msvStore/accounts
	}

LOCAL_C void CreateSignatureL(TMsvId serviceId)
	{
	testUtils->iMsvEntry->SetEntryL(serviceId);
	CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer* charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* signature=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(signature);

	TBuf<100> bodyContents = _L("(Signature:) Symbian Ltd.");
	signature->InsertL(0, bodyContents);

	CMsvStore* msvStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(msvStore);
	
	msvStore->StoreBodyTextL(*signature);
	msvStore->Commit();
	CleanupStack::PopAndDestroy(4); // msvStore, signature, charFormatLayer, paraFormatLayer
	}

LOCAL_C void SetSignatureToEmailL(TBool addSignature, TMsvId serviceId)
	{
	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(serviceId, smtpAccount);
	accounts->LoadSmtpSettingsL(smtpAccount, *smtpSettings);

	smtpSettings->SetAddSignatureToEmail(addSignature);
	
	accounts->SaveSmtpSettingsL(smtpAccount, *smtpSettings);

	CleanupStack::PopAndDestroy(2); // smtpSettings, msvStore/accounts

	if (addSignature)
		testUtils->WriteComment(_L("\tAdd VCard"));
	}

LOCAL_C void SetDefaultServiceL(TMsvId smtpService)
	{
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(smtpService, smtpAccount);
	accounts->SetDefaultSmtpAccountL(smtpAccount);
	CleanupStack::PopAndDestroy(accounts);
	}

LOCAL_C void WaitForServerClose()
	{
	//Kill the message server if its running
	_LIT(KMsvServerPattern, "!MsvServer*");
	TFindProcess findprocess(KMsvServerPattern);
	TFullName name;
	if(findprocess.Next(name)==KErrNone)
		{
		RProcess process;
    	User::LeaveIfError(process.Open(findprocess));
     	process.Kill(KErrCancel) ;
		process.Close() ;
		}
		
	FOREVER
		{
		TFullName name;
		TFindProcess find(KMsvServerPattern);
		if (find.Next(name) != KErrNone)
			break;
		User::After(100000);
		}
	}

LOCAL_C void DeleteMessageFolderAndCreateMessagesL()
	{
	test.Console()->ClearScreen();
	testUtils->iMsvSession->CloseMessageServer();
	testUtils->Reset();

	CTestTimer* timer = CTestTimer::NewL();
	timer->After(1000000); //1 second delay
	CActiveScheduler::Start();
	delete timer;

	WaitForServerClose();

	CleanupStack::PopAndDestroy(3); //testUtils, scheduler, log

	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoClientSideL();

	pop3Service = testUtils->CreatePopServiceL();
	smtpService = testUtils->CreateSmtpServiceL();
	
	CleanupStack::PopAndDestroy(testUtils);
	testUtils = CEmailTestUtils::NewLC(test, ETuGoServerSide);

	testUtils->CreateMessageFilesL(pop3Service, KMsvGlobalInBoxIndexEntryId, _L("c:\\mailtest\\IMCM\\"));

	testUtils->WriteComment(_L("CImEmailOperation Tests"));
	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);

	testUtils->GoClientSideL();
	SetDefaultServiceL(smtpService);

	// get a list of all email messages in the Inbox
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	delete messageSelection;
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);

	test.Console()->ClearScreen();
	}

LOCAL_C TMsvId CreateNewPlaintextMessageWithIncompleteAttachmentL()
	{
	TMsvEmailTypeList msvEmailTypeList = 0;
	TMsvPartList partList = 0;

	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalInBoxIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestUiTimer* testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
	testActive->StartL();
	testUiTimer->IssueRequest();
	CActiveScheduler::Start();

	//check progress
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf1 = emailOperation->ProgressL();	
	paramPack.Set(progBuf1);
	TMsvId messageId = paramPack();	

	CleanupStack::PopAndDestroy(3); // testActive, emailOperation, testUiTimer

	//
	// Add an attachment
	//
	testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	_LIT(KAttachment1, "C:\\mailtest\\IMCM\\rfc82201.txt");
	_LIT(KAttachment2, "C:\\mailtest\\IMCM\\rfc82202.txt");

	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));
	testUtils->iMsvEntry->SetEntryL(messageId);
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);
	imEmailMessage->AttachmentManager().AddAttachmentL(KAttachment1, attachmentInfo, testActive->iStatus);
	CleanupStack::Pop(attachmentInfo);	
	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete

	testUtils->iMsvEntry->SetEntryL(messageId);
	attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);
	imEmailMessage->AttachmentManager().AddAttachmentL(KAttachment2, attachmentInfo, testActive->iStatus);
	CleanupStack::Pop(attachmentInfo);	

	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete

	const TDesC8& progBuf2 = imEmailMessage->ProgressL();	
	paramPack.Set(progBuf2);
	TMsvId attachmentId = paramPack();	

	CleanupStack::PopAndDestroy(2); // imEmailMessage, testActive

	//
	// Set the attachment to be incomplete
	//
	
	testUtils->iMsvEntry->SetEntryL(attachmentId);
	TMsvEmailEntry attachmentEntry = testUtils->iMsvEntry->Entry();
	attachmentEntry.SetComplete(EFalse);
	testUtils->iMsvEntry->ChangeL(attachmentEntry);

	return messageId;
	}

LOCAL_C void doMainL()
	{
	InitL();

	testUtils->GoClientSideL();
	testUtils->ClearEmailAccountsL();
	pop3Service = testUtils->CreatePopServiceL();
	smtpService = testUtils->CreateSmtpServiceL();
		
	CleanupStack::Pop(log);
	CleanupStack::PopAndDestroy(testUtils);
	testUtils = CEmailTestUtils::NewLC(test, ETuGoServerSide);
	CleanupStack::PushL(log);

	testUtils->CreateMessageFilesL(pop3Service, KMsvGlobalInBoxIndexEntryId, _L("c:\\mailtest\\IMCM\\"));

	test.Console()->SetPos(0, 6);
	test.Printf(_L("Performing Tests:    "));
	
	testUtils->GoClientSideL();
	SetDefaultServiceL(smtpService);

	// get a list of all email messages in the Inbox
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	delete messageSelection;
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);

	test.Console()->SetPos(3, 7);
	testUtils->WriteComment(_L("Heap Failure tests:"));

//	CreateVCardL();
	SetAddVCardToEmailL(ETrue, smtpService);
	CreateSignatureL(smtpService);
	SetSignatureToEmailL(ETrue, smtpService);

//	
//	Create New tests
//

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create New Message with VCard and Signature Tests"));
	TMsvPartList partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	TMsvEmailTypeList emailTypeList = 0;
	CreateNewWithHeapFailureL(partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create New Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateNewWithHeapFailureL(partList, emailTypeList, testNo++);

//	
//	Create Reply tests
//

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Reply Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateReplyWithHeapFailureL(messageSelection->At(5), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Reply Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateReplyWithHeapFailureL(messageSelection->At(5), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Reply Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateReplyWithHeapFailureL(messageSelection->At(1), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Reply Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateReplyWithHeapFailureL(messageSelection->At(1), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Reply Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateReplyWithHeapFailureL(messageSelection->At(3), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Reply Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateReplyWithHeapFailureL(messageSelection->At(3), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	TMsvId messageWithIncompleteAttachment = CreateNewPlaintextMessageWithIncompleteAttachmentL();
	log->AppendComment(_L8("\tPlaintext Create Reply Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateReplyWithHeapFailureL(messageWithIncompleteAttachment, partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Reply Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateReplyWithHeapFailureL(messageWithIncompleteAttachment, partList, emailTypeList, testNo++);

//	
//	Create Forward tests
//

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Forward Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateForwardWithHeapFailureL(messageSelection->At(5), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Forward Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateForwardWithHeapFailureL(messageSelection->At(5), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Forward Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateForwardWithHeapFailureL(messageSelection->At(1), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Forward Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateForwardWithHeapFailureL(messageSelection->At(1), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Forward Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateForwardWithHeapFailureL(messageSelection->At(3), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Forward Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateForwardWithHeapFailureL(messageSelection->At(3), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Forward Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateForwardWithHeapFailureL(messageWithIncompleteAttachment, partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Forward Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateForwardWithHeapFailureL(messageWithIncompleteAttachment, partList, emailTypeList, testNo++);

//
//  Delete message folder as this is causing problems due to __ASSERT_DEBUGs in MsvServer
//  This can be removed once this has been fixed in the server!!
//
	DeleteMessageFolderAndCreateMessagesL();

//	
//	Create ForwardAsAttachment tests
//

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create ForwardAsAttachment Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateForwardAsAttachmentWithHeapFailureL(messageSelection->At(5), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create ForwardAsAttachment Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateForwardAsAttachmentWithHeapFailureL(messageSelection->At(5), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create ForwardAsAttachment Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateForwardAsAttachmentWithHeapFailureL(messageSelection->At(1), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create ForwardAsAttachment Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateForwardAsAttachmentWithHeapFailureL(messageSelection->At(1), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create ForwardAsAttachment Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateForwardAsAttachmentWithHeapFailureL(messageSelection->At(3), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create ForwardAsAttachment Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateForwardAsAttachmentWithHeapFailureL(messageSelection->At(3), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create ForwardAsAttachment Message with VCard and Signature Tests"));
	messageWithIncompleteAttachment = CreateNewPlaintextMessageWithIncompleteAttachmentL();
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateForwardAsAttachmentWithHeapFailureL(messageWithIncompleteAttachment, partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create ForwardAsAttachment Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateForwardAsAttachmentWithHeapFailureL(messageWithIncompleteAttachment, partList, emailTypeList, testNo++);

//	
//	Create Receipt tests
//

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Receipt Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateReceiptWithHeapFailureL(messageSelection->At(5), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Receipt Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateReceiptWithHeapFailureL(messageSelection->At(5), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Receipt Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateReceiptWithHeapFailureL(messageSelection->At(1), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Receipt Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateReceiptWithHeapFailureL(messageSelection->At(1), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Receipt Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateReceiptWithHeapFailureL(messageSelection->At(3), partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Receipt Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateReceiptWithHeapFailureL(messageSelection->At(3), partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	log->AppendComment(_L8("\tPlaintext Create Receipt Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = 0;
	CreateReceiptWithHeapFailureL(messageWithIncompleteAttachment, partList, emailTypeList, testNo++);

	log->AppendComment(_L8("\tHTML Create Receipt Message with VCard and Signature Tests"));
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateReceiptWithHeapFailureL(messageWithIncompleteAttachment, partList, emailTypeList, testNo++);

	testUtils->TestHarnessCompleted();
	delete messageSelection;
	testUtils->Reset();
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
	test.Start(_L("T_IMCM02 Test CImEmailOperation class\n"));
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
