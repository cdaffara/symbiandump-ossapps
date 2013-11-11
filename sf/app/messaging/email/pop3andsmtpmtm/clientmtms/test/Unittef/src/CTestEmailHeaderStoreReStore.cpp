
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
// file  CTestEmailHeaderStoreReStore.cpp
// This contains Ut for email header migration.
// 


#include "emailtestutils.h"
#include <pop3set.h>
#include <miutlog.h>

#include <msventry.h>
#include <msvids.h>
#include <miutconv.h>
#include <charconv.h>
#include <imcvcodc.h>


#include "miuthdr.h"    
#include <miutlog.h>

#include <imapset.h>
#include <smtpset.h>
#include <miutset.h>

#include <cemailaccounts.h>
#include <iapprefs.h>

#include "CTestEmailHeaderStoreReStore.h" 
#include <sqldb.h>


// local variables etc //


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

LOCAL_D CEmailTestUtils* testUtils;

 
LOCAL_D CEmailAccounts* account;
LOCAL_D TPopAccount popAccount;
LOCAL_D TImapAccount imapAccount;


_LIT(KMiutSubject, "short mail about something interesting");
_LIT8(KMiutMsgId, "0123456789AB.CdeFGHIj@symbian.com");
_LIT(KMiutFrom, "ralph-greenwell@psion.com");
_LIT(KMiutReplyTo, "ralph.greenwell@virgin.net");
_LIT(KMiutToRecipients1, "recipient1@address.com");
_LIT(KMiutToRecipients2, "recipient2@address.com");
_LIT(KMiutCcRecipients1, "cc1@address.com");
_LIT(KMiutCcRecipients2, "cc2@address.com");
_LIT(KMiutBccRecipients, "bcc@address.com");
_LIT8(KMiutResentMsgId, "0123456789AB.CdeFGHIj@symbian.com");
_LIT(KMiutResentFrom, "test@psion6.demon.co.uk");
_LIT(KMiutResentToRecipients1, "recipient1@address.com");
_LIT(KMiutResentToRecipients2, "recipient2@address.com");
_LIT(KMiutResentCcRecipients1, "cc1@address.com");
_LIT(KMiutResentCcRecipients2, "cc2@address.com");
_LIT(KMiutResentBccRecipients, "bcc@address.com");

_LIT(KPOPAccountName, "PopAccount");
_LIT(KIMAPAccountName, "ImapAccount");


#define KMiutRemoteSize 					123456

#define KMaxImHeaderStringLengthLong		KMaxImHeaderStringLength+1

LOCAL_D TBuf<KMaxImHeaderStringLengthLong> longValue;
LOCAL_D TBuf8<KMaxImHeaderStringLengthLong> longValue2;
LOCAL_D TBuf<KMaxImHeaderStringLengthLong> longValue3;

// MIME//


#define KImRelativePathString _L8("..\\..\\")
#define KImailHeader1 _L8("TEXT")
#define KImailHeader2 _L8("PLAIN")
#define KImailHeader3 _L8("CHARSET")
#define KImailHeader4 _L8("US-ASCII")

#define KImailHeader5 _L8("<kjashd@jsdkjd.com")
#define KImailHeader6 _L("http:\\\\abc\\def\\hij\\klmn.html")
#define KImailHeader7 _L8("93245ihdflkjsd098325lksdf@09835.psion.com")
#define KImailHeader8 _L8("US-ASCII")
#define KImailHeader9 _L8("FILE")
#define KImailHeader10 _L8("ATTACHMENT")
#define KImailHeader11 _L8("CALENDAR")

#define KImailParam1 _L8("charset")
#define KImailParam11 _L8(" ")
#define KImailParam2 _L8("ISO-8859-1")
#define KImailParam3 _L8("filename")
#define KImailParam31 _L8("")
#define KImailParam4 _L8("ohio state.jpeg")
#define KImailParam5 _L8("START")
#define KImailParam6 _L8("0x1003")
#define KImailParam7 _L8("method")
#define KImailParam8 _L8("request")
#define KImailParam9 _L8("component")
#define KImailParam10 _L8("vevent")

#define KImailParam100 _L8("") 
#define KImailParam1000 _L8("test") 
#define KImailParam2000 _L8("") 

//#define KImailParam11 _L8("event.ics")
#define KImailHeader75 _L8("")
#define KMaxImMimeFieldLengthLong KMaxImMimeFieldLength+1


void CObserver::HandleSessionEvent(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	}
	
	
void CObserver::HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
	{
	}



 /*
 //TEF//

*/


RTest test(_L("TestStoreReStore1"));

CTestEmailHeaderStoreReStore::CTestEmailHeaderStoreReStore()
	{
	}

CTestEmailHeaderStoreReStore::~CTestEmailHeaderStoreReStore()
	{
	}


void CTestEmailHeaderStoreReStore::SetupL()
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

void CTestEmailHeaderStoreReStore::TearDownL()
	{
	_LIT(KFunction, "TearDownL");
	INFO_PRINTF1(KFunction);
	
	delete iEntry;
	delete iSession;
	delete testUtils;
	delete imHeader;
	delete imReciveHeader;
	delete iencodedHeader;
	delete imEncodedReciveHeader;
	delete imMimeHeader;
	delete imMimeReciveHeader;

	}


void CTestEmailHeaderStoreReStore::InitialiseTestL()
	{
	testUtils = CEmailTestUtils::NewL(test);
	
	iob = new (ELeave) CObserver;

    iSession = CMsvSession::OpenSyncL(*iob);
 	
	iEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryIdValue, TMsvSelectionOrdering());

	}

/*
test for CImheader.
Create Account For POP3
Create a Message header for same MTM.
*/

void CTestEmailHeaderStoreReStore::TestStoreRestoreHeaderL()
	{
	CreateAccountL();	
	CreateMessagesL(KUidMsgTypePOP3);
	}

/*
Create Account for POP3 MTM.
*/
void CTestEmailHeaderStoreReStore::CreateAccountL()	
	{
	
	INFO_PRINTF1(_L("Create Email Message account .."));
	
	// Create email accounts
	account = CEmailAccounts::NewLC();
		

	CImPop3Settings* popSetting = new (ELeave) CImPop3Settings;

	CImSmtpSettings* smtpSetting = new (ELeave) CImSmtpSettings;

	CImIAPPreferences* iap = CImIAPPreferences::NewLC();
	CImIAPPreferences* smtpIAP = CImIAPPreferences::NewLC();

	account->PopulateDefaultPopSettingsL(*popSetting, *iap);
	account->PopulateDefaultSmtpSettingsL(*smtpSetting, *smtpIAP);

	popAccount = account->CreatePopAccountL(KPOPAccountName, *popSetting, *iap, EFalse);
	
	ASSERT_TRUE(popAccount.iPopAccountId != KErrNone);
	
	TSmtpAccount smtpAccount;
	smtpAccount = account->CreateSmtpAccountL(popAccount, *smtpSetting, *smtpIAP, EFalse);
	ASSERT_TRUE(smtpAccount.iSmtpAccountId != KErrNone);

	CImImap4Settings* imapSetting = new (ELeave) CImImap4Settings;

	account->PopulateDefaultImapSettingsL(*imapSetting, *iap);
	account->PopulateDefaultSmtpSettingsL(*smtpSetting, *smtpIAP);

	imapAccount = account->CreateImapAccountL(KIMAPAccountName, *imapSetting, *iap, EFalse);
	ASSERT_TRUE(imapAccount.iImapAccountId != KErrNone);
	
	smtpAccount = account->CreateSmtpAccountL(imapAccount, *smtpSetting, *smtpIAP, EFalse);
	ASSERT_TRUE(smtpAccount.iSmtpAccountId != KErrNone);
	
	CleanupStack::PopAndDestroy(3);
	}

/*
Create messag3e for  POP3 MTM.
*/	
void CTestEmailHeaderStoreReStore::CreateMessagesL(TUid aMtm)
	{
	
	INFO_PRINTF1(_L("Store  Email Message SMS header .."));
	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = aMtm;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());
	//entry1Id=entry1.Id();

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);

	//Create Encodeing Info.

	TImHeaderEncodingInfo info;
	info.SetField(TImHeaderEncodingInfo::EFrom);
	info.SetOffset(1);
	info.SetLength(10);
	info.SetCharsetUid(5);
	info.SetEncodingType(TImHeaderEncodingInfo::EBase64);

	info.SetArrayValue(25);

	info.SetAddSpace(ETrue);

	info.SetEncodedLength(14);
	
	TImHeaderEncodingInfo info1;
	info1.SetField(TImHeaderEncodingInfo::EFrom);
	info1.SetOffset(1);
	info1.SetLength(10);
	info1.SetCharsetUid(5);
	info1.SetEncodingType(TImHeaderEncodingInfo::EBase64);

	info1.SetArrayValue(25);

	info1.SetAddSpace(ETrue);

	info1.SetEncodedLength(14);
			
	
	TImHeaderEncodingInfo info2;
	info2.SetField(TImHeaderEncodingInfo::EFrom);
	info2.SetOffset(1);
	info2.SetLength(10);
	info2.SetCharsetUid(5);
	info2.SetEncodingType(TImHeaderEncodingInfo::EBase64);

	info2.SetArrayValue(25);

	info2.SetAddSpace(ETrue);

	info2.SetEncodedLength(14);

			

	imHeader = CImHeader::NewLC(); 
	CleanupStack::Pop();
		
	imHeader->Reset();
	imHeader->AddEncodingInfoL(info);
	imHeader->AddEncodingInfoL(info1);
	imHeader->AddEncodingInfoL(info2);	
	
	imHeader->AddEncodingInfoL(info);
	imHeader->AddEncodingInfoL(info1);
	imHeader->AddEncodingInfoL(info2);	
	imHeader->AddEncodingInfoL(info);
	imHeader->AddEncodingInfoL(info1);
	imHeader->AddEncodingInfoL(info2);	
	
	imHeader->AddEncodingInfoL(info);
	imHeader->AddEncodingInfoL(info1);
	imHeader->AddEncodingInfoL(info2);	

	imHeader->AddEncodingInfoL(info);
	imHeader->AddEncodingInfoL(info1);
	imHeader->AddEncodingInfoL(info2);	
	
	imHeader->AddEncodingInfoL(info);
	imHeader->AddEncodingInfoL(info1);
	imHeader->AddEncodingInfoL(info2);	

	//Add Subject and others
	imHeader->SetSubjectL(KMiutSubject);
	
	imHeader->SetImMsgIdL(KMiutMsgId);

	imHeader->SetFromL(KMiutFrom);
	imHeader->SetReplyToL(KMiutReplyTo);
	imHeader->SetRemoteSize(KMiutRemoteSize);

	imHeader->ToRecipients().AppendL(KMiutToRecipients1);
	imHeader->ToRecipients().AppendL(KMiutToRecipients2);
	imHeader->CcRecipients().AppendL(KMiutCcRecipients1);
	imHeader->CcRecipients().AppendL(KMiutCcRecipients2);
	imHeader->BccRecipients().AppendL(KMiutBccRecipients);
	imHeader->SetBodyEncoding(EMsgOutboxMIME);
	
	imHeader->SetResentMsgIdL(KMiutResentMsgId);
	imHeader->SetResentFromL(KMiutResentFrom);
	imHeader->ResentToRecipients().AppendL(KMiutResentToRecipients1);
	imHeader->ResentToRecipients().AppendL(KMiutResentToRecipients2);
	imHeader->ResentCcRecipients().AppendL(KMiutResentCcRecipients1);
	imHeader->ResentCcRecipients().AppendL(KMiutResentCcRecipients2);
	imHeader->ResentBccRecipients().AppendL(KMiutResentBccRecipients);
	//Store the Message header.
	imHeader->StoreL(*fileStore);
	CleanupStack::PopAndDestroy(1); 
	//restore the Message Header
	TestReStoreHeaderL();
	}

/*
Restore the CImHeader and Compare.
*/

void CTestEmailHeaderStoreReStore::TestReStoreHeaderL()
	{
	
	INFO_PRINTF1(_L("ReStore Email Message header .."));
			
	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 1);//2
	ASSERT_TRUE(count == 1);

	recvEntry->SetEntryL((*msvEntrySelection)[0]);
	
	
	imReciveHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imReciveHeader->Reset();
	
	//CMsvStore* fStore=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore=recvEntry->ReadStoreL();
	CleanupStack::PushL(fStore);
	
	imReciveHeader->RestoreL(*fStore);
	
	INFO_PRINTF1(_L("Comparing Email Message header .."));
	
	TBool result1=((imHeader->Subject())==(imReciveHeader->Subject()));
	ASSERT_TRUE(result1 != KErrNone);
	
	TBool result2=((imHeader->ImMsgId())==(imReciveHeader->ImMsgId()));
	ASSERT_TRUE(result2 != KErrNone);
	
	TBool result3=((imHeader->From())==(imReciveHeader->From()));
	ASSERT_TRUE(result3 != KErrNone);
	
	TBool result3a=((imHeader->ReceiptAddress())==(imReciveHeader->ReceiptAddress()));
	ASSERT_TRUE(result3a != KErrNone);
		
	TBool result4=((imHeader->ReplyTo())==(imReciveHeader->ReplyTo()));
	ASSERT_TRUE(result4 != KErrNone);
	
	
	TInt ii;
	TBool result5 = ETrue;
	for ( ii=0; ii<imHeader->ToRecipients().Count(); ii++)
		{
		if (imHeader->ToRecipients().Count()==imReciveHeader->ToRecipients().Count())
			result5&=((imHeader->ToRecipients()[ii])==(imReciveHeader->ToRecipients()[ii]));
		else
			result5&=EFalse;
		}
	ASSERT_TRUE(result5 != KErrNone);


	TBool result6 = ETrue;
	for ( ii=0; ii<imHeader->CcRecipients().Count(); ii++)
		{
		if (imHeader->CcRecipients().Count()==imReciveHeader->CcRecipients().Count())
			result6&=((imHeader->CcRecipients()[ii])==(imReciveHeader->CcRecipients()[ii]));
		else
			result6&=EFalse;
		}
	ASSERT_TRUE(result6 != KErrNone);

	TBool result7 = ETrue;
	for ( ii=0; ii<imHeader->BccRecipients().Count(); ii++)
		{
		if (imHeader->BccRecipients().Count()==imReciveHeader->BccRecipients().Count())
			result7&=((imHeader->BccRecipients()[ii])==(imReciveHeader->BccRecipients()[ii]));
		else
			result7&=EFalse;
		}
	ASSERT_TRUE(result7 != KErrNone);

	TBool result13=((imHeader->BodyEncoding())==(imReciveHeader->BodyEncoding()));
	ASSERT_TRUE(result13 != KErrNone);
	
	
	TBool result8=((imHeader->ResentMsgId())==(imReciveHeader->ResentMsgId()));
	ASSERT_TRUE(result8 != KErrNone);

	TBool result9=((imHeader->ResentFrom())==(imReciveHeader->ResentFrom()));
	ASSERT_TRUE(result9 != KErrNone);
	
	TBool result10 = ETrue;
	for ( ii=0; ii<imHeader->ResentToRecipients().Count(); ii++)
		{
		if (imHeader->ResentToRecipients().Count()==imReciveHeader->ResentToRecipients().Count())
			result10&=((imHeader->ResentToRecipients()[ii])==(imReciveHeader->ResentToRecipients()[ii]));
		else
			result10&=EFalse;
		}
	ASSERT_TRUE(result10 != KErrNone);

	TBool result11 = ETrue;
	for ( ii=0; ii<imHeader->ResentCcRecipients().Count(); ii++)
		{
		if (imHeader->ResentCcRecipients().Count()==imReciveHeader->ResentCcRecipients().Count())
			result11&=((imHeader->ResentCcRecipients()[ii])==(imReciveHeader->ResentCcRecipients()[ii]));
		else
			result11&=EFalse;
		}
	ASSERT_TRUE(result11 != KErrNone);
	
		TBool result12 = ETrue;
	for ( ii=0; ii<imHeader->ResentBccRecipients().Count(); ii++)
		{
		if (imHeader->ResentBccRecipients().Count()==imReciveHeader->ResentBccRecipients().Count())
			result12&=((imHeader->ResentBccRecipients()[ii])==(imReciveHeader->ResentBccRecipients()[ii]));
		else
			result12&=EFalse;
		}
	ASSERT_TRUE(result12 != KErrNone);
	
	TInt test1 =  imHeader->iEncodingInfo->Count();
	ASSERT_EQUALS(test1, 18);

	CleanupStack::PopAndDestroy(2); 
	}

	

/*
test For Encoded header.
Create Account.
Create Encoded Message header.
*/
void CTestEmailHeaderStoreReStore::TestCreateEncodedMessageHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Encoded Header  ");
	INFO_PRINTF1(KFunction);

	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//Create Encoded header.
	iencodedHeader = CImHeader::NewLC(); 
	CleanupStack::Pop();
	iencodedHeader->Reset();

	longValue.Fill('x',KMaxImHeaderStringLengthLong);
	longValue2.Fill('x',KMaxImHeaderStringLengthLong);
	longValue.Zero();
	longValue.Copy(_L8("=?gb2312?B?yrnTw1NPSFW3/s7xxve3osvN08q8/iB0ZXN0IGNoaW5lc2UgY2hhcmFjdGU=?=<ibrahim.rahman@symbian.com>"));

	longValue2.Zero();
	longValue2.Copy(_L8("=?gb2312?B?suLK1NbQzsTX1rf7oaF0ZXN0IGNoaW5lc2UgY2hhcmFjdGVycw==?="));
	
	//Fill the Encoded Header.
	iencodedHeader->SetSubjectL(longValue2);
	iencodedHeader->SetFromL(KMiutFrom);
	iencodedHeader->SetReplyToL(KMiutReplyTo);

	iencodedHeader->ToRecipients().AppendL(longValue);
	iencodedHeader->ToRecipients().AppendL(longValue);

	iencodedHeader->CcRecipients().AppendL(longValue);
	iencodedHeader->CcRecipients().AppendL(longValue);

	iencodedHeader->BccRecipients().AppendL(longValue);
	iencodedHeader->BccRecipients().AppendL(longValue);
	
	//Save the Encoded header.
	iencodedHeader->SaveEncodedHeadersL();
	//Store the Encoded header.
	iencodedHeader->StoreL(*fileStore);
	CleanupStack::PopAndDestroy(1);
	
	//restore the Encoded Message header.
	TestReStoreEncodedHeaderL();
	
	longValue.Zero();
	longValue2.Zero();
	}

/*
Restore the Encoded Message header and Compare .
*/
void CTestEmailHeaderStoreReStore::TestReStoreEncodedHeaderL()
	{
	_LIT(KFunction, ":Test For ReStore Encoded Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 2); //3
	ASSERT_TRUE(count == 2);

	recvEntry->SetEntryL((*msvEntrySelection)[1]);
	
	imEncodedReciveHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imEncodedReciveHeader->Reset();
	
	//CMsvStore* fStore=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore = iEntry->ReadStoreL();
	CleanupStack::PushL(fStore);
	
	imEncodedReciveHeader->RestoreL(*fStore);
	
	INFO_PRINTF1(_L("Comparing Email Encoded Message header .."));

	TBool result1=((iencodedHeader->Subject())==(imEncodedReciveHeader->Subject()));
	ASSERT_TRUE(result1 != KErrNone);


	TBool result2=((iencodedHeader->ImMsgId())==(imEncodedReciveHeader->ImMsgId()));
	ASSERT_TRUE(result2 != KErrNone);
	
	TBool result3=((iencodedHeader->From())==(imEncodedReciveHeader->From()));
	ASSERT_TRUE(result3 != KErrNone);
	
	TBool result3a=((iencodedHeader->ReceiptAddress())==(imEncodedReciveHeader->ReceiptAddress()));
	ASSERT_TRUE(result3a != KErrNone);
		
	TBool result4=((iencodedHeader->ReplyTo())==(imEncodedReciveHeader->ReplyTo()));
	ASSERT_TRUE(result4 != KErrNone);


	TInt ii;
	TBool result5 = ETrue;
	for ( ii=0; ii<iencodedHeader->ToRecipients().Count(); ii++)
		{
		if (iencodedHeader->ToRecipients().Count()==imEncodedReciveHeader->ToRecipients().Count())
			result5&=((iencodedHeader->ToRecipients()[ii])==(imEncodedReciveHeader->ToRecipients()[ii]));
		else
			result5&=EFalse;
		}
	ASSERT_TRUE(result5 != KErrNone);


	TBool result6 = ETrue;
	for ( ii=0; ii<iencodedHeader->CcRecipients().Count(); ii++)
		{
		if (iencodedHeader->CcRecipients().Count()==imEncodedReciveHeader->CcRecipients().Count())
			result6&=((iencodedHeader->CcRecipients()[ii])==(imEncodedReciveHeader->CcRecipients()[ii]));
		else
			result6&=EFalse;
		}
	ASSERT_TRUE(result6 != KErrNone);

	TBool result7 = ETrue;
	for ( ii=0; ii<iencodedHeader->BccRecipients().Count(); ii++)
		{
		if (iencodedHeader->BccRecipients().Count()==imEncodedReciveHeader->BccRecipients().Count())
			result7&=((iencodedHeader->BccRecipients()[ii])==(imEncodedReciveHeader->BccRecipients()[ii]));
		else
			result7&=EFalse;
		}
	ASSERT_TRUE(result7 != KErrNone);
	CleanupStack::PopAndDestroy(2); 
	}

/*
Create  MIme Header.
Create Account.
Create MIME header part and Store it .
*/	


void CTestEmailHeaderStoreReStore::TestCreateMimeMessageHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Mime Header  ");
	INFO_PRINTF1(KFunction);

	
	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//MIME header.
	imMimeHeader = CImMimeHeader::NewLC(); 
	CleanupStack::Pop();
	imMimeHeader->Reset();
	
	//Set  MIME part
	imMimeHeader->SetContentTypeL(KImailHeader1);
	imMimeHeader->SetContentSubTypeL(KImailHeader2);
	imMimeHeader->SetContentDispositionL(KImailHeader9);
	imMimeHeader->SetContentDescriptionL(KImailHeader1);
	imMimeHeader->SetContentBaseL(KImailHeader3);
	imMimeHeader->SetContentLocationL(KImailHeader6);
	imMimeHeader->SetContentIDL(KImailHeader4);
	imMimeHeader->SetRelativePathL(KImRelativePathString);
	
	
	// test the string arrays
	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(KImailParam11);
	array.AppendL(KImailParam2);
	array.AppendL(KImailParam3);
	array.AppendL(KImailParam4);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(KImailParam31);
	arrayB.AppendL(KImailParam4);

	
	CDesC8Array& arrayC=imMimeHeader->XTypeParams(); 
	arrayC.AppendL(KImailParam31);
	arrayC.AppendL(KImailParam4);


	INFO_PRINTF1(_L("Store Email MIME  header .."));
	imMimeHeader->StoreL(*fileStore);

	CleanupStack::PopAndDestroy(1); 
	//Restore the MIME header part and Compare.
	TestReStoreMimedHeaderL();
	}
	


	
/*
ReStore the MIME part 
*/	
void CTestEmailHeaderStoreReStore::TestReStoreMimedHeaderL()
	{
	
	_LIT(KFunction, ":Test ForReStoring MIME Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 3);//4
	ASSERT_TRUE(count == 3);//4

	recvEntry->SetEntryL((*msvEntrySelection)[2]);//3

	
	imMimeReciveHeader = CImMimeHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imMimeReciveHeader->Reset();
	
	//CMsvStore* fStore1=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore1=recvEntry->ReadStoreL();
	CleanupStack::PushL(fStore1);
	
	INFO_PRINTF1(_L("ReStore Email MIME  header .."));
	imMimeReciveHeader->RestoreL(*fStore1);
	
	TBool result1=((KImailHeader1)==(imMimeReciveHeader->ContentType()));
	ASSERT_TRUE(result1 != KErrNone);
	TBool result2=((KImailHeader2)==(imMimeReciveHeader->ContentSubType()));
	ASSERT_TRUE(result2 != KErrNone);
	TBool result3=((KImailHeader9)==(imMimeReciveHeader->ContentDisposition()));
	ASSERT_TRUE(result3 != KErrNone);
	TBool result4=((KImailHeader1)==(imMimeReciveHeader->ContentDescription()));
	ASSERT_TRUE(result4 != KErrNone);
	TBool result5=((KImailHeader3)==(imMimeReciveHeader->ContentBase()));
	ASSERT_TRUE(result5 != KErrNone);
	TBool result6=((KImailHeader4)==(imMimeReciveHeader->ContentID()));
	ASSERT_TRUE(result6 != KErrNone);
		
	TBool result8=((KImailHeader6)==(imMimeReciveHeader->ContentLocation()));
	ASSERT_TRUE(result8 != KErrNone);	
	
	TBool result9=((KImRelativePathString)==(imMimeReciveHeader->RelativePath()));
	ASSERT_TRUE(result9 != KErrNone);
	
	CleanupStack::PopAndDestroy(2); 
	}

	
void CTestEmailHeaderStoreReStore::TestCreateMimeMessageEmptyHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Mime Empty Header  ");
	INFO_PRINTF1(KFunction);

	
	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//MIME header.
	imMimeHeader = CImMimeHeader::NewLC(); 
	CleanupStack::Pop();
	imMimeHeader->Reset();
	
	//Set  MIME part
	
	// test the string arrays
	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(KImailParam2);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(KImailParam4);

	
	CDesC8Array& arrayC=imMimeHeader->XTypeParams(); 
	arrayC.AppendL(KImailParam4);
	

	INFO_PRINTF1(_L("Store Email MIME  header .."));
	imMimeHeader->StoreL(*fileStore);

	CleanupStack::PopAndDestroy(1); 
	//Restore the MIME header part and Compare.
	TestReStoreMimedEmptyHeaderL();
	}

/*
ReStore the MIME part 
*/	
void CTestEmailHeaderStoreReStore::TestReStoreMimedEmptyHeaderL()
	{
	
	_LIT(KFunction, ":Test ForReStoring MIME Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 4);//4
	ASSERT_TRUE(count == 4);//4



	recvEntry->SetEntryL((*msvEntrySelection)[3]);//3

	
	imMimeReciveHeader = CImMimeHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imMimeReciveHeader->Reset();
	
	//CMsvStore* fStore1=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore1=recvEntry->ReadStoreL();
	CleanupStack::PushL(fStore1);
	
	INFO_PRINTF1(_L("ReStore Email MIME  header .."));
	imMimeReciveHeader->RestoreL(*fStore1);

	CleanupStack::PopAndDestroy(2); 
	}



void CTestEmailHeaderStoreReStore::TestCreateMimeMessageReStoreHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Mime ReStore Header  ");
	INFO_PRINTF1(KFunction);

	
	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//MIME header.
	imMimeHeader = CImMimeHeader::NewLC(); 
	CleanupStack::Pop();
	imMimeHeader->Reset();
	
	//Set  MIME part
	
	// test the string arrays
	
	imMimeHeader->SetContentTypeL(KImailHeader1);

	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(KImailParam1);
	//array.AppendL(KImailParam2);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(KImailParam4);
	
	CDesC8Array& arrayC=imMimeHeader->XTypeParams(); 
	arrayC.AppendL(KImailParam4);


	INFO_PRINTF1(_L("Store Email MIME  header .."));
	imMimeHeader->StoreL(*fileStore);

	CleanupStack::PopAndDestroy(1); 
	//Restore the MIME header part and Compare.
	TestReStoreMimedReStoreHeaderL();
	}

/*
ReStore the MIME part 
*/	
void CTestEmailHeaderStoreReStore::TestReStoreMimedReStoreHeaderL()
	{
	
	_LIT(KFunction, ":Test ForReStoring MIME Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	//TInt count = msvEntrySelection->Count();

	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 5);//4
	ASSERT_TRUE(count == 5);//4



	recvEntry->SetEntryL((*msvEntrySelection)[4]);//3 //2 /4

	
	imMimeReciveHeader = CImMimeHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imMimeReciveHeader->Reset();
	
	//CMsvStore* fStore1=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore1=recvEntry->ReadStoreL();
	CleanupStack::PushL(fStore1);
	
	INFO_PRINTF1(_L("ReStore Email MIME  header .."));
	imMimeReciveHeader->RestoreL(*fStore1);

	CleanupStack::PopAndDestroy(2); 
	}


void CTestEmailHeaderStoreReStore::TestCreateMimeMessageReStoreTestHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Test Mime ReStore Header  ");
	INFO_PRINTF1(KFunction);

	
	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//MIME header.
	imMimeHeader = CImMimeHeader::NewLC(); 
	CleanupStack::Pop();
	imMimeHeader->Reset();
	
	//Set  MIME part
	
	// test the string arrays
	
	imMimeHeader->SetContentTypeL(KImailHeader1);

	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(KImailParam1);
	array.AppendL(KImailParam2);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(KImailParam4);
	arrayB.AppendL(KImailParam4);
	
	CDesC8Array& arrayC=imMimeHeader->XTypeParams(); 
	arrayC.AppendL(KImailParam4);
	arrayC.AppendL(KImailParam4);


	INFO_PRINTF1(_L("Store Email MIME  header .."));
	imMimeHeader->StoreL(*fileStore);

	CleanupStack::PopAndDestroy(1); 
	//Restore the MIME header part and Compare.
	TestReStoreMimedReStoreTestHeaderL();
	}

/*
ReStore the MIME part 
*/	
void CTestEmailHeaderStoreReStore::TestReStoreMimedReStoreTestHeaderL()
	{
	
	_LIT(KFunction, ":Test For ReStoring Test MIME Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	//TInt count = msvEntrySelection->Count();

	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 6);//4
	ASSERT_TRUE(count == 6);//4


	recvEntry->SetEntryL((*msvEntrySelection)[5]); //5

	
	imMimeReciveHeader = CImMimeHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imMimeReciveHeader->Reset();
	
	//CMsvStore* fStore1=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore1=recvEntry->ReadStoreL();
	CleanupStack::PushL(fStore1);
	
	INFO_PRINTF1(_L("ReStore Email MIME  header .."));
	imMimeReciveHeader->RestoreL(*fStore1);

	CleanupStack::PopAndDestroy(2); 
	}



void CTestEmailHeaderStoreReStore::TestCreateMimeMessageEmptyXtypeTestHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Empty Xtype Mime ReStore Header  ");
	INFO_PRINTF1(KFunction);

	
	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//MIME header.
	imMimeHeader = CImMimeHeader::NewLC(); 
	CleanupStack::Pop();
	imMimeHeader->Reset();
	
	//Set  MIME part
	
	// test the string arrays
	
	imMimeHeader->SetContentTypeL(KImailHeader1);

	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(KImailParam100);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(KImailParam100);
	
	CDesC8Array& arrayC=imMimeHeader->XTypeParams(); 
	arrayC.AppendL(KImailParam100);


	INFO_PRINTF1(_L("Store Email MIME  header .."));
	imMimeHeader->StoreL(*fileStore);

	CleanupStack::PopAndDestroy(1); 
	//Restore the MIME header part and Compare.
	TestReStoreMimedEmptyXtypeTestHeaderL();
	}

/*
ReStore the MIME part 
*/	
void CTestEmailHeaderStoreReStore::TestReStoreMimedEmptyXtypeTestHeaderL()
	{
	
	_LIT(KFunction, ":Test ForReStoring Empty Xtype  MIME Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	//TInt count = msvEntrySelection->Count();

	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 7);//4
	ASSERT_TRUE(count == 7);//4


	recvEntry->SetEntryL((*msvEntrySelection)[6]);//3 //2 //5

	
	imMimeReciveHeader = CImMimeHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imMimeReciveHeader->Reset();
	
	//CMsvStore* fStore1=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore1=recvEntry->ReadStoreL();
	CleanupStack::PushL(fStore1);
	
	INFO_PRINTF1(_L("ReStore Email MIME  header .."));
	imMimeReciveHeader->RestoreL(*fStore1);

	CleanupStack::PopAndDestroy(2); 
	}



void CTestEmailHeaderStoreReStore::TestCreateMimeMessageEmptyMimeOneTestHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Mime One ReStore Header  ");
	INFO_PRINTF1(KFunction);

	
	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//MIME header.
	imMimeHeader = CImMimeHeader::NewLC(); 
	CleanupStack::Pop();
	imMimeHeader->Reset();
	
	//Set  MIME part
	
	// test the string arrays
	
	imMimeHeader->SetContentTypeL(KImailHeader1); 

	CDesC8Array& array=imMimeHeader->ContentTypeParams();
	array.AppendL(KImailParam1000);
	array.AppendL(KImailParam2000);
	array.AppendL(KImailParam2000);

	CDesC8Array& arrayB=imMimeHeader->ContentDispositionParams();
	arrayB.AppendL(KImailParam1000);
	arrayB.AppendL(KImailParam2000);
	arrayB.AppendL(KImailParam2000);
	
	CDesC8Array& arrayC=imMimeHeader->XTypeParams(); 
	arrayC.AppendL(KImailParam1000);
	arrayC.AppendL(KImailParam2000);
	arrayC.AppendL(KImailParam2000);


	INFO_PRINTF1(_L("Store Email MIME  header .."));
	imMimeHeader->StoreL(*fileStore);

	CleanupStack::PopAndDestroy(1); 
	//Restore the MIME header part and Compare.
	TestReStoreMimedEmptyOneTestHeaderL();
	}

/*
ReStore the MIME part 
*/	
void CTestEmailHeaderStoreReStore::TestReStoreMimedEmptyOneTestHeaderL()
	{
	
	_LIT(KFunction, ":Test ForReStoring One MIME Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	//TInt count = msvEntrySelection->Count();

	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 8);//4
	ASSERT_TRUE(count == 8);//4


	recvEntry->SetEntryL((*msvEntrySelection)[7]);//3 //2 //5

	
	imMimeReciveHeader = CImMimeHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imMimeReciveHeader->Reset();
	
	//CMsvStore* fStore1=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore1=recvEntry->ReadStoreL();
	CleanupStack::PushL(fStore1);
	
	INFO_PRINTF1(_L("ReStore Email MIME  header .."));
	imMimeReciveHeader->RestoreL(*fStore1);

	CleanupStack::PopAndDestroy(2); 
	}


/*
test For Encoded header.
Create Account.
Create Encoded Message header.
*/
void CTestEmailHeaderStoreReStore::TestCreateEmptytestEncodedMessageHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Empty Encoded Header  ");
	INFO_PRINTF1(KFunction);

	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//Create Encoded header.
	iencodedHeader = CImHeader::NewLC(); 
	CleanupStack::Pop();	
	iencodedHeader->Reset();
	longValue.Copy(_L8("<ibrahim.rahman@symbian.com>"));

	longValue2.Copy(_L8("=?="));
	
	//Fill the Encoded Header.
	iencodedHeader->SetSubjectL(longValue2);
	iencodedHeader->SetFromL(KMiutFrom);
	iencodedHeader->SetReplyToL(KMiutReplyTo);

	iencodedHeader->ToRecipients().AppendL(longValue);

	iencodedHeader->CcRecipients().AppendL(longValue);

	iencodedHeader->BccRecipients().AppendL(longValue);

	//Save the Encoded header.
	iencodedHeader->SaveEncodedHeadersL();
	//Store the Encoded header.
	iencodedHeader->StoreL(*fileStore);
	CleanupStack::PopAndDestroy(1);
	
	//restore the Encoded Message header.
	TestReStoreEmptytestEncodedHeaderL();
	
	longValue.Zero();
	longValue2.Zero();
	}

/*
Restore the Encoded Message header and Compare .
*/
void CTestEmailHeaderStoreReStore::TestReStoreEmptytestEncodedHeaderL()
	{
	_LIT(KFunction, ":Test For ReStore Empty Encoded Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 9); //3
	ASSERT_TRUE(count == 9);

	recvEntry->SetEntryL((*msvEntrySelection)[8]);
	
	imEncodedReciveHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imEncodedReciveHeader->Reset();
	
	//CMsvStore* fStore=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore = iEntry->ReadStoreL();
	CleanupStack::PushL(fStore);
	
	imEncodedReciveHeader->RestoreL(*fStore);
	
	CleanupStack::PopAndDestroy(2); 
	}





/*
test For Encoded header.
Create Account.
Create Encoded Message header.
*/
void CTestEmailHeaderStoreReStore::TestCreateEmptytestEncodedMessageOneHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Empty Encoded Header  ");
	INFO_PRINTF1(KFunction);

	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//Create Encoded header.
	iencodedHeader = CImHeader::NewLC(); 
	CleanupStack::Pop();
	iencodedHeader->Reset();

	longValue.Copy(_L8(""));

	
	//Fill the Encoded Header.
	iencodedHeader->SetSubjectL(longValue2);
	iencodedHeader->SetFromL(KMiutFrom);
	iencodedHeader->SetReplyToL(KMiutReplyTo);

	iencodedHeader->ToRecipients().AppendL(longValue);

	iencodedHeader->CcRecipients().AppendL(longValue);

	iencodedHeader->BccRecipients().AppendL(longValue);
	
	//Save the Encoded header.
	iencodedHeader->SaveEncodedHeadersL();

	//Store the Encoded header.
	iencodedHeader->StoreL(*fileStore);
	CleanupStack::PopAndDestroy(1);
	
	//restore the Encoded Message header.
	TestReStoreEmptytestEncodedOneHeaderL();
	
	longValue.Zero();
	longValue2.Zero();
	}

/*
Restore the Encoded Message header and Compare .
*/
void CTestEmailHeaderStoreReStore::TestReStoreEmptytestEncodedOneHeaderL()
	{
	_LIT(KFunction, ":Test For ReStore Two Encoded Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 10); //3
	ASSERT_TRUE(count == 10);

	recvEntry->SetEntryL((*msvEntrySelection)[9]);
	
	imEncodedReciveHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imEncodedReciveHeader->Reset();
	
	//CMsvStore* fStore=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore = iEntry->ReadStoreL();
	CleanupStack::PushL(fStore);
	
	imEncodedReciveHeader->RestoreL(*fStore);
	
	CleanupStack::PopAndDestroy(2); 
	}


/*
test For Encoded header.
Create Account.
Create Encoded Message header.
*/
void CTestEmailHeaderStoreReStore::TestCreateEmptytestEncodedMessageTwoHeaderL()
	{
	_LIT(KFunction, ":Test For Storing Two Encoded Header  ");
	INFO_PRINTF1(KFunction);

	CreateAccountL();	
	//Create msg entry 
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypePOP3;

	iEntry->CreateL(entry1);
	iEntry->SetEntryL(entry1.Id());

	CMsvStore* fileStore = iEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	
	//Create Encoded header.
	iencodedHeader = CImHeader::NewLC(); 
	CleanupStack::Pop();
	iencodedHeader->Reset();

	//longValue.Fill('x',KMaxImHeaderStringLengthLong);
	//longValue3.Fill('x',KMaxImHeaderStringLengthLong);
	//longValue.Zero();
	longValue.Copy(_L8("test"));

	//longValue3.Zero();
	longValue3.Copy(_L8(" test1"));
	
	//Fill the Encoded Header.
	iencodedHeader->SetSubjectL(longValue2);
	iencodedHeader->SetFromL(KMiutFrom);
	iencodedHeader->SetReplyToL(KMiutReplyTo);

	iencodedHeader->ToRecipients().AppendL(longValue);
	iencodedHeader->ToRecipients().AppendL(longValue3);
	iencodedHeader->ToRecipients().AppendL(longValue);

	iencodedHeader->CcRecipients().AppendL(longValue);
	iencodedHeader->CcRecipients().AppendL(longValue3);
	iencodedHeader->CcRecipients().AppendL(longValue);

	iencodedHeader->BccRecipients().AppendL(longValue);
	iencodedHeader->BccRecipients().AppendL(longValue3);
	iencodedHeader->BccRecipients().AppendL(longValue);
	
	//Save the Encoded header.
	iencodedHeader->SaveEncodedHeadersL();
	//Store the Encoded header.
	iencodedHeader->StoreL(*fileStore);
	CleanupStack::PopAndDestroy(1);
	
	//restore the Encoded Message header.
	TestReStoreEmptytestEncodedTwoHeaderL();
	
	longValue.Zero();
	longValue3.Zero();
	}

/*
Restore the Encoded Message header and Compare .
*/
void CTestEmailHeaderStoreReStore::TestReStoreEmptytestEncodedTwoHeaderL()
	{
	_LIT(KFunction, ":Test For ReStore Two Encoded Header  ");
	INFO_PRINTF1(KFunction);

	TMsvSelectionOrdering ordering;	
	ordering.ShowInvisibleEntries();
	
		
	CMsvEntry* recvEntry = CMsvEntry::NewL(*iSession, KMsvGlobalInBoxIndexEntryId, ordering);
	
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=recvEntry->ChildrenL();
	CleanupStack::PushL(msvEntrySelection);
	
	// Check how many mesasges there are - should be just 1
	TInt count = msvEntrySelection->Count();
	INFO_PRINTF3(_L("Number of new messages: %d, should be: %d"), count, 11); //3
	ASSERT_TRUE(count == 11);

	recvEntry->SetEntryL((*msvEntrySelection)[10]);
	
	imEncodedReciveHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	CleanupStack::Pop();
	imEncodedReciveHeader->Reset();
	
	//CMsvStore* fStore=recvEntry->ReadDBStoreL(EHeaderEntry);
	CMsvStore* fStore = iEntry->ReadStoreL();
	CleanupStack::PushL(fStore);
	
	imEncodedReciveHeader->RestoreL(*fStore);
	
	CleanupStack::PopAndDestroy(2); 
	}



CTestSuite* CTestEmailHeaderStoreReStore::CreateSuiteL(const TDesC& aName)
	{
	SUB_SUITE;
	//test for CImHeader
	ADD_ASYNC_TEST_STEP(TestStoreRestoreHeaderL); //0
	//test for Encoded Header
	ADD_ASYNC_TEST_STEP(TestCreateEncodedMessageHeaderL); //1
	//test for Mime header
	ADD_ASYNC_TEST_STEP(TestCreateMimeMessageHeaderL); //2 
	
	ADD_ASYNC_TEST_STEP(TestCreateMimeMessageEmptyHeaderL);//3 
	
	ADD_ASYNC_TEST_STEP(TestCreateMimeMessageReStoreHeaderL);//4
	
	ADD_ASYNC_TEST_STEP(TestCreateMimeMessageReStoreTestHeaderL); //5
	
	ADD_ASYNC_TEST_STEP(TestCreateMimeMessageEmptyXtypeTestHeaderL); // 6
	
	ADD_ASYNC_TEST_STEP(TestCreateMimeMessageEmptyMimeOneTestHeaderL); // 7
	
	ADD_ASYNC_TEST_STEP(TestCreateEmptytestEncodedMessageHeaderL); // 8
	
	ADD_ASYNC_TEST_STEP(TestCreateEmptytestEncodedMessageOneHeaderL); //9
	
	ADD_ASYNC_TEST_STEP(TestCreateEmptytestEncodedMessageTwoHeaderL); //10
	
	END_SUITE;
	}








