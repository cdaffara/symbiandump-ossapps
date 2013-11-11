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
// Implementation of the TChineseCalendar class.
// 
//

// User includes
#include "calconv.h"

// Constants
const TInt KChinStartEpoch = 758325;
const TInt KFifteenth = 15;
const TInt KDegreesPerMonth = 30;
const TInt KMonthsInYear = 12;
const TInt KFirstChinYear = -2636;
const TInt KYearsInChinCycle = 60;
const TInt KCycleOffset = 1;


//
// Construction/Destruction
//

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    TChineseCalendar
// Arguments:   None
//
// Comments:    Constructer
//
// Return:      None
//------------------------------------------------------
EXPORT_C TChineseCalendar::TChineseCalendar()
	{
	iStartEpoch = KChinStartEpoch;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    CurrentMajorSolarTerm
// Arguments:   TReal
//
// Comments:    Returns the index of the last major solar term
//
// Return:      see comment
//------------------------------------------------------
TReal TChineseCalendar::CurrentMajorSolarTerm(TReal& aJulianDay) const
	{
	TReal sigma;
	TInt sigmaI;

	AdjustJDToNoon(aJulianDay);

	sigma = ChineseTimeZone(aJulianDay);
	sigma = UniversalFromLocal(aJulianDay,sigma);
	SolarLongitude(sigma,sigma);
	sigma /= 30.0;
	Floor(sigmaI,sigma);
	sigmaI += 2;
	Amod(sigma,sigmaI,12.0);
	Round(sigmaI,sigma);
	return sigmaI;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseTimeZone
// Arguments:   TReal
//
// Comments:    Determines which time zone to use
//
// Return:      TReal - time zone
//------------------------------------------------------
TReal TChineseCalendar::ChineseTimeZone(const TReal& aJulianDay) const
	{
	TReal result;
	TGregorianCalendar greg(aJulianDay);
	TArithmeticalDate date;

	greg.GetDate(date);

	if(date.iYear < 1929)
		{
		result = 465.0 + (40.0 / 60.0);
		}
	else
		{
		result = 480.0;
		}
	return result;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseDateNextSolarLongitudeL
// Arguments:   TReal , TReal
//
// Comments:    calculates the first date on or after the given
//				julian day when the solar longitude will be a 
//				multiple of aDegrees degrees. 
//
// Return:      TInt - see comment
//------------------------------------------------------
TInt TChineseCalendar::ChineseDateNextSolarLongitude(TReal& aJulianDay, const TReal& aDegrees) const
	{
	TReal chTimeZone;
	TReal chDateNxtSlrLong;
	TInt rtn;

	AdjustJDToNoon(aJulianDay);
	chTimeZone = ChineseTimeZone(aJulianDay);

	chDateNxtSlrLong = UniversalFromLocal(aJulianDay,chTimeZone);
	chDateNxtSlrLong = DateNextSolarLongitude(chDateNxtSlrLong,aDegrees);
	chDateNxtSlrLong = LocalFromUniversal(chDateNxtSlrLong,chTimeZone);

	AdjustJDFromNoon(chDateNxtSlrLong);
	Floor(rtn,chDateNxtSlrLong);

	return rtn;
	}


//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    MajorSolarTermOnOrAfter
// Arguments:   TReal
//
// Comments:    Determines the major solar term on or after 
//				the given julian day.
//
// Return:      TReal - see comment
//------------------------------------------------------
TReal TChineseCalendar::MajorSolarTermOnOrAfter(TReal& aJulianDay) const
	{
	TReal majorSolarTerm;
	majorSolarTerm = ChineseDateNextSolarLongitude(aJulianDay,KDegreesPerMonth);
	return majorSolarTerm;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    GetDate
// Arguments:   TChineseDate &
//
// Comments:    This function Determines the chinese date and 
//				places it in the TChineseDate class provided
//				for the julian day value held internally in the 
//				Chinese class.
//
// Return:      None
//------------------------------------------------------
EXPORT_C void TChineseCalendar::GetDate(TChineseDate &aDate)
	{
	ChineseFromFixed(aDate,iJulianDay);
	}


//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseNewMoonBefore
// Arguments:   TReal
//
// Comments:    corrects for chinese month starting at noon
//				in Beijing
//
// Return:      TInt - new moon
//------------------------------------------------------
TInt TChineseCalendar::ChineseNewMoonBefore(TReal aJulianDay) const
	{
	TReal newMoon;
	TReal timeZone;
	TInt rtn;

	AdjustJDToNoon(aJulianDay);

	timeZone = ChineseTimeZone(aJulianDay);
	newMoon = UniversalFromLocal(aJulianDay,timeZone);
	newMoon = NewMoonBefore(newMoon);
	newMoon = LocalFromUniversal(newMoon,timeZone);

	AdjustJDFromNoon(newMoon);
	Floor(rtn,newMoon);

	return rtn;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseNewMoonOnOrAfter
// Arguments:   TReal
//
// Comments:    corrects for chinese month starting at noon
//				in Beijing
//
// Return:      TInt - new moon
//------------------------------------------------------
TInt TChineseCalendar::ChineseNewMoonOnOrAfter(TReal aJulianDay) const
	{
	TReal newMoon;
	TReal timeZone;
	TInt rtn;

	AdjustJDToNoon(aJulianDay);

	timeZone = ChineseTimeZone(aJulianDay);
	newMoon = UniversalFromLocal(aJulianDay,timeZone);
	newMoon = NewMoonAtOrAfter(newMoon);
	newMoon = LocalFromUniversal(newMoon,timeZone);

	AdjustJDFromNoon(newMoon);
	Floor(rtn,newMoon);

	return rtn;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    NoMajorSolarTerm
// Arguments:   TReal
//
// Comments:    Determines whether a given month lacks a 
//				major solar term.  The first month without
//				a major solar term is a leap month.
//
// Return:      TBool - ETrue if month has no major solar term
//------------------------------------------------------
TBool TChineseCalendar::NoMajorSolarTerm(TReal aJulianDay) const
	{
	TReal chinNewMoon;
	TReal currentMajSolTrm1;
	TReal currentMajSolTrm2;

	chinNewMoon = ChineseNewMoonOnOrAfter(aJulianDay + KCalConvDayOffsetByOne);
	currentMajSolTrm1 = CurrentMajorSolarTerm(chinNewMoon);
	currentMajSolTrm2 = CurrentMajorSolarTerm(aJulianDay);
	
	return(currentMajSolTrm1 == currentMajSolTrm2);
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    PriorLeapMonth
// Arguments:   TReal , TReal
//
// Comments:    this function is used in conjunction with
//				NoMajorSolarTerm() to ensure that only the 
//				first month missing a major solar term is 
//				a leap month
//
// Return:      TBool - 
//------------------------------------------------------
TBool TChineseCalendar::PriorLeapMonth(TReal& aMnthprime, TReal aMnth) const
	{
	TBool rtn,majorSolarTerm;
	TReal newMoonBefore;

	majorSolarTerm = NoMajorSolarTerm(aMnth);
	newMoonBefore = ChineseNewMoonBefore(aMnth);

	if((aMnth >= aMnthprime) && 
		(PriorLeapMonth(aMnthprime,newMoonBefore) || majorSolarTerm))
		{
		rtn = ETrue;
		}
	else
		{
		rtn = EFalse;
		}
	return rtn;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseNewYear
// Arguments:   TInt
//
// Comments:    Determines the chinese new year
//
// Return:      TReal - Julian day of the start of the given year
//------------------------------------------------------
TReal TChineseCalendar::ChineseNewYear(TInt aYear) const
	{
	TGregorianCalendar greg(iJulianDay);
	TArithmeticalDate date;
	TReal solarTerm1;
	TReal solarTerm2;
	TReal newMoon1;
	TReal newMoon2;
	TReal newMoon3;
	TReal rtn;
	TBool majorSolarTerm1;
	TBool majorSolarTerm2;
	TReal tempReal;
	TInt tempInt;

	// get solar term 1
	date.iDay = KFifteenth;
	date.iMonth = EDecember + KCalConvMonthOffsetByOne;
	date.iYear = aYear - KCalConvYearOffsetByOne;
	solarTerm1 = greg.GregToJulianDay(date);
	solarTerm1 = MajorSolarTermOnOrAfter(solarTerm1);

	// get solar term 2
	date.iDay = KFifteenth;
	date.iMonth = EDecember + KCalConvMonthOffsetByOne;
	date.iYear = aYear;
	solarTerm2 = greg.GregToJulianDay(date);
	solarTerm2 = MajorSolarTermOnOrAfter(solarTerm2);

	// get new moon 1
	newMoon1 = ChineseNewMoonOnOrAfter(solarTerm1 + KCalConvDayOffsetByOne);

	// get new moon 2
	newMoon2 = ChineseNewMoonOnOrAfter(newMoon1 + KCalConvDayOffsetByOne);

	// get new moon 3
	newMoon3 = ChineseNewMoonBefore(solarTerm2 + KCalConvDayOffsetByOne);

	// perform chinese new year calculation
	tempReal = (newMoon3 - newMoon1) / KMeanSynodicMonth;
	Round(tempInt,tempReal);
	majorSolarTerm1 = NoMajorSolarTerm(newMoon1);
	majorSolarTerm2 = NoMajorSolarTerm(newMoon2);
	if((tempInt == KMonthsInYear) && (majorSolarTerm1 || majorSolarTerm2))
		{
		rtn = ChineseNewMoonOnOrAfter(newMoon2 + KCalConvDayOffsetByOne);
		}
	else
		{
		rtn = newMoon2;
		}
	return rtn;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    SetDate
// Arguments:   TChineseDate&
//
// Comments:    this function sets the julian day value
//				in the base class from the given 
//				TChineseDate
//
// Return:      None
//------------------------------------------------------
EXPORT_C TInt TChineseCalendar::SetDate(const TChineseDate &aDate)
	{
	TReal jD;
	TChineseDate ChinDate = aDate;
	if(ValidDate(ChinDate)==EFalse)
		{
		return KCalConInvalidDate;
		}
		
	ChineseToFixed(aDate,jD);
	iJulianDay = jD;
	return KErrNone;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseFromFixed
// Arguments:   TChineseDate &, TReal
//
// Comments:    this function converts a julian day value to
//				a chinese date in the form TChineseDate
//
// Return:      None
//------------------------------------------------------
void TChineseCalendar::ChineseFromFixed(TChineseDate &aDate, const TReal& aJulianDay) const
	{
	TGregorianCalendar greg(aJulianDay);
	TArithmeticalDate date;
	TReal solarTerm1;
	TReal solarTerm2;
	TReal lunarTerm1;
	TReal lunarTerm2;
	TReal lunarTerm3;
	TInt leapYear;
	TInt year;
	TInt elapsedYears;
	TBool majorSolarTerm;
	TReal tempReal;
	TInt tempInt;

	// get the Gregorian date
	greg.GetDate(date);
	year = date.iYear;

	// get solar term 1
	date.iDay = KFifteenth;
	date.iMonth = EDecember + KCalConvMonthOffsetByOne;
	date.iYear = year - KCalConvYearOffsetByOne;
	solarTerm1 = greg.GregToJulianDay(date);
	solarTerm1 = MajorSolarTermOnOrAfter(solarTerm1);

	// get solar term 2
	date.iDay = KFifteenth;
	date.iMonth = EDecember + KCalConvMonthOffsetByOne;
	date.iYear = year;
	solarTerm2 = greg.GregToJulianDay(date);
	solarTerm2 = MajorSolarTermOnOrAfter(solarTerm2);

	// get lunar term 1
	if((solarTerm1 <= aJulianDay) && (aJulianDay < solarTerm2))
		{
		lunarTerm1 = solarTerm1 + KCalConvDayOffsetByOne;
		}
	else
		{
		lunarTerm1 = solarTerm2 + KCalConvDayOffsetByOne;
		}
	lunarTerm1 = ChineseNewMoonOnOrAfter(lunarTerm1);

	// get lunar term 2
	if((solarTerm1 <= aJulianDay) && (aJulianDay < solarTerm2))
		{
		lunarTerm2 = solarTerm2 + KCalConvDayOffsetByOne;
		}
	else
		{
		date.iDay = KFifteenth;
		date.iMonth = EDecember + KCalConvMonthOffsetByOne;
		date.iYear = year + KCalConvYearOffsetByOne;
		lunarTerm2 = greg.GregToJulianDay(date);
		lunarTerm2 = MajorSolarTermOnOrAfter(lunarTerm2) + KCalConvDayOffsetByOne;
		}
	lunarTerm2 = ChineseNewMoonBefore(lunarTerm2);

	// get Lunar term 3
	lunarTerm3 = aJulianDay + KCalConvDayOffsetByOne;
	lunarTerm3 = ChineseNewMoonBefore(lunarTerm3);

	// get leap year
	tempReal = ((lunarTerm2 - lunarTerm1) / KMeanSynodicMonth);
	Round(tempInt,tempReal);
	if(tempInt == KMonthsInYear)
		{
		leapYear = ETrue;
		}
	else
		{
		leapYear = EFalse;
		}

	// get month
	tempReal = ((lunarTerm3 - lunarTerm1) / KMeanSynodicMonth);
	Round(tempInt,tempReal);
	if(leapYear && PriorLeapMonth(lunarTerm1,lunarTerm3))
		{
		tempInt--;
		}
	Amod(tempReal,tempInt,KMonthsInYear);
	Round(aDate.iMonth,tempReal);

	// get leap month
	majorSolarTerm = NoMajorSolarTerm(lunarTerm3);
	if((leapYear) && majorSolarTerm && (!PriorLeapMonth(lunarTerm1,ChineseNewMoonBefore(lunarTerm3))))
		{
		aDate.iLeapMonth = ETrue;
		}
	else
		{
		aDate.iLeapMonth = EFalse;
		}

	// get elapsed years
	elapsedYears = year - KFirstChinYear;

	date.iDay = KCalConvFirstDay;
	date.iMonth = EJuly + KCalConvMonthOffsetByOne;
	date.iYear = year;
	tempReal = greg.GregToJulianDay(date);
	if((aDate.iMonth < (ENovember + KCalConvMonthOffsetByOne)) || (aJulianDay > tempReal))
		{
		elapsedYears++;
		}

	// get cycle
	tempReal = (elapsedYears - KCalConvYearOffsetByOne) / KYearsInChinCycle;
	Floor(aDate.iCycle,tempReal);
	aDate.iCycle++;

	// get year
	Amod(tempReal,elapsedYears,KYearsInChinCycle);
	Round(aDate.iYear,tempReal);

	// get day
	Floor(tempInt,aJulianDay);
	aDate.iDay = tempInt;
	Floor(tempInt,lunarTerm3);
	aDate.iDay -= tempInt;
	aDate.iDay += KCalConvDayOffsetByOne;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ValidDate
// Arguments:   TChineseDate &
//
// Comments:    This function Determines whether the given
//				date is a valid chinese date
//
// Return:      TBool - ETrue if date is valid, else EFalse
//------------------------------------------------------
TBool TChineseCalendar::ValidDate(const TChineseDate &aDate) const
	{
	TReal jD;
	TChineseDate chinDate;

	ChineseToFixed(aDate,jD);
	ChineseFromFixed(chinDate,jD);

	if(aDate.operator==(chinDate))
		return ETrue;
	else
		return EFalse;
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseToDateTime
// Arguments:   TDateTime &
//
// Comments:    This function converts the date held within
//				the TChineseCalendar class to a TDateTime format and 
//				places it in the TDateTime class provided.
//
// Return:      void
//------------------------------------------------------
EXPORT_C void TChineseCalendar::ChineseToDateTime(TDateTime &aDT)
	{
	TArithmeticalDate gregDate;
	TGregorianCalendar greg(iJulianDay);

	greg.GregFromJulianDay(gregDate,iJulianDay);

	aDT.SetMicroSecond(0);
	aDT.SetSecond(0);
	aDT.SetMinute(0);
	aDT.SetHour(0);
	aDT.SetMonth((TMonth)(gregDate.iMonth - KCalConvMonthOffsetByOne));
	aDT.SetDay(gregDate.iDay - KCalConvMonthOffsetByOne);
	aDT.SetYear(gregDate.iYear);
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    DateTimeToChinese
// Arguments:   TDateTime &
//
// Comments:    Sets the date held in the given TDateTime
//				class to the TChineseCalendar class
//
// Return:      void
//------------------------------------------------------
EXPORT_C void TChineseCalendar::DateTimeToChinese(const TDateTime &aDT)
	{
	TArithmeticalDate gregDate;
	TGregorianCalendar greg;

	gregDate.iDay = aDT.Day() + KCalConvMonthOffsetByOne;
	gregDate.iMonth = (TInt)aDT.Month() + KCalConvMonthOffsetByOne;
	gregDate.iYear = aDT.Year();

	iJulianDay = greg.GregToJulianDay(gregDate);
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseToFixed
// Arguments:   TChineseDate , TReal &
//
// Comments:    This function converts a chinese date to 
//				to a julian day value.
//
// Return:      void
//------------------------------------------------------
void TChineseCalendar::ChineseToFixed(const TChineseDate& aDate, TReal &aJulianDay) const
	{
	TInt year;
	TReal newYear;
	TReal pi;
	TReal priorNewMoon;
	TReal yearReal;
	TChineseDate chinDate;

	// get year
	yearReal = (aDate.iCycle - KCycleOffset) * KYearsInChinCycle;
	Floor(year,yearReal);
	year += aDate.iYear - KCalConvYearOffsetByOne;
	year += KFirstChinYear;

	// get new year
	newYear = ChineseNewYear(year);

	// get papa
	pi = ChineseNewMoonOnOrAfter(newYear + ((aDate.iMonth - KCalConvMonthOffsetByOne) * 29));

	// get delta
	ChineseFromFixed(chinDate,pi);

	// get priorNewMoon
	if((aDate.iMonth == chinDate.iMonth) && (aDate.iLeapMonth == chinDate.iLeapMonth))
		{
		priorNewMoon = pi;
		}
	else
		{
		priorNewMoon = ChineseNewMoonOnOrAfter(pi + KCalConvDayOffsetByOne);
		}

	aJulianDay = priorNewMoon + aDate.iDay - KCalConvDayOffsetByOne;
	}
