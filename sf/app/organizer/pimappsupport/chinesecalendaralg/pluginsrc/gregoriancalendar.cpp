// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implementation of the TGregorianCalendar class.
//

#include <e32std.h>
#include <e32math.h>
#include "gregoriancalendar.h"

// Constants
const TReal KFourYears			= 4.0;
const TReal KFourHundredYears	= 400;
const TReal KOneHundredYears	= 100;
const TReal KGregStartEpoch		= 1721424.5;
//
const TInt KDayOffsetByZero		= 0;
const TInt KGregDaysInNormYear	= 365;
const TInt KDaysIn400Years		= 146097;
const TInt KDaysIn100Years		= 36524;
const TInt KDaysIn4Years		= 1461;
const TInt KGregCycle			= 12;
const TInt KGregLeaps			= 7;
const TInt KGregOffset			= 11;
const TInt KGregStdMonth		= 30;
const TInt KLastDayInDecember	= 31;



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    TGregorianCalendar
// Arguments:   None
//
// Comments:    Constructor
//
// Return:      None
//------------------------------------------------------
TGregorianCalendar::TGregorianCalendar()
	{
	InitMembers();
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    TGregorianCalendar
// Arguments:   TReal
//
// Comments:    Overloaded Constructor
//
// Return:      None
//------------------------------------------------------
TGregorianCalendar::TGregorianCalendar(TReal aJD) __SOFTFP
	{
	iJulianDay = aJD;
	InitMembers();
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    InitMembers
// Arguments:   None
//
// Comments:    This function initialises the member variables
//				or the class.  It is called in the constructors
//
// Return:      
//------------------------------------------------------
void TGregorianCalendar::InitMembers()
	{
	iStartEpoch = KGregStartEpoch + KCalConvPointFive;
	Floor(iDaysInNormYear,KGregDaysInNormYear);
	iCycle = KGregCycle;
	iLeaps = KGregLeaps;
	iOffset = KGregOffset;
	iStdYearMonth = KGregStdMonth;
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    IsLeapYear
// Arguments:   const TInt
//
// Comments:    Determines whether the given year is a leap year.
//
// Return:      ETrue if leap year, else EFalse
//------------------------------------------------------
TBool TGregorianCalendar::IsLeapYear(const TInt aYear)const
	{
	TReal result;
	TBool rtn = EFalse;

	Mod(result,aYear,KFourYears);

	if	(!result)
		{
		rtn = ETrue;
		}

	Mod(result,aYear,KOneHundredYears);

	if	(!result)
		{
		Mod(result,aYear,KFourHundredYears);

		if	(result)
			{
			rtn = EFalse;
			}
		}
	return rtn;
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    GregToJulianDay
// Arguments:   const TArithmeticalDate&
//
// Comments:    converts gregorian date to Julian day value
//
// Return:      Julian day value for the given Gregorian date
//------------------------------------------------------
TReal TGregorianCalendar::GregToJulianDay(const TArithmeticalDate& aDate) const __SOFTFP
	{
	TReal result;
	TReal tempReal;
	TInt tempInt;

	// days to the start of the year based on normal years
	result = iStartEpoch + (iDaysInNormYear * (aDate.iYear - 1));

	// leap year corrections
	tempReal = (aDate.iYear - 1) / KFourYears;
	Floor(tempInt,tempReal);
	result += tempInt;

	tempReal = (aDate.iYear - 1) / KOneHundredYears;
	Floor(tempInt,tempReal);
	result -= tempInt;

	tempReal = (aDate.iYear - 1) / KFourHundredYears;
	Floor(tempInt,tempReal);
	result += tempInt;

	// days in whole months in the year in question - approx 
	// based on Feb having 30 days
	tempInt = DaysMonthsElapsed(aDate.iMonth);
	result += tempInt;

	// correction for assumption that Feb has 30 days
	if	(aDate.iMonth <= (EFebruary + 1))
		result += KDayOffsetByZero;
	else if	(aDate.iMonth > (EFebruary + 1) && IsLeapYear(aDate.iYear))
		result -= 1;
	else
		result -= 2;

	// days in month in question
	result += aDate.iDay;

	return result;
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    GregorianToDateTime
// Arguments:   TDateTime& 
//
// Comments:    This gets the date from the TGregorianCalendar class
//				and places in the the given TDateTime class
//
// Return:      void
//------------------------------------------------------
void TGregorianCalendar::GregorianToDateTime(TDateTime& aDT)
	{
	TArithmeticalDate gregDate;
	GregFromJulianDay(gregDate,iJulianDay);

	aDT.Set(0,EJanuary,0,0,0,0,0);
	aDT.SetMonth((TMonth)(gregDate.iMonth - 1));
	aDT.SetDay(gregDate.iDay - 1);
	aDT.SetYear(gregDate.iYear);
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    CalcGregYear
// Arguments:   TArithmeticalDate&
//
// Comments:    Calculates the gregorian year from Julian 
//				day value.
//
// Return:      None
//------------------------------------------------------
void TGregorianCalendar::CalcGregYear(TArithmeticalDate& aDate, TReal aJulianDay) const __SOFTFP
	{
	TReal d0;
	TReal d1;
	TReal d2;
	TReal d3;
	TReal year;
	TInt n400;
	TInt n100;
	TInt n4;
	TInt n1;
	TInt32 yearInt32;

	// determine year
	// get to start of calendar ie year 1
	d0 = aJulianDay - iStartEpoch - 1;

	Mod(d1,d0,KDaysIn400Years);

	Mod(d2,d1,KDaysIn100Years);

	Mod(d3,d2,KDaysIn4Years);

	// number of 400 year blocks
	d0 /= KDaysIn400Years;
	Floor(n400,d0);

	// number of 100 year blocks
	d1 /= KDaysIn100Years;
	Floor(n100,d1);

	// number of 4 year blocks
	d2 /= KDaysIn4Years;
	Floor(n4,d2);

		// number of 1 year blocks
	d3 /= KGregDaysInNormYear;
	Floor(n1,d3);

	// calc year
	year = (KFourHundredYears * n400) + (KOneHundredYears * n100) + (KFourYears * n4) + n1;
	Math::Int(yearInt32,year);

	if	((n100 == (KFourHundredYears / KOneHundredYears)) || (n1 == KFourYears))
		{
		aDate.iYear = yearInt32;
		}
	else
		{
		aDate.iYear = yearInt32 + 1;
		}
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    GregDateDiff
// Arguments:   const TArithmeticalDate, const TArithmeticalDate
//
// Comments:    This function Determines the difference in days
//				between two gregorian dates.
//
// Return:      Days difference
//------------------------------------------------------
TInt TGregorianCalendar::GregDateDiff(const TArithmeticalDate aDate1, const TArithmeticalDate aDate2)const
	{
	TInt diff;
	TReal diffReal;
	diffReal = GregToJulianDay(aDate2) - GregToJulianDay(aDate1);
	Floor(diff,diffReal);
	return diff;
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    DayNumber
// Arguments:   const TArithmeticalDate& 
//
// Comments:    this function returns the number of days that
//				have elapsed in the given year
//
// Return:      see comment
//------------------------------------------------------
TInt TGregorianCalendar::DayNumber(const TArithmeticalDate& aDate)const
	{
	TArithmeticalDate tempDate;
	TInt dayNum;

	tempDate.iDay = KLastDayInDecember;
	tempDate.iMonth = EDecember + 1;
	tempDate.iYear = aDate.iYear - 1;

	dayNum = GregDateDiff(tempDate,aDate);

	return dayNum;
	}

//------------------------------------------------------
// Class:       TGregorianCalendar
// Function:    GregFromJulianDay
// Arguments:   TArithmeticalDate& , TReal
//
// Comments:    This function converts a gregorian date to
//				a julian day
//
// Return:      void
//------------------------------------------------------
void TGregorianCalendar::GregFromJulianDay(TArithmeticalDate& aDate, TReal aJulianDay) const __SOFTFP
	{
	TReal priorDays;
	TReal correction;
	TReal day;
	TReal month;
	TInt32 dayInt32;
	TInt32 monthInt32;

	CalcGregYear(aDate,aJulianDay);

	aDate.iDay = 1;
	aDate.iMonth = EJanuary + 1;

	priorDays = aJulianDay - GregToJulianDay(aDate);

	aDate.iMonth = EMarch + 1;

	if	(aJulianDay < GregToJulianDay(aDate))
		{
		correction = KDayOffsetByZero;
		}
	else if	((aJulianDay >= GregToJulianDay(aDate)) && IsLeapYear(aDate.iYear))
		{
		correction = 1;
		}
	else
		{
		correction = 2;
		}

	month = priorDays + correction;
	Math::Int(monthInt32,month);
	monthInt32 = DayMonthInYear(monthInt32);
	aDate.iMonth = monthInt32;

	day = aJulianDay - GregToJulianDay(aDate) + 1;
	Math::Int(dayInt32,day);
	aDate.iDay = dayInt32;
	}


//------------------------------------------------------
// Class:       TArithmeticalCalendar
// Function:    DaysMonthsElapsed
// Arguments:   const TInt ,const TInt ,const TInt , const TInt ,const TInt
//
// Comments:    The use of this function varies between calendars 
//				however since the form of the equation is the same it 
//				is represented in this class.  The equation is use to 
//				calculate the number of days in whole months of a particular
//				year up to a particular date in the Gregorian and 
//				Islamic calendars.  The Hebrew calendar uses this
//				equation to calculate the number of whole months 
//				since the start of the calendar.
//				
//
// Return:      See comments
//------------------------------------------------------
TInt TGregorianCalendar::DaysMonthsElapsed(const TInt aYear) const
	{
	TReal tempReal;
	TInt result;

	tempReal = iOffset * iLeaps;
	Mod(tempReal,tempReal,iCycle);
	tempReal = ((iLeaps * aYear) - iLeaps + tempReal) / iCycle;
	Floor(result,tempReal);
	result = result + (iStdYearMonth * (aYear - KCalConvYearOffsetByOne));

	return result;
	}

//------------------------------------------------------
// Class:       TArithmeticalCalendar
// Function:    DayMonthInYear
// Arguments:   None
//
// Comments:    This function Determines the month based on
//				the number of days into the year it is
//
// Return:      TInt - Month
//------------------------------------------------------
TInt TGregorianCalendar::DayMonthInYear(const TInt aDays) const
	{
	TReal tempReal;
	TInt result;

	tempReal = iLeaps * iOffset;
	Mod(tempReal,tempReal,iCycle);
	tempReal = (iCycle * aDays) + (iCycle * iStdYearMonth) + iLeaps - KCalConvYearOffsetByOne + iCycle - tempReal;
	tempReal = tempReal / ((iCycle * iStdYearMonth) + iLeaps);
	Floor(result,tempReal);

	return result;
	}
