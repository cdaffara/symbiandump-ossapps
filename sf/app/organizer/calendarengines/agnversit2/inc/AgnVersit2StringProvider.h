#ifndef AGNVERSIT2STRINGPROVIDER_H
#define AGNVERSIT2STRINGPROVIDER_H/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Caches language specific text fields from a
*  resource file and provides access to them.
*
*/



//System includes
#include <e32base.h>				// CBase
#include <f32file.h>				// RFs
#include <barsc.h>					// RResourceFile
#include <barsread.h>				// TResourceReader

//User includes
#include "agnversit2strings.hrh"	// TStringId

/**
Class to load and cache strings from resource files.
@internalTechnology
*/
class CAgnVersit2StringProvider : public CBase
	{
public:		// Construction/destruction.
	static CAgnVersit2StringProvider* NewL();
	~CAgnVersit2StringProvider();

public:		// Methods.
	const TDesC& StringL(TStringId aStringId);

private:	// Construction.
	CAgnVersit2StringProvider();
	void ConstructL();

private:	// Methods.
	void ReadStringFromResourceL(TStringId aStringId);
	void ReadAllStringsL();
	
private:	// Members.
	RFs iFs;
	HBufC* iResourceBuffer;
	RResourceFile iResourceFile;
	TResourceReader iResourceReader;
	RPointerArray<HBufC> iStringArray;
	};

#endif // AGNVERSIT2STRINGPROVIDER_H

// End of file.