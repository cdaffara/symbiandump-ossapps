// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cimapcharconv.h"
#include <miutconv.h>
#include <imcvcodc.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "cimconvertcharconv.h"  
#include "cimconvertheader.h"
#endif
const TInt KBufSize = 100;

CImapCharconv* CImapCharconv::NewL(RFs& aFs)
	{
	CImapCharconv* self = new (ELeave) CImapCharconv();
	CleanupStack::PushL(self);
	self->ConstructL(aFs);
	CleanupStack::Pop();
	return self;
	}

CImapCharconv::CImapCharconv()
	{
	}

void CImapCharconv::ConstructL(RFs& aFs)
	{
	iCharacterConverter = CCnvCharacterSetConverter::NewL();
	iCharConv = CImConvertCharconv::NewL(*iCharacterConverter, aFs);
	iHeaderConverter = CImConvertHeader::NewL(*iCharConv);
	}

CImapCharconv::~CImapCharconv()
	{
	delete iHeaderConverter;
	delete iCharConv;
	delete iCharacterConverter;	
	}

/**
Converts the supplied Imap-Utf7 encoded string into Unicode, allocating sufficient buffer space for the Unicode string.
@return a heap buffer containing the Unicode string
*/
EXPORT_C HBufC16* CImapCharconv::ConvertToUnicodeFromImapUtf7L(const TDesC8& aImapUtf7)
// This code is based on CnvUtfConverter::ConvertToUnicodeFromUtf7L()
	{
	// If aImapUtf7 is an empty string return 
	if (aImapUtf7.Length()==0)
		{
		HBufC16* hBuf = HBufC16::NewL(1);
		return hBuf;
		}

	// else convert aImapUtf7 to Unicode storing the result in a buffer, reallocating
	// it when needed.
	TInt length = aImapUtf7.Length();

	iCharConv->PrepareToConvertToFromOurCharsetL(KCharacterSetIdentifierImapUtf7);

	TPtrC8 utf7 (aImapUtf7);
	TBuf<KBufSize> buf;
	HBufC16* hBuf = HBufC16::NewLC(length);
	TPtr unicode = hBuf->Des();

	// Dummy parameters for the call to ConvertToOurCharsetL()
	// Note that they don't actually get updated by this method.
	// In particular, numUnconvertedChars does not get updated.  
	// So we use the return value (assigned to "unconverted") instead.
	TInt numUnconvertedChars=0;
	TInt indexFirstUnconvertedChar=0;

	FOREVER
		{
		TInt unconverted = iCharConv->ConvertToOurCharsetL(utf7, buf, numUnconvertedChars, indexFirstUnconvertedChar);
		if(unconverted == CnvUtfConverter::EErrorIllFormedInput || unconverted < 0)
			{
			User::Leave(KErrCorrupt);
			}	        

		if (unicode.Length() + buf.Length() > unicode.MaxLength())
			{
			// Reallocate hBuf
			hBuf = hBuf->ReAllocL(unicode.Length() + buf.Length());
			CleanupStack::Pop();
			CleanupStack::PushL(hBuf);
			unicode.Set(hBuf->Des());
			}
		
		unicode.Append(buf);
		if (unconverted == 0)
			{
			break;
			}

		utf7.Set(utf7.Right(unconverted));
		}

	CleanupStack::Pop(hBuf);
	return hBuf;
	}

/**
Converts the supplied Unicode string into a Imap-Utf7 encoded string, allocating sufficient buffer space for the Imap-Utf7 string.
@return a heap buffer containing the Imap-Utf7 encoded string
*/
EXPORT_C HBufC8* CImapCharconv::ConvertFromUnicodeToImapUtf7L(const TDesC16& aUnicode)
// This code is based on CnvUtfConverter::ConvertFromUnicodeToImapUtf7L()
	{
	// If aUnicode is  Null string, return an empty HBufC
	if (aUnicode.Length() == 0)
		{
		HBufC8* hBuf8 = HBufC8::NewL(1);
		return hBuf8;
		}

	// Otherwise, convert and store result in a buffer, reallocating that buffer if needed.
	TInt length = aUnicode.Length();
	
	iCharConv->PrepareToConvertToFromOurCharsetL(KCharacterSetIdentifierImapUtf7);

	TPtrC16 unicode (aUnicode);
	TBuf8<KBufSize> buf;
	HBufC8* hBuf8 = HBufC8::NewLC(length);
	TPtr8 utf7 = hBuf8->Des();

	// Dummy parameters for the call to ConvertFromOurCharsetL()
	// Note that they don't actually get updated by this method.
	// In particular, numUnconvertedChars does not get updated.  
	// So we use the return value (assigned to "unconverted") instead.
	TInt numUnconvertedChars=0;
	TInt indexFirstUnconvertedChar=0;

	FOREVER
		{
		TInt unconverted = iCharConv->ConvertFromOurCharsetL(unicode, buf, numUnconvertedChars, indexFirstUnconvertedChar);
		if(unconverted == CnvUtfConverter::EErrorIllFormedInput || unconverted < 0)
			{
			User::Leave(KErrCorrupt);
			}            

		if (utf7.Length() + buf.Length() > utf7.MaxLength())
			{
			// Reallocate the hBuf8
			hBuf8 = hBuf8->ReAllocL(utf7.Length() + buf.Length());
			CleanupStack::Pop();
			CleanupStack::PushL(hBuf8);
			utf7.Set(hBuf8->Des());
			}
		utf7.Append(buf);
		if (unconverted == 0) 
			{
			break;
			}
		unicode.Set(unicode.Right(unconverted));
		}
	CleanupStack::Pop();
	return hBuf8;
	}
	
	

/**
Gets the header converter

@return Header converter
*/
EXPORT_C CImConvertHeader& CImapCharconv::HeaderConverter()
	{
	return *iHeaderConverter;
	}

