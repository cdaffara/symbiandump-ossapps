/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CalenDay commons. All UI specified constants and enums shared
*               beetwen many files should be moved here.
*
*/

#ifndef CALENDAYCOMMONHEADERS_H_
#define CALENDAYCOMMONHEADERS_H_

#include <QDate>

// Constants defined by UI spec.

/*!
 Number of hours in day (UI spec)
 */
const int KCalenHoursInDay = 24;

/*!
 Number of time slots in hour (one slot = 15 minutes) (UI spec)
 */
const int KCalenSlotsInHour = 4;

/*!
 Content area (1/4 of screen) dedicated for All day events in Day View (UI spec)
 */
const qreal KCalenAllDayEventArea = 0.25;

/*!
 The width [un] of empty right column in Day View (UI spec)
 */
const qreal KCalenEmptyRightColumnWidth = 6.0;

/*!
 The minimum width [un] of touchable event bubble (UI spec)
 */
const qreal KCalenMinTouchableEventWidth = 8.0;

/*!
 The minimum width [un] of event bubble (UI spec)
 */
const qreal KCalenMinEventWidth = 3.0;

/*!
 The width [un] of time strip (indicator) in event bubble (UI spec)
 */
const qreal KCalenTimeStripWidth = 1.5;

/*!
 The minimum width [un] of time strip (indicator) in event bubble (UI spec)
 */
const qreal KCalenMinTimeStripWidth = 1.4;

/*!
 The width [un] of space beetwen event bubbles in Day View (UI spec)
 */
const qreal KCalenSpaceBeetwenEvents = 1.5;

/*!
 The minimum width [un] of space beetwen event bubbles in Day View (UI spec)
 */
const qreal KCalenMinSpaceBeetwenEvents = 0.1;

/*!
 Number of All day events displayed in single day that remains touchable 
 in portrait orientation (UI spec)
 */
const int KCalenTouchableAllDayEventsCountPortrait = 1;

/*!
 Number of All day events displayed in single day that remains touchable 
 in landscape orientation (UI spec)
 */
const int KCalenTouchableAllDayEventsCountLandscape = 2;

/*!
 Thickness [un] of hour line (UI spec)
 */
const qreal KCalenHourLineThickness = 0.15;

/*!
 Thickness [un] of current time line (UI spec)
 */
const qreal KCalenCurrentTimeLineThickness = 0.75;

/*!
 Width [un] of dashes in half-hour line (UI spec)
 */
const qreal KCalenHalfHourLineDashWidth = 1.5;

/*!
 Color scheme for hour and half-hour lines (UI spec)
 */
const QString KCalenHourLineColor = "qtc_cal_day_hour_lines";

/*!
 Color scheme for current time line (UI spec)
 */
const QString KCalenTimeLineColor = "qtc_cal_month_current_day";

/*!
 Color scheme for Day View background in portrait mode (UI spec)
 */
const QString KCalenBackgroundColorPortrait = "qtg_graf_cal_bg_prt";

/*!
 Color scheme for Day View background in landscape mode (UI spec)
 */
const QString KCalenBackgroundColorLandscape = "qtg_graf_cal_bg_lsc";


// Enumerations

/*!
 \enum CalenScrollDirection
 \brief Enumeration identifies horizontal scroll directions of scroll area.
 
 ECalenScrollNoDayChange means that move does not change the current day.
 */
enum CalenScrollDirection
{
    ECalenScrollNoDayChange = 0,
    ECalenScrollToNext,
    ECalenScrollToPrev
};

#endif /* CALENDAYCOMMONHEADERS_H_ */
