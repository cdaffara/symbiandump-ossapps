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
* Description: Unit test for MpDetailsViewPlugin
*
*/

#include <QSignalSpy>
#include <qnamespace.h>
#include <hbapplication.h>
#include <hbmainwindow.h>
#include <hbInstance.h>

#include "unittest_mpdetailsviewplugin.h"
#include "stub/inc/mpdetailsview.h"

// Do this so we can access all member variables.
#define private public
#include "mpdetailsviewplugin.h"
#undef private


/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main( int argc, char *argv[] )
{
    HbApplication app( argc, argv );
    HbMainWindow window;
    TestMpDetailsViewPlugin tv;


if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpdetailsviewplugin.txt";

        return QTest::qExec(&tv, 3, pass);
    }    
}

TestMpDetailsViewPlugin::TestMpDetailsViewPlugin()
    : mTest( 0 )
{
}

TestMpDetailsViewPlugin::~TestMpDetailsViewPlugin()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpDetailsViewPlugin::initTestCase()
{
}

/*!
 Called after the last testfunction was executed.
 */
void TestMpDetailsViewPlugin::cleanupTestCase()
{
QCoreApplication::processEvents();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpDetailsViewPlugin::init()
{
    mTest = new MpDetailsViewPlugin();
}

/*!
 Called after every testfunction.
 */
void TestMpDetailsViewPlugin::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 testConstructor
 */
void TestMpDetailsViewPlugin::testConstructor()
{
    QVERIFY( !mTest->mView );
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::NullView ); 
}

/*!
 testCreateView
 */
void TestMpDetailsViewPlugin::testCreateView()
{
    mTest->createView();
    QVERIFY( mTest->mView );
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::Created );
    QSignalSpy spy( mTest, SIGNAL( command( int ) ) );
    mTest->mView->emitCommand( 123 );
    QCOMPARE( spy.count(), 1 );
    QCOMPARE( spy.at( 0 ).at( 0 ), QVariant( 123 ) );
    
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 0 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );
    
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}


/*!
 testdestroyView
 */
void TestMpDetailsViewPlugin::testdestroyView()
{
    mTest->mView = new MpDetailsView();
    mTest->mState = MpDetailsViewPlugin::Created;
    mTest->destroyView();
    QVERIFY( !mTest->mView );
    
    mTest->mView = new MpDetailsView();
    mTest->mState = MpDetailsViewPlugin::Initialized;
    mTest->destroyView();
    QVERIFY( !mTest->mView );
    
    mTest->mView = new MpDetailsView();
    mTest->mState = MpDetailsViewPlugin::Activated;
    mTest->destroyView();
    QVERIFY( !mTest->mView );
    
    mTest->mView = new MpDetailsView();
    mTest->mState = MpDetailsViewPlugin::NullView;
    mTest->destroyView();
    QVERIFY( mTest->mView );
    
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 0 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );
    
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}

/*!
 testActivateView
 */
void TestMpDetailsViewPlugin::testActivateView()
{
    mTest->mState = MpDetailsViewPlugin::NullView;
    mTest->activateView(); // we should be able to call activate without a view and not crash.
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::NullView );
    
    mTest->mView = new MpDetailsView();
    mTest->mState = MpDetailsViewPlugin::NullView;
    mTest->activateView();
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::NullView );
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 0 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );

    mTest->mView->resetCounters();
    mTest->mState = MpDetailsViewPlugin::Activated;
    mTest->activateView();
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::Activated );
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 0 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );
           
    mTest->mView->resetCounters();
    mTest->mState = MpDetailsViewPlugin::Created;
    mTest->activateView();
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::Activated );
    QCOMPARE( mTest->mView->mInitializeViewCount, 1 );
    QCOMPARE( mTest->mView->mActivateViewCount, 1 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );
       
    mTest->mView->resetCounters();
    mTest->mState = MpDetailsViewPlugin::Initialized;
    mTest->activateView();
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::Activated );
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 1 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );
       
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}
    
/*!
 testDeactivateView
 */
void TestMpDetailsViewPlugin::testDeactivateView()
{
    mTest->mState = MpDetailsViewPlugin::NullView;
    mTest->deactivateView(); // we should be able to call deactivate without a view and not crash.
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::NullView );
    
    mTest->mView = new MpDetailsView();
    mTest->mState = MpDetailsViewPlugin::NullView;
    mTest->deactivateView();
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::NullView );
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 0 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );

    mTest->mView->resetCounters();
    mTest->mState = MpDetailsViewPlugin::Created;
    mTest->deactivateView();
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::Created );
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 0 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );
       
    mTest->mView->resetCounters();
    mTest->mState = MpDetailsViewPlugin::Initialized;
    mTest->deactivateView();
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::Initialized );
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 0 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 0 );
    
    mTest->mView->resetCounters();
    mTest->mState = MpDetailsViewPlugin::Activated;
    mTest->deactivateView();
    QCOMPARE( mTest->mState, MpDetailsViewPlugin::Initialized );
    QCOMPARE( mTest->mView->mInitializeViewCount, 0 );
    QCOMPARE( mTest->mView->mActivateViewCount, 0 );
    QCOMPARE( mTest->mView->mDeactivateViewCount, 1 );
           
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}

void TestMpDetailsViewPlugin::testGetView()
{
    MpDetailsView* tmpView;
    mTest->mView = new MpDetailsView();
    tmpView = mTest->mView;
    QCOMPARE( int( tmpView ),int( mTest->getView() ) );
    delete mTest->mView; 
    //mTest->mView = 0; intentionally not = 0 to prevent unecesary double delete.
}
    
// End of file
