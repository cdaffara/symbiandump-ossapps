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
// cimconvertheader.h
//
/**
 * @file
 * @internal
 * @released
 */

#if !defined(__CIMCONVERTHEADER_H__)
#define __CIMCONVERTHEADER_H__

#include <s32buf.h>
#include <s32stor.h>
#include <txtrich.h>
#include <miutatch.h>
#include <miutconv.h>
#include <miuthdr.h>
#include <mentact.h>		// CMsgActive

#include <imcvdata.h>
#include <imcvtext.h>
#include <imutdll.h>
#include <imcvcodc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "timrfc822datefield.h"	
#include "cimconvertcharconv.h"				
#endif

/**
Maximum length allowed for an 'encoded-word' rfc2047
@internalComponent
@released
*/
const TInt KEncodedWordMaxLength = 75;

/**
All encoding information except data and charset name length. =?B?=?    => 7
@internalComponent
@released
*/
const TInt KMaxEncodedInformationLength = 7;

/**
Length of the field name, eg "Subject: ". This may change as other fields are processed.
@internalComponent
@released
*/
const TInt KMaxHeaderFieldNameLength = 12;

/**
@internalComponent
@released
*/
const TInt KImCvFinished=(-1);
const TInt KImCvAdvance=(1);

// error states involved in sending attachments etc.
/**
@internalComponent
@deprecated
*/
enum TIattErrorCode
	{
	KImAttNoError = 0,
	KImAttFinished = 1
	};


/**
@internalComponent
@released
*/
enum TImBodyConvAlgorithm
	{
	ENoAlgorithm,
	EQPEncode,
	EQPDecode,
	EBase64Encode,
	EBase64Decode,
	EUUEncode,
	EUUDecode
	};
	
// Used in CImSendConvert
/**
@internalComponent
@deprecated
*/
enum TImSendEncoding 
	{ 
	ESendNoAlgorithm,
	ESendMimeAlgorithm
	};	

struct SAttachmentInfo
/**
@internalComponent
@released
*/
	{
	TInt iSize;
	TFileName iName;
	TFileName iPath;
	TBool iComplete;
	};

class TImCodecB64WithLineBreaks : public TImCodecB64
/**
@internalComponent
@released
*/
	{
public:
	TInt Encode(const TDesC8& aSrcString, TDes8& rDestString);
	};

// For converting a text string to/from encoded form
// Note: 'Word' here has special meaning derived from the one defined in rfc2047.
// It refers to one encoding instance.

//----------------------------------------------------------------------------------------
class CImConvertHeader : public CBase
//----------------------------------------------------------------------------------------
/**
@internalTechnology
@released
*/
	{
public:
	IMPORT_C static CImConvertHeader* NewL(CImConvertCharconv& aConverter);
	~CImConvertHeader();

	IMPORT_C void SetMessageType(TBool aIsMIME); 
	IMPORT_C void DecodeAllHeaderFieldsL(CImHeader& rHeader); 

	// Functions for decoding & converting descriptors

	IMPORT_C void DecodeNonMIMEHeaderFieldL(const TDesC8& aBufIn, TDes& aBufOut); 
	IMPORT_C void DecodeHeaderFieldL(const TDesC8& aBufIn, TDes& aBufOut); 
	IMPORT_C void DecodeHeaderFieldL(const TDesC16& aBufIn, TDes& aBufOut); 
	IMPORT_C void ConvertHeaderFieldL(const TDesC16& aBufIn, RBuf8& aBufOut, TBool aIsAddressField); 
	
	// Header needed for retrieving encoding information, used in Send code.
    IMPORT_C void EncodeHeaderFieldL(const TDesC& aBufIn, RBuf8& aBufOut, 
            CArrayFix<TImHeaderEncodingInfo>* aInfoArray, TInt aState, TInt aArrayVal = 0);

	IMPORT_C void EncodeHeaderFieldL(const TDesC& aBufIn, RBuf8& aBufOut,  
		                             const TUint aCharset, const TImHeaderEncodingInfo::TEncodingType aType, TBool aIsAddressField);
	
	IMPORT_C TBool FindEncodedWord(TPtrC8& aData, TInt& aInit,TInt& rStart, TInt& rEnd);

	void Append(TDes& aBuffer, const TDesC8& aAddition);
	inline CImConvertCharconv& CharConv();

	IMPORT_C TUint OverrideCharset() const;
	IMPORT_C void SetOverrideCharset(TUint aCharset);

private:
	void ConstructL();
	CImConvertHeader(CImConvertCharconv&);
	// Functions dealing only with single 'encoded-word's
	void GetCharsetAndEncodeDataL(const TDesC& aBufIn, RBuf8& aBufOut, TImHeaderEncodingInfo& aInfo);
	void EncodeWordL(const TDesC& aBufIn, RBuf8& aBufOut, TImHeaderEncodingInfo& aInfo,
						const TDesC8& aCharsetName, RBuf8& aEncodedWord);

	TBool DecodeWordL(const TDesC8& aBufIn, TDes& aBufOut, TInt rRemainder); 
	void DecodeRecipientListL( CDesCArray& aArray);
	void DecodeFieldL(const TDesC& aField);

	TBool DoCharsetConversionL(const TDesC& aDataToConvert, RBuf8& aEncodedWord);

	void DoEncodingL(RBuf8& aEncodedWord, TImHeaderEncodingInfo& aInfo);

	TInt AddEncodedWordInfoL(const TDesC8& aEncodedWord, RBuf8& aBufOut, 
							 TImHeaderEncodingInfo& aInfo, const TDesC8& aCharsetName);

	TBool IsAscii( TUint aChar ) const;
	TBool ExtractTextToEncode(const TDesC& aBufIn, TInt& rStart, TInt& rEnd, TBool aIsAddressField);
	void Insert(TDes8& aBuffer, const TDesC16& aInsert);

private:
	TImHeaderEncodingInfo iEncodingInfo;
	CImConvertCharconv& iCharConv;

	// Get access to QP/ Base64 encoding/decoding functions
	TImCodecB64	 iB64Codec;
	TImCodecQP	 iQPCodec;

	// Need in the narrow build, to check if possible to convert to/from
	// the machines character set.
	TUint iCharacterSetId; 

	CImHeader* iHeader;	

	// Converted static variables used in EncodeWordL()
	TBool	isMIMEMessageHeader;
	TUint   iOverrideCharset;
	};

#include <imcvcodc.inl>

#endif //__CIMCONVERTHEADER_H__

