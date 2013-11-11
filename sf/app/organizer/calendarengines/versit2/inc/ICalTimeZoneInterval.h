#ifndef ICALTIMEZONEINTERVAL_H
#define ICALTIMEZONEINTERVAL_H/*
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
* Description:   Holds the definition of CICalTimeZoneInterval.
*
*/



// User includes.
#include "ICalComponent.h"	// CICalComponent

/**
Class representing a VTIMEZONE DAYLIGHT/STANDARD component.
@internalTechnology
*/
class CICalTimeZoneInterval : public CICalComponent
	{
public:	// Construction/destruction.
	static CICalTimeZoneInterval* NewLC(TICalComponentType aType, TICalMethod aMethod);
	~CICalTimeZoneInterval();

private:	// Construction.
	CICalTimeZoneInterval(TICalComponentType aType);
	void ConstructL(TICalMethod aMethod);

private:	// From CICalBase.
	TBool ValidatePropertyImpl(const TDesC& aName) const;
	TBool ValidateComponent(TICalComponentType aType) const;

	};

#endif	// ICALTIMEZONEINTERVAL_H

// End of File
