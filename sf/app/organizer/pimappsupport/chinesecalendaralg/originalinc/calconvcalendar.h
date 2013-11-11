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

#ifndef __CALENDAR_H__
#define __CALENDAR_H__

#include <e32std.h>
#include <e32def.h>


const TInt KCalConvDayOffsetByOne = 1;
const TInt KCalConvMonthOffsetByOne = 1;
const TInt KCalConvYearOffsetByOne = 1;
const TReal KCalConvPointFive = 0.5;


class TCalendar
	{
public:
	TCalendar();
	IMPORT_C TCalendar& operator=(const TCalendar& aTCal);
	IMPORT_C TReal JulianDay();

protected:
	void Round(TInt& aRes, TReal& aNum) const;
	void Mod(TReal& aRes, const TReal& aAbscissa, const TReal& aDenominator) const;
	void Amod(TReal& aRes, const TReal& aAbscissa, const TReal& aDenominator) const;
	void Floor(TInt& aRes, const TReal& aNum) const;
	void Ceiling(TInt& aRes, const TReal& aNum) const;

protected:
	TReal iStartEpoch;
	TInt iDaysInNormYear;
	TReal iJulianDay;
	TInt iCycle;
	TInt iLeaps;
	TInt iOffset;
	TInt iStdYearMonth;
	};


#endif
