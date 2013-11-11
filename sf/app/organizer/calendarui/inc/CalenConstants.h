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
* Description:   Constants used in every where in Calendar code
 *
*/



#ifndef CALENCONSTANTS_H
#define CALENCONSTANTS_H

//  INCLUDES
#include <avkon.rsg>
#include <e32std.h>

// CONSTANTS
const TInt KCalenMinutesInHour = 60;
const TInt KCalenHoursInDay = 24;
const TInt KCalenDaysInWeek = 7;

const TInt KNoUserInfoNoteDisplay = 0;
const TInt KCalenTimeZoneChanged = 1; 
const TInt KCalenLostAlarms = 2; 

const TInt KNaviLabelSize = 32;

const TInt KDayNames[] =
    {
    R_QTN_WEEK_LONG_MONDAY,
    R_QTN_WEEK_LONG_TUESDAY,
    R_QTN_WEEK_LONG_WEDNESDAY,
    R_QTN_WEEK_LONG_THURSDAY,
    R_QTN_WEEK_LONG_FRIDAY,
    R_QTN_WEEK_LONG_SATURDAY,
    R_QTN_WEEK_LONG_SUNDAY
    };

const TInt KMonthNames[] =
    {
    R_QTN_MONTH_LONG_JANUARY,
    R_QTN_MONTH_LONG_FEBRUARY,
    R_QTN_MONTH_LONG_MARCH,
    R_QTN_MONTH_LONG_APRIL,
    R_QTN_MONTH_LONG_MAY,
    R_QTN_MONTH_LONG_JUNE,
    R_QTN_MONTH_LONG_JULY,
    R_QTN_MONTH_LONG_AUGUST,
    R_QTN_MONTH_LONG_SEPTEMBER,
    R_QTN_MONTH_LONG_OCTOBER,
    R_QTN_MONTH_LONG_NOVEMBER,
    R_QTN_MONTH_LONG_DECEMBER
    };

const TInt KWeekTitleSize(64);

const QString activityName = "Calendar"; //Activity Name to be stored in Activity Manager DB

// FIXME: can be here now, but later on move to some other header.
enum TCalenWeekTitle
    {
    EWeekTitleNumber,
    EWeekTitleDuration
    };

#endif // CALENCONSTANTS_H


// End of File
