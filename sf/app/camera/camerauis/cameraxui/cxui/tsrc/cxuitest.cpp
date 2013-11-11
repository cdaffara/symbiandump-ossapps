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
#include <QTest>
#include <QSignalSpy>
#include <hbmainwindow.h>
#include "cxuitest.h"
#include "cxeengine.h"
#include "cxenamespace.h"
#include "cxevideocapturecontrol.h"
#include "cxeviewfindercontrol.h"
#include "cxuiviewmanagertest.h"
#include "cxuiviewmanager.h"
#include "cxutils.h"
#include "cxuienums.h"
#include "cxuicapturekeyhandler.h"


// Run before each test
void TestCxUi::initTestCase()
{    
    mMainWindow = new HbMainWindow;    
    mEngineFake = CxuiFakeEngine::createFakeEngine();  
    
    mKeyHandler = new CxuiCaptureKeyHandler(*mEngineFake);    
    mViewManager = new CxuiViewManager(*mMainWindow, *mEngineFake, *mKeyHandler);
    mViewManager->prepareWindow();
}

// Run after each test
void TestCxUi::cleanup()
{
    //delete mEngineFake;
}

// Run after each test
void TestCxUi::cleanupTestCase()
{
    //delete mEngine;
    //delete mMainWindow;
}

void TestCxUi::initVideoMode()
{
    /*mEngineFake->initMode( Cxe::VideoMode );
    
    QSignalSpy deviceStateSpy(&mEngineFake->videoCaptureControl(), 
            SIGNAL(stateChanged(CxeVideoCaptureControl::State, int)));    
    QVERIFY( deviceStateSpy.isValid() ); 
    
    // Wait 500ms for signals
    QTest::qWait( 5000 );

    QCOMPARE( deviceStateSpy.count(), 3 );
    
    QList<QVariant> initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Open );
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Preparing );
    initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Ready );
    QVERIFY( initModeArguments.at(1).toInt() == 0 );  */
}

void TestCxUi::sendCameraKeyEvent(QEvent::Type event)
{
    CX_DEBUG_ENTER_FUNCTION(); 
    QKeyEvent* captureKeyEvent;
    if (event == QEvent::None) {
        captureKeyEvent = QKeyEvent::createExtendedKeyEvent(QEvent::KeyPress, Qt::Key_unknown, 0,
                0xab, 0, 0);
        QApplication::postEvent(mMainWindow, captureKeyEvent);
        captureKeyEvent = QKeyEvent::createExtendedKeyEvent(QEvent::KeyRelease, Qt::Key_unknown, 0,
                0xab, 0, 0);
        QApplication::postEvent(mMainWindow, captureKeyEvent);
        
    } else {
        captureKeyEvent = QKeyEvent::createExtendedKeyEvent(event, Qt::Key_unknown, 0, 0xab, 0, 0);
        QApplication::postEvent(mMainWindow, captureKeyEvent);
    }
    QTest::qWait(100);
    CX_DEBUG_EXIT_FUNCTION();  
}

void TestCxUi::sendAutofocusKeyEvent(QEvent::Type event)
{
    CX_DEBUG_ENTER_FUNCTION();
    QKeyEvent* autofocusKeyEvent = QKeyEvent::createExtendedKeyEvent(event, Qt::Key_unknown, 0,
            0xe2, 0, 0);
    QApplication::postEvent(mMainWindow, autofocusKeyEvent);
    QTest::qWait(50);
    CX_DEBUG_EXIT_FUNCTION();  
}

// main() function non-GUI testing
QTEST_MAIN( TestCxUi );
