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
// Test Context module
// 
//

#pragma message(__FILE__ "(1) : NOTE: Requires TContext.dat in the help directory")

// System includes
#include <e32test.h>
#include <f32file.h>
#include <coehelp.h>

// User includes
#include "hlpmodel.h"

// Globals
static RFs					TheFs;
static RTest				TheTest(_L("TCONTEXT - Test context sensitive searching in help model"));
static CTrapCleanup*		TheTrapCleanup;
static CActiveScheduler*	TheScheduler;

// Constants
const TInt KTestCleanupStack=0x20;


class CHelpTester : public CBase, public MHlpModelObserver
	{
public:
	CHelpTester();
	~CHelpTester();
	void ConstructL();

public:
	void Test1L();

public: // from MHlpModelObserver
	void HandleModelEventL(TInt aEvent);

private:
	TInt iLastResponse;
	CHlpModel* iModel;
	};


CHelpTester::CHelpTester()
	{
	}

CHelpTester::~CHelpTester()
	{
	delete iModel;
	}

void CHelpTester::ConstructL()
	{
	iModel = CHlpModel::NewL(TheFs, this);
	}

/**
@SYMTestCaseID PIM-TCONTEXT-0001
*/	
void CHelpTester::Test1L()
	{
	TheTest.Next(_L("@SYMTestCaseID PIM-TCONTEXT-0001 Test 1: Test category searching"));
	
	iModel->OpenL();

	// Create a context that we know exists
	TCoeContextName contextName(_L("A_Remove_dialog"));
	TUid contextUid = {268450396};
	TCoeHelpContext context(contextUid, contextName);

	// Request a search on the context
	iModel->ContextSearchL(context);
	
	// Check that the response is "Found"
	TheTest(iLastResponse == ETopicAvailable);

	// Now get the topic text
	CHlpTopic* topic = CHlpTopic::NewLC();
	TRAPD(error, iModel->LoadTopicL(topic));
	TheTest(error == KErrNone);
	CleanupStack::PopAndDestroy(); // topic
	
	// Create a context that we know doesn't exists
	contextName = _L("SomeSymbianTopic");
	contextUid.iUid = 123456789;
	TCoeHelpContext missingContext(contextUid, contextName);

	// Request a search on the context
	iModel->ContextSearchL(missingContext);
	
	// Check that the response is "Found"
	TheTest(iLastResponse == ETopicNotFound);
	
	iModel->CloseL();
	}

void CHelpTester::HandleModelEventL(TInt aEvent)
	{
	// Just store the last event reported by the model so that we
	// can test against it in the actual test.
	iLastResponse = aEvent;
	}






static void DoTestL()
	{
	CHelpTester* tester = new(ELeave) CHelpTester();
	CleanupStack::PushL(tester);
	tester->ConstructL();
	tester->Test1L();
	CleanupStack::PopAndDestroy();
	}

static void setupFileServerAndSchedulerL()
//
// Initialise the cleanup stack.
//
	{
	TheTest(TheFs.Connect() == KErrNone);
	TheScheduler = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install(TheScheduler);
	}


static void setupCleanup()
//
// Initialise the cleanup stack.
//
    {
	TheTrapCleanup = CTrapCleanup::New();
	TheTest(TheTrapCleanup!=NULL);
	TRAPD(r,\
		{\
		for (TInt i=KTestCleanupStack;i>0;i--)\
			CleanupStack::PushL((TAny*)0);\
		CleanupStack::Pop(KTestCleanupStack);\
		});
	TheTest(r==KErrNone);
	}

GLDEF_C TInt E32Main()
//
// Test Help Model API
//
    {
	__UHEAP_MARK;

	TheTest.Title();
	setupCleanup();

	TRAPD(r, 
			setupFileServerAndSchedulerL();
			DoTestL();
		);
	TheTest(r==KErrNone);

	delete TheScheduler;
	delete TheTrapCleanup;
	TheFs.Close();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
    }
