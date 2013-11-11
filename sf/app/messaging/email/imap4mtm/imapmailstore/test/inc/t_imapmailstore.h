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

#ifndef __T_IMAPMAILSTORE_H
#define __T_IMAPMAILSTORE_H

#include <test/tefunit.h>

#include "emailtestutils.h"
#include "MSVSERV.H"
#include "cimapmailstore.h"

//constants shared by the tests
const TInt KTestStringLength=10;
const TInt KMaxFilePathLength=50;
const TInt K2Seconds=2000000;
const TInt KBufferGranularity = 256;
const TInt KLogId=4;
_LIT(KMiutToRecipients1, "recipient1@address.com");
_LIT(KMiutToRecipients2, "recipient2@address.com");
_LIT(KMiutCcRecipients1, "cc1@address.com");
_LIT(KMiutCcRecipients2, "cc2@address.com");
_LIT(KMiutBccRecipients, "bcc@address.com");
_LIT(KEmail40kBodyFile,"c:\\msgtest\\testdata\\T_OnlinePerformance40KBody.txt");
_LIT(KTestTextString1,"testchunk1");
_LIT(KTestTextString2,"testchunk2");
_LIT(KTestTextString3,"testchunk3");
_LIT(KStoringChunk0,"Storing chunk 0...");
_LIT(KStoringChunk1,"Storing chunk 1...");
_LIT(KStoringChunk2,"Storing chunk 2...");
_LIT(KRestoringBodyData,"Restoring body data...");
_LIT(KCheckingBodyData,"Checking size of body data...");
_LIT(KBodyDataSize,"Checking size of body data...");
_LIT(KCheckingData,"Checking order the data was stored...");
_LIT(KRestoringCImMimeHeader,"Restoring CImMimeHeader...");
_LIT(KStoringCImHeader,"Storing CImHeader...");
_LIT(KStoringCImMimeHeader,"Storing CImMimeHeader...");
_LIT(KRestoringCImHeader,"Restoring CImHeader...");
_LIT(KCDrive,"c:\\");
_LIT(KImapMailStoreTest,"t_imapmailstore");
_LIT(KAttachmentDetails,"testattachment");
_LIT(KBodyDetails,"testbody");
_LIT(KCancellingAllRequests,"Cancelling all requests from for entry %d...");	
_LIT(KMakingRequestCImMimeHeader,"Making a request to store a CImMimeHeader in entry %d...");
_LIT(KOutstandingRequests,"Testing that there is %d requests outstanding...");
_LIT(KCancellingObserver,"Cancelling requests made by observer...");
_LIT(KMiutFrom,"ralph-greenwell@psion.com");
_LIT(KMiutSubject,"short mail about something interesting");
_LIT(KMiutReplyTo,"ralph.greenwell@virgin.net");
_LIT(KImailHeader6,"http:\\\\abc\\def\\hij\\klmn.html");
_LIT8(KImailHeader1,"TEXT");
_LIT8(KImailHeader9,"FILE");
_LIT8(KImailHeader2,"PLAIN");
_LIT8(KImailHeader3,"CHARSET");
_LIT8(KImailHeader4,"US-ASCII");
_LIT8(KMiutMsgId,"0123456789AB.CdeFGHIj@symbian.com");

// Note that all test suites must begin with "CTest"
class CTestImapMailStore : public  CActiveTestFixture, public MMailStoreObserver, public MChunkOutOfOrderObserver
	{
public:
	CTestImapMailStore();
	// SetUp and TearDown code (optional)
	virtual void SetupL();
	virtual void TearDownL();
	static CTestSuite* CreateSuiteL(const TDesC& aName);
	void StoreOperationComplete(TMsvId aId, TInt aErrorCode);
	// from MChunkOutOfOrderObserver
	void EnableSendFetch();
protected:
	void CreateMessageStructureL(TMsvId aId);
	void CreateCImHeaderL(CImHeader& aCImHeader);
	void CreateCImMimeHeaderL(CImMimeHeader& aCImMimeHeader);
protected:
	CImapMailStore* iImapMailStore;
	TInt iRequestCount;
	TMsvId iMsvMessageEntry;
	TMsvId iAttMessageEntry;
	TMsvId iBodyMessageEntry;
	TMsvId iFolderId;
	CMsvServerEntry* iServerEntry;
	CMsvServer*	iMsvServer;
	CImapSettings* iSettings;
	CFetchBodyInfo* iInfo;
	};
	
#endif //__T_IMAPMAILSTORE_H
