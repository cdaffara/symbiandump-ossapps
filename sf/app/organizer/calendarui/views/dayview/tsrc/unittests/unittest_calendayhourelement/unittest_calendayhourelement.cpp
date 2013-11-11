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
 * Description: Test class for TestCalenDayHourElement
 *
 */
#include <QGraphicsItem>
#include <QtTest/QtTest>

#ifndef __WINSCW__
#define private public
#define protected public
#endif

#include "calendayhourelementtest.h"
#include "calendayhourscrollarea.h"

#include <QPainter>
#include <QImage>
#include <QPair>
#include <QDateTime>

const qreal WIDGET_WIDTH = 50;
const qreal WIDGET_HEIGHT = 150;

class TestCalenDayHourElement : public QObject
{
Q_OBJECT

public:
    TestCalenDayHourElement();
    virtual ~TestCalenDayHourElement();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testSetGetTime();
    void testPaint_data();
    void testPaint();
    void testLocaleChanged();

private:
    CalenDayHourElementTest *mHourElement;
    CalenDayHourScrollArea  *mContainer;
    QTime   mTime;
};

/*!
 Constructor
 */
TestCalenDayHourElement::TestCalenDayHourElement() :
   mHourElement(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayHourElement::~TestCalenDayHourElement()
{

}

/*!
 Called before testcase
 */
void TestCalenDayHourElement::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenDayHourElement::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayHourElement::init()
{
    mTime = QTime(12,0,0);
    mContainer = new CalenDayHourScrollArea();
    mHourElement = new CalenDayHourElementTest(mTime,mContainer);
}

/*!
 Called after everyfunction
 */
void TestCalenDayHourElement::cleanup()
{
    if (mHourElement) {
        delete mHourElement;
        mHourElement = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if content widget is not initialized
 2. Test if content widget is correcty created
 */
void TestCalenDayHourElement::testConstructors()
{
    //1)
    CalenDayHourElementTest *testHourElement = 0;
    QVERIFY(!testHourElement);
    
    testHourElement = new CalenDayHourElementTest(mTime,mContainer);
    
    QVERIFY(testHourElement);
    
    delete testHourElement;
}

/*!
   \brief It test is setting and getting of time works good.
   
   0)Test if time from constructor works good
   1)Test set new time
   2)Change time and test new
 */
void TestCalenDayHourElement::testSetGetTime()
{
    //0)
    QVERIFY(mTime == mHourElement->time());
    
    QTime testValue(15,15,15);
    //1)
    mHourElement->setTime(testValue);
    
    QVERIFY(testValue == mHourElement->time());
    
    //2)
    testValue = QTime(10,10,10);
    
    mHourElement->setTime(testValue);
    QVERIFY(testValue == mHourElement->time());
}

/*!
   \brief Test data for \sa testPaint
   
   Tested data:
   1)Hour is eual 0
   2)Hour is bigger than 0
   3)Test current time
 */
void TestCalenDayHourElement::testPaint_data()
{
    QTest::addColumn<QDateTime>("testedValue");
    QTest::addColumn<QString>("testName");
    
    QTest::newRow("hour = 0") << QDateTime(QDate(2010,06,26),QTime(0,10,0)) 
                              << QString("hour = 0"); 
    QTest::newRow("hour != 0") << QDateTime(QDate(2010,06,26),QTime(15,15,15)) 
                               << QString("hour != 0");
    QTest::newRow("current time") << QDateTime::currentDateTime() 
                                  << QString("current time");
}

/*!
   \brief Test painting method
   
   It check painting based on given time.
   \sa testPaint_data
 */
void TestCalenDayHourElement::testPaint()
{
    //1)
    //get data to test
    QFETCH(QDateTime, testedValue); 
    QFETCH(QString, testName); 
    
    mHourElement->setTime(testedValue.time());
    
    //set date to check
    mContainer->setDateTime(testedValue);
    
    QStyleOptionGraphicsItem * option = new QStyleOptionGraphicsItem();
    //preapre drawed area smaller because of drawing lines on corners
    option->rect = QRect(10,10,WIDGET_WIDTH-20,WIDGET_HEIGHT/2);
    mHourElement->resize(WIDGET_WIDTH,WIDGET_HEIGHT);
    QSize size = mHourElement->size().toSize();
     
    //create image that will simulate widget where painting should be done
    QImage img(size,QImage::Format_RGB32);
    //create painter which will be used to paint
    QPainter painter(&img);
    //fill image with grey color to have better filings with look of "paper"
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    //run paint
    mHourElement->paint(&painter,option,0);
     
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourElement_testPaint_" + testName + ".jpg");   
#endif
    
    //2)
    QDateTime currentDateTime = QDateTime::currentDateTime();
    int currentHour = currentDateTime.time().hour();
    mHourElement->setTime(QTime(currentHour + 1, 0 ));
    //fill image with grey color to have better filings with look of "paper"
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    //run paint
    mHourElement->paint(&painter,option,0);

#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourElement_testPaint_" + testName + "2.jpg");
#endif

    //3)
#ifndef __WINSCW__
    mHourElement->mContainer = NULL;    
    //fill image with grey color to have better filings with look of "paper"
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::gray));
    //run paint
    mHourElement->paint(&painter,option,0);
    
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenDayHourElement_testPaint_" + testName + "3.jpg");
#endif

#endif
}

/*!
 Test function for localChanged method
 */
void TestCalenDayHourElement::testLocaleChanged() 
{
#ifndef __WINSCW__
    mHourElement->mTimeTextItem->setText(QString(""));
    mHourElement->localeChanged();
    QVERIFY(mHourElement->mTimeTextItem->text() != QString(""));
#endif
}

QTEST_MAIN(TestCalenDayHourElement);
#include "unittest_calendayhourelement.moc"
