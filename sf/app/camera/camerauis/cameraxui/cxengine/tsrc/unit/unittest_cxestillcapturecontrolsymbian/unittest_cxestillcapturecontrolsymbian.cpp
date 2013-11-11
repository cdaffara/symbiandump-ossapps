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
#include <QDate>
#include <QTest>
#include <QDebug>
#include <QSignalSpy>

#include "cxetestutils.h"
#include "cxefakecameradevice.h"
#include "cxefakecameradevicecontrol.h"
#include "cxecameradevicecontrol.h"
#include "cxestillcapturecontrolsymbian.h"
#include "unittest_cxestillcapturecontrolsymbian.h"
#include "cxefakeviewfindercontrol.h"
#include "cxesnapshotcontrol.h"
#include "cxesensoreventhandlersymbian.h"
#include "cxefilenamegeneratorsymbian.h"
#include "cxefakefilenamegenerator.h"
#include "cxefakesensoreventhandler.h"
#include "cxesettingsimp.h"
#include "cxefakeautofocuscontrol.h"
#include "cxefakesettings.h"
#include "cxefakequalitypresets.h"
#include "cxefakefilesavethread.h"
#include "cxediskmonitor.h"

UnitTestCxeStillCaptureControlSymbian::UnitTestCxeStillCaptureControlSymbian()
    : mFakeCameraDeviceControl(0)
{
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
}

UnitTestCxeStillCaptureControlSymbian::~UnitTestCxeStillCaptureControlSymbian()
{
    cleanup();
}
void UnitTestCxeStillCaptureControlSymbian::init()
{
}

void UnitTestCxeStillCaptureControlSymbian::cleanup()
{
}

// Run before each individual test case
void UnitTestCxeStillCaptureControlSymbian::initTestCase()
{
    mFakeSettings = new CxeFakeSettings();
    mFakeSettings->set(CxeSettingIds::IMAGE_QUALITY, 0);
    mFakeCameraDeviceControl = new CxeFakeCameraDeviceControl();
    mFakeCameraDevice = new CxeFakeCameraDevice();
    mFakeCameraDevice->newCamera(mFakeCameraDeviceControl->cameraIndex(), mFakeCameraDeviceControl);
    mViewfinderControl = new CxeFakeViewfinderControl();
    mSnapshotControl = new CxeSnapshotControl(*mFakeCameraDevice);
    mFilenameGenerator = new CxeFakeFilenameGenerator();
    mSensorEventHandler = new CxeFakeSensorEventHandler();
    mFakeAutoFocusControl = new CxeFakeAutoFocusControl();
    mFakeQualityPresets = new CxeFakeQualityPresets();
    mFakeFileSaveThread = new CxeFakeFileSaveThread();

    mDiskMonitor = new CxeDiskMonitor(*mFakeSettings);


    mStillCaptureControl = new CxeStillCaptureControlSymbian(
            *mFakeCameraDevice,
            *mViewfinderControl,
            *mSnapshotControl,
            *mFakeCameraDeviceControl,
            *mFilenameGenerator,
            *mSensorEventHandler,
            *mFakeAutoFocusControl,
            *mFakeSettings,
            *mFakeQualityPresets,
            *mFakeFileSaveThread,
            *mDiskMonitor);

    // init supported qualities.
    mStillCaptureControl->supportedImageQualities();

    mSpyState = new QSignalSpy(mStillCaptureControl, SIGNAL(stateChanged(CxeStillCaptureControl::State, CxeError::Id)));
}

// Run after each individual test case
void UnitTestCxeStillCaptureControlSymbian::cleanupTestCase()
{
    delete mFakeSettings;
    mFakeSettings = 0;
    delete mStillCaptureControl;
    mStillCaptureControl = 0;
    delete mSensorEventHandler;
    mSensorEventHandler = 0;
    delete mFilenameGenerator;
    mFilenameGenerator = 0;
    delete mViewfinderControl;
    mViewfinderControl = 0;
    delete mSnapshotControl;
    mSnapshotControl = 0;
    delete mFakeCameraDeviceControl;
    mFakeCameraDeviceControl = 0;
    delete mFakeCameraDevice;
    mFakeCameraDevice = 0;
    delete mFakeAutoFocusControl;
    mFakeAutoFocusControl = 0;
    delete mSpyState;
    mSpyState = 0;
    delete mFakeQualityPresets;
    mFakeQualityPresets = 0;
    delete mDiskMonitor;
    mDiskMonitor = 0;
    delete mFakeFileSaveThread;
    mFakeFileSaveThread = 0;
}


void UnitTestCxeStillCaptureControlSymbian::testCapture()
{
    // deinitialize still capture control
    mStillCaptureControl->deinit();
    // prepare still capture control
    mStillCaptureControl->init();
    QVERIFY(mStillCaptureControl->state() == CxeStillCaptureControl::Ready);
    QVERIFY(mStillCaptureControl->mode() == CxeStillCaptureControl::SingleImageCapture);
    QVERIFY(mStillCaptureControl->imageCount() == 0);

    mStillCaptureControl->capture();
    QVERIFY(mStillCaptureControl->state() == CxeStillCaptureControl::Capturing);
    QVERIFY(CxeTestUtils::waitForSignal(*mSpyState, 1000, 2));
}

void UnitTestCxeStillCaptureControlSymbian::testState()
{
    QVERIFY(mStillCaptureControl->state()); // Passed if not undefined state
}

void UnitTestCxeStillCaptureControlSymbian::testImageCount()
{
    QCOMPARE(mStillCaptureControl->imageCount(), 0);
}

void UnitTestCxeStillCaptureControlSymbian::testReset()
{
    mStillCaptureControl->reset();
    QCOMPARE(mStillCaptureControl->imageCount(), 0);
}

void UnitTestCxeStillCaptureControlSymbian::testCancelAll()
{
    mStillCaptureControl->cancelAll();
    QCOMPARE(mStillCaptureControl->imageCount(), 0);
}

void UnitTestCxeStillCaptureControlSymbian::testSetMode()
{
    QVERIFY(mStillCaptureControl->mode() == CxeStillCaptureControl::SingleImageCapture);
    mStillCaptureControl->setMode(CxeStillCaptureControl::BurstCapture);
    QCOMPARE(mStillCaptureControl->mode(), CxeStillCaptureControl::BurstCapture);
}

void UnitTestCxeStillCaptureControlSymbian::testMode()
{
    QCOMPARE(mStillCaptureControl->mode(), CxeStillCaptureControl::BurstCapture);
}

void UnitTestCxeStillCaptureControlSymbian::testImageDataQueue()
{
    // Passes if not a NULL pointer
    QVERIFY(&mStillCaptureControl->imageDataQueue());
}

void UnitTestCxeStillCaptureControlSymbian::testInit()
{
    QSignalSpy spy(mStillCaptureControl, SIGNAL(imagePrepareComplete(CxeError::Id)));
    mStillCaptureControl->deinit();
    mSpyState->clear();
    QVERIFY(mStillCaptureControl->state() == CxeStillCaptureControl::Uninitialized);
    mStillCaptureControl->init();
    QVERIFY(mStillCaptureControl->state() == CxeStillCaptureControl::Ready);
    QVERIFY(CxeTestUtils::waitForSignal(*mSpyState, 1000));
    QVERIFY(CxeTestUtils::waitForSignal(spy, 1000));
}

void UnitTestCxeStillCaptureControlSymbian::testDeinit()
{
    mStillCaptureControl->init();
    mSpyState->clear();
    QVERIFY(mStillCaptureControl->state() == CxeStillCaptureControl::Ready);
    mStillCaptureControl->deinit();
    QVERIFY(mStillCaptureControl->state() == CxeStillCaptureControl::Uninitialized);
    QVERIFY(CxeTestUtils::waitForSignal(*mSpyState, 1000));
}


void UnitTestCxeStillCaptureControlSymbian::testSupportedImageQualities()
{
    int count = mStillCaptureControl->supportedImageQualities().count();
    QVERIFY(count > 0);
}

// main() function - Need event loop for waiting signals,
// so can't use QTEST_APPLESS_MAIN.
QTEST_MAIN(UnitTestCxeStillCaptureControlSymbian);
