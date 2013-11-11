// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CSTOREUTILITIES_H__
#define __CSTOREUTILITIES_H__

#include <imcvsend.h>
#include <cmsvbodytext.h>
#include "cimapsettings.h"

/**
Used for requests to store CImHeader objects.
@internalComponent
@prototype
*/
class CStoreUtilities : public CBase
	{
public:
	static CStoreUtilities* NewL(TImEncodingType aEncodingType,TUint aCharsetId,RFs& aFs);
	virtual ~CStoreUtilities();
	HBufC8* DecodeL(const TPtrC8& aBodyData, const TBool aEndOfStream);
	void WriteToBodyL(const TDesC8& aText,CRichText* aMessageBody);
	void AttachFooterInfoL(TInt32 aBodyPartRemainingSize,CImapSettings& aImapSettings,HBufC8*& aDecodedData);
private:
	CStoreUtilities(TImEncodingType aEncodingType,TUint aCharsetId,RFs& aFs);
	void ConstructL();
	void AppendExtendL(HBufC8** aBufferPtr, const TDesC8& aText);
	TBool CheckUUEStartL(const TDesC8& aSourceLine);
private:
	TBuf8<20>				iLeftOver;
	// Decoder used for fetch in progres
	TImCodecB64				iB64Decoder;
	TImCodecUU				iUUDecoder;
	TImCodecQP				iQPDecoder;
	HBufC8*					iPartialLine;
	// Bool to indicate that we are in between of a UUEncoded start and end tag.
	TBool					iUUDecoding;
	HBufC*					iFooterString;
	TImEncodingType			iEncodingType;
	CImConvertCharconv*		iCharConv;
	CCnvCharacterSetConverter* iCharacterConverter;
	TUint					iCharsetId;
	RFs&					iFs;
	};

#endif //__CSTOREUTILITIES_H__