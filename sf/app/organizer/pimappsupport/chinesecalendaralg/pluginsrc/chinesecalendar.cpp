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

#include "chinesecalendar.h"
#include "arithmeticaldate.h"
#include "gregoriancalendar.h"

#if defined(_DEBUG)
_LIT(KCalconPanic,"Calcon");
#endif

const TUint KMonthInvalid=0;
const TUint K29DayMonth=1;
const TUint K30DayMonth=2;
const TUint K29DayLeapMonth=3;
const TUint K30DayLeapMonth=4;

const TUint KMonthMask=1<<15;
const TUint KLeapMonthShift=28;

const TInt KNoOfYearsInCycle = 60;

enum TCalconPanic
	{
	ECalconChineseFromFixedMonthInvalid=0,
	ECalconChineseToFixedLeapYearInvalid,
	ECalconGetDataYearOutOfRange,
	ECalconGetDataMonthOutOfRange,
	ECalconGetNewYearYearOutOfRange
	};

TUint TCalconData::GetData(TInt aCycle, TInt aYear, TInt aMonth) const
	{
	__ASSERT_DEBUG((aYear>=0 && aYear<=59), User::Panic(_L("Calcon"), ECalconGetDataYearOutOfRange));	//year range is 0..59
	__ASSERT_DEBUG((aMonth>=0 && aMonth<=12), User::Panic(_L("Calcon"), ECalconGetDataMonthOutOfRange));//month range is 0..12

	TInt y=(aCycle*KNoOfYearsInCycle)+aYear;
	y-=(KFirstYear-1);	//there are KFirstYear-1 entries missing from the head of the table
	TUint16 x=iCalConDataMonth[y];

	TUint mask=KMonthMask>>aMonth;
	
	TInt flag=K29DayMonth;
	if (x & mask)
		flag=K30DayMonth;

	TUint leapMonth=iCalConDataYear[y]>>KLeapMonthShift;
	leapMonth--;

	if ((TUint)aMonth==leapMonth)
		flag+=2;	//--> K29/30DayLeapMonth
	
	if ((aMonth==12) && (leapMonth==0))
		flag=KMonthInvalid;//month doesn't exist

	return flag;
	}

TUint TCalconData::GetNewYear(TInt aCycle, TInt aYear) const
	{
	__ASSERT_DEBUG((aYear>=0 && aYear<=59), User::Panic(_L("Calcon"), ECalconGetNewYearYearOutOfRange));
	TInt y=(aCycle*KNoOfYearsInCycle)+aYear;
	y-=(KFirstYear-1);
	return (iCalConDataYear[y] & 0x0fffffff);
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
void TChineseCalendar::ChineseToDateTime(TDateTime &aDT)
	{
	TArithmeticalDate gregDate;
	TGregorianCalendar greg(iJulianDay);

	greg.GregFromJulianDay(gregDate,iJulianDay);
	aDT.Set(0,EJanuary,0,0,0,0,0);

	aDT.SetDay(gregDate.iDay - KCalConvMonthOffsetByOne);
	aDT.SetMonth((TMonth)(gregDate.iMonth - KCalConvMonthOffsetByOne));
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
void TChineseCalendar::DateTimeToChinese(const TDateTime &aDT)
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
// Function:    SetDate
// Arguments:   TChineseDate&
//
// Comments:    this function sets the julian day value
//				in the base class from the given 
//				TChineseDate
//
// Return:      None
//------------------------------------------------------
TInt TChineseCalendar::SetDate(const TChineseDate &aDate)
	{
	TReal jD;
	TChineseDate ChinDate = aDate;
	
	if(!ValidDate(ChinDate))
		return KErrArgument;
	
	if (!ChineseToFixed(aDate,jD))
		return KErrArgument;

	if (jD<KFirstJulianDate || jD>KLastJulianDate)
		return KErrArgument;

	iJulianDay = (TInt)jD;
	return KErrNone;
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
TInt TChineseCalendar::GetDate(TChineseDate &aDate)
	{
	return ChineseFromFixed(aDate,iJulianDay);
	}

//------------------------------------------------------
// Class:       TChineseCalendar
// Function:    ChineseToFixed
// Arguments:   TChineseDate , TReal &
//				TChineseDate members start at 1 (not zero)
//
// Comments:    This function converts a chinese date to 
//				to a julian day value.
//
// Return:      TBool ETrue if date valid, else EFalse
//------------------------------------------------------
TBool TChineseCalendar::ChineseToFixed(const TChineseDate& aDate, TReal &aJulianDay) const
	{
	TInt cycle=aDate.iCycle-KFirstCycle;	//cycle starts from zero
	TInt year=aDate.iYear-1;				//year is 0..59

	TInt days=0;

	TInt targetMonth=aDate.iMonth;			//targetMonth is 1..12
	TBool leap=aDate.iLeapMonth;
	if (leap)
		{
		targetMonth++;
		if (iData.GetData(cycle, year, targetMonth-1)<3)
			return EFalse;	//not a leap month
		}

	TInt month=1;
	while (month<=targetMonth)
		{
		TInt daysFlag=iData.GetData(cycle, year, month-1);
		
//We need to handle case where targetMonth is a leap month
//Eg if Chinese month==6 targetMonth will be 6
//Eg but if Chinese month 5 is a leap month, month 6 will be 5(leap) so we need to take it into account
//BUT Eg if Chinese momth== 7(leap) we've already taken this into account.
		if (month==targetMonth)	
			if ((leap) || (daysFlag<3))
				break;

		switch (daysFlag)
			{
			case KMonthInvalid:
				return EFalse;
			case K29DayMonth:
				days+=29;
				break;
			case K30DayMonth:
				days+=30;
				break;
			case K29DayLeapMonth:
				__ASSERT_DEBUG(!leap, User::Panic(KCalconPanic,ECalconChineseToFixedLeapYearInvalid));
				leap=ETrue;
				targetMonth++;
				days+=29;
				break;
			case K30DayLeapMonth:
				__ASSERT_DEBUG(!leap, User::Panic(KCalconPanic,ECalconChineseToFixedLeapYearInvalid));
				leap=ETrue;
				targetMonth++;
				days+=30;
				break;
			}
		month++;
		}

//Check that if days==30, the requested month actually has 30 days
	TInt checkMonth = aDate.iMonth;
	if (leap)
		checkMonth++;	//this is the month requested by the user
	TUint daysFlag=iData.GetData(cycle, year, checkMonth-1);

	if ((aDate.iDay==30) && ((daysFlag==K29DayMonth) || (daysFlag==K29DayLeapMonth)))
		return EFalse;

	days+=aDate.iDay-1;
	
	days+=iData.GetNewYear(cycle, year);//add the New Year
	aJulianDay=days;
	return ETrue;
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
TInt TChineseCalendar::ChineseFromFixed(TChineseDate &aDate, const TReal& aJulianDay) const
	{
	if ((aJulianDay<KFirstJulianDate)
		|| (aJulianDay>KLastJulianDate))
		return KErrArgument;

	TInt cycleIndex=0;

	while ((cycleIndex < KLastCycle-KFirstCycle) 
			&& (aJulianDay >= iData.GetNewYear(cycleIndex+1,0)))
		cycleIndex++;
	
	aDate.iCycle=cycleIndex + KFirstCycle;
	TInt chineseNewYear;
	TInt yearCount=0;
	if (cycleIndex==0)
		yearCount=KFirstYear-1;

	while (yearCount<60 && aJulianDay >= iData.GetNewYear(cycleIndex,yearCount))
		yearCount++;
		
	aDate.iYear=yearCount;
	chineseNewYear = iData.GetNewYear(cycleIndex,--yearCount);
	
	TInt addedNumberOfDays = 0;
	TInt previousAddedNumberOfDays = 0;
	TInt monthCount = 1;
	aDate.iMonth = 0;

	TInt monthNumber; // 0=No month exists, 1 = 29 day month, 2 = 30 day month, 3 = 29 day leap month, 4 = 30 day leap month

	while (aJulianDay >= (chineseNewYear + addedNumberOfDays))
		{
		previousAddedNumberOfDays = addedNumberOfDays;
		monthNumber = iData.GetData(cycleIndex,yearCount,monthCount-1);

		switch (monthNumber)
			{
		case KMonthInvalid:
			__ASSERT_DEBUG(0, User::Panic(_L("Calcon"),ECalconChineseFromFixedMonthInvalid));
			break;
		case K29DayMonth:
			addedNumberOfDays += 29;
			aDate.iMonth++;
			aDate.iLeapMonth = EFalse;
			break;
		case K30DayMonth:
			addedNumberOfDays += 30;
			aDate.iMonth++;
			aDate.iLeapMonth = EFalse;
			break;
		case K29DayLeapMonth:
			addedNumberOfDays += 29;
			aDate.iLeapMonth = ETrue;
			break;
		case K30DayLeapMonth:
			addedNumberOfDays += 30;
			aDate.iLeapMonth = ETrue;
			break;
			}
		monthCount++;
		}

	aDate.iDay = (TInt)aJulianDay - chineseNewYear-previousAddedNumberOfDays;
	aDate.iDay++;
	return KErrNone;
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
	//do some trivial checks to ensure that the date is in the range of the lookup table
	if (aDate.iYear==0 || aDate.iYear>KNoOfYearsInCycle)
		return EFalse;

	if (aDate.iCycle < KFirstCycle)
		return EFalse;

	if (aDate.iCycle==KFirstCycle && aDate.iYear < KFirstYear)
		return EFalse;
	
	if (aDate.iCycle > KLastCycle)
		return EFalse;

	if ( (aDate.iCycle==KLastCycle) && (aDate.iYear>KLastYear))
		return EFalse;

	if (aDate.iDay==0 || aDate.iDay>30)
		return EFalse;

	return ETrue;
	}

TReal TChineseCalendar::JulianDate() __SOFTFP
	{
	return iJulianDay;
	}
