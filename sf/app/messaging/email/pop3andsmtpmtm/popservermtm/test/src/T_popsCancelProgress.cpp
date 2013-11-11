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
// Name of test harness: T_PopsCancelProgress
// Component: POPS
// Brief description of test harness:
// Tests that a cancelling a CImTextServerSession connect operation leaves the
// object in a consistent state, allowing it's progress to be queried safely
// after cancelling.
// Detailed description of test harness:
// See "INC040630 - AV20 Panic while disconnecting a service"
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_PopsCancelProgress.<PLATFORM>.<VARIANT>.LOG.txt
// 
//

#include "emailtestutils.h"
#include "scripttestutils.h"
#include "miutlog.h"
#include "miutmsg.h"
#include <txtrich.h>
#include <popsmtm.h>
#include <cemailaccounts.h>

//------------------------------------------------------------------------------

class CPopsCancelProgressTest : public CBase
	{
public:
	static CPopsCancelProgressTest* NewLC(CEmailTestUtils& aTestUtils);
	void StartTestL();
	void InitL();
	void ResetL();
	void SetUpPopServiceL();
	void EndTestL();
	void EndTestHarnessL();

	~CPopsCancelProgressTest();

private:
	CPopsCancelProgressTest(CEmailTestUtils& aTestUtils);
	void ConstructL();
	void ResetMtmL();

public:
	CMsvEntrySelection* iSelection;
	CEmailTestUtils&	iTestUtils;

private:
	TInt				iTestError;
	TInt				iTestNumber;
	TInt				iGlobalError;
	TMsvId				iPop3Service;
	};

LOCAL_D CTrapCleanup* theCleanup;
_LIT(KImcmTest, "T_PopsCancelProgress Test");
RTest test(KImcmTest);

//------------------------------------------------------------------------------

LOCAL_C void doMainL()
	{
	CActiveScheduler* scheduler;
	scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);

	CEmailTestUtils* testUtils = CEmailTestUtils::NewLC(test);
	CPopsCancelProgressTest* popsTestHarness = CPopsCancelProgressTest::NewLC(*testUtils);

	popsTestHarness->SetUpPopServiceL();
	popsTestHarness->ResetL();
	popsTestHarness->StartTestL();
	popsTestHarness->EndTestL();
	popsTestHarness->EndTestHarnessL();

	CleanupStack::PopAndDestroy(3); // popsTestHarness, testUtils, scheduler
	}

//------------------------------------------------------------------------------

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("T_PopsCancelProgress Test"));

	theCleanup = CTrapCleanup::New();

	TRAPD(ret,doMainL());		

	test(ret == KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();

	__UHEAP_MARKEND;
	User::Heap().Check();
	return(KErrNone);
	}

//------------------------------------------------------------------------------

CPopsCancelProgressTest* CPopsCancelProgressTest::NewLC(CEmailTestUtils& aTestUtils)
	{
	CPopsCancelProgressTest* self = new (ELeave) CPopsCancelProgressTest(aTestUtils);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

//------------------------------------------------------------------------------

void CPopsCancelProgressTest::ResetMtmL()
	{
	// Get server MTM.
	iTestUtils.Reset();
	iTestUtils.CleanMessageFolderL();
	iTestUtils.GoServerSideL();
	SetUpPopServiceL();
	iTestUtils.InstantiatePopServerMtmL();
	}

//------------------------------------------------------------------------------

void CPopsCancelProgressTest::ResetL()
	{
	ResetMtmL();
	}

//------------------------------------------------------------------------------

void CPopsCancelProgressTest::EndTestL()
	{
	if ((iTestError != KErrNone) && (iGlobalError == KErrNone))
		{
		iGlobalError = iTestError;
		}

	iTestUtils.TestFinish(iTestNumber, iTestError);
	}

//------------------------------------------------------------------------------

void CPopsCancelProgressTest::EndTestHarnessL()
	{
	if (iGlobalError == KErrNone)
		{
		iTestUtils.TestHarnessCompleted();
		}
	else
		{
		iTestUtils.TestHarnessFailed(iGlobalError);
		}
	}

//------------------------------------------------------------------------------

void CPopsCancelProgressTest::InitL()
	{
	iTestUtils.FileSession().SetSessionPath(_L("C:\\"));
	iTestUtils.FileSession().Delete(_L("c:\\logs\\email\\imsk110.scr"));
	iTestUtils.CleanMessageFolderL();
	iTestUtils.GoServerSideL();
	}

//------------------------------------------------------------------------------

void CPopsCancelProgressTest::ConstructL()
	{
	InitL();
	iSelection = new (ELeave) CMsvEntrySelection;
	}

//------------------------------------------------------------------------------

CPopsCancelProgressTest::~CPopsCancelProgressTest()
	{
	delete iSelection;
	}

//------------------------------------------------------------------------------

CPopsCancelProgressTest::CPopsCancelProgressTest(CEmailTestUtils& aTestUtils) 
:	iTestUtils(aTestUtils), 
	iGlobalError(KErrNone)
{}

//------------------------------------------------------------------------------

void CPopsCancelProgressTest::StartTestL()
	{
	TRequestStatus mtmStatus;
	TBuf8<128> parameter;

	iTestUtils.Printf(_L("Starting POP Server Mtm Cancel/Progress Test.\n"));
	iTestUtils.Printf(_L("Putting the POP Server Mtm into the EPopConnecting state.\n"));
	iTestUtils.iPopServerMtm->StartCommandL(
		*iSelection, KPOP3MTMConnect, parameter, mtmStatus);
	iTestUtils.Printf(_L("Cancelling the POP Server Mtm EPopConnecting operation.\n"));
	iTestUtils.iPopServerMtm->Cancel();
	iTestUtils.Printf(_L("Querying the POP Server Mtm progress...\n"));
	iTestUtils.iPopServerMtm->Progress();
	}

//------------------------------------------------------------------------------

void CPopsCancelProgressTest::SetUpPopServiceL()
	{
	iTestUtils.GoClientSideL();

	// Create the POP3 service and set the settings for this test.
	iPop3Service = iTestUtils.CreatePopServiceL();
	iSelection->AppendL(iPop3Service);

	CImPop3Settings* settings = new(ELeave) CImPop3Settings();
	CleanupStack::PushL(settings);
	settings->Reset();
	_LIT(KPopServer, "pop3.demon.co.uk");
	settings->SetServerAddressL(KPopServer);
	settings->SetLoginNameL(iTestUtils.MachineName());
	settings->SetPasswordL(iTestUtils.MachineName());
	settings->SetPort(110);
	settings->SetDisconnectedUserMode(ETrue);

	CEmailAccounts* accounts = CEmailAccounts::NewLC();

	TPopAccount id;
	accounts->GetPopAccountL(iPop3Service, id);
	accounts->SavePopSettingsL(id, *settings);

	CleanupStack::PopAndDestroy(2, settings); // accounts/store, settings
	iTestUtils.GoServerSideL();
	}

//------------------------------------------------------------------------------

