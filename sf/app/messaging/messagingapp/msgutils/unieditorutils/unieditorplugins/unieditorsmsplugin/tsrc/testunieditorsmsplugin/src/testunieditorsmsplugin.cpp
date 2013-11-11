/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * This component and the accompanying materials are made available
 * under the terms of "Eclipse Public License v1.0"
 * which accompanies this distribution, and is available
 * at the URL "http://www.eclipse.org/legal/epl-v10.html".
 *
 * Initial Contributors:
 * Nokia Corporation - initial contribution.
 *
 * Contributors:
 * 
 * Description: Main test class definition for UnieditorSMSPlugin
 */

#include <QtTest/QtTest>
#include <QTimer>
#include <QSignalSpy>
#include "debugtraces.h"
#include <etelmm.h>
#include <xqconversions.h>
#include <SenduiMtmUids.h>
#include <e32base.h>
#include <e32property.h>
#include <etelmm.h>
#include <es_ini.h>
#include <SenduiMtmUids.h>
#include <centralrepository.h>
#include <MessagingInternalCRKeys.h>    
#include <startupdomainpskeys.h>
#include "convergedmessage.h"
#include "convergedmessageid.h"
#include "unieditorsmsplugin.h"
#include "testunieditorsmsplugin.h"
#include "testunieditorsmsplugin.ini"

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::HandleSessionEventL
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
{
	
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::initTestCase
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::initTestCase()
{
	//Initialise SMS MTM
	initL();
	
	//Initialise Service Center Settings 
	createSCSettingsL();
	
	//Instantiate Unieditor SMS plugin
	msgPlugin = new UniEditorSmsPlugin;
    QVERIFY2(msgPlugin != NULL, "SMS Unieditor Plugin Not created");
    
    //Validate SMS services
    QVERIFY(msgPlugin->validateService() != 0);
	QVERIFY(msgPlugin->isServiceValid() != 0);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::init
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::init()
{
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::CreateSMS
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::CreateSMS()
{
	//Setting Encoding settings for Unicode
	msgPlugin->setEncodingSettings(EFalse, ESmsEncodingNone, -1);
			
	//Constructing a new SMS
	ConvergedMessage message;
	message.setBodyText(QString(TEST_MSG_BODY));
	
	ConvergedMessageAddress address1;
	address1.setAddress(QString(TEST_MSG_TO1));
	address1.setAlias(QString(TEST_MSG_ALIAS1));
	message.addToRecipient(address1);
	
	message.setPriority(ConvergedMessage::Normal);
	
	//Sending SMS to drafts folder
	smsId = msgPlugin->convertTo(&message);
	QVERIFY(smsId != -1);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::SendAndTestSMS
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::SendAndTestSMS()
{
	QTest::qWait(1000);
		
	//Checking SMS in drafts folder
	CMsvEntry* rootentry = iSession->GetEntryL(KMsvDraftEntryId);
	CMsvEntrySelection* messages = rootentry->ChildrenL();	
	TInt count = rootentry->Count();
	
	QVERIFY(count > 0);
	
	TMsvId draftMsgId = messages->At(count - 1);
	QVERIFY2(draftMsgId == smsId, "SMS Not Sent to Drafts Folder");
		
	ConvergedMessage* message = msgPlugin->convertFrom(draftMsgId);
	QVERIFY(message != NULL);
	QVERIFY(QString(TEST_MSG_TO1).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_MSG_ALIAS1).contains(message->fromAddress()->alias(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_MSG_BODY).compare(message->bodyText(), Qt::CaseInsensitive) == 0);
	
	TInt numOfRemainingChars;
	TInt numOfPDUs;
	TBool unicodeMode;
	TSmsEncoding alternativeEncodingType;
	
	QString bodyMessage = message->bodyText();
	QVERIFY(msgPlugin->getNumPDUs(bodyMessage, numOfRemainingChars, numOfPDUs, unicodeMode, alternativeEncodingType) == true);
	QVERIFY(numOfPDUs  == 1);
	QVERIFY(unicodeMode == 0);
	QVERIFY(alternativeEncodingType == ESmsEncodingNone);
	
	delete message;
	
	QVERIFY(msgPlugin->send(smsId) == true);
	
	QTest::qWait(1000);
			
	//Checking SMS in outbox folder
	rootentry = iSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
	messages = rootentry->ChildrenL();	
	count = rootentry->Count();
	
	QVERIFY(count > 0);
	TMsvId outboxMsgId = messages->At(count - 1);
	
	QVERIFY2(outboxMsgId == smsId, "SMS Not Sent to Outbox Folder");
	message = msgPlugin->convertFrom(outboxMsgId);
	QVERIFY(message != NULL);
	qDebug("conversion OK");
	QVERIFY(QString(TEST_MSG_TO1).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_MSG_ALIAS1).contains(message->fromAddress()->alias(), Qt::CaseInsensitive) == true);
	delete message;
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::CreateSMSWithTextAttachment
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::CreateSMSWithTextAttachment()
{
	msgPlugin->setEncodingSettings(ETrue, ESmsEncodingNone, -1);
			
	//Constructing SMS with text attachment
	ConvergedMessage message;
	
	ConvergedMessageAddress address;
	address.setAddress(QString(TEST_MSG_TO2));
	message.addToRecipient(address);
	
	message.setPriority(ConvergedMessage::Normal);
	
	ConvergedMessageAttachmentList attachmentList;    
	
	//Add a text attachment to attachment list. 
	QString attachmentPath = TEST_TEXT_ATTACHMENT;
	ConvergedMessageAttachment* attachment = 
		new ConvergedMessageAttachment(attachmentPath, ConvergedMessageAttachment::EAttachment);
	
	attachmentList.append(attachment);
	//Add attachments' list
	message.addAttachments(attachmentList);
	
	smsId = msgPlugin->convertTo(&message);
	QVERIFY(smsId != -1);
	
	QVERIFY(msgPlugin->send(smsId) == true);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::TestSMSWithTextAttachment
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::TestSMSWithTextAttachment()
{
	QTest::qWait(1000);
		
	CMsvEntry* rootentry = iSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
	CMsvEntrySelection* messages = rootentry->ChildrenL();	
	TInt count = rootentry->Count();
	
	QVERIFY(count > 0);
	TMsvId outboxMsgId = messages->At(count - 1);
		
	QVERIFY2(outboxMsgId == smsId, "SMS Not Sent to Outbox Folder");
	ConvergedMessage* message = msgPlugin->convertFrom(outboxMsgId);
	QVERIFY(message != NULL);
	QVERIFY(QString(TEST_MSG_TO2).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	delete message;

	qDebug("Text Attachment SMS Sending passed");
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::CreateSMSWithVCardAttachment
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::CreateSMSWithVCardAttachment()
{
	msgPlugin->setEncodingSettings(ETrue, ESmsEncodingNone, -1);
		
	//Constructing SMS with VCard attachment
	ConvergedMessage message;
	
	ConvergedMessageAddress address;
	address.setAddress(QString(TEST_MSG_TO2));
	message.addToRecipient(address);
	
	message.setPriority(ConvergedMessage::Normal);
	
	ConvergedMessageAttachmentList attachmentList;    
	
	//Add a vcard attachment to attachment list. 
	QString attachmentPath = TEST_VCARD_ATTACHMENT;
	ConvergedMessageAttachment* attachment = 
		new ConvergedMessageAttachment(attachmentPath, ConvergedMessageAttachment::EAttachment);
	
	attachmentList.append(attachment);
	//Add attachments' list
	message.addAttachments(attachmentList);
	
	smsId = msgPlugin->convertTo(&message);
	QVERIFY(smsId != -1);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::TestSMSWithVCardAttachment
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::TestSMSWithVCardAttachment()
{
	QTest::qWait(1000);
	
	//Verifying a SMS with VCard attachment
	CMsvEntry* rootentry = iSession->GetEntryL(KMsvDraftEntryId);
	CMsvEntrySelection* messages = rootentry->ChildrenL();	
	TInt count = rootentry->Count();
	
	QVERIFY(count > 0);

	TMsvId draftMsgId = messages->At(count - 1);
	QVERIFY2(draftMsgId == smsId, "SMS Not Sent to Drafts Folder");
		
	ConvergedMessage* message = msgPlugin->convertFrom(draftMsgId);
	QVERIFY(message != NULL);
	QVERIFY(QString(TEST_MSG_TO2).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(message->attachments().count() == 1);
	QVERIFY(message->attachments()[0]->attachmentType() == ConvergedMessageAttachment::EAttachment);
	QVERIFY(message->attachments()[0]->filePath().contains(QString(TEST_VCARD_ATTACHMENT).mid(QString(TEST_VCARD_ATTACHMENT).indexOf(QString("TestUnieditorSMSPluginSample.vcf"), 0, Qt::CaseInsensitive)), Qt::CaseInsensitive) == true);
	delete message;
	message = NULL;
	qDebug("VCard Attachment SMS passed");
	
	msgPlugin->deleteDraftsEntry(draftMsgId);
	
	message = msgPlugin->convertFrom(draftMsgId);
	QVERIFY(message == NULL);
	
	qDebug("Draft SMS deleted Sucessfully");
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::CreateSMSWithVCalAttachment
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::CreateSMSWithVCalAttachment()
{
	msgPlugin->setEncodingSettings(EFalse, ESmsEncodingNone, -1);
			
	//Constructing SMS with VCal attachment
	ConvergedMessage message;
	
	ConvergedMessageAddress address;
	address.setAddress(QString(TEST_MSG_TO1));
	message.addToRecipient(address);
	
	message.setPriority(ConvergedMessage::Normal);
	
	ConvergedMessageAttachmentList attachmentList;    
	
	//Add a vcal attachment to attachment list. 
	QString attachmentPath = TEST_VCAL_ATTACHMENT;
	ConvergedMessageAttachment* attachment = 
		new ConvergedMessageAttachment(attachmentPath, ConvergedMessageAttachment::EAttachment);
	
	attachmentList.append(attachment);
	//Add attachments' list
	message.addAttachments(attachmentList);
	
	smsId = msgPlugin->convertTo(&message);
	QVERIFY(smsId != -1);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::TestSMSWithVCalAttachment
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::TestSMSWithVCalAttachment()
{
	QTest::qWait(1000);
	
	//Verifying a SMS with VCal attachment
	CMsvEntry* rootentry = iSession->GetEntryL(KMsvDraftEntryId);
	CMsvEntrySelection* messages = rootentry->ChildrenL();	
	TInt count = rootentry->Count();
	
	QVERIFY(count > 0);
	
	TMsvId draftMsgId = messages->At(count - 1);
	QVERIFY2(draftMsgId == smsId, "SMS Not Sent to Drafts Folder");
	
	ConvergedMessage* message = msgPlugin->convertFrom(draftMsgId);
	QVERIFY(message != NULL);
	QVERIFY(QString(TEST_MSG_TO1).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(message->attachments().count() == 1);
	QVERIFY(message->attachments()[0]->attachmentType() == ConvergedMessageAttachment::EAttachment);
	QVERIFY(message->attachments()[0]->filePath().contains(QString(TEST_VCAL_ATTACHMENT).mid(QString(TEST_VCAL_ATTACHMENT).indexOf(QString("TestUnieditorSMSPluginSample.vcs"), 0, Qt::CaseInsensitive)), Qt::CaseInsensitive) == true);
	delete message;
	message = NULL;
	
	qDebug("VCal Attachment SMS passed");
	
	msgPlugin->deleteDraftsEntry(draftMsgId);
	
	message = msgPlugin->convertFrom(draftMsgId);
	QVERIFY(message == NULL);
	
	qDebug("Draft SMS deleted Sucessfully");
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::CreateEmailSMS
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::CreateEmailSMS()
{
	//Constructing a SMS - Email
	ConvergedMessage message;
		
	ConvergedMessageAddress address1;
	address1.setAddress(QString(TEST_MSG_TO3));
	message.addToRecipient(address1);
	
	ConvergedMessageAddress address2;
	address2.setAddress(QString(TEST_MSG_TO2));
	message.addToRecipient(address2);
	
	message.setPriority(ConvergedMessage::Normal);
	
	message.setSubject(QString(TEST_MSG_SUBJECT));
	
	message.setBodyText(QString(TEST_MSG_BODY));
	
	smsId = msgPlugin->convertTo(&message);
	QVERIFY(smsId != -1);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::TestEmailSMS
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::TestEmailSMS()
{
	QTest::qWait(1000);
			
	//Verifying Email SMS
	CMsvEntry* rootentry = iSession->GetEntryL(KMsvDraftEntryId);
	CMsvEntrySelection* messages = rootentry->ChildrenL();	
	TInt count = rootentry->Count();
	
	QVERIFY(count > 0);
	
	TMsvId draftMsgId = messages->At(count - 1);
	QVERIFY2(draftMsgId == smsId, "SMS Not Sent to Drafts Folder");
		
	ConvergedMessage* message = msgPlugin->convertFrom(draftMsgId);
	QVERIFY(message != NULL);
	QVERIFY(QString(TEST_MSG_TO1).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_MSG_BODY).compare(message->bodyText(), Qt::CaseInsensitive) == 0);
	QVERIFY(QString(TEST_MSG_SUBJECT).compare(message->subject(), Qt::CaseInsensitive) == 0);
	delete message;
	
	qDebug("Creation EMail passed");
	
	QVERIFY(msgPlugin->send(smsId) == true);
	
	QTest::qWait(1000);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::CreateSMSForExtraOptions
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::CreateSMSForExtraOptions()
{
	ConvergedMessage message;
	message.setBodyText(QString(TEST_MSG_BODY));
	
	ConvergedMessageAddress address1;
	address1.setAddress(QString(TEST_MSG_TO2));
	message.addToRecipient(address1);
	
	ConvergedMessageAddress address2;
	address2.setAddress(QString(TEST_MSG_TO1));
	message.addFromRecipient(address2);
	
	message.setPriority(ConvergedMessage::Normal);
	
	smsId = msgPlugin->convertTo(&message);
	QVERIFY(smsId != -1);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::TestSMSForExtraOptions
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::TestSMSForExtraOptions()
{
	QTest::qWait(1000);
				
	CMsvEntry* rootentry = iSession->GetEntryL(KMsvDraftEntryId);
	CMsvEntrySelection* messages = rootentry->ChildrenL();	
	TInt count = rootentry->Count();
	
	QVERIFY(count > 0);
	
	TMsvId draftMsgId = messages->At(count - 1);
	QVERIFY2(draftMsgId == smsId, "SMS Not Sent to Drafts Folder");
		
	QVERIFY(msgPlugin->messageType() == ConvergedMessage::Sms);
	
	//Verifying SMS for Forwarding
	ConvergedMessage* message = msgPlugin->convertFrom(draftMsgId, UniEditorPluginInterface::Forward);
	QVERIFY(message != NULL);
	QVERIFY(QString(TEST_MSG_TO1).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(message->toAddressList().count() == 1);
	QVERIFY(message->toAddressList()[0]->address().contains(QString(TEST_MSG_TO2), Qt::CaseInsensitive) == true);
	QVERIFY(QString(TEST_MSG_BODY).compare(message->bodyText(), Qt::CaseInsensitive) == 0);
	delete message;
	
	qDebug("SMS for Forward Passed");
	
	//Verifying SMS for Reply from Drafts
	message = msgPlugin->convertFrom(draftMsgId, UniEditorPluginInterface::Reply);
	QVERIFY(message != NULL);
	QVERIFY(QString(TEST_MSG_TO1).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(message->toAddressList().count() == 1);
	QVERIFY(message->toAddressList()[0]->address().contains(QString(TEST_MSG_TO2), Qt::CaseInsensitive) == true);
	QVERIFY(message->bodyText().contains(QString(TEST_MSG_BODY), Qt::CaseInsensitive) == true);
	delete message;
	
	qDebug("SMS for Reply - ESmsSubmit Passed");
	
	QVERIFY(msgPlugin->send(smsId) == true);
	
	QTest::qWait(1000);
			
	rootentry = iSession->GetEntryL(KMsvGlobalOutBoxIndexEntryId);
	messages = rootentry->ChildrenL();	
	count = rootentry->Count();
	
	QVERIFY(count > 0);
	TMsvId outboxMsgId = messages->At(count - 1);
	
	//Verifying SMS for Reply when SMS is sent
	message = msgPlugin->convertFrom(outboxMsgId, UniEditorPluginInterface::Reply);
	QVERIFY(message != NULL);
	QVERIFY(QString(TEST_MSG_TO1).contains(message->fromAddress()->address(), Qt::CaseInsensitive) == true);
	QVERIFY(message->toAddressList().count() == 1);
	QVERIFY(message->toAddressList()[0]->address().contains(QString(TEST_MSG_TO2), Qt::CaseInsensitive) == true);
	QVERIFY(message->bodyText().compare(QString(""), Qt::CaseInsensitive) == 0);
	delete message;
	
	qDebug("SMS for Reply - ESmsDeliver Passed");	
	
	iSmsClientMtm->Entry().DeleteL(smsId);
	QTest::qWait(1000);
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::cleanup
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::cleanup()
{
	
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::cleanupTestCase
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::cleanupTestCase()
{
	//Cleanup
	delete msgPlugin;
    
    if(iSmsClientMtm)
	{
		delete iSmsClientMtm;
		iSmsClientMtm = NULL;
	}
	
	if (iMtmRegistry)
	{
		delete iMtmRegistry;
		iMtmRegistry = NULL;
	}
 
	if (iSession)
	{
		delete iSession;
		iSession = NULL;
	}

	if (iCenRepSession)
	{
		delete iCenRepSession;
		iCenRepSession = NULL;
	}
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::createSCSettingsL
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::createSCSettingsL()
{
	qDebug("TestUnieditorSMSPlugin::createSCSettingsL enter");
		
	TMsvSelectionOrdering ordering;

	CMsvEntry* root = CMsvEntry::NewL(iSmsClientMtm->Session(),
									  KMsvRootIndexEntryId,
									  ordering);

	CSmsAccount* smsAccount = CSmsAccount::NewLC();

	CSmsSettings* settings = CSmsSettings::NewL();
	CleanupStack::PushL(settings);
	smsAccount->LoadSettingsL(*settings);

	TMsvEntry entry;
	entry.iMtm = KUidMsgTypeSMS;
	entry.iType = KUidMsvServiceEntry;
	entry.SetReadOnly(EFalse);
	entry.SetVisible(EFalse);
	entry.iDate.UniversalTime();
	entry.iDetails.Set(_L("Nokiatest"));

	root->SetEntryL(KMsvRootIndexEntryId);
	root->CreateL(entry);

	QString serviceCenter01(SERVICE_CENTER_01);
	QString serviceCenter02(SERVICE_CENTER_02);
	HBufC* sC01 = XQConversions::qStringToS60Desc(serviceCenter01);
	HBufC* sC02 = XQConversions::qStringToS60Desc(serviceCenter02);
	CleanupStack::PushL(sC01);
	CleanupStack::PushL(sC02);
	settings->AddServiceCenterL(_L("Nokia"), *sC01);
	settings->AddServiceCenterL(_L("Nokia"), *sC02);
	CleanupStack::PopAndDestroy(sC02);
	CleanupStack::PopAndDestroy(sC01);
	settings->SetDefaultServiceCenter(1);
	settings->SetValidityPeriod(ESmsVPWeek);
	settings->SetReplyQuoted(ETrue);
	settings->SetRejectDuplicate(ETrue);
	settings->SetDelivery(ESmsDeliveryImmediately);
	settings->SetDeliveryReport(EFalse);
	settings->SetReplyPath(EFalse);
	settings->SetMessageConversion(ESmsConvPIDNone);
	settings->SetCanConcatenate(ETrue);
	settings->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet7Bit);
	settings->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger);
	settings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
	settings->SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
	settings->SetSmsBearer(CSmsSettings::ESmsBearerCircuitPreferred);
	settings->SetCommDbAction(CSmsSettings::ENone);
	settings->SetSmsBearerAction(CSmsSettings::ENone);
	
	// Save settings
	CMsvStore* msvstore = root->EditStoreL();
	CleanupStack::PushL(msvstore);

	TInt maxTries(5);
	TBool done(EFalse);
	while (maxTries && !done)
	{
		qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL b4");
		TRAPD( err, smsAccount->SaveSettingsL( *settings ) );	
		qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL after");
		if (err == KErrNone)
		{
			qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL settings saved");

			done = ETrue;
		}
		else if (err == KErrLocked)
		{
			qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL KErrLocked");

			// Wait a while and retry.
			User::After(100000); // 0.1 seconds
			maxTries--;
		}
		else
		{
		qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL leaving - ");
		qDebug() << err;
			User::Leave(err);
		}
	}

	msvstore->CommitL();
	CleanupStack::PopAndDestroy(msvstore);
	
	CleanupStack::PopAndDestroy(settings);
	CleanupStack::PopAndDestroy(smsAccount);
	
	qDebug("TestUnieditorSMSPlugin::createSCSettingsL exit");
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::initL
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::initL()
{
	iCenRepSession = CRepository::NewL(KCRUidSmum);

    // initialise
    iSession = CMsvSession::OpenSyncL(*this);

    // Create the SMS Service   
    TMsvId serviceId = CreateSmsServiceL();

    // Update the SMS cenrep with the default settings.
    CreateDefaultSettingsL(serviceId);

    iMtmRegistry = CClientMtmRegistry::NewL(*iSession); 
    iSmsClientMtm = STATIC_CAST( CSmsClientMtm*, iMtmRegistry->NewMtmL( KUidMsgTypeSMS ));
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::CreateSmsServiceL
//---------------------------------------------------------------
TMsvId TestUnieditorSMSPlugin::CreateSmsServiceL()
    {
	qDebug("TestUnieditorSMSPlugin::CreateSmsServiceL enter");
		 
	TMsvId serviceEntryId = KMsvNullIndexEntryId;
	TInt err = KErrNone;
	TRAP( err, serviceEntryId = ServiceIdL());

	// If no service, create one
	if (err == KErrNotFound)
		{
		TMsvEntry entry;
		entry.iMtm = KUidMsgTypeSMS;
		entry.iType = KUidMsvServiceEntry;
		entry.SetReadOnly(EFalse);
		entry.SetVisible(EFalse);
		entry.iDate.HomeTime();
		entry.iDetails.Set(KSmsService);
		CMsvEntry* root = iSession->GetEntryL(KMsvRootIndexEntryId);
		CleanupStack::PushL(root);

		// In case no root store, create one...
		if (!root->HasStoreL())
			{
			// --- The entry does not have a store. EditStoreL() will create one ---
			CMsvStore* store = root->EditStoreL();
			CleanupStack::PushL(store);
			store->CommitL();
			CleanupStack::PopAndDestroy(); // store
			store = NULL; // destroyed
			}
		root->CreateL(entry);
		CleanupStack::PopAndDestroy(); // root
		serviceEntryId = entry.Id();

		}
	
	qDebug("TestUnieditorSMSPlugin::CreateSmsServiceL exit");
	return serviceEntryId;
	}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::ServiceIdL
//---------------------------------------------------------------
TMsvId TestUnieditorSMSPlugin::ServiceIdL()
{
	qDebug("TestUnieditorSMSPlugin::ServiceIdL enter");
		
	TMsvId id = KMsvNullIndexEntryId;
	CMsvEntry* root = iSession->GetEntryL(KMsvRootIndexEntryId);
	CleanupStack::PushL(root);
	TSmsUtilities::ServiceIdL(*root, id);
	CleanupStack::PopAndDestroy(root);
	
	qDebug("TestUnieditorSMSPlugin::ServiceIdL exit");
	return id;
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::CreateDefaultSettingsL
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::CreateDefaultSettingsL(TMsvId aServiceId)
{
	qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL enter");

	CSmsSettings* serviceSettings = CSmsSettings::NewL();
	CleanupStack::PushL(serviceSettings);
	CSmsAccount* smsAccount = CSmsAccount::NewLC();

	// Read the RFS related settings from shared data.
	TInt originalCount = 0;
	smsAccount->LoadSettingsL(*serviceSettings);
	originalCount = serviceSettings->ServiceCenterCount();

	if (!originalCount)
		{
		qDebug("Original count = 0");

		ReadDefaultSettingsFromSharedDataL(serviceSettings);

		// Rest of the sms settings, which are fixed.
		serviceSettings->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFInteger); //relative
		serviceSettings->SetDelivery(ESmsDeliveryImmediately);
		serviceSettings->SetCanConcatenate(ETrue);
		serviceSettings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxInvisible);
		serviceSettings->SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);
		serviceSettings->SetRejectDuplicate(ETrue);
		TInt descriptionLength = KSmsDescriptionLength;

		// Read the value for description length 
		CRepository* repository = CRepository::NewLC(KCRUidMuiuSettings);
		if (KErrNone == repository->Get(KMuiuDescriptionLength,
				descriptionLength))
			{
			//Make sure value is not zero
			descriptionLength = Max(descriptionLength, KSmsDescriptionLength);
			}
		CleanupStack::PopAndDestroy(); // repository
		serviceSettings->SetDescriptionLength(descriptionLength);

		// Set saving to commsdb
		serviceSettings->SetCommDbAction(CSmsSettings::EStoreToCommDb);
		serviceSettings->SetSmsBearerAction(CSmsSettings::EStoreToCommDb);

		}

	// Save settings
	CMsvEntry* service = iSession->GetEntryL(aServiceId);
	CleanupStack::PushL(service);
	CMsvStore* msvstore = service->EditStoreL();
	CleanupStack::PushL(msvstore);

	TInt maxTries(5);
	TBool done(EFalse);
	while (maxTries && !done)
		{
		TRAPD( err, smsAccount->SaveSettingsL( *serviceSettings ) );
		if (err == KErrNone)
			{
			qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL settings saved");

			done = ETrue;
			}
		else if (err == KErrLocked)
			{
			qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL KErrLocked");

			// Wait a while and retry.
			User::After(100000); // 0.1 seconds
			maxTries--;
			}
		else
			{
			User::Leave(err);
			}
		}

	msvstore->CommitL();
	CleanupStack::PopAndDestroy(2); // msvstore, service
	CleanupStack::PopAndDestroy(2); // serviceSettings, smsAccount

	qDebug("TestUnieditorSMSPlugin::CreateDefaultSettingsL Exit"); 
}

//---------------------------------------------------------------
// TestUnieditorSMSPlugin::ReadDefaultSettingsFromSharedDataL
//---------------------------------------------------------------
void TestUnieditorSMSPlugin::ReadDefaultSettingsFromSharedDataL(CSmsSettings* aServiceSettings)
    {
	qDebug("TestUnieditorSMSPlugin::ReadDefaultSettingsFromSharedDataL Enter"); 

	if ( iCenRepSession )
		{
		TInt readedSetting;

		// Delivery report 
		if (iCenRepSession->Get(KSmumDeliveryReport, readedSetting) != KErrNone)
			{
			readedSetting = KDefDeliveryReport;
			}
		aServiceSettings->SetDeliveryReport(readedSetting);

		// Validity period
		if (iCenRepSession->Get(KSmumValidityPeriod, readedSetting) != KErrNone)
			{
			readedSetting = KDefValidityPeriod;
			}
		aServiceSettings->SetValidityPeriod(readedSetting);

		// Message conversion
		if (iCenRepSession->Get(KSmumMessageConversion, readedSetting)
				!= KErrNone)
			{
			readedSetting = KDefMessageConversion;
			}
		aServiceSettings->SetMessageConversion((TSmsPIDConversion) readedSetting);

		// Preferred connection
		if (iCenRepSession->Get(KSmumPreferredConnection, readedSetting)
				!= KErrNone)
			{
			readedSetting = KDefPreferredConnection;
			}
		aServiceSettings->SetSmsBearer((CSmsSettings::TMobileSmsBearer) readedSetting);

		// Reply via same centre 
		if (iCenRepSession->Get(KSmumRemoveReplyViaSameCentre, readedSetting)
				!= KErrNone)
			{
			if (iCenRepSession->Get(KSmumReplyViaSameCentre, readedSetting)
					!= KErrNone)
				{
				readedSetting = KDefReplyViaSameCentre;
				}
			}
		else
			{
			if (!readedSetting)
				{
				if (iCenRepSession->Get(KSmumReplyViaSameCentre, readedSetting)
						!= KErrNone)
					{
					readedSetting = KDefReplyViaSameCentre;
					}
				}
			}
		aServiceSettings->SetReplyPath(readedSetting);
		}

	qDebug("TestUnieditorSMSPlugin::ReadDefaultSettingsFromSharedDataL Exit"); 
    }

//---------------------------------------------------------------
// main
// main entry point
//---------------------------------------------------------------
int main(int argc, char *argv[])
    { 
	int ret = -1;
	QCoreApplication app(argc, argv);
	QObject* tc = new TestUnieditorSMSPlugin();
	
#ifdef __WINSCW__
	char *new_argv[3]; 
	QString str = "C:\\data\\" + QFileInfo(QCoreApplication::applicationFilePath()).baseName() + ".log";	
	QByteArray bytes = str.toAscii();	
	char arg1[] = "-o";	
	new_argv[0] = argv[0];	
	new_argv[1] = arg1;	
	new_argv[2] = bytes.data();	
	ret = QTest::qExec(tc, 3, new_argv);
#else
	ret = QTest::qExec(tc, argc, argv);	
#endif
	
	delete tc;
	return ret;
    }
