// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __AGMDATE_H__
#define __AGMDATE_H__

#include "agmcalendartime.h"

const TInt KAgnMinutesInADay = 1440;

NONSHARABLE_CLASS(AgnDateTime)
/** Provides static functions for converting dates and times between formats and 
for retrieving the minimum and maximum dates in the agenda model's valid date 
range (from 1st January 1980 to 31st December 2100). 
@internalAll
@released
*/
	{
public:
	static void AddTo(TDateTime& aDate, TTimeIntervalYears aYears);
	static void AddTo(TDateTime& aDate, TTimeIntervalMonths aMonths);
	static TTimeIntervalMonths MonthsFrom(const TDateTime& aStartDate, const TDateTime& aEndDate);
	static TTimeIntervalYears YearsFrom(const TDateTime& aStartDate, const TDateTime& aEndDate);
	static TBool IsLessThan(const TDateTime& aDateLeft, const TDateTime& aDateRight);
	
	IMPORT_C static TTime MinDate();
	IMPORT_C static TTime MaxDate();
	
	IMPORT_C static TBool IsValidAgendaTTime(const TTime& aDate);

	static const MAgnCalendarTimeMode* FixedTimeMode();
	static const MAgnCalendarTimeMode* FloatingTimeMode();
	
	IMPORT_C static TTime ConvertToLocalTimeL(const TTime& aUtcTime);
	IMPORT_C static TTime ConvertToUtcTimeL(const TTime& aLocalTime);

    static CAgnTlsProxy* GenerateTlsIfRequiredL(TBool& aGenerated);
    static void ClearTls(TAny* );

	IMPORT_C static TTime ResetToMidnight(const TTime& aTime);
	static TInt DaysBetweenDates(const TTime& aTimeLater, const TTime& aTimeEarlier);
	};

#endif
