// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <test/testexecutelog.h>
#include <cemailaccounts.h>
#include <iapprefs.h>

#include "t_smtp_utc_step.h"

RTest test(_L("UTC SMTP Test"));
_LIT(KSmtpServer, "10.22.64.6");
_LIT(KSmtpEmailAddress, "davids@msexchange2k.closedtest.intra");

const TInt KOneHour = 3600;
const TInt KLatency = 5000000;
	
void DummyObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/){}

// For loading the serial comms device drivers 
#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif

void FormatDateTime(TDes& aBuffer, const TTime& aTime)
	{ 
	aBuffer.Zero();
	_LIT(KFormat4,"%/0%1%/1%2%/2%3%/3 %-B%:0%J%:1%T%:2%S%:3%+B");
	aTime.FormatL(aBuffer,KFormat4); 
	}

CSmtpUtcEntryStep::CSmtpUtcEntryStep()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName(KSmtpUtcEntryStep);
	}
		
CSmtpUtcEntryStep::~CSmtpUtcEntryStep()
	{
	delete iSession;
	delete iObserver;
	delete iTestUtils;
	delete iScheduler;
	delete iTestActive;
	}

TVerdict CSmtpUtcEntryStep::doTestStepPreambleL()
	{
	SetTestStepResult(EPass);
	
	SetTestStepResult(EPass);
	iScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(iScheduler);
	
	iTestUtils = CEmailTestUtils::NewL(test);
	iTestUtils->FileSession().SetSessionPath(_L("C:\\"));
	iTestUtils->CreateAllTestDirectories();
	iTestUtils->CleanMessageFolderL();
	iTestUtils->ClearEmailAccountsL();
	iTestUtils->GoClientSideL();
	
	// Connect to the Message Server...
	iObserver = new (ELeave) DummyObserver();
	iSession = CMsvSession::OpenSyncL(*iObserver);
	
	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	
	iTestActive = new (ELeave) CTestActive();
	
	return TestStepResult();
	}
	
void CSmtpUtcEntryStep::CreateServiceAndAccountL()
	{
	CEmailAccounts* accounts = CEmailAccounts::NewL();
	CleanupStack::PushL(accounts);
	CImSmtpSettings* settings=new(ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();
	
	iSmtpAccountId=accounts->CreateSmtpAccountL(_L("SMTP"),*settings, *smtpIAP, EFalse);
	accounts->SetDefaultSmtpAccountL(iSmtpAccountId);
	//override default settings
	settings->SetServerAddressL(KSmtpServer);
	settings->SetEmailAddressL(KSmtpEmailAddress);
	accounts->SaveSmtpSettingsL(iSmtpAccountId, *settings);
	
	CleanupStack::PopAndDestroy(smtpIAP);
	CleanupStack::PopAndDestroy(settings);
	CleanupStack::PopAndDestroy(accounts);
	}
	
TVerdict CSmtpUtcEntryStep::TestDateL()
	{
	TMsvSelectionOrdering ordering;	
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, iMessageId,ordering);
	CleanupStack::PushL(entry);
	
	//retrive and print the date of the entry	
	TMsvEntry childDetails;
	TBuf<45> dateTimeString;
	
	childDetails=entry->Entry();
	CleanupStack::PopAndDestroy(entry);
	
	dateTimeString.Zero();
	FormatDateTime(dateTimeString,childDetails.iDate); 
	INFO_PRINTF2(_L("%S"),&dateTimeString);
	
	//Get the time difference between the enty timestamp and the currect utc time
	TTime currentUtcTime;
	currentUtcTime.UniversalTime();
	TTimeIntervalMicroSeconds difference=currentUtcTime.MicroSecondsFrom(childDetails.iDate);
	
	//the difference should be less than 5 second
	TVerdict result = (Abs(difference.Int64())<KLatency? EPass:EFail);	
	return result;	
	}
	
	
void CSmtpUtcEntryStep::CreateMessageL()
	{
	//create some body text
	CParaFormatLayer* paraFormatLayer=CParaFormatLayer::NewL();
	CleanupStack::PushL(paraFormatLayer);

	CCharFormatLayer* charFormatLayer=CCharFormatLayer::NewL(); 
	CleanupStack::PushL(charFormatLayer);

	CRichText* bodyText=CRichText::NewL(paraFormatLayer, charFormatLayer, CEditableText::EFlatStorage, 256);
	CleanupStack::PushL(bodyText);

	TBuf<100> bodyContents = _L("Test body");
	bodyText->InsertL(0, bodyContents);
	
	//create the message
	TMsvPartList partList = KMsvMessagePartBody;
	CImEmailOperation* emailOp = CImEmailOperation::CreateNewL(iTestActive->iStatus, *iSession, KMsvGlobalOutBoxIndexEntryIdValue, KMsvMessagePartBody, partList, KUidMsgTypeSMTP); 
	CleanupStack::PushL(emailOp);
		
	// Active object which stops the active scheduler
	iTestActive->StartL(); 
	CActiveScheduler::Start();
		
	// Get the new message id.
	TPckg<TMsvId> param(iMessageId);
	param.Copy(emailOp->FinalProgress());
	CleanupStack::PopAndDestroy(emailOp);
	
	// Save the message body text
	TMsvSelectionOrdering ordering;	
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, iMessageId,ordering);
	CleanupStack::PushL(entry);
	CImEmailMessage* message = CImEmailMessage::NewL(*entry);
	CleanupStack::PushL(message); 
	iTestActive->StartL();
	message->StoreBodyTextL(iMessageId, *bodyText, iTestActive->iStatus);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(message); 
	
	// fill in header details for email message
	CMsvStore* store = 	entry->EditStoreL();
	CleanupStack::PushL(store);
	CImHeader* header = CImHeader::NewLC();
	header->RestoreL(*store);
	header->ToRecipients().AppendL(KSmtpEmailAddress);
	header->SetSubjectL(_L("Message from T_SMTC_UTC !!"));
	header->StoreL(*store);
	store->CommitL();
	
 	CleanupStack::PopAndDestroy(2); //header, store
 	
 	CleanupStack::PopAndDestroy(entry); 	
	CleanupStack::PopAndDestroy(bodyText); 
	CleanupStack::PopAndDestroy(charFormatLayer); 
	CleanupStack::PopAndDestroy(paraFormatLayer); 
	}

void CSmtpUtcEntryStep::SendMessageL()
	{
	//select the message in the outbox, there is only one
	TMsvSelectionOrdering ordering;	
	CMsvEntry* entry = CMsvEntry::NewL(*iSession,KMsvGlobalOutBoxIndexEntryIdValue,ordering);
	CleanupStack::PushL(entry);
    entry->SetEntryL(KMsvGlobalOutBoxIndexEntryIdValue);
    //copy the message to the smtp service
    CMsvOperation* emailOp;
    emailOp = entry->CopyL(iMessageId,iSmtpAccountId.iSmtpService,iTestActive->iStatus);
    CleanupStack::PushL(emailOp);
	// Active object which stops the active scheduler
	iTestActive->StartL();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(emailOp); 
	CleanupStack::PopAndDestroy(entry); 	
	}

TVerdict CSmtpUtcEntryStep::RunTestL()
	{
	//create a message
	CreateMessageL();
	TVerdict result = TestDateL();
	
	if(result==EFail)
		{
		ERR_PRINTF1(_L("Incorrect timestamp in outbox entry."));
		return result;	
		}
	//send the message
	SendMessageL();
	
	result=TestDateL();
	if(result==EFail)
		{
		ERR_PRINTF1(_L("Incorrect timestamp in sent items entry."));
		return result;	
		}
	return result;	
	}
	
TVerdict CSmtpUtcEntryStep::doTestStepL()
	{		
	SetTestStepResult(EPass);
	
	CreateServiceAndAccountL();
	//set the utc offset to + 1 hour
	User::SetUTCOffset(KOneHour);
	TVerdict result=RunTestL();
	if(result==EFail)
		{
		SetTestStepResult(EFail);
		return TestStepResult();	
		}
	
	//set the utc offset to - 1 hour
	User::SetUTCOffset(-KOneHour);
	result=RunTestL();
	if(result==EFail)
		{
		SetTestStepResult(EFail);
		return TestStepResult();	
		}
		
	return TestStepResult();
	}	

TVerdict CSmtpUtcEntryStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


