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
LOCAL_D RTest	test(_L("TEST_SAVELOADBIN"));



void doTestL()
	{
	console=Console::NewL(_L("TEST_SAVELOADBIN"),TSize(76,18));
	CleanupStack::PushL(console);
// tests a vcard without  agents 
	RDesReadStream input1(vcardbinary);
    CParserVCard* vcard1 = CParserVCard::NewL();
	CleanupStack::PushL(vcard1);
    vcard1->SetEntityNameL(_L("vcard"));
	vcard1->InternalizeL(input1);
	TInt numfiles = vcard1->SaveBinaryValuesToFilesL(0,_L("c:\\"));
	ASSERT(numfiles == 1);
	numfiles =  vcard1->LoadBinaryValuesFromFilesL();
	ASSERT(numfiles == 1);
	CleanupStack::PopAndDestroy(vcard1);
// tests a vcard with  agents 
	RDesReadStream input2(vcardagent);
    CParserVCard* vcard2 = CParserVCard::NewL();
	CleanupStack::PushL(vcard2);
    vcard2->SetEntityNameL(_L("vcard"));
	vcard2->InternalizeL(input2);
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);
	// the overloaded member functions that accept a RFs param is used
	numfiles = vcard2->SaveBinaryValuesToFilesL(0,_L("c:\\"),fs);
	ASSERT(numfiles == 1);
	numfiles =  vcard2->LoadBinaryValuesFromFilesL(fs);
	ASSERT(numfiles == 1);
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy(vcard2);
// tests a vcard with a binary data field that is empty 
	RDesReadStream input3(vcardbinaryempty);
    CParserVCard* vcard3 = CParserVCard::NewL();
	CleanupStack::PushL(vcard3);
    vcard3->SetEntityNameL(_L("vcard"));
	vcard3->InternalizeL(input3);
	fs.Connect();
	CleanupClosePushL(fs);
	numfiles = vcard3->SaveBinaryValuesToFilesL(0,_L("c:\\"),fs);
	ASSERT(numfiles == 0);
	numfiles = vcard3->LoadBinaryValuesFromFilesL(fs);
	ASSERT(numfiles == 0);
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy(vcard3);

    CleanupStack::PopAndDestroy(console);
	}

/**
@SYMTestCaseID PIM-TESTSAVELOADBIN-0001
*/	
TInt E32Main()
	{
	__UHEAP_MARK;
	test.Start(_L("@SYMTestCaseID PIM-TESTSAVELOADBIN-0001 TEST_SAVELOADBIN"));
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
