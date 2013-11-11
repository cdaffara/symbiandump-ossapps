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
* Description:   Implements the definition of CICalParser. It deals with parsing the ICAL received onto the device.
*
*/



// Class include.
#include "ICalParser.h"	// CICalParser

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICal.h"					// CICal
#include "ICalContentLineReader.h"	// CICalContentLineReader
#include "ICalKeyWords.h"			// Literals
#include "ICalUtil.h"				// NICalUtil

using namespace NICalUtil;

/**
Allocates and constructs a new CICalParser.
@return A new CICalParser.
@publishedPartner
*/
EXPORT_C CICalParser* CICalParser::NewL()
	{
	TRACE_ENTRY_POINT;
	
	CICalParser* self = CICalParser::NewLC();
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}
	
/**
Allocates and constructs a new CICalParser.
The pointer to the new object is left on the Cleanup Stack.
@return  A new CICalParser.
@publishedPartner
*/
EXPORT_C CICalParser* CICalParser::NewLC()
	{
	TRACE_ENTRY_POINT;
	
	CICalParser* self = new (ELeave) CICalParser;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor
@internalTechnology
*/
CICalParser::~CICalParser()
	{
	TRACE_ENTRY_POINT;
	
	iCals.ResetAndDestroy();
	TRACE_EXIT_POINT;
	}

/**
Creates a CICalContentLinerReader from aReadStream and parses iCalendar
information.
@param aReadStream The stream to read from.
@publishedPartner
*/
EXPORT_C void CICalParser::InternalizeL(RReadStream& aReadStream)
	{
	TRACE_ENTRY_POINT;
	
	CICalContentLineReader* reader = CICalContentLineReader::NewLC(aReadStream);
	
	TPtrC line;
	TPtrC name;
	TPtrC parameters;
	TPtrC values;
	
	TInt error = KErrNone;
	
	while ((error = reader->GetNextContentLine(line)) == KErrNone)
		{
		if (ExtractSectionsL(line, name, parameters, values) == KErrNone)
			{
			if ((name.CompareF(KICalBegin) == 0) && (values.CompareF(KICalVCalendar) == 0))
				{
				// This is the start of a new iCalendar
				CICal* cal = CICal::NewLC();
				cal->InternalizeL(*reader);
				User::LeaveIfError(iCals.Append(cal));
				CleanupStack::Pop(cal);
				}
				
			// Else not a VCALENDAR - ignore it.
			}
			
		// Else no property value given on the line.
		}
		
	if ((error != KErrEof) && (error != KErrNone))
		{
		User::Leave(error);
		}
		
	CleanupStack::PopAndDestroy(reader);
	TRACE_EXIT_POINT;
	}
	
/**
Get the number of CICal objects owned by this CICalParser.
@return The number of CICal objects.
@publishedPartner
*/
EXPORT_C TInt CICalParser::CalCount() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iCals.Count();
	}
	
/**
Returns the iCalendar at the given index.
@param aIndex The iCalendar required.
@return A modifiable reference to the required iCalendar.
@publishedPartner
*/
EXPORT_C CICal& CICalParser::Cal(TInt aIndex)
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return *iCals[aIndex];
	}
	
/**
Default constructor.
@internalTechnology
*/
CICalParser::CICalParser()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Second phase construction.
@internalTechnology
*/
void CICalParser::ConstructL()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

// End of File
