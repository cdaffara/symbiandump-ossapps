// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "calcommonimpl.h"
#include <calcommon.h>

void CalUtils::Panic(TCalInterimApiPanicCode aPanic)
	{
	_LIT(KPanicString, "CalInterimAPI");
	User::Panic(KPanicString(), aPanic);
	}

TAgnRpt::TWeekInMonth CalUtils::WeekNumberToAgnWeekInMonthL(TInt8 aWeek)
	{
	TAgnRpt::TWeekInMonth weekInMonth(TAgnRpt::EFirst);
	switch (aWeek)
		{
		case -1:
			weekInMonth = TAgnRpt::ELast;
			break;
		case 1:
			weekInMonth = TAgnRpt::EFirst;
			break;
		case 2:
			weekInMonth = TAgnRpt::ESecond;
			break;
		case 3:
			weekInMonth = TAgnRpt::EThird;
			break;
		case 4:
			weekInMonth = TAgnRpt::EFourth;
			break;
		default:
			User::Leave(KErrArgument);
		}
	return weekInMonth;
	}

TInt8 CalUtils::AgnWeekInMonthToWeekNumberL(TAgnRpt::TWeekInMonth aWeek)
	{
	TInt8 week(1);
	switch (aWeek)
		{
		case TAgnRpt::ELast:
			week = -1;
			break;
		case TAgnRpt::EFirst:
			week = 1;
			break;
		case TAgnRpt::ESecond:
			week = 2;
			break;
		case TAgnRpt::EThird:
			week = 3;
			break;
		case TAgnRpt::EFourth:
			week = 4;
			break;
		default:
			User::Leave(KErrArgument);
		}
	return week;
	}

TAgnCalendarTime CalUtils::TCalTimeToTAgnCalendarTimeL(const TCalTime& aCalTime)
	{
	TAgnCalendarTime agnTime;
	if (aCalTime.TimeMode() == TCalTime::EFloating)
		{
		agnTime.SetFloatingL(aCalTime.TimeLocalL());
		}
	else
		{
		agnTime.SetUtcL(aCalTime.TimeUtcL());
		}
	return agnTime;
	}

TCalTime CalUtils::TAgnCalendarTimeToTCalTimeL(const TAgnCalendarTime& aAgnTime)
	{
	TCalTime calTime;
	if (aAgnTime.TimeMode() == TCalTime::EFloating)
		{
		calTime.SetTimeLocalFloatingL(aAgnTime.LocalL());
		}
	else
		{
		calTime.SetTimeUtcL(aAgnTime.UtcL());
		}
	return calTime;
	}

/** Constructs a TCalTimeRange.
@param aStartTime Start time of time range
@param aEndTime End time of time range
@publishedAll
@released
@capability None
*/
EXPORT_C CalCommon::TCalTimeRange::TCalTimeRange(TCalTime aStartTime, TCalTime aEndTime)
	:iStartTime(aStartTime),iEndTime(aEndTime)
	{
	};

/** Returns the start time of the range.
@return The start time of the range.
@publishedAll
@released
@capability None
*/
EXPORT_C TCalTime CalCommon::TCalTimeRange::StartTime() const
	{
	return iStartTime;
	}

/** Returns the end time of the range.
@return The end time of the range.
@publishedAll
@capability None
@released
*/
EXPORT_C TCalTime CalCommon::TCalTimeRange::EndTime() const
	{
	return iEndTime;
	}

/** Constructs a TCalPriorityRange that includes all priorities.

@publishedAll
@released
*/
EXPORT_C CalCommon::TCalPriorityRange::TCalPriorityRange()
	: iHighestPriority(0), iLowestPriority(KMaxTUint)
	{
	}

/** Constructs a TCalPriorityRange.

Lower numerical values have higher priority. So to filter on entries between priority 1 and 3
included aHighestPriority should be 1 and aLowestPriority should be 3.

@param aHighestPriority The highest priority in the range.
@param aLowestPriority The lowest priority in the range.
@publishedAll
@prototype
@capability None
*/
EXPORT_C CalCommon::TCalPriorityRange::TCalPriorityRange(TUint aHighestPriority, TUint aLowestPriority)
	: iHighestPriority(aHighestPriority), iLowestPriority(aLowestPriority)
	{
	}

/** Returns the lowest priority of the range.

@return The lowest priority of the range.
@publishedAll
@prototype
@capability None
*/
EXPORT_C TUint CalCommon::TCalPriorityRange::LowestPriority() const
	{
	return iLowestPriority;
	}

/** Returns the highest priority of the range.

@return The highest priority of the range.
@publishedAll
@prototype
@capability None
*/
EXPORT_C TUint CalCommon::TCalPriorityRange::HighestPriority() const
	{
	return iHighestPriority;
	}

/** Creates a default favourite filter. This filter will accept any value of
the favourite

@publishedAll
@released
*/
EXPORT_C CalCommon::TCalFavouriteFilter::TCalFavouriteFilter()
	: iValue(0), iMask(0)
	{
	}

/** Creates a user integer filter.

@param aValue The bit values that the bits of the user integer must match.
@param aMask A mask indicating which bits must be taken into account 
when doing the user integer filtering.
@publishedAll
@released
*/
EXPORT_C CalCommon::TCalFavouriteFilter::TCalFavouriteFilter(TUint32 aValue,
		TUint32 aMask)
	: iValue(aValue), iMask(aMask)
	{
	}

/** Returns the bit values that the bits of the user integer must match.

@return The bit values that the bits of the user integer must match.
@publishedAll
@released
*/
EXPORT_C TUint32 CalCommon::TCalFavouriteFilter::Value() const
	{
	return iValue;
	}

/** Returns a mask indicating which bits must be taken into account when 
doing the user integer filtering.

@return A mask indicating which bits must be taken into account when doing 
 the user integer filtering.
@publishedAll
@released
*/
EXPORT_C TUint32 CalCommon::TCalFavouriteFilter::Mask() const
	{
	return iMask;
	}

