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
 
#ifndef __VRECUR_H__
#define __VRECUR_H__

#include <versit.h>

//
// CWeekDayArray
//

class CWeekDayArray : public CBase
/** Defines an array of the days in the week on which a 'weekly' 
or 'monthly by position' repeat event occurs. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CWeekDayArray();
	IMPORT_C ~CWeekDayArray();
	IMPORT_C void ExternalizeL(RWriteStream& aStream);
public:	
	/** The array of days in the week. */
	CArrayFix<TDay>* iArray;
	};

//
// CVersitRecurrence
//
class CVersitRecurrence : public CBase
/** Abstract base class for all recurrence property value classes.

A pointer to a derived recurrence property value class instance is owned by 
the CParserPropertyValueRecurrence class.

Implementations of this class define when an event is to repeat. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CVersitRecurrence(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate);
	IMPORT_C ~CVersitRecurrence();
	/** Externalises an occurrence list to aStream.
	
	@param aStream The stream to which the occurrence list is to be externalised. */
	virtual void ExternalizeOccurrenceListsL(RWriteStream& aStream) const=0;
	/** Repeat type. 
	@publishedAll
	@released */
	enum TType
		{
		/** Daily repeat. */
		EDaily=1,
		/** Weekly repeat. */
		EWeekly,
		/** Monthly repeat, by relative position within the month. */
		EMonthlyByPos,
		/** Monthly repeat, by day number within the month. */
		EMonthlyByDay,
		/** Yearly repeat, by specific months within the year. */
		EYearlyByMonth,
		/** Yearly repeat, by specific days within the year. */
		EYearlyByDay
		};
public:
	/** The type of repeat (daily, weekly etc.). */
	TType iRepeatType;
	/** The interval between repeats: a number of days, weeks, months or years, depending 
	on the repeat type. */
	TInt iInterval;
	/** The duration in days, weeks, months or years (depending on the repeat type) 
	for the repeat.
	
	A value of zero indicates the repeat should continue forever. */
	TInt iDuration;
	/** Specification for the date at which the repeat will end. If a duration and 
	an end date are both specified, the end date takes precedence. */
	TVersitDateTime* iEndDate;
	};

//
// CVersitRecurrenceDaily
//
class CVersitRecurrenceDaily : public CVersitRecurrence
/** Defines when a 'daily' recurrence is to be repeated.

Used by a repeating event (a vCalendar event or to-do) to define when it is 
to occur. The days on which the event occurs are identified by the number 
of days between repeats, e.g. every third day.

A pointer to this object may be owned by a CParserPropertyValueRecurrence 
object. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CVersitRecurrenceDaily(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate);
public: //from CVersitRecurrence
	IMPORT_C void ExternalizeOccurrenceListsL(RWriteStream& /*aStream*/) const;
	};

//
//	CVersitRecurrenceWeekly
//
class CVersitRecurrenceWeekly : public CVersitRecurrence
/** Defines a list of days when a 'weekly' recurrence is to be 
repeated.

Used by a repeating event (a vCalendar event or to-do) to define when it is 
to occur. The days on which the event occurs are identified by the number 
of weeks between repeats and the day(s) of the week on which the event occurs, 
e.g. on Monday every other week.

A pointer to this object may be owned by a CParserPropertyValueRecurrence 
object. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CVersitRecurrenceWeekly(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,CWeekDayArray* aArrayOfWeekDayOccurrences);
	IMPORT_C ~CVersitRecurrenceWeekly();
public: //from CVersitRecurrence
	IMPORT_C void ExternalizeOccurrenceListsL(RWriteStream& aStream) const;
public:
	CWeekDayArray* iArrayOfWeekDayOccurrences;		//Mon-Sun
	};

//
// CVersitRecurrenceMonthlyByPos
//
class CVersitRecurrenceMonthlyByPos : public CVersitRecurrence
/** Defines a list of days when a 'monthly by position' recurrence 
is to be repeated.

Used by a repeating event (a vCalendar event or to-do) to define when it is 
to occur.

The days on which the event occurs are identified by their relative position 
within the month, for example the second Monday or the last Friday.

A pointer to this object may be owned by a CParserPropertyValueRecurrence 
object

Note: The CMonthPosition class, defined within this class, is used to 
define the positions of days within the month. 
@publishedAll
@released
*/
	{
public:
	class CMonthPosition : public CBase
	/** Defines a week within the month, using the numeric occurrence of the week 
	(between 1 and 5 inclusive) counting from either the start or end of the month, 
	and defines an array of days within this week. 
	@publishedAll
	@released
	*/
		{
	public:
		IMPORT_C ~CMonthPosition();
	public:
		/** Flags that define whether the week number is counted from the start or end of the month. 
		@publishedAll
		@released */
		enum TSign 
				{
				/** Indicates that the iWeekNo member specifies a number counting forwards 
				from the start of the month. */
				EWeeksFromStartOfMonth,
				/** Indicates that the iWeekNo member specifies a number counting backwards from 
				the end of the month. */
				EWeeksFromEndOfMonth
				};
	public:
		/** Indicates whether the week number iWeekNo is counted from the start or the 
		end of the month. A plus sign denotes from the start of the month and a minus 
		sign denotes from the end. */
		TSign iSign;
		/** A week number within the month, between 1 and 5 inclusive. */
		TInt iWeekNo;
		/** Pointer to an array of week days. */
		CWeekDayArray* iArrayOfWeekDays;
		};
	IMPORT_C CVersitRecurrenceMonthlyByPos(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,CArrayPtrFlat<CMonthPosition>* aMonthPositions);
	IMPORT_C ~CVersitRecurrenceMonthlyByPos();
public: //framework
	IMPORT_C void ExternalizeOccurrenceListsL(RWriteStream& aStream) const;
public:
	/** Array of 'month positions' which define the days on which the event occurs. */
	CArrayPtrFlat<CMonthPosition>* iMonthPositions;
	};

//
// CVersitRecurrenceMonthlyByDay
//
class CVersitRecurrenceMonthlyByDay : public CVersitRecurrence
/** Defines a list of days when a 'monthly by day' recurrence 
is to repeat.

Used by a repeating event (a vCalendar event or to-do) to define when it is 
to occur.

The days on which the event occurs are identified by a number, counting 
either from the start or the end of the month.

A pointer to this object may be owned by a CParserPropertyValueRecurrence 
object. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CVersitRecurrenceMonthlyByDay(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,	CArrayFix<TInt>* aArrayOfOccurrencesInDaysFromStartOfMonth
		,CArrayFix<TInt>* aArrayOfOccurrencesInDaysFromEndOfMonth,TBool aLastDay);
	IMPORT_C ~CVersitRecurrenceMonthlyByDay();
public: //framework
	IMPORT_C void ExternalizeOccurrenceListsL(RWriteStream& aStream) const;
public:
	/** Array of days, counting from the start of the month, on which the event occurs. */
	CArrayFix<TInt>* iArrayOfOccurrencesInDaysFromStartOfMonth; //1-31
	/** Array of days, counting from the end of the month, on which the event occurs. */
	CArrayFix<TInt>* iArrayOfOccurrencesInDaysFromEndOfMonth; //1-31
	/** Identifies whether the event occurs on the last day of the month. */
	TBool iLastDay;
	};

//
// CVersitRecurrenceYearlyByMonth
//
class CVersitRecurrenceYearlyByMonth : public CVersitRecurrence
/** Defines a list of months when a 'yearly by month' recurrence 
is to repeat.

Used by a repeating event (a vCalendar event or to-do) to define the months  
in which it is to occur. The months on which the event occurs are identified 
by their number in the year (between 1 and 12 inclusive).

A pointer to this object may be owned by a CParserPropertyValueRecurrence 
object. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CVersitRecurrenceYearlyByMonth(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,CArrayFix<TMonth>* aArrayOfMonthsInYearOccurrences);
	IMPORT_C ~CVersitRecurrenceYearlyByMonth();
public: //framework
	IMPORT_C void ExternalizeOccurrenceListsL(RWriteStream& aStream) const;
public:
	/** Pointer to an array of month values. */
	CArrayFix<TMonth>* iArrayOfMonthsInYearOccurrences; //Jan-Dec
	};

//
// CVersitRecurrenceYearlyByDay
//
class CVersitRecurrenceYearlyByDay : public CVersitRecurrence
/** Defines a list of days when a 'yearly by day' recurrence is 
to be repeated.

Used by a repeating event (a vCalendar event or to-do) to define when it is 
to occur.

The days on which the repeat occurs are identified by their day number in 
the year (between 1 and 366 inclusive).

A pointer to this object may be owned by a CParserPropertyValueRecurrence 
object. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CVersitRecurrenceYearlyByDay(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,CArrayFix<TInt>* aArrayOfDaysInYearOccurrences);
	IMPORT_C ~CVersitRecurrenceYearlyByDay();
public: //framework
	IMPORT_C void ExternalizeOccurrenceListsL(RWriteStream& aStream) const;
public:
	/** Pointer to an array of integers between 1 and 366 inclusive. 
	
	Each integer represents a day on which the repeat event occurs. */
	CArrayFix<TInt>* iArrayOfDaysInYearOccurrences; 
	};

//
// CParserPropertyValueRecurrence
//
class CParserPropertyValueRecurrence : public CParserTimePropertyValue
/** A recurrence property value parser.

This is used to store and retrieve the recurrence information for a repeating 
vEvent or vTodo. This information is stored as a CVersitRecurrence object.

The UID for a recurrence property value is KVCalPropertyRecurrenceUid. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CParserPropertyValueRecurrence(CVersitRecurrence* aValue);
	IMPORT_C ~CParserPropertyValueRecurrence();
	inline CVersitRecurrence* Value() const;
public: // from CParserTimePropertyValue
	IMPORT_C void ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& aIncrement,const CVersitDaylight* aDaylight);
	IMPORT_C void ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& aIncrement);
public: // from CParserPropertyValue
	IMPORT_C void ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/,TInt /*aLengthOutput*/);
protected:
	CVersitRecurrence* iValue;
	};

//
// CRecurrenceParser
//
class CRecurrenceParser : public CVersitParser
/** Base class used in the derivation of CParserVCalEntity.

Provides recurrence functionality for vEvents and vToDos in vCalendars. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C CRecurrenceParser(TBool aHasVersion);
	IMPORT_C CParserPropertyValue* MakePropertyValueRecurrenceL(TDes& aRecurringEntity);
private:	
	static void ResetAndDestroyArrayOfMonthPositions(TAny* aObject);	
	void GetFrequencyAndIntervalL(CVersitRecurrence::TType& aFrequency,TInt& aInterval, const TDesC& aRecurrenceType);
	CVersitRecurrence* GetFrequencyModifiersL(const CVersitRecurrence::TType& aRepeatType,TInt aInterval, const TDesC& aListDates);
	CWeekDayArray* GetListOfWeekDayOccurrencesL(const TDesC& aListDays);
private: // from CVersitParser
	IMPORT_C virtual void Reserved1();
	IMPORT_C virtual void Reserved2();
	};

#include <vrecur.inl>

#endif
