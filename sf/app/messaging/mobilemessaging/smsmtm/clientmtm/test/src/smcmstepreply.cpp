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
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-6
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ 234, other
 @SYMTestCaseDesc Creates a SMS with locale UTC offset 0, sets UTC offset to -8, reply the SMS
 @SYMTestActions  Checks the timestamps of original and replied SMS. (also check original test actions of T_SMCM1)
 @SYMTestExpectedResults The timestamp of the original SMS should be smaller than the timestamp of the replied SMS.
*/


#include "smcmstepreply.h"

#include <msvuids.h>
#include <smutset.h>
#include <smuthdr.h>

#include <csmsaccount.h>

CSMCMStepReply::~CSMCMStepReply( )
	{
	
	}

CSMCMStepReply::CSMCMStepReply( )
	{
	SetTestStepName( KSMCMStepReply );
	}

// @return - TVerdict code
// Override of base class pure virtual
TVerdict CSMCMStepReply::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepReply::doTestStepL( )" ) );
	
	SetTestStepResult( EFail );
	
	ChangeLocale( KUTC0 );
	
	SetSettingsL( );
	
	TMessageSettings settings;
	settings.iOperation = EOperationReply;
	settings.iOriginalBoxId = KMsvGlobalInBoxIndexEntryId;
	settings.iOriginalSmsPDUType = CSmsPDU::ESmsDeliver;
	settings.iOriginalBioType = 0;
	
	TMsvId entryId = CreateMessageAndEntryL( KUserDataOriginal, settings );
	
	ChangeLocale( KUTCMin8 );
	
	SetTestStepResult( PerformTestL( entryId, KUserDataReply, settings ) );
	
	ChangeLocale( KUTC0 );

	return TestStepResult( );
	
	}

