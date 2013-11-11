// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __CALENDARCONVERTER_H__
#define __CALENDARCONVERTER_H__

// System includes
#include <e32std.h>
#include <e32base.h>

class TChineseDate  
/** Chinese date.

Its public member data is the year cycle, the year (1-60), the month (1-12), whether or 
not the month is a leap month, and the day in the month (1-29 or 30). 
@publishedAll
@released */
	{
public:
	IMPORT_C TChineseDate();
	IMPORT_C TBool operator==(const TChineseDate& aDate) const;
public:
	/** The year cycle. */
	TInt iCycle;
	/** The year (1-60). */
	TInt iYear;
	/** The month (1-12). */
	TInt iMonth;
	/** Whether or not the month is a leap month. */
	TBool iLeapMonth;
	/** The day in the month (1-29 or 30). */
	TInt iDay;
	};


class CChineseCalendarConverter : public CBase
/** Converts between TDateTime and TChineseDate formats in both directions. Chinese dates 
are calculated using the -2636 epoch. This is equivalent to 2637 BCE (Before Common Era). 
@publishedAll
@released */
	{
public:
	IMPORT_C static CChineseCalendarConverter* NewL();
	/** Creates a Chinese date from a TDateTime value.
	
	@param aDateTime The date/time value to convert.
	@param aChineseDate On return, contains the Chinese date. If the supplied date 
	is invalid, this contains KErrArgument. */
	virtual void DateTimeToChineseL(const TDateTime& aDateTime, TChineseDate& aChineseDate)=0;

	/** Creates a TDateTime value from a Chinese date.
	
	@param aChineseDate A date in Chinese format.
	@param aDateTime On return, contains a date value. If the supplied date is 
	invalid, this contains KErrArgument. */
	virtual void ChineseToDateTimeL(const TChineseDate& aChineseDate, TDateTime& aDateTime)=0;

	/** Returns the range of dates, in standard date format, acceptable to the Chinese 
	calendar converter. 
	
	@param aLower On return, contains the lower limit of the converter.
	@param aUpper On return, contains the upper limit of the converter. */
	virtual void DateRange(TDateTime& aLower, TDateTime& aUpper)=0;

	/** Returns the range of dates, in Chinese date format, acceptable to the Chinese 
	calendar converter.
	
	@param aLower On return, contains the lower limit of the converter.
	@param aUpper On return, contains the upper limit of the converter. */
	virtual void DateRange(TChineseDate& aLower, TChineseDate& aUpper)=0;
	/** Returns the result of the last calendar conversion as a Julian date. 
	
	@return The Julian date. */
	virtual TReal JulianDate() __SOFTFP =0;
	};
#endif
