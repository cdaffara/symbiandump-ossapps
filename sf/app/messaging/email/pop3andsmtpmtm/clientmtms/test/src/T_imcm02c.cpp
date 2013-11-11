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
// Name of test harness: T_IMCM02C
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests CImEmailOperation - creates forward plaintext/HTML message 
// with/without attachments; forward HTML message with no text alternative. 
// Also tests adding VCard and signature from the SMTP settings.
// Detailed description of test harness:
// As above
// Input files required to run test harness:
// \MSG\IMCMTSRC\contacts.cdb					<DRIVE>:\system\data\contacts.cdb
// \MSG\IMCMTSRC\T_IMCM02C_Entry_Structure.txt	<DRIVE>:\msgtest\IMCM\T_IMCM02C_Entry_Structure.txt
// \MSG\IMCMTSRC\rfc82201.txt					<DRIVE>:\msgtest\IMCM\rfc82201.txt
// \MSG\IMCMTSRC\rfc82202.txt					<DRIVE>:\msgtest\IMCM\rfc82202.txt
// \MSG\IMCMTSRC\rfc82203.txt					<DRIVE>:\msgtest\IMCM\rfc82203.txt
// \MSG\IMCMTSRC\rfc82204.txt					<DRIVE>:\msgtest\IMCM\rfc82204.txt
// \MSG\IMCMTSRC\rfc82205.txt					<DRIVE>:\msgtest\IMCM\rfc82205.txt
// \MSG\IMCMTSRC\rfc82206.txt					<DRIVE>:\msgtest\IMCM\rfc82206.txt
// Intermediate files produced while running test harness:
// <DRIVE>:\msglogs\T_IMCM02C\Entry_Structure.txt
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IMCM02C.<PLATFORM>.<VARIANT>.LOG.txt
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
// 1. \epoc32\release\wins\<VARIANT>\T_IMCM02C.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IMCM02C.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_IMCM02C.exe on the other platform
// 
//

#include "emailtestutils.h"

#include <miutmsg.h>


#include <apfrec.h>
#include <apgicnfl.h>
#include <cemailaccounts.h>

#include <cntdb.h>

// local variables etc //


_LIT(KImcm02Test, "T_IMCM02c - CImEmailOperation Tests");
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
#define KOkFileNameEntryStructure _L("T_IMCM02c_Entry_Structure.txt")

LOCAL_D TBool StartTimeSet=EFalse;
LOCAL_D TTime StartTime;
LOCAL_D TInt  KnownDateTimeIndex=0;

LOCAL_C void SetKnownDateAndTime()
	{
	TDateTime aDateTimes[]={
				TDateTime(2002,EJune,18,10,30,0,0),
				TDateTime(2003,EJanuary,1,21,0,0,0),
				TDateTime(2001,EDecember,18,2,15,0,0),
				TDateTime(2004,EMay,18,17,50,0,0)
				};
	if (!StartTimeSet)
		{
		StartTime.UniversalTime();
		StartTimeSet=ETrue;
		}
	TTime aFakeTime(aDateTimes[KnownDateTimeIndex++]);
	KnownDateTimeIndex&=3;
	User::SetHomeTime(aFakeTime);
	}

LOCAL_C void RestoreDateAndTime()
	{
	if (StartTimeSet)
		{
		TTime aCurrentTime;
		aCurrentTime.UniversalTime();
		TTimeIntervalSeconds aDuration;
		if (!aCurrentTime.SecondsFrom(StartTime,aDuration))
			StartTime+=aDuration;
		User::SetHomeTime(StartTime);
		StartTimeSet=EFalse;
		}
	}

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


LOCAL_C void CreateForwardL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CImEmailOperation* emailOperation = CImEmailOperation::CreateForwardL(testActive->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP);
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
	TSmtpAccount account;
	accounts->GetSmtpAccountL(serviceId, account);
  	accounts->LoadSmtpSettingsL(account, *smtpSettings);

	smtpSettings->SetAddVCardToEmail(addVCard);
	accounts->SaveSmtpSettingsL(account, *smtpSettings);
	
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
	TSmtpAccount account;
	accounts->GetSmtpAccountL(serviceId, account);
	accounts->LoadSmtpSettingsL(account, *smtpSettings);

	accounts->LoadSmtpSettingsL(account,*smtpSettings);
	smtpSettings->SetAddSignatureToEmail(addSignature);
	accounts->SaveSmtpSettingsL(account, *smtpSettings);
	
	CleanupStack::PopAndDestroy(2); // smtpSettings, msvStore/accounts

	if (addSignature)
		testUtils->WriteComment(_L("\tAdd VCard"));
	}


LOCAL_C void SetDefaultServiceL(TMsvId smtpService)
	{
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount account;
	accounts->GetSmtpAccountL(smtpService, account);
	accounts->SetDefaultSmtpAccountL(account);
	// is default service ??
	TSmtpAccount defaultAccount;
	accounts->DefaultSmtpAccountL(defaultAccount);
	if (smtpService != defaultAccount.iSmtpService)
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(accounts);
	}

LOCAL_C void SetIncludeToCcL(TInt aInt, TMsvId serviceId)
	{
	CImSmtpSettings* smtpSettings = new (ELeave) CImSmtpSettings;
	CleanupStack::PushL(smtpSettings);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(serviceId, smtpAccount);
	accounts->LoadSmtpSettingsL(smtpAccount, *smtpSettings);

	smtpSettings->SetToCcIncludeLimitL(aInt);
	
	accounts->SaveSmtpSettingsL(smtpAccount, *smtpSettings);

	CleanupStack::PopAndDestroy(2); // smtpSettings, msvStore/accounts
	}

LOCAL_C void DoPlaintextCreateForwardTestsL(TMsvId aMessageId)
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = 0;

	//
	// Plaintext messages
	//
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message Tests"));

	// basic new message with nothing!
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body
	partList = KMsvMessagePartBody;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment
	partList = KMsvMessagePartAttachments;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body and attachment
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject
	partList = KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body and subject
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and subject
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and subject 
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator
	partList = KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body and originator
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and originator
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and originator
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and originator
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients
	partList = KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and recipients
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and recipients
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and recipients
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message with VCard Tests"));

	// basic new message with VCard
	partList = 0;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body and VCard
	partList = KMsvMessagePartBody;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and VCard
	partList = KMsvMessagePartAttachments;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and VCard
	partList = KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and VCard
	partList = KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients and VCard
	partList = KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and VCard
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and VCard
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and VCard
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// Store signature in the SMTP settings
	SetAddVCardToEmailL(EFalse, smtpService);
	CreateSignatureL(smtpService);
	SetSignatureToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message with signature Tests"));

	// basic new message with signature!
	partList = 0;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body and signature
	partList = KMsvMessagePartBody;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment and signature
	partList = KMsvMessagePartAttachments;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject and signature
	partList = KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator and signature
	partList = KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients and signature
	partList = KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients and signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients and signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// Create a VCard and store in the SMTP settings
	SetAddVCardToEmailL(ETrue, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message with VCard and signature Tests"));

	// basic new message with VCard and Signature!
	partList = 0;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, VCard and Signature
	partList = KMsvMessagePartBody;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, VCard and Signature
	partList = KMsvMessagePartAttachments;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, VCard and Signature
	partList = KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, VCard and Signature
	partList = KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with recipients, VCard and Signature
	partList = KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with originator, recipients, VCard and Signature
	partList = KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// basic new message with subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject, originator, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription | KMsvMessagePartOriginator | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);
	}

LOCAL_C void DoPlaintextCreateForwardIncludeToCcTestsL(TMsvId aMessageId)
	{
	TMsvPartList partList = 0; 
	TMsvEmailTypeList emailTypeList = 0;

	//
	// Plaintext messages
	//
	
	// set #1 - no To/Cc header information included
	SetIncludeToCcL(0, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message IncludeToCc:0 Tests"));

	// Body only, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// attachments only, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// Body and attachments, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);


	// set #2 - all To/Cc header information included
	SetIncludeToCcL(10, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message IncludeToCc:10 Tests"));

	// Body only, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// attachments only, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// Body and attachments, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	
	// set #3 - truncated To/Cc header information included
	SetIncludeToCcL(3, smtpService);
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message IncludeToCc:3 Tests"));

	// Body only, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// attachments only, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// Body and attachments, Forward to all
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);
	
	// Reset IncludeToCc setting.
	SetIncludeToCcL(0, smtpService);
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

//
//	Create Plaintext Forward tests
//
	SetKnownDateAndTime();

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests1:"));
	testUtils->WriteComment(_L("Test 1 - Create Plaintext Forward Tests"));
	// Forward to normal plaintext message with no attachments
	testUtils->WriteComment(_L("Create Forward to Plaintext message with no attachments Tests"));
	TRAPD(ret,DoPlaintextCreateForwardTestsL(messageSelection->At(5)));
	if (ret)
		{
		testUtils->TestFinish(testNo,ret);
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}

	SetKnownDateAndTime();

	test.Console()->SetPos(3, 8);
	test.Printf(_L("Create Forward Tests 2:  "));
	//Create Forwarded message to plaintext message with attachment
	testUtils->WriteComment(_L("Test 2 -Create Forward to Plaintext message with attachment Tests"));
	TRAP(ret,DoPlaintextCreateForwardTestsL(messageSelection->At(1)));
	if (ret)
		{
		testUtils->TestFinish(testNo,ret);
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}

	SetKnownDateAndTime();

	//Create Forwarded html message with attachment and no text alternative to the HTML
	test.Console()->SetPos(3, 9);
	test.Printf(_L("Create Forward Tests 3:  "));
	testUtils->WriteComment(_L("Test 3 - Create Forward to HTML message with attachment and no text alt. Tests"));
	TRAP(ret,DoPlaintextCreateForwardTestsL(messageSelection->At(3)));
	if (ret)
		{
		testUtils->TestFinish(testNo,ret);
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}

	SetKnownDateAndTime();

	test.Console()->SetPos(3, 10);
	test.Printf(_L("Create Forward Tests 4:  "));
	//Create Forward to to plaintext message, include To/Cc info in body header.
	testUtils->WriteComment(_L("Test 4 - Create Forward to Plaintext message including to/cc info Tests"));
	TRAP(ret,DoPlaintextCreateForwardIncludeToCcTestsL(messageSelection->At(6)));
	if (ret!=KErrNone)
		{
		testUtils->TestFinish(testNo,ret);
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}
	
	SetKnownDateAndTime();

	test.Console()->SetPos(3, 11);
	test.Printf(_L("Create Forward Tests 5:  "));
	//Create Forward to to plaintext message, include To/Cc info in body header.
	testUtils->WriteComment(_L("Test 5 - Create Forward to Plaintext message including to only info Tests"));
	TRAP(ret,DoPlaintextCreateForwardIncludeToCcTestsL(messageSelection->At(7)));
	if (ret!=KErrNone)
		{
		testUtils->TestFinish(testNo,ret);
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}
	
	SetKnownDateAndTime();

	test.Console()->SetPos(3, 12);
	test.Printf(_L("Create Forward Tests 6:  "));
	//Create Forward to to plaintext message, include To/Cc info in body header.
	testUtils->WriteComment(_L("Test 6 - Create Forward to Plaintext message including cc only info Tests"));
	TRAP(ret,DoPlaintextCreateForwardIncludeToCcTestsL(messageSelection->At(8)));
	if (ret!=KErrNone)
		{
		testUtils->TestFinish(testNo,ret);
		testUtils->TestHarnessFailed(ret);
		delete messageSelection;
		Closedown();
		return;
		}

	RestoreDateAndTime();

	testUtils->FindChildrenL(KMsvGlobalOutBoxIndexEntryId, parsedFileName.FullName(), ETrue, EFalse); // writes message info (from Outbox) into files

	testUtils->TestFinish(testNo-1);
 	testUtils->TestHarnessCompleted();

	delete messageSelection;
	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCM02c Test CImEmailOperation class\n"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	RestoreDateAndTime();
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
