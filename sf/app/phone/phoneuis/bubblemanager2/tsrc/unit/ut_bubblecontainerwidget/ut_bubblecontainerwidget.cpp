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

#include <QtGui>
#include <QtTest/QtTest>

#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbaction.h>
#include <hbmainwindow.h>
#include <hbinstance.h>
#include <hbstyle.h>
#include <hbframeitem.h>

#include "bubbletest.h"
#include "bubblecontainerwidget.h"

class TestBubbleContainerWidget : public BubbleContainerWidget
{
public:
    TestBubbleContainerWidget() {};
    ~TestBubbleContainerWidget() {};

    void testMouseMove(QGraphicsSceneMouseEvent *event)
         { BubbleContainerWidget::mouseMoveEvent(event); };
};

class ut_BubbleContainerWidget : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testLeftMouseClick();
    void testRightMouseClick();
    void testMouseMove();

private:
    TestBubbleContainerWidget* mContainer;
    HbMainWindow* mMainWindow;
};

void ut_BubbleContainerWidget::initTestCase()
{
    mMainWindow = new HbMainWindow();
    mContainer = new TestBubbleContainerWidget();
    mMainWindow->addView(mContainer);
    mMainWindow->show();

    HbFrameItem* frame =
        qgraphicsitem_cast<HbFrameItem*>(
            static_cast<HbWidget*>(mContainer)->primitive("background"));
    QVERIFY(frame->frameDrawer().frameGraphicsName()=="qtg_fr_list_normal");
    QVERIFY(frame->isVisible()==false);
}

void ut_BubbleContainerWidget::cleanupTestCase()
{
    delete mMainWindow;
}

void ut_BubbleContainerWidget::testLeftMouseClick()
{
    QSignalSpy spy(mContainer,SIGNAL(clicked()));

    QPointF clickPos = mContainer->scenePos() +
                       mContainer->rect().center();
    QPoint windowPos = mMainWindow->mapFromScene( clickPos );

    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton, 0, windowPos,100 );
    QCOMPARE( spy.count(), 1 );
}

void ut_BubbleContainerWidget::testRightMouseClick()
{
    QSignalSpy spy(mContainer,SIGNAL(clicked()));

    QPointF clickPos = mContainer->scenePos() +
                       mContainer->rect().center();
    QPoint windowPos = mMainWindow->mapFromScene( clickPos );

    QTest::mouseClick( mMainWindow->viewport(), Qt::RightButton, 0, windowPos,100 );
    QCOMPARE( spy.count(), 0 );
}

void ut_BubbleContainerWidget::testMouseMove()
{
    QSignalSpy spy(mContainer,SIGNAL(clicked()));

    QPointF clickPos = mContainer->scenePos() +
                       mContainer->rect().center();
    QPoint windowPos = mMainWindow->mapFromScene( clickPos );

    // move within
    QTest::mousePress( mMainWindow->viewport(), Qt::LeftButton, 0, windowPos, 100 );

    // QTest::mouseMove() doesn't work
    QGraphicsSceneMouseEvent moveEvent(QEvent::MouseMove);
    moveEvent.setButtons(Qt::LeftButton);
    moveEvent.setPos(windowPos-QPointF(1,1));
    mContainer->testMouseMove(&moveEvent);

    QTest::mouseRelease( mMainWindow->viewport(), Qt::LeftButton, 0, windowPos, 100 );
    QCOMPARE( spy.count(), 1 );


    // move out
    QTest::mousePress( mMainWindow->viewport(), Qt::LeftButton, 0, windowPos, 100 );
    moveEvent.setPos(QPointF(-10,-10));
    mContainer->testMouseMove(&moveEvent);

    QTest::mouseRelease( mMainWindow->viewport(), Qt::LeftButton, 0, windowPos, 100 );
    QCOMPARE( spy.count(), 1 );

    // move in
    QTest::mousePress( mMainWindow->viewport(), Qt::LeftButton, 0, QPoint(1,1), 100 );
    moveEvent.setPos(QPointF(windowPos));
    mContainer->testMouseMove(&moveEvent);

    QTest::mouseRelease( mMainWindow->viewport(), Qt::LeftButton, 0, windowPos, 100 );
    QCOMPARE( spy.count(), 1 );

}

BUBBLE_TEST_MAIN(ut_BubbleContainerWidget)
#include "ut_bubblecontainerwidget.moc"
