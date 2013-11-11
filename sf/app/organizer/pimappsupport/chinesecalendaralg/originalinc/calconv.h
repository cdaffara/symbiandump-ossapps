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
//

#ifndef __CALCONV_H__
#define __CALCONV_H__

// User includes
#include "calconvAstronomicalCal.h"
#include "calconvArithmeticalCal.h"

// Constants
const TInt KCalConInvalidDate = -50000; // invalid date error code

// Classes referenced
class TChineseDate;
class TArithmeticalDate;


class TChineseCalendar : public TAstronomicalCalendar  
{
public:
	IMPORT_C TChineseCalendar();
	IMPORT_C void DateTimeToChinese(const TDateTime& aDT);
	IMPORT_C void ChineseToDateTime(TDateTime& aDT);
	IMPORT_C TInt SetDate(const TChineseDate& aDate);
	IMPORT_C void GetDate(TChineseDate& aDate);

private:
	void ChineseToFixed(const TChineseDate& aDate, TReal& aJulianDay) const;
	void ChineseFromFixed(TChineseDate& aDate,const TReal& aJulianDay) const;
	TReal ChineseNewYear(TInt aYear) const;
	TBool NoMajorSolarTerm(TReal aJulianDay) const;
	TBool PriorLeapMonth(TReal& aMnthprime, TReal aMnth) const;
	TInt ChineseNewMoonOnOrAfter(TReal aJulianDay) const;
	TInt ChineseNewMoonBefore(TReal aJulianDay) const;
	TReal MajorSolarTermOnOrAfter(TReal& aJulianDay) const;
	TInt ChineseDateNextSolarLongitude(TReal& aJulianDay, const TReal& aDegrees) const;
	TReal ChineseTimeZone(const TReal& aJulianDay) const;
	TReal CurrentMajorSolarTerm(TReal& aJulianDay) const;
	TBool ValidDate(const TChineseDate& aDate) const;
	};

class TGregorianCalendar : public TArithmeticalCalendar
	{
public:
	IMPORT_C TGregorianCalendar();
	IMPORT_C TGregorianCalendar(TReal aJulianDay);
	IMPORT_C void GregorianToDateTime(TDateTime& aDT);
	IMPORT_C void DateTimeToGregorian(TDateTime& aDT);
	IMPORT_C void GetDate(TArithmeticalDate& aDate);
	IMPORT_C TInt SetDate(TArithmeticalDate& aGregDate);

private:
	void GregFromJulianDay(TArithmeticalDate& aDate, TReal aJulianDay) const;
	void InitMembers();
	TInt DayNumber(const TArithmeticalDate& aDate) const;
	void CalcGregYear(TArithmeticalDate& aDate, TReal aJulianDay) const;
	TBool IsLeapYear(const TInt aYear) const;
	TReal GregToJulianDay(const TArithmeticalDate& aYear) const;
	TInt GregDateDiff(const TArithmeticalDate aDate1, const TArithmeticalDate aDate2) const;
	TBool ValidDate(const TArithmeticalDate& aDate) const;

private:
	friend class TAstronomicalCalendar;
	friend class TChineseCalendar;
	};

class TChineseDate  
{
public:
	IMPORT_C TChineseDate();
	IMPORT_C TBool operator==(const TChineseDate& aDate) const;

public:
	TInt iCycle;
	TInt iYear;
	TInt iMonth;
	TBool iLeapMonth;
	TInt iDay;
};

class TArithmeticalDate  
{
public:
	IMPORT_C TArithmeticalDate();
	IMPORT_C TBool operator==(const TArithmeticalDate& aDate) const;

public:
	TInt iDay;
	TInt iMonth;
	TInt iYear;
};


#endif
