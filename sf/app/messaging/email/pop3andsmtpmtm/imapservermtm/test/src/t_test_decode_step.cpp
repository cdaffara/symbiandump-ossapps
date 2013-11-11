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

#include <testexecutelog.h>
#include "t_test_decode_step.h"
#include "MIUTMSG.H"
#include <utf.h>

RTest test(_L("Decode Test"));
const TInt KImapPort = 143;

 
CTestDecode::~CTestDecode()
	{
	delete iConsole;
	delete iSpoofServer;	
	delete iTestUtils;
	delete iImapClient;	
	delete iScheduler;
	iSession->CloseMessageServer( );
	delete iSession;
	delete iSessionObserver;
	
	delete iExpectedPartsBuffer;
	delete iActualPartsBuffer;
	
	iFs.Close( );
	}

CTestDecode::CTestDecode()
	{
	// Call base class method to set up the human readable name for logging
	SetTestStepName( KTestDecode );
	
	iFs.Connect( );
	}

void CTestDecode::TestComplete( TInt aErrorCode )
	{
	CActiveScheduler::Stop();	
	
	if( aErrorCode != KErrNone && aErrorCode != KErrEof )
		{
		ERR_PRINTF2( _L( "Error: %d" ), aErrorCode );	
		}
	}
	
TBufferType CTestDecode::ConvertToBufferType(const TDesC& aBufferTypeDesc)
	{
	if ( aBufferTypeDesc.CompareF( _L( "TBufferType::EBodyBuffer" ) ) == KErrNone )
		{
		return ( TBufferType ) EBodyBuffer;
		}
	else if ( aBufferTypeDesc.CompareF( _L( "TBufferType::EAttachmentBuffer" ) ) == KErrNone )
		{
		return ( TBufferType ) EAttachmentBuffer;
		}
	return ( TBufferType ) EUndefinedBuffer;
	}

void CTestDecode::GetDetailsFromIniFileL()
	{
	
	//get the script file path
	if( !GetStringFromConfig( ConfigSection( ) , 
							  _L( "ScriptFile" ),
							  iScriptFile ) )
		{
		ERR_PRINTF1( _L( "Error reading script file path from ini file" ) );
		User::Leave( KErrArgument );	
		
		}
	
	TPtrC parameter;

	if( GetStringFromConfig( ConfigSection( ),
							 _L( "BufferFile" ),
							 parameter ) )
		{
		TLex parser( parameter );
		parser.SkipSpace( );
		
		while ( !parser.Eos( ) )
			{
			TBuffer* buffer = new ( ELeave ) TBuffer();
			CleanupStack::PushL( buffer );
		
			TPtrC arg;

			parser.SkipSpace( );
			
			if (parser.Eos())
				{
				ERR_PRINTF1( _L( "Invalid Argument for BufferFile" ) );
				User::Leave( KErrArgument );	
				}
			
			arg.Set( parser.NextToken( ) );
		
			buffer->iBufferType = ConvertToBufferType( arg );

			arg.Set( parser.NextToken( ) );

			parser.SkipSpace( );
		
			User::LeaveIfError( parser.Val( buffer->iAllowJunkAtEnd ) );

			RFile file;
			file.Open( iFs, arg, EFileRead );
			CleanupClosePushL( file );
		
			file.Read( buffer->iBuffer );
			iExpectedPartsBuffer->AppendL( *buffer );
		
			CleanupStack::PopAndDestroy( 1 );	//file
			CleanupStack::Pop( buffer );
			
			parser.SkipSpace( );
			
			}
		}
	else
		{
		ERR_PRINTF1( _L( "Error reading buffer file path from ini file" ) );
		User::Leave( KErrArgument );
		
		}
	
	//get the script file path
	if( !GetBoolFromConfig( ConfigSection( ),
							_L( "FetchWholeMessage" ),
							iFetchWholeMessage ) )
		{
		ERR_PRINTF1( _L( "Error reading \"FetchWholeMessage\"-argument from ini file" ) );
		User::Leave( KErrArgument );
		
		}
	}

void CTestDecode::TraverseL( const TMsvId aTMsvId )
	{
	//open the entry, dont sort
	TMsvSelectionOrdering ordering;	
	ordering.SetSorting( EMsvSortByNone );
	CMsvEntry* entry = CMsvEntry::NewL( *iSession, aTMsvId,ordering );
	CleanupStack::PushL( entry );
	
	//retrieve the entries children
	CMsvEntrySelection* msvEntrySelection;
	msvEntrySelection=entry->ChildrenL( );
	CleanupStack::PushL( msvEntrySelection );
	TInt count = entry->Count( );
	
	CMsvEntry* child;
	TMsvEntry childDetails;

	//for each child test if its a message entry
	for( TInt i = 0; i < count; i++ )
		{
		child=iSession->GetEntryL( ( *msvEntrySelection ) [ i ] );
		CleanupStack::PushL( child );
		childDetails=child->Entry( );
		
		if( childDetails.iType == KUidMsvMessageEntry )
			{
			//First Check the body
			CParaFormatLayer* paraFormatLayer = CParaFormatLayer::NewL( );
			CleanupStack::PushL( paraFormatLayer );
			CCharFormatLayer* charFormatLayer = CCharFormatLayer::NewL( );
			CleanupStack::PushL( charFormatLayer );
			CRichText* richText = CRichText::NewL( paraFormatLayer, charFormatLayer );
			CleanupStack::PushL( richText );

			CImEmailMessage* emailMessage = CImEmailMessage::NewLC( *child );
			
			emailMessage->GetBodyTextL( child->EntryId( ), 
										CImEmailMessage::EThisMessageOnly,
										*richText, 
										*paraFormatLayer, 
										*charFormatLayer );
										
			emailMessage->GetAttachmentsListL( child->EntryId( ),
											   CImEmailMessage::EAllAttachments,
											   CImEmailMessage::EThisMessageOnly );

			TDes* buffer16;
			buffer16 = new( ELeave ) TBuf<5000>( );
			CleanupStack::PushL( buffer16 );
			richText->Extract( *buffer16, 0 );
			TBuf8<KMaxBufferSize> buffer8;
			TInt unconverted =  CnvUtfConverter::ConvertFromUnicodeToUtf8( buffer8, 
																	   	   *buffer16 );
			iActualPartsBuffer->AppendL( buffer8 );
			CleanupStack::PopAndDestroy( buffer16 );

			MMsvAttachmentManager& attachmentMgr = emailMessage->AttachmentManager();
			RPointerArray<CMsvAttachment> attachmentInfoArray;
			attachmentInfoArray.Reset();
			CMsvAttachment* attachmentInfo;
			
			for (TInt i= 0; i< attachmentMgr.AttachmentCount();i++)
				{
				attachmentInfo = attachmentMgr.GetAttachmentInfoL(i);
				attachmentInfoArray.Append(attachmentInfo);
				
				TDes8* buffer;									
				buffer = new( ELeave ) TBuf8<5000>( );
				CleanupStack::PushL( buffer );
			
				RFile file;
				file.Open( iFs, attachmentInfo->FilePath( ), EFileRead );
				CleanupClosePushL( file );
		
				file.Read( *buffer );
				
				iActualPartsBuffer->AppendL( *buffer );
		
				CleanupStack::PopAndDestroy( 2, buffer );	//file, buffer
			
			}
			
			//if this is the first timestamp the us iActualTimeStamp, else use iActualTimeStamp2
			CleanupStack::PopAndDestroy( 4, paraFormatLayer );	

			}

		//recursive call
		TraverseL( childDetails.Id( ) );
		
		CleanupStack::PopAndDestroy( child );
		}
			
	CleanupStack::PopAndDestroy( msvEntrySelection );
	CleanupStack::PopAndDestroy( entry );	
	}


TVerdict CTestDecode::doTestStepPreambleL( )
	{
	SetTestStepResult( EFail );
	
	iScheduler = new ( ELeave ) CActiveScheduler;
	CActiveScheduler::Install( iScheduler );
	
	iTestUtils = CEmailTestUtils::NewL( test );
	iTestUtils->FileSession( ).SetSessionPath( _L( "C:\\" ) );
	iTestUtils->CleanMessageFolderL( );
	iTestUtils->ClearEmailAccountsL( );
	
	iTestUtils->GoClientSideL( );
	
	//initialize the buffers
	iExpectedPartsBuffer = new( ELeave) CArrayFixSeg< TBuffer >( 1 );	// granularity of 1, very often there 
	iActualPartsBuffer = new( ELeave) CDesC8ArraySeg( 1 );				// will beonly 1 message part

	//start the imap server
	GetDetailsFromIniFileL( );
	iConsole=Console::NewL( _L( "Decode Test" ),TSize( KConsFullScreen, KConsFullScreen ) );
	iSpoofServer = CSpoofServer::NewL(*this,iScriptFile);
	iSpoofServer->StartL(KImapPort);
	//start the imap client
	iImapClient = CActiveImapClient::NewL(this,iFetchWholeMessage);
	iImapClient->StartL();
	
	CActiveScheduler::Start();
	SetTestStepResult( EPass );
	return TestStepResult();
	}

TVerdict CTestDecode::CheckBuffer( const TDesC8& aActualBuffer, 
								   const TDesC8& aExpectedBuffer, 
								   TBool aAllowJunkAtEnd )
	{
	TVerdict result = EFail;
	
	TInt compareResult = aExpectedBuffer.Compare( aActualBuffer );
	if ( compareResult == 0)
		{
		INFO_PRINTF1( _L( "Decoded buffer does match reference file" ) );
		result = EPass;
		}
	else if ( compareResult == aExpectedBuffer.Length( )-aActualBuffer.Length( ) &&
			  aAllowJunkAtEnd )
		{
		WARN_PRINTF1( _L( "Decoded buffer math reference file, but contains extra \"junk\" at the end" ) );
		result = EPass;
		}
	else
		{
		INFO_PRINTF1( _L( "Decoded buffer does not match reference file" ) );
		}
	
	return result;
	
	}
		
TVerdict CTestDecode::doTestStepL( )
	{

	TVerdict result = EPass;

	// Session observer. Needed to create a session
	iSessionObserver = new (ELeave) TDummySessionObserver;
	// Session. Needed to create a client registry.
	iSession=CMsvSession::OpenSyncL(*iSessionObserver);
	// Client registry. Needed to get the MTM component
	TraverseL(iImapClient->GetAccount().iImapService);	

	// Check every message-part (we expect them to be in the same order 
	// as defined in the .ini configuration file)
	TInt partsCount = iExpectedPartsBuffer->Count( );
	if ( partsCount != iActualPartsBuffer->Count( ) )
		{
		ERR_PRINTF1( _L( "Number of expected parts does not mach actual number of parts" ) );
		User::Leave( KErrGeneral );
		}

	// Stop as soon as a test fail, we don't care about the rest.
	for ( TInt i=0; ( ( i < partsCount ) && ( result != EFail ) ); i++ )
		{
		TBuffer expectedPart = iExpectedPartsBuffer->At( i );
		result = CheckBuffer( iActualPartsBuffer->MdcaPoint( i ), 
							  expectedPart.iBuffer,
							  expectedPart.iAllowJunkAtEnd );
		
		}
	
	SetTestStepResult( result );
	return TestStepResult( );
	
	}
	

TVerdict CTestDecode::doTestStepPostambleL()
	{
	return TestStepResult( );
	
	}
	
	


