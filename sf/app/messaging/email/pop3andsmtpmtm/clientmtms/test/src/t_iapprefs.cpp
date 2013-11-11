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
// Name of test harness: T_IAPPREFS
// Component: IMCM
// Owner: KG
// Brief description of test harness:
// Tests IAP Preferences
// Detailed description of test harness:
// As above
// Input files required to run test harness:
// None
// Intermediate files produced while running test harness:
// None
// Output files produced by running test harness:
// <DRIVE>:\msglogs\T_IAPPREFS.<PLATFORM>.<VARIANT>.LOG.txt
// Description of how to build test harness:
// cd \msg\imcm\
// bldmake bldfiles
// abld test build
// Description of how to run test harness:
// WINS running instructions:
// 1. \epoc32\release\wins\<VARIANT>\T_IAPPREFS.exe can be used at the command prompt
// or executable can be run from Windows Explorer.
// All other platform running instructions:
// 1. Copy \epoc32\release\<PLATFORM>\<VARIANT>\T_IAPPREFS.exe onto the other platform
// 2. Run T_IAPPREFS.exe on the other platform
// 
//

#include "emailtestutils.h"
#include "t_iapprefs.h"
#include <e32test.h>
#include <msvids.h>

RTest test(_L("T_IAPP"));
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D CEmailTestUtils* testUtils;


CT_iapprefs* CT_iapprefs::NewLC()
	{
	CT_iapprefs* self = new (ELeave) CT_iapprefs();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CT_iapprefs::ConstructL()
	{
	iPref = CImIAPPreferences::NewLC();
	CleanupStack::Pop();
	}

CT_iapprefs::CT_iapprefs()
	{
	}

CT_iapprefs::~CT_iapprefs()
	{
	delete iPref;
	}

void CT_iapprefs::HeapTestL()
	{
#ifdef _DEBUG // stop compiler complaining that i isn't used in release builds.
	TInt i = 0;
#endif
	TInt err = KErrNone;
	do
		{
		__UHEAP_FAILNEXT(i++);

		TRAP(err, TestAllButStoreL());

		__UHEAP_RESET;
		} while (err == KErrNoMemory);

	User::LeaveIfError(err);

	}

void CT_iapprefs::TestL()
	{
	TestAllButStoreL();
	}

void CT_iapprefs::TestAllButStoreL()
	{
	TestVersionL();
	TestNoIAPsL();
	TestAddIAPL();
	TestRemoveIAPL();
	}

void CT_iapprefs::TestVersionL()
	{
	TestL(iPref->Version()==2);		// MLM 13/08/01. Changed to version 2 for Messaging 6.2.
	}


void CT_iapprefs::TestNoIAPsL()
	{
	PopulateChoicesL(0);
	TestL(iPref->NumberOfIAPs() == 0);
	PopulateChoicesL(2);
	TestL(iPref->NumberOfIAPs() == 2);
	}

void CT_iapprefs::TestAddIAPL()
	{
	PopulateChoicesL(4);
	for(TUint i = 0; i < 4; ++i)
		TestL(iPref->IAPPreference(i).iIAP==i);
	}

void CT_iapprefs::TestRemoveIAPL()
	{
	PopulateChoicesL(4);
	iPref->RemoveIAPL(0);
	TestL(iPref->NumberOfIAPs()==3);
	TUint i;
	for(i = 0; i < 3; ++i)
		TestL(iPref->IAPPreference(i).iIAP==i+1);

	iPref->RemoveIAPL(2); // the last one
	TestL(iPref->NumberOfIAPs()==2);
	for(i = 0; i < 2; ++i)
		TestL(iPref->IAPPreference(i).iIAP==i+1);
	}

void CT_iapprefs::PopulateChoicesL(TInt n)
	{
        for(;iPref->NumberOfIAPs() > 0; iPref->RemoveIAPL(0)) {};

	TestL(iPref->NumberOfIAPs()==0);
	for(TInt j = 0; j < n; ++j)
		{
		TImIAPChoice choice = {j,(TCommDbDialogPref)1};
		iPref->AddIAPL(choice, j);
		}
	}

void CT_iapprefs::TestL(TBool aCondition)
	{
	if(!aCondition) 
		User::Leave(KErrTestFailed);
	}

void doMainL()
	{
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install( scheduler );
	testUtils = CEmailTestUtils::NewLC(test);
	CT_iapprefs* iapp = CT_iapprefs::NewLC();

	testUtils->TestStart(1);
	iapp->TestL();
	testUtils->TestFinish(1);
	testUtils->TestStart(2);
	iapp->HeapTestL();
	testUtils->TestFinish(2);
	testUtils->TestHarnessCompleted();
	CleanupStack::PopAndDestroy(3);//iapp, scheduler, testUtils
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("T_IAPP Test CImIAPPreferences class\n"));
	theCleanup=CTrapCleanup::New();
	TRAPD(ret,doMainL());
	test(ret==KErrNone);
	delete theCleanup;	
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return (KErrNone);
	}
