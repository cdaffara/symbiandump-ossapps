#ifndef ICALRULESEGMENT_H
#define ICALRULESEGMENT_H/*
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
* Description:   Holds the definition of CICalRuleSegment.
*
*/



// System includes
#include <e32base.h>	// CBase

// Forward class declarations
class CICalValue;

/**
Class representing a recurrence rule value segment.
@publishedPartner
*/
class CICalRuleSegment : public CBase
	{
public:	// Enumerations.
	enum TSegmentType
		{
		/** A frequency (FREQ) segment.*/
		ESegFreq,
		/** An UNTIL segment.*/
		ESegUntil,
		/** A COUNT segment.*/
		ESegCount,
		/** An INTERVAL segment.*/
		ESegInterval,
		/** A BYSECOND segment.*/
		ESegBySecond,
		/** A BYMINUTE segment.*/
		ESegByMinute,
		/** A BYHOUR segment.*/
		ESegByHour,
		/** A BYDAY segment.*/
		ESegByDay,
		/** A BYMONTHDAY segment.*/
		ESegByMonthDay,
		/** A BYYEARDAY segment.*/
		ESegByYearDay,
		/** A BYWEEKNO segment.*/
		ESegByWeekNo,
		/** A BYMONTH segment.*/
		ESegByMonth,
		/** A BYSETPOS segment.*/
		ESegByPos,
		/** A WKST segment.*/
		ESegWkSt,
		/** An X-name extension segment.*/
		ESegExtension
		};

	enum TFreq
		{
		/** Frequency in seconds.*/
		EFreqSecondly,
		/** Frequency in minutes.*/
		EFreqMinutely,
		/** Frequency in hours.*/
		EFreqHourly,
		/** Frequency in days.*/
		EFreqDaily,
		/** Frequency in weeks.*/
		EFreqWeekly,
		/** Frequency in months.*/
		EFreqMonthly,
		/** Frequency in years.*/
		EFreqYearly
		};

public:	// Construction/destruction.
	static CICalRuleSegment* NewL(const TDesC& aSource);
	static CICalRuleSegment* NewLC(const TDesC& aSource);
	~CICalRuleSegment();

public:	// Methods.
	IMPORT_C const TDesC& TypeName() const;
	IMPORT_C const RPointerArray<CICalValue>& Values() const;
	IMPORT_C TSegmentType Type() const;
	IMPORT_C TFreq FreqL() const;
	
private:	// Construction.
	CICalRuleSegment();
	void ConstructL(const TDesC& aSource);
	void SetTypeL(const TDesC& aType);
	
private:	// Attributes.
	TSegmentType iType;
	HBufC* iTypeString;
	RPointerArray<CICalValue> iValues;
	};

#endif	// ICALRULESEGMENT_H

// End of File
