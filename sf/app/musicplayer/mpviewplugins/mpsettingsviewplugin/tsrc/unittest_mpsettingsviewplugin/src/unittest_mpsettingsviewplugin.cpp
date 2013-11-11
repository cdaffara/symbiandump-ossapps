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
* Description: Unit test for MpSettingsViewPlugin
*
*/

#include <QSignalSpy>
#include <qnamespace.h>

#include "stub/inc/mpsettingsview.h"
#include "stub/inc/xqplugin.h"
#include "unittest_mpsettingsviewplugin.h"


// Do this so we can access all member variables.
#define private public
#include "mpsettingsviewplugin.h"
#undef private

//This so we mpsettingsviewplugin.cpp can load qt stub items
#include "../../src/mpsettingsviewplugin.cpp"

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMpSettingsViewPlugin tv;

    if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsettingsviewplugin.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

TestMpSettingsViewPlugin::TestMpSettingsViewPlugin()
    : mTest(0)
{
}

TestMpSettingsViewPlugin::~TestMpSettingsViewPlugin()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSettingsViewPlugin::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSettingsViewPlugin::cleanupTestCase()
{
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSettingsViewPlugin::init()
{
    mTest = new MpSettingsViewPlugin();
}

/*!
 Called after every testfunction.
 */
void TestMpSettingsViewPlugin::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 testConstructor
 */
void TestMpSettingsViewPlugin::testConstructor()
{
    QVERIFY(!mTest->mView);
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::NullView); 
}

/*!
 testCreateView
 */
void TestMpSettingsViewPlugin::testCreateView()
{
    mTest->createView();
    QVERIFY(mTest->mView);
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::Created);
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
void TestMpSettingsViewPlugin::testdestroyView()
{
    mTest->mView = new MpSettingsView();
    mTest->mState = MpSettingsViewPlugin::Created;
    mTest->destroyView();
    QVERIFY(!mTest->mView);
    mTest->mView = new MpSettingsView();
    mTest->mState = MpSettingsViewPlugin::NullView;
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
void TestMpSettingsViewPlugin::testActivateView()
{
    mTest->mState = MpSettingsViewPlugin::NullView;
    mTest->activateView(); // we should be able to call activate without a view and not crash.

    
    mTest->mView = new MpSettingsView();
    mTest->mState = MpSettingsViewPlugin::NullView;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::NullView);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);

    mTest->mView->resetCounters();
    mTest->mState = MpSettingsViewPlugin::Activated;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
           
    mTest->mView->resetCounters();
    mTest->mState = MpSettingsViewPlugin::Created;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,1);
    QCOMPARE(mTest->mView->mActivateViewCount,1);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       
    mTest->mView->resetCounters();
    mTest->mState = MpSettingsViewPlugin::Initialized;
    mTest->activateView();
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::Activated);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,1);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       

    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}
    
/*!
 testDeactivateView
 */
void TestMpSettingsViewPlugin::testDeactivateView()
{
    mTest->mState = MpSettingsViewPlugin::NullView;
    mTest->deactivateView(); // we should be able to call deactivate without a view and not crash.
    
    mTest->mView = new MpSettingsView();
    mTest->mState = MpSettingsViewPlugin::NullView;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::NullView);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);

    mTest->mView->resetCounters();
    mTest->mState = MpSettingsViewPlugin::Activated;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::Initialized);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,1);
           
    mTest->mView->resetCounters();
    mTest->mState = MpSettingsViewPlugin::Created;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::Created);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
       
    mTest->mView->resetCounters();
    mTest->mState = MpSettingsViewPlugin::Initialized;
    mTest->deactivateView();
    QCOMPARE(mTest->mState, MpSettingsViewPlugin::Initialized);
    QCOMPARE(mTest->mView->mInitializeViewCount,0);
    QCOMPARE(mTest->mView->mActivateViewCount,0);
    QCOMPARE(mTest->mView->mDeactivateViewCount,0);
    
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}

void TestMpSettingsViewPlugin::testGetView()
{
    MpSettingsView* tmpView;
    mTest->mView = new MpSettingsView();
    tmpView = mTest->mView;
    mTest->getView();
    QCOMPARE(int(tmpView),int(mTest->getView()));
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}
    
// End of file
