// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "T_SmsRegress.h"
#include <smscmds.h>
_LIT(KSmsScript, "sms.script");

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

LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;

	smsTest->NotifySaPhoneOnL();
	
	CSmsRegressTest* SmcmTest = CSmsRegressTest::NewLC(*smsTest, KNullDesC, nextTest);

	smsTest->WaitForInitializeL();

	SmcmTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmcmTest. scheduler
	}

CSmsRegressTest::CSmsRegressTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest), iOperations(aSmsTest)
	{
	CActiveScheduler::Add(this);
	}

void CSmsRegressTest::RunAutoL()
	{
	iNextTest = 0;

	iSmsTest.TestStart(++iNextTest, _L("Test2SMSSInstances"));  
	Test2SMSSInstancesL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest);
	}

void CSmsRegressTest::Test2SMSSInstancesL()
	{
	TTime now;
	now.HomeTime();

	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection;
	CleanupStack::PushL(sel);

	iSmsTest(iSmsTest.ReadScriptL(KSmsScript, KMsvGlobalOutBoxIndexEntryId, *sel, now));
	iSmsTest.SetEntryL(KMsvGlobalOutBoxIndexEntryId);

	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();

	CMsvOperation* op = iSmsTest.iMsvEntry->CopyL((*sel)[0], KMsvGlobalOutBoxIndexEntryId, wait->iStatus);
	CleanupStack::PushL(op);
	CActiveScheduler::Start();

	TMsvLocalOperationProgress prog;
	TPckgC<TMsvLocalOperationProgress> pkg(prog);
	pkg.Set(op->ProgressL());
	prog = pkg();
	User::LeaveIfError(prog.iError);

	const TMsvId sms1 = (*sel)[0];
	const TMsvId sms2 = prog.iId;

	CleanupStack::PopAndDestroy(2); //wait, op

	_LIT(KDesc1, "Remote");
	_LIT(KDesc2, "Local");

	iSmsTest.SetEntryL(sms1);
	TMsvEntry entry(iSmsTest.Entry());
	entry.iDescription.Set(KDesc1);
	iSmsTest.ChangeEntryL(entry);
	
	iSmsTest.SetEntryL(sms2);
	entry = iSmsTest.Entry();
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	entry.iDescription.Set(KDesc2);
	iSmsTest.ChangeEntryL(entry);

	iSmsTest.Printf(_L("Test2SMSSInstancesL for 2 messages:\n"));
	iSmsTest.DisplayMessageL(sms1);
	iSmsTest.DisplayMessageL(sms2);

	iOperations.ResetAndDestroy();

	//Start first operation
	COpActive* opActive = new (ELeave) COpActive(iOperations, sms1);
	CleanupStack::PushL(opActive);
	iSmsTest.SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	opActive->iOperation = iSmsTest.iMsvEntry->CopyL(opActive->Id(), iSmsTest.iSmsServiceId, opActive->Status());
	iOperations.AppendL(opActive);
	CleanupStack::Pop(opActive);
	
	//Start second operation
	opActive = new (ELeave) COpActive(iOperations, sms2);
	CleanupStack::PushL(opActive);
	iSmsTest.SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	sel->Reset();
	sel->AppendL(opActive->Id());
	opActive->iOperation = iSmsTest.iMsvSession->TransferCommandL(*sel, ESmsMtmCommandScheduleCopy, KNullDesC8, opActive->Status());
	iOperations.AppendL(opActive);
	CleanupStack::Pop(opActive);

	CleanupStack::PopAndDestroy(sel);

	iState = ETest2SMSSInstances;
	iOperations.Start(iStatus);
	SetActive();
	CActiveScheduler::Start();
	}


CSmsRegressTest::~CSmsRegressTest()
	{
	Cancel();
	delete iWatchers;

	iSocket.Close();
	iSocketServ.Close();
	}

void CSmsRegressTest::ConstructL()
	{
	iSmsTest.SetLogToFile();
	iTimer = CTestTimer::NewL();

	User::LeaveIfError(iSocketServ.Connect());
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(iSocketServ.FindProtocol(protocolname,protoinfo));
	User::LeaveIfError(iSocket.Open(iSocketServ,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));
	
	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(iSocket.Bind(smsaddr));
	}

void CSmsRegressTest::DoCancel()
	{
	iOperations.Cancel();
	iTimer->Cancel();
	}

void CSmsRegressTest::DoRunTest2SMSSInstancesL()
	{
	TInt count = iOperations.Count();
	
	while (count-- && !IsActive())
		{
		const COpActive& active = *iOperations[count];
		iSmsTest.SetEntryL(active.Id());
		const TMsvEntry& entry = iSmsTest.Entry();
		switch (entry.SendingState())
			{
			case KMsvSendStateSent:
			case KMsvSendStateFailed:
				iSmsTest.Printf(_L("%d Complete [SendingState=%d]\n"), entry.Id(), entry.SendingState());
				break; //do nothing
			default:
				iSmsTest.Printf(_L("%d Active [SendingState=%d]\n"), entry.Id(), entry.SendingState());
				iTimer->AfterReq(2000000, iStatus);
				SetActive();
				break;
			}
		}
	}

void CSmsRegressTest::RunL()
	{
	User::LeaveIfError(iStatus.Int());

	switch (iState)
		{
		case ETest2SMSSInstances:
			DoRunTest2SMSSInstancesL();
			break;
		default:
			iSmsTest(EFalse);
			break;
		}

	if (!IsActive())
		CActiveScheduler::Stop();
	}

TInt CSmsRegressTest::RunError(TInt aError)
	{
	iSmsTest.Printf(_L("CSmsRegressTest::RunError [iStatus=%d aError, iState=%d]\n"), iStatus.Int(), aError, iState);

	if (iStatus.Int() == KErrNone)
		iStatus = aError;

	CActiveScheduler::Stop();
	return KErrNone;
	}

CSmsRegressTest* CSmsRegressTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmsRegressTest* self = new (ELeave) CSmsRegressTest(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
	}

void CSmsRegressTest::ShowMenuL()
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Start"));

	TInt result = iSmsTest.DisplayMenu(_L("SMS Client MTM Test"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			RunAutoL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Printf(_L("Press any key to continue...\n"));
		iSmsTest.Test().Getch();
		}

	ShowMenuL();
	}

/*
TBool CSmsRegressTest::CompareSettings(const CSmsSettings& aLeft, const CSmsSettings& aRight, TBool aTestSC)
	{
	TCompare compare(CompareMessageSettings(aLeft, aRight));

	compare(aLeft.CommDbAction() == aRight.CommDbAction());
	compare(aLeft.SmsBearerAction() == aRight.SmsBearerAction());
	compare(aLeft.SmsBearer() == aRight.SmsBearer());
	compare(aLeft.Delivery() == aRight.Delivery());
	compare(CompareBools(aLeft.ReplyQuoted(), aRight.ReplyQuoted()));
	compare(aLeft.SpecialMessageHandling() == aRight.SpecialMessageHandling());
	compare(aLeft.StatusReportHandling() == aRight.StatusReportHandling());

	if (aTestSC)
		{
		compare(aLeft.DefaultSC() == aRight.DefaultSC());
		compare(aLeft.NumSCAddresses() == aRight.NumSCAddresses());

		TInt count = aLeft.NumSCAddresses();

		while (count--)
			{
			compare(CompareNumbers(aLeft.SCAddress(count), aRight.SCAddress(count)));
			}
		}

	return compare;
	}

TBool CSmsRegressTest::CompareMessageSettings(const CSmsMessageSettings& aLeft, const CSmsMessageSettings& aRight)
	{
	TCompare compare(CompareBools(aLeft.CanConcatenate(), aRight.CanConcatenate()));
	compare(aLeft.CharacterSet() == aRight.CharacterSet());
	compare(CompareBools(aLeft.DeliveryReport(), aRight.DeliveryReport()));
	compare(aLeft.MessageConversion() == aRight.MessageConversion());
	compare(CompareBools(aLeft.RejectDuplicate(), aRight.RejectDuplicate()));
	compare(CompareBools(aLeft.ReplyPath(), aRight.ReplyPath()));
	compare(aLeft.ValidityPeriod() == aRight.ValidityPeriod());
	compare(aLeft.ValidityPeriodFormat() == aRight.ValidityPeriodFormat());
	return compare;
	}

TBool CSmsRegressTest::CompareNumbers(const CSmsNumber& aLeft, const CSmsNumber& aRight)
	{
	TCompare compare(aLeft.Name() == aRight.Name());
	compare(aLeft.Address() == aRight.Address());
	compare(aLeft.LogId() == aRight.LogId());
	return compare;
	}
*/
COpActive::COpActive(COpActiveArray& aParent, TMsvId aId, TInt aPriority)
: CActive(aPriority), iParent(aParent), iSmsTest(aParent.iSmsTest), iId(aId)
	{
	CActiveScheduler::Add(this);
	}

COpActive::~COpActive()
	{
	Cancel();
	delete iOperation;
	}

void COpActive::RunL()
	{
	iSmsTest.Printf(_L("COpActive::RunL [iStatus=%d]\n"), iStatus.Int());
	User::LeaveIfError(iStatus.Int());
	iSmsTest.SetProgressL(*iOperation);
	iSmsTest.Printf(_L("COpActive::RunL [Progress.iError=%d]\n"), iSmsTest.iProgress.iError);
	User::LeaveIfError(iSmsTest.iProgress.iError);
	Complete(KErrNone);
	}

void COpActive::DoCancel()
	{
	iOperation->Cancel();
	Complete(KErrCancel);
	}

TInt COpActive::RunError(TInt aError)
	{
	Complete(aError);
	return KErrNone;
	}

void COpActive::Complete(TInt aError)
	{
	iParent.ActiveComplete(*this, aError);
	}

COpActiveArray::COpActiveArray(CSmsTestUtils& aSmsTest)
: CArrayPtrFlat<COpActive>(1), iSmsTest(aSmsTest)
	{
	}

COpActiveArray::~COpActiveArray()
	{
	Cancel();
	ResetAndDestroy();
	}

void COpActiveArray::Cancel()
	{
	TInt count = Count();

	while (count--)
		At(count)->Cancel();
	}

void COpActiveArray::Start(TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	iReport = &aStatus;
	}

void COpActiveArray::ActiveComplete(const CActive& aActive, TInt aError)
	{
	iSmsTest.Printf(_L("COpActiveArray::ActiveComplete [Id=aActive.Id() aError=%d]\n"), &aActive, aError);
	
	if (iReport != NULL)
		{
		if (aError != KErrNone)
			User::RequestComplete(iReport, aError);
		else
			{
			TInt count = Count();
			TBool active = EFalse;
			while (!active && count--)
				active = active || At(count)->IsActive();

			if (!active)
				User::RequestComplete(iReport, KErrNone);
			}
		}
	}
