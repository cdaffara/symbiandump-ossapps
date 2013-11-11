// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "ObexHarness.h"
#include "obextestutils.h"

CObexClientTest* CObexClientTest::NewL(CObexTestUtils& aTestUtils, RTest& aTest)
    {
    CObexClientTest* self = new (ELeave) CObexClientTest(aTestUtils, aTest);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }


CObexClientTest* CObexClientTest::NewL(CObexTestUtils* aTestUtils, RTest& aTest)
    {
    CObexClientTest* self = new (ELeave) CObexClientTest(aTestUtils, aTest);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }


CObexClientTest::CObexClientTest(CObexTestUtils& aTestUtils, RTest& aTest)
: CMsvClientTest(aTestUtils, aTest),
  iObexTestUtils(aTestUtils),
  iSessionReset(EFalse),
  iIgnoreSendErrors(EFalse)
    {
	iAllTestPassed = ETrue;
    }


CObexClientTest::CObexClientTest(CObexTestUtils* aTestUtils, RTest& aTest)
: CMsvClientTest(*aTestUtils, aTest),
  iObexTestUtils(*aTestUtils),
  iOwnedObexTestUtils(aTestUtils),
  iSessionReset(EFalse),
  iIgnoreSendErrors(EFalse)
    {
	iAllTestPassed = ETrue;
    }

void CObexClientTest::ConstructL()
    {
    CMsvClientTest::ConstructL();

	// Create the headers lists.
	iCurrentHeaders = CObexHeaderList::NewL();
	iVerifyHeaders = CObexTestHeaderList::NewL();
    }

CObexClientTest::~CObexClientTest()
    {
	// All test states passed, log tests completed OK.
	if (iAllTestPassed)
		{
		ObexTestUtils().TestHarnessCompleted();
		}

	delete iOwnedObexTestUtils;
    delete iSendAs;
	delete iCurrentHeaders;
	delete iVerifyHeaders;
	
	iAttachmentsList.ResetAndDestroy();
    }

void CObexClientTest::LogCommentL(const TDesC& aError)
    {
    iObexTestUtils.WriteComment(aError);
    }

void CObexClientTest::Reset()
    {
	iObexTestUtils.Reset();
	iSessionReset = ETrue;
    }

/**
 * Creates the CSendAs object and its associated message.
 */
void CObexClientTest::CreateSendAsL() 
    {
	// If a SendAs object's still around, delete it.
    if (iSendAs)
        {
    	delete iSendAs;
        iSendAs = 0;
        }
	
	// create the member variable
	iSendAs = CSendAs::NewL(*this);
	
	// set it to always use the correct MTM
	iSendAs->SetMtmL(ObexTestUtils().CurrentObexMtm());
	
	// set it to use the correct service (the only one in its array
	// of available services)
	iSendAs->SetService(0);

	// create the SendAs object's associated message
	iSendAs->CreateMessageL();
    iObexTestUtils.SetEntryL(iSendAs->MessageId());
    }


void CObexClientTest::RunL()
	{
	CBaseTestHarness::RunL();
        
	// Clear flag for a failed test state.
	if (iStatus.Int() != KErrNone && iStatus.Int() != KRequestPending)
		{
		iAllTestPassed = EFalse;
		}
	}

void CObexClientTest::ClearCurrentHeadersL()
	{
	// To clear the headers, just delete and re-create the list,
	// the list will delete each of the headers.
	if (iCurrentHeaders)
		{
		delete iCurrentHeaders;
		iCurrentHeaders = NULL;

		iCurrentHeaders = CObexHeaderList::NewL();
		}
	}

void CObexClientTest::ClearVerifyHeadersL()
	{
	// To clear the headers, just delete and re-create the list,
	// the list will delete each of the headers.
	if (iVerifyHeaders)
		{
		delete iVerifyHeaders;
		iVerifyHeaders = NULL;

		iVerifyHeaders = CObexTestHeaderList::NewL();
		}
	}

void CObexClientTest::ReleaseCurrentHeaders()
	{
	// This must only be called after iCurrentHeaders has been handed over
	// via CurrentHeaders().
	iCurrentHeaders = NULL;

	// Create a new set of headers.
	iCurrentHeaders = CObexHeaderList::NewL();
	}


void CObexClientTest::ClearAttachmentsL()
	{
	iAttachmentsList.ResetAndDestroy();
	}
