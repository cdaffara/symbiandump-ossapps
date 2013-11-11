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
// Implementation of the TAstronomicalCalendar class.
// 
//

// System includes
#include <e32base.h>
#include <e32math.h>

// User includes
#include "calconv.h"
#include "calconvAstronomicalCal.h"

// Constants
const TReal KCoeff19th[] =
	{
	-0.00002,
	0.000297,
	0.025184,
	-0.181133,
	0.553040,
	-0.861938,
	0.677066,
	-0.212591,
	};

const TReal KCoeff18th[] =
	{
	-0.000009,
	0.003844,
	0.083563,
	0.865736,
	4.867575,
	15.845535,
	31.332267,
	38.291999,
	28.316289,
	11.636204,
	2.043794,
	};

const TInt KCoeffs[] =
	{
	403406,
	195207,
	119433,
	112392,
	3891,
	2819,
	1721,
	0,
	660,
	350,
	334,
	314,
	268,
	242,
	234,
	158,
	132,
	129,
	114,
	99,
	93,
	86,
	78,
	72,
	68,
	64,
	46,
	38,
	37,
	32,
	29,
	28,
	27,
	27,
	25,
	24,
	21,
	21,
	20,
	18,
	17,
	14,
	13,
	13,
	13,
	12,
	10,
	10,
	10,
	10,
	};

const TReal KMulti[] =
	{
	4.721964,
	5.937458,
	1.115589,
	5.781616,
	5.5474,
	1.512,
	4.1897,
	1.163,
	5.415,
	4.315,
	4.553,
	5.198,
	5.989,
	2.911,
	1.423,
	0.061,
	2.317,
	3.193,
	2.828,
	0.52,
	4.65,
	4.35,
	2.75,
	4.5,
	3.23,
	1.22,
	0.14,
	3.44,
	4.37,
	1.14,
	2.84,
	5.96,
	5.09,
	1.72,
	2.56,
	1.92,
	0.09,
	5.98,
	4.03,
	4.47,
	0.79,
	4.24,
	2.01,
	2.65,
	4.98,
	0.93,
	2.21,
	3.59,
	1.5,
	2.55,
	};

const TReal KAdd[] =
	{
	0.01621043,
	628.30348067,
	628.30821524,
	628.29634302,
	1256.605691,
	1256.60984,
	628.324766,
	0.00813,
	1256.5931,
	575.3385,
	-0.33931,
	7771.37715,
	786.04191,
	0.05412,
	393.02098,
	-0.34861,
	1150.67698,
	157.74337,
	52.9667,
	588.4927,
	52.9611,
	-39.807,
	522.3769,
	550.7647,
	2.6108,
	157.7385,
	1884.9103,
	-77.5655,
	2.6489,
	1179.0627,
	550.7575,
	-79.6139,
	1884.8981,
	21.3219,
	1097.7103,
	548.6856,
	254.4393,
	-557.3143,
	606.9774,
	21.3279,
	1097.7163,
	-77.5282,
	1884.9191,
	2.0781,
	294.2463,
	-0.0799,
	469.4114,
	-0.6829,
	214.6325,
	1572.084,
	};

const TReal KMuArray[] =
	{
	-0.40720,
	0.17241,
	0.01608,
	0.01039,
	0.00739,
	-0.00514,
	0.00208,
	-0.00111,
	-0.00057,
	0.00056,
	-0.00042,
	0.00042,
	0.00038,
	-0.00024,
	-0.00007,
	0.00004,
	0.00004,
	0.00003,
	0.00003,
	-0.00003,
	0.00003,
	-0.00002,
	-0.00002,
	0.00002,
	};

const TInt8 KOmegaArray[] =
	{
	0,
	1,
	0,
	0,
	1,
	1,
	2,
	0,
	0,
	1,
	0,
	1,
	1,
	1,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	};

const TInt8 KXiArray[] =
	{
	0,
	1,
	0,
	0,
	-1,
	1,
	2,
	0,
	0,
	1,
	0,
	1,
	1,
	-1,
	2,
	0,
	3,
	1,
	0,
	1,
	-1,
	-1,
	1,
	0,
	};

const TInt8 KGammaArray[] =
	{
	1,
	0,
	2,
	0,
	1,
	1,
	0,
	1,
	1,
	2,
	3,
	0,
	0,
	2,
	1,
	2,
	0,
	1,
	2,
	1,
	1,
	1,
	3,
	4,
	};

const TInt8 KZetaArray[] =
	{
	0,
	0,
	0,
	2,
	0,
	0,
	0,
	-2,
	2,
	0,
	0,
	2,
	-2,
	0,
	0,
	-2,
	0,
	-2,
	2,
	2,
	2,
	-2,
	0,
	0,
	};

const TReal KIotaArray[] =
	{
	299.77,
	251.88,
	251.83,
	349.42,
	84.66,
	141.74,
	207.14,
	154.84,
	34.52,
	207.19,
	291.34,
	161.72,
	239.56,
	331.55,
	};

const TReal KChiArray[] =
	{
	0.107408,
	0.016321,
	26.641886,
	36.412478,
	18.206239,
	53.303771,
	2.453732,
	7.306860,
	27.261239,
	0.121824,
	1.844379,
	24.198154,
	25.513099,
	3.592518,
	};

const TReal KNuArray[] =
	{
	-0.009173,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	};

const TReal KLamdaArray[] =
	{
	0.000325,
	0.000165,
	0.000164,
	0.000126,
	0.00011,
	0.000062,
	0.00006,
	0.000056,
	0.000047,
	0.000042,
	0.00004,
	0.000037,
	0.000035,
	0.000023,
	};

const TInt  KInitOne				= 1;
const TInt KHoursInDay				= 24;
const TInt KMinsInHour				= 60;
const TInt KSecsInMin				= 60;
const TInt KDaysInGregHundredYears	= 36525;
const TReal KMeanDaysInGregYear		= 365.25;
const TInt  KSquared				= 2;
const TInt  KCubed					= 3;
const TInt  KQuad					= 4;
const TInt K360Degrees				= 360;
const TReal KDegreesToRadians		= 0.017453292519943296; //(KPi/180);
const TInt  KNumElementsCoeff19th	= 8;
const TInt  KNumElementsCoeff18th	= 11;
const TInt  KNoOfPeriodicTerms		= 50;
const TInt  KMaxCorrectionValues	= 24;
const TReal KMonthsInTropicalYear	= 12.3685;
const TInt  KMaxAdditionalValues	= 14;

//
// Construction/Destruction
//

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    TAstronomicalCalendar
// Arguments:   None
//
// Comments:    Constructor
//
// Return:      None
//------------------------------------------------------
TAstronomicalCalendar::TAstronomicalCalendar()
	{

	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    UniversalFromLocal
// Arguments:   TReal , TInt 
//
// Comments:    Universal time is used for time keeping purposes.
//				It is given as a fraction on a solar day
//
// Return:      Universal time
//------------------------------------------------------
TReal TAstronomicalCalendar::UniversalFromLocal(const TReal& aTime,const TReal& aZone) const
	{
	return (aTime - (aZone / (KHoursInDay * KMinsInHour)));
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    LocalFromUniversal
// Arguments:   TReal , TInt 
//
// Comments:    Universal time is used for time keeping purposes.
//				It is given as a fraction on a solar day
//
// Return:      Local time
//------------------------------------------------------
TReal TAstronomicalCalendar::LocalFromUniversal(const TReal& aTime, const TReal& aZone) const
	{
	return (aTime + (aZone / (KHoursInDay * KMinsInHour)));
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendarL
// Function:    EphemerisCorrection
// Arguments:   TReal 
//
// Comments:    Astronomical calculations are performed using
//				Ephemeris time that is not affected by nutation
//				and aberation. 
//
// Return:      Correction value
//------------------------------------------------------
void TAstronomicalCalendar::EphemerisCorrection(const TReal aJulianDay, TReal& aCorrection) const
	{
	TGregorianCalendar greg(aJulianDay);
	TArithmeticalDate gregDate;
	TInt year;
	TReal theta;
	TReal x;
	aCorrection = 0;

	// get the value for the year
	greg.GetDate(gregDate);
	year = gregDate.iYear;
	// get theta value
	PopulateTheta(theta,year);
	// get x value
	EphemerisCorrPopX(year, x);

	// perform ephemeris correction
	if((year >= 1988) && (year <= 2019))
		{
		aCorrection = ((year - 1933) / (KHoursInDay * KMinsInHour * KSecsInMin));
		}
	else if((year >= 1900) && (year <= 1987))
		{
		GetPoly(KCoeff19th,theta,KNumElementsCoeff19th,aCorrection);
		}
	else if((year >= 1800) && (year <= 1899))
		{
		GetPoly(KCoeff18th,theta,KNumElementsCoeff18th, aCorrection);
		}
	else if((year >= 1620) && (year <= 1799))
		{
		TReal result;
		result = year - 1600;
		Math::Pow(result,result,KSquared);
		result = 196.58333 - (4.0675 * (year - 1600)) + 0.0219167 * result;
		aCorrection = result / (KHoursInDay * KMinsInHour * KSecsInMin);
		}
	else
		{
		TReal result;
		Math::Pow(result,x,KSquared);
		result = (result / 41048480) - 15;
		aCorrection = result / (KHoursInDay * KMinsInHour * KSecsInMin);
		}
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    PopulateTheta
// Arguments:   TReal&
//
// Comments:    Determines the constant for theta
//				This function is used for the ephemeris correction
//
// Return:      None
//------------------------------------------------------
void TAstronomicalCalendar::PopulateTheta(TReal &aTheta,TInt aYear) const
	{
	TGregorianCalendar greg;
	TArithmeticalDate gregDate1;
	TArithmeticalDate gregDate2;
	gregDate1.iDay = KCalConvFirstDay;
	gregDate1.iMonth = EJanuary + KCalConvMonthOffsetByOne;
	gregDate1.iYear = 1900;
	gregDate2.iDay = KCalConvFirstDay;
	gregDate2.iMonth = EJuly + KCalConvMonthOffsetByOne;
	gregDate2.iYear = aYear;
	aTheta = greg.GregDateDiff(gregDate1,gregDate2) / KDaysInGregHundredYears;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    EphemerisCorrPopX
// Arguments:   TReal&
//
// Comments:    Determines the constant for x
//				This function is used for the ephemeris correction
//
// Return:      TReal - ephemeris constant
//------------------------------------------------------
void TAstronomicalCalendar::EphemerisCorrPopX(const TInt aYear, TReal& aEmphCorr) const
	{
	TGregorianCalendar greg;
	TArithmeticalDate gregDate1;
	TArithmeticalDate gregDate2;
	gregDate1.iDay = KCalConvFirstDay;
	gregDate1.iMonth = EJanuary + KCalConvMonthOffsetByOne;
	gregDate1.iYear = 1810;
	gregDate2.iDay = KCalConvFirstDay;
	gregDate2.iMonth = EJanuary + KCalConvMonthOffsetByOne;
	gregDate2.iYear = aYear;
	aEmphCorr = 0.5 + greg.GregDateDiff(gregDate1,gregDate2);
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    GetPoly
// Arguments:   TReal& , CArrayFixFlat<TReal>& , TReal&
//
// Comments:    performs the following calculation
//
//										 2
//				SIGMA (Array[i] * Operand )
//				   i
//
// Return:      TReal - result of above calculation
//------------------------------------------------------
void TAstronomicalCalendar::GetPoly(const TReal* aArray,const TReal& aOperand, TInt aCount, TReal& aResult) const
	{
	TInt count;
	TReal poly;

	aResult = aArray[0];
	for(count = KInitOne; count < aCount; count++)
		{
		Math::Pow(poly,aOperand,count);
		aResult += aArray[count] * poly;
		}
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    EphemerisFromUniversal
// Arguments:   None
//
// Comments:    converts ephemeris time to universal time
//
// Return:      TReal - universal time
//------------------------------------------------------
void TAstronomicalCalendar::EphemerisFromUniversal(const TReal& aJulianDay, TReal& aCorrectedJD) const
	{
	TReal emphCorr;
	EphemerisCorrection(aJulianDay, emphCorr);
	aCorrectedJD = aJulianDay + emphCorr;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    UniversalFromEphemeris
// Arguments:   None
//
// Comments:    Converts universal time to ephemeris time
//
// Return:      TReal - ephemeris time
//------------------------------------------------------
TReal TAstronomicalCalendar::UniversalFromEphemeris(const TReal& aJulianDay) const
	{
	TReal result;
	EphemerisCorrection(aJulianDay, result);
	return (aJulianDay - result);
	}


//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    j2000
// Arguments:   None
//
// Comments:    Julian day value of January 1st 2000
//
// Return:      TReal - see comment
//------------------------------------------------------
void TAstronomicalCalendar::j2000(TReal& aJ2000) const
	{
	TGregorianCalendar greg;
	TArithmeticalDate date;

	date.iDay = KCalConvFirstDay;
	date.iMonth = EJanuary + KCalConvMonthOffsetByOne;
	date.iYear = 2000;

	aJ2000 = KCalConvPointFive + greg.GregToJulianDay(date);

	AdjustJDToNoon(aJ2000);
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    JulianCenturies
// Arguments:   None
//
// Comments:    Number of centuries before or after January
//				1st 2000 (Gregorian)
//
// Return:      TReal - centuries and fraction thereof
//------------------------------------------------------
void TAstronomicalCalendar::JulianCenturies(const TReal& aJulianDay, TReal& aJC) const
	{
	TReal emph;
	TReal julian2000;
	EphemerisFromUniversal(aJulianDay,emph);
	j2000(julian2000);

	aJC = ( emph- julian2000) / KDaysInGregHundredYears;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    SolarLongitudeL
// Arguments:   TReal&
//
// Comments:    calculates the longitude of the sun at 
//				any give Julian Day value.
//
// Return:      TReal - longitude
//------------------------------------------------------
void TAstronomicalCalendar::SolarLongitude(const TReal& aJulianDay, TReal& aTheta) const
	{
	TReal centuries;  // Julian centuries
	TReal longitude;  // longitude
	TReal aberration;
	TReal nutation;

	// get the julian centuries
	JulianCenturies(aJulianDay,centuries);

	// get the longitude
	GetLongitude(centuries, longitude);

	Aberration(centuries,aberration);
	Nutation(centuries,nutation);

	aTheta = (longitude + aberration + nutation);
	aTheta = (aTheta / KPi) * 180;
	Mod(aTheta,aTheta,K360Degrees);
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    Nutation
// Arguments:   TReal &
//
// Comments:    corrects for changes in celestial longitude
//				and latitude caused by the gravitational pull
//				of the sun and moon on the earth.
//
// Return:      TReal - correction value
//------------------------------------------------------
void TAstronomicalCalendar::Nutation(const TReal &aJulianCenturies, TReal& aNutation) const
	{
	TReal a;
	TReal b;

	a = 124.9 - (1934.134 * aJulianCenturies) + (0.002063 * aJulianCenturies * aJulianCenturies);
	a *=KDegreesToRadians;
	Math::Sin(a,a);

	b = 201.11 + (72001.5377 * aJulianCenturies) + (0.00057 * aJulianCenturies * aJulianCenturies);
	b *= KDegreesToRadians;
	Math::Sin(b,b);

	aNutation = (-0.0000834 * a) - (0.0000064 * b);
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    Aberration
// Arguments:   TReal &
//
// Comments:    This function makes a correction for the 
//				movement of the earth (the apparent movement
//				of the sun) in the time taken for light to 
//				travel to earth.
//
// Return:      TReal - correction in radians
//------------------------------------------------------
void TAstronomicalCalendar::Aberration(const TReal &aJulianCenturies, TReal& aAberration) const
	{
	aAberration = 177.63 + (35999.01848 * aJulianCenturies);
	aAberration *= KDegreesToRadians;
	Math::Cos(aAberration,aAberration);
	aAberration = (0.0000017 * aAberration) - 0.0000973;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    GetLongitude
// Arguments:   TReal&
//
// Comments:    calculates the longitude component of the 
//				solar longitude calculation.
//				This function is used by the SolarLongitudeL 
//				function.
//
// Return:      TReal - longitude
//------------------------------------------------------
void TAstronomicalCalendar::GetLongitude(const TReal& aC, TReal& aLongitude) const
	{
	TInt count;
	aLongitude = 0;

	// get longitude
	for(count = 0;count < KNoOfPeriodicTerms;count++)
		{
		TReal temp;
		temp = (KMulti[count] + (KAdd[count] * aC));
		// convert to radians for the Sin function
		Math::Sin(temp,temp);
		temp = KCoeffs[count] * temp;
		aLongitude += temp;
		}
	aLongitude = 4.9353929 + (628.33196168 * aC) + (0.0000001 * aLongitude);
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    DateNextSolarLongitudeL
// Arguments:   TReal , TReal
//
// Comments:    
//
// Return:      TReal
//------------------------------------------------------
TReal TAstronomicalCalendar::DateNextSolarLongitude(const TReal& aJulianDay,const TReal& aDegrees) const
	{
	TReal next;
	TInt nextInt;
	TReal upperLimit;
	TReal lowerLimit;
	TReal testDate;

	// get next
	SolarLongitude(aJulianDay,next);
	next/= aDegrees;
	Ceiling(nextInt,next);
	next = nextInt * aDegrees;
	Mod(next,next,K360Degrees);

	// this is designed to define a range (aJulianDay -> UpperLimit) 
	// that will allow the sun to pass though aDegrees once and once only
	upperLimit = aJulianDay + ((aDegrees / K360Degrees) * 400);
	lowerLimit = aJulianDay;

	for(testDate = (upperLimit + lowerLimit) / 2;
		(upperLimit - lowerLimit) >= 0.00001;
		testDate = (upperLimit + lowerLimit) / 2)
		{
		TReal solLong;
		SolarLongitude(testDate, solLong);
		if(next == 0)
			{
			if(solLong <= aDegrees)
				{
				upperLimit = testDate;
				}
			else
				{
				lowerLimit = testDate;
				}
			}
		else
			{
			if(solLong >= next)
				{
				upperLimit = testDate;
				}
			else
				{
				lowerLimit = testDate;
				}
			}
		}
	return testDate;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    NewMoonAtOrAfter
// Arguments:   TReal& 
//
// Comments:    Calculates the time of the new moon by deturmining
//				the sums of periodic terms.
//
// Return:      TReal - time of new moon
//------------------------------------------------------
TReal TAstronomicalCalendar::NewMoonAtOrAfter(const TReal& aJulianDay) const
	{
	TReal jDReal = aJulianDay;
	TReal result;
	TInt jD;


	AdjustJDFromNoon(jDReal);
	Floor(jD,jDReal);
	TGregorianCalendar greg(jDReal);
	AdjustJDToNoon(jDReal);

	TArithmeticalDate date;
	TInt approx;
	TInt error;
	TReal gamma;

	TReal temp;
	TInt count;

	// get date
	greg.GetDate(date);

	// get Gamma
	gamma = date.iYear + (greg.DayNumber(date) / KMeanDaysInGregYear) - 2000;

	// get approx
	temp = gamma * KMonthsInTropicalYear;
	Floor(approx,temp);
	approx--;

	// get error
	error = 0;

	for(count = approx;NewMoonTime(count) < aJulianDay;count++)
		{
		error++;
		}

	// calc return
	result = NewMoonTime(approx + error);
	return result;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    NewMoonTime
// Arguments:   TInt
//
// Comments:    This function is used as part of the calculation
//				for NewMoonAtOrAfter()
//
// Return:      TReal - derived value
//------------------------------------------------------
TReal TAstronomicalCalendar::NewMoonTime(TInt aTime) const
	{
	TReal newMoonTime;
	TReal kappa;
	TReal jde;
	TReal epsilon;
	TReal solarAnomaly;
	TReal lunarAnomaly;
	TReal moonArgument;
	TReal omega;
	TReal correction;
	TReal additional;

	TReal temp;
	TInt count;

	// get Kappa
	kappa = aTime / 1236.85;

	// get jde
	jde = 2451550.09765;
	temp = KMeanSynodicMonth * 1236.85 * kappa;
	jde += temp;
	Math::Pow(temp,kappa,KSquared);
	jde += temp * 0.0001337;
	Math::Pow(temp,kappa,KCubed);
	jde -= temp * 0.00000015;
	Math::Pow(temp,kappa,KQuad);
	jde += temp * 0.00000000073;

	// get epsilon
	epsilon = 1 - (0.002516 * kappa);
	Math::Pow(temp,kappa,KSquared);
	epsilon -= temp * 0.0000074;

	// get solar anomaly
	solarAnomaly = 2.5534 + (29.10535669 * 1236.85 * kappa);
	Math::Pow(temp,kappa,KSquared);
	solarAnomaly -= temp * 0.0000218;
	Math::Pow(temp,kappa,KCubed);
	solarAnomaly -= temp * 0.00000011;

	// get lunar anomaly
	lunarAnomaly = 201.5643 + (385.81693528 * 1236.85 * kappa);
	Math::Pow(temp,kappa,KSquared);
	lunarAnomaly += temp * 0.0107438;
	Math::Pow(temp,kappa,KCubed);
	lunarAnomaly += temp * 0.00001239;
	Math::Pow(temp,kappa,KQuad);
	lunarAnomaly -= temp * 0.000000058;

	// get moon argument
	moonArgument = 160.7108 + (390.67050274 * 1236.85 * kappa);
	Math::Pow(temp,kappa,KSquared);
	moonArgument -= temp * 0.0016341;
	Math::Pow(temp,kappa,KCubed);
	moonArgument -= temp * 0.00000227;
	Math::Pow(temp,kappa,KQuad);
	moonArgument += temp * 0.000000011;

	// get omega
	omega = 124.7746 + (-1.5637558 * 1236.85 * kappa);
	Math::Pow(temp,kappa,KSquared);
	omega += temp * 0.0020691;
	Math::Pow(temp,kappa,KCubed);
	omega += temp * 0.00000215;

	// convert to radians for the Sin function
	omega *= KDegreesToRadians;
	Math::Sin(correction,omega);
	correction = -0.00017 * correction;
	for(count = 0; count < KMaxCorrectionValues;count++)
		{
		TReal tempCorrection;
		TReal epsilonToTheOmega;
		tempCorrection = (KXiArray[count] * solarAnomaly) + 
						(KGammaArray[count] * lunarAnomaly) +
						(KZetaArray[count] * moonArgument);
		// convert to radians for the Sin function
		tempCorrection *= KDegreesToRadians;
		Math::Sin(tempCorrection,tempCorrection);
		Math::Pow(epsilonToTheOmega,epsilon,KOmegaArray[count]);
		tempCorrection = KMuArray[count] * epsilonToTheOmega * tempCorrection;
		correction += tempCorrection;
		}
	
	for(count = 0,additional = 0;count < KMaxAdditionalValues;count++)
		{
		TReal addnTemp;
		Math::Pow(addnTemp,kappa,KSquared);
		addnTemp = KIotaArray[count] + (KChiArray[count] * aTime) +
					(KNuArray[count] * addnTemp);
		// convert to radians for the Sin function
		addnTemp *= KDegreesToRadians;
		Math::Sin(addnTemp,addnTemp);
		addnTemp = KLamdaArray[count] * addnTemp;
		additional += addnTemp;
		}

	// calculate result
	newMoonTime = jde + correction + additional;
	newMoonTime = UniversalFromEphemeris(newMoonTime);
	return newMoonTime;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    NewMoonBefore
// Arguments:   
//
// Comments:    
//
// Return:      
//------------------------------------------------------
TReal TAstronomicalCalendar::NewMoonBefore(const TReal &aJulianDay) const
	{
	TReal result;
	result = NewMoonAtOrAfter(aJulianDay);
	result-=45;
	result = NewMoonAtOrAfter(result);
	return result;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    AdjustJDToNoon
// Arguments:   TReal&
//
// Comments:    This function corrects for the fact that the
//				julian day starts at noon however the julian
//				day held in the base class starts on the 
//				preceeding midnight.
//
// Return:      None
//------------------------------------------------------
void TAstronomicalCalendar::AdjustJDToNoon(TReal &aJulianDay) const
	{
	aJulianDay -= KCalConvPointFive;
	}

//------------------------------------------------------
// Class:       TAstronomicalCalendar
// Function:    AdjustJDFromNoon
// Arguments:   TReal&
//
// Comments:    This function corrects for the fact that the
//				julian day starts at noon however the julian
//				day held in the base class starts on the 
//				preceeding midnight.
//
// Return:      None
//------------------------------------------------------
void TAstronomicalCalendar::AdjustJDFromNoon(TReal &aJulianDay) const
	{
	aJulianDay += KCalConvPointFive;
	}
