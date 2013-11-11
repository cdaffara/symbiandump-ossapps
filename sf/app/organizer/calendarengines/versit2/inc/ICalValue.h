#ifndef ICALVALUE_H
#define ICALVALUE_H/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Holds the definition of CICalValue.
*
*/



// System includes.
#include <e32base.h>	// CBase

// Forward declarations.
class CICalRuleSegment;

/**
Class representing a property-, property parameter- or rule segement value.
@publishedPartner
*/
class CICalValue : public CBase
	{
public:	// Enumerations.
	enum TTimeZoneType
		{
		/** Local "floating" time.*/
		EFloatingTime,
		/** UTC time.*/
		EUtcTime,
		/** User specified time zone.*/
		ESpecifiedTimeZone
		};
		
public:	// Construction/destruction.
	IMPORT_C static CICalValue* NewL();
	IMPORT_C static CICalValue* NewLC();
	~CICalValue();

public:	// Methods.	
	IMPORT_C HBufC8* BinaryLC() const;
	IMPORT_C void SetBinaryL(const TDesC8& aBuffer);
	IMPORT_C TBool BooleanL() const;
	IMPORT_C void SetBooleanL(TBool aBool);
	IMPORT_C void GetDateL(TTime& aDate) const;
	IMPORT_C void SetDateL(const TTime& aDate);
	IMPORT_C void GetTimeL(TTime& aTime, TTimeZoneType& aTzType) const;
	IMPORT_C void SetTimeL(const TTime& aTime, TTimeZoneType aTzType);
	IMPORT_C void GetDateTimeL(TTime& aDateTime, TTimeZoneType& aTzType, TInt aFirstCharacterNum = 0) const;
	IMPORT_C void SetDateTimeL(const TTime& aDateTime, TTimeZoneType aTzType);
	IMPORT_C TTimeIntervalSeconds DurationL() const;
	IMPORT_C void SetDurationL(TTimeIntervalSeconds aDuration);
	IMPORT_C void GetFloatL(TReal& aFloat) const;
	IMPORT_C void SetFloatL(const TReal& aFloat);
	IMPORT_C TInt IntegerL() const;
	IMPORT_C void SetIntegerL(TInt aInt);
	
	IMPORT_C void GetPeriodL(
		TTime& aStartTime, 
		TTimeZoneType& aStartTzType, 
		TTime& aEndTime, 
		TTimeZoneType& aEndTzType) const;
		
	IMPORT_C void SetPeriodL(
		const TTime& aStartTime, 
		TTimeZoneType aStartTzType, 
		const TTime& aEndTime,
		TTimeZoneType aEndTzType);
		
	IMPORT_C void GetDayL(TDay& aDay, TInt& aPos) const;
	IMPORT_C void SetDayL(TDay aDay, TInt aPos = 0);
	IMPORT_C TMonth MonthL() const;
	IMPORT_C void SetMonthL(TMonth aMonth);
	IMPORT_C void GetRecurrenceRuleL(RPointerArray<CICalRuleSegment>& aRuleSegmentArray) const;
	IMPORT_C void SetRecurrenceRuleL(const TDesC& aRuleValue);
	IMPORT_C const TDesC& TextL() const;
	IMPORT_C void SetTextL(const TDesC& aText);
	IMPORT_C TTimeIntervalSeconds UtcOffsetL() const;
	IMPORT_C void SetUtcOffsetL(TTimeIntervalSeconds aOffset);

private:	// Construction.
	CICalValue();
	void ConstructL();

private:	// Methods.
	void CheckNullValueL() const;
	void PrepareValuePointer();
	TInt ReadIntL(const TDesC& aDes) const;
	void AppendDateToValueL(const TTime& aDate);
	void GetDateFromValueL(TTime& aDate, TInt aFirstCharacterNum = 0) const;
	void AppendTimeToValueL(const TTime& aTime);
	void GetTimeFromValueL(TTime& aTime, TTimeZoneType& aTzType, TInt aFirstCharacterNum = 0) const;
	void GetTimeIntervalFromValueL(TTimeIntervalSeconds& aTime, TInt aFirstCharacterNum = 0) const;
	TBool GetDurDateL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const;
	TBool GetDurTimeL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const;
	TBool GetDurHourL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const;
	TBool GetDurMinuteL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const;
	TBool GetDurSecondL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const;
	TBool GetDurWeekL(TInt& aIntervalSecs, TInt& aCurrentCharNumber) const;
	
private:	// Attributes.
	HBufC* iValue;
	};

#endif // ICALVALUE_H

// End of File
