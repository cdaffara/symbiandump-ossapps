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
// file  CTestHeaderStoreReStore.cpp
// This contains Ut for Sms header migration.
// 

#include "csmsheaderbodySqlstorerestore.h"
#include <smuthdr.h>
#include <txtrich.h>
#include <msvuids.h>
#include <smsuaddr.h>
#include <csmsaccount.h>
#include <Smut.h>
#include <sqldb.h>
#include <csmsemailfields.h>


#define MY_ASSERT_TRUE(a)  \
		INFO_PRINTF1(KFunction);   \
		ASSERT_TRUE(a);
#define MY_ASSERT_FALSE(a)  \
		INFO_PRINTF1(KFunction);   \
		ASSERT_FALSE(a);
#define MY_ASSERT_EQUALS(a,b)  \
		INFO_PRINTF1(KFunction);   \
		ASSERT_EQUALS(a,b);



_LIT(KDbFileName, "\\messaging.db");


const TTimeIntervalMicroSeconds32 KWaitForWatchersToStart = 20000000;
const TTimeIntervalMicroSeconds32 KWaitForWatchersToStartDisplayInterval = 5000000;

_LIT(KMessageData, "This is a simple text message");
_LIT(KRecipientOk,		"+44111111111");  // has to be valid number and switched on
_LIT(KRecipientOk1,		"+44111111115");  // has to be valid number and switched on
_LIT(KRecipientOk3,		"+44111111119");  // has to be valid number and switched on
_LIT(KRecipientName,		"XYx"); 
_LIT(KRecipientName1,		"Sukanta"); 
_LIT(KRealAddress,		"XYZ@nokia.com"); 
_LIT(KRealAddress1,		"Abc123@nokia.com"); 
_LIT(KRealAddress11,		""); 
_LIT(KSubject, "Subject subject");
_LIT(KRecipientEmpty, "");


RTest test(_L("TestStoreReStore1"));

CTestHeaderStoreReStore::CTestHeaderStoreReStore()
	{
	}

CTestHeaderStoreReStore::~CTestHeaderStoreReStore()
	{
	}


void CTestHeaderStoreReStore::SetupL()
	{
	_LIT(KFunction, "SetupL");
	INFO_PRINTF1(KFunction);
	
	TParse parse;
	TPtrC drive = TDriveUnit(EDriveC).Name();
	parse.Set(KDbFileName, &drive, NULL);
	TFileName dbFile = parse.FullName();
	RSqlDatabase::Delete(dbFile);

	InitialiseTestL();
	}

void CTestHeaderStoreReStore::TearDownL()
	{
	_LIT(KFunction, "TearDownL");
	INFO_PRINTF1(KFunction);
	
	delete iTestUtils;
	delete iTestActive;
	delete iEntryObserver;
	delete iSessionObserver;
	delete iSession;
	delete iheader;
	delete iReciveheader;
	}


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
	
void CTestHeaderStoreReStore::InitialiseTestL()
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

void CTestHeaderStoreReStore::SetRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils->Printf(_L("--Setting single recipient\n"));
	
	CSmsNumber* rcpt = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt);
	rcpt->SetAddressL(KRecipientOk);
	rcpt->SetNameL(KRecipientName);
	aHeader.Recipients().AppendL(rcpt);
	CleanupStack::Pop(rcpt);
	}
	
void CTestHeaderStoreReStore::SetEmptyRecipientsL(CSmsHeader& aHeader)
	{
	iTestUtils->Printf(_L("--Setting single recipient\n"));
	
	CSmsNumber* rcpt = CSmsNumber::NewL();
	CleanupStack::PushL(rcpt);
	rcpt->SetAddressL(KRecipientEmpty);
	aHeader.Recipients().AppendL(rcpt);
	CleanupStack::Pop(rcpt);
	}

void CTestHeaderStoreReStore::TestStoreHeaderL()
	{
	
	_LIT(KFunction, ":Test For Store SMS Header ");
	INFO_PRINTF1(KFunction);

	
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
	iheader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils->iRichText);
		
	// Set the body text...
	iTestUtils->iRichText->Reset();
	iTestUtils->iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	iheader->SetSmsSettingsL(*iTestUtils->iServiceSettings); 
	
	// Set the service centre
	TInt defaultIndex = iTestUtils->iServiceSettings->DefaultServiceCenter();
	iheader->SetServiceCenterAddressL(iTestUtils->iServiceSettings->GetServiceCenter(defaultIndex).Address());

	// Set recipient - ask derived class
	SetRecipientsL(*iheader);
	
	CSmsNumber* rcpt1 = CSmsNumber::NewL();
	rcpt1->SetStatus((CMsvRecipient::TRecipientStatus)1);
	rcpt1->SetError(2);
	rcpt1->SetRetries(3);
	TTime date;
	date.UniversalTime();
	rcpt1->SetTimeValue(date.Int64());
	
	rcpt1->SetAddressL(KRecipientOk1);
	rcpt1->SetNameL(KRecipientName);
	iheader->Recipients().AppendL(rcpt1);

	CSmsNumber* rcpt2 = CSmsNumber::NewL();
	rcpt2->SetAddressL(KRecipientOk3);
	rcpt2->SetNameL(KRecipientName);
	iheader->Recipients().AppendL(rcpt2);

	CSmsNumber* rcpt3 = CSmsNumber::NewL();
	rcpt3->SetAddressL(KRecipientOk3);
	rcpt3->SetNameL(KRecipientName1);
	iheader->Recipients().AppendL(rcpt3);

	_LIT(KFunction1, ":Test For Store Email over SMS Header ");
	INFO_PRINTF1(KFunction1);

	
	CSmsEmailFields* emailFields = NULL;
    emailFields = CSmsEmailFields::NewL( iheader->EmailFields() );
    emailFields->AddAddressL(KRealAddress);
    emailFields->AddAddressL(KRealAddress1);
    emailFields->SetSubjectL(KSubject);
    iheader->SetEmailFieldsL( *emailFields );
 
	iTestUtils->Printf(_L("--Setting single recipient Done \n"));
	
	// Create the entry - set context to the global outbox.
	iTestUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils->iMsvEntry->CreateL(entry);

	// Create new store and save header information 
	iTestUtils->iMsvEntry->SetEntryL(entry.Id()); 
	CMsvStore* store = iTestUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store); 
	iheader->StoreL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy(1); //store 
	
	INFO_PRINTF1(_L("Successfully Message header Stored"));
	
	TestReStoreHeaderL();
	}

void CTestHeaderStoreReStore::TestReStoreHeaderL()
	{

	_LIT(KFunction, ":Test For ReStore SMS Header ");
	INFO_PRINTF1(KFunction);

			
	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvGlobalOutBoxIndexEntryId,ordering);
	CleanupStack::PushL(entry);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 1);
	ASSERT_TRUE(count == 1);


	// Restore the message
	iReciveheader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iTestUtils->iRichText);
	entry->SetEntryL((*msvEntrySelection)[0]);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);	
	iTestUtils->Printf(_L("--Restore \n"));
	iReciveheader->RestoreL(*store);
	
	//Check for a recipient count
	
	TInt cc= iReciveheader->Recipients().Count();
	ASSERT_EQUALS(cc,4);
	
	_LIT(KFunction1, ":Test For ReStore Email over SMS Header ");
	INFO_PRINTF1(KFunction1);

	CSmsEmailFields* emailFields = NULL;
    emailFields = CSmsEmailFields::NewL(iReciveheader->EmailFields());
	
	const MDesCArray& addresses = emailFields->Addresses();
	count = addresses.MdcaCount();
	ASSERT_EQUALS(count,2);
	
	for( TInt i=0; i < count; ++i )
		{
		addresses.MdcaPoint(i);		
		}
	
	CleanupStack::PopAndDestroy(3,entry);	
	INFO_PRINTF1(_L("Successfully Message header ReStored"));

	}
	

void CTestHeaderStoreReStore::TestStoreReStoreSMSHeaderL()
	{
	
	_LIT(KFunction, ":Test For Store SMS Header ");
	INFO_PRINTF1(KFunction);

	
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
	iheader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils->iRichText);
		
	// Set the body text...
	iTestUtils->iRichText->Reset();
	iTestUtils->iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	iheader->SetSmsSettingsL(*iTestUtils->iServiceSettings); 
	
	// Set the service centre
	TInt defaultIndex = iTestUtils->iServiceSettings->DefaultServiceCenter();
	iheader->SetServiceCenterAddressL(iTestUtils->iServiceSettings->GetServiceCenter(defaultIndex).Address());


	_LIT(KFunction1, ":Test For Store Email over SMS Header ");
	INFO_PRINTF1(KFunction1);

	
 
	iTestUtils->Printf(_L("--Setting single recipient Done \n"));
	
	// Create the entry - set context to the global outbox.
	iTestUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils->iMsvEntry->CreateL(entry);

	// Create new store and save header information 
	iTestUtils->iMsvEntry->SetEntryL(entry.Id()); 
	CMsvStore* store = iTestUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store); 
	
	iheader->StoreL(*store);
	
	store->CommitL();
	CleanupStack::PopAndDestroy(1);//Store 
	
	INFO_PRINTF1(_L("Successfully Message header Stored"));
	
	TestReStoreSMSHeaderL();
	}

void CTestHeaderStoreReStore::TestReStoreSMSHeaderL()
	{

	_LIT(KFunction, ":Test For ReStore SMS Header ");
	INFO_PRINTF1(KFunction);

			
	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvGlobalOutBoxIndexEntryId,ordering);
	CleanupStack::PushL(entry);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 1);
	ASSERT_TRUE(count == 1);


	// Restore the message
	iReciveheader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iTestUtils->iRichText);
	entry->SetEntryL((*msvEntrySelection)[0]);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);	
	iTestUtils->Printf(_L("--Restore \n"));
	iReciveheader->RestoreL(*store);
	
	
	_LIT(KFunction1, ":Test For ReStore Email over SMS Header ");
	INFO_PRINTF1(KFunction1);

	CleanupStack::PopAndDestroy(3,entry);	
	INFO_PRINTF1(_L("Successfully Message header ReStored"));
	}


void CTestHeaderStoreReStore::TestStoreReStoreEOSSMSHeaderL()
	{
	
	_LIT(KFunction, ":Test For Store SMS Header ");
	INFO_PRINTF1(KFunction);

	
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
	iheader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils->iRichText);
	
	// Set the body text...
	iTestUtils->iRichText->Reset();
	iTestUtils->iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	iheader->SetSmsSettingsL(*iTestUtils->iServiceSettings); 
	
	// Set the service centre
	TInt defaultIndex = iTestUtils->iServiceSettings->DefaultServiceCenter();
	iheader->SetServiceCenterAddressL(iTestUtils->iServiceSettings->GetServiceCenter(defaultIndex).Address());

	// Set recipient - ask derived class

	_LIT(KFunction1, ":Test For Store Email over SMS Header ");
	INFO_PRINTF1(KFunction1);

	
	CSmsEmailFields* emailFields = NULL;
    emailFields = CSmsEmailFields::NewL( iheader->EmailFields() );
    emailFields->AddAddressL(KRealAddress11);
    iheader->SetEmailFieldsL( *emailFields );
 
	iTestUtils->Printf(_L("--Setting single recipient Done \n"));
	
	// Create the entry - set context to the global outbox.
	iTestUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils->iMsvEntry->CreateL(entry);

	// Create new store and save header information 
	iTestUtils->iMsvEntry->SetEntryL(entry.Id()); 
	CMsvStore* store = iTestUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store); 
	iheader->StoreL(*store);

	store->CommitL();
	CleanupStack::PopAndDestroy(1);//Store 
	
	INFO_PRINTF1(_L("Successfully Message header Stored"));
	
	TestReStoreEOSHeaderL();
	}

void CTestHeaderStoreReStore::TestReStoreEOSHeaderL()
	{

	_LIT(KFunction, ":Test For ReStore SMS Header ");
	INFO_PRINTF1(KFunction);

			
	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvGlobalOutBoxIndexEntryId,ordering);
	CleanupStack::PushL(entry);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 1);
	ASSERT_TRUE(count == 1);


	// Restore the message
	iReciveheader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iTestUtils->iRichText);
	entry->SetEntryL((*msvEntrySelection)[0]);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);	
	iTestUtils->Printf(_L("--Restore \n"));
	iReciveheader->RestoreL(*store);
	
	CleanupStack::PopAndDestroy(3,entry);	
	INFO_PRINTF1(_L("Successfully Message header ReStored"));

	}

void CTestHeaderStoreReStore::TestStoreReStoreEmptyEOSSMSHeaderL()
	{
	
	_LIT(KFunction, ":Test For Store SMS Header ");
	INFO_PRINTF1(KFunction);

	
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
	iheader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils->iRichText);
	
	// Set the body text...
	iTestUtils->iRichText->Reset();
	iTestUtils->iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	iheader->SetSmsSettingsL(*iTestUtils->iServiceSettings); 
	
	// Set the service centre
	TInt defaultIndex = iTestUtils->iServiceSettings->DefaultServiceCenter();
	iheader->SetServiceCenterAddressL(iTestUtils->iServiceSettings->GetServiceCenter(defaultIndex).Address());

	_LIT(KFunction1, ":Test For Store Email over SMS Header ");
	INFO_PRINTF1(KFunction1);

	
	CSmsEmailFields* emailFields = NULL;
    emailFields = CSmsEmailFields::NewL( iheader->EmailFields() );
    iheader->SetEmailFieldsL( *emailFields );
 
	iTestUtils->Printf(_L("--Setting single recipient Done \n"));
	
	// Create the entry - set context to the global outbox.
	iTestUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils->iMsvEntry->CreateL(entry);

	// Create new store and save header information 
	iTestUtils->iMsvEntry->SetEntryL(entry.Id()); 
	CMsvStore* store = iTestUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store); 
	iheader->StoreL(*store);

	store->CommitL();
	CleanupStack::PopAndDestroy(1);//store
	
	INFO_PRINTF1(_L("Successfully Message header Stored"));
	
	TestReStoreEmptyEOSHeaderL();
	}

void CTestHeaderStoreReStore::TestReStoreEmptyEOSHeaderL()
	{

	_LIT(KFunction, ":Test For ReStore SMS Header ");
	INFO_PRINTF1(KFunction);

			
	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvGlobalOutBoxIndexEntryId,ordering);
	CleanupStack::PushL(entry);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 1);
	ASSERT_TRUE(count == 1);


	// Restore the message
	iReciveheader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iTestUtils->iRichText);
	entry->SetEntryL((*msvEntrySelection)[0]);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);	
	iTestUtils->Printf(_L("--Restore \n"));
	iReciveheader->RestoreL(*store);
	
	CleanupStack::PopAndDestroy(3,entry);	
	INFO_PRINTF1(_L("Successfully Message header ReStored"));

	}

void CTestHeaderStoreReStore::TestMultipuleEOSRecipientSMSHeaderL()
	{
	
	_LIT(KFunction, ":Test For Store SMS Header ");
	INFO_PRINTF1(KFunction);

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
	iheader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils->iRichText);
	
	// Set the body text...
	iTestUtils->iRichText->Reset();
	iTestUtils->iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	iheader->SetSmsSettingsL(*iTestUtils->iServiceSettings); 
	
	// Set the service centre
	TInt defaultIndex = iTestUtils->iServiceSettings->DefaultServiceCenter();
	iheader->SetServiceCenterAddressL(iTestUtils->iServiceSettings->GetServiceCenter(defaultIndex).Address());

	// Set recipient - ask derived class

	_LIT(KFunction1, ":Test For Store Email over SMS Header ");
	INFO_PRINTF1(KFunction1);

	
	CSmsEmailFields* emailFields = NULL;
    emailFields = CSmsEmailFields::NewL( iheader->EmailFields() );
    emailFields->AddAddressL(KRealAddress);
            
    iheader->SetEmailFieldsL( *emailFields );
 
	iTestUtils->Printf(_L("--Setting single recipient Done \n"));
	
	// Create the entry - set context to the global outbox.
	iTestUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils->iMsvEntry->CreateL(entry);

	// Create new store and save header information 
	iTestUtils->iMsvEntry->SetEntryL(entry.Id()); 
	CMsvStore* store = iTestUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store); 
	iheader->StoreL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy(1);//store
	
	INFO_PRINTF1(_L("Successfully Message header Stored"));
	
	TestReStoreMultipuleEOSRecipientSMSHeaderL();
	}

void CTestHeaderStoreReStore::TestReStoreMultipuleEOSRecipientSMSHeaderL()
	{

	_LIT(KFunction, ":Test For ReStore SMS Header ");
	INFO_PRINTF1(KFunction);

			
	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvGlobalOutBoxIndexEntryId,ordering);
	CleanupStack::PushL(entry);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 1);
	ASSERT_TRUE(count == 1);


	// Restore the message
	iReciveheader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iTestUtils->iRichText);
	entry->SetEntryL((*msvEntrySelection)[0]);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);	
	iTestUtils->Printf(_L("--Restore \n"));
	iReciveheader->RestoreL(*store);
	

	CleanupStack::PopAndDestroy(3,entry);	
	INFO_PRINTF1(_L("Successfully Message header ReStored"));

	}


void CTestHeaderStoreReStore::TestEmptyEOSSubjectSMSHeaderL()
	{
	
	_LIT(KFunction, ":Test For Store SMS Header ");
	INFO_PRINTF1(KFunction);

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
	iheader = CSmsHeader::NewL(CSmsPDU::ESmsSubmit, *iTestUtils->iRichText);
	//CleanupStack::PushL(iheader);
	
	// Set the body text...
	iTestUtils->iRichText->Reset();
	iTestUtils->iRichText->InsertL(0, KMessageData);
	
	// Copy the message settings...
	iheader->SetSmsSettingsL(*iTestUtils->iServiceSettings); 
	
	// Set the service centre
	TInt defaultIndex = iTestUtils->iServiceSettings->DefaultServiceCenter();
	iheader->SetServiceCenterAddressL(iTestUtils->iServiceSettings->GetServiceCenter(defaultIndex).Address());


	_LIT(KFunction1, ":Test For Store Email over SMS Header ");
	INFO_PRINTF1(KFunction1);

	
	CSmsEmailFields* emailFields = NULL;
    emailFields = CSmsEmailFields::NewL( iheader->EmailFields() );
    emailFields->SetSubjectL(KSubject);
        
    iheader->SetEmailFieldsL( *emailFields );
 
	iTestUtils->Printf(_L("--Setting single recipient Done \n"));
	
	// Create the entry - set context to the global outbox.
	iTestUtils->iMsvEntry->SetEntryL(KMsvGlobalOutBoxIndexEntryId);
	iTestUtils->iMsvEntry->CreateL(entry);

	// Create new store and save header information 
	iTestUtils->iMsvEntry->SetEntryL(entry.Id()); 
	CMsvStore* store = iTestUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(store); 
	iheader->StoreL(*store);
	store->CommitL();
	CleanupStack::PopAndDestroy(1);//store
	
	INFO_PRINTF1(_L("Successfully Message header Stored"));
	
	TestReStoreEmptySubjectSMSHeaderL();
	}

void CTestHeaderStoreReStore::TestReStoreEmptySubjectSMSHeaderL()
	{

	_LIT(KFunction, ":Test For ReStore SMS Header ");
	INFO_PRINTF1(KFunction);

			
	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	CMsvEntry* entry = CMsvEntry::NewL(*iSession, KMsvGlobalOutBoxIndexEntryId,ordering);
	CleanupStack::PushL(entry);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 1);
	ASSERT_TRUE(count == 1);


	// Restore the message
	iReciveheader = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iTestUtils->iRichText);
	
	entry->SetEntryL((*msvEntrySelection)[0]);

	CMsvStore* store = entry->ReadStoreL();
	CleanupStack::PushL(store);	
	iTestUtils->Printf(_L("--Restore \n"));
	iReciveheader->RestoreL(*store);
	
	CleanupStack::PopAndDestroy(3,entry);	
	INFO_PRINTF1(_L("Successfully Message header ReStored"));

	}


CTestSuite* CTestHeaderStoreReStore::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestStoreHeaderL);
	ADD_ASYNC_TEST_STEP(TestStoreReStoreSMSHeaderL);
	ADD_ASYNC_TEST_STEP(TestStoreReStoreEOSSMSHeaderL);
	ADD_ASYNC_TEST_STEP(TestStoreReStoreEmptyEOSSMSHeaderL);
	ADD_ASYNC_TEST_STEP(TestMultipuleEOSRecipientSMSHeaderL);
	ADD_ASYNC_TEST_STEP(TestEmptyEOSSubjectSMSHeaderL);
	END_SUITE;
	}
