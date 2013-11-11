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

#include "t_imapmailstoreheaders.h"

/**
@SYMTestCaseID MSG-IMAP-MAILSTORE-0001
@SYMTestType UT
@SYMTestPriority High
@SYMPREQ 1160
@SYMTestCaseDesc Tests storage of Header using CImapMailStore.
@SYMTestActions  1. Creates a list of index entries into which header will be stored.
				 2. Creates a CImHeader and a CImMimeHeader populated with test values.
				 3. Calls CImapMailStore to store the headers in the message store.
				 4. Uses CMsvStore to read the headers back.
				 5. Tests the headers were stored correctly.

@SYMTestExpectedResults Contents of headers should match the test values they were populated with.
*/

_LIT(KTestingCImMimeHeader,"Testing Storage of CImMimeHeader...");
_LIT(KTestingCImHeader,"Testing Storage of CImHeader...");

CTestImapMailStoreHeaders::CTestImapMailStoreHeaders()
	{	
	}
	
/**
Calls the base classes SetupL() to clean the message store and initialise test.
Creates CImHeader and CImMimeHeader objects.
*/
void CTestImapMailStoreHeaders::SetupL()
//virtual 
	{
	CTestImapMailStore::SetupL();	
	CImHeader* header = CImHeader::NewLC(); 
	CreateCImHeaderL(*header);
	CleanupStack::Pop(header); 
	++iRequestCount;
	INFO_PRINTF1(KStoringCImHeader);
	iImapMailStore->StorePartL(header,iMsvMessageEntry,*this);
	
	CImMimeHeader* mimeHeader = CImMimeHeader::NewLC(); 
	CreateCImMimeHeaderL(*mimeHeader);
	CleanupStack::Pop(mimeHeader); 
	INFO_PRINTF1(KStoringCImMimeHeader);
	iImapMailStore->StorePartL(mimeHeader,iAttMessageEntry,*this);
	++iRequestCount;
	CActiveScheduler::Start();	
	}

/**
Restores and check the values of a CImHeader
*/
void CTestImapMailStoreHeaders::TestCImHeaderL()
	{
	INFO_PRINTF1(KTestingCImHeader);
	INFO_PRINTF1(KRestoringCImHeader);
	iServerEntry->SetEntry(iMsvMessageEntry);	
	CMsvStore* store=iServerEntry->ReadStoreL();
	CleanupStack::PushL(store);
	CImHeader* header = CImHeader::NewLC();	
	header->RestoreL(*store);

	TPtrC tempPtr(KMiutSubject);	
	ASSERT_EQUALS(header->Subject(),tempPtr);	
	tempPtr.Set(KMiutFrom);
	ASSERT_EQUALS(header->From(),tempPtr);
	
	TPtrC8 tempPtr8(KMiutMsgId);
	ASSERT_EQUALS(header->ImMsgId(),tempPtr8);
	tempPtr.Set(KMiutReplyTo);
	ASSERT_EQUALS(header->ReplyTo(),tempPtr);
	CleanupStack::PopAndDestroy(2,store);
	}
	
/**
Restores and check the values of a CImMimeHeader
*/
void CTestImapMailStoreHeaders::TestCImMimeHeaderL()
	{
	
	
	INFO_PRINTF1(KTestingCImMimeHeader);
	INFO_PRINTF1(KRestoringCImMimeHeader);
	iServerEntry->SetEntry(iAttMessageEntry);
	CMsvStore* store=iServerEntry->ReadStoreL();
	CleanupStack::PushL(store);
	CImMimeHeader* header = CImMimeHeader::NewLC();	
	header->RestoreL(*store);
	
	TPtrC8 tempPtr8(KImailHeader1);
	ASSERT_EQUALS(header->ContentType(),tempPtr8);
	ASSERT_EQUALS(header->ContentDescription(),tempPtr8);
	ASSERT_EQUALS(header->ContentDescription(),tempPtr8);
	
	tempPtr8.Set(KImailHeader9);
	ASSERT_EQUALS(header->ContentDisposition(),tempPtr8);
	ASSERT_EQUALS(header->ContentDisposition(),tempPtr8);
	
	tempPtr8.Set(KImailHeader4);
	ASSERT_EQUALS(header->ContentID(),tempPtr8);
	
	tempPtr8.Set(KImailHeader3);
	ASSERT_EQUALS(header->ContentBase(),tempPtr8);	

	TPtrC tempPtr(KImailHeader6);
	ASSERT_EQUALS(header->ContentLocation(),tempPtr);

	CleanupStack::PopAndDestroy(2,store);
	}

CTestSuite* CTestImapMailStoreHeaders::CreateSuiteL(const TDesC& aName)
// static
	{
	SUB_SUITE;
	ADD_ASYNC_TEST_STEP(TestCImHeaderL);
	ADD_ASYNC_TEST_STEP(TestCImMimeHeaderL);
	END_SUITE;
	}
