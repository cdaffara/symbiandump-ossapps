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

#ifndef __TJAPANESESOUNDPROPERTY_H__
#define __TJAPANESESOUNDPROPERTY_H__

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
    static CParserVCard*                    InternalizeCardLC(const TDesC& aFile);
    static CParserVCard*                    InternalizeCardLC(const TDesC& aPath, const TDesC& aFileName);
    static void                             CheckNumberOfSoundPropertiesL(const TDesC& aFile, const CParserVCard& aParser, TInt aExpectedArrayElementCount, TInt aExpectedPropertyCount = 1);
    static void                             CreateVCardTestL();
	};


#endif
