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
// Name of test harness: T_IMCM02E1
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests CImEmailOperation - creates plaintext/HTML Receipt message 
// with/without attachments; Also tests adding VCard and signature from the 
// SMTP settings.
// Detailed description of test harness:
// As above
// Input files required to run test harness:
// \MSG\IMCMTSRC\contacts.cdb					<DRIVE>:\system\data\contacts.cdb
// \MSG\IMCMTSRC\T_IMCM02E1_Entry_Structure.txt	<DRIVE>:\msgtest\IMCM\T_IMCM02E1_Entry_Structure.txt
// \MSG\IMCMTSRC\rfc82201.txt					<DRIVE>:\msgtest\IMCM\rfc82201.txt
// \MSG\IMCMTSRC\rfc82202.txt					<DRIVE>:\msgtest\IMCM\rfc82202.txt
// \MSG\IMCMTSRC\rfc82203.txt					<DRIVE>:\msgtest\IMCM\rfc82203.txt
// \MSG\IMCMTSRC\rfc82204.txt					<DRIVE>:\msgtest\IMCM\rfc82204.txt
// \MSG\IMCMTSRC\rfc82205.txt					<DRIVE>:\msgtest\IMCM\rfc82205.txt
// \MSG\IMCMTSRC\rfc82206.txt					<DRIVE>:\msgtest\IMCM\rfc82206.txt
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_IMCM02E1\Entry_Structure.txt
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMCM02E1.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMCM02E1.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMCM02E1.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMCM02E1.exe on the other platform
// 
//

#include "emailtestutils.h"

#include <miutmsg.h>


#include <apfrec.h>
#include <apgicnfl.h>
#include <ecom/ecom.h>
#include <cemailaccounts.h>
#include <mmsvattachmentmanager.h>

#include <cntdb.h>

// local variables etc //

#define KPeriod					10000	// period of timer

_LIT(KImcm02Test, "T_IMCM02e1 - CImEmailOperation Tests");
RTest test(KImcm02Test);

LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D TMsvId pop3Service;
LOCAL_D TMsvId smtpService;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CMsvEntrySelection* messageSelection;
LOCAL_D TInt testNo = 1;
LOCAL_D TBool testFailed=EFalse;

#define KComponent				_L("IMCM")
#define KTestMessageDir			_L("")
#define KOkFileNameEntryStructure _L("T_IMCM02e1_Entry_Structure.txt")


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
		testFailed=ETrue;
		return;
		}
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->ClearEmailAccountsL();
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	testUtils->WriteComment(_L("CImEmailOperation Tests"));
	}
	
LOCAL_C void Closedown()
	{
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler
	User::After(5000000);
	}


LOCAL_C void CreateReceiptL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CImEmailOperation* emailOperation = CImEmailOperation::CreateReceiptL(testActive->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	testActive->StartL();
	CActiveScheduler::Start();
	testUtils->TestFinish(aTestNo,testActive->iStatus.Int());
	CleanupStack::PopAndDestroy(2); // testActive, emailOperation
	}

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

	CleanupStack::PopAndDestroy(2); // smtpSettings, accounts
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
	
	// is default service ??
	accounts->GetSmtpAccountL(smtpService, smtpAccount);
	if (smtpService != smtpAccount.iSmtpService)
		User::Leave(KErrNotFound);
	CleanupStack::PopAndDestroy(accounts);
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

	testActive->StartL();
	CActiveScheduler::Start();
	if (testActive->iStatus.Int())
		{
		testUtils->WriteComment(_L("CreateNewPlaintextMessageWithIncompleteAttachmentL failed"));
		testUtils->TestHarnessFailed(testActive->iStatus.Int());
		}

	//check progress
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf1 = emailOperation->ProgressL();	
	paramPack.Set(progBuf1);
	TMsvId messageId = paramPack();	

	CleanupStack::PopAndDestroy(2); // testActive, emailOperation

	//
	// Add an attachment
	//
	testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	_LIT(KAttachment1, "C:\\mailtest\\IMCM\\rfc82201.txt");

	CImEmailMessage* imEmailMessage = CImEmailMessage::NewLC(*(testUtils->iMsvEntry));

	testUtils->iMsvEntry->SetEntryL(messageId);
	CMsvAttachment* attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);
	imEmailMessage->AttachmentManager().AddAttachmentL(KAttachment1, attachmentInfo, testActive->iStatus);
	CleanupStack::Pop(attachmentInfo);

	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete

	if (testActive->iStatus.Int())
		{
		testUtils->WriteComment(_L("CreateNewPlaintextMessageWithIncompleteAttachmentL failed"));
		testUtils->TestHarnessFailed(testActive->iStatus.Int());
		}

	testUtils->iMsvEntry->SetEntryL(messageId);
	attachmentInfo = CMsvAttachment::NewL(CMsvAttachment::EMsvFile);
	CleanupStack::PushL(attachmentInfo);
	imEmailMessage->AttachmentManager().AddAttachmentL(KAttachment1, attachmentInfo, testActive->iStatus);
	CleanupStack::Pop(attachmentInfo);

	testActive->StartL();
	CActiveScheduler::Start();	// wait for the asynch operation to complete

	if (testActive->iStatus.Int())
		{
		testUtils->WriteComment(_L("CreateNewPlaintextMessageWithIncompleteAttachmentL failed"));
		testUtils->TestHarnessFailed(testActive->iStatus.Int());
		}

	const TDesC8& progBuf2 = imEmailMessage->ProgressL();	
	paramPack.Set(progBuf2);
	TMsvId attachmentId = paramPack();	

	CleanupStack::PopAndDestroy(2); // imEmailMessage, testActive

	//
	// Set the attachment to be not complete
	//
	
	testUtils->iMsvEntry->SetEntryL(attachmentId);
	TMsvEmailEntry attachmentEntry = testUtils->iMsvEntry->Entry();
	attachmentEntry.SetComplete(EFalse);
	testUtils->iMsvEntry->ChangeL(attachmentEntry);

	return messageId;
	}

LOCAL_C void DoPlaintextCreateReceiptTestsL(TMsvId aMessageId)
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = 0;

	//
	// Plaintext messages
	//
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message Tests"));

	// basic new message with nothing!
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body
	partList = KMsvMessagePartBody;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment
	partList = KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and attachment
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject
	partList = KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and subject
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and subject
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and subject 
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator
	partList = KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and originator
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and originator
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients
	partList = KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and recipients
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message with VCard Tests"));

	// basic new message with VCard
	partList = 0;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and VCard
	partList = KMsvMessagePartBody;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and VCard
	partList = KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and VCard
	partList = KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and VCard
	partList = KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients and VCard
	partList = KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and VCard
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// Store signature in the SMTP settings
	SetAddVCardToEmailL(EFalse, smtpService);
	CreateSignatureL(smtpService);
	SetSignatureToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message with signature Tests"));

	// basic new message with signature!
	partList = 0;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and signature
	partList = KMsvMessagePartBody;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and signature
	partList = KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and signature
	partList = KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and signature
	partList = KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients and signature
	partList = KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message with VCard and signature Tests"));

	// basic new message with VCard and Signature!
	partList = 0;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, VCard and Signature
	partList = KMsvMessagePartBody;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, VCard and Signature
	partList = KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, VCard and Signature
	partList = KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, VCard and Signature
	partList = KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients, VCard and Signature
	partList = KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients, VCard and Signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);
	}

LOCAL_C void DoHtmlCreateReceiptTestsL(TMsvId aMessageId)
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = KMsvEmailTypeListMHTMLMessage;

	//
	// HTML Messages
	//
	testUtils->WriteComment(_L("\tHTML Create Forward Message Tests"));

	// basic new message with nothing!
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body
	partList = KMsvMessagePartBody;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment
	partList = KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and attachment
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject
	partList = KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and subject
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and subject
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and subject 
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator
	partList = KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and originator
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and originator
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients
	partList = KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and recipients
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tHTML Create Forward Message with VCard Tests"));

	// basic new message with VCard
	partList = 0;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and VCard
	partList = KMsvMessagePartBody;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and VCard
	partList = KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and VCard
	partList = KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and VCard
	partList = KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients and VCard
	partList = KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and VCard
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// Store signature in the SMTP settings
	SetAddVCardToEmailL(EFalse, smtpService);
	CreateSignatureL(smtpService);
	SetSignatureToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tHTML Create Forward Message with signature Tests"));

	// basic new message with signature!
	partList = 0;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body and signature
	partList = KMsvMessagePartBody;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and signature
	partList = KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and signature
	partList = KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and signature
	partList = KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients and signature
	partList = KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tHTML Create Forward Message with VCard and signature Tests"));

	// basic new message with VCard and Signature!
	partList = 0;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, VCard and Signature
	partList = KMsvMessagePartBody;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, VCard and Signature
	partList = KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, VCard and Signature
	partList = KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, VCard and Signature
	partList = KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients, VCard and Signature
	partList = KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients, VCard and Signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateReceiptL(aMessageId, partList, emailTypeList, testNo++);
	}

LOCAL_C TBool CompareLogsL()
	{
	TBool errorOccured = EFalse;

	CDesCArray* correctFile = new (ELeave) CDesCArrayFlat(1);
	CPtrCArray* resultFile = new (ELeave) CPtrCArray(1);

	TParse parsedFileName;
	TParse parsedResultFileName;
	TFileName name;
	name.Append(KOkFileNameEntryStructure);
	testUtils->ResolveFile(KComponent, name, parsedFileName);
	correctFile->AppendL(parsedFileName.FullName());
	name.Zero();
	name.Copy(KFileNameEntryStructure);
	testUtils->ResolveLogFile(name, parsedResultFileName);
	resultFile->AppendL(parsedResultFileName.FullName());

	RFileReadStream file1;
	RFileReadStream file2;
	HBufC8* line1Buffer = HBufC8::NewLC(1024);
	TPtr8 line1 = line1Buffer->Des();
	TBuf8<1> aChar1;

	HBufC8* line2Buffer = HBufC8::NewLC(1024);
	TPtr8 line2 = line2Buffer->Des();
	TBuf8<1> aChar2;

	for(TInt i=0; i<correctFile->MdcaCount(); i++)
		{
		TFileName file1Name = correctFile->MdcaPoint(i);

		TFileName file2Name = resultFile->MdcaPoint(i);

		TInt error1=KErrNone;
		TInt error2=KErrNone;
		
		test.Printf(_L("\n%s"), file2Name.PtrZ());

		error1=file1.Open(testUtils->FileSession(), file1Name, EFileShareAny);
		if(error1!=KErrNone)
			{
			errorOccured = ETrue;
			}
		else
			{
			error2=file2.Open(testUtils->FileSession(), file2Name, EFileShareAny);
			if(error2!=KErrNone)
				{
				errorOccured = ETrue;
				}
			}
		
		if((error1==KErrNone)&&(error2==KErrNone))
			{
			// read the file into the conversion object
			TBool finished = EFalse;
	
			do	{
				line1.FillZ();
				line1.SetLength(0);
				// compile the lines one char at a time
				do	{
					TRAPD( error, file1.ReadL(aChar1, 1) ); 
					if (error!=KErrEof)
						line1.Append(aChar1);
					else
						{
						finished = ETrue;
						break;
						}
					// stop at the end of line or no more data
					}
				while((aChar1[0]!=0x0A)&&(line1.Length()<1024));

				line2.FillZ();
				line2.SetLength(0);
				do	{
					TRAPD( error, file2.ReadL(aChar2, 1) ); 
					if (error!=KErrEof)
						line2.Append(aChar2);
					else
						{
						finished = ETrue;
						break;
						}
					// stop at the end of line or no more data
					}
				while((aChar2[0]!=0x0A)&&(line2.Length()<1024));

				// get rid of white space
   				line1.TrimRight();
   				line2.TrimRight();
   
   				if(line1.Compare(line2) != 0)
   					{
   					// error so no point in continuing to compare!!
   					errorOccured = ETrue;
   					file1.Close();
   					file2.Close();
   					delete correctFile;
   					delete resultFile;
   					CleanupStack::PopAndDestroy(2);  //line 1 , line 2
   					return ETrue;
   					}
				}
			while(!finished);
			}		

		file1.Close();
		file2.Close();
		}

	delete correctFile;
	delete resultFile;
	CleanupStack::PopAndDestroy(2);  //line 1 , line 2
	
	return errorOccured;
	}

LOCAL_C void doMainL()
	{
	InitL();
	if (testFailed)
		{
		Closedown();
		return;
		}
	testUtils->GoClientSideL();
	smtpService = testUtils->CreateSmtpServiceL();
	pop3Service = testUtils->CreatePopServiceL();
	testUtils->GoServerSideL();

	testUtils->CreateMessageFilesL(pop3Service, KMsvGlobalInBoxIndexEntryId, _L("c:\\mailtest\\IMCM\\"));

	test.Console()->SetPos(0, 6);
	test.Printf(_L("Performing Tests:\n"));
	
	testUtils->GoClientSideL();

	SetDefaultServiceL(smtpService);

	// get a list of all email messages in the Inbox
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	delete messageSelection;
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);

	TParse parsedFileName;
	TFileName name(KTestMessageDir);
	testUtils->ResolveLogFile(name, parsedFileName);

	test.Console()->SetPos(3, 8);
	test.Printf(_L("Create Receipt Tests 1:"));
	//Create Receipt to message with attachment which is not complete
	testUtils->WriteComment(_L("Test 1 - Create Receipt to Plaintext message with incomplete atcmt Tests"));
	TMsvId messageWithIncompleteAttachment = CreateNewPlaintextMessageWithIncompleteAttachmentL();
	TRAPD(ret,DoPlaintextCreateReceiptTestsL(messageWithIncompleteAttachment));
	if (ret)
		{
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}

//
//	Create HTML Receipttests
//
	test.Console()->SetPos(3, 9);
	test.Printf(_L("Create Receipt Tests 2:"));
	testUtils->WriteComment(_L("Test 2 - Create HTML Receipt Tests"));
	// Receipt to normal plaintext message with no attachments
	testUtils->WriteComment(_L("Create Receipt to Plaintext message with no atcmts Tests"));
	TRAP(ret,DoHtmlCreateReceiptTestsL(messageSelection->At(5)));
	if (ret)
		{
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}

	test.Console()->SetPos(3, 10);
	test.Printf(_L("Create Receipt Tests 3:"));
	//Create Receipt to plaintext message with attachment
	testUtils->WriteComment(_L("Test 3 - Create Receipt to Plaintext message with atcmt Tests"));
	TRAP(ret,DoHtmlCreateReceiptTestsL(messageSelection->At(1)));
	if (ret)
		{
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}


	testUtils->FindChildrenL(KMsvGlobalOutBoxIndexEntryId, parsedFileName.FullName(), ETrue, EFalse); // writes message info (from Outbox) into files

	testUtils->TestFinish(testNo-1);
 	testUtils->TestHarnessCompleted();

	delete messageSelection;
	Closedown();
	
	// shouldn't have to do this, but convertor plugins aren't 
	// doing it yet so we get a false memory leak so remove this
	// when they fix that.
	REComSession::FinalClose();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCM02e1 Test CImEmailOperation class\n"));
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
