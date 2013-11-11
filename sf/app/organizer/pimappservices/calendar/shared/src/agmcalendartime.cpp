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


#include "agmdate.h"
#include "agmtlsproxy.h"
#include "agmpanic.h"

#include <tz.h>
#include <tzconverter.h>
#include <vtzrules.h>

const TInt16  KOffsetUnspecified = KMaxTInt16;
const TUint16 KTzIdFloating = 0x8000;
const TUint16 KTzIdUnspecified = 0x7fff;

const TUint32 KAgnNewCalTimeMask = (1 << 30);
const TUint32 KAgmTimeFloatingFlag = 0x01;

// TAgnCalendarTime //

EXPORT_C TAgnCalendarTime::TAgnCalendarTime() : 
	iTime(Time::NullTTime()), 
	iLocalOffsetInMinutes(KOffsetUnspecified), 
	iTzId(KTzIdUnspecified), 
	iTimeZoneAccessor(NULL)
	{
	}

EXPORT_C void TAgnCalendarTime::SetFloatingL(const TTime& aTime)
	{
	SetDateTimeL(aTime, ETrue, MAgnCalendarTimeMode::ELocal);
	}

EXPORT_C void TAgnCalendarTime::SetLocalL(const TTime& aTime)
	{
	SetDateTimeL(aTime, EFalse, MAgnCalendarTimeMode::ELocal);
	}
	
EXPORT_C void TAgnCalendarTime::SetUtcL(const TTime& aTime)
	{
	SetDateTimeL(aTime, EFalse, MAgnCalendarTimeMode::EUtc);
	}

void TAgnCalendarTime::SetNull()
	{
	iTime = Time::NullTTime();
	iLocalOffsetInMinutes = KOffsetUnspecified;
	}
	
EXPORT_C TTime TAgnCalendarTime::LocalL() const
	{
	if (TimeMode() == MAgnCalendarTimeMode::EFixedTimeZone)
		{
		return DateTimeL(MAgnCalendarTimeMode::EFixedLocal);
		}
	return DateTimeL(MAgnCalendarTimeMode::ELocal);
	}

EXPORT_C TTime TAgnCalendarTime::UtcL() const
	{
	return DateTimeL(MAgnCalendarTimeMode::EUtc);
	}

void TAgnCalendarTime::SetDateTimeL(const TTime& aTime, TBool aFloating, MAgnCalendarTimeMode::TFormat aFormat)
/** Set the time and time mode of the AgnCalendarTime
@internalAll

@param aTime The time to be stored, can be UTC if aFormat is EUtc, or local if aFormat is ELocal or EFixedLocal
@param aMode Reference to a MAgnCalendarTimeMode derived time mode class. This time mode must be retrieved from AgnDateTime or CAgnTlsProxy
@param aFormat The time format of aTime
*/
	{
	if (!AgnDateTime::IsValidAgendaTTime(aTime))
		{
		// If the time is outside the range between MinDate and MaxDate, don't 
		// convert it, but normalise it to the max or min datetimes as 
		// appropriate.
		if(aTime == Time::NullTTime())
			{
			iTime = aTime;	
			}
		else if(aTime <= AgnDateTime::MinDate())
			{
			iTime = AgnDateTime::MinDate();
			}
		else if(aTime >= AgnDateTime::MaxDate())
			{
			iTime = AgnDateTime::MaxDate();
			}
 		iLocalOffsetInMinutes = KOffsetUnspecified;
 		if ( ! aFloating)
            { 
            SetFloatingFlag(EFalse);
            }
 		else
            {
            SetFloatingFlag(ETrue);
            }
		}
	else
		{
		if ( ! aFloating)
			{
			if (aFormat == MAgnCalendarTimeMode::EUtc)
				{
				// If the time is UTC, just store it. The offset is unknown at this point.
				iTime = aTime;
				iLocalOffsetInMinutes = KOffsetUnspecified;
				}
			else
				{
				// If the time is local, convert it and store the offset as well as the time.
				TTime localTime(aTime);
				TTime utcTime(aTime);
				TimeZoneAccessor()->FixedTimeMode().ToL(aFormat, utcTime);
				//In order to ensure UTC time is still within the range of Max and Min time
				//after the conversion. Any UTC time that is not within the range is converted
				//to Max or Min time.
				if (utcTime <= AgnDateTime::MinDate())
					{
					localTime = utcTime = AgnDateTime::MinDate();
					}
				else if (utcTime >= AgnDateTime::MaxDate())
					{
					localTime = utcTime = AgnDateTime::MaxDate();
					}
				iTime = utcTime;
				StoreNewOffset(localTime);
				SetTzId(TimeZoneAccessor()->CurrentTzId());
				}
			SetFloatingFlag(EFalse);
			}
		else
			{
			// floating time
			if (aFormat == MAgnCalendarTimeMode::EUtc)
				{
				TTime utcTime(aTime);
				TTime localTime(aTime);
				TimeZoneAccessor()->FloatingTimeMode().ToL(aFormat, localTime);
				iTime = localTime;
				StoreNewOffset(utcTime);
				SetTzId(TimeZoneAccessor()->CurrentTzId());
				}
			else
				{
				iTime = aTime;
				iLocalOffsetInMinutes = KOffsetUnspecified;
				}
			SetFloatingFlag(ETrue);
			}
		}
	__ASSERT_DEBUG(IsValidTime(), User::Invariant());
	}

EXPORT_C TTime TAgnCalendarTime::DateTimeL(MAgnCalendarTimeMode::TFormat aFormat) const
	{
	if(iTime <= AgnDateTime::MinDate() || iTime >= AgnDateTime::MaxDate() || iTime == Time::NullTTime())
		{
		// If the time is outside the range between MinDate and MaxDate, don't convert it. 
		// If the time is to be returned in UTC, don't convert it. 
		return iTime;
		}

	if ((TimeMode() == MAgnCalendarTimeMode::EFixedUtc && aFormat == MAgnCalendarTimeMode::EUtc) ||
		(TimeMode() == MAgnCalendarTimeMode::EFloating && aFormat == MAgnCalendarTimeMode::ELocal) ||
		(TimeMode() == MAgnCalendarTimeMode::EFloating && aFormat == MAgnCalendarTimeMode::EFixedLocal) ||
		(TimeMode() == MAgnCalendarTimeMode::EFixedTimeZone && aFormat == MAgnCalendarTimeMode::EFixedLocal))
		{
		return iTime;
		}
	
	// In this case the time is to be converted
	TUint16 tzId = TimeZoneAccessor()->CurrentTzId(); // Check whether or not the time zone has changed. 
	
	// Note that each TAgnCalendarTime's offset is updated on demand, so there is no need to update all 
	// offsets when the local time zone changes.
	
	if (TimeMode() == MAgnCalendarTimeMode::EFixedUtc)
		{
		if (iLocalOffsetInMinutes == KOffsetUnspecified ||
			TzId() != tzId)
			{
			// If the offset is unknown or the time zone has changed, recalculate the offset.
			TTime newLocalTime(iTime);
			CalendarTimeMode()->FromL(aFormat, newLocalTime);
			StoreNewOffset(newLocalTime);
			SetTzId(tzId);
			}
		}
	else if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		if (iLocalOffsetInMinutes == KOffsetUnspecified ||
			TzId() != tzId)
			{
			// If the offset is unknown or the time zone has changed, recalculate the offset.
			TTime newUtcTime(iTime);
			CalendarTimeMode()->FromL(aFormat, newUtcTime);
			StoreNewOffset(newUtcTime);
			SetTzId(tzId);
			}
		}
	else // using rule time mode
		{
		User::Leave(KErrNotSupported);
		}
			
	return iTime + TTimeIntervalMinutes(iLocalOffsetInMinutes);
	}

	
void TAgnCalendarTime::StoreNewOffset(const TTime& aTime) const
	{
	TTimeIntervalMinutes mins;
	aTime.MinutesFrom(iTime, mins);
	iLocalOffsetInMinutes = mins.Int();
	}

CAgnTlsProxy* TAgnCalendarTime::TimeZoneAccessor() const
	{
	if (!iTimeZoneAccessor)
		{
		iTimeZoneAccessor = static_cast<CAgnTlsProxy*>(Dll::Tls());
		__ASSERT_ALWAYS(iTimeZoneAccessor, User::Invariant());
		}
	return iTimeZoneAccessor;
	}

EXPORT_C TBool TAgnCalendarTime::IsSet() const
	{
	return (iTime != Time::NullTTime());
	}


	
EXPORT_C TBool TAgnCalendarTime::operator==(const TAgnCalendarTime& aTime) const
	{
	if (TimeMode() == aTime.TimeMode() &&
		(TimeMode() == MAgnCalendarTimeMode::EFixedUtc || TimeMode() == MAgnCalendarTimeMode::EFloating))
		{
		// both times are either floating or fixed, no conversion needed
		return (iTime == aTime.iTime);
		}
	else
		{
		return (UtcL() == aTime.UtcL());
		}
	}



EXPORT_C TBool TAgnCalendarTime::operator!=(const TAgnCalendarTime& aTime) const
	{
	if (TimeMode() == aTime.TimeMode() &&
		(TimeMode() == MAgnCalendarTimeMode::EFixedUtc || TimeMode() == MAgnCalendarTimeMode::EFloating))
		{
		// both times are either floating or fixed, no conversion needed
		return (iTime != aTime.iTime);
		}
	else
		{
		return (UtcL() != aTime.UtcL());
		}
	}



EXPORT_C TBool TAgnCalendarTime::operator<(const TAgnCalendarTime& aTime) const
	{
	if (TimeMode() == aTime.TimeMode() &&
		(TimeMode() == MAgnCalendarTimeMode::EFixedUtc || TimeMode() == MAgnCalendarTimeMode::EFloating))
		{
		// both times are either floating or fixed, no conversion needed
		return (iTime < aTime.iTime);
		}
	else
		{
		return (UtcL() < aTime.UtcL());
		}
	}



EXPORT_C TBool TAgnCalendarTime::operator>(const TAgnCalendarTime& aTime) const
	{
	if (TimeMode() == aTime.TimeMode() &&
		(TimeMode() == MAgnCalendarTimeMode::EFixedUtc || TimeMode() == MAgnCalendarTimeMode::EFloating))
		{
		// both times are either floating or fixed, no conversion needed
		return (iTime > aTime.iTime);
		}
	else
		{
		return (UtcL() > aTime.UtcL());
		}
	}



EXPORT_C TBool TAgnCalendarTime::operator<=(const TAgnCalendarTime& aTime) const
	{
	if (TimeMode() == aTime.TimeMode() &&
		(TimeMode() == MAgnCalendarTimeMode::EFixedUtc || TimeMode() == MAgnCalendarTimeMode::EFloating))
		{
		// both times are either floating or fixed, no conversion needed
		return (iTime <= aTime.iTime);
		}
	else
		{
		return (UtcL() <= aTime.UtcL());
		}
	}



EXPORT_C TBool TAgnCalendarTime::operator>=(const TAgnCalendarTime& aTime) const
	{
	if (TimeMode() == aTime.TimeMode() &&
		(TimeMode() == MAgnCalendarTimeMode::EFixedUtc || TimeMode() == MAgnCalendarTimeMode::EFloating))
		{
		// both times are either floating or fixed, no conversion needed
		return (iTime >= aTime.iTime);
		}
	else
		{
		return (UtcL() >= aTime.UtcL());
		}
	}



EXPORT_C TBool TAgnCalendarTime::IsValidTime() const
	{
	TBool isNullTime(iTime == Time::NullTTime());
	TBool isWithinValidRange(iTime >= Time::MinTTime() && iTime <= Time::MaxTTime());
	return (isNullTime || isWithinValidRange);
	}

/*
In v9.1 time was stored as a TUint64 value of number of microseconds since midnight
January 1st, 0 AD nominal Gregorian. 

The TAgnCalendarTime count time is a valid if it lay between Midnight, January 1st 1900 and 
Midnight, December 31st 2100. It means TUint64 value should be between 0x00D504A2C672E000 and 0x00eb8d745a6fc000. 
Also NullTime (0x8000000000000000) is supported.

Internalize:
1) read low 32 bits
2) read high 32 bits
3) if 2nd msb bit of high = 1 then: (note the 2nd msb bit is used because the 1st msb bit is used by Time::NullTTime().
        - must be reading v9.2+ format (iTime values that are either agn null time or between agn min time to agn max time can never have this msb bit set to 1)
        - flip 2nd msb bit to 0 to create iTime later in step 4
        - read another 32 bits and if lsb = 1 set to floating mode else must be fixed mode (other 31 bits will be used for TzId in future)
        - read yet another 32 bits (future padding)
4) set iTime = TTime(MAKE_TINT64(high, low))


Externalize:
1) write low 32 bits of iTime,
2) mask the high 32 bits of iTime with (1<<30) to signify a v9.2+ time format
3) write masked high bits of iTime
4) write another 32 bits (lsb = 0 for fixed time mode, 1 for floating time mode)
5) write yet another 32 bits (value = 0 always for future use) 
*/
EXPORT_C void TAgnCalendarTime::InternalizeL(RReadStream& aStream)
	{
	TUint32 low = aStream.ReadUint32L();
	TUint32 high = aStream.ReadUint32L();

	if (high & KAgnNewCalTimeMask)
		{
		// reading v9.2+ format
		high = high & ~KAgnNewCalTimeMask;

		TUint32 attr = aStream.ReadUint32L(); //read attribute
		if (attr & KAgmTimeFloatingFlag)
			{
			SetFloatingFlag(ETrue);
			}
		else	
			{
			SetFloatingFlag(EFalse);
			}

		aStream.ReadUint32L(); //reserved 32 bits for future use
		}
	else
		{
		// for backcompatibility, v9.1 only support fix time mode
		iTzId = KTzIdUnspecified;
		SetFloatingFlag(EFalse);
		}

	iTime = TTime(MAKE_TINT64(high, low));  //old data format
	__ASSERT_ALWAYS(IsValidTime(), User::Leave(KErrCorrupt));
	}

void TAgnCalendarTime::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << 	I64LOW(iTime.Int64());
	aStream << 	(I64HIGH(iTime.Int64()) | KAgnNewCalTimeMask);
	TUint32 attr(0);
	if (TimeMode() == MAgnCalendarTimeMode::EFloating)
		{
		attr |= KAgmTimeFloatingFlag;
		}
	
	aStream << attr;
	aStream.WriteUint32L(0); // reserved 32 bits for future use

	}

void TAgnCalendarTime::SetFloatingFlag(TBool aFloating) const
	{
	if (aFloating)
		{
		iTzId |= KTzIdFloating;
		}
	else
		{
		iTzId &= ~KTzIdFloating;
		}
	}

void TAgnCalendarTime::SetTzId(TUint16 aTzId) const
	{
	iTzId = aTzId | (iTzId & KTzIdFloating);
	}

TUint16 TAgnCalendarTime::TzId() const
	{
	return (iTzId & ~KTzIdFloating);
	}
	
const MAgnCalendarTimeMode* TAgnCalendarTime::CalendarTimeMode() const
	{
	if (iTzId & KTzIdFloating)
		{
		return &TimeZoneAccessor()->FloatingTimeMode();
		}
	else
		{
		// this is needed for alarm info reconsturction
		return &TimeZoneAccessor()->FixedTimeMode();
		}
	}
	
EXPORT_C MAgnCalendarTimeMode::TTimeMode TAgnCalendarTime::TimeMode() const
	{
	if (CalendarTimeMode())
		{
		return CalendarTimeMode()->TimeMode();
		}
	return MAgnCalendarTimeMode::EFixedUtc;    // default is fixed time mode if not set
	}

/*static*/ TInt TAgnCalendarTime::Compare(const TAgnCalendarTime& aLeft, const TAgnCalendarTime& aRight)
	{
	if (aLeft == aRight)
		{
		return 0;
		}
	else if (aLeft > aRight)
		{
		return 1;
		}
	return -1;
	}

/*static*/ void TAgnCalendarTime::InsertInOrderL(RArray<TAgnCalendarTime>& aTimeArray, const TAgnCalendarTime& aTimeToInsert)
	{
	TLinearOrder<TAgnCalendarTime> agnCalTimeOrder(TAgnCalendarTime::Compare);
	TInt err = aTimeArray.InsertInOrder(aTimeToInsert, agnCalTimeOrder);
	if (err != KErrAlreadyExists)
		{
		User::LeaveIfError(err);
		}
	}

/*static*/ TBool TAgnCalendarTime::CompareTimeArrays(const RArray<TAgnCalendarTime>* aLeft, const RArray<TAgnCalendarTime>* aRight)
	{
	if (aLeft == NULL)
		{
		if (aRight == NULL)
			{
			return ETrue;
			}
		return EFalse;
		}

	// aLeft is non-NULL
	if (aRight == NULL)
		{
		return EFalse;
		}
	
	if (aLeft->Count() != aRight->Count())
		{
		return EFalse;
		}
	
	const TInt KTimeCount = aLeft->Count();
	for (TInt i = 0; i < KTimeCount; ++i)
		{
		if ((*aLeft)[i] != (*aRight)[i])
			{
			return EFalse;
			}
		}
	return ETrue;
	}

/*static*/ void TAgnCalendarTime::InternalizeTimeArrayL(RArray<TAgnCalendarTime>& aArray, RReadStream& aStream)
	{
	aArray.Reset();
	const TInt KCount = aStream.ReadUint16L();
	
	TAgnCalendarTime time;
	for (TInt i = 0; i < KCount; ++i)
		{
		aStream >> time;
		aArray.AppendL(time);
		}
	}

/*static*/ void TAgnCalendarTime::ExternalizeTimeArrayL(RArray<TAgnCalendarTime>& aArray, RWriteStream& aStream)
	{
	const TInt KCount = aArray.Count();
	aStream.WriteUint16L(KCount);
	
	for (TInt i = 0; i < KCount; ++i)
		{
		aStream << aArray[i];
		}
	}

// TAgnCalendarFixedTimeMode //

TAgnCalendarFixedTimeMode::TAgnCalendarFixedTimeMode(CTzConverter& aTimeConverter) :
	iTimeConverter(aTimeConverter)
	{
	}

void TAgnCalendarFixedTimeMode::ToL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const
	{
	switch (aFormat)
		{
		case ELocal: 
		case EFixedLocal: 
			{
			User::LeaveIfError(iTimeConverter.ConvertToUniversalTime(aTime));
			}
		case EUtc:
			{
			// do nothing - no conversion necessary
			break;
			}
		default: 
			{
			Panic(EAgmErrUnsupportedTimeMode);
			}
		};
	}

void TAgnCalendarFixedTimeMode::FromL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const
	{
	switch (aFormat)
		{
		case ELocal: 
		case EFixedLocal: 
			{
			User::LeaveIfError(iTimeConverter.ConvertToLocalTime(aTime));
			break;
			}
		case EUtc:
			{
			// do nothing - no conversion necessary
			break;
			}
		default: 
			{
			Panic(EAgmErrUnsupportedTimeMode);
			}
		}
	}

MAgnCalendarTimeMode::TTimeMode TAgnCalendarFixedTimeMode::TimeMode() const
	{
	return MAgnCalendarTimeMode::EFixedUtc;
	}

// TAgnCalendarFixedUsingRulesTimeMode //

TAgnCalendarFixedUsingRulesTimeMode::TAgnCalendarFixedUsingRulesTimeMode(CTzRules& aTimeZoneRules, CTzConverter& aTimeConverter) :
	iTimeZoneRules(aTimeZoneRules), iTimeConverter(aTimeConverter)
	{
	}

const CTzRules& TAgnCalendarFixedUsingRulesTimeMode::TzZone() const
	{
	return iTimeZoneRules;
	}

void TAgnCalendarFixedUsingRulesTimeMode::ToL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const
	{
	switch (aFormat)
		{
		case ELocal:
			{
			User::LeaveIfError(iTimeConverter.ConvertToLocalTime(aTime));
			break;
			}
		case EUtc:
			{
			iTimeZoneRules.ConvertToLocalL(aTime);
			break;
			}
		case EFixedLocal: 
			{
			// do nothing - no conversion necessary
			break;
			}
		default: 
			{
			Panic(EAgmErrUnsupportedTimeMode);
			}
		};
	}

void TAgnCalendarFixedUsingRulesTimeMode::FromL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const
	{
	switch (aFormat)
		{
		case EUtc: 
			{
			iTimeZoneRules.ConvertToUtcL(aTime);
			break;
			}
		case ELocal:
			{
			User::LeaveIfError(iTimeConverter.ConvertToUniversalTime(aTime));
			break;
			}
		case EFixedLocal: 
			{
			// do nothing - no conversion necessary
			break;
			}
		default: 
			{
			Panic(EAgmErrUnsupportedTimeMode);
			}
		}
	}

MAgnCalendarTimeMode::TTimeMode TAgnCalendarFixedUsingRulesTimeMode::TimeMode() const
	{
	return MAgnCalendarTimeMode::EFixedTimeZone;
	}

// TAgnCalendarFloatingTimeMode //

TAgnCalendarFloatingTimeMode::TAgnCalendarFloatingTimeMode(CTzConverter& aTimeConverter) :
	iTimeConverter(aTimeConverter)
	{
	}

void TAgnCalendarFloatingTimeMode::ToL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const
	{
	switch (aFormat)
		{
		case ELocal: 
		case EFixedLocal: 
			{
			// do nothing - no conversion necessary
			break;
			}
		case EUtc:
			{
			User::LeaveIfError(iTimeConverter.ConvertToLocalTime(aTime));
			break;
			}
		default: 
			{
			Panic(EAgmErrUnsupportedTimeMode);
			}
		};
	}

void TAgnCalendarFloatingTimeMode::FromL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const
	{
	switch (aFormat)
		{
		case ELocal: 
		case EFixedLocal: 
			{
			// do nothing - no conversion necessary
			break;
			}
		case EUtc:
			{
			User::LeaveIfError(iTimeConverter.ConvertToUniversalTime(aTime));
			break;
			}
		default: 
			{
			Panic(EAgmErrUnsupportedTimeMode);
			}
		};
	}

MAgnCalendarTimeMode::TTimeMode TAgnCalendarFloatingTimeMode::TimeMode() const
	{
	return MAgnCalendarTimeMode::EFloating;
	}
