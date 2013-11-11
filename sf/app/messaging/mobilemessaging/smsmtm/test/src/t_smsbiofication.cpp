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

#include <e32test.h>
#include <e32uid.h>
#include <e32std.h>
#include <smuthdr.h>
#include <smscmds.h>
#include "t_smsbiofication.h"
#include <txtrich.h>
#include <biouids.h>

RTest test(_L("T_SmsBiofication Testrig"));
CTrapCleanup* theCleanup;

_LIT(KBioficationScript, "biofication.script");
_LIT(KSmsTestFolderDetails, "Test folder");
_LIT(KSmsTestFolderDescription, "Copied or moved message folder");


CSmsBioficationTest* CSmsBioficationTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmsBioficationTest* self = new (ELeave) CSmsBioficationTest(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();
	return self;
	}

CSmsBioficationTest::~CSmsBioficationTest()
	{
	delete iTestSelection;

	iSocket.Close();
	iSocketServ.Close();
	}

CSmsBioficationTest::CSmsBioficationTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest), iState(EStateIdle)
	{
	}

void CSmsBioficationTest::ConstructL()
	{
	SetTestNameL(KSmsBioficationTestName);
	iSmsTest.SetLogToFile();

	iSelection = new (ELeave) CMsvEntrySelection();
	iTestSelection = new (ELeave) CMsvEntrySelection();
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

void CSmsBioficationTest::RunL()
	{
	CActiveScheduler::Stop();

	iSmsTest(iStatus.Int() == KErrNone);

	iSmsTest.SetProgressL(*iOperation);

	switch (iState)
		{
		case EStateEnumerating:
			{
			RunEnumerateL();
			break;
			}
		case EStateCopyingToPhoneStore:
			{
			RunCopyToPhoneStoreL();
			break;
			}
		case EStateCopyingFromPhoneStore:
		case EStateMovingFromPhoneStore:
		case EStateDeletingFromPhoneStore:
			{
			RunCopyMoveDeleteFromPhoneStoreL();
			break;
			}
		case EStateSending:
			{
			ChangeMtmUidsL(*iSelection);
			break;
			}
		default:
			break;
		}
	iState=EStateIdle;
	}

void CSmsBioficationTest::DoCancel()
	{
	if (iOperation)
		{
		iOperation->Cancel();
		iSmsTest.Printf(_L("Operation Cancelled!\n"));
		}
	else
		{
		iSmsTest.Printf(_L("No operation to cancel!\n"));
		}

	delete iOperation;
	iOperation = NULL;
	}


void CSmsBioficationTest::TestSetupL()
	{
	iSmsTest.Test().Next(_L("Test Setup"));
	iState=EStateSettingUp;
	TFullName name;
	TFindProcess find(_L("watcher*"));
	while (find.Next(name)==KErrNone)
		{
		RProcess process;
		process.Open(find);
		process.Terminate(KErrGeneral);
		process.Close();
		}
	//  Create test folder
	MsvEntry().SetEntryL(KMsvDraftEntryId);
	TMsvEntry entry;
	entry.iServiceId = iSmsTest.iSmsServiceId;
	entry.SetVisible(ETrue);
	entry.iMtm = KUidMsgTypeSMS;
	entry.iDate.HomeTime();
	entry.iType = KUidMsvFolderEntry;
	entry.iDetails.Set(KSmsTestFolderDetails);
	entry.iDescription.Set(KSmsTestFolderDescription);
	MsvEntry().CreateL(entry);
	iTestFolderId=MsvEntry().EntryId();

	TTime now;
	now.HomeTime();
	iSmsTest.ReadScriptL(iScriptFile, KMsvNullIndexEntryId, *iSelection, now);
	ChangeMtmUidsL(*iSelection);

	iState=EStateIdle;

	/*
	User::LeaveIfError(iSocketServ.Connect());
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(iSocketServ.FindProtocol(protocolname,protoinfo));

	User::LeaveIfError(iSocket.Open(iSocketServ,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));
	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(iSocket.Bind(smsaddr));
	*/
	}

void CSmsBioficationTest::TestEnumerateL()
	{
	iSmsTest.Test().Next(_L("Enumerate Phone Stores"));
	iState = EStateEnumerating;

	iTestSelection->Reset();
	iTestSelection->AppendL(iSmsTest.iSmsServiceId);

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iTestSelection, ESmsMtmCommandEnumeratePhoneStores, TPtrC8(), iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsBioficationTest::TestCopyToPhoneStoreL()
	{
	iSmsTest.Test().Next(_L("Copy To Phone Store"));
	iState = EStateCopyingToPhoneStore;

	delete iTestSelection;
	iTestSelection=iSelection->CopyL();
	iTestSelection->InsertL(0,iSmsTest.iSmsServiceId);

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iTestSelection, ESmsMtmCommandCopyToPhoneStore, TPtrC8(), iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsBioficationTest::TestCopyFromPhoneStoreL()
	{
	iSmsTest.Test().Next(_L("Copy From Phone Store"));
	iState = EStateCopyingFromPhoneStore;

	DeleteEntriesFromTestFolderL();

	MsvEntry().SetEntryL(iEnumerationFolderId);

	delete iTestSelection;
	iTestSelection = MsvEntry().ChildrenL();
	iTestSelection->InsertL(0,iSmsTest.iSmsServiceId);

	TPckgBuf<TMsvId> pkg(iTestFolderId);

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iTestSelection, ESmsMtmCommandCopyFromPhoneStore, pkg, iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsBioficationTest::TestMoveFromPhoneStoreL()
	{
	iSmsTest.Test().Next(_L("Move From Phone Store"));
	iState = EStateMovingFromPhoneStore;

	DeleteEntriesFromTestFolderL();

	MsvEntry().SetEntryL(iEnumerationFolderId);
	delete iTestSelection;
	iTestSelection = MsvEntry().ChildrenL();
	iTestSelection->InsertL(0,iSmsTest.iSmsServiceId);

	TPckgBuf<TMsvId> pkg(iTestFolderId);

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iTestSelection, ESmsMtmCommandMoveFromPhoneStore, pkg, iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsBioficationTest::TestDeleteFromPhoneStoreL()
	{
	iSmsTest.Test().Next(_L("Delete From SIM"));
	iState = EStateDeletingFromPhoneStore;

	MsvEntry().SetEntryL(iEnumerationFolderId);
	delete iTestSelection;
	iTestSelection = MsvEntry().ChildrenL();
	iTestSelection->InsertL(0,iSmsTest.iSmsServiceId);

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iTestSelection, ESmsMtmCommandDeleteFromPhoneStore, TPtrC8(), iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsBioficationTest::TestSendL()
	{
	iSmsTest.Test().Next(_L("Send"));
	iState = EStateSending;

	iSmsTest.Test().Next(_L("Test Send Entry\n"));

	DeleteEntriesFromTestFolderL();

	for (TInt i=iSelection->Count()-1; i>=0; i--)
		{
		MsvEntry().SetEntryL(iSelection->At(i));
		TMsvEntry entry=MsvEntry().Entry();
		if (entry.Parent()!=KMsvGlobalOutBoxIndexEntryId)
			iSelection->Delete(i);
		else
			{
			entry.iMtm=KUidMsgTypeSMS;
			MsvEntry().ChangeL(entry);
			MsvEntry().SetEntryL(KMsvGlobalOutBoxIndexEntryId);
			MsvEntry().CopyL(iSelection->At(i),iTestFolderId);
			}
		}

	MsvEntry().SetEntryL(iTestFolderId);
	delete iTestSelection;
	iTestSelection = MsvEntry().ChildrenL();

	delete iOperation;
	iOperation = NULL;
	iOperation = MsvEntry().CopyL(*iTestSelection, iSmsTest.iSmsServiceId, iStatus);

	SetActive();
	CActiveScheduler::Start();
	}

void CSmsBioficationTest::TestDeleteEnumerationFolderL()
	{
	iSmsTest.Test().Next(_L("Test Delete Enumeration Folder\n"));

	iTestSelection->Reset();
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();

	iSmsTest.SetEntryL(iEnumerationFolderId);
	iSmsTest.SetEntryL(iSmsTest.Entry().Parent());

	CMsvOperation* op = iSmsTest.iMsvEntry->DeleteL(iEnumerationFolderId, wait->iStatus);
	CleanupStack::PushL(op);

	CActiveScheduler::Start();

	iSmsTest.Printf(_L("Remote DeleteL() completed with %d\n"), wait->iStatus.Int());

	User::LeaveIfError(wait->iStatus.Int());

	iSmsTest.SetProgressL(*op);

	iSmsTest.Printf(_L("Operation completed with %d, msg count %d\n"), iSmsTest.iProgress.iError, iSmsTest.iProgress.iMsgCount);
	User::LeaveIfError(iSmsTest.iProgress.iError);

	CleanupStack::PopAndDestroy(2); //wait, op
	iState = EStateIdle;
	}


void CSmsBioficationTest::RunAutoL()
	{
	iSmsTest.SetLogToConsole(ETrue);

	iSmsTest.TestStart(++iNextTest, _L("Test 1 - Setup"));
	TestSetupL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	

	iSmsTest.TestStart(++iNextTest, _L("Test 1 - Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	

	if (iTestSelection->Count())
		{
		iSmsTest.TestStart(++iNextTest, _L("Test 1 - Delete From Phone Store"));
		TestDeleteFromPhoneStoreL();
		User::LeaveIfError(iStatus.Int());
		iSmsTest.TestFinish(iNextTest, KErrNone);
		}

	

	iSmsTest.TestStart(++iNextTest, _L("Test 1 - Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	test(iTestSelection->Count()==0);
	iSmsTest.TestFinish(iNextTest, KErrNone);

	const TInt KWaitAfterEnum = 10;
	const TInt KWaitAfterSend = 10;
	iSmsTest.Printf(_L("Waiting %d seconds after enum...\n"), KWaitAfterEnum);
	User::After(KWaitAfterEnum * 1000000);
	iSmsTest.Printf(_L("Finished waiting\n"));

	

	iSmsTest.TestStart(++iNextTest, _L("Test 2 - Copy to Phone Store"));
	TestCopyToPhoneStoreL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	


	iSmsTest.TestStart(++iNextTest, _L("Test 2 - Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	test(CompareSelectionsL(*iSelection,*iTestSelection));
	iSmsTest.TestFinish(iNextTest, KErrNone);

	

	iSmsTest.TestStart(++iNextTest, _L("Test 2 - Copy From Phone Store"));
	TestCopyFromPhoneStoreL();
	User::LeaveIfError(iStatus.Int());
	test(CompareSelectionsL(*iSelection,*iTestSelection));
	iSmsTest.TestFinish(iNextTest, KErrNone);

	

	iSmsTest.TestStart(++iNextTest, _L("Test 2 - Delete From Phone Store"));
	TestDeleteFromPhoneStoreL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	

	iSmsTest.TestStart(++iNextTest, _L("Test 2 - Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	test(iTestSelection->Count()==0);
	iSmsTest.TestFinish(iNextTest, KErrNone);

	

	iSmsTest.TestStart(++iNextTest, _L("Test 3 - Send"));
	TestSendL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.Printf(_L("Waiting %d seconds after send...\n"), KWaitAfterSend);
	User::After(KWaitAfterSend * 1000000);
	iSmsTest.Printf(_L("Finished waiting\n"));

	iSmsTest.TestStart(++iNextTest, _L("Test 2 - Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	test(CompareSelectionsL(*iSelection,*iTestSelection));
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Test 3 - Move from Phone Store"));
	TestMoveFromPhoneStoreL();
	User::LeaveIfError(iStatus.Int());
	test(CompareSelectionsL(*iSelection,*iTestSelection));
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Test 2 - Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	test(iTestSelection->Count()==0);
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Test 2 - Delete Enumeration Folder"));
	TestDeleteEnumerationFolderL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	}

void CSmsBioficationTest::ShowMenuL()
	{
	}

void CSmsBioficationTest::DeleteEntriesFromTestFolderL()
	{
	MsvEntry().SetEntryL(iTestFolderId);
	delete iTestSelection;
	iTestSelection = MsvEntry().ChildrenL();
	if (iTestSelection->Count())
		{
		CMsvOperationWait* wait = CMsvOperationWait::NewLC();
		wait->Start();
		CMsvOperation* op=MsvEntry().DeleteL(*iTestSelection,wait->iStatus);
		CleanupStack::PushL(op);
		CActiveScheduler::Start();
		User::LeaveIfError(wait->iStatus.Int());
		iSmsTest.SetProgressL(*op);
		User::LeaveIfError(iSmsTest.iProgress.iError);
		CleanupStack::PopAndDestroy(2); //wait, op
		iTestSelection->Reset();
		}
	}

void CSmsBioficationTest::ChangeMtmUidsL(CMsvEntrySelection& aSelection)
	{
	for (TInt i=0; i<aSelection.Count(); i++)
		{
		MsvEntry().SetEntryL(aSelection[i]);
		TMsvEntry entry=MsvEntry().Entry();
		if (entry.iBioType!=EBioMsgIdUnknown)
			{
			entry.iMtm=KUidBIOMessageTypeMtm;  //  The test utils do not set the correct MTM uid for BIO messages
			MsvEntry().ChangeL(entry);
			}
		}
	}

TBool CSmsBioficationTest::CompareSelectionsL(const CMsvEntrySelection& aSelection1,const CMsvEntrySelection& aSelection2)
	{
	TInt count1=aSelection1.Count();
	TInt count2=aSelection2.Count();
	iSmsTest.Printf(_L("Comparing selections, 1st msg count %d, 2nd msg count %d\n"), count1,count2);
	TBool thesame=count1==count2;
	for (TInt i=0; thesame && (i<count1); i++)
		{
		CMsvEntrySelection* selection2=aSelection2.CopyLC();
		thesame=EFalse;
		for (TInt j=0; (!thesame) && (j<count2); j++)
			{
			thesame=CompareEntriesL(aSelection1[i],selection2->At(j));
			if (thesame)
				selection2->Delete(j);
			}
		CleanupStack::PopAndDestroy();
		}
	return thesame;
	}

TBool CSmsBioficationTest::CompareEntriesL(TMsvId aId1, TMsvId aId2)
	{
	TBool thesame=iSmsTest.AreMessagesSameTextL(aId1,aId2);
	if (thesame)
		{
		MsvEntry().SetEntryL(aId1);
		TMsvEntry entry1=MsvEntry().Entry();
		MsvEntry().SetEntryL(aId2);
		TMsvEntry entry2=MsvEntry().Entry();
		thesame=(entry1.iBioType==entry2.iBioType) && (entry1.iMtm==entry2.iMtm);
		}
	return thesame;
	}

void CSmsBioficationTest::RunEnumerateL()
	{
	TInt err = iStatus.Int();

	if (err == KErrNone)
		{
		err = iSmsTest.iProgress.iError;
		iStatus = err;

		if (err == KErrNone)
			{
			iEnumerationFolderId=iSmsTest.iProgress.iEnumerateFolder;
			MsvEntry().SetEntryL(iEnumerationFolderId);
			delete iTestSelection;
			iTestSelection = MsvEntry().ChildrenL();

			iSmsTest.Printf(_L("%d message(s) on SIM. %d message(s) now contained in folder %d.\n"), iSmsTest.iProgress.iMsgCount, iTestSelection->Count(), iEnumerationFolderId);
			iSmsTest.DisplayMessagesL(*iTestSelection);
			}
		}

	iSmsTest.Printf(_L("Enumerate Phone Store completed with %d\n"), err);
	iState = EStateIdle;

	delete iOperation;
	iOperation = NULL;
	}

void CSmsBioficationTest::RunCopyToPhoneStoreL()
	{
	TInt err = iStatus.Int();

	if (err == KErrNone)
		{
		err = iSmsTest.iProgress.iError;
		}

	iSmsTest.Printf(_L("Copy To SIM completed with error %d\n"), err);

	iState = EStateIdle;
	}

void CSmsBioficationTest::RunCopyMoveDeleteFromPhoneStoreL()
	{
	TInt err = iStatus.Int();

	if (err == KErrNone)
		{
		err = iSmsTest.iProgress.iError;
		}

	MsvEntry().SetEntryL(iTestFolderId);
	delete iTestSelection;
	iTestSelection = MsvEntry().ChildrenL();

	switch (iState)
		{
		case EStateCopyingFromPhoneStore:
			iSmsTest.Printf(_L("Copy From Phone Store completed with error %d\n"), err);
			break;
		case EStateMovingFromPhoneStore:
			iSmsTest.Printf(_L("Move From Phone Store completed with error %d\n"), err);
			break;
		case EStateDeletingFromPhoneStore:
			iSmsTest.Printf(_L("Delete From Phone Store completed with error %d\n"), err);
			break;
		default:
			break;
		}

	iState = EStateIdle;
	}

LOCAL_C void doMainL()
	{
	CActiveScheduler* iScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(iScheduler);
	CActiveScheduler::Install( iScheduler );

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;

	smsTest->NotifySaPhoneOnL();

	CSmsBioficationTest* bioficationTest = CSmsBioficationTest::NewLC(*smsTest, KBioficationScript, nextTest);

	smsTest->WaitForInitializeL();

	bioficationTest->StartL();

	CleanupStack::PopAndDestroy(3); //bioficationTest, smsTest, iScheduler
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

