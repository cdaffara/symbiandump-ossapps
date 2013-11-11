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

#include "agmdate.h"
#include "agmtlsproxy.h"
#include "agmtzrules.h"

const TInt KAgnDateMonthsInAYear = 12;

EXPORT_C TTime AgnDateTime::MinDate()
/** Gets the minimum date/time allowed for agenda entries as a TTime. 

This value marks the start of the agenda model's valid date range.

@internalAll
@return The agenda model's minimum date/time. 
*/
	{ 
	//Midnight, January 1st 1900
	return (TTime(MAKE_TINT64(0x00D504A2,0xC672E000))); 
	} 

EXPORT_C TTime AgnDateTime::MaxDate()
/** Gets the maximum date/time allowed for agenda entries as a TTime. 

This value marks the end of the agenda model's valid date range.

@internalAll
@return The agenda model's maximum date/time. 
*/
	{ 
	//Midnight, December 31st 2100
	return (TTime(MAKE_TINT64(0x00eb8d74,0x5a6fc000)));
	} 


void AgnDateTime::AddTo(TDateTime& aDate,TTimeIntervalMonths aMonth)
/** Adds a time interval in months to a date/time. This takes account of the date of the month, which simply 
adding the TTimeIntervalMonths would not do.

@internalAll
@param aDate Reference to a date/time.
@param aMonth The number of months to add to the date/time. 
*/
	{
	TInt month = aDate.Month() + (aDate.Year()*KAgnDateMonthsInAYear) + aMonth.Int();
	TInt day = aDate.Day();
	TInt year = month / KAgnDateMonthsInAYear;
	month %= KAgnDateMonthsInAYear;
	TInt daysInMonth = Time::DaysInMonth(year,TMonth(month))-1;
	if (day > daysInMonth)
		{
		day = daysInMonth;
		}
	aDate.Set(year,TMonth(month),day,aDate.Hour(),aDate.Minute(),aDate.Second(),aDate.MicroSecond());
	}



void AgnDateTime::AddTo(TDateTime& aDate,TTimeIntervalYears aYear)
/** Adds a time interval in years to a date/time.

@internalAll
@param aDate Reference to a date/time. On return, the date is modified and all 
time components are set to zero.
@param aYear The number of years to add to the date/time. 
*/
	{
	AddTo(aDate,TTimeIntervalMonths(aYear.Int() * KAgnDateMonthsInAYear));
	}



TTimeIntervalMonths AgnDateTime::MonthsFrom(const TDateTime& aStartDate,const TDateTime& aEndDate)
/** Calculates the number of whole months between two date/times.

@internalAll
@param aStartDate The date to start calculating from.
@param aEndDate The date to calculate to.
@return The number of whole months between the two date/times. 
*/
	{

	TInt monthsDifference = (aEndDate.Year()-aStartDate.Year())*12+(aEndDate.Month()-aStartDate.Month());

	if (aEndDate.Day() < aStartDate.Day())
		{ // check relative positions within the month
		TInt endDaysInMonth = Time::DaysInMonth(aEndDate.Year(),aEndDate.Month());
		TInt startDaysInMonth = Time::DaysInMonth(aStartDate.Year(),aStartDate.Month());
		if (! ((aEndDate.Day() == (endDaysInMonth-1))&&(aStartDate.Day() == (startDaysInMonth-1))))
		   		--monthsDifference;
		}
	return(monthsDifference);			
	}


TTimeIntervalYears	AgnDateTime::YearsFrom(const TDateTime& aStartDate,const TDateTime& aEndDate)
/** Calculates the number of whole years between two date/times.

@internalAll
@param aStartDate The date to start calculating from.
@param aEndDate The date to calculate to.
@return The number of whole years between the two date/times. 
*/
	{

	TTimeIntervalMonths mos = MonthsFrom(aStartDate,aEndDate);
	TTimeIntervalYears ret = mos.Int() / 12;
	return(ret);			
	}

TBool AgnDateTime::IsLessThan(const TDateTime& aDateLeft, const TDateTime& aDateRight)
/** Tests whether one TDateTime instance is earlier than another.

@internalAll
@param aDateLeft First date/time.
@param aDateRight Second date/time.
@return True if the first date/time specified is earlier than the second. False 
if it is more recent, or they are the same. 
*/
	{
	return aDateLeft.Year() < aDateRight.Year() ||
			(aDateLeft.Year() == aDateRight.Year() && aDateLeft.Month() < aDateRight.Month()) ||
			(aDateLeft.Year() == aDateRight.Year() && 
			aDateLeft.Month() == aDateRight.Month() &&
			aDateLeft.Day() < aDateRight.Day());
	}

/** Is time between min and max time. */
EXPORT_C TBool AgnDateTime::IsValidAgendaTTime(const TTime& aDate)
	{
	return (aDate >= AgnDateTime::MinDate() && aDate <= AgnDateTime::MaxDate());
	}

/** Fetch a pointer to the fixed time mode converter */
const MAgnCalendarTimeMode* AgnDateTime::FixedTimeMode()
	{
	CAgnTlsProxy* proxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
	__ASSERT_DEBUG(proxy != NULL, User::Invariant());
	return static_cast<const MAgnCalendarTimeMode*>(&proxy->FixedTimeMode());
	}

/** Fetch a pointer to the floating time mode converter */
const MAgnCalendarTimeMode* AgnDateTime::FloatingTimeMode()
	{

	CAgnTlsProxy* proxy = static_cast<CAgnTlsProxy*>(Dll::Tls());
	__ASSERT_DEBUG(proxy != NULL, User::Invariant());
	return static_cast<const MAgnCalendarTimeMode*>(&proxy->FloatingTimeMode());
	}

/** Convert a time from system local to UTC, without caching the result. */
EXPORT_C TTime AgnDateTime::ConvertToLocalTimeL(const TTime& aUtcTime)
	{
    CAgnTlsProxy* proxy = CAgnTlsProxy::CreateL(CAgnTlsProxy::TAgnTlsTzRulesType_None);
    CleanupStack::PushL(TCleanupItem(CAgnTlsProxy::Release, proxy));
    TAgnCalendarTime utcTime;
    utcTime.SetUtcL(aUtcTime);
    TTime ret = utcTime.LocalL();
	CleanupStack::PopAndDestroy(proxy);
    return ret;
	}

/** Convert a time from UTC to system local, without caching the result. */
EXPORT_C TTime AgnDateTime::ConvertToUtcTimeL(const TTime& aLocalTime)
	{
    CAgnTlsProxy* proxy = CAgnTlsProxy::CreateL(CAgnTlsProxy::TAgnTlsTzRulesType_None);
    CleanupStack::PushL(TCleanupItem(CAgnTlsProxy::Release, proxy));
    TAgnCalendarTime utcTime;
    utcTime.SetLocalL(aLocalTime);
    TTime ret = utcTime.UtcL();
	CleanupStack::PopAndDestroy(proxy);
    return ret;
	}
  
/** This function resets the hours, minutes, seconds and microseconds to 0 
more quickly than using a TDateTime. */
EXPORT_C TTime AgnDateTime::ResetToMidnight(const TTime& aTime)
	{
	const TInt64 KMicrosecondsInADay (MAKE_TINT64(0x00000014,0x1DD76000));
	const TInt64 newTime((aTime.Int64() / KMicrosecondsInADay) * KMicrosecondsInADay);
	return TTime(newTime);
	}

/** Correctly calculate the number of days between two TTimes. DaysFrom gives unexpected results if the
time of day is different for each TTime. */
TInt AgnDateTime::DaysBetweenDates(const TTime& aTimeLater, const TTime& aTimeEarlier)
	{
	TTime later = AgnDateTime::ResetToMidnight(aTimeLater);
	TTime earlier = AgnDateTime::ResetToMidnight(aTimeEarlier);
	return later.DaysFrom(earlier).Int();
	}
