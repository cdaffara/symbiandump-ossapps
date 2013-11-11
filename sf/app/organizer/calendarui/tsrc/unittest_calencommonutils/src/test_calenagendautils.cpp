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
#include <agendaentry.h>

#include "calenagendautils.h"
#include "test_calenagendautils.h"

/*!
	\class TestCalenAgendaUtils

	This is unittest class for CalenAgendaUtils apis.
 */
 
TestCalenAgendaUtils::TestCalenAgendaUtils()
{
    // No implementation yet
}

TestCalenAgendaUtils::~TestCalenAgendaUtils()
{
    // No implementation yet
}

/*!
	This function is called to before every testcase.
 */
void TestCalenAgendaUtils::init()
{
    // No implementation yet
}

/*!
	This function is called after every testcase.
 */
void TestCalenAgendaUtils::cleanup()
{
    // No implementation yet
}

/*!
    This function is to test the api of CalenDateUtils::endsAtStartOfDayL.
 */
void TestCalenAgendaUtils::test_endsAtStartOfDay()
{
	AgendaEntry entry;
	entry.setSummary("summary");
	entry.setStartAndEndTime(QDateTime(QDate(2010, 5, 1), QTime(12, 30)),
	                         QDateTime(QDate(2010, 5, 6), QTime(2, 59)));
	bool outPut = CalenAgendaUtils::endsAtStartOfDay(
												entry, 
												QDateTime(QDate(2010, 5, 6)));
	QVERIFY(!outPut);

	entry.setStartAndEndTime(QDateTime(QDate(2010, 5, 1), QTime(1, 35)),
	                         QDateTime(QDate(2010, 5, 6), QTime(23, 59)));
	outPut = CalenAgendaUtils::endsAtStartOfDay(
												entry, 
												QDateTime(QDate(2010, 5, 6)));
	QVERIFY(!outPut);
	
	entry.setStartAndEndTime(QDateTime(QDate(2010, 5, 1), QTime(2, 30)),
		                         QDateTime(QDate(2010, 5, 5), QTime(23, 59, 59, 999)));
	outPut = CalenAgendaUtils::endsAtStartOfDay(
													entry, 
													QDateTime(QDate(2010, 5, 6)));
	QVERIFY(!outPut);
	
	entry.setStartAndEndTime(QDateTime(QDate(2010, 5, 1), QTime(1, 35)),
	                         QDateTime(QDate(2010, 5, 6)));
	outPut = CalenAgendaUtils::endsAtStartOfDay(
												entry, 
												QDateTime(QDate(2010, 5, 6)));
	QVERIFY(outPut);

}
// End of file	--Don't remove this.
