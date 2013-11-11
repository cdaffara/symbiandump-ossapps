// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_smssendas.h"
#include "smscmds.h"
#include "smuthdr.h"
#include <txtrich.h>
#include <txtfmlyr.h>
#include "smsclnt.h"

RTest test(_L("T_SmsSendAs Testrig"));
CTrapCleanup* theCleanup;

_LIT(KSmsScript, "sms.script");
_LIT(KWapScript, "wap.script");
_LIT(KNbsScript, "nbs.script");

LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;

	smsTest->NotifySaPhoneOnL();

	CSmsSendAsTest* asTest = CSmsSendAsTest::NewL(*smsTest, KNullDesC, nextTest);
	CleanupStack::PushL(asTest);

	smsTest->WaitForInitializeL();

	asTest->StartL();

	//CleanupStack::PopAndDestroy(3); //SmsTest, scheduler
	CleanupStack::PopAndDestroy(asTest);
	CleanupStack::PopAndDestroy(smsTest);
	CleanupStack::PopAndDestroy(scheduler);
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("Setup"));
	theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}


CSmsSendAsTest::~CSmsSendAsTest()
/**
	destructor
 */
	{
	iSocket.Close();
	iSocketServ.Close();
	}

CSmsSendAsTest* CSmsSendAsTest::NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmsSendAsTest* self = new (ELeave) CSmsSendAsTest(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();
	return self;
	}



CSmsSendAsTest::CSmsSendAsTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest), iState(EStateWaiting)
	{
	}

void CSmsSendAsTest::RunAutoL()
/**
	Runs tests in sequence, checks iStatus
 */
	{
	iSmsTest.TestStart(++iNextTest, _L("Test SendAs"));
	TestSendAsL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	
	iSmsTest.TestStart(++iNextTest, _L("Send the messages"));
	TestSendingL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	}

void CSmsSendAsTest::ConstructL()
	{
	iSmsTest.SetLogToFile();
	iSmsTest.DeleteServicesL();
	iSmsTest.CreateServicesL();

	iSelection = new (ELeave) CMsvEntrySelection();
	iTimer = CTestTimer::NewL();

	User::LeaveIfError(iSocketServ.Connect());
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(iSocketServ.FindProtocol(protocolname,protoinfo));
	User::LeaveIfError(iSocket.Open(iSocketServ,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));

	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(iSocket.Bind(smsaddr));

	CActiveScheduler::Add(this);
	}

void CSmsSendAsTest::ShowMenuL()
/**
	Shows menu
 */
	{
	iSmsTest.ResetMenu();
	iSmsTest.AppendToMenuL(_L("Test SendAs"));
	iSmsTest.AppendToMenuL(_L("Send the messages"));
	iSmsTest.AppendToMenuL(_L("Run auto"));

	TInt result = iSmsTest.DisplayMenu(_L("SMS Service Centre Test"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			TestSendAsL();
			break;
		case 2:
			TestSendingL();
			break;
		case 3:
			iSmsTest.SetRunAuto(ETrue);
			RunAutoL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

	ShowMenuL();
	}

void CSmsSendAsTest::RunL()
/**
	Handles completed async operations
 */
	{
	CActiveScheduler::Stop();

	switch (iState)
		{
		case EStateSending:
			{
			DoRunSendingL();
			break;
			}
		default:
			{
			break;
			}
		}
	}

void CSmsSendAsTest::DoRunSendingL()
/**
	Checks the status
 */	
	{
	if (iOperation)
		iSmsTest.SetProgressL(*iOperation);

	if (iStatus == KErrNone)
		iStatus = iSmsTest.iProgress.iError;

	iSmsTest.Printf(_L("Printing completed with error %d\n"), iStatus.Int());

	iSmsTest.DisplaySendingStatesL(*iSelection);
	iState = EStateWaiting;

	delete iOperation;
	iOperation = NULL;
	}

void CSmsSendAsTest::DoCancel()
/**
	Cancels the async operation
 */
	{
	if (iOperation)
		{
		iOperation->Cancel();
		iSmsTest.Printf(_L("Operation Cancelled!\n"));
		}
	delete iOperation;
	iOperation = NULL;
	}

void CSmsSendAsTest::TestSendAsL()
/**
	Tests SendAs  
 */
	{
	DeleteChildrenL(KMsvGlobalOutBoxIndexEntryId);

	TTime now;
	now.HomeTime();

	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(sel);
	iSelection->Reset();
	
	iSmsTest(iSmsTest.ReadScriptL(KSmsScript, KMsvGlobalOutBoxIndexEntryId, *sel, now));
	iSmsTest(iSmsTest.ReadScriptL(KWapScript, KMsvGlobalOutBoxIndexEntryId, *sel, now));
	iSmsTest(iSmsTest.ReadScriptL(KNbsScript, KMsvGlobalOutBoxIndexEntryId, *sel, now));
	const TInt count = sel->Count();

	iSmsTest(count);

	CRichText* richOld = CRichText::NewL(iSmsTest.iParaFormat, iSmsTest.iCharFormat);
	CleanupStack::PushL(richOld);

	CRichText* richNew = CRichText::NewL(iSmsTest.iParaFormat, iSmsTest.iCharFormat);
	CleanupStack::PushL(richNew);

	CSmsHeader* headerOld = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *richOld);
	CleanupStack::PushL(headerOld);

	CSmsHeader* headerNew = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *richNew);
	CleanupStack::PushL(headerNew);

	for (TInt i = 0; i < count; i++) //order important
		{
		//Get the existing message
		TMsvId id = sel->At(i);
		iSmsTest.SetEntryL(id);

		TUid bioType;
		bioType.iUid = iSmsTest.Entry().iBioType;

			iSmsTest.Printf(_L("%d/%d SendAs for BioType %d\n"), i + 1, count, bioType.iUid); 

			CMsvStore* store = iSmsTest.ReadStoreL();
			CleanupStack::PushL(store);
			headerOld->RestoreL(*store);
			store->RestoreBodyTextL(*richOld);
			CleanupStack::PopAndDestroy(store);
			
			CSendAs* sendAs = CSendAs::NewL(*this);
			CleanupStack::PushL(sendAs);
			
			sendAs->SetMtmL(KUidMsgTypeSMS);
			iSmsTest(sendAs->AvailableServices().Count());
			sendAs->SetService(0);
			sendAs->CreateMessageL(KMsvGlobalOutBoxIndexEntryId);

			for (TInt j = 0; j < headerOld->Recipients().Count(); j++)
				{
				CSmsNumber* number = headerOld->Recipients()[j];
				TPtrC name(number->Name());
				TPtrC addr(number->Address());
				sendAs->AddRecipientL(addr, name);
				}

			sendAs->SetBodyL(*richOld);

			if (bioType.iUid != 0)
				sendAs->SetBioTypeL(bioType);

			sendAs->SaveMessageL(ETrue);

			//Get the new message
			CSmsClientMtm* mtm = REINTERPRET_CAST(CSmsClientMtm*, &sendAs->ClientMtm());
			CMsvEntry& entry = mtm->Entry();
			store = entry.ReadStoreL();
			CleanupStack::PushL(store);
			headerNew->RestoreL(*store);
			store->RestoreBodyTextL(*richNew);
			CleanupStack::PopAndDestroy(store);

			iSmsTest.Printf(_L("\tCreated %d\n"), entry.EntryId());
			test(id != entry.EntryId());
			CompareHeadersL(*headerOld, *headerNew);

			CSmsSettings* msgSettings = CSmsSettings::NewL();
			CleanupStack::PushL(msgSettings);

			//Check the settings are the same (except charset)
			headerNew->GetSmsSettingsL(*msgSettings);
			CompareSettingsL(mtm->ServiceSettings(), *msgSettings);
			CleanupStack::PopAndDestroy(msgSettings);

			iSelection->AppendL(entry.EntryId());

			CleanupStack::PopAndDestroy(sendAs);
			}

	CleanupStack::PopAndDestroy(5);
	}

void CSmsSendAsTest::CompareSettingsL(const CSmsMessageSettings& aS1, const CSmsMessageSettings& aS2)
/**
	Compares two sms message settings 

	@param		aS1			A sms message setting
	@param		aS2			A sms message setting	
 */
	{
	RTest& t = iSmsTest.Test();
	t(aS1.ValidityPeriod() == aS2.ValidityPeriod());
	t(aS1.ValidityPeriodFormat() == aS2.ValidityPeriodFormat());
	t(aS1.RejectDuplicate() == aS2.RejectDuplicate());
	t(aS1.DeliveryReport() == aS2.DeliveryReport());
	t(aS1.ReplyPath() == aS2.ReplyPath());
	t(aS1.MessageConversion() == aS2.MessageConversion());
	}

void CSmsSendAsTest::CompareHeadersL(const CSmsHeader& aH1, const CSmsHeader& aH2) const
/**
	Compares two sms headers

	@param		aH1			A sms message header
	@param		aH2			A sms message header	
 */
	{
	RTest& t = iSmsTest.Test();
	t(aH1.Type() == aH2.Type());

	t(aH1.Recipients().Count() == aH2.Recipients().Count());

	for (TInt i = 0; i < aH1.Recipients().Count(); i++)
		{
		CSmsNumber* number1 = aH1.Recipients()[i];
		CSmsNumber* number2 = aH2.Recipients()[i];

		t(number1->Address().Compare(number2->Address()) == 0);
		t(number1->Name().Compare(number2->Name()) == 0);
		}

	t(aH1.FromAddress().CompareF(aH2.FromAddress()) == 0);

	TInt length = aH1.Message().Buffer().Length();
	HBufC* hBuf1 = HBufC::NewLC(length);
	TPtr buf1 = hBuf1->Des();
	aH1.Message().Buffer().Extract(buf1, 0, length);
	
	length = aH2.Message().Buffer().Length();
	HBufC* hBuf2 = HBufC::NewLC(length);
	TPtr buf2 = hBuf2->Des();
	aH2.Message().Buffer().Extract(buf2, 0, length);

	t(buf1.Compare(buf2) == 0);

	CleanupStack::PopAndDestroy(2); //hBuf1, hBuf2
	}

void CSmsSendAsTest::TestSendingL()
/**
	Tests sending
 */
	{
	iSmsTest.Test().Next(_L("Test Sending"));

	if (!iSelection->Count())
		User::Leave(KErrNotFound);

	delete iOperation;
	iOperation = NULL;
	iState = EStateSending;

	MsvEntry().SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iOperation = MsvEntry().CopyL(*iSelection, iSmsTest.iSmsServiceId, iStatus);
	SetActive();
	CActiveScheduler::Start();
	}


void CSmsSendAsTest::DeleteChildrenL(TMsvId aParent)
/**
	Deletes children

	@param		aParent			A Message Id
 */
	{
	iSmsTest.SetEntryL(aParent);
	CMsvEntrySelection* sel = iSmsTest.ChildrenWithMtmLC(KUidMsgTypeSMS);
	TInt count = sel->Count();

	while (count--)
		{
		iSmsTest.DeleteEntryL(sel->At(count));
		}

	CleanupStack::PopAndDestroy(sel);
	}

TBool CSmsSendAsTest::CapabilityOK(TUid /*aCapability*/, TBool /*aResponse*/)
/**
	This is required by MSendAsObserver
 */
	{
	return ETrue;
	}

