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

#ifndef __ASTRONOMICALCAL_H__
#define __ASTRONOMICALCAL_H__


#include <e32std.h>
#include <e32base.h>
#include "calconvcalendar.h"

const TInt KCalConvFirstDay = 1;
const TReal KMeanSynodicMonth = 29.530588853;


class TAstronomicalCalendar : public TCalendar  
	{
public:
	TAstronomicalCalendar();

protected:
	void  AdjustJDFromNoon(TReal& aJulianDay) const;
	void  AdjustJDToNoon(TReal& aJulianDay) const;
	TReal NewMoonAtOrAfter(const TReal& aJulianDay) const;
	void  EphemerisCorrection(const TReal aJulianDay, TReal& emphCorr) const;
	TReal LocalFromUniversal(const TReal& aTime, const TReal& aZone) const;
	TReal UniversalFromLocal(const TReal& aTime, const TReal& aZone) const;
	TReal DateNextSolarLongitude(const TReal& aJulianDay, const TReal& aDegrees) const;
	void  SolarLongitude(const TReal& aJulianDay, TReal& aTheta) const;
	void  JulianCenturies(const TReal& aJulianDay, TReal& aJC) const;
	void  EphemerisFromUniversal(const TReal& aJulianDay, TReal& aCorrectedJD) const;
	TReal UniversalFromEphemeris(const TReal& aJulianDay) const;
	void  EphemerisCorrPopX(const TInt aYear, TReal& aEmphCorr) const;
	TReal NewMoonBefore(const TReal& aJulianDay) const;
private:
	TReal NewMoonTime(TInt aTime) const;
	void  GetLongitude(const TReal& aC, TReal& aLongitude) const;
	void  Aberration(const TReal& aJulianCenturies, TReal& aAberration) const;
	void  Nutation(const TReal& aJulianCenturies, TReal& aNutation) const;
	void  j2000(TReal& aJ2000) const;
	void  GetPoly(const TReal* aArray, const TReal& aOperand, TInt aCount, TReal& aResult) const;
	void  PopulateTheta(TReal& aTheta, TInt aYear) const;
	};

#endif
