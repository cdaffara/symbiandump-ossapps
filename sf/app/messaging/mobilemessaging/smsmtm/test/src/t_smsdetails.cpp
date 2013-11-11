// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "t_smsdetails.h"

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
_LIT(KRecipientOk,		"+447747065441");  // has to be the number of SIM in the data blok
_LIT(KMessageData, "This is a simple text message");

_LIT(KFirstName,	"Joe");
_LIT(KLastName,		"Bloggs");
_LIT(KContactInfo,	"Joe Bloggs");

_LIT(KTestReceiveDetails_1,	"Test receive details - number not in contacts database.");
_LIT(KTestReceiveDetails_2,	"Test receive details - number in contacts database.");
_LIT(KTestReceiveDetails_3,	"Test receive details - number duplicated in contacts database.");

#if defined (__WINS__)
_LIT(KPddName,"ECDRV");
_LIT(KLddName,"ECOMM");
#else	//__WINS__
_LIT(KPddSirName,"ESIR");
_LIT(KPddUartName,"ECUART4");
_LIT(KLddName,"ECOMM");
#endif	//__WINS__

RTest test(_L("T_SmsDetails Test Harness"));

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
	CTestDetailsBase* testCase = NULL;
	
	testCase = 	CTestReceiveDetails_1::NewL(*iTestUtils, *this);
	RunTestCaseL(*testCase);

	testCase = 	CTestReceiveDetails_2::NewL(*iTestUtils, *this);
	RunTestCaseL(*testCase);

	testCase = 	CTestReceiveDetails_3::NewL(*iTestUtils, *this);
	RunTestCaseL(*testCase);
	
	if( iTestFailCount > 0 )
		iTestUtils->Printf(_L("Test Complete -> %d Failures\n"), iTestFailCount);
	else
		iTestUtils->Printf(_L("Test Complete -> Passed\n"));
	}
	
void CTestEngine::RunTestCaseL(CTestDetailsBase& aTestCase)
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
	
	// Remove the contacts database file
	iTestUtils->DeleteContactsFileL();
	
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
	
//	methods from MTestObserver

void CTestEngine::TestFailed()
	{
	iTestUtils->Printf(_L("!!Test Failed\n"));
	++iTestFailCount;
	CActiveScheduler::Stop();
	}
	
void CTestEngine::TestPassed()
	{
	iTestUtils->Printf(_L("!!Test Complete\n"));
	CActiveScheduler::Stop();
	}
	
/*
 * CTestDetailsBase
 */
 
CTestDetailsBase::~CTestDetailsBase()
	{
	Cancel();
	
	delete iEntryForObserver;
	delete iOperation;
	}	
	
void CTestDetailsBase::Start()
	{
	iTestUtils.Printf(_L("Starting %S\n"), &TestName());
	iState = EClearFolders;
	CompleteSelf();
	}
	
CTestDetailsBase::CTestDetailsBase(CSmsTestUtils& aTestUtils, MTestObserver& aObserver)
: CActive(CActive::EPriorityStandard), iTestUtils(aTestUtils), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}
	
void CTestDetailsBase::ConstructL()
	{
	iEntryForObserver = Session().GetEntryL(KMsvGlobalInBoxIndexEntryId);
	}
	
void CTestDetailsBase::CompleteSelf()
	{
	TRequestStatus* pStat = &iStatus;
	User::RequestComplete(pStat, KErrNone);
	SetActive();
	}
	
CMsvSession& CTestDetailsBase::Session()
	{
	return *iTestUtils.iMsvSession;
	}

CMsvEntry& CTestDetailsBase::MsvEntry()
	{
	return *iTestUtils.iMsvEntry;
	}

CSmsClientMtm& CTestDetailsBase::Client()
	{
	return *iTestUtils.iSmsClientMtm;
	}
	
CSmsSettings& CTestDetailsBase::ServiceSettings()
	{
	return *iTestUtils.iServiceSettings;
	}
	
void CTestDetailsBase::Test(TBool aTest)
	{
	if( !aTest )
		iObserver.TestFailed();
	}
		
void CTestDetailsBase::ClearFoldersL()
	{
	iTestUtils.DeleteSmsMessagesL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils.DeleteSmsMessagesL(KMsvGlobalInBoxIndexEntryId);
	iTestUtils.DeleteSmsMessagesL(KMsvSentEntryId);		
	}
		
void CTestDetailsBase::CreateMessageL()
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
	
void CTestDetailsBase::SetRecipientsL(CSmsHeader& aHeader)
	{
	CSmsNumber* rcpt0 = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt0);
	rcpt0->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt0);
	CleanupStack::Pop(rcpt0);
	}

void CTestDetailsBase::SendMessageL()
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
	
void CTestDetailsBase::ChangeServiceSettingsL()
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
	
// methods from CActive

void CTestDetailsBase::RunL()
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
		iState = ESendMessage;
		CompleteSelf();
		} break;
	case ESendMessage:
		{
		SendMessageL();
		iState = EMessageSent;
		} break;
	case EMessageSent:
		{
		// Observer the in-box.
		iEntryForObserver->SetEntryL(KMsvGlobalInBoxIndexEntryId);
		iEntryForObserver->AddObserverL(*this);
		
		iState = EPendingReceivedMessage;
		} break;
	case EPendingReceivedMessage:
		{
		// End the test...
		iObserver.TestPassed();
		} break;			
	default:
		User::Invariant();
		break;
		}
	}
	
void CTestDetailsBase::DoCancel()
	{
	}
	
TInt CTestDetailsBase::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}
	
//	methods from MMsvEntryObserver	
	
void CTestDetailsBase::HandleEntryEventL(TMsvEntryEvent aEvent, TAny* aArg1, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	__ASSERT_DEBUG( iState == EPendingReceivedMessage, User::Invariant() );
	
	if( aEvent == EMsvNewChildren )
		{
		iTestUtils.Printf(_L("Message received in in-box...\n"));
		
		CMsvEntrySelection* sel = (CMsvEntrySelection*)(aArg1);
		
		for( TInt ii=0; ii < sel->Count(); ++ii )
			{
			CheckReceivedMessageL(sel->At(ii));	
			}
			
		// Test should be over - complete-self.
		CompleteSelf();
		}
	}
	
/*
 * CTestReceiveDetails_1
 */
 
CTestReceiveDetails_1* CTestReceiveDetails_1::NewL(CSmsTestUtils& aTestUtils, MTestObserver& aObserver)
	{
	CTestReceiveDetails_1* self = new (ELeave) CTestReceiveDetails_1(aTestUtils, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CTestReceiveDetails_1::~CTestReceiveDetails_1()
	{
	}
	
CTestReceiveDetails_1::CTestReceiveDetails_1(CSmsTestUtils& aTestUtils, MTestObserver& aObserver)
: CTestDetailsBase(aTestUtils, aObserver)
	{
	}

// methods from CTestSendBase

void CTestReceiveDetails_1::CheckReceivedMessageL(TMsvId aMessageId)
	{
	iTestUtils.Printf(_L("Checking received message %d has correct details\n"), aMessageId);
	
	// Need to see that the details contains a telephone number
	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(aMessageId);
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	// Recipient 2 delivered to first...
	iTestUtils.Printf(_L("->Details info is %S - should be %S\n"), &entry.iDetails, &KRecipientOk());
	Test(entry.iDetails.CompareF(KRecipientOk()) == 0);	
	}

const TDesC& CTestReceiveDetails_1::TestName()
	{
	return KTestReceiveDetails_1();
	}

/*
 * CTestReceiveDetails_2
 */
 
CTestReceiveDetails_2* CTestReceiveDetails_2::NewL(CSmsTestUtils& aTestUtils, MTestObserver& aObserver)
	{
	CTestReceiveDetails_2* self = new (ELeave) CTestReceiveDetails_2(aTestUtils, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CTestReceiveDetails_2::~CTestReceiveDetails_2()
	{
	}
	
CTestReceiveDetails_2::CTestReceiveDetails_2(CSmsTestUtils& aTestUtils, MTestObserver& aObserver)
: CTestDetailsBase(aTestUtils, aObserver)
	{
	}
	
void CTestReceiveDetails_2::ConstructL()
	{
	CTestDetailsBase::ConstructL();
	
	// Add contact to database...
	TContactItemId contId = iTestUtils.CreateContactL(KFirstName, KLastName, KRecipientOk, KNullDesC);
	if( contId == KNullContactId )
		User::Leave(KErrCorrupt);
	}

// methods from CTestSendBase

void CTestReceiveDetails_2::CheckReceivedMessageL(TMsvId aMessageId)
	{
	iTestUtils.Printf(_L("Checking received message %d has correct details\n"), aMessageId);
	
	// Need to see that the details contains a telephone number
	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(aMessageId);
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	// Recipient 2 delivered to first...
	iTestUtils.Printf(_L("->Details info is %S - should be %S\n"), &entry.iDetails, &KContactInfo());
	Test(entry.iDetails.CompareF(KContactInfo()) == 0);
	}

const TDesC& CTestReceiveDetails_2::TestName()
	{
	return KTestReceiveDetails_2();
	}
	
/*
 * CTestReceiveDetails_3
 */
 
CTestReceiveDetails_3* CTestReceiveDetails_3::NewL(CSmsTestUtils& aTestUtils, MTestObserver& aObserver)
	{
	CTestReceiveDetails_3* self = new (ELeave) CTestReceiveDetails_3(aTestUtils, aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;	
	}

CTestReceiveDetails_3::~CTestReceiveDetails_3()
	{
	}
	
CTestReceiveDetails_3::CTestReceiveDetails_3(CSmsTestUtils& aTestUtils, MTestObserver& aObserver)
: CTestDetailsBase(aTestUtils, aObserver)
	{
	}
	
void CTestReceiveDetails_3::ConstructL()
	{
	CTestDetailsBase::ConstructL();
	
	// Add contact to database - this is a duplicate...
	TContactItemId contId = iTestUtils.CreateContactL(KFirstName, KLastName, KRecipientOk, KNullDesC);
	if( contId == KNullContactId )
		User::Leave(KErrCorrupt);
	}

// methods from CTestSendBase

void CTestReceiveDetails_3::CheckReceivedMessageL(TMsvId aMessageId)
	{
	iTestUtils.Printf(_L("Checking received message %d has correct details\n"), aMessageId);
	
	// Need to see that the details contains a telephone number
	MsvEntry().SetEntryL(KMsvGlobalInBoxIndexEntryId);
	MsvEntry().SetEntryL(aMessageId);
	TMsvSmsEntry entry = MsvEntry().Entry();
	
	// Recipient 2 delivered to first...
	iTestUtils.Printf(_L("->Details info is %S - should be %S\n"), &entry.iDetails, &KRecipientOk());
	Test(entry.iDetails.CompareF(KRecipientOk()) == 0);
	}

const TDesC& CTestReceiveDetails_3::TestName()
	{
	return KTestReceiveDetails_3();
	}
