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
* Description: Unit test for MpTrackListWidget
*
*/

/*
* there are no valuable test cases for following functions 
* resizeEvent()
* gestureEvent()
* mousePressEvent()
*
*/

#include "unittest_mptracklistwidget.h"
#include "hbinstancestub.h"
#include <hblistview.h>

// Do this so we can access all member variables.
#define private public
#include "mptracklistwidget.h"
#undef private

//This so we can test private functions
#include "../../src/mptracklistwidget.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpTrackListWidget tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mptracklistwidget.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpTrackListWidget::TestMpTrackListWidget()
    : mTest(0)
{
}

TestMpTrackListWidget::~TestMpTrackListWidget()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpTrackListWidget::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpTrackListWidget::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpTrackListWidget::init()
{
    mTest = new MpTrackListWidget();
}

/*!
 Called after every testfunction.
 */
void TestMpTrackListWidget::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 test constructor.
*/
void TestMpTrackListWidget::testConstructor()
{
    QVERIFY( mTest->mList->layoutName() == "mpmwtracklist" );
}

/*!
 test swipeAngleToDirection
 */
void TestMpTrackListWidget::testSwipeAngleToDirection() 
{
    hbInstance->allMainWindows()[0]->mOrientation = Qt::Vertical;
    QSwipeGesture::SwipeDirection direction;
    
    direction = mTest->swipeAngleToDirection( 70 );
    QVERIFY( direction == QSwipeGesture::Up );
    
    direction = mTest->swipeAngleToDirection( 250 );
    QVERIFY( direction == QSwipeGesture::Down );
    
    direction = mTest->swipeAngleToDirection( 20 );
    QVERIFY( direction == QSwipeGesture::Right );
    
    direction = mTest->swipeAngleToDirection( 340 );
    QVERIFY( direction == QSwipeGesture::Right );
    
    direction = mTest->swipeAngleToDirection( 160 );
    QVERIFY( direction == QSwipeGesture::Left );
    
    hbInstance->allMainWindows()[0]->mOrientation = Qt::Horizontal;
#ifdef __WINS__ 
    direction = mTest->swipeAngleToDirection( 70 );
    QVERIFY( direction == QSwipeGesture::Right );
    
    direction = mTest->swipeAngleToDirection( 100 );
    QVERIFY( direction == QSwipeGesture::Right );
#else
    direction = mTest->swipeAngleToDirection( 70 );
    QVERIFY( direction == QSwipeGesture::Left );
    
    direction = mTest->swipeAngleToDirection( 340 );
    QVERIFY( direction == QSwipeGesture::Up );    
#endif    
}

// End of file
