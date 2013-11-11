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

#include <vutil.h>

// System includes
#include <f32file.h>
#include <charconv.h>
#include <utf.h>
#include <s32mem.h>
#include <concnf.h>
#include <confndr.h>
#include <conlist.h>

// User includes
#include <vuid.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "versit_internal.h"
#endif 


EXPORT_C CVersitUnicodeUtils::~CVersitUnicodeUtils()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iCharSetsAvailable;
	delete iUnicodeConverter;
	if (iFsConnected)
		iFsSession.Close();
	}

EXPORT_C void CVersitUnicodeUtils::CreateConverterL()
/** Creates a converter for converting between Unicode and Code Page 1252.

Any unconvertible Unicode characters are converted into question marks. */
	{
	User::LeaveIfError(iFsSession.Connect());
	iFsConnected=ETrue;
	//
	iUnicodeConverter = CCnvCharacterSetConverter::NewL();
	iCharSetsAvailable = iUnicodeConverter->CreateArrayOfCharacterSetsAvailableL(iFsSession);
	//
	iUnicodeConverter->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, *iCharSetsAvailable, iFsSession);
	iUnicodeConverter->SetReplacementForUnconvertibleUnicodeCharactersL(KVersitQuestionMark);
	iCurrentConverterCharSet=KCharacterSetIdentifierCodePage1252;
	}

EXPORT_C HBufC8* CVersitUnicodeUtils::NarrowL(const TDesC& aDesC)
/** Converts a string from Unicode to Code Page 1252. 

@param aDesC The Unicode string to be converted.
@return A pointer to a Code Page 1252 string. The caller takes ownership. */
	{
	HBufC8* narrow=NarrowLC(aDesC);
	CleanupStack::Pop(); //buf8
	return narrow;
	}

EXPORT_C HBufC8* CVersitUnicodeUtils::NarrowLC(const TDesC& aDesC)
/** Converts a string from Unicode to Code Page 1252. 

@param aDesC The Unicode string to be converted.
@return A pointer to a Code Page 1252 string. The pointer is left on the cleanup 
stack. The caller takes ownership. */
	{
	HBufC8* buf8=HBufC8::NewLC(aDesC.Length());
	TPtr8 temp = buf8->Des();
	User::LeaveIfError(iUnicodeConverter->ConvertFromUnicode(temp, aDesC));
	return buf8;
	}

EXPORT_C HBufC* CVersitUnicodeUtils::WidenL(const TDesC8& aDesC8)
/** Converts a string from Code Page 1252 into Unicode. 

@param aDesC8 The Code Page 1252 string to be converted.
@return Pointer to the string converted into Unicode. The caller takes ownership. */
	{
	HBufC* widen=WidenLC(aDesC8);
	CleanupStack::Pop(); //buf
	return widen;
	}

EXPORT_C HBufC* CVersitUnicodeUtils::WidenLC(const TDesC8& aDesC8)
/** Converts a string from Code Page 1252 into Unicode. 

@param aDesC8 The Code Page 1252 string to be converted.
@return Pointer to the string converted into Unicode. The pointer is left on the 
cleanup stack. The caller takes ownership. */
	{
	HBufC* buf=HBufC::NewLC(aDesC8.Length());
	TInt state;
	TPtr temp = buf->Des();
	SetCurrentCharSetL(KCharacterSetIdentifierCodePage1252);
	User::LeaveIfError(iUnicodeConverter->ConvertToUnicode(temp, aDesC8, state));
	return buf;
	}

CCnvCharacterSetConverter::TAvailability CVersitUnicodeUtils::SetCurrentCharSetL(TUint aCharacterSet)
	{
	if (iCurrentConverterCharSet==aCharacterSet)
		return CCnvCharacterSetConverter::EAvailable;
	CCnvCharacterSetConverter::TAvailability avail=CCnvCharacterSetConverter::EAvailable;
	TRAPD(err,iUnicodeConverter->PrepareToConvertToOrFromL(aCharacterSet,*iCharSetsAvailable,iFsSession));
	if (err==KErrNotFound)
		avail=CCnvCharacterSetConverter::ENotAvailable;
	else
		User::LeaveIfError(err);
	if (avail==CCnvCharacterSetConverter::EAvailable)
		iCurrentConverterCharSet=aCharacterSet;
	return avail;
	}

TUint CVersitUnicodeUtils::AutoDetectCharSetL(const TDesC8& aSample
														,const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* aAutoDetectCharSets)
	{
	const CArrayFix<CCnvCharacterSetConverter::SCharacterSet>* charSets=(aAutoDetectCharSets ? aAutoDetectCharSets:iCharSetsAvailable);
	TUint charSetId;
	TInt confidenceLevel;

	iUnicodeConverter->AutoDetectCharacterSetL(confidenceLevel,charSetId,*charSets,aSample);
	return charSetId;
	}
