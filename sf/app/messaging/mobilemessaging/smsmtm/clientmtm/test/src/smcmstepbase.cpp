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

#include "smcmstepbase.h"

#include <csmsaccount.h>
#include <csmsemailfields.h>
#include <e32math.h>
#include <emsformatie.h>
#include <emspictureie.h>
#include "emstestutils.h"
#include <f32file.h>
#include <smuthdr.h>
#include <mtmdef.h>

RTest test( _L( "SMS Test" ) );

const TInt KOneHourSeconds = 3600;

// @return - TVerdict
// Implementation of CTestStep base class virtual
// All common initialisation to derived classes is done in here.
TVerdict CSMCMStepBase::doTestStepPreambleL( )
	{
	INFO_PRINTF1( _L( "CSMCMStepBase::doTestStepPreambleL()" ) );
	
	ChangeLocale ( KUTC0 );
	
	iScheduler = new( ELeave ) CActiveScheduler;
	CActiveScheduler::Install( iScheduler );

	// need this to open a server session
	iDummyObserver = new( ELeave ) TMsvDummyObserver;

	// All the test steps will need an SMS Client MTM so we
	// build it here	
	iSmsTestUtils = CSmsTestUtils::NewL( test );
	iSmsTestUtils->SetLogToFile( );
	iSmsTestUtils->InstantiateClientMtmsL( );

	//  Initialize mailbox
	CSmsSettings* settings = CSmsSettings::NewL( );
	CleanupStack::PushL( settings );
	CSmsAccount* account = CSmsAccount::NewLC( );
	TRAPD(err,account->InitialiseDefaultSettingsL( *settings ) );
	if ( err != KErrNone )
		{
		User::Leave( err );
		}
	
	CleanupStack::PopAndDestroy( account );
	CleanupStack::PopAndDestroy( settings ); 

	// open the server session
	iServerSession = CMsvSession::OpenSyncL( *iDummyObserver );
	
	// create the Client MTM registry and get the Client MTM from it
	iClientMtmRegistry = CClientMtmRegistry::NewL( *iServerSession );
	iSmsClientMtm = ( CSmsClientMtm* ) iClientMtmRegistry->NewMtmL( KUidMsgTypeSMS );

	// create the root entry
	iMsvEntry = CMsvEntry::NewL( *iServerSession, 
								 KMsvRootIndexEntryId, 
								 TMsvSelectionOrdering( ) );
	
	TSmsUtilities::ServiceIdL( *iServerSession, 
								iSmsServiceId, 
								KUidMsgTypeSMS );

	SetTestStepResult( EPass );

	return TestStepResult( );
	}

CSMCMStepBase::~CSMCMStepBase( )
	{
	delete iSmsClientMtm;
	delete iClientMtmRegistry;	

	delete iDummyObserver; 
	delete iServerSession; 

	delete iScheduler;

	}

CSMCMStepBase::CSMCMStepBase( )
	{
	
	}
	
void CSMCMStepBase::ChangeLocale( TInt aHours )
	{
	User::SetUTCOffset( KOneHourSeconds * aHours );	
	}
	
TMsvId CSMCMStepBase::CreateEmailSmsMessageL( const TDesC& aEmailMessageData )
	{
	RFs fs;
 	User::LeaveIfError( fs.Connect( ) );
 	CleanupClosePushL( fs );
 
 	CSmsMessage* message = CSmsMessage::NewL( fs, 
 											  CSmsPDU::ESmsDeliver, 
 											  CSmsEditorBuffer::NewL( *iSmsTestUtils->iRichText ), 
 											  EFalse );
 	CleanupStack::PushL( message );
 	
 	CSmsBufferBase& buffer = message->Buffer( );
 	buffer.Reset( );
 	buffer.InsertL( 0, aEmailMessageData );
 	
 	// Set the PID for email.
 	if( message->SmsPDU( ).ProtocolIdentifierPresent( ) )
 		{
 		message->SmsPDU( ).SetPIDType( TSmsProtocolIdentifier::ESmsPIDTelematicInterworking );
 		message->SmsPDU( ).SetTelematicDeviceIndicator( TSmsProtocolIdentifier::ESmsTelematicDevice );
 		message->SmsPDU( ).SetTelematicDeviceType( TSmsProtocolIdentifier::ESmsInternetElectronicMail );
 		
 		}

	// Create a new SMS header ( CSmsHeader takes ownership of message )
 	CSmsHeader* header = CSmsHeader::NewL( message );
 	CleanupStack::Pop( message );
 	CleanupStack::PushL( header );
 	
 	header->SetFromAddressL( KScriptItemRecipientsDef );
 	header->Message( ).SetServiceCenterAddressL( KScriptItemSCDef );

	// Set the entry data...
 	TMsvEntry entry;
 	entry.iType = KUidMsvMessageEntry;
 	entry.SetSendingState( KMsvSendStateNotApplicable );
 	TSmsUtilities::PopulateMsgEntry( entry, 
 									 header->Message( ), 
 									 iSmsTestUtils->iSmsServiceId );

 	entry.iDate.UniversalTime( );

	const CSmsEmailFields& fields = header->EmailFields( );
 	// Set the details fiels to the address field.
	if ( !fields.HasAddress( ) )
		{
		SetTestStepResult( EFail );
		}
	TInt count = fields.Addresses( ).MdcaCount( );
 	entry.iDetails.Set( fields.Addresses( ).MdcaPoint( 0 ).Left( KSmsDetailsLength ) );			
 	
 	// Set the description to either the subject or the begining part of the
 	// body text.
 	HBufC* desBuffer = HBufC::NewLC( iSmsClientMtm->ServiceSettings( ).DescriptionLength( ) );
 	TPtr description( desBuffer->Des( ) );
 	if( fields.Subject( ).Length( ) > 0 )
 		{
 		// There is a subject - use this as the description
 		description.Copy( fields.Subject( ).Left( description.MaxLength( ) ) );
 		
 		}
 	else
 		{
 		// No subject - use the begining part of the body text.
 		CSmsBufferBase& body = header->Message( ).Buffer( );
 		TInt length = body.Length( );
 		HBufC* buf = HBufC::NewLC( length );
 		TPtr ptr( buf->Des( ) );
 		body.Extract( ptr, 0, length );
 
 		description.Copy( ptr.Left( description.MaxLength( ) ) );
 		CleanupStack::PopAndDestroy( buf );
 		
 		}
 	entry.iDescription.Set( description );

	// Create the entry in the inbox.
 	iSmsTestUtils->SetEntryL( KMsvGlobalInBoxIndexEntryId );
 	iSmsTestUtils->CreateEntryL( entry );
 	iServerSession->CleanupEntryPushL( entry.Id( ) );
 	
 	// Store the header...
 	iSmsTestUtils->SetEntryL( entry.Id( ) );
 	CMsvStore* store = iSmsTestUtils->EditStoreL( );
 	CleanupStack::PushL( store );
 
 	header->StoreL( *store );
 
 	// Store the body text
 	store->StoreBodyTextL( *iSmsTestUtils->iRichText );
 	store->CommitL( );
 
 	entry.iSize = store->SizeL( );
 	entry.SetReadOnly( ETrue );
 	iSmsTestUtils->ChangeEntryL( entry );
 	
	CleanupStack::PopAndDestroy( store );
 	iServerSession->CleanupEntryPop( );
 	CleanupStack::PopAndDestroy( 3, &fs );	// fs, header, desBuffer
 	
 	return entry.Id( );

	}

TMsvId CSMCMStepBase::CreateMessageAndEntryL( const TDesC& aUserData, TMessageSettings aSettings )
	{
	TMsvEntry entry;
	entry.iType = KUidMsvMessageEntry;
	entry.SetSendingState( KMsvSendStateUnknown );
	entry.iDetails.Set( _L( "Alias" ) );
	entry.iBioType = aSettings.iOriginalBioType;
	
	iSmsTestUtils->iRichText->Reset( );
	iSmsTestUtils->iRichText->InsertL( 0, aUserData );
	
	CSmsHeader* header = NULL;
	if ( aSettings.iOperation == EOperationNone || aSettings.iOperation == EOperationReply )
		{
		header = CSmsHeader::NewL( aSettings.iOriginalSmsPDUType, 
								   *iSmsTestUtils->iRichText );	
		}
	else // EOperationForward
		{
		header = CSmsHeader::NewL( CSmsMessage::NewL( iSmsTestUtils->FileSession(), 
													  aSettings.iOriginalSmsPDUType, 
													  CSmsEditorBuffer::NewL( *iSmsTestUtils->iRichText ) ) );		
			}
	

	CleanupStack::PushL( header );
	
	// Add formating and a picture
	CEmsFormatIE* format = CEmsFormatIE::NewL( );
	CleanupStack::PushL( format );
	format->SetStartPosition(10);
	format->SetFormatLength(10);
	format->SetBold( ETrue );
	header->Message( ).AddEMSInformationElementL( *format );
	CleanupStack::PopAndDestroy( format );

	CEmsPictureIE* pic = EmsTestUtils::CreatePictureL( 0 );
	CleanupStack::PushL( pic );
	pic->SetStartPosition( 0 );
	header->Message().AddEMSInformationElementL( *pic );
	CleanupStack::PopAndDestroy(pic);

	header->SetFromAddressL( KScriptItemRecipientsDef );
	header->Message().SetServiceCenterAddressL( KScriptItemSCDef );
	
	TTime now;
	now.UniversalTime( );
	
	iSmsTestUtils->CreateMessageL( aSettings.iOriginalBoxId,
								   entry, 
								   *header, 
								   *iSmsTestUtils->iRichText,
								   now );
	entry.SetReadOnly( ETrue );
	
	CleanupStack::PopAndDestroy( header );

	return entry.Id( );
	
	}

// Creates a random string of lenth aLen (is used to generate a random body)	
HBufC* CSMCMStepBase::CreateRandomLC( TInt aLen ) const
	{
	HBufC* msg = HBufC::NewLC( aLen );
	TPtr msgPtr( msg->Des( ) );
	msgPtr.SetLength( aLen );
	
	TUint16* ptr = &msgPtr[ 0 ];
	while( --aLen )
		{
		// Create a random character between ASCII code 65 and 128
		*ptr++ = TUint16( Math::Random( ) % ( 128 - 65 ) +65 );
		}
		
	return msg;
	}

TMsvId CSMCMStepBase::PerformForwardOrReplyOperationL( TMessageOperation aOperation, 
										 			   TMsvId aInFolder )
	{
	TMsvPartList part = 0;
	CMsvOperationWait* wait = CMsvOperationWait::NewLC( );
	wait->Start( );
	
	CMsvOperation* operation = NULL;
	
	switch( aOperation )
		{
		case EOperationReply:
			operation = iSmsClientMtm->ReplyL( aInFolder, part, wait->iStatus );
			break;
		case EOperationForward:
			operation = iSmsClientMtm->ForwardL( aInFolder, part, wait->iStatus );
			break;
		default:
			User::Leave( KErrNotSupported );
			break;
			
		}
	

	CleanupStack::PushL( operation );

	CActiveScheduler::Start( ); // Wait for operation to complete

	if ( wait->iStatus.Int() != KErrNone)
		{
		SetTestStepResult( EFail );
		
		}

	// Extract the TMsvId new message created using the operation
	TPckgBuf< TMsvId > pkg;
	pkg.Copy( operation->ProgressL( ) );
	TMsvId messageId = pkg( );
	
	// Cleanup
	CleanupStack::PopAndDestroy( 2, wait );		// operation, wait
	
	return( messageId );
	
	}


TVerdict CSMCMStepBase::PerformTestL( TMsvId aOriginalMessageId, 
									  const TDesC& aNewText, 
									  const TMessageSettings& aSettings )
	{
	 // Lets point at the message which we'll forward
	iSmsClientMtm->SwitchCurrentEntryL( aOriginalMessageId );

	TMsvId responseMessageId = PerformForwardOrReplyOperationL( aSettings.iOperation, KMsvDraftEntryId );

	// Load the newly created FORWARD message	
	iSmsClientMtm->SwitchCurrentEntryL( responseMessageId );
	iSmsClientMtm->LoadMessageL( );
	
	// Add new part of message
	iSmsClientMtm->Body( ).InsertL( 0, aNewText );
	iSmsClientMtm->SaveMessageL( );
		
	TVerdict result = CompareMessagesL( aOriginalMessageId, responseMessageId, aSettings );
	
	return result;	
	}
	

TVerdict CSMCMStepBase::CompareMessagesL( TMsvId aOriginalMessageId, 
										  TMsvId aTargetMessageId, 
										  const TMessageSettings& aSettings )
	{
	__ASSERT_ALWAYS( aOriginalMessageId != aTargetMessageId, User::Panic( SMSTEST_PANIC, KSmsTestUtilsPanicCannotCompareSameIds ) );
	
	TVerdict result = EPass;
	// Load the Original Message
	iSmsClientMtm->SwitchCurrentEntryL( aOriginalMessageId );
	iSmsClientMtm->LoadMessageL( );

	TMsvEntry originalEntry = iSmsClientMtm->Entry( ).Entry( );

	const CSmsHeader& originalHeader = iSmsClientMtm->SmsHeader( );

	// copy original body text & description in a buffer to be used later
	CSmsEditorBuffer* origBuffer = CSmsEditorBuffer::NewL( iSmsClientMtm->Body( ) );
	TInt length = origBuffer->Length( );
	TBufC< KMaxBodyLength > originalMessage;
	
	TPtr origMsgPtr( originalMessage.Des( ) );
	origBuffer->Extract( origMsgPtr, 0, length );
	
	TBufC< CSmsBufferBase::EMaxBufLength > originalDescription;
	TPtr origDescPtr( originalDescription.Des( ) );
	origDescPtr.Copy( originalEntry.iDescription );

	// Load the Target message
	iSmsClientMtm->SwitchCurrentEntryL( aTargetMessageId );
	iSmsClientMtm->LoadMessageL( );
	
	TMsvEntry targetEntry = iSmsClientMtm->Entry( ).Entry( );
	
	const CSmsHeader& targetHeader = iSmsClientMtm->SmsHeader( );
	
	// Compare UTC Time of original and target messages
	if ( originalEntry.iDate >= targetEntry.iDate )
		{
		result = EFail;
		
		}
		
	// Applicable to both Forward and Reply
	TBool originalTextIncluded = EFalse;
	TMsvPartList partList = KMsvMessagePartBody;
	if ( iSmsClientMtm->Find( originalMessage.Des( ), partList ) == KMsvMessagePartBody )
		{
		originalTextIncluded = ETrue;
		
		}
	if ( !iSmsTestUtils->AreMessagesSameTextL( originalEntry.Id(), 
											   targetEntry.Id(), 
											   ETrue ) )
		{
		result = EFail;
		}

	if ( aSettings.iOperation == EOperationReply )
		{
		// nothing extra to do for a reply
		}
		
	if ( aSettings.iOperation == EOperationForward )
		{
		TUint stateOrig = originalEntry.SendingState( );
		TUint statetarg = targetEntry.SendingState( );
		if ( ( originalEntry.SendingState( ) != targetEntry.SendingState( ) ) || 
			 ( originalEntry.iBioType != targetEntry.iBioType ) || 
			 ( originalDescription != targetEntry.iDescription) )
			{
			result = EFail;
			
			}
		if ( !originalTextIncluded )
			{
			INFO_PRINTF1( _L( "Comparison Failed - Forward messages expect original text to be included" ) );
			result = EFail;
			
			}
		}
	
	return result;
	
	}
	
void CSMCMStepBase::SetSettingsL( )
	{
	CSmsSettings* settings = CSmsSettings::NewLC( );
	
	CSmsAccount* account = CSmsAccount::NewLC( );
	account->InitialiseDefaultSettingsL( *settings );

	settings->AddServiceCenterL( KScriptItemSC1, KScriptItemSC1Def );
	settings->AddServiceCenterL( KScriptItemSC2, KScriptItemSC2Def );
	settings->AddServiceCenterL( KScriptItemSC3, KScriptItemSC3Def );
	settings->SetDefaultServiceCenter( 2 );
	
	settings->SetValidityPeriod( ESmsVPWeek );
	settings->SetReplyQuoted( ETrue );
	settings->SetRejectDuplicate( ETrue );
	settings->SetDelivery( ESmsDeliveryImmediately );
	settings->SetDeliveryReport( ETrue );
	settings->SetReplyPath( EFalse );
	settings->SetMessageConversion( ESmsConvPIDNone );
	settings->SetCanConcatenate( ETrue );
	settings->SetCharacterSet( TSmsDataCodingScheme::ESmsAlphabet8Bit );
	settings->SetValidityPeriodFormat( TSmsFirstOctet::ESmsVPFSemiOctet );
	settings->SetStatusReportHandling( CSmsSettings::EMoveReportToInboxVisible );
	settings->SetSpecialMessageHandling( CSmsSettings::EMoveReportToInboxVisible );
	
	account->SaveSettingsL( *settings );
	
	iSmsTestUtils->iServiceSettings->CopyL( *settings);
	
	CleanupStack::PopAndDestroy( 2, settings );
	
	}
							   

