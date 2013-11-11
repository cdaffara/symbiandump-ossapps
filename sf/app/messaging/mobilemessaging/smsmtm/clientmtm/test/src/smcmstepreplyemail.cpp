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
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-7
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ 234, other
 @SYMTestCaseDesc Creates an Email over SMS with locale UTC offset 0, sets UTC offset to -8, reply the Email over SMS
 @SYMTestActions  Checks the timestamps of original and replied Email over SMS. (also check original test actions of T_SMCM1)
 @SYMTestExpectedResults The timestamp of the original Email over SMS should be smaller than the timestamp of the replied Email over SMS.
*/

#include "smcmstepreplyemail.h"

#include <csmsemailfields.h>
#include <smuthdr.h> 

CSMCMStepReplyEmail::CSMCMStepReplyEmail( )
	{
	SetTestStepName( KSMCMStepReplyEmail );
	}
	
CSMCMStepReplyEmail::~CSMCMStepReplyEmail( )
	{
	
	}
	
TVerdict CSMCMStepReplyEmail::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepReplyEmail::doTestStepL( )" ) );
	
	ChangeLocale( KUTC0 );
	
	SetSettingsL( );
	
	TVerdict result = EPass;
	SetTestStepResult( result );
	
	_LIT( KTestEmailAddress1,	"user@domain1.domain2" );
 	_LIT( KTestEmailSubject1,	"Re: a subject" );
 	_LIT( KTestEmailMessage1,	"user@domain1.domain2##a subject#some body text" );
 	result = DoTestReplyEmailL( KTestEmailMessage1, KTestEmailAddress1, KTestEmailSubject1 );
 	if ( result == EFail )
 		{
 		SetTestStepResult( EFail );
 		}
 
 	// Reset Time to UTC0 for Next Test
 	ChangeLocale( KUTC0 );
 
 	_LIT( KTestEmailAddress2,	"user@domain1.domain2" );
 	_LIT( KTestEmailSubject2,	"" );
 	_LIT( KTestEmailMessage2,	"user@domain1.domain2 some body text" );
 	result = DoTestReplyEmailL( KTestEmailMessage2, KTestEmailAddress2, KTestEmailSubject2 );
 	if ( result == EFail )
 		{
 		SetTestStepResult( EFail );
 		}
 	
 	// if locale isn't changed the log file timestamps will be out of order 
	// as they are in local time	
 	ChangeLocale( KUTC0 );
 
	return TestStepResult( );
	
	}
	
TVerdict CSMCMStepReplyEmail::DoTestReplyEmailL( const TDesC& aEmailMessageData, 
											 const TDesC& aAddress, 
											 const TDesC& aSubject )
	{
	TVerdict result = EPass;
	
	TMsvId entrId = CreateEmailSmsMessageL( aEmailMessageData );

	ChangeLocale( KUTCMin8 );
	
	iSmsTestUtils->SetEntryL( entrId );
 	iSmsClientMtm->SwitchCurrentEntryL( entrId ); 
 	iSmsClientMtm->LoadMessageL( );
 	// store the original entry (to compare UTC)
   	TMsvEntry originalEntry = iSmsClientMtm->Entry( ).Entry( );
   	
 	TMsvPartList part=0;
 	CMsvOperationWait* wait = CMsvOperationWait::NewLC( );
 	CMsvOperation* op = iSmsClientMtm->ReplyL( KMsvDraftEntryId, part, wait->iStatus );
 	CleanupStack::PushL( op );
 	wait->Start( );
 	CActiveScheduler::Start( );

	TPckgBuf< TMsvId > pkg;
 	pkg.Copy( op->ProgressL() );
 	TMsvId reply = pkg();
 
 	CleanupStack::PopAndDestroy( 2, wait ); // op, wait
 	
 	// Switch and load the reply message
 	iSmsClientMtm->SwitchCurrentEntryL( reply );
 	iSmsClientMtm->LoadMessageL( );
 	
 	// Check the reply message...
 	const CSmsHeader& replyHeader = iSmsClientMtm->SmsHeader( );

	// 1. Email fields...
 	const CSmsEmailFields& fields = replyHeader.EmailFields( );
 	if ( fields.Addresses( ).MdcaCount( ) != 1)
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("CSmsEmailFields Address is not present or more than one is present" ) );
 		}
 	if ( fields.Addresses( ).MdcaPoint( 0 ).Compare( aAddress ) != 0 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("CSmsEmailFields Address is different" ) );
 		}
 	if ( fields.Subject( ).Compare( aSubject ) != 0 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("CSmsEmailFields Subject is not the expected one" ) );
 		}
 	
 	// 2. PID...
 	CSmsSettings* smsset = CSmsSettings::NewL( );
 	CleanupStack::PushL( smsset );
 	replyHeader.GetSmsSettingsL( *smsset );
 	if ( smsset->MessageConversion( ) != ESmsConvMail )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("Message Conversion is not ESmsConvMail" ) );
 		}
 		
 	CleanupStack::PopAndDestroy( smsset );
 	
 	// 3. Email <-> SMS gateway (i.e. the recipient)
 	const CArrayPtrFlat< CSmsNumber >& rcpts = replyHeader.Recipients( );
 	if ( rcpts.Count( ) != 1 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("SMS Gateway count is wrong" ) );
 		}
 	const CSmsNumber& rcpt = *rcpts.At( 0 );
 	if ( rcpt.Address( ).Compare( KScriptItemRecipientsDef ) != 0 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("SMS Gateway address is wrong" ) );
 		}
 
 	// 4. Description - should be the same as the subject.
 	TMsvEntry replyEntry = iSmsClientMtm->Entry( ).Entry( );
 	TInt maxDesLength = iSmsClientMtm->ServiceSettings( ).DescriptionLength( );
 	if ( replyEntry.iDescription.Compare( fields.Subject( ).Left( maxDesLength ) ) != 0 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("Description does not match subject" ) );
 		}
 	
 	// 5. Details - should be the same as the address.
 	if ( replyEntry.iDetails.Compare( fields.Addresses( ).MdcaPoint( 0 ).Left( KSmsDetailsLength ) ) != 0 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("Details do not match address" ) );
 		}
 		
 	// 6. UTC - UTC time of the original entry should be smaller than reply entry UTC time
 	if ( originalEntry.iDate >= replyEntry.iDate )
		{
		result = EFail;
		ERR_PRINTF1( _L("UTC time is wrong" ) );
		}

	return result;
	}

