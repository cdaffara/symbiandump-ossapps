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
#include <hbslider.h>
#include <hbaction.h>
#include "cxuitestbat.h"
#include "cxeengine.h"
#include "cxenamespace.h"
#include "cxevideocapturecontrol.h"
#include "cxeviewfindercontrol.h"
#include "cxuiviewmanager.h"
#include "cxutils.h"
#include "cxuienums.h"
#include "cxuicapturekeyhandler.h"
#include "cxuidocumentloader.h"


// Run before each test
void TestCxUiBat::initTestCase()
{

    mMainWindow = new HbMainWindow;
    mMainWindow->setOrientation(Qt::Horizontal);
    mEngine = CxeEngine::createEngine();

    mKeyHandler = new CxuiCaptureKeyHandler(*mEngine);
    mViewManager = new CxuiViewManager(*mMainWindow, *mEngine, *mKeyHandler);

    mViewManager->prepareWindow();
    mMainWindow->showFullScreen();
}

// Run after each test
void TestCxUiBat::cleanup()
{
    //delete mEngine;
}

// Run after each test
void TestCxUiBat::cleanupTestCase()
{
    delete mMainWindow;
    delete mKeyHandler;
    delete mViewManager;
    //delete mEngine;
}

void TestCxUiBat::initStillImageMode()
{
    mEngine->initMode( Cxe::ImageMode );

}

void TestCxUiBat::sendCameraKeyEvent(QEvent::Type event)
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
    CX_DEBUG_EXIT_FUNCTION();
}

void TestCxUiBat::sendAutofocusKeyEvent(QEvent::Type event)
{
    CX_DEBUG_ENTER_FUNCTION();
    QKeyEvent* autofocusKeyEvent = QKeyEvent::createExtendedKeyEvent(event, Qt::Key_unknown, 0,
            0xe2, 0, 0);
    QApplication::postEvent(mMainWindow, autofocusKeyEvent);
    CX_DEBUG_EXIT_FUNCTION();
}

void TestCxUiBat::initVideoMode()
{
    mEngine->initMode( Cxe::VideoMode );

    QSignalSpy deviceStateSpy(&mEngine->videoCaptureControl(),
            SIGNAL(stateChanged(CxeVideoCaptureControl::State, CxeError::Id)));
    QVERIFY( deviceStateSpy.isValid() );

    QTest::qWait( 3000 );

    QCOMPARE( deviceStateSpy.count(), 3 );

    QList<QVariant> initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Initialized );
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Preparing );
    initModeArguments = deviceStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Ready );
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
}

// Tests capture key in still mode
void TestCxUiBat::testCaptureKeyInImageMode()
    {
    mEngine->initMode(Cxe::ImageMode);
    QTest::qWait(3000);
    // Test if we are in still VF
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::StillPreCaptureView );

    CX_DEBUG(("testCaptureKey in still capture mode"));
    sendCameraKeyEvent();

    QSignalSpy deviceStateSpy(&mEngine->stillCaptureControl(),
            SIGNAL(snapshotReady(int, CxeError::Id)));
    QVERIFY( deviceStateSpy.isValid() );
    QTest::qWait(2000);
    QCOMPARE( deviceStateSpy.count(), 1 );
    if (deviceStateSpy.count() > 0) {
        // Expect zero error code
        QList<QVariant> initModeArguments = deviceStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).toInt() == 0 );
    }
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPostCaptureView );

    CX_DEBUG(("Test to get back viewfinder with capture key"));
    sendCameraKeyEvent();
    QTest::qWait(1000);
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPreCaptureView );

    // Test autofocus with halfkeypress and releasing
    CX_DEBUG(("testAutoFocus_1"));

    QSignalSpy autofocusStateSpy(&mEngine->autoFocusControl(),
            SIGNAL(stateChanged(CxeAutoFocusControl::State, CxeError::Id)));
    QVERIFY( autofocusStateSpy.isValid() );

    // Test autofocus with halfkeypress and canceling it
    sendAutofocusKeyEvent(QEvent::KeyPress);
    CX_DEBUG(("testAutoFocus_2"));
    sendAutofocusKeyEvent(QEvent::KeyRelease);
    QTest::qWait(1000);

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
    CX_DEBUG(("testAutoFocus_3"));

    sendAutofocusKeyEvent(QEvent::KeyPress);
    CX_DEBUG(("Capture autofocused image"));
    QTest::qWait(3000);
    sendCameraKeyEvent();
    sendAutofocusKeyEvent(QEvent::KeyRelease);
    QTest::qWait(1000);

    QCOMPARE( autofocusStateSpy.count(), 2 );
    if (autofocusStateSpy.count() > 0) {
    QList<QVariant> initModeArguments = autofocusStateSpy.takeFirst();
    QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::InProgress );
    QVERIFY( initModeArguments.at(1).toInt() == 0 );
    initModeArguments = autofocusStateSpy.takeAt(0);
    QVERIFY( initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Ready ||
            initModeArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Failed );

    }

    QCOMPARE( deviceStateSpy.count(), 1 );
    if (deviceStateSpy.count() > 0) {
        // Expect zero error code
        QList<QVariant> initModeArguments = deviceStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).toInt() == 0 );
    }
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPostCaptureView );
    }

// Tests capture key in video mode
void TestCxUiBat::testCaptureKeyInVideoMode()
{

    connect(this, SIGNAL(toPrecaptureView()), mViewManager, SLOT(changeToPrecaptureView()));
    QSignalSpy deviceStateSpy(&mEngine->videoCaptureControl(),
            SIGNAL(stateChanged(CxeVideoCaptureControl::State, CxeError::Id)));
    QVERIFY( deviceStateSpy.isValid() );
    emit toPrecaptureView();
    QTest::qWait(1500);
    // Test if we are in still VF
    QVERIFY(mMainWindow->currentViewIndex() == CxUi::VideoPreCaptureView );


    // Generate camera key event to start recording
    CX_DEBUG(("Start video recording"));
    sendCameraKeyEvent();
    QTest::qWait(5000);

    // Generate camera key event to stop recording
    CX_DEBUG(("Stop video recording"));
    sendCameraKeyEvent();
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
    sendCameraKeyEvent();
    QTest::qWait(5000);
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::VideoPreCaptureView );

    QCOMPARE(deviceStateSpy.count(), 3);
    if (deviceStateSpy.count() > 0) {
        QList<QVariant> initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Initialized  );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Preparing );
        initModeArguments = deviceStateSpy.takeAt(0);
        QVERIFY( initModeArguments.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Ready );
        QVERIFY( initModeArguments.at(1).toInt() == 0 );
    }


}

// Tests only that correct view is activated
void TestCxUiBat::testItemVisibilityInPrecaptureview()
{
    CX_DEBUG(("testZoomInPrecaptureview"));
    mEngine->initMode(Cxe::ImageMode);
    emit toPrecaptureView();
    QTest::qWait(1000);
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPreCaptureView );

    QSignalSpy zoomStateSpy(&mEngine->zoomControl(),
            SIGNAL(stateChanged(CxeZoomControl::State, CxeError::Id)));
    QVERIFY( zoomStateSpy.isValid() );

    QSignalSpy zoomLevelStateSpy(&mEngine->zoomControl(),
            SIGNAL(zoomLevelChanged(int)));
    QVERIFY( zoomLevelStateSpy.isValid() );

    CX_DEBUG(("ShowControls"));
    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(180,60),5 );
    QTest::qWait(300);
    QCOMPARE(mMainWindow->isItemVisible(Hb::TitlePaneItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::NaviPaneItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::ToolBarItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::IndicatorItems), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::PrimarySoftKeyItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::SecondarySoftKeyItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::MiddleSoftKeyItem), true);
    QCOMPARE(mMainWindow->isItemVisible(Hb::SoftKeyItems), true);


    CX_DEBUG(("test Zoom slider - touch events control zoom level"));
    QTest::qWait(1000);

    HbAction *zoomAction = qobject_cast<HbAction *>(mViewManager->documentLoader()->findObject("cxui_action_zoom"));
    HbSlider *zoom = qobject_cast<HbSlider *>(mViewManager->documentLoader()->findWidget("zoom"));
    CX_DEBUG_ASSERT(zoomAction);
    CX_DEBUG_ASSERT(zoom);

    // trigger zoom action on toolbar
    zoomAction->trigger();

    // get the zoom position
    QPoint centerPoint = zoom->geometry().toRect().center();
    QPoint firstPoint = zoom->geometry().toRect().topLeft();
    QPoint secondPoint = zoom->geometry().toRect().bottomRight();

    CX_DEBUG( ("zoom slider coordinates topleft(%d,%d) and bottomright(%d,%d)",
            firstPoint.x(), firstPoint.y(), secondPoint.x(), secondPoint.y() ));

    firstPoint = (centerPoint - firstPoint);
    secondPoint = (secondPoint - centerPoint);

    CX_DEBUG( ("dragging zoom from (%d,%d) to (%d,%d)",
            firstPoint.x(), firstPoint.y(),
            secondPoint.x(), secondPoint.y() ));

    // click on zoom
    QTest::mousePress( mMainWindow->viewport(), Qt::LeftButton,0, firstPoint );
    QTest::mouseMove ( mMainWindow->viewport(), secondPoint, 1000 );
    QTest::mouseRelease( mMainWindow->viewport(), Qt::LeftButton,0, secondPoint);
    // At least ZoominIn/ZoominOut and Ready states are set
    QVERIFY(zoomStateSpy.count() > 2 );
    // At least once zoom value has been changed.
    QVERIFY(zoomLevelStateSpy.count() > 2 );

    CX_DEBUG( ("mouse click on (%d,%d)", centerPoint.x(), centerPoint.y() ));
    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton,0, centerPoint);
    // At least ZoominIn/ZoominOut and Ready states are set
    QVERIFY(zoomStateSpy.count() > 2 );
    // At least once zoom value has been changed.
    QVERIFY(zoomLevelStateSpy.count() > 2 );
    QTest::qWait(1000);

    /*CX_DEBUG(("testFlash"));
    QTest::qWait(1000);
    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(35,150));
    QTest::qWait(1000);
    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(185,150));
    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(35,150));
    sendCameraKeyEvent(QEvent::KeyPress);
    sendCameraKeyEvent(QEvent::KeyRelease);*/


    CX_DEBUG(("testGoToVideo"));
    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(35,100), 500 );
    QTest::qWait(3000);
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::VideoPreCaptureView );

    CX_DEBUG(("testGoToStill"));
    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(180,60) );
    QTest::qWait(1000);
    QTest::mouseClick( mMainWindow->viewport(), Qt::LeftButton,0, QPoint(35,100), 500 );
    QTest::qWait(3000);
    QVERIFY( mMainWindow->currentViewIndex()== CxUi::StillPreCaptureView );
}

// main() function non-GUI testing
QTEST_MAIN( TestCxUiBat );
