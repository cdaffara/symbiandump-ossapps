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
* Description: Unit Test for mpsettingsview.
*
*/

#include <hbmainwindow.h>
#include <hbapplication.h>
#include <QSignalSpy>

#include "unittest_mpsettingsview.h"

// Do this so we can access all member variables.
#define private public
#include "mpsettingsview.h"
#undef private

/*!
 Make our test case a stand-alone executable that runs all the test functions.
 */
int main(int argc, char *argv[])
{
    HbApplication app(argc, argv);
    HbMainWindow window;

    TestMpSettingsView tv;

if ( argc > 1 ) {
        return QTest::qExec( &tv, argc, argv);
    }
    else {
        char *pass[3];
        pass[0] = argv[0];
        pass[1] = "-o";
        pass[2] = "c:\\data\\unittest_mpsettingsview.txt";

        return QTest::qExec(&tv, 3, pass);
    }
}

//Constructor
TestMpSettingsView::TestMpSettingsView()
    : mTest(0)
{

}

//Destructor
TestMpSettingsView::~TestMpSettingsView()
{
    delete mTest;
}

/*!
 Called before the first testfunction is executed.
 */
void TestMpSettingsView::initTestCase()
{

}

/*!
 Called after the last testfunction was executed.
 */
void TestMpSettingsView::cleanupTestCase()
{
QCoreApplication::processEvents();
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSettingsView::init()
{
    mTest = new MpSettingsView();
}

/*!
 Called after every testfunction.
 */
void TestMpSettingsView::cleanup()
{
    delete mTest;
    mTest = 0;
}

/*!
 Called before each testfunction is executed.
 */
void TestMpSettingsView::testInitializeView()
{
    //test widget initialization
    QVERIFY(mTest->mAudioEffectsWidget == 0);
    QVERIFY(mTest->mSoftKeyBack == 0);
    QVERIFY(mTest->mWindow == 0);

    mTest->initializeView();
    QVERIFY(mTest->mAudioEffectsWidget != 0);
    QVERIFY(mTest->mSoftKeyBack != 0);

}

/*!
 Test ActivateView
*/
void TestMpSettingsView::testActivateView()
{
    //test navigation action is set
    mTest->initializeView();    
    mTest->activateView();
    QCOMPARE(mTest->navigationAction(), mTest->mSoftKeyBack );

}

/*!
test DecativateView
 */
void TestMpSettingsView::testDeactivateView()
{
    //test navigation action is set to 0
    mTest->initializeView();
    mTest->activateView();
    mTest->deactivateView();    
    QVERIFY(mTest->navigationAction()== 0 );
    mTest->deactivateView();


}
/*!
test Back()
 */
void TestMpSettingsView::testBack()
{
    QSignalSpy spy(mTest, SIGNAL(command(int)));
    mTest->back();
    QCOMPARE(spy.count(), 1);

}

