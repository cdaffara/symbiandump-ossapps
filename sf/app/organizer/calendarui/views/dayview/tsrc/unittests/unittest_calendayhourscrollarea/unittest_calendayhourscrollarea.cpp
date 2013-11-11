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

#include <QPainter>
#include <QImage>
#include <QGraphicsScene>
#include <hbstyleloader.h>

#define private public

#include "calendayhourscrollarea.h"

// Test variables
bool gTestLocaleChanged = false;
bool gTestUpdatePerformed = false;
const qreal WIDGET_WIDTH = 300;
const qreal WIDGET_HEIGHT = 1000;

class TestCalenDayHourScrollArea : public QObject
{
Q_OBJECT

public:
    TestCalenDayHourScrollArea();
    virtual ~TestCalenDayHourScrollArea();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testSetGetDateTime();
    void testScrollToHour();
    void testscrollVertically();
    void testLocaleChanged();
    void testUpdateTimeIndicator();

private:
    CalenDayHourScrollArea *mHourScrollArea;
};

/*!
 Constructor
 */
TestCalenDayHourScrollArea::TestCalenDayHourScrollArea() :
   mHourScrollArea(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayHourScrollArea::~TestCalenDayHourScrollArea()
{

}

/*!
 Called before testcase
 */
void TestCalenDayHourScrollArea::initTestCase()
{
    HbStyleLoader::registerFilePath(":/calendayhourelement.css");
    HbStyleLoader::registerFilePath(":/calendayhourelement.widgetml");
    HbStyleLoader::registerFilePath(":/calendayitem.css");
    HbStyleLoader::registerFilePath(":/calendayitem.widgetml");
    HbStyleLoader::registerFilePath(":/calendayeventspane.css");
}

/*!
 Called after testcase
 */
void TestCalenDayHourScrollArea::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayHourScrollArea::init()
{
    mHourScrollArea = new CalenDayHourScrollArea();
}

/*!
 Called after everyfunction
 */
void TestCalenDayHourScrollArea::cleanup()
{
    if (mHourScrollArea) {
        delete mHourScrollArea;
        mHourScrollArea = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if hour scroll area is not initialized
 2. Test if hour scroll area is correcty created
 */
void TestCalenDayHourScrollArea::testConstructors()
{
    //1)
    CalenDayHourScrollArea *testHourScrollArea = 0;
    QVERIFY(!testHourScrollArea);
    
    testHourScrollArea = new CalenDayHourScrollArea();
    
    QVERIFY(testHourScrollArea);
    
    delete testHourScrollArea;
}

/*!
   \brief It test seting and geting datetime.
   1. Test first setting datetime
   2. Test datetime after change 
 */
void TestCalenDayHourScrollArea::testSetGetDateTime()
{
    QDateTime testValue(QDate(2010,06,29),QTime(8,42,10));
    //1)
    mHourScrollArea->setDateTime(testValue);
    QCOMPARE(mHourScrollArea->dateTime(),testValue);
    
    //2)
    testValue = QDateTime(QDate(2011,7,30),QTime(9,43,11));
    mHourScrollArea->setDateTime(testValue);
    QCOMPARE(mHourScrollArea->dateTime(),testValue);
}

/*!
   \brief It test scrolling to given hour.
   To draw result on image uncomment SAVE_IMAGES in pro file
   1. Test scroll to 0h
   2. Test scroll to 12h
   3. Test scroll to 20h
 */
void TestCalenDayHourScrollArea::testScrollToHour()
{
    mHourScrollArea->resize(WIDGET_WIDTH,WIDGET_HEIGHT);
    QGraphicsScene scene;
    scene.addItem(mHourScrollArea);
    QDateTime testValue(QDate(2010,06,29),QTime(8,42,10));
    mHourScrollArea->setDateTime(testValue);
    //resize widget to be bigger than (0,0)
    QSize size = mHourScrollArea->size().toSize();
    
    //create image that will simulate widget where painting should be done
    QPixmap img(size);
    //create painter which will be used to paint
    QPainter painter;    
    
    //0
    mHourScrollArea->scrollToHour(0);
    painter.begin(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    //fill image with white color to have better filings with look of "paper"
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    scene.render(&painter);
    painter.end();
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourScrollArea_testScrollToHour_0.png");
#endif
    //1
    
    mHourScrollArea->scrollToHour(12);
    painter.begin(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    scene.render(&painter);
    painter.end();
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourScrollArea_testScrollToHour_12.png");
#endif    
    //2
    
    mHourScrollArea->scrollToHour(20);
    painter.begin(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    scene.render(&painter);
    painter.end();
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourScrollArea_testScrollToHour_20.png");
#endif
    scene.removeItem(mHourScrollArea);
}

/*!
   \brief It test scrolling to vertical position.
   To draw result on image uncomment SAVE_IMAGES in pro file
   1. Test scroll to QPoint(0,250)
   2. Test scroll to QPoint(0,500)
   3. Test scroll to QPoint(0,2000)
 */
void TestCalenDayHourScrollArea::testscrollVertically()
{
    mHourScrollArea->resize(WIDGET_WIDTH,WIDGET_HEIGHT);
    QGraphicsScene scene;
    scene.addItem(mHourScrollArea);
    QDateTime testValue(QDate(2010,06,29),QTime(8,42,10));
    mHourScrollArea->setDateTime(testValue);
    //resize widget to be bigger than (0,0)
    QSize size = mHourScrollArea->size().toSize();
    
    //create image that will simulate widget where painting should be done
    QPixmap img(size);
    //create painter which will be used to paint
    QPainter painter;

    //1
    mHourScrollArea->scrollVertically(QPoint(10,250));
    painter.begin(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    scene.render(&painter);
    painter.end();
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourScrollArea_testscrollVertically_250.png");
#endif
    //2
    mHourScrollArea->scrollVertically(QPoint(10,500));
    painter.begin(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    scene.render(&painter);
    painter.end();
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourScrollArea_testscrollVertically_500.png");
#endif    
    //3
    mHourScrollArea->scrollVertically(QPoint(10,2000));
    painter.begin(&img);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    scene.render(&painter);
    painter.end();
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourScrollArea_testscrollVertically_2000.png");
#endif
    scene.removeItem(mHourScrollArea);
}

/*!
   \brief It tests localeChanged slot
   1) test if localeChanged was called on hour elements
   2) test if nothing changes if there are no hour elements
 */
void TestCalenDayHourScrollArea::testLocaleChanged()
{
    //1)
    gTestLocaleChanged = false;
    mHourScrollArea->localeChanged();
    QCOMPARE(gTestLocaleChanged, true);
    
    //2)
    gTestLocaleChanged = false;
    // clear the list of elements
    mHourScrollArea->mHourElements.clear();
    QCOMPARE(mHourScrollArea->mHourElements.count(), 0);
    mHourScrollArea->localeChanged();
    QCOMPARE(gTestLocaleChanged, false);
}
    
/*!
   \brief It tests updateTimeIndicator slot
   1) hour elements > 0, update not performed for day before yesterday
   2) hour elements > 0, update performed for current day
   3) hour elements > 0, update not performed for day after tomorrow
   4) hour elements = 0, update not performed for day before yesterday
   5) hour elements = 0, update not performed for current day
   6) hour elements = 0, update not performed for day after tomorrow
 */
void TestCalenDayHourScrollArea::testUpdateTimeIndicator()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    
    //1)
    gTestUpdatePerformed = false;
    mHourScrollArea->setDateTime(currentDateTime.addDays(-2));
    mHourScrollArea->updateTimeIndicator();
    QCOMPARE(gTestUpdatePerformed, false);
    
    //2)
    gTestUpdatePerformed = false;
    mHourScrollArea->setDateTime(currentDateTime);
    mHourScrollArea->updateTimeIndicator();
    QCOMPARE(gTestUpdatePerformed, true);
    
    //3)
    gTestUpdatePerformed = false;
    mHourScrollArea->setDateTime(currentDateTime.addDays(2));
    mHourScrollArea->updateTimeIndicator();
    QCOMPARE(gTestUpdatePerformed, false);
    
    // clear the list of elements
    mHourScrollArea->mHourElements.clear();
    QCOMPARE(mHourScrollArea->mHourElements.count(), 0);
    
    //4)
    gTestUpdatePerformed = false;
    mHourScrollArea->setDateTime(currentDateTime.addDays(-2));
    mHourScrollArea->updateTimeIndicator();
    QCOMPARE(gTestUpdatePerformed, false);
    
    //5)
    gTestUpdatePerformed = false;
    mHourScrollArea->setDateTime(currentDateTime);
    mHourScrollArea->updateTimeIndicator();
    QCOMPARE(gTestUpdatePerformed, false);
    
    //6)
    gTestUpdatePerformed = false;
    mHourScrollArea->setDateTime(currentDateTime.addDays(2));
    mHourScrollArea->updateTimeIndicator();
    QCOMPARE(gTestUpdatePerformed, false);
}

QTEST_MAIN(TestCalenDayHourScrollArea);
#include "unittest_calendayhourscrollarea.moc"
