// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "testframeutils.h"
#include "parsers.h"
#include "commands.h"
#include "msvtestutils.h"


//
//
// DLL Entry
//

//
//
// CActiveTestState
//

EXPORT_C CActiveTestState::CActiveTestState(TInt aPriority) : CActive(aPriority)
	{
	}

EXPORT_C TInt CActiveTestState::RunError(TInt aError)
	{
	User::RequestComplete(iReportStatus, aError);
	return KErrNone;
	}


//
//
// CBaseTestHarness
//

EXPORT_C CBaseTestHarness::~CBaseTestHarness()
	{
	iStateList->ResetAndDestroy();
	delete iStateList;
	}

EXPORT_C void CBaseTestHarness::StartL(TRequestStatus& aStatus)
	{
	TTestDebugInfo debugInfo = GetDebugInfo();
	HBufC* startString = debugInfo.TestStartStringLC();
	LogCommentL(*startString);
	TestConsole().Printf(*startString);
	TestConsole().Printf(_L("\n"));

	CleanupStack::PopAndDestroy(startString);
	iCurrentState = 0;
	iReportStatus = &aStatus;
	
	if (iCurrentState < iStateList->Count())
		{
		TTestDebugInfo moreInfo=(*iStateList)[iCurrentState]->GetDebugInfo();
		HBufC* debugString=moreInfo.TestStartStringLC();
		TestConsole().Printf(*debugString);
		TestConsole().Printf(_L("\r"));
		CleanupStack::PopAndDestroy(debugString);

		(*iStateList)[iCurrentState]->StartL(iStatus);
		}

	aStatus = KRequestPending;
	SetActive();
	}


EXPORT_C void CBaseTestHarness::AddStateL(MBaseTestState* aTestState, TTestDebugInfo aDebugInfo)
	{
	aTestState->SetDebugInfo(aDebugInfo);
	TRAPD(err, iStateList->AppendL(aTestState));
	if (err != KErrNone)
		// If the state can't be added then delete it.
		// We do this because we have taken ownership of the state.
		{
		delete aTestState;
		User::Leave(err);
		}
	}


// From CActive
EXPORT_C void CBaseTestHarness::RunL()
	{
	if (iStatus == KErrNone)
		{
		iCurrentState++;
		if (iCurrentState < iStateList->Count())
			{
			TTestDebugInfo moreInfo=(*iStateList)[iCurrentState]->GetDebugInfo();
			HBufC* debugString=moreInfo.TestCompleteStringLC();
			TestConsole().Printf(*debugString);
			TestConsole().Printf(_L("\r"));
			CleanupStack::PopAndDestroy(debugString);

			(*iStateList)[iCurrentState]->StartL(iStatus);
			SetActive();
			}
		else
			{
			TTestDebugInfo debugInfo = GetDebugInfo();
			HBufC* testCompleted = debugInfo.TestCompleteStringLC();
			LogCommentL(*testCompleted);

			CleanupStack::PopAndDestroy(testCompleted);
			User::RequestComplete(iReportStatus, iStatus.Int());
			}
		}
	else
		{
		// Pass the debug info back from the child state and log the error
		TTestDebugInfo debugInfo = (*iStateList)[iCurrentState]->GetDebugInfo();
		SetDebugInfo(debugInfo);
		HBufC* errorText = debugInfo.TestFailedStringL(iStatus.Int());
		CleanupStack::PushL(errorText);
		LogCommentL(*errorText);
		TestConsole().Printf(*errorText);
		TestConsole().Printf(_L("\r"));
							  
		CleanupStack::PopAndDestroy(errorText);
		User::RequestComplete(iReportStatus, iStatus.Int());
		}
	}

EXPORT_C void CBaseTestHarness::DoCancel()
	{
	}

EXPORT_C void CBaseTestHarness::LogCommentL(const TDesC& /* aError */)
	{
	}

EXPORT_C TInt CBaseTestHarness::RunError(TInt aError)
	{
	// Pass the debug info back from the child state and log the error
	TTestDebugInfo debugInfo = (*iStateList)[iCurrentState]->GetDebugInfo();
	SetDebugInfo(debugInfo);
	aError = KErrNone;
	HBufC* errorText = NULL;
	TRAP(aError,
		{
		errorText=debugInfo.TestFailedStringL(aError);
		LogCommentL(*errorText);
		});
		
	TestConsole().Printf(*errorText);
	TestConsole().Printf(_L("\r"));

	delete errorText;
	User::RequestComplete(iReportStatus, aError);
	return aError;
	}

EXPORT_C void CBaseTestHarness::ConstructL()
	{
	CActiveScheduler::Add(this);
	iStateList = new (ELeave) CArrayPtrFlat<MBaseTestState>(10);
	}

EXPORT_C CBaseTestHarness::CBaseTestHarness(RTest& aTest) : CActive(EPriorityHigh), iTest(aTest)
	{
	}

EXPORT_C RTest& CBaseTestHarness::TestConsole()
	{
	return iTest;
	}

EXPORT_C void CBaseTestHarness::SetHeapFailureTesting(TBool aTest)
	{
	iTestHeapFailure = aTest;
	}

//
//
// CMainTestHarness
//

EXPORT_C CMainTestHarness* CMainTestHarness::NewL(RTest& aTest)
	{
	CMainTestHarness* self = new (ELeave) CMainTestHarness(aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CMainTestHarness::ConstructL()
	{
	CBaseTestHarness::ConstructL();
	}

EXPORT_C CMainTestHarness::CMainTestHarness(RTest& aTest) : CBaseTestHarness(aTest)
	{
	}

EXPORT_C void CMainTestHarness::StartL(TRequestStatus& aStatus)
	{
	CBaseTestHarness::StartL(aStatus);
	}


//
//
// CMsvClientTest
//


EXPORT_C CMsvClientTest* CMsvClientTest::NewL(CMsvTestUtils& aTestUtils, RTest& aTest)
	{
	CMsvClientTest* self = new (ELeave) CMsvClientTest(aTestUtils, aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CMsvClientTest* CMsvClientTest::NewL(CMsvTestUtils* aTestUtils, RTest& aTest)
	{
	CMsvClientTest* self = new (ELeave) CMsvClientTest(aTestUtils, aTest);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CMsvClientTest::~CMsvClientTest()
	{
	Cancel();
	delete iCurrentSelection;
	delete iOperation;
	}

EXPORT_C void CMsvClientTest::DoCancel()
	{
	if (iOperation)
		{
		iOperation->Cancel();
		}
	}

EXPORT_C void CMsvClientTest::SetCurrentOperation(CMsvOperation* aOperation)
	{
	delete iOperation;
	iOperation = aOperation;
	}


EXPORT_C CMsvOperation& CMsvClientTest::CurrentOperation()
	{
	return *iOperation;
	}

EXPORT_C void CMsvClientTest::ConstructL()
	{
	CBaseTestHarness::ConstructL();
	iCurrentSelection = new (ELeave) CMsvEntrySelection;
	}

EXPORT_C CMsvClientTest::CMsvClientTest(CMsvTestUtils& aMsvTestUtils, RTest& aTest) 
: CBaseTestHarness(aTest), 
  iMsvTestUtils(aMsvTestUtils)
	{
	iOperation = NULL;
	}

EXPORT_C CMsvClientTest::CMsvClientTest(CMsvTestUtils* aMsvTestUtils, RTest& aTest) 
: CBaseTestHarness(aTest), 
  iMsvTestUtils(*aMsvTestUtils),
  iOwnedMsvTestUtils(aMsvTestUtils)  
	{
	iOperation = NULL;
	}

EXPORT_C CMsvTestUtils& CMsvClientTest::MsvTestUtils()
	{
	return iMsvTestUtils;
	}

// MMS Test Harness: added to enable derived classes to reset data
EXPORT_C void CMsvClientTest::Reset()
	{
	iMsvTestUtils.Reset();
	}
	
EXPORT_C void MBaseTestState::SetDebugInfo(const TTestDebugInfo& aDebugInfo) 
{
iDebugInfo = aDebugInfo;
}

EXPORT_C const TTestDebugInfo& MBaseTestState::GetDebugInfo() const 
{
return iDebugInfo;
}

EXPORT_C MBaseTestState::~MBaseTestState()
{
}