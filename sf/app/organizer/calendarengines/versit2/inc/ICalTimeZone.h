#ifndef ICALTIMEZONE_H
#define ICALTIMEZONE_H/*
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
* Description:   Holds the definition of CICalTimeZone.
*
*/



// User includes
#include "ICalComponent.h"	// CICalComponent

/**
Class representing a VTIMEZONE component.
@internalTechnology
*/
class CICalTimeZone : public CICalComponent
	{
public:	// Construction/destruction.
	static CICalTimeZone* NewLC(TICalMethod aMethod);
	~CICalTimeZone();

private:	// Construction.
	CICalTimeZone();
	void ConstructL(TICalMethod aMethod);

private:	// From CICalBase.
	TBool ValidatePropertyImpl(const TDesC& aName) const;
	TBool ValidateComponent(TICalComponentType aType) const;
	};
	
#endif // ICALTIMEZONE_H

// End of File
