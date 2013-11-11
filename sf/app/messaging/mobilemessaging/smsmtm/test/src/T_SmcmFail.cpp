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

#include "T_SmcmFail.h"
#include <biodb.h>

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

	CSmcmFailTest* SmcmTest = CSmcmFailTest::NewL(*smsTest, KNullDesC, nextTest);
	CleanupStack::PushL(SmcmTest);

	SmcmTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmcmTest. scheduler
	}

CSmcmFailTest::CSmcmFailTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
: CSmsTestBase(aSmsTest, aScriptFile, aCurrentTest)
	{
	}

void CSmcmFailTest::RunAutoL()
	{
	iSmsTest.TestStart(++iNextTest, _L("CSmsSettings Fail"));
	TestSettingsFailL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsNumber Fail"));
	TestNumberFailL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsHeader Fail"));
	TestHeaderFailL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("Load Fail"));
	TestLoadFailL(); //Cannot be done 'cause of defect in message server
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("Save Fail"));
	TestSaveFailL(); //Cannot be done 'cause of defect in message server
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("Addressee Fail"));
	TestAddresseeFailL(); //Cannot be done 'cause of defect in message server
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("Bio Fail"));
	this->TestBifFailL();
	iSmsTest.TestFinish(iNextTest);


	iSmsTest.TestStart(++iNextTest, _L("Reply Fail"));
	TestReplyFailL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("Forward Fail"));
	TestForwardFailL(); //Cannot be done 'cause of defect in message server
	iSmsTest.TestFinish(iNextTest);
	}

CSmcmFailTest::~CSmcmFailTest()
	{
	delete iRichText;
	delete iParaLayer;
	delete iCharLayer;
	}

void CSmcmFailTest::ConstructL()
	{
	iSmsTest.SetLogToFile();
	iSmsTest.InstantiateClientMtmsL();

	iSelection = new (ELeave) CMsvEntrySelection();
	iTimer = CTestTimer::NewL();

	iParaLayer =CParaFormatLayer::NewL();
	iCharLayer = CCharFormatLayer::NewL();
	iRichText = CRichText::NewL(iParaLayer, iCharLayer, CEditableText::EFlatStorage,256);

	CActiveScheduler::Add(this);
	}

CSmcmFailTest* CSmcmFailTest::NewL(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aCurrentTest)
	{
	CSmcmFailTest* self = new (ELeave) CSmcmFailTest(aSmsTest, aScriptFile, aCurrentTest);
	CleanupStack::PushL(self);

	self->ConstructL();

	CleanupStack::Pop();
	return self;
	}

void CSmcmFailTest::ShowMenuL()
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
		test.Getch();
		}

	ShowMenuL();
	}

void CSmcmFailTest::TestSettingsFailL()
	{
	CSmsSettings* settings = NULL;

	TInt error;
	TInt failCount = 0;

	//Failure Test CSmsSettings::NewL
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, settings = CSmsSettings::NewL());

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}

		}
	while (error);

	iSmsTest.Printf(_L("CSmsSettings::NewL Fail Count %d\n"), failCount);
	CleanupStack::PushL(settings);

	failCount = 0;

	//Failure Test CSmsSettings::AddServiceCenterL()
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TInt scCount = settings->ServiceCenterCount();
		TRAP(error, settings->AddServiceCenterL(_L("Anthony"), _L("+44")));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			test(scCount == settings->ServiceCenterCount());
			}
		else
			{
			test(scCount == settings->ServiceCenterCount() - 1);
			}
		}
	while (error);

	iSmsTest.Printf(_L("CSmsSettings::AddSCAddress Fail Count %d\n"), failCount);
	CSmsSettings* settings2 = CSmsSettings::NewL();
	CleanupStack::PushL(settings2);
	failCount = 0;

	//Failure Test CSmsSettings::CopyL()
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, settings2->CopyL(*settings));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}
		}
	while (error);

	iSmsTest.Printf(_L("CSmsSettings::CopyL Fail Count %d\n"), failCount);

	CleanupStack::PopAndDestroy(); //settings2
	CleanupStack::PopAndDestroy(); //settings
	}

void CSmcmFailTest::TestHeaderFailL()
	{
	CSmsHeader* header = NULL;

	TInt error;
	TInt failCount = 0;

	//Failure Test CSmsHeader::NewL, Deliver
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver,*iRichText));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}

		}
	while (error);

	iSmsTest.Printf(_L("CSmsHeader::NewL (Deliver) Fail Count %d\n"), failCount);
	delete header;

	failCount = 0;

	//Failure Test CSmsHeader::NewL, StatusReport
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, header = CSmsHeader::NewL(CSmsPDU::ESmsStatusReport,*iRichText));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}
		}
	while (error);

	iSmsTest.Printf(_L("CSmsHeader::NewL (StatusReport) Fail Count %d\n"), failCount);
	delete header;

	failCount = 0;

	//Failure Test CSmsHeader::NewL, Submit
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit,*iRichText));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}

		}
	while (error);

	iSmsTest.Printf(_L("CSmsHeader::NewL (Submit) Fail Count %d\n"), failCount);
	delete header;
	}

void CSmcmFailTest::TestNumberFailL()
	{
	CSmsNumber* number = NULL;

	TInt error;
	TInt failCount = 0;

	//Failure Test CSmsNumber::NewL
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, number = CSmsNumber::NewL());

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}

		}
	while (error);

	iSmsTest.Printf(_L("CSmsNumber::NewL Fail Count %d\n"), failCount);
	CleanupStack::PushL(number);
	CSmsNumber* number2 = NULL;

	failCount = 0;

	//Failure Test CSmsNumber::NewL,2
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, number2 = CSmsNumber::NewL(*number));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}

		}
	while (error);

	iSmsTest.Printf(_L("CSmsNumber::NewL (Number) Fail Count %d\n"), failCount);
	CleanupStack::PushL(number2);

	number->SetAddressL(_L("44"));
	number->SetNameL(_L("AA"));

	failCount = 0;

	//Failure Test CSmsNumber::CopyL
	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, number2->CopyL(*number));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}
		else
			{
			test(number->Name() == number2->Name());
			test(number->Address() == number2->Address());
			}
		}
	while (error);

	iSmsTest.Printf(_L("CSmsNumber::CopyL Fail Count %d\n"), failCount);
	CleanupStack::PopAndDestroy(); //number
	CleanupStack::PopAndDestroy(); //number2
	}


void CSmcmFailTest::TestReplyFailL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Reply Fail"));

	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	CMsvOperation* op = NULL; 
	TInt error;
	TMsvPartList part=0;
	TInt failCount = 0;

	do
		{
		CleanFolderL(KMsvDraftEntryId);
		CleanFolderL(KMsvGlobalOutBoxIndexEntryId);
		CleanFolderL(KMsvGlobalInBoxIndexEntryId);

		TMsvId id = iSmsTest.CreateDummyMessageToReceiveL();
		Session().CleanupEntryPushL(id);
		Client().SwitchCurrentEntryL(id); // Lets go to the received area
		Client().LoadMessageL();
	
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, op = Client().ReplyL(KMsvDraftEntryId, part, wait->iStatus));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);

			if (failCount % 100 == 0)
				iSmsTest.Printf(_L("CSmsClientMtm::ReplyL still failing, count = %d\n"), failCount);
			}

		CleanupStack::PopAndDestroy(); //id
		}
	while (error);
	
	iSmsTest.Printf(_L("CSmsClientMtm::ReplyL Fail Count %d\n"), failCount);
	CleanupStack::PushL(op);
	wait->Start();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2); //op, wait
	}

void CSmcmFailTest::CleanFolderL(TMsvId aFolder)
	{
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	CMsvOperation* op = NULL; 

	iSmsTest.SetEntryL(aFolder);
	CMsvEntrySelection* sel = MsvEntry().ChildrenL();

	CleanupStack::PushL(sel);
	if (sel->Count())
		{
		iSmsTest.Printf(_L("Deleting %d existing messages\n"), sel->Count());
		op = MsvEntry().DeleteL(*sel, wait->iStatus);
		CleanupStack::PushL(op);
		wait->Start();
		CActiveScheduler::Start();
		CleanupStack::PopAndDestroy(); //op
		}
	CleanupStack::PopAndDestroy(2); //sel, wait
	}

void CSmcmFailTest::TestForwardFailL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Forward Fail"));

	CleanFolderL(KMsvDraftEntryId);
	CleanFolderL(KMsvGlobalOutBoxIndexEntryId);
	CleanFolderL(KMsvGlobalInBoxIndexEntryId);

	TMsvId id = iSmsTest.CreateDummyMessageToReceiveL();
	Client().SwitchCurrentEntryL(id);
	Client().LoadMessageL();

	TMsvPartList part=0;

	TInt error;
	TInt failCount = 0;
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	CMsvOperation* op = NULL; 

	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, op = Client().ForwardL(KMsvDraftEntryId, part, wait->iStatus));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);

			if (failCount % 100 == 0)
				iSmsTest.Printf(_L("CSmsClientMtm::ForwardL still failing, count = %d\n"), failCount);
			}

		}
	while (error);

	CleanupStack::PushL(op);
	iSmsTest.Printf(_L("CSmsClientMtm::ForwardL Fail Count %d\n"), failCount);
	wait->Start();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(2); //op, wait
	}

void CSmcmFailTest::TestLoadFailL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Load Fail"));

	CleanFolderL(KMsvDraftEntryId);
	CleanFolderL(KMsvGlobalOutBoxIndexEntryId);
	CleanFolderL(KMsvGlobalInBoxIndexEntryId);

	TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	Client().SwitchCurrentEntryL(id);
	
	TInt error;
	TInt failCount = 0;

	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, Client().LoadMessageL());

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}
		}
	while (error);

	iSmsTest.Printf(_L("CSmsClientMtm::LoadMessageL Fail Count %d\n"), failCount);
	}

void CSmcmFailTest::TestSaveFailL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Save Fail"));

	CleanFolderL(KMsvDraftEntryId);
	CleanFolderL(KMsvGlobalOutBoxIndexEntryId);
	CleanFolderL(KMsvGlobalInBoxIndexEntryId);

	TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	Client().SwitchCurrentEntryL(id);
	Client().LoadMessageL();
	
	TInt error;
	TInt failCount = 0;

	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, Client().SaveMessageL());

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			}
		}
	while (error);

	iSmsTest.Printf(_L("CSmsClientMtm::SaveMessageL Fail Count %d\n"), failCount);
	}

void CSmcmFailTest::TestAddresseeFailL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Save Fail"));

	CleanFolderL(KMsvDraftEntryId);
	CleanFolderL(KMsvGlobalOutBoxIndexEntryId);
	CleanFolderL(KMsvGlobalInBoxIndexEntryId);

	TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	Client().SwitchCurrentEntryL(id);
	Client().LoadMessageL();
	
	TInt error;
	TInt failCount = 0;

	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TInt count = Client().AddresseeList().Count();
		TRAP(error, Client().AddAddresseeL(_L("44")));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			test(count == Client().AddresseeList().Count());
			}
		else
			{
			test(count == Client().AddresseeList().Count() - 1);
			}
		}
	while (error);

	iSmsTest.Printf(_L("CSmsClientMtm Addressee1 Fail Count %d\n"), failCount);

	failCount = 0;

	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TInt count = Client().AddresseeList().Count();
		TRAP(error, Client().AddAddresseeL(_L("44"), _L("AA")));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory);
			test(count == Client().AddresseeList().Count());
			}
		else
			{
			test(count == Client().AddresseeList().Count() - 1);
			}
		}
	while (error);

	iSmsTest.Printf(_L("CSmsClientMtm Addressee2 Fail Count %d\n"), failCount);
	}

void CSmcmFailTest::TestBifFailL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Bio Fail"));

	CleanFolderL(KMsvDraftEntryId);
	CleanFolderL(KMsvGlobalOutBoxIndexEntryId);
	CleanFolderL(KMsvGlobalInBoxIndexEntryId);

	TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	Client().SwitchCurrentEntryL(id);
	Client().LoadMessageL();
	
	TInt error;
	TInt failCount = 0;

	CBIODatabase* bdb = CBIODatabase::NewLC(Session().FileSession());

	if (!bdb->BIOCount())
		{
		CleanupStack::PopAndDestroy(); //bdb
		iSmsTest.Printf(_L("No Bio messages in Bio DB\n"));
		return;
		}

	// Get the whole bif file class
	const CBioInfoFileReader& reader = bdb->BifReader(0);

	do
		{
		__UHEAP_FAILNEXT(failCount++);

		TRAP(error, Client().BioTypeChangedL(reader.MessageTypeUid()));

		__UHEAP_RESET;

		if (error)
			{
			test(error == KErrNoMemory || error == KErrNotFound);
			}
		}
	while (error && error != KErrNotFound);

	iSmsTest.Printf(_L("CSmsClientMtm::BioTypeChangedL Fail Count %d\n"), failCount);
	CleanupStack::PopAndDestroy(); //bdb
	}
