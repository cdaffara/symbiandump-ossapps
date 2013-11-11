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
* Description:   Implements the definition of CICalJournal. It deals with Journal property of calendar event.
*
*/



// Class include.
#include "ICalJournal.h"	// CICalJournal

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalKeyWords.h"			// Literals

/**
Static factory construction
@return A new CICalJournal
@internalTechnology
*/
CICalJournal* CICalJournal::NewLC(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	CICalJournal* self = new (ELeave) CICalJournal;
	CleanupStack::PushL(self);
	self->ConstructL(aMethod);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor
@internalTechnology
*/
CICalJournal::~CICalJournal()
	{
	TRACE_ENTRY_POINT;
	}

/**
Constructor
@internalTechnology
*/
CICalJournal::CICalJournal()
	{
	TRACE_ENTRY_POINT;
	
	iComponentType = EICalJournal;
	
	TRACE_EXIT_POINT;
	}

/**
Internal construction
@internalTechnology
*/
void CICalJournal::ConstructL(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	iMethod = aMethod;
	switch(iMethod)
		{
		case EMethodNone:
			iComponentMethodBitMask = EMaskJournalNone;
			break;
		case EMethodPublish:
			iComponentMethodBitMask = EMaskJournalPublish;
			break;
		case EMethodAdd:
			iComponentMethodBitMask = EMaskJournalAdd;
			break;
		case EMethodCancel:
			iComponentMethodBitMask = EMaskJournalCancel;
			break;
		default:
			// This is invalid should we leave?
			iComponentMethodBitMask = 0;
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
TBool CICalJournal::ValidatePropertyImpl(const TDesC& aName) const
	{
	TRACE_ENTRY_POINT;
	
	if ((aName.CompareF(KICalClass) == 0) || (aName.CompareF(KICalCreated) == 0) ||
		(aName.CompareF(KICalDescription) == 0) || (aName.CompareF(KICalDtstart) == 0) ||
		(aName.CompareF(KICalDtstamp) == 0) || (aName.CompareF(KICalLastmodified) == 0) ||
		(aName.CompareF(KICalOrganizer) == 0) || (aName.CompareF(KICalRecurrenceId) == 0) ||
		(aName.CompareF(KICalSequence) == 0) || (aName.CompareF(KICalStatus) == 0) ||
		(aName.CompareF(KICalSummary) == 0) || (aName.CompareF(KICalUid) == 0) ||
		(aName.CompareF(KICalUrl) == 0))
		{
		//	0..1	CLASS
		//	0..1	CREATED
		//	0..1	DESCRIPTION
		//	0..1	DTSTART
		//	0..1	DTSTAMP
		//	0..1	LAST-MOD
		//	0..1	ORGANIZER
		//	0..1	RECURRENCE-ID
		//	0..1	SEQUENCE
		//	0..1	STATUS
		//	0..1	SUMMARY
		//	0..1	UID
		//	0..1	URL
		TRACE_EXIT_POINT;
		return (FindProperty(aName) == NULL);
		}
	else if ((aName.CompareF(KICalAttach) == 0) || (aName.CompareF(KICalAttendee) == 0) ||
		(aName.CompareF(KICalCategories) == 0) || (aName.CompareF(KICalComment) == 0) ||
		(aName.CompareF(KICalContact) == 0) || (aName.CompareF(KICalExdate) == 0) ||
		(aName.CompareF(KICalExrule) == 0) || (aName.CompareF(KICalRelatedto) == 0) ||
		(aName.CompareF(KICalRdate) == 0) || (aName.CompareF(KICalRRule) == 0) ||
		(aName.CompareF(KICalRequeststatus) == 0))
		{
		//	0..*	ATTACH
		//	0..*	ATTENDEE
		//	0..*	CATEGORIES
		//	0..*	COMMENT
		//	0..*	CONTACT
		//	0..*	EXDATE
		//	0..*	EXRULE
		//	0..*	RELATED-TO
		//	0..*	RDATE
		//	0..*	RRULE
		//	0..*	REQUEST-STATUS
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
TBool CICalJournal::ValidateComponent(TICalComponentType /*aType*/) const
	{
	TRACE_ENTRY_POINT;
    TRACE_EXIT_POINT;
	// VJOURNAL cannot contain other components.
	return EFalse;
	}

// End of File
