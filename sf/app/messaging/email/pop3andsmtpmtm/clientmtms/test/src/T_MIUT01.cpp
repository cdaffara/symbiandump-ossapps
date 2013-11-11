// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Name of test harness: T_MIUT01
// Component: IMCM
// Owner: KP
// Brief description of test harness:
// Tests storing/retrieving data from CImHeader - most of the functionality is
// tested - some new functions may not be tested!
// Detailed description of test harness:
// As above.
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// <DRIVE>:\msgtest\T_MIUT01\T_IMCM.log
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_MIUT01.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// The following instructions are the same for all platforms:
// 1. Build T_DB test harness from COMMDB component:
// cd \commdb\group
// bldmake bldfiles
// abld test build t_db
// 2. Build the test utilities:
// cd \msg\testutils\group\
// bldmake bldfiles
// abld build
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_MIUT01.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_MIUT01.exe onto the other platform
// 2. Copy \epoc32\release\<PLATFORM>\<VARIANT>\MSVTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 3. Copy \epoc32\release\<PLATFORM>\<VARIANT>\EMAILTESTUTILS.DLL into 
// <DRIVE>:\system\libs on the other platform
// 4. Run T_MIUT01.exe on the other platform
// 
//

#include "emailtestutils.h"
#include "MIUTHDR.H"    // CImHeader
#include <pop3set.h>
#include <miutlog.h>

#include <msventry.h>
#include "MSVSERV.H"
#include <msvids.h>
#include <miutconv.h>
#include <charconv.h>
#include <imcvcodc.h>
#include <cemailaccounts.h>
#include <iapprefs.h>

//
// Still need to test:
//	- Charset()/SetCharset()
//	- EncodingInfo()/AddEncodingInfo()
//

// local variables etc //

_LIT(KMiut01Test, "T_MIUT01 - Test CImHeader class");
RTest test(KMiut01Test);
LOCAL_D CTrapCleanup* theCleanup;

LOCAL_D CImHeader* imHeader;
LOCAL_D CImHeader* imHeader2;
LOCAL_D CImHeader* imHeaderVerify;	// To verify stored header identical to original
LOCAL_D TMsvEntry entry; 
LOCAL_D CEmailTestUtils* testUtils;
LOCAL_D CImLog* log;

_LIT(KMiutSubject, "short mail about something interesting");
_LIT8(KMiutMsgId, "0123456789AB.CdeFGHIj@symbian.com");
_LIT(KMiutReceiptAddress, "i-want-a.receipt.com>");
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

_LIT(KMiutTestPassed, "PASSED test %d\n");
_LIT(KMiutTestFailed, "FAILED test %d\n");



#define KMiutRemoteSize 					123456
#define KMaxImHeaderStringLengthLong		KMaxImHeaderStringLength+1

LOCAL_D TBuf<KMaxImHeaderStringLengthLong> longValue;
LOCAL_D TBuf8<KMaxImHeaderStringLengthLong> longValue2;

//

LOCAL_C void ResultOfTest(TBool aResult,TInt aTestNo)
	{
	if (aResult)
		{
		test.Printf(KMiutTestPassed, aTestNo);
		}
	else
		{
		test.Printf(KMiutTestFailed, aTestNo);
		}
	}

LOCAL_C void StoreMessageHeaderL(CImHeader* aCImHeader)
	{
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	testUtils->iMsvEntry->CreateL(entry);
	TRAPD(error,testUtils->iMsvEntry->SetEntryL(entry.Id()));
	CMsvStore* fileStore = testUtils->iMsvEntry->EditStoreL();
	CleanupStack::PushL(fileStore);
	aCImHeader->StoreL(*fileStore);
	fileStore->CommitL();
	CleanupStack::PopAndDestroy(); // filestore
	}

LOCAL_C void RestoreMessageHeaderL(CImHeader* aCImHeader)
	{
	aCImHeader->Reset();
	CMsvStore* fStore=testUtils->iMsvEntry->ReadStoreL();
	CleanupStack::PushL(fStore);
	aCImHeader->RestoreL(*fStore);
	CleanupStack::PopAndDestroy(); //fStore
	}

//
// CImHeader Test Functions //
//

LOCAL_C void CreateMessageHeader()
	{
	// a bunch of data for the header object....
	imHeader->Reset();
	imHeader->SetSubjectL(KMiutSubject);
	imHeader->SetImMsgIdL(KMiutMsgId);
	imHeader->SetFromL(KMiutFrom);
	imHeader->SetReplyToL(KMiutReplyTo);
	imHeader->SetReceiptAddressL(KMiutReceiptAddress);
	imHeader->SetRemoteSize(KMiutRemoteSize);

	imHeader->ToRecipients().AppendL(KMiutToRecipients1);
	imHeader->ToRecipients().AppendL(KMiutToRecipients2);
	imHeader->CcRecipients().AppendL(KMiutCcRecipients1);
	imHeader->CcRecipients().AppendL(KMiutCcRecipients2);
	imHeader->BccRecipients().AppendL(KMiutBccRecipients);
	imHeader->SetBodyEncoding(EMsgOutboxMIME);
	log->AppendComment(_L8("\tCreated Message Header 1"));
	}

LOCAL_C void CreateMessageHeader2()
	{
	// a bunch of data for the header object....
	imHeader->Reset();
	imHeader->SetSubjectL(KMiutSubject);

	imHeader->SetImMsgIdL(KMiutMsgId);
	imHeader->SetReceiptAddressL(KMiutReceiptAddress);

	imHeader->SetFromL(KMiutFrom);
	imHeader->SetReplyToL(KMiutReplyTo);
	imHeader->SetRemoteSize(KMiutRemoteSize);

	imHeader->ToRecipients().AppendL(KMiutToRecipients1);
	imHeader->ToRecipients().AppendL(KMiutToRecipients2);
	imHeader->CcRecipients().AppendL(KMiutCcRecipients1);
	imHeader->CcRecipients().AppendL(KMiutCcRecipients2);
	imHeader->BccRecipients().AppendL(KMiutBccRecipients);
	
	imHeader->SetResentMsgIdL(KMiutResentMsgId);
	imHeader->SetResentFromL(KMiutResentFrom);
	imHeader->ResentToRecipients().AppendL(KMiutResentToRecipients1);
	imHeader->ResentToRecipients().AppendL(KMiutResentToRecipients2);
	imHeader->ResentCcRecipients().AppendL(KMiutResentCcRecipients1);
	imHeader->ResentCcRecipients().AppendL(KMiutResentCcRecipients2);
	imHeader->ResentBccRecipients().AppendL(KMiutResentBccRecipients);
	imHeader->SetBodyEncoding(EMsgOutboxMIME);
	log->AppendComment(_L8("\tCreated Message Header 2"));
	}

LOCAL_C void CreateMessageHeader3()
	{
	//Create message header without receipt address
	imHeader->Reset();
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
	log->AppendComment(_L8("\tCreated Message Header 1"));
	}

//The function populates the header with sender address in Cc list. 
//Also inserts duplicate addresses in both To and Cc list.
LOCAL_C void CreateMessageHeader4()
	{
	imHeader->Reset();
	imHeader->SetSubjectL(KMiutSubject);
	imHeader->SetImMsgIdL(KMiutMsgId);
	imHeader->SetFromL(KMiutFrom);
	imHeader->SetRemoteSize(KMiutRemoteSize);

	imHeader->ToRecipients().AppendL(KMiutToRecipients1);
	imHeader->ToRecipients().AppendL(KMiutToRecipients2);
	imHeader->ToRecipients().AppendL(KMiutToRecipients2);
	
	imHeader->CcRecipients().AppendL(KMiutFrom);
	imHeader->CcRecipients().AppendL(KMiutCcRecipients1);
	imHeader->CcRecipients().AppendL(KMiutCcRecipients1);
	
	imHeader->SetBodyEncoding(EMsgOutboxMIME);
	log->AppendComment(_L8("\tCreated Message Header 4"));
	}

LOCAL_C void CreateLongMessageHeader()
	{
	// a bunch of data for the header object....
	// all the fields are set to a length greater than 1000 i.e 1001

	imHeader->Reset();
	longValue.Fill('x',KMaxImHeaderStringLengthLong);
	longValue2.Fill('x',KMaxImHeaderStringLengthLong);

	imHeader->SetSubjectL(longValue);

	imHeader->SetImMsgIdL(longValue2);
	imHeader->SetReceiptAddressL(longValue);

	imHeader->SetFromL(longValue);
	imHeader->SetReplyToL(longValue);
	imHeader->SetRemoteSize(KMiutRemoteSize);

	imHeader->ToRecipients().AppendL(longValue);
	imHeader->ToRecipients().AppendL(longValue);
	imHeader->CcRecipients().AppendL(longValue);
	imHeader->CcRecipients().AppendL(longValue);
	imHeader->BccRecipients().AppendL(longValue);

	imHeader->SetResentMsgIdL(longValue2);
	imHeader->SetResentFromL(longValue);
	imHeader->ResentToRecipients().AppendL(longValue);
	imHeader->ResentToRecipients().AppendL(longValue);
	imHeader->ResentCcRecipients().AppendL(longValue);
	imHeader->ResentCcRecipients().AppendL(longValue);
	imHeader->ResentBccRecipients().AppendL(longValue);
	imHeader->SetBodyEncoding(EMsgOutboxMIME);
	log->AppendComment(_L8("\tCreated Long Message Header"));
	}

LOCAL_C TBool CheckGB2312Descriptor(const TDesC &aDes)
	{
	TUint16 refDecodeData[42]={0x7F8E,0xFFFD,0x5766,0x627F,0x7BA1,0x5236,0x6838,0x5B50,0x539F,0x6599,
		0x8D70,0x79C1,0x56F0,0xFFFD,0x003C,0x0069,0x0062,0x0072,0x0061,0x0068,0x0069,
		0x006D,0x002E,0x0072,0x0061,0x0068,0x006D,0x0061,0x006E,0x0040,0x0073,0x0079,
		0x006D,0x0062,0x0069,0x0061,0x006E,0x002E,0x0063,0x006F,0x006D,0x003E};


	TPtrC16 compareagainst(refDecodeData,42);

	return(aDes.Compare(compareagainst)==0 ? ETrue : EFalse);
	}

LOCAL_C void CreateGB2312EncodedNonMimeMessageHeader(CImHeader *aHeader)
	{
	// a bunch of data for the header object....
	// all the fields are set to a length greater than 1000 i.e 1001
	aHeader->Reset();
	longValue.Zero();
	longValue.Copy(_L8("美國坦承管制核子原料走私困難<ibrahim.rahman@symbian.com>"));

	
	aHeader->SetSubjectL(longValue);
	aHeader->ToRecipients().AppendL(longValue);
	log->AppendComment(_L8("\tCreated Encoded Non Mime Message Header"));

	longValue.Zero();
	}

LOCAL_C void CreateEncodedMessageHeader(TBool aWrongEncoding, CImHeader *aHeader)
	{
	// a bunch of data for the header object....
	// all the fields are set to a length greater than 1000 i.e 1001
	aHeader->Reset();
	//longValue.Fill('x',KMaxImHeaderStringLengthLong);
	//longValue2.Fill('x',KMaxImHeaderStringLengthLong);
	longValue.Zero();
	if (!aWrongEncoding)
		longValue.Copy(_L8("=?gb2312?B?yrnTw1NPSFW3/s7xxve3osvN08q8/iB0ZXN0IGNoaW5lc2UgY2hhcmFjdGU=?=<ibrahim.rahman@symbian.com>"));
	else
		longValue.Copy(_L8("=?utf-7?B?yrnTw1NPSFW3/s7xxve3osvN08q8/iB0ZXN0IGNoaW5lc2UgY2hhcmFjdGU=?=<ibrahim.rahman@symbian.com>"));

	longValue2.Zero();
	if (!aWrongEncoding)
		longValue2.Copy(_L8("=?gb2312?B?suLK1NbQzsTX1rf7oaF0ZXN0IGNoaW5lc2UgY2hhcmFjdGVycw==?="));
	else
		longValue2.Copy(_L8("=?utf-7?B?suLK1NbQzsTX1rf7oaF0ZXN0IGNoaW5lc2UgY2hhcmFjdGVycw==?="));

	aHeader->SetSubjectL(longValue2);
	aHeader->SetFromL(longValue);

	aHeader->ToRecipients().AppendL(longValue);
	aHeader->ToRecipients().AppendL(longValue);

	aHeader->CcRecipients().AppendL(longValue);
	aHeader->CcRecipients().AppendL(longValue);

	aHeader->BccRecipients().AppendL(longValue);

	log->AppendComment(_L8("\tCreated Encoded Message Header"));

	longValue.Zero();
	longValue2.Zero();
	}


LOCAL_C TBool CompareFilledMessageHeader(TInt aTest)
	{
	// need to check each of the fields of the stored header (imHeaderVerify) are 
	// the same as that originally entered (imHeader)
	TBool result = ETrue;
	log->AppendComment(_L8("\tComparing Filled Message Header"));
	TBool result1=((imHeader->Subject())==(imHeaderVerify->Subject()));
	if (!result1)
		log->AppendError(_L8("\t\tSubjects not identical"),-1);
	result &= result1;
	TBool result2=((imHeader->ImMsgId())==(imHeaderVerify->ImMsgId()));
	if (!result2)
		log->AppendError(_L8("\t\tMsgIds not identical"),-1);
	result &= result2;
	TBool result3=((imHeader->From())==(imHeaderVerify->From()));
	if (!result3)
		log->AppendError(_L8("\t\tFrom fields not identical"),-1);
	result &= result3;
	TBool result3a=((imHeader->ReceiptAddress())==(imHeaderVerify->ReceiptAddress()));
	if (!result3a)
		log->AppendError(_L8("\t\tRecipient Address fields not identical"),-1);
	result &= result3a;
	TBool result4=((imHeader->ReplyTo())==(imHeaderVerify->ReplyTo()));
	if (!result4)
		log->AppendError(_L8("\t\tReplyTo fields not identical"),-1);
	result &= result4;

	TInt ii;
	TBool result5 = ETrue;
	for ( ii=0; ii<imHeader->ToRecipients().Count(); ii++)
		{
		if (imHeader->ToRecipients().Count()==imHeaderVerify->ToRecipients().Count())
			result5&=((imHeader->ToRecipients()[ii])==(imHeaderVerify->ToRecipients()[ii]));
		else
			result5&=EFalse;
		}
	if (!result5)
		log->AppendError(_L8("\t\tTo Recipients not identical"),-1);
	result &= result5;

	TBool result6 = ETrue;
	for ( ii=0; ii<imHeader->CcRecipients().Count(); ii++)
		{
		if (imHeader->CcRecipients().Count()==imHeaderVerify->CcRecipients().Count())
			result6&=((imHeader->CcRecipients()[ii])==(imHeaderVerify->CcRecipients()[ii]));
		else
			result6&=EFalse;
		}
	if (!result6)
		log->AppendError(_L8("\t\tCc Recipients not identical"),-1);
	result &= result6;

	TBool result7 = ETrue;
	for ( ii=0; ii<imHeader->BccRecipients().Count(); ii++)
		{
		if (imHeader->BccRecipients().Count()==imHeaderVerify->BccRecipients().Count())
			result7&=((imHeader->BccRecipients()[ii])==(imHeaderVerify->BccRecipients()[ii]));
		else
			result7&=EFalse;
		}
	if (!result7)
		log->AppendError(_L8("\t\tBcc Recipients not identical"),-1);
	result &= result7;

	TBool result8=((imHeader->ResentMsgId())==(imHeaderVerify->ResentMsgId()));
	if (!result8)
		log->AppendError(_L8("\t\tResent-MsgIds not identical"),-1);
	result &= result8;
	TBool result9=((imHeader->ResentFrom())==(imHeaderVerify->ResentFrom()));
	if (!result9)
		log->AppendError(_L8("\t\tResent-From fields not identical"),-1);
	result &= result9;

	TBool result10 = ETrue;
	for ( ii=0; ii<imHeader->ResentToRecipients().Count(); ii++)
		{
		if (imHeader->ResentToRecipients().Count()==imHeaderVerify->ResentToRecipients().Count())
			result10&=((imHeader->ResentToRecipients()[ii])==(imHeaderVerify->ResentToRecipients()[ii]));
		else
			result10&=EFalse;
		}
	if (!result10)
		log->AppendError(_L8("\t\tResent-To Recipients not identical"),-1);
	result &= result10;

	TBool result11 = ETrue;
	for ( ii=0; ii<imHeader->ResentCcRecipients().Count(); ii++)
		{
		if (imHeader->ResentCcRecipients().Count()==imHeaderVerify->ResentCcRecipients().Count())
			result11&=((imHeader->ResentCcRecipients()[ii])==(imHeaderVerify->ResentCcRecipients()[ii]));
		else
			result11&=EFalse;
		}
	if (!result11)
		log->AppendError(_L8("\t\tResent-Cc Recipients not identical"),-1);
	result &= result11;

	TBool result12 = ETrue;
	for ( ii=0; ii<imHeader->ResentBccRecipients().Count(); ii++)
		{
		if (imHeader->ResentBccRecipients().Count()==imHeaderVerify->ResentBccRecipients().Count())
			result12&=((imHeader->ResentBccRecipients()[ii])==(imHeaderVerify->ResentBccRecipients()[ii]));
		else
			result12&=EFalse;
		}
	if (!result12)
		log->AppendError(_L8("\t\tResent-Bcc Recipients not identical"),-1);
	result &= result12;

	TBool result13=((imHeader->BodyEncoding())==(imHeaderVerify->BodyEncoding()));
	if (!result13)
		log->AppendError(_L8("\t\tBody Encoding (charset) not identical"),-1);
	result &= result13;

	ResultOfTest(result,aTest);
	log->AppendComment(_L8("\tCompared Filled Message Header"));
	return result;
	}

LOCAL_C TBool CompareBlankMessageHeader(TInt aTest)
	{
	// need to check each of the fields of the stored header (imHeaderVerify) are 
	// the same as that originally entered (imHeader).  Here the length should be 
	// 0 so we don't actually need to compare!
	TBool result = ETrue;
	log->AppendComment(_L8("\tComparing Blank Message Header"));
	TBool result1=((imHeader->Subject().Length()==0)==(imHeaderVerify->Subject().Length()==0));
	if (!result1)
		log->AppendError(_L8("\t\tSubjects not identical"),-1);
	result &= result1;
	TBool result2=((imHeader->ImMsgId().Length()==0)==(imHeaderVerify->ImMsgId().Length()==0));
	if (!result2)
		log->AppendError(_L8("\t\tMsgIds not identical"),-1);
	result &= result2;
	TBool result3=((imHeader->From().Length()==0)==(imHeaderVerify->From().Length()==0));
	if (!result3)
		log->AppendError(_L8("\t\tFrom fields not identical"),-1);
	result &= result3;
	TBool result3a=((imHeader->ReceiptAddress().Length()==0)==(imHeaderVerify->ReceiptAddress().Length()==0));
	if (!result3a)
		log->AppendError(_L8("\t\tRecipient Address fields not identical"),-1);
	result &= result3a;
	TBool result4=((imHeader->ReplyTo().Length()==0)==(imHeaderVerify->ReplyTo().Length()==0));
	if (!result4)
		log->AppendError(_L8("\t\tReplyTos not identical"),-1);
	result &= result4;

	TBool result5=(imHeader->ToRecipients().Count()==imHeaderVerify->ToRecipients().Count()!=EFalse);
	if (!result5)
		log->AppendError(_L8("\t\tTo Recipients not identical"),-1);
	result &= result5;
	TBool result6=(imHeader->CcRecipients().Count()==imHeaderVerify->CcRecipients().Count()!=EFalse);
	if (!result6)
		log->AppendError(_L8("\t\tCc Recipients not identical"),-1);
	result &= result6;
	TBool result7=(imHeader->BccRecipients().Count()==imHeaderVerify->BccRecipients().Count()!=EFalse);
	if (!result7)
		log->AppendError(_L8("\t\tBcc Recipients not identical"),-1);
	result &= result7;

	TBool result8=((imHeader->ResentMsgId().Length()==0)==(imHeaderVerify->ResentMsgId().Length()==0));
	if (!result8)
		log->AppendError(_L8("\t\tResent-MsgIds not identical"),-1);
	result &= result8;
	TBool result9=((imHeader->ResentFrom().Length()==0)==(imHeaderVerify->ResentFrom().Length()==0));
	if (!result9)
		log->AppendError(_L8("\t\tResent-From fields not identical"),-1);
	result &= result9;

	TBool result10=(imHeader->ResentToRecipients().Count()==imHeaderVerify->ResentToRecipients().Count()!=EFalse);
	if (!result10)
		log->AppendError(_L8("\t\tResent-To Recipients not identical"),-1);
	result &= result10;
	TBool result11=(imHeader->ResentCcRecipients().Count()==imHeaderVerify->ResentCcRecipients().Count()!=EFalse);
	if (!result11)
		log->AppendError(_L8("\t\tResent-Cc Recipients not identical"),-1);
	result &= result11;
	TBool result12=(imHeader->ResentBccRecipients().Count()==imHeaderVerify->ResentBccRecipients().Count()!=EFalse);
	if (!result12)
		log->AppendError(_L8("\t\tResent-Bcc Recipients not identical"),-1);
	result &= result12;

	TBool result13=((imHeader->BodyEncoding())==(imHeaderVerify->BodyEncoding()));
	if (!result13)
		log->AppendError(_L8("\t\tBody Encoding (charset) not identical"),-1);
	result &= result13;

	ResultOfTest(result,aTest);
	log->AppendComment(_L8("\tCompared Blank Message Header"));
	return result;
	}

LOCAL_C TBool CompareLongMessageHeader(TInt aTest)
	{
	log->AppendComment(_L8("\tComparing Long Message Header"));
	TBool result = ETrue;
	TBool result1=((imHeader->Subject().Left(KMaxImHeaderStringLength))==(imHeaderVerify->Subject()));
	if (!result1)
		log->AppendError(_L8("\t\tSubjects not identical"),-1);
	result &= result1;
	TBool result2=((imHeader->ImMsgId().Left(KMaxImHeaderStringLength))==(imHeaderVerify->ImMsgId()));
	if (!result2)
		log->AppendError(_L8("\t\tMessageIds not identical"),-1);
	result &= result2;
	TBool result3=((imHeader->From().Left(KMaxImHeaderStringLength))==(imHeaderVerify->From()));
	if (!result3)
		log->AppendError(_L8("\t\tFrom Fields not identical"),-1);
	result &= result3;
	TBool result3a=((imHeader->ReceiptAddress().Left(KMaxImHeaderStringLength))==(imHeaderVerify->ReceiptAddress()));
	if (!result3a)
		log->AppendError(_L8("\t\tReplyTos not identical"),-1);
	result &= result3a;
	TBool result4=((imHeader->ReplyTo().Left(KMaxImHeaderStringLength))==(imHeaderVerify->ReplyTo()));
	if (!result4)
		log->AppendError(_L8("\t\tReplyTos not identical"),-1);
	result &= result4;

	TBool result5 = ETrue;
	TInt ii;
	for ( ii=0; ii<imHeader->ToRecipients().Count(); ii++)
		{
		if (imHeader->ToRecipients().Count()==imHeaderVerify->ToRecipients().Count())
			{
			longValue=(imHeader->ToRecipients()[ii]);
			longValue.SetLength(KMaxImHeaderStringLength);
			result5&=((longValue)==(imHeaderVerify->ToRecipients()[ii].Left(KMaxImHeaderStringLength)));
			if (!result5)
				log->AppendError(_L8("\t\tTo Recipients not identical"),-1);
			}
		else
			result5&=EFalse;
		}
	result &= result5;
	TBool result6 = ETrue;
	for ( ii=0; ii<imHeader->CcRecipients().Count(); ii++)
		{
		if (imHeader->CcRecipients().Count()==imHeaderVerify->CcRecipients().Count())
			{
			longValue=(imHeader->CcRecipients()[ii]);
			longValue.SetLength(KMaxImHeaderStringLength);
			result6&=((longValue)==(imHeaderVerify->CcRecipients()[ii].Left(KMaxImHeaderStringLength)));
			if (!result6)
				log->AppendError(_L8("\t\tCc Recipients not identical"),-1);
			}
		else
			result6&=EFalse;
		}
	result &= result6;
	TBool result7 = ETrue;
	for ( ii=0; ii<imHeader->BccRecipients().Count(); ii++)
		{
		if (imHeader->BccRecipients().Count()==imHeaderVerify->BccRecipients().Count())
			{
			longValue=(imHeader->BccRecipients()[ii]);
			longValue.SetLength(KMaxImHeaderStringLength);
			result7&=((longValue)==(imHeaderVerify->BccRecipients()[ii].Left(KMaxImHeaderStringLength)));
			if (!result7)
				log->AppendError(_L8("\t\tBcc Recipients not identical"),-1);
			}
		else
			result7&=EFalse;
		}
	result &= result7;

	TBool result8=((imHeader->ResentMsgId().Left(KMaxImHeaderStringLength))==(imHeaderVerify->ResentMsgId()));
	if (!result8)
		log->AppendError(_L8("\t\tResent-MessageIDs not identical"),-1);
	result &= result8;
	TBool result9=((imHeader->ResentFrom().Left(KMaxImHeaderStringLength))==(imHeaderVerify->ResentFrom()));
	if (!result9)
		log->AppendError(_L8("\t\tResent-Froms not identical"),-1);
	result &= result9;
	TBool result10 = ETrue;
	for ( ii=0; ii<imHeader->ResentToRecipients().Count(); ii++)
		{
		if (imHeader->ResentToRecipients().Count()==imHeaderVerify->ResentToRecipients().Count())
			{
			longValue=(imHeader->ResentToRecipients()[ii]);
			longValue.SetLength(KMaxImHeaderStringLength);
			result10&=((longValue)==(imHeaderVerify->ResentToRecipients()[ii].Left(KMaxImHeaderStringLength)));
			if (!result10)
				log->AppendError(_L8("\t\tResent-To Recipients not identical"),-1);
			}
		else
			result10&=EFalse;
		}
	result &= result10;
	TBool result11 = ETrue;
	for ( ii=0; ii<imHeader->ResentCcRecipients().Count(); ii++)
		{
		if (imHeader->ResentCcRecipients().Count()==imHeaderVerify->ResentCcRecipients().Count())
			{
			longValue=(imHeader->ResentCcRecipients()[ii]);
			longValue.SetLength(KMaxImHeaderStringLength);
			result11&=((longValue)==(imHeaderVerify->ResentCcRecipients()[ii].Left(KMaxImHeaderStringLength)));
			if (!result11)
				log->AppendError(_L8("\t\tResent-Cc Recipients not identical"),-1);
			}
		else
			result11&=EFalse;
		}
	result &= result11;
	TBool result12 = ETrue;
	for ( ii=0; ii<imHeader->ResentBccRecipients().Count(); ii++)
		{
		if (imHeader->ResentBccRecipients().Count()==imHeaderVerify->ResentBccRecipients().Count())
			{
			longValue=(imHeader->ResentBccRecipients()[ii]);
			longValue.SetLength(KMaxImHeaderStringLength);
			result12&=((longValue)==(imHeaderVerify->ResentBccRecipients()[ii].Left(KMaxImHeaderStringLength)));
			if (!result12)
				log->AppendError(_L8("\t\tResent-Bcc Recipients not identical"),-1);
			}
		else
			result12&=EFalse;
		}
	result &= result12;

	TBool result13=((imHeader->BodyEncoding())==(imHeaderVerify->BodyEncoding()));
	if (!result13)
		log->AppendError(_L8("\t\tBody Encoding (charset) not identical"),-1);
	result &= result13;

	log->AppendComment(_L8("\tCompared Long Message Header"));
	ResultOfTest(result,aTest);

	return result;
	}

//


LOCAL_C TBool CompareEncodedMessageHeader(TInt aTest)
	{
	log->AppendComment(_L8("\tComparing Encoded Message Header"));
	TBool result = ETrue;

	TBool result1=((imHeader->Subject().Left(KMaxImHeaderStringLength))==(imHeaderVerify->Subject()));
	if (!result1)
		log->AppendError(_L8("\t\tSubjects not identical"),-1);
	result &= result1;

	TBool result3=((imHeader->From().Left(KMaxImHeaderStringLength))==(imHeaderVerify->From()));
	if (!result3)
		log->AppendError(_L8("\t\tFrom Fields not identical"),-1);
	result &= result3;

	TBool result4=((imHeader->ReplyTo().Left(KMaxImHeaderStringLength))==(imHeaderVerify->ReplyTo()));
	if (!result4)
		log->AppendError(_L8("\t\tReplyTos not identical"),-1);
	result &= result4;

	TBool result5 = ETrue;
	TInt ii;
	for ( ii=0; ii<imHeader->ToRecipients().Count(); ii++)
		{
		if (imHeader->ToRecipients().Count()==imHeaderVerify->ToRecipients().Count())
			{
			longValue=(imHeader->ToRecipients()[ii]);
			result5&=((longValue)==(imHeaderVerify->ToRecipients()[ii]));
			if (!result5)
				log->AppendError(_L8("\t\tTo Recipients not identical"),-1);
			}
		else
			result5&=EFalse;
		}
	result &= result5;
	TBool result6 = ETrue;
	for ( ii=0; ii<imHeader->CcRecipients().Count(); ii++)
		{
		if (imHeader->CcRecipients().Count()==imHeaderVerify->CcRecipients().Count())
			{
			longValue=(imHeader->CcRecipients()[ii]);
			result6&=((longValue)==(imHeaderVerify->CcRecipients()[ii]));
			if (!result6)
				log->AppendError(_L8("\t\tCc Recipients not identical"),-1);
			}
		else
			result6&=EFalse;
		}
	result &= result6;
	TBool result7 = ETrue;
	for ( ii=0; ii<imHeader->BccRecipients().Count(); ii++)
		{
		if (imHeader->BccRecipients().Count()==imHeaderVerify->BccRecipients().Count())
			{
			longValue=(imHeader->BccRecipients()[ii]);
			result7&=((longValue)==(imHeaderVerify->BccRecipients()[ii]));
			if (!result7)
				log->AppendError(_L8("\t\tBcc Recipients not identical"),-1);
			}
		else
			result7&=EFalse;
		}
	result &= result7;

	
	TBool result13=((imHeader->BodyEncoding())==(imHeaderVerify->BodyEncoding()));
	if (!result13)
		log->AppendError(_L8("\t\tBody Encoding (charset) not identical"),-1);
	result &= result13;

	log->AppendComment(_L8("\tCompared Encoded Message Header"));
	ResultOfTest(result, aTest);

	return result;
	}



LOCAL_C TBool CompareEncodedMessageHeaderExpectDifferent(TInt aTest)
	{
	log->AppendComment(_L8("\tComparing Encoded Message Header"));
	TBool result = ETrue;

	TBool result1=((imHeader->Subject().Left(KMaxImHeaderStringLength))==(imHeaderVerify->Subject()));
	result &= result1;

	TBool result3=((imHeader->From().Left(KMaxImHeaderStringLength))==(imHeaderVerify->From()));
	result &= result3;

	TBool result4=((imHeader->ReplyTo().Left(KMaxImHeaderStringLength))==(imHeaderVerify->ReplyTo()));
	result &= result4;

	TBool result5 = ETrue;
	TInt ii;
	for ( ii=0; ii<imHeader->ToRecipients().Count(); ii++)
		{
		if (imHeader->ToRecipients().Count()==imHeaderVerify->ToRecipients().Count())
			{
			longValue=(imHeader->ToRecipients()[ii]);
			result5&=((longValue)==(imHeaderVerify->ToRecipients()[ii]));
			}
		else
			result5&=EFalse;
		}
	result &= result5;
	TBool result6 = ETrue;
	for ( ii=0; ii<imHeader->CcRecipients().Count(); ii++)
		{
		if (imHeader->CcRecipients().Count()==imHeaderVerify->CcRecipients().Count())
			{
			longValue=(imHeader->CcRecipients()[ii]);
			result6&=((longValue)==(imHeaderVerify->CcRecipients()[ii]));
			}
		else
			result6&=EFalse;
		}
	result &= result6;
	TBool result7 = ETrue;
	for ( ii=0; ii<imHeader->BccRecipients().Count(); ii++)
		{
		if (imHeader->BccRecipients().Count()==imHeaderVerify->BccRecipients().Count())
			{
			longValue=(imHeader->BccRecipients()[ii]);
			result7&=((longValue)==(imHeaderVerify->BccRecipients()[ii]));
			}
		else
			result7&=EFalse;
		}
	result &= result7;

	
	TBool result13=((imHeader->BodyEncoding())==(imHeaderVerify->BodyEncoding()));
	result &= result13;

	log->AppendComment(_L8("\tCompared Encoded Message Header Different"));
	result = !result;
	ResultOfTest(result, aTest);

	return result;
	}


//

LOCAL_C TBool FilledMessageHeader(TInt aTest)
	{
	// Fill in the fields in the header
	log->AppendComment(_L8("\tFilled Message Header Test (Test 1)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;

	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	CreateMessageHeader();
	StoreMessageHeaderL(imHeader);
	RestoreMessageHeaderL(imHeaderVerify);
	TBool result=CompareFilledMessageHeader(aTest);
	CleanupStack::PopAndDestroy(2); //imHeader, imHeaderVerify
	return result;
	}

LOCAL_C TBool BlankMessageHeader(TInt aTest)
	{
	// the Headers contain blank fields
	log->AppendComment(_L8("\tBlank Message Header Test (Test 2)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	StoreMessageHeaderL(imHeader);
	RestoreMessageHeaderL(imHeaderVerify);
	TBool result=CompareBlankMessageHeader(aTest);
	CleanupStack::PopAndDestroy(2); //imHeader, imHeaderVerify
	return result;
	}

LOCAL_C TBool LongMessageHeader(TInt aTest)
	{
	log->AppendComment(_L8("\tLong Message Header Test (Test 3)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	// the Headers contain fields that are longer than 1000 chars
	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	CreateLongMessageHeader();
	StoreMessageHeaderL(imHeader);
	RestoreMessageHeaderL(imHeaderVerify);
	TBool result=CompareLongMessageHeader(aTest);
	CleanupStack::PopAndDestroy(2); //imHeader, imHeaderVerify
	return result;
	}


LOCAL_C TBool MessageHeaderReDecodeHeadersOOMTest(TInt aTest)
	{
	log->AppendComment(_L8("\tEncoded Message Header Test (Test 13)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	
	RFs& fileSvrSession = testUtils->iMsvEntry->Session().FileSession();
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
	CleanupStack::PushL(charConv);
	CImConvertHeader* headerConverter = CImConvertHeader::NewL(*charConv); 
	CleanupStack::PushL(headerConverter);

	// Create the correct result in imHeaderVerify
	CreateEncodedMessageHeader(EFalse, imHeaderVerify);
	headerConverter->SetMessageType(ETrue);
	headerConverter->DecodeAllHeaderFieldsL(*imHeaderVerify);

	// Memory failure loop.
	TBool result = ETrue;
	TBool finished = EFalse;
	TInt  theFailCount=0;
	while (!finished)
		{
		// Create the correct result in imHeader
		__UHEAP_MARK;
		imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
		CreateEncodedMessageHeader(EFalse, imHeader);
		headerConverter->SetMessageType(ETrue);
		headerConverter->DecodeAllHeaderFieldsL(*imHeader);
		imHeader->SetOverrideCharset(0x10000fbe);

		__UHEAP_FAILNEXT(theFailCount);
		theFailCount++;
		TRAPD(err, imHeader->ReDecodeL(fileSvrSession));
		__UHEAP_RESET;

		if (err == KErrNone)
			{
			TBool compareResult = CompareEncodedMessageHeader(aTest);
			if (!compareResult)
				log->AppendError(_L8("\t\t\tComparing different out-of-memory redecode tests"), -1);
			result &= compareResult;
			finished = ETrue;
			}
		else
			{
			if (err != KErrNoMemory)
				{
				log->AppendError(_L8("\t\t\tOut-of-memory redecode tests resulted in leave other than KErrNoMemory"), -1);
				result &= EFalse;
				finished = ETrue;
				}
			}
		CleanupStack::PopAndDestroy(imHeader);
		__UHEAP_MARKEND;
		}
	CleanupStack::PopAndDestroy(4, imHeaderVerify);
	return result;
	}


LOCAL_C TBool MessageHeaderDecodeAllHeadersOOMTest(TInt aTest)
	{
	log->AppendComment(_L8("\tEncoded Message Header Test (Test 12)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	
	RFs& fileSvrSession = testUtils->iMsvEntry->Session().FileSession();
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
	CleanupStack::PushL(charConv);
	CImConvertHeader* headerConverter = CImConvertHeader::NewL(*charConv); 
	CleanupStack::PushL(headerConverter);

	// Create the correct result in imHeaderVerify
	CreateEncodedMessageHeader(EFalse, imHeaderVerify);
	headerConverter->SetMessageType(ETrue);
	headerConverter->DecodeAllHeaderFieldsL(*imHeaderVerify);

	// Memory failure loop.
	TBool result = ETrue;
	TBool finished = EFalse;
	TInt  theFailCount=0;
	while (!finished)
		{
		// Create the correct result in imHeader
		__UHEAP_MARK;
		imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
		CreateEncodedMessageHeader(EFalse, imHeader);
		headerConverter->SetMessageType(ETrue);

		__UHEAP_FAILNEXT(theFailCount);
		theFailCount++;
		TRAPD(err, headerConverter->DecodeAllHeaderFieldsL(*imHeader));
		__UHEAP_RESET;

		if (err == KErrNone)
			{
			TBool compareResult = CompareEncodedMessageHeader(aTest);
			if (!compareResult)
				log->AppendError(_L8("\t\t\tComparing different out-of-memory tests"), -1);
			result &= compareResult;
			finished = ETrue;
			}
		else
			{
			if (err != KErrNoMemory)
				{
				log->AppendError(_L8("\t\t\tOut-of-memory tests resulted in leave other than KErrNoMemory"), -1);
				result &= EFalse;
				finished = ETrue;
				}
			}
		CleanupStack::PopAndDestroy(imHeader);
		__UHEAP_MARKEND;
		}
	CleanupStack::PopAndDestroy(4, imHeaderVerify);
	return result;
	}

LOCAL_C TBool EncodedMessageHeaderDecode(TInt aTest)
	{
	log->AppendComment(_L8("\tEncoded Message Header Test (Test 11)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;

	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	
	RFs& fileSvrSession = testUtils->iMsvEntry->Session().FileSession();
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
	CleanupStack::PushL(charConv);
	CImConvertHeader* headerConverter = CImConvertHeader::NewL(*charConv); 
	CleanupStack::PushL(headerConverter);

	// Create the correct result in imHeaderVerify
	CreateEncodedMessageHeader(EFalse, imHeaderVerify);
	// Mime test verify header.
	headerConverter->SetMessageType(ETrue);
	headerConverter->DecodeAllHeaderFieldsL(*imHeaderVerify);
	

// Create the correct result in imHeader
	CreateEncodedMessageHeader(EFalse, imHeader);
	// Mime test verify header.
	headerConverter->SetMessageType(ETrue);
	headerConverter->DecodeAllHeaderFieldsL(*imHeader);
	StoreMessageHeaderL(imHeader);
	imHeader->Reset();
	RestoreMessageHeaderL(imHeader);
	_LIT(KDisappearMessage, "This should disappear.");

	imHeader->SetSubjectL(KDisappearMessage);
	TBool result = CompareEncodedMessageHeaderExpectDifferent(aTest);
	if (!result)
		log->AppendError(_L8("\t\t\tComparing different after SetSubject failed"), -1);

	// Perform normal decoding initially then redecode to test copy encoded data in redecode
	// step doesn't overwrite real encoded data with disappear message.
	headerConverter->SetMessageType(ETrue);
	headerConverter->DecodeAllHeaderFieldsL(*imHeader);
	imHeader->ReDecodeL(fileSvrSession);
	TBool result2 = CompareEncodedMessageHeader(aTest);
	if (!result2)
		log->AppendError(_L8("\t\t\tComparing after DecodeAllHeaderFields and ReDecode failed"), -1);
	result &= result2;

	CleanupStack::PopAndDestroy(5, imHeader);
	return result;
	}

LOCAL_C TBool EncodedNonMimeMessageHeader(TInt)
	{
	log->AppendComment(_L8("\tEncoded Message Header Test (Test 10)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;

	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	
	RFs& fileSvrSession = testUtils->iMsvEntry->Session().FileSession();
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
	CleanupStack::PushL(charConv);
	CImConvertHeader* headerConverter = CImConvertHeader::NewL(*charConv); 
	CleanupStack::PushL(headerConverter);

	// Create the correct result in imHeaderVerify
	CreateGB2312EncodedNonMimeMessageHeader(imHeaderVerify);
	// Non-Mime test verify header, this will decode with system default
	// as this isn't GB2312 the verify header is incorrectly decoded
	headerConverter->DecodeAllHeaderFieldsL(*imHeaderVerify);
	
	TBool result = ETrue;
	if (CheckGB2312Descriptor(imHeaderVerify->Subject()) != EFalse || 
		CheckGB2312Descriptor(imHeaderVerify->ToRecipients()[0]) != EFalse)
		{
		// The imVerifyHeader was decoded with the system default charset.
		// It should not be the same as the properly decoded data.
		result = EFalse;
		}
	if (!result)
		log->AppendError(_L8("\t\t\tBad header decoded correctly with system charset"),-1);


	// Create a header with bad data.
	// Purposely use the incorrect charset data for a mime header.
	CreateGB2312EncodedNonMimeMessageHeader(imHeader);
	// imHeader should contain incorrect data.
	headerConverter->DecodeAllHeaderFieldsL(*imHeader);
	// Save bad header to CMsvStore,
	StoreMessageHeaderL(imHeader);

	CMsvStore* fStore=testUtils->iMsvEntry->ReadStoreL();
	CleanupStack::PushL(fStore);
	const TUid KUidMsgFileIMailEncodedHeader = {0x101FD0E3};
	TBool result1=fStore->IsPresentL(KUidMsgFileIMailEncodedHeader);
	CleanupStack::PopAndDestroy(); //fStore
	if (!result1)
		log->AppendError(_L8("\t\t\tEncoded Header stream missing"),-1);
	result &= result1;

	// Clear header and restore from the CMsvStore with the bad data.
	imHeader->Reset();
	RestoreMessageHeaderL(imHeader);

	TBool result2 = ETrue;
	if (CheckGB2312Descriptor(imHeader->Subject()) != EFalse || 
		CheckGB2312Descriptor(imHeader->ToRecipients()[0]) != EFalse)
		{
		// The imHeader was decoded with the system default charset.
		// It should not be the same as the properly decoded data.
		result2 = EFalse;
		}
	if (!result2)
		log->AppendError(_L8("\t\t\tBad header restored correctly"),-1);
	result &= result2;
		
	imHeader->SetOverrideCharset(0x10000fbe);
	TBool result3 = (imHeader->OverrideCharset() == 0x10000fbe);
	if (!result3)
		log->AppendError(_L8("\t\t\tGet / Set Overridecharset failed"),-1);
	result &=result3;

	// Redecode imHeader with the correct charset set as the override charset
	// and compare with initial result.
	imHeader->ReDecodeL(fileSvrSession);
	TBool result4 = EFalse;
	if (CheckGB2312Descriptor(imHeader->Subject()) != EFalse || 
		CheckGB2312Descriptor(imHeader->ToRecipients()[0]) != EFalse)
		{
		// The imHeader was decoded with the override charset
		// and matches the reference data.
		result4 = ETrue;
		}
	if (!result4)
		log->AppendError(_L8("\t\t\tRedecode with correct charset failed"),-1);
	result &= result4;
	
	CleanupStack::PopAndDestroy(5, imHeader);
	return result;
	}

LOCAL_C TBool EncodedMessageHeader(TInt aTest)
	{
	log->AppendComment(_L8("\tEncoded Message Header Test (Test 9)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;

	// the Headers contain fields that are longer than 1000 chars

	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	
	RFs& fileSvrSession = testUtils->iMsvEntry->Session().FileSession();
	CCnvCharacterSetConverter* characterConverter = CCnvCharacterSetConverter::NewL();
	CleanupStack::PushL(characterConverter);
	CImConvertCharconv* charConv = CImConvertCharconv::NewL(*characterConverter, fileSvrSession);
	CleanupStack::PushL(charConv);
	CImConvertHeader* headerConverter = CImConvertHeader::NewL(*charConv); 
	CleanupStack::PushL(headerConverter);

	// Create the correct result in imHeaderVerify
	CreateEncodedMessageHeader(EFalse, imHeaderVerify);
	// Mime test verify header.
	headerConverter->SetMessageType(ETrue);
	headerConverter->DecodeAllHeaderFieldsL(*imHeaderVerify);


	// Create a header with bad data.
	// Purposely use the incorrect charset data for a mime header.
	CreateEncodedMessageHeader(ETrue, imHeader);
	headerConverter->SetMessageType(ETrue);
	// imHeader should contain incorrect data.
	headerConverter->DecodeAllHeaderFieldsL(*imHeader);
	// Save bad header to CMsvStore,
	StoreMessageHeaderL(imHeader);

	// for Chris
	// Clear header and restore from the CMsvStore with the bad data.
	imHeader->Reset();
	RestoreMessageHeaderL(imHeader);

	TBool result = CompareEncodedMessageHeaderExpectDifferent(aTest);
	if (!result)
		log->AppendError(_L8("\t\t\tRestore failed comparision"),-1);
		
	// set override charset and store the header
	imHeader->SetOverrideCharset(0x10000fbe);
	StoreMessageHeaderL(imHeader);
	
	// reset imHeader and check the override charset is also reset
	imHeader->Reset();
	imHeader->OverrideCharset();
	TBool result1 = (imHeader->OverrideCharset() == 0);
	if (!result1)
		log->AppendError(_L8("\t\t\tOveride charset not zero after reset"),-1);
	result &=result1;

	// should have restored the bad data, and the override charset
	RestoreMessageHeaderL(imHeader);
	TBool result2 = (imHeader->OverrideCharset() == 0x10000fbe);
	if (!result2)
		log->AppendError(_L8("\t\t\tGet / Set Overridecharset failed"),-1);
	result &=result2;

	// Redecode imHeader with the correct charset set as the override charset
	// and compare with initial result.
	imHeader->ReDecodeL(fileSvrSession);
	TBool result3 = CompareEncodedMessageHeader(aTest);
	if (!result3)
		log->AppendError(_L8("\t\t\tRedecode failed"),-1);
	result &=result3;

	// Test below is decode imHeader with correct charset.
	// Override with purposely bad charset.
	// Set override charset back to zero.
	// Check it is back to its original state (i.e. imHeaderVerify)
	imHeader->Reset();
	CreateEncodedMessageHeader(EFalse, imHeader);
	headerConverter->SetMessageType(ETrue);
	// imHeader should contain correct data.
	headerConverter->DecodeAllHeaderFieldsL(*imHeader);
	// Should now be the same as the imHeaderVerify object.
	TBool result4 = CompareEncodedMessageHeader(aTest);
	if (!result4)
		log->AppendError(_L8("\t\t\tCreated header data does not match verify header."),-1);
	result &=result4;

	// Set override charset to a purposely incorrect charset.
	imHeader->SetOverrideCharset(0x10003b10);
	imHeader->ReDecodeL(fileSvrSession);
	TBool result5 = CompareEncodedMessageHeaderExpectDifferent(aTest);
	if (!result5)
		log->AppendError(_L8("\t\t\tReDecoding with bad charset still matches verify header with good charset."),-1);
	result &= result5;

	// Set override charset to be not used. I.e. zero
	imHeader->SetOverrideCharset(0);
	imHeader->ReDecodeL(fileSvrSession);

	TBool result6 = CompareEncodedMessageHeader(aTest);
	if (!result6)
		log->AppendError(_L8("\t\t\tRedecoded header with zeroed override charset does not match verify header."),-1);
	result &=result6;

	// Save bad header to CMsvStore,
	StoreMessageHeaderL(imHeader);

	CleanupStack::PopAndDestroy(5, imHeader);
	return result;
	}

//checks for duplicate recipients in the to and cc list.
LOCAL_C TBool CheckForDuplicateRecipients(CImHeader* aCImHeader)
	{
	//check for duplicates in To list 
	TInt toCount = aCImHeader->ToRecipients().Count();
	for(TInt toCounter=0; toCounter<toCount; ++toCounter)
		{
		for(TInt jj=toCounter+1; jj<toCount; ++jj)
			{
			if((aCImHeader->ToRecipients()[jj].FindF(aCImHeader->ToRecipients()[toCounter])) >= 0)
				{
				return ETrue;
				}
			}
		}
 
 	//check for duplicates in Cc list
	TInt ccCount = aCImHeader->CcRecipients().Count();
	for(TInt ccCounter=0; ccCounter<ccCount; ++ccCounter)
		{
		for(TInt jj=ccCounter+1; jj<ccCount; ++jj)
			{
			if((aCImHeader->CcRecipients()[jj].FindF(aCImHeader->CcRecipients()[ccCounter])) >= 0)
				{
				return ETrue;
				}
			}
		}

	//check for duplicates across To and Cc lists
	for(TInt tcCounter=0; tcCounter<ccCount; ++tcCounter)
		{
		for(TInt jj=0; jj<toCount; ++jj)
			{
			if((aCImHeader->ToRecipients()[jj].FindF(aCImHeader->CcRecipients()[tcCounter])) >= 0)
				{
				return ETrue;
				}
			}
		}

	return EFalse;
	}

LOCAL_C TBool ReplyHeader1(TInt aTest)
	{
	// Fill in the fields in the header
	log->AppendComment(_L8("\tReply Header Test 1 (Test 4)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeader2 = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection

	log->AppendComment(_L8("\t\tReply To Originator"));
	CreateMessageHeader();
	TBuf<10> buf = _L("Re: %S");
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::EOriginator, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result = ETrue;

	TBool result1=(imHeader2->ToRecipients()[0] == imHeader->ReplyTo());
	if (!result1)
		log->AppendError(_L8("\t\t\tTo Recipient not identical to From"),-1);
	result &= result1;
	TBool result2=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result2)
		log->AppendError(_L8("\t\t\tSubjects not identical"),-1);
	result &= result2;
	TBool result3=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result3)
		log->AppendError(_L8("\t\t\tInReplyTo field not identical to MsgId"),-1);
	result &= result3;

	log->AppendComment(_L8("\t\tReply To Sender"));
	CreateMessageHeader();
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::ESender, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result4=(imHeader2->ToRecipients()[0] == imHeader->ReplyTo());
	if (!result4)
		log->AppendError(_L8("\t\t\tTo Recipient not identical to From"),-1);
	result &= result4;
	TBool result5=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result5)
		log->AppendError(_L8("\t\t\tSubjects not identical"),-1);
	result &= result5;
	TBool result6=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result6)
		log->AppendError(_L8("\t\t\tInReplyTo field not identical to MsgId"),-1);
	result &= result6;

	log->AppendComment(_L8("\t\tReply To All"));
	CreateMessageHeader();
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::EAll, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result7=(imHeader2->ToRecipients()[0] == imHeader->ReplyTo());
	if (!result7)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to From"),-1);
	result &= result7;

	TBool result8=(imHeader2->CcRecipients()[0] == imHeader->ToRecipients()[0]);
	result8 &= (imHeader2->CcRecipients()[1] == imHeader->ToRecipients()[1]);
	if (!result8)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to To Recipients"),-1);
	result &= result8;

	TBool result9=(imHeader2->CcRecipients()[2] == imHeader->CcRecipients()[0]);
	result9 &= (imHeader2->CcRecipients()[3] == imHeader->CcRecipients()[1]);
	if (!result9)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to Cc Recipients"),-1);
	result &= result9;
	TBool result10=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result10)
		log->AppendError(_L8("\t\t\tSubject fields not identical"),-1);
	result &= result10;
	TBool result11=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result11)
		log->AppendError(_L8("\t\t\tInReplyTo field not identical to MsgId"),-1);
	result &= result11;

//

	log->AppendComment(_L8("\t\tReply To Recipients"));
	CreateMessageHeader();
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::ERecipients, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result12=(imHeader2->ToRecipients()[0] == imHeader->ToRecipients()[0]);
	result12 &= (imHeader2->ToRecipients()[1] == imHeader->ToRecipients()[1]);
	if (!result12)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to To Recipients"),-1);
	result &= result12;
	TBool result13=(imHeader2->CcRecipients()[0] == imHeader->CcRecipients()[0]);
	result13 &= (imHeader2->CcRecipients()[1] == imHeader->CcRecipients()[1]);
	if (!result13)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to Cc Recipients"),-1);
	result &= result13;
	TBool result14=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result14)
		log->AppendError(_L8("\t\t\tSubject fields not identical"),-1);
	result &= result14;
	TBool result15=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result15)
		log->AppendError(_L8("\t\t\tInReplyTo field not identical to MsgId"),-1);
	result &= result15;
	
	
	log->AppendComment(_L8("\t\tReply To All - Duplicate To Recipients in Cc list"));
	CreateMessageHeader4();
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::EAll, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result16=(imHeader2->ToRecipients().Count() == 1);
	result16 &= (imHeader2->CcRecipients().Count() == 3);
	result16 &= (CheckForDuplicateRecipients(imHeader2) == EFalse);
	if (!result16)
		log->AppendError(_L8("\t\t\tDuplicate Recipients in To, Cc lists"),-1);
	result &= result16;


	log->AppendComment(_L8("\t\tReply To All - Duplicate Recipients in To and Cc list"));
	CreateMessageHeader4();
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::ERecipients, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result17=(imHeader2->ToRecipients().Count() == 2);
	result17 &= (imHeader2->CcRecipients().Count() == 2);
	result17 &= (CheckForDuplicateRecipients(imHeader2) == EFalse);
	if (!result17)
		log->AppendError(_L8("\t\t\tDuplicate Recipients in To, Cc list"),-1);
	result &= result17;


	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(3); //imHeader, imHeaderVerify, imHeader
	log->AppendComment(_L8("Reply Header Test 1 completed"));
	return result;
	}

LOCAL_C TBool ReplyHeader2(TInt aTest)
	{
	// Fill in the fields in the header
	log->AppendComment(_L8("\tReply Header Test 2 (Test 5)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeader2 = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection

	log->AppendComment(_L8("\t\tReply To Originator"));
	CreateMessageHeader2();
	TBuf<10> buf = _L("Re: %S");
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::EOriginator, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result = ETrue;

	TBool result1=(imHeader2->ToRecipients()[0] == imHeader->ReplyTo());
	if (!result1)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to ReplyTo"),-1);
	result &= result1;
	TBool result2=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result2)
		log->AppendError(_L8("\t\t\tSubjects not identical"),-1);
	result &= result2;
	TBool result3=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result3)
		log->AppendError(_L8("\t\t\tInReplyTo field not identical to MsgId"),-1);
	result &= result3;

	log->AppendComment(_L8("\t\tReply To Sender"));
	CreateMessageHeader2();
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::ESender, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result4=(imHeader2->ToRecipients()[0] == imHeader->ResentFrom());
	if (!result4)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to ResentFrom"),-1);
	result &= result4;
	TBool result4a=(imHeader2->ToRecipients()[0] == imHeader->ResentFrom());
	if (!result4a)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to ResentFrom"),-1);
	result &= result4a;
	TBool result5=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result5)
		log->AppendError(_L8("\t\t\tSubjects not identical"),-1);
	result &= result5;
	TBool result6=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result6)
		log->AppendError(_L8("\t\t\tInReplyTo not identical to MsgId"),-1);
	result &= result6;

	log->AppendComment(_L8("\t\tReply To All"));
	CreateMessageHeader2();
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::EAll, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result7=(imHeader2->ToRecipients()[0] == imHeader->From());
	if (!result7)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to From"),-1);
	result &= result7;
	TBool result7a=(imHeader2->ToRecipients()[1] == imHeader->ResentFrom());
	if (!result7a)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to Resent-From"),-1);
	result &= result7a;
	TBool result8=(imHeader2->ToRecipients()[2] == imHeader->ToRecipients()[0]);
	result8 &= (imHeader2->ToRecipients()[3] == imHeader->ToRecipients()[1]);
	if (!result8)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to To Recipients"),-1);
	result &= result8;
	TBool result9=(imHeader2->CcRecipients()[0] == imHeader->CcRecipients()[0]);
	result9 &= (imHeader2->CcRecipients()[1] == imHeader->CcRecipients()[1]);
	if (!result9)
		log->AppendError(_L8("\t\t\tCc Recipients not identical to Cc Recipients"),-1);
	result &= result9;
	TBool result10=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result10)
		log->AppendError(_L8("\t\t\tSubjects not identical"),-1);
	result &= result10;
	TBool result11=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result11)
		log->AppendError(_L8("\t\t\tInReplyTo not identical to MsgId"),-1);
	result &= result11;

	log->AppendComment(_L8("\t\tReply To Recipients"));
	CreateMessageHeader2();
	imHeader->CreateReplyL(*imHeaderVerify, CImHeader::ERecipients, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result12=(imHeader2->ToRecipients()[0] == imHeader->ToRecipients()[0]);
	result12 &= (imHeader2->ToRecipients()[1] == imHeader->ToRecipients()[1]);
	if (!result12)
		log->AppendError(_L8("\t\t\tTo Recipients not identical to To Recipients"),-1);
	result &= result12;
	TBool result13=(imHeader2->CcRecipients()[0] == imHeader->CcRecipients()[0]);
	result13 &= (imHeader2->CcRecipients()[1] == imHeader->CcRecipients()[1]);
	if (!result13)
		log->AppendError(_L8("\t\t\tCc Recipients not identical to Cc Recipients"),-1);
	result &= result13;
	TBool result14=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result14)
		log->AppendError(_L8("\t\t\tSubjects not identical"),-1);
	result &= result14;
	TBool result15=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result15)
		log->AppendError(_L8("\t\t\tInReplyTo not identical to MsgId"),-1);
	result &= result15;

	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(3); //imHeader, imHeaderVerify, imHeader
	return result;
	}

LOCAL_C TBool ReceiptHeader1(TInt aTest)
	{
	// Fill in the fields in the header
	log->AppendComment(_L8("\tReceipt Header Test (Test 6)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeader2 = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection

	CreateMessageHeader();
	TBuf<10> buf = _L("Re: %S");
	imHeader->CreateReceiptL(*imHeaderVerify, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result = ETrue;
	TBool result1=(imHeader2->ToRecipients()[0] == imHeader->ReceiptAddress());
	if (!result1)
		log->AppendError(_L8("\t\tTo Recipients not identical to Recipient Address"),-1);
	result &= result1;
	TBool result2=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result2)
		log->AppendError(_L8("\t\tInReplyTo not identical to MsgId"),-1);
	result &= result2;
	TBool result3=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result3)
		log->AppendError(_L8("\t\tSubjects not identical"),-1);
	result &= result3;
	
	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(3); //imHeader, imHeaderVerify, imHeader
	return result;
	}

LOCAL_C TBool ReceiptHeader2(TInt aTest)
	{
	// Fill in the fields in the header
	log->AppendComment(_L8("\tReceipt Header Test (Test 7)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeader2 = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection

	CreateMessageHeader3();
	TBuf<10> buf = _L("Re: %S");
	imHeader->CreateReceiptL(*imHeaderVerify, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result = ETrue;
	TBool result1=(imHeader2->ToRecipients()[0] == imHeader->From());
	if (!result1)
		log->AppendError(_L8("\t\tTo Recipients not identical to Recipient Address"),-1);
	result &= result1;
	TBool result2=(imHeader2->InReplyTo() == imHeader->ImMsgId());
	if (!result2)
		log->AppendError(_L8("\t\tInReplyTo not identical to MsgId"),-1);
	result &= result2;
	TBool result3=(imHeader2->Subject() == (_L("Re: short mail about something interesting")));
	if (!result3)
		log->AppendError(_L8("\t\tSubjects not identical"),-1);
	result &= result3;
	
	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(3); //imHeader, imHeaderVerify, imHeader
	return result;
	}

LOCAL_C TBool ForwardHeader(TInt aTest)
	{
	// Fill in the fields in the header
	log->AppendComment(_L8("\tForward Header Test (Test 8)"));
	entry.iType = KUidMsvMessageEntry;
	entry.iMtm = KUidMsgTypeIMAP4;
	entry.iServiceId = KMsvLocalServiceIndexEntryId;
	imHeader = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeader2 = CImHeader::NewLC(); // Cleaned up when exiting this menu selection
	imHeaderVerify = CImHeader::NewLC(); // Cleaned up when exiting this menu selection

	CreateMessageHeader();
	TBuf<10> buf = _L("Fwd: %S");
	imHeader->CreateForwardL(*imHeaderVerify, buf);
	StoreMessageHeaderL(imHeaderVerify);
	RestoreMessageHeaderL(imHeader2);

	TBool result = ETrue;
	result &= (imHeader2->Subject() == (_L("Fwd: short mail about something interesting")));
	if (!result)
		log->AppendError(_L8("\t\tSubjects not identical"),-1);
	
	ResultOfTest(result,aTest);
	CleanupStack::PopAndDestroy(3); //imHeader, imHeaderVerify, imHeader
	return result;
	}



#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
LOCAL_C void CreateAccount()
	{
	log->AppendComment(_L8("\tCreating email accounts..."));
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	CImImap4Settings* imapSetting = new (ELeave) CImImap4Settings;
	CImIAPPreferences* iap = CImIAPPreferences::NewLC();

	_LIT(KIMAPAccountName, "ImapAccount");
	accounts->PopulateDefaultImapSettingsL(*imapSetting, *iap);
	TImapAccount imapAccount = accounts->CreateImapAccountL(KIMAPAccountName, *imapSetting, *iap, EFalse);
	CleanupStack::PopAndDestroy(2);
	delete imapSetting;
	log->AppendComment(_L8("\tAccount Created succesfully."));
	}
	
#endif


//

LOCAL_C void ImHeaderSelectionL()
	{
	TBool result = ETrue;

#if (defined SYMBIAN_MESSAGESTORE_HEADER_BODY_USING_SQLDB)
	CreateAccount();
#endif
	
	testUtils->TestStart(1);
	TBool result1=FilledMessageHeader(1);
	if (result1)
		log->AppendComment(_L8("PASSED Test 1"));
	else
		log->AppendError(_L8("FAILED Test 1"),-1);
	result &= result1;
	testUtils->TestFinish(1);

	testUtils->TestStart(2);
	TBool result2=BlankMessageHeader(2);
	if (result2)
		log->AppendComment(_L8("PASSED Test 2"));
	else
		log->AppendError(_L8("FAILED Test 2"),-1);
	result &= result2;
	testUtils->TestFinish(2);
	
	testUtils->TestStart(3);
	TBool result3=LongMessageHeader(3);
	if (result3)
		log->AppendComment(_L8("PASSED Test 3"));
	else
		log->AppendError(_L8("FAILED Test 3"),-1);
	result &= result3;
	testUtils->TestFinish(3);
	
	testUtils->TestStart(4);
	TBool result4=ReplyHeader1(4);
	if (result4)
		log->AppendComment(_L8("PASSED Test 4"));
	else
		log->AppendError(_L8("FAILED Test 4"),-1);
	result &= result4;
	testUtils->TestFinish(4);
	
	testUtils->TestStart(5);
	TBool result5=ReplyHeader2(5);
	if (result5)
		log->AppendComment(_L8("PASSED Test 5"));
	else
		log->AppendError(_L8("FAILED Test 5"),-1);
	result &= result5;
	testUtils->TestFinish(5);
	
	testUtils->TestStart(6);
	TBool result6=ReceiptHeader1(6);
	if (result6)
		log->AppendComment(_L8("PASSED Test 6"));
	else
		log->AppendError(_L8("FAILED Test 6"),-1);
	result &= result6;
	testUtils->TestFinish(6);
	
	testUtils->TestStart(7);
	TBool result7=ReceiptHeader2(7);
	if (result7)
		log->AppendComment(_L8("PASSED Test 7"));
	else
		log->AppendError(_L8("FAILED Test 7"),-1);
	result &= result7;
	testUtils->TestFinish(7);

	testUtils->TestStart(8);
	TBool result8=ForwardHeader(8);
	if (result8)
		log->AppendComment(_L8("PASSED Test 8"));
	else
		log->AppendError(_L8("FAILED Test 8"),-1);
	result &= result8;
	testUtils->TestFinish(8);

	
	testUtils->TestStart(9);
	TBool result9 = EncodedMessageHeader(9);
	if (result9)
		log->AppendComment(_L8("PASSED Test 9"));
	else
		log->AppendError(_L8("FAILED Test 9"),-1);
	result &= result9;
	testUtils->TestFinish(9);

	
	testUtils->TestStart(10);
	TBool result10 = EncodedNonMimeMessageHeader(10);
	if (result10)
		log->AppendComment(_L8("PASSED Test 10"));
	else
		log->AppendError(_L8("FAILED Test 10"),-1);
	result &= result10;
	testUtils->TestFinish(10);

	
	testUtils->TestStart(11);
	TBool result11 = EncodedMessageHeaderDecode(11);
	if (result11)
		log->AppendComment(_L8("PASSED Test 11"));
	else
		log->AppendError(_L8("FAILED Test 11"),-1);
	result &= result11;
	testUtils->TestFinish(11);

	
	testUtils->TestStart(12);
	TBool result12 = MessageHeaderDecodeAllHeadersOOMTest(12);
	if (result12)
		log->AppendComment(_L8("PASSED Test 12"));
	else
		log->AppendError(_L8("FAILED Test 12"),-1);
	result &= result12;
	testUtils->TestFinish(12);

	testUtils->TestStart(13);
	TBool result13 = MessageHeaderReDecodeHeadersOOMTest(13);
	if (result13)
		log->AppendComment(_L8("PASSED Test 13"));
	else
		log->AppendError(_L8("FAILED Test 13"),-1);
	result &= result13;
	testUtils->TestFinish(13);

	if (result)
		{
		log->AppendComment(_L8("PASSED All tests"));
		test.Printf(_L("PASSED all Tests"));
		testUtils->TestHarnessCompleted();
		}
	else
		{
		test.Printf(_L("FAILED  Tests"));
		log->AppendError(_L8("FAILED At Least one test"),-1);
		testUtils->TestHarnessFailed(KErrGeneral);
		}
	}

//

LOCAL_C void InitL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(scheduler);
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test);
	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();

	log = CImLog::NewL(_L("c:\\logs\\email\\T_IMCM.log"), EAppend);
	CleanupStack::PushL(log);
	log->AppendComment(_L8("********* T_MIUT01 Test CImHeader Class **********"));
	TBuf8<80> buf;

#if defined(__WINS__)	
	buf.Append(_L8("WINS "));
#else
	buf.Append(_L8("MARM "));
#endif
#if defined(_UNICODE)
	buf.Append(_L8("U"));
#endif
#if defined(_DEBUG)
	buf.Append(_L8("DEB"));
#else
	buf.Append(_L8("REL"));
#endif
	log->AppendComment(buf);
	}
	
LOCAL_C void Closedown()
	{
	log->AppendComment(_L8("**********    T_MIUT01 Tests Complete    **********"));
	log->AppendComment(_L8(""));

	CleanupStack::PopAndDestroy(3);  //testUtils, log, scheduler
	}

//

LOCAL_C void doMainL()
	{
	InitL();
	testUtils->GoClientSideL();
	test.Printf(_L("Performing Header Tests\n"));

	ImHeaderSelectionL();

	Closedown();
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_MIUT01 Test CImHeader class"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}
