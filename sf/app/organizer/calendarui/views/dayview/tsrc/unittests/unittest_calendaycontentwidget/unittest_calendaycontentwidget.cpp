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

#define private public

#include "calendayitemview.h"
#include "calendaycontentwidget.h"

class TestCalenContentWidget : public QObject
{
Q_OBJECT

public:
    TestCalenContentWidget();
    virtual ~TestCalenContentWidget();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testAdd();
    void testTake();
    void testRemove();
    void testRelayoutWidgets();
    void testWidgetScrolled(); 

private:
    CalenDayContentWidget *mContentWidget;
};

/*!
 Constructor
 */
TestCalenContentWidget::TestCalenContentWidget() :
    mContentWidget(NULL)
{

}

/*!
 Destructor
 */
TestCalenContentWidget::~TestCalenContentWidget()
{

}

/*!
 Called before testcase
 */
void TestCalenContentWidget::initTestCase()
{
}

/*!
 Called after testcase
 */
void TestCalenContentWidget::cleanupTestCase()
{
}

/*!
 Called before every function
 */
void TestCalenContentWidget::init()
{
    mContentWidget = new CalenDayContentWidget();
}

/*!
 Called after everyfunction
 */
void TestCalenContentWidget::cleanup()
{
    if (mContentWidget) {
        delete mContentWidget;
        mContentWidget = NULL;
    }
}

/*!
 Test class for constructors
 1. Test if content widget is correcty created
 */
void TestCalenContentWidget::testConstructors()
{
    CalenDayContentWidget *contentWidget = NULL;
    
    QVERIFY(!contentWidget);
    contentWidget = new CalenDayContentWidget();
    QVERIFY(contentWidget);
    
    delete contentWidget;
}

/*!
 Test add() function
 1. Test no error when trying to add NULL pointer
 2. Test if widget is added correctly and layout is created
 3. Test if widget is added correctly at end of list
 4. Test if widget is added correctly as first item
 5. Test if widget is added correctly when no position is specified
 */
void TestCalenContentWidget::testAdd()
{
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
    QVERIFY(!mContentWidget->mLayout);
    
    //1)
    mContentWidget->add(NULL);
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
    QVERIFY(!mContentWidget->mLayout);
    
    //2)
    HbWidget *second = new HbWidget();
    mContentWidget->add(second);
    QVERIFY(mContentWidget->mLayout);
    QCOMPARE(mContentWidget->mWidgets.count(), 1);
    
    //3)
    HbWidget *last = new HbWidget();
    mContentWidget->add(last, CalenDayContentWidget::ECalenLastWidget);
    QCOMPARE(mContentWidget->mWidgets.count(), 2);
    QCOMPARE(mContentWidget->mWidgets.at(1), last);
    
    //4)
    HbWidget *first = new HbWidget();
    mContentWidget->add(first, CalenDayContentWidget::ECalenFirstWidget);
    QCOMPARE(mContentWidget->mWidgets.count(), 3);
    QCOMPARE(mContentWidget->mWidgets.at(0), first);
    
    //5)
    HbWidget *other = new HbWidget();
    mContentWidget->add(other, CalenDayContentWidget::CalenWidgetPosition(-1));
    QCOMPARE(mContentWidget->mWidgets.count(), 4);
    QCOMPARE(mContentWidget->mWidgets.last(), other);
}

/*!
 Test take() function
 1. Test NULL pointer if there are no widgets to be taken
 2. Test if first widget is taken correctly
 3. Test if last widget is taken correctly
 4. Test default behavior if wrong position is specified
 */
void TestCalenContentWidget::testTake()
{
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
    
    //1)
    QVERIFY(!mContentWidget->take(CalenDayContentWidget::ECalenFirstWidget));
    
    // Prepare data
    HbWidget *first = new HbWidget();
    HbWidget *second = new HbWidget();
    HbWidget *last = new HbWidget();
    mContentWidget->add(first);
    mContentWidget->add(second);
    mContentWidget->add(last);
    QCOMPARE(mContentWidget->mWidgets.count(), 3);
        
    //2)
    HbWidget *widget = mContentWidget->take(CalenDayContentWidget::ECalenFirstWidget);
    QCOMPARE(mContentWidget->mWidgets.count(), 2);
    QCOMPARE(widget, first);
    
    delete widget;
    widget = NULL;
    
    //3)
    widget = mContentWidget->take(CalenDayContentWidget::ECalenLastWidget);
    QCOMPARE(mContentWidget->mWidgets.count(), 1);
    QCOMPARE(widget, last);
        
    delete widget;
    widget = NULL;
    
    //4)
    widget = mContentWidget->take(CalenDayContentWidget::CalenWidgetPosition(-1));
    QCOMPARE(mContentWidget->mWidgets.count(), 1);
    QVERIFY(!widget);
}

/*!
 Test remove() function
 1. Test if there is no error if remove is called on empty content
 2. Test if first widget is removed correctly
 3. Test if last widget is removed correctly
 */
void TestCalenContentWidget::testRemove()
{
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
        
    //1)
    mContentWidget->remove(CalenDayContentWidget::ECalenFirstWidget);
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
    
    // Prepare data
    HbWidget *first = new HbWidget();
    HbWidget *second = new HbWidget();
    HbWidget *last = new HbWidget();
    mContentWidget->add(first);
    mContentWidget->add(second);
    mContentWidget->add(last);
    QCOMPARE(mContentWidget->mWidgets.count(), 3);
        
    //2)
    mContentWidget->remove(CalenDayContentWidget::ECalenFirstWidget);
    QCOMPARE(mContentWidget->mWidgets.count(), 2);
    QCOMPARE(mContentWidget->mWidgets.first(), second);
    
    //3)
    mContentWidget->remove(CalenDayContentWidget::ECalenLastWidget);
    QCOMPARE(mContentWidget->mWidgets.count(), 1);
    QCOMPARE(mContentWidget->mWidgets.last(), second);
}

/*!
 Test relayoutWidgets() slot
 1. Test if relayout widgets on empty content
 2. Test ECalenScrollToNext case
 3. Test ECalenScrollToPrev case
 4. Test ECalenScrollNoDayChange case
 5. Test wrong case
 */
void TestCalenContentWidget::testRelayoutWidgets()
{
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
    
    //1)
    mContentWidget->relayoutWidgets(ECalenScrollNoDayChange);
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
    
    // Prepare data
    HbWidget *first = new HbWidget();
    HbWidget *second = new HbWidget();
    HbWidget *last = new HbWidget();
    mContentWidget->add(first);
    mContentWidget->add(second);
    mContentWidget->add(last);
    QCOMPARE(mContentWidget->mWidgets.count(), 3);
    
    //2)
    mContentWidget->relayoutWidgets(ECalenScrollToNext);
    QCOMPARE(mContentWidget->mWidgets.first(), second);
    
    //3)
    mContentWidget->relayoutWidgets(ECalenScrollToPrev);
    QCOMPARE(mContentWidget->mWidgets.first(), first);
    
    //4)
    mContentWidget->relayoutWidgets(ECalenScrollNoDayChange);
    QCOMPARE(mContentWidget->mWidgets.first(), first);
    
    //5)
    mContentWidget->relayoutWidgets(CalenScrollDirection(-1));
    QCOMPARE(mContentWidget->mWidgets.first(), first);
}

/*!
 Test testWidgetScrolled() slot
 1. Test widgetScrolled with empty content
 2. Test widgetScrolled with 3 views
 */
void TestCalenContentWidget::testWidgetScrolled() 
{
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
    QPointF newPos(10, 20);
    
    //1)
    mContentWidget->widgetScrolled(newPos);
    QCOMPARE(mContentWidget->mWidgets.count(), 0);
    
#ifndef __WINSCW__
    // Prepare data
    MCalenServices services;
    HbModelIterator iterator(0);
    CalenDayItemView *first = new CalenDayItemView(services, &iterator);
    CalenDayItemView *second = new CalenDayItemView(services, &iterator);
    CalenDayItemView *last = new CalenDayItemView(services, &iterator);
    mContentWidget->add(first);
    mContentWidget->add(second);
    mContentWidget->add(last);
    QCOMPARE(mContentWidget->mWidgets.count(), 3);
    QCOMPARE(first->mCurrentPos, QPointF(0,0));
    QCOMPARE(second->mCurrentPos, QPointF(0,0));
    QCOMPARE(last->mCurrentPos, QPointF(0,0));
    
    //2
    mContentWidget->widgetScrolled(newPos);
    CalenDayItemView *view = static_cast<CalenDayItemView *> (mContentWidget->mWidgets.at(0));
    QCOMPARE(view->mCurrentPos, newPos);
    view = static_cast<CalenDayItemView *> (mContentWidget->mWidgets.at(1));
    QCOMPARE(view->mCurrentPos, newPos);
    view = static_cast<CalenDayItemView *> (mContentWidget->mWidgets.at(2));
    QCOMPARE(view->mCurrentPos, newPos);
#endif // __WINSCW__
}

QTEST_MAIN(TestCalenContentWidget);
#include "unittest_calendaycontentwidget.moc"
