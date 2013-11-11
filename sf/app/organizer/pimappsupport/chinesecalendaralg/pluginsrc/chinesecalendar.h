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

#ifndef __CHINESECALENDAR_H__
#define __CHINESECALENDAR_H__

#include "calcondata.h"
#include <calendarconverter.h>

class TCalconData
	{
public:
	TUint GetData(TInt aCycle, TInt aYear, TInt aMonth) const;
	TUint GetNewYear(TInt aCycle, TInt aYear) const;

private:
	static const TUint16 iCalConDataMonth[KNumberOfYears];
	static const TUint32 iCalConDataYear[KNumberOfYears+1];	//marker on end of table
	};

class TChineseCalendar
	{
public:
	void DateTimeToChinese(const TDateTime& aDT);
	void ChineseToDateTime(TDateTime& aDT);
	TInt SetDate(const TChineseDate& aDate);
	TInt GetDate(TChineseDate& aDate);
	TInt ChineseFromFixed(TChineseDate& aDate,const TReal& aJulianDay) const;
	TReal JulianDate() __SOFTFP;

private:
	TBool ValidDate(const TChineseDate &aDate) const;
	TBool ChineseToFixed(const TChineseDate& aDate, TReal& aJulianDay) const;
	TReal iJulianDay;
	TCalconData iData;
	};

#endif
