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
* Description:   Implements the definition of CICalValue.
*
*/



// Class include.
#include "ICalValue.h"	// CICalValue

//debug
#include "calendarengines_debug.h"

// User includes.
#include "ICalKeyWords.h"		// Literals
#include "ICalRuleSegment.h"	// CICalRuleSegment
#include "CleanupPointerArray.h"	// For TCleanupPointerArray

// Constants.

// Maximum number of characters to use for storing TInt.
// (The maximum number of characters for storing a decimal representation of
// KMaxTInt, KMinTInt or even KMaxTUint is 10 on current implementations.
// The maximum number of characters for storing a decimal representation of 
// KMaxTUint64 is 20, so using this for future proofing.)
const TInt KICalTIntWidth = 20;

// Width of "HHMMSS"
const TInt KICalTimeWidth = 6;

// Width of "YYYYMMDD"
const TInt KICalDateWidth = 8;

// Width of "YYYYMMDDTHHMMSS" (note the 'T' in the middle)
const TInt KICalDateTimeWidth = KICalDateWidth + KICalTimeWidth + 1;

// Width of a duration.
const TInt KICalDurationWidth = 25;

// Width of a short format UTC offset.
const TInt KShortFormatUtcOffsetWidth = 5;	// The width of (e.g.) +1000.

// Width of a long format UTC offset.
const TInt KLongFormatUtcOffsetWidth = 7;	// The width of (e.g.) -013045.

// Time constants.	
const TInt KICalSecondsPerMinute =     60;
const TInt KICalSecondsPerHour   =   3600; // That is:      60*60;
const TInt KICalSecondsPerDay    =  86400; // That is:   24*60*60;
const TInt KICalSecondsPerWeek   = 604800; // That is: 7*24*60*60;

// Time characters
const TUint KICalUtcChar('Z');		// UTC time.
const TUint KICalTimeChar('T');		// Date/time separator.
const TUint KICalPeriodChar('/');	// Period separator.
const TUint KICalMicroChar('.');	// Second/microsecond separator.
const TUint KICalDurationChar('P');	// Duration.
const TUint KICalWeekChar('W');		// Week.
const TUint KICalDayChar('D');		// Day.
const TUint KICalHourChar('H');		// Hour.
const TUint KICalMinuteChar('M');	// Minute.
const TUint KICalSecondChar('S');	// Second.
const TUint KICalPositiveChar('+');	// Positive values.
const TUint KICalNegativeChar('-');	// Negative values.

// Date format.
_LIT(KICalDateFormat, "%F%Y%M%D");
	
/**
Static factory construction.
@return A new CICalValue.
@publishedPartner
*/
EXPORT_C CICalValue* CICalValue::NewL()
	{
	TRACE_ENTRY_POINT;
	
	CICalValue* self  = CICalValue::NewLC();
	CleanupStack::Pop(self);
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Static factory construction.
@return A new CICalValue.
@publishedPartner
*/
EXPORT_C CICalValue* CICalValue::NewLC()
	{
	TRACE_ENTRY_POINT;
	
	CICalValue* self  = new (ELeave) CICalValue;
	CleanupStack::PushL(self);
	self->ConstructL();
	
	TRACE_EXIT_POINT;
	return self;
	}

/**
Destructor.
@publishedPartner
*/
CICalValue::~CICalValue()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	delete iValue;
	}

/**
Returns a copy of the value as an 8-bit string. User takes ownership of the
returned descriptor, which is also left on the Cleanup Stack. Note that each
16-bit word in the value is copied into a single 8-bit word in the returned
descriptor and any values greater than 256 are set to the value '1'. This will
not occur if the value was set using SetBinaryL().
@return An 8-bit copy of the value.
@publishedPartner
*/
EXPORT_C HBufC8* CICalValue::BinaryLC() const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	HBufC8* buf = HBufC8::NewLC(iValue->Length());
	buf->Des().Copy(*iValue);
	
	TRACE_EXIT_POINT;
	return buf;
	}

/**
Stores an 8-bit buffer in the CICalValue. Each 8-bit value is stored within a
16-bit value internally.
@param aBuffer The 8-bit buffer to be stored.
@publishedPartner
*/
EXPORT_C void CICalValue::SetBinaryL(const TDesC8& aBuffer)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	iValue = HBufC::NewL(aBuffer.Length());
	iValue->Des().Copy(aBuffer);
	}

/**
Returns the CICalValue as a TBool. Leaves if the value cannot be read as a TBool.
@return TBool stored in the value.
@leave KErrCorrupt if value is not a valid Boolean value.
@publishedPartner
*/
EXPORT_C TBool CICalValue::BooleanL() const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	
	if (iValue->Des() == KICalTrue)
		{
		TRACE_EXIT_POINT;
		return ETrue;
		}
	else if (iValue->Des() == KICalFalse)
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}
	
	// Else...
	User::Leave(KErrCorrupt);
	
	TRACE_EXIT_POINT;
	return EFalse;	// Never reached.
	}

/**
Stores a TBool in the CICalValue.
@param aBool TBool to be stored.
@publishedPartner
*/
EXPORT_C void CICalValue::SetBooleanL(TBool aBool)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	
	if (aBool) // ETrue
		{
		iValue = KICalTrue().AllocL();
		}
	else	// EFalse
		{
		iValue = KICalFalse().AllocL();
		}
		
	TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as a date (TTime).
@param aDate TTime to store the date in.
@publishedPartner
*/
EXPORT_C void CICalValue::GetDateL(TTime& aDate) const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	GetDateFromValueL(aDate);
	
	TRACE_EXIT_POINT;
	}

/**
Stores a date (TTime) as the value.
@param aDate TTime to be stored.
@publishedPartner
*/
EXPORT_C void CICalValue::SetDateL(const TTime& aDate)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	iValue = HBufC::NewL(KICalDateWidth);
	AppendDateToValueL(aDate);
	
	TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as a time (TTime).
@param aTime TTime to store the time in.
@param aTzType Enumeration showing whether the time represents local time (not
supported), UTC or a custom time zone.
@publishedPartner
*/
EXPORT_C void CICalValue::GetTimeL(TTime& aTime, TTimeZoneType& aTzType) const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	GetTimeFromValueL(aTime, aTzType);
	
	TRACE_EXIT_POINT;
	}

/**
Stores a time (TTime) as the value.
@param aTime TTime to be stored.
@param aTzType Enumeration showing whether the time represents local time (not
supported), UTC or a custom time zone.
@publishedPartner
*/
EXPORT_C void CICalValue::SetTimeL(const TTime& aTime, TTimeZoneType aTzType)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	
	if (aTzType == EUtcTime)
		{
		iValue = HBufC::NewL(KICalTimeWidth + 1);
		AppendTimeToValueL(aTime);
		iValue->Des().Append(KICalUtcChar);	// Append a 'Z' to signify UTC.
		}
	else if (aTzType == ESpecifiedTimeZone || aTzType == EFloatingTime)
		{
		iValue = HBufC::NewL(KICalTimeWidth);
		AppendTimeToValueL(aTime);
		}
	
	TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as a date/time (TTime).
@param aDateTime TTime to store the date/time in.
@param aTzType Enumeration showing whether the time represents local time (not
supported), UTC or a custom time zone.
@param aFirstCharacterNum Skip this many characters of the internal buffer
@leave Leaves with KErrCorrupt if the value is not a valid date/time.
@publishedPartner
*/
EXPORT_C void CICalValue::GetDateTimeL(TTime& aDateTime, TTimeZoneType& aTzType, TInt aFirstCharacterNum) const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	
	// The GetTimeFromValueL() function explicitly sets the TTime passed
	// in, so create a new TTime for the date and merge with the time later.
	TTime theDate;
	GetDateFromValueL(theDate, aFirstCharacterNum);
	
	// Only start looking for the time at character position KICalDateWidth
	// plus one for the 'T' separator.
	if (iValue->Length() > aFirstCharacterNum + KICalDateWidth + 1)
		{
		TTime theTime;
		if ((*iValue)[aFirstCharacterNum + KICalDateWidth] != KICalDateTimeSeparator)
			{
			User::Leave(KErrCorrupt);
			}

		GetTimeFromValueL(theTime, aTzType, aFirstCharacterNum + KICalDateWidth + 1);

		// Set the time component.
		TDateTime dateValues(theDate.DateTime());
		TDateTime combinedValues(theTime.DateTime());
		
		// Set the date component.
		combinedValues.SetYear(dateValues.Year());
		combinedValues.SetMonth(dateValues.Month());
		combinedValues.SetDay(dateValues.Day());
		
		// Set the value to return.
		aDateTime = TTime(combinedValues);
		}
	else
		{
		// There is no time component specified.  For compatibility with MS Outlook,
		// which can export RECURRENCE-ID fields with no time component, we use
		// a default time of '00:00'
		aDateTime = theDate;
		}
	
	TRACE_EXIT_POINT;
	}

/**
Stores a date/time (TTime) as the value.
@param aDateTime TTime to be stored.
@param aTzType Enumeration showing whether the time represents floating time,
UTC or a custom time zone.
@publishedPartner
*/
EXPORT_C void CICalValue::SetDateTimeL(const TTime& aDateTime, TTimeZoneType aTzType)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	
	if (aTzType == EUtcTime)
		{
		iValue = HBufC::NewL(KICalDateTimeWidth + 1);
		AppendDateToValueL(aDateTime);
		
		iValue->Des().Append(KICalTimeChar);
		AppendTimeToValueL(aDateTime);
		
		iValue->Des().Append(KICalUtcChar);	// For UTC Time.
		}
	else if (aTzType == ESpecifiedTimeZone || aTzType == EFloatingTime)
		{
		iValue = HBufC::NewL(KICalDateTimeWidth);
		AppendDateToValueL(aDateTime);
		
		iValue->Des().Append(KICalTimeChar);
		AppendTimeToValueL(aDateTime);
		}
	
	TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as a Duration (TTimeIntervalSeconds).
@return TTimeInterval containing the value's duration.
@publishedPartner
*/
EXPORT_C TTimeIntervalSeconds CICalValue::DurationL() const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	TTimeIntervalSeconds theTis;
	GetTimeIntervalFromValueL(theTis);
	
    TRACE_EXIT_POINT;
	return theTis;
	}

/**
Stores a Duration (TTimeIntervalSeconds) as the value.
@param aDuration TTimeIntervalSeconds to be stored.
@publishedPartner
*/
EXPORT_C void CICalValue::SetDurationL(TTimeIntervalSeconds aDuration)
	{
	TRACE_ENTRY_POINT;
	
	// e.g. P15DT5H0M20S
	PrepareValuePointer();
	
	iValue = HBufC::NewL(KICalDurationWidth);
	TInt durInt(aDuration.Int());
	
	if (durInt < 0)
		{
		// First character is a '-' for negative values
		iValue->Des().Append(KICalNegativeChar);
		
		// Now we've set the sign, change the durInt to +ve
		durInt =- durInt;
		}
		
	iValue->Des().Append(KICalDurationChar);
	
	// Add day portion.
	TInt numDays(durInt / KICalSecondsPerDay);
	durInt -= (numDays * KICalSecondsPerDay);
	iValue->Des().AppendNum(numDays);
	iValue->Des().Append(KICalDayChar);
	iValue->Des().Append(KICalTimeChar);
	
	// Add hour portion.
	TInt numHours(durInt / KICalSecondsPerHour);
	durInt -= (numHours * KICalSecondsPerHour);
	iValue->Des().AppendNum(numHours);
	iValue->Des().Append(KICalHourChar);
	
	// Add minute portion.
	TInt numMinutes(durInt / KICalSecondsPerMinute);
	durInt -= (numMinutes * KICalSecondsPerMinute);
	iValue->Des().AppendNum(numMinutes);
	iValue->Des().Append(KICalMinuteChar);
	
	// Add second portion.
	TInt numSeconds(durInt);
	iValue->Des().AppendNum(numSeconds);
	iValue->Des().Append(KICalSecondChar);
	
    TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as a Float (TReal).
@param aFloat The value to return.
@leave Leaves if there is an error reading a float.
@publishedPartner
*/
EXPORT_C void CICalValue::GetFloatL(TReal& aFloat) const
	{
    TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	TLex stringLex(iValue->Des());
	User::LeaveIfError(stringLex.Val(aFloat));
	
	TRACE_EXIT_POINT;
	}

/**
Stores a float (TReal) as the value.
@param aFloat The value to store.
@publishedPartner
*/
EXPORT_C void CICalValue::SetFloatL(const TReal& aFloat)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();

	iValue = HBufC::NewL(KDefaultRealWidth);
	TRealFormat format;
	iValue->Des().Num(aFloat, format);
	
	TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as an Integer.
@return The integer requested.
@publishedPartner
*/
EXPORT_C TInt CICalValue::IntegerL() const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	
	TRACE_EXIT_POINT;
	return ReadIntL(iValue->Des());
	}

/**
Stores an Integer as the value.
@param aInt The integer to be stored.
@publishedPartner
*/
EXPORT_C void CICalValue::SetIntegerL(TInt aInt)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	iValue = HBufC::NewL(KICalTIntWidth);
	iValue->Des().Num(aInt);
	
	TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as a Period (two TTimes).
@param aStartTime TTime marking the beginning of the period.
@param aStartTzType Enumeration showing whether the start time represents local
time (not supported), UTC or a custom time zone.
@param aEndTime TTime marking the end of the period.
@param aEndTzType Enumeration showing whether the end time represents local
time (not supported), UTC or a custom time zone.
@publishedPartner
*/
EXPORT_C void CICalValue::GetPeriodL(
	TTime& aStartTime, 
	TTimeZoneType& aStartTzType, 
	TTime& aEndTime, 
	TTimeZoneType& aEndTzType) const
	{
	TRACE_ENTRY_POINT;
	
	// Example :   19970101T180000Z/19970102T070000Z
	// that is : yyyymmddThhmmss[Z]/yyyymmddThhmmss[Z]
	
	// Example2:   19970101T180000Z/PT5H30M
	// that is2: yyyymmddThhmmss[Z]/[+/-]P[lots of stuff]
	
	// As the first part is always a date-time, get this out to start off with
	GetDateTimeL(aStartTime, aStartTzType, 0);
	
	// Next we need to get the position of the separator.
	TInt charNumber(iValue->Des().Locate(KICalPeriodChar));
	User::LeaveIfError(charNumber);
	
	// If the character after the separator is a 'P', '+' or '-' the second part is a duration.
	++charNumber;
	if (charNumber >= iValue->Length())
		{
		User::Leave(KErrCorrupt);
		}
		
	TChar theChar(iValue->Des()[charNumber]);
	
	if ((theChar == KICalDurationChar) || 
		(theChar == KICalPositiveChar) || 
		(theChar == KICalNegativeChar))
		{
		// Call a function to change the duration into a TTime (date + time)
		TTimeIntervalSeconds interval;
		GetTimeIntervalFromValueL(interval, charNumber);
		
		// Convert from this TimeInterval to a TTime
		aEndTime = aStartTime;
		aEndTime += interval;
		
		// The timezone will be the same as the first.
		aEndTzType = aStartTzType;
		}
	else // Presume it's a date-time
		{
		GetDateTimeL(aEndTime, aEndTzType, charNumber);
		}
	
	TRACE_EXIT_POINT;
	}

/**
Stores a Period (two TTimes) as the value.
@param aStartTime TTime to be stored containing the beginning of the period.
@param aStartTzType Enumeration showing whether the start time represents local
time (not supported), UTC or a custom time zone.
@param aEndTime TTime to be stored containing the end of the period.
@param aEndTzType Enumeration showing whether the end time represents local
time (not supported), UTC or a custom time zone.
@publishedPartner
*/
EXPORT_C void CICalValue::SetPeriodL(
	const TTime& aStartTime, 
	TTimeZoneType aStartTzType, 
	const TTime& aEndTime, 
	TTimeZoneType aEndTzType)
	{
	TRACE_ENTRY_POINT;
	
	// Example:   19970101T180000Z/19970102T070000Z
	// that is: yyyymmddThhmmss[Z]/yyyymmddThhmmss[Z]
	
	PrepareValuePointer();

	SetDateTimeL(aStartTime, aStartTzType);
	
	if (aEndTzType == EUtcTime)
		{
		iValue->ReAllocL(iValue->Length() + 2 + KICalDateTimeWidth);
		iValue->Des().Append(KICalPeriodChar);
		
		AppendDateToValueL(aEndTime);
		iValue->Des().Append(KICalTimeChar);
		
		AppendTimeToValueL(aEndTime);
		iValue->Des().Append(KICalUtcChar);
		}
	else if (aEndTzType == ESpecifiedTimeZone || aEndTzType == EFloatingTime)
		{
		iValue->ReAllocL(iValue->Length() + 1 + KICalDateTimeWidth);
		
		iValue->Des().Append(KICalPeriodChar);
		AppendDateToValueL(aEndTime);
		
		iValue->Des().Append(KICalTimeChar);
		AppendTimeToValueL(aEndTime);
		}
	
	TRACE_EXIT_POINT;
	}

/**
Returns the value as an enumerated day and position.
@param aDay A modifiable reference to an enumerated day.
@param aPos A modifiable reference to a position.
@leave KErrCorrupt if the day part is invalid.
@publishedPartner
*/
EXPORT_C void CICalValue::GetDayL(TDay& aDay, TInt& aPos) const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	
	// Find the end of the numeric part.
	_LIT(KICalDayNumeric, "-+0123456789");
	TInt endNumeric(0);
	
	while (endNumeric < iValue->Length())
		{
		if (KICalDayNumeric().Locate((*iValue)[endNumeric]) == KErrNotFound)
			{
			break;
			}
			
		++endNumeric;
		}
	
	// Set the numeric part.	
	if (endNumeric != 0)
		{
		aPos = ReadIntL(iValue->Left(endNumeric));
		}
	else
		{
		aPos = 0;
		}
	
	// Set the day part.
	TPtrC dayStr(iValue->Mid(endNumeric));
			
	if (dayStr.CompareF(KICalMonday) == 0)
		{
		aDay = EMonday;
		}
	else if (dayStr.CompareF(KICalTuesday) == 0)
		{
		aDay = ETuesday;
		}
	else if (dayStr.CompareF(KICalWednesday) == 0)
		{
		aDay = EWednesday;
		}
	else if (dayStr.CompareF(KICalThursday) == 0)
		{
		aDay = EThursday;
		}
	else if (dayStr.CompareF(KICalFriday) == 0)
		{
		aDay = EFriday;
		}
	else if (dayStr.CompareF(KICalSaturday) == 0)
		{
		aDay = ESaturday;
		}
	else if (dayStr.CompareF(KICalSunday) == 0)
		{
		aDay = ESunday;
		}
	else
		{
		User::Leave(KErrCorrupt);	// Invalid day.
		}
		
	TRACE_EXIT_POINT;
	}
	
/**
Sets the value from an enumerated date and position.
@param aDay The day to set.
@param aPos The position to set.
@leave Leaves with KErrUnknown if the value of aDay is not known.
@publishedPartner
*/
EXPORT_C void CICalValue::SetDayL(TDay aDay, TInt aPos)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	
	// Get the day as a descriptor.
	TPtrC dayName;
	
	switch (aDay)
		{
		case EMonday:
			dayName.Set(KICalMonday());
			break;
		case ETuesday:
			dayName.Set(KICalTuesday());
			break;
		case EWednesday:
			dayName.Set(KICalWednesday());
			break;
		case EThursday:
			dayName.Set(KICalThursday());
			break;
		case EFriday:
			dayName.Set(KICalFriday());
			break;
		case ESaturday:
			dayName.Set(KICalSaturday());
			break;
		case ESunday:
			dayName.Set(KICalSunday());
			break;
		default:
			User::Leave(KErrUnknown);
			break;
		}
		
	// We need space for a number and a day name.
	iValue = HBufC::NewL(KICalTIntWidth + dayName.Length());
	iValue->Des().AppendNum(aPos);
	iValue->Des().Append(dayName);
	
	TRACE_EXIT_POINT;
	}

/**
Gets the value as a month.
@return The month set in the value.
@leave Leaves with KErrCorrupt if the value is not a valid month.
@publishedPartner
*/
EXPORT_C TMonth CICalValue::MonthL() const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	
	TInt num(IntegerL());
	
	if ((num < 1) || (num > 12))
		{
		User::Leave(KErrCorrupt);
		}

    TRACE_EXIT_POINT;
	return static_cast<TMonth>(num - 1);
	}

/**
Sets a month as the value.
@param aMonth The month to set.
@publishedPartner
*/
EXPORT_C void CICalValue::SetMonthL(TMonth aMonth)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	SetIntegerL(aMonth + 1);
	
    TRACE_EXIT_POINT;
	}
	
/**
Returns the value as an array of CICalRuleSegments. Assumes that all escaping
has been previously removed.
@param aRuleSegmentArray The array that the segments are appended to.
@leave Leaves if there is an error adding a rule segment to the array.
@publishedPartner
*/
EXPORT_C void CICalValue::GetRecurrenceRuleL(RPointerArray<CICalRuleSegment>& aRuleSegmentArray) const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	
	// Find the first semicolon:
	TInt nextSemiColon(iValue->Locate(KICalSemiColonChar));

	if (nextSemiColon == KErrNotFound)
		{
		// This means there is only one segment, pretend the semicolon exists at the end of the buffer
		nextSemiColon = iValue->Length();
		}
		
	TUint charNum(0);
	CleanupPointerArrayPushL(aRuleSegmentArray);
	do
		{
		// Create a rule segment from characters charNum to nextSemiColon
		CICalRuleSegment* theRule = CICalRuleSegment::NewLC(iValue->Mid(charNum, nextSemiColon - charNum));
		User::LeaveIfError(aRuleSegmentArray.Append(theRule));	// Takes ownership.
		CleanupStack::Pop(theRule);
		
		// Put the character marker past the current semicolon
		charNum = nextSemiColon + 1;
		
		// We only want to check for any more semicolons in the area to the right of charNum.
		// First check if there are ANY characters to the right of charNum
		if (charNum < iValue->Length())
			{
			// Find the location of the next semi colon in this area.
			nextSemiColon = iValue->Right(iValue->Length() - charNum).Locate(KICalSemiColonChar);
			
			if (nextSemiColon != KErrNotFound)
				{
				// Set the semicolon to be in it's correct position, shifted to take into account the fact
				// that we were only looking in the right part of the original descriptor.
				nextSemiColon += charNum;
				}
			else
				{
				// There are no more semicolons, read in the last value and then exit the loop
				nextSemiColon = iValue->Length();
				}
			}
		}
	while (charNum < iValue->Length());
	
	CleanupStack::Pop(&aRuleSegmentArray);
	TRACE_EXIT_POINT;
	}

/**
Sets a complete RRULE as a descriptor value. Note that this is no different
than setting as text and is only supplied for symmetry.
@param aRuleValue The descriptor containing the complete RRULE.
@publishedPartner
*/
EXPORT_C void CICalValue::SetRecurrenceRuleL(const TDesC& aRuleValue)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	iValue = aRuleValue.AllocL();
	
	TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as text.
@return The value as text.
@publishedPartner
*/
EXPORT_C const TDesC& CICalValue::TextL() const
	{
	TRACE_ENTRY_POINT;
	
	CheckNullValueL();
	
	TRACE_EXIT_POINT;
	return *iValue;
	}

/**
Stores text as the value.
@param aText The text to be stored.
@publishedPartner
*/
EXPORT_C void CICalValue::SetTextL(const TDesC& aText)
	{
	TRACE_ENTRY_POINT;
	
	PrepareValuePointer();
	iValue = aText.AllocL();
	
	TRACE_EXIT_POINT;
	}

/**
Returns the CICalValue as a UTC Offset (TTimeIntervalSeconds).
@return TTimeIntervalSeconds containing the offset.
@leave Leaves with KErrCorrupt if the value is invalid.
@publishedPartner
*/
EXPORT_C TTimeIntervalSeconds CICalValue::UtcOffsetL() const
	{
	TRACE_ENTRY_POINT;
	
	// Format is (e.g.) +1000 (10 hours ahead) or -013045 (1.5 hours and 45 seconds behind).
	CheckNullValueL();

	const TInt length = iValue->Length();
	
	if ((length != KLongFormatUtcOffsetWidth) && (length != KShortFormatUtcOffsetWidth))
		{
		// Invalid UTC Offset - we can't really test more than this, so the output may be garbage!
		User::Leave(KErrCorrupt);
		}
		
	// Get the value of the hour component.
	TInt hours(ReadIntL(iValue->Des().Left(3)));	// '+' or '-' plus the hours component.

	// Get the value of the minute component.
	TInt minutes(ReadIntL(iValue->Des().Mid(3, 2)));	// The minutes component.
	
	// Get the value of the (optional) second component.
	TInt seconds(0);
	
	if (length == KLongFormatUtcOffsetWidth)	// Long format.
		{
		seconds = ReadIntL(iValue->Des().Mid(5, 2));	// The second component.
		}
		
	// Convert to TTimeIntervalSeconds.
	TRACE_EXIT_POINT;
	return TTimeIntervalSeconds((hours * KICalSecondsPerHour) + (minutes * KICalSecondsPerMinute) + seconds);
	}

/**
Stores a UTC Offset (TTimeIntervalSeconds) as the value.
@param aOffset TTimeInterval to set as the offset.
@publishedPartner
*/
EXPORT_C void CICalValue::SetUtcOffsetL(TTimeIntervalSeconds aOffset)
	{
	TRACE_ENTRY_POINT;
	
	// Format is (e.g.) +1000 (10 hours ahead) or -013045 (1.5 hours and 45 seconds behind).
	PrepareValuePointer();
	
	// Create a buffer long enough to hold the widest format.
	iValue = HBufC::NewL(KLongFormatUtcOffsetWidth);

	// Get the offset as an integer.
	TInt offset(aOffset.Int());

	// Set the sign.	
	if (offset < 0)
		{
		iValue->Des().Append(KICalNegativeChar);
		
		// Set offset to positive for calculations.
		offset =- offset;
		}
	else
		{
		iValue->Des().Append(KICalPositiveChar);
		}
		
	// Add hour portion.
	TInt numHours(offset / KICalSecondsPerHour);
	offset -= (numHours * KICalSecondsPerHour);
	iValue->Des().AppendNumFixedWidth(numHours, EDecimal, 2);
	
	// Add minute portion.
	TInt numMinutes(offset / KICalSecondsPerMinute);
	offset -= (numMinutes * KICalSecondsPerMinute);
	iValue->Des().AppendNumFixedWidth(numMinutes, EDecimal, 2);
	
	// Add second portion if necessary.
	if (offset > 0)
		{
		iValue->Des().AppendNumFixedWidth(offset, EDecimal, 2);
		}
		
	TRACE_EXIT_POINT;
	}

/**
Constructor.
@internalTechnology
*/
CICalValue::CICalValue()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}
	
/**
Internal construction.
@internalTechnology
*/
void CICalValue::ConstructL()
	{
	TRACE_ENTRY_POINT;
	TRACE_EXIT_POINT;
	}

/**
Checks for a NULL value. Should be called before accessing iValue. 
@leave Leaves with KErrCorrupt if the value is NULL.
@internalTechnology
*/
void CICalValue::CheckNullValueL() const
	{
	TRACE_ENTRY_POINT;
	
	if (!iValue)
		{
		User::Leave(KErrCorrupt);
		}
	
	TRACE_EXIT_POINT;
	}

/**
Prepares iValue pointer for writing. Deletes existing pointer if it exists.
@internalTechnology
*/
void CICalValue::PrepareValuePointer()
	{
	TRACE_ENTRY_POINT;
	
	if (iValue)
		{
		delete iValue;
		iValue = NULL;
		}
		
	TRACE_EXIT_POINT;
	}
	
/**
Reads a TInt from the given descriptor.
@param aDes The descriptor to convert
@return The integer conversion.
@leave Leaves if there is an error reading an integer.
@internalTechnology
*/
TInt CICalValue::ReadIntL(const TDesC& aDes) const
	{
	TRACE_ENTRY_POINT;
	
	TLex stringLex(aDes);
	TInt returnValue(0);
	User::LeaveIfError(stringLex.Val(returnValue));
	
	TRACE_EXIT_POINT;
	return returnValue;
	}
	
/**
Appends the date to iValue. Member iValue must be initialised before calling.
@param aDate TTime to append to the value.
@internalTechnology
*/
void CICalValue::AppendDateToValueL(const TTime& aDate)
	{
	TRACE_ENTRY_POINT;
	
	TBuf<KICalDateWidth> formattedDate;
	aDate.FormatL(formattedDate, KICalDateFormat);
	iValue->Des().Append(formattedDate);
	
	TRACE_EXIT_POINT;
	}

/**
Gets the date from iValue descriptor. Should be in the format YYYYMMDD
@param aDate A modifiable reference returning a date.
@param aFirstCharacterNum Skip this many characters at the start of the value.
@leave Leaves with KErrCorrupt if the format of the value is not a valid date.
@internalTechnology
*/
void CICalValue::GetDateFromValueL(TTime& aDate, TInt aFirstCharacterNum) const
	{
	TRACE_ENTRY_POINT;
	
	if (aFirstCharacterNum + KICalDateWidth > iValue->Length())
		{
		User::Leave(KErrCorrupt);
		}
	
	// Get the year as an int.
	TInt year(ReadIntL(iValue->Des().Mid(aFirstCharacterNum, 4)));	// YYYYmmdd
	
	if (year < 0)	// All positive years are valid!
		{
		User::Leave(KErrCorrupt);
		}

	// Get the month as an int.
	TInt month(ReadIntL(iValue->Mid(aFirstCharacterNum + 4, 2)));	// yyyMMdd
	
	if ((month < 1) || (month > 12))
		{
		User::Leave(KErrCorrupt);
		}
		
	// Get the day as an int.
	TInt day(ReadIntL(iValue->Mid(aFirstCharacterNum + 6, 2)));	// yyyymmDD

	if ((day < 1) || (day > Time::DaysInMonth(year, static_cast<TMonth>(month - 1))))	// Zero-offset month.
		{
		User::Leave(KErrCorrupt);
		}
		
	// Set the date from its component parts.
	aDate = 0;
	aDate += TTimeIntervalYears(year);
	aDate += TTimeIntervalMonths(month - 1);	// Zero-offset.
	aDate += TTimeIntervalDays(day - 1);		// Zero-offset.
	
	TRACE_EXIT_POINT;
	}

/**
Appends the time to iValue. Member iValue must be initialised before calling.
@param aTime TTime to append to the value.
@internalTechnology
*/
void CICalValue::AppendTimeToValueL(const TTime& aTime)
	{
	TRACE_ENTRY_POINT;
	
	// Format is HHMMSS followed optionally by a Z for UTC time.
	// Note that the 'Z' is not appended here.
	TDateTime fullTime(aTime.DateTime());
	
	iValue->Des().AppendNumFixedWidth(fullTime.Hour(), EDecimal, 2);
	iValue->Des().AppendNumFixedWidth(fullTime.Minute(), EDecimal, 2);
	iValue->Des().AppendNumFixedWidth(fullTime.Second(), EDecimal, 2);
	
	TRACE_EXIT_POINT;
	}
	
/**
Gets the time from iValue descriptor, starting from a pre-determined point.
Should be in the format HHMMSS[Z]
@param aTime TTime to store result of read in
@param aTzType Enumeration showing whether the time represents local time (not
supported), UTC or a custom time zone.
@param aFirstCharacterNum The character number to start searching from.
@leave Leaves with KErrCorrupt if the value does not hold a valid time.
@internalTechnology
*/
void CICalValue::GetTimeFromValueL(TTime& aTime, TTimeZoneType& aTzType, TInt aFirstCharacterNum) const
	{
	TRACE_ENTRY_POINT;
	
	if (aFirstCharacterNum + KICalTimeWidth > iValue->Length())
		{
		User::Leave(KErrCorrupt);
		}

	// Create a new descriptor containing just the first KICalTimeWidth characters of iValue
	// It's size is one larger as we will need to add a full stop (see below)
	HBufC* formattedTime = HBufC::NewLC(iValue->Des().Mid(aFirstCharacterNum, KICalTimeWidth).Length() + 1);
	*formattedTime = iValue->Des().Mid(aFirstCharacterNum, KICalTimeWidth);
	
	// The only formatting needed is to append a full stop
	formattedTime->Des().Append(KICalMicroChar);
	
	if (aTime.Set(*formattedTime))
		{
		User::Leave(KErrCorrupt);
		}
	
	if ((iValue->Length() > (KICalTimeWidth + aFirstCharacterNum)) && 
		(iValue->Des()[KICalTimeWidth + aFirstCharacterNum] == KICalUtcChar))
		{
		aTzType = EUtcTime;
		}
	else
		{//DAVIDTODO:
		// If there is no 'Z' character then assume that there
		// is a specified time zone - EFloatingTime is not used.
		aTzType = ESpecifiedTimeZone;
		}
	
	CleanupStack::PopAndDestroy(formattedTime);
	
	TRACE_EXIT_POINT;
	}

/**
Retrieves a time interval stored as a duration as per the RFC 2445
specification.
@param aTimeInterval TTimeIntervalSeconds to store the result in.
@param aFirstCharacterNum The position in the value containing the first
character of the duration.
@leave Leaves with KErrCorrupt if the value does not hold a valid interval.
@internalTechnology
*/
void CICalValue::GetTimeIntervalFromValueL(TTimeIntervalSeconds& aTimeInterval, TInt aFirstCharacterNum) const
	{
	TRACE_ENTRY_POINT;
	
	//     dur-value  = (["+"] / "-") "P" (dur-date / dur-time / dur-week)
	//     dur-date   = dur-day [dur-time]
	//     dur-time   = "T" (dur-hour / dur-minute / dur-second)
	//     dur-week   = 1*DIGIT "W"
	//     dur-hour   = 1*DIGIT "H" [dur-minute]
	//     dur-minute = 1*DIGIT "M" [dur-second]
	//     dur-second = 1*DIGIT "S"
	//     dur-day    = 1*DIGIT "D"

	// Example: A duration of 15 days, 5 hours and 20 seconds would be:
	//     P15DT5H0M20S
	// A duration of 7 weeks would be:
	//     P7W
	
	TInt intervalMultiplier(1); // This variable sets the interval to be positive or negative.

	// Length should be at least two.
	if ((aFirstCharacterNum >= iValue->Length()) || (iValue->Length() < 2 + aFirstCharacterNum))
		{
		User::Leave(KErrCorrupt);
		}

	// Check that the first character is a 'P', '+' or '-' and adjust the interval accordingly.
	TChar firstCharacter(iValue->Des()[aFirstCharacterNum]);
	
	if (firstCharacter == KICalDurationChar)
		{
		intervalMultiplier = 1;
		aFirstCharacterNum += 1; // Skip the "P"
		}
	else if (firstCharacter == KICalPositiveChar)
		{
		intervalMultiplier = 1;
		aFirstCharacterNum += 2; // Skip the "+P"
		}
	else if (firstCharacter == KICalNegativeChar)
		{
		intervalMultiplier = -1;
		aFirstCharacterNum += 2; // Skip the "-P"
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	
	// It can only contain a dur-date, or a dur-time, or a dur-week
	TInt theInterval(0);
	if (!GetDurDateL(theInterval, aFirstCharacterNum))
		{
		if (!GetDurTimeL(theInterval, aFirstCharacterNum))
			{
			if (!GetDurWeekL(theInterval, aFirstCharacterNum))
				{
				User::Leave(KErrCorrupt);
				}
			}
		}
		
	theInterval *= intervalMultiplier;
	
	// Now we've done the multiply we can convert it into a TTimeIntervalSeconds
	aTimeInterval = theInterval;
	TRACE_EXIT_POINT;
	}

/**
Reads in and converts a dur-day into a number of seconds
@param aIntervalSecs Function adds number of seconds to this variable.
@param aCurrentCharNumber Character number to start looking in the value.
@return ETrue if the value could be interpreted as a dur-date, EFalse otherwise.
@internalTechnology
*/
TBool CICalValue::GetDurDateL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const
	{
	TRACE_ENTRY_POINT;
	
	//	dur-date   = dur-day [dur-time]
	//	dur-day    = 1*DIGIT "D"
	
	if (aCurrentCharNumber >= iValue->Length())
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}
		
	// Create a new TPtrC containing the data from iValue which we need.
	TPtrC data(iValue->Mid(aCurrentCharNumber));
	
	// Find the next 'D' in the descriptor.
	TInt nextDPos(data.Locate(KICalDayChar));
	
	// Check it exists
	if (nextDPos != KErrNotFound)
		{
		// If character found, construct a number from currentCharNumber to nextDPos
		TInt theNum(ReadIntL(data.Left(nextDPos)));

		// Convert this time from days to seconds
		theNum *= KICalSecondsPerDay;
		aIntervalSecs += theNum;
		aCurrentCharNumber += nextDPos + 1; // Extra increment to get past the 'D'
		
		// Check if a dur-time follows. It's optional
		// so we can ignore it's return value.
		GetDurTimeL(aIntervalSecs, aCurrentCharNumber);
		TRACE_EXIT_POINT;
		return ETrue;
		}

	// If no character found, return EFalse
	TRACE_EXIT_POINT;
	return EFalse;
	}

/**
Reads in and converts a dur-time into a number of seconds
@param aIntervalSecs Function adds number of seconds to this variable.
@param aCurrentCharNumber Character number to start looking in the value.
@return ETrue if the value could be interpreted as a dur-time, EFalse otherwise.
@leave Leaves with KErrCorrupt if the value does not hold a valid time.
@internalTechnology
*/
TBool CICalValue::GetDurTimeL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const
	{
	TRACE_ENTRY_POINT;
	
	//	dur-time   = "T" (dur-hour / dur-minute / dur-second) e.g. T5H0M20S

	if (aCurrentCharNumber >= iValue->Length())
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}

	if (iValue->Des()[aCurrentCharNumber] == KICalTimeChar)
		{
		++aCurrentCharNumber; // Increment to get past 'T'
		
		if (!GetDurHourL(aIntervalSecs, aCurrentCharNumber))
			{
			if (!GetDurMinuteL(aIntervalSecs, aCurrentCharNumber))
				{
				if (!GetDurSecondL(aIntervalSecs, aCurrentCharNumber))
					{
					// We should not have read a 'T' and failed to match hour/minute/second
					User::Leave(KErrCorrupt);
					}
				}
			}
		}
	else
		{
		// First character is not a 'T', therefore value is not a dur-time
		TRACE_EXIT_POINT;
		return EFalse;
		}
		
	TRACE_EXIT_POINT;
	return ETrue;
	}
	
/**
Reads in and converts a dur-hour into a number of seconds. There is no range
check on the number of hours.
@param aIntervalSecs Function adds number of seconds to this variable.
@param aCurrentCharNumber Character number to start looking in the value.
@return ETrue if the value could be interpreted as a dur-hour, EFalse otherwise.
@internalTechnology
*/
TBool CICalValue::GetDurHourL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const
	{
	TRACE_ENTRY_POINT;
	
	//	dur-hour   = 1*DIGIT "H" [dur-minute]
	
	if (aCurrentCharNumber >= iValue->Length())
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}

	// Create a new TPtrC containing the data from iValue which we need
	TPtrC data(iValue->Mid(aCurrentCharNumber));
	
	// Find the next 'H' in the descriptor
	TInt nextHPos(data.Locate(KICalHourChar));
	
	// Check it exists
	if (nextHPos != KErrNotFound)
		{
		// If character found, construct a number from currentCharNumber to nextHPos
		TInt theNum(ReadIntL(data.Left(nextHPos)));
		
		// Convert this time from hours to seconds
		theNum *= KICalSecondsPerHour;
		
		aIntervalSecs += theNum;
		aCurrentCharNumber += nextHPos + 1; // Extra increment to get past 'H'
		
		// Check if a dur-minute follows. It's optional
		// so we can ignore it's return value.
		GetDurMinuteL(aIntervalSecs,aCurrentCharNumber);
		TRACE_EXIT_POINT;
		return ETrue;
		}

	// If no character found, return EFalse
	TRACE_EXIT_POINT;
	return EFalse;
	}

/**
Reads in and converts a dur-minute into a number of seconds. There is no range
check on the number of minutes.
@param aIntervalSecs Function adds number of seconds to this variable.
@param aCurrentCharNumber Character number to start looking in the value.
@return ETrue if the value could be interpreted as a dur-minute, EFalse
otherwise.
@internalTechnology
*/
TBool CICalValue::GetDurMinuteL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const
	{
	TRACE_ENTRY_POINT;
	
	//	dur-minute = 1*DIGIT "M" [dur-second]

	if (aCurrentCharNumber >= iValue->Length())
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}

	// Create a new TPtrC containing the data from iValue which we need
	TPtrC data(iValue->Mid(aCurrentCharNumber));
	
	// Find the next 'M' in the descriptor
	TInt nextMPos(data.Locate(KICalMinuteChar));
	
	// Check it exists
	if (nextMPos != KErrNotFound)
		{
		// If character found, construct a number from currentCharNumber to nextMPos
		TInt theNum(ReadIntL(data.Left(nextMPos)));

		// Convert this time from minutes to seconds
		theNum *= KICalSecondsPerMinute;
		
		aIntervalSecs += theNum;
		aCurrentCharNumber += nextMPos + 1; // Extra increment to get past 'M'
		
		// Check if a dur-second follows. It's optional
		// so we can ignore it's return value.
		GetDurSecondL(aIntervalSecs,aCurrentCharNumber);
		TRACE_EXIT_POINT;
		return ETrue;
		}

	// If no character found, return EFalse
	TRACE_EXIT_POINT;
	return EFalse;
	}
		
/**
Reads in and converts a dur-second into a number of seconds. There is no range
check on the number of seconds.
@param aIntervalSecs Function adds number of seconds to this variable.
@param aCurrentCharNumber Character number to start looking in the value.
@return ETrue if the value could be interpreted as a dur-second, EFalse
otherwise.
@internalTechnology
*/
TBool CICalValue::GetDurSecondL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const
	{
	TRACE_ENTRY_POINT;
	
	//	dur-second = 1*DIGIT "S"

	if (aCurrentCharNumber >= iValue->Length())
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}

	// Create a new TPtrC containing the data from iValue which we need
	TPtrC data(iValue->Mid(aCurrentCharNumber));
	
	// Find the next 'S' in the descriptor
	TInt nextSPos(data.Locate(KICalSecondChar));
	
	// Check it exists
	if (nextSPos != KErrNotFound)
		{
		// If character found, construct a number from currentCharNumber to nextSPos
		TInt theNum(ReadIntL(data.Left(nextSPos)));
		
		aIntervalSecs += theNum;
		aCurrentCharNumber += nextSPos;
		
		TRACE_EXIT_POINT;
		return ETrue;
		}

	// If no character found, return EFalse
	TRACE_EXIT_POINT;
	return EFalse;
	}

/**
Reads in and converts a dur-week into a number of seconds
@param aIntervalSecs Function adds number of seconds to this variable
@param aCurrentCharNumber Character number to start looking in the value.
@return ETrue if the value could be interpreted as a dur-week, EFalse
otherwise.
@internalTechnology
*/
TBool CICalValue::GetDurWeekL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const
	{
	TRACE_ENTRY_POINT;
	
	//	dur-week   = 1*DIGIT "W"
	
	if (aCurrentCharNumber >= iValue->Length())
		{
		TRACE_EXIT_POINT;
		return EFalse;
		}

	// Create a new TPtrC containing the data from iValue which we need
	TPtrC data(iValue->Mid(aCurrentCharNumber));
	
	// Find the next 'W' in the descriptor
	TInt nextWPos(data.Locate(KICalWeekChar));
	
	// Check it exists
	if (nextWPos != KErrNotFound)
		{
		// If character found, construct a number from currentCharNumber to nextWPos
		TInt theNum(ReadIntL(data.Left(nextWPos)));
		
		// Convert this time from weeks to seconds
		theNum *= KICalSecondsPerWeek;
		
		aIntervalSecs += theNum;
		aCurrentCharNumber += nextWPos;
		
		TRACE_EXIT_POINT;
		return ETrue;
		}

	// If no character found, return EFalse
	TRACE_EXIT_POINT;
	return EFalse;
	}


// End of File


	
