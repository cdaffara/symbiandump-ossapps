// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains PhbkSync Close() matched Test Steps
// 
//

#include "TE_PhBkSyncBase.h"
#include "TestOpenSteps.h"

LOCAL_C void SetUpPhoneUsingTSYL(TInt aTestNumber)
{
	RProperty testNumberProperty;
	User::LeaveIfError(testNumberProperty.Attach(KUidPSSimTsyCategory, KPSSimTsyTestNumber));
	CleanupClosePushL(testNumberProperty);

	TRequestStatus status;
	testNumberProperty.Subscribe(status);
	User::LeaveIfError(testNumberProperty.Set(KUidPSSimTsyCategory,KPSSimTsyTestNumber,aTestNumber));
	User::WaitForRequest(status);
	User::LeaveIfError(status.Int());
	TInt testNumberCheck;
	User::LeaveIfError(testNumberProperty.Get(testNumberCheck));
	if (aTestNumber != testNumberCheck)
		{
		User::Leave(KErrNotFound);		
		}

	CleanupStack::PopAndDestroy(&testNumberProperty);
}

//
//    Test step1 -- Close() matched test step       //
//
CPhbkSyncOpenTest_01::CPhbkSyncOpenTest_01()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncOpenTest_01"));
	}

CPhbkSyncOpenTest_01::~CPhbkSyncOpenTest_01()
	{
	}

TVerdict CPhbkSyncOpenTest_01::doTestStepL( )
	{
	//
	// Stop PhBkSyncSvr before deleting INI and Contacts...
	//
	iSession.Close();
	ConfigurePhbkSyncToIdleL();

	RFs fs;
	User::LeaveIfError(fs.Connect()); // Connect to File server
	CleanupClosePushL(fs);
	TInt err(fs.Delete(KPhbkSyncIniFile)); // Delete PhBkSync.INI file

	// either successful or no default .ini file
	TESTCHECKCONDITION(err == KErrNone  ||  err == KErrNotFound  ||  err == KErrPathNotFound);

	// Delete default ContactsDb
	TRAP(err,CContactDatabase::DeleteDefaultFileL());
	
	// Deleted successfully OR no default ContactsDb  
	TESTCHECKCONDITION(err == KErrNone  ||  err == KErrNotFound); 

	CleanupStack::PopAndDestroy(); // fs

	// before calling the API connect(), the phone has to be ready
	SetUpPhoneUsingTSYL(0);  // initialise the phone using [test0]
	
	ConfigurePhbkSyncToFullL();

	// now, call connect and this one should start the server
	User::LeaveIfError(iSession.Connect());

	return TestStepResult();
	}

