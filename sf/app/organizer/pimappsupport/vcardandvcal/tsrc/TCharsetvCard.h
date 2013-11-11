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

#ifndef __TCHARSETVCARD_H__
#define __TCHARSETVCARD_H__

// System includes
#include <vcal.h>
#include <vcard.h>
#include <e32cons.h>
#include <s32stor.h>
#include <s32file.h>
#include <f32file.h>
#include <charconv.h>


//
// ------> CVersitCardTest (header)
//
class CVersitCardTest : public CBase
	{
//
public:										// CONSTRUCT
//
	static CVersitCardTest*					NewLC();
	~CVersitCardTest();

//
private:									// INTERNAL CONSTRUCT
//
	void									ConstructL();

//
public:										// ACCESS
//
	void									StartTestsL();

//
private:									// INTERNAL
//
	
	// Read and write to the file using all encodings and character sets
	void									InternalizeAndExternalizeCardL(const TDesC& aFile, CParserVCard& aParser) const;

	// Read and write to the file using the specified default encodings and character
	// sets
	void									InternalizeCardL(const TDesC& aFile, CParserVCard& aParser, Versit::TVersitCharSet aCharSet, Versit::TVersitEncoding aEncoding) const;
	void									ExternalizeCardL(const TDesC& aFile, CParserVCard& aParser, Versit::TVersitCharSet aCharSet, Versit::TVersitEncoding aEncoding) const;

	// Return a descriptor representation of the specified character set
	static const TDesC&						CharsetAsDescriptor(Versit::TVersitCharSet aCharSet);

	// Return a descriptor representation of the specified encoding
	static const TDesC&						EncodingAsDescriptor(Versit::TVersitEncoding aEncoding);

	// Guess the charset of the document
	TUint									AutoDetectCharacterSetL(RFs& aFsSession, const TDesC& aFile) const;

	// Map charconv id to versit id
	static Versit::TVersitCharSet			MapCharConvCharSetToVersitCharSet(TUint aCharConvCharSet);

//
private:													// MEMBER DATA
//
	CDirScan*												iDirectoryScanner;
	CArrayFix<CCnvCharacterSetConverter::SCharacterSet>*	iArrayOfCharacterSetsAvailable;
	};


#endif
