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

#include "unittest_cxeenginesymbian.h"
#include "cxeenginesymbianunit.h"
#include "cxefakecameradevicecontrol.h"
#include "cxefakestillcapturecontrol.h"
#include "cxefakevideocapturecontrol.h"
#include "cxefakeviewfindercontrol.h"
#include "cxetestutils.h"
#include "cxutils.h"

UnitTestCxeEngineSymbian::UnitTestCxeEngineSymbian()
    : mEngine(NULL)
{
}

UnitTestCxeEngineSymbian::~UnitTestCxeEngineSymbian()
{
    delete mEngine;
}

// Run before each individual test case
void UnitTestCxeEngineSymbian::init()
{
    delete mEngine;
    mEngine = NULL;
    mEngine = new CxeEngineSymbianUnit();
    mEngine->construct();
    mEngine->initMode(Cxe::ImageMode);

    mCameraDeviceControl = qobject_cast<CxeFakeCameraDeviceControl*>(
            &mEngine->cameraDeviceControl());
    mStillCaptureControl = qobject_cast<CxeFakeStillCaptureControl*>(
            &mEngine->stillCaptureControl());
    mVideoCaptureControl = qobject_cast<CxeFakeVideoCaptureControl*>(
            &mEngine->videoCaptureControl());
    mViewfinderControl = qobject_cast<CxeFakeViewfinderControl*>(
            &mEngine->viewfinderControl());
}

// Run after each individual test case
void UnitTestCxeEngineSymbian::cleanup()
{
    delete mEngine;
    mEngine = NULL;
    mCameraDeviceControl = NULL;
    mStillCaptureControl = NULL;
    mVideoCaptureControl = NULL;
    mViewfinderControl = NULL;
}

void UnitTestCxeEngineSymbian::testCameraDeviceControl()
{
    QVERIFY(&mEngine->cameraDeviceControl());
}

void UnitTestCxeEngineSymbian::testViewfinderControl()
{
    QVERIFY(&mEngine->viewfinderControl());
}

void UnitTestCxeEngineSymbian::testStillCaptureControl()
{
    QVERIFY(&mEngine->stillCaptureControl());
}

void UnitTestCxeEngineSymbian::testVideoCaptureControl()
{
    QVERIFY(&mEngine->videoCaptureControl());
}

void UnitTestCxeEngineSymbian::testAutoFocusControl()
{
    QVERIFY(&mEngine->autoFocusControl());
}

void UnitTestCxeEngineSymbian::testZoomControl()
{
    QVERIFY(&mEngine->zoomControl());
}

void UnitTestCxeEngineSymbian::testSettings()
{
    QVERIFY(&mEngine->settings());
}

void UnitTestCxeEngineSymbian:: testSensorEventHandler()
{
    QVERIFY(&mEngine->sensorEventHandler());
}

void UnitTestCxeEngineSymbian::testFeatureManager()
{
    QVERIFY(&mEngine->featureManager());
}

void UnitTestCxeEngineSymbian::testMode()
{
    QCOMPARE(mEngine->mode(), Cxe::ImageMode);
}

/*!
    Test that initMode() changes current mode
*/
void UnitTestCxeEngineSymbian::testInitMode1()
{
    mEngine->initMode(Cxe::VideoMode);
    QCOMPARE(mEngine->mode(), Cxe::VideoMode);

    mEngine->initMode(Cxe::ImageMode);
    QCOMPARE(mEngine->mode(), Cxe::ImageMode);
}

/*!
    Test that initMode() calls reserve when needed
*/
void UnitTestCxeEngineSymbian::testInitMode2()
{
    mCameraDeviceControl->setState(CxeCameraDeviceControl::Idle);
    mEngine->initMode(Cxe::ImageMode);

    // Verify that reseve() has been called using device control call history
    QVERIFY(mCameraDeviceControl->callHistory().indexOf(
            CxeFakeCameraDeviceControl::Reserve) >= 0);
}

/*!
    Test that deinit() and init() are called correctly when switching
    to video mode
*/
void UnitTestCxeEngineSymbian::testInitMode3()
{
    mCameraDeviceControl->setState(CxeCameraDeviceControl::Ready);
    mStillCaptureControl->setState(CxeStillCaptureControl::Ready);
    mVideoCaptureControl->setState(CxeVideoCaptureControl::Idle);
    mEngine->initMode(Cxe::VideoMode);

    // Verify that init() and deinit() have been called using call histories
    QVERIFY(mStillCaptureControl->callHistory().indexOf(
            CxeFakeStillCaptureControl::Deinit) >= 0);
    QVERIFY(mVideoCaptureControl->callHistory().indexOf(
            CxeFakeVideoCaptureControl::Init) >= 0);

    // Verify that viewfinder was stopped
    QVERIFY(mViewfinderControl->callHistory().indexOf(
            CxeFakeViewfinderControl::Stop) >= 0);
}

/*!
    Test that deinit() and init() are called correctly when switching
    to still image mode
*/
void UnitTestCxeEngineSymbian::testInitMode4()
{
    // First set the engine to video mode that a mode switch is really needed
    mEngine->initMode(Cxe::VideoMode);
    mCameraDeviceControl->setState(CxeCameraDeviceControl::Ready);
    mStillCaptureControl->setState(CxeStillCaptureControl::Uninitialized);
    mVideoCaptureControl->setState(CxeVideoCaptureControl::Initialized);

    mEngine->initMode(Cxe::ImageMode);

    // Verify that init() and deinit() have been called using call histories
    QVERIFY(mVideoCaptureControl->callHistory().indexOf(
            CxeFakeVideoCaptureControl::Deinit) >= 0);
    QVERIFY(mStillCaptureControl->callHistory().indexOf(
            CxeFakeStillCaptureControl::Init) >= 0);

    // Verify that viewfinder was stopped
    QVERIFY(mViewfinderControl->callHistory().indexOf(
            CxeFakeViewfinderControl::Stop) >= 0);
}


// main() function
QTEST_MAIN(UnitTestCxeEngineSymbian);
