/*
* Copyright (c) 2005-2007 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Part of SyncML Data Synchronization Plug In Adapter
*
*/



#include <e32std.h>
#include <msvids.h>
#include <txtrich.h>
#include "vmessageparser.h"
#include "logger.h"


const TInt KVersionMaxLength = 4;
const TInt KDateMaxLength = 48;
const TInt KTagAndValueMaxLength = 48;
const TInt KDefaultWriteBufSize = 1024;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CVMessageParser* CVMessageParser::NewL()
// -----------------------------------------------------------------------------
//
CVMessageParser* CVMessageParser::NewL()
	{
	CVMessageParser* self = NewLC();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CVMessageParser* CVMessageParser::NewLC()
// -----------------------------------------------------------------------------
//
CVMessageParser* CVMessageParser::NewLC()
	{
	CVMessageParser* self = new( ELeave ) CVMessageParser();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::~CVMessageParser()
// -----------------------------------------------------------------------------
//
CVMessageParser::~CVMessageParser()
	{
	iRecipients.Close();
	delete iMessageBody;
	iMessageBody = NULL;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::CVMessageParser()
// -----------------------------------------------------------------------------
//
CVMessageParser::CVMessageParser()
	{
	}

// -----------------------------------------------------------------------------
// CVMessageParser::ConstructL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::ConstructL()
	{
	}

// -----------------------------------------------------------------------------
// CVMessageParser::ReadMessage()
// -----------------------------------------------------------------------------
//
TInt CVMessageParser::ParseMessageL( const TDesC8& aVMessage )
	{
	LOGGER_ENTERFN( "CVMessageParser::ParseMessageL" );

	const TUint8* ptr8 = aVMessage.Ptr();	
	const TUint16* ptr16 = reinterpret_cast<const TUint16*>( ptr8 );
	iReadBuf.Set( ptr16, aVMessage.Length()/2 );
	LOG(iReadBuf);	
	
	iReadBufPosition = 0;

	TInt err = ReadMessageHeader();
	
	if (err < 0)
		{
		LOGGER_WRITE_1( "ReadMessageHeader() failed with %d", err );
		}
	else
		{
		err = ReadMessageBodyL();
		LOGGER_WRITE_1( "ReadMessageBody() result: %d", err );
		}
    LOGGER_LEAVEFN( "CVMessageParser::ParseMessageL" );
	return err;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::ConstructMessageL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::ConstructMessageL( CBufBase& aVMessage )
	{
	LOGGER_ENTERFN( "CVMessageParser::ConstructMessageL" );
	LOGGER_WRITE_1( "Initial buffer size: %d", aVMessage.Size() );

	iWriteBuf = &aVMessage;
	iWriteBufPosition = 0,
	iWriteIndentLevel = 0;

	iWriteBufSize = KDefaultWriteBufSize;
	if (iMessageBody)
		{
		iWriteBufSize += iMessageBody->Size();
		}

	iWriteBuf->ResizeL( iWriteBufSize );
	LOGGER_WRITE_1( "Buffer size resized to %d", iWriteBuf->Size() );
	
	WriteMessageLineL( KVMsgTagBegin, KVMsgSectionVMsg ); // BEGIN:VMSG
	WriteMessagePropertiesL();
	WriteMessageVCARDL( iSender.iName, iSender.iNumber );
	WriteMessageEnvelopeL();
	WriteMessageLineL( KVMsgTagEnd, KVMsgSectionVMsg ); // END:VMSG

	iWriteBuf->ResizeL( iWriteBufPosition );

	LOGGER_WRITE_1( "Message length: %d", iWriteBufPosition );
	LOGGER_LEAVEFN( "CVMessageParser::ConstructMessageL" );
	}

// -----------------------------------------------------------------------------
// CVMessageParser::ResetAll()
// -----------------------------------------------------------------------------
//
void CVMessageParser::ResetAll()
	{
	// Public data 

	iHomeTime = 0;
	iUniversalTime = 0;
	iFolder.Zero();
	iStatus.Zero();
	iSender.iNumber.Zero();
	iSender.iName.Zero();
	iRecipients.Reset();
	if (iMessageBody)
		{
		delete iMessageBody;
		iMessageBody = NULL;
		}


	// Private variables
	iReadBufPosition = 0;
	iWriteBufPosition = 0;
	iWriteIndentLevel = 0;
	iWriteBuf = NULL;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::LoadMessageBodyL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::LoadMessageBodyL( CRichText &aMsgBody )
	{
	if (iMessageBody)
		{
		aMsgBody.InsertL(0, *iMessageBody);
		}
	}

// -----------------------------------------------------------------------------
// CVMessageParser::StoreMessageBodyL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::StoreMessageBodyL( CRichText &aMsgBody )
	{
	if (iMessageBody)
		{
		LOGGER_WRITE( "Message body not empty" );
		User::Leave( KErrGeneral );
		}

	TInt bodyLenght =  aMsgBody.DocumentLength();

	if (bodyLenght > 0)
		{
		iMessageBody = HBufC::NewL( bodyLenght );
		TPtr16 ptrBody = iMessageBody->Des();
		aMsgBody.Extract( ptrBody );
		}
	}

// -----------------------------------------------------------------------------
// CVMessageParser::ParseTelephoneNumber()
// -----------------------------------------------------------------------------
//
TBool CVMessageParser::ParseTelephoneNumber( const TDesC &aNumberString, TTelephoneNumber &aNumberStore )
	{
	_LIT( KStartMarker, "<" );
	_LIT( KEndMarker, ">" );
    LOGGER_WRITE_1("aNumberString: %S", &aNumberString );
	TInt startMarker = aNumberString.Find( KStartMarker );

	if (startMarker >= 0)
		{
		TInt endMarker = aNumberString.Find( KEndMarker );
		if (endMarker <= (startMarker + 1))
			{
			LOGGER_WRITE( "Incorrect number" );
			return EFalse;
			}
		else // case 1: Abc Pqr <+01234567890>
			{
			TPtrC16 name = aNumberString.Left( startMarker );
			TPtrC16 number = aNumberString.Mid( (startMarker + 1), (endMarker - startMarker - 1) ); 
			AssignDataToBuffer( name, aNumberStore.iName );
			AssignDataToBuffer( number, aNumberStore.iNumber );
			}
		}
	else // just number
		{
		AssignDataToBuffer( aNumberString, aNumberStore.iNumber );
		aNumberStore.iName.Zero();
		}
    LOGGER_WRITE_1("aNumberStore.iName: %S", &aNumberStore.iName);

	return ETrue;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::ReadMessageHeader()
// -----------------------------------------------------------------------------
//
TInt CVMessageParser::ReadMessageHeader()
	{
	LOGGER_WRITE( "CVMessageParser::ReadMessageHeader()" );

	TInt err;

	err = FindMessageTagWithValue( KVMsgTagBegin, KVMsgSectionVMsg );
	if (err < 0)
		{
		return err;
		}
		
	err = ReadTaggedTimeStamp( iUniversalTime );
	if ( err == KErrNone )
		{
		LOGGER_WRITE( "Time stamp tag found, universal time set" );
		}		
				
	err = GetMessageTagValue( KVMsgTagStatus, iStatus );
	if (err < 0)
		{
		return err;
		}		
	err = GetMessageTagValue( KVMsgTagBox, iFolder );
	if (err < 0)
		{
		return err;
		}
	err = FindMessageTagWithValue( KVMsgTagBegin, KVMsgSectionVCard );
	if (err < 0)
		{
		return err;
		}
	err = ReadVCard( iSender );
	if (err < 0)
		{
		return err;
		}
	err = FindMessageTagWithValue( KVMsgTagBegin, KVMsgSectionVEnv );
	if (err < 0)
		{
		return err;
		}
	else
		{
		err = ReadEnvelope();
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::ReadEnvelope()
// -----------------------------------------------------------------------------
//
TInt CVMessageParser::ReadEnvelope()
	{
	TBuf<KTagAndValueMaxLength> tagValue;
	TInt err( KErrNone );

	err = GetMessageTagValue( KVMsgTagBegin, tagValue );
	if (err < 0)
		{
		return err;
		}
	else if (tagValue.Compare( KVMsgSectionVCard ) == 0)
		{
		TTelephoneNumber recipient;
		err = ReadVCard( recipient );
		if (err < 0)
			{
			return err;
			}
		else
			{
			if (recipient.iNumber.Length() > 0)
			    {
			    iRecipients.Append( recipient );
			    }
			    
			err = FindMessageTagWithValue( KVMsgTagBegin, KVMsgSectionVEnv );
			if (err >= 0)
				{
				err = ReadEnvelope();
				}
			}
		}
	else if (tagValue.Compare( KVMsgSectionVBody ) == 0)
		{
		err = KErrNone;
		}
	else
		{
		LOGGER_WRITE( "Unexpected message tag value" );
		return KErrGeneral;
		}

	return err;
	}
	
// -----------------------------------------------------------------------------
// CVMessageParser::ReadTaggedTimestamp()
// Read and parse tagged timestamp from vMessage (optional feature)
// Format: X-NOK-DT:yyyymmddThhmmssZ (example: X-NOK-DT:20060329T091530Z)
// -----------------------------------------------------------------------------
//	
TInt CVMessageParser::ReadTaggedTimeStamp( TTime& aUniversalTime )
	{
	LOGGER_WRITE( "CVMessageParser::ReadTaggedTimestamp" );	
	
	TInt err( KErrNone );
	TBuf<20> timeStamp;
	
	err = GetMessageTagValue( KVMsgTagDateTime, timeStamp, EFalse );
	if ( err < 0 )
		{
		return KErrNotFound;
		}
	
	LOG( timeStamp );
	
	if (timeStamp.Length() != 16 || timeStamp[8] != 'T' || timeStamp[15] != 'Z')
		{
		return KErrNotSupported;
		}

	TLex lexYear( timeStamp.Mid( 0, 4 ) );
	TLex lexMonth( timeStamp.Mid( 4, 2 ) );
	TLex lexDay( timeStamp.Mid( 6, 2 ) );
	TLex lexHour( timeStamp.Mid( 9, 2 ) );
	TLex lexMinute( timeStamp.Mid( 11, 2 ) );
	TLex lexSecond( timeStamp.Mid( 13, 2 ) );		
	
	TInt valYear( 0 );
	TInt valMonth( 0 );
	TInt valDay( 0 );
	TInt valHour( 0 );
	TInt valMinute( 0 );
	TInt valSecond( 0 );

	TBool parseDone( EFalse );		
	if ( lexYear.Val( valYear ) == KErrNone )
		{
		if ( lexMonth.Val( valMonth ) == KErrNone )
			{
			if ( lexDay.Val( valDay ) == KErrNone )
				{
				if ( lexHour.Val( valHour ) == KErrNone )
					{
					if ( lexMinute.Val( valMinute ) == KErrNone )
						{
						if ( lexSecond.Val( valSecond ) == KErrNone )
							{
							parseDone = ETrue;
							}
						}
					}
				}
			}		
		}
	if ( !parseDone )
		{
		return KErrNotSupported;
		}
		
	TDateTime dateTime;
	TMonth month = static_cast<TMonth>(--valMonth);
	err = dateTime.Set( valYear, month, --valDay, valHour, valMinute, valSecond, 0 );	
	if ( err != KErrNone )
		{
		return KErrNotSupported;
		}
		
	aUniversalTime = dateTime;	
	
	return KErrNone; 	
	}		

// -----------------------------------------------------------------------------
// CVMessageParser::ReadVCard()
// -----------------------------------------------------------------------------
//
TInt CVMessageParser::ReadVCard( TTelephoneNumber &aResult )
	{		
	TInt err = KErrNone;
	TBuf<KVersionMaxLength> version;

	err = GetMessageTagValue( KVMsgTagVersion, version );
	if (err < 0)
		{
		return err;
		}

	err = GetMessageTagValue( KVMsgTagName, aResult.iName );
	if (err < 0)
		{
		return err;
		}

	err = GetMessageTagValue( KVMsgTagTelephone, aResult.iNumber );
	if (err < 0)
		{
		return err;
		}

	err = FindMessageTagWithValue( KVMsgTagEnd, KVMsgSectionVCard );
	if (err < 0)
		{
		return err;
		}

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::ReadMessageBody()
// -----------------------------------------------------------------------------
//
TInt CVMessageParser::ReadMessageBodyL()
	{
	LOGGER_WRITE( "CVMessageParser::ReadMessageBody()" );

	TInt err;
	
	TPtrC pBuf = iReadBuf.Mid( iReadBufPosition );
	err = pBuf.Find( KVMsgLineFeed );
	if (err < 0)
		{
		return err;
		}
	
	iReadBufPosition += ( err + KVMsgLineFeed().Length() );
	TInt bodyStart = iReadBufPosition;

	TBuf<KDateMaxLength> dateBuf;
	err = GetMessageTagValue(KVMsgTagDate, dateBuf);
	if (err < 0)
		{
		LOGGER_WRITE( "No date field in message body" );
		iReadBufPosition = bodyStart;
		}
	else
		{
		for (TInt i = 0; i < dateBuf.Length(); i++)
			{
			// This is needed, because TTime::Parse() can not handle
			// situation where '.' character is used as a delimiter in date.
			// Expected date format is like 18.1.2005 17:32:50.
			//
			if (dateBuf[i] == ' ')
				{
				break;
				}
			else if (dateBuf[i] == '.')
				{
				dateBuf[i] = '/';
				}
			}
		
        TLocale locale;
        TDateFormat originalFormat = locale.DateFormat();
        locale.SetDateFormat( EDateEuropean );
        locale.Set(); // transfer the new locale settings to the system
        
		err = iHomeTime.Parse( dateBuf );
		if ( err < 0 )
			{
			LOGGER_WRITE_1( "Parsing date from message body failed with %d", err );
			iHomeTime = 0;
			}
		locale.SetDateFormat( originalFormat );
		locale.Set(); // transfer the original locale settings back to the system

		TPtrC pDateStart = iReadBuf.Mid( iReadBufPosition );
		err = pDateStart.Find( KVMsgLineFeed );
		if (err < 0)
			{
			return err;
			}
		iReadBufPosition += ( err + KVMsgLineFeed().Length() );
		bodyStart = iReadBufPosition;
		}

	err = FindMessageTagWithValue( KVMsgTagEnd, KVMsgSectionVBody );
	if (err < 0)
		{
		return err;
		}
	else
		{
		TInt bodyLength = err - bodyStart;
		iMessageBody = HBufC::NewL( bodyLength );
		TPtrC pData = iReadBuf.Mid( bodyStart, bodyLength );
		TPtr pMsgBody = iMessageBody->Des();
		pMsgBody.Copy( pData );
		pMsgBody.Trim();
		err = pMsgBody.Length();
		LOG( pMsgBody );
		}

	return err;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::FindMessageTagWithValue()
// -----------------------------------------------------------------------------
//
TInt CVMessageParser::FindMessageTagWithValue( const TDesC& aMsgTag, const TDesC& aValue )
	{
	TBuf<KTagAndValueMaxLength> tagAndValue( aMsgTag );
	tagAndValue.Append( aValue );

	TPtrC pBuf = iReadBuf.Mid( iReadBufPosition );

	TInt err;
	TInt tagStartPosition;
		
	err = pBuf.Find( tagAndValue );
	if (err < 0)
		{
		LOGGER_WRITE( "Message tag with value not found" );
		return err;
		}
	else 
		{
		tagStartPosition = iReadBufPosition + err; // position before tag and value
		iReadBufPosition += ( err + tagAndValue.Length() ); // position after tag and value
		}

	return tagStartPosition; 
	}

// -----------------------------------------------------------------------------
// CVMessageParser::GetMessageTagValue()
// -----------------------------------------------------------------------------
//
TInt CVMessageParser::GetMessageTagValue( const TDesC& aMsgTag, TDes& aValue, TBool aMoveReadBufPosition )
	{
	TInt err;
	TInt valueLength;
	TInt readBufPosition( iReadBufPosition );

	TPtrC pBuf = iReadBuf.Mid( readBufPosition );
	
	err = pBuf.Find( aMsgTag );
	if (err < 0)
		{	
		LOGGER_WRITE( "Message tag not found" );
		return err; 
		}
	else
		{
		readBufPosition += (err + aMsgTag.Length());
		}

	TPtrC pValueStart = iReadBuf.Mid( readBufPosition ); 

	err = pValueStart.Find( KVMsgLineFeed );
	if (err < 0)
		{
		return err; 
		}
	else
		{
		valueLength = err;
		}

	StoreMessageDataField( readBufPosition, valueLength, aValue );
	
	if ( aMoveReadBufPosition )
		{
		iReadBufPosition = readBufPosition;
		}

	return valueLength;
	}
	
// -----------------------------------------------------------------------------
// CVMessageParser::StoreMessageDataField()
// -----------------------------------------------------------------------------
//
void CVMessageParser::StoreMessageDataField(TInt aStart, TInt aLength, TDes &aLocalStore)
	{
	TInt copyLength;

	copyLength = aLength > aLocalStore.MaxLength() ? aLocalStore.MaxLength() : aLength;

	TPtrC pData = iReadBuf.Mid( aStart, copyLength );
	aLocalStore.Copy( pData );
	aLocalStore.Trim();

	if (copyLength < aLength)
		{
		LOGGER_WRITE_2( "Data length: %d, stored: %d", aLength, copyLength );	
		}
	}

// -----------------------------------------------------------------------------
// CVMessageParser::WriteMessagePropertiesL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::WriteMessagePropertiesL()
	{
	LOGGER_WRITE( "CVMessageParser::WriteMessagePropertiesL()" );
	WriteMessageLineL( KVMsgTagVersion, KVMsgVersion );
	WriteMessageLineL( KVMsgTagStatus, iStatus );
	WriteMessageLineL( KVMsgTagBox, iFolder );
	WriteTaggedTimestampL( iUniversalTime );
	}

// -----------------------------------------------------------------------------
// CVMessageParser::WriteMessageVCARDL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::WriteMessageVCARDL(const TDesC& aName, const TDesC& aNumber)
	{
	LOGGER_WRITE( "CVMessageParser::WriteMessageVCARDL()" );
	WriteMessageLineL( KVMsgTagBegin, KVMsgSectionVCard );
	WriteMessageLineL( KVMsgTagVersion, KVMsgVCardVersion );
	WriteMessageLineL( KVMsgTagName, aName );
	WriteMessageLineL( KVMsgTagTelephone, aNumber );
	WriteMessageLineL( KVMsgTagEnd, KVMsgSectionVCard );
	}

// -----------------------------------------------------------------------------
// CVMessageParser::WriteMessageEnvelopeL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::WriteMessageEnvelopeL()
	{
	LOGGER_WRITE( "CVMessageParser::WriteMessageEnvelopeL()" );

	TInt i;
	TInt numRecipients = iRecipients.Count();

	for (i = 0; i < numRecipients; i++)
		{
		WriteMessageLineL( KVMsgTagBegin, KVMsgSectionVEnv );
		WriteMessageVCARDL( iRecipients[i].iName, iRecipients[i].iNumber );		
		}

	WriteMessageLineL( KVMsgTagBegin, KVMsgSectionVEnv );
	WriteMessageBodyL();
	WriteMessageLineL( KVMsgTagEnd, KVMsgSectionVEnv );

	for (i = 0; i < numRecipients; i++)
		{
		WriteMessageLineL( KVMsgTagEnd, KVMsgSectionVEnv );	
		}	
	}

// -----------------------------------------------------------------------------
// CVMessageParser::WriteTaggedTimestampL()
// -----------------------------------------------------------------------------
//	
void CVMessageParser::WriteTaggedTimestampL( TTime aUniversalTime )
	{
	LOGGER_WRITE( "CVMessageParser::WriteTaggedTimestamp()" );
	
	TDateTime dateTime = aUniversalTime.DateTime();
	
	TBuf<16> timestamp;
	
	// Default timesamp format: yyyymmddThhmmssZ - like 20060329T091530Z	
	_LIT( KTimestampFormat, "%+04d%+02d%+02dT%+02d%+02d%+02dZ" );
	timestamp.Format( KTimestampFormat, 
		dateTime.Year(),
		dateTime.Month() + 1,
		dateTime.Day() + 1,
		dateTime.Hour(),
		dateTime.Minute(),
		dateTime.Second() );
		
	WriteMessageLineL( KVMsgTagDateTime, timestamp );	
	}

// -----------------------------------------------------------------------------
// CVMessageParser::WriteMessageBodyL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::WriteMessageBodyL()
	{
	LOGGER_WRITE( "CVMessageParser::WriteMessageBodyL()" );

	WriteMessageLineL( KVMsgTagBegin, KVMsgSectionVBody );

    // "Date" field is always device local time, not UTC
	TBuf<KDateMaxLength> dateStr;
	_LIT( KDateFormat, "%F%*D.%*M.%Y% %H:%T:%S" );
	iHomeTime.FormatL( dateStr, KDateFormat );
	WriteMessageLineL(KVMsgTagDate, dateStr );

	if (iMessageBody)
		{
		LOG(*iMessageBody);
		WriteMessageLineL( KDesNoData, *iMessageBody, EFalse );
		}
	else
		{
		LOGGER_WRITE( "Message body empty" );
		}

	WriteMessageLineL( KVMsgTagEnd, KVMsgSectionVBody );
	}

// -----------------------------------------------------------------------------
// CVMessageParser::WriteMessageLineL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::WriteMessageLineL( const TDesC& aMsgLineTag, const TDesC& aMsgLineData, TBool aIndent )
	{
	if (aIndent)
		{
		TInt indent = GetIndentLevel( aMsgLineTag );
		TBuf<KTagAndValueMaxLength> indentSpace;
		indentSpace.AppendFill( TChar(' '), indent );
		WriteToMessageL( indentSpace );
		}

	if (aMsgLineTag.Length() > 0)
		{
		WriteToMessageL( aMsgLineTag );
		}

	WriteToMessageL( aMsgLineData );	
	WriteToMessageL( KVMsgLineFeed );
	}

// -----------------------------------------------------------------------------
// CVMessageParser::WriteToMessageL()
// -----------------------------------------------------------------------------
//
void CVMessageParser::WriteToMessageL( const TDesC &aData )
	{
	TPtrC8 writeData;

	const TUint16* ptr16 = aData.Ptr();
	const TUint8* ptr8 = reinterpret_cast<const TUint8*>(ptr16);
	writeData.Set( ptr8, aData.Length()*2 );

	TInt newPosition = iWriteBufPosition + writeData.Length();

	if (newPosition > iWriteBufSize)
		{
		LOGGER_WRITE( "Expand write buffer" );
		TInt expandStep = newPosition - iWriteBufSize + 100;
		iWriteBuf->ExpandL( iWriteBufSize, expandStep );
		iWriteBufSize += expandStep;
		LOGGER_WRITE_1( "Write buffer expanded to %d bytes", iWriteBufSize );
		}

	iWriteBuf->Write( iWriteBufPosition, writeData );
	iWriteBufPosition = newPosition;
	}


// -----------------------------------------------------------------------------
// CVMessageParser::GetIndentLevel()
// -----------------------------------------------------------------------------
//
TInt CVMessageParser::GetIndentLevel( const TDesC& aMsgLineTag )
	{
	if (aMsgLineTag.Compare( KVMsgTagEnd ) == 0)
		{
		iWriteIndentLevel--;
		}
	TInt result = iWriteIndentLevel * KIndentStepSize;

	if (aMsgLineTag.Compare( KVMsgTagBegin ) == 0)
		{
		iWriteIndentLevel++;
		}
	return result;
	}

// -----------------------------------------------------------------------------
// CVMessageParser::AssignDataToBuffer()
// -----------------------------------------------------------------------------
//
void CVMessageParser::AssignDataToBuffer( const TDesC &aData, TDes &aBuffer )
	{
	aBuffer.Zero();

	if (aBuffer.MaxLength() >= aData.Length())
		{
		aBuffer = aData;
		}
	else
		{
		LOGGER_WRITE_1( "AssignDataToBuffer: too short buffer for data: %d", aBuffer.MaxLength() );
		LOG( aData );
		aBuffer = aData.Left( aBuffer.MaxLength() );
		}
	}


//  End of File  
