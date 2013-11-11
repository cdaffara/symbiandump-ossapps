// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cstoreutilities.h"
#include <miuthdr.h>
#include <barsread.h>

// The maximum number of octets to be uuencoded on each line is 45
const TInt KUuDecodedLineLength = 45;	
// Initial size of buffer in which the "Real name <user@host>" strings
// are built before they're put into the CImHeader and the size by
// which to increment the buffer when necessary
const TInt KImapAddressSizeInc=256;
const TInt KKiloByteSize = 1024;
const TInt KBufferExtension = 4;
/**
Constructor.
*/
CStoreUtilities::CStoreUtilities(TImEncodingType aEncodingType,TUint aCharsetId,RFs& aFs)
	:iEncodingType(aEncodingType),iCharsetId(aCharsetId),iFs(aFs)
	{
	}
	
/**
Factory constructors.
Ownership aHeader is taken.
*/	
CStoreUtilities* CStoreUtilities::NewL(TImEncodingType aEncodingType,TUint aCharsetId,RFs& aFs)
	{
	CStoreUtilities* self=new(ELeave) CStoreUtilities(aEncodingType,aCharsetId,aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}
	
void CStoreUtilities::ConstructL()
	{
	// Create converter objects
	iCharacterConverter=CCnvCharacterSetConverter::NewL();
	iCharConv=CImConvertCharconv::NewL(*iCharacterConverter, iFs);	
	}

/**
Destructor.
*/	
CStoreUtilities::~CStoreUtilities()
	{
	delete iCharConv;
	delete iCharacterConverter;
	delete iFooterString;
	delete iPartialLine;
	}

/**
Checks if the descriptor contains the UUE begin header
Extracts the file name if it is
*/	
TBool CStoreUtilities::CheckUUEStartL(const TDesC8& aSourceLine)
	{
	
	TInt sourceLength = aSourceLine.Length();
	if(sourceLength < KImcvUueStart().Length()+3) // can't be "begin ###", it's not long enough; 3=length of ###
		return EFalse;

	if(!aSourceLine.Left(KImcvUueStart().Length()).CompareF(KImcvUueStart)) // start of line might be UUE boundary
		{
		// we also need to check that the next three chars are numbers - Unix file access code
		const TUint8* sourceLinePtr = aSourceLine.Ptr();
		TInt length=KImcvUueStart().Length();// this defines length as 6 ie. "b e g i n  " 
		if( TChar(sourceLinePtr[length]).IsDigit() && 
			TChar(sourceLinePtr[length+1]).IsDigit() && 
			TChar(sourceLinePtr[length+2]).IsDigit() )
			{
			// Found 'begin ###' at the start of a line - assume this is a UUencode header
			// The attachment name in this header is ignored. We use the value from the MIME header
			return ETrue;
			}
		}
		
	return EFalse;
	}
	
/**
Appends data to partial line object extening the destination buffer as appropirate 
@param aBufferPtr the destination bufer
@param aText the data to be appended
*/	
	
void CStoreUtilities::AppendExtendL(HBufC8** aBufferPtr, const TDesC8& aText)
	{
	HBufC8 *buffer = *aBufferPtr;
	TInt32 space = buffer->Des().MaxLength() - (buffer->Des().Length() + aText.Length());
	if (space < 0)
		{
		TInt32 inc = (-space) + KImapAddressSizeInc - ((-space) % KImapAddressSizeInc);
		TInt32 newSize = buffer->Des().MaxLength() + inc;
		HBufC8 *newBuf = buffer->ReAllocL(newSize);
		*aBufferPtr = buffer = newBuf;
		}
	buffer->Des().Append(aText);
	}
	
	
void CStoreUtilities::AttachFooterInfoL(TInt32 aBodyPartRemainingSize,CImapSettings& aImapSettings,HBufC8*& aDecodedData)
	{
	const HBufC8* buf= aImapSettings.DefaultPartialMessageFooter();

	_LIT(KIntegerDirective,"%d");
	const TInt KNoOfDigitsInMailSize = 10; // maximum length for no of digits for remaining body parts size
	TResourceReader reader;
	reader.SetBuffer(buf);
	// Check if %d is not found in the resource string (To avoid problems due to localisation)
	if(buf->Find((TDesC8&)KIntegerDirective) == KErrNotFound)
		{
		iFooterString = (reader.ReadTPtrC()).AllocL();
		}
	else
		{
		HBufC* resourceBuf = (reader.ReadTPtrC()).AllocL();
		CleanupStack::PushL(resourceBuf);
		iFooterString = HBufC::NewL(resourceBuf->Length()+KNoOfDigitsInMailSize);
		iFooterString->Des().Format(*resourceBuf,(aBodyPartRemainingSize / KKiloByteSize));
		CleanupStack::PopAndDestroy(resourceBuf);
		}	
	//caller will have left aDecodedData on the cleanupstack
	CleanupStack::Pop(aDecodedData); 
	TInt newSize = aDecodedData->Size() + iFooterString->Size();
	aDecodedData = aDecodedData->ReAlloc(newSize);
	CleanupStack::PushL(aDecodedData);
	aDecodedData->Des().Append(*iFooterString);
	delete iFooterString;
	iFooterString = NULL;
			
	}
	

// convert text from its charset and write to richtext store. aText
// can span multiple and partial lines
void CStoreUtilities::WriteToBodyL(const TDesC8& aText,CRichText* aMessageBody)
	{
	TInt pos = aMessageBody->DocumentLength();

	// Add bits of body text, converting along the way, till no characters left
	// .. to convert.

	// Convert text before writing to body.
	TInt rem = 0;

	// there will be a max of one output char per input byte
	HBufC16* text16=HBufC16::NewLC(aText.Length());
	TPtr16 ptr16=text16->Des();
	
	TBool preparedToConvert = iCharConv->PrepareToConvertToFromOurCharsetL(iCharsetId);
	
	if (!preparedToConvert)
		{
		ptr16.Copy(aText);
		aMessageBody->InsertL(pos, ptr16);
		}
	else
		{
		TInt unconvertedChars, firstPos; // not used 
		rem = iCharConv->ConvertToOurCharsetL(aText, ptr16, 
											  unconvertedChars, firstPos);
		if (rem < 0) // error
			{
			// Copy unconverted characters.
			ptr16.Copy(aText);
			aMessageBody->InsertL(pos, ptr16);
			}
		else if (rem && rem < iLeftOver.MaxLength())
			{
			iLeftOver.Copy(aText.Right(rem));	
			}

		// convert CRLF to ELineBreak
		TInt start = 0;
		TInt length = ptr16.Length();
		
		if (length>0)
			{
			TInt i;
			for (i=1; i<length; i++)
				{
				if (ptr16[i-1] == KImcvCR && ptr16[i] == KImcvLF)
					{
					ptr16[i-1] = CEditableText::ELineBreak;

					// write this line to body
					TPtrC ptr = ptr16.Mid(start, i-start);
					aMessageBody->InsertL(pos, ptr);
					pos += ptr.Length();
					start = i+1;
					}
				}

			if (start != i)
				{
				TPtrC ptr = ptr16.Mid(start, i-start);
				aMessageBody->InsertL(pos, ptr);
				}
			}
		}

	CleanupStack::PopAndDestroy(text16);
	}
	
// Decode and store received data
HBufC8* CStoreUtilities::DecodeL(const TPtrC8& aBodyData, const TBool aEndOfStream)
	{
	// Somewhere to store decoded data, at least as long as source (plus anything we have left
	// in the partial line buffer which may now get consumed)
	TInt outputbuffersize=aBodyData.Length()+KBufferExtension;
	if (iPartialLine)
		outputbuffersize+=iPartialLine->Des().Length();

	HBufC8* decoded=HBufC8::NewL(outputbuffersize);
	TPtr8 decoded_ptr=decoded->Des();

	// Which decoder are we using?
	switch(iEncodingType)
		{
		case EEncodingTypeNone:
		case EEncodingType7Bit:
		case EEncodingType8Bit:
		case EEncodingTypeBinary:
		case EEncodingTypeUnknown:
			// Nothing to do, just copy data
			decoded->Des().Append(aBodyData);
			break;

		case EEncodingTypeBASE64:
			// Decode Base64 data: just filter it through decoder, it
			// ignores line breaks anyway.
			iB64Decoder.Decode(aBodyData,decoded_ptr);
			break;

		case EEncodingTypeUU:
			{
			TPtrC8 bodydata=aBodyData;

			// Got a partial buffer?
			if (!iPartialLine)
				{
				// Allocate buffer
				iPartialLine=HBufC8::NewL(KUuDecodedLineLength);
				iUUDecoding = EFalse;
				}
			
			// Decode UUEncoded data: line by line
			TBool decodeEnded = EFalse;
			TInt position=0;
			while ( bodydata.Length() && !decodeEnded )
				{
				// Find() returns the start of "\r\n". The decoding algorithm
				// requires that the encoded line contains the "\r\n".
				TInt lineEnd = bodydata.Find( _L8("\r\n") );
				if (lineEnd != KErrNotFound)
					{
					lineEnd = lineEnd + 2;
					AppendExtendL( &iPartialLine, bodydata.Left( lineEnd ));
				
					bodydata.Set( bodydata.Mid( lineEnd ) );
				
					// Check for a well-formated  begin-tag
					if ( CheckUUEStartL( iPartialLine->Des() ) )
						{
						iUUDecoding = ETrue;
						}
					else if ( iPartialLine->Compare( KImcvUueEnd ) != 0 && iUUDecoding )
						{
						// Every malformatted string is decoded as an empty string 
						// with length 0. Appending such a string is harmless.
						TPtr8 destination((unsigned char*)decoded_ptr.Ptr()+position,0,outputbuffersize-position);
						iUUDecoder.Decode(*iPartialLine,destination);
						position+=destination.Length();			
						}
					else if ( iUUDecoding )
						{
						decodeEnded = ETrue;
						iUUDecoding = EFalse;	
						}
					
					iPartialLine->Des().Zero();
					}
				else
					{
					AppendExtendL( &iPartialLine, bodydata);
					
					// advance to end of bodydata
  					bodydata.Set(bodydata.Ptr()+bodydata.Length(), 0);
					}
				}
			decoded->Des().SetLength(position);	
			break;
			}

		case EEncodingTypeQP:
			{
			TPtrC8 bodydata=aBodyData;

			// Got a partial buffer?
			if (!iPartialLine)
				{
				// Allocate buffer
				iPartialLine=HBufC8::NewL(256);
				}

			// Build buffer to decode: basically, QP decoder wants CRLF terminated
			// lines, so we build them in the iPartialLine buffer. There may be
			// stuff already there from previous data packet - so we just append.
			TInt position=0;
			while(bodydata.Length())
				{
				// Find a line break
				TInt lineend=bodydata.Find(_L8("\r\n"));

				// No break?
				if (lineend==KErrNotFound && !aEndOfStream)
					{
					// Stick it all in the partialline buffer, we should get a CRLF
					// soon...
					AppendExtendL( &iPartialLine,bodydata);
					break;
					}
				else
					{
					if (lineend==KErrNotFound)
						{
						// Append whole thing left to buffer
						AppendExtendL( &iPartialLine,bodydata);

						// advance to end of bodydata
						bodydata.Set(bodydata.Ptr()+bodydata.Length(), 0);
						}
					else
						{
						// Append to buffer up to that point (including the \r\n)
						AppendExtendL( &iPartialLine,bodydata.Left(lineend+2));

						// Remove from the buffer we're working on (including the \r\n)
						bodydata.Set(bodydata.Ptr()+lineend+2,bodydata.Length()-lineend-2);
						}

					// Decode & skip on in buffer
					TPtr8 destination((unsigned char*)decoded_ptr.Ptr()+position,0,outputbuffersize-position);
					iQPDecoder.Decode(*iPartialLine,destination);
					position+=destination.Length();
					iPartialLine->Des().Zero();
					}
				}

			// Update decoded
			decoded->Des().SetLength(position);
			break;
			}
		}

	// put back any partially converted data
	if (iLeftOver.Length())
		{
		decoded->Des().Insert(0, iLeftOver);
		iLeftOver.SetLength(0);
		}
		
	return decoded;
	}

