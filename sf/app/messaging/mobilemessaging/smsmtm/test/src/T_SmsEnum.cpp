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
#include <smutset.h>
#include <smscmds.h>
#include "T_SmsEnum.h"
#include <txtrich.h>
#include <csmsaccount.h>

RTest test(_L("T_SmsEnum Testrig"));
CTrapCleanup* theCleanup;

_LIT(KSmsScript, "smsenum.script");

#if defined (__WINS__)
_LIT(KPddName,"ECDRV");
_LIT(KLddName,"ECOMM");
#else	//__WINS__
_LIT(KPddSirName,"ESIR");
_LIT(KPddUartName,"ECUART4");
_LIT(KLddName,"ECOMM");
#endif	//__WINS__

_LIT(KMessageData, "This is a simple text message");
_LIT(KRecipientOk,		"+447973500446");

void CSmsEnumTest::TestCopyFromSimL(TMsvId aFolderId)
/**
	Tests copying messages from SIM
 */
	{
	iSmsTest.Test().Next(_L("Copy From SIM"));
	iState = EStateCopyFromSim;

	if (aFolderId == KErrNotFound)
		MsvEntry().SetEntryL(iSmsTest.iProgress.iEnumerateFolder);
	else
		MsvEntry().SetEntryL(aFolderId);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();
	iSelection->InsertL(0,iSmsTest.iSmsServiceId);

	TPckgBuf<TMsvId> pkg(KMsvGlobalInBoxIndexEntryId);

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandCopyFromPhoneStore, pkg, iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsEnumTest::TestMoveFromSimL(TMsvId aFolderId)
/**
	Tests moving messages from SIM
 */
	{
	iSmsTest.Test().Next(_L("Move From SIM"));
	iState = EStateMoveFromSim;

	if (aFolderId == KErrNotFound)
		MsvEntry().SetEntryL(iSmsTest.iProgress.iEnumerateFolder);
	else
		MsvEntry().SetEntryL(aFolderId);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();
	iSelection->InsertL(0,iSmsTest.iSmsServiceId);

	TPckgBuf<TMsvId> pkg(KMsvGlobalInBoxIndexEntryId);

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandMoveFromPhoneStore, pkg, iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsEnumTest::TestDeleteFromSimL()
/**
	Tests deleting messages from SIM
 */
	{
	iSmsTest.Test().Next(_L("Delete From SIM"));
	iState = EStateDeleteFromSim;

	MsvEntry().SetEntryL(iSmsTest.iProgress.iEnumerateFolder);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();
	iSelection->InsertL(0,iSmsTest.iSmsServiceId);

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandDeleteFromPhoneStore, TPtrC8(), iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

TInt CSmsEnumTest::GetInboxMsgsCountL()
/**
	Count messages in Inbox
 */
	{
	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();
	return iSelection->Count();
	}

void CSmsEnumTest::ClearInboxL()
/**
	Remove all messages from the Inbox
 */
	{
	iState = EStateClearInbox;
	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();

	if (iSelection->Count() > 0)
		{
		delete iOperation;
		iOperation = NULL;
		iOperation = MsvEntry().DeleteL(*iSelection, iStatus);
		SetActive();
		CActiveScheduler::Start();
		}
	}

void CSmsEnumTest::SaveClass2FolderIdL(TMsvId aFolderId)
/**
	Set the class 2 folder in the service Id
	so that it will be picked up correctly by SMSS
 */
	{
	MsvEntry().SetEntryL(iSmsTest.iSmsServiceId);
	CSmsSettings *settings=CSmsSettings::NewL();
	CleanupStack::PushL(settings);

	CSmsAccount* account = CSmsAccount::NewLC();
	TRAPD(error, account->LoadSettingsL(*settings));
	if (error)
		{
		account->InitialiseDefaultSettingsL(*settings);
		}

	settings->SetClass2Folder(aFolderId);

	account->SaveSettingsL(*settings);
	CleanupStack::PopAndDestroy(account);
	CleanupStack::PopAndDestroy(settings);
	}

TMsvId CSmsEnumTest::CreateEnumerateFolderLC(TMsvId aParent)
	{
	iSmsTest.SetEntryL(aParent);

	TMsvEntry entry;
	entry.SetVisible(EFalse);
	entry.iType = KUidMsvFolderEntry;
	entry.iMtm = KUidMsgTypeSMS;
	entry.iServiceId = iSmsTest.iSmsServiceId;
	iSmsTest.CreateEntryL(entry);

	iSmsTest.iMsvSession->CleanupEntryPushL(entry.Id());
	return entry.Id();
	}

void CSmsEnumTest::TestCopyToSimL()
/**
	Tests copying messages to SIM
 */
	{
	iSmsTest.Test().Next(_L("Copy to SIM"));
	iState = EStateCopyToSim;

	iSelection->Reset();

	TTime now;
	now.HomeTime();
	iSelection->Reset();

	iSmsTest.ReadScriptL(iScriptFile, KMsvGlobalOutBoxIndexEntryId, *iSelection, now);
	iSelection->InsertL(0,iSmsTest.iSmsServiceId);
	iSmsTest(iSelection->Count());

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandCopyToPhoneStore, TPtrC8(), iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsEnumTest::TestMoveToSimL(TMsvId aSourceFolderId)
/**
	Tests copying messages to SIM
 */
	{
	iSmsTest.Test().Next(_L("Move to SIM"));
	iState = EStateMoveToSim;

	MsvEntry().SetEntryL(aSourceFolderId);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();
	iSelection->InsertL(0,iSmsTest.iSmsServiceId);

	iSmsTest(iSelection->Count());

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandMoveToPhoneStore, TPtrC8(), iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsEnumTest::TestEnumerateL(TMsvId aFolderId)
/**
	Enumerates
 */
	{
	iSmsTest.Test().Next(_L("Enumerate SIM"));
	iState = EStateEnumerating;

	iSmsTest.Printf(_L("TestEnumerateL(%d)\n"), aFolderId);

	if (aFolderId != KErrNotFound)
		{
		iLastEnumerateFolder() = aFolderId;
		iLastEnumerateFolder.SetLength(4);
		}
	else
		iLastEnumerateFolder.SetLength(0);

	iSelection->Reset();
	iSelection->AppendL(iSmsTest.iSmsServiceId);

	delete iOperation;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandEnumeratePhoneStores, iLastEnumerateFolder, iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsEnumTest::TestEnumerateAndDescLengthL(TMsvId aFolderId)
/**
	Tests both enumeration and changing the default description length.
*/
	{
	iSmsTest.Test().Next(_L("Enumerate SIM and non-default description length"));
	iState = EStateEnumeratingAndDescLength;

	iSmsTest.Printf(_L("TestEnumerateAndDescLength(%d)\n"), aFolderId);

	if (aFolderId != KErrNotFound)
		{
		iLastEnumerateFolder() = aFolderId;
		iLastEnumerateFolder.SetLength(4);
		}
	else
		iLastEnumerateFolder.SetLength(0);

	// Set the description length - first restore the sms settings
	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);

	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iSmsTest.iServiceSettings);

	// Set the description length to 1
	iSmsTest.iServiceSettings->SetDescriptionLength(1);

	// Store the sms settings
	account->SaveSettingsL(*iSmsTest.iServiceSettings);
	CleanupStack::PopAndDestroy(account);

	iSelection->Reset();
	iSelection->AppendL(iSmsTest.iSmsServiceId);

	delete iOperation;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandEnumeratePhoneStores, iLastEnumerateFolder, iStatus);
	SetActive();
	CActiveScheduler::Start();
	}

void CSmsEnumTest::TestEnumerateAndDetailsFieldL()
	{
	iSmsTest.Test().Next(_L("Enumerate SIM and test that details field is ok."));
	iState = EStateEnumeratingAndDetailsField;
	
	// First create deliver message in the inbox.
	TMsvEntry entry;
	entry.SetVisible(ETrue); 
	entry.SetInPreparation(ETrue); 
	entry.iServiceId = iSmsTest.iSmsServiceId; 
	entry.iType = KUidMsvMessageEntry; 
	entry.iMtm = KUidMsgTypeSMS; 
	entry.iDate.HomeTime(); 
	entry.iSize = 0; 
	entry.iDescription.Set(KNullDesC); 
	entry.iDetails.Set(KNullDesC); 
	entry.SetSendingState(KMsvSendStateNotApplicable); 

	// Create the SMS header object...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iSmsTest.iRichText);
	CleanupStack::PushL(header);
	
	// Set the body text...
	iSmsTest.iRichText->Reset();
	iSmsTest.iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	header->SetSmsSettingsL(*iSmsTest.iServiceSettings); 
	
	// Set recipient - need to set directly in the CSmsMessage as this a Deliver PDU
	header->Message().SetToFromAddressL(KRecipientOk);
	
	// Update entry description and details...
	entry.iDetails.Set(header->Message().ToFromAddress());
	entry.iDescription.Set(iSmsTest.iRichText->Read(0, iSmsTest.iServiceSettings->DescriptionLength()));
	entry.SetInPreparation(EFalse);
	
	// Create the entry - set context to the global outbox.
	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().CreateL(entry);

	// Create new store and save header information 
	MsvEntry().SetEntryL(entry.Id()); 
	CMsvStore* store = MsvEntry().EditStoreL(); 
	CleanupStack::PushL(store); 
	header->StoreL(*store);
	store->StoreBodyTextL(*iSmsTest.iRichText);
	store->CommitL(); 
	CleanupStack::PopAndDestroy(2, header); 
	
	// Move the message to the SIM

	delete iSelection;
	iSelection = NULL;
	iSelection = new (ELeave) CMsvEntrySelection();
	iSelection->AppendL(iSmsTest.iSmsServiceId);
	iSelection->AppendL(entry.Id());
	
	iSmsTest(iSelection->Count());

	delete iOperation;
	iOperation = NULL;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandMoveToPhoneStore, TPtrC8(), iStatus);
	SetActive();
	CActiveScheduler::Start();
	
	// Enumerate the SIM - first create the folder into which to enumerate
	TMsvId folder = CreateEnumerateFolderLC(KMsvGlobalInBoxIndexEntryId);
	
	iLastEnumerateFolder() = folder;
	iLastEnumerateFolder.SetLength(4);

	iSelection->Reset();
	iSelection->AppendL(iSmsTest.iSmsServiceId);

	delete iOperation;
	iOperation = Session().TransferCommandL(*iSelection, ESmsMtmCommandEnumeratePhoneStores, iLastEnumerateFolder, iStatus);
	SetActive();
	CActiveScheduler::Start();
	
	// Ok see if things are how it should be...
	iSmsTest.SetEntryL(folder);
	delete iSelection;
	iSelection = MsvEntry().ChildrenL();
	iSmsTest.Test()(iSelection->Count() == 1);

	iSmsTest.SetEntryL(iSelection->At(0));
	entry = MsvEntry().Entry();
	iSmsTest.Test()(entry.iDetails.Length() > 0);
	
	// Destroy the enumerate folder
	CleanupStack::PopAndDestroy();	
	}

void CSmsEnumTest::TestMultipleRecipientEnumerateL()
	{
	// Clean the existing messages
	ClearInboxL();
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	if (iSelection->Count())
		{
		TestDeleteFromSimL();
		User::LeaveIfError(iStatus.Int());
		}
	
	// Copy to SIM
	TestCopyToSimL();
	User::LeaveIfError(ErrorStatus());
	// Count the number of messages to be copied onto SIM,
	// including multiple msgs for multiple recipients.
	TInt msgCount = 0;
	for (TInt index=1; index < (iSelection->Count()); index++)
		{
		CSmsHeader* smsHeader = iSmsTest.GetHeaderLC(iSelection->At(index));
		if (smsHeader->Message().Type() == CSmsPDU::ESmsSubmit)
			{
			msgCount += smsHeader->Recipients().Count();
			}
		else
			{
			++msgCount;
			}
		CleanupStack::PopAndDestroy(smsHeader);
		}

	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	//Count the number of messages copied onto SIM
	TInt simMsgCount = iSmsTest.iProgress.iMsgCount;

	// Check msg count on SIM after copy
	iSmsTest.Printf(_L("Count msgs on SIM: Expected msgs %d, Copied msgs %d\n"), msgCount, simMsgCount);
	if (msgCount != simMsgCount)
		{
		User::LeaveIfError(KErrNotFound);
		}
	
	// Clean local Inbox and copy msgs from sim
	if (iSelection->Count())
		{
		ClearInboxL();
		TestCopyFromSimL();
		User::LeaveIfError(ErrorStatus());
		}
	msgCount = GetInboxMsgsCountL();

	// Check msg count in local inbox after copy from SIM
	iSmsTest.Printf(_L("Count msgs in Inbox: Expected msgs %d, Copied msgs %d\n"), simMsgCount, msgCount);
	if (msgCount != simMsgCount)
		{
		User::LeaveIfError(KErrNotFound);
		}

	}


CSmsEnumTest::CSmsEnumTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest), iState(EStateWaiting)
	{
	CActiveScheduler::Add(this);
	}

LOCAL_C void doMainL()
	{
#if defined (__WINS__)
	User::LoadPhysicalDevice(KPddName);
	User::LoadLogicalDevice(KLddName);
#else	//__WINS__
	User::LoadPhysicalDevice(KPddSirName);
	User::LoadPhysicalDevice(KPddUartName);
    User::LoadLogicalDevice(KLddName);
#endif	//__WINS__

	TInt r = StartC32();
	if( (r != KErrNone) && (r != KErrAlreadyExists) )
		test.Printf(_L("Failed to start C32 %d\n"), r);

	CActiveScheduler* iScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(iScheduler);
	CActiveScheduler::Install( iScheduler );

	CSmsTestUtils* smsTest = CSmsTestUtils::NewL(test);
	CleanupStack::PushL(smsTest);
	TInt nextTest = 0;

	smsTest->NotifySaPhoneOnL();

	CSmsEnumTest* enumTest = CSmsEnumTest::NewLC(*smsTest, KSmsScript, nextTest);

	smsTest->WaitForInitializeL();

	enumTest->StartL();

	//CleanupStack::PopAndDestroy(3); //SmsTest, iScheduler
	CleanupStack::PopAndDestroy(enumTest);
	CleanupStack::PopAndDestroy(smsTest);
	CleanupStack::PopAndDestroy(iScheduler);
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("Setup"));
	theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test.Printf(_L("doMainL completed with %d\n"), ret);
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

void CSmsEnumTest::RunAutoL()
/**
	Runs tests in sequence, checks iStatus
 */
	{
	// Wait 5 seconds for everything to start up
	User::After(5000000);

	// Initial clean-up
	iSmsTest.TestStart(++iNextTest, _L("(Cleanup) Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	if (iSelection->Count())
		{
		iSmsTest.TestStart(++iNextTest, _L("(Cleanup) Delete from SIM"));
		TestDeleteFromSimL();
		User::LeaveIfError(iStatus.Int());
		iSmsTest.TestFinish(iNextTest, KErrNone);
		}

	// Actual testing
	ClearInboxL();
	iSmsTest.TestStart(++iNextTest, _L("Enumerate - check details field"));
	TestEnumerateAndDetailsFieldL();
	User::LeaveIfError(ErrorStatus());
	iSmsTest.TestFinish(iNextTest, KErrNone);
	ClearInboxL();

	iSmsTest.TestStart(++iNextTest, _L("Copy to SIM"));
	TestCopyToSimL();
	User::LeaveIfError(ErrorStatus());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

//	iSmsTest.TestStart(++iNextTest, _L("Test UnRead Status - all messages un-read after 1st enumerate"));
//	TestUnreadStatusL();
//	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Enumerate and non-default description length"));
	TestEnumerateAndDescLengthL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Test Read Status - all messages read after 2nd enumerate"));
	TestReadStatusL();
	iSmsTest.TestFinish(iNextTest, KErrNone);

	SaveClass2FolderIdL(iSmsTest.iProgress.iEnumerateFolder);
	TMsvId class2Folder = iSmsTest.iProgress.iEnumerateFolder;
	ClearInboxL();

	if (iSelection->Count())
		{
		iSmsTest.TestStart(++iNextTest, _L("Move from SIM"));
		TestMoveFromSimL(class2Folder);
		User::LeaveIfError(ErrorStatus());
		iSmsTest.TestFinish(iNextTest, KErrNone);
		}
	
	iSmsTest.TestStart(++iNextTest, _L("Copy to SIM"));
	TestCopyToSimL();
	User::LeaveIfError(ErrorStatus());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Test SIM store status after copy to SIM"));
	TestSimStoreStatusL(class2Folder);
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Enumerate"));
	TestEnumerateL();
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	if (iSelection->Count())
		{
		iSmsTest.TestStart(++iNextTest, _L("Move from SIM"));
		TestMoveFromSimL(class2Folder);
		User::LeaveIfError(ErrorStatus());
		iSmsTest.TestFinish(iNextTest, KErrNone);
		}

	iSmsTest.TestStart(++iNextTest, _L("Test phone store status after move from SIM"));
	TestPhoneStoreStatusL(KMsvGlobalInBoxIndexEntryId);
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Move to SIM"));
	TestMoveToSimL(KMsvGlobalInBoxIndexEntryId);
	User::LeaveIfError(ErrorStatus());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Test SIM store status after move to SIM"));
	TestSimStoreStatusL(class2Folder);
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Copy from SIM"));
	TestCopyFromSimL(class2Folder);
	User::LeaveIfError(ErrorStatus());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	iSmsTest.TestStart(++iNextTest, _L("Test phone store status after copy from SIM"));
	TestPhoneStoreStatusL(KMsvGlobalInBoxIndexEntryId);
	iSmsTest.TestFinish(iNextTest, KErrNone);

	// Copy to SIM, multiple recipients
	iSmsTest.TestStart(++iNextTest, _L("Copy to SIM, multiple recipients"));
	TestMultipleRecipientEnumerateL();
	iSmsTest.TestFinish(iNextTest, KErrNone);

	TestAllEnumerateL();
	TMsvId folder = CreateEnumerateFolderLC(KMsvGlobalInBoxIndexEntryId);
	TestAllEnumerateL(folder);
	TestAllEnumerateL(folder);
	folder = CreateEnumerateFolderLC(KMsvGlobalInBoxIndexEntryId);
	TestAllEnumerateL(folder);
	CleanupStack::PopAndDestroy(2);

	iSmsTest.TestStart(++iNextTest, _L("Enumerate before move"));
	TestEnumerateL(KErrNotFound);
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	if (iSelection->Count())
		{
		iSmsTest.TestStart(++iNextTest, _L("Move from SIM"));
		TestMoveFromSimL();
		User::LeaveIfError(iStatus.Int());
		iSmsTest.TestFinish(iNextTest, KErrNone);
		}
	}

void CSmsEnumTest::TestAllEnumerateL(TMsvId aFolderId)
	{
	iSmsTest.TestStart(++iNextTest, _L("Enumerate"));
	TestEnumerateL(aFolderId);
	User::LeaveIfError(iStatus.Int());
	iSmsTest.TestFinish(iNextTest, KErrNone);

	if (iSelection->Count())
		{
		iSmsTest.TestStart(++iNextTest, _L("Copy from SIM"));
		TestCopyFromSimL(aFolderId);
		User::LeaveIfError(ErrorStatus());
		iSmsTest.TestFinish(iNextTest, KErrNone);
		}
	}

void CSmsEnumTest::ConstructL()
	{
	SetTestNameL(KSmsEnumTestName);
	iSmsTest.SetLogToFile();

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

	iParaFormat = CParaFormatLayer::NewL();
	iCharFormat = CCharFormatLayer::NewL();
	iBody = CRichText::NewL(iParaFormat, iCharFormat);
	iHeader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iBody);
	}

void CSmsEnumTest::ShowMenuL()
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Enumerate SIM"));
	iSmsTest.AppendToMenuL(_L("Copy To SIM"));
	iSmsTest.AppendToMenuL(_L("Display Log"));
	iSmsTest.AppendToMenuL(_L("Clear Log"));

	TInt result = iSmsTest.DisplayMenu(_L("SMS Enumerate Sim Test"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			TestEnumerateL();
			break;
		case 2:
			TestCopyToSimL();
			break;
		case 3:
			DisplayLogL();
			break;
		case 4:
			ClearLogL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}

	ShowMenuL();
	}

CSmsEnumTest::~CSmsEnumTest()
	{
	delete iBody;
	delete iHeader;
	delete iParaFormat;
	delete iCharFormat;

	iSocket.Close();
	iSocketServ.Close();
	}

CSmsEnumTest* CSmsEnumTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmsEnumTest* self = new (ELeave) CSmsEnumTest(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();
	return self;
	}

void CSmsEnumTest::RunL()
/**
	Handles completed async operations
 */
	{
	CActiveScheduler::Stop();

	iSmsTest(iStatus.Int() == KErrNone);

	iSmsTest.SetProgressL(*iOperation);

	switch (iState)
		{
		case EStateEnumerating:
			{
			DoRunEnumerateL();
			break;
			}
		case EStateEnumeratingAndDescLength:
			{
			DoRunEnumerateAndDescLengthL();
			} break;
		case EStateCopyFromSim:
		case EStateMoveFromSim:
		case EStateDeleteFromSim:
			{
			DoRunCopyMoveDeleteFromSim();
			break;
			}
		case EStateCopyToSim:
			{
			DoRunCopyToSim();
			break;
			}
		case EStateClearInbox:
			{
			DoRunClearInbox();
			break;
			}
		default:
			{
			break;
			}
		}

	delete iOperation;
	iOperation = NULL;
	}

TInt CSmsEnumTest::ErrorStatus()
	{
	if (iStatus.Int() == KErrNone)
		return(iSmsTest.iProgress.iError);
	else
		return(iStatus.Int());
	}

void CSmsEnumTest::DoRunClearInbox()
/**
	Checks status
 */
	{
	TInt err = iStatus.Int();
	iSmsTest.Printf(_L("Clear Inbox completed with error %d\n"), err);
	iState = EStateWaiting;
	}

void CSmsEnumTest::DoRunCopyToSim()
/**
	Checks status
 */
	{
	iSmsTest.Printf(_L("Copy To SIM completed with error %d\n"), ErrorStatus());

	iState = EStateWaiting;

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Printf(_L("\nPress any key to continue...\n"));
		iSmsTest.Test().Getch();
		}
	}

void CSmsEnumTest::DoRunMoveToSim()
/**
	Checks status
 */
	{
	iSmsTest.Printf(_L("Move To SIM completed with error %d\n"), ErrorStatus());

	iState = EStateWaiting;

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Printf(_L("\nPress any key to continue...\n"));
		iSmsTest.Test().Getch();
		}
	}

void CSmsEnumTest::DoRunCopyMoveDeleteFromSim()
/**
	Checks status
 */
	{
	switch (iState)
		{
		case EStateCopyFromSim:
			iSmsTest.Printf(_L("Copy From SIM completed with error %d\n"), ErrorStatus());
			break;
		case EStateMoveFromSim:
			iSmsTest.Printf(_L("Move From SIM completed with error %d\n"), ErrorStatus());
			break;
		case EStateDeleteFromSim:
			iSmsTest.Printf(_L("Delete From SIM completed with error %d\n"), ErrorStatus());
			break;
		default:
			break;
		}

	iState = EStateWaiting;

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Printf(_L("\nPress any key to continue...\n"));
		iSmsTest.Test().Getch();
		}
	}

void CSmsEnumTest::DoRunEnumerateL()
/**
	Checks status
 */
	{
	TInt err = iStatus.Int();

	if (err == KErrNone)
		{
		err = iSmsTest.iProgress.iError;
		iStatus = err;

		if (err == KErrNone)
			{
			MsvEntry().SetEntryL(iSmsTest.iProgress.iEnumerateFolder);
			delete iSelection;
			iSelection = NULL;
			iSelection = MsvEntry().ChildrenL();

			iSmsTest.Printf(_L("%d message(s) on SIM. %d message(s) now contained in folder %d.\n"), iSmsTest.iProgress.iMsgCount, iSelection->Count(), iSmsTest.iProgress.iEnumerateFolder);
			iSmsTest.DisplayMessagesL(*iSelection);
			}
		}

	iSmsTest.Printf(_L("Enumerate SIM completed with %d\n"), err);
	iState = EStateWaiting;

	CheckParentAfterEnumerateL();

	delete iOperation;
	iOperation = NULL;

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Printf(_L("\nPress any key to continue...\n"));
		iSmsTest.Test().Getch();

		if  (iSmsTest.iProgress.iMsgCount)
			ShowMenuAfterEnumerateL();
		}
	}

void CSmsEnumTest::DoRunEnumerateAndDescLengthL()
	{
	// Print out messages and check that the description is only a char in length
	TInt err = iStatus.Int();

	if (err == KErrNone)
		{
		err = iSmsTest.iProgress.iError;
		iStatus = err;

		if (err == KErrNone)
			{
			MsvEntry().SetEntryL(iSmsTest.iProgress.iEnumerateFolder);
			delete iSelection;
			iSelection = MsvEntry().ChildrenL();

			iSmsTest.Printf(_L("%d message(s) on SIM. %d message(s) now contained in folder %d.\n"), iSmsTest.iProgress.iMsgCount, iSelection->Count(), iSmsTest.iProgress.iEnumerateFolder);
			TInt count = iSelection->Count();
			
			for (TInt i = 0; i < count && err==KErrNone; ++i)
				{
				// Display the message and then check the description
				TMsvId id = (*iSelection)[i];
				iSmsTest.DisplayMessageL(id);

				iSmsTest.SetEntryL(id);
				if( iSmsTest.Entry().iDescription.Length() != 1 )
					err = KErrCorrupt;
				}
			}
		}

	iSmsTest.Printf(_L("Enumerate SIM completed with %d\n"), err);
	iState = EStateWaiting;

	CheckParentAfterEnumerateL();

	delete iOperation;
	iOperation = NULL;

	if (!iSmsTest.RunAuto())
		{
		iSmsTest.Printf(_L("\nPress any key to continue...\n"));
		iSmsTest.Test().Getch();

		if  (iSmsTest.iProgress.iMsgCount)
			ShowMenuAfterEnumerateL();
		}

	// Restore the description length
	iSmsTest.SetEntryL(iSmsTest.iSmsServiceId);

	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iSmsTest.iServiceSettings);

	// Set the description length to default
	iSmsTest.iServiceSettings->SetDescriptionLength(KSmsDescriptionLength);

	// Store the sms settings
	account->SaveSettingsL(*iSmsTest.iServiceSettings);
	CleanupStack::PopAndDestroy(account);
	}

void CSmsEnumTest::CheckParentAfterEnumerateL()
	{
	if (iLastEnumerateFolder.Length() == 0)
		{
		iSmsTest.SetEntryL(iSmsTest.iProgress.iEnumerateFolder);
		const TMsvId parent = iSmsTest.Entry().Parent();
		if (parent != iSmsTest.iSmsServiceId)
			{
			iSmsTest.Printf(_L("Parent of enum folder is not the SMS service [parent=%d folder=%d service=%d]\n"),
				parent, iSmsTest.iProgress.iEnumerateFolder, iSmsTest.iSmsServiceId);
			User::Leave(KErrGeneral);
			}
		}
	else if (iSmsTest.iProgress.iEnumerateFolder != iLastEnumerateFolder())
		{
		iSmsTest.Printf(_L("Folder does not match [folder=%d request=%d]\n"),
			iSmsTest.iProgress.iEnumerateFolder, iLastEnumerateFolder());
		User::Leave(KErrGeneral);
		}
	}

void CSmsEnumTest::ShowMenuAfterEnumerateL()
/**
	Shows menu after enumerating
 */
	{
	iSmsTest.ResetMenu();

	iSmsTest.AppendToMenuL(_L("Copy From Sim To Inbox"));
	iSmsTest.AppendToMenuL(_L("Move From Sim To Inbox"));
	iSmsTest.AppendToMenuL(_L("Delete From Sim"));

	TInt result = iSmsTest.DisplayMenu(_L("What to do with enumerated messages?"));

	if (result <= 0)
		return;

	switch (result)
		{
		case 1:
			TestCopyFromSimL();
			break;
		case 2:
			TestMoveFromSimL();
			break;
		case 3:
			TestDeleteFromSimL();
			break;
		default:
			User::Leave(KErrArgument);
			break;
		}
	}

void CSmsEnumTest::DoCancel()
/**
	Cancels the async operation
 */
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

void CSmsEnumTest::TestDeleteRemoteEntryL()
/**
	Tests deleting remote entries 
 */
	{
	iSmsTest.Test().Next(_L("Test Delete Remote Entry\n"));

	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();

	iSmsTest.SetEntryL(iSmsTest.iProgress.iEnumerateFolder);
	iSmsTest.SetEntryL(iSmsTest.Entry().Parent());

	CMsvOperation* op = iSmsTest.iMsvEntry->DeleteL(iSmsTest.iProgress.iEnumerateFolder, wait->iStatus);
	CleanupStack::PushL(op);

	CActiveScheduler::Start();

	iSmsTest.Printf(_L("Remote DeleteL() completed with %d\n"), wait->iStatus.Int());

	User::LeaveIfError(wait->iStatus.Int());

	iSmsTest.SetProgressL(*op);

	iSmsTest.Printf(_L("Operation completed with %d, msg count %d\n"), iSmsTest.iProgress.iError, iSmsTest.iProgress.iMsgCount);
	User::LeaveIfError(iSmsTest.iProgress.iError);

	CleanupStack::PopAndDestroy(2); //wait, op
	}

void CSmsEnumTest::TestUnreadStatusL()
/**
	Tests that the unread flag of any messaged on the SIM is propagated
	to the entries created in the class 2 folder

	These should have the Unread() flag SET after the first enumeration.
 */
	{
	iSmsTest.Test().Next(_L("Test Unread Status\n"));

	MsvEntry().SetEntryL(iSmsTest.iProgress.iEnumerateFolder);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();

	TInt count = iSelection->Count();
	TInt status = KErrNone;

	while (count--)
		{
		MsvEntry().SetEntryL(iSelection->At(count));
		if (!MsvEntry().Entry().Unread())
			{
			status = KErrCorrupt;
			break;
			}
		}

	iSmsTest.Printf(_L("TestUnreadStatusL() completed with %d\n"), status);
	User::LeaveIfError(status);
	}

void CSmsEnumTest::TestReadStatusL()
/**
	Tests that the unread flag of any messaged on the SIM is propagated
	to the entries created in the class 2 folder

	These should have the Unread() flag UNSET after the second enumeration.
 */
	{
	iSmsTest.Test().Next(_L("Test Unread Status\n"));

	MsvEntry().SetEntryL(iSmsTest.iProgress.iEnumerateFolder);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();

	TInt count = iSelection->Count();
	TInt status = KErrNone;

	while (count--)
		{
		MsvEntry().SetEntryL(iSelection->At(count));
		if (MsvEntry().Entry().Unread())
			{
			status = KErrCorrupt;
			break;
			}
		}

	iSmsTest.Printf(_L("TestReadStatusL() completed with %d\n"), status);
	User::LeaveIfError(status);
	}

void CSmsEnumTest::TestPhoneStoreStatusL(TMsvId aFolderId)
/**
	Tests that the messages in the specified folder have the storage
	set to ESmsPhoneStorage and have no iSlotArray members.
 */
	{
	iSmsTest.Test().Next(_L("Test Phone Store Status\n"));

	MsvEntry().SetEntryL(aFolderId);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();

	TInt count = iSelection->Count();
	TInt status = KErrNone;

	while (count--)
		{
		MsvEntry().SetEntryL(iSelection->At(count));
		CMsvStore* store = MsvEntry().ReadStoreL();
		CleanupStack::PushL(store);
		iHeader->RestoreL(*store);
		CSmsMessage& smsmessage = iHeader->Message();

		if ((smsmessage.iSlotArray.Count() > 0) ||
		    (smsmessage.Storage() != CSmsMessage::ESmsPhoneStorage))
			{
			status = KErrCorrupt;
			}

		CleanupStack::PopAndDestroy(store);
		}

	iSmsTest.Printf(_L("TestPhoneStoreStatusL() completed with %d\n"), status);
	User::LeaveIfError(status);
	}

void CSmsEnumTest::TestSimStoreStatusL(TMsvId aFolderId)
/**
	Tests that the messages in the specified folder have the storage
	set to ESmsSimStorage and have at least 1 iSlotArray member.
 */
	{
	iSmsTest.Test().Next(_L("Test Sim Store Status\n"));

	MsvEntry().SetEntryL(aFolderId);

	delete iSelection;
	iSelection = NULL;
	iSelection = MsvEntry().ChildrenL();

	TInt count = iSelection->Count();
	TInt status = KErrNone;

	while (count--)
		{
		MsvEntry().SetEntryL(iSelection->At(count));
		CMsvStore* store = MsvEntry().ReadStoreL();
		CleanupStack::PushL(store);
		iHeader->RestoreL(*store);
		CSmsMessage& smsmessage = iHeader->Message();

		if ((smsmessage.iSlotArray.Count() < 1) ||
		    (smsmessage.Storage() != CSmsMessage::ESmsSIMStorage))
			{
			status = KErrCorrupt;
			}

		CleanupStack::PopAndDestroy(store);
		}

	iSmsTest.Printf(_L("TestSimStoreStatusL() completed with %d\n"), status);
	User::LeaveIfError(status);
	}
