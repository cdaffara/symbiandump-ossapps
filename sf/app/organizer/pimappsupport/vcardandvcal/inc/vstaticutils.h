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

#ifndef __VSTATICUTILS_H__
#define __VSTATICUTILS_H__

// System includes
#include <e32base.h>

// User includes
#include <versit.h>

// Classes referenced
class CVersitAdditionalPropertyStorage;

class VersitUtils
/** A utility class which provides functions for handling character set conversions, 
and for the parsing of character strings. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C static TBool EightBitEncoding(Versit::TVersitCharSet aCharSet);
	IMPORT_C static TBool EightBitEncoding(TUint aCharSetId);
	IMPORT_C static TUid ConArcEncodingUid(Versit::TVersitEncoding aEncoding);
	IMPORT_C static const TDesC8& IANACharacterSetName(Versit::TVersitCharSet aCharSet);
	IMPORT_C static const TDesC8& IANAEncodingName(Versit::TVersitEncoding aEncoding);
	IMPORT_C static TUid CharConvCharSetUid(Versit::TVersitCharSet aVersitSet);
	static Versit::TVersitCharSet CharSet(TUint aCharConvCharSetUid);
	IMPORT_C static TBool DescriptorContainsOnlySevenBitCharacters(const TDesC& aText);
	IMPORT_C static TBool RequiresEncoding(const TDesC& aText);
	IMPORT_C static void UncodeToNarrowL(const TDesC& aUnicode, TDes8& aNarrow, const Versit::TEncodingAndCharset& aEncodingCharset);
	IMPORT_C static void AddEscapedString(TDes& aDestination,const TDesC& aTextToEscape,Versit::TVersitCharSet aCharSet);
	IMPORT_C static void AddEscapedString(TDes& aDestination,const TDesC& aTextToEscape,TUint aCharSetId);
	IMPORT_C static void ConArcEncodeL(RReadStream& aSource, CBufBase& aTarget, TUid aConArcEncodingUid);
	IMPORT_C static void WrapLinesL(CBufBase& aBuffer, TInt aMaxLineLength);
	IMPORT_C static TBool CheckAndIgnoreCustomErrorL(TInt aError); 
	IMPORT_C static void RemoveEscapeChars(TPtr8& aText);
	IMPORT_C static void RemoveEscapeChars(HBufC16& aText);
	IMPORT_C static void RemoveEscapeChars(HBufC16& aText, TUint aCharSetUid);
	IMPORT_C static void RemoveEscapeChars(TPtr16& aText, TUint aCharSetUid);
	IMPORT_C static TBool IsNoneWhiteSpaceChar(const TDesC8& aString);
	IMPORT_C static void StripWhiteSpace(TPtr8& aString);
	IMPORT_C static void StripWhiteSpace(TPtr16& aString);
	static inline TBool IsWhiteSpace(TUint aChar);
	static inline TBool IsBeginOrEnd(TUid aUid);
	IMPORT_C static TBool IsNoneWhiteSpaceWideChar(const TDesC16& aString);
    IMPORT_C static CDesCArray* ParseForArrayPropertiesL(TPtr16 aStringValue, TUint aLineCharacterSetId);
    //
    IMPORT_C static void AllocateAdditionalPropertyStorageL(CVersitTlsData& aTlsData, CParserProperty& aProperty, TPtr16& aStringValue, TUint aLineCharacterSetId);
    IMPORT_C static void FreeAdditionalPropertyStorageL(const CParserProperty& aProperty);
    IMPORT_C static CParserPropertyValue* AdditionalPropertyValueFromStorageL(const CParserProperty& aProperty);

private:
	static void AddEscapedString(TBool aIsShiftJis,TDes& aDestination,const TDesC& aTextToEscape);
	static const TDesC &VersitUtils::EscapeChar(TBool aIsShiftJis);
	};

inline TBool VersitUtils::IsWhiteSpace(TUint aChar)
/** Tests whether the specified character is white space 
(either a tab or a space character).

@param aChar The character to test.
@return ETrue if the character is white space, otherwise EFalse. */
	{return (aChar==CVersitParser::ESpace || aChar==CVersitParser::EHTab);}
inline TBool VersitUtils::IsBeginOrEnd(TUid aUid)
/** Tests whether the specified UID is the Versit Begin or End UID (KVersitTokenBeginUid 
or KVersitTokenEndUid).

@param aUid The UID to test.
@return ETrue if the UID is the Begin or End UID, otherwise EFalse. */
	{return (aUid.iUid==KVersitTokenBeginUid || aUid.iUid==KVersitTokenEndUid);}
#endif
