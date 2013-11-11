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
* Description:   Class implementing the definiton of CICal.
*
*/



// Class include.
#include "ICal.h"	// CICal

//debug
#include "calendarengines_debug.h"

// System includes.
#include <s32strm.h>	// RWriteStream

// User includes.
#include "ICalBase.h"				// TICalComponentType
#include "ICalKeyWords.h"			// Literals
#include "ICalContentLineWriter.h"	// CICalContentLineWriter

/**
Static factory construction
@return A new CICal
@publishedPartner
*/
EXPORT_C CICal* CICal::NewL()
	{
	TRACE_ENTRY_POINT;
	
	CICal* self = CICal::NewLC();
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Static factory construction
@return A new CICal
@publishedPartner
*/
EXPORT_C CICal* CICal::NewLC()
	{
	TRACE_ENTRY_POINT;
	
	CICal* self = new (ELeave) CICal;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	TRACE_EXIT_POINT;
	return self;
	}
	
EXPORT_C void CICal::SetMethodL(TICalMethod aMethod)
	{
	TRACE_ENTRY_POINT;
	
	iMethod = aMethod;
	
	TRACE_EXIT_POINT;
	}

/**
Destructor
@internalTechnology
*/
CICal::~CICal()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
	
	

/**
Creates a CICalContentLineWriter from aWriteStream and calls
CICalbase::Externalize with it.
@param aWriteStream The stream to externalize to.
@publishedPartner
*/
EXPORT_C void CICal::ExternalizeL(RWriteStream& aWriteStream)
	{
	TRACE_ENTRY_POINT;
	
	CICalContentLineWriter* writer = CICalContentLineWriter::NewLC(aWriteStream);
	CICalBase::ExternalizeL(*writer);
	CleanupStack::PopAndDestroy(writer);
	
	TRACE_EXIT_POINT;
	}

/**
Constructor
@internalTechnology
*/
CICal::CICal()
	{
	TRACE_ENTRY_POINT;
	
	iComponentType = EICalCalendar;
	
	TRACE_EXIT_POINT;
	}

/**
Internal construction
@internalTechnology
*/
void CICal::ConstructL()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
	
/**
Checks that the iCalendar specification allows this calendar component to
contain the property aName
@param aName the name of the property to validate
@return ETrue if the property is valid for this component, otherwise EFalse
@internalTechnology
*/
TBool CICal::ValidatePropertyImpl(const TDesC& aName) const
	{
	TRACE_ENTRY_POINT;
	
	 if ((aName.CompareF(KICalProdId) == 0) || (aName.CompareF(KICalVersion) == 0) ||
	 	(aName.CompareF(KICalCalscale) == 0) || (aName.CompareF(KICalMethod) == 0))
		{
		//	1	PRODID
		//	1	VERSION
		//	0..1	CALSCALE
		//	0..1	METHOD
        TRACE_EXIT_POINT;
		return (FindProperty(aName) == NULL);
		}

	if ((aName.Length() >= 2) && (aName.Left(2).CompareF(KICalXProperty) == 0))
		{
		//	0..*	X-[anything]
		TRACE_EXIT_POINT;
		return ETrue;
		}

    TRACE_EXIT_POINT;
	// If we got this far we didn't match any of the possible property names
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
TBool CICal::ValidateComponent(TICalComponentType aType) const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	// VCALENDAR can have all components nested except another VCALENDAR or a VALARM, STANDARD or DAYLIGHT.
	// However, we also return false for invalid components.
	return ((aType != EICalCalendar) &&
			(aType != EICalAlarm) &&
			(aType != EICalStandard) &&
			(aType != EICalDaylight) &&
			(aType != EICalInvalid));
	}
	
// End of File
