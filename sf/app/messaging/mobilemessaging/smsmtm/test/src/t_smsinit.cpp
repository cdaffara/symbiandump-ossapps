// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <e32std.h>
#include "t_smsinit.h"

RTest test(_L("T_SmsInit Testrig"));
CTrapCleanup* theCleanup;

LOCAL_C void doMainL()
	{
	CActiveScheduler* iScheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(iScheduler);
	CActiveScheduler::Install( iScheduler );

	CSmsInit* SmsTest = CSmsInit::NewL(test, ETuCreateTestDirectories);
	CleanupStack::PushL(SmsTest);

	SmsTest->StartL();

	CleanupStack::PopAndDestroy(2); //SmsTest, iScheduler
	}

GLDEF_C TInt E32Main()
	{	
	__UHEAP_MARK;
	test.Start(_L("Setup"));
	theCleanup = CTrapCleanup::New();
	TRAPD(ret,doMainL());		
	test(ret==KErrNone);
	delete theCleanup;	
	test.Console()->SetPos(0, 13);
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}

CSmsInit::CSmsInit()
: CSmsTestBase()
	{
	}

void CSmsInit::ConstructL(RTest& aTest, TUint aCreationFlags)
	{
	iSmsTest = CSmsTestUtils::NewL(aTest, aCreationFlags);
	}

void CSmsInit::ShowMenuL()
	{
	iSmsTest->ResetMenu();

	iSmsTest->AppendToMenuL(_L("Create SMS MTM Dat File"));
	iSmsTest->AppendToMenuL(_L("Install SMS MTM Group"));
	iSmsTest->AppendToMenuL(_L("Both of the Above (Create Dat File and Install MTM Group)"));
	iSmsTest->AppendToMenuL(_L("Replace/Create Sms Service"));
	iSmsTest->AppendToMenuL(_L("All of the Above"));

	TInt result = iSmsTest->DisplayMenu(_L("SMS Initialisation"));

	if (result <= 0)
		return;

	if (result == 1 || result == 3 || result == 5)
		{
		iSmsTest->Printf(_L("Creating SMS MTM Dat File\n"));
		iSmsTest->CreateServerMtmRegsL();
		}

	if (result != 1)
		{
		iSmsTest->Printf(_L("Going Client Side\n"));
		iSmsTest->GoClientSideL();
		}

	if (result != 1 && result != 4)
		{
		iSmsTest->Printf(_L("Installing SMS MTM Group\n"));
		iSmsTest->InstallMtmGroupsL();
		}

	if (result == 4 || result == 5)
		{
		iSmsTest->Printf(_L("Replacing/Creating Sms Service\n"));
		iSmsTest->DeleteServicesL();
		iSmsTest->CreateServicesL();
		}

	iSmsTest->Test().Printf(_L("\nPress any key to continue..."));
	iSmsTest->Test().Getch();

	ShowMenuL();
	}

CSmsInit::~CSmsInit()
	{
	}

CSmsInit* CSmsInit::NewL(RTest& aTest, TUint aCreationFlags)
	{
	CSmsInit* self = new (ELeave) CSmsInit();
	CleanupStack::PushL(self);

	self->ConstructL(aTest, aCreationFlags);

	CleanupStack::Pop();
	return self;
	}
