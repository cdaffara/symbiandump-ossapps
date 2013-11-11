/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


//debug
#include <qdatetime.h>
#include "calendarui_debug.h"

#include "calendateutils.h"
#include <agendautil.h>

#include <hbextendedlocale.h>

//  LOCAL CONSTANTS AND MACROS
const int KDefaultStartTime(8);    // 8 am ( 0 to 23 hour scale)

// ============================ MEMBER FUNCTIONS ==============================

// ============================  CalenDateUtils  ==============================
// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 bool CalenDateUtils::onSameDay( const QDateTime& x, const QDateTime& y )
    {
    return x.date().year()  == y.date().year() 
        && x.date().month() == y.date().month()
        && x.date().day()   == y.date().day();
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 bool CalenDateUtils::onSameMonth( const QDateTime& x, const QDateTime& y )
    {
    return ( x.date().year() == y.date().year() && x.date().month() == y.date().month() );
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::beginningOfDay( const QDateTime& startTime )
    {    
    QTime zeroTime(0,0,0,0);
    QDateTime ret(startTime.date(), zeroTime);
    return ret;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::displayTimeOnDay( const QDateTime& startTime,
                                                 const QDateTime& day )
    {
    if( ! onSameDay( startTime, day ) )
        {
        return beginningOfDay( day );
        }
    else 
        {
        return startTime;
        }
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 bool CalenDateUtils::timeRangesIntersect( const QDateTime& xStart,
                                                    const QDateTime& xEnd,
                                                    const QDateTime& yStart,
                                                    const QDateTime& yEnd )
    {
    return (! ( yEnd <=  xStart || xEnd <= yStart )
        || (xStart == xEnd && yStart <= xStart && xStart < yEnd)
        || (yStart == yEnd && xStart <= yStart && yStart < xEnd)
        || (xStart == xEnd && yStart == yEnd && xStart == yStart));
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 bool CalenDateUtils::isValidDay( const QDateTime& time )
    {
    // Interim API supports range from 1900-2100, 
    return ( minTime() <= time && time <= maxTime() );
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::limitToValidTime( const QDateTime& time )
    {    
    QDateTime valid = time;
    valid = valid > maxTime() ? maxTime() : valid;
    valid = valid < minTime() ? minTime() : valid;

    return valid;
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// Returns maximum time allowed, 31.12.2100 0:00 is max so 30.12.2100 is last actual
// date to be used.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::maxTime()
    { 
    return AgendaUtil::maxTime();
    }

// -----------------------------------------------------------------------------
// ?classname::?member_function
// Returns minimum time allowed, 1.1.1900 0:00 is min
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::minTime()
    {
    return AgendaUtil::minTime();
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 int CalenDateUtils::timeOfDay( const QDateTime& dateTime )
    {    
    QDateTime midnight = beginningOfDay( dateTime );
    int resultInSec = midnight.secsTo(dateTime);
    
    return (resultInSec/60);
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::roundToPreviousHour( const QDateTime& dateTime ) 
    {
    QTime time = dateTime.time();
    time.setHMS(time.hour(),0,0,0);
    return QDateTime( dateTime.date(), time );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 int CalenDateUtils::roundToPreviousHour( const int& minutes )
    {
    return ( (minutes / 60) * 60 );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::now()
    {
    return QDateTime::currentDateTime();
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::today()
    {
    return CalenDateUtils::beginningOfDay( now() );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 bool CalenDateUtils::isOnToday( const QDateTime& time )
    {
    return CalenDateUtils::onSameDay( time, today() );
    }

// -----------------------------------------------------------------------------
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 QDateTime CalenDateUtils::defaultTime( const QDateTime& date )
    {
    QDateTime dateTime;
    // DD:MM:YY @ hh:mm:ss
    dateTime = CalenDateUtils::beginningOfDay( date ); 
    // DD:MM:YY @ 00:00:00
    QTime time(KDefaultStartTime, 0, 0, 0);
    dateTime.setTime(time); // DD:MM:YY @ 08:00 am
  
    return dateTime;
    }      

 // -----------------------------------------------------------------------------
 // (other items were commented in a header).
 // -----------------------------------------------------------------------------
 //
 QDateTime CalenDateUtils::futureOf(const QDateTime& dateTime, int numOfDays)
     {
     QDateTime result;
     int dayNumber = dateTime.date().day();
     int numOfDaysInMonth = dateTime.date().daysInMonth();
     int month = dateTime.date().month();
     int year = dateTime.date().year();
     int buff = numOfDays;
     QDate date(year, month, dayNumber);
     while(dayNumber + buff > numOfDaysInMonth)
         {
         if(month == 12) {
         // If December,
         month = 1; // January
         year++;
         }
         else {
         month++;
         }
         // Create the qdate with these details
         date.setDate(year, month, 1);
         // check to see if it goes beyond the next month also
         buff = buff - (numOfDaysInMonth - dayNumber);
         dayNumber = 0;
         numOfDaysInMonth = date.daysInMonth();
         }
     
     // Add the buff days to the day number to get the result
     int day = dayNumber + buff;
     
     date.setYMD(date.year(), date.month(), day);
     result.setDate(date);
     result.setTime(dateTime.time());
     return result;
     }
 
 /*!
	 Retruns the dateformat based current locale settings.
  */
 QString CalenDateUtils::dateFormatString()
 {
	 HbExtendedLocale locale = HbExtendedLocale::system();
 
	 QString dateFormat;
	 switch (locale.dateStyle()) {
		 case HbExtendedLocale::American:
			 dateFormat.append("MM");
			 dateFormat.append(locale.dateSeparator(1));
			 dateFormat.append("dd");
			 dateFormat.append(locale.dateSeparator(1));
			 dateFormat.append("yyyy");
			 break;
 
		 case HbExtendedLocale::European:
			 dateFormat.append("dd");
			 dateFormat.append(locale.dateSeparator(1));
			 dateFormat.append("MM");
			 dateFormat.append(locale.dateSeparator(1));
			 dateFormat.append("yyyy");
			 break;
 
		 case HbExtendedLocale::Japanese:
			 dateFormat.append("yyyy");
			 dateFormat.append(locale.dateSeparator(1));
			 dateFormat.append("MM");
			 dateFormat.append(locale.dateSeparator(1));
			 dateFormat.append("dd");
			 break;
	 }
 
	 return dateFormat;
 }
 
// End of File
