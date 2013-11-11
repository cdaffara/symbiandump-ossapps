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
/**
 * @file 
 * @publishedAll
 * @released
 */

#if !defined(__IMCVCODC_H__)
#define __IMCVCODC_H__

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

#define KBase64 _L("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=")
#define KCharFormat _L("%c")

// Shared implementation code	
//----------------------------------------------------------------------------------------
class TImCodec
//----------------------------------------------------------------------------------------
/**
@publishedAll
@released
*/
	{
protected:
	// base64 and UU coding defines.
	enum EMaskValues{ ESixBitMask = 0x3F, EEightBitMask = 0xFF };
	enum EMaskShiftValues{ ESix = 6, EFour = 4, ETwo = 2, EZero = 0 };

public:
	virtual TInt Encode( const TDesC8& aSrcString, TDes8& rDestString)=0 ;
	virtual TBool Decode( const TDesC8& aInputLine, TDes8& rOutputLine)=0;

protected:
	// TImCodec();
	inline TBool IsDigit( TChar aChar );
	};


// Interface for sending file data.
//----------------------------------------------------------------------------------------
class TImFileCodec : public TImCodec
//----------------------------------------------------------------------------------------
/**
@publishedAll
@released
*/
	{
public:
	virtual TInt PrefixNextLineL( TDes8& rOutputLine, const TFileName& aName, TInt& rPaddingCount );
	virtual TInt PostfixNextLine( TDes8& rOutputLine, TInt& rPaddingCount );
	virtual void Initialise();

protected:
	TInt iPrefixState;
	TInt iPostfixState;
	};	

// Dummy, just copies
//----------------------------------------------------------------------------------------
class TImCodecNull :  public TImFileCodec
//----------------------------------------------------------------------------------------
/**
@publishedAll
@released
*/
	{
public:
	TInt Encode( const TDesC8& aSrcString, TDes8& rDestString);
	TBool Decode( const TDesC8& aInputLine, TDes8& rOutputLine);
	};
	
// Quoted-printable encoding/decoding

//----------------------------------------------------------------------------------------
class TImCodecQP : public TImFileCodec
//----------------------------------------------------------------------------------------
/**
@publishedAll
@released
*/
	{
public:
	IMPORT_C TImCodecQP();
	IMPORT_C TInt Encode( const TDesC8& aSrcString, TDes8& rDestString);
	IMPORT_C TBool Decode( const TDesC8& aInputLine, TDes8& rOutputLine);

	// Not to be used anymore..
	IMPORT_C TInt EncodeRichText( const TDesC8& aInputLine, TDes8& rOutputLine);
	IMPORT_C TInt DecodeRichText( const TDesC8& aSrcString, TDes& rDestString );

	// Functions which allow flexiblity. Can replace the '=' char or add characters..
	// to what is defined as 'plain.
	inline void AddPlainChar(const TDesC8&  aCharList );
	inline void AddEncodeChar(const TDesC8&  aCharList );
	inline void SetQPChar( TUint8 aChar);

	inline TUint8 ReplacementChar( TChar aControlChar );
	inline TBool IsPlain( TChar aChar );

private:
	TBool SmartBreak( TInt written, const TDesC8& pSource );
	inline TBool IsBreakable( TChar aChar);
	inline void AddSoftLineBreak(TDes8& aPtr, TInt& aPadding, TInt& aWritten);
	inline void AddSoftLineBreak(const TUint8* apEnd, TUint8* aPtr, TInt& aPadding, TInt& aWritten);

private:
	TUint8 iQPCharacter;
	TPtrC8 iPlainCharList;  
	TPtrC8 iEncodeCharList;  

	TInt iPaddingCount;
	};



// Base64 coding/decoding

//----------------------------------------------------------------------------------------
class TImCodecB64 : public TImFileCodec
//----------------------------------------------------------------------------------------
/**
@publishedAll
@released
*/
	{
private:
	// base64 coding defines
	enum{ EPadChar = 64 };

public:
	IMPORT_C TImCodecB64();
	IMPORT_C TInt Encode( const TDesC8& aSrcString, TDes8& rDestString);
	IMPORT_C TBool Decode( const TDesC8& aSrcString, TDes8& rDestString);
 	IMPORT_C void  Initialise();

protected:
	TInt DoEncode(const TDesC8& aSrcString, TDes8& rDestString, TBool aInsertLineBreaks);

private:
	TInt iShiftStored;
	TInt iMaskShiftStored;
	};

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS  
#include "cimconvertheader.h"
#endif

// UU coding/decoding

//----------------------------------------------------------------------------------------
class TImCodecUU : public TImFileCodec
//----------------------------------------------------------------------------------------
/**
@publishedAll
@released
*/
	{
private:
	// UU coding defines

	enum{ ESpace = 32, EBackQuote = 96 };

	enum TImBodyPostfix
		{
		EInvertedComma = 0,
		EEndString,
		EEndOfPostfix
		};

public:
	IMPORT_C TImCodecUU();
	IMPORT_C TInt Encode( const TDesC8& aSrcString, TDes8& rDestString );
	IMPORT_C TBool Decode( const TDesC8& aSrcString, TDes8& rDestString );
	TInt PrefixNextLineL( TDes8& rOutputLine, const TFileName& aName, TInt& rPaddingCount );
	TInt PostfixNextLine( TDes8& rOutputLine, TInt& rPaddingCount );
private:
	void AppendFilenameL( TDes8& rOutputLine, const TFileName& aName );
	
	};

#endif

