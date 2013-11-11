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
 
#include <vrecur.h>
#include "verror.h"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include "versit_internal.h"
#endif
//
// CWeekDayArray
//

EXPORT_C CWeekDayArray::CWeekDayArray()
: iArray(NULL)
/** Constructs an array of the days in the week. */
	{}

EXPORT_C CWeekDayArray::~CWeekDayArray()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iArray;
	}

EXPORT_C void CWeekDayArray::ExternalizeL(RWriteStream& aStream)
/** Externalises the array of days in the week to a write stream.

Writes the first two characters of each day name, for instance "MO TU WE".

The presence of this function means that the standard templated operator<<() 
(defined in s32strm.h) is available to externalise objects of this class.

@param aStream Stream to which the array is to be externalised. */
	{
	if (iArray)
		{
		TInt count=iArray->Count();
		for (TInt ii=0; ii<count; ii++)
			{
			switch ((*iArray)[ii])
				{
			case EMonday:
				aStream.WriteL(KVersitRecurrenceMonday8);
				break;
			case ETuesday:
				aStream.WriteL(KVersitRecurrenceTuesday8);
				break;
			case EWednesday:
				aStream.WriteL(KVersitRecurrenceWednesday8);
				break;
			case EThursday:
				aStream.WriteL(KVersitRecurrenceThursday8);
				break;
			case EFriday:
				aStream.WriteL(KVersitRecurrenceFriday8);
				break;
			case ESaturday:
				aStream.WriteL(KVersitRecurrenceSaturday8);
				break;
			case ESunday:
				aStream.WriteL(KVersitRecurrenceSunday8);
				break;
			default:
				Panic(ENoWeekdayFound);
				break;
				};
			aStream.WriteL(KVersitTokenSpace);
			}
		}
	}

//
// CVersitRecurrence
//
EXPORT_C CVersitRecurrence::CVersitRecurrence(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate)
: iInterval(aInterval),iDuration(aDuration),iEndDate(aEndDate)
/** Constructs the CVersitRecurrence object.

Initialises the interval (iInterval), duration (iDuration) and optionally, the end 
date (iEndDate).

This is invoked by the constructor of a derived class.

@param aInterval The interval between repeats.
@param aDuration The duration for the recurrence.
@param aEndDate A pointer to the end date. The object takes ownership of this 
pointer. */
	{}

EXPORT_C CVersitRecurrence::~CVersitRecurrence()
/** Frees all resources owned by the CVersitRecurrence object, prior to its destruction. */
	{
	delete iEndDate;
	}

//
// CVersitRecurrenceDaily
//
EXPORT_C CVersitRecurrenceDaily::CVersitRecurrenceDaily(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate)
: CVersitRecurrence(aInterval,aDuration,aEndDate)
/** Constructs the CVersitRecurrenceDaily object.

Sets the interval, the duration and, optionally, an end date for the repeat. 
The repeat type is set to EDaily.

If a duration and an end date are both specified, the end date takes precedence.

@param aInterval The number of days between repeats. 
@param aDuration The duration, in days, for which the event should continue 
recurring. A value of zero indicates the repeat should continue forever.
@param aEndDate Pointer to the end date for the repeat. The object takes ownership 
of this pointer. */
	{
	iRepeatType=EDaily;
	}

EXPORT_C void CVersitRecurrenceDaily::ExternalizeOccurrenceListsL(RWriteStream& /*aStream*/) const
/** This function is empty because an occurrence list is not applicable to a daily recurrence type. 

@param aStream Not used.*/
	{}

//
//	CVersitRecurrenceWeekly
//
EXPORT_C CVersitRecurrenceWeekly::CVersitRecurrenceWeekly(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,CWeekDayArray* aArrayOfWeekdayOccurrences)
: CVersitRecurrence(aInterval,aDuration,aEndDate)
, iArrayOfWeekDayOccurrences(aArrayOfWeekdayOccurrences)
/** Constructs the CVersitRecurrenceWeekly object.

Initialises the interval, the duration, optionally, an end date for the repeat and, 
optionally, a pointer to an array of the days of the week on which the repeat 
event occurs. Sets the repeat type to EWeekly. 

@param aInterval The number of weeks between repeats.
@param aDuration The duration, in weeks, for which the event should recur.
A value of zero indicates the repeat should continue forever.
@param aEndDate Pointer to the end date for the repeat. The object takes 
ownership of this pointer.
@param aArrayOfWeekdayOccurrences Array of the days of the week on which 
the repeat event occurs. The object takes ownership of this pointer. */
	{
	iRepeatType=EWeekly;
	}

EXPORT_C CVersitRecurrenceWeekly::~CVersitRecurrenceWeekly()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iArrayOfWeekDayOccurrences;
	}

EXPORT_C void CVersitRecurrenceWeekly::ExternalizeOccurrenceListsL(RWriteStream& aStream) const
/** Writes the codes for the days of the week on which the event occurs (e.g. "TU 
TH") to the outgoing stream, aStream.

@param aStream The stream to which the occurrence list is to be written. */
	{
	if (iArrayOfWeekDayOccurrences)
		iArrayOfWeekDayOccurrences->ExternalizeL(aStream);
	}

//
// CVersitRecurrenceMonthlyByPos
//

EXPORT_C CVersitRecurrenceMonthlyByPos::CVersitRecurrenceMonthlyByPos(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,CArrayPtrFlat<CMonthPosition>* aMonthPositions)
: CVersitRecurrence(aInterval,aDuration,aEndDate)
,iMonthPositions(aMonthPositions)
/** Constructs the CVersitRecurrenceMonthlyByPos object.

Sets the interval, the duration, optionally, an end date for the repeat and 
a pointer to an array of 'month positions', which specify when the event occurs. 
Also sets the repeat type to EMonthlyByPos.

If a duration and an end date are both specified, the end date takes precedence.

@param aInterval The number of months between repeats. 
@param aDuration The duration, in months, for which the event should recur. 
A value of zero indicates the repeat should continue forever.
@param aEndDate Pointer to the end date for the repeat event. The object takes 
ownership of this pointer. 
@param aMonthPositions Pointer to an array of the positions of days within 
the month. The object takes ownership of this pointer. */
	{
	iRepeatType=EMonthlyByPos;
	}

EXPORT_C CVersitRecurrenceMonthlyByPos::~CVersitRecurrenceMonthlyByPos()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	if (iMonthPositions)
		{
		iMonthPositions->ResetAndDestroy();
		delete iMonthPositions;
		}
	}

EXPORT_C void CVersitRecurrenceMonthlyByPos::ExternalizeOccurrenceListsL(RWriteStream& aStream) const
/** Writes the days of the month on which the event occurs to the output stream, 
aStream.

If an event occurs on Monday and Tuesday of the first week and the second to last week 
of a month, the string written to aStream would be "1+ MO TU 2- MO TU ", with 
the plus sign indicating that the week is counted from the start of the month 
and the minus sign indicating that the week is counted from the end of the 
month.

@param aStream The stream to which the occurrence list is to be written. */
	{
	if (iMonthPositions)
		{
		const TInt count = iMonthPositions->Count();
		CMonthPosition* monthPos;
		TBuf8<1> weekNo;
		for (TInt ii = 0; ii < count; ii++)
			{
			monthPos = (*iMonthPositions)[ii];
			__ASSERT_DEBUG((monthPos->iWeekNo < 10), Panic(EWeekNoOverTen));
			weekNo.Num(monthPos->iWeekNo);
			aStream.WriteL(weekNo);
			if (monthPos->iSign == CMonthPosition::EWeeksFromEndOfMonth)
				aStream.WriteL(KVersitTokenMinus);
			else
				aStream.WriteL(KVersitTokenPlus);
			aStream.WriteL(KVersitTokenSpace);
			if (monthPos->iArrayOfWeekDays)
				monthPos->iArrayOfWeekDays->ExternalizeL(aStream);
			}
		}
	}

EXPORT_C CVersitRecurrenceMonthlyByPos::CMonthPosition::~CMonthPosition()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iArrayOfWeekDays;
	}

//
// CVersitRecurrenceMonthlyByDay
//
EXPORT_C CVersitRecurrenceMonthlyByDay::CVersitRecurrenceMonthlyByDay(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,	CArrayFix<TInt>* aArrayOfOccurrencesInDaysFromStartOfMonth
		,CArrayFix<TInt>* aArrayOfOccurrencesInDaysFromEndOfMonth,TBool aLastDay)
: CVersitRecurrence(aInterval,aDuration,aEndDate)
,iArrayOfOccurrencesInDaysFromStartOfMonth(aArrayOfOccurrencesInDaysFromStartOfMonth)
,iArrayOfOccurrencesInDaysFromEndOfMonth(aArrayOfOccurrencesInDaysFromEndOfMonth)
,iLastDay(aLastDay)
/** Constructs the CVersitRecurrenceMonthlyByDay object.

Sets the repeat type to CVersitRecurrence::EMonthlyByDay. Note: if a duration and an end 
date are both specified, the end date takes precedence.

@param aInterval The number of months between repeats.
@param aDuration The duration, in months, for which the event should recur. 
A value of zero indicates the repeat should continue forever.
@param aEndDate Optional pointer to the end date for the repeat event. The object 
takes ownership of this pointer.
@param aArrayOfOccurencesInDaysFromStartOfMonth Optional pointer to an array of 
numbers, each of which can have a value between 1 and 31 inclusive, which identify 
the days, counting from the start of the month, on which the repeat event occurs. The 
object takes ownership of this pointer.
@param aArrayOfOccurrencesInDaysFromEndOfMonth Optional pointer to an array of numbers, 
each of which can have a value between 1 and 31 inclusive, which identify the days, 
counting from the end of the month, on which the repeat event occurs. (The last day of 
the month is represented by 1). The object takes ownership of this pointer.
@param aLastDay Optional - ETrue if the event occurs on the last day of the month. This 
day can then be represented in the output stream using the LD recurrence value. EFalse if not. */
	{
	iRepeatType=EMonthlyByDay;
	}

EXPORT_C CVersitRecurrenceMonthlyByDay::~CVersitRecurrenceMonthlyByDay()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iArrayOfOccurrencesInDaysFromStartOfMonth;
	delete iArrayOfOccurrencesInDaysFromEndOfMonth;
	}

EXPORT_C void CVersitRecurrenceMonthlyByDay::ExternalizeOccurrenceListsL(RWriteStream& aStream) const
/** Writes the days of the month on which the event occurs to the output stream, 
aStream.

For days counted from the beginning of the month, the string written to aStream 
might be "1+ 2+ 16+ 17+ ", with the plus sign indicating that the day is counted 
from the start of the month.

For days counted from the end of the month, the string written to aStream 
might be "1- 3- 4- LD ", with "LD" signifying that the event occurs on the 
last day of the month. 

@param aStream The stream to which the occurrence list is to be written. */
	{
	if (iArrayOfOccurrencesInDaysFromStartOfMonth)
		{
		TInt count = iArrayOfOccurrencesInDaysFromStartOfMonth->Count();
		TInt occurrence;
		TBuf8<KVersitDefaultBufferSize> temp;
		for (TInt ii = 0; ii < count; ii++)
			{
			occurrence = (*iArrayOfOccurrencesInDaysFromStartOfMonth)[ii];
			if ((occurrence < 32) && (occurrence > 0)) //day in month
				{
				temp.Num(occurrence);
				aStream.WriteL(temp);
				aStream.WriteL(KVersitTokenPlus);
				aStream.WriteL(KVersitTokenSpace);
				}
			}
		}
	if (iArrayOfOccurrencesInDaysFromEndOfMonth)
		{
		TInt count = iArrayOfOccurrencesInDaysFromEndOfMonth->Count();
		TInt occurrence;
		TBuf8<KVersitDefaultBufferSize> temp;
		for (TInt ii = 0; ii < count; ii++)
			{
			occurrence = (*iArrayOfOccurrencesInDaysFromEndOfMonth)[ii];
			if ((occurrence < 32) && (occurrence > 0)) //day in month
				{
				temp.Num(occurrence);
				aStream.WriteL(temp);
				aStream.WriteL(KVersitTokenMinus);
				aStream.WriteL(KVersitTokenSpace);
				}
			}
		}
	if (iLastDay)
		{
		aStream.WriteL(KVersitRecurrenceLastDay);
		aStream.WriteL(KVersitTokenSpace);
		}
	}

//
// CVersitRecurrenceYearlyByMonth
//

EXPORT_C CVersitRecurrenceYearlyByMonth::CVersitRecurrenceYearlyByMonth(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,CArrayFix<TMonth>* aArrayOfMonthsInYearOccurrences)
: CVersitRecurrence(aInterval,aDuration,aEndDate)
, iArrayOfMonthsInYearOccurrences(aArrayOfMonthsInYearOccurrences)
/** Constructs the CVersitRecurrenceYearlyByMonth object.

Sets the repeat type to CVersitRecurrence::EYearlyByMonth.

If a duration and an end date are both specified, the end date takes precedence.

@param aInterval The number of years between repeats. 
@param aDuration The duration, in years, for which the event should recur. 
A value of zero indicates the repeat should continue forever. 
@param aEndDate Optional pointer to the end date for the repeat event. The object takes 
ownership of this pointer. 
@param aArrayOfMonthsInYearOccurrences Pointer to an array of month values. 
The object takes ownership of this pointer. */
	{
	iRepeatType=EYearlyByMonth;	
	}

EXPORT_C CVersitRecurrenceYearlyByMonth::~CVersitRecurrenceYearlyByMonth()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iArrayOfMonthsInYearOccurrences;
	}

EXPORT_C void CVersitRecurrenceYearlyByMonth::ExternalizeOccurrenceListsL(RWriteStream& aStream) const
/** Writes the months of the year, as month numbers, in which the event occurs to 
the output stream.

For example, if the event occurs from September to December inclusive, the 
string written to aStream would be "9 10 11 12 ".

@param aStream The stream to which the occurrence list is to be written. */
	{
	if (iArrayOfMonthsInYearOccurrences)
		{
		TInt count = iArrayOfMonthsInYearOccurrences->Count();
		TBuf8<KVersitDefaultBufferSize> temp;
		for (TInt ii = 0; ii < count; ii++)
			{
			temp.Num((*iArrayOfMonthsInYearOccurrences)[ii] + 1); // Add 1 since EJanuary = 0, not 1
			aStream.WriteL(temp);
			aStream.WriteL(KVersitTokenSpace);
			}
		}
	}

//
// CVersitRecurrenceYearlyByDay
//
EXPORT_C CVersitRecurrenceYearlyByDay::CVersitRecurrenceYearlyByDay(TInt aInterval,TInt aDuration,TVersitDateTime* aEndDate,CArrayFix<TInt>* aArrayOfDaysInYearOccurrences)
: CVersitRecurrence(aInterval,aDuration,aEndDate)
,iArrayOfDaysInYearOccurrences(aArrayOfDaysInYearOccurrences)
/** Constructs the CVersitRecurrenceYearlyByDay object.

Sets the repeat type to EYearlyByDay.

If a duration and an end date are both specified, the end date takes precedence.

@param aInterval The number of years between repeats. 
@param aDuration The duration, in years, for which the event should 
recur. A value of zero indicates the repeat should continue forever. 
@param aEndDate Optional pointer to the end date for the repeat event. The object takes 
ownership of this pointer. 
@param aArrayOfDaysInYearOccurrences Pointer to an array of integers between 
1 and 366 inclusive. Each integer represents a day on which the repeat event 
occurs. The object takes ownership of this pointer. */
	{
	iRepeatType=EYearlyByDay;
	}

EXPORT_C CVersitRecurrenceYearlyByDay::~CVersitRecurrenceYearlyByDay()
/** Frees all resources owned by the object, prior to its destruction. */
	{
	delete iArrayOfDaysInYearOccurrences;
	}

EXPORT_C void CVersitRecurrenceYearlyByDay::ExternalizeOccurrenceListsL(RWriteStream& aStream) const
/** Writes the days of the year on which the event occurs to the output stream, 
aStream.

For example, the function might write the string "10 11 12 13 14 15 ".

@param aStream The stream to which the occurrence list is to be written. */
	{
	if (iArrayOfDaysInYearOccurrences)
		{
		TInt count=iArrayOfDaysInYearOccurrences->Count();
		TInt occurrence;
		TBuf8<KVersitDefaultBufferSize> temp;
		for (TInt ii = 0; ii < count; ii++)
			{
			occurrence = (*iArrayOfDaysInYearOccurrences)[ii];
			temp.Num(occurrence);
			if ((occurrence < 366) && (occurrence > 0)) //day in year
				{
				aStream.WriteL(temp);
				aStream.WriteL(KVersitTokenSpace);
				}
			}
		}
	}

//
// CParserPropertyValueRecurrence
//
EXPORT_C CParserPropertyValueRecurrence::CParserPropertyValueRecurrence(CVersitRecurrence* aValue)
: CParserTimePropertyValue(TUid::Uid(KVCalPropertyRecurrenceUid))
, iValue(aValue)
/** Constructs a recurrence property value. 

Sets the property value's UID to KVCalPropertyRecurrenceUid.

@param aValue A pointer to the recurrence value to assign to the property 
value parser. The property value parser takes ownership of the pointer. */
	{}

EXPORT_C CParserPropertyValueRecurrence::~CParserPropertyValueRecurrence()
/** Frees all resources owned by the property value, prior to its destruction. */
	{
	delete iValue;
	}

EXPORT_C void CParserPropertyValueRecurrence::ConvertAllDateTimesToUTCL(const TTimeIntervalSeconds& /*aIncrement*/,const CVersitDaylight* /*aDaylight*/)
/** Converts the end time of the recurrence value into universal time.

The date/time is checked against the daylight saving information provided 
in aDaylight. If it falls inside the daylight saving period then the daylight 
saving offset is subtracted from the time to convert it to universal time. 
Otherwise the time is modified by aIncrement to convert it to universal time.

Note that the daylight savings offset will adjust the time both for the daylight 
saving and for the time zone.

The function has no effect if the value is already stored in universal time.

If aDaylight is a NULL pointer then aIncrement is used.

@param aIncrement A time interval in seconds which represents the negative 
of the time zone of the originating machine. For instance, if the time zone 
is +04:30 (that is 4hr 30mins ahead of UTC), aIncrement should be set to minus 
the number of seconds in 4hr 30mins..
@param aDaylight Pointer to the specification for daylight saving. If the date/time 
value is within the period for daylight saving, the value is modified by the 
daylight saving offset (which accounts for both the time zone and daylight 
saving rule). 
@deprecated since 9.1
*/
	{
	}

EXPORT_C void CParserPropertyValueRecurrence::ConvertAllUTCDateTimesToMachineLocalL(const TTimeIntervalSeconds& /*aIncrement*/)
/** Converts the end time of the recurrence value into machine-local time. 

This process involves adjusting the date/time value by the offset in aIncrement.

The function has no effect if the value is already stored in machine-local 
time.

@param aIncrement A time interval which represents the number of seconds which 
is to be added to the date/time value. This should normally be the universal 
time offset for the machine's locale. 
@deprecated since 9.1
*/
	{
	}

EXPORT_C void CParserPropertyValueRecurrence::ExternalizeL(RWriteStream& aStream,const Versit::TEncodingAndCharset& /*aEncodingCharset*/
																																,TInt /*aLengthOutput*/)
/** Externalises the recurrence property value into aStream.

The property is written to the stream in the following order:

- Repeat type, e.g. "D" is written for a daily repeat (see KVersitRecurrenceDaily).

- Repeat interval, e.g. "2" for a weekly repeat type that occurs fortnightly.

- Occurrence list (optional), e.g. the string "MO TH" is written for a weekly repeat type that recurs 
on Mondays and Thursdays.

- End date (optional) - the date on which the event should stop recurring.

- Duration, e.g. "#10" for a weekly repeat type that should last for 10 weeks.

For example, "W2 TU TH #10", means a repeat every second Tuesday and Thursday for 10 weeks

@param aStream Stream to which the value is to be externalised.
@param aEncodingCharset Specifies character set and encoding information. This is not 
used by this function since the recurrence rule value is in ASCII.
@param aLengthOutput The amount of text that has been output so far on the line. This is not used by 
this function since the line wrapping is not a general case for recurrence values. */
	{
	if (!iValue)
		return;
	switch(iValue->iRepeatType)
		{
	case CVersitRecurrence::EDaily:
		aStream.WriteL(KVersitRecurrenceDaily);
		break;
	case CVersitRecurrence::EWeekly:
		aStream.WriteL(KVersitRecurrenceWeekly);
		break;
	case CVersitRecurrence::EMonthlyByPos:
		aStream.WriteL(KVersitRecurrenceMonthlyByPos8);
		break;
	case CVersitRecurrence::EMonthlyByDay:
		aStream.WriteL(KVersitRecurrenceMonthlyByDay8);
		break;
	case CVersitRecurrence::EYearlyByMonth:
		aStream.WriteL(KVersitRecurrenceYearlyByMonth8);
		break;
	case CVersitRecurrence::EYearlyByDay:
		aStream.WriteL(KVersitRecurrenceYearlyByDay8);
		break;
	default:
		Panic(ENoRecognizedRepeatType);
		break;
		};
	TBuf8<KVersitDefaultBufferSize> temp;
	temp.Num(iValue->iInterval);
	aStream.WriteL(temp);
	aStream.WriteL(KVersitTokenSpace);
	iValue->ExternalizeOccurrenceListsL(aStream);
	if (iValue->iEndDate)
		{
		TBuf8<KVersitMaxDateTimeLength> buf;
		EncodeVersitDateTimeL(buf,*iValue->iEndDate);
		aStream.WriteL(buf);
		}
	else 
		{
		aStream.WriteL(KVersitRecurrenceNumberOf);
		temp.Num(iValue->iDuration);
		aStream.WriteL(temp);
		}
	}

//
// CRecurrenceParser
//
EXPORT_C CRecurrenceParser::CRecurrenceParser(TBool aHasVersion)
	: CVersitParser(aHasVersion)		//(aHasVersion? ESupportsVersion:ENoVersionProperty)	
/** Constructs a recurrence parser.

@param aHasVersion CVersitParser::ESupportsVersion if a version property is needed; 
ENoVersionProperty otherwise. */
	{}

EXPORT_C CParserPropertyValue* CRecurrenceParser::MakePropertyValueRecurrenceL(TDes& aRecurringEntity)
/** Internalises a recurrence property value in descriptor form.

Creates a recurrence parser using the recurrence information provided in aRecurringEntity 
(which has the format described in CParserPropertyValueRecurrence::ExternalizeL()).

@param aRecurringEntity A recurring entity in descriptor form. */
	{
	aRecurringEntity.TrimAll();
	if(aRecurringEntity.Length()==0)
		{
		CParserPropertyValue* value = new(ELeave) CParserPropertyValueRecurrence(NULL);
		return value;
		}
		
	// if there is no space between Event's interval (e.g. D1) and occurrence (e.g. #2), 
	// i.e. for e.g. D1#2 then leave 
	TInt posSpace=aRecurringEntity.Locate(ESpace);
	TInt posNumSign=aRecurringEntity.Locate(KVersitRecurrenceNumberOfVal);
	if(posSpace==KErrNotFound && posNumSign!=KErrNotFound)
		{
		User::Leave(KVersitErrBadRepeatValue);
		}
	//Extract the first part - Recurrence Type	
	TPtrC recurrEntity(aRecurringEntity);
	TPtrC recurrenceType;
	if(posSpace==KErrNotFound)
		{
		recurrenceType.Set(recurrEntity.Left(aRecurringEntity.Length()));
		}
	else
		{
		recurrenceType.Set(recurrEntity.Left(posSpace));
		}

	if(recurrenceType.Length()<2)
		{
		// Only one letter for iRecurrenceType is not acceptable.
		User::Leave(KVersitErrBadRepeatValue);
		}

	// if occurrence is not specified i.e. only D<n> specified, not D<n> #<n>
	// then by vcal specification default occurrence value should be 2
	TInt len=aRecurringEntity.Length()-posSpace-1;
	_LIT(KVersitRecurrenceDefVal, "#2");
	TPtr restString((TPtr &)KVersitRecurrenceDefVal);

	//if occurrence is specified the extract from Recurring Rule
	if(posSpace!=KErrNotFound) 
		{
		restString.Set(&aRecurringEntity[posSpace+1],len,len);
		}

	posSpace=restString.LocateReverse(ESpace);
	TBool dateTimeAtEnd=EFalse;
	TBool numCurrenceAtEnd=EFalse;
	TBool thereIsADateList=ETrue;
	len=restString.Length()-posSpace-1;
	TPtr dateTime(&restString[posSpace+1],len,len);
	if(dateTime.Length()>14)
		{
		dateTimeAtEnd=ETrue;
		if (posSpace==KErrNotFound)
			restString.Set(NULL,0,0);
		else
			restString.Set(&restString[0],posSpace,posSpace);
		}
	else 
		{
		//Ensure that dateTime is a time parameter, and not a weekday.
		//This is true if the sequence length is greater than two
		//characters, and the sequence begins with 'T'
		TInt posTimeDelimeter=dateTime.Locate(KVersitTimePeriodTimeVal);
		if(posTimeDelimeter == 0 && dateTime.Length() > 2)
			{
			//we want to strip off this date from 
			if(posSpace==KErrNotFound)
				restString.Set(NULL,0,0);
			else
				restString.Set(&restString[0],posSpace,posSpace);
			}
		}
	TPtrC numCurrence;
	TPtrC dateList;//use null string
	if (restString.Length()>0)
		{
		posSpace=restString.Locate(KVersitRecurrenceNumberOfVal);
		if(posSpace >= 0)
			{
			numCurrenceAtEnd=ETrue;
			if(posSpace>restString.Length()-2)
				User::Leave(KVersitErrBadRepeatValue);
			numCurrence.Set(restString.Mid(posSpace+1));
			}
		if(posSpace > 1)
			{
			len=posSpace-1;
			restString.Set(&restString[0],len,len);//there is space before '#'
			}
		else
			thereIsADateList=(posSpace==KErrNotFound);
		}
	else
		thereIsADateList=EFalse;
	if(thereIsADateList)
		{
		dateList.Set(restString);
		}
	CVersitRecurrence::TType repeatType;
	TInt interval;
	GetFrequencyAndIntervalL(repeatType,interval,recurrenceType);//parse first part, such as W2,D1,YM1...
	CVersitRecurrence* recurrence = GetFrequencyModifiersL(repeatType,interval,dateList/*dateListConcatenated*/);//parse the second part such as "1TH TU 2 WE"
	CleanupStack::PushL(recurrence);
	
	// set duration and enddate values
	if(numCurrenceAtEnd)
		{
		TInt numChar;
		recurrence->iDuration = GetNumberL(numCurrence,numChar);
		}
	if(dateTimeAtEnd)
		// Coverity doesn't acknowledge member assignment as leave safe
		// coverity[alloc_fn] coverity[assign]
		recurrence->iEndDate=DecodeDateTimeL(dateTime);

	// no duration (#n) and no end date implies duration = #2
	if ( !(numCurrenceAtEnd || dateTimeAtEnd) )
		{
		recurrence->iDuration = 2; 
		}

	// coverity[leave_without_push]
	CParserPropertyValue* value = new(ELeave) CParserPropertyValueRecurrence(recurrence);
	CleanupStack::Pop(recurrence);
	return value;
	}

void CRecurrenceParser::ResetAndDestroyArrayOfMonthPositions(TAny* aObject)	
	{
	CArrayPtr<CVersitRecurrenceMonthlyByPos::CMonthPosition>* array=REINTERPRET_CAST(CArrayPtr<CVersitRecurrenceMonthlyByPos::CMonthPosition>*,aObject);
	if (array)
		array->ResetAndDestroy();
	delete array;
	}

// For interoperability with Nokia 9110's incorrectly specified
// vCalendar repeat rules a check is made after matching a 'M' or 'Y'
// character that the next character isn't the repeat interval.  See
// defects EDNCDUG-4FXPKP and EDNCDUG-4FXRAA (Symbian defect tracking v3)
// for more detail.
//
void CRecurrenceParser::GetFrequencyAndIntervalL(CVersitRecurrence::TType& aFrequency,TInt& aInterval, const TDesC& aRecurrenceType)
	{
	TInt position = 0;
	const TUint type = aRecurrenceType[position];
	TPtrC recMonthlyByPos(KVersitRecurrenceMonthlyByPos);//MP
	TPtrC recYearlyByDay(KVersitRecurrenceYearlyByDay);//YD
	if (type == KVersitRecurrenceDailyVal)
		{
		position++;
		aFrequency = CVersitRecurrence::EDaily;
		}
	else if (type == KVersitRecurrenceWeeklyVal)
		{
		position++;
		aFrequency = CVersitRecurrence::EWeekly;
		}
	else if (type == recMonthlyByPos[0])
		{
		position++;
		if (aRecurrenceType[position] == recMonthlyByPos[position])
			aFrequency = CVersitRecurrence::EMonthlyByPos;
		else
			aFrequency = CVersitRecurrence::EMonthlyByDay;
		TChar nextChar = aRecurrenceType[position];
		if (!nextChar.IsDigit())
			position++;
		}
	else if (type == recYearlyByDay[0])
		{
		position++;
		if (aRecurrenceType[position] == recYearlyByDay[position])
			aFrequency = CVersitRecurrence::EYearlyByDay;
		else
			aFrequency = CVersitRecurrence::EYearlyByMonth;
		TChar nextChar = aRecurrenceType[position];
		if (!nextChar.IsDigit())
			position++;
		}
	else
		User::Leave(KVersitErrRepeatTypeNotHandled);
	TInt numChars;
	aInterval = GetNumberL(aRecurrenceType.Mid(position),numChars);
	}

CVersitRecurrence* CRecurrenceParser::GetFrequencyModifiersL(const CVersitRecurrence::TType& aRepeatType,TInt aInterval, const TDesC& aListDates)
	{
	CVersitRecurrence* recurrence = NULL;
	switch (aRepeatType)
		{
		case CVersitRecurrence::EDaily:
			recurrence = new(ELeave)CVersitRecurrenceDaily(aInterval, 0, 0);
			break;
		case CVersitRecurrence::EWeekly:
			{
			CWeekDayArray* weekDays=GetListOfWeekDayOccurrencesL(aListDates);
			CleanupStack::PushL(weekDays);
			recurrence = new(ELeave)CVersitRecurrenceWeekly(aInterval, 0, 0, weekDays);
			CleanupStack::Pop(weekDays);
			}
			break;
		case CVersitRecurrence::EMonthlyByPos:
			{
			CArrayPtrFlat<CVersitRecurrenceMonthlyByPos::CMonthPosition>* monthPositions
				= new(ELeave) CArrayPtrFlat<CVersitRecurrenceMonthlyByPos::CMonthPosition>(5);
			CleanupStack::PushL(TCleanupItem(ResetAndDestroyArrayOfMonthPositions, monthPositions));
			CPtrCArray* array =new(ELeave)CPtrC16Array(5);//each of them is something like "1 TH TU"
			CleanupStack::PushL(array);//
			TInt i;
			TInt lastDigitPos=0;
			TInt length=aListDates.Length();
			if (length>0)
				{
				for(i=1;i<length;i++)
					{//the first digit is not we after, eg. "1+ MO 2+ TU 3+ WE #10", the first digit we are looking for is "2" and the first element of the array is "1+ MO"
					if(((TChar)aListDates[i]).IsDigit())
						{
						if(i-lastDigitPos==1)
							User::Leave(KVersitErrBadRepeatValue);
						array->AppendL(aListDates.Mid(lastDigitPos,i-lastDigitPos));
						lastDigitPos=i;
						}
					}
				if(lastDigitPos!=length-1)
					array->AppendL(aListDates.Mid(lastDigitPos));
				}
			TInt count=array->Count();
			for(i=0;i<count;i++)
				{
				CVersitRecurrenceMonthlyByPos::CMonthPosition* newMonthPosition=new(ELeave)CVersitRecurrenceMonthlyByPos::CMonthPosition;
				CleanupStack::PushL(newMonthPosition);//
				TInt posSpace=((*array)[i].Locate(ESpace));
				TPtrC weekNo((*array)[i]);
				TPtrC dayList;
				if(posSpace!=KErrNotFound)
					{
					weekNo.Set((*array)[i].Left(posSpace));
					if(posSpace!=(*array)[i].Length()-1)//space is not the last char
						{
						dayList.Set((*array)[i].Mid(posSpace+1));
						}
					}
				else
					User::Leave(KVersitErrBadRepeatValue);
				if ((TChar)weekNo[weekNo.Length()-1]==KVersitTokenMinusVal)
					newMonthPosition->iSign=CVersitRecurrenceMonthlyByPos::CMonthPosition::EWeeksFromEndOfMonth;
				else
					newMonthPosition->iSign=CVersitRecurrenceMonthlyByPos::CMonthPosition::EWeeksFromStartOfMonth;
				TInt numChars;
				newMonthPosition->iWeekNo=GetNumberL((*array)[i],numChars); //1-5
				if ((newMonthPosition->iWeekNo<6)&&(newMonthPosition->iWeekNo>0))
					{
					newMonthPosition->iArrayOfWeekDays=GetListOfWeekDayOccurrencesL(dayList);
					if(newMonthPosition->iArrayOfWeekDays)
						{
						monthPositions->AppendL(newMonthPosition);
						CleanupStack::Pop(newMonthPosition); //
						}
					else
						CleanupStack::PopAndDestroy(newMonthPosition);
					}
				else
					CleanupStack::PopAndDestroy(newMonthPosition); //

				}
			CleanupStack::PopAndDestroy(array);//
			recurrence=new(ELeave)CVersitRecurrenceMonthlyByPos(aInterval,0,0,monthPositions);
			CleanupStack::Pop(monthPositions); // 
			break;
			}
		case CVersitRecurrence::EMonthlyByDay:
			{
			CArrayFix<TInt>* arrayOfOccurrencesInDaysFromStartOfMonth=new(ELeave)CArrayFixFlat<TInt>(3);
			CleanupStack::PushL(arrayOfOccurrencesInDaysFromStartOfMonth);
			CArrayFix<TInt>* arrayOfOccurrencesInDaysFromEndOfMonth=new(ELeave)CArrayFixFlat<TInt>(3);
			CleanupStack::PushL(arrayOfOccurrencesInDaysFromEndOfMonth);
			TBool lastDay=EFalse;

			TPtrC restdates(aListDates);
			TPtrC date(aListDates);
			TInt posSpace=0;
			TInt i=0;
			TInt length=aListDates.Length();

			while(i<length&&(posSpace!=KErrNotFound))
				{
				i++;
				posSpace=restdates.Locate(ESpace);
				if(posSpace>0)
					{
					date.Set(restdates.Left(posSpace));
					if(posSpace<restdates.Length()-1)
						restdates.Set(restdates.Mid(posSpace+1));
					else
						posSpace=KErrNotFound;
					}
				else
					{
					date.Set(restdates);
					posSpace=KErrNotFound;
					}

				if(TChar(date[0]).IsDigit())
					{
					TInt numChar;
					if (aListDates[date.Length()-1]==KVersitTokenMinusVal)
						arrayOfOccurrencesInDaysFromEndOfMonth->AppendL(GetNumberL(date,numChar));
					else
						arrayOfOccurrencesInDaysFromStartOfMonth->AppendL(GetNumberL(date,numChar));
					}
				else if (date.CompareC(KVersitRecurrenceLastDay))//LD
					lastDay=ETrue;
				}

			if (arrayOfOccurrencesInDaysFromStartOfMonth->Count())
				{
				if (arrayOfOccurrencesInDaysFromEndOfMonth->Count())
					{
					recurrence = new(ELeave)CVersitRecurrenceMonthlyByDay(aInterval, 0, 0, arrayOfOccurrencesInDaysFromStartOfMonth, arrayOfOccurrencesInDaysFromEndOfMonth, lastDay);
					CleanupStack::Pop(2,arrayOfOccurrencesInDaysFromStartOfMonth); // 
					}
				else
					{
					recurrence = new(ELeave)CVersitRecurrenceMonthlyByDay(aInterval, 0, 0, arrayOfOccurrencesInDaysFromStartOfMonth, NULL, lastDay);
					CleanupStack::PopAndDestroy(); // 
					arrayOfOccurrencesInDaysFromEndOfMonth = NULL;
					CleanupStack::Pop(arrayOfOccurrencesInDaysFromStartOfMonth); // 
					}
				}
			else
				{
				if (arrayOfOccurrencesInDaysFromEndOfMonth->Count())
					{
					recurrence = new(ELeave)CVersitRecurrenceMonthlyByDay(aInterval, 0, 0, NULL, arrayOfOccurrencesInDaysFromEndOfMonth, lastDay);
					CleanupStack::Pop(arrayOfOccurrencesInDaysFromEndOfMonth); // 
					}
				else										
					{
					recurrence = new(ELeave)CVersitRecurrenceMonthlyByDay(aInterval, 0, 0, NULL, NULL, lastDay);
					CleanupStack::PopAndDestroy(arrayOfOccurrencesInDaysFromEndOfMonth); // 
					arrayOfOccurrencesInDaysFromEndOfMonth = NULL;
					}
				CleanupStack::PopAndDestroy(arrayOfOccurrencesInDaysFromStartOfMonth); // 
				arrayOfOccurrencesInDaysFromStartOfMonth = NULL;
				}			
			break;
			}
		case CVersitRecurrence::EYearlyByMonth:
			{
			CArrayFix<TMonth>* arrayOfMonthInYearOccurrences=new(ELeave)CArrayFixFlat<TMonth>(3);
			CleanupStack::PushL(arrayOfMonthInYearOccurrences);
			TInt month;
			TInt length=aListDates.Length();
			TPtrC restdates(aListDates);
			TPtrC date(aListDates);
			TInt posSpace=0;
			TInt i=0;
			while(i<length&&(posSpace!=KErrNotFound))
				{
				i++;
				posSpace=restdates.Locate(ESpace);
				if(posSpace>0)
					{
					date.Set(restdates.Left(posSpace));
					if(posSpace<restdates.Length()-1)
						restdates.Set(restdates.Mid(posSpace+1));
					else
						posSpace=KErrNotFound;
					}
				else
					{
					date.Set(restdates);
					posSpace=KErrNotFound;
					}
				if(TChar(date[0]).IsDigit())
					{
					TInt numChars;
					month = GetNumberL(date,numChars);
					if ((month > 0) && (month < 13))
						arrayOfMonthInYearOccurrences->AppendL(TMonth(month - 1));
					}
				}

			if (arrayOfMonthInYearOccurrences->Count())
				{
				recurrence = new(ELeave)CVersitRecurrenceYearlyByMonth(aInterval, 0, 0, arrayOfMonthInYearOccurrences);
				CleanupStack::Pop(arrayOfMonthInYearOccurrences); //
				}
			else
				{
				recurrence = new(ELeave)CVersitRecurrenceYearlyByMonth(aInterval, 0, 0, NULL);
				CleanupStack::PopAndDestroy(arrayOfMonthInYearOccurrences); //
				arrayOfMonthInYearOccurrences = NULL;
				}
			break;
			}
		case CVersitRecurrence::EYearlyByDay:
			{
			CArrayFix<TInt>* arrayOfDayInYearOccurrences=new(ELeave)CArrayFixFlat<TInt>(3);
			CleanupStack::PushL(arrayOfDayInYearOccurrences);
			TInt day;
			TInt length=aListDates.Length();
			TPtrC restdates(aListDates);
			TPtrC date(aListDates);
			TInt posSpace=0;
			TInt i=0;
			while(i<length&&(posSpace!=KErrNotFound))
				{
				i++;
				posSpace=restdates.Locate(ESpace);
				if(posSpace>0)
					{
					date.Set(restdates.Left(posSpace));
					if(posSpace<restdates.Length()-1)
						restdates.Set(restdates.Mid(posSpace+1));
					else
						posSpace=KErrNotFound;
					}
				else
					{
					date.Set(restdates);
					posSpace=KErrNotFound;
					}
				if(TChar(date[0]).IsDigit())
					{
					TInt numChars;
					day = GetNumberL(date,numChars);
					if ((day > 0) && (day < 366))
						arrayOfDayInYearOccurrences->AppendL(day);
					}
				}
			if (arrayOfDayInYearOccurrences->Count())
				{
				recurrence = new(ELeave)CVersitRecurrenceYearlyByDay(aInterval, 0, 0, arrayOfDayInYearOccurrences);
				CleanupStack::Pop(arrayOfDayInYearOccurrences); //arrayOfDayInYearOccurrences
				}
			else
				{
				recurrence = new(ELeave)CVersitRecurrenceYearlyByDay(aInterval, 0, 0, NULL);
				CleanupStack::PopAndDestroy(arrayOfDayInYearOccurrences); //arrayOfDayInYearOccurrences
				arrayOfDayInYearOccurrences = NULL;
				}
			break;
			}
		};
	return recurrence;
	}

CWeekDayArray* CRecurrenceParser::GetListOfWeekDayOccurrencesL(const TDesC& aListDays)
	{
	if (aListDays.Length()==0)
		return NULL;
	CWeekDayArray* arrayOfWeekDayOccurrences = new(ELeave)CWeekDayArray;
	CleanupStack::PushL(arrayOfWeekDayOccurrences);
	arrayOfWeekDayOccurrences->iArray = new(ELeave)CArrayFixFlat<TDay>(3);
	TPtrC restdays(aListDays);
	TPtrC day(aListDays);
	TInt posSpace=0;
	while(posSpace!=KErrNotFound)
		{
		posSpace=restdays.Locate(ESpace);
		if(posSpace>0)
			{
			day.Set(restdays.Left(posSpace));
			if(posSpace<restdays.Length()-1)
				restdays.Set(restdays.Mid(posSpace+1));
			else
				posSpace=KErrNotFound;
			}
		else
			{
			day.Set(restdays);
			posSpace=KErrNotFound;
			}
		((TChar)day[0]).UpperCase();
		switch ((TChar)day[0])
			{
			case 'F':
				if (!day.CompareC(KVersitRecurrenceFriday))
					arrayOfWeekDayOccurrences->iArray->AppendL(EFriday);
				break;
			case 'M':
				if (!day.CompareC(KVersitRecurrenceMonday))
					arrayOfWeekDayOccurrences->iArray->AppendL(EMonday);
				break;
			case 'S':
				if (!day.CompareC(KVersitRecurrenceSaturday()))
					arrayOfWeekDayOccurrences->iArray->AppendL(ESaturday);
				else if (!day.CompareF(KVersitRecurrenceSunday))
					arrayOfWeekDayOccurrences->iArray->AppendL(ESunday);
				break;
			case 'T':
				if (!day.CompareC(KVersitRecurrenceTuesday))
					arrayOfWeekDayOccurrences->iArray->AppendL(ETuesday);
				else if (!day.CompareF(KVersitRecurrenceThursday))
					arrayOfWeekDayOccurrences->iArray->AppendL(EThursday);
				break;
			case 'W':
				if (!day.CompareC(KVersitRecurrenceWednesday))
					arrayOfWeekDayOccurrences->iArray->AppendL(EWednesday);
				break;
			default:
				User::Leave(KVersitErrBadRepeatValue);
			}
		}
	CleanupStack::Pop(arrayOfWeekDayOccurrences);
	if (arrayOfWeekDayOccurrences->iArray->Count())
		return arrayOfWeekDayOccurrences;
	delete arrayOfWeekDayOccurrences;
	return NULL;
	}

EXPORT_C void CRecurrenceParser::Reserved1()
	{}

EXPORT_C void CRecurrenceParser::Reserved2()
	{}
