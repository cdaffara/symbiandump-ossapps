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

#include "ctestimapfetchbody.h"
#include "cfakeinputstream.h"
#include "cfakeoutputstream.h"
#include "cactivewaiter.h"
#include "moutputstream.h"
#include "cimapsession.h"
#include <msvids.h>
#include <msvuids.h>
#include <miuthdr.h>
#include <miutset.h>
#include "emailtestutils.h"
#include "MSVSERV.H"
#include "cimapfetchbodyresponse.h"
#include "cimaputils.h"
#include "cimapfolderinfo.h"
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
#include "cimapcapabilityinfo.h"
#endif

_LIT(KImapFetchBodyTest,"t_imapfetchbody");
_LIT(KBodyDetails,"testbody");
_LIT(KCDrive,"c:\\");
RTest test(KImapFetchBodyTest);

CTestImapFetchBody::CTestImapFetchBody()
	: iInputStream(NULL)
	, iOutputStream(NULL)
	, iActiveWaiter(NULL)
	, iImapSession(NULL)
	, iFetchBodyInfo(NULL)
	, iMsvMessageEntry(0)
	, iAttMessageEntry(0)
	, iBodyMessageEntry(0)
	, iFolderId(0)
	, iImapSettings(NULL)
	, iImapMailStore(NULL)
	, iServerEntry(NULL)
	, iFetchBodyResponse(NULL)
	, iMsvServer(NULL)
	{}

CTestImapFetchBody::~CTestImapFetchBody()
	{
	delete iImapSession;
	delete iActiveWaiter;
	delete iOutputStream;
	delete iInputStream;
	delete iFetchBodyInfo;	
	delete iImapMailStore;
	delete iImapSettings;
	delete iFetchBodyResponse;
	delete iServerEntry;
	CImapUtils::Delete();
	delete iMsvServer;
	}
	

void CTestImapFetchBody::CreateMessageStructureL(TMsvId aId)
	{
	//message root entry
	TMsvEntry entry1;
	entry1.iType = KUidMsvMessageEntry;
	entry1.iServiceId = KMsvLocalServiceIndexEntryId;
	entry1.iMtm = KUidMsgTypeIMAP4;
	iServerEntry->SetEntry(aId);
	iServerEntry->CreateEntry(entry1);
	iServerEntry->SetEntry(entry1.Id());
	iMsvMessageEntry=entry1.Id();

	//folder entry
	TMsvEntry folder;
	folder.iType = KUidMsvFolderEntry;
	folder.iMtm = KUidMsvLocalServiceMtm;
	folder.iServiceId = KMsvLocalServiceIndexEntryId;
	iServerEntry->SetEntry(iMsvMessageEntry);
	iServerEntry->CreateEntry(folder);
	iServerEntry->SetEntry(folder.Id());
	iFolderId=folder.Id();

	//body entry
	TMsvEntry bodyentry;
	bodyentry.iType = KUidMsvEmailTextEntry;
	bodyentry.iServiceId = KMsvLocalServiceIndexEntryId;
	bodyentry.iMtm = KUidMsgTypeIMAP4;
	bodyentry.iDetails.Set(KBodyDetails);
	iServerEntry->SetEntry(iFolderId);
	iServerEntry->CreateEntry(bodyentry);
	iServerEntry->SetEntry(bodyentry.Id());
	iBodyMessageEntry=bodyentry.Id();	
	}


void CTestImapFetchBody::SetupL()
	{	
	ASSERT(iInputStream == NULL);
	ASSERT(iOutputStream == NULL);
	ASSERT(iActiveWaiter == NULL);
	ASSERT(iImapSession == NULL);
	ASSERT(iMsvServer == NULL);
	
	//clear the messaging folder
	CEmailTestUtils* testUtils;
	testUtils = CEmailTestUtils::NewL(test);
	CleanupStack::PushL(testUtils);
	testUtils->FileSession().SetSessionPath(KCDrive);
	
// TODO: How to clean the message folders or close the messaging server without a PANIC ????
//	testUtils->CleanMessageFolderL();
	
	testUtils->ClearEmailAccountsL();
	
	testUtils->CreateAllTestDirectories( );
	testUtils->FileSession( ).SetSessionPath( _L( "C:\\" ) );

	CleanupStack::PopAndDestroy(testUtils);

	iMsvServer = CMsvServer::NewL();

	// wait a couple of seconds
	CTestTimer* timer = CTestTimer::NewL();
	timer->After(5000000);
	CActiveScheduler::Start();
	delete timer;
	iServerEntry = CMsvServerEntry::NewL(*iMsvServer, KMsvRootIndexEntryId);
	CImapUtils::CreateL();
	iImapMailStore = CImapMailStore::NewL(*iServerEntry);	 
	iImapSettings = CImapSettings::NewL(*iServerEntry);

	iInputStream = CFakeInputStream::NewL(Logger());
	iOutputStream = CFakeOutputStream::NewL(Logger());
	iActiveWaiter = new(ELeave)CActiveWaiter(Logger());
	iImapSession = CImapSession::NewL(*iImapSettings,*iImapMailStore,*iInputStream, *iOutputStream);
	
	//create the test environment 
	CreateMessageStructureL(KMsvGlobalOutBoxIndexEntryIdValue);	

	
	INFO_PRINTF1(_L("Setup: ServerGreeting"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* OK Microsoft Exchange 2000 IMAP4rev1 server version 6.0.6249.0 (lon-cn-exchng2k.msexchange2k.closedtest.intra) ready.\r\n"));
	
	ASSERT_EQUALS(CImapSession::EServerStateNone, iImapSession->ServerState());
	
	iImapSession->ReadServerGreetingL(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();
	
	INFO_PRINTF1(_L("...Login"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("1 OK LOGIN completed\r\n"));
			
	iImapSession->LoginL(iActiveWaiter->iStatus, _L8("username"), _L8("password"));
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(CImapSession::EServerStateAuthenticated, iImapSession->ServerState());


	INFO_PRINTF1(_L("...Select inbox"));
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("2 OK [READ-WRITE] SELECT completed\r\n"));
		
	CImapFolderInfo* folderInfo = CImapFolderInfo::NewL();
	CleanupStack::PushL(folderInfo);
	
	folderInfo->SetNameL(_L16("inbox"));	
	CleanupStack::Pop();//folderInfo
	iImapSession->SelectL(iActiveWaiter->iStatus, folderInfo);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(CImapSession::EServerStateSelected, iImapSession->ServerState());
	ASSERT_EQUALS(folderInfo, iImapSession->SelectedFolderInfo());
	
	
	INFO_PRINTF1(_L("Setup: Complete"));
	}
	
void CTestImapFetchBody::TearDownL()
	{
	delete iImapSession;
	iImapSession = NULL;
	
	delete iActiveWaiter;
	iActiveWaiter = NULL;
	
	delete iOutputStream;
	iOutputStream = NULL;
	
	delete iInputStream;
	iInputStream = NULL;
	
	delete iServerEntry;
	iServerEntry = NULL;
	
	delete iImapMailStore;
	iImapMailStore = NULL;
		
	delete iImapSettings;
	iImapSettings = NULL;
	
	delete iServerEntry;
	iServerEntry = NULL;

	CImapUtils::Delete();
	
	delete iMsvServer;
	iMsvServer = NULL;
	}

// Tests
void CTestImapFetchBody::TestFetchBodyL()
	{
	INFO_PRINTF1(_L("TestFetchBodyL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BODY[1]<0> {19}\r\n"));
	iInputStream->AppendInputStringL(_L8("a small test body\r\n")); 
	iInputStream->AppendInputStringL(_L8("UID 2717 FLAGS (\\Seen))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	CFetchBodyInfo* iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus,messageUid,peek,*iFetchBodyInfo, *iFetchBodyResponse);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapFetchBody::TestFetchBodyWithMimeL()
	{
	INFO_PRINTF1(_L("TestFetchBodyWithMimeL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BODY[1]<0> {10}\r\n"));
	iInputStream->AppendInputStringL(_L8("the end.\r\n"));
	iInputStream->AppendInputStringL(_L8(" BODY[1.MIME] {417}\r\n"));
	iInputStream->AppendInputStringL(_L8("From: \"pipex\" <mdgi05@imap.dial.pipex.com>\r\n"));
	iInputStream->AppendInputStringL(_L8("To: \"David Test Account\" <davids@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: t1\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Wed, 10 May 2006 10:23:50 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("MIME-Version: 1.0\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Type: text/plain;\r\n"));
	iInputStream->AppendInputStringL(_L8("	 charset=\"iso-8859-1\"\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Transfer-Encoding: 7bit\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Priority: 3\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MSMail-Priority: Normal\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Mailer: Microsoft Outlook Express 6.00.2800.1807\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2800.1807\r\n"));
	iInputStream->AppendInputStringL(_L8(" UID 3120 FLAGS (\\Seen \\Draft))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	delete iFetchBodyInfo;
	iFetchBodyInfo=NULL;
	iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	TImEncodingType encoding = EEncodingTypeNone;
	iFetchBodyInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	iFetchBodyInfo->SetContentTransferEncoding(encoding);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus,messageUid,peek,*iFetchBodyInfo, *iFetchBodyResponse);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}
	
void CTestImapFetchBody::TestFetchBodyWithMime2L()
	{
	INFO_PRINTF1(_L("TestFetchBodyWithMime2L"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 3120 BODY[1]<0> {10}\r\n"));
	iInputStream->AppendInputStringL(_L8("the end.\r\n"));
	iInputStream->AppendInputStringL(_L8(" BODY[1.MIME] {417}\r\n"));
	iInputStream->AppendInputStringL(_L8("From: \"pipex\" <mdgi05@imap.dial.pipex.com>\r\n"));
	iInputStream->AppendInputStringL(_L8("To: \"David Test Account\" <davids@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: t1\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Wed, 10 May 2006 10:23:50 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("MIME-Version: 1.0\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Type: text/plain;\r\n"));
	iInputStream->AppendInputStringL(_L8("	 charset=\"iso-8859-1\"\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Transfer-Encoding: 7bit\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Priority: 3\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MSMail-Priority: Normal\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Mailer: Microsoft Outlook Express 6.00.2800.1807\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2800.1807\r\n"));
	iInputStream->AppendInputStringL(_L8(" FLAGS (\\Seen \\Draft))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	delete iFetchBodyInfo;
	iFetchBodyInfo=NULL;
	iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	TImEncodingType encoding = EEncodingTypeNone;
	iFetchBodyInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	iFetchBodyInfo->SetContentTransferEncoding(encoding);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus, messageUid, peek,*iFetchBodyInfo, *iFetchBodyResponse);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}

// When server responses with no body length
void CTestImapFetchBody::TestFetchBodyWithNoBodyLengthInServerResponseL()
	{
	INFO_PRINTF1(_L("TestFetchBodyWithNoBodyLengthInServerResponseL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (UID 3120 BODY[1]<0> \"text is written here\" BODY[1.MIME] {417}\r\n"));
	iInputStream->AppendInputStringL(_L8("From: \"pipex\" <mdgi05@imap.dial.pipex.com>\r\n"));
	iInputStream->AppendInputStringL(_L8("To: \"David Test Account\" <davids@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: t1\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Wed, 10 May 2006 10:23:50 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("MIME-Version: 1.0\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Type: text/plain;\r\n"));
	iInputStream->AppendInputStringL(_L8("	 charset=\"iso-8859-1\"\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Transfer-Encoding: 7bit\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Priority: 3\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MSMail-Priority: Normal\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Mailer: Microsoft Outlook Express 6.00.2800.1807\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2800.1807\r\n"));
	iInputStream->AppendInputStringL(_L8(" FLAGS (\\Seen \\Draft))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	delete iFetchBodyInfo;
	iFetchBodyInfo=NULL;
	iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	TImEncodingType encoding = EEncodingTypeNone;
	iFetchBodyInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	iFetchBodyInfo->SetContentTransferEncoding(encoding);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus, messageUid, peek,*iFetchBodyInfo, *iFetchBodyResponse);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}

/**
 The server might explicitly state that there is no MIME data by using NIL
 */
void CTestImapFetchBody::TestFetchBodyWithNilMimeL()
	{
	INFO_PRINTF1(_L("TestFetchBodyWithNilMimeL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (FLAGS (\\Seen \\Draft)"));
	iInputStream->AppendInputStringL(_L8("UID 3120 BODY[1]<0> {10}\r\n"));
	iInputStream->AppendInputStringL(_L8("the end.\r\n"));
	iInputStream->AppendInputStringL(_L8(" BODY[1.MIME] nil)\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	delete iFetchBodyInfo;
	iFetchBodyInfo=NULL;
	iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	TImEncodingType encoding = EEncodingTypeNone;
	iFetchBodyInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	iFetchBodyInfo->SetContentTransferEncoding(encoding);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus, messageUid, peek, *iFetchBodyInfo, *iFetchBodyResponse);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}

/**
 The server might explicitly state that there is no MIME data by using an empty string.
 This is what the WU server does.
 */
void CTestImapFetchBody::TestFetchBodyWithEmptyMimeL()
	{
	INFO_PRINTF1(_L("TestFetchBodyWithEmptyMimeL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (FLAGS (\\Seen \\Draft)"));
	iInputStream->AppendInputStringL(_L8("UID 3120 BODY[1]<0> {10}\r\n"));
	iInputStream->AppendInputStringL(_L8("the end.\r\n"));
	iInputStream->AppendInputStringL(_L8(" BODY[1.MIME] \"\")\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	delete iFetchBodyInfo;
	iFetchBodyInfo=NULL;
	iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	TImEncodingType encoding = EEncodingTypeNone;
	iFetchBodyInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	iFetchBodyInfo->SetContentTransferEncoding(encoding);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus, messageUid, peek, *iFetchBodyInfo, *iFetchBodyResponse);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}
void CTestImapFetchBody::TestFetchBodyWithMimeZeroAngelBracketL()
	{
	INFO_PRINTF1(_L("TestFetchBodyWithMimeZeroAngelBracketL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BODY[1]<0> {10}\r\n"));
	iInputStream->AppendInputStringL(_L8("the end.\r\n"));
	iInputStream->AppendInputStringL(_L8(" BODY[1.MIME]<0> {417}\r\n"));
	iInputStream->AppendInputStringL(_L8("From: \"pipex\" <mdgi05@imap.dial.pipex.com>\r\n"));
	iInputStream->AppendInputStringL(_L8("To: \"David Test Account\" <davids@msexchange2k.closedtest.intra>\r\n"));
	iInputStream->AppendInputStringL(_L8("Subject: t1\r\n"));
	iInputStream->AppendInputStringL(_L8("Date: Wed, 10 May 2006 10:23:50 +0100\r\n"));
	iInputStream->AppendInputStringL(_L8("MIME-Version: 1.0\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Type: text/plain;\r\n"));
	iInputStream->AppendInputStringL(_L8("	 charset=\"iso-8859-1\"\r\n"));
	iInputStream->AppendInputStringL(_L8("Content-Transfer-Encoding: 7bit\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Priority: 3\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MSMail-Priority: Normal\r\n"));
	iInputStream->AppendInputStringL(_L8("X-Mailer: Microsoft Outlook Express 6.00.2800.1807\r\n"));
	iInputStream->AppendInputStringL(_L8("X-MimeOLE: Produced By Microsoft MimeOLE V6.00.2800.1807\r\n"));
	iInputStream->AppendInputStringL(_L8(" UID 3120 FLAGS (\\Seen \\Draft))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed.\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	delete iFetchBodyInfo;
	iFetchBodyInfo=NULL;
	iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	TImEncodingType encoding = EEncodingTypeNone;
	iFetchBodyInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	iFetchBodyInfo->SetContentTransferEncoding(encoding);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus,messageUid,peek,*iFetchBodyInfo, *iFetchBodyResponse);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}

void CTestImapFetchBody::TestFetchBodyWithCancelL()
	{
	INFO_PRINTF1(_L("TestFetchBodyWithCancelL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BODY[1]<0> {19}\r\n"));
	iInputStream->AppendInputStringL(_L8("a small test body\r\n")); 
		
	iInputStream->NotifyWhenStreamIsEmpty(*this);
	
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	CFetchBodyInfo* iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus,messageUid,peek,*iFetchBodyInfo, *iFetchBodyResponse);
	
	// OnInputStreamIsEmptyL will be called during WaitActive, causing FetchBodyL to be cancelled
	iActiveWaiter->WaitActive(KErrCancel);	

	// Simulate the rest of the response being returned (after the cancel)
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("UID 2717 FLAGS (\\Seen))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed\r\n"));
		
		
	// Flush the rest of the response
	iImapSession->FlushCancelledCommand(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	// Response data should now be flushed.
	// Try to do a simple list command using the same input stream.
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Drafts\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\Marked) \"/\" INBOX\r\n"));
	iInputStream->AppendInputStringL(_L8("4 OK LIST completed\r\n"));
		
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L(""), _L("%"), folderList);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(folderList.Count(), 2);
	
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'/');
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16("Drafts"));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_TRUE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	ASSERT_EQUALS(folderList[1]->iHierarchySeperator, (TChar)'/');
	ASSERT_EQUALS(folderList[1]->FolderName(), _L16("INBOX"));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_TRUE(folderList[1]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::EUnmarked));

	folderList.ResetAndDestroy();

	INFO_PRINTF1(_L("Complete"));
	}

	
// Tests
void CTestImapFetchBody::TestFetchMultiChunkBodyL()
	{
	INFO_PRINTF1(_L("TestFetchMultiChunkBodyL"));
	
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BODY[1] {19}\r\n"));
	iInputStream->AppendInputStringL(_L8("a small test body\r\n")); 
	iInputStream->AppendInputStringL(_L8("UID 2717 FLAGS (\\Seen))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	CFetchBodyInfo* fetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	TImEncodingType encoding = EEncodingTypeNone;
	fetchBodyInfo->SetCharsetId(KCharacterSetIdentifierAscii);	
	fetchBodyInfo->SetContentTransferEncoding(encoding);
	fetchBodyInfo->SetSizeToFetch(messageSize);
	fetchBodyInfo->SetIsText(ETrue);
	fetchBodyInfo->SetRelativePathL(bodySection);
	fetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus,messageUid,peek,*fetchBodyInfo, *iFetchBodyResponse);
	delete fetchBodyInfo;

	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
void CTestImapFetchBody::TestFetchBinaryBodyL()
	{
	INFO_PRINTF1(_L("TestFetchBinaryBodyL"));
		
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BINARY[1]<0> {19}\r\n"));
	iInputStream->AppendInputStringL(_L8("a small test body\r\n")); 
	iInputStream->AppendInputStringL(_L8("UID 2717 FLAGS (\\Seen))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed\r\n"));
		
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	// Set BINARY capability
	CImapCapabilityInfo& capabilityInfo = const_cast<CImapCapabilityInfo&>(iImapSession->CapabilityInfo());
	capabilityInfo.SetFlag(CImapCapabilityInfo::EBinaryCap, ETrue);
	
	CFetchBodyInfo* iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus,messageUid,peek,*iFetchBodyInfo, *iFetchBodyResponse);
	iActiveWaiter->WaitActive();

	INFO_PRINTF1(_L("Complete"));
	}	

void CTestImapFetchBody::TestFetchBinaryBodyWithCancelL()
	{
	INFO_PRINTF1(_L("TestFetchBinaryBodyWithCancelL"));
	
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* 1 FETCH (BINARY[1]<0> {19}\r\n"));
	iInputStream->AppendInputStringL(_L8("a small test body\r\n")); 
		
	iInputStream->NotifyWhenStreamIsEmpty(*this);
	
	//some test values for the info class
	_LIT8(KBodySection,"1");
	const TDesC8& bodySection(KBodySection);
	TInt messageSize=19;
	
	//more test values
	TUint messageUid=45;
	TBool peek=ETrue;
	
	// Set BINARY capability
	CImapCapabilityInfo& capabilityInfo = const_cast<CImapCapabilityInfo&>(iImapSession->CapabilityInfo());
	capabilityInfo.SetFlag(CImapCapabilityInfo::EBinaryCap, ETrue);
	
	CFetchBodyInfo* iFetchBodyInfo = CFetchBodyInfo::NewL(iBodyMessageEntry);
	iFetchBodyInfo->SetSizeToFetch(messageSize);
	iFetchBodyInfo->SetIsText(ETrue);
	iFetchBodyInfo->SetRelativePathL(bodySection);
	iFetchBodyInfo->SetBodyPartRemainingSize(0);

	delete iFetchBodyResponse;
	iFetchBodyResponse = NULL;
	iFetchBodyResponse = CImapFetchBodyResponse::NewL();
	iImapSession->FetchBodyL(iActiveWaiter->iStatus,messageUid,peek,*iFetchBodyInfo, *iFetchBodyResponse);
	
	// OnInputStreamIsEmptyL will be called during WaitActive, causing FetchBodyL to be cancelled
	iActiveWaiter->WaitActive(KErrCancel);	

	// Simulate the rest of the response being returned (after the cancel)
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("UID 2717 FLAGS (\\Seen))\r\n"));
	iInputStream->AppendInputStringL(_L8("3 OK FETCH completed\r\n"));
		
		
	// Flush the rest of the response
	iImapSession->FlushCancelledCommand(iActiveWaiter->iStatus);
	iActiveWaiter->WaitActive();

	// Response data should now be flushed.
	// Try to do a simple list command using the same input stream.
	iInputStream->ResetInputStrings();
	iInputStream->AppendInputStringL(_L8("* LIST (\\UnMarked) \"/\" Drafts\r\n"));
	iInputStream->AppendInputStringL(_L8("* LIST (\\Marked) \"/\" INBOX\r\n"));
	iInputStream->AppendInputStringL(_L8("4 OK LIST completed\r\n"));
		
	RArrayImapListFolderInfo folderList;
	iImapSession->ListL(iActiveWaiter->iStatus, _L(""), _L("%"), folderList);
	iActiveWaiter->WaitActive();

	ASSERT_EQUALS(folderList.Count(), 2);
	
	ASSERT_EQUALS(folderList[0]->iHierarchySeperator, (TChar)'/');
	ASSERT_EQUALS(folderList[0]->FolderName(), _L16("Drafts"));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_FALSE(folderList[0]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_TRUE(folderList[0]->QueryFlag(CImapListFolderInfo::EUnmarked));
	
	ASSERT_EQUALS(folderList[1]->iHierarchySeperator, (TChar)'/');
	ASSERT_EQUALS(folderList[1]->FolderName(), _L16("INBOX"));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoinferiors));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::ENoselect));
	ASSERT_TRUE(folderList[1]->QueryFlag(CImapListFolderInfo::EMarked));
	ASSERT_FALSE(folderList[1]->QueryFlag(CImapListFolderInfo::EUnmarked));

	folderList.ResetAndDestroy();

	INFO_PRINTF1(_L("Complete"));
	}
#endif
	
void CTestImapFetchBody::OnInputStreamIsEmptyL()
	{
	iActiveWaiter->CancelWaitActive(*this);
	}
	
void CTestImapFetchBody::DoCancel()
	{
	iImapSession->Cancel();
	}	

CTestSuite* CTestImapFetchBody::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestFetchBodyL);
	ADD_ASYNC_TEST_STEP(TestFetchBodyWithCancelL);
//	ADD_ASYNC_TEST_STEP(TestFetchMultiChunkBodyL);
	ADD_ASYNC_TEST_STEP(TestFetchBodyWithMimeL);
	ADD_ASYNC_TEST_STEP(TestFetchBodyWithMimeZeroAngelBracketL);
	ADD_ASYNC_TEST_STEP(TestFetchBodyWithMime2L);
	ADD_ASYNC_TEST_STEP(TestFetchBodyWithNoBodyLengthInServerResponseL);
	ADD_ASYNC_TEST_STEP(TestFetchBodyWithNilMimeL);
	ADD_ASYNC_TEST_STEP(TestFetchBodyWithEmptyMimeL);
#if (defined SYMBIAN_EMAIL_CAPABILITY_SUPPORT)
	// repeate the above test cases using FETCH BINARY 
	ADD_ASYNC_TEST_STEP(TestFetchBinaryBodyL);		
	ADD_ASYNC_TEST_STEP(TestFetchBinaryBodyWithCancelL); 
#endif 
	END_SUITE;
	}
