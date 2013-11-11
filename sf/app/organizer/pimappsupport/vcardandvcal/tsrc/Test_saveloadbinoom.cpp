// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 
#include <s32mem.h>
#include <e32test.h>
#include <vprop.h>
#include <versit.h>
#include <vcal.h>
#include <vcard.h>
#include <f32file.h>
#include <s32file.h>
#include "testdata.h"

// Type definitions
#define UNUSED_VAR(a) a = a

CConsoleBase* console;
LOCAL_D RTest	test(_L("TEST_SAVELOADBINOOM"));



void doTestL()
	{
	console=Console::NewL(_L("TEST_SAVELOADBINOOM"),TSize(76,18));
	CleanupStack::PushL(console);
	for (TInt ii=0;ii<10000;++ii)
		{
			CleanupStack::PushL(&ii);
		}
	CleanupStack::Pop(10000);
	
	RDesReadStream input1(vcardbinary);
    CParserVCard* vcard1 = CParserVCard::NewL();
	CleanupStack::PushL(vcard1);
    vcard1->SetEntityNameL(_L("vcard"));
	vcard1->InternalizeL(input1);
	
	TInt ret=KErrNoMemory;
	TInt failAt=0;	  
	while (ret!=KErrNone)
		{
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAP(ret, vcard1->SaveBinaryValuesToFilesL(0,_L("c:\\")));
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			}
		__UHEAP_RESET;
		if (ret!=KErrNoMemory&&ret!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),ret);
			test.Getch();
			}
		test(ret==KErrNoMemory||ret==KErrNone);
		}
	
	while (ret!=KErrNone)
		{
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAP(ret, vcard1->LoadBinaryValuesFromFilesL());
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			}
		__UHEAP_RESET;
		if (ret!=KErrNoMemory&&ret!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),ret);
			test.Getch();
			}
		test(ret==KErrNoMemory||ret==KErrNone);
		}
		
	CleanupStack::PopAndDestroy(vcard1);
	
	RDesReadStream input2(vcardagent);
    CParserVCard* vcard2 = CParserVCard::NewL();
	CleanupStack::PushL(vcard2);
    vcard2->SetEntityNameL(_L("vcard"));
	vcard2->InternalizeL(input2);
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);

	ret=KErrNoMemory;
	failAt=0;	  
	while (ret!=KErrNone)
		{
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAP(ret, vcard2->SaveBinaryValuesToFilesL(0,_L("c:\\"),fs));
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			}
		__UHEAP_RESET;
		if (ret!=KErrNoMemory&&ret!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),ret);
			test.Getch();
			}
		test(ret==KErrNoMemory||ret==KErrNone);
		}
	
	while (ret!=KErrNone)
		{
		failAt++;
		__UHEAP_SETFAIL(RHeap::EDeterministic,failAt);
		__UHEAP_MARK;			
		TRAP(ret, vcard2->LoadBinaryValuesFromFilesL(fs));
		if (ret!=KErrNone)
			{
			__UHEAP_MARKEND;
			}
		__UHEAP_RESET;
		if (ret!=KErrNoMemory&&ret!=KErrNone)
			{
			test.Printf(_L("Non standard error: %d\n"),ret);
			test.Getch();
			}
		test(ret==KErrNoMemory||ret==KErrNone);
		}
	
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy(vcard2);
    CleanupStack::PopAndDestroy(console);
	}
	
/**
@SYMTestCaseID PIM-TESTSAVELOADBINOOM-0001
*/
TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTestCaseID PIM-TESTSAVELOADBINOOM-0001 TEST_SAVELOADBINOOM"));
	test.Title();
	CTrapCleanup* cleanup = CTrapCleanup::New();
	TRAPD(error,doTestL());
	UNUSED_VAR(error); //Used to supress build warnings
	
	ASSERT(error == KErrNone);
	delete cleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return (KErrNone);
	}
