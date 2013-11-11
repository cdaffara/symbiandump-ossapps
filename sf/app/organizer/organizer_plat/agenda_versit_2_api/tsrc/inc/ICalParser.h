#ifndef ICAL_PARSER_H
#define ICAL_PARSER_H/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Holds the definition of CICalParser.
*
*/



// System includes.
#include <e32base.h>	// CBase

// Forward declarations.
class CICal;

/**
Directs parsing of iCalendar streams.
@publishedPartner
*/
class CICalParser : public CBase
	{
public:	// Construction/destruction.
	IMPORT_C static CICalParser* NewL();
	IMPORT_C static CICalParser* NewLC();
	~CICalParser();
	
public:	// Methods.
	IMPORT_C void InternalizeL(RReadStream& aReadStream);
	IMPORT_C TInt CalCount() const;
	IMPORT_C CICal& Cal(TInt aIndex);
	
private:	// Construction.
	CICalParser();
	void ConstructL();

private:	// Attributes.
	RPointerArray<CICal> iCals;	
	};
	
#endif //ICAL_PARSER_H

// End of File
