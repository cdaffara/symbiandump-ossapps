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

#ifndef __CIMAPCHARCONV_H__
#define __CIMAPCHARCONV_H__

#include <e32base.h>

class CCnvCharacterSetConverter;
class CImConvertCharconv;
class CImConvertHeader;
class RFs;

/**
@internalTechnology
@prototype
*/
class CImapCharconv : public CBase
	{
public:
	static CImapCharconv* NewL(RFs& aFs);
	~CImapCharconv();

	IMPORT_C HBufC16* ConvertToUnicodeFromImapUtf7L(const TDesC8& aImapUtf7);
	IMPORT_C HBufC8* ConvertFromUnicodeToImapUtf7L(const TDesC16& aUnicode);
	
	IMPORT_C CImConvertHeader& HeaderConverter();

private:
	CImapCharconv();
	void ConstructL(RFs& aFs);
	
private:
	CCnvCharacterSetConverter* iCharacterConverter;
	CImConvertCharconv* iCharConv;
	CImConvertHeader* iHeaderConverter;
	};

#endif // __CIMAPCHARCONV_H__
