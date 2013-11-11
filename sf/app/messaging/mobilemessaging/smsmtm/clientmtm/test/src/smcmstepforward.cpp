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
//

/**
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-8
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ 234, other
 @SYMTestCaseDesc Creates a SMS with locale UTC offset 0, sets UTC offset to -8, forward the SMS
 @SYMTestActions  Checks the timestamps of original and forwarded SMS. (also check original test actions of T_SMCM1)
 @SYMTestExpectedResults The timestamp of the original SMS should be smaller than the timestamp of the forwarded SMS.
*/


#include "smcmstepforward.h"

#include <csmsaccount.h>
#include <e32math.h>

const TInt KDecreaseBodyLengthValue = 25;

CSMCMStepForward::CSMCMStepForward( )
	{
	SetTestStepName( KSMCMStepForward );
	}
	
CSMCMStepForward::~CSMCMStepForward( )
	{
	
	}
	
TVerdict CSMCMStepForward::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepForward::doTestStepL( )" ) );

	// Easier to have default EPass. Otherwise a passing test may overwrite the
	// result of earlier failed tests (unless we use an extra boolean which is set
	// to false if a test fails )
	SetTestStepResult( EPass );
	
	SetSettingsL( );
	
	TInt length = KMaxBodyLength;
	while ( length > 0 )
		{
		TUint32 bioType = Math( ).Random( );
		if ( doTestForwardL( length , bioType) == EFail )
			{
			SetTestStepResult( EFail );
			}
		length -= KDecreaseBodyLengthValue;
		}
		
	ChangeLocale( KUTC0 );

	return TestStepResult( );
	
	}
	
TVerdict CSMCMStepForward::doTestForwardL( TInt aBodyLength, TInt32 aBioType )
	{
	ChangeLocale( KUTC0 );
	
	HBufC* originalBody = CreateRandomLC( aBodyLength );
	
	TMessageSettings settings;
	settings.iOperation = EOperationForward;
	settings.iOriginalBoxId = KMsvGlobalOutBoxIndexEntryId;
	settings.iOriginalSmsPDUType = CSmsPDU::ESmsDeliver;
	settings.iOriginalBioType = aBioType;
	
	TMsvId entryId = CreateMessageAndEntryL( *originalBody, settings );
	
	ChangeLocale( KUTCMin8 );
	
	TVerdict verdict;
	verdict = PerformTestL( entryId, KUserDataForward, settings );
	
	CleanupStack::PopAndDestroy( originalBody );
	
	return verdict;
	
	}
