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
* Description: Unit test for mpnowplayingwidget
*
*/

#include <qnamespace.h>
#include <QtGui/QtGui>

#include "unittest_mpsnapshotwidget.h"

// Do this so we can access all member variables.
#define private public
#include "mpsnapshotwidget.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QGraphicsScene *testScene = new QGraphicsScene(QRectF(0,0,200,200));
    QGraphicsView mainWindow(testScene);
    mainWindow.show();

    TestMpSnapshotWidget tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsnapshotwidget.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpSnapshotWidget::TestMpSnapshotWidget()
    : mTest(0)
{
}

TestMpSnapshotWidget::~TestMpSnapshotWidget()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSnapshotWidget::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSnapshotWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSnapshotWidget::init()
{
    mTest = new MpSnapshotWidget();
}

/*!
 Called after every testfunction.
 */
void TestMpSnapshotWidget::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Tests constructor. Good cases.
 */
void TestMpSnapshotWidget::testConstructor()
{
    cleanup();
    init();
    QVERIFY(mTest->mSnapshot == 0);
    cleanup();
    QGraphicsRectItem rectItem;
    mTest = new MpSnapshotWidget(&rectItem);
    QVERIFY(mTest->parentItem()  == &rectItem);
    mTest->setParentItem(0);
}

/*!
 Tests snapshot capture size, also test the captured data and 
 the ability to set position and bounding rect so the object can 
 be used to mock the original.
 */
void TestMpSnapshotWidget::testCapture()
{
    cleanup();
    init();
    QGraphicsView *mainWindow = qobject_cast<QGraphicsView *>(qApp->activeWindow());
    QVERIFY(mainWindow != 0);
    QGraphicsRectItem rectItem(QRectF(0,0,10,10));
    mainWindow->scene()->addItem(&rectItem);
    rectItem.show();
    rectItem.setPos(10,10);
    QRgb red = qRgb(255,0,0);
    QRgb green = qRgb(0,255,0);
    rectItem.setPen(QPen(red));
    rectItem.setBrush(QBrush(green));
    mTest->capture(mainWindow, &rectItem);
    mainWindow->scene()->addItem(mTest);
    
    QVERIFY(mTest->mSnapshot != 0);
    QVERIFY(mTest->mSnapshot->width()  == 10);
    QVERIFY(mTest->mSnapshot->height()  == 10);
    QCOMPARE(mTest->scenePos(), rectItem.scenePos());
    QCOMPARE(mTest->sceneBoundingRect(),rectItem.sceneBoundingRect());
    QCOMPARE(mTest->mSnapshot->pixel( 0, 0 ) , red);
    QCOMPARE(mTest->mSnapshot->pixel( 5, 5 ) , green);
    
    rectItem.setPos(20,20);
    mTest->capture(mainWindow, &rectItem);
    
    QCOMPARE(mTest->scenePos(), rectItem.scenePos());
    QCOMPARE(mTest->sceneBoundingRect(),rectItem.sceneBoundingRect());
    QCOMPARE(mTest->mSnapshot->pixel( 0, 0 ) , red);
    QCOMPARE(mTest->mSnapshot->pixel( 5, 5 ) , green);
    
    mainWindow->scene()->removeItem(mTest);
    mainWindow->scene()->removeItem(&rectItem);
}

// End of file
