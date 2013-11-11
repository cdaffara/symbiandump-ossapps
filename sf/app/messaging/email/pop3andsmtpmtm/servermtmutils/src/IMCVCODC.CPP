// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "IMCVCODC.H"
#include "IMUTDLL.H"
#include "IMCVSEND.H"

const TInt8 AsciiToBase64[80]=
	{
	 62, -1, -1, -1, 63, 52, 53, 54, 55, 56,
	 57, 58, 59, 60, 61, -1, -1, -1, 64, -1,
	 -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,
	  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
	 18, 19, 20, 21, 22, 23, 24, 25, -1, -1,
	 -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
	 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
	};

const TInt8 Base64ToAscii[65]=
	{
	 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
	 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
	 85, 86, 87, 88, 89, 90, 97, 98, 99,100,
	101,102,103,104,105,106,107,108,109,110,
	111,112,113,114,115,116,117,118,119,120,
	121,122, 48, 49, 50, 51, 52, 53, 54, 55,
	 56, 57, 43, 47, 61 
	};

const TInt8 KImcvLookUpStartOffset = 43;
const TUint8 KImcvConvEquals = '=';

const TInt KImMaxLengthForEncodedWordAndExtraEncodingTokens = 9;

// Define the maximum length that one encoded character should occupy.
// For certain encoding schemes such as Japanese, a single unicode character
// may need a multi character string to represent it when it is sent as part
// of an email header. This constant defines a maximum length for that string.
// The value is based on the maximum length for a Japanese character
const TInt KImMaxLengthForEncodedChar = 8;


//---------------------------------------------------------------------------------
//              Class TImNoCodec Functions
//---------------------------------------------------------------------------------


//
TInt TImCodecNull::Encode( const TDesC8& aSrcString, TDes8& rDestString)
	{
	rDestString.Copy(aSrcString);
	return 0;
	}

//	
TBool TImCodecNull::Decode( const TDesC8& aInputLine, TDes8& rOutputLine)
	{
	rOutputLine.Copy(aInputLine);
	return ETrue;
	}



//---------------------------------------------------------------------------------
//              Class TImCodecQP Functions
//
//			Utility class, providing encoding/decoding functions for :
//			Quotable-printable, Base64 & UU.
//			Input/output consisting of 8bit strings.
//---------------------------------------------------------------------------------

//
EXPORT_C TImCodecQP::TImCodecQP() : iPlainCharList(TPtrC8()), iEncodeCharList(TPtrC8())
	{
	iQPCharacter=KImcvEquals;
	}

//
TBool TImCodecQP::SmartBreak( TInt written, const TDesC8& pSource )
	{
	TLex8 source( pSource );

	// only check for whether we should break if the current char is breakable
	if ( !IsBreakable(source.Peek()) )
		return EFalse;

 	// scan ahead looking for the next breakable char.

	source.Inc();
	TBool found = EFalse;
	TInt encodingOffset=(IsPlain(source.Peek()) ? 0 : 2);
	while ( !found && !source.Eos() )
		{
		found = IsBreakable(source.Peek());
		if (!found)
			{
			encodingOffset+=(IsPlain(source.Peek()) ? 0 : 2);
			source.Inc();
			}
		}

	// there's another breakable char before the end of the text 
	// - we need to break now if it's too far away
	// but only if the non-breakable text fits on a line itself.

	return ((written+source.Offset()+encodingOffset) > KMaxIMailBodyLineLength-4);
	}

//
EXPORT_C TInt TImCodecQP::Encode( const TDesC8& aInputLine, TDes8& rOutputLine)
	{
	 // number of characters written to the current line.
	TInt written=0; 

	// Used to check we do not 'fold' a line after encountering an ESC character.
	TInt escapeChar=0;

	// number of characters introduced into the line by the encoding mechanism.
	TInt localPaddingCount=0; 
	
	TChar nextChar=0;

	// buffer used for preparing the hex encoding of a non-printable char
	TBuf8<5> formatted; 
	
	TLex8 lexSource(aInputLine);
	TDes8& ptr = rOutputLine;
	TInt ptrEnd = ptr.MaxLength();

	if (!lexSource.Eos())
		{
		lexSource.Get();
		nextChar = lexSource.Peek();
		lexSource.Inc(-1);
		}

	while (written<KImMailMaxBufferSize-2 && !lexSource.Eos())
		{
		if (written>=KMaxIMailBodyLineLength-3 && !escapeChar)
			{
			// force a soft line break.
			__ASSERT_ALWAYS( ptr.Length()+4< ptr.MaxLength(), gPanic(KPanicDescriptorToSmall) );
			ptr.Append(iQPCharacter);
			ptr.Append(KImcvCRLF);
			written+=3; localPaddingCount+=3;
			break;
			}

		TChar peek = lexSource.Peek();

		if (peek==KImcvESC)
			escapeChar=3;

		if ( SmartBreak(written,lexSource.Remainder()) && !escapeChar)
			{
			AddSoftLineBreak( ptr, localPaddingCount, written);
			break;
			}
		else if ( IsPlain(peek) && peek!=KImcvSP && peek!=KImcvEquals )
			// simple case, printable character
			{
			__ASSERT_ALWAYS( ptr.Length()+1<ptrEnd, gPanic(KPanicDescriptorToSmall) );
			ptr.Append(peek);
			written++;
			}
		else if ( peek==KImcvTab || peek==KImcvSP )
			// Tab or space, need to check for immediate EOL so we can keep the space character
			{
			TChar thisChar =lexSource.Get();
			TChar nextChar =(lexSource.Eos() ? (TChar)0 : lexSource.Get());

			if (nextChar==KImcvCR && lexSource.Peek()==KImcvLF)
				// Encode space, as EOL follows.
				{
				__ASSERT_ALWAYS( ptr.Length()+5<ptrEnd, gPanic(KPanicDescriptorToSmall) );
				formatted.Format( KImcvQPFormatString, (TUint) thisChar );
				formatted.UpperCase();
				ptr.Append( formatted );

				// also skip EOL the characters we just allowed for
				written+=5;
				localPaddingCount+=2;
				break;
				}
			else
				// no EOL, just carry on after the whitespace
				{
				__ASSERT_ALWAYS( ptr.Length()+1<ptrEnd, gPanic(KPanicDescriptorToSmall) );
				ptr.Append( thisChar );
				written++;
				lexSource.Inc(-1);
				if (nextChar)
					lexSource.Inc(-1);
				}
			}
		else if (peek==KImcvCR && nextChar==KImcvLF)
			// Do not encode.
			{
			__ASSERT_ALWAYS( ptr.Length()+2<ptrEnd, gPanic(KPanicDescriptorToSmall) );
			ptr.Append(KImcvCRLF);
			written+=2;
			lexSource.Inc();
			break;
			}
		else if ( (TUint8)peek==iQPCharacter )
			// '=' in source text
			{
			__ASSERT_ALWAYS( ptr.Length()+3<ptrEnd, gPanic(KPanicDescriptorToSmall) );
			ptr.Append( KImcvQPEqualsSign );
			written += 3;
			localPaddingCount+=2;
			}
		else if (peek==KImcvTab || peek==KImcvHyphen || peek==KImcvSP)
			{
			__ASSERT_ALWAYS( ptr.Length()+1<ptrEnd, gPanic(KPanicDescriptorToSmall) );
			ptr.Append( peek );
			written++;
			}
		else
			// non-printable char, must encode with Hex value
			{
			__ASSERT_ALWAYS( ptr.Length()+3<ptrEnd, gPanic(KPanicDescriptorToSmall) );
			formatted.Format( KImcvQPEncoded, (TUint) peek );
			formatted.UpperCase();
			ptr.Append(formatted);
			written += 3;
			localPaddingCount += 2;
			}
		
		// advance to next source character	
		lexSource.Inc();
		if (escapeChar)
			escapeChar--;

		// check whether we should give up without creating all the output
		// non-quotable char to come & we're nearly at max length.

		TChar thisChar = lexSource.Get();

		// just in case we're pointing at the last character
		nextChar = (lexSource.Eos() ? (TChar)0 : lexSource.Peek());
		if (thisChar != 0)
			lexSource.Inc(-1);

		} // while

	return written-localPaddingCount;
	}

//
EXPORT_C TInt TImCodecQP::Decode( const TDesC8& aSrcString, TDes8& rDestString )
	{
	TInt error = KErrNone;

	__ASSERT_DEBUG(aSrcString.Length(), gPanic(KPanicInvalidSMTPLine));

	rDestString = KNullDesC8;

	TPtrC8 source( aSrcString.Ptr(), aSrcString.Length() );
	const TUint8* pSource = source.Ptr();
	const TUint8* pEnd = pSource+aSrcString.Length();
	
	// find out if this is a blank line, if so then we'll add a paragraph delimiter instead
	// assume it's blank and then look for non-blank characters
	// avoid the CRLF at the end of the line (we know it's there thanks to the assertion above)

	TBool blankLine = ETrue; 
	while (pSource < pEnd-2) 
		{
		if (*pSource!=KImcvSP && *pSource!=KImcvTab)
			{
			blankLine = EFalse;
			break;
			}
		pSource++;
		}

	if ( blankLine )
		{
		rDestString.Copy( aSrcString );
		return KErrNone;
		}

	TInt outputLength=0;
	TUint8 loBits;
	TUint8 hiBits;
	TUint8 asciiValue;
	pSource = source.Ptr();	// reset to start of source data
	const TUint8 zero = '0';
	const TUint8 alphaAdjust = 55;  // 'A' is ascii 65 so we need to subtract 55 from 
									// alphabetical hex digits to get their numeric value
	while( pSource < pEnd )
		{
		if (*pSource != iQPCharacter )
			{
			//  Quoted character or Attachment bound, just bung it on & move to the next one
			// *ptr++ = *pSource;
			outputLength++;
			rDestString.Append( *pSource );
			}
		else	// check for encoded character
			{
			// start looking at the next two characters, if they are there.

			if ( pSource+2 < pEnd )
				{
				pSource++;

				// check for '=' at EOL => this is a soft break, so remove it
				if (*pSource != KImcvCR) 
					{
					if(*pSource != KImcvLF) 
						{
					 	 // now decode hex value into ASCII code : hi-order bits come first
						 hiBits = (TUint8)(0x0F & (IsDigit( *pSource ) ? (TUint8)(*pSource-zero) : (TUint8)(*pSource-alphaAdjust)));
						 pSource++;
						 loBits = (TUint8)(0x0F & (IsDigit( *pSource ) ? (TUint8)(*pSource-zero) : (TUint8)(*pSource-alphaAdjust)));
						 asciiValue = (TUint8)( (hiBits<<4) + loBits);
						 // bung the character thus formed onto the decoded string
						 rDestString.Append( asciiValue );
						 // *ptr++ = asciiValue;
						 outputLength++;
						}
					}
				else
					{
					pSource++;
					if(*pSource != KImcvLF)
						{
						error=KErrCorrupt;
						pSource-=2;
						rDestString.Append( *pSource );
						pSource++;
						rDestString.Append( *pSource );
						pSource++;
						outputLength+=2;
						}
					}
				}
			else
				{
				// copy the rest of the data & use up the input string in the process.

				while (pSource < pEnd)
					{
					error=KErrCorrupt; // not QP compliant
					//*ptr++ = *pSource++;
					outputLength++;
					rDestString.Append( *pSource );
					pSource++;
					}
				}
			} // check for '=' char
		
		pSource++;  // next source charactery
	} // while

	rDestString.SetLength(outputLength);

	return error;
	}


// Not used. Remove in 6.3
//
EXPORT_C TInt TImCodecQP::EncodeRichText( const TDesC8& aInputLine, TDes8& rOutputLine)
	{
	TInt written=0;  // number of characters written to the current line
	TInt localPaddingCount=0; // number of characters introduced into the line by the encoding mechanism
	
	// buffer used for preparing the hex encoding of a non-printable char
	TBuf8<5> formatted; 
	
	TLex8 lexSource(aInputLine);
	TDes8& ptr = rOutputLine;
	TInt ptrEnd = ptr.MaxLength();

	while (written < KMaxIMailBodyLineLength-3 &&!lexSource.Eos())
		{
		if ( SmartBreak(written,lexSource.Remainder()) )
			{
			AddSoftLineBreak(ptr, localPaddingCount, written);
			break;
			}

		TChar peek = lexSource.Peek();
		if ( IsPlain(peek)  && peek!=KImcvSP )
			// simple case, printable character
			{
			__ASSERT_ALWAYS( ptr.Length()+1<ptrEnd, gPanic(KPanicDescriptorToSmall) );
			ptr.Append(peek);
			written++;
			}
		else if ( peek==KImcvTab || peek==KImcvSP )
			// Tab or space, need to check for immediate EOL so we can keep the space character
			{
			TChar thisChar = (lexSource.Eos() ? (TChar)0 : lexSource.Get());

			if ( lexSource.Peek()==ETextParagraphDelimiter )
				{
				__ASSERT_ALWAYS( ptr.Length()+5<ptrEnd, gPanic(KPanicDescriptorToSmall) );
				formatted.Format( KImcvQPFormatString, (TUint) thisChar );
				formatted.UpperCase();
				ptr.Append( formatted );
				ptr.Append(KImcvCRLF);

				// also skip EOL the characters we just allowed for
				written+=7;
				localPaddingCount+=4;

				// advance to next source character	
				lexSource.Inc();
				break;
				}
			else
				// no EOL, just carry on after the whitespace
				{
				__ASSERT_ALWAYS( ptr.Length()+1<ptrEnd, gPanic(KPanicDescriptorToSmall) );
				ptr.Append( thisChar );
				written++;
				lexSource.Inc(-1);
				}
			}
		else if ( (TUint8)peek==iQPCharacter )
			// '=' in source text
			{
			//is there enough space to encoded a char and put a soft break on the end.
			if(written > KMaxIMailBodyLineLength-6)
				break;

			__ASSERT_ALWAYS( ptr.Length()+3<ptrEnd, gPanic(KPanicDescriptorToSmall) );
			ptr.Append( KImcvQPEqualsSign );
			written += 3;
			localPaddingCount+=2;
			}
		else if (peek<KImcvSP  && peek!=KImcvLF && peek !=KImcvCR )
			// check for CEditableText control character
			{
			TChar replacement = ReplacementChar(peek);
			if (replacement)
				{
				__ASSERT_ALWAYS( ptr.Length()+1<ptrEnd, gPanic(KPanicDescriptorToSmall) );
				ptr.Append( replacement );
				written++;
				}
			else if ( lexSource.Peek()==ETextParagraphDelimiter )
				{
				ptr.Append(KImcvCRLF);
				written+=2;
				localPaddingCount+=1;
				break;
				}
			else
				localPaddingCount-=1;
			}
		else
			// non-printable char, must encode with Hex value
			{
			//is there enough space to encoded a char and put a soft break on the end
			if(written > KMaxIMailBodyLineLength-6)
				{
				if (written+3 < KMaxIMailBodyLineLength)
					{
					AddSoftLineBreak(ptr, localPaddingCount, written);
					}
				break;
				}
			__ASSERT_ALWAYS( ptr.Length()+3<ptrEnd, gPanic(KPanicDescriptorToSmall) );
			formatted.Format( KImcvQPEncoded, (TUint) peek );
			formatted.UpperCase();
			ptr.Append(formatted);
			written += 3;
			localPaddingCount += 2;
			}
		
		// advance to next source character	
		lexSource.Inc();

		// check whether we should give up without creating all the output
		// non-quotable char to come & we're nearly at max length.

		TChar thisChar = lexSource.Get();
		// just in case we're pointing at the last character
		TChar nextChar = (lexSource.Eos() ? (TChar)0 : lexSource.Peek());
		if (thisChar != 0)
			lexSource.Inc(-1);

		if ( !IsPlain( thisChar ) && (written >= KMaxIMailBodyLineLength-2) 
			|| ( thisChar==KImcvSP ||  thisChar==KImcvTab) 
			&& nextChar==CEditableText::EParagraphDelimiter 
			&& (written >= KMaxIMailBodyLineLength-3) ) // whitespace just before linebreak
			break;
		} // while

	rOutputLine.SetLength( written );

	return 	(written-localPaddingCount);
	}


EXPORT_C TInt TImCodecQP::DecodeRichText( const TDesC8& aSrcString, TDes& rDestString )
	{
	TInt error = KErrNone;

	__ASSERT_DEBUG(aSrcString.Length()>=2&&aSrcString[aSrcString.Length()-2]==KImcvCR&&aSrcString[aSrcString.Length()-1]==KImcvLF, gPanic(KPanicInvalidSMTPLine));

	rDestString = KNullDesC;

	TPtrC8 source( aSrcString.Ptr(), aSrcString.Length() );
	const TUint8* pSource = source.Ptr();
	const TUint8* pEnd = pSource+aSrcString.Length();
	
	// find out if this is a blank line, if so then we'll add a paragraph delimiter instead
	TBool blankLine = ETrue; // assume it's blank and then look for non-blank characters
	while (pSource < pEnd-2) // avoid the CRLF at the end of the line (we know it's there thanks to the assertion above)
		{
		if (*pSource!=KImcvSP && *pSource!=KImcvTab)
			{
			blankLine = EFalse;
			break;
			}
		else
			pSource++;
		}

	if ( blankLine )
		rDestString.Copy( aSrcString )
		;	
	else
		{
		TInt outputLength=0;
		TUint8 loBits;
		TUint8 hiBits;
		TUint8 asciiValue;
		pSource = source.Ptr();	// reset to start of source data
		const TUint8 zero = '0';
		const TUint8 alphaAdjust = 55;  // 'A' is ascii 65 so we need to subtract 55 from 
										// alphabetical hex digits to get their numeric value
		while( pSource < pEnd )
			{
			// check for encoded character
			if (*pSource == iQPCharacter )
				{
				// start looking at the next two characters, if they are there
				if ( pSource+2 < pEnd )
					{
					pSource++;
					// check for '=' at EOL => this is a soft break, so remove it
					if (*pSource != KImcvCR) 
						{
						// now decode hex value into ASCII code : hi-order bits come first
						hiBits = (TUint8)(0x0F & (IsDigit( *pSource ) ? (TUint8)(*pSource-zero) : (TUint8)(*pSource-alphaAdjust)));
						pSource++;
						loBits = (TUint8)(0x0F & (IsDigit( *pSource ) ? (TUint8)(*pSource-zero) : (TUint8)(*pSource-alphaAdjust)));
						asciiValue = (TUint8)( (hiBits<<4) + loBits);
						// bung the character thus formed onto the decoded string
						rDestString.Append( asciiValue );
						// *ptr++ = asciiValue;
						outputLength++;
						}
					else
						{
						pSource++;
						if(*pSource != KImcvLF)
							{
							error=KErrCorrupt;
							pSource-=2;
							rDestString.Append( *pSource );
							pSource++;
							rDestString.Append( *pSource );
							pSource++;
							outputLength+=2;
							}
						}
					}
				else
					{
					// copy the rest of the data & use up the input string in the process
					while (pSource < pEnd)
						{
						error=KErrCorrupt; // not QP compliant
						//*ptr++ = *pSource++;
						outputLength++;
						rDestString.Append( *pSource );
						pSource++;
						}
					}
				} // check for '=' char
			else
				{
				if ( *pSource == KImcvCR && (pSource+1 < pEnd && *(pSource+1) == KImcvLF))
					{
					// this is a hard CRLF, so replace it with a Paragraph Delimiter
					
					// *ptr++ = ETextParagraphDelimiter;
					rDestString.Append( (TChar) CEditableText::EParagraphDelimiter );
					outputLength++;
					
					// ... and skip an extra char (the LF)
					pSource++;
					}
				else
					{
					//  Quoted character or Attachment bound, just bung it on & move to the next one
					// *ptr++ = *pSource;
					outputLength++;
					rDestString.Append( *pSource );
					}
				}
			// next source character
			pSource++;
			} // while
		rDestString.SetLength(outputLength);
		} // else
	return error;
	}


//---------------------------------------------------------------------------------
//              Class TImFileCodec Functions
//---------------------------------------------------------------------------------
	

TInt TImFileCodec::PostfixNextLine(TDes8&  rOutputLine, TInt& rPaddingCount)
	{
	return BlankLine(rOutputLine, rPaddingCount);
	}


TInt TImFileCodec::PrefixNextLineL(TDes8& /* rOutputLine*/, const TFileName& /*aName*/, TInt& /*rPaddingCount*/)
	{
	return KImAttFinished;
	}


void TImFileCodec::Initialise()
	{
	iPostfixState=0;
	iPrefixState=0;
	}

//---------------------------------------------------------------------------------
//              Class TImCodecB64 Functions
//---------------------------------------------------------------------------------


EXPORT_C TImCodecB64::TImCodecB64(): iShiftStored(0), iMaskShiftStored(ESix)
	{}


// Returns ETrue if aSrcString is not long enough to decode fully, resulting in the storage of
// the last character and requiring another aSrcString (poss 0 length) to be passed to it to 
// clear this character. 
// Returns EFalse if the line was decoded OK or the end of the encoded file is reached ie "="
//
EXPORT_C TBool TImCodecB64::Decode(const TDesC8& aSrcString, TDes8& rDestString)
	{
	TInt decodedInt=0;
	TInt8 offsetChar=0;
	TUint8 decodedChar=0;
	 
	// Clears the destination string
	rDestString.Zero();

	// Initialise variables
	const TUint8* srcStringPtr=aSrcString.Ptr();
	const TUint8* srcStringEnd=aSrcString.Length()+srcStringPtr;
	TUint8* destStringPtr=(TUint8*)rDestString.Ptr();
	TUint8* destStringPtrBase=destStringPtr;

	TInt maskShift=iMaskShiftStored;
	TInt shiftStorage=iShiftStored;
	
	// Main character process loop
	while(srcStringPtr<srcStringEnd)	
		{
		offsetChar=(TInt8)(*srcStringPtr-KImcvLookUpStartOffset);
		srcStringPtr++;

		// Check for valid B64 character		
		if((offsetChar>=0)&&(offsetChar<80))
			{
			// Read in next character and B64 decode
			decodedInt=AsciiToBase64[offsetChar];

			// Exits when a PAD char is reached
			if(decodedInt==EPadChar)
				{
				rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
				return EFalse;
				}

			// Ensures the first 2 chars of 4 are received before processing
			if(maskShift==ESix)
				maskShift=EFour;
			else
				{
				shiftStorage=shiftStorage<<ESix;
				shiftStorage=shiftStorage|decodedInt;
				decodedChar=(TUint8)((shiftStorage>>maskShift)&EEightBitMask);
				
				if((maskShift-=ETwo)<EZero)
					maskShift=ESix; 
				
				*destStringPtr++=decodedChar;
				}
			shiftStorage=decodedInt;
			}
		}
	iShiftStored=shiftStorage;
	iMaskShiftStored=maskShift;
	
	rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
	
	return maskShift<ESix;
	}

EXPORT_C TInt TImCodecB64::Encode(const TDesC8& aSrcString, TDes8& rDestString)
	{
	return DoEncode(aSrcString, rDestString, EFalse);
	}

EXPORT_C void TImCodecB64::Initialise()
	{
	iMaskShiftStored=ESix;
	}

TInt TImCodecB64::DoEncode(const TDesC8& aSrcString, TDes8& rDestString, TBool aInsertLineBreaks)
	{
	// Clears the destination string
	rDestString.Zero();
	
	// Initialise variables
	const TUint8* srcStringPtr=aSrcString.Ptr();
	const TUint8* srcStringEnd=aSrcString.Length()+srcStringPtr;
	TUint8* destStringPtr=(TUint8*)rDestString.Ptr();
	TUint8* destStringPtrBase=destStringPtr;

	TInt character=0;
	TUint8 encodedChar=0;
	TInt charStorage=0;
 	TInt maskShift=EZero;
	TInt destStringCharNum = 0;

	while(srcStringPtr<=srcStringEnd)
		{
		// maskShift is used as a char read counter
		if(maskShift==ESix)
			{
			// If the 3rd char read is also the last char then the while loop
			// is broken on the next check.
			if(srcStringPtr==srcStringEnd)
				srcStringPtr++;
			maskShift=EZero;
			character=0;   
			}
		else
			{
			if(srcStringPtr==srcStringEnd)
				character=0;
			else
				character=*srcStringPtr;

			srcStringPtr++;
			// Shifts charStorage ready for the next char
			charStorage=charStorage<<8;
			maskShift+=ETwo;
			}
		charStorage=charStorage|character;
		// Shifts the mask to the correct bit location
		// Masks (AND's) the valid bits from charStorage
		// Shifts the valid bits into the low order 8bits
		// Converts to BASE64 char, Casts the result to an unsigned char (which it should be ?....I hope)
		encodedChar=(TUint8)Base64ToAscii[((charStorage>>maskShift)&ESixBitMask)];

		*destStringPtr++=encodedChar;
		destStringCharNum++;

		// Add a CRLF every KMaxB64EncodedCharsPerLine characters so as not to exceed the line length
		// limitation specified in RFC 2822.
		if (aInsertLineBreaks && destStringCharNum == KMaxB64EncodedCharsPerLine)
			{
			destStringCharNum = 0;
			*destStringPtr++ = '\r';
			*destStringPtr++ = '\n';
			}
		}
	
	// Check for not enough chars and pad if required
	if (maskShift==EFour)
		{
		*destStringPtr++=KImcvConvEquals;
		*destStringPtr++=KImcvConvEquals;
		}
	else
		if(maskShift==ESix)
			*destStringPtr++=KImcvConvEquals;	
			
	rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
	return ((TInt)(srcStringPtr-srcStringEnd));
	}

TInt TImCodecB64WithLineBreaks::Encode(const TDesC8& aSrcString, TDes8& rDestString)
   	{
	return DoEncode(aSrcString, rDestString, ETrue);
   	}

//---------------------------------------------------------------------------------
//              Class TImCodecUU Functions
//---------------------------------------------------------------------------------


EXPORT_C TImCodecUU::TImCodecUU()
	{}


// returns ETrue if aSrcString is not long enough, relative to its line length character.
// or if an invalid UU encoded character is found.
// returns EFalse if the line was decoded OK
//
EXPORT_C TBool TImCodecUU::Decode(const TDesC8& aSrcString, TDes8& rDestString)
	{
	// Clears the destination string
	rDestString.Zero();

	// Initialise variables
	const TUint8* srcStringPtr=aSrcString.Ptr();
	TInt numEncChrs=*srcStringPtr++-ESpace;
	TInt maskShift=ESix;
	TInt numDecChrs=0;
	TInt shiftStorage=0;
	TInt character=0;
	TUint8 decodedChar=0;
	
	// Checks if the srcString is atleast long enough to decode.
	if((numEncChrs*4)>((aSrcString.Length()-1)*3)) return ETrue;

	// Main character process loop
	while(numDecChrs<numEncChrs)	
		{
		// Read in next character
		character=*srcStringPtr++;

		// Check for UU character validity
		if((character<ESpace)||(character>EBackQuote)) return ETrue;

		// Protocol thing		
		if(character==EBackQuote)
			character=ESpace;

		character-=ESpace;
		
		// Ensures the first 2 chars of 4 are received before processing
		if(maskShift==ESix)
			maskShift=EFour;
		else
			{
			shiftStorage=shiftStorage<<ESix;
			shiftStorage=shiftStorage|character;

			decodedChar=(TUint8)((shiftStorage>>maskShift)&EEightBitMask);
			maskShift=((maskShift==EZero)? ESix:(maskShift-ETwo)); 
				
			rDestString.Append(decodedChar);
			numDecChrs++;
			}
		shiftStorage=character;
		}
	return EFalse;
	}


EXPORT_C TInt TImCodecUU::Encode(const TDesC8& aSrcString, TDes8& rDestString)
	{
	// Clears the destination string
	rDestString.Zero();
	
	// Initialise variables
	const TUint8* srcStringPtr=aSrcString.Ptr();
	const TInt srcLength=aSrcString.Length();
	const TUint8* srcStringEnd=srcStringPtr+srcLength;
	TInt charStorage=0;
	TInt maskShift=EZero;
	TInt character=0;
	TUint8 encodedChar=0;
	TInt destStringCharNum = 0;
	TInt srcStringBytesRemaining = 0;

   	while(srcStringPtr<=srcStringEnd)
   		{
		// Add unencoded length to start of line as per UU specification.
		if (destStringCharNum == 0)
			{
			srcStringBytesRemaining = srcStringEnd - srcStringPtr;
			if ( srcStringBytesRemaining < KMaxUUUnEncodedCharsPerLine ) 
				rDestString.Append(static_cast<TUint8>(srcStringBytesRemaining) + ESpace);
			else
				rDestString.Append(KMaxUUUnEncodedCharsPerLine + ESpace);
			}

		if(maskShift==ESix)
			{
			// If the 3rd char read is also the last char then the while loop
			// is broken on the next check.
			if(srcStringPtr==srcStringEnd)
				srcStringPtr++;
			character=0;
			maskShift=EZero;
			}
		else
			{
			if(srcStringPtr==srcStringEnd)
				character=0;
			else
				character=*srcStringPtr;

			srcStringPtr++;
			charStorage=charStorage<<8;
			maskShift+=ETwo;
			}
		charStorage=charStorage|character;
		// Shifts the mask to the correct bit location
		// Masks (AND's) the valid bits
		// Shifts the valid bits into the low order 8bits
		// Adds 32 (SPACE), Casts the result to an unsigned char (which it should be ?....I hope)
		encodedChar=(TUint8)(((charStorage>>maskShift)&ESixBitMask)+ESpace);

		// Protocol thing		
		if(encodedChar==ESpace)
			encodedChar=EBackQuote;

		rDestString.Append(encodedChar);
		++destStringCharNum;

		// Add a CRLF every KMaxUUEncodedCharsPerLine characters so as not to exceed the line length
		// limitation recommended for UUE.
		if (destStringCharNum == KMaxUUEncodedCharsPerLine)
			{
			destStringCharNum = 0;
			rDestString.Append(KImcvCRLF);
			}
		}

	return ((TInt)(srcStringPtr-srcStringEnd));
	}


TInt TImCodecUU::PrefixNextLineL( TDes8& rOutputLine, const TFileName& aName, TInt& rPaddingCount )
	{
	rOutputLine.Append( KImcvUueStart );
	rOutputLine.Append( KImcvUue644 );
	AppendFilenameL( rOutputLine, aName );
	rPaddingCount=rOutputLine.Length();
	return 1;
	}

TInt TImCodecUU::PostfixNextLine( TDes8& rOutputLine, TInt& rPaddingCount )
	{
	switch( iPostfixState )
		{
		case EInvertedComma:
			rOutputLine.Append( KImcvInvertedComma );
			break;
		case EEndString:
			rOutputLine = KImcvUueEnd;
			break;
		}
	
	iPostfixState++;
	rPaddingCount=rOutputLine.Length();

	return (iPostfixState==EEndOfPostfix ? KImAttFinished : KErrNone);
	}



void TImCodecUU::AppendFilenameL( TDes8& rOutputLine, const TFileName& aName)
	{
	HBufC8* buf = HBufC8::NewLC( aName.Length() );
	buf->Des().Copy( aName );
	rOutputLine.Append(KImcvSpace);
	rOutputLine.Append( *buf );
	CleanupStack::PopAndDestroy(); // buf
	}




//---------------------------------------------------------------------------------
//              Class CImConvertHeader Functions
//
// Utility class, providing functionality for encoding/decoding header fields.
// Requires use of Charconv "Wrapper" class.
//------------------------------------------------------------------------------

/**
@publishedPartner
@released
*/
EXPORT_C CImConvertHeader* CImConvertHeader::NewL(CImConvertCharconv& aConverter)
	{
	CImConvertHeader* self = new (ELeave) CImConvertHeader (aConverter);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CImConvertHeader::CImConvertHeader(CImConvertCharconv& aConverter) : 
	iCharConv (aConverter)
	{
	}

CImConvertHeader::~CImConvertHeader() 
	{
	iHeader = NULL;
	}

void CImConvertHeader::ConstructL() 
	{
	iQPCodec.AddEncodeChar(KImcvEncodeCharacterList);

	// Default setting.
	iCharConv.PrepareToConvertToFromOurCharsetL(iCharConv.DefaultCharset());
	}

EXPORT_C void CImConvertHeader::SetMessageType(TBool aIsMIME)
	{
	isMIMEMessageHeader=aIsMIME;
	}

EXPORT_C void CImConvertHeader::SetOverrideCharset(TUint aCharset)
/**
Sets the override character set to use when decoding the 8 bit data. Zero
indicates the charset not to be used.

@param aCharset
The new character set to use as an overriding character set.
*/
	{
	iOverrideCharset = aCharset;
	}

EXPORT_C TUint CImConvertHeader::OverrideCharset() const
/**
Retrieves the override character set that will be used when decoding the 8 bit
data. A character set value of zero indicates no overriding character set will
be used.
*/
	{
	return iOverrideCharset;
	}

//------------------------------------------------------------------------------

EXPORT_C void CImConvertHeader::DecodeAllHeaderFieldsL(CImHeader& rHeader)
	{
	iHeader = &rHeader;

	iHeader->SaveEncodedHeadersL();
	
	// If Header field does not have a Mime info, but header has encoded words, then 
	// we need to parse it accordingly. 
	TBool nonMime = EFalse; 
	if (!isMIMEMessageHeader) 
		{ 
		isMIMEMessageHeader = ETrue; 
		nonMime = ETrue; 
		}

	// FROM:
	iEncodingInfo.SetField(TImHeaderEncodingInfo::EFrom);
	DecodeFieldL( rHeader.From());

	// SUBJECT:
	iEncodingInfo.SetField(TImHeaderEncodingInfo::ESubject);
	DecodeFieldL( rHeader.Subject());

	// REPLYTO:
	iEncodingInfo.SetField(TImHeaderEncodingInfo::EReplyTo);
	DecodeFieldL( rHeader.ReplyTo());

	// TO:
	iEncodingInfo.SetField(TImHeaderEncodingInfo::ETo);
	DecodeRecipientListL(rHeader.ToRecipients());

	// CC:
	iEncodingInfo.SetField(TImHeaderEncodingInfo::ECc);
	DecodeRecipientListL(rHeader.CcRecipients());

	// BCC:
	iEncodingInfo.SetField(TImHeaderEncodingInfo::EBcc);
	DecodeRecipientListL(rHeader.BccRecipients());

	if(nonMime) 
		{ 
		isMIMEMessageHeader = EFalse; 
		} 	
		
	iHeader = NULL; // iHeader is not used outside non-exported function DecodeFieldL
	}


void CImConvertHeader::DecodeFieldL(const TDesC& aField)
	{
	HBufC* buf = HBufC::NewLC(aField.Size());
	TPtr ptr(buf->Des());
	DecodeHeaderFieldL(aField, ptr);

	switch (iEncodingInfo.Field())
		{
	case TImHeaderEncodingInfo::EFrom:
		iHeader->SetFromL(*buf);
		break;
	case TImHeaderEncodingInfo::ESubject:
		iHeader->SetSubjectL(*buf);
		break;
	case TImHeaderEncodingInfo::EReplyTo:
		iHeader->SetReplyToL(*buf);
		break;
	default:
		__ASSERT_DEBUG(1, User::Invariant());
		}
	CleanupStack::PopAndDestroy( ); // buf
	}


void CImConvertHeader::DecodeRecipientListL(CDesCArray& aArray)
	{
	HBufC* buf = NULL; 
	TInt i = aArray.Count();
	while (i--)
		{
		iEncodingInfo.SetArrayValue(i);

		if (!(aArray[i]).Length())
			continue; // Should really delete them some how

		buf = HBufC::NewLC(aArray[i].Length());
		TPtr ptr(buf->Des());

		DecodeHeaderFieldL(aArray[i], ptr);
		aArray.Delete(i);
		aArray.InsertL(i, *buf);

		CleanupStack::PopAndDestroy( ); // buf
		}
	}

/**
Search for one or more encoded words in aLine

@publishedPartner
@released
*/

 EXPORT_C void CImConvertHeader::DecodeHeaderFieldL(const TDesC16& aBufIn, TDes& aBufOut)

	{
	// input should be 8bit, possible loss of data otherwise.
	HBufC8* buffer8 = HBufC8::NewLC( aBufIn.Length() );
	buffer8->Des().Copy( aBufIn );

	if (isMIMEMessageHeader)
    	
    	DecodeHeaderFieldL( *buffer8, aBufOut);

	else
		 DecodeNonMIMEHeaderFieldL( *buffer8, aBufOut);



	CleanupStack::PopAndDestroy( ); // buffer8
	}

/**
Search for one or more encoded words in aLine

@publishedPartner
@released
*/
 EXPORT_C void CImConvertHeader::DecodeHeaderFieldL(const TDesC8& aBufIn, TDes& aBufOut)
	{
	__ASSERT_DEBUG(aBufIn.Size() <= aBufOut.MaxSize(), User::Invariant());
	aBufOut.SetLength(0);

	TBool wordExists = ETrue;
	TBool decoded = EFalse;
	TInt rem = 0;
	TPtrC8 encodedWord8(aBufIn);
	TChar QMark = KImcvQuestionMark;
	TInt spaces = 0;
	TLex8 lex(aBufIn);

	while (wordExists)
		{
		wordExists = EFalse;

		lex.SkipSpaceAndMark();
		spaces = lex.TokenLength();

		// Get next Encoded word
		while (lex.Peek() != KImcvEquals && !lex.Eos())
			{
			lex.Inc();
			}

		if (lex.Eos())
			{
			break;	
			}

		lex.Inc();
		if (lex.Peek() == QMark)
			{
			// We have the start of an encoded word..
			// Copy over all data marked prior to word
			lex.Inc(-1);

			if (spaces==lex.TokenLength())
				{
				// contiguous encoded-words, spaces removed in decoded word.
				iEncodingInfo.SetAddSpace(ETrue);
				}
			else
				{
				do
					{
					lex.Inc(-1);
					}	
				while (lex.Peek()==KImcvSpaceChar || lex.Peek()==KImcvTab);
				lex.Inc(); 	
				if (!lex.Eos() && 
					(lex.Peek()==KImcvSpaceChar || lex.Peek()==KImcvTab) ) // Get back to space
					{
					lex.Inc();	
					}
				TPtrC8 marked = lex.MarkedToken();
				Append(aBufOut, marked);

				}

			lex.SkipSpaceAndMark(); // Start of encoded word

			//--Need to check two question marks within encoded word --
			lex.Inc(2);
			while ( lex.Peek() != QMark && !lex.Eos() )
				{
				lex.Inc();
				}
			
			if (lex.Peek() != QMark)
				{
				break; // incorrectly formed encoded-word
				}
			
			if (!lex.Eos())
				{
				lex.Inc();
				}
			else
				{
				break;	
				}
				
			if (!lex.Eos())
				{
				lex.Inc();
				}				
			else
				{
				break;
				}
				
			if (!lex.Eos() && lex.Peek() != QMark)
				{
				break; // incorrectly formed encoded-word
				}
				
			lex.Inc();
			
			while ( lex.Peek() != QMark && !lex.Eos() )
				{
				lex.Inc();
				}
			
			if (lex.Peek() != QMark)
				{
				break; // incorrectly formed encoded-word
				}
			
			if (!lex.Eos())
				{
				lex.Inc();
				}
			else
				{
				break;	
				}
			
			if (lex.Peek() != KImcvEquals)
				{
				break; // incorrectly formed encoded-word
				}
				
			if (!lex.Eos())
				{
				lex.Inc();
				}
			else
				{
				break;	
				}
			//---------------------------------------------------------

			encodedWord8.Set(lex.MarkedToken());

			wordExists = ETrue;
			}

		if (wordExists)
			{
			// If found ...
			decoded=ETrue;
			HBufC* decodedWord = HBufC::NewLC(encodedWord8.Length());

			TPtr decodedPtr(decodedWord->Des());
 			if ( DecodeWordL(encodedWord8, decodedPtr, rem) )
				{
				// replace decoded string with new string
				TUint offset = aBufOut.Length();


				// Store encoding information in Header object.
				iEncodingInfo.SetEncodedLength(lex.TokenLength());
				iEncodingInfo.SetOffset(offset);
				iEncodingInfo.SetLength(decodedPtr.Length());

				if (iHeader)
					{
					iHeader->AddEncodingInfoL(iEncodingInfo);			
					}
					
				}

			aBufOut.Append( decodedPtr );

			CleanupStack::PopAndDestroy(); //  decodedWord;
			}	
		} // while

	lex.UnGetToMark();
	if (decoded && lex.MarkedOffset())
		{
		lex.UnGet();
		if (lex.Peek()==KImcvSpaceChar || lex.Peek()==KImcvTab)
			{
			lex.Mark();
			}			
		}

	if(decoded==EFalse)
		{
		DecodeNonMIMEHeaderFieldL(aBufIn,aBufOut);

		}
 	else
		{
		TPtrC8 marked = lex.RemainderFromMark();
  	    Append(aBufOut, marked);

 		}
}


// For non-MIME case, System default being ASCII normally 
//
 EXPORT_C void CImConvertHeader::DecodeNonMIMEHeaderFieldL(const TDesC8& aBufIn, TDes& aBufOut)

	{
	__ASSERT_DEBUG(aBufIn.Size() <= aBufOut.MaxSize(), User::Invariant());

	TUint charset;

	(iOverrideCharset != 0) ? 
		(charset = iOverrideCharset) : (charset = iCharConv.DefaultCharset());

	if (iCharConv.PrepareToConvertToFromOurCharsetL(charset)== EFalse)
		{
		// Charset not available, don't decode.
		aBufOut.Copy(aBufIn);
		}

	// Character set conversion
	TInt unconvertedChars;
	TInt pos;
	iCharConv.ConvertToOurCharsetL(aBufIn, aBufOut, unconvertedChars, pos);

}



// aBufIn is of the form "=?" "charset" "?" "Encoding" "?" "Encoding Text" "?="
// Decode the 'Encoding Text' based on the information in 'Encoding' and 'charset'
// and pass back in aBufOut

//
  TBool CImConvertHeader::DecodeWordL(const TDesC8& aBufIn, TDes& aBufOut,  TInt rRemainder)
	{
	rRemainder=0;
      HBufC8* dataBuf8 = HBufC8::NewLC(aBufOut.MaxLength());


	const TUint8* ptr = aBufIn.Ptr();	

	__ASSERT_DEBUG( (*ptr == KImcvEquals) && (*(ptr+1) == KImcvQuestionMark),User::Invariant());

	// Extract the charset string and store
	TInt i;
	for(i = 2; *(ptr+i) != KImcvQuestionMark ; i++)
		;

	TUint uid;
	if (iOverrideCharset != 0)
		{
		uid = iOverrideCharset;
		}
	else
		{
		TPtrC8 buf8(ptr+2, i-2);
		uid = iCharConv.GetMimeCharsetUidL(buf8);
		}

	if (!uid || !iCharConv.PrepareToConvertToFromOurCharsetL(uid))
		{
		// Charset not available, dont decode.
        aBufOut.Copy(aBufIn);
 	  	CleanupStack::PopAndDestroy(dataBuf8); 
		return EFalse;
		}

	iEncodingInfo.SetCharsetUid(uid);

	// Extract the Encoding string and store
	for(i++; *(ptr+i) != KImcvQuestionMark ; i++)
		;
	TPtrC8 buf28(ptr+i-1, 1);
	iEncodingInfo.SetEncodingType(buf28); 	// Currently encoding is either Q or B


	// Now at the encoding text
	TInt j;
	for(j = i+1; *(ptr+j) != KImcvQuestionMark ; j++)
		;

	// Intermediate buffer for decoding, prior to converting

    HBufC8* decodeBuf8 = HBufC8::NewLC(aBufOut.MaxLength());

	decodeBuf8->Des().Copy( aBufIn.Mid(i+1, j -i-1));

   	// Depending on type of encoding Q or B, decode.

	TPtr8 dataPtr8(dataBuf8->Des());
	TPtr8 decodePtr8(decodeBuf8->Des());
	switch (iEncodingInfo.EncodingType())
		{
		case TImHeaderEncodingInfo::EBase64 :
			iB64Codec.Decode( *decodeBuf8, dataPtr8 );
			break;
		case TImHeaderEncodingInfo::EQP :
			// Replace all underscores in text with SPACE.
			i=decodePtr8.Length();
			while (i--)
				{
				if (decodePtr8[i] == KImcvUnderScore)
				    decodePtr8[i] = KImcvSpaceChar;
				}
            iQPCodec.Decode( *decodeBuf8, dataPtr8 );
			break;
		default:
			dataBuf8->Des().Copy(*decodeBuf8); 
			break;
		}

	// Character set conversion
	TInt unconvertedChars, pos;
	rRemainder = iCharConv.ConvertToOurCharsetL(*dataBuf8, aBufOut, unconvertedChars, pos);

	CleanupStack::PopAndDestroy(decodeBuf8);
	CleanupStack::PopAndDestroy(dataBuf8); 
	return (rRemainder < 0) ? EFalse:ETrue;
	}
/**
Convert function used for non-MIME messages.

@publishedPartner
@released
*/
EXPORT_C void CImConvertHeader::ConvertHeaderFieldL(const TDesC16& aBufIn, RBuf8& aBufOut, TBool aIsAddressField)
	{
	// Don't make use of the unconverted char info. 
	//Assume text has been checked for non ascii characters. 

	// Need to identify the email address
	TInt start=0;
	TInt end=0;

	if (ExtractTextToEncode(aBufIn, start, end, aIsAddressField)==EFalse)
	    aBufOut.Copy(aBufIn);
	else
		{
		// clear out buffer
		aBufOut.Zero();
		TInt unconvertedChars;
		TInt pos;
		iCharConv.ConvertFromOurCharsetL( aBufIn.Mid(start, end-start+1), aBufOut, unconvertedChars, pos);
  
		if (start>0)
			{
			// Copy over text prior to bit needing encoding
			aBufOut.ReAllocL(aBufOut.Length() + aBufIn.Length());
			Insert( aBufOut, aBufIn.Left(start));

			}
		
		if (end<aBufIn.Length())
			{
			// Copy over text after bit needing encoding
		 
            aBufOut.ReAllocL(aBufOut.Length() + (aBufIn.Length()-end-1));
			aBufOut.Append(aBufIn.Right(aBufIn.Length()-end-1));

			}
		}
	}


/**
@publishedPartner
@released
*/
EXPORT_C void CImConvertHeader::EncodeHeaderFieldL(const TDesC& aBufIn, RBuf8& aBufOut,				
                CArrayFix<TImHeaderEncodingInfo>* aInfoArray, TInt aState, TInt aArrayVal)
	{
	// In case not requiring encoding, already ascii
   // allocate enough space to copy data
	aBufOut.ReAllocL(aBufIn.Length());
	aBufOut.Copy(aBufIn);

	for (TInt i=0; i < aInfoArray->Count(); i++)
		{
		if ( (aInfoArray->At(i).Field() == (TImHeaderEncodingInfo::TFieldList) aState)
								&& ((*aInfoArray)[i].ArrayValue() == (TInt)aArrayVal) )
			{
			GetCharsetAndEncodeDataL( aBufIn, aBufOut, (*aInfoArray)[i]);
			}
		}
	}

/**
 This function has to encode without any prestored information.
 Encodes from UNICODE, to UTF-7

@publishedPartner
@released
*/
EXPORT_C void CImConvertHeader::EncodeHeaderFieldL(const TDesC& aBufIn, RBuf8&  aBufOut,
				                                   const TUint aCharset, const TImHeaderEncodingInfo::TEncodingType aType, TBool aIsAddressField)
	{
	TImHeaderEncodingInfo infoArray;

	aBufOut.ReAllocL(aBufIn.Length());
	aBufOut.Copy(aBufIn);

	infoArray.SetCharsetUid(aCharset);
	infoArray.SetEncodingType(aType);
	
	TInt startOfEncoding = 0;
	TInt endOfEncoding = 0;
	if(ExtractTextToEncode(aBufIn, startOfEncoding, endOfEncoding, aIsAddressField))
		{
		infoArray.SetOffset(startOfEncoding);
		infoArray.SetLength(endOfEncoding - startOfEncoding + 1);
		GetCharsetAndEncodeDataL(aBufIn, aBufOut, infoArray);	
		}
	}

// Return the offset in the text buffer for the text portion of the header field
//
TBool CImConvertHeader::ExtractTextToEncode(const TDesC& aBufIn, TInt& rStart, TInt& rEnd, TBool aIsAddressField)
	{
	TInt startOfEncoding = 0;
	TInt endOfEncoding = 0;
	TBool mustEncode = EFalse;
	TBool mustEncodeThisWord=EFalse;

	// Check aField for non ascii characters - encode. Return in rEncodedField.

	TInt len=aBufIn.Length();
	TInt i=0;
	for (;i < len;i++)
		{
		TChar inChar = aBufIn[i];
		if (inChar==KImcvSpaceChar || inChar==KImcvQuote || inChar==KImcvTab)
			{
			if (mustEncodeThisWord)
				endOfEncoding=i-1;
			else if (!mustEncode)
				startOfEncoding=i+1; 
			mustEncodeThisWord=EFalse;
			}
		else if (inChar==KImcvLeftChevron && aIsAddressField)
			{
			if (mustEncodeThisWord)
				{
				endOfEncoding=i-1;
				mustEncodeThisWord=EFalse;
				}
			if (mustEncode)
				break; // already have word to encode, now at an address
			}
		else if (inChar==KImcvRightChevron && aIsAddressField)
			{
			startOfEncoding=i+1; 
			}
		else if ( !IsAscii(inChar) )
			{
			mustEncode = ETrue;
			mustEncodeThisWord=ETrue;
			}
		}

	if (mustEncodeThisWord)
		endOfEncoding=i-1;

	rStart=startOfEncoding;
	rEnd=endOfEncoding;
	return mustEncode;
	}
		                                               
void CImConvertHeader::GetCharsetAndEncodeDataL(const TDesC&  aBufIn,
									RBuf8& aBufOut, TImHeaderEncodingInfo& aInfo)
	{
	// Work out which character set to use, and get a string representation
	// of it.
	HBufC8* charsetName=iCharConv.GetMimeCharsetTextStringL(aInfo.CharsetUid());

	TUint charset=aInfo.CharsetUid();
	if (charset==KCharacterSetIdentifierIso88591)
		charset=KCharacterSetIdentifierCodePage1252;
	
	if (!charsetName || !iCharConv.PrepareToConvertToFromOurCharsetL(charset))
		{
		delete charsetName; 
		charsetName=NULL;

		// Problem with charset Uid, use default character set.
		charset=iCharConv.DefaultCharset();
		aInfo.SetCharsetUid(charset);
		charsetName=iCharConv.GetMimeCharsetTextStringL(charset);
		if (charset==KCharacterSetIdentifierIso88591)
			charset=KCharacterSetIdentifierCodePage1252;
		__ASSERT_ALWAYS(iCharConv.PrepareToConvertToFromOurCharsetL(charset), User::Invariant());
		}

	CleanupStack::PushL(charsetName);
	__ASSERT_ALWAYS(charsetName, gPanic(KPanicInvalidDefaultCharacterSet) );

	// Create a buffer to use to store the encoded word
	HBufC8* encodedWordBuffer = HBufC8::NewL(aBufOut.MaxSize());
	RBuf8 encodedWord(encodedWordBuffer);
	encodedWord.CleanupClosePushL();

	EncodeWordL(aBufIn, aBufOut, aInfo, *charsetName, encodedWord);

	CleanupStack::PopAndDestroy(2, charsetName);  // charset name, encoded word
	}


void CImConvertHeader::EncodeWordL(const TDesC& aBufIn, 
								   RBuf8& aBufOut, TImHeaderEncodingInfo& aInfo,
								   const TDesC8& aCharsetName, RBuf8& aEncodedWord)
	{
	TInt max = KEncodedWordMaxLength - KMaxEncodedInformationLength -
		aCharsetName.Length() - KMaxHeaderFieldNameLength;

	TBool splitAndRetry = ETrue;

	// We will convert the word to the required character set, and perform any
	// encoding (base64, quoted-printable) required. At each stage we check whether
	// the buffer has exceeded the maximum length allowed for a line. If so, we
	// cut the buffer into two halves and encode each half separately.

	if (aInfo.Length() <= max)
		{
		// Identify the part of the input buffer that will be encoded
		TPtrC data = aBufIn.Mid(aInfo.Offset(), aInfo.Length());

		// Perform the conversion to the required character set
		DoCharsetConversionL(data, aEncodedWord);

		if (aEncodedWord.Length() <= max)
			{
			// Perform any base64 / quoted printable encoding
			DoEncodingL(aEncodedWord, aInfo);

			if (aEncodedWord.Length() <= max)
				{
				// We have managed to do all the steps without the buffer exceeding
				// the maximum line length
	  			splitAndRetry = EFalse;
				}
			}
		}

	if (splitAndRetry)
		{
		TInt len = aInfo.Length()/2;
		TUint16 offset = (TUint16) aInfo.Offset();
		TUint16 length = (TUint16) aInfo.Length();

		aInfo.SetOffset(offset+len);
		aInfo.SetLength(length-len);
		EncodeWordL(aBufIn, aBufOut, aInfo, aCharsetName, aEncodedWord);
		aBufOut.ReAllocL(aBufOut.MaxLength()+1);
		aBufOut.Insert(offset+len,KImcvSpace);

		aInfo.SetOffset(offset);
		aInfo.SetLength(len);
		EncodeWordL(aBufIn, aBufOut, aInfo, aCharsetName, aEncodedWord);

		aInfo.SetLength(length);
		}
	else
		{
		// Delete unencoded data, prior to putting in encoded data
		aBufOut.Delete(aInfo.Offset(), aInfo.Length());
		aBufOut.ReAllocL(aBufOut.MaxLength()-aInfo.Length());
		AddEncodedWordInfoL(aEncodedWord, aBufOut, aInfo, aCharsetName);
		aEncodedWord.Zero();
		}
	}


TBool CImConvertHeader::DoCharsetConversionL(const TDesC& aDataToConvert, RBuf8& aEncodedWord)
	{
	TBool convertOk = ETrue;

	TInt unconvertedChars, pos;
	TInt excessChars;
	TInt lastExcessChars = 0;
	TBool failNextTimeExcessCharsStaySame = EFalse;
	TBool finishedConvert = EFalse;

	while (!finishedConvert)
		{
		excessChars = iCharConv.ConvertFromOurCharsetL(aDataToConvert, aEncodedWord, unconvertedChars, pos);

		if (excessChars == 0)
			{
			finishedConvert = ETrue;
			}
		else if (excessChars == CCnvCharacterSetConverter::EErrorIllFormedInput)
			{
			User::Leave(KErrCorrupt);
			}
		else
			{
			// We did not convert all the characters in the buffer. This is
			// probably because the output buffer is not large enough.
			// If the number of characters left unconverted this time is the same
			// as last time, then perhaps the conversion is failing for other
			// reasons. To avoid getting stuck like this forever, we will only
			// allow this to happen once.

			if (excessChars == lastExcessChars)
				{
				if (failNextTimeExcessCharsStaySame)
					{
					finishedConvert = ETrue;
					convertOk = EFalse;
					}
				else
					{
					failNextTimeExcessCharsStaySame = ETrue;
					}
				}
			else
				{
					failNextTimeExcessCharsStaySame = EFalse;
				}

			// If we are going around again, increase the size of the output buffer
			if (!finishedConvert)
				{
				aEncodedWord.ReAllocL(aEncodedWord.MaxSize() + KImMaxLengthForEncodedChar);
				lastExcessChars = excessChars;
				}
			}
		}

	return convertOk;
	}

void CImConvertHeader::DoEncodingL(RBuf8& aEncodedWord, TImHeaderEncodingInfo& aInfo)
	{
	// Take a copy of the encoded word so far. This will be used as the
	// input to the encoding routines
	HBufC8* dataToEncode = HBufC8::NewLC(aEncodedWord.Length());
	dataToEncode->Des().Copy(aEncodedWord);
	aEncodedWord.Zero();

	if (aInfo.EncodingType() == TImHeaderEncodingInfo::EBase64)
		{
		// Increase the size of the buffer to allow for the base64 encoding.
		// Base64 takes three 8 bit octets and encodes them into four 6 bit
		// characters.
		aEncodedWord.ReAllocL((((*dataToEncode).Length() + 2) / 3) * 4);

		iB64Codec.Encode(*dataToEncode, aEncodedWord);
		}
	else if (aInfo.EncodingType() == TImHeaderEncodingInfo::EQP)
		{
		// Worst case scenario is that each character in the buffer has to be
		// encoded to its hex equivalent with preceding '=' sign. Also added an
		// extra byte becuase the QP encoder expects the buffer to be one longer
		// than required.
		aEncodedWord.ReAllocL(((*dataToEncode).Length() * 3) + 1);

		// Replace all underscores in text with SPACE.
		for(TInt i=0; i < (*dataToEncode).Length(); i++)
			{
			if ((*dataToEncode)[i] == KImcvSpaceChar)
				dataToEncode->Des()[i] = KImcvUnderScore;	
			}
		 iQPCodec.Encode(*dataToEncode, aEncodedWord);
		}
	else
		aEncodedWord.Copy(*dataToEncode);

	CleanupStack::PopAndDestroy(); // dataToEncode
	}


TInt CImConvertHeader::AddEncodedWordInfoL(const TDesC8& aEncodedWord, RBuf8& aBufOut,
									TImHeaderEncodingInfo& aInfo, const TDesC8& aCharsetName)
	{
	// Encoded data 'in place' in OutBuf.

	// Add BEFORE encode data
	TInt offset = aInfo.Offset();
	TInt charsetSize = aCharsetName.Size();
		
	// Convert based on iCharSet and iEncoding
	
	// allocate enough space for encoded word plus extra encoding tokens
	// KImMaxLengthForEncodedWordAndExtraEncodingTokens = size of '?=' + '?' + ('B' or 'Q') + '?' + '?=' + ' '
	aBufOut.ReAllocL(aBufOut.MaxSize() + KImMaxLengthForEncodedWordAndExtraEncodingTokens + charsetSize + aEncodedWord.Size());
	aBufOut.Insert( offset, KImcvEncodedWordStart);

	// KImcvEncodedWordStart = 2
	// Other numerical values are from the encoded word characters
	// =? .. ?.. ? .. ?=

	aBufOut.Insert( offset + 2, aCharsetName);
	aBufOut.Insert( offset + 2 + charsetSize, KImcvQuestionMarkString);


	if (aInfo.EncodingType() == TImHeaderEncodingInfo::EBase64)
    	{
		aBufOut.Insert( offset + 2 + charsetSize+1, KImcvB);
		}
	else
		{	
		aBufOut.Insert( offset + 2 + charsetSize+1, KImcvQ);
		}

	aBufOut.Insert( offset + 2 + charsetSize+2, KImcvQuestionMarkString);

	offset += 2 + charsetSize + 3;

	aBufOut.Insert( offset, aEncodedWord );

	aBufOut.Insert( offset + aEncodedWord.Length(), KImcvEncodedWordEnd);

	TInt returnOffset = offset + aEncodedWord.Length() + 2;

	if (aInfo.AddSpace())
		{
    	aBufOut.Insert( aInfo.Offset(), KImcvSpace);

		returnOffset++;
		}

	// Return end of encoding offset.
	return returnOffset;
	}



// Check aData to see if it has any partial 'encoded-word's.
// ie a string of the gotm '=? ... ? ... ?=' which is incomplete.
// Return length of new string, up to the start of the partial encoded word.
//
EXPORT_C TBool CImConvertHeader::FindEncodedWord(TPtrC8& aData, TInt& aInit, TInt& rStart, TInt& rEnd)
	{
	TBool found = EFalse;

	TPtrC8 data = aData.Mid(aInit);
	rStart = data.Find(KImcvEncodedWordStart);
	rEnd = data.Length();
	if (rStart != KErrNotFound)
		{
		found = ETrue;

		TInt i;
		TInt j = 0;

		// Check for the three question marks
		for (i = rStart+2; i < data.Length(); i++)
			{
			if (data[i] == KImcvQuestionMark)	
				j++;
			if (j == 3)
				break; 
			}

		if (j != 3)
			found = EFalse;	// Have encountered an incomplete encoded-word

		// Check last ? is followed by =
		rEnd = i+1;
		if ( rEnd > data.Length() || data[rEnd] != KImcvEquals )
			found = EFalse;	// Have encountered an incomplete encoded-word
		} // if

 	return found;
	}


TBool CImConvertHeader::IsAscii( TUint aChar ) const
	{
	return ( ((aChar >= 32) && (aChar <= 126)));
	};

// function for adding 8 bit descriptor onto a 16bit descritpr.
//
void CImConvertHeader::Append(TDes& aBuffer, const TDesC8& aAddition)
//----------------------------------------------------------------------------------------
	{
	TInt addLen = aAddition.Length();
	TInt bufLen = aBuffer.Length();

	aBuffer.SetLength(bufLen+addLen);
	for(TInt i = 0; i < addLen; i++)
		aBuffer[bufLen+i] = aAddition[i];
	}


void CImConvertHeader::Insert(TDes8& aBuffer, const TDesC16& aInsert)
	{
	TInt addLen = aInsert.Length();
	TInt bufLen = aBuffer.Length();
	TInt origAddLen=addLen;

	aBuffer.SetLength(bufLen+addLen);


	while (bufLen > addLen)
		{
		aBuffer[bufLen+addLen-1] = aBuffer[bufLen-1];
		bufLen--;
		}

	while (addLen > bufLen)
		{
		aBuffer[addLen-1] = (TUint8) aInsert[addLen-1];
		addLen--;
		}

	// Assert bufLen==addLen
	while(bufLen)
		{
		aBuffer[bufLen+origAddLen-1] = aBuffer[bufLen-1];
		aBuffer[bufLen-1] = (TUint8) aInsert[bufLen-1];
		bufLen--;
		}


	}



