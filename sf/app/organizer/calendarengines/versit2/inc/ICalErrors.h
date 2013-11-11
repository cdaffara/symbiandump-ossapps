#ifndef ICALERRORS_H
#define ICALERRORS_H/*
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
* Description:   Holds the versit2-specific error codes.
*
*/



/**
Versit2-specific error codes.
@publishedPartner
*/
enum
	{
	/** An invalid subcomponent nesting has occurred.*/
	KErrUnsupportedComponent = -21000,
	/** An attempt to add an invalid property has occurred.*/
	KErrUnsupportedProperty,
	/** An attempt to add a property with no value has occurred.*/
	KErrPropertyHasNoValue,
	/** An attempt to add a property parameter with no value has occurred.*/
	KErrParameterHasNoValue,
	/** An attempt to access the frequency of a rule segment
	that does not hold frequency information has occurred.*/
	KErrRuleTypeNotFrequency,
	/** An attempt to access the frequency of a rule
	segment with an unknown value has occurred.*/
	KErrRuleFrequencyUnknown,
	/** An attempt to add a rule segment with no type has occurred.*/
	KErrInvalidRuleSegment,
	/** An attempt to add a rule segment with no value has occurred.*/
	KErrRuleSegmentHasNoValue
	};

#endif	// ICALERRORS_H

// End of File
