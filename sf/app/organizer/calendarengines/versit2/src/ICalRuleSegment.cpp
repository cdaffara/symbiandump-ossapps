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
* Description:   Implements the definition of CICalRuleSegment.It deals with recurrence rule property of a claendar event.
*
*/



// Class include.
#include "ICalRuleSegment.h"	// CICalRuleSegment

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalKeyWords.h"	// Literals
#include "ICalErrors.h"		// Error codes
#include "ICalValue.h"		// CICalValue

/**
Static factory construction. Not exported as this class is only provided as a
utility for breaking down recurrence rules into segments - it is only ever
created by the CICalValue class.
@param aSource TDesC containing the rule segment information
@return A new rule segment
@internalTechnology
*/
CICalRuleSegment* CICalRuleSegment::NewL(const TDesC& aSource)
	{
	TRACE_ENTRY_POINT;
	
	CICalRuleSegment* self = CICalRuleSegment::NewLC(aSource);
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Static factory construction. Not exported as this class is only provided as a
utility for breaking down recurrence rules into segments - it is only ever
created by the CICalValue class.
@param aSource TDesC containing the rule segment information
@return A new rule segment
@internalTechnology
*/
CICalRuleSegment* CICalRuleSegment::NewLC(const TDesC& aSource)
	{
	TRACE_ENTRY_POINT;
	
	CICalRuleSegment* self = new (ELeave) CICalRuleSegment;
	CleanupStack::PushL(self);
	self->ConstructL(aSource);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor
@internalTechnology
*/
CICalRuleSegment::~CICalRuleSegment()
	{
	TRACE_ENTRY_POINT;
	
	iValues.ResetAndDestroy();
	delete iTypeString;
	}

/**
Access method for the segment type as a descriptor.
@return The segment type as a descriptor or an empty descriptor if no type is
set.
@publishedPartner
*/
EXPORT_C const TDesC& CICalRuleSegment::TypeName() const
	{
	TRACE_ENTRY_POINT;
	
	if (iTypeString)
		{
		TRACE_EXIT_POINT;
		return *iTypeString;
		}
		
	// Else type is not set due to SetTypeL() leaving.
	TRACE_EXIT_POINT;
	return KNullDesC;
	}

/**
Access method for the value array.
@return The value array.
@publishedPartner
*/
EXPORT_C const RPointerArray<CICalValue>& CICalRuleSegment::Values() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iValues;
	}
	
/**
Access method for the segment type as an enumeration.
@return The segment type as an enumeration.
@publishedPartner
*/
EXPORT_C CICalRuleSegment::TSegmentType CICalRuleSegment::Type() const
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	return iType;
	}

/**
Access method for the segment value as a frequency enumeration. Only the first
value is used.
@return The segment value as a frequency.
@leave Leaves with KErrRuleTypeNotFrequency if this rule segment does not
contain a frequency.
@leave Leaves with KErrRuleFrequencyUnknown if the frequency cannot be
determined.
@publishedPartner
*/
EXPORT_C CICalRuleSegment::TFreq CICalRuleSegment::FreqL() const
	{
	TRACE_ENTRY_POINT;
	
	// Type MUST be frequency if this method is called.
	
	if (iType != ESegFreq)
		{
		User::Leave(KErrRuleTypeNotFrequency);
		}
	
	// The rule must have at least one value.
	// If it has more than one then the first value is taken (for recoverability).
	if (iValues.Count() == 0)
		{
		User::Leave(KErrRuleFrequencyUnknown);
		}
	
	// Compare the first value.
	const TDesC& firstValue = iValues[0]->TextL();
	
	if (firstValue.CompareF(KICalYearly) == 0)
		{
		TRACE_EXIT_POINT;
		return EFreqYearly;
		}
	else if (firstValue.CompareF(KICalMonthly) == 0)
		{
		TRACE_EXIT_POINT;
		return EFreqMonthly;
		}
	else if (firstValue.CompareF(KICalWeekly) == 0)
		{
		TRACE_EXIT_POINT;
		return EFreqWeekly;
		}
	else if (firstValue.CompareF(KICalDaily) == 0)
		{
		TRACE_EXIT_POINT;
		return EFreqDaily;
		}
	else if (firstValue.CompareF(KICalHourly) == 0)
		{
		TRACE_EXIT_POINT;
		return EFreqHourly;
		}
	else if (firstValue.CompareF(KICalMinutely) == 0)
		{
		TRACE_EXIT_POINT;
		return EFreqMinutely;
		}
	else if (firstValue.CompareF(KICalSecondly) == 0)
		{
		TRACE_EXIT_POINT;
		return EFreqSecondly;
		}
	
	// Else...
	User::Leave(KErrRuleFrequencyUnknown);
	
	TRACE_EXIT_POINT;
	return EFreqDaily;	// Never reached.
	}
	
/**
Constructor
@internalTechnology
*/
CICalRuleSegment::CICalRuleSegment()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Internal construction.
@param aSource Descriptor containing details of the segment.
@leave Leaves with KErrInvalidRuleSegment if the rule segment has no "type".
@leave Leaves with KErrRuleSegmentHasNoValue if the rule segment has no value.
@leave Leaves if there is an error adding a value to this rule segment.
@internalTechnology
*/
void CICalRuleSegment::ConstructL(const TDesC& aSource)
	{
	TRACE_ENTRY_POINT;
	
	// We've been passed a descriptor containing the type string followed
	// by an equals sign followed by (one or more) comma separated values.
	// All escaping is assumed to have been previously removed.
	
	// Get the position of the equals sign.
	const TInt equalsSignPos = aSource.Locate(KICalEqualsChar);
	
	if (equalsSignPos < 1)	// "Type" must be at least one character long.
		{
		User::Leave(KErrInvalidRuleSegment);
		}

	if ((equalsSignPos + 1) >= aSource.Length())	// Value must be at least one character long.
		{
		User::Leave(KErrRuleSegmentHasNoValue);
		}

	// Any text before this point is the type string.
	SetTypeL(aSource.Left(equalsSignPos));
	
	// Any text after this point contains value information
	TPtrC aRemainder(aSource.Mid(equalsSignPos + 1));
	
	while (aRemainder.Length() > 0)
		{
		// Find the next comma.
		TInt endOfValue(aRemainder.Locate(KICalCommaChar));
		
		if (endOfValue == KErrNotFound)
			{
			endOfValue = aRemainder.Length();
			}
		
		// Add value.
		if (endOfValue > 0)
			{
			CICalValue* val = CICalValue::NewLC();
			val->SetTextL(aRemainder.Left(endOfValue));
			User::LeaveIfError(iValues.Append(val));
			CleanupStack::Pop(val);
			}
		
		// Set the remainder.
		if (endOfValue < (aRemainder.Length() - 1))	// Value must be at least one character long.
			{
			aRemainder.Set(aRemainder.Mid(endOfValue + 1));
			}
		else
			{
			break;
			}
		}
	
	TRACE_EXIT_POINT;
	}

/**
Sets the type of a rule segment.
@param aType The type as a descriptor.
@internalTechnology
*/
void CICalRuleSegment::SetTypeL(const TDesC& aType)
	{
	TRACE_ENTRY_POINT;

	// Set the type.
	
	if (aType.CompareF(KICalFreq) == 0)
		{
		iType = ESegFreq;
		}
	else if (aType.CompareF(KICalUntil) == 0)
		{
		iType = ESegUntil;
		}
	else if (aType.CompareF(KICalCount) == 0)
		{
		iType = ESegCount;
		}
	else if (aType.CompareF(KICalInterval) == 0)
		{
		iType = ESegInterval;
		}
	else if (aType.CompareF(KICalBySecond) == 0)
		{
		iType = ESegBySecond;
		}
	else if (aType.CompareF(KICalByMinute) == 0)
		{
		iType = ESegByMinute;
		}
	else if (aType.CompareF(KICalByHour) == 0)
		{
		iType = ESegByHour;
		}
	else if (aType.CompareF(KICalByDay) == 0)
		{
		iType = ESegByDay;
		}
	else if (aType.CompareF(KICalByMonthDay) == 0)
		{
		iType = ESegByMonthDay;
		}
	else if (aType.CompareF(KICalByYearDay) == 0)
		{
		iType = ESegByYearDay;
		}
	else if (aType.CompareF(KICalByWeekNo) == 0)
		{
		iType = ESegByWeekNo;
		}
	else if (aType.CompareF(KICalByMonth) == 0)
		{
		iType = ESegByMonth;
		}
	else if (aType.CompareF(KICalBySetPos) == 0)
		{
		iType = ESegByPos;
		}
	else if (aType.CompareF(KICalWkSt) == 0)
		{
		iType = ESegWkSt;
		}
	else
		{
		iType = ESegExtension;
		}
	
	// Set the type string.
	delete iTypeString;
	iTypeString = NULL;
	iTypeString = aType.AllocL();
	
	TRACE_EXIT_POINT;
	}

// End of File

