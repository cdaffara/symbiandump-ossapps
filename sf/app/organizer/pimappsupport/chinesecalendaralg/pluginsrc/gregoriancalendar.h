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

#ifndef __GREGORIANCALENDAR_H__
#define __GREGORIANCALENDAR_H__

#include "calendar.h"
#include "arithmeticaldate.h"

class TGregorianCalendar : public TCalendar
	{
public:
	TGregorianCalendar();
	TGregorianCalendar(TReal aJulianDay);
	void GregorianToDateTime(TDateTime& aDT);
	void GregFromJulianDay(TArithmeticalDate& aDate, TReal aJulianDay) const;
	TReal GregToJulianDay(const TArithmeticalDate& aYear) const;
private:
	void InitMembers();
	TInt DayNumber(const TArithmeticalDate& aDate) const;
	void CalcGregYear(TArithmeticalDate& aDate, TReal aJulianDay) const;
	TBool IsLeapYear(const TInt aYear) const;
	TInt GregDateDiff(const TArithmeticalDate aDate1, const TArithmeticalDate aDate2) const;
	TInt DayMonthInYear(const TInt) const;
	TInt DaysMonthsElapsed(const TInt) const;
	};

#endif
