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
* Description:   Class implementing the definition of CICalAlarm. This class deals with alarm property of calendar events.
*
*/



// Class include.
#include "ICalAlarm.h"	// CICalAlarm

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalKeyWords.h"			// Literals

/**
Static factory construction
@return A new CICalAlarm
@internalTechnology
*/
CICalAlarm* CICalAlarm::NewLC(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	CICalAlarm* self = new (ELeave) CICalAlarm;
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor
@internalTechnology
*/
CICalAlarm::~CICalAlarm()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Constructor
@internalTechnology
*/
CICalAlarm::CICalAlarm()
	{
	TRACE_ENTRY_POINT;
	
	iComponentType = EICalAlarm;
	
	TRACE_EXIT_POINT;
	}

/**
Internal construction
@internalTechnology
*/
void CICalAlarm::ConstructL(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	iMethod = aMethod;
	iComponentMethodBitMask = EMaskAlarmAny;
	
	TRACE_EXIT_POINT;
	}
	
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CICalAlarm::ValidatePropertyImpl(const TDesC& aName) const
	{
	TRACE_ENTRY_POINT;
	
	 if ((aName.CompareF(KICalAction) == 0) || (aName.CompareF(KICalTrigger) == 0) ||
	 	(aName.CompareF(KICalDuration) == 0) || (aName.CompareF(KICalRepeat) == 0) ||
	 	(aName.CompareF(KICalDescription) == 0) || (aName.CompareF(KICalSummary) == 0))
		{
		//	1	ACTION
		//	1	TRIGGER
		//	0..1	DURATION
		//	0..1	REPEAT
		//	0..1	DESCRIPTION
		//	0..1	SUMMARY
		TRACE_EXIT_POINT;
		return (FindProperty(aName) == NULL);
		}
	else if(aName.CompareF(KICalAttendee) == 0 || aName.CompareF(KICalAttach) == 0)
		{
		//	0..*	ATTENDEE
		//	0..*	ATTACH
		TRACE_EXIT_POINT;
		return ETrue;
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
TBool CICalAlarm::ValidateComponent(TICalComponentType /*aType*/) const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	// VALARM cannot contain other components.
	return EFalse;
	}
	
// End of File

