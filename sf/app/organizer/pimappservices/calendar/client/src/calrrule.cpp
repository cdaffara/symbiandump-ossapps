// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <calrrule.h>

#include "agmutil.h"
#include "calcommonimpl.h"

/** Constructor for the weekday within a month data structure.

@param aDay The weekday to set.
@param aWeekInMonth The week to set. 

@publishedAll
@released
@capability None
*/
EXPORT_C TCalRRule::TDayOfMonth::TDayOfMonth(TDay aDay, TInt8 aWeekInMonth) :
	iDay(aDay), iWeekInMonth(aWeekInMonth)
	{
	}

/** Get the weekday.
@return The weekday.

@publishedAll
@released 
@capability None
*/
EXPORT_C TDay TCalRRule::TDayOfMonth::Day() const
	{
	return iDay;
	}

/** Get the week within the month.
@return The week within the month.

@publishedAll
@released 
@capability None
*/
EXPORT_C TInt8 TCalRRule::TDayOfMonth::WeekInMonth() const
	{
	return iWeekInMonth;
	}

// Some bits are used internally to differentiate between rule types:
//  if bit 63 is set on a monthly rule, then it is week-of-month format, e.g. 3rd Wed
//  if bit 63 is not set on a monthly rule, then it is date-of-month format, e.g. 10th
const TUint KMonthlyByWeek = 63;

//  if bit 62 is set on a yearly rule, then it is week-of-month-of-year format, e.g. 3rd Wed of June
//  if bit 62 is not set on a yearly rule, then it is date-of-year format, e.g. 10th June
const TUint KYearlyByWeek = 62;

// Provides a mapping between the numbers 0 to 63 to bit fields.
// 0 maps to EBit1
// 1 maps to EBit2
// ...etc
TBool TCalRRule::GetNthBit(TUint aNum) const
	{
	TUint64 bit(EBit1);
	
	if (aNum >= 64)
		{
		CalUtils::Panic(ERepeatRuleBitFailure);
		}
	
	bit = bit << aNum;
	
	if (iBuffer & bit)
		{
		return ETrue;
		}
	return EFalse;
	}

void TCalRRule::SetNthBit(TUint aNum)
	{
	TUint64 bit(EBit1);
	
	if (aNum >= 64)
		{
		CalUtils::Panic(ERepeatRuleBitFailure);
		}
	
	bit = bit << aNum;
	
	iBuffer |= bit;
	}

/** Constructor for the iCal repeat rule type.
This does not set the type. All data is reset.
 
@publishedAll
@released
@capability None
*/
EXPORT_C TCalRRule::TCalRRule()
	{
	iType = EInvalid;
	InitialiseData();
	}

/** Constructor for the iCal repeat rule type.
All data is reset.

@leave KErrNotSupported If an invalid repeat rule type is passed in.

@param aType The type of repeat rule. One of: EDaily, EWeekly, EMonthly, EYearly. 

@publishedAll
@released
@capability None
*/
EXPORT_C TCalRRule::TCalRRule(TCalRRule::TType aType)
	{
	__ASSERT_ALWAYS(aType == EDaily || aType == EWeekly || aType == EMonthly || aType == EYearly, 
						Panic(EAgmErrBadRepeat));
	iType = aType;
	InitialiseData();
	}

void TCalRRule::InitialiseData()
	{
	iBuffer = 0;
	iCount = 0;
	iUntil.SetTimeUtcL(Time::NullTTime()); // this can't leave
	iInterval = 1;
	TLocale locale;
	iWkSt = locale.StartOfWeek();
	iReserved = 0;
	iReserved2 = 0;
	}

/** Sets the repeat definition's type.

This will be one of: daily, weekly, monthly or yearly. If the type is weekly,
it must have at least one weekday set by calling SetByDay().

The type may only be set once, from either the constructor or from the SetType function. 
@leave KErrNotSupported If an invalid type is passed in.
@panic CalInterimAPI 15 The repeat rule type has already been set.
@param aType The repeat definition's type. 
@see SetByDay()
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::SetType(TCalRRule::TType aType)
	{
	__ASSERT_ALWAYS(iType == EInvalid, CalUtils::Panic(ERepeatRuleTypeAlreadySet)); // type must not be set already
	__ASSERT_ALWAYS(aType == EDaily || aType == EWeekly || aType == EMonthly || aType == EYearly, 
			User::Leave(KErrNotSupported)); // aType must be a meaningful type	
	iType = aType;
	}

/** Gets the repeat definition's type. 

This will be one of: daily, weekly, monthly or yearly.

@return The repeat definition's type. 
@publishedAll
@released 
@capability None
*/
EXPORT_C TCalRRule::TType TCalRRule::Type() const
	{
	return iType;
	}

/** Sets the repeat's start date.

The repeat rule's start date should match the entry's start date. If it does
not then it will be modified to match the entry's start date (or end date for Todos)
when the rule is applied to the entry using CCalEntry::SetRRuleL.

@param aStartDate The start date, as a TCalTime so it may be accessed in utc or system local time. 
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::SetDtStart(const TCalTime& aStartDate)
	{
	iDtStart = aStartDate;
	}
	
/** Gets the date on which this repeat rule starts.

@return The start date, as a TCalTime so it may be accessed in utc or system local time. 
@publishedAll
@released 
@capability None
*/
EXPORT_C TCalTime TCalRRule::DtStart() const
	{ 	
	return iDtStart; 
	}

/** Sets the repeat's end date. 

Only one of the until or count parameters may be set. Setting the until date will reset the count parameter.
If the until date is set to TCalTime::MaxTime() the rule will repeat forever.

@param aEndDate The end date, as a TCalTime so it may be accessed in utc or system local time. 
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::SetUntil(const TCalTime& aEndDate)
	{
	//"-1" means only until time has been set 
	iCount = -1;	


	if (aEndDate.TimeUtcL() == TCalTime::MaxTime() || aEndDate.TimeLocalL() == TCalTime::MaxTime())
		{
		iCount = 0;	
		}

	iUntil = aEndDate;
	}
	
/** Gets the date on which this repeat rule finishes.

@return The end date, as a TCalTime so it may be accessed in utc or system local time. If 
the until value has not been set this will return Time::NullTTime.
@publishedAll
@released 
@capability None
*/
EXPORT_C TCalTime TCalRRule::Until() const
	{
	return iUntil; 
	}

/** Sets the repeat interval. 

This is a number of days for a daily repeat, a number of weeks for a 
weekly repeat, etc. 

@param aInterval The interval. Any parameter that is greater than 255 or less than 1 will be ignored.
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::SetInterval(TInt aInterval)
	{
	if (aInterval > 0 && aInterval < 256)
		{
		iInterval = aInterval;
		}
	}

/** Gets the repeat interval.

This is a number of days for a daily repeat, a number of weeks for a 
weekly repeat, etc. 

@return The interval. 
@publishedAll
@released 
@capability None
*/
EXPORT_C TInt TCalRRule::Interval() const
	{
	return iInterval;
	}

/** Sets the number of instances that this repeat rule will contain. Setting the count
 	will reset the until time to Time::NullTTime.

@param aCount The number of instances. Any non-positive parameter will be ignored.
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::SetCount(TUint aCount)
	{
	if (aCount > 0)
		{
		iCount = aCount;
		
		if(iUntil.TimeLocalL() != Time::NullTTime())
			{
			iUntil.SetTimeLocalL(Time::NullTTime());
			}	
		}
	}

/** Gets the number of instances that this repeat rule will contain.

@return The number of instances. It will return:

>0 - Indicate how many occurrences of a repeating rule.
 0 - Repeating forever. Note that it is the default value if there is neither Until time nor Count has been set. 
<0 - A Until time has been set but not the count. 

@publishedAll
@released 
@capability None
*/
EXPORT_C TUint TCalRRule::Count() const
	{
	return iCount;
	}
	
/** Set all weekdays on which this rule is repeated.

@leave KErrNotSupported if not called on a weekly repeat rule.
@param aDays Array containing all weekdays that are to be set. Any weekdays not included 
in this array will not be set. Any weekdays not in the TDay enum will be ignored. 
@publishedAll
@released
@capability None
*/
EXPORT_C void TCalRRule::SetByDay(const RArray<TDay>& aDays)
	{
	__ASSERT_ALWAYS(iType == EWeekly, User::Leave(KErrNotSupported));
	
	iBuffer = 0;
	
	TInt daysCount = aDays.Count();
	for (TInt i = 0; i < daysCount; ++i)
		{
		TDay day(aDays[i]);
		if (day >= EMonday && day <= ESunday)
			{
			// if this day is valid then set this bit
			SetNthBit(static_cast<TUint>(day));
			}
		}
	}

/** Gets all weekdays on which this rule is repeated.

@param aDays On return this array contains all weekdays that are to be set. 
This function will do nothing if this is not a weekly repeat rule. 
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::GetByDayL(RArray<TDay>& aDays) const
	{
	if (iType == EWeekly)
		{
		aDays.Reset();
		TDay day = EMonday;
		while(day <= ESunday)
			{
			if (GetNthBit(static_cast<TUint>(day)))
				{
				aDays.AppendL(day);
				}
			day = static_cast<TDay>((static_cast<TInt>(day) + 1));
			}
		}
	}

/** Set all weekdays in the month on which this rule is repeated.

@leave KErrNotSupported If it is called on a repeat rule which is not monthly or yearly.

@param aDays Array containing all days in the month that are to be set. Any days in the month
not included in this array will not be set. For a yearly rule, only the first item in the array 
will be used. Any invalid data will be ignored. 
@publishedAll
@released
@capability None
*/
EXPORT_C void TCalRRule::SetByDay(const RArray<TDayOfMonth>& aDays)
	{
	__ASSERT_ALWAYS(iType == EMonthly || iType == EYearly, User::Leave(KErrNotSupported));
		
	if (iType == EMonthly)
		{
		iBuffer = 0;
		TInt daysCount = aDays.Count();
		for (TInt i = 0; i < daysCount; ++i)
			{
			TDayOfMonth dayOfMonth = aDays[i];
			TDay weekDay = dayOfMonth.Day();
			if (weekDay >= EMonday && weekDay <= ESunday)
				{
				TInt byteToSet = dayOfMonth.WeekInMonth();
				// if this day and week are valid then set this bit
				if (byteToSet == -1 || (byteToSet >= 1 && byteToSet <= 4))
					{
					if (byteToSet == -1)
						{
						byteToSet = 0;
						}
					
					// first 7 bits represent which weekdays are set on the last week of the month - (week = -1)
					// next 7 bits represent which weekdays are set on the 1st week of the month - (week = 1)
					// next 7 bits represent which weekdays are set on the 2nd week of the month - (week = 2)
					// next 7 bits represent which weekdays are set on the 3rd week of the month - (week = 3)
					// next 7 bits represent which weekdays are set on the 4th week of the month - (week = 4)
					TUint bitToSet = weekDay + 7 * byteToSet;
					SetNthBit(bitToSet);
					}
				}
			}
		SetNthBit(KMonthlyByWeek);
		}
	else if (iType == EYearly)
		{
		if (!GetNthBit(KYearlyByWeek))
			{
			iBuffer = 0;
			SetNthBit(KYearlyByWeek);
			}
			
		if (aDays.Count() >= 1)
			{
			TDay theDay = aDays[0].Day();
			TInt8 theWeek = aDays[0].WeekInMonth();
			
			if (theDay >= EMonday && theDay <= ESunday && 
				(theWeek == -1 || (theWeek >= 1 && theWeek <= 4)) )
				{
				TUint8* bufferPtr = (TUint8*)&iBuffer; // convert the iBuffer store to three TUint8s
				// EMonday=0 so store it as 1, this allows us to tell if any days have been set
				bufferPtr[0] = (TUint8)theDay+1;
				bufferPtr[1] = (TUint8)theWeek;
				}
			}
		}
	}

/** Gets all days in the month on which this rule is repeated.

@param aDays On return this array contains all days in the month that are to be set. 
This function will do nothing if this is not a monthly or yearly repeat rule. 
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::GetByDayL(RArray<TDayOfMonth>& aDays) const
	{
	if (iType == EMonthly)
		{
		aDays.Reset();
		if (GetNthBit(KMonthlyByWeek))
			{
			// if i == 35, then weekNum = 5 below which is too high
			for (TUint i = 0; i < 35; ++i)
				{
				if (GetNthBit(i))
					{
					// This is the reverse of the algorithm in TCalRRule::SetByDay(const RArray<TDayOfMonth>& aDays)
					TDay day = static_cast<TDay>(i % 7);
					TInt weekNum = i / 7;
					if (weekNum == 0)
						{
						weekNum = -1;
						}
					TDayOfMonth dayOfMonth(day, weekNum);
					aDays.AppendL(dayOfMonth);
					}
				}
			}
		}
	else if (iType == EYearly)
		{
		aDays.Reset();
		if (GetNthBit(KYearlyByWeek))
			{
			TUint8* bufferPtr = (TUint8*)&iBuffer; // convert the iBuffer store to three TUint8s
			TDay theDay = TDay(bufferPtr[0]-1); // -1 because EMonday is stored as +1
			TInt8 theWeek = TInt8(bufferPtr[1]);
			
			if (theDay >= EMonday && theDay <= ESunday)
				{
				TDayOfMonth dayOfMonth(theDay, theWeek);
				aDays.AppendL(dayOfMonth);
				}
			}
		}
	}
	
/** Sets all dates of the month on which this rule is repeated.

@leave KErrNotSupported If it is not a monthly repeat rule.

@param aMonthDays Array containing all month dates that are to be set. Any month dates
not included in this array will not be set. Any out of range dates will be ignored. 
@publishedAll
@released
@capability None
*/
EXPORT_C void TCalRRule::SetByMonthDay(const RArray<TInt>& aMonthDays)
	{
	__ASSERT_ALWAYS(iType == EMonthly, User::Leave(KErrNotSupported));
	
	iBuffer = 0;
	TInt daysCount = aMonthDays.Count();
	for (TInt i = 0; i < daysCount; ++i)
		{
		TInt bit = aMonthDays[i];
		
		// check this date is valid
		if (bit >= 0 && bit <= 30)
			{
			SetNthBit(bit);
			}
		}
	}

/** Gets all dates of the month on which this rule is repeated.

@param aMonthDays On return, this array contains all month dates that are to be set.
This function will do nothing if this is not a monthly repeat rule. 
Note that 0 represents 1st of the month, 1 represents 2nd, etc.
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::GetByMonthDayL(RArray<TInt>& aMonthDays) const
	{
	if (iType == EMonthly)
		{
		aMonthDays.Reset();
		if (!GetNthBit(KMonthlyByWeek))
			{			
			// check first 31 bits of iBuffer to see which dates have been set
			for (TInt i = 0; i <= 30; ++i)
				{
				if (GetNthBit(i))
					{
					aMonthDays.AppendL(i);
					}
				}
			}
		}
	}

/** Sets the month of the year for this repeat rule.

@leave KErrNotSupported If it is not on a yearly repeat rule 
@param aMonths Array of months on which to repeat. Only the first month in the array is used.
@publishedAll
@released
@capability None
*/
EXPORT_C void TCalRRule::SetByMonth(const RArray<TMonth> aMonths)
	{
	__ASSERT_ALWAYS(iType == EYearly, User::Leave(KErrNotSupported));
	
	if (aMonths.Count() >= 1)
		{
		TMonth month = aMonths[0];
		if (month >= EJanuary && month <= EDecember)
			{
			if (!GetNthBit(KYearlyByWeek))
				{
				iBuffer = 0;
				SetNthBit(KYearlyByWeek);
				}
			
			TUint8* bufferPtr = (TUint8*)&iBuffer; // convert the iBuffer store to three TUint8s
			// EJanuary=0 so store it as 1, this allows us to tell if any months have been set
			bufferPtr[2] = (TUint8)month+1;
			}
		}
	}

/** Gets the month of the year for this repeat rule.
This function will do nothing if this is not a yearly repeat rule
@param aMonths On return, the months on which to repeat.
@publishedAll
@released 
@capability None
*/
EXPORT_C void TCalRRule::GetByMonthL(RArray<TMonth>& aMonths) const
	{
	if (iType == EYearly)
		{
		aMonths.Reset();
		if (GetNthBit(KYearlyByWeek))
			{
			TUint8* bufferPtr = (TUint8*)&iBuffer; // convert the iBuffer store to three TUint8s
			TMonth theMonth = TMonth(bufferPtr[2]-1); // -1 because EJanuary stored as 1
			if (theMonth >= EJanuary && theMonth <= EDecember)
				{
				aMonths.AppendL(theMonth);
				}
			}
		}
	}

/** Sets the first day of the week for this repeat rule.

@param aDay The weekday to be the first. Any invalid parameter will be ignored. 
@publishedAll
@released
@capability None
*/
EXPORT_C void TCalRRule::SetWkSt(TDay aDay)
	{
	if (aDay >= EMonday && aDay <= ESunday)
		{
		iWkSt = aDay;
		}
	}
	
/** Gets the first day of the week for this repeat rule.

@return The weekday to be the first.
@publishedAll
@released 
@capability None
*/
EXPORT_C TDay TCalRRule::WkSt() const
	{
	return iWkSt;
	}

void TCalRRule::SetUntilAndCount(const TCalTime& aTime, TUint aCount)
	{
	iUntil = aTime;
	iCount = aCount;
	}
