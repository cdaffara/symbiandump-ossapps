/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  MR organizer definition
*
*/


#ifndef M_MRRECURRENCERULE_H
#define M_MRRECURRENCERULE_H

#include <e32base.h>
//#include <mrinfodef.h>

/**
 * MRRecurrenceRule defines meeting request recurrence rule.
 */
class MRRecurrenceRule
    {
public:
    enum TMRRecurrenceType
        {
        /** The type has not yet been defined */
        EMRRecurrenceInvalid,
        /** Rule which repeats based on a number of days. */
        EMRRecurrenceDaily,        
        /** Rule which repeats based on a number of weeks */
        EMRRecurrenceWeekly,
        /** Rule which repeats based on a number of months */
        EMRRecurrenceMonthly,
	EMRRecurrenceMonthlyByDay,
        /** Rule which repeats based on a number of years */
        EMRRecurrenceYearly,
	EMRRecurrenceYearlyByDay
        };
    
    /** Definition for recurrent day */
    enum TMRRecurrentDay
        {
        EMRRecurrenceMonday,
        EMRRecurrenceTuesday,
        EMRRecurrenceWednesday,
        EMRRecurrenceThursday,
        EMRRecurrenceFriday,
        EMRRecurrenceSaturday,
        EMRRecurrenceSunday
        };
    
    /** Definition for recurrent week */
    enum TMRRecurrenceWeek
        {
        EMRRecurrenceLastWeekOfMonth   = -1,
        EMRRecurrenceFirstWeekOfMonth  =  1,
        EMRRecurrenceSecondWeekOfMonth =  2,
        EMRRecurrenceThirdWeekOfMonth  =  3,
        EMRRecurrenceFourthWeekOfMonth =  4,
        };
    
    /** Definition for recurrence months */
    enum TMRRecurrenceMonth
        {
        EMRRecurrenceJanuary = 1,
        EMRRecurrenceFebryary,
        EMRRecurrenceMarch,
        EMRRecurrenceApril,
        EMRRecurrenceMay,
        EMRRecurrenceJune,
        EMRRecurrenceJuly,
        EMRRecurrenceAugust,
        EMRRecurrenceSeptember,
        EMRRecurrenceOctober,
        EMRRecurrenceNovember,
        EMRRecurrenceDecember
        };
    
    /**
     * Definition to represent a weekday within a month.
     *
     * Valid values of iWeekInMonth are defined in enumeration 
     * TMRRecurrenceWeek. Valid values for iDayOfWeek are defined in 
     * enumeration TMRRecurrentDay.
     *
     * Example 1: The 3rd Wednesday would have iDay = EMRRecurrenceWednesday
     * and iWeekInMonth = EMRRecurrenceThirdWeekOfMonth. 
     *
     * Example 2: The last Sunday would have iDay = EMRRecurrenceSunday and 
     * iWeekInMonth = EMRRecurrenceLastWeekOfMonth.
     */
    struct TRecurrentDaysofMonth
        {
        TMRRecurrenceWeek iWeekOfMonth;
        TMRRecurrentDay   iDayOfWeek;
        };
    
public: // Destruction

    /**
     * Virtual destructor.
     */
    virtual ~MRRecurrenceRule()  { }

public: // Interface

    /**
     * Sets recurrence type. Calling this method will clear the 
     * previously set recurrence rules.
     * @param aType Recurrence rule type.
     * @exception System wide error code.
     */
    virtual void SetRecurrenceTypeL( TMRRecurrenceType aType ) = 0;

    /** 
     * Fetches recurrence type.
     * @return recurrence type.
     */
    virtual TMRRecurrenceType Type() const = 0;

    /**
     * Sets the recurrence start time.
     * @param aStartTime Recurrence start time as utc.
     * @return System wide error code.
     */
    virtual void SetRecurrenceStartTimeL( TTime aStartTime ) = 0;

    /**
     * Fetches the recurrence start time.
     * Time information is returned in utc.
     * @return recurrence start time as utc.
     */
    virtual TTime RecurrenceStartTime() const = 0;

    /**
     * Sets the recurrence until time.
     * @param aStartTime Recurrence until time in utc.
     * @return System wide error code.
     */
    virtual void SetRecurrenceUntilL( TTime aStartTime ) = 0;

    /**
     * Fetches the date on which this repeat rule finishes.
     * Time information is returned in utc.
     * @return The end time in utc.
     */
    virtual TTime RecurrentUntil() const = 0;
    
    /**
     * Sets recurrent rule interval.
     * @param aInterval Recurrent rule interval.
     * @exception Normal system wide error code.
     */
    virtual void SetRecurrentInterval( TInt aInterval ) = 0;
    
    /**
     * Fetches the recurrent interval.
     * Interval is a number of days for a daily repeat, a number of weeks for 
     * a weekly repeat, etc.
     * @return recurrent interval.
     */
    virtual TInt RecurrentInterval() const = 0;
    
    /**
     * Sets recurrent count.
     * @param aCount
     * @exception Normal system wide error code.
     */  
    virtual void SetRecurrentCountL( TInt aCount ) = 0;
    
    /**
     * Fetches the number of recurrences.
     * @return Number of recurreces.
     */
    virtual TUint RecurrentCount() const = 0;
    
    /**
     * Sets the all weekdays on which this rule is repeated.
     * Calling this method has no effect if recurrence rule type is not
     * weekly.
     * @param aDays Recurrence rule weekly days.
     * @exception Normal system wide error code.
     */
    virtual void SetRecurrentWeekDaysL( RArray<TMRRecurrentDay>& aDays ) = 0;
    
    /**
     * Fetches all weekdays on which this rule is repeated.
     * The returned array is empty if this is not a weekly recurrence.
     * @return Recurrent weekly days.
     */
    virtual const RArray<TMRRecurrentDay>& RecurrentWeekDays() const = 0;
    
    /**
     * Sets the days in the month on which this rule is repeated.
     * Calling this method has no effect if recurrence rule type is not
     * monthly or yearly.
     * @param aDays Days in the month on which this rule is repeated.
     * @exception Normal system wide error code.
     */
    virtual void SetRecurrentMonthDaysL( RArray<TInt>& aDays ) = 0;
    
    /**
     * Fetches all days in the month on which this rule is repeated.
     * The returned array is empty if this is not a monthly or yearly recurrence.
     * @return Days in the month on which this rule is repeated.
     */
    virtual const RArray<TInt>& RecurrentMonthDays() const = 0;

	virtual void SetRecurrentDaysofMonthL( RArray<TRecurrentDaysofMonth>& aDays ) = 0;
		
	virtual const RArray<TRecurrentDaysofMonth>& RecurrentDaysofMonth() const = 0;

    /**
     * Sets the months of the year for this repeat rule.
     * Calling this method has no effect if recurrence rule type is not
     * yearly.
     * @param aMonths Months of the year for this repeat rule.
     * @exception Normal system wide error code.
     */
    virtual void SetRecurrentMonthsL( RArray<TMRRecurrenceMonth>& aMonths ) = 0;
    
    /**
     * Fetches the months of the year for this repeat rule. 
     * This function will do nothing if this is not a yearly repeat rule.
     * @return Months of the year for this repeat rule. 
     */
    virtual const RArray<TMRRecurrenceMonth>& RecurrentMonths() const = 0;
    };


#endif // M_MRRECURRENCERULE_H

