/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements the definition of CICalTimeZone.It deals with decoding time zone property of a calendar event.
*
*/



// Class include.
#include "ICalTimeZone.h"	// CICalTimeZone

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalKeyWords.h"			// Literals

/**
Static factory construction
@return A new CICalTimeZone
@internalTechnology
*/
CICalTimeZone* CICalTimeZone::NewLC(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	CICalTimeZone* self = new (ELeave) CICalTimeZone;
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor
@internalTechnology
*/
CICalTimeZone::~CICalTimeZone()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Constructor
@internalTechnology
*/
CICalTimeZone::CICalTimeZone()
	{
	TRACE_ENTRY_POINT;
	
	iComponentType = EICalTimeZone;
	iComponentMethodBitMask = EMaskTimezoneAny;
	
	TRACE_EXIT_POINT;
	}

/**
Internal construction
@internalTechnology
*/
void CICalTimeZone::ConstructL(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	iMethod = aMethod;
	
	TRACE_EXIT_POINT;
	}
	
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CICalTimeZone::ValidatePropertyImpl(const TDesC& aName) const
	{
	TRACE_ENTRY_POINT;
	
	if ((aName.CompareF(KICalTzid) == 0) || (aName.CompareF(KICalLastmodified) == 0) || (aName.CompareF(KICalTzurl) == 0))
		{
		//	1		TZID
		//	0..1	LAST-MOD
		//	0..1	TZURL
		TRACE_EXIT_POINT;
		return (FindProperty(aName) == NULL);
		}

	if ((aName.Length() >= 2) && (aName.Left(2).CompareF(KICalXProperty) == 0))
		{
		//	0..*	X-[anything]
		TRACE_EXIT_POINT;
		return ETrue;
		}

	// If we got this far we didn't match any of the possible property names
	TRACE_EXIT_POINT;
	return EFalse;	
	}

/**
Checks that the iCalendar specification allows this calendar component to
contain a nested component of type aType.
@param aType the type of component to validate.
@return ETrue if the component is a valid child for this component, otherwise
EFalse.
@internalTechnology
*/
TBool CICalTimeZone::ValidateComponent(TICalComponentType aType) const
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	// VTIMEZONE can only contain STANDARD or DAYLIGHT components.
	return ((aType == EICalDaylight) || (aType == EICalStandard));
	}
	
// End of File

