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
// Name of test harness: T_PERFORMANCE
// Component: IMCM
// Brief description of test harness:
// Tests the performance of CImEmailOperation - reply/forward/new to plaintext message including/excluding 
// attachments; reply/forward/new to plaintext message as HTML including/excluding 
// attachments; reply/forward/new to HTML message including/excluding attachments; 
// Also tests adding VCard and signature from the SMTP settings.
// Detailed description of test harness:
// As above
// Input files required to run test harness:
// \MSG\IMCMTSRC\contacts.cdb					<DRIVE>:\system\data\contacts.cdb
// \MSG\IMCMTSRC\T_PERFORMANCE_Entry_Structure.txt	<DRIVE>:\msgtest\IMCM\T_PERFORMANCE_Entry_Structure.txt
// \MSG\IMCMTSRC\rfc82201.txt					<DRIVE>:\msgtest\IMCM\rfc82201.txt
// \MSG\IMCMTSRC\rfc82202.txt					<DRIVE>:\msgtest\IMCM\rfc82202.txt
// \MSG\IMCMTSRC\rfc82203.txt					<DRIVE>:\msgtest\IMCM\rfc82203.txt
// \MSG\IMCMTSRC\rfc82204.txt					<DRIVE>:\msgtest\IMCM\rfc82204.txt
// \MSG\IMCMTSRC\rfc82205.txt					<DRIVE>:\msgtest\IMCM\rfc82205.txt
// \MSG\IMCMTSRC\rfc82206.txt					<DRIVE>:\msgtest\IMCM\rfc82206.txt
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_PERFORMANCE\Entry_Structure.txt
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_PERFORMANCE.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_PERFORMANCE.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_PERFORMANCE.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_PERFORMANCE.exe on the other platform
// 
//

#include "emailtestutils.h"
#include <miutmsg.h>
#include <e32base.h>
#include <cemailaccounts.h>
#include <ecom/ecom.h>

#include <cntdb.h>

// local variables etc //

_LIT(KImcm02Test, "T_PERFORMANCE - CImEmailOperation Performance Tests");
RTest test(KImcm02Test);

LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D TMsvId pop3Service;
LOCAL_D TMsvId smtpService;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CMsvEntrySelection* messageSelection;
LOCAL_D TInt testNo = 0;
LOCAL_D TMsvPartList partList = 0;
LOCAL_D TMsvEmailTypeList emailTypeList = 0;

#define KComponent				_L("IMCM")
#define KTestMessageDir			_L("")
const TInt KStandardPriority = 10;
const TMsvId KInbox	= 4096;
_LIT(KCountMessage,"Count is %d");

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	CContactDatabase* contacts(NULL);
	TRAPD(err, contacts = CContactDatabase::OpenL());
	delete contacts;
	if (err==KErrNotFound)
		{
		testUtils->WriteComment(_L("contacts file is missing from system data"));		
		return;
		}
	testUtils->CleanMessageFolderL();
	testUtils->GoClientSideL();
	testUtils->ClearEmailAccountsL();
	smtpService = testUtils->CreateSmtpServiceL();
	pop3Service = testUtils->CreatePopServiceL();
	testUtils->GoServerSideL();
	
	test.Next(_L("Create Data Component FileStores"));
	testUtils->CreateSmtpServerMtmRegL();
	testUtils->CreatePopServerMtmRegL();

	//	Loading the DLLs
	testUtils->CreateRegistryObjectAndControlL();
	testUtils->InstallSmtpMtmGroupL();
	testUtils->InstallPopMtmGroupL();
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler

	// 11/10/04 MLM added to clean up CCnaConverterList's use of REComSession		
	REComSession::FinalClose();

	User::After(5000000);
	}


LOCAL_C void SetAddVCardToEmailL(TBool addVCard, TMsvId serviceId)
	{
	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);
	
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL( serviceId, smtpAccountId);
	accounts->LoadSmtpSettingsL(smtpAccountId, *smtpSettings);
	smtpSettings->SetAddVCardToEmail(addVCard);
	accounts->SaveSmtpSettingsL(smtpAccountId, *smtpSettings);
	CleanupStack::PopAndDestroy(2); // msvStore, smtpSettings
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

	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL( serviceId, smtpAccountId);
	accounts->LoadSmtpSettingsL(smtpAccountId, *smtpSettings);
	smtpSettings->SetAddSignatureToEmail(addSignature);
	accounts->SaveSmtpSettingsL(smtpAccountId, *smtpSettings);

	CleanupStack::PopAndDestroy(2); // smtpSettings, msvStore/accounts
	if (addSignature)
		testUtils->WriteComment(_L("\tAdd VCard"));
	}


LOCAL_C void SetDefaultServiceL(TMsvId smtpService)
	{
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL(smtpService, smtpAccountId);
	accounts->SetDefaultSmtpAccountL(smtpAccountId);

	TSmtpAccount defaultSmtpAccountId;
	accounts->DefaultSmtpAccountL(defaultSmtpAccountId);
	if (smtpService != defaultSmtpAccountId.iSmtpService)
		User::Leave(KErrNotFound);
	CleanupStack::PopAndDestroy(accounts);
	}

LOCAL_C void TestMessageProgressL(CImEmailOperation& aEmailOperation)
	{
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = aEmailOperation.ProgressL();
	paramPack.Set(progBuf);
	TMsvId messageId=paramPack();
	TInt error = aEmailOperation.iStatus.Int();
	if (error || messageId==NULL)
		testUtils->TestFinish(testNo,-1);
	else if (!error && messageId)
		testUtils->TestFinish(testNo);
	else
		testUtils->TestFinish(testNo,-19);
	}

LOCAL_C void CreateNewL(TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateNewL started"));
	
	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation=CImEmailOperation::CreateNewL(waiter->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, smtpService, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP, KStandardPriority);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateNewL finished"));
	TestMessageProgressL(*emailOperation);
	
	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreatNewMessageL()
	{
	emailTypeList=0;
	testUtils->WriteComment(_L("Create New Plaintext message without attachment"));
	CreateNewL(partList, emailTypeList);

	testUtils->WriteComment(_L("Create New to Plaintext message with attachment"));
	CreateNewL(partList, emailTypeList);
	
	emailTypeList=KMsvEmailTypeListMHTMLMessage;

	testUtils->WriteComment(_L("Create New HTML message without attachment"));
	CreateNewL(partList, emailTypeList);

	testUtils->WriteComment(_L("Create New HTML message with attachment"));
	CreateNewL(partList, emailTypeList);
	}

LOCAL_C void CreateReplyL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateReply started"));

	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation = CImEmailOperation::CreateReplyL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP,KStandardPriority);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateReply finished"));
	TestMessageProgressL(*emailOperation);

	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreateReplyWithFormattedSubjectL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateReply started"));

	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation = CImEmailOperation::CreateReplyL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, _L("Reply: "), aMsvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateReply finished"));
	TestMessageProgressL(*emailOperation);

	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreateReplyWithPlainOrRichTextL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList,TBool aMessageTextType)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateReply started"));

	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation = CImEmailOperation::CreateReplyL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP, KStandardPriority, aMessageTextType);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateReply finished"));
	TestMessageProgressL(*emailOperation);

	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreateReceiptL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateReply started"));

	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation = CImEmailOperation::CreateReceiptL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP,KStandardPriority);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateReply finished"));
	TestMessageProgressL(*emailOperation);
	//Verification
	{
	CMsvEntry* entry = CMsvEntry::NewL(*(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, TMsvSelectionOrdering(KMsvNoGrouping, EMsvSortByNone, ETrue));
	CleanupStack::PushL(entry);
	
	entry->SetEntryL(KInbox);
	CMsvEntrySelection* selection = entry->ChildrenL();
	CleanupStack::PushL(selection);
	TInt count = selection->Count();
	testUtils->Printf(_L("Count value is %d"), count);
	for(TInt i = 0; i<count;i++)
		{
		entry->SetEntryL((*selection)[i]);
		TMsvEntry checkEntry = entry->Entry();
		if(checkEntry.Id() == KMsvGlobalOutBoxIndexEntryId)
			{
			selection = entry->ChildrenL();
			TInt outBoxChildrenCount = selection->Count();
			}
		}
	CleanupStack::PopAndDestroy(selection);
	CleanupStack::PopAndDestroy(entry);
	}

	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreateReceiptWithFormattedSubjectL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateReply started"));

	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation = CImEmailOperation::CreateReceiptL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, _L("Receipt of message: "), aMsvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateReply finished"));
	TestMessageProgressL(*emailOperation);

	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreateReceiptPlainOrRichTextL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList,TBool aMessageTextType)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateReply started"));

	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation = CImEmailOperation::CreateReceiptL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList,aMsvEmailTypeList, KUidMsgTypeSMTP, KStandardPriority, aMessageTextType);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateReply finished"));
	TestMessageProgressL(*emailOperation);

	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreatReplyMessageL()
	{
	emailTypeList=0;
	testUtils->WriteComment(_L("Create Reply to Plaintext message without attachment"));
	CreateReplyL(messageSelection->At(5), partList, emailTypeList);

	testUtils->WriteComment(_L("Create Reply to Plaintext message with attachment"));
	CreateReplyL(messageSelection->At(1), partList, emailTypeList);
	
	emailTypeList=KMsvEmailTypeListMHTMLMessage;

	testUtils->WriteComment(_L("Create Reply to HTML message without attachment"));
	CreateReplyL(messageSelection->At(5), partList, emailTypeList);

	testUtils->WriteComment(_L("Create Reply to HTML message with attachment"));
	CreateReplyL(messageSelection->At(1), partList, emailTypeList);

	testUtils->WriteComment(_L("Create Reply with formatted Subject"));
	CreateReplyWithFormattedSubjectL(messageSelection->At(1), partList, emailTypeList);

	testUtils->WriteComment(_L("Create Reply to HTML message as RichText"));
	CreateReplyWithPlainOrRichTextL(messageSelection->At(1), partList, emailTypeList,EFalse);

	testUtils->WriteComment(_L("Create Reply to HTML message as PlainText"));
	CreateReplyWithPlainOrRichTextL(messageSelection->At(1), partList, emailTypeList,ETrue);
	}

LOCAL_C void CreateReceiptMessageL()
	{
	emailTypeList=0;
	testUtils->WriteComment(_L("Create Receipt to Plaintext message without attachment"));
	CreateReceiptL(messageSelection->At(5), partList, emailTypeList);

	emailTypeList=KMsvEmailTypeListMHTMLMessage;

	testUtils->WriteComment(_L("Create Receipt to HTML message with attachment"));
	CreateReceiptL(messageSelection->At(1), partList, emailTypeList);

	testUtils->WriteComment(_L("Create HTML message  as Receipt with formatted subject"));
	CreateReceiptWithFormattedSubjectL(messageSelection->At(1), partList, emailTypeList);

	testUtils->WriteComment(_L("Create HTML message as Receipt and store as PlainText"));
	CreateReceiptPlainOrRichTextL(messageSelection->At(1), partList, emailTypeList, EFalse);

	testUtils->WriteComment(_L("Create HTML message as Receipt and store as RichText"));
	CreateReceiptPlainOrRichTextL(messageSelection->At(1), partList, emailTypeList, ETrue);
	}

LOCAL_C void CreateForwardL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateForward started"));
	
	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation = CImEmailOperation::CreateForwardL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId,KMsvGlobalOutBoxIndexEntryId, smtpService, aPartList, aMsvEmailTypeList,KUidMsgTypeSMTP, KStandardPriority);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateForward finished"));
	TestMessageProgressL(*emailOperation);
	
	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreateForwardWithSubjectL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateForward started"));
	
	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
																	
	CImEmailOperation* emailOperation = CImEmailOperation::CreateForwardL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId,KMsvGlobalOutBoxIndexEntryId, aPartList,_L("Addtional Subject"), aMsvEmailTypeList,KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateForward finished"));
	TestMessageProgressL(*emailOperation);
	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

LOCAL_C void CreateForwardWithPlainOrRichTextL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TBool aTextType)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateForward started"));
	
	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
																	
	CImEmailOperation* emailOperation = CImEmailOperation::CreateForwardL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId,KMsvGlobalOutBoxIndexEntryId, smtpService, aPartList, aMsvEmailTypeList,KUidMsgTypeSMTP, KStandardPriority,aTextType);
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateForward finished"));
	TestMessageProgressL(*emailOperation);
	
	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}
	

LOCAL_C void CreatForwardMessageL()
	{
	emailTypeList=0;

	testUtils->WriteComment(_L("Create Forward message of Plaintext without attachment"));
	CreateForwardL(messageSelection->At(5), partList, emailTypeList);

	testUtils->WriteComment(_L("Create Forward message of Plaintext with attachment"));
	CreateForwardL(messageSelection->At(1), partList, emailTypeList);
	
	emailTypeList=KMsvEmailTypeListMHTMLMessage;

	testUtils->WriteComment(_L("Create Forward message of HTML message without attachment"));
	CreateForwardL(messageSelection->At(5), partList, emailTypeList);

	testUtils->WriteComment(_L("Create Forward message of HTML message with attachment"));
	CreateForwardL(messageSelection->At(1), partList, emailTypeList);
	
	testUtils->WriteComment(_L("Create Forward message of HTML message with additional subject message"));
	CreateForwardWithSubjectL(messageSelection->At(1), partList, emailTypeList);
	
	testUtils->WriteComment(_L("Create Forward message of HTML message as Rich text"));
	CreateForwardWithPlainOrRichTextL(messageSelection->At(1), partList, emailTypeList,EFalse);
	
	testUtils->WriteComment(_L("Create Forward to with message as plain text"));
	CreateForwardWithPlainOrRichTextL(messageSelection->At(1), partList, emailTypeList,ETrue);
	}

LOCAL_C void CreateForwardAsAttachmentL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TBool aPriority)
	{
	testNo++;
	testUtils->TestStart(testNo);
	testUtils->WriteComment(_L("CreateForward started"));
	
	CMsvOperationWait* waiter=CMsvOperationWait::NewLC();
	waiter->Start();
	CImEmailOperation* emailOperation = NULL;
	if(aPriority)
		{
		emailOperation = CImEmailOperation::CreateForwardAsAttachmentL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId,KMsvGlobalOutBoxIndexEntryId, smtpService, aPartList, aMsvEmailTypeList,KUidMsgTypeSMTP, KStandardPriority);	
		}
	else
		{
		emailOperation = CImEmailOperation::CreateForwardAsAttachmentL(waiter->iStatus, *(testUtils->iMsvSession), aMessageId,KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList,KUidMsgTypeSMTP);
		}
	CleanupStack::PushL(emailOperation);
	CActiveScheduler::Start();

	testUtils->WriteComment(_L("CreateForward finished"));
	TestMessageProgressL(*emailOperation);
	
	CleanupStack::PopAndDestroy(2, waiter); // emailOperation, waiter
	}

	
LOCAL_C void CreatForwardAsAttachmentMessageL()
	{
	emailTypeList=0;

	testUtils->WriteComment(_L("Create Forward As Attachment without message priority and SMTP Service ID"));
	CreateForwardAsAttachmentL(messageSelection->At(5), partList, emailTypeList, EFalse);

	emailTypeList=KMsvEmailTypeListMHTMLMessage;

	testUtils->WriteComment(_L("Create Forward As Attachment with priority and SMTP Service ID"));
	CreateForwardAsAttachmentL(messageSelection->At(1), partList, emailTypeList, ETrue);
	}
	
LOCAL_C void doMainL()
	{
	InitL();

	testUtils->CreateMessageFilesL(pop3Service, KMsvGlobalInBoxIndexEntryId, _L("\\mailtest\\IMCM\\"));

	test.Console()->SetPos(0, 6);
	test.Printf(_L("Performing Tests:\n"));
	
	testUtils->GoClientSideL();

	SetDefaultServiceL(smtpService);
	SetAddVCardToEmailL(ETrue, smtpService);
	CreateSignatureL(smtpService);
	SetSignatureToEmailL(ETrue, smtpService);
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;

	// get a list of all email messages in the Inbox
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	CleanupStack::PushL(messageSelection);

	CreatNewMessageL();
	CreatReplyMessageL();
	CreateReceiptMessageL();
	CreatForwardMessageL();
	//CreatForwardAsAttachmentMessageL();
	testUtils->TestHarnessCompleted();
	CleanupStack::PopAndDestroy(messageSelection);
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_PERFORMANCE Test CImEmailOperation class\n"));
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
