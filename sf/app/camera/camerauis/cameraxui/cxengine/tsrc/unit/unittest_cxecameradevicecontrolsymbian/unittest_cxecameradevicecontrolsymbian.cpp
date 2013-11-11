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
#include <QTimer>
#include <QSignalSpy>

#include "unittest_cxecameradevicecontrolsymbian.h"
#include "cxecameradevicecontrolunit.h"
#include "cxetestutils.h"
#include "cxutils.h"

UnitTestCxeCameraDeviceControlSymbian::UnitTestCxeCameraDeviceControlSymbian()
    : mDeviceControl(NULL)
{
    qRegisterMetaType<CxeError::Id>("CxeError::Id");
}

UnitTestCxeCameraDeviceControlSymbian::~UnitTestCxeCameraDeviceControlSymbian()
{
    delete mDeviceControl;
}

// Run before each individual test case
void UnitTestCxeCameraDeviceControlSymbian::init()
{
    delete mDeviceControl;
    mDeviceControl = NULL;
    mDeviceControl = new CxeCameraDeviceControlUnit();
    mDeviceControl->init();
}

// Run after each individual test case
void UnitTestCxeCameraDeviceControlSymbian::cleanup()
{
    delete mDeviceControl;
    mDeviceControl = NULL;
}

void UnitTestCxeCameraDeviceControlSymbian::testMode()
{
    // Image mode is the default
    QVERIFY(mDeviceControl->mode() == Cxe::ImageMode);
}

void UnitTestCxeCameraDeviceControlSymbian::testSetMode()
{
    mDeviceControl->setMode(Cxe::VideoMode);

    QVERIFY(mDeviceControl->mode() == Cxe::VideoMode);
}

void UnitTestCxeCameraDeviceControlSymbian::testRelease()
{
    mDeviceControl->release();

    QVERIFY(mDeviceControl->state() == CxeCameraDeviceControl::Idle);
}

void UnitTestCxeCameraDeviceControlSymbian::testCameraIndex()
{
    QVERIFY(mDeviceControl->cameraIndex() == Cxe::PrimaryCameraIndex);
}

void UnitTestCxeCameraDeviceControlSymbian::testSwitchCamera()
{
    QVERIFY(mDeviceControl->cameraIndex() == Cxe::PrimaryCameraIndex);

    mDeviceControl->switchCamera(Cxe::SecondaryCameraIndex);

    // Reserve can last long and we have had hacks with extra delay.
    // Hence using quite large time out value.
    QVERIFY(CxeTestUtils::waitForState<CxeCameraDeviceControl>(
            *mDeviceControl, CxeCameraDeviceControl::Ready, 3000));

    QVERIFY(mDeviceControl->cameraIndex() == Cxe::SecondaryCameraIndex);
}

void UnitTestCxeCameraDeviceControlSymbian::testReserve()
{
    mDeviceControl->reserve();

    // Using quite large time out value. See testSwitchCamera() comments.
    QVERIFY(CxeTestUtils::waitForState<CxeCameraDeviceControl>(
            *mDeviceControl, CxeCameraDeviceControl::Ready, 3000));
}

// main() function
QTEST_MAIN(UnitTestCxeCameraDeviceControlSymbian);
