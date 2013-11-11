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
* Description:
*
*/

#ifndef __TEST_CALENDATEUTILS_H__
#define __TEST_CALENDATEUTILS_H__

#include <QObject>

class TestCalenDateUtils : public QObject
{
	Q_OBJECT
public:
    TestCalenDateUtils();
    ~TestCalenDateUtils();
    
private slots:
    void init();
    void cleanup();
    
    void test_onSameDay();
    void test_onSameMonth();
    void test_beginningOfDay();
    void test_displayTimeOnDay();
    void test_timeRangesIntersect();
    void test_isValidDay();
    void test_limitToValidTime();
    void test_maxTime();
    void test_minTime();
    void test_timeOfDay();
    void test_roundToPreviousHour();
    void test_roundToPreviousHourFromMinutes();
    void test_now();
    void test_today();
    void test_isOnToday();
    void test_defaultTime();
    void test_futureOf();
};

#endif      // __TEST_CALENDATEUTILS_H__

// End of file
