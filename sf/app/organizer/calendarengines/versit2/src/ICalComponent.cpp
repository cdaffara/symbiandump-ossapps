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
* Description:   Implements the definition of CICalComponent. It defines various ICAL components for calendar events.
*
*/



// Class include.
#include "ICalComponent.h"		// CICalComponent

//debug
#include "calendarengines_debug.h"

// User includes
#include "ICalAlarm.h"				// CICalAlarm
#include "ICalEvent.h"				// CICalEvent
#include "ICalFreeBusy.h"			// CICalFreeBusy
#include "ICalJournal.h"			// CICalJournal
#include "ICalKeyWords.h"			// Literals
#include "ICalTimeZone.h"			// CICalTimeZone
#include "ICalTimeZoneInterval.h"	// CICalTimeZoneInterval
#include "ICalTodo.h"				// CICalTodo

/**
Destructor
@internalTechnology
*/
CICalComponent::~CICalComponent()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Factory function for creating new components of the type indicated by the aName
argument.
@param aName The type of component to create
@return The new component.
@leave Leaves with KErrCorrupt if the requested component is unknown.
@publishedPartner
*/
EXPORT_C CICalComponent* CICalComponent::CreateICalComponentL(const TDesC& aName, TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	CICalComponent* self = CreateICalComponentLC(aName, aMethod);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}
	
/**
Factory function for creating new components of the type indicated by the aName
argument. Pushes the new component onto the Cleanup Stack.
@param aName The type of component to create.
@return The new component.
@leave Leaves with KErrCorrupt if the requested component is unknown.
@publishedPartner
*/
EXPORT_C CICalComponent* CICalComponent::CreateICalComponentLC(const TDesC& aName, TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	CICalComponent* self = NULL;

	if (aName.CompareF(KICalEvent) == 0)
		{
		self = CICalEvent::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalTodo) == 0)
		{
		self = CICalTodo::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalJournal) == 0)
		{
		self = CICalJournal::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalAlarm) == 0)
		{
		self = CICalAlarm::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalTimeZone) == 0)
		{
		self = CICalTimeZone::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalFreeBusy) == 0)
		{
		self = CICalFreeBusy::NewLC(aMethod);
		}
	else if (aName.CompareF(KICalStandard) == 0)
		{
		self = CICalTimeZoneInterval::NewLC(EICalStandard, aMethod);
		}
	else if (aName.CompareF(KICalDaylight) == 0)
		{
		self = CICalTimeZoneInterval::NewLC(EICalDaylight, aMethod);
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
    
    TRACE_EXIT_POINT;
	return self;
	}
	
/**
Factory function for creating new components of the type indicated by the aType
argument.
@param aType The type of component to create.
@return The new component.
@leave Leaves with KErrCorrupt if the requested component is unknown.
@publishedPartner
*/
EXPORT_C CICalComponent* CICalComponent::CreateICalComponentL(CICalBase::TICalComponentType aType, TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	CICalComponent* self = CreateICalComponentLC(aType, aMethod);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;	
	}
	
/**
Factory function for creating new components of the type indicated by the aType
argument. Pushes the new component onto the cleanup stack.
@param aType The type of component to create.
@return The new component.
@leave Leaves with KErrCorrupt if the requested component is unknown.
@publishedPartner
*/
EXPORT_C CICalComponent* CICalComponent::CreateICalComponentLC(CICalBase::TICalComponentType aType, TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	CICalComponent* self = NULL;
	
	switch (aType)
		{
		case EICalEvent:
			self = CICalEvent::NewLC(aMethod);
			break;
		case EICalTodo:
			self = CICalTodo::NewLC(aMethod);
			break;
		case EICalJournal:
			self = CICalJournal::NewLC(aMethod);
			break;
		case EICalAlarm:
			self = CICalAlarm::NewLC(aMethod);
			break;
		case EICalFreeBusy:
			self = CICalFreeBusy::NewLC(aMethod);
			break;
		case EICalTimeZone:
			self = CICalTimeZone::NewLC(aMethod);
			break;
		case EICalStandard:
			self = CICalTimeZoneInterval::NewLC(EICalStandard, aMethod);
			break;
		case EICalDaylight:
			self = CICalTimeZoneInterval::NewLC(EICalDaylight, aMethod);
			break;
		default:
			User::Leave(KErrCorrupt);
			break;
		}
    
    TRACE_EXIT_POINT;
	return self;
	}

/**
Constructor.
@internalTechnology
*/
CICalComponent::CICalComponent()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// End of File
