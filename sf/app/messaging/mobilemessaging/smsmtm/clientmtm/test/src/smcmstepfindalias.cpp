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
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-5
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ unknown
 @SYMTestCaseDesc Search for alias in SMS (KMsvMessagePartOriginator) (ported from T_SMCM1)
 @SYMTestActions  Create an SMS and search for alias in it
 @SYMTestExpectedResults Alias which is et on creation should be find.
*/

#include "smcmstepfindalias.h"
#include <mtmdef.h>

CSMCMStepFindAlias::CSMCMStepFindAlias( )
	{
	SetTestStepName( KSMCMStepFindAlias );	
	}
	
CSMCMStepFindAlias::~CSMCMStepFindAlias( )
	{
	
	}
	
TVerdict CSMCMStepFindAlias::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepFindAlias::doTestStepL( )" ) );
	SetTestStepResult( EFail );
	
	TMessageSettings settings;
	settings.iOperation = EOperationNone;
	settings.iOriginalBoxId = KMsvGlobalInBoxIndexEntryId;
	settings.iOriginalSmsPDUType = CSmsPDU::ESmsDeliver;
	settings.iOriginalBioType = 0;
	TMsvId entryId = CreateMessageAndEntryL( KUserDataOriginal, settings );
	
	iSmsClientMtm->SwitchCurrentEntryL( entryId );
	iSmsClientMtm->LoadMessageL( );
	
	TMsvPartList partList;
	partList = KMsvMessagePartOriginator;
	
	// Find Alias
	if ( iSmsClientMtm->Find( _L( "Alias" ), partList ) == partList )
		{
		SetTestStepResult( EPass );
		}
	else 
		{
		ERR_PRINTF1( _L("\"Alias\" not found in KMsvMessagePartOriginator" ) );
		}
	
	return TestStepResult( );
		
	}
