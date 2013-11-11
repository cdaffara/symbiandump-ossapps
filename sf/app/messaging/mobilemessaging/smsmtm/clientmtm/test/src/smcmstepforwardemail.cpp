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
 @SYMTestCaseID MSG-SMSCLIENTMTM-0243-9
 @SYMTestType UT
 @SYMTestPriority High
 @SYMPREQ 234, other
 @SYMTestCaseDesc Creates an Email over SMS with locale UTC offset 0, sets UTC offset to -8, forward the Email over SMS
 @SYMTestActions  Checks the timestamps of original and forwarded Email over SMS. (also check original test actions of T_SMCM1)
 @SYMTestExpectedResults The timestamp of the original Email over SMS should be smaller than the timestamp of the forwarded Email over SMS.
*/

#include "smcmstepforwardemail.h"

#include <csmsemailfields.h>
#include <smuthdr.h> 

CSMCMStepForwardEmail::CSMCMStepForwardEmail( )
	{
	SetTestStepName( KSMCMStepForwardEmail );
	}
	
CSMCMStepForwardEmail::~CSMCMStepForwardEmail( )
	{
	
	}
	
TVerdict CSMCMStepForwardEmail::doTestStepL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepReplyEmail::doTestStepL( )" ) );
	
	ChangeLocale( KUTC0 );
	
	SetSettingsL( );
	
	// Easier to have default EPass. Otherwise a passing test may overwrite the
	// result of earlier failed tests (unless we use an extra boolean which is set
	// to false if a test fails )
	TVerdict result = EPass;
	SetTestStepResult( result );
	 
 	_LIT( KTestEmailSubject1,	"Fw: a subject" );
 	_LIT( KTestEmailBody1,		"some body text" );
 	_LIT( KTestEmailMessage1,	"user@domain1.domain2##a subject#some body text" );
 	result = DoTestForwardEmailL( KTestEmailMessage1, KTestEmailSubject1, KTestEmailBody1 );
 	if ( result == EFail )
 		{
 		SetTestStepResult( EFail );
 		}
 	
 	// Reset Time to UTC0 for Next Test	
 	ChangeLocale( KUTC0 );
 
 	_LIT( KTestEmailSubject2,	"" );
 	_LIT( KTestEmailBody2,		"some body text" );
 	_LIT( KTestEmailMessage2,	"user@domain1.domain2 some body text" );
 	result = DoTestForwardEmailL( KTestEmailMessage2, KTestEmailSubject2, KTestEmailBody2 );
 	if ( result == EFail )
 		{
 		SetTestStepResult( EFail );
 		}
 	
 	// if locale isn't changed the log file timestamps will be out of order 
	// as they are in local time	
 	ChangeLocale( KUTC0 );

	return TestStepResult( );
	
	}
	
TVerdict CSMCMStepForwardEmail::DoTestForwardEmailL( const TDesC& aEmailMessageData, 
								  					 const TDesC& aSubject, 
								  					 const TDesC& aBody )
	{
	TVerdict result = EPass;
	
	// Create an email message and then a forward message to it.
 	TMsvId entryId = CreateEmailSmsMessageL( aEmailMessageData );
 	iSmsTestUtils->SetEntryL( entryId );
 	iSmsClientMtm->SwitchCurrentEntryL( entryId ); 
 	iSmsClientMtm->LoadMessageL( );
 	// store the original entry (to compare UTC)
   	TMsvEntry originalEntry = iSmsClientMtm->Entry( ).Entry( );

	ChangeLocale ( KUTCMin8 );
 
 	TMsvPartList part = 0;
 	CMsvOperationWait* wait = CMsvOperationWait::NewLC( );
 	CMsvOperation* op = iSmsClientMtm->ForwardL( KMsvDraftEntryId, part, wait->iStatus );
 	CleanupStack::PushL( op );
 	wait->Start( );
 	CActiveScheduler::Start( );
 
 	TPckgBuf< TMsvId > pkg;
 	pkg.Copy( op->ProgressL( ) );
 	TMsvId forward = pkg( );
 
 	CleanupStack::PopAndDestroy( 2, wait ); // op, wait

 	
 	// Switch and load the forward message
 	iSmsClientMtm->SwitchCurrentEntryL(forward);
 	iSmsClientMtm->LoadMessageL();
 	
 	// Check the forward message...
 	const CSmsHeader& forwardHeader = iSmsClientMtm->SmsHeader();
 	
 	// 1. Email fields...(address should be empty)
 	const CSmsEmailFields& fields = forwardHeader.EmailFields();
 	if( fields.HasAddress( ) )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("CSmsEmailFields Address is not empty" ) );
 		}
 	if ( fields.Addresses( ).MdcaCount( ) !=0 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("CSmsEmailFields Address is not empty" ) );
 		}
 	if ( fields.Subject( ).Compare( aSubject ) != 0 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("CSmsEmailFields Subject is not the expected one" ) );
 		}
 	
 	// 2. PID...
 	CSmsSettings* smsset = CSmsSettings::NewL( );
 	CleanupStack::PushL( smsset );
 	forwardHeader.GetSmsSettingsL( *smsset );
 	if ( smsset->MessageConversion( ) != ESmsConvMail )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("Message Conversion is not ESmsConvMail" ) );
 		}
 	CleanupStack::PopAndDestroy( smsset );
 	
 	// 3. Email <-> SMS gateway (ie the recipient)
 	const CArrayPtrFlat< CSmsNumber >& rcpts = forwardHeader.Recipients( );
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
 
 	// 4. Description - should be the same as the subject or body text.
 	TMsvEntry forwardEntry = iSmsClientMtm->Entry( ).Entry( );
 	TInt maxDesLength = iSmsClientMtm->ServiceSettings( ).DescriptionLength( );
 	if( fields.Subject( ).Length( ) )
 		{
 		// Description should be same as subject.
 		if ( forwardEntry.iDescription.Compare( fields.Subject( ).Left( maxDesLength ) ) != 0)
 			{
 			result = EFail;
 			ERR_PRINTF1( _L("Description does not match subject" ) );
 			}
 		}
 	else
 		{
 		// Description should be the same as the body text.
 		HBufC* body = HBufC::NewLC( iSmsClientMtm->Body( ).DocumentLength( ) );
 		TPtr bodyPtr( body->Des( ) );
 		iSmsClientMtm->Body( ).Extract( bodyPtr );
 
 		if ( forwardEntry.iDescription.Compare( bodyPtr.Left( maxDesLength ) ) != 0 )
 			{
 			result = EFail;
 			ERR_PRINTF1( _L("Description does not match body text" ) );
 			}
 		CleanupStack::PopAndDestroy( body );
 		}
 	
 	// 5. Details - should be the same as the address (ie empty).
 	if( forwardEntry.iDetails.Compare( KNullDesC( ) ) != 0 )
 		{
 		result = EFail;
 		ERR_PRINTF1( _L("Details do not match address" ) );
 		}
	
 	// 6. Body data.
 	if ( !TestForwardBodyL( iSmsClientMtm->Body( ), aBody) )
		{
		result = EFail;
		ERR_PRINTF1( _L("Body does not" ) );
		}
	
	// 7. UTC - UTC time of the original entry should be smaller than forward entry UTC time
 	if ( originalEntry.iDate >= forwardEntry.iDate )
		{
		result = EFail;
		ERR_PRINTF1( _L("UTC time is wrong" ) );
		}
	
	return result;
	}
	
TBool CSMCMStepForwardEmail::TestForwardBodyL( const CRichText& aBody, const TDesC& aExpected ) const
	{
	HBufC* body = HBufC::NewLC( aBody.DocumentLength( ) );
	TPtr bodyPtr( body->Des( ) );

	aBody.Extract( bodyPtr );

	const TBool check = ( *body == aExpected );

	CleanupStack::PopAndDestroy( body );

	return check;
	}
