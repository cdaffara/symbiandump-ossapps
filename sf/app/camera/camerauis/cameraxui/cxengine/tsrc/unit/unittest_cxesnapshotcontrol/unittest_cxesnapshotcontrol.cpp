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
#include <QObject>
#include <QTest>
#include <QSignalSpy>
#include <ecam/camerasnapshot.h>

#include "cxefakecameradevice.h"
#include "cxefakecameradevicecontrol.h"
#include "cxeerror.h"
#include "cxetestutils.h"
#include "cxesnapshotcontrol.h"
#include "cxesnapshotcontrolprivate.h"
#include "unittest_cxesnapshotcontrol.h"


UnitTestCxeSnapshotControl::UnitTestCxeSnapshotControl()
    : mSnapshotControl(NULL),
      mCameraDevice(NULL),
      mCameraDeviceControl(NULL)
{
}

UnitTestCxeSnapshotControl::~UnitTestCxeSnapshotControl()
{
    cleanup();
}


void UnitTestCxeSnapshotControl::init()
{
    CX_DEBUG_ENTER_FUNCTION();

    mCameraDeviceControl = new CxeFakeCameraDeviceControl;
    mCameraDevice = new CxeFakeCameraDevice;
    mCameraDevice->newCamera(Cxe::PrimaryCameraIndex, mCameraDeviceControl);
    mCameraDeviceControl->setState(CxeCameraDeviceControl::Ready);

    mSnapshotControl = new CxeSnapshotControl(*mCameraDevice);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSnapshotControl::cleanup()
{
    delete mSnapshotControl;
    mSnapshotControl = NULL;

    delete mCameraDevice;
    mCameraDevice = NULL;

    delete mCameraDeviceControl;
    mCameraDeviceControl = NULL;
}

void UnitTestCxeSnapshotControl::testState()
{
    CX_DEBUG_ENTER_FUNCTION();

    QVERIFY(mSnapshotControl->state() == CxeSnapshotControl::Idle);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSnapshotControl::testCalculateSnapshotSize()
{
    CX_DEBUG_ENTER_FUNCTION();

    QSize snapshotSize;
    QSize displaySize;

    // 16:9 display, 4:3 capture format
    displaySize = QSize(1600, 900);
    snapshotSize = mSnapshotControl->calculateSnapshotSize(displaySize, Cxe::AspectRatio4to3);
    QVERIFY(snapshotSize == QSize(1200, 900));

    // 16:9 display, 16:9 capture format
    displaySize = QSize(640, 360);
    snapshotSize = mSnapshotControl->calculateSnapshotSize(displaySize, Cxe::AspectRatio16to9);
    QVERIFY(snapshotSize == QSize(640, 360));


    // 4:3 display, 16:9 capture format
    displaySize = QSize(640, 480);
    snapshotSize = mSnapshotControl->calculateSnapshotSize(displaySize, Cxe::AspectRatio16to9);
    QVERIFY(snapshotSize == QSize(640, 360));

    // 4:3 display, 4:3 capture format
    displaySize = QSize(640, 480);
    snapshotSize = mSnapshotControl->calculateSnapshotSize(displaySize, Cxe::AspectRatio4to3);
    QVERIFY(snapshotSize == QSize(640, 480));

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSnapshotControl::testStart()
{
    CX_DEBUG_ENTER_FUNCTION();

    QVERIFY(mSnapshotControl->state() == CxeSnapshotControl::Idle);
    mSnapshotControl->start(QSize(640,360));
    QVERIFY(mSnapshotControl->state() == CxeSnapshotControl::Active);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSnapshotControl::testStop()
{
    CX_DEBUG_ENTER_FUNCTION();

    QVERIFY(mSnapshotControl->state() == CxeSnapshotControl::Idle);
    mSnapshotControl->stop();
    QVERIFY(mSnapshotControl->state() == CxeSnapshotControl::Idle);
    mSnapshotControl->start(QSize(640,360));
    QVERIFY(mSnapshotControl->state() == CxeSnapshotControl::Active);
    mSnapshotControl->stop();
    QVERIFY(mSnapshotControl->state() == CxeSnapshotControl::Idle);

    CX_DEBUG_EXIT_FUNCTION();
}

void UnitTestCxeSnapshotControl::testHandleCameraEvent()
{
    CX_DEBUG_ENTER_FUNCTION();

    QSignalSpy spySnapshotReady(mSnapshotControl, SIGNAL(snapshotReady(CxeError::Id, const QImage&)));

    // Snapshot not started, camera events should not effect in any way.
    mSnapshotControl->handleCameraEvent(KUidECamEventSnapshotUidValue, KErrNone);
    QVERIFY(!CxeTestUtils::waitForSignal(spySnapshotReady, 1000));

    // After starting we are supposed to get the snapshotReady signal.
    mSnapshotControl->start(QSize(640,360));
    mSnapshotControl->handleCameraEvent(KUidECamEventSnapshotUidValue, KErrNone);
    QVERIFY(CxeTestUtils::waitForSignal(spySnapshotReady, 1000));

    CX_DEBUG_EXIT_FUNCTION();
}

QTEST_MAIN(UnitTestCxeSnapshotControl);

// end of file
