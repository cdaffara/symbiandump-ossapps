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
// Implementation of the TArithmeticalCalendar class.
// 
//

// System includes
#include <e32math.h>

// User includes
#include "calconvArithmeticalCal.h"


//
// Construction/Destruction
//

//------------------------------------------------------
// Class:       TArithmeticalCalendar
// Function:    TArithmeticalCalendar
// Arguments:   None
//
// Comments:    Constructor
//
// Return:      None
//------------------------------------------------------
TArithmeticalCalendar::TArithmeticalCalendar()
	{
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
TInt TArithmeticalCalendar::DaysMonthsElapsed(const TInt aYear) const
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
// Function:    IsLeapYear
// Arguments:   const TInt , const TInt ,const TInt , const TInt
//
// Comments:    Determines whether the given year is a leap year.
//				Gregorian uses a different implimentation.
//
// Return:      ETrue if leap year, else EFalse
//------------------------------------------------------
TBool TArithmeticalCalendar::IsLeapYear(const TInt aYear) const
	{
	TReal result;
	TBool rtn;

	result = (aYear + iOffset) * iLeaps;
	Mod(result,result,iCycle);

	if(result < iLeaps)
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
// Class:       TArithmeticalCalendar
// Function:    DayMonthInYear
// Arguments:   None
//
// Comments:    This function Determines the month based on
//				the number of days into the year it is
//
// Return:      TInt - Month
//------------------------------------------------------
TInt TArithmeticalCalendar::DayMonthInYear(const TInt aDays) const
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
