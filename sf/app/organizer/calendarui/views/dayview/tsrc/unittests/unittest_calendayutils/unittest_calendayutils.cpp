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
 * Description: Test class for CalenDayContentWidget
 *
 */
#include <QGraphicsItem>
#include <QtTest/QtTest>

#include "hbstyle.h"
#include "calendayutils.h"
#include "hbdeviceprofile.h"
#include "calendaycommonheaders.h"
#include "agendaentry.h"

// Test variables
QRectF gTestWindowRect = QRectF(0, 0, 10, 20);
Qt::Orientation gTestOrientation = Qt::Horizontal;
qreal param_value = 10;
qreal unitInPix = 1;

class TestCalenDayUtils : public QObject
{
Q_OBJECT

public:
    TestCalenDayUtils();
    virtual ~TestCalenDayUtils();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testInstance();
    void testScreenWidth();
    void testHourElementWidth();
    void testHourElementHeight();
    void testContentWidth();
    void testMinEventHeight();
    void testMainWindow();
    void testGetEventValidStartEndTime();

    void testConstructors();
    void testOrientation();

private:
    CalenDayUtils *mUtils;
    qreal mUnit;
};

/*!
 Constructor
 */
TestCalenDayUtils::TestCalenDayUtils() :
    mUtils(NULL)
{
    HbDeviceProfile s;
    mUnit = s.unitValue();
}

/*!
 Destructor
 */
TestCalenDayUtils::~TestCalenDayUtils()
{

}

/*!
 Called before testcase
 */
void TestCalenDayUtils::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayUtils::cleanupTestCase()
{
}

/*!
 Called before every function
 */
void TestCalenDayUtils::init()
{
    mUtils = CalenDayUtils::instance();
}

/*!
 Called after everyfunction
 */
void TestCalenDayUtils::cleanup()
{
    //delete mUtils;
}

void TestCalenDayUtils::testInstance()
{
    CalenDayUtils* utils = CalenDayUtils::instance();
    QVERIFY(utils);
}

void TestCalenDayUtils::testScreenWidth()
{
    qreal sw = mUtils->screenWidth();
    QVERIFY(sw == gTestWindowRect.width());
}

void TestCalenDayUtils::testHourElementWidth()
{
    qreal hEW = mUtils->hourElementWidth();
    qreal myWidth = mUnit * 8.04 + param_value * 2;

    QCOMPARE(hEW, myWidth);

}

void TestCalenDayUtils::testHourElementHeight()
{

    qreal hEH = mUtils->hourElementHeight();
    qreal myHeight = mUnit * 4.1 + param_value * 2 + param_value * 2;

    QCOMPARE(hEH, myHeight);
}

void TestCalenDayUtils::testContentWidth()
{
    qreal cW = mUtils->contentWidth();
    qreal myWidth = gTestWindowRect.width() - (mUnit * 8.04 + param_value * 2);

    QCOMPARE(cW, myWidth);
}

/*!
 Test function for minimum event's height
 */
void TestCalenDayUtils::testMinEventHeight()
{
    qreal minH = mUtils->minEventHeight();
    qreal myH = param_value + param_value + param_value;
    
    QCOMPARE(minH, myH);
}

/*!
 Test function for main window getter
 */
void TestCalenDayUtils::testMainWindow()
{
    HbMainWindow *window = NULL;
    QVERIFY(!window);
    
    window = mUtils->mainWindow();
    QVERIFY(window);
}

/*!
 Test function for getEventValidStartEndTime function
 1) start and end time are in current day: event start - end
 2) start time is yesterday, end time is in current day: event 0.00 - end
 3) start time is yesterday, end time is tomorrow: event 0.00 - 23.59
 */
void TestCalenDayUtils::testGetEventValidStartEndTime()
{
    QDateTime currentDate(QDate(2010, 9, 5), QTime(8, 15));
    
    //1)
    QDateTime inStart = currentDate.addSecs(-3600);     // currentTime - 1 hour
    QDateTime inEnd = currentDate.addSecs(3600);        // currentTime + 1 hour
    
    AgendaEntry entry;
    entry.setStartAndEndTime(inStart, inEnd);
    
    QDateTime outStart;
    QDateTime outEnd;
    mUtils->getEventValidStartEndTime(outStart, outEnd, entry, currentDate);
    QCOMPARE(outStart, inStart);
    QCOMPARE(outEnd, inEnd);
    
    //2)
    inStart = currentDate.addDays(-1);      // currentTime - 1 day
    inEnd = currentDate.addSecs(3600);      // currentTime + 1 hour
    
    entry.setStartAndEndTime(inStart, inEnd);
    
    mUtils->getEventValidStartEndTime(outStart, outEnd, entry, currentDate);
    QCOMPARE(outStart, QDateTime(currentDate.date(), QTime(0, 0, 0, 0)));
    QCOMPARE(outEnd, inEnd);
    
    //3)
    inStart = currentDate.addDays(-1);      // currentTime - 1 day
    inEnd = currentDate.addDays(1);         // currentTime + 1 day
    
    entry.setStartAndEndTime(inStart, inEnd);
    
    mUtils->getEventValidStartEndTime(outStart, outEnd, entry, currentDate);
    QCOMPARE(outStart, QDateTime(currentDate.date(), QTime(0, 0, 0, 0)));
    QCOMPARE(outEnd, QDateTime(currentDate.date(), QTime(23, 59, 0, 0)));
}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenDayUtils::testConstructors()
{
    //1)
    CalenDayUtils *testUtils = 0;
    QVERIFY(!testUtils);

    //2)
    testUtils = CalenDayUtils::instance();
    QVERIFY(testUtils);
}

/*!
 Test function for getting main window's orientation
 1)test if horizontal orientation is returned
 2)test if vertical orientation is returned
 */
void TestCalenDayUtils::testOrientation()
{
    //1)
    gTestOrientation = Qt::Horizontal;
    QCOMPARE(CalenDayUtils::instance()->orientation(), Qt::Horizontal);

    //2)
    gTestOrientation = Qt::Vertical;
    QCOMPARE(CalenDayUtils::instance()->orientation(), Qt::Vertical);
}

QTEST_MAIN(TestCalenDayUtils);
#include "unittest_calendayutils.moc"
