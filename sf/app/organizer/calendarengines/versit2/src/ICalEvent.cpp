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
* Description:   Implements the definition of CICalEvent.
*
*/



// Class include.
#include "ICalEvent.h"	// CICalEvent

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalKeyWords.h"			// Literals

/**
Static factory construction
@return A new event component
@internalTechnology
*/
CICalEvent* CICalEvent::NewLC(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	CICalEvent* self = new (ELeave) CICalEvent;
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	TRACE_EXIT_POINT;
	
	return self;
	}

/**
Destructor
@internalTechnology
*/
CICalEvent::~CICalEvent()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Constructor
@internalTechnology
*/
CICalEvent::CICalEvent()
	{
	TRACE_ENTRY_POINT;
	
	iComponentType = EICalEvent;
	
	TRACE_EXIT_POINT;
	}

/**
Internal construction
@internalTechnology
*/
void CICalEvent::ConstructL(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	iMethod = aMethod;
	switch(iMethod)
		{
		case EMethodNone:
			iComponentMethodBitMask = EMaskEventNone;
			break;
		case EMethodPublish:
			iComponentMethodBitMask = EMaskEventPublish;
			break;
		case EMethodRequest:
			iComponentMethodBitMask = EMaskEventRequest;
			break;
		case EMethodReply:
			iComponentMethodBitMask = EMaskEventReply;
			break;
		case EMethodAdd:
			iComponentMethodBitMask = EMaskEventAdd;
			break;
		case EMethodCancel:
			iComponentMethodBitMask = EMaskEventCancel;
			break;
		case EMethodRefresh:
			iComponentMethodBitMask = EMaskEventRefresh;
			break;
		case EMethodCounter:
			iComponentMethodBitMask = EMaskEventCounter;
			break;
		case EMethodDeclineCounter:
			iComponentMethodBitMask = EMaskEventDeclineCounter;
			break;
		}
	TRACE_EXIT_POINT;
	}
	
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CICalEvent::ValidatePropertyImpl(const TDesC& aName) const
	{
	TRACE_ENTRY_POINT;
	
	if ((aName.CompareF(KICalClass) == 0) || (aName.CompareF(KICalCreated) == 0) ||
		(aName.CompareF(KICalDescription) == 0) || (aName.CompareF(KICalDtstart) == 0) ||
		(aName.CompareF(KICalGeo) == 0) || (aName.CompareF(KICalLastmodified) == 0) ||
		(aName.CompareF(KICalLocation) == 0) || (aName.CompareF(KICalOrganizer) == 0) ||
		(aName.CompareF(KICalPriority) == 0) || (aName.CompareF(KICalDtstamp) == 0) ||
		(aName.CompareF(KICalSequence) == 0) || (aName.CompareF(KICalStatus) == 0) ||
		(aName.CompareF(KICalSummary) == 0) || (aName.CompareF(KICalTransp) == 0) ||
		(aName.CompareF(KICalUid) == 0) || (aName.CompareF(KICalUrl) == 0) ||
		(aName.CompareF(KICalRecurrenceId) == 0))
		{
		//	0..1	CLASS	
		//	0..1	CREATED
		//	0..1	DESCRIPTION
		//	0..1	DTSTART
		//	0..1	GEO
		//	0..1	LAST-MOD
		//	0..1	LOCATION
		//	0..1	ORGANIZER
		//	0..1	PRIORITY
		//	0..1	DTSTAMP
		//	0..1	SEQUENCE
		//	0..1	STATUS
		//	0..1	SUMMARY
		//	0..1	TRANSP
		//	0..1	UID
		//	0..1	URL
		//	0..1	RECURRENCE-ID
		TRACE_EXIT_POINT;
		return (FindProperty(aName) == NULL);
		}
	// Either DTEND or DURATION may exist but *not* both
	else if (aName.CompareF(KICalDtend) == 0)
		{
		//	0..1	DTEND
		if (FindProperty(aName) == NULL)
			{
			TRACE_EXIT_POINT;
			return (FindProperty(KICalDuration) == NULL);
			}
		TRACE_EXIT_POINT;
		return EFalse;
		}
	else if (aName.CompareF(KICalDuration) == 0)
		{
		//	0..1	DURATION
		if (FindProperty(aName) == NULL)
			{
			TRACE_EXIT_POINT;
			return (FindProperty(KICalDtend) == NULL);
			}
        
        TRACE_EXIT_POINT;			
		return EFalse;
		}
	else if ((aName.CompareF(KICalAttach) == 0) || (aName.CompareF(KICalAttendee) == 0) ||
			(aName.CompareF(KICalCategories) == 0) || (aName.CompareF(KICalComment) == 0) ||
			(aName.CompareF(KICalContact) == 0) || (aName.CompareF(KICalExdate) == 0) ||
			(aName.CompareF(KICalExrule) == 0) || (aName.CompareF(KICalRequeststatus) == 0) ||
			(aName.CompareF(KICalRelatedto) == 0) || (aName.CompareF(KICalResources) == 0) ||
			(aName.CompareF(KICalRdate) == 0) || (aName.CompareF(KICalRRule) == 0))
		{
		//	0..*	ATTACH
		//	0..*	ATTENDEE
		//	0..*	CATEGORIES
		//	0..*	COMMENT
		//	0..*	CONTACT
		//	0..*	EXDATE
		//	0..*	EXRULE
		//	0..*	REQUEST-STATUS
		//	0..*	RELATED-TO
		//	0..*	RESOURCES
		//	0..*	RDATE
		//	0..*	RRULE
		TRACE_EXIT_POINT;
		return ETrue;
		}
	else if ((aName.Length() >= 2) && (aName.Left(2).CompareF(KICalXProperty) == 0))
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
TBool CICalEvent::ValidateComponent(TICalComponentType aType) const
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	// VEVENTs can only contain VALARMs.
	return (aType == EICalAlarm);
	}
	
// End of File
