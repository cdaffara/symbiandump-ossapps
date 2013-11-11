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
#include <QPen>

#include <QPainter>
#include <QImage>
#include <QGraphicsScene>
#include <hbstyleloader.h>

#include "calendayeventspane.h"

const qreal WIDGET_WIDTH = 300;
const qreal WIDGET_HEIGHT = 1000;

class TestCalenEventsPane : public QObject
{
Q_OBJECT

public:
    TestCalenEventsPane();
    virtual ~TestCalenEventsPane();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testSetGetTopLineDrawing();
    void testContentDrawing();

private:
    CalenDayEventsPane *mEventsPane;
};

/*!
 Constructor
 */
TestCalenEventsPane::TestCalenEventsPane() :
   mEventsPane(NULL)
{

}

/*!
 Destructor
 */
TestCalenEventsPane::~TestCalenEventsPane()
{

}

/*!
 Called before testcase
 */
void TestCalenEventsPane::initTestCase()
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
void TestCalenEventsPane::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenEventsPane::init()
{
    mEventsPane = new CalenDayEventsPane();
}

/*!
 Called after everyfunction
 */
void TestCalenEventsPane::cleanup()
{
    if (mEventsPane) {
        delete mEventsPane;
        mEventsPane = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if events pane is not initialized
 2. Test if events pane is correcty created
 */
void TestCalenEventsPane::testConstructors()
{
    //1)
    CalenDayEventsPane *testEventsPane = 0;
    QVERIFY(!testEventsPane);
    
    //2)
    testEventsPane = new CalenDayEventsPane();
    QVERIFY(testEventsPane);
    delete testEventsPane;
}

/*!
   Test setting and getting info about top line drawing
   1. Test first set of value
   2. Test change value
 */
void TestCalenEventsPane::testSetGetTopLineDrawing()
{
    //1)
    mEventsPane->setDrawTopLine(true);
    QCOMPARE(mEventsPane->isTopLineDrawed(),true);
    //2)
    mEventsPane->setDrawTopLine(false);
    QCOMPARE(mEventsPane->isTopLineDrawed(),false);
}

/*!
   Test drawing content of widget
   1. Test without top line draw
   2. Test with top line draw
 */
void TestCalenEventsPane::testContentDrawing()
{
    mEventsPane->resize(WIDGET_WIDTH,WIDGET_HEIGHT);
    QGraphicsScene scene;
    scene.addItem(mEventsPane);
    
    //resize widget to be bigger than (0,0)
    QSize size = mEventsPane->size().toSize();
    QRect viewRectangle(0,10,size.width(),size.height()-20);
    //create image that will simulate widget where painting should be done
    QPixmap img(size);
    //create painter which will be used to paint
    QPainter painter;    
    
    //1)
    mEventsPane->setDrawTopLine(false);
    painter.begin(&img);
    painter.setViewport(viewRectangle);
    painter.setRenderHint(QPainter::Antialiasing);
    //fill image with white color to have better filings with look of "paper"
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::white));
    scene.render(&painter);
    painter.end();
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenEventsPane_testContentDrawing_false.png");
#endif
    
    //2)
    mEventsPane->setDrawTopLine(true);
    painter.begin(&img);
    painter.setViewport(viewRectangle);
    painter.setRenderHint(QPainter::Antialiasing);
    //fill image with white color to have better filings with look of "paper"
    painter.fillRect(0,0,size.width(),size.height(),QColor(Qt::white));
    scene.render(&painter);
    painter.end();
#ifdef SAVE_IMAGES
    //save drawed image
    img.save("c:/unittest/TestCalenEventsPane_testContentDrawing_true.png");
#endif  
   
    scene.removeItem(mEventsPane);
}

QTEST_MAIN(TestCalenEventsPane);
#include "unittest_calendayeventspane.moc"
