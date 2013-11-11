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
// Test Help Model module
// 
//

// System includes
#include <e32test.h>
#include <f32file.h>

// User includes
#include "hlpmodel.h"

// Globals
static RFs					TheFs;
static RTest				TheTest(_L("TMODEL - Test Help Model API"));
static CTrapCleanup*		TheTrapCleanup;
static CActiveScheduler*	TheScheduler;

// Constants
const TInt KTestCleanupStack=0x20;


/**
@SYMTestCaseID PIM-TMODEL-0001 
*/

static void Test1L()
	{
	__UHEAP_MARK;
	TheTest.Next(_L("@SYMTestCaseID PIM-TMODEL-0001 Test 1: Test memory leaks in help model"));
	CHlpModel::NewLC(TheFs, NULL);
	CleanupStack::PopAndDestroy(); // model
	__UHEAP_MARKEND;

	__UHEAP_MARK;
	CHlpModel* model = CHlpModel::NewLC(TheFs, NULL);
	model->OpenL();
	CleanupStack::PopAndDestroy(); // model
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID PIM-TMODEL-0002
*/
	
static void Test2L()
	{
	__UHEAP_MARK;
	TheTest.Next(_L("@SYMTestCaseID PIM-TMODEL-0002 Test 2: Test category listing"));
	CHlpModel* model = CHlpModel::NewLC(TheFs, NULL);
	model->OpenL();

	TBuf<KHlpMaxTextColLength> entry;
	CDesCArray* catList = new(ELeave) CDesCArrayFlat(2);
	CleanupStack::PushL(catList);

	model->CategoryListL(catList);
	for (TInt i=0; i<catList->Count(); i++)
		{
		entry.Append(catList->MdcaPoint(i));
		TheTest.Printf(_L("\n%S\n"), &entry);
		}
	//TheTest.Console()->Getch();
	model->CloseL();
	CleanupStack::PopAndDestroy(2); // catList, model
	__UHEAP_MARKEND;
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
		Test1L();
		Test2L();
		)
	TheTest(r==KErrNone);

	delete TheScheduler;
	delete TheTrapCleanup;
	TheFs.Close();
	TheTest.Close();

	__UHEAP_MARKEND;
	return KErrNone;
    }
