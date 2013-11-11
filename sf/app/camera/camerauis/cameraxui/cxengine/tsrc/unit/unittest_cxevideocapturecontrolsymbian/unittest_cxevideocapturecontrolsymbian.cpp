/*
 * Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "cxetestutils.h"
#include "unittest_cxevideocapturecontrolsymbian.h"
#include "cxevideocapturecontrolsymbian.h"
#include "cxevideocapturecontrolsymbianunit.h"
#include "cxesnapshotcontrol.h"
#include "cxefakefilenamegenerator.h"
#include "cxedummycamera.h"
#include "cxefakecameradevice.h"
#include "cxefakeviewfindercontrol.h"
#include "cxefakecameradevicecontrol.h"
#include "cxefakequalitypresets.h"
#include "cxefakesettings.h"
#include "cxutils.h"
#include "cxediskmonitor.h"


// Enums


UnitTestCxeVideoCaptureControlSymbian::UnitTestCxeVideoCaptureControlSymbian()
    : mCxeVideoCaptureControlSymbian(0)
{
    mSettings = new CxeFakeSettings();
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
    qRegisterMetaType<CxeVideoCaptureControl::State>("CxeVideoCaptureControl::State");
}

UnitTestCxeVideoCaptureControlSymbian::~UnitTestCxeVideoCaptureControlSymbian()
{
    cleanup();
}

// Run before each individual test case
void UnitTestCxeVideoCaptureControlSymbian::init()
{
}

// Run after each individual test case
void UnitTestCxeVideoCaptureControlSymbian::cleanup()
{
}

// Run before each individual test case
void UnitTestCxeVideoCaptureControlSymbian::initTestCase()
{
    CX_DEBUG_ENTER_FUNCTION();
    mCameraDeviceControl = new CxeFakeCameraDeviceControl();
    mCameraDeviceControl->setMode(Cxe::VideoMode);
    mCameraDevice = new CxeFakeCameraDevice();
    mViewfinderControl = new CxeFakeViewfinderControl();
    mSnapshotControl = new CxeSnapshotControl(*mCameraDevice);
    mSettings->set(CxeSettingIds::VIDEO_SHOWCAPTURED, -1);
    mFilenameGeneratorSymbian = new CxeFakeFilenameGenerator();
    mFilenameGeneratorSymbian->init(Cxe::VideoMode);
    mFakeQualityPresets = new CxeFakeQualityPresets();
    mDiskMonitor = new CxeDiskMonitor(*mSettings);
    mCxeVideoCaptureControlSymbian = new CxeVideoCaptureControlSymbianUnit(*mCameraDevice,
                                                                       *mViewfinderControl,
                                                                       *mSnapshotControl,
                                                                       *mCameraDeviceControl,
                                                                       *mFilenameGeneratorSymbian,
                                                                       *mSettings,
                                                                       *mFakeQualityPresets,
                                                                       *mDiskMonitor);
    mCameraDevice->newCamera(mCameraDeviceControl->cameraIndex(), mCameraDeviceControl);

    mSpyState = new QSignalSpy(mCxeVideoCaptureControlSymbian,
                                SIGNAL(stateChanged(
                                CxeVideoCaptureControl::State,
                                CxeError::Id)));


    // Connect video init complete signal to CameraDeviceControl initModeComplete
    connect(mCxeVideoCaptureControlSymbian, SIGNAL(videoPrepareComplete(CxeError::Id)),
             mCameraDeviceControl, SIGNAL(initModeComplete(CxeError::Id)) );

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::cleanupTestCase()
{
    CX_DEBUG_ENTER_FUNCTION();
    delete mCxeVideoCaptureControlSymbian;
    mCxeVideoCaptureControlSymbian = 0;
    delete mFilenameGeneratorSymbian;
    mFilenameGeneratorSymbian = 0;
    delete mViewfinderControl;
    mViewfinderControl = 0;
    delete mSnapshotControl;
    mSnapshotControl = 0;
    delete mCameraDeviceControl;
    mCameraDeviceControl = 0;
    delete mCameraDevice;
    mCameraDevice = 0;
    delete mFakeQualityPresets;
    mFakeQualityPresets = 0;
    delete mDiskMonitor;
    mDiskMonitor = 0;
    delete mSettings;
    mSettings = 0;
    delete mSpyState;
    mSpyState = 0;
    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeVideoCaptureControlSymbian::testState()
{
    CX_DEBUG_ENTER_FUNCTION();
    QVERIFY(mCxeVideoCaptureControlSymbian->state() == CxeVideoCaptureControl::Idle);
    CX_DEBUG_EXIT_FUNCTION();
}



void UnitTestCxeVideoCaptureControlSymbian::testRecord()
{
    CX_DEBUG_ENTER_FUNCTION();

    doPrepareStuff();
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Ready);

    mCxeVideoCaptureControlSymbian->record();
    QTest::qWait(1000); // Wait for recording to start.
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Recording);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testPause()
{
    CX_DEBUG_ENTER_FUNCTION();

    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->record();
    QTest::qWait(1000); // Wait for recording to start.
    mCxeVideoCaptureControlSymbian->pause();
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Paused);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testStop()
{
    CX_DEBUG_ENTER_FUNCTION();

    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->record();
    QTest::qWait(1000); // Wait for recording to start.
    mCxeVideoCaptureControlSymbian->stop();
    QVERIFY((mCxeVideoCaptureControlSymbian->state() == CxeVideoCaptureControl::Stopping)
        || (mCxeVideoCaptureControlSymbian->state() == CxeVideoCaptureControl::Initialized)
        || (mCxeVideoCaptureControlSymbian->state() == CxeVideoCaptureControl::Idle)
        || (mCxeVideoCaptureControlSymbian->state() == CxeVideoCaptureControl::Ready));

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testRemainingTime()
{
    CX_DEBUG_ENTER_FUNCTION();

    int remainingtime = 0;
    mCxeVideoCaptureControlSymbian->remainingTime(remainingtime);
    // not possible to test more.

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testElapsedTime()
{
    CX_DEBUG_ENTER_FUNCTION();

    int elapsedtime = 0;
    bool timefetched = mCxeVideoCaptureControlSymbian->elapsedTime(elapsedtime);
    QVERIFY(timefetched == false);
    // not possible to test more.

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testReset()
{
    CX_DEBUG_ENTER_FUNCTION();

    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->record();
    QTest::qWait(2000);
    mCxeVideoCaptureControlSymbian->stop();
    // just call to see if it doesn't crash
    mCxeVideoCaptureControlSymbian->reset();
    QVERIFY(mCxeVideoCaptureControlSymbian->snapshot().isNull());
    QVERIFY(mCxeVideoCaptureControlSymbian->filename().isEmpty());

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testFilename()
{
    CX_DEBUG_ENTER_FUNCTION();

    doPrepareStuff();
    QString filename = mCxeVideoCaptureControlSymbian->filename();
    QVERIFY(!mCxeVideoCaptureControlSymbian->filename().isEmpty());

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testSnapshot()
{
    CX_DEBUG_ENTER_FUNCTION();

    QCOMPARE(mCxeVideoCaptureControlSymbian->snapshot(), QPixmap());
    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->record();
    QTest::qWait(1000);
    mCxeVideoCaptureControlSymbian->stop();
    // snapshot is not created with dummycamera.
    // snapshot should still be null
    QCOMPARE(mCxeVideoCaptureControlSymbian->snapshot(), QPixmap());

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testHandleSnapshotReady()
{
    CX_DEBUG_ENTER_FUNCTION();
    QImage snapshot;

    QSignalSpy spy(mCxeVideoCaptureControlSymbian, SIGNAL(snapshotReady(CxeError::Id, const QImage&, const QString&)));
    mCxeVideoCaptureControlSymbian->deinit();
    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->handleSnapshotReady(CxeError::OutOfMemory, snapshot);
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Ready);

    // check that we receive this signal once the snapshot is ready
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));

    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->handleSnapshotReady(CxeError::None, snapshot);
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testInit()
{
    CX_DEBUG_ENTER_FUNCTION();

    mCxeVideoCaptureControlSymbian->deinit();
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Idle);
    mCxeVideoCaptureControlSymbian->init();
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(),CxeVideoCaptureControl::Ready);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testDeinit()
{
    CX_DEBUG_ENTER_FUNCTION();

    mCxeVideoCaptureControlSymbian->init();

    QCOMPARE(mCxeVideoCaptureControlSymbian->state(),CxeVideoCaptureControl::Ready);
    mCxeVideoCaptureControlSymbian->deinit();
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(),CxeVideoCaptureControl::Idle);


    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testHandleSoundPlayed()
{
    CX_DEBUG_ENTER_FUNCTION();

    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->handleSoundPlayed();
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Ready);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testMvruoOpenComplete()
{
    CX_DEBUG_ENTER_FUNCTION();

    doPrepareStuff();
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Ready);
    mCxeVideoCaptureControlSymbian->MvruoOpenComplete(CxeError::NotSupported);
    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->MvruoOpenComplete(CxeError::None);
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Ready);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testMvruoPrepareComplete()
{
    CX_DEBUG_ENTER_FUNCTION();

    // since we can't go to preparing state using fake classes
    // and current implementation of CxeVideoCaptureControlSymbian
    // we just check that it doesn't crash, or change the state
    // unnecessarily.
    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->MvruoPrepareComplete(-5);
    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->MvruoPrepareComplete(0);
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Ready);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testMvruoRecordComplete()
{
    CX_DEBUG_ENTER_FUNCTION();

    mCxeVideoCaptureControlSymbian->MvruoRecordComplete(-5);
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Idle);
    doPrepareStuff();
    mCxeVideoCaptureControlSymbian->MvruoRecordComplete(0);
    QCOMPARE(mCxeVideoCaptureControlSymbian->state(), CxeVideoCaptureControl::Ready);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeVideoCaptureControlSymbian::testMvruoEvent()
{
    CX_DEBUG_ENTER_FUNCTION();

    QSignalSpy spy(mCxeVideoCaptureControlSymbian, SIGNAL(videoComposed(CxeError::Id, const QString&)));

    const TUid KCamCControllerCCVideoRecordStopped = {0x2000E546};
    const TUid KCamCControllerCCVideoFileComposed = {0x2000E547};

    TMMFEvent event;
    event.iEventType.iUid = KCamCControllerCCVideoRecordStopped.iUid;
    mCxeVideoCaptureControlSymbian->MvruoEvent(event);
    event.iEventType.iUid = KCamCControllerCCVideoFileComposed.iUid;
    mCxeVideoCaptureControlSymbian->MvruoEvent(event);
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));

    CX_DEBUG_EXIT_FUNCTION();
}


void UnitTestCxeVideoCaptureControlSymbian::doPrepareStuff()
{
    CX_DEBUG_ENTER_FUNCTION();

    mCxeVideoCaptureControlSymbian->init();
    CX_DEBUG((("QTest::qWait(100)")));
    QTest::qWait(100);

    CX_DEBUG_EXIT_FUNCTION();
}


// main() function non-GUI testing
QTEST_MAIN(UnitTestCxeVideoCaptureControlSymbian);

// end of file

