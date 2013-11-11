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
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-2
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ unknown
 @SYMTestCaseDesc Create a CSmsAccount and settings (ported from T_SMCM1)
 @SYMTestActions  Save and load settings
 @SYMTestExpectedResults Test should not Leave.
*/

#include "smcmstepsettings.h"

#include <csmsaccount.h>

CSMCMStepSettings::CSMCMStepSettings( )
	{
	SetTestStepName( KSMCMStepSettings );
	}

CSMCMStepSettings::~CSMCMStepSettings( )
	{
	
	}

TVerdict CSMCMStepSettings::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepSettings::doTestStepL( )" ) );
	
	SetTestStepResult( EFail );
	
	SetSettingsL( );
	
	iSmsClientMtm->Entry().SetEntryL( iSmsTestUtils->iSmsServiceId );
	
	CSmsAccount* account = CSmsAccount::NewLC( );
	CSmsSettings* loadedSettings = CSmsSettings::NewLC( );
	account->LoadSettingsL( *loadedSettings );

	if ( loadedSettings->DefaultServiceCenter() == 2 )
		{
		SetTestStepResult( EPass );
		
		}

	CleanupStack::PopAndDestroy( 2, account );	// loadedSettings, account
	
	return TestStepResult( );
	
	}
