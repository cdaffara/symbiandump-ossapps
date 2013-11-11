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
//

#include "ctestimapfetchsinglebodystructure.h"

#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"

#include "moutputstream.h"
#include "cimapsession.h"
#include "cimapfolderinfo.h"

#include "cimapfetchresponse.h"
#include "cimaprfc822headerfields.h"
#include "cimapbodystructure.h"

CTestImapFetchSingleBodyStructure::CTestImapFetchSingleBodyStructure()
	{}

CTestImapFetchSingleBodyStructure::~CTestImapFetchSingleBodyStructure()
	{}

TBool CTestImapFetchSingleBodyStructure::DescriptorsMatch(const TDesC8& aFirst, const TDesC8& aSecond)
	{
	return (aFirst == aSecond);
	}

// Tests
void CTestImapFetchSingleBodyStructure::TestTextBodyFetchL()
	{
	INFO_PRINTF1(_L("TestTextBodyFetchL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 284 FLAGS (\\Seen) BODYSTRUCTURE (\"TEXT\" \"PLAIN\" (\"charset\" \"iso-8859-1\") NIL NIL \"7BIT\" 194 4 NIL NIL NIL) BODY[HEADER.FIELDS (Received Date Subject From)] {111}\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Thu, 27 May 2004 14:57:46 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: test_mail_1\r\n"));
	iInputStream->AppendInputStringL(_L8("From: <matthewf@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("\r\n"));
	iInputStream->AppendInputStringL(_L8(")\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
	/*
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 284 FLAGS (\\Seen) BODY[HEADER.FIELDS (Received Date Subject From)] {111}\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Thu, 27 May 2004 14:57:46 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: test_mail_1\r\n"));
	iInputStream->AppendInputStringL(_L8("From: <matthewf@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("\r\n"));
	iInputStream->AppendInputStringL(_L8(" BODYSTRUCTURE (\"TEXT\" \"PLAIN\" (\"charset\" \"iso-8859-1\") NIL NIL \"7BIT\" 194 4 NIL NIL NIL))\r\n"));
	iInputStream->AppendInputStringL(_L8("4 OK FETCH completed.\r\n"));
	*/		
	
	TUint messageUid = 284;
	CImapFetchResponse* fetchResponse = CImapFetchResponse::NewL();
	CleanupStack::PushL(fetchResponse);
	_LIT8(KFieldNames, "Received Date Subject From");
	
	iImapSession->FetchBodyStructureAndHeadersL(iActiveWaiter->iStatus, messageUid, KFieldNames, *fetchResponse);
	iActiveWaiter->WaitActive();
	
	CImapRfc822HeaderFields* headerFields = fetchResponse->HeaderFields();
	ASSERT_NOT_NULL(headerFields);
				
	TPtrC8 ptrReceived = headerFields->FieldValue(CImapRfc822HeaderFields::EImapReceived);
	TPtrC8 ptrDate = headerFields->FieldValue(CImapRfc822HeaderFields::EImapDate);
	TPtrC8 ptrSubject = headerFields->FieldValue(CImapRfc822HeaderFields::EImapSubject);
	TPtrC8 ptrFrom = headerFields->FieldValue(CImapRfc822HeaderFields::EImapFrom);
				
	ASSERT_EQUALS(ptrReceived, _L8(""));
	ASSERT_EQUALS(ptrDate, _L8("Thu, 27 May 2004 14:57:46 +0100"));
	ASSERT_EQUALS(ptrSubject, _L8("test_mail_1"));
	ASSERT_EQUALS(ptrFrom, _L8("<matthewf@msexchange2k.closedtest.intra>"));
	
	CImapBodyStructure* bodyStructure = fetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructure);
	
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->Type(), _L8("TEXT")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->SubType(), _L8("PLAIN")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->BodyEncoding(), _L8("7BIT")));
	
	ASSERT_EQUALS(fetchResponse->MessageUid(), messageUid);
	ASSERT_TRUE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::ERecent));
	
	CImapFolderInfo* folderInfo = iImapSession->SelectedFolderInfo();
	ASSERT_NOT_NULL(folderInfo);
	ASSERT_FALSE(folderInfo->MessageFlagsChanged());
						
	CleanupStack::PopAndDestroy(fetchResponse);
	
	INFO_PRINTF1(_L("Complete"));
	}
	
	//new
void CTestImapFetchSingleBodyStructure::TestBasicBodyFetchL()
	{
	INFO_PRINTF1(_L("TestBasicBodyFetchL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 284 FLAGS (\\Seen) BODYSTRUCTURE (\"APPLICATION\" \"X-ZIP-COMPRESSED\" (\"name\" \" \") NIL NIL \"BASE64\" 13684 NIL (\"attachment\" (\"filename\" \" \")) NIL) BODY[HEADER.FIELDS (Received Date Subject From)] {111}\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Thu, 27 May 2004 14:57:46 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: test_mail_1\r\n"));
	iInputStream->AppendInputStringL(_L8("From: <matthewf@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("\r\n"));
	iInputStream->AppendInputStringL(_L8(")\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
		
	TUint messageUid = 284;
	CImapFetchResponse* fetchResponse = CImapFetchResponse::NewL();
	CleanupStack::PushL(fetchResponse);
	_LIT8(KFieldNames, "Received Date Subject From");
	
	iImapSession->FetchBodyStructureAndHeadersL(iActiveWaiter->iStatus, messageUid, KFieldNames, *fetchResponse);
	iActiveWaiter->WaitActive();
	
	CImapRfc822HeaderFields* headerFields = fetchResponse->HeaderFields();
	ASSERT_NOT_NULL(headerFields);
				
	TPtrC8 ptrReceived = headerFields->FieldValue(CImapRfc822HeaderFields::EImapReceived);
	TPtrC8 ptrDate = headerFields->FieldValue(CImapRfc822HeaderFields::EImapDate);
	TPtrC8 ptrSubject = headerFields->FieldValue(CImapRfc822HeaderFields::EImapSubject);
	TPtrC8 ptrFrom = headerFields->FieldValue(CImapRfc822HeaderFields::EImapFrom);
				
	ASSERT_EQUALS(ptrReceived, _L8(""));
	ASSERT_EQUALS(ptrDate, _L8("Thu, 27 May 2004 14:57:46 +0100"));
	ASSERT_EQUALS(ptrSubject, _L8("test_mail_1"));
	ASSERT_EQUALS(ptrFrom, _L8("<matthewf@msexchange2k.closedtest.intra>"));
	
	CImapBodyStructure* bodyStructure = fetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructure);
	
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->Type(), _L8("APPLICATION")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->SubType(), _L8("X-ZIP-COMPRESSED")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->BodyEncoding(), _L8("BASE64")));
	
	ASSERT_EQUALS(fetchResponse->MessageUid(), messageUid);
	ASSERT_TRUE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::ERecent));
	
	CleanupStack::PopAndDestroy(fetchResponse);
	
	INFO_PRINTF1(_L("Complete"));
	}
	//new

void CTestImapFetchSingleBodyStructure::TestMultipartL()
	{
	INFO_PRINTF1(_L("TestMultipartL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 32475 FLAGS () BODYSTRUCTURE ((\"TEXT\" \"PLAIN\" (\"CHARSET\" \"US-ASCII\") NIL NIL \"7BIT\" 9 1 NIL NIL NIL)(\"IMAGE\" \"JPEG\" (\"x-mac-type\" \"4A504547\" \"x-mac-creator\" \"4A565752\" \"name\" \"screen.jpg\") NIL NIL \"X-UUENCODE\" 201017 NIL (\"attachment\" (\"filename\" \"screen.jpg\")) NIL) \"mixed\" (\"boundary\" \"=====================_11834497==_\") NIL NIL) BODY[HEADER.FIELDS (Received Date Subject From Reply-to To Cc Bcc Message-ID Return-Receipt-To X-Return-Receipt-To Disposition-Notification-To Disposition-Notification-Options Priority X-Priority X-MSMail-Priority Importance Precedence)] {922}\r\n"));
	iInputStream->AppendInputStringL(_L8("Received: from [127.0.0.1] ([10.23.179.168]) by lon-cn-exchng2k.msexchange2k.closedtest.intra with Microsoft SMTPSVC(5.0.2195.5329);\r\n"));
	iInputStream->AppendInputStringL(_L8("\t Mon, 11 Jul 2005 15:41:16 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("Received: from mx3.messagingengine.com (mx3.internal [10.202.2.202])\r\n"));
	iInputStream->AppendInputStringL(_L8("\t by server2.messagingengine.com (Cyrus v2.3-alpha) with LMTPA;\r\n"));
	iInputStream->AppendInputStringL(_L8("\t Tue, 05 Apr 2005 07:47:35 -0400\r\n"));
	iInputStream->AppendInputStringL(_L8("Received: from smtp003.mail.ukl.yahoo.com (smtp003.mail.ukl.yahoo.com [217.12.11.34])\r\n"));
	iInputStream->AppendInputStringL(_L8("\t\r\n"));
	iInputStream->AppendInputStringL(_L8("by mx3.messagingengine.com (Postfix) with SMTP id A956E5A0CB3\r\n"));
 	iInputStream->AppendInputStringL(_L8("\tfor <l_luchford@fastmail.co.uk>; Tue,  5 Apr 2005 07:47:34 -0400 (EDT)\r\n"));
	iInputStream->AppendInputStringL(_L8("Received: from unknown (HELO lon-leel03.yahoo.co.uk) (l?luchford@80.7.227.13 with login)\r\n"));
	iInputStream->AppendInputStringL(_L8("  by smtp003.mail.ukl.yahoo.com with SMTP; 5 Apr 2005 11:47:22 -0000\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Tue, 05 Apr 2005 12:47:19 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject:\r\n"));
	iInputStream->AppendInputStringL(_L8("From: Lee Luchford <l_luchford@yahoo.co.uk>\r\n"));
	iInputStream->AppendInputStringL(_L8("To: l_luchford@fastmail.co.uk\r\n"));
	iInputStream->AppendInputStringL(_L8("Message-Id: <6.2.1.2.0.20050405124701.0274a008@pop.mail.yahoo.co.uk>\r\n"));
	iInputStream->AppendInputStringL(_L8("\r\n"));
	iInputStream->AppendInputStringL(_L8(")\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
	
	TUint messageUid = 32475;
	CImapFetchResponse* fetchResponse = CImapFetchResponse::NewL();
	CleanupStack::PushL(fetchResponse);
	_LIT8(KFieldNames, "Received Date Subject From");
	
	iImapSession->FetchBodyStructureAndHeadersL(iActiveWaiter->iStatus, messageUid, KFieldNames, *fetchResponse);
	iActiveWaiter->WaitActive();
	
	CImapRfc822HeaderFields* headerFields = fetchResponse->HeaderFields();
	ASSERT_NOT_NULL(headerFields);
	
	TPtrC8 ptrReceived = headerFields->FieldValue(CImapRfc822HeaderFields::EImapReceived);
	TPtrC8 ptrDate = headerFields->FieldValue(CImapRfc822HeaderFields::EImapDate);
	TPtrC8 ptrSubject = headerFields->FieldValue(CImapRfc822HeaderFields::EImapSubject);
	TPtrC8 ptrFrom = headerFields->FieldValue(CImapRfc822HeaderFields::EImapFrom);
	TPtrC8 ptrMsgId = headerFields->FieldValue(CImapRfc822HeaderFields::EImapMessageId);
	
	ASSERT_EQUALS(ptrReceived, _L8("from unknown (HELO lon-leel03.yahoo.co.uk) (l?luchford@80.7.227.13 with login)  by smtp003.mail.ukl.yahoo.com with SMTP; 5 Apr 2005 11:47:22 -0000"));
	ASSERT_EQUALS(ptrDate, _L8("Tue, 05 Apr 2005 12:47:19 +0100"));
	ASSERT_EQUALS(ptrSubject, _L8(""));
	ASSERT_EQUALS(ptrFrom, _L8("Lee Luchford <l_luchford@yahoo.co.uk>"));
	ASSERT_EQUALS(ptrMsgId, _L8("<6.2.1.2.0.20050405124701.0274a008@pop.mail.yahoo.co.uk>"));
	
	CImapBodyStructure* bodyStructure = fetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructure);
	
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->Type(), _L8("MULTIPART")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->SubType(), _L8("mixed")));
	ASSERT_EQUALS(bodyStructure->BodyStructureType(), CImapBodyStructure::ETypeMultipart);
	
	ASSERT_EQUALS(bodyStructure->EmbeddedBodyStructureList().Count(), 2);
	
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->EmbeddedBodyStructureList()[0]->Type(), _L8("TEXT")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->EmbeddedBodyStructureList()[0]->SubType(), _L8("PLAIN")));
	ASSERT_EQUALS(bodyStructure->EmbeddedBodyStructureList()[0]->BodyStructureType(), CImapBodyStructure::ETypeText);
	
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->EmbeddedBodyStructureList()[1]->Type(), _L8("IMAGE")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->EmbeddedBodyStructureList()[1]->SubType(), _L8("JPEG")));
	ASSERT_EQUALS(bodyStructure->EmbeddedBodyStructureList()[1]->BodyStructureType(), CImapBodyStructure::ETypeBasic);
		
	ASSERT_EQUALS(fetchResponse->MessageUid(), messageUid);
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::ESeen));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EAnswered));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EFlagged));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EDeleted));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::EDraft));
	ASSERT_FALSE(fetchResponse->MessageFlagInfo().QueryFlag(TMessageFlagInfo::ERecent));
	
	CleanupStack::PopAndDestroy(fetchResponse);
	
	INFO_PRINTF1(_L("Complete"));
	}
	//new

void CTestImapFetchSingleBodyStructure::TestRFC822BodyFetchL()
	{
	INFO_PRINTF1(_L("TestRFC822BodyFetchL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 871 FLAGS (\\Seen) BODYSTRUCTURE (\"MESSAGE\" \"RFC822\" NIL NIL \"HTML Message with attachment - no images\" \"7bit\" 596 (\"Wed, 13 Oct 1999 12:11:11 +0100\" \"HTML Message with attachment - no images\" ((\"matthewf\" NIL \"matthewf\" \"msexchange2k.closedtest.intra\")) ((\"matthewf\" NIL \"matthewf\" \"msexchange2k.closedtest.intra\")) ((\"matthewf\" NIL \"matthewf\" \"msexchange2k.closedtest.intra\")) ((\"matthewf\" NIL \"matthewf\" \"msexchange2k.closedtest.intra\")) NIL NIL NIL \"<01fc01bf156b$a82fc490$5f0c970a@lon-kevalp.plc.psion.com>\") (\"TEXT\" \"PLAIN\" NIL NIL NIL \"8bit\" 0 0 NIL NIL NIL) 10 NIL (\"inline\" NIL) NIL) NIL NIL) BODY[HEADER.FIELDS (Received Date Subject From Reply-to To Cc Bcc Message-ID Return-Receipt-To X-Return-Receipt-To Disposition-Notification-To Disposition-Notification-Options Priority X-Priority X-MSMail-Priority Importance Precedence)] {111}\r\n"));
	
	iInputStream->AppendInputStringL(_L8("Date: Wed, 13 Oct 1999 12:11:11 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: test_mail_1\r\n"));
	iInputStream->AppendInputStringL(_L8("From: <matthewf@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("\r\n"));
	iInputStream->AppendInputStringL(_L8(")\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
	
	TUint messageUid = 596;
	CImapFetchResponse* fetchResponse = CImapFetchResponse::NewL();
	CleanupStack::PushL(fetchResponse);
	_LIT8(KFieldNames, "Received Date Subject From Reply-to To Cc Bcc Message-ID Return-Receipt-To X-Return-Receipt-To Disposition-Notification-To Disposition-Notification-Options Priority X-Priority X-MSMail-Priority Importance Precedence");
	
	iImapSession->FetchBodyStructureAndHeadersL(iActiveWaiter->iStatus, messageUid, KFieldNames, *fetchResponse);
	iActiveWaiter->WaitActive();
	
	CImapBodyStructure* bodyStructure = fetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructure);
	

	ASSERT_TRUE(DescriptorsMatch(bodyStructure->Type(), _L8("MESSAGE")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->SubType(), _L8("RFC822")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->BodyDescription(), _L8("HTML Message with attachment - no images")));

	ASSERT_TRUE(DescriptorsMatch(bodyStructure->BodyId(), _L8("")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->BodyEncoding(), _L8("7bit")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->BodySizeOctets(), _L8("596")));
	ASSERT_EQUALS(bodyStructure->ParameterList().Count(), 0);
 
	// Check its envelope
	CImapEnvelope& envelope = bodyStructure->GetRfc822EnvelopeStructureL();
	ASSERT_TRUE(DescriptorsMatch(envelope.EnvDate(), _L8("Wed, 13 Oct 1999 12:11:11 +0100")));
	ASSERT_TRUE(DescriptorsMatch(envelope.EnvSubject(), _L8("HTML Message with attachment - no images")));
	ASSERT_EQUALS(envelope.EnvFrom().Count(), 1);
	CheckAddressL(envelope.EnvFrom()[0], _L16("\"matthewf\" <matthewf@msexchange2k.closedtest.intra>"));

	ASSERT_EQUALS(envelope.EnvSender().Count(), 1);
	CheckAddressL(envelope.EnvSender()[0], _L16("\"matthewf\" <matthewf@msexchange2k.closedtest.intra>"));
	ASSERT_EQUALS(envelope.EnvReplyTo().Count(), 1);
	CheckAddressL(envelope.EnvReplyTo()[0], _L16("\"matthewf\" <matthewf@msexchange2k.closedtest.intra>"));
	ASSERT_EQUALS(envelope.EnvTo().Count(), 1);
	CheckAddressL(envelope.EnvTo()[0], _L16("\"matthewf\" <matthewf@msexchange2k.closedtest.intra>"));
	ASSERT_EQUALS(envelope.EnvCc().Count(), 0);
	ASSERT_EQUALS(envelope.EnvBcc().Count(), 0);	
	ASSERT_TRUE(DescriptorsMatch(envelope.EnvInReplyTo(), _L8("")));
	ASSERT_TRUE(DescriptorsMatch(envelope.EnvMessageId(), _L8("<01fc01bf156b$a82fc490$5f0c970a@lon-kevalp.plc.psion.com>")));
	
	// Check the embedded bodystructure
	ASSERT_EQUALS(bodyStructure->EmbeddedBodyStructureList().Count(), 1);
	CImapBodyStructure* bodyStructureEmbedded = bodyStructure->EmbeddedBodyStructureList()[0];
	
	ASSERT_TRUE(DescriptorsMatch(bodyStructureEmbedded->Type(), _L8("TEXT")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructureEmbedded->SubType(), _L8("PLAIN")));
	ASSERT_EQUALS(bodyStructureEmbedded->ParameterList().Count(), 0);
 	ASSERT_TRUE(DescriptorsMatch(bodyStructureEmbedded->BodyId(), _L8("")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructureEmbedded->BodyDescription(), _L8("")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructureEmbedded->BodyEncoding(), _L8("8bit")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructureEmbedded->BodySizeOctets(), _L8("0")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructureEmbedded->BodyLines(), _L8("0")));
	CleanupStack::PopAndDestroy(fetchResponse);
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapFetchSingleBodyStructure::TestPipexFetchL()
	{
	INFO_PRINTF1(_L("TestPipexFetchL"));

	iInputStream->ResetInputStrings();	
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 1361 FLAGS (\\Draft \\Seen) BODYSTRUCTURE (\"text\" \"plain\" (\"charset\" \"iso-8859-1\") NIL NIL \"7bit\" 1040 15 NIL NIL NIL) BODY[HEADER.FIELDS (\"Received\" \"Date\" \"Subject\" \"From\" \"Priority\" \"X-MSMail-Priority\")] {152}\r\n"));
	iInputStream->AppendInputStringL(_L8("From: \"IMAP\" <davids@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: message for test 4.2.2\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Fri, 12 May 2006 09:34:30 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MSMail-Priority: Normal\r\n"));
	iInputStream->AppendInputStringL(_L8(")\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
		
	TUint messageUid = 1361;
	CImapFetchResponse* fetchResponse = CImapFetchResponse::NewL();
	CleanupStack::PushL(fetchResponse);
	_LIT8(KFieldNames, "Received Date Subject From Priority X-MSMail-Priority");
	
	iImapSession->FetchBodyStructureAndHeadersL(iActiveWaiter->iStatus, messageUid, KFieldNames, *fetchResponse);
	iActiveWaiter->WaitActive();
	
	CImapBodyStructure* bodyStructure = fetchResponse->BodyStructure();
	ASSERT_NOT_NULL(bodyStructure);
	
	ASSERT_EQUALS(bodyStructure->BodyStructureType(), CImapBodyStructure::ETypeText);
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->Type(), _L8("text")));
	ASSERT_TRUE(DescriptorsMatch(bodyStructure->SubType(), _L8("plain")));
	
	CleanupStack::PopAndDestroy(fetchResponse);
	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapFetchSingleBodyStructure::CheckAddressL(const CImapEnvelope::TAddress& aAddress, const TDesC16& aExpectedAddressString)
	{
	HBufC16* addressString = aAddress.CreateAddressStringL();
	CleanupStack::PushL(addressString);
	
	ASSERT_EQUALS((TDesC16&)*addressString, aExpectedAddressString);
	
	CleanupStack::PopAndDestroy(addressString);
	}
	
CTestSuite* CTestImapFetchSingleBodyStructure::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestTextBodyFetchL);
	ADD_ASYNC_TEST_STEP(TestBasicBodyFetchL);
	ADD_ASYNC_TEST_STEP(TestRFC822BodyFetchL);
	ADD_ASYNC_TEST_STEP(TestMultipartL);
	ADD_ASYNC_TEST_STEP(TestPipexFetchL);
	END_SUITE;
	}
