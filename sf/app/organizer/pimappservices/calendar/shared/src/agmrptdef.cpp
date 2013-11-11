// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include "agmrptdef.h"

#include "agmsimpleentry.h"
#include "agmdate.h"
#include "agmutil.h"
#include "agmtlsproxy.h"
#include "agmdebug.h"
#include "agmtzrules.h"

#include <vtzrules.h>

const TInt16 KRptLocalOffsetNotSet = KMaxTInt16;

// --------------------------------------- TAgnRpt -------------------------------

TBool TAgnRpt::HourAndMinutesDifferentFromStartTimeL(const TTime& aRptLocalTime) const
	{
	const TDateTime KRptLocalTestDT = aRptLocalTime.DateTime();
	const TDateTime KRptLocalStartDT = StartTimeAsRptLocalL().DateTime();
	
	if (KRptLocalTestDT.Hour() != KRptLocalStartDT.Hour() ||
		KRptLocalTestDT.Minute() != KRptLocalStartDT.Minute())
		{
		// Hour and minute are not the same, it is possible that they are at the same time in UTC but
		// due to the 'missing hour' the two local times appear different.
		// Compare the UTC times.
		
		const TDateTime KUtcStartDT = StartTime().UtcL().DateTime();
		const TDateTime KUtcTestDT = ConvertFromRepeatLocalToUtcL(aRptLocalTime).DateTime();
		
		if (KUtcStartDT.Hour() != KUtcTestDT.Hour() ||
			KUtcStartDT.Minute() != KUtcTestDT.Minute())
			{
			return ETrue;
			}
		}
	return EFalse;
	}

/** Constructs the repeat object. 
The until date is initialised to max time, the repeat interval to 1.
@internalComponent
*/
TAgnRpt::TAgnRpt(CAgnRptDef& aOwningRptDef) :
	iOwningRptDef(aOwningRptDef)
	{
	ClearAll();
	}

/** Constructs a new TAgnRpt object from an existing one.

@internalComponent
@param aRRule The object to be copied. */
TAgnRpt::TAgnRpt(const TAgnRpt& aRRule, CAgnRptDef& aOwningRptDef) :
	iOwningRptDef(aOwningRptDef)
	{
	ClearAll();
	iUntilTime = aRRule.iUntilTime;
	iInterval = aRRule.iInterval;
	iCount = aRRule.iCount;
	iUntiltimeHasBeenSet = aRRule.iUntiltimeHasBeenSet;
	}
	
void TAgnRpt::ResetCachedStartTimeOffset()
	{
	iStartTimeRptLocalOffset = KRptLocalOffsetNotSet;
	}

void TAgnRpt::ResetCachedUntilTimeOffset()
	{
	iUntilTimeRptLocalOffset = KRptLocalOffsetNotSet;
	}
	
MAgnCalendarTimeMode::TTimeMode TAgnRpt::TimeMode() const
	{
	return iOwningRptDef.TimeMode();
	}
	
TTime TAgnRpt::ConvertFromRepeatLocalToUtcL(const TTime& aRptLocalDate) const
	{
	return iOwningRptDef.ConvertFromRepeatLocalToUtcL(aRptLocalDate);
	}
	
TTime TAgnRpt::ConvertFromUtcToRepeatLocalL(const TTime& aUtcDate) const
	{
	return iOwningRptDef.ConvertFromUtcToRepeatLocalL(aUtcDate);
	}
	
TTime TAgnRpt::ConvertFromRepeatLocalToSystemLocalL(const TTime& aRptLocalTime) const
	{
	// If TimeMode is floating, avoid unnecessary conversion by using aRptLocalTime	
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		return aRptLocalTime;
		}
	
	const TTime KUtcTime = ConvertFromRepeatLocalToUtcL(aRptLocalTime);
	return AgnDateTime::ConvertToLocalTimeL(KUtcTime);
	}

TTime TAgnRpt::ConvertFromSystemLocalToRepeatLocalL(const TTime& aSysLocalTime) const
	{
	// If TimeMode is floating, avoid unnecessary conversion by using aSysLocalTime	
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		return aSysLocalTime;
		}
	
	const TTime KUtcTime = AgnDateTime::ConvertToLocalTimeL(aSysLocalTime);
	return ConvertFromUtcToRepeatLocalL(KUtcTime);
	}

TTime TAgnRpt::StartTimeAsRptLocalL() const
 	{
 	TTime startTimeRptLocal;
 	GetTimeAsRptLocalL(StartTime(), startTimeRptLocal, iStartTimeRptLocalOffset);
 	return startTimeRptLocal;
 	}

TTime TAgnRpt::UntilTimeAsRptLocalL() const
 	{
 	TTime untilTimeRptLocal;
 	GetTimeAsRptLocalL(UntilTimeL(), untilTimeRptLocal, iUntilTimeRptLocalOffset);
 	return untilTimeRptLocal;
 	}

void TAgnRpt::GetTimeAsRptLocalL(const TAgnCalendarTime& aCalendarTime, TTime& aTimeAsRptLocal, TInt16& aCachedOffset) const
	{
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		aTimeAsRptLocal = aCalendarTime.LocalL(); // if the entry is floating, system local is the same as rpt local
		return;
		}
	
	if (aCachedOffset == KRptLocalOffsetNotSet)
		{
		aCachedOffset = 0;
 		TTime rptLocalTime = ConvertFromUtcToRepeatLocalL(aCalendarTime.UtcL());
		TTimeIntervalMinutes mins;
		rptLocalTime.MinutesFrom(aCalendarTime.UtcL(), mins);
		aCachedOffset = mins.Int();
 		}
	aTimeAsRptLocal = aCalendarTime.UtcL() + TTimeIntervalMinutes(aCachedOffset);
 	}

EXPORT_C const TAgnCalendarTime& TAgnRpt::UntilTimeL() const
 	{
 	//	if iCount is set, the until time will not be set.
 	if (iCount > 0)
 		{
 		TTime time = FindRptUntilTimeLocalL(iCount);
 			 	
 		if (!iUntiltimeHasBeenSet)
 			{
 			if (TimeMode() == MAgnCalendarTimeMode::EFloating)	
 				{
 				iUntilTime.SetFloatingL(time);	
 				}
 			else
 		 		{
 		 		iUntilTime.SetLocalL(time);	
 				}
			}
 		}
 		
 	if(iUntilTime.UtcL() > AgnDateTime::MaxDate())
	 	{
	 	iUntilTime.SetUtcL(AgnDateTime::MaxDate());	
	 	}
	 	
 	return iUntilTime;
 	}

void TAgnRpt::SetUntilTime(const TAgnCalendarTime& aUntilTime)
	{
	iUntilTime = aUntilTime;
	iUntilTimeRptLocalOffset = KRptLocalOffsetNotSet;
	iUntiltimeHasBeenSet = ETrue;
	}

const TAgnCalendarTime& TAgnRpt::StartTime() const
/** Gets the start time as a @see TAgnCalendarTime.

@internalComponent
@return The start time. */
	{
	return iOwningRptDef.StartTime();
	}

void TAgnRpt::ClearAll()
/** Clears all the repeat details. 

The until time is set to max time and the repeat interval to 1.
The cached repeat local time offsets are reset.

@internalComponent
*/
	{
	iUntilTime.SetUtcL(AgnDateTime::MaxDate()); // this can't leave
	iStartTimeRptLocalOffset = KRptLocalOffsetNotSet;
	iUntilTimeRptLocalOffset = KRptLocalOffsetNotSet;
	iInterval = 1;
	iCount = -1;
	iUntiltimeHasBeenSet = EFalse;
	}


TInt TAgnRpt::InvariantL() const
/**
Check that the rpt doesn't violate its invariants
@internalComponent
*/
	{
	if (Type() < EDaily || Type() > EYearlyByDay)
		{
		return (EAgmErrBadRepeat);
		}

 		
	if (Interval() <= 0 || !AgnDateTime::IsValidAgendaTTime(iUntilTime.UtcL()) || StartTime() >= iUntilTime)
		{
		return (EAgmErrBadTime);
		}

	return (KErrNone);
	}



void TAgnRpt::InternalizeL(RReadStream& aStream)
/** Internalises the repeat definition from a read stream, including type, repeat 
details and exception list.

@internalComponent
@param aStream The stream from which the object should be internalised. */
	{
	aStream >> iUntilTime;
	iInterval = aStream.ReadUint16L();
	iStartTimeRptLocalOffset = KRptLocalOffsetNotSet;
	iUntilTimeRptLocalOffset = KRptLocalOffsetNotSet;
	}


void TAgnRpt::ExternalizeL(RWriteStream& aStream) const
/** Externalises the repeat definition to a write stream.

@internalComponent
@param aStream The stream to which the object should be externalised. */
	{
	aStream << iUntilTime;
	aStream.WriteUint16L(iInterval);
	}
	
TBool TAgnRpt::operator==(const TAgnRpt& aRRule) const
/** Compares two repeat details objects.

@internalComponent
@param aRRule The repeat details to compare with this object.
@return True if all properties agree, otherwise False. */
	{
	return (iInterval == aRRule.iInterval && UntilTimeL() == aRRule.UntilTimeL());
	}

TBool TAgnRpt::operator!=(const TAgnRpt& aRRule) const
	{
	return ( ! (*this == aRRule) );
	}

TUint TAgnRpt::MapDayToBits(TDay aDay) const
/**
 Provides a mapping between the enumeration values of TDay to bit fields
*/
	{
	switch(aDay)
		{
	case EMonday : 	return (EBit1);
	case ETuesday:	return (EBit2);
	case EWednesday:return (EBit3);
	case EThursday:	return (EBit4);
	case EFriday:	return (EBit5);
	case ESaturday:	return (EBit6);
	case ESunday:	return (EBit7);
	default: 
		{
		_DBGLOG_ENTRY(AgmDebug::DebugLog("TAgnRpt: Panic - EAgmErrInvalidWeekDay");)
		DBG_PANIC(EAgmErrInvalidWeekDay);
		}
		}
	return (EBit0);
	}


TUint TAgnRpt::MapDateToBits(TUint aNum) const
/**
 Provides a mapping between the numbers 0 to 30 to bit fields.
 0 = the first day of a month, 30 = the 31st day of the month
*/
	{
	if (aNum > 30)
		{
		_DBGLOG_ENTRY(AgmDebug::DebugLog("TAgnRpt: Panic - EAgmErrInvalidMonthDay");)
		DBG_PANIC(EAgmErrInvalidMonthDay);
		return (EBit0);
		}

	return (1 << aNum);
	}

TTime TAgnRpt::NudgeNextAlignedStartTimeL(const TTime& aRptLocalDate) const
	{
	TTime startTime = StartTimeAsRptLocalL();

	// Find the previous aligned start time from aRptLocalDate
	TTime alignedDate(startTime + aRptLocalDate.DaysFrom(startTime));
	
	const TTime KUtcAlignedDate = ConvertFromRepeatLocalToUtcL(alignedDate);
	const TTime KUtcLocalDate = ConvertFromRepeatLocalToUtcL(aRptLocalDate);
	
	// Don't add a day if aRptLocalDate is already 'aligned start time'
	// or if it is before the start of the repeat rule, because DaysFrom
	// already returns 'next aligned start time' in that case
	if ((aRptLocalDate != alignedDate && aRptLocalDate > startTime) || KUtcLocalDate > KUtcAlignedDate)
		{
		// Add a day to get the 'next aligned start time'
		alignedDate += TTimeIntervalDays(1);
		}
	
	return alignedDate;
	}
 
TTime TAgnRpt::NudgePreviousAlignedStartTimeL(const TTime& aRptLocalDate) const
	{
	TTime startTime = StartTimeAsRptLocalL();
	
	// Find the previous aligned start time from aRptLocalDate
	TTime alignedDate(startTime + aRptLocalDate.DaysFrom(startTime));
	
	const TTime KUtcAlignedDate = ConvertFromRepeatLocalToUtcL(alignedDate);
	const TTime KUtcLocalDate = ConvertFromRepeatLocalToUtcL(aRptLocalDate);

	if ( (aRptLocalDate != alignedDate && aRptLocalDate < startTime) || KUtcLocalDate < KUtcAlignedDate )
		{
		// Subtract a day when aRptLocalDate is before the repeat rule
		// start date, because DaysFrom() is a day out in that case 
		alignedDate -= TTimeIntervalDays(1);
		}
		
	return alignedDate;
	}

EXPORT_C TTime TAgnRpt::FindRptUntilTimeLocalL(TUint aCount) const
/** Calculates the repeat's end date from the specified number of instances. 

If the end date would be after the agenda model's valid date range, it returns 
a NULL TTime value.

@internalComponent
@param aCount The number of instances.
@return The end date (System Local Time). */
	{
	TTime end = FindRptUntilTimeRptLocalL(aCount);
	return ConvertFromRepeatLocalToSystemLocalL(end);
	}
	
EXPORT_C TUint16 TAgnRpt::Interval() const
/** Gets the repeat interval.

@return The interval. */
	{
	return (iInterval);
	}

EXPORT_C void TAgnRpt::SetInterval(TUint16 aInterval)
/** Sets the repeat interval. 

This is a number of days for a daily repeat, a number of weeks for a 
weekly repeat, etc. 

@param aInterval The interval. */
	{
	if (aInterval > 0)
		{
		iInterval = aInterval;
		}
	}
	
EXPORT_C void TAgnRpt::SetCount(TInt aRepCount)
	{
	iCount = aRepCount;
	iUntiltimeHasBeenSet = EFalse;
	}
	
EXPORT_C TInt TAgnRpt::Count() const
/*
>0 - Indicate how many occurrences of a repeating rule.
 0 - Repeating forever. Note that it is the default value if there is neither Until time nor Count has been set. 
<0 - A Until time has been set but not the count. 

*/	{
	return iCount;
	}

// --------------------------------- TAgnDailyRpt ------------------------------
EXPORT_C TAgnDailyRpt::TAgnDailyRpt(CAgnRptDef& aOwningRptDef) : TAgnRpt(aOwningRptDef)
/**
This is not inline to prevent non-exportable functions causing a linker error in calling code
Constructs a default daily repeat object. 
@internalComponent
*/
	{
	}

/** Constructs a new TAgnDailyRpt object from an existing TAgnDailyRpt object.

@internalComponent
@param aRRule The object to be copied. */
EXPORT_C TAgnDailyRpt::TAgnDailyRpt(const TAgnDailyRpt& aRRule, CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aRRule, aOwningRptDef)
	{
	}

TAgnRpt::TType TAgnDailyRpt::Type() const
	{
	return TAgnRpt::EDaily;
	}

TBool TAgnDailyRpt::IsAlignedUtcL(const TTime& aDate) const
/**
 Returns true if aDate (UTC) falls on an instance generated by the repeat algorithm 
@internalComponent
*/
	{
	// convert aDate from UTC to Repeat Local
	TTime rptLocalDate = ConvertFromUtcToRepeatLocalL(aDate);
	return IsAlignedRptLocalL(rptLocalDate);
	}
	
TBool TAgnDailyRpt::IsAlignedRptLocalL(const TTime& aDate) const
/**
Returns true if aDate (Repeat Local) falls on an instance generated by the repeat algorithm 
@internalComponent
*/
	{
	// see if the day falls on an instance
	if ( AgnDateTime::DaysBetweenDates(aDate, StartTimeAsRptLocalL()) % iInterval )
		{
		return EFalse;
		}
	
	if (HourAndMinutesDifferentFromStartTimeL(aDate))
		{
		return EFalse;
		}
	
	return ETrue;	
	}

TUint TAgnDailyRpt::InstanceCountL() const
/** Gets the number of instances generated by the repeat algorithm.

@internalComponent
@return The number of instances. */
	{
	// to avoid unnecessary time conversions, date substractions should be done in 
	// the time reference in which start and end dates are stored
	return 1 + (AgnDateTime::DaysBetweenDates(UntilTimeAsRptLocalL(), StartTimeAsRptLocalL()) / iInterval) ;
	}

TTime TAgnDailyRpt::FindRptUntilTimeRptLocalL(TUint aCount) const
	{
	// do calculations in repeat local
	TUint max = (AgnDateTime::MaxDate().DaysFrom(StartTimeAsRptLocalL()).Int() + 1) / iInterval;
	if (aCount > max)
		{
		return AgnDateTime::MaxDate();
		}
	return StartTimeAsRptLocalL() + TTimeIntervalDays(iInterval * (aCount-1));
	}

TTime TAgnDailyRpt::NudgeNextInstanceUtcL(const TTime& aUtcDate) const
/**
If aUtcDate is already a date generated by the repeat algorithm then that is the date returned,
otherwise the next instance from aUtcDate is returned (in UTC time)
@internalComponent
*/
	{
	//                       aDate-start 
	// nextDate = start + ( ------------- + 1) * interval
	//                         interval	
	// (NOTE: (aDate-start)/interval is NOT incremented if the modulus of division is zero )	

	// convert aUtcDate to Repeat Local Time
	TTime thisDate = ConvertFromUtcToRepeatLocalL(aUtcDate);
		
	TTime next = NudgeNextAlignedStartTimeL(thisDate);
	
	if (IsAlignedRptLocalL(next))
		{
		return ConvertFromRepeatLocalToUtcL(next);
		}
		
	thisDate = next;
	
	// get number of last instance
	TInt instanceNumber = AgnDateTime::DaysBetweenDates(thisDate, StartTimeAsRptLocalL()) / iInterval;
	// if there isn't an instance on our date, we want the next instance
	if (AgnDateTime::DaysBetweenDates(thisDate, StartTimeAsRptLocalL()) % iInterval)
		{
		++instanceNumber;
		}
	
	// resolve instance number to days since start
	TInt daysFromStartToNextInstance = instanceNumber * iInterval;
	
	// add to start date
	TTime nextDate = StartTimeAsRptLocalL() + TTimeIntervalDays(daysFromStartToNextInstance);
	
	if (nextDate > AgnDateTime::MaxDate())
		{
		return Time::NullTTime();
		}
		
	// convert from repeat local to UTC and return
	return ConvertFromRepeatLocalToUtcL(nextDate);
	}


TTime TAgnDailyRpt::NudgePreviousInstanceUtcL(const TTime& aUtcDate) const
/**
If aUtcDate is already a date generated by the repeat algorithm then that is the date returned, 
otherwise the previous instance is returned.
@internalComponent
*/
	{

	TTime thisDate = ConvertFromUtcToRepeatLocalL(aUtcDate);	
		
	TTime prev = NudgePreviousAlignedStartTimeL(thisDate);
	
	if (IsAlignedRptLocalL(prev))
		{
		return ConvertFromRepeatLocalToUtcL(prev);
		}
		
	thisDate = prev;
	
	// get number of last instance
	TInt instanceNumber = AgnDateTime::DaysBetweenDates(thisDate, StartTimeAsRptLocalL()) / iInterval;

	// resolve instance number to days since start
	TInt daysFromStartToPreviousInstance = instanceNumber * iInterval;
	
	// add to start date
	TTime prevDate = StartTimeAsRptLocalL() + TTimeIntervalDays(daysFromStartToPreviousInstance);
	
	// an instance could have occurred later on our date - get previous instance
	if (prevDate > thisDate )
		{
		prevDate -= TTimeIntervalDays(iInterval);
		}
	
	if (prevDate < AgnDateTime::MinDate())
		{
		return Time::NullTTime();
		}
			
	return ConvertFromRepeatLocalToUtcL(prevDate);
	}


// ------------------------------------ TAgnWeeklyRpt--------------------------------
EXPORT_C TAgnWeeklyRpt::TAgnWeeklyRpt(CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aOwningRptDef)
/** Constructs a weekly repeat object. 

The weekly repeat days are all cleared, and the day which is the start of the 
week is initialised from the system's locale settings. 

@internalComponent
*/
	{
	iWeeklyRptDays = 0;
	TLocale locale;
	SetFirstDayOfWeek(locale.StartOfWeek());
	}
	
EXPORT_C TAgnWeeklyRpt::TAgnWeeklyRpt(const TAgnWeeklyRpt& aRRule, CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aRRule, aOwningRptDef)
/** Constructs a new TAgnWeeklyRpt object from an existing TAgnWeeklyRpt object.

@internalComponent
@param aRRule The object to be copied. */
	{
	iWeeklyRptDays = aRRule.iWeeklyRptDays;
	iFirstDayOfWeek = aRRule.iFirstDayOfWeek;
	}

TAgnRpt::TType TAgnWeeklyRpt::Type() const
	{
	return TAgnRpt::EWeekly;
	}

EXPORT_C void TAgnWeeklyRpt::SetDay(TDay aDay)
/** Adds a day - more than one day can be set at a time.

@internalComponent
@param aDay The day to be added. */
	{ 

	iWeeklyRptDays |= MapDayToBits(aDay);
	}

EXPORT_C TBool TAgnWeeklyRpt::IsDaySet(TDay aDay) const
/** Tests whether a specified day has been set.

@internalComponent
@param aDay The day to test.
@return True if the day is set, false if not. */
	{ 
	return (iWeeklyRptDays & (MapDayToBits(aDay))); 
	}


EXPORT_C void TAgnWeeklyRpt::SetFirstDayOfWeek(TDay aDay)
/**
Set the start day of the week

@internalComponent
*/
	{
	iFirstDayOfWeek= static_cast<TUint8>(MapDayToBits(aDay));
	}

EXPORT_C TDay TAgnWeeklyRpt::FirstDayOfWeek() const
/** Gets the day that is considered to be the first day of the week.

This is set during construction from the operating system's locale settings.

@internalComponent
@return The first day of the week. */
	{
	switch(iFirstDayOfWeek)
		{
	case EBit1: return (EMonday);
	case EBit2:	return (ETuesday);
	case EBit3: return (EWednesday);
	case EBit4:	return (EThursday);
	case EBit5:	return (EFriday);
	case EBit6:	return (ESaturday);
	case EBit7:	return (ESunday);
	default: 
		{
		_DBGLOG_ENTRY(AgmDebug::DebugLog("TAgnWeeklyRpt: Panic - EAgmErrInvalidWeekDay");)
		DBG_PANIC(EAgmErrInvalidWeekDay);
		}
		}
	return (EMonday); 
	}

TInt TAgnWeeklyRpt::InvariantL() const
/**
Check that the rpt doesn't violate its invariants
@internalComponent
*/
	{
	if (TAgnRpt::InvariantL()==KErrNone && NumDaysSet() > 0  &&
		(iFirstDayOfWeek==EBit1|| iFirstDayOfWeek==EBit2 || iFirstDayOfWeek==EBit3|| iFirstDayOfWeek==EBit4 || 
		iFirstDayOfWeek==EBit5 || iFirstDayOfWeek==EBit6 || iFirstDayOfWeek==EBit7))
		{
		return (KErrNone);
		}

	return (EAgmErrBadRepeat);
	}

void TAgnWeeklyRpt::InternalizeL(RReadStream& aStream)
/** Internalises the weekly repeat object from a read stream.

@internalComponent
@param aStream Stream from which the object should be internalised. */
	{
	TAgnRpt::InternalizeL(aStream);
	aStream >> iWeeklyRptDays;
	aStream >> iFirstDayOfWeek;
	}

void TAgnWeeklyRpt::ExternalizeL(RWriteStream& aStream) const
/** Externalises the weekly repeat object to a write stream.

@internalComponent
@param aStream Stream to which the object should be externalised. */
	{

	TAgnRpt::ExternalizeL(aStream);
	aStream << iWeeklyRptDays;
	aStream << iFirstDayOfWeek;
	}

TBool TAgnWeeklyRpt::operator==(const TAgnWeeklyRpt& aRRule) const
/** Compares two weekly repeat objects for equality.

@internalComponent
@param aRRule The instance to be compared.
@return True if all repeat details are equal, otherwise False. */
	{
	return (TAgnRpt::operator==(aRRule) && iWeeklyRptDays==aRRule.iWeeklyRptDays);
	}

TUint TAgnWeeklyRpt::InstanceCountL() const
/** Gets the number of instances of the repeat.

@internalComponent
@return The number of instances. */
	{
	TTime startOfFirstWeek = DayAtStartOfWeek(StartTimeAsRptLocalL());
	TUint count = AgnDateTime::DaysBetweenDates(UntilTimeAsRptLocalL(), startOfFirstWeek) / (7 * iInterval); // No of complete weeks
	TTime startDate = startOfFirstWeek + TTimeIntervalDays(count * 7 * iInterval); // must check any remaining bit of last week
	count*=NumDaysSet();
	while (startDate <= UntilTimeAsRptLocalL())
		{
		if (IsAlignedRptLocalL(startDate))
			{
			++count;
			}
		startDate += TTimeIntervalDays(1);
		}

	// remove any instances before the start date
	while (startOfFirstWeek < StartTimeAsRptLocalL())
		{
		if (IsAlignedRptLocalL(startOfFirstWeek))
			{
			count--;
			}			
		startOfFirstWeek += TTimeIntervalDays(1);
		}

	return (count);
	}
	
TTime TAgnWeeklyRpt::FindRptUntilTimeRptLocalL(TUint aCount) const
	{
	const TInt KDaysInAWeek = 7;
	
	// we do all calculations using repeat local times
	TTimeIntervalDays maxDays = AgnDateTime::MaxDate().DaysFrom(StartTimeAsRptLocalL());
	TInt max = (((maxDays.Int() / KDaysInAWeek) * NumDaysSet()) / iInterval) + 1;
	
	if (aCount > max)
		{
		return AgnDateTime::MaxDate();
		}
	
	TInt instancesPerWeek = NumDaysSet(); // number of instances in each week
	TTime end;
	if (aCount % instancesPerWeek == 0)
		{
		end = StartTimeAsRptLocalL() + TTimeIntervalDays(((aCount - 1) / instancesPerWeek) * KDaysInAWeek * iInterval);
		}
	else
		{
		end = StartTimeAsRptLocalL() + TTimeIntervalDays((aCount / instancesPerWeek) * KDaysInAWeek * iInterval);
		}

	if (end > AgnDateTime::MaxDate())
		{
		return (Time::NullTTime());
		}

	TInt remainder = aCount % instancesPerWeek;
	TInt numToNudge = remainder ? remainder : instancesPerWeek;

	end=NudgeNextInstanceRptLocalL(end);
	for (; numToNudge > 1 && end != Time::NullTTime(); numToNudge--)
		{
		end += TTimeIntervalDays(1);
		end = NudgeNextInstanceRptLocalL(end);
		}	
	return end;	
	}

TTime TAgnWeeklyRpt::NudgeNextInstanceUtcL(const TTime& aDate) const
/**
If aDate is already a date generated by the repeat algorithm then that is the date returned,
otherwise the next instance from aDate is returned (in UTC)
*/
	{
	
	// Convert aDate from UTC to repeat local
	TTime next = ConvertFromUtcToRepeatLocalL(aDate);
	
	next = NudgeNextInstanceRptLocalL(next);
	
	// convert from repeat local back to UTC and return
	return ConvertFromRepeatLocalToUtcL(next);
	}

TTime TAgnWeeklyRpt::NudgeNextInstanceRptLocalL(const TTime& aRptLocalDate) const
/**
Nudges next instance in repeat local time
*/
	{
	TTime next = NudgeNextAlignedStartTimeL(aRptLocalDate);
	FOREVER
		{
		if (IsAlignedRptLocalL(next))
			{
			break;
			}
		if (next == AgnDateTime::MaxDate())
			{
			return (Time::NullTTime());
			}
		next += TTimeIntervalDays(1);
		}
	return next;
	}

TTime TAgnWeeklyRpt::NudgePreviousInstanceUtcL(const TTime& aDate) const
/**
If aDate is already a date generated by the repeat algorithm then that is the date returned, 
otherwise the previous instance is returned.

The algorithm works by skipping backswards over any unaligned weeks, if the current date 
isn't in an aligned week then it is jumped back to take it to the week after the previous aligned 
week. Then one day at a time is subtracted and each day checked to see whether it is aligned or not.

If no aligned day is found after creeping back 7 days, then the week is re-checked to see if it is 
still in an aligned week (by recalling this function). 
Both received parameter (aDate) and returned value are expressed in UTC
@internalComponent
*/
	{
	TTime prev = ConvertFromUtcToRepeatLocalL(aDate);
	prev = NudgePreviousInstanceRptLocalL(prev);
	return ConvertFromRepeatLocalToUtcL(prev);
	}
	
TTime TAgnWeeklyRpt::NudgePreviousInstanceRptLocalL(const TTime& aDate) const
	{
	TTime prev = NudgePreviousAlignedStartTimeL(aDate);
	
	if (IsAlignedRptLocalL(prev))
		return prev;
	
	// get the first day of the week from the prev parameter
	TDay dayOfWeekADate = prev.DayNoInWeek();
	TTime newDate = prev;
	while (dayOfWeekADate != FirstDayOfWeek())
		{
		newDate -= TTimeIntervalDays(1);
		if (dayOfWeekADate > EMonday)
			{
			dayOfWeekADate = (TDay)(TInt(dayOfWeekADate) - 1);
			}
		else
			{
			dayOfWeekADate = ESunday;
			}
		}
	// get the first day of the week from the StartDate
	TDay dayOfWeekStartTime = StartTimeAsRptLocalL().DayNoInWeek();
	TTime newStartTime = StartTimeAsRptLocalL();
	while (dayOfWeekStartTime != FirstDayOfWeek())
		{
		newStartTime -= TTimeIntervalDays(1);
		if (dayOfWeekStartTime > EMonday)
			{
			dayOfWeekStartTime = (TDay)(TInt(dayOfWeekStartTime) - 1);
			}
		else
			{
			dayOfWeekStartTime = ESunday;
			}
		}
	// check if this week has any instances
	TInt weeks = AgnDateTime::DaysBetweenDates(newDate, newStartTime) / 7; // needs to compare the first days in the weeks
	if (weeks % iInterval != 0)
		{
		// if not, jump to the week after the previous instance, then creep back one day at a time
		TInt daysAfterStartTime = AgnDateTime::DaysBetweenDates(prev, StartTimeAsRptLocalL());
		TInt weeksSafelyAfterLastInstance = (daysAfterStartTime/7) % iInterval - 1;
		if (weeksSafelyAfterLastInstance > 0)
			{
			prev = prev - TTimeIntervalDays(weeksSafelyAfterLastInstance*7);
			}
		}
		
	TInt count = 0;
	while (!IsAlignedRptLocalL(prev) && count < 7)
		{
		prev -= TTimeIntervalDays(1);
		++count;
		}
	if (count >= 7)
		{
		prev = NudgePreviousInstanceRptLocalL(prev); // call the 'jump back' code as may no longer be in a week with instances
		}
	if (prev < AgnDateTime::MinDate())
		{
		return (Time::NullTTime());
		}
	if (prev > AgnDateTime::MaxDate())
		{
		return (Time::NullTTime());
		}
	return prev;
	}


TUint TAgnWeeklyRpt::NumDaysSet() const
/** Gets the number of repeat days in the week which are set.

@internalComponent
@return The number of days. */
	{

	TUint count=0;
	for (TUint ii = 1;ii <= 0x40; ii <<= 1)
		{
		count += !!(iWeeklyRptDays & ii);
		}
	return (count);
	}

TBool TAgnWeeklyRpt::IsAlignedUtcL(const TTime& aDate) const
/**
Returns true if aDate falls on an instance generated by the repeat algorithm
(ignores repeat start/end dates)
*/
	{
	
	TTime localDate = ConvertFromUtcToRepeatLocalL(aDate);
	
	return IsAlignedRptLocalL(localDate);
	}
	
TBool TAgnWeeklyRpt::IsAlignedRptLocalL(const TTime& aDate) const
	{
	if (!IsDateInValidWeekRptLocalL(aDate))
		return (EFalse);

	// if it's in a valid week check if that day is set
	if (!IsDaySet(aDate.DayNoInWeek()))
		{
		return EFalse;
		}
		
	if (HourAndMinutesDifferentFromStartTimeL(aDate))
		{
		return EFalse;
		}
	
	return ETrue;
	}

TInt TAgnWeeklyRpt::IsDateInValidWeekRptLocalL(const TTime& aDate) const
// aDate is in Repeat Local Time
//
// Determine if aDate falls in a valid week i.e. one aligned according to the interval.
//
// For positive weeks the algorithm is its valid if ((week no -1) % interval)==0
//		Week numbers: 1  2  3  4  5  6  7  8
//   interval of 1    V  V  V  V  V  V  V  V      (5 - 1) % 1  == 0 so valid
//   interval of 2    V     V     V     V		  (5 - 1) % 2 == 0 so valid
//   interval of 3    V        V        V		  (5 - 1) % 3 == 1 so invalid  etc.
//
// But for negative weeks the algorithm is its valid if ((week no)% interval)==0
// i.e.		
//		week no			-4  -3  -2  -1  1  2  3  4  5
//  interval of 3            V     		V		 V				(-3 % 3) == 0 so valid
//
	{
		
	TInt weekNum=WeekNumRptLocalL(aDate);
	if (weekNum > 0) // in a positive week i.e. aDate >= the start of the first week
		return ((weekNum-1) % iInterval == 0);
	else
		return ((weekNum) % iInterval == 0); // aDate < the start of the first week
	}


TInt TAgnWeeklyRpt::WeekNumRptLocalL(const TTime& aDate) const
// First calculate the week number i.e.
//		December    January
//		29  30  31  1  2  3  4  5  6  7  8  9  10
//		T   F   S   S  M  T  W  T  F  S  S  M  T
//                  X
// 		        |     1st week		  | 2nd week
// 
// If X is the start date of the repeat and the first day of the week is a saturday then
// sunday the 8th is in week number  ((8 - (-1)) / 7) + 1  i.e the 2nd week
// Note that -1 represents 31st of december as it is 1 less than whatever the internal 
// representation of 1st Jan is.

	{
	TTime startOf1stWeek=DayAtStartOfWeek(StartTimeAsRptLocalL());
	TInt weekNo;
		if (aDate >= startOf1stWeek)
			weekNo=(AgnDateTime::DaysBetweenDates(aDate, startOf1stWeek)/7)+1;
		else
			weekNo=((AgnDateTime::DaysBetweenDates(aDate, startOf1stWeek)+1)/7)-1;    // i.e. december the30th above is in week -1

	return (weekNo);
	}


TTime TAgnWeeklyRpt::DayAtStartOfWeek(const TTime& aDay) const
//
// Returns the day at the start of the week containing aDay
//
// i.e. M  T  W  T  F  S  S  M  T  W  T  F  S  S
//      46 47 48 49 50 51 52 53 54 55 56 57 58 59
// If aDay is 53 (Monday) and the start of the week is a monday then 53 is returned
// If aDay is 53 but the start of the week is a Wednesday then 48 is returned
    {

    TTime dayAtWeekStart= aDay - TTimeIntervalDays(aDay.DayNoInWeek()) + TTimeIntervalDays(FirstDayOfWeek());
    if (dayAtWeekStart < 0)
 		dayAtWeekStart = 0;
    else if (dayAtWeekStart > aDay)
        dayAtWeekStart = dayAtWeekStart - TTimeIntervalDays(7);
    return(dayAtWeekStart);
    }




// ------------------------------ TAgnMonthlyRpt ----------------------------  
	
TAgnMonthlyRpt::TAgnMonthlyRpt(CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aOwningRptDef)
	{
	}

TAgnMonthlyRpt::TAgnMonthlyRpt(const TAgnRpt& aRRule, CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aRRule, aOwningRptDef)
	{
	}
	
TUint TAgnMonthlyRpt::InstanceCountL() const
/** Calculates the number of instances generated by the repeat algorithm.

@internalComponent
@return The number of instances. */
	{

	if (NumInstancesPerMonth()==0)
		return (0);

	TUint count=0;
	TTime nextDate = NudgeNextInstanceRptLocalL(StartTimeAsRptLocalL());
	while (nextDate <= UntilTimeAsRptLocalL() && nextDate >= AgnDateTime::MinDate() && nextDate <= AgnDateTime::MaxDate() )
		{
		nextDate += TTimeIntervalDays(1);
		nextDate = NudgeNextInstanceRptLocalL(nextDate);
		++count;
		}
	return (count);
	}

TTime TAgnMonthlyRpt::FindRptUntilTimeRptLocalL(TUint aCount) const
	{
	TTimeIntervalMonths monthsFrom = AgnDateTime::MaxDate().MonthsFrom(StartTimeAsRptLocalL());
	TInt numInstancesPerMonth = NumInstancesPerMonth();
	TUint max = (((monthsFrom.Int() + 1) * numInstancesPerMonth) / iInterval) + 1;
	
	if (aCount > max)
		{
		return AgnDateTime::MaxDate();
		}
	TInt rem = aCount;
	
	TTime end=StartTimeAsRptLocalL();
	if (IsAlignedRptLocalL(end))
		rem--;
	while (rem)
		{
		end += TTimeIntervalDays(1);
		if (end > AgnDateTime::MaxDate())
			return (Time::NullTTime());
		end=NudgeNextInstanceRptLocalL(end);
		rem--;
		}
	return end;
	}
	
TBool TAgnMonthlyRpt::DateFallsInAlignedMonthL(const TDateTime& aDate) const
/**
Returns ETrue if aDate (Repeat Local Time) falls within a month generated by the the repeat algorithm for
the monthly repeat types.
@internalComponent
*/
	{

	TDateTime start = StartTimeAsRptLocalL().DateTime();
	if (((aDate.Month() + (12 * aDate.Year())) -
			(start.Month() + (12 * start.Year()))) % iInterval)
		{
		return (EFalse);
		}
	return (ETrue);
	}


void TAgnMonthlyRpt::NudgeNextMonthL(TTime& aDate) const
/**
Finds start of next valid month; doesnt change day if it is in valid month
In this case you dont want to risk returning a result prior to aDate 
@internalComponent
*/
	{

	TDateTime next = aDate.DateTime();
	TDateTime start = StartTimeAsRptLocalL().DateTime();

	TTimeIntervalMonths monthsSince = (next.Month() - start.Month()) + 12 * (next.Year() - start.Year()); // no months since repeat started
	TTimeIntervalMonths nextValidMonth = ((monthsSince.Int()+Interval()-1)/Interval())*Interval();
	if (nextValidMonth > monthsSince)
		{
		AgnDateTime::AddTo(next,TTimeIntervalMonths(nextValidMonth.Int() - monthsSince.Int()));
		next.SetDay(0);		// set to start of month
		aDate = TTime(next);
		if (aDate > AgnDateTime::MaxDate())
			aDate = Time::NullTTime();
		}
	}

void TAgnMonthlyRpt::NudgePreviousMonthL(TTime& aDate) const
/**
Finds previous valid month; doesnt change day if it is in valid month
In this case you dont want to risk returning a result after aDate
@internalComponent
*/
	{
	TDateTime prev = aDate.DateTime();
	TDateTime start = StartTimeAsRptLocalL().DateTime();
	TTimeIntervalMonths monthNo=prev.Month() - start.Month() + 12 * (prev.Year() - start.Year());
	TTimeIntervalMonths monthsSince=(monthNo.Int()/Interval())*Interval();
	
	if (monthsSince.Int() < Abs(monthNo.Int()) || Abs(monthNo.Int() - monthsSince.Int()) == 11)
		{
		AgnDateTime::AddTo(prev,TTimeIntervalMonths(monthsSince.Int()-monthNo.Int()));
		prev.SetDay(Time::DaysInMonth(prev.Year(),prev.Month())-1);	// move to end of month
		aDate = TTime(prev);
		if (aDate < AgnDateTime::MinDate())
			aDate=Time::NullTTime();
		}
	}

TTime TAgnMonthlyRpt::NudgeNextInstanceUtcL(const TTime& aDate) const
/**
If aDate is already a date generated by the repeat algorithm then that is the date returned,
otherwise the next instance from aDate is returned 
@internalComponent
*/
	{
	TTime next = ConvertFromUtcToRepeatLocalL(aDate);
	next = NudgeNextInstanceRptLocalL(next);
	return ConvertFromRepeatLocalToUtcL(next);
	}

TTime TAgnMonthlyRpt::NudgeNextInstanceRptLocalL(const TTime& aDate) const
/**
If aDate is already a date generated by the repeat algorithm then that is the date returned,
otherwise the next instance from aDate is returned
@internalComponent
*/
	{

	TTime next = NudgeNextAlignedStartTimeL(aDate);
	
	if ( ! DateFallsInAlignedMonthL(next.DateTime()) || ! IsAlignedRptLocalL(next))
		{
		NudgeNextMonthL(next);
		if (next == Time::NullTTime())
			{
			return (Time::NullTTime());
			}
		while ( ! IsAlignedRptLocalL(next) )
			{ 
			TTimeIntervalDays numDaysToNextInstance = NumDaysToNextInstanceFrom(next);
			if (numDaysToNextInstance.Int())
				{
				next = next + TTimeIntervalDays(numDaysToNextInstance.Int());
				}
			else
				// jump to the 1st day of the next month
				{
				TDateTime d1 = next.DateTime();
				TInt day = d1.Day();
				TInt daysInMonth = Time::DaysInMonth(d1.Year(),d1.Month());
				next = next + TTimeIntervalDays(daysInMonth - day);
				}
			TDateTime d2 = next.DateTime();
			if (d2.Day() == 0)	// i.e. the 1st of the month
				{
				AgnDateTime::AddTo(d2,TTimeIntervalMonths(Interval()-1));
				next = TTime(d2);
				}
			if (next > AgnDateTime::MaxDate())
				{
				return (Time::NullTTime());
				}
			}
		}
	return next;
	}	

TTime TAgnMonthlyRpt::NudgePreviousInstanceUtcL(const TTime& aDate) const
//
// If aDate is already a date generated by the repeat algorithm then that is the date returned, 
// otherwise the previous instance is returned (UTC)
//
	{
	TTime prev = ConvertFromUtcToRepeatLocalL(aDate);
	prev = NudgePreviousInstanceRptLocalL(prev);
	return ConvertFromRepeatLocalToUtcL(prev);
	}

TTime TAgnMonthlyRpt::NudgePreviousInstanceRptLocalL(const TTime& aDate) const
//
// If aDate is already a date generated by the repeat algorithm then that is the date returned, 
// otherwise the previous instance is returned (Repeat Local Time)
//
	{
	TTime prev = NudgePreviousAlignedStartTimeL(aDate);
	if (!(IsAlignedRptLocalL(prev)))
		{
		if (Interval() > 1)
			{
			NudgePreviousMonthL(prev);
			if (prev==Time::NullTTime())
				return (Time::NullTTime());
			if (IsAlignedRptLocalL(prev))
				return prev;
			}
		do
			{
			TDateTime prevTDateTime=prev.DateTime();
			if (prevTDateTime.Day()==0 && Interval() > 1)
				{
				AgnDateTime::AddTo(prevTDateTime,TTimeIntervalMonths(1-Interval()));
				prev = TTime(prevTDateTime);
				}
			if (prev == AgnDateTime::MinDate())		
				return (Time::NullTTime());
			prev -= TTimeIntervalDays(1);
			}
		while (!(IsAlignedRptLocalL(prev)));
		}
	return (prev);
	}


// ------------------------------------- TAgnMonthlyByDaysRpt -------------------------
EXPORT_C TAgnMonthlyByDaysRpt::TAgnMonthlyByDaysRpt(CAgnRptDef& aOwningRptDef) : 
	TAgnMonthlyRpt(aOwningRptDef)
/** Constructs a monthly by days repeat object. 

The repeat days are all cleared. 

@internalComponent
*/
	{
	ClearAllDays();
	}
	
EXPORT_C TAgnMonthlyByDaysRpt::TAgnMonthlyByDaysRpt(const TAgnMonthlyByDaysRpt& aRRule, CAgnRptDef& aOwningRptDef) : 
	TAgnMonthlyRpt(aRRule, aOwningRptDef)
/** Constructs a new TAgnMonthlyByDaysRpt object from an existing one.

@internalComponent
@param aRRule The object to be copied. */
	{
	for (TInt i = 0; i < KMonthlyByDaysArraySize; ++i)
		{
		iMonthlyRptDays[i] = aRRule.iMonthlyRptDays[i];
		}
	}

TAgnRpt::TType TAgnMonthlyByDaysRpt::Type() const
	{
	return TAgnRpt::EMonthlyByDays;
	}
	
void TAgnMonthlyByDaysRpt::ClearAllDays()
/** Clears all the monthly repeat days. 

@internalComponent
*/
	{

	iMonthlyRptDays[EFirst]=0;
	iMonthlyRptDays[ESecond]=0;
	iMonthlyRptDays[EThird]=0;
	iMonthlyRptDays[EFourth]=0;
	iMonthlyRptDays[ELast]=0;
	}

EXPORT_C void TAgnMonthlyByDaysRpt::SetDay(TDay aDay,TWeekInMonth aWeek)
/** Adds a day in the specified week to the repeat object.

@internalComponent
@param aDay The day to be added.
@param aWeek The week in the month for the day to be added. */
	{

	__ASSERT_DEBUG(aWeek == EFirst || aWeek == ESecond || aWeek == EThird || aWeek == EFourth || aWeek == ELast,
				Panic(EAgmErrInvalidWeekNumber));

	iMonthlyRptDays[aWeek] |= MapDayToBits(aDay);
	}


EXPORT_C TBool TAgnMonthlyByDaysRpt::IsDaySet(TDay aDay,TWeekInMonth aWeek) const
/** Tests whether a day is set as a repeat in the given week.

@internalComponent
@param aDay The day to be tested for.
@param aWeek The week in the month.
@return True if set, False if not set. */
	{

	__ASSERT_DEBUG(aWeek == EFirst || aWeek == ESecond || aWeek == EThird || aWeek == EFourth || aWeek == ELast,
				Panic(EAgmErrInvalidWeekNumber));

	return (iMonthlyRptDays[aWeek]&(MapDayToBits(aDay))); 
	}


TInt TAgnMonthlyByDaysRpt::NumDaysSet() const
/** Gets the number of repeat days in the month.

@internalComponent
@return The number of days set. */
	{

	TInt total = 0;
	for (TUint i = EMonday; i <= ESunday; ++i)
		{
		for (TUint j = EFirst; j <= ELast; ++j)
			{
			if (IsDaySet(static_cast<TDay>(i), static_cast<TWeekInMonth>(j)))
				{
				++total;
				}
			}
		}

	return (total);
	}


TInt TAgnMonthlyByDaysRpt::NumInstancesPerMonth() const
	{
	return (NumDaysSet());
	}	

TInt TAgnMonthlyByDaysRpt::InvariantL() const
//
// Check that the rpt doesn't violate its invariants
//
	{

	if (TAgnRpt::InvariantL()==KErrNone && NumDaysSet() > 0)
		return (KErrNone);

	return (EAgmErrBadRepeat);
	}



void TAgnMonthlyByDaysRpt::InternalizeL(RReadStream& aStream)
/** Internalises the monthly by days repeat object from a read stream.

@internalComponent
@param aStream Stream from which the object should be internalised. */
	{
	
	TAgnRpt::InternalizeL(aStream);
	aStream >> iMonthlyRptDays[0];
	aStream >> iMonthlyRptDays[1];
	aStream >> iMonthlyRptDays[2];
	aStream >> iMonthlyRptDays[3];
	aStream >> iMonthlyRptDays[4];
	}

void TAgnMonthlyByDaysRpt::ExternalizeL(RWriteStream& aStream) const
/** Externalises the monthly by days repeat object to a write stream.

@internalComponent
@param aStream Stream to which the object should be externalised. */
	{

	TAgnRpt::ExternalizeL(aStream);
	aStream << iMonthlyRptDays[0];
	aStream << iMonthlyRptDays[1];
	aStream << iMonthlyRptDays[2];
	aStream << iMonthlyRptDays[3];
	aStream << iMonthlyRptDays[4];
	}

TBool TAgnMonthlyByDaysRpt::operator==(const TAgnMonthlyByDaysRpt& aRRule) const
/** Compares two monthly by day repeat objects for equality.

@internalComponent
@param aRRule The object to be compared.
@return True if all repeat details agree, otherwise False. */
	{

	return (TAgnRpt::operator==(aRRule) 
			&& iMonthlyRptDays[0]==aRRule.iMonthlyRptDays[0]
			&& iMonthlyRptDays[1]==aRRule.iMonthlyRptDays[1]
			&& iMonthlyRptDays[2]==aRRule.iMonthlyRptDays[2]
			&& iMonthlyRptDays[3]==aRRule.iMonthlyRptDays[3]
			&& iMonthlyRptDays[4]==aRRule.iMonthlyRptDays[4]);
	}

TBool TAgnMonthlyByDaysRpt::IsAlignedUtcL(const TTime& aDate) const
	{
	TTime localDate = ConvertFromUtcToRepeatLocalL(aDate);
	return IsAlignedRptLocalL(localDate);
	}
	
TBool TAgnMonthlyByDaysRpt::IsAlignedRptLocalL(const TTime& aDate) const
//
// Returns true if aDate falls on an instance generated by the repeat algorithm
// (ignores repeat start/end dates)
//
	{

	TDateTime dateTime=aDate.DateTime();

	TInt weekNo=dateTime.Day()/7;
	TDay day = aDate.DayNoInWeek();

	// First check the first 4 weeks
	if (weekNo < 4 && IsDaySet(day,(TWeekInMonth)weekNo)) 
			return (ETrue);

	// Now check if aDate is in the last week of the month
	if ((dateTime.Day() >= Time::DaysInMonth(dateTime.Year(),dateTime.Month()) - 7)  && (IsDaySet(day,ELast)))
		{
		if (HourAndMinutesDifferentFromStartTimeL(aDate))
			{
			return EFalse;
			}
		
		return (ETrue);
		}
	
	return (EFalse);
	}


TTimeIntervalDays TAgnMonthlyByDaysRpt::NumDaysToNextInstanceFrom(const TTime& aDay) const 
//
// Calculates the number of days between aDay and the *next* day which is set, if there is no
// next day before the end of the month then 0 is returned.
// The calculation proceeds by first determining which week of the month aDay+1 is in and then
// checking the remainder of the days of that week, if an instance cannot be found then all days of
// any of the remaining first 4 weeks are checked, if an instance still hasn't been found than the last
// week is checked - this requires skipping backwards as the 4th and last weeks overlap and may coincide
// exactly in the case ofFebruary in a non-leap year. Further explanations of this are given later on
// nearer to the code.
// i.e. suppose aDay is Sunday of the 2nd week and the 1st day of the week is a Friday and 
// there are 31 days in the month:
//	F S S M T W T    F S S M T W T    F S S M T W T    F S S M T W T   F S S    
//						   S                           <---4th week->
//                                                           <--last week--->
// We start by checking in the 2nd week from Monday onwards until thursday is reached. If an instance
// hasn't been found then we move on to the 3rd week, checking from a Friday onwards until Thursday,
// if an instance hasn't been found the process is repeated with the 4th week. If an instance still hasn't
// been found then we must check the last week which means jumping backwards to what is effectively monday
// of the 4th week.

	{

	static const TInt startDayOfMonthFactor[8] = {EMonday,ESunday,ESaturday,EFriday,EThursday,EWednesday,ETuesday};
	
	TInt nextDayFromTheParam = (aDay.DayNoInWeek() + 1) % 7; // checking is exclusive of aDay
	TDateTime dateTime = aDay.DateTime();
	if (dateTime.Day() + 1 == Time::DaysInMonth(dateTime.Year(),dateTime.Month()))
		return (0); // we're on the last day of the month

	TDateTime temp(dateTime);
	temp.SetDay(0);
	TTime temp2(temp);
	TDay dayAtStartOfMonth = temp2.DayNoInWeek();


	// find out what week we're in:
	TInt weekNo = (dateTime.Day() + 1) / 7;
	
	TInt numDaysToNextInstance = 1;
	TInt currentDay = nextDayFromTheParam;
	TInt numDaysInWeekToCheck;
	TInt lowestNumDaysToNextInstanceSoFar = 0;
	// the current week we're in may need partial checking
	if (weekNo < 4)
		{
		numDaysInWeekToCheck = 7 - ((currentDay + startDayOfMonthFactor[dayAtStartOfMonth]) % 7);					
		for (; numDaysInWeekToCheck > 0; --numDaysInWeekToCheck)
			{
			if (IsDaySet((TDay)currentDay,(TWeekInMonth)weekNo))
				{
				if (weekNo == 3) // the 4th week - don't return as this may be bettered by the last week - see later explanation
					{
					if (!lowestNumDaysToNextInstanceSoFar)
						lowestNumDaysToNextInstanceSoFar = numDaysToNextInstance;
					goto doincrement;
					}
				else
					return (numDaysToNextInstance);
				}
doincrement:
			++numDaysToNextInstance;
			if (currentDay == (TInt)ESunday)
				currentDay = (TInt)EMonday;
			else
				++currentDay;
			}			
		}
	++weekNo;

	// while subsequent weeks need full checking
	currentDay = dayAtStartOfMonth;
	for (numDaysInWeekToCheck = 7; weekNo < 4;)
		{
		if (IsDaySet((TDay)currentDay,TWeekInMonth(weekNo)))
			{
			if (weekNo == 3) // the 4th week - don't return as this may be bettered by the last week - see later explanation
				{
				if (!lowestNumDaysToNextInstanceSoFar)
					lowestNumDaysToNextInstanceSoFar = numDaysToNextInstance;
				goto increment;
				}
			else
				return (numDaysToNextInstance);
			}
		else
increment:
			{
			++numDaysToNextInstance;
			if (currentDay == (TInt)ESunday)
				currentDay = (TInt)EMonday;
			else
				++currentDay;
			if (numDaysInWeekToCheck == 1)
				{
				numDaysInWeekToCheck = 7;
				++weekNo;
				}
			else
				--numDaysInWeekToCheck;
			}
		}

	
	// if the 1st four weeks have been checked and nothing has been found or if
	// we were in the last week to begin with then check the last week

	// As an example consider the end of January 1996 with Thursday of the last week being set ...
	// 22 23 24 25 26 27 28 29 30 31
	// M  T  W  T  F  S  S  M  T  W  
	// <------4th week------>
	//       <-------last week----->
	//
	// So the 25th of Jan is an instance, suppose we have just checked the 4th week and 
	// having found no instances are currently positioned on the 29th. We now need to go
	// back and check the last week, hence we need to jump to the 24th and check from there
	// onwards until we find the 25th - consequently the numDaysToNextInstance needs decrementing
	// 5 i.e the difference between 29 and 24.
	// A complication arises if the search started from i.e. the 27th because in this case if
	// we jump all the way back to the 24th to start the search it would wrongly flag the 25
	// as being the *next* instance from where the search starts from - this needs to be taken
	// into consideration so that when we are jumping backwards from the 29th we only jump back
	// as far as the 27 and hence just check the last 5 days in the last week and not all 7 of them
	//
	// Consider the following for another complication - in this case the 4th and last weeks coincide
	// (this complication isn't limited to this case though)
	// days set in 4th week:	|         | |
	//							F S S M T W T
	// days set in last week:   | |         |
	// In this case, if checking is from Saturday onwards then when the 4th week is being checked it will
	// find Wednesday as the next occurrence - but the *next* occurrence is Saturday (of the last week). So
	// Wednesday needs to be remembered in case it can't be bettered upon

	TInt daysInMonth = Time::DaysInMonth(dateTime.Year(),dateTime.Month());
	TInt dayNumOfTheParam = dateTime.Day() + 1;
	TInt dayNumOf1stDayOfLastWeek = daysInMonth - 7;
	TInt diffBetween1stDayOfLastWeekAndParam = 0;
	if (dayNumOf1stDayOfLastWeek <= dayNumOfTheParam)
		diffBetween1stDayOfLastWeekAndParam = dayNumOfTheParam - dayNumOf1stDayOfLastWeek;		

	numDaysToNextInstance -= (7 - (daysInMonth - 28));
	numDaysToNextInstance += diffBetween1stDayOfLastWeekAndParam; 
	currentDay = ((TInt)dayAtStartOfMonth + daysInMonth - 7 + diffBetween1stDayOfLastWeekAndParam) % 7; // first day of last week is...
	numDaysInWeekToCheck = 7 - diffBetween1stDayOfLastWeekAndParam;
	while (numDaysInWeekToCheck > 0)
		{
		if (IsDaySet((TDay)currentDay,ELast))
			{
			if (lowestNumDaysToNextInstanceSoFar && lowestNumDaysToNextInstanceSoFar < numDaysToNextInstance)
				numDaysToNextInstance = lowestNumDaysToNextInstanceSoFar;
			return (numDaysToNextInstance);
			}
		
		if (currentDay == (TInt)ESunday)
			currentDay = (TInt)EMonday;
		else
			++currentDay;
		--numDaysInWeekToCheck;
		++numDaysToNextInstance;
		}

	return (lowestNumDaysToNextInstanceSoFar);
	}

/*
 * Produces the week to check from a specified TTime (Repeat Local).
 *
 * @param	"const TTime& aTime"
 *			The date to check
 * @return "TWeekInMonth"
 *			The Week to check. If this week is EFourth, you should also check
 *			the last week ( ELast ).
 */
TAgnRpt::TWeekInMonth TAgnMonthlyByDaysRpt::GetWeekToCheckRptLocal(const TTime& aTime) const
	{
	return static_cast<TAgnRpt::TWeekInMonth>( aTime.DayNoInMonth() / 7 );
	}

TBool TAgnMonthlyByDaysRpt::CheckLastWeekRptLocal(const TTime& aTime) const
	{
	TInt endOfMonth = aTime.DaysInMonth();
	TInt currentDay = aTime.DayNoInMonth();
	TInt startOfLastWeek = endOfMonth - 7;
	return ( currentDay >= startOfLastWeek );
	}


/*
 * Returns true, if the specified TTime is a day with an event on it.
 *
 * @param	"const TTime& aTime"
 *			The date to check (Repeat Local)
 * @return  "TBool"
 *			ETrue if an event occurs on this date
 *			EFalse if not.
 */
TBool TAgnMonthlyByDaysRpt::EventOnDateRptLocal( const TTime& aTime ) const
	{
	TAgnRpt::TWeekInMonth weekInMonth = GetWeekToCheckRptLocal( aTime );
	TDay dayOfWeek = aTime.DayNoInWeek();
	TBool retval = EFalse;

	retval = IsDaySet( dayOfWeek, weekInMonth );

	if (!retval && weekInMonth == TAgnRpt::EFourth && CheckLastWeekRptLocal(aTime))
		{
		retval = IsDaySet( dayOfWeek, TAgnRpt::ELast );
		}

	return retval;
	}


/*
 * Find the last event on the specified month.
 *
 *
 * @return  "TTime"
 *			The date of the last event (Repeat Local).
 *
 */
TTime TAgnMonthlyByDaysRpt::FindLastEventOnSpecifiedMonthRptLocal(const TTime& aTime) const
	{
	TTime dateCounter = aTime;
	TTime retval = aTime;
	TDateTime dateTime;
	const TInt KEndDayOfMonth = aTime.DaysInMonth(); // last day of month,..
	const TInt KStartDayInMonth = aTime.DayNoInMonth(); // curent starting day in month
	TInt counter = 0;

	for (counter = KEndDayOfMonth; counter > KStartDayInMonth; counter--)
		{
		dateTime = dateCounter.DateTime();
		dateTime.SetDay( counter );
		dateCounter = dateTime;
		if ( EventOnDateRptLocal( dateCounter ) )
			{
			retval = dateCounter;
			break;
			}
		}

	return retval;
	}
	
// ------------------------------- TAgnMonthlyByDatesRpt ------------------------------------------------------

EXPORT_C TAgnMonthlyByDatesRpt::TAgnMonthlyByDatesRpt(CAgnRptDef& aOwningRptDef) : 
	TAgnMonthlyRpt(aOwningRptDef)
/** Constructs a monthly repeat by dates object. 

All monthly repeat dates are cleared. 

@internalComponent
*/
	{
	iMonthlyRptDates = 0;
	}

EXPORT_C TAgnMonthlyByDatesRpt::TAgnMonthlyByDatesRpt(const TAgnMonthlyByDatesRpt& aRRule, CAgnRptDef& aOwningRptDef) : 
	TAgnMonthlyRpt(aRRule, aOwningRptDef)
/** Constructs a new TAgnMonthlyByDatesRpt object from an existing one.

@internalComponent
@param aRRule The object to be copied. */
	{
	iMonthlyRptDates = aRRule.iMonthlyRptDates; 
	}

TAgnRpt::TType TAgnMonthlyByDatesRpt::Type() const
	{
	return TAgnRpt::EMonthlyByDates;
	}
	
EXPORT_C void TAgnMonthlyByDatesRpt::SetDate(TUint aDateInMonth)
/** Sets the date specified.

@internalComponent
@param aDateInMonth The date to be added, (between 0 and 30 inclusive). */
	{

	iMonthlyRptDates |= MapDateToBits(aDateInMonth);
	}

EXPORT_C TBool TAgnMonthlyByDatesRpt::IsDateSet(TUint aDateInMonth) const
/** Tests whether the supplied date is set.

@internalComponent
@param aDateInMonth The date to be tested for (between 0 and 30 inclusive).
@return True if set, False if not set. */
	{

	return (iMonthlyRptDates & (MapDateToBits(aDateInMonth))); 
	}


TInt TAgnMonthlyByDatesRpt::NumDatesSet() const
/** Gets the number of repeat dates in the month which are set.

@internalComponent
@return The number of dates set. */
	{
	TInt count = 0;
	for (TUint i = 0; i <= 30; ++i) // check each date in a month
		{
		if (IsDateSet(i))
			{
			++count;
			}
		}

	return (count);
	}

TInt TAgnMonthlyByDatesRpt::NumInstancesPerMonth() const
	{
	return (NumDatesSet());
	}

TInt TAgnMonthlyByDatesRpt::InvariantL() const
//
// Check that the rpt doesn't violate its invariants
//
	{
	if (TAgnRpt::InvariantL()==KErrNone && NumDatesSet() > 0)
		return (KErrNone);

	return (EAgmErrBadRepeat);
	}



void TAgnMonthlyByDatesRpt::InternalizeL(RReadStream& aStream)
/** Internalises the monthly by date repeat object from a read stream.

@internalComponent
@param aStream Stream from which the object should be internalised. */
	{

	TAgnRpt::InternalizeL(aStream);		
	aStream >> iMonthlyRptDates;
	}

void TAgnMonthlyByDatesRpt::ExternalizeL(RWriteStream& aStream) const
/** Externalises the monthly by date repeat object to a write stream.

@internalComponent
@param aStream Stream to which the object should be externalised. */
	{

	TAgnRpt::ExternalizeL(aStream);
	aStream << iMonthlyRptDates;
	}

TBool TAgnMonthlyByDatesRpt::operator==(const TAgnMonthlyByDatesRpt& aRRule) const
/** Compares two monthly by date repeat objects for equality.

@internalComponent
@param aRRule The object to be compared.
@return True if all repeat details agree, otherwise False. */
	{

	return (TAgnRpt::operator==(aRRule) && iMonthlyRptDates==aRRule.iMonthlyRptDates);
	} 
			
TBool TAgnMonthlyByDatesRpt::IsAlignedUtcL(const TTime& aDate) const
//
// Returns true is aDate (UTC) falls on a date generated by the repeat algorithm
// (ignores repeat start/end dates)
//
	{
	TTime localDate = ConvertFromUtcToRepeatLocalL(aDate);
	return IsAlignedRptLocalL(localDate);
	}
	
TBool TAgnMonthlyByDatesRpt::IsAlignedRptLocalL(const TTime& aDate) const
//
// Returns true is aDate (Repeat Local Time) falls on a date generated by the repeat algorithm
// (ignores repeat start/end dates)
//
	{
	TDateTime date = aDate.DateTime();
	
	TUint dayBit = MapDateToBits(date.Day());
	// If the date being checked is, for example, the 30th April and the repeat
	// is set to repeat on the 31st of the month then this is still an aligned
	// date so the high order bits in dayBit need setting to catch this i.e.
	// if 30th april = 0001000... then this needs setting to 11110000
	if (date.Day() == Time::DaysInMonth(date.Year(),date.Month())-1)
		{
		dayBit =~ (dayBit-1); 
		}

	if ( ! (dayBit & iMonthlyRptDates))
		{
		return EFalse;
		}

	if (HourAndMinutesDifferentFromStartTimeL(aDate))
		{
		return EFalse;
		}
	
	return ETrue;
	}


TTimeIntervalDays TAgnMonthlyByDatesRpt::NumDaysToNextInstanceFrom(const TTime& aDate) const
//
// Calculates the number of days between aDate and the *next* date which is set i.e. if
// the date in aDate is the 2nd of the month, and if the repeat is set to repeat on the
// 5th and 10th of the month then 3 is returned. If there is no subsequent date set then
// 0 is returned.
//
	{
	// Assumes aDate is expressed in Repeat Local Time
		
	TDateTime dateTime = aDate.DateTime();
	TUint daysInMonth = Time::DaysInMonth(dateTime.Year(),dateTime.Month());
	TUint dayInMonth = dateTime.Day() + 1; 
	for (TInt num=1; dayInMonth < 31;++dayInMonth)
		{
		if (IsDateSet(dayInMonth))
			return (num);
		if (dayInMonth < daysInMonth-1)
		// If the date being checked is, for example, the 30th April and the repeat
		// is set to repeat on the 31st of the month then the number to return is the
		// difference between 30 and aDate not 31 and aDate - hence num should not be
		// incremented in this case
			++num;
		}
	return (0);
	}


/*
 * Find the last event on the first month if the rule begins.
 *
 *
 * @return  "TTime" (Repeat Local)
 *			The date of the last event.
 *
 */
TTime TAgnMonthlyByDatesRpt::FindLastEventOnSpecifiedMonthRptLocal(const TTime& aTime) const
	{
	const TInt KEndDayOfMonth = aTime.DaysInMonth() - 1; // zero initialised
	TTime retval = aTime;
	TInt counter = 0;
	TDateTime lastDay;

	for (counter = KEndDayOfMonth; counter >= 0; counter--)
		{
		if ( IsDateSet( counter ) )
			{
			lastDay = retval.DateTime();
			lastDay.SetDay( counter );
			retval = lastDay;
			break;
			}
		}

	return retval;
	}

// ------------------------------ TAgnYearlyByDateRpt ----------------------------
EXPORT_C TAgnYearlyByDateRpt::TAgnYearlyByDateRpt(CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aOwningRptDef)
/**
This is not inline to prevent non-exportable functions causing a linker error in calling code

@internalComponent
*/
	{}

EXPORT_C TAgnYearlyByDateRpt::TAgnYearlyByDateRpt(const TAgnYearlyByDateRpt& aRRule, CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aRRule, aOwningRptDef)
/** Constructs a new TAgnYearlyByDateRpt object from an existing one.

@internalComponent
@param aRRule The object to be copied. */
	{}

TAgnRpt::TType TAgnYearlyByDateRpt::Type() const
	{
	return TAgnRpt::EYearlyByDate;
	}

TUint TAgnYearlyByDateRpt::InstanceCountL() const
/** Gets the number of instances generated by the repeat algorithm.

@internalComponent
@return The number of instances. */
	{
	// Years from start to end divided by the interval
	// Add one for the first year: if start and end are in the same year,
	// we must return 1.
	return ( 1 + UntilTimeAsRptLocalL().YearsFrom(StartTimeAsRptLocalL()).Int()/iInterval );
	}
                                        
TBool TAgnYearlyByDateRpt::IsAlignedRptLocalL(const TTime& aDate) const
//
// Returns true if aDate (Repeat Local Time) falls on an instance generated by the repeat algorithm
// (ignores repeat start/end dates)
//
	{
	// All date and time comparisons must be done in Repeat Local
		
	if (StartTimeAsRptLocalL() == aDate)
		return (ETrue);
	TDateTime startDateTime = StartTimeAsRptLocalL().DateTime();
	TDateTime thisDateTime = aDate.DateTime();
	if ( (thisDateTime.Year()-startDateTime.Year() ) % iInterval)
		return (EFalse);
	if (startDateTime.Month()!=thisDateTime.Month())
		return (EFalse);
	if (startDateTime.Day()!=thisDateTime.Day())
		{		
	// finally check for 28/29th February
		if (startDateTime.Month()!=EFebruary)
			return (EFalse);
		if (startDateTime.Day()!=28)
			return (EFalse);
		if (thisDateTime.Day()!=Time::DaysInMonth(thisDateTime.Year(),thisDateTime.Month())-1)
			return (EFalse);
		}
		
	if (HourAndMinutesDifferentFromStartTimeL(aDate))
		{
		return EFalse;
		}
	
	return ETrue;
	}

TBool TAgnYearlyByDateRpt::IsAlignedUtcL(const TTime& aUtcDate) const
//
// Returns true if aDate (UTC) falls on an instance generated by the repeat algorithm
// (ignores repeat start/end dates)
//
	{
	// convert aDate from UTC to Repeat Local
	TTime rptLocalDate = ConvertFromUtcToRepeatLocalL(aUtcDate);	
	return IsAlignedRptLocalL(rptLocalDate);
	}

TTime TAgnYearlyByDateRpt::FindRptUntilTimeRptLocalL(TUint aCount) const
	{
	TTime end = StartTimeAsRptLocalL();
	TUint max = (AgnDateTime::MaxDate().YearsFrom(end).Int() + 1) / iInterval;
	if (aCount > max)
		{
		end = AgnDateTime::MaxDate();
		}
	else
		{
		end += TTimeIntervalYears((aCount - 1) * iInterval);	
		}
	return end;
	}

TTime TAgnYearlyByDateRpt::NudgeNextInstanceUtcL(const TTime& aDate) const
//
// If aDate (UTC) is already a date generated by the repeat algorithm then that is the date returned,
// otherwise the next instance from aDate is returned 
//
	{
	TTime rptLocalDate = ConvertFromUtcToRepeatLocalL(aDate);
	rptLocalDate = NudgeNextInstanceRptLocalL(rptLocalDate);
	return ConvertFromRepeatLocalToUtcL(rptLocalDate);
	}
	
TTime TAgnYearlyByDateRpt::NudgeNextInstanceRptLocalL(const TTime& aDate) const
//
// If aDate (Repeat Local Time) is already a date generated by the repeat algorithm then that is the date returned,
// otherwise the next instance from aDate is returned 
//
	{
	TTime next = NudgeNextAlignedStartTimeL(aDate);
	
	if (IsAlignedRptLocalL(next))
		return (next);
	
	TDateTime thisTDateTime=next.DateTime();
	TDateTime startTDateTime=StartTimeAsRptLocalL().DateTime();

	TTimeIntervalYears years=AgnDateTime::YearsFrom(startTDateTime,thisTDateTime);
	// Add a year, since the next interval is at least one year away
	// This fixes the 'yearly repeating entries on a leap year don't appear
	// on the 29th Feb' bug
	years = (years.Int() +1);
	years=((years.Int()+Interval()-1)/Interval())*Interval();
	AgnDateTime::AddTo(startTDateTime,years);

	if (AgnDateTime::IsLessThan(startTDateTime,thisTDateTime))
		AgnDateTime::AddTo(startTDateTime,TTimeIntervalYears(Interval()));

	TTime date = TTime(startTDateTime);
	if ( date > AgnDateTime::MaxDate())
		{
		return (Time::NullTTime());
		}
	
	return date;
	}

TTime TAgnYearlyByDateRpt::NudgePreviousInstanceUtcL(const TTime& aDate) const
//
// If aDate (UTC) is already a date generated by the repeat algorithm then that is the date returned, 
// otherwise the previous instance is returned 
//
	{
	TTime rptLocalDate = ConvertFromUtcToRepeatLocalL(aDate);
	rptLocalDate = NudgePreviousInstanceRptLocalL(rptLocalDate);
	return ConvertFromRepeatLocalToUtcL(rptLocalDate);
	}

TTime TAgnYearlyByDateRpt::NudgePreviousInstanceRptLocalL(const TTime& aDate) const
//
// If aDate (Repeat Local Time) is already a date generated by the repeat algorithm then that is the date returned, 
// otherwise the previous instance is returned 
//
	{
	TTime prev = NudgePreviousAlignedStartTimeL(aDate);
	
	if (IsAlignedRptLocalL(prev))
		return (prev);

	TDateTime thisTDateTime = prev.DateTime();
	TDateTime startTDateTime = StartTimeAsRptLocalL().DateTime();

	TTimeIntervalYears years=AgnDateTime::YearsFrom(startTDateTime,thisTDateTime);

	if (years.Int()>=(TInt)Interval())
		{
		years=(years.Int()/Interval())*Interval();
		AgnDateTime::AddTo(startTDateTime,years);
		}

	TTime date = TTime(startTDateTime);
	if (date < AgnDateTime::MinDate())
		{
		return (Time::NullTTime());
		}

	return date;
	}


// ------------------------------ TAgnYearlyByDayRpt ----------------------------
EXPORT_C TAgnYearlyByDayRpt::TAgnYearlyByDayRpt(CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aOwningRptDef)
/**
This is not inline to prevent non-exportable functions causing a linker error in calling code

@internalComponent
*/
	{
	iWeekInMonth = static_cast<TAgnRpt::TWeekInMonth>(0);
	iDay = static_cast<TDay>(0);
	}

EXPORT_C TAgnYearlyByDayRpt::TAgnYearlyByDayRpt(const TAgnYearlyByDayRpt& aRRule, CAgnRptDef& aOwningRptDef) : 
	TAgnRpt(aRRule, aOwningRptDef)
/** Constructs a new TAgnYearlyByDayRpt object from an existing one.

@internalComponent
@param aRRule The object to be copied. */
	{
	iWeekInMonth = aRRule.iWeekInMonth;
	iDay = aRRule.iDay;
	}

TAgnRpt::TType TAgnYearlyByDayRpt::Type() const
	{
	return TAgnRpt::EYearlyByDay;
	}
	
TUint TAgnYearlyByDayRpt::InstanceCountL() const
/** Gets the number of yearly repeat instances.

@internalComponent
@return The number of instances. */
 	{

	TTime UntilTime = UntilTimeAsRptLocalL();
	TTime strtDate= StartTimeAsRptLocalL();
	 for (TInt count = 1; count <= 7; ++count)
		{
		if (UntilTime.DayNoInWeek()==iDay)
			break;
		else
			UntilTime -= TTimeIntervalDays(1);
		}
       
   	return 1 + ( (UntilTime.DateTime().Year() - strtDate.DateTime().Year()) /Interval()); 
	}

TBool TAgnYearlyByDayRpt::IsAlignedUtcL(const TTime& aDate) const
//
// Returns true if aDate (UTC) falls on an instance generated by the repeat algorithm
// (ignores repeat start/end dates)
//
	{
	TTime rptLocalDate = ConvertFromUtcToRepeatLocalL(aDate);
	return IsAlignedRptLocalL(rptLocalDate);
	}
	
TBool TAgnYearlyByDayRpt::IsAlignedRptLocalL(const TTime& aDate) const
//
// Returns true if aDate (Repeat Local Time) falls on an instance generated by the repeat algorithm
// (ignores repeat start/end dates)
//
	{

	if (StartTimeAsRptLocalL() == aDate)
		return (ETrue);

	TDateTime startDateTime = StartTimeAsRptLocalL().DateTime();
	TDateTime thisDateTime = aDate.DateTime();
	if ((thisDateTime.Year()-startDateTime.Year())%Interval())
		return (EFalse);

	if (startDateTime.Month()!=thisDateTime.Month())
		return (EFalse);
	
	if (aDate.DayNoInWeek()!=iDay)
		return (EFalse);

	TInt dayInMonth = aDate.DayNoInMonth();
	switch (iWeekInMonth)
		{
		case EFirst:
			{
			if (dayInMonth > 6)
				return (EFalse);
			break;
			}
		case ESecond:
			{
			if (dayInMonth < 7 || dayInMonth > 13)
				return (EFalse);
			break;
			}
		case EThird:
			{
			if (dayInMonth < 14 || dayInMonth > 20)
				return (EFalse);
			break;
			}
		case EFourth:
			{
			if (dayInMonth < 21 || dayInMonth > 27)
				return (EFalse);
			break;
			}
		case ELast:
			{
			if (dayInMonth < aDate.DaysInMonth() - 7)
				return (EFalse);
			break;
			}
		default: 
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("TAgnYearlyByDayRpt: Panic - EAgmErrInvalidWeekNumber");)
			DBG_PANIC(EAgmErrInvalidWeekNumber);
			}
		}

	if (HourAndMinutesDifferentFromStartTimeL(aDate))
		{
		return EFalse;
		}
	
	return (ETrue);
	}

TTime TAgnYearlyByDayRpt::FindRptUntilTimeRptLocalL(TUint aCount) const
	{
	// do all calculations in Repeat Local times, and convert to UTC before returning
	
	TUint max = AgnDateTime::MaxDate().YearsFrom(StartTimeAsRptLocalL()).Int();
	
	TUint count = aCount > max ? max : aCount;

	TTime end=StartTimeAsRptLocalL();
	end+=TTimeIntervalYears((count-1)*Interval());

	TDateTime dt = end.DateTime();
	TInt firstDay = GetFirstDayInWeekRptLocal(iWeekInMonth,dt.Month(),dt.Year());
	dt.SetDay(firstDay);
	end = dt;
	SetDateAccordingToDayRptLocal(end,iDay,iWeekInMonth);

	return end;
	}

TTime TAgnYearlyByDayRpt::NudgeNextInstanceUtcL(const TTime& aDate) const
//
// If aDate (UTC) is already a date generated by the repeat algorithm then that is the date returned,
// otherwise the next instance from aDate is returned.
// Complication:
// Suppose an entry starts on the 29 th of a month in year X
// 29 X ....	27 28 Y .....	26 Z
// I            I  a			I
// if aDate is 28th Y then because 26 Z is less than a year from a the calculation will be in
// error unless  a is advanced to compensate.
//
	{
	TTime rptLocalDate = ConvertFromUtcToRepeatLocalL(aDate);
	rptLocalDate = NudgeNextInstanceRptLocalL(rptLocalDate);
	return ConvertFromRepeatLocalToUtcL(rptLocalDate);
	}
	
TTime TAgnYearlyByDayRpt::NudgeNextInstanceRptLocalL(TTime aDate) const
//
// If aDate (Repeat Local Time) is already a date generated by the repeat algorithm then that is the date returned,
// otherwise the next instance from aDate is returned.
// Complication:
// Suppose an entry starts on the 29 th of a month in year X
// 29 X ....	27 28 Y .....	26 Z
// I            I  a			I
// if aDate is 28th Y then because 26 Z is less than a year from a the calculation will be in
// error unless  a is advanced to compensate.
//
	{
	TTime ttime = NudgeNextAlignedStartTimeL(aDate);
	
	for (TInt count = 1; count <= 7; ++count)
		{
		if (ttime.DayNoInWeek()==iDay)
			break;
		else
			ttime += TTimeIntervalDays(1);
		}

	if (ttime > AgnDateTime::MaxDate()) 
		return (Time::NullTTime());

	aDate = ttime;
	if (IsAlignedRptLocalL(aDate))
		return (aDate);

	TDateTime thisTDateTime = aDate.DateTime();
	TDateTime startTDateTime = StartTimeAsRptLocalL().DateTime();

	TTimeIntervalYears years=AgnDateTime::YearsFrom(startTDateTime,thisTDateTime);
	years=((years.Int()+Interval()-1)/Interval())*Interval();
	AgnDateTime::AddTo(startTDateTime,years);

	if (AgnDateTime::IsLessThan(startTDateTime,thisTDateTime))
		AgnDateTime::AddTo(startTDateTime,TTimeIntervalYears(Interval()));

	TInt firstDay = GetFirstDayInWeekRptLocal(iWeekInMonth,startTDateTime.Month(),startTDateTime.Year());
	startTDateTime.SetDay(firstDay);
	ttime = startTDateTime;
	SetDateAccordingToDayRptLocal(ttime,iDay,iWeekInMonth);

	if (ttime > AgnDateTime::MaxDate())
		return (Time::NullTTime());

	return ttime;
	}

TTime TAgnYearlyByDayRpt::NudgePreviousInstanceUtcL(const TTime& aDate) const
//
// If aDate (UTC) is already a date generated by the repeat algorithm then that is the date returned, 
// otherwise the previous instance is returned 
//
// Complication:
// Suppose the repeat starts on the 1st of January year X and an instance is on 5th January year Y
//		1 X ........................1 2 3 4 5 6 7 8 Y
//		s							a	b	I	  d
// Now if bis choosen as the point to nudge from there is no problem, but suppose b is the date
// to nudge from then if the number of years between b and s is calculated the it will be 1 too many
// because a is more than 365 day after s (assuming Y = X + 1). So if aDate is b then it needs to be
// reduced to < a to find the correct previous instance.
	{
	TTime rptLocalDate = ConvertFromUtcToRepeatLocalL(aDate);
	rptLocalDate = NudgePreviousInstanceRptLocalL(rptLocalDate);
	return ConvertFromRepeatLocalToUtcL(rptLocalDate);
	}

TTime TAgnYearlyByDayRpt::NudgePreviousInstanceRptLocalL(TTime aDate) const
//
// If aDate (Repeat Local Time) is already a date generated by the repeat algorithm then that is the date returned, 
// otherwise the previous instance is returned 
//
// Complication:
// Suppose the repeat starts on the 1st of January year X and an instance is on 5th January year Y
//		1 X ........................1 2 3 4 5 6 7 8 Y
//		s							a	b	I	  d
// Now if bis choosen as the point to nudge from there is no problem, but suppose b is the date
// to nudge from then if the number of years between b and s is calculated the it will be 1 too many
// because a is more than 365 day after s (assuming Y = X + 1). So if aDate is b then it needs to be
// reduced to < a to find the correct previous instance.

	{

	TTime ttime = NudgePreviousAlignedStartTimeL(aDate);
	for (TInt count = 1; count <= 7; ++count)
		{
		if (ttime.DayNoInWeek()==iDay)
			break;
		else
			ttime -= TTimeIntervalDays(1);
		}

	aDate = ttime;
	if (IsAlignedRptLocalL(aDate))
		return (aDate);

	TDateTime thisTDateTime = aDate.DateTime();
	TDateTime startTDateTime = StartTimeAsRptLocalL().DateTime();

	TTimeIntervalYears years=AgnDateTime::YearsFrom(startTDateTime,thisTDateTime);

	if (years.Int()>=(TInt)Interval())
		{
		years=(years.Int()/Interval())*Interval();
		AgnDateTime::AddTo(startTDateTime,years);
		}

	TInt firstDay = GetFirstDayInWeekRptLocal(iWeekInMonth,startTDateTime.Month(),startTDateTime.Year());
	startTDateTime.SetDay(firstDay);
	ttime = startTDateTime;
	SetDateAccordingToDayRptLocal(ttime,iDay,iWeekInMonth);

	if (ttime < AgnDateTime::MinDate())
		return (Time::NullTTime());

	return ttime;
	}


TInt TAgnYearlyByDayRpt::GetFirstDayInWeekRptLocal(TWeekInMonth aWeekInMonth,TMonth aMonth, TInt aYear) const
//
// Return the day number for the first day in the week aWeekInMonth
//
	{

	TInt day=0;
	switch (aWeekInMonth)
		{
		case EFirst:	day= 0;	break;
		case ESecond:	day= 7;	break;
		case EThird:	day= 14; break;
		case EFourth:	day= 21; break;
		case ELast:		day= Time::DaysInMonth(aYear,aMonth) - 1; break;						
		default:
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("TAgnYearlyByDayRpt: Panic - EAgmErrInvalidWeekNumber");)
			DBG_PANIC(EAgmErrInvalidWeekNumber);
			}
		}
	return (day);
	}

void TAgnYearlyByDayRpt::SetDateAccordingToDayRptLocal(TTime& aDate,TDay aDay,TWeekInMonth aWeekInMonth) const
//
// Adjust aDate (Repeat Local Time) until its day of the week (i.e. tuesday) corresponds with aDay
//
	{

	for(TInt count = 1; count <= 7; ++count)
		{
		if (aDate.DayNoInWeek()==aDay)
			break;
		else
			{
			if (aWeekInMonth == ELast)
				aDate -= TTimeIntervalDays(1);
			else
				aDate += TTimeIntervalDays(1);
			}
		}
	}


EXPORT_C TTime TAgnYearlyByDayRpt::FindStartDayL(TDay aDay,TWeekInMonth aWeek,TMonth aMonth,TInt aYear)
/** Sets the repeat's start date using the details specified. These details are assumed to refer
to the time zone where the repeating activity takes place.

@internalComponent
@param aDay The start day.
@param aWeek The start week in the month.
@param aMonth The start month.
@param aYear The start year. */
	{

	iWeekInMonth = aWeek;
	iDay = aDay;
	TInt dayInMonth=GetFirstDayInWeekRptLocal(aWeek,aMonth,aYear);
	TDateTime startDT = StartTimeAsRptLocalL().DateTime();
	TTime date(TDateTime(aYear,aMonth,dayInMonth,startDT.Hour(),startDT.Minute(),startDT.Second(),startDT.MicroSecond()));	
	SetDateAccordingToDayRptLocal(date,aDay,aWeek);
	return date;
	}

EXPORT_C void TAgnYearlyByDayRpt::GetStartDayL(TDay& aDay,TWeekInMonth& aWeek,TMonth& aMonth,TInt& aYear) const
/** Gets the start day, week, month and year for the current start date. These details refer
to the time zone where the repeating entry takes place, not the time zone where the user is.

@internalComponent
@param aDay On return, the start day.
@param aWeek On return, the week in the month of the start date.
@param aMonth On return, the month of the start date.
@param aYear On return, the year of the start date. */
	{

	TTime date = StartTimeAsRptLocalL(); // Repeat Local
	aDay = date.DayNoInWeek();
	aWeek = iWeekInMonth;
	TDateTime dateTime = date.DateTime();
	aMonth = dateTime.Month();
	aYear = dateTime.Year();	
	}


void TAgnYearlyByDayRpt::InternalizeL(RReadStream& aStream)
/** Internalises the yearly by day repeat object from a read stream.

@internalComponent
@param aStream Stream from which the object should be internalised. */
	{
	TAgnRpt::InternalizeL(aStream);
	iWeekInMonth = TWeekInMonth(aStream.ReadUint8L());	
			
	iDay = TDay(aStream.ReadUint8L());
	}
	
void TAgnYearlyByDayRpt::ExternalizeL(RWriteStream& aStream) const
/** Externalises the yearly by day repeat object to a write stream.

@internalComponent
@param aStream Stream to which the object should be externalised. */
	{
	TAgnRpt::ExternalizeL(aStream);
	aStream.WriteUint8L(iWeekInMonth);
	aStream.WriteUint8L(iDay);
	}


TInt TAgnYearlyByDayRpt::InvariantL() const
//
// Check that the rpt doesn't violate its invariants
//
	{


	if (TAgnRpt::InvariantL()==KErrNone && (iWeekInMonth == EFirst || iWeekInMonth == ESecond
			|| iWeekInMonth == EThird || iWeekInMonth == EFourth || iWeekInMonth == ELast) &&
			(iDay == EMonday || iDay == ETuesday || iDay == EWednesday || iDay == EThursday ||
			iDay == EFriday || iDay == ESaturday || iDay == ESunday))
		return (KErrNone);

	return (EAgmErrBadRepeat);
	}


// --------------------------------------- CAgnRptDef -------------------------------

CAgnRptDef::CAgnRptDef(const CAgnSimpleEntry& aOwningEntry) :
	iOwningEntry(aOwningEntry)
	{
	}

/** Gets the first instance of the repeat as a TAgnCalendarTime.
This is either the first sporadic date or the start of the repeat rule, whichever is earlier.

@return The first instance of the repeat.
@internalComponent
*/
EXPORT_C const TAgnCalendarTime& CAgnRptDef::FirstInstanceL() const
	{
	if (!HasRepeatRule() && !HasSporadicDates())
		{
		User::Leave(KErrCorrupt);
		}
		
	if (HasRepeatRule() && !HasSporadicDates())
		{
		return StartTime(); 
		}
	else if (!HasRepeatRule() && HasSporadicDates())
		{
		return (*iSporadicDates)[0]; 
		}
	
	// has both rule and rdates	
	const TAgnCalendarTime& KRuleFirstDate = StartTime();   
	const TAgnCalendarTime& KRdateFirstDate = (*iSporadicDates)[0];  
	
	return KRuleFirstDate < KRdateFirstDate ? KRuleFirstDate : KRdateFirstDate;
	}

/** Gets the last instance of the repeat as a TAgnCalendarTime.
This is either the last sporadic date or the end of the repeat rule, whichever is latest.

@return The last instance of the repeat.
@internalComponent
*/
EXPORT_C const TAgnCalendarTime& CAgnRptDef::LastInstanceL() const
	{
	if (!HasRepeatRule() && !HasSporadicDates())
		{
		User::Leave(KErrCorrupt);
		}
	
	if (HasRepeatRule() && !HasSporadicDates())
		{
		return iRRule->UntilTimeL(); 
		}
	else if (!HasRepeatRule() && HasSporadicDates())
		{
		return (*iSporadicDates)[iSporadicDates->Count() - 1]; 
		}
	
	// has both rule and rdates	
	const TAgnCalendarTime& KRuleLastDate = iRRule->UntilTimeL();   
	const TAgnCalendarTime& KRdateLastDate = (*iSporadicDates)[iSporadicDates->Count() - 1]; 
	return KRuleLastDate > KRdateLastDate ? KRuleLastDate : KRdateLastDate;
	}

EXPORT_C void CAgnRptDef::SetRRuleL(const TAgnRpt& aRRule)
	{
	CreateRptObjectL(aRRule);
	}

/** Create a Repeat Rule of the appropriate repeat rule type operator.
@param aRRule  Repeat details. */
void CAgnRptDef::CreateRptObjectL(const TAgnRpt& aRRule)
	{
	delete iRRule;
	iRRule = NULL;
	
	switch (aRRule.Type())
		{
		case TAgnRpt::EDaily:			
			iRRule = new (ELeave) TAgnDailyRpt( static_cast<const TAgnDailyRpt&>(aRRule), *this );
			break;

		case TAgnRpt::EWeekly:			
			iRRule = new (ELeave) TAgnWeeklyRpt( static_cast<const TAgnWeeklyRpt&>(aRRule), *this );
			break;

		case TAgnRpt::EMonthlyByDays:	
			iRRule = new (ELeave) TAgnMonthlyByDaysRpt( static_cast<const TAgnMonthlyByDaysRpt&>(aRRule), *this );
			break;

		case TAgnRpt::EMonthlyByDates:	
			iRRule = new (ELeave) TAgnMonthlyByDatesRpt( static_cast<const TAgnMonthlyByDatesRpt&>(aRRule), *this );
			break;

		case TAgnRpt::EYearlyByDate:		
			iRRule = new (ELeave) TAgnYearlyByDateRpt( static_cast<const TAgnYearlyByDateRpt&>(aRRule), *this );
			break;

		case TAgnRpt::EYearlyByDay:		
			iRRule = new (ELeave) TAgnYearlyByDayRpt( static_cast<const TAgnYearlyByDayRpt&>(aRRule), *this );
			break;

		default:
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("CAgnRptDef: Panic - EAgmErrInvalidRptType");)
			DBG_PANIC(EAgmErrInvalidRptType);
			break;
			}	
		}
	}
	
void CAgnRptDef::CreateRptObjectL(TAgnRpt::TType aType)
//
// Creates an object of the appropriate repeat rule type
// operator.
//
	{
	delete iRRule;
	iRRule = NULL;

	switch (aType)
		{
		case TAgnRpt::EDaily:			
			iRRule = new (ELeave) TAgnDailyRpt(*this);
			break;
			
		case TAgnRpt::EWeekly:			
			iRRule = new (ELeave) TAgnWeeklyRpt(*this);
			break;
			
		case TAgnRpt::EMonthlyByDays:	
			iRRule = new (ELeave) TAgnMonthlyByDaysRpt(*this);
			break;
			
		case TAgnRpt::EMonthlyByDates:	
			iRRule = new (ELeave) TAgnMonthlyByDatesRpt(*this);
			break;
			
		case TAgnRpt::EYearlyByDate:		
			iRRule = new (ELeave) TAgnYearlyByDateRpt(*this);
			break;
			
		case TAgnRpt::EYearlyByDay:		
			iRRule = new (ELeave) TAgnYearlyByDayRpt(*this);
			break;
			
		default:
			{
			_DBGLOG_ENTRY(AgmDebug::DebugLog("CAgnRptDef: Panic - EAgmErrInvalidRptType");)
			DBG_PANIC(EAgmErrInvalidRptType);
			break;
			}
		}

	}

EXPORT_C CAgnRptDef* CAgnRptDef::NewL(const CAgnSimpleEntry& aOwningEntry)
/** Allocates and constructs a repeat definition.

@internalComponent
@return Pointer to the created repeat definition. */
	{
	return new (ELeave) CAgnRptDef(aOwningEntry);
	}

EXPORT_C CAgnRptDef::~CAgnRptDef()
/** Frees all resources owned by the repeat definition, prior to 
its destruction. 

@internalComponent
*/
	{
    if (iRRule)
        {
        delete iRRule;
        iRRule = NULL;
        }
    if (iTimeZone)
        {
        delete iTimeZone;
        iTimeZone = NULL;
        }

	ClearTimeArray(iSporadicDates);
	ClearTimeArray(iExceptions);
	}
	
EXPORT_C void CAgnRptDef::AddExceptionL(const TAgnCalendarTime& aException)
/** Adds an exception to the list of exceptions (first creating the exception list 
if necessary).

@internalComponent
@param aException The exception to add to the list. */
	{
	TAgnCalendarTime exceptionToAdd = aException;
	
	// MS Outlook exports exception dates without time information, i.e. time is reset to midnight.
	// Need to nudge to see if a date occurs on that day.
	if (!IsAnInstanceL(aException.LocalL()))
		{
		TTime exDateUtc = aException.UtcL();
		TTime actualInstanceTimeUtc;
		if (NudgeNextInstanceUtcL(exDateUtc, actualInstanceTimeUtc))
			{
			TTime exDateRepeatLocal = ConvertFromUtcToRepeatLocalL(exDateUtc);
			TTime actualInstanceTimeRepeatLocal = ConvertFromUtcToRepeatLocalL(actualInstanceTimeUtc);

			// Only change exception date if nudged date is the same date in repeat local time
			if (AgnDateTime::DaysBetweenDates(actualInstanceTimeRepeatLocal, exDateRepeatLocal) == 0)
				{
				if (aException.TimeMode() != MAgnCalendarTimeMode::EFloating)
					{
					exceptionToAdd.SetUtcL(actualInstanceTimeUtc);
					}
				else
					{
					exceptionToAdd.SetFloatingL(actualInstanceTimeRepeatLocal);
					}
				}
			}
		}
		
	if (IsAnInstanceL(exceptionToAdd.LocalL()))
		{
		EnsureTimeArrayExistsL(iExceptions);
		TAgnCalendarTime::InsertInOrderL(*iExceptions, exceptionToAdd);
		}
	}

EXPORT_C void CAgnRptDef::PruneExceptionsL()
/** Removes any exceptions which do not occur on instances
@internalComponent
*/
	{
	if (iExceptions)
		{
		// iterate through exceptions in reverse order so that removing exceptions doesn't upset the order
		for (TInt i = iExceptions->Count() - 1; i >= 0; --i)
			{
			const TAgnCalendarTime& KException = (*iExceptions)[i];
			if ( ! IsAnInstanceL(KException.LocalL()))
				{
				iExceptions->Remove(i);
				}
			}
		if (iExceptions->Count() == 0)
			{
			iExceptions->Close();
			delete iExceptions;
			iExceptions = NULL;
			}
		}
	}

EXPORT_C TBool CAgnRptDef::FindException(const TAgnCalendarTime& aException) const
/** Tests whether aException is in the exceptions list.

@internalComponent
@param aException The exception.
@return ETrue = present, otherwise EFalse. */
	{

	if (iExceptions)
		{
		if (iExceptions->Find(aException) != KErrNotFound)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

EXPORT_C void CAgnRptDef::ClearTimeArray(RArray<TAgnCalendarTime>*& aTimeArray)
	{
	if (aTimeArray)
		{
		aTimeArray->Reset();
		delete aTimeArray;
		aTimeArray = NULL;
		}
	}

EXPORT_C void CAgnRptDef::RemoveAllExceptions()
/** Removes all the exceptions from the exception list. 

@internalComponent
*/
	{
	ClearTimeArray(iExceptions);
	}

void CAgnRptDef::CopyTimeArrayL(const RArray<TAgnCalendarTime>* aSource, RArray<TAgnCalendarTime>*& aDestination)
	{
	ClearTimeArray(aDestination);
	
	if (aSource)
		{
		EnsureTimeArrayExistsL(aDestination);
		
		const TInt KNumTimes = aSource->Count();
		for (TInt i = 0; i < KNumTimes; ++i)
			{
			TAgnCalendarTime::InsertInOrderL(*aDestination, (*aSource)[i]);
			}
		}
	}
	
void CAgnRptDef::EnsureTimeArrayExistsL(RArray<TAgnCalendarTime>*& aTimeArray)
	{
	if ( ! aTimeArray)
		{
		aTimeArray = new (ELeave) RArray<TAgnCalendarTime>;
		}
	}
	
EXPORT_C void CAgnRptDef::AddSporadicDateL(const TAgnCalendarTime& aSporadicDate)
/** Adds a sporadic date to the list of sporadic dates (first creating the sporadic date list 
if necessary).

@internalComponent
@param aSporadicDate The sporadic date to add to the list. */
	{
	EnsureTimeArrayExistsL(iSporadicDates);
	TAgnCalendarTime::InsertInOrderL(*iSporadicDates, aSporadicDate);
	}

TInt CAgnRptDef::FindSporadicDate(const TAgnCalendarTime& aSporadicDate) const
/** Tests whether aSporadicDate is in the sporadic dates list, using the sporadic date 
as the find key.

@internalComponent
@param aSporadicDate The sporadic dates.
@return ETrue = present, otherwise EFalse. */
	{
	if (iSporadicDates)
		{
		return iSporadicDates->Find(aSporadicDate);
		}
	return KErrNotFound;
	}

void CAgnRptDef::RemoveSporadicDate(const TAgnCalendarTime& aSporadicDate)
/** Removes a sporadic date from the sporadic date list. 

If it is the last sporadic date in the list, the list is deleted.

@internalComponent
@param aSporadicDate The sporadic date to be removed.
@return ETrue if the sporadic date was found and removed, otherwise EFalse. */
	{

	if (iSporadicDates)
		{
		TInt pos = FindSporadicDate(aSporadicDate);
		if (pos != KErrNotFound)
			{
			iSporadicDates->Remove(pos);
			if (iSporadicDates->Count() == 0)
				{
				ClearTimeArray(iSporadicDates);
				}
			}
	   }
	}

EXPORT_C void CAgnRptDef::RemoveAllSporadicDates()
/** Removes all the sporadic dates from the sporadic date list. 

@internalComponent
*/
	{
	ClearTimeArray(iSporadicDates);
	}

EXPORT_C void CAgnRptDef::CopyL(const CAgnRptDef& aRptDef)
/** Copies the contents of a repeat definition into this object.

@internalComponent
@param aRRule Pointer to the item to be copied. */
	{
	if (aRptDef.HasRepeatRule())
		{
		CreateRptObjectL(*aRptDef.iRRule);		
		}

	CopyTimeArrayL(aRptDef.iExceptions, iExceptions);
	CopyTimeArrayL(aRptDef.iSporadicDates, iSporadicDates);
		
	if (aRptDef.iTimeZone != NULL)
		{
		if(iTimeZone == NULL)
			{
			iTimeZone = aRptDef.iTimeZone->CloneL(); 
			}
		else
			{
			iTimeZone->CopyL(*aRptDef.iTimeZone);
			}
		TimeZoneChangedL();
		}
	else
		{
		ResetTimeZone();
		}
	}

void CAgnRptDef::ResetTimeZone()
	{
	delete iTimeZone;
	iTimeZone = NULL;
	
	if (iRRule)
		{
		iRRule->ResetCachedStartTimeOffset();
		iRRule->ResetCachedUntilTimeOffset();
		}
	}

TBool CAgnRptDef::operator==(const CAgnRptDef& aRptDef) const
/** Compares two repeat definitions for equality. 

This compares all repeat details, the repeat type and the exception list.

@internalComponent
@param aRRule The repeat definition.
@return True if complete definitions agree, otherwise False. */
	{

	if (HasRepeatRule() && !aRptDef.HasRepeatRule() || !HasRepeatRule() && aRptDef.HasRepeatRule())
		{
		return EFalse;
		}

	if (aRptDef.HasRepeatRule())
		{
		if (Type() != aRptDef.Type())
			{
			return EFalse;
			}
		
		switch (Type())
			{
			case TAgnRpt::EDaily:
				{
				const TAgnDailyRpt* d1 = static_cast<const TAgnDailyRpt*>(RRule());
				const TAgnDailyRpt* d2 = static_cast<const TAgnDailyRpt*>(aRptDef.RRule());
				if (!(*d1 == *d2)) return EFalse;
				}
				break;
			case TAgnRpt::EWeekly: 
				{
				const TAgnWeeklyRpt* w1 = static_cast<const TAgnWeeklyRpt*>(RRule());
				const TAgnWeeklyRpt* w2 = static_cast<const TAgnWeeklyRpt*>(aRptDef.RRule());
				if (!(*w1 == *w2)) return EFalse;
				}
				break;
			case TAgnRpt::EMonthlyByDays: 
				{
				const TAgnMonthlyByDaysRpt* mp1 = static_cast<const TAgnMonthlyByDaysRpt*>(RRule());
				const TAgnMonthlyByDaysRpt* mp2 = static_cast<const TAgnMonthlyByDaysRpt*>(aRptDef.RRule());
				if (!(*mp1 == *mp2)) return EFalse;
				}
				break;
			case TAgnRpt::EMonthlyByDates: 
				{
				const TAgnMonthlyByDatesRpt* md1 = static_cast<const TAgnMonthlyByDatesRpt*>(RRule());
				const TAgnMonthlyByDatesRpt* md2 = static_cast<const TAgnMonthlyByDatesRpt*>(aRptDef.RRule());
				if (!(*md1 == *md2)) return EFalse;
				}
				break;
			case TAgnRpt::EYearlyByDay: 
				{
				const TAgnYearlyByDayRpt* yp1 = static_cast<const TAgnYearlyByDayRpt*>(RRule());
				const TAgnYearlyByDayRpt* yp2 = static_cast<const TAgnYearlyByDayRpt*>(aRptDef.RRule());
				if (!(*yp1 == *yp2)) return EFalse;
				}
				break;
			case TAgnRpt::EYearlyByDate: 
				{
				const TAgnYearlyByDateRpt* yd1 = static_cast<const TAgnYearlyByDateRpt*>(RRule());
				const TAgnYearlyByDateRpt* yd2 = static_cast<const TAgnYearlyByDateRpt*>(aRptDef.RRule());
				if (!(*yd1 == *yd2)) return EFalse;
				}
				break;
			}
		}

	if ( ! TAgnCalendarTime::CompareTimeArrays(Exceptions(), aRptDef.Exceptions()) )
		{
		return EFalse;
		}

	if ( ! TAgnCalendarTime::CompareTimeArrays(SporadicDateList(), aRptDef.SporadicDateList()) )
		{
		return EFalse;
		}

	return ETrue;
	}

TInt CAgnRptDef::InvariantL() const
/**
Check that the rpt doesn't violate its invariants
@internalComponent
*/
	{
	// accept as valid if iRRule has not yet been set	
	if (iRRule == NULL)
		{
		return KErrNone;
		}
	
	if (iRRule->InvariantL()==KErrNone)
		{
 		return (KErrNone);
		}
		
	return (EAgmErrBadRepeat);
	}



EXPORT_C void CAgnRptDef::ExternalizeL(RWriteStream& aStream, TBool aToBuffer)
/** Externalises the repeat definition to a write stream.

@internalComponent
@param aStream Stream to which the object should be externalised. */
	{
	__ASSERT_ALWAYS(InvariantL()==KErrNone, User::Leave(EAgmErrBadRepeat));

    // Repeat Rule
	if (HasRepeatRule())
		{
		aStream.WriteUint8L(ETrue);
		aStream.WriteUint8L(Type());
		aStream << *iRRule;	
		}
	else
		{
		aStream.WriteUint8L(EFalse);
		}	

	// Exceptions
	if (iExceptions)
		{
		aStream.WriteUint8L(ETrue);
		TAgnCalendarTime::ExternalizeTimeArrayL(*iExceptions, aStream);
		}
	else
		{
		aStream.WriteUint8L(EFalse);
		}
	
	// Sporadic Dates
	if (iSporadicDates)
		{
		aStream.WriteUint8L(ETrue);
		TAgnCalendarTime::ExternalizeTimeArrayL(*iSporadicDates, aStream);
		}
	else
		{
		aStream.WriteUint8L(EFalse);
		}
		
  	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
  		{// no timezone rule for floating time mode
	   	aStream.WriteUint8L(EFalse);
  		}
  	else
  		{
	  	if (iTimeZone == NULL)
	  		{	// Set time Zone for fixed time mode
	  		SetTimeZoneL();
	  		}

	   	aStream.WriteUint8L(ETrue);
	   	if(aToBuffer)
	   		{
	   		iTimeZone->WriteToBufferL(aStream);
	   		}
	   	else
	   		{
	   		iTimeZone->ExternalizeL(aStream);
	   		}
  		}
	}
				
void CAgnRptDef::InternalizeRepeatRuleL(RReadStream& aStream)
	{
	TBool hasRepeatRule = aStream.ReadUint8L();
	
	// Repeat Rule
	if (hasRepeatRule)
		{
		TAgnRpt::TType type = static_cast<TAgnRpt::TType>(aStream.ReadUint8L());
	
		if (type < TAgnRpt::EDaily || type > TAgnRpt::EYearlyByDay)
			{
			User::Leave(KErrCorrupt);
			}

		CreateRptObjectL(type);
		aStream >> *iRRule;
		}
	}

void CAgnRptDef::InternalizeExceptionsL(RReadStream& aStream)
	{
	ClearTimeArray(iExceptions);
	
	if (aStream.ReadUint8L())
		{
		EnsureTimeArrayExistsL(iExceptions);
		TAgnCalendarTime::InternalizeTimeArrayL(*iExceptions, aStream);
		}
	}

void CAgnRptDef::InternalizeSporadicDatesL(RReadStream& aStream)
	{
	ClearTimeArray(iSporadicDates);

	if (aStream.ReadUint8L())
		{
		EnsureTimeArrayExistsL(iSporadicDates);
		TAgnCalendarTime::InternalizeTimeArrayL(*iSporadicDates, aStream);
		}
	}

void CAgnRptDef::InternalizeTimeZoneL(RReadStream& aStream, TBool aFromBuffer)
	{
	// Time Zone
	TBool hasTimeZone = aStream.ReadUint8L();
	ResetTimeZone();
	
	if(hasTimeZone)
		{
		iTimeZone = CAgnTzRules::NewL(aStream, aFromBuffer);
		TimeZoneChangedL();
		}
	}

EXPORT_C void CAgnRptDef::InternalizeL(RReadStream& aStream, TBool aFromBuffer)
/** Internalises the repeat definition from a read stream, including type, repeat 
details and exception list.

@internalComponent
@param aStream Stream from which the object should be internalised. */
	{
    InternalizeRepeatRuleL(aStream);
    InternalizeExceptionsL(aStream);
    InternalizeSporadicDatesL(aStream);
    InternalizeTimeZoneL(aStream, aFromBuffer);	
  	  	  
	__ASSERT_ALWAYS(InvariantL()==KErrNone, User::Leave(KErrCorrupt));
	}


EXPORT_C TUint CAgnRptDef::InstanceCountL() const
/** Gets the number of repeat instances generated by the repeat algorithm, including 
excepted instances.

@internalComponent
@return The number of repeat instances. */
	{
	TInt total((0));
	
	if (HasRepeatRule())
		{
		total = iRRule->InstanceCountL();
		}
	else
		{
		total = 1;//count DtStart if there is no Repeat Rule
		}
		
	if (HasSporadicDates())
		{
		total = total + iSporadicDates->Count();
		}
	
	return total;
	}


EXPORT_C TBool CAgnRptDef::IsAnUnexceptedInstanceL(const TTime& aDate) const
/** Tests whether aDate falls on an instance generated from the repeat algorithm or a sporadic date
which is not excepted.

@internalComponent
@param aDate The system local date to be checked, expressed as Current System Local Time
@return True = the date falls on a non excepted instance, otherwise False. */
	{
	// IsAnInstanceL() is exported, so it takes System Local Times.
	if (!IsAnInstanceL(aDate))
		{
		return EFalse;
		}
	// FindException() is exported, so it takes System Local Times.//	
	TAgnCalendarTime exceptionToFind;
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		exceptionToFind.SetFloatingL(aDate);
		}
	else
		{
		exceptionToFind.SetLocalL(aDate);
		}
	if (FindException(exceptionToFind))  
		{
		return EFalse;
		}

	return ETrue;
	}


EXPORT_C TBool CAgnRptDef::IsAnInstanceL(const TTime& aDate) const
/** Tests whether aDate (System local time) falls on an instance generated from the repeat algorithm or on a sporadic date 
including excepted items. 

Note: this function is not intended to be called by a view.

@internalComponent
@param aDate The date to be checked, expressed as System Local Time
@return True = date falls on an instance, otherwise False. */
	{
	return IsASporadicDateInstanceL(aDate) || IsARepeatRuleDateInstanceL(aDate) || (aDate == StartTime().LocalL());
	}

EXPORT_C TBool CAgnRptDef::NudgeNextInstanceL(const TTime& aLocalDate, TTime& aLocalNext, TBool aCheckUnexcepted) const
/** Tests whether there is another repeat instance date after aDate (System local time), including excepted 
instances and Sporadic Dates. 

If there is, it returns ETrue and sets aNext to it. If not, it returns 
EFalse and sets aNext to the last valid instance date. If aDate is a valid 
instance, then aNext is set to it, not the next instance.

For example:

If repeat instances fall on 10th Jan and 20th Jan, then if aDate = 10th Jan, 
this function returns ETrue and sets aNext to 10th Jan. If aDate = 11th to 19th Jan, 
it  returns ETrue and sets aNext to 20th Jan. If aDate = 21st Jan, it returns EFalse 
and sets aNext to 20th Jan.

This function can leave.

@internalComponent
@param aDate The date, expressed as System Local Time
@param aNext On return contains the date (in System Local Time) of the next instance if there is one, 
or if not, the last valid instance date
@return True if there is a next instance, otherwise False. */
    {
	TTime utcDate = AgnDateTime::ConvertToUtcTimeL(aLocalDate) + TTimeIntervalMicroSeconds(1);

	TBool retVal = NudgeInstanceForwardsUtcL(utcDate, aLocalNext, aCheckUnexcepted);

	if (retVal)
		{			
		aLocalNext = AgnDateTime::ConvertToLocalTimeL(aLocalNext);	
		}
	
	return retVal;
	}

TBool CAgnRptDef::NudgeInstanceForwardsUtcL(const TTime& aUtcDate, TTime& aUtcNext, TBool aCheckExcepted) const 
	{
	TBool retValue((EFalse));
	
	// If there is a Rpt Rule and No Sporadic Dates
	if (iRRule && !HasSporadicDates())
		{
		retValue = NudgeNextRepeatInstanceUtcL(aUtcDate, aUtcNext, aCheckExcepted);
		}
	// else if there are Sporadic Dates and no Rpt Rule	
	else if (!iRRule && HasSporadicDates())	
		{
		retValue = NudgeNextSporadicInstanceUtcL(aUtcDate, aUtcNext, aCheckExcepted);
		const TAgnCalendarTime& KFirstInstanceUtc = StartTime();
		if (aUtcDate <= KFirstInstanceUtc.UtcL() && 
			(KFirstInstanceUtc.UtcL() < aUtcNext || retValue == EFalse))
			{
			if ( ! FindException(KFirstInstanceUtc))
				{
				aUtcNext = KFirstInstanceUtc.UtcL();
				retValue = ETrue;
				}
			}
		}
	else if (iRRule && HasSporadicDates()) // there are both Sporadic Dates and a Rule	
		{
		TTime rptNext;
		TBool rptHasNext = NudgeNextRepeatInstanceUtcL(aUtcDate, rptNext, aCheckExcepted);
	
		TTime sporadicNext;
		TBool sporadicHasNext = NudgeNextSporadicInstanceUtcL(aUtcDate, sporadicNext, aCheckExcepted);
			
		if (sporadicHasNext && !rptHasNext)
			{
			aUtcNext = sporadicNext;
			retValue = ETrue;
			}
		else if (!sporadicHasNext && rptHasNext)
			{
			aUtcNext = rptNext;
			retValue = ETrue;
			}
		else if (sporadicHasNext && rptHasNext) //  both have a next instance. Return the valid date closest to aDate 
			{
			aUtcNext = sporadicNext < rptNext ? sporadicNext : rptNext; 
			retValue = ETrue;
			}
		else  // neither has a next instance return EFalse  
			{			 
			retValue = EFalse;			
			}		
		}		
	return retValue;				
	}


TBool CAgnRptDef::NudgeNextRepeatInstanceUtcL(const TTime& aUtcDate, TTime& aUtcNext, TBool aConsiderExceptions) const
	{
	if (aUtcDate > iRRule->UntilTimeL().UtcL())
		{
		return (EFalse);
		}
		
	return aConsiderExceptions ? NudgeNextUnexceptedRepeatInstanceUtcL(aUtcDate, aUtcNext) : NudgeNextRepeatInstanceToNextValidInstanceUtcL(aUtcDate, aUtcNext);		
	}


TBool CAgnRptDef::NudgeNextRepeatInstanceToNextValidInstanceUtcL(const TTime& aUtcDate, TTime& aUtcNext) const
	{
	if (aUtcDate < StartTime().UtcL())
		{
		aUtcNext = iRRule->NudgeNextInstanceUtcL(StartTime().UtcL());	
		}
	else
		{
		aUtcNext = iRRule->NudgeNextInstanceUtcL(aUtcDate);	
		}	
	
	// if this is a valid instance that is the same as the one passed to the repeat rule's nudge
	if (aUtcNext != Time::NullTTime() && aUtcNext < aUtcDate)
		{
		return EFalse;
		}

	return aUtcNext != Time::NullTTime() && aUtcNext <= iRRule->UntilTimeL().UtcL() && aUtcNext >= StartTime().UtcL();		
	}


TBool CAgnRptDef::NudgePreviousRepeatInstanceToPreviousValidInstanceUtcL(const TTime& aUtcDate, TTime& aUtcPrev) const
	{
	TTime dateUtc = aUtcDate;
	if (aUtcDate > iRRule->UntilTimeL().UtcL())
		{
		dateUtc = iRRule->UntilTimeL().UtcL();
		}
		
	aUtcPrev = iRRule->NudgePreviousInstanceUtcL(dateUtc);		

	// if this is a valid instance, check that this is previous (time-wise)
	if (aUtcPrev != Time::NullTTime() && aUtcPrev > aUtcDate)   
		{
		return EFalse;
		}	
				
	return aUtcPrev != Time::NullTTime() && aUtcPrev <= iRRule->UntilTimeL().UtcL() && aUtcPrev >= StartTime().UtcL();		
	}

TBool CAgnRptDef::NudgeNextUnexceptedRepeatInstanceUtcL(const TTime& aUtcDate, TTime& aUtcNext) const
	{
	TTime currentDate = aUtcDate;
	if (currentDate < StartTime().UtcL())
		{
		currentDate = StartTime().UtcL();
		}

	TTime tmpLocalTime;	
	TTime next = aUtcNext;
	FOREVER
		{
		if (!NudgeNextRepeatInstanceToNextValidInstanceUtcL(currentDate, next))
			{
			return EFalse;
			}

		TAgnCalendarTime candidateException;
		if (TimeMode() == MAgnCalendarTimeMode::EFloating)
			{
			candidateException.SetFloatingL(AgnDateTime::ConvertToLocalTimeL(next));
			}
		else
			{
			candidateException.SetUtcL(next);
			}
		if (FindException(candidateException))
			{
			tmpLocalTime = AgnDateTime::ConvertToLocalTimeL(currentDate);
			tmpLocalTime += static_cast<TTimeIntervalDays>(1);
			currentDate = AgnDateTime::ConvertToUtcTimeL(tmpLocalTime);
			}
		else
			{
			aUtcNext = next;
			return ETrue;
			}
		}	
	}
	    

// aCheckExcepted indicates that sporadic dates that have exceptions are not to be considered as candidates
TBool CAgnRptDef::NudgeNextSporadicInstanceUtcL(const TTime& aUtcDate, TTime& aUtcNext, TBool aCheckExcepted) const
	{
	TAgnCalendarTime utcDate;
	utcDate.SetUtcL(aUtcDate);
	TBool retVal((EFalse));
	TBool moreCandidates((ETrue));
	TInt bestMatchPos((KErrNotFound));			
	TInt pos((iSporadicDates->Count() - 1));
		
	while (moreCandidates && pos >= 0)
		{
		// if the date is less than the sporadic array[pos] date
		moreCandidates = (utcDate <= (*iSporadicDates)[pos--]);
			
		// if the sporadic array[pos] date is later than the KDate and if we are checking exceptions it is not excepted
		const TAgnCalendarTime& KExceptionToFind = (*iSporadicDates)[pos + 1];
		if (moreCandidates && !(aCheckExcepted && FindException(KExceptionToFind)))
			{
			bestMatchPos = pos + 1;
			}	
		}
	
    if (bestMatchPos != KErrNotFound)
        {
        retVal = ETrue; 
        aUtcNext = (*iSporadicDates)[bestMatchPos].UtcL();
        }
    
	return retVal;
	}


EXPORT_C TBool CAgnRptDef::NudgePreviousUnexceptedInstanceL(const TTime& aLocalDate, TTime& aLocalPrev) const
/** Tests whether there is a repeat instance or sporadic date before aDate. 

If there is, it returns ETrue and sets aNext to it. If not, it returns EFalse 
and sets aNext to the last valid instance date. If aDate is a valid instance, 
then aNext is set to it, not the next instance.

This function can leave.

For example:

If repeat instances fall on 10th Jan and 20th Jan, then if aDate = 20th Jan, 
it returns ETrue and sets aNext to 20th Jan. If aDate = 11th to 19th Jan, it 
returns ETrue and sets aNext to 10th Jan. If aDate = 9th Jan, it returns EFalse 
and sets aNext to 10th Jan.

@internalComponent
@param aDate The date (System Local Time).
@param aPrev On return contains the date (System Local Time) of the previous instance if there 
is one, otherwise the first valid instance date.
@return True if there is a previous instance, otherwise false. */
	{
	TTime utcDate = AgnDateTime::ConvertToUtcTimeL(aLocalDate) - TTimeIntervalMicroSeconds(1);

	TBool retVal = NudgeInstanceBackwardsUtcL(utcDate, aLocalPrev, ETrue);
	
	if (retVal)
		{
		aLocalPrev = AgnDateTime::ConvertToLocalTimeL(aLocalPrev);	
		}
	
	return retVal;	  
	}


TBool CAgnRptDef::NudgeInstanceBackwardsUtcL(const TTime& aUtcDate, TTime& aUtcPrev, TBool aCheckExcepted) const
	{
	TBool retValue((EFalse));
	// If there is a Rpt Rule and No Sporadic Dates
	if (iRRule && !HasSporadicDates())
		{
		retValue = NudgePreviousRepeatInstanceUtcL(aUtcDate, aUtcPrev, aCheckExcepted);
		}
	// else if there are Sporadic Dates and no Rpt Rule	
	else if (!iRRule && HasSporadicDates())	
		{
		retValue = NudgePreviousSporadicInstanceUtcL(aUtcDate, aUtcPrev, aCheckExcepted);
		const TAgnCalendarTime& KFirstInstanceUtc = StartTime();
		if (aUtcDate >= KFirstInstanceUtc.UtcL() && 
			(KFirstInstanceUtc.UtcL() > aUtcPrev || retValue == EFalse))
			{
			if (! FindException(KFirstInstanceUtc))
				{
				aUtcPrev = KFirstInstanceUtc.UtcL();
				retValue = ETrue;
				}
			}
		}
	else if (iRRule && HasSporadicDates())
		{
		// there are both Sporadic Dates and a Rule	
		TTime rptPrevUtc;
		TBool rptHasPrev = NudgePreviousRepeatInstanceUtcL(aUtcDate, rptPrevUtc, aCheckExcepted);
	
		TTime sporadicPrevUtc;
		TBool sporadicHasPrev = NudgePreviousSporadicInstanceUtcL(aUtcDate, sporadicPrevUtc, aCheckExcepted);
			
		if (sporadicHasPrev && !rptHasPrev)
			{
			aUtcPrev = sporadicPrevUtc;
			retValue = ETrue;
			}
		else if (!sporadicHasPrev && rptHasPrev)
			{
			aUtcPrev = rptPrevUtc;
			retValue = ETrue;
			}
		else if (sporadicHasPrev && rptHasPrev)	  //  both have a next instance. Return the valid date closest to aDate 
			{
			aUtcPrev = sporadicPrevUtc > rptPrevUtc ? sporadicPrevUtc : rptPrevUtc;
			retValue = ETrue; 
			}
		else   //  neither have a next instance (under some circumstances repeat rules do not produce aNext.  
		       // It is beleived that this is not used by the caller if this function returns EFalse
			{
			retValue = EFalse;
			}			
		}
	return retValue;	
	} 

TBool CAgnRptDef::NudgePreviousSporadicInstanceUtcL(const TTime& aUtcDate, TTime& aUtcPrev, TBool aCheckExcepted) const
	{
	TAgnCalendarTime utcTime;
	utcTime.SetUtcL(aUtcDate);
	const TInt KCount = iSporadicDates->Count();
	TBool retVal((EFalse));
	TBool moreCandidates((ETrue));
	TInt bestMatchPos((KErrNotFound));			
	TInt pos((0));
		
	while (moreCandidates && KCount > pos)
		{
		// if the date is more than the sporadic array[pos] date
		moreCandidates = (utcTime >= (*iSporadicDates)[pos++]);  
			
		// if the sporadic array[pos] date is less than the KDate and if we are checking exceptions and it is not excepted
		const TAgnCalendarTime& KExceptionToFind = (*iSporadicDates)[pos - 1];
		if (moreCandidates && !(aCheckExcepted && FindException(KExceptionToFind)))
			{
			bestMatchPos = pos - 1;
			}	
		}
	
    if (bestMatchPos != KErrNotFound)
        {
        retVal = ETrue;
        aUtcPrev = (*iSporadicDates)[bestMatchPos].UtcL();
        }
       
	return retVal;		
	}
		  
		  
TBool CAgnRptDef::NudgePreviousRepeatInstanceUtcL(const TTime& aUtcDate, TTime& aUtcPrev, TBool aCheckExcepted) const
	{
	if (aUtcDate < StartTime().UtcL())
		{
		return EFalse;
		}
		
	return aCheckExcepted ? NudgePreviousUnexceptedRepeatInstanceUtcL(aUtcDate, aUtcPrev) : NudgePreviousRepeatInstanceToPreviousValidInstanceUtcL(aUtcDate, aUtcPrev);		
	}


TBool CAgnRptDef::NudgePreviousUnexceptedRepeatInstanceUtcL(const TTime& aUtcDate, TTime& aUtcPrev) const 
	{
	TTime fromUtc = aUtcDate;
	TTime prev = aUtcPrev;
	if (fromUtc > iRRule->UntilTimeL().UtcL())
		{
		fromUtc = iRRule->UntilTimeL().UtcL();
		}
	TTime tmpLocalTime;
	FOREVER
		{
		if (!NudgePreviousRepeatInstanceToPreviousValidInstanceUtcL(fromUtc, prev))
			{
			return EFalse;
			}
			
		TAgnCalendarTime candidateException;
		if (TimeMode() == MAgnCalendarTimeMode::EFloating)
			{
			candidateException.SetFloatingL(AgnDateTime::ConvertToLocalTimeL(prev));
			}
		else
			{
			candidateException.SetUtcL(prev);	
			}
		if ( ! FindException(candidateException))
			{
			aUtcPrev = prev;
			return ETrue;
			}
		else
			{
			tmpLocalTime = AgnDateTime::ConvertToLocalTimeL(fromUtc);
			tmpLocalTime -= static_cast<TTimeIntervalDays>(1);
			fromUtc = AgnDateTime::ConvertToUtcTimeL(tmpLocalTime);
			}
		}		 
	}

EXPORT_C TBool CAgnRptDef::NudgeNextInstanceUtcL(const TTime& aDayUtc, TTime& aNextUtc) const
/**
@internalComponent
*/
	{
	TTime utcNext;
	TBool retVal = NudgeInstanceForwardsUtcL(aDayUtc, utcNext, EFalse);
	if (retVal)
		{
		aNextUtc = utcNext;
		}
	return retVal;	
	}

EXPORT_C TBool CAgnRptDef::NudgePreviousInstanceUtcL(const TTime& aDayUtc, TTime& aPrevUtc) const
/**
@internalComponent
*/
    {
	TTime utcPrev;
	TBool retVal = NudgeInstanceBackwardsUtcL(aDayUtc, utcPrev, EFalse);
	if (retVal)
		{
		aPrevUtc = utcPrev;
		}
	return retVal;	
	}

CAgnTlsProxy* CAgnRptDef::TimeZoneAccessor()
	{
	// The client session for the agenda server creates and stores the time
	// zone accessor (which contains a session to the TZ server) in TLS. As
	// long as this is called from the same trhead that created the session
	// we can use the TZ server contained in the TZ accessor stored in TLS.
	CAgnTlsProxy* timeZoneAccessor = static_cast<CAgnTlsProxy*>(Dll::Tls());
	return timeZoneAccessor;
	}

EXPORT_C void CAgnRptDef::SetTimeZoneL(const CTzRules& aTimeZone)
/** Sets the time zone to aTimeZone

@internalComponent
@param aTimeZone The time zone in which the repeat entry times are expressed. It contains a 
collection of time zone rules. */
	{
	__ASSERT_DEBUG(TimeMode() != MAgnCalendarTimeMode::EFloating, User::Leave(KErrNotSupported));
	ResetTimeZone();
	iTimeZone = CAgnTzRules::NewL(aTimeZone);
  	TimeZoneChangedL();
	}

/** 
* Sets the time zone to the current system time zone. Leaves with KErrGeneral
* if the time zone server could not be accessed. 
*/
EXPORT_C void CAgnRptDef::SetTimeZoneL()
/**
@internalComponent
*/
	{
	__ASSERT_DEBUG(TimeMode() != MAgnCalendarTimeMode::EFloating, User::Leave(KErrNotSupported));
	ResetTimeZone();
	iTimeZone = CAgnTzRules::NewL();
  	TimeZoneChangedL();
	}

void CAgnRptDef::TimeZoneChangedL()
	{
	if (iRRule)
		{
		iRRule->ResetCachedStartTimeOffset();
		iRRule->ResetCachedUntilTimeOffset();
		}
 	}
 
EXPORT_C CTzRules* CAgnRptDef::CloneTzRulesL()
/** Gets a copy of the time zone rules that apply to the repeat definition.

@internalComponent
*/
	{
	if (iTimeZone)
		{
		return iTimeZone->CloneTzRulesL();	
		}
	return NULL;
	}

TBool CAgnRptDef::IsARepeatRuleDateInstanceL(const TTime& aLocalDate) const
	{
	if (!HasRepeatRule())
		{
		return EFalse;
		}	
	
	if (aLocalDate < StartTime().LocalL() || aLocalDate > iRRule->UntilTimeL().LocalL())
		{ 
		return EFalse;	
		}	
	
			
	// convert aDate to UTC before passing it TAgnRpt::IsAlignedUtcL()
	TTime utcDate = AgnDateTime::ConvertToUtcTimeL(aLocalDate);
	return iRRule->IsAlignedUtcL(utcDate);	
	}


EXPORT_C TAgnRpt::TType CAgnRptDef::Type() const
/** Gets the repeat definition's type, as set by SetDaily(), SetWeekly() etc.

@internalComponent
@return The repeat definition's type. */
	{
	if (iRRule)
		{
		return iRRule->Type(); 	
		}
	return TAgnRpt::EDaily;
	}

TBool CAgnRptDef::IsASporadicDateInstanceL(const TTime& aLocalDate) const
	{
	if (!HasSporadicDates())
		{
		return EFalse;
		}
	
	TAgnCalendarTime instanceDate;
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		instanceDate.SetFloatingL(aLocalDate);
		}
	else
		{
		instanceDate.SetLocalL(aLocalDate);
		}
	return (FindSporadicDate(instanceDate) != KErrNotFound);	
	}
	
/** Returns ETrue if there is at least one sporadic date

@internalComponent
@return if there is at least one sporadic date  */		
TBool CAgnRptDef::HasSporadicDates() const
	{
	return (iSporadicDates && iSporadicDates->Count() != 0);
	}	

/** Returns ETrue if there is a repeat rule

@internalComponent
@return if there is a repeat rule  */
TBool CAgnRptDef::HasRepeatRule() const
	{
	return (iRRule != NULL);
	}

/** Clear the Repeat Rule only, leaving sporadic dates and exception dates intact
@internalComponent
*/
void CAgnRptDef::ClearRRule()
	{
	delete iRRule;
	iRRule = NULL;
	}

void CAgnRptDef::StartTimeChanged()
	{
	if (iRRule)
		{
		iRRule->ResetCachedStartTimeOffset();
		}
	}

const CTzRules* CAgnRptDef::TzRules() const
	{
	if(iTimeZone)
		{
		return iTimeZone->TzRules();
		}
	
	return NULL; 
	}

EXPORT_C TTime CAgnRptDef::ConvertFromRepeatLocalToUtcL(const TTime& aRptLocalDate) const
	{
	if (TimeMode() != MAgnCalendarTimeMode::EFloating && TzRules())
		{
		TTime utcDate = aRptLocalDate;
		TzRules()->ConvertToUtcL(utcDate);
		return utcDate;
		}

	return AgnDateTime::ConvertToUtcTimeL(aRptLocalDate);
	}

EXPORT_C TTime CAgnRptDef::ConvertFromUtcToRepeatLocalL(const TTime& aUtcDate) const
	{
	if (TimeMode() != MAgnCalendarTimeMode::EFloating && TzRules())
		{
		TTime rptLocalDate = aUtcDate;
		TzRules()->ConvertToLocalL(rptLocalDate);
		return rptLocalDate;
		}

	return AgnDateTime::ConvertToLocalTimeL(aUtcDate);
	}

MAgnCalendarTimeMode::TTimeMode CAgnRptDef::TimeMode() const
	{
	return iOwningEntry.TimeMode();
	}

EXPORT_C const TAgnRpt* CAgnRptDef::RRule() const 
/** Gets a pointer to the repeat details.
@return Pointer to the repeat details
@internalComponent
*/
	{
	return iRRule;
	}

EXPORT_C const RArray<TAgnCalendarTime>* CAgnRptDef::Exceptions() const
/** Gets a pointer to the repeat definition's exception list.
@return Pointer to the exception list.
@internalComponent
*/
	{
	return iExceptions;
	}

EXPORT_C const RArray<TAgnCalendarTime>* CAgnRptDef::SporadicDateList() const
/** Gets a pointer to the repeat definition's sporadic date list.
@return Pointer to the sporadic date list.
@internalComponent
*/
	{
	return iSporadicDates;
	}

const TAgnCalendarTime& CAgnRptDef::StartTime() const
/** Gets the start date expressed as Current System Local Time.
@internalComponent
@return The start date expressed as Current System Local Time.
*/
	{
	if (iOwningEntry.Type() ==  CCalEntry::ETodo)
		{
		return iOwningEntry.StartTime();
		}
	else
		{
		return iOwningEntry.EntryTime();
		}
	}

EXPORT_C void CAgnRptDef::SetUntilTime(const TAgnCalendarTime& aUntilTime)
	{
	__ASSERT_ALWAYS(iRRule, Panic(EAgmErrNoRepeatRule));
	iRRule->SetUntilTime(aUntilTime);
	}

EXPORT_C void CAgnRptDef::SetInterval(TUint16 aInterval)
	{
	__ASSERT_ALWAYS(iRRule, Panic(EAgmErrNoRepeatRule));
	iRRule->SetInterval(aInterval);
	}

EXPORT_C CAgnTzRules* CAgnRptDef::AgnTzRules() const
	{
	return iTimeZone;
	}
