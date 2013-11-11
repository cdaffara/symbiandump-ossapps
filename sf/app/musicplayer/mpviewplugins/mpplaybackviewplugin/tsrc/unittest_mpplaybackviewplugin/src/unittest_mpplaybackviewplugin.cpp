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
* Description: Unit test for MpPlaybackViewPlugin
*
*/

#include <QSignalSpy>
#include <qnamespace.h>

#include "stub/inc/mpplaybackview.h"
#include "stub/inc/xqplugin.h"
#include "unittest_mpplaybackviewplugin.h"


// Do this so we can access all member variables.
#define private public
#include "mpplaybackviewplugin.h"
#undef private

//This so mpplaybackviewplugin.cpp can load qt stub items
#include "../../src/mpplaybackviewplugin.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpPlaybackViewPlugin tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpplaybackviewplugin.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpPlaybackViewPlugin::TestMpPlaybackViewPlugin()
    : mTest(0)
{
}

TestMpPlaybackViewPlugin::~TestMpPlaybackViewPlugin()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpPlaybackViewPlugin::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpPlaybackViewPlugin::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpPlaybackViewPlugin::init()
{
    mTest = new MpPlaybackViewPlugin();
}

/*!
 Called after every testfunction.
 */
void TestMpPlaybackViewPlugin::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 testConstructor
 */
void TestMpPlaybackViewPlugin::testConstructor()
{
    QVERIFY(!mTest->mView);
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::NullView); 
}

/*!
 testCreateView
 */
void TestMpPlaybackViewPlugin::testCreateView()
{
    mTest->createView();
    QVERIFY(mTest->mView);
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::Created);
    QSignalSpy spy( mTest, SIGNAL(command(int)) );
    mTest->mView->emitCommand(123);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(123));
    
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
    
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}


/*!
 testdestroyView
 */
void TestMpPlaybackViewPlugin::testdestroyView()
{
    mTest->mView = new MpPlaybackView();
    mTest->mState = MpPlaybackViewPlugin::Created;
    mTest->destroyView();
    QVERIFY(!mTest->mView);
    mTest->mView = new MpPlaybackView();
    mTest->mState = MpPlaybackViewPlugin::NullView;
    mTest->destroyView();
    QVERIFY(mTest->mView);
    
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
    
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}

/*!
 testActivateView
 */
void TestMpPlaybackViewPlugin::testActivateView()
{
    mTest->mState = MpPlaybackViewPlugin::NullView;
    mTest->activateView(); // we should be able to call activate without a view and not crash.

    
    mTest->mView = new MpPlaybackView();
    mTest->mState = MpPlaybackViewPlugin::NullView;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::NullView);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);

    mTest->mView->resetCounters();
    mTest->mState = MpPlaybackViewPlugin::Activated;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
           
    mTest->mView->resetCounters();
    mTest->mState = MpPlaybackViewPlugin::Created;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,1);
    QCOMPARE(mTest->mView->mActivateViewCount,1);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       
    mTest->mView->resetCounters();
    mTest->mState = MpPlaybackViewPlugin::Initialized;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,1);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       

    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}
    
/*!
 testDeactivateView
 */
void TestMpPlaybackViewPlugin::testDeactivateView()
{
    mTest->mState = MpPlaybackViewPlugin::NullView;
    mTest->deactivateView(); // we should be able to call deactivate without a view and not crash.
    
    mTest->mView = new MpPlaybackView();
    mTest->mState = MpPlaybackViewPlugin::NullView;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::NullView);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);

    mTest->mView->resetCounters();
    mTest->mState = MpPlaybackViewPlugin::Activated;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::Initialized);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,1);
           
    mTest->mView->resetCounters();
    mTest->mState = MpPlaybackViewPlugin::Created;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::Created);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       
    mTest->mView->resetCounters();
    mTest->mState = MpPlaybackViewPlugin::Initialized;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpPlaybackViewPlugin::Initialized);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
    
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}

void TestMpPlaybackViewPlugin::testGetView()
{
    MpPlaybackView* tmpView;
    mTest->mView = new MpPlaybackView();
    tmpView = mTest->mView;
    mTest->getView();
    QCOMPARE(int(tmpView),int(mTest->getView()));
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}
    
// End of file
