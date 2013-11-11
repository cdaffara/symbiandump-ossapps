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
// T_IMCV_Transform_Send
// Component: IMUT
// Owner: Ibrahim Rahman
// Brief description of test harness:
// Testing that the Charset/Encoding setting code works.
// Relating to the sending of incoming email messages.
// This uses the TImEmailTransformingInfo class in IMCM.
// Detailed description of test harness:
// Input files required to run test harness:
// Need to run t_imcv_transform_prestest prior to running this test.
// It will will the mail directory in the correct state.
// Intermediate files produced while running test harness:
// ASCII_old_ImcvSendOutputText.txt
// ASCII_SDC_Info_ImcvSendOutputText.txt
// B64_Info_ImcvSendOutputText.txt
// Charset_ASCII_Info_ImcvSendOutputText.txt
// Charset_Big5_Info_ImcvSendOutputText.txt
// Charset_JP_Info_ImcvSendOutputText.txt
// Charset_UTF7_Info_ImcvSendOutputText.txt
// Charset_UTF8_Info_ImcvSendOutputText.txt
// Def_Info_ImcvSendOutputText.txt
// None_Info_ImcvSendOutputText.txt
// QP_Info_ImcvSendOutputText.txt
// Service_B64_Info_ImcvSendOutputText.txt
// Service_Def_Info_ImcvSendOutputText.txt
// Service_QP_Info_ImcvSendOutputText.txt
// Service_UTF7_Info_ImcvSendOutputText.txt
// UTF7_SDC_Info_ImcvSendOutputText.txt
// UTF8_old_ImcvSendOutputText.txt
// UU_Info_ImcvSendOutputText.txt
// Wrong_SDC_Info_ImcvSendOutputText.txt
// Output files produced by running test harness:
// Description of how to build test harness:
// Description of how to run test harness:
// 
//

#include <e32test.h>
#include <f32fsys.h>
#include <s32file.h>
#include <e32base.h>
#include <txtetext.h>
#include <txtrich.h>
#include <bautils.h>
#include <miuthdr.h>
#include <miutconv.h>
#include <miutmsg.h>
#include <miutset.h>  //KUidMsgTypePOP3
#include <imutdll.h>	
#include <imcvtext.h>
#include <imcvrecv.h>
#include <imcvsend.h>

//Oyster includes
#include <msvstd.h>
#include <msvids.h>
#include <msvuids.h>
#include <msvruids.h>
#include <msvreg.h>

#include "msvindexadapter.h"

#include "MSVSERV.H"
#include <msventry.h> 
#include <msvapi.h>
#include <mtclbase.h>

#include "msvtestutilsbase.h"
#include "msvtestutils.h"
#include "emailtestutils.h"
#include <ecom/ecom.h>

#include <cemailaccounts.h>


void PrintTextL(const TDesC& aBuf, TInt err, TFileName& aFilename);
void PrintTextL(const TDesC& aBuf, TInt err);

LOCAL_D TInt testNo = 1;
LOCAL_D TMsvId pop3Service;
LOCAL_D TMsvId smtpService;

// File paths used by test program
#define KOutputFile				_L("ImcvSendOutputText.txt")
#define KFilePath				_L("rfc822\\Transform_Send\\")
#define KOkOutputPath			_L("imcv\\Transform_Send\\")
#define KComponent				_L("IMUT")

#define KMailFolder				KMsvGlobalOutBoxIndexEntryId

LOCAL_D RTest test(_L("T_IMCV_Transform_Send test harness"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_C CActiveScheduler* scheduler;
class CDummyMsvSessionObserver;
LOCAL_D CMsvEntrySelection* messageSelection;

LOCAL_D	TBuf<10> domainName;
LOCAL_D	TDateTime dtime;
LOCAL_D CEmailTestUtils* testUtils;

LOCAL_D TInt allEncodingMsgCount;
LOCAL_D TInt japaneseEndPos;

// Filename Prefixes
#define KAsciiOld			_L("ASCII_Old_")
#define KUTF8Old			_L("UTF8_Old_")
#define KServiceDefInfo		_L("Service_Def_Info_")
#define KServiceQPInfo		_L("Service_QP_Info_")
#define KServiceB64Info		_L("Service_B64_Info_")
#define KServiceUTF7Info	_L("Service_UTF7_Info_")
#define KDefInfo			_L("Def_Info_")
#define KNoneInfo			_L("None_Info_")
#define KUUInfo				_L("UU_Info_")
#define KB64Info			_L("B64_Info_")
#define KQPInfo				_L("QP_Info_")
#define KCTE7Bit			_L("CTE_7Bit_Info_")
#define KWrongSDCInfo		_L("Wrong_SDC_Info_")
#define KASCIIInfo			_L("ASCII_SDC_Info_")
#define KUTF7Info			_L("UTF7_SDC_Info_")
#define KCharsetASCIInfo	_L("Charset_ASCII_Info_")
#define KCharsetUTF8Info	_L("Charset_UTF8_Info_")
#define KCharsetUTF7Info	_L("Charset_UTF7_Info_")
#define KCharsetBig5Info	_L("Charset_Big5_Info_")
#define KCharsetJPInfo		_L("Charset_JP_Info_")


class TestUiTimer : public CTimer
	{
public:
	static TestUiTimer* NewLC(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation);

	void RunL();
	void DoCancel();
	void ConstructL();
	void IssueRequest();
	
	CConsoleBase*				iConsole;
	CImEmailOperation*			iEmailOperation;
	TTimeIntervalMicroSeconds32 period;
protected:
	TInt iCount;
	TBool iClosing;
	TestUiTimer(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation);
	};

//
// Progress timer
//	  
TestUiTimer* TestUiTimer::NewLC(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation)
	{
	TestUiTimer* self = new(ELeave) TestUiTimer(aConsole, aEmailOperation);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

TestUiTimer::TestUiTimer(CConsoleBase* aConsole, CImEmailOperation* aEmailOperation)
	: CTimer(EPriorityStandard+3), iEmailOperation(aEmailOperation)
	{
	iConsole = aConsole;
	period = KPeriod;
	iClosing = EFalse;
	iCount = 0;
	}

void TestUiTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
	}

void TestUiTimer::IssueRequest()
	{
	After(period);
	}

void TestUiTimer::DoCancel()
	{
	CTimer::DoCancel();
	}

void TestUiTimer::RunL()
	{
	// display the current progress
	TMsvId temp;	
	TPckgC<TMsvId> paramPack(temp);

	const TDesC8& progBuf = iEmailOperation->ProgressL();	
	paramPack.Set(progBuf);
	TMsvId progress=paramPack();	

	test.Console()->SetPos(0, 10);
	test.Printf(_L("   MessageId %d		\n"),progress);

	IssueRequest();
	};

//

class CDummyMsvSessionObserver : public CBase , public MMsvSessionObserver
	{
public:
	void HandleSessionEventL(TMsvSessionEvent /*aEvent*/, TAny* /*aArg1*/, TAny* /*aArg2*/, TAny* /*aArg3*/)
		{};
	};


LOCAL_C void CreateNewL(TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CImEmailOperation* emailOperation = CImEmailOperation::CreateNewL(testActive->iStatus, *(testUtils->iMsvSession), KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestUiTimer* testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
	testActive->StartL();
	testUiTimer->IssueRequest();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(3); // testActive, emailOperation, testUiTimer
	}


LOCAL_C void CreateForwardL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CImEmailOperation* emailOperation = CImEmailOperation::CreateForwardL(testActive->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestUiTimer* testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
	testActive->StartL();
	testUiTimer->IssueRequest();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(3); // testActive, emailOperation, testUiTimer
	}


LOCAL_C void CreateForwardAsAttachmentL(TMsvId aMessageId, TMsvPartList aPartList, TMsvEmailTypeList aMsvEmailTypeList, TInt aTestNo)
	{
	testUtils->TestStart(aTestNo);
	CTestActive* testActive = new (ELeave) CTestActive();
	CleanupStack::PushL(testActive);

	CImEmailOperation* emailOperation = CImEmailOperation::CreateForwardAsAttachmentL(testActive->iStatus, *(testUtils->iMsvSession), aMessageId, KMsvGlobalOutBoxIndexEntryId, aPartList, aMsvEmailTypeList, KUidMsgTypeSMTP);
	CleanupStack::PushL(emailOperation);

	TestUiTimer* testUiTimer = TestUiTimer::NewLC(test.Console(), emailOperation);
	testActive->StartL();
	testUiTimer->IssueRequest();
	CActiveScheduler::Start();

	CleanupStack::PopAndDestroy(3); // testActive, emailOperation, testUiTimer
	}


LOCAL_C void SetDefaultServiceL(TMsvId smtpService)
	{
	
	CEmailAccounts* accounts = CEmailAccounts::NewLC();
	
	TSmtpAccount smtpAccountId;
	accounts->GetSmtpAccountL(smtpService,smtpAccountId); 
	accounts->SetDefaultSmtpAccountL(smtpAccountId);
	
	accounts->DefaultSmtpAccountL(smtpAccountId);
	
	if (smtpService != smtpAccountId.iSmtpService)
		{
		User::Leave(KErrNotFound);
		}
	CleanupStack::PopAndDestroy(accounts);
	}

LOCAL_C void DoPlaintextCreateNewTestsL()
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = 0;

	//
	// Plaintext messages
	//
	testUtils->WriteComment(_L("\tPlaintext Create New Message Tests"));

	// only body, subject, recipients, VCard and Signature
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateNewL(partList, emailTypeList, testNo++);

	}

LOCAL_C void DoHtmlCreateNewTestsL()
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = KMsvEmailTypeListMHTMLMessage;

	//
	// HTML Messages
	//
	testUtils->WriteComment(_L("\tHTML Create New Message Tests"));

	// only body
	partList = KMsvMessagePartBody;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment
	partList = KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and attachment
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments;
	CreateNewL(partList, emailTypeList, testNo++);

	// basic new message with subject
	partList = KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only body and subject
	partList = KMsvMessagePartBody | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);

	// only attachment and subject
	partList = KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateNewL(partList, emailTypeList, testNo++);
	}


LOCAL_C void DoHtmlCreateForwardTestsL(TMsvId aMessageId)
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = KMsvEmailTypeListMHTMLMessage;

	//
	// HTML Messages
	//
	testUtils->WriteComment(_L("\tHTML Create Forward Message Tests"));

	// basic new message with subject and signature
	partList = KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, subject and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);
	}


LOCAL_C void DoPlaintextCreateForwardTestsL(TMsvId aMessageId)
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = 0;

	//
	// Plaintext messages
	//
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message Tests"));


	// only body, subject and recipients
	partList = KMsvMessagePartBody | KMsvMessagePartDescription | KMsvMessagePartRecipient;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);

	// only body, attachment, originator and signature
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartOriginator;
	CreateForwardL(aMessageId, partList, emailTypeList, testNo++);
	}

LOCAL_C void DoPlaintextCreateForwardAsAttachmentTestsL(TMsvId aMessageId)
	{
	TMsvPartList partList = 0;
	TMsvEmailTypeList emailTypeList = 0;

	//
	// Plaintext messages
	//
	testUtils->WriteComment(_L("\tPlaintext Create Forward Message Tests"));

	// only body, attachment and subject 
	partList = KMsvMessagePartBody | KMsvMessagePartAttachments | KMsvMessagePartDescription;
	CreateForwardAsAttachmentL(aMessageId, partList, emailTypeList, testNo++);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void WriteToFile(RFile file, TDesC8& aInputLine)
//----------------------------------------------------------------------------------------
	{
	file.Write(aInputLine);
	}

LOCAL_C void LogFileName(TDesC& aNameExn, TParse& aLogFileName)
	{
	TFileName filename(aNameExn);
	filename.Append(KOutputFile);

	testUtils->ResolveLogFile(filename, aLogFileName);
	}


//----------------------------------------------------------------------------------------
LOCAL_C void WriteDataL( CImSendMessage* sendMsg, TDesC& nameExn, TInt count)
//----------------------------------------------------------------------------------------
	{
	RFile file;	
	TParse parsedLogFileName;
	LogFileName(nameExn, parsedLogFileName);

	if(!testUtils->FileSession().MkDirAll(parsedLogFileName.DriveAndPath()))
		{
		TFileName buffer(parsedLogFileName.FullName());
		test.Printf(TRefByValue<const TDesC>_L("Created %S directory\n"),&buffer);
		}
	
	TInt err=file.Open(testUtils->FileSession(), parsedLogFileName.FullName(), EFileStreamText|EFileWrite|EFileShareAny);
    if (err==KErrNotFound) // file does not exist - create it
		test(file.Create(testUtils->FileSession(), parsedLogFileName.FullName(), EFileStreamText|EFileWrite|EFileShareAny)==KErrNone);
 
	TInt offset=0;
	test(file.Seek(ESeekEnd, offset)==KErrNone);

	TBuf8<10> num;
	num.Num(count);
	file.Write(_L8("\r\n---------------------- Sending Email "));
	file.Write(num);
	file.Write(_L8(" ------------------------\r\n\r\n"));
	
	TBuf8<1024> line;
	TInt paddingCount;

	// write the data from the SendMessage object into the segmented buffer
	TInt errorCode = KErrNone;
	
	do {
		line.FillZ();
		line.SetLength(0);
		
		errorCode = sendMsg->NextLineL( line, paddingCount );

		__ASSERT_DEBUG( line.Length(), User::Panic(_L("Empty line returned from SendMessage"),1));
		__ASSERT_DEBUG( line[line.Length()-2]==0x0D && line[line.Length()-1]==0x0A, User::Panic(_L("Line not CRLF terminated"),1));

		WriteToFile(file, line);
		}
	while(errorCode == KErrNone);

	file.Close();
	}

//----------------------------------------------------------------------------------------
LOCAL_C void SendOneMessage(TMsvId msgId, TUint aCharset, TImSendMethod aMethod, TPtrC16 nameExt, TInt count)
//----------------------------------------------------------------------------------------
	{
	TTime time(dtime);
	testUtils->iServerEntry->SetEntry(msgId);

	CImSendMessage* sendMsg = CImSendMessage::NewL(*(testUtils->iServerEntry));
	CleanupStack::PushL(sendMsg);

	sendMsg->InitialiseL( testUtils->iServerEntry->Entry().Id(), aMethod, time, domainName, aCharset, ESendNoCopy);

 	WriteDataL( sendMsg, nameExt, count);

	CleanupStack::PopAndDestroy(); // sendMsg
	}


//----------------------------------------------------------------------------------------
LOCAL_C void SendOneMessage(TMsvId msgId, TImEmailTransformingInfo& info, TPtrC16 nameExt, TInt count)
//----------------------------------------------------------------------------------------
	{
	TTime time(dtime);
	testUtils->iServerEntry->SetEntry(msgId);

	CImSendMessage* sendMsg = CImSendMessage::NewL(*(testUtils->iServerEntry));
	CleanupStack::PushL(sendMsg);

	sendMsg->InitialiseL( testUtils->iServerEntry->Entry().Id(), time, domainName, info, ESendNoCopy);

	TBuf<56> buf;
	buf.Copy(nameExt);
	if (info.SendMethod() == ESendAsSimpleEmail)
		buf.Append(_L("Sending Non-MIME Email\n"));
	else
		buf.Append(_L("Sending Non-MIME Email\n"));
	test.Printf(buf);

 	WriteDataL( sendMsg, nameExt, count);

	CleanupStack::PopAndDestroy(); // sendMsg
	}



//----------------------------------------------------------------------------------------
LOCAL_C void ModifyMessageL(TMsvId msgId,  TImEmailTransformingInfo& info)
//----------------------------------------------------------------------------------------
	{
	testUtils->iServerEntry->SetEntry(msgId);
	CMsvStore* store = testUtils->iServerEntry->EditStoreL();
	CleanupStack::PushL(store);
	info.StoreL(*store);
	store->CommitL();

	CleanupStack::PopAndDestroy(); // store
	}

//----------------------------------------------------------------------------------------
LOCAL_C void DeleteInfoL(TMsvId msgId)
//----------------------------------------------------------------------------------------
	{
	testUtils->iServerEntry->SetEntry(msgId);
	CMsvStore* store = testUtils->iServerEntry->EditStoreL();
	CleanupStack::PushL(store);

	if (store->IsPresentL( KUidMsgFileTransformingInfo) )
		{
		store->RemoveL(KUidMsgFileTransformingInfo);	
		store->CommitL();
		}

	CleanupStack::PopAndDestroy(); // store
	}

//----------------------------------------------------------------------------------------
LOCAL_C void ModifyServiceL( TImEmailTransformingInfo& info)
//----------------------------------------------------------------------------------------
	{
	testUtils->iServerEntry->SetEntry(KMsvRootIndexEntryId);
	CMsvStore* store = testUtils->iServerEntry->EditStoreL();
	CleanupStack::PushL(store);

	info.StoreL(*store);
	store->CommitL();

	CleanupStack::PopAndDestroy(); // store
	}



//----------------------------------------------------------------------------------------
LOCAL_C void SetEncodingQP(	TImEmailTransformingInfo& info, TImSendMethod method)
//----------------------------------------------------------------------------------------
	{
	info.SetToDefault(method);
	info.SetHeaderEncodingQ();
	info.SetBodyTextEncoding(EEncodingTypeQP);
	info.SetHTMLEncoding(EEncodingTypeQP);
	info.SetAttachmentEncoding(EEncodingTypeQP);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void SetEncodingB64(TImEmailTransformingInfo& info, TImSendMethod method)
//----------------------------------------------------------------------------------------
	{
	info.SetToDefault(method);
	info.SetHeaderEncodingQ();
	info.SetBodyTextEncoding(EEncodingTypeBASE64);
	info.SetHTMLEncoding(EEncodingTypeBASE64);
	info.SetAttachmentEncoding(EEncodingTypeBASE64);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void SetEncodingUU(TImEmailTransformingInfo& info, TImSendMethod method)
//----------------------------------------------------------------------------------------
	{
	info.SetToDefault(method);
	info.SetHeaderEncodingQ();
	info.SetBodyTextEncoding(EEncodingTypeUU);
	info.SetHTMLEncoding(EEncodingTypeUU);
	info.SetAttachmentEncoding(EEncodingTypeUU);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void SetEncodingNone(TImEmailTransformingInfo& info, TImSendMethod method)
//----------------------------------------------------------------------------------------
	{
	info.SetToDefault(method);
	info.SetHeaderEncodingQ();
	info.SetBodyTextEncoding(EEncodingTypeNone);
	info.SetHTMLEncoding(EEncodingTypeNone);
	info.SetAttachmentEncoding(EEncodingTypeNone);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void SetEncodingCTE7Bit(TImEmailTransformingInfo& info, TImSendMethod method)
//----------------------------------------------------------------------------------------
	{
	info.SetToDefault(method);
	info.SetHeaderEncodingQ();
	info.SetBodyTextEncoding(EEncodingType7Bit);
	info.SetHTMLEncoding(EEncodingType7Bit);
	info.SetAttachmentEncoding(EEncodingType7Bit);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void SendMessagesL()
//----------------------------------------------------------------------------------------
	{
	TImEmailTransformingInfo info;

	domainName.Copy(_L("psion.com"));
	test.Next(_L("Write Test"));
	dtime.Set(1996, EDecember, 1, 23, 59, 59, 0);

	testUtils->iServerEntry->SetEntry(KMailFolder);

	CMsvEntrySelection* children = new(ELeave) CMsvEntrySelection;
	testUtils->iServerEntry->GetChildren(*children);

	TInt i=0;
	// Remove info stored in message headers
	for ( i=0; i < allEncodingMsgCount; i++)
		DeleteInfoL((*children)[i]);
	
	// Check SMTP service, see if it has a Transform store.

	// SetSystemDefault(KCharacterSetIdentifierIso88591);

	// "OLD" Method
	for ( i=0; i < allEncodingMsgCount; i++)
		SendOneMessage( (*children)[i],KCharacterSetIdentifierIso88591,ESendAsSimpleEmail, KAsciiOld, i);

	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage( (*children)[i],KCharacterSetIdentifierIso88591,ESendAsMimeEmail, KAsciiOld, i);

	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage( (*children)[i],KCharacterSetIdentifierUtf8,ESendAsSimpleEmail, KUTF8Old, i);

	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage( (*children)[i],KCharacterSetIdentifierUtf8,ESendAsMimeEmail, KUTF8Old, i);

	// Testing setting info in Service.


	//--------------------------------------------------------------

	info.SetToDefault(ESendAsSimpleEmail);
	ModifyServiceL(info);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KServiceDefInfo, i);

	SetEncodingQP(info,ESendAsSimpleEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KServiceQPInfo, i);

	SetEncodingB64(info,ESendAsSimpleEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KServiceB64Info, i);

	SetEncodingNone(info, ESendAsSimpleEmail);
	info.SetHeaderAndBodyCharset(KCharacterSetIdentifierUtf7);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KServiceUTF7Info, i);

	
	info.SetToDefault(ESendAsMimeEmail);
	ModifyServiceL(info);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KServiceDefInfo, i);

	SetEncodingQP(info,ESendAsMimeEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KServiceQPInfo, i);

	SetEncodingB64(info,ESendAsMimeEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KServiceB64Info, i);

	SetEncodingNone(info, ESendAsMimeEmail);
	info.SetHeaderAndBodyCharset(KCharacterSetIdentifierUtf7);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KServiceUTF7Info, i);


	// NON_ MIME
	info.SetToDefault(ESendAsSimpleEmail);
		
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsMimeEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KDefInfo, i);

	SetEncodingNone(info,ESendAsSimpleEmail);
	info.SetHeaderCharset(KCharacterSetIdentifierIso88591);
	info.SetBodyTextCharset(KCharacterSetIdentifierIso88591);
	info.SetHTMLCharset(KCharacterSetIdentifierIso88591);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsSimpleEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KNoneInfo, i);

	SetEncodingUU(info,ESendAsSimpleEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsSimpleEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KUUInfo, i);

	SetEncodingB64(info,ESendAsSimpleEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsSimpleEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KB64Info, i);

	SetEncodingQP(info,ESendAsSimpleEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsSimpleEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KQPInfo, i);

	// MIME
	info.SetToDefault(ESendAsMimeEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsMimeEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KDefInfo, i);
		

	SetEncodingNone(info, ESendAsMimeEmail);
	info.SetHeaderCharset(KCharacterSetIdentifierIso88591);
	info.SetBodyTextCharset(KCharacterSetIdentifierIso88591);
	info.SetHTMLCharset(KCharacterSetIdentifierIso88591);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsMimeEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KNoneInfo, i);

	SetEncodingUU(info, ESendAsMimeEmail);
	info.SetHeaderCharset(KCharacterSetIdentifierUtf8);
	info.SetBodyTextCharset(KCharacterSetIdentifierUtf8);
	info.SetHTMLCharset(KCharacterSetIdentifierUtf8);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsMimeEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KUUInfo, i);

	SetEncodingB64(info, ESendAsMimeEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsMimeEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KB64Info, i);

	SetEncodingQP(info, ESendAsMimeEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsMimeEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KQPInfo, i);

	SetEncodingCTE7Bit(info, ESendAsMimeEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsMimeEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KCTE7Bit, i);

	// Testing System Default Charset non-MIME. & MIME

	TImEmailTransformingInfo info3;
	info3.SetToDefault(ESendAsSimpleEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info3, KUTF7Info, i);
	info3.SetToDefault(ESendAsMimeEmail);
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info3, KUTF7Info, i);

	// Test CHARSET

	SetEncodingNone(info, ESendAsMimeEmail);
	info.SetHeaderAndBodyCharset(KCharacterSetIdentifierUtf7);
	for (i=0; i < allEncodingMsgCount; i++)
		ModifyMessageL((*children)[i], info);
	info.SetToDefault(ESendAsMimeEmail); //should be ignored.
	for (i=0; i < allEncodingMsgCount; i++)
		SendOneMessage((*children)[i], info, KCharsetUTF7Info, i);

	// Test Japanese charset encoding

	SetEncodingB64(info, ESendAsMimeEmail);
	info.SetHeaderEncodingB();
	info.SetHeaderAndBodyCharset(KCharacterSetIdentifierIso2022Jp);
	for (i = 0; i <= japaneseEndPos; i++)
		{
		ModifyMessageL((*children)[i], info);
		}
	for (i = 0; i <= japaneseEndPos; i++)
		{
		SendOneMessage((*children)[i], info, KCharsetJPInfo, i);
		}

	SetEncodingQP(info, ESendAsMimeEmail);
	info.SetHeaderAndBodyCharset(KCharacterSetIdentifierIso2022Jp);
	for (i = 0; i <= japaneseEndPos; i++)
		{
		ModifyMessageL((*children)[i], info);
		}
	for (i = 0; i <= japaneseEndPos; i++)
		{
		SendOneMessage((*children)[i], info, KCharsetJPInfo, i);
		}
	
	delete children;
	}


//----------------------------------------------------------------------------------------
LOCAL_C void UpL()
//----------------------------------------------------------------------------------------
	{
	scheduler = new (ELeave)CActiveScheduler;
	CActiveScheduler::Install( scheduler );
	CleanupStack::PushL(scheduler);

	testUtils = CEmailTestUtils::NewLC(test,ETuCreateServerMtmReg);	

	TParse parsedLogFileName;
	TFileName name;

	testUtils->ResolveLogFile(name, parsedLogFileName);
	testUtils->FileSession().RmDir(parsedLogFileName.DriveAndPath());

	//	Loading the DLLs
	testUtils->CreateRegistryObjectAndControlL();
	testUtils->InstallSmtpMtmGroupL();
	testUtils->InstallPopMtmGroupL();

	testUtils->CreateAllTestDirectories();
	testUtils->FileSession().SetSessionPath(_L("C:\\"));
	testUtils->FileSession().MkDir(parsedLogFileName.DriveAndPath());
	testUtils->ClearEmailAccountsL();
	testUtils->CleanMessageFolderL();
	testUtils->GoServerSideL();
	}


//----------------------------------------------------------------------------------------
void PrintTextL(const TDesC& aBuf, TInt err, TFileName& aFilename)
//----------------------------------------------------------------------------------------
	{
	HBufC* buff= HBufC::NewLC(100);

	(buff->Des()).Format(aBuf, err, aFilename.PtrZ());
	testUtils->WriteComment(buff->Des());
	test.Printf(buff->Des());

	CleanupStack::PopAndDestroy();  // buff
	}

//----------------------------------------------------------------------------------------
void PrintTextL(const TDesC& aBuf, TInt err)
//----------------------------------------------------------------------------------------
	{
	HBufC* buff= HBufC::NewLC(100);

	(buff->Des()).Format(aBuf, err);
	testUtils->WriteComment(buff->Des());

	CleanupStack::PopAndDestroy();  // buff
	}


//----------------------------------------------------------------------------------------
LOCAL_C void CompareLogs()
//----------------------------------------------------------------------------------------
	{
	TInt testHarnessError=0;

	test.Console()->ClearScreen();
	test.Next(_L("Comparing:"));

	CPtrCArray* file = new (ELeave) CPtrCArray(20);
	
	file->AppendL(KAsciiOld);
	file->AppendL(KAsciiOld);
	file->AppendL(KUTF8Old);
	file->AppendL(KServiceDefInfo);
	file->AppendL(KServiceQPInfo);
	file->AppendL(KServiceB64Info);	
	file->AppendL(KServiceUTF7Info);
	file->AppendL(KDefInfo);
	file->AppendL(KNoneInfo);
	file->AppendL(KUUInfo);
	file->AppendL(KB64Info);
	file->AppendL(KQPInfo);	
	file->AppendL(KUTF7Info);
	file->AppendL(KCharsetUTF7Info);
	file->AppendL(KCharsetJPInfo);
	file->AppendL(KCTE7Bit);

	RFileReadStream file1;
	RFileReadStream file2;

	TFileName file1Name;
	TFileName file2Name;
	TParse parsedLogFileName;
	TParse parsedFileName;

	for (TInt i=0; i<file->MdcaCount(); i++)
		{
		test.Printf(_L(""));

		TBool errorOccured = EFalse;

		file1Name.Copy(file->At(i));
		file1Name.Append(KOutputFile);
		testUtils->ResolveLogFile(file1Name, parsedLogFileName);

		file2Name.Copy(KOkOutputPath); file2Name.Append(file->At(i)); file2Name.Append(KOutputFile);
		testUtils->ResolveFile(KComponent, file2Name, parsedFileName);

		TInt error1=KErrNone;
		TInt error2=KErrNone;
		
		if (i==0)
			testUtils->WriteComment(_L("Checking Epoc 6.0 Functionality\n"));
		if (i==2)
			testUtils->WriteComment(_L("Setting TImEmailTransformingInfo in Service Settings\n"));
		if (i==6)
			testUtils->WriteComment(_L("Setting TImEmailTransformingInfo in Messages\n"));
		if (i==11)
			testUtils->WriteComment(_L("Checking SystemDefaultCharset\n"));
		if (i==14)
			testUtils->WriteComment(_L("Checking Charset Settings\n"));
		
		
		test.Printf(_L("\n%s"), file2Name.PtrZ());

		error1=file1.Open(testUtils->FileSession(), parsedLogFileName.FullName(), EFileShareAny);
		if (error1!=KErrNone)
			{
			file1Name = parsedLogFileName.FullName();
			PrintTextL(_L("\nERROR %d opening file %s"), error1, file1Name);
			errorOccured = ETrue;
			}
		else
			{
			error2=file2.Open(testUtils->FileSession(), parsedFileName.FullName(), EFileShareAny);
			if(error2!=KErrNone)
				{
				PrintTextL(_L("\nERROR %d opening file %s"), error2, file2Name);
				errorOccured = ETrue;
				}
			}
		
		if ((error1==KErrNone)&&(error2==KErrNone))
			{
			// read the file into the conversion object
			HBufC8* line1buf = HBufC8::NewLC(1024);
			TPtr8 line1 = line1buf->Des();
			TBuf8<1> aChar1;
			TBuf8<1024> line2;
			TBuf8<1> aChar2;
			TInt file1LineCounter = 0;

			TBool finished = EFalse;

			do	{
				line1.FillZ();
				line1.SetLength(0);
				// compile the lines one char at a time
				do	{
					TRAPD( error, file1.ReadL(aChar1, 1) ); 
					if (error!=KErrEof)
						if (aChar1[0]!=0x0D)
							line1.Append(aChar1);
					else
						{
						finished = ETrue;
						break;
						}
					// stop at the end of line or no more data
					}
				while((aChar1[0]!=0x0A)&&(line1.Length()<1024));

				line2.FillZ();
				line2.SetLength(0);
				do	{
					TRAPD( error, file2.ReadL(aChar2, 1) ); 
					if (error!=KErrEof)
						if (aChar1[0]!=0x0D)
							line2.Append(aChar1);
					else
						break;
					// stop at the end of line or no more data
					}
				while((aChar2[0]!=0x0A)&&(line2.Length()<1024));
				
				if (!finished)
					{
					if(aChar1[0]==0x0A) 
						file1LineCounter++;
					if(line1.Match(_L8("Message-ID*")) == 0)
						continue;
					if(line1.Find(_L8("EPOC32=")) != KErrNotFound)
						continue;
					if(!line2.Length())
						{
						PrintTextL(_L("\nERROR - End of file reached on line %d"), file1LineCounter);
						errorOccured = ETrue;
						break;
						} 

					if (line1.Compare(line2))
						{
						PrintTextL( _L("\nERROR - Line %d is different"), file1LineCounter);
						errorOccured = ETrue;
						} 
					}
				}
			while(!finished);
			CleanupStack::PopAndDestroy();  // line1buf

		}

		file1.Close();
		file2.Close();

		if (errorOccured)
			{
			testUtils->TestFinish(i+1, KErrGeneral);
			testHarnessError=errorOccured;
			test.Printf(_L(" - FAILED"));
			}
		else
			{
			testUtils->TestFinish(i+1, KErrNone);
			test.Printf(_L(" - OK"));
			}
		}
	
	if (testHarnessError==0)
		testUtils->TestHarnessCompleted(); 
	else
		testUtils->TestHarnessFailed(testHarnessError);

	delete file;
	test.Printf(_L("\n"));

	}


//----------------------------------------------------------------------------------------
LOCAL_C void Down()
//----------------------------------------------------------------------------------------
	{
	CleanupStack::PopAndDestroy(testUtils);  
	CleanupStack::PopAndDestroy(scheduler);
	}



//----------------------------------------------------------------------------------------
void TestingTransformingInfoAPI()
//----------------------------------------------------------------------------------------
	{
	TImEmailTransformingInfo info;
	TImEmailTransformingInfo info1;

	// Check overloaded operators.
	info.SetToDefault(ESendAsMimeEmail); 
	info.SetHeaderCharset(KCharacterSetIdentifierIso88591);
	info.SetBodyTextCharset(KCharacterSetIdentifierIso88591);
	info.SetHTMLCharset(KCharacterSetIdentifierIso88591);
	info1.SetToDefault(ESendAsSimpleEmail); 

	if (info==info1)
		testUtils->WriteComment( _L("\nTesting TInEmail overloaded ==- FAILED"));
	else
		testUtils->WriteComment( _L("\nTesting TInEmail overloaded ==- OK"));

	info=info1;
	if (info==info1)
		testUtils->WriteComment( _L("\nTesting TImEmailTransformingInfo =- OK"));
	else
		testUtils->WriteComment( _L("\nTesting TImEmailTransformingInfo =- FAILED"));
	}


//----------------------------------------------------------------------------------------
void FillMailboxWithMessagesL()
//----------------------------------------------------------------------------------------
	{
	testUtils->GoClientSideL();

	smtpService = testUtils->CreateSmtpServiceL();
	pop3Service = testUtils->CreatePopServiceL();

	testUtils->GoServerSideL();

	TParse parsedLogFileName;
	TFileName name(KFilePath);
	testUtils->ResolveFile(KComponent, name, parsedLogFileName);
	testUtils->SpecifyRfc822Dir(parsedLogFileName.FullName());

	testUtils->CreateMessageFilesL(pop3Service, KMsvGlobalInBoxIndexEntryId, parsedLogFileName.FullName());

	test.Console()->SetPos(0, 6);
	test.Printf(_L("Performing Tests:\n"));
	
	testUtils->GoClientSideL();

	SetDefaultServiceL(smtpService);

	// get a list of all email messages in the Inbox
	testUtils->iMsvEntry->SetEntryL(KMsvGlobalInBoxIndexEntryId);
	delete messageSelection;
	messageSelection = testUtils->iMsvEntry->ChildrenWithTypeL(KUidMsvMessageEntry);
	
//	
//	Create New tests
//
	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create New Tests1:    "));
	testUtils->WriteComment(_L("Create New Plaintext Message Tests"));
	DoPlaintextCreateNewTestsL();

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create New Tests2:    "));
	testUtils->WriteComment(_L("Create New HMTL Message Tests"));
	DoHtmlCreateNewTestsL();

//
//	Create Plaintext Forward tests
//
	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests1:"));
	testUtils->WriteComment(_L("Create Plaintext Forward Tests"));
	// Forward to normal plaintext message with no attachments
	testUtils->WriteComment(_L("Create Forward to Plaintext message with no attachments Tests"));
	DoPlaintextCreateForwardTestsL(messageSelection->At(5));

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests2:"));
	//Create Forwarded message to plaintext message with attachment
	testUtils->WriteComment(_L("Create Forward to Plaintext message with attachment Tests"));
	DoPlaintextCreateForwardTestsL(messageSelection->At(1));

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests3:"));
	//Create Forwarded html message with attachment and no text alternative to the HTML
	testUtils->WriteComment(_L("Create Forward to HTML message with attachment and no text alt. Tests"));
	DoPlaintextCreateForwardTestsL(messageSelection->At(3));

//
//	Create HTML Forward tests
//
	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests5:"));
	testUtils->WriteComment(_L("Create HTML Forward Tests"));
	// Forward to normal plaintext message with no attachments
	testUtils->WriteComment(_L("Create Forward to Plaintext message with no attachments Tests"));
	DoHtmlCreateForwardTestsL(messageSelection->At(5));

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests6:"));
	//Create Forwarded plaintext message with attachment
	testUtils->WriteComment(_L("Create Forward to Plaintext message with attachment Tests"));
	DoHtmlCreateForwardTestsL(messageSelection->At(1));

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests7:"));
	//Create Forwarded html message with attachment and no text alternative to the HTML
	testUtils->WriteComment(_L("Create Forward to HTML message with attachment and no text alt. Tests"));
	DoHtmlCreateForwardTestsL(messageSelection->At(3));

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests8:"));
	//Forward As Attachment to plaintext message with attachment
	testUtils->WriteComment(_L("Create Forward As Atcmt to Plaintext message with atcmt Tests"));
	DoPlaintextCreateForwardAsAttachmentTestsL(messageSelection->At(1));

	// The messages created so far will be used in all the encoding tests.
	allEncodingMsgCount = testNo - 1;

	// The following messages will be used only in specific individual tests

	// Japanese charset header tests
	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests9:"));
	// Create forwarded plaintext message with single Japanese character in subject
	testUtils->WriteComment(_L("Create Forward to Plaintext message with single Japanese subject Tests"));
	DoPlaintextCreateForwardTestsL(messageSelection->At(6));

	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests10:"));
	// Create forwarded plaintext message with long Japanese subject line
	testUtils->WriteComment(_L("Create Forward to Plaintext message with long Japanese subject Tests"));
	DoPlaintextCreateForwardTestsL(messageSelection->At(7));
	japaneseEndPos = testNo - 2;

    // DEF064111: Garbled Japanese encoding of subject line containing <,> and @ symbols
	test.Console()->SetPos(3, 7);
	test.Printf(_L("Create Forward Tests11:"));
	// Create forwarded plaintext message with garbled Japanese encoding of 
	// subject line containing <,> and @ symbols
	testUtils->WriteComment(_L("Create Forward to Plaintext message with garbled Japanese encoding of subject line containing <,> and @ symbols"));
	DoPlaintextCreateForwardTestsL(messageSelection->At(8));
	japaneseEndPos = testNo - 2;
	
	
	testUtils->FindChildrenL(KMsvGlobalOutBoxIndexEntryId, EFalse, EFalse); // writes message info (from Outbox) into files

	testUtils->Reset();
	delete messageSelection;

	}

//----------------------------------------------------------------------------------------
LOCAL_C void doMainL()
//----------------------------------------------------------------------------------------
	{	
	UpL();

	test.Console()->ClearScreen();
	test.Console()->SetPos(0, 0);
	test.Printf(_L("\n---------------------"));
	test.Printf(_L("\n   IMCV Tranform SEND Test   "));
	test.Printf(_L("\n---------------------\n"));

	FillMailboxWithMessagesL();

	testUtils->GoServerSideL();


	SendMessagesL();
	TestingTransformingInfoAPI();
	CompareLogs();
 	
	Down();
	
	// close this as conarc doesn't, remove when conarc fix their defect
	REComSession::FinalClose();
  }

//----------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
//----------------------------------------------------------------------------------------
	{	
	__UHEAP_MARK;
	test.Start(_L("T_IMCV Test harness"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
