// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_ConnectAndSyncAfterDisconnect.cpp 
// Component: imapservermtm
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_ConnectAndSyncAfterDisconnect.WINSCW.DEB.LOG
// 
//


#include "EmailTestUtils.h"
#include <ImapConnectionObserver.H>
#include <cemailaccounts.h>


_LIT(KAt ,"@");
_LIT8(KFolderPath, "");
_LIT(KServer, ".intra");
_LIT(KConnectAndSyncAfterDisconnect, " Testing ConnectAndSyncAfterDisconnect");
_LIT(KMsgCompleted, "\t CopyL completed with : %d");
_LIT(KMsgSend, "\t messages to send in the outbox = : %d");
_LIT(KProgressError, "\t Progress error = %d");
	
RTest test(KConnectAndSyncAfterDisconnect);
TImapAccount imapAccount;
LOCAL_D TMsvId imap4Service;
LOCAL_D TMsvId smtpService;
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CTrapCleanup* theCleanup;

#if defined (__WINS__)
	#define PDD_NAME		_L("ECDRV")
	#define LDD_NAME		_L("ECOMM")
#else
	#define PDD_NAME		_L("EUART1")
	#define LDD_NAME		_L("ECOMM")
#endif
	// local variables etc //

class TDummySessionObserver : public CBase , public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
		{}
	};
	

class CDummyConnectionObserver : public MMsvImapConnectionObserver
	{
		void HandleImapConnectionEvent(TImapConnectionEvent /*aConnectionEvent*/)
		{}
	};

// Disconnect from ImapServer
LOCAL_C void DisconnectImapServerL()
	{
	test.Printf(_L("DisConnecting to IMAP server                      \n"));
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
 	
  	CMsvEntrySelection *msvSelection = new(ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);
	msvSelection->AppendL(imap4Service);

	TBuf8 <1> aParameter;
  	CMsvOperation* msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMDisconnect,*msvSelection, aParameter, testActive->iStatus);
	testUtils->WriteComment(_L("Dissconnect from IMAP server  "));
	CleanupStack::PushL(msvOperation);

	testActive->StartL();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(3,testActive);//msvOperation,msvSelection,children,testActive
	}


LOCAL_C void ConnectAndFullSyncImapServerL()
	{
	CTestActive* testActive = new(ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	
	CMsvEntrySelection * msvSelection = new(ELeave) CMsvEntrySelection;
	CleanupStack::PushL(msvSelection);
	
	test.Printf(_L("Connecting to IMAP server             \n"));
	msvSelection->AppendL(imap4Service);
	
	CDummyConnectionObserver *dummyObserver = new(ELeave)CDummyConnectionObserver; 
	CleanupStack::PushL(dummyObserver);
	TPckg<MMsvImapConnectionObserver*> aParameter(dummyObserver);
  	
  	CMsvOperation* msvOperation = testUtils->iImapClientMtm->InvokeAsyncFunctionL(KIMAP4MTMConnectAndSyncCompleteAfterDisconnect,*msvSelection, aParameter, testActive->iStatus);
   	testUtils->WriteComment(_L(" Connect to Imap server and Sync the mails  "));
	CleanupStack::PushL(msvOperation);
	
	testActive->StartL();
	CActiveScheduler::Start();

   	CleanupStack::PopAndDestroy(4,testActive);//msvOperation,dummyObserver,msvSelection,testActive
   	}


//	
LOCAL_C void CreateNewSmtpSettingsL()
	{
	testUtils->iMsvEntry->SetEntryL(smtpService);
	TMsvId serviceId = smtpService;
	//overwrite the settings with test code one.  Don't want the default settings.
	CImSmtpSettings* settings = new(ELeave) CImSmtpSettings();
	CleanupStack::PushL(settings);
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL( serviceId, smtpAccountId);
	accounts->LoadSmtpSettingsL(smtpAccountId, *settings);
	TInt serverLength = testUtils->MachineName().Length();
	HBufC* serverad = HBufC::NewL(serverLength + 7 ); // 7 is the length of KServerAddress
	CleanupStack::PushL(serverad);
	serverad->Des().Copy(testUtils->MachineName());
	serverad->Des().Append(KServer);	
    settings->SetServerAddressL(*serverad);
    HBufC* emailaddress = HBufC::NewL(serverLength + 8 + serverad->Des().Length() + 1 );
   	CleanupStack::PushL(emailaddress);
   	emailaddress->Des().Copy(testUtils->MachineName());
   	emailaddress->Des().Append(KAt);
   	emailaddress->Des().Append(*serverad);
   	settings->SetEmailAddressL(*emailaddress);
	settings->SetReplyToAddressL(*emailaddress);
	settings->SetReceiptAddressL(*emailaddress);
	settings->SetEmailAliasL(_L("Test Account"));
	settings->SetBodyEncoding(EMsgOutboxMIME);
	settings->SetRequestReceipts(EFalse);
	settings->SetPort(25);
	TSmtpAccount smtpAccount;
 	accounts->GetSmtpAccountL(smtpService, smtpAccount);
	accounts->SaveSmtpSettingsL(smtpAccount, *settings);
	CleanupStack::PopAndDestroy(emailaddress);
	CleanupStack::PopAndDestroy(serverad);
	CleanupStack::PopAndDestroy(2,settings);
	testUtils->WriteComment(_L("Created New SmtpSettings  "));	
		
	}
LOCAL_C void CreateNewPlaintextMessageL()
	{
	CTestActive* testActive = new(ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	TMsvEmailTypeList msvEmailTypeList = 0;
	TMsvPartList partList = (KMsvMessagePartBody | KMsvMessagePartAttachments);
	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession),KMsvGlobalOutBoxIndexEntryId, partList, msvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);
	TestMsvOperationTimer* testMsvOperationTimer = TestMsvOperationTimer::NewLC(test.Console(), emailOperation, test);
	testActive->StartL();
	testMsvOperationTimer->IssueRequest();
	CActiveScheduler::Start();
	//check progress
	TMsvId temp;
	TPckgC<TMsvId> paramPack(temp);
	const TDesC8& progBuf = emailOperation->ProgressL();
	paramPack.Set(progBuf);
	TMsvId newMessageId;
	newMessageId = paramPack();
	test(newMessageId != NULL);
	testUtils->WriteComment(_L("\t Created New plaintext message"));
	testUtils->iMsvEntry->SetEntryL(newMessageId);
    CMsvStore* store = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store);
	CImHeader* header = CImHeader::NewLC();
	header->RestoreL(*store);
	
	TInt serverLength = testUtils->MachineName().Length();
	HBufC* serverad = HBufC::NewL(serverLength + 7 ); // 7 is the length of KServerAddress
	CleanupStack::PushL(serverad);
	serverad->Des().Copy(testUtils->MachineName());
	serverad->Des().Append(KServer);	
    HBufC* emailaddress = HBufC::NewL(serverLength + 8 + serverad->Des().Length() + 1 );
   	CleanupStack::PushL(emailaddress);
   	emailaddress->Des().Copy(testUtils->MachineName());
   	emailaddress->Des().Append(KAt);
   	emailaddress->Des().Append(*serverad);
   	header->ToRecipients().AppendL(*emailaddress);
   	
	header->SetSubjectL(_L("Test sending message using CMsvEntry::CopyL!!"));
	header->StoreL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy(emailaddress);
	CleanupStack::PopAndDestroy(serverad);
	CleanupStack::PopAndDestroy(5,testActive); // header,store,testMsvOperationTimer,emailOperation,testActive
	
		
	}
LOCAL_C void SendSmtpMessageL()
	{
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	testUtils->InstantiateSmtpClientMtmL();
	CMsvEntrySelection* sendSel = testUtils->iMsvEntry->ChildrenL();
	CleanupStack::PushL(sendSel);
	TInt count = sendSel->Count();
	test(count >= 1);
	testUtils->Printf(KMsgSend, count);
	testUtils->iSmtpClientMtm->SetCurrentEntryL(testUtils->iMsvSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId));
	CMsvOperation* msvOperation = testUtils->iSmtpClientMtm->Entry().CopyL(*sendSel, smtpService, testActive->iStatus);
	CleanupStack::PushL(msvOperation);
	testActive->StartL();
	CActiveScheduler::Start();

	//check progress
    testUtils->Printf(KMsgCompleted, testActive->iStatus.Int()); 

	CMsvEntrySelection* sendSelc = testUtils->iMsvEntry->ChildrenL();
	TInt noOfMessages = sendSelc->Count();

	// Check Whether the mails in Outbox is Zero (i.e. mails has been sent..).
    test(noOfMessages == 0);
  	delete sendSelc;
 	TImSmtpProgress temp;	
	TPckgC<TImSmtpProgress> paramPack(temp);
	const TDesC8& progBuf = msvOperation->ProgressL();
	paramPack.Set(progBuf);
	TImSmtpProgress progress = paramPack();	
	
	testUtils->Printf(KProgressError, progress.Error()); 

	test(progress.Error()==0);
	CleanupStack::PopAndDestroy(3,testActive); //msvOperation,sendsel,testActive
	}
	
LOCAL_C void CreateandSendMessageL()
	{
	test.Next(_L("Test sending message to server "));
	CreateNewSmtpSettingsL();
	testUtils->iMsvEntry->SetEntryL(smtpService);
	CreateNewPlaintextMessageL();
	SendSmtpMessageL();
	}

LOCAL_C void TestConnectAndSyncAfterDisconnectL()
	{
	CreateandSendMessageL();
	// Tests start here
	testUtils->iImapClientMtm->SwitchCurrentEntryL(imap4Service);
	// connect, do full sync and then  so that the inbox can be synced
    test.Printf(_L("Connecting to IMAP server            \n"));
    ConnectAndFullSyncImapServerL();
    //Check if disconnects immediately
    DisconnectImapServerL();
	}

LOCAL_C void InitL()
	{
	// Load the serial comms device drivers.  If this is not done,
	// connecting via NT-RAS returns KErrNotFound (-1).
	TInt driverErr;
	driverErr=User::LoadPhysicalDevice(PDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);
	driverErr=User::LoadLogicalDevice(LDD_NAME);
	if (driverErr!=KErrNone && driverErr!=KErrAlreadyExists)
		User::Leave(driverErr);

	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);
	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
    testUtils->ClearEmailAccountsL();
	test.Next(_L("Create Data Component FileStores"));
	testUtils->CreateSmtpServerMtmRegL();
	//	Loading the DLLs
	testUtils->CreateRegistryObjectAndControlL();
	testUtils->InstallSmtpMtmGroupL();
	testUtils->GoClientSideL();
	// Create Services
	smtpService = testUtils->CreateSmtpServiceL();
	imap4Service = testUtils->CreateImapServiceL();
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	TSmtpAccount smtpAccount;
	accounts->GetSmtpAccountL(smtpService, smtpAccount);
	accounts->SetDefaultSmtpAccountL(smtpAccount);
    testUtils->WriteComment(_L("Create  Imap   Services           ")); 
	// Delete any *.scr files in c:\logs\email directory.
	testUtils->DeleteScriptFilesFromLogsDirL();

	testUtils->iMsvEntry->SetEntryL(imap4Service);
	//overwrite the settings with test code one.  Don't want the default settings.
	CImImap4Settings* settings = new(ELeave) CImImap4Settings();
	CleanupStack::PushL(settings);
	settings->Reset();
	TInt serverLength = testUtils->MachineName().Length();
	HBufC* serverad = HBufC::NewL(serverLength + 7 ); // 7 is the length of KServerAddress
	CleanupStack::PushL(serverad);
	serverad->Des().Copy(testUtils->MachineName());
	serverad->Des().Append(KServer);
	settings->SetServerAddressL(*serverad);
	CleanupStack::PopAndDestroy(serverad); 
	settings->SetDeleteEmailsWhenDisconnecting(ETrue);
	settings->SetAcknowledgeReceipts(ETrue);
	settings->SetAutoSendOnConnect(ETrue);
	settings->SetLoginNameL(testUtils->MachineName());
	settings->SetPasswordL(testUtils->MachineName());
   	settings->SetPort(143);
	settings->SetFolderPathL(KFolderPath);
	accounts->GetImapAccountL(imap4Service, imapAccount);
	accounts->SaveImapSettingsL(imapAccount, *settings);
	CleanupStack::PopAndDestroy(2,accounts); //accounts,settings
	test.Printf(_L("Instantiating IMAP4 Client MTM"));
	testUtils->InstantiateImapClientMtmL();
	}
	
LOCAL_C void Closedown()
	{
     
	CleanupStack::PopAndDestroy(2);  //testUtils, scheduler 
	}

LOCAL_C void doMainL()
	{
	test.Start(_L("Test - ConnectAndSyncAfterDisconnect"));
	InitL();
  	testUtils->TestStart(1);
	TestConnectAndSyncAfterDisconnectL();
 	testUtils->TestFinish(1);

	testUtils->TestHarnessCompleted();

	Closedown();		
	}


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

