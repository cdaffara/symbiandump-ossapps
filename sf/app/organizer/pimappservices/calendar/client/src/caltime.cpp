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

/**
 @file
 @publishedAll 
 @released 
*/


#include <caltime.h>

// dependency on AgnModel for time constants and conversions
#include "agmdate.h"

/** Constructor for the TCalTime class. 
The time is initialised to Time::NullTTime(). 

@publishedAll
@released
@capability None
*/
EXPORT_C TCalTime::TCalTime()
	{
	iTime = Time::NullTTime();
	iTimeMode = EFixedUtc;
	iReserved1 = 0;
	iReserved2 = 0;
	}
	
/**
Sets the time to a UTC value.  Time values set using this API are said to be "fixed".
Fixed times in UTC format can be converted to the equivalent local time in any time 
zone using RTz, or converted to the local time of the current system time zone by
calling TimeLocalL().  This is described in RFC2445 as FORM #2: DATE WITH UTC TIME.

@capability None
@param aUtcTime The time to be set in UTC. 
@leave KErrArgument If the given time is not null but smaller than TCalTime::MinTime() or bigger than TCalTime::MaxTime()
*/
EXPORT_C void TCalTime::SetTimeUtcL(const TTime& aUtcTime)
	{
	if (aUtcTime != Time::NullTTime() && (aUtcTime < MinTime() || aUtcTime > MaxTime()))
		{
		User::Leave(KErrArgument);
		}
	iTime = aUtcTime; 
	iTimeMode = static_cast<TUint8>(EFixedUtc);
	}
	
/**
Sets the time to a local time value bound to the current system time zone.  Time values
set using this API are said to be "fixed".  The local time can be converted to an
equivalent local time in any other time zone by calling TimeUtcL() and then converting
the UTC time to the equivalent local time zone using RTz.  This is described in RFC2445
as FORM #3: DATE WITH LOCAL TIME AND TIME ZONE REFERENCE.

@capability None
@param aLocalTime The time to be set in system-local time. 
@leave KErrArgument If the given time is not null but smaller than TCalTime::MinTime() or bigger than TCalTime::MaxTime()
*/
EXPORT_C void TCalTime::SetTimeLocalL(const TTime& aLocalTime)
	{
	TTime utcTime = AgnDateTime::ConvertToUtcTimeL(aLocalTime);
	if (utcTime != Time::NullTTime() && (utcTime < MinTime() || utcTime > MaxTime()))
		{
		User::Leave(KErrArgument);
		}
	iTime = utcTime;
	iTimeMode = static_cast<TUint8>(EFixedUtc);
	}
	
/**
Returns the fixed or floating time as UTC.

@capability None
@return The time in UTC. 
*/
EXPORT_C TTime TCalTime::TimeUtcL() const
	{
	if (TimeMode() == EFloating)
		{
		return AgnDateTime::ConvertToUtcTimeL(iTime);
		}
	else
		{
		return iTime;
		}
	}
	
/**
Returns the fixed or floating time as local time.  If the time is fixed, the time is converted to the local time of the 
current system time zone before being returned.

@capability None
@return The time in system-local time. 
*/
EXPORT_C TTime TCalTime::TimeLocalL() const
	{
	if (TimeMode() == EFloating)
		{
		return iTime;
		}
	else
		{
		return AgnDateTime::ConvertToLocalTimeL(iTime);
		}
	}

/** Get the maximum time allowed in the Interim API.
Note that this time is the same for UTC and system local time - it can't be converted.

@publishedAll
@released
@capability None
@return The maximum time. 
*/
EXPORT_C TTime TCalTime::MaxTime()
	{
	return AgnDateTime::MaxDate();
	}

/** Get the minimum time allowed in the Interim API.
Note that this time is the same for UTC and system local time - it can't be converted.

@publishedAll
@released
@capability None
@return The minimum time.
*/
EXPORT_C TTime TCalTime::MinTime()
	{
	return AgnDateTime::MinDate();
	}


/**
Sets the time to a local floating time value.  Time values set using this API are said to be
"floating" and are not bound to any time zone in particular.  Floating time is
used to represent the same time regardless of which time zone it is currently being
observed from.	 This is described in RFC2445 as FORM #1: DATE WITH LOCAL TIME.

@publishedAll
@released

@capability None
@param aLocalTime The time to be set in floating local time. 
@leave KErrArgument If the given time is not null but smaller than TCalTime::MinTime() or bigger than TCalTime::MaxTime()
*/
EXPORT_C void TCalTime::SetTimeLocalFloatingL(const TTime& aLocalTime)
	{
	if (aLocalTime != Time::NullTTime() && (aLocalTime < MinTime() || aLocalTime > MaxTime()))
		{
		User::Leave(KErrArgument);
		}
	iTime = aLocalTime;
	iTimeMode = static_cast<TUint8>(EFloating);
	}

/**
Returns the time mode that the time has been set to, whether it be floating local time,
fixed UTC time, or fixed local time.

@publishedAll
@released

@return Time mode of the calendar time
*/
EXPORT_C TCalTime::TTimeMode TCalTime::TimeMode() const
	{
	return static_cast<TTimeMode>(iTimeMode);
	}
