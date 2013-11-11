// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_smsdeliver.h"

#include <e32test.h>
#include <smutset.h>
#include <smuthdr.h>
#include <txtrich.h>
#include "smstestutils.h"
#include <msvuids.h>
#include <smsuaddr.h>
#include <csmsaccount.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include <tmsvsmsentry.h>
#endif

_LIT(KFailed,"Failed to complete tests.");
_LIT(KRecipientOk,		"+447747065438");  // has to be valid number and switched on
_LIT(KRecipientBad,		"+442071541467");
_LIT(KRecipientFail,	"+34933753717");
_LIT(KMessageData, "This is a simple text message");
_LIT(KTestDeliverOneRecipientOk,		"Single recipient, deliver ok");
_LIT(KTestDeliverOneRecipientFail, 		"Single recipient, deliver fail");
_LIT(KTestDeliverMultiRecipientsOk_1,	"Multi (2) recipients, deliver ok"); 
_LIT(KTestDeliverMultiRecipientsOk_2,	"Multi (2) recipients, deliver ok, but 1 fails to send");
_LIT(KTestDeliverMultiRecipientsOk_3,	"Multi (3) recipients, deliver ok, but 1 fails to send");
_LIT(KTestDeliverNoMatching,			"Single recipient, no matching");
_LIT(KTestDeliverNoStatusReports,		"Single recipient, no status reports");
_LIT(KTestDeliverNotSent,				"Single recipient, not sent");
_LIT(KTestDeliverMultiRecipientsFail_1,	"Multi (2) recipients, deliver failure");
_LIT(KTestDeliverMultiRecipientsMixed_1,"Multi (2) recipients, 1 deliver ok, 1 deliver failure");
_LIT(KTestDeliverMultiRecipientsMixed_2,"Multi (2) recipients, 2 deliver failures");

#if defined (__WINS__)
_LIT(KPddName,"ECDRV");
_LIT(KLddName,"ECOMM");
#else	//__WINS__
_LIT(KPddSirName,"ESIR");
_LIT(KPddUartName,"ECUART4");
_LIT(KLddName,"ECOMM");
#endif	//__WINS__

RTest test(_L("T_SmsDeliver Test Harness"));

const TTimeIntervalMicroSeconds32 KWaitForWatchersToStart = 20000000;
const TTimeIntervalMicroSeconds32 KWaitForWatchersToStartDisplayInterval = 5000000;

LOCAL_C void DoTestsL()
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
	
	// Create and install the active scheduler
 	CActiveScheduler* scheduler= new (ELeave) CActiveScheduler();
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	// Create the test engine
	CTestEngine* testEngine = CTestEngine::NewL();
	CleanupStack::PushL(testEngine);

	// Do tests...
	testEngine->DoTestsL();
	
	CleanupStack::PopAndDestroy(2, scheduler);	
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	CTrapCleanup* tc=CTrapCleanup::New();
	TRAPD(err,DoTestsL());
	if (err!=KErrNone)
		User::Panic(KFailed, err);
	delete tc;
	test.Close();
	__UHEAP_MARKEND;
	return KErrNone;
	}
	
/*
 *	CTestEngine
 */
	
CTestEngine* CTestEngine::NewL()
	{
	CTestEngine* self = new (ELeave) CTestEngine();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestEngine::~CTestEngine()
	{
	delete iTestUtils;
	delete iWatchers;

	iSocket.Close();
	iSocketServ.Close();
	}

CTestEngine::CTestEngine()
: CBase()
	{
	}
	
void CTestEngine::DoTestsL()
	{

	CTestDeliverBase* testCase = NULL;
	
	testCase = CTestDeliverOneRecipientOk::NewL(*iTestUtils);
	RunTestCaseL(*testCase);

	testCase = CTestDeliverMultiRecipientsOk_1::NewL(*iTestUtils);
	RunTestCaseL(*testCase);

	testCase = CTestDeliverMultiRecipientsOk_2::NewL(*iTestUtils);
	RunTestCaseL(*testCase);

	testCase = CTestDeliverMultiRecipientsOk_3::NewL(*iTestUtils);
	RunTestCaseL(*testCase);

	testCase = CTestDeliverNoMatching::NewL(*iTestUtils);
	RunTestCaseL(*testCase);
	
	testCase = CTestDeliverNoStatusReports::NewL(*iTestUtils);
	RunTestCaseL(*testCase);

	testCase = CTestDeliverNotSent::NewL(*iTestUtils);
	RunTestCaseL(*testCase);

	testCase = CTestDeliverOneRecipientFail::NewL(*iTestUtils);
	RunTestCaseL(*testCase);
	
	testCase = 	CTestDeliverMultiRecipientsFail_1::NewL(*iTestUtils);
	RunTestCaseL(*testCase);

	testCase = 	CTestDeliverMultiRecipientsMixed_1::NewL(*iTestUtils);
	RunTestCaseL(*testCase);

	testCase = 	CTestDeliverMultiRecipientsMixed_2::NewL(*iTestUtils);
	RunTestCaseL(*testCase);
	}
	
void CTestEngine::RunTestCaseL(CTestDeliverBase& aTestCase)
	{
	CleanupStack::PushL(&aTestCase);
	aTestCase.Start();
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(&aTestCase);
	}

void CTestEngine::ConstructL()
	{
	// This should create the SMS service, including the service settings.
	iTestUtils = CSmsTestUtils::NewL(test);
	
	// Notify the system agent that the phone is on.
	iTestUtils->NotifySaPhoneOnL();
	
	// Tell the tests utils to log to file
	iTestUtils->SetLogToFile();
	
	// Restore the service settings.
	iTestUtils->SetEntryL(iTestUtils->iSmsServiceId);
	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iTestUtils->iServiceSettings);
	
	// Remove the default service centres from the service settings
	TInt count = iTestUtils->iServiceSettings->ServiceCenterCount();
	while( count-- )
		{
		iTestUtils->iServiceSettings->RemoveServiceCenter(count);
		}
		
	// Add the Vodafone service centre - store the settings.
	iTestUtils->iServiceSettings->AddServiceCenterL(_L("Vodafone"), _L("+447785016005"));

	// Set delivery options - do matching and make reports visible.	
	iTestUtils->iServiceSettings->SetDeliveryReport(ETrue);
	iTestUtils->iServiceSettings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisibleAndMatch);

	account->SaveSettingsL(*iTestUtils->iServiceSettings);
	CleanupStack::PopAndDestroy(account);
	
	// Create a session with the SMS stack - note order of the next few things
	// is important.
	User::LeaveIfError(iSocketServ.Connect());
	TProtocolDesc protoinfo;
	TProtocolName protocolname(KSmsDatagram);
	User::LeaveIfError(iSocketServ.FindProtocol(protocolname,protoinfo));
	User::LeaveIfError(iSocket.Open(iSocketServ,protoinfo.iAddrFamily,protoinfo.iSockType,protoinfo.iProtocol));
	
	TSmsAddr smsaddr;
	smsaddr.SetSmsAddrFamily(ESmsAddrSendOnly);
	User::LeaveIfError(iSocket.Bind(smsaddr));
	
	// Initialise phone...
	iTestUtils->WaitForInitializeL();
	
	// Start watchers...
	StartWatcherL();
	}
	
TBool CTestEngine::WatchersAlreadyRunningL()
	{
	RSocketServ server;
	RSocket socket;

	User::LeaveIfError(server.Connect());
	CleanupClosePushL(server);

		// test if it's there... 
	TProtocolDesc protoInfo;
	TProtocolName protocolname;
	protocolname.Copy(KSmsDatagram);
	User::LeaveIfError(server.FindProtocol(protocolname,protoInfo));

	User::LeaveIfError(socket.Open(server,
										protoInfo.iAddrFamily,
										protoInfo.iSockType, 
										protoInfo.iProtocol));
	CleanupClosePushL(socket);

	TSmsAddr addr;
	addr.SetSmsAddrFamily(ESmsAddrRecvAny);
	TInt err = socket.Bind(addr);

	CleanupStack::Pop(2, &server);
	socket.CancelAll();
	socket.Close();
	server.Close();
	return (err == KErrAlreadyExists);
	}
	
void CTestEngine::StartWatcherL()
	{
	iTestUtils->Printf(_L("Start the Watcher"));

	if( WatchersAlreadyRunningL() )
		{
		iTestUtils->Printf(_L("Watchers are already running\n"));
		return;
		}

	iWatchers = CTestUtilsWatcherStarter::NewL();

	TTimeIntervalMicroSeconds32 wait = KWaitForWatchersToStart;
	TBool started = EFalse;
	while( !started && wait.Int() > 0 )
		{
		iTestUtils->Printf(_L("Waiting %d secs for watchers to start...\n"), wait.Int() / 1000000);
		wait = wait.Int() - KWaitForWatchersToStartDisplayInterval.Int();
		User::After(KWaitForWatchersToStartDisplayInterval);
		started = WatchersAlreadyRunningL();
		}

	if( !WatchersAlreadyRunningL() )
		{
		iTestUtils->Printf(_L("WARNING: NBS Watcher has not started yet\n"));
		}
	}
	
/*
 * CTestDeliverBase
 */
 
CTestDeliverBase::~CTestDeliverBase()
	{
	delete iEntryForObserver;
	delete iOperation;
	}	
	
void CTestDeliverBase::Start()
	{
	iTestUtils.Printf(_L("Starting %S\n"), &TestName());
	iState = EClearFolders;
	CompleteSelf();
	}
	
CTestDeliverBase::CTestDeliverBase(CSmsTestUtils& aTestUtils)
: CActive(CActive::EPriorityStandard), iTestUtils(aTestUtils)
	{
	CActiveScheduler::Add(this);
	}
	
void CTestDeliverBase::ConstructL()
	{
	iEntryForObserver = Session().GetEntryL(KMsvGlobalInBoxIndexEntryId);
	}
	
void CTestDeliverBase::CompleteSelf()
	{
	TRequestStatus* pStat = &iStatus;
	User::RequestComplete(pStat, KErrNone);
	SetActive();
	}
	
CMsvSession& CTestDeliverBase::Session()
	{
	return *iTestUtils.iMsvSession;
	}

CMsvEntry& CTestDeliverBase::MsvEntry()
	{
	return *iTestUtils.iMsvEntry;
	}

CSmsClientMtm& CTestDeliverBase::Client()
	{
	return *iTestUtils.iSmsClientMtm;
	}
	
CSmsSettings& CTestDeliverBase::ServiceSettings()
	{
	return *iTestUtils.iServiceSettings;
	}
		
void CTestDeliverBase::ClearFoldersL()
	{
	iTestUtils.DeleteSmsMessagesL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils.DeleteSmsMessagesL(KMsvGlobalInBoxIndexEntryId);
	iTestUtils.DeleteSmsMessagesL(KMsvSentEntryId);		
	}
	
void CTestDeliverBase::TestComplete()
	{
	iTestUtils.Printf(_L("Test Complete\n"));
	CActiveScheduler::Stop();
	}
	
void CTestDeliverBase::CreateMessageL()
	{
	iTestUtils.Printf(_L("Creating message...\n"));
	
	TMsvEntry entry;
	entry.SetVisible(ETrue); 
	entry.SetInPreparation(ETrue); 
	entry.iServiceId = iTestUtils.iSmsServiceId; 
	entry.iType = KUidMsvMessageEntry; 
	entry.iMtm = KUidMsgTypeSMS; 
	entry.iDate.HomeTime(); 
	entry.iSize = 0; 
	entry.iDescription.Set(KNullDesC); 
	entry.iDetails.Set(KNullDesC); 

	// Check send options...
	TUint sendState = KMsvSendStateWaiting; 
	switch( ServiceSettings().Delivery() )  
		{ 
	case ESmsDeliveryScheduled: 
		sendState = KMsvSendStateScheduled; 
		break; 
	case ESmsDeliveryUponRequest: 
		sendState = KMsvSendStateUponRequest; 
		break; 
	default: 
		sendState = KMsvSendStateWaiting; 
		break;
		}
	entry.SetSendingState(sendState); 

	// Create the SMS header object...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);
	
	// Set the body text...
	iTestUtils.iRichText->Reset();
	iTestUtils.iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	header->SetSmsSettingsL(ServiceSettings()); 
	
	// Set the service centre
	TInt defaultIndex = ServiceSettings().DefaultServiceCenter();
	header->SetServiceCenterAddressL(ServiceSettings().GetServiceCenter(defaultIndex).Address());

	// Set recipient - ask derived class
	SetRecipientsL(*header);
	
	// Update entry description and details...
	CArrayPtrFlat<CSmsNumber>& recipient = header->Recipients();
	entry.iDetails.Set(recipient[0]->Address());
	entry.iDescription.Set(iTestUtils.iRichText->Read(0, ServiceSettings().DescriptionLength()));
	entry.SetInPreparation(EFalse);
	
	// Create the entry - set context to the global outbox.
	MsvEntry().SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	MsvEntry().CreateL(entry);

	// Create new store and save header information 
	MsvEntry().SetEntryL(entry.Id()); 
	CMsvStore* store = MsvEntry().EditStoreL(); 
	CleanupStack::PushL(store); 
	header->StoreL(*store);
	store->StoreBodyTextL(*iTestUtils.iRichText);
	store->CommitL(); 
	CleanupStack::PopAndDestroy(2, header); 
	
	iMessageId = entry.Id();
	}
	
void CTestDeliverBase::SendMessageL()
	{
	iTestUtils.Printf(_L("Sending message %d\n"), iMessageId);

	delete iOperation;
	iOperation = NULL;

	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sel);
	sel->AppendL(iMessageId);

	iTestUtils.DisplayMessageL(iMessageId);
	
	MsvEntry().SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iOperation = MsvEntry().CopyL(*sel, iTestUtils.iSmsServiceId, iStatus);
	CleanupStack::PopAndDestroy(sel);

	SetActive();
	}
	
void CTestDeliverBase::ChangeServiceSettingsL()
	{
	// Restore the service settings.
	iTestUtils.SetEntryL(iTestUtils.iSmsServiceId);
	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iTestUtils.iServiceSettings);
	
	// Set the service settings 
	iTestUtils.iServiceSettings->SetValidityPeriod(TTimeIntervalMinutes(5));
	iTestUtils.iServiceSettings->SetDeliveryReport(ETrue);
	iTestUtils.iServiceSettings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisibleAndMatch);
	
	// Store the new service settings
	account->SaveSettingsL(*iTestUtils.iServiceSettings);
	CleanupStack::PopAndDestroy(account);
	}
	
void CTestDeliverBase::CheckCreatedMessageL()
	{
	// Need to check the following - 
	// 1. All recipients have correct delivery status - ENoAckRequested.
	// 2. The index summary is correct - ENoAckSummary
	// 3. The index message IS is invalid.
	
	iTestUtils.Printf(_L("Checking created message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	TInt rcptCount = header->Recipients().Count();
	for( TInt ii = 0; ii < rcptCount; ++ii )
		{
		CSmsNumber* rcpt = header->Recipients()[ii];
		iTestUtils.Printf(_L("--Recipient %d - Delivery status -> %d - should be %d (ENoAckRequested)\n"), ii, rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
		iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);
		}
		
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (ENoAckSummary)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::ENoAckSummary);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::ENoAckSummary);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId == EFalse);
	
	CleanupStack::PopAndDestroy(header);
	}
	
TMsvId CTestDeliverBase::GetObserverEntry()
	{
	return iMessageId;
	}

// methods from CActive

void CTestDeliverBase::RunL()
	{
	User::LeaveIfError(iStatus.Int());
	
	switch( iState )
		{
	case EClearFolders:
		{
		ClearFoldersL();
		iState = EChangeServiceSettings;
		CompleteSelf();
		} break;
	case EChangeServiceSettings:
		{
		ChangeServiceSettingsL();
		iState = ECreateMessage;
		CompleteSelf();
		} break;			
	case ECreateMessage:
		{
		CreateMessageL();
		iState = ECheckCreatedMessage;
		CompleteSelf();
		} break;
	case ECheckCreatedMessage:
		{
		CheckCreatedMessageL();
		iState = ESendMessage;
		CompleteSelf();
		} break;
	case ESendMessage:
		{
		SendMessageL();
		iState = ECheckSentMessage;
		} break;
	case ECheckSentMessage:
		{
		CheckSentMessageL();
		
		TMsvId entryId = GetObserverEntry();
		
		if( entryId != KMsvNullIndexEntryId )
			{
			// Need to observe something - move observer entry to specified entry 
			// and wait for it to be updated
			iEntryForObserver->SetEntryL(entryId);
			iEntryForObserver->AddObserverL(*this);
			
			iState = EPendingDeliveryReport;
			}
		else
			{
			// Not observing anything - end of test.
			TestComplete();
			}
		} break;
	case EPendingDeliveryReport:
		{
		// End the test...
		TestComplete();
		} break;			
	default:
		User::Invariant();
		break;
		}
	}
	
void CTestDeliverBase::DoCancel()
	{
	}
	
TInt CTestDeliverBase::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}
	
//	methods from MMsvEntryObserver	
	
void CTestDeliverBase::HandleEntryEventL(TMsvEntryEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	__ASSERT_DEBUG( iState == EPendingDeliveryReport, User::Invariant() );
	
	if( aEvent == EMsvEntryChanged )
		{
		iTestUtils.Printf(_L("Message entry has been updated\n"));

		// Check that the sent message has been updated.
		CheckDeliveredMessageL();
		}
	}
	
/*
 * CTestDeliverOneRecipientOk
 */
 
 CTestDeliverOneRecipientOk* CTestDeliverOneRecipientOk::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverOneRecipientOk* self = new (ELeave) CTestDeliverOneRecipientOk(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverOneRecipientOk::~CTestDeliverOneRecipientOk()
	{
	}
	
CTestDeliverOneRecipientOk::CTestDeliverOneRecipientOk(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
		
//	 methods from CTestDeliverBase

void CTestDeliverOneRecipientOk::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting single recipient\n"));
	
	CSmsNumber* rcpt = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt);
	rcpt->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt);
	CleanupStack::Pop(rcpt);
	}

void CTestDeliverOneRecipientOk::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 1);
	iTestUtils.Test()(header->Recipients().Count() == 1);
	
	CSmsNumber* rcpt = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId != EFalse);
		
	iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, rcpt->LogId());
	iTestUtils.Test()(logId == rcpt->LogId());
	
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverOneRecipientOk::CheckDeliveredMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status correctly set
	// 3. The index summary is correctly set
	
	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 1);
	iTestUtils.Test()(header->Recipients().Count() == 1);
	
	CSmsNumber* rcpt = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
	iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EAllSuccessful)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EAllSuccessful);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EAllSuccessful);
	
	// Test is no ended - complete self to stop.
	CompleteSelf();		
	
	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverOneRecipientOk::TestName()
	{
	return KTestDeliverOneRecipientOk();
	}
	
/*
 * CTestDeliverOneRecipientFail
 */
 
 CTestDeliverOneRecipientFail* CTestDeliverOneRecipientFail::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverOneRecipientFail* self = new (ELeave) CTestDeliverOneRecipientFail(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverOneRecipientFail::~CTestDeliverOneRecipientFail()
	{
	}
	
CTestDeliverOneRecipientFail::CTestDeliverOneRecipientFail(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
		
//	 methods from CTestDeliverBase

void CTestDeliverOneRecipientFail::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting single recipient\n"));
	
	CSmsNumber* rcpt = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt);
	rcpt->SetAddressL(KRecipientFail);
	aHeader.Recipients().AppendL(rcpt);
	CleanupStack::Pop(rcpt);
	}
	
void CTestDeliverOneRecipientFail::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 1);
	iTestUtils.Test()(header->Recipients().Count() == 1);
	
	CSmsNumber* rcpt = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId != EFalse);
		
	iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, rcpt->LogId());
	iTestUtils.Test()(logId == rcpt->LogId());
	
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverOneRecipientFail::CheckDeliveredMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status correctly set
	// 3. The index summary is correctly set
	
 	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 1);
	iTestUtils.Test()(header->Recipients().Count() == 1);
	
	CSmsNumber* rcpt = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Delivery status -> %d - should be %d (EAckError)\n"), rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
	iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);
	
	iTestUtils.Printf(_L("- Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt->Status(), CMsvRecipient::EFailedToSend);
	iTestUtils.Test()(rcpt->Status()==CMsvRecipient::EFailedToSend);

	iTestUtils.Printf(_L("- Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
	iTestUtils.Test()(rcpt->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EAllFailed)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EAllFailed);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EAllFailed);
	
	// Test is no ended - complete self to stop.
	CompleteSelf();		
	
	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverOneRecipientFail::TestName()
	{
	return KTestDeliverOneRecipientFail();
	}
	
/*
 * CTestDeliverMultiRecipientsOk_1
 */
 
CTestDeliverMultiRecipientsOk_1* CTestDeliverMultiRecipientsOk_1::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverMultiRecipientsOk_1* self = new (ELeave) CTestDeliverMultiRecipientsOk_1(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverMultiRecipientsOk_1::~CTestDeliverMultiRecipientsOk_1()
	{
	}
	
CTestDeliverMultiRecipientsOk_1::CTestDeliverMultiRecipientsOk_1(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
	
//	methods from CTestDeliverBase

void CTestDeliverMultiRecipientsOk_1::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting multiple (2) recipients\n"));
	
	CSmsNumber* rcpt0 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt0);
	rcpt0->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt0);
	CleanupStack::Pop(rcpt0);
	
	CSmsNumber* rcpt1 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt1);
	rcpt1->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt1);
	CleanupStack::Pop(rcpt1);
	}
	
void CTestDeliverMultiRecipientsOk_1::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipients have delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	CSmsNumber* rcpt1 = header->Recipients()[1];
	
	iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);	
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId == EFalse);
		
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverMultiRecipientsOk_1::CheckDeliveredMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status is correctly set
	// 3. The index summary is correctly set

	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set - round %d...\n"), iMessageId, ++iCheckCount);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	CSmsNumber* rcpt1 = header->Recipients()[1];
	
	TMsvSmsEntry entry = MsvEntry().Entry();

	if( iCheckCount == 1 )
		{
		// At least one of the recipients has been delivered to ok - check the 
		// status of recipient 0 and make decisions based on that.
		CSmsNumber* pendingRcpt = NULL;
		
		if( rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck )
			{
			// Recipient 1 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
			
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
			
			pendingRcpt = rcpt0;
			}
		else
			{
			// Recipient 0 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
			
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
			
			pendingRcpt = rcpt1;
			}
		TLogId logId = 0;	
		TBool validId = entry.MessageId(logId);
		
		iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
		iTestUtils.Test()(validId != EFalse);
			
		iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, pendingRcpt->LogId());
		iTestUtils.Test()(logId == pendingRcpt->LogId());
		}
	else
		{
		// Both recipients has been delivered to ok - check the status of both
		// recipients
		iTestUtils.Printf(_L("--Second check of flags -> both recipients have been delivered to\n"));
		
		iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
		iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
	
		iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
		iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
		
		iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EAllSuccessful)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EAllSuccessful);
		iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EAllSuccessful);

		// Test is no ended - complete self to stop.
		CompleteSelf();		
		}
	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverMultiRecipientsOk_1::TestName()
	{
	return KTestDeliverMultiRecipientsOk_1();
	}
	
/*
 * CTestDeliverMultiRecipientsOk_2
 */
 
CTestDeliverMultiRecipientsOk_2* CTestDeliverMultiRecipientsOk_2::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverMultiRecipientsOk_2* self = new (ELeave) CTestDeliverMultiRecipientsOk_2(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverMultiRecipientsOk_2::~CTestDeliverMultiRecipientsOk_2()
	{
	}
	
CTestDeliverMultiRecipientsOk_2::CTestDeliverMultiRecipientsOk_2(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
	
//	methods from CTestDeliverBase

void CTestDeliverMultiRecipientsOk_2::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting multiple (2) recipients\n"));
	
	CSmsNumber* rcpt0 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt0);
	rcpt0->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt0);
	CleanupStack::Pop(rcpt0);
	
	CSmsNumber* rcpt1 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt1);
	rcpt1->SetAddressL(KRecipientBad);
	aHeader.Recipients().AppendL(rcpt1);
	CleanupStack::Pop(rcpt1);
	}

void CTestDeliverMultiRecipientsOk_2::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipients have delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	CSmsNumber* rcpt1 = header->Recipients()[1];
	
	iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (ENoAckRequested)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
	iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);	
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId != EFalse);
		
	iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, rcpt0->LogId());
	iTestUtils.Test()(logId == rcpt0->LogId());
		
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverMultiRecipientsOk_2::CheckDeliveredMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status is correctly set
	// 3. The index summary is correctly set

	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	CSmsNumber* rcpt1 = header->Recipients()[1];
	
	TMsvSmsEntry entry = MsvEntry().Entry();

	iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
	iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
	
	iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (ENoAckRequested)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
	iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EAllSuccessful)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EAllSuccessful);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EAllSuccessful);

	// Test is no ended - complete self to stop.
	CompleteSelf();		

	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverMultiRecipientsOk_2::TestName()
	{
	return KTestDeliverMultiRecipientsOk_2();
	}
	
/*
 * CTestDeliverMultiRecipientsOk_3
 */
 
CTestDeliverMultiRecipientsOk_3* CTestDeliverMultiRecipientsOk_3::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverMultiRecipientsOk_3* self = new (ELeave) CTestDeliverMultiRecipientsOk_3(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverMultiRecipientsOk_3::~CTestDeliverMultiRecipientsOk_3()
	{
	}
	
CTestDeliverMultiRecipientsOk_3::CTestDeliverMultiRecipientsOk_3(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
	
//	methods from CTestDeliverBase

void CTestDeliverMultiRecipientsOk_3::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting multiple (3) recipients\n"));
	
	CSmsNumber* rcpt0 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt0);
	rcpt0->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt0);
	CleanupStack::Pop(rcpt0);
	
	CSmsNumber* rcpt1 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt1);
	rcpt1->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt1);
	CleanupStack::Pop(rcpt1);

	CSmsNumber* rcpt2 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt2);
	rcpt2->SetAddressL(KRecipientBad);
	aHeader.Recipients().AppendL(rcpt2);
	CleanupStack::Pop(rcpt2);
	}

void CTestDeliverMultiRecipientsOk_3::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipients have delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 3);
	iTestUtils.Test()(header->Recipients().Count() == 3);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	CSmsNumber* rcpt1 = header->Recipients()[1];
	
	iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);	
	
	CSmsNumber* rcpt2 = header->Recipients()[2];
	
	iTestUtils.Printf(_L("--Recipient 2 - Delivery status -> %d - should be %d (ENoAckRequested)\n"), rcpt2->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
	iTestUtils.Test()(rcpt2->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);

	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId == EFalse);
		
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverMultiRecipientsOk_3::CheckDeliveredMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status is correctly set
	// 3. The index summary is correctly set

	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set - round %d...\n"), iMessageId, ++iCheckCount);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 3);
	iTestUtils.Test()(header->Recipients().Count() == 3);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	CSmsNumber* rcpt1 = header->Recipients()[1];
	CSmsNumber* rcpt2 = header->Recipients()[2];
	
	TMsvSmsEntry entry = MsvEntry().Entry();
		
	if( iCheckCount == 1 )
		{
		// At least one of the recipients has been delivered to ok - check the 
		// status of recipient 0 and make decisions based on that.
		CSmsNumber* pendingRcpt = NULL;
		
		if( rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck )
			{
			// Recipient 2 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
			
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
			
			iTestUtils.Printf(_L("--Recipient 2 - Delivery status -> %d - should be %d (ENoAckRequested)\n"), rcpt2->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
			iTestUtils.Test()(rcpt2->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);
			
			pendingRcpt = rcpt0;
			}
		else
			{
			// Recipient 0 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
			
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
			
			iTestUtils.Printf(_L("--Recipient 2 - Delivery status -> %d - should be %d (ENoAckRequested)\n"), rcpt2->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
			iTestUtils.Test()(rcpt2->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);
			
			pendingRcpt = rcpt1;
			}
		TLogId logId = 0;	
		TBool validId = entry.MessageId(logId);
		
		iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
		iTestUtils.Test()(validId != EFalse);
			
		iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, pendingRcpt->LogId());
		iTestUtils.Test()(logId == pendingRcpt->LogId());
		}
	else
		{
		// Both recipients has been delivered to ok - check the status of both
		// recipients
		iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
		iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
		
		iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
		iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
		
		iTestUtils.Printf(_L("--Recipient 2 - Delivery status -> %d - should be %d (ENoAckRequested)\n"), rcpt2->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
		iTestUtils.Test()(rcpt2->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);

		iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EAllSuccessful)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EAllSuccessful);
		iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EAllSuccessful);

		// Test is no ended - complete self to stop.
		CompleteSelf();		
		}
	
	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverMultiRecipientsOk_3::TestName()
	{
	return KTestDeliverMultiRecipientsOk_3();
	}
	
/*
 * CTestDeliverNoMatching
 */
 
 CTestDeliverNoMatching* CTestDeliverNoMatching::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverNoMatching* self = new (ELeave) CTestDeliverNoMatching(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverNoMatching::~CTestDeliverNoMatching()
	{
	}
	
CTestDeliverNoMatching::CTestDeliverNoMatching(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
	
//	methods from MMsvEntryObserver	
	
void CTestDeliverNoMatching::HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	if( aEvent == EMsvNewChildren )
		{
		iTestUtils.Printf(_L("Received new message in Inbox - should be status report...\n"));
		
		CMsvEntrySelection* sel = static_cast<CMsvEntrySelection*>(aArg1);

		// Check how many mesasges there are - should be just 1
		TInt count = sel->Count();

		iTestUtils.Printf(_L("--Number of new messages -> %d - should be %d\n"), count, 1);
		iTestUtils.Test()(count == 1);
		
		// Restore the message

		CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
		CleanupStack::PushL(header);

		MsvEntry().SetEntryL((*sel)[0]);
		CMsvStore* store = MsvEntry().ReadStoreL();
		CleanupStack::PushL(store);

		header->RestoreL(*store);
		iTestUtils.iRichText->Reset();
		store->RestoreBodyTextL(*iTestUtils.iRichText);

		CleanupStack::PopAndDestroy(store);
		
		// Check that it's a Status Report...
		iTestUtils.Printf(_L("--Message type -> %d - should be %d\n"), header->Type(), CSmsPDU::ESmsStatusReport);
		iTestUtils.Test()(header->Type() == CSmsPDU::ESmsStatusReport);
		
		// Check the message reference
		TLogId logId = header->Message().LogServerId();
		iTestUtils.Printf(_L("--Log Server Id -> %d - should be %d\n"), logId, iLogId);
		iTestUtils.Test()(logId == iLogId);
		
		// Check the deliver status
		CSmsStatusReport& report = header->StatusReport();
		iTestUtils.Printf(_L("--Deliver status -> %d - should be %d (ESmsShortMessageReceivedBySME)\n"), report.Status(), TSmsStatus::ESmsShortMessageReceivedBySME);
		iTestUtils.Test()(report.Status() == TSmsStatus::ESmsShortMessageReceivedBySME);
		
		CleanupStack::PopAndDestroy(header);
		
		// Check that the sent message has been updated.
		CheckDeliveredMessageL();
		}
	}
				
//	 methods from CTestDeliverBase

void CTestDeliverNoMatching::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting single recipient\n"));
	
	CSmsNumber* rcpt = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt);
	rcpt->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt);
	CleanupStack::Pop(rcpt);
	}

void CTestDeliverNoMatching::ChangeServiceSettingsL()
	{
	// Restore the service settings.
	iTestUtils.SetEntryL(iTestUtils.iSmsServiceId);
	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iTestUtils.iServiceSettings);
	
	// Set the service settings 
	iTestUtils.iServiceSettings->SetValidityPeriod(TTimeIntervalMinutes(5));
	iTestUtils.iServiceSettings->SetDeliveryReport(ETrue);
	iTestUtils.iServiceSettings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
	
	// Store the new service settings
	account->SaveSettingsL(*iTestUtils.iServiceSettings);
	CleanupStack::PopAndDestroy(account);
	}
	
TMsvId CTestDeliverNoMatching::GetObserverEntry()
	{
	return KMsvGlobalInBoxIndexEntryId;
	}

void CTestDeliverNoMatching::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 1);
	iTestUtils.Test()(header->Recipients().Count() == 1);
	
	CSmsNumber* rcpt = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId != EFalse);
		
	iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, rcpt->LogId());
	iTestUtils.Test()(logId == rcpt->LogId());
	
	iLogId = rcpt->LogId();
	
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverNoMatching::CheckDeliveredMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status correctly set
	// 3. The index summary is correctly set
	
	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 1);
	iTestUtils.Test()(header->Recipients().Count() == 1);
	
	CSmsNumber* rcpt = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	// Test is no ended - complete self to stop.
	CompleteSelf();		
	
	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverNoMatching::TestName()
	{
	return KTestDeliverNoMatching();
	}
	
/*
 * CTestDeliverNoStatusReports
 */
 
 CTestDeliverNoStatusReports* CTestDeliverNoStatusReports::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverNoStatusReports* self = new (ELeave) CTestDeliverNoStatusReports(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverNoStatusReports::~CTestDeliverNoStatusReports()
	{
	}
	
CTestDeliverNoStatusReports::CTestDeliverNoStatusReports(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
				
//	 methods from CTestDeliverBase

void CTestDeliverNoStatusReports::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting single recipient\n"));
	
	CSmsNumber* rcpt = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt);
	rcpt->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt);
	CleanupStack::Pop(rcpt);
	}

void CTestDeliverNoStatusReports::ChangeServiceSettingsL()
	{
	// Restore the service settings.
	iTestUtils.SetEntryL(iTestUtils.iSmsServiceId);
	CSmsAccount* account = CSmsAccount::NewLC();
	account->LoadSettingsL(*iTestUtils.iServiceSettings);
	
	// Set the service settings 
	iTestUtils.iServiceSettings->SetValidityPeriod(TTimeIntervalMinutes(5));
	iTestUtils.iServiceSettings->SetDeliveryReport(EFalse);
	iTestUtils.iServiceSettings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisibleAndMatch);
	
	// Store the new service settings
	account->SaveSettingsL(*iTestUtils.iServiceSettings);
	CleanupStack::PopAndDestroy(account);
	}
	
TMsvId CTestDeliverNoStatusReports::GetObserverEntry()
	{
	return KMsvNullIndexEntryId;
	}

void CTestDeliverNoStatusReports::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 1);
	iTestUtils.Test()(header->Recipients().Count() == 1);
	
	CSmsNumber* rcpt = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Delivery status -> %d - should be %d (ENoAckRequested)\n"), rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
	iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (ENoAckSummary)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::ENoAckSummary);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::ENoAckSummary);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId == EFalse);
		
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverNoStatusReports::CheckDeliveredMessageL()
	{
	User::Invariant();
	}

const TDesC& CTestDeliverNoStatusReports::TestName()
	{
	return KTestDeliverNoStatusReports();
	}
	
/*
 * CTestDeliverNotSent
 */
 
 CTestDeliverNotSent* CTestDeliverNotSent::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverNotSent* self = new (ELeave) CTestDeliverNotSent(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverNotSent::~CTestDeliverNotSent()
	{
	}
	
CTestDeliverNotSent::CTestDeliverNotSent(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
				
//	 methods from CTestDeliverBase

void CTestDeliverNotSent::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting single recipient\n"));
	
	CSmsNumber* rcpt = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt);
	rcpt->SetAddressL(KRecipientBad);
	aHeader.Recipients().AppendL(rcpt);
	CleanupStack::Pop(rcpt);
	}

TMsvId CTestDeliverNotSent::GetObserverEntry()
	{
	return KMsvNullIndexEntryId;
	}

void CTestDeliverNotSent::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 1);
	iTestUtils.Test()(header->Recipients().Count() == 1);
	
	CSmsNumber* rcpt = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Delivery status -> %d - should be %d (ENoAckRequested)\n"), rcpt->AckStatus(ESmsAckTypeDelivery), CSmsNumber::ENoAckRequested);
	iTestUtils.Test()(rcpt->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::ENoAckRequested);
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (ENoAckSummary)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::ENoAckSummary);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::ENoAckSummary);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId == EFalse);
		
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverNotSent::CheckDeliveredMessageL()
	{
	User::Invariant();
	}

const TDesC& CTestDeliverNotSent::TestName()
	{
	return KTestDeliverNotSent();
	}

/*
 * CTestDeliverMultiRecipientsFail_1
 */
 
CTestDeliverMultiRecipientsFail_1* CTestDeliverMultiRecipientsFail_1::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverMultiRecipientsFail_1* self = new (ELeave) CTestDeliverMultiRecipientsFail_1(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverMultiRecipientsFail_1::~CTestDeliverMultiRecipientsFail_1()
	{
	}
	
CTestDeliverMultiRecipientsFail_1::CTestDeliverMultiRecipientsFail_1(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
	
//	methods from CTestDeliverBase

void CTestDeliverMultiRecipientsFail_1::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting multiple (3) recipients\n"));
	
	CSmsNumber* rcpt0 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt0);
	rcpt0->SetAddressL(KRecipientFail);
	aHeader.Recipients().AppendL(rcpt0);
	CleanupStack::Pop(rcpt0);
	
	CSmsNumber* rcpt1 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt1);
	rcpt1->SetAddressL(KRecipientFail);
	aHeader.Recipients().AppendL(rcpt1);
	CleanupStack::Pop(rcpt1);
	}
	
void CTestDeliverMultiRecipientsFail_1::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipients have delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	CSmsNumber* rcpt1 = header->Recipients()[1];
	
	iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);	
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId == EFalse);
		
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverMultiRecipientsFail_1::CheckDeliveredMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status is correctly set
	// 3. The index summary is correctly set

	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set - round %d...\n"), iMessageId, ++iCheckCount);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	CSmsNumber* rcpt1 = header->Recipients()[1];
	TMsvSmsEntry entry = MsvEntry().Entry();
		
	if( iCheckCount == 1 )
		{
		// At least one of the recipients has been delivered to ok - check the 
		// status of recipient 0 and make decisions based on that.
		CSmsNumber* pendingRcpt = NULL;
		
		if( rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck )
			{
			// Recipient 2 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
			
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);
			
			iTestUtils.Printf(_L("--Recipient 1 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt1->Status(), CMsvRecipient::EFailedToSend);
			iTestUtils.Test()(rcpt1->Status()==CMsvRecipient::EFailedToSend);

			iTestUtils.Printf(_L("--Recipient 1 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt1->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
			iTestUtils.Test()(rcpt1->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
			
			pendingRcpt = rcpt0;
			}
		else
			{
			// Recipient 0 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);
			
			iTestUtils.Printf(_L("--Recipient 0 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt0->Status(), CMsvRecipient::EFailedToSend);
			iTestUtils.Test()(rcpt0->Status()==CMsvRecipient::EFailedToSend);
			
			iTestUtils.Printf(_L("--Recipient 0 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt0->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
			iTestUtils.Test()(rcpt0->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);				
		
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
			
			pendingRcpt = rcpt1;
			}
		TLogId logId = 0;	
		TBool validId = entry.MessageId(logId);
		
		iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
		iTestUtils.Test()(validId != EFalse);
			
		iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, pendingRcpt->LogId());
		iTestUtils.Test()(logId == pendingRcpt->LogId());
		}
	else
		{
		// Both recipients has been delivered to ok - check the status of both
		// recipients
		iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
		iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);
		
		iTestUtils.Printf(_L("--Recipient 0 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt0->Status(), CMsvRecipient::EFailedToSend);
		iTestUtils.Test()(rcpt0->Status()==CMsvRecipient::EFailedToSend);
	
		iTestUtils.Printf(_L("--Recipient 0 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt0->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
		iTestUtils.Test()(rcpt0->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
		
		iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
		iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);
		
		iTestUtils.Printf(_L("--Recipient 1 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt1->Status(), CMsvRecipient::EFailedToSend);
		iTestUtils.Test()(rcpt1->Status()==CMsvRecipient::EFailedToSend);
				
		iTestUtils.Printf(_L("--Recipient 1 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt1->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
		iTestUtils.Test()(rcpt1->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
		
		iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EAllFailed)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EAllFailed);
		iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EAllFailed);

		// Test is no ended - complete self to stop.
		CompleteSelf();		
		}
	
	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverMultiRecipientsFail_1::TestName()
	{
	return KTestDeliverMultiRecipientsFail_1();
	}

/*
 * CTestDeliverMultiRecipientsMixed_1
 */
 
CTestDeliverMultiRecipientsMixed_1* CTestDeliverMultiRecipientsMixed_1::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverMultiRecipientsMixed_1* self = new (ELeave) CTestDeliverMultiRecipientsMixed_1(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}
		
CTestDeliverMultiRecipientsMixed_1::~CTestDeliverMultiRecipientsMixed_1()
	{
	}
	
CTestDeliverMultiRecipientsMixed_1::CTestDeliverMultiRecipientsMixed_1(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}
	
//	methods from CTestDeliverBase

void CTestDeliverMultiRecipientsMixed_1::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting multiple (3) recipients\n"));
	
	CSmsNumber* rcpt0 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt0);
	rcpt0->SetAddressL(KRecipientFail);
	aHeader.Recipients().AppendL(rcpt0);
	CleanupStack::Pop(rcpt0);
	
	CSmsNumber* rcpt1 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt1);
	rcpt1->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt1);
	CleanupStack::Pop(rcpt1);
	}
	
void CTestDeliverMultiRecipientsMixed_1::CheckSentMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipients have delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient
	
	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	
	iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
	
	CSmsNumber* rcpt1 = header->Recipients()[1];
	
	iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);	
	
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);
	
	TLogId logId = 0;	
	TBool validId = entry.MessageId(logId);
	
	iTestUtils.Printf(_L("--Index message Id validity -> %d - should be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId == EFalse);
		
	CleanupStack::PopAndDestroy(header);
	}
	
void CTestDeliverMultiRecipientsMixed_1::CheckDeliveredMessageL()
	{
	// Need to check the following - 
	// 1. Correct number of recipients
	// 2. The recipient has delivery status is correctly set
	// 3. The index summary is correctly set

	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set - round %d...\n"), iMessageId, ++iCheckCount);
		
	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);
	
	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);
	
	CSmsNumber* rcpt0 = header->Recipients()[0];
	CSmsNumber* rcpt1 = header->Recipients()[1];
	
	TMsvSmsEntry entry = MsvEntry().Entry();
		
	if( iCheckCount == 1 )
		{
		// At least one of the recipients has been delivered to ok - check the 
		// status of recipient 0 and make decisions based on that.
		CSmsNumber* pendingRcpt = NULL;
		
		if( rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck )
			{
			// Recipient 2 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
			
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
			
			pendingRcpt = rcpt0;
			}
		else
			{
			// Recipient 0 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);
			
			iTestUtils.Printf(_L("--Recipient 0 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt0->Status(), CMsvRecipient::EFailedToSend);
			iTestUtils.Test()(rcpt0->Status()==CMsvRecipient::EFailedToSend);

			iTestUtils.Printf(_L("--Recipient 0 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt0->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
			iTestUtils.Test()(rcpt0->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
			
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);
			
			pendingRcpt = rcpt1;
			}
		TLogId logId = 0;	
		TBool validId = entry.MessageId(logId);
		
		iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
		iTestUtils.Test()(validId != EFalse);
			
		iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, pendingRcpt->LogId());
		iTestUtils.Test()(logId == pendingRcpt->LogId());
		}
	else
		{
		// Status-reports for both recipients have been received - check the 
		// status of both recipients.
		iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
		iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);
		
		iTestUtils.Printf(_L("--Recipient 0 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt0->Status(), CMsvRecipient::EFailedToSend);
		iTestUtils.Test()(rcpt0->Status()==CMsvRecipient::EFailedToSend);

		iTestUtils.Printf(_L("--Recipient 0 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt0->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
		iTestUtils.Test()(rcpt0->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
		
		iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckSuccessful)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckSuccessful);
		iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckSuccessful);
		
		iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EMixed)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EMixed);
		iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EMixed);

		// Test is no ended - complete self to stop.
		CompleteSelf();		
		}
	
	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverMultiRecipientsMixed_1::TestName()
	{
	return KTestDeliverMultiRecipientsMixed_1();
	}

/*
 * CTestDeliverMultiRecipientsMixed_2
 */

CTestDeliverMultiRecipientsMixed_2* CTestDeliverMultiRecipientsMixed_2::NewL(CSmsTestUtils& aTestUtils)
	{
	CTestDeliverMultiRecipientsMixed_2* self = new (ELeave) CTestDeliverMultiRecipientsMixed_2(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestDeliverMultiRecipientsMixed_2::~CTestDeliverMultiRecipientsMixed_2()
	{
	}

CTestDeliverMultiRecipientsMixed_2::CTestDeliverMultiRecipientsMixed_2(CSmsTestUtils& aTestUtils)
: CTestDeliverBase(aTestUtils)
	{
	}

//	methods from CTestDeliverBase

void CTestDeliverMultiRecipientsMixed_2::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils.Printf(_L("--Setting multiple (2) recipients\n"));

	CSmsNumber* rcpt0 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt0);
	rcpt0->SetAddressL(KRecipientFail);
	aHeader.Recipients().AppendL(rcpt0);
	CleanupStack::Pop(rcpt0);

	CSmsNumber* rcpt1 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt1);
	rcpt1->SetAddressL(KRecipientFail);
	aHeader.Recipients().AppendL(rcpt1);
	CleanupStack::Pop(rcpt1);
	}

void CTestDeliverMultiRecipientsMixed_2::CheckSentMessageL()
	{
	// Need to check the following -
	// 1. Correct number of recipients
	// 2. The recipients have delivery status correctly set
	// 3. The index summary is correctly set
	// 4. The index message ID is valid.
	// 5. The index message ID matches that of the recipient

	iTestUtils.Printf(_L("Checking sent message %d has correct flags set...\n"), iMessageId);

	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);

	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);

	CSmsNumber* rcpt0 = header->Recipients()[0];

	iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);

	CSmsNumber* rcpt1 = header->Recipients()[1];

	iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
	iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);

	TMsvSmsEntry entry = MsvEntry().Entry();

	iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EPendingAcks)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EPendingAcks);
	iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EPendingAcks);

	TLogId logId = 0;
	TBool validId = entry.MessageId(logId);

	iTestUtils.Printf(_L("--Index message Id validity -> %d - should be %d\n"), validId, EFalse);
	iTestUtils.Test()(validId == EFalse);

	CleanupStack::PopAndDestroy(header);
	}

void CTestDeliverMultiRecipientsMixed_2::CheckDeliveredMessageL()
	{
	// Need to check the following -
	// 1. Correct number of recipients
	// 2. The recipient has delivery status is correctly set
	// 3. The index summary is correctly set

	iTestUtils.Printf(_L("Checking delivered message %d has correct flags set - round %d...\n"), iMessageId, ++iCheckCount);

	// Restore the recipient...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils.iRichText);
	CleanupStack::PushL(header);

	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(iMessageId);
	CMsvStore* store = iTestUtils.ReadStoreL();
	CleanupStack::PushL(store);
	header->RestoreL(*store);
	CleanupStack::PopAndDestroy(store);

	iTestUtils.Printf(_L("--Number of recipients -> %d - should be %d\n"), header->Recipients().Count(), 2);
	iTestUtils.Test()(header->Recipients().Count() == 2);

	CSmsNumber* rcpt0 = header->Recipients()[0];
	CSmsNumber* rcpt1 = header->Recipients()[1];

	TMsvSmsEntry entry = MsvEntry().Entry();

	if( iCheckCount == 1 )
		{
		// At least one of the recipients has been delivered to ok - check the
		// status of recipient 0 and make decisions based on that.
		CSmsNumber* pendingRcpt = NULL;

		if( rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck )
			{
			// Recipient 2 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);

			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);

			iTestUtils.Printf(_L("--Recipient 1 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt1->Status(), CMsvRecipient::EFailedToSend);
			iTestUtils.Test()(rcpt1->Status()==CMsvRecipient::EFailedToSend);

			iTestUtils.Printf(_L("--Recipient 1 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt1->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
			iTestUtils.Test()(rcpt1->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
		

			pendingRcpt = rcpt0;
			}
		else
			{
			// Recipient 0 delivered to first...
			iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
			iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);

			iTestUtils.Printf(_L("--Recipient 0 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt0->Status(), CMsvRecipient::EFailedToSend);
			iTestUtils.Test()(rcpt0->Status()==CMsvRecipient::EFailedToSend);

			iTestUtils.Printf(_L("--Recipient 0 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt0->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
			iTestUtils.Test()(rcpt0->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
			
			iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EPendingAck)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EPendingAck);
			iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EPendingAck);

			pendingRcpt = rcpt1;
			}
		TLogId logId = 0;
		TBool validId = entry.MessageId(logId);

		iTestUtils.Printf(_L("--Index message Id validity -> %d - should NOT be %d\n"), validId, EFalse);
		iTestUtils.Test()(validId != EFalse);

		iTestUtils.Printf(_L("--Index message Id -> %d - should be %d\n"), logId, pendingRcpt->LogId());
		iTestUtils.Test()(logId == pendingRcpt->LogId());
		}
	else
		{
		// Status-reports for both recipients have been received - check the
		// status of both recipients.
		iTestUtils.Printf(_L("--Recipient 0 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt0->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
		iTestUtils.Test()(rcpt0->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);

		iTestUtils.Printf(_L("--Recipient 0 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt0->Status(), CMsvRecipient::EFailedToSend);
		iTestUtils.Test()(rcpt0->Status()==CMsvRecipient::EFailedToSend);

		iTestUtils.Printf(_L("--Recipient 0 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt0->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
		iTestUtils.Test()(rcpt0->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
		
		iTestUtils.Printf(_L("--Recipient 1 - Delivery status -> %d - should be %d (EAckError)\n"), rcpt1->AckStatus(ESmsAckTypeDelivery), CSmsNumber::EAckError);
		iTestUtils.Test()(rcpt1->AckStatus(ESmsAckTypeDelivery) == CSmsNumber::EAckError);
		
		iTestUtils.Printf(_L("--Recipient 1 - Recipient status -> %d - should be %d (EFailedToSend)\n"), rcpt1->Status(), CMsvRecipient::EFailedToSend);
		iTestUtils.Test()(rcpt1->Status()==CMsvRecipient::EFailedToSend);

		iTestUtils.Printf(_L("--Recipient 1 - Error status -> %d - should not be %d (ESmsShortMessageReceivedBySME)\n"), rcpt1->Error(), TSmsStatus::ESmsShortMessageReceivedBySME);
		iTestUtils.Test()(rcpt1->Error()!= TSmsStatus::ESmsShortMessageReceivedBySME);
		
		iTestUtils.Printf(_L("--Index summary -> %d - should be %d (EAllFailed)\n"), entry.AckSummary(ESmsAckTypeDelivery), TMsvSmsEntry::EAllFailed);
		iTestUtils.Test()(entry.AckSummary(ESmsAckTypeDelivery) == TMsvSmsEntry::EAllFailed);

		// Test is no ended - complete self to stop.
		CompleteSelf();
		}

	CleanupStack::PopAndDestroy(header);
	}

const TDesC& CTestDeliverMultiRecipientsMixed_2::TestName()
	{
	return KTestDeliverMultiRecipientsMixed_2();
	}
