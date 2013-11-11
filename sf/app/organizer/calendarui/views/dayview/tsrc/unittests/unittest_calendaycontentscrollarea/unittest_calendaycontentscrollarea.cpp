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
 * Description: Test class for CalenDayContentScrollArea
 *
 */
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QtTest/QtTest>
#include <QGesture>

#include <HbStyleLoader>
#include <HbWidget>
#include <HbSwipeGesture>

#define private public
#define protected public

#include "calendaycontentscrollarea.h"

// Test variables
qreal gTestScrollAreaWidth = 300;
qreal gTestScrollAreaHeight = 200;
QRectF gTestWindowRect = QRectF(0, 0, gTestScrollAreaWidth, gTestScrollAreaHeight);
Qt::Orientation gTestOrientation = Qt::Vertical;
Qt::GestureState gTestGestureState = Qt::NoGesture;
qreal gTestSceneSwipeAngle = 0.0;
QSwipeGesture::SwipeDirection gTestSceneHorizontalDirection = QSwipeGesture::NoDirection;

class TestCalenDayContentScrollArea : public QObject
{
Q_OBJECT

public:
    TestCalenDayContentScrollArea();
    virtual ~TestCalenDayContentScrollArea();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void testConstructors();
    void testScrollToMiddleWidget();
    void testScrollByAmount();
    void testGestureEvent();
    void testEventFilter();
    void testEvent();
    void testMoveTo();
    void testHorizontalSwipe();
    void testMoveFinished();
    void testOrientationChanged();
    void testCheckPanDirection();
    void testSetGetDisallowedScrollDirection();

private:
    CalenDayContentScrollArea *mContentScrollArea;
};

/*!
 Constructor
 */
TestCalenDayContentScrollArea::TestCalenDayContentScrollArea() :
    mContentScrollArea(NULL)
{

}

/*!
 Destructor
 */
TestCalenDayContentScrollArea::~TestCalenDayContentScrollArea()
{

}

/*!
 Called before testcase
 */
void TestCalenDayContentScrollArea::initTestCase()
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
void TestCalenDayContentScrollArea::cleanupTestCase()
{

}

/*!
 Called before every function
 */
void TestCalenDayContentScrollArea::init()
{
    // Create and configure scroll area
    mContentScrollArea = new CalenDayContentScrollArea();
    mContentScrollArea->setScrollDirections(Qt::Horizontal | Qt::Vertical);
    mContentScrollArea->resize(gTestScrollAreaWidth, gTestScrollAreaHeight);
    
    // Create and set content widget
    qreal contentWidth = mContentScrollArea->mContentWidth;
    HbWidget *contentWidget = new HbWidget();
    contentWidget->resize(3*contentWidth, 2*gTestScrollAreaHeight);
    mContentScrollArea->setContentWidget(contentWidget);
}

/*!
 Called after everyfunction
 */
void TestCalenDayContentScrollArea::cleanup()
{
    if (mContentScrollArea) {
        delete mContentScrollArea;
        mContentScrollArea = NULL;
    }
}

/*!
 Test function for constructors
 1. Test if content scroll area is not initialized
 2. Test if content scroll area is correcty created
 */
void TestCalenDayContentScrollArea::testConstructors()
{
    //1)
    CalenDayContentScrollArea *testContentScrollArea = 0;
    QVERIFY(!testContentScrollArea);
    
    testContentScrollArea = new CalenDayContentScrollArea();
    QVERIFY(testContentScrollArea);
    
    delete testContentScrollArea;
}

/*!
 Test function for scrollToMiddleWidget
 1. Test if content widget is available
 2. Test initial position of content widget
 3. Test the position of content widget after scrolling
 */
void TestCalenDayContentScrollArea::testScrollToMiddleWidget()
{
    //1)
    QVERIFY(mContentScrollArea->contentWidget());
    
    //2)
    QPointF pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(pos.x(), abs(0.0));
    QCOMPARE(pos.y(), abs(0.0));
    
    //3)
    qreal contentWidth = mContentScrollArea->mContentWidth;
    mContentScrollArea->scrollToMiddleWidget();
    pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(abs(pos.x()), abs(contentWidth));
    QCOMPARE(abs(pos.y()), abs(0.0));
}

/*!
 Test function for scrollToMiddleWidget
 1. Test if content widget is available
 2. Test initial position of content widget
 3. Test the position of content widget after vertical gesture
 4. Test the position of content widget after horizontal gesture
 5. Test the position of content widget after non-specified gesture
 */
void TestCalenDayContentScrollArea::testScrollByAmount()
{
    //1)
    QVERIFY(mContentScrollArea->contentWidget());

    //2)
    QPointF pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(abs(pos.x()), abs(0.0));
    QCOMPARE(abs(pos.y()), abs(0.0));

    //3)
    QPointF delta(50.0, 25.0);
    mContentScrollArea->mPanDayDirection
        = CalenDayContentScrollArea::ECalenPanVertical;
    mContentScrollArea->scrollByAmount(delta);
    pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(abs(pos.x()), abs(0.0));
    QCOMPARE(abs(pos.y()), abs(delta.y()));
    
    //4)
    mContentScrollArea->mPanDayDirection
        = CalenDayContentScrollArea::ECalenPanHorizontal;
    mContentScrollArea->scrollByAmount(delta);
    pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(abs(pos.x()), abs(delta.x()));
    QCOMPARE(abs(pos.y()), abs(delta.y()));
    
    //5)
    QPointF newDelta(5.0, 5.0);
    delta += newDelta;
    mContentScrollArea->mPanDayDirection
        = CalenDayContentScrollArea::ECalenPanNotSet;
    mContentScrollArea->scrollByAmount(newDelta);
    pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(abs(pos.x()), abs(delta.x()));
    QCOMPARE(abs(pos.y()), abs(delta.y()));
}

/*!
 Test function for gestureEvent
 1. Call gestureEvent with QPanGesture (nothing should happen)
 2. Call gestureEvent with HbSwipeGesture, vertical movement
 KCalenSwipeAngle < angle < 180 - KCalenSwipeAngle
 3. Call gestureEvent with HbSwipeGesture, horizontal movement right
 angle < KCalenSwipeAngle
 4. Call gestureEvent with HbSwipeGesture, horizontal movement left
 180.0 - KCalenSwipeAngle < angle < 180.0
 */
void TestCalenDayContentScrollArea::testGestureEvent()
{
    QGestureEvent *gestureEvent = NULL;
    mContentScrollArea->mMoveDirection = ECalenScrollNoDayChange;

    //1)
    QList<QGesture*> list;
    QPanGesture *panGesture = new QPanGesture();
    list.append(panGesture);
    gestureEvent = new QGestureEvent(list);
    mContentScrollArea->gestureEvent(gestureEvent);
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollNoDayChange);

    delete gestureEvent;
    gestureEvent = NULL;

    //2)
    HbSwipeGesture *swipeGesture = new HbSwipeGesture();
    list.append(swipeGesture);
    gTestGestureState = Qt::GestureStarted;
    gTestSceneSwipeAngle = KCalenSwipeAngle + 1.0;
    gestureEvent = new QGestureEvent(list);
    mContentScrollArea->gestureEvent(gestureEvent);
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollNoDayChange);

    //3)
    gTestGestureState = Qt::GestureStarted;
    gTestSceneSwipeAngle = KCalenSwipeAngle - 1.0;
    gTestSceneHorizontalDirection = QSwipeGesture::Right;
    mContentScrollArea->gestureEvent(gestureEvent);
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollToPrev);

    //4)
    gTestGestureState = Qt::GestureStarted;
    gTestSceneSwipeAngle = 180.0 - KCalenSwipeAngle + 1.0;
    gTestSceneHorizontalDirection = QSwipeGesture::Left;
    mContentScrollArea->gestureEvent(gestureEvent);
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollToNext);

    delete gestureEvent;
}

/*!
 Test function for eventFilter
 1. Call eventFilter with event type: None
 2. Call eventFilter with event type: Gesture, scroll area is moving
 3. Call eventFilter with event type: Gesture, scroll area area not moving,
 swipe gesture
 */
void TestCalenDayContentScrollArea::testEventFilter()
{
    bool result = false;

    //1)
    QEvent noneEvent(QEvent::None);
    result = mContentScrollArea->eventFilter(NULL, &noneEvent);
    QVERIFY(!result);   //false
    
    //2)
    QEvent gestureEvent(QEvent::Gesture);
    mContentScrollArea->mIsMoving = true;
    result = mContentScrollArea->eventFilter(NULL, &gestureEvent);
    QVERIFY(result);   //true
    
    //3)
    QSwipeGesture *swipeGesture = new QSwipeGesture();
    QList<QGesture*> list;
    list.append(swipeGesture);
    QGestureEvent *gesture = new QGestureEvent(list);
    QEvent *swipeGestureEvent = static_cast <QEvent*> (gesture);
    
    mContentScrollArea->mIsMoving = false;
    result = mContentScrollArea->eventFilter(NULL, swipeGestureEvent);
    QVERIFY(result);   //true
    
    delete gesture;
}

/*!
 Test function for event
 1. Test event type: Gesture, scroll area not moving
 2. Test event type: Gesture, scroll area is moving
 3. Test event type: FocusOut, scroll area not moving
 4. Test event type: FocusOut, scroll area is moving
 5. Test event type: FocusIn, scroll area not moving
 6. Test event type: FocusIn, scroll area is moving
 7. Test event type: LayoutRequest
 */
void TestCalenDayContentScrollArea::testEvent()
{
    bool result = false;

    //1)
    QEvent gestureEvent(QEvent::Gesture);
    mContentScrollArea->mIsMoving = false;
    result = mContentScrollArea->event(&gestureEvent);
    QVERIFY(!result);   //false
    
    //2)
    mContentScrollArea->mIsMoving = true;
    result = mContentScrollArea->event(&gestureEvent);
    QVERIFY(result);   //true
    
    //3)
    QEvent focusOutEvent(QEvent::FocusOut);
    mContentScrollArea->mIsMoving = false;
    result = mContentScrollArea->event(&focusOutEvent);
    QVERIFY(!result);   //false
    
    //4)
    mContentScrollArea->mIsMoving = true;
    result = mContentScrollArea->event(&focusOutEvent);
    QVERIFY(result);   //true
    
    //5)
    QEvent focusInEvent(QEvent::FocusIn);
    mContentScrollArea->mIsMoving = false;
    result = mContentScrollArea->event(&focusInEvent);
    QVERIFY(!result);   //false
    
    //6)
    mContentScrollArea->mIsMoving = true;
    result = mContentScrollArea->event(&focusInEvent);
    QVERIFY(result);   //true
    
    //7)
    QEvent layoutReqEvent(QEvent::LayoutRequest);
    QPointF oldPos = mContentScrollArea->contentWidget()->pos();
    mContentScrollArea->event(&layoutReqEvent);
    QPointF pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(oldPos == pos, false);
}

/*!
 Test function for moveTo
 1. Test moveTo, day not changed
 2. Test moveTo, day changed to next
 3. Test moveTo, day changed to prev
 4. Test moveTo, disallowed directon
 */
void TestCalenDayContentScrollArea::testMoveTo()
{
#ifndef __WINSCW__
    //1)
    mContentScrollArea->mMoveDirection = ECalenScrollNoDayChange;
    QPointF newPos(10,0);
    mContentScrollArea->moveTo(newPos);
    QPointF pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(abs(pos.x()), abs(mContentScrollArea->contentWidget()->pos().x()));
    QCOMPARE(abs(pos.y()), abs(mContentScrollArea->contentWidget()->pos().y()));
    
    //2)
    newPos = QPointF(20,0);
    mContentScrollArea->mMoveDirection = ECalenScrollToNext;
    mContentScrollArea->moveTo(newPos);
    pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(abs(pos.x()), abs(mContentScrollArea->contentWidget()->pos().x()));
    QCOMPARE(abs(pos.y()), abs(mContentScrollArea->contentWidget()->pos().y()));
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollNoDayChange);
    
    //3)
    newPos = QPointF(-20,0);
    mContentScrollArea->mMoveDirection = ECalenScrollToNext;
    mContentScrollArea->moveTo(newPos);
    pos = mContentScrollArea->contentWidget()->pos();
    QCOMPARE(abs(mContentScrollArea->contentWidget()->pos().x()), abs(pos.x()));
    QCOMPARE(abs(mContentScrollArea->contentWidget()->pos().y()), abs(pos.y()));
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollNoDayChange);
    
    //4)
    newPos = QPointF(20,0);
    pos = mContentScrollArea->contentWidget()->pos();
    mContentScrollArea->mDisallowedDirection = ECalenScrollToNext;
    mContentScrollArea->mMoveDirection = ECalenScrollToNext;
    mContentScrollArea->moveTo(newPos);
    QCOMPARE(abs(mContentScrollArea->contentWidget()->pos().x()), abs(pos.x()));
    QCOMPARE(abs(mContentScrollArea->contentWidget()->pos().y()), abs(pos.y()));
#endif /* __WINSCW__ */
}

/*!
 Test function for checking if swipe is horizontal or not
 1) test Horizontal swipe
 2) test no horizontal swipe
 */
void TestCalenDayContentScrollArea::testHorizontalSwipe()
{
#ifndef __WINSCW__
    //1)
    QCOMPARE(mContentScrollArea->isHorizontalSwipe(KCalenSwipeAngle - 5), true);

    //2)
    QCOMPARE(mContentScrollArea->isHorizontalSwipe(KCalenSwipeAngle + 5), false);
#endif /* __WINSCW__ */
}

/*!
 Test moveFinished slot
 1) test moveFinished, no day change
 2) test moveFinished, day changed to next
 3) test moveFinished, day changed to prev
 */
void TestCalenDayContentScrollArea::testMoveFinished()
{
#ifndef __WINSCW__
    //1)
    mContentScrollArea->mMoveDirection = ECalenScrollNoDayChange;
    mContentScrollArea->moveFinished();
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollNoDayChange);
    
    //2)
    mContentScrollArea->mMoveDirection = ECalenScrollToNext;
    mContentScrollArea->moveFinished();
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollNoDayChange);
        
    //3)
    mContentScrollArea->mMoveDirection = ECalenScrollToPrev;
    mContentScrollArea->moveFinished();
    QCOMPARE(mContentScrollArea->mMoveDirection, ECalenScrollNoDayChange);
#endif /* __WINSCW__ */
}

/*!
 Test orientationChanged slot
 1) test orientation changed to horizontal
 2) test orientation changed to vertical
 */
void TestCalenDayContentScrollArea::testOrientationChanged() 
{
#ifndef __WINSCW__
    //1)
    mContentScrollArea->orientationChanged(Qt::Horizontal);
    QCOMPARE(mContentScrollArea->mOrientation, Qt::Horizontal);
    
    //2)
    mContentScrollArea->orientationChanged(Qt::Vertical);
    QCOMPARE(mContentScrollArea->mOrientation, Qt::Vertical);
#endif /* __WINSCW__ */
}

/*!
 Test checkPanDirection function
 1) test pan direction, gesture cancelled, vertical orientation
 2) test pan direction, gesture started, vertical orientation, vertical movement
 3) test pan direction, gesture started, vertical orientation, horizontal movement
 4) test pan direction, gesture started, horizontal orientation, vertical movement
 5) test pan direction, gesture started, horizontal orientation, horizontal movement
 */
void TestCalenDayContentScrollArea::testCheckPanDirection()
{
#ifndef __WINSCW__
    QPanGesture *gesture = new QPanGesture();
    
    //1)
    gesture->setOffset(QPointF(10, 100));
    gTestGestureState = Qt::GestureCanceled;
    mContentScrollArea->mPanDayDirection = CalenDayContentScrollArea::ECalenPanNotSet;
    mContentScrollArea->mOrientation = Qt::Vertical;
    mContentScrollArea->checkPanDirection(gesture);
    QCOMPARE(mContentScrollArea->mPanDayDirection, CalenDayContentScrollArea::ECalenPanNotSet);

    // Cases 2-5 temporarily unavailable
/*
    //2)
    gesture->setOffset(QPointF(10, 100));
    gTestGestureState = Qt::GestureStarted;
    mContentScrollArea->mPanDayDirection = CalenDayContentScrollArea::ECalenPanNotSet;
    mContentScrollArea->mOrientation = Qt::Vertical;
    mContentScrollArea->checkPanDirection(gesture);
    QCOMPARE(mContentScrollArea->mPanDayDirection, CalenDayContentScrollArea::ECalenPanVertical);
    
    //3)
    gesture->setOffset(QPointF(100, 10));
    gTestGestureState = Qt::GestureStarted;
    mContentScrollArea->mPanDayDirection = CalenDayContentScrollArea::ECalenPanNotSet;
    mContentScrollArea->mOrientation = Qt::Vertical;
    mContentScrollArea->checkPanDirection(gesture);
    QCOMPARE(mContentScrollArea->mPanDayDirection, CalenDayContentScrollArea::ECalenPanHorizontal);
    
    //4)
    gesture->setOffset(QPointF(10, 100));
    gTestGestureState = Qt::GestureStarted;
    mContentScrollArea->mPanDayDirection = CalenDayContentScrollArea::ECalenPanNotSet;
    mContentScrollArea->mOrientation = Qt::Horizontal;
    mContentScrollArea->checkPanDirection(gesture);
    QCOMPARE(mContentScrollArea->mPanDayDirection, CalenDayContentScrollArea::ECalenPanHorizontal);
    
    //5)
    gesture->setOffset(QPointF(100, 10));
    gTestGestureState = Qt::GestureStarted;
    mContentScrollArea->mPanDayDirection = CalenDayContentScrollArea::ECalenPanNotSet;
    mContentScrollArea->mOrientation = Qt::Horizontal;
    mContentScrollArea->checkPanDirection(gesture);
    QCOMPARE(mContentScrollArea->mPanDayDirection, CalenDayContentScrollArea::ECalenPanVertical);
*/
#endif /* __WINSCW__ */
}

/*!
 Test setter and getter for disallowed direction
 1) test setter
 2) test getter
 */
void TestCalenDayContentScrollArea::testSetGetDisallowedScrollDirection()
{
    QCOMPARE(mContentScrollArea->mDisallowedDirection, ECalenScrollNoDayChange);
    
    //1)
    mContentScrollArea->setDisallowedScrollDirection(ECalenScrollToNext);
    QCOMPARE(mContentScrollArea->mDisallowedDirection, ECalenScrollToNext);
    
    //2)
    QCOMPARE(mContentScrollArea->disallowedScrollDirection(), ECalenScrollToNext);
}

QTEST_MAIN(TestCalenDayContentScrollArea);
#include "unittest_calendaycontentscrollarea.moc"
