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
// All the others should pass.
// 
//

/**
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-4
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ unknown
 @SYMTestCaseDesc Search for text strings in SMS (body and header fields) (ported from T_SMCM1)
 @SYMTestActions  Create an SMS and search for text strings in it
 @SYMTestExpectedResults Some find operations are not supported and thse should return the appropiate error. 
*/

#include "smcmstepfind.h"
#include <mtmdef.h>

CSMCMStepFind::CSMCMStepFind( )
	{
	SetTestStepName( KSMCMStepFind );
	}

CSMCMStepFind::~CSMCMStepFind( )
	{
	
	}

TVerdict CSMCMStepFind::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepFind::doTestStepL( )" ) );
	
	SetTestStepResult( EPass );
	
	TMsvPartList partList;
	
	TMessageSettings settings;
	settings.iOperation = EOperationNone;
	settings.iOriginalBoxId = KMsvGlobalInBoxIndexEntryId;
	settings.iOriginalSmsPDUType = CSmsPDU::ESmsSubmit;
	settings.iOriginalBioType = 0;
	TMsvId entryId = CreateMessageAndEntryL( KUserDataFind, settings );
	
	iSmsClientMtm->SwitchCurrentEntryL( entryId );
	iSmsClientMtm->LoadMessageL( );
	
	// Add some recipients
	iSmsClientMtm->AddAddresseeL( _L( "+358503367709" ), _L( "Mico3" ) );
	iSmsClientMtm->AddAddresseeL( _L( "+358503367709" ), _L( "Mico4" ) );
	iSmsClientMtm->AddAddresseeL( _L( "//\\" ), _L( "\n" ) );
	iSmsClientMtm->AddAddresseeL( _L( "\n0" ), _L( "^" ) );
	iSmsClientMtm->AddAddresseeL( _L("1234"), _L("~¨") );
	iSmsClientMtm->AddAddresseeL( _L("äöåÖÅÖÖ¤"), _L("fdsa4324521{[]}") );
	iSmsClientMtm->AddAddresseeL( _L("äöåÖÅÖÖ¤"), _L("") );
	iSmsClientMtm->AddAddresseeL( _L("äöåÖÅÖÖ¤") );
	
	// remove recipient
	iSmsClientMtm->RemoveAddressee( 2 );
	
	// Validate Message
	partList = KMsvMessagePartRecipient;
	
	if ( iSmsClientMtm->ValidateMessage( partList ) == KErrNone )
		{
		// all the addressees should be invalid
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Invalid message context or not supported: KMsvMessagePartRecipient" ) );
		}
	
	iSmsClientMtm->RemoveAddressee( iSmsClientMtm->AddresseeList( ).Count( ) - 1 );
	iSmsClientMtm->RemoveAddressee( iSmsClientMtm->AddresseeList( ).Count( ) - 1 );
	iSmsClientMtm->RemoveAddressee( iSmsClientMtm->AddresseeList( ).Count( ) - 1 );
	iSmsClientMtm->RemoveAddressee( iSmsClientMtm->AddresseeList( ).Count( ) - 2 );
	iSmsClientMtm->RemoveAddressee( iSmsClientMtm->AddresseeList( ).Count( ) - 2 );
	
	partList=KMsvMessagePartRecipient;			// supported
	if ( iSmsClientMtm->ValidateMessage( partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Invalid message context or not supported: KMsvMessagePartRecipient" ) );
		}
		
	partList=KMsvMessagePartBody;				// not supported, returns 0
	if ( iSmsClientMtm->ValidateMessage( partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Invalid message context or not supported: KMsvMessagePartBody" ) );
		}
	
	partList=KMsvMessagePartOriginator;			// not supported, returns 0
	if ( iSmsClientMtm->ValidateMessage( partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Invalid message context or not supported: KMsvMessagePartOriginator" ) );		
		}
	
	partList=KMsvMessagePartDescription;		// not supported, returns 0
	if ( iSmsClientMtm->ValidateMessage( partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Invalid message context or not supported: KMsvMessagePartDescription" ) );
		}
	
	partList=KMsvMessagePartDate;				// not supported, returns 0
	if ( iSmsClientMtm->ValidateMessage( partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Invalid message context or not supported: KMsvMessagePartDate" ) );
		}
	
	partList=KMsvMessagePartAttachments;		// not supported, returns 0
	if ( iSmsClientMtm->ValidateMessage( partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Invalid message context or not supported: KMsvMessagePartAttachments" ) );
		}
	
	partList=KMsvMessagePartBody | KMsvMessagePartRecipient;
	if ( iSmsClientMtm->ValidateMessage( partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Invalid message context or not supported: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}
	
	// Find in body and recipients
	iSmsClientMtm->AddAddresseeL( _L( "+358503367709" ), _L( "Mr. Tester" ) );
	
	partList=KMsvMessagePartRecipient;
	if ( iSmsClientMtm->Find( _L( "est" ), partList ) != partList ) 
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Cannot find \"est\" in message context: KMsvMessagePartRecipient" ) );
		}
		
	partList = KMsvMessagePartBody;
	
	if ( iSmsClientMtm->Find( _L( "end" ), partList ) != KMsvMessagePartBody )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Cannot find \"end\" in message context: KMsvMessagePartBody" ) );
		}
		
	partList = KMsvMessagePartOriginator;
	if ( iSmsClientMtm->Find( _L( "end" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"end\" in message context: KMsvMessagePartOriginator" ) );
		}
	
	partList = KMsvMessagePartDescription;		
	if ( iSmsClientMtm->Find( _L( "end" ), partList ) != KMsvMessagePartDescription )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"end\" in message context: KMsvMessagePartDescription" ) );
		}
	
	partList = KMsvMessagePartDate;				// not supported, returns 0
	if ( iSmsClientMtm->Find( _L( "end" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"end\" in message context: KMsvMessagePartDate" ) );
		}
	
	partList = KMsvMessagePartAttachments;		// not supported, returns 0
	if ( iSmsClientMtm->Find( _L( "end" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"end\" in message context: KMsvMessagePartAttachments" ) );
		}

	partList = KMsvMessagePartBody | KMsvMessagePartRecipient;
	if ( iSmsClientMtm->Find( _L( "\n" ), partList ) == partList )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"\\n\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( "äöå" ), partList ) == partList )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"äöå\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( "{[}]" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"{[}]\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( "\\" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"\\\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( "4352316542631753dfgfdgs4t6543w563vsd4" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"4352316542631753dfgfdgs4t6543w563vsd4\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( "~" ), partList ) == 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Cannot find \"~\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( " " ), partList ) == 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Cannot find \" \" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( "^" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"^\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( "   \"dsasaf¤;¨" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"   \\\"dsasaf¤;¨\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( " " ), partList ) != 0 ) // alt+255
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \" \" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}

	if ( iSmsClientMtm->Find( _L( "¨" ), partList ) == 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"¨\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}
		
	if ( iSmsClientMtm->Find( _L( "/\\" ), partList ) != 0 )
		{
		SetTestStepResult( EFail );
		ERR_PRINTF1( _L( "Found \"/\\\" in message context: KMsvMessagePartBody|KMsvMessagePartRecipient" ) );
		}
		
	return TestStepResult( );
	
	}

