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

#ifndef __VUTIL_H__
#define __VUTIL_H__

// System includes
#ifndef __E32BASE_H__
#include <e32base.h>
#endif
#ifndef __F32FILE_H__
#include <f32file.h>
#endif
#ifndef __CHARCONV_H__
#include <charconv.h>
#endif


class Versit
/** Provides Versit-specific encoding and character set identifiers. 
@publishedAll
@released
*/
	{
public:
	/** Versit-specific encoding identifiers. 
	@publishedAll
	@released
	*/
	enum TVersitEncoding
		{
		/** No encoding. */
		ENoEncoding,
		/** Quoted Printable encoding. */
		EQuotedPrintableEncoding,
		/** Base 64 encoding. */
		EBase64Encoding,

		// Used internally - not to be used by versit clients
		/** Eight bit encoding. */
		EEightBitEncoding
		};

	/** Versit-specific character set identifiers. These are used in the Versit API 
	to represent foreign (non-Unicode) character sets for vCard and vCalendar 
	properties. 
	@publishedAll
	@released */
	enum TVersitCharSet
		{
		/** Unknown character set. */
		EUnknownCharSet,
		/** ASCII (7-bit).*/
		EUSAsciiCharSet,	
		/** UTF-8 Unicode transformation format. */
		EUTF8CharSet,
		/** UTF-7 Unicode transformation format. */
		EUTF7CharSet,
		/** ISO 8859-1 (8-bit). */
		EISO88591CharSet,
		/** ISO 8859-2 (8-bit). */
		EISO88592CharSet,
		/** ISO 8859-4 (8-bit). */
		EISO88594CharSet,
		/** ISO 8859-5 (8-bit). */
		EISO88595CharSet,
		/** ISO 8859-7 (8-bit). */
		EISO88597CharSet,
		/** ISO 8859-9 (8-bit). */
		EISO88599CharSet,
		/** ISO 8859-3 (8-bit). */
		EISO88593CharSet,
		/** ISO 8859-10 (8-bit). */
		EISO885910CharSet,
		/** GB 2312. */
		EGB231280CharSet,
		/** GBK. */
		EGBKCharSet,
		/** Big 5. */
		EBIG5CharSet,
		/** ISO-2022-JP. */
		EISO2022JPCharSet,
		/** EUC-JP. */
		EEUCJPCharSet,
		/** Shift JIS. */
		EShiftJISCharSet,
		/** JIS. */
		EJISCharSet,
		/** Must be first character set. */
		EFirstCharSet=EUSAsciiCharSet,
		/** Must be last character set. */
		ELastCharSet=EJISCharSet
		};

	class TEncodingAndCharset
	/** Specifies an encoding, a character set and a character set converter. 
	@publishedAll
	@released */
		{
	public:
		inline TEncodingAndCharset(TVersitEncoding aEncoding,TUint aCharSetId) :iEncoding(aEncoding), iCharSetId(aCharSetId) 
		/** The C++ constructor initialises the encoding and character set.
	
		@param aEncoding An encoding.
		@param aCharSetId A character set. */
			{}
	public:
		/** An encoding. */
		TVersitEncoding iEncoding;
		/** A character set. */
		TUint iCharSetId;
		/** A character set converter. */
		CCnvCharacterSetConverter* iConverter;
		};
	};

class CVersitUnicodeUtils : public CBase
/** Versit Unicode utilities class. 

This enables conversion between Unicode and ISO character sets. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C ~CVersitUnicodeUtils();
	IMPORT_C HBufC8* NarrowL(const TDesC& aDesC);
	IMPORT_C HBufC8* NarrowLC(const TDesC& aDesC);
	IMPORT_C HBufC* WidenL(const TDesC8& aDesC8);
	IMPORT_C HBufC* WidenLC(const TDesC8& aDesC8);
	IMPORT_C void CreateConverterL();
	CCnvCharacterSetConverter::TAvailability SetCurrentCharSetL(TUint aCharacterSet);

public:
	inline CCnvCharacterSetConverter& CharacterSetConverter() const;
	inline TUint ConvertStandardNameL(const TDesC8& aStandardNameOfCharacterSet);
	inline HBufC8* StandardNameL(TUint aCharSetId);
	TUint AutoDetectCharSetL(const TDesC8& aSample,const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* aAutoDetectCharSets);

private:
	RFs iFsSession;
	CCnvCharacterSetConverter* iUnicodeConverter;
	TBool iFsConnected;
	TUint iCurrentConverterCharSet;
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* iCharSetsAvailable;
	};


inline CCnvCharacterSetConverter& CVersitUnicodeUtils::CharacterSetConverter() const
/** Returns the character set converter created in CreateConverterL().
	
@return A character set converter. */
	{
	return *iUnicodeConverter;
	}
inline TUint CVersitUnicodeUtils::ConvertStandardNameL(const TDesC8& aStandardNameOfCharacterSet)
/** Returns the Symbian OS UID for a character set from its Internet-standard 
name.
	
@param aStandardNameOfCharacterSet Internet-standard name of a character set 
encoded in 8-bit ASCII.
@return The Symbian OS UID of the specified character set. If the name is not 
known, zero is returned. */
	{
	return iUnicodeConverter->ConvertStandardNameOfCharacterSetToIdentifierL(aStandardNameOfCharacterSet,iFsSession);
	}
inline HBufC8* CVersitUnicodeUtils::StandardNameL(TUint aCharSetId)
/** Returns the Internet-standard name of a character set, from its Symbian OS UID. 

@param aCharSetId The Symbian OS UID of a character set.
@return The Internet-standard name or MIME name of the specified character 
set, or NULL if the UID is not known. The name is encoded in 8-bit ASCII. */
	{
	return iUnicodeConverter->ConvertCharacterSetIdentifierToStandardNameL(aCharSetId,iFsSession);
	}

#endif
