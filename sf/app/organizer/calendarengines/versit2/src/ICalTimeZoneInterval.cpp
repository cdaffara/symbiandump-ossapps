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
* Description:   Implements the definition of CICalTimeZoneInterval.
*
*/



// Class include.
#include "ICalTimeZoneInterval.h"	// CICalTimeZoneInterval

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalKeyWords.h"			// Literals

/**
Static factory construction
@param aType The type of time zone interval.
@return A new timezone interval component
@leave Leaves with KErrCorrupt if the requested component is invalid.
@internalTechnology
*/
CICalTimeZoneInterval* CICalTimeZoneInterval::NewLC(TICalComponentType aType, TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	if ((aType != EICalDaylight) && (aType != EICalStandard))
		{
		User::Leave(KErrCorrupt);
		}

	CICalTimeZoneInterval* self = new (ELeave) CICalTimeZoneInterval(aType);
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	
    TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor
@internalTechnology
*/
CICalTimeZoneInterval::~CICalTimeZoneInterval()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Constructor
@param aType The type of time zone interval.
@internalTechnology
*/
CICalTimeZoneInterval::CICalTimeZoneInterval(TICalComponentType aType)
	{
	TRACE_ENTRY_POINT;
	
	iComponentType = aType;
	
	TRACE_EXIT_POINT;
	}

/**
Internal construction
@internalTechnology
*/
void CICalTimeZoneInterval::ConstructL(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	iMethod = aMethod;
	iComponentMethodBitMask = EMaskTimezoneIntervalAny;
	
	TRACE_EXIT_POINT;
	}

/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName.
@param aName the name of the property to validate.
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CICalTimeZoneInterval::ValidatePropertyImpl(const TDesC& aName) const
	{
	TRACE_ENTRY_POINT;
	
	if ((aName.CompareF(KICalDtstart) == 0) || (aName.CompareF(KICalTzoffsetto) == 0) ||
		(aName.CompareF(KICalTzoffsetfrom) == 0) || (aName.CompareF(KICalComment) == 0) ||
		(aName.CompareF(KICalRdate) == 0) || (aName.CompareF(KICalRRule) == 0) ||
		(aName.CompareF(KICalTzname) == 0))
		{
		//	1		dtstart / tzoffsetto / tzoffsetfrom
		//	0..1	comment / rdate / rrule / tzname
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
TBool CICalTimeZoneInterval::ValidateComponent(TICalComponentType /*aType*/) const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	// DAYLIGHT or STANDARD components cannot contain other components.
	return EFalse;
	}
	
// End of file


