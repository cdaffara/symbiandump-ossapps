#ifndef ICALUTIL_H
#define ICALUTIL_H/*
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
* Description:   Holds the definiton of NICalUtil.
*
*/



// System includes
#include <e32base.h>	// CBase

// Forward declarations.
class CICalProperty;
class CICalPropertyParam;

/**
Utility functions for parsing iCalendars.
@internalTechnology
*/
namespace NICalUtil
	{
	enum TEscaping
		{
		/** Escape characters are dealt with as per the specification.*/
		EEscapeNormal,
		/** Escaped commas constitute value separators.*/
		EEscapeValueSeparators
		};
		
	TInt ExtractSectionsL(const TDesC& aLine, TPtrC& aName, TPtrC& aParameters, TPtrC& aValues);
	void ExtractPropertyL(const TDesC& aParameters, const TDesC& aValues, CICalProperty& aProperty);
	void ExtractParametersL(const TDesC& aParameters, CICalProperty& aProperty);
	void ExtractParameterL(const TDesC& aParameter, CICalPropertyParam& aPropertyParam);
	void ExtractParameterValuesL(const TDesC& aValues, CICalPropertyParam& aPropertyParam);
	void ExtractPropertyValuesL(const TDesC& aValues, CICalProperty& aProperty, TEscaping aEscaping = EEscapeNormal);
	TBool PropertyHasTextValueL(const CICalProperty& aProperty);
	}
	
#endif	// ICALUTIL_H

// End of File
