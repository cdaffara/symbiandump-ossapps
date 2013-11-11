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

#include <testexecutelog.h>
#include "t_smsspecialindicationsstep.h"
#include <smuthdr.h>
#include <txtrich.h>
#include <msvuids.h>
#include <smsuaddr.h>
#include <csmsaccount.h>
#include "gsmuieoperations.h"
#include <commsdattypesv1_1.h>

using namespace CommsDat;
LOCAL_D RTest test(_L("TestTextE_1_1"));
_LIT(KTestMessageDir,"");

const TTimeIntervalMicroSeconds32 KWaitForWatchersToStart = 20000000;
const TTimeIntervalMicroSeconds32 KWaitForWatchersToStartDisplayInterval = 5000000;

_LIT(KMessageData, "This is a simple text message");
_LIT(KRecipientOk,		"+44111111111");  // has to be valid number and switched on

		
void EntryObserver::HandleEntryEventL(TMsvEntryEvent aEvent, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	switch (aEvent)
		{
			case EMsvEntryChanged:
				{
				break;
				}
			case EMsvNewChildren:
				{	
				CActiveScheduler::Stop();
				break;
				}
						
		default:
				{
				break;
				}
		}
	}
		
CSmsSpecialIndicationsStep::CSmsSpecialIndicationsStep()
:CTestStep()
	{	
	}
		
CSmsSpecialIndicationsStep::~CSmsSpecialIndicationsStep()
	{
	delete iTestUtils;
	delete iEntry;
	delete iScheduler;
	delete iEntryObserver;
	delete iSessionObserver;
	delete iSession;
	delete iWatchers;
	delete iOperation;
	delete iTestActive;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	}

void CSmsSpecialIndicationsStep::InitialiseSpecialIndicationsStepL()
	{	
	SetTestStepName(KSmsSpecialIndicationsStep1);
	iScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(iScheduler); 
	CActiveScheduler::Install(iScheduler);
	}	
	
void CSmsSpecialIndicationsStep::InitialiseTestL()
	{
	INFO_PRINTF1(_L("Initialising the test..."));
	iTestUtils = CSmsTestUtils::NewL(test);	
	// Notify the system agent that the phone is on.
	iTestUtils->NotifySaPhoneOnL();
	
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

	// set up an opserver to observe the inbox
	iEntryObserver = new (ELeave) EntryObserver();
	iSessionObserver = new (ELeave) SessionObserver();
	iSession = CMsvSession::OpenSyncL(*iSessionObserver);
	TMsvSelectionOrdering ordering;	
	iEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryIdValue,ordering);
	iEntry->AddObserverL(*iEntryObserver);
	
	//used for acynchronous calls
	iTestActive=new(ELeave) CTestActive;
	
	INFO_PRINTF1(_L("Successfully initialised test"));
	
}
	
void CSmsSpecialIndicationsStep::InitialiseSimTsyL()
	{
	
	INFO_PRINTF1(_L("Initialising the SimTsy..."));
	
	_LIT(KDefaultTsyName, "SIM"); 
	HBufC* tsyName =KDefaultTsyName().AllocLC();

	//Initialize TSY using the System Agent
	TInt testState;
	TInt testNumber=1;
	if(KErrNone != RProperty::Get(KUidPSSimTsyCategory, KPSSimTsyTestNumber, testState))
		{
		User::LeaveIfError(RProperty::Define(KUidPSSimTsyCategory, KPSSimTsyTestNumber, RProperty::EInt));
		}
	User::LeaveIfError(RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, testNumber));
	if(KErrNone != RProperty::Get(KUidSystemCategory, KMsvTestUidPhonePwrValue, testState))
		{
		User::LeaveIfError(RProperty::Define(KUidSystemCategory, KMsvTestUidPhonePwrValue, RProperty::EInt));
		}
	User::LeaveIfError(RProperty::Set(KUidSystemCategory, KMsvTestUidPhonePwrValue, EMsvTestPhoneOn));

	User::LeaveIfError(RProperty::Get(KUidPSSimTsyCategory, KPSSimTsyTestNumber, testNumber));

	RTelServer etelServer;
	User::LeaveIfError(etelServer.Connect());
	CleanupClosePushL(etelServer);
	User::LeaveIfError(etelServer.LoadPhoneModule(tsyName->Des()));

	// Find the phone corresponding to this TSY and open a number of handles on it
	TInt numPhones;
	User::LeaveIfError(etelServer.EnumeratePhones(numPhones));	
	TBool found=EFalse;
	
	RMobilePhone iPhone;
	while (!found && numPhones--)
		{
		TName phoneTsy;
		User::LeaveIfError(etelServer.GetTsyName(numPhones,phoneTsy));
		if (phoneTsy.CompareF(tsyName->Des())==KErrNone)
			{
			found = ETrue;
			RTelServer::TPhoneInfo info;
			User::LeaveIfError(etelServer.GetPhoneInfo(numPhones,info));
			CleanupClosePushL(iPhone);
			User::LeaveIfError(iPhone.Open(etelServer,info.iName));
			User::LeaveIfError(iPhone.Initialise());
			//Required Pause to Allow SMSStack to Complete its Async Init call to the TSY and finish its StartUp.
			TTimeIntervalMicroSeconds32 InitPause=9000000;  
			User::After(InitPause);							
			
			CleanupStack::PopAndDestroy(&iPhone);
			}
		}
	
	CleanupStack::PopAndDestroy(&etelServer);	

	CMDBSession* dbSession = CMDBSession::NewL(CMDBSession::LatestVersion());
	CleanupStack::PushL(dbSession);
	CMDBRecordSet<CCDGlobalSettingsRecord> globalSettingsRecord(KCDTIdGlobalSettingsRecord);
	TRAPD(err, globalSettingsRecord.LoadL(*dbSession));
	if(err != KErrNone)
		{
		User::Leave(KErrNotFound);				
		}
	
	CCDModemBearerRecord *modemRecord = static_cast<CCDModemBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdModemBearerRecord));		
	CleanupStack::PushL(modemRecord);	
	modemRecord->SetRecordId(((CCDGlobalSettingsRecord*)globalSettingsRecord.iRecords[0])->iModemForPhoneServicesAndSMS);
	modemRecord->LoadL(*dbSession);
	modemRecord->iTsyName.SetMaxLengthL(tsyName->Des().Length());
	modemRecord->iTsyName = tsyName->Des();	
	modemRecord->ModifyL(*dbSession);
	CleanupStack::PopAndDestroy(3);  //tsyName, dbSession, modemRecord 	
	
	INFO_PRINTF1(_L("Successfully initialised the Sim Tsy"));
	}
	
void CSmsSpecialIndicationsStep::StartWatcherL()
	{
	INFO_PRINTF1(_L("Start the Watcher"));

	if( WatchersAlreadyRunningL() )
		{
		INFO_PRINTF1(_L("Watchers are already running\n"));
		return;
		}

	iWatchers = CTestUtilsWatcherStarter::NewL();

	TTimeIntervalMicroSeconds32 wait = KWaitForWatchersToStart;
	TBool started = EFalse;
	while( !started && wait.Int() > 0 )
		{
		INFO_PRINTF2(_L("Waiting %d secs for watchers to start..."), wait.Int() / 1000000);
		wait = wait.Int() - KWaitForWatchersToStartDisplayInterval.Int();
		User::After(KWaitForWatchersToStartDisplayInterval);
		started = WatchersAlreadyRunningL();
		}

	if( !WatchersAlreadyRunningL() )
		{
		INFO_PRINTF1(_L("WARNING: NBS Watcher has not started yet\n"));
		}
	}
	
TBool CSmsSpecialIndicationsStep::WatchersAlreadyRunningL()
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

void CSmsSpecialIndicationsStep::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils->Printf(_L("--Setting single recipient\n"));
	
	CSmsNumber* rcpt = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt);
	rcpt->SetAddressL(KRecipientOk);
	aHeader.Recipients().AppendL(rcpt);
	CleanupStack::Pop(rcpt);
	}
	
void CSmsSpecialIndicationsStep::CreateMessageL()
	{
	INFO_PRINTF1(_L("Creating message..."));
	
	TMsvEntry entry;
	entry.SetVisible(ETrue); 
	entry.SetInPreparation(ETrue); 
	entry.iServiceId = iTestUtils->iSmsServiceId; 
	entry.iType = KUidMsvMessageEntry; 
	entry.iMtm = KUidMsgTypeSMS; 
	entry.iDate.HomeTime(); 
	entry.iSize = 0; 
	entry.iDescription.Set(KNullDesC); 
	entry.iDetails.Set(KNullDesC); 
	entry.SetSendingState(KMsvSendStateScheduled);

	// Create the SMS header object...
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils->iRichText);
	CleanupStack::PushL(header);
	
	// Set the body text...
	iTestUtils->iRichText->Reset();
	iTestUtils->iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	header->SetSmsSettingsL(*iTestUtils->iServiceSettings); 
	
	// Set the service centre
	TInt defaultIndex = iTestUtils->iServiceSettings->DefaultServiceCenter();
	header->SetServiceCenterAddressL(iTestUtils->iServiceSettings->GetServiceCenter(defaultIndex).Address());

	// Set recipient - ask derived class
	SetRecipientsL(*header);
	
	// Update entry description and details...
	CArrayPtrFlat<CSmsNumber>& recipient = header->Recipients();
	entry.iDetails.Set(recipient[0]->Address());
	entry.iDescription.Set(iTestUtils->iRichText->Read(0, iTestUtils->iServiceSettings->DescriptionLength()));
	entry.SetInPreparation(EFalse);
	
	// Create the entry - set context to the global outbox.
	iTestUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils->iMsvEntry->CreateL(entry);

	// Create new store and save header information 
	iTestUtils->iMsvEntry->SetEntryL(entry.Id()); 
	CMsvStore* store = iTestUtils->iMsvEntry->EditStoreL(); 
	CleanupStack::PushL(store); 
	header->StoreL(*store);
	store->StoreBodyTextL(*iTestUtils->iRichText);
	store->CommitL(); 
	CleanupStack::PopAndDestroy(2, header); 
	
	iMessageId = entry.Id();

	}
	
void CSmsSpecialIndicationsStep::SendMessageL()
	{
	INFO_PRINTF1(_L("Sending message..."));

	delete iOperation;
	iOperation = NULL;

	CMsvEntrySelection* sel = new (ELeave) CMsvEntrySelection();
	CleanupStack::PushL(sel);
	sel->AppendL(iMessageId);
	iTestUtils->DisplayMessageL(iMessageId);
	iTestUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iOperation = iTestUtils->iMsvEntry->CopyL(*sel, iTestUtils->iSmsServiceId,iTestActive->iStatus);
	iTestActive->StartL();
	CActiveScheduler::Start();	
	CleanupStack::PopAndDestroy(sel);
	}

TBool CSmsSpecialIndicationsStep::CheckMessageL()
	{

	INFO_PRINTF1(_L("Received new message in Inbox - should be status report..."));
		
	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId,ordering);
	CleanupStack::PushL(entry);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 1);

	// Restore the message
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iTestUtils->iRichText);
	CleanupStack::PushL(header);
	entry->SetEntryL((*msvEntrySelection)[0]);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);	
	header->RestoreL(*store);
	
	if(store->HasBodyTextL())
		{
		store->RestoreBodyTextL(*iTestUtils->iRichText);	
		}
		
	TBuf<KSmsDescriptionLength> desc;
	User::LeaveIfError(TSmsUtilities::GetDescription(header->Message(), desc));
	CleanupStack::PopAndDestroy(4,entry);	
	
	INFO_PRINTF1(desc);
	
	_LIT(KMsg,"14 Messages Waiting");
	if(desc==KMsg)
		{
		return ETrue;	
		}
	else
		{
		return EFalse;		
		}	
	}

TVerdict CSmsSpecialIndicationsStep::doTestStepPreambleL()
	{	
	__UHEAP_MARK;
	SetTestStepResult(EPass);
	test.Start(_L("sdf"));
	InitialiseTestL();
	InitialiseSimTsyL();
	StartWatcherL();
	iTestUtils->DeleteSmsMessagesL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils->DeleteSmsMessagesL(KMsvGlobalInBoxIndexEntryId);
	iTestUtils->DeleteSmsMessagesL(KMsvSentEntryId);
	CreateMessageL();
	SendMessageL();
	//wait for message to be received...
	CActiveScheduler::Start();	

	return TestStepResult();
	}
		

TVerdict CSmsSpecialIndicationsStep::doTestStepL()
	{		
	if(CheckMessageL())
		{
		SetTestStepResult(EPass);	
		}
	else
		{
		SetTestStepResult(EFail);			
		}
	
	return TestStepResult();
	}	

TVerdict CSmsSpecialIndicationsStep::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}
	
	


