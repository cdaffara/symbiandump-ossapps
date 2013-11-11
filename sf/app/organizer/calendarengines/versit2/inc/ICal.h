#ifndef ICAL_H
#define ICAL_H/*
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
* Description:   Holds the definiton of CICal.
*
*/



// User includes.
#include "ICalBase.h"	// CICalBase

// Forward declaration.
class RWriteStream;

/**
Class representing a VCALENDAR component.
@publishedPartner
*/
class CICal : public CICalBase
	{
public:	// Construction/destruction.
	IMPORT_C static CICal* NewL();
	IMPORT_C static CICal* NewLC();
	~CICal();

public:	// Methods.
	IMPORT_C void ExternalizeL(RWriteStream& aWriteStream);
	IMPORT_C void SetMethodL(TICalMethod aMethod);

private:	// Construction.
	CICal();
	void ConstructL();
	
private:	// From CICalBase.
	TBool ValidatePropertyImpl(const TDesC& aName) const;
	TBool ValidateComponent(TICalComponentType aType) const;
	};
	
#endif	// ICAL_H

// End of File
