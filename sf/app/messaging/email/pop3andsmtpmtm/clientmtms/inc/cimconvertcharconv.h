// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// cimconvertcharconv.h
//

/**
 * @file 
 * @internal
 * @released
 */

#ifndef __CIMCONVERTCHARCONV_H__
#define __CIMCONVERTCHARCONV_H__

/**
 * IMail-specific stream type within an IMail file store for TImEmailTransformingInfo
 * @internalTechnology
 * @released
 */
const TUint KUidMsvCharsetNone				= {0x10003C7F};		// 268450943

/**
 * IMail-specific stream type within an IMail file store for TImEmailTransformingInfo
 * @internalComponent
 * @released
 */
const TUid KUidMsgFileTransformingInfo		= {0x100094A5};		// 268473509
const TUid KUidMsgEmailGeneralCenRep		= {0x10285A25};		// 271079973

/**
 * Default Sending Charset/Encoding Values
 * @internalComponent
 * @released
 */
_LIT8(KMiutUndisplayableChar, "?");
#define KDefaultPlainCharset		KCharacterSetIdentifierIso88591
const TImEncodingType KDefaultMIMEHeaderEncoding	  =	EEncodingTypeBASE64;		// MIME
const TImEncodingType KDefaultMIMEBodyTextEncoding	  =	EEncodingTypeQP;
const TImEncodingType KDefaultMIMEHTMLEncoding		  =	EEncodingTypeBASE64;
const TImEncodingType KDefaultMIMEAttachmentEncoding  =	EEncodingTypeBASE64;
const TImEncodingType KDefaultPlainHeaderEncoding	  =	EEncodingTypeNone;	// Non-MIME or Plain
const TImEncodingType KDefaultPlainBodyTextEncoding	  =	EEncodingTypeNone;
const TImEncodingType KDefaultPlainAttachmentEncoding =	EEncodingTypeUU;

// Wrapper class for CHARCONV.
// 'Our' charset dependant on build, UNICODE or CodePage 1252.
//
class CImConvertCharconv : public CBase
/**
@internalTechnology
@released
*/
	{
public:	
	IMPORT_C static CImConvertCharconv* NewL(CCnvCharacterSetConverter& aConverter, RFs& anFs);
	~CImConvertCharconv();

	// MUST call prepare function first.
	IMPORT_C TInt ConvertToOurCharsetL( const TDesC8& aBufIn, TDes& rBufOut, 
										TInt& rNumUnconvertedChars, 
										TInt& rIndexOfFirstUnconvertedChar);
	IMPORT_C TInt ConvertFromOurCharsetL(const TDesC& aBufIn, TDes8& rBufOut, 
										 TInt& rNumUnconvertedChars,
										 TInt& rIndexOfFirstUnconvertedChar);

	// MUST be called prior to calling above two functions.
	IMPORT_C TInt PrepareToConvertToFromOurCharsetL(const TUint aUid);

	IMPORT_C TUint GetMimeCharsetUidL(const TDesC8& aBufIn) const;
	IMPORT_C HBufC8* GetMimeCharsetTextStringL(const TUint& aUid) const;
	IMPORT_C TUint DefaultCharset() const;
	IMPORT_C TUint SystemDefaultCharset() const;

	static TInt StraightCopy( const TDesC8& aBufIn, TDes& rBufOut);
	static TInt StraightCopy( const TDesC& aBufIn, TDes8& rBufOut);
private:
	void ConstructL();
	CImConvertCharconv(CCnvCharacterSetConverter& aConverter, RFs& anFs);
	TBool CharsetAvailable(const TUint aUid);
	void SetSystemDefaultCharsetL();

	TInt GetSystemCharsetFromCenRepL(TDes8& aMimeCharSet);

	enum TEmailGeneralSettingsCenRepId
		{
		EEmailGeneralCharSetId				= 0x00000000
		};

private:
	CCnvCharacterSetConverter& iConverter;
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* iCharsetsAvailable; 

	RFs& iFs;
	TUint iCharsetUid;
	TUint iSystemDefaultCharset;		// Value obtained from .ini file
	TInt  iCharconvState;
	};

#endif //__CIMCONVERTCHARCONV_H__
