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

#include "t_imapmailstorecancelingrequests.h"
#include "t_testobserver.h"

/**
@SYMTestCaseID MSG-IMAP-MAILSTORE-0005
@SYMTestType UT
@SYMTestPriority High
@SYMPREQ 1160
@SYMTestCaseDesc Tests cancelling of storage requests made to CImapMailStore based on observer and TMsvId.
@SYMTestActions  1. Creates a list of index entries into which the body and its CImMimeHeader header will be stored.
				 2. Makes requests to store headers with different TMsvId's and tests the number of outstanding requests.
				 3. Cancels requests for one of the TMsvId's and tests the number of outstanding requests.
				 4. Makes requests to store a header using another observer.
				 5. Cancels all requests for one of the observers and tests the number of outstanding requests.
				 
@SYMTestExpectedResults The outstanding requests should be correctly updated when requests are cancelled.
*/

CTestImapMailStoreCancelingRequests::CTestImapMailStoreCancelingRequests()
	{
	}

/**
Calls the base classes SetupL() to clean the message store and initialise test.
*/
void CTestImapMailStoreCancelingRequests::SetupL()
//virtual 
	{
	CTestImapMailStore::SetupL();
	}

void CTestImapMailStoreCancelingRequests::TestCancelingRequestsL()
	{

	INFO_PRINTF2(KMakingRequestCImMimeHeader,iBodyMessageEntry);
	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader);
	CleanupStack::Pop(mimeHeader); 
	iImapMailStore->StorePartL(mimeHeader,iBodyMessageEntry,*this);
	++iRequestCount;
	INFO_PRINTF2(KOutstandingRequests,iImapMailStore->RequestCount());
	ASSERT_EQUALS(iImapMailStore->RequestCount(),iRequestCount);

	INFO_PRINTF2(KMakingRequestCImMimeHeader,iBodyMessageEntry);
	CImMimeHeader* mimeHeader2 = CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader2);
	CleanupStack::Pop(mimeHeader2); 
	iImapMailStore->StorePartL(mimeHeader2,iBodyMessageEntry,*this);
	++iRequestCount;
	INFO_PRINTF2(KOutstandingRequests,iImapMailStore->RequestCount());
	ASSERT_EQUALS(iImapMailStore->RequestCount(),iRequestCount);

	INFO_PRINTF2(KMakingRequestCImMimeHeader,iAttMessageEntry);	
	CImMimeHeader* mimeHeader3 = CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader3);
	CleanupStack::Pop(mimeHeader3); 
	iImapMailStore->StorePartL(mimeHeader3,iAttMessageEntry,*this);
	++iRequestCount;
	INFO_PRINTF2(KOutstandingRequests,iImapMailStore->RequestCount());
	ASSERT_EQUALS(iImapMailStore->RequestCount(),iRequestCount);
	
	//update iRequestCount
	iRequestCount-=2;

	INFO_PRINTF2(KCancellingAllRequests,iBodyMessageEntry);
	iImapMailStore->CancelRequest(iBodyMessageEntry);
	INFO_PRINTF2(KOutstandingRequests,iRequestCount);
	ASSERT_EQUALS(iImapMailStore->RequestCount(),iRequestCount);	
	
	//make a request using CTestObserver, this is to test case where multiple observers are uing the API.
	CTestObserver* testObserver = new (ELeave)CTestObserver(*iImapMailStore);
	CleanupStack::PushL(testObserver);
	testObserver->MakeRequestL(iBodyMessageEntry);
	++iRequestCount;
	INFO_PRINTF2(KOutstandingRequests,iImapMailStore->RequestCount());
	ASSERT_EQUALS(iImapMailStore->RequestCount(),iRequestCount);
	INFO_PRINTF1(KCancellingObserver);
	iImapMailStore->CancelRequest(*this);
	
	//update iRequestCount
	--iRequestCount;
	
	INFO_PRINTF2(KOutstandingRequests,iRequestCount);
	ASSERT_EQUALS(iImapMailStore->RequestCount(),iRequestCount);
	
	testObserver->CancelL();	
	
	CleanupStack::PopAndDestroy(testObserver); 
	}


CTestSuite* CTestImapMailStoreCancelingRequests::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestCancelingRequestsL);
	END_SUITE;
	}
