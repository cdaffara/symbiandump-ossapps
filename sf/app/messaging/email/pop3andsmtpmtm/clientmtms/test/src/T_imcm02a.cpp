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
// Name of test harness: T_IMCM02A
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests CImEmailOperation - creates new plaintext and html messages; Also 
// tests adding VCard and signature from the SMTP settings.
// Detailed description of test harness:
// As above
// Input files required to run test harness:
// \MSG\IMCMTSRC\contacts.cdb					<DRIVE>:\system\data\contacts.cdb
// \MSG\IMCMTSRC\T_IMCM02A_Entry_Structure.txt	<DRIVE>:\msgtest\IMCM\T_IMCM02a_Entry_Structure.txt
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_IMCM02A\T_IMCM.log
// <DRIVE>:\msglogs\T_IMCM02A\Entry_Structure.txt
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMCM02A.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMCM02A.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMCM02A.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMCM02A.exe on the other platform
// 
//

#include "emailtestutils.h"

#include <miutlog.h>
#include <miutmsg.h>


#include <apfrec.h>
#include <apgicnfl.h>
#include <ecom/ecom.h>
#include <cemailaccounts.h>
#include <iapprefs.h>

/*
// These headers are required when generating the contacts.cdb file
#include <cntdb.h>
#include <cntfldst.h>
#include <cntitem.h>
*/
// local variables etc //

#define KPeriod					10000	// period of timer

_LIT(KImcm02Test, "T_IMCM02a - CImEmailOperation Tests");
_LIT(KMsvServerPattern, "!MsvServer*");
RTest test(KImcm02Test);

LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D CImLog* log;

LOCAL_D TMsvId messageId;
LOCAL_D TMsvId smtpService;
LOCAL_D TMsvId smtpService2;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CMsvEntrySelection* messageSelection;
LOCAL_D TInt testNo = 1;

#define KComponent				_L("IMCM")
#define KTestMessageDir			_L("")
#define KOkFileNameEntryStructure _L("T_IMCM02a_Entry_Structure.txt")

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
	// display the current progress
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);

	const TDesC8& progBuf = iEmailOperation->ProgressL();	
	paramPack.Set(progBuf);
	TMsvId progress=paramPack();	

	test.Console()->SetPos(0, 10);
	test.Printf(_L("   MessageId %d		\n"),progress);

	IssueRequest();
	};

//


/*
// Do not delete this function as it may be required to regenerate the contacts.cdb file
// if there are any source/binary incompatible changes!

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

	testUtils->WriteComment(_L("CImEmailOperation Tests"));
	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("********* T_IMCM02a Test CImEmailOperation *********"));
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
	log->AppendComment(_L8("**********    T_IMCM02a Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	User::After(5000000);
	}

LOCAL_C void TestMessageProgressL(CImEmailOperation& aEmailOperation, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo, TMsvId aSmtpServiceId)
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
	messageId=paramPack();
	TInt error = aEmailOperation.iStatus.Int();

	TMsvEmailEntry entry;
	TMsvId serviceId;
	testUtils->iMsvSession->GetEntry(messageId, serviceId, entry);

	if (aSmtpServiceId != KMsvNullIndexEntryId)
		test(entry.iServiceId == aSmtpServiceId);

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

LOCAL_C void CreateNewL(TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo, TMsvId aSmtpServiceId = KMsvNullIndexEntryId)
	{
	testUtils->TestStart(aTestNo);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);


	CImEmailOperation* emailOperation;
	if (aSmtpServiceId == KMsvNullIndexEntryId)
		emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP);
	else
		emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, aSmtpServiceId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestUiTimer* testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
	testActive->StartL();
	testUiTimer->IssueRequest();
	CActiveScheduler::Start();

	TestMessageProgressL(*emailOperation, aPartList, aMsvEmailTypeList, aTestNo, aSmtpServiceId);

	CleanupStack::PopAndDestroy(3); // testActive, emailOperation, testUiTimer
	}

LOCAL_C void SetAddVCardToEmailL(TBool addVCard, TMsvId serviceId)
	{
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);

	TSmtpAccount smtpAccount;
 	accounts->GetSmtpAccountL(serviceId, smtpAccount);
 	accounts->LoadSmtpSettingsL(smtpAccount, *smtpSettings);
	
	smtpSettings->SetAddVCardToEmail(addVCard);
	
	accounts->SaveSmtpSettingsL(smtpAccount, *smtpSettings);
	
	CleanupStack::PopAndDestroy(2, accounts); // smtpSettings, accounts
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
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);

  	TSmtpAccount smtpAccount;
 	accounts->GetSmtpAccountL(serviceId, smtpAccount);
 	accounts->LoadSmtpSettingsL(smtpAccount, *smtpSettings);

	smtpSettings->SetAddSignatureToEmail(addSignature);
	
	accounts->SaveSmtpSettingsL(smtpAccount, *smtpSettings);
	
	CleanupStack::PopAndDestroy(2, accounts); // smtpSettings, accounts
	if (addSignature)
		log->AppendComment(_L8("\tAdd VCard"));
	}


LOCAL_C void SetDefaultServiceL(TMsvId smtpService)
	{
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(smtpService, smtpAccount);
	accounts->SetDefaultSmtpAccountL(smtpAccount);
	CleanupStack::PopAndDestroy(accounts);
	}

LOCAL_C void DoPlaintextCreateNewTestsL()
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = 0;

	//
	// Plaintext messages
	//
	log->AppendComment(_L8("\tPlaintext Create New Message Tests"));

	// basic new message with nothing!
	CreateNewL(partList, emailTypeList, testNo++);

	// only body
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and attachment
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and subject
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and subject
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and subject 
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator
	partList = KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and originator
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject and originator
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with recipients
	partList = KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator and recipients
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	log->AppendComment(_L8("\tPlaintext Create New Message with VCard Tests"));

	// basic new message with VCard
	partList = 0;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and VCard
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and VCard
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject and VCard
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator and VCard
	partList = KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with recipients and VCard
	partList = KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and VCard
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// Store signature in the SMTP settings
	log->AppendComment(_L8("\tPlaintext Create New Message with signature Tests"));
	SetAddVCardToEmailL(EFalse, smtpService);
	CreateSignatureL(smtpService);
	SetSignatureToEmailL(ETrue, smtpService);

	// basic new message with signature!
	partList = 0;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and signature
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and signature
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject and signature
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator and signature
	partList = KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with recipients and signature
	partList = KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	log->AppendComment(_L8("\tPlaintext Create New Message with VCard and Signature Tests"));
	SetAddVCardToEmailL(ETrue, smtpService);

	// basic new message with VCard and Signature!
	partList = 0;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, VCard and Signature
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, VCard and Signature
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, VCard and Signature
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator, VCard and Signature
	partList = KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with recipients, VCard and Signature
	partList = KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator, recipients, VCard and Signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);
	}

LOCAL_C void DoHtmlCreateNewTestsL()
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = KMsvEmailTypeListMHTMLMessage;

	//
	// HTML Messages
	//
	log->AppendComment(_L8("\tHTML Create New Message Tests"));

	// basic new message with nothing!
	partList = 0;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and attachment
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and subject
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and subject
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and subject 
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator
	partList = KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and originator
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject and originator
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with recipients
	partList = KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	TRAPD(createErr, CreateNewL(partList, emailTypeList, testNo++));

	// only body, attachment and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator and recipients
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	log->AppendComment(_L8("\tHTML Create New Message with VCard Tests"));

	// basic new message with VCard
	partList = 0;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and VCard
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and VCard
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject and VCard
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator and VCard
	partList = KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with recipients and VCard
	partList = KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and VCard
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// Store signature in the SMTP settings
	SetAddVCardToEmailL(EFalse, smtpService);
	CreateSignatureL(smtpService);
	SetSignatureToEmailL(ETrue, smtpService);
	log->AppendComment(_L8("\tHTML Create New Message with Signature Tests"));

	// basic new message with signature!
	partList = 0;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and signature
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and signature
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject and signature
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator and signature
	partList = KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with recipients and signature
	partList = KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	log->AppendComment(_L8("\tHTML Create New Message with signature and VCard Tests"));

	// basic new message with VCard and Signature!
	partList = 0;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, VCard and Signature
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, VCard and Signature
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, VCard and Signature
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator, VCard and Signature
	partList = KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with recipients, VCard and Signature
	partList = KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with originator, recipients, VCard and Signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);
	}

LOCAL_C void DoCreateNewUsingNonDefaultServiceTestsL()
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = 0;

	//
	// Plaintext Messages
	//
	log->AppendComment(_L8("\tCreate New Message using Non-default service Tests"));

	// basic new message with nothing!
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++, smtpService2);

	SetAddVCardToEmailL(ETrue, smtpService2);
	CreateNewL(partList, emailTypeList, testNo++, smtpService2);

	CreateSignatureL(smtpService2);
	SetSignatureToEmailL(ETrue, smtpService2);
	CreateNewL(partList, emailTypeList, testNo++, smtpService2);

	SetAddVCardToEmailL(EFalse, smtpService2);
	CreateNewL(partList, emailTypeList, testNo++, smtpService2);

	//
	// HTML Messages
	//
	log->AppendComment(_L8("\tCreate New HTML Message using Non-default service Tests"));

	// basic new message with nothing!
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	SetSignatureToEmailL(EFalse, smtpService2);
	CreateNewL(partList, emailTypeList, testNo++, smtpService2);

	SetAddVCardToEmailL(ETrue, smtpService2);
	CreateNewL(partList, emailTypeList, testNo++, smtpService2);

	CreateSignatureL(smtpService2);
	SetSignatureToEmailL(ETrue, smtpService2);
	CreateNewL(partList, emailTypeList, testNo++, smtpService2);

	SetAddVCardToEmailL(EFalse, smtpService2);
	CreateNewL(partList, emailTypeList, testNo++, smtpService2);

	}

LOCAL_C void DoSaveNonHTMLTestsL()
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = KMsvEmailTypeListMHTMLMessage;

	//
	// HTML Messages
	//
	log->AppendComment(_L8("\tCreate New HTML Message using Non-default service Tests"));

	// basic new message with nothing!
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	if( messageId != KMsvNullIndexEntryId )
		{
		// Change the message to be non-HTML
		CMsvEntry* msvEntry = testUtils->iMsvSession->GetEntryL(messageId);
		CleanupStack::PushL(msvEntry);
		TMsvEmailEntry entry = msvEntry->Entry();
	    entry.SetMHTMLEmail(EFalse);
		msvEntry->ChangeL(entry);

		// Save the message - first create the CImEmailMessage object
		CImEmailMessage* message = CImEmailMessage::NewL(*msvEntry);
		CleanupStack::PushL(message);

		// Create rich text object
		CParaFormatLayer* paraF = CParaFormatLayer::NewL(); 
		CleanupStack::PushL(paraF);

		CCharFormatLayer* charF = CCharFormatLayer::NewL(); 
		CleanupStack::PushL(charF);

		CRichText* body = CRichText::NewL(paraF, charF); 
		CleanupStack::PushL(body);

		// Create the test active object
		CTestActive* testActive = new (ELeave) CTestActive();
		CleanupStack::PushL(testActive);

		testActive->StartL();
		message->StoreBodyTextL(messageId, *body, testActive->iStatus);
		CActiveScheduler::Start();

		CleanupStack::PopAndDestroy(6, msvEntry);
		}
	}

/*LOCAL_C void DoMiscellaneousTestsL()
	{
//
//	Test functionality where the user requests VCard but contacts.cdb does not exist
//
	// rename contacts.cdb file so that it looks like it has been deleted or doesn't exist
	_LIT(KContactsDb, "C:\\system\\data\\contacts.cdb");
	_LIT(KOtherName, "C:\\system\\data\\contact.tmp");
	CFileMan* fileMan = CFileMan::NewL(testUtils->iMsvSession->FileSession());
	CleanupStack::PushL(fileMan);
	TInt error = fileMan->Rename(KContactsDb, KOtherName);

	SetAddVCardToEmailL(ETrue, smtpService);

	TMsvPartList partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	TMsvEmailTypeList emailTypeList = 0;
	TRAPD(err, CreateNewL(partList, emailTypeList, testNo++));
	test(err == KErrNotFound);
	
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	TRAP(err, CreateNewL(partList, emailTypeList, testNo++));
	test(err == KErrNotFound);

	SetAddVCardToEmailL(EFalse, smtpService);

	// rename the contacts database file back to Contacts.cdb if it was changed!
	if (error != KErrNotFound)
		error = fileMan->Rename(KOtherName, KContactsDb);
	CleanupStack::PopAndDestroy(fileMan);

//
//	Test functionality where the user requests signature but signature does not exist
//
	SetSignatureToEmailL(ETrue, smtpService);
	emailTypeList = 0;
	CreateNewL(partList, emailTypeList, testNo++);
	
	emailTypeList = KMsvEmailTypeListMHTMLMessage;
	CreateNewL(partList, emailTypeList, testNo++);
	}
*/
LOCAL_C void WaitForServerClose()
	{
	//Kill the message server if its running
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

LOCAL_C void CreateTwoSetsOfServicesL()
	{
	testUtils->GoClientSideL();
	
	// create service 1
	
	// creates a single set of smtp & pop services
	testUtils->CreatePopServiceL(); 
	
	// create smtp id
	smtpService = testUtils->CreateSmtpServiceL();
	SetDefaultServiceL(smtpService);
	
	TMsvId popService = testUtils->FindPopServiceL(); 
	
	// create service 2

	// email account utility
	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	// smtp settings	
	CImSmtpSettings* settings = new(ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);
	settings->Reset();

	// dummy pop settings
	CImPop3Settings* pop3settings = new(ELeave) CImPop3Settings;
	CleanupStack::PushL(pop3settings);

	// dummy preferences
	CImIAPPreferences* prefs = CImIAPPreferences::NewLC();
	
	// set defaults
	accounts->PopulateDefaultPopSettingsL(*pop3settings, *prefs);
	accounts->PopulateDefaultSmtpSettingsL(*settings, *prefs);

	
	_LIT(KSmtpServer, "smtp.dial.pipex.com");
	settings->SetServerAddressL(KSmtpServer);
	settings->SetEmailAliasL(_L("Messaging Test1"));
	_LIT(KSmtpEmailAddress, "msgtest1@internet-test.plc.psion.com");
	settings->SetEmailAddressL(KSmtpEmailAddress);
	settings->SetReplyToAddressL(KSmtpEmailAddress);
	settings->SetBodyEncoding(EMsgOutboxMIME);
	settings->SetReceiptAddressL(KSmtpEmailAddress);
	settings->SetPort(25);

	// create account
 	TPopAccount popAccount = accounts->CreatePopAccountL(_L("SMTP Service 2"), *pop3settings, *prefs, EFalse);
 	TSmtpAccount smtpAccountId = accounts->CreateSmtpAccountL(popAccount, *settings, *prefs, EFalse);

//	accounts->SetDefaultPOPAccountL(id);
//	smtpService2 = accounts->DefaultSMTPServiceL();
	TSmtpAccount smtpAccountId2;
	accounts->DefaultSmtpAccountL(smtpAccountId2);
	smtpService2 = smtpAccountId2.iSmtpService;

	// set default service back to service 1
//	accounts->ChangeDefaultPOPServiceL(popService);
//	accounts->ChangeDefaultSMTPServiceL(smtpService);
	SetDefaultServiceL(smtpService);

	CleanupStack::PopAndDestroy(4, accounts); // prefs, pop3settings, settings, accounts
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
	testUtils->ClearEmailAccountsL();
	testUtils->CleanMessageFolderL();
	CreateTwoSetsOfServicesL();

	testUtils->WriteComment(_L("CImEmailOperation Tests"));
	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);

	SetDefaultServiceL(smtpService);

	// get a list of all email messages in the Inbox
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	delete messageSelection;
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);

	test.Console()->ClearScreen();
	}

LOCAL_C TBool CompareLogs(CEmailTestUtils* testUtils)
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

	CreateTwoSetsOfServicesL();

	test.Console()->SetPos(0, 6);
	test.Printf(_L("Performing Tests:\n"));
	
	SetDefaultServiceL(smtpService);

//	CreateVCardL();
	// get a list of all email messages in the Inbox
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	delete messageSelection;
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);

	TParse parsedFileName;
	TFileName name(KTestMessageDir);
	testUtils->ResolveLogFile(name, parsedFileName);

//	
//	Create New tests
//
	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create New Tests1:    "));
	testUtils->WriteComment(_L("Create New Plaintext Message Tests"));
	DoPlaintextCreateNewTestsL();
	testUtils->FindChildrenL(KMsvGlobalOutBoxIndexEntryId, parsedFileName.FullName(), ETrue, EFalse); // writes message info (from Outbox) into files
	DeleteMessageFolderAndCreateMessagesL();

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create New Tests2:    "));
	testUtils->WriteComment(_L("Create New HMTL Message Tests"));
	DoHtmlCreateNewTestsL();
	testUtils->FindChildrenL(KMsvGlobalOutBoxIndexEntryId, parsedFileName.FullName(), EFalse, EFalse); // writes message info (from Outbox) into files
	DeleteMessageFolderAndCreateMessagesL();

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create New Tests3:    "));
	testUtils->WriteComment(_L("Create New Message Using Non-default Service Tests"));
	DoCreateNewUsingNonDefaultServiceTestsL();
	testUtils->FindChildrenL(KMsvGlobalOutBoxIndexEntryId, parsedFileName.FullName(), EFalse, EFalse); // writes message info (from Outbox) into files
	DeleteMessageFolderAndCreateMessagesL();

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Save New Tests4:    "));
	testUtils->WriteComment(_L("Test Saving a Non-HTML message."));
	DoSaveNonHTMLTestsL();
	testUtils->FindChildrenL(KMsvGlobalOutBoxIndexEntryId, parsedFileName.FullName(), EFalse, EFalse); // writes message info (from Outbox) into files

//
//	Make sure that we can create emails when there is no contacts database or no richtext 
//	for signature.
//
//	test.Console()->SetPos(3, 7);
//	test.Printf(_L("Create New Tests3:    "));
//	testUtils->WriteComment(_L("Doing Miscellaneous Tests"));
//	DoMiscellaneousTestsL();
//	testUtils->FindChildrenL(KMsvGlobalOutBoxIndexEntryId, parsedFileName.FullName(), EFalse, EFalse); // writes message info (from Outbox) into files

	testUtils->TestFinish(testNo-1);
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
