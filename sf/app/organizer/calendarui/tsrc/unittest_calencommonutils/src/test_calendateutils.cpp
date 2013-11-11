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

#include <QtTest/QtTest>
#include <QDateTime>

#include "calendateutils.h"
#include "test_calendateutils.h"

/*!
	\class TestCalenDateUtils

	This is unittest class for CalenDateUtils apis.
 */
 
TestCalenDateUtils::TestCalenDateUtils()
{
    // No implementation yet
}

TestCalenDateUtils::~TestCalenDateUtils()
{
    // No implementation yet
}

/*!
	This function is called to before every testcase.
 */
void TestCalenDateUtils::init()
{
    // No implementation yet
}

/*!
	This function is called after every testcase.
 */
void TestCalenDateUtils::cleanup()
{
    // No implementation yet
}

/*!
    This function is to test the api of CalenDateUtils::onSameDay.
 */
void TestCalenDateUtils::test_onSameDay()
{
	QDateTime firstParam(QDate(2010, 10, 9));
	QDateTime secondParam(QDate(2009, 22, 9));
	QCOMPARE(CalenDateUtils::onSameDay(firstParam, secondParam), false);

	firstParam = QDateTime(QDate(2010, 2, 30));
	secondParam = QDateTime::currentDateTime();
	QCOMPARE(CalenDateUtils::onSameDay(firstParam, secondParam), false);

	firstParam = QDateTime(QDate(2010, 12, 5), QTime(6, 44));
	secondParam = QDateTime(QDate(2010, 12, 5), QTime(9, 33));
	QCOMPARE(CalenDateUtils::onSameDay(firstParam, secondParam), true);

	firstParam = QDateTime(QDate(-1756, 12, 5));
	secondParam = QDateTime(QDate(1900, 5, 9));
	QCOMPARE(CalenDateUtils::onSameDay(firstParam, secondParam), false);

}

/*!
    This function is to test the api of CalenDateUtils::onSameMonth.
 */
void TestCalenDateUtils::test_onSameMonth()
{
	QDateTime firstParam(QDate(2010, 10, 9));
	QDateTime secondParam(QDate(2010, 22, 9));
	QCOMPARE(CalenDateUtils::onSameMonth(firstParam, secondParam), false);

	firstParam = QDateTime(QDate(2010, 2, 13));
	secondParam = QDateTime(QDate(2009, 2, 3));
	QCOMPARE(CalenDateUtils::onSameMonth(firstParam, secondParam), false);

	firstParam = QDateTime(QDate(2010, 12, 5));
	secondParam = QDateTime(QDate(2010, 12, 6));
	QCOMPARE(CalenDateUtils::onSameMonth(firstParam, secondParam), true);

	firstParam = QDateTime(QDate(2010, 13, 5));
	secondParam = QDateTime(QDate(2010, 12, 9));
	QCOMPARE(CalenDateUtils::onSameMonth(firstParam, secondParam), false);
}

/*!
    This function is to test the api of CalenDateUtils::beginningOfDay.
 */
void TestCalenDateUtils::test_beginningOfDay()
{
	QDateTime expectedDate(QDate(2010, 5, 4), QTime(0, 0));
	QDateTime param(QDate(2010, 5, 4), QTime(15, 44));
	QCOMPARE(CalenDateUtils::beginningOfDay(param), expectedDate);

	expectedDate = QDateTime(QDate(2010, 3, 3), QTime(0, 0));
	param = QDateTime(QDate(2010, 3, 3), QTime(2, 33));
	QCOMPARE(CalenDateUtils::beginningOfDay(param), expectedDate);

	expectedDate = QDateTime(QDate(2010, 3, 5));
	param = QDateTime(QDate(2010, 3, 5), QTime(23, 59, 59, 999));
	QCOMPARE(CalenDateUtils::beginningOfDay(param), expectedDate);

	expectedDate = QDateTime(QDate(2010, 12, 15), QTime(0, 0));
	param = QDateTime(QDate(2010, 12, 15), QTime(24, 59));
	QCOMPARE(CalenDateUtils::beginningOfDay(param), expectedDate);
}

/*!
    This function is to test the api of CalenDateUtils::displayTimeOnDay.
 */
void TestCalenDateUtils::test_displayTimeOnDay()
{
	QDateTime firstParam(QDate(2010, 10, 9), QTime(12, 45));
	QDateTime secondParam(QDate(2010, 10, 9));
	QDateTime actualOutput;
	QDateTime expectedOutput = firstParam;
	
	actualOutput = CalenDateUtils::displayTimeOnDay(firstParam, secondParam);
	QCOMPARE(actualOutput, expectedOutput);

	firstParam = QDateTime(QDate(2010, 2, 13));
	secondParam = QDateTime(QDate(2009, 2, 3), QTime(23, 59));
	actualOutput = QDateTime();
	expectedOutput = CalenDateUtils::beginningOfDay(secondParam);
	
	actualOutput = CalenDateUtils::displayTimeOnDay(firstParam, secondParam);
	QCOMPARE(actualOutput, expectedOutput);

	firstParam = QDateTime(QDate(2010, 1, 5));
	secondParam = QDateTime(QDate(2010, 13, 6));
	actualOutput = QDateTime();
	
	actualOutput = CalenDateUtils::displayTimeOnDay(firstParam, secondParam);
	QVERIFY(!actualOutput.isValid());

}

/*!
    This function is to test the api of CalenDateUtils::displayTimeOnDay.
 */
void TestCalenDateUtils::test_timeRangesIntersect()
{
	QDateTime firstStartTime(QDate(2010, 10, 9), QTime(12, 45));
	QDateTime firstEndTime(QDate(2010, 10, 10));
	QDateTime secondStartTime(QDate(2010, 6, 5));
	QDateTime secondEndTime(QDate(2010, 10, 9));

	bool actualOutput = CalenDateUtils::timeRangesIntersect(firstStartTime,
	                                                        firstEndTime,
	                                                        secondStartTime,
	                                                        secondEndTime);
	QCOMPARE(actualOutput, false);

	firstStartTime = QDateTime(QDate(2009, 5, 7));
	firstEndTime = QDateTime(QDate(2009, 5, 6));
	secondStartTime = QDateTime(QDate(2009, 5, 6));
	secondEndTime = QDateTime(QDate(2009, 5, 6));
	actualOutput = true;

	actualOutput = CalenDateUtils::timeRangesIntersect(firstStartTime,
	                                                   firstEndTime,
	                                                   secondStartTime,
	                                                   secondEndTime);
	QCOMPARE(actualOutput, false);

	firstStartTime = QDateTime(QDate(2009, 5, 7));
	firstEndTime = QDateTime(QDate(2009, 6, 11));
	secondStartTime = QDateTime(QDate(2009, 5, 7), QTime(2,0));
	secondEndTime = QDateTime(QDate(2009, 5, 8));
	actualOutput = false;

	actualOutput = CalenDateUtils::timeRangesIntersect(firstStartTime,
	                                                   firstEndTime,
	                                                   secondStartTime,
	                                                   secondEndTime);
	QCOMPARE(actualOutput, true);

}

/*!
    This function is to test the api of CalenDateUtils::isValidDay.
 */
void TestCalenDateUtils::test_isValidDay()
{
	QVERIFY(!CalenDateUtils::isValidDay(QDateTime(QDate(1899,12,28))));
	
	QVERIFY(CalenDateUtils::isValidDay(QDateTime(QDate(2100,12,31))));
	
	QVERIFY(CalenDateUtils::isValidDay(QDateTime(QDate(2100,12,30))));
	
	QVERIFY(!CalenDateUtils::isValidDay(QDateTime(QDate(2100,2,31))));

}

/*!
    This function is to test the api of CalenDateUtils::limitToValidTime.
 */
void TestCalenDateUtils::test_limitToValidTime()
{
	QDateTime dateTime(QDate(1899, 12, 28));

	QDateTime actualOutput = CalenDateUtils::limitToValidTime(dateTime);
	QCOMPARE(actualOutput , QDateTime(QDate(1900,1,1)));

	dateTime = QDateTime(QDate(2010, 10, 20));
	actualOutput = CalenDateUtils::limitToValidTime(dateTime);
	QCOMPARE(actualOutput, QDateTime(QDate(2010,10,20)));

	dateTime = QDateTime();
	actualOutput = CalenDateUtils::limitToValidTime(dateTime);
	QCOMPARE(actualOutput, QDateTime(QDate(1900,1,1)));

	dateTime = QDateTime(QDate(2101, 1, 1));
	actualOutput = CalenDateUtils::limitToValidTime(dateTime);
	QCOMPARE(actualOutput, QDateTime(QDate(2100,12,31)));

	dateTime = QDateTime(QDate(2100, 12, 31), QTime(12,55));
	actualOutput = CalenDateUtils::limitToValidTime(dateTime);
	QCOMPARE(actualOutput, QDateTime(QDate(2100,12,31)));
	
	dateTime = QDateTime(QDate(2010,2,31));
	actualOutput = CalenDateUtils::limitToValidTime(dateTime);
	QCOMPARE(actualOutput, QDateTime(QDate(1900,1,1)));

}

/*!
    This function is to test the api of CalenDateUtils::maxTime.
 */
void TestCalenDateUtils::test_maxTime()
{
	QCOMPARE(CalenDateUtils::maxTime() , QDateTime(QDate(2100,12,31)));
}

/*!
    This function is to test the api of CalenDateUtils::minTime.
 */
void TestCalenDateUtils::test_minTime()
{
	QCOMPARE(CalenDateUtils::minTime() , QDateTime(QDate(1900,1,1)));
}

/*!
    This function is to test the api of CalenDateUtils::timeOfDay.
 */
void TestCalenDateUtils::test_timeOfDay()
{
	QDateTime dateTime = QDateTime(QDate(1900, 1, 1));
	int actulOutput = CalenDateUtils::timeOfDay(dateTime);
	int expectedOutput = 0;
	QCOMPARE(actulOutput, expectedOutput);

	dateTime = QDateTime(QDate(2010, 12, 8), QTime(12, 30));
	expectedOutput = 750;
	actulOutput = CalenDateUtils::timeOfDay(dateTime);
	QCOMPARE(actulOutput, expectedOutput);

	dateTime = QDateTime(QDate(2010, 12, 8), QTime(6, 59, 59, 999));
	expectedOutput = 420;
	actulOutput = CalenDateUtils::timeOfDay(dateTime);
	QCOMPARE(actulOutput, expectedOutput);

	dateTime = QDateTime(QDate(2010, 4, 31), QTime(7, 0));
	expectedOutput = 420;
	actulOutput = CalenDateUtils::timeOfDay(dateTime);
	QCOMPARE(actulOutput, expectedOutput);

	dateTime = QDateTime(QDate(2010, 4, 7), QTime(6, 59, 59));
	expectedOutput = 419;
	actulOutput = CalenDateUtils::timeOfDay(dateTime);
	QCOMPARE(actulOutput, expectedOutput);

	dateTime = QDateTime(QDate(2010, 4, 7), QTime(12, 60, 60));
	expectedOutput = 0;
	actulOutput = CalenDateUtils::timeOfDay(dateTime);
	QCOMPARE(actulOutput, expectedOutput);
}

/*!
    This function is to test the api of CalenDateUtils::roundToPreviousHour.
 */
void TestCalenDateUtils::test_roundToPreviousHour()
{
	QDateTime dateTime = QDateTime(QDate(1900, 1, 1), QTime(23, 59, 59));
	QDateTime actulOutput = CalenDateUtils::roundToPreviousHour(dateTime);
	QDateTime expectedOutput(QDate(1900, 1, 1), QTime(23, 0));
	QCOMPARE(actulOutput, expectedOutput);

	dateTime = QDateTime(QDate(2010, 12, 8), QTime(12, 30));
	expectedOutput = QDateTime(QDate(2010, 12, 8), QTime(12, 0));
	actulOutput = CalenDateUtils::roundToPreviousHour(dateTime);
	QCOMPARE(actulOutput, expectedOutput);

	dateTime = QDateTime(QDate(2010, 12, 8), QTime(6, 59, 59, 999));
	expectedOutput = QDateTime(QDate(2010, 12, 8), QTime(6, 0));
	actulOutput = CalenDateUtils::roundToPreviousHour(dateTime);
	QCOMPARE(actulOutput, expectedOutput);

	dateTime = QDateTime(QDate(2010, 4, 3), QTime(12, 60, 60));
	expectedOutput = QDateTime(QDate(2010, 4, 3));
	actulOutput = CalenDateUtils::roundToPreviousHour(dateTime);
	QCOMPARE(actulOutput, expectedOutput);
}

/*!
    This function is to test the api of CalenDateUtils::roundToPreviousHour.
 */
void TestCalenDateUtils::test_roundToPreviousHourFromMinutes()
{
	int minutes = 145;
	int actulOutput = CalenDateUtils::roundToPreviousHour(minutes);
	int expectedOutput = 120;
	QCOMPARE(actulOutput, expectedOutput);

	minutes = -255;
	expectedOutput = -240;
	actulOutput = CalenDateUtils::roundToPreviousHour(minutes);
	QCOMPARE(actulOutput, expectedOutput);

	minutes = 606;
	expectedOutput = 600;
	actulOutput = CalenDateUtils::roundToPreviousHour(minutes);
	QCOMPARE(actulOutput, expectedOutput);
}

/*!
    This function is to test the api of CalenDateUtils::now.
 */
void TestCalenDateUtils::test_now()
{
	QCOMPARE(CalenDateUtils::now() , QDateTime::currentDateTime());
}

/*!
    This function is to test the api of CalenDateUtils::today.
 */
void TestCalenDateUtils::test_today()
{
	QDateTime expectedTime = QDateTime::currentDateTime();
	expectedTime.setTime(QTime(0,0));
	QCOMPARE(CalenDateUtils::today() , expectedTime );
}

/*!
    This function is to test the api of CalenDateUtils::isOnToday.
 */
void TestCalenDateUtils::test_isOnToday()
{
	QVERIFY(CalenDateUtils::isOnToday(QDateTime::currentDateTime()));
	QDateTime dateTime = QDateTime::currentDateTime().addDays(1);
	QVERIFY(!CalenDateUtils::isOnToday(dateTime));
}

/*!
    This function is to test the api of CalenDateUtils::defaultTime.
 */
void TestCalenDateUtils::test_defaultTime()
{
	QDateTime dateTime = QDateTime(QDate(2010, 3, 2));
	QDateTime expectedOutput = dateTime;
	// 8:00 am is default time for Calendar Editor
	expectedOutput.setTime(QTime(8, 0));
	QDateTime actualOutput = CalenDateUtils::defaultTime(dateTime);
	QCOMPARE(actualOutput, expectedOutput);
}

/*!
    This function is to test the api of CalenDateUtils::futureOf.
 */
void TestCalenDateUtils::test_futureOf()
{
	QDateTime dateTime = QDateTime(QDate(2009, 10, 30));
	QDateTime expectedOutput = QDateTime(QDate(2009, 11, 26));
	QDateTime actualOutput = CalenDateUtils::futureOf(dateTime, 27);
	QCOMPARE(actualOutput, expectedOutput);
}

// End of file	--Don't remove this.
