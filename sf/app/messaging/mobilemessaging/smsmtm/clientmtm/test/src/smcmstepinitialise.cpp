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
// The UTC offset is et to -8 and the timestamp of the SMS service is compared against the current time
// 
//

/**
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-1
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ 234
 @SYMTestCaseDesc Initialise a new SMS service and check the UTC timestamp
 @SYMTestActions  UTC offset is set to 0, then any existing SMS service is removed and a new one is created.
 @SYMTestExpectedResults The timestamp of the SMS service should be smaller than the current time.
*/

#include "smcmstepinitialise.h"

CSMCMStepInitialise::CSMCMStepInitialise( )
	{
	SetTestStepName( KSMCMStepInitialise );
	}
	
	
CSMCMStepInitialise::~CSMCMStepInitialise( )
	{
		
	}
	
TVerdict CSMCMStepInitialise::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepInitialise::doTestStepL( )" ) );
	
	SetTestStepResult( EFail );
	
	ChangeLocale( KUTC0 );
	
	// first delete any old SMS service
	iSmsTestUtils->DeleteSmsServiceL( );
	
	// create a new SMS service (this will be time stamped UTC)
	TMsvId serviceId = KMsvNullIndexEntryId;
	serviceId = iSmsTestUtils->CreateSmsServiceL( );
	
	iSmsTestUtils->iSmsClientMtm->SwitchCurrentEntryL( serviceId );
	TMsvEntry serviceEntry = iSmsTestUtils->iSmsClientMtm->Entry( ).Entry( );

	// Change locale to UTC -8
	ChangeLocale( KUTCMin8 );
	TTime now;
	now.UniversalTime( );

	// Check if current time is bigger than time when service was created
	if ( serviceEntry.iDate < now )
		{
		SetTestStepResult( EPass );	
		
		}
	
	// if locale isn't changed the log file timestamps will be out of order 
	// as they are in local time
	ChangeLocale( KUTC0 );
	
	return TestStepResult( );
	
	}
