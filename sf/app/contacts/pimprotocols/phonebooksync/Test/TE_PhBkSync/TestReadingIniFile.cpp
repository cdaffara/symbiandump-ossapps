// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains PhbkSync API file corruption related Unit Test Steps
// 
//

#include "TE_PhBkSyncBase.h"
#include "TestReadingIniFile.h"

const TInt KNumberOfActions = 3;
const TInt KCorruptIt = 0;
const TInt KZeroLengthIt = 1;
const TInt KDeleteIt = 2;

//
//    Test step1 -- Test#1 in Unit Test Spec. Doc. //
//
CPhbkSyncIniFileTest::CPhbkSyncIniFileTest()
	{
	// store the name of this test step
	SetTestStepName(_L("PhbkSyncIniFileTest"));
	}
	
CPhbkSyncIniFileTest::~CPhbkSyncIniFileTest()
	{
	//NOP
	}

/** 
 * This test step is checking the loading of the ini file for the phone book syncronizer.
 * It will attemp to load a ini file that is a) Corrupt, Missing, Zero Length, and finally,
 *  valid.
 */
TVerdict CPhbkSyncIniFileTest::doTestStepL( )
	{
	
	
	for(TInt action=0;action<KNumberOfActions;action++)
		{
		iSession.Close();
		
		//	File does not exist
		ConfigurePhbkSyncToIdleL();
		DoToIniFileL(action);
		ConfigurePhbkSyncToFullL();
	
		TInt err=iSession.Connect();
		TESTCHECK(err,KErrNone);
		TRAP(err,CheckIniFileL());
		TESTCHECK(err,KErrNone);
		}

	return TestStepResult();
	}

void CPhbkSyncIniFileTest::CheckIniFileL()
	{

	// check the expected behaviour of the .ini file
	// No synchronisation is done for any phonebook
	TInt i(0);
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		CheckCacheStateInvalidL(iPhoneBook[i]);
		}

	// the newly created .ini file has Manual sync option
	// for all phonebooks
	iSession.Close();
	User::After(5*KOneSeconduS); // give server enough time to shut down
	User::LeaveIfError(iSession.Connect());
	for (i=0; i<KTotalNumberOfPhoneBook; i++) 
		{
		CheckSyncModeL(RPhoneBookSession::EManual, iPhoneBook[i]);
		}
	
	}

void CPhbkSyncIniFileTest::DoToIniFileL(TInt aAction)
	{

	//	Connect to the file server; creates a session
	RFs fs;
	RFile file;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TInt err=KErrNone;
	const TInt corruptStringPosition=20;
	TInt zeroSize=0;
	_LIT8(KCorruptString, "This file has been corrupted :(");

	//	What should we do to this file
	switch (aAction)
		{
		case KDeleteIt:
			INFO_PRINTF1(_L("Deleting file"));
			err=fs.Delete(KPhbkSyncIniFile);

			if ((err != KErrNone) && (err != KErrNotFound))
				{
				file.Close();
				err=fs.Delete(KPhbkSyncIniFile);
				
				if ((err != KErrNone) && (err != KErrNotFound))
					{	
					TEST(EFalse);
					}
				}
	
			break;
		
		case KCorruptIt:
			INFO_PRINTF1(_L("Corrupting file" ));
			
			//	Open the file
			err=file.Open(fs, KPhbkSyncIniFile, EFileShareAny | EFileWrite );

			if (err != KErrNone) // There is no ini file, so use default settings
				{
				TEST(EFalse);
				fs.Close();
				CleanupStack::PopAndDestroy(&fs);
				return;
				}
		
			CleanupClosePushL(file);			
			err=file.Write(corruptStringPosition,KCorruptString);
			TESTCHECK(err,KErrNone);
			
			file.Close();
			CleanupStack::PopAndDestroy(&file);
			
			
			break;
			
		case KZeroLengthIt:
			INFO_PRINTF1(_L("Zero length file" ));
						
			//	Open the file
			err=file.Open(fs, KPhbkSyncIniFile, EFileShareAny | EFileRead | EFileWrite );

			if (err != KErrNone) // There is no ini file, so use default settings
				{
				TEST(EFalse);
				fs.Close();
				CleanupStack::PopAndDestroy(&fs);
				return;
				}
				
			CleanupClosePushL(file);
			err=file.SetSize(zeroSize);
			TESTCHECK(err, EFalse);
		
			file.Close();
			CleanupStack::PopAndDestroy(&file);

			break;
			
		default:
			break;
		}
	

	fs.Close();
	CleanupStack::PopAndDestroy(); // fs 
	}
