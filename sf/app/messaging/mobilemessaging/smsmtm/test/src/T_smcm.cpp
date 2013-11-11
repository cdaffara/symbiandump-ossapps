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
#include "T_smcm.h"
#include <e32math.h>
#include <csmsemailfields.h>
#include "emstestutils.h"
#include <emsformatie.h>
#include <emspictureie.h>
#include <csmsaccount.h>

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
	
	CSmcmTest* SmcmTest = CSmcmTest::NewLC(*smsTest, KNullDesC, nextTest);

	SmcmTest->StartL();

	CleanupStack::PopAndDestroy(3); //SmcmTest. scheduler

	}

CSmcmTest::CSmcmTest(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
: CSmsTestBase(aSmsTest, aScriptFile, aNextTest)
	{
	}

void CSmcmTest::RunAutoL()
	{
	iSmsTest.TestStart(++iNextTest, _L("CSmsSettings"));
	TestSettingsL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsClientMtm Capabilities"));
	TestCapabilitiesL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsClientMtm Global Find"));
	TestFindL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsClientMtm Find Alias"));
	TestFindAliasL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsClientMtm Reply"));
	TestReplyL();
	TestReplyEmailL();
	TestReplyWithOverloadedSaveMessageL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsClientMtm Forward"));
	TestForwardL();
	TestForwardEmailL();
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsClientMtm Async Functions"));
// There are no async function in sms client mtm
	iSmsTest.TestFinish(iNextTest);

	iSmsTest.TestStart(++iNextTest, _L("CSmsClientMtm Sync Functions"));
	TestSyncFunctionsL();
	iSmsTest.TestFinish(iNextTest);
	}

CSmcmTest::~CSmcmTest()
	{
	delete iSmsHeader;
	delete iRichText;
	delete iParaLayer;
	delete iCharLayer;
	}

void CSmcmTest::ConstructL()
	{
	iSmsTest.SetLogToFile();				// Lets log to a file
	iSmsTest.InstantiateClientMtmsL();

	iSelection = new (ELeave) CMsvEntrySelection();
	iTimer = CTestTimer::NewL();

	iParaLayer =CParaFormatLayer::NewL();	// Rich Text for the body part of the message
	iCharLayer = CCharFormatLayer::NewL();
	iRichText = CRichText::NewL(iParaLayer, iCharLayer, CEditableText::EFlatStorage,256);

	CActiveScheduler::Add(this);
	}

CSmcmTest* CSmcmTest::NewLC(CSmsTestUtils& aSmsTest, const TDesC& aScriptFile, TInt& aNextTest)
	{
	CSmcmTest* self = new (ELeave) CSmcmTest(aSmsTest, aScriptFile, aNextTest);
	CleanupStack::PushL(self);

	self->ConstructL();
	return self;
	}

void CSmcmTest::ShowMenuL()
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

void CSmcmTest::TestSettingsL()
	{
	iSmsTest.Printf(_L("Testing Create Settings...\n"));
	MsvEntry().SetEntryL(iSmsTest.iSmsServiceId);

// Lets create sms settings (just in case it does not exist yet)

	CSmsSettings* settings = CSmsSettings::NewL();
	CleanupStack::PushL(settings);
	CSmsAccount* account = CSmsAccount::NewLC();
	account->InitialiseDefaultSettingsL(*settings);
	settings->AddServiceCenterL(_L("Radiolinja0"),_L("+358508771010"));
	settings->AddServiceCenterL(_L("Radiolinja1"),_L("+358508771010"));
	settings->AddServiceCenterL(_L("Nokia"),_L("+358454400050"));
	settings->SetDefaultServiceCenter(2);
	iSmsTest(settings->DefaultServiceCenter()==2);
	settings->SetValidityPeriod(ESmsVPWeek);
	settings->SetReplyQuoted(ETrue);
	settings->SetRejectDuplicate(ETrue);
	settings->SetDelivery(ESmsDeliveryImmediately);
	settings->SetDeliveryReport(ETrue);
	settings->SetReplyPath(EFalse);
	settings->SetMessageConversion(ESmsConvPIDNone);
	settings->SetCanConcatenate(ETrue);
	settings->SetCharacterSet(TSmsDataCodingScheme::ESmsAlphabet8Bit);
	settings->SetValidityPeriodFormat(TSmsFirstOctet::ESmsVPFSemiOctet);
	settings->SetStatusReportHandling(CSmsSettings::EMoveReportToInboxVisible);
	settings->SetSpecialMessageHandling(CSmsSettings::EMoveReportToInboxVisible);

	account->SaveSettingsL(*settings);
	CleanupStack::PopAndDestroy(account);

	iSmsTest.iServiceSettings->CopyL(*settings);
	CleanupStack::PopAndDestroy(settings); 
	}

// Now we shall test the recipient handling, find and validate features

void CSmcmTest::TestFindL()
	{
	iSmsTest.Test().Next(_L("Testing CSmsClientMtm Global Find"));

// Before testing the find and validate we need a dummy message to play with

	TMsvPartList partList;
	TMsvId id = iSmsTest.CreateDummyMessageToSendL();
	Client().SwitchCurrentEntryL(id);

// Lets load the message we just created
	Client().LoadMessageL();

// And add some recipients
	Client().AddAddresseeL(_L("+358503367709"), _L("Mico3"));
	Client().AddAddresseeL(_L("+358503367709"), _L("Mico4"));
	Client().AddAddresseeL(_L("//\\"), _L("\n"));
	Client().AddAddresseeL(_L("\n0"), _L("^"));
	Client().AddAddresseeL(_L("1234"), _L("~¨"));
	Client().AddAddresseeL(_L("äöåÖÅÖÖ¤"), _L("fdsa4324521{[]}"));
	Client().AddAddresseeL(_L("äöåÖÅÖÖ¤"), _L(""));
	Client().AddAddresseeL(_L("äöåÖÅÖÖ¤"));
	
// Remove Recipient
	Client().RemoveAddressee(2);

// Validating Message
	partList=KMsvMessagePartRecipient;
	iSmsTest(Client().ValidateMessage(partList));   // All the addresses are not valid

	Client().RemoveAddressee(Client().AddresseeList().Count()-1);
	Client().RemoveAddressee(Client().AddresseeList().Count()-1);
	Client().RemoveAddressee(Client().AddresseeList().Count()-1);
	Client().RemoveAddressee(Client().AddresseeList().Count()-2);
	Client().RemoveAddressee(Client().AddresseeList().Count()-2);

	partList=KMsvMessagePartRecipient;			// supported
	iSmsTest(!Client().ValidateMessage(partList));   
	partList=KMsvMessagePartBody;				// not supported, returns 0
	iSmsTest(!Client().ValidateMessage(partList));	
	partList=KMsvMessagePartOriginator;			// not supported, returns 0
	iSmsTest(!Client().ValidateMessage(partList));
	partList=KMsvMessagePartDescription;		// not supported, returns 0
	iSmsTest(!Client().ValidateMessage(partList));
	partList=KMsvMessagePartDate;				// not supported, returns 0
	iSmsTest(!Client().ValidateMessage(partList));
	partList=KMsvMessagePartAttachments;		// not supported, returns 0
	iSmsTest(!Client().ValidateMessage(partList));
	partList=KMsvMessagePartBody|KMsvMessagePartRecipient;
	iSmsTest(!Client().ValidateMessage(partList));
	iSmsTest.Printf(_L("Test Validate Passed\n"));

// Find in body and recipients
	Client().AddAddresseeL(_L("+358503367709"), _L("Mr. Tester"));
	partList=KMsvMessagePartRecipient;
	iSmsTest(Client().Find(_L("est"),partList)==KMsvMessagePartRecipient);
	partList=KMsvMessagePartBody;				
	iSmsTest(Client().Find(_L("end"),partList)==KMsvMessagePartBody);	
	partList=KMsvMessagePartOriginator;			
	iSmsTest(!Client().Find(_L("end"),partList));
	partList=KMsvMessagePartDescription;		// not supported, returns 0
	iSmsTest(Client().Find(_L("end"),partList));
	partList=KMsvMessagePartDate;				// not supported, returns 0
	iSmsTest(!Client().Find(_L("end"),partList));
	partList=KMsvMessagePartAttachments;		// not supported, returns 0
	iSmsTest(!Client().Find(_L("end"),partList));
	partList=KMsvMessagePartBody|KMsvMessagePartRecipient;
//	resultList=Client().Find(_L(""),partList);		// crashes with empty string
	iSmsTest(!Client().Find(_L("\n"),partList));
	iSmsTest(!Client().Find(_L("äöå"),partList));
	iSmsTest(!Client().Find(_L("{[}]"),partList));
	iSmsTest(!Client().Find(_L("\\"),partList));
	iSmsTest(!Client().Find(_L("4352316542631753dfgfdgs4t6543w563vsd4"),partList));
	iSmsTest( Client().Find(_L("~"),partList));
	iSmsTest(!Client().Find(_L("^"),partList));
	iSmsTest(!Client().Find(_L("   \"dsasaf¤;¨"),partList));
	iSmsTest(!Client().Find(_L("\\"),partList));
	iSmsTest(!Client().Find(_L(" "),partList)); // alt+255
	iSmsTest(Client().Find(_L("¨"),partList));
	iSmsTest(!Client().Find(_L("/\\"),partList));
	iSmsTest(!Client().Find(_L("\\ "),partList));
	iSmsTest.Printf(_L("Test Find Passed\n"));
	}

void CSmcmTest::TestCapabilitiesL()
	{
// Query MTM's capabilities
	TInt response;
	iSmsTest(Client().QueryCapability(KUidMtmQueryMaxBodySize,response)==KErrNone);
//	iSmsTest(response==KSmscMaxTotalMsgSize);
	iSmsTest(Client().QueryCapability(KUidMtmQueryMaxTotalMsgSize,response)==KErrNone);
//	iSmsTest(response==KSmcmMaxMessageNumber*KSmcmMaxCharsInMessageConcatenated7Bit);
	iSmsTest(Client().QueryCapability(KUidMtmQuerySupportedBody,response)==KErrNone);
	iSmsTest(response==KMtm7BitBody + KMtm8BitBody + KMtm16BitBody);
	iSmsTest(Client().QueryCapability(KUidMtmQuerySupportAttachments,response)==KErrNotSupported);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMtmQuerySupportSubject,response)==KErrNotSupported);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMtmQuerySupportsFolder,response)==KErrNotSupported);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMtmQueryOffLineAllowed,response)==KErrNotSupported);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMtmQueryCanSendMsg,response)==KErrNone);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMtmQueryCanReceiveMsg,response)==KErrNone);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMtmQueryMaxRecipientCount,response)==KErrNone);
	iSmsTest(response==-1);
	iSmsTest(Client().QueryCapability(KUidMtmQuerySendAsRequiresRenderedImage,response)==KErrNotSupported);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMtmQuerySendAsRenderingUid,response)==KErrNotSupported);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMsvMtmQueryEditorUid,response)==KErrNone);
	iSmsTest(response==268441151); // 0x1000163f in hex form
	iSmsTest(Client().QueryCapability(KUidMsvQuerySupportsBioMsg,response)==KErrNone);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidMsvQuerySupportsScheduling,response)==KErrNone);
	iSmsTest(response);
	iSmsTest(Client().QueryCapability(KUidSmcmTestNonSense,response)==KErrNotSupported);
	iSmsTest(response);
	iSmsTest.Printf(_L("Test Query Capabilities Passed\n"));
	}

// Now testing find alias
void CSmcmTest::TestFindAliasL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Find Alias"));
	iSmsTest.Printf(_L("This test is INCOMPLETE\n"));

	TMsvEntry entry;
	entry.iType=KUidMsvMessageEntry;
	entry.SetSendingState(KMsvSendStateNotApplicable);
	
	// Begin extracted code
	iSmsTest.iRichText->Reset();
	iSmsTest.iRichText->InsertL(0,_L("Test message"));
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iSmsTest.iRichText);
	CleanupStack::PushL(header);

	header->SetFromAddressL(KScriptItemRecipientsDef);
	header->Message().SetServiceCenterAddressL(KScriptItemSCDef);

	entry.iDetails.Set(_L("host")); //set alias

	TTime now;
	now.HomeTime();
	iSmsTest.CreateMessageL(KMsvGlobalInBoxIndexEntryId, entry, *header, *iSmsTest.iRichText, now);
	entry.SetReadOnly(ETrue);
	iSmsTest.ChangeEntryL(entry);
	TMsvId id = entry.Id();
	CleanupStack::PopAndDestroy(header); //header

	// End extracted code

	iSmsTest.SetEntryL(id);

	Client().SwitchCurrentEntryL(id); // Lets point at the received message
	Client().LoadMessageL();

	TMsvPartList partList;
	partList=KMsvMessagePartOriginator;
	//Find alias
	iSmsTest(Client().Find(_L("host"),partList)==KMsvMessagePartOriginator);
	iSmsTest.Printf(_L("Alias Found\n"));
	iSmsTest.Printf(_L("Find Alias test passed\n"));
	}

// Now testing replying to received messages
void CSmcmTest::TestReplyL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Reply"));
	iSmsTest.Printf(_L("This test is INCOMPLETE\n"));

	TMsvEntry entry;
	entry.iType=KUidMsvMessageEntry;
	entry.SetSendingState(KMsvSendStateNotApplicable);
	
	// Begin extracted code
	iSmsTest.iRichText->Reset();
	iSmsTest.iRichText->InsertL(0,_L("This is a test message with formatting and pictures"));
	CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iSmsTest.iRichText);
	CleanupStack::PushL(header);

	// Add formatting and picture
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	CleanupStack::PushL(object);
	object->SetStartPosition(10);
	object->SetFormatLength(10);
	object->SetBold(ETrue);
	header->Message().AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	CEmsPictureIE* pic = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic);
	pic->SetStartPosition(20);
	header->Message().AddEMSInformationElementL(*pic);
	CleanupStack::PopAndDestroy(pic);

	header->SetFromAddressL(KScriptItemRecipientsDef);
	header->Message().SetServiceCenterAddressL(KScriptItemSCDef);

	TTime now;
	now.HomeTime();
	iSmsTest.CreateMessageL(KMsvGlobalInBoxIndexEntryId, entry, *header, *iSmsTest.iRichText, now);
	entry.SetReadOnly(ETrue);
	iSmsTest.ChangeEntryL(entry);
	TMsvId id = entry.Id();
	CleanupStack::PopAndDestroy(1); //header

	// End extracted code

	iSmsTest.SetEntryL(id);

	Client().SwitchCurrentEntryL(id); // Lets point at the received message
	Client().LoadMessageL();

	TMsvPartList part=0;
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	CMsvOperation* op=Client().ReplyL(KMsvDraftEntryId, part, wait->iStatus);
	CleanupStack::PushL(op);
	wait->Start();
	CActiveScheduler::Start();

	TPckgBuf<TMsvId> pkg;
	pkg.Copy(op->ProgressL());
	TMsvId progress = pkg();

	CleanupStack::PopAndDestroy(2); //op, wait

	// let's check to see if the inbox message and the reply are the same
	// (this call will compare ems objects as well
	if (!iSmsTest.AreMessagesSameTextL(id, progress, ETrue))
		{
		iSmsTest.Printf(_L("EMS objects do not compare!!\n"));
		}
	else
		{
		iSmsTest.Printf(_L("EMS objects compare OK.\n"));
		}
	
	Client().SwitchCurrentEntryL(progress);
	Client().LoadMessageL();
		
	Client().Body().InsertL(0,_L("This is reply"));
	Client().SaveMessageL();
	}
	
TMsvId CSmcmTest::CreateEmailSmsMessageL(const TDesC& aEmailMessageData)
	{
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	CSmsMessage* message = CSmsMessage::NewL(fs, CSmsPDU::ESmsDeliver, CSmsEditorBuffer::NewL(*iRichText), EFalse);
	CleanupStack::PushL(message);
	
	CSmsBufferBase& buffer = message->Buffer();
	buffer.Reset();
	buffer.InsertL(0, aEmailMessageData);
	
	// Set the PID for email.
	CSmsPDU& pdu = message->SmsPDU();
	if( pdu.ProtocolIdentifierPresent() )
		{
		pdu.SetPIDType(TSmsProtocolIdentifier::ESmsPIDTelematicInterworking);
		pdu.SetTelematicDeviceIndicator(TSmsProtocolIdentifier::ESmsTelematicDevice);
		pdu.SetTelematicDeviceType(TSmsProtocolIdentifier::ESmsInternetElectronicMail);
		}

	// Create a new SMS header
	CSmsHeader* header = CSmsHeader::NewL(message);
	CleanupStack::Pop(message);
	CleanupStack::PushL(header);
	
	header->SetFromAddressL(KScriptItemRecipientsDef);
	header->Message().SetServiceCenterAddressL(KScriptItemSCDef);
	
	// Set the entry data...
	TMsvEntry entry;
	entry.iType = KUidMsvMessageEntry;
	entry.SetSendingState(KMsvSendStateNotApplicable);
	TSmsUtilities::PopulateMsgEntry(entry, header->Message(), iSmsTest.iSmsServiceId);
	TTime now;
	now.HomeTime();
	entry.iDate = now;
	
	const CSmsEmailFields& fields = header->EmailFields();
	// Set the details fiels to the address field.
	iSmsTest(fields.HasAddress());
	entry.iDetails.Set(fields.Addresses().MdcaPoint(0).Left(KSmsDetailsLength));			
	
	// Set the description to either the subject or the begining part of the
	// body text.
	HBufC* desBuffer = HBufC::NewLC(Client().ServiceSettings().DescriptionLength());
	TPtr description(desBuffer->Des());
	if( fields.Subject().Length() > 0 )
		{
		// There is a subject - use this as the description
		description.Copy(fields.Subject().Left(description.MaxLength()));
		}
	else
		{
		// No subject - use the begining part of the body text.
		CSmsBufferBase& body = header->Message().Buffer();
		TInt length = body.Length();
		HBufC* buf = HBufC::NewLC(length);
		TPtr ptr(buf->Des());
		body.Extract(ptr, 0, length);

		description.Copy(ptr.Left(description.MaxLength()));
		CleanupStack::PopAndDestroy(buf);
		}
	entry.iDescription.Set(description);

	// Create the entry in the inbox.
	iSmsTest.SetEntryL(KMsvGlobalInBoxIndexEntryId);
	iSmsTest.CreateEntryL(entry);
	Session().CleanupEntryPushL(entry.Id());
	
	// Store the header...
	iSmsTest.SetEntryL(entry.Id());
	CMsvStore* store = iSmsTest.EditStoreL();
	CleanupStack::PushL(store);

	header->StoreL(*store);

	// Store the body text - need to insert into rich text object.
	iSmsTest.iRichText->Reset();
	TInt length = header->Message().Buffer().Length();
	HBufC* buf = HBufC::NewLC(length);

	TPtr ptr(buf->Des());
	header->Message().Buffer().Extract(ptr, 0, length);
	iSmsTest.iRichText->InsertL(0, ptr); 

	store->StoreBodyTextL(*iSmsTest.iRichText);
	store->CommitL();

	entry.iSize = store->SizeL();
	entry.SetReadOnly(ETrue);
	iSmsTest.ChangeEntryL(entry);
	
	CleanupStack::PopAndDestroy(2, store); // store, buf
	Session().CleanupEntryPop();
	CleanupStack::PopAndDestroy(3, &fs);	// fs, header, desBuffer
	
	return entry.Id();
	}
	
void CSmcmTest::DoTestReplyEmailL(const TDesC& aEmailMessageData, const TDesC& aAddress, const TDesC& aSubject)
	{
	// Create an email message and then a reply message to it.
	TMsvId id = CreateEmailSmsMessageL(aEmailMessageData);
	iSmsTest.SetEntryL(id);
	Client().SwitchCurrentEntryL(id); 
	Client().LoadMessageL();

	TMsvPartList part=0;
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	CMsvOperation* op=Client().ReplyL(KMsvDraftEntryId, part, wait->iStatus);
	CleanupStack::PushL(op);
	wait->Start();
	CActiveScheduler::Start();

	TPckgBuf<TMsvId> pkg;
	pkg.Copy(op->ProgressL());
	TMsvId reply = pkg();

	CleanupStack::PopAndDestroy(2, wait); // op, wait
	
	// Switch and load the reply message
	Client().SwitchCurrentEntryL(reply);
	Client().LoadMessageL();
	
	// Check the reply message...
	const CSmsHeader& replyHeader = Client().SmsHeader();
	
	// 1. Email fields...
	const CSmsEmailFields& fields = replyHeader.EmailFields();
	iSmsTest(fields.Addresses().MdcaCount() == 1);
	iSmsTest(fields.Addresses().MdcaPoint(0).Compare(aAddress) == 0);
	iSmsTest(fields.Subject().Compare(aSubject) == 0);
	
	// 2. PID...
	CSmsSettings* smsset = CSmsSettings::NewL();
	CleanupStack::PushL(smsset);
	replyHeader.GetSmsSettingsL(*smsset);
	iSmsTest(smsset->MessageConversion()==ESmsConvMail);
	CleanupStack::PopAndDestroy(smsset);
	
	// 3. Email <-> SMS gateway (i.e. the recipient)
	const CArrayPtrFlat<CSmsNumber>& rcpts = replyHeader.Recipients();
	iSmsTest(rcpts.Count() == 1);
	const CSmsNumber& rcpt = *rcpts.At(0);
	iSmsTest(rcpt.Address().Compare(KScriptItemRecipientsDef) == 0);

	// 4. Description - should be the same as the subject.
	TMsvEntry replyEntry = Client().Entry().Entry();
	TInt maxDesLength = Client().ServiceSettings().DescriptionLength();
	iSmsTest(replyEntry.iDescription.Compare(fields.Subject().Left(maxDesLength)) == 0); 
	
	// 5. Details - should be the same as the address.
	iSmsTest(replyEntry.iDetails.Compare(fields.Addresses().MdcaPoint(0).Left(KSmsDetailsLength)) == 0);	
	}
	
void CSmcmTest::TestReplyEmailL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Reply (Email SMS"));

	_LIT(KTestEmailAddress1,	"user@domain1.domain2");
	_LIT(KTestEmailSubject1,	"Re: a subject");
	_LIT(KTestEmailMessage1,	"user@domain1.domain2##a subject#some body text");
	DoTestReplyEmailL(KTestEmailMessage1, KTestEmailAddress1, KTestEmailSubject1);

	_LIT(KTestEmailAddress2,	"user@domain1.domain2");
	_LIT(KTestEmailSubject2,	"");
	_LIT(KTestEmailMessage2,	"user@domain1.domain2 some body text");
	DoTestReplyEmailL(KTestEmailMessage2, KTestEmailAddress2, KTestEmailSubject2);
	}

TBool CSmcmTest::TestForwardBodyL(const CRichText& aBody, const TDesC& aExpected) const
	{
	HBufC* body = HBufC::NewLC(aBody.DocumentLength());
	TPtr bodyPtr(body->Des());

	aBody.Extract(bodyPtr);

	const TBool check = (*body == aExpected);

	CleanupStack::PopAndDestroy(body);

	return check;
	}

TBool CSmcmTest::TestForwardEntry(const TMsvEntry& aOriginalEntry, const TMsvEntry& aForwardEntry) const
	{
	TBool ret =  (aOriginalEntry.SendingState()		== aForwardEntry.SendingState());
	ret = ret && (aOriginalEntry.iBioType			== aForwardEntry.iBioType);
	ret = ret && (aOriginalEntry.iDescription		== aForwardEntry.iDescription);
	return ret;
	}

HBufC* CSmcmTest::CreateRandomLC(TInt aLen) const
	{
	HBufC* originalMsg = HBufC::NewLC(aLen);
	TPtr originalPtr(originalMsg->Des());
	originalPtr.SetLength(aLen);
	TUint16* ptr = &originalPtr[0];
	while (aLen--)
		{
		*ptr++ = TUint16(Math::Random() % (128 - 65) + 65);
		}

	return originalMsg;
	}

void CSmcmTest::TestForwardL(const TMsvEntry& aEntry, TInt aBodyLength)
	{
	//Create new message
	_LIT(KTestForwardAddress, "+445554444333");
	_LIT(KTestForwardName, "RonaldMcDonald");

	TTime now;
	now.HomeTime();
	TMsvEntry originalEntry(aEntry);

	//Set the body
	HBufC* originalBody = CreateRandomLC(aBodyLength);
	iSmsTest.iRichText->Reset();
	iSmsTest.iRichText->InsertL(0, *originalBody);
		
	CSmsHeader* originalHeader = CSmsHeader::NewL(CSmsMessage::NewL(iSmsTest.FileSession(), CSmsPDU::ESmsDeliver, CSmsEditorBuffer::NewL(*iSmsTest.iRichText)));
	CleanupStack::PushL(originalHeader);

	// Add formatting and picture
	CEmsFormatIE* object = CEmsFormatIE::NewL();
	CleanupStack::PushL(object);
	object->SetStartPosition(0);
	object->SetFormatLength(5);
	object->SetBold(ETrue);
	originalHeader->Message().AddEMSInformationElementL(*object);
	CleanupStack::PopAndDestroy(object);

	CEmsPictureIE* pic = EmsTestUtils::CreatePictureL(0);
	CleanupStack::PushL(pic);
	pic->SetStartPosition(0);
	originalHeader->Message().AddEMSInformationElementL(*pic);
	CleanupStack::PopAndDestroy(pic);

	//Add a recipient
	CSmsNumber* originalNumber = CSmsNumber::NewL();
	CleanupStack::PushL(originalNumber);
	originalNumber->SetAddressL(KTestForwardAddress);
	originalNumber->SetNameL(KTestForwardName);
	originalHeader->Recipients().AppendL(originalNumber);
	CleanupStack::Pop(originalNumber);
	
	//Create the new message
	iSmsTest.CreateMessageL(KMsvGlobalOutBoxIndexEntryId, originalEntry, *originalHeader, *iSmsTest.iRichText, now);
	Session().CleanupEntryPushL(originalEntry.Id());

	MsvEntry().SetEntryL(originalEntry.Id());
	originalEntry = MsvEntry().Entry();
	// Need to secure the memory for the iDescription since the entry will change later on
	HBufC* descBuf= originalEntry.iDescription.AllocLC();
	originalEntry.iDescription.Set(descBuf->Ptr(), descBuf->Length());

	//Switch current entry and set the message body
	Client().SwitchCurrentEntryL(originalEntry.Id());
	Client().LoadMessageL();

	//Check body
	iSmsTest(TestForwardBodyL(Client().Body(), *originalBody));
	//Forward the message
	TMsvPartList part=0;
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	wait->Start();

	CMsvOperation* op = Client().ForwardL(KMsvDraftEntryId, part, wait->iStatus);
	CleanupStack::PushL(op);

	CActiveScheduler::Start();

	iSmsTest(wait->iStatus.Int() == KErrNone);

	//Get the new message ID
	TPckgBuf<TMsvId> pkg;
	pkg.Copy(op->ProgressL());
	const TMsvId progress = pkg();
	
	CleanupStack::PopAndDestroy(op);
	CleanupStack::PopAndDestroy(wait);

	op = NULL;

	Session().CleanupEntryPushL(progress);

	Client().SwitchCurrentEntryL(progress);
	Client().LoadMessageL();

	//Check the new message body
	iSmsTest(TestForwardBodyL(Client().Body(), *originalBody));

	//Check the entry
	iSmsTest(TestForwardEntry(originalEntry, Client().Entry().Entry()));

	// let's check to see if the inbox message and the forward are the same
	// (this call will compare ems objects as well
	if (!iSmsTest.AreMessagesSameTextL(originalEntry.Id(), progress, ETrue))
		{
		iSmsTest.Printf(_L("EMS objects do not compare!!\n"));
		}
	else
		{
		iSmsTest.Printf(_L("EMS objects compare OK.\n"));
		}

	CleanupStack::PopAndDestroy(); //message
	CleanupStack::PopAndDestroy(descBuf);
	CleanupStack::PopAndDestroy(); //message
	CleanupStack::PopAndDestroy(originalHeader);
	CleanupStack::PopAndDestroy(originalBody);
	}

void CSmcmTest::TestForwardL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Forward"));

	TInt len=3000;
	while (len > 0)
		{
		TMsvEntry entry = TMsvEntry();
		entry.iBioType = Math().Random();
		iSmsTest.Printf(_L("TestForward Len %d BioType %d\n"), len, entry.iBioType);
		TestForwardL(entry, len);
		len -= 25;
		}
	}
	
void CSmcmTest::DoTestForwardEmailL(const TDesC& aEmailMessageData, const TDesC& aSubject, const TDesC& aBody)
	{
	// Create an email message and then a forward message to it.
	TMsvId id = CreateEmailSmsMessageL(aEmailMessageData);
	iSmsTest.SetEntryL(id);
	Client().SwitchCurrentEntryL(id); 
	Client().LoadMessageL();

	TMsvPartList part=0;
	CMsvOperationWait* wait = CMsvOperationWait::NewLC();
	CMsvOperation* op=Client().ForwardL(KMsvDraftEntryId, part, wait->iStatus);
	CleanupStack::PushL(op);
	wait->Start();
	CActiveScheduler::Start();

	TPckgBuf<TMsvId> pkg;
	pkg.Copy(op->ProgressL());
	TMsvId forward = pkg();

	CleanupStack::PopAndDestroy(2, wait); // op, wait
	
	// Switch and load the forward message
	Client().SwitchCurrentEntryL(forward);
	Client().LoadMessageL();
	
	// Check the forward message...
	const CSmsHeader& forwardHeader = Client().SmsHeader();
	
	// 1. Email fields...(address should be empty)
	const CSmsEmailFields& fields = forwardHeader.EmailFields();
	iSmsTest(!fields.HasAddress());
	iSmsTest(fields.Addresses().MdcaCount()==0);
	iSmsTest(fields.Subject().Compare(aSubject) == 0);
	
	// 2. PID...
	CSmsSettings* smsset = CSmsSettings::NewL();
	CleanupStack::PushL(smsset);
	forwardHeader.GetSmsSettingsL(*smsset);
	iSmsTest(smsset->MessageConversion()==ESmsConvMail);
	CleanupStack::PopAndDestroy(smsset);
	
	// 3. Email <-> SMS gateway (ie the recipient)
	const CArrayPtrFlat<CSmsNumber>& rcpts = forwardHeader.Recipients();
	iSmsTest(rcpts.Count() == 1);
	const CSmsNumber& rcpt = *rcpts.At(0);
	iSmsTest(rcpt.Address().Compare(KScriptItemRecipientsDef) == 0);

	// 4. Description - should be the same as the subject or body text.
	TMsvEntry forwardEntry = Client().Entry().Entry();
	TInt maxDesLength = Client().ServiceSettings().DescriptionLength();
	if( fields.Subject().Length() )
		{
		// Description should be same as subject.
		iSmsTest(forwardEntry.iDescription.Compare(fields.Subject().Left(maxDesLength)) == 0); 		
		}
	else
		{
		// Description should be the same as the body text.
		HBufC* body = HBufC::NewLC(Client().Body().DocumentLength());
		TPtr bodyPtr(body->Des());
		Client().Body().Extract(bodyPtr);

		iSmsTest(forwardEntry.iDescription.Compare(bodyPtr.Left(maxDesLength)) == 0);
		CleanupStack::PopAndDestroy(body);
		}
	
	// 5. Details - should be the same as the address (ie empty).
	iSmsTest(forwardEntry.iDetails.Compare(KNullDesC())==0);
	
	// 6. Body data.
	iSmsTest(TestForwardBodyL(Client().Body(), aBody));
	}
	
void CSmcmTest::TestForwardEmailL()
	{
	iSmsTest.Test().Next(_L("CSmsClientMtm Forward (Email SMS)"));

	_LIT(KTestEmailSubject1,	"Fw: a subject");
	_LIT(KTestEmailBody1,		"some body text");
	_LIT(KTestEmailMessage1,	"user@domain1.domain2##a subject#some body text");
	DoTestForwardEmailL(KTestEmailMessage1, KTestEmailSubject1, KTestEmailBody1);

	_LIT(KTestEmailSubject2,	"");
	_LIT(KTestEmailBody2,		"some body text");
	_LIT(KTestEmailMessage2,	"user@domain1.domain2 some body text");
	DoTestForwardEmailL(KTestEmailMessage2, KTestEmailSubject2, KTestEmailBody2);
	}

void CSmcmTest::TestSyncFunctionsL()
	{
	iSmsTest.Test().Next(_L("Test CSmsClientMtm Sync Functions"));
	iSmsTest.Printf(_L("This test is INCOMPLETE\n"));
	}


// Now testing replying to received messages
void CSmcmTest::TestReplyWithOverloadedSaveMessageL()
    {
    iSmsTest.Test().Next(_L("CSmsClientMtm SavMessage"));
    iSmsTest.Printf(_L("This test is INCOMPLETE\n"));

    TMsvEntry entry;
    entry.iType=KUidMsvMessageEntry;
    entry.SetSendingState(KMsvSendStateNotApplicable);
    
    // Begin extracted code
    iSmsTest.iRichText->Reset();
    iSmsTest.iRichText->InsertL(0,_L("This is a test message, SaveMessageL"));
    CSmsHeader* header = CSmsHeader::NewL(CSmsPDU::ESmsDeliver, *iSmsTest.iRichText);
    CleanupStack::PushL(header);

    // Add formatting and picture
    CEmsFormatIE* object = CEmsFormatIE::NewL();
    CleanupStack::PushL(object);
    object->SetStartPosition(10);
    object->SetFormatLength(10);
    object->SetBold(ETrue);
    header->Message().AddEMSInformationElementL(*object);
    CleanupStack::PopAndDestroy(object);

    CEmsPictureIE* pic = EmsTestUtils::CreatePictureL(0);
    CleanupStack::PushL(pic);
    pic->SetStartPosition(20);
    header->Message().AddEMSInformationElementL(*pic);
    CleanupStack::PopAndDestroy(pic);

    header->SetFromAddressL(KScriptItemRecipientsDef);
    header->Message().SetServiceCenterAddressL(KScriptItemSCDef);
  
    TTime now;
    now.HomeTime();
    iSmsTest.CreateMessageL(KMsvGlobalInBoxIndexEntryId, entry, *header, *iSmsTest.iRichText, now);
    entry.SetReadOnly(ETrue);
    iSmsTest.ChangeEntryL(entry);
    TMsvId id = entry.Id();
    CleanupStack::PopAndDestroy(1); //header
    // End extracted code

    iSmsTest.SetEntryL(id);

    Client().SwitchCurrentEntryL(id); // Lets point at the received message
    Client().LoadMessageL();

    TMsvPartList part=0;
    CMsvOperationWait* wait = CMsvOperationWait::NewLC();
    CMsvOperation* op=Client().ReplyL(KMsvDraftEntryId, part, wait->iStatus);
    CleanupStack::PushL(op);
    wait->Start();
    CActiveScheduler::Start();

    TPckgBuf<TMsvId> pkg;
    pkg.Copy(op->ProgressL());
    TMsvId progress = pkg();

    CleanupStack::PopAndDestroy(2); //op, wait

    // let's check to see if the inbox message and the reply are the same
    // (this call will compare ems objects as well
    if (!iSmsTest.AreMessagesSameTextL(id, progress, ETrue))
        {
        iSmsTest.Printf(_L("EMS objects do not compare!!\n"));
        }
    else
        {
        iSmsTest.Printf(_L("EMS objects compare OK.\n"));
        }
    
    Client().SwitchCurrentEntryL(progress);
    Client().LoadMessageL();
    
    CMsvStore* msvstore = MsvEntry().EditStoreL();
    CleanupStack::PushL(msvstore);
        
    Client().Body().InsertL(0,_L("This is reply, SaveMessageL"));
    Client().SaveMessageL(*msvstore, entry);
    msvstore->CommitL();
    CleanupStack::PopAndDestroy(msvstore);
    }
