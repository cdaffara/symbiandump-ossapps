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
// T_ImapIoProgress.CPP
// 
//

#include <e32test.h>
#include <e32base.h>
#include "impspan.h"
#include "imapio.h"
#include "emailtestutils.h"

//----------------------------------------------------------------------------------------

LOCAL_D RTest test(_L("T_ImapIO Test Harness"));
TInt testNum = 1;
LOCAL_D CTrapCleanup* theCleanup;
LOCAL_D RFs theFs;	
LOCAL_C CActiveScheduler* theScheduler;
CEmailTestUtils* testUtils;
TMsvId serviceId;
 
GLDEF_C void gPanic( TImpsPanic aReason)
	{
	User::Panic(_L("T_ImapIO") ,aReason);
	}

//----------------------------------------------------------------------------------------
LOCAL_C void testImapIOProgress()
//----------------------------------------------------------------------------------------
	{
	CImapIO* imapIO = CImapIO::NewL(0);
	CleanupStack::PushL(imapIO);
	
	testUtils->TestStart(testNum, _L("Test Progress With Disconnect"));

	TRequestStatus status;
	CImIAPPreferences* prefs = CImIAPPreferences::NewLC();
	testUtils->WriteComment(_L("Creating an CImapIO object and leave it in a connecting state...\n"));
	imapIO->ConnectL(status, _L("localhost"), 143, *prefs, EFalse);

	TUint32 iap;
	TInt returnValue;
	testUtils->WriteComment(_L("Checking GetIAPValue returns a valid value...\n"));
	returnValue = imapIO->GetIAPValue(iap);
	test(returnValue != KErrNotFound);
	testUtils->WriteComment(_L("Checking GetConnectionStage returns a valid value...\n"));
	returnValue = imapIO->GetConnectionStage();
	test(returnValue != KErrNotFound);

	// Disconnect deletes its iSession. This sets up the conditions for the test
	// of behaviour change.
	testUtils->WriteComment(_L("Disconnect the CImapIO object. This removes its CImTextServerSession.\n"));
	imapIO->Disconnect();

	testUtils->WriteComment(_L("Checking GetIAPValue still returns KErrNotFound value after disconnect...\n"));
	returnValue = imapIO->GetIAPValue(iap);
	test(returnValue == KErrNotFound);
	testUtils->WriteComment(_L("Checking GetConnectionStage still returns KErrNotFound value after disconnect...\n"));
	returnValue = imapIO->GetConnectionStage();
	test(returnValue == KErrNotFound);

	testUtils->TestFinish(testNum++);
	CleanupStack::PopAndDestroy(); // prefs
	CleanupStack::PopAndDestroy(); // imapIO
	
	}

//----------------------------------------------------------------------------------------
LOCAL_C void doMainL()
//----------------------------------------------------------------------------------------
	{
		
	__UHEAP_MARK;
	theScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(theScheduler);

	CActiveScheduler::Install( theScheduler );

	testUtils = CEmailTestUtils::NewL(test);
	CleanupStack::PushL(testUtils);

	test.SetLogged(ETrue);
	testImapIOProgress();
	testUtils->TestHarnessCompleted();
	
	CleanupStack::PopAndDestroy(2);
	__UHEAP_MARKEND;
	}

//----------------------------------------------------------------------------------------
GLDEF_C TInt E32Main()
//----------------------------------------------------------------------------------------
	{	
	__UHEAP_MARK;
	test.Start(_L("T_ImapIO Test Harness"));
	theCleanup=CTrapCleanup::New();

	TRAPD(ret,doMainL());

	test(ret==KErrNone);
	delete theCleanup;
	test.End();
	test.Close();
	__UHEAP_MARKEND;
	return(KErrNone);
	}
