/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  ?Description
 *
*/

#ifndef __CALENDATEUTILS_H__
#define __CALENDATEUTILS_H__

#include <e32base.h>
#include <qglobal.h>					// Q_DECL_EXPORT macro

#ifdef  CALENDATEUTILS_DLL
#define CALENDATEUTIL_EXPORT Q_DECL_EXPORT
#else
#define CALENDATEUTIL_EXPORT Q_DECL_IMPORT
#endif

// forward declarations
class QDateTime;

/**
 *  CalenDateUtils contains static utility functions useful
 *  when comparing and calculating with dates and times.
 *
 *  @lib Calendar.app
 *  @since 2.1
 */

class CALENDATEUTIL_EXPORT CalenDateUtils 
    {
public:
    static bool onSameDay( const QDateTime& x, const QDateTime& y );
    static bool onSameMonth( const QDateTime& x, const QDateTime& y );
    static QDateTime beginningOfDay( const QDateTime& startTime );
    static QDateTime displayTimeOnDay( const QDateTime& startTime, 
                                            const QDateTime& day );

    static bool timeRangesIntersect( const QDateTime& xStart,
                                               const QDateTime& xEnd,
                                               const QDateTime& yStart, 
                                               const QDateTime& yEnd );

    /**
     * Is aTime between KCalenMaxYear/KCalenMaxMonth/KCalenMaxDay
     * and KCalenMinYear/KCalenMinMonth/KCalenMinDay.
     * Min/Max day is defined agenda server.
     * @param aTime aTime to be checked
     * @return EFalse : Out of range
     */
    static bool isValidDay( const QDateTime& time );

    /**
     * Return Min or Max time if aTime goes out of bounds. 
     * Valid range is [CalenDateUtils::MinTime(), CalenDateUtils::MaxTime]
     * @param aTime time to be checked
     * @return aTime, if aTime in [CalenDateUtils::MinTime(), CalenDateUtils::MaxTime]
     *         CalenDateUtils::MinTime(), if aTime < CalenDateUtils::MinTime()
     *         CalenDateUtils::MaxTime(), if aTime > CalenDateUtils::MaxTime()
     */
    static QDateTime limitToValidTime( const QDateTime& time );

    /**
     * Return maximum allowed time. (31.
     */
    static QDateTime maxTime();

    /**
     * Return minimum allowed time.
     */
    static QDateTime minTime();

    /**
     * Return time of day as a minutes from midnight. Useful to get hours::minutes component of datetime, 
     * regardless of date
     */
    static int timeOfDay( const QDateTime& dateTime );

    /**
     * Round QDateTime to previous full hour, e.g. RoundToPreviousHour( 23.11.2006 9:31 ) = 23.11.2006 9:00
     */
    static QDateTime roundToPreviousHour( const QDateTime& dateTime );

    /**
     * Round QDateTimeIntervalMinutes to previous full hour, e.g. RoundToPreviousHour( 130 min ) = 120 min
     */
    static int roundToPreviousHour( const int& minutes );
    

    /**
     * @return current time. 
     */ 
    static QDateTime now();
    
    /**
     * @return today with time component set to 00:00. 
     */ 
    static QDateTime today();

    /**
     * @return ETrue if given aTime is on today, EFalse otherwise
     */     
    static bool isOnToday( const QDateTime& time );

    /* 
     * Given aDate = DD::MM::YY @ hh:mm:ss, it returns a QDateTime obj DD::MM::YY @ 08:00 am
     * @param: aDate, which has the DD::MM::YY
     */
    static QDateTime defaultTime( const QDateTime& date );
    
    static QDateTime futureOf(const QDateTime& dateTime, int numOfDays);
	
    /**
     * @return the dateformat based current locale settings.
     */
    static  QString dateFormatString();
    
   
private:
    // Hiding constructor, because this is static utility class.
    //lint -e{1526} 
    CalenDateUtils();

    // Hiding assignment, because this is static utility class.
    //lint -e{1526} 
    CalenDateUtils& operator=( const CalenDateUtils& );

    };

#endif // __CALENDATEUTILS_H__


// End of File
