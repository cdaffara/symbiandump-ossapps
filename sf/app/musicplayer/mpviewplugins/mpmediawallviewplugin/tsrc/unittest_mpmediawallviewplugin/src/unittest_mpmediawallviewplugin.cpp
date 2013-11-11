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
* Description: Unit test for MpMediaWallViewPlugin
*
*/

#include <QSignalSpy>
#include <qnamespace.h>

#include "stub/inc/mpmediawallview.h"
#include "stub/inc/xqplugin.h"
#include "unittest_mpmediawallviewplugin.h"


// Do this so we can access all member variables.
#define private public
#include "mpmediawallviewplugin.h"
#undef private

//This so MpMediaWallViewPlugin.cpp can load qt stub items
#include "../../src/mpmediawallviewplugin.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpMediaWallViewPlugin tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpmediawallviewplugin.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpMediaWallViewPlugin::TestMpMediaWallViewPlugin()
    : mTest(0)
{
}

TestMpMediaWallViewPlugin::~TestMpMediaWallViewPlugin()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpMediaWallViewPlugin::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpMediaWallViewPlugin::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpMediaWallViewPlugin::init()
{
    mTest = new MpMediaWallViewPlugin();
}

/*!
 Called after every testfunction.
 */
void TestMpMediaWallViewPlugin::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 testConstructor
 */
void TestMpMediaWallViewPlugin::testConstructor()
{
    QVERIFY(!mTest->mView);
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::NullView);
}

/*!
 testCreateView
 */
void TestMpMediaWallViewPlugin::testCreateView()
{

    mTest->createView();
    QVERIFY(mTest->mView);
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::Created);
    /*
    QSignalSpy spy( mTest, SIGNAL(command(int)) );
    mTest->mView->emitCommand(123);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0), QVariant(123));
    */
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
    
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.

}


/*!
 testdestroyView
 */
void TestMpMediaWallViewPlugin::testdestroyView()
{

    mTest->mView = new MpMediaWallView();
    mTest->mState = MpMediaWallViewPlugin::Created;
    mTest->destroyView();
    QVERIFY(!mTest->mView);
    mTest->mView = new MpMediaWallView();
    mTest->mState = MpMediaWallViewPlugin::NullView;
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
void TestMpMediaWallViewPlugin::testActivateView()
{

    mTest->mState = MpMediaWallViewPlugin::NullView;
    mTest->activateView(); // we should be able to call activate without a view and not crash.

    
    mTest->mView = new MpMediaWallView();
    mTest->mState = MpMediaWallViewPlugin::NullView;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::NullView);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);

    mTest->mView->resetCounters();
    mTest->mState = MpMediaWallViewPlugin::Activated;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
           
    mTest->mView->resetCounters();
    mTest->mState = MpMediaWallViewPlugin::Created;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,1);
    QCOMPARE(mTest->mView->mActivateViewCount,1);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       
    mTest->mView->resetCounters();
    mTest->mState = MpMediaWallViewPlugin::Initialized;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,1);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       

    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.

}

    
/*!
 testDeactivateView
 */
void TestMpMediaWallViewPlugin::testDeactivateView()
{

    mTest->mState = MpMediaWallViewPlugin::NullView;
    mTest->deactivateView(); // we should be able to call deactivate without a view and not crash.
    
    mTest->mView = new MpMediaWallView();
    mTest->mState = MpMediaWallViewPlugin::NullView;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::NullView);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);

    mTest->mView->resetCounters();
    mTest->mState = MpMediaWallViewPlugin::Activated;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::Initialized);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,1);
           
    mTest->mView->resetCounters();
    mTest->mState = MpMediaWallViewPlugin::Created;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::Created);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       
    mTest->mView->resetCounters();
    mTest->mState = MpMediaWallViewPlugin::Initialized;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpMediaWallViewPlugin::Initialized);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
    
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.

}

void TestMpMediaWallViewPlugin::testGetView()
{
    MpMediaWallView* tmpView;
    mTest->mView = new MpMediaWallView();
    tmpView = mTest->mView;
    mTest->getView();
    QCOMPARE(int(tmpView),int(mTest->getView()));
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.

}
    
// End of file
