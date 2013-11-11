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
 * Description: CalenLauncher unit test class.
 *
 */

// System includes
#include <QtTest/QtTest>
#include <QtGui>
#include <e32base.h>
#include <hbmainwindow.h>
#include <hbview.h>

// User includes
#include "calenlauncher.h"

/*!
	\class TestCalenLauncher
	
	This is unit test class for CalenLauncher apis.
 */
class TestCalenLauncher : public QObject
{
	Q_OBJECT
	
public slots:
	void launcherFailed(int error);
	
private slots:
	// Test case setup.
	void initTestCase(); 
	void cleanupTestCase();
	
	// Test cases.
	void test_launchCalendarApp();
	
private:
	CalenLauncher* mCalenLauncher;
};

/*!
	This function is called to before the first test case.
 */
void TestCalenLauncher::initTestCase()
{
	mCalenLauncher = new CalenLauncher();
	connect(mCalenLauncher, SIGNAL(calendarLaunchFailed(int)), this,
		        SLOT(launcherFailed(int)));
}

/*!
	This function is called after the last testcase.
 */
void TestCalenLauncher::cleanupTestCase()
{
	delete mCalenLauncher;
}

/*!
	Tests the api CalenLauncher::launchCalendarApp.
 */
void TestCalenLauncher::test_launchCalendarApp()
{
	QDate dateFocused(2015, 05, 05);
	QTime timeSet(0, 0, 0);
	QDateTime focusedDateTime(dateFocused, timeSet);
	// Call the API for launching calendar 
	mCalenLauncher->launchCalendarApp(CalenLauncher::MonthView, focusedDateTime);
}

/*!
	This slot is called on failure of calendar launch.
 */
void TestCalenLauncher::launcherFailed(int error)
{	
	Q_UNUSED(error)
	// Indicate launching failed.
	QVERIFY(true == false);
}

QTEST_MAIN(TestCalenLauncher)
#include "unittest_calenlauncher.moc"

// End of file	--Don't remove this.
