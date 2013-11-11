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
// Implementation of the TCalendar class.
//

#include <e32def.h>
#include <e32math.h>
#include "calendar.h"

// Constants
const TReal KRemainderApproxZero = 0.000000001;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//------------------------------------------------------
// Class:       TCalendar
// Function:    TCalendar
// Arguments:   None
//
// Comments:    Constructor
//
// Return:      none
//------------------------------------------------------
TCalendar::TCalendar()
	{
	iJulianDay = 0;
	}


//------------------------------------------------------
// Class:       TCalendar
// Function:    operator =
// Arguments:   TCalendar &TCal  -  the TCalendar
//				class by referance.
//
// Comments:    Copies the JulianDay value from one instance 
//				of TCalendar to an other.
//
// Return:      *this
//------------------------------------------------------
TCalendar& TCalendar::operator =(const TCalendar &TCal)
	{
	iJulianDay = TCal.iJulianDay;
	return (*this);
	}

//------------------------------------------------------
// Class:       TCalendar
// Function:    Floor
// Arguments:   TReal& ,TReal& 
//
// Comments:    This function returns the floor value of the
//				value given in the aNum argument as Determined by:
//
//				the largest integer less than or equal to aNum
//
// Return:      None
//------------------------------------------------------
void TCalendar::Floor(TInt& aRes,const TReal& aNum) const
	{
	TReal tempReal;
	TInt32 tempReal32;
	TInt precision = 0;

	TReal wholeNoTemp;
	Math::Frac(wholeNoTemp,aNum);
	if(wholeNoTemp < 0)
		{
		wholeNoTemp = -wholeNoTemp;
		}

	if((aNum == 0) || (wholeNoTemp < KRemainderApproxZero))
		{
		tempReal = aNum;
		}
	else
		{
		tempReal = aNum - KCalConvPointFive;
		}
	Math::Round(tempReal,tempReal,precision);
	Math::Int(tempReal32,tempReal);
	aRes = tempReal32;
	}

//------------------------------------------------------
// Class:       TCalendar
// Function:    Ceiling
// Arguments:   TReal& ,TReal& 
//
// Comments:    This function returns the ceiling value of the
//				value given in the aNum argument as Determined by:
//
//				the smallest integer less than or equal to aNum
//
// Return:      None
//------------------------------------------------------
void TCalendar::Ceiling(TInt& aRes,const TReal& aNum) const
	{
	TReal tempReal;
	TInt32 tempReal32;
	TInt precision = 0;

	TReal wholeNoTemp;
	Math::Frac(wholeNoTemp,aNum);
	if(wholeNoTemp < 0)
		{
		wholeNoTemp = -wholeNoTemp;
		}

	if((aNum == 0) || (wholeNoTemp < KRemainderApproxZero))
		{
		tempReal = aNum;
		}
	else
		{
		tempReal = aNum + KCalConvPointFive;
		}
	Math::Round(tempReal,tempReal,precision);
	Math::Int(tempReal32,tempReal);
	aRes = tempReal32;
	}

//------------------------------------------------------
// Class:       TCalendar
// Function:    Mod
// Arguments:   TReal &, const TReal &, const TReal &
//
// Comments:    return the mod of the aAbscissa / aDenominator
//				quotient as Determined by:
//				
//				aAbscissa - aDenominator(floor(aAbscissa / aDenominator))
//
// Return:      None
//------------------------------------------------------
void TCalendar::Mod(TReal &aRes, const TReal &aAbscissa, const TReal &aDenominator) const
	{
	TReal quo;
	TInt tempInt;

	quo = aAbscissa / aDenominator;

	Floor(tempInt,quo);

	aRes = aAbscissa - (aDenominator * tempInt);
	}

//------------------------------------------------------
// Class:       TCalendar
// Function:    Amod
// Arguments:   TReal &, const TReal &, const TReal &
//
// Comments:    Determines the adjusted remainder
//
// Return:      None
//------------------------------------------------------
void TCalendar::Amod(TReal &aRes, const TReal &aAbscissa, const TReal &aDenominator) const
	{
	Mod(aRes,aAbscissa,aDenominator);

	if(aRes == 0)
		{
		aRes = aDenominator;
		}
	}

//------------------------------------------------------
// Class:       TCalendar
// Function:    Round
// Arguments:   TInt &, TReal &
//
// Comments:    rounds of the aNum argument
//
// Return:      None
//------------------------------------------------------
void TCalendar::Round(TInt &aRes, TReal &aNum) const
	{
	TReal temp;
	temp = aNum + 0.5;
	Floor(aRes,temp);
	}

//------------------------------------------------------
// Class:       TCalendar
// Function:    JulianDay
// Arguments:   None
//
// Comments:    Returns the Julian day as a TInt
//
// Return:      TInt
//------------------------------------------------------
TInt TCalendar::JulianDay()
	{
	return (TInt)iJulianDay;
	}
