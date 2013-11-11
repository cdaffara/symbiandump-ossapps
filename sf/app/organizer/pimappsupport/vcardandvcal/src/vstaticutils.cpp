// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <vstaticutils.h>

// System includes
#include <f32file.h>
#include <charconv.h>
#include <utf.h>
#include <s32mem.h>
#include <concnf.h>
#include <confndr.h>
#include <conlist.h>

// User includes
#include <vutil.h>
#include <vuid.h>
#include "verror.h"
#include "VersitAdditionalStorage.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "versit_internal.h"
#include "versittlscontainer.h"
#endif

// Constants
_LIT(KVersitEscapeCharacter, "\\");
const TText KVersitSemiColon = ';';


EXPORT_C TBool VersitUtils::EightBitEncoding(Versit::TVersitCharSet aCharSet)
/** Tests whether the specified character set is encoded using 8 bits or not. 
For instance ASCII is 7-bit; ISO-8859-1 is 8-bit. 

@param aCharSet A character set.
@return ETrue if the character set uses 8 bits; EFalse if it uses 7 bits. */
	{
	switch(aCharSet)
		{
	case Versit::EUSAsciiCharSet:
	case Versit::EUTF7CharSet:
		return EFalse;
	//
	case Versit::EUTF8CharSet:
	case Versit::EISO88591CharSet:
	case Versit::EISO88592CharSet:
	case Versit::EISO88594CharSet:
	case Versit::EISO88595CharSet:
	case Versit::EISO88597CharSet:
	case Versit::EISO88599CharSet:
	case Versit::EISO88593CharSet:
	case Versit::EISO885910CharSet:
	case Versit::EGB231280CharSet:
	case Versit::EGBKCharSet:
	case Versit::EBIG5CharSet:
	case Versit::EISO2022JPCharSet:
	case Versit::EEUCJPCharSet:
	case Versit::EShiftJISCharSet:
	case Versit::EJISCharSet:
		return ETrue;
	default:
		break;
		}

	__ASSERT_DEBUG(EFalse, User::Invariant());
	return EFalse;
	}

EXPORT_C TBool VersitUtils::EightBitEncoding(TUint aCharSetId)
/** Tests whether the character set identified by the specified ID is encoded using 8 
bits or not. 

@param aCharSetId A character set ID.
@return ETrue if the character set uses 8 bits; EFalse if it uses 7 bits. */
	{
	return (aCharSetId!=KCharacterSetIdentifierUtf7 && aCharSetId!=KCharacterSetIdentifierAscii);
	}

EXPORT_C TUid VersitUtils::ConArcEncodingUid(Versit::TVersitEncoding aEncoding)
/** Returns the UID of a character set converter suitable for the specified encoding. 

@param aEncoding An encoding type.
@return The UID of a character set converter. 
@see CCnaConverterList::NewConverterL() */
	{
	switch(aEncoding)
		{
	case Versit::EQuotedPrintableEncoding:
		return TUid::Uid(KTextToQuotedPrintableConverter);
	case Versit::EBase64Encoding:
		return TUid::Uid(KTextToBase64Converter);
	default:
	case Versit::ENoEncoding:
	case Versit::EEightBitEncoding:
		__ASSERT_DEBUG(EFalse, User::Invariant());
		return KNullUid;
		}
	}

EXPORT_C const TDesC8& VersitUtils::IANACharacterSetName(Versit::TVersitCharSet aCharSet)
/** Returns the IANA character set name corresponding to the specified character set identifier.

@param aCharSet A character set.
@return An IANA character set name. */
	{
	switch(aCharSet)
		{
	case Versit::EUSAsciiCharSet:
		return KNullDesC8; // Should this do something else?
	//
	case Versit::EUTF7CharSet:
		return KVersitTokenUTF7;
	case Versit::EUTF8CharSet:
		return KVersitTokenUTF8;
	case Versit::EISO88591CharSet:
		return KVersitTokenISO1;
	case Versit::EISO88592CharSet:
		return KVersitTokenISO2;
	case Versit::EISO88594CharSet:
		return KVersitTokenISO4;
	case Versit::EISO88595CharSet:
		return KVersitTokenISO5;
	case Versit::EISO88597CharSet:
		return KVersitTokenISO7;
	case Versit::EISO88599CharSet:
		return KVersitTokenISO9;
	case Versit::EISO88593CharSet:
		return KVersitTokenISO3;
	case Versit::EISO885910CharSet:
		return KVersitTokenISO10;
	case Versit::EGB231280CharSet:
		return KVersitTokenGB2312;
	case Versit::EGBKCharSet:
		return KVersitTokenGBK;
	case Versit::EBIG5CharSet:
		return KVersitTokenBIG5;
	case Versit::EISO2022JPCharSet:
		return KVersitTokenISO2022JP;
	case Versit::EEUCJPCharSet:
		return KVersitTokenEUCJP;
	case Versit::EShiftJISCharSet:
		return KVersitTokenShiftJIS;
	case Versit::EJISCharSet:
		return KVersitTokenJIS;
	default:
		break;
		}

	__ASSERT_DEBUG(EFalse, User::Invariant());
	return KNullDesC8;
	}

EXPORT_C const TDesC8& VersitUtils::IANAEncodingName(Versit::TVersitEncoding aEncoding)
/** Returns the MIME encoding type corresponding to the specified encoding.

@param aEncoding An encoding type.
@return The MIME encoding type corresponding to the specified encoding. */
	{
	switch(aEncoding)
		{
	case Versit::EQuotedPrintableEncoding:
		return KVersitTokenQUOTEDPRINTABLE;
	case Versit::EBase64Encoding:
		return KVersitTokenBASE64;
	case Versit::EEightBitEncoding:
		return KVersitToken8BIT;
	default:
	case Versit::ENoEncoding:
		break;
		}

	__ASSERT_DEBUG(EFalse, User::Invariant());
	return KNullDesC8;
	}

EXPORT_C TUid VersitUtils::CharConvCharSetUid(Versit::TVersitCharSet aVersitSet)
/** Returns the character converter UID associated with the specified Versit character 
set identifier. 

@param aVersitSet A character set identifier.
@return The character converter UID associated with the specified Versit character set 
identifier. These UIDs are defined in charconv.h. */
	{
	switch(aVersitSet)
		{
	// ASCII
	case Versit::EUSAsciiCharSet:
		return TUid::Uid(KCharacterSetIdentifierAscii);

	// ISO's
	case Versit::EISO88591CharSet:
		return TUid::Uid(KCharacterSetIdentifierIso88591);
	case Versit::EISO88592CharSet:
		return TUid::Uid(KCharacterSetIdentifierIso88592);
	case Versit::EISO88594CharSet:
		return TUid::Uid(KCharacterSetIdentifierIso88594);
	case Versit::EISO88595CharSet:
		return TUid::Uid(KCharacterSetIdentifierIso88595);
	case Versit::EISO88597CharSet:
		return TUid::Uid(KCharacterSetIdentifierIso88597);
	case Versit::EISO88599CharSet:
		return TUid::Uid(KCharacterSetIdentifierIso88599);
	case Versit::EISO88593CharSet:
		return TUid::Uid(KCharacterSetIdentifierIso88593);
	case Versit::EISO885910CharSet:
		return TUid::Uid(KCharacterSetIdentifierIso885910);
	case Versit::EGB231280CharSet:
		return TUid::Uid(KCharacterSetIdentifierGb2312);
	case Versit::EGBKCharSet:
		return TUid::Uid(KCharacterSetIdentifierGbk);
	case Versit::EBIG5CharSet:
		return TUid::Uid(KCharacterSetIdentifierBig5);
	case Versit::EISO2022JPCharSet:
		return TUid::Uid(KCharacterSetIdentifierIso2022Jp);
	case Versit::EEUCJPCharSet:
		return TUid::Uid(KCharacterSetIdentifierEucJpPacked);
	case Versit::EShiftJISCharSet:
		return TUid::Uid(KCharacterSetIdentifierShiftJis);
	case Versit::EJISCharSet:
		return TUid::Uid(KCharacterSetIdentifierJis);

	// UTF
	case Versit::EUTF7CharSet:
		return TUid::Uid(KCharacterSetIdentifierUtf7);
	case Versit::EUTF8CharSet:
		return TUid::Uid(KCharacterSetIdentifierUtf8);
	default:
		return KNullUid;
		}
	}

Versit::TVersitCharSet VersitUtils::CharSet(TUint aCharConvCharSetUid)
/** Returns the Versit character set identifier associated with the specified 
character converter UID. 

@param aCharConvCharSetUid A character converter UID. These UIDs are defined in charconv.h.
@return A Versit character set identifier associated with the specified character 
converter UID. */
	{
	switch(aCharConvCharSetUid)
		{
	// ASCII
	case KCharacterSetIdentifierAscii:
		return Versit::EUSAsciiCharSet;

	// ISO's
	case KCharacterSetIdentifierIso88591:
		return Versit::EISO88591CharSet;
	case KCharacterSetIdentifierIso88592:
		return Versit::EISO88592CharSet;
	case KCharacterSetIdentifierIso88594:
		return Versit::EISO88594CharSet;
	case KCharacterSetIdentifierIso88595:
		return Versit::EISO88595CharSet;
	case KCharacterSetIdentifierIso88597:
		return Versit::EISO88597CharSet;
	case KCharacterSetIdentifierIso88599:
		return Versit::EISO88599CharSet;
	case KCharacterSetIdentifierIso88593:
		return Versit::EISO88593CharSet;
	case KCharacterSetIdentifierIso885910:
		return Versit::EISO885910CharSet;
	case KCharacterSetIdentifierGb2312:
		return Versit::EGB231280CharSet;
	case KCharacterSetIdentifierGbk:
		return Versit::EGBKCharSet;
	case KCharacterSetIdentifierBig5:
		return Versit::EBIG5CharSet;
	case KCharacterSetIdentifierIso2022Jp:
		return Versit::EISO2022JPCharSet;
	case KCharacterSetIdentifierEucJpPacked:
		return Versit::EEUCJPCharSet;
	case KCharacterSetIdentifierShiftJis:
		return Versit::EShiftJISCharSet;
	case KCharacterSetIdentifierJis:
		return Versit::EJISCharSet;

	// UTF
	case KCharacterSetIdentifierUtf7:
		return Versit::EUTF7CharSet;
	case KCharacterSetIdentifierUtf8:
		return Versit::EUTF8CharSet;
	default:
		return Versit::EUnknownCharSet;
		}
	}

EXPORT_C TBool VersitUtils::DescriptorContainsOnlySevenBitCharacters(const TDesC& aText)
/** Tests whether all of the characters in the specified text can be represented 
in 7-bit ASCII. 

@param aText Text to be tested.
@return ETrue if all of the characters in the specified text can be 
represented in 7 bit ASCII, otherwise EFalse. */
	{
	const TInt KMax7BitValue = 127;
	//
	const TInt length = aText.Length();
	const TText* pText = aText.Ptr();
	for(TInt i=0; i<length; i++)
		{
#ifdef _DEBUG
		const TPtrC pChar(aText.Mid(i, 1));
		(void) pChar;
#endif
		if	(pText[i] > KMax7BitValue)
			return EFalse;
		}

	// Can be represented in pure 7 bit ASCII
	return ETrue;
	}

EXPORT_C TBool VersitUtils::RequiresEncoding(const TDesC& aText)
/** Tests whether the specified text requires encoding.

@param aText Text to be tested.
@return ETrue if the specified text needs encoding, otherwise EFalse. */
	{
	// See http://www.freesoft.org/CIE/RFC/1521/6.htm for details on the QP tests
	// below.
	const TInt length = aText.Length();
	const TUint KVersitCharacterValueSpace = 0x20;
	const TUint KVersitCharacterValueTab = 0x09;
	const TUint KVersitCharacterValueEquals = 0x3d;
	const TUint KVersitCharacterValueExclamationPoint = 0x21;
	const TUint KVersitCharacterValueTilde = 0x7e;
	const TUint KVersitCharacterValueDoubleQuotes   = 0x22;
	const TUint KVersitCharacterValueDollarSign   = 0x24;
	const TUint KVersitCharacterValueAtSign   = 0x40;
	const TUint KVersitCharacterValueSquareBracketOpen   = 0x5B;
	const TUint KVersitCharacterValueBackSlash   = 0x5C;
	const TUint KVersitCharacterValueSquareBracketClose   = 0x5D;
	const TUint KVersitCharacterValueCircumflex   = 0x5E;
	const TUint KVersitCharacterValueSingleQuote   = 0x60;
	const TUint KVersitCharacterValueCurlyBracketOpen   = 0x7B;
	const TUint KVersitCharacterValueOrOperatorSign   = 0x7C;
	const TUint KVersitCharacterValueCurlyBracketClose   = 0x7D;

	//
	const TText* pText = aText.Ptr();
	for(TInt i=0; i<length; i++)
		{
		// Test for BASE64 and QP special characters
		//	QP also protects punctuation characters listed in 
		//  RFC1521 as being affected by EBCDIC gateways.
		const TUint character = pText[i];
		switch(character)
			{
		case KVersitCharacterValueTab:
		case KVersitCharacterValueEquals: // EQUALS SIGN
		case CVersitParser::ECarriageReturn:
		case CVersitParser::ELineFeed:
		case KVersitCharacterValueDoubleQuotes:
		case KVersitCharacterValueDollarSign:
		case KVersitCharacterValueAtSign:
		case KVersitCharacterValueSquareBracketOpen:
		case KVersitCharacterValueBackSlash:
		case KVersitCharacterValueSquareBracketClose:
		case KVersitCharacterValueCircumflex:
		case KVersitCharacterValueSingleQuote:
		case KVersitCharacterValueCurlyBracketOpen:
		case KVersitCharacterValueOrOperatorSign:
		case KVersitCharacterValueCurlyBracketClose:
			return ETrue;

		// It's okay to allow spaces in property values without
		// encoding.
		case KVersitCharacterValueSpace:
			continue;

		// Check for non-printable characters
		default:
			if	(character <= KVersitCharacterValueExclamationPoint || character >= KVersitCharacterValueTilde)
				{
				return ETrue;
				}
			}
		}

	// No encoding required
	return EFalse;
	}

EXPORT_C void VersitUtils::UncodeToNarrowL(const TDesC& aUnicode, TDes8& aNarrow, const Versit::TEncodingAndCharset& aEncodingCharset)
/** Converts Unicode text to 8-bit, using the specified character set converter. 

@param aUnicode The Unicode text to be converted.
@param aNarrow On return, the converted text.
@param aEncodingCharset Specifies the character set converter to use. */
	{
	User::LeaveIfError(CVersitParser::ConvertFromUnicodeToISOL(aNarrow, aUnicode, aEncodingCharset.iConverter));
	}

EXPORT_C void VersitUtils::AddEscapedString(TDes& aDestination,const TDesC& aTextToEscape,Versit::TVersitCharSet aCharSet)
/** Adds an escape character before the semi-colons in a given text string.

@param aDestination On return, the escaped text.
@param aTextToEscape Text to be escaped.
@param aCharSet A Versit character set identifier. */
	{
	AddEscapedString(aDestination,aTextToEscape,aCharSet==Versit::EShiftJISCharSet);
	}

EXPORT_C void VersitUtils::AddEscapedString(TDes& aDestination,const TDesC& aTextToEscape,TUint aCharSetId)
/** Adds an escape character before the semi-colons in a given text string. 

@param aDestination On return, the escaped text.
@param aTextToEscape Text to be escaped.
@param aCharSetId A character set ID. These are defined in charconv.h. */
	{
	AddEscapedString(aCharSetId==KCharacterSetIdentifierShiftJis,aDestination,aTextToEscape);
	}

void VersitUtils::AddEscapedString(TBool aIsShiftJis,TDes& aDestination,const TDesC& aTextToEscape)
	{
	const TInt length = aTextToEscape.Length();
	const TText* pText = aTextToEscape.Ptr();
	const TDesC &escapeChar = EscapeChar(aIsShiftJis);
	const TText* pEscape = escapeChar.Ptr();
	for(TInt ii=0; ii<length; pText++, ii++)
		{
		if	(*pText==KVersitSemiColon || *pText==*pEscape)
			{
			aDestination.Append(escapeChar);
			}
		aDestination.Append(*pText);
		}
	}

EXPORT_C void VersitUtils::ConArcEncodeL(RReadStream& aReadStream, CBufBase& aTarget, TUid aConArcEncodingUid)
/** Encodes a buffer using the specified character set converter. 

@param aSource Buffer to be encoded.
@param aTarget On return, the encoded buffer.
@param aConArcEncodingUid Converter UID, as returned by VersitUtils::ConArcEncodingUid(). */
	{
	__ASSERT_DEBUG(aConArcEncodingUid != KNullUid, User::Invariant());
	if	(aConArcEncodingUid != KNullUid)
		{
		CCnaConverterList* converterList = CCnaConverterList::NewLC();
		CConverterBase* converter = converterList->NewConverterL(aConArcEncodingUid);
		__ASSERT_DEBUG(converter, User::Invariant());
		if	(!converter)
			{
			CleanupStack::PopAndDestroy(converterList);
			return;
			}

		CleanupStack::PushL(converter);	

		RBufWriteStream writeStream;
		writeStream.Open(aTarget);
		converter->ConvertObjectL(aReadStream, writeStream);
		writeStream.Close();

		CleanupStack::PopAndDestroy(2, converterList);	// converter, convList
		}
	}

EXPORT_C void VersitUtils::WrapLinesL(CBufBase& aBuffer, TInt aMaxLineLength)
/** Ensures that a buffer doesn't contain any lines longer than the specified length. 

Any lines longer than the specified length are wrapped.

@param aBuffer Buffer to be wrapped.
@param aMaxLineLength The maximum line length. */
	{
	_LIT8(KVersitLineIndent, "    ");
	const TInt KLineIndentLength = KVersitLineIndent().Length();
	const TInt KCarriageReturnLineFeedLength = 2;
	const TInt length = aBuffer.Size();
	//
	TInt pos = 0;
	TInt cycles = length / aMaxLineLength;
	if	(length % aMaxLineLength != 0)
		cycles++;
	//
	for(; cycles>0; pos += (aMaxLineLength + KLineIndentLength + KCarriageReturnLineFeedLength))
		{
		aBuffer.InsertL(pos, KVersitLineIndent);
		aBuffer.InsertL(pos, KVersitTokenCRLF);
		cycles--;
		}
	}

EXPORT_C TBool VersitUtils::CheckAndIgnoreCustomErrorL(TInt aError)
/** Tests whether an error code is Versit-specific or not and leaves (with that 
error code) if it is not.

@param aError Error code to test.
@return ETrue if the error code is Versit-specific. */
	{
	switch(aError)
		{
		case KVersitErrBadDateTime:			
		case KVersitErrRepeatTypeNotHandled:	
		case KVersitErrBadRepeatValue:			
		case KVersitErrBadTimeZone:		
		case KVersitErrBadTimePeriod:			
		case KVersitErrNumberExpected:
		case KVersitErrNestedVcardExpected:
		case KVersitErrNoPropertyName:
			return ETrue;
		default:;
		}
	//Standard Epoc Error
	User::LeaveIfError(aError);
	return EFalse;
	}

EXPORT_C void VersitUtils::RemoveEscapeChars(TPtr8& aText)
/** Removes escape characters (backslashes) from the specified 8-bit string. 

@param aText A raw 8-bit string value. */
	{
	TInt dest=aText.Find(KVersitEscapedSemiColon);
	if (dest>KErrNotFound)
		{
		TInt pendingBackslash=EFalse;
		TInt length=aText.Length();
		TInt ii;
		for (ii=dest;ii<length;++ii)
			{
			if (pendingBackslash)
				{
				if (aText[ii]!=KVersitTokenSemiColonVal && aText[ii]!=KVersitTokenBackslashVal)
					aText[dest++]=KVersitTokenBackslashVal;
				pendingBackslash=EFalse;
				}
			else
				pendingBackslash=(aText[ii]==KVersitTokenBackslashVal);
			if (!pendingBackslash)
				aText[dest++]=aText[ii];
			}
		if (pendingBackslash)
			aText[dest++]=KVersitTokenBackslashVal;
		aText.SetLength(dest);
		}
	}

EXPORT_C void VersitUtils::RemoveEscapeChars(HBufC16& aText)
/** Removes escape characters (backslashes) from the specified 16-bit string. 

@param aText A raw 16-bit string value. */
	{
    TPtr16 pText(aText.Des());
    RemoveEscapeChars(pText, KCharacterSetIdentifierAscii);
	}

EXPORT_C void VersitUtils::RemoveEscapeChars(HBufC16& aText,TUint aCharSetUid)
/** Removes escape characters (backslashes or for the Shift-JIS character set, 
Yen-symbols/backslashes, Shift-JIS behaviour is platform dependent) from the 
specified 16-bit string. 

@param aText A raw 16-bit string value.
@param aCharSetUid The character set UID of the given 16-bit string; these are 
defined in charconv.h. This is needed to identify whether or not the character 
set is Shift-JIS. */
	{
    TPtr16 pText(aText.Des());
    RemoveEscapeChars(pText, aCharSetUid);
	}

EXPORT_C void VersitUtils::RemoveEscapeChars(TPtr16& aText, TUint aCharSetUid)
/** Removes escape characters (backslashes or for the Shift-JIS character set, 
Yen-symbols/backslashes, Shift-JIS behaviour is platform dependent) from the 
specified 16-bit string. 

@param aText A raw 16 bit string value.
@param aCharSetUid The character set UID of the aText parameter. This is required 
to identify the escape character. */
    {
    const TDesC &KEscapeString = EscapeChar( KCharacterSetIdentifierShiftJis == aCharSetUid );
	const TUint16 KEscapedCharacterValue = KEscapeString[0];
    //
    const TInt foundPos = aText.Find(&KEscapedCharacterValue, 1);
    const TInt length = aText.Length();
    TInt finalLength = length;
    TBool previousCharacterWasEscaped = EFalse;
    if  (length && foundPos > KErrNotFound)
        {
        TUint16* pSubFieldCharacter = const_cast<TUint16*>(&aText[0]);
        TUint16* pDestSubFieldCharacter = pSubFieldCharacter;
        //
        for(TInt j=0; j<length; j++)
            {
            if  (previousCharacterWasEscaped)
                {
                // If the next character was a semi-colon, then un-escape the
                // semi-colon and insert it into the output stream. If it wasn't
                // a semi-colon, then just insert the escape character as a literal.
                // Be careful not to double-escape characters!
                if  (*pSubFieldCharacter == KVersitTokenSemiColonVal ||
                     *pSubFieldCharacter == KEscapedCharacterValue)
                    {
                    // Convert "\;" to ";" and "\\" to "\"
                    *pDestSubFieldCharacter++ = *pSubFieldCharacter;
                    --finalLength;
                    }
                else
                    {
                    // Convert "\<anything else>" to "\<anything else>"
                    *pDestSubFieldCharacter++ = KEscapedCharacterValue;
                    *pDestSubFieldCharacter++ = *pSubFieldCharacter;
                    }
                //
                previousCharacterWasEscaped = EFalse;
                }
            else
                {
                // Don't copy escaped characters to the output sub-field.
                previousCharacterWasEscaped = (*pSubFieldCharacter == KEscapedCharacterValue);
                if  (!previousCharacterWasEscaped)
                    {
                    *pDestSubFieldCharacter++ = *pSubFieldCharacter;
                    }
                }

            // Move on to next character
            ++pSubFieldCharacter;
            }
        //
        aText.SetLength(finalLength);
        }
    }

EXPORT_C TBool VersitUtils::IsNoneWhiteSpaceChar(const TDesC8& aString)
/** Tests whether the specified 8 bit string contains no white spaces. 

Any CR/LF pair at the end of the string is disregarded.

@param aString The string to test.
@return ETrue if the string contains no white spaces, otherwise EFalse. */
	{
	TInt length=aString.Length();
	if (length>0 && aString[length-1]==CVersitParser::ELineFeed)		//Count CRLF at the end as white space
		{
		if (--length>0 && aString[length-1]==CVersitParser::ECarriageReturn)
			--length;
		}
	TInt ii;
	for (ii=0;ii<length;++ii)
		{
		if (!IsWhiteSpace(aString[ii]))
			return ETrue;
		}
	return EFalse;
	}

EXPORT_C TBool VersitUtils::IsNoneWhiteSpaceWideChar(const TDesC16& aString)
/** Tests whether the specified 16 bit string contains no white spaces. 

Any CR/LF pair at the end of the string is disregarded.

@param aString The string to test.
@return ETrue if the string contains no white spaces, otherwise EFalse. */
	{
	TInt length=aString.Length();
	if (length>0 && aString[length-1]==CVersitParser::ELineFeed)		//Count CRLF at the end as white space
		{
		if (--length>0 && aString[length-1]==CVersitParser::ECarriageReturn)
			--length;
		}
	TInt ii;
	for (ii=0;ii<length;++ii)
		{
		if (!IsWhiteSpace(aString[ii]))
			return ETrue;
		}
	return EFalse;
	}

EXPORT_C void VersitUtils::StripWhiteSpace(TPtr8& aString)
/** Strips white space from the beginning and end of the specified 8-bit string. 

@param aString A 8-bit string. */
	{
	TInt length=aString.Length();
	TInt firstNoneWhiteSpace;
	for (firstNoneWhiteSpace=0;firstNoneWhiteSpace<length;++firstNoneWhiteSpace)
		{
		if (!IsWhiteSpace(aString[firstNoneWhiteSpace]))
			break;
		}
	if (firstNoneWhiteSpace==length)
		{
		aString.Set(NULL,0,0);
		return;
		}
	TInt lastNoneWhiteSpace;
	for (lastNoneWhiteSpace=length-1;lastNoneWhiteSpace>0;--lastNoneWhiteSpace)
		{
		if (!IsWhiteSpace(aString[lastNoneWhiteSpace]))
			break;
		}
	length=lastNoneWhiteSpace-firstNoneWhiteSpace+1;
	aString.Set(&aString[firstNoneWhiteSpace],length,length);
	}

EXPORT_C void VersitUtils::StripWhiteSpace(TPtr16& aString)
/** Strips white space from the beginning and end of the specified 16-bit string. 

@param aString A 16-bit string. */
	{
	TInt length=aString.Length();
	if (length==0)
		return;
	TInt firstNoneWhiteSpace;
	for (firstNoneWhiteSpace=0;firstNoneWhiteSpace<length;++firstNoneWhiteSpace)
		{
		if (!IsWhiteSpace(aString[firstNoneWhiteSpace]))
			break;
		}
	if (firstNoneWhiteSpace==length)
		{
		aString.Set(NULL,0,0);
		return;
		}
	TInt lastNoneWhiteSpace;
	for (lastNoneWhiteSpace=length-1;lastNoneWhiteSpace>0;--lastNoneWhiteSpace)
		{
		if (!IsWhiteSpace(aString[lastNoneWhiteSpace]))
			break;
		}
	length=lastNoneWhiteSpace-firstNoneWhiteSpace+1;
	aString.Set(&aString[firstNoneWhiteSpace],length,length);
	}

EXPORT_C CDesCArray* VersitUtils::ParseForArrayPropertiesL(TPtr16 aStringValue, TUint aLineCharacterSetId)
/** Parses a compound property value string. 

The sub-values found are appended to an array, after removal of escape characters. 
The array is returned, and ownership is transferred to the caller.

@param aStringValue Compound property value string to parse for array elements.
@param aLineCharacterSetId The character set of the current line. This is required 
in order to identify the escape character.
@return Array of property values found in the string. */
    {
	const TInt length = aStringValue.Length();
    const TDesC &KEscapeString = EscapeChar( KCharacterSetIdentifierShiftJis == aLineCharacterSetId );
	const TUint16 KEscapedCharacterValue = KEscapeString[0];
    //
	CDesCArray* arrayOfValues = new(ELeave) CDesCArrayFlat(5);
	if  (length)
        {
        CleanupStack::PushL(arrayOfValues);
        //
        TInt i = 0;
        TInt subFieldIndex = 0;
        TInt subFieldStartCharacterIndex = 0;
        TUint16* pCharacter = const_cast<TUint16*>(&aStringValue[0]); 
        TBool previousCharacterWasEscaped = EFalse;
        TBool escapeEscaped = EFalse;
        //
        do
            {
            if  ((*pCharacter == KVersitTokenSemiColonVal && (!previousCharacterWasEscaped || escapeEscaped) ) || i == length)
                {
                // One complete sub-field reached. First check for pronunciation data...
                TInt subFieldLength = (i - subFieldStartCharacterIndex);
                TPtrC16 pConstRawSubField(aStringValue.Mid(subFieldStartCharacterIndex, subFieldLength));
                TPtr16 pRawSubField(const_cast<TUint16*>(pConstRawSubField.Ptr()), pConstRawSubField.Length(), pConstRawSubField.Length());
                
                // Now un-escape the sub field
                RemoveEscapeChars(pRawSubField, aLineCharacterSetId);

                // Save sub-field to array
        		arrayOfValues->AppendL(pRawSubField);

                // Move on to the next sub field
                ++subFieldIndex;
                previousCharacterWasEscaped = EFalse;
                subFieldStartCharacterIndex = i + 1;
                }
            //
            TBool lastEscape = previousCharacterWasEscaped;
            previousCharacterWasEscaped = (*pCharacter == KEscapedCharacterValue);
            escapeEscaped = !escapeEscaped && previousCharacterWasEscaped && lastEscape;// if // set true, if // or / set false
            
            if  (++i <= length)
                {
                ++pCharacter;
                }
            }
        while (i <= length);

        //
        CleanupStack::Pop(arrayOfValues);
        }
    //
	return arrayOfValues;
    }


EXPORT_C void VersitUtils::AllocateAdditionalPropertyStorageL(CVersitTlsData& aTlsData, CParserProperty& aProperty, TPtr16& aStringValue, TUint aLineCharacterSetId)
/** Allocates an additional property value object for certain CParserProperty objects. 

This is needed to support the parsing and storage of Japanese pronunciation SOUND properties. 
Due to the existing Symbian Versit architecture, it was not possible to change the 
underlying storage type of the SOUND property to an array data type. Therefore, this method 
provides a means of storing the array-based value which can be retrieved when necessary.

It is called by CParserVCard::MakePropertyValueSoundL().

@param aTlsData The CVersitParser::iStaticUtils member.
@param aProperty A SOUND property.
@param aStringValue A 16-bit raw property value string. 
@param aLineCharacterSetId The character set ID of the current line. 
This is required to identify the escape character. */
    {
    
    // Only SOUND property currently supports this extension approach
    if  (aProperty.NameUid() == TUid::Uid(KVCardPropertySoundUid))
        {
        // Check for array of values in the case of Japanese pronunciation data
        CDesCArray* arrayOfValues = ParseForArrayPropertiesL(aStringValue, aLineCharacterSetId);
        if  (arrayOfValues && arrayOfValues->Count() > 1)
            {
            // Create additional property value
			CleanupStack::PushL(arrayOfValues);
			CParserPropertyValueCDesCArray* value = new(ELeave) CParserPropertyValueCDesCArray(arrayOfValues);

            // Add it to storage area
            CVersitAdditionalStorage& additionalStorage = aTlsData.AdditionalStorage();
            additionalStorage.AddToStorageL(&aProperty, value);

            // Tidy up
			CleanupStack::Pop(arrayOfValues);
            }
        else
            {
            // Single element, therefore no need to store as an array-based property.
            delete arrayOfValues;
            }
        }
    }

EXPORT_C void VersitUtils::FreeAdditionalPropertyStorageL(const CParserProperty& aProperty)
/** Frees any additional storage associated with the specified property. 

Additional storage may have previously been allocated using AllocateAdditionalPropertyStorageL().

This is called by CParserProperty's destructor.

@param aProperty The property which may have additional storage associated with it. 
@see VersitUtils::AllocateAdditionalPropertyStorageL() */
    {
    // Only SOUND property currently supports this extension approach.
    if  (aProperty.NameUid() == TUid::Uid(KVCardPropertySoundUid))
        {
        // If the property type is SOUND then it is possible that the property will also
        // have an attached array-based property value. Make sure we free that too.
        CVersitTlsData& data = CVersitTlsData::VersitTlsDataL();
        CVersitAdditionalStorage& additionalStorage = data.AdditionalStorage();
        additionalStorage.FreeStorage(&aProperty);
        data.CloseVersitTlsData();
        }
    }

EXPORT_C CParserPropertyValue* VersitUtils::AdditionalPropertyValueFromStorageL(const CParserProperty& aProperty)
/** Retrieves any additional storage property value associated with the 
specified property.

@param aProperty The property that may have additional associated data.
@return Additional storage property value associated with the specified property.
@see VersitUtils::AllocateAdditionalPropertyStorageL() */
    {
    CParserPropertyValue* ret = NULL;

    // Only SOUND property currently supports this extension approach.
    if  (aProperty.NameUid() == TUid::Uid(KVCardPropertySoundUid))
        {
        // If the property type is SOUND then it is possible that the property will also
        // have an attached array-based property value. Make sure we free that too.
        CVersitTlsData& data = CVersitTlsData::VersitTlsDataL();
        CVersitAdditionalStorage& additionalStorage = data.AdditionalStorage();
        CBase* storedValue = additionalStorage.FromStorage(&aProperty);
        ret = static_cast<CParserPropertyValue*>(storedValue);
        data.CloseVersitTlsData();
        }
    //
    return ret;
    }

const TDesC &VersitUtils::EscapeChar(TBool aIsShiftJis)
/**
If using shifjis charset return shiftjis charset as generated by relevant charconv plugin
else return standard unicode escape charcter \
*/
	{
    if( aIsShiftJis )
    	{
		CVersitTlsData *data = NULL;
		TRAP_IGNORE( data = &CVersitTlsData::VersitTlsDataL() );
		if (data)
			{
		    CVersitAdditionalStorage& additionalStorage = data->AdditionalStorage();
		    CBase* storedValue = additionalStorage.FromStorage(KTLSVars);
		    data->CloseVersitTlsData();
		    
		    if( NULL == storedValue )
		    	{
		    	return KVersitEscapeCharacter();
		    	}
		    else
		    	{
		    	return *( static_cast<CVersitTLSContainer *>(storedValue)->iShiftJisEscape );
		    	}
			}
    	}
    	return KVersitEscapeCharacter();
	}



