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
#include "cxutils.h"
//#include "cxuiviewmanager.h"
#include "cxuifakeengine.h"
#include "cxuitest.h"
#include "cxuienums.h"
//#include "cxeviewfindercontrol.h"


// Tests only that correct view is activated
void TestCxUi::testItemVisibilityInPrecaptureview()
    {
    CX_DEBUG(("testZoomInPrecaptureview"));
    mEngine->initMode(Cxe::ImageMode);
    emit toPrecaptureView();
    QTest::qWait(1000);
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPreCaptureView );
    //QVERIFY(mMainWindow->visibleItems().count() == 0);
    QTest::mousePress( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(180,60), 5 );
    //QVERIFY(mMainWindow->visibleItems().count() == 7);
    //mMainWindow->visibleItems();
    QCOMPARE(mMainWindow->isItemVisible(Hb::TitlePaneItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::NaviPaneItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::ToolBarItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::IndicatorItems), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::PrimarySoftKeyItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::SecondarySoftKeyItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::MiddleSoftKeyItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::SoftKeyItems), true);
    QTest::mousePress( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(35,150), 5 );
    QCOMPARE(mMainWindow->visibleItems(), Hb::AllItems);
    QCOMPARE(mMainWindow->isItemVisible(Hb::TitlePaneItem), false);
 
    }

// Tests only that correct view is activated
void TestCxUi::testViewManager()
    {
    CX_DEBUG(("testViewManager"));
    mEngineFake->initModeFake(Cxe::ImageMode);

    connect(this, SIGNAL(toPostcaptureView()), mViewManager, SLOT(changeToPostView()));
    connect(this, SIGNAL(toPrecaptureView()), mViewManager, SLOT(changeToPreView()));

    // Test if we are in still VF
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::StillPreCaptureView );

    emit toPostcaptureView();
    // Test if image review activated
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPostCaptureView );

    emit toPrecaptureView();
    // Test if still VF view active
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPreCaptureView );

    mEngineFake->initModeFake(Cxe::VideoMode);
    emit toPrecaptureView();
    // Test if video VF view active
    QVERIFY( mMainWindow->currentViewIndex() == CxUi::VideoPreCaptureView );

    emit toPostcaptureView();
    // Test if video review active
    QVERIFY( mMainWindow->currentViewIndex() == CxUi::VideoPostCaptureView );

    emit toPrecaptureView();
    // Test if video VF view active
    QVERIFY( mMainWindow->currentViewIndex() == CxUi::VideoPreCaptureView ); 

    // Go back to original state...
    mEngineFake->initModeFake(Cxe::ImageMode);
    emit toPrecaptureView();
    QTest::qWait(500);
    }

// Tests capture key in still mode
/*
void TestCxUi::testCaptureKeyInImageMode()
    {
    mEngine->initMode(Cxe::ImageMode);
    QTest::qWait(1000);
    // Test if we are in still VF
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::StillPreCaptureView );    
    
    CX_DEBUG(("testCaptureKey in still capture mode"));
    
    QKeyEvent* captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QSignalSpy deviceStateSpy(&mEngine->stillCaptureControl(), 
            SIGNAL(snapshotReady(int)));    
    QVERIFY( deviceStateSpy.isValid() );        
    QTest::qWait(3000);
    QCOMPARE( deviceStateSpy.count(), 1 );
    if (deviceStateSpy.count() > 0) {
        // Expect zero error code
        QList<QVariant> initModeArguments = deviceStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).toInt() == 0 );
    }    
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPostCaptureView );
    
    CX_DEBUG(("Test to get back viewfinder with capture key"));
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, Qt::Key_unknown, 0, 0xab, 0,0);    
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(1000); 
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPreCaptureView );
    
    // Test autofocus with halfkeypress and releasing
    CX_DEBUG(("testAutoFocus_1"));
    
    QSignalSpy autofocusStateSpy(&mEngine->autoFocusControl(), 
            SIGNAL(stateChanged(CxeAutoFocusControl::State,int)));          
    QVERIFY( autofocusStateSpy.isValid() );     
    
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, 
            Qt::Key_unknown, 
            0, 
            0xe2, 
            0,
            0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    
    CX_DEBUG(("testAutoFocus cancel"));
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, 
            Qt::Key_unknown, 
            0, 
            0xe2, 
            0,
            0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);       
    QTest::qWait(100);
    
    QCOMPARE( autofocusStateSpy.count(), 3 );
    if (autofocusStateSpy.count() > 0) {
        QList<QVariant> initModeArguments = autofocusStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::InProgress );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        CX_DEBUG(("testAutoFocus_2a"));
        initModeArguments = autofocusStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Canceling );
        initModeArguments = autofocusStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Unknown );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );  
    }    

    // Test autofocus with halfkeypress and capture focused image
    CX_DEBUG(("testAutoFocus_2"));

    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, 
            Qt::Key_unknown, 
            0, 
            0xe2, 
            0,
            0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(5000);   

    QCOMPARE( autofocusStateSpy.count(), 2 );
    if (autofocusStateSpy.count() > 0) {
    QList<QVariant> initModeArguments = autofocusStateSpy.takeFirst();
    QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::InProgress );
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    initModeArguments = autofocusStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Ready || 
            initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Failed );

    }    

    CX_DEBUG(("Capture autofocused image"));
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, 
            Qt::Key_unknown, 
            0, 
            0xab, 
            0,
            0);
    QApplication::postEvent(mMainWindow, captureKeyEvent); 
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(5000);

    QCOMPARE( deviceStateSpy.count(), 1 );
    if (deviceStateSpy.count() > 0) {
        // Expect zero error code
        QList<QVariant> initModeArguments = deviceStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).toInt() == 0 );
    }    
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPostCaptureView );
    
    }
    */
/*
// Tests capture key in video mode
void TestCxUi::testCaptureKeyInVideoMode()
    { 
    connect(this, SIGNAL(toPrecaptureView()), mViewManager, SLOT(changeToPreView()));
    QSignalSpy deviceStateSpy(&mEngine->videoCaptureControl(), 
            SIGNAL(stateChanged(CxeVideoCaptureControl::State, int)));    
    QVERIFY( deviceStateSpy.isValid() ); 
    emit toPrecaptureView();
    QTest::qWait(500);
    // Test if we are in still VF
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::VideoPreCaptureView );

    // Generate camera key event to start recording
    CX_DEBUG(("Start video recording"));
    QKeyEvent* captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, 
            Qt::Key_unknown, 
            0, 
            0xab, 
            0,
            0);    

    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(5000);

    // Generate camera key event to stop recording
    CX_DEBUG(("Stop video recording"));
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, 
            Qt::Key_unknown, 
            0, 
            0xab, 
            0,
            0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(3000);

    QCOMPARE( deviceStateSpy.count(), 4 );

    QList<QVariant> initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::PlayingStartSound );
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Recording );
    initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Stopping );
    initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Idle );
    QVERIFY( initModeArguments.at(1).toInt() == 0 );  

    QVERIFY( mMainWindow->currentViewIndex()== CxUi::VideoPostCaptureView );

    CX_DEBUG(("Test to get back viewfinder with capture key"));
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyPress, Qt::Key_unknown, 0, 0xab, 0,0);    
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    captureKeyEvent = QKeyEvent::createExtendedKeyEvent(
            QEvent::KeyRelease, Qt::Key_unknown, 0, 0xab, 0,0);
    QApplication::postEvent(mMainWindow, captureKeyEvent);
    QTest::qWait(5000); 
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::VideoPreCaptureView );

    QCOMPARE(deviceStateSpy.count(), 3);
    if (deviceStateSpy.count() > 0) {        
        QList<QVariant> initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Open );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Preparing );
        initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Ready );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );  
    }   

    }
    */
