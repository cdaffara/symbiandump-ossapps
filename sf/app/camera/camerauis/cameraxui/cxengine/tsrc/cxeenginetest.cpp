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
#include "cxutils.h"
#include "cxeenginetest.h"
#include "cxeengine.h"
#include "cxecameradevicecontrol.h"
#include "cxestillcapturecontrol.h"
#include "cxevideocapturecontrol.h"
#include "cxeviewfindercontrol.h"
#include "cxeautofocuscontrol.h"
#include "cxeerror.h"

using namespace Cxe;

/**
 * void TestCxeEngine::init()
 * Helper function to initialize the engine.
 * Run at the start of each test case.
 */
void TestCxeEngine::init()
{
    // Commented out because engine deletion fails -> engine created only once
    // mEngine = CxeEngine::createEngine();
}

/**
 * void TestCxeEngine::initTestCase()
 * Tests that the engine initialization works properly
 */
void TestCxeEngine::initTestCase()
{
    mWindow = new HbMainWindow();
    mEngine = CxeEngine::createEngine();

    mWindow->setOrientation(Qt::Horizontal);
    mWindow->showFullScreen();

    // For viewfinder to be started, window needs to be set
    mEngine->viewfinderControl().setWindow( mWindow->effectiveWinId());
}

/**
 * Cleans up the engine.
 * Run after each test case.
 */
void TestCxeEngine::cleanup()
{
    // commented out because engine delete fails
    // delete mEngine;
    // mEngine = NULL;
}

/**
 * void TestCxeEngine::cleanupTestCase()
 * Cleans up the engine and rest of the test environment.
 * NOTE: This should be the last test case to be executed
 */
void TestCxeEngine::cleanupTestCase()
{
    delete mEngine;
    mEngine = NULL;
    delete mWindow;
    mWindow = NULL;
}

/**
 * void TestCxeEngine::createAndDestroyEngine()
 * Tests that the engine construction and deletion works without errors.
 */
void TestCxeEngine::createAndDestroyEngine()
{
    // Do nothing here... just test that we can create and delete the engine
    // instance without problems.
    QVERIFY( mEngine != NULL );
}

/**
 * void TestCxeEngine::initStillImageMode()
 * Initializes the still image mode.
 * NOTE: No initialization and cleanup, so that it can be used in test cases
 * requiring still image mode to be ready.
 */
void TestCxeEngine::initStillImageMode()
{
    CX_DEBUG_ENTER_FUNCTION();
    CxeCameraDeviceControl &deviceControl = mEngine->cameraDeviceControl();
    CxeStillCaptureControl &stillCaptureControl = mEngine->stillCaptureControl();

    QSignalSpy deviceStateSpy(&deviceControl, SIGNAL(initModeComplete(CxeError::Id)));
    QSignalSpy stillStateSpy(&stillCaptureControl, SIGNAL(stateChanged(CxeStillCaptureControl::State, CxeError::Id)));

    // Check that the signals were connected withtout errors
    QVERIFY( deviceStateSpy.isValid() );
    QVERIFY( stillStateSpy.isValid() );

    if (stillCaptureControl.state()!=CxeStillCaptureControl::Ready) {
        mEngine->initMode(ImageMode);

        // Wait for a maximum of 7 seconds for the operation to finish
        int waitCount = 0;
        while (deviceStateSpy.count() < 1) {
            QTest::qWait(1000); // wait one second
            waitCount++;
            if (waitCount >= 7) {
                QFAIL("initModeComplete timed out");
            }
        }

        // Wait for any unexpected extra signals
        QTest::qWait(2000);

        // Check signal lists... Expect exactly one initModeComplete signal
        QCOMPARE( deviceStateSpy.count(), 1 );

        // Expect zero error code
        QList<QVariant> initModeArguments = deviceStateSpy.takeFirst();
        QVERIFY( initModeArguments.at(0).toInt() == 0 ); // CxeError::None

        // Check signal changes for still capture control
        // Just 1 state change signal expected: (Uninitialized ->) Ready
        /*
        int count = stillStateSpy.count();
        for (int i=0; i<count ; i++) {
            QList<QVariant> capArgs = stillStateSpy.takeAt(0);
            qDebug("State:%d, err:%d", capArgs.at(0).value<CxeStillCaptureControl::State> (),
                    capArgs.at(1).toInt());
        }
        */

        QCOMPARE( stillStateSpy.count(), 1 );
        QList<QVariant> initArgs = stillStateSpy.takeFirst();
        QVERIFY( initArgs.at(0).value<CxeStillCaptureControl::State>() == CxeStillCaptureControl::Ready );
        QVERIFY( initArgs.at(1).toInt() == 0 );

        //! @todo: check devicecontrolstate and stillcapturestate
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * void TestCxeEngine::testAutoFocusBasic()
 * Test that AF either succeeds or fails when attempted.
 */
void TestCxeEngine::testAutoFocusBasic()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Initialize before every test case
    init();

    // Initialize still mode - other details tested internally
    initStillImageMode();

    // Actual test of autofocus
    CxeAutoFocusControl &afControl = mEngine->autoFocusControl();
    QSignalSpy afDeviceStateSpy(&afControl, SIGNAL(stateChanged(CxeAutoFocusControl::State, CxeError::Id)) );
    QVERIFY( afDeviceStateSpy.isValid() );

    // Wait half a second before starting AF
    QTest::qWait(500);
    afControl.start();
    QTest::qWait(3000);
    afControl.cancel();

    // Wait for signals
    QTest::qWait(2000);
    qDebug("AF signals:%d", afDeviceStateSpy.count());

    int count = afDeviceStateSpy.count();
    for (int i=0; i<count; i++) {
        QList<QVariant> afArgs = afDeviceStateSpy.takeAt(0);
        qDebug("State:%d, err:%d", afArgs.at(0).value<CxeAutoFocusControl::State> (),
                afArgs.at(1).toInt());
    }

    /*
    QCOMPARE( afDeviceStateSpy.count(), 2 );

    QList<QVariant> afArguments = afDeviceStateSpy.takeAt(0);
    QVERIFY( afArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::InProgress );
    QVERIFY( afArguments.at(1).toInt() == 0 );

    afArguments = afDeviceStateSpy.takeAt(0);
    QVERIFY( afArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Ready ||
             afArguments.at(0).value<CxeAutoFocusControl::State>() == CxeAutoFocusControl::Failed );
    QVERIFY( afArguments.at(1).toInt() == 0 );
    */

    // Cleanup after every test case
    cleanup();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * void TestCxeEngine::testImageCapture()
 * Tests that image capture works properly.
 */
void TestCxeEngine::testImageCapture()
{
    CX_DEBUG_ENTER_FUNCTION();
    // Initialize before every test case
    init();

    // Initialize still mode - other details tested internally
    initStillImageMode();

    // Init signal spy for still capture control
    CxeStillCaptureControl &stillCaptureControl = mEngine->stillCaptureControl();
    QSignalSpy stillStateSpy(&stillCaptureControl, SIGNAL(stateChanged(CxeStillCaptureControl::State, CxeError::Id)));
    QSignalSpy stillSnapSpy(&stillCaptureControl, SIGNAL(snapshotReady(int, CxeError::Id)));
    QSignalSpy stillImageSpy(&stillCaptureControl, SIGNAL(imageCaptured(CxeError::Id)));

    // Check that the signals were connected withtout errors
    QVERIFY( stillStateSpy.isValid() );
    QVERIFY( stillSnapSpy.isValid() );
    QVERIFY( stillImageSpy.isValid() );

    // Wait half a second before starting capture
    CX_DEBUG(("Waiting before starting capture"));
    QTest::qWait(500);
    stillCaptureControl.capture();
    CX_DEBUG(("Capture command executed"));

    // Wait for signals and saving to complete
    QTest::qWait(5000);

    // Confirm that one snapshot signal and one image captured signal are received
    QCOMPARE( stillSnapSpy.count(), 1 );
    QCOMPARE( stillImageSpy.count(), 1 );

    // 2 Signal expected: Capturing -> Ready/Uninitialized
    qDebug("Still capture control state signals:%d", stillStateSpy.count());
    QCOMPARE( stillStateSpy.count(), 2 );

    QList<QVariant> capArguments = stillStateSpy.takeAt(0);
    QVERIFY( capArguments.at(0).value<CxeStillCaptureControl::State>() == CxeStillCaptureControl::Capturing );
    QVERIFY( capArguments.at(1).toInt() == 0 );
    
    capArguments = stillStateSpy.takeAt(0);
    QVERIFY( capArguments.at(0).value<CxeStillCaptureControl::State>() == CxeStillCaptureControl::Ready );
    QVERIFY( capArguments.at(1).toInt() == 0 );

    // Cleanup after every test case
    cleanup();
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * void TestCxeEngine::testBatchCapture()
 * Test that a number of images can be captured.
 */
void TestCxeEngine::testBatchCapture()
{
    CX_DEBUG_ENTER_FUNCTION();
    for(int i=0; i<3; i++) {
        testImageCapture();
        mEngine->stillCaptureControl().reset();
    }
    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * void TestCxeEngine::initVideoMode()
 * Initializes the video mode.
 * NOTE: No initialization and cleanup in done here, so that it can be used in
 * test cases requiring video mode to be ready.
 */
void TestCxeEngine::initVideoMode()
{
    CX_DEBUG_ENTER_FUNCTION();

    QSignalSpy deviceStateSpy(&mEngine->videoCaptureControl(), SIGNAL(stateChanged(CxeVideoCaptureControl::State, CxeError::Id)));
    QVERIFY( deviceStateSpy.isValid() );

    if (mEngine->videoCaptureControl().state()!=CxeVideoCaptureControl::Ready) {
        // Call initialize and wait 500ms for signals
        mEngine->initMode(VideoMode);
        QTest::qWait(500);

        // 3 state change signals expected: Open -> Preparing -> Ready
        QCOMPARE( deviceStateSpy.count(), 3 );

        /*
        int count = deviceStateSpy.count()
        for (int i=0; i<count ; i++) {
            QList<QVariant> capArgs = deviceStateSpy.takeAt(0);
            qDebug("State:%d, err:%d", capArgs.at(0).value<CxeVideoCaptureControl::State> (),
                    capArgs.at(1).toInt());
        }
        */

        QList<QVariant> initArgs = deviceStateSpy.takeAt(0);
        QVERIFY(initArgs.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Initialized);
        QVERIFY( initArgs.at(1).toInt() == 0 );

        initArgs = deviceStateSpy.takeAt(0);
        QVERIFY( initArgs.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Preparing );
        QVERIFY( initArgs.at(1).toInt() == 0 );

        initArgs = deviceStateSpy.takeAt(0);
        QVERIFY( initArgs.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Ready );
        QVERIFY( initArgs.at(1).toInt() == 0 );
    }

    CX_DEBUG_EXIT_FUNCTION();
}

/**
 * void TestCxeEngine::testVideoCapture()
 * Tests that video capture works properly.
 */
void TestCxeEngine::testVideoCapture()
{
    CX_DEBUG_ENTER_FUNCTION();

    // Initialize before every test case
    init();

    // Initialize the video mode
    initVideoMode();

    // Init signal spy for still capture control
    CxeVideoCaptureControl &videoCaptureControl = mEngine->videoCaptureControl();
    QSignalSpy videoStateSpy(&videoCaptureControl, SIGNAL(stateChanged(CxeVideoCaptureControl::State, CxeError::Id)));
    QSignalSpy videoSnapSpy(&videoCaptureControl, SIGNAL(snapshotReady(CxeError::Id)));
    QSignalSpy videoSpy(&videoCaptureControl, SIGNAL(videoComposed()));

    // Check that the signals were connected withtout errors
    QVERIFY( videoStateSpy.isValid() );
    QVERIFY( videoSnapSpy.isValid() );
    QVERIFY( videoSpy.isValid() );

    // Wait half a second before starting recording
    QTest::qWait(500);
    videoCaptureControl.record();

    // Wait for 2 seconds before stopping video
    QTest::qWait(2000);
    videoCaptureControl.stop();

    // Wait some time for video to stop and signals to arrive
    QTest::qWait(2000);

    // Expected state sequence: PlayingStartSound -> Recording -> Stopping -> Idle/Ready
    qDebug("Video capture control state signals:%d", videoStateSpy.count());
    QCOMPARE(videoStateSpy.count(), 4);

    QList<QVariant> stateArgs = videoStateSpy.takeAt(0);
    QVERIFY( stateArgs.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::PlayingStartSound );
    QVERIFY( stateArgs.at(1).toInt() == 0 );

    stateArgs = videoStateSpy.takeAt(0);
    QVERIFY( stateArgs.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Recording );
    QVERIFY( stateArgs.at(1).toInt() == 0 );
    
    stateArgs = videoStateSpy.takeAt(0);
    QVERIFY( stateArgs.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Stopping );
    QVERIFY( stateArgs.at(1).toInt() == 0 );
    
    stateArgs = videoStateSpy.takeAt(0);
    QVERIFY( stateArgs.at(0).value<CxeVideoCaptureControl::State>() == CxeVideoCaptureControl::Idle );
    QVERIFY( stateArgs.at(1).toInt() == 0 );
    
    /*
    int count = videoStateSpy.count();
    for (int i=0; i<count; i++) {
        QList<QVariant> capArgs = videoStateSpy.takeAt(0);
        qDebug("State:%d, err:%d", capArgs.at(0).value<CxeVideoCaptureControl::State> (),
                capArgs.at(1).toInt());
    }
    */

    // Confirm that one snapshot signal and one video composed signal is received
    QCOMPARE( videoSnapSpy.count(), 1 );
    QCOMPARE( videoSpy.count(), 1 );

    // Cleanup after every test case
    cleanup();
    CX_DEBUG_EXIT_FUNCTION();
}

/*
void TestCxeEngine::testTemplate()
{
    // Initialize before every test case
    init();

    // Initialize the image mode
    initTestCase();

    // Cleanup after every test case
    cleanup();
}
*/

// main() function non-GUI testing
QTEST_MAIN( TestCxeEngine );
