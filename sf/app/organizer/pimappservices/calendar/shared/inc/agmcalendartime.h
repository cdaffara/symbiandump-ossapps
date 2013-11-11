// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll
 @released
*/


#ifndef __AGMCALENDARTIME_H__
#define __AGMCALENDARTIME_H__

#include <e32base.h>

class CTzRules;
class CTzConverter;
class CAgnTlsProxy;
class RReadStream;
class RWriteStream;

/** Interface class representing a time mode.
A time mode can be floating, fixed to UTC, or fixed to a particular time zone (but that is not supported yet).
This class provides functions to convert between UTC and the relevant local time.
*/
NONSHARABLE_CLASS(MAgnCalendarTimeMode)
	{
public:
	enum TFormat {ELocal, EUtc, EFixedLocal}; // EFixedLocal indicate the local time that is associated with the rule of TAgnCalendarFixedUsingRulesTimeMode

	/** The time mode for a calendar entry	*/
	enum TTimeMode
		{
		/** Floating time. */
		EFloating,
		/** Fixed time in UTC format. When a fixed time entry is repeating, its repeat rule use EFixedTimeZone mode to store the corresponding time zone where the start and end time are stored in EFixedUtc. */
		EFixedUtc,
		/** Timezone rule time mode. Currently, this is used only for repeating entry where the start and end time are stored in EFixedUtc more */ 
		EFixedTimeZone,
		};
public:
	virtual void ToL(TFormat aFormat, TTime& aTime) const = 0;
	virtual void FromL(TFormat aFormat, TTime& aTime) const = 0;
	virtual TTimeMode TimeMode() const = 0;
	};


/**
The main time class for use within Calendar.
This stores either a fixed or floating time - a fixed time is stored as UTC, a floating time is stored as local. 
The offset between UTC and local is cached and updated when necessary.

This class should be used as widely as possible throughout Calendar, eventually replacing the less useful TTime.
TCalTime is an equivalent class to TAgnCalendarTime, but is exposed in the Calendar API.
*/
NONSHARABLE_CLASS(TAgnCalendarTime)
	{
public:
	IMPORT_C TAgnCalendarTime();
	
	IMPORT_C void SetFloatingL(const TTime& aTime); // sets floating time
	IMPORT_C void SetLocalL(const TTime& aTime);	// sets fixed time
	IMPORT_C void SetUtcL(const TTime& aTime);		// sets fixed time

	IMPORT_C TTime LocalL() const;
	IMPORT_C TTime UtcL() const;

	IMPORT_C TBool IsSet() const; // returns ETrue if non-NullTTime
	IMPORT_C TBool operator==(const TAgnCalendarTime& aTime) const;
	IMPORT_C TBool operator!=(const TAgnCalendarTime& aTime) const;
	IMPORT_C TBool operator<(const TAgnCalendarTime& aTime) const;
	IMPORT_C TBool operator>(const TAgnCalendarTime& aTime) const;
	IMPORT_C TBool operator<=(const TAgnCalendarTime& aTime) const;
	IMPORT_C TBool operator>=(const TAgnCalendarTime& aTime) const;

	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C MAgnCalendarTimeMode::TTimeMode TimeMode() const; // return the time mode

	void SetNull();
	IMPORT_C TBool IsValidTime() const;
	void ExternalizeL(RWriteStream& aStream) const;
	const MAgnCalendarTimeMode* CalendarTimeMode() const;

	// functions used in RArrays of TAgnCalendarTime
	static TInt Compare(const TAgnCalendarTime& aLeft, const TAgnCalendarTime& aRight);
	static void InsertInOrderL(RArray<TAgnCalendarTime>& aTimeArray, const TAgnCalendarTime& aTimeToInsert);
	static TBool CompareTimeArrays(const RArray<TAgnCalendarTime>* aLeft, const RArray<TAgnCalendarTime>* aRight);
	static void InternalizeTimeArrayL(RArray<TAgnCalendarTime>& aArray, RReadStream& aStream);
	static void ExternalizeTimeArrayL(RArray<TAgnCalendarTime>& aArray, RWriteStream& aStream);
	
private:
	void StoreNewOffset(const TTime& aTime) const;
	CAgnTlsProxy* TimeZoneAccessor() const;

	void SetTzId(TUint16 aTzId) const;		// Always use this to set the iTzId time zone variable
	TUint16 TzId() const;					// Always use this to get the iTzId time zone variable
	void SetFloatingFlag(TBool aFloating) const;
	
	void SetDateTimeL(const TTime& aTime, TBool aFloating, MAgnCalendarTimeMode::TFormat aFormat = MAgnCalendarTimeMode::EUtc);
	TTime DateTimeL(MAgnCalendarTimeMode::TFormat aFormat) const;
private:
	TTime iTime; // time stored as UTC or local depending on iCalendarMode
	mutable TInt16 iLocalOffsetInMinutes;
	mutable TUint16 iTzId; // caches current time zone ID AND floating status
	mutable CAgnTlsProxy* iTimeZoneAccessor; // not owned
	};

/** Concrete time mode class for fixed UTC time mode.
*/
NONSHARABLE_CLASS(TAgnCalendarFixedTimeMode) : public MAgnCalendarTimeMode
	{
public:
	TAgnCalendarFixedTimeMode(CTzConverter& aTimeConverter);
	void ToL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const;
	void FromL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const;
	TTimeMode TimeMode() const;
private:
	TAgnCalendarFixedTimeMode(const TAgnCalendarFixedTimeMode&); // not implemented
	TAgnCalendarFixedTimeMode& operator=(TAgnCalendarFixedTimeMode&); // not implemented
private:
	CTzConverter& iTimeConverter; // not owned
	};

/** Concrete time mode class for 'fixed to time zone' time mode.
*/
NONSHARABLE_CLASS(TAgnCalendarFixedUsingRulesTimeMode) : public MAgnCalendarTimeMode
	{
public:
	TAgnCalendarFixedUsingRulesTimeMode(CTzRules& aTimeZoneRules, CTzConverter& aTimeConverter);
	void ToL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const;
	void FromL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const;
	const CTzRules& TzZone() const;
	TTimeMode TimeMode() const;
private:
	TAgnCalendarFixedUsingRulesTimeMode(const TAgnCalendarFixedUsingRulesTimeMode&); // not implemented
	TAgnCalendarFixedUsingRulesTimeMode& operator=(TAgnCalendarFixedUsingRulesTimeMode&); // not implemented
private:
	const CTzRules& iTimeZoneRules; // not owned
	CTzConverter& iTimeConverter; // not owned
	};

/** Concrete time mode class for floating time mode.
*/
NONSHARABLE_CLASS(TAgnCalendarFloatingTimeMode) : public MAgnCalendarTimeMode
	{
public:
	TAgnCalendarFloatingTimeMode(CTzConverter& aTimeConverter);
	void ToL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const;
	void FromL(MAgnCalendarTimeMode::TFormat aFormat, TTime& aTime) const;
	TTimeMode TimeMode() const;
private:
	TAgnCalendarFloatingTimeMode(const TAgnCalendarFloatingTimeMode&); // not implemented
	TAgnCalendarFloatingTimeMode& operator=(TAgnCalendarFloatingTimeMode&); // not implemented
private:
	CTzConverter& iTimeConverter; // not owned
	};

#endif
